#ifndef INDUSTRIALCAMERA_H
#define INDUSTRIALCAMERA_H

#include "Driver/IndustrialCameraDriver.h"
#include "base/SingleTon.h"
#include <mutex>

class IndustrialCamera
{
public:
    ~IndustrialCamera();
    IndustrialCamera();
    void init();
    static IndustrialCamera *getInstance();
    int connect();
    int disconnect();
    void setIp(std::string ip){this->ip = ip;}
    std::string getIp(){return ip;}
    void setUserName(std::string userName){this->userName = userName;}
    std::string getUserName(){return userName;}
    void setPassword(std::string password){this->password = password;}
    std::string getPassword(){return password;}
    /*
        CLE_PRESET, SET_PRESET, GOTO_PRESET, LIGHT_PWRON
    */
    int presetOperation(int nCmd, int nPresetIndex);

    /*
        PAN_LEFT, PAN_RIGHT, TILT_UP, TILT_DOWN, ZOOM_IN, ZOOM_OUT
    */
    int platformOperation(int nCmd, int nStop, int nSpeed = 5);

    int takePicture(char* sFileName);
    int detectDevice();
    int rebootDevice();
private:
    IndustrialCameraDriver*      m_pIndustrialCameraDriver;
	std::string	ip; //IP地址
	std::string userName;
	std::string password;
};

#endif // INDUSTRALCAMERA_H
