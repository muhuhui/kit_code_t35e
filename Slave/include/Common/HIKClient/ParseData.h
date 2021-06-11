#pragma once
#include<stdio.h>
#include<stdlib.h>
#include <string.h>
#include<time.h>
#include "XmlParse.h"
#include "JsonParse.h"
#define xml 1
#define jjson 2
#define img 3
#define SIZE 128
#define XML "application/xml"
#define JJSON "application/json"
#define IMG "image"
#define END1 '\r'
#define END2 '\n'
#define END3 "\r\n"

typedef struct node
{
	char data;
	node *next;
}node;
typedef struct CBuffer
{
	int size;
	node *head;
	node *end;
}CBuffer;

//缓冲区初始化
bool cBuffer_Init(CBuffer *pBuf);

//缓冲区添加新元素
void cBuffer_Add(CBuffer *pBuf, char data);

//从缓冲区中获取指定位置起指定长度的数据
char* cBuffer_Get(CBuffer *pBuf, int start,int length);

//从缓冲区头部起删除指定长度的数据
void cBuffer_Delete(CBuffer *pBuf, int length);

//清理缓冲区
void cBuffer_Clear(CBuffer *pBuf);

//缓冲区数据解析
bool cBuffer_Parse(CBuffer *pBuf);
