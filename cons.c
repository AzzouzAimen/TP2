#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>

/* Define semaphore operations */
struct sembuf P = {0, -1, 0};  // P operation (wait)
struct sembuf V = {0, 1, 0};   // V operation (signal)

int main() {
    key_t key_shm, key_sem;
    int shm_id, sem_id;
    int *buffer; // Shared buffer (1 integer space)
    int Mc;      // Item to consume

    /* Generate keys for shared memory and semaphores */
    key_shm = ftok("tmp.c", 'A'); // Shared memory key
    if (key_shm == -1) {
        perror("Failed to generate shared memory key");
        exit(1);
    }

    key_sem = ftok("tmp.c", 'B'); // Semaphore key
    if (key_sem == -1) {
        perror("Failed to generate semaphore key");
        exit(1);
    }

    /* Access the shared memory segment */
    shm_id = shmget(key_shm, sizeof(int), IPC_CREAT|0666); // Shared memory segment
    if (shm_id == -1) {
        perror("Failed to access shared memory");
        exit(1);
    }

    /* Attach shared memory to process address space */
    buffer = (int *)shmat(shm_id, NULL, 0);
    if (buffer == (void *)-1) {
        perror("Failed to attach shared memory");
        exit(1);
    }

    /* Access the semaphore set */
    sem_id = semget(key_sem, 2, 0666); // 2 semaphores (svide and splein)
    if (sem_id == -1) {
        perror("Failed to access semaphores");
        exit(1);
    }

    printf("Consumer ready to consume...\n");

    while (1) {
        /* P operation on splein (wait until the buffer is full) */
        P.sem_num = 0; // splein semaphore
        if (semop(sem_id, &P, 1) == -1) {
            perror("Failed to perform P operation on splein");
            exit(1);
        }

        /* Consume the item from the buffer */
        Mc = *buffer;
        printf("Consumer: Retrieved item %d from buffer.\n", Mc);

        /* V operation on svide (signal that the buffer is now empty) */
        V.sem_num = 1; // svide semaphore
        if (semop(sem_id, &V, 1) == -1) {
            perror("Failed to perform V operation on svide");
            exit(1);
        }

        /* Simulate processing of the consumed item */
        sleep(1);
    }

    /* Detach shared memory */
    if (shmdt(buffer) == -1) {
        perror("Failed to detach shared memory");
        exit(1);
    }

    return 0;
}
