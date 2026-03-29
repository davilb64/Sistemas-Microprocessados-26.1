  .cdecls "msp430.h"
  .global main

  .text

main:
  mov.w #(WDTPW|WDTHOLD), &WDTCTL
  mov.w #0x4400, SP

init:
  ;entradas
  mov.w #vetor, R12 
  mov.b #0x0A, R13
  mov.b @R12, R4 ;menor valor atual
  mov.b #1, R5 ;contador igual
  
  ;programa
  call #menor
  mov.b R4, R12
  mov.b R5, R13

end:
  jmp $
  nop

menor:
  inc.w(R12)
  cmp.b @R12, R4
  jhs r4maior
  jlo r4menor
  
r4maior:
  jz igual
  mov.b @R12, R4
  dec.w(R13)
  mov.b #1, R5
  jnz menor
  jmp endMenor

r4menor:
  dec.w(R13)
  jnz menor
  jmp endMenor

igual:
  inc.w(R5)
  dec.w(R13)
  jnz menor
  jmp endMenor

endMenor:
  ret

  .data
;vetor: .byte 1,2,4,8,10,16,32,64,128,254
vetor: .byte 1,2,4,8,1,16,32,1,128,254
