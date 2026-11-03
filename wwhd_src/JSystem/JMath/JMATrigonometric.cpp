/** HD: fixed-size JMath trigonometric lookup tables replace the dynamic GC sine table. */
#include "bindings.h"
#include "JSystem/JMath/JMATrigonometric.h"
static JMATrigStaticBlock sTrigStaticBlock;
static u32 sTrigStaticRegister;
static JMASinCosTable sSinCosTable;
static JMALookupTable sAtanTable;
static JMALookupTable sAsinTable;
/* 027ED4AC */
static void __sinit_JMATrigonometric_cpp() {
    sTrigStaticBlock.mWords[0] = 0;
    sTrigStaticBlock.mWords[1] = 0;
    sTrigStaticBlock.mWords[2] = 0;
    sTrigStaticBlock.mWords[3] = 0;
    JMATrigConstructRegister(&sTrigStaticRegister);
    JMATrigConstructArray(&sSinCosTable,8192,8,gabi::guest_fn((const void*)JMATrigPairConstructor));
    for (s32 i = 0; i < 8192; ++i) {
        f64 angle = (6.2831854820251465 * (f64)i) * 0.0001220703125;
        sSinCosTable.mEntries[i].mSin = (f32)JMATrigSin(angle);
        sSinCosTable.mEntries[i].mCos = (f32)JMATrigCos(angle);
    }
    for (s32 i = 0; i < 1024; ++i) sAtanTable.mEntries[i] = (f32)JMATrigAtan((f64)i * 0.0009765625);
    sAtanTable.mEntries[0] = 0.0f;
    sAtanTable.mEntries[1024] = 0.7853981852531433f;
    for (s32 i = 0; i < 1024; ++i) sAsinTable.mEntries[i] = (f32)JMATrigAsin((f64)i * 0.0009765625);
    sAsinTable.mEntries[0] = 0.0f;
    sAsinTable.mEntries[1024] = 0.7853981852531433f;
}
/* WWHD glue
 data sTrigStaticBlock 104A24E0
 data sTrigStaticRegister 101F9808
 data sSinCosTable 104A44F8
 data sAtanTable 104A24F0
 data sAsinTable 104A34F4
 ext 028F026C void JMATrigConstructRegister(void*)
 ext 027ED6A8 JMath::RandomPair* JMATrigPairConstructor(JMath::RandomPair*) callback
 ext 028EFFD0 void JMATrigConstructArray(void*,u32,u32,u32)
 ext 028F38F8 f64 JMATrigSin(f64)
 ext 028F3C78 f64 JMATrigCos(f64)
 ext 028F3CA0 f64 JMATrigAtan(f64)
 ext 028F3EC0 f64 JMATrigAsin(f64)
*/
