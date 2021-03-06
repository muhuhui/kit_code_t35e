//
//
//  Generated by StarUML(tm) C++ Add-In
//
//  @ Project : T3C_SLAVE
//  @ File Name : GotoCommand.h
//  @ Date : 2019-06-27
//  @ Author : ShunWang
//
//


#if !defined(_PDDDETCOMMAND_H)
#define _PDDDETCOMMAND_H

#include "Command.h"
enum PDDGOTO_STATUS
{
	PDDGOTO_ST_SNDCMD = 0,    //发送goto指令
	PDDGOTO_ST_WAITRESULT,    //等待执行结果
	PDDGOTO_ST_BACK,          //等待执行结果
	PDDGOTO_ST_BACKRESULT,    //等待执行结果
    PDDGOTO_ST_FINSIH
};

class PddDetCommand : public Command
{
public:
    PddDetCommand() { name = "PddDetCommand"; }
private:
	int exec();
};

#endif  //_PDDDETCOMMAND_H
