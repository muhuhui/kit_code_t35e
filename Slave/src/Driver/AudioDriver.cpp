/*================================================================
*   Copyright (C) 2020 Tetraelc Ltd. All rights reserved.
*   
*   File name: AudioDriver.cpp
*   Author: JiangJin
*   Create Date: 2020-08-06
*   Description: 
*
================================================================*/


#include "AudioDriver.h"

#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thread>
#include "Transaction/Motor.h"
#include "Transaction/SettingService.h"
#include "Driver/PowerDriver.h"

#define LOG_TAG "AudioDriver"
#define ALARM_WAV_DIR "Config/alarm.wav"

AudioDriver::AudioDriver():
    m_isWarn(false)
{

}

AudioDriver::~AudioDriver()
{
    snd_pcm_drain(m_handle);
    snd_pcm_close(m_handle);
}

AudioDriver *AudioDriver::getInstance()
{
  return SingleTon<AudioDriver>::GetInstance();
}

int AudioDriver::init()
{
    int nRead;
    FILE *fp = fopen(ALARM_WAV_DIR, "rb");
    if(fp == NULL) {
        printf("%s::%s(), open file failed:\n", LOG_TAG, __FUNCTION__);
        return -1;
    }

    nRead = fread(&wav_header, 1, sizeof(wav_header), fp);

    printf("nread=%d\n", nRead);
    printf("rld=%4s\n", wav_header.rld);
    printf("rLen=%d\n", wav_header.rLen);
    printf("wld=%s\n", wav_header.wld);
    printf("fld=%s\n", wav_header.fld);
    printf("fLen=%d\n", wav_header.fLen);
    printf("wFormatTag=%d\n", wav_header.wFormatTag);
    printf("wChannels=%d\n", wav_header.wChannels);
    printf("nSamplesPersec=%d\n", wav_header.nSamplesPersec);
    printf("nAvgBitsPerSample=%d\n", wav_header.nAvgBitsPerSample);
    printf("wBlockAlign=%d\n", wav_header.wBlockAlign);
    printf("wBitsPerSample=%d\n", wav_header.wBitsPerSample);
    printf("dld=%s\n", wav_header.dld);
    printf("wSampleLength=%d\n", wav_header.wSampleLength);

    fclose(fp);

    m_periodSize = 1024;
    m_readSize = m_periodSize * wav_header.wBlockAlign;
    m_bufferSize = m_readSize * 2;

    /* std::thread runThread;
    runThread = std::thread(AudioDriver::audioPlayingThread, this);
    runThread.detach(); */

    return 0;
}

int AudioDriver::audioInit()
{
    int dir = 0;
    int nRet = 0;
    int channels = wav_header.wChannels;
    int frequency = wav_header.nSamplesPersec;
    int bit = wav_header.wBitsPerSample;

    nRet = snd_pcm_open(&m_handle, "default", SND_PCM_STREAM_PLAYBACK, 0);
    if(nRet < 0) {
        perror("\nopen PCM device failed:");
        return -1;
    }

    snd_pcm_hw_params_alloca(&m_params); //分配params结构体

    nRet = snd_pcm_hw_params_any(m_handle, m_params);//初始化params
    if(nRet < 0) {
        perror("\nsnd_pcm_hw_params_any:");
        return -1;
    }
    nRet = snd_pcm_hw_params_set_access(m_handle, m_params, SND_PCM_ACCESS_RW_INTERLEAVED); //初始化访问权限
    if(nRet < 0) {
        perror("\nsed_pcm_hw_set_access:");
        return -1;
    }

    //采样位数
    switch(bit/8) 
    {
        case 1:snd_pcm_hw_params_set_format(m_handle, m_params, SND_PCM_FORMAT_U8);
        break;
        case 2:snd_pcm_hw_params_set_format(m_handle, m_params, SND_PCM_FORMAT_S16_LE);
        break;
        case 3:snd_pcm_hw_params_set_format(m_handle, m_params, SND_PCM_FORMAT_S24_LE);
        break;
    }
    nRet = snd_pcm_hw_params_set_channels(m_handle, m_params, channels); //设置声道,1表示单声道，2表示立体声
    if(nRet < 0) {
        perror("\nsnd_pcm_hw_params_set_channels:");
        return -1;
    }
    nRet = snd_pcm_hw_params_set_rate_near(m_handle, m_params, (unsigned int*)&frequency, &dir); //设置频率
    if(nRet < 0) {
        perror("\nsnd_pcm_hw_params_set_rate_near:");
        return -1;
    }

    nRet = snd_pcm_hw_params_set_period_size_near(m_handle, m_params, &m_periodSize, &dir);
    if(nRet < 0) {
        perror("\nsnd_pcm_hw_params_set_period_size:");
        return -1;
    }

    nRet = snd_pcm_hw_params_set_buffer_size_near(m_handle, m_params, (snd_pcm_uframes_t *)&m_bufferSize); //设置周期长度
    if(nRet < 0) {
        perror("\nsnd_pcm_hw_params_set_buffer_size:");
        return -1;
    }

    nRet = snd_pcm_hw_params(m_handle, m_params);
    if(nRet < 0) {
        perror("\nsnd_pcm_hw_params: ");
        return -1;
    }

    printf("%s::%s(), m_periodSize: %d, m_readSize: %d, m_bufferSize: %d\n", LOG_TAG, __FUNCTION__, (int)m_periodSize, m_readSize, m_bufferSize);

    return 0;
}

#define DEBUG(x,y...)	{printf("[ %s : %s : %d] ",__FILE__, __func__, __LINE__); printf(x,##y); printf("\n");}

static char *alsa_mix_dev = "default";
static char *alsa_mix_headphone_ctrl = "Headphone";
static char *alsa_mix_pcm_ctrl = "PCM";
 
void AudioDriver::volumeInit(char *alsa_mix_ctrl, snd_mixer_t **alsa_mix_handle_p, snd_mixer_elem_t **alsa_mix_elem_p)
{
	int alsa_mix_index = 0;
	snd_mixer_selem_id_t *alsa_mix_sid = NULL;
 
	snd_mixer_selem_id_alloca(&alsa_mix_sid);
	snd_mixer_selem_id_set_index(alsa_mix_sid, alsa_mix_index);
	snd_mixer_selem_id_set_name(alsa_mix_sid, alsa_mix_ctrl);
 
	if ((snd_mixer_open(alsa_mix_handle_p, 0)) < 0)
		DEBUG ("Failed to open mixer");
	if ((snd_mixer_attach(*alsa_mix_handle_p, alsa_mix_dev)) < 0)
		DEBUG ("Failed to attach mixer");
	if ((snd_mixer_selem_register(*alsa_mix_handle_p, NULL, NULL)) < 0)
		DEBUG ("Failed to register mixer element");
 
	if (snd_mixer_load(*alsa_mix_handle_p) < 0)
		DEBUG ("Failed to load mixer element");
	*alsa_mix_elem_p = snd_mixer_find_selem(*alsa_mix_handle_p, alsa_mix_sid);
	if (!*alsa_mix_elem_p)
		DEBUG ("Failed to find mixer element");
	if (snd_mixer_selem_set_playback_volume_range (*alsa_mix_elem_p, 0, 100) < 0)
		DEBUG("Failed to set playback volume range");
}
 
void AudioDriver::volumeUninit(snd_mixer_t *alsa_mix_handle)
{
	if(alsa_mix_handle){
		snd_mixer_close(alsa_mix_handle);
	}
}
 
int AudioDriver::volumeGet(void)
{
	long ll, lr, vol;
	snd_mixer_t *alsa_mix_headphone_handle = NULL;
	snd_mixer_elem_t *alsa_mix_headphone_elem = NULL;
 
	//Headphone channel
	volumeInit(alsa_mix_headphone_ctrl, &alsa_mix_headphone_handle, &alsa_mix_headphone_elem);
	//处理事件
	snd_mixer_handle_events(alsa_mix_headphone_handle);
	//左声道
	snd_mixer_selem_get_playback_volume(alsa_mix_headphone_elem, SND_MIXER_SCHN_FRONT_LEFT, &ll);
	//右声道
	snd_mixer_selem_get_playback_volume(alsa_mix_headphone_elem, SND_MIXER_SCHN_FRONT_RIGHT, &lr);

	volumeUninit(alsa_mix_headphone_handle);
 
	vol = (int)((ll + lr) >> 1);
	return vol;
}
 
int AudioDriver::volumeSet(int vol)
{
	snd_mixer_t *alsa_mix_headphone_handle = NULL;
	snd_mixer_elem_t *alsa_mix_headphone_elem = NULL;
 
	if(vol > 100)
		vol = 100;
	if(vol < 0)
		vol = 0;
 
	//Headphone channel
	volumeInit(alsa_mix_headphone_ctrl, &alsa_mix_headphone_handle, &alsa_mix_headphone_elem);
	//左音量
	snd_mixer_selem_set_playback_volume(alsa_mix_headphone_elem, SND_MIXER_SCHN_FRONT_LEFT, vol);
	//右音量
	snd_mixer_selem_set_playback_volume(alsa_mix_headphone_elem, SND_MIXER_SCHN_FRONT_RIGHT, vol);

	volumeUninit(alsa_mix_headphone_handle);

	snd_mixer_t *alsa_mix_pcm_handle = NULL;
	snd_mixer_elem_t *alsa_mix_pcm_elem = NULL;
 
	//Headphone channel
	volumeInit(alsa_mix_pcm_ctrl, &alsa_mix_pcm_handle, &alsa_mix_pcm_elem);
	//左音量
	snd_mixer_selem_set_playback_volume(alsa_mix_pcm_elem, SND_MIXER_SCHN_FRONT_LEFT, vol);
	//右音量
	snd_mixer_selem_set_playback_volume(alsa_mix_pcm_elem, SND_MIXER_SCHN_FRONT_RIGHT, vol);

	volumeUninit(alsa_mix_pcm_handle);

	return 0;
}

int AudioDriver::audioPlay(char* filePath)
{
    int nRet = 0;
    char *buffer = (char*)malloc(m_readSize);

    FILE *fp = fopen(filePath, "rb");
    if(fp == NULL) {
        printf("%s::%s(), open file failed\n", LOG_TAG, __FUNCTION__);
        return -1;
    }

    fseek(fp, 58, SEEK_SET); //定位歌曲到数据区

    while (1)
    {
        memset(buffer, 0, m_readSize);
        nRet = fread(buffer, 1, m_readSize, fp);
        if(nRet == 0) {
            std::cerr << "end of file on input" << std::endl;
            break;
        }
        else if ((nRet > 0) && (nRet != m_readSize)) {
            std::cerr << "short read: read " << nRet << " bytes" << std::endl;
        }
        else if (nRet < 0) {
            std::cerr << "read error" << nRet << std::endl;
            break;
        }
        //写音频数据到PCM设备 
        int nTry = 0;
        while((nTry < 3) && ((nRet = snd_pcm_writei(m_handle, buffer, m_periodSize)) < 0))
        {
            usleep(2000); 
            if (nRet == -EPIPE) { //欠载
                fprintf(stderr, "underrun occurred\n");
                snd_pcm_prepare(m_handle);  //完成硬件参数设置，使设备准备好 
            }
            else if (nRet < 0) {
                fprintf(stderr, "error from writei: %s\n", snd_strerror(nRet));
            }
            nTry++;
        }
    }
    fclose(fp);
    free(buffer);
    return nRet;
}

void AudioDriver::audioPlayingThread(AudioDriver* pAudioDriver)
{
    printf("%s::%s(), start.\n", LOG_TAG, __FUNCTION__);
    // pAudioDriver->audioInit();
    int warnDelay = 0;
    SettingService* pService = SettingService::getInstance();
    Motor* pMotor = Motor::getInstance();
	int vol = 100;
    // pAudioDriver->volumeSet(vol);
    PowerDriver *pPowerDriver = PowerDriver::getInstance();
    while (1) 
    {
        if (pAudioDriver->isWarn()) {
            warnDelay = 500000;
        }
        else if (pMotor->getCurStatus() != STOP_STATE &&
            pService->m_setting.getMoveWarn()) {
            usleep(200000);
            if (pMotor->getCurStatus() != STOP_STATE) {
                warnDelay = 1000000;
            }
        }
        else {
            warnDelay = 0;
            sleep(1);
            continue;
        }
        // pAudioDriver->audioPlay((char*)ALARM_WAV_DIR);
        // usleep(warnDelay);
        pPowerDriver->switchPowerBeep(1);
        sleep(3);
        pPowerDriver->switchPowerBeep(0);
        usleep(warnDelay);
    }
    printf("%s::%s(), end.\n", LOG_TAG, __FUNCTION__);
}
