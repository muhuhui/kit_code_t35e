#ifndef HALLDRIVER_H
#define HALLDRIVER_H

#include "Common/TcpClient/TcpCltSession.h"
#include "base/SingleTon.h"


class HallDriver
{
public:
    ~HallDriver();
    static HallDriver *getInstance();

    int init();

    static void run(HallDriver *pHallDriver);

    std::string spliceReq(const char* reqName, int transId);
    int parseRes(std::string res, TcpResp& result, int& trigger);

    bool getHall();
    int catchHall(int& trigger);

private:
    TcpClient*              m_tcpClient;
    bool                    m_bThreadRun;
    bool                    m_bCaught;
};

#endif // HALLDRIVER_H
