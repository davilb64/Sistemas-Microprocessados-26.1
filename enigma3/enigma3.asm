  .cdecls "msp430.h"
  .global main

RT_TAM  .equ 6       ; Tamanho dos rotores
CONF1   .equ 1       ; Configuração do Rotor 1

  .text

main:
  mov.w #(WDTPW|WDTHOLD), &WDTCTL
  mov.w #0x4400, SP

EXP3:
  mov.w #MSG, R4 ;end. msg 
  mov.w #GSM, R5 ;end. cifrada 
  call #ENIGMA3

  ;Decifrar
  mov.w #GSM, R4 ;end. cifrada
  mov.w #DCF, R5 ;end. decifrada
  call #ENIGMA3

  jmp $
  nop

ENIGMA3:
  mov.b 0(R4), R13
  tst.b R13
  jz fim_enigma3

  ; tira 41 da letra atual
  sub.b #0x41, R13

  ; RT1 - IDA 
  mov.w R13, R11
  add.w #CONF1, R11
  call #MOD_RT
  mov.w #RT1, R8 ; ponteiro RT1
  add.w R11, R8
  mov.b 0(R8), R12 ; valor que sai do rotor
  
  ; REFLETOR
  mov.w #RF1, R9 ; ponteiro RF1
  add.w R12, R9
  mov.b 0(R9), R10 ; guarda valor que queremos alcançar na volta

  ; RT1 VOLTA 
  mov.w #RT1, R8
  clr.w R11 
busca_rt1:
  cmp.b 0(R8), R10
  jz achou_rt1
  inc.w R8
  inc.w R11
  jmp busca_rt1

achou_rt1:
  ; calcula letra de saida 
  sub.w #CONF1, R11  
  add.w #RT_TAM, R11
  call #MOD_RT

  add.w #0x41, R11
  mov.b R11, 0(R5)

  inc.w R4
  inc.w R5
  jmp ENIGMA3

fim_enigma3:
  ret

; Sub-rotina para modulo baseada no tamanho do rotor
MOD_RT:
  cmp.w #0, R11
  jge mod_pos
  add.w #RT_TAM, R11
  jmp MOD_RT
mod_pos:
  cmp.w #RT_TAM, R11
  jl mod_ok
  sub.w #RT_TAM, R11
  jmp mod_pos
mod_ok:
  ret

; Dados para o enigma
  .data
MSG: .byte "CABECAFEFACAFAD",0 ;Mensagem em claro
GSM: .byte "XXXXXXXXXXXXXXX",0 ;Mensagem cifrada
DCF: .byte "XXXXXXXXXXXXXXX",0 ;Mensagem decifrada
RT1: .byte 2, 4, 1, 5, 3, 0    ;Trama do Rotor 1
RF1: .byte 3, 5, 4, 0, 2, 1    ;Tabela do Refletor