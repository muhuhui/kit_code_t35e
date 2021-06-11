#include "Transaction/Warning.h"


Warning::Warning()
{
  m_fSmognThreshold = 99.0;
  m_fIlluminanceThreshold = 99.0;
  m_fBatteryTempThreshold = 70.0;
  m_fO2Threshold = 20.0;
  m_fCOThreshold = 3.0;
  m_fSO2Threshold = 1.0;
  m_fCH4Threshold = 3.0;
}

Warning::~Warning()
{

}

void Warning::setRobotID(int nRobotID)
{
  m_nRobotID = nRobotID;
}

int Warning::getRobotID()
{
  return m_nRobotID;
}

void Warning::setSmogThreshold(float fThreshold)
{
  m_fSmognThreshold = fThreshold;
}

float Warning::getSmogThreshold()
{
  return m_fSmognThreshold;
}

void Warning::setIlluminanceThreshold(float fThreshold)
{
  m_fIlluminanceThreshold = fThreshold;
}

float Warning::getIlluminanceThreshold()
{
  return m_fIlluminanceThreshold;
}

void Warning::setBatteryTempThreshold(float fThreshold)
{
  m_fBatteryTempThreshold = fThreshold;
}

float Warning::getBatteryTempThreshold()
{
  return m_fBatteryTempThreshold;
}

void Warning::setO2Threshold(float fThreshold)
{
  m_fO2Threshold = fThreshold;
}

float Warning::getO2Threshold()
{
  return m_fO2Threshold;
}

void Warning::setCOThreshold(float fThreshold)
{
  m_fCOThreshold = fThreshold;
}

float Warning::getCOThreshold()
{
  return m_fCOThreshold;
}

void Warning::setSO2Threshold(float fThreshold)
{
  m_fSO2Threshold = fThreshold;
}

float Warning::getSO2Threshold()
{
  return m_fSO2Threshold;
}

void Warning::setCH4Threshold(float fThreshold)
{
  m_fCH4Threshold = fThreshold;
}

float Warning::getCH4Threshold()
{
  return m_fCH4Threshold;
}

float Warning::getThermalCameraHighThreshold()
{
  return m_fThermalHighThreshold;
}

void Warning::setThermalCameraHighThreshold(float fThreshold)
{
  m_fThermalHighThreshold = fThreshold;
}

float Warning::getThermalCameraLowThreshold()
{
  return m_fThermalLowThreshold;
}

void Warning::setThermalCameraLowThreshold(float fThreshold)
{
  m_fThermalLowThreshold = fThreshold;
}