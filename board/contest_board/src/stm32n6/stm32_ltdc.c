/****************************************************************************
 * arch/arm/src/stm32n6/stm32_ltdc.c
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

/* References:
 *   STM32N6 Reference Manual
 *   STM32CubeN6 LTDC examples
 */

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <debug.h>

#include <nuttx/irq.h>
#include <nuttx/kmalloc.h>
#include <nuttx/semaphore.h>
#include <nuttx/video/fb.h>

#include <arch/board/board.h>

#include "arm_internal.h"
#include "hardware/stm32_ltdc.h"
#include "hardware/stm32_rcc.h"
#include "stm32_gpio.h"
#include "stm32_ltdc.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* LTDC configuration - must be defined in board.h */

#ifndef BOARD_LTDC_WIDTH
#  define BOARD_LTDC_WIDTH  800
#endif

#ifndef BOARD_LTDC_HEIGHT
#  define BOARD_LTDC_HEIGHT 480
#endif

#ifndef BOARD_LTDC_HSYNC
#  define BOARD_LTDC_HSYNC  4
#endif

#ifndef BOARD_LTDC_VSYNC
#  define BOARD_LTDC_VSYNC  4
#endif

#ifndef BOARD_LTDC_HBP
#  define BOARD_LTDC_HBP    8
#endif

#ifndef BOARD_LTDC_VBP
#  define BOARD_LTDC_VBP    8
#endif

#ifndef BOARD_LTDC_HFP
#  define BOARD_LTDC_HFP    8
#endif

#ifndef BOARD_LTDC_VFP
#  define BOARD_LTDC_VFP    8
#endif

/* Pixel format */

#define LTDC_PIXEL_FORMAT_ARGB8888  0
#define LTDC_PIXEL_FORMAT_RGB888    1
#define LTDC_PIXEL_FORMAT_RGB565    2

/* Framebuffer size - double buffered */

#define LTDC_FRAMEBUFFER_SIZE  (BOARD_LTDC_WIDTH * BOARD_LTDC_HEIGHT * 2)

/****************************************************************************
 * Private Types
 ****************************************************************************/

/* LTDC device private data structure */

struct stm32_ltdc_priv_s
{
  struct fb_vtable_s vtable;    /* Framebuffer vtable */
  uint8_t *fbmem;              /* Framebuffer memory */
  uint16_t width;              /* Display width */
  uint16_t height;             /* Display height */
  uint8_t bpp;                 /* Bits per pixel */
  uint8_t pixfmt;              /* Pixel format */
};

/****************************************************************************
 * Private Data
 ****************************************************************************/

/* LTDC device instance */

static struct stm32_ltdc_priv_s g_ltdc_priv;

/* Framebuffer memory - statically allocated */

static uint8_t g_ltdc_fb[LTDC_FRAMEBUFFER_SIZE]
  __attribute__((aligned(32)));

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

/* LTDC register access */

static inline uint32_t ltdc_getreg(uint32_t offset);
static inline void ltdc_putreg(uint32_t offset, uint32_t value);
static inline void ltdc_modifyreg(uint32_t offset, uint32_t clearbits,
                                   uint32_t setbits);

/* LTDC configuration */

static void ltdc_configure_clocks(void);
static void ltdc_configure_pins(void);
static void ltdc_configure_timing(void);
static void ltdc_configure_layer(void);

/* Framebuffer operations */

static int ltdc_getvideoinfo(struct fb_vtable_s *vplane,
                              struct fb_videoinfo_s *vinfo);
static int ltdc_getplaneinfo(struct fb_vtable_s *vplane, int planeno,
                              struct fb_planeinfo_s *pinfo);
#ifdef CONFIG_FB_HWCURSOR
static int ltdc_setcursor(struct fb_vtable_s *vplane,
                           struct fb_setcursor_s *settings);
#endif

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: ltdc_getreg
 *
 * Description:
 *   Read an LTDC register
 *
 ****************************************************************************/

static inline uint32_t ltdc_getreg(uint32_t offset)
{
  return getreg32(STM32_LTDC_BASE + offset);
}

/****************************************************************************
 * Name: ltdc_putreg
 *
 * Description:
 *   Write an LTDC register
 *
 ****************************************************************************/

static inline void ltdc_putreg(uint32_t offset, uint32_t value)
{
  putreg32(value, STM32_LTDC_BASE + offset);
}

/****************************************************************************
 * Name: ltdc_modifyreg
 *
 * Description:
 *   Modify an LTDC register
 *
 ****************************************************************************/

static inline void ltdc_modifyreg(uint32_t offset, uint32_t clearbits,
                                   uint32_t setbits)
{
  modifyreg32(STM32_LTDC_BASE + offset, clearbits, setbits);
}

/****************************************************************************
 * Name: ltdc_configure_clocks
 *
 * Description:
 *   Configure LTDC clocks using PLL4
 *   Reference: STM32N6570-DK LTDC example
 *
 ****************************************************************************/

static void ltdc_configure_clocks(void)
{
  /* Enable LTDC peripheral clock (LTDC is on APB5) */

  rcc_enable_apb5_clock(RCC_APB5ENR_LTDCEN);

  /* Configure LTDC pixel clock via IC16
   * Clock chain: PLL1 -> IC16 -> LTDC
   * PLL1 runs at 600MHz (configured in stm32_clockconfig.c)
   * For 800x480 panel: pixel clock = 33.333 MHz
   * IC16 divider = 600 / 33.333 = 18
   *
   * Reference: rgblcd.c:828-842
   */

  /* IC16CFGR: IC16 source = PLL1, divider = 18 */

  modifyreg32(STM32_RCC_BASE + RCC_IC16CFGR_OFFSET, 0,
               RCC_IC16CFGR_IC16SRC_PLL1 | (18 << RCC_IC16CFGR_IC16DIV_SHIFT));

  /* Select IC16 as LTDC clock source */

  modifyreg32(STM32_RCC_BASE + RCC_CCIPR5_OFFSET, RCC_CCIPR5_LTDCSEL_MASK,
               RCC_CCIPR5_LTDCSEL_IC16);
}

/****************************************************************************
 * Name: ltdc_configure_pins
 *
 * Description:
 *   Configure LTDC GPIO pins
 *   Reference: STM32N6570-DK LTDC example
 *
 ****************************************************************************/

static void ltdc_configure_pins(void)
{
  /* Configure LTDC pins - This is board-specific
   * For STM32N6570-DK:
   *   PH3  -> LTDC_B4  (AF14)
   *   PH6  -> LTDC_B5  (AF14)
   *   PB14 -> LTDC_HSYNC (AF14)
   *   PB13 -> LTDC_CLK  (AF14)
   *   PB15 -> LTDC_G4  (AF14)
   *   PE11 -> LTDC_VSYNC (AF14)
   *   PD8  -> LTDC_R7  (AF14)
   *   PH4  -> LTDC_R4  (AF14)
   *   PG6  -> LTDC_B3  (AF14)
   *   PA1  -> LTDC_G2  (AF14)
   *   PB11 -> LTDC_G6  (AF14)
   *   PA15 -> LTDC_R5  (AF14)
   *   PG15 -> LTDC_B0  (AF14)
   *   PG1  -> LTDC_G1  (AF14)
   *   PB12 -> LTDC_G5  (AF14)
   *   PA7  -> LTDC_B1  (AF14)
   *   PA2  -> LTDC_B7  (AF14)
   *   PG12 -> LTDC_G0  (AF14)
   *   PB4  -> LTDC_R3  (AF14)
   *   PG8  -> LTDC_G7  (AF14)
   *   PA8  -> LTDC_B6  (AF14)
   *   PG13 -> LTDC_DE  (AF14)
   *   PA0  -> LTDC_G3  (AF14)
   *   PG11 -> LTDC_R6  (AF14)
   */

  /* Enable GPIO clocks (GPIO is on AHB4 in STM32N6) */

  rcc_enable_ahb4_clock(RCC_AHB4ENR_GPIOAEN);
  rcc_enable_ahb4_clock(RCC_AHB4ENR_GPIOBEN);
  rcc_enable_ahb4_clock(RCC_AHB4ENR_GPIOFEN);
  rcc_enable_ahb4_clock(RCC_AHB4ENR_GPIOGEN);
  rcc_enable_ahb4_clock(RCC_AHB4ENR_GPIOHEN);

  /* Enable LTDC clock (LTDC is on APB5 in STM32N6) */

  rcc_enable_apb5_clock(RCC_APB5ENR_LTDCEN);

  /* Configure LTDC pins - 20 pins total (19x AF14 + 1x AF10 for VSYNC)
   * Reference: stm32n6xx_hal_msp.c:134-234
   * stm32_gpio_config(port, pin, mode, otype, speed, pupd, af)
   */

  /* Port A pins (AF14) */

  stm32_gpio_config(GPIO_PORTA, 0,  GPIO_MODER_AF, GPIO_OTYPER_PP, GPIO_OSPEEDR_HIGH, GPIO_PUPDR_NONE, GPIO_AF14);  /* G3 */
  stm32_gpio_config(GPIO_PORTA, 1,  GPIO_MODER_AF, GPIO_OTYPER_PP, GPIO_OSPEEDR_HIGH, GPIO_PUPDR_NONE, GPIO_AF14);  /* G2 */
  stm32_gpio_config(GPIO_PORTA, 2,  GPIO_MODER_AF, GPIO_OTYPER_PP, GPIO_OSPEEDR_HIGH, GPIO_PUPDR_NONE, GPIO_AF14);  /* B7 */
  stm32_gpio_config(GPIO_PORTA, 5,  GPIO_MODER_AF, GPIO_OTYPER_PP, GPIO_OSPEEDR_HIGH, GPIO_PUPDR_NONE, GPIO_AF14);  /* CLK */
  stm32_gpio_config(GPIO_PORTA, 8,  GPIO_MODER_AF, GPIO_OTYPER_PP, GPIO_OSPEEDR_HIGH, GPIO_PUPDR_NONE, GPIO_AF14);  /* B6 */
  stm32_gpio_config(GPIO_PORTA, 9,  GPIO_MODER_AF, GPIO_OTYPER_PP, GPIO_OSPEEDR_HIGH, GPIO_PUPDR_NONE, GPIO_AF14);  /* B5 */
  stm32_gpio_config(GPIO_PORTA, 10, GPIO_MODER_AF, GPIO_OTYPER_PP, GPIO_OSPEEDR_HIGH, GPIO_PUPDR_NONE, GPIO_AF14);  /* B4 */
  stm32_gpio_config(GPIO_PORTA, 11, GPIO_MODER_AF, GPIO_OTYPER_PP, GPIO_OSPEEDR_HIGH, GPIO_PUPDR_NONE, GPIO_AF14);  /* B3 */
  stm32_gpio_config(GPIO_PORTA, 15, GPIO_MODER_AF, GPIO_OTYPER_PP, GPIO_OSPEEDR_HIGH, GPIO_PUPDR_NONE, GPIO_AF14);  /* R5 */

  /* Port B pins (AF14) */

  stm32_gpio_config(GPIO_PORTB, 4,  GPIO_MODER_AF, GPIO_OTYPER_PP, GPIO_OSPEEDR_HIGH, GPIO_PUPDR_NONE, GPIO_AF14);  /* R3 */
  stm32_gpio_config(GPIO_PORTB, 10, GPIO_MODER_AF, GPIO_OTYPER_PP, GPIO_OSPEEDR_HIGH, GPIO_PUPDR_NONE, GPIO_AF14);  /* G7 */
  stm32_gpio_config(GPIO_PORTB, 11, GPIO_MODER_AF, GPIO_OTYPER_PP, GPIO_OSPEEDR_HIGH, GPIO_PUPDR_NONE, GPIO_AF14);  /* G6 */
  stm32_gpio_config(GPIO_PORTB, 12, GPIO_MODER_AF, GPIO_OTYPER_PP, GPIO_OSPEEDR_HIGH, GPIO_PUPDR_NONE, GPIO_AF14);  /* G5 */
  stm32_gpio_config(GPIO_PORTB, 15, GPIO_MODER_AF, GPIO_OTYPER_PP, GPIO_OSPEEDR_HIGH, GPIO_PUPDR_NONE, GPIO_AF14);  /* G4 */

  /* Port F pins (AF14) */

  stm32_gpio_config(GPIO_PORTF, 8,  GPIO_MODER_AF, GPIO_OTYPER_PP, GPIO_OSPEEDR_HIGH, GPIO_PUPDR_NONE, GPIO_AF14);  /* R6 */
  stm32_gpio_config(GPIO_PORTF, 9,  GPIO_MODER_AF, GPIO_OTYPER_PP, GPIO_OSPEEDR_HIGH, GPIO_PUPDR_NONE, GPIO_AF14);  /* HSYNC */

  /* Port G pins - VSYNC uses AF10! */

  stm32_gpio_config(GPIO_PORTG, 0,  GPIO_MODER_AF, GPIO_OTYPER_PP, GPIO_OSPEEDR_HIGH, GPIO_PUPDR_NONE, GPIO_AF10);  /* VSYNC (AF10) */
  stm32_gpio_config(GPIO_PORTG, 9,  GPIO_MODER_AF, GPIO_OTYPER_PP, GPIO_OSPEEDR_HIGH, GPIO_PUPDR_NONE, GPIO_AF14);  /* R7 */
  stm32_gpio_config(GPIO_PORTG, 13, GPIO_MODER_AF, GPIO_OTYPER_PP, GPIO_OSPEEDR_HIGH, GPIO_PUPDR_NONE, GPIO_AF14);  /* DE */

  /* Port H pins (AF14) */

  stm32_gpio_config(GPIO_PORTH, 4,  GPIO_MODER_AF, GPIO_OTYPER_PP, GPIO_OSPEEDR_HIGH, GPIO_PUPDR_NONE, GPIO_AF14);  /* R4 */

  /* Backlight pin PA3 - GPIO output, initially off */

  stm32_gpio_config(GPIO_PORTA, 3, GPIO_MODER_OUTPUT, GPIO_OTYPER_PP, GPIO_OSPEEDR_LOW, GPIO_PUPDR_NONE, 0);
  stm32_gpio_write(GPIO_PORTA, 3, false);

  lcdinfo("LTDC pins configured\n");
}

/****************************************************************************
 * Name: ltdc_configure_timing
 *
 * Description:
 *   Configure LTDC timing parameters
 *
 ****************************************************************************/

static void ltdc_configure_timing(void)
{
  uint32_t regval;

  /* Configure synchronous size register (SSCR) */

  regval = LTDC_SSCR_VSH(BOARD_LTDC_VSYNC - 1) |
           LTDC_SSCR_HSW(BOARD_LTDC_HSYNC - 1);
  ltdc_putreg(STM32_LTDC_SSCR_OFFSET, regval);

  /* Configure back porch configuration register (BPCR) */

  regval = LTDC_BPCR_AVBP(BOARD_LTDC_VSYNC + BOARD_LTDC_VBP - 1) |
           LTDC_BPCR_AHBP(BOARD_LTDC_HSYNC + BOARD_LTDC_HBP - 1);
  ltdc_putreg(STM32_LTDC_BPCR_OFFSET, regval);

  /* Configure active width configuration register (AWCR) */

  regval = LTDC_AWCR_AAH(BOARD_LTDC_VSYNC + BOARD_LTDC_VBP +
                          BOARD_LTDC_HEIGHT - 1) |
           LTDC_AWCR_AAW(BOARD_LTDC_HSYNC + BOARD_LTDC_HBP +
                          BOARD_LTDC_WIDTH - 1);
  ltdc_putreg(STM32_LTDC_AWCR_OFFSET, regval);

  /* Configure total width configuration register (TWCR) */

  regval = LTDC_TWCR_TOTALH(BOARD_LTDC_VSYNC + BOARD_LTDC_VBP +
                             BOARD_LTDC_HEIGHT + BOARD_LTDC_VFP - 1) |
           LTDC_TWCR_TOTALW(BOARD_LTDC_HSYNC + BOARD_LTDC_HBP +
                             BOARD_LTDC_WIDTH + BOARD_LTDC_HFP - 1);
  ltdc_putreg(STM32_LTDC_TWCR_OFFSET, regval);

  /* Configure background color */

  ltdc_putreg(STM32_LTDC_BCCR_OFFSET, 0x00000000);

  lcdinfo("LTDC timing configured: %dx%d\n", BOARD_LTDC_WIDTH,
          BOARD_LTDC_HEIGHT);
}

/****************************************************************************
 * Name: ltdc_configure_layer
 *
 * Description:
 *   Configure LTDC layer 1
 *
 ****************************************************************************/

static void ltdc_configure_layer(void)
{
  uint32_t regval;

  /* Configure window horizontal position */

  regval = LTDC_LXWHPCR_WHSTPOS(BOARD_LTDC_HSYNC + BOARD_LTDC_HBP) |
           LTDC_LXWHPCR_WHSPPOS(BOARD_LTDC_HSYNC + BOARD_LTDC_HBP +
                                 BOARD_LTDC_WIDTH - 1);
  ltdc_putreg(STM32_LTDC_L1WHPCR_OFFSET, regval);

  /* Configure window vertical position */

  regval = LTDC_LXWVPCR_WVSTPOS(BOARD_LTDC_VSYNC + BOARD_LTDC_VBP) |
           LTDC_LXWVPCR_WVSPPOS(BOARD_LTDC_VSYNC + BOARD_LTDC_VBP +
                                 BOARD_LTDC_HEIGHT - 1);
  ltdc_putreg(STM32_LTDC_L1WVPCR_OFFSET, regval);

  /* Configure pixel format - RGB565 */

  ltdc_putreg(STM32_LTDC_L1PFCR_OFFSET, LTDC_LXPFCR_RGB565);

  /* Configure constant alpha */

  ltdc_putreg(STM32_LTDC_L1CACR_OFFSET, 0xff);

  /* Configure default color */

  ltdc_putreg(STM32_LTDC_L1DCCR_OFFSET, 0x00000000);

  /* Configure blending factors */

  regval = LTDC_LXBFCR_BF1_PIXEL_ALPHA_x_CA |
           LTDC_LXBFCR_BF2_PIXEL_ALPHA_x_CA;
  ltdc_putreg(STM32_LTDC_L1BFCR_OFFSET, regval);

  /* Configure color frame buffer address */

  ltdc_putreg(STM32_LTDC_L1CFBAR_OFFSET, (uint32_t)g_ltdc_fb);

  /* Configure color frame buffer length */

  regval = LTDC_LXCFBLR_CFBLL(BOARD_LTDC_WIDTH * 2 + 3) |
           LTDC_LXCFBLR_CFBP(BOARD_LTDC_WIDTH * 2);
  ltdc_putreg(STM32_LTDC_L1CFBLR_OFFSET, regval);

  /* Configure color frame buffer line number */

  ltdc_putreg(STM32_LTDC_L1CFBLNR_OFFSET, BOARD_LTDC_HEIGHT);

  /* Enable layer 1 */

  ltdc_putreg(STM32_LTDC_L1CR_OFFSET, LTDC_LXCR_LEN);

  /* Reload shadow registers - SRCR.IMR = immediate reload */

  ltdc_modifyreg(STM32_LTDC_SRCR_OFFSET, 0, LTDC_SRCR_IMR);

  lcdinfo("LTDC layer 1 configured\n");
}

/****************************************************************************
 * Framebuffer Operations
 ****************************************************************************/

static int ltdc_getvideoinfo(struct fb_vtable_s *vplane,
                              struct fb_videoinfo_s *vinfo)
{
  struct stm32_ltdc_priv_s *priv = (struct stm32_ltdc_priv_s *)vplane;

  DEBUGASSERT(priv != NULL && vinfo != NULL);

  lcdinfo("vplane=%p vinfo=%p\n", vplane, vinfo);

  vinfo->fmt = FB_FMT_RGB16_565;
  vinfo->xres = priv->width;
  vinfo->yres = priv->height;
  vinfo->nplanes = 1;

  return OK;
}

static int ltdc_getplaneinfo(struct fb_vtable_s *vplane, int planeno,
                              struct fb_planeinfo_s *pinfo)
{
  struct stm32_ltdc_priv_s *priv = (struct stm32_ltdc_priv_s *)vplane;

  DEBUGASSERT(priv != NULL && pinfo != NULL && planeno == 0);

  lcdinfo("vplane=%p planeno=%d pinfo=%p\n", vplane, planeno, pinfo);

  pinfo->fbmem = priv->fbmem;
  pinfo->fblen = priv->width * priv->height * (priv->bpp / 8);
  pinfo->stride = priv->width * (priv->bpp / 8);
  pinfo->display = 0;
  pinfo->bpp = priv->bpp;

  return OK;
}

#ifdef CONFIG_FB_HWCURSOR
static int ltdc_setcursor(struct fb_vtable_s *vplane,
                           struct fb_setcursor_s *settings)
{
  /* Not supported */

  return -ENOSYS;
}
#endif

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: stm32_ltdcinitialize
 *
 * Description:
 *   Initialize the LTDC controller
 *
 * Returned Value:
 *   OK on success; a negated errno on failure
 *
 ****************************************************************************/

int stm32_ltdcinitialize(void)
{
  struct stm32_ltdc_priv_s *priv = &g_ltdc_priv;

  lcdinfo("Initializing LTDC\n");

  /* Initialize private data */

  memset(priv, 0, sizeof(struct stm32_ltdc_priv_s));

  priv->width = BOARD_LTDC_WIDTH;
  priv->height = BOARD_LTDC_HEIGHT;
  priv->bpp = 16;  /* RGB565 */
  priv->pixfmt = LTDC_PIXEL_FORMAT_RGB565;
  priv->fbmem = g_ltdc_fb;

  /* Set framebuffer vtable operations */

  priv->vtable.getvideoinfo = ltdc_getvideoinfo;
  priv->vtable.getplaneinfo = ltdc_getplaneinfo;
#ifdef CONFIG_FB_HWCURSOR
  priv->vtable.setcursor = ltdc_setcursor;
#endif

  /* Configure LTDC clocks */

  ltdc_configure_clocks();

  /* Configure LTDC pins */

  ltdc_configure_pins();

  /* Configure LTDC timing */

  ltdc_configure_timing();

  /* Configure LTDC layer */

  ltdc_configure_layer();

  /* Enable LTDC */

  ltdc_modifyreg(STM32_LTDC_GCR_OFFSET, 0, LTDC_GCR_LTDCEN);

  /* Clear framebuffer */

  memset(g_ltdc_fb, 0, LTDC_FRAMEBUFFER_SIZE);

  /* Register framebuffer device /dev/fb0 */

#ifdef CONFIG_VIDEO_FB
  fb_register_device(0, 0, &priv->vtable);
#endif

  /* Turn on backlight (PA3) */

  stm32_gpio_write(GPIO_PORTA, 3, true);

  lcdinfo("LTDC initialized successfully\n");

  return OK;
}

/****************************************************************************
 * Name: stm32_ltdcuninitialize
 *
 * Description:
 *   Uninitialize the LTDC controller
 *
 ****************************************************************************/

void stm32_ltdcuninitialize(void)
{
  lcdinfo("Uninitializing LTDC\n");

  /* Disable LTDC */

  ltdc_modifyreg(STM32_LTDC_GCR_OFFSET, LTDC_GCR_LTDCEN, 0);

  /* Disable LTDC clock (LTDC is on APB5, bit 1) */

  modifyreg32(STM32_RCC_BASE + RCC_APB5ENR_OFFSET,
              RCC_APB5ENR_LTDCEN, 0);
}

/****************************************************************************
 * Name: stm32_ltdcgetvplane
 *
 * Description:
 *   Get video plane reference used by framebuffer interface
 *
 * Parameter:
 *   vplane - Video plane
 *
 * Returned Value:
 *   Video plane reference
 *
 ****************************************************************************/

struct fb_vtable_s *stm32_ltdcgetvplane(int vplane)
{
  struct stm32_ltdc_priv_s *priv = &g_ltdc_priv;

  lcdinfo("vplane=%d\n", vplane);

  if (vplane == 0)
    {
      return &priv->vtable;
    }

  return NULL;
}
