/****************************************************************************
 * arch/arm/src/stm32n6/stm32_i2c.c
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
 * STM32N6 I2C Driver
 *
 * This driver supports:
 *   - Master mode I2C transfers
 *   - 7-bit and 10-bit addressing
 *   - Standard-mode (100 kHz), Fast-mode (400 kHz), Fast-mode Plus (1 MHz)
 *   - Interrupt-driven transfers
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <sys/types.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <assert.h>
#include <errno.h>
#include <debug.h>

#include <nuttx/arch.h>
#include <nuttx/mutex.h>
#include <nuttx/semaphore.h>
#include <nuttx/kmalloc.h>
#include <nuttx/clock.h>
#include <nuttx/i2c/i2c_master.h>
#include <nuttx/spinlock.h>

#include "arm_internal.h"
#include "stm32_i2c.h"
#include "hardware/stm32_i2c.h"
#include "hardware/stm32_gpio.h"
#include "hardware/stm32_rcc.h"
#include "hardware/stm32_nvic.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* Default I2C timing values for 200MHz I2C kernel clock (APB1 = 200MHz).
 *
 * TIMINGR register layout:
 *   [31:28] PRESC  - Timing prescaler (divides by PRESC+1)
 *   [23:20] SCLDEL - Data setup time = (SCLDEL+1) * tPRESC
 *   [19:16] SDADEL - Data hold time  = SDADEL * tPRESC
 *   [15:8]  SCLH   - SCL high period = (SCLH+1) * tPRESC
 *   [7:0]   SCLL   - SCL low period  = (SCLL+1) * tPRESC
 *
 * fSCL = fI2CCLK / ((PRESC+1) * (SCLL+1 + SCLH+1))
 *
 * 100kHz Standard mode: PRESC=9 (tPRESC=50ns), SCLL=99, SCLH=99
 *   fSCL = 200MHz / (10 * 200) = 100kHz
 *   SCL low = 5.0us (>= 4.7us), SCL high = 5.0us (>= 4.0us)
 *   Data setup = 250ns (>= 250ns)
 *
 * 400kHz Fast mode: PRESC=4 (tPRESC=25ns), SCLL=59, SCLH=39
 *   fSCL = 200MHz / (5 * 100) = 400kHz
 *   SCL low = 1.5us (>= 1.3us), SCL high = 1.0us (>= 0.6us)
 *   Data setup = 100ns (>= 100ns)
 *
 * 1MHz Fast-mode Plus: PRESC=1 (tPRESC=10ns), SCLL=64, SCLH=34
 *   fSCL = 200MHz / (2 * 100) = 1MHz
 *   SCL low = 650ns (>= 500ns), SCL high = 350ns (>= 260ns)
 *   Data setup = 50ns (>= 50ns)
 */

#define STM32_I2C_TIMINGR_100KHZ   0x90406363
#define STM32_I2C_TIMINGR_400KHZ   0x4030273b
#define STM32_I2C_TIMINGR_1MHZ     0x10402240

/* Timeout for I2C operations */

#define STM32_I2C_TIMEOUT_DEFAULT   500  /* milliseconds */

/****************************************************************************
 * Private Types
 ****************************************************************************/

/* I2C device private data structure */

struct stm32_i2c_priv_s
{
  /* Generic I2C device -- must be first */

  struct i2c_master_s dev;

  /* Port configuration */

  uint8_t port;           /* I2C port number (1-4) */
  uint32_t base;          /* I2C peripheral base address */
  uint32_t frequency;     /* Current I2C frequency */
  uint32_t timing;        /* Current timing register value */

  /* Transfer state */

  mutex_t lock;           /* Exclusive access to the I2C bus */
  sem_t sem_isr;          /* ISR wait semaphore */
  volatile int result;    /* Transfer result */

  /* Message processing state */

  FAR struct i2c_msg_s *msgs;   /* Message array */
  int msgcount;                  /* Number of messages */
  int msgidx;                    /* Current message index */
  FAR uint8_t *ptr;              /* Current data pointer */
  int remaining;                 /* Remaining bytes in current message */
  bool firstbyte;                /* First byte flag */
};

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

/* Register access helpers */

static inline uint32_t stm32_i2c_getreg(struct stm32_i2c_priv_s *priv,
                                         uint32_t offset);
static inline void stm32_i2c_putreg(struct stm32_i2c_priv_s *priv,
                                     uint32_t offset, uint32_t value);
static inline void stm32_i2c_modifyreg(struct stm32_i2c_priv_s *priv,
                                        uint32_t offset, uint32_t clearbits,
                                        uint32_t setbits);

/* I2C timing calculation */

static uint32_t stm32_i2c_timing(uint32_t frequency);

/* I2C operations */

static int stm32_i2c_transfer(struct i2c_master_s *dev,
                               struct i2c_msg_s *msgs, int count);
#ifdef CONFIG_I2C_RESET
static int stm32_i2c_reset(struct i2c_master_s *dev);
#endif
static int stm32_i2c_setup(struct i2c_master_s *dev);
static int stm32_i2c_shutdown(struct i2c_master_s *dev);

/* Transfer helpers */

static int stm32_i2c_sendstart(struct stm32_i2c_priv_s *priv);
static int stm32_i2c_waitforcompletion(struct stm32_i2c_priv_s *priv);

/****************************************************************************
 * Private Data
 ****************************************************************************/

/* I2C operations vtable */

static const struct i2c_ops_s g_i2c_ops =
{
  .transfer  = stm32_i2c_transfer,
#ifdef CONFIG_I2C_RESET
  .reset     = stm32_i2c_reset,
#endif
  .setup     = stm32_i2c_setup,
  .shutdown  = stm32_i2c_shutdown,
};

/* I2C device private data structures */

#ifdef CONFIG_STM32N6_I2C1
static struct stm32_i2c_priv_s g_i2c1_priv =
{
  .dev.ops   = &g_i2c_ops,
  .port      = 1,
  .base      = STM32N6_I2C1_BASE,
  .frequency = 0,
  .timing    = 0,
};
#endif

#ifdef CONFIG_STM32N6_I2C2
static struct stm32_i2c_priv_s g_i2c2_priv =
{
  .dev.ops   = &g_i2c_ops,
  .port      = 2,
  .base      = STM32N6_I2C2_BASE,
  .frequency = 0,
  .timing    = 0,
};
#endif

#ifdef CONFIG_STM32N6_I2C3
static struct stm32_i2c_priv_s g_i2c3_priv =
{
  .dev.ops   = &g_i2c_ops,
  .port      = 3,
  .base      = STM32N6_I2C3_BASE,
  .frequency = 0,
  .timing    = 0,
};
#endif

#ifdef CONFIG_STM32N6_I2C4
static struct stm32_i2c_priv_s g_i2c4_priv =
{
  .dev.ops   = &g_i2c_ops,
  .port      = 4,
  .base      = STM32N6_I2C4_BASE,
  .frequency = 0,
  .timing    = 0,
};
#endif

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: stm32_i2c_getreg
 *
 * Description:
 *   Read an I2C register
 *
 ****************************************************************************/

static inline uint32_t stm32_i2c_getreg(struct stm32_i2c_priv_s *priv,
                                         uint32_t offset)
{
  return getreg32(priv->base + offset);
}

/****************************************************************************
 * Name: stm32_i2c_putreg
 *
 * Description:
 *   Write an I2C register
 *
 ****************************************************************************/

static inline void stm32_i2c_putreg(struct stm32_i2c_priv_s *priv,
                                     uint32_t offset, uint32_t value)
{
  putreg32(value, priv->base + offset);
}

/****************************************************************************
 * Name: stm32_i2c_modifyreg
 *
 * Description:
 *   Modify an I2C register
 *
 ****************************************************************************/

static inline void stm32_i2c_modifyreg(struct stm32_i2c_priv_s *priv,
                                        uint32_t offset, uint32_t clearbits,
                                        uint32_t setbits)
{
  modifyreg32(priv->base + offset, clearbits, setbits);
}

/****************************************************************************
 * Name: stm32_i2c_timing
 *
 * Description:
 *   Calculate I2C timing register value based on desired frequency
 *
 ****************************************************************************/

static uint32_t stm32_i2c_timing(uint32_t frequency)
{
  /* Use pre-calculated timing values for common frequencies */

  if (frequency <= I2C_SPEED_STANDARD)
    {
      /* 100 kHz Standard mode */

      return STM32_I2C_TIMINGR_100KHZ;
    }
  else if (frequency <= I2C_SPEED_FAST)
    {
      /* 400 kHz Fast mode */

      return STM32_I2C_TIMINGR_400KHZ;
    }
  else
    {
      /* 1 MHz Fast mode Plus */

      return STM32_I2C_TIMINGR_1MHZ;
    }
}

/****************************************************************************
 * Name: stm32_i2c_sendstart
 *
 * Description:
 *   Send START condition and slave address
 *
 ****************************************************************************/

static int stm32_i2c_sendstart(struct stm32_i2c_priv_s *priv)
{
  FAR struct i2c_msg_s *msg = &priv->msgs[priv->msgidx];
  uint32_t cr2 = 0;

  /* Set slave address */

  if (msg->flags & I2C_M_TEN)
    {
      /* 10-bit addressing */

      cr2 |= (msg->addr & 0x03ff) << I2C_CR2_SADD10_SHIFT;
      cr2 |= I2C_CR2_ADD10;
    }
  else
    {
      /* 7-bit addressing */

      cr2 |= (msg->addr & 0x7f) << I2C_CR2_SADD7_SHIFT;
    }

  /* Set transfer direction */

  if (msg->flags & I2C_M_READ)
    {
      cr2 |= I2C_CR2_RD_WRN;
    }

  /* Set number of bytes */

  if (msg->length <= 255)
    {
      cr2 |= (msg->length << I2C_CR2_NBYTES_SHIFT);
    }
  else
    {
      /* Use RELOAD mode for transfers > 255 bytes */

      cr2 |= (255 << I2C_CR2_NBYTES_SHIFT);
      cr2 |= I2C_CR2_RELOAD;
    }

  /* If this is not the last message and next message has NOSTART,
   * don't send STOP after this message.
   * Otherwise, use AUTOEND to automatically send STOP.
   */

  if (priv->msgidx < priv->msgcount - 1)
    {
      FAR struct i2c_msg_s *nextmsg = &priv->msgs[priv->msgidx + 1];
      if (nextmsg->flags & I2C_M_NOSTART)
        {
          /* Don't use AUTOEND - we need to continue */

          cr2 |= I2C_CR2_RELOAD;
        }
      else if (msg->flags & I2C_M_NOSTOP)
        {
          /* Repeated START - don't send STOP */

        }
      else
        {
          cr2 |= I2C_CR2_AUTOEND;
        }
    }
  else
    {
      /* Last message - send STOP */

      if (!(msg->flags & I2C_M_NOSTOP))
        {
          cr2 |= I2C_CR2_AUTOEND;
        }
    }

  /* Clear any pending interrupts */

  stm32_i2c_putreg(priv, STM32_I2C_ICR_OFFSET, I2C_ICR_CLEARMASK);

  /* Set CR2 and generate START */

  cr2 |= I2C_CR2_START;
  stm32_i2c_putreg(priv, STM32_I2C_CR2_OFFSET, cr2);

  return OK;
}

/****************************************************************************
 * Name: stm32_i2c_waitforcompletion
 *
 * Description:
 *   Wait for I2C transfer completion
 *
 ****************************************************************************/

static int stm32_i2c_waitforcompletion(struct stm32_i2c_priv_s *priv)
{
  int ret;

  /* Wait for semaphore to be posted by ISR or timeout */

  ret = nxsem_tickwait(&priv->sem_isr, MSEC2TICK(STM32_I2C_TIMEOUT_DEFAULT));
  if (ret < 0)
    {
      i2cerr("ERROR: I2C transfer timeout\n");
      return ret;
    }

  return priv->result;
}

/****************************************************************************
 * Name: stm32_i2c_isr_handler
 *
 * Description:
 *   I2C interrupt handler
 *
 ****************************************************************************/

static int stm32_i2c_isr_handler(int irq, FAR void *context, FAR void *arg)
{
  FAR struct stm32_i2c_priv_s *priv = (FAR struct stm32_i2c_priv_s *)arg;
  uint32_t isr;

  DEBUGASSERT(priv != NULL);

  /* Read interrupt status */

  isr = stm32_i2c_getreg(priv, STM32_I2C_ISR_OFFSET);

  /* Check for errors */

  if (isr & I2C_ISR_ERRORMASK)
    {
      if (isr & I2C_INT_NACK)
        {
          i2cerr("ERROR: NACK received\n");
          priv->result = -ENXIO;
        }
      else if (isr & I2C_INT_BERR)
        {
          i2cerr("ERROR: Bus error\n");
          priv->result = -EIO;
        }
      else if (isr & I2C_INT_ARLO)
        {
          i2cerr("ERROR: Arbitration lost\n");
          priv->result = -EAGAIN;
        }
      else if (isr & I2C_INT_OVR)
        {
          i2cerr("ERROR: Overrun/Underrun\n");
          priv->result = -EIO;
        }
      else
        {
          i2cerr("ERROR: Unknown error (ISR=0x%08" PRIx32 ")\n", isr);
          priv->result = -EIO;
        }

      /* Clear error flags */

      stm32_i2c_putreg(priv, STM32_I2C_ICR_OFFSET, I2C_ICR_CLEARMASK);

      /* Disable I2C */

      stm32_i2c_modifyreg(priv, STM32_I2C_CR1_OFFSET, I2C_CR1_PE, 0);

      /* Post semaphore to wake waiting thread */

      nxsem_post(&priv->sem_isr);
      return OK;
    }

  /* Check for STOP condition */

  if (isr & I2C_INT_STOP)
    {
      /* Clear STOP flag */

      stm32_i2c_putreg(priv, STM32_I2C_ICR_OFFSET, I2C_INT_STOP);

      /* Transfer complete */

      priv->result = OK;
      nxsem_post(&priv->sem_isr);
      return OK;
    }

  /* Check for TXIS (transmit interrupt status) */

  if (isr & I2C_ISR_TXIS)
    {
      if (priv->remaining > 0 && priv->ptr != NULL)
        {
          /* Send next byte */

          stm32_i2c_putreg(priv, STM32_I2C_TXDR_OFFSET, *priv->ptr++);
          priv->remaining--;
        }
      else
        {
          /* No more data - send 0 */

          stm32_i2c_putreg(priv, STM32_I2C_TXDR_OFFSET, 0);
        }
    }

  /* Check for RXNE (receive data register not empty) */

  if (isr & I2C_ISR_RXNE)
    {
      if (priv->remaining > 0 && priv->ptr != NULL)
        {
          /* Read received byte */

          *priv->ptr++ = stm32_i2c_getreg(priv, STM32_I2C_RXDR_OFFSET) & 0xff;
          priv->remaining--;
        }
      else
        {
          /* Discard received byte */

          stm32_i2c_getreg(priv, STM32_I2C_RXDR_OFFSET);
        }
    }

  /* Check for TC (transfer complete) */

  if (isr & I2C_ISR_TC)
    {
      /* Move to next message */

      priv->msgidx++;

      if (priv->msgidx < priv->msgcount)
        {
          /* Send repeated START for next message */

          stm32_i2c_sendstart(priv);
        }
      else
        {
          /* All messages transferred */

          if (!(priv->msgs[priv->msgcount - 1].flags & I2C_M_NOSTOP))
            {
              /* Wait for STOP condition */

            }
          else
            {
              priv->result = OK;
              nxsem_post(&priv->sem_isr);
            }
        }
    }

  /* Check for TCR (transfer complete reload) */

  if (isr & I2C_ISR_TCR)
    {
      if (priv->remaining > 0)
        {
          /* More data to transfer - reload NBYTES */

          uint32_t cr2 = stm32_i2c_getreg(priv, STM32_I2C_CR2_OFFSET);
          uint32_t nbytes;

          cr2 &= ~I2C_CR2_NBYTES_MASK;

          if (priv->remaining > 255)
            {
              nbytes = 255;
              cr2 |= I2C_CR2_RELOAD;
            }
          else
            {
              nbytes = priv->remaining;
              cr2 &= ~I2C_CR2_RELOAD;
            }

          cr2 |= (nbytes << I2C_CR2_NBYTES_SHIFT);
          stm32_i2c_putreg(priv, STM32_I2C_CR2_OFFSET, cr2);
        }
    }

  return OK;
}

/****************************************************************************
 * Name: stm32_i2c_setup
 *
 * Description:
 *   Setup the I2C hardware
 *
 ****************************************************************************/

static int stm32_i2c_setup(struct i2c_master_s *dev)
{
  FAR struct stm32_i2c_priv_s *priv = (FAR struct stm32_i2c_priv_s *)dev;
  uint32_t timing;

  DEBUGASSERT(priv != NULL);

  i2cinfo("I2C%d: Setup\n", priv->port);

  /* Calculate timing for default frequency (100kHz) */

  timing = stm32_i2c_timing(I2C_SPEED_STANDARD);

  /* Disable I2C before configuration */

  stm32_i2c_putreg(priv, STM32_I2C_CR1_OFFSET, 0);

  /* Set timing register */

  stm32_i2c_putreg(priv, STM32_I2C_TIMINGR_OFFSET, timing);
  priv->timing = timing;
  priv->frequency = I2C_SPEED_STANDARD;

  /* Enable I2C with interrupts */

  stm32_i2c_putreg(priv, STM32_I2C_CR1_OFFSET,
                    I2C_CR1_PE | I2C_CR1_TXIE | I2C_CR1_RXIE |
                    I2C_CR1_TCIE | I2C_CR1_STOPIE | I2C_CR1_NACKIE |
                    I2C_CR1_ERRIE);

  return OK;
}

/****************************************************************************
 * Name: stm32_i2c_shutdown
 *
 * Description:
 *   Shutdown the I2C hardware
 *
 ****************************************************************************/

static int stm32_i2c_shutdown(struct i2c_master_s *dev)
{
  FAR struct stm32_i2c_priv_s *priv = (FAR struct stm32_i2c_priv_s *)dev;

  DEBUGASSERT(priv != NULL);

  i2cinfo("I2C%d: Shutdown\n", priv->port);

  /* Disable I2C */

  stm32_i2c_putreg(priv, STM32_I2C_CR1_OFFSET, 0);

  return OK;
}

/****************************************************************************
 * Name: stm32_i2c_transfer
 *
 * Description:
 *   Perform a sequence of I2C transfers
 *
 ****************************************************************************/

static int stm32_i2c_transfer(struct i2c_master_s *dev,
                               struct i2c_msg_s *msgs, int count)
{
  FAR struct stm32_i2c_priv_s *priv = (FAR struct stm32_i2c_priv_s *)dev;
  FAR struct i2c_msg_s *msg;
  int ret;

  DEBUGASSERT(priv != NULL && msgs != NULL && count > 0);

  i2cinfo("I2C%d: Transfer %d messages\n", priv->port, count);

  /* Get exclusive access to the I2C bus */

  ret = nxmutex_lock(&priv->lock);
  if (ret < 0)
    {
      return ret;
    }

  /* Save message information */

  priv->msgs = msgs;
  priv->msgcount = count;
  priv->msgidx = 0;
  priv->result = -ETIMEDOUT;

  /* Check if frequency needs to be updated */

  msg = &msgs[0];
  if (msg->frequency != priv->frequency && msg->frequency > 0)
    {
      uint32_t timing = stm32_i2c_timing(msg->frequency);

      if (timing != priv->timing)
        {
          /* Disable I2C */

          stm32_i2c_modifyreg(priv, STM32_I2C_CR1_OFFSET, I2C_CR1_PE, 0);

          /* Update timing */

          stm32_i2c_putreg(priv, STM32_I2C_TIMINGR_OFFSET, timing);
          priv->timing = timing;
          priv->frequency = msg->frequency;

          /* Re-enable I2C */

          stm32_i2c_modifyreg(priv, STM32_I2C_CR1_OFFSET, 0, I2C_CR1_PE);
        }
    }

  /* Initialize transfer state */

  msg = &msgs[0];
  priv->ptr = msg->buffer;
  priv->remaining = msg->length;
  priv->firstbyte = true;

  /* Enable I2C if not already enabled */

  stm32_i2c_modifyreg(priv, STM32_I2C_CR1_OFFSET, 0, I2C_CR1_PE);

  /* Send START condition and slave address */

  ret = stm32_i2c_sendstart(priv);
  if (ret < 0)
    {
      goto done;
    }

  /* Wait for transfer completion */

  ret = stm32_i2c_waitforcompletion(priv);

done:
  /* Release the I2C bus */

  nxmutex_unlock(&priv->lock);

  return ret;
}

/****************************************************************************
 * Name: stm32_i2c_reset
 *
 * Description:
 *   Perform an I2C bus reset
 *
 ****************************************************************************/

#ifdef CONFIG_I2C_RESET
static int stm32_i2c_reset(struct i2c_master_s *dev)
{
  FAR struct stm32_i2c_priv_s *priv = (FAR struct stm32_i2c_priv_s *)dev;
  int ret;

  DEBUGASSERT(priv != NULL);

  i2cinfo("I2C%d: Reset\n", priv->port);

  /* Get exclusive access */

  ret = nxmutex_lock(&priv->lock);
  if (ret < 0)
    {
      return ret;
    }

  /* Disable I2C */

  stm32_i2c_putreg(priv, STM32_I2C_CR1_OFFSET, 0);

  /* Short delay */

  up_mdelay(10);

  /* Re-enable I2C */

  stm32_i2c_putreg(priv, STM32_I2C_CR1_OFFSET,
                    I2C_CR1_PE | I2C_CR1_TXIE | I2C_CR1_RXIE |
                    I2C_CR1_TCIE | I2C_CR1_STOPIE | I2C_CR1_NACKIE |
                    I2C_CR1_ERRIE);

  nxmutex_unlock(&priv->lock);

  return OK;
}
#endif

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: stm32_i2cbus_initialize
 *
 * Description:
 *   Initialize the selected I2C port
 *
 ****************************************************************************/

FAR struct i2c_master_s *stm32_i2cbus_initialize(int port)
{
  FAR struct stm32_i2c_priv_s *priv;
  irqstate_t flags;
  int irq;
  int irq_err;

  i2cinfo("I2C%d: Initialize\n", port);

  /* Get the private data structure for this port */

  switch (port)
    {
#ifdef CONFIG_STM32N6_I2C1
      case 1:
        priv = &g_i2c1_priv;
        irq = STM32N6_IRQ_I2C1;
        irq_err = STM32N6_IRQ_I2C1_ERR;
        break;
#endif

#ifdef CONFIG_STM32N6_I2C2
      case 2:
        priv = &g_i2c2_priv;
        irq = STM32N6_IRQ_I2C2;
        irq_err = STM32N6_IRQ_I2C2_ERR;
        break;
#endif

#ifdef CONFIG_STM32N6_I2C3
      case 3:
        priv = &g_i2c3_priv;
        irq = STM32N6_IRQ_I2C3;
        irq_err = STM32N6_IRQ_I2C3_ERR;
        break;
#endif

#ifdef CONFIG_STM32N6_I2C4
      case 4:
        priv = &g_i2c4_priv;
        irq = STM32N6_IRQ_I2C4;
        irq_err = STM32N6_IRQ_I2C4_ERR;
        break;
#endif

      default:
        i2cerr("ERROR: Unsupported I2C port: %d\n", port);
        return NULL;
    }

  /* Initialize the mutex and semaphore */

  nxmutex_init(&priv->lock);
  nxsem_init(&priv->sem_isr, 0, 0);

  /* Enable I2C peripheral clock and configure GPIO pins.
   * I2C1/2/3 are on APB1 bus.
   */

  switch (port)
    {
#ifdef CONFIG_STM32N6_I2C1
      case 1:
        {
          /* Enable GPIOB clock for I2C1 pins (PB6=SCL, PB7=SDA) */

          rcc_enable_ahb4_clock(RCC_AHB4ENR_GPIOBEN);

          /* Configure PB6 as I2C1_SCL (AF4, open-drain, pull-up) */

          gpio_set_mode(STM32_GPIOB_BASE, 6, GPIO_MODER_AF);
          gpio_set_af(STM32_GPIOB_BASE, 6, GPIO_AF4);
          gpio_set_output_type(STM32_GPIOB_BASE, 6, GPIO_OTYPER_OD);
          gpio_set_speed(STM32_GPIOB_BASE, 6, GPIO_OSPEEDR_HIGH);
          gpio_set_pupd(STM32_GPIOB_BASE, 6, GPIO_PUPDR_UP);

          /* Configure PB7 as I2C1_SDA (AF4, open-drain, pull-up) */

          gpio_set_mode(STM32_GPIOB_BASE, 7, GPIO_MODER_AF);
          gpio_set_af(STM32_GPIOB_BASE, 7, GPIO_AF4);
          gpio_set_output_type(STM32_GPIOB_BASE, 7, GPIO_OTYPER_OD);
          gpio_set_speed(STM32_GPIOB_BASE, 7, GPIO_OSPEEDR_HIGH);
          gpio_set_pupd(STM32_GPIOB_BASE, 7, GPIO_PUPDR_UP);

          rcc_enable_apb1_clock(RCC_APB1ENR1_I2C1EN);
          break;
        }
#endif

#ifdef CONFIG_STM32N6_I2C2
      case 2:
        {
          /* Enable GPIOB clock for I2C2 pins (PB10=SCL, PB11=SDA) */

          rcc_enable_ahb4_clock(RCC_AHB4ENR_GPIOBEN);

          /* Configure PB10 as I2C2_SCL (AF4, open-drain, pull-up) */

          gpio_set_mode(STM32_GPIOB_BASE, 10, GPIO_MODER_AF);
          gpio_set_af(STM32_GPIOB_BASE, 10, GPIO_AF4);
          gpio_set_output_type(STM32_GPIOB_BASE, 10, GPIO_OTYPER_OD);
          gpio_set_speed(STM32_GPIOB_BASE, 10, GPIO_OSPEEDR_HIGH);
          gpio_set_pupd(STM32_GPIOB_BASE, 10, GPIO_PUPDR_UP);

          /* Configure PB11 as I2C2_SDA (AF4, open-drain, pull-up) */

          gpio_set_mode(STM32_GPIOB_BASE, 11, GPIO_MODER_AF);
          gpio_set_af(STM32_GPIOB_BASE, 11, GPIO_AF4);
          gpio_set_output_type(STM32_GPIOB_BASE, 11, GPIO_OTYPER_OD);
          gpio_set_speed(STM32_GPIOB_BASE, 11, GPIO_OSPEEDR_HIGH);
          gpio_set_pupd(STM32_GPIOB_BASE, 11, GPIO_PUPDR_UP);

          rcc_enable_apb1_clock(RCC_APB1ENR1_I2C2EN);
          break;
        }
#endif

#ifdef CONFIG_STM32N6_I2C3
      case 3:
        {
          /* Enable GPIOA and GPIOB clocks for I2C3 pins
           * (PA7=SCL, PB5=SDA)
           */

          rcc_enable_ahb4_clock(RCC_AHB4ENR_GPIOAEN |
                                RCC_AHB4ENR_GPIOBEN);

          /* Configure PA7 as I2C3_SCL (AF4, open-drain, pull-up) */

          gpio_set_mode(STM32_GPIOA_BASE, 7, GPIO_MODER_AF);
          gpio_set_af(STM32_GPIOA_BASE, 7, GPIO_AF4);
          gpio_set_output_type(STM32_GPIOA_BASE, 7, GPIO_OTYPER_OD);
          gpio_set_speed(STM32_GPIOA_BASE, 7, GPIO_OSPEEDR_HIGH);
          gpio_set_pupd(STM32_GPIOA_BASE, 7, GPIO_PUPDR_UP);

          /* Configure PB5 as I2C3_SDA (AF4, open-drain, pull-up) */

          gpio_set_mode(STM32_GPIOB_BASE, 5, GPIO_MODER_AF);
          gpio_set_af(STM32_GPIOB_BASE, 5, GPIO_AF4);
          gpio_set_output_type(STM32_GPIOB_BASE, 5, GPIO_OTYPER_OD);
          gpio_set_speed(STM32_GPIOB_BASE, 5, GPIO_OSPEEDR_HIGH);
          gpio_set_pupd(STM32_GPIOB_BASE, 5, GPIO_PUPDR_UP);

          rcc_enable_apb1_clock(RCC_APB1ENR1_I2C3EN);
          break;
        }
#endif

      default:
        break;
    }

  /* Attach both event and error interrupt handlers */

  flags = enter_critical_section();
  irq_attach(irq, stm32_i2c_isr_handler, priv);
  up_enable_irq(irq);
  irq_attach(irq_err, stm32_i2c_isr_handler, priv);
  up_enable_irq(irq_err);
  leave_critical_section(flags);

  return &priv->dev;
}

/****************************************************************************
 * Name: stm32_i2cbus_uninitialize
 *
 * Description:
 *   Uninitialize an I2C bus
 *
 ****************************************************************************/

int stm32_i2cbus_uninitialize(FAR struct i2c_master_s *dev)
{
  FAR struct stm32_i2c_priv_s *priv = (FAR struct stm32_i2c_priv_s *)dev;
  irqstate_t flags;
  int irq;
  int irq_err;

  DEBUGASSERT(dev != NULL);

  i2cinfo("I2C%d: Uninitialize\n", priv->port);

  /* Shutdown the I2C hardware */

  stm32_i2c_shutdown(dev);

  /* Disable and detach both event and error interrupts */

  switch (priv->port)
    {
#ifdef CONFIG_STM32N6_I2C1
      case 1:
        irq = STM32N6_IRQ_I2C1;
        irq_err = STM32N6_IRQ_I2C1_ERR;
        break;
#endif

#ifdef CONFIG_STM32N6_I2C2
      case 2:
        irq = STM32N6_IRQ_I2C2;
        irq_err = STM32N6_IRQ_I2C2_ERR;
        break;
#endif

#ifdef CONFIG_STM32N6_I2C3
      case 3:
        irq = STM32N6_IRQ_I2C3;
        irq_err = STM32N6_IRQ_I2C3_ERR;
        break;
#endif

#ifdef CONFIG_STM32N6_I2C4
      case 4:
        irq = STM32N6_IRQ_I2C4;
        irq_err = STM32N6_IRQ_I2C4_ERR;
        break;
#endif

      default:
        return -EINVAL;
    }

  flags = enter_critical_section();
  up_disable_irq(irq);
  irq_detach(irq);
  up_disable_irq(irq_err);
  irq_detach(irq_err);
  leave_critical_section(flags);

  /* Destroy the mutex and semaphore */

  nxmutex_destroy(&priv->lock);
  nxsem_destroy(&priv->sem_isr);

  return OK;
}
