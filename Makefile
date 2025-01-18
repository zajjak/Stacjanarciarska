# Kompilator i flagi kompilacji
CC = gcc
CFLAGS = -Wall -g

# Pliki źródłowe
SOURCES = kasjer.c narciarz.c pracownik.c

# Nazwy plików wykonywalnych
EXEC_KASJER = kasjer
EXEC_NARCIARZ = narciarz
EXEC_PRACOWNIK = pracownik

# Reguła domyślna - kompiluje wszystkie pliki wykonywalne
all: $(EXEC_KASJER) $(EXEC_NARCIARZ) $(EXEC_PRACOWNIK)

# Reguła do kompilacji kasjer.c
$(EXEC_KASJER): kasjer.c
	$(CC) $(CFLAGS) kasjer.c -o $(EXEC_KASJER)

# Reguła do kompilacji narciarz.c
$(EXEC_NARCIARZ): narciarz.c
	$(CC) $(CFLAGS) narciarz.c -o $(EXEC_NARCIARZ)

# Reguła do kompilacji pracownik.c
$(EXEC_PRACOWNIK): pracownik.c
	$(CC) $(CFLAGS) pracownik.c -o $(EXEC_PRACOWNIK)

# Reguła do czyszczenia plików wykonywalnych
clean:
	rm -f $(EXEC_KASJER) $(EXEC_NARCIARZ) $(EXEC_PRACOWNIK)

# Reguła do uruchamiania programu kasjer
run: $(EXEC_KASJER)
	./$(EXEC_KASJER)