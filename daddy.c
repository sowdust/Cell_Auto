#ifndef _HEADER_
#define _HEADER_
#include "header.h"
#endif

int	n, i;
pid_t	p, q;
char	c;
short	rule_index;	// passata come argomento al figlio
char *  argv_exec[3];
char *  arg_due;

void menu(int);

void main(int argc, char* argv[])
{
	signal(SIGINT,menu);
	signal(SIGHUP,menu);
	arg_due = (char*) malloc(sizeof(char)*2);

	//	come input si puo' dare il numero di processi cooperanti
	if(argc<2)	n=N_PROC_DEFAULT;
	else		n=atoi(argv[1]);

	menu(0);
}

void menu(int a)
{
	do{

		//	stampa menu
		printf("Selezionare il tipo di automa da provare\n");
		printf("[1]\tConway's Game of Life\n");
		printf("[2]\t34Life\n");
		printf("[3]\tHigh Life\n");
		printf("[4]\tDay and Night\n");
		printf("[5]\tEmulatore automa unidimensionale triangolo di Sierpinski\n");
		printf("[6]\tPattern casuale da file\n");
		printf("[0]\tUscire\n");

		if( (c = getchar()) == '0' ) exit(EXIT_SUCCESS);
	
		//	trasforma la scelta in una stringa
		//	e sottrae uno 
		arg_due[0]= c;
		arg_due[1] = random_in_range(1,RLE_FILES_NUMBER+1);
		arg_due[0]='\0';

		argv_exec[0] = "./figlio";
		argv_exec[1] = arg_due;
		argv_exec[2] =	NULL;

		printf("[%d]Processo generatore. Forkera' %d figli\n",getpid(),n);
	
		i=0;
		while( ++i <= n)
		{
			if( ( p=fork())  < 0) {
				fprintf (stderr, "Errore nell fork\n%s\n",
			strerror(errno) );
			exit(ERROR);
			}
			if(!p)
			 {
				if(execve(argv_exec[0],argv_exec,NULL)==-1) {
					fprintf (stderr, "Errore nell execve\n%s\n",
					strerror(errno) );
					exit(ERROR);
				}
			}
		}
	}while( (q = wait(NULL)) >= 0 );
		printf("[padre:%d]: processo #%d terminato\n",getpid(),q);

}
