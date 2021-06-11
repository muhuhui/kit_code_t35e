#ifndef __MSGCOMM_H__
#define __MSGCOMM_H__

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>

namespace MsgComm {

typedef enum {
    SERVER = 0,
    CLIENT, 
}COMM_SIDE;

template <class T>
class MsgComm
{
protected:
private:
	typedef struct {
		long type;
		T data;
		long transId;
	}Msg;
	int m_nMsgId;
public:
	MsgComm();
	~MsgComm();
	int msgGet(unsigned char srvId);
	int msgRcv(long msgType, T *tMsg, long &transId, int timeout = 3);
	int msgSnd(long msgType, T *tMsg, long transId);
	int msgDel();

    COMM_SIDE m_commSide;
};

template <class T>
MsgComm<T>::MsgComm():
	m_nMsgId(-1),
	m_commSide(CLIENT)
{

}

template <class T>
MsgComm<T>::~MsgComm()
{
	if (m_commSide == SERVER) {
		msgDel();
	}
}
/* 
 * srvId range: 0~254 
 * */
template <class T>
int MsgComm<T>::msgGet(unsigned char srvId)
{
	key_t key = ftok("/home/ubuntu", srvId + 1);
	if (key < 0) {
		perror("ftok error");
		return -1;
	}

	int msgId = msgget(key, 0);
	if (msgId >= 0 ) {
		m_nMsgId = msgId;
		printf("%s(), key: %d, get msgId: %d\n", __FUNCTION__, key, msgId);
		return 0;
	}

	msgId = msgget(key, IPC_CREAT|IPC_EXCL|0666);
	if (msgId < 0) {
		perror("msgget error");
		return -2;
	}
	m_nMsgId = msgId;
	printf("%s(), key: %d, create new msgId: %d\n", __FUNCTION__, key, msgId);

	return 0;
}

template <class T>
int MsgComm<T>::msgRcv(long msgType, T *tMsg, long &transId, int timeout)
{
	Msg msg;
	
    int readByte = -1;
    if (m_commSide == SERVER) {
        readByte = msgrcv(m_nMsgId, &msg, sizeof(msg) - sizeof(msg.type), msgType, 0);		//服务端阻塞型
        if (readByte < 0) {
            perror("msgrcv error");
            return -1;
        }
    }
    else if (m_commSide == CLIENT) {
        int tryCnt = 0;
        while ((readByte < 0) && (tryCnt <= timeout*10)) {     //xs超时
            tryCnt++;
            readByte = msgrcv(m_nMsgId, &msg, sizeof(msg) - sizeof(msg.type), msgType, IPC_NOWAIT);		//客户端非阻塞型
            if (readByte < 0) {
                usleep(100000);
            }
        }
        if (readByte < 0) {
            perror("msgrcv timeout error");
        }
    }
	*tMsg = msg.data;
	transId = msg.transId;
	
	return readByte;
}

template <class T>
int MsgComm<T>::msgSnd(long msgType, T *tMsg, long transId)
{
	Msg msg;
	msg.type = msgType;
	msg.data = *tMsg;
	msg.transId = transId;

	int ret = msgsnd(m_nMsgId, &msg, sizeof(msg) - sizeof(msg.type), 0);
	if (ret != 0) {
		perror("msgsnd error");
		return -1;
	}

	return 0;
}

template <class T>
int MsgComm<T>::msgDel() 
{
	if (m_nMsgId < 0) {
		return -1;
	}

	if (msgctl(m_nMsgId, IPC_RMID, NULL) < 0) {
		perror("msgctl error");
		return -2;
	}

	return 0;
}

template <class MReq, class MRes>
class ServiceClient
{
public:
	int serviceClient(unsigned char srvId, unsigned char cltId);
	int call(MReq& mReq, MRes& mRes, int timeout = 3);
private:
    MsgComm<MReq> m_commReq;
    MsgComm<MRes> m_commRes;
	int m_nSrvId = 0;
	int m_nCltId = 0;
};

template <class MReq, class MRes>
int ServiceClient<MReq, MRes>::serviceClient(unsigned char srvId, unsigned char cltId)
{
    m_commReq.m_commSide = CLIENT;
	m_commReq.msgGet(srvId);

    m_commRes.m_commSide = CLIENT;
	m_commRes.msgGet(srvId);

	m_nSrvId = srvId;
	m_nCltId = cltId;
	
	return 0;
}

/* 
 * transId: 消息发送序号配对id
 * transType: 消息通道，0-15bit: transId, 16-23: cltId, 24-31: srvId
 * */
template <class MReq, class MRes>
int ServiceClient<MReq, MRes>::call(MReq& mReq, MRes& mRes, int timeout)
{
	static int transId = 0;
	const int srvId = m_nSrvId;
	const int cltId = m_nCltId;
	transId++;
	if (transId >= (1 << 16)) {
		transId = 0;
	}
	long transType = ((srvId + 1) << 24) | ((cltId + 1) << 16) | transId;
	if (m_commReq.msgSnd(((srvId + 1) << 24), &mReq, transType) < 0) {
		return -1; 
	}
    if (m_commRes.msgRcv(transType, &mRes, transType, timeout) < 0) {
        m_commReq.msgRcv(((srvId + 1) << 24), &mReq, transType, timeout);
        return -1;
    }
	return 0;
}

template <class MReq, class MRes>
class ServiceServer
{
public:
    int advertiseService(unsigned char srvId, bool (*fCallBack)(MReq&, MRes&));
	static void *msgRcvThread(void* arg);
private:
    MsgComm<MReq> m_commReq;
    MsgComm<MRes> m_commRes;
	bool (*srvCallBack)(MReq&, MRes&) = NULL;
	int m_nSrvId = 0;
};

template <class MReq, class MRes>
int ServiceServer<MReq, MRes>::advertiseService(unsigned char srvId, bool (*fCallBack)(MReq&, MRes&))
{
    m_commReq.m_commSide = SERVER;
	m_commReq.msgGet(srvId);

    m_commRes.m_commSide = SERVER;
	m_commRes.msgGet(srvId);

    srvCallBack = fCallBack;
    m_nSrvId = srvId;

    pthread_t threadId;
    pthread_create(&threadId, NULL, &msgRcvThread, this);

    return 0;
}

template <class MReq, class MRes>
void* ServiceServer<MReq, MRes>::msgRcvThread(void *arg)
{
	ServiceServer<MReq, MRes>* server = (ServiceServer<MReq, MRes>*)arg;
    MReq msgReq;
    MRes msgRes;
	long transType;
	const int srvId = server->m_nSrvId;
	
	while (1)
	{
		if (server->m_commReq.msgRcv((srvId + 1) << 24, &msgReq, transType) >= 0) {
			server->srvCallBack(msgReq, msgRes);
			server->m_commRes.msgSnd(transType, &msgRes, transType);
		}
	}

	pthread_exit(NULL);
}

}

#endif

