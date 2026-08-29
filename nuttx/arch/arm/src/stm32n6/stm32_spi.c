/****************************************************************************
 * arch/arm/src/stm32n6/stm32_spi.c
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
 * The external functions, stm32_spi1/2/3select and stm32_spi1/2/3status
 * must be provided by board-specific logic.  They are implementations of the
 * select and status methods of the SPI interface defined by struct spi_ops_s
 * (see include/nuttx/spi/spi.h). All other methods (including
 * stm32_spibus_initialize()) are provided by common STM32 logic.  To use
 * this common SPI logic on your board:
 *
 *  1. Provide logic in stm32_board_initialize() to configure SPI chip
 *     select pins.
 *  2. Provide stm32_spi1/2/3select() and stm32_spi1/2/3status()
 *     functions in your board-specific logic.  These functions will perform
 *     chip selection and status operations using GPIOs in the way your board
 *     is configured.
 *  3. Add a calls to stm32_spibus_initialize() in your low level
 *     application initialization logic
 *  4. The handle returned by stm32_spibus_initialize() may then be used to
 *     bind the SPI driver to higher level logic (e.g., calling
 *     mmcsd_spislotinitialize(), for example, will bind the SPI driver to
 *     the SPI MMC/SD driver).
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <sys/types.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <semaphore.h>
#include <assert.h>
#include <errno.h>
#include <debug.h>

#include <nuttx/irq.h>
#include <nuttx/arch.h>
#include <nuttx/mutex.h>
#include <nuttx/spi/spi.h>

#include "arm_internal.h"
#include "chip.h"
#include "stm32_gpio.h"
#include "hardware/stm32_gpio.h"
#include "stm32_spi.h"
#include "hardware/stm32_spi.h"
#include "hardware/stm32_rcc.h"
#include "hardware/stm32_nvic.h"
#include <nuttx/spinlock.h>

#include <arch/board/board.h>

#if defined(CONFIG_STM32N6_SPI1) || defined(CONFIG_STM32N6_SPI2) || \
    defined(CONFIG_STM32N6_SPI3)

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* Configuration ************************************************************/

/* SPI interrupts */

#ifdef CONFIG_STM32N6_SPI_INTERRUPTS
#  error "Interrupt driven SPI not yet supported"
#endif

/* Can't have both interrupt driven SPI and SPI DMA */

#if defined(CONFIG_STM32N6_SPI_INTERRUPTS) && defined(CONFIG_STM32N6_SPI_DMA)
#  error "Cannot enable both interrupt mode and DMA mode for SPI"
#endif

/* SPI DMA priority */

#ifdef CONFIG_STM32N6_SPI_DMA
#  if defined(CONFIG_SPI_DMAPRIO)
#    define SPI_DMA_PRIO  CONFIG_SPI_DMAPRIO
#  else
#    define SPI_DMA_PRIO  0
#  endif
#endif

/****************************************************************************
 * Private Types
 ****************************************************************************/

struct stm32_spidev_s
{
  struct spi_dev_s spidev;       /* Externally visible part of the SPI interface */
  uint32_t         spibase;      /* SPIn base address */
  uint32_t         spiclock;     /* Clocking for the SPI module */
#ifdef CONFIG_STM32N6_SPI_INTERRUPTS
  uint8_t          spiirq;       /* SPI IRQ number */
#endif
#ifdef CONFIG_STM32N6_SPI_DMA
  volatile uint8_t rxresult;     /* Result of the RX DMA */
  volatile uint8_t txresult;     /* Result of the TX DMA */
  uint16_t         rxch;         /* The RX DMA channel number */
  uint16_t         txch;         /* The TX DMA channel number */
  DMA_HANDLE       rxdma;        /* DMA channel handle for RX transfers */
  DMA_HANDLE       txdma;        /* DMA channel handle for TX transfers */
  sem_t            rxsem;        /* Wait for RX DMA to complete */
  sem_t            txsem;        /* Wait for TX DMA to complete */
#endif
  bool             initialized;  /* Has SPI interface been initialized */
  mutex_t          lock;         /* Held while chip is selected for mutual exclusion */
  uint32_t         frequency;    /* Requested clock frequency */
  uint32_t         actual;       /* Actual clock frequency */
  uint8_t          nbits;        /* Width of word in bits (4 through 16) */
  uint8_t          mode;         /* Mode 0,1,2,3 */
};

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

/* Helpers */

static inline uint32_t spi_getreg(struct stm32_spidev_s *priv,
                                  uint8_t offset);
static inline void spi_putreg(struct stm32_spidev_s *priv,
                              uint8_t offset, uint32_t value);
static inline uint16_t spi_readword(struct stm32_spidev_s *priv);
static inline void spi_writeword(struct stm32_spidev_s *priv,
                                 uint16_t word);
static inline uint8_t spi_readbyte(struct stm32_spidev_s *priv);
static inline void spi_writebyte(struct stm32_spidev_s *priv,
                                 uint8_t byte);
static inline bool spi_16bitmode(struct stm32_spidev_s *priv);

/* DMA support */

#ifdef CONFIG_STM32N6_SPI_DMA
static void        spi_dmarxwait(struct stm32_spidev_s *priv);
static void        spi_dmatxwait(struct stm32_spidev_s *priv);
static inline void spi_dmarxwakeup(struct stm32_spidev_s *priv);
static inline void spi_dmatxwakeup(struct stm32_spidev_s *priv);
static void        spi_dmarxcallback(DMA_HANDLE handle, uint8_t isr,
                                     void *arg);
static void        spi_dmatxcallback(DMA_HANDLE handle, uint8_t isr,
                                     void *arg);
static void        spi_dmarxsetup(struct stm32_spidev_s *priv,
                                  void *rxbuffer, void *rxdummy,
                                  size_t nwords);
static void        spi_dmatxsetup(struct stm32_spidev_s *priv,
                                  const void *txbuffer,
                                  const void *txdummy, size_t nwords);
static inline void spi_dmarxstart(struct stm32_spidev_s *priv);
static inline void spi_dmatxstart(struct stm32_spidev_s *priv);
#endif

/* SPI methods */

static int         spi_lock(struct spi_dev_s *dev, bool lock);
static uint32_t    spi_setfrequency(struct spi_dev_s *dev,
                                    uint32_t frequency);
static void        spi_setmode(struct spi_dev_s *dev,
                               enum spi_mode_e mode);
static void        spi_setbits(struct spi_dev_s *dev, int nbits);
static uint32_t    spi_send(struct spi_dev_s *dev, uint32_t wd);
static void        spi_exchange(struct spi_dev_s *dev,
                                const void *txbuffer,
                                void *rxbuffer, size_t nwords);
#ifndef CONFIG_SPI_EXCHANGE
static void        spi_sndblock(struct spi_dev_s *dev,
                                const void *txbuffer, size_t nwords);
static void        spi_recvblock(struct spi_dev_s *dev,
                                 void *rxbuffer, size_t nwords);
#endif

/* Initialization */

static void        spi_bus_initialize(struct stm32_spidev_s *priv);

/****************************************************************************
 * Private Data
 ****************************************************************************/

#ifdef CONFIG_STM32N6_SPI1
static const struct spi_ops_s g_spi1ops =
{
  .lock              = spi_lock,
  .select            = stm32_spi1select,
  .setfrequency      = spi_setfrequency,
  .setmode           = spi_setmode,
  .setbits           = spi_setbits,
#ifdef CONFIG_SPI_HWFEATURES
  .hwfeatures        = 0,
#endif
  .status            = stm32_spi1status,
#ifdef CONFIG_SPI_CMDDATA
  .cmddata           = stm32_spi1cmddata,
#endif
  .send              = spi_send,
#ifdef CONFIG_SPI_EXCHANGE
  .exchange          = spi_exchange,
#else
  .sndblock          = spi_sndblock,
  .recvblock         = spi_recvblock,
#endif
#ifdef CONFIG_SPI_TRIGGER
  .trigger           = 0,
#endif
#ifdef CONFIG_SPI_CALLBACK
  .registercallback  = stm32_spi1register,
#else
  .registercallback  = 0,
#endif
};

static struct stm32_spidev_s g_spi1dev =
{
  .spidev   =
  {
    .ops    = &g_spi1ops,
  },
  .spibase  = STM32_SPI1_BASE,
  .spiclock = STM32N6_HCLK_FREQUENCY,  /* SPI1 on APB2 */
#ifdef CONFIG_STM32N6_SPI_INTERRUPTS
  .spiirq   = STM32_IRQ_SPI1,
#endif
#ifdef CONFIG_STM32N6_SPI_DMA
  .rxch     = DMACHAN_SPI1_RX,
  .txch     = DMACHAN_SPI1_TX,
  .rxsem    = SEM_INITIALIZER(0),
  .txsem    = SEM_INITIALIZER(0),
#endif
  .lock     = NXMUTEX_INITIALIZER,
};
#endif

#ifdef CONFIG_STM32N6_SPI2
static const struct spi_ops_s g_spi2ops =
{
  .lock              = spi_lock,
  .select            = stm32_spi2select,
  .setfrequency      = spi_setfrequency,
  .setmode           = spi_setmode,
  .setbits           = spi_setbits,
#ifdef CONFIG_SPI_HWFEATURES
  .hwfeatures        = 0,
#endif
  .status            = stm32_spi2status,
#ifdef CONFIG_SPI_CMDDATA
  .cmddata           = stm32_spi2cmddata,
#endif
  .send              = spi_send,
#ifdef CONFIG_SPI_EXCHANGE
  .exchange          = spi_exchange,
#else
  .sndblock          = spi_sndblock,
  .recvblock         = spi_recvblock,
#endif
#ifdef CONFIG_SPI_TRIGGER
  .trigger           = 0,
#endif
#ifdef CONFIG_SPI_CALLBACK
  .registercallback  = stm32_spi2register,
#else
  .registercallback  = 0,
#endif
};

static struct stm32_spidev_s g_spi2dev =
{
  .spidev   =
  {
    .ops    = &g_spi2ops,
  },
  .spibase  = STM32_SPI2_BASE,
  .spiclock = STM32N6_HCLK_FREQUENCY,  /* SPI2 on APB1 */
#ifdef CONFIG_STM32N6_SPI_INTERRUPTS
  .spiirq   = STM32_IRQ_SPI2,
#endif
#ifdef CONFIG_STM32N6_SPI_DMA
  .rxch     = DMACHAN_SPI2_RX,
  .txch     = DMACHAN_SPI2_TX,
  .rxsem    = SEM_INITIALIZER(0),
  .txsem    = SEM_INITIALIZER(0),
#endif
  .lock     = NXMUTEX_INITIALIZER,
};
#endif

#ifdef CONFIG_STM32N6_SPI3
static const struct spi_ops_s g_spi3ops =
{
  .lock              = spi_lock,
  .select            = stm32_spi3select,
  .setfrequency      = spi_setfrequency,
  .setmode           = spi_setmode,
  .setbits           = spi_setbits,
#ifdef CONFIG_SPI_HWFEATURES
  .hwfeatures        = 0,
#endif
  .status            = stm32_spi3status,
#ifdef CONFIG_SPI_CMDDATA
  .cmddata           = stm32_spi3cmddata,
#endif
  .send              = spi_send,
#ifdef CONFIG_SPI_EXCHANGE
  .exchange          = spi_exchange,
#else
  .sndblock          = spi_sndblock,
  .recvblock         = spi_recvblock,
#endif
#ifdef CONFIG_SPI_TRIGGER
  .trigger           = 0,
#endif
#ifdef CONFIG_SPI_CALLBACK
  .registercallback  = stm32_spi3register,
#else
  .registercallback  = 0,
#endif
};

static struct stm32_spidev_s g_spi3dev =
{
  .spidev   =
  {
    .ops    = &g_spi3ops,
  },
  .spibase  = STM32_SPI3_BASE,
  .spiclock = STM32N6_HCLK_FREQUENCY,  /* SPI3 on APB1 */
#ifdef CONFIG_STM32N6_SPI_INTERRUPTS
  .spiirq   = STM32_IRQ_SPI3,
#endif
#ifdef CONFIG_STM32N6_SPI_DMA
  .rxch     = DMACHAN_SPI3_RX,
  .txch     = DMACHAN_SPI3_TX,
  .rxsem    = SEM_INITIALIZER(0),
  .txsem    = SEM_INITIALIZER(0),
#endif
  .lock     = NXMUTEX_INITIALIZER,
};
#endif

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: spi_getreg
 *
 * Description:
 *   Get the contents of the SPI register at offset
 *
 ****************************************************************************/

static inline uint32_t spi_getreg(struct stm32_spidev_s *priv,
                                  uint8_t offset)
{
  return getreg32(priv->spibase + offset);
}

/****************************************************************************
 * Name: spi_putreg
 *
 * Description:
 *   Write a 32-bit value to the SPI register at offset
 *
 ****************************************************************************/

static inline void spi_putreg(struct stm32_spidev_s *priv,
                              uint8_t offset, uint32_t value)
{
  putreg32(value, priv->spibase + offset);
}

/****************************************************************************
 * Name: spi_readword
 *
 * Description:
 *   Read one word (TWO bytes!) from SPI
 *
 ****************************************************************************/

static inline uint16_t spi_readword(struct stm32_spidev_s *priv)
{
  /* Wait until the receive buffer is not empty */

  while ((spi_getreg(priv, STM32_SPI_SR_OFFSET) & SPI_SR_RXP) == 0);

  /* Then return the received word */

  return (uint16_t)(spi_getreg(priv, STM32_SPI_RXDR_OFFSET) & 0xffff);
}

/****************************************************************************
 * Name: spi_readbyte
 *
 * Description:
 *   Read one byte from SPI
 *
 ****************************************************************************/

static inline uint8_t spi_readbyte(struct stm32_spidev_s *priv)
{
  /* Wait until the receive buffer is not empty */

  while ((spi_getreg(priv, STM32_SPI_SR_OFFSET) & SPI_SR_RXP) == 0);

  /* Then return the received byte */

  return (uint8_t)(spi_getreg(priv, STM32_SPI_RXDR_OFFSET) & 0xff);
}

/****************************************************************************
 * Name: spi_writeword
 *
 * Description:
 *   Write one 16-bit frame to the SPI FIFO
 *
 ****************************************************************************/

static inline void spi_writeword(struct stm32_spidev_s *priv,
                                 uint16_t word)
{
  /* Wait until the transmit buffer is empty */

  while ((spi_getreg(priv, STM32_SPI_SR_OFFSET) & SPI_SR_TXP) == 0);

  /* Then send the word */

  spi_putreg(priv, STM32_SPI_TXDR_OFFSET, (uint32_t)word);
}

/****************************************************************************
 * Name: spi_writebyte
 *
 * Description:
 *   Write one 8-bit frame to the SPI FIFO
 *
 ****************************************************************************/

static inline void spi_writebyte(struct stm32_spidev_s *priv,
                                 uint8_t byte)
{
  /* Wait until the transmit buffer is empty */

  while ((spi_getreg(priv, STM32_SPI_SR_OFFSET) & SPI_SR_TXP) == 0);

  /* Then send the byte */

  spi_putreg(priv, STM32_SPI_TXDR_OFFSET, (uint32_t)byte);
}

/****************************************************************************
 * Name: spi_16bitmode
 *
 * Description:
 *   Check if the SPI is operating in 16-bit mode
 *
 ****************************************************************************/

static inline bool spi_16bitmode(struct stm32_spidev_s *priv)
{
  return (priv->nbits > 8);
}

/****************************************************************************
 * Name: spi_modifycfg1
 *
 * Description:
 *   Clear and set bits in the CFG1 register
 *
 ****************************************************************************/

static void spi_modifycfg1(struct stm32_spidev_s *priv,
                           uint32_t setbits, uint32_t clrbits)
{
  uint32_t cfg1;

  cfg1 = spi_getreg(priv, STM32_SPI_CFG1_OFFSET);
  cfg1 &= ~clrbits;
  cfg1 |= setbits;
  spi_putreg(priv, STM32_SPI_CFG1_OFFSET, cfg1);
}

/****************************************************************************
 * Name: spi_modifycfg2
 *
 * Description:
 *   Clear and set bits in the CFG2 register
 *
 ****************************************************************************/

static void spi_modifycfg2(struct stm32_spidev_s *priv,
                           uint32_t setbits, uint32_t clrbits)
{
  uint32_t cfg2;

  cfg2 = spi_getreg(priv, STM32_SPI_CFG2_OFFSET);
  cfg2 &= ~clrbits;
  cfg2 |= setbits;
  spi_putreg(priv, STM32_SPI_CFG2_OFFSET, cfg2);
}

/****************************************************************************
 * Name: spi_lock
 *
 * Description:
 *   On SPI busses where there are multiple devices, it will be necessary to
 *   lock SPI to have exclusive access to the busses for a sequence of
 *   transfers.  The bus should be locked before the chip is selected.
 *
 ****************************************************************************/

static int spi_lock(struct spi_dev_s *dev, bool lock)
{
  struct stm32_spidev_s *priv = (struct stm32_spidev_s *)dev;
  int ret;

  if (lock)
    {
      /* Take the mutex (perhaps waiting) */

      ret = nxmutex_lock(&priv->lock);
    }
  else
    {
      nxmutex_unlock(&priv->lock);
      ret = OK;
    }

  return ret;
}

/****************************************************************************
 * Name: spi_setfrequency
 *
 * Description:
 *   Set the SPI frequency.
 *
 * Input Parameters:
 *   dev -       Device-specific state data
 *   frequency - The SPI frequency requested
 *
 * Returned Value:
 *   Returns the actual frequency selected
 *
 ****************************************************************************/

static uint32_t spi_setfrequency(struct spi_dev_s *dev,
                                 uint32_t frequency)
{
  struct stm32_spidev_s *priv = (struct stm32_spidev_s *)dev;
  uint32_t setbits;
  uint32_t actual;

  /* Limit to max possible */

  if (frequency > STM32_SPI_CLK_MAX)
    {
      frequency = STM32_SPI_CLK_MAX;
    }

  /* Has the frequency changed? */

  if (frequency != priv->frequency)
    {
      /* Choices are limited by PCLK frequency with a set of divisors */

      if (frequency >= priv->spiclock >> 1)
        {
          setbits = SPI_CFG1_MBRd2;  /* fPCLK/2 */
          actual = priv->spiclock >> 1;
        }
      else if (frequency >= priv->spiclock >> 2)
        {
          setbits = SPI_CFG1_MBRd4;  /* fPCLK/4 */
          actual = priv->spiclock >> 2;
        }
      else if (frequency >= priv->spiclock >> 3)
        {
          setbits = SPI_CFG1_MBRd8;  /* fPCLK/8 */
          actual = priv->spiclock >> 3;
        }
      else if (frequency >= priv->spiclock >> 4)
        {
          setbits = SPI_CFG1_MBRd16; /* fPCLK/16 */
          actual = priv->spiclock >> 4;
        }
      else if (frequency >= priv->spiclock >> 5)
        {
          setbits = SPI_CFG1_MBRd32; /* fPCLK/32 */
          actual = priv->spiclock >> 5;
        }
      else if (frequency >= priv->spiclock >> 6)
        {
          setbits = SPI_CFG1_MBRd64; /* fPCLK/64 */
          actual = priv->spiclock >> 6;
        }
      else if (frequency >= priv->spiclock >> 7)
        {
          setbits = SPI_CFG1_MBRd128; /* fPCLK/128 */
          actual = priv->spiclock >> 7;
        }
      else
        {
          setbits = SPI_CFG1_MBRd256; /* fPCLK/256 */
          actual = priv->spiclock >> 8;
        }

      /* Disable SPI, modify MBR, re-enable */

      spi_putreg(priv, STM32_SPI_CR1_OFFSET, 0);
      spi_modifycfg1(priv, setbits, SPI_CFG1_MBR_MASK);
      spi_putreg(priv, STM32_SPI_CR1_OFFSET, SPI_CR1_SPE);

      spiinfo("Frequency %" PRId32 "->%" PRId32 "\n", frequency, actual);

      priv->frequency = frequency;
      priv->actual    = actual;
    }

  return priv->actual;
}

/****************************************************************************
 * Name: spi_setmode
 *
 * Description:
 *   Set the SPI mode.  see enum spi_mode_e for mode definitions
 *
 ****************************************************************************/

static void spi_setmode(struct spi_dev_s *dev, enum spi_mode_e mode)
{
  struct stm32_spidev_s *priv = (struct stm32_spidev_s *)dev;
  uint32_t setbits;
  uint32_t clrbits;

  spiinfo("mode=%d\n", mode);

  /* Has the mode changed? */

  if (mode != priv->mode)
    {
      switch (mode)
        {
        case SPIDEV_MODE0: /* CPOL=0; CPHA=0 */
          setbits = 0;
          clrbits = SPI_CFG2_CPOL | SPI_CFG2_CPHA;
          break;

        case SPIDEV_MODE1: /* CPOL=0; CPHA=1 */
          setbits = SPI_CFG2_CPHA;
          clrbits = SPI_CFG2_CPOL;
          break;

        case SPIDEV_MODE2: /* CPOL=1; CPHA=0 */
          setbits = SPI_CFG2_CPOL;
          clrbits = SPI_CFG2_CPHA;
          break;

        case SPIDEV_MODE3: /* CPOL=1; CPHA=1 */
          setbits = SPI_CFG2_CPOL | SPI_CFG2_CPHA;
          clrbits = 0;
          break;

        default:
          return;
        }

      spi_putreg(priv, STM32_SPI_CR1_OFFSET, 0);
      spi_modifycfg2(priv, setbits, clrbits);
      spi_putreg(priv, STM32_SPI_CR1_OFFSET, SPI_CR1_SPE);

      priv->mode = mode;
    }
}

/****************************************************************************
 * Name: spi_setbits
 *
 * Description:
 *   Set the number of bits per word.
 *
 ****************************************************************************/

static void spi_setbits(struct spi_dev_s *dev, int nbits)
{
  struct stm32_spidev_s *priv = (struct stm32_spidev_s *)dev;
  uint32_t setbits;
  uint32_t clrbits;

  spiinfo("nbits=%d\n", nbits);

  /* Has the number of bits changed? */

  if (nbits != priv->nbits)
    {
      /* Set the number of bits (valid range 4-16) */

      if (nbits < 4 || nbits > 16)
        {
          spierr("ERROR: nbits out of range: %d\n", nbits);
          return;
        }

      clrbits = SPI_CFG1_DSIZE_MASK;
      setbits = SPI_CFG1_DSIZE(nbits);

      /* If nbits is <=8, then we are in byte mode and FTHLV shall be set
       * accordingly.
       */

      if (nbits < 9)
        {
          setbits |= SPI_CFG1_FTHLV_1DATA;
          clrbits |= SPI_CFG1_FTHLV_MASK;
        }
      else
        {
          setbits |= SPI_CFG1_FTHLV_1DATA;
          clrbits |= SPI_CFG1_FTHLV_MASK;
        }

      spi_putreg(priv, STM32_SPI_CR1_OFFSET, 0);
      spi_modifycfg1(priv, setbits, clrbits);
      spi_putreg(priv, STM32_SPI_CR1_OFFSET, SPI_CR1_SPE);

      priv->nbits = nbits;
    }
}

/****************************************************************************
 * Name: spi_send
 *
 * Description:
 *   Exchange one word on SPI
 *
 ****************************************************************************/

static uint32_t spi_send(struct spi_dev_s *dev, uint32_t wd)
{
  struct stm32_spidev_s *priv = (struct stm32_spidev_s *)dev;
  uint32_t regval;
  uint32_t ret;

  DEBUGASSERT(priv && priv->spibase);

  /* Set transfer size to 1 */

  spi_putreg(priv, STM32_SPI_CR2_OFFSET, 1);

  /* Start the transfer */

  spi_putreg(priv, STM32_SPI_CR1_OFFSET,
             spi_getreg(priv, STM32_SPI_CR1_OFFSET) | SPI_CR1_CSTART);

  /* According to the number of bits, access data register as word or byte */

  if (spi_16bitmode(priv))
    {
      spi_writeword(priv, (uint16_t)(wd & 0xffff));
      ret = (uint32_t)spi_readword(priv);
    }
  else
    {
      spi_writebyte(priv, (uint8_t)(wd & 0xff));
      ret = (uint32_t)spi_readbyte(priv);
    }

  /* Wait for end of transfer */

  while ((spi_getreg(priv, STM32_SPI_SR_OFFSET) & SPI_SR_EOT) == 0);

  /* Clear EOT flag */

  spi_putreg(priv, STM32_SPI_IFCR_OFFSET, SPI_IFCR_EOTC);

  /* Check and clear any error flags */

  regval = spi_getreg(priv, STM32_SPI_SR_OFFSET);
  if (regval & (SPI_SR_OVR | SPI_SR_MODF | SPI_SR_CRCERR))
    {
      spi_putreg(priv, STM32_SPI_IFCR_OFFSET,
                 SPI_IFCR_OVRC | SPI_IFCR_MODFC | SPI_IFCR_CRCEC);
    }

  if (spi_16bitmode(priv))
    {
      spiinfo("Sent: %04" PRIx32 " Return: %04" PRIx32
              " Status: %08" PRIx32 "\n", wd, ret, regval);
    }
  else
    {
      spiinfo("Sent: %02" PRIx32 " Return: %02" PRIx32
              " Status: %08" PRIx32 "\n", wd, ret, regval);
    }

  UNUSED(regval);
  return ret;
}

/****************************************************************************
 * Name: spi_exchange (no DMA)
 *
 * Description:
 *   Exchange a block of data on SPI without using DMA
 *
 ****************************************************************************/

#if !defined(CONFIG_STM32N6_SPI_DMA) || defined(CONFIG_STM32N6_DMACAPABLE)
#if !defined(CONFIG_STM32N6_SPI_DMA)
static void spi_exchange(struct spi_dev_s *dev, const void *txbuffer,
                         void *rxbuffer, size_t nwords)
#else
static void spi_exchange_nodma(struct spi_dev_s *dev,
                               const void *txbuffer,
                               void *rxbuffer, size_t nwords)
#endif
{
  struct stm32_spidev_s *priv = (struct stm32_spidev_s *)dev;
  DEBUGASSERT(priv && priv->spibase);

  spiinfo("txbuffer=%p rxbuffer=%p nwords=%zu\n", txbuffer, rxbuffer,
          nwords);

  /* Set transfer size */

  spi_putreg(priv, STM32_SPI_CR2_OFFSET, (uint32_t)nwords);

  /* Start the transfer */

  spi_putreg(priv, STM32_SPI_CR1_OFFSET,
             spi_getreg(priv, STM32_SPI_CR1_OFFSET) | SPI_CR1_CSTART);

  /* 8- or 16-bit mode? */

  if (spi_16bitmode(priv))
    {
      /* 16-bit mode */

      const uint16_t *src  = (const uint16_t *)txbuffer;
            uint16_t *dest = (uint16_t *)rxbuffer;
            uint16_t  word;

      while (nwords-- > 0)
        {
          /* Get the next word to write.  Is there a source buffer? */

          if (src)
            {
              word = *src++;
            }
          else
            {
              word = 0xffff;
            }

          /* Exchange one word */

          word = (uint16_t)spi_send(dev, word);

          /* Is there a buffer to receive the return value? */

          if (dest)
            {
              *dest++ = word;
            }
        }
    }
  else
    {
      /* 8-bit mode */

      const uint8_t *src  = (const uint8_t *)txbuffer;
            uint8_t *dest = (uint8_t *)rxbuffer;
            uint8_t  word;

      while (nwords-- > 0)
        {
          /* Get the next word to write.  Is there a source buffer? */

          if (src)
            {
              word = *src++;
            }
          else
            {
              word = 0xff;
            }

          /* Exchange one word */

          word = (uint8_t)spi_send(dev, (uint32_t)word);

          /* Is there a buffer to receive the return value? */

          if (dest)
            {
              *dest++ = word;
            }
        }
    }
}
#endif /* !CONFIG_STM32N6_SPI_DMA || CONFIG_STM32N6_DMACAPABLE */

/****************************************************************************
 * Name: spi_exchange (with DMA capability)
 *
 * Description:
 *   Exchange a block of data on SPI using DMA
 *
 ****************************************************************************/

#ifdef CONFIG_STM32N6_SPI_DMA
static void spi_exchange(struct spi_dev_s *dev, const void *txbuffer,
                         void *rxbuffer, size_t nwords)
{
  struct stm32_spidev_s *priv = (struct stm32_spidev_s *)dev;

#ifdef CONFIG_STM32N6_DMACAPABLE
  if ((txbuffer &&
       !stm32_dmacapable((uint32_t)txbuffer, nwords, priv->txccr)) ||
      (rxbuffer &&
       !stm32_dmacapable((uint32_t)rxbuffer, nwords, priv->rxccr)))
    {
      spi_exchange_nodma(dev, txbuffer, rxbuffer, nwords);
    }
  else
#endif
    {
      static uint16_t rxdummy = 0xffff;
      static const uint16_t txdummy = 0xffff;

      spiinfo("txbuffer=%p rxbuffer=%p nwords=%zu\n", txbuffer, rxbuffer,
              nwords);
      DEBUGASSERT(priv && priv->spibase);

      /* Set transfer size */

      spi_putreg(priv, STM32_SPI_CR2_OFFSET, (uint32_t)nwords);

      /* Setup DMAs */

      spi_dmarxsetup(priv, rxbuffer, &rxdummy, nwords);
      spi_dmatxsetup(priv, txbuffer, &txdummy, nwords);

      /* Start the DMAs */

      spi_dmarxstart(priv);
      spi_dmatxstart(priv);

      /* Start the transfer */

      spi_putreg(priv, STM32_SPI_CR1_OFFSET,
                 spi_getreg(priv, STM32_SPI_CR1_OFFSET) | SPI_CR1_CSTART);

      /* Then wait for each to complete */

      spi_dmarxwait(priv);
      spi_dmatxwait(priv);
    }
}
#endif /* CONFIG_STM32N6_SPI_DMA */

/****************************************************************************
 * Name: spi_sndblock
 *
 * Description:
 *   Send a block of data on SPI
 *
 ****************************************************************************/

#ifndef CONFIG_SPI_EXCHANGE
static void spi_sndblock(struct spi_dev_s *dev, const void *txbuffer,
                         size_t nwords)
{
  spiinfo("txbuffer=%p nwords=%zu\n", txbuffer, nwords);
  return spi_exchange(dev, txbuffer, NULL, nwords);
}
#endif

/****************************************************************************
 * Name: spi_recvblock
 *
 * Description:
 *   Receive a block of data from SPI
 *
 ****************************************************************************/

#ifndef CONFIG_SPI_EXCHANGE
static void spi_recvblock(struct spi_dev_s *dev, void *rxbuffer,
                          size_t nwords)
{
  spiinfo("rxbuffer=%p nwords=%zu\n", rxbuffer, nwords);
  return spi_exchange(dev, NULL, rxbuffer, nwords);
}
#endif

/****************************************************************************
 * Name: spi_bus_initialize
 *
 * Description:
 *   Initialize the selected SPI bus in its default state (Master, 8-bit,
 *   mode 0, etc.)
 *
 ****************************************************************************/

static void spi_bus_initialize(struct stm32_spidev_s *priv)
{
  /* Disable SPI */

  spi_putreg(priv, STM32_SPI_CR1_OFFSET, 0);

  /* Configure CFG1: 8-bit data, FIFO threshold 1 */

  spi_putreg(priv, STM32_SPI_CFG1_OFFSET,
             SPI_CFG1_DSIZE_8BIT | SPI_CFG1_FTHLV_1DATA);

  /* Configure CFG2: Master mode, Motorola SPI protocol, CPOL=0, CPHA=0,
   * SSM enabled, SS output enable
   */

  spi_putreg(priv, STM32_SPI_CFG2_OFFSET,
             SPI_CFG2_MASTER | SPI_CFG2_SP_MOTOROLA | SPI_CFG2_SSM |
             SPI_CFG2_SSOE | SPI_CFG2_SSOM | SPI_CFG2_AFCNTR);

  /* Disable all interrupts */

  spi_putreg(priv, STM32_SPI_IER_OFFSET, 0);

  /* Clear all interrupt flags */

  spi_putreg(priv, STM32_SPI_IFCR_OFFSET,
             SPI_IFCR_EOTC | SPI_IFCR_TXTFC | SPI_IFCR_UDRC |
             SPI_IFCR_OVRC | SPI_IFCR_CRCEC | SPI_IFCR_TIFREC |
             SPI_IFCR_MODFC | SPI_IFCR_TSERFC | SPI_IFCR_SUSPC);

  /* Set default transfer size */

  spi_putreg(priv, STM32_SPI_CR2_OFFSET, 0);

  priv->frequency = 0;
  priv->nbits     = 8;
  priv->mode      = SPIDEV_MODE0;

  /* Select a default frequency of approx. 400KHz */

  spi_setfrequency((struct spi_dev_s *)priv, 400000);

  /* Enable SPI */

  spi_putreg(priv, STM32_SPI_CR1_OFFSET, SPI_CR1_SPE);

#ifdef CONFIG_STM32N6_SPI_DMA
  /* Get DMA channels */

  priv->rxdma = stm32_dmachannel(priv->rxch);
  priv->txdma = stm32_dmachannel(priv->txch);
  DEBUGASSERT(priv->rxdma && priv->txdma);

  /* Enable DMA */

  spi_modifycfg1(priv, SPI_CFG1_RXDMAEN | SPI_CFG1_TXDMAEN, 0);
#endif
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: stm32_spibus_initialize
 *
 * Description:
 *   Initialize the selected SPI bus
 *
 * Input Parameters:
 *   Port number (for hardware that has multiple SPI interfaces)
 *
 * Returned Value:
 *   Valid SPI device structure reference on success; a NULL on failure
 *
 ****************************************************************************/

struct spi_dev_s *stm32_spibus_initialize(int bus)
{
  struct stm32_spidev_s *priv = NULL;

  irqstate_t flags = enter_critical_section();

#ifdef CONFIG_STM32N6_SPI1
  if (bus == 1)
    {
      priv = &g_spi1dev;

      if (!priv->initialized)
        {
          /* Enable SPI1 clock (SPI1 is on APB2) */

          rcc_enable_apb2_clock(RCC_APB2ENR_SPI1EN);

          /* Configure SPI1 pins: SCK, MISO, and MOSI */

          stm32_configgpio(GPIO_SPI1_SCK);
          stm32_configgpio(GPIO_SPI1_MISO);
          stm32_configgpio(GPIO_SPI1_MOSI);

          /* Set up default configuration: Master, 8-bit, etc. */

          spi_bus_initialize(priv);
          priv->initialized = true;
        }
    }
  else
#endif
#ifdef CONFIG_STM32N6_SPI2
  if (bus == 2)
    {
      priv = &g_spi2dev;

      if (!priv->initialized)
        {
          /* Enable SPI2 clock (SPI2 is on APB1) */

          rcc_enable_apb1_clock(RCC_APB1ENR1_SPI2EN);

          /* Configure SPI2 pins: SCK, MISO, and MOSI */

          stm32_configgpio(GPIO_SPI2_SCK);
          stm32_configgpio(GPIO_SPI2_MISO);
          stm32_configgpio(GPIO_SPI2_MOSI);

          /* Set up default configuration: Master, 8-bit, etc. */

          spi_bus_initialize(priv);
          priv->initialized = true;
        }
    }
  else
#endif
#ifdef CONFIG_STM32N6_SPI3
  if (bus == 3)
    {
      priv = &g_spi3dev;

      if (!priv->initialized)
        {
          /* Enable SPI3 clock (SPI3 is on APB1) */

          rcc_enable_apb1_clock(RCC_APB1ENR1_SPI3EN);

          /* Configure SPI3 pins: SCK, MISO, and MOSI */

          stm32_configgpio(GPIO_SPI3_SCK);
          stm32_configgpio(GPIO_SPI3_MISO);
          stm32_configgpio(GPIO_SPI3_MOSI);

          /* Set up default configuration: Master, 8-bit, etc. */

          spi_bus_initialize(priv);
          priv->initialized = true;
        }
    }
  else
#endif
    {
      spierr("ERROR: Unsupported SPI bus: %d\n", bus);
    }

  leave_critical_section(flags);
  return (struct spi_dev_s *)priv;
}

#endif /* CONFIG_STM32N6_SPI1 || CONFIG_STM32N6_SPI2 || CONFIG_STM32N6_SPI3 */
