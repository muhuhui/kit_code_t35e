//
//
//  Generated by StarUML(tm) C++ Add-In
//
//  @ Project : T3C_SLAVE
//  @ File Name : TakePicCommand.h
//  @ Date : 2019-07-02
//  @ Author : ShunWang
//
//


#if !defined(_TAKEPICCOMMAND_H)
#define _TAKEPICCOMMAND_H

#include"Transaction/MoveTaskManage/Command.h"
#include"Transaction/ViewPoint.h"

/* enum POINT_NUM
{
    POINT_FIRST = 0,
    POINT_LAST,
}; */
enum TAKEPIC_STATUS
{
	TAKEPIC_ST_ROTATE = 0,   //转到预置点
	TAKEPIC_ST_WAIT,         //等待6S
	TAKEPIC_ST_CAPTURE,       //截屏
    PTZ_ST_CAPTURE,
    PDD_ST_WAIT,
    PDD_ST_CAPTURE
};
class TakePicCommand : public Command
{
public:
    TakePicCommand(){ name = "TakePicCommand"; }
private:
    int exec();
};

#endif  //_TAKEPICCOMMAND_H
