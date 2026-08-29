/****************************************************************************
 * arch/arm/src/stm32n6/hardware/stm32_ltdc.h
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

#ifndef __ARCH_ARM_SRC_STM32N6_HARDWARE_STM32_LTDC_H
#define __ARCH_ARM_SRC_STM32N6_HARDWARE_STM32_LTDC_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* LTDC Base Address ********************************************************/

#define STM32_LTDC_BASE            0x48001000

/* LTDC Register Offsets ****************************************************/

/* Global registers - offsets from LTDC base (per RM0486 / CMSIS) */

#define STM32_LTDC_SSCR_OFFSET     0x0008  /* Synchronization size configuration register */
#define STM32_LTDC_BPCR_OFFSET     0x000c  /* Back porch configuration register */
#define STM32_LTDC_AWCR_OFFSET     0x0010  /* Active width configuration register */
#define STM32_LTDC_TWCR_OFFSET     0x0014  /* Total width configuration register */
#define STM32_LTDC_GCR_OFFSET      0x0018  /* Global control register */
#define STM32_LTDC_SRCR_OFFSET     0x0024  /* Shadow reload configuration register */
#define STM32_LTDC_GCCR_OFFSET     0x0028  /* Gamma correction configuration register */
#define STM32_LTDC_BCCR_OFFSET     0x002c  /* Background color configuration register */
#define STM32_LTDC_IER_OFFSET      0x0034  /* Interrupt enable register */
#define STM32_LTDC_ISR_OFFSET      0x0038  /* Interrupt status register */
#define STM32_LTDC_ICR_OFFSET      0x003c  /* Interrupt clear register */
#define STM32_LTDC_LIPCR_OFFSET    0x0040  /* Line interrupt position configuration register */
#define STM32_LTDC_CPSR_OFFSET     0x0044  /* Current position status register */
#define STM32_LTDC_CDSR_OFFSET     0x0048  /* Current display status register */
#define STM32_LTDC_EDCR_OFFSET     0x0060  /* External display control register */
#define STM32_LTDC_IER2_OFFSET     0x0064  /* Interrupt enable register 2 */
#define STM32_LTDC_ISR2_OFFSET     0x0068  /* Interrupt status register 2 */
#define STM32_LTDC_ICR2_OFFSET     0x006c  /* Interrupt clear register 2 */
#define STM32_LTDC_LIPCR2_OFFSET   0x0070  /* Line interrupt position configuration register 2 */
#define STM32_LTDC_ECRCR_OFFSET    0x0078  /* Expected CRC register */
#define STM32_LTDC_CCRCR_OFFSET    0x007c  /* Computed CRC register */
#define STM32_LTDC_RB0AR_OFFSET    0x0080  /* Rotation buffer 0 address register */
#define STM32_LTDC_RB1AR_OFFSET    0x0084  /* Rotation buffer 1 address register */
#define STM32_LTDC_RBPR_OFFSET     0x0088  /* Rotation buffer pitch register */
#define STM32_LTDC_RIFCR_OFFSET    0x008c  /* Rotation intermediate frame color register */
#define STM32_LTDC_FUTR_OFFSET     0x0090  /* FIFO underrun threshold register */

/* Layer 1 registers - Layer 1 base = LTDC_BASE + 0x100 */

#define STM32_LTDC_L1C0R_OFFSET    0x0100  /* Layer 1 configuration 0 register */
#define STM32_LTDC_L1C1R_OFFSET    0x0104  /* Layer 1 configuration 1 register */
#define STM32_LTDC_L1RCR_OFFSET    0x0108  /* Layer 1 reload control register */
#define STM32_LTDC_L1CR_OFFSET     0x010c  /* Layer 1 control register */
#define STM32_LTDC_L1WHPCR_OFFSET  0x0110  /* Layer 1 window horizontal position configuration register */
#define STM32_LTDC_L1WVPCR_OFFSET  0x0114  /* Layer 1 window vertical position configuration register */
#define STM32_LTDC_L1CKCR_OFFSET   0x0118  /* Layer 1 color keying configuration register */
#define STM32_LTDC_L1PFCR_OFFSET   0x011c  /* Layer 1 pixel format configuration register */
#define STM32_LTDC_L1CACR_OFFSET   0x0120  /* Layer 1 constant alpha configuration register */
#define STM32_LTDC_L1DCCR_OFFSET   0x0124  /* Layer 1 default color configuration register */
#define STM32_LTDC_L1BFCR_OFFSET   0x0128  /* Layer 1 blending factors configuration register */
#define STM32_LTDC_L1BLCR_OFFSET   0x012c  /* Layer 1 burst length configuration register */
#define STM32_LTDC_L1PCR_OFFSET    0x0130  /* Layer 1 planar configuration register */
#define STM32_LTDC_L1CFBAR_OFFSET  0x0134  /* Layer 1 color frame buffer address register */
#define STM32_LTDC_L1CFBLR_OFFSET  0x0138  /* Layer 1 color frame buffer length register */
#define STM32_LTDC_L1CFBLNR_OFFSET 0x013c  /* Layer 1 color frame buffer line number register */
#define STM32_LTDC_L1AFBA0R_OFFSET 0x0140  /* Layer 1 auxiliary frame buffer address 0 register */
#define STM32_LTDC_L1AFBA1R_OFFSET 0x0144  /* Layer 1 auxiliary frame buffer address 1 register */
#define STM32_LTDC_L1AFBLR_OFFSET  0x0148  /* Layer 1 auxiliary frame buffer length register */
#define STM32_LTDC_L1AFBLNR_OFFSET 0x014c  /* Layer 1 auxiliary frame buffer line number register */
#define STM32_LTDC_L1CLUTWR_OFFSET 0x0150  /* Layer 1 CLUT write register */
#define STM32_LTDC_L1SISR_OFFSET   0x0154  /* Layer 1 scaler input size register */
#define STM32_LTDC_L1SOSR_OFFSET   0x0158  /* Layer 1 scaler output size register */
#define STM32_LTDC_L1SVSFR_OFFSET  0x015c  /* Layer 1 scaler vertical scaling factor register */
#define STM32_LTDC_L1SVSPR_OFFSET  0x0160  /* Layer 1 scaler vertical scaling phase register */
#define STM32_LTDC_L1SHSFR_OFFSET  0x0164  /* Layer 1 scaler horizontal scaling factor register */
#define STM32_LTDC_L1SHSPR_OFFSET  0x0168  /* Layer 1 scaler horizontal scaling phase register */
#define STM32_LTDC_L1CYR0R_OFFSET  0x016c  /* Layer 1 conversion YCbCr RGB 0 register */
#define STM32_LTDC_L1CYR1R_OFFSET  0x0170  /* Layer 1 conversion YCbCr RGB 1 register */
#define STM32_LTDC_L1FPF0R_OFFSET  0x0174  /* Layer 1 flexible pixel format 0 register */
#define STM32_LTDC_L1FPF1R_OFFSET  0x0178  /* Layer 1 flexible pixel format 1 register */

/* Layer 2 registers - Layer 2 base = LTDC_BASE + 0x200 */

#define STM32_LTDC_L2C0R_OFFSET    0x0200  /* Layer 2 configuration 0 register */
#define STM32_LTDC_L2C1R_OFFSET    0x0204  /* Layer 2 configuration 1 register */
#define STM32_LTDC_L2RCR_OFFSET    0x0208  /* Layer 2 reload control register */
#define STM32_LTDC_L2CR_OFFSET     0x020c  /* Layer 2 control register */
#define STM32_LTDC_L2WHPCR_OFFSET  0x0210  /* Layer 2 window horizontal position configuration register */
#define STM32_LTDC_L2WVPCR_OFFSET  0x0214  /* Layer 2 window vertical position configuration register */
#define STM32_LTDC_L2CKCR_OFFSET   0x0218  /* Layer 2 color keying configuration register */
#define STM32_LTDC_L2PFCR_OFFSET   0x021c  /* Layer 2 pixel format configuration register */
#define STM32_LTDC_L2CACR_OFFSET   0x0220  /* Layer 2 constant alpha configuration register */
#define STM32_LTDC_L2DCCR_OFFSET   0x0224  /* Layer 2 default color configuration register */
#define STM32_LTDC_L2BFCR_OFFSET   0x0228  /* Layer 2 blending factors configuration register */
#define STM32_LTDC_L2BLCR_OFFSET   0x022c  /* Layer 2 burst length configuration register */
#define STM32_LTDC_L2PCR_OFFSET    0x0230  /* Layer 2 planar configuration register */
#define STM32_LTDC_L2CFBAR_OFFSET  0x0234  /* Layer 2 color frame buffer address register */
#define STM32_LTDC_L2CFBLR_OFFSET  0x0238  /* Layer 2 color frame buffer length register */
#define STM32_LTDC_L2CFBLNR_OFFSET 0x023c  /* Layer 2 color frame buffer line number register */
#define STM32_LTDC_L2AFBA0R_OFFSET 0x0240  /* Layer 2 auxiliary frame buffer address 0 register */
#define STM32_LTDC_L2AFBA1R_OFFSET 0x0244  /* Layer 2 auxiliary frame buffer address 1 register */
#define STM32_LTDC_L2AFBLR_OFFSET  0x0248  /* Layer 2 auxiliary frame buffer length register */
#define STM32_LTDC_L2AFBLNR_OFFSET 0x024c  /* Layer 2 auxiliary frame buffer line number register */
#define STM32_LTDC_L2CLUTWR_OFFSET 0x0250  /* Layer 2 CLUT write register */
#define STM32_LTDC_L2SISR_OFFSET   0x0254  /* Layer 2 scaler input size register */
#define STM32_LTDC_L2SOSR_OFFSET   0x0258  /* Layer 2 scaler output size register */
#define STM32_LTDC_L2SVSFR_OFFSET  0x025c  /* Layer 2 scaler vertical scaling factor register */
#define STM32_LTDC_L2SVSPR_OFFSET  0x0260  /* Layer 2 scaler vertical scaling phase register */
#define STM32_LTDC_L2SHSFR_OFFSET  0x0264  /* Layer 2 scaler horizontal scaling factor register */
#define STM32_LTDC_L2SHSPR_OFFSET  0x0268  /* Layer 2 scaler horizontal scaling phase register */
#define STM32_LTDC_L2CYR0R_OFFSET  0x026c  /* Layer 2 conversion YCbCr RGB 0 register */
#define STM32_LTDC_L2CYR1R_OFFSET  0x0270  /* Layer 2 conversion YCbCr RGB 1 register */
#define STM32_LTDC_L2FPF0R_OFFSET  0x0274  /* Layer 2 flexible pixel format 0 register */
#define STM32_LTDC_L2FPF1R_OFFSET  0x0278  /* Layer 2 flexible pixel format 1 register */

/* LTDC Register Addresses **************************************************/

#define STM32_LTDC_SSCR            (STM32_LTDC_BASE + STM32_LTDC_SSCR_OFFSET)
#define STM32_LTDC_BPCR            (STM32_LTDC_BASE + STM32_LTDC_BPCR_OFFSET)
#define STM32_LTDC_AWCR            (STM32_LTDC_BASE + STM32_LTDC_AWCR_OFFSET)
#define STM32_LTDC_TWCR            (STM32_LTDC_BASE + STM32_LTDC_TWCR_OFFSET)
#define STM32_LTDC_GCR             (STM32_LTDC_BASE + STM32_LTDC_GCR_OFFSET)
#define STM32_LTDC_SRCR            (STM32_LTDC_BASE + STM32_LTDC_SRCR_OFFSET)
#define STM32_LTDC_GCCR            (STM32_LTDC_BASE + STM32_LTDC_GCCR_OFFSET)
#define STM32_LTDC_BCCR            (STM32_LTDC_BASE + STM32_LTDC_BCCR_OFFSET)
#define STM32_LTDC_IER             (STM32_LTDC_BASE + STM32_LTDC_IER_OFFSET)
#define STM32_LTDC_ISR             (STM32_LTDC_BASE + STM32_LTDC_ISR_OFFSET)
#define STM32_LTDC_ICR             (STM32_LTDC_BASE + STM32_LTDC_ICR_OFFSET)
#define STM32_LTDC_LIPCR           (STM32_LTDC_BASE + STM32_LTDC_LIPCR_OFFSET)
#define STM32_LTDC_CPSR            (STM32_LTDC_BASE + STM32_LTDC_CPSR_OFFSET)
#define STM32_LTDC_CDSR            (STM32_LTDC_BASE + STM32_LTDC_CDSR_OFFSET)

#define STM32_LTDC_L1CR            (STM32_LTDC_BASE + STM32_LTDC_L1CR_OFFSET)
#define STM32_LTDC_L1WHPCR         (STM32_LTDC_BASE + STM32_LTDC_L1WHPCR_OFFSET)
#define STM32_LTDC_L1WVPCR         (STM32_LTDC_BASE + STM32_LTDC_L1WVPCR_OFFSET)
#define STM32_LTDC_L1PFCR          (STM32_LTDC_BASE + STM32_LTDC_L1PFCR_OFFSET)
#define STM32_LTDC_L1CACR          (STM32_LTDC_BASE + STM32_LTDC_L1CACR_OFFSET)
#define STM32_LTDC_L1DCCR          (STM32_LTDC_BASE + STM32_LTDC_L1DCCR_OFFSET)
#define STM32_LTDC_L1BFCR          (STM32_LTDC_BASE + STM32_LTDC_L1BFCR_OFFSET)
#define STM32_LTDC_L1CFBAR         (STM32_LTDC_BASE + STM32_LTDC_L1CFBAR_OFFSET)
#define STM32_LTDC_L1CFBLR         (STM32_LTDC_BASE + STM32_LTDC_L1CFBLR_OFFSET)
#define STM32_LTDC_L1CFBLNR        (STM32_LTDC_BASE + STM32_LTDC_L1CFBLNR_OFFSET)
#define STM32_LTDC_L1CLUTWR        (STM32_LTDC_BASE + STM32_LTDC_L1CLUTWR_OFFSET)

#define STM32_LTDC_L2CR            (STM32_LTDC_BASE + STM32_LTDC_L2CR_OFFSET)
#define STM32_LTDC_L2WHPCR         (STM32_LTDC_BASE + STM32_LTDC_L2WHPCR_OFFSET)
#define STM32_LTDC_L2WVPCR         (STM32_LTDC_BASE + STM32_LTDC_L2WVPCR_OFFSET)
#define STM32_LTDC_L2PFCR          (STM32_LTDC_BASE + STM32_LTDC_L2PFCR_OFFSET)
#define STM32_LTDC_L2CACR          (STM32_LTDC_BASE + STM32_LTDC_L2CACR_OFFSET)
#define STM32_LTDC_L2DCCR          (STM32_LTDC_BASE + STM32_LTDC_L2DCCR_OFFSET)
#define STM32_LTDC_L2BFCR          (STM32_LTDC_BASE + STM32_LTDC_L2BFCR_OFFSET)
#define STM32_LTDC_L2CFBAR         (STM32_LTDC_BASE + STM32_LTDC_L2CFBAR_OFFSET)
#define STM32_LTDC_L2CFBLR         (STM32_LTDC_BASE + STM32_LTDC_L2CFBLR_OFFSET)
#define STM32_LTDC_L2CFBLNR        (STM32_LTDC_BASE + STM32_LTDC_L2CFBLNR_OFFSET)
#define STM32_LTDC_L2CLUTWR        (STM32_LTDC_BASE + STM32_LTDC_L2CLUTWR_OFFSET)

/* LTDC register bit definitions ********************************************/

/* LTDC global control register (GCR) */

#define LTDC_GCR_LTDCEN            (1 << 0)   /* Bit 0: LCD-TFT controller enable */
#define LTDC_GCR_DBW_SHIFT         (4)        /* Bits 4-6: Dither blue width */
#define LTDC_GCR_DBW_MASK          (0x7 << LTDC_GCR_DBW_SHIFT)
#define LTDC_GCR_DGW_SHIFT         (8)        /* Bits 8-10: Dither green width */
#define LTDC_GCR_DGW_MASK          (0x7 << LTDC_GCR_DGW_SHIFT)
#define LTDC_GCR_DRW_SHIFT         (12)       /* Bits 12-14: Dither red width */
#define LTDC_GCR_DRW_MASK          (0x7 << LTDC_GCR_DRW_SHIFT)
#define LTDC_GCR_DEN               (1 << 16)  /* Bit 16: Dither enable */
#define LTDC_GCR_PCPOL             (1 << 28)  /* Bit 28: Pixel clock polarity */
#define LTDC_GCR_DEPOL             (1 << 29)  /* Bit 29: Data enable polarity */
#define LTDC_GCR_VSPOL             (1 << 30)  /* Bit 30: Vertical sync polarity */
#define LTDC_GCR_HSPOL             (1 << 31)  /* Bit 31: Horizontal sync polarity */

/* LTDC shadow reload configuration register (SRCR) */

#define LTDC_SRCR_IMR              (1 << 0)   /* Bit 0: Immediate reload */
#define LTDC_SRCR_VBR              (1 << 1)   /* Bit 1: Vertical blanking reload */

/* LTDC interrupt enable register (IER) */

#define LTDC_IER_LIE               (1 << 0)   /* Bit 0: Line interrupt enable */
#define LTDC_IER_FUIE              (1 << 1)   /* Bit 1: FIFO underrun interrupt enable */
#define LTDC_IER_TERRIE            (1 << 2)   /* Bit 2: Transfer error interrupt enable */
#define LTDC_IER_RRIE              (1 << 3)   /* Bit 3: Register reload interrupt enable */

/* LTDC interrupt status register (ISR) */

#define LTDC_ISR_LIF               (1 << 0)   /* Bit 0: Line interrupt flag */
#define LTDC_ISR_FUIF              (1 << 1)   /* Bit 1: FIFO underrun interrupt flag */
#define LTDC_ISR_TERRIF            (1 << 2)   /* Bit 2: Transfer error interrupt flag */
#define LTDC_ISR_RRIF              (1 << 3)   /* Bit 3: Register reload interrupt flag */

/* LTDC interrupt clear register (ICR) */

#define LTDC_ICR_CLIF              (1 << 0)   /* Bit 0: Clears the line interrupt flag */
#define LTDC_ICR_CFUIF             (1 << 1)   /* Bit 1: Clears the FIFO underrun interrupt flag */
#define LTDC_ICR_CTERRIF           (1 << 2)   /* Bit 2: Clears the transfer error interrupt flag */
#define LTDC_ICR_CRRIF             (1 << 3)   /* Bit 3: Clears the register reload interrupt flag */

/* LTDC line interrupt position configuration register (LIPCR) */

#define LTDC_LIPCR_LIPOS_SHIFT     (0)        /* Bits 0-10: Line interrupt position */
#define LTDC_LIPCR_LIPOS_MASK      (0x7ff << LTDC_LIPCR_LIPOS_SHIFT)
#define LTDC_LIPCR_LIPOS(n)        ((uint32_t)(n) << LTDC_LIPCR_LIPOS_SHIFT)

/* LTDC current position status register (CPSR) */

#define LTDC_CPSR_CYPOS_SHIFT      (0)        /* Bits 0-15: Current Y position */
#define LTDC_CPSR_CYPOS_MASK       (0xffff << LTDC_CPSR_CYPOS_SHIFT)
#define LTDC_CPSR_CXPOS_SHIFT      (16)       /* Bits 16-31: Current X position */
#define LTDC_CPSR_CXPOS_MASK       (0xffff << LTDC_CPSR_CXPOS_SHIFT)

/* LTDC current display status register (CDSR) */

#define LTDC_CDSR_VDES             (1 << 0)   /* Bit 0: Vertical data enable status */
#define LTDC_CDSR_HDES             (1 << 1)   /* Bit 1: Horizontal data enable status */
#define LTDC_CDSR_VSYNCS           (1 << 2)   /* Bit 2: Vertical sync status */
#define LTDC_CDSR_HSYNCS           (1 << 3)   /* Bit 3: Horizontal sync status */

/* LTDC synchronization size configuration register (SSCR) */

#define LTDC_SSCR_VSH_SHIFT        0
#define LTDC_SSCR_VSH_MASK         (0x7ff << LTDC_SSCR_VSH_SHIFT)
#define LTDC_SSCR_VSH(n)           ((n) << LTDC_SSCR_VSH_SHIFT)
#define LTDC_SSCR_HSW_SHIFT        16
#define LTDC_SSCR_HSW_MASK         (0xfff << LTDC_SSCR_HSW_SHIFT)
#define LTDC_SSCR_HSW(n)           ((n) << LTDC_SSCR_HSW_SHIFT)

/* LTDC back porch configuration register (BPCR) */

#define LTDC_BPCR_AVBP_SHIFT       0
#define LTDC_BPCR_AVBP_MASK        (0x7ff << LTDC_BPCR_AVBP_SHIFT)
#define LTDC_BPCR_AVBP(n)          ((n) << LTDC_BPCR_AVBP_SHIFT)
#define LTDC_BPCR_AHBP_SHIFT       16
#define LTDC_BPCR_AHBP_MASK        (0xfff << LTDC_BPCR_AHBP_SHIFT)
#define LTDC_BPCR_AHBP(n)          ((n) << LTDC_BPCR_AHBP_SHIFT)

/* LTDC active width configuration register (AWCR) */

#define LTDC_AWCR_AAH_SHIFT        0
#define LTDC_AWCR_AAH_MASK         (0x7ff << LTDC_AWCR_AAH_SHIFT)
#define LTDC_AWCR_AAH(n)           ((n) << LTDC_AWCR_AAH_SHIFT)
#define LTDC_AWCR_AAW_SHIFT        16
#define LTDC_AWCR_AAW_MASK         (0xfff << LTDC_AWCR_AAW_SHIFT)
#define LTDC_AWCR_AAW(n)           ((n) << LTDC_AWCR_AAW_SHIFT)

/* LTDC total width configuration register (TWCR) */

#define LTDC_TWCR_TOTALH_SHIFT     0
#define LTDC_TWCR_TOTALH_MASK      (0x7ff << LTDC_TWCR_TOTALH_SHIFT)
#define LTDC_TWCR_TOTALH(n)        ((n) << LTDC_TWCR_TOTALH_SHIFT)
#define LTDC_TWCR_TOTALW_SHIFT     16
#define LTDC_TWCR_TOTALW_MASK      (0xfff << LTDC_TWCR_TOTALW_SHIFT)
#define LTDC_TWCR_TOTALW(n)        ((n) << LTDC_TWCR_TOTALW_SHIFT)

/* LTDC layer window horizontal position configuration register (LxWHPCR) */

#define LTDC_LXWHPCR_WHSTPOS_SHIFT 0
#define LTDC_LXWHPCR_WHSTPOS_MASK  (0xfff << LTDC_LXWHPCR_WHSTPOS_SHIFT)
#define LTDC_LXWHPCR_WHSTPOS(n)    ((n) << LTDC_LXWHPCR_WHSTPOS_SHIFT)
#define LTDC_LXWHPCR_WHSPPOS_SHIFT 16
#define LTDC_LXWHPCR_WHSPPOS_MASK  (0xfff << LTDC_LXWHPCR_WHSPPOS_SHIFT)
#define LTDC_LXWHPCR_WHSPPOS(n)    ((n) << LTDC_LXWHPCR_WHSPPOS_SHIFT)

/* LTDC layer window vertical position configuration register (LxWVPCR) */

#define LTDC_LXWVPCR_WVSTPOS_SHIFT 0
#define LTDC_LXWVPCR_WVSTPOS_MASK  (0x7ff << LTDC_LXWVPCR_WVSTPOS_SHIFT)
#define LTDC_LXWVPCR_WVSTPOS(n)    ((n) << LTDC_LXWVPCR_WVSTPOS_SHIFT)
#define LTDC_LXWVPCR_WVSPPOS_SHIFT 16
#define LTDC_LXWVPCR_WVSPPOS_MASK  (0x7ff << LTDC_LXWVPCR_WVSPPOS_SHIFT)
#define LTDC_LXWVPCR_WVSPPOS(n)    ((n) << LTDC_LXWVPCR_WVSPPOS_SHIFT)

/* LTDC layer pixel format configuration register (LxPFCR) */

#define LTDC_LXPFCR_ARGB8888       0          /* ARGB8888 */
#define LTDC_LXPFCR_RGB888         1          /* RGB888 */
#define LTDC_LXPFCR_RGB565         2          /* RGB565 */
#define LTDC_LXPFCR_ARGB1555       3          /* ARGB1555 */
#define LTDC_LXPFCR_ARGB4444       4          /* ARGB4444 */
#define LTDC_LXPFCR_L8             5          /* L8 (8-bit luminance) */
#define LTDC_LXPFCR_AL44           6          /* AL44 (4-bit alpha, 4-bit luminance) */
#define LTDC_LXPFCR_AL88           7          /* AL88 (8-bit alpha, 8-bit luminance) */
#define LTDC_LXPFCR_L4             8          /* L4 (4-bit luminance) */
#define LTDC_LXPFCR_A8             9          /* A8 (8-bit alpha) */
#define LTDC_LXPFCR_A4             10         /* A4 (4-bit alpha) */

/* LTDC layer blending factors configuration register (LxBFCR) */

#define LTDC_LXBFCR_BF1_PIXEL_ALPHA_x_CA     (4 << 0)
#define LTDC_LXBFCR_BF1_CONST_ALPHA          (6 << 0)
#define LTDC_LXBFCR_BF2_PIXEL_ALPHA_x_CA     (5 << 4)
#define LTDC_LXBFCR_BF2_CONST_ALPHA          (7 << 4)

/* LTDC layer control register (LxCR) */

#define LTDC_LXCR_LEN              (1 << 0)   /* Bit 0: Layer enable */
#define LTDC_LXCR_COLKEN           (1 << 1)   /* Bit 1: Color keying enable */
#define LTDC_LXCR_CLUTEN           (1 << 4)   /* Bit 4: Color look-up table enable */

/* LTDC layer color frame buffer length register (LxCFBLR) */

#define LTDC_LXCFBLR_CFBLL_SHIFT   0
#define LTDC_LXCFBLR_CFBLL_MASK    (0x1fff << LTDC_LXCFBLR_CFBLL_SHIFT)
#define LTDC_LXCFBLR_CFBLL(n)      ((n) << LTDC_LXCFBLR_CFBLL_SHIFT)
#define LTDC_LXCFBLR_CFBP_SHIFT    16
#define LTDC_LXCFBLR_CFBP_MASK     (0x1fff << LTDC_LXCFBLR_CFBP_SHIFT)
#define LTDC_LXCFBLR_CFBP(n)       ((n) << LTDC_LXCFBLR_CFBP_SHIFT)

/* LTDC layer color frame buffer line number register (LxCFBLNR) */

#define LTDC_LXCFBLNR_CFBLNBR_SHIFT 0
#define LTDC_LXCFBLNR_CFBLNBR_MASK  (0x7ff << LTDC_LXCFBLNR_CFBLNBR_SHIFT)
#define LTDC_LXCFBLNR_CFBLNBR(n)    ((n) << LTDC_LXCFBLNR_CFBLNBR_SHIFT)

#endif /* __ARCH_ARM_SRC_STM32N6_HARDWARE_STM32_LTDC_H */
