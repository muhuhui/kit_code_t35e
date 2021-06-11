/*================================================================
*   Copyright (C) 2020 Tetraelc Ltd. All rights reserved.
*   
*   File name: TcpClient.h
*   Author: JiangJin
*   Create Date: 2020-06-28
*   Description: 
*
================================================================*/


#pragma once

#include <string>
#include <vector>
#include <mutex>
#include <map>
#include <list>
#include <queue>
extern "C"
{
#include "TinyFrame.h"
#include "utils.h"
}

#define JSON_TRY \
try \
{ 

#define JSON_CATCH \
} \
catch(...) \
{ \
    printf("json parse error"); \
} 

typedef struct 
{
    std::string transId;
    int errcode;
    std::string errmsg;
} TcpResp;

typedef struct {
    time_t tm;
    int transId;
    char msg[10240];
    int len;
} TcpMsg;

class TcpClient
{
public:
    TcpClient();
    TcpClient(char* ip, int port, int wait = 1);
    ~TcpClient();
    int request(std::string cmd, std::string &result);
    int shortRequest(std::string cmd, std::string &result);
    void TcpClose();
    void TcpOpen();
    void TcpRealClose();
    int init();
    static void RecvThread(TcpClient* pClient);
    std::string RecExtration(const char* origin, const char* extract);
    std::string SenExtration(const char* origin, const char* extract);

    std::queue<std::string> m_queue;
    std::list<TcpMsg> m_tfMsgList;
    std::mutex m_tfMsgLck;
    int m_socketfd;
    TinyFrame* m_tf;
    int m_tcpEnable;
    int m_waitTime;
    std::string m_sTargetIp;
    int m_nPort;
private:
    std::mutex m_reqlck;
};

