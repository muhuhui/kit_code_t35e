#include <thread>
#include "Transaction/Robot.h"
#include <stdio.h> 
#include <execinfo.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#define PID_FILE "/home/ubuntu/.Slave.pid"
bool checkSingleProcess()
{
    char str[256] = "";
    //文件不存在，返回
    if (access(PID_FILE, F_OK)) {
        return true;
    }
    printf("file (%s) exists!\n", PID_FILE);
    int fd = open(PID_FILE, O_RDONLY);
    if (fd >= 0) {
        //读取程序PID
        if (read(fd, str, sizeof(str)) > 0) {
            printf("last pid: %s\n",str);
            int lastPID = atoi(str);
            if (lastPID > 0) {
                //在进程表中查找pid值
                char pidCmd[1024] = "";
                snprintf(pidCmd, sizeof(pidCmd), "ps -ef|awk '{print $2}'|grep %d", lastPID);
                FILE* fp = NULL;
                if ((fp = popen(pidCmd, "r")) == NULL) {
                    printf("call popen failed\n");
                }
                else {
                    //判断进程是否存在
                    memset(str, 0, sizeof(str));
                    char* pstr = fgets(str, sizeof(str), fp);
                    pclose(fp);
                    if (pstr != NULL) {
                        printf("running pid: %s",str);
                        snprintf(pidCmd, sizeof(pidCmd), "kill -9 %d", lastPID);
                        system(pidCmd);
                        return false;
                    }
                }
            }
        }
    }
    else {
        perror("open file failed");
    }

    return true;
}

void deleteFile()
{
    remove(PID_FILE);
}

void catch_signal(int sign)
{
    switch (sign)
    {
        case SIGINT:
            exit(0);
            break;
    }
}


void updateFilePID()
{
    pid_t pid = -1;
    pid = getpid();
    char str[256] = "";
    int fd = open(PID_FILE, O_RDWR | O_CREAT | O_TRUNC, 0777);
    memset(str, 0, sizeof(str));
    snprintf(str, sizeof(str), "%d", (int)pid);
    write(fd, str, strlen(str));
    close(fd);
    printf("current pid: %s\n", str);

    atexit(deleteFile);
    signal(SIGINT, catch_signal);
}

void server_backtrace(int sig)
{
    //打开文件
    time_t tSetTime;
    time(&tSetTime);
    struct tm* ptm = localtime(&tSetTime);
    mkdir("./coredump", S_IRWXU);
    char fname[256] = {0};
    sprintf(fname, "./coredump/core.%d-%d-%d_%d_%d_%d",
            ptm->tm_year+1900, ptm->tm_mon+1, ptm->tm_mday,
            ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
    FILE* f = fopen(fname, "a");
    if (f == NULL){
        return;
    }
    int fd = fileno(f);

    //锁定文件
    struct flock fl;
    fl.l_type = F_WRLCK;
    fl.l_start = 0;
    fl.l_whence = SEEK_SET;
    fl.l_len = 0;
    fl.l_pid = getpid();
    fcntl(fd, F_SETLKW, &fl);

    //输出程序的绝对路径
    char buffer[4096];
    memset(buffer, 0, sizeof(buffer));
    int count = readlink("/proc/self/exe", buffer, sizeof(buffer));
    if(count > 0){
        buffer[count] = '\n';
        buffer[count + 1] = 0;
        fwrite(buffer, 1, count+1, f);
    }

    //输出信息的时间
    memset(buffer, 0, sizeof(buffer));
    sprintf(buffer, "Dump Time: %d-%d-%d %d:%d:%d\n",
            ptm->tm_year+1900, ptm->tm_mon+1, ptm->tm_mday,
            ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
    fwrite(buffer, 1, strlen(buffer), f);

    //线程和信号
    sprintf(buffer, "Curr thread: %u, Catch signal:%d\n",
            (int)pthread_self(), sig);
    fwrite(buffer, 1, strlen(buffer), f);

    //堆栈
    void* DumpArray[256];
    int    nSize =    backtrace(DumpArray, 256);
    sprintf(buffer, "backtrace rank = %d\n", nSize);
    fwrite(buffer, 1, strlen(buffer), f);
    if (nSize > 0){
        char** symbols = backtrace_symbols(DumpArray, nSize);
        if (symbols != NULL){
            for (int i=0; i<nSize; i++){
                fwrite(symbols[i], 1, strlen(symbols[i]), f);
                fwrite("\n", 1, 1, f);
            }
            free(symbols);
        }
    }

    //文件解锁后关闭
    fl.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &fl);
    fclose(f);
}

void signal_crash_handler(int sig)
{
    server_backtrace(sig);
    exit(-1);
}

int main(int argc, char **argv)
{
    MyLog4CXX *pLog = MyLog4CXX::getInstance();
    pLog->init("./Config/log4cplus.properties");
    log4cplus::Logger logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("root.main"));
    log4cplus::NDCContextCreator _ndc(logger.getName());
    LOG4CPLUS_DEBUG(logger, "Into main");

    signal(SIGSEGV, signal_crash_handler);    // SIGSEGV，非法内存访问
    
    if (!checkSingleProcess()) {
        printf("--- Slave error, process is already running, killed!\n");
        LOG4CPLUS_INFO(logger, "process is already running, killed!");
    }
    updateFilePID();

    Robot *pRobot = Robot::getInstance();
    int nRet = pRobot->init();
    if (nRet != 0) {
        return -1;
    }

    MoveTaskManage *moveTaskManage=MoveTaskManage::getInstance();
    std::thread processThread;
    processThread = std::thread(MoveTaskManage::process, moveTaskManage);
    processThread.detach();     

    std::thread regularTaskAgingCheckThread;
    regularTaskAgingCheckThread = std::thread(MoveTaskManage::regularTaskAgingCheck, moveTaskManage);
    regularTaskAgingCheckThread.detach();  

    std::thread selfInspectionThread;
    selfInspectionThread = std::thread(Robot::selfInspection, pRobot);
    selfInspectionThread.detach();

    PositionMove* pMove = PositionMove::getInstance();
    pMove->init(); 

    pRobot->timingPublishRunData();

    return 0;
}
