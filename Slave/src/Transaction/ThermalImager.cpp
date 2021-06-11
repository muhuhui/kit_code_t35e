#include "Transaction/ThermalImager.h"
#include "Transaction/SettingService.h"
#include "Transaction/ConfigServer.h"
#include "Transaction/Robot.h"


ThermalImager::~ThermalImager()
{

}

ThermalImager *ThermalImager::getInstance()
{
    return SingleTon<ThermalImager>::GetInstance();
}

int ThermalImager::init()
{
    Robot *pRobot = Robot::getInstance();
	//ip和端口初始化
	printf("__FILE__ =%s, __LINE__=%d\n",__FILE__,__LINE__);
	setIp(pRobot->m_pSettingService->m_setting.getThermalImagerIp().c_str());
	setUserName(pRobot->m_pSettingService->m_setting.getCameraUserName());
	setPassword(pRobot->m_pSettingService->m_setting.getCameraPassword());
	m_pThermalDriver = ThermalDriver::getInstance();
    m_pThermalDriver->init(ip,userName,"123456");
    return 0;
}

int ThermalImager::close()
{
    int nRet = 0;
    nRet = m_pThermalDriver->close();
    return nRet;
}

int ThermalImager::connect()
{
    int nRet = 0;
    nRet = m_pThermalDriver->connect();
    return nRet;
}

int ThermalImager::disconnect()
{
    int nRet = 0;
    nRet = m_pThermalDriver->disconnect();
    return nRet;
}

int ThermalImager::takePicture(char* sFileName)
{
    int nRet = 0;
    nRet = m_pThermalDriver->takePicture(sFileName);
    return nRet;
}

int ThermalImager::setFocus(int nFocus)
{
    int nRet = 0;
    nRet = m_pThermalDriver->setFocus(nFocus);
    return nRet;
}

float ThermalImager::getMaxTemp()
{
    float maxTemp = 0;
    maxTemp = m_pThermalDriver->getMaxTemp();
    return maxTemp;
}

int ThermalImager::detectDevice()
{
    int nRet = 0;
    nRet = m_pThermalDriver->detectDevice();
    return nRet;

}
