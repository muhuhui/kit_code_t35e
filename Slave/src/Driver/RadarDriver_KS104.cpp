#include "Driver/RadarDriver_KS104.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>

RadarDriver_KS104::~RadarDriver_KS104()
{
}

int RadarDriver_KS104::init(const char *sDevice, int nBaud, int nDataBit, int nParity, int nStopBit)
{
  int nRet = 0;

  setDevName(sDevice);

  m_pUartSel = UartPortSelect::getInstance();

  nRet = m_pUartSel->init(sDevice, nBaud, nDataBit, nParity, nStopBit);
  printf("---RadarDriver_KS104::%s(), %s %d %d %d %d\n", __FUNCTION__, sDevice, nBaud, nDataBit, nParity, nStopBit);
  if(nRet != 0){
    printf("RadarDriver_KS104::%s(), fail to initiate %s device, nRet: %d\n", __FUNCTION__, sDevice, nRet);
  }

  return nRet;
}

int RadarDriver_KS104::us_readVal(char addr, unsigned short &nVal)
{
    char cmd[] = { addr, 0x02, (char)0xb0 };
    char rcvData[64] = {0};

    std::string sDev = getDevName();
    if (sDev.size() == 0) {
        printf("RadarDriver_KS104::%s(), device name is empty!\n", __FUNCTION__);
        return -1;
    }

    int rcvBytes = 2;
	int ret = 0;
	ret = m_pUartSel->queryUartFrame(sDev, cmd, sizeof(cmd), rcvData, sizeof(rcvData), rcvBytes);
    if (ret != 0) {
        printf("RadarDriver_KS104::%s(), uart send failed!\n", __FUNCTION__);
        return -1;
    }
    if (rcvBytes != 2) {
        printf("rcvBytes = %d, RadarDriver_KS104::%s(), uart get wrong bytes!\n", rcvBytes,__FUNCTION__);
        return -1;
    }
    unsigned short val = rcvData[0] << 8 | rcvData[1];
    // printf("addr:%d, val(%d)\n", addr, val);
    if (val > 6000) {
        printf("val(%d) out of range!\n", val);
        return -1;
    }
    nVal = val;

    return 0;
}


