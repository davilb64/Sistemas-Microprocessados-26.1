  .cdecls "msp430.h"
  .global main

  .text

main:
  mov.w #(WDTPW|WDTHOLD), &WDTCTL
  mov.w #0x4400, SP

init:
  ;entradas
  ;Sem carry
  ;mov.w #0x0000, R4
  ;mov.w #0x0001, R5

  ;Com Carry
  mov.w #0xF00F, R4
  mov.w #0xFF0F, R5

  ;programa
  add.w R4, R5
  jc carry

end:
  jmp $
  nop

carry:
  mov.w #0xFFFF, R5
  jmp end
  nop