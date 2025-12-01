# Contagem de Frequência com Processos e Threads  

Este repositório reúne quatro implementações para contagem da frequência de números em um arquivo contendo **100 milhões de valores** (um por linha, entre 1 e 100). O objetivo é comparar o desempenho entre:

• solução sequencial  
• paralelização com threads  
• paralelização com múltiplos processos  
• variação no nível de paralelismo  

Os testes foram realizados em ambiente Linux (Ubuntu via WSL2), utilizando linguagem C, a biblioteca **PThreads** e o benchmark **hyperfine**.

---

## 1. Arquivos do Repositório

| Arquivo | Descrição |
|--------|-----------|
| `conta_frequencia_sequencial.c` | Implementação sequencial fornecida na atividade |
| `contagem_com_2threads.c` | Implementação paralela usando 2 threads |
| `contagem_com_10threads.c` | Implementação paralela usando 10 threads |
| `contagem_com_2processos.c` | Implementação usando 2 processos e memória compartilhada |
| `numeros.txt` | Arquivo de entrada (100 milhões de números, não incluso por tamanho) |


## 2. Como Compilar

Execute os comandos a seguir no terminal dentro da pasta do projeto.

### Sequencial
```bash
gcc -O2 -o contagem_sequencial conta_frequencia_sequencial.c
```
### 2 threads
```bash
gcc -O2 -pthread -o contagem_com_2threads contagem_com_2threads.c
```
### 10 threads
```bash
gcc -O2 -pthread -o contagem_com_10threads contagem_com_10threads.c
```
### 2 processos
```bash
gcc -O2 -o contagem_com_2processos contagem_com_2processos.c
```

## 3. Como Executar

A versão sequencial recebe o arquivo de entrada como parâmetro:
```bash
./contagem_sequencial numeros.txt
```
As versões paralelas usam o mesmo arquivo numeros.txt definido no código.

## 4. Medindo Desempenho com Hyperfine
Instalação:
```bash
sudo apt install hyperfine
```
Exemplo de benchmark:
```bash
hyperfine "./contagem_sequencial numeros.txt"
```

## 5. Resultados Obtidos

### Tabela de desempenho

| Programa                   | Mediana | σ (desvio padrão) | User |
|---------------------------|---------|--------------------|------|
| contagem_sequencial       | 4.268 s | 0.314 s            | 4.356 s |
| contagem_com_2processos   | 1.291 s | 0.055 s            | 2.593 s |
| contagem_com_2threads     | 1.739 s | 0.262 s            | 3.465 s |
| contagem_com_10threads    | 1.532 s | 0.368 s            | 6.078 s |

## 6. Configuração do Computador Utilizado nos Testes

Sistema Operacional:
Windows 11 + WSL2 (Ubuntu 22.04)

Processador:
Intel Core i3-1115G4
• 2 núcleos físicos
• 4 threads
• Frequência base: 2.19 GHz
• Frequência durante testes: ~2.44 GHz
• Cache L1: 160 KB
• Cache L2: 2.5 MB
• Cache L3: 6 MB

Memória RAM:
16 GB DDR4 (3200 MT/s)

Armazenamento:
SSD NVMe ADATA 256 GB

Ferramentas:
GCC e Hyperfine

## 7. Considerações

Os experimentos mostraram como o paralelismo impacta o desempenho. A solução sequencial foi a mais lenta, enquanto a versão com 2 processos apresentou o melhor tempo. Isso acontece porque os processos rodam de forma independente, o que reduz contenção e aproveita melhor o escalonamento do sistema operacional.

As versões com threads também apresentaram melhora significativa, mas com maior variabilidade. A implementação com 10 threads não superou a de 2 processos, principalmente devido à disputa por cache, gargalo de acesso ao arquivo e maior custo de criação e sincronização de threads.

No geral, os resultados confirmam que o paralelismo traz ganhos reais, mas a quantidade de threads ou processos precisa ser escolhida considerando o hardware e a natureza da tarefa.
