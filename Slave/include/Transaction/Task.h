#ifndef TASK_H
#define TASK_H

#include <list>
#include <time.h>
#include "Transaction/ViewPoint.h"
#include<string>
using namespace std;


enum TaskStatus {
    TASK_ACTIVE = 0,
    TASK_INACTIVE,
    TASK_INVALID
};

enum TaskOverType {
    OVER_STAND_STILL = 1,
    OVER_CHARGE = 2,
    OVER_TO_BEGINNING = 3,
};

enum TaskType {
    TASK_PERIOD = 0,
    TASK_ONETIME,
    TASK_NOW,
};

enum TaskPeriodUnit {
    PERIOD_DAY = 0,
    PERIOD_HOUR,
};

class Task
{
public:
    Task();
    ~Task();
    int getTaskId() const { return taskId; }
    void setTaskId(int val) { taskId = val; }
    std::string getTaskName(){return taskName;}
    void setTaskName(std::string taskName){this->taskName = taskName;}
    int  getTempId(){return tempId;}     
    void setTempId(int tempId) {this->tempId = tempId;}   
    int getRobotId() const { return robotId; }
    void setRobotId(int val) { robotId = val; }
    int getTaskType(){return taskType;}
    void setTaskType(int taskType     ){this->taskType = taskType;}
    time_t getStartTime(){return startTime;}
    void setStartTime(time_t startTime){this->startTime = startTime;}
    time_t getEndTime(){return endTime;}
    void setEndTime(time_t endTime){this->endTime = endTime;}
    float getSpeed(){return speed;}
    void setSpeed(float speed){this->speed = speed;} 
    int getRepeatUnit(){return repeatUnit;}
    void setRepeatUnit(int repeatUnit) {this->repeatUnit = repeatUnit;}
    int  getRepeatPeriod(){return repeatPeriod;}
    void setRepeatPeriod(int repeatPeriod){this->repeatPeriod = repeatPeriod;}    
    std::list<ViewPoint> getViewPoints(){return viewPoints;}
    void setViewPoints(std::list<ViewPoint> viewPoints) { this->viewPoints = viewPoints; }
    void setViewType(int viewType) { this->viewType = viewType; }
    int getViewType() { return viewType; }
    void setOverType(int overType) { this->overType = overType; }
    int getOverType() { return overType; }
    

private:
    int     taskId; 
    std::string  taskName;
    int     tempId;
    int     robotId;
    int     taskType;
    time_t  startTime;
    time_t  endTime; 
    float   speed;
    int     repeatUnit;
    int     repeatPeriod;
    int     viewType;
    int     overType;
    std::list<ViewPoint>   viewPoints;
};

#endif // TASK_H
