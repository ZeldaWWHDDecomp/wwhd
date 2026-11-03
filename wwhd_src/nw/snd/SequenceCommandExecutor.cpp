/* Sequence command effects, reconstructed from WWHD's opcode dispatch. */
#include "nw/snd/SequenceCommandReader.h"
#include "nw/snd/AudioCurves.h"
using namespace nw::snd;
void emitSequenceNote(SequenceCommandTrack_l* track,u32 key,u32 velocity,s32 length,u32 mode);
void setSequenceMuteMode(SequenceCommandTrack_l* track,s32 value);
void releaseSequenceTrackNotes(SequenceCommandTrack_l* track,s32 value);
void updateSequenceTrackNotes(SequenceCommandTrack_l* track);
SequenceCommandTrack_l* findSequenceTrack(void* player,s32 index);
void resetSequenceTrack(SequenceCommandTrack_l* track);
void setSequenceTrackPosition(SequenceCommandTrack_l* track,const be<u8>* sequence,s32 offset);
void startSequenceTrack(SequenceCommandTrack_l* track);
void setSequencePlayerCommandE0(void* player,u32 value,SequenceCommandTrack_l* track);
struct SequencePlayerCommandState_l {
    be<u8> reserved,mMasterByte;
    be<u16> mBoundedLevel;
    SequenceByteRamp mVolume;
};
/* Division follows the Espresso signed overflow/zero result used by the byte ramps. */
static s32 commandDivide(s32 value,s32 divisor) {
    if (divisor==0 || (u32(value)==0x80000000u && divisor==-1)) return value<0 ? -1 : 0;
    return value/divisor;
}
/* inlined byte ramps restart at the interpolated current value. */
static void restartCommandRamp(SequenceByteRamp& ramp,u32 target,s32 frames,bool signedValues) {
    s32 elapsed=ramp.mElapsed;
    s32 duration=ramp.mDuration;
    u32 origin;
    if (elapsed>=duration) {
        origin=ramp.mTarget;
        ramp.mDuration=s16(frames);
        ramp.mElapsed=0;
    } else {
        s32 from=signedValues ? s8(static_cast<u8>(ramp.mOrigin)) : s32(ramp.mOrigin);
        s32 to=signedValues ? s8(static_cast<u8>(ramp.mTarget)) : s32(ramp.mTarget);
        s32 delta=(to-from)*elapsed;
        ramp.mDuration=s16(frames);
        s32 step=commandDivide(delta,duration);
        ramp.mElapsed=0;
        origin=u32(from)+u32(step);
    }
    ramp.mOrigin=origin;
    ramp.mTarget=target;
}
/* Modulation groups share the same four-byte/float command encoding. */
static void setModulationCommand(SequenceModulation& modulation,u32 operation,s32 value) {
    switch (operation) {
    case 0: modulation.mRange=value; break;
    case 1: modulation.mScope=value; break;
    case 2: modulation.mDepth=gabi::fmuls_ppc(f32(u8(value)),0.0078125f); break;
    case 3: modulation.mSpeed=gabi::fmuls_ppc(f32(u8(value)),0.390625f); break;
    case 5: modulation.mType=value; break;
    }
}
/* 028AB2E8 */
void SequenceCommandReader::executeCommand(SequenceCommandTrack_l* track,u32 opcode,s32 first,s32 second) {
    gabi::AutoFrame frame{gabi::current_function()};
    SequenceCommandState_l* state=sequenceCommandState(track);
    void* owner=track->player.get();
    SequencePlayerCommandState_l* player=gabi::at<SequencePlayerCommandState_l>(gabi::ea(owner)+0x1A8);
    if (opcode<=0xFF) {
        switch (opcode) {
        case 0x81: if (first<0x10000) state->mBank=u32(first)&0xFFFF; return;
        case 0x88: {
            SequenceCommandTrack_l* other=findSequenceTrack(owner,first);
            if (other==nullptr || other==track) return;
            resetSequenceTrack(other);
            setSequenceTrackPosition(other,state->mSequenceBase,second);
            startSequenceTrack(other);
            return;
        }
        case 0x89: state->mCursor.mPosition=gabi::at<const be<u8>>(gabi::ea(state->mSequenceBase.get())+u32(first)); return;
        case 0x8A: {
            u32 depth=state->mStackDepth;
            if (depth>=10) return;
            SequenceCommandStackEntry& entry=state->mStack[depth];
            entry.mPosition=state->mCursor.mPosition;
            entry.mLoop=0;
            u32 next=static_cast<u32>(state->mStackDepth)+1;
            const be<u8>* base=state->mSequenceBase;
            state->mStackDepth=next;
            state->mCursor.mPosition=gabi::at<const be<u8>>(gabi::ea(base)+u32(first));
            return;
        }
        case 0xB0: player->mMasterByte=first; return;
        case 0xB1: state->mEnvelopeDelay=u8(first); return;
        case 0xB2:
            state->mReuseVoice=first!=0;
            if (!first) return;
            releaseSequenceTrackNotes(track,-1);
            updateSequenceTrackNotes(track);
            return;
        case 0xB3: state->mVelocityScale=first; return;
        case 0xB4: state->mBiquadType=first; return;
        case 0xB5: state->mBiquadValue=gabi::fdivs_ppc(f32(first),127.0f); return;
        case 0xB6: state->mReleasePriorityFixed=first; return;
        case 0xBD: state->mModulation[0].mScope=first; return;
        case 0xBE: state->mModulation[0].mRange=first; return;
        case 0xBF: state->mSynthFlag=first!=0; return;
        case 0xC0: restartCommandRamp(state->mPan,u32(first)-64,second,true); return;
        case 0xC1: restartCommandRamp(state->mVolume,first,second,false); return;
        case 0xC2: restartCommandRamp(player->mVolume,first,second,false); return;
        case 0xC3: state->mTranspose=first; return;
        case 0xC4: restartCommandRamp(state->mBend,first,second,true); return;
        case 0xC5: state->mNotePriorityBias=first; return;
        case 0xC6: state->mNotePriority=first; return;
        case 0xC7: state->mWaitForNotes=first!=0; return;
        case 0xC8:
            state->mNoteMode=first!=0;
            releaseSequenceTrackNotes(track,-1);
            updateSequenceTrackNotes(track);
            return;
        case 0xC9:
            state->mPortamento=1;
            state->mPortamentoKey=u32(first)+u32(static_cast<s8>(state->mTranspose));
            return;
        case 0xCA: state->mModulation[0].mDepth=gabi::fmuls_ppc(f32(u8(first)),0.0078125f); return;
        case 0xCB: state->mModulation[0].mSpeed=gabi::fmuls_ppc(f32(u8(first)),0.390625f); return;
        case 0xCC: state->mModulationRoute[0]=first; return;
        case 0xCD: state->mModulation[0].mType=first; return;
        case 0xCE: state->mPortamento=first!=0; return;
        case 0xCF: state->mPitchTime=first; return;
        case 0xD0: case 0xD1: case 0xD2: case 0xD3: state->mEnvelope[opcode-0xD0]=first; return;
        case 0xD4: {
            u32 depth=state->mStackDepth;
            if (depth>=10) return;
            SequenceCommandStackEntry& entry=state->mStack[depth];
            entry.mPosition=state->mCursor.mPosition;
            entry.mLoop=1;
            entry.mRemaining=first;
            state->mStackDepth=static_cast<u32>(state->mStackDepth)+1;
            return;
        }
        case 0xD5: restartCommandRamp(state->mSecondaryVolume,first,second,false); return;
        case 0xD6:
            if (*gabi::at<const be<u8>>(0x104D48E0)!=0)
                reinterpret_cast<SequenceArgumentReader*>(this)->findVariable(owner,track,first);
            return;
        case 0xD7: restartCommandRamp(state->mSignedRamp,first,second,true); return;
        case 0xD8: state->mCenteredControl=gabi::fmuls_ppc(f32(s32(u32(first)-64)),0.015625f); return;
        case 0xD9: state->mRoutingByte=first; return;
        case 0xDA: state->mEffectByte=first; return;
        case 0xDB: state->mControlByte=first; return;
        case 0xDC: state->mNotePan=u32(first)-64; return;
        case 0xDD: setSequenceMuteMode(track,first); return;
        case 0xDE: state->mMixByte=first; return;
        case 0xDF: state->mReleaseGuard=u8(first)>=64; return;
        case 0xE0: state->mModulation[0].mDelay=u32(first)*5; return;
        case 0xE1: player->mBoundedLevel=first>1023 ? 1023 : first<0 ? 0 : first; return;
        case 0xE3: state->mNotePitchOffset=gabi::fmuls_ppc(f32(first),0.015625f); return;
        case 0xE4: state->mModulation[0].mSpeed=s16(first)==0 ? 0.0f : gabi::fdivs_ppc(100.0f,f32(s16(first))); return;
        case 0xFB:
            state->mEnvelopeDelay=255;
            for (u32 index=0;index<4;++index) state->mEnvelope[index]=255;
            return;
        case 0xFD: {
            u32 depth=state->mStackDepth;
            if (!depth) return;
            SequenceCommandStackEntry* entry;
            do {
                depth=u8(depth-1);
                state->mStackDepth=depth;
                entry=gabi::at<SequenceCommandStackEntry>(gabi::ea(track)+0x10C+depth*8);
                if (entry->mLoop==0) {
                    if (entry!=nullptr) state->mCursor.mPosition=entry->mPosition;
                    return;
                }
            } while (depth!=0);
            return;
        }
        case 0xFC: {
            u32 depth=state->mStackDepth;
            if (!depth) return;
            SequenceCommandStackEntry* entry=gabi::at<SequenceCommandStackEntry>(gabi::ea(track)+0x104+depth*8);
            if (entry->mLoop==0) return;
            u32 remaining=entry->mRemaining;
            if (remaining!=0) {
                remaining=u8(remaining-1);
                if (remaining==0) { state->mStackDepth=depth-1; return; }
            }
            const be<u8>* position=entry->mPosition;
            entry->mRemaining=remaining;
            state->mCursor.mPosition=position;
            return;
        }
        default: return;
        }
    }
    if (opcode>0xFFFF) return;
    u32 command=opcode&0xFF;
    be<s16>* variable=nullptr;
    if ((opcode&0xF0)==0x80 || (opcode&0xF0)==0x90) {
        variable=const_cast<be<s16>*>(reinterpret_cast<SequenceArgumentReader*>(this)->findVariable(owner,track,first));
        if (variable==nullptr) return;
    }
    switch (command) {
    case 0x80: *variable=second; return;
    case 0x81: *variable=u32(static_cast<s16>(*variable))+u32(second); return;
    case 0x82: *variable=u32(static_cast<s16>(*variable))-u32(second); return;
    case 0x83: *variable=u32(static_cast<s16>(*variable))*u32(second); return;
    case 0x84: if (second!=0) *variable=commandDivide(*variable,s16(second)); return;
    case 0x85: {
        s32 value=*variable;
        u32 result;
        if (second>=0) result=(u32(second)&0x20) ? 0 : u32(value)<<(u32(second)&31);
        else { u32 shift=(0u-u32(second))&63; result=u32(value>>(shift>31 ? 31 : shift)); }
        *variable=result;
        return;
    }
    case 0x86: {
        bool negative=second<0;
        s32 bound=negative ? s16(0u-u32(second)) : second;
        u32 random=nextRandomHalfword();
        s32 value=s32(random*(u32(bound)+1))>>16;
        *variable=negative ? 0u-u32(value) : u32(value);
        return;
    }
    case 0x87: *variable=u32(static_cast<s16>(*variable))&u32(second); return;
    case 0x88: *variable=u32(static_cast<s16>(*variable))|u32(second); return;
    case 0x89: *variable=u32(static_cast<s16>(*variable))^u32(second); return;
    case 0x8A: *variable=~u32(second); return;
    case 0x8B:
        if (second!=0) { s32 value=*variable; *variable=u32(value)-u32(commandDivide(value,second))*u32(second); }
        return;
    case 0x90: state->mCondition=s32(*variable)==second; return;
    case 0x91: state->mCondition=s32(*variable)>=second; return;
    case 0x92: state->mCondition=s32(*variable)>second; return;
    case 0x93: state->mCondition=s32(*variable)<=second; return;
    case 0x94: state->mCondition=s32(*variable)<second; return;
    case 0x95: state->mCondition=s32(*variable)!=second; return;
    case 0xA0: case 0xA1: case 0xA2: case 0xA3: case 0xA5:
        setModulationCommand(state->mModulation[1],command-0xA0,first); return;
    case 0xA4: state->mModulationRoute[1]=first; return;
    case 0xA6: case 0xA7: case 0xA8: case 0xA9: case 0xAB:
        setModulationCommand(state->mModulation[2],command-0xA6,first); return;
    case 0xAA: state->mModulationRoute[2]=first; return;
    case 0xAC: case 0xAD: case 0xAE: case 0xAF: case 0xB1:
        setModulationCommand(state->mModulation[3],command-0xAC,first); return;
    case 0xB0: state->mModulationRoute[3]=first; return;
    case 0xE0: setSequencePlayerCommandE0(owner,u32(first)&0xFFFF,track); return;
    case 0xE1: case 0xE3: case 0xE5: state->mModulation[(command-0xDF)/2].mDelay=u32(first)*5; return;
    case 0xE2: case 0xE4: case 0xE6:
        state->mModulation[(command-0xE0)/2].mSpeed=s16(first)==0 ? 0.0f : gabi::fdivs_ppc(100.0f,f32(s16(first))); return;
    default: return;
    }
}
/* 028AD7B8 */
void SequenceCommandReader::emitNote(SequenceCommandTrack_l* track,u32 key,u32 velocity,s32 length,u32 mode) {
    gabi::AutoFrame frame{gabi::current_function()};
    emitSequenceNote(track,key,velocity,length,mode);
}
/* WWHD glue (nextRandomHalfword is bound in SequenceArgumentReader.cpp)
ext 0288DFE4 void emitSequenceNote(nw::snd::SequenceCommandTrack_l*,u32,u32,s32,u32)
ext 0288E5C4 void setSequenceMuteMode(nw::snd::SequenceCommandTrack_l*,s32)
ext 0288DBE8 void releaseSequenceTrackNotes(nw::snd::SequenceCommandTrack_l*,s32)
ext 0288DC60 void updateSequenceTrackNotes(nw::snd::SequenceCommandTrack_l*)
ext 0288A750 nw::snd::SequenceCommandTrack_l* findSequenceTrack(void*,s32)
ext 0288DCB8 void resetSequenceTrack(nw::snd::SequenceCommandTrack_l*)
ext 0288DD54 void setSequenceTrackPosition(nw::snd::SequenceCommandTrack_l*,const be<u8>*,s32)
ext 0288DD64 void startSequenceTrack(nw::snd::SequenceCommandTrack_l*)
ext 0288BF9C void setSequencePlayerCommandE0(void*,u32,nw::snd::SequenceCommandTrack_l*)
*/
