/* JAudio 1 sound-effect parameters and manager. */
#pragma once
#include "JSystem/JAudio/JAIBasic.h"
#include "JSystem/JAudio/JAISoundParams.h"
namespace JAInter {
struct SeParameter {
    SeParameter();
    be<u16> mPortValues[16];
    be<u16> mPortFlags;
    u8 padding[2];
    MoveParaSet mTrackParameters[16];
    MoveParaSet mVolumes[8];
    MoveParaSet mPans[8];
    MoveParaSet mPitches[8];
    MoveParaSet mFxmixes[8];
    MoveParaSet mAuxMixes[8];
    MoveParaSet mDolbys[8];
    gptr<be<f32>> volumeSource, panSource, pitchSource, fxmixSource;
    be<u32> updateFlags;
    gptr<be<f32>> dolbySource;
    gptr<SeParameter> previous, next;
};
WWHD_OFFSET(SeParameter, mTrackParameters, 0x24);
WWHD_OFFSET(SeParameter, mVolumes, 0x124);
WWHD_OFFSET(SeParameter, mPans, 0x1A4);
WWHD_OFFSET(SeParameter, mPitches, 0x224);
WWHD_OFFSET(SeParameter, mFxmixes, 0x2A4);
WWHD_OFFSET(SeParameter, mAuxMixes, 0x324);
WWHD_OFFSET(SeParameter, mDolbys, 0x3A4);
WWHD_OFFSET(SeParameter, volumeSource, 0x424);
WWHD_OFFSET(SeParameter, previous, 0x43C);
WWHD_SIZE(SeParameter, 0x444);
struct SePlayerParameterView {
    u8 reserved[0x2C];
    u8 mCommand[0x18];
};
struct SeSequenceParameterView { u8 reserved[0x1261]; be<u8> state; };
WWHD_SIZE(SePlayerParameterView, 0x44);
struct SeSeqUpdateView {
    u8 reserved[0x44];
    gptr<be<u32>> trackupdate;
    gptr<JAISound> sequence;
    gptr<SePlayerParameterView> systemTrackParameter;
};
WWHD_OFFSET(SeSeqUpdateView, trackupdate, 0x44);
WWHD_OFFSET(SeSeqUpdateView, sequence, 0x48);
WWHD_SIZE(SeSeqUpdateView, 0x50);
namespace SeMgr {
void init();
void storeSeBuffer(gptr<JAISound>*, JAInter::Actor*, u32, u32, u8, JAISoundInfoView*);
void startSeSequence();
void sendSeAllParameter(JAISound*);
void releaseSeRegist(JAISound*);
void releaseSeBuffer(JAISound*, u32);
void setSeqMuteFromSeStart(JAISound*);
void clearSeqMuteFromSeStop(JAISound*);
void checkPlayingSeUpdateMultiplication(JAISound*, SeSeqUpdateView*, be<f32>*, MoveParaSet*, f32, u32, be<f32>*);
void checkPlayingSeUpdateAddition(JAISound*, SeSeqUpdateView*, be<f32>*, MoveParaSet*, u32, be<f32>*, f32);
void releaseSeParameterPointer(SeParameter*);
SeParameter* getSeParametermeterPointer();
u8 changeIDToCategory(u32);
void processGFrameSe();
void checkNextFrameSe();
void checkPlayingSe();
void checkSeMovePara();
}}
using JAInter::SeSeqUpdateView;
using SeMoveConstructor = void (*)(JAInter::MoveParaSet*);
void constructSeMoveArray(void*, u32, u32, SeMoveConstructor);
void initializeSeHalfParameter(JAInter::MoveParaSet*);
void initializeSeZeroParameter(JAInter::MoveParaSet*);
f64 getSeDolbyCenter();
void registerSeStatics(AudioDestructorRecord*);

void checkSeNextFrame();
void checkSePlaying();
void checkSeParameterMoves();

void seAssert(const char*, u32, const char*);
SeSeqUpdateView* getSePlayTrackInfo(u32);
void setSeSequencePortFloat(SeSeqUpdateView*, u32, u32, f32);
void setSeSoundInterVolume(JAISound*, u32, u32, u32, f32);
u32 getSeSequenceTrackMax();
u32 getSeSwitchBits(JAISound*);
u32 getSeMuteVolume();
u32 getSeMuteMoveSpeed();
void setSeSequenceVolume(JAISound*, u32, u32, f32);
void releaseSeSoundRegistration(JAISound*);

JAInter::SeParameter* getSeSoundParameter(JAISound*);
JAInter::SeSequenceParameterView* getSeSequenceParameter(JAISound*);
u32 getSeCategory(JAISound*);
void setSeSequencePortWord(SeSeqUpdateView*, u32, u32, u32);
void addSePortCommand(void*);
u32 moveSeParameter(JAInter::MoveParaSet*);

void setSeTrackInterrupt(JAISound*, u32, u32);
void releaseSeLinkSound(JAInter::LinkSound*, JAISound*);

using SeArrayConstructor = void* (*)(void*);
void* constructSeArray(void*, u32, u32, SeArrayConstructor, u32);
void* initializeSeLink(void*);
void* initializeSeParameters(void*);
u32 getSeRegistMax();
u32 getSeTrackMax();
u32 getSeSceneMax();
void setSeTrackMax(u32);
void initSeLink(JAInter::LinkSound*, JAISound*, u32);

u32 getSeSoundID(JAISound*);
u32 checkSeSoundHandle(JAISound*, u32, JAISoundInfoView*);
u32 getSePriority(JAISound*);
void stopSeSound(JAISound*, u32);
JAISound* getFreeSeSound(JAInter::LinkSound*);
JAInter::SeParameter* getFreeSeParameters();
u32 getSeCameraMax();
void initSeSound(JAISound*, gptr<JAISound>*, JAInter::Actor*, u32, u32, u32, JAISoundInfoView*);

void readSeTrackPort(BasicTrackView*, u32, be<u16>*);
f64 sqrtSeDistance(f64);
void setSeTrackPort(JAISound*, u32, u32, u32);
void setSeMuteOnStart(JAISound*);
void sendSeParameters(JAISound*);
u32 getSeOffset(JAIBasic*, u32);
u32 checkSeSwitch(JAISound*, u32);
f64 getSeDistanceMax();
u32 getSeWaitMax();

void constructSeScratch(JAISound*);
void destroySeScratch(JAISound*, u32);
void transformSePosition(void*, cXyz*, cXyz*);
void releaseSePlayingBuffer(JAISound*, u32);
// Audio1 owner export: the sound-effect sequence handle (104B5018), read by Sound::setSePortData.
extern gptr<JAISound>& audioSeSequenceHandle;
