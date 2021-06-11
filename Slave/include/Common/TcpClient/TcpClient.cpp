/*================================================================
*   Copyright (C) 2020 Tetraelc Ltd. All rights reserved.
*   
*   File name: TcpClient.cpp
*   Author: JiangJin
*   Create Date: 2020-11-26
*   Description: 
*
================================================================*/

#include "TcpClient.h"
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <time.h>
#include <iostream>
#include "Driver/MyLog4CXX.h"
#include <list>
#include <math.h>
#include <thread>

#define BUFFER_SIZE 4096

std::map<TinyFrame*, TcpClient*> g_tfSockfdMap;

static int ExtratNum(const char* str)
{
    const char* a = str;
    int cnt_int = 0;
    //cnt_int 用于存放字符串中的数字.

    for (int i = 0; a[i] != '}'; ++i) //当a数组元素不为结束符时.遍历字符串a.
    {
        if (a[i] >= '0'&& a[i] <= '9') //如果是数字.
        {
            cnt_int *= 10;//先乘以10保证先检测到的数字存在高位，后检测的存在低位
            cnt_int += a[i] - '0'; //数字字符的ascii-字符'0'的ascii码就等于该数字.
        }
    }
    std::cout << cnt_int << std::endl; //输出数字.(12345)
    return cnt_int;
}

void TF_WriteImpl(TinyFrame *tf, const uint8_t *buff, uint32_t len)
{
    //printf("\033[32mTF_WriteImpl - sending frame:\033[0m\n");
    //dumpFrame(buff, len);
    usleep(1000);

    auto iter = g_tfSockfdMap.find(tf);
    TcpClient* clt = iter->second;
    if (iter != g_tfSockfdMap.end() && clt != NULL) {
        volatile int sockfd = clt->m_socketfd;
        if (sockfd >= 0) {
            write(sockfd, buff, len);
        }
    }
    else {
        printf("%s(), No peer!\n", __FUNCTION__);
    }
}

void TcpClient::RecvThread(TcpClient* pClient)
{
    //auto logger = initLogger("root.TcpClient" + std::string(__FUNCTION__));
    printf("%s(), start\n", __FUNCTION__);
    //LOG4CPLUS_INFO(logger, " start");

    while (1) 
    {
        if (pClient->m_socketfd == -1) {
            int sockfd = -1;
            struct sockaddr_in servaddr;
            memset(&servaddr, 0, sizeof(servaddr));

            servaddr.sin_family = AF_INET;
            servaddr.sin_port = htons(pClient->m_nPort);
            servaddr.sin_addr.s_addr = inet_addr(pClient->m_sTargetIp.c_str());
            if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                printf("\n Error : Could not create socket \n");
                sleep(3);
                continue;
            }

            if (::connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
                perror("connect failed");
                close(sockfd);
                sleep(3);
                continue;
            }
            pClient->m_socketfd = sockfd;
        }

        ssize_t n = 0;
        uint8_t recvBuff[10240];
        while (1) {
            if ((n = read(pClient->m_socketfd, recvBuff, sizeof(recvBuff) - 1)) > 0) {
                //printf("\033[36m--- RX %ld bytes ---\033[0m\n", n);
                //dumpFrame(recvBuff, (size_t)n);
                TF_Accept(pClient->m_tf, recvBuff, (size_t)n);
            }
            else {
                pClient->m_socketfd = -1;
                break;
            }
        }
        printf("%s(), read finished\n", __FUNCTION__);
    }

    printf("%s(), end\n", __FUNCTION__);
    //LOG4CPLUS_INFO(logger, " end");
}

TF_Result replyListener(TinyFrame *tf, TF_Msg *msg)
{
    const int MAX_MSG_SIZE = 128;
    const int MAX_TIME_INTERVAL = 60;
    //dumpFrameInfo(msg);

    if (msg->data == NULL) {
        printf("%s(), msg timeout\n", __FUNCTION__);
        return TF_STAY;
    }

    auto iter = g_tfSockfdMap.find(tf);
    TcpClient* clt = iter->second;
    if (iter == g_tfSockfdMap.end() || clt == NULL) {
        return TF_STAY;
    }

    TcpMsg tcpMsg;
    time_t nowTime;
    time(&nowTime);

    clt->m_tfMsgLck.lock();
    if (clt->m_tfMsgList.size() > 0) {
        tcpMsg = clt->m_tfMsgList.front();
        if (abs((int)nowTime - (int)tcpMsg.tm) > MAX_TIME_INTERVAL) {
            clt->m_tfMsgList.pop_front();
        }
        else if (clt->m_tfMsgList.size() >= MAX_MSG_SIZE) {
            clt->m_tfMsgList.pop_front();
        }
    }
    
    memset(tcpMsg.msg, 0, msg->len+1);
    memcpy(tcpMsg.msg, msg->data, msg->len);
    tcpMsg.tm = nowTime;
    tcpMsg.len = msg->len;
    tcpMsg.transId = msg->frame_id;
    clt->m_tfMsgList.push_back(tcpMsg);
    clt->m_tfMsgLck.unlock();
    //printf("%s(), frame: %x, push msg: %s\n", __FUNCTION__, tcpMsg.transId, tcpMsg.msg);

    return TF_STAY;
}


TcpClient::~TcpClient()
{
    if (m_socketfd >= 0) {
        close(m_socketfd);
    }
}

TcpClient::TcpClient(char* ip, int port, int wait):
    m_socketfd(-1),
    m_tcpEnable(1),
    m_waitTime(wait),
    m_sTargetIp(ip),
    m_nPort(port)
{
    m_tfMsgList.clear();
}

int TcpClient::init()
{
    m_tf = TF_Init(TF_MASTER);
    TF_AddGenericListener(m_tf, replyListener);
    g_tfSockfdMap[m_tf] = this;

    std::thread tcpThread;
    tcpThread = std::thread(TcpClient::RecvThread, this);
    tcpThread.detach();
    
    return 0;
}

int TcpClient::request(std::string cmd, std::string& result)
{
    static std::mutex lck;
    if (m_tcpEnable) {
        //auto logger = initLogger("root.TcpClient" + std::string(__FUNCTION__));

        TF_Msg tfmsg;
        memset(&tfmsg, 0, sizeof(TF_Msg));
        tfmsg.len = cmd.size();
        tfmsg.type = 1;
        tfmsg.data = (const uint8_t *)cmd.c_str();

        // if (TF_SendSimple(m_tf, frameId, (const uint8_t*)cmd.c_str(), cmd.size()) == false) {
        int tryCnt = 5;
        lck.lock();
        while (TF_Send(m_tf, &tfmsg) == false) {
            lck.unlock();
            if (tryCnt <= 0) {
                printf("%s(), send failed\n", __FUNCTION__);
                //LOG4CPLUS_INFO(logger, " send failed");
                break;
            }
            tryCnt--;
            usleep(10000);
            lck.lock();
        }
        lck.unlock();
        int frameId = tfmsg.frame_id;

        //printf("%s(), frame: %x, cmd: %s\n", __FUNCTION__, frameId, cmd.c_str());
        //LOG4CPLUS_INFO(logger, " frame: " << frameId << ", cmd: " << cmd);

        const int MS_INTERVAL = 50;
        tryCnt = m_waitTime*1000/MS_INTERVAL;
        //printf("%s(), waitTime: %d, tryCnt: %d\n", __FUNCTION__, m_waitTime, tryCnt);
        while (tryCnt--) {
            m_tfMsgLck.lock();

            if (!m_tfMsgList.size()) {
                //add into a queue
                std::string cmdStr = cmd + std::string("No response, trying ") + std::to_string(tryCnt);
                if (m_queue.size() < 1024) {
                    m_queue.push(cmdStr);
                }
            }

            for (auto iter = m_tfMsgList.begin(); iter != m_tfMsgList.end(); iter++) {
                //printf("%s(), tryCnt: %d, frame: %x\n", __FUNCTION__, tryCnt, iter->transId);
                if (iter->transId == frameId) {
                    result = iter->msg;
                    m_tfMsgList.erase(iter);
                    m_tfMsgLck.unlock();

                    //add into a queue
                    std::string cmdStr = cmd + result;
                    if(m_queue.size() < 1024){
                    	m_queue.push(cmdStr);
                    }

                    //printf("%s(), frame: %x, res: %s\n", __FUNCTION__, frameId, result.c_str());
                    //LOG4CPLUS_INFO(logger, "frame: " << frameId << ", res: " << result);
                    return 0;
                }
            }
            m_tfMsgLck.unlock();
            usleep(MS_INTERVAL*1000);
        }
        //printf("%s(), frame %x receive timeout!\n", __FUNCTION__, frameId);
        //LOG4CPLUS_INFO(logger, " frame " << frameId << " receive timeout!");
        return -1;
    }
    else {
        return -2;
    }
}

int TcpClient::shortRequest(std::string cmd, std::string &result)
{
    if(m_tcpEnable) {
        int sock_cli = socket(AF_INET, SOCK_STREAM, 0);
        if (sock_cli < 0) {
            perror("create socket failed");
            return -1;
        }

        struct sockaddr_in servaddr;
        memset(&servaddr, 0, sizeof(servaddr));

        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(m_nPort);
        servaddr.sin_addr.s_addr = inet_addr(m_sTargetIp.c_str());

        m_reqlck.lock();
        
        if (::connect(sock_cli, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
            perror("connect failed");
            close(sock_cli);
            m_reqlck.unlock();
            return -1;
        }

        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 200000;
        setsockopt(sock_cli, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(struct timeval));
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
        setsockopt(sock_cli, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(struct timeval));

        char recvbuf[BUFFER_SIZE];
        if (send(sock_cli, cmd.c_str(), cmd.length(), 0) < 0) {
            perror("send failed");
            close(sock_cli);
            m_reqlck.unlock();
            return -2;
        }

        memset(&recvbuf, 0, BUFFER_SIZE);
        if (recv(sock_cli, recvbuf, sizeof(recvbuf),0) < 0) {
            perror("receive failed");
            close(sock_cli);
            m_reqlck.unlock();
            return -3;
        }
        result.clear();
        result = recvbuf;
        close(sock_cli);
        usleep(50000);
        m_reqlck.unlock();

        return 0;
    }
    else {
        return -1;
    }
}

void TcpClient::TcpClose() {
    m_tcpEnable = 0;
}

void TcpClient::TcpOpen() {
    m_tcpEnable = 1;
}

void TcpClient::TcpRealClose() {
    close(m_socketfd);
}

std::string TcpClient::RecExtration(const char* origin, const char* extract) {
    std::string sRet = "";
    const char* ret = strstr(origin, extract);
    if(ret != NULL) {
        sRet = ret;
    }
    return sRet;
}

std::string TcpClient::SenExtration(const char* origin, const char* extract) {
    std::string sRet = "";
    const char* ret = strstr(origin, extract);
    if(ret == NULL) {
        return sRet;
    }
    else {
        sRet = std::to_string(ExtratNum(ret));
        return sRet;
    }
}
