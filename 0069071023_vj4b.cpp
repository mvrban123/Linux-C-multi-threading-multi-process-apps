#include <stdio.h>
#include <cstdlib>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
using namespace std;
pthread_mutex_t monitor;
pthread_cond_t act, cekaona;
bool otvoreno, gotovaSmjena = false, spava = false;
int br_mjesta = 0;
void *sweeneytodd(void *x) {
        pthread_mutex_lock(&monitor);
        otvoreno = true;
        printf("Brijac: Otvaram salon\n");
        pthread_mutex_unlock(&monitor);
        while(1) {
                pthread_mutex_lock(&monitor);
                if(gotovaSmjena == true) otvoreno = false;
                if(br_mjesta > 0) {
                        printf("Brijac: Idem raditi\n");
                        pthread_cond_signal(&cekaona);
                        pthread_mutex_unlock(&monitor);
                        sleep(2);
                        pthread_cond_wait(&act, &monitor);
                        printf("Brijac: Klijent gotov\n");
                }
                else if(otvoreno == true) {
                        printf("Brijac: Spim dok klijenti ne dodju\n");
                        pthread_mutex_unlock(&monitor);
                        spava = true;
                        pthread_cond_wait(&act, &monitor);
                        spava = false;
                }
                else {
                        printf("Brijac: Zatvaram salon\n");
                        pthread_mutex_unlock(&monitor);
                        break;
                }
                pthread_mutex_unlock(&monitor);
        }
}
void *victim(void *x) {
        int brojKlijenta = *((int*) x);
        pthread_mutex_lock(&monitor);
        printf("Klijent [%d]: Zelim na brijanje\n", brojKlijenta);
        if(otvoreno == true && br_mjesta < 5) {
                br_mjesta++;
                printf("Klijent [%d]: Ulazim u cekaonu [%d]\n", brojKlijenta, br_mjesta);
                if(spava == true) pthread_cond_signal(&act);
                pthread_mutex_unlock(&monitor);
                pthread_cond_wait(&cekaona, &monitor);
                printf("Klijent [%d]: brijac mi radi brijanje\n", brojKlijenta);
                pthread_mutex_lock(&monitor);
                br_mjesta--;
                pthread_cond_signal(&act);
        }
        else printf("Klijent [%d]: Nema mjesta u cekaoni\n", brojKlijenta);
        pthread_mutex_unlock(&monitor);
}
void endme(int sig){
        pthread_mutex_destroy(&monitor);
        pthread_cond_destroy(&cekaona);
        pthread_cond_destroy(&act);
        exit(1);
}
int main(){
        sigset(SIGINT, endme);
        pthread_mutex_init(&monitor, NULL);
        pthread_cond_init(&act, NULL);
        pthread_cond_init(&cekaona, NULL);
        pthread_t brijac;
        pthread_t victims[10];
        pthread_create(&brijac, NULL, sweeneytodd, NULL);
        for(int i=0; i<9; i++){
            pthread_create(&victims[i], NULL, victim, (void*)&i);
            sleep(1);
        }
        gotovaSmjena = true;
        pthread_join(brijac, NULL);
        for(int i = 0; i<7; i++) pthread_join(victims[i], NULL);
        endme(1);
        return 0;
}
