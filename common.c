#ifndef _HEADER_
#define _HEADER_
#include "header.h"
#endif

int sem_id; // usata in funzioni che richimano P e V

int P(int semid, int semnum)	// signal
{
		struct sembuf cmd;
		cmd.sem_num = semnum;
		cmd.sem_op = -1;
		cmd.sem_flg = 0;
		semop(semid, &cmd, 1);
		return 1;

}


int V(int semid, int semnum)	// wait
{
		struct sembuf cmd;
		cmd.sem_num = semnum;
		cmd.sem_op = 1;
		cmd.sem_flg = 0;
		semop(semid, &cmd, 1);
		return 0;
}

int Z(int semid, int semnum)	// wait for zero
{
		struct sembuf cmd;
		cmd.sem_num = semnum;
		cmd.sem_op = 0;
		cmd.sem_flg = 0;
		if(semop(semid, &cmd, 1) < 0)
		{
			fprintf (stderr, "Errore inizializzazione del semaforo\n%s\n",
				strerror(errno) );

		}
		return 0;
}

int R(int semid, int semnum, int val)	// reset
{

	union semun arg;
	arg.val = val;
	if( semctl(semid,semnum,SETVAL,arg) == -1 )  
	{
		fprintf (stderr, "Errore inizializzazione del semaforo\n%s\n",
				strerror(errno) );
	}
}

stato get_stato(int x, int y, short unsigned* m)
{
	// a meno che diversamente specificato all'infuori di questa
	// funzione, le celle al di la' dei bordi vengono considerate
	// come perennemente morte
	if(x<0 || y<0 || x>=N_X || y>=N_Y) return MORTO;

	switch(m[x*N_Y+y])
	{
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

short in_array(short n,short* a,short size)
{
	short i;
	for(i=0;i<size;++i) {
		if( *(a+i)==n ) return 1;
	}
	return 0;	
}

void clean_matrix(short unsigned* m,int* n_gen)
{
	*n_gen=0;
	int i;
	for(i=0;i<N_X*N_Y;i++)
	{	m[i]=0;		}
}
void init_matrix(short unsigned* m,int* n_gen)
{
	*n_gen=0;
	int i;
	for(i=0;i<N_X*N_Y;i++) {
		if( random_in_range(0,RAND_MAX) % 2 == 0 )
			m[i]=0;
		else
			m[i]=1;
	}
}

void copy_matrix(short unsigned* m,short unsigned* k,int* n_gen)
{
	*n_gen=0;
	int i;
	for(i=0;i<N_X*N_Y;i++) {
		m[i]=k[i];
	}
}


void  uccidi(int x,int y,short unsigned* m)
{
	m[x*N_Y+y]=0;
}

void fiat(int x,int y,short unsigned* m)
{
	m[x*N_Y+y]=1;
}

/*	funzione trovata in rete per il calcolo di un numero		*/
/*	pseudo casuale all'interno dell'intervallo semi aperto		*/
/*		[min,max)												*/
/*				  			 autore presunto: Ryan Reich		*/
int random_in_range (unsigned int min, unsigned int max)	
{
	int base_random = rand();
	if (RAND_MAX == base_random) return random_in_range(min, max);
	int range       = max - min,
		remainder   = RAND_MAX % range,
		bucket      = RAND_MAX / range;
	 if (base_random < RAND_MAX - remainder) {	
		return min + base_random/bucket;
	} else {
		return random_in_range (min, max);
	}
}

void termina_proc(lista l,int signal)
{
	if((l)==NULL) 	return ;

   	kill((l)->p, signal);
   	termina_proc((l)->nxt,signal);
	return ;
}
