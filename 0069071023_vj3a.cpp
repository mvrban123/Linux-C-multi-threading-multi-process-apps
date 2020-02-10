#include <iostream>
#include <cstdlib>
#include <csignal>
#include <ctime>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>

using namespace std;

#define LIJEVA_STRANA 0
#define DESNA_STRANA 1
#define M 0
#define LJ 1

short brod_strana = 0;
int broj[2][2] = {(0, 0), (0, 0)}, br_putnika = 0, brod_ljudozder = 0, brod_misionar = 0;

sem_t k_o, nutra, van, s[2][2], brod;
pthread_t dretva, brod_dretva;

void *misionar(void *x)
{
	sem_wait(&k_o);
	bool obala = rand() % 2;
	bool usao = 0;
	broj[obala][M]++;
	
	while (!usao) {
		if (obala == brod_strana && br_putnika < 7) {
			usao = 1;
			broj[obala][M]--;
			br_putnika++;
			brod_misionar++;
			
			if (br_putnika == 3)
				sem_post(&nutra);
			if (br_putnika >= 3 && br_putnika < 7 && broj[obala][LJ] > 0)
				sem_post(&s[obala][LJ]);
		}
		
		else {
			sem_post(&k_o);
			sem_wait(&s[obala][M]);
			sem_wait(&k_o);
		}
	}
	
	sem_post(&k_o);
	sem_wait(&brod);
	sem_wait(&k_o);
	
	cout << "Misionar iskrcan" << endl;
	
   	br_putnika--;
	brod_misionar--;
	
	if (!br_putnika)
		sem_post(&van);
	sem_post(&k_o);
}

void *ljudozder(void *x)
{
	sem_wait(&k_o);
	bool obala = rand() % 2;
	bool usao = 0;
	broj[obala][LJ]++;
	
	while (!usao) {
		if (obala == brod_strana && br_putnika < 7 && brod_ljudozder < brod_misionar) {
			usao = 1;
			broj[obala][LJ]--;
			br_putnika++;
			brod_ljudozder++;
			
			if (br_putnika == 3)
				sem_post(&nutra);
			
			if (br_putnika >=3 && br_putnika < 7 && broj[obala][M] > 0)
				sem_post(&s[obala][M]);
		}
		
		else {
			sem_post(&k_o);
			sem_wait(&s[obala][LJ]);
			sem_wait(&k_o);
		}
	}
	
	sem_post(&k_o);
	sem_wait(&brod);
	sem_wait(&k_o);
	
	cout << "Ljudozder iskrcan" << endl;
	br_putnika--;
	brod_ljudozder--;
	
	if (!br_putnika)
		sem_post(&van);
	
	sem_post(&k_o);
}

void *dretva_brod(void *x)
{
	bool obala = rand() % 2;
	while (1)
	{
		sem_wait(&nutra);
		sleep(1);
		obala = !obala;
		
		cout << "Brod krenuo!" << endl;
		cout << ((obala) ? "LIJEVO" : "DESNO");
		cout << " -> ";
		cout << ((obala) ? "DESNO" : "LIJEVO") << endl;
		sleep(2);
		cout << "Stigao na drugu stranu!" << endl;
		
		if (obala) brod_strana = 0;
		else brod_strana = 1;
		
		for (int i = 0; i < 7; i++)
			sem_post(&brod);
		
		sem_wait(&van);
		
		for (int i = 0; i < broj[obala][M]; i++)
			sem_post(&s[brod_strana][M]);
		
		for (int i = 0; i < broj[brod_strana][LJ]; i++)
			sem_post(&s[obala][LJ]);
	}
}

void postaviSemafore()
{
	sem_init(&k_o, 0, 1);
	sem_init(&nutra, 0, 0);
	sem_init(&van, 0, 0);
	sem_init(&s[0][0], 0, 0);
	sem_init(&s[1][0], 0, 0);
	sem_init(&s[0][1], 0, 0);
	sem_init(&s[1][1], 0, 0);
	sem_init(&brod, 0, 0);
}

void unistiSemafore()
{
	sem_destroy(&k_o);
	sem_destroy(&nutra);
	sem_destroy(&van);
	sem_destroy(&s[0][0]);
	sem_destroy(&s[1][0]);
	sem_destroy(&s[0][1]);
	sem_destroy(&s[1][1]);
	sem_destroy(&brod);
}

void izlaz(int sig)
{
	unistiSemafore();
	kill(getpid(), SIGKILL);
}

void suma()
{
	while (1)
	{
		pthread_create(&dretva, NULL, ljudozder, NULL); 
		sleep(1);
		pthread_create(&dretva, NULL, misionar, NULL); 
		pthread_create(&dretva, NULL, ljudozder, NULL); 
		sleep(1);
	}
}

int main()
{
	sigset(SIGINT, izlaz);
	postaviSemafore();
	srand(time(0));
	pthread_create(&brod_dretva, NULL, &dretva_brod, NULL);
	suma();
	
	return 0;
}

