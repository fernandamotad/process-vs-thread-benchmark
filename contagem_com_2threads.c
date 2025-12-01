#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define TOTAL_LINHAS 100000000L
#define N_VALORES 101

typedef struct {
    const char *arquivo;
    long linha_inicio;
    long qtd_linhas;
    long contagem[N_VALORES];
} ThreadArgs;

void contar_intervalo(const char *nome_arquivo,
                      long linha_inicio,
                      long qtd_linhas,
                      long *contagem) {
    FILE *f = fopen(nome_arquivo, "r");
    if (!f) {
        perror("fopen");
        exit(1);
    }

    long tamanho_linha = 4; /* "ddd\n" */
    long offset = linha_inicio * tamanho_linha;
    if (fseek(f, offset, SEEK_SET) != 0) {
        perror("fseek");
        fclose(f);
        exit(1);
    }

    char buffer[8];
    for (long i = 0; i < qtd_linhas; i++) {
        if (!fgets(buffer, sizeof(buffer), f)) {
            break;
        }
        int n = atoi(buffer);
        if (n >= 1 && n <= 100) {
            contagem[n]++;
        }
    }

    fclose(f);
}

void *thread_func(void *arg) {
    ThreadArgs *t = (ThreadArgs *)arg;

    for (int i = 0; i < N_VALORES; i++) {
        t->contagem[i] = 0;
    }

    contar_intervalo(t->arquivo, t->linha_inicio,
                     t->qtd_linhas, t->contagem);

    return NULL;
}

int main(void) {
    const char *arquivo = "numeros.txt";

    long linhas_por_thread = TOTAL_LINHAS / 2;

    ThreadArgs targs[2];
    pthread_t tids[2];

    for (int i = 0; i < 2; i++) {
        targs[i].arquivo = arquivo;
        targs[i].linha_inicio = i * linhas_por_thread;
        targs[i].qtd_linhas =
            (i == 1) ? TOTAL_LINHAS - linhas_por_thread : linhas_por_thread;

        pthread_create(&tids[i], NULL, thread_func, &targs[i]);
    }

    for (int i = 0; i < 2; i++) {
        pthread_join(tids[i], NULL);
    }

    long contagem_final[N_VALORES] = {0};

    for (int num = 1; num <= 100; num++) {
        contagem_final[num] =
            targs[0].contagem[num] + targs[1].contagem[num];
    }

    long total_lido = 0;
    for (int num = 1; num <= 100; num++) {
        total_lido += contagem_final[num];
    }

    printf("Total de números lidos: %ld\n", total_lido);

    int mais_freq = 1;
    int menos_freq = 1;
    for (int num = 1; num <= 100; num++) {
        if (contagem_final[num] > contagem_final[mais_freq])
            mais_freq = num;
        if (contagem_final[num] < contagem_final[menos_freq])
            menos_freq = num;
    }

    printf("Número mais frequente: %d com %ld ocorrências\n",
           mais_freq, contagem_final[mais_freq]);
    printf("Número menos frequente: %d com %ld ocorrências\n",
           menos_freq, contagem_final[menos_freq]);

    return 0;
}

