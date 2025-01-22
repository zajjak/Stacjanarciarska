CC = gcc
CFLAGS = -Wall -Wextra -std=c99
SRCS = kasjer.c pracownik.c narciarz.c

# Pliki nagłówkowe
HEADERS = kolejka.h
OBJS = $(SRCS:.c=.o)

# Pliki wykonywalne
TARGETS = kasjer pracownik narciarz

all: $(TARGETS)

kasjer: kasjer.o
	$(CC) $(CFLAGS) -o $@ $^

pracownik: pracownik.o
	$(CC) $(CFLAGS) -o $@ $^

narciarz: narciarz.o
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(OBJS) $(TARGETS)