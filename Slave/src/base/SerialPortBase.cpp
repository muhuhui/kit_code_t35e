#include "base/SerialPortBase.h"

#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <sys/time.h>
#include <stdio.h>


/****************************************/
timestamp_t Timestamp::now()
{
    struct timeval tv;
    gettimeofday(&tv,0);
    timestamp_t time=(timestamp_t)tv.tv_sec*1000+tv.tv_usec/1000;
    return time;
}

/****************************************/
Stream::Stream(int initCapacity)
{
    buffer=new char[initCapacity];
    capacity=initCapacity;
    start=0;
    length=0;
}

Stream::~Stream()
{
    delete[] buffer;
}

int Stream::getLength()
{
    return length;
}

void Stream::append(char aChar)
{
    if(length>=capacity)
        expand();
    int pos=start+length;
    if(pos>=capacity)
        pos-=capacity;
    buffer[pos]=aChar;
    length++;
}

void Stream::append(const char* buf,int len)
{
    for(int i=0;i<len;i++)
        append(buf[i]);
}

char Stream::peek()
{
    if(length==0)
        return 0;
    return buffer[start];
}

int Stream::peek(char* buf,int len)
{
    if(len>length)
        len=length;
    for(int i=0;i<len;i++)
    {
        int pos=start+i;
        if(pos>=capacity)
            pos-=capacity;
        buf[i]=buffer[pos];
    }
    return len;
}

char Stream::take()
{
    if(length==0)
        return 0;
    char aChar=buffer[start];
    start++;
    length--;
    if(start>=capacity)
        start-=capacity;
    return aChar;
}

int Stream::take(char* buf,int len)
{
    if(len>length)
        len=length;
    for(int i=0;i<len;i++)
        buf[i]=take();
    return len;
}

void Stream::expand()
{
    int newCapacity=capacity*2;
    char* newBuf=new char[newCapacity];
    int newLength=length;
    take(newBuf,newLength);
    delete[] buffer;
    buffer=newBuf;
    capacity=newCapacity;
    start=0;
    length=newLength;
}


/****************************************/
SerialPortBase::SerialPortBase():stream()
{
    printf("--- SerialPortBase() \n");
    pthread_mutex_init(&mutex,0);
}

SerialPortBase::~SerialPortBase()
{
    close();
}

void* receiveThread(void* arg)
{
    SerialPortBase* pSerialPortBase=(SerialPortBase*)arg;
    char buf[1024];
    pSerialPortBase->readEnable = false;
    while(true)
    {
        pthread_testcancel();
        if (pSerialPortBase->readEnable != true) {
            usleep(5000);
            continue;
        }
        int len=read(pSerialPortBase->fd,buf,sizeof(buf));
        if(len>0)
        {
            pthread_mutex_lock(&(pSerialPortBase->mutex));
            pSerialPortBase->stream.append(buf,len);
            pthread_mutex_unlock(&(pSerialPortBase->mutex));
        }
        usleep(1000);
    }
}

int SerialPortBase::open(const char* dev,int baud,int dataBits,int parityMode,int stopBits)
{
#if 1

    fd=::open(dev,O_RDWR|O_NOCTTY|O_NDELAY);
    if(fd<0)
        return DEV_NOT_FOUND;

    struct termios options;
    //bzero(&options,sizeof(options));
    tcgetattr(fd, &options);

    options.c_iflag = 0;
    options.c_oflag = 0;
    options.c_cflag = 3261;
    options.c_lflag = 0;
    options.c_line = 0;
    options.c_ispeed = 13;
    options.c_ospeed = 13;

    int baudT=transformBaud(baud);
    if(baudT<0)
        return BAUD_NOT_SUPPORTED;
    cfsetispeed(&options,baudT);
    cfsetospeed(&options,baudT);
    int dataBitsT=transformDataBits(dataBits);
    if(dataBitsT<0)
        return DATABITS_NOT_SUPPORTED;
    options.c_cflag|=dataBitsT;
    if(parityMode==PARITY_ODD)
    {
        options.c_cflag|=PARENB;
        options.c_cflag|=PARODD;
    }
    else if(parityMode==PARITY_EVEN)
        options.c_cflag|=PARENB;
    else if(parityMode!=PARITY_NONE)
        return PARITYMODE_NOT_SUPPORTED;
    if(stopBits==2)
        options.c_cflag|=CSTOPB;
    else if(stopBits!=1)
        return STOPBITS_NOT_SUPPORTED;
    options.c_cc[VTIME]=1;
    options.c_cc[VMIN]=1;

    if(tcsetattr(fd,TCSANOW,&options))
        return CONFIG_FAIL;
    if(tcflush(fd,TCIOFLUSH))
        return CONFIG_FAIL;
    if(pthread_create(&tid,0,receiveThread,this)!=0)
        return NEW_THREAD_FAIL;
    return OK;

#else

    struct termios options;
    bzero(&options,sizeof(options));
    int baudT=transformBaud(baud);
    if(baudT<0)
        return BAUD_NOT_SUPPORTED;
    cfsetispeed(&options,baudT);
    cfsetospeed(&options,baudT);
    int dataBitsT=transformDataBits(dataBits);
    if(dataBitsT<0)
        return DATABITS_NOT_SUPPORTED;
    options.c_cflag|=dataBitsT;
    if(parityMode==PARITY_ODD)
    {
        options.c_cflag|=PARENB;
        options.c_cflag|=PARODD;
    }
    else if(parityMode==PARITY_EVEN)
        options.c_cflag|=PARENB;
    else if(parityMode!=PARITY_NONE)
        return PARITYMODE_NOT_SUPPORTED;
    if(stopBits==2)
        options.c_cflag|=CSTOPB;
    else if(stopBits!=1)
        return STOPBITS_NOT_SUPPORTED;
    options.c_cc[VTIME]=1;
    options.c_cc[VMIN]=1;
    fd=::open(dev,O_RDWR|O_NOCTTY|O_NDELAY);
    if(fd<0)
        return DEV_NOT_FOUND;
    if(tcsetattr(fd,TCSANOW,&options))
        return CONFIG_FAIL;
    if(tcflush(fd,TCIOFLUSH))
        return CONFIG_FAIL;
    if(pthread_create(&tid,0,receiveThread,this)!=0)
        return NEW_THREAD_FAIL;
    return OK;

#endif
}

int SerialPortBase::close()
{
    if(fd>=0)
    {
        ::close(fd);
        pthread_cancel(tid);
        pthread_mutex_destroy(&mutex);
        fd=-1;
    }
}

int SerialPortBase::write(const char* data,int len)
{
    return ::write(fd,data,len);
}

int SerialPortBase::available()
{
    int len=stream.getLength();
    return len;
}

int SerialPortBase::peek(char* buf,int len)
{
    len=stream.peek(buf,len);
    return len;
}

int SerialPortBase::read(char* buf,int len,int timeout)
{
    timestamp_t start=Timestamp::now();
    int total=0;
    while(total<len)
    {
        pthread_mutex_lock(&mutex);
        int readLen=stream.take(buf+total,len-total);
        pthread_mutex_unlock(&mutex);
        if(readLen>0)
            total+=readLen;
        timestamp_t now=Timestamp::now();
        if(now>=start+timeout)
            break;
        usleep(1000);
    }
    return total;
}

int SerialPortBase::read(char* buf,int maxLen,const char* end,int timeout,int* recvLen)
{
    int endLen=strlen(end);
    timestamp_t start=Timestamp::now();
    int total=0;
    while(total<maxLen)
    {
        pthread_mutex_lock(&mutex);
        int readLen=stream.take(buf+total,1);
        pthread_mutex_unlock(&mutex);
        if(readLen>0)
        {
            total+=readLen;
            if(endsWith(buf,total,end,endLen))
            {
                if(recvLen!=0)
                    *recvLen=total;
                return READ_END;
            }
        }
        timestamp_t now=Timestamp::now();
        if(now>=start+timeout)
            return READ_TIMEOUT;
        usleep(1000);
    }
    return READ_BUFFER_FULL;
}

int SerialPortBase::transformBaud(int baud)
{
    int map[][2]={{2400,B2400},{4800,B4800},{9600,B9600},
                  {19200,B19200},{38400,B38400},{57600,B57600},
                  {115200,B115200}};
    for(int i=0;i<sizeof(map)/sizeof(int)/2;i++)
        if(map[i][0]==baud)
            return map[i][1];
    return -1;
}

int SerialPortBase::transformDataBits(int dataBits)
{
    int map[][2]={{5,CS5},{6,CS6},{7,CS7},{8,CS8}};
    for(int i=0;i<sizeof(map)/sizeof(int)/2;i++)
        if(map[i][0]==dataBits)
            return map[i][1];
    return -1;
}

bool SerialPortBase::endsWith(const char* str,int strLen,const char* end,int endLen)
{
    if(strLen<endLen)
        return false;
    for(int i=endLen-1;i>=0;i--)
        if(end[i]!=str[strLen-endLen+i])
            return false;
    return true;
}
