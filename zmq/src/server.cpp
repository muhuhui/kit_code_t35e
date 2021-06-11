/*
 * @Author: zhangrui 
 * @Date: 2020-11-20 14:42:21
 * @LastEditTime: 2020-11-21 13:04:14
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \zmq\server.cpp
 */

#include <pthread.h>
#include <string>
#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <algorithm>
#include <string>
#include "jsoncpp/json.h"
#include "zmq/zmqpp/zmqpp.hpp"

#define RESULT_FAILED ">FAILED"
#define RESULT_SUCCESS ">SUCCESS"
#define SHELL_SW_UPGRADE "/usr/local/bin/upgrade.sh"

static int runShell(const char* cmd)
{
    FILE* pp = popen(cmd, "r"); // build pipe
    if (!pp) {
        printf("%s, popen failed\n", __FUNCTION__);
        return -1;
    }
    printf("%s, run cmd: \"%s\"\n", __FUNCTION__, cmd);

    // collect cmd execute result
    char tmp[1024] = {};
    char all[4096]={};
    int rc = 0;
    while (fgets(tmp, sizeof(tmp), pp) != NULL) {
        strcat(all, tmp);
        std::cout << tmp << std::endl; // can join each line as string

        //get result from all
        std::string str_all(all);
        std::size_t found = str_all.rfind(RESULT_FAILED);
        if(found != std::string::npos) {
            printf("Shell returns FAIL\n");
            rc = -1;
            break;
        }
        else {
            found = str_all.rfind(RESULT_SUCCESS);
            if(found != std::string::npos) {
                printf("Shell returns SUCCESS\n");
                rc = 0;
                break;
            }
        }
    }
    pclose(pp);


    printf("Get shell return value %d\n", rc);
    return rc;
}


int main () {
    //  Prepare our context and socket
    zmqpp::context context;

    // generate a pull socket
    zmqpp::socket_type type = zmqpp::socket_type::rep;
    zmqpp::socket socket (context, type);
    const std::string endpoint = "tcp://*:5557";

    // bind to the socket
    std::cout << "Binding to " << endpoint << "..." << std::endl;
    socket.bind(endpoint);
    
    Json::Reader reader;
    Json::Value root;
    Json::Value resp;

    while (true) {
        zmqpp::message message;
        socket.receive(message);
        std::string text;
        message >> text;
        std::cout << "Message: " << text << std::endl;

        int robot_id = 0;
        int robot_type = 0;
        int config_type = -1;
        int radar_type = -1;
        std::string robot_ip = "";
        std::string ptz_ip = "";
        std::string robotType = "";
        std::string radarType = "";
        std::string filePath = "";
        std::string ipAddr = "";

        if(reader.parse(text, root)) {
            config_type = root["configType"].asInt();
            radar_type = root["radarType"].asInt();
            robot_ip = root["robotIp"].asString();
            ptz_ip = root["ptzIp"].asString();
            robot_id = root["robotId"].asInt();
            robot_type = root["robotType"].asInt();
            filePath = root["path"].asString();
            ipAddr = root["ipaddr"].asString();
        }
        else {
            resp["nErrcode"] = -1;
            resp["sFeedback"] = "Json Error";
            std::string strJson = resp.toStyledString();
            strJson.erase(std::remove_if(strJson.begin(), strJson.end(), 
                        [](char c) -> bool { return (c == '\n' || c == '\t'); }), strJson.end());
            socket.send(strJson);
            continue;
        }

        //  Send reply back to client
        resp["nErrcode"] = 0;
        resp["sFeedback"] = "";
        std::string strJson = resp.toStyledString();
        strJson.erase(std::remove_if(strJson.begin(), strJson.end(), 
                    [](char c) -> bool { return (c == '\n' || c == '\t'); }), strJson.end());
        socket.send(strJson);

        char cmd[1024] = {0};
        memset(cmd, 0, sizeof(cmd));
        switch (config_type)
        {
            case 0://更新配置文件
                snprintf(cmd, sizeof(cmd), "%s -c %s", SHELL_SW_UPGRADE, filePath.c_str());
                printf("Upgrade config file, config type: %d", config_type);
            break;
            case 1://更新软件包
                snprintf(cmd, sizeof(cmd), "%s -p %s", SHELL_SW_UPGRADE, filePath.c_str());
                printf("Upgrade SW package,  config type: %d", config_type);
            break;
            case 2://暂不支持
            default://错误
                resp["nErrcode"] = -1;
                resp["sFeedback"] = "Json Error";
                std::string strJson = resp.toStyledString();
                strJson.erase(std::remove_if(strJson.begin(), strJson.end(), 
                            [](char c) -> bool { return (c == '\n' || c == '\t'); }), strJson.end());
                socket.send(strJson);
                printf("Ivalid config type: %d", config_type);
                continue;
        }

        //run shell
        int rc = runShell(cmd);
        if(!rc)
        {
            //return 0;
        }
        
        sleep(1);
    }

    return 0;
}

