#ifndef _HEADER_
#define _HEADER_
#include "header.h"
#endif

void stampa_messaggio(msg_rqst msg);
int P(int semid, int semnum);
int V(int semid, int semnum);
int random_in_range (unsigned int min, unsigned int max);
short in_array(short n,short* a,short size);
stato get_stato(int x, int y, short unsigned* m);
void init_matrix(short unsigned* m,int* n_gen);
void  uccidi(int x,int y,short unsigned* m);
void fiat(int x,int y,short unsigned* m);
