#pragma once
#include<stdio.h>
#include<stdlib.h>
#include <string.h>
#include "cJSON.h"
#include "curl/curl.h"

void ParseToJson(const char *pData, int length);

void URLDownload(const char* url);

size_t write_file(void *ptr, size_t size, size_t nmemb, FILE *stream);
