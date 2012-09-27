//
//  BlockProcessor.cpp
//  playproc
//
//  Created by Andrew Wheeler on 9/17/12.
//  Copyright (c) 2012 Andrew Wheeler. All rights reserved.
//

#include "BlockProcessor.h"

#include "/System/Library/Frameworks/CoreServices.framework/Frameworks/CarbonCore.framework/Headers/MacErrors.h"

OSStatus BlockProcessor::Init(short* pCoeffs, int nTaps) {
 
    mnTaps = nTaps;
    // Setup the coefficients
    mpCoeffs = new short[mnTaps];
    if (!mpCoeffs)
        return -1;
    for (int i = 0; i < mnTaps; i++) {
        mpCoeffs[i] = pCoeffs[i];
    }
    
    // Setup state buffer. It's ~2*nTaps so we can process data in this statue buffer.
    mpState = new short[mnTaps-1];
    if (!mpState)
        return -1;
    for (int i = 0; i < mnTaps; i++)
        mpState[i] = 0;

    // Setup somewhat hacky 2nd state buffer.  We're going to try to get rid of this.
    mpState2 = new short[mnTaps-1];
    if (!mpState2)
        return -1;
    for (int i = 0; i < mnTaps; i++)
        mpState2[i] = 0;
    
    return 0;
}

OSStatus LowPassBlockProcessor::Init() {
  
 //   short taps[8] = { 0x1000, 0x1000, 0x1000, 0x1000, 0x1000, 0x1000, 0x1000, 0x1000, };
    short taps[8] = { 0, 0x00, 0x00,0x0 , 0x0 , 0x0,  0x0, 0x1fff };
    return BlockProcessor::Init(taps, 8);
}
    

void BlockProcessor::Process(short* pInOut, int nSamples) {

    // Buffer should be much larger than the filter.
    assert(nSamples > mnTaps);
    // Implementation requires filter length to be 3 or larger
    assert( mnTaps > 2);
    
    // Save the last taps-1 samples for next time
    for (int i = 0; i < mnTaps-1; i++) {
        mpState2[i] = pInOut[nSamples - (mnTaps-1) + i];
    }
    
    // We're doing in-place processing, so process the
    // data backwards.  This assures x[n] and older
    // is preserved when we're working on y[n].
   
    // Process newest (nSamples - (mnTaps-1) ) samples.  We'll do
    // the last mnTaps-1 samples below.
    for (int i = nSamples-1; i >= mnTaps-1; i--) {
        long accum = 0;
        for (int j = 0; j < mnTaps; j++) {
            short x = pInOut[i - j];
            accum += mpCoeffs[j] * x /*pInOut[i - j]*/;
        }
        short y = accum >> 15;
        pInOut[i] = y /*accum >> 15*/;
    }
    
    long accum = 0;
    // Process the first mnTaps-1 samples using the previous mnTaps-1 samples
    for (int i = mnTaps-2; i >= 0; i--)  {
        int coeff=0;
        for (int j = i; j >= 0; j--)    {
            accum += mpCoeffs[coeff++] * pInOut[j];
        }
        for (int k = mnTaps-2; k >= i; k--) {
            accum += mpCoeffs[coeff++] * mpState[k];
        }
        pInOut[i] = accum >> 15;
    }
#if 0
    // The rest of the samples
    for (int i = mnTaps; i < nSamples; i++) {
        accum = 0;
        for (int j = 0; j < mnTaps; j++)
            accum += mpCoeffs[j] * pInOut[mnTaps + j];
        pInOut[i] = accum >> 15;
    }
#endif
#if 0
    // Save the last taps-1 samples for next time
    for (int i = 0; i < mnTaps-1; i++) {
        mpState[i] = pInOut[nSamples - (mnTaps-1) + i];
    }
#else
    // Save the last taps-1 samples for next time
    for (int i = 0; i < mnTaps-1; i++) {
        *mpState++ = *mpState2++;
    }
#endif
}