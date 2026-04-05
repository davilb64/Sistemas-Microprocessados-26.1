  .cdecls "msp430.h"
  .global main

  .text

main:
  mov.w #(WDTPW|WDTHOLD), &WDTCTL
  mov.w #0x4400, SP

  mov.w #MSG, R4 ;end. msg
  mov.w #GSM, R5 ;end. cifrada
  call #ENIGMA2

  mov.w #GSM, R4 ;end. cifrada
  mov.w #DCF, R5 ;end. decifrada

  call #ENIGMA2

  jmp $
  nop

ENIGMA2:
  mov.w #0x0041, R7 ;Letra A
  mov.w #RT1, R8 ;ponteiro RT1
  mov.w #RF1, R9 ;ponteiro RF1

  mov.b 0(R4), R13
  sub.b R7, R13 ;tira 41 da letra atual
  mov.b R13, R11
  add.w R11, R8 ;move o ponteiro de rt1 pra desejada
  mov.b 0(R8), R12
  add.w R12, R9 ;move o ponteiro de RF1 pra desejada
  mov.b 0(R9), R10 ;guarda valor que queremos alcançar

  ;loop comparação
  mov.w #RT1, R8 ;ponteiro RT1
loopcomparacao:
  cmp.b 0(R8),R10
  jz igual
  inc.w R8
  jmp loopcomparacao

igual:
  mov.w #RT1, R10 ;inicio RT1
  sub.w R10, R8
  add.w R7, R8
  mov.b R8, 0(R5)

  inc.w(R4)
  inc.w(R5)

  tst.b 0(R4)
  jnz ENIGMA2

  ret


; Dados para o enigma
  .data
MSG: .byte "CABECAFEFACAFAD",0 ;Mensagem em claro
GSM: .byte "XXXXXXXXXXXXXXX",0 ;Mensagem cifrada
DCF: .byte "XXXXXXXXXXXXXXX",0 ;Mensagem decifrada
RT1: .byte 2, 4, 1, 5, 3, 0    ;Trama do Rotor 1
RF1: .byte 3, 5, 4, 0, 2, 1    ;Tabela do Refletor




