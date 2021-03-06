//
//
//  Generated by StarUML(tm) C++ Add-In
//
//  @ Project : T3C_SLAVE
//  @ File Name : RunModeCommand.cpp
//  @ Date : 2019-06-25
//  @ Author : ShunWang
//
//


#include "RunModeCommand.h"
#include "Transaction/Motor.h"

int RunModeCommand::exec()
{
	//解析JSON参数
	int mode = _paraJson["mode"].asInt();
    Motor *motor = Motor::getInstance();
	if(_exeStatus != ST_SUCCESS && _exeStatus != ST_FAIL)
	{
        int ret = motor->setMotionMode((RunMode)mode);
		if(0 == ret)
		{
			_exeStatus =ST_SUCCESS;
		}
		else
		{
			_exeStatus = ST_FAIL;
		}
	}
	return _exeStatus;	
	
}

