#pragma once
#include "wwhd.h"
#include "JSystem/JMath/random.h"
struct JMATrigStaticBlock { be<u32> mWords[4]; };
struct JMASinCosPair { be<f32> mSin; be<f32> mCos; };
WWHD_SIZE(JMATrigStaticBlock,16);
WWHD_SIZE(JMASinCosPair,8);
void JMATrigConstructRegister(void*);
JMath::RandomPair* JMATrigPairConstructor(JMath::RandomPair*);
void JMATrigConstructArray(void*,u32,u32,u32);
f64 JMATrigSin(f64);
f64 JMATrigCos(f64);
f64 JMATrigAtan(f64);
f64 JMATrigAsin(f64);

struct JMASinCosTable { JMASinCosPair mEntries[8192]; };
struct JMALookupTable { be<f32> mEntries[1025]; };
WWHD_SIZE(JMASinCosTable,65536);
WWHD_SIZE(JMALookupTable,4100);
// The sine/cosine table defined in JMATrigonometric.cpp (104A44F8), shared with JMAEulerToQuat.
extern JMASinCosTable& sSinCosTable;
