#include "base/GPIOBase.h"
#include <thread>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>



extern "C"
{
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define ERROR 0x01
#define IN	  0x80
#define OUT	  0x40
#define PINMAX	223
#define PINMIN	0

#define	DEV_INFO	"[GPIO_TEST]"

//#define	GPIO_DEBUG

#ifdef	GPIO_DEBUG
#define	dbg_info(format, args...)	printf("%d:"format, __LINE__, ##args)
#else
#define	dbg_info(format, args...)
#endif
#define	dbg_err(format, args...)	printf("%d:"format, __LINE__, ##args)



void umsg(void){
  printf("usage: gpio_in num1~num2 value \n\t example: gpio_out 7~14 1\n");
  exit(1);
}

static int gpio_export(int pin){
  char name[4];
  int fd,len;

  fd = open("/sys/class/gpio/export", O_WRONLY);
  if (fd < 0) {
    perror("open export");
    return(-1);
  }
  len = sprintf(name, "%d", pin);
  dbg_info("gpiopin = gpio%s\n",name);
  if (write(fd, name, sizeof(name)) < 0) {
    perror("write export");
    close(fd);
    return -1;
  }
  close(fd);
  return 0;
}
static int gpio_direction(int pin,char *dir){
  char path[64];
  int fd,res;

  sprintf(path,"/sys/class/gpio/gpio%d/direction", pin);

  dbg_info("direction path: %s,direction = %s\n", path, dir);

  fd = open(path, O_WRONLY);
  if (fd < 0) {
    perror("open direction");
    return -1;
  }
  res = write(fd, dir, sizeof(dir));
  if (res <= 0) {
    perror("write direction");
    close(fd);
    return -1;
  }
  close(fd);
  return 0;
}

static int gpio_write(int pin, int value){
  char path[64];
  char value_s[4];
  int fd;
  int len;

  sprintf(path, "/sys/class/gpio/gpio%d/value",pin);
  len = sprintf(value_s, "%d", value);
  dbg_info("value path: %s, value = %d\n", path,value);

  fd = open(path, O_RDWR);
  if (fd < 0) {
    perror("open value");
    return -1;
  }

  int res = write(fd, value_s, len);
  if (res <= 0) {
    perror("write value");
    close(fd);
    return -1;
  }
  close(fd);
  return 0;
}
static int gpio_read(int pin,int *value){
  char path[64];
  char value_str[4];
  int fd;

  sprintf(path, "/sys/class/gpio/gpio%d/value", pin);
  dbg_info("value path: %s\n", path);
  fd = open(path, O_RDONLY);
  if (fd < 0) {
    perror("open value");
    return -1;
  }
  if (read(fd, value_str, 3) < 0) {
    perror("read value");
    close(fd);
    return -1;
  }
  sscanf(value_str,"%d",value);

  dbg_info("value_read = %d\n",*value);

  close(fd);
  return 0;
}
static int gpio_unexport(int pin)
{
  char name[4];
  int len;
  int fd;

  fd = open("/sys/class/gpio/unexport", O_WRONLY);
  if (fd < 0) {
    perror("open unexport");
    return -1;
  }
  len = sprintf(name, "%d", pin);
  dbg_info("name = gpio%s\n",name);
  if (write(fd, name, len) < 0) {
    perror("write unexport");
    close(fd);
    return -1;
  }
  close(fd);
  return 0;
}

}

GPIOBase::~GPIOBase()
{
  m_bThreadRun = false;
}

GPIOBase *GPIOBase::getInstance()
{
  return SingleTon<GPIOBase>::GetInstance();
}

//添加要轮询的gpio输入口
int GPIOBase::initMonitor(int *pins, int nCnt)  
{

  for(int i = 0; i < nCnt; ++i) {
    m_pinMap.insert(std::make_pair(pins[i], 0));
  }

  if(false == m_bInit) {

    m_bInit = true;
    m_bThreadRun = true;

    std::thread runThread;
    runThread = std::thread(GPIOBase::run, this);
    runThread.detach();
  }
}

int GPIOBase::getValue(int nPin)
{
  return m_pinMap[nPin];
}

#define MAX_QUEUE_SIZE 100

void GPIOBase::setValue(int nPin, int nValue)
{
    static pthread_mutex_t wMtx = PTHREAD_MUTEX_INITIALIZER;

    pthread_mutex_lock(&wMtx);

    if (m_writePins.size() >= MAX_QUEUE_SIZE) {
        m_writePins.pop();
    }
    m_writePins.push(std::make_pair(nPin, nValue));

    pthread_mutex_unlock(&wMtx);
}

void GPIOBase::getPins()
{
  int nRet = 0;
  int nPin = 0;
  int nVal = 0;
  std::map<int, int>::iterator iter = m_pinMap.begin();

  while (iter != m_pinMap.end())
  {
    nPin = iter->first;
    nRet = readPin(nPin, nVal);
    if( 0 == nRet ) {
      m_pinMap[nPin] = nVal;
    }

    ++iter;
  }
}

int GPIOBase::readPin(int nPin, int &nVal)
{
  int res;
  char sDirect[8] = "in";
  char buffer[64];

  snprintf(buffer, sizeof(buffer), "/sys/class/gpio/gpio%d",nPin);
  if(access(buffer, F_OK) != 0) {
      res = gpio_export(nPin);
      if(res < 0)
        return -1;
  }

  res = gpio_direction(nPin, sDirect);
  if(res < 0)
    return -1;
  res = gpio_read(nPin, &nVal);
  if(res < 0)
    return -1;

  return 0;
}

void GPIOBase::writePins()
{
    if (m_writePins.size() > 0) {
        std::pair<int, int> pinPair = m_writePins.front();
        if (writePin(pinPair.first, pinPair.second) < 0) {
            printf("gpio %d write value: %d failed!\n", pinPair.first, pinPair.second);
        }
        m_writePins.pop();
    }
}

int GPIOBase::writePin(int pin, int value)
{
  int res;
  char sDirect[8] = "high";
  char buffer[64];

  snprintf(buffer, sizeof(buffer), "/sys/class/gpio/gpio%d", pin);
  if(access(buffer, F_OK) != 0) {
      res = gpio_export(pin);
      if(res < 0)
        return -1;
  }

  if(value == 0) {
      strncpy(sDirect, "low", sizeof(sDirect));
  }
  res = gpio_direction(pin, sDirect);
  if(res < 0)
    return -1;

  return 0;
}

void GPIOBase::run(GPIOBase *pGPIOBase)
{
  printf("--- GPIOBase run Start...\n");

  const int nBaseFrequence = 50000;   //0.05s

  while (pGPIOBase->m_bThreadRun) {
      pGPIOBase->getPins();
      pGPIOBase->writePins();

      usleep(nBaseFrequence);
  }
  printf("--- GPIOBase run Over...\n");
}
