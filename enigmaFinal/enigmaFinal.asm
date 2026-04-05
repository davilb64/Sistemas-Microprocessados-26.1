  .cdecls "msp430.h"
  .global main

RT_TAM  .equ 32      ; Tamanho dos rotores 
CONF_L  .equ 6       ; Configuração do Rotor Esq 
CONF_C  .equ 10      ; Configuração do Rotor Cen 
CONF_R  .equ 12      ; Configuração do Rotor Dir

  .text

main:
  mov.w #(WDTPW|WDTHOLD), &WDTCTL
  mov.w #0x4400, SP

VISTO1:
  mov.w #MSG, R4 ;end. msg
  mov.w #GSM, R5 ;end. cifrada
  call #RESETE   ; zera rotacoes iniciais
  call #ENIGMA   ; Cifrar

  mov.w #GSM, R4 ;end. cifrada
  mov.w #DCF, R5 ;end. decifrada
  call #RESETE   ; zera rotacoes para decifrar 
  call #ENIGMA   ; Decifrar

  jmp $
  nop

RESETE:
  clr.w &ROT_L
  clr.w &ROT_C
  clr.w &ROT_R
  ret

ENIGMA:
  mov.b 0(R4), R13
  tst.b R13
  jz fim_enigma

  ; Verifica se o caractere esta na base ok
  cmp.b #0x3B, R13
  jl pula_char
  cmp.b #0x5B, R13
  jge pula_char

  ; Subtrai 3Bh 
  sub.b #0x3B, R13

  ; RT2 (ESQUERDO) 
  mov.w R13, R11
  sub.w &ROT_L, R11   ; subtrai giro na entrada
  add.w #CONF_L, R11  ; soma conf
  call #MOD_RT
  mov.w #RT2, R8
  add.w R11, R8
  mov.b 0(R8), R12
  mov.w R12, R11
  add.w &ROT_L, R11   ; soma giro na saida
  call #MOD_RT

  ; RT3 (CENTRAL) - IDA
  sub.w &ROT_C, R11   ; subtrai giro na entrada
  add.w #CONF_C, R11  ; soma conf
  call #MOD_RT
  mov.w #RT3, R8
  add.w R11, R8
  mov.b 0(R8), R12
  mov.w R12, R11
  add.w &ROT_C, R11   ; soma giro na saida
  call #MOD_RT

  ; RT5 (DIREITO) - IDA 
  sub.w &ROT_R, R11   ; subtrai giro na entrada
  add.w #CONF_R, R11  ; soma conf
  call #MOD_RT
  mov.w #RT5, R8
  add.w R11, R8
  mov.b 0(R8), R12
  mov.w R12, R11
  add.w &ROT_R, R11   ; soma giro na saida
  call #MOD_RT

  ; REFLETOR (RF2) 
  mov.w #RF2, R9
  add.w R11, R9
  mov.b 0(R9), R10

  ; RT5 (DIREITO) - VOLTA
  mov.w R10, R11
  sub.w &ROT_R, R11   ; desconta giro para voltar
  call #MOD_RT
  mov.w R11, R10

  mov.w #RT5, R8
  clr.w R11
busca_rt_r:
  cmp.b 0(R8), R10
  jz achou_rt_r
  inc.w R8
  inc.w R11
  jmp busca_rt_r
achou_rt_r:
  add.w &ROT_R, R11   ; devolve giro na saida
  sub.w #CONF_R, R11
  call #MOD_RT
  mov.w R11, R10

  ; RT3 (CENTRAL)
  sub.w &ROT_C, R10   ; desconta giro para voltar
  mov.w R10, R11
  call #MOD_RT
  mov.w R11, R10

  mov.w #RT3, R8
  clr.w R11
busca_rt_c:
  cmp.b 0(R8), R10
  jz achou_rt_c
  inc.w R8
  inc.w R11
  jmp busca_rt_c
achou_rt_c:
  add.w &ROT_C, R11   ; devolve giro na saida
  sub.w #CONF_C, R11
  call #MOD_RT
  mov.w R11, R10

  ; RT2 (ESQUERDO) - VOLTA
  sub.w &ROT_L, R10   ; desconta giro para voltar
  mov.w R10, R11
  call #MOD_RT
  mov.w R11, R10

  mov.w #RT2, R8
  clr.w R11
busca_rt_l:
  cmp.b 0(R8), R10
  jz achou_rt_l
  inc.w R8
  inc.w R11
  jmp busca_rt_l
achou_rt_l:
  add.w &ROT_L, R11   ; devolve giro na saida
  sub.w #CONF_L, R11
  call #MOD_RT

  ; DEVOLVE PARA ASCII
  add.w #0x3B, R11
  mov.b R11, 0(R5)

  inc.w &ROT_L
  cmp.w #RT_TAM, &ROT_L
  jne avanca_main
  clr.w &ROT_L
  
  inc.w &ROT_C
  cmp.w #RT_TAM, &ROT_C
  jne avanca_main
  clr.w &ROT_C
  
  inc.w &ROT_R
  cmp.w #RT_TAM, &ROT_R
  jne avanca_main
  clr.w &ROT_R
  jmp avanca_main

pula_char:
  mov.b R13, 0(R5)

avanca_main:
  inc.w R4
  inc.w R5
  jmp ENIGMA

fim_enigma:
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

; Área de dados
  .data
ROT_L: .word 0
ROT_C: .word 0
ROT_R: .word 0

MSG: .byte "UMA NOITE DESTAS, VINDO DA CIDADE PARA O ENGENHO NOVO, "
     .byte "ENCONTREI NO TREM DA CENTRAL UM RAPAZ AQUI DO BAIRRO, "
     .byte "QUE EU CONHECO DE VISTA E DE CHAPEU.", 0
GSM: .byte "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
      .byte "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
      .byte "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX",0
DCF: .byte "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
      .byte "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
      .byte "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX",0

; Rotores disponíveis 
ROTORES:
RT1: .byte 13, 23, 0, 9, 4, 2, 5, 11, 12, 17, 21, 6, 28, 25, 30, 10
     .byte 22, 1, 3, 26, 24, 31, 8, 14, 29, 15, 18, 16, 19, 7, 27, 20
RT2: .byte 6, 24, 2, 8, 25, 20, 16, 29, 23, 0, 7, 19, 30, 17, 12, 15
     .byte 5, 4, 26, 10, 11, 18, 28, 27, 14, 9, 13, 1, 21, 31, 22, 3
RT3: .byte 6, 15, 23, 7, 27, 13, 19, 3, 16, 4, 17, 20, 24, 25, 0, 10
     .byte 30, 26, 22, 1, 8, 11, 14, 31, 9, 28, 5, 18, 12, 2, 29, 21
RT4: .byte 15, 16, 5, 18, 31, 26, 19, 28, 1, 2, 14, 12, 24, 20, 21, 0
     .byte 11, 23, 4, 10, 7, 3, 25, 29, 27, 8, 17, 6, 9, 13, 22, 30
RT5: .byte 13, 25, 1, 26, 6, 12, 9, 2, 28, 11, 16, 15, 4, 8, 3, 31
     .byte 5, 18, 23, 17, 24, 27, 0, 22, 29, 19, 7, 10, 14, 21, 20, 30

; Refletores disponíveis 
REFLETORES:
RF1: .byte 26, 23, 31, 9, 29, 20, 16, 11, 27, 3, 14, 7, 21, 28, 10, 25
     .byte 6, 22, 24, 30, 5, 12, 17, 1, 18, 15, 0, 8, 13, 4, 19, 2
RF2: .byte 20, 29, 8, 9, 23, 27, 21, 11, 2, 3, 25, 7, 13, 12, 22, 16
     .byte 15, 28, 30, 26, 0, 6, 14, 4, 31, 10, 19, 5, 17, 1, 18, 24
RF3: .byte 14, 30, 7, 5, 15, 3, 18, 2, 23, 17, 29, 28, 25, 27, 0, 4
     .byte 19, 9, 6, 16, 26, 22, 21, 8, 31, 12, 20, 13, 11, 10, 1, 24