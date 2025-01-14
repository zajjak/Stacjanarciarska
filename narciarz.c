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
    long Bt = 0;
    int wybor = dice(4);
    if(wybor == 1){
        //printf("Kupuje bilet na 1 godzine");
        Bt = Bt + 60*minuta;
    }
    else if(wybor == 2){
        //printf("Kupuje bilet na 2 godziny");
        Bt = Bt + 120*minuta;
    }
    else if(wybor == 3){
        //printf("Kupuje bilet na 3 godziny");
        Bt = Bt + 180*minuta;
    }
    else if(wybor == 4){
        //printf("Kupuje bilet na caly dzien");
        Bt = Bt + duration*minuta;
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
    int semID;
    if ( (klucz1 = ftok(".", 'A')) == -1 )
    {
        printf("Blad ftok (A)\n");
        exit(2);
    }

   semID = alokujSemafor(klucz1, N, IPC_CREAT | 0666);
  
    while(timeNow < Be){

        int wybBramki = dice(4)-1;
        waitSemafor(semID, wybBramki, 0);

        usleep(1 * minuta);
        signalSemafor(semID, wybBramki);

        // Time update
        gettimeofday(&systemTime, NULL);
        timeNow = systemTime.tv_usec;
    }

    return 0;
}
/*
Be - bilet end
*/