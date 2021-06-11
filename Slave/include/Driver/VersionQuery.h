#ifndef VERSIONQUERY_H
#define VERSIONQUERY_H

#include "Common/TcpClient/TcpCltSession.h"
#include "base/SingleTon.h"

enum VersionInfo{
    reqMotionVersion = 1,
    reqPtzVersion = 2,
    reqSensorVersion = 3,
    reqPMUVersion = 4,
};

struct DeviceInfo{
    std::string bootloader;
    std::string application;
    std::string transId;
};

class VersionQuery
{
public:
    ~VersionQuery();
    static VersionQuery *getInstance();
    
    std::string spliceReq(const char* reqName, int transId);
    int parseRes(std::string res, DeviceInfo& result);
    int queryVersion(VersionInfo info);
    std::string getVersion();

private:
    TcpClient*    m_tcpClient;
    DeviceInfo    m_deviceInfo;
};

#endif // VERSIONQUERY_H