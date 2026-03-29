  .cdecls "msp430.h"
  .global main

  .text

main:
  mov.w #(WDTPW|WDTHOLD), &WDTCTL
  mov.w #0x4400, SP

init:
  ;entradas
  
  ;programa

end:
  jmp $
  nop