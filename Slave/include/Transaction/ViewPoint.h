#pragma once

#include <string>

using namespace std;

struct PresetInfo{
	int x_pos;
	int y_pos;
	int z_pos;
	int zoom;
};

class ViewPoint
{
public:
	ViewPoint();
	~ViewPoint();
	int getPointId(){return pointId;}
	void setPointId(int pointId){this->pointId = pointId;}
	string getName(){return name;}
	void setName(string name){this->name = name;}
	int getAreaId(){return areaId;}
	void setAreaId(int areaId){this->areaId = areaId;}
    int getArmPosition() { return armPosition; }
    void setArmPosition(int position) { this->armPosition = position; }
	int getPosition(){return position;}
	void setPosition(int position){this->position = position;}
	int getPresetNumber(){return presetNumber;}
	void setPresetNumber(int presetNumber){this->presetNumber = presetNumber;}
	int getIndustrialCameraFocus(){return industrialCameraFocus;}
	void setIndustrialCameraFocus(int industrialCameraFocus){this->industrialCameraFocus = industrialCameraFocus;}
	int getThermalImagerFocus(){return thermalImagerFocus;}
	void setThermalImagerFocus(int thermalImagerFocus){this->thermalImagerFocus = thermalImagerFocus;}
	string getPicUrl(){return picUrl;}
	void setPicUrl(string picUrl){this->picUrl = picUrl;}
    std::string getInstanceName() { return sInstanceName; }
    void setInstanceName(std::string instanceName) { this->sInstanceName = instanceName; }
	int getPddDis() { return PddDis; }
	void setPddDis(int dis){this->PddDis = dis;}
	bool getPdd() { return pdd; }
	void setPdd(bool pdd){this->pdd = pdd;}
	bool getVideo() { return vid; }
	void setVideo(bool vid){this->vid = vid;}
	bool getPic() { return picture; }
	void setPic(bool picture){this->picture = picture;}
	PresetInfo getPresetInfo() { return preInfo; }
	void setPresetInfo(PresetInfo preInfo){this->preInfo = preInfo;}
	float getX1() { return x1; }
	void setX1(float x1){this->x1 = x1;}
	float getX2() { return x2; }
	void setX2(float x2){this->x2 = x2;}
	float getY1() { return y1; }
	void setY1(float y1){this->y1 = y1;}
	float getY2() { return y2; }
	void setY2(float y2){this->y2 = y2;}
private:
	int pointId;
	string name;
	int areaId;
    int armPosition;
	int position;
	int presetNumber; //預置點信息
	int industrialCameraFocus;
	int thermalImagerFocus;
    string picUrl;
    std::string sInstanceName;
	int PddDis;
	bool pdd;
	bool video;
	bool picture;
	bool noise;
	bool vid;
	PresetInfo preInfo;
	float x1;
	float y1;
	float x2;
	float y2;
};

