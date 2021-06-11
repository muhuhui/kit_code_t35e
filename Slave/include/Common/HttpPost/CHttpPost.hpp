#pragma once
#ifndef C_HTTP_POST_HPP
#define C_HTTP_POST_HPP

#include <iostream>
#include <string>
#include <memory>
#include <utility>
#include <vector>
#include <mutex>

#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/NetException.h>
#include <Poco/StreamCopier.h>
#include <Poco/URI.h>
#include <Poco/JSON/Parser.h>
#include "Poco/Dynamic/Var.h"
#include "Poco/JSON/Query.h"

#include "Poco/Util/Application.h"
#include "Poco/Path.h"

#include "Poco/AutoPtr.h"
#include "Poco/Util/IniFileConfiguration.h"



class CHttpPost
{
private:
    static std::shared_ptr< CHttpPost > ptr_;
    std::mutex m_lck;
    // static std::shared_ptr< URI > pUrl_;
    Poco::URI url_;
    Poco::Net::HTTPClientSession session_;
    Poco::Net::HTTPRequest request_;
    std::string dataToPost_;

    CHttpPost(void) {};
public:

    CHttpPost(CHttpPost const&) = delete;
    void operator=(CHttpPost const&) = delete;
    ~CHttpPost() {};

    struct IdentifyResult {
        std::string item_name;
        std::string item_unit;
        std::string value;
        int isAlarm;
        std::string time;
        std::string errMsg;
        std::string errCode;
        std::string nId;
    };

    static void SetURL( const std::string& _url );
    static bool SetConfigFile( const std::string& _filepathAndName = "./Config/ParametersConfig.ini"
                                , const std::string& _propertyName = "CvServer.url");

    static void Post ( const Poco::JSON::Object& _jMsg );

    // post the vMsg, which is a vector of pair<key , value>
    static void Post ( const std::vector< std::pair< std::string, std::string > > & _vMsg);
    static std::string GetResponse();
    // post the specific info
    static void ImageProcessPost ( const uint32_t& _transId, const uint32_t& _workId, const std::string& _filePath );
    static int GetImageProRes(std::vector<IdentifyResult>& resultList);
    static int GetImageProRes(uint32_t _transId, uint32_t _workId, std::string _filePath, std::vector<IdentifyResult>& resultList);
    static void ViewPointInfPost(int robotId, std::string pointName, int taskId, std::string data_path, std::string type);
    static int SendViewPointInf(int robotId, std::string pointName, int taskId, std::string uploadFilePath, std::string type);
    static int heartBeatPost(int robotId, std::string pointName, std::string status);
    static int sendHeartBeat(int robotId, std::string pointName);

    static int GetImageProRes(Poco::JSON::Object obj, std::vector<IdentifyResult>& resultList);
    static int GetImageProRes(std::vector<IdentifyResult>& resultList, bool is_new);
};




#endif
