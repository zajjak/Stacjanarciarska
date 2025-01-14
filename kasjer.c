#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <signal.h> // obsluga sygnalow
#include <sys/wait.h>

#include <semaphore.h>
#include "kolejka.h"

#define p 40 // ilosc narciarzy

pid_t ski_pid[p];



//struktura komunikatu
struct bufor{
        long mtype;
        int mvalue;
};

int semID1; //ID semafora do bramek
int shmID, msgID;  //ID kolejki kom., pamieci dzielonej

void init_time(long poczatek, long koniec){

    struct timeval systemTime;
    gettimeofday(&systemTime, NULL);
    long Tps = systemTime.tv_usec/1000;
    long Ts;

    // Ustawienie czasu startu symulacji na 0
    while(Tps != 0){
        gettimeofday(&systemTime, NULL);
        Tps = systemTime.tv_usec/1000;
    }
    // Poczatek czasu symulacji
    gettimeofday(&systemTime, NULL);
    long Tp = systemTime.tv_usec;
    long Tk = Tp + koniec*minuta;
    printf("Start symulacji\n");
    wyswietl_czas(poczatek, systemTime.tv_usec/minuta);

    for(int i=0;i<p;i++){
    usleep(minuta*dice(2));
    ski_pid[i]=spawnNarciarz();
    }


    // Warunek zakonczenia symulacji
    while(systemTime.tv_usec < Tk){
        gettimeofday(&systemTime, NULL);
    }

    // Zakonczenie symulacji
    for (int i = 0; i < p; i++) {
        kill(ski_pid[i], SIGTERM);
    }
    printf("Koniec symulacji\n");
    wyswietl_czas(strt,systemTime.tv_usec/minuta);
    
}
/*
start = liczba minut od 00:00
koniec = liczba minut symulacji
*/

void init_bramki(){
    key_t klucz1;

    if ( (klucz1 = ftok(".", 'A')) == -1 )
    {
        printf("Blad ftok (main)\n");
        exit(1);
    }
    if ((semID1 = semget(klucz1, N, IPC_CREAT | 0666)) == -1) {
        perror("Blad semget");
        exit(1);
    }

    for (int i = 0; i < N; i++) {
        if (semctl(semID1, i, SETVAL, NUM_RESOURCES) == -1) {
            perror("Blad semctl (SETVAL)");
            exit(1);
        }
    }
}

//funkcja koniec -- do obslugi przerwania
void koniec(int sig)
{
   msgctl(msgID,IPC_RMID,NULL);
   shmctl(shmID,IPC_RMID, NULL);
   printf("MAIN - funkcja koniec sygnal %d: Koniec.\n",sig);
   exit(1);

}

int main(){

    // Semafory do bramek
    init_bramki();

    // Utworzenie kolejki komunikatow
    key_t klucz2, kluczm;
    struct bufor komunikat;
    struct sigaction act;
    
    act.sa_handler=koniec;
    sigemptyset(&act.sa_mask);
    act.sa_flags=0;
    sigaction(SIGINT,&act,0);

    if ( (klucz2 = ftok(".", 'B')) == -1 )
    {
        printf("Blad ftok (main)\n");
        exit(1);
    }

    // Wystartowanie czasu symulacji
    init_time(strt,duration);

    // Usunięcie semaforów
    if (semctl(semID1, 0, IPC_RMID) == -1) {
        perror("Blad semctl (IPC_RMID)");
        exit(1);
    }

    return 0;
}

/*
usleep(1000) to jedna minuta symuulacji
*/