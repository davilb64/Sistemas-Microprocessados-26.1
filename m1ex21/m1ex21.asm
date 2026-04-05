  .cdecls "msp430.h"
  .global main

  .text

main:
  mov.w #(WDTPW|WDTHOLD), &WDTCTL
  mov.w #0x4400, SP

init:
  ;entradas
    ;end. vetor
    mov.w #vetor, R12
    ;tamanho do vetor
    mov.b #0x0A, R13

  ;programa
    call #ORDENA

end:
  jmp $
  nop

ORDENA:
  mov.w R12, R4 ;guarda endereco inicial do vetor
  mov.b R13, R5 ;guarda tamanho inicial do vetor
loop:
  cmp.b @R12, 1(R12) ;Compara posição atual com posterior
  jeq igual 
  jhs dstMaior
  jlo dstMenor
dstMaior:
  ;como a posterior é maior, apenas incrementa a posição atual e decrementa o contador de posições analisadas do vetor
  inc.w(R12) 
  dec.w(R13)
  jnz loop 
  jmp fimLoop ;pula pro fim se analisou ultima posiçao

dstMenor:
  ;realiza permutação entre os valores atuais
  mov.b @R12, R6 ;move o valor do end. atual para um reg. temporário
  mov.b 1(R12), 0(R12) ;permuta numeros entre as posições analisadas
  mov.b R6, 1(R12) 
  inc.w(R12)
  dec.w(R13)
  jnz loop
  jmp fimLoop ;pula pro fim se analisou ultima posiçao

igual:
  ;iguais não precisam permutar
  inc.w(R12)
  dec.w(R13)
  jnz loop
  jmp fimLoop ;pula pro fim se analisou ultima posiçao

fimLoop:
  dec.w (R5) ;Decrementa tamanho do vetor para não analisar a ultima posição
  cmp.b #1, R5 ;se R5 == 1, ele não retorna pois não terá com o que analisar
  jeq fimOrdena
  mov.w R4, R12 ;retorna endereço inicial do vetor para r12
  mov.b R5, R13 ;retorna número de comparações para r13
  jmp loop
  
fimOrdena:
  ret

  .data
vetor: .byte 10,9,8,7,6,5,4,3,2,1
;vetor: .byte 254,100,20,90,25,200,101,215,220,2
