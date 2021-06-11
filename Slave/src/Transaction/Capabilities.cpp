/*================================================================
*   Copyright (C) 2020 Tetraelc Ltd. All rights reserved.
*   
*   File name: Capabilities.cpp
*   Author: JiangJin
*   Create Date: 2020-10-14
*   Description: 
*
================================================================*/


#include "Transaction/Capabilities.h"

Capabilities::Capabilities():
    m_armCapability(false, false),
    m_bIsSupportPDDetect(false),
    m_radarCapability(true, false, RADAR_KS104),
    m_chargeCapability(true, true),
    m_bIsSupport2DCoordinate(false)

{

}

Capabilities* Capabilities::getInstance()
{
    static Capabilities capabilities;
    return &capabilities;
}
    
