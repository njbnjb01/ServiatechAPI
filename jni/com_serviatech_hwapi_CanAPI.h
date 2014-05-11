/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class com_serviatech_hwapi_CanAPI */

#ifndef _Included_com_serviatech_hwapi_CanAPI
#define _Included_com_serviatech_hwapi_CanAPI
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     com_serviatech_hwapi_CanAPI
 * Method:    open
 * Signature: (Ljava/lang/String;)Ljava/io/FileDescriptor;
 */
JNIEXPORT jint JNICALL Java_com_serviatech_hwapi_CanAPI_opencan
  (JNIEnv *, jobject, jstring);

/*
 * Class:     com_serviatech_hwapi_CanAPI
 * Method:    close
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_serviatech_hwapi_CanAPI_closecan
  (JNIEnv *, jobject);

/*
 * Class:     com_serviatech_hwapi_CanAPI
 * Method:    sendcan
 * Signature: (I[B)I
 */
JNIEXPORT jint JNICALL Java_com_serviatech_hwapi_CanAPI_writecan
  (JNIEnv *, jobject, jint, jbyteArray);

/*
 * Class:     com_serviatech_hwapi_CanAPI
 * Method:    readcan
 * Signature: ()[B
 */
JNIEXPORT jbyteArray JNICALL Java_com_serviatech_hwapi_CanAPI_readcan
  (JNIEnv *, jobject);

#ifdef __cplusplus
}
#endif
#endif
