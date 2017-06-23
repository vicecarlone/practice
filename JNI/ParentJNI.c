#include <jni.h>
#include <stdio.h>
#include "ParentJNI.h"

JNIEXPORT vpod JNICALL Java_ParentJNI_sayHello(JNIEnv *env, jobject thisObj){
	printf("Hello World!\n");
	return;
}