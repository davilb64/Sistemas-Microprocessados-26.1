  .cdecls "msp430.h"
  .global main

  .text

main:
  mov.w #(WDTPW|WDTHOLD), &WDTCTL
  mov.w #0x4400, SP

  mov.w #MSG, R4 ;end. msg
  mov.w #GSM, R5 ;end. cifrada

  call #ENIGMA1
  
  jmp $
  nop

ENIGMA1:

  mov.w #RT1, R6 ;end. chave inicial
  mov.b #0x41, R7

  sub.b R7, 0(R4)
  mov.b 0(R4), R8
  add.w R8, R6
  add.b 0(R6), R7

  mov.b R7, 0(R5)

  inc.w(R5)
  inc.w(R4)
  tst.b 0(R4)
  jnz ENIGMA1


  ret


; Dados para o enigma
  .data
MSG: .byte "CABECAFEFACAFAD",0 ;Mensagem em claro
GSM: .byte "XXXXXXXXXXXXXXX",0 ;Mensagem cifrada
RT1: .byte 2, 4, 1, 5, 3, 0    ;Trama do Rotor 1




