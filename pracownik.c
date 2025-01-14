#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/msg.h>

#include "kolejka.h"

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <chair_num> <semID> <shmID>\n", argv[0]);
        exit(1);
    }

    int chair_num = atoi(argv[1]);
    int semID = atoi(argv[2]);
    int shmID = atoi(argv[3]);

    // Przyłączenie segmentu pamięci dzielonej
    Chair *chairs = (Chair *)shmat(shmID, NULL, 0);
    if (chairs == (void *)-1) {
        perror("Blad shmat");
        exit(1);
    }

    while (true) {
        // Symulacja ruchu krzesełka
        usleep(10 * MINUTA);

        // Sprawdzenie, czy krzesełko jest zajęte
        if (chairs[chair_num].is_occupied) {
            // Wysiadanie narciarzy
            for (int i = 0; i < NUM_RESOURCES; i++) {
                if (chairs[chair_num].pid[i] != 0) {
                    printf("Narciarz %d wysiada z krzesełka %d\n", chairs[chair_num].pid[i], chair_num);
                    chairs[chair_num].pid[i] = 0;
                }
            }
            chairs[chair_num].is_occupied = 0;

            // Operacja V (signal) na semaforze kontrolującym krzesełko
            if (signalSemafor(semID, chair_num) == -1) {
                exit(1);
            }
        }
    }

    // Odłączenie segmentu pamięci dzielonej
    if (shmdt(chairs) == -1) {
        perror("Blad shmdt");
        exit(1);
    }

    return 0;
}