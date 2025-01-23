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


#define NUM_RESOURCES 10 // ilość miejsca za każdą bramką
#define N 4 // liczba bramek na dole stacji

#define TRUE 1
#define FALSE 0

#define MINUTA 1000 // czas jednej minuty w usleep
#define SEKUNDA 17 // czas jednej sekundy w usleep
#define STRT 480 // czas startu symulacji 00:00 + <STRT> minut
#define DURATION 360 // Czas trwania symulacji w minutach

#define P 200 // ilosc narciarzy
#define SEAT_CAPACITY 3 // ilość miejsc na krześle
#define NUM_CHAIRS 80 // ilość krzeselek

/*
   @brief Chair shareMemory

   Struktura krzesełka

   @param pids[SEAT_CAPICITY] przechowuje pidy procesów które wsiadły na krzesełko.
   @param timeTop to czas symulacji w którym krzesełko dojedzie na górę.
   @param count to liczba osób na krzesełku.
*/
struct Chair {
    long pids[SEAT_CAPACITY];
    long timeTop;
    int count;
};

/*
   @brief SharedNum shareMemory

   struktura do przydzialania numeru krzesła.

   @param current_chair przechowuje najbliższy numer krzesełka na który można wsiąść.
*/
struct SharedNum{
    int current_chair;
};

/*
   @brief Funkcja losująca liczbę

   Ta funkcja przyjmuje wartość całkowitą n i zwraca
   pseudo losową liczbę z przedziału 1-n. Działa jak
   kostka do gry.

   @param n liczba całkowita do której będzie losowane.

   @return Liczba całkowita z przedziału 1-n.
*/
int dice(int n)
{
    return rand() % n + 1;
}

/*
   @brief Funkcja wyświetlająca czas symulacyjny

   Funkcja wyświetla czas symulacji w formacie godzin : minut.

   @param stala_minuty liczba minut początkowych, ile minut od północy.
   @param dodane_minuty liczba minut upłyniętych od początkowych.
*/
void wyswietl_czas(long stala_minuty, long dodane_minuty) {
    long calkowite_minuty = stala_minuty + dodane_minuty;
    long godziny = calkowite_minuty / 60;
    long minuty = calkowite_minuty % 60;

    printf("Czas: %02ld:%02ld\n", godziny, minuty);
}

/*
   @brief Ustawia kolor tekstu w terminalu

   Funkcja ustawia kolor tekstu na podstawie podanego kodu koloru.

   @param color_code Kod koloru w formacie ANSI.
*/
void set_color(const char* color_code) {
    printf("%s", color_code);
}

/*
   @brief Resetuje kolor tekstu w terminalu

   Funkcja przywraca domyślny kolor tekstu w terminalu.
*/
void reset_color() {
    printf("\033[0m");
}

/*
   @brief Alokuje semafor

   Funkcja tworzy nowy zestaw semaforów na podstawie podanego klucza.

   @param klucz Klucz identyfikujący zestaw semaforów.
   @param number Liczba semaforów w zestawie.
   @param flagi Flagi przy tworzeniu semaforów.

   @return Identyfikator alokowanego semafora.
*/
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

/*
   @brief Zwalnia semafor

   Funkcja usuwa zestaw semaforów na podstawie ich identyfikatora.

   @param semID Identyfikator semafora do usunięcia.
   @param number Numer semafora do usunięcia.

   @return Status operacji (0 w przypadku powodzenia, -1 w przypadku błędu).
*/
int zwolnijSemafor(int semID, int number)
{
   return semctl(semID, number, IPC_RMID, NULL);
}

/*
   @brief Inicjalizuje semafor

   Funkcja ustawia początkową wartość semafora.

   @param semID Identyfikator zestawu semaforów.
   @param number Numer semafora do inicjalizacji.
   @param val Wartość początkowa semafora.
*/
void inicjalizujSemafor(int semID, int number, int val)
{
   if ( semctl(semID, number, SETVAL, val) == -1 )
   {
      perror("Blad semctl (inicjalizujSemafor): ");
      exit(1);
   }
}

/*
   @brief Zmniejsza wartość semafora (operacja wait)

   Funkcja zmniejsza wartość semafora o 1, czekając, jeśli wartość jest równa 0.

   @param semID Identyfikator zestawu semaforów.
   @param number Numer semafora.
   @param flags Flagi operacji semafora.

   @return 1 w przypadku powodzenia, -1 w przypadku błędu.
*/
int waitSemafor(int semID, int number, int flags)
{
   struct sembuf operacje[1];
   operacje[0].sem_num = number;
   operacje[0].sem_op = -1;
   operacje[0].sem_flg = 0 | flags;

   if ( semop(semID, operacje, 1) == -1 )
   {
      return -1;
   }

   return 1;
}

/*
   @brief Zwiększa wartość semafora (operacja signal)

   Funkcja zwiększa wartość semafora o 1.

   @param semID Identyfikator zestawu semaforów.
   @param number Numer semafora.
*/
void signalSemafor(int semID, int number)
{
   struct sembuf operacje[1];
   operacje[0].sem_num = number;
   operacje[0].sem_op = 1;

   if (semop(semID, operacje, 1) == -1 )
      perror("Blad semop (postSemafor): ");
}

/*
   @brief Pobiera wartość semafora

   Funkcja zwraca bieżącą wartość semafora.

   @param semID Identyfikator zestawu semaforów.
   @param number Numer semafora.

   @return Wartość semafora.
*/
int valueSemafor(int semID, int number)
{
   return semctl(semID, number, GETVAL, NULL);
}
