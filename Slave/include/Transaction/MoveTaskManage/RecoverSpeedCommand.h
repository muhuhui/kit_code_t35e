//
//
//  Generated by StarUML(tm) C++ Add-In
//
//  @ Project : T3C_SLAVE
//  @ File Name : RecoverSpeedCommand.h
//  @ Date : 2019-06-25
//  @ Author : ShunWang
//
//


#if !defined(_RECOVERSPEEDCOMMAND_H)
#define _RECOVERSPEEDCOMMAND_H

#include"Transaction/MoveTaskManage/Command.h"

class RecoverSpeedCommand : public Command
{
public:
    RecoverSpeedCommand() { name = "RecoverSpeedCommand"; }
private:
    int exec();
};

#endif  //_RECOVERSPEEDCOMMAND_H
