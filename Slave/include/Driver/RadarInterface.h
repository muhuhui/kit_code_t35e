#ifndef RadarInterface_H
#define RadarInterface_H

#include <string>

typedef enum {
    I2C_INTERFACE = 0,
    UART_INTERFACE = 1
} RadarInfType;

class RadarInterface
{
public:
    RadarInterface(RadarInfType type): m_InfType(type) {}

    virtual int init(char* i2cDevName, char i2cAddr) { return 0; }

    virtual int init(const char *sDevice, int nBaud, int nDataBit, int nParity, int nStopBit) { return 0; }

    virtual int us_readVal(char addr, unsigned short &val) = 0;

    std::string getDevName() { return m_sDevName; }
    void setDevName(std::string name) { m_sDevName = name; }

    RadarInfType getInfType() { return m_InfType; }

private:
    RadarInfType m_InfType = UART_INTERFACE;
    std::string m_sDevName;
};

#endif // RadarInterface_H
