#include "Driver/ZMQDriver.h"
#include "Driver/MyLog4CXX.h"

const std::string LOG_TAG("root.ZMQDriver::");

ZMQDriver::ZMQDriver():
    m_srvEndPoint("tcp://*:5555"),
    m_topicEndPoint("tcp://*:5556")
{
    m_srvSocket = new zmqpp::socket(m_context, zmqpp::socket_type::rep);
    m_topicSocket = new zmqpp::socket(m_context, zmqpp::socket_type::pub);
}

ZMQDriver::~ZMQDriver()
{
    delete m_srvSocket;
    delete m_topicSocket;
}

ZMQDriver *ZMQDriver::getInstance()
{
    return SingleTon<ZMQDriver>::GetInstance();
}

int ZMQDriver::initZMQ()
{
    auto logger = initLogger(LOG_TAG + std::string(__FUNCTION__));

    // bind to the socket
    m_srvSocket->bind(m_srvEndPoint.c_str());
    printf("--- ZMQDriver::initZMQ, binding to %s\n", m_srvEndPoint.c_str());
    LOG4CPLUS_INFO(logger, "--- ZMQDriver::initZMQ, binding to " << m_srvEndPoint.c_str());

    m_topicSocket->bind(m_topicEndPoint.c_str());
    printf("--- ZMQDriver::initZMQ, binding to %s\n", m_topicEndPoint.c_str());
    LOG4CPLUS_INFO(logger, "--- ZMQDriver::initZMQ, binding to " << m_topicEndPoint.c_str());

    pthread_t thread;
    pthread_create(&thread, NULL, serverThread, this);

    return 0;
}

void ZMQDriver::registerCallBack(Service* callback)
{
    srvCallBack = callback;
}

bool ZMQDriver::publishMsg(std::string msg)
{
    zmqpp::message message;
    message << msg;
    return m_topicSocket->send(message);
}

void *ZMQDriver::serverThread(void* arg)
{
    auto logger = initLogger(LOG_TAG + std::string(__FUNCTION__));
    printf("--- ZMQDriver::%s start\n", __FUNCTION__);
    LOG4CPLUS_INFO(logger, "--- ZMQDriver::serverThread start");

    ZMQDriver* pZmq = (ZMQDriver*)arg;
    
    while (1) 
    {
        zmqpp::message message;
        pZmq->m_srvSocket->receive(message);
        std::string cmd, result;
        message >> cmd;

        pZmq->srvCallBack(cmd, result);

        pZmq->m_srvSocket->send(result);
    }

    return 0;
}

