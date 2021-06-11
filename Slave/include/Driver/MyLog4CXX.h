#ifndef MYLOG4CXX_H
#define MYLOG4CXX_H

#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/configurator.h>
#include <log4cplus/initializer.h>
#include <log4cplus/helpers/loglog.h>
#include <log4cplus/helpers/stringhelper.h>
#include <log4cplus/ndc.h>

#include <atomic>
#include <string>
#include <iostream>
#include <memory>

class MyLog4CXX
{
private:
    /* Singleton */
    MyLog4CXX();
    MyLog4CXX(const MyLog4CXX&);
    MyLog4CXX& operator=(const MyLog4CXX&);
    static std::atomic<MyLog4CXX*> m_instance;
    static std::mutex m_mutex;

    /* data */
    static log4cplus::Initializer m_initializer;
    std::string m_filename;
    static log4cplus::Logger m_rootLog;
public:
    // TODO: Singleton
    // MyLog4CXX() = delete;
    // MyLog4CXX(const MyLog4CXX& other) = delete;
    static MyLog4CXX* getInstance();
    // TODO: Dynamic load config file.
    void init();
    bool init(std::string file);
    bool setConfigFile(std::string file = "../Config/log4cplus.properties");
    
    // TODO: Log hierarchy(parent-log, child-log, child-log-level)
    // TODO: Output settings
    void markThread(std::string threadName, log4cplus::Logger &logger);
    log4cplus::Logger markThread(std::string threadName);
    ~MyLog4CXX();
};

inline log4cplus::Logger initLogger(std::string loggerName) {
    log4cplus::Logger logger = log4cplus::Logger::getInstance(LOG4CPLUS_STRING_TO_TSTRING(loggerName));
	log4cplus::NDCContextCreator _ndc(logger.getName());
	return logger;
}

#endif