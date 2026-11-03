/* JAudio sound controls, following CC0 tww and the WWHD sound-handle layout. */
#include "JSystem/JAudio/JAISound_methods.h"
static JAInter::SeSoundVtable soundVtable;
static AudioHeaderStatics soundStaticState;
static AudioDestructorRecord soundStaticRecord;
static u32 soundLineBit(u32 index) {index&=63;return index<32?1u<<index:0;}
/* 0280B38C */
void JAISound::setSeInterRandomPara(be<f32>* output,u32 range,f32 minimum,f32 maximum) {
    if(extra) return;
    u32 seed=u32(audioRandomSeed)*1664525u+1013904223u;
    audioRandomSeed=seed;
    // The binary's exact mantissa conversion produces the seed with its low9bits cleared.
    u32 random=seed&0xFFFFFE00u;
    u32 remainder=random%(range*2u);
    f32 difference=gabi::fsubs_ppc(static_cast<f32>(remainder+1),static_cast<f32>(range));
    f32 offset=difference/1000.0f;
    f32 old=*output;
    f32 value=gabi::fadds_ppc(old,offset);
    if(value>maximum) *output=maximum;
    else if(old<gabi::fsubs_ppc(minimum,offset)) *output=minimum;
    else *output=value;
}
/* 0280D430 */
void JAISound::setSeDistancePitch(u32 frames) {
    if(extra) return;
    f32 value=1.0f;
    u32 bits=getSwBit();
    if(bits&0x10) {
        u32 seed=u32(audioRandomSeed)*1664525u+1013904223u;
        // Binary mantissa construction, scaling by16 and truncation selects seed's top nibble exactly.
        u32 random=seed>>28;
        value=gabi::fsubs_ppc(1.0f,static_cast<f32>(random)/192.0f);
        audioRandomSeed=seed;
        bits=getSwBit();
    }
    if((bits&0x4000) && !(bits&0x302) && JAIGlobalParameter::audioParamAudioCameraMax==1) {
        f32 maximum=JAIGlobalParameter::audioParamDistanceMax;
        f32 distance=positionInfo[0].distance;
        f32 amount=JAIGlobalParameter::audioParamSeDistancePitchMax;
        if(distance<maximum) value=gabi::fmadds(amount,distance/maximum,value);
        else value=gabi::fadds_ppc(value,amount);
    }
    if(bits&0xC0) value=gabi::fadds_ppc(value,static_cast<f32>(static_cast<u8>(flags))/192.0f);
    setSeInterPitch(4,value,frames,0.0f);
}
/* 0280CB64 */
f32 JAISound::setPositionDopplarCommon(u32 type) {
    if(extra) return 1.0f;
    JAInter::SePositionInfo* position=positionInfo;
    JAInter::Camera* camera=audioBasicSingleton->mAudioCamera;
    cXyz* current=camera->position;
    cXyz* previous=camera->previousPosition;
    f32 py=position->current.y,oldX=position->previous.x,px=position->current.x;
    f32 cy=current->y,cx=current->x;
    f32 objectX=gabi::fsubs_ppc(px,oldX);
    f32 dy=gabi::fsubs_ppc(cy,py),dx=gabi::fsubs_ppc(cx,px);
    f32 cameraX=gabi::fsubs_ppc(cx,static_cast<f32>(previous->x));
    f32 cameraY=gabi::fsubs_ppc(cy,static_cast<f32>(previous->y));
    f32 pz=position->current.z;
    f32 objectY=gabi::fsubs_ppc(py,static_cast<f32>(position->previous.y));
    f32 cz=current->z;
    f32 dz=gabi::fsubs_ppc(cz,pz);
    f32 vx=gabi::fsubs_ppc(cameraX,objectX),vy=gabi::fsubs_ppc(cameraY,objectY);
    f32 cameraZ=gabi::fsubs_ppc(cz,static_cast<f32>(previous->z));
    f32 objectZ=gabi::fsubs_ppc(pz,static_cast<f32>(position->previous.z));
    f32 vz=gabi::fsubs_ppc(cameraZ,objectZ);
    f32 square=gabi::fmadds(dz,dz,gabi::fmadds(dx,dx,gabi::fmuls_ppc(dy,dy)));
    f64 first=soundSqrt(square);
    f32 nextY=gabi::fadds_ppc(dy,vy),nextX=gabi::fadds_ppc(dx,vx),nextZ=gabi::fadds_ppc(dz,vz);
    f32 nextSquare=gabi::fmadds(nextZ,nextZ,gabi::fmadds(nextX,nextX,gabi::fmuls_ppc(nextY,nextY)));
    f64 second=soundSqrt(nextSquare);
    u32 factor=type>>8;factor*=factor;
    f32 difference=static_cast<f32>(first-second);
    f32 scale=static_cast<f32>(JAIGlobalParameter::audioParamDopplarParameter)/static_cast<f32>(factor);
    f32 normalized=difference/scale;
    f32 result=1.0f/gabi::fsubs_ppc(1.0f,normalized);
    if(result<0.1f) return 0.1f;
    if(result>2.0f) result=2.0f;
    return result;
}
/* 0280CD4C */
f32 JAISound::setDistanceVolumeCommon(f32 maximum,u32 type) {
    if(extra) return 1.0f;
    JAInter::SePositionInfo* positions=positionInfo;
    f32 distance;
    if(cameraIndex==4) {
        u32 count=JAIGlobalParameter::audioParamAudioCameraMax;
        distance=positions[0].distance;
        for(u32 i=1;i<count;i=static_cast<u8>(i+1)) {
            f32 candidate=positions[i].distance;
            if(candidate<distance) distance=candidate;
        }
    } else distance=positions[cameraIndex].distance;
    f32 threshold=JAIGlobalParameter::audioParamMaxVolumeDistance;
    if(distance<threshold) return 1.0f;
    distance=gabi::fsubs_ppc(distance,threshold);
    f32 range=gabi::fsubs_ppc(maximum,threshold);
    if(type>3) range=range/static_cast<f32>(1u<<((type&3)+1));
    else if(type) range=gabi::fmuls_ppc(range,static_cast<f32>(1u<<(type&3)));
    if(!(distance<range)) return type>3?0.0f:static_cast<f32>(JAIGlobalParameter::audioParamMinDistanceVolume);
    f32 proportion=gabi::fsubs_ppc(1.0f,distance/range);
    if(type>3) return proportion;
    f32 minimum=JAIGlobalParameter::audioParamMinDistanceVolume;
    return gabi::fmadds(gabi::fsubs_ppc(1.0f,minimum),proportion,minimum);
}
/* 0280CED8 */
f32 JAISound::setDistancePanCommon() {
    if(extra) return 0.5f;
    if(JAIGlobalParameter::audioParamAudioCameraMax!=1) {
        if(cameraIndex==4) return 0.5f;
        return static_cast<f32>(static_cast<u8>(cameraIndex)&1);
    }
    f32 x=positionInfo[0].current.x;
    f32 z=positionInfo[0].current.z;
    f32 ax=std::fabs(x),az=std::fabs(z);
    if(!(ax<1.0f)) {} else if(az<1.0f) return 0.5f;
    f32 maximum=JAIGlobalParameter::audioParamPanDistanceMax;
    if(maximum<ax) ax=maximum;
    if(maximum<az) az=maximum;
    if(x==0.0f && z==0.0f) return 0.5f;
    if(ax<az) return gabi::fadds_ppc(x/gabi::fmuls_ppc(static_cast<f32>(JAIGlobalParameter::audioParamPanAngleParameter2),az),0.5f);
    f32 range=JAIGlobalParameter::audioParamPanDistance2Max;
    f32 result=gabi::fsubs_ppc(range,ax)/gabi::fmuls_ppc(static_cast<f32>(JAIGlobalParameter::audioParamPanAngleParameter),gabi::fsubs_ppc(range,az));
    if(x>0.0f) result=gabi::fsubs_ppc(1.0f,result);
    return result;
}
/* 0280CFFC */
void JAISound::setSeDistanceParameters() {
    if(extra) return;
    u32 frames=reserved5;
    if(state==2) frames=0;
    gabi::call_ptr<void>(vtable->setDistanceVolume,this,frames);
    gabi::call_ptr<void>(vtable->setDistancePan,this,frames);
    gabi::call_ptr<void>(vtable->setDistancePitch,this,frames);
    gabi::call_ptr<void>(vtable->setPositionDopplar,this);
    gabi::call_ptr<void>(vtable->setDistanceFxmix,this,frames);
    gabi::call_ptr<void>(vtable->setDistanceFir,this,frames);
    if(!(getSwBit()&0x400)) {
        JAIBasic* basic=audioBasicSingleton;
        auto* table=basic->mVtable.get();
        f32 value=gabi::call_ptr<f32>(table->getMapInfoFx,basic,static_cast<u32>(mapInfo));
        setFxmix(value,0,2);
    }
    gabi::call_ptr<void>(vtable->setDistanceDolby,this,frames);
}
/* 0280D324 */
void JAISound::setSeDistanceVolume(u32 frames) {
    if(extra) return;
    u32 bits=getSwBit();
    f32 value=1.0f;
    if(!(bits&2)) value=gabi::call_ptr<f32>(vtable->distanceVolumeCommon,this,static_cast<f32>(JAIGlobalParameter::audioParamDistanceMax),(bits>>16)&7);
    setSeInterVolume(4,value,frames,0);
}
/* 0280D3C8 */
void JAISound::setSeDistancePan(u32 frames) {
    if(extra) return;
    f32 value=gabi::call_ptr<f32>(vtable->distancePanCommon,this);
    setSeInterPan(4,value,frames,0);
}
/* 0280D5AC */
void JAISound::setSeDistanceFxmix(u32 frames) {
    if(extra) return;
    u32 amount=JAIGlobalParameter::audioParamSeDefaultFx;
    u32 bits=getSwBit();
    if(!(bits&4) && JAIGlobalParameter::audioParamAudioCameraMax==1) {
        f32 range=JAIGlobalParameter::audioParamDistanceMax;
        f32 distance=positionInfo[0].distance;
        if(!(distance<range)) amount=JAIGlobalParameter::audioParamSeDistanceFxParameter;
        else amount=static_cast<u16>(gabi::ftoi(gabi::fmuls_ppc(distance/range,static_cast<f32>(static_cast<u16>(JAIGlobalParameter::audioParamSeDistanceFxParameter)))));
    }
    if(amount>127) amount=127;
    setSeInterFxmix(4,static_cast<f32>(amount)/127.0f,frames,0);
}
/* 0280D6B8 */
void JAISound::setSeDistanceDolby(u32 frames) {
    if(extra) return;
    f32 amount=0.0f;
    if(position) {
        f32 z=positionInfo[0].current.z;
        f32 front=JAIGlobalParameter::audioParamSeDolbyFrontDistanceMax;
        if(!(z<front)) {
            if(!(z<0.0f)) {
                f32 behind=JAIGlobalParameter::audioParamSeDolbyBehindDistanceMax;
                if(!(z<behind)) amount=127.0f;
                else {f32 center=JAIGlobalParameter::audioParamSeDolbyCenterValue;amount=gabi::fmadds(gabi::fsubs_ppc(127.0f,center),z/behind,center);}
            } else amount=gabi::fmuls_ppc(static_cast<f32>(JAIGlobalParameter::audioParamSeDolbyCenterValue),gabi::fsubs_ppc(front,z))/front;
        }
    }
    u32 value=static_cast<u8>(gabi::ftoi(amount));
    setSeInterDolby(4,static_cast<f32>(value)/127.0f,frames,0);
}
/* 0280D820 */
void JAISound::setSePositionDopplar() {
    if(extra) return;
    u32 frames=JAIGlobalParameter::audioParamDopplarMoveTime;
    if(state==2) frames=1;
    u32 mask=getSwBit()&0x300;
    if(!mask || JAIGlobalParameter::audioParamAudioCameraMax!=1) return;
    f32 value=gabi::call_ptr<f32>(vtable->positionDopplarCommon,this,mask);
    setSeInterPitch(1,value,frames,0.0f);
}
/* 0280C9E4 */
void JAISound::setTrackVolume(u32 index,f32 value,u32 frames) {
    if(extra || (mSoundID&0xC0000000u)!=0x80000000u) return;
    JAInter::SeqParameter* seq=getSeqParameter();
    if(!seq) return;
    if(state>=4 && !(seq->mUpdateData->mUpdateFlags&soundLineBit(index))) return;
    if(frames==0) frames=1;
    s32 result=seq->mTrackVolumes[index].set(value,frames);
    if(result==1) {seq=getSeqParameter();seq->mParameterFlags[6]=u32(seq->mParameterFlags[6])|soundLineBit(index);}
    JAInter::SeqUpdateData* update=getSeqParameter()->mUpdateData;
    if(update && result!=2) update->mActiveTrackFlag=u32(update->mActiveTrackFlag)|0x40;
}
/* 0280CAD4 */
void JAISound::setTrackInterruptSwitch(u32 index,u32 enabled) {
    if(extra || (mSoundID&0xC0000000u)!=0x80000000u) return;
    JAInter::SeqParameter* seq=getSeqParameter();
    if(!seq) return;
    seq->mTrackStates[index]=static_cast<u8>(enabled);
    JAInter::SeqUpdateData* update=getSeqParameter()->mUpdateData;
    if(update) update->mActiveTrackFlag=u32(update->mActiveTrackFlag)|0x800000;
}
/* 0280C72C */
void JAISound::setPauseMode(u32 mode,u32 volume) {
    switch(mSoundID&0xC0000000u) {
    case 0x80000000u: {
        JAInter::SeqParameter* seq=getSeqParameter();
        if(!seq) return;
        if(mode==3) mode=4;
        else if(mode==2) {soundTrackPause(&seq->mTrack,1,1);seq=getSeqParameter();}
        else if(mode==1) {setSeqInterVolume(11,static_cast<f32>(volume)/127.0f,1);seq=getSeqParameter();}
        else if(mode==0) {
            if(seq->mState==1) {setSeqInterVolume(11,1.0f,1);seq=getSeqParameter();}
            else if(seq->mState==2) {soundTrackPause(&seq->mTrack,0,1);seq=getSeqParameter();}
        }
        seq->mState=static_cast<u8>(mode);return;
    }
    case 0xC0000000u: {
        JAInter::StreamParameter* stream=getStreamParameter();
        if(!stream) return;
        if(mode==1) {setStreamInterVolume(11,static_cast<f32>(volume)/127.0f,1);stream=getStreamParameter();}
        else if(mode==0 && stream->mPauseMode==1) {setStreamInterVolume(11,1.0f,1);stream=getStreamParameter();}
        stream->mPauseMode=static_cast<u8>(mode);return;
    }
    default:soundControlAssert("JAISound.cpp",1840,"(0)");return;
    }
}
/* 0280C36C */
void JAISound::setSeqPortData(u32 port,u32 value,u32 frames) {
    gabi::AutoFrame frame;
    gabi::FrameLocal<be<u16>> previous(8);
    if(extra || (mSoundID&0xC0000000u)!=0x80000000u) return;
    JAInter::SeqParameter* seq=getSeqParameter();
    if(!seq) return;
    if(seq->mPortParameters[port].mCurrentValue==0.0f && state>=3) {
        soundReadPort(&seq->mTrack,port<<16,previous.get());
        seq=getSeqParameter();seq->mPortParameters[port].mCurrentValue=static_cast<f32>(static_cast<u16>(*previous));
        seq=getSeqParameter();
    }
    s32 result=seq->mPortParameters[port].set(static_cast<f32>(value),frames);
    if(result==1) {seq=getSeqParameter();seq->mParameterFlags[0]=u32(seq->mParameterFlags[0])|soundLineBit(port);}
    JAInter::SeqUpdateData* update=getSeqParameter()->mUpdateData;
    if(update && result!=2) update->mActiveTrackFlag=u32(update->mActiveTrackFlag)|0x10;
}
/* 0280C504 */
void JAISound::setTrackPortData(u32 trackIndex,u32 port,u32 value) {
    if(extra || (mSoundID&0xC0000000u)!=0x80000000u) return;
    JAInter::SeqParameter* seq=getSeqParameter();
    if(!seq) return;
    JAInter::SeqUpdateData* update=seq->mUpdateData;
    if(!update) return;
    if(state>=4) {
        u32 route=getTrackPortRoute(trackIndex,port);
        soundWritePort(&seq->mTrack,route,value);
        getSeqParameter()->mTrackPorts[trackIndex][port]=static_cast<s16>(value);
    } else {
        update->mActiveTrackFlag=u32(update->mActiveTrackFlag)|0x1000;
        seq=getSeqParameter();seq->mParameterFlags[12]=u32(seq->mParameterFlags[12])|soundLineBit(trackIndex);
        seq=getSeqParameter();seq->mPortFlags[trackIndex]=u32(seq->mPortFlags[trackIndex])|soundLineBit(port);
        getSeqParameter()->mTrackPorts[trackIndex][port]=static_cast<s16>(value);
    }
}
/* 0280C638 */
void JAISound::setSePortData(u32 port,u32 value) {
    if(extra || (mSoundID&0xC0000000u)) return;
    JAInter::SeParameter* se=getSeParameter();
    if(!se) return;
    if(state==1) {
        se->mPortValues[port]=static_cast<u16>(value);
        se=getSeParameter();se->mPortFlags=u16(se->mPortFlags)|soundLineBit(port);
    } else soundSeTrackPort(audioSeSequenceHandle,track,port,value);
}
/* 0280C6D8 */
void JAISound::setPortData(u32 port,u32 value) {
    if(extra) return;
    switch(mSoundID&0xC0000000u) {
    case 0:setSePortData(port,value);return;
    case 0x80000000u:setSeqPortData(port,value,0);return;
    case 0xC0000000u:return;
    default:soundControlAssert("JAISound.cpp",541,"(0)");return;
    }
}
/* 0280BA28 */
void JAISound::setPan(f32 value,u32 frames,u32 index) {
    if(extra) return;
    switch(mSoundID&0xC0000000u) {
    case 0:setSeInterPan(index,value,frames,0);return;
    case 0x80000000u:setSeqInterPan(index,value,frames);return;
    case 0xC0000000u:setStreamInterPan(index,value,frames);return;
    default:soundControlAssert("JAISound.cpp",322,"(0)");return;
    }
}
/* 0280BD74 */
void JAISound::setPitch(f32 value,u32 frames,u32 index) {
    SoundDispatchObject* dispatch=static_cast<SoundDispatchObject*>(static_cast<void*>(extra));
    if(dispatch) {gabi::tail_ptr<void>(dispatch->mVtable->pitch,dispatch,index,value);return;}
    switch(mSoundID&0xC0000000u) {
    case 0:setSeInterPitch(index,value,frames,0.0f);return;
    case 0x80000000u:setSeqInterPitch(index,value,frames);return;
    case 0xC0000000u:setStreamInterPitch(index,value,frames);return;
    default:soundControlAssert("JAISound.cpp",373,"(0)");return;
    }
}
/* 0280BFD8 */
void JAISound::setFxmix(f32 value,u32 frames,u32 index) {
    if(extra) return;
    switch(mSoundID&0xC0000000u) {
    case 0:setSeInterFxmix(index,value,frames,0);return;
    case 0x80000000u:setSeqInterFxmix(index,value,frames);return;
    case 0xC0000000u:return;
    default:soundControlAssert("JAISound.cpp",421,"(0)");return;
    }
}
/* 0280C248 */
void JAISound::setDolby(f32 value,u32 frames,u32 index) {
    if(extra) return;
    switch(mSoundID&0xC0000000u) {
    case 0:setSeInterDolby(index,value,frames,0);return;
    case 0x80000000u:setSeqInterDolby(index,value,frames);return;
    case 0xC0000000u:return;
    default:soundControlAssert("JAISound.cpp",469,"(0)");return;
    }
}
/* 0280B518 */
void JAISound::setSeInterVolume(u32 index,f32 value,u32 frames,u32 variation) {
    gabi::AutoFrame frame;
    gabi::FrameLocal<be<f32>> adjusted(8);
    *adjusted=value;
    if(extra || (mSoundID&0xC0000000u)) return;
    JAInter::SeParameter* se=getSeParameter();
    if(!se) return;
    if(variation!=0) {
        u32 scaled=variation*1000u;
        u32 high=u32((u64(scaled)*0x02040811u)>>32);
        u32 range=(((scaled-high)>>1)+high)>>6;
        JAISound* receiver=soundRandomParameter(this,adjusted.get(),range,0.0f,1.0f);
        se=receiver->getSeParameter();
    }
    se->mVolumes[index].set(*adjusted,frames);
}
/* 0280B8B0 */
void JAISound::setSeInterPan(u32 index,f32 value,u32 frames,u32 variation) {
    gabi::AutoFrame frame;
    gabi::FrameLocal<be<f32>> adjusted(8);
    *adjusted=value;
    if(extra || (mSoundID&0xC0000000u)) return;
    JAInter::SeParameter* se=getSeParameter();
    if(!se) return;
    if(variation!=0) {
        u32 scaled=variation*1000u;
        u32 high=u32((u64(scaled)*0x02040811u)>>32);
        u32 range=(((scaled-high)>>1)+high)>>6;
        JAISound* receiver=soundRandomParameter(this,adjusted.get(),range,0.0f,1.0f);
        se=receiver->getSeParameter();
    }
    se->mPans[index].set(*adjusted,frames);
}
/* 0280BF1C */
void JAISound::setSeInterFxmix(u32 index,f32 value,u32 frames,u32 variation) {
    gabi::AutoFrame frame;
    gabi::FrameLocal<be<f32>> adjusted(8);
    *adjusted=value;
    if(extra || (mSoundID&0xC0000000u)) return;
    JAInter::SeParameter* se=getSeParameter();
    if(!se) return;
    if(variation!=0) {
        u32 scaled=variation*1000u;
        u32 high=u32((u64(scaled)*0x02040811u)>>32);
        u32 range=(((scaled-high)>>1)+high)>>6;
        JAISound* receiver=soundRandomParameter(this,adjusted.get(),range,0.0f,1.0f);
        se=receiver->getSeParameter();
    }
    se->mFxmixes[index].set(*adjusted,frames);
}
/* 0280C18C */
void JAISound::setSeInterDolby(u32 index,f32 value,u32 frames,u32 variation) {
    gabi::AutoFrame frame;
    gabi::FrameLocal<be<f32>> adjusted(8);
    *adjusted=value;
    if(extra || (mSoundID&0xC0000000u)) return;
    JAInter::SeParameter* se=getSeParameter();
    if(!se) return;
    if(variation!=0) {
        u32 scaled=variation*1000u;
        u32 high=u32((u64(scaled)*0x02040811u)>>32);
        u32 range=(((scaled-high)>>1)+high)>>6;
        JAISound* receiver=soundRandomParameter(this,adjusted.get(),range,0.0f,1.0f);
        se=receiver->getSeParameter();
    }
    se->mDolbys[index].set(*adjusted,frames);
}
/* 0280BBB8 */
void JAISound::setSeInterPitch(u32 index,f32 value,u32 frames,f32 variation) {
    gabi::AutoFrame frame;
    gabi::FrameLocal<be<f32>> adjusted(8);
    *adjusted=value;
    if(extra || (mSoundID&0xC0000000u)) return;
    JAInter::SeParameter* se=getSeParameter();
    if(!se) return;
    if(variation!=0) {
        f32 scaled=gabi::fmuls_ppc(variation,1000.0f);
        u32 range;
        if(scaled>=2147483648.0f) range=u32(gabi::ftoi(gabi::fsubs_ppc(scaled,2147483648.0f)))+0x80000000u;
        else range=u32(gabi::ftoi(scaled));
        JAISound* receiver=soundRandomParameter(this,adjusted.get(),range,0.1f,16.0f);
        se=receiver->getSeParameter();
    }
    se->mPitches[index].set(*adjusted,frames);
}
/* 0280B5D4 */
void JAISound::setStreamInterVolume(u32 index,f32 value,u32 frames) {
    if(extra || (mSoundID&0xC0000000u)!=0xC0000000u) return;
    JAInter::StreamParameter* stream=getStreamParameter();
    if(!stream) return;
    s32 result=stream->mVolumes[index].set(value,frames);
    if(result==1) {stream=getStreamParameter();stream->mVolumeFlags=u32(stream->mVolumeFlags)|soundLineBit(index);}
    stream=getStreamParameter();
    JAInter::StreamUpdateData* update=stream->mUpdateData;
    if(update && result!=2) update->mActiveFlags=u32(update->mActiveFlags)|0x40000;
}
/* 0280B96C */
void JAISound::setStreamInterPan(u32 index,f32 value,u32 frames) {
    if(extra || (mSoundID&0xC0000000u)!=0xC0000000u) return;
    JAInter::StreamParameter* stream=getStreamParameter();
    if(!stream) return;
    s32 result=stream->mPans[index].set(value,frames);
    if(result==1) {stream=getStreamParameter();stream->mPanFlags=u32(stream->mPanFlags)|soundLineBit(index);}
    stream=getStreamParameter();
    JAInter::StreamUpdateData* update=stream->mUpdateData;
    if(update && result!=2) update->mActiveFlags=u32(update->mActiveFlags)|0x80000;
}
/* 0280BCBC */
void JAISound::setStreamInterPitch(u32 index,f32 value,u32 frames) {
    if(extra || (mSoundID&0xC0000000u)!=0xC0000000u) return;
    JAInter::StreamParameter* stream=getStreamParameter();
    if(!stream) return;
    s32 result=stream->mPitches[index].set(value,frames);
    if(result==1) {stream=getStreamParameter();stream->mPitchFlags=u32(stream->mPitchFlags)|soundLineBit(index);}
    stream=getStreamParameter();
    JAInter::StreamUpdateData* update=stream->mUpdateData;
    if(update && result!=2) update->mActiveFlags=u32(update->mActiveFlags)|0x100000;
}
/* 0280B2AC */
void JAISound::setSeqInterVolume(u32 index,f32 value,u32 frames) {
    SoundDispatchObject* dispatch=static_cast<SoundDispatchObject*>(static_cast<void*>(extra));
    if(dispatch) {gabi::call_ptr<void>(dispatch->mVtable->volume,dispatch,frames,value);return;}
    if((mSoundID&0xC0000000u)!=0x80000000u) return;
    JAInter::SeqParameter* seq=getSeqParameter();
    if(!seq) return;
    s32 result=seq->mVolumes[index].set(value,frames);
    if(result==1) {seq=getSeqParameter();seq->mParameterFlags[1]=u32(seq->mParameterFlags[1])|soundLineBit(index);}
    JAInter::SeqUpdateData* update=getSeqParameter()->mUpdateData;
    if(update && result!=2) update->mActiveTrackFlag=u32(update->mActiveTrackFlag)|0x40000;
}
/* 0280B78C */
void JAISound::setSeqInterPan(u32 index,f32 value,u32 frames) {
    if(extra) return;
    if((mSoundID&0xC0000000u)!=0x80000000u) return;
    JAInter::SeqParameter* seq=getSeqParameter();
    if(!seq) return;
    if(index>=JAIGlobalParameter::audioSeqParameterLines) {soundControlAssert("JAISound.cpp",859,"line_<JAIGlobalParameter::seqParameterLines");seq=getSeqParameter();}
    s32 result=seq->seqPan[index].set(value,frames);
    if(result==1) {seq=getSeqParameter();seq->mParameterFlags[2]=u32(seq->mParameterFlags[2])|soundLineBit(index);}
    JAInter::SeqUpdateData* update=getSeqParameter()->mUpdateData;
    if(update && result!=2) update->mActiveTrackFlag=u32(update->mActiveTrackFlag)|0x80000;
}
/* 0280BA94 */
void JAISound::setSeqInterPitch(u32 index,f32 value,u32 frames) {
    if(extra) return;
    if((mSoundID&0xC0000000u)!=0x80000000u) return;
    JAInter::SeqParameter* seq=getSeqParameter();
    if(!seq) return;
    if(index>=JAIGlobalParameter::audioSeqParameterLines) {soundControlAssert("JAISound.cpp",881,"line_<JAIGlobalParameter::seqParameterLines");seq=getSeqParameter();}
    s32 result=seq->seqPitch[index].set(value,frames);
    if(result==1) {seq=getSeqParameter();seq->mParameterFlags[3]=u32(seq->mParameterFlags[3])|soundLineBit(index);}
    JAInter::SeqUpdateData* update=getSeqParameter()->mUpdateData;
    if(update && result!=2) update->mActiveTrackFlag=u32(update->mActiveTrackFlag)|0x100000;
}
/* 0280BDF8 */
void JAISound::setSeqInterFxmix(u32 index,f32 value,u32 frames) {
    if(extra) return;
    if((mSoundID&0xC0000000u)!=0x80000000u) return;
    JAInter::SeqParameter* seq=getSeqParameter();
    if(!seq) return;
    if(index>=JAIGlobalParameter::audioSeqParameterLines) {soundControlAssert("JAISound.cpp",903,"line_<JAIGlobalParameter::seqParameterLines");seq=getSeqParameter();}
    s32 result=seq->seqFxmix[index].set(value,frames);
    if(result==1) {seq=getSeqParameter();seq->mParameterFlags[4]=u32(seq->mParameterFlags[4])|soundLineBit(index);}
    JAInter::SeqUpdateData* update=getSeqParameter()->mUpdateData;
    if(update && result!=2) update->mActiveTrackFlag=u32(update->mActiveTrackFlag)|0x200000;
}
/* 0280C03C */
void JAISound::setSeqInterDolby(u32 index,f32 value,u32 frames) {
    if(extra) return;
    if((mSoundID&0xC0000000u)!=0x80000000u) return;
    JAInter::SeqParameter* seq=getSeqParameter();
    if(!seq) return;
    if(index>=JAIGlobalParameter::audioSeqParameterLines) {soundControlAssert("JAISound.cpp",925,"line_<JAIGlobalParameter::seqParameterLines");seq=getSeqParameter();}
    JAInter::MoveParaSet* move=seq->seqDolby+index;
    if(audioBasicSingleton->mOutputMode!=2) {value=0.0f;if(move->mCurrentValue==0.0f) return;}
    s32 result=move->set(value,frames);
    if(result==1) {seq=getSeqParameter();seq->mParameterFlags[5]=u32(seq->mParameterFlags[5])|soundLineBit(index);}
    JAInter::SeqUpdateData* update=getSeqParameter()->mUpdateData;
    if(update && result!=2) update->mActiveTrackFlag=u32(update->mActiveTrackFlag)|0x400000;
}
/* 0280B710 */
f32 JAISound::getSeqInterVolume(u32 index) {
    if(extra) return 1.0f;
    if(state==4 || state==5) return getSeqParameter()->mVolumes[index].mCurrentValue;
    return -1.0f;
}
/* 0280DCB4 */
static void __sinit_JAISound_cpp() {
    soundStaticState.mWords[2]=0;soundStaticState.mWords[0]=0;
    soundStaticState.mWords[3]=0;soundStaticState.mWords[1]=0;
    soundRegister(&soundStaticRecord);
}
static JAInter::StreamUpdateData* soundStreamUpdate;
/* 0280AF40 */
static JAISound* constructSound(JAISound* sound) {
    if(!sound) sound=static_cast<JAISound*>(soundObjectAllocate(sizeof(JAISound)));
    if(sound) {
        sound->extra=nullptr;sound->vtable=&soundVtable;sound->track=0;sound->camera=10;
        sound->fadeCounter=0;
        sound->reserved5=0;
        sound->owner=nullptr;sound->info=nullptr;sound->mapInfo=0;sound->position=nullptr;
        sound->state=0;sound->positionState=0;sound->elapsed=0;sound->priorityAdjustment=0;
        sound->next=nullptr;sound->cameraIndex=0;sound->positionInfo=nullptr;sound->actor=nullptr;
        sound->flags=0;sound->previous=nullptr;sound->parameter=nullptr;sound->mSoundID=0;sound->frameCounter=0;
    }
    return sound;
}
/* 0280AFE0 */
static void destroySound(JAISound* sound,u32 flags) {
    if(!sound) return;
    sound->info=nullptr;sound->mapInfo=0;sound->camera=10;sound->vtable=&soundVtable;
    sound->state=0;sound->priorityAdjustment=0;sound->parameter=nullptr;sound->positionState=0;
    sound->owner=nullptr;sound->previous=nullptr;sound->fadeCounter=0;sound->positionInfo=nullptr;
    sound->elapsed=0;sound->mSoundID=0;sound->actor=nullptr;sound->flags=0;sound->next=nullptr;
    sound->track=0;sound->position=nullptr;
    sound->reserved5=0;
    sound->cameraIndex=0;sound->frameCounter=0;
    if(flags&1) soundObjectDelete(sound);
}
/* 0280D12C */
u32 JAISound::checkSoundHandle(u32 id,JAISoundInfoView* newInfo) {
    if(((u32(mSoundID)^id)&0xC0000000u)==0 && info->priority>newInfo->priority) return 1;
    stop(0);return 0;
}
/* 0280D18C */
void JAISound::initParameter(gptr<JAISound>* handle,JAInter::Actor* actorData,u32 id,u32 frames,u32 cameraNumber,JAISoundInfoView* entry) {
    mSoundID=id;
    if(actorData) {
        actor=static_cast<cXyz*>(actorData->mActor.get());
        if(actorData->mActor) { position=actorData->mPosition;positionState=actorData->mPositionState; }
        else { positionState=0;position=nullptr; }
        mapInfo=actorData->mMapInfo;
    } else { positionState=0;position=nullptr;mapInfo=0;actor=nullptr; }
    fadeCounter=frames;cameraIndex=cameraNumber;info=entry;camera=10;owner=handle;
    reserved5=soundDistanceMoveTime();
    priorityAdjustment=0;frameCounter=0;
    if(handle) *handle=this;
}
/* 0280B68C */
void JAISound::setVolume(f32 value,u32 frames,u32 index) {
    SoundDispatchObject* dispatch=static_cast<SoundDispatchObject*>(static_cast<void*>(extra));
    if(dispatch) { gabi::tail_ptr<void>(dispatch->mVtable->volume,dispatch,frames,value);return; }
    switch(mSoundID&0xC0000000u) {
    case 0:soundSetSeVolume(this,index,value,frames,0);return;
    case 0x80000000u:soundSetSeqVolume(this,index,value,frames);return;
    case 0xC0000000u:soundSetStreamVolume(this,index,value,frames);return;
    default:soundControlAssert("JAISound.cpp",274,"(0)");return;
    }
}
/* 0280B1D8 */
void JAISound::stop(u32 frames) {
    SoundDispatchObject* dispatch=static_cast<SoundDispatchObject*>(static_cast<void*>(extra));
    if(dispatch) gabi::tail_ptr<void>(dispatch->mVtable->stop,dispatch,frames);
    else soundStopHandle(audioBasicSingleton,this,frames);
}
/* 0280B160 */
void JAISound::start(u32 frames) {
    SoundDispatchObject* dispatch=static_cast<SoundDispatchObject*>(static_cast<void*>(extra));
    if(dispatch) gabi::call_ptr<void>(dispatch->mVtable->start,dispatch,frames);
    else { setPrepareFlag(0);fadeCounter=frames; }
}
/* 0280C4C8 */
u32 JAISound::getTrackPortRoute(u32 trackIndex,u32 port) {
    if(mSoundID&0x800) return 0x20000000u+(trackIndex>>4)+((trackIndex<<4)&0xF0u)+(port<<16);
    return 0x10000000u+(trackIndex&15)+(port<<16);
}
/* 0280D2D8 */
u32 JAISound::getID() {
    SoundDispatchObject* dispatch=static_cast<SoundDispatchObject*>(static_cast<void*>(extra));
    if(dispatch) return gabi::tail_ptr<u32>(dispatch->mVtable->getID,dispatch);
    return mSoundID;
}
/* 0280B064 */
void JAISound::setSeqPrepareFlag(u32 flag) {
    if(!extra && (mSoundID&0xC0000000u)==0x80000000u) {
        JAInter::SeqParameter* seq=getSeqParameter();
        if(seq) seq->mUpdateData->mPrepareFlag=flag;
    }
}
/* 0280B0BC */
void JAISound::setStreamPrepareFlag(u32 flag) {
    if(!extra && (mSoundID&0xC0000000u)==0xC0000000u && getStreamParameter())
        soundStreamUpdate->mPrepareFlag=flag;
}
/* 0280B110 */
void JAISound::setPrepareFlag(u32 flag) {
    if(extra) return;
    switch(mSoundID&0xC0000000u) {
    case 0:return;
    case 0x80000000u:setSeqPrepareFlag(flag);return;
    case 0xC0000000u:setStreamPrepareFlag(flag);return;
    default:soundControlAssert("JAISound.cpp",582,"(0)");return;
    }
}
/* 0280C2AC */
void JAISound::setSeqTempoProportion(f32 value,u32 frames) {
    if(!extra && (mSoundID&0xC0000000u)==0x80000000u) {
        JAInter::SeqParameter* seq=getSeqParameter();
        if(seq) {
            seq->mTempo.set(value,frames);
            JAInter::SeqUpdateData* update=getSeqParameter()->mUpdateData;
            if(update) update->mActiveTrackFlag=u32(update->mActiveTrackFlag)|4;
        }
    }
}
/* 0280C320 */
void JAISound::setTempoProportion(f32 value,u32 frames) {
    if(extra) return;
    switch(mSoundID&0xC0000000u) {
    case 0:case 0xC0000000u:return;
    case 0x80000000u:setSeqTempoProportion(value,frames);return;
    default:soundControlAssert("JAISound.cpp",517,"(0)");return;
    }
}
/* 0280B05C */
JAInter::SeqParameter* JAISound::getSeqParameter() {
    return reinterpret_cast<JAInter::SeqParameter*>(static_cast<JAInter::SeParameter*>(parameter));
}
/* 0280B0B4 */
JAInter::StreamParameter* JAISound::getStreamParameter() {
    return reinterpret_cast<JAInter::StreamParameter*>(static_cast<JAInter::SeParameter*>(parameter));
}
/* 0280B384 */
JAInter::SeParameter* JAISound::getSeParameter() { return parameter; }
/* 0280CFF0 */
u32 JAISound::getSwBit() { return info->mFlag; }
/* 0280D300 */
u32 JAISound::checkSwBit(u32 mask) { return getSwBit()&mask; }
/* 0280D2C4 */
u8 JAISound::getInfoPriority() { return info->priority; }
/* 0280D2D0 */
u8 JAISound::getSeCategoryNumber() { return soundCategoryFromID(mSoundID); }
/* 0280CB4C */
void JAISound::clearMainSoundPPointer() {
    gptr<JAISound>* handle=owner;
    if(handle) *handle=nullptr;
}
/* 0280DCDC */
void JAISound::setSeDistanceFir(u8) {}
/* WWHD glue
ext 028F4384 f64 soundSqrt(f64)
ext 0281882C void soundTrackPause(JASystem::TTrack*,u32,u32)
str 1016FBA0 in 0280C72C
str 1016FB9C in 0280C72C
ext 02818C68 u32 soundReadPort(JASystem::TTrack*,u32,be<u16>*)
ext 02818C20 u32 soundWritePort(JASystem::TTrack*,u32,u32)
ext 0280C504 void soundSeTrackPort(JAISound*,u32,u32,u32)
str 1016FB84 in 0280C6D8
str 1016FB80 in 0280C6D8
str 1016FB54 in 0280C248
str 1016FB50 in 0280C248
str 1016FB00 in 0280BFD8
str 1016FAFC in 0280BFD8
str 1016FAAC in 0280BD74
str 1016FAA8 in 0280BD74
str 1016FA4C in 0280BA28
str 1016FA48 in 0280BA28
ext 0280B38C JAISound* soundRandomParameter(JAISound*,be<f32>*,u32,f32,f32)
str 1016FB40 in 0280C03C
str 1016FB14 in 0280C03C
str 1016FAEC in 0280BDF8
str 1016FAC0 in 0280BDF8
str 1016FA90 in 0280BA94
str 1016FA64 in 0280BA94
str 1016FA38 in 0280B78C
str 1016FA0C in 0280B78C
data soundVtable 1016FC2C
data soundStaticState 104B50A0
data soundStaticRecord 101F9CB8
ext 028F026C void soundRegister(void*)
str 1016F9F0 in 0280B68C
str 1016F9EC in 0280B68C
ext 028047C0 u32 soundDistanceMoveTime()
ext 0280B2AC void soundSetSeqVolume(JAISound*,u32,f32,u32)
ext 0280B5D4 void soundSetStreamVolume(JAISound*,u32,f32,u32)
ext 0280B518 void soundSetSeVolume(JAISound*,u32,f32,u32,u32)
data soundStreamUpdate 104B512C
str 1016F9BC in 0280B110
str 1016F9B8 in 0280B110
str 1016FB6C in 0280C320
str 1016FB68 in 0280C320
ext 02802A2C void soundStopHandle(JAIBasic*,JAISound*,u32)
ext 0273AA24 void soundControlAssert(const char*,u32,const char*)
ext 0273AD10 void* soundObjectAllocate(u32)
ext 0273AF40 void soundObjectDelete(void*)
ext 0280729C u8 soundCategoryFromID(u32)
*/
