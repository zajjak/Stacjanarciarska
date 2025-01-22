CC = gcc
SRCS = kasjer.c narciarz.c pracownik.c
TARGETS = kasjer narciarz pracownik

all: $(TARGETS)

narciarz:
	$(CC) narciarz.c -o narciarz

pracownik:
	$(CC) pracownik.c -o pracownik

kasjer:
	$(CC) kasjer.c -o kasjer

run: kasjer
	./kasjer

clean:
	rm -f $(TARGETS)

# Zawsze wykonuj kompilację
.PHONY: all kasjer narciarz pracownik clean
