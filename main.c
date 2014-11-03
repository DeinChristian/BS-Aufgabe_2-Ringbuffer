/* 
 * File:   main.c
 * Author: Louisa Spahl
 *
 * Created on 3. November 2014, 21:05
 */

#include <unistd.h> // provides access to the POSIX operating system API.
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// Konstanten
#define MAX 16 // Buffer-Memory Größe

// Thread Mutexe
pthread_mutex_t rb_mutex = PTHREAD_MUTEX_INITIALIZER;
//...
pthread_cond_t not_empty_condvar = PTHREAD_COND_INITIALIZER;
pthread_cond_t not_full_condvar = PTHREAD_COND_INITIALIZER;
//...

// Variablen
int thread_id[4] = {0, 1, 2, 3};

// Ringpuffer (rb) Struktur
typedef struct {
    int buffer[MAX];
    int *p_in;
    int *p_out;
    int count;
} rb;
rb x = {{0}, NULL, NULL, 0}; // Leerer Ringpuffer x
rb *p_rb = &x; // Pointer auf Ringpuffer x

#define p_start (int *)(p_rb -> buffer) // Pointer auf erstes Element im Buffer
#define p_end (int *)((p_rb -> buffer) + MAX-1) // Pointer auf letztes Element im Buffer

void* p_1_w(void *pid); // Prozess 1
void* p_2_w(void *pid); // Prozess 2
void* consumer(void *pid); // Consumer-Prozess
void* control(void *pid); // Control-Prozess

/**
 * Main
 * @param argc
 * @param argv
 * @return 
 */
int main(int argc, char** argv) {
    int i;
    pthread_t threads[4];
    
    printf("Start der Simulation! \n");
    
    p_rb -> p_in = p_start; // Setzen von p_in Pointer im Buffer auf p_start
    p_rb -> p_out = p_start; // Setzen von p_out Pointer im Buffer auf p_start
    p_rb -> count = 0; // Setzen von count im Buffer auf 0
    
    printf("Counter (im Buffer) ist %d. \n", p_rb -> count);
    
    // Threads starten
    pthread_create(&threads[0], NULL, start_routine(), )
    
    return (EXIT_SUCCESS);
}

/**
 * Temporäre Prozedur
 * @return 
 */
int start_routine() {
    //...
    return 0;
}