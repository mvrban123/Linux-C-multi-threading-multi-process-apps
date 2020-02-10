#include <iostream>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <malloc.h>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <unistd.h>
#include <iomanip>
#include <sys/wait.h>

using namespace std;

typedef struct PODACI
{
	int INPUT, OUTPUT, M[5];
} mem;

mem *dijeljeno;
int shmId, N, K, puno = 0, puni = 1, prazno = 2, SemId;

void SemGet(int n)
{
	SemId = semget(IPC_PRIVATE, n, 0600);
	
	if (SemId == -1)
	{
		cout << "Nema semafora!" << endl;
		exit(1);
	}
}

int SemSetVal(int SemNum, int SemVal)
{
	return semctl(SemId, SemNum, SETVAL, SemVal);
}

int SemOp(int SemNum, int SemOp)
{
	struct sembuf SemBuf;
	SemBuf.sem_num = SemNum;
	SemBuf.sem_op = SemOp;
	SemBuf.sem_flg = 0;
	return semop(SemId, &SemBuf, 1);
}

void SemRemove(void)
{
	(void) semctl(SemId, 0, IPC_RMID, 0);
}

void Proizvodac(int j)
{
	srand(getpid());
	int i = 0;
	for (i = 0; i < K; i++)
	{
		SemOp(puno, -1);
		SemOp(puni, -1);
		(*dijeljeno).M[(*dijeljeno).INPUT] = rand() % 1000;
		cout << "Proizvodac " << j << " salje " << (*dijeljeno).M[(*dijeljeno).INPUT] << endl;
		sleep(1);
		(*dijeljeno).INPUT = ((*dijeljeno).INPUT + 1) % 5;
		SemOp(puni, 1);
		SemOp(prazno, 1);
	}
	cout << "Proizvodac " << j << " done " << endl;
	exit(0);
}

void Potrosac()
{
	int suma = 0;
	int i = 0;
	for (i = 0; i < (N * K); i++)
	{
		SemOp(prazno, -1);
		cout << "Potrosac prima " << (*dijeljeno).M[(*dijeljeno).INPUT] << endl;
		sleep(1);
		suma += (*dijeljeno).M[(*dijeljeno).OUTPUT];
		(*dijeljeno).OUTPUT = ((*dijeljeno).OUTPUT + 1) % 5;
		SemOp(puno, 1);
	}
	cout << "Potrosac - primljeno = " << suma << endl;
	exit(0);
}

void brisi(int sig)
{
	(void) shmdt((char *) dijeljeno);
	(void) shmctl(shmId, IPC_RMID, NULL);
	exit(0);
}

int main(int arg_count, char *args[])
{
	if (arg_count != 3 || atoi(args[1]) < 1)
	{
		cout << "Krivi parametri! Try again\n";
		return 0;
	}
	
	N = atoi(args[1]);
	K = atoi(args[2]);
	sigset(SIGINT, brisi);
	shmId = shmget(IPC_PRIVATE, sizeof(mem), 0600);
	if (shmId == -1)
	{
		cout << "No memory!" << endl;
		exit(1);
	}
	
	dijeljeno = (mem *) shmat(shmId, NULL, 0);
	(*dijeljeno).INPUT = 0;
	(*dijeljeno).OUTPUT = 0;
	SemGet(3);
	SemSetVal(puni, 1);
	SemSetVal(puno, 5);
	SemSetVal(prazno, 0);
	
	for (int k = 0; k <= N; k++)
	{
		if (fork() == 0)
		{
			if (k == 0) Potrosac();
			else Proizvodac(k);
		}
	}
	for (int i = 0; i <= N; i++)
	{
		wait(NULL);
	}
	semctl(SemId, 0, IPC_RMID, 0);
	brisi(0);
	
	return 0;
}

