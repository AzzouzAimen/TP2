#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>

int sem;
int mem;
struct sembuf Psplein = {0, -1, 0};
struct sembuf Vsplein = {0, 1, 0};
struct sembuf Psvide = {1, -1, 0};
struct sembuf Vsvide = {1, 1, 0};

int main(void) {
    key_t clef;
    int p, err;

    clef = ftok("principal.c", 'A');
    sem = semget(clef, 2, IPC_CREAT | 0666);
    if (sem == -1) {
        printf("\nErreur de création des sémaphores");
        exit(1);
    }

    // Initialize semaphores
    semctl(sem, 0, SETVAL, 0); // Psplein
    semctl(sem, 1, SETVAL, 1); // Psvide

    clef = ftok("principal.c", 'B');
    mem = shmget(clef, sizeof(int), IPC_CREAT | 0666);
    if (mem == -1) {
        printf("\nErreur de création de la mémoire partagée");
        exit(2);
    }

    p = fork();
    if (p == -1) {
        printf("\nErreur de création du processus");
        exit(3);
    }
    if (p == 0) {
        err = execl("./prod", "prod", NULL);
        printf("\nErreur de chargement du programme producteur, erreur %d\n", err); 
        exit(4);  
    }

    p = fork();
    if (p == -1) {
        printf("\nErreur de création du processus");
        exit(3);
    }
    if (p == 0) {
        err = execl("./cons", "cons", NULL);
        printf("\nErreur de chargement du programme consommateur, erreur %d\n", err); 
        exit(4);  
    }

    while (wait(NULL) > 0);

    // Clean up semaphores and shared memory
    if (semctl(sem, 0, IPC_RMID) == -1) {
        perror("Erreur lors de la suppression des sémaphores");
    }
    if (shmctl(mem, IPC_RMID, NULL) == -1) {
        perror("Erreur lors de la suppression de la mémoire partagée");
    }

    return 0;
}