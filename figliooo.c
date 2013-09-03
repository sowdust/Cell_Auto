
#ifndef _HEADER_
#define _HEADER_
#include "header.h"
#endif

int P(int semid, int semnum) {
		struct sembuf cmd;
		cmd.sem_num = semnum;
		cmd.sem_op = -1;
		cmd.sem_flg = 0;
		semop(semid, &cmd, 1);
}


int V(int semid, int semnum) {
		struct sembuf cmd;
		cmd.sem_num = semnum;
		cmd.sem_op = 1;
		cmd.sem_flg = 0;
		semop(semid, &cmd, 1);
}


void main(int argc, char* argv[]) {
	int qid_to_gr,qid_to_proc;
	int shm_id, sem_id;
	key_t k_att_rqsts=ftok(FTOK_PATH,'A');
	key_t k_att_rspns=ftok(FTOK_PATH,'B');
	msg_rspns* msg_to_proc = (msg_rspns*)malloc(sizeof(msg_rspns));
	buffer* shm;
	buffer* local =  (buffer*)malloc(sizeof(buffer));

// aggancia le code
	if((qid_to_gr=msgget(k_att_rqsts,0))==-1) {
		fprintf (stderr, "[padre:%d]:Impossibile connettersi al gestore. Uscendo \n%s\n",getpid(),strerror(errno) );
		exit(0);
	}
	if((qid_to_proc=msgget(k_att_rspns,0))==-1) {
		fprintf (stderr, "[padre:%d]:Impossibile connettersi al gestore. Uscendo \n%s\n",getpid(),strerror(errno) );
		exit(0);
	}
	msg_rqst msg;
	msg.p=getpid();
	msg.r=AGGANCIO;
	msg.type=AGGANCIO;

// richiede autorizzazione a GR
	if((msgsnd(qid_to_gr,&msg,sizeof(msg_rqst)-sizeof(long),0))<0)
		fprintf (stderr, "Errore nell'invio messaggio [file] %s [linea] %d\n%s\n",__FILE__,__LINE__,strerror(errno) );
	else 
		printf("[figlio:%d] Messaggio di richiesta inviato\n",getpid());

// ottiene autorizzazione
	if(msgrcv(qid_to_proc,msg_to_proc,sizeof(msg_rspns)-sizeof(long),0,0)<=0) {
		fprintf (stderr, "[%d] ricevuto messaggio vuoto [file] %s [linea] %d [expecting type] %ld\n%s\n",getpid(),__FILE__,__LINE__,(long)getpid(),strerror(errno) );
		// SIGNAL TO CODA DEI PROCESSI TOLTI
		exit(0);
	} else {
		printf("[%d] YAY!!! agganciato al GR\n",getpid());
		shm_id=msg_to_proc->shm_id;
		sem_id=msg_to_proc->sem_id;
	}
	
	shm = shmat(shm_id,NULL,0);
	printf("agganciato al buffer?");
	init_buffer(local);

	int i,option,j,max;

	for(i=0;i<10;++i) {
		option = rand() % 3;
		if(0==option) {
			printf("[%d] downloading\n",getpid());	
			local->first=shm->first;
			local->n=shm->n;
			for(j=0;j<shm->n;++j)
				add_to_buffer(local,shm->buffer[(shm->first+j)%shm->size].v,0);		
		}
		if(1==option) {
			printf("[%d] working\n",getpid());	
			sleep(2);
		}
		if(2==option) {
			printf("[%d] uploading\n",getpid());	
			P(sem_id,0);
			status s = add_to_buffer(shm,5,getpid());
			V(sem_id,0);
			if(s==OK) {
				msg.p=getpid();
				msg.r=UPLOAD;
				if((msgsnd(qid_to_gr,&msg,sizeof(msg_rqst),0))<0)
					fprintf (stderr, "Errore nell'invio messaggio upload [file] %s [linea] %d\n%s\n",__FILE__,__LINE__,strerror(errno) );
				else 
					printf("[figlio:%d] Messaggio di upload\n",getpid());
			}else
				fprintf (stderr, "Errore nell'inserimento in buffer [file] %s [linea] %d\n%s\n",__FILE__,__LINE__,strerror(errno) );
		}
		
		
		
		
	}
	
	

	//print_buffer(shm);
//	print_buffer(shm);
	exit(0);
}