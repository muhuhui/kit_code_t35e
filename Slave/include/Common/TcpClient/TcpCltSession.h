/*================================================================
*   Copyright (C) 2020 Tetraelc Ltd. All rights reserved.
*   
*   File name: TcpCltSession.h
*   Author: JiangJin
*   Create Date: 2020-12-29
*   Description: 
*
================================================================*/


#pragma once

#include "TcpClient.h"
#include "base/SingleTon.h"

typedef enum
{
    TCP_MAIN_BOARD = 0,
    TCP_PTZ_BOARD = 1,
    TCP_ENV_BOARD = 2,
    TCP_MAIN_BOARD_UPGRADE = 3,
    TCP_PTZ_BOARD_UPGRADE = 4,
    TCP_ENV_BOARD_UPGRADE = 5
} TcpNum;

class TcpCltSession
{
public:
    static TcpCltSession *getInstance();
    TcpCltSession();
    ~TcpCltSession();

    int createSession(TcpNum num, char* targetIp, int port, int wait = 1);
    void closeSession(TcpNum num);
    void openSession(TcpNum num);
    void realcloseSession(TcpNum num);

    TcpClient* getSession(TcpNum num);
    std::map<TcpNum, TcpClient*> m_cltSession;
};

