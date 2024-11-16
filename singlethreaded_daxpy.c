#include <stdio.h>

void daxpy(int n, double a, double *x, double *y) {
    for (int i = 0; i < n; i++) {
        y[i] = a * x[i] + y[i];
    }
}

int main() {
    int n = 50;  // Size of the vectors
    double a = 2.5;  // Scaling factor

    double x[n];
    double y[n];

    // Initialize the arrays
    for (int i = 0; i < n; i++) {
        x[i] = i + 1.0;  // Example: x = {1.0, 2.0, 3.0, ..., 100.0}
        y[i] = (i + 1) * 10.0;  // Example: y = {10.0, 20.0, 30.0, ..., 1000.0}
    }

    // Perform the DAXPY operation
    daxpy(n, a, x, y);

    // Print the result
    printf("Resulting Y vector:\n");
    for (int i = 0; i < n; i++) {
        printf("%f\n", y[i]);
    }

    return 0;
}
