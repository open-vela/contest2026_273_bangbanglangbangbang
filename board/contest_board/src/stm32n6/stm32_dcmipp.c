/****************************************************************************
 * arch/arm/src/stm32n6/stm32_dcmipp.c
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
#include <nuttx/arch.h>
#include <nuttx/irq.h>

#include <errno.h>
#include <debug.h>
#include <string.h>

#include "arm_internal.h"
#include "hardware/stm32_dcmipp.h"
#include "hardware/stm32_rcc.h"
#include "stm32_dcmipp.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define DCMIPP_TIMEOUT_MS  100

/****************************************************************************
 * Private Types
 ****************************************************************************/

/* SNPS PHY frequency table entry for CSI bitrate configuration */

struct snps_phy_freq_s
{
  uint32_t reg_val;
  uint32_t freq_mhz;
};

/****************************************************************************
 * Private Data
 ****************************************************************************/

/* SNPS PHY frequency table indexed by CSI_PHY_BT_xxx values */

static const struct snps_phy_freq_s g_snps_phy_freqs[63] =
{
  { 0x00U, 460U },  /* BT_80   */
  { 0x10U, 460U },  /* BT_90   */
  { 0x20U, 460U },  /* BT_100  */
  { 0x30U, 460U },  /* BT_110  */
  { 0x01U, 460U },  /* BT_120  */
  { 0x11U, 460U },  /* BT_130  */
  { 0x21U, 460U },  /* BT_140  */
  { 0x31U, 460U },  /* BT_150  */
  { 0x02U, 460U },  /* BT_160  */
  { 0x12U, 460U },  /* BT_170  */
  { 0x22U, 460U },  /* BT_180  */
  { 0x32U, 460U },  /* BT_190  */
  { 0x03U, 460U },  /* BT_205  */
  { 0x13U, 460U },  /* BT_220  */
  { 0x23U, 460U },  /* BT_235  */
  { 0x33U, 460U },  /* BT_250  */
  { 0x04U, 460U },  /* BT_275  */
  { 0x14U, 460U },  /* BT_300  */
  { 0x25U, 460U },  /* BT_325  */
  { 0x35U, 460U },  /* BT_350  */
  { 0x05U, 460U },  /* BT_400  */
  { 0x16U, 460U },  /* BT_450  */
  { 0x26U, 460U },  /* BT_500  */
  { 0x37U, 460U },  /* BT_550  */
  { 0x07U, 460U },  /* BT_600  */
  { 0x18U, 460U },  /* BT_650  */
  { 0x28U, 460U },  /* BT_700  */
  { 0x39U, 460U },  /* BT_750  */
  { 0x09U, 460U },  /* BT_800  */
  { 0x19U, 460U },  /* BT_850  */
  { 0x29U, 460U },  /* BT_900  */
  { 0x3AU, 460U },  /* BT_950  */
  { 0x0AU, 460U },  /* BT_1000 */
  { 0x1AU, 460U },  /* BT_1050 */
  { 0x2AU, 460U },  /* BT_1100 */
  { 0x3BU, 460U },  /* BT_1150 */
  { 0x0BU, 460U },  /* BT_1200 */
  { 0x1BU, 460U },  /* BT_1250 */
  { 0x2BU, 460U },  /* BT_1300 */
  { 0x3CU, 460U },  /* BT_1350 */
  { 0x0CU, 460U },  /* BT_1400 */
  { 0x1CU, 460U },  /* BT_1450 */
  { 0x2CU, 460U },  /* BT_1500 */
  { 0x3DU, 285U },  /* BT_1550 */
  { 0x0DU, 295U },  /* BT_1600 */
  { 0x1DU, 304U },  /* BT_1650 */
  { 0x2EU, 313U },  /* BT_1700 */
  { 0x3EU, 322U },  /* BT_1750 */
  { 0x0EU, 331U },  /* BT_1800 */
  { 0x1EU, 341U },  /* BT_1850 */
  { 0x2FU, 350U },  /* BT_1900 */
  { 0x3FU, 359U },  /* BT_1950 */
  { 0x0FU, 368U },  /* BT_2000 */
  { 0x40U, 377U },  /* BT_2050 */
  { 0x41U, 387U },  /* BT_2100 */
  { 0x42U, 396U },  /* BT_2150 */
  { 0x43U, 405U },  /* BT_2200 */
  { 0x44U, 414U },  /* BT_2250 */
  { 0x45U, 423U },  /* BT_2300 */
  { 0x46U, 432U },  /* BT_2350 */
  { 0x47U, 442U },  /* BT_2400 */
  { 0x48U, 451U },  /* BT_2450 */
  { 0x49U, 460U },  /* BT_2500 */
};

static dcmipp_frame_callback_t g_pipe_callback[DCMIPP_NUM_OF_PIPES];
static void *g_pipe_callback_arg[DCMIPP_NUM_OF_PIPES];
static bool g_dcmipp_initialized = false;

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: stm32_dcmipp_enable_clock
 *
 * Description:
 *   Enable the DCMIPP peripheral clock.
 *
 ****************************************************************************/

static void stm32_dcmipp_enable_clock(void)
{
  /* Enable DCMIPP clock on APB5 */

  modifyreg32(STM32_RCC_BASE + RCC_APB5ENR_OFFSET, 0, RCC_APB5ENR_DCMIPPEN);

  /* Small delay for clock to stabilize */

  up_udelay(10);
}

/****************************************************************************
 * Name: stm32_dcmipp_reset
 *
 * Description:
 *   Reset the DCMIPP peripheral.
 *
 ****************************************************************************/

static void stm32_dcmipp_reset(void)
{
  /* Assert reset */

  modifyreg32(STM32_RCC_BASE + RCC_APB5RSTR_OFFSET, 0, RCC_APB5RSTR_DCMIPPRST);

  /* Wait a bit */

  up_udelay(10);

  /* Release reset */

  modifyreg32(STM32_RCC_BASE + RCC_APB5RSTR_OFFSET, RCC_APB5RSTR_DCMIPPRST, 0);

  up_udelay(10);
}

/****************************************************************************
 * Name: stm32_dcmipp_irq_handler
 *
 * Description:
 *   DCMIPP interrupt handler. Processes frame completion events.
 *
 ****************************************************************************/

static int stm32_dcmipp_irq_handler(int irq, void *context, void *arg)
{
  uint32_t status;
  uint32_t pipe;

  UNUSED(irq);
  UNUSED(context);
  UNUSED(arg);

  /* Read common status register 2 */

  status = DCMIPP->CMSR2;

  /* Handle pipe frame events */

  for (pipe = 0; pipe < DCMIPP_NUM_OF_PIPES; pipe++)
  {
    uint32_t frame_flag = DCMIPP_CMSR2_P0FRAMEF << (pipe * 4);
    uint32_t ovr_flag = DCMIPP_CMSR2_P0OVRF << (pipe * 4);

    if (status & ovr_flag)
    {
      verr("DCMIPP pipe%d overrun!\n", pipe);

      /* Clear overrun flag */

      DCMIPP->CMFCR = ovr_flag;
    }

    if (status & frame_flag)
    {
      /* Clear frame flag */

      DCMIPP->CMFCR = frame_flag;

      /* Call registered callback */

      if (g_pipe_callback[pipe] != NULL)
      {
        g_pipe_callback[pipe](pipe, g_pipe_callback_arg[pipe]);
      }
    }
  }

  /* Clear parallel sync error if set */

  if (status & DCMIPP_CMSR2_PRERRF)
  {
    DCMIPP->CMFCR = DCMIPP_CMSR2_PRERRF;
  }

  return OK;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: stm32_dcmipp_init
 ****************************************************************************/

int stm32_dcmipp_init(void)
{
  if (g_dcmipp_initialized)
    {
      return 0;
    }

  /* Enable peripheral clock */

  stm32_dcmipp_enable_clock();

  /* Reset the DCMIPP */

  stm32_dcmipp_reset();

  /* Configure IPPLUG default settings
   * Client1 (Pipe0): 128-byte burst, 2 outstanding transactions
   * Client2 (Pipe1): 128-byte burst, 4 outstanding transactions
   * Client3 (Pipe2): 64-byte burst, 2 outstanding transactions
   */

  DCMIPP->IPGR1 = (0x06U << 0);  /* Memory page size = 4KB */
  DCMIPP->IPGR2 = (0x00U << 0);  /* Timeout */

  /* Client 1 (Pipe0) */

  DCMIPP->IPC1R1 = (0x04U << 4) | (0x01U << 0);  /* 128B burst, 2 outstanding */
  DCMIPP->IPC1R2 = (0x200U << 0);  /* DPREG Start */
  DCMIPP->IPC1R3 = (0x3FFU << 0);  /* DPREG End */

  /* Client 2 (Pipe1) */

  DCMIPP->IPC2R1 = (0x04U << 4) | (0x03U << 0);  /* 128B burst, 4 outstanding */
  DCMIPP->IPC2R2 = (0x000U << 0);  /* DPREG Start */
  DCMIPP->IPC2R3 = (0x1FFU << 0);  /* DPREG End */

  /* Client 3 (Pipe2) */

  DCMIPP->IPC3R1 = (0x03U << 4) | (0x01U << 0);  /* 64B burst, 2 outstanding */
  DCMIPP->IPC3R2 = (0x200U << 0);
  DCMIPP->IPC3R3 = (0x3FFU << 0);

  /* Clear all interrupts */

  DCMIPP->CMFCR = 0xFFFFFFFF;
  DCMIPP->P0FCR = 0xFFFFFFFF;
  DCMIPP->P1FCR = 0xFFFFFFFF;
  DCMIPP->P2FCR = 0xFFFFFFFF;

  /* Register IRQ handler */

  irq_attach(STM32_IRQ_DCMIPP, stm32_dcmipp_irq_handler, NULL);

  /* Enable the DCMIPP interrupt at NVIC level */

  up_enable_irq(STM32_IRQ_DCMIPP);

  g_dcmipp_initialized = true;
  return 0;
}

/****************************************************************************
 * Name: stm32_dcmipp_deinit
 ****************************************************************************/

void stm32_dcmipp_deinit(void)
{
  if (!g_dcmipp_initialized)
    {
      return;
    }

  /* Disable interrupts */

  DCMIPP->CMIER = 0;

  /* Disable pipe captures */

  DCMIPP->P0FCTCR = 0;
  DCMIPP->P1FCTCR = 0;
  DCMIPP->P2FCTCR = 0;

  /* Disable parallel interface */

  DCMIPP->PRCR &= ~DCMIPP_PRCR_ENABLE;

  /* Disable CSI */

  CSI->CR &= ~CSI_CR_CSIEN;

  /* Disable clock */

  modifyreg32(STM32_RCC_BASE + RCC_APB5ENR_OFFSET, RCC_APB5ENR_DCMIPPEN, 0);

  /* Detach IRQ */

  up_disable_irq(STM32_IRQ_DCMIPP);
  irq_detach(STM32_IRQ_DCMIPP);

  g_dcmipp_initialized = false;
}

/****************************************************************************
 * Name: stm32_dcmipp_parallel_config
 ****************************************************************************/

int stm32_dcmipp_parallel_config(const struct dcmipp_parallel_cfg_s *cfg)
{
  uint32_t prcr;

  if (cfg == NULL)
    {
      return -EINVAL;
    }

  /* Build PRCR register value */

  prcr = cfg->format | cfg->extended_data | cfg->vsync_pol |
         cfg->hsync_pol | cfg->pck_pol;

  /* Hardware sync mode, no swap */

  prcr |= 0;  /* ESS=0 for hardware sync, no swap bits/cycles */

  /* Write to PRCR register */

  DCMIPP->PRCR = prcr;

  /* Enable sync error interrupt */

  DCMIPP->CMIER |= DCMIPP_CMIER_PRERRIE;

  /* Enable the parallel interface */

  DCMIPP->PRCR |= DCMIPP_PRCR_ENABLE;

  /* Select parallel input mode */

  DCMIPP->CMCR &= ~DCMIPP_CMCR_INSEL;

  return 0;
}

/****************************************************************************
 * Name: stm32_dcmipp_csi_config
 ****************************************************************************/

int stm32_dcmipp_csi_config(const struct dcmipp_csi_cfg_s *cfg)
{
  uint32_t lmcfgr;
  uint32_t dphy_ier = 0;

  if (cfg == NULL)
    {
      return -EINVAL;
    }

  /* Ensure CSI is disabled first */

  CSI->CR &= ~CSI_CR_CSIEN;

  /* Configure Lane Merger */

  if (cfg->lane_mapping == 1)  /* Physical mapping */
    {
      lmcfgr = cfg->num_lanes |
               (1U << CSI_LMCFGR_DL0MAP_Pos) |
               (2U << CSI_LMCFGR_DL1MAP_Pos);
    }
  else  /* Inverted mapping */
    {
      lmcfgr = cfg->num_lanes |
               (2U << CSI_LMCFGR_DL0MAP_Pos) |
               (1U << CSI_LMCFGR_DL1MAP_Pos);
    }

  CSI->LMCFGR = lmcfgr;

  /* Enable CSI */

  CSI->CR |= CSI_CR_CSIEN;

  /* Enable CSI error interrupts */

  CSI->IER0 = CSI_IER0_CCFIFOFIE | CSI_IER0_SYNCERRIE |
              CSI_IER0_SPKTERRIE | CSI_IER0_IDERRIE |
              CSI_IER0_SPKTIE;

  /* Enable D-PHY interrupts based on lane count */

  if (cfg->num_lanes == CSI_ONE_DATA_LANE)
    {
      if (cfg->lane_mapping == 1)
        {
          dphy_ier = CSI_IER1_ESOTDL0IE | CSI_IER1_ESOTSYNCDL0IE |
                     CSI_IER1_EESCDL0IE | CSI_IER1_ESYNCESCDL0IE |
                     CSI_IER1_ECTRLDL0IE;
        }
      else
        {
          dphy_ier = CSI_IER1_ESOTDL1IE | CSI_IER1_ESOTSYNCDL1IE |
                     CSI_IER1_EESCDL1IE | CSI_IER1_ESYNCESCDL1IE |
                     CSI_IER1_ECTRLDL1IE;
        }
    }
  else  /* Two data lanes */
    {
      if (cfg->lane_mapping == 1)
        {
          dphy_ier = CSI_IER1_ESOTDL0IE | CSI_IER1_ESOTSYNCDL0IE |
                     CSI_IER1_EESCDL0IE | CSI_IER1_ESYNCESCDL0IE |
                     CSI_IER1_ECTRLDL0IE |
                     CSI_IER1_ESOTDL1IE | CSI_IER1_ESOTSYNCDL1IE |
                     CSI_IER1_EESCDL1IE | CSI_IER1_ESYNCESCDL1IE |
                     CSI_IER1_ECTRLDL1IE;
        }
      else
        {
          dphy_ier = CSI_IER1_ESOTDL1IE | CSI_IER1_ESOTSYNCDL1IE |
                     CSI_IER1_EESCDL1IE | CSI_IER1_ESYNCESCDL1IE |
                     CSI_IER1_ECTRLDL1IE |
                     CSI_IER1_ESOTDL0IE | CSI_IER1_ESOTSYNCDL0IE |
                     CSI_IER1_EESCDL0IE | CSI_IER1_ESYNCESCDL0IE |
                     CSI_IER1_ECTRLDL0IE;
        }
    }

  CSI->IER1 = dphy_ier;

  /* Configure D-PHY: power up, enable PLL */

  CSI->PCR = CSI_PCR_CSION | CSI_PCR_IFON;

  /* Configure PHY PLL for the requested bitrate */

  if (cfg->phy_bitrate <= CSI_PHY_BT_2500)
    {
      /* Set PHY test control to configure the PLL multiplier */

      CSI->PHY_TST_CTRL0 = 0x00000000;
      CSI->PHY_TST_CTRL1 = g_snps_phy_freqs[cfg->phy_bitrate].reg_val;

      /* Enable PLL */

      CSI->PHY_PLLCR = CSI_PHY_PLLCR_PLLEN;
    }

  /* Select CSI serial input mode */

  DCMIPP->CMCR |= DCMIPP_CMCR_INSEL;

  return 0;
}

/****************************************************************************
 * Name: stm32_dcmipp_pipe_config
 ****************************************************************************/

int stm32_dcmipp_pipe_config(uint32_t pipe, uint32_t dt, uint32_t vc,
                              uint32_t pp_format, uint32_t framerate)
{
  if (pipe >= DCMIPP_NUM_OF_PIPES)
    {
      return -EINVAL;
    }

  switch (pipe)
    {
      case DCMIPP_PIPE0:
        /* Configure flow selection: data type + virtual channel */

        DCMIPP->P0FSCR = (dt << DCMIPP_P0FSCR_DTIDA_Pos) |
                         (vc << DCMIPP_P0FSCR_VC_Pos) |
                         DCMIPP_DTMODE_DTIDA;

        /* Configure flow control: frame rate */

        DCMIPP->P0FCTCR = (framerate & DCMIPP_P0FCTCR_FRATE_Msk);

        /* Configure pixel packer */

        DCMIPP->P0PPCR = pp_format & DCMIPP_P1PPCR_FORMAT_Msk;

        /* Enable pipe0 frame interrupt */

        DCMIPP->CMIER |= (DCMIPP_CMIER_P0FRAMEIE | DCMIPP_CMIER_P0OVRIE);
        break;

      case DCMIPP_PIPE1:
        /* Configure flow selection */

        DCMIPP->P1FSCR = (dt << DCMIPP_P0FSCR_DTIDA_Pos) |
                         (vc << DCMIPP_P0FSCR_VC_Pos) |
                         DCMIPP_DTMODE_DTIDA;

        /* Configure flow control */

        DCMIPP->P1FCTCR = (framerate & DCMIPP_P0FCTCR_FRATE_Msk);

        /* Configure pixel packer */

        DCMIPP->P1PPCR = pp_format & DCMIPP_P1PPCR_FORMAT_Msk;

        /* Enable pipe1 frame interrupt */

        DCMIPP->CMIER |= (DCMIPP_CMIER_P1FRAMEIE | DCMIPP_CMIER_P1OVRIE);
        break;

      case DCMIPP_PIPE2:
        /* Configure flow selection */

        DCMIPP->P2FSCR = (dt << DCMIPP_P0FSCR_DTIDA_Pos) |
                         (vc << DCMIPP_P0FSCR_VC_Pos) |
                         DCMIPP_DTMODE_DTIDA;

        /* Configure flow control */

        DCMIPP->P2FCTCR = (framerate & DCMIPP_P0FCTCR_FRATE_Msk);

        /* Configure pixel packer */

        DCMIPP->P2PPCR = pp_format & DCMIPP_P1PPCR_FORMAT_Msk;

        /* Enable pipe2 frame interrupt */

        DCMIPP->CMIER |= (DCMIPP_CMIER_P2FRAMEIE | DCMIPP_CMIER_P2OVRIE);
        break;

      default:
        return -EINVAL;
    }

  return 0;
}

/****************************************************************************
 * Name: stm32_dcmipp_pipe_setbuf
 ****************************************************************************/

int stm32_dcmipp_pipe_setbuf(uint32_t pipe, uint32_t addr)
{
  if (pipe >= DCMIPP_NUM_OF_PIPES)
    {
      return -EINVAL;
    }

  /* Address must be 16-byte aligned */

  if (addr & 0xf)
    {
      return -EINVAL;
    }

  switch (pipe)
    {
      case DCMIPP_PIPE0:
        DCMIPP->P0PPM0AR1 = addr;
        break;

      case DCMIPP_PIPE1:
        DCMIPP->P1PPM0AR1 = addr;
        break;

      case DCMIPP_PIPE2:
        DCMIPP->P2PPM0AR1 = addr;
        break;

      default:
        return -EINVAL;
    }

  return 0;
}

/****************************************************************************
 * Name: stm32_dcmipp_pipe_start
 ****************************************************************************/

int stm32_dcmipp_pipe_start(uint32_t pipe, uint32_t mode)
{
  if (pipe >= DCMIPP_NUM_OF_PIPES)
    {
      return -EINVAL;
    }

  switch (pipe)
    {
      case DCMIPP_PIPE0:
        /* Set capture mode */

        DCMIPP->P0FCTCR |= mode;

        /* Enable capture */

        DCMIPP->P0FCTCR |= (1 << 0);  /* CPTREQ bit */
        break;

      case DCMIPP_PIPE1:
        DCMIPP->P1FCTCR |= mode;
        DCMIPP->P1FCTCR |= (1 << 0);
        break;

      case DCMIPP_PIPE2:
        DCMIPP->P2FCTCR |= mode;
        DCMIPP->P2FCTCR |= (1 << 0);
        break;

      default:
        return -EINVAL;
    }

  return 0;
}

/****************************************************************************
 * Name: stm32_dcmipp_pipe_stop
 ****************************************************************************/

int stm32_dcmipp_pipe_stop(uint32_t pipe)
{
  if (pipe >= DCMIPP_NUM_OF_PIPES)
    {
      return -EINVAL;
    }

  switch (pipe)
    {
      case DCMIPP_PIPE0:
        /* Disable capture */

        DCMIPP->P0FCTCR &= ~(1 << 0);
        break;

      case DCMIPP_PIPE1:
        DCMIPP->P1FCTCR &= ~(1 << 0);
        break;

      case DCMIPP_PIPE2:
        DCMIPP->P2FCTCR &= ~(1 << 0);
        break;

      default:
        return -EINVAL;
    }

  /* Disable AXI transfer error interrupt */

  DCMIPP->CMIER &= ~DCMIPP_CMIER_ATXERRIE;

  return 0;
}

/****************************************************************************
 * Name: stm32_dcmipp_register_callback
 ****************************************************************************/

void stm32_dcmipp_register_callback(uint32_t pipe,
                                     dcmipp_frame_callback_t callback,
                                     void *arg)
{
  if (pipe < DCMIPP_NUM_OF_PIPES)
    {
      g_pipe_callback[pipe] = callback;
      g_pipe_callback_arg[pipe] = arg;
    }
}
