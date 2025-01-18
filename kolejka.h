#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <signal.h> // obsluga sygnaloSw
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <pthread.h>


#define NUM_RESOURCES 2 // ilość miejsca za każdą bramką
#define N 4 // liczba bramek na dole stacji

#define MINUTA 1000 // czas jednej minuty w usleep
#define SEKUNDA 17 // czas jednej sekundy w usleep
#define STRT 480 // czas startu symulacji 00:00 + <STRT> minut
#define DURATION 120 // Czas trwania symulacji w minutach

#define P 100 // ilosc narciarzy
#define SEAT_CAPACITY 3 // ilość miejsc na krześle
#define NUM_CHAIRS 10 // ilość krzeselek

struct Chair {
    pid_t pids[SEAT_CAPACITY];
    int count;
};

struct SharedNum{
    int current_chair;
};

int dice(int n)
{
    return rand() % n + 1;
}

void wyswietl_czas(long stala_minuty, long dodane_minuty) {
    long calkowite_minuty = stala_minuty + dodane_minuty;
    long godziny = calkowite_minuty / 60;
    long minuty = calkowite_minuty % 60;

    printf("Czas: %02ld:%02ld\n", godziny, minuty);
}



// ===========================Działanie semaforów===========================

void sem_wait(int semid, int semnum) {
    struct sembuf op = {semnum, -1, 0};
    semop(semid, &op, 1);
}

void sem_signal(int semid, int semnum) {
    struct sembuf op = {semnum, 1, 0};
    semop(semid, &op, 1);
}


int alokujSemafor(key_t klucz, int number, int flagi)
{
   int semID;
   if ( (semID = semget(klucz, number, flagi)) == -1)
   {
      perror("Blad semget (alokujSemafor): ");
      exit(1);
   }
   return semID;
}

int zwolnijSemafor(int semID, int number)
{
   return semctl(semID, number, IPC_RMID, NULL);
}

void inicjalizujSemafor(int semID, int number, int val)
{

   if ( semctl(semID, number, SETVAL, val) == -1 )
   {
      perror("Blad semctl (inicjalizujSemafor): ");
      exit(1);
   }
}

int waitSemafor(int semID, int number, int flags)
{
   int result;
   struct sembuf operacje[1];
   operacje[0].sem_num = number;
   operacje[0].sem_op = -1;
   operacje[0].sem_flg = 0 | flags;//SEM_UNDO;

   if ( semop(semID, operacje, 1) == -1 )
   {
      //perror("Blad semop (waitSemafor)");
      return -1;
   }

   return 1;
}

void signalSemafor(int semID, int number)
{
   struct sembuf operacje[1];
   operacje[0].sem_num = number;
   operacje[0].sem_op = 1;
   //operacje[0].sem_flg = SEM_UNDO;

   if (semop(semID, operacje, 1) == -1 )
      perror("Blad semop (postSemafor): ");

   return;
}

int valueSemafor(int semID, int number)
{
   return semctl(semID, number, GETVAL, NULL);
}
