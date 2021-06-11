#pragma once


#include <pthread.h>
#include "HttpClient.h"
#include<errno.h>  
#include<sys/types.h>  
#include<sys/socket.h>  
#include<netinet/in.h> 

#define ALARM_GUARD  2
#define ALARM_LISTEN 3
#define BUFFERSIZE   1024*4096

void alarm_guard(const char *ip, const char *port, char *username, char *password);
void *stop_guard(void*);
void alarm_listen(const char *ip, const int port);
void *stop_listen(void*);
