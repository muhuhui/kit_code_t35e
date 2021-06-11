#include "Transaction/Setting.h"

Setting::Setting()
{
  m_fBatteryQuantityMin = 30.0;
}

Setting::~Setting()
{

}

//���õ����С�������
void Setting::setBatteryQuantityMin(float fValue)
{
  m_fBatteryQuantityMin = fValue;
}

//��ȡ�����С�������
float Setting::getBatteryQuantityMin()
{

  return m_fBatteryQuantityMin;
}

//�������IP
void Setting::setIndustrialCameraIp(std::string ipAddr)
{
	sIndustrialCameraIp = ipAddr;
}
//��ȡ���IP
std::string Setting::getIndustrialCameraIp()
{
	return sIndustrialCameraIp;
}
//��������˿�
void Setting::setIndustrialCameraPort(int port)
{
	nIndustrialCameraPort = port;
}
//��ȡ����˿�
int Setting::getIndustrialCameraPort()
{
	return nIndustrialCameraPort;
}

//����������IP
void Setting::setThermalImagerIp(std::string ipAddr)
{
	
	sThermalImagerIp = ipAddr;
}
//��ȡ������IP
std::string Setting::getThermalImagerIp()
{
	return sThermalImagerIp;
}
//���������Ƕ˿�
void Setting::setThermalImagerPort(int port)
{
	nThermalImagerPort = port;
}
//��ȡ�����Ƕ˿�
int Setting::getThermalImagerPort()
{
	return nThermalImagerPort;
}



