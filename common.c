#ifndef _HEADER_
#define _HEADER_
#include "header.h"
#endif

int sem_id;

void stampa_messaggio(msg_rqst msg) {
	printf("[pid] %d [richiesta] %d\n",msg.p,msg.r);
}

int P(int semid, int semnum) {
		struct sembuf cmd;
		cmd.sem_num = semnum;
		cmd.sem_op = -1;
		cmd.sem_flg = 0;
		semop(semid, &cmd, 1);
		return 1;
}


int V(int semid, int semnum) {
		struct sembuf cmd;
		cmd.sem_num = semnum;
		cmd.sem_op = 1;
		cmd.sem_flg = 0;
		semop(semid, &cmd, 1);
		return 0;
}

/* Would like a semi-open interval [min, max) */
int random_in_range (unsigned int min, unsigned int max)
{
	int base_random = rand(); /* in [0, RAND_MAX] */
	if (RAND_MAX == base_random) return random_in_range(min, max);
	/* now guaranteed to be in [0, RAND_MAX) */
	int range       = max - min,
		remainder   = RAND_MAX % range,
		bucket      = RAND_MAX / range;
  /* There are range buckets, plus one smaller interval
     within remainder of RAND_MAX */
	 if (base_random < RAND_MAX - remainder) {	
		return min + base_random/bucket;
	} else {
		return random_in_range (min, max);
	}
}

stato get_stato(int x, int y, short unsigned* m){ 
	if(x<0 || y<0 || x>=N_X || y>=N_Y) return MORTO;		// se non calcolati, i bordi non fanno parte dell'universo

	switch(m[x*N_Y+y]) {
		case 0:
			return MORTO;
			break;
		case 1:
			return VIVO;
			break;
		default:
			printf("Strano stato\t [%d,%d] %d \n ",x,y,m[x*N_Y+y]);
			m[x*N_Y+y]=0;
			printf("ucciso\n");
			return ICS;
			break;
	}
}

short in_array(short n,short* a,short size) {
	short i;
	for(i=0;i<size;++i) {
		if( *(a+i)==n ) return 1;
	}
	return 0;	
}

void init_matrix_1D(short unsigned* m,int* n_gen) {
	P(sem_id,0);
	*n_gen=0;
	int i;
	for(i=0;i<N_X*N_Y;i++) {
		if(rand()%3==0)
			m[i]=1;
		else
			m[i]=0;
	}
	V(sem_id,0);

}


void init_matrix(short unsigned* m,int* n_gen) {
	*n_gen=0;
	int i;
	for(i=0;i<N_X;i++) {
		m[i]=0;
	}
	fiat(N_X/2,0,m);
	*n_gen=0;

}


void  uccidi(int x,int y,short unsigned* m) {
	m[x*N_Y+y]=0;
}

void fiat(int x,int y,short unsigned* m) {
	m[x*N_Y+y]=1;
}
