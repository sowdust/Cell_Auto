#ifndef _HEADER_
#define _HEADER_
#include "header.h"
#endif


int n_x, n_y;
int size_s = 0;
int size_b = 0;
int rule_s[80];
int rule_b[80];
char rule_letta[80];
short unsigned * m = NULL;

short unsigned * commento(FILE *);
short unsigned * leggi_rule(FILE *);
short unsigned * decodifica_rule(char *);
short unsigned * start(FILE *);
short unsigned * prepara_matrice(FILE *f);

short unsigned * start(FILE * f)
{
	char c;
	short unsigned * m;
	
	c = fgetc(f);

	switch(c)
	{
		case '#':
			m=commento(f);
			break;
		case 'x':
			break;
		case '!':
			break;
		case EOF:
			break;
		default:
			break;
	}
	return m;
}

short unsigned *  commento(FILE * f)
{
	char c;
	int max = 120;
	short unsigned * m;
	char *s = (char *) malloc(sizeof(char) * max);
	
	do {
		fgets(s,max,f);
		printf("#%s\n", s);
	}	while( (c = fgetc(f)) == '#' );

	if ( c == 'x' )
	{
		m=leggi_rule(f);
	} else
	{
		printf("Errore letto %c  al posto di x\n",c);
	}
	
}

short unsigned * leggi_rule(FILE * f)
{
	if (fscanf(f, " = %d, y = %d, rule = %80s ",&n_x,&n_y,rule_letta) < 3)
	{
		perror("letti meno elementi\n");
	}
	decodifica_rule(rule_letta);
	m = (short unsigned *) malloc (sizeof(short unsigned)*n_x*n_y);
	return( prepara_matrice(f) );
}

short unsigned * decodifica_rule(char *s)
{
	char c;
	int r_s;
	int r_b;
	int i=0;
	short unsigned * m;
	
	if ( s[0]!= 'B')
	{
		printf("Letto %c al posto di B\n",c);
	}
	while( s[++i]!='/' )
	{	
		rule_b[size_b++]=s[i]-'0';	
	}
	++i;
	while( s[++i]!='\0' )
	{
		rule_s[size_s++]=s[i]-'0';
	}
	
	return m;
}


short unsigned * prepara_matrice(FILE *f)
{
	int i;
	int line=0, col=0;
	char c, d;

	while( c!='!' &&  ( c=fgetc(f) )!= EOF)
	{
		if( isdigit(c) )
		{
			d = fgetc(f);
			
			if( d='o' )
			{
				for(i=0; i<(c-'0'); ++i)
				{
					fiat(col++,line,m);  
				}	
			} else {
				for(i=0; i<(c-'0'); ++i)
				{
					uccidi(col++,line,m);  
				}			
			}
		}
		if( c == 'o' )
		{
			fiat(col++,line,m);
		}
		if( c== 'b' )
		{
			uccidi(col++,line,m);
		}
		if( c== '$' )
		{	col = 0;
			++line;
		}
	}
	printf("%d %d %d %d \n",line,col,n_y,n_x);
	
	return m;
}

void main(int argc, char* argv[]) {

	FILE * f;
	char * filename;
	char c;
	
	filename = "RLE/c2-0002.lif";
	if( !(f = fopen(filename,"r")))
	{
		;//error
	}
	
	while( m == NULL )
	{
		m = start(f);
	}
	
	if( !fclose(f) )
	{
		;// error
	}
}
