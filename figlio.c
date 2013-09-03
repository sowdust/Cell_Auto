#ifndef _HEADER_
#define _HEADER_
#include "header.h"
#endif



void	uscendo(int);
void	evolvi (short unsigned*, short unsigned*, int*, short*, short,
		short*, short);
void	sierpinski(short unsigned*, short unsigned*, int*);
short	vicini_vivi(int, int, short unsigned*);
int	get_population(short unsigned*);
int	in_stallo(short unsigned*, short unsigned*);





//	se in sezione critica = 1
int	inuso = 0;

//	elementi IPC
int	qid_to_gr, qid_to_proc, qid_figlio_term;
int	shm_id, sem_id, sh_gen_id;


/*	le regole per l'evoluzione possono essere passate in		*/
/*	notazione MCell: Sx/By dove 'x' e' il numero di vicini		*/
/*	vivi necessari per sopravvivere e 'y' il numero di vicini	*/
/*	vivi necessari per rinascere					*/
short	rule_s[2]={2,3};		// S23/B3: game of life
short	rule_b[1]={3};
short	size_s=2;
short	size_b=1;






int	in_stallo(short unsigned* m, short unsigned* local)
{	// da usare in testing, con un solo processo figlio

	int diff=0,y;
	inuso=P(sem_id,0);
	for(y=0;y<N_Y*N_X;++y) {
		if(local[y]!=m[y])
			++diff;
	}
	inuso=V(sem_id,0);
	return (diff == 0);
}


void	evolvi (short unsigned* shm, short unsigned* local,int* n_gen,
		short* s,short size_s,short* b,short size_b)
{

	int x, y;
	short vivi;
	
	inuso=P(sem_id,0);
	
	for ( y=0; y < N_Y * N_X; ++y )
	{ // copia la situazione corrente in array locale
		local[y]=shm[y];
	}

	for( y=0; y < N_Y; ++y )
	{
		for( x=0; x < N_X; ++x )
		{
			vivi=vicini_vivi(x,y,local);

			switch( get_stato(x,y,local)) 
			{
				case VIVO:	// S
					if( in_array(vivi,s,size_s) )
						fiat(x,y,shm);
					else
						uccidi(x,y,shm);
					break;
				case MORTO:	// B
					if( in_array(vivi,b,size_b) )
						fiat(x,y,shm);
					else
						uccidi(x,y,shm);
					break;
				default:	// ?
					;
					break;
			}
		}
	}
	++(*n_gen);
	inuso=V(sem_id,0);
}


void	sierpinski(short unsigned* shm, short unsigned* local,int* n_gen)
{
	int x,y;
	int des,sin,su;

	inuso=P(sem_id,0);
	
	if( (* n_gen) == 0 ) 
	{	// inizializza la matrice con una sola cella viva
		for ( y=0; y < N_Y * N_X; ++y )	local[y]=0;
		
		fiat( random_in_range(0,N_X), random_in_range(0,N_Y), local);
		++(* n_gen);
	} else
	{
		for( y=0; y < N_Y * N_X; ++y )	local[y]=shm[y];
	}

	for( y=0; y < N_Y; ++y )
	{ 
		for( x=0; x < N_X; ++x )
		{
			// rendiamo l'universo un toro \m/
			if(x==0)	sin=N_X-1;
			else		sin=x-1;
			if(x==N_X-1)	des=0;
			else		des=x+1;
			if(y==0)	su=N_Y-1;	
			else		su=y-1;
	
			switch( get_stato(x,y,local) )
			{
				case MORTO:
					if	// XOR
					(	(get_stato(sin,su,local)==VIVO)
					 ^	(get_stato(des,su,local)==VIVO)
					 ) 
						fiat(x,y,shm);
					else
						uccidi(x,y,shm);
					break;
				case VIVO:
					if	// condizione da rivedere
					(	(get_stato(sin,su,local)==VIVO)
					 &&	(get_stato(des,su,local)==VIVO)
					 ) 
						uccidi(x,y,shm);
					else
						fiat(x,y,shm);
					break;
				defaut:
					;
					break;
				}
			}
		} 
	++(*n_gen);
	inuso=V(sem_id,0);
}


short	vicini_vivi(int x,int y,short unsigned* shm)
{
	int su, giu, des, sin;
	short tot, inusodoppio;
	
	//	uniamo i bordi dell'universo in modo da 
	//	renderlo toroidale
	if(x==0)	sin=N_X-1;
	else		sin=x-1;
	if(x==N_X-1)	des=0;
	else		des=x+1;
	if(y==0)	su=N_Y-1;
	else		su=y-1;
	if(y==N_Y-1)	giu=0;
	else		giu=y+1;
	
	if(!inuso)
	{
		inusodoppio=P(sem_id,0);
	}
	tot =	 ((get_stato(sin,su,shm)==VIVO) + (get_stato(sin,y,shm)==VIVO)
		+ (get_stato(sin,giu,shm)==VIVO) + (get_stato(x,su,shm)==VIVO)
		+ (get_stato(x,giu,shm)==VIVO) + (get_stato(des,su,shm)==VIVO)
		+ (get_stato(des,y,shm)==VIVO) + (get_stato(des,giu,shm)==VIVO) );
	
	if(inusodoppio)
	{
		inuso=V(sem_id,0);
	}
	
	return	tot;
}

//	richiamata dal gestore dei segnali TERM KILL e INT
void	uscendo(int s)
{
	if(s)	printf("[%d] termina\n",getpid());
	else	printf("[%d] termina per cause non naturali\n",getpid());
	msg_rqst msg2;
	msg2.p=getpid();
	msg2.r=ADDIO;
	msg2.type=ADDIO;
	if ((msgsnd(qid_figlio_term,&msg2,sizeof(msg_rqst),0)) < 0)
		fprintf (stderr, "[%d]:Impossibile contattare GR prima di terminazione \n%s\n",
			getpid(),strerror(errno) );
	if(inuso)
	{	// liberiamo la memoria condivisa
		V(sem_id,0);
		inuso=0;
	}
	exit(EXIT_SUCCESS);
}


int	get_population(short unsigned* m)
{
	int p=0, y, x;

	inuso=P(sem_id,0);
	for( y=0; y < N_Y; ++y )
	{
		for( x=0; x < N_X; ++x)
		{
			if((get_stato(x,y,m))==VIVO) 	++p;
		}
	}
	inuso=V(sem_id,0);
	return p;
}


void	main(int argc, char* argv[])
{
	int x,y;
	int* n_generazioni;
	short vivi;
	stato c_s;
	
	short unsigned** shm;
	short unsigned** local_m = (short unsigned**) 
				malloc(sizeof(short unsigned) * N_X * N_Y);
	
	key_t k_att_rqsts=ftok(FTOK_PATH,'A');
	key_t k_att_rspns=ftok(FTOK_PATH,'B');
	key_t k_att_figlio_term=ftok(FTOK_PATH,'E');
	
	msg_rspns* msg_to_proc = (msg_rspns*)malloc(sizeof(msg_rspns));
	msg_rqst msg;
	
	signal(SIGINT, uscendo);
	signal(SIGTERM, uscendo);
	signal(SIGKILL, uscendo);

	//	aggancia le code
	if(	((qid_to_gr=msgget(k_att_rqsts,0))==-1) 
		|| ((qid_to_proc=msgget(k_att_rspns,0))==-1)
		|| ((qid_figlio_term=msgget(k_att_figlio_term,0))==-1)	)
	{
		fprintf (stderr, "[%d]Impossibile connettersi alle code \n%s\n",
		getpid(),strerror(errno) );
		uscendo(0);
	}

	//	richiede aggancio al GR tramite messaggio
	msg.p=getpid();
	msg.r=AGGANCIO;
	if( (msgsnd(qid_to_gr,&msg,sizeof(msg_rqst),0)) < 0 ) 
	{
		fprintf (stderr, "Errore nell'invio messaggio di aggancio\n%s\n",
			strerror(errno) );
		uscendo(0);
	}
	
	//	ricezione conferma aggancio
	if(msgrcv(qid_to_proc,msg_to_proc,sizeof(msg_rspns)-sizeof(long),0,0) <= 0)
	{
		fprintf (stderr, "[%d] messaggio vuoto  [%s:%d]\n%s\n",
		getpid(),__FILE__,__LINE__,strerror(errno) );
		uscendo(0);	// comunica al GR la propria uscita
	} else
	{
		shm_id = msg_to_proc->shm_id; // shared matrix
		sem_id = msg_to_proc->sem_id; // semaforo
		sh_gen_id = msg_to_proc->sh_gen_id; // shared n_generazioni
		printf("[%d] Agganciato al GR\n ", getpid());
	}
	
	//	COLLEGAMENTO MEM CONDIVISA IN PROPRIO SPAZIO INDIRIZZI
	if((shm = shmat(shm_id,NULL,0)) < 0 )
	{
		fprintf (stderr, "[%d] Errore agganciando matrice condivisa\n",
			getpid(),strerror(errno) );
	}
	if((n_generazioni = shmat(sh_gen_id,NULL,0)) < 0)
	{
		fprintf (stderr, "[%d] Errore agganciando l'intero condiviso\n",
			getpid(),strerror(errno) );
	}

	while(1) {
//		evolvi_life(shm,local_m,n_generazioni);
		evolvi(shm,local_m,n_generazioni,&rule_s,size_s,&rule_b,size_b);
		//sierpinski(shm,local_m,n_generazioni);
		if(in_stallo(local_m,shm)) {
			printf("\n\n\n Situazione di stallo\n\n");
			sleep(2);
			init_matrix(shm,n_generazioni);
		}
	}
	

	uscendo(1);
}