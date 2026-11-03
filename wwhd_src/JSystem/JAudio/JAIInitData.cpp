/* Based on CC0 tww JAIInitData.cpp; HD drops the obsolete HED warning and temporary-file cleanup. */
#include "JSystem/JAudio/JAIInitData.h"
using InitWord = be<u32>;
static InitWord* aafPointer;
static JAInter::BankWave::initOnCode_s* initOnCodeBnk;
static JAInter::BankWave::initOnCode_s* initOnCodeWs;
static s32 wsMax;
using InitDataPointer = gptr<void>;
static InitDataPointer* initOnCodeStrm;
static void* streamList;
static void* initOnCodeFxScene;
static JAIBasic* msBasic;
static JKRHeap* sSystemHeap;
static AudioHeaderStatics initHeaderStatics;
static AudioDestructorRecord initDestructorRecord;
namespace JAInter { namespace InitData {
/* 02804814 */
void checkInitDataOnMemory() {
    u32 index = 0;
    for (;;) {
        InitWord* base = aafPointer;
        u32 kind = base[index++];
        if (!kind) break;
        switch (kind) {
        case 1: {
            u32 offset = base[index++];
            u32 size = base[index];
            index += 2;
            be<u8>* copy = transInitDataFile((be<u8>*)base + offset, size);
            initAudioSoundTable(copy, size);
            break;
        }
        case 2: {
            u8 entry = 0;
            u32 words = 0;
            while (base[index + words]) words += 3;
            be<u8>* copy = transInitDataFile((be<u8>*)(base + index), words / 3 * 12 + 4);
            base = aafPointer;
            initOnCodeBnk = (BankWave::initOnCode_s*)copy;
            if (base[index]) {
                base = aafPointer;
                do {
                    BankWave::initOnCode_s* table = initOnCodeBnk;
                    u32 relative = gabi::ea((u8*)table[entry].data);
                    table[entry].data = (u8*)base + relative;
                    base = aafPointer;
                    index += 3;
                    ++entry;
                } while (base[index]);
            }
            ++index;
            break;
        }
        case 3: {
            u8 entry = 0;
            u32 words = 0;
            while (base[index + words]) words += 3;
            be<u8>* copy = transInitDataFile((be<u8>*)(base + index), words / 3 * 12 + 4);
            base = aafPointer;
            initOnCodeWs = (BankWave::initOnCode_s*)copy;
            if (base[index]) {
                base = aafPointer;
                do {
                    BankWave::initOnCode_s* table = initOnCodeWs;
                    u32 relative = gabi::ea((u8*)table[entry].data);
                    table[entry].data = (u8*)base + relative;
                    ++wsMax;
                    base = aafPointer;
                    index += 3;
                    ++entry;
                } while (base[index]);
            }
            ++index;
            break;
        }
        case 4: index += 3; break;
        case 5: {
            u32 dataIndex = index;
            InitDataPointer* copy = (InitDataPointer*)transInitDataFile((be<u8>*)(base + index), 8);
            base = aafPointer;
            initOnCodeStrm = copy;
            u32 offset = base[dataIndex];
            u32 size = base[dataIndex + 1];
            void* data = transInitDataFile((be<u8>*)base + offset, size);
            copy[0] = data;
            InitDataPointer* list = initOnCodeStrm;
            streamList = list[0];
            index += 3;
            break;
        }
        case 6: {
            u32 offset = base[index];
            u32 size = base[index + 1];
            InitWord* scenes = (InitWord*)transInitDataFile((be<u8>*)base + offset, size);
            setAudioSceneMax(scenes[0]);
            msBasic->mSceneTable = scenes + 1;
            for (u32 scene = 0; scene < getAudioSceneMax(); ++scene) {
                auto* pointers = (InitDataPointer*)(void*)msBasic->mSceneTable;
                u32 relative = gabi::ea((void*)pointers[scene]);
                pointers[scene] = (u8*)scenes + relative;
            }
            index += 3;
            break;
        }
        case 7: {
            u32 offset = base[index];
            u32 size = base[index + 1];
            void* copy = transInitDataFile((be<u8>*)base + offset, size);
            index += 3;
            initOnCodeFxScene = copy;
            break;
        }
        case 8: {
            u32 offset = base[index];
            u32 size = base[index + 1];
            JAIBasic* basic = msBasic;
            void* copy = transInitDataFile((be<u8>*)base + offset, (size & 0xFFF0) + 16);
            basic->mInitData = copy;
            index += 3;
            break;
        }
        default: while (base[index++]) {} break;
        }
    }
    BankWave::init();
}
/* 02804C38 */
u32 checkInitDataFile() {
    be<u8>* filename = getAudioInitFilename();
    if (!audioInitFileExists(filename)) {
        BasicPathHeapView* heap = (BasicPathHeapView*)(JKRHeap*)sSystemHeap;
        be<u8>* path = getAudioResourcePath();
        u32 prefixLength = 0;
        while (path[prefixLength]) ++prefixLength;
        filename = getAudioInitFilename();
        u32 nameLength = 0;
        while (filename[nameLength]) ++nameLength;
        u32 target = heap->vtable->alloc;
        void* buffer = gabi::call_ptr<void*>(target, heap, prefixLength + nameLength + 1, 4);
        path = getAudioResourcePath();
        filename = getAudioInitFilename();
        formatBasicPath(buffer, "%s%s%c", path, filename, 0);
        setAudioInitFilename(buffer);
        filename = getAudioInitFilename();
        if (!audioInitFileExists(filename)) return 0;
    }
    filename = getAudioInitFilename();
    loadTmpDVDFile((const char*)filename, (gptr<void>*)&aafPointer);
    if (!aafPointer) return 0;
    checkInitDataOnMemory();
    return 1;
}
}}
/* 02804D64 */
void __sinit_JAIInitData_cpp() {
    initHeaderStatics.mWords[2] = 0;
    initHeaderStatics.mWords[0] = 0;
    initHeaderStatics.mWords[3] = 0;
    initHeaderStatics.mWords[1] = 0;
    registerInitDataStatics(&initDestructorRecord);
}
using namespace JAInter::InitData;
/* WWHD glue
data aafPointer 101F9C68
data initOnCodeBnk 101F9B34
data initOnCodeWs 101F9B38
data wsMax 101F9B40
data initOnCodeStrm 101F9CF0
data streamList 104B5130
data initOnCodeFxScene 101F9BA8
data msBasic 104B4F14
data sSystemHeap 101F9DA8
data initHeaderStatics 104B4FDC
data initDestructorRecord 101F9C5C
str 1016F684 "%s%s%c"
ext 0280DCE0 void initAudioSoundTable(be<u8>*, u32)
ext 02804460 void setAudioSceneMax(u32)
ext 028046B8 u32 getAudioSceneMax()
ext 02804778 be<u8>* getAudioInitFilename()
ext 0280479C be<u8>* getAudioResourcePath()
ext 0280E13C u32 audioInitFileExists(be<u8>*)
ext 02804614 void setAudioInitFilename(void*)
ext 028F026C void registerInitDataStatics(AudioDestructorRecord*)
*/
