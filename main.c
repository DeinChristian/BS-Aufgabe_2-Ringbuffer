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
#include <sched.h>
#include <termios.h>

#include "main.h" // Header-Datei für Funktionen dieser Datei

// Fehlerbehandlung
#define handle_error(msg) do { perror(msg); exit(EXIT_FAILURE); } while (0)

// Konstanten
#define MAX 16 // Buffer-Memory Größe

// Thread Mutexe
pthread_mutex_t rb_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t not_empty_condvar = PTHREAD_COND_INITIALIZER;
pthread_cond_t not_full_condvar = PTHREAD_COND_INITIALIZER;

// Variablen
int thread_id[4] = {0, 1, 2, 3};
char lower_case_alphabet[] = "abcdefghijklmnopqrstuvwxyz"; //terminiert mit \0 also 27 Zeichen
char capital_alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"; 
int pos_in_alphabet = 0; //Position beider Alphabete
//int producer_1_start_stop;
//int producer_2_start_stop;
//int consumer_start_stop;

// Ringpuffer (rb) Struktur
typedef struct {
    char buffer[MAX];
    char *p_in;
    char *p_out;
    int count;
} rb;
rb x = {{0}, NULL, NULL, 0}; // Leerer Ringpuffer x
rb *p_rb = &x; // Pointer auf Ringpuffer x (liefert die Adresse der ersten Stelle?!)

#define p_start (char *)(p_rb->buffer) // Pointer auf erstes Element im Buffer
#define p_end (char *)((p_rb->buffer) + MAX-1) // Pointer auf letztes Element im Buffer

/**
 * Main
 * @param argc
 * @param argv
 * @return EXIT_SUCCESS
 */
int main (int argc, char** argv) { 
    printf("Start der Simulation! \n");
    
    pthread_t threads[4]; //Feld, welches vier Elemente enthält. z.B. wie thread_id[4] oben im Quellcode
    // Definition von Thread-Scheduling und Prioriät
    pthread_attr_t  my_thread_attr; 
    struct sched_param my_prio;
    pthread_attr_init(&my_thread_attr);
    pthread_attr_setinheritsched(&my_thread_attr, PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedpolicy(&my_thread_attr, SCHED_FIFO);
    my_prio.sched_priority = 32;
    pthread_attr_setschedparam(&my_thread_attr, &my_prio);
    
    
    p_rb->p_in = p_start; // Setzen von p_in Pointer im Buffer auf p_start. auch p_rb.p_in = p_start möglich siehe RMP 08
    p_rb->p_out = p_start; // Setzen von p_out Pointer im Buffer auf p_start
    p_rb->count = 0; // Setzen von count im Buffer auf 0
    
    printf("Counter (im Buffer) ist %d. \n", p_rb -> count);
    
    // Threads starten
    int thread0 = pthread_create(&threads[0], NULL, &p_1_w, (void *)thread_id);  //thread_id liefert die Adresse des ersten Feldes
    int thread1 = pthread_create(&threads[1], NULL, &p_2_w, (void *)&thread_id[1]);
    int thread2 = pthread_create(&threads[2], NULL, &consumer, (void *)&thread_id[2]);
    int thread3 = pthread_create(&threads[3], NULL, &control, (void *)&thread_id[3]);
    
    // Fehlermeldung, wenn das Erstellen eines der Threads fehlschlägt.
    if (thread0 != 0 || thread1 != 0 || thread2 != 0 || thread3 != 0) {
        handle_error("Fehler beim Erzeugen eines Threads.");
    } 
        
    // Threads joinen bzw. warten, bis alle Threads terminieren
    int i;
    for(i = 0; i < 4; i++) {
        if(pthread_join(threads[i], NULL) != 0) {
            handle_error("Fehler bei der Terminierung der Threads.");
        }
    }
    printf("Ende (nach dem Join der Threads). \n");
    
    return (EXIT_SUCCESS);
}


/**
 * Prozess 1
 * @param pid Prozess-ID
 * @return 
 */
void* p_1_w(void *pid) {    
    return write_c(pid, (char *)lower_case_alphabet);
}

/**
 * Prozess 2
 * @param pid Prozess-ID
 * @return NULL
 */
void* p_2_w(void *pid) {
   return write_c(pid, (char *)capital_alphabet);
}

/**
 * Consumer
 * @param pid Prozess-ID
 * @return NULL
 */
void* consumer(void *pid) {
    return read_rb(pid);
}

/**
 * Control
 * @param pid Prozess-ID
 * @return NULL
 */
void* control(void *pid) {
    char chari;
    do{
        chari = get_char();
        switch(chari)
        {
            case '1' : NULL; break;
            case '2' : NULL; break;
            case 'c' :
            case 'C' : NULL; break;
            case 'q' :
            case 'Q' : pthread_exit((int*)&thread_id[1]);
                       pthread_exit((int*)&thread_id[1]);
                       pthread_exit((int*)&thread_id[2]);
                       return (EXIT_SUCCESS);

            case 'h' : printf("\n Folgende Tastatureingaben sind erlaubt: \n ");
                       printf(" 1: Start bzw. Stop von Poducer_1 \n ");
                       printf(" 2: Start bzw. Stop von Poducer_2 \n");
                       printf("c/C: Start bzw. Stop von Consumer \n");  
                       printf("q/Q: Terminierung der Threads, sodass der Main_Thread das System beendet \n");
                       printf("  h: diese Liste von möglichen Eingaben lierfern \n"); break;
            default : ;
        }  
    } while(1);   
}

/**
 * Schreiben eines Charakters
 * @param pid Prozess-ID
 * @return NULL
 */
void* write_c(void *pid, char *alphabet) {
    int i = 0;
//    printf("Start: Schreiben von Prozess %d. \n", *(int*)pid);
    
    while(1) {
        i++;
        
        
        pthread_mutex_lock(&rb_mutex);
         
        // Prüfung, ob der Ringbuffer voll ist
        while(p_rb->p_in == p_rb->p_out && p_rb->count == MAX) {
//            printf("Ringpuffer voll! Warten ... \n");
            //Condition 'not_full_convar' ist eine Warteschlange, in die der Thread eingetragen wird.
            if(pthread_cond_wait(&not_full_condvar, &rb_mutex) != 0) {
                handle_error("Fehler beim Eintragen eines Threads in Condition-Warteschlange aufgetreten.");
            } 
//            printf("Prozess %d ist aufgewacht. Anzahl Zeichen im Ring: %d. \n", *(int*)pid, p_rb->count);            
        }
        
        if(alphabet[pos_in_alphabet] == '\0') { //ist das chararray (Alphabet) am Ende
            pos_in_alphabet = 0;                // dann setze es wieder auf den Anfang
        } 
        sleep(3);
        *(p_rb->p_in) = alphabet[pos_in_alphabet];  //p_rb->p_in ist Adresse *(p_rb->p_in) ist Inhalt der Adresse, und dieser wird auf z_var gesetzt
        (p_rb->p_in)++;     // die Adresse wird incrementiert -> hier um 4, weil int vier zeichen (chars ein zeichen)
        if(p_rb->p_in > p_end) {
            p_rb->p_in = p_start;
        }
        (p_rb->count)++; 
        pos_in_alphabet++; //inkementiere die Stelle des Pointers im chararray (Alphabet)
        if(p_rb->count != 0) {
//            printf("Buffer signalisiert, dass er nicht leer ist. \n");
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
    
//    printf("Start: Lesen von Prozess %d. \n", *(int*)pid);
    
    while(1) {
        i++;
        pthread_mutex_lock(&rb_mutex);
        
        // Prüfung, ob der Ringbuffer leer ist
        while(p_rb->count == 0) {
//            printf("Ringpuffer leer! Warten ... \n");
            pthread_cond_wait(&not_empty_condvar, &rb_mutex);
//            printf("Prozess %d ist aufgewacht. Anzahl Zeichen im Ring: %d. \n", *(int*)pid, p_rb->count);            
        }
        
        (p_rb->count)--;
        sleep(2); //schläft für 2 Sekunden bevor der Thread das zeichen ausgibt
        if(i%30 == 0){
            printf("%c \n", *(p_rb->p_out));
        }else{
            printf("%c", *(p_rb->p_out));
            fflush(stdout); // cursor im output bleibt auf gleicher Zeile
        }
        
        (p_rb->p_out)++;
        if(p_rb->p_out > p_end) {
            p_rb->p_out = p_start;
        }
        if(p_rb->count <= MAX) {
//            printf("Buffer signalisiert, dass er nicht voll ist. \n");
            pthread_cond_signal(&not_full_condvar);
        }
        pthread_mutex_unlock(&rb_mutex);
        sleep(1);
    }
    
    return (NULL);
}

/**
 * 
 * Danke TTT für die Hilfe mit der Char-Eingabe ohne Echo! :-)
 * @return char
 */
char get_char() {    
    static struct termios old, new;
    char result;
    
    tcgetattr(0, &old); // grab old terminal i/o settings
    new = old; // make new settings same as old settings
    new.c_lflag &= ~ICANON; // disable buffered i/o
    new.c_lflag &= ~ECHO; // set echo mode
    tcsetattr(0, TCSANOW, &new); // use these new terminal i/o settings now
    result = getchar();
    tcsetattr(0, TCSANOW, &old);
    
    return result;
}