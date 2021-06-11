//
// Created by ahuo on 18-7-18.
//

#ifndef FTPCLIENT_FTPCLIENTINTERFACE_H
#define FTPCLIENT_FTPCLIENTINTERFACE_H

class FtpClientInterface {

public:
    FtpClientInterface();
    ~FtpClientInterface();

    bool login(const char *sServerIp, int nPort, const char *sUserName, const char * sPassword);
    int logout();

    bool isConnected();

    bool downloadFile(const char *serverFile, const char *localFile);
    bool upLoadFile(const char *localFile, const char *serverFile);

};

#endif //FTPCLIENT_FTPCLIENTINTERFACE_H
