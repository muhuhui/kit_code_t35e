#ifndef LEDDRIVER_H
#define LEDDRIVER_H

#include "Common/TcpClient/TcpCltSession.h"
#include "base/SingleTon.h"


typedef struct
{
    int brightness;
    int rgb;
    int freq;
} LedCfgPara;

class LedDriver
{
public:
    ~LedDriver();
    static LedDriver *getInstance();

    int init();

    static void run(LedDriver *pLedDriver);

    std::string spliceReq(const char* reqName, int transId);
    std::string spliceReq(const char* reqName, int transId, int sw);
    std::string spliceReq(const char* reqName, int transId, LedCfgPara para);
    int parseRes(std::string res, TcpResp& result);
    int parseRes(std::string res, TcpResp& result, int& sw);

    int swCtrl(int sw);
    int getSw(int& sw);
    int config(LedCfgPara para); 
private:
    TcpClient*              m_tcpClient;
    bool                    m_bThreadRun;
};

#endif // LEDDRIVER_H
