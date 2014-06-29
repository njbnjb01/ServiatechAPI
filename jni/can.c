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
//	LOGI("openfd = %d in open", openfd);
	if (openfd < 0 ){
		/* Throw an exception */
		LOGE("Cannot open dev");
		/* TODO: throw an exception */
		return OPEN_DEV_ERROR;
	}

	strcpy((char *)(ifr.ifr_name),dev_name);
	ioctl(openfd, SIOCGIFINDEX,&ifr);
//	LOGI("can_ifindex = %x\n",ifr.ifr_ifindex);

	addr.can_family = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;
	bind(openfd, (struct sockaddr*)&addr, sizeof(addr));
	(*env)->ReleaseStringUTFChars(env, dev, dev_name);

	int loopback = 0; // 0表示关闭, 1表示开启(默认)
	setsockopt(openfd, SOL_CAN_RAW, CAN_RAW_LOOPBACK, &loopback, sizeof(loopback));
	int ro = 0; // 0表示关闭(默认), 1表示开启
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
//	LOGI("openfd=%d",openfd);
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
//	LOGI("can data_len : %d!",data_len);

	data = (*env)->GetByteArrayElements(env, dataArr, NULL);
	for(i=0;i<data_len;i++){
		frame.data[i] = data[i];
//		LOGI("frame.data[%d] = %d",i,data[i]);
	}
	frame.can_id = canid;
	frame.can_dlc = data_len;
//	LOGI("openfd = %d in write", openfd);
	nbytes = sendto(openfd,&frame,sizeof(struct can_frame),0,(struct sockaddr*)&addr,sizeof(addr));
	//nbytes = write(openfd, &frame, sizeof(frame));
	(*env)->ReleaseByteArrayElements(env, dataArr,data,0);

	return nbytes;//返回写入的字节数
}

/*
 * Class:     com_serviatech_hwapi_CanAPI
 * Method:    readcan
 * Signature: ()Lcom/serviatech/hwapi/CanFrame;
 */
JNIEXPORT jobject JNICALL Java_com_serviatech_hwapi_CanAPI_readcan
  (JNIEnv *env, jobject thiz){
	int i,  ret, s, len;
	unsigned long nbytes;
	struct ifreq ifr;
	struct can_frame frame;
	int can_data[8];

	static jobject global_frame = NULL;

	if(global_frame!=NULL){
		(*env)->DeleteGlobalRef(env, global_frame);
//		LOGI("release global ref..");
	}
//
//	LOGI("#########ReadCan");
	if(openfd < 0){
		LOGE("can is down");
		return NO_DEVICE_OPEN;
	}
//	LOGI("openfd = %d", openfd);

	jclass objectClass = (*env)->FindClass(env, "com/serviatech/hwapi/CanFrame");
//	jfieldID id = (*env)->GetFieldID(env, objectClass,"id","I");
//	jfieldID dlc = (*env)->GetFieldID(env, objectClass,"dlc","B");
//	jfieldID data = (*env)->GetFieldID(env, objectClass,"data","[I");
	jmethodID jmiInit = (*env)->GetMethodID(env, objectClass,"<init>","(IB[I)V");

	nbytes = recvfrom(openfd,&frame,sizeof(struct can_frame),0,(struct sockaddr *)&addr,&len);
	//for debug
	ifr.ifr_ifindex = addr.can_ifindex;
	ioctl(s,SIOCGIFNAME,&ifr);
//	LOGI("Received a CAN frame from interface %s\n",ifr.ifr_name);
//	LOGI("frame message\n"
//		"--can_id = %x\n"
//		"--can_dlc = %x\n"
//		"--data = %s\n",frame.can_id,frame.can_dlc,frame.data);

	for(i=0;i<frame.can_dlc;i++){
		can_data[i] = (int)0xFF&frame.data[i];
//		LOGI("frame.data[%d] = %d can_data[%d]= %d \n",i,frame.data[i],i,can_data[i]);
	}

	jint *by = (jint*)can_data;
	jintArray jarray = (*env)->NewIntArray(env,frame.can_dlc);
	(*env)->SetIntArrayRegion(env,jarray, 0, frame.can_dlc, by);
//	(*env)->SetObjectField(env,jcan, data, jarray);
//	(*env)->SetIntField(env, jcan, id, frame.can_id);
//	(*env)->SetByteField(env, jcan, dlc, frame.can_dlc);

	jobject jcan = (*env)->NewObject(env,objectClass,jmiInit,frame.can_id,frame.can_dlc,jarray);
	global_frame = (*env)->NewGlobalRef(env, jcan);

	(*env)->DeleteLocalRef(env, jcan);
	(*env)->DeleteLocalRef(env, jarray);

	return global_frame;
}
/*
 * Class:     com_serviatech_hwapi_CanAPI
 * Method:    setFilter
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_com_serviatech_hwapi_CanAPI_setFilter
  (JNIEnv *env, jobject thiz, jint canid, jint mask){
	int ret;
	struct can_filter filter[1];
	filter[0].can_id = canid;
	filter[0].can_mask = mask;

	ret = setsockopt(openfd, SOL_CAN_RAW, CAN_RAW_FILTER, &filter, sizeof(filter));

	return ret;
}
