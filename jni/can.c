#include "com_serviatech_hwapi_CanAPI.h"
#include "can.h"

#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include<sys/time.h>
#include <sys/select.h>
#include <sys/timeb.h>
#include <pthread.h>
#include <stdio.h>
#include<unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/net.h>

#define NO_Baudrate -2
#define OPEN_DEV_ERROR -3
#define CONFIG_DEV_ERROR -4
#define SETTIMEOUT_ERROR -5
#define NOT_FOR_READ -6
#define PREPARE_READ_ERROR -7
#define READ_DATA_ERROR -8
#define WRITE_ERROR -9
#define NO_DEVICE_OPEN -10
#define NOT_FOR_WRITE -11
#define PREPARE_WRITE_ERROR -12

enum sock_type {
	SOCK_STREAM	= 1,
	SOCK_DGRAM	= 2,
	SOCK_RAW	= 3,
	SOCK_RDM	= 4,
	SOCK_SEQPACKET	= 5,
	SOCK_DCCP	= 6,
	SOCK_PACKET	= 10,
};

#ifndef PF_CAN
#define PF_CAN 29
#endif

#ifndef AF_CAN
#define AF_CAN PF_CAN
#endif

struct sockaddr_can addr;
int openfd = -1;

#include <android/log.h>
#define LOG_TAG "CanJNI"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN,LOG_TAG,__VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

/*
 * Class:     com_serviatech_hwapi_CanAPI
 * Method:    open
 * Signature: (Ljava/lang/String;)Ljava/io/FileDescriptor;
 */
JNIEXPORT jint JNICALL Java_com_serviatech_hwapi_CanAPI_opencan
  (JNIEnv *env, jobject thiz, jstring dev){

	struct ifreq ifr;

	const char *dev_name = (char *)(*env)->GetStringUTFChars(env, dev, NULL);
	LOGI("Opening dev path %s", dev_name);

	openfd = socket(PF_CAN,SOCK_RAW,CAN_RAW);
	LOGI("openfd = %d in open", openfd);
	if (openfd < 0 ){
		/* Throw an exception */
		LOGE("Cannot open dev");
		/* TODO: throw an exception */
		return OPEN_DEV_ERROR;
	}

	strcpy((char *)(ifr.ifr_name),dev_name);
	ioctl(openfd, SIOCGIFINDEX,&ifr);
	LOGI("can_ifindex = %x\n",ifr.ifr_ifindex);

	addr.can_family = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;
	bind(openfd, (struct sockaddr*)&addr, sizeof(addr));
	(*env)->ReleaseStringUTFChars(env, dev, dev_name);

	int loopback = 1; // 0表示关闭, 1表示开启(默认)
	setsockopt(openfd, SOL_CAN_RAW, CAN_RAW_LOOPBACK, &loopback, sizeof(loopback));
	int ro = 1; // 0表示关闭(默认), 1表示开启
	setsockopt(openfd, SOL_CAN_RAW, CAN_RAW_RECV_OWN_MSGS, &ro, sizeof(ro));

	return openfd;
}

/*
 * Class:     com_serviatech_hwapi_CanAPI
 * Method:    close
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_serviatech_hwapi_CanAPI_closecan
  (JNIEnv *env, jobject thiz){
	unsigned i;
	LOGI("#########CloseCan");
	close(openfd);
	LOGI("openfd=%d",openfd);
	openfd = -1;
}

/*
 * Class:     com_serviatech_hwapi_CanAPI
 * Method:    sendcan
 * Signature: (I[B)I
 */
JNIEXPORT jint JNICALL Java_com_serviatech_hwapi_CanAPI_writecan
  (JNIEnv *env, jobject thiz, jint canid, jbyteArray dataArr){

	jbyte *data;

	struct ifreq ifr;
	struct can_frame frame;
	int nbytes;
	int i;

	LOGI("#########WriteCan");
	if(openfd < 0){
		LOGE("can is down");
		return NO_DEVICE_OPEN;
	}

	//int data_len = sizeof(dataArr) / sizeof(dataArr[0]);
	jsize data_len = (*env)->GetArrayLength(env, dataArr);
	if(data_len>8){
		LOGE("can data_len: %d >8!",data_len);
		return -1;
	}
	LOGI("can data_len : %d!",data_len);

	data = (*env)->GetByteArrayElements(env, dataArr, NULL);
	for(i=0;i<data_len;i++){
		frame.data[i] = data[i];
		LOGI("frame.data[%d] = %d",i,data[i]);
	}
	frame.can_id = canid;
	frame.can_dlc = data_len;
	LOGI("openfd = %d in write", openfd);
	nbytes = sendto(openfd,&frame,sizeof(struct can_frame),0,(struct sockaddr*)&addr,sizeof(addr));
	//nbytes = write(openfd, &frame, sizeof(frame));
	(*env)->ReleaseByteArrayElements(env, dataArr,data,0);

	return nbytes;//返回写入的字节数
}

/*
 * Class:     com_serviatech_hwapi_CanAPI
 * Method:    readcan
 * Signature: ()[B
 */
JNIEXPORT jbyteArray JNICALL Java_com_serviatech_hwapi_CanAPI_readcan
  (JNIEnv *env, jobject thiz){
	int i,  ret, s, len;
	unsigned long nbytes;
	struct ifreq ifr;
	struct can_frame frame;

	LOGI("#########ReadCan");
	if(openfd < 0){
		LOGE("can is down");
		return NO_DEVICE_OPEN;
	}
	LOGI("openfd = %d", openfd);
	nbytes = recvfrom(openfd,&frame,sizeof(struct can_frame),0,(struct sockaddr *)&addr,&len);
	////for debug
	ifr.ifr_ifindex = addr.can_ifindex;
	ioctl(s,SIOCGIFNAME,&ifr);
	LOGI("Received a CAN frame from interface %s\n",ifr.ifr_name);
	LOGI("frame message\n"
		"--can_id = %x\n"
		"--can_dlc = %x\n"
		"--data = %s\n",frame.can_id,frame.can_dlc,frame.data);

	jbyte *by = (jbyte*)frame.data;
	jbyteArray jarray = (*env)->NewByteArray(env,frame.can_dlc);
	(*env)->SetByteArrayRegion(env,jarray, 0, frame.can_dlc, by);

	return jarray;
}
