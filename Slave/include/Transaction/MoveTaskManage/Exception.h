#ifndef __MOVEEXCEPTION_H
#define __MOVEEXCEPTION_H

#include "ThirdParty/jsoncpp/json.h"
#include "MoveTaskComponent.h"

class Exception
{
public:
    Exception() {};
	Exception(Json::Value& paraJson);
    ~Exception() {};

	Json::Value _paraJson; //动作参数

	void setParaJson(Json::Value& paraJson);
	Json::Value& getParaJson();
    Json::Value* getSharedParaJson();
    void setSharedParaJson(Json::Value* paraJson);
    void setSharedParent(MoveTaskComponent* parent);
    virtual void exceptionExit() {};

private:
    MoveTaskComponent* _sharedParent;

};


#endif
