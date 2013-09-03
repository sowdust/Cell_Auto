#ifndef _HEADER_
#define _HEADER_
#include "header.h"
#endif

void main(int argc, char* argv[])
{
	int n,i=0;
	pid_t p,q;	

	if(argc<2)	n=N_PROC_DEFAULT;
	else		n=atoi(argv[1]);
	printf("Processo generatore. Forkera' %d figli\n",n);
	
	while( ++i <= n)
	{
		if( ( p=fork())  < 0) {
			fprintf (stderr, "Errore nell fork [file] %s [linea] %d\n%s\n",__FILE__,__LINE__,strerror(errno) );
			exit(ERROR);
		}
		if(!p) {
			if(execve("./figlio",NULL,NULL)==-1) {
				fprintf (stderr, "Errore nell execve [file] %s [linea] %d\n%s\n",__FILE__,__LINE__,strerror(errno) );
				exit(ERROR);
			}
		}
	}
	while( (q = wait(NULL)) >= 0 )
		printf("[padre:%d]: processo #%d terminato\n",getpid(),q);
	
}
