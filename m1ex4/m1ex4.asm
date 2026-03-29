  .cdecls "msp430.h"
  .global main

  .text

main:
  mov.w #(WDTPW|WDTHOLD), &WDTCTL
  mov.w #0x4400, SP

init:
  ;entradas
  mov.b #4, R4
  mov.b #3, R5
  ;programa
  mov.b R4, R6 ;fixa R4 inicial em R6
  dec.b(R5)
mult:
  add.w R6, R4
  dec.b(R5)
  jnz mult  

end:
  jmp $
  nop