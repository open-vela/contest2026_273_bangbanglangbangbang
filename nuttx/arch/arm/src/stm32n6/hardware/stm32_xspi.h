/****************************************************************************
 * arch/arm/src/stm32n6/hardware/stm32_xspi.h
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

#ifndef __ARCH_ARM_SRC_STM32N6_HARDWARE_STM32_XSPI_H
#define __ARCH_ARM_SRC_STM32N6_HARDWARE_STM32_XSPI_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include "chip.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* XSPI Register Offsets ****************************************************/

#define XSPI_CR_OFFSET          0x0000  /* Control Register */
#define XSPI_DCR1_OFFSET        0x0008  /* Device Configuration Register 1 */
#define XSPI_DCR2_OFFSET        0x000C  /* Device Configuration Register 2 */
#define XSPI_DCR3_OFFSET        0x0010  /* Device Configuration Register 3 */
#define XSPI_DCR4_OFFSET        0x0014  /* Device Configuration Register 4 */
#define XSPI_SR_OFFSET          0x0020  /* Status Register */
#define XSPI_FCR_OFFSET         0x0024  /* Flag Clear Register */
#define XSPI_DLR_OFFSET         0x0040  /* Data Length Register */
#define XSPI_AR_OFFSET          0x0048  /* Address Register */
#define XSPI_DR_OFFSET          0x0050  /* Data Register */
#define XSPI_PSMKR_OFFSET       0x0080  /* Polling Status Mask Register */
#define XSPI_PSMAR_OFFSET       0x0088  /* Polling Status Match Register */
#define XSPI_PIR_OFFSET         0x0090  /* Polling Interval Register */
#define XSPI_CCR_OFFSET         0x0100  /* Communication Configuration Register */
#define XSPI_TCR_OFFSET         0x0108  /* Timing Configuration Register */
#define XSPI_IR_OFFSET          0x0110  /* Instruction Register */
#define XSPI_ABR_OFFSET         0x0120  /* Alternate Bytes Register */
#define XSPI_LPTR_OFFSET        0x0130  /* Low-Power Timeout Register */
#define XSPI_WPCCR_OFFSET       0x0140  /* Wrap Communication Config Register */
#define XSPI_WPTCR_OFFSET       0x0148  /* Wrap Timing Configuration Register */
#define XSPI_WPIR_OFFSET        0x0150  /* Wrap Instruction Register */
#define XSPI_WPABR_OFFSET       0x0160  /* Wrap Alternate Bytes Register */
#define XSPI_WCCR_OFFSET        0x0180  /* Write Communication Config Register */
#define XSPI_WTCR_OFFSET        0x0188  /* Write Timing Configuration Register */
#define XSPI_WIR_OFFSET         0x0190  /* Write Instruction Register */
#define XSPI_WABR_OFFSET        0x01A0  /* Write Alternate Bytes Register */
#define XSPI_HLCR_OFFSET        0x0200  /* HyperBus Latency Config Register */
#define XSPI_CALFCR_OFFSET      0x0210  /* Full-Cycle Calibration Config Register */
#define XSPI_CALMR_OFFSET       0x0218  /* DLL Master Calibration Config Register */
#define XSPI_CALSOR_OFFSET      0x0220  /* Slave Output Calibration Config Register */
#define XSPI_CALSIR_OFFSET      0x0228  /* Slave Input Calibration Config Register */

/* XSPI IO Manager (XSPIM) Register Offsets *********************************/

#define XSPIM_CR_OFFSET         0x0000  /* XSPIM Control Register */

/* XSPI Register Addresses **************************************************/

/* XSPI1 Registers */

#ifdef CONFIG_STM32N6_XSPI1
#  define XSPI1_CR              (STM32N6_XSPI1_BASE + XSPI_CR_OFFSET)
#  define XSPI1_DCR1            (STM32N6_XSPI1_BASE + XSPI_DCR1_OFFSET)
#  define XSPI1_DCR2            (STM32N6_XSPI1_BASE + XSPI_DCR2_OFFSET)
#  define XSPI1_DCR3            (STM32N6_XSPI1_BASE + XSPI_DCR3_OFFSET)
#  define XSPI1_DCR4            (STM32N6_XSPI1_BASE + XSPI_DCR4_OFFSET)
#  define XSPI1_SR              (STM32N6_XSPI1_BASE + XSPI_SR_OFFSET)
#  define XSPI1_FCR             (STM32N6_XSPI1_BASE + XSPI_FCR_OFFSET)
#  define XSPI1_DLR             (STM32N6_XSPI1_BASE + XSPI_DLR_OFFSET)
#  define XSPI1_AR              (STM32N6_XSPI1_BASE + XSPI_AR_OFFSET)
#  define XSPI1_DR              (STM32N6_XSPI1_BASE + XSPI_DR_OFFSET)
#  define XSPI1_PSMKR           (STM32N6_XSPI1_BASE + XSPI_PSMKR_OFFSET)
#  define XSPI1_PSMAR           (STM32N6_XSPI1_BASE + XSPI_PSMAR_OFFSET)
#  define XSPI1_PIR             (STM32N6_XSPI1_BASE + XSPI_PIR_OFFSET)
#  define XSPI1_CCR             (STM32N6_XSPI1_BASE + XSPI_CCR_OFFSET)
#  define XSPI1_TCR             (STM32N6_XSPI1_BASE + XSPI_TCR_OFFSET)
#  define XSPI1_IR              (STM32N6_XSPI1_BASE + XSPI_IR_OFFSET)
#  define XSPI1_ABR             (STM32N6_XSPI1_BASE + XSPI_ABR_OFFSET)
#  define XSPI1_LPTR            (STM32N6_XSPI1_BASE + XSPI_LPTR_OFFSET)
#  define XSPI1_WPCCR           (STM32N6_XSPI1_BASE + XSPI_WPCCR_OFFSET)
#  define XSPI1_WPTCR           (STM32N6_XSPI1_BASE + XSPI_WPTCR_OFFSET)
#  define XSPI1_WPIR            (STM32N6_XSPI1_BASE + XSPI_WPIR_OFFSET)
#  define XSPI1_WPABR           (STM32N6_XSPI1_BASE + XSPI_WPABR_OFFSET)
#  define XSPI1_WCCR            (STM32N6_XSPI1_BASE + XSPI_WCCR_OFFSET)
#  define XSPI1_WTCR            (STM32N6_XSPI1_BASE + XSPI_WTCR_OFFSET)
#  define XSPI1_WIR             (STM32N6_XSPI1_BASE + XSPI_WIR_OFFSET)
#  define XSPI1_WABR            (STM32N6_XSPI1_BASE + XSPI_WABR_OFFSET)
#  define XSPI1_HLCR            (STM32N6_XSPI1_BASE + XSPI_HLCR_OFFSET)
#  define XSPI1_CALFCR          (STM32N6_XSPI1_BASE + XSPI_CALFCR_OFFSET)
#  define XSPI1_CALMR           (STM32N6_XSPI1_BASE + XSPI_CALMR_OFFSET)
#  define XSPI1_CALSOR          (STM32N6_XSPI1_BASE + XSPI_CALSOR_OFFSET)
#  define XSPI1_CALSIR          (STM32N6_XSPI1_BASE + XSPI_CALSIR_OFFSET)
#endif

/* XSPI2 Registers */

#ifdef CONFIG_STM32N6_XSPI2
#  define XSPI2_CR              (STM32N6_XSPI2_BASE + XSPI_CR_OFFSET)
#  define XSPI2_DCR1            (STM32N6_XSPI2_BASE + XSPI_DCR1_OFFSET)
#  define XSPI2_DCR2            (STM32N6_XSPI2_BASE + XSPI_DCR2_OFFSET)
#  define XSPI2_DCR3            (STM32N6_XSPI2_BASE + XSPI_DCR3_OFFSET)
#  define XSPI2_DCR4            (STM32N6_XSPI2_BASE + XSPI_DCR4_OFFSET)
#  define XSPI2_SR              (STM32N6_XSPI2_BASE + XSPI_SR_OFFSET)
#  define XSPI2_FCR             (STM32N6_XSPI2_BASE + XSPI_FCR_OFFSET)
#  define XSPI2_DLR             (STM32N6_XSPI2_BASE + XSPI_DLR_OFFSET)
#  define XSPI2_AR              (STM32N6_XSPI2_BASE + XSPI_AR_OFFSET)
#  define XSPI2_DR              (STM32N6_XSPI2_BASE + XSPI_DR_OFFSET)
#  define XSPI2_PSMKR           (STM32N6_XSPI2_BASE + XSPI_PSMKR_OFFSET)
#  define XSPI2_PSMAR           (STM32N6_XSPI2_BASE + XSPI_PSMAR_OFFSET)
#  define XSPI2_PIR             (STM32N6_XSPI2_BASE + XSPI_PIR_OFFSET)
#  define XSPI2_CCR             (STM32N6_XSPI2_BASE + XSPI_CCR_OFFSET)
#  define XSPI2_TCR             (STM32N6_XSPI2_BASE + XSPI_TCR_OFFSET)
#  define XSPI2_IR              (STM32N6_XSPI2_BASE + XSPI_IR_OFFSET)
#  define XSPI2_ABR             (STM32N6_XSPI2_BASE + XSPI_ABR_OFFSET)
#  define XSPI2_LPTR            (STM32N6_XSPI2_BASE + XSPI_LPTR_OFFSET)
#  define XSPI2_WPCCR           (STM32N6_XSPI2_BASE + XSPI_WPCCR_OFFSET)
#  define XSPI2_WPTCR           (STM32N6_XSPI2_BASE + XSPI_WPTCR_OFFSET)
#  define XSPI2_WPIR            (STM32N6_XSPI2_BASE + XSPI_WPIR_OFFSET)
#  define XSPI2_WPABR           (STM32N6_XSPI2_BASE + XSPI_WPABR_OFFSET)
#  define XSPI2_WCCR            (STM32N6_XSPI2_BASE + XSPI_WCCR_OFFSET)
#  define XSPI2_WTCR            (STM32N6_XSPI2_BASE + XSPI_WTCR_OFFSET)
#  define XSPI2_WIR             (STM32N6_XSPI2_BASE + XSPI_WIR_OFFSET)
#  define XSPI2_WABR            (STM32N6_XSPI2_BASE + XSPI_WABR_OFFSET)
#  define XSPI2_HLCR            (STM32N6_XSPI2_BASE + XSPI_HLCR_OFFSET)
#  define XSPI2_CALFCR          (STM32N6_XSPI2_BASE + XSPI_CALFCR_OFFSET)
#  define XSPI2_CALMR           (STM32N6_XSPI2_BASE + XSPI_CALMR_OFFSET)
#  define XSPI2_CALSOR          (STM32N6_XSPI2_BASE + XSPI_CALSOR_OFFSET)
#  define XSPI2_CALSIR          (STM32N6_XSPI2_BASE + XSPI_CALSIR_OFFSET)
#endif

/* XSPI3 Registers */

#ifdef CONFIG_STM32N6_XSPI3
#  define XSPI3_CR              (STM32N6_XSPI3_BASE + XSPI_CR_OFFSET)
#  define XSPI3_DCR1            (STM32N6_XSPI3_BASE + XSPI_DCR1_OFFSET)
#  define XSPI3_DCR2            (STM32N6_XSPI3_BASE + XSPI_DCR2_OFFSET)
#  define XSPI3_DCR3            (STM32N6_XSPI3_BASE + XSPI_DCR3_OFFSET)
#  define XSPI3_DCR4            (STM32N6_XSPI3_BASE + XSPI_DCR4_OFFSET)
#  define XSPI3_SR              (STM32N6_XSPI3_BASE + XSPI_SR_OFFSET)
#  define XSPI3_FCR             (STM32N6_XSPI3_BASE + XSPI_FCR_OFFSET)
#  define XSPI3_DLR             (STM32N6_XSPI3_BASE + XSPI_DLR_OFFSET)
#  define XSPI3_AR              (STM32N6_XSPI3_BASE + XSPI_AR_OFFSET)
#  define XSPI3_DR              (STM32N6_XSPI3_BASE + XSPI_DR_OFFSET)
#  define XSPI3_PSMKR           (STM32N6_XSPI3_BASE + XSPI_PSMKR_OFFSET)
#  define XSPI3_PSMAR           (STM32N6_XSPI3_BASE + XSPI_PSMAR_OFFSET)
#  define XSPI3_PIR             (STM32N6_XSPI3_BASE + XSPI_PIR_OFFSET)
#  define XSPI3_CCR             (STM32N6_XSPI3_BASE + XSPI_CCR_OFFSET)
#  define XSPI3_TCR             (STM32N6_XSPI3_BASE + XSPI_TCR_OFFSET)
#  define XSPI3_IR              (STM32N6_XSPI3_BASE + XSPI_IR_OFFSET)
#  define XSPI3_ABR             (STM32N6_XSPI3_BASE + XSPI_ABR_OFFSET)
#  define XSPI3_LPTR            (STM32N6_XSPI3_BASE + XSPI_LPTR_OFFSET)
#  define XSPI3_WPCCR           (STM32N6_XSPI3_BASE + XSPI_WPCCR_OFFSET)
#  define XSPI3_WPTCR           (STM32N6_XSPI3_BASE + XSPI_WPTCR_OFFSET)
#  define XSPI3_WPIR            (STM32N6_XSPI3_BASE + XSPI_WPIR_OFFSET)
#  define XSPI3_WPABR           (STM32N6_XSPI3_BASE + XSPI_WPABR_OFFSET)
#  define XSPI3_WCCR            (STM32N6_XSPI3_BASE + XSPI_WCCR_OFFSET)
#  define XSPI3_WTCR            (STM32N6_XSPI3_BASE + XSPI_WTCR_OFFSET)
#  define XSPI3_WIR             (STM32N6_XSPI3_BASE + XSPI_WIR_OFFSET)
#  define XSPI3_WABR            (STM32N6_XSPI3_BASE + XSPI_WABR_OFFSET)
#  define XSPI3_HLCR            (STM32N6_XSPI3_BASE + XSPI_HLCR_OFFSET)
#  define XSPI3_CALFCR          (STM32N6_XSPI3_BASE + XSPI_CALFCR_OFFSET)
#  define XSPI3_CALMR           (STM32N6_XSPI3_BASE + XSPI_CALMR_OFFSET)
#  define XSPI3_CALSOR          (STM32N6_XSPI3_BASE + XSPI_CALSOR_OFFSET)
#  define XSPI3_CALSIR          (STM32N6_XSPI3_BASE + XSPI_CALSIR_OFFSET)
#endif

/* XSPI IO Manager Register Addresses ***************************************/

#define XSPIM_CR                (STM32N6_XSPIM_BASE + XSPIM_CR_OFFSET)

/* Register Bitfield Definitions ********************************************/

/* XSPI Control Register (CR) */

#define XSPI_CR_EN              (1 << 0)   /* Bit 0: Enable */
#define XSPI_CR_ABORT           (1 << 1)   /* Bit 1: Abort request */
#define XSPI_CR_DMAEN           (1 << 2)   /* Bit 2: DMA enable */
#define XSPI_CR_TCEN            (1 << 3)   /* Bit 3: Timeout counter enable */
#define XSPI_CR_DMM             (1 << 6)   /* Bit 6: Dual-memory mode */
#define XSPI_CR_FSEL            (1 << 7)   /* Bit 7: Flash memory selection */
#define XSPI_CR_FTHRES_SHIFT    (8)        /* Bits 13:8: FIFO threshold level */
#define XSPI_CR_FTHRES_MASK     (0x3f << XSPI_CR_FTHRES_SHIFT)
#  define XSPI_CR_FTHRES(n)    ((uint32_t)(n) << XSPI_CR_FTHRES_SHIFT)
#define XSPI_CR_TEIE            (1 << 16)  /* Bit 16: Transfer error interrupt enable */
#define XSPI_CR_TCIE            (1 << 17)  /* Bit 17: Transfer complete interrupt enable */
#define XSPI_CR_FTIE            (1 << 18)  /* Bit 18: FIFO threshold interrupt enable */
#define XSPI_CR_SMIE            (1 << 19)  /* Bit 19: Status match interrupt enable */
#define XSPI_CR_TOIE            (1 << 20)  /* Bit 20: Timeout interrupt enable */
#define XSPI_CR_APMS            (1 << 22)  /* Bit 22: Automatic poll mode stop */
#define XSPI_CR_PMM             (1 << 23)  /* Bit 23: Polling match mode */
#define XSPI_CR_CSSEL           (1 << 24)  /* Bit 24: Chip select selection */
#define XSPI_CR_NOPREF          (1 << 25)  /* Bit 25: No prefetch */
#define XSPI_CR_NOPREF_AXI      (1 << 26)  /* Bit 26: No prefetch on AXI */
#define XSPI_CR_FMODE_SHIFT     (28)       /* Bits 29:28: Functional mode */
#define XSPI_CR_FMODE_MASK      (3 << XSPI_CR_FMODE_SHIFT)
#  define XSPI_CR_FMODE_INDWR   (0 << XSPI_CR_FMODE_SHIFT) /* Indirect write */
#  define XSPI_CR_FMODE_INDRD   (1 << XSPI_CR_FMODE_SHIFT) /* Indirect read */
#  define XSPI_CR_FMODE_APOLL   (2 << XSPI_CR_FMODE_SHIFT) /* Auto-polling */
#  define XSPI_CR_FMODE_MMAP    (3 << XSPI_CR_FMODE_SHIFT) /* Memory-mapped */
#define XSPI_CR_MSEL_SHIFT      (30)       /* Bits 31:30: Memory select (multi-chip) */
#define XSPI_CR_MSEL_MASK       (3 << XSPI_CR_MSEL_SHIFT)

/* XSPI Device Configuration Register 1 (DCR1) */

#define XSPI_DCR1_CKMODE        (1 << 0)   /* Bit 0: Mode 0 / Mode 3 clock */
#define XSPI_DCR1_FRCK          (1 << 1)   /* Bit 1: Free running clock */
#define XSPI_DCR1_CSHT_SHIFT    (8)        /* Bits 13:8: Chip-select high time */
#define XSPI_DCR1_CSHT_MASK     (0x3f << XSPI_DCR1_CSHT_SHIFT)
#  define XSPI_DCR1_CSHT(n)    ((uint32_t)(n) << XSPI_DCR1_CSHT_SHIFT)
#define XSPI_DCR1_DEVSIZE_SHIFT (16)       /* Bits 20:16: Device size (2^n bytes) */
#define XSPI_DCR1_DEVSIZE_MASK  (0x1f << XSPI_DCR1_DEVSIZE_SHIFT)
#  define XSPI_DCR1_DEVSIZE(n) ((uint32_t)(n) << XSPI_DCR1_DEVSIZE_SHIFT)
#define XSPI_DCR1_EXTENDMEM     (1 << 21)  /* Bit 21: Extended memory */
#define XSPI_DCR1_MTYP_SHIFT    (24)       /* Bits 26:24: Memory type */
#define XSPI_DCR1_MTYP_MASK     (7 << XSPI_DCR1_MTYP_SHIFT)
#  define XSPI_DCR1_MTYP_MICRON   (0 << XSPI_DCR1_MTYP_SHIFT)
#  define XSPI_DCR1_MTYP_MACRONIX (1 << XSPI_DCR1_MTYP_SHIFT)
#  define XSPI_DCR1_MTYP_STD      (2 << XSPI_DCR1_MTYP_SHIFT)
#  define XSPI_DCR1_MTYP_HYPERBUS (3 << XSPI_DCR1_MTYP_SHIFT)
#  define XSPI_DCR1_MTYP_MXRAM    (4 << XSPI_DCR1_MTYP_SHIFT)

/* XSPI Device Configuration Register 2 (DCR2) */

#define XSPI_DCR2_WRAPSIZE_SHIFT  (4)      /* Bits 6:4: Wrap size */
#define XSPI_DCR2_WRAPSIZE_MASK   (7 << XSPI_DCR2_WRAPSIZE_SHIFT)
#  define XSPI_DCR2_WRAPSIZE(n)  ((uint32_t)(n) << XSPI_DCR2_WRAPSIZE_SHIFT)
#define XSPI_DCR2_PRESCALER_SHIFT (24)     /* Bits 31:24: Clock prescaler */
#define XSPI_DCR2_PRESCALER_MASK  (0xff << XSPI_DCR2_PRESCALER_SHIFT)
#  define XSPI_DCR2_PRESCALER(n) ((uint32_t)(n) << XSPI_DCR2_PRESCALER_SHIFT)

/* XSPI Device Configuration Register 3 (DCR3) */

#define XSPI_DCR3_CSHTN_SHIFT    (0)       /* Bits 5:0: CS high time next memory */
#define XSPI_DCR3_CSHTN_MASK     (0x3f << XSPI_DCR3_CSHTN_SHIFT)
#  define XSPI_DCR3_CSHTN(n)    ((uint32_t)(n) << XSPI_DCR3_CSHTN_SHIFT)
#define XSPI_DCR3_MAXTRAN_SHIFT  (16)      /* Bits 23:16: Max transfer size */
#define XSPI_DCR3_MAXTRAN_MASK   (0xff << XSPI_DCR3_MAXTRAN_SHIFT)
#  define XSPI_DCR3_MAXTRAN(n)  ((uint32_t)(n) << XSPI_DCR3_MAXTRAN_SHIFT)

/* XSPI Status Register (SR) */

#define XSPI_SR_TEF             (1 << 0)   /* Bit 0: Transfer error flag */
#define XSPI_SR_TCF             (1 << 1)   /* Bit 1: Transfer complete flag */
#define XSPI_SR_FTF             (1 << 2)   /* Bit 2: FIFO threshold flag */
#define XSPI_SR_SMF             (1 << 3)   /* Bit 3: Status match flag */
#define XSPI_SR_TOF             (1 << 4)   /* Bit 4: Timeout flag */
#define XSPI_SR_BUSY            (1 << 5)   /* Bit 5: Busy flag */
#define XSPI_SR_FLEVEL_SHIFT    (8)        /* Bits 13:8: FIFO level */
#define XSPI_SR_FLEVEL_MASK     (0x3f << XSPI_SR_FLEVEL_SHIFT)

/* XSPI Flag Clear Register (FCR) */

#define XSPI_FCR_CTEF           (1 << 0)   /* Bit 0: Clear transfer error flag */
#define XSPI_FCR_CTCF           (1 << 1)   /* Bit 1: Clear transfer complete flag */
#define XSPI_FCR_CSMF           (1 << 3)   /* Bit 3: Clear status match flag */
#define XSPI_FCR_CTOF           (1 << 4)   /* Bit 4: Clear timeout flag */

/* XSPI Communication Configuration Register (CCR) */

#define XSPI_CCR_IMODE_SHIFT    (0)        /* Bits 2:0: Instruction mode */
#define XSPI_CCR_IMODE_MASK     (7 << XSPI_CCR_IMODE_SHIFT)
#  define XSPI_CCR_IMODE_NONE   (0 << XSPI_CCR_IMODE_SHIFT)
#  define XSPI_CCR_IMODE_1LINE  (1 << XSPI_CCR_IMODE_SHIFT)
#  define XSPI_CCR_IMODE_2LINE  (2 << XSPI_CCR_IMODE_SHIFT)
#  define XSPI_CCR_IMODE_4LINE  (3 << XSPI_CCR_IMODE_SHIFT)
#  define XSPI_CCR_IMODE_8LINE  (4 << XSPI_CCR_IMODE_SHIFT)
#define XSPI_CCR_IDTR           (1 << 3)   /* Bit 3: Instruction DTR mode */
#define XSPI_CCR_ISIZE_SHIFT    (4)        /* Bits 5:4: Instruction size */
#define XSPI_CCR_ISIZE_MASK     (3 << XSPI_CCR_ISIZE_SHIFT)
#  define XSPI_CCR_ISIZE_8BIT   (0 << XSPI_CCR_ISIZE_SHIFT)
#  define XSPI_CCR_ISIZE_16BIT  (1 << XSPI_CCR_ISIZE_SHIFT)
#  define XSPI_CCR_ISIZE_24BIT  (2 << XSPI_CCR_ISIZE_SHIFT)
#  define XSPI_CCR_ISIZE_32BIT  (3 << XSPI_CCR_ISIZE_SHIFT)
#define XSPI_CCR_ADMODE_SHIFT   (6)        /* Bits 8:6: Address mode */
#define XSPI_CCR_ADMODE_MASK    (7 << XSPI_CCR_ADMODE_SHIFT)
#  define XSPI_CCR_ADMODE_NONE  (0 << XSPI_CCR_ADMODE_SHIFT)
#  define XSPI_CCR_ADMODE_1LINE (1 << XSPI_CCR_ADMODE_SHIFT)
#  define XSPI_CCR_ADMODE_2LINE (2 << XSPI_CCR_ADMODE_SHIFT)
#  define XSPI_CCR_ADMODE_4LINE (3 << XSPI_CCR_ADMODE_SHIFT)
#  define XSPI_CCR_ADMODE_8LINE (4 << XSPI_CCR_ADMODE_SHIFT)
#define XSPI_CCR_ADDTR          (1 << 9)   /* Bit 9: Address DTR mode */
#define XSPI_CCR_ADSIZE_SHIFT   (10)       /* Bits 11:10: Address size */
#define XSPI_CCR_ADSIZE_MASK    (3 << XSPI_CCR_ADSIZE_SHIFT)
#  define XSPI_CCR_ADSIZE_8BIT  (0 << XSPI_CCR_ADSIZE_SHIFT)
#  define XSPI_CCR_ADSIZE_16BIT (1 << XSPI_CCR_ADSIZE_SHIFT)
#  define XSPI_CCR_ADSIZE_24BIT (2 << XSPI_CCR_ADSIZE_SHIFT)
#  define XSPI_CCR_ADSIZE_32BIT (3 << XSPI_CCR_ADSIZE_SHIFT)
#define XSPI_CCR_ABMODE_SHIFT   (12)       /* Bits 14:12: Alternate bytes mode */
#define XSPI_CCR_ABMODE_MASK    (7 << XSPI_CCR_ABMODE_SHIFT)
#  define XSPI_CCR_ABMODE_NONE  (0 << XSPI_CCR_ABMODE_SHIFT)
#  define XSPI_CCR_ABMODE_1LINE (1 << XSPI_CCR_ABMODE_SHIFT)
#  define XSPI_CCR_ABMODE_2LINE (2 << XSPI_CCR_ABMODE_SHIFT)
#  define XSPI_CCR_ABMODE_4LINE (3 << XSPI_CCR_ABMODE_SHIFT)
#  define XSPI_CCR_ABMODE_8LINE (4 << XSPI_CCR_ABMODE_SHIFT)
#define XSPI_CCR_ABDTR          (1 << 15)  /* Bit 15: Alternate bytes DTR mode */
#define XSPI_CCR_ABSIZE_SHIFT   (16)       /* Bits 17:16: Alternate bytes size */
#define XSPI_CCR_ABSIZE_MASK    (3 << XSPI_CCR_ABSIZE_SHIFT)
#  define XSPI_CCR_ABSIZE_8BIT  (0 << XSPI_CCR_ABSIZE_SHIFT)
#  define XSPI_CCR_ABSIZE_16BIT (1 << XSPI_CCR_ABSIZE_SHIFT)
#  define XSPI_CCR_ABSIZE_24BIT (2 << XSPI_CCR_ABSIZE_SHIFT)
#  define XSPI_CCR_ABSIZE_32BIT (3 << XSPI_CCR_ABSIZE_SHIFT)
#define XSPI_CCR_DMODE_SHIFT    (18)       /* Bits 20:18: Data mode */
#define XSPI_CCR_DMODE_MASK     (7 << XSPI_CCR_DMODE_SHIFT)
#  define XSPI_CCR_DMODE_NONE   (0 << XSPI_CCR_DMODE_SHIFT)
#  define XSPI_CCR_DMODE_1LINE  (1 << XSPI_CCR_DMODE_SHIFT)
#  define XSPI_CCR_DMODE_2LINE  (2 << XSPI_CCR_DMODE_SHIFT)
#  define XSPI_CCR_DMODE_4LINE  (3 << XSPI_CCR_DMODE_SHIFT)
#  define XSPI_CCR_DMODE_8LINE  (4 << XSPI_CCR_DMODE_SHIFT)
#define XSPI_CCR_DDTR           (1 << 21)  /* Bit 21: Data DTR mode */
#define XSPI_CCR_DQSE           (1 << 22)  /* Bit 22: DQS enable */
#define XSPI_CCR_FMODE_SHIFT    (24)       /* Bits 25:24: Functional mode */
#define XSPI_CCR_FMODE_MASK     (3 << XSPI_CCR_FMODE_SHIFT)
#  define XSPI_CCR_FMODE_INDWR  (0 << XSPI_CCR_FMODE_SHIFT) /* Indirect write */
#  define XSPI_CCR_FMODE_INDRD  (1 << XSPI_CCR_FMODE_SHIFT) /* Indirect read */
#  define XSPI_CCR_FMODE_APOLL  (2 << XSPI_CCR_FMODE_SHIFT) /* Auto-polling */
#  define XSPI_CCR_FMODE_MMAP   (3 << XSPI_CCR_FMODE_SHIFT) /* Memory-mapped */
#define XSPI_CCR_SIOO           (1 << 28)  /* Bit 28: Send instruction only once */

/* XSPI Timing Configuration Register (TCR) */

#define XSPI_TCR_DCYC_SHIFT     (0)        /* Bits 4:0: Number of dummy cycles */
#define XSPI_TCR_DCYC_MASK      (0x1f << XSPI_TCR_DCYC_SHIFT)
#  define XSPI_TCR_DCYC(n)     ((uint32_t)(n) << XSPI_TCR_DCYC_SHIFT)
#define XSPI_TCR_DHQC           (1 << 28)  /* Bit 28: Delay hold quarter cycle */
#define XSPI_TCR_SSHIFT         (1 << 30)  /* Bit 30: Sample shift */

/* XSPI Polling Status Mask Register (PSMKR) */

#define XSPI_PSMKR_MASK_SHIFT   (0)        /* Bits 31:0: Status mask */
#define XSPI_PSMKR_MASK_MASK    (0xffffffff)

/* XSPI Polling Status Match Register (PSMAR) */

#define XSPI_PSMAR_MATCH_SHIFT  (0)        /* Bits 31:0: Status match */
#define XSPI_PSMAR_MATCH_MASK   (0xffffffff)

/* XSPI Polling Interval Register (PIR) */

#define XSPI_PIR_INTERVAL_SHIFT (0)        /* Bits 15:0: Polling interval */
#define XSPI_PIR_INTERVAL_MASK  (0xffff << XSPI_PIR_INTERVAL_SHIFT)
#  define XSPI_PIR_INTERVAL(n) ((uint32_t)(n) << XSPI_PIR_INTERVAL_SHIFT)

/* XSPI HyperBus Latency Configuration Register (HLCR) */

#define XSPI_HLCR_LM_SHIFT      (0)        /* Bits 7:0: Latency mode */
#define XSPI_HLCR_LM_MASK       (0xff << XSPI_HLCR_LM_SHIFT)
#define XSPI_HLCR_WZL           (1 << 8)   /* Bit 8: Write zero latency */
#define XSPI_HLCR_TACC_SHIFT    (16)       /* Bits 23:16: Access time */
#define XSPI_HLCR_TACC_MASK     (0xff << XSPI_HLCR_TACC_SHIFT)
#  define XSPI_HLCR_TACC(n)    ((uint32_t)(n) << XSPI_HLCR_TACC_SHIFT)
#define XSPI_HLCR_TRWR_SHIFT    (24)       /* Bits 31:24: Read-write recovery */
#define XSPI_HLCR_TRWR_MASK     (0xff << XSPI_HLCR_TRWR_SHIFT)
#  define XSPI_HLCR_TRWR(n)    ((uint32_t)(n) << XSPI_HLCR_TRWR_SHIFT)

/* XSPI Full-Cycle Calibration Configuration Register (CALFCR) */

#define XSPI_CALFCR_CALMAX      (1 << 0)   /* Bit 0: Calibration max reached */

/* XSPIM Control Register (XSPIM_CR) */

#define XSPIM_CR_MUXEN          (1 << 0)   /* Bit 0: Multiplexer enable */
#define XSPIM_CR_CSSEL_OVR_SHIFT (4)       /* Bits 6:4: CS override */
#define XSPIM_CR_CSSEL_OVR_MASK (7 << XSPIM_CR_CSSEL_OVR_SHIFT)
#  define XSPIM_CR_CSSEL_OVR_DIS  (0 << XSPIM_CR_CSSEL_OVR_SHIFT)
#  define XSPIM_CR_CSSEL_OVR_NCS1 (1 << XSPIM_CR_CSSEL_OVR_SHIFT)
#  define XSPIM_CR_CSSEL_OVR_NCS2 (7 << XSPIM_CR_CSSEL_OVR_SHIFT)
#define XSPIM_CR_REQ2ACK_TIME_SHIFT (16)   /* Bits 23:16: Req-to-ack time */
#define XSPIM_CR_REQ2ACK_TIME_MASK (0xff << XSPIM_CR_REQ2ACK_TIME_SHIFT)
#  define XSPIM_CR_REQ2ACK_TIME(n) ((uint32_t)(n) << XSPIM_CR_REQ2ACK_TIME_SHIFT)

/* Functional Mode convenience aliases ****************************************/

#define XSPI_FMODE_INDWRITE     XSPI_CR_FMODE_INDWR
#define XSPI_FMODE_INDREAD      XSPI_CR_FMODE_INDRD
#define XSPI_FMODE_AUTOPOLL     XSPI_CR_FMODE_APOLL
#define XSPI_FMODE_MEMMAP       XSPI_CR_FMODE_MMAP

/* Memory Type convenience aliases *******************************************/

#define XSPI_MTYP_MICRON        XSPI_DCR1_MTYP_MICRON
#define XSPI_MTYP_MACRONIX      XSPI_DCR1_MTYP_MACRONIX
#define XSPI_MTYP_STANDARD      XSPI_DCR1_MTYP_STD
#define XSPI_MTYP_HYPERBUS      XSPI_DCR1_MTYP_HYPERBUS
#define XSPI_MTYP_MACRONIXRAM   XSPI_DCR1_MTYP_MXRAM

/* RCC Clock Enable/Reset convenience aliases ********************************/

#define XSPI_RCC_EN_XSPI1      RCC_AHB5ENR_XSPI1EN
#define XSPI_RCC_EN_XSPI2      RCC_AHB5ENR_XSPI2EN
#define XSPI_RCC_EN_XSPI3      RCC_AHB5ENR_XSPI3EN
#define XSPI_RCC_EN_XSPIM      RCC_AHB5ENR_XSPIMEN
#define XSPI_RCC_RST_XSPI1     RCC_AHB5RSTR_XSPI1RST
#define XSPI_RCC_RST_XSPI2     RCC_AHB5RSTR_XSPI2RST
#define XSPI_RCC_RST_XSPI3     RCC_AHB5RSTR_XSPI3RST

#endif /* __ARCH_ARM_SRC_STM32N6_HARDWARE_STM32_XSPI_H */
