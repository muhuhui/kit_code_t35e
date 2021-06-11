#include "Transaction/IndustrialCamera.h"
#include <string.h>
#include "Transaction/SettingService.h"
#include "Transaction/ConfigServer.h"
#include "Transaction/Robot.h"

IndustrialCamera::~IndustrialCamera()
{
	
}

//构造函数
IndustrialCamera::IndustrialCamera()
{
	//init();
}

IndustrialCamera *IndustrialCamera::getInstance()
{
    return SingleTon<IndustrialCamera>::GetInstance();
}

//初始化
void IndustrialCamera::init()
{
    Robot *pRobot = Robot::getInstance();
	//ip和端口初始化
	setIp(pRobot->m_pSettingService->m_setting.getIndustrialCameraIp());
	setUserName(pRobot->m_pSettingService->m_setting.getCameraUserName());
	setPassword(pRobot->m_pSettingService->m_setting.getCameraPassword());
	m_pIndustrialCameraDriver = IndustrialCameraDriver::getInstance();
	m_pIndustrialCameraDriver->init(ip,userName,password);
}


//连接
int IndustrialCamera::connect()
{
    int nRet = 0;
	m_pIndustrialCameraDriver->init(ip,userName,password);
    nRet = m_pIndustrialCameraDriver->connect();
    return nRet;
}
//断开连接
int IndustrialCamera::disconnect()
{
    int nRet = 0;
    nRet = m_pIndustrialCameraDriver->disconnect();
    return nRet;
}


//预置点设置与动作
int IndustrialCamera::presetOperation(int nCmd, int nPresetIndex)
{
    int nRet = 0;
    nRet = m_pIndustrialCameraDriver->presetOperation(nCmd, nPresetIndex);
    return nRet;
}

//云台控制
int IndustrialCamera::platformOperation(int nCmd, int nStop, int nSpeed)
{
    int nRet = 0;
    nRet = m_pIndustrialCameraDriver->platformOperation( nCmd, nStop, nSpeed);
    return nRet;
}

//拍照
int IndustrialCamera::takePicture(char* sFileName)
{
    int nRet = 0;
    nRet = m_pIndustrialCameraDriver->takePicture(sFileName);
    return nRet;
}

int IndustrialCamera::detectDevice()
{
    int nRet = 0;
    nRet = m_pIndustrialCameraDriver->detectDevice();
    return nRet;
}

int IndustrialCamera::rebootDevice()
{
    int nRet = 0;
    nRet = m_pIndustrialCameraDriver->rebootDevice();
    return nRet;
}

