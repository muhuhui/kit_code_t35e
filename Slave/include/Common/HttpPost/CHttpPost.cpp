#include "CHttpPost.hpp"


using Poco::AutoPtr;
using Poco::Util::IniFileConfiguration;
using namespace std;
using namespace Poco::Util;
using namespace Poco;
using namespace Poco::Net;
using namespace Poco::JSON;
// std::shared_ptr< URI > CHttpPost::pUrl_ = nullptr;
std::shared_ptr< CHttpPost > CHttpPost::ptr_ = nullptr;

static std::string getTime()
{
  setenv("TZ","GMT-8",1);

  time_t timep;
  time(&timep);

  char tmp[128];
  strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S", localtime(&timep));

  return tmp;
}

void removeEscape(string& name) 
{
    std::string::size_type startpos = 0;
    while(startpos != std::string::npos) 
    {
        startpos = name.find("\\");
        if(startpos != std::string::npos) 
        {
            name.replace(startpos,1,"");
        }
    }
}

void CHttpPost::SetURL( const string& _url )
{
    ptr_->session_.reset();
    ptr_->url_ =  URI(_url);
    ptr_->session_.setHost( ptr_->url_.getHost() );
    ptr_->session_.setPort( ptr_->url_.getPort() );
    ptr_->request_.setMethod( HTTPRequest::HTTP_POST );
    ptr_->request_.setURI( ptr_->url_.getPath() );
    ptr_->request_.setVersion( HTTPRequest::HTTP_1_1 );
    ptr_->session_.setTimeout(Timespan(0,0,5,0,0));
}

bool CHttpPost::SetConfigFile( const std::string& _filepathAndName
                             , const std::string& _propertyName )
{
    AutoPtr<IniFileConfiguration> pConf(new IniFileConfiguration(_filepathAndName));
    if ( pConf->has( _propertyName ) )
    {
        SetURL( pConf->getRawString( _propertyName ) );
        return true;
    } else {
        cout << "The config file @ " << _filepathAndName 
        << "don't contain the HostPath key and value!" << endl;
        return false;
    }    
}

void CHttpPost::Post ( const Poco::JSON::Object& _jMsg )
{
    ostringstream oss;
    _jMsg.stringify( oss );
    string data( oss.str() );

    removeEscape(data);
    std::cout << "消除转义后的字符串：" << data << std::endl;

    ptr_->request_.setChunkedTransferEncoding( false );
    ptr_->request_.setContentType( "application/json" );
    ptr_->request_.setContentLength( data.length() );
    try
    {
        ptr_->session_.sendRequest( ptr_->request_ ) << data;
    }
    catch(NetException & ex)
    {
        std::cout << ex.displayText(); 
    }
    
}

void CHttpPost::Post ( const vector< pair< string, string > > & _vMsg)
{
    Poco::JSON::Object jObj;
    for (auto propPair : _vMsg) {
        jObj.set( propPair.first, propPair.second );
    }
    Post(jObj);
}

void CHttpPost::ImageProcessPost ( const uint32_t& _transId, const uint32_t& _workId, const std::string& _filePath )
{
    vector< pair< string, string > > vMsg;
    vMsg.push_back( make_pair("transid", to_string(_transId) ) );
    vMsg.push_back( make_pair("workId", to_string(_workId) ) );
    vMsg.push_back( make_pair("filePath", _filePath ) );
    Post( vMsg );
}

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <time.h>
void CHttpPost::ViewPointInfPost(int robotId, std::string pointName, int taskId, std::string data_path, std::string type)
{
    vector< pair< string, string > > vMsg;
    vMsg.push_back(make_pair("config_sn", "42d4f13e-583f-4bef-bc3a-074c894607a7"));
    char sRobotId[100] = "";
    snprintf(sRobotId, sizeof(sRobotId), "tetra-gaotang-robot%02d", robotId);
    vMsg.push_back(make_pair("robot_id", sRobotId));
    vMsg.push_back(make_pair("location_id", pointName));
    char group_id[100] = "";
    snprintf(group_id, sizeof(group_id), "%d", taskId);
    vMsg.push_back(make_pair("group_id", group_id));
    vMsg.push_back(make_pair("data_path", data_path));
    vMsg.push_back(make_pair("result_data", ""));
    vMsg.push_back(make_pair("type", type));
    time_t tim;
    time (&tim);
    struct tm* nowTime = localtime(&tim);
    char strTime[50];
    strftime(strTime, sizeof(strTime), "%Y-%m-%d %H:%M:%S", nowTime);

    vMsg.push_back(make_pair("send_time", strTime));
    Post( vMsg );
}

std::string CHttpPost::GetResponse()
{
    HTTPResponse res;
    std::istream & is = ptr_->session_.receiveResponse(res);
    string ostr;
    StreamCopier::copyToString(is, ostr);
    return ostr;
}

int CHttpPost::GetImageProRes(std::vector<IdentifyResult>& resultList)
{
    std::string jsonResult = GetResponse();
    if (jsonResult.empty()) { 
        printf("CHttpPost::%s(), get response failed!", __FUNCTION__);
        return -1; 
    }
    printf("CHttpPost::%s(), get result: %s\n", __FUNCTION__, jsonResult.c_str());

    Parser parser;
    Dynamic::Var jsonVar = parser.parse(jsonResult);
    Object::Ptr pObj = jsonVar.extract<Object::Ptr>();
    Dynamic::Var errMsgVar = pObj->get("errmsg");
    if (errMsgVar.toString() != "success") {
        printf("CHttpPost::%s(), image identification failed!", __FUNCTION__);
        return -1;
    }

    resultList.clear();
    Array::Ptr pArray = pObj->getArray("resultList");
    Array::ConstIterator it = pArray->begin();
    for (; it != pArray->end(); it++) {
        cout << "array: " << it->toString() << endl;
        pObj = it->extract<Object::Ptr>();
        Dynamic::Var item_name = pObj->get("item_name");
        Dynamic::Var item_unit = pObj->get("item_unit");
        Dynamic::Var value = pObj->get("value");
        IdentifyResult result;
        result.item_name = item_name.toString();
        result.item_unit = item_unit.toString();
        result.value = value.toString();
        resultList.push_back(result);
    }
    for(auto result: resultList)
    {
        cout << "CHttpPost::" << __FUNCTION__ << ": " << result.item_name << " " << result.item_unit << " " << result.value << endl;
    }

    return 0;
}

int CHttpPost::heartBeatPost(int robotId, std::string pointName, std::string status)
{
    vector< pair< string, string > > vMsg;
    vMsg.push_back(make_pair("config_sn", "42d4f13e-583f-4bef-bc3a-074c894607a7"));
    char sRobotId[100] = "";
    snprintf(sRobotId, sizeof(sRobotId), "tetra-gaotang-robot%02d", robotId);
    vMsg.push_back(make_pair("robot_id", sRobotId));
    vMsg.push_back(make_pair("location_id", pointName));
    vMsg.push_back(make_pair("status", status));
    vMsg.push_back(make_pair("remain_battery", "0"));
    vMsg.push_back(make_pair("rtsp_url", ""));
    time_t tim;
    time (&tim);
    struct tm* nowTime = localtime(&tim);
    char strTime[50];
    strftime(strTime, sizeof(strTime), "%Y-%m-%d %H:%M:%S", nowTime);
    vMsg.push_back(make_pair("send_time", strTime));
    Post( vMsg );
}


int CHttpPost::GetImageProRes(uint32_t _transId, uint32_t _workId, std::string _filePath, std::vector<IdentifyResult>& resultList)
{
    try {
        if (ptr_ == nullptr) {
            ptr_ = std::shared_ptr< CHttpPost >( new CHttpPost );
        }
        std::lock_guard<std::mutex> lck(ptr_->m_lck);

        CHttpPost::SetConfigFile();
        CHttpPost::ImageProcessPost(_transId, _workId, _filePath.c_str());
        CHttpPost::GetImageProRes(resultList);
    } catch(Poco::Net::NetException & ex) {
        std::cout << "CHttpPost::" << __FUNCTION__ << "(), " << ex.displayText() << std::endl;
        return -1;
    } catch(...) {
        std::cout << "CHttpPost::GetImageProRes(), other error" << std::endl;
        return -1;
    }
    return 0;
}

int CHttpPost::SendViewPointInf(int robotId, std::string pointName, int taskId, std::string uploadFilePath, std::string type)
{
    try {
        if (ptr_ == nullptr) {
            ptr_ = std::shared_ptr< CHttpPost >( new CHttpPost );
        }
        std::lock_guard<std::mutex> lck(ptr_->m_lck);

        CHttpPost::SetURL("http://aiapi_uat.newhopelh.cn/uploadResultData");
        CHttpPost::ViewPointInfPost(robotId, pointName, taskId, uploadFilePath, type);
        std::string jsonResult = CHttpPost::GetResponse();
        printf("jsonResult: %s\n", jsonResult.c_str());
    }
    catch(Poco::Net::NetException &ex) {
        std::cout << ex.displayText() << std::endl;
        return -1;
    }
    catch(...) {
        std::cout << "CHttpPost::SendViewPointInf(), camera http error" << std::endl;
        return -1;
    }
    return 0;
}

int CHttpPost::sendHeartBeat(int robotId, std::string pointName)
{
    try {
        if (ptr_ == nullptr) {
            ptr_ = std::shared_ptr< CHttpPost >( new CHttpPost );
        }
        std::lock_guard<std::mutex> lck(ptr_->m_lck);

        CHttpPost::SetURL("http://aiapi_uat.newhopelh.cn/heartReport");
        CHttpPost::heartBeatPost(robotId, pointName, "running");
        std::string jsonResult = CHttpPost::GetResponse();
        printf("jsonResult: %s\n", jsonResult.c_str());
    }
    catch(Poco::Net::NetException &ex) {
        std::cout << ex.displayText() << std::endl;
        return -1;
    }
    catch(...) {
        std::cout << "CHttpPost::sendHeartBeat(), camera http error" << std::endl;
        return -1;
    }
    return 0;
}

int CHttpPost::GetImageProRes(Poco::JSON::Object obj, std::vector<IdentifyResult>& resultList)
{
    try {
        if (ptr_ == nullptr) {
            ptr_ = std::shared_ptr< CHttpPost >( new CHttpPost );
        }
        std::lock_guard<std::mutex> lck(ptr_->m_lck);

        CHttpPost::SetConfigFile();
        CHttpPost::Post(obj);
        CHttpPost::GetImageProRes(resultList, true);
    } catch(Poco::Net::NetException & ex) {
        std::cout << "CHttpPost::" << __FUNCTION__ << "(), " << ex.displayText() << std::endl;
        return -1;
    } catch(...) {
        std::cout << "CHttpPost::GetImageProRes(), other error" << std::endl;
        return -1;
    }
    return 0;
}

int CHttpPost::GetImageProRes(std::vector<IdentifyResult>& resultList, bool is_new)
{
    time_t tm;
    time(&tm);
    std::string jsonResult = GetResponse();
    if (jsonResult.empty()) { 
        printf("CHttpPost::%s(), get response failed!", __FUNCTION__);
        return -1;
    }
    printf("CHttpPost::%s(), get result: %s\n", __FUNCTION__, jsonResult.c_str());

    Parser parser;
    Dynamic::Var jsonVar = parser.parse(jsonResult);
    Object::Ptr pObj = jsonVar.extract<Object::Ptr>();
    try{
        Dynamic::Var errMsgVar = pObj->get("workStatus");
        if (errMsgVar.toString() != "SUCCESS") {
            printf("CHttpPost::%s(), image identification failed!", __FUNCTION__);
            return -1;
        }

        resultList.clear();
        Array::Ptr pArray = pObj->getArray("resultList");
        Array::ConstIterator it = pArray->begin();
        for (; it != pArray->end(); it++) {
            cout << "array: " << it->toString() << endl;
            pObj = it->extract<Object::Ptr>();
            Dynamic::Var item_name = pObj->get("udName");
            Dynamic::Var item_unit = pObj->get("itemUnit");
            Dynamic::Var value = pObj->get("value");
            Dynamic::Var errMsg = pObj->get("mErrMsg");
            Dynamic::Var errCode = pObj->get("mErrCode");
            //Dynamic::Var nId = pObj->get("nId");
            IdentifyResult result;
            result.item_name = item_name.toString();
            result.item_unit = item_unit.toString();
            result.value = value.toString();
            result.time = getTime();
            result.errMsg = errMsg.toString();
            result.errCode = errCode.toString();
            //result.nId = nId.toString();
            resultList.push_back(result);
        }
    }
    catch(Poco::Exception &exc) {
        Dynamic::Var errMsgVar = pObj->get("errmsg");
        if (errMsgVar.toString() != "success") {
            printf("CHttpPost::%s(), image identification failed!", __FUNCTION__);
            return -1;
        }

        resultList.clear();
        Array::Ptr pArray = pObj->getArray("resultList");
        Array::ConstIterator it = pArray->begin();
        for (; it != pArray->end(); it++) {
            cout << "array: " << it->toString() << endl;
            pObj = it->extract<Object::Ptr>();
            Dynamic::Var item_name = pObj->get("item_name");
            Dynamic::Var item_unit = pObj->get("item_unit");
            Dynamic::Var value = pObj->get("value");
            IdentifyResult result;
            result.item_name = item_name.toString();
            result.item_unit = item_unit.toString();
            result.value = value.toString();
            result.time = getTime();
            result.errMsg = "";
            result.errCode = "";
            resultList.push_back(result);
        }
    }
    for(auto result: resultList)
    {
        cout << "CHttpPost::" << __FUNCTION__ << ": " << result.item_name << " " << result.item_unit << " " << result.errMsg << " " << result.errCode << endl;
    }

    return 0;
}