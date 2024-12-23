//Library
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
//Files
#include "error.c"
#include "functions.h"
//Global variables
#define int chairNum = 80
#define int chairCap = 3
#define int chariAvaible = 40
#define int maxPeople = chairCap * chariAvaible

void ticket_buy(){
    /*
    karnet czasowy t1 t2 t3
    lub dzienny

    dzieci poniżej 12 roku życia
    i seniorzy
    mają zniżkę 25%

    poniżej 8 roku życia
    cały czas nadzór rodzica
    */
}

void lower_station(){
    /*
    wejście na peron
    czterema bramkami jednocześnie
    na peronie dolnej stacji może przebywać max N osób

    obsługuje pracownik1
    */
}

void higher_station(){
    /*
    wyjazd z peronu w dwie strony (jednokierunkowe)
    obsługuje pracownik2
    */
}

void emergency(){
    /*
    przypadek wypadku koleji/zagrożenie
    pracownik1/pracownik2 zatrzymuje kolej

    następnie pracownik który zatrzymał kolej
    komunikuje się z drugim
    po otrzymaniu gotowości włącza z powrotem kolej
    */
}

void ski(){
    /*
    3 trasy o różnym poziomie trudnośći
    t1 t2 i t3 gdzie t1<t2<t3
    */
}

void work_time(){
    /*
    tp-godzina początku/otwarcia
    tk-godzina końca

    zakończenie ma być:
    zamknięcie bramek
    transport wszystkich na górę
    odczekanie 5 min
    wyłączenie koleji
    */
}
/*
dzieci 4-8 lat wejście na krzesełko tylko z osobą dorosłą.
Każde przejście przez bramki (użycie danego karnetu) jest rejestrowane (id karnetu - godzina)
na koniec dnia jest generowany raport/podsumowanie ilości wykonanych zjazdów przez poszczególne osoby/karnety.
Osoby uprawnione VIP wchodzą na peron dolnej stacji bez kolejki (używając karnetu!);
*/

int main()
{

}