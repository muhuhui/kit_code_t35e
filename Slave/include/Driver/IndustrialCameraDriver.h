#ifndef INDUSTRIALCAMERADRIVER_H
#define INDUSTRIALCAMERADRIVER_H

#include "Common/SingleTon.h"
#include "ThirdParty/onvif/onvifCamera.h"
#include <string>

typedef enum
{
	CLE_PRESET, 
	SET_PRESET, 
	GOTO_PRESET //ת��Ԥ�õ�
} PRESET_OPT_E;

class IndustrialCameraDriver
{
public:
    ~IndustrialCameraDriver();
    static IndustrialCameraDriver *getInstance();
    void init(std::string ip, std::string userName,std::string password);
    int connect();
    int disconnect();
    int rebootDevice();
    /*
    typedef 
        PAN_LEFT, PAN_RIGHT, TILT_UP, TILT_DOWN, ZOOM_IN, ZOOM_OUT
    */
    int presetOperation(int nCmd, int nPresetIndex);  
    int platformOperation(int nCmd, int nStop, int nSpeed);
    int takePicture(char* sFileName);
    int detectDevice();

private:
    std::string ip; //�û���
	std::string userName; //�û���
	std::string password; //����
	char xDeviceAddr[128];      //onvifɨ�赽�ķ����ַ

};

#endif // INDUSTRIALCAMERADRIVER_H
