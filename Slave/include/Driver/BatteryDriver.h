#ifndef BATTERYDRIVER_H
#define BATTERYDRIVER_H

#include "Common/TcpClient/TcpCltSession.h"
#include "SingleTon.h"
#include <stdio.h> 
#include <string>
#include <unistd.h>
//读取电池信息
struct BatteryInfo{
     float quantity;
     float voltage;
     float current;
     float temp;
     int cycle;
     int capacity;
};
class BatteryDriver
{
public:
    ~BatteryDriver();
    static BatteryDriver* getInstance();
    int init();
    //电池信息请求
    std::string spliceReq(const char* reqName, int transId);
    //电池反馈信息请求
    int parseRes(std::string res, TcpResp& result, BatteryInfo& batteryInfo);
    //获得电池的信息
    int getBatteryInfo(BatteryInfo& batteryInfo);
    //更新电池信息
    static void updateInfo(BatteryDriver* pBatteryDriver);
    
    //info
    float getQuantity(){return m_BatteryInfo.quantity;};
    float getVoltage(){return m_BatteryInfo.voltage;};
    float getCurrent(){return m_BatteryInfo.current;};
    float getTemperature(){return m_BatteryInfo.temp;};
    int isCharge(){return (m_BatteryInfo.current > 0.5 ? 1 : 0);};
    
private:
    BatteryInfo m_BatteryInfo;
    TcpClient* m_tcpClient;
};

#endif // BATTERYDRIVER_H
