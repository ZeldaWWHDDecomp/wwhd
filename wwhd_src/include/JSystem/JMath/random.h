/** JMath random generators. HD retains the GameCube state layout. */
#pragma once
#include "wwhd.h"
namespace JMath {
struct TRandom_fast_ { be<u32> value; };
struct TRandom_enough_ {
    void setSeed(u32 value);
    f32 get();
    be<u32> mSeed;
    be<u32> mValue;
    be<u32> mShuffle[32];
};
struct RandomStaticBlock { be<u32> mWords[4]; };
struct RandomPair { be<u32> mValues[2]; };
}
WWHD_OFFSET(JMath::TRandom_fast_,value,0);
WWHD_SIZE(JMath::TRandom_fast_,4);
WWHD_OFFSET(JMath::TRandom_enough_,mSeed,0);
WWHD_OFFSET(JMath::TRandom_enough_,mValue,4);
WWHD_OFFSET(JMath::TRandom_enough_,mShuffle,8);
WWHD_SIZE(JMath::TRandom_enough_,0x88);
WWHD_SIZE(JMath::RandomPair,8);
WWHD_SIZE(JMath::RandomStaticBlock,16);

Pair32 RandomUnsignedRemainder(u32, u32, u32, u32);
void RandomConstructRegister(void*);
