/*
 * mysem.h
 *
 *  Created on: 2018��10��19��
 *      Author: lin
 */

#ifndef MYSEM_H_
#define MYSEM_H_

#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

// �����ź����ṹ
union semaphore{
	int val;
	struct semid_ds *buf;
	unsigned short *array;
};

/*��������*/

/*��ʼ���ź���
 * sem_id �ź����ı�ʶid
 * init_val �ź�����pv��������ֵ
 * */
int init_sem(int sem_id,int init_val){
	union semaphore sem;
	sem.val = init_val; //���ó�ֵ
	if(semctl(sem_id,0,SETVAL,sem)==-1){
		printf("init semaphore error \n");
		return -1;
	}
	return 0;
}
/*ɾ��ָ����ʶid���ź���*/
int del_sem(int sem_id){
	union semaphore sem;
	if(semctl(sem_id,0,IPC_RMID,sem)==-1){
		return -1;
	}
	return 0;
}
/* ��ָ����ʶ���ź���-1 ���ź���ֵΪ0ʱִ��p�����Ľ��̻ᴦ�������ȴ���״̬*/
int P(int sem_id){ //�ź���-1
	struct sembuf sem_b;
	sem_b.sem_num = 0;
	sem_b.sem_op = -1;
	sem_b.sem_flg = SEM_UNDO;
	if(semop(sem_id,&sem_b,1)==-1){
		return -1;
	}
	return 0;
}
/* ��ָ����ʶ���ź���+1 ���ͺ�����ֵΪ0ִ����v������ ���Ѵ��������ȴ��Ľ��̴��ھ���״̬�ȴ�cpu����*/
int V(int sem_id){ //�ź���+1
	struct sembuf sem_b;
	sem_b.sem_num = 0;
	sem_b.sem_op = 1;
	sem_b.sem_flg = SEM_UNDO;
	if(semop(sem_id,&sem_b,1)==-1){
		return -1;
	}
	return 0;
}

#endif /* MYSEM_H_ */
