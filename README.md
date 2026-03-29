# Laboratório de Sistemas Microprocessados (ENE0058)

Este repositório contém os códigos e projetos desenvolvidos para a disciplina de Laboratório de Sistemas Microprocessados (ENE0058) da Universidade de Brasília (UnB), referentes ao semestre 2026/1.

## Ambiente de Desenvolvimento

* **Hardware:** Kit Texas Instruments MSP-EXP430F5529LP.
* **IDE:** Code Composer Studio Theia.
* **Linguagens:** C e Assembly (MSP430).

## Estrutura do Repositório

Os diretórios estão organizados conforme os módulos da disciplina:

* **Módulo 0:** Introdução ao ambiente CCS Theia, manipulação básica de registradores e familiarização com a depuração em hardware.
* **Módulo 1:** Programação em Assembly. Contém exercícios focados na arquitetura do MSP430, modos de endereçamento, saltos condicionais, manipulação de memória RAM e uso de sub-rotinas.

## Compilação e Execução

1. Clone o repositório localmente.
2. No Code Composer Studio, importe os projetos pela opção `File -> Import Project(s)...` e selecione a pasta clonada.
3. Para compilar, utilize `CTRL+B`. Os binários serão gerados na pasta `Debug` de cada projeto.
4. Para gravar na placa e iniciar a depuração, utilize `F5`.