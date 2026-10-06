/* WWHD flame pillar, reconstructed from HD disassembly. */
#include "d/actor/d_a_obj_flame.h"
namespace daObjFlame {
void Act_c::set_switch() {
    WWHD_FUNC(0x02344974,void,this);
    if(mType!=1) return;
    s32 mode=mModeProc; bool active=mode==3||mode==4;
    s32 sw=prm(8,16); s32 room=home.roomNo;
    if(active) dComIfGs_onSwitch(sw,room); else dComIfGs_offSwitch(sw,room);
}
VERIFY(0x02344974,&Act_c::set_switch);
void Act_c::ki_init() {
    WWHD_FUNC(0x02344A30,void,this);
    s32 count=(s32)((u32)prm(5,8)+1);
    if(count!=32 && count>0) { mKiCount=0; mKiActive=1; mKiTimer=0; }
}
VERIFY(0x02344A30,&Act_c::ki_init);
void Act_c::mode_wait() {
    WWHD_FUNC(0x02344A8C,void,this);
    u8 schedule=prm(8,0);
    if(schedule) {
        if(!(gabi::call<u32>(0x025602A8)&schedule)) return;
        if(gabi::call<s32>(0x025602CC)) return;
    } else if(mTimer>0.0f) return;
    if(mType==1) {
        gabi::store<f32>((u32)mpBtk+4,0.0f); u32 brk=mpBrk; if(brk) gabi::store<f32>(brk+4,0.0f);
        mModeProc=2; mTimer=23.0f; ki_init();
        mEm0State=1; m457=1; mEm1State=1; mEm2State=1;
    } else { mModeProc=1; mTimer=127.0f; m457=1; mEm0State=1; mEm2State=1; mEm1State=1; }
}
VERIFY(0x02344A8C,&Act_c::mode_wait);
u32 Act_c::se_fireblast_omen() {
    WWHD_FUNC(0x02344B94,u32,this);
    s32 rev=dComIfGp_getReverb(current.roomNo); return gabi::call<u32>(0x025E1A40,0x704Eu,&current.pos,0u,(s32)(s8)rev); /* mDoAud_seStart: its result stays in r3 (mode_l_before tail-calls this) */
}
VERIFY(0x02344B94,&Act_c::se_fireblast_omen);
void Act_c::mode_wait2() {
    WWHD_FUNC(0x02344BDC,void,this);
    se_fireblast_omen(); if(mTimer>0.0f) return;
    gabi::store<f32>((u32)mpBtk+4,0.0f); u32 brk=mpBrk; if(brk) gabi::store<f32>(brk+4,0.0f);
    mModeProc=2; mTimer=23.0f; ki_init();
}
VERIFY(0x02344BDC,&Act_c::mode_wait2);
u32 Act_c::mode_l_before() {
    WWHD_FUNC(0x02344C50,u32,this);
    f32 timer=mTimer; m457=0;
    if(timer>0.0f) return se_fireblast_omen();
    s32 rev=mReverb; mModeProc=3; mTimer=22.0f; return gabi::call<u32>(0x025E1A40,0x380Du,&eyePos,0u,rev); /* mDoAud_seStart */
}
VERIFY(0x02344C50,&Act_c::mode_l_before);
void Act_c::mode_l_u() {
    WWHD_FUNC(0x02344CA4,void,this);
    f32 timer=mTimer; m457=1; mHeight=0.04545454680919647f*(22.0f-timer);
    if(timer>0.0f) return; mModeProc=4; mTimer=90.0f;
}
VERIFY(0x02344CA4,&Act_c::mode_l_u);
void Act_c::mode_u() {
    WWHD_FUNC(0x02344CF4,void,this);
    if(mTimer>0.0f) return; mModeProc=5; mEm0State=3; mTimer=25.0f;
}
VERIFY(0x02344CF4,&Act_c::mode_u);
void Act_c::mode_u_l() {
    WWHD_FUNC(0x02344D28,void,this);
    f32 timer=mTimer; m457=1; mHeight=0.03999999910593033f*timer;
    if(timer>0.0f) return; mModeProc=6; mTimer=20.0f; mEm2State=3; mEm1State=3;
}
VERIFY(0x02344D28,&Act_c::mode_u_l);
void Act_c::mode_l_after() {
    WWHD_FUNC(0x02344D78,void,this);
    if(mTimer>0.0f) return; mModeProc=0; mTimer=(u8)prm(8,0) ? 0.0f :120.0f;
}
VERIFY(0x02344D78,&Act_c::mode_l_after);
u32 PrmAbstract(const Act_c* actor,u32 width,u32 shift) {
    WWHD_FUNC(0x02345AC0,u32,actor,width,shift);
    u32 value=gabi::load<u32>(gabi::ea(actor)+0xB0);
    u32 mask=(width&32)?0u:1u<<(width&31);
    return ((shift&32)?0u:value>>(shift&31)) & (mask-1);
}
VERIFY(0x02345AC0,PrmAbstract);
}
namespace daObjFlame {
s32 Delete(Act_c* actor) {
    WWHD_FUNC(0x023455B0,s32,actor);
    gabi::call(0x025204C8,&actor->mPhs,gabi::at<u8>(0x101C94E8)); return 1;
}
VERIFY(0x023455B0,Delete);
void emptyVirtual(void* self) { WWHD_FUNC(0x02345A50,void,self); }
VERIFY(0x02345A50,emptyVirtual);
void deleteHelper(void* self,u32 flags) {
    WWHD_FUNC(0x02345A3C,void,self,flags);
    if(self && (flags&1)) gabi::call(0x0273AF40,self);
}
VERIFY(0x02345A3C,deleteHelper);
void destroyActor(Act_c* self,u32 flags) {
    WWHD_FUNC(0x02345A54,void,self,flags);
    if(!self) return;
    gabi::call(0x02515980,&self->mCps,2); gabi::call(0x02515860,&self->mStts,2);
    gabi::call(0x025D50BC,self,0); if(flags&1) gabi::call(0x0273AF40,self);
}
VERIFY(0x02345A54,destroyActor);
s32 cleanupEmitters(Act_c* actor) {
    WWHD_FUNC(0x02345890,s32,actor);
    u32 type=actor->mType;
    if(type>=4) { JUT_ASSERT_fail(STR(0x10028F90),0x1C0,STR(0x10028FA4)); type=actor->mType; }
    if(!gabi::load<u8>(0x101C9520+0x58*type)) {
        be<u32>* slots[]={&actor->mpEmitter0,&actor->mpEmitter1,&actor->mpEmitter2};
        for(auto slot:slots) {
            u32 em=*slot;
            if(em) { u32 flag=gabi::load<u32>(em+0x254); gabi::store<u32>(em+0x5C,0xFFFFFFFF); gabi::store<u32>(em+0x254,flag|1); *slot=0; }
        }
    }
    return 1;
}
VERIFY(0x02345890,cleanupEmitters);
void staticInit() {
    WWHD_FUNC(0x0234596C,void,(u32)0);
    for(s32 off=12;off>=0;off-=4) gabi::store<u32>(0x10469BAC+off,0);
    gabi::call(0x028F026C,gabi::at<u8>(0x101C9474));
    gabi::store<f32>(0x10469BA0,-3.1415927410125732f); gabi::store<f32>(0x10469BA4,3.1415927410125732f);
    gabi::call(0x028ED6F8,gabi::at<u8>(0x10469BA8)); gabi::call(0x028F026C,gabi::at<u8>(0x101C9480));
    gabi::call(0x028EAB2C,gabi::at<u8>(0x10469BA9)); gabi::call(0x028F026C,gabi::at<u8>(0x101C948C));
    gabi::store<f32>(0x101C9550,3.3333334922790527f); gabi::store<f32>(0x101C9558,0.8149999976158142f);
    gabi::store<f32>(0x101C9580,2.7166666984558105f); gabi::store<f32>(0x101C9588,2.7166666984558105f); gabi::store<f32>(0x101C9590,0.5433333516120911f);
}
VERIFY(0x0234596C,staticInit);
}
namespace daObjFlame {
void ki_make(Act_c* actor) {
    WWHD_FUNC(0x02345068,void,actor);
    if(!actor->mKiActive) return;
    if(actor->mKiDelay>0) { actor->mKiActive=0; actor->mKiDelay=(s32)((u32)(s32)actor->mKiDelay-1); return; }
    s32 count=(s32)((u32)actor->prm(5,8)+1); if(count==32) count=0;
    if(actor->mKiCount>=count) { actor->mKiActive=0; actor->mKiDelay=actor->prm(2,13); return; }
    s32 timer=(s32)((u32)(s32)actor->mKiTimer-1);
    if(timer>0) { actor->mKiTimer=timer; return; }
    actor->mKiTimer=5; actor->mKiCount=(s32)((u32)(s32)actor->mKiCount+1);
    f32 angle=cM_rndFX(32768.0f); gabi::Local<csXyz> rotation;
    gabi::call(0x0201A478,rotation.get(),0,(s16)gabi::ftoi(angle),0);
    s32 room=actor->current.roomNo;
    gabi::call(0x025D5834,0xD7,-32766,&actor->current.pos,room,rotation.get(),(u32)0,-1,(u32)0);
}
VERIFY(0x02345068,ki_make);
void eff_hase(Act_c* actor) {
    WWHD_FUNC(0x023451C8,void,actor);
    u32 play=gabi::ea(dComIfGp_get()); u32 pa=gabi::load<u32>(play+0x5AB0);
    gabi::call(0x025A8D40,pa,0xC06B,gabi::at<u8>(gabi::ea(actor)+0x2EC),255,gabi::at<u8>(0x101D5E98),gabi::at<u8>(0x101D5E98),0);
}
VERIFY(0x023451C8,eff_hase);
s32 Draw(Act_c* actor) {
    WWHD_FUNC(0x023457C8,s32,actor);
    auto env=dKy_getEnvlight(); gabi::call(0x025626A4,env,0,&actor->current.pos,&actor->tevStr);
    env=dKy_getEnvlight(); J3DModel* model=actor->mpModel; gabi::call(0x02562F5C,env,model,&actor->tevStr);
    model=actor->mpModel; u32 btk=actor->mpBtk; u32 data=gabi::load<u32>(gabi::ea(model)+0xAC); f32 frame=gabi::load<f32>(btk+4);
    gabi::call(0x025E7FC4,btk,data,frame);
    u32 brk=actor->mpBrk;
    if(brk) { model=actor->mpModel; frame=gabi::load<f32>(brk+4); data=gabi::load<u32>(gabi::ea(model)+0xAC); gabi::call(0x025E83FC,brk,data,frame); }
    u32 play=gabi::ea(dComIfGp_get()); gabi::store<u32>(0x104B4634,gabi::load<u32>(play+0x5D80));
    play=gabi::ea(dComIfGp_get()); gabi::store<u32>(0x104B4638,gabi::load<u32>(play+0x5D80));
    model=actor->mpModel; gabi::call(0x025E2DE0,model,0);
    play=gabi::ea(dComIfGp_get()); gabi::store<u32>(0x104B4634,gabi::load<u32>(play+0x5D78));
    play=gabi::ea(dComIfGp_get()); gabi::store<u32>(0x104B4638,gabi::load<u32>(play+0x5D7C)); return 1;
}
VERIFY(0x023457C8,Draw);
}
namespace daObjFlame {
void em_simple_inv(Act_c* actor) {
    WWHD_FUNC(0x023441EC,void,actor);
    s8 a=actor->mEm0State,b=actor->mEm1State; if(a==3) actor->mpEmitter0=0;
    s8 c=actor->mEm2State; if(b==3) actor->mpEmitter1=0; if(c==3) actor->mpEmitter2=0;
}
VERIFY(0x023441EC,em_simple_inv);
void em_manual_inv(Act_c* actor) {
    WWHD_FUNC(0x02344230,void,actor);
    u32 type=actor->mType;
    if(type>=4) { JUT_ASSERT_fail(STR(0x10028DC0),0x1C0,STR(0x10028DD4)); type=actor->mType; }
    bool enabled=gabi::load<u8>(0x101C9521+type*0x58)!=0; s8 first=actor->mEm0State;
    bool stop=enabled && actor->m459!=0; s8 last=actor->mEm2State;
    if(first==2 && stop) { first=3; actor->mEm0State=3; }
    if(last==2 && stop) { first=actor->mEm0State; actor->mEm2State=3; }
    auto remove=[](be<u32>& slot) { u32 em=slot; if(em) { u32 flag=gabi::load<u32>(em+0x254); gabi::store<u32>(em+0x5C,0xFFFFFFFF); gabi::store<u32>(em+0x254,flag|1); slot=0; } };
    if(first==3) remove(actor->mpEmitter0);
    if((s8)actor->mEm1State==3) remove(actor->mpEmitter1);
    if((s8)actor->mEm2State==3) remove(actor->mpEmitter2);
}
VERIFY(0x02344230,em_manual_inv);
}
namespace daObjFlame {
s32 Execute(Act_c* actor) {
    WWHD_FUNC(0x023455E0,s32,actor);
    auto attr=[&]() { u32 type=actor->mType; if(type>=4) { JUT_ASSERT_fail(STR(0x10028F58),0x1C0,STR(0x10028F6C)); type=actor->mType; } return 0x101C94F4+type*0x58; };
    u32 a=attr(); actor->scale.x=(f32)actor->mOrigScale.x*gabi::load<f32>(a+8);
    a=attr(); actor->scale.y=((f32)actor->mOrigScale.y*gabi::load<f32>(a+12))*(f32)actor->mExtraScaleY;
    a=attr(); actor->scale.z=(f32)actor->mOrigScale.z*gabi::load<f32>(a+8);
    gabi::call(0x02344DF4,actor); ki_make(actor);
    if(!actor->prm(1,31)) eff_hase(actor);
    s32 type=actor->mType; s16 rot=actor->mRotY;
    if(type==1) { f32 height=actor->mHeight; actor->mRotY=(s16)(rot+400); if(height>0.009999999776482582f) gabi::store<f32>(0x1047562C,1.0f); }
    else actor->mRotY=(s16)(rot+400);
    s32 mode=actor->mModeProc; actor->m459=0;
    if((u32)(mode-1)<=3) {
        gabi::call(0x025D5218,(u32)0x023443A0,actor);
        if(actor->mType==1) gabi::call(0x025D5218,(u32)0x023446B0,actor);
    }
    actor->set_switch(); gabi::call(0x02343AE4,actor); return 1;
}
VERIFY(0x023455E0,Execute);
}
namespace daObjFlame {
void set_mtx(Act_c* actor) {
    WWHD_FUNC(0x02343AE4,void,actor);
    f32 x=actor->scale.x,z=actor->scale.z,y=actor->scale.y; u32 model=gabi::ea((J3DModel*)actor->mpModel);
    gabi::store<f32>(model+0xBC,x); gabi::store<f32>(model+0xC0,y); gabi::store<f32>(model+0xC4,z);
    x=actor->current.pos.x; y=actor->current.pos.y; z=actor->current.pos.z;
    u32 mtx=0x1048D0CC; gabi::call(0x028E93CC,mtx,x,y,z);
    s16 rz=actor->shape_angle.z,rx=actor->shape_angle.x,ry=actor->shape_angle.y;
    gabi::call(0x025F1B48,mtx,rx,ry,rz); ry=actor->mRotY; gabi::call(0x025F1C28,mtx,ry);
    f32 matrix[12]; for(u32 i=0;i<12;i++) matrix[i]=gabi::load<f32>(mtx+4*i);
    model=gabi::ea((J3DModel*)actor->mpModel); for(u32 i=0;i<12;i++) gabi::store<f32>(model+0xC8+4*i,matrix[i]);
    u32 type=actor->mType;
    auto attr=[&]() { if(type>=4) { JUT_ASSERT_fail(STR(0x10028D0C),0x1C0,STR(0x10028D20)); type=actor->mType; } return 0x101C94F4+type*0x58; };
    u32 attrs[6]; for(u32 i=0;i<6;i++) attrs[i]=attr();
    f32 minx=gabi::load<s16>(attrs[0]+0x48),miny=gabi::load<s16>(attrs[1]+0x4A),minz=gabi::load<s16>(attrs[2]+0x4C);
    f32 maxx=gabi::load<s16>(attrs[3]+0x4E),maxy=gabi::load<s16>(attrs[4]+0x50),maxz=gabi::load<s16>(attrs[5]+0x52);
    f32 height=actor->mHeight; gabi::call(0x025D674C,actor,minx,miny,minz,maxx,maxy*height,maxz);
}
VERIFY(0x02343AE4,set_mtx);
void set_mtx_thunk(Act_c* actor) { WWHD_FUNC(0x02343D78,void,actor); set_mtx(actor); }
VERIFY(0x02343D78,set_mtx_thunk);
}
namespace daObjFlame {
void em_position(Act_c* actor) {
    WWHD_FUNC(0x02343D7C,void,actor);
    if(!(s8)actor->m457) return;
    u32 mtx=0x1048D0CC; f32 z=actor->current.pos.z,y=actor->current.pos.y,x=actor->current.pos.x;
    gabi::call(0x028E93CC,mtx,x,y,z);
    s16 rz=actor->shape_angle.z,rx=actor->shape_angle.x,ry=actor->shape_angle.y; gabi::call(0x025F1B48,mtx,rx,ry,rz);
    u32 type=actor->mType;
    auto attr=[&]() { if(type>=4) { JUT_ASSERT_fail(STR(0x10028D50),0x1C0,STR(0x10028D64)); type=actor->mType; } return 0x101C94F4+type*0x58; };
    u32 a=attr();
    if(!gabi::load<u8>(a+0x2C)) {
        if(actor->mpEmitter0) {
            gabi::call(0x025F23EC); type=actor->mType; u32 a0=attr(),a1=attr();
            f32 height=actor->mHeight,sy1=gabi::load<f32>(a1+12),sy0=gabi::load<f32>(a0+12),extra=actor->mExtraScaleY;
            f32 offset=gabi::fmadds((height*1500.0f)*sy0,extra,(-300.0f*sy1)*extra);
            gabi::call(0x025F24E0,0.0f,offset,0.0f);
            u32 em=actor->mpEmitter0; gabi::call(0x028249B0,mtx,em+0x1F0,em+0x22C); gabi::call(0x025F2468);
        }
        u32 em=actor->mpEmitter1; if(em) gabi::call(0x028249B0,mtx,em+0x1F0,em+0x22C);
        type=actor->mType;
    }
    a=attr(); f32 height=actor->mHeight,sy=gabi::load<f32>(a+4),ratio=actor->m46C;
    gabi::call(0x025F24E0,0.0f,((height*1500.0f)*sy)*ratio,0.0f);
    gabi::call(0x028E8F64,mtx,(u32)0x101FFBA8,&actor->eyePos);
    height=actor->mHeight; actor->mCollision=gabi::fmsubs(1500.0f,height,300.0f)>0.0f;
    if(!actor->mCollision) return;
    type=actor->mType; a=attr(); sy=gabi::load<f32>(a+4); ratio=actor->m46C;
    gabi::call(0x025F24E0,0.0f,(-300.0f*sy)*ratio,0.0f);
    gabi::call(0x028E8F64,mtx,(u32)0x101FFBA8,&actor->mCpsP1);
}
VERIFY(0x02343D7C,em_position);
}
namespace daObjFlame {
void em_simple_set(Act_c* actor) {
    WWHD_FUNC(0x02344014,void,actor);
    u32 type=actor->mType;
    auto attr=[&]() { if(type>=4) { JUT_ASSERT_fail(STR(0x10028D88),0x1C0,STR(0x10028D9C)); type=actor->mType; } return 0x101C94F4+type*0x58; };
    u32 a=attr(); s8 state=actor->mEm0State; bool blocked=gabi::load<u8>(a+0x2D)!=0 && actor->m459!=0;
    auto particle=[](u32 id,cXyz* pos) { u32 play=gabi::ea(dComIfGp_get()); u32 pa=gabi::load<u32>(play+0x5AB0); gabi::call(0x025A8D40,pa,id,pos,255,(u32)0x101D5E98,(u32)0x101D5E98,0); };
    if(state==1 && !blocked) {
        f32 x=actor->eyePos.x; a=attr(); f32 sy=gabi::load<f32>(a+4),extra=actor->mExtraScaleY,y=actor->eyePos.y,z=actor->eyePos.z;
        gabi::Local<cXyz> pos; pos->x=x; pos->y=gabi::fmadds(-300.0f*sy,extra,y); pos->z=z;
        particle(0x805A,pos.get());
    }
    if((s8)actor->mEm1State==1) particle(0x805B,&actor->eyePos);
    if((s8)actor->mEm2State==1 && !blocked) { type=actor->mType; a=attr(); u32 id=gabi::load<u16>(a+0x2A); particle(id,gabi::at<cXyz>(gabi::ea(actor)+0x2EC)); }
}
VERIFY(0x02344014,em_simple_set);
}
namespace daObjFlame {
void em_manual_set(Act_c* actor) {
    WWHD_FUNC(0x023432B4,void,actor);
    u32 type=actor->mType;
    auto attr=[&]() { if(type>=4) { JUT_ASSERT_fail(STR(0x10028C64),0x1C0,STR(0x10028C78)); type=actor->mType; } return 0x101C94F4+type*0x58; };
    u32 a=attr(); bool blocked=gabi::load<u8>(a+0x2D)!=0 && actor->m459!=0;
    auto particle=[&](u32 id,cXyz* scale) { u32 play=gabi::ea(dComIfGp_get()); u32 pa=gabi::load<u32>(play+0x5AB0); return gabi::call<u32>(0x025A847C,pa,0,id,gabi::at<cXyz>(gabi::ea(actor)+0x2EC),gabi::at<csXyz>(gabi::ea(actor)+0x2F8),scale,255,0,-1,0,0,0); };
    if((s8)actor->mEm0State==1 && !blocked && type!=1) {
        a=attr(); f32 x=gabi::load<f32>(a+0x30); a=attr(); f32 extra=actor->mExtraScaleY,y=gabi::load<f32>(a+0x34)*extra; a=attr(); f32 z=gabi::load<f32>(a+0x30);
        gabi::Local<cXyz> scale; scale->x=x; scale->y=y; scale->z=z; actor->mpEmitter0=particle(0x805A,scale.get()); actor->mEm0State=2;
    }
    if((s8)actor->mEm1State==1) {
        type=actor->mType; a=attr(); f32 x=gabi::load<f32>(a+0x38); a=attr(); f32 extra=actor->mExtraScaleY,y=gabi::load<f32>(a+0x3C)*extra; a=attr(); f32 z=gabi::load<f32>(a+0x38);
        gabi::Local<cXyz> scale; scale->x=x; scale->y=y; scale->z=z; actor->mpEmitter1=particle(0x805B,scale.get()); actor->mEm1State=2;
    }
    if((s8)actor->mEm2State==1 && !blocked) {
        type=actor->mType; a=attr(); f32 x=gabi::load<f32>(a+0x40); a=attr(); f32 y=gabi::load<f32>(a+0x40); a=attr(); f32 z=gabi::load<f32>(a+0x40);
        gabi::Local<cXyz> scale; scale->x=x; scale->y=y; scale->z=z; a=attr(); u32 id=gabi::load<u16>(a+0x2A); actor->mpEmitter2=particle(id,scale.get()); actor->mEm2State=2;
    }
}
VERIFY(0x023432B4,em_manual_set);
}
namespace daObjFlame {
void create_mode_init(Act_c* actor) {
    WWHD_FUNC(0x0234365C,void,actor);
    u32 schedule=(u8)actor->prm(8,0); u32 current=gabi::call<u8>(0x025602A8);
    if(!schedule || !current) { actor->mModeProc=0; actor->mTimer=schedule?0.0f:120.0f; actor->mHeight=0.0f; return; }
    s32 timer=gabi::call<s32>(0x025602CC); u32 play=gabi::ea(dComIfGp_get());
    u32 clock=gabi::load<u32>(play+0x5150); u32 getter=gabi::load<u32>(clock+0x15C);
    u32 config=gabi::call<u32>(getter,play+0x5150); u32 step=gabi::load<u8>(config+15)*30;
    if(schedule<current) schedule<<=8;
    u32 shifts=0; while(!(schedule&current)) { current<<=1; ++shifts; }
    timer=(s32)(step*shifts+(u32)timer);
    u32 type=actor->mType;
    auto attr=[&]() { if(type>=4) { JUT_ASSERT_fail(STR(0x10028CD4),0x1C0,STR(0x10028CE8)); type=actor->mType; } return 0x101C94F4+type*0x58; };
    u32 a=attr(); f32 phase=(f32)timer*gabi::load<f32>(a+0x24); if(type!=1) phase-=127.0f;
    s32 mode; f32 remain,height;
    if(phase<0.0f) { mode=1; remain=phase+127.0f; height=0.0f; }
    else if(phase<23.0f) { mode=2; remain=23.0f-phase; height=0.0f; }
    else if(phase<45.0f) { mode=3; remain=45.0f-phase; height=0.04545454680919647f*(22.0f-remain); }
    else if(phase<135.0f) { mode=4; remain=135.0f-phase; height=1.0f; }
    else if(phase<160.0f) { mode=5; remain=160.0f-phase; height=0.03999999910593033f*remain; }
    else if(phase<180.0f) { mode=6; remain=180.0f-phase; height=0.0f; }
    else { actor->mTimer=0.0f; actor->mModeProc=0; actor->mHeight=0.0f; return; }
    actor->mModeProc=mode; actor->mTimer=remain; actor->mHeight=height;
    if(mode!=1) {
        f32 fraction=phase*0.0055555556900799274f; u32 btk=actor->mpBtk;
        if(btk) gabi::store<f32>(btk+4,gabi::fmadds(fraction,(f32)gabi::load<s16>(btk+10),1.0f));
        u32 brk=actor->mpBrk; if(brk) gabi::store<f32>(brk+4,gabi::fmadds(fraction,(f32)gabi::load<s16>(brk+10),1.0f));
        mode=actor->mModeProc;
    }
    if(mode==0) return;
    u32 index=(u32)(mode-1);
    if(index<=3) { mode=actor->mModeProc; index=(u32)(mode-1); actor->mEm0State=1; }
    type=actor->mType; if(index<=4) { actor->mEm1State=1; actor->mEm2State=1; }
    a=attr(); if(!gabi::load<u8>(a+0x2C)) em_manual_set(actor); actor->m457=1;
}
VERIFY(0x0234365C,create_mode_init);
}
namespace daObjFlame {
void mode_proc_call(Act_c* actor) {
    WWHD_FUNC(0x02344DF4,void,actor);
    f32 timer=actor->mTimer;
    if(!(timer< -0.10000000149011612f)) {
        s32 mode=actor->mModeProc;
        if(mode==0 || mode==1) actor->mTimer=timer-1.0f;
        else {
            u32 type=actor->mType;
            if(type>=4) { JUT_ASSERT_fail(STR(0x10028EE4),0x1C0,STR(0x10028EF8)); timer=actor->mTimer; type=actor->mType; }
            actor->mTimer=timer-gabi::load<f32>(0x101C9518+type*0x58);
        }
    }
    u32 entry=0x10028EAC+8*(u32)(s32)actor->mModeProc;
    s16 index=gabi::load<s16>(entry+2),adjust=gabi::load<s16>(entry); u32 self=gabi::ea(actor)+(s32)adjust,target;
    if(index<0) target=gabi::load<u32>(entry+4);
    else { s16 vtOff=gabi::load<s16>(entry+6); u32 vt=gabi::load<u32>(self+(s32)vtOff); target=gabi::load<u32>(vt+8*(s32)index+4); }
    gabi::call(target,self);
    auto attr=[&]() { u32 type=actor->mType; if(type>=4) { JUT_ASSERT_fail(STR(0x10028EE4),0x1C0,STR(0x10028EF8)); type=actor->mType; } return 0x101C94F4+type*0x58; };
    u32 a=attr();
    if(gabi::load<u8>(a+0x2C)) { em_position(actor); em_simple_set(actor); em_simple_inv(actor); }
    else { em_manual_set(actor); em_manual_inv(actor); em_position(actor); }
    s32 mode=actor->mModeProc;
    if(mode!=0 && mode!=1) {
        gabi::call(0x025E742C,(u32)actor->mpBtk); u32 brk=actor->mpBrk; if(brk) gabi::call(0x025E742C,brk);
        s32 rev=actor->mReverb; mDoAud_seStart(0x7009,&actor->eyePos,0,rev);
    }
    if(!actor->mCollision) return;
    if(actor->mModeProc==5) { a=attr(); f32 height=actor->mHeight,limit=gabi::load<f32>(a+0x54); if(!(height>limit)) return; }
    gabi::call(0x020181FC,gabi::ea(actor)+0x514,&actor->mCpsP0);
    u32 play=gabi::ea(dComIfGp_get()); gabi::call(0x0200E240,play+0x26A4,&actor->mCps);
}
VERIFY(0x02344DF4,mode_proc_call);
}
namespace daObjFlame {
bool create_heap(Act_c* actor) {
    WWHD_FUNC(0x02342FC4,bool,actor);
    auto attr=[&]() { u32 type=actor->mType; if(type>=4) { JUT_ASSERT_fail(STR(0x10028C04),0x1C0,STR(0x10028C18)); type=actor->mType; } return 0x101C94F4+type*0x58; };
    struct Name { be<u32> text,vt; };
    auto resource=[&](u32 a,u32 off) { gabi::Local<Name> name; name->text=0x101C94E8; name->vt=0x10028BCC; u32 res=gabi::load<u32>(0x101F4F28),id=gabi::load<u32>(a+off); return gabi::call<u32>(0x026066C4,res,name.get(),id); };
    u32 a=attr(),data=resource(a,0x10);
    if(!data) JUT_ASSERT_fail(STR(0x10028C04),0x1F5,STR(0x10028C3C));
    u32 model=gabi::call<u32>(0x025E38E0,data,0,0x11020203); actor->mpModel=gabi::at<J3DModel>(model);
    a=attr(); u32 animation=resource(a,0x14); u32 btk=gabi::call<u32>(0x0273AD10,0x74);
    if(btk) btk=gabi::call<u32>(0x025E7C6C,btk); actor->mpBtk=btk;
    if(!animation) { JUT_ASSERT_fail(STR(0x10028C04),0x1FE,STR(0x10028C4C)); btk=actor->mpBtk; }
    s32 btkOK=0;
    if(btk) {
        u32 type=actor->mType; if(type>=4) { JUT_ASSERT_fail(STR(0x10028C04),0x1C0,STR(0x10028C18)); btk=actor->mpBtk; type=actor->mType; }
        f32 speed=gabi::load<f32>(0x101C9514+type*0x58);
        btkOK=gabi::call<s32>(0x025E7CE0,btk,data,animation,1,2,0,-1,0,0,speed);
    }
    a=attr(); s32 brkOK=0;
    if(gabi::load<s32>(a+0x18)<0) brkOK=1;
    else {
        // The original repeats its type assertion before the resource lookup.
        u32 type=actor->mType;
        if(type>=4) { JUT_ASSERT_fail(STR(0x10028C04),0x1C0,STR(0x10028C18)); type=actor->mType; a=0x101C94F4+type*0x58; }
        animation=resource(a,0x18); u32 brk=gabi::call<u32>(0x0273AD10,0x78);
        if(brk) brk=gabi::call<u32>(0x025E80D0,brk); actor->mpBrk=brk;
        if(!animation) { JUT_ASSERT_fail(STR(0x10028C04),0x214,STR(0x10028C58)); brk=actor->mpBrk; }
        if(brk) {
            type=actor->mType; if(type>=4) { JUT_ASSERT_fail(STR(0x10028C04),0x1C0,STR(0x10028C18)); brk=actor->mpBrk; type=actor->mType; }
            f32 speed=gabi::load<f32>(0x101C9514+type*0x58);
            brkOK=gabi::call<s32>(0x025E8154,brk,data,animation,1,2,0,-1,0,0,speed);
        }
    }
    return actor->mpModel!=nullptr && btkOK!=0 && brkOK!=0;
}
VERIFY(0x02342FC4,create_heap);
bool heap_thunk(Act_c* actor) { WWHD_FUNC(0x023432B0,bool,actor); return create_heap(actor); }
VERIFY(0x023432B0,heap_thunk);
}
namespace daObjFlame {
s32 create(Act_c* actor) {
    WWHD_FUNC(0x0234521C,s32,actor);
    u32 ea=gabi::ea(actor),flags=gabi::load<u32>(ea+0x2E4);
    if(!(flags&8)) {
        if(actor) {
            gabi::call(0x025D4ED0,actor); gabi::store<u32>(ea+0xB4,0x10028BF4);
            gabi::call(0x0200BD2C,&actor->mStts); gabi::call(0x02515DA0,ea+0x3DC);
            gabi::store<u32>(ea+0x3D8,0x1004AE88); gabi::store<u32>(ea+0x3DC,0x1004AEC0);
            gabi::call(0x02515FB8,&actor->mCps); gabi::store<u32>(ea+0x510,0x100015A8); gabi::store<u32>(ea+0x50C,0x10028BE4);
            gabi::call(0x02018150,ea+0x514); flags=gabi::load<u32>(ea+0x2E4);
            gabi::store<u32>(ea+0x438,0x1004AF18); gabi::store<u32>(ea+0x52C,0x1004AF60); gabi::store<u32>(ea+0x510,0x1004AF70);
        }
        gabi::store<u32>(ea+0x2E4,flags|8);
    }
    s32 result=gabi::call<s32>(0x02520460,&actor->mPhs,(u32)0x101C94E8);
    if(result!=4) return result;
    actor->mType=actor->prm(2,28);
    auto attr=[&]() { u32 type=actor->mType; if(type>=4) { JUT_ASSERT_fail(STR(0x10028F20),0x1C0,STR(0x10028F34)); type=actor->mType; } return 0x101C94F4+type*0x58; };
    u32 a=attr(); u32 heap=gabi::load<u32>(a+0x1C);
    if(!gabi::call<s32>(0x025D63E8,actor,(u32)0x023432B0,heap)) return 5;
    u32 x=gabi::load<u32>(ea+0x330),y=gabi::load<u32>(ea+0x334),z;
    actor->m46C=1.0f; gabi::store<u32>(ea+0x590,x); s32 type=actor->mType; z=gabi::load<u32>(ea+0x338);
    gabi::store<u32>(ea+0x594,y); gabi::store<u32>(ea+0x598,z); actor->mExtraScaleY=type==1?1.0004417896270752f:1.0f;
    a=attr(); actor->scale.x=(f32)actor->scale.x*gabi::load<f32>(a+8);
    a=attr(); f32 extra=actor->mExtraScaleY,sy=gabi::load<f32>(a+12),oldy=actor->scale.y; actor->scale.y=oldy*(sy*extra);
    a=attr(); f32 oldz=actor->scale.z,sz=gabi::load<f32>(a+8); actor->mEm2State=0; actor->mEm1State=0; actor->mEm0State=0; actor->scale.z=oldz*sz;
    create_mode_init(actor); actor->set_switch(); u32 model=gabi::ea((J3DModel*)actor->mpModel); gabi::store<u32>(ea+0x348,model?model+0xC8:0);
    set_mtx_thunk(actor); actor->mStts.Init(100,255,actor); gabi::call(0x025164C0,&actor->mCps,(u32)0x10028FE4);
    z=gabi::load<u32>(ea+0x31C); type=actor->mType; x=gabi::load<u32>(ea+0x314); y=gabi::load<u32>(ea+0x318);
    gabi::store<u32>(ea+0x534,x); gabi::store<u32>(ea+0x538,y); gabi::store<u32>(ea+0x53C,z);
    gabi::store<u32>(ea+0x540,x); gabi::store<u32>(ea+0x544,y); gabi::store<u32>(ea+0x440,ea+0x3C0); gabi::store<u32>(ea+0x548,z);
    a=attr(); f32 radius=gabi::load<f32>(a)*145.0f; actor->mCollision=0; actor->mCpsRad=radius; em_position(actor);
    s32 rev=dComIfGp_getReverb(actor->home.roomNo); actor->mReverb=rev; actor->m459=0;
    s32 bound=(s32)((u32)actor->prm(2,13)+1); f32 random=cM_rndF((f32)bound); s32 delay=gabi::ftoi(random),mode=actor->mModeProc;
    actor->mKiDelay=delay; if(mode!=0 && delay==0) actor->mKiDelay=1; return result;
}
VERIFY(0x0234521C,create);
}
namespace daObjFlame {
s32 liftup_magmarock(fopAc_ac_c* rock,Act_c* actor) {
    WWHD_FUNC(0x023443A0,s32,rock,actor);
    if(!gabi::call<s32>(0x025D4604,rock) || !rock || gabi::load<s16>(gabi::ea(rock)+8)!=0x2A) return 0;
    u32 id=rock?gabi::load<u32>(gabi::ea(rock)+4):0xFFFFFFFF;
    if(gabi::call<s32>(0x025DD868,id)) return 0;
    u32 type=actor->mType;
    auto attr=[&]() { if(type>=4) { JUT_ASSERT_fail(STR(0x10028E1C),0x1C0,STR(0x10028E30)); type=actor->mType; } return 0x101C94F4+type*0x58; };
    u32 a=attr(); f32 ey=actor->eyePos.y,base=actor->current.pos.y,radius=gabi::fmadds(gabi::load<f32>(a),145.0f,200.0f);
    bool upper=ey<base; f32 low=(upper?ey:base)-100.0f,high=(upper?base:ey)+1000.0f;
    gabi::Local<cXyz> pos,target; pos->x=rock->current.pos.x; pos->y=0.0f; pos->z=rock->current.pos.z;
    target->x=actor->eyePos.x; target->y=0.0f; target->z=actor->eyePos.z;
    f32 dist=gabi::call<f32>(0x028E8DE8,pos.get(),target.get()); dist=gabi::call<f32>(0x028F4384,dist);
    if(!(dist<radius)) return 0;
    f32 ry=rock->current.pos.y; if(!(ry>low) || !(ry<high)) return 0;
    type=actor->mType; if(type==1) return 0;
    f32 height=actor->mHeight,boost,offset=100.0f;
    if(height<0.10000000149011612f) boost=height*2700.0f;
    else if(height>0.8999999761581421f) { f32 fall=1.0f-height; boost=fall*2700.0f; offset=fall*1000.0f; }
    else boost=270.0f;
    a=attr(); f32 ratio=actor->m46C,scale=gabi::load<f32>(a+4); offset*=scale*ratio;
    a=attr(); scale=gabi::load<f32>(a+4); ratio=actor->m46C;
    f32 x=actor->eyePos.x,y=actor->eyePos.y,z=actor->eyePos.z; s32 mode=actor->mModeProc;
    gabi::Local<cXyz> destination; destination->x=x; destination->y=gabi::fmadds(boost,scale*ratio,y+offset); destination->z=z;
    u32 vt=gabi::load<u32>(gabi::ea(rock)+0xB4); u32 call=gabi::load<u32>(vt+((mode==1||mode==2)?0x1C:0x14));
    gabi::call(call,rock,destination.get()); actor->m459=1; return 0;
}
VERIFY(0x023443A0,liftup_magmarock);
s32 liftup_mflft(fopAc_ac_c* platform,Act_c* actor) {
    WWHD_FUNC(0x023446B0,s32,platform,actor);
    if(!gabi::call<s32>(0x025D4604,platform) || !platform || gabi::load<s16>(gabi::ea(platform)+8)!=0x5B) return 0;
    f32 desired=gabi::load<s16>(gabi::ea(platform)+0x3B6)==0?1.0004417896270752f:1.0f;
    f32 height=actor->mHeight,boost,offset;
    if(height<0.10000000149011612f) { boost=height*2700.0f; offset=100.0f; }
    else if(height>0.8999999761581421f) { f32 fall=1.0f-height; offset=fall*1000.0f; boost=fall*2700.0f; }
    else { offset=100.0f; boost=270.0f; }
    u32 type=actor->mType;
    auto attr=[&]() { if(type>=4) { JUT_ASSERT_fail(STR(0x10028E64),0x1C0,STR(0x10028E78)); type=actor->mType; } return 0x101C94F4+type*0x58; };
    u32 a=attr(); f32 ratio=actor->m46C,scale=gabi::load<f32>(a+4); offset*=scale*ratio;
    a=attr(); ratio=actor->m46C; scale=gabi::load<f32>(a+4); f32 y=actor->eyePos.y,base=actor->current.pos.y,x=actor->eyePos.x,z=actor->eyePos.z;
    y=gabi::fmadds(boost,scale*ratio,y+offset); if(y>base+5000.0f) y=base+5000.0f;
    s32 mode=actor->mModeProc;
    if(mode!=1 && mode!=2) { gabi::Local<cXyz> destination; destination->x=x; destination->y=y; destination->z=z; u32 vt=gabi::load<u32>(gabi::ea(platform)+0xB4); u32 call=gabi::load<u32>(vt+0x14); gabi::call(call,platform,destination.get()); }
    actor->m459=1; gabi::call(0x0200ECD4,&actor->m46C,1.0f,0.30000001192092896f,0.10000000149011612f,0.009999999776482582f);
    gabi::call(0x0200ECD4,&actor->mExtraScaleY,desired,0.30000001192092896f,0.10000000149011612f,0.009999999776482582f); return 0;
}
VERIFY(0x023446B0,liftup_mflft);
}
