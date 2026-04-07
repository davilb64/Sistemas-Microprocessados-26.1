  .cdecls "msp430.h"
  .global main

  .text
main:
  mov.w #(WDTPW|WDTHOLD), &WDTCTL
  mov.w #0x4400, SP

  mov.w #string, R12
  mov.w #sigla, R13 

  push R4
  call #extraiSigla
  pop R4
  
  jmp $
  nop

extraiSigla:
  mov.b @R12+, R4
  tst.b R4
  jz fim

  cmp.b #0x41, R4
  jlo extraiSigla

  cmp.b #0x5B, R4
  jhs extraiSigla

  mov.b R4, 0(R13)
  inc.w R13
  jmp extraiSigla

fim:
  mov.b #0, 0(R13)
  ret

; Dados
  .data
string: .byte "SisMicAkaEDosdngfjaOJnND",0 
sigla: .byte "XXXXXXXXXXXXXXXXXXXXXXXXX",0 

