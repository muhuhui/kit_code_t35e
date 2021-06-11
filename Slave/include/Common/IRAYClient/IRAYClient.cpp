#include "LinuxDef.h"
#include "IRAYClient.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DATA_BYTE_SIZE (384*288*2)
#define IMAGE_HEIGHT 288
#define IMAGE_WIDTH 384

IRAYClient* IRAYClient::m_pInstance = NULL;


float getMaxOrMin(float *arr,int count,bool isMax){
	float temp=arr[0];
	for(int i=1;i<count;i++){
		if(isMax){
			if(temp<arr[i]){
			temp=arr[i];
			}
		}else{
			if(temp>arr[i]){
			temp=arr[i];
			}			
		}	
	}
	return temp;	
}

static float convertTempdataToC(unsigned short temp)
{
    float fTemp = 0.0f;
    if (temp <= 7300)
    {
        fTemp = (float) (temp + 7000) / 30.0 - 273.15;
    }
    else
    {
        fTemp = (float) (temp - 3300) / 15.0 - 273.15;
    }
    return fTemp;
}


static void call_back_nomalvideo(char* pbuff, int headsize, int datasize, int timetick, int biskeyframe, void* context)
{
	IRAYClient * pClient = IRAYClient::getInstance();
	pClient->client_set_dat_ptr(pbuff);
	pClient->client_set_head_size(headsize);
	pClient->client_set_dat_siz(datasize);
	
	//printf("%s Get video temperature frame: Address 0x%08x, Headsize: %d, Datasize: %d\n", __FUNCTION__, pbuff, headsize, datasize);
}

static void call_back_messagecallback(IRNETHANDLE hHandle, WPARAM wParam, LPARAM lParam, void* context)
{
    switch (wParam)
    {
    case  LAUMSG_LINKMSG:
    {
        if (lParam == 0)
        {
            printf("connect successfull @%d\n", __LINE__);
        }
        else
        {
            printf("connect failed @%d\n", __LINE__);
        }
        break;
    }
    default:
        break;
    }
    return;
}

IRAYClient* IRAYClient::getInstance()
{
	if (m_pInstance)
	{
		return m_pInstance;
	}
	else
	{
		m_pInstance = new IRAYClient();
		return m_pInstance;
	}
}

int IRAYClient::client_init(const char* ip_addr, const char* port_no, const char* user_name, const char* user_pwd)
{
    if (ip_addr) {
        snprintf(m_ip, sizeof(m_ip), "%s", ip_addr);
    }
    if (port_no) {
        snprintf(m_port, sizeof(m_port), "%s", port_no);
    }
    if (user_name) {
        snprintf(m_username, sizeof(m_username), "%s", user_name);
    }
    if (user_pwd) {
        snprintf(m_password, sizeof(m_password), "%s", user_pwd);
    }
    return 0;
}



//int IRAYClient::client_set_opt(const char* ip_addr, const char* port_no, const char* user_name, const char* user_pwd)
int IRAYClient::client_set_opt()
{
    m_client_info.m_sername = (char*)"server";
    m_client_info.m_username = m_username;
    m_client_info.m_password = m_password;
    m_client_info.m_buffnum = 50;
    m_client_info.m_ch = 0;                       //channel no
    m_client_info.m_hChMsgWnd = nullptr;
    m_client_info.m_hVideohWnd = nullptr;
    m_client_info.m_nChmsgid = 0;
    m_client_info.m_playstart = FALSE;            //NOTE, TRUE->FALSE
    m_client_info.m_tranType = 3;
    m_client_info.m_useoverlay = FALSE;
    m_client_info.nColorKey = 0x00FF00FF;
    m_client_info.context = (void*)2;
    m_client_info.m_messagecallback = call_back_messagecallback;
    return 0;
}

IRNETHANDLE IRAYClient::client_open()
{
    if (m_hchann != IRNETHANDLE(-1))
    {
        return m_hchann;
    }

    int port = atoi(m_port);
    //printf("IP: %s, Port: %d, Stream: %d\n", m_ip, port, STREAM_NO_TEMP);
    IRNETHANDLE handle = IRNET_ClientStart(m_ip, &m_client_info, port, STREAM_NO_TEMP);
    if (handle != IRNETHANDLE(-1))
    {
        m_hchann = handle;
	return m_hchann;
    }
    return IRNETHANDLE(-1);
}

int IRAYClient::client_start_nomal_cap()
{
    BOOL rc = IRNET_ClientStartNomalCap(m_hchann, call_back_nomalvideo, NULL, NULL, NULL);
    if (rc == true)
    {
	printf("%s succeed\n", __FUNCTION__);
        return 0; //succeeded
    }
    else
    {
	printf("%s failed (%d)\n", __FUNCTION__, rc);
    }

    return -1;    //failed
}

int IRAYClient::client_close()
{
    if (m_hchann != IRNETHANDLE(-1))
    {
        IRNET_ClientStop(m_hchann);
        IRNET_ClientCleanup();
        m_hchann = IRNETHANDLE(-1);
        return 0;
    }
    return -1;
}

int IRAYClient::client_get_temp_raw_dat()
{
    m_lck.lock();
    //start up
    IRNET_ClientStartup(0, NULL);
    IRNET_ClientWaitTime(30, 4);

    //FIXME: the param should be settable
    client_init("192.168.61.13", "3000", "888888", "888888"); 

    //set options
    IRAYClient::client_set_opt();

    //open
    IRNETHANDLE handle = client_open();
    if ((IRNETHANDLE)(-1) == handle)
    {
        printf("IRNET_ClientStart failed @%d\n", __LINE__);
        client_close();
        m_lck.unlock();
        return -1;
    }

    //wait until device ready
    sleep(1);

    //nomal capture
    client_start_nomal_cap();
    sleep(1);

    //dump_raw_dat();
    if (-1 ==dump_centigrade_dat())
    {
        client_close();
        m_lck.unlock();
        return -1;
    }

    //write file
    write_into_file("/tmp/temperature");

    client_close();
    m_lck.unlock();

    return 0;
}

void* IRAYClient::client_set_dat_ptr(char* addr)
{
	m_temp_raw_data_ptr = addr;
	return m_temp_raw_data_ptr;
}

int IRAYClient::client_set_head_size(int headSize)
{
	m_temp_raw_head_size = headSize;
	return m_temp_raw_head_size;
}

int IRAYClient::client_set_dat_siz(int datSize)
{
	m_temp_raw_data_size = datSize;
	return m_temp_raw_data_size;
}

int IRAYClient::dump_raw_dat()
{
#if 0
    printf("Data address: 0X%08x, HeaderSize: %d, DataSize: %d\n", m_temp_raw_data_ptr, m_temp_raw_head_size, m_temp_raw_data_size);
    unsigned short *ptr = (unsigned short *)m_temp_raw_data_ptr;
    for(int i=0; i<m_temp_raw_data_size/2; i++)
    {
	if(i%128 == 0) printf("\n===================== %d ===================\n", i);
        printf(" 0x%04x", *(ptr + i)); //raw data
    }
#endif
    return 0;
}

int IRAYClient::dump_centigrade_dat()
{
    printf("Data address: 0X%08x, HeaderSize: %d, DataSize: %d\n", m_temp_raw_data_ptr, m_temp_raw_head_size, m_temp_raw_data_size);
    unsigned short *ptr = (unsigned short *)m_temp_raw_data_ptr;

    if(m_temp_raw_data_size < DATA_BYTE_SIZE)
    {
        printf("IRAYClient::dump_centigrade_dat(), Data length not satify!\n");
        return -1;
    }
    //LT384
    // pixel: 384*288
    // 2 bytes per pixel
    //for(int i=0; i<m_temp_raw_data_size/2; i++)
/*     for(int i=0; i<(382*288); i++)
    {
	//if(i%128 == 0) printf("\n===================== %d ===================\n", i);
        //printf(" %02f", convertTempdataToC(*(ptr + i)));  //in degree centigrade
    } */
    return 0;
}


int IRAYClient::write_into_file(const char* file_name)
{
    if(!file_name)
    {
	printf("file is NULL, %s, %d\n", __FUNCTION__, __LINE__);
        return -1;
    }
    float* pbuf = NULL;

    //unsigned short -> float
    pbuf = (float*)malloc(DATA_BYTE_SIZE/2*4);
    if(!pbuf)
    {
	printf("malloc failed, %s, %d\n", __FUNCTION__, __LINE__);
        return -1;
    }

    //data -> buf
    int i = 0;
    unsigned short *ptr = (unsigned short *)m_temp_raw_data_ptr;
    while(i < DATA_BYTE_SIZE/2)
    {
        pbuf[i] = convertTempdataToC(*(ptr + i));
//        if(i%348 == 0){
//	     printf("%f\t", pbuf[i]);
//        }
        i++;
    }

    m_min_temp = getMaxOrMin(pbuf, sizeof(pbuf)/sizeof(float), false);
    m_max_temp = getMaxOrMin(pbuf, sizeof(pbuf)/sizeof(float), true);

    //open file
    FILE *pf;
    if ((pf = fopen(file_name, "w+")) == NULL) {
        printf("open file failed, %s, %s failed!\n", __FUNCTION__, file_name);
        free(pbuf);
        return -1;
    }

    //write
    fwrite(pbuf, 4, DATA_BYTE_SIZE/2, pf);

    //close
    fclose(pf);

    //free
    free(pbuf);
    return 0;
}

int IRAYClient::get_local_temp(std::vector<float> corners)
{
    if(corners.size() != 4)
    {
        printf("IRAYClient::get_local_temp, corners insufficient\n");
        return -1;
    }
    
    //防止溢出
    for(auto iter = corners.begin(); iter != corners.end(); iter++) {
        if(*iter < 0.01) {
            *iter = 0.01;
        }
        if(*iter > 0.99) {
            *iter = 0.99;
        }
    }

    if(corners[0] > corners[2]) {
        float temp = corners[0];
        corners[0] = corners[2];
        corners[2] = temp;
    }

    if(corners[1] > corners[3]) {
        float temp = corners[1];
        corners[1] = corners[3];
        corners[3] = temp;
    }

    m_lck.lock();
    //start up
    IRNET_ClientStartup(0, NULL);
    IRNET_ClientWaitTime(30, 4);

    //FIXME: the param should be settable
    client_init("192.168.61.13", "3000", "888888", "888888"); 

    //set options
    IRAYClient::client_set_opt();

    //open
    IRNETHANDLE handle = client_open();
    if ((IRNETHANDLE)(-1) == handle)
    {
        printf("IRNET_ClientStart failed @%d\n", __LINE__);
        client_close();
        m_lck.unlock();
        return -1;
    }

    //wait until device ready
    sleep(1);

    //nomal capture
    if(-1 == client_start_nomal_cap())
    {
        printf("IRNET_Client client_start_nomal_cap failed @%d\n", __LINE__);
        client_close();
        m_lck.unlock();
        return -1;
    }

    sleep(1);

    if (-1 == dump_centigrade_dat())
    {
        client_close();
        m_lck.unlock();
        return -1;
    }
    
    temp_extract(corners);
    client_close();
    m_lck.unlock();

    return 0;
}

void IRAYClient::temp_extract(std::vector<float> corners)
{
    //unsigned short -> float
    float* pbuf = (float*)malloc(DATA_BYTE_SIZE/2*4);

    //data -> buf
    int i = 0;
    unsigned short *ptr = (unsigned short *)m_temp_raw_data_ptr;
    while(i < DATA_BYTE_SIZE/2)
    {
        pbuf[i] = convertTempdataToC(*(ptr + i));
        i++;
    }

    int height = int(IMAGE_HEIGHT*corners[3]) - int(IMAGE_HEIGHT*corners[1]);
    int width = int(IMAGE_WIDTH*corners[2]) - int(IMAGE_WIDTH*corners[0]);

    printf("x1:%f, y1:%f; x2:%f, y2:%f; height:%d, width:%d \n", corners[0], corners[1], corners[2], corners[3], height, width);

    if(height >= IMAGE_HEIGHT){
        height = IMAGE_HEIGHT;
    }

    if(width >= IMAGE_WIDTH){
        width = IMAGE_WIDTH;
    }

    float* ext_buf = (float*)malloc((width*height*4));

    for(int row = 0; row < height; row++)
    {
        for(int col = 0; col < width; col++)
        {
            ext_buf[row*width+col] = pbuf[int(IMAGE_HEIGHT*corners[1])*IMAGE_WIDTH+row*IMAGE_WIDTH+int(IMAGE_WIDTH*corners[0])+col];
        }
    }

    m_min_temp = getMaxOrMin(ext_buf, width*height, false);
    m_max_temp = getMaxOrMin(ext_buf, width*height, true);

    free(pbuf);
    free(ext_buf);

    printf("IRAYClient::%s(), get min max temp as: %f, %f.\n", __FUNCTION__, m_min_temp, m_max_temp);
    return;
}
