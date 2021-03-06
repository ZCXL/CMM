/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class cmminterface_CMMInterface */

#ifndef _Included_cmminterface_CMMInterface
#define _Included_cmminterface_CMMInterface
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     cmminterface_CMMInterface
 * Method:    compile
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_cmminterface_CMMInterface_compile
  (JNIEnv * e, jclass jc, jstring path);

/*
 * Class:     cmminterface_CMMInterface
 * Method:    run
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_cmminterface_CMMInterface_run
  (JNIEnv * e, jclass jc, jstring path);

/*
 * Class:     cmminterface_CMMInterface
 * Method:    getResult
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_cmminterface_CMMInterface_getResult
  (JNIEnv * e, jclass jc);

/*
 * Class:     cmminterface_CMMInterface
 * Method:    getState
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_cmminterface_CMMInterface_getState
  (JNIEnv * e, jclass jc);

#ifdef __cplusplus
}
#endif
#endif
