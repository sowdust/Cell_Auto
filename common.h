#ifndef _HEADER_
#define _HEADER_
#include "header.h"
#endif

/* 
	gestione semafori
*/
int		P(int, int );		// signal
int		V(int, int);		// wait
int		Z(int, int);		// wait 4 0
int		R(int, int, int);	// reset

/*
	libreria per migliore pseudo casualita'
	in intervallo aperto [a,b)
*/
int		random_in_range (unsigned int, unsigned int);


short		in_array(short, short*, short);

/*
	inizializza matrice in modo casuale
*/
void		clean_matrix(short unsigned*,int*);
void		init_matrix(short unsigned*,int*);
void copy_matrix(short unsigned*, short unsigned*, int*);

/*
	imposta a 1 (vivo) o 0 (morto) lo stato di una cella
	simulando due dimensioni su un vettore unidimensionale
*/
void		uccidi(int,int,short unsigned*);
void		fiat(int,int,short unsigned*);

/*	
	ritorna lo stato della cella (VIVO se 1, MORTO se 0)
	simulando due dimensioni su un vettore unidimensionale
*/
stato		get_stato(int, int, short unsigned*);

void		termina_proc(lista,int);
