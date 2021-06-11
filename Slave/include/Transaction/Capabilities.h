/*================================================================
*   Copyright (C) 2020 Tetraelc Ltd. All rights reserved.
*   
*   File name: Capabilities.h
*   Author: JiangJin
*   Create Date: 2020-10-14
*   Description: 
*
================================================================*/


#pragma once
#include <iostream>

typedef enum 
{
    RADAR_NONE = 0,
    RADAR_KS104 = 1,
    RADAR_T30UX,
} RadarType;


class Capabilities
{
public:
    Capabilities();
    static Capabilities* getInstance();

    void setSupportPtzMotor(bool support) { m_bIsSupportPtzControl = support; }
    bool isSupportPtzMotor() { return m_bIsSupportPtzControl; }

    void setSupportArmMotor(bool support) { m_armCapability.m_bIsSupportArm = support; }
    bool isSupportArmMotor() { return m_armCapability.m_bIsSupportArm; }
    void setSupportArmReverse(bool support) { m_armCapability.m_bIsSupportArmReverse = support; }
    bool isSupportArmReverse() { return m_armCapability.m_bIsSupportArmReverse; }

    void setSupportPDDetect(bool support) { m_bIsSupportPDDetect = support; }
    bool isSupportPDDetect() { return m_bIsSupportPDDetect; }

    void setSupportSeerBattery(bool support) { m_bIsSupportSeerBat = support; }
    bool isSupportSeerBattery() { return m_bIsSupportSeerBat; }

    void setSupportRadarManage(bool support) { m_radarCapability.m_bIsSupportRadarManage = support; }
    bool isSupportRadarManage() { return m_radarCapability.m_bIsSupportRadarManage; }
    void setSupportRadarBelow(bool support) { m_radarCapability.m_bIsSupportRadarBelow = support; }
    bool isSupportRadarBelow() { return m_radarCapability.m_bIsSupportRadarBelow; }
    void setSupportRadarType(RadarType type) { m_radarCapability.m_radarType = type; }
    RadarType getSupportRadarType() { return m_radarCapability.m_radarType; }

    void setSupportBatteryCharge(bool support) { m_chargeCapability.m_bIsSupportBatteryCharge = support; }
    bool isSupportBatteryCharge() { return m_chargeCapability.m_bIsSupportBatteryCharge; }
    void setSupportChargeControl(bool support) { m_chargeCapability.m_bIsSupportChargeControl = support; }
    bool isSupportChargeControl() { return m_chargeCapability.m_bIsSupportChargeControl; }

    void setSupport2DCoordinate(bool support) { m_bIsSupport2DCoordinate = support; }
    bool isSupport2DCoordinate() { return m_bIsSupport2DCoordinate; }
private:
    struct ArmMotorCapability {
        ArmMotorCapability(bool supportArm, bool supportArmReverse) {
            this->m_bIsSupportArm = supportArm;
            this->m_bIsSupportArmReverse = supportArmReverse;
        }
        bool m_bIsSupportArm;               //支持升降臂
        bool m_bIsSupportArmReverse;        //升降臂反向行走
    } m_armCapability;
    bool m_bIsSupportPDDetect;
    bool m_bIsSupportSeerBat;
    struct RadarCapability {
        RadarCapability(bool supportManage, bool supportRadarBelow, RadarType type) {
            this->m_bIsSupportRadarManage = supportManage;
            this->m_bIsSupportRadarBelow = supportRadarBelow;
            this->m_radarType = type;
        }

        bool m_bIsSupportRadarManage;       //支持雷达
        bool m_bIsSupportRadarBelow;        //支持下方雷达
        RadarType m_radarType;              //雷达型号
    } m_radarCapability;
    struct ChargeCapability {
        ChargeCapability(bool supportBattery, bool supportControl) {
            this->m_bIsSupportBatteryCharge = supportBattery;
            this->m_bIsSupportChargeControl = supportControl;
        }
        bool m_bIsSupportBatteryCharge;     //支持电池
        bool m_bIsSupportChargeControl;     //手动控制充电
    } m_chargeCapability;
    bool m_bIsSupport2DCoordinate;          //二维坐标系
    bool m_bIsSupportPtzControl;
};

