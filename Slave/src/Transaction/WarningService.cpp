#include "Transaction/WarningService.h"
#include <vector>
#include <string>
#include "Driver/MyLog4CXX.h"

const std::string LOG_TAG("root.WarningService::");

using namespace std;

WarningService::~WarningService()
{

}

WarningService *WarningService::getInstance()
{
  return SingleTon<WarningService>::GetInstance();
}

int WarningService::init()
{
  m_pPocoBase = MySQLDriver::getInstance();
  return 0;
}

void WarningService::getThreshold(Warning &warn)
{
  try {
      int nRobotID = warn.getRobotID();

      float fSmognThreshold = stof(getSmogAlarmValue(nRobotID));
      float fIlluminanceThreshold = stof(getIlluminanceAlarmValue(nRobotID));
      float fBatteryTempThreshold = stof(getBatteryAlarmValue(nRobotID));
      float fO2Threshold = stof(getO2AlarmValue(nRobotID));
      float fCOThreshold = stof(getCOAlarmValue(nRobotID));
      float fSO2Threshold = stof(getSO2AlarmValue(nRobotID));
      float fCH4Threshold = stof(getCH4AlarmValue(nRobotID));
      float fThermalHighThreshold = stof(getHighAlarmValue(nRobotID));
      float fThermalLowThreshold = stof(getLowAlarmValue(nRobotID));

      warn.setSmogThreshold(fSmognThreshold);
      warn.setIlluminanceThreshold(fIlluminanceThreshold);
      warn.setBatteryTempThreshold(fBatteryTempThreshold);
      warn.setO2Threshold(fO2Threshold);
      warn.setCOThreshold(fCOThreshold);
      warn.setSO2Threshold(fSO2Threshold);
      warn.setCH4Threshold(fCH4Threshold);
      warn.setThermalCameraHighThreshold(fThermalHighThreshold);
      warn.setThermalCameraLowThreshold(fThermalLowThreshold);
  }
  catch(std::exception& e) {
      std::cout << __FILE__ << " : " << __FUNCTION__ << " : " << __LINE__ << " : " << e.what() << std::endl;
  }
}

bool WarningService::queryAlarmNearby(AlarmInfo vAlarmInfo, int nIntervalSeconds)
{
  int nNum = 0;
  std::string logname = LOG_TAG + std::string(__FUNCTION__);
  auto logger = initLogger(logname);
  try {

    std::string sqlStr = Poco::format("select COUNT(*) from alarm as a LEFT JOIN alarm_item_config as b on b.item_id = a.item_id \
                                      where b.item_name = '%s' and abs(TIMESTAMPDIFF(SECOND, alarm_time, '%s')) < %d and robot_id = %d", \
        vAlarmInfo.strItemName, vAlarmInfo.strAlarmTime, nIntervalSeconds, vAlarmInfo.nRobotID);

    Session ses = m_pPocoBase->getSession();
    if(ses.isConnected())
    {
      try {
          Statement statement(ses);
          statement << sqlStr, into(nNum), now;
      } catch(Poco::Exception& exc) {
        std::cerr << "WarningService::queryAlarmNearby " << exc.displayText() << std::endl;
      }
    }
    ses.close();

  } catch(Poco::Exception& exc) {
    std::cerr << "WarningService::queryAlarmNearby " << exc.displayText() << std::endl;
    LOG4CPLUS_ERROR(logger, exc.displayText());
    
  }

  return nNum > 0 ? true : false;
}

float WarningService::queryAlarmNearbydis(AlarmInfo vAlarmInfo)
{
  std::string logname = LOG_TAG + std::string(__FUNCTION__);
  auto logger = initLogger(logname);

  float pos = -1;
  try {

    string sqlStr = Poco::format("select alarm_position from alarm where alarm_id = (select MAX(alarm_id) from alarm as a LEFT JOIN alarm_item_config as b on b.item_id = a.item_id where b.item_name = '%s') AND robot_id = %d",\
                                 vAlarmInfo.strItemName,vAlarmInfo.nRobotID);
    //printf("sqlStr %s\n",sqlStr.c_str());
    Session ses = m_pPocoBase->getSession();
    if(ses.isConnected())
    {
      try {
          Statement statement(ses);
          statement << sqlStr,into(pos), now;
      } catch(Poco::Exception& exc) {
        std::cerr << "WarningService::queryAlarmNearbydis " << exc.displayText() << std::endl;
      }
    }
    ses.close();

  } catch(Poco::Exception& exc) {

    std::cerr << "WarningService::queryAlarmNearbydis " << exc.displayText() << std::endl;
    LOG4CPLUS_ERROR(logger, exc.displayText());
  }

  return pos;
}

int WarningService::insertAlarm(AlarmInfo vAlarmInfo)
{  
  std::string logname = LOG_TAG + std::string(__FUNCTION__);
  auto logger = initLogger(logname);
  int nNum = 0;
  try {

    std::string sqlStr = Poco::format("INSERT INTO alarm SELECT NULL, a.item_id, '%s', '%f', %d, '%s', '%s' from alarm_item_config as a \
                                      where a.item_name = '%s'", \
        vAlarmInfo.strAlarmTime, vAlarmInfo.dPosition, vAlarmInfo.nRobotID, vAlarmInfo.strReason, vAlarmInfo.strValue, vAlarmInfo.strItemName);

    printf("sqlStr %s\n",sqlStr.c_str());
    m_pPocoBase = MySQLDriver::getInstance();
    Session ses = m_pPocoBase->getSession();
    if(ses.isConnected())
    {
      try {
          Statement statement(ses);
          statement << sqlStr, now;
      } catch(Poco::Exception& exc) {
        std::cerr << "WarningService::insertAlarm " << exc.displayText() << std::endl;
      }
    }
    ses.close();

    char pSQL[1024];
    snprintf(pSQL, sizeof(pSQL), "select MAX(alarm_id) from alarm WHERE robot_id = %d", vAlarmInfo.nRobotID);

    Session sesRtn = m_pPocoBase->getSession();
    if(sesRtn.isConnected())
    {
      try {
          Statement statementRtn(sesRtn);
          statementRtn << pSQL, into(nNum), now;
      } catch(Poco::Exception& exc) {
        std::cerr << "WarningService::insertAlarm " << exc.displayText() << std::endl;
      }
    }
    sesRtn.close();

  } catch(Poco::Exception& exc) {

    std::cerr << "WarningService::insertAlarm " << exc.displayText() << std::endl;
    LOG4CPLUS_ERROR(logger, exc.displayText());
  }

  return nNum;
}

std::string WarningService::getSettingParam(std::string strName, int nRobotID)
{
  std::string logname = LOG_TAG + std::string(__FUNCTION__);
  auto logger = initLogger(logname);
  std::string strRet = "0";
  try {

    std::vector<std::string> vstrValues;
    char pSQL[1024];
    snprintf(pSQL, sizeof(pSQL), "SELECT parameter_value FROM parameters as a left JOIN parameter_config as b \
             on b.config_id = a.config_id where robot_id = %d and parameter_name ='%s'", \
        nRobotID, strName.c_str());

    Session ses = m_pPocoBase->getSession();
    if(ses.isConnected())
    {
      try {
          Statement statement(ses);
          statement<< pSQL, into(vstrValues), now;
      } catch(Poco::Exception& exc) {
        std::cerr << "WarningService::getSettingParam " << exc.displayText() << std::endl;
      }
    }
    ses.close();

    if (vstrValues.size() > 0)
      strRet = vstrValues[0];
    else
      strRet = "0";

  } catch(Poco::Exception& exc) {

    std::cerr << "WarningService::getSettingParam " << exc.displayText() << std::endl;
    LOG4CPLUS_ERROR(logger, exc.displayText());
  }

  return strRet;
}
