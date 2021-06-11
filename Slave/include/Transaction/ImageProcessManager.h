#ifndef ImageProcessManager_H
#define ImageProcessManager_H

#include "base/SingleTon.h"
#include "base/Subjects.h"
#include "base/Observers.h"
#include <queue>
#include <mutex> 
#include "Transaction/ConfigServer.h"
#include "Transaction/MySQLServer.h"


class ImageProcessManager
{
public:
    ~ImageProcessManager();
    static ImageProcessManager *getInstance();

    void init();

    void attach(Observers *pObserver);
    void detach(Observers *pObserver);

    void appendViewPointInfo(ViewPointInfo info);
    int getProcessResult(ViewPointInfo info);

private:
    void notify();
    static void run(ImageProcessManager *pImagePorcessManager);

private:
    std::queue<ViewPointInfo>  m_ViewPointInfo;
    std::list<Observers* >     m_ObserverList;
    std::mutex                 m_lck;

};

#endif // ImageProcessManager