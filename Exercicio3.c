#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <stdlib.h>

int n = 10;

int main(int argc, char **argv) {
    int segment_id = shmget(IPC_PRIVATE, sizeof(int), S_IRUSR | S_IWUSR);
    int *sharedMemory = (int *)shmat(segment_id, NULL, 0);
    *sharedMemory = 0;
    for (int i = 1; i < n; i++) {
        pid_t pid_filho = fork();
        if (pid_filho == 0) {
            printf("Sou um processo filho (PID: %d), estou fazendo a operação %d + %d.\n", getpid(), *sharedMemory, i);
            *sharedMemory = *sharedMemory + i;
            shmdt(sharedMemory);
            exit(0);
        }
        wait(NULL);
    }
    while (wait(NULL) != -1){}; // Espera os processos filhos finalizarem
    printf("Sou o processo pai (PID: %d) e este é o resultado das somas: %d.\n", getpid(), *sharedMemory);
    shmctl(segment_id, IPC_RMID, NULL);
    return 0;
}