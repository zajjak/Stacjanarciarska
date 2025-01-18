#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <signal.h> // obsluga sygnalow
#include <sys/wait.h>

#include "kolejka.h"

int semID1; //ID semafora do bramek
int shmID1, semID2, semID3;  //ID kolejki kom., pamieci dzielonej
int shmID2,semID4; // ID numeru krzesla

pid_t ski_pid[P];
pid_t pracownik_pid;


pid_t spawnPracownik()
{
    pid_t pracownik_pid = fork();
    if (pracownik_pid == 0){
        execl("./pracownik","pracownik",NULL);
        perror("Failed to exec pracownik");
        exit(EXIT_FAILURE);
    }
    else if (pracownik_pid == -1){
        perror("Failed to fork pracownik");
        exit(EXIT_FAILURE);
    }
    return pracownik_pid;
}

pid_t spawnNarciarz()
{
    pid_t ski_pid = fork();
    if (ski_pid == 0){
        execl("./narciarz","narciarz",NULL);
        perror("Failed to exec narciarz");
        exit(EXIT_FAILURE);
    }
    else if (ski_pid == -1){
        perror("Failed to fork narciarz");
        exit(EXIT_FAILURE);
    }
    return ski_pid;
}

void init_time(long poczatek, long koniec){

    struct timeval systemTime;
    gettimeofday(&systemTime, NULL);
    long Tps = systemTime.tv_usec/MINUTA;
    long Ts;

    // Ustawienie czasu startu symulacji na 0
    while(Tps != 0){
        gettimeofday(&systemTime, NULL);
        Tps = systemTime.tv_usec/MINUTA;
    }
    // Poczatek czasu symulacji
    gettimeofday(&systemTime, NULL);
    long Tp = systemTime.tv_usec;
    long Tk = Tp + koniec*MINUTA;
    printf("Start symulacji\n");
    wyswietl_czas(poczatek, systemTime.tv_usec/MINUTA);

    for(int i=0;i<P;i++){
    usleep(MINUTA*dice(2));
    ski_pid[i]=spawnNarciarz();
    }
    pracownik_pid=spawnPracownik();


    // Warunek zakonczenia symulacji
    while(systemTime.tv_usec < Tk){
        gettimeofday(&systemTime, NULL);
    }

    // Zakonczenie symulacji
    for (int i = 0; i < P; i++) {
        kill(ski_pid[i], SIGTERM);
    }
    kill(pracownik_pid, SIGTERM);
    printf("Koniec symulacji\n");
    wyswietl_czas(STRT,systemTime.tv_usec/MINUTA);
}
/*
start = liczba minut od 00:00
koniec = liczba minut symulacji
*/

void init_bramki(){
    key_t klucz1;

    if ( (klucz1 = ftok(".", 'A')) == -1 )
    {
        printf("Blad ftok (main A)\n");
        exit(1);
    }
    if ((semID1 = semget(klucz1, N, IPC_CREAT | 0666)) == -1) {
        perror("Blad semget (main semID1)");
        exit(1);
    }

    for (int i = 0; i < N; i++) {
        if (semctl(semID1, i, SETVAL, NUM_RESOURCES) == -1) {
            perror("Blad semctl (main semID1)");
            exit(1);
        }
    }
}
/*
Inicjalizuje N semaforow(bramek)
Wartosc poczatkowa semaforow = NUM_RESOURCES
NUM_RESOURCES * N = liczba narciarzy możliwych na dolnym peronie
*/

void init_kolejka(){
    key_t klucz2, kluczm1, klucz3;

    if ( (klucz2 = ftok(".", 'B')) == -1 )
    {
        printf("Blad ftok (main B)\n");
        exit(1);
    }
    if ((semID2 = semget(klucz2, NUM_CHAIRS, IPC_CREAT | 0666)) == -1) {
        perror("Blad semget (main semID2)");
        exit(1);
    }
    for (int i = 0; i < NUM_CHAIRS; i++) {
        if (semctl(semID2, i, SETVAL, 0) == -1) {
            perror("Blad semctl (main semID2)");
            exit(1);
        }
    }

    if ( (kluczm1 = ftok(".", 'C')) == -1 )
    {
        printf("Blad ftok (main C)\n");
        exit(1);
    }
    if ((shmID1 = shmget(kluczm1, sizeof(struct Chair), IPC_CREAT | 0666)) == -1) {
        perror("Blad shmget (main shmID1)");
        exit(1);
    }
    struct Chair* chairs = (struct Chair*)shmat(shmID1, NULL, 0);
    if(chairs == (void *)-1){
        perror("Blad shmat (main chairs)");
        exit(1);
    }
    for (int i = 0; i < NUM_CHAIRS; i++) {
        chairs[i].count = 0;
        for (int j = 0; j < SEAT_CAPACITY; j++) {
            chairs[i].pids[j] = 0;
        }
    }

    if ( (klucz3 = ftok(".", 'D')) == -1 )
    {
        printf("Blad ftok (main D)\n");
        exit(1);
    }
    if ((semID3 = semget(klucz3, NUM_CHAIRS, IPC_CREAT | 0666)) == -1) {
        perror("Blad semget (main semID3)");
        exit(1);
    }
    for (int i = 0; i < NUM_CHAIRS; i++) {
        if (semctl(semID3, i, SETVAL, 0) == -1) {
            perror("Blad semctl (main semID3)");
            exit(1);
        }
    }

}

void init_numer() {
    key_t klucz4, kluczm2;

    // Tworzenie semafora
    if ((klucz4 = ftok(".", 'E')) == -1) {
        printf("Blad ftok (main E)\n");
        exit(1);
    }
    if ((semID4 = semget(klucz4, 1, IPC_CREAT | 0666)) == -1) {
        perror("Blad semget (main semID4)");
        exit(1);
    }
    if (semctl(semID4, 0, SETVAL, 1) == -1) {
        perror("Blad semctl (main semID4)");
        exit(1);
    }

    // Tworzenie pamięci współdzielonej
    if ((kluczm2 = ftok(".", 'F')) == -1) {
        printf("Blad ftok (main F)\n");
        exit(1);
    }
    if ((shmID2 = shmget(kluczm2, sizeof(struct SharedNum), IPC_CREAT | 0666)) == -1) {
        perror("Blad shmget (main shmID2)");
        exit(1);
    }

    // Inicjalizacja aktualnego numeru krzesła w pamięci współdzielonej
    struct SharedNum *sharedNum = (struct SharedNum *)shmat(shmID2, NULL, 0);
    if (sharedNum == (void *)-1) {
        perror("Blad shmat (main sharedNum)");
        exit(1);
    }

    // Inicjalizacja wartości w pamięci współdzielonej
    sharedNum->current_chair = 0;

    // Zakończenie operacji na pamięci współdzielonej
    shmdt(sharedNum);
}



void destroy(){
    if (semctl(semID1, 0, IPC_RMID) == -1) {
        perror("Blad semctl (main semID1)");
        exit(1);
    }
    if (semctl(semID2, 0, IPC_RMID) == -1) {
        perror("Blad semctl (main semID2)");
        exit(1);
    }
    if (semctl(semID3, 0, IPC_RMID) == -1) {
        perror("Blad semctl (main semID3)");
        exit(1);
    }
    if (semctl(semID4, 0, IPC_RMID) == -1) {
        perror("Blad semctl (main semID4)");
        exit(1);
    }
    if (shmctl(shmID1, IPC_RMID, NULL) == -1) {
        perror("Blad shmctl (main shmID1)");
        exit(1);
    }
    if (shmctl(shmID2, IPC_RMID, NULL) == -1) {
        perror("Blad shmctl (main shmID2)");
        exit(1);
    }
}
/*
Usuwanie semaforow i pamieci dzielonej.
*/


int main(){

    //current_chair=0;

    // Semafory do bramek
    init_bramki();

    // Pamięć dzielona i semafory do kolejki
    init_kolejka();



    // Wystartowanie czasu symulacji
    init_time(STRT,DURATION);

    // Usunięcie semaforów
    destroy();

    return 0;
}

/*
usleep(1000) to jedna minuta symuulacji
usleep(17) to jedna sekunda symulacji (+/- 1)
*/