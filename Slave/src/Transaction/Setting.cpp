#include "Transaction/Setting.h"

Setting::Setting()
{
  m_fBatteryQuantityMin = 30.0;
}

Setting::~Setting()
{

}

//设置电池最小允许电量
void Setting::setBatteryQuantityMin(float fValue)
{
  m_fBatteryQuantityMin = fValue;
}

//获取电池最小允许电量
float Setting::getBatteryQuantityMin()
{

  return m_fBatteryQuantityMin;
}

//设置相机IP
void Setting::setIndustrialCameraIp(std::string ipAddr)
{
	sIndustrialCameraIp = ipAddr;
}
//获取相机IP
std::string Setting::getIndustrialCameraIp()
{
	return sIndustrialCameraIp;
}
//设置相机端口
void Setting::setIndustrialCameraPort(int port)
{
	nIndustrialCameraPort = port;
}
//获取相机端口
int Setting::getIndustrialCameraPort()
{
	return nIndustrialCameraPort;
}

//设置热像仪IP
void Setting::setThermalImagerIp(std::string ipAddr)
{
	
	sThermalImagerIp = ipAddr;
}
//获取热像仪IP
std::string Setting::getThermalImagerIp()
{
	return sThermalImagerIp;
}
//设置热像仪端口
void Setting::setThermalImagerPort(int port)
{
	nThermalImagerPort = port;
}
//获取热像仪端口
int Setting::getThermalImagerPort()
{
	return nThermalImagerPort;
}



