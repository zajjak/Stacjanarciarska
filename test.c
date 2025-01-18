#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/wait.h>

#define NUM_CHAIRS 10
#define SEAT_CAPACITY 3
#define NUM_PROCESSES 10  // Liczba procesów próbujących wsiąść
#define RIDE_TIME 200      // Czas jazdy (mikrosekundy)

struct Chair {
    pid_t pids[SEAT_CAPACITY];
    int count;
};

void sem_wait(int semid, int semnum) {
    struct sembuf op = {semnum, -1, 0};
    semop(semid, &op, 1);
}

void sem_signal(int semid, int semnum) {
    struct sembuf op = {semnum, 1, 0};
    semop(semid, &op, 1);
}

int main() {
    int shmid = shmget(IPC_PRIVATE, NUM_CHAIRS * sizeof(struct Chair), IPC_CREAT | 0666);
    int semid = semget(IPC_PRIVATE, NUM_CHAIRS, IPC_CREAT | 0666);
    int semid2 = semget(IPC_PRIVATE, NUM_CHAIRS, IPC_CREAT | 0666);

    struct Chair* chairs = (struct Chair*)shmat(shmid, NULL, 0);

    // Inicjalizacja krzesełek i semaforów
    for (int i = 0; i < NUM_CHAIRS; i++) {
        chairs[i].count = 3;
        for (int j = 0; j < SEAT_CAPACITY; j++) {
            chairs[i].pids[j] = 0;
        }
        semctl(semid, i, SETVAL, 3);
    }

    // Tworzenie wielu procesów
    for (int i = 0; i < NUM_PROCESSES; i++) {
        if (fork() == 0) {
            while (1) {
                int chair_num = i / SEAT_CAPACITY;  // Grupy po trzy procesy na kolejne krzesełka
                sem_wait(semid, chair_num);
                if (chairs[chair_num].count < SEAT_CAPACITY) {
                    chairs[chair_num].pids[chairs[chair_num].count] = getpid();
                    chairs[chair_num].count++;
                    printf("Process %d seated at chair %d\n", getpid(), chair_num);
                } else {
                    // printf("Process %d couldn't find a seat\n", getpid());
                }
                sleep(2);
            }
            // shmdt(chairs);
            exit(0);
        }
    }

    // Symulacja cyklicznego odjazdu i wysiadania krzesełek
    while (1) {
        for (int i = 0; i < NUM_CHAIRS; i++) {
            usleep(100);
            semctl(semid, i, SETVAL, 0);
            if (chairs[i].count > 0) {
                printf("Chair %d departed with %d people\n", i, chairs[i].count);
                usleep(RIDE_TIME);  // Symulacja czasu jazdy
                printf("Chair %d arrived, people are disembarking\n", i);
                semctl(semid, i, SETVAL, 3);
                for (int j = 0; j < chairs[i].count; j++) {
                    printf("Process %d disembarked from chair %d\n", chairs[i].pids[j], i);
                    chairs[i].pids[j] = 0;  // Usunięcie procesu z krzesełka
                }
                chairs[i].count = 0;  // Resetowanie licznika po wysiadaniu
            }
        }
    }

    shmdt(chairs);
    shmctl(shmid, IPC_RMID, NULL);
    semctl(semid, 0, IPC_RMID);

    return 0;
}
