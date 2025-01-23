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
#include <sys/shm.h>
#include <pthread.h>
#include <semaphore.h>

#include "kolejka.h"

volatile bool stop_thread = false; // Flaga kontrolująca wątek

pthread_t child_thread1,child_thread2;
sem_t child_sem1, child_sem2, child_sem3,child_sem4;
int semID3;
int semID1;
int semID2;
int semID4;

/**
 * @brief Obsługuje sygnał SIGTERM.
 * 
 * Funkcja wywoływana po otrzymaniu sygnału SIGTERM. Zwalnia semafory i kończy program.
 * 
 * @param signum Numer sygnału (SIGTERM).
 */
void handle_sigterm(int signum) {
    //printf("Otrzymano sygnał SIGTERM, zakończenie programu\n");
    sem_destroy(&child_sem1);
    sem_destroy(&child_sem2);
    sem_destroy(&child_sem3);
    sem_destroy(&child_sem4);
    stop_thread = true;
    exit(0);
}

/**
 * @brief Inicjalizuje semafory i pamięć współdzieloną.
 * 
 * Tworzy unikalne klucze i przypisuje semafory do globalnych identyfikatorów semaforów.
 */
void init_shmsem(){
    key_t klucz1;
    if ( (klucz1 = ftok(".", 'A')) == -1 )
    {
        printf("Blad ftok (A)\n");
        exit(2);
    }
    semID1 = alokujSemafor(klucz1, N, IPC_CREAT | 0666);

    key_t klucz2;
    if ( (klucz2 = ftok(".", 'B')) == -1 )
    {
        printf("Blad ftok (A)\n");
        exit(2);
    }
    semID2 = alokujSemafor(klucz2, NUM_CHAIRS, IPC_CREAT | 0666);


    key_t klucz3;
    if ( (klucz3 = ftok(".", 'D')) == -1 )
    {
        printf("Blad ftok (A)\n");
        exit(2);
    }
    semID3 = alokujSemafor(klucz3, NUM_CHAIRS, IPC_CREAT | 0666);

    key_t klucz4;
    if ( (klucz4 = ftok(".", 'E')) == -1 )
    {
        printf("Blad ftok (A)\n");
        exit(2);
    }
    semID4 = alokujSemafor(klucz4, 1, IPC_CREAT | 0666);
}

/**
 * @brief Generuje czas wygaśnięcia biletu na podstawie losowego wyboru.
 * 
 * @return Czas wygaśnięcia biletu w mikrosekundach.
 */
long ticketBuy(){
    struct timeval systemTime;
    gettimeofday(&systemTime, NULL);
    long Bt = 0;
    int wybor = dice(4);
    if(wybor == 1){
        //printf("Kupuje bilet na 1 godzine");
        Bt = systemTime.tv_usec+ 60*MINUTA;
    }
    else if(wybor == 2){
        //printf("Kupuje bilet na 3 godziny");
        Bt = systemTime.tv_usec+180*MINUTA;
    }
    else if(wybor == 3){
        //printf("Kupuje bilet na 6 godziny");
        Bt = 360*MINUTA + systemTime.tv_usec;
    }
    else if(wybor == 4){
        //printf("Kupuje bilet na caly dzien");
        Bt = DURATION*MINUTA;
    }
    return Bt;
}

/**
 * @brief Symuluje zjazd osoby dorosłej.
 * 
 * @param wybor Numer wybranej trasy (1 - łatwa, 2 - średnia, 3 - trudna).
 */
void zjazd(int wybor){
    if(wybor == 1){
        //printf("Wybrano latwa trase\n");
        usleep(1*MINUTA);
    }
    else if(wybor == 2){
        //printf("Wybrano srednia trase\n");
        usleep(2*MINUTA);
    }
    else if(wybor == 3){
        //printf("Wybrano trudna trase\n");
        usleep(3*MINUTA);
    }
}

/**
 * @brief Symuluje zjazd osoby niepełnoletniej.
 * 
 * @param wybor Numer wybranej trasy (1 - łatwa, 2 - średnia, 3 - trudna).
 */
void zjazdDzieci(int wybor){
    if(wybor == 1){
        //printf("Wybrano latwa trase\n");
        usleep(2*MINUTA);
    }
    else if(wybor == 2){
        //printf("Wybrano srednia trase\n");
        usleep(3*MINUTA);
    }
    else if(wybor == 3){
        //printf("Wybrano trudna trase\n");
        usleep(4*MINUTA);
    }
}

/**
 * @brief Funkcja wątku dziecka (1 dziecko).
 * 
 * Obsługuje zachowanie dziecka podczas korzystania z kolejki linowej.
 * 
 * @param arg Argument wątku (nieużywany).
 * @return NULL
 */
void* child_thread_function1(void* arg) {
    set_color("\033[34m");
    // printf("jesem watek %ld\n",pthread_self());
    int wybBramki;
    while(!stop_thread){
        wybBramki = dice(3);
        waitSemafor(semID1, wybBramki, 0);
        sem_post(&child_sem1);
        sem_wait(&child_sem2);
        zjazdDzieci(dice(3));
    }
    set_color("\033[34m");
    // printf("jesem watek koncze dzialanie %ld\n",pthread_self());
    return NULL;
}

/**
 * @brief Funkcja wątku dziecka (2 dzieci).
 * 
 * Obsługuje zachowanie dzieci podczas korzystania z kolejki linowej.
 * 
 * @param arg Argument wątku (nieużywany).
 * @return NULL
 */
void* child_thread_function2(void* arg) {
    set_color("\033[34m");
    // printf("jesem watek %ld\n",pthread_self());
    int wybBramki;
    while(!stop_thread){
        wybBramki = dice(3);
        waitSemafor(semID1, wybBramki, 0);
        sem_post(&child_sem3);
        sem_wait(&child_sem4);
        zjazdDzieci(dice(3));
    }
    set_color("\033[34m");
    // printf("jesem watek koncze dzialanie %ld\n",pthread_self());
    return NULL;
}

int main(){

    // Ustawienie obsługi sygnału SIGTERM
    signal(SIGTERM, handle_sigterm);

    // losowanie wieku z przedzialu 18-93
    int wiek=dice(65)+18;
    if (wiek > 65){
        //printf("znizka seniora przy zakupie biletu");
    }


    // ustawienie czasu
    struct timeval systemTime;
    gettimeofday(&systemTime, NULL);
    srand(systemTime.tv_usec);

    long Be = ticketBuy();// bilet end
    long timeNow=systemTime.tv_usec;

    FILE *file = fopen("raport.txt", "a");
    if (file == NULL) {
        perror("Blad otwarcia pliku raport.txt");
        exit(1);
    }

    // =============================Semafors and Sherememory==============================

    init_shmsem();

    key_t kluczm1;
    int shmID1;
    if ( (kluczm1 = ftok(".", 'C')) == -1 )
    {
        printf("Blad ftok (A)\n");
        exit(2);
    }  
    if ((shmID1 = shmget(kluczm1, sizeof(struct Chair), IPC_CREAT | 0666)) == -1) {
        set_color("\033[31m");
        perror("Blad shmget kluczm1");
        exit(1);
    }
    struct Chair* chairs = (struct Chair*)shmat(shmID1, NULL, 0);

    key_t kluczm2;
    int shmID2;
    if ( (kluczm2 = ftok(".", 'F')) == -1 )
    {
        printf("Blad ftok (A)\n");
        exit(2);
    }  
    if ((shmID2 = shmget(kluczm2, sizeof(struct SharedNum), IPC_CREAT | 0666)) == -1) {
        set_color("\033[31m");
        perror("Blad shmget kluczm2");
        exit(1);
    }
    struct SharedNum* sharedNum = (struct SharedNum*)shmat(shmID2, NULL, 0);

    
    // ========= Czy VIP ============
    bool czyVIP = FALSE;
    if(dice(25)==1){ // losowanie, szansa 1/25
        // printf("witamy Vipa %d\n",getpid());
        czyVIP = TRUE;
    }

    // =========== Czy ma dziecko =============
    int czyDziecko = 0;
    if(dice(20)==1){ // losowanie, szansa 1/20
        set_color("\033[31m");
        //printf("narciarz z 1 dzieckiem %d\n",getpid());
        sem_init(&child_sem1, 0, 0);
        sem_init(&child_sem2, 0, 0);
        if (pthread_create(&child_thread1, NULL, child_thread_function1, NULL) != 0) {
            set_color("\033[34m");
            perror("Błąd przy tworzeniu wątku");
        return 1;
        }
        czyDziecko = 1;
    }
    else if(dice(25)==1){ // losowanie, szansa 1/25
        set_color("\033[31m");
        printf("narciarz z 2 dziecmi %d\n",getpid());
        sem_init(&child_sem1, 0, 0);
        sem_init(&child_sem2, 0, 0);
        sem_init(&child_sem3, 0, 0);
        sem_init(&child_sem4, 0, 0);
        if (pthread_create(&child_thread1, NULL, child_thread_function1, NULL) != 0) {
            set_color("\033[34m");
            perror("Błąd przy tworzeniu wątku");
            return 1;
        }    
        if (pthread_create(&child_thread2, NULL, child_thread_function2, NULL) != 0) {
            set_color("\033[34m");
            perror("Błąd przy tworzeniu wątku");
            return 1;
        }
        czyDziecko = 2;
    }

    int wybKrzesla;
    int wybBramki;
    // ============= Wybieranie bramki i krzesleka ================
    while(timeNow < Be){
        // ------------------- Przejscie przez bramke -----------------
        // Losowa bramka na wejscie
        wybBramki = dice(3);
        // Bramka VIP
        if(czyVIP==TRUE){
            wybBramki = 0;
        }
        // czeka na miejsce za bramka
        waitSemafor(semID1, wybBramki, 0);
        if(czyDziecko == 1){
            sem_wait(&child_sem1);
        }
        if(czyDziecko == 2){
            sem_wait(&child_sem1);
            sem_wait(&child_sem3);
            set_color("\033[31m");
        }

        
        // Przejscie przez bramke
        usleep(10*SEKUNDA); 
        gettimeofday(&systemTime,NULL);

        /*
        if(wybBramki==0){
            set_color("\033[95m");
            printf("Process %d przeszedl przez bramke %d\n", getpid(), wybBramki);
        }
        else{
            set_color("\033[31m");
            printf("Process %d przeszedl przez bramke %d\n", getpid(), wybBramki);
        }
        */

        
        // zapis rejestru
        long calkowite_minuty = STRT + systemTime.tv_usec/MINUTA;
        long godziny = calkowite_minuty / 60;
        long minuty = calkowite_minuty % 60;
        fprintf(file,"Czas: %02ld:%02ld\n", godziny, minuty);
        fprintf(file, "Process %d przeszedl przez bramke %d\n", getpid(), wybBramki);

        // --------------- Wybor krzesla ---------------
        while(1){
            // Sekacja krytyczna wyboru krzeselka
            waitSemafor(semID4, 0, 0);
            //printf("current_chair = %d\n", sharedNum->current_chair);
                        // Sekcja z dzickiem
            if(czyDziecko == 2){
                wybKrzesla=sharedNum->current_chair;
                if (chairs[wybKrzesla].count + 2 < SEAT_CAPACITY) {
                chairs[wybKrzesla].pids[chairs[wybKrzesla].count] = child_thread1;
                chairs[wybKrzesla].count++;
                chairs[wybKrzesla].pids[chairs[wybKrzesla].count] = child_thread2;
                chairs[wybKrzesla].count++;
                chairs[wybKrzesla].pids[chairs[wybKrzesla].count] = getpid();
                chairs[wybKrzesla].count++;
                set_color("\033[31m"); // Czerwony
                printf("Process %d wybrał krzeselko %d z dwojką dzieci\n", getpid(), wybKrzesla);
                signalSemafor(semID4, 0);
                //printf("chairs[%d].count = %d\n", wybKrzesla, chairs[wybKrzesla].count);
                waitSemafor(semID2, wybKrzesla, 0); // Czeka na przyjazd krzesla
                //printf("Process %d wszedl na krzeselko %d\n", getpid(), wybKrzesla);
                if (systemTime.tv_usec < DURATION*MINUTA){
                    signalSemafor(semID1,wybBramki);
                }
                // Znaleziono miejsce wyjscie z petli
                break;
            } else {
                //printf("Process %d couldn't find a seat\n", getpid());
                sharedNum->current_chair=(sharedNum->current_chair+1)%NUM_CHAIRS;
                signalSemafor(semID4, 0);
            }
            }
            else if(czyDziecko == 1){
                wybKrzesla=sharedNum->current_chair;
                if (chairs[wybKrzesla].count + 1< SEAT_CAPACITY) {
                chairs[wybKrzesla].pids[chairs[wybKrzesla].count] = child_thread1;
                chairs[wybKrzesla].count++;
                chairs[wybKrzesla].pids[chairs[wybKrzesla].count] = getpid();
                chairs[wybKrzesla].count++;
                set_color("\033[31m"); // Czerwony
                printf("Process %d wybrał krzeselko %d z jednym dzicekiem\n", getpid(), wybKrzesla);
                signalSemafor(semID4, 0);
                //printf("chairs[%d].count = %d\n", wybKrzesla, chairs[wybKrzesla].count);
                waitSemafor(semID2, wybKrzesla, 0); // Czeka na przyjazd krzesla
                //printf("Process %d wszedl na krzeselko %d\n", getpid(), wybKrzesla);
                if (systemTime.tv_usec < DURATION*MINUTA){
                    signalSemafor(semID1,wybBramki);
                }
                // Znaleziono miejsce wyjscie z petli
                break;
            } else {
                //printf("Process %d couldn't find a seat\n", getpid());
                sharedNum->current_chair=(sharedNum->current_chair+1)%NUM_CHAIRS;
                signalSemafor(semID4, 0);
            }
            // Sekcja bez dziecka
            }else if(czyDziecko == FALSE){
                wybKrzesla=sharedNum->current_chair;
                if (chairs[wybKrzesla].count < SEAT_CAPACITY) {
                    chairs[wybKrzesla].pids[chairs[wybKrzesla].count] = getpid();
                    chairs[wybKrzesla].count++;
                    set_color("\033[31m"); // Czerwony
                    printf("Process %d wybrał krzeselko %d\n", getpid(), wybKrzesla);
                    signalSemafor(semID4, 0);
                    //printf("chairs[%d].count = %d\n", wybKrzesla, chairs[wybKrzesla].count);
                    waitSemafor(semID2, wybKrzesla, 0); // Czeka na przyjazd krzesla
                    //printf("Process %d wszedl na krzeselko %d\n", getpid(), wybKrzesla);
                    gettimeofday(&systemTime,NULL);
                    if (systemTime.tv_usec < DURATION*MINUTA){
                        signalSemafor(semID1,wybBramki);
                    }
                    // Znaleziono miejsce wyjscie z petli
                    break;
                } else {
                    //printf("Process %d couldn't find a seat\n", getpid());
                    sharedNum->current_chair=(sharedNum->current_chair+1)%NUM_CHAIRS;
                    signalSemafor(semID4, 0);
                }

            }

        }

        // Czekanie na dojechanie na gore kolejki
        waitSemafor(semID3, wybKrzesla, 0);
        if(czyDziecko == 1){
            waitSemafor(semID3, wybKrzesla, 0);
        }
        
        if(czyDziecko == 2){
            waitSemafor(semID3, wybKrzesla, 0);
            waitSemafor(semID3, wybKrzesla, 0);
        }

        // =============================== Czesc zjazdowa =========================
        // pozwala zacząc zjeżdżać dziecku
        if(czyDziecko == 1){
            sem_post(&child_sem2);
        }
        if(czyDziecko == 2){
            sem_post(&child_sem2);
            sem_post(&child_sem4);
        }
        
        //printf("Process %d zszedl z krzeselka i zaczyna zjezdzac%d\n", getpid(), wybKrzesla);
        // Symulacja jazdy
        zjazd(dice(3));
        // Time update
        gettimeofday(&systemTime, NULL);
        timeNow = systemTime.tv_usec;
    }
    
    // =========== Koniec jazdy ================
    set_color("\033[31m");
    printf("Process %d konczy jazde\n", getpid());
    stop_thread = true;
    wyswietl_czas(STRT,timeNow/MINUTA);
    sem_destroy(&child_sem1);
    sem_destroy(&child_sem2);
    sem_destroy(&child_sem3);
    sem_destroy(&child_sem4);
    fclose(file);
    return 0;
}
/*
Be - bilet end
*/