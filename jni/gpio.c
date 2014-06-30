#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <jni.h>
#include <stdio.h>
#include <stdlib.h>

#include "com_serviatech_hwapi_GPIO.h"
#include <android/log.h>
static const char *TAG="GPIO";
#define LOGI(fmt, args...) __android_log_print(ANDROID_LOG_INFO,  TAG, fmt, ##args)
#define LOGD(fmt, args...) __android_log_print(ANDROID_LOG_DEBUG, TAG, fmt, ##args)
#define LOGE(fmt, args...) __android_log_print(ANDROID_LOG_ERROR, TAG, fmt, ##args)

/*
 * Class:     com_serviatech_hwapi_GPIO
 * Method:    write
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_com_serviatech_hwapi_GPIO_write
  (JNIEnv *env, jclass clazz, jint gpio, jint value){
	int valuefd, exportfd, directionfd;
	int n;
	char gpio_str[50];
	char direction_path[50];
	char value_path[50];
	char value_str[50];

    exportfd = open("/sys/class/gpio/export", O_WRONLY);
    if (exportfd < 0)
    {
        LOGD("Cannot open GPIO to export it\n");
        return -1;
    }
    n = sprintf(gpio_str,"%d",gpio);
    if(n>3 && n<0){
    	return -1;
    }
    write(exportfd, gpio_str, n+1);
    close(exportfd);

    n = sprintf(direction_path,"/sys/class/gpio/gpio%d/direction",gpio);
    directionfd = open(direction_path, O_RDWR);
	if (directionfd < 0)
	{
		LOGD("Cannot open GPIO direction it\n");
		return -1;
	}
	write(directionfd, "out", 4);
	close(directionfd);

	n = sprintf(value_path,"/sys/class/gpio/gpio%d/value",gpio);
	valuefd = open(value_path, O_RDWR);
	if (valuefd < 0)
	{
		LOGD("Cannot open GPIO value\n");
		return -1;
	}

	n = sprintf(value_str,"%d",value);
	if(n==1 && (value ==0 || value == 1))
	write(valuefd,value_str, 2);

	return 0;
}

/*
 * Class:     com_serviatech_hwapi_GPIO
 * Method:    read
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_com_serviatech_hwapi_GPIO_read
  (JNIEnv *env, jclass clazz, jint gpio){
	int valuefd, exportfd, directionfd;
	int n;
	char gpio_str[50];
	char direction_path[50];
	char value_path[50];
	char value_ch;
	exportfd = open("/sys/class/gpio/export", O_WRONLY);
	if (exportfd < 0)
	{
		LOGD("Cannot open GPIO to export it\n");
		return -1;
	}
	n = sprintf(gpio_str,"%d",gpio);
	if(n>3 && n<0){
		return -1;
	}
	write(exportfd, gpio_str, n+1);
	close(exportfd);

	n = sprintf(direction_path,"/sys/class/gpio/gpio%d/direction",gpio);
	directionfd = open(direction_path, O_RDWR);
	if (directionfd < 0)
	{
		LOGD("Cannot open GPIO direction it\n");
		return -1;
	}
	write(directionfd, "in", 3);
	close(directionfd);

	n = sprintf(value_path,"/sys/class/gpio/gpio%d/value",gpio);
	valuefd = open(value_path, O_RDWR);
	if (valuefd < 0)
	{
		LOGD("Cannot open GPIO value\n");
		return -1;
	}

	read(valuefd, &value_ch, 1);
	close(valuefd);

	if(value_ch == '0')
		return 0;
	else if(value_ch == '1')
		return 1;
}
