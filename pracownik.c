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

    // Poczatek czasu symulacji
    gettimeofday(&systemTime, NULL);
    printf("Start of kolej simulation\n");
    wyswietl_czas(STRT, systemTime.tv_usec/MINUTA);
    long Tp = systemTime.tv_usec;
    long Tk = Tp + DURATION*MINUTA + (15*MINUTA); // 15 minut na rozładownie krzesełka po zamknieciu
    long timeNow = systemTime.tv_usec/MINUTA;
    /*
     for(int i=0;i<NUM_CHAIRS;i++){
            // Wyświetlanie zawartości pamięci dzielonej `chairs`
            printf("Chair %d status:\n", i);
            printf("  Count: %d\n", chairs[i].count);
            for (int j = 0; j < SEAT_CAPACITY; j++) {
                printf("  PID[%d]: %d\n", j, chairs[i].pids[j]);
            }
    }   
    */ 


    while (systemTime.tv_usec < Tk) {
        for (int i = 0; i < NUM_CHAIRS; i++) {
            gettimeofday(&systemTime, NULL);
            timeNow = systemTime.tv_usec/MINUTA;

            usleep(SEKUNDA*10);
            semctl(semID2, i, SETVAL, 3); // Odblokownaie sem
            usleep(SEKUNDA*10);
            semctl(semID2, i, SETVAL, 0); // Blokowanie sem
            
            printf("Chair %d departed with %d people\n", i, chairs[i].count);
           
           for(int k=0;k<NUM_CHAIRS;k++){
            // Wyświetlanie zawartości pamięci dzielonej `chairs`
            printf("Chair %d status:\n", k);
            printf("  Count: %d\n", chairs[k].count);
            for (int j = 0; j < SEAT_CAPACITY; j++) {
                printf("  PID[%d]: %d\n", j, chairs[k].pids[j]);
            }
            }
            usleep(13*MINUTA);  // Symulacja czasu jazdy
            printf("Chair %d arrived, people are disembarking\n", i);
            semctl(semID3, i, SETVAL, chairs[i].count); // Odblokowanie sem
            for (int j = 0; j < chairs[i].count; j++) {
                printf("Process %d disembarked from chair %d\n", chairs[i].pids[j], i);
                chairs[i].pids[j] = 0;  // Usunięcie procesu z krzesełka
            }
            chairs[i].count = 0;  // Resetowanie licznika po wysiadaniu

            if(systemTime.tv_usec >= Tk) {
                wyswietl_czas(STRT, systemTime.tv_usec/MINUTA);
                break;
            }
        }
    }


    printf("End of kolej simulation\n");
    return 0;
}