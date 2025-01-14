#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define NUM_THREADS 5
#define NUM_RESOURCES 2

sem_t sem; // Deklaracja semafora

void* thread_function(void* arg) {
    int thread_num = *(int*)arg;
    free(arg);

    printf("Thread %d waiting to enter critical section.\n", thread_num);
    sem_wait(&sem); // Zmniejszenie semafora, czekanie na zasób
    printf("Thread %d entered critical section.\n", thread_num);

    // Symulacja pracy w sekcji krytycznej
    sleep(2);

    printf("Thread %d leaving critical section.\n", thread_num);
    sem_post(&sem); // Zwiększenie semafora, zwolnienie zasobu

    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];

    // Inicjalizacja semafora z wartością równą liczbie zasobów
    if (sem_init(&sem, 0, NUM_RESOURCES) != 0) {
        perror("Semaphore initialization failed");
        exit(EXIT_FAILURE);
    }

    // Tworzenie wątków
    for (int i = 0; i < NUM_THREADS; i++) {
        int* thread_num = malloc(sizeof(int));
        *thread_num = i + 1;
        if (pthread_create(&threads[i], NULL, thread_function, thread_num) != 0) {
            perror("Thread creation failed");
            exit(EXIT_FAILURE);
        }
    }

    // Oczekiwanie na zakończenie wątków
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }ads have finished.\n");

    // Zniszczenie semafora
    sem_destroy(&sem);

    printf("All thre
    return 0;
}
