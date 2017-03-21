#ifndef _SYS_LOG_H
#define _SYS_LOG_H

#include <stdio.h>
#include "utPrt.h"
/*
	LOG_TAG should define before #include 
	#define LOG_TAG xxx
	#include "log.h"
*/
#define SYS_LOG_LEVEL SYS_LOG_LEVEL_DEBUG
#define SYS_LOG_DETAIL_LEVEL SYS_LOG_DETAIL_LEVEL_FUNCTION
#define _PRINT printf  //chose from printf or utPrt

/********************DONT TOUCH THIS**************************/
#define SYS_LOG_LEVEL_CLOSE 0
#define SYS_LOG_LEVEL_ERROR 1
#define SYS_LOG_LEVEL_INFO  2
#define SYS_LOG_LEVEL_DEBUG 3

#define SYS_LOG_DETAIL_LEVEL_NORMAL     0
#define SYS_LOG_DETAIL_LEVEL_FUNCTION   1
#define SYS_LOG_DETAIL_LEVEL_FILE       2
 
#ifndef LOG_TAG
#define LOG_TAG "SYSLOG"
#endif 
 
#ifndef SYS_LOG_LEVEL
#define SYS_LOG_LEVEL SYS_LOG_LEVEL_DEBUG
#endif

#ifndef SYS_LOG_DETAIL_LEVEL
#define SYS_LOG_DETAIL_LEVEL SYS_LOG_DETAIL_LEVEL_NORMAL
#endif

#if SYS_LOG_DETAIL_LEVEL == SYS_LOG_DETAIL_LEVEL_NORMAL
#define PRINT(fmt, ...) \
    _PRINT("%s: "fmt"\r\n",LOG_TAG,##__VA_ARGS__)
#elif SYS_LOG_DETAIL_LEVEL == SYS_LOG_DETAIL_LEVEL_FUNCTION
#define PRINT(fmt, ...) \
    _PRINT("%s: %s(%d): "##fmt"\r\n",LOG_TAG, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#elif SYS_LOG_DETAIL_LEVEL == SYS_LOG_DETAIL_LEVEL_FILE
#define PRINT(fmt, ...) \
    _PRINT("%s: %s(%d)-<%s>: "##fmt"\r\n",LOG_TAG, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#endif
 
#if SYS_LOG_LEVEL >= SYS_LOG_LEVEL_DEBUG
#define LOGD(fmt,...) PRINT(fmt,##__VA_ARGS__)
#else
#define LOGD(fmt,...) 
#endif
 
#if SYS_LOG_LEVEL >= SYS_LOG_LEVEL_INFO
#define LOGI(fmt,...) PRINT(fmt,##__VA_ARGS__)
#else
#define LOGI(fmt,...) 
#endif
 
#if SYS_LOG_LEVEL >= SYS_LOG_LEVEL_ERROR
#define LOGE(fmt,...) PRINT(fmt,##__VA_ARGS__)
#else
#define LOGE(fmt,...) 
#endif

extern void prta(void *array, u32 len);
#endif
