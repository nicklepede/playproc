//
//  AudioBuffer.cpp
//  playproc
//
//  Created by Andrew Wheeler on 9/15/12.
//  Copyright (c) 2012 Andrew Wheeler. All rights reserved.
//

#include "SimpleAudioPlayer.h"
#include <AudioToolbox/AudioFile.h>
#include <AudioToolbox/AudioQueue.h>
#include <AudioToolbox/AudioToolbox.h>

OSStatus SimpleAudioPlayer::Play(CFStringRef strFilename) {

    
    // Init the block processor (kinda sloppy)
    mProcessor.Init();
    
    OSStatus err;
   	CFURLRef urlFile = CFURLCreateWithFileSystemPath(kCFAllocatorDefault, strFilename, kCFURLPOSIXPathStyle, false);
    
	// Open the audio file
    err = AudioFileOpenURL(urlFile, kAudioFileReadPermission , 0, &mAudioFile);
	CFRelease(urlFile);
    if (err)
        return err;

    // Determine the needed size
    UInt32 size = sizeof(mDataFormat);
    memset(&mDataFormat, 0, size);

    err = AudioFileGetProperty(mAudioFile, kAudioFilePropertyDataFormat, &size, &mDataFormat);
    if(err)
        return err;

    // Make sure the wavefile is 16-bit MONO
    if (mDataFormat.mFormatID != kAudioFormatLinearPCM
        || mDataFormat.mChannelsPerFrame != 1
        || mDataFormat.mBitsPerChannel != 16
        || mDataFormat.mBytesPerPacket != 2)
        return -1;
    
    // Set the buffer size to one second
    bufferByteSize = mDataFormat.mBytesPerFrame*mDataFormat.mSampleRate * 0.5;
    mNumPacketsToRead = bufferByteSize /mDataFormat.mBytesPerPacket;
    
    // Create the Queue
    AudioQueueNewOutput ( &mDataFormat, SimpleAudioPlayer::HandleOutputBuffer, this, CFRunLoopGetCurrent (), kCFRunLoopCommonModes, 0, &mQueue );
    mCurrentPacket = 0;
    mIsRunning = true;
    
    // Allocate/Prime the buffers
    for (int i = 0; i < kNumberBuffers; ++i) {
        AudioQueueAllocateBuffer ( mQueue, bufferByteSize, &mBuffers[i] );
        
        HandleOutputBuffer ( this, mQueue, mBuffers[i] );
    }
 
    // Set the gain
    AudioQueueSetParameter ( mQueue, kAudioQueueParam_Volume, 1.0 );

    // Play!
    AudioQueueStart ( mQueue, NULL );
    do {
        CFRunLoopRunInMode ( kCFRunLoopDefaultMode, 0.25, false );
    } while (mIsRunning);
    CFRunLoopRunInMode ( kCFRunLoopDefaultMode, 1, false );
    
    AudioQueueDispose ( mQueue, true );
    AudioFileClose (mAudioFile);
    
    return 0;
}

void SimpleAudioPlayer::Process(void* pData, int nSamples)
{
#if 1
    mProcessor.Process((short*) pData, nSamples);
#else
    short* pSamples = (short*) pData;
    short gain = 0x4000;
    for (int i = 0; i < nSamples; i++) {
        short x = pSamples[i];
        long val = x * gain;
        short y = val >> 15;
        pSamples[i] = y;
    }
#endif
}
void  SimpleAudioPlayer::HandleOutputBuffer (
                                           void                *aqData,
                                           AudioQueueRef       inAQ,
                                           AudioQueueBufferRef inBuffer
                                           ) {
    SimpleAudioPlayer *pAqData = (SimpleAudioPlayer *) aqData;
    if (pAqData->mIsRunning == 0) return;
    UInt32 numBytesReadFromFile;
    UInt32 numPackets = pAqData->mNumPacketsToRead;
    AudioFileReadPackets (
                          pAqData->mAudioFile,
                          false,
                          &numBytesReadFromFile,
                          NULL,
                          pAqData->mCurrentPacket,
                          &numPackets,
                          inBuffer->mAudioData
                          );
    if (numPackets > 0) {
        
        pAqData->Process(inBuffer->mAudioData, numPackets);
        
        
        inBuffer->mAudioDataByteSize = numBytesReadFromFile;
        AudioQueueEnqueueBuffer ( pAqData->mQueue, inBuffer, 0, NULL );
        pAqData->mCurrentPacket += numPackets;
    } else {
        AudioQueueStop (
                        pAqData->mQueue,
                        false
                        );
        pAqData->mIsRunning = false;
    }
}

