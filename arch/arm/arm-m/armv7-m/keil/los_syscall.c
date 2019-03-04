/*----------------------------------------------------------------------------
 * Copyright (c) <2019>, <Huawei Technologies Co., Ltd>
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific prior written
 * permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *---------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------
 * Notice of Export Control Law
 * ===============================================
 * Huawei LiteOS may be subject to applicable export control laws and regulations, which might
 * include those applicable to Huawei LiteOS of U.S. and the country in which you are located.
 * Import, export and usage of Huawei LiteOS in any manner by you shall be in compliance with such
 * applicable export control laws and regulations.
 *---------------------------------------------------------------------------*/

#include <los_typedef.h>

/*******************************************************************************
 Function:
        VOID SVC_Handler(VOID);
 Description:
        SVC Handler.
*******************************************************************************/
__asm VOID SVC_Handler(VOID)
{
    PRESERVE8

    IMPORT svc_tables

    MRS   R3, CONTROL
    MOV   R1, #2
    MSR   CONTROL, R1

    ; if r7 == -1, enter privileged

    MVN   r1, #0
    CMP   r1, r7
    BNE   %F0
    BX    LR
0
    TST   LR, #4
    MOVEQ R0, SP
    MRSNE R0, PSP

    LDR   R1, [R0, #4 * 6]
    ORRS  R1, R3
    STR   R1, [R0, #4 * 4]      ; save original (PC | usr-bit) in R12

    ADR   R1, __svc_usr_stub
    STR   R1, [R0, #4 * 6]

    BX    LR

    NOP

__svc_usr_stub
    PUSH  {R4, R5, R12, LR}

    LDR   R4, =svc_tables

    LSRS  R5, r7, #22           ; get module id in r0
    LDR   R4, [r4, r5]

    CBZ   R4, %F2

    LSLS  R5, r7, #16           ; clean high 16 bits
    LSRS  R5, r5, #14           ; get svc id in r1

    LDR   R4, [r4, r5]

    CBZ   R4, %F2

    BLX   R4
0
    POP   {R4, R5, R12, LR}

    TST   R12, #1
    BEQ   %F1
    MOV   R3, #3
    MSR   CONTROL, R3
    BX    R12
1
    ORR   R12, R12, #1
    BX    R12

2
    MVN   R0, #0                ; return -1 on error
    B     %B0

    ALIGN
}

