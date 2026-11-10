/* Based on CC0 tww JAIConst.cpp; HD uses the file-device adapter. */
#include "JSystem/JAudio/JAIConst.h"
static JKRHeap* msCurrentHeap;
static JKRHeap* sSystemHeap;
static void* audioFileDevice;
static AudioHeaderStatics constHeaderStatics;
static AudioDestructorRecord constDestructorRecord;
static JAInter::Camera nullCamera;
static JAInter::Actor nullActor;
struct AudioMatrix { be<f32> entries[12]; };
static AudioMatrix camMtx;
static cXyz camTrans;
static cXyz camPreTrans;
static u32 audioRandomSeed;
namespace JAInter {
/* 02803C78 */
be<u8>* transInitDataFile(be<u8>* source, u32 size) {
    JKRHeap* heap = msCurrentHeap;
    if (!heap) { constHeapAssert("JAIBasic.h", 169, "msCurrentHeap"); heap = msCurrentHeap; }
    be<u8>* result = (be<u8>*)allocateAudioInitBytes(size, heap, 64);
    if (result) for (u32 i = 0; i < size; ++i) result[i] = source[i];
    return result;
}
/* 02803D14 */
void loadTmpDVDFile(const char* filename, gptr<void>* output) {
    u32 size = checkAudioFile(audioFileDevice, filename);
    if (!size) { *output = nullptr; return; }
    void* destination = allocateAudioInitBytes(size, sSystemHeap, 64);
    *output = destination;
    if (!loadAudioFile(audioFileDevice, filename, destination)) *output = nullptr;
}
/* 02803D9C */
u32 routeToTrack(u32 route) {
    u32 type = route & 0xF0000000;
    if (!type) return 0;
    if (type == 0x10000000) return route & 15;
    if (type == 0x20000000) return route & 255;
    return 0;
}
}
/* 02803DDC */
void __sinit_JAIConst_cpp() {
    constHeaderStatics.mWords[3] = 0;
    constHeaderStatics.mWords[2] = 0;
    constHeaderStatics.mWords[1] = 0;
    constHeaderStatics.mWords[0] = 0;
    registerConstStatics(&constDestructorRecord);
    nullCamera.position = &camTrans;
    nullCamera.previousPosition = &camPreTrans;
    nullCamera.matrix = &camMtx;
    nullActor.mPosition = nullptr;
    nullActor.field_0x8 = nullptr;
    nullActor.mMapInfo = 0;
    nullActor.field_0x0 = nullptr;
    constructAudioRandom((be<u32>*)&audioRandomSeed, 0);
}
using namespace JAInter;
/* WWHD glue
data msCurrentHeap 101F9B54
data sSystemHeap 101F9DA8
data audioFileDevice 101F9D58
data constHeaderStatics 104B4F18
data constDestructorRecord 101F9B58
data nullCamera 104B4F68
data nullActor 104B4F28
data camMtx 104B4F38
data camTrans 104B4F74
data camPreTrans 104B4F80
data audioRandomSeed 104B4F8C
str 1016F564 "JAIBasic.h"
str 1016F554 "msCurrentHeap"
ext 0273AA24 void constHeapAssert(const char*, u32, const char*)
ext 0273B0D4 void* allocateAudioInitBytes(u32, JKRHeap*, s32)
ext 0280EF40 u32 checkAudioFile(void*, const char*)
ext 0280EEB8 u32 loadAudioFile(void*, const char*, void*)
ext 027ED6EC void constructAudioRandom(be<u32>*, u32)
ext 028F026C void registerConstStatics(AudioDestructorRecord*)
*/
