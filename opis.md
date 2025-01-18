Temat 16 – Stacja narciarska
Na terenie stacji narciarskiej znajduje się z krzesełkowa kolej linowa. Kolej składa się z 3 osobowych krzesełek o łącznej liczbie 80 sztuk. Jednocześnie może być zajętych 40 krzesełek na których siedzi maksymalnie 3x40 = 120 osób. Narciarze/turyści przychodzą na teren stacji w losowych momentach czasu (nie wszyscy z nich muszą jeździć na nartach). Wejście na teren kolejki linowej odbywa się po opłaceniu karnetu w kasie. Karnety są czasowe (Tk1, Tk2, Tk3) lub dzienne. Dzieci poniżej 12 roku życia oraz seniorzy powyżej 65 roku życia mają 25% zniżkę. Dzieci poniżej 8 roku życia znajdują się pod stałą opieką osoby dorosłej.
Wejście na peron dolnej stacji odbywa się czterema bramkami jednocześnie. Na peronie dolnej stacji może przebywać maksymalnie N osób. Wyjazd z peronu stacji górnej odbywa się dwoma drogami jednocześnie (ruch jednokierunkowy). Stacja dolna jest obsługiwana przez pracownika1, stacja górna jest obsługiwana przez pracownika2. W przypadku zagrożenia pracownik1 lub pracownik2 zatrzymują kolej linową (sygnał1). Aby wznowić działanie pracownik, który zatrzymał kolej komunikuje się z drugim pracownikiem – po otrzymaniu komunikatu zwrotnego o gotowości kolej jest uruchamiana ponownie (sygnał2).
Zjazd odbywa się trzema trasami o różnym stopniu trudności – średni czas przejazdu dla poszczególnych tras jest różny i wynosi odpowiednio T1, T2 i T3 (T1<T2<T3).
Zasady działania stacji ustalone przez kierownika są następujące:
•
Kolej linowa jest czynna w godzinach od Tp do Tk, W momencie osiągnięcia czasu Tk na bramkach przestają działać karnety. Wszystkie osoby, które weszły na peron mają zostać przetransportowane do stacji górnej. Następnie po 5 sekundach kolej ma zostać wyłączona.
•
Dzieci w wieku od 4 do 8 lat siadają na krzesełko pod opieką osoby dorosłej;
•
Osoba dorosła może opiekować się jednocześnie co najwyżej dwoma dziećmi w wieku od 4 do 8 lat;
•
Każde przejście przez bramki (użycie danego karnetu) jest rejestrowane (id karnetu - godzina) – na koniec dnia jest generowany raport/podsumowanie ilości wykonanych zjazdów przez poszczególne osoby/karnety.
•
Osoby uprawnione VIP wchodzą na peron dolnej stacji bez kolejki (używając karnetu!);

https://github.com/zajjak/Stacjanarciarska.git



Co zapytac:

Kolej linowa jest czynna w godzinach od Tp do Tk
Jak zrobić czas?
Ksjer = szef
wątek co liczy czas kontrola czasu czas systemowy czy nie został czas końcowy 
wyłączyć
dokończenie dzaiłania

od 9 rano do 16 popołudniu
skalownaie czasu 
Tk koniec symulacji

//Wyjazd z peronu stacji górnej odbywa się dwoma drogami jednocześnie (ruch jednokierunkowy).

Dzieci poniżej 8 roku życia znajdują się pod stałą opieką osoby dorosłej.
Osoba dorosła może opiekować się jednocześnie co najwyżej dwoma dziećmi w wieku od 4 do 8 lat;

proces główny dorosłego
wątek dziecko obniżanie

Wejście na teren kolejki linowej odbywa się po opłaceniu karnetu w kasie.

Kolejka liniowa
jako segment pamięci dzielonej
    krzesełka:
    podzielić pmięć na tyle częsci co krzesełek
    pidy w każdej części
    krzesełka są numerowana
    80 krzesełek 39 zajetych 39 wolnych 1 wysiada 1 wsiada
    semafory addres krzesełka przy upływie czasu
    jedno krzesełko jest na 3 osoby

/*
80 semaforów = krzesełko
na 3 osoby je zamknąć

4 bramki po 25
wejście blokuje
wyjście 
dziecko to to wątek

pracownik dolny
   podjeżdza id_krzesełka 
   sleep na 5s
   i to na co mają wejść

pracownik górny

semctl(entry_sem_id,LEFT_ENTRY_IDX,SETVAL,N/4);
