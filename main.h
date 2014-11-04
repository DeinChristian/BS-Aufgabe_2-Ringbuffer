/* 
 * File:   main.h
 * Author: Louisa Spahl
 *
 * Created on 4. November 2014, 14:10
 */

#ifndef MAIN_H
#define	MAIN_H

void* write_c(void *pid);
void* read_rb(void *pid);

static void * thread_start(void *arg);

#endif	/* MAIN_H */

