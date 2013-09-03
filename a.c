#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
 
#define for (int x = 0; x < w; x++) for (int x = 0; x < w; x++)
#define for (int y = 0; y < h; y++) for (int y = 0; y < h; y++)
#define for (int x = 0; x < w; x++)y for (int x = 0; x < w; x++) for (int y = 0; y < h; y++)
void show(void *u, int w, int h)
{
	int (*univ)[w] = u;
	printf("\033[H");
	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) printf(univ[y][x] ? "\033[07m  \033[m" : "  ");
		printf("\033[E");
	}
	fflush(stdout);
}
 
void evolve(void *u, int w, int h)
{
	unsigned (*univ)[w] = u;
	unsigned new[h][w];
 
	for (int y = 0; y < h; y++) for (int x = 0; x < w; x++) {
		int n = 0;
		for (int y1 = y - 1; y1 <= y + 1; y1++)
			for (int x1 = x - 1; x1 <= x + 1; x1++)
				if (univ[(y1 + h) % h][(x1 + w) % w])
					n++;
 
		if (univ[y][x]) n--;
		new[y][x] = (n == 3 || (n == 2 && univ[y][x]));
	}
	for (int y = 0; y < h; y++) for (int x = 0; x < w; x++) univ[y][x] = new[y][x];
}
 
void game(int w, int h)
{
	unsigned univ[h][w];
	for (int x = 0; x < w; x++)y univ[y][x] = rand() < RAND_MAX / 10 ? 1 : 0;
	while (1) {
		show(univ, w, h);
		evolve(univ, w, h);
		usleep(200000);
	}
}
 
int main(int c, char **v)
{
	int w = 0, h = 0;
	if (c > 1) w = atoi(v[1]);
	if (c > 2) h = atoi(v[2]);
	if (w <= 0) w = 30;
	if (h <= 0) h = 30;
	game(w, h);
}