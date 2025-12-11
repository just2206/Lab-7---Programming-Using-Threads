#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

// Define MAX as 20 for a 20x20 matrix as per the prompt
#define MAX 20

// Global matrices
int matA[MAX][MAX]; 
int matB[MAX][MAX]; 

int matSumResult[MAX][MAX];
int matDiffResult[MAX][MAX]; 
int matProductResult[MAX][MAX]; 

// Structure to pass arguments to thread functions
typedef struct {
    int thread_id; // For simplicity, this will be the starting row index
    int rows_per_thread;
} ThreadArgs;

void fillMatrix(int matrix[MAX][MAX]) {
    for(int i = 0; i<MAX; i++) {
        for(int j = 0; j<MAX; j++) {
            // Use MAX for the random range for slightly larger numbers
            matrix[i][j] = rand() % 50 + 1; 
        }
    }
}

void printMatrix(int matrix[MAX][MAX]) {
    for(int i = 0; i<MAX; i++) {
        for(int j = 0; j<MAX; j++) {
            printf("%5d", matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

// Fetches the appropriate coordinates from the argument, and sets
// the cell of matSumResult at the coordinates to the sum of the
// values at the coordinates of matA and matB.
void* computeSum(void* args) { 
    ThreadArgs* data = (ThreadArgs*)args;
    
    // Calculate the start and end row for this thread
    int start_row = data->thread_id;
    int end_row = start_row + data->rows_per_thread;
    
    for (int i = start_row; i < end_row; i++) {
        for (int j = 0; j < MAX; j++) {
            matSumResult[i][j] = matA[i][j] + matB[i][j];
        }
    }

    free(data); // Free the dynamically allocated argument structure
    return NULL;
}

// Fetches the appropriate coordinates from the argument, and sets
// the cell of matDiffResult at the coordinates to the difference of the
// values at the coordinates of matA and matB.
void* computeDiff(void* args) { 
    ThreadArgs* data = (ThreadArgs*)args;
    
    // Calculate the start and end row for this thread
    int start_row = data->thread_id;
    int end_row = start_row + data->rows_per_thread;
    
    for (int i = start_row; i < end_row; i++) {
        for (int j = 0; j < MAX; j++) {
            matDiffResult[i][j] = matA[i][j] - matB[i][j];
        }
    }

    free(data); // Free the dynamically allocated argument structure
    return NULL;
}

// Fetches the appropriate coordinates from the argument, and sets
// the cell of matProductResult at the coordinates to the inner product
// of matA and matB.
void* computeProduct(void* args) { 
    ThreadArgs* data = (ThreadArgs*)args;
    
    // Calculate the start and end row for this thread
    int start_row = data->thread_id;
    int end_row = start_row + data->rows_per_thread;
    
    // Matrix Multiplication: matProductResult[i][j] = Sum(matA[i][k] * matB[k][j])
    for (int i = start_row; i < end_row; i++) {
        for (int j = 0; j < MAX; j++) {
            matProductResult[i][j] = 0; // Initialize sum for cell
            for (int k = 0; k < MAX; k++) {
                matProductResult[i][j] += matA[i][k] * matB[k][j];
            }
        }
    }

    free(data); // Free the dynamically allocated argument structure
    return NULL;
}

// Spawn a thread to fill a portion of each result matrix.
// For a 20x20 matrix with 10 threads, each thread handles 2 rows.
int main() {
    srand(time(0));  // Do Not Remove. Just ignore and continue below.
    
    // The matrix size (MAX) is now defined globally as 20.
    const int NUM_THREADS = 10;
    const int ROWS_PER_THREAD = MAX / NUM_THREADS; // 20 / 10 = 2 rows per thread

    // 1. Fill the matrices (matA and matB) with random values.
    fillMatrix(matA);
    fillMatrix(matB);
    
    // 2. Print the initial matrices.
    printf("Matrix Size: %dx%d\n", MAX, MAX);
    printf("Number of Threads per Operation: %d\n\n", NUM_THREADS);
    printf("Matrix A (First 5x5 preview):\n");
    // printMatrix(matA); // Commented out to reduce output, but can be uncommented
    for(int i = 0; i<5; i++) { for(int j = 0; j<5; j++) { printf("%5d", matA[i][j]); } printf(" ...\n"); } printf("\n");
    
    printf("Matrix B (First 5x5 preview):\n");
    // printMatrix(matB); // Commented out to reduce output, but can be uncommented
    for(int i = 0; i<5; i++) { for(int j = 0; j<5; j++) { printf("%5d", matB[i][j]); } printf(" ...\n"); } printf("\n");
    
    // 3. Create pthread_t objects for our threads.
    pthread_t sum_threads[NUM_THREADS];
    pthread_t diff_threads[NUM_THREADS];
    pthread_t product_threads[NUM_THREADS];
    
    // --- 4. Create threads for all operations ---
    for (int i = 0; i < NUM_THREADS; i++) {
        // Calculate the starting row index for this thread (0, 2, 4, 6, ...)
        int start_row = i * ROWS_PER_THREAD;
        
        // --- Setup Arguments ---
        // Need three separate mallocs for the three different operations, 
        // as each thread function will free its own argument structure.
        ThreadArgs *sum_args = (ThreadArgs*)malloc(sizeof(ThreadArgs));
        sum_args->thread_id = start_row;
        sum_args->rows_per_thread = ROWS_PER_THREAD;

        ThreadArgs *diff_args = (ThreadArgs*)malloc(sizeof(ThreadArgs));
        diff_args->thread_id = start_row;
        diff_args->rows_per_thread = ROWS_PER_THREAD;

        ThreadArgs *prod_args = (ThreadArgs*)malloc(sizeof(ThreadArgs));
        prod_args->thread_id = start_row;
        prod_args->rows_per_thread = ROWS_PER_THREAD;
        
        // --- Create Threads ---
        pthread_create(&sum_threads[i], NULL, computeSum, sum_args);
        pthread_create(&diff_threads[i], NULL, computeDiff, diff_args);
        pthread_create(&product_threads[i], NULL, computeProduct, prod_args);
    }
    
    // --- 5. Wait for all threads to finish. ---
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(sum_threads[i], NULL);
        pthread_join(diff_threads[i], NULL);
        pthread_join(product_threads[i], NULL);
    }
    
    // 6. Print the results.
    printf("--- Results (First 5x5 preview) ---\n");
    
    printf("Sum (matA + matB):\n");
    // printMatrix(matSumResult); // Commented out to reduce output
    for(int i = 0; i<5; i++) { for(int j = 0; j<5; j++) { printf("%5d", matSumResult[i][j]); } printf(" ...\n"); } printf("\n");
    
    printf("Difference (matA - matB):\n");
    // printMatrix(matDiffResult); // Commented out to reduce output
    for(int i = 0; i<5; i++) { for(int j = 0; j<5; j++) { printf("%5d", matDiffResult[i][j]); } printf(" ...\n"); } printf("\n");
    
    printf("Product (matA * matB):\n");
    // printMatrix(matProductResult); // Commented out to reduce output
    for(int i = 0; i<5; i++) { for(int j = 0; j<5; j++) { printf("%5d", matProductResult[i][j]); } printf(" ...\n"); } printf("\n");
    
    return 0;
}