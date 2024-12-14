#include <semaphore.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

int sem;
int mem;

struct sembuf Psplein = {0, -1, 0};
struct sembuf Vsplein = {0, 1, 0};
struct sembuf Psvide = {1, -1, 0};
struct sembuf Vsvide = {1, 1, 0};

int main(void)
{
    key_t clef;
    int *T;

    clef = ftok("principal.c", 'A');
    sem = semget(clef, 2, IPC_CREAT | 0666);
    if (sem == -1)
    {
        printf("\n Erreur de creation des sémaphores");
        exit(1);
    }

    clef = ftok("principal.c", 'A');
    mem = shmget(clef, sizeof(int), IPC_CREAT | 0666);
    if (mem == -1)
    {
        printf("\n Erreur de récupération de la mémoire partagée");
        exit(2);
    }
    T = shmat(mem, 0, 0);
    int Mp;
    do
    {
        Mp = 4 + 5;
        semop(sem, &Psvide, 1);
        *T = Mp;
        printf("('le producteur vient de déposer un objet : %d\n", Mp);
        semop(sem, &Vsplein, 1);
        sleep(1); 

    } while (0);

    shmdt(T);

    return 0;
}