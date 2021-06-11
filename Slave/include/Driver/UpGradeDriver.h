#ifndef UPGRADEDRIVER_H
#define UPGRADEDRIVER_H

#include "Common/TcpClient/TcpCltSession.h"
#include "base/SingleTon.h"

class UpGradeDriver
{
public:
    ~UpGradeDriver();
    static UpGradeDriver *getInstance();

    int init();

    int request(std::string sReqName, std::string sReq, std::string& sRes);

private:
    bool m_isConnect;
};

#endif // UPGRADEDRIVER_H