#ifndef PARTIALDISCHARGEDETECTORDRIVER_H
#define PARTIALDISCHARGEDETECTORDRIVER_H

#include "Common/TcpClient/TcpCltSession.h"
#include "base/SingleTon.h"


typedef struct 
{
    int tev;
    int ultra;
} PDDStat;

class PartialDischargeDetectorDriver
{
public:
    ~PartialDischargeDetectorDriver();
    static PartialDischargeDetectorDriver *getInstance();

    int init();
    static void getInfoThread(PartialDischargeDetectorDriver* pddDriver);

    std::string spliceReq(const char* reqName, int transId);
    int parseRes(std::string res, TcpResp& result, PDDStat& stat);
    int getPDDStatus();
    
    float getTEVData();
    float getUWData();

    bool m_bThreadRun;
private:
    TcpClient* m_tcpClient;
    PDDStat m_data;
};

#endif // PARTIALDISCHARGEDETECTORDRIVER_H

