#include <stdio.h>
#include <pthread.h>
#define N 50
#define NUM_THREADS 4

// Struct to hold arguments for each thread
typedef struct {
    int start;
    int end;
    float a;
    const float *x;
    float *y;
} ThreadData;

// Function to perform a daxpy operation: y = a * x + y
void *daxpy(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    for (int i = data->start; i < data->end; i++) {
        data->y[i] = data->a * data->x[i] + data->y[i];
    }
    return NULL;
}

int main() {
    float a = 2.5f;
    float x[N];
    float y[N];
    pthread_t threads[NUM_THREADS];
    ThreadData threadData[NUM_THREADS];
    int chunkSize = N / NUM_THREADS;

    // Initialize x and y
    for (int i = 0; i < N; i++) {
        x[i] = 1.0f;
        y[i] = 2.0f;
    }
    // Create threads
    for (int i = 0; i < NUM_THREADS; i++) {
        threadData[i].start = i * chunkSize;
        threadData[i].end = (i == NUM_THREADS - 1) ? N : (i + 1) * chunkSize;
        threadData[i].a = a;
        threadData[i].x = x;
        threadData[i].y = y;

        if (pthread_create(&threads[i], NULL, daxpy, &threadData[i]) != 0) {
            perror("Failed to create thread");
            return 1;
        }
    }

    // Join threads
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    // Print the result of the DAXPY operation
    printf("DAXPY result:\n");
    printf("y = [ ");
    for (int i = 0; i < N; i++) {
        printf("%f ", y[i]);
    }
    printf("]\n");

    printf("DAXPY operation completed.\n");

    return 0;
}