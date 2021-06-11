#ifndef RFIDDRIVER_H
#define RFIDDRIVER_H

#include "Common/TcpClient/TcpCltSession.h"
#include "SingleTon.h"
#include <stdio.h> 
#include <string>
#include <unistd.h>

class RFIDDriver
{
public:
    ~RFIDDriver();
    static RFIDDriver* getInstance();

    void init();
    //请求函数
    std::string spliceReq(const char* reqName, int transId);
    //响应函数
    int parseRes(std::string res, TcpResp& result,std::string &rfidNUM);
    //获得RFID的编号
    int getRFIDNum(std::string& rfidNUM);
private:
    TcpClient* m_tcpClient;
};

#endif // RFIDDRIVER_H

