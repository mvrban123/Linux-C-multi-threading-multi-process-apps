#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <signal.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>

using namespace std;

int br=0;
int walls;
pthread_cond_t red;
pthread_mutex_t mon;
sem_t putin;

void wall() 
{
    pthread_mutex_lock(&mon);
    br++;
    if (br<walls) 
	{
        pthread_cond_wait(&red, &mon);
    } 
    else 
	{
        br = 0;
        pthread_cond_broadcast(&red);
        cout<<endl;
    }
    pthread_mutex_unlock(&mon);
}

void *work(void *rb)
{
    int n = *(int*) rb;
    int num;
    
    sem_wait(&putin);
    cout<<"Dretva "<<n+1<<". Unesite broj: ";
    cin>>num;
    sem_post(&putin);
    wall();
    cout<<"Dretva "<<n+1<<". Uneseni broj je "<<num<<endl;
}

int main(int argc, char* argv[]) 
{
	if(argc!=2) 
	{
		printf("Uneseni argumenti nisu dobri!\n");
		return 0;
	}

	for(int i=0; i<argc; i++) 
	{
		if(i==1 && argv[i]>0) walls=atoi(argv[i]);
	}

	if(walls<=0) 
	{
		printf("Krivi unos!\n");
		return 0;
	}
	
    sem_init(&putin, 0, 1);
    int field[walls];
    cout<<"Broj dretvi = "<<walls<<endl;
    pthread_t drtv[walls];
    
    for (int i=0; i<walls; i++) 
	{
        field[i]=i;
        pthread_create(&drtv[i], NULL, work, &field[i]);
    }
    for (int i = 0; i < walls; i++)
	{
        pthread_join(drtv[i], NULL);
    }

    return 0;
}
