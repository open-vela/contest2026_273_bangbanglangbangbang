/****************************************************************************
 * arch/arm/src/stm32n6/hardware/stm32_dcmipp.h
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

#ifndef __ARCH_ARM_SRC_STM32N6_HARDWARE_STM32_DCMIPP_H
#define __ARCH_ARM_SRC_STM32N6_HARDWARE_STM32_DCMIPP_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include <stdint.h>
#include "chip.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* DCMIPP and CSI base addresses (non-secure, APB5) */

#define APB5PERIPH_BASE        0x48000000UL
#define DCMIPP_BASE            (APB5PERIPH_BASE + 0x2000UL)
#define CSI_BASE               (APB5PERIPH_BASE + 0x6000UL)

/* Number of pipes */

#define DCMIPP_NUM_OF_PIPES    3

/****************************************************************************
 * DCMIPP Register Offsets
 ****************************************************************************/

/* IPPLUG registers */

#define DCMIPP_IPGR1_OFFSET    0x000
#define DCMIPP_IPGR2_OFFSET    0x004
#define DCMIPP_IPGR3_OFFSET    0x008
#define DCMIPP_IPGR8_OFFSET    0x01C

/* IPPLUG Client registers (base for client 1, +0x10 per client) */

#define DCMIPP_IPC1R1_OFFSET   0x020
#define DCMIPP_IPC1R2_OFFSET   0x024
#define DCMIPP_IPC1R3_OFFSET   0x028
#define DCMIPP_IPC2R1_OFFSET   0x030
#define DCMIPP_IPC2R2_OFFSET   0x034
#define DCMIPP_IPC2R3_OFFSET   0x038
#define DCMIPP_IPC3R1_OFFSET   0x040
#define DCMIPP_IPC3R2_OFFSET   0x044
#define DCMIPP_IPC3R3_OFFSET   0x048
#define DCMIPP_IPC4R1_OFFSET   0x050
#define DCMIPP_IPC4R2_OFFSET   0x054
#define DCMIPP_IPC4R3_OFFSET   0x058
#define DCMIPP_IPC5R1_OFFSET   0x060
#define DCMIPP_IPC5R2_OFFSET   0x064
#define DCMIPP_IPC5R3_OFFSET   0x068

/* Parallel interface registers */

#define DCMIPP_PRCR_OFFSET     0x104  /* Parallel interface control register */
#define DCMIPP_PRESCR_OFFSET   0x108  /* Embedded sync code register */
#define DCMIPP_PRESUR_OFFSET   0x10C  /* Embedded sync unmask register */
#define DCMIPP_PRIER_OFFSET    0x1F4  /* Parallel interrupt enable register */
#define DCMIPP_PRSR_OFFSET     0x1F8  /* Parallel status register */
#define DCMIPP_PRFCR_OFFSET    0x1FC  /* Parallel interrupt clear register */

/* Common registers */

#define DCMIPP_CMCR_OFFSET     0x204  /* Common configuration register */
#define DCMIPP_CMFRCR_OFFSET   0x208  /* Common frame counter register */
#define DCMIPP_CMIER_OFFSET    0x3F0  /* Common interrupt enable register */
#define DCMIPP_CMSR1_OFFSET    0x3F4  /* Common status register 1 */
#define DCMIPP_CMSR2_OFFSET    0x3F8  /* Common status register 2 */
#define DCMIPP_CMFCR_OFFSET    0x3FC  /* Common interrupt clear register */

/* Pipe0 registers (dump pipe) */

#define DCMIPP_P0FSCR_OFFSET   0x404
#define DCMIPP_P0FCTCR_OFFSET  0x500
#define DCMIPP_P0SCSTR_OFFSET  0x504
#define DCMIPP_P0SCSZR_OFFSET  0x508
#define DCMIPP_P0DCCNTR_OFFSET 0x5B0
#define DCMIPP_P0DCLMTR_OFFSET 0x5B4
#define DCMIPP_P0PPCR_OFFSET   0x5C0
#define DCMIPP_P0PPM0AR1_OFFSET 0x5C4
#define DCMIPP_P0PPM0AR2_OFFSET 0x5C8
#define DCMIPP_P0STM0AR_OFFSET 0x5D0
#define DCMIPP_P0IER_OFFSET    0x5F4
#define DCMIPP_P0SR_OFFSET     0x5F8
#define DCMIPP_P0FCR_OFFSET    0x5FC

/* Pipe1 registers (main pipe) */

#define DCMIPP_P1FSCR_OFFSET   0x804
#define DCMIPP_P1SRCR_OFFSET   0x820
#define DCMIPP_P1BPRCR_OFFSET  0x824
#define DCMIPP_P1BPRSR_OFFSET  0x828
#define DCMIPP_P1DECR_OFFSET   0x830
#define DCMIPP_P1BLCCR_OFFSET  0x840
#define DCMIPP_P1EXCR1_OFFSET  0x844
#define DCMIPP_P1EXCR2_OFFSET  0x848
#define DCMIPP_P1ST1CR_OFFSET  0x850
#define DCMIPP_P1DMCR_OFFSET   0x870
#define DCMIPP_P1CCCR_OFFSET   0x880
#define DCMIPP_P1CTCR1_OFFSET  0x8A0
#define DCMIPP_P1FCTCR_OFFSET  0x900
#define DCMIPP_P1CRSTR_OFFSET  0x904
#define DCMIPP_P1CRSZR_OFFSET  0x908
#define DCMIPP_P1DCCR_OFFSET   0x90C
#define DCMIPP_P1PPCR_OFFSET   0x9C0
#define DCMIPP_P1PPM0AR1_OFFSET 0x9C4
#define DCMIPP_P1PPM0AR2_OFFSET 0x9C8
#define DCMIPP_P1PPM0PR_OFFSET 0x9CC
#define DCMIPP_P1STM0AR_OFFSET 0x9D0
#define DCMIPP_P1PPM1AR1_OFFSET 0x9D4
#define DCMIPP_P1PPM1AR2_OFFSET 0x9D8
#define DCMIPP_P1PPM1PR_OFFSET 0x9DC
#define DCMIPP_P1STM1AR_OFFSET 0x9E0
#define DCMIPP_P1PPM2AR1_OFFSET 0x9E4
#define DCMIPP_P1PPM2AR2_OFFSET 0x9E8
#define DCMIPP_P1STM2AR_OFFSET 0x9F0
#define DCMIPP_P1IER_OFFSET    0x9F4
#define DCMIPP_P1SR_OFFSET     0x9F8
#define DCMIPP_P1FCR_OFFSET    0x9FC

/* Pipe2 registers (ancillary pipe) */

#define DCMIPP_P2FSCR_OFFSET   0xC04
#define DCMIPP_P2FCTCR_OFFSET  0xD00
#define DCMIPP_P2PPCR_OFFSET   0xDC0
#define DCMIPP_P2PPM0AR1_OFFSET 0xDC4
#define DCMIPP_P2PPM0AR2_OFFSET 0xDC8
#define DCMIPP_P2PPM0PR_OFFSET 0xDCC
#define DCMIPP_P2IER_OFFSET    0xDF4
#define DCMIPP_P2SR_OFFSET     0xDF8
#define DCMIPP_P2FCR_OFFSET    0xDFC

/****************************************************************************
 * DCMIPP PRCR Register Bits (Parallel interface Control Register)
 ****************************************************************************/

#define DCMIPP_PRCR_ENABLE     (1 << 0)   /* Parallel interface enable */
#define DCMIPP_PRCR_FORMAT_Pos 4
#define DCMIPP_PRCR_FORMAT_Msk (0xFF << DCMIPP_PRCR_FORMAT_Pos)
#define DCMIPP_PRCR_EDM_Pos    12
#define DCMIPP_PRCR_EDM_Msk    (0x7 << DCMIPP_PRCR_EDM_Pos)
#define DCMIPP_PRCR_VSPOL      (1 << 16)  /* VSYNC polarity */
#define DCMIPP_PRCR_HSPOL      (1 << 17)  /* HSYNC polarity */
#define DCMIPP_PRCR_PCKPOL     (1 << 18)  /* Pixel clock polarity */
#define DCMIPP_PRCR_ESS        (1 << 20)  /* Embedded sync select */
#define DCMIPP_PRCR_SWAPCYCLES (1 << 22)  /* Swap cycles */
#define DCMIPP_PRCR_SWAPBITS   (1 << 23)  /* Swap bits */

/* Parallel format values */

#define DCMIPP_FORMAT_BYTE          0U
#define DCMIPP_FORMAT_YUV422       (0x1EU << DCMIPP_PRCR_FORMAT_Pos)
#define DCMIPP_FORMAT_RGB565       (0x22U << DCMIPP_PRCR_FORMAT_Pos)
#define DCMIPP_FORMAT_RGB666       (0x23U << DCMIPP_PRCR_FORMAT_Pos)
#define DCMIPP_FORMAT_RGB888       (0x24U << DCMIPP_PRCR_FORMAT_Pos)
#define DCMIPP_FORMAT_RAW8         (0x2AU << DCMIPP_PRCR_FORMAT_Pos)
#define DCMIPP_FORMAT_RAW10        (0x2BU << DCMIPP_PRCR_FORMAT_Pos)
#define DCMIPP_FORMAT_RAW12        (0x2CU << DCMIPP_PRCR_FORMAT_Pos)

/* Extended data mode */

#define DCMIPP_INTERFACE_8BITS   0U
#define DCMIPP_INTERFACE_10BITS  (1U << DCMIPP_PRCR_EDM_Pos)
#define DCMIPP_INTERFACE_12BITS  (2U << DCMIPP_PRCR_EDM_Pos)
#define DCMIPP_INTERFACE_14BITS  (3U << DCMIPP_PRCR_EDM_Pos)
#define DCMIPP_INTERFACE_16BITS  (4U << DCMIPP_PRCR_EDM_Pos)

/* Polarity definitions */

#define DCMIPP_VSPOLARITY_LOW   0U
#define DCMIPP_VSPOLARITY_HIGH  DCMIPP_PRCR_VSPOL
#define DCMIPP_HSPOLARITY_LOW   0U
#define DCMIPP_HSPOLARITY_HIGH  DCMIPP_PRCR_HSPOL
#define DCMIPP_PCKPOLARITY_FALLING 0U
#define DCMIPP_PCKPOLARITY_RISING  DCMIPP_PRCR_PCKPOL

/****************************************************************************
 * DCMIPP CMCR Register Bits (Common Configuration Register)
 ****************************************************************************/

#define DCMIPP_CMCR_INSEL      (1 << 0)   /* Input selection: 0=parallel, 1=CSI */
#define DCMIPP_CMCR_SWAPRB     (1 << 1)   /* Swap R/U and B/V */
#define DCMIPP_CMCR_INSEL_Pos  0

#define DCMIPP_PARALLEL_MODE   0U
#define DCMIPP_SERIAL_MODE     DCMIPP_CMCR_INSEL

/****************************************************************************
 * DCMIPP P0FCTCR Register Bits (Pipe0 Flow Control Configuration)
 ****************************************************************************/

#define DCMIPP_P0FCTCR_CPTMODE (1 << 3)   /* Capture mode: 0=continuous, 1=snapshot */
#define DCMIPP_P0FCTCR_FRATE_Pos 4
#define DCMIPP_P0FCTCR_FRATE_Msk (0x3 << DCMIPP_P0FCTCR_FRATE_Pos)

#define DCMIPP_MODE_CONTINUOUS 0U
#define DCMIPP_MODE_SNAPSHOT   DCMIPP_P0FCTCR_CPTMODE

#define DCMIPP_FRAME_RATE_ALL      0U
#define DCMIPP_FRAME_RATE_1_OVER_2 (1U << DCMIPP_P0FCTCR_FRATE_Pos)
#define DCMIPP_FRAME_RATE_1_OVER_4 (2U << DCMIPP_P0FCTCR_FRATE_Pos)
#define DCMIPP_FRAME_RATE_1_OVER_8 (3U << DCMIPP_P0FCTCR_FRATE_Pos)

/****************************************************************************
 * DCMIPP P0FSCR Register Bits (Pipe0 Flow Selection Configuration)
 ****************************************************************************/

#define DCMIPP_P0FSCR_DTMODE_Pos 0
#define DCMIPP_P0FSCR_DTMODE_Msk (0x3 << DCMIPP_P0FSCR_DTMODE_Pos)
#define DCMIPP_P0FSCR_DTIDA_Pos  4
#define DCMIPP_P0FSCR_DTIDA_Msk  (0x3F << DCMIPP_P0FSCR_DTIDA_Pos)
#define DCMIPP_P0FSCR_DTIDB_Pos  12
#define DCMIPP_P0FSCR_DTIDB_Msk  (0x3F << DCMIPP_P0FSCR_DTIDB_Pos)
#define DCMIPP_P0FSCR_VC_Pos     18
#define DCMIPP_P0FSCR_VC_Msk     (0x3 << DCMIPP_P0FSCR_VC_Pos)

#define DCMIPP_DTMODE_DTIDA          (0U << DCMIPP_P0FSCR_DTMODE_Pos)
#define DCMIPP_DTMODE_DTIDA_OR_DTIDB (1U << DCMIPP_P0FSCR_DTMODE_Pos)
#define DCMIPP_DTMODE_ALL_EXCEPT     (2U << DCMIPP_P0FSCR_DTMODE_Pos)
#define DCMIPP_DTMODE_ALL            (3U << DCMIPP_P0FSCR_DTMODE_Pos)

/****************************************************************************
 * DCMIPP P0PPCR Register Bits (Pipe0 Pixel Packer Configuration)
 ****************************************************************************/

#define DCMIPP_P0PPCR_BSM_Pos      2
#define DCMIPP_P0PPCR_BSM_Msk      (0x3 << DCMIPP_P0PPCR_BSM_Pos)
#define DCMIPP_P0PPCR_OEBS         (1 << 4)
#define DCMIPP_P0PPCR_LSM_Pos      5
#define DCMIPP_P0PPCR_LSM_Msk      (0x1 << DCMIPP_P0PPCR_LSM_Pos)
#define DCMIPP_P0PPCR_OELS         (1 << 6)
#define DCMIPP_P0PPCR_LINEMULT_Pos 10
#define DCMIPP_P0PPCR_LINEMULT_Msk (0x7 << DCMIPP_P0PPCR_LINEMULT_Pos)

/****************************************************************************
 * DCMIPP P1PPCR Register Bits (Pipe1 Pixel Packer Configuration)
 ****************************************************************************/

#define DCMIPP_P1PPCR_FORMAT_Pos 0
#define DCMIPP_P1PPCR_FORMAT_Msk (0xF << DCMIPP_P1PPCR_FORMAT_Pos)
#define DCMIPP_P1PPCR_LMAWM_Pos 16
#define DCMIPP_P1PPCR_LMAWM_Msk (0x7 << DCMIPP_P1PPCR_LMAWM_Pos)

/* Pixel packer formats */

#define DCMIPP_PIXEL_PACKER_RGB888_YUV444_1 0U
#define DCMIPP_PIXEL_PACKER_RGB565_1       (1U << DCMIPP_P1PPCR_FORMAT_Pos)
#define DCMIPP_PIXEL_PACKER_ARGB8888       (2U << DCMIPP_P1PPCR_FORMAT_Pos)
#define DCMIPP_PIXEL_PACKER_RGBA888        (3U << DCMIPP_P1PPCR_FORMAT_Pos)
#define DCMIPP_PIXEL_PACKER_MONO_Y8        (4U << DCMIPP_P1PPCR_FORMAT_Pos)
#define DCMIPP_PIXEL_PACKER_YUV444_1       (5U << DCMIPP_P1PPCR_FORMAT_Pos)
#define DCMIPP_PIXEL_PACKER_YUV422_1       (6U << DCMIPP_P1PPCR_FORMAT_Pos)
#define DCMIPP_PIXEL_PACKER_YUV422_2       (7U << DCMIPP_P1PPCR_FORMAT_Pos)
#define DCMIPP_PIXEL_PACKER_YUV420_2       (8U << DCMIPP_P1PPCR_FORMAT_Pos)
#define DCMIPP_PIXEL_PACKER_YUV420_3       (9U << DCMIPP_P1PPCR_FORMAT_Pos)

/****************************************************************************
 * DCMIPP P1DECR Register Bits (Pipe1 Decimation)
 ****************************************************************************/

#define DCMIPP_P1DECR_VDEC_Pos 0
#define DCMIPP_P1DECR_VDEC_Msk (0x3 << DCMIPP_P1DECR_VDEC_Pos)
#define DCMIPP_P1DECR_HDEC_Pos 4
#define DCMIPP_P1DECR_HDEC_Msk (0x3 << DCMIPP_P1DECR_HDEC_Pos)

/****************************************************************************
 * DCMIPP P0SCSZR Register Bits
 ****************************************************************************/

#define DCMIPP_P0SCSZR_POSNEG (1 << 28)

/****************************************************************************
 * DCMIPP CMIER Register Bits (Common Interrupt Enable)
 ****************************************************************************/

#define DCMIPP_CMIER_ATXERRIE  (1 << 0)   /* AXI transfer error */
#define DCMIPP_CMIER_PRERRIE   (1 << 1)   /* Parallel sync error */
#define DCMIPP_CMIER_P0FRAMEIE (1 << 8)   /* Pipe0 frame complete */
#define DCMIPP_CMIER_P0VSYNCIE (1 << 9)   /* Pipe0 vsync */
#define DCMIPP_CMIER_P0LINEIE  (1 << 10)  /* Pipe0 multiline */
#define DCMIPP_CMIER_P0LIMITIE (1 << 11)  /* Pipe0 limit */
#define DCMIPP_CMIER_P0OVRIE   (1 << 12)  /* Pipe0 overrun */
#define DCMIPP_CMIER_P1LINEIE  (1 << 13)  /* Pipe1 multiline */
#define DCMIPP_CMIER_P1FRAMEIE (1 << 14)  /* Pipe1 frame complete */
#define DCMIPP_CMIER_P1VSYNCIE (1 << 15)  /* Pipe1 vsync */
#define DCMIPP_CMIER_P1OVRIE   (1 << 16)  /* Pipe1 overrun */
#define DCMIPP_CMIER_P2LINEIE  (1 << 17)  /* Pipe2 multiline */
#define DCMIPP_CMIER_P2FRAMEIE (1 << 18)  /* Pipe2 frame complete */
#define DCMIPP_CMIER_P2VSYNCIE (1 << 19)  /* Pipe2 vsync */
#define DCMIPP_CMIER_P2OVRIE   (1 << 20)  /* Pipe2 overrun */

/****************************************************************************
 * DCMIPP CMSR2/CMFCR Register Bits (Status / Flag Clear)
 ****************************************************************************/

#define DCMIPP_CMSR2_ATXERRF  (1 << 0)
#define DCMIPP_CMSR2_PRERRF   (1 << 1)
#define DCMIPP_CMSR2_P0FRAMEF (1 << 8)
#define DCMIPP_CMSR2_P0VSYNCF (1 << 9)
#define DCMIPP_CMSR2_P0LINEF  (1 << 10)
#define DCMIPP_CMSR2_P0LIMITF (1 << 11)
#define DCMIPP_CMSR2_P0OVRF   (1 << 12)
#define DCMIPP_CMSR2_P1LINEF  (1 << 13)
#define DCMIPP_CMSR2_P1FRAMEF (1 << 14)
#define DCMIPP_CMSR2_P1VSYNCF (1 << 15)
#define DCMIPP_CMSR2_P1OVRF   (1 << 16)
#define DCMIPP_CMSR2_P2LINEF  (1 << 17)
#define DCMIPP_CMSR2_P2FRAMEF (1 << 18)
#define DCMIPP_CMSR2_P2VSYNCF (1 << 19)
#define DCMIPP_CMSR2_P2OVRF   (1 << 20)

/****************************************************************************
 * CSI Register Offsets
 ****************************************************************************/

#define CSI_CR_OFFSET          0x0000
#define CSI_CFGR_OFFSET        0x0004
#define CSI_LMCFGR_OFFSET      0x0010
#define CSI_DLSCR_OFFSET       0x0014
#define CSI_PCR_OFFSET         0x0018
#define CSI_GCR_OFFSET         0x0020
#define CSI_IER0_OFFSET        0x0040
#define CSI_SR0_OFFSET         0x0044
#define CSI_FCR0_OFFSET        0x0048
#define CSI_IER1_OFFSET        0x0050
#define CSI_SR1_OFFSET         0x0054
#define CSI_FCR1_OFFSET        0x0058
#define CSI_VSCR_OFFSET        0x0070

/* Virtual channel registers (base + 0x100 * vc) */

#define CSI_VCCR0_OFFSET       0x0100
#define CSI_VCSR0_OFFSET       0x0104
#define CSI_VCFCR0_OFFSET      0x0108
#define CSI_VCCR1_OFFSET       0x0200
#define CSI_VCSR1_OFFSET       0x0204
#define CSI_VCFCR1_OFFSET      0x0208
#define CSI_VCCR2_OFFSET       0x0300
#define CSI_VCCR3_OFFSET       0x0400

/* PHY registers */

#define CSI_DL0CR_OFFSET       0x0500
#define CSI_DL1CR_OFFSET       0x0510
#define CSI_DL2CR_OFFSET       0x0520
#define CSI_DL3CR_OFFSET       0x0530
#define CSI_DL4CR_OFFSET       0x0540
#define CSI_DL5CR_OFFSET       0x0550
#define CSI_PHY_CTRL_OFFSET    0x0600
#define CSI_PHY_SCR_OFFSET     0x0604
#define CSI_PHY_PLLCR_OFFSET   0x0610
#define CSI_PHY_TST_CTRL0_OFFSET 0x0620

/****************************************************************************
 * CSI CR Register Bits
 ****************************************************************************/

#define CSI_CR_CSIEN           (1 << 0)   /* CSI enable */

/****************************************************************************
 * CSI LMCFGR Register Bits (Lane Merger Configuration)
 ****************************************************************************/

#define CSI_LMCFGR_LANENB_Pos  4
#define CSI_LMCFGR_LANENB_Msk  (0x3 << CSI_LMCFGR_LANENB_Pos)
#define CSI_LMCFGR_DL0MAP_Pos  8
#define CSI_LMCFGR_DL0MAP_Msk  (0x3 << CSI_LMCFGR_DL0MAP_Pos)
#define CSI_LMCFGR_DL1MAP_Pos  12
#define CSI_LMCFGR_DL1MAP_Msk  (0x3 << CSI_LMCFGR_DL1MAP_Pos)

#define CSI_ONE_DATA_LANE      (1U << CSI_LMCFGR_LANENB_Pos)
#define CSI_TWO_DATA_LANES     (2U << CSI_LMCFGR_LANENB_Pos)

/****************************************************************************
 * CSI PCR Register Bits (PHY Control)
 ****************************************************************************/

#define CSI_PCR_CSION          (1 << 0)   /* D-PHY enable */
#define CSI_PCR_IFON           (1 << 1)   /* Interface enable */
#define CSI_PCR_PRGREN         (1 << 4)   /* PRBS generator enable */

/****************************************************************************
 * CSI PHY PLLCR Register Bits
 ****************************************************************************/

#define CSI_PHY_PLLCR_PLLEN   (1 << 0)
#define CSI_PHY_PLLCR_PLLRDY  (1 << 16)

/****************************************************************************
 * CSI IER0/SR0/FCR0 Register Bits (Interrupt Enable/Status/Flag Clear)
 ****************************************************************************/

#define CSI_IER0_SYNCERRIE    (1 << 0)
#define CSI_IER0_WDERRIE      (1 << 1)
#define CSI_IER0_SPKTERRIE    (1 << 2)
#define CSI_IER0_IDERRIE      (1 << 3)
#define CSI_IER0_CECCERRIE    (1 << 4)
#define CSI_IER0_ECCERRIE     (1 << 5)
#define CSI_IER0_CRCERRIE     (1 << 6)
#define CSI_IER0_CCFIFOFIE    (1 << 7)
#define CSI_IER0_SPKTIE       (1 << 8)
#define CSI_IER0_EOF3IE       (1 << 9)
#define CSI_IER0_EOF2IE       (1 << 10)
#define CSI_IER0_EOF1IE       (1 << 11)
#define CSI_IER0_EOF0IE       (1 << 12)
#define CSI_IER0_SOF3IE       (1 << 13)
#define CSI_IER0_SOF2IE       (1 << 14)
#define CSI_IER0_SOF1IE       (1 << 15)
#define CSI_IER0_SOF0IE       (1 << 16)
#define CSI_IER0_TIM3IE       (1 << 17)
#define CSI_IER0_TIM2IE       (1 << 18)
#define CSI_IER0_TIM1IE       (1 << 19)
#define CSI_IER0_TIM0IE       (1 << 20)
#define CSI_IER0_LB3IE        (1 << 21)
#define CSI_IER0_LB2IE        (1 << 22)
#define CSI_IER0_LB1IE        (1 << 23)
#define CSI_IER0_LB0IE        (1 << 24)

#define CSI_SR0_SYNCERRF      (1 << 0)
#define CSI_SR0_WDERRF        (1 << 1)
#define CSI_SR0_SPKTERRF      (1 << 2)
#define CSI_SR0_IDERRF        (1 << 3)
#define CSI_SR0_CECCERRF      (1 << 4)
#define CSI_SR0_ECCERRF       (1 << 5)
#define CSI_SR0_CRCERRF       (1 << 6)
#define CSI_SR0_CCFIFOFF      (1 << 7)
#define CSI_SR0_SPKTF         (1 << 8)
#define CSI_SR0_EOF3F         (1 << 9)
#define CSI_SR0_EOF2F         (1 << 10)
#define CSI_SR0_EOF1F         (1 << 11)
#define CSI_SR0_EOF0F         (1 << 12)
#define CSI_SR0_SOF3F         (1 << 13)
#define CSI_SR0_SOF2F         (1 << 14)
#define CSI_SR0_SOF1F         (1 << 15)
#define CSI_SR0_SOF0F         (1 << 16)

/* DPHY interrupts (IER1/SR1) */

#define CSI_IER1_ESOTDL0IE    (1 << 0)
#define CSI_IER1_ESOTSYNCDL0IE (1 << 1)
#define CSI_IER1_EESCDL0IE    (1 << 2)
#define CSI_IER1_ESYNCESCDL0IE (1 << 3)
#define CSI_IER1_ECTRLDL0IE   (1 << 4)
#define CSI_IER1_ESOTDL1IE    (1 << 5)
#define CSI_IER1_ESOTSYNCDL1IE (1 << 6)
#define CSI_IER1_EESCDL1IE    (1 << 7)
#define CSI_IER1_ESYNCESCDL1IE (1 << 8)
#define CSI_IER1_ECTRLDL1IE   (1 << 9)

/****************************************************************************
 * CSI Data Types
 ****************************************************************************/

#define CSI_DT_YUV420_8    0x18U
#define CSI_DT_YUV420_10   0x19U
#define CSI_DT_YUV422_8    0x1EU
#define CSI_DT_YUV422_10   0x1FU
#define CSI_DT_RGB444      0x20U
#define CSI_DT_RGB555      0x21U
#define CSI_DT_RGB565      0x22U
#define CSI_DT_RGB666      0x23U
#define CSI_DT_RGB888      0x24U
#define CSI_DT_RAW8        0x2AU
#define CSI_DT_RAW10       0x2BU
#define CSI_DT_RAW12       0x2CU
#define CSI_DT_RAW14       0x2DU

/****************************************************************************
 * CSI PHY Bitrate Values
 ****************************************************************************/

#define CSI_PHY_BT_80      0U
#define CSI_PHY_BT_90      1U
#define CSI_PHY_BT_100     2U
#define CSI_PHY_BT_110     3U
#define CSI_PHY_BT_120     4U
#define CSI_PHY_BT_130     5U
#define CSI_PHY_BT_140     6U
#define CSI_PHY_BT_150     7U
#define CSI_PHY_BT_160     8U
#define CSI_PHY_BT_170     9U
#define CSI_PHY_BT_180     10U
#define CSI_PHY_BT_190     11U
#define CSI_PHY_BT_205     12U
#define CSI_PHY_BT_220     13U
#define CSI_PHY_BT_235     14U
#define CSI_PHY_BT_250     15U
#define CSI_PHY_BT_275     16U
#define CSI_PHY_BT_300     17U
#define CSI_PHY_BT_325     18U
#define CSI_PHY_BT_350     19U
#define CSI_PHY_BT_400     20U
#define CSI_PHY_BT_450     21U
#define CSI_PHY_BT_500     22U
#define CSI_PHY_BT_550     23U
#define CSI_PHY_BT_600     24U
#define CSI_PHY_BT_650     25U
#define CSI_PHY_BT_700     26U
#define CSI_PHY_BT_750     27U
#define CSI_PHY_BT_800     28U
#define CSI_PHY_BT_850     29U
#define CSI_PHY_BT_900     30U
#define CSI_PHY_BT_950     31U
#define CSI_PHY_BT_1000    32U
#define CSI_PHY_BT_1050    33U
#define CSI_PHY_BT_1100    34U
#define CSI_PHY_BT_1150    35U
#define CSI_PHY_BT_1200    36U
#define CSI_PHY_BT_1250    37U
#define CSI_PHY_BT_1300    38U
#define CSI_PHY_BT_1350    39U
#define CSI_PHY_BT_1400    40U
#define CSI_PHY_BT_1450    41U
#define CSI_PHY_BT_1500    42U
#define CSI_PHY_BT_1550    43U
#define CSI_PHY_BT_1600    44U
#define CSI_PHY_BT_1650    45U
#define CSI_PHY_BT_1700    46U
#define CSI_PHY_BT_1750    47U
#define CSI_PHY_BT_1800    48U
#define CSI_PHY_BT_1850    49U
#define CSI_PHY_BT_1900    50U
#define CSI_PHY_BT_1950    51U
#define CSI_PHY_BT_2000    52U
#define CSI_PHY_BT_2050    53U
#define CSI_PHY_BT_2100    54U
#define CSI_PHY_BT_2150    55U
#define CSI_PHY_BT_2200    56U
#define CSI_PHY_BT_2250    57U
#define CSI_PHY_BT_2300    58U
#define CSI_PHY_BT_2350    59U
#define CSI_PHY_BT_2400    60U
#define CSI_PHY_BT_2450    61U
#define CSI_PHY_BT_2500    62U

/****************************************************************************
 * RCC DCMIPP Clock/Reset Definitions
 ****************************************************************************/

#define RCC_APB5ENR_DCMIPPEN  (1 << 2)    /* DCMIPP clock enable */
#define RCC_APB5RSTR_DCMIPPRST (1 << 2)   /* DCMIPP reset */

/****************************************************************************
 * Inline Register Access Macros
 ****************************************************************************/

/* DCMIPP register access */

#define DCMIPP_REG(offset) \
  (*(volatile uint32_t *)(DCMIPP_BASE + (offset)))

/* CSI register access */

#define CSI_REG(offset) \
  (*(volatile uint32_t *)(CSI_BASE + (offset)))

/****************************************************************************
 * DCMIPP_TypeDef - register layout for direct register access
 ****************************************************************************/

typedef struct
{
  volatile uint32_t IPGR1;       /* 0x000 */
  volatile uint32_t IPGR2;       /* 0x004 */
  volatile uint32_t IPGR3;       /* 0x008 */
  volatile uint32_t RESERVED0[4];
  volatile uint32_t IPGR8;       /* 0x01C */
  volatile uint32_t IPC1R1;      /* 0x020 */
  volatile uint32_t IPC1R2;      /* 0x024 */
  volatile uint32_t IPC1R3;      /* 0x028 */
  volatile uint32_t RESERVED1;
  volatile uint32_t IPC2R1;      /* 0x030 */
  volatile uint32_t IPC2R2;      /* 0x034 */
  volatile uint32_t IPC2R3;      /* 0x038 */
  volatile uint32_t RESERVED2;
  volatile uint32_t IPC3R1;      /* 0x040 */
  volatile uint32_t IPC3R2;      /* 0x044 */
  volatile uint32_t IPC3R3;      /* 0x048 */
  volatile uint32_t RESERVED3;
  volatile uint32_t IPC4R1;      /* 0x050 */
  volatile uint32_t IPC4R2;      /* 0x054 */
  volatile uint32_t IPC4R3;      /* 0x058 */
  volatile uint32_t RESERVED4;
  volatile uint32_t IPC5R1;      /* 0x060 */
  volatile uint32_t IPC5R2;      /* 0x064 */
  volatile uint32_t IPC5R3;      /* 0x068 */
  volatile uint32_t RESERVED5[38];
  volatile uint32_t PRCR;        /* 0x104 */
  volatile uint32_t PRESCR;      /* 0x108 */
  volatile uint32_t PRESUR;      /* 0x10C */
  volatile uint32_t RESERVED6[57];
  volatile uint32_t PRIER;       /* 0x1F4 */
  volatile uint32_t PRSR;        /* 0x1F8 */
  volatile uint32_t PRFCR;       /* 0x1FC */
  volatile uint32_t RESERVED7;
  volatile uint32_t CMCR;        /* 0x204 */
  volatile uint32_t CMFRCR;      /* 0x208 */
  volatile uint32_t RESERVED8[121];
  volatile uint32_t CMIER;       /* 0x3F0 */
  volatile uint32_t CMSR1;       /* 0x3F4 */
  volatile uint32_t CMSR2;       /* 0x3F8 */
  volatile uint32_t CMFCR;       /* 0x3FC */
  volatile uint32_t RESERVED9;
  volatile uint32_t P0FSCR;      /* 0x404 */
  volatile uint32_t RESERVED10[62];
  volatile uint32_t P0FCTCR;     /* 0x500 */
  volatile uint32_t P0SCSTR;     /* 0x504 */
  volatile uint32_t P0SCSZR;     /* 0x508 */
  volatile uint32_t RESERVED11[41];
  volatile uint32_t P0DCCNTR;    /* 0x5B0 */
  volatile uint32_t P0DCLMTR;    /* 0x5B4 */
  volatile uint32_t RESERVED12[2];
  volatile uint32_t P0PPCR;      /* 0x5C0 */
  volatile uint32_t P0PPM0AR1;   /* 0x5C4 */
  volatile uint32_t P0PPM0AR2;   /* 0x5C8 */
  volatile uint32_t RESERVED13;
  volatile uint32_t P0STM0AR;    /* 0x5D0 */
  volatile uint32_t RESERVED14[8];
  volatile uint32_t P0IER;       /* 0x5F4 */
  volatile uint32_t P0SR;        /* 0x5F8 */
  volatile uint32_t P0FCR;       /* 0x5FC */
  volatile uint32_t RESERVED15;
  volatile uint32_t P0CFSCR;     /* 0x604 */
  volatile uint32_t RESERVED17[62];
  volatile uint32_t P0CFCTCR;    /* 0x700 */
  volatile uint32_t P0CSCSTR;    /* 0x704 */
  volatile uint32_t P0CSCSZR;    /* 0x708 */
  volatile uint32_t RESERVED18[45];
  volatile uint32_t P0CPPCR;     /* 0x7C0 */
  volatile uint32_t P0CPPM0AR1;  /* 0x7C4 */
  volatile uint32_t P0CPPM0AR2;  /* 0x7C8 */
  volatile uint32_t RESERVED19[14];
  volatile uint32_t P1FSCR;      /* 0x804 */
  volatile uint32_t RESERVED20[6];
  volatile uint32_t P1SRCR;      /* 0x820 */
  volatile uint32_t P1BPRCR;     /* 0x824 */
  volatile uint32_t P1BPRSR;     /* 0x828 */
  volatile uint32_t RESERVED21;
  volatile uint32_t P1DECR;      /* 0x830 */
  volatile uint32_t RESERVED22[3];
  volatile uint32_t P1BLCCR;     /* 0x840 */
  volatile uint32_t P1EXCR1;     /* 0x844 */
  volatile uint32_t P1EXCR2;     /* 0x848 */
  volatile uint32_t RESERVED23;
  volatile uint32_t P1ST1CR;     /* 0x850 */
  volatile uint32_t P1ST2CR;     /* 0x854 */
  volatile uint32_t P1ST3CR;     /* 0x858 */
  volatile uint32_t P1STSTR;     /* 0x85C */
  volatile uint32_t P1STSZR;     /* 0x860 */
  volatile uint32_t P1ST1SR;     /* 0x864 */
  volatile uint32_t P1ST2SR;     /* 0x868 */
  volatile uint32_t P1ST3SR;     /* 0x86C */
  volatile uint32_t P1DMCR;      /* 0x870 */
  volatile uint32_t RESERVED24[3];
  volatile uint32_t P1CCCR;      /* 0x880 */
  volatile uint32_t P1CCRR1;     /* 0x884 */
  volatile uint32_t P1CCRR2;     /* 0x888 */
  volatile uint32_t P1CCGR1;     /* 0x88C */
  volatile uint32_t P1CCGR2;     /* 0x890 */
  volatile uint32_t P1CCBR1;     /* 0x894 */
  volatile uint32_t P1CCBR2;     /* 0x898 */
  volatile uint32_t RESERVED25;
  volatile uint32_t P1CTCR1;     /* 0x8A0 */
  volatile uint32_t P1CTCR2;     /* 0x8A4 */
  volatile uint32_t P1CTCR3;     /* 0x8A8 */
  volatile uint32_t RESERVED26[21];
  volatile uint32_t P1FCTCR;     /* 0x900 */
  volatile uint32_t P1CRSTR;     /* 0x904 */
  volatile uint32_t P1CRSZR;     /* 0x908 */
  volatile uint32_t P1DCCR;      /* 0x90C */
  volatile uint32_t P1DSCR;      /* 0x910 */
  volatile uint32_t P1DSRTIOR;   /* 0x914 */
  volatile uint32_t P1DSSZR;     /* 0x918 */
  volatile uint32_t RESERVED28;
  volatile uint32_t P1CMRICR;    /* 0x920 */
  volatile uint32_t P1RIxCR1;    /* 0x924 */
  volatile uint32_t P1RIxCR2;    /* 0x928 */
  volatile uint32_t RESERVED29[17];
  volatile uint32_t P1GMCR;      /* 0x970 */
  volatile uint32_t RESERVED30[3];
  volatile uint32_t P1YUVCR;     /* 0x980 */
  volatile uint32_t P1YUVRR1;    /* 0x984 */
  volatile uint32_t P1YUVRR2;    /* 0x988 */
  volatile uint32_t P1YUVGR1;    /* 0x98C */
  volatile uint32_t P1YUVGR2;    /* 0x990 */
  volatile uint32_t P1YUVBR1;    /* 0x994 */
  volatile uint32_t P1YUVBR2;    /* 0x998 */
  volatile uint32_t RESERVED31[9];
  volatile uint32_t P1PPCR;      /* 0x9C0 */
  volatile uint32_t P1PPM0AR1;   /* 0x9C4 */
  volatile uint32_t P1PPM0AR2;   /* 0x9C8 */
  volatile uint32_t P1PPM0PR;    /* 0x9CC */
  volatile uint32_t P1STM0AR;    /* 0x9D0 */
  volatile uint32_t P1PPM1AR1;   /* 0x9D4 */
  volatile uint32_t P1PPM1AR2;   /* 0x9D8 */
  volatile uint32_t P1PPM1PR;    /* 0x9DC */
  volatile uint32_t P1STM1AR;    /* 0x9E0 */
  volatile uint32_t P1PPM2AR1;   /* 0x9E4 */
  volatile uint32_t P1PPM2AR2;   /* 0x9E8 */
  volatile uint32_t RESERVED34;
  volatile uint32_t P1STM2AR;    /* 0x9F0 */
  volatile uint32_t P1IER;       /* 0x9F4 */
  volatile uint32_t P1SR;        /* 0x9F8 */
  volatile uint32_t P1FCR;       /* 0x9FC */
  volatile uint32_t RESERVED35;
  volatile uint32_t P1CFSCR;     /* 0xA04 */
  volatile uint32_t RESERVED36[7];
  volatile uint32_t P1CBPRCR;    /* 0xA24 */
  volatile uint32_t RESERVED37[6];
  volatile uint32_t P1CBLCCR;    /* 0xA40 */
  volatile uint32_t P1CEXCR1;    /* 0xA44 */
  volatile uint32_t P1CEXCR2;    /* 0xA48 */
  volatile uint32_t RESERVED38;
  volatile uint32_t P1CST1CR;    /* 0xA50 */
  volatile uint32_t P1CST2CR;    /* 0xA54 */
  volatile uint32_t P1CST3CR;    /* 0xA58 */
  volatile uint32_t P1CSTSTR;    /* 0xA5C */
  volatile uint32_t P1CSTSZR;    /* 0xA60 */
  volatile uint32_t RESERVED39[7];
  volatile uint32_t P1CCCCR;     /* 0xA80 */
  volatile uint32_t P1CCCRR1;    /* 0xA84 */
  volatile uint32_t P1CCCRR2;    /* 0xA88 */
  volatile uint32_t P1CCCGR1;    /* 0xA8C */
  volatile uint32_t P1CCCGR2;    /* 0xA90 */
  volatile uint32_t P1CCCBR1;    /* 0xA94 */
  volatile uint32_t P1CCCBR2;    /* 0xA98 */
  volatile uint32_t RESERVED40;
  volatile uint32_t P1CCTCR1;    /* 0xAA0 */
  volatile uint32_t P1CCTCR2;    /* 0xAA4 */
  volatile uint32_t P1CCTCR3;    /* 0xAA8 */
  volatile uint32_t RESERVED41[21];
  volatile uint32_t P1CFCTCR;    /* 0xB00 */
  volatile uint32_t P1CCRSTR;    /* 0xB04 */
  volatile uint32_t P1CCRSZR;    /* 0xB08 */
  volatile uint32_t P1CDCCR;     /* 0xB0C */
  volatile uint32_t P1CDSCR;     /* 0xB10 */
  volatile uint32_t P1CDSRTIOR;  /* 0xB14 */
  volatile uint32_t P1CDSSZR;    /* 0xB18 */
  volatile uint32_t RESERVED43;
  volatile uint32_t P1CCMRICR;   /* 0xB20 */
  volatile uint32_t P1CRIxCR1;   /* 0xB24 */
  volatile uint32_t P1CRIxCR2;   /* 0xB28 */
  volatile uint32_t RESERVED44[37];
  volatile uint32_t P1CPPCR;     /* 0xBC0 */
  volatile uint32_t P1CPPM0AR1;  /* 0xBC4 */
  volatile uint32_t P1CPPM0AR2;  /* 0xBC8 */
  volatile uint32_t P1CPPM0PR;   /* 0xBCC */
  volatile uint32_t RESERVED45;
  volatile uint32_t P1CPPM1AR1;  /* 0xBD4 */
  volatile uint32_t P1CPPM1AR2;  /* 0xBD8 */
  volatile uint32_t P1CPPM1PR;   /* 0xBDC */
  volatile uint32_t RESERVED47;
  volatile uint32_t P1CPPM2AR1;  /* 0xBE4 */
  volatile uint32_t P1CPPM2AR2;  /* 0xBE8 */
  volatile uint32_t RESERVED48[6];
  volatile uint32_t P2FSCR;      /* 0xC04 */
  volatile uint32_t RESERVED49[62];
  volatile uint32_t P2FCTCR;     /* 0xD00 */
  volatile uint32_t P2CRSTR;     /* 0xD04 */
  volatile uint32_t P2CRSZR;     /* 0xD08 */
  volatile uint32_t P2DCCR;      /* 0xD0C */
  volatile uint32_t P2DSCR;      /* 0xD10 */
  volatile uint32_t P2DSRTIOR;   /* 0xD14 */
  volatile uint32_t P2DSSZR;     /* 0xD18 */
  volatile uint32_t RESERVED51;
  volatile uint32_t P2CMRICR;    /* 0xD20 */
  volatile uint32_t P2RIxCR1;    /* 0xD24 */
  volatile uint32_t P2RIxCR2;    /* 0xD28 */
  volatile uint32_t RESERVED53[17];
  volatile uint32_t P2GMCR;      /* 0xD70 */
  volatile uint32_t RESERVED54[19];
  volatile uint32_t P2PPCR;      /* 0xDC0 */
  volatile uint32_t P2PPM0AR1;   /* 0xDC4 */
  volatile uint32_t P2PPM0AR2;   /* 0xDC8 */
  volatile uint32_t P2PPM0PR;    /* 0xDCC */
  volatile uint32_t P2STM0AR;    /* 0xDD0 */
  volatile uint32_t RESERVED55[8];
  volatile uint32_t P2IER;       /* 0xDF4 */
  volatile uint32_t P2SR;        /* 0xDF8 */
  volatile uint32_t P2FCR;       /* 0xDFC */
  volatile uint32_t RESERVED56;
  volatile uint32_t P2CFSCR;     /* 0xE04 */
  volatile uint32_t RESERVED57[62];
  volatile uint32_t P2CFCTCR;    /* 0xF00 */
  volatile uint32_t P2CCRSTR;    /* 0xF04 */
  volatile uint32_t P2CCRSZR;    /* 0xF08 */
  volatile uint32_t P2CDCCR;     /* 0xF0C */
  volatile uint32_t P2CDSCR;     /* 0xF10 */
  volatile uint32_t P2CDSRTIOR;  /* 0xF14 */
  volatile uint32_t P2CDSSZR;    /* 0xF18 */
  volatile uint32_t RESERVED59[2];
  volatile uint32_t P2CRIxCR1;   /* 0xF24 */
  volatile uint32_t P2CRIxCR2;   /* 0xF28 */
  volatile uint32_t RESERVED60[37];
  volatile uint32_t P2CPPCR;     /* 0xFC0 */
  volatile uint32_t P2CPPM0AR1;  /* 0xFC4 */
  volatile uint32_t P2CPPM0AR2;  /* 0xFC8 */
  volatile uint32_t P2CPPM0PR;   /* 0xFCC */
  volatile uint32_t RESERVED61[7];
  volatile uint32_t HWCFGR2;     /* 0xFEC */
  volatile uint32_t HWCFGR1;     /* 0xFF0 */
  volatile uint32_t VERR;        /* 0xFF4 */
  volatile uint32_t IPIDR;       /* 0xFF8 */
  volatile uint32_t SIDR;        /* 0xFFC */
} dcmipp_regs_t;

#define DCMIPP ((dcmipp_regs_t *)DCMIPP_BASE)

/****************************************************************************
 * CSI_TypeDef - register layout for direct register access
 ****************************************************************************/

typedef struct
{
  volatile uint32_t CR;          /* 0x000 */
  volatile uint32_t CFGR;        /* 0x004 */
  volatile uint32_t RESERVED0[2];
  volatile uint32_t LMCFGR;     /* 0x010 */
  volatile uint32_t DLSCR;       /* 0x014 */
  volatile uint32_t PCR;         /* 0x018 */
  volatile uint32_t RESERVED1;
  volatile uint32_t GCR;         /* 0x020 */
  volatile uint32_t RESERVED2[7];
  volatile uint32_t IER0;        /* 0x040 */
  volatile uint32_t SR0;         /* 0x044 */
  volatile uint32_t FCR0;        /* 0x048 */
  volatile uint32_t RESERVED3;
  volatile uint32_t IER1;        /* 0x050 */
  volatile uint32_t SR1;         /* 0x054 */
  volatile uint32_t FCR1;        /* 0x058 */
  volatile uint32_t RESERVED4[5];
  volatile uint32_t VSCR;        /* 0x070 */
  volatile uint32_t RESERVED5[35];
  volatile uint32_t VCCR0;       /* 0x100 */
  volatile uint32_t VCSR0;       /* 0x104 */
  volatile uint32_t VCFCR0;      /* 0x108 */
  volatile uint32_t RESERVED6[61];
  volatile uint32_t VCCR1;       /* 0x200 */
  volatile uint32_t VCSR1;       /* 0x204 */
  volatile uint32_t VCFCR1;      /* 0x208 */
  volatile uint32_t RESERVED7[61];
  volatile uint32_t VCCR2;       /* 0x300 */
  volatile uint32_t VCSR2;       /* 0x304 */
  volatile uint32_t VCFCR2;      /* 0x308 */
  volatile uint32_t RESERVED8[61];
  volatile uint32_t VCCR3;       /* 0x400 */
  volatile uint32_t VCSR3;       /* 0x404 */
  volatile uint32_t VCFCR3;      /* 0x408 */
  volatile uint32_t RESERVED9[61];
  volatile uint32_t DL0CR;       /* 0x500 */
  volatile uint32_t RESERVED10[3];
  volatile uint32_t DL1CR;       /* 0x510 */
  volatile uint32_t RESERVED11[3];
  volatile uint32_t DL2CR;       /* 0x520 */
  volatile uint32_t RESERVED12[3];
  volatile uint32_t DL3CR;       /* 0x530 */
  volatile uint32_t RESERVED13[3];
  volatile uint32_t DL4CR;       /* 0x540 */
  volatile uint32_t RESERVED14[3];
  volatile uint32_t DL5CR;       /* 0x550 */
  volatile uint32_t RESERVED15[43];
  volatile uint32_t PHY_CTRL;    /* 0x600 */
  volatile uint32_t PHY_SCR;     /* 0x604 */
  volatile uint32_t RESERVED16[2];
  volatile uint32_t PHY_PLLCR;   /* 0x610 */
  volatile uint32_t RESERVED17[3];
  volatile uint32_t PHY_TST_CTRL0; /* 0x620 */
  volatile uint32_t RESERVED18[3];
  volatile uint32_t PHY_TST_CTRL1; /* 0x630 */
  volatile uint32_t RESERVED19[243];
  volatile uint32_t PTSCR;       /* 0x1014 */
  volatile uint32_t PTSR;        /* 0x1018 */
} csi_regs_t;

#define CSI ((csi_regs_t *)CSI_BASE)

#endif /* __ARCH_ARM_SRC_STM32N6_HARDWARE_STM32_DCMIPP_H */
