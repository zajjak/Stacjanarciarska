# Makefile

# Kompilatory i flagi
CC = gcc
CFLAGS = -Wall -g

# Pliki źródłowe
SRC_NARCIARZ = narciarz.c
SRC_PRACOWNIK = pracownik.c
SRC_KASJER = kasjer.c

# Pliki wykonywalne
BIN_NARCIARZ = narciarz
BIN_PRACOWNIK = pracownik
BIN_KASJER = kasjer

# Cele
all: $(BIN_NARCIARZ) $(BIN_PRACOWNIK) $(BIN_KASJER) run_kasjer

$(BIN_NARCIARZ): $(SRC_NARCIARZ)
	$(CC) $(CFLAGS) -o $@ $<

$(BIN_PRACOWNIK): $(SRC_PRACOWNIK)
	$(CC) $(CFLAGS) -o $@ $<

$(BIN_KASJER): $(SRC_KASJER)
	$(CC) $(CFLAGS) -o $@ $<

run_kasjer: $(BIN_KASJER)
	./$(BIN_KASJER)

clean:
	rm -f $(BIN_NARCIARZ) $(BIN_PRACOWNIK) $(BIN_KASJER)

.PHONY: all clean run_kasjer
