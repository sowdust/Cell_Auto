#ifndef _HEADER_
#define _HEADER_
#include "header.h"
#endif


int qid_to_gr,qid_to_proc,qid_nuovo_dato,qid_nuovo_disponibile,qid_figlio_term;
int shm_id, sem_id, sh_gen_id;
int in_sez_critica=0;

int	check(short unsigned* m, short unsigned* local ); 
void	 evolvi(short unsigned* shm, short unsigned* local,int* n_gen,short* s,short size_s,short* b,short size_b);
void	 evolvi_1D(short unsigned* shm, short unsigned* local,int* n_gen,short* s,short size_s,short* b,short size_b);
short	vicini_vivi(int x,int y,short unsigned* m);
void	uscendo(int s);
short vicini_vivi_1D(int x,int y,short unsigned* shm);


short rule_s[2]={2,3};
short rule_b[1]={3};
short size_s=2;
short size_b=1;


int check(short unsigned* m, short unsigned* local) {
	int diff=0,y;
	P(sem_id,0);
	for(y=0;y<N_Y*N_X;++y) {
		if(local[y]!=m[y])
			++diff;
	}
	V(sem_id,0);
	return diff;
}

void evolvi(short unsigned* shm, short unsigned* local,int* n_gen,short* s,short size_s,short* b,short size_b) {
	int x, y;
	short vivi;
	stato c_s;
	
	P(sem_id,0);
	for(y=0;y<N_Y*N_X;++y) {
		local[y]=shm[y];
	}	
	for(y=0;y<N_Y;y++) {
		for(x=0;x<N_X;x++) {
			vivi=vicini_vivi(x,y,local);
			
			switch(get_stato(x,y,local)) {
				case VIVO:
					if(in_array(vivi,s,size_s))
						fiat(x,y,shm);
					else
						uccidi(x,y,shm);
					break;
				case MORTO:
					if(in_array(vivi,b,size_b))
						fiat(x,y,shm);
					else
						uccidi(x,y,shm);
					break;
				default:
					printf("ma che succede?? [%d,%d]\n",x,y);
					break;
			}
		}
		
	}
	++(*n_gen);
	V(sem_id,0);

}

void sierpinski(short unsigned* shm, short unsigned* local,int* n_gen) {
	int x,y;
	
	int des,sin,su;

	in_sez_critica=P(sem_id,0);
	for(y=0;y<N_Y*N_X;++y) {
		local[y]=shm[y];
	}
	
	if( *n_gen!=0 && ( (*n_gen-1) % N_Y ==0 ) ) {
		fiat(random_in_range(0,N_X),0,shm);
		y=1;
	}else {
		y=0;
	}

	for(y; y<N_Y; ++y) { 
		for(x=0; x<N_X; ++x) {
				if(x==0)	sin=N_X;
				else		sin=x-1;
				if(x==N_X)	des=0;
				else		des=x+1;
				if(y==0)	su=N_Y;	
				else		su=y-1;
	
			switch(get_stato(x,y,local)) {
				case MORTO:
					if( (get_stato(x-1,y-1,local)==VIVO) ^ (get_stato(x+1,y-1,local)==VIVO) ) // xor
						fiat(x,y,shm);
					else
						uccidi(x,y,shm);
					break;
				case VIVO:
						//if( ((*n_gen-1) % N_Y == 0 ) )
					if( *n_gen!=0 && ( (*n_gen-1) % N_Y ==0 ) )
							uccidi(x,y,shm);
					else
							fiat(x,y,shm);
					break;
				defaut:
					;// errore
					break;
				}
			}
		} 
	
	++(*n_gen);
	in_sez_critica=V(sem_id,0);	
	
}


void evolvi_1D(short unsigned* shm, short unsigned* local,int* n_gen,short* s,short size_s,short* b,short size_b) {
	int x, y;
	short vivi;
	stato c_s;
	
	P(sem_id,0);
	for(y=0;y<N_Y*N_X;++y) {
		local[y]=shm[y];
	}
	if(*n_gen==N_X) {
		*n_gen=-1;
	}
	x=	++(*n_gen);	
	for(y=0;y<N_Y;y++) {
		vivi=vicini_vivi_1D(x,y,local);
		if(vivi)
			printf("[%d,%d] vicini vivi: %d\n",x,y,vivi); 

	
		switch(get_stato(x,y-1,local)) {
			case VIVO:
				if(in_array(vivi,s,size_s))
					fiat(x,y,shm);
				else
					uccidi(x,y,shm);
				break;
			case MORTO:
				if(in_array(vivi,b,size_b))
					fiat(x,y,shm);
				else
					uccidi(x,y,shm);
				break;
			default:
				printf("ma che succede?? [%d,%d]\n",x,y);
				break;
		}
	}
	V(sem_id,0);

}

short vicini_vivi(int x,int y,short unsigned* shm) {
	int su,giu,des,sin;
	if(x==0)	sin=N_X;
	else		sin=x-1;
	if(x==N_X)	des=0;
	else		des=x+1;
	if(y==0)	su=N_Y;
	else		su=y-1;
	if(y==N_Y)	giu=0;
	else		giu=y+1;
	
	return	((get_stato(sin,su,shm)==VIVO) + (get_stato(sin,y,shm)==VIVO) + (get_stato(sin,giu,shm)==VIVO)
		+ (get_stato(x,su,shm)==VIVO) + (get_stato(x,giu,shm)==VIVO) + (get_stato(des,su,shm)==VIVO)
		+ (get_stato(des,y,shm)==VIVO) + (get_stato(des,giu,shm)==VIVO));
}

short vicini_vivi_1D(int x,int y,short unsigned* shm) {
	int des,sin;
	if(x==0)	sin=N_X;
	else		sin=x-1;
	if(x==N_X)	des=0;
	else		des=x+1;
	
	return	((get_stato(sin,y,shm)==VIVO) + (get_stato(des,y,shm)==VIVO));
}


void uscendo(int s) {
	if(s)	printf("[%d] termina\n",getpid());
	else	printf("[%d] termina per cause non naturali\n",getpid());
	msg_rqst msg2;
	msg2.p=getpid();
	msg2.r=ADDIO;
	msg2.type=ADDIO;
	if ((msgsnd(qid_figlio_term,&msg2,sizeof(msg_rqst),0))<0)
		fprintf (stderr, "[%d]:Impossibile contattare GR prima di terminazione \n%s\n",getpid(),strerror(errno) );
	if(in_sez_critica) {
		V(sem_id,0);
		in_sez_critica=0;
	}
	exit(EXIT_SUCCESS);
}

int get_population(short unsigned* m) {
	int p=0,y,x;
	P(sem_id,0);
	for(y=0;y<N_Y;++y) {
		for(x=0;x<N_X;++x) {
			if((get_stato(x,y,m))==VIVO) {
				++p;
			}
		}
	}
	V(sem_id,0);
	return p;
}


void main(int argc, char* argv[]) {

	int x,y;
	
	key_t k_att_rqsts=ftok(FTOK_PATH,'A');
	key_t k_att_rspns=ftok(FTOK_PATH,'B');
	key_t k_att_nuovo_dato=ftok(FTOK_PATH,'C');
	key_t k_att_nuovo_disponibile=ftok(FTOK_PATH,'D');
	key_t k_att_figlio_term=ftok(FTOK_PATH,'E');
	
	msg_rspns* msg_to_proc = (msg_rspns*)malloc(sizeof(msg_rspns));
	msg_rqst* msg_nuovo_dato = (msg_rqst*)malloc(sizeof(msg_rqst));
	msg_rqst* msg_nuovo_disponibile = (msg_rqst*)malloc(sizeof(msg_rqst));
	short unsigned** shm;
	int* n_generazioni;
	short unsigned** local_m=(short unsigned**)malloc(sizeof(short unsigned)*N_X*N_Y);
	short vivi;
	stato c_s;
	
	signal(SIGINT,uscendo);
	signal(SIGKILL,uscendo);

// aggancia le code
	if(((qid_to_gr=msgget(k_att_rqsts,0))==-1)  || ((qid_to_proc=msgget(k_att_rspns,0))==-1) || ((qid_figlio_term=msgget(k_att_figlio_term,0))==-1) || ((qid_nuovo_dato=msgget(k_att_nuovo_dato,0))==-1)  || ((qid_nuovo_disponibile=msgget(k_att_nuovo_disponibile,0))==-1)) {
		fprintf (stderr, "[%d]Impossibile connettersi al gestore. Uscendo \n%s\n",getpid(),strerror(errno) );
		exit(0);
	}
	msg_rqst msg;
	msg.p=getpid();
	msg.r=AGGANCIO;
// richiede autorizzazione a GR
	if((msgsnd(qid_to_gr,&msg,sizeof(msg_rqst),0))<0)
		fprintf (stderr, "Errore nell'invio messaggio di aggancio [file] %s [linea] %d\n%s\n",__FILE__,__LINE__,strerror(errno) );
	
// ottiene autorizzazione e porzione da esaminare 
	if(msgrcv(qid_to_proc,msg_to_proc,sizeof(msg_rspns)-sizeof(long),0,0)<=0) {  // da cambiare selezionando pid come type
		fprintf (stderr, "[%d] ricevuto messaggio vuoto [file] %s [linea] %d [expecting type] %ld\n%s\n",getpid(),__FILE__,__LINE__,(long)getpid(),strerror(errno) );
		// SIGNAL TO CODA DEI PROCESSI TOLTI
		exit(0);
	} else {
		shm_id=msg_to_proc->shm_id;
		sem_id=msg_to_proc->sem_id;
		sh_gen_id=msg_to_proc->sh_gen_id;
		
		printf("[%d] YAY! agganciato al GR\n ",getpid());
	}
	if((shm = shmat(shm_id,NULL,0))<0)
		fprintf (stderr, "[%d] Errore agganciando memoria [file] %s [linea] %d [expecting type] %ld\n%s\n",getpid(),__FILE__,__LINE__,(long)getpid(),strerror(errno) );
	if((n_generazioni = shmat(sh_gen_id,NULL,0))<0)
		fprintf (stderr, "[%d] Errore agganciando memoria int [file] %s [linea] %d [expecting type] %ld\n%s\n",getpid(),__FILE__,__LINE__,(long)getpid(),strerror(errno) );
	
	
	printf("\ninitial pop: %d \n",get_population(shm));
	
	for(y=0;y<N_Y;++y) {
		for(x=0;x<N_X;++x) {
			if(get_stato(x,y,shm)==ICS) printf("[%d,%d] %d ",x,y);
		}
	}
	
	
	
	printf("Rule S:\t");
	int kk;
	for(kk=0;kk<size_s;++kk)
		printf("%d ",rule_s[kk]);
	printf("\nRule B:\t");
	
	for(kk=0;kk<size_b;++kk)
		printf("%d ",rule_b[kk]);
	
	printf("fatto\n");
	
	
	
	
	
	while(1) {

//		evolvi_life(shm,local_m,n_generazioni);
		//evolvi(shm,local_m,n_generazioni,&rule_s,size_s,&rule_b,size_b);
		sierpinski(shm,local_m,n_generazioni);
		/*if(!check(local_m,shm)) {
			printf("\n\n\n Situazione di stallo\n\n");
			sleep(2);
			init_matrix(shm,n_generazioni);
		}
*/
	}
	

	uscendo(1);
}