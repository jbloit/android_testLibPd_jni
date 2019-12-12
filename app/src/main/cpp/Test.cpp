//
// Created by julien@macmini on 2019-12-09.
//

#include "Test.h"
#include <iostream>

extern "C" {
int libpd_init();
}

Test::Test(){
    prop = 12;
    int a = libpd_init();
    std::cout << " " << a << " \n";
    std::cout << " " << a << " \n";
}

Test::~Test(){
}

int Test::getProp(){
    return  prop;
}