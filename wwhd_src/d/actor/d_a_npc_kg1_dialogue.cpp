/* Salvatore (Squid-Hunt), reconstructed from WWHD. */
#include "d/actor/d_a_npc_kg1.h"

template<class T> static T readAt(u32 p,u32 off) { return gabi::load<T>(p+off); }
template<class T> static void writeAt(u32 p,u32 off,T v) { gabi::store<T>(p+off,v); }
static u32 playObject() { return gabi::call<u32>(0x025200D4); }
static u32 modelOf(daNpc_Kg1_c* a) { return readAt<u32>(readAt<u32>(gabi::ea(a),0x44C),0x90); }
static u32 saveEvent(u32 offset=0x644) { return gabi::load<u32>(0x101F84DC)+offset; }
static void resetTalkEvent() {
    u32 play=playObject(); writeAt<u16>(play,0x52B8,readAt<u16>(play,0x52B8)|8);
}
void daNpc_Kg1_wait_action(daNpc_Kg1_c* a) {
    WWHD_FUNC(0x02266D60,void,a);
    u32 p=gabi::ea(a),play=playObject();
    s32 staff=gabi::call<s32>(0x02542D88,play+0x52C4,STR(0x1001BEC8),0,0);
    gabi::Local<be<s16>> process; *process=0x3E;
    u32 board=gabi::call<u32>(0x025D5218,0x025E121C,process.get());
    switch(a->mWaitMode) {
    case 0: {
        writeAt<u8>(p,0x97D,0); writeAt<u8>(p,0x3B7,1); writeAt<u8>(p,0x3B8,1);
        if(board) gabi::call(0x021C5CD4,board);
        gabi::call(0x02265DC8,a);
        u8 attentive=readAt<u8>(p,0x95B),talk=a->mTalkActive;
        a->mEventOrder=attentive!=0;
        if(!talk) break;
        a->mWaitMode=1; writeAt<f32>(p,0x7E4,0.0f);
        [[fallthrough]];
    }
    case 1: {
        s32 status=gabi::call<s32>(0x025A11EC,a,1);
        u32 message=readAt<u32>(p,0x7C0);
        bool speaking=true;
        if(message<0x1D56 && a->mRequestedAnimation==4) speaking=false;
        message=readAt<u32>(p,0x7C0); writeAt<u8>(p,0x3B7,(u8)speaking);
        if(message==0x1D5B) writeAt<u8>(p,0x97C,0);
        else if((message==0x1D5C || message==0x1D57) && status==6) writeAt<u8>(p,0x97C,1);
        if(status==0x12) {
            resetTalkEvent();
            if(a->mSequenceFlags[0]) {
                writeAt<u8>(p,0x97D,0); a->mTalkActive=0; a->mEventOrder=3; a->mWaitMode=2;
            } else { a->mTalkActive=0; a->mWaitMode=0; a->mRequestedAnimation=2; }
        }
        break;
    }
    case 2: {
        writeAt<u8>(p,0x97D,0);
        if(board) { gabi::call(0x021C5C8C,board); gabi::call(0x021C5CC8,board); }
        s8 room=readAt<s8>(p,0x326); a->mWaitMode=3;
        s32 reverb=gabi::call<s32>(0x02520540,room);
        gabi::call(0x025E1A40,0x8A8,p+0x37C,0,reverb);
        writeAt<u8>(p,0x9A7,60); break;
    }
    case 3: {
        if(a->mEventOrder || !board) break;
        u32 finished=gabi::call<u32>(0x021C5CB8,board); a->mSequenceFlags[2]=finished!=0;
        if(!finished) break;
        if(gabi::call<u32>(0x0207A9A0,p+0x9A7)) break;
        u32 success=gabi::call<u32>(0x021C5C74,board); a->mSequenceFlags[1]=(u8)success;
        if(success) {
            u32 score=gabi::call<u32>(0x021C5C84,board); writeAt<u8>(p,0x9A6,(u8)score);
            writeAt<u8>(p,0x9A4,0); a->mWaitMode=4; writeAt<u8>(p,0x9A7,90);
        } else {
            writeAt<u8>(p,0x9A4,0); a->mRequestedAnimation=4; writeAt<u8>(p,0x9A7,90); a->mWaitMode=4;
        }
        play=playObject(); writeAt<u16>(play,0x52A6,readAt<u16>(play,0x52A6)&0xFFFD);
        writeAt<u8>(p,0x9A5,1); break;
    }
    case 4:
        if(!gabi::call<u32>(0x02007898,0)) break;
        writeAt<u8>(p,0x9A5,0); gabi::call(0x025B8B68,saveEvent(),0x2540);
        if(board) { writeAt<u8>(board,0x6E3,1); gabi::call(0x021C5CD4,board); }
        a->mWaitMode=5; a->mEventOrder=2;
        play=playObject(); gabi::call(0x02543280,play+0x52C4,staff); resetTalkEvent(); break;
    case 5:
        if(a->mTalkActive!=1) break;
        writeAt<u8>(p,0x97D,1);
        if(gabi::call<s32>(0x025A11EC,a,1)!=0x12) break;
        a->mEventOrder=0; resetTalkEvent();
        { u8 success=a->mSequenceFlags[1]; a->mTalkActive=0;
          if(success) { a->mEventOrder=5; a->mWaitMode=6; }
          else if(a->mSequenceFlags[0]) { a->mEventOrder=3; a->mWaitMode=2; }
          else { a->mWaitMode=0; a->mRequestedAnimation=2; } }
        break;
    case 6: {
        if(a->mEventOrder) break;
        writeAt<u8>(p,0x97D,0);
        struct Rewards_l { be<u8> values[3]; }; gabi::Local<Rewards_l> rewards;
        rewards->values[0]=7; rewards->values[1]=0xCC; rewards->values[2]=5;
        u32 wins=gabi::call<u32>(0x025B8BB0,saveEvent(),0xFE07);
        u32 index=(u8)(wins-1);
        if(index>=3) { gabi::call(0x0273AA24,STR(0x1001BECC),0x3C8,STR(0x1001BEDC)); index=2; }
        s8 room=readAt<s8>(p,0x326); u8 item=gabi::load<u8>(gabi::ea(rewards.get())+index);
        u32 present=gabi::call<u32>(0x025D7DEC,p+0x314,item,0,-1,room,0,0);
        writeAt<u32>(p,0x9A8,present); if(present!=0xFFFFFFFF) writeAt<u32>(playObject(),0x52A0,present);
        a->mWaitMode=7; break;
    }
    case 7: {
        s16 event=a->mEventIds[2]; play=playObject();
        if(!gabi::call<u32>(0x025440C8,play+0x52C4,event)) break;
        resetTalkEvent(); a->mEventOrder=2; a->mRequestedAnimation=11; a->mWaitMode=8; a->mSequenceFlags[3]=1; break;
    }
    case 8:
        writeAt<u8>(p,0x3B8,1); writeAt<u8>(p,0x3B7,1);
        if(a->mTalkActive==1) {
            bool wasReward=a->mRequestedAnimation==11; writeAt<u8>(p,0x97D,1);
            if(gabi::call<s32>(0x025A11EC,a,1)==0x12) {
                resetTalkEvent(); u8 highscore=a->mSequenceFlags[5]; a->mTalkActive=0; a->mEventOrder=0;
                if(highscore) { a->mEventOrder=5; a->mWaitMode=9; }
                else { a->mWaitMode=0; a->mRequestedAnimation=2; }
            } else if(wasReward && a->mRequestedAnimation==4) a->mRequestedAnimation=11;
        }
        break;
    case 9: {
        if(a->mEventOrder) break;
        writeAt<u8>(p,0x97D,0);
        struct Rewards_l { be<u8> values[2]; }; gabi::Local<Rewards_l> rewards;
        rewards->values[0]=0xF1; rewards->values[1]=6;
        u32 wins=gabi::call<u32>(0x025B8BB0,saveEvent(),0xFF07),index=(u8)(wins-1);
        s8 room=readAt<s8>(p,0x326); if(index>1) index=1;
        u8 item=gabi::load<u8>(gabi::ea(rewards.get())+index);
        u32 present=gabi::call<u32>(0x025D7DEC,p+0x314,item,0,-1,room,0,0);
        writeAt<u32>(p,0x9A8,present); if(present!=0xFFFFFFFF) writeAt<u32>(playObject(),0x52A0,present);
        a->mWaitMode=10; break;
    }
    case 10: {
        s16 event=a->mEventIds[2]; play=playObject();
        if(!gabi::call<u32>(0x025440C8,play+0x52C4,event)) break;
        resetTalkEvent(); a->mEventOrder=2; a->mSequenceFlags[4]=1; a->mWaitMode=11; break;
    }
    case 11:
        if(a->mTalkActive==1) {
            writeAt<u8>(p,0x97D,1);
            if(gabi::call<s32>(0x025A11EC,a,1)==0x12) {
                resetTalkEvent(); a->mRequestedAnimation=2; a->mEventOrder=0; a->mWaitMode=0; a->mTalkActive=0;
            }
        }
        break;
    }
    gabi::call(0x02266C90,a);
}
VERIFY(0x02266D60,daNpc_Kg1_wait_action);
