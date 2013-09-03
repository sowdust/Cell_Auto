#ifndef _HEADER_
#define _HEADER_
#include "header.h"
#endif

#define buffer_type 1
#define alarm_delay 20
#define ALARM_DELAY 60
 
int qid_to_gr, qid_to_proc,qid_nuovo_dato,qid_nuovo_disponibile,qid_figlio_term;
int shm_id;
int sh_gen_id;
int sem_id;
//int n_proc=0;
time_t best_before;
lista* lista_processi=NULL;
short unsigned* mat;
extern char **environ;  /* see environ(7) */


int* dividi_area(int n_proc,int x) {
	int i;
	int* area=(int*)malloc(sizeof(int)*n_proc);
	for(i=0;i<n_proc;++i)
		area[i]=x/n_proc+i*n_proc;
	return area;	
}

short* aree_occupate(int n_proc) {
	int i;
	short* occupate=(short*)malloc(sizeof(short)*n_proc);
	for(i=0;i<n_proc;++i)
		*(occupate+i)=0;
	return occupate;
}

int area_libera(short* occupate,int* aree) {
	int i=0;
	while(*(occupate+(i++))) ;
	return i-1;
}

int create_matrix() {
	int mem_id;
	if((mem_id=shmget(IPC_PRIVATE,sizeof(short unsigned)*N_X*N_Y,IPC_CREAT|PERMESSI_REPOSITORY))==-1)
		return ERROR;
	return mem_id;
}

int crea_buffer() {
printf("Attivatosi il gestore del buffer\t [pid] %d\n",getpid());
	if(!buffer_type)	return(create_buffer());
	else			return(create_matrix());
	
}



int crea_semaforo(int val) {
	union semun arg;
	int sem_id;
	if((sem_id=semget(IPC_PRIVATE,1,PERMESSI_SEMAFORI))<0) {
		fprintf (stderr, "Errore nella creazione del semaforo. [file] %s [linea] %d\n%s\n",__FILE__,__LINE__,strerror(errno) );
		exit(0);
	}
	arg.val=val;
	if(semctl(sem_id,0,SETVAL,arg)==-1) {
		fprintf (stderr, "Errore nell inizializzazione del semaforo. [file] %s [linea] %d\n%s\n",__FILE__,__LINE__,strerror(errno) );
		exit(0);
	}
	return sem_id;
}

void anelito(int s) {
	printf("Terminazione:\n");
	print_list(lista_processi);
//	print_buffer(buff);
	if((shmctl(shm_id, IPC_RMID, NULL) == -1) || (shmctl(sh_gen_id, IPC_RMID, NULL) == -1))
		fprintf (stderr, "Errore eliminazione mem condivisa [file] %s [linea] %d\n%s\n",__FILE__,__LINE__,strerror(errno) );
	if(semctl(sem_id, 0, IPC_RMID, NULL)==-1)
		fprintf (stderr, "Errore eliminazione semaforo [file] %s [linea] %d\n%s\n",__FILE__,__LINE__,strerror(errno) );
	if((msgctl(qid_to_gr, IPC_RMID, NULL)==-1)
		|| (msgctl(qid_to_proc, IPC_RMID, NULL)==-1) || (msgctl(qid_nuovo_dato, IPC_RMID, NULL)==-1)
		|| (msgctl(qid_nuovo_disponibile, IPC_RMID, NULL)==-1) || (msgctl(qid_figlio_term, IPC_RMID, NULL)==-1))        
		fprintf (stderr, "Errore eliminazione code messaggi [file] %s [linea] %d\n%s\n",__FILE__,__LINE__,strerror(errno) );
	exit(EXIT_SUCCESS);
}

void allarme(int s) {
	P(sem_id,0);
	alarm(ALARM_DELAY);
	V(sem_id,0);
}

void main(int argc, char* argv[]) {
	
	printf("[%d] Avvio del gestore repository GR\n",getpid());
		
	int x=N_X,y=N_Y;
	int n_proc=0;
	int* n_generazioni;

	ssize_t msg_size;
	//unsigned int sveglia=alarm(alarm_delay);
	msg_rqst* m = (msg_rqst*)malloc(sizeof(msg_rqst));
	msg_rqst* msg_nuovo_dato = (msg_rqst*)malloc(sizeof(msg_rqst));
	msg_rqst* msg_figlio_term = (msg_rqst*)malloc(sizeof(msg_rqst));
	msg_rspns msg_to_proc;
	msg_rqst msg_nuovo_disponibile;
	key_t k_att_rqsts=ftok(FTOK_PATH,'A');	// requests
	key_t k_att_rspns=ftok(FTOK_PATH,'B');	// responses
	key_t k_att_nuovo_dato=ftok(FTOK_PATH,'C');
	key_t k_att_nuovo_disponibile=ftok(FTOK_PATH,'D');
	key_t k_att_figlio_term=ftok(FTOK_PATH,'E');
	
	pid_t pid_tv;
	
	//short* assegnate = aree_occupate(n_proc);
	//int* aree = dividi_area(n_proc,x);
	//alarm(ALARM_DELAY);
	
	char *argv_exec[] = {
        "/usr/bin/xfce4-terminal",
        "--geometry",
        "160x45",
        "-x",
        "./tv",
        NULL };
	
	

	if(signal(SIGALRM,allarme)==SIG_ERR)
		fprintf (stderr, "Errore segnale allarme [file] %s [linea] %d\n%s\n",__FILE__,__LINE__,strerror(errno) );
	if(signal(SIGINT,anelito)==SIG_ERR)
		fprintf (stderr, "Errore segnale allarme [file] %s [linea] %d\n%s\n",__FILE__,__LINE__,strerror(errno) );
	/*if(signal(SIGKILL,anelito)==SIG_ERR)
		fprintf (stderr, "Errore segnale allarme [file] %s [linea] %d\n%s\n",__FILE__,__LINE__,strerror(errno) );
*/
// creazione buffer 
	if((sh_gen_id=shmget(IPC_PRIVATE,sizeof(short int),IPC_CREAT|PERMESSI_REPOSITORY))==-1)
		exit(ERROR);
		
	shm_id=create_matrix();
	mat=shmat(shm_id,NULL,0);
	n_generazioni=shmat(sh_gen_id,NULL,0);
	init_matrix(mat,n_generazioni);

	

		
		
		
// creazione code messaggi
	if((qid_to_gr=msgget(k_att_rqsts,IPC_CREAT|PERMESSI_CODA))==-1) {
		fprintf (stderr, "Errore nella creazione della coda di richieste. [file] %s [linea] %d\n%s\n",__FILE__,__LINE__,strerror(errno) );
		exit(0);
	}
		
	if((qid_to_proc=msgget(k_att_rspns,IPC_CREAT|PERMESSI_CODA))==-1) {
		fprintf (stderr, "Errore nella creazione della coda dei responsi. [file] %s [linea] %d\n%s\n",__FILE__,__LINE__,strerror(errno) );
		exit(0);
	}
	
	if((qid_nuovo_dato=msgget(k_att_nuovo_dato,IPC_CREAT|PERMESSI_CODA))==-1) {
		fprintf (stderr, "Errore nella creazione della coda nuovo dato. [file] %s [linea] %d\n%s\n",__FILE__,__LINE__,strerror(errno) );
		exit(0);
	}
	
	if((qid_nuovo_disponibile=msgget(k_att_nuovo_disponibile,IPC_CREAT|PERMESSI_CODA))==-1) {
		fprintf (stderr, "Errore nella creazione della coda nuovo_disponibile. [file] %s [linea] %d\n%s\n",__FILE__,__LINE__,strerror(errno) );
		exit(0);
	}
	
	if((qid_figlio_term=msgget(k_att_figlio_term,IPC_CREAT|PERMESSI_CODA))==-1) {
		fprintf (stderr, "Errore nella creazione della coda figli terminati. [file] %s [linea] %d\n%s\n",__FILE__,__LINE__,strerror(errno) );
		exit(0);
	}
// creazione semaforo 
	sem_id=crea_semaforo(1);
		
	
// fork per la tv
       /*
         * If execve succeeded, the invoked program has
         * replaced this process, and will either run or
         * (presumably) report its own errors. If we're
         * still in control, the execve failed, so print
         * an error and exit.
         */

    pid_tv = fork();

    if (pid_tv == -1) {
        perror("fork");
    }
    if (pid_tv == 0) {
        (void) execv(argv_exec[0], argv_exec);
         perror(argv_exec[0]);
    } else {
        if (wait(0) != pid_tv) {
            perror("wait");
        }
    } 

	while(1) {
		msg_size=msgrcv(qid_to_gr,m,sizeof(msg_rqst),0,0);
		if(msg_size>0) {	
			if((ins_in_coda(&lista_processi,m->p,time(NULL)))==OK && m->r==AGGANCIO) {
				msg_to_proc.p=m->p;
				msg_to_proc.r=AGGANCIO;
				msg_to_proc.type=m->p;
				msg_to_proc.shm_id=shm_id;
				msg_to_proc.sem_id=sem_id;
				msg_to_proc.sh_gen_id=sh_gen_id;
				
				if((msgsnd(qid_to_proc,&msg_to_proc,sizeof(msg_rspns)-sizeof(long),IPC_NOWAIT))<0) {
					fprintf (stderr, "Errore nell'invio messaggio [file] %s [linea] %d\n%s\n",__FILE__,__LINE__,strerror(errno) );
					/// TODO: KILL THAT PROCESS
				}else{
					printf("Processo %d registrato al GR\n",msg_to_proc.p);
	printf("%d\n",__LINE__);
					++n_proc;
				}
			}
			else {
				printf("messaggio: %d, %d, %d\n",(int)msg_size,m->p,m->r);
				fprintf (stderr, "[Richiesta di aggancio non accettata] messaggio: %d, %d, %d. [file] %s [linea] %d\n%s\n",(int)msg_size,m->p,m->r,__FILE__,__LINE__,strerror(errno) );
			}
		}
	printf("%d\n",__LINE__);
		
		if((msg_size=msgrcv(qid_figlio_term,msg_figlio_term,sizeof(msg_rqst),0,IPC_NOWAIT))>0) {
			printf("Il GR dice addio al figlio %d\n",msg_figlio_term->p);
			remove_by_pid(&lista_processi,msg_figlio_term->p);
		}
	}
	
	anelito(0);
}
