#ifndef ILLUMINATEDRIVER_H
#define ILLUMINATEDRIVER_H

#include "Common/TcpClient/TcpCltSession.h"
#include "base/SingleTon.h"


class IlluminateDriver
{
public:
    ~IlluminateDriver();
    static IlluminateDriver *getInstance();

    int init();

    static void run(IlluminateDriver *pIlluminateDriver);

    std::string spliceReq(const char* reqName, int transId);
    std::string spliceReq(const char* reqName, int transId, int sw);
    int parseRes(std::string res, TcpResp& result);
    int parseRes(std::string res, TcpResp& result, int& sw);

    int swCtrl(int sw);
    int getSw(int& sw);
private:
    bool                    m_bThreadRun;
    TcpClient*              m_tcpClient;
};

#endif // ILLUMINATEDRIVER_H
