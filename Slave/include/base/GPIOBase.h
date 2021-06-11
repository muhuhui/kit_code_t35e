#ifndef GPIOBASE_H
#define GPIOBASE_H

#include "base/SingleTon.h"
#include <map>
#include <queue>
#include <utility>


class GPIOBase
{
public:
  ~GPIOBase();
  static GPIOBase *getInstance();

  int initMonitor(int *pins, int nCnt);
  int getValue(int nPin);
  void setValue(int nPin, int nValue);

  static void run(GPIOBase *pGPIOBase);

private:
  void getPins();
  int readPin(int nPin, int &nVal);
  void writePins();
  int writePin(int nPin, int nVal);

private:
  bool                    m_bInit = false;
  bool                    m_bThreadRun;

  std::queue<std::pair<int, int>> m_writePins;
  std::map<int, int>      m_pinMap;   // <pin_num, value>
};

#endif // GPIOBASE_H
