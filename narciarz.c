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

#include "kolejka.h"

pthread_t child_thread;

// Funkcja obsługi sygnału SIGTERM
void handle_sigterm(int signum) {
    //printf("Otrzymano sygnał SIGTERM, zakończenie programu\n");
    if (child_thread != 0) {
        // Czekanie na zakończenie wątku
        pthread_join(child_thread, NULL);
    }
    exit(0);
}

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
        //printf("Kupuje bilet na 3 godziny");
        Bt = Bt + 180*MINUTA;
    }
    else if(wybor == 3){
        //printf("Kupuje bilet na 6 godziny");
        Bt = Bt + 360*MINUTA;
    }
    else if(wybor == 4){
        //printf("Kupuje bilet na caly dzien");
        Bt = DURATION*MINUTA;
    }
    return Bt;
}
/*
Bt = czas dzialania biletu
*/
// Funkcja symulujaca zjazd osoby doroslej
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
// Funkcja symulujaca zjazd osoby niepelnoletniej (wolniej od osoby doroslej)
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

// Funkcja watku dizcka
void* child_thread_function(void* arg) {
    zjazdDzieci(dice(3));
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

    long Be = systemTime.tv_usec + ticketBuy();// bilet end
    long timeNow=systemTime.tv_usec;

    FILE *file = fopen("raport.txt", "a");
    if (file == NULL) {
        perror("Blad otwarcia pliku raport.txt");
        exit(1);
    }

    // =============================Semafors and Sherememory==============================
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
    
    // ========= Czy VIP ============
    bool czyVIP = FALSE;
    if(dice(25)==1){ // losowanie, szansa 1/25
        // printf("witamy Vipa %d\n",getpid());
        czyVIP = TRUE;
    }

    // =========== Czy ma dziecko =============
    bool czyDziecko = FALSE;
    if(dice(20)==1){ // losowanie, szansa 1/20
        //printf("narciarz z dzieckiem %d\n",getpid());
        int iloscLat= dice(15) + 3;
        if(iloscLat<12){
            // printf("znizka ulogwa przy kupowaniu biletu\n");
        }
        czyDziecko = TRUE;
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
        if(czyDziecko==TRUE){
            waitSemafor(semID1, wybBramki, 0);
        }
            
        // Przejscie przez bramke
        usleep(10*SEKUNDA); 
        gettimeofday(&systemTime,NULL);
        
        long calkowite_minuty = STRT + systemTime.tv_usec/MINUTA;
        long godziny = calkowite_minuty / 60;
        long minuty = calkowite_minuty % 60;

        fprintf(file,"Czas: %02ld:%02ld\n", godziny, minuty);
        fprintf(file, "Process %d przeszedl przez bramke %d\n", getpid(), wybBramki);
        if (czyDziecko == TRUE) {
            fprintf(file, "z dzieckiem\n");
        }

        // --------------- Wybor krzesla ---------------
        while(1){
            // Sekacja krytyczna wyboru krzeselka
            waitSemafor(semID4, 0, 0);
            //printf("current_chair = %d\n", sharedNum->current_chair);

            // Sekcja z dzickiem
            if(czyDziecko == TRUE){
                wybKrzesla=sharedNum->current_chair;
                if (chairs[wybKrzesla].count + 1< SEAT_CAPACITY) {
                chairs[wybKrzesla].pids[chairs[wybKrzesla].count] = getpid();
                chairs[wybKrzesla].count++;
                chairs[wybKrzesla].pids[chairs[wybKrzesla].count] = getpid();
                chairs[wybKrzesla].count++;
                printf("    Process %d wybrał krzeselko z dzicekiem %d\n", getpid(), wybKrzesla);
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
                    printf("    Process %d wybrał krzeselko %d\n", getpid(), wybKrzesla);
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

        // =============================== Czesc zjazdowa =========================
        // pozwala zacząc zjeżdżać dziecku
        if(czyDziecko==TRUE){
            child_thread = getpid();
            pthread_create(&child_thread, NULL, child_thread_function, &child_thread);
        }
        
        //printf("Process %d zszedl z krzeselka i zaczyna zjezdzac%d\n", getpid(), wybKrzesla);
        // Symulacja jazdy
        zjazd(dice(3));

        if(czyDziecko==TRUE){
            // czeka aż dziecko dojedzie
            pthread_join(child_thread, NULL);
        }

        // Time update
        gettimeofday(&systemTime, NULL);
        timeNow = systemTime.tv_usec;
    }
    
    // =========== Koniec jazdy ================
    printf("Process %d konczy jazde\n", getpid());
    if(czyDziecko == TRUE){
        //Czekanie na zakończenie wątku 
        pthread_join(child_thread, NULL);
    }
    // wyswietl_czas(STRT,timeNow/MINUTA);
    fclose(file);
    return 0;
}
/*
Be - bilet end
*/