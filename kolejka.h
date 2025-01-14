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


#define PERON_CAPACITY 3
#define NUM_RESOURCES 2 // liczba blokujaca semafory / bramki
#define N 4 // liczba bramek na wejsciu/semaforow 4 + 1

#define minuta 1000
#define strt 480
#define duration 540



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

pid_t spawnNarciarz()
{
    pid_t ski_pid = fork();
    if (ski_pid == 0){
        execl("./narciarz","narciarz",NULL);
        perror("Failed to exec workerBee");
        exit(EXIT_FAILURE);
    }
    else if (ski_pid == -1){
        perror("Failed to fork bee");
        exit(EXIT_FAILURE);
    }
    return ski_pid;
}

// ===========================Działanie semaforów===========================

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
