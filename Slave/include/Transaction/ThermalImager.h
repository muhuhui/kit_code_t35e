#ifndef THERMALIMAGER_H
#define THERMALIMAGER_H

#include "Driver/ThermalDriver.h"
#include <mutex>

class ThermalImager
{
public:
    ~ThermalImager();
    static ThermalImager *getInstance();
    int init();
    int close();
    int connect();
    int disconnect();
    void setIp(std::string ip){this->ip = ip;}
    std::string getIp(){return this->ip;}
    void setUserName(std::string userName){this->userName = userName;}
    std::string getUserName(){return userName;}
    void setPassword(std::string password){this->password = password;}
    std::string getPassword(){return password;}    
    int takePicture(char* sFileName);
    int setFocus(int nFocus);
    float getMaxTemp();
    int detectDevice();

private:
    ThermalDriver*   m_pThermalDriver; //红外相机驱动
    std::string	ip;                 //ip地址
	std::string userName;              //用户名  
	std::string password;              //密码  
};
#endif // THERMALIMAGER_H
