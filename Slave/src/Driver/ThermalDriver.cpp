#include "Driver/ThermalDriver.h"
#include "ThirdParty/onvif/onvifCamera.h"
ThermalDriver::~ThermalDriver()
{

}

ThermalDriver *ThermalDriver::getInstance()
{
    return SingleTon<ThermalDriver>::GetInstance();
}

void ThermalDriver::init(std::string ip, std::string userName,std::string password)
{
	this->ip = ip;
	this->userName = userName;
	this->password = password;
}


int ThermalDriver::close()
{
	return 0;
}

int ThermalDriver::connect()
{

	int ret =  cb_Connect(const_cast<char*>(ip.c_str()),xDeviceAddr,const_cast<char*>(userName.c_str()), const_cast<char*>(password.c_str()));
	return ret;
}

int ThermalDriver::disconnect()
{
	return 0;
}

int ThermalDriver::takePicture(char* sFileName)
{
	return  cb_Snapshot_Infrared(xDeviceAddr,sFileName,const_cast<char*>(userName.c_str()), const_cast<char*>(password.c_str()));
}

int ThermalDriver::setFocus(int nFocus)
{
	return 0;
}

float ThermalDriver::getMaxTemp()
{
    int fRet = 0.0;
    return fRet;
}

int ThermalDriver::detectDevice()
{
    return ONVIF_DetectDeviceByIP(const_cast<char*>(ip.c_str()), xDeviceAddr);
}
