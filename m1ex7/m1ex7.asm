  .cdecls "msp430.h"
  .global main

  .text

main:
  mov.w #(WDTPW|WDTHOLD), &WDTCTL
  mov.w #0x4400, SP

init:
  ;entradas

  ;programa
  ;iniciais:
  mov.w #0x0001, R4 ; representa menor valor atual
  mov.w #0x0001, R5 ; representa maior valor atual
  mov.w #0x2400, R6 ; contador pos atual memoria
  mov.w #0x000E, R7 ; repetiçoes
  mov.w #0, R8; temp

  mov.w R4, 0(R6) ;seta inicial no endereco
  add.w #2, R6
  mov.w R5, 0(R6) 
  call #FIB

end:
  jmp $
  nop

FIB:
  mov.w R5, R8
  add.w R4, R5
  mov.w R8, R4

  ;memo
  add.w #2, R6
  mov.w R5, 0(R6)
  dec.b(R7)
  jnz FIB

  ret


