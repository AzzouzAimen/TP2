#include <semaphore.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <unistd.h>

#define MAX 5
int cpt = 0;

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
    sem = semget(clef, 2, 0666);
    if (sem == -1)
    {
        printf("\nErreur d'accès aux sémaphores");
        exit(1);
    }

    clef = ftok("principal.c", 'B');
    mem = shmget(clef, sizeof(int), 0666);
    if (mem == -1)
    {
        printf("\nErreur d'accès à la mémoire partagée");
        exit(2);
    }

    T = shmat(mem, 0, 0);
    int Mc;
    do
    {
        semop(sem, &Psplein, 1);
        Mc = *T;
        printf("Le consommateur a retiré un objet : %d\n\n", Mc);
        cpt++;
        semop(sem, &Vsvide, 1);
        sleep(1); 
    } while (cpt < MAX);

    shmdt(T);
    return 0;
}