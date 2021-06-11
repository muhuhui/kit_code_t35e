#ifndef RadarManager_H
#define RadarManager_H

#include "base/SingleTon.h"
#include "base/Subjects.h"
#include "base/Observers.h"
#include <queue>
#include <map>
#include "Transaction/ConfigServer.h"
#include "Driver/RadarDriver.h"


class RadarManager : public Subjects
{
public:
    ~RadarManager();
    static RadarManager *getInstance();

    void attach(Observers *pObserver);
    void detach(Observers *pObserver);

    void init();
    
    int addRadar(RadarNum num);

    int setWarnThreshold(int nThreshold);
    int setBackWarnThreshold(int nThreshold);
    int setDownWarnThreshold(int nThreshold);

    int getWarnStatus() { return m_eWarnType; }
    bool getWarnStatus(RadarNum num) { return (m_eWarnType >> num) & 0x01; };
    void setWarnStatus(RadarNum num) { m_eWarnType |= (1 << num); }
    void clearWarnStatus(RadarNum num) { m_eWarnType &= (~ (1 << num)); }
    int medianFilter(int sample, std::queue<int>& que, uint8_t winSize=5);
    float getDistance(int num);

    std::queue<int>         m_frontBuf;
    std::queue<int>         m_backBuf;
    std::queue<int>         m_downBuf;
    //屏蔽后雷达
	const LimitRadarback *m_limitRadarback;
    int minLimit;
    int maxLimit;
    
private:
    int setThreshold();

private:
    bool                    m_bThreadRun;
    int                     m_eWarnType;
    std::list<Observers* >  m_ObserverList;
    std::map<RadarNum, int> m_radardisMap;
    int                     m_radarNum;
    RadarDriver*            m_pRadarDriver;

    void notify();

    void judgeWarn();

 
    static void run(RadarManager *pRadarManager);
};

#endif // RadarManager_H
