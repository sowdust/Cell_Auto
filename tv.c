#ifndef _HEADER_
#define _HEADER_
#include "header.h"
#endif



void riparti(int);
void esci(int);



int qid_to_gr,qid_to_proc,qid_figlio_term;
int shm_id,sem_id,sh_gen_id;
short unsigned* shm;
int* n_generazioni;
int inuso = 0;
pid_t pid_gr;




void riparti(int s)
{
	char* string="Big bang in corso...";
	int row, col;
	getmaxyx(stdscr,row,col);
	if(inuso) inuso=V(sem_id,0);

	clear();
	mvprintw(row/2,(col-strlen(string))/2,"%s",string);
	refresh();
	if(inuso)	inuso=V(sem_id,0);
	// getppid() non funzionava
	if(kill(pid_gr,SIGQUIT)!=0)
	{
		fprintf(stderr,"[%d]:Errore invio kill \n%s\n",
		getpid(),strerror(errno) );	

	}
	signal(SIGQUIT,riparti);
}


void esci(int s)
{
	char * string;
	int row, col;
	getmaxyx(stdscr,row,col);
	
	clear();
	mvprintw(row/2,(col-strlen(string))/2,"Addio");
	refresh();
	msg_rqst msg2;
	msg2.p=getpid();
	msg2.r=ADDIO;
	msg2.type=ADDIO;
	if ((msgsnd(qid_figlio_term,&msg2,sizeof(msg_rqst)-sizeof(long),0)) < 0)
	{
		fprintf(stderr,"[%d]:Errore avviso GR terminazione \n%s\n",
		getpid(),strerror(errno) );
	}
	endwin();
	exit(EXIT_SUCCESS);

}

void main(int argc, char* argv[])
{

	key_t k_att_rqsts=ftok(FTOK_PATH,'A');
	key_t k_att_rspns=ftok(FTOK_PATH,'B');
	key_t k_att_figlio_term=ftok(FTOK_PATH,'E');
	
	msg_rspns* msg_to_proc = (msg_rspns*) malloc(sizeof(msg_rspns));
	msg_rqst* msg_nuovo_dato = (msg_rqst*) malloc(sizeof(msg_rqst));
	msg_rqst msg;
	
	char ch;
	char* string;
	
	short vivi;

	int population, tot;
	int row, col, x, y;


	//	Gestione segnali
	
	signal(SIGINT,esci);
	signal(SIGQUIT,riparti);


	//	Aggancio delle code

	if(	((qid_to_gr=msgget(k_att_rqsts,0))==-1) 
		|| ((qid_to_proc=msgget(k_att_rspns,0))==-1)
		|| ((qid_figlio_term=msgget(k_att_figlio_term,0))==-1)	)
	{
		fprintf(stderr, "[%d]Impossibile connettersi al gestore.\n%s\n",
		getpid(),strerror(errno) );
		esci(0);
	}

	//	Invio richiesta di aggancio al GR

	msg.p=getpid();	msg.r=AGGANCIO;
	if((msgsnd(qid_to_gr,&msg,sizeof(msg_rqst)-sizeof(long),0))<0)
	{
		fprintf(stderr, "Errore nell'invio messaggio di aggancio\n%s\n",
		strerror(errno) );
		esci(0);
	}



	if( msgrcv(qid_to_proc,msg_to_proc,sizeof(msg_rspns)-sizeof(long),0,0 )
		<= 0 )
	{
		fprintf (stderr, "[%d] ricevuto messaggio vuoto\n%s\n",
		getpid(),strerror(errno) );
		esci(0);
	} else
	{
		shm_id=msg_to_proc->shm_id;
		sem_id=msg_to_proc->sem_id;
		sh_gen_id=msg_to_proc->sh_gen_id;
		pid_gr = msg_to_proc->pid_gr;

		printf("[%d] Agganciato al GR\n",getpid());
	}

	//	Aggancio memoria condivisa
	
	shm = shmat(shm_id,NULL,0);
	n_generazioni = shmat(sh_gen_id,NULL,0);



	//	Avvio Ncurses
	
	initscr();	// avvia la sessione
	cbreak();	// disabilita line buffering ma non se causano segnali
	keypad(stdscr, TRUE);	// f1, f2.. per ora inutilizzato
	noecho();	// intuitivo
	start_color();	// lapalissiano
	curs_set(0);	// disabilita cursore

	init_pair(1, COLOR_BLACK, COLOR_CYAN);
	init_pair(2, COLOR_WHITE, COLOR_BLUE);
	init_pair(3, COLOR_WHITE, COLOR_BLACK);

	getmaxyx(stdscr,row,col);
	string=" The Game of Life ";

	attron(A_STANDOUT);
	mvprintw(row/2,(col-strlen(string))/2,"%s",string);
	attroff(A_STANDOUT);

	refresh();	
	sleep(3);
	clear();


	while(1) {
		attron(COLOR_PAIR(1));
		mvprintw(0,0,"Premere Ctrl+\\ per azzerare la matrice");
		attroff(COLOR_PAIR(1));
		tot=0;
		population=0;
		attron(COLOR_PAIR(2));

		inuso = P(sem_id,0);

		for(y=0;y<N_Y;++y)
		{
			//	spostati al centro a sinistra
			move(4+y,(col-N_X)/2);

			for(x=0;x<N_X;++x)
			{
				if(get_stato(x,y,shm)==VIVO)
				{
					addch(ACS_DIAMOND);
					++population;
				} else {
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
		mvprintw(1,(col-strlen(string))/2,
			"Popolazione: %d su %d ",population,tot);
		mvprintw(2,(col-strlen(string))/2," Generazione: %d ",
			*n_generazioni);
		attroff(COLOR_PAIR(3));

	}

}
