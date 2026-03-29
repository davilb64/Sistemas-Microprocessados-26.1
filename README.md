# Laboratório de Sistemas Microprocessados (ENE0058)

[cite_start]Este repositório contém os códigos e projetos desenvolvidos para a disciplina de Laboratório de Sistemas Microprocessados (ENE0058) da Universidade de Brasília (UnB), referentes ao semestre 2026/1[cite: 1023, 1027].

## Ambiente de Desenvolvimento

* [cite_start]**Hardware:** Kit Texas Instruments MSP-EXP430F5529LP[cite: 1066].
* [cite_start]**IDE:** Code Composer Studio Theia[cite: 2].
* [cite_start]**Linguagens:** C e Assembly (MSP430)[cite: 1036, 1038].

## Estrutura do Repositório

Os diretórios estão organizados conforme os módulos da disciplina:

* [cite_start]**Módulo 0:** Introdução ao ambiente CCS Theia, manipulação básica de registradores e familiarização com a depuração em hardware[cite: 1, 1074].
* **Módulo 1:** Programação em Assembly. [cite_start]Contém exercícios focados na arquitetura do MSP430, modos de endereçamento, saltos condicionais, manipulação de memória RAM e uso de sub-rotinas[cite: 546, 1074].

## Compilação e Execução

1. Clone o repositório localmente.
2. [cite_start]No Code Composer Studio Theia, importe os projetos pela opção `File -> Import Project(s)...` e selecione a pasta clonada[cite: 8].
3. Para compilar, utilize `CTRL+B`. [cite_start]Os binários serão gerados na pasta `Debug` de cada projeto[cite: 62].
4. [cite_start]Para gravar na placa e iniciar a depuração, utilize `F5`[cite: 63].