#include "Driver/IndustrialCameraDriver.h"
#include <stdio.h>
#include <string.h>

IndustrialCameraDriver::~IndustrialCameraDriver()
{

}

IndustrialCameraDriver *IndustrialCameraDriver::getInstance()
{
    return SingleTon<IndustrialCameraDriver>::GetInstance();
}

void IndustrialCameraDriver::init(std::string ip, std::string userName,std::string password)
{

    this->ip = ip;
    this->userName = userName;
    this->password = password;
}



int IndustrialCameraDriver::connect()
{	
    return cb_Connect(const_cast<char*>(ip.c_str()),xDeviceAddr,const_cast<char*>(userName.c_str()),const_cast<char*>(password.c_str()));
}

int IndustrialCameraDriver::disconnect()
{
	return 0;
}

int IndustrialCameraDriver::rebootDevice()
{
	return cb_SystemReboot(xDeviceAddr,const_cast<char*>(userName.c_str()),const_cast<char*>(password.c_str()));
}


int IndustrialCameraDriver::presetOperation(int nCmd, int nPresetIndex)
{
    int nRet = 0;
	switch(nCmd)
	{
		case GOTO_PRESET:
		{
			nRet = cb_PTZ_GotoPreset(xDeviceAddr, nPresetIndex, const_cast<char*>(userName.c_str()),const_cast<char*>(password.c_str()));
            printf("IndustrialCameraDriver::%s(), xDeviceAddr: %s, nPresetIndex: %d, userName: %s, password: %s\n", __FUNCTION__, xDeviceAddr, nPresetIndex, userName.c_str(), password.c_str());
			break;
		}
		default:
		{
			break;
		}
	}
    return nRet;
}

int IndustrialCameraDriver::platformOperation(int nCmd, int nStop, int nSpeed)
{
    return 0;
}

int IndustrialCameraDriver::takePicture(char* sFileName)
{
	return cb_Snapshot_Visible(xDeviceAddr,sFileName,const_cast<char*>(userName.c_str()),const_cast<char*>(password.c_str()));
}

int IndustrialCameraDriver::detectDevice()
{
    return ONVIF_DetectDeviceByIP(const_cast<char*>(ip.c_str()), xDeviceAddr);
}

