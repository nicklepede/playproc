//
//  AudioBuffer.h
//  playproc
//
//  Created by Andrew Wheeler on 9/15/12.
//  Copyright (c) 2012 Andrew Wheeler. All rights reserved.
//

#ifndef __playproc__AudioBuffer__
#define __playproc__AudioBuffer__

#include <iostream>
#include <AudioToolbox/AudioToolbox.h>
#include "BlockProcessor.h"

class SimpleAudioPlayer
{
public:
    OSStatus Play(const CFStringRef szFilename);

private:
    void Process(void* pData, int nSamples);
    static void HandleOutputBuffer ( void *aqData, AudioQueueRef inAQ, AudioQueueBufferRef inBuffer );
    static const int kNumberBuffers = 3;
    AudioStreamBasicDescription   mDataFormat;
    AudioQueueRef                 mQueue;
    AudioQueueBufferRef           mBuffers[kNumberBuffers];
    AudioFileID                   mAudioFile;
    UInt32                        bufferByteSize;
    SInt64                        mCurrentPacket;
    UInt32                        mNumPacketsToRead;
    bool                          mIsRunning;
    LowPassBlockProcessor         mProcessor;
};
#endif /* defined(__playproc__AudioBuffer__) */
