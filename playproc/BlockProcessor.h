//
//  BlockProcessor.h
//  playproc
//
//  Created by Andrew Wheeler on 9/17/12.
//  Copyright (c) 2012 Andrew Wheeler. All rights reserved.
//

#ifndef __playproc__BlockProcessor__
#define __playproc__BlockProcessor__

#include <iostream>
#include <AudioToolbox/AudioToolbox.h>

class BlockProcessor {
    
public:
    
    OSStatus Init(short* pCoeffs, int nTaps);
    void Process(short* pInOut, int nSamples);
    
private:
    int mnTaps;
    short* mpCoeffs;
    short* mpState;
    short* mpState2;
};

class LowPassBlockProcessor : public BlockProcessor {
public:
    OSStatus Init();
};
#endif /* defined(__playproc__BlockProcessor__) */
