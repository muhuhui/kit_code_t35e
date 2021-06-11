#ifndef CONTROLBOARD_H
#define CONTROLBOARD_H

#include "Driver/SerialDriver.h"
#include "Common/SingleTon.h"

enum MotionMode 
{
    Motion_Normal = 0,
    Motion_FRID
};


enum RadarControl
{
  Radar_Close = 0,
  Radar_Open
};

enum MotorWarn {
    MotorErr_OK = 0,
    Motor_Err_no = 0x1516,      //不可识别故障
    Motor_Err_oc = 0x160C,      //过流
    Motor_Err_oL = 0x1614,      //过载
    Motor_Err_oH = 0x1611,      //放电回路频繁动作平均功率大
    Motor_Err_LH = 0x1411,      //放电报警瞬时功率大
    Motor_Err_oU = 0x161C,      //过压
    Motor_Err_PA = 0x170A,      //参数故障
    Motor_Err_IC = 0x120C,      //电流采样故障
    Motor_Err_EC = 0x0E0C,      //编码器故障ABZ报警
    Motor_Err_PE = 0x170E,      //位置超差
    Motor_Err_St = 0x1A1B,      //失速
    Motor_Err_LU = 0x141C,      //欠压
    Motor_Err_EH = 0x0E11,      //编码器故障UVW报警
    Motor_Err_Ao = 0x0A16,      //缺相保护
    Motor_Err_FE = 0x0F0E,      //FPGA配置失败
    Motor_Err_Id = 0x120D,      //输入口功能定义重复
    Motor_Err_dE = 0x0D0E,      //功率母线未准备好
    Motor_Err_SL = 0x1A14,      //速度指令太小
    Motor_Err_Fd = 0x0F0D,      //FPGA死机
    Motor_Err_cE = 0x0C0E,      //通讯错误
    Motor_Err_bE = 0x0B0E,      //协同模式报警
    Motor_Err_Lo = 0x1416       //CAN总线断线报警
};


#endif // CONTROLBOARD_H
