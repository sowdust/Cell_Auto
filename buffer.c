#ifndef _HEADER_
#define _HEADER_
#include "header.h"
#endif

void init_buffer(buffer* b) {
	b->first=0;
	b->n=0;
	b->size=N_ELEMENTI_BUFFER;
}

void ooinit_matrix(short unsigned* m) {
	int i;
	for(i=0;i<N_X*N_Y;++i) {
		if((rand()%50)==0)
			m[i]=1;
		else
			m[i]=0;
	}

}

void oinit_matrix(short unsigned* m) {
	int i;
	for( i=0; i<N_X*N_Y; ++i ) {
		if(i==0) {
			m[i]=1;
		} else {
			m[i]=0;
		}
	}

}


void init_matrix_(short unsigned* m, int* n_gen) {
	*n_gen=0;
	int i;
	for(i=0; i < N_X*N_Y; ++i) {
		if(i==2+N_X+N_Y || i==3+N_X+N_Y || i==4+N_X+N_Y)
			m[i]=1;
		else
			m[i]=0;
	}

}

void init_matrixxx(short unsigned* m,int* n_gen) {
	int i;
	for(i=0;i<N_X*N_Y;i++) {
		if(rand()%3==0)
			m[i]=1;
		else
			m[i]=0;
	}

}

void print_matrix(short unsigned* m) {
	int x,y;
	for(y=0;y<N_Y;++y) {
		for(x=0;x<N_X;++x) {
			printf("%d ",get_stato(x,y,m)==VIVO);
		}
		printf("\n");
	}
}

void print_buffer(buffer* b) {
	int i;
	printf("[Buffer]\t first: %d\t #: %d\t max size: %d\n",b->first,b->n,b->size);
	for(i=0;i<b->n;i++) 
		printf("[%d:%d] %d \t [pid] %d \t [time] %d\n", i+b->first,i,b->buffer[(i+b->first)%b->size].v, b->buffer[(i+b->first)%b->size].p, (int)b->buffer[(i+b->first)%b->size].t);
}

int add_to_buffer(buffer* b,int v,pid_t p) {
	if(b->n >= b->size) { 
		fprintf (stderr, "Buffer pieno.\n%s\n",strerror(errno) );
		return(BUFFER_PIENO);
	}
	int c=(b->first+(b->n++))%b->size;
	b->buffer[c].v=v;
	b->buffer[c].p=p;
	b->buffer[c].t=time(NULL);
	return c;
}

int delete_older_than(buffer* b, time_t t) {
	if(b->buffer[b->first].t >= t) return 0;
	int c=0;
	while(b->buffer[b->first].t<t && c<b->size && b->buffer[b->first].t>0 && b->n>=0)
	{ b->n--; c++; b->first++; }
	return c;
}
