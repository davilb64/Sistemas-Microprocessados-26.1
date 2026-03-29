# Sistemas Microprocessados 26.1

Códigos em Assembly para o microcontrolador **MSP430F5529**, desenvolvidos no Code Composer Studio (CCS).

## Estrutura do repositório

Cada pasta contém um exercício independente com seu arquivo `.asm` (ou `.c`):

| Pasta    | Arquivo fonte        |
|----------|----------------------|
| m0ex01   | m0ex01.c             |
| m0ex02   | m0ex02.asm           |
| m0ex04   | m0ex04.asm           |
| m0ex05   | m0ex05.asm           |
| m1ex3    | m1ex3.asm            |
| m1ex7    | m1ex7.asm            |
| m1ex16   | m1ex16.asm           |
| m1ex17   | m1ex17.asm           |
| m1ex21   | m1ex21.asm           |

## Como rodar no CCS (Code Composer Studio)

### Pré-requisitos

- [Code Composer Studio](https://www.ti.com/tool/CCSTUDIO) instalado (versão 12 ou superior recomendada)
- Suporte ao MSP430 instalado (selecione durante a instalação do CCS)
- Placa **MSP430F5529 LaunchPad** (ou emulador)

### Passo a passo

#### 1. Criar um novo projeto no CCS

1. Abra o CCS e vá em **File → New → CCS Project**
2. Preencha os campos:
   - **Target**: selecione `MSP430F5529`
   - **Project name**: use o mesmo nome da pasta (ex: `m0ex02`)
   - **Project type**: `Empty Project (with main.asm)` para projetos Assembly, ou `Empty Project (with main.c)` para C
3. Clique em **Finish**

#### 2. Adicionar o arquivo fonte

1. Clique com o botão direito no projeto recém-criado no **Project Explorer**
2. Vá em **Add Files...**
3. Navegue até a pasta do exercício (ex: `m0ex02/`) e selecione o arquivo `m0ex02.asm`
4. Escolha **Copy files** e clique em **OK**
5. Delete o arquivo `main.asm` (ou `main.c`) que foi criado automaticamente pelo CCS, se houver

> **Alternativa:** arraste o arquivo `.asm` diretamente para dentro do projeto no Project Explorer.

#### 3. Compilar (Build)

1. Clique no ícone de **martelo** (Build) na barra de ferramentas, ou use **Project → Build Project**
2. Verifique a aba **Console** — não deve haver erros

#### 4. Depurar / Gravar na placa

1. Conecte a placa MSP430F5529 LaunchPad via USB
2. Clique no ícone de **inseto** (Debug) ou use **Run → Debug**
3. O CCS vai gravar o firmware na placa e pausar na primeira instrução
4. Use **Run → Resume (F8)** para executar o programa
5. Para encerrar a sessão de debug, clique em **Run → Terminate**

### Dicas

- Certifique-se de que o driver USB da LaunchPad está instalado (normalmente instalado junto com o CCS)
- Se aparecer erro de target connection, verifique se a placa está conectada e tente **Reset** em **Run → Reset**
- Para ver registradores da CPU durante o debug, vá em **View → Registers**
