#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>

#define TOTAL_LINHAS 100000000L
#define N_VALORES 101

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

int main(void) {
    const char *arquivo = "numeros.txt";
    const char *shm_name = "/contagem_shm";

    long linhas_por_proc = TOTAL_LINHAS / 2;

    size_t tamanho_shm = N_VALORES * 2 * sizeof(long);

    int fd = shm_open(shm_name, O_CREAT | O_RDWR, 0600);
    if (fd == -1) {
        perror("shm_open");
        exit(1);
    }

    if (ftruncate(fd, tamanho_shm) == -1) {
        perror("ftruncate");
        exit(1);
    }

    long *shared = mmap(NULL, tamanho_shm,
                        PROT_READ | PROT_WRITE,
                        MAP_SHARED, fd, 0);
    if (shared == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    close(fd);

    for (int i = 0; i < N_VALORES * 2; i++) {
        shared[i] = 0;
    }

    pid_t pid1 = fork();
    if (pid1 == 0) {
        long *cont1 = shared; /* índices 0..100 */
        contar_intervalo(arquivo, 0, linhas_por_proc, cont1);
        exit(0);
    }

    pid_t pid2 = fork();
    if (pid2 == 0) {
        long *cont2 = shared + N_VALORES; /* índices 101..201 */
        contar_intervalo(arquivo, linhas_por_proc,
                         TOTAL_LINHAS - linhas_por_proc, cont2);
        exit(0);
    }

    int status;
    waitpid(pid1, &status, 0);
    waitpid(pid2, &status, 0);

    long contagem_final[N_VALORES] = {0};
    for (int num = 1; num <= 100; num++) {
        contagem_final[num] =
            shared[num] + shared[N_VALORES + num];
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

    munmap(shared, tamanho_shm);
    shm_unlink(shm_name);

    return 0;
}
