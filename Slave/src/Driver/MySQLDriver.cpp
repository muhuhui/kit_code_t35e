#include <string.h>
#include "Driver/MySQLDriver.h"

MySQLDriver::~MySQLDriver()
{
  Poco::Data::MySQL::Connector::unregisterConnector();
}

MySQLDriver *MySQLDriver::getInstance()
{
  return SingleTon<MySQLDriver>::GetInstance();
}

int MySQLDriver::init(std::string sServerIp, std::string sServerPort, std::string sDataBase,
                      std::string sUserName, std::string sPasswd)
{
  int nRet = 0;
  try {
    Poco::Data::MySQL::Connector::registerConnector();
    std::string sqlStr = Poco::format( \
          "host=%s;port=%s;db=%s;user=%s;password=%s;compress=true;auto-reconnect=true", \
          sServerIp,
          sServerPort,
          sDataBase,
          sUserName,
          sPasswd);   //db=smart;

    m_pSessionPool = new Poco::Data::SessionPool("mysql", sqlStr, 1, 32, 5);

  } catch(Poco::Exception& exc) {

    nRet = -1;
    std::cerr << "MySQLDriver::init " << exc.displayText() << std::endl;
  }

  return nRet;
}
