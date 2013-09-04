#ifndef _HEADER_
#define _HEADER_
#include "header.h"
#endif


int start(char * filename, short unsigned * m, short* size_b, 
	short* size_s, short rule_b[8], short rule_s[8])
{
	char c;
	FILE * f;
	int ret;
	

	if( !(f = fopen(filename,"r")))
	{
		fprintf(stderr,"fopen\n%s\n",strerror(errno));
	}


	switch(	c = fgetc(f) )
	{
		case '#':
			ret=commento(f,m,size_b,size_s,rule_b,rule_s);
			break;
		case 'x':
			ret=leggi_rule(f,m,size_b,size_s,rule_b,rule_s);
			break;
		default:
			printf("Letto %c\n",c);
			ret = -1;
			break;
	}

	if( fclose(f) != 0 )
	{
		fprintf(stderr,"fclosen\n%s\n",strerror(errno));
	}
	return ret;
}

int  commento(FILE * f, short unsigned * m, short* size_b, short* size_s,
 	short* rule_b, short* rule_s)
{
	char c;
	int max = 120;
	char *s = (char *) malloc(sizeof(char) * max);
	
	do {
		fgets(s,max,f);
		printf("#%s\n", s);
	}	while( (c = fgetc(f)) == '#' );

	if ( c == 'x' )
	{
		return leggi_rule(f,m,size_b,size_s,rule_b,rule_s);
		
	} else
	{
		printf("Errore letto %c  al posto di x\n",c);
		return -1;
	}
	
}

int decodifica_rule(char * s,short* size_b, short* size_s, short* rule_b,
	 short* rule_s)
{	// nella forma Bxy/Sklm
	int i=0;
	*size_s = 0;
	*size_b = 0;
	
	if ( s[0]!= 'B')
	{
		printf("Letto %c al posto di B\n",s[0]);
		return -1;
	}
	while( s[++i]!='/' )
	{	
		rule_b[(*size_b)++]=s[i]-'0';	
	}
	while( !isdigit(s[++i])) ; // avanza la S
	while( isdigit(s[i]))
	{
		rule_s[(*size_s)++] = s[i++] - '0';
	}
	

	return (*size_s + *size_b);
}


int leggi_rule(FILE * f, short unsigned * m, short* size_b, short* size_s,
	 short* rule_b, short* rule_s)
{

 	char rule_letta[80];
 	int n_x, n_y;
 	int sizes;

 	if (fscanf(f, " = %d, y = %d, rule = %80s ",&n_x,&n_y,rule_letta)
		 < 3)
 	{
 		perror("letti meno elementi\n");
		return -1;
 	}
 	if(n_x > N_X || n_y > N_Y)
	{
		perror("pattern troppo grande");
		return -1;
	}
	sizes=decodifica_rule(rule_letta,size_b,size_s,rule_b,rule_s);
	if( (sizes<2) || (sizes>16) )
	{
		perror("regole S/P non valide");
		return -1;
	}


 	return( prepara_matrice(f,m,n_x,n_y) );
}


int prepara_matrice(FILE * f, short unsigned * m,int n_x, int n_y)
{
	int i;
	int line=0, col=0;
	char c, d;
	int diff_x = (N_X-n_x)/2;
	int diff_y = (N_Y-n_y)/2;
	
	for(i=0; i<N_X*N_Y; ++i)
	{
		m[i]=0;
	}

	while( c!='!' &&  ( c=fgetc(f) )!= EOF)
	{
		if( isdigit(c) )
		{
			d = fgetc(f);
			
			if( 'o' == d )
			{
				for(i=0; i<(c-'0'); ++i)
				{
					fiat(diff_x+ col++,diff_y + line,m); 
				}	
			} else {
				for(i=0; i<(c-'0'); ++i)
				{
					uccidi(diff_x + col++,
						diff_y + line,m);  
				}
			}
		}
		if( c == 'o' )
		{
			fiat(diff_x + col++, diff_y + line,m);
		}
		if( c== 'b' )
		{	
			uccidi( diff_x + col++,diff_y + line,m);
		}
		if( c== '$' )
		{
			if(col < n_x)
			{
				for(col; col<n_x; ++col)
				{
					uccidi( diff_x + col,diff_y
							+ line,m);
				}
			}
			col = 0;
			printf("\n");
			++line;
		}
	}
	


	return 0;
}

