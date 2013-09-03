old
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




void evolvi_sierpinski(short unsigned* shm, short unsigned* local) {
	int x, y;
	short vivi;
	stato c_s;
	
	P(sem_id,0);
	for(y=0;y<N_Y*N_X;++y) {
		local[y]=shm[y];
	}
	for(y=0;y<N_Y;++y) {
		for(x=0;x<N_X;++x) {
			vivi=vicini_vivi(x,y,local);
			c_s=get_stato(x,y,local);
			
			if(c_s==ICS) {
				printf("ma che succede?? [%d,%d]\n",x,y);
				return ;
			}
			
			if(c_s==MORTO) {
				if(vivi==1) {
					fiat(x,y,shm);
					printf("%d %d vivo \n",x,y);
				}
			} else {
				if(vivi<1 || vivi > 2) {
					uccidi(x,y,shm);
					printf("%d %d muoio \n",x,y);
				}
			}
		}
	}
	V(sem_id,0);
}



void evolvi_sierpinskii(short unsigned* shm, short unsigned* local) {
	int x, y;
	short vivi;
	stato c_s;
	
	P(sem_id,0);
	for(y=0;y<N_Y*N_X;++y) {
		local[y]=shm[y];
	}	
	for(y=0;y<N_Y;++y) {
		for(x=0;x<N_X;++x) {
			vivi=vicini_vivi(x,y,local);
			c_s=get_stato(x,y,local);
			
			if(vivi > 2) {
				uccidi(x,y,shm);
				printf("%d %d muoio \n",x,y);
			}
			else if(vivi == 1 && c_s == MORTO) {
				fiat(x,y,shm);
				printf("%d %d vivo \n",x,y);
			}
		}
		
	}
	V(sem_id,0);

}
