#ifndef TOUCHDRIVER_H
#define TOUCHDRIVER_H

#include "base/SingleTon.h"
#include "base/GPIOBase.h"
#include "base/Subjects.h"
#include "base/Observers.h"


enum TouchWarnType {
  TOUCH_NONE_WARN = 0,
  TOUCH_FRONT_WARN = 1,
  TOUCH_BACK_WARN = 2,
  TOUCH_BOTH_WARN = 3
};

class TouchDriver : public Subjects
{
public:
  ~TouchDriver();
  static TouchDriver *getInstance();

  int init();
  TouchWarnType getWarnType();

  void attach(Observers *pObserver);
  void detach(Observers *pObserver);
  void notify();

  static void run(TouchDriver *pTouchDriver);

  void judgeWarn(int nWarnType);

private:
  bool                    m_bThreadRun;
  TouchWarnType           m_eTouchWarnType;

  std::list<Observers* >   m_ObserverList;

  GPIOBase*               m_pGPIOBase;
};

#endif // TOUCHDRIVER_H
