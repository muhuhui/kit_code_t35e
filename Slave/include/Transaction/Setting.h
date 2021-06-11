#ifndef SETTING_H
#define SETTING_H

#include "Common/SingleTon.h"
#include <string>
#include <list>

#define begin_pos "orbit_start_position"
#define end_pos "orbit_end_position"
#define arm_begin_pos "arm_begin_pos"
#define arm_end_pos "arm_end_pos"
#define default_speed "speed"
#define acceleration "orbit_acceleration"
#define deceleration "orbit_deceleration"
#define run_mode "run_mode"
#define motor_lock "lock"
#define battery_alarm_quantity "battery_alarm_quantity"
#define battery_alarm_temperature "battery_alarm_temperature"
#define camera_ip "camera_ip"
#define camera_tcp_port "camera_tcp_port"
#define thermal_imager_ip "thermal_imager_ip"
#define thermal_imager_tcp_port "thermal_imager_tcp_port"
#define camera_user_name "camera_user_name"
#define camera_password "camera_password"
#define picture_path "rtmp_picture_url"
#define video_path "rtmp_video_url"
#define radar_switch "radar_switch"
#define ptz_switch "ptz_switch"
#define move_warn "move_warn"
#define ulsound_switch "ulsound_switch"
#define ulsound_switch_back "ulsound_switch_back"
#define ulsound_switch_down "ulsound_switch_down"
#define ptz_sleep_switch "ptz_sleep_switch"
#define ul_sound_threshold "obstacle_alarm_distance"
#define back_ul_sound_threshold "back_obstacle_alarm_distance"
#define down_ul_sound_threshold "down_obstacle_alarm_distance"
#define arm_restrict_switch "lift_manual_switch"
#define arm_lift_switch "lift_switch"
#define driver_msg_switch "driver_msg_switch"
#define home_switch "home_switch"

enum RunMode {
    TASK_MODE = 0,
    REMOTE_CONTROL_MODE,
    HANDLE_CONTROL_MODE,
    SCAN_MODE,
    NONE_MODE,
};

class Setting
{
public:
    Setting();
    ~Setting();

    void setPtzPowerSwitch(bool PtzSwitch) { m_bPtzPowerSwitch = PtzSwitch; }
    bool getPtzPowerSwitch() { return m_bPtzPowerSwitch; }

    void setPtzSleepSwitch(bool PtzSwitch) { m_bPtzSleepSwitch = PtzSwitch; }
    bool getPtzSleepSwitch() { return m_bPtzSleepSwitch; }

    void setUlsoundSwitch(bool UlsoundSwitch) { m_bUlsoundSwitch = UlsoundSwitch; }
    bool getUlsoundSwitch() { return m_bUlsoundSwitch; }

    void setBackUlsoundSwitch(bool UlsoundSwitch) { m_bBackUlsoundSwitch = UlsoundSwitch; }
    bool getBackUlsoundSwitch() { return m_bBackUlsoundSwitch; }

    void setDownUlsoundSwitch(bool UlsoundSwitch) { m_bDownUlsoundSwitch = UlsoundSwitch; }
    bool getDownUlsoundSwitch() { return m_bDownUlsoundSwitch; }

    void setPtzSwitch(bool ptzSwitch) { m_bPtzSwitch = ptzSwitch; }
    bool getPtzSwitch() { return m_bPtzSwitch; }

    void setDriverMsgSwitch(bool drvSwitch) { m_bDrvSwitch = drvSwitch; }
    bool getDriverMsgSwitch() { return m_bDrvSwitch; }

    void setRadarSwitch(bool radarSwitch) { m_bRadarSwitch = radarSwitch; }
    bool getRadarSwitch() { return m_bRadarSwitch; }

    void setArmLiftMoving( bool armLiftMoving ) { m_bArmLiftMoving = armLiftMoving; }
    bool getArmLiftMoving( void ) { return m_bArmLiftMoving; }

    void setBeginPos(float pos) { m_fBeginPos = pos; }
    float getBeginPos() { return m_fBeginPos; }

    void setEndPos(float pos) { m_fEndPos = pos; };
    float getEndPos() { return m_fEndPos; }

    void setArmBeginPos(float pos) { m_fArmBeginPos = pos; }
    float getArmBeginPos() { return m_fArmBeginPos; }

    void setArmEndPos(float pos) { m_fArmEndPos = pos; }
    float getArmEndPos() { return m_fArmEndPos; }

    void setDefaultSpeed(float speed) { m_fDefaultSpeed = speed; }
    float getDefaultSpeed() { return m_fDefaultSpeed; }

    void setLiftRestrictSwitch( bool liftSwitch) {m_bLiftRestrictSwitch = liftSwitch;}    
    bool getLiftRestrictSwitch() {return m_bLiftRestrictSwitch;}

    void setHomeSwitch( bool liftSwitch) {m_bHomeSwitch = liftSwitch;}    
    bool getHomeSwitch() {return m_bHomeSwitch;}
    
    void setAcc(float acc) { m_fAcc = acc; }
    float getAcc() { return m_fAcc; }

    void setDec(float dec) { m_fDec = dec; }
    float getDec() { return m_fDec; }

    void setRunMode(RunMode mode) { m_nRunMode = mode; }
    RunMode getRunMode() { return m_nRunMode; }

    void setMotorLock(bool isLock) { m_bMotorIsLock = isLock; } 
    bool getMotorLock() { return m_bMotorIsLock; } 

    void setBatteryQuantityMin(float fValue);
    float getBatteryQuantityMin();

    void setIndustrialCameraIp(std::string ipAddr);
    std::string getIndustrialCameraIp();

    void setIndustrialCameraPort(int port);
    int getIndustrialCameraPort();

    void setThermalImagerIp(std::string ipAddr);
    std::string getThermalImagerIp();

    void setThermalImagerPort(int port);
    int getThermalImagerPort(); 

    void setCameraUserName(std::string cameraUserName){this->cameraUserName = cameraUserName;}
    std::string getCameraUserName(){return cameraUserName;}
    void setCameraPassword(std::string cameraPassword){this->cameraPassword = cameraPassword;}
    std::string getCameraPassword(){return cameraPassword;}

    void setPicturePath(std::string path) { m_sPicturePath = path;}
    std::string getPicturePath() { return m_sPicturePath; }

    void setVideoPath(std::string path) { m_sVideoPath = path;}
    std::string getVideoPath() { return m_sVideoPath; }

    void setULSoundThreshold(int thd) {m_ulSoundThreshold = thd;};
    int getULSoundThreshold(){return m_ulSoundThreshold;};

    void setbackULSoundThreshold(int thd) {m_backulSoundThreshold = thd;};
    int getbackULSoundThreshold(){return m_backulSoundThreshold;};

    void setdownULSoundThreshold(int thd) {m_downulSoundThreshold = thd;};
    int getdownULSoundThreshold(){return m_downulSoundThreshold;};

    int getMoveWarn() { return m_nMoveWarn; }
    void setMoveWarn(int moveWarn) { m_nMoveWarn = moveWarn; }
    
private:
    bool                    m_bUlsoundSwitch = true;            //超声波开关
    bool                    m_bPtzSwitch = true;                //移动时云台转向开关
    bool                    m_bRadarSwitch = true;              //雷达开关
    bool                    m_bArmLiftMoving = true;            //移动前机臂复位
    float                   m_fBeginPos = 0;                    //起点
    float                   m_fEndPos = 1000000;                //终点
    float                   m_fArmBeginPos = 0;                 //升降臂起点
    float                   m_fArmEndPos = 1000;                //升降臂终点
    int                     m_ulSoundThreshold = 0;             //超声波侦测距离
    float                   m_fDefaultSpeed = 500;              //默认运行速度
    float                   m_fAcc = 400;                       //运动加速度
    float                   m_fDec = 2000;                      //运动减速度
    RunMode                 m_nRunMode = REMOTE_CONTROL_MODE;   //运行模式
    bool                    m_bMotorIsLock = false;             //运动控制锁定
    float                   m_fBatteryQuantityMin = 30;         //回充电量
    std::string     		sIndustrialCameraIp;                //可见光ip
    int             		nIndustrialCameraPort;              //可见光port
    std::string     		sThermalImagerIp;                   //热像仪ip
    int             		nThermalImagerPort;                 //热像仪port
    std::string             cameraUserName;                     //相机用户名
    std::string             cameraPassword;                     //相机用户密码
    std::string             m_sPicturePath;                     //服务器图片上传路径
    std::string             m_sVideoPath;                       //服务器视频上传路径
    int                     m_nMoveWarn = 0;                        //行走声光告警
    bool                    m_bBackUlsoundSwitch=true;          //后超声波
    bool                    m_bDownUlsoundSwitch=true;          //下超声波
    bool                    m_bPtzSleepSwitch = false;          //云台休眠开关
    int                     m_backulSoundThreshold = 0;             //后超声波侦测距离
    int                     m_downulSoundThreshold = 0;             //下超声波侦测距离
    bool                    m_bPtzPowerSwitch = 1;              //云台给电开关
    bool                    m_bLiftRestrictSwitch = false;        //升降臂收起限制开关
    bool                    m_bDrvSwitch = false;               //驱动板交互信息开关
    bool                    m_bHomeSwitch = true;               //巡检前归零开关
};

#endif // SETTING_H
