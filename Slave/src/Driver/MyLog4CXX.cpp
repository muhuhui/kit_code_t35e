#include "Driver/MyLog4CXX.h"
std::atomic<MyLog4CXX*> MyLog4CXX::m_instance;
std::mutex MyLog4CXX::m_mutex;

log4cplus::Initializer MyLog4CXX::m_initializer;
log4cplus::Logger MyLog4CXX::m_rootLog = log4cplus::Logger::getRoot();

MyLog4CXX::MyLog4CXX(){
};

MyLog4CXX* MyLog4CXX::getInstance() {
    auto tmp = m_instance.load(std::memory_order_relaxed);
    std::atomic_thread_fence(std::memory_order_acquire);
    if (tmp == nullptr) {
        std::lock_guard<std::mutex> lock(m_mutex);
        tmp = m_instance.load(std::memory_order_relaxed);
        if (tmp == nullptr) {
            tmp = new MyLog4CXX;
            std::atomic_thread_fence(std::memory_order_release);
            m_instance.store(tmp, std::memory_order_relaxed);
        }
    }
    return m_instance;
}

bool MyLog4CXX::init( std::string file ) {
    bool bSet = this->setConfigFile(file);
    if (bSet) {
        this->init();
        return true;
    } else {
        return false;
    }
}

void MyLog4CXX::init() {
    // Load config dynamically
    m_rootLog.setLogLevel(log4cplus::ALL_LOG_LEVEL);
    try {
        log4cplus::ConfigureAndWatchThread configThread(m_filename, 5*1000);
        LOG4CPLUS_DEBUG(m_rootLog, "Succeed to load Config file. \n");
    } catch(...) {
        LOG4CPLUS_ERROR(m_rootLog, "Fail to load Config file. \n");
    }
}

bool MyLog4CXX::setConfigFile(std::string file) {
    if (!file.empty()) {
        m_filename = file;
        return true;
    } else {
        return false;
    }
}

void MyLog4CXX::markThread(std::string threadName, log4cplus::Logger &logger) {
    logger = markThread(threadName);
}

log4cplus::Logger MyLog4CXX::markThread(std::string threadName){
    std::string threadFullName = "root.thread-" + threadName;
    auto logger = log4cplus::Logger::getInstance(LOG4CPLUS_STRING_TO_TSTRING(threadFullName));
	log4cplus::NDCContextCreator _ndc(LOG4CPLUS_STRING_TO_TSTRING(threadFullName));
    return logger;
}


