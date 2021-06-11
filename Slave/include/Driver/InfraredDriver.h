#ifndef INFRAREDDRIVER_H
#define INFRAREDDRIVER_H


#include "base/SingleTon.h"
#include "base/GPIOBase.h"
#include "base/Subjects.h"
#include "base/Observers.h"


enum InfraredWarnType {
  INFRARED_NONE_WARN = 0,
  INFRARED_FRONT_WARN = 1,
  INFRARED_BACK_WARN = 2,
  INFRARED_BOTH_WARN = 3
};

class InfraredDriver : public Subjects
{
public:
  ~InfraredDriver();
  static InfraredDriver *getInstance();

  int init();
  InfraredWarnType getWarnType();

  void setEnabled(bool bEnabled);

  void attach(Observers *pObserver);
  void detach(Observers *pObserver);
  void notify();

  static void run(InfraredDriver *pInfraredDriver);

  void judgeWarn(int nWarnType);

private:
  bool                    m_bThreadRun;
  bool                    m_bEnabled;

  InfraredWarnType        m_eInfraredWarnType;

  std::list<Observers* >   m_ObserverList;

  GPIOBase*               m_pGPIOBase;
};

#endif // INFRAREDDRIVER_H
