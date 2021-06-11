
#include "LinuxDef.h"

void Sleep(unsigned int dwMilliseconds)
{
	usleep(dwMilliseconds * 1000);
//	sleep(dwMilliseconds);
}

unsigned long GetTickCount()
{
    struct timeval  tv;
    gettimeofday(&tv, NULL);
    return ((tv.tv_sec/* * 1000*/) + (tv.tv_usec / 1000000));
}
unsigned long timeGetTime(void)
{
    struct timeval  tv;
    gettimeofday(&tv, NULL);
    return ((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
}

off_t tell(int fildes)
{
	return lseek(fildes, 0, SEEK_CUR);
}

int ioctlsocket (SOCKET sock, long cmd, unsigned long *argp)
{
	if (cmd != 1)//FIONBIO)
	{
		return 0;
	}
	
	int flags;
	if((flags = fcntl(sock,F_GETFL, 0)) < 0)
	{
		return 0;
	}
	
	if (*argp == 0)
	{
		flags &= ~O_NONBLOCK;
		if(fcntl(sock, F_SETFL, flags) < 0 )
		{
			return 0;
		}
	}
	else
	{
		flags |= O_NONBLOCK;
		if(fcntl(sock, F_SETFL, flags) < 0 )
		{
			return 0;
		}
	}

	return 1;
}

int WSAGetLastError(void)
{
	return errno;
}

