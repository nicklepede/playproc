//
//  main.cpp
//  playproc
//
//  Created by Andrew Wheeler on 9/15/12.
//  Copyright (c) 2012 Andrew Wheeler. All rights reserved.
//

#include <iostream>
#include "SimpleAudioPlayer.h"

#define kTestWavefilepath CFSTR("/Users/thf876/dev/dsper/playproc/playproc/test.wav")

int main(int argc, const char * argv[])
{

    OSStatus ret;
    // insert code here...
    SimpleAudioPlayer buff;
    ret = buff.Play(kTestWavefilepath);
    if (!ret) {
        std::cout << "Loaded test file...";
    }
    else {
        std::cout << "Error Loading file" << ret;
    }
    
    return 0;
}

