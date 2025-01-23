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
int shmID2,semID4, semID5, semID6; // ID numeru krzesla

pid_t ski_pid[P];
pid_t pracownik_pid;

// Funkcja obsługi sygnału SIGTERM
/**
 * @brief Obsługuje sygnał SIGTERM wysyłany do procesu.
 * Usuwa wszystkie semafory i pamięć dzieloną, po czym kończy działanie programu.
 * @param signum Numer sygnału (SIGTERM).
 */
void handle_sigterm(int signum) {
    reset_color();
    //printf("Otrzymano sygnał SIGTERM, zakończenie programu\n");
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
        perror("Blad semctl (main semID3)");
        exit(1);
    }
    if (semctl(semID5, 0, IPC_RMID) == -1) {
        perror("Blad semctl (main semID3)");
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

    printf("Semafory i pamięć dzielona zostały usunięte.\n");
    exit(0);
}

// Funkcja obsługi sygnału SIGINT
/**
 * @brief Obsługuje sygnał SIGINT wysyłany do procesu (np. przez Ctrl+C).
 * Usuwa wszystkie semafory i pamięć dzieloną, po czym kończy działanie programu.
 * @param signum Numer sygnału (SIGINT).
 */
void handle_sigint(int signum) {
    reset_color();
    //printf("Otrzymano sygnał SIGTERM, zakończenie programu\n");
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
        perror("Blad semctl (main semID3)");
        exit(1);
    }
    if (semctl(semID5, 0, IPC_RMID) == -1) {
        perror("Blad semctl (main semID3)");
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

    printf("Semafory i pamięć dzielona zostały usunięte.\n");
    exit(0);
}


// Start procesu Pracownika
/**
 * @brief Tworzy nowy proces potomny dla pracownika.
 * Proces potomny wykonuje program "pracownik", natomiast proces macierzysty zwraca jego PID.
 * @return PID nowego procesu potomnego.
 */
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

// Start procesu Narciarza
/**
 * @brief Tworzy nowy proces potomny dla narciarza.
 * Proces potomny wykonuje program "narciarz", natomiast proces macierzysty zwraca jego PID.
 * @return PID nowego procesu potomnego.
 */
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

// Inicjacja czasu symulacji
/**
 * @brief Rozpoczyna symulację czasu.
 * Usuwa istniejący raport, inicjuje czas symulacji oraz uruchamia procesy dla pracownika i narciarzy.
 * Kończy symulację po osiągnięciu zadanej liczby minut.
 * @param poczatek Liczba minut od 00:00, kiedy symulacja się rozpoczyna.
 * @param koniec Czas trwania symulacji w minutach.
 */
void init_time(long poczatek, long koniec){

    const char *filename = "raport.txt";
    if (remove(filename) == 0) {
        // printf("Plik %s został usunięty.\n", filename);
    } else {
        perror("Błąd usuwania pliku");
    }  

    struct timeval systemTime;
    gettimeofday(&systemTime, NULL);
    long Tps = systemTime.tv_usec/MINUTA;
    //long Ts;

    // Ustawienie czasu startu symulacji na 0
    while(Tps != 0){
        gettimeofday(&systemTime, NULL);
        Tps = systemTime.tv_usec/MINUTA;
    }

    // Poczatek czasu symulacji
    gettimeofday(&systemTime, NULL);
    long Tk = koniec*MINUTA;

    printf("Start symulacji\n");
    wyswietl_czas(poczatek, systemTime.tv_usec/MINUTA);

    // Stwożeni procesow potomnych
    pracownik_pid=spawnPracownik();
    for(int i=0;i<P;i++){
    usleep(100*dice(2));
    ski_pid[i]=spawnNarciarz();        
    }
    
    // Warunek zakonczenia symulacji
    while(systemTime.tv_usec < Tk){
        gettimeofday(&systemTime, NULL);
    }

    // Zakonczenie symulacji
    waitpid(pracownik_pid, NULL, 0);
    for (int i = 0; i < P; i++) {
        kill(ski_pid[i], SIGTERM);
    }
    
    gettimeofday(&systemTime,NULL);
    reset_color();
    printf("Koniec symulacji\n");
    reset_color();
    wyswietl_czas(STRT,systemTime.tv_usec/MINUTA);
}

// Inicjalizacja bramek na dole krzeselka
/**
 * @brief Tworzy N semaforów (bramek) na dolnym peronie.
 * Każdy semafor jest inicjowany wartością NUM_RESOURCES, co pozwala kontrolować liczbę narciarzy.
 * NUM_RESOURCES * N określa maksymalną liczbę narciarzy na peronie.
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

// Inicjalizacja semaforow do kolejki
/**
 * @brief Tworzy semafory i pamięć dzieloną na potrzeby kolejki.
 * Każdy semafor odpowiada za pojedyncze krzesło w kolejce.
 * Pamięć dzielona przechowuje informacje o krzesłach, takie jak czas dotarcia na górę oraz PID-y narciarzy na każdym krześle.
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
        chairs[i].timeTop = 999999;
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

// Inicjalizacja numeru krzesła w pamięci dzielonej
/**
 * @brief Tworzy pamięć dzieloną oraz semafory do przechowywania i synchronizacji numeru aktualnego krzesła.
 * Inicjalizuje wartość numeru krzesła na 0.
 */
void init_numer() {
    key_t klucz4, kluczm2, klucz5;

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

    if ((kluczm2 = ftok(".", 'F')) == -1) {
        printf("Blad ftok (main F)\n");
        exit(1);
    }
    if ((shmID2 = shmget(kluczm2, sizeof(struct SharedNum), IPC_CREAT | 0666)) == -1) {
        perror("Blad shmget (main shmID2)");
        exit(1);
    }
    struct SharedNum *sharedNum = (struct SharedNum *)shmat(shmID2, NULL, 0);
    if (sharedNum == (void *)-1) {
        perror("Blad shmat (main sharedNum)");
        exit(1);
    }
    sharedNum->current_chair = 0;
    shmdt(sharedNum);

    if ((klucz5 = ftok(".", 'G')) == -1) {
        printf("Blad ftok (main G)\n");
        exit(1);
    }
    if ((semID5 = semget(klucz5, 1, IPC_CREAT | 0666)) == -1) {
        perror("Blad semget (main semID4)");
        exit(1);
    }
    if (semctl(semID5, 0, SETVAL, NUM_CHAIRS/2) == -1) {
        perror("Blad semctl (main semID4)");
        exit(1);
    }
}

// Funkcja zwalniająca zasoby
/**
 * @brief Usuwa wszystkie utworzone semafory i segmenty pamięci dzielonej.
 * Funkcja jest wywoływana na końcu programu lub w momencie zakończenia symulacji.
 */
void destroy() {
    // Usuwanie semaforów
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
        perror("Blad semctl (main semID3)");
        exit(1);
    }
    if (semctl(semID5, 0, IPC_RMID) == -1) {
        perror("Blad semctl (main semID3)");
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

    reset_color();
    printf("Semafory i pamięć dzielona zostały usunięte.\n");
}


int main(){

    // Ustawienie obsługi sygnału
    signal(SIGTERM, handle_sigterm);
    signal(SIGINT, handle_sigint);
    
    // Semafory do bramek
    init_bramki();

    // Pamięć dzielona i semafory do kolejki
    init_kolejka();

    // Pamięć dzielona i semafory do numeru krzesła
    init_numer();

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