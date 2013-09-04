#ifndef	HEADER
#define _HEADER_
#include "header.h"
#endif
#include "rle_reader.h"
#define N_FILES 10
#define RLE_FILE_DIR "RLE/"
#define RLE_FILE_EXT ".lif"



void	uscendo(int);
void	evolvi (short unsigned*, short unsigned*, int*, short*, short,
		short*, short);
void	do_sierpinski(short unsigned*, short unsigned*, int*);
void	init_from_file(short unsigned*,short unsigned*, int*,int,int);
short	vicini_vivi(int, int, short unsigned*);
int	get_population(short unsigned*);
int	in_stallo(short unsigned*, short unsigned*);

//	se in sezione critica = 1
int	inuso = 0;

// 	elementi IPC
int	qid_to_gr, qid_to_proc, qid_figlio_term;
int	shm_id, sem_id, sh_gen_id;

//	nome file per pattern
char * 	f;



int	in_stallo(short unsigned* m, short unsigned* local)
{	// teoricamente scorretta con piu processi visto che intercorre
	// del tempo tra la chiamata di V della funzione di evoluzione
	// e la chiamata P di questa, ma empriricamente aiuta a 
	// identificare situazioni periodiche, come succede con i blinker
	// | - | - ... oppure 0 + 0 + 0 + ...

	int diff=0,y;
	inuso=P(sem_id,0);
	for(y=0;y<N_Y*N_X;++y) {
		if(local[y]!=m[y])
			++diff;
	}
	inuso=V(sem_id,0);
	return (diff == 0);
}

void	init_from_file(short unsigned * m,short unsigned * from, 
	int * n_gen, int xx, int yy)
{
	int x,y;
	int i,j;
	int first_x = (N_X-xx)/2;
	int first_y = (N_Y-yy)/2;

	for(i=0; i<N_X; ++i)
	{
		for(j=0; j<N_Y; ++j)
		{
			if( x>=first_x && x<=(N_X-first_x)
				&& y>=first_y && y<=(N_Y-first_y))
			{
				if( get_stato(x-first_x,y-first_y,from) 
					== VIVO )
					fiat(x,y,m);
				else
					uccidi(x,y,m);

			} else 
			{
				uccidi(x,y,m);
			}
		}
	}

	*n_gen = 0;
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


void	do_sierpinski(short unsigned* shm, short unsigned* local,int* n_gen)
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

char *	rendi_filename(int n)
{
	char * dir = RLE_FILE_DIR;
	char * extension = RLE_FILE_EXT;
	char   fn[2];
	char * f = (char*) malloc(sizeof(char) * FILENAME_MAX);

	fn[0] = (char) (n + '0');
	fn[1] = '\0';

	strcpy(f,dir);
	strcat(f,fn);
	strcat(f,extension);


	return f;
}


short	vicini_vivi(int x,int y,short unsigned* shm)
{
	int su, giu, des, sin;
	short tot, inusodoppio;
	
	//	uniamo i bordi dell'universo in modo da 
	//	renderlo toroidale
	if(x==0) 	sin=N_X-1;
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
		fprintf (stderr, "[%d]:Impossibile contattare GR\n%s\n",
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
	short rule_number;
	stato c_s;
	rule* curr_rule;

	// usato per caricare RLE 
	int file_number;
	
	short unsigned* shm;
	short unsigned* local_m = (short unsigned*) 
				malloc(sizeof(short unsigned) * N_X * N_Y);
	short unsigned* k;
	
	key_t k_att_rqsts=ftok(FTOK_PATH,'A');
	key_t k_att_rspns=ftok(FTOK_PATH,'B');
	key_t k_att_figlio_term=ftok(FTOK_PATH,'E');
	
	msg_rspns* msg_to_proc = (msg_rspns*)malloc(sizeof(msg_rspns));
	msg_rqst msg;



/************************************************************************/
/*			REGOLE PREDEFINITE PER AUTOMA			*/
/*	Le regole si definiscono in notazione MCell:			*/
/*	Sp/Bq	dove b e' il numero di vicini vivi necessari affinche'	*/
/*		una cella sopravviva, q e' il numero di vivi necessari	*/
/*		affinche' una cellula morta rinasca			*/
/************************************************************************/


//	typedef struct {
//		short rule_s[8];	es: {2,3} per S23
//		short rule_b[8];	es: {3} per B3
//		short  size_rule_s;	es: 2 per S23
//		short  size_rule_b;	es: 1 per B3
//		short sierpinski;	1 se si tratta dell'automa 1D,
//					0 altrimenti
//	} rule;


rule game_of_life	= { {2,3,0,0,0,0,0,0},	{3,0,0,0,0,0,0,0},	2,	1,	0 };
rule three4_life	= { {3,4,0,0,0,0,0,0},	{3,4,0,0,0,0,0,0},	2,	2,	0 };
rule high_life		= { {3,2,0,0,0,0,0,0},	{3,6,0,0,0,0,0,0},	2,	2,	0 };
rule day_night		= { {3,4,6,7,8,0,0,0},	{3,6,7,8,0,0,0,0},	5,	4,	0 };
rule sierp		= { {0,0,0,0,0,0,0,0},	{8,0,0,0,0,0,0,0},	1,	1,	1 };
rule rules[] = { game_of_life,
	three4_life,
	high_life,
	day_night,
	sierp
	};
	

	
	signal(SIGINT, uscendo);
	signal(SIGTERM, uscendo);



	//	decide quale regola applicare in base all'input
	if(argc < 2)	rule_number=0;
	else		rule_number=atoi(argv[1])-1;
	if(argc < 3)	file_number = 1;
	else		file_number = atoi(argv[2]);
	curr_rule = (rule*) malloc (sizeof(rule));
	if(rule_number == 5)
	{
		f = rendi_filename(file_number);
		printf(" caricando file %s \n",f);

		k = (short unsigned*) malloc (sizeof(short unsigned)*N_X*N_Y);
		while( start(f,k,&curr_rule->size_rule_b, 
			&curr_rule->size_rule_s, curr_rule->rule_b,
			curr_rule->rule_s) < 0) ;
		curr_rule->sierpinski=2;
		
	} else
	{
		*curr_rule = rules[rule_number];
	}

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
	if(msgrcv(qid_to_proc,msg_to_proc,sizeof(msg_rspns)-sizeof(long),0,0)
			<= 0)
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
	
	//	aggiunta mem condivisa in proprio spazio indirizzi
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
	
	
	switch (curr_rule->sierpinski)
	{
		case 1:
			while(1)
			{
				do_sierpinski(shm,local_m,n_generazioni);
				if(in_stallo(local_m,shm))
				{
					printf("Stallo alla generazione %d \n",
					*n_generazioni);
					sleep(2);
					init_matrix(shm,n_generazioni);
				}
			}
			break;
		case 2:
			while(1)
			{
				if(*n_generazioni == 0)
				{	
					P(sem_id,0);
					copy_matrix(shm,k,n_generazioni);
					V(sem_id,0);
				}
				evolvi(shm,local_m,n_generazioni, curr_rule->rule_s, 
					curr_rule->size_rule_s, curr_rule->rule_b, curr_rule->size_rule_b );
				if(in_stallo(local_m,shm))
				{
					printf("Stallo alla generazione %d \n",
					*n_generazioni);
					sleep(2);
					inuso=P(sem_id,0);
					init_matrix(shm,n_generazioni);
					inuso=V(sem_id,0);
				}
			}
			break;
		default:
			while(1)
			{
				evolvi(shm,local_m,n_generazioni, curr_rule->rule_s, 
					curr_rule->size_rule_s, curr_rule->rule_b, curr_rule->size_rule_b );
				if(in_stallo(local_m,shm))
				{
					printf("Stallo alla generazione %d \n",
					*n_generazioni);
					sleep(2);
					inuso=P(sem_id,0);
					init_matrix(shm,n_generazioni);
					inuso=V(sem_id,0);
				}
			}
			break;
	}
}
