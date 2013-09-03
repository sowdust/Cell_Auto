

void init_blinkers(short unsigned* m) {
	int i;
	int j=1;
	for(i=0;i<N_X*N_Y-1;++i) {
		if(i==j+2+N_X+N_Y || i==j+3+N_X+N_Y || j+i==4+N_X+N_Y)
			m[i]=1;
		else
			m[i]=0; ++j;
	}

}
