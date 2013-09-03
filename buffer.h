#ifndef _HEADER_
#define _HEADER_
#include "header.h"
#endif

void init_buffer(buffer* b);
void init_matrix(short unsigned* m, int* n_gen);
void print_buffer(buffer* b);
int add_to_buffer(buffer* b,int v,pid_t p);
int delete_older_than(buffer* b, time_t t);
void print_matrix(short unsigned* m);
