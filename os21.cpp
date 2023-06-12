#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#define N 5 

typedef enum { THINKING, HUNGRY, EATING } State;

State state[N];
sem_t mutex;
sem_t phil_sem[N];

int left(int i) {
    return i;
}

int right(int i) {
    return (i + 1) % N;
}

void test(int i) {
    if (state[i] == HUNGRY && state[left(i)] != EATING && state[right(i)] != EATING) {
        state[i] = EATING;
        sem_post(&phil_sem[i]);
    }
}

void take_forks(int i) {
    sem_wait(&mutex);
    state[i] = HUNGRY;
    test(i);
    sem_post(&mutex);
    sem_wait(&phil_sem[i]);
}

void put_forks(int i) {
    sem_wait(&mutex);
    state[i] = THINKING;
    test(left(i));
    test(right(i));
    sem_post(&mutex);
}

void* philosopher(void* arg) {
    int phil_id = *(int*)arg;
    int eating_count = 0;

    while (eating_count < 3) {  
        printf("Philosopher %d is thinking.\n", phil_id);
        sleep(1);
        take_forks(phil_id);
        printf("Philosopher %d is eating.\n", phil_id);
        sleep(1);
        eating_count++;
        put_forks(phil_id);
    }
    printf("Philosopher %d has finished eating.\n", phil_id);
    return NULL;
}

int main() {
    pthread_t philosophers[N];
    int phil_ids[N];
    sem_init(&mutex, 0, 1);
    for (int i = 0; i < N; i++) {
        sem_init(&phil_sem[i], 0, 0);
    }
    for (int i = 0; i < N; i++) {
        phil_ids[i] = i;
        pthread_create(&philosophers[i], NULL, philosopher, &phil_ids[i]);
    }
    for (int i = 0; i < N; i++) {
        pthread_join(philosophers[i], NULL);
    }
    sem_destroy(&mutex);
    for (int i = 0; i < N; i++) {
        sem_destroy(&phil_sem[i]);
    }
    return 0;
}
