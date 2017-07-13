/*
 *  Author : WangBoJing , email : 1989wangbojing@gmail.com
 *
 *  Copyright Statement:
 *  --------------------
 *  This software is protected by Copyright and the information contained
 *  herein is confidential. The software may not be copied and the information
 *  contained herein may not be used or disclosed except with the written
 *  permission of Author. (C) 2016
 *
 *

****       *****
  ***        *
  ***        *                         *               *
  * **       *                         *               *
  * **       *                         *               *
  *  **      *                        **              **
  *  **      *                       ***             ***
  *   **     *       ******       ***********     ***********    *****    *****
  *   **     *     **     **          **              **           **      **
  *    **    *    **       **         **              **           **      *
  *    **    *    **       **         **              **            *      *
  *     **   *    **       **         **              **            **     *
  *     **   *            ***         **              **             *    *
  *      **  *       ***** **         **              **             **   *
  *      **  *     ***     **         **              **             **   *
  *       ** *    **       **         **              **              *  *
  *       ** *   **        **         **              **              ** *
  *        ***   **        **         **              **               * *
  *        ***   **        **         **     *        **     *         **
  *         **   **        **  *      **     *        **     *         **
  *         **    **     ****  *       **   *          **   *          *
*****        *     ******   ***         ****            ****           *
                                                                       *
                                                                      *
                                                                  *****
                                                                  ****


 *
 */

#include <string.h>
#include <errno.h>

#include "NattyClient-jni.h"
#include "NattyProtoClient.h"

#include <pthread.h>
#if 0
#include <android/log.h>
#endif 


JavaVM *gJavaVM;
jobject gJavaObj;

static volatile int gIsThreadExit = 0;


static void* native_thread_exec(void *arg){
	JNIEnv *env;
	(*gJavaVM)->AttachCurrentThread(gJavaVM, (void**)&env, NULL);
	LOG("AttachCurrentThread");
	jclass javaClass = (*env)->GetObjectClass(env, gJavaObj);
	if (javaClass == NULL) {
		LOG("Fail to find javaClass");
		return 0;
	}
	LOG("javaClass");
#if 0
	jmethodID javaCallback = (*env)->GetMethodID(env, javaClass, "onNativeCallback", "(I)V");
	if (javaCallback == NULL) {
		LOG("Fail to find method onNativeCallback");
		return 0;
	}

	int count = 0;
	while (!gIsThreadExit) {
		(*env)->CallVoidMethod(env, gJavaObj, javaCallback, count++);
		sleep(2);
	}
#elif 1
	U8 json[4] = {'A', 'B', 'C', 'D'};
	int length = 4;

	jmethodID javaCallback = (*env)->GetMethodID(env, javaClass, "ntyNativeLoginAckResult", "([BI)V");
	if (javaCallback == NULL) {
		LOG("Fail to find method %s", "ntyNativeLoginAckResult");
		return ;
	}
	while (!gIsThreadExit) {
		LOG("gIsThreadExit loop leave");
		(*env)->CallVoidMethod(env, gJavaObj, javaCallback, json, length);
		sleep(2);
	}
#else
#endif
	(*gJavaVM)->DetachCurrentThread(gJavaVM);

	LOG("native_thread_exec loop leave");
}

void Java_com_wbj_ndk_natty_client_NattyClient_nativeInitilize(JNIEnv *env, jobject thiz) {
	(*env)->GetJavaVM(env, &gJavaVM);
	gJavaObj = (*env)->NewGlobalRef(env, thiz);
}

void Java_com_wbj_ndk_natty_client_NattyClient_nativeThreadStart(JNIEnv *env, jobject thiz) {
	gIsThreadExit = 0;
	pthread_t threadId;
	if (pthread_create(&threadId, NULL, native_thread_exec, NULL) != 0) {
		LOG("native thread start failed !");
		return;
	}

	LOG("native thread start success !");
}

void Java_com_wbj_ndk_natty_client_NattyClient_nativeThreadStop(JNIEnv *env, jobject thiz) {
	gIsThreadExit = 1;
	LOG("native thread stop success !");
}

jstring Java_com_wbj_ndk_natty_client_NattyClient_stringFromJNI(JNIEnv *env, jobject thiz) {

#if defined(__arm__)
  #if defined(__ARM_ARCH_7A__)
    #if defined(__ARM_NEON__)
      #if defined(__ARM_PCS_VFP)
        #define ABI "armeabi-v7a/NEON (hard-float)"
      #else
        #define ABI "armeabi-v7a/NEON"
      #endif
    #else
      #if defined(__ARM_PCS_VFP)
        #define ABI "armeabi-v7a (hard-float)"
      #else
        #define ABI "armeabi-v7a"
      #endif
    #endif
  #else
   #define ABI "armeabi"
  #endif
#elif defined(__i386__)
   #define ABI "x86"
#elif defined(__x86_64__)
   #define ABI "x86_64"
#elif defined(__mips64)  /* mips64el-* toolchain defines __mips__ too */
   #define ABI "mips64"
#elif defined(__mips__)
   #define ABI "mips"
#elif defined(__aarch64__)
   #define ABI "arm64-v8a"
#else
   #define ABI "unknown"
#endif

    return (*env)->NewStringUTF(env, "Hello from JNI !  Compiled with ABI " ABI ".");//
}

jint Java_com_wbj_ndk_natty_client_NattyClient_add(JNIEnv *env, jobject thiz, jint a, jint b) {
	return a+b;
}


void ntyCallJavaFunction(const char *func, int arg) {
	JNIEnv *env;
	LOG("ntyCallJavaFunction");
	(*gJavaVM)->AttachCurrentThread(gJavaVM, (void**)&env, NULL);
	LOG("AttachCurrentThread");
	jclass javaClass = (*env)->GetObjectClass(env, gJavaObj);
	if (javaClass == NULL) {
		LOG("Fail to find javaClass");
		return ;
	}
	LOG("javaClass");
	jmethodID javaCallback = (*env)->GetMethodID(env, javaClass, func, "(I)V");
	if (javaCallback == NULL) {
		LOG("Fail to find method %s", func);
		return ;
	}
	LOG("javaCallback %s", func);
	(*env)->CallVoidMethod(env, gJavaObj, javaCallback, arg);

	(*gJavaVM)->DetachCurrentThread(gJavaVM);
	LOG("native_thread_exec loop leave");
}

void ntyCallJavaFunctionNoThread(const char *func, int arg) {
	JNIEnv *env;
	LOG("ntyCallJavaFunction");
	(*gJavaVM)->AttachCurrentThread(gJavaVM, (void**)&env, NULL);
	LOG("AttachCurrentThread");
	jclass javaClass = (*env)->GetObjectClass(env, gJavaObj);
	if (javaClass == NULL) {
		LOG("Fail to find javaClass");
		return;
	}
	LOG("javaClass");
	jmethodID javaCallback = (*env)->GetMethodID(env, javaClass, func, "(I)V");
	if (javaCallback == NULL) {
		LOG("Fail to find method %s", func);
		return;
	}
	LOG("javaCallback %s", func);
	(*env)->CallVoidMethod(env, gJavaObj, javaCallback, arg);
	
	if((*gJavaVM)->DetachCurrentThread(gJavaVM) != JNI_OK) {
		LOG("DetachCurrentThread %s", func);
		return ;
	}
}

void ntyCallJavaFuncStatus(const char *func, int status) {
	JNIEnv *env;
	if ((*gJavaVM)->AttachCurrentThread(gJavaVM, (void**)&env, NULL) != JNI_OK) {
		LOG(" ntyCallJavaFuncStatus failed %s\n", func);
		return ;
	}
	jclass javaClass = (*env)->GetObjectClass(env, gJavaObj);
	if (javaClass == NULL) {
		LOG("Fail to find javaClass");
		return ;
	}
	jmethodID javaCallback = (*env)->GetMethodID(env, javaClass, func, "(I)V");
	if (javaCallback == NULL) {
		LOG("Fail to find method %s", func);
		return ;
	}
	(*env)->CallVoidMethod(env, gJavaObj, javaCallback, status);

	if((*gJavaVM)->DetachCurrentThread(gJavaVM) != JNI_OK) {
		LOG("DetachCurrentThread %s", func);
		return ;
	}
}

void ntyCallJavaFuncParam(const char *func, U8 *json, int length) {
	JNIEnv *env;
	if ((*gJavaVM)->AttachCurrentThread(gJavaVM, (void**)&env, NULL) != JNI_OK) {
		LOG(" ntyCallJavaFuncParam failed %s\n", func);
		return ;
	}
	jclass javaClass = (*env)->GetObjectClass(env, gJavaObj);
	if (javaClass == NULL) {
		LOG("Fail to find javaClass");
		return ;
	}
	jmethodID javaCallback = (*env)->GetMethodID(env, javaClass, func, "([BI)V");
	if (javaCallback == NULL) {
		LOG("Fail to find method %s", func);
		return ;
	}

	jbyteArray RtnArr = NULL;
	RtnArr = (*env)->NewByteArray(env, length);
	(*env)->SetByteArrayRegion(env, RtnArr, 0, length, (jbyte*)json);

	(*env)->CallVoidMethod(env, gJavaObj, javaCallback, RtnArr, length);

	if((*gJavaVM)->DetachCurrentThread(gJavaVM) != JNI_OK) {
		LOG("DetachCurrentThread %s", func);
		return ;
	}
}

void ntyCallJavaFuncReturn(const char *func, DEVID fromId, U8 *json, int length) {
	JNIEnv *env;
	if ((*gJavaVM)->AttachCurrentThread(gJavaVM, (void**)&env, NULL) != JNI_OK) {
		LOG(" ntyCallJavaFuncParam failed %s\n", func);
		return ;
	}
	jclass javaClass = (*env)->GetObjectClass(env, gJavaObj);
	if (javaClass == NULL) {
		LOG("Fail to find javaClass");
		return ;
	}

	jmethodID javaCallback = (*env)->GetMethodID(env, javaClass, func, "(J[BI)V");
	if (javaCallback == NULL) {
		LOG("Fail to find method %s", func);
		return ;
	}

	jbyteArray RtnArr = NULL;
	RtnArr = (*env)->NewByteArray(env, length);
	(*env)->SetByteArrayRegion(env, RtnArr, 0, length, (jbyte*)json);

	(*env)->CallVoidMethod(env, gJavaObj, javaCallback, fromId, RtnArr, length);
	if((*gJavaVM)->DetachCurrentThread(gJavaVM) != JNI_OK) {
		LOG("DetachCurrentThread %s", func);
		return ;
	}
}


void ntyCallJavaFuncPacket(const char *func, DEVID fromId, DEVID gId, int length) {
	JNIEnv *env;
	if ((*gJavaVM)->AttachCurrentThread(gJavaVM, (void**)&env, NULL) != JNI_OK) {
		LOG(" ntyCallJavaFuncParam failed %s\n", func);
		return ;
	}
	jclass javaClass = (*env)->GetObjectClass(env, gJavaObj);
	if (javaClass == NULL) {
		LOG("Fail to find javaClass");
		return ;
	}
	jmethodID javaCallback = (*env)->GetMethodID(env, javaClass, func, "(JJI)V");
	if (javaCallback == NULL) {
		LOG("Fail to find method %s", func);
		return ;
	}


	(*env)->CallVoidMethod(env, gJavaObj, javaCallback, fromId, gId, length);
	if((*gJavaVM)->DetachCurrentThread(gJavaVM) != JNI_OK) {
		LOG("DetachCurrentThread %s", func);
		return ;
	}
}

void ntySendSuccess(int arg) {
	LOG("Success\n");
	ntyCallJavaFunction("onNativeSendSuccess", arg);
}


void ntySendFailed(int arg) {
	LOG("failed : %d\n", arg);
#if 0
	if (arg == STATUS_TIMEOUT)
		ntyCallJavaFunction("onNativeSendTimeout", arg);
	else if (arg == STATUS_NOEXIST)
		ntyCallJavaFunction("onNativeSendNoExist", arg);
#else
	ntyCallJavaFunction("onNativeSendFailed", arg);
#endif
}

void ntyUserRecvCb(int len) {
	LOG("ntyUserRecvCb : %d\n", len);
	ntyCallJavaFunction("onNativeUserRecvCallback", len);
}

void ntyHeartBeatAckResult(int status) {
	LOG(" ntyLoginAckResult:%d\n", status);
}


void ntyLogoutAckResult(int status) {
	LOG(" ntyLogoutAckResult:%d\n", status);
	ntyCallJavaFunction("ntyLogoutAckResult", status);
}

void ntyTimeAckResult(U8 *json, int length) {

}

void ntyICCIDAckResult(U8 *json, int length) {

}

void ntyCommonReqResult(DEVID fromId, U8 *json, int length) {
	LOG(" ntyCommonReqResult : %lld\n", fromId);
	LOG(" ntyCommonReqResult : %s\n", json);

	ntyCallJavaFuncReturn("ntyNativeCommonReqResult", fromId, json, length);
}

void ntyCommonAckResult(U8 *json, int length) {
	ntyCallJavaFuncParam("ntyNativeCommonAckResult", json, length);
}

void ntyPacketRecvResult(DEVID fromId, DEVID gId, int length) {
	ntyCallJavaFuncPacket("ntyNativePacketRecvResult", fromId, gId, length);
}


//1
void ntyLoginAckResult(U8 *json, int length) {
	LOG(" ntyLoginAckResult:%s\n", json);
	ntyCallJavaFuncParam("ntyNativeLoginAckResult", json, length);
}
//2
void ntyBindResult(int arg) {
	LOG("ntyBindResult\n");
	ntyCallJavaFuncStatus("ntyNativeBindResult", arg);
}
//3
void ntyUnBindResult(int arg) {
	LOG("ntyUnBindResult\n");
	ntyCallJavaFuncStatus("ntyNativeUnBindResult", arg);
}
//4
void ntyVoiceDataAckResult(int status) {
	//voice data send success
	LOG(" ntyVoiceDataAckResult: %d\n", status);
	ntyCallJavaFuncStatus("ntyNativeVoiceDataAckResult", status);
}
//5
void ntyOfflineMsgAckResult(U8 *json, int length) {
	LOG(" ntyOfflineMsgAckResult:%s\n", json);
	ntyCallJavaFuncParam("ntyNativeOfflineMsgAckResult", json, length);
}
//6
void ntyDataResult(U8 *json, int length) {
	LOG(" ntyDataResult:%d\n", length);
	ntyCallJavaFuncParam("ntyNativeDataResult", json, length);
}
//7
void ntyVoiceBroadCastResult(DEVID fromId, U8 *json, int length) {
	LOG(" ntyVoiceBroadCastResult:%s\n", json);

	ntyCallJavaFuncReturn("ntyNativeVoiceBroadCastResult", fromId, json, length);
}
//8
void ntyLocationBroadCastResult(DEVID fromId, U8 *json, int length) {
	LOG(" ntyLocationBroadCastResult:%s\n", json);

	ntyCallJavaFuncReturn("ntyNativeLocationBroadCastResult", fromId, json, length);
}
//9
void ntyCommonBroadCastResult(DEVID fromId, U8 *json, int length) {
	LOG(" ntyCommonBoradCastResult:%s\n", json);

	ntyCallJavaFuncReturn("ntyNativeCommonBoradCastResult", fromId, json, length);
}
//10
void ntyDisconnect(int arg) {
	LOG("ntyDisconnect %d\n", arg);
	ntyCallJavaFunction("onNativeDisconnect", arg);
}
//11
void ntyReconnected(int arg) {
	LOG("ntyReconnected %d\n", arg);
	ntyCallJavaFunction("onNativeReconnect", arg);
	//ntyCallJavaFunctionNoThread("onNativeReconnect", arg);
}


void ntyLocationPushResult(U8 *json, int length) {
	LOG(" ntyLocationPushResult:%s\n", json);

	ntyCallJavaFuncParam("ntyNativeLocationPushResult", json, length);
}

void ntyWeatherPushResult(U8 *json, int length) {
	LOG(" ntyWeatherPushResult:%s\n", json);

	ntyCallJavaFuncParam("ntyNativeWeatherPushResult", json, length);
}

void ntyDataRoute(DEVID fromId, U8 *json, int length) {
	LOG(" ntyDataRoute:%s\n", json);

	ntyCallJavaFuncParam("ntyNativeDataRoute", json, length);
}

void ntyBindConfirmResult(DEVID fromId, U8 *json, int length) {
	LOG(" ntyDataRoute:%s\n", json);

	ntyCallJavaFuncReturn("ntyBindConfirmResult", fromId, json, length);
}

//0
jbyteArray Java_com_wbj_ndk_natty_client_NattyClient_ntyGetVersion(JNIEnv *env, jobject thiz) {
	char *buffer = ntyProtoClientGetVersion();
	int length = strlen(buffer);

	jbyteArray rtnArr = (*env)->NewByteArray(env, length);
	(*env)->SetByteArrayRegion(env, rtnArr, 0, length, (jbyte*)buffer);

	return rtnArr;
}

//1
int Java_com_wbj_ndk_natty_client_NattyClient_ntyGetVoiceBufferSize(JNIEnv *env, jobject thiz) {
	return ntyGetRecvBigLength();
}

//2
jbyteArray Java_com_wbj_ndk_natty_client_NattyClient_ntyGetVoiceBuffer(JNIEnv *env, jobject thiz) {
	char *buffer = (char*)ntyGetRecvBigBuffer();
	jsize len = ntyGetRecvBigLength();

	jbyteArray rtnArr = (*env)->NewByteArray(env, len);
	(*env)->SetByteArrayRegion(env, rtnArr, 0, len, (jbyte*)buffer);

	return rtnArr;
}

//3
void Java_com_wbj_ndk_natty_client_NattyClient_ntySetSelfId(JNIEnv *env, jobject thiz, jlong aid) {
	DEVID AppId;
	memcpy(&AppId, &aid, sizeof(DEVID));

	ntySetDevId(AppId);

	LOG(" setNattyDevId : %lld ", AppId);

	return ;
}
//4
void Java_com_wbj_ndk_natty_client_NattyClient_ntyClientInitilize(JNIEnv *env, jobject thiz) {
	(*env)->GetJavaVM(env, &gJavaVM);
	gJavaObj = (*env)->NewGlobalRef(env, thiz);

	LOG(" ntyClientInitilize ");
	return ;
}

//5
void Java_com_wbj_ndk_natty_client_NattyClient_ntyBindClient(JNIEnv *env, jobject thiz, jlong DevID, jbyteArray json, int length) {

	DEVID id = 0;
	memcpy(&id, &DevID, sizeof(DEVID));

	jbyte *bData = (*env)->GetByteArrayElements(env, json, 0);

	LOG(" ntyBindClient : %lld ", id);
	ntyBindClient(id, bData, length);
}
//6
int Java_com_wbj_ndk_natty_client_NattyClient_ntyBindConfirmReqClient(JNIEnv *env, jobject thiz, jlong proposerId, jlong devId, int msgId, jbyteArray json, int length) {

	DEVID proposer = 0;
	DEVID deviceId = 0;

	memcpy(&proposer, &proposerId, sizeof(DEVID));
	memcpy(&deviceId, &devId, sizeof(DEVID));

	jbyte *bData = (*env)->GetByteArrayElements(env, json, 0);

	return ntyBindConfirmReqClient(proposer, deviceId, msgId, bData, length);
}

//7
void Java_com_wbj_ndk_natty_client_NattyClient_ntyUnBindClient(JNIEnv *env, jobject thiz, jlong DevID) {

	DEVID id = 0;
	memcpy(&id, &DevID, sizeof(DEVID));

	LOG(" ntyUnBindClient : %lld ", id);
	ntyUnBindClient(id);
}

//8
int Java_com_wbj_ndk_natty_client_NattyClient_ntyStartupClient(JNIEnv *env, jobject thiz) {
	int status = 0;

	ntySetProxyCallback(ntyUserRecvCb);
	ntySetSendFailedCallback(ntySendFailed);
	ntySetSendSuccessCallback(ntySendSuccess);
	ntySetProxyReconnect(ntyReconnected);
	ntySetProxyDisconnect(ntyDisconnect);
	ntySetBindResult(ntyBindResult);
	ntySetUnBindResult(ntyUnBindResult);
	
	ntySetLoginAckResult(ntyLoginAckResult);
	ntySetHeartBeatAckResult(ntyHeartBeatAckResult);
	ntySetLogoutAckResult(ntyLogoutAckResult);
#if (NTY_PROTO_SELFTYPE==NTY_PROTO_CLIENT_WATCH)
	ntySetTimeAckResult(ntyTimeAckResult);
	ntySetICCIDAckResult(ntyICCIDAckResult);
#endif
	ntySetCommonReqResult(ntyCommonReqResult);
	ntySetVoiceDataAckResult(ntyVoiceDataAckResult);
	ntySetOfflineMsgAckResult(ntyOfflineMsgAckResult);
	ntySetLocationPushResult(ntyLocationPushResult);
	ntySetWeatherPushResult(ntyWeatherPushResult);

	ntySetDataRoute(ntyDataRoute);
	ntySetDataResult(ntyDataResult);
	ntySetBindConfirmResult(ntyBindConfirmResult);
	ntySetPacketRecv(ntyPacketRecvResult);

	ntySetVoiceBroadCastResult(ntyVoiceBroadCastResult);
	ntySetLocationBroadCastResult(ntyLocationBroadCastResult);
	ntySetCommonBroadCastResult(ntyCommonBroadCastResult);


	ntyStartupClient(&status);
	if (status == -1) {
		ntyShutdownClient();
	}

	LOG(" ntyStartupClient %d", status);
	return status;
}
//9
int Java_com_wbj_ndk_natty_client_NattyClient_ntyShutdownClient(JNIEnv *env, jobject thiz) {

	ntyShutdownClient();
	
	return 0;
}
//10
int Java_com_wbj_ndk_natty_client_NattyClient_ntyCommonReqClient(JNIEnv *env, jobject thiz, jlong gId, jbyteArray json, int length) {
	DEVID groupId = 0;
	memcpy(&groupId, &gId, sizeof(DEVID));

	jbyte *data = (*env)->GetByteArrayElements(env, json, 0);
	return ntyCommonReqClient(groupId, data, length);
}
//11
int Java_com_wbj_ndk_natty_client_NattyClient_ntyDataRouteClient(JNIEnv *env, jobject thiz, jlong gId, jbyteArray json, int length) {
	DEVID groupId = 0;
	memcpy(&groupId, &gId, sizeof(DEVID));

	jbyte *data = (*env)->GetByteArrayElements(env, json, 0);
	return ntyDataRouteClient(groupId, data, length);
}
//12
int Java_com_wbj_ndk_natty_client_NattyClient_ntyVoiceDataReqClient(JNIEnv *env, jobject thiz, jlong gId, jbyteArray data, int length) {
	DEVID groupId = 0;
	memcpy(&groupId, &gId, sizeof(DEVID));
	jbyte *bData = (*env)->GetByteArrayElements(env, data, 0);

	LOG("groupId: %lld", groupId);
	return ntyVoiceDataReqClient(groupId, bData, length);
}

//13
int Java_com_wbj_ndk_natty_client_NattyClient_ntyMsgPushReqClient(JNIEnv *env, jobject thiz, jlong gId, jbyteArray data, int length) {
	DEVID groupId = 0;
	memcpy(&groupId, &gId, sizeof(DEVID));
	jbyte *bData = (*env)->GetByteArrayElements(env, data, 0);

	LOG("groupId: %lld", groupId);
	return ntyMsgPushReqClient(groupId, bData, length);
}






















