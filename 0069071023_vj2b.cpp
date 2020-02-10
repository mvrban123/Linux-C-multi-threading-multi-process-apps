#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <cstdlib>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>

using namespace std;

int segm_a;

struct segment{
    int pravo;
    int zastavica[2];
};
segment *sega;

void van(int i, int j){
   sega->pravo = j;
   sega->zastavica[i] = 0;
   sleep(1);
}

void nutra(int i, int j){
    sega->zastavica[i] = 1;
    while(sega->zastavica[j] != 0){
        if(sega->pravo == j){
            sega->zastavica[i] = 0;
            while(sega->pravo == j) continue;
            sega->zastavica[i] = 1;
        }
    }
}

void prov(int x){
 int i = x;
 for(int k = 1; k <= 5; k++){
      nutra(i, 1-i);
      for(int m = 1; m <= 5; m++){
          cout << "Proces: " << i+1 << " K.O. br: " << k << " (" << m << "/5)\n";
          sleep(1);
      }
      van(i, 1-i);
   }
}

void cisti(int sig) {
    (void) shmdt((char*) sega);
    (void) shmctl(segm_a, IPC_RMID, NULL);
    exit(0);
}

int main(){
    segm_a = shmget(IPC_PRIVATE, sizeof(segment), 0600);
    if (segm_a == -1) exit(1);
    sigset(SIGINT, cisti);
    sega = (segment*) shmat(segm_a, NULL, 0);
    int i = 0;
    if (fork() == 0){
        prov(i);
            exit(0);
        }
    i = 1;
    if (fork() == 0){
        prov(i);
            exit(0);
        }
    wait(NULL);
    wait(NULL);
    cisti(0);
    return 0;
}

