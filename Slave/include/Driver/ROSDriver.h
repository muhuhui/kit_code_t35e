/* #ifndef ROSDRIVER_H
#define ROSDRIVER_H

#include <stdio.h>
#include <string>
#include <map>
#include <ros/ros.h>
#include <ros/network.h>
#include "Common/SingleTon.h"

enum PublisherType {
  PUB_MOTOR_INFO = 0,
  PUB_END
};

enum SubcriberType {
  SUB_MOTOR_INFO = 0,
  SUB_RFID_INFO,
  SUB_END
};

enum ClientType {
  CLIENT_MOTOR_CMD = 0,
  CLIENT_SET_OBJECT = 0,
  CLIENT_FDCONTROL_CMD,
  CLIENT_END
};

enum ServerType {
  SERVER_MOTOR_CMD = 0,
  SERVER_END
};

class ROSDriver
{
public:
    ~ROSDriver();
    static ROSDriver *getInstance();

    int initRos(std::string sMasterUrl, std::string sHostUrl, int nRobotNum);
    bool isRosAlive();
    void heartBeat();
	void spin();
    template <class M>
    int initPublisher(uint32_t nPublisherId, const std::string& topic, uint32_t queue_size, bool latch = false)
    {        
        ros::NodeHandle nodeHandle;
        ros::Publisher  pub = nodeHandle.advertise<M>(topic, queue_size, latch);

        if(!pub) {
            printf("--- %s: failed\n", __FUNCTION__);
            return -1;
        }

        if(m_publisherMap.find(nPublisherId) == m_publisherMap.end()) {
            m_publisherMap[nPublisherId] = pub;
        }

        return 0;
    }

    template <typename N>
    void publishMsg(uint32_t nPublisherId, const N& message) const
    {
        if(m_publisherMap.find(nPublisherId) != m_publisherMap.end()) {
            m_publisherMap.at(nPublisherId).publish(message);
        }
        else {
            printf("--- %s: no such id: %d\n", __FUNCTION__, nPublisherId);
        }
    }

    template<class MReq, class MRes>
    int registerCallBack(const std::string& serviceName, const boost::function<bool(MReq&, MRes&)>& callback)
    {
        ros::NodeHandle nodeHandle;
        ros::ServiceServer service = nodeHandle.advertiseService(serviceName, callback);
        if(!service) {
            printf("--- %s: failed\n", __FUNCTION__);
            return -1;
        }

        m_serviceServerList.push_back(service);
        return 0;
    }

    template<class MReq, class MRes>
    int registerCallBack(const std::string& serviceName, bool(*srv_func)(MReq&, MRes&))
    {
        ros::NodeHandle nodeHandle;
        ros::ServiceServer service = nodeHandle.advertiseService(serviceName, srv_func);
        if(!service) {
            printf("--- %s: failed\n", __FUNCTION__);
            return -1;
        }

        m_serviceServerList.push_back(service);
        return 0;
    }

    template<class T, class MReq, class MRes>
    int registerCallBack(const std::string& serviceName, bool(T::*srv_func)(MReq &, MRes &), T *obj)
    {
        ros::NodeHandle nodeHandle;
        ros::ServiceServer service = nodeHandle.advertiseService(serviceName, srv_func, obj);
        if(!service) {
            printf("--- %s: failed\n", __FUNCTION__);
            return -1;
        }

        m_serviceServerList.push_back(service);
        return 0;
    }

    // Subscriber
    template<class M>
    int registerSubscriberCallBack(uint32_t nSubcriberId, const std::string& topic, uint32_t queue_size, void(*fp)(M))
    {
        ros::NodeHandle nodeHandle;
        ros::Subscriber sub = nodeHandle.subscribe<M>(topic, queue_size, fp);
        if(!sub) {
            printf("--- %s: failed\n", __FUNCTION__);
            return -1;
        }

        if(m_subscriberMap.find(nSubcriberId) == m_subscriberMap.end()) {
            m_subscriberMap.insert(std::make_pair(nSubcriberId, sub));
        }
        return 0;
    }

    // Client
    template <class M>
    int registerClientCallBack(uint32_t nClientId, const std::string& serverName)
    {
        ros::NodeHandle nodeHandle;
        ros::ServiceClient client = nodeHandle.serviceClient<M>(serverName);
        if(!client) {
            printf("--- %s: failed\n", __FUNCTION__);
            return -1;
        }

        if(m_serviceClientMap.find(nClientId) == m_serviceClientMap.end()) {
            m_serviceClientMap.insert(std::make_pair(nClientId, client));
        }
        return 0;
    }

    template<class Service>
    bool callServer(uint32_t nClientId, Service& service)
    {
        if(m_serviceClientMap.find(nClientId) != m_serviceClientMap.end()) {
            return m_serviceClientMap.at(nClientId).call(service);
        }
        else {
            printf("--- %s: no such id: %d\n", __FUNCTION__, nClientId);
            return false;
        }
    }

private:
    std::map<int, ros::Publisher>   m_publisherMap;
    std::list<ros::ServiceServer>   m_serviceServerList;
    std::map<int, ros::Subscriber>      m_subscriberMap;
    std::map<int, ros::ServiceClient>   m_serviceClientMap;
};

#endif // ROSDRIVER_H */
