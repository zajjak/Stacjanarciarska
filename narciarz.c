#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <signal.h> // obsluga sygnalow
#include <sys/wait.h>

#include "kolejka.h"



long ticketBuy(){
    struct timeval systemTime;
    gettimeofday(&systemTime, NULL);
    long Bt = 0;
    int wybor = 4;//dice(4);
    if(wybor == 1){
        //printf("Kupuje bilet na 1 godzine");
        Bt = Bt + 60*MINUTA;
    }
    else if(wybor == 2){
        //printf("Kupuje bilet na 2 godziny");
        Bt = Bt + 120*MINUTA;
    }
    else if(wybor == 3){
        //printf("Kupuje bilet na 3 godziny");
        Bt = Bt + 180*MINUTA;
    }
    else if(wybor == 4){
        //printf("Kupuje bilet na caly dzien");
        Bt = DURATION;
    }
    return Bt;
}
/*
Bt = czas dzialania biletu
*/


int main(){

    // ustawienie czasu
    struct timeval systemTime;
    gettimeofday(&systemTime, NULL);
    srand(systemTime.tv_usec);

    long Be = systemTime.tv_usec + ticketBuy();// bilet end
    long timeNow=systemTime.tv_usec;

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

    key_t klucz4;
    int semID4;
    if ( (klucz4 = ftok(".", 'E')) == -1 )
    {
        printf("Blad ftok (A)\n");
        exit(2);
    }
    semID4 = alokujSemafor(klucz4, 1, IPC_CREAT | 0666);

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
    
    int wybKrzesla;
   
    while(timeNow < Be){
        //printf("Process %d zaczyna jazde\n", getpid());
        // Losowa bramka na wejscie
        int wybBramki = dice(4)-1; 
        

        waitSemafor(semID1, wybBramki, 0);
        // Przejscie przez bramke
        usleep(333); 
        printf("Process %d przeszedl przez bramke %d\n", getpid(), wybBramki);
        // Wybor krzesla
        
        while(1){
            waitSemafor(semID4, 0, 0);
            printf("current_chair = %d\n", sharedNum->current_chair);
            wybKrzesla=sharedNum->current_chair;
            if (chairs[wybKrzesla].count < SEAT_CAPACITY) {
                chairs[wybKrzesla].pids[chairs[wybKrzesla].count] = getpid();
                chairs[wybKrzesla].count++;
                printf("Process %d wybrał krzeselko %d\n", getpid(), wybKrzesla);
                signalSemafor(semID4, 0);
                //printf("chairs[%d].count = %d\n", wybKrzesla, chairs[wybKrzesla].count);
                waitSemafor(semID2, wybKrzesla, 0); // Czeka na przyjazd krzesla
                //printf("Process %d wszedl na krzeselko %d\n", getpid(), wybKrzesla);
                signalSemafor(semID1,wybBramki);

                break;
            } else {
                printf("Process %d couldn't find a seat\n", getpid());
                sharedNum->current_chair=(sharedNum->current_chair+1)%NUM_CHAIRS;
                signalSemafor(semID4, 0);
            }
        }

        waitSemafor(semID3, wybKrzesla, 0);
        //printf("Process %d zszedl z krzeselka i zaczyna zjezdzac%d\n", getpid(), wybKrzesla);
        // Symulacja jazdy
        usleep(1*MINUTA);


        // Time update
        gettimeofday(&systemTime, NULL);
        timeNow = systemTime.tv_usec;
    }

    printf("Process %d konczy jazde\n", getpid());

    return 0;
}
/*
Be - bilet end
*/