#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    FILE *arquivo;
    long i = 0;
    const long TOTAL = 100000000;

    // Vetor para contar ocorrências (índices de 1 a 100)
    long contagem[101] = {0};

    // Abre o arquivo para leitura
    if(argc < 2) {
        arquivo = fopen("numeros.txt", "r");
    }else{
        arquivo = fopen(argv[1], "r");
    }
    
    if (arquivo == NULL) {
        perror("Erro ao abrir o arquivo");
        return 1;
    }

    // Lê os números do arquivo
    int aux;
    while (i < TOTAL && fscanf(arquivo, "%d", &aux) == 1) {
        contagem[aux]++;
        i++;
    }

    printf("Total de números lidos: %ld\n", i);

    fclose(arquivo);

    // Encontra o número mais e menos frequente
    int mais_frequente = 1;
    int menos_frequente = 100;
    
    for (int num = 1; num <= 100; num++) {
        if (contagem[num] > contagem[mais_frequente]) {
            mais_frequente = num;
        }
        if (contagem[num] < contagem[menos_frequente]) {
            menos_frequente = num;
        }
    }

    printf("Número mais frequente: %d com %ld ocorrências\n", mais_frequente, contagem[mais_frequente]);
    printf("Número menos frequente: %d com %ld ocorrências\n", menos_frequente, contagem[menos_frequente]);

    return 0;
}
