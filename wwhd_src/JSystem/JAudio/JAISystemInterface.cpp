/* Sequence port arguments from CC0 tww, with the Cafe path adapter kept external. */
#include "JSystem/JAudio/JAISystemInterface.h"
#include "JSystem/JAudio/JAIGlobalParameter.h"
using namespace JAInter::SystemInterface;
static AudioHeaderStatics staticState;
static AudioDestructorRecord staticRecord;
static AudioDestructorRecord commandRecord;
static JASystem::Kernel::TPortCmd systemPortCmd;
namespace JAInter { namespace SystemInterface {
/* 0280E764 */
void trackInit(SeqUpdateData* update) {
    JAISound* sound=update->mSequence;
    if(!sound) return;
    u32 count=16;
    u32 flags;
    if(systemSoundID(sound)&0x800) {
        count=32;
        u32 frequency=u32(systemClockInfo()[0])>>2;
        u64 product=u64(frequency)*200u;
        auto ticks=systemDivideTicks(u32(product>>32),u32(product),0,1000000);
        systemSleepTicks(ticks.r3,ticks.r4);
        update->mUpdateFlags=0;flags=0;
    } else flags=update->mUpdateFlags;
    for(u32 track=0;track<count;++track) {
        if(!(flags&(1u<<track))) {
            JASystem::TTrack* child=trackToSeqp(sound,u8(track));
            systemOuterInit(update,child,track,65535,0);
        }
        if(track+1<count) flags=update->mUpdateFlags;
    }
}
/* 0280E184 */
u8 checkSeqActiveFlag(JASystem::TTrack* track) {
    if(!track) return 0;
    u8 status=track->mStatus;
    if(!status) return 0;
    for(u32 i=0;i<16;++i) if(track->mChildren[i]) return status;
    return 0;
}
/* 0280E264 */
JASystem::TTrack* trackToSeqp(JAISound* sound,u8 index) {
    u32 id=systemSoundID(sound);
    SeqParameter* parameter=sound->getSeqParameter();
    JASystem::TTrack* root=&parameter->mTrack;
    if(id&0x800) {
        JASystem::TTrack* child=root->mChildren[index>>4];
        return child?static_cast<JASystem::TTrack*>(child->mChildren[index&15]):nullptr;
    }
    return root->mChildren[index&15];
}
/* 0280E338 */
void setPortParameter(JASystem::Kernel::TPortArgs* args,JASystem::TTrack* track,u32 index,u8 flag) {
    u32 shift=(index-2)&63;
    u32 mask=shift<32?(1u<<shift):0;
    if(static_cast<u32>(args->mFlags)&mask) {
        f32 value=args->mValues[index];
        systemSetOuterParameter(track->mOuterParam,flag,value);
        args->mFlags=static_cast<u32>(args->mFlags)^mask;
    }
}
/* 0280E3A4 */
void setSePortParameter(JASystem::Kernel::TPortArgs* args) {
    JASystem::TTrack* track=args->mTrack;
    if(!track) return;
    setPortParameter(args,track,2,1);setPortParameter(args,track,3,2);
    setPortParameter(args,track,4,8);setPortParameter(args,track,5,4);
    setPortParameter(args,track,9,64);setPortParameter(args,track,6,16);
    if((static_cast<u32>(args->mFlags)&0x40) && args->mInterruptData)
        systemTrackInterrupt(track,5);
}
/* 0280E47C */
u32 outerInit(SeqUpdateData* update,JASystem::TTrack* track,u32 index,u32 flags,u32) {
    if(!track) return 0;
    PlayerParameter* player=&update->systemTrackParameter[index];
    player->mTrack=track;
    JASystem::Kernel::TPortArgs* args=&player->mPortArgs.asStruct;
    args->mTrack=track;
    update->systemTrackParameter[index].mCommand.setPortCmd(setSePortParameter,args);
    TOuterParam* outer=track->mOuterParam;
    if(index==systemSeqTrackMax()) {
        args->mTrackVolume=update->mSeqVolume;args->mTrackPitch=update->mSeqPitch;
        args->mTrackFxmix=update->mSeqFxmix;args->mTrackPan=update->mSeqPan;args->mTrackDolby=update->mSeqDolby;
        args->mFlags=0xFF;args->mTrackTempo=update->mSeqTempo;
        systemOuterOn(outer,64);
    } else {
        SeqParameter* parameter=update->mSequence->getSeqParameter();
        args->mTrackVolume=parameter->mTrackVolumes[index].mCurrentValue;
        args->mTrackPitch=parameter->mTrackPitches[index].mCurrentValue;
        args->mTrackFxmix=parameter->mTrackFxmixes[index].mCurrentValue;
        args->mTrackPan=parameter->mTrackPans[index].mCurrentValue;
        args->mInterruptData=0;args->mTrackDolby=parameter->mTrackDolbys[index].mCurrentValue;
        args->mFlags=0x7F;
        systemTrackMute(track,(static_cast<u8>(parameter->mMuteBits[index].mFlags)>>7)&1);
    }
    for(u32 flag=1;flag<=16;flag<<=1) systemOuterOn(outer,flag);
    for(u32 flag=1;flag<=16;flag<<=1) if(!(flags&flag)) systemSetOuterParameter(outer,flag,0.0f);
    update->systemTrackParameter[index].mCommand.addPortCmdOnce();
    return 1;
}
/* 0280E6F4 */
void rootInit(SeqUpdateData* update) {
    JAISound* sound=update->mSequence;
    SeqParameter* parameter=sound->getSeqParameter();
    JASystem::TTrack* root=&parameter->mTrack;
    u32 tracks=systemSeqTrackMax();
    systemOuterInit(update,root,tracks,0xFFFF,0);
    sound->getSeqParameter();
}
/* 0280E13C */
u32 checkFileExsistence(const char* path) {
    gabi::AutoFrame frame;
    struct PathBuffer { be<u8> bytes[0x101]; };
    gabi::FrameLocal<PathBuffer> buffer(8);
    systemLookupPath(buffer.get(),path);
    return static_cast<u8>(buffer->bytes[0])!=0;
}
/* 0280E300 */
void setSeqPortargsF32(JAInter::SeqUpdateData* update,u32 track,u8 index,f32 value) {
    update->systemTrackParameter[track].mPortArgs.asArray[index].asFloat=value;
}
/* 0280E31C */
void setSeqPortargsU32(JAInter::SeqUpdateData* update,u32 track,u8 index,u32 value) {
    update->systemTrackParameter[track].mPortArgs.asArray[index].asWord=value;
}
}}
/* 0280E844 */
static void __sinit_JAISystemInterface_cpp() {
    staticState.mWords[2]=0;staticState.mWords[0]=0;
    staticState.mWords[3]=0;staticState.mWords[1]=0;
    systemRegister(&staticRecord);
    systemConstructPortLink(&systemPortCmd,&systemPortCmd);
    systemPortCmd.mFunc=0;systemPortCmd.mArgs=nullptr;
    systemRegister(&commandRecord);
}
/* WWHD glue
ext C0009C80 be<u32>* systemClockInfo()
ext 028F5A1C Pair32 systemDivideTicks(u32,u32,u32,u32)
ext C000A0D0 void systemSleepTicks(u32,u32)
ext 02816D60 void systemOuterOn(TOuterParam*,u32)
ext 028189A0 void systemTrackMute(JASystem::TTrack*,u32)
ext 0280D2D8 u32 systemSoundID(JAISound*)
ext 02816CA4 void systemSetOuterParameter(TOuterParam*,u32,f32)
ext 028197F4 void systemTrackInterrupt(JASystem::TTrack*,u32)
ext 028046DC u32 systemSeqTrackMax()
ext 0280E47C void systemOuterInit(JAInter::SeqUpdateData*,JASystem::TTrack*,u32,u32,u32)
data staticState 104B5138
data staticRecord 101F9CF4
data commandRecord 101F9D00
data systemPortCmd 104B5148
ext 0284B7BC void systemLookupPath(void*,const char*)
ext 028F026C void systemRegister(void*)
ext 027EC50C void systemConstructPortLink(void*,void*)
*/
