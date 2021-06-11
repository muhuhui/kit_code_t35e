#ifndef LOWCONSUMEMANAGE_H
#define LOWCONSUMEMANAGE_H

#include "Common/TcpClient/TcpCltSession.h"
#include "base/SingleTon.h"
#include <stdio.h> 
#include <string>
#include <unistd.h>
#include <map>


typedef enum {
    LOW_CON_RADAR = 0,
    LOW_CON_RFID,
    LOW_CON_MOVE_MOTOR,
    LOW_CON_LIFT_MOTOR,
} LowConEnum;

typedef struct {
    int radar;
    int rfid;
    int moveMotor;
    int liftMotor;
} LowConStatus;

class LowConManage
{
public:
    ~LowConManage();
    static LowConManage* getInstance();
    void init();

    std::string spliceReq(const char* reqName, int transId);
    std::string spliceReq(const char* reqName, int transId, std::map<LowConEnum, int>& stat);
    int parseRes(std::string res, TcpResp& result);
    int parseRes(std::string res, TcpResp& result, LowConStatus& stat);

    int switchSet(LowConStatus& stat);
    int switchGet(LowConStatus& stat);
    int rfidSwitch(bool sw);
    int radarSwitch(bool sw);
    int moveMotorSwitch(bool sw);
    int liftMotorSwitch(bool sw);

private:
    TcpClient* m_tcpClient;
};

#endif // LOWCONSUMEMANAGE_H
