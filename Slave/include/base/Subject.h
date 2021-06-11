#ifndef SUBJECT_H
#define SUBJECT_H

#include <iostream>
#include <list>
#include "base/Observer.h"

using namespace std;

class Subject
{
public:
    virtual void attach(Observer *) = 0;
    virtual void detach(Observer *) = 0;
    virtual void notify() = 0;
};

#endif //SUBJECT_H
