#ifndef SERIALPORTBASE_H
#define SERIALPORTBASE_H


/*https://zhoujianshi.github.io*/


/**********************************/
typedef long long timestamp_t;

class Timestamp
{
public:
    //获取以毫秒计的时间戳
    static timestamp_t now();
};

/**********************************/
class Stream
{

private:
    //缓冲区
    char* buffer;
    //缓冲区容量
    int capacity;
    //流的开头
    int start;
    //流的长度
    int length;

public:
    //初始化一个流，参数为：初始容量
    Stream(int initCapacity=16);
    //清理
    ~Stream();
    //获取流的长度
    int getLength();
    //向流的末尾增加一字符
    void append(char aChar);
    //向流的末尾增加多个字符
    void append(const char* buf,int len);
    //查看流的第一个字符，如果长度为0则返回0
    char peek();
    //查看流开头的多个字符，返回实际查看到的长度
    int peek(char* buf,int len);
    //取出流的第一个字符，如果长度为0则返回0
    char take();
    //取出流开头的多个字符，返回实际取出的长度
    int take(char* buf,int len);

private:
    //扩大容量至两倍
    void expand();

};


/**********************************/
#include <pthread.h>

//串口类
class SerialPortBase
{

public:
    //无校验
    static const int PARITY_NONE=0;
    //奇校验
    static const int PARITY_ODD=1;
    //偶校验
    static const int PARITY_EVEN=2;
    //函数成功
    static const int OK=1;
    //设备未找到
    static const int DEV_NOT_FOUND=-1;
    //不支持该波特率
    static const int BAUD_NOT_SUPPORTED=-2;
    //不支持该数据位数
    static const int DATABITS_NOT_SUPPORTED=-3;
    //不支持该校验模式
    static const int PARITYMODE_NOT_SUPPORTED=-4;
    //不支持该停止位数
    static const int STOPBITS_NOT_SUPPORTED=-5;
    //未知配置错误
    static const int CONFIG_FAIL=-6;
    //创建线程出错
    static const int NEW_THREAD_FAIL=-7;
    //成功读到结尾符
    static const int READ_END=1;
    //读取超时
    static const int READ_TIMEOUT=-1;
    //读取时缓冲区满
    static const int READ_BUFFER_FULL=-2;

private:
    //串口设备文件描述符
    int fd;
    //字符流
    Stream stream;
    //后台接收线程
    pthread_t tid;
    //对字符流加的锁
    pthread_mutex_t mutex;

    bool readEnable;

public:
    SerialPortBase();
    ~SerialPortBase();
    //开启串口，参数为：设备名、波特率、数据位数、校验模式、停止位数，返回函数执行结果
    int open(const char* dev,int baud,int dataBits,int parityMode,int stopBits);
    //关闭串口
    int close();
    //写串口，参数为：数据、长度，返回实际写入长度
    int write(const char* data,int len);
    //获取可读长度
    int available();
    //读串口，但不移除数据，返回实际读取长度
    int peek(char* buf,int len);
    //读串口，直到收到预期长度的数据或超时，参数为：接收缓冲区、预期接收长度、超时（毫秒）,返回实际读取长度
    int read(char* buf,int len,int timeout);
    //读串口，直到读到预期的结尾符或缓冲区满或超时，参数为：接收缓冲区、最大长度、预期结尾符、超时（毫秒）、实际接收长度，返回READ_END、READ_TIMEOUT或READ_BUFFER_FULL
    int read(char* buf,int maxLen,const char* end,int timeout,int* recvLen);

    void enableRead() { readEnable = true; }
    void disableRead() { readEnable = false; }

private:
    //将数字型波特率转化为系统调用参数
    int transformBaud(int baud);
    //将数字型数据位数转化为系统调用参数
    int transformDataBits(int dataBits);
    long long getTimestamp();
    //判断字符串str是否以字符串end结尾
    bool endsWith(const char* str,int strLen,const char* end,int endLen);

    //后台接收线程函数
    friend void* receiveThread(void* arg);

};



#endif // SERIALPORTBASE_H
