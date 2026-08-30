/****************************************************************************
 * arch/arm/src/stm32n6/stm32_uart.c
 *
 * SPDX-License-Identifier: Apache-2.0
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include <nuttx/serial/serial.h>

#include <sys/types.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include <debug.h>
#include <nuttx/irq.h>
#include <nuttx/arch.h>
#include <arch/board/board.h>

#include "arm_internal.h"
#include "hardware/stm32_usart.h"
#include "hardware/stm32_rcc.h"
#include "hardware/stm32_gpio.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* USART base addresses */

#define USART1_BASE  0x40011000
#define USART2_BASE  0x40004400
#define USART3_BASE  0x40004800

/* Helper macros for register access */

#define USART_REG(base, offset) \
  (*(volatile uint32_t *)((base) + (offset)))

/* Default configuration for USART1 */

#ifndef CONFIG_STM32N6_USART1_BAUD
#  define CONFIG_STM32N6_USART1_BAUD 115200
#endif

#ifndef CONFIG_STM32N6_USART1_BITS
#  define CONFIG_STM32N6_USART1_BITS 8
#endif

#ifndef CONFIG_STM32N6_USART1_PARITY
#  define CONFIG_STM32N6_USART1_PARITY 0
#endif

#ifndef CONFIG_STM32N6_USART1_STOPBITS
#  define CONFIG_STM32N6_USART1_STOPBITS 1
#endif

#ifndef CONFIG_STM32N6_USART1_RXBUFSIZE
#  define CONFIG_STM32N6_USART1_RXBUFSIZE 256
#endif

#ifndef CONFIG_STM32N6_USART1_TXBUFSIZE
#  define CONFIG_STM32N6_USART1_TXBUFSIZE 256
#endif

/* Default configuration for USART2 */

#ifndef CONFIG_STM32N6_USART2_BAUD
#  define CONFIG_STM32N6_USART2_BAUD 115200
#endif

#ifndef CONFIG_STM32N6_USART2_BITS
#  define CONFIG_STM32N6_USART2_BITS 8
#endif

#ifndef CONFIG_STM32N6_USART2_PARITY
#  define CONFIG_STM32N6_USART2_PARITY 0
#endif

#ifndef CONFIG_STM32N6_USART2_STOPBITS
#  define CONFIG_STM32N6_USART2_STOPBITS 1
#endif

#ifndef CONFIG_STM32N6_USART2_RXBUFSIZE
#  define CONFIG_STM32N6_USART2_RXBUFSIZE 256
#endif

#ifndef CONFIG_STM32N6_USART2_TXBUFSIZE
#  define CONFIG_STM32N6_USART2_TXBUFSIZE 256
#endif

/* Default configuration for USART3 */

#ifndef CONFIG_STM32N6_USART3_BAUD
#  define CONFIG_STM32N6_USART3_BAUD 115200
#endif

#ifndef CONFIG_STM32N6_USART3_BITS
#  define CONFIG_STM32N6_USART3_BITS 8
#endif

#ifndef CONFIG_STM32N6_USART3_PARITY
#  define CONFIG_STM32N6_USART3_PARITY 0
#endif

#ifndef CONFIG_STM32N6_USART3_STOPBITS
#  define CONFIG_STM32N6_USART3_STOPBITS 1
#endif

#ifndef CONFIG_STM32N6_USART3_RXBUFSIZE
#  define CONFIG_STM32N6_USART3_RXBUFSIZE 256
#endif

#ifndef CONFIG_STM32N6_USART3_TXBUFSIZE
#  define CONFIG_STM32N6_USART3_TXBUFSIZE 256
#endif

/****************************************************************************
 * Private Types
 ****************************************************************************/

struct stm32_uart_priv_s
{
  uint32_t base;       /* USART base address */
  int      irq;        /* USART IRQ number */
  uint32_t pclk_freq;  /* Peripheral clock frequency (Hz) */
  uint32_t baud;       /* Baud rate */
  int      bits;       /* Data bits (7, 8, 9) */
  int      parity;     /* Parity (0=none, 1=odd, 2=even) */
  int      stopbits;   /* Stop bits (1 or 2) */
};

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

static int  stm32_uart_setup(struct uart_dev_s *dev);
static void stm32_uart_shutdown(struct uart_dev_s *dev);
static int  stm32_uart_attach(struct uart_dev_s *dev);
static void stm32_uart_detach(struct uart_dev_s *dev);
static int  stm32_uart_ioctl(struct file *filep, int cmd, unsigned long arg);
static bool stm32_uart_txready(struct uart_dev_s *dev);
static void stm32_uart_send(struct uart_dev_s *dev, int ch);
static bool stm32_uart_rxavailable(struct uart_dev_s *dev);
static int  stm32_uart_receive(struct uart_dev_s *dev, unsigned int *status);
static void stm32_uart_rxint(struct uart_dev_s *dev, bool enable);
static void stm32_uart_txint(struct uart_dev_s *dev, bool enable);
static bool stm32_uart_txempty(struct uart_dev_s *dev);

/* Forward declarations of interrupt handlers */

#ifdef CONFIG_STM32N6_USART1
int stm32_uart1_interrupt(int irq, void *context, void *arg);
#endif
#ifdef CONFIG_STM32N6_USART2
int stm32_uart2_interrupt(int irq, void *context, void *arg);
#endif
#ifdef CONFIG_STM32N6_USART3
int stm32_uart3_interrupt(int irq, void *context, void *arg);
#endif

/****************************************************************************
 * Private Data
 ****************************************************************************/

static const struct uart_ops_s g_uart_ops =
{
  .setup       = stm32_uart_setup,
  .shutdown    = stm32_uart_shutdown,
  .attach      = stm32_uart_attach,
  .detach      = stm32_uart_detach,
  .ioctl       = stm32_uart_ioctl,
  .txready     = stm32_uart_txready,
  .send        = stm32_uart_send,
  .rxavailable = stm32_uart_rxavailable,
  .receive     = stm32_uart_receive,
  .rxint       = stm32_uart_rxint,
  .txint       = stm32_uart_txint,
  .txempty     = stm32_uart_txempty,
};

/* USART1 private data and port */

#ifdef CONFIG_STM32N6_USART1
static struct stm32_uart_priv_s g_uart1_priv =
{
  .base     = USART1_BASE,
  .irq      = STM32N6_USART1_IRQ,
  .pclk_freq = STM32N6_PCLK2_FREQUENCY,
  .baud     = CONFIG_STM32N6_USART1_BAUD,
  .bits     = CONFIG_STM32N6_USART1_BITS,
  .parity   = CONFIG_STM32N6_USART1_PARITY,
  .stopbits = CONFIG_STM32N6_USART1_STOPBITS,
};

static char g_uart1rxbuffer[CONFIG_STM32N6_USART1_RXBUFSIZE];
static char g_uart1txbuffer[CONFIG_STM32N6_USART1_TXBUFSIZE];

struct uart_dev_s g_uart1port =
{
#ifdef CONFIG_STM32N6_USART1_SERIAL_CONSOLE
  .isconsole = true,
#else
  .isconsole = false,
#endif
  .ops       = &g_uart_ops,
  .priv      = &g_uart1_priv,
  .xmit =
  {
    .buffer  = g_uart1txbuffer,
    .size    = sizeof(g_uart1txbuffer),
    .head    = 0,
    .tail    = 0,
  },
  .recv =
  {
    .buffer  = g_uart1rxbuffer,
    .size    = sizeof(g_uart1rxbuffer),
    .head    = 0,
    .tail    = 0,
  },
};
#endif /* CONFIG_STM32N6_USART1 */

/* USART2 private data and port */

#ifdef CONFIG_STM32N6_USART2
static struct stm32_uart_priv_s g_uart2_priv =
{
  .base     = USART2_BASE,
  .irq      = STM32N6_USART2_IRQ,
  .pclk_freq = STM32N6_PCLK1_FREQUENCY,
  .baud     = CONFIG_STM32N6_USART2_BAUD,
  .bits     = CONFIG_STM32N6_USART2_BITS,
  .parity   = CONFIG_STM32N6_USART2_PARITY,
  .stopbits = CONFIG_STM32N6_USART2_STOPBITS,
};

static char g_uart2rxbuffer[CONFIG_STM32N6_USART2_RXBUFSIZE];
static char g_uart2txbuffer[CONFIG_STM32N6_USART2_TXBUFSIZE];

struct uart_dev_s g_uart2port =
{
#ifdef CONFIG_STM32N6_USART2_SERIAL_CONSOLE
  .isconsole = true,
#else
  .isconsole = false,
#endif
  .ops       = &g_uart_ops,
  .priv      = &g_uart2_priv,
  .xmit =
  {
    .buffer  = g_uart2txbuffer,
    .size    = sizeof(g_uart2txbuffer),
    .head    = 0,
    .tail    = 0,
  },
  .recv =
  {
    .buffer  = g_uart2rxbuffer,
    .size    = sizeof(g_uart2rxbuffer),
    .head    = 0,
    .tail    = 0,
  },
};
#endif /* CONFIG_STM32N6_USART2 */

/* USART3 private data and port */

#ifdef CONFIG_STM32N6_USART3
static struct stm32_uart_priv_s g_uart3_priv =
{
  .base     = USART3_BASE,
  .irq      = STM32N6_USART3_IRQ,
  .pclk_freq = STM32N6_PCLK1_FREQUENCY,
  .baud     = CONFIG_STM32N6_USART3_BAUD,
  .bits     = CONFIG_STM32N6_USART3_BITS,
  .parity   = CONFIG_STM32N6_USART3_PARITY,
  .stopbits = CONFIG_STM32N6_USART3_STOPBITS,
};

static char g_uart3rxbuffer[CONFIG_STM32N6_USART3_RXBUFSIZE];
static char g_uart3txbuffer[CONFIG_STM32N6_USART3_TXBUFSIZE];

struct uart_dev_s g_uart3port =
{
#ifdef CONFIG_STM32N6_USART3_SERIAL_CONSOLE
  .isconsole = true,
#else
  .isconsole = false,
#endif
  .ops       = &g_uart_ops,
  .priv      = &g_uart3_priv,
  .xmit =
  {
    .buffer  = g_uart3txbuffer,
    .size    = sizeof(g_uart3txbuffer),
    .head    = 0,
    .tail    = 0,
  },
  .recv =
  {
    .buffer  = g_uart3rxbuffer,
    .size    = sizeof(g_uart3rxbuffer),
    .head    = 0,
    .tail    = 0,
  },
};
#endif /* CONFIG_STM32N6_USART3 */

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: stm32_uart_configure
 *
 * Description:
 *   Configure a USART with the specified parameters.
 *
 ****************************************************************************/

static void stm32_uart_configure(struct stm32_uart_priv_s *priv)
{
  uint32_t cr1 = 0;
  uint32_t cr2 = 0;
  uint32_t brr;

  /* Enable peripheral clock based on bus assignment */

  if (priv->base == USART1_BASE)
    {
      rcc_enable_apb2_clock(RCC_APB2ENR_USART1EN);
    }
  else
    {
      /* USART2/3 are on APB1 */

      uint32_t clk_mask = 0;

      if (priv->base == USART2_BASE)
        {
          clk_mask = RCC_APB1ENR1_USART2EN;
        }
      else if (priv->base == USART3_BASE)
        {
          clk_mask = RCC_APB1ENR1_USART3EN;
        }

      if (clk_mask != 0)
        {
          rcc_enable_apb1_clock(clk_mask);
        }
    }

  /* Configure CR1: UE, TE, RE */

  cr1 = USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;

  /* Configure data bits */

  if (priv->bits == 9)
    {
      cr1 |= USART_CR1_M0;
    }
  else if (priv->bits == 7)
    {
      cr1 |= USART_CR1_M1;
    }
  /* 8 bits: M0=0, M1=0 (default) */

  /* Configure parity */

  if (priv->parity == 1)  /* Odd parity */
    {
      cr1 |= USART_CR1_PCE | USART_CR1_PS;
    }
  else if (priv->parity == 2)  /* Even parity */
    {
      cr1 |= USART_CR1_PCE;
    }

  /* Configure stop bits in CR2 */

  if (priv->stopbits == 2)
    {
      cr2 |= (0x2 << 12);  /* STOP[1:0] = 10 for 2 stop bits */
    }

  /* Set registers */

  USART_REG(priv->base, STM32_USART_CR1_OFFSET) = cr1;
  USART_REG(priv->base, STM32_USART_CR2_OFFSET) = cr2;

  /* Calculate and set baud rate
   * BRR = fck / baud
   */

  brr = priv->pclk_freq / priv->baud;
  USART_REG(priv->base, STM32_USART_BRR_OFFSET) = brr;
}

/****************************************************************************
 * Name: stm32_uart_setup
 *
 * Description:
 *   Configure a USART port.
 *
 ****************************************************************************/

static int stm32_uart_setup(struct uart_dev_s *dev)
{
  struct stm32_uart_priv_s *priv =
    (struct stm32_uart_priv_s *)dev->priv;

  stm32_uart_configure(priv);

  return OK;
}

/****************************************************************************
 * Name: stm32_uart_shutdown
 *
 * Description:
 *   Disable a USART port.
 *
 ****************************************************************************/

static void stm32_uart_shutdown(struct uart_dev_s *dev)
{
  struct stm32_uart_priv_s *priv =
    (struct stm32_uart_priv_s *)dev->priv;

  /* Disable all interrupts and USART */

  USART_REG(priv->base, STM32_USART_CR1_OFFSET) = 0;
}

/****************************************************************************
 * Name: stm32_uart_attach
 *
 * Description:
 *   Configure interrupts and attach the USART interrupt handler.
 *
 ****************************************************************************/

static int stm32_uart_attach(struct uart_dev_s *dev)
{
  struct stm32_uart_priv_s *priv =
    (struct stm32_uart_priv_s *)dev->priv;

  /* Attach the appropriate interrupt handler based on base address */

  xcpt_t handler = NULL;

#ifdef CONFIG_STM32N6_USART1
  if (priv->base == USART1_BASE)
    {
      handler = stm32_uart1_interrupt;
    }
#endif
#ifdef CONFIG_STM32N6_USART2
  if (priv->base == USART2_BASE)
    {
      handler = stm32_uart2_interrupt;
    }
#endif
#ifdef CONFIG_STM32N6_USART3
  if (priv->base == USART3_BASE)
    {
      handler = stm32_uart3_interrupt;
    }
#endif

  if (handler != NULL)
    {
      irq_attach(priv->irq, handler, NULL);
      up_enable_irq(priv->irq);
    }

  return OK;
}

/****************************************************************************
 * Name: stm32_uart_detach
 *
 * Description:
 *   Detach the USART interrupt handler.
 *
 ****************************************************************************/

static void stm32_uart_detach(struct uart_dev_s *dev)
{
  struct stm32_uart_priv_s *priv =
    (struct stm32_uart_priv_s *)dev->priv;

  up_disable_irq(priv->irq);
  irq_detach(priv->irq);
}

/****************************************************************************
 * Name: stm32_uart_ioctl
 *
 * Description:
 *   Handle ioctl commands.
 *
 ****************************************************************************/

static int stm32_uart_ioctl(struct file *filep, int cmd, unsigned long arg)
{
  return -ENOTTY;
}

/****************************************************************************
 * Name: stm32_uart_txready
 *
 * Description:
 *   Check if TX buffer is empty (TXE flag).
 *
 ****************************************************************************/

static bool stm32_uart_txready(struct uart_dev_s *dev)
{
  struct stm32_uart_priv_s *priv =
    (struct stm32_uart_priv_s *)dev->priv;

  return (USART_REG(priv->base, STM32_USART_ISR_OFFSET) &
          USART_ISR_TXE) != 0;
}

/****************************************************************************
 * Name: stm32_uart_send
 *
 * Description:
 *   Send one character.
 *
 ****************************************************************************/

static void stm32_uart_send(struct uart_dev_s *dev, int ch)
{
  struct stm32_uart_priv_s *priv =
    (struct stm32_uart_priv_s *)dev->priv;

  USART_REG(priv->base, STM32_USART_TDR_OFFSET) = (uint32_t)ch;
}

/****************************************************************************
 * Name: stm32_uart_rxavailable
 *
 * Description:
 *   Check if there is received data (RXNE flag).
 *
 ****************************************************************************/

static bool stm32_uart_rxavailable(struct uart_dev_s *dev)
{
  struct stm32_uart_priv_s *priv =
    (struct stm32_uart_priv_s *)dev->priv;

  return (USART_REG(priv->base, STM32_USART_ISR_OFFSET) &
          USART_ISR_RXNE) != 0;
}

/****************************************************************************
 * Name: stm32_uart_receive
 *
 * Description:
 *   Receive one character.
 *
 ****************************************************************************/

static int stm32_uart_receive(struct uart_dev_s *dev, unsigned int *status)
{
  struct stm32_uart_priv_s *priv =
    (struct stm32_uart_priv_s *)dev->priv;

  *status = USART_REG(priv->base, STM32_USART_ISR_OFFSET);
  return (int)(USART_REG(priv->base, STM32_USART_RDR_OFFSET) & 0xFF);
}

/****************************************************************************
 * Name: stm32_uart_rxint
 *
 * Description:
 *   Enable/disable RXNE interrupt.
 *
 ****************************************************************************/

static void stm32_uart_rxint(struct uart_dev_s *dev, bool enable)
{
  struct stm32_uart_priv_s *priv =
    (struct stm32_uart_priv_s *)dev->priv;

  uint32_t cr1 = USART_REG(priv->base, STM32_USART_CR1_OFFSET);

  if (enable)
    {
      cr1 |= USART_CR1_RXNEIE;
    }
  else
    {
      cr1 &= ~USART_CR1_RXNEIE;
    }

  USART_REG(priv->base, STM32_USART_CR1_OFFSET) = cr1;
}

/****************************************************************************
 * Name: stm32_uart_txint
 *
 * Description:
 *   Enable/disable TXE interrupt.
 *
 ****************************************************************************/

static void stm32_uart_txint(struct uart_dev_s *dev, bool enable)
{
  struct stm32_uart_priv_s *priv =
    (struct stm32_uart_priv_s *)dev->priv;

  uint32_t cr1 = USART_REG(priv->base, STM32_USART_CR1_OFFSET);

  if (enable)
    {
      cr1 |= USART_CR1_TXEIE;
    }
  else
    {
      cr1 &= ~USART_CR1_TXEIE;
    }

  USART_REG(priv->base, STM32_USART_CR1_OFFSET) = cr1;
}

/****************************************************************************
 * Name: stm32_uart_txempty
 *
 * Description:
 *   Check if TX is completely done (TC flag).
 *
 ****************************************************************************/

static bool stm32_uart_txempty(struct uart_dev_s *dev)
{
  struct stm32_uart_priv_s *priv =
    (struct stm32_uart_priv_s *)dev->priv;

  return (USART_REG(priv->base, STM32_USART_ISR_OFFSET) &
          USART_ISR_TC) != 0;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: stm32_uart1_interrupt
 *
 * Description:
 *   USART1 interrupt handler.
 *
 ****************************************************************************/

#ifdef CONFIG_STM32N6_USART1
int stm32_uart1_interrupt(int irq, void *context, void *arg)
{
  uint32_t status = USART_REG(USART1_BASE, STM32_USART_ISR_OFFSET);

  /* Handle RXNE - received data */

  if (status & USART_ISR_RXNE)
    {
      uart_recvchars(&g_uart1port);
    }

  /* Handle TXE - transmit buffer empty */

  if (status & USART_ISR_TXE)
    {
      uart_xmitchars(&g_uart1port);
    }

  return OK;
}
#endif /* CONFIG_STM32N6_USART1 */

/****************************************************************************
 * Name: stm32_uart2_interrupt
 *
 * Description:
 *   USART2 interrupt handler.
 *
 ****************************************************************************/

#ifdef CONFIG_STM32N6_USART2
int stm32_uart2_interrupt(int irq, void *context, void *arg)
{
  uint32_t status = USART_REG(USART2_BASE, STM32_USART_ISR_OFFSET);

  /* Handle RXNE - received data */

  if (status & USART_ISR_RXNE)
    {
      uart_recvchars(&g_uart2port);
    }

  /* Handle TXE - transmit buffer empty */

  if (status & USART_ISR_TXE)
    {
      uart_xmitchars(&g_uart2port);
    }

  return OK;
}
#endif /* CONFIG_STM32N6_USART2 */

/****************************************************************************
 * Name: stm32_uart3_interrupt
 *
 * Description:
 *   USART3 interrupt handler.
 *
 ****************************************************************************/

#ifdef CONFIG_STM32N6_USART3
int stm32_uart3_interrupt(int irq, void *context, void *arg)
{
  uint32_t status = USART_REG(USART3_BASE, STM32_USART_ISR_OFFSET);

  /* Handle RXNE - received data */

  if (status & USART_ISR_RXNE)
    {
      uart_recvchars(&g_uart3port);
    }

  /* Handle TXE - transmit buffer empty */

  if (status & USART_ISR_TXE)
    {
      uart_xmitchars(&g_uart3port);
    }

  return OK;
}
#endif /* CONFIG_STM32N6_USART3 */
