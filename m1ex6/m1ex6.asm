  .cdecls "msp430.h"
  .global main

  .text

main:
  mov.w #(WDTPW|WDTHOLD), &WDTCTL
  mov.w #0x4400, SP

init:
  ;entradas
  mov.b #0x08, R12
  mov.b #0x09, R13
  ;programa
  mov.b R13, R4
  dec.b(R12)
  call #mult8

end:
  jmp $
  nop

mult8:
  add.w R4, R13
  dec(R12)
  jnz mult8
  nop
  ret