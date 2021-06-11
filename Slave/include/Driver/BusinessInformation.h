#ifndef BUSINESSINFO_H
#define BUSINESSINFO_H

#include "Common/TcpClient/TcpCltSession.h"
#include "base/SingleTon.h"
#include <stdio.h> 
#include <string>
#include <unistd.h>
#include <vector> 

typedef struct {
    int rfid;
    int battery;
    int radar;
    int moveMotor;
    int liftMotor;
} InspectStatus;

typedef enum {
    MAP_SCAN_START = 0,
    MAP_SCAN_STOP,
    MAP_SCAN_CANCEL,
} MapScanStatus;

typedef struct {
    long long num;
    int pos;
} RfidMapping;


class BusinessInfor
{
public:
    ~BusinessInfor();
    static BusinessInfor* getInstance();

    std::string spliceReq(const char* reqName, int transId);
    std::string spliceReq(const char* reqName, int transId, int cmd);
    std::string spliceReq(const char* reqName, int transId, std::vector<RfidMapping>& rfid);

    int parseRes(std::string res, TcpResp& result);
    int parseRes(std::string res, TcpResp& result, std::vector<RfidMapping>& rfid);
    int parseRes(std::string res, TcpResp& result, InspectStatus& stat);

    int init();
    int getInspectStatus(InspectStatus& stat);
    int rfidCalibration(int cmd);
    int rfidMapGet(std::vector<RfidMapping>& rfid);
    int rfidMapImport(std::vector<RfidMapping>& rfid);

private:
    TcpClient* m_tcpClient;
};

#endif // BUSINESSINFO_H
