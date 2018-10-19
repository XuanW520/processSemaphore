/*
 ============================================================================
 Name        : processSemaphore.c
 Author      : lingo
 Version     : 
 CreateDate  : 2018����4:23:42
 ============================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include "mysem.h"

#define MY_SHM_ID 9956
#define MY_SEM_KEY1 9957
#define MY_SEM_KEY2 9958
#define BUFF_SIZE 255
#define random(x) (rand()%x)

typedef struct{
	int length;
	int data[BUFF_SIZE];
	int oddNum;
	int evenNum;
}Buff;

void getodd(Buff *buff){
	// buff ����Ϊ1
	for(int i = 0; i < buff->length; i++){
		if(1 == (buff->data[i]%2)){

			printf("getodd num is : %i \n",buff->data[i]);
			// ��������ֵɾ�����������������λ
			for(int j = i; j < buff->length -1; j++){
				buff->data[j] = buff->data[j+1];
			}
			buff->length--;
			break;
		}
	}
}
void countodd(Buff *buff){
	buff->oddNum++;
	printf("Now the oddNum=%i \n",buff->oddNum);
}

void geteven(Buff *buff){
	// buff ����Ϊ1
	for(int i = 0; i < buff->length; i++){
		if(0 == (buff->data[i]%2)){
			printf("geteven num is : %i \n",buff->data[i]);
			// ��������ֵɾ�����������������λ
			for(int j = i; j < buff->length -1; j++){
				buff->data[j] = buff->data[j+1];
			}
			buff->length--;
			break;
		}
	}
}
void counteven(Buff *buff){
	buff->evenNum++;
	printf("Now the evenNum=%i \n",buff->evenNum);
}
void produce(Buff *buff){
	int temp = random(10);
	buff->data[buff->length] = temp;
	buff->length++;
	printf("produce a num : %i \n",temp);
	sleep(1);
}
int main(int arg,char* argv[]){
	Buff *buff;
	pid_t pid;
	int shmid,empty,full;
	//�����ڴ�
	shmid = shmget(MY_SHM_ID,sizeof(Buff),0666|IPC_CREAT);
	// ����һ���ź��� ��ͬkey��Ӧ��ͬ�ź���
	empty = semget(MY_SEM_KEY1,1,0666|IPC_CREAT);
	full = semget(MY_SEM_KEY2,1,0666|IPC_CREAT);
	if(shmid == -1 || empty == -1 || full == -1){
		printf("error semget / shmget");
	}
	// ��ʼ��empty�ź���val��ֵ ��P(empty) empty-- ��0ʱ��������  ���̴��ڵȴ�״̬ �ȴ���һ������ִ��V(empty)
	init_sem(empty,10);
	// ��ֵΪ0 ����P(full)����wait״̬ �ȴ���һ������ִ��V(full) ����
	init_sem(full,0);

	// ӳ�乲���ڴ�
	buff = (Buff*)shmat(shmid,NULL,0);
	// ��ʼ������
	buff->length = 0;
	buff->oddNum = 0;
	buff->evenNum = 0;


	// ���������ӽ��� p1 produce p2 getodd  p3 geteven
	int i;
	for(i = 0; i < 3; i++){
		pid = fork();
		if(pid == 0 || pid < 0) break;
	}
	if(pid < 0){
		printf("fork error");
	}else if(0 == pid){
		if(i==0){
			printf("Child process P1 is run,processid is %d,parentid is %i  empty = %i full = %i\n",getpid(),getppid(),empty,full);
			// �������� С�ڵ���empty.val ������������ ����������ʱ���ڱ���״̬
			for(int j = 0; j < 8; j++){
				P(empty);
				produce(buff);
				V(full);
			}
		}
		if(i==1){
			printf("Child process P2 is run,processid is %d,parentid is %i  empty = %i full = %i\n",getpid(),getppid(),empty,full);
			for(int j = 0; j < 2; j++){
				P(full);
				geteven(buff);
				counteven(buff);
				printf("Now buff'length = %i\n",buff->length);
				V(empty);
			}
		}
		if(i==2){
			printf("Child process P3 is run,processid is %d,parentid is %i  empty = %i full = %i\n",getpid(),getppid(),empty,full);
			for(int j = 0; j < 2; j++){
				P(full);
				getodd(buff);
				countodd(buff);
				printf("Now buff'length = %i\n",buff->length);
				V(empty);
			}
		}
	}else{
		printf("Parent process is run,processid is %d  empty = %i full = %i shmid = %i\n",getpid(),empty,full,shmid);
		sleep(10);
		del_sem(empty);
		del_sem(full);
	}
	return 0;
}
