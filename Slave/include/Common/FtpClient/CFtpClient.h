/*================================================================
*   Copyright (C) 2020 Tetraelc Ltd. All rights reserved.
*   
*   File name: CFtpClient.h
*   Author: JiangJin
*   Create Date: 2020-06-28
*   Description: 
*
================================================================*/


#pragma once

#include <string>
#include <mutex>
#include <Poco/File.h>
#include <Poco/Net/FTPClientSession.h>
#include <Poco/StreamCopier.h>
#include <vector>

using Poco::Net::FTPClientSession;

class CFtpClient
{
public:
    ~CFtpClient();
    bool connect(std::string host, std::string username, std::string password, Poco::UInt16 port = 21);
    int upload(std::string localPath, std::vector<std::string> curFileList, std::string uploadPath, std::vector<std::string> uploadFileList);
    int download(std::string workingDir);
    int close();
private:
    std::mutex m_lck;
    FTPClientSession *pClientSession = nullptr;
};

int ftpRemoteUpload(std::string localPath, std::vector<std::string> curFileList, std::string uploadPath);
