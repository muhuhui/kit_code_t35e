//
//
//  Generated by StarUML(tm) C++ Add-In
//
//  @ Project : T3C_SLAVE
//  @ File Name : RunModeCommand.h
//  @ Date : 2019-06-25
//  @ Author : ShunWang
//
//


#if !defined(_RUNMODECOMMAND_H)
#define _RUNMODECOMMAND_H

#include"Transaction/MoveTaskManage/Command.h"

class RunModeCommand : public Command
{
public:
    RunModeCommand() { name = "RunModeCommand"; }
private:
    int exec();
};

#endif  //_RUNMODECOMMAND_H
