  .cdecls "msp430.h"
  .global main

  .text

main:
  mov.w #(WDTPW|WDTHOLD), &WDTCTL

  mov.w #v1, R4
  mov.b 1(R4), R5

  mov.w #v2, R4
  mov.b 1(R4), R6
  
  mov.w #v3, R4
  mov.b 1(R4), R7

  mov.w #v3, R4
  mov.b 1(R4), R8

  jmp $
  nop

  .data
v1: .byte 1, 2, 3, 4
v2: .word 1, 2, 3, 4
v3: .byte '1', '2', '3', '4'
v4: .byte "1, 2, 3, 4"