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

    key_t klucz1;
    int semID1;
    if ( (klucz1 = ftok(".", 'A')) == -1 )
    {
        printf("Blad ftok (A)\n");
        exit(2);
    }

    semID1 = alokujSemafor(klucz1, N, IPC_CREAT | 0666);

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
        perror("Blad shmget (shmID1)");
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

    key_t kluczm2;
    int shmID2;
    if ( (kluczm2 = ftok(".", 'F')) == -1 )
    {
        printf("Blad ftok (A)\n");
        exit(2);
    }  
    if ((shmID2 = shmget(kluczm2, sizeof(struct SharedNum), IPC_CREAT | 0666)) == -1) {
        perror("Blad shmget");
        exit(1);
    }
    struct SharedNum* sharedNum = (struct SharedNum*)shmat(shmID2, NULL, 0);

    key_t klucz4;
    int semID4;
    if ( (klucz4 = ftok(".", 'E')) == -1 )
    {
        printf("Blad ftok (A)\n");
        exit(2);
    }
    semID4 = alokujSemafor(klucz4, 1, IPC_CREAT | 0666);

    // Poczatek czasu symulacji
    gettimeofday(&systemTime, NULL);
    printf("Start of kolej simulation\n");
    wyswietl_czas(STRT, systemTime.tv_usec/MINUTA);
    long Tp = systemTime.tv_usec;
    long Tk = Tp + DURATION*MINUTA+(15*MINUTA); // 15 minut na rozładownie krzesełka po zamknieciu
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
   pid_t gornaStacja;
   gornaStacja=fork();
   if(gornaStacja == 0){
            struct timeval systemTime;
            int i=0;
            while(1){
                
                        gettimeofday(&systemTime,NULL);
    
                        if(systemTime.tv_usec>=chairs[i].timeTop){
                        
                        printf("Chair %d arrived, people are disembarking\n", i);
                        semctl(semID3, i, SETVAL, chairs[i].count); // Odblokowanie sem
                        for (int j = 0; j < chairs[i].count; j++) {
                        // printf("Process %d disembarked from chair %d\n", chairs[i].pids[j], i);
                        chairs[i].pids[j] = 0;  // Usunięcie procesu z krzesełka
                        }
                        chairs[i].count = 0;  // Resetowanie licznika po wysiadaniu
                        chairs[i].timeTop = 999999;
                        i=(i+1)%NUM_CHAIRS;


                        }
                        usleep(100);
            
            }

   }
   else if(gornaStacja == -1){
    perror("error forka");
    exit(1);
   }

    while (systemTime.tv_usec < Tk) {
        for (int i = 0; i < NUM_CHAIRS; i++) {
            gettimeofday(&systemTime, NULL);
            timeNow = systemTime.tv_usec/MINUTA;
            sharedNum->current_chair=i;
            usleep(SEKUNDA*5);
            semctl(semID2, i, SETVAL, 3); // Odblokownaie sem
            usleep(SEKUNDA*5);
            semctl(semID2, i, SETVAL, 0); // Blokowanie sem

            gettimeofday(&systemTime,NULL);
            chairs[i].timeTop=systemTime.tv_usec+(20*MINUTA);
            printf("Chair %d departed with %d people %ld, %ld\n", i, chairs[i].count,chairs[i].timeTop,systemTime.tv_usec);
            
            if(systemTime.tv_usec >= Tk-(15*MINUTA)) {
                printf("Koniec regulaminowego czasu kolejki");   
                wyswietl_czas(STRT, systemTime.tv_usec/MINUTA);
                for(int k=0; k<N;k++)
                    semctl(semID1, k, SETVAL, 0);

                for(int k=0; k<NUM_CHAIRS;k++){
                    semctl(semID2,k,SETVAL,0);
                    semctl(semID3,k,SETVAL,0);
                }
                semctl(semID4,0,SETVAL,0);
            }
        }
    }
    
    kill(gornaStacja,SIGTERM);

           for(int k=0;k<NUM_CHAIRS;k++){
            // Wyświetlanie zawartości pamięci dzielonej `chairs`
            printf("Chair %d status:\n", k);
            printf("  Count: %d\n", chairs[k].count);
            for (int j = 0; j < SEAT_CAPACITY; j++) {
                printf("  PID[%d]: %d\n", j, chairs[k].pids[j]);
            }
            }  

    printf("End of kolej simulation\n");
    return 0;
}