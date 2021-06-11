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

//��������ʼ��
bool cBuffer_Init(CBuffer *pBuf);

//�����������Ԫ��
void cBuffer_Add(CBuffer *pBuf, char data);

//�ӻ������л�ȡָ��λ����ָ�����ȵ�����
char* cBuffer_Get(CBuffer *pBuf, int start,int length);

//�ӻ�����ͷ����ɾ��ָ�����ȵ�����
void cBuffer_Delete(CBuffer *pBuf, int length);

//��������
void cBuffer_Clear(CBuffer *pBuf);

//���������ݽ���
bool cBuffer_Parse(CBuffer *pBuf);
