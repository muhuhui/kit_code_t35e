#ifndef __SHMCOMM_H__
#define __SHMCOMM_H__

#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <iostream>
#include <string.h>
#include <fcntl.h>

namespace ShmComm {

union semun
{
    int val;
    struct semid_ds *buf;
    unsigned short *arry;
};

static int sem_id = 0;

static int semGet()
{
	key_t key = ftok("/home/ubuntu", 'a');
    if ((sem_id = semget(key, 1, 0)) >= 0) {
        printf("%s(), get semId: %d\n", __FUNCTION__, sem_id);
        return 0;
    }
    if ((sem_id = semget(key, 1, 0666|IPC_CREAT)) >= 0) {
        printf("%s(), create semId: %d\n", __FUNCTION__, sem_id);
        return 0;
    }
    perror("semget error");
    return -1;
}

// 初始化信号量
static int semInit()
{
    union semun sem_union;
    sem_union.val = 1;
    if (-1 == semctl(sem_id, 0, SETVAL, sem_union)) {
        return -1;
    }
    return 0;
}

// 删除信号量 
static void semDel()
{
    union semun sem_union;
    if (-1 == semctl(sem_id, 0, IPC_RMID, sem_union)) {
        fprintf(stderr, "Failed delete semphore\n");
    }
}

// 对信号量-1操作，即等待P(sv)
static int semP()
{
    struct sembuf sem_b;
    sem_b.sem_num = 0;
    sem_b.sem_op = -1; // P()
    sem_b.sem_flg = SEM_UNDO;

    if (-1 == semop(sem_id, &sem_b, 1)) {
        fprintf(stderr, "Failed semaphore_p()\n");
        return -1;
    }

    return 0;
}

// 释放操作， +1， 发送信号V(sv)
static int semV()
{
    struct sembuf sem_b;
    sem_b.sem_num = 0;
    sem_b.sem_op = 1; // V()
    sem_b.sem_flg = SEM_UNDO;

    if (-1 == semop(sem_id, &sem_b, 1)) {
        fprintf(stderr, "Failed semaphore_v()\n");
        return -1;
    }

    return 0;
}

/*
 * ftok：创建唯一键值，key值等于第二个参数的低8位<<24|第一个参数路径的索引值（即ls -i 路径的值）
 * shmget：获取共享内存id，第二个参数是分配的大小，第三个参数在创建内存段时才需要creat|excl|权限
 * shmat：获取内存地址，第二个参数在手动分配地址时使用，从该地址开始往后搜寻对齐段的起始地址并返回，若只需读取，第三个参数设SHM_RDONLY
 * shmdt：解除映射
 * shmctl：销毁共享内存
 */
template <class T>
class ShmComm 
{
protected:
	typedef enum {
		SENDER = 0,
		RECEIVER
	}COMM_SIDE;
	COMM_SIDE m_commSide;
public:
	int shmGet(unsigned char topicId);
	int shmSnd(T* msg);
	int shmRcv(T* msg);
	int shmDel();
	ShmComm();
	~ShmComm();
private:
	int m_nShmId;
};

template <class T>
ShmComm<T>::ShmComm():
	m_nShmId(0),
	m_commSide(RECEIVER)
{
}

template <class T>
ShmComm<T>::~ShmComm()
{
	if (m_commSide == SENDER) {
		shmDel();
        semDel();
	}
}

template <class T>
int ShmComm<T>::shmGet(unsigned char topicId)
{
    semGet();
    if (m_commSide == SENDER) {
        semInit();
    }

	key_t key = ftok("/home/ubuntu", topicId + 1);
	if (key < 0) {
		perror("ftok error");
		return -1;
	}

	m_nShmId = shmget(key, 0, 0);
	if (m_nShmId >= 0) {
		printf("%s(), get shmId: %d\n", __FUNCTION__, m_nShmId);
		return 0;
	}

	// m_nShmId = shmget(key, 1024, IPC_CREAT|IPC_EXCL|0666);
    m_nShmId = shmget(key, sizeof(T) + sizeof(int), IPC_CREAT|IPC_EXCL|0666);
	if (m_nShmId < 0 ) {
		perror("shmget error");
		return -2;
	}
	printf("%s(), create new shmId: %d\n", __FUNCTION__, m_nShmId);

	return 0;
}

template <typename T>
int ShmComm<T>::shmDel()
{
	if (m_nShmId == 0) {
		return -1;
	}

	char* shmAddr = (char*)shmat(m_nShmId, NULL, 0);
	if (shmAddr < (char*)0) {
		perror("shmat error");
		return -2;
	}

	if (shmdt(shmAddr) < 0) {
		perror("shmdt error");
		return -3;
	}

	if (shmctl(m_nShmId, IPC_RMID, NULL) < 0) {
		perror("shmctl error");
		return -4;
	}

	printf("%s(), delete shmId: %d\n", __FUNCTION__, m_nShmId);
	return 0;
}

template <class T>
int ShmComm<T>::shmSnd(T *msg)
{
	static int sndCnt = 0;

    semP();
	char* shmAddr = (char*)shmat(m_nShmId, NULL, 0);
	if (shmAddr < (char*)0) {
		perror("shmat error");
        semV();
		return -1;
	}

	sndCnt++;
    T *tmsg = (T*)((int*)shmAddr + 1);
    memcpy(tmsg, msg, sizeof(T));
	*(int*)shmAddr = sndCnt;

	if (shmdt(shmAddr) < 0) {
		perror("shmdt error");
        semV();
		return -2;
	}
    semV();

	return 0;
}

template <class T>
int ShmComm<T>::shmRcv(T *msg)
{
    int nRet = 0;
	static int RcvCnt = 0;

    semP();
	char* shmAddr = (char*)shmat(m_nShmId, NULL, SHM_RDONLY);
	if (shmAddr < (char*)0) {
		perror("shmat error");
        semV();
		return -1;
	}

	int SndCnt = *(int*)shmAddr;
	if (SndCnt != RcvCnt) {
        T *tmsg = (T*)((int*)shmAddr+1);
        memcpy(msg, tmsg, sizeof(T));
        RcvCnt = SndCnt;
	}
    else {
        nRet = -2;
    }

	if (shmdt(shmAddr) < 0) {
		perror("shmdt error");
        nRet = -3;
	}
    semV();

	return nRet;
}

#define SHM_COMM_T ShmComm<T>
template <class T>
class Subscriber: public ShmComm<T>
{
public:
	int subscribe(unsigned char topicId, void (*fCallBack)(T msg));

	static void *shmRcvThread(void *arg);
private:
	void (*shmCallBack)(T msg) = NULL;
};

template <class T>
int Subscriber<T>::subscribe(unsigned char topicId, void (*fCallBack)(T msg))
{
    this->m_commSide = SHM_COMM_T::RECEIVER;
	shmCallBack = fCallBack;
	int ret = SHM_COMM_T::shmGet(topicId);

	pthread_t threadId;
	pthread_create(&threadId, NULL, &shmRcvThread, this);
	return ret;
}

template <class T>
void* Subscriber<T>::shmRcvThread(void *arg)
{
	T msg;
	Subscriber<T> *client = (Subscriber<T>*)arg;
	while (1)
	{
		if (client->shmRcv(&msg) == 0) {
			client->shmCallBack(msg);
		}
		usleep(10000);
	}

	pthread_exit(NULL);
}

template <class T>
class Publisher: public ShmComm<T>
{
public:
	int advertise(unsigned char topicId);
	int publish(T msg);
private:
};

template <class T>
int Publisher<T>::advertise(unsigned char topicId)
{
	this->m_commSide = SHM_COMM_T::SENDER;
	int ret = SHM_COMM_T::shmGet(topicId);
	return ret;
}

template <class T>
int Publisher<T>::publish(T msg)
{
    int ret = this->shmSnd(&msg);
	return ret;
}

}

#endif
