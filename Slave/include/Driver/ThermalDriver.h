#ifndef THERMALDRIVERADAPTER_H
#define THERMALDRIVERADAPTER_H
#include "Common/SingleTon.h"
#include <string>


class ThermalDriver 
{
public:
    ~ThermalDriver();
    static ThermalDriver *getInstance();
    void init(std::string ip, std::string userName,std::string password);
    int close();
    int connect();
    int disconnect();
    int takePicture(char* sFileName);
    int setFocus(int nFocus);
    float getMaxTemp();
    int detectDevice();
private:
	char xDeviceAddr[128];
	std::string ip;
	std::string userName;
	std::string password;
	
};

#endif // THERMALDRIVER_H
