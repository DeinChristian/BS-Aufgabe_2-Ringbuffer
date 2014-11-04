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

#include "main.h" // Header-Datei für Funktionen dieser Datei

// Fehlerbehandlung
#define handle_error(msg) do { perror(msg); exit(EXIT_FAILURE); } while (0)

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

#define p_start (int *)(p_rb->buffer) // Pointer auf erstes Element im Buffer
#define p_end (int *)((p_rb->buffer) + MAX-1) // Pointer auf letztes Element im Buffer

void* p_1_w(void *pid); // Prozess 1
void* p_2_w(void *pid); // Prozess 2
void* consumer(void *pid); // Consumer-Prozess
void* control(void *pid); // Control-Prozess

/**
 * Main
 * @param argc
 * @param argv
 * @return EXIT_SUCCESS
 */
int main(int argc, char** argv) {
    pthread_t threads[4];
    
    printf("Start der Simulation! \n");
    
    //...
    
    p_rb->p_in = p_start; // Setzen von p_in Pointer im Buffer auf p_start
    p_rb->p_out = p_start; // Setzen von p_out Pointer im Buffer auf p_start
    p_rb->count = 0; // Setzen von count im Buffer auf 0
    
    printf("Counter (im Buffer) ist %d. \n", p_rb -> count);
    
    // Threads starten
    int thread0 = pthread_create(&threads[0], NULL, &thread_start, (void *)thread_id);
    int thread1 = pthread_create(&threads[1], NULL, &thread_start, (void *)&thread_id[1]);
    int thread2 = pthread_create(&threads[2], NULL, &thread_start, (void *)&thread_id[2]);
    
    // Fehlermeldung, wenn das Erstellen eines der Threads fehlschlägt.
    if (thread0 != 0 || thread1 != 0 || thread2 != 0) {
        handle_error("Fehler beim Erzeugen eines Threads");
    } 
    
    //...
    
    // Threads joinen bzw. warten, bis alle Threads terminieren
    int i;
    for(i = 0; i < 4; i++) {
        pthread_join(threads[i], NULL);
    }
    printf("Ende (nach dem Join der Threads). \n");
    
    return (EXIT_SUCCESS);
}

/**
 * Schreiben
 * @param pid Prozess-ID
 * @return NULL
 */
void* write_c(void *pid) {
    int i = 0;
    int z_var = 0;
    
    printf("Start: Schreiben von Prozess %d: \n", *(int*)pid);
    
    while(1) {
        i++;
        z_var++;
        
        if(z_var > 9) {
            z_var = 0;
        }        
        pthread_mutex_lock(&rb_mutex);
        
        // Prüfung, ob der Ringbuffer voll ist
        while(p_rb->p_in == p_rb->p_out && p_rb->count == MAX) {
            printf("Ringpuffer voll! Warten ... \n");
            pthread_cond_wait(&not_full_condvar, &rb_mutex);
            printf("Prozess %d ist aufgewacht. Count: %d \n", *(int*)pid, p_rb->count);            
        }
        
        *(p_rb->p_in) = z_var;
        (p_rb->p_in)++;
        if(p_rb->p_in > p_end) {
            p_rb->p_in = p_start;
        }
        (p_rb->count)++;
        if(p_rb->count != 0) {
            printf("Buffer signalisiert, dass er nicht leer ist. \n");
            pthread_cond_signal(&not_empty_condvar);            
        }
        
        pthread_mutex_unlock(&rb_mutex);
        sleep(1);
    }
    return (NULL);
}

/**
 * Lesen
 * @param pid
 * @return NULL
 */
void* read_rb(void *pid) {
    int i = 0;
    
    printf("Start: Lesen von Prozess %d: \n", *(int*)pid);
    
    while(1) {
        i++;
        pthread_mutex_lock(&rb_mutex);
        
        // Prüfung, ob der Ringbuffer leer ist
        while(p_rb->count == 0) {
            printf("Ringpuffer leer! Warten ... \n");
            pthread_cond_wait(&not_empty_condvar, &rb_mutex);
            printf("Prozess %d ist aufgewacht. Count: %d \n", *(int*)pid, p_rb->count);            
        }
        
        (p_rb->count)--;
        printf("Älteres Zeichen ausgegeben: %d: \n", *(p_rb->p_out));
        (p_rb->p_out)++;
        if(p_rb->p_out > p_end) {
            p_rb->p_out = p_start;
        }
        if(p_rb->count <= MAX) {
            printf("Buffer signalisiert, dass er nicht voll ist. \n");
            pthread_cond_signal(&not_full_condvar);
        }
        pthread_mutex_unlock(&rb_mutex);
        sleep(1);
    }
    
    return (NULL);
}

static void * thread_start(void *arg) {
    return (NULL);
}