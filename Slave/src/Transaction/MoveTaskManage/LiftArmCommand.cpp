#include "LiftArmCommand.h"
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include "Transaction/Motor.h"

int LiftArmCommand::exec()
{
//解析JSON参数
    int liftArmCmd = _paraJson["liftArmCmd"].asInt();
    Motor *pMotor = Motor::getInstance();

    int Ret = 0;
    if(_exeRet!=ST_SUCCESS && _exeRet!=ST_FAIL)
    {
        switch(liftArmCmd)
        {
            case Motion_Arm_Up:
            {
                printf("move control command:arm up\n");
                Ret=pMotor->armUp();
                if(0==Ret)
                _exeRet=ST_SUCCESS;
                else
                _exeRet=ST_FAIL;
                break;
            }
            case Motion_Arm_Down:
            {
                printf("move control command:arm down\n");
                Ret=pMotor->armDown();
                if(0==Ret)
                _exeRet=ST_SUCCESS;
                else
                _exeRet=ST_FAIL;
                break;
            }
            default:
            {
                _exeRet=ST_FAIL;
                break;
            }
        }
    }
    return _exeRet;
}

