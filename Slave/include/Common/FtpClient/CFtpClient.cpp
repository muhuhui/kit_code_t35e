/*================================================================
*   Copyright (C) 2020 Tetraelc Ltd. All rights reserved.
*   
*   File name: CFtpClient.cpp
*   Author: JiangJin
*   Create Date: 2020-06-28
*   Description: 
*
================================================================*/


#include "CFtpClient.h"
#include <iostream>
#include <fstream>
#include <Poco/Net/NetException.h>
#include <stdio.h>
#include <string.h>

using std::string;

CFtpClient::~CFtpClient()
{
    if (pClientSession != nullptr) {
        delete pClientSession;
    }
}

bool CFtpClient::connect(string host, string username, string password, Poco::UInt16 port)
{
    bool isLogined;
    m_lck.lock();
    try {
        if (pClientSession == nullptr) {
            pClientSession = new FTPClientSession(host, port, "", "");
        }
        pClientSession->login(username, password);

        isLogined = pClientSession->isLoggedIn();
    }
    catch(Poco::Net::NetException& ex) {
        std::cout << ex.displayText() << std::endl;
    }
    catch(...) {
        std::cout << "CFtpClient::" << __FUNCTION__ << "(), other error" << std::endl;
    }
    m_lck.unlock();

    return isLogined;
}

int CFtpClient::upload(std::string localPath, std::vector<std::string> curFileList, std::string uploadPath, std::vector<std::string> uploadFileList)
{
	if (pClientSession == nullptr) {
		printf("CFtpClient::%s(), client session point is null!\n", __FUNCTION__);
		return -1;
	}

    m_lck.lock();
    try {
        std::cout << "CFTPClient::upload(), localPath: " << localPath.c_str() << ", uploadPath: " << uploadPath.c_str() << std::endl;
        const char s[2] = "/";
        std::string workingDir;
        char *token = strtok(const_cast<char*>(uploadPath.c_str()), s);
        while(token != NULL) {
            printf("CFTPClient::upload(), token: %s, workingDir: %s\n", token, workingDir.c_str());
            workingDir = workingDir + "/" + token;
            try {
                pClientSession->createDirectory(workingDir);
            }
            catch(Poco::Net::NetException& ex) {
                std::cout << ex.displayText() << std::endl;
            }
            catch(...) {

            }
            printf("CFTPClient::upload(), token: %s, workingDir: %s\n", token, workingDir.c_str());
            pClientSession->setWorkingDirectory(workingDir.c_str());
            token = strtok(NULL, s);
        }

        auto iter = curFileList.begin();
        auto upIter = uploadFileList.begin();
        for (; iter != curFileList.end() && upIter != uploadFileList.end(); iter++, upIter++) {
            std::string localFile = localPath + "/" + *iter;
            std::string uploadFile = workingDir + "/" + *upIter;
            std::cout << "CFTPClient::upload(), upload " << localFile.c_str() << " to " << uploadFile.c_str() << std::endl;
            std::ifstream ifs(localFile.c_str(), std::ios::binary);
            if (ifs.fail()) {
                m_lck.unlock();
                return -1;
            }
            std::ostream& os = pClientSession->beginUpload(upIter->c_str());
            // Poco::StreamCopier::copyStream(ifs, os);
            os << ifs.rdbuf();
            pClientSession->endUpload();
            ifs.close();
        }
    }
    catch(Poco::Net::NetException& ex) {
        std::cout << ex.displayText() << std::endl;
    }
    catch(...) {
        std::cout << "CFtpClient::" << __FUNCTION__ << "(), other error" << std::endl;
    }
    m_lck.unlock();

    return 0;
}

int CFtpClient::download(string workingDir)
{
	if (pClientSession == nullptr) {
		printf("CFtpClient::%s(), client session point is null!\n", __FUNCTION__);
		return -1;
	}

    m_lck.lock();
    try {
        pClientSession->setWorkingDirectory(workingDir);

        std::string str;
        std::vector<std::string> fileList;
        std::vector<std::string> fileDirList;
        std::istream &ftpFileIn = pClientSession->beginList();	//下载目录中文件列表
        while (ftpFileIn >> str) {		//存储文件列表
            fileList.push_back(str);
        }
        pClientSession->endList();	    //关闭目录下载连接

        std::istream &ftpPathIn = pClientSession->beginList(workingDir);
        while (ftpPathIn >> str) {
            fileDirList.push_back(str);
        }
        pClientSession->endList();

        for (size_t i = 0; i != fileDirList.size(); i++) {
            std::cout << "download file " << fileList[i].c_str() << " from " << workingDir.c_str() << std::endl;
            std::ofstream file;
            file.open(fileList[i].c_str());
            std::istream& istr = pClientSession->beginDownload(fileDirList[i].c_str());
            Poco::StreamCopier::copyStream(istr, file);
            pClientSession->endDownload();		//关闭下载连接
            file.close();
        }
    }
    catch(Poco::Net::NetException& ex) {
        std::cout << ex.displayText() << std::endl;
    }
    catch(...) {
        std::cout << "CFtpClient::" << __FUNCTION__ << "(), other error" << std::endl;
    }
    m_lck.unlock();

    return 0;
}

int CFtpClient::close()
{
    if (pClientSession == nullptr) {
        return -1;
    }
    m_lck.lock();
	pClientSession->close();
    m_lck.unlock();
    return 0;
}


static int localTest()
{
    CFtpClient ftpClient;  

    if (!ftpClient.connect("192.168.10.88", "jjin", "jj")) {
        printf("robot init ftp connect failed!\n");
        return -1;
    }
    if (ftpClient.download("/home/jjin/workspace/deploy") != 0) {
        printf("robot init ftp download failed!\n");
        return -1;
    }
    std::vector<std::string> fileList = {"deploy.sh", "setup.sh"};
    if (ftpClient.upload("/home/jjin/workspace/", fileList, "/", fileList) != 0) {
        printf("robot init ftp upload failed!\n");
        ftpClient.close();
        return -1;
    }
    ftpClient.close();
    return 0;
}

#include <time.h>
int ftpRemoteUpload(std::string localPath, std::vector<std::string> curFileList, std::string uploadPath)
{
    CFtpClient ftpClient;  

    if (!ftpClient.connect("101.132.161.240", "ftp_tc", "H48SNvqRp6aNRpXC")) {
        printf("robot init ftp connect failed!\n");
        return -1;
    }
    if (ftpClient.upload(localPath, curFileList, uploadPath, curFileList) != 0) {
        printf("robot init ftp upload failed!\n");
        ftpClient.close();
        return -1;
    }
    ftpClient.close();
    return 0;
}
