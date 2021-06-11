#ifndef RadarDriver_T30UX_H
#define RadarDriver_T30UX_H

#include "base/I2CBase.h"
#include "Driver/RadarInterface.h"


typedef enum {
    I2C_AIN0 = 0x04, 
    I2C_AIN1 = 0x05, 
    I2C_AIN2 = 0x06,
    I2C_AIN3 = 0x07
} I2C_MUX;

#define I2C_FRONT_RADAR I2C_AIN0
#define I2C_BACK_RADAR I2C_AIN1
#define I2C_DEV_NAME "/dev/i2c-0"
#define I2C_RADAR_ADDR 0x48

class RadarDriver_T30UX : public RadarInterface
{
public:
    RadarDriver_T30UX(): RadarInterface(I2C_INTERFACE) {}
    ~RadarDriver_T30UX();

    virtual int init(char* i2cDevName, char i2cAddr);

    virtual int us_readVal(char addr, unsigned short &val);

private:
    I2CBase*                m_pI2CBase;
};

#endif // RadarDriver_T30UX_H
