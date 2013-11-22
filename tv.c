#ifndef _HEADER_
#define _HEADER_
#include "header.h"
#endif



void riparti(int);
void esci(int);
void init_manually (int, int, int);



int qid_to_gr,qid_to_proc,qid_figlio_term;
int shm_id,shm_id_copy,sem_id,sh_gen_id,sem_id_counter;
short unsigned* shm;
short unsigned* shm_copy;
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
	
	clear();
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
	if(kill(pid_gr,SIGINT)!=0 && kill(pid_gr,SIGTERM)!=0)
	{
		fprintf(stderr,"[%d]:Errore invio kill \n%s\n",
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
	
	signal(SIGTERM,esci);
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
		shm_id_copy=msg_to_proc->shm_id_copy;
		sem_id_counter=msg_to_proc->sem_id_counter;
		sem_id=msg_to_proc->sem_id;
		sh_gen_id=msg_to_proc->sh_gen_id;
		pid_gr = msg_to_proc->pid_gr;

		printf("[%d] Agganciato al GR\n",getpid());
	}

	//	Aggancio memoria condivisa
	
	shm = shmat(shm_id,NULL,0);
	shm_copy = shmat(shm_id_copy,NULL,0);
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
	//string=" The Game of Life ";

	//attron(A_STANDOUT);
	//mvprintw(row/2,(col-strlen(string))/2,"%s",string);
	//attroff(A_STANDOUT);

	refresh();	
	clear();


	if( get_init_choice(col,row) )
	{
		clean_matrix(shm,n_generazioni);
		init_manually(col,y,x);
	}

	clear();
	refresh();

//	MATRIX EVOLUTION (GESTORE)
	while( 1 )
	{
		R(sem_id_counter,0,N_PROC_DEFAULT);
		attron(COLOR_PAIR(1));
		mvprintw(0,0,"Premere Ctrl+\\ per azzerare la matrice");
		attroff(COLOR_PAIR(1));
		attron(COLOR_PAIR(2));
		tot=0;
		population=0;
		
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
					fiat(x,y,shm_copy);
				} else {
					addch(' ');
					uccidi(x,y,shm_copy);
				}
				refresh();
				++tot;
			}
			printw("\n");
		}
		inuso = V(sem_id,0);

		attroff(COLOR_PAIR(2));
		attron(COLOR_PAIR(3));
		mvprintw(1,(col-50)/2,
			"Popolazione: %d su %d ",population,tot);
		mvprintw(2,(col-50)/2," Generazione: %d ",
			(*n_generazioni)++);
		attroff(COLOR_PAIR(3)); 

		Z(sem_id_counter,0); 
	}

}

void init_manually (int col, int y, int x) 
{
	clear();
	refresh();
	WINDOW *evolving_win;
	int highlight_x = 0;
	int highlight_y = 0;

	int evolving = 0;
	int c = 0;

	evolving_win = newwin(N_Y, N_X, 4, (col-N_X)/2 );
	keypad(evolving_win, TRUE);
	box(evolving_win, 0, 0);
	
	// stampa istruzioni
	mvprintw(0,0,"Muoversi con le frecce all interno della griglia");
	mvprintw(1,0,"Premere lo spazio per cambiare stato a una cella");
	mvprintw(2,0,"Premere Invio per dare inizio all' evoluzione");
	mvprintw(3,0,"Premere Esc per uscire");
	refresh();



//	MATRIX INITIALIZATION (USER)

	while( c != 10 ) // 10 := ENTER
	{
		c = getch();	
		mvwprintw(evolving_win,highlight_y,highlight_x," ");

		switch(c)
		{
			case KEY_UP:
				if(highlight_y==0)	highlight_y=N_Y-1;
				else				--highlight_y;
				break;
			case KEY_DOWN:
				if(highlight_y==N_Y-1)	highlight_y=0;
				else					++highlight_y;
				break;
			case KEY_RIGHT:
				if(highlight_x==N_X-1)	highlight_x=0;
				else					++highlight_x;
				break;
			case KEY_LEFT:
				if(highlight_x==0)	highlight_x=N_X-1;
				else				--highlight_x;
				break;
			case 32:	// backspace
				if(get_stato(highlight_x,highlight_y,shm) == VIVO)
					uccidi(highlight_x,highlight_y,shm);
				else
					fiat(highlight_x,highlight_y,shm);
				break;
			case 27:	// esc
				esci(0);
			default: break;

		}
		wattron(evolving_win, A_REVERSE);
		mvwprintw(evolving_win,highlight_y,highlight_x," ");
		wattroff(evolving_win, A_REVERSE);
		for(y=0;y<N_Y;++y)
		{
			for(x=0;x<N_X;++x)
			{
				if(get_stato(x,y,shm)==VIVO)
				{
					mvwprintw(evolving_win,y,x,"X");
				}
			}
		}
		wrefresh(evolving_win);
	}

	noecho();	// intuitivo
	curs_set(0);	// disabilita cursore
	clear();
	refresh();
}



int get_init_choice (int col, int y, int x) 
{
	int c;
	clear();
	refresh();

	// stampa istruzioni
	mvprintw(5,5,"Selezionare nella finestra a fianco il tipo di evoluzione");
	mvprintw(7,5,"Scegliere in che modo inizializzare l'universo e premere il tasto corrispondente");
	mvprintw(9,5,"[M] Manuale: selezionare le celle vive da tastiera");
	mvprintw(11,5,"[R] Random: casuale con probabilita' 50%");
	mvprintw(13,5,"In ogni momento, premere Ctrl+C per uscire");
	refresh();

	while( c = getch() ) 
	{
		mvprintw(15,5,"Premuto %d",c);
		switch(c)
		{
			case 114:
				return 0;
				break;
			case 109:
				return 1;
				break;
			default:
				mvprintw(15,5,"Opzione  %c non valida");
				break;
		}
	
	}

	return 0;
}

