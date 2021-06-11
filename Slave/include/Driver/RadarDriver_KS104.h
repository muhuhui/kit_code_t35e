#ifndef RadarDriver_KS104_H
#define RadarDriver_KS104_H

#include "Driver/RadarInterface.h"
#include "base/UartPortSelect.h"

#define UART_FRONT_RADAR      0xe8
#define UART_BACK_RADAR       0xea
#define UART_DOWN_RADAR       0xec

class RadarDriver_KS104 : public RadarInterface
{
public:
    RadarDriver_KS104(): RadarInterface(UART_INTERFACE) {}
    
    ~RadarDriver_KS104();

    virtual int init(const char *sDevice, int nBaud, int nDataBit, int nParity, int nStopBit);

    virtual int us_readVal(char nAddr, unsigned short &nVal);

private:
    UartPortSelect* m_pUartSel;
};

#endif // RadarDriver_KS104_H
