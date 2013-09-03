#ifndef _HEADER_
#define _HEADER_
#include "header.h"
#endif

void main(int argc, char* argv[]) {
	
	lista l=new_nodo(getpid(),time(NULL));
	ins_in_coda(&l,getpid(),time(NULL));
	ins_in_coda(&l,getpid(),time(NULL));
	ins_in_coda(&l,getpid(),time(NULL));
	ins_in_coda(&l,getpid(),time(NULL));
	ins_in_coda(&l,getpid(),time(NULL));
	ins_in_coda(&l,5,time(NULL));
	ins_in_coda(&l,getpid(),time(NULL));
	ins_in_coda(&l,getpid(),time(NULL));
	ins_in_coda(&l,getpid(),time(NULL));
	ins_in_coda(&l,5,time(NULL));
	sleep(2);
	time_t t=time(NULL);
	ins_in_coda(&l,getpid(),time(NULL));
	ins_in_coda(&l,getpid(),time(NULL));
	ins_in_coda(&l,getpid(),time(NULL));
	ins_in_coda(&l,getpid(),time(NULL));
	ins_in_coda(&l,getpid(),time(NULL));
	ins_in_coda(&l,getpid(),time(NULL));
	print_list(l);
	sleep(1);
	printf("rimuovo %d\n",(int)t);
	remove_by_time(&l,t);
	if(remove_one_pid(&l,5)==OK) printf("OK");
	print_list(l);
	/*
	ins_in_coda(l,getpid(),time(NULL));
	
	ins_in_coda(l,getpid(),time(NULL));
	
	ins_in_coda(l,getpid(),time(NULL));*/
	
//	stampa_lista(l);
	//processo* l=add_processo(NULL,getpid(),time());
	//processo* m=add_processo(l,getpid(),time());
	//print_list(l);
	
}
