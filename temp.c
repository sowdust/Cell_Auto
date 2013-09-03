void evolvi_life(short unsigned* shm, short unsigned* local,int* n_gen) {
	int x, y;
	short vivi;
	stato c_s;
	
	P(sem_id,0);
	for(y=0;y<N_Y*N_X;++y) {
		local[y]=shm[y];
	}	
	for(y=0;y<N_Y;y++) {
		for(x=0;x<N_X;x++) {
			vivi=vicini_vivi(x,y,local);
			
			switch(get_stato(x,y,local)) {
				case VIVO:
					if(vivi<2 || vivi>3) {
						uccidi(x,y,shm);
					} else {
						fiat(x,y,shm);
					}
					break;
				case MORTO:
					if(vivi == 3) {
						fiat(x,y,shm);
					} else {
						uccidi(x,y,shm);
					}
					break;
				default:
					printf("ma che succede?? [%d,%d]\n",x,y);
					break;
			}
		}
		
	}
	++(*n_gen);
	V(sem_id,0);

}