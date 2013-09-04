#ifndef _HEADER_
#define _HEADER_
#include "header.h"
#endif



void	anelito(int);
int	create_matrix();
int	crea_semaforo(int);



//	elementi IPC
int	qid_to_gr, qid_to_proc, qid_figlio_term;
int	shm_id, sem_id, sh_gen_id;

//	matrice condivisa
short unsigned* mat;

//	lista processi agganciati (figli e tv)
lista* lista_processi = NULL;





int create_matrix()
{
	int mem_id;
	if((mem_id = shmget(IPC_PRIVATE,sizeof(short unsigned)*N_X*N_Y,
		IPC_CREAT | PERMESSI_REPOSITORY)) == -1 )
		return ERROR;
	return mem_id;
}


int crea_semaforo(int val)
{
	int sem_id;
	union semun arg;

	if((sem_id = semget(IPC_PRIVATE,1,PERMESSI_SEMAFORI)) < 0)
	{
		fprintf (stderr, "Errore nella creazione del semaforo.\n%s\n",
			strerror(errno) );
		anelito(0);
	}
	arg.val=val;
	if( semctl(sem_id,0,SETVAL,arg) == -1 )
	{
		fprintf (stderr, "Errore inizializzazione del semaforo\n%s\n",
		strerror(errno) );
		anelito(0);
	}
	return sem_id;
}


void anelito(int s)
{
	printf("Terminazione GR\n");
	print_list(lista_processi);
	
	if((shmctl(shm_id, IPC_RMID, NULL) == -1)
		|| (shmctl(sh_gen_id, IPC_RMID, NULL) == -1))
		fprintf (stderr, "Errore eliminazione mem condivisa\n%s\n",
			strerror(errno) );
		
		
	if(semctl(sem_id, 0, IPC_RMID, NULL)==-1)
		fprintf (stderr, "Errore eliminazione semaforo\n%s\n",
			strerror(errno) );
			
	if((msgctl(qid_to_gr, IPC_RMID, NULL)==-1)
		|| (msgctl(qid_to_proc, IPC_RMID, NULL)==-1)
		|| (msgctl(qid_figlio_term, IPC_RMID, NULL)==-1))        
		fprintf (stderr, "Errore eliminazione code messaggi \n%s\n",
			strerror(errno) );
	exit(EXIT_SUCCESS);
}


void main(int argc, char* argv[])
{
	
	printf("[%d] Avvio del gestore repository GR\n",getpid());

	int x = N_X, y = N_Y;
	int n_proc=0;
	int* n_generazioni;

	msg_rqst* m = (msg_rqst*) malloc(sizeof(msg_rqst));
	msg_rqst* msg_figlio_term = (msg_rqst*) malloc(sizeof(msg_rqst));
	msg_rspns msg_to_proc;

	key_t k_att_rqsts=ftok(FTOK_PATH,'A');
	key_t k_att_rspns=ftok(FTOK_PATH,'B');
	key_t k_att_figlio_term=ftok(FTOK_PATH,'E');

	pid_t pid_tv;
	pid_t pid_daddy;
	
	char *argv_exec[] = {
        "/usr/bin/xfce4-terminal",
        "--geometry",
        "160x48",
        "-x",
        "./tv",
        NULL };

	char *argv_daddy[] = {
        "/usr/bin/xfce4-terminal",
        "--geometry",
        "90x30",
        "-x",
        "./daddy",
        NULL };



	if(signal(SIGTERM,anelito)==SIG_ERR)
		fprintf (stderr, "Errore segnale terminazione\n%s\n",
		strerror(errno) );
	if(signal(SIGINT,anelito)==SIG_ERR)
		fprintf (stderr, "Errore segnale interruzione\n%s\n",
		strerror(errno) );

	// agganciamento a n_generazioni condiviso
	if((sh_gen_id = shmget(IPC_PRIVATE,sizeof(short int),
		IPC_CREAT|PERMESSI_REPOSITORY))==-1)
		anelito(0);
	n_generazioni=shmat(sh_gen_id,NULL,0);


	// agganciamento a matrice condivisa
	shm_id = create_matrix();
	mat = shmat(shm_id,NULL,0);

	// inizializzazione universo
	init_matrix(mat,n_generazioni);


	// creazione code messaggi
	if((qid_to_gr = msgget(k_att_rqsts,IPC_CREAT|PERMESSI_CODA))==-1)
	{
		fprintf (stderr, "Errore creazione coda richieste.\n%s\n",
		strerror(errno) );
		anelito(0);
	}

	if((qid_to_proc = msgget(k_att_rspns,IPC_CREAT|PERMESSI_CODA))==-1)
	{
		fprintf (stderr, "Errore creazione coda responsi.\n%s\n",
		strerror(errno) );
		anelito(0);
	}

	if((qid_figlio_term = msgget(k_att_figlio_term,IPC_CREAT|PERMESSI_CODA))==-1)
	{
		fprintf (stderr, "Errore creazione coda figli terminati.\n%s\n",
		strerror(errno) );
		anelito(0);
	}

	// creazione semaforo inizializzato a 1
	sem_id = crea_semaforo(1);


	// fork per il processo tv
	pid_tv = fork();
	if (pid_tv == -1)
	{
		fprintf (stderr, "Errore nella fork.\n%s\n",
		strerror(errno) );
		anelito(0);
	}
	if (pid_tv == 0)
	{	// execv passa automaticamente envp
		(void) execv(argv_exec[0], argv_exec);
		fprintf (stderr, "Errore nella execv.\n%s\n",
		strerror(errno) );
		anelito(0);
	} else
	{
		if (wait(0) != pid_tv)
		{
			fprintf (stderr, "Errore nella wait.\n%s\n",
			strerror(errno) );
			anelito(0);
		}
	}
	//	fork per il padre
	pid_daddy = fork();
	if (pid_daddy == -1)
	{
		fprintf (stderr, "Errore nella fork.\n%s\n",
		strerror(errno) );
		anelito(0);
	}
	if (pid_daddy == 0)
	{	// execv passa automaticamente envp
		(void) execv(argv_daddy[0], argv_daddy);
		fprintf (stderr, "Errore nella execv.\n%s\n",
		strerror(errno) );
		anelito(0);
	} else
	{
		if (wait(0) != pid_daddy)
		{
			fprintf (stderr, "Errore nella wait.\n%s\n",
			strerror(errno) );
			anelito(0);
		}
	}

	while(1) {

		//	richieste di aggancio
		if( msgrcv(qid_to_gr,m,sizeof(msg_rqst),0,0)>0 )
		{
			if( (ins_in_coda(&lista_processi,m->p,time(NULL)))==OK
				&& m->r==AGGANCIO )
			{
				msg_to_proc.p=m->p;
				msg_to_proc.r=AGGANCIO;
				msg_to_proc.type=m->p;
				msg_to_proc.shm_id=shm_id;
				msg_to_proc.sem_id=sem_id;
				msg_to_proc.sh_gen_id=sh_gen_id;

				//	risposta del gr
				if( (msgsnd(qid_to_proc,&msg_to_proc,
				sizeof(msg_rspns)-sizeof(long),IPC_NOWAIT)) <0 )
				{
					fprintf (stderr,"Errore invio messa%s\n",
					strerror(errno) );
				} else
				{
					printf("Processo %d registrato al GR\n",
					msg_to_proc.p);
					++n_proc;
				}
			} else
			{
				fprintf(stderr,"Aggancio non accettata\n%s\n",
				strerror(errno) );
			}
		}

		//	messaggi terminazione figli
		if( msgrcv(qid_figlio_term,msg_figlio_term,
			sizeof(msg_rqst),0,IPC_NOWAIT) > 0)
		{
			printf("Il GR dice addio al figlio %d\n",msg_figlio_term->p);
			remove_by_pid(&lista_processi,msg_figlio_term->p);
		}
	}

	anelito(0);
}
