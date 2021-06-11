#ifndef SUBJECTS_H
#define SUBJECTS_H

#include <iostream>
#include <list>
#include "base/Observers.h"

using namespace std;

class Subjects
{
public:
    virtual void attach(Observers *) = 0;
    virtual void detach(Observers *) = 0;
    virtual void notify() = 0;
};

#endif // SUBJECTS_H
