#ifndef _HEADER_
#define _HEADER_
#include "header.h"
#endif

lista new_nodo(pid_t p,time_t t) {
      lista l=(lista)malloc(sizeof(struct nodo));
      l->p=p;
      l->t=t;
      l->nxt=NULL;
      return l;
}

status ins_in_coda(lista* l,pid_t p,time_t t) {
    if((*l)==NULL) {
      if(NULL==((*l)=new_nodo(p,t)))
          return ERROR;
      return OK;
    }
    ins_in_coda(&((*l)->nxt),p,t);
    return OK;
}

void print_list(lista l) {
	printf("Lista dei processi agganciati al GR:\n");
	if(l==NULL)   	printf("Lista vuota.\n");
 	else {
 		printf("[pid] %d \t [time] %d\n",l->p,(int)l->t);
 		if(l->nxt!=NULL)
 		printa_list(l->nxt);
 	}
}
printa_list(lista l) {
	if(l==NULL)   	printf("Lista vuota.\n");
 	else {
 		printf("[pid] %d \t [time] %d\n",l->p,(int)l->t);
 		if(l->nxt!=NULL)
 		printa_list(l->nxt);
 	}
}

status remove_by_pid(lista* l,pid_t p) {
	if((*l)==NULL)	return ERROR;
 	if((*l)->p==p) {
		lista temp= (*l);
	   	(*l)=(*l)->nxt;
	        free(temp);
   		return OK;
   	}
 	remove_by_pid(&((*l)->nxt),p);
 	return OK;
}

status remove_lista(lista* l) {
	if((*l)==NULL)	return OK;
 	lista temp= (*l);
	(*l)=(*l)->nxt;
	free(temp);
	remove_lista(&((*l)->nxt));
   	return OK;
}

