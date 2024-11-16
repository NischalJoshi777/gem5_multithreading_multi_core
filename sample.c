#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define N 2
#define NUM_THREADS 4

float x[N], y[N];
float a = 2.5;

struct ThreadArgs {
    int start;
    int end;
};

void *daxpy_thread(void *arg) {
    struct ThreadArgs *args = (struct ThreadArgs *)arg;
    int start = args->start;
    int end = args->end;

    for (int i = start; i < end; i++) {
        __asm__ __volatile__ (
            "movss (%0), %%xmm0\n\t"
            "mulss %2, %%xmm0\n\t"
            "addss (%1), %%xmm0\n\t"
            "movss %%xmm0, (%1)"
            :
            : "r"(&x[i]), "r"(&y[i]), "m"(a)
            : "xmm0", "memory"
        );
    }

    pthread_exit(NULL);
}

int main() {
    pthread_t threads[NUM_THREADS];
    struct ThreadArgs thread_args[NUM_THREADS];

    // Initialize x and y arrays
    for (int i = 0; i < N; i++) {
        x[i] = (float)i;
        y[i] = (float)(N - i);
    }

    // Create threads
    int chunk_size = N / NUM_THREADS;
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_args[i].start = i * chunk_size;
        thread_args[i].end = (i == NUM_THREADS - 1) ? N : (i + 1) * chunk_size;
        pthread_create(&threads[i], NULL, daxpy_thread, (void *)&thread_args[i]);
    }

    // Wait for threads to complete
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Daxpy computation completed.\n");

    return 0;
}