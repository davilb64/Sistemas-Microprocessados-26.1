  .cdecls "msp430.h"
  .global main

  .text

main:
  mov.w #(WDTPW|WDTHOLD), &WDTCTL

  mov.w #3, R4
  mov.w #4, R5
  clr R6

main_loop:
  call #acumula
  dec R4
  jnz main_loop

  jmp $
  nop

;subrotina
acumula:
  add R5,R6
  ret