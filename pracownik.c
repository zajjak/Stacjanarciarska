#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/msg.h>

#include "kolejka.h"

int main() {

    // ustawienie czasu
    struct timeval systemTime;
    gettimeofday(&systemTime, NULL);
    srand(systemTime.tv_usec);

    key_t klucz2;
    int semID2;
    if ( (klucz2 = ftok(".", 'B')) == -1 )
    {
        printf("Blad ftok (A)\n");
        exit(2);
    }
    semID2 = alokujSemafor(klucz2, NUM_CHAIRS, IPC_CREAT | 0666);

    key_t kluczm1;
    int shmID1;
    if ( (kluczm1 = ftok(".", 'C')) == -1 )
    {
        printf("Blad ftok (A)\n");
        exit(2);
    }
    if ((shmID1 = shmget(kluczm1, sizeof(struct Chair), IPC_CREAT | 0666)) == -1) {
        perror("Blad shmget");
        exit(1);
    }
    struct Chair* chairs = (struct Chair*)shmat(shmID1, NULL, 0);

    key_t klucz3;
    int semID3;
    if ( (klucz3 = ftok(".", 'D')) == -1 )
    {
        printf("Blad ftok (A)\n");
        exit(2);
    }
    semID3 = alokujSemafor(klucz3, NUM_CHAIRS, IPC_CREAT | 0666);


    long timeNow = systemTime.tv_usec/MINUTA;
    while (timeNow < (STRT + DURATION)) {
        for (int i = 0; i < NUM_CHAIRS; i++) {
            gettimeofday(&systemTime, NULL);
            timeNow = systemTime.tv_usec/MINUTA;

            usleep(333);
            semctl(semID2, i, SETVAL, 3); // Odblokownaie sem
            usleep(SEKUNDA*20);
            semctl(semID2, i, SETVAL, 0); // Blokowanie sem
            
            printf("Chair %d departed with %d people\n", i, chairs[i].count);
            usleep(5*MINUTA);  // Symulacja czasu jazdy
            printf("Chair %d arrived, people are disembarking\n", i);
            semctl(semID3, i, SETVAL, chairs[i].count); // Odblokowanie sem
            for (int j = 0; j < chairs[i].count; j++) {
                //printf("Process %d disembarked from chair %d\n", chairs[i].pids[j], i);
                chairs[i].pids[j] = 0;  // Usunięcie procesu z krzesełka
            }
            chairs[i].count = 0;  // Resetowanie licznika po wysiadaniu
        }
    }
    return 0;
}