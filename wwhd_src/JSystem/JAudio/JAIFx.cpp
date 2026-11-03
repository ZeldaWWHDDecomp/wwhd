/* Based on the CC0 tww JAudio 1 reference; WWHD owns the observed layout and behavior. */
#include "JSystem/JAudio/JAIFx.h"
// HD: accessor indices retain the full incoming register width.
namespace JAInter { namespace Fx {
using FxBufferSize = be<u32>;
using FxBufferPointer = gptr<s16>;
using FxScenePointer = gptr<void>;
static FxBufferSize* mBufferSizeMax;
static FxBufferPointer* mBufferPointer;
static FxScenePointer* mFxconfigTable;
static u8 mSceneMax;
static initOnCodeFxScene_s* initOnCodeFxScene;
static JKRHeap* msCurrentHeap;
}}
using namespace JAInter::Fx;
static AudioHeaderStatics fxHeaderStatics;
static AudioDestructorRecord fxDestructorRecord;
namespace JAInter { namespace Fx {
/* 0280417C */
u32 getBufferSizeMax(u32 line) { return mBufferSizeMax[line]; }
/* 02804190 */
void setSceneMax(u8 count) { mSceneMax = count; }
/* 0280419C */
void setBufferMax(u32 a, u32 b, u32 c, u32 d) {
    mBufferSizeMax[0] = a;
    mBufferSizeMax[1] = b;
    mBufferSizeMax[2] = c;
    mBufferSizeMax[3] = d;
}
/* 028041C4 */
void setTablePointer(gptr<void>* table) { mFxconfigTable = table; }
/* 028041D0 */
void setScenePointer(u32 scene, void* config) { mFxconfigTable[scene] = config; }
/* 028041E4 */
void setBufferPointer(u32 line, s16* buffer) { mBufferPointer[line] = buffer; }
/* 028041F8 */
s16* getBufferPointer(u32 line) { return mBufferPointer[line]; }
/* 0280420C */
void init() {
    if (!initOnCodeFxScene) return;
    JKRHeap* heap = msCurrentHeap;
    if (!heap) {
        fxAssert("JAIBasic.h", 169, "msCurrentHeap");
        heap = msCurrentHeap;
    }
    mBufferSizeMax = (be<u32>*)allocateFxArray(16, heap, 4);
    heap = msCurrentHeap;
    if (!heap) {
        fxAssert("JAIBasic.h", 169, "msCurrentHeap");
        heap = msCurrentHeap;
    }
    gptr<s16>* buffers = (gptr<s16>*)allocateFxArray(16, heap, 4);
    initOnCodeFxScene_s* config = initOnCodeFxScene;
    mBufferPointer = buffers;
    setSceneMax((u8)config->sceneCount);
    setBufferMax(config->bufferSizes[0], config->bufferSizes[1],
                 config->bufferSizes[2], config->bufferSizes[3]);
    heap = msCurrentHeap;
    if (!heap) {
        fxAssert("JAIBasic.h", 169, "msCurrentHeap");
        heap = msCurrentHeap;
    }
    gptr<void>* table = (gptr<void>*)allocateFxArray((u32)mSceneMax * 4, heap, 32);
    if (!table) fxAssert("JAIFx.cpp", 46, "_heap");
    setTablePointer(table);
    for (u8 scene = 0; scene < mSceneMax; ++scene) {
        initOnCodeFxScene_s* base = initOnCodeFxScene;
        u32 offset = config->sceneOffsets[scene];
        setScenePointer(scene, (u8*)base + offset);
    }
    for (u32 line = 0; line < 4; ++line) {
        u32 size = getBufferSizeMax(line);
        if (!size) continue;
        heap = msCurrentHeap;
        if (!heap) {
            fxAssert("JAIBasic.h", 169, "msCurrentHeap");
            heap = msCurrentHeap;
            size = getBufferSizeMax(line);
        }
        s16* buffer = (s16*)allocateFxArray((size * 160u) & ~1u, heap, 32);
        if (!buffer) fxAssert("JAIFx.cpp", 57, "_buf");
        setBufferPointer(line, buffer);
        buffer = getBufferPointer(line);
        void* configs = mFxconfigTable[0];
        setFxLine(line, buffer, (u8*)configs + line * 32);
    }
}
}}
/* 0280442C */
void __sinit_JAIFx_cpp() {
    fxHeaderStatics.mWords[2] = 0;
    fxHeaderStatics.mWords[0] = 0;
    fxHeaderStatics.mWords[3] = 0;
    fxHeaderStatics.mWords[1] = 0;
    registerFxStatics(&fxDestructorRecord);
}
/* WWHD glue
data initOnCodeFxScene 101F9BA8
data msCurrentHeap 101F9B54
str 1016F5F8 "JAIBasic.h"
str 1016F5DC "msCurrentHeap"
str 1016F5EC "JAIFx.cpp"
str 1016F5CC "_heap"
str 1016F5D4 "_buf"
ext 0273AA24 void fxAssert(const char*, u32, const char*)
ext 0273B0D4 void* allocateFxArray(u32, JKRHeap*, s32)
ext 02816254 void setFxLine(u32, s16*, void*)
data mBufferSizeMax 104B4FBC
data mBufferPointer 104B4FC0
data mFxconfigTable 104B4FC4
data mSceneMax 104B4FC8
data fxHeaderStatics 104B4FAC
data fxDestructorRecord 101F9B9C
ext 028F026C void registerFxStatics(AudioDestructorRecord*)
*/
