/* 
 * File:   main.h
 * Author: Louisa Spahl
 *
 * Created on 4. November 2014, 14:10
 */

#ifndef MAIN_H
#define	MAIN_H

void* p_1_w(void *pid); // Prozess 1 //Thread
void* p_2_w(void *pid); // Prozess 2 //Thread
void* consumer(void *pid); // Consumer-Prozess //Thread
void* control(void *pid); // Control-Prozess //Thread

void* write_c(void *pid, char *alphabet, int *stop, pthread_cond_t *restart);
void* read_rb(void *pid);

char get_char();

#endif	/* MAIN_H */

