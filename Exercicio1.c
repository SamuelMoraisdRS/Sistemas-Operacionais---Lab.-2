#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <stdlib.h>

void filho_incrementa(int *valor)
{ // Filho incrementa a variável valor usando ponteiros
    *valor = *valor + 10;
}

int main(int argc, char **argv)
{
    
    int segment_id = shmget(IPC_PRIVATE, sizeof(int), S_IRUSR | S_IWUSR);
    // Criando o ponteiro que vai apontar para o endereço de memória compartilhada
    int * sharedMemory = (int *)shmat(segment_id, NULL, 0);
    * sharedMemory = 10;
    pid_t pid_filho = fork(); // Funcao usada para criar um novo processo
    pid_t pid_filho2 = -1;
    if (pid_filho != 0) {
        pid_filho2 = fork();
    }
    if (pid_filho < 0)
    { // Erro na criacao do processo
        printf("Sou o processo %d e nao consegui criar um novo processo.\n", getpid());
        return 1;
    }
    else if (pid_filho == 0)
    { // Processo filho
        // Anexando o segmento de memoria compartilhada
        // sharedMemory = (int *)shmat(segment_id, NULL, 0);

        // Escrevendo na memória compartilhada
        // sprintf(sharedMemory, "Olá, processo pai, sou o seu filho e o meu ID é %d.", getpid());
        
        filho_incrementa(sharedMemory);
        
        printf("Sou o processo filho 1 de pid: %d, estou incrementando o valor para %d\n", getpid(), *sharedMemory);

        // Desanexando a memória compartilhada
        shmdt(sharedMemory);
        // Encerrando o processo filho
        exit(0);
    }

    else if (pid_filho2 == 0)
    { // Processo neto
        // sharedMemory = (int *)shmat(segment_id, NULL, 0);

        filho_incrementa(sharedMemory);
        printf("Sou o processo neto. Meu PID é: %d, estou incrementando o valor para %d\n", getpid(), *sharedMemory);
        shmdt(sharedMemory);
        exit(0);
    }

    else
    { // Processo pai
        // Fazendo o pai esperar que o filho termine de escrever
        while (wait(NULL) != -1){};
        // Salvando o ponteiro que aponta para a memória compartilhada
        // sharedMemory = (int *)shmat(segment_id, NULL, 0);
        // Imprimindo mensagem deixada pelo processo filho
        printf("Sou o processo pai, e este é o valor após ser incrementado: %d\n", *sharedMemory);
        // Removendo o segmento de memória compartilhada
        shmctl(segment_id, IPC_RMID, NULL);
    }
    return 0;
}