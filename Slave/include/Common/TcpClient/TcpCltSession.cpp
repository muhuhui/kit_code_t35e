/*================================================================
*   Copyright (C) 2020 Tetraelc Ltd. All rights reserved.
*   
*   File name: TcpCltSession.cpp
*   Author: JiangJin
*   Create Date: 2020-12-29
*   Description: 
*
================================================================*/


#include "TcpCltSession.h"

TcpCltSession::TcpCltSession()
{
    m_cltSession.clear();
}

TcpCltSession::~TcpCltSession()
{
    for (auto iter = m_cltSession.begin(); iter != m_cltSession.end(); iter++) {
        delete iter->second;
    }
}

TcpCltSession *TcpCltSession::getInstance()
{
  return SingleTon<TcpCltSession>::GetInstance();
}

int TcpCltSession::createSession(TcpNum num, char* targetIp, int port, int wait)
{
    if (m_cltSession.find(num) != m_cltSession.end()) {
        printf("TcpClient::%s, session %s:%d already exists\n", __FUNCTION__, targetIp, port);
        return -1;
    }

    if (targetIp == NULL || port <= 0) {
        printf("TcpClient::%s, session invalid\n", __FUNCTION__);
        return -2;
    }

    TcpClient* clt = new TcpClient(targetIp, port, wait);
    clt->init();
    m_cltSession[num] = clt;
    printf("TcpClient::%s, create session %s:%d\n", __FUNCTION__, targetIp, port);
    return 0;
}

TcpClient* TcpCltSession::getSession(TcpNum num)
{
    if (m_cltSession.find(num) == m_cltSession.end()) {
        return NULL;
    }
    return m_cltSession[num];
}

void TcpCltSession::closeSession(TcpNum num)
{
    if (m_cltSession.find(num) == m_cltSession.end()) {
        return;
    }
    else {
        m_cltSession[num]->TcpClose();
    }
}

void TcpCltSession::openSession(TcpNum num)
{
    if (m_cltSession.find(num) == m_cltSession.end()) {
        return;
    }
    else {
        m_cltSession[num]->TcpOpen();
    }
}

void TcpCltSession::realcloseSession(TcpNum num)
{
    if (m_cltSession.find(num) == m_cltSession.end()) {
        return;
    }
    else {
        m_cltSession[num]->TcpRealClose();
        auto it = m_cltSession.find(num);
        m_cltSession.erase(it);
    }
}

