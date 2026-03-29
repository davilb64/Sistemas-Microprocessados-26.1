# Sistemas Microprocessados – 26.1

Exercícios de Assembly para o microcontrolador **MSP430F5529** desenvolvidos no Code Composer Studio (CCS).

## Estrutura do repositório

| Pasta    | Descrição |
|----------|-----------|
| `m0ex01` | Módulo 0 – Exercício 1 (C) |
| `m0ex02` | Módulo 0 – Exercício 2 |
| `m0ex04` | Módulo 0 – Exercício 4 |
| `m0ex05` | Módulo 0 – Exercício 5 |
| `m1ex3`  | Módulo 1 – Exercício 3 |
| `m1ex4`  | Módulo 1 – Exercício 4 |
| `m1ex7`  | Módulo 1 – Exercício 7 |
| `m1ex16` | Módulo 1 – Exercício 16 |
| `m1ex17` | Módulo 1 – Exercício 17 |
| `m1ex21` | Módulo 1 – Exercício 21 |

## Pré-requisitos

- [Code Composer Studio (CCS)](https://www.ti.com/tool/CCSTUDIO) versão 12 ou superior
- Suporte ao **MSP430** instalado no CCS (selecionável durante a instalação ou via *Help → Install New Software*)

## Como importar e rodar no CCS

### 1. Criar um novo projeto Assembly para MSP430

1. Abra o CCS e vá em **File → New → CCS Project**.
2. Preencha os campos:
   - **Target**: selecione a família **MSP430** e o dispositivo **MSP430F5529**.
   - **Connection**: *Texas Instruments MSP430 USB1* (ou o debugger conectado).
   - **Project name**: use o mesmo nome da pasta (ex.: `m0ex02`).
   - **Project type**: selecione **Empty Assembly-only Project**.
3. Clique em **Finish**.

### 2. Adicionar o arquivo `.asm` ao projeto

1. Copie (ou arraste) o arquivo `.asm` da pasta correspondente para dentro da pasta do projeto no **Project Explorer**.
2. Confirme a cópia quando o CCS perguntar.

### 3. Compilar (*Build*)

- Pressione **Ctrl + B** ou clique no ícone de martelo (**Build**) na barra de ferramentas.
- Verifique a aba **Console** – deve aparecer `**** Build Finished ****` sem erros.

### 4. Gravar e depurar (*Debug*)

1. Conecte a placa **MSP-EXP430F5529LP** (LaunchPad) via USB.
2. Pressione **F11** ou clique em **Run → Debug**.
3. O CCS irá gravar o firmware na placa e pausar na primeira instrução.
4. Use **F8** para continuar a execução, **F6** para avançar linha a linha (*Step Over*) ou **F5** para entrar em sub-rotinas (*Step Into*).
5. Para encerrar a sessão de debug, clique em **Run → Terminate** ou pressione **Ctrl + F2**.

### 5. Rodar sem depurador

Após gravar uma vez via debug, desconecte o cabo USB e reconecte: o MSP430 executará o código automaticamente ao receber alimentação.

## Dicas

- O watchdog timer do MSP430 é ativado por padrão e causa reset se não for desabilitado. A linha abaixo deve estar no início do `main`:
  ```asm
  mov.w #(WDTPW|WDTHOLD), &WDTCTL
  ```
- Para usar as definições de registradores (`WDTCTL`, `P1DIR`, etc.), inclua o header no topo do arquivo:
  ```asm
  .cdecls "msp430.h"
  ```
- Arquivos gerados pelo CCS (`Debug/`, `.cproject`, `.project`, etc.) já estão no `.gitignore` e não precisam ser versionados.
