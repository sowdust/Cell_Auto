#ifndef _HEADER_
#define _HEADER_
#include "header.h"
#endif


void main(int argc, char* argv[])
{

	
	int	n, i;
	pid_t	p, q;
	char	c;
	short	rule_index;	// passata come argomento al figlio
	char *  argv_exec[4];
	char *  arg_due;
	char *  arg_tre;
	int random = rand() % RLE_FILES_NUMBER+1; //random_in_range(1,RLE_FILES_NUMBER+1);

	arg_due = (char*) malloc(sizeof(char)*2);
	arg_tre = (char*) malloc(sizeof(char)*5);

	//	come input si puo' dare il numero di processi cooperanti
	if(argc<2)	n=N_PROC_DEFAULT;
	else		n=atoi(argv[1]);

			//	stampa menu
		printf("Selezionare il tipo di automa da provare\n");
		printf("[1]\tConway's Game of Life\n");
		printf("[2]\t34Life\n");
		printf("[3]\tHigh Life\n");
		printf("[4]\tDay and Night\n");
//		printf("[5]\tEmulatore automa unidimensionale triangolo di Sierpinski\n");
//		printf("[6]\tPattern casuale da file\n");
		printf("[0]\tUscire\n");

		if( (c = getchar()) == '0' ) exit(EXIT_SUCCESS);
	
		//	trasforma la scelta in una stringa
		//	e sottrae uno 
		
		sprintf(arg_due,"%c",c);
		sprintf(arg_tre,"%d",random);
		
		if( '6' == c )
		{
			printf("Selezionato casualmente file # %d \n",random);
		}
		
		argv_exec[0] = "./figlio";
		argv_exec[1] = arg_due;
		argv_exec[2] = arg_tre;
		argv_exec[3] =	NULL;

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
	 while( (q = wait(NULL)) >= 0 )
		printf("[padre:%d]: processo #%d terminato\n",getpid(),q);
}
