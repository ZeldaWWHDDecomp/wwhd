/* Sequence manager, following CC0 tww; HD retains separate constructor thunks. */
#include "JSystem/JAudio/JAISequenceMgr.h"
#include "JSystem/JAudio/JAIGlobalParameter.h"
#include "JSystem/JAudio/JAISound_methods.h"
#include "JSystem/JAudio/JAISequenceHeap.h"
using namespace JAInter;
using namespace JAInter::SequenceMgr;
static SeqUpdateData* seqTrackInfo;
static void* fixedSequenceBuffers;
static SequenceArchive* arcPointer;
static SequenceArchiveEntry* archiveEntries;
static u32 archiveEntryCount;
static s32 sequenceWaveScene;
static u32 sequenceCookieSize;
static AudioHeaderStatics sequenceStaticState;
static AudioDestructorRecord sequenceStaticRecord;
static AudioDestructorRecord sequenceGuardRecord;
static LinkSound seqControl;
static u32 sequenceShiftOne(u32 count) { count&=63;return count<32?(1u<<count):0; }
/* 0280AD40 */
static void __sinit_JAISequenceMgr_cpp() {
    sequenceStaticState.mWords[2]=0;sequenceStaticState.mWords[0]=0;
    sequenceStaticState.mWords[3]=0;sequenceStaticState.mWords[1]=0;
    sequenceRegister(&sequenceStaticRecord);
    sequenceGuardConstruct(seqControl.criticalSection);
    sequenceRegister(&sequenceGuardRecord);
}

/* 02808090 */
static void initializeSequencePortParameter(MoveParaSet* parameter) {
    parameter->mMoveCounter=0;parameter->mCurrentValue=1.0f;parameter->mTargetValue=1.0f;
}
/* 028080AC */
static void initializeSequenceVolumeParameter(MoveParaSet* parameter) {
    parameter->mMoveCounter=0;parameter->mCurrentValue=1.0f;parameter->mTargetValue=1.0f;
}
/* 028080C8 */
static void initializeSequenceTrackVolume(MoveParaSet* parameter) {
    parameter->mMoveCounter=0;parameter->mCurrentValue=1.0f;parameter->mTargetValue=1.0f;
}
/* 028080E4 */
static void initializeSequenceTrackPitch(MoveParaSet* parameter) {
    parameter->mMoveCounter=0;parameter->mCurrentValue=1.0f;parameter->mTargetValue=1.0f;
}
/* 0280811C */
static void initializeSequencePitchParameter(MoveParaSet* parameter) {
    parameter->mMoveCounter=0;parameter->mCurrentValue=1.0f;parameter->mTargetValue=1.0f;
}
/* 02808138 */
static void initializeSequenceFxmixParameter(MoveParaSet* parameter) {
    parameter->mMoveCounter=0;parameter->mCurrentValue=1.0f;parameter->mTargetValue=1.0f;
}
/* 02808154 */
static void initializeSequenceDolbyParameter(MoveParaSet* parameter) {
    parameter->mMoveCounter=0;parameter->mCurrentValue=1.0f;parameter->mTargetValue=1.0f;
}
/* 0280AD9C */
static MoveParaSet* constructSequenceHalfParameter(MoveParaSet* parameter) {
    if(!parameter) parameter=static_cast<MoveParaSet*>(sequenceObjectAllocate(sizeof(MoveParaSet)));
    if(parameter) { parameter->mMoveCounter=0;parameter->mTargetValue=0.5f;parameter->mCurrentValue=0.5f; }
    return parameter;
}
/* 0280ADE8 */
static MoveParaSet* constructSequenceZeroParameter(MoveParaSet* parameter) {
    if(!parameter) parameter=static_cast<MoveParaSet*>(sequenceObjectAllocate(sizeof(MoveParaSet)));
    if(parameter) { parameter->mMoveCounter=0;parameter->mTargetValue=0.0f;parameter->mCurrentValue=0.0f; }
    return parameter;
}
/* 0280AE34 */
static MuteBit* constructSequenceMuteBit(MuteBit* bit) {
    if(!bit) bit=static_cast<MuteBit*>(sequenceObjectAllocate(sizeof(MuteBit)));
    if(bit) bit->mFlags=static_cast<u8>(bit->mFlags)&0x5F;
    return bit;
}
/* 0280AEC8 */
static PlayerParameter* constructSequencePlayerParameter(PlayerParameter* parameter) {
    if(!parameter) parameter=static_cast<PlayerParameter*>(sequenceObjectAllocate(sizeof(PlayerParameter)));
    if(parameter) {
        parameter->mTrack=nullptr;
        JSUPtrLink* link=&parameter->mCommand;
        if(gabi::ea(link)==0) link=static_cast<JSUPtrLink*>(sequenceObjectAllocate(sizeof(JSUPtrLink)));
        if(link) sequencePortLinkConstruct(link,&parameter->mCommand);
        parameter->mCommand.mArgs=nullptr;parameter->mCommand.mFunc=0;
    }
    return parameter;
}
/* 02808170 */
static SeqParameter* constructSequenceParameter(SeqParameter* parameter) {
    if(!parameter) parameter=static_cast<SeqParameter*>(sequenceObjectAllocate(sizeof(SeqParameter)));
    if(parameter) {
        parameter->mTempo.mMoveCounter=0;parameter->mTempo.mCurrentValue=1.0f;parameter->mTempo.mTargetValue=1.0f;
        constructSequenceMoveArray(parameter->mPortParameters,16,16,initializeSequencePortParameter);
        constructSequenceMoveArray(parameter->mVolumes,20,16,initializeSequenceVolumeParameter);
        constructSequenceMoveArray(parameter->mTrackVolumes,32,16,initializeSequenceTrackVolume);
        constructSequenceAllocatingMoveArray(parameter->mTrackPans,32,16,constructSequenceHalfParameter);
        constructSequenceMoveArray(parameter->mTrackPitches,32,16,initializeSequenceTrackPitch);
        constructSequenceAllocatingMoveArray(parameter->mTrackFxmixes,32,16,constructSequenceZeroParameter);
        constructSequenceAllocatingMoveArray(parameter->mTrackDolbys,32,16,constructSequenceZeroParameter);
        constructSequenceAllocatingMoveArray(parameter->mTrackAuxMixes,32,16,constructSequenceZeroParameter);
        constructSequenceMuteArray(parameter->mMuteBits,32,1,constructSequenceMuteBit);
        constructSequenceTrack(&parameter->mTrack);
        constructSequenceOuter(&parameter->mOuterParam);
    }
    return parameter;
}
static JKRHeap* getSequenceCurrentHeap() {
    JKRHeap* heap=audioCurrentHeap;
    if(!heap) { sequenceAssert("JAIBasic.h",169,"msCurrentHeap");heap=audioCurrentHeap; }
    return heap;
}
/* 028082B0 */
static SeqUpdateData* constructSequenceUpdateData(SeqUpdateData* update) {
    if(!update) update=static_cast<SeqUpdateData*>(sequenceObjectAllocate(sizeof(SeqUpdateData)));
    if(update) {
        update->mActiveTrackFlag=0;update->mUpdateFlags=0;
        update->mSeqVolume=1.0f;update->mSeqPitch=1.0f;update->mSeqTempo=1.0f;
        update->mReadState=0;update->mState=0;update->mSeqPan=0.5f;update->mSeqDolby=0.0f;update->mSeqFxmix=0.0f;
        update->mUpdateState=0;update->mPrepareFlag=0;update->mSequence=nullptr;
        JKRHeap* heap=getSequenceCurrentHeap();
        void* storage=sequenceHeapAllocate(sequenceCookieSize+33*sizeof(PlayerParameter),heap,32);
        PlayerParameter* players=nullptr;
        if(storage) players=constructSequencePlayerArray(static_cast<u8*>(storage)+sequenceCookieSize,33,sizeof(PlayerParameter),constructSequencePlayerParameter,0);
        update->systemTrackParameter=players;
        if(!players) sequenceAssert("JAISequenceMgr.cpp",115,"systemTrackParameter");
        heap=getSequenceCurrentHeap();
        be<f32>* volume=static_cast<be<f32>*>(sequenceHeapAllocate(sequenceTrackMax()*4,heap,32));
        update->trackVolume=volume;
        if(!volume) sequenceAssert("JAISequenceMgr.cpp",117,"trackVolume");
        heap=getSequenceCurrentHeap();
        be<f32>* pan=static_cast<be<f32>*>(sequenceHeapAllocate(sequenceTrackMax()*4,heap,32));
        update->trackPan=pan;
        if(!pan) sequenceAssert("JAISequenceMgr.cpp",119,"trackPan");
        heap=getSequenceCurrentHeap();
        be<f32>* pitch=static_cast<be<f32>*>(sequenceHeapAllocate(sequenceTrackMax()*4,heap,32));
        update->trackPitch=pitch;
        if(!pitch) sequenceAssert("JAISequenceMgr.cpp",121,"trackPitch");
        heap=getSequenceCurrentHeap();
        be<f32>* fxmix=static_cast<be<f32>*>(sequenceHeapAllocate(sequenceTrackMax()*4,heap,32));
        update->trackFxmix=fxmix;
        if(!fxmix) sequenceAssert("JAISequenceMgr.cpp",123,"trackFxmix");
        heap=getSequenceCurrentHeap();
        be<f32>* dolby=static_cast<be<f32>*>(sequenceHeapAllocate(sequenceTrackMax()*4,heap,32));
        update->trackDolby=dolby;
        if(!dolby) sequenceAssert("JAISequenceMgr.cpp",125,"trackDolby");
        heap=getSequenceCurrentHeap();
        be<u32>* flags=static_cast<be<u32>*>(sequenceHeapAllocate(sequenceTrackMax()*4+4,heap,32));
        update->trackupdate=flags;
        if(!flags) sequenceAssert("JAISequenceMgr.cpp",133,"trackupdate");
    }
    return update;
}
namespace JAInter { namespace SequenceMgr {
/* 028098AC */
void checkPlayingSeqTrack(u32 index) {
    gabi::AutoFrame frame;
    gabi::FrameLocal<be<u16>> portValue(16);
    SeqUpdateData* update=seqTrackInfo+index;
    if(update->mSequence->getSeqParameter()->mState==2) return;
    SeqParameter* parameter=update->mSequence->getSeqParameter();
    be<u32>* trackFlags=update->trackupdate;
    for(u32 i=0;i<sequenceTrackMax()+1;i=static_cast<u8>(i+1)) trackFlags[i]=0;
    if(update->mActiveTrackFlag&2) {
        JAISound* sound=update->mSequence;
        if(!sound->fadeCounter || sound->state<4) {
            if(sound->state>=3) {sequenceTrackStop(&parameter->mTrack);sound=update->mSequence;}
            sound->clearMainSoundPPointer();stopSeq(update->mSequence);
            update->mActiveTrackFlag=0;return;
        }
        sequenceSetInterVolume(sound,7,0.0f,sound->fadeCounter);
        update->mSequence->state=5;
        update->mActiveTrackFlag=u32(update->mActiveTrackFlag)^2;
    }
    JAISound* sound=update->mSequence;
    if(sound) {
        if(sound->actor) {
            u32 camera=0,count;
            if(sound->cameraIndex==4) {count=sequenceCameraMax();sound=update->mSequence;}
            else {camera=sound->cameraIndex;count=1;}
            for(;count;--count,++camera) {
                SePositionInfo* position=sound->positionInfo+camera;
                position->previous=position->current;
                void* matrix=audioBasicSingleton->mAudioCamera[camera].matrix;
                sequenceTransform(matrix,sound->position,&position->current);
                f32 y=position->current.y,x=position->current.x,z=position->current.z;
                f32 square=gabi::fmadds(z,z,gabi::fmadds(x,x,gabi::fmuls_ppc(y,y)));
                position->distance=static_cast<f32>(sequenceSqrt(square));
                sound=update->mSequence;
                SeSoundVtable* table=sound->vtable;
                f64 maximum=sequenceDistanceMax();
                f32 volume=gabi::call_ptr<f32>(table->distanceVolumeCommon,sound,maximum,0u);
                u32 frames=sequenceDistanceMoveTime();
                sequenceSetInterVolume(update->mSequence,4,static_cast<f32>(static_cast<u8>(gabi::ftoi(gabi::fmuls_ppc(volume,127.0f)))),frames);
                sound=update->mSequence;
                f32 pan=gabi::call_ptr<f32>(sound->vtable->distancePanCommon,sound);
                frames=sequenceDistanceMoveTime();
                sequenceSetInterPan(update->mSequence,4,static_cast<f32>(static_cast<u8>(gabi::ftoi(pan))),frames);
                sound=update->mSequence;
                f32 pitch=gabi::call_ptr<f32>(sound->vtable->positionDopplarCommon,sound,256u);
                frames=sequenceDopplerTime();
                sequenceSetInterPitch(update->mSequence,4,pitch,frames);
                sound=update->mSequence;
            }
        }
        if(sound) sound->frameCounter=u32(sound->frameCounter)+1;
    }
    if(!update->mActiveTrackFlag) return;
    checkPlayingSeqUpdateMultiplication(index,static_cast<u8>(sequencePlayTrackMax()+12),0x40000,parameter->mVolumes,&parameter->mParameterFlags[1],2,&update->mSeqVolume);
    checkPlayingSeqUpdateAddition(index,sequenceParameterLines(),0x80000,parameter->seqPan,&parameter->mParameterFlags[2],4,&update->mSeqPan,0.5f);
    checkPlayingSeqUpdateMultiplication(index,sequenceParameterLines(),0x100000,parameter->seqPitch,&parameter->mParameterFlags[3],3,&update->mSeqPitch);
    checkPlayingSeqUpdateAddition(index,sequenceParameterLines(),0x200000,parameter->seqFxmix,&parameter->mParameterFlags[4],5,&update->mSeqFxmix,0.0f);
    checkPlayingSeqUpdateAddition(index,sequenceParameterLines(),0x400000,parameter->seqDolby,&parameter->mParameterFlags[5],6,&update->mSeqDolby,0.5f);
    if(update->mActiveTrackFlag&4) {
        if(!parameter->mTempo.move()) update->mActiveTrackFlag=u32(update->mActiveTrackFlag)^4;
        f32 tempo=parameter->mTempo.mCurrentValue;
        if(update->mSeqTempo!=tempo) {
            update->mSeqTempo=tempo;
            u32 track=sequenceTrackMax();
            sequenceSetPortF32(seqTrackInfo+index,track,9,static_cast<f32>(parameter->mTempo.mCurrentValue));
            track=sequenceTrackMax();trackFlags[track]=u32(trackFlags[track])|0x80;
        }
    }
    if(update->mActiveTrackFlag&0x10) {
        for(u32 port=0;port<16;++port) {
            u32 bit=sequenceShiftOne(port);
            MoveParaSet* value=parameter->mPortParameters+port;
            if(parameter->mParameterFlags[0]&bit) {
                if(!value->move()) parameter->mParameterFlags[0]=u32(parameter->mParameterFlags[0])^bit;
            }
            sequenceReadPort(&parameter->mTrack,port<<16,portValue.get());
            u32 data=static_cast<u16>(gabi::ftoi(value->mCurrentValue));
            if(static_cast<u16>(*portValue)!=data) sequenceWritePort(&parameter->mTrack,port<<16,data);
        }
        if(!parameter->mParameterFlags[0]) update->mActiveTrackFlag=u32(update->mActiveTrackFlag)^0x10;
    }
    if(update->mActiveTrackFlag&0x20) {
        update->mActiveTrackFlag=u32(update->mActiveTrackFlag)^0x20;
        for(u32 track=0;track<sequenceTrackMax();track=static_cast<u8>(track+1)) {
            MuteBit* mute=parameter->mMuteBits+track;
            u32 flags=mute->mFlags;
            if((flags&0x20) && ((flags>>7)&1)!=((flags>>6)&1)) {
                JASystem::TTrack* child=sequenceChildTrack(update->mSequence,track);
                if(child) sequenceTrackMute(child,(static_cast<u8>(mute->mFlags)>>6)&1);
                flags=mute->mFlags;mute->mFlags=(flags&0x7F)|((flags<<1)&0x80);
            }
        }
    }
    checkPlayingSeqUpdateTrack(index,0x40,parameter->mTrackVolumes,&parameter->mParameterFlags[6],2,update->trackVolume);
    checkPlayingSeqUpdateTrack(index,0x80,parameter->mTrackPans,&parameter->mParameterFlags[7],4,update->trackPan);
    checkPlayingSeqUpdateTrack(index,0x200,parameter->mTrackPitches,&parameter->mParameterFlags[8],3,update->trackPitch);
    checkPlayingSeqUpdateTrack(index,0x800,parameter->mTrackFxmixes,&parameter->mParameterFlags[9],5,update->trackFxmix);
    checkPlayingSeqUpdateTrack(index,0x100,parameter->mTrackDolbys,&parameter->mParameterFlags[10],6,update->trackDolby);
    if(update->mActiveTrackFlag&0x800000) {
        update->mActiveTrackFlag=u32(update->mActiveTrackFlag)^0x800000;
        for(u32 track=0;track<sequenceTrackMax();track=static_cast<u8>(track+1)) {
            if(parameter->mTrackStates[track]==1) {
                trackFlags[track]=u32(trackFlags[track])|0x40;
                sequenceSetPortU32(seqTrackInfo+index,track,8,1);
                parameter->mTrackStates[track]=0;
            }
        }
    }
    if(update->mActiveTrackFlag&0x1000) {
        for(u32 track=0;track<sequenceTrackMax();track=static_cast<u8>(track+1)) {
            u32 bit=sequenceShiftOne(track);
            if(!(parameter->mParameterFlags[12]&bit)) continue;
            parameter->mParameterFlags[12]=u32(parameter->mParameterFlags[12])^bit;
            u32 pending=parameter->mPortFlags[track];
            for(u32 port=0;port<16;++port) {
                u32 portBit=sequenceShiftOne(port);
                if(pending&portBit) {
                    u32 route=update->mSequence->getTrackPortRoute(track,port);
                    sequenceWritePort(&parameter->mTrack,route,static_cast<u16>(parameter->mTrackPorts[track][port]));
                    pending=u32(parameter->mPortFlags[track])^portBit;parameter->mPortFlags[track]=pending;
                }
            }
        }
        if(!parameter->mParameterFlags[12]) update->mActiveTrackFlag=u32(update->mActiveTrackFlag)^0x1000;
    }
}

/* 0280A68C */
void storeSeqBuffer(gptr<JAISound>* handle,Actor* actor,u32 soundID,u32 parameter,u32 camera,JAISoundInfoView* info) {
    u32 play=info->fixedSequenceSlot;
    if(handle && *handle) {
        if((*handle)->track!=play) (*handle)->stop(0);
        else if((*handle)->checkSoundHandle(soundID,info)) return;
    }
    SeqUpdateData* update=seqTrackInfo+play;
    if(update->mSequence) {
        if(update->mSequence->state==5) {
            sequenceTrackStop(&update->mSequence->getSeqParameter()->mTrack);
            update->mSequence->clearMainSoundPPointer();stopSeq(update->mSequence);
        } else if(update->mSequence->state==1) {
            update->mSequence->getID();*handle=nullptr;return;
        } else if(update->mSequence->info->priority<=info->priority) {
            sequenceTrackStop(&update->mSequence->getSeqParameter()->mTrack);
            update->mSequence->clearMainSoundPPointer();stopSeq(update->mSequence);
        } else {*handle=nullptr;return;}
    }
    JAISound* sound=seqControl.getSound();
    if(!sound) {*handle=nullptr;return;}
    SeqParameter* seq=sound->getSeqParameter();
    sequenceSetOuter(&seq->mTrack,&seq->mOuterParam);
    seq->mParameterFlags[3]=0;seq->mParameterFlags[2]=0;
    seq->mTempo.mMoveCounter=0;seq->mTempo.mCurrentValue=1.0f;seq->mTempo.mTargetValue=1.0f;
    seq->mParameterFlags[1]=0;seq->mParameterFlags[10]=0;seq->mParameterFlags[7]=0;
    seq->mParameterFlags[9]=0;seq->mParameterFlags[6]=0;seq->mParameterFlags[12]=0;
    seq->mHeapNumber=-1;seq->mParameterFlags[5]=0;seq->mParameterFlags[8]=0;
    seq->mParameterFlags[4]=0;seq->mParameterFlags[11]=0;seq->mParameterFlags[0]=0;
    seq->mState=0;seq->mPrepareFlags=0;
    for(u32 i=0;i<sequenceTrackMax();++i) {
        seq->mTrackVolumes[i].mCurrentValue=1.0f;seq->mTrackVolumes[i].mTargetValue=1.0f;seq->mTrackVolumes[i].mMoveCounter=0;
        seq->mTrackPans[i].mCurrentValue=0.5f;seq->mTrackPans[i].mTargetValue=0.5f;seq->mTrackPans[i].mMoveCounter=0;
        seq->mTrackPitches[i].mCurrentValue=1.0f;seq->mTrackPitches[i].mTargetValue=1.0f;seq->mTrackPitches[i].mMoveCounter=0;
        seq->mTrackFxmixes[i].mCurrentValue=0.0f;seq->mTrackFxmixes[i].mTargetValue=0.0f;seq->mTrackFxmixes[i].mMoveCounter=0;
        seq->mTrackDolbys[i].mCurrentValue=0.5f;seq->mTrackDolbys[i].mTargetValue=0.5f;seq->mTrackDolbys[i].mMoveCounter=0;
        seq->mTrackAuxMixes[i].mCurrentValue=0.0f;seq->mTrackAuxMixes[i].mTargetValue=0.0f;seq->mTrackAuxMixes[i].mMoveCounter=0;
        seq->mTrackStates[i]=0;seq->mPortFlags[i]=0;
        for(u32 port=0;port<16;++port) seq->mTrackPorts[i][port]=0;
    }
    for(u32 i=0;i<16;++i) {
        seq->mPortParameters[i].mCurrentValue=0.0f;seq->mPortParameters[i].mMoveCounter=0;seq->mPortParameters[i].mTargetValue=0.0f;
    }
    for(u32 i=0;i<sequencePlayTrackMax()+12;++i) {
        seq->mVolumes[i].mCurrentValue=1.0f;seq->mVolumes[i].mTargetValue=1.0f;seq->mVolumes[i].mMoveCounter=0;
    }
    for(u32 i=0;i<sequenceParameterLines();++i) {
        seq->seqPan[i].mTargetValue=0.5f;seq->seqPan[i].mCurrentValue=0.5f;seq->seqPan[i].mMoveCounter=0;
        seq->seqPitch[i].mTargetValue=1.0f;seq->seqPitch[i].mCurrentValue=1.0f;seq->seqPitch[i].mMoveCounter=0;
        seq->seqFxmix[i].mTargetValue=0.0f;seq->seqFxmix[i].mCurrentValue=0.0f;seq->seqFxmix[i].mMoveCounter=0;
        seq->seqDolby[i].mCurrentValue=0.5f;seq->seqDolby[i].mMoveCounter=0;seq->seqDolby[i].mTargetValue=0.5f;
    }
    update->mSequence=sound;sound->mSoundID=soundID;
    SeqParameter* current=update->mSequence->getSeqParameter();
    current->mUpdateData=seqTrackInfo+play;
    update->mSequence->track=play;
    seqTrackInfo[play].mActiveTrackFlag=1;seqTrackInfo[play].mPrepareFlag=0;seqTrackInfo[play].mUpdateFlags=0;
    if(info->mFlag&1) {
        for(u32 i=0;i<sequencePlayTrackMax();++i) {
            JAISound* other=seqTrackInfo[i].mSequence;
            if(i!=play && other) {other->getID();if(!(other->getSwBit()&2)) sequenceSetInterVolume(other,10,0.0f,10);}
        }
    }
    sound->initParameter(handle,actor,soundID,parameter,static_cast<u8>(camera),info);
}

/* 02808F50 */
void checkEntriedSeq() {
    auto frameScope=gabi::AutoFrame(gabi::current_function());
    gabi::FrameLocal<be<u8>> heapIndex(8);
    for(u32 index=0;index<sequencePlayTrackMax();++index) {
        SeqUpdateData* update=seqTrackInfo+index;
        if(!update->mSequence || !(update->mActiveTrackFlag&1) || update->mUpdateState) continue;
        u32 id=update->mSequence->getID();
        u32 offset=sequenceSoundOffset(audioBasicSingleton,id);
        u32 size=sequenceArchiveSize(arcPointer,offset);
        void* buffer=sequenceFindHeap(update->mSequence->getID()&0x3FF,heapIndex);
        if(!buffer) {
            if(update->mSequence->checkSwBit(0x10)) {
                buffer=sequenceStayHeap(size,update->mSequence->getID()&0x3FF);
                *heapIndex=255;
                update->mSequence->getSeqParameter()->mHeapIndex=255;
                if(!buffer && !update->mSequence->checkSwBit(0x20)) sequenceAssert("JAISequenceMgr.cpp",250,"buffer");
            }
            if(!buffer && (update->mSequence->checkSwBit(0x20) || !update->mSequence->checkSwBit(0x10))) {
                u32 available=sequenceUsefulHeap();
                *heapIndex=static_cast<u8>(available);
                if(available>=sequenceAutoHeapMax()) {
                    for(u32 heap=0;heap<sequenceAutoHeapMax();++heap) {
                        if(sequenceAutoHeaps()[heap].mUseCounter!=0xFFFFFFFFu) {
                            u32 track=0;
                            for(;track<sequencePlayTrackMax();++track) {
                                if(seqTrackInfo[track].mSequence) {
                                    HeapBlock* block=sequenceAutoHeaps()+heap;
                                    u32 number=seqTrackInfo[track].mSequence->getID()&0x3FF;
                                    if(number==block->mDataNumber) track=sequencePlayTrackMax();
                                }
                            }
                            if(track==sequencePlayTrackMax()) {
                                sequenceReleaseHeap(static_cast<u8>(heap));
                                *heapIndex=static_cast<u8>(heap);
                            }
                        }
                    }
                    if(static_cast<u8>(*heapIndex)>=sequenceAutoHeapMax()) {
                        update->mSequence->getID();
                        update->mSequence->stop(0);continue;
                    }
                } else if(size>=sequenceAutoHeapSize()) {
                    sequenceAutoHeapSize();update->mSequence->getID();update->mSequence->stop(0);continue;
                }
                update->mSequence->getSeqParameter()->mHeapIndex=*heapIndex;
                u32 number=update->mSequence->getID()&0x3FF;
                buffer=sequenceAllocateAutoHeap(update->mSequence->getSeqParameter()->mHeapIndex,number);
            }
            if(!update->mSequence->checkSwBit(0x40)) {
                update->mSequence->getID();update->mSequence->getID();
                update->mSequence->state=1;
                u32 heap=static_cast<u8>(*heapIndex);
                u32 callbackArg=index|(offset<<16)|(heap<<8);
                sequenceSetHeapLoaded(heap,1);update->mUpdateState=1;
                sequenceLoadAsync(arcPointer,offset,buffer,size,checkDvdLoadArc,callbackArg);
            } else {
                sequenceLoadSync(arcPointer,offset,buffer,size);
                update->mSequence->state=2;
            }
        } else {
            if(gabi::ea(buffer)==0xFFFFFFFFu) continue;
            update->mSequence->getID();
            u32 heap=static_cast<u8>(*heapIndex);
            if(heap!=255) {
                sequenceAllocateAutoHeap(heap,update->mSequence->getID()&0x3FF);
                heap=static_cast<u8>(*heapIndex);
            }
            update->mSequence->getSeqParameter()->mHeapIndex=heap;
            update->mSequence->state=2;
        }
        if(buffer) {update->mTrackState=static_cast<be<u8>*>(buffer);update->mActiveTrackFlag^=1;}
        else stopSeq(update->mSequence);
    }
}

/* 028085E8 */
void init() {
    JAIBasic* basic=audioBasicSingleton;
    auto* dispatch=basic->mVtable.get();
    u32 count=sequenceControlMax();
    JAISound* sounds=gabi::call_ptr<JAISound*>(dispatch->makeSound,basic,count);
    if(!sounds) sequenceAssert("JAISequenceMgr.cpp",59,"soundObjects");
    JKRHeap* heap=getSequenceCurrentHeap();
    fixedSequenceBuffers=static_cast<gptr<JAISound>*>(sequenceHeapAllocate(sequencePlayTrackMax()*4,heap,32));
    seqControl.init(sounds,sequenceControlMax());
    for(u32 i=0;i<sequenceControlMax();++i) {
        heap=getSequenceCurrentHeap();
        SeqParameter* parameter=static_cast<SeqParameter*>(sequenceParameterAllocate(sizeof(SeqParameter),heap,32));
        if(parameter) parameter=constructSequenceParameter(parameter);
        if(!parameter) sequenceAssert("JAISequenceMgr.cpp",65,"_para");
        static_cast<JAISound*>(static_cast<void*>(seqControl.Buffer))[i].parameter=reinterpret_cast<SeParameter*>(parameter);
        heap=getSequenceCurrentHeap();
        u32 seqPanCount=sequenceParameterLines();
        void* seqPanStorage=sequenceHeapAllocate(seqPanCount*16+sequenceCookieSize,heap,32);
        MoveParaSet* seqPan=nullptr;
        if(seqPanStorage) seqPan=sequenceConstructDynamicMoves(static_cast<u8*>(seqPanStorage)+sequenceCookieSize,seqPanCount,16,sequenceInitPan,0);
        parameter->seqPan=seqPan;
        if(!seqPan) sequenceAssert("JAISequenceMgr.cpp",68,"_para->seqPan");
        heap=getSequenceCurrentHeap();
        u32 seqPitchCount=sequenceParameterLines();
        void* seqPitchStorage=sequenceHeapAllocate(seqPitchCount*16+sequenceCookieSize,heap,32);
        MoveParaSet* seqPitch=nullptr;
        if(seqPitchStorage) seqPitch=sequenceConstructDynamicMoves(static_cast<u8*>(seqPitchStorage)+sequenceCookieSize,seqPitchCount,16,initializeSequencePitchParameter,0);
        parameter->seqPitch=seqPitch;
        if(!seqPitch) sequenceAssert("JAISequenceMgr.cpp",70,"_para->seqPitch");
        heap=getSequenceCurrentHeap();
        u32 seqFxmixCount=sequenceParameterLines();
        void* seqFxmixStorage=sequenceHeapAllocate(seqFxmixCount*16+sequenceCookieSize,heap,32);
        MoveParaSet* seqFxmix=nullptr;
        if(seqFxmixStorage) seqFxmix=sequenceConstructDynamicMoves(static_cast<u8*>(seqFxmixStorage)+sequenceCookieSize,seqFxmixCount,16,initializeSequenceFxmixParameter,0);
        parameter->seqFxmix=seqFxmix;
        if(!seqFxmix) sequenceAssert("JAISequenceMgr.cpp",72,"_para->seqFxmix");
        heap=getSequenceCurrentHeap();
        u32 seqDolbyCount=sequenceParameterLines();
        void* seqDolbyStorage=sequenceHeapAllocate(seqDolbyCount*16+sequenceCookieSize,heap,32);
        MoveParaSet* seqDolby=nullptr;
        if(seqDolbyStorage) seqDolby=sequenceConstructDynamicMoves(static_cast<u8*>(seqDolbyStorage)+sequenceCookieSize,seqDolbyCount,16,initializeSequenceDolbyParameter,0);
        parameter->seqDolby=seqDolby;
        if(!seqDolby) sequenceAssert("JAISequenceMgr.cpp",74,"_para->seqDolby");
    }
    heap=getSequenceCurrentHeap();
    fixedSequenceBuffers=static_cast<gptr<JAISound>*>(sequenceHeapAllocate(sequencePlayTrackMax()*4,heap,32));
    if(!fixedSequenceBuffers) sequenceAssert("JAISequenceMgr.cpp",78,"FixSeqBufPointer");
    heap=getSequenceCurrentHeap();
    u32 tracks=sequencePlayTrackMax();
    void* storage=sequenceHeapAllocate(tracks*sizeof(SeqUpdateData)+sequenceCookieSize,heap,32);
    SeqUpdateData* updates=nullptr;
    if(storage) updates=sequenceConstructUpdates(static_cast<u8*>(storage)+sequenceCookieSize,tracks,sizeof(SeqUpdateData),constructSequenceUpdateData,0);
    seqTrackInfo=updates;
    if(!updates) sequenceAssert("JAISequenceMgr.cpp",80,"seqTrackInfo");
    for(u32 i=0;i<sequencePlayTrackMax();++i) {
        static_cast<gptr<JAISound>*>(static_cast<void*>(fixedSequenceBuffers))[i]=nullptr;
        SeqUpdateData* update=seqTrackInfo+i;
        update->mSeqTempo=1.0f;update->mSeqFxmix=0.0f;update->mSeqPan=0.5f;
        update->mSeqVolume=1.0f;update->mSeqPitch=1.0f;update->mSeqDolby=0.0f;
        for(u32 track=0;track<sequenceTrackMax();++track) {
            update->trackVolume[track]=1.0f;update->trackPan[track]=64.0f;update->trackPitch[track]=1.0f;
            update->trackFxmix[track]=0.0f;update->trackDolby[track]=0.0f;update->trackupdate[track]=0;
        }
    }
}
/* 0280A260 */
void checkReadSeq() {
    for(u32 index=0;index<sequencePlayTrackMax();++index) {
        SeqUpdateData* update=seqTrackInfo+index;
        if(!update->mSequence || update->mSequence->state!=2) continue;
        if(update->mSequence->getSeqParameter()->mHeapNumber!=-1) continue;
        if(update->mSequence->getSeqParameter()->mUpdateData->mPrepareFlag) continue;
        SeqParameter* parameter=update->mSequence->getSeqParameter();
        if(parameter->mTrack.mStatus) continue;
        u32 id=update->mSequence->getID();
        u32 offset=sequenceSoundOffset(audioBasicSingleton,id);
        // The caller obtains the size, although the HD track method consumes only data.
        sequenceArchiveSize(arcPointer,offset);
        sequenceSetData(&parameter->mTrack,static_cast<be<u8>*>(update->mTrackState));
        SeqUpdateData* initial=seqTrackInfo+update->mSequence->track;
        initial->mSeqTempo=1.0f;initial->mSeqFxmix=0.0f;initial->mSeqPan=0.5f;
        initial->mSeqVolume=1.0f;initial->mSeqPitch=1.0f;initial->mSeqDolby=0.0f;
        for(u32 track=0;track<sequenceTrackMax();++track) {
            initial->trackVolume[track]=1.0f;
            initial->trackPan[track]=64.0f;
            initial->trackPitch[track]=1.0f;
            initial->trackFxmix[track]=0.0f;
            initial->trackDolby[track]=0.0f;
            initial->trackupdate[track]=0;
        }
        update->mSequence->state=3;
        if(update->mSequence->fadeCounter>1) {
            sequenceSetInterVolume(update->mSequence,7,0.0f,0);
            JAISound* sound=update->mSequence;
            sequenceSetInterVolume(sound,7,1.0f,sound->fadeCounter);
        }
        if(update->mState) {
            sequencePauseMode(update->mSequence,update->mState,update->mReadState);
            update->mSeqVolume=1.1f;
        }
        JAIBasic* basic=audioBasicSingleton;
        BasicVtableRecord* table=basic->mVtable.get();
        gabi::call_ptr<void>(table->setSeExtParameter,basic,static_cast<JAISound*>(update->mSequence));
        sequencePlayingTrack(index);
        if(update->mSequence) {
            sequenceRootInit(update);
            sequenceStartTrack(&parameter->mTrack);
        }
    }
}
/* 0280A0B4 */
void checkPlayingSeq() {
    for(u32 index=0;index<sequencePlayTrackMax();++index) {
        SeqUpdateData* update=seqTrackInfo+index;
        if(!update->mSequence || update->mSequence->state<4) continue;
        sequencePlayingTrack(index);
        for(u32 track=0;track<sequenceTrackMax()+1;track=(track+1)&255) {
            u32 flags=update->trackupdate[track];
            if(!flags) continue;
            if(track!=32) {
                SeqParameter* parameter=update->mSequence->getSeqParameter();
                if(!(u32(parameter->mUpdateData->mUpdateFlags)&sequenceShiftOne(track))) continue;
                flags=update->trackupdate[track];
            }
            sequenceSetPortU32(update,track,1,flags);
            update->systemTrackParameter[track].mCommand.addPortCmdOnce();
        }
    }
}
/* 028094AC */
void checkPlayingSeqUpdateMultiplication(u32 index,u32 count,u32 mask,MoveParaSet* parameters,be<u32>* flags,u32 port,be<f32>* output) {
    be<u32>* active=&seqTrackInfo[index].mActiveTrackFlag;
    if(!(u32(*active)&mask)) return;
    f32 value=1.0f;
    u32 bitIndex=0;
    for(u32 remaining=count;remaining;--remaining) {
        u32 bit=sequenceShiftOne(bitIndex);
        if(u32(*flags)&bit) {
            if(parameters->move()==0) *flags=u32(*flags)^bit;
        }
        value=gabi::fmuls_ppc(value,parameters->mCurrentValue);
        ++parameters;bitIndex=(bitIndex+1)&255;
    }
    if(f32(*output)!=value) {
        *output=value;
        u32 track=sequenceTrackMax();
        sequenceSetPortF32(seqTrackInfo+index,track,port,value);
        u32 bit=sequenceShiftOne(port-2);
        track=sequenceTrackMax();
        be<u32>* updates=seqTrackInfo[index].trackupdate;
        updates[track]=u32(updates[track])|bit;
    }
    if(u32(*flags)==0) *active=u32(*active)^mask;
}
/* 028095F0 */
void checkPlayingSeqUpdateAddition(u32 index,u32 count,u32 mask,MoveParaSet* parameters,be<u32>* flags,u32 port,be<f32>* output,f32 base) {
    be<u32>* active=&seqTrackInfo[index].mActiveTrackFlag;
    if(!(u32(*active)&mask)) return;
    f32 value=0.0f;
    u32 bitIndex=0;
    for(u32 remaining=count;remaining;--remaining) {
        u32 bit=sequenceShiftOne(bitIndex);
        if(u32(*flags)&bit) {
            if(parameters->move()==0) *flags=u32(*flags)^bit;
        }
        f32 delta=gabi::fsubs_ppc(parameters->mCurrentValue,base);
        value=gabi::fadds_ppc(value,delta);
        ++parameters;bitIndex=(bitIndex+1)&255;
    }
    value=gabi::fadds_ppc(value,base);
    if(value>1.0f) value=1.0f;
    else if(!(value>=0.0f)) value=0.0f;
    if(f32(*output)!=value) {
        *output=value;
        u32 track=sequenceTrackMax();
        sequenceSetPortF32(seqTrackInfo+index,track,port,value);
        u32 bit=sequenceShiftOne(port-2);
        track=sequenceTrackMax();
        be<u32>* updates=seqTrackInfo[index].trackupdate;
        updates[track]=u32(updates[track])|bit;
    }
    if(u32(*flags)==0) *active=u32(*active)^mask;
}
/* 02809790 */
void checkPlayingSeqUpdateTrack(u32 index,u32 mask,MoveParaSet* parameters,be<u32>* flags,u32 port,be<f32>* previous) {
    be<u32>* active=&seqTrackInfo[index].mActiveTrackFlag;
    if(!(u32(*active)&mask)) return;
    u32 portBit=sequenceShiftOne(port-2);
    for(u32 track=0;track<sequenceTrackMax();track=(track+1)&255) {
        u32 bit=sequenceShiftOne(track);
        MoveParaSet* parameter=parameters+track;
        if(u32(*flags)&bit) {
            if(parameter->move()==0) *flags=u32(*flags)^bit;
            f32 value=parameter->mCurrentValue;
            if(f32(previous[track])!=value) {
                previous[track]=value;
                SeqUpdateData* update=seqTrackInfo+index;
                sequenceSetPortF32(update,track,port,parameter->mCurrentValue);
                be<u32>* updates=seqTrackInfo[index].trackupdate;
                updates[track]=u32(updates[track])|portBit;
            }
        }
    }
    if(u32(*flags)==0) *active=u32(*active)^mask;
}

/* 02808E30 */
void stopSeq(JAISound* sound) {
    if(sound->getSwBit()&1) {
        for(u32 i=0;i<sequencePlayTrackMax();++i) {
            JAISound* other=seqTrackInfo[i].mSequence;
            if(other!=sound && other) {
                other->getID();
                if(!(other->getSwBit()&2)) sequenceSetInterVolume(other,10,1.0f,10);
            }
        }
    }
    sound->owner=nullptr;
    sound->getSeqParameter()->mUpdateData=nullptr;
    if(sound->state>=3) sequenceReleaseHeap(sound->getSeqParameter()->mHeapIndex);
    sound->state=0;
    seqControl.releaseSound(sound);
    seqTrackInfo[sound->track].mSequence=nullptr;
}
/* 02809334 */
void checkFadeoutSeq() {
    for(u32 i=0;i<sequencePlayTrackMax();++i) {
        SeqUpdateData* update=seqTrackInfo+i;
        if(update->mSequence && update->mSequence->state==5 && sequenceInterVolume(update->mSequence,7)==0.0f) {
            sequenceTrackStop(&update->mSequence->getSeqParameter()->mTrack);
            update->mSequence->clearMainSoundPPointer();
            stopSeq(update->mSequence);
            update->mActiveTrackFlag=0;
        }
    }
}
/* 02809400 */
void checkStoppedSeq() {
    for(u32 i=0;i<sequencePlayTrackMax();++i) {
        SeqUpdateData* update=seqTrackInfo+i;
        if(update->mSequence && (update->mSequence->state==4 || update->mSequence->state==5)) {
            if(!sequenceActiveFlag(&update->mSequence->getSeqParameter()->mTrack)) {
                update->mSequence->clearMainSoundPPointer();
                stopSeq(update->mSequence);
                update->mActiveTrackFlag=0;
            }
        }
    }
}
/* 0280A1C4 */
void checkStartedSeq() {
    for(u32 i=0;i<sequencePlayTrackMax();++i) {
        SeqUpdateData* update=seqTrackInfo+i;
        if(update->mSequence && update->mSequence->state==3 && sequenceActiveFlag(&update->mSequence->getSeqParameter()->mTrack)) {
            update->mSequence->state=4;
            sequenceTrackInit(update);
        }
    }
}
/* 0280AC38 */
void releaseSeqBuffer(JAISound* sound,u32 frames) {
    u32 state=sound->state;
    if(state>=4 && frames!=0) {
        SeqUpdateData* update=seqTrackInfo+sound->track;
        update->mActiveTrackFlag=u32(update->mActiveTrackFlag)|2;
        seqTrackInfo[sound->track].mSequence->fadeCounter=frames;
        return;
    }
    if(state>=3) sequenceTrackStop(&sound->getSeqParameter()->mTrack);
    else if(state!=0) {
        sound->getID();
        sequenceReleaseHeap(sound->getSeqParameter()->mHeapIndex);
    }
    sound->clearMainSoundPPointer();
    stopSeq(sound);
}

/* 02808D54 */
void checkDvdLoadArc(u32,u32 tag) {
    u32 heapIndex=(tag>>8)&0xFF;
    u32 trackIndex=tag&0xFF;
    u32 offset=(tag>>16)&0x3FF;
    if(heapIndex!=255) sequenceSetHeapLoaded(heapIndex,0);
    if(trackIndex>254) return;
    if(trackIndex<254) {
        SeqUpdateData* update=seqTrackInfo+trackIndex;
        JAISound* sound=update->mSequence;
        update->mUpdateState=0;
        if(sound && sound->state==1) {
            u32 id=sound->getID();
            if(offset==sequenceSoundOffset(audioBasicSingleton,id)) { sound->state=2;return; }
        }
    }
    sequenceReleaseHeap(heapIndex);
}
/* 0280A554 */
void checkSeqWave() {
    for(u32 i=0;i<sequencePlayTrackMax();++i) {
        SeqUpdateData* update=seqTrackInfo+i;
        if(!update->mSequence || !update->mSequence->getSeqParameter()) continue;
        if(update->mSequence->getSeqParameter()->mHeapNumber==-1 || sequenceWaveScene==-1) continue;
        s32 heapNumber=update->mSequence->getSeqParameter()->mHeapNumber;
        sequenceSoundID(update->mSequence);
        u32 scene=sequenceWaveScene;
        u32 heap=heapNumber;
        if(scene==heap || heap==0xFF00FF00u || (((scene^heap)&0xFFFF0000u)==0 && heap==0xFFFFu) || (((scene^heap)&0xFFFFu)==0 && heap==0xFFFF0000u))
            update->mSequence->getSeqParameter()->mHeapNumber=-1;
    }
}
/* 02808D10 */
void setArchivePointer(SequenceArchive* archive) {
    arcPointer=archive;
    u32 count=archive->mCount;
    SequenceArchiveEntry* entries=archive->mEntries;
    for(u32 i=0;i<count;++i) entries[i].mData=reinterpret_cast<be<u8>*>(archive)+gabi::ea(entries[i].mData);
    archiveEntries=entries;archiveEntryCount=count;
}
/* 02808C44 */
void getArchiveName(be<u8>* output) {
    output[0]=0;
    if(sequenceAudioPath()) {
        be<u8>* source=sequenceAudioPath();
        be<u8>* cursor=output;
        while(*cursor) ++cursor;
        do { *cursor=*source;if(!*source) break;++cursor;++source; } while(true);
    }
    be<u8>* source=sequenceArchivePath();
    be<u8>* cursor=output;
    while(*cursor) ++cursor;
    do { *cursor=*source;if(!*source) break;++cursor;++source; } while(true);
    source=sequenceArchiveFilename();cursor=output;
    while(*cursor) ++cursor;
    do { *cursor=*source;if(!*source) break;++cursor;++source; } while(true);
}
/* 0280AD2C */
SeqUpdateData* getPlayTrackInfo(u32 index) { return seqTrackInfo+index; }
/* 0280A654 */
void processGFrameSequence() {
    sequenceCheckEntried();sequenceCheckFadeout();sequenceCheckStopped();
    sequenceCheckPlaying();sequenceCheckStarted();sequenceCheckRead();sequenceCheckWave();
}
}}
/* WWHD glue
ext 028047F0 u32 sequenceCameraMax()
ext 028E8F64 void sequenceTransform(void*,cXyz*,cXyz*)
ext 028F4384 f64 sequenceSqrt(f64)
ext 02804718 f64 sequenceDistanceMax()
ext 028047C0 u32 sequenceDistanceMoveTime()
ext 028047B4 u32 sequenceDopplerTime()
ext 0280B78C void sequenceSetInterPan(JAISound*,u32,f32,u32)
ext 0280BA94 void sequenceSetInterPitch(JAISound*,u32,f32,u32)
ext 02818C68 u32 sequenceReadPort(JASystem::TTrack*,u32,be<u16>*)
ext 02818C20 u32 sequenceWritePort(JASystem::TTrack*,u32,u32)
ext 0280E264 JASystem::TTrack* sequenceChildTrack(JAISound*,u32)
ext 028189A0 void sequenceTrackMute(JASystem::TTrack*,u32)
ext 028197FC void sequenceSetOuter(JASystem::TTrack*,TOuterParam*)
ext 02807C80 void* sequenceFindHeap(u32,be<u8>*)
ext 02807F38 void* sequenceStayHeap(u32,u32)
ext 02807E14 u32 sequenceUsefulHeap()
ext 028046F4 u32 sequenceAutoHeapMax()
ext 0280473C u32 sequenceAutoHeapSize()
ext 02808084 JAInter::HeapBlock* sequenceAutoHeaps()
ext 02807EEC void* sequenceAllocateAutoHeap(u32,u32)
ext 02810828 u32 sequenceLoadAsync(JAInter::SequenceMgr::SequenceArchive*,u32,void*,u32,SequenceLoadCallback,u32)
ext 02810744 u32 sequenceLoadSync(JAInter::SequenceMgr::SequenceArchive*,u32,void*,u32)
str 1016F988 in 02808F50
str 1016F980 in 02808F50
data fixedSequenceBuffers 104B5098
ext 028046E8 u32 sequenceControlMax()
ext 028047FC u32 sequenceParameterLines()
ext 0273B050 void* sequenceParameterAllocate(u32,JKRHeap*,s32)
ext 02808100 void sequenceInitPan(JAInter::MoveParaSet*) callback
ext 028EFF8C JAInter::MoveParaSet* sequenceConstructDynamicMoves(void*,u32,u32,SequenceMoveInitializer,u32)
ext 028EFF8C JAInter::SeqUpdateData* sequenceConstructUpdates(void*,u32,u32,SequenceUpdateConstructor,u32)
str 1016F92C in 028085E8
str 1016F940 in 028085E8
str 1016F950 in 028085E8
str 1016F90C in 028085E8
str 1016F8D4 in 028085E8
str 1016F91C in 028085E8
str 1016F8DC in 028085E8
str 1016F8EC in 028085E8
str 1016F8FC in 028085E8
str 1016F95C in 028085E8
str 1016F970 in 028085E8
data sequenceStaticState 104B503C
data sequenceStaticRecord 101F9C94
data sequenceGuardRecord 101F9CA0
data seqControl 104B504C
ext 028F026C void sequenceRegister(void*)
ext 02760084 void sequenceGuardConstruct(void*)
data sequenceCookieSize 101FCD08
str 1016F868 "JAIBasic.h"
str 1016F858 "msCurrentHeap"
str 1016F874 "JAISequenceMgr.cpp"
str 1016F888 "systemTrackParameter"
str 1016F840 "trackVolume"
str 1016F8A0 "trackPan"
str 1016F8AC "trackPitch"
str 1016F8B8 "trackFxmix"
str 1016F8C4 "trackDolby"
str 1016F84C "trackupdate"
ext 0273AA24 void sequenceAssert(const char*,u32,const char*)
ext 028046DC u32 sequenceTrackMax()
ext 0273B0D4 void* sequenceHeapAllocate(u32,JKRHeap*,s32)
ext 028EFF8C JAInter::PlayerParameter* constructSequencePlayerArray(void*,u32,u32,SequencePlayerConstructor,u32)
ext 028EFFD0 void constructSequenceMoveArray(void*,u32,u32,SequenceMoveInitializer)
ext 028EFFD0 void constructSequenceAllocatingMoveArray(void*,u32,u32,SequenceMoveConstructor)
ext 028EFFD0 void constructSequenceMuteArray(void*,u32,u32,SequenceMuteConstructor)
ext 02816ED8 void constructSequenceTrack(JASystem::TTrack*)
ext 02816BE0 void constructSequenceOuter(TOuterParam*)
ext 0273AD10 void* sequenceObjectAllocate(u32)
ext 027EC50C void sequencePortLinkConstruct(JSUPtrLink*,void*)
data seqTrackInfo 104B5094
data arcPointer 104B509C
data archiveEntries 101F9CAC
data archiveEntryCount 101F9CB0
data sequenceWaveScene 101F9B3C
ext 0280470C u32 sequencePlayTrackMax()
ext 02810660 u32 sequenceArchiveSize(JAInter::SequenceMgr::SequenceArchive*,u32)
ext 02817A34 u32 sequenceSetData(JASystem::TTrack*,void*)
ext 0280C72C void sequencePauseMode(JAISound*,u32,u32)
ext 0280E6F4 void sequenceRootInit(JAInter::SeqUpdateData*)
ext 0281874C u32 sequenceStartTrack(JASystem::TTrack*)
ext 028098AC void sequencePlayingTrack(u32)
ext 0280E31C void sequenceSetPortU32(JAInter::SeqUpdateData*,u32,u32,u32)
ext 0280E300 void sequenceSetPortF32(JAInter::SeqUpdateData*,u32,u32,f32)
ext 0280B2AC void sequenceSetInterVolume(JAISound*,u32,f32,u32)
ext 0280B710 f32 sequenceInterVolume(JAISound*,u32)
ext 02818784 void sequenceTrackStop(JASystem::TTrack*)
ext 0280E184 u32 sequenceActiveFlag(JASystem::TTrack*)
ext 0280E764 void sequenceTrackInit(JAInter::SeqUpdateData*)
ext 028030B4 u32 sequenceSoundOffset(JAIBasic*,u32)
ext 02808048 void sequenceSetHeapLoaded(u32,u32)
ext 02807DF0 void sequenceReleaseHeap(u32)
ext 0280D2D8 u32 sequenceSoundID(JAISound*)
ext 0280479C be<u8>* sequenceAudioPath()
ext 02804790 be<u8>* sequenceArchivePath()
ext 028047A8 be<u8>* sequenceArchiveFilename()
ext 02808F50 void sequenceCheckEntried()
ext 02809334 void sequenceCheckFadeout()
ext 02809400 void sequenceCheckStopped()
ext 0280A0B4 void sequenceCheckPlaying()
ext 0280A1C4 void sequenceCheckStarted()
ext 0280A260 void sequenceCheckRead()
ext 0280A554 void sequenceCheckWave()
*/
