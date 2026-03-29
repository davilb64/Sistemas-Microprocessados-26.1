  .cdecls "msp430.h"
  .global main

  .text

main:
  mov.w #(WDTPW|WDTHOLD), &WDTCTL
  mov.w #0x4400, SP

init:
  ;entradas
  mov.w #vetor, R12 
  mov.w #0x05, R13
  mov.w @R12, R4 ;maior valor atual
  mov.w #1, R5 ;contador igual
  
  ;programa
  dec.b(R13)
  call #maior16
  mov.w R4, R12
  mov.w R5, R13

end:
  jmp $
  nop

maior16:
  add.w #2, R12
  cmp.w @R12, R4
  jhs r4maior
  jlo r4menor
  
r4maior:
  jz igual
  dec.w(R13)
  jnz maior16
  jmp endMaior16
  

r4menor:
  mov.w @R12, R4
  dec.w(R13)
  mov.w #1, R5
  jnz maior16
  jmp endMaior16

igual:
  inc.w(R5)
  dec.w(R13)
  jnz maior16
  jmp endMaior16

endMaior16:
  ret

  .data
;vetor: .byte 1,2,4,8,10,16,32,64,128,254
;vetor: .byte 1,20,1,21,1,22,1,24,1,24
vetor: .word 20,200,50,100,200
