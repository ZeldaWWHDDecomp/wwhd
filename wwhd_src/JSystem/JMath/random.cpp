/** JMath shuffled random generator; private decomp source. */
#include "bindings.h"
#include "JSystem/JMath/random.h"
static JMath::RandomStaticBlock sRandomStaticBlock;
static u32 sRandomStaticRegister;

/* 027ED6A8 */
static JMath::RandomPair* RandomPair__ct(JMath::RandomPair* object) {
    if (object == NULL) object = (JMath::RandomPair*)operator_new(sizeof(JMath::RandomPair));
    if (object != NULL) {
        object->mValues[1] = 0;
        object->mValues[0] = 0;
    }
    return object;
}

/* 027ED6EC */
static JMath::TRandom_fast_* TRandom_fast__ct(JMath::TRandom_fast_* object, u32 value) {
    if (object == NULL) object = (JMath::TRandom_fast_*)operator_new(sizeof(JMath::TRandom_fast_));
    if (object != NULL) object->value = value;
    return object;
}

/* 027ED730 */
void JMath::TRandom_enough_::setSeed(u32 value) {
    mValue = 0;
    u32 seed = value + 1;
    if (seed == 0) seed = 1;
    for (u32 i = 8; i != 0; --i) {
        u64 product = (u64)seed * 16807;
        seed = RandomUnsignedRemainder((u32)(product >> 32), (u32)product, 0, 0x7fffffff).r4;
    }
    for (u32 i = 32; i != 0;) {
        --i;
        u64 product = (u64)seed * 16807;
        seed = RandomUnsignedRemainder((u32)(product >> 32), (u32)product, 0, 0x7fffffff).r4;
        mSeed = seed;
        mShuffle[i] = seed;
        if (i != 0) seed = mSeed;
    }
}

/* 027ED7E0 */
f32 JMath::TRandom_enough_::get() {
    u64 product = (u64)(u32)mSeed * 16807;
    u32 seed = RandomUnsignedRemainder((u32)(product >> 32), (u32)product, 0, 0x7fffffff).r4;
    u32 index = ((u32)mValue >> 24) & 0xfc;
    mSeed = seed;
    mValue = mShuffle[index / 4];
    mShuffle[index / 4] = seed;
    f32 result = (f32)(u32)mValue * 4.656612873077393e-10f;
    return result <= 0.9999998807907104f ? result : 0.9999998807907104f;
}

/* 027ED884 */
static void __sinit_random_cpp() {
    sRandomStaticBlock.mWords[2] = 0;
    sRandomStaticBlock.mWords[0] = 0;
    sRandomStaticBlock.mWords[3] = 0;
    sRandomStaticBlock.mWords[1] = 0;
    RandomConstructRegister(&sRandomStaticRegister);
}

/* WWHD glue
 data sRandomStaticBlock 104B44F8
 data sRandomStaticRegister 101F9814
 ext 028F597C Pair32 RandomUnsignedRemainder(u32, u32, u32, u32)
 ext 028F026C void RandomConstructRegister(void*)
*/
