/****************************************************************************
 * arch/arm/src/stm32n6/stm32_xspi.c
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <sys/types.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <debug.h>

#include <arch/barriers.h>
#include <arch/board/board.h>

#include <nuttx/arch.h>
#include <nuttx/clock.h>
#include <nuttx/kmalloc.h>
#include <nuttx/mutex.h>
#include <nuttx/nuttx.h>
#include <nuttx/signal.h>
#include <nuttx/spi/qspi.h>

#include "arm_internal.h"
#include "stm32_gpio.h"
#include "stm32_xspi.h"
#include "hardware/stm32_xspi.h"
#include "hardware/stm32_rcc.h"
#include "chip.h"

#ifdef CONFIG_STM32N6_XSPI2

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* XSPI2 peripheral register base (non-secure) */

#define XSPI2_REG_BASE       STM32N6_XSPI2_BASE

/* XSPI2 memory-mapped base address */

#define XSPI2_MM_BASE        STM32N6_XSPI2_MM_BASE

/* XSPIM register base */

#define XSPIM_REG_BASE       STM32N6_XSPIM_BASE

/* Default clock: 200MHz HCLK */

#ifndef STM32N6_XSPI2_CLK_FREQUENCY
#  define STM32N6_XSPI2_CLK_FREQUENCY  STM32N6_HCLK_FREQUENCY
#endif

/* Default flash size: 32MB = 256Mbit -> 25 address bits -> DEVSIZE = 24 */

#ifndef CONFIG_STM32N6_XSPI2_FLASH_SIZE
#  define CONFIG_STM32N6_XSPI2_FLASH_SIZE  (32 * 1024 * 1024)
#endif

/* MX25UM25645G Octal DTR commands */

#define MX25UM_OCTAL_READ_DTR    0xEE  /* Octal I/O DTR Read */
#define MX25UM_OCTAL_PP_DTR      0x12  /* Octal Page Program DTR */
#define MX25UM_SECTOR_ERASE      0x21  /* Sector Erase (4KB) */
#define MX25UM_WRITE_ENABLE      0x06  /* Write Enable */
#define MX25UM_READ_STATUS       0x05  /* Read Status Register */
#define MX25UM_READ_CFG_REG2     0x71  /* Read Configuration Register 2 */
#define MX25UM_WRITE_CFG_REG2    0x72  /* Write Configuration Register 2 */
#define MX25UM_RST_ENABLE        0x66  /* Reset Enable */
#define MX25UM_RESET             0x99  /* Reset Memory */
#define MX25UM_RDID              0x9F  /* Read JEDEC ID */

/* DTR instruction encoding: high byte = cmd, low byte = ~cmd */

#define XSPI_DTR_INSTR(cmd)  (((uint16_t)(cmd) << 8) | (uint8_t)(~(cmd)))

/****************************************************************************
 * Private Types
 ****************************************************************************/

/* Physical link modes for the XSPI interface */

enum xspi_phylink_e
{
  XSPI_PHY_1S1S1S = 0,  /* Single SPI: 1-line instruction/address/data */
  XSPI_PHY_4S4S4S,      /* Quad SPI: 4-line instruction/address/data */
  XSPI_PHY_8D8D8D,      /* Octal DTR: 8-line everything, double rate */
};

/* The XSPI controller state */

struct stm32n6_xspidev_s
{
  struct qspi_dev_s  qspi;       /* Externally visible QSPI interface */
  uint32_t           base;       /* XSPI register base address */
  uint32_t           frequency;  /* Requested clock frequency */
  uint32_t           actual;     /* Actual clock frequency */
  uint8_t            mode;       /* SPI mode (0 or 3) */
  uint8_t            intf;       /* XSPI controller number */
  bool               initialized;/* TRUE: Controller has been initialized */
  mutex_t            lock;       /* Bus mutex */
  bool               memmap;     /* TRUE: In memory-mapped mode */
  enum xspi_phylink_e phylink;   /* Current physical link mode */
};

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

static inline uint32_t xspi_getreg(struct stm32n6_xspidev_s *priv,
                                   unsigned int offset);
static inline void xspi_putreg(struct stm32n6_xspidev_s *priv,
                               uint32_t value, unsigned int offset);

static void xspi_waitbusy(struct stm32n6_xspidev_s *priv);
static void xspi_abort(struct stm32n6_xspidev_s *priv);
static void xspi_clearflags(struct stm32n6_xspidev_s *priv);

/* QSPI operations */

static int      xspi_lock(struct qspi_dev_s *dev, bool lock);
static uint32_t xspi_setfrequency(struct qspi_dev_s *dev,
                                  uint32_t frequency);
static void     xspi_setmode(struct qspi_dev_s *dev,
                             enum qspi_mode_e mode);
static void     xspi_setbits(struct qspi_dev_s *dev, int nbits);
static int      xspi_command(struct qspi_dev_s *dev,
                             struct qspi_cmdinfo_s *cmdinfo);
static int      xspi_memory(struct qspi_dev_s *dev,
                            struct qspi_meminfo_s *meminfo);
static void    *xspi_alloc(struct qspi_dev_s *dev, size_t buflen);
static void     xspi_free(struct qspi_dev_s *dev, void *buffer);

/* Internal helpers */

static void xspi_config_phylink(struct stm32n6_xspidev_s *priv,
                                enum xspi_phylink_e phylink);
static int  xspi_hw_initialize(struct stm32n6_xspidev_s *priv);
static int  xspi_mx25um_init(struct stm32n6_xspidev_s *priv);

/****************************************************************************
 * Private Data
 ****************************************************************************/

static const struct qspi_ops_s g_xspi2ops =
{
  .lock              = xspi_lock,
  .setfrequency      = xspi_setfrequency,
  .setmode           = xspi_setmode,
  .setbits           = xspi_setbits,
  .command           = xspi_command,
  .memory            = xspi_memory,
  .alloc             = xspi_alloc,
  .free              = xspi_free,
};

static struct stm32n6_xspidev_s g_xspi2dev =
{
  .qspi              =
  {
    .ops             = &g_xspi2ops,
  },
  .base              = XSPI2_REG_BASE,
  .lock              = NXMUTEX_INITIALIZER,
  .intf              = 2,
};

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static inline uint32_t xspi_getreg(struct stm32n6_xspidev_s *priv,
                                   unsigned int offset)
{
  return getreg32(priv->base + offset);
}

static inline void xspi_putreg(struct stm32n6_xspidev_s *priv,
                               uint32_t value, unsigned int offset)
{
  putreg32(value, priv->base + offset);
}

static void xspi_waitbusy(struct stm32n6_xspidev_s *priv)
{
  while (xspi_getreg(priv, XSPI_SR_OFFSET) & XSPI_SR_BUSY)
    ;
}

static void xspi_abort(struct stm32n6_xspidev_s *priv)
{
  uint32_t regval;

  regval = xspi_getreg(priv, XSPI_CR_OFFSET);
  regval |= XSPI_CR_ABORT;
  xspi_putreg(priv, regval, XSPI_CR_OFFSET);
}

static void xspi_clearflags(struct stm32n6_xspidev_s *priv)
{
  xspi_putreg(priv, XSPI_FCR_CTEF | XSPI_FCR_CTCF |
              XSPI_FCR_CSMF | XSPI_FCR_CTOF, XSPI_FCR_OFFSET);
}

/****************************************************************************
 * Name: xspi_config_phylink
 *
 * Description:
 *   Configure the XSPI physical link mode (1S1S1S, 4S4S4S, 8D8D8D).
 *
 ****************************************************************************/

static void xspi_config_phylink(struct stm32n6_xspidev_s *priv,
                                enum xspi_phylink_e phylink)
{
  priv->phylink = phylink;
}

/****************************************************************************
 * Name: xspi_build_ccr
 *
 * Description:
 *   Build the CCR register value based on the current physical link mode
 *   and the command parameters.
 *
 ****************************************************************************/

static uint32_t xspi_build_ccr(struct stm32n6_xspidev_s *priv,
                               bool is_write, bool has_addr,
                               bool has_data, int addrlen)
{
  uint32_t ccr = 0;

  switch (priv->phylink)
    {
      case XSPI_PHY_1S1S1S:
        ccr |= XSPI_CCR_IMODE_1LINE;
        ccr |= XSPI_CCR_ISIZE_8BIT;
        if (has_addr)
          {
            ccr |= XSPI_CCR_ADMODE_1LINE;
            ccr |= (addrlen == 4) ? XSPI_CCR_ADSIZE_32BIT :
                    (addrlen == 3) ? XSPI_CCR_ADSIZE_24BIT :
                    (addrlen == 2) ? XSPI_CCR_ADSIZE_16BIT :
                                     XSPI_CCR_ADSIZE_8BIT;
          }

        if (has_data)
          {
            ccr |= XSPI_CCR_DMODE_1LINE;
          }

        break;

      case XSPI_PHY_4S4S4S:
        ccr |= XSPI_CCR_IMODE_4LINE;
        ccr |= XSPI_CCR_ISIZE_8BIT;
        if (has_addr)
          {
            ccr |= XSPI_CCR_ADMODE_4LINE;
            ccr |= XSPI_CCR_ADSIZE_24BIT;
          }

        if (has_data)
          {
            ccr |= XSPI_CCR_DMODE_4LINE;
          }

        break;

      case XSPI_PHY_8D8D8D:
        ccr |= XSPI_CCR_IMODE_8LINE;
        ccr |= XSPI_CCR_ISIZE_16BIT;  /* 16-bit instruction for DTR */
        ccr |= XSPI_CCR_IDTR;         /* Instruction DTR */
        if (has_addr)
          {
            ccr |= XSPI_CCR_ADMODE_8LINE;
            ccr |= XSPI_CCR_ADSIZE_32BIT;  /* 32-bit addr for DTR */
            ccr |= XSPI_CCR_ADDTR;         /* Address DTR */
          }

        if (has_data)
          {
            ccr |= XSPI_CCR_DMODE_8LINE;
            ccr |= XSPI_CCR_DDTR;         /* Data DTR */
            ccr |= XSPI_CCR_DQSE;         /* DQS enable */
          }

        break;

      default:
        break;
    }

  /* Set functional mode */

  if (is_write || !has_data)
    {
      ccr |= XSPI_CCR_FMODE_INDWR;
    }
  else
    {
      ccr |= XSPI_CCR_FMODE_INDRD;
    }

  return ccr;
}

/****************************************************************************
 * Name: xspi_encode_cmd
 *
 * Description:
 *   Encode the command value. In Octal DTR mode, the instruction is
 *   16 bits: high byte = command, low byte = complement.
 *
 ****************************************************************************/

static uint32_t xspi_encode_cmd(struct stm32n6_xspidev_s *priv, uint16_t cmd)
{
  if (priv->phylink == XSPI_PHY_8D8D8D)
    {
      return XSPI_DTR_INSTR(cmd & 0xff);
    }

  return cmd & 0xff;
}

/****************************************************************************
 * Name: xspi_command_send_data
 *
 * Description:
 *   Send a command with optional data (no address).
 *
 ****************************************************************************/

static int xspi_command_send_data(struct stm32n6_xspidev_s *priv,
                                  uint8_t cmd, const uint8_t *data,
                                  uint16_t datalen)
{
  uint32_t ccr;
  uint32_t regval;
  int i;

  xspi_abort(priv);
  xspi_waitbusy(priv);
  xspi_clearflags(priv);

  /* Build CCR */

  ccr = xspi_build_ccr(priv, true, false, datalen > 0, 0);
  xspi_putreg(priv, ccr, XSPI_CCR_OFFSET);

  /* Set data length if any */

  if (datalen > 0)
    {
      xspi_putreg(priv, datalen - 1, XSPI_DLR_OFFSET);
    }

  /* Write instruction to IR register */

  xspi_putreg(priv, xspi_encode_cmd(priv, cmd), XSPI_IR_OFFSET);

  /* Start transfer by setting the address register (triggers the command) */

  xspi_putreg(priv, 0, XSPI_AR_OFFSET);

  /* Write data if any */

  if (datalen > 0 && data != NULL)
    {
      for (i = 0; i < datalen; i++)
        {
          /* Wait for FIFO threshold */

          while (!(xspi_getreg(priv, XSPI_SR_OFFSET) & XSPI_SR_FTF))
            ;

          putreg8(data[i], priv->base + XSPI_DR_OFFSET);
        }
    }

  /* Wait for transfer complete */

  while (!(xspi_getreg(priv, XSPI_SR_OFFSET) & XSPI_SR_TCF))
    ;

  xspi_putreg(priv, XSPI_FCR_CTCF, XSPI_FCR_OFFSET);
  xspi_abort(priv);
  xspi_waitbusy(priv);

  return OK;
}

/****************************************************************************
 * Name: xspi_command_read
 *
 * Description:
 *   Send a command and read data (no address).
 *
 ****************************************************************************/

static int xspi_command_read(struct stm32n6_xspidev_s *priv,
                             uint8_t cmd, uint8_t *data,
                             uint16_t datalen)
{
  uint32_t ccr;
  int i;

  xspi_abort(priv);
  xspi_waitbusy(priv);
  xspi_clearflags(priv);

  /* Build CCR */

  ccr = xspi_build_ccr(priv, false, false, datalen > 0, 0);
  xspi_putreg(priv, ccr, XSPI_CCR_OFFSET);

  /* Set data length if any */

  if (datalen > 0)
    {
      xspi_putreg(priv, datalen - 1, XSPI_DLR_OFFSET);
    }

  /* Write instruction to IR register */

  xspi_putreg(priv, xspi_encode_cmd(priv, cmd), XSPI_IR_OFFSET);

  /* Start transfer */

  xspi_putreg(priv, 0, XSPI_AR_OFFSET);

  /* Read data if any */

  if (datalen > 0 && data != NULL)
    {
      for (i = 0; i < datalen; i++)
        {
          /* Wait for FIFO threshold or transfer complete */

          while (!(xspi_getreg(priv, XSPI_SR_OFFSET) &
                   (XSPI_SR_FTF | XSPI_SR_TCF)))
            ;

          if (xspi_getreg(priv, XSPI_SR_OFFSET) & XSPI_SR_TCF)
            {
              /* Read remaining data */

              while (xspi_getreg(priv, XSPI_SR_OFFSET) &
                     XSPI_SR_FLEVEL_MASK)
                {
                  data[i] = getreg8(priv->base + XSPI_DR_OFFSET);
                  i++;
                  if (i >= datalen)
                    {
                      goto done;
                    }
                }

              break;
            }

          data[i] = getreg8(priv->base + XSPI_DR_OFFSET);
        }
    }

done:
  while (!(xspi_getreg(priv, XSPI_SR_OFFSET) & XSPI_SR_TCF))
    ;

  xspi_putreg(priv, XSPI_FCR_CTCF, XSPI_FCR_OFFSET);
  xspi_abort(priv);
  xspi_waitbusy(priv);

  return OK;
}

/****************************************************************************
 * Name: xspi_command_send_addr_data
 *
 * Description:
 *   Send a command with address and optional write data.
 *
 ****************************************************************************/

static int xspi_command_send_addr_data(struct stm32n6_xspidev_s *priv,
                                       uint8_t cmd, uint32_t addr,
                                       const uint8_t *data,
                                       uint16_t datalen, int addrlen)
{
  uint32_t ccr;
  int i;

  xspi_abort(priv);
  xspi_waitbusy(priv);
  xspi_clearflags(priv);

  /* Build CCR */

  ccr = xspi_build_ccr(priv, true, true, datalen > 0, addrlen);
  xspi_putreg(priv, ccr, XSPI_CCR_OFFSET);

  /* Set data length if any */

  if (datalen > 0)
    {
      xspi_putreg(priv, datalen - 1, XSPI_DLR_OFFSET);
    }

  /* Write instruction to IR register */

  xspi_putreg(priv, xspi_encode_cmd(priv, cmd), XSPI_IR_OFFSET);

  /* Write address - triggers the transfer */

  xspi_putreg(priv, addr, XSPI_AR_OFFSET);

  /* Write data if any */

  if (datalen > 0 && data != NULL)
    {
      for (i = 0; i < datalen; i++)
        {
          while (!(xspi_getreg(priv, XSPI_SR_OFFSET) & XSPI_SR_FTF))
            ;

          putreg8(data[i], priv->base + XSPI_DR_OFFSET);
        }
    }

  /* Wait for transfer complete */

  while (!(xspi_getreg(priv, XSPI_SR_OFFSET) & XSPI_SR_TCF))
    ;

  xspi_putreg(priv, XSPI_FCR_CTCF, XSPI_FCR_OFFSET);
  xspi_abort(priv);
  xspi_waitbusy(priv);

  return OK;
}

/****************************************************************************
 * Name: xspi_command_read_addr_data
 *
 * Description:
 *   Send a command with address and read data.
 *
 ****************************************************************************/

static int xspi_command_read_addr_data(struct stm32n6_xspidev_s *priv,
                                       uint8_t cmd, uint32_t addr,
                                       uint8_t *data,
                                       uint16_t datalen, int addrlen,
                                       uint8_t dummies)
{
  uint32_t ccr;
  int i;

  xspi_abort(priv);
  xspi_waitbusy(priv);
  xspi_clearflags(priv);

  /* Build CCR */

  ccr = xspi_build_ccr(priv, false, true, datalen > 0, addrlen);
  xspi_putreg(priv, ccr, XSPI_CCR_OFFSET);

  /* Set dummy cycles in TCR */

  xspi_putreg(priv, (dummies & 0x1f) << XSPI_TCR_DCYC_SHIFT,
              XSPI_TCR_OFFSET);

  /* Set data length */

  if (datalen > 0)
    {
      xspi_putreg(priv, datalen - 1, XSPI_DLR_OFFSET);
    }

  /* Write instruction to IR register */

  xspi_putreg(priv, xspi_encode_cmd(priv, cmd), XSPI_IR_OFFSET);

  /* Write address - triggers the transfer */

  xspi_putreg(priv, addr, XSPI_AR_OFFSET);

  /* Read data */

  if (datalen > 0 && data != NULL)
    {
      for (i = 0; i < datalen; i++)
        {
          while (!(xspi_getreg(priv, XSPI_SR_OFFSET) &
                   (XSPI_SR_FTF | XSPI_SR_TCF)))
            ;

          if (xspi_getreg(priv, XSPI_SR_OFFSET) & XSPI_SR_TCF)
            {
              while (xspi_getreg(priv, XSPI_SR_OFFSET) &
                     XSPI_SR_FLEVEL_MASK)
                {
                  data[i] = getreg8(priv->base + XSPI_DR_OFFSET);
                  i++;
                  if (i >= datalen)
                    {
                      goto done;
                    }
                }

              break;
            }

          data[i] = getreg8(priv->base + XSPI_DR_OFFSET);
        }
    }

done:
  while (!(xspi_getreg(priv, XSPI_SR_OFFSET) & XSPI_SR_TCF))
    ;

  xspi_putreg(priv, XSPI_FCR_CTCF, XSPI_FCR_OFFSET);
  xspi_abort(priv);
  xspi_waitbusy(priv);

  return OK;
}

/****************************************************************************
 * Name: xspi_memory_read
 *
 * Description:
 *   Perform a memory read using XSPI.
 *
 ****************************************************************************/

static int xspi_memory_read(struct stm32n6_xspidev_s *priv,
                            struct qspi_meminfo_s *meminfo)
{
  uint32_t ccr;
  uint32_t dummies;
  int i;

  xspi_abort(priv);
  xspi_waitbusy(priv);
  xspi_clearflags(priv);

  /* Build CCR for read */

  ccr = xspi_build_ccr(priv, false, true, true, meminfo->addrlen);

  /* Set functional mode to indirect read */

  ccr &= ~XSPI_CCR_FMODE_MASK;
  ccr |= XSPI_CCR_FMODE_INDRD;

  xspi_putreg(priv, ccr, XSPI_CCR_OFFSET);

  /* Set dummy cycles */

  dummies = meminfo->dummies;
  xspi_putreg(priv, (dummies & 0x1f) << XSPI_TCR_DCYC_SHIFT,
              XSPI_TCR_OFFSET);

  /* Set data length */

  xspi_putreg(priv, meminfo->buflen - 1, XSPI_DLR_OFFSET);

  /* Write instruction */

  xspi_putreg(priv, xspi_encode_cmd(priv, meminfo->cmd), XSPI_IR_OFFSET);

  /* Write address - triggers the transfer */

  xspi_putreg(priv, meminfo->addr, XSPI_AR_OFFSET);

  /* Read data byte by byte */

  for (i = 0; i < (int)meminfo->buflen; i++)
    {
      while (!(xspi_getreg(priv, XSPI_SR_OFFSET) &
               (XSPI_SR_FTF | XSPI_SR_TCF)))
        ;

      if (xspi_getreg(priv, XSPI_SR_OFFSET) & XSPI_SR_TCF)
        {
          while (xspi_getreg(priv, XSPI_SR_OFFSET) & XSPI_SR_FLEVEL_MASK)
            {
              ((uint8_t *)meminfo->buffer)[i] =
                getreg8(priv->base + XSPI_DR_OFFSET);
              i++;
              if (i >= (int)meminfo->buflen)
                {
                  goto done;
                }
            }

          break;
        }

      ((uint8_t *)meminfo->buffer)[i] = getreg8(priv->base + XSPI_DR_OFFSET);
    }

done:
  while (!(xspi_getreg(priv, XSPI_SR_OFFSET) & XSPI_SR_TCF))
    ;

  xspi_putreg(priv, XSPI_FCR_CTCF, XSPI_FCR_OFFSET);
  xspi_abort(priv);
  xspi_waitbusy(priv);

  return OK;
}

/****************************************************************************
 * Name: xspi_memory_write
 *
 * Description:
 *   Perform a memory write using XSPI.
 *
 ****************************************************************************/

static int xspi_memory_write(struct stm32n6_xspidev_s *priv,
                             struct qspi_meminfo_s *meminfo)
{
  uint32_t ccr;
  int i;

  xspi_abort(priv);
  xspi_waitbusy(priv);
  xspi_clearflags(priv);

  /* Build CCR for write */

  ccr = xspi_build_ccr(priv, true, true, true, meminfo->addrlen);
  xspi_putreg(priv, ccr, XSPI_CCR_OFFSET);

  /* Set data length */

  xspi_putreg(priv, meminfo->buflen - 1, XSPI_DLR_OFFSET);

  /* Write instruction */

  xspi_putreg(priv, xspi_encode_cmd(priv, meminfo->cmd), XSPI_IR_OFFSET);

  /* Write address - triggers the transfer */

  xspi_putreg(priv, meminfo->addr, XSPI_AR_OFFSET);

  /* Write data byte by byte */

  for (i = 0; i < (int)meminfo->buflen; i++)
    {
      while (!(xspi_getreg(priv, XSPI_SR_OFFSET) & XSPI_SR_FTF))
        ;

      putreg8(((const uint8_t *)meminfo->buffer)[i],
              priv->base + XSPI_DR_OFFSET);
    }

  /* Wait for transfer complete */

  while (!(xspi_getreg(priv, XSPI_SR_OFFSET) & XSPI_SR_TCF))
    ;

  xspi_putreg(priv, XSPI_FCR_CTCF, XSPI_FCR_OFFSET);
  xspi_abort(priv);
  xspi_waitbusy(priv);

  return OK;
}

/****************************************************************************
 * Name: xspi_wait_write_complete
 *
 * Description:
 *   Wait for a write/erase operation to complete by polling the status
 *   register of the flash.
 *
 ****************************************************************************/

static int xspi_wait_write_complete(struct stm32n6_xspidev_s *priv,
                                    uint32_t timeout_ms)
{
  uint8_t status;
  uint32_t elapsed = 0;

  while (elapsed < timeout_ms)
    {
      /* Read status register using autopolling */

      xspi_abort(priv);
      xspi_waitbusy(priv);
      xspi_clearflags(priv);

      /* Configure autopolling on status register WIP bit */

      /* Use manual polling for simplicity */

      xspi_command_read(priv, MX25UM_READ_STATUS, &status, 1);

      if (!(status & 0x01))  /* WIP bit = 0 means write complete */
        {
          return OK;
        }

      nxsig_usleep(1000);  /* 1ms */
      elapsed++;
    }

  spierr("ERROR: Write operation timeout\n");
  return -ETIMEDOUT;
}

/****************************************************************************
 * QSPI Operations
 ****************************************************************************/

static int xspi_lock(struct qspi_dev_s *dev, bool lock)
{
  struct stm32n6_xspidev_s *priv = (struct stm32n6_xspidev_s *)dev;
  int ret;

  if (lock)
    {
      ret = nxmutex_lock(&priv->lock);
    }
  else
    {
      ret = nxmutex_unlock(&priv->lock);
    }

  return ret;
}

static uint32_t xspi_setfrequency(struct qspi_dev_s *dev,
                                  uint32_t frequency)
{
  struct stm32n6_xspidev_s *priv = (struct stm32n6_xspidev_s *)dev;
  uint32_t prescaler;
  uint32_t actual;
  uint32_t regval;

  if (priv->memmap)
    {
      return 0;
    }

  spiinfo("frequency=%" PRId32 "\n", frequency);

  if (priv->frequency == frequency)
    {
      return priv->actual;
    }

  /* Calculate prescaler: prescaler = CLK / frequency */

  prescaler = (STM32N6_XSPI2_CLK_FREQUENCY + frequency - 1) / frequency;

  if (prescaler < 1)
    {
      prescaler = 1;
    }
  else if (prescaler > 256)
    {
      prescaler = 256;
    }

  /* Set prescaler in DCR2 register */

  regval = xspi_getreg(priv, XSPI_DCR2_OFFSET);
  regval &= ~XSPI_DCR2_PRESCALER_MASK;
  regval |= ((prescaler - 1) << XSPI_DCR2_PRESCALER_SHIFT);
  xspi_putreg(priv, regval, XSPI_DCR2_OFFSET);

  actual = STM32N6_XSPI2_CLK_FREQUENCY / prescaler;

  priv->frequency = frequency;
  priv->actual = actual;

  spiinfo("prescaler=%" PRId32 " actual=%" PRId32 "\n", prescaler, actual);
  return actual;
}

static void xspi_setmode(struct qspi_dev_s *dev, enum qspi_mode_e mode)
{
  struct stm32n6_xspidev_s *priv = (struct stm32n6_xspidev_s *)dev;
  uint32_t regval;

  if (priv->memmap)
    {
      return;
    }

  spiinfo("mode=%d\n", mode);

  if (mode != priv->mode)
    {
      regval = xspi_getreg(priv, XSPI_DCR1_OFFSET);
      regval &= ~XSPI_DCR1_CKMODE;

      switch (mode)
        {
          case QSPIDEV_MODE0:
            break;

          case QSPIDEV_MODE3:
            regval |= XSPI_DCR1_CKMODE;
            break;

          default:
            spiinfo("unsupported mode=%d\n", mode);
            return;
        }

      xspi_putreg(priv, regval, XSPI_DCR1_OFFSET);
      priv->mode = mode;
    }
}

static void xspi_setbits(struct qspi_dev_s *dev, int nbits)
{
  /* XSPI always operates in 8-bit mode */

  if (8 != nbits)
    {
      spiinfo("unsupported nbits=%d\n", nbits);
    }
}

static int xspi_command(struct qspi_dev_s *dev,
                        struct qspi_cmdinfo_s *cmdinfo)
{
  struct stm32n6_xspidev_s *priv = (struct stm32n6_xspidev_s *)dev;

  if (priv->memmap)
    {
      return -EBUSY;
    }

  spiinfo("cmd=%04x flags=%02x\n", cmdinfo->cmd, cmdinfo->flags);

  if (QSPICMD_ISADDRESS(cmdinfo->flags))
    {
      if (QSPICMD_ISDATA(cmdinfo->flags))
        {
          if (QSPICMD_ISREAD(cmdinfo->flags))
            {
              return xspi_command_read_addr_data(priv, cmdinfo->cmd,
                       cmdinfo->addr, cmdinfo->buffer,
                       cmdinfo->buflen, cmdinfo->addrlen,
                       cmdinfo->dummies);
            }
          else
            {
              return xspi_command_send_addr_data(priv, cmdinfo->cmd,
                       cmdinfo->addr, cmdinfo->buffer,
                       cmdinfo->buflen, cmdinfo->addrlen);
            }
        }
      else
        {
          /* Command + address only */

          uint32_t ccr;

          xspi_abort(priv);
          xspi_waitbusy(priv);
          xspi_clearflags(priv);

          ccr = xspi_build_ccr(priv, true, true, false,
                               cmdinfo->addrlen);
          xspi_putreg(priv, ccr, XSPI_CCR_OFFSET);
          xspi_putreg(priv, xspi_encode_cmd(priv, cmdinfo->cmd),
                      XSPI_IR_OFFSET);
          xspi_putreg(priv, cmdinfo->addr, XSPI_AR_OFFSET);

          while (!(xspi_getreg(priv, XSPI_SR_OFFSET) & XSPI_SR_TCF))
            ;

          xspi_putreg(priv, XSPI_FCR_CTCF, XSPI_FCR_OFFSET);
          xspi_abort(priv);
          xspi_waitbusy(priv);

          return OK;
        }
    }
  else
    {
      if (QSPICMD_ISDATA(cmdinfo->flags))
        {
          if (QSPICMD_ISREAD(cmdinfo->flags))
            {
              return xspi_command_read(priv, cmdinfo->cmd,
                       cmdinfo->buffer, cmdinfo->buflen);
            }
          else
            {
              return xspi_command_send_data(priv, cmdinfo->cmd,
                       cmdinfo->buffer, cmdinfo->buflen);
            }
        }
      else
        {
          return xspi_command_send_data(priv, cmdinfo->cmd, NULL, 0);
        }
    }

  return OK;
}

static int xspi_memory(struct qspi_dev_s *dev,
                       struct qspi_meminfo_s *meminfo)
{
  struct stm32n6_xspidev_s *priv = (struct stm32n6_xspidev_s *)dev;

  if (priv->memmap)
    {
      return -EBUSY;
    }

  spiinfo("cmd=%04x addr=%08lx len=%lu\n",
          meminfo->cmd, (unsigned long)meminfo->addr,
          (unsigned long)meminfo->buflen);

  if (QSPIMEM_ISREAD(meminfo->flags))
    {
      return xspi_memory_read(priv, meminfo);
    }
  else
    {
      return xspi_memory_write(priv, meminfo);
    }
}

static void *xspi_alloc(struct qspi_dev_s *dev, size_t buflen)
{
  return kmm_malloc(ALIGN_UP(buflen, 4));
}

static void xspi_free(struct qspi_dev_s *dev, void *buffer)
{
  if (buffer)
    {
      kmm_free(buffer);
    }
}

/****************************************************************************
 * Name: xspi_hw_initialize
 *
 * Description:
 *   Initialize the XSPI2 peripheral hardware.
 *
 ****************************************************************************/

static int xspi_hw_initialize(struct stm32n6_xspidev_s *priv)
{
  uint32_t regval;

  /* Enable XSPI2 and XSPIM clocks */

  rcc_enable_ahb5_clock(RCC_AHB5ENR_XSPI2EN | RCC_AHB5ENR_XSPIMEN);

  /* Reset XSPI2 */

  rcc_set_ahb5_reset(RCC_AHB5RSTR_XSPI2RST);
  nxsig_usleep(10);
  rcc_clear_ahb5_reset(RCC_AHB5RSTR_XSPI2RST);
  nxsig_usleep(10);

  /* Disable XSPI */

  xspi_putreg(priv, 0, XSPI_CR_OFFSET);
  xspi_waitbusy(priv);

  /* Configure DCR1: Macronix memory type, 32MB (25 bits -> DEVSIZE=24),
   * CS high time = 1 cycle, Clock mode 0
   */

  regval = XSPI_DCR1_MTYP_MACRONIX |
           (24 << XSPI_DCR1_DEVSIZE_SHIFT) |
           (1 << XSPI_DCR1_CSHT_SHIFT);
  xspi_putreg(priv, regval, XSPI_DCR1_OFFSET);

  /* Configure DCR2: prescaler = 0 (div by 1) initially */

  xspi_putreg(priv, 0, XSPI_DCR2_OFFSET);

  /* Configure CR: FIFO threshold = 8, enable */

  regval = (8 << XSPI_CR_FTHRES_SHIFT) | XSPI_CR_EN;
  xspi_putreg(priv, regval, XSPI_CR_OFFSET);

  /* Configure XSPIM for XSPI2 with NCS2 override */

  regval = XSPIM_CR_MUXEN | XSPIM_CR_CSSEL_OVR_NCS2 |
           XSPIM_CR_REQ2ACK_TIME(3);
  putreg32(regval, XSPIM_REG_BASE + XSPIM_CR_OFFSET);

  /* Set initial timing: sample shift half cycle, DHQC for DTR */

  regval = XSPI_TCR_SSHIFT | XSPI_TCR_DHQC;
  xspi_putreg(priv, regval, XSPI_TCR_OFFSET);

  priv->initialized = true;
  priv->memmap = false;
  priv->phylink = XSPI_PHY_1S1S1S;

  return OK;
}

/****************************************************************************
 * Name: xspi_mx25um_init
 *
 * Description:
 *   Initialize MX25UM25645G NOR Flash into Octal DTR mode.
 *
 ****************************************************************************/

static int xspi_mx25um_init(struct stm32n6_xspidev_s *priv)
{
  uint8_t data;
  uint8_t jedecid[3];

  /* Start in 1S1S1S mode */

  priv->phylink = XSPI_PHY_1S1S1S;

  /* Reset the flash in all modes */

  xspi_config_phylink(priv, XSPI_PHY_1S1S1S);
  xspi_command_send_data(priv, MX25UM_RST_ENABLE, NULL, 0);
  xspi_command_send_data(priv, MX25UM_RESET, NULL, 0);

  xspi_config_phylink(priv, XSPI_PHY_4S4S4S);
  xspi_command_send_data(priv, MX25UM_RST_ENABLE, NULL, 0);
  xspi_command_send_data(priv, MX25UM_RESET, NULL, 0);

  xspi_config_phylink(priv, XSPI_PHY_8D8D8D);
  xspi_command_send_data(priv, MX25UM_RST_ENABLE, NULL, 0);
  xspi_command_send_data(priv, MX25UM_RESET, NULL, 0);

  /* Wait for reset */

  nxsig_usleep(10000);

  /* Switch back to 1S1S1S for initial configuration */

  xspi_config_phylink(priv, XSPI_PHY_1S1S1S);

  /* Read JEDEC ID */

  xspi_command_read(priv, MX25UM_RDID, jedecid, 3);
  spiinfo("JEDEC ID: %02x %02x %02x\n", jedecid[0], jedecid[1], jedecid[2]);

  /* Check Macronix manufacturer ID (0xC2) */

  if (jedecid[0] != 0xc2)
    {
      spierr("ERROR: Not a Macronix flash (ID=%02x)\n", jedecid[0]);
      return -ENODEV;
    }

  /* Read Configuration Register 2 (CR2) at address 0x00000000
   * to check if already in Octal mode
   */

  xspi_command_read_addr_data(priv, MX25UM_READ_CFG_REG2, 0x00000000,
                              &data, 1, 3, 0);

  spiinfo("CR2[0]=%02x\n", data);

  /* Check if already in Octal DTR mode (bits [1:0] == 0b10) */

  if ((data & 0x03) != 0x02)
    {
      /* Need to enable Octal DTR mode */

      /* Write Enable */

      xspi_command_send_data(priv, MX25UM_WRITE_ENABLE, NULL, 0);

      /* Write CR2[0] = 0x02 to enable Octal DTR mode */

      data = 0x02;
      xspi_command_send_addr_data(priv, MX25UM_WRITE_CFG_REG2, 0x00000000,
                                  &data, 1, 3);

      /* Switch to Octal DTR mode */

      xspi_config_phylink(priv, XSPI_PHY_8D8D8D);
    }
  else
    {
      /* Already in Octal DTR mode */

      xspi_config_phylink(priv, XSPI_PHY_8D8D8D);
    }

  spiinfo("MX25UM25645G initialized in Octal DTR mode\n");

  return OK;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: stm32n6_xspi_initialize
 *
 * Description:
 *   Initialize the XSPI2 peripheral and return a QSPI device.
 *
 ****************************************************************************/

FAR struct qspi_dev_s *stm32n6_xspi_initialize(int intf)
{
  struct stm32n6_xspidev_s *priv;
  int ret;

  spiinfo("intf=%d\n", intf);

  /* Only XSPI2 is supported */

  if (intf != 2)
    {
      spierr("ERROR: Only XSPI2 is supported (intf=%d)\n", intf);
      return NULL;
    }

  priv = &g_xspi2dev;

  /* Configure GPIO pins for XSPI2 */

#ifdef GPIO_XSPI2_CLK
  stm32_configgpio(GPIO_XSPI2_CLK);
#endif
#ifdef GPIO_XSPI2_NCS
  stm32_configgpio(GPIO_XSPI2_NCS);
#endif
#ifdef GPIO_XSPI2_DQS
  stm32_configgpio(GPIO_XSPI2_DQS);
#endif
#ifdef GPIO_XSPI2_D0
  stm32_configgpio(GPIO_XSPI2_D0);
  stm32_configgpio(GPIO_XSPI2_D1);
  stm32_configgpio(GPIO_XSPI2_D2);
  stm32_configgpio(GPIO_XSPI2_D3);
  stm32_configgpio(GPIO_XSPI2_D4);
  stm32_configgpio(GPIO_XSPI2_D5);
  stm32_configgpio(GPIO_XSPI2_D6);
  stm32_configgpio(GPIO_XSPI2_D7);
#endif

  /* Has the XSPI hardware been initialized? */

  if (!priv->initialized)
    {
      /* Initialize hardware */

      ret = xspi_hw_initialize(priv);
      if (ret < 0)
        {
          spierr("ERROR: Failed to initialize XSPI hardware\n");
          return NULL;
        }

      /* Initialize MX25UM25645G flash into Octal DTR mode */

      ret = xspi_mx25um_init(priv);
      if (ret < 0)
        {
          spierr("ERROR: Failed to initialize MX25UM25645G\n");
          return NULL;
        }

      /* Set default frequency: 50MHz */

      xspi_setfrequency(&priv->qspi, CONFIG_STM32N6_XSPI2_FREQUENCY);
    }

  return &priv->qspi;
}

/****************************************************************************
 * Name: stm32n6_xspi_enter_memorymapped
 *
 * Description:
 *   Enter memory-mapped mode for XSPI2.
 *
 ****************************************************************************/

void stm32n6_xspi_enter_memorymapped(void)
{
  struct stm32n6_xspidev_s *priv = &g_xspi2dev;
  uint32_t ccr;
  uint32_t regval;

  if (!priv->initialized || priv->memmap)
    {
      return;
    }

  nxmutex_lock(&priv->lock);

  /* Abort anything in progress */

  xspi_abort(priv);
  xspi_waitbusy(priv);

  /* Configure read command (0xEE with 20 dummy cycles) for memory-mapped */

  ccr = xspi_build_ccr(priv, false, true, true, 4);
  ccr &= ~XSPI_CCR_FMODE_MASK;
  ccr |= XSPI_CCR_FMODE_INDRD;
  xspi_putreg(priv, ccr, XSPI_CCR_OFFSET);

  /* Set dummy cycles for read: 20 */

  regval = (20 << XSPI_TCR_DCYC_SHIFT) | XSPI_TCR_SSHIFT | XSPI_TCR_DHQC;
  xspi_putreg(priv, regval, XSPI_TCR_OFFSET);

  /* Write read instruction (DTR encoded) */

  xspi_putreg(priv, XSPI_DTR_INSTR(MX25UM_OCTAL_READ_DTR),
              XSPI_IR_OFFSET);

  /* Configure write command (0x12) for memory-mapped */

  ccr = xspi_build_ccr(priv, true, true, true, 4);
  ccr &= ~XSPI_CCR_FMODE_MASK;
  ccr |= XSPI_CCR_FMODE_INDWR;
  xspi_putreg(priv, ccr, XSPI_WCCR_OFFSET);

  /* Write instruction for write path */

  xspi_putreg(priv, XSPI_DTR_INSTR(MX25UM_OCTAL_PP_DTR), XSPI_WIR_OFFSET);

  /* Set write timing (no dummy cycles for write) */

  regval = XSPI_TCR_SSHIFT | XSPI_TCR_DHQC;
  xspi_putreg(priv, regval, XSPI_WTCR_OFFSET);

  /* Now set the main CCR to memory-mapped mode */

  ccr = xspi_build_ccr(priv, false, true, true, 4);
  ccr &= ~XSPI_CCR_FMODE_MASK;
  ccr |= XSPI_CCR_FMODE_MMAP;
  xspi_putreg(priv, ccr, XSPI_CCR_OFFSET);

  /* Set dummy cycles for read in TCR */

  regval = (20 << XSPI_TCR_DCYC_SHIFT) | XSPI_TCR_SSHIFT | XSPI_TCR_DHQC;
  xspi_putreg(priv, regval, XSPI_TCR_OFFSET);

  /* Write read instruction */

  xspi_putreg(priv, XSPI_DTR_INSTR(MX25UM_OCTAL_READ_DTR),
              XSPI_IR_OFFSET);

  /* Disable timeout counter */

  regval = xspi_getreg(priv, XSPI_CR_OFFSET);
  regval &= ~XSPI_CR_TCEN;
  xspi_putreg(priv, regval, XSPI_CR_OFFSET);

  priv->memmap = true;

  spiinfo("Entered memory-mapped mode\n");

  nxmutex_unlock(&priv->lock);
}

/****************************************************************************
 * Name: stm32n6_xspi_exit_memorymapped
 *
 * Description:
 *   Exit memory-mapped mode for XSPI2.
 *
 ****************************************************************************/

void stm32n6_xspi_exit_memorymapped(void)
{
  struct stm32n6_xspidev_s *priv = &g_xspi2dev;

  if (!priv->initialized || !priv->memmap)
    {
      return;
    }

  nxmutex_lock(&priv->lock);

  UP_DSB();
  xspi_abort(priv);
  priv->memmap = false;

  spiinfo("Exited memory-mapped mode\n");

  nxmutex_unlock(&priv->lock);
}

#endif /* CONFIG_STM32N6_XSPI2 */
