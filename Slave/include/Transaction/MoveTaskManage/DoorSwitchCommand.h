//
//
//  Generated by StarUML(tm) C++ Add-In
//
//  @ Project : T3C_SLAVE
//  @ File Name : DoorSwitchCommand.h
//  @ Date : 2019-06-25
//  @ Author : ShunWang
//
//


#if !defined(_DOORSWITCHCOMMAND_H)
#define _DOORSWITCHCOMMAND_H

#include "Command.h"

class DoorSwitchCommand : public Command
{
public:
    DoorSwitchCommand() { name =  "DoorSwitchCommand"; }
private:
	int exec();
};

#endif  //_DOORSWITCHCOMMAND_H
