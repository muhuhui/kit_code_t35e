#ifndef MYSQLDRIVER_H
#define MYSQLDRIVER_H

#include "Common/SingleTon.h"
#include "Transaction/Setting.h"
#include "Poco/Data/Session.h"
#include "Poco/Data/SessionPool.h"
#include "Poco/Data/MySQL/Connector.h"
#include "Poco/Data/RecordSet.h"


using namespace Poco::Data::Keywords;
using Poco::Data::Session;
using Poco::Data::SessionPool;
using Poco::Data::Statement;
using Poco::Data::RecordSet;

class MySQLDriver
{
public:
    ~MySQLDriver();
    static MySQLDriver *getInstance();

    int init(std::string sServerIp, std::string sServerPort, std::string sDataBase,
             std::string sUserName, std::string sPasswd);

    Session getSession(){ return m_pSessionPool->get(); }

private:
    SessionPool* m_pSessionPool;
};

#endif // MYSQLDRIVER_H
