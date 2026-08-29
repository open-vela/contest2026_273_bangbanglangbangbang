/****************************************************************************
 * arch/arm/src/stm32n6/hardware/stm32_usart.h
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to you under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ****************************************************************************/

#ifndef __ARCH_ARM_SRC_STM32N6_HARDWARE_STM32_USART_H
#define __ARCH_ARM_SRC_STM32N6_HARDWARE_STM32_USART_H

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* USART Register Offsets */

#define STM32_USART_CR1_OFFSET    0x0000  /* Control register 1 */
#define STM32_USART_CR2_OFFSET    0x0004  /* Control register 2 */
#define STM32_USART_CR3_OFFSET    0x0008  /* Control register 3 */
#define STM32_USART_BRR_OFFSET    0x000C  /* Baud rate register */
#define STM32_USART_GTPR_OFFSET   0x0010  /* Guard time and prescaler register */
#define STM32_USART_RTOR_OFFSET   0x0014  /* Receiver timeout register */
#define STM32_USART_RQR_OFFSET    0x0018  /* Request register */
#define STM32_USART_ISR_OFFSET    0x001C  /* Interrupt and status register */
#define STM32_USART_ICR_OFFSET    0x0020  /* Interrupt flag clear register */
#define STM32_USART_RDR_OFFSET    0x0024  /* Receive data register */
#define STM32_USART_TDR_OFFSET    0x0028  /* Transmit data register */
#define STM32_USART_PRESC_OFFSET  0x002C  /* Prescaler register */

/* USART Register Addresses */

#define STM32_USART1_CR1          (STM32_USART1_BASE + STM32_USART_CR1_OFFSET)
#define STM32_USART1_CR2          (STM32_USART1_BASE + STM32_USART_CR2_OFFSET)
#define STM32_USART1_CR3          (STM32_USART1_BASE + STM32_USART_CR3_OFFSET)
#define STM32_USART1_BRR          (STM32_USART1_BASE + STM32_USART_BRR_OFFSET)
#define STM32_USART1_ISR          (STM32_USART1_BASE + STM32_USART_ISR_OFFSET)
#define STM32_USART1_ICR          (STM32_USART1_BASE + STM32_USART_ICR_OFFSET)
#define STM32_USART1_RDR          (STM32_USART1_BASE + STM32_USART_RDR_OFFSET)
#define STM32_USART1_TDR          (STM32_USART1_BASE + STM32_USART_TDR_OFFSET)

/* USART CR1 Register Bits */

#define USART_CR1_UE              (1 << 0)   /* USART enable */
#define USART_CR1_UESM            (1 << 1)   /* USART enable in Stop mode */
#define USART_CR1_RE              (1 << 2)   /* Receiver enable */
#define USART_CR1_TE              (1 << 3)   /* Transmitter enable */
#define USART_CR1_IDLEIE           (1 << 4)   /* IDLE interrupt enable */
#define USART_CR1_RXNEIE          (1 << 5)   /* RXNE interrupt enable */
#define USART_CR1_TCIE            (1 << 6)   /* Transmission complete interrupt enable */
#define USART_CR1_TXEIE           (1 << 7)   /* TXE interrupt enable */
#define USART_CR1_PEIE            (1 << 8)   /* PE interrupt enable */
#define USART_CR1_PS              (1 << 9)   /* Parity selection */
#define USART_CR1_PCE             (1 << 10)  /* Parity control enable */
#define USART_CR1_WAKE            (1 << 11)  /* Receiver wakeup method */
#define USART_CR1_M0              (1 << 12)  /* Word length bit 0 */
#define USART_CR1_MME             (1 << 13)  /* Mute mode enable */
#define USART_CR1_CMIE            (1 << 14)  /* Character match interrupt enable */
#define USART_CR1_OVER8           (1 << 15)  /* Oversampling mode */
#define USART_CR1_DEDT            (0x1f << 16) /* Driver Enable de-assertion time */
#define USART_CR1_DEAT            (0x1f << 21) /* Driver Enable assertion time */
#define USART_CR1_RTOIE           (1 << 26)  /* Receiver timeout interrupt enable */
#define USART_CR1_EOBIE           (1 << 27)  /* End of Block interrupt enable */
#define USART_CR1_M1              (1 << 28)  /* Word length bit 1 */

/* USART ISR Register Bits */

#define USART_ISR_PE              (1 << 0)   /* Parity error */
#define USART_ISR_FE              (1 << 1)   /* Framing error */
#define USART_ISR_NE              (1 << 2)   /* Noise detected flag */
#define USART_ISR_ORE             (1 << 3)   /* Overrun error */
#define USART_ISR_IDLE            (1 << 4)   /* Idle line detected */
#define USART_ISR_RXNE            (1 << 5)   /* Read data register not empty */
#define USART_ISR_TC              (1 << 6)   /* Transmission complete */
#define USART_ISR_TXE             (1 << 7)   /* Transmit data register empty */
#define USART_ISR_LBDF            (1 << 8)   /* LIN break detection flag */
#define USART_ISR_CTSIF           (1 << 9)   /* CTS interrupt flag */
#define USART_ISR_CTS             (1 << 10)  /* CTS flag */
#define USART_ISR_RTOF            (1 << 11)  /* Receiver timeout flag */
#define USART_ISR_EOBF            (1 << 12)  /* End of block flag */
#define USART_ISR_UDR             (1 << 13)  /* SPI slave underrun error flag */
#define USART_ISR_ABRE            (1 << 14)  /* Auto baud rate error */
#define USART_ISR_ABRF            (1 << 15)  /* Auto baud rate flag */
#define USART_ISR_BUSY            (1 << 16)  /* Busy flag */
#define USART_ISR_CMF             (1 << 17)  /* Character match flag */
#define USART_ISR_SBKF            (1 << 18)  /* Send break flag */
#define USART_ISR_RWU             (1 << 19)  /* Receiver wakeup from Mute mode */
#define USART_ISR_TEACK           (1 << 21)  /* Transmit enable acknowledge flag */
#define USART_ISR_REACK           (1 << 22)  /* Receive enable acknowledge flag */

#endif /* __ARCH_ARM_SRC_STM32N6_HARDWARE_STM32_USART_H */
