#include <jni.h>
#include <string>
#include "Test.h"

extern "C" {

JNIEXPORT jstring JNICALL
Java_com_jbloit_android_1testlibpd_1jni_MainActivity_stringFromJNI(
            JNIEnv *env,
            jobject /* this */) {
        std::string hello = "Hello from C++";
        return env->NewStringUTF(hello.c_str());
    }

JNIEXPORT jint JNICALL
Java_com_jbloit_android_1testlibpd_1jni_MainActivity_test(
        JNIEnv *env,
        jobject /* this */) {
        Test* test = new Test();
        int prop = test->getProp();
    return prop;
}

}
