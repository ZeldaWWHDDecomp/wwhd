/* JAudio 1 interface with the WWHD extension fields. */
#pragma once
#include "JSystem/JAudio/JAIAnimation.h"
struct BasicVtableRecord {
    union {
        be<u32> entries[20];
        struct { be<u32> reserved0[5],makeSound,reserved18[5],getMapInfoFx,reserved30,setSeExtParameter,reserved38[6]; };
    };
};
WWHD_OFFSET(BasicVtableRecord,makeSound,0x14);
WWHD_OFFSET(BasicVtableRecord,getMapInfoFx,0x2C);
WWHD_OFFSET(BasicVtableRecord,setSeExtParameter,0x34);
WWHD_SIZE(BasicVtableRecord,0x50);
namespace JAInter {
struct Camera {
    gptr<cXyz> position;
    gptr<cXyz> previousPosition;
    gptr<void> matrix;
    void init();
};
WWHD_OFFSET(Camera, matrix, 8);
WWHD_SIZE(Camera, 12);
}
struct JAISoundInfoView {
    be<u32> mFlag;
    be<u8> priority;
    be<u8> fixedSequenceSlot;
    be<u16> mOffsetNo;
    be<f32> pitch;
    be<u8> volume;
    be<u8> fxmix;
    u8 padding[2];
};
WWHD_OFFSET(JAISoundInfoView, mOffsetNo, 6);
WWHD_SIZE(JAISoundInfoView, 16);
struct JAIFrameCallbackVtable {
    be<u32> reserved[5];
    be<u32> prepare;
    be<u32> reservedPrepare;
    be<u32> start;
    be<u32> reservedStart;
    be<u32> frame;
    be<u32> reservedFrame;
    be<u32> remapID;
};
struct JAIFrameCallback { gptr<JAIFrameCallbackVtable> vtable; };
class JAIBasic {
public:
    JAIBasic();
    void startSoundBasic(u32, gptr<JAISound>*, JAInter::Actor*, u32, u32, JAISoundInfoView*);
    void prepareSoundVec(u32, gptr<JAISound>*, cXyz*, u32, u32, u32);
    void stopActorSoundOneBuffer(void*, JAISound*);
    void stopIDSoundOneBuffer(u32, JAISound*);
    void stopIDActorSoundOneBuffer(u32, void*, JAISound*);
    void stopSoundHandle(JAISound*, u32);
    void stopAllSound(void*);
    void stopAllSound(u32);
    void stopAllSound(u32, void*);
    void startSoundDirectID(u32, gptr<JAISound>*, JAInter::Actor*, u32, u32);
    void startSoundActor(u32, gptr<JAISound>*, JAInter::Actor*, u32, u32);
    void startSoundVec(u32, gptr<JAISound>*, cXyz*, u32, u32, u32);
    JAISound* makeSound(u32 count);
    void deleteObject(void*);
    void processFrameWork();
    void setCameraInfo(cXyz*, cXyz*, void*, u32);
    u16 getSoundOffsetNumberFromID(u32);
    u32 getMapInfoFxline(u32);
    u32 getMapInfoGround(u32);
    f32 getMapInfoFxParameter(u32);
    void setSeCategoryVolume(u32, u32);
    void setRegisterTrackCallback();
    void setFrameCallback(JAIFrameCallback*);
    void initStream();
    void initDriver(JKRHeap*, u32, u32);
    void initAudioThread(JKRHeap*, u32, u32);
    void initInterface(u32);
    void initInterfaceMain();
    u32 initReadFile();
    void initHeap();
    void initCamera();
    void initArchive();
    void initResourcePath();
    void setSeExtParameter(JAISound*);
    void stopAllSe(u32, void*);
    void stopPlayingCategoryObjectSe(u32, void*);
    static u16 setParameterSeqSync(void*, u32);
    gptr<JAInter::Camera> mAudioCamera;
    gptr<JKRHeap> mHeap;
    gptr<void> mDriverHeap;
    gptr<void> mDriverParameters;
    be<u8> initLoadFileSw;
    be<u8> mOutputMode;
    be<u16> mFlags;
    be<u32> mFrameCounter;
    be<u32> mStreamParameter;
    gptr<void> mInitData;
    gptr<void> mSceneTable;
    gptr<void> mStreamBuffer;
    gptr<JAIFrameCallback> mFrameCallback;
    gptr<BasicVtableRecord> mVtable;
};
WWHD_OFFSET(JAIBasic, initLoadFileSw, 0x10);
WWHD_OFFSET(JAIBasic, mFlags, 0x12);
WWHD_OFFSET(JAIBasic, mFrameCounter, 0x14);
WWHD_OFFSET(JAIBasic, mSceneTable, 0x20);
WWHD_OFFSET(JAIBasic, mFrameCallback, 0x28);
WWHD_OFFSET(JAIBasic, mVtable, 0x2C);
WWHD_SIZE(JAIBasic, 0x30);
JAISoundInfoView* getBasicSoundInfo(u32);
u32 getBasicSoundFormat(u32);
u32 canProcessAudioFrame();
void processBasicSeFrame();
void processBasicSequenceFrame();
void processBasicStreamFrame();
using BasicTrackCallback = u16 (*)(void*, u32);
void registerBasicTrackCallback(BasicTrackCallback);

void basicSoundAssert(const char*, u32, const char*);
void storeBasicSequence(gptr<JAISound>*, JAInter::Actor*, u32, u32, u32, JAISoundInfoView*);
void storeBasicSe(gptr<JAISound>*, JAInter::Actor*, u32, u32, u32, JAISoundInfoView*);
void storeBasicStream(gptr<JAISound>*, JAInter::Actor*, u32, u32, u32, JAISoundInfoView*);
void releaseBasicSequence(JAISound*, u32);
void releaseBasicSe(JAISound*, u32);
void releaseBasicStream(JAISound*, u32);
void setBasicPrepareFlag(JAISound*, u32);
u32 basicCategoryMax();
u32 basicSoundCategory(u32);

u32 basicSoundSwitch(JAISound*, u32);
void basicClearSoundOwner(JAISound*);
void* allocateBasicSoundArray(u32, JKRHeap*, s32);
using BasicSoundConstructor = JAISound* (*)(JAISound*);
JAISound* constructBasicSound(JAISound*);
JAISound* constructBasicSoundArray(void*, u32, u32, BasicSoundConstructor, u32);

void basicSetFxmix(JAISound*, f32, u32, u32);
u32 checkBasicInitFile();
void checkBasicInitMemory();
JKRHeap* createBasicSolidHeap(u32, JKRHeap*, u32);

struct BasicPathHeapVtable { be<u32> reserved[13]; be<u32> alloc; };
struct BasicPathHeapView { u8 reserved[12]; gptr<BasicPathHeapVtable> vtable; };
struct BasicArchiveName { be<u8> bytes[256]; };
WWHD_OFFSET(BasicPathHeapView, vtable, 12);
WWHD_OFFSET(BasicPathHeapVtable, alloc, 0x34);
void formatBasicPath(void*, const char*, void*, void*, u32);
void getBasicArchiveName(be<u8>*);
void setBasicArchive(void*);

struct BasicHeapName {
    gptr<const char> name;
    be<u32> vtable;
    void destroy(u32 flags);
    void assureTermination();
};
WWHD_SIZE(BasicHeapName, 8);
struct BasicCameraMatrix { be<f32> entries[12]; };
struct BasicVectorBits { be<u32> x, y, z; };
using BasicCameraConstructor = void (*)(JAInter::Camera*);
void basicInitializeCamera(JAInter::Camera*);
JAInter::Camera* constructBasicCameras(void*, u32, u32, BasicCameraConstructor, u32);
void basicLookAt(BasicCameraMatrix*, cXyz*, cXyz*, BasicVectorBits*);
void registerBasicStatics(AudioDestructorRecord*);

u32 getBasicHeapOverhead(u32);
void* createBasicDramHeap(u32, BasicHeapName*, JKRHeap*, u32, u32);
void initBasicDramHeap(void*, u32);
void initBasicAramHeap(void*, u32);
void initBasicAudioBank(u32);
void activateAudioWorkPool(u32);  /* 0280F2C4: activates the TWorkPool (101F9D6C), not a heap size */
void initBasicDvdAudio(u32);
void initBasicAudioBridge();
void setBasicAudioBridgeMode(u32, u32);
void initBasicTrackPool(u32);
void startBasicDvdThread(u32, u32, u32, u32);
void startBasicAudioThread(u32, u32);
be<u32>* getBasicSystemInfo();
Pair32 divideBasicTicks(u32, u32, u32, u32);
void sleepBasicTicks(u32, u32);
void setBasicMixerLevel(f32, f32);
void basicAudioInitializationFence();
void initBasicSequenceMgr();
void initBasicSeMgr();
void initBasicStreamMgr();
void startBasicSeSequence();
void checkBasicEnteredSequence();

void deleteBasicHeapName(BasicHeapName*);

struct BasicTrackView {
    u8 reserved[0x2F8];
    gptr<BasicTrackView> parent;
    u8 reservedParent[0x40];
    gptr<void> outer;
    u8 reservedOuter[8];
    be<u32> route;
};
struct BasicSeqParameterView { u8 reserved[0x1360]; u8 rootTrack[1]; };
struct BasicSeqUpdateView {
    be<u32> reserved;
    be<u32> updateFlags;
    u8 reservedFlags[0x40];
    gptr<JAISound> sequence;
};
struct BasicSeTrackUpdate {
    be<u8> flags;
    u8 reserved[3];
    be<f32> volume, pitch, fxmix, pan, dolby;
};
WWHD_OFFSET(BasicTrackView, parent, 0x2F8);
WWHD_OFFSET(BasicTrackView, outer, 0x33C);
WWHD_OFFSET(BasicTrackView, route, 0x348);
WWHD_SIZE(BasicTrackView, 0x34C);
WWHD_OFFSET(BasicSeqParameterView, rootTrack, 0x1360);
WWHD_OFFSET(BasicSeqUpdateView, sequence, 0x48);
WWHD_SIZE(BasicSeqUpdateView, 0x4C);
WWHD_SIZE(BasicSeTrackUpdate, 24);
BasicSeqUpdateView* getBasicPlayTrackInfo(u32);
BasicSeqParameterView* getBasicSeqParameter(JAISound*);
void initBasicOuterParameters(BasicSeqUpdateView*, BasicTrackView*, u32, u32, u32);
void setBasicOuterParameter(void*, u32, f32);
void writeBasicTrackPort(BasicTrackView*, u32, u32);
// Audio1 owner exports: the JAIBasic singleton (104B4F14) and the current audio heap (101F9B54).
extern gptr<JAIBasic>& audioBasicSingleton;
extern gptr<JKRHeap>& audioCurrentHeap;
