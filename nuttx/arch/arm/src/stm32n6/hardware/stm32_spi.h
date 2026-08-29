/****************************************************************************
 * arch/arm/src/stm32n6/hardware/stm32_spi.h
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

#ifndef __ARCH_ARM_SRC_STM32N6_HARDWARE_STM32_SPI_H
#define __ARCH_ARM_SRC_STM32N6_HARDWARE_STM32_SPI_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include "chip.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* STM32N6 SPI supports 4-16 bit data frames and FIFOs */

#define HAVE_SPI_I2S            /* SPI peripherals have I2S mode */
#define HAVE_SPI_TI_MODE        /* Have Motorola and TI frame modes */
#define HAVE_SPI_ARB_DATA_SIZE  /* Supports arbitrary data size from 4-16 bits */
#define HAVE_SPI_FIFOS          /* Have Tx/Rx FIFOs */
#define HAVE_SPI_NSSP           /* Have NSS Pulse Management in master mode */

/* Maximum allowed speed as per specifications */

#define STM32_SPI_CLK_MAX       50000000UL  /* 50 MHz max SPI clock */

/* Number of SPI peripherals */

#if defined(CONFIG_STM32N6_SPI1) || defined(CONFIG_STM32N6_SPI2) || \
    defined(CONFIG_STM32N6_SPI3)
#  if !defined(STM32N6_NSPI)
#    define STM32N6_NSPI 3
#  endif
#endif

/* SPI Base Addresses ********************************************************/

#define STM32_SPI1_BASE         0x40013000  /* SPI1 base address */
#define STM32_SPI2_BASE         0x40003800  /* SPI2 base address */
#define STM32_SPI3_BASE         0x40003C00  /* SPI3 base address */

/* Register Offsets **********************************************************/

#define STM32_SPI_CR1_OFFSET    0x0000  /* SPI Control Register 1 */
#define STM32_SPI_CR2_OFFSET    0x0004  /* SPI Control Register 2 */
#define STM32_SPI_CFG1_OFFSET   0x0008  /* SPI Configuration Register 1 */
#define STM32_SPI_CFG2_OFFSET   0x000C  /* SPI Configuration Register 2 */
#define STM32_SPI_IER_OFFSET    0x0010  /* SPI Interrupt Enable Register */
#define STM32_SPI_SR_OFFSET     0x0014  /* SPI Status Register */
#define STM32_SPI_IFCR_OFFSET   0x0018  /* SPI Interrupt Flag Clear Register */
#define STM32_SPI_TXDR_OFFSET   0x0020  /* SPI Transmit Data Register */
#define STM32_SPI_RXDR_OFFSET   0x0030  /* SPI Receive Data Register */
#define STM32_SPI_CRCPOLY_OFFSET 0x0040 /* SPI CRC Polynomial Register */
#define STM32_SPI_TXCRC_OFFSET  0x0044  /* SPI TX CRC Register */
#define STM32_SPI_RXCRC_OFFSET  0x0048  /* SPI RX CRC Register */
#define STM32_SPI_I2SCFGR_OFFSET 0x0050 /* SPI I2S Configuration Register */

/* Register Addresses ********************************************************/

/* SPI1 Registers */

#ifdef CONFIG_STM32N6_SPI1
#  define STM32_SPI1_CR1        (STM32_SPI1_BASE + STM32_SPI_CR1_OFFSET)
#  define STM32_SPI1_CR2        (STM32_SPI1_BASE + STM32_SPI_CR2_OFFSET)
#  define STM32_SPI1_CFG1       (STM32_SPI1_BASE + STM32_SPI_CFG1_OFFSET)
#  define STM32_SPI1_CFG2       (STM32_SPI1_BASE + STM32_SPI_CFG2_OFFSET)
#  define STM32_SPI1_IER        (STM32_SPI1_BASE + STM32_SPI_IER_OFFSET)
#  define STM32_SPI1_SR         (STM32_SPI1_BASE + STM32_SPI_SR_OFFSET)
#  define STM32_SPI1_IFCR       (STM32_SPI1_BASE + STM32_SPI_IFCR_OFFSET)
#  define STM32_SPI1_TXDR       (STM32_SPI1_BASE + STM32_SPI_TXDR_OFFSET)
#  define STM32_SPI1_RXDR       (STM32_SPI1_BASE + STM32_SPI_RXDR_OFFSET)
#  define STM32_SPI1_CRCPOLY    (STM32_SPI1_BASE + STM32_SPI_CRCPOLY_OFFSET)
#  define STM32_SPI1_TXCRC      (STM32_SPI1_BASE + STM32_SPI_TXCRC_OFFSET)
#  define STM32_SPI1_RXCRC      (STM32_SPI1_BASE + STM32_SPI_RXCRC_OFFSET)
#endif

/* SPI2 Registers */

#ifdef CONFIG_STM32N6_SPI2
#  define STM32_SPI2_CR1        (STM32_SPI2_BASE + STM32_SPI_CR1_OFFSET)
#  define STM32_SPI2_CR2        (STM32_SPI2_BASE + STM32_SPI_CR2_OFFSET)
#  define STM32_SPI2_CFG1       (STM32_SPI2_BASE + STM32_SPI_CFG1_OFFSET)
#  define STM32_SPI2_CFG2       (STM32_SPI2_BASE + STM32_SPI_CFG2_OFFSET)
#  define STM32_SPI2_IER        (STM32_SPI2_BASE + STM32_SPI_IER_OFFSET)
#  define STM32_SPI2_SR         (STM32_SPI2_BASE + STM32_SPI_SR_OFFSET)
#  define STM32_SPI2_IFCR       (STM32_SPI2_BASE + STM32_SPI_IFCR_OFFSET)
#  define STM32_SPI2_TXDR       (STM32_SPI2_BASE + STM32_SPI_TXDR_OFFSET)
#  define STM32_SPI2_RXDR       (STM32_SPI2_BASE + STM32_SPI_RXDR_OFFSET)
#  define STM32_SPI2_CRCPOLY    (STM32_SPI2_BASE + STM32_SPI_CRCPOLY_OFFSET)
#  define STM32_SPI2_TXCRC      (STM32_SPI2_BASE + STM32_SPI_TXCRC_OFFSET)
#  define STM32_SPI2_RXCRC      (STM32_SPI2_BASE + STM32_SPI_RXCRC_OFFSET)
#endif

/* SPI3 Registers */

#ifdef CONFIG_STM32N6_SPI3
#  define STM32_SPI3_CR1        (STM32_SPI3_BASE + STM32_SPI_CR1_OFFSET)
#  define STM32_SPI3_CR2        (STM32_SPI3_BASE + STM32_SPI_CR2_OFFSET)
#  define STM32_SPI3_CFG1       (STM32_SPI3_BASE + STM32_SPI_CFG1_OFFSET)
#  define STM32_SPI3_CFG2       (STM32_SPI3_BASE + STM32_SPI_CFG2_OFFSET)
#  define STM32_SPI3_IER        (STM32_SPI3_BASE + STM32_SPI_IER_OFFSET)
#  define STM32_SPI3_SR         (STM32_SPI3_BASE + STM32_SPI_SR_OFFSET)
#  define STM32_SPI3_IFCR       (STM32_SPI3_BASE + STM32_SPI_IFCR_OFFSET)
#  define STM32_SPI3_TXDR       (STM32_SPI3_BASE + STM32_SPI_TXDR_OFFSET)
#  define STM32_SPI3_RXDR       (STM32_SPI3_BASE + STM32_SPI_RXDR_OFFSET)
#  define STM32_SPI3_CRCPOLY    (STM32_SPI3_BASE + STM32_SPI_CRCPOLY_OFFSET)
#  define STM32_SPI3_TXCRC      (STM32_SPI3_BASE + STM32_SPI_TXCRC_OFFSET)
#  define STM32_SPI3_RXCRC      (STM32_SPI3_BASE + STM32_SPI_RXCRC_OFFSET)
#endif

/* Register Bitfield Definitions ********************************************/

/* SPI Control Register 1 (CR1) */

#define SPI_CR1_SPE             (1 << 0)   /* Bit 0: Serial Peripheral Enable */
#define SPI_CR1_MASRX           (1 << 8)   /* Bit 8: Master Automatic SUSP in Receive mode */
#define SPI_CR1_CSTART          (1 << 9)   /* Bit 9: Master transfer start */
#define SPI_CR1_CSUSP           (1 << 10)  /* Bit 10: Master SUSPend request */
#define SPI_CR1_HDDIR           (1 << 11)  /* Bit 11: Duplex mode data direction */
#define SPI_CR1_SSI             (1 << 12)  /* Bit 12: Internal SS signal input level */
#define SPI_CR1_CRC33_17        (1 << 13)  /* Bit 13: 32-bit CRC polynomial configuration */
#define SPI_CR1_RCRCINI         (1 << 14)  /* Bit 14: CRC initialization pattern for RX */
#define SPI_CR1_TCRCINI         (1 << 15)  /* Bit 15: CRC initialization pattern for TX */
#define SPI_CR1_IOLOCK          (1 << 16)  /* Bit 16: Locking the AF configuration */

/* SPI Control Register 2 (CR2) */

#define SPI_CR2_TSER_SHIFT      (16)       /* Bits 31:16: Number of data transfer extension */
#define SPI_CR2_TSER_MASK       (0xffff << SPI_CR2_TSER_SHIFT)
#define SPI_CR2_TSIZE_SHIFT     (0)        /* Bits 15:0: Number of data at current transfer */
#define SPI_CR2_TSIZE_MASK      (0xffff << SPI_CR2_TSIZE_SHIFT)

/* SPI Configuration Register 1 (CFG1) */

#define SPI_CFG1_DSIZE_SHIFT    (0)        /* Bits 4:0: Data frame size */
#define SPI_CFG1_DSIZE_MASK     (0x1f << SPI_CFG1_DSIZE_SHIFT)
#  define SPI_CFG1_DSIZE(n)     ((uint32_t)((n) - 1) << SPI_CFG1_DSIZE_SHIFT)
#  define SPI_CFG1_DSIZE_4BIT   (3 << SPI_CFG1_DSIZE_SHIFT)
#  define SPI_CFG1_DSIZE_8BIT   (7 << SPI_CFG1_DSIZE_SHIFT)
#  define SPI_CFG1_DSIZE_16BIT  (15 << SPI_CFG1_DSIZE_SHIFT)
#define SPI_CFG1_FTHLV_SHIFT    (5)        /* Bits 8:5: FIFO threshold level */
#define SPI_CFG1_FTHLV_MASK     (0xf << SPI_CFG1_FTHLV_SHIFT)
#  define SPI_CFG1_FTHLV_1DATA  (0 << SPI_CFG1_FTHLV_SHIFT)
#  define SPI_CFG1_FTHLV_2DATA  (1 << SPI_CFG1_FTHLV_SHIFT)
#  define SPI_CFG1_FTHLV_4DATA  (3 << SPI_CFG1_FTHLV_SHIFT)
#  define SPI_CFG1_FTHLV_8DATA  (7 << SPI_CFG1_FTHLV_SHIFT)
#  define SPI_CFG1_FTHLV_16DATA (15 << SPI_CFG1_FTHLV_SHIFT)
#define SPI_CFG1_UDRCFG_SHIFT   (9)        /* Bits 10:9: Behavior of slave TX at underrun */
#define SPI_CFG1_UDRCFG_MASK    (3 << SPI_CFG1_UDRCFG_SHIFT)
#define SPI_CFG1_UDRDET_SHIFT   (11)       /* Bits 12:11: Detection of underrun condition */
#define SPI_CFG1_UDRDET_MASK    (3 << SPI_CFG1_UDRDET_SHIFT)
#define SPI_CFG1_RXDMAEN       (1 << 14)   /* Bit 14: Rx DMA stream enable */
#define SPI_CFG1_TXDMAEN       (1 << 15)   /* Bit 15: Tx DMA stream enable */
#define SPI_CFG1_CRCSIZE_SHIFT  (16)       /* Bits 20:16: CRC size */
#define SPI_CFG1_CRCSIZE_MASK   (0x1f << SPI_CFG1_CRCSIZE_SHIFT)
#define SPI_CFG1_CRCEN         (1 << 22)   /* Bit 22: Hardware CRC computation enable */
#define SPI_CFG1_MBR_SHIFT      (28)       /* Bits 30:28: Master baud rate prescaler */
#define SPI_CFG1_MBR_MASK       (7 << SPI_CFG1_MBR_SHIFT)
#  define SPI_CFG1_MBRd2        (0 << SPI_CFG1_MBR_SHIFT) /* fPCLK/2 */
#  define SPI_CFG1_MBRd4        (1 << SPI_CFG1_MBR_SHIFT) /* fPCLK/4 */
#  define SPI_CFG1_MBRd8        (2 << SPI_CFG1_MBR_SHIFT) /* fPCLK/8 */
#  define SPI_CFG1_MBRd16       (3 << SPI_CFG1_MBR_SHIFT) /* fPCLK/16 */
#  define SPI_CFG1_MBRd32       (4 << SPI_CFG1_MBR_SHIFT) /* fPCLK/32 */
#  define SPI_CFG1_MBRd64       (5 << SPI_CFG1_MBR_SHIFT) /* fPCLK/64 */
#  define SPI_CFG1_MBRd128      (6 << SPI_CFG1_MBR_SHIFT) /* fPCLK/128 */
#  define SPI_CFG1_MBRd256      (7 << SPI_CFG1_MBR_SHIFT) /* fPCLK/256 */

/* SPI Configuration Register 2 (CFG2) */

#define SPI_CFG2_MSSI_SHIFT     (0)        /* Bits 2:0: Master SS Idleness */
#define SPI_CFG2_MSSI_MASK      (7 << SPI_CFG2_MSSI_SHIFT)
#define SPI_CFG2_MIDI_SHIFT     (4)        /* Bits 7:4: Master Inter-Data Idleness */
#define SPI_CFG2_MIDI_MASK      (0xf << SPI_CFG2_MIDI_SHIFT)
#define SPI_CFG2_RDIOP          (1 << 13)  /* Bit 13: Rx DQ on Previous Packets */
#define SPI_CFG2_RDIOM          (1 << 14)  /* Bit 14: Rx DQ on Master */
#define SPI_CFG2_IOSWP          (1 << 15)  /* Bit 15: Swap functionality of MISO and MOSI */
#define SPI_CFG2_COMM_SHIFT     (17)       /* Bits 18:17: SPI Communication mode */
#define SPI_CFG2_COMM_MASK      (3 << SPI_CFG2_COMM_SHIFT)
#  define SPI_CFG2_COMM_FULL    (0 << SPI_CFG2_COMM_SHIFT) /* Full-duplex */
#  define SPI_CFG2_COMM_SIMPLEX_TX (1 << SPI_CFG2_COMM_SHIFT) /* Simplex TX */
#  define SPI_CFG2_COMM_SIMPLEX_RX (2 << SPI_CFG2_COMM_SHIFT) /* Simplex RX */
#  define SPI_CFG2_COMM_HALF    (3 << SPI_CFG2_COMM_SHIFT) /* Half-duplex */
#define SPI_CFG2_SP_SHIFT       (19)       /* Bits 21:19: Serial Protocol */
#define SPI_CFG2_SP_MASK        (7 << SPI_CFG2_SP_SHIFT)
#  define SPI_CFG2_SP_MOTOROLA  (0 << SPI_CFG2_SP_SHIFT) /* Motorola mode */
#  define SPI_CFG2_SP_TI        (1 << SPI_CFG2_SP_SHIFT) /* TI mode */
#define SPI_CFG2_MASTER         (1 << 22)  /* Bit 22: SPI Master mode */
#define SPI_CFG2_LSBFRST        (1 << 23)  /* Bit 23: Data frame format */
#define SPI_CFG2_CPHA           (1 << 24)  /* Bit 24: Clock Phase */
#define SPI_CFG2_CPOL           (1 << 25)  /* Bit 25: Clock Polarity */
#define SPI_CFG2_SSM            (1 << 26)  /* Bit 26: Software Slave Management */
#define SPI_CFG2_SSIOP          (1 << 28)  /* Bit 28: SS Input/Output Polarity */
#define SPI_CFG2_SSOE           (1 << 29)  /* Bit 29: SS Output Enable */
#define SPI_CFG2_SSOM           (1 << 30)  /* Bit 30: SS Output Management in Master mode */
#define SPI_CFG2_AFCNTR         (1 << 31)  /* Bit 31: Alternate function GPIO control */

/* SPI Interrupt Enable Register (IER) */

#define SPI_IER_RXPIE           (1 << 0)   /* Bit 0: Rx Packet Available Interrupt Enable */
#define SPI_IER_TXPIE           (1 << 1)   /* Bit 1: Tx Packet Request Interrupt Enable */
#define SPI_IER_DXPIE           (1 << 2)   /* Bit 2: Duplex Transfer Interrupt Enable */
#define SPI_IER_EOTIE           (1 << 3)   /* Bit 3: End of Transfer Interrupt Enable */
#define SPI_IER_TXTFIE          (1 << 4)   /* Bit 4: Transfer Filled Interrupt Enable */
#define SPI_IER_UDRIE           (1 << 5)   /* Bit 5: Underrun Interrupt Enable */
#define SPI_IER_OVRIE           (1 << 6)   /* Bit 6: Overrun Interrupt Enable */
#define SPI_IER_CRCEIE          (1 << 7)   /* Bit 7: CRC Error Interrupt Enable */
#define SPI_IER_TIFREIE         (1 << 8)   /* Bit 8: TI Frame Error Interrupt Enable */
#define SPI_IER_MODFIE          (1 << 9)   /* Bit 9: Mode Fault Interrupt Enable */
#define SPI_IER_TSERFIE         (1 << 10)  /* Bit 10: TSER Loaded Interrupt Enable */

/* SPI Status Register (SR) */

#define SPI_SR_RXP              (1 << 0)   /* Bit 0: Rx Packet Available */
#define SPI_SR_TXP              (1 << 1)   /* Bit 1: Tx Packet Available */
#define SPI_SR_DXP              (1 << 2)   /* Bit 2: Duplex Transfer */
#define SPI_SR_EOT              (1 << 3)   /* Bit 3: End of Transfer */
#define SPI_SR_TXTF             (1 << 4)   /* Bit 4: Transmission Transfer Filled */
#define SPI_SR_UDR              (1 << 5)   /* Bit 5: Underrun */
#define SPI_SR_OVR              (1 << 6)   /* Bit 6: Overrun */
#define SPI_SR_CRCERR           (1 << 7)   /* Bit 7: CRC Error */
#define SPI_SR_TIFRE            (1 << 8)   /* Bit 8: TI Frame Error */
#define SPI_SR_MODF             (1 << 9)   /* Bit 9: Mode Fault */
#define SPI_SR_TSERF            (1 << 10)  /* Bit 10: TSER Loaded */
#define SPI_SR_SUSP             (1 << 11)  /* Bit 11: SUSPend */
#define SPI_SR_TXC              (1 << 12)  /* Bit 12: TxFIFO Transmission Complete */
#define SPI_SR_RXPLVL_SHIFT     (13)       /* Bits 14:13: RxFIFO Packing Level */
#define SPI_SR_RXPLVL_MASK      (3 << SPI_SR_RXPLVL_SHIFT)
#define SPI_SR_RXWNE            (1 << 15)  /* Bit 15: RxFIFO Word Not Empty */
#define SPI_SR_CTSIZE_SHIFT     (16)       /* Bits 31:16: Number of data frames remaining */
#define SPI_SR_CTSIZE_MASK      (0xffff << SPI_SR_CTSIZE_SHIFT)

/* SPI Interrupt Flag Clear Register (IFCR) */

#define SPI_IFCR_EOTC           (1 << 3)   /* Bit 3: End of Transfer flag Clear */
#define SPI_IFCR_TXTFC          (1 << 4)   /* Bit 4: Transmission Transfer Filled flag Clear */
#define SPI_IFCR_UDRC           (1 << 5)   /* Bit 5: Underrun flag Clear */
#define SPI_IFCR_OVRC           (1 << 6)   /* Bit 6: Overrun flag Clear */
#define SPI_IFCR_CRCEC          (1 << 7)   /* Bit 7: CRC Error flag Clear */
#define SPI_IFCR_TIFREC         (1 << 8)   /* Bit 8: TI Frame Error flag Clear */
#define SPI_IFCR_MODFC          (1 << 9)   /* Bit 9: Mode Fault flag Clear */
#define SPI_IFCR_TSERFC         (1 << 10)  /* Bit 10: TSER Loaded flag Clear */
#define SPI_IFCR_SUSPC          (1 << 11)  /* Bit 11: SUSPend flag Clear */

/* I2S Configuration Register (I2SCFGR) */

#define SPI_I2SCFGR_I2SMOD      (1 << 0)   /* Bit 0: I2S mode selection */
#define SPI_I2SCFGR_I2SCFG_SHIFT (1)       /* Bits 3:1: I2S configuration mode */
#define SPI_I2SCFGR_I2SCFG_MASK (7 << SPI_I2SCFGR_I2SCFG_SHIFT)
#define SPI_I2SCFGR_I2SSTD_SHIFT (4)       /* Bits 5:4: I2S standard selection */
#define SPI_I2SCFGR_I2SSTD_MASK (3 << SPI_I2SCFGR_I2SSTD_SHIFT)
#define SPI_I2SCFGR_PCMSYNC     (1 << 7)   /* Bit 7: PCM frame synchronization */
#define SPI_I2SCFGR_DATLEN_SHIFT (8)       /* Bits 10:8: Data length to be transferred */
#define SPI_I2SCFGR_DATLEN_MASK (7 << SPI_I2SCFGR_DATLEN_SHIFT)
#define SPI_I2SCFGR_CHLEN       (1 << 11)  /* Bit 11: Channel length */
#define SPI_I2SCFGR_CKPOL       (1 << 12)  /* Bit 12: Clock polarity */
#define SPI_I2SCFGR_FIXCH       (1 << 13)  /* Bit 13: Fixed channel length */
#define SPI_I2SCFGR_WSINV       (1 << 14)  /* Bit 14: Word select inversion */
#define SPI_I2SCFGR_DATFMT      (1 << 15)  /* Bit 15: Data frame format */
#define SPI_I2SCFGR_I2SDIV_SHIFT (16)      /* Bits 23:16: Linear prescaler */
#define SPI_I2SCFGR_I2SDIV_MASK (0xff << SPI_I2SCFGR_I2SDIV_SHIFT)
#define SPI_I2SCFGR_ODD         (1 << 24)  /* Bit 24: Odd factor for the prescaler */
#define SPI_I2SCFGR_MCKOE       (1 << 25)  /* Bit 25: Master clock output enable */

#endif /* __ARCH_ARM_SRC_STM32N6_HARDWARE_STM32_SPI_H */
