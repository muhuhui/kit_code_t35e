#ifndef OBSERVERS_H
#define OBSERVERS_H

#include <iostream>
#include <list>

using namespace std;

enum SubType {
  SUB_CHARGEPILE = 0,
  SUB_FIREDOOR,
  SUB_HALL,
  SUB_INFRARED,
  SUB_TOUCH,
  SUB_ULTRASOUND,
  SUB_RFID,
  SUB_MOTOR_WARN,
  SUB_LIFT_MOTOR_WARN,
};

class Observers
{
public:
    virtual void update(int nSub) = 0;
};

#endif // OBSERVERS_H
