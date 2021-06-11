#ifndef WARNING_H
#define WARNING_H

#include <string>

struct AlarmInfo
{
  int       nAlarmID;
  int       nItemID;
  int       nRobotID;
  double     dPosition;

  std::string   strItemName;
  std::string   strLevelName;
  std::string   strReason;
  std::string   strAlarmTime;
  std::string   strValue;

  time_t        tAlarmTime;
};

class Warning
{
public:
  Warning();
  ~Warning();

  void setRobotID(int nRobotID);
  int getRobotID();

  void setSmogThreshold(float fThreshold);
  float getSmogThreshold();

  void setIlluminanceThreshold(float fThreshold);
  float getIlluminanceThreshold();

  void setBatteryTempThreshold(float fThreshold);
  float getBatteryTempThreshold();

  void setO2Threshold(float fThreshold);
  float getO2Threshold();

  void setCOThreshold(float fThreshold);
  float getCOThreshold();

  void setSO2Threshold(float fThreshold);
  float getSO2Threshold();

  void setCH4Threshold(float fThreshold);
  float getCH4Threshold();

  void setThermalCameraHighThreshold(float fThreshold);
  float getThermalCameraHighThreshold();

  void setThermalCameraLowThreshold(float fThreshold);
  float getThermalCameraLowThreshold();

private:
  int                   m_nRobotID;

  float                 m_fSmognThreshold;
  float                 m_fIlluminanceThreshold;
  float                 m_fBatteryTempThreshold;
  float                 m_fO2Threshold;
  float                 m_fCOThreshold;
  float                 m_fSO2Threshold;
  float                 m_fCH4Threshold;
  float                 m_fThermalHighThreshold;
  float                 m_fThermalLowThreshold;
};

#endif // WARNING_H
