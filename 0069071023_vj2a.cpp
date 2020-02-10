#include <iostream>
#include <stdio.h>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <pthread.h>
using namespace std;
struct mem{
        int br[30];
        mem *sljedeci;
};
mem *prvi=new mem;
void *unos(void *arg){
        int j=*((int*)arg), f;
        mem *novi=prvi;
        for(int i=0; i<=j; i++){
                f=novi->br[29];
                novi=novi->sljedeci;
        }
        srand(f);
        for(int i=0; i<30; i++) novi->br[i]=rand() % 32768;
        return NULL;
}
void *ispis(void *arg){
        int zbr=0;
        int j=*((int*)arg);
        mem *novi=prvi;
        for(int i=0; i<=j; i++) novi=novi->sljedeci;
        for(int i=0; i<30; i++) zbr += novi->br[i];
        printf("Polje [%d] suma = %d\n", j+1, zbr);
        sleep(1);
        printf("Polje [%d] aritm_sredina = %.5f\n", j+1, ((float)zbr/(float)30));
        return NULL;
}
int main(int argc, char *argv[]){
        system("clear");
        if(argc != 2 && cout << "Argumenti nisu dobro uneseni. Unesi samo jedan." << endl) exit(0);
        int m = atoi(argv[1]);
        srand(time(NULL));
        prvi->br[29]=rand();
        for(int i=0; i<m; i++){
                mem *novi=new mem;
                novi->sljedeci=prvi->sljedeci;
                prvi->sljedeci=novi;
        }
        int n[m];
        pthread_t tid[m];
        for(int i=0; i<m; i++){
                n[i]=i;
                pthread_create(&tid[i], NULL, unos, &n[i]);
                pthread_create(&tid[i], NULL, ispis, &n[i]);
        }
        for(int i=0; i<m; i++) pthread_join(tid[i], NULL);
        return 0;
}

