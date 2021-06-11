#ifndef CHARGEPILEDRIVER_H
#define CHARGEPILEDRIVER_H

#include "Common/TcpClient/TcpCltSession.h"
#include "base/SingleTon.h"
#include <list>
#include <map>
#include <unistd.h>

enum ChargePileStatus 
{
    CP_ERROR = 0,
    CP_OK = 1,
    CP_OFFLINE = 2
};

enum ChargeStatus
{
    CP_NOT_IN_CHARGE = 0,
    CP_IN_SEARCHING,
    CP_IN_CHARGING,
    CP_CHARGE_FULL,
    CP_CHARGE_FAIL,
};

typedef struct 
{
    bool isWarning;
    ChargeStatus chargeStat;
} ChargeInfo;

class ChargePileDriver
{
public:
    ~ChargePileDriver();
    static ChargePileDriver *getInstance();
    static void deleteChargePile();

    int init(std::map<int, char*> chargePilesMap);
    void resetMap(std::map<int, char*> chargePilesMap);
    std::map<int, ChargePileStatus> *getStatus();

    std::string spliceReq(const char* reqName, int transId);
    std::string spliceReq(const char* reqName, int transId, int pos);
    int parseRes(std::string res, TcpResp& result);
    int parseRes(std::string res, TcpResp& result, ChargeInfo& chargeInfo);

    int gotoCharge(int pos);
    int getChargeInfo(ChargeInfo& stat);

private:
    TcpClient* m_tcpClient;
    std::map<int, ChargePileStatus> m_statusMap;
    std::map<int, char*> m_chargepilesMap;
    ChargeInfo m_chargeStatus;
};

#endif // CHARGEPILEDRIVER_H
