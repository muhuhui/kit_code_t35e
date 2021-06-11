#ifndef ZMQDRIVER_H
#define ZMQDRIVER_H

#include <stdio.h>
#include <string>
#include <iostream>
#include <map>
#include <zmqpp/zmqpp.hpp>
#include <pthread.h>
#include "Common/SingleTon.h"

class ZMQDriver
{
    typedef void Service(std::string cmd, std::string& result);
public:
    ZMQDriver();
    ~ZMQDriver();
    static ZMQDriver *getInstance();

    int initZMQ();
    bool publishMsg(std::string msg);
    void registerCallBack(Service* callback);

private:
    Service* srvCallBack;
    zmqpp::context m_context;
    zmqpp::socket* m_srvSocket;
    zmqpp::socket* m_topicSocket;
    const std::string m_srvEndPoint;
    const std::string m_topicEndPoint;

    static void *serverThread(void* arg);
};

#endif
