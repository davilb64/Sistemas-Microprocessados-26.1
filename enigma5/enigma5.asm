  .cdecls "msp430.h"
  .global main

RT_TAM  .equ 6       ; Tamanho dos rotores
CONF2   .equ 4       ; Configuração do Rotor 2
CONF3   .equ 1       ; Configuração do Rotor 3

  .text

main:
  mov.w #(WDTPW|WDTHOLD), &WDTCTL
  mov.w #0x4400, SP

EXP5:
  mov.w #MSG, R4 ;end. msg
  mov.w #GSM, R5 ;end. cifrada
  clr.w &ROT_RT2 ; zera rotacao inicial
  call #ENIGMA5  ; Cifrar

  mov.w #GSM, R4 ;end. cifrada
  mov.w #DCF, R5 ;end. decifrada
  clr.w &ROT_RT2 ; zera rotacao para decifrar
  call #ENIGMA5  ; Decifrar

  jmp $
  nop

ENIGMA5:
  mov.b 0(R4), R13
  tst.b R13
  jz fim_enigma5

  sub.b #0x41, R13

  ; RT2 IDA
  mov.w R13, R11
  sub.w &ROT_RT2, R11 ; subtrai giro na entrada
  add.w #CONF2, R11   ; soma conf
  call #MOD_RT
  mov.w #RT2, R8
  add.w R11, R8
  mov.b 0(R8), R12
  mov.w R12, R11
  add.w &ROT_RT2, R11 ; soma giro na saida
  call #MOD_RT

  ; RT3 IDA
  add.w #CONF3, R11
  call #MOD_RT
  mov.w #RT3, R8
  add.w R11, R8
  mov.b 0(R8), R12
  mov.w R12, R11

  ; REFLETOR
  mov.w #RF1, R9
  add.w R11, R9
  mov.b 0(R9), R10

  ; RT3 VOLTA 
  mov.w #RT3, R8
  clr.w R11
busca_rt3:
  cmp.b 0(R8), R10
  jz achou_rt3
  inc.w R8
  inc.w R11
  jmp busca_rt3
achou_rt3:
  sub.w #CONF3, R11
  add.w #RT_TAM, R11
  call #MOD_RT
  mov.w R11, R10

  ; RT2 VOLTA
  sub.w &ROT_RT2, R10 ; desconta giro para voltar
  mov.w R10, R11
  add.w #RT_TAM, R11
  call #MOD_RT
  mov.w R11, R10

  mov.w #RT2, R8
  clr.w R11
busca_rt2:
  cmp.b 0(R8), R10
  jz achou_rt2
  inc.w R8
  inc.w R11
  jmp busca_rt2
achou_rt2:
  add.w &ROT_RT2, R11 ; devolve giro na saida
  sub.w #CONF2, R11
  add.w #RT_TAM, R11
  call #MOD_RT

  add.w #0x41, R11
  mov.b R11, 0(R5)

  ; Gira rotor apos cifrar
  inc.w &ROT_RT2
  cmp.w #RT_TAM, &ROT_RT2
  jne avanca5
  clr.w &ROT_RT2

avanca5:
  inc.w R4
  inc.w R5
  jmp ENIGMA5

fim_enigma5:
  ret

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
ROT_RT2: .word 0

MSG: .byte "CABECAFEFACAFAD",0 ; Mensagem em claro
GSM: .byte "XXXXXXXXXXXXXXX",0 ; Mensagem cifrada
DCF: .byte "XXXXXXXXXXXXXXX",0 ; Mensagem decifrada

; Rotores disponíveis
ROTORES:
RT1: .byte 2, 4, 1, 5, 3, 0    ; Trama do Rotor 1
RT2: .byte 1, 5, 3, 2, 0, 4    ; Trama do Rotor 2
RT3: .byte 4, 0, 5, 2, 3, 1    ; Trama do Rotor 3
RT4: .byte 3, 4, 1, 5, 2, 0    ; Trama do Rotor 4
RT5: .byte 5, 2, 3, 4, 1, 0    ; Trama do Rotor 5

; Refletores disponíveis
REFLETORES:
RF1: .byte 3, 5, 4, 0, 2, 1    ; Tabela do Refletor 1
RF2: .byte 4, 5, 3, 2, 0, 1    ; Tabela do Refletor 2
RF3: .byte 3, 2, 1, 0, 5, 4    ; Tabela do Refletor 3