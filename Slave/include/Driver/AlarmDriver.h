#ifndef ALARMDRIVER_H
#define ALARMDRIVER_H

#include "Common/TcpClient/TcpCltSession.h"
#include "base/SingleTon.h"


class AlarmDriver
{
public:
    ~AlarmDriver();
    static AlarmDriver *getInstance();

    int init();

    static void run(AlarmDriver *pAlarmDriver);

    std::string spliceReq(const char* reqName, int transId);
    std::string spliceReq(const char* reqName, int transId, int sw);
    int parseRes(std::string res, TcpResp& result);
    int parseRes(std::string res, TcpResp& result, int& sw);

    int swCtrl(int sw);
    int getSw(int& sw);
private:
    TcpClient*              m_tcpClient;
    bool                    m_bThreadRun;
};

#endif // ALARMDRIVER_H
