#ifndef _HEADER_
#define _HEADER_
#include "header.h"
#endif


int qid_to_gr,qid_to_proc,qid_nuovo_dato,qid_nuovo_disponibile,qid_figlio_term;
int shm_id,sem_id,sh_gen_id;
short unsigned** shm;
int* n_generazioni;
int inuso = 0;

void riparti(int s) {
	char* string;
	int row;
	int col;
	getmaxyx(stdscr,row,col);
	if(inuso) inuso=V(sem_id,0);
	
	clear();
	string="Inizializzazione buffer...";
	clear();
	mvprintw(row/2,(col-strlen(string))/2,"%s",string);
	refresh();
	sleep(1);
	if(inuso)	inuso=V(sem_id,0);
	inuso=P(sem_id,0);
	init_matrix(shm,n_generazioni);
	inuso=V(sem_id,0);
}

void esci(int s) {
	char * string;
	int row;
	int col;
	getmaxyx(stdscr,row,col);
	
	string="addio";
	clear();
	mvprintw(row/2,(col-strlen(string))/2,"%s",string);
	refresh();
	msg_rqst msg2;
	msg2.p=getpid();
	msg2.r=ADDIO;
	msg2.type=ADDIO;
	if ((msgsnd(qid_figlio_term,&msg2,sizeof(msg_rqst),0))<0)
		fprintf (stderr, "[%d]:Impossibile contattare GR prima di terminazione \n%s\n",getpid(),strerror(errno) );
	
	endwin();
	exit(EXIT_SUCCESS);
	

}

void main(int argc, char* argv[]) {

	key_t k_att_rqsts=ftok(FTOK_PATH,'A');
	key_t k_att_rspns=ftok(FTOK_PATH,'B');
	key_t k_att_nuovo_dato=ftok(FTOK_PATH,'C');
	key_t k_att_nuovo_disponibile=ftok(FTOK_PATH,'D');
	key_t k_att_figlio_term=ftok(FTOK_PATH,'E');
	short vivi;
	
	msg_rspns* msg_to_proc = (msg_rspns*)malloc(sizeof(msg_rspns));
	msg_rqst* msg_nuovo_dato = (msg_rqst*)malloc(sizeof(msg_rqst));
	
	char ch;
	char* string;
	int row, col, x, y, cur_line;
	stato c_s;
	int population, tot;
	int color;

// aggancia le code
	if(((qid_to_gr=msgget(k_att_rqsts,0))==-1)  || ((qid_to_proc=msgget(k_att_rspns,0))==-1) || ((qid_figlio_term=msgget(k_att_figlio_term,0))==-1)) {
		fprintf (stderr, "[%d]Impossibile connettersi al gestore. Uscendo \n%s\n",getpid(),strerror(errno) );
		exit(0);
	}
	msg_rqst msg;
	msg.p=getpid();
	msg.r=AGGANCIO;
// richiede autorizzazione a GR
	if((msgsnd(qid_to_gr,&msg,sizeof(msg_rqst),0))<0)
		fprintf (stderr, "Errore nell'invio messaggio di aggancio [file] %s [linea] %d\n%s\n",__FILE__,__LINE__,strerror(errno) );
	
	
	signal(SIGINT,esci);
	signal(SIGKILL,esci);
	signal(SIGQUIT,riparti);

	if(msgrcv(qid_to_proc,msg_to_proc,sizeof(msg_rspns)-sizeof(long),0,0)<=0) {  // da cambiare selezionando pid come type
		fprintf (stderr, "[%d] ricevuto messaggio vuoto [file] %s [linea] %d [expecting type] %ld\n%s\n",getpid(),__FILE__,__LINE__,(long)getpid(),strerror(errno) );
		// SIGNAL TO CODA DEI PROCESSI TOLTI
		exit(0);
	} else {
		shm_id=msg_to_proc->shm_id;
		sem_id=msg_to_proc->sem_id;
		sh_gen_id=msg_to_proc->sh_gen_id;

		printf("[%d] Agganciato al GR\n",getpid());
	}
	
	
		shm = shmat(shm_id,NULL,0);
		n_generazioni = shmat(sh_gen_id,NULL,0);


		
//		NCURSES 
	initscr();
	cbreak();			// l ine buffering disabilitato ma con segnali
	keypad(stdscr, TRUE);		// f1, f2..
	noecho();
	start_color();
	curs_set(0);			// disabilita cursore

	init_pair(1, COLOR_BLACK, COLOR_CYAN);
	init_pair(2, COLOR_WHITE, COLOR_BLUE);
	init_pair(3, COLOR_WHITE, COLOR_BLACK);
	
	getmaxyx(stdscr,row,col);		/* get the number of rows and columns */
	string=" The Game of Life ";

	attron(A_STANDOUT);
	mvprintw(row/2,(col-strlen(string))/2,"%s",string);
	attroff(A_STANDOUT);

	refresh();			
	sleep(2);
	clear();


	while(1) {
		attron(COLOR_PAIR(1));
		mvprintw(0,0,"Premere \\ per azzerare la matrice");
		attroff(COLOR_PAIR(1));
		tot=0;
		population=0;
		attron(COLOR_PAIR(2));
		
		inuso = P(sem_id,0);
		
		for(y=0;y<N_Y;++y) {
			move(4+y,(col-N_X)/2); //			move(4+y,(col-N_X)%2);

			for(x=0;x<N_X;++x) {
				if(get_stato(x,y,shm)==VIVO) {
					addch(ACS_DIAMOND);
					++population;
				}else{
					addch(' ');
				}
				refresh();
				++tot;
			}
			printw("\n");
		}
		inuso = V(sem_id,0);
		
		attroff(COLOR_PAIR(2));
		attron(COLOR_PAIR(3));
		mvprintw(1,(col-strlen(string))/2," Popolazione: %d su %d ",population,tot);
		mvprintw(2,(col-strlen(string))/2," Generazione: %d ",*n_generazioni);
		attroff(COLOR_PAIR(3));

	}

}


