#include "Driver/InfraredDriver.h"
#include <thread>
#include <stdio.h>
#include <unistd.h>

#define FRONT_INFRARED    28
#define BACK_INFRARED     30

InfraredDriver::~InfraredDriver()
{
  m_bThreadRun = false;
}

InfraredDriver *InfraredDriver::getInstance()
{
  return SingleTon<InfraredDriver>::GetInstance();
}

int InfraredDriver::init()
{
//  int pins[2] = {FRONT_INFRARED, BACK_INFRARED};

//  m_bThreadRun = true;
//  m_bEnabled = true;

//  m_eInfraredWarnType = INFRARED_NONE_WARN;

//  m_pGPIOBase = GPIOBase::getInstance();
//  m_pGPIOBase->initMonitor(pins, 2);

//  std::thread runThread;
//  runThread = std::thread(InfraredDriver::run, this);
//  runThread.detach();
  return 0;
}

InfraredWarnType InfraredDriver::getWarnType()
{
  return m_eInfraredWarnType;
}

void InfraredDriver::setEnabled(bool bEnabled)
{
  m_bEnabled = bEnabled;
}

void InfraredDriver::attach(Observers *pObserver)
{
  m_ObserverList.push_back(pObserver);
}

void InfraredDriver::detach(Observers *pObserver)
{
  m_ObserverList.remove(pObserver);
}

void InfraredDriver::notify()
{
  std::list<Observers *>::iterator it = m_ObserverList.begin();
  while (it != m_ObserverList.end())
  {
    (*it)->update(SUB_INFRARED);
    ++it;
  }
}

void InfraredDriver::judgeWarn(int nWarnType)
{
//  int nVal = 0, nResult = 0;

//  nVal = m_pGPIOBase->getValue(INFRARED_FRONT_WARN);
//  nResult += nVal * 1;

//  nVal = m_pGPIOBase->getValue(INFRARED_BACK_WARN);
//  nResult += nVal * 2;

  InfraredWarnType eWarnType = (InfraredWarnType)nWarnType;
  if(m_bEnabled && eWarnType != m_eInfraredWarnType) {
    m_eInfraredWarnType = eWarnType;
    notify();
  }
}

void InfraredDriver::run(InfraredDriver *pInfraredDriver)
{
//  printf("--- InfraredDriver run Start...\n");

//  const int nBaseFrequence = 200000;   //0.2s

//  while (pInfraredDriver->m_bThreadRun) {

//    pInfraredDriver->judgeWarn();
//    usleep(nBaseFrequence);
//  }
//  printf("--- InfraredDriver run Over...\n");
}
