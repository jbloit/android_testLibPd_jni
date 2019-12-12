//
// Created by julien@macmini on 2019-12-09.
//

#ifndef ANDROID_TESTLIBPD_JNI_TEST_H
#define ANDROID_TESTLIBPD_JNI_TEST_H

#include "libpd/libpd_wrapper/z_libpd.h"
#include "libpd/cpp/PdBase.hpp"


class Test {
public:
    Test();
    ~Test();
    int getProp();
private:
    int prop;
};


#endif //ANDROID_TESTLIBPD_JNI_TEST_H
