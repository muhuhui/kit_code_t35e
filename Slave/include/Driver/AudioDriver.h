/*================================================================
*   Copyright (C) 2020 Tetraelc Ltd. All rights reserved.
*   
*   File name: AudioDriver.h
*   Author: JiangJin
*   Create Date: 2020-08-06
*   Description: 
*
================================================================*/


#pragma once

#include <alsa/asoundlib.h>
#include "base/SingleTon.h"

class AudioDriver
{
public:
    AudioDriver();
    ~AudioDriver();
    int init();
    int audioInit();
    int audioPlay(char* filePath);
    void volumeInit(char *alsa_mix_ctrl, snd_mixer_t **alsa_mix_handle_p, snd_mixer_elem_t **alsa_mix_elem_p);
    void volumeUninit(snd_mixer_t *alsa_mix_handle);
    int volumeGet(void);
    int volumeSet(int vol);
    bool isWarn() { return m_isWarn; }
    void setWarn(bool isWarn) { m_isWarn = isWarn; }
    static AudioDriver *getInstance();

private:
    struct WAV_HEADER
    {
        char rld[4];            //riff 标志符号
        int rLen;
        char wld[4];            //格式类型（wave）
        char fld[4];            //"fmt"
        int fLen;               //sizeof(wave format matex)
        short wFormatTag;       //编码格式
        short wChannels;        //声道数
        int nSamplesPersec ;    //采样频率
        int nAvgBitsPerSample;  //WAVE文件采样大小
        short wBlockAlign;      //块对齐
        short wBitsPerSample;   //WAVE文件采样大小
        char dld[4];            //data
        int wSampleLength;      //音频数据的大小
    } wav_header;

    snd_pcm_t* m_handle;          //PCI设备句柄
    snd_pcm_hw_params_t* m_params;//硬件信息和PCM流配置
    snd_pcm_uframes_t m_periodSize;   //中断接收的字节
    int m_readSize;               //一次读取大小
    int m_bufferSize;             //缓冲区大小
    bool m_isWarn;                //是否告警

    static void audioPlayingThread(AudioDriver* pAudioDriver);
};

