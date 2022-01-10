/**************************************************************************//**
 * @file     startup.s
 * @brief    CMSIS-Core(M) Device Startup File for Cortex-M3 Device
 * @version  V2.0.1
 * @date     23. July 2019
 ******************************************************************************/
/*
 * Copyright (c) 2009-2019 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

                .syntax  unified
                .arch    armv7-m

                .section .isr_vector
                .align   2
                .globl   __Vectors
                .globl   __Vectors_End
                .globl   __Vectors_Size
__Vectors:
                .long    _vStackTop                         /*     Top of Stack */
                .long    ResetISR                           /*     Reset Handler */
                .long    NMI_Handler                        /* -14 NMI Handler */
                .long    HardFault_Handler                  /* -13 Hard Fault Handler */
                .long    MemManage_Handler                  /* -12 MPU Fault Handler */
                .long    BusFault_Handler                   /* -11 Bus Fault Handler */
                .long    UsageFault_Handler                 /* -10 Usage Fault Handler */
                .long    _VectorChecksum                    /*     Checksum (required by the ROM bootloader) */
                .long    0                                  /*     Reserved */
                .long    0                                  /*     Reserved */
                .long    0                                  /*     Reserved */
                .long    SVC_Handler                        /*  -5 SVCall Handler */
                .long    DebugMon_Handler                   /*  -4 Debug Monitor Handler */
                .long    0                                  /*     Reserved */
                .long    PendSV_Handler                     /*  -2 PendSV Handler */
                .long    SysTick_Handler                    /*  -1 SysTick Handler */

                /* Interrupts */
                .long    WDT_Handler
                .long    TIMER0_Handler
                .long    TIMER1_Handler
                .long    TIMER2_Handler
                .long    TIMER3_Handler
                .long    UART0_Handler
                .long    UART1_Handler
                .long    UART2_Handler
                .long    UART3_Handler
                .long    PWM1_Handler
                .long    I2C0_Handler
                .long    I2C1_Handler
                .long    I2C2_Handler
                .long    SPI_Handler
                .long    SSP0_Handler
                .long    SSP1_Handler
                .long    PLL0_Handler
                .long    RTC_Handler
                .long    EINT0_Handler
                .long    EINT1_Handler
                .long    EINT2_Handler
                .long    EINT3_Handler
                .long    ADC_Handler
                .long    BOD_Handler
                .long    USB_Handler
                .long    CAN_Handler
                .long    DMA_Handler
                .long    I2S_Handler
                .long    ENET_Handler
                .long    RIT_Handler
                .long    MCPWM_Handler
                .long    QEI_Handler
                .long    PLL1_Handler
                .long    USBActivity_Handler
                .long    CANActivity_Handler

//                .space   (189 * 4)                          /* Interrupts 35 .. 224 are left out */
__Vectors_End:
                .equ     __Vectors_Size, __Vectors_End - __Vectors
                .size    __Vectors, . - __Vectors


                .thumb
                .section .text
                .align   2

                .thumb_func
                .type    ResetISR, %function
                .globl   ResetISR
                .fnstart
ResetISR:
                ldr      r4, =__data_section_table
                ldr      r5, =__data_section_table_end

.L_loop0:
                cmp      r4, r5
                bge      .L_loop0_done
                ldr      r1, [r4]
                ldr      r2, [r4, #4]
                ldr      r3, [r4, #8]

.L_loop0_0:
                subs     r3, #4
                ittt     ge
                ldrge    r0, [r1, r3]
                strge    r0, [r2, r3]
                bge      .L_loop0_0

                adds     r4, #12
                b        .L_loop0
.L_loop0_done:

                ldr      r3, =__bss_section_table
                ldr      r4, =__bss_section_table_end

.L_loop2:
                cmp      r3, r4
                bge      .L_loop2_done
                ldr      r1, [r3]
                ldr      r2, [r3, #4]
                movs     r0, 0

.L_loop2_0:
                subs     r2, #4
                itt      ge
                strge    r0, [r1, r2]
                bge      .L_loop2_0

                adds     r3, #8
                b        .L_loop2
.L_loop2_done:

                bl       main

                .fnend
                .size    ResetISR, . - ResetISR

/* The default macro is not used for HardFault_Handler
 * because this results in a poor debug illusion.
 */
                .thumb_func
                .type    HardFault_Handler, %function
                .weak    HardFault_Handler
                .fnstart
HardFault_Handler:
                b        .
                .fnend
                .size    HardFault_Handler, . - HardFault_Handler

                .thumb_func
                .type    Default_Handler, %function
                .weak    Default_Handler
                .fnstart
Default_Handler:
                b        .
                .fnend
                .size    Default_Handler, . - Default_Handler

/* Macro to define default exception/interrupt handlers.
 * Default handler are weak symbols with an endless loop.
 * They can be overwritten by real handlers.
 */
                .macro   Set_Default_Handler  Handler_Name
                .weak    \Handler_Name
                .set     \Handler_Name, Default_Handler
                .endm


/* Default exception/interrupt handler */

                Set_Default_Handler  NMI_Handler
                Set_Default_Handler  MemManage_Handler
                Set_Default_Handler  BusFault_Handler
                Set_Default_Handler  UsageFault_Handler
                Set_Default_Handler  SVC_Handler
                Set_Default_Handler  DebugMon_Handler
                Set_Default_Handler  PendSV_Handler
                Set_Default_Handler  SysTick_Handler

                Set_Default_Handler  WDT_Handler
                Set_Default_Handler  TIMER0_Handler
                Set_Default_Handler  TIMER1_Handler
                Set_Default_Handler  TIMER2_Handler
                Set_Default_Handler  TIMER3_Handler
                Set_Default_Handler  UART0_Handler
                Set_Default_Handler  UART1_Handler
                Set_Default_Handler  UART2_Handler
                Set_Default_Handler  UART3_Handler
                Set_Default_Handler  PWM1_Handler
                Set_Default_Handler  I2C0_Handler
                Set_Default_Handler  I2C1_Handler
                Set_Default_Handler  I2C2_Handler
                Set_Default_Handler  SPI_Handler
                Set_Default_Handler  SSP0_Handler
                Set_Default_Handler  SSP1_Handler
                Set_Default_Handler  PLL0_Handler
                Set_Default_Handler  RTC_Handler
                Set_Default_Handler  EINT0_Handler
                Set_Default_Handler  EINT1_Handler
                Set_Default_Handler  EINT2_Handler
                Set_Default_Handler  EINT3_Handler
                Set_Default_Handler  ADC_Handler
                Set_Default_Handler  BOD_Handler
                Set_Default_Handler  USB_Handler
                Set_Default_Handler  CAN_Handler
                Set_Default_Handler  DMA_Handler
                Set_Default_Handler  I2S_Handler
                Set_Default_Handler  ENET_Handler
                Set_Default_Handler  RIT_Handler
                Set_Default_Handler  MCPWM_Handler
                Set_Default_Handler  QEI_Handler
                Set_Default_Handler  PLL1_Handler
                Set_Default_Handler  USBActivity_Handler
                Set_Default_Handler  CANActivity_Handler



                .end
