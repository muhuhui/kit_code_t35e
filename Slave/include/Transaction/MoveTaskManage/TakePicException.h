#ifndef __TAKEPICEXCEPTION_H
#define __TAKEPICEXCEPTION_H

#include "Transaction/MoveTaskManage/Exception.h"
#include "ThirdParty/jsoncpp/json.h"

class TakePicException: public Exception
{
public:
    void exceptionExit();
};

#endif
