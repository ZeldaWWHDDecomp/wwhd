#include "d/actor/d_a_kddoor.h"
namespace Kddoor {
static BOOL nodeCallback(void*,s32);
static BOOL makeKey(Actor*); static BOOL makeStop(Actor*); static BOOL drawSet(Arm*);
static void calcMatrix(Actor*); static void armMatrix(Arm*,DoorInfo*,f32,f32,u32); static void stopMatrix(Stop*,DoorInfo*);
static BOOL createHeap(Actor*); static BOOL heapCallback(Actor*);
static void openInit(Actor*); static void closeInit(Actor*); static s32 stopFront(Actor*); static s32 stopBack(Actor*); static void setStop(Actor*);
static void armCloseInit(Arm*); static void stopCloseInit(Stop*); static void armOpenInit(Arm*); static void stopOpenInit(Stop*);
static BOOL openProc(Actor*); static void openEnd(Actor*); static BOOL closeProc(Actor*); static void closeEnd(Actor*);
static BOOL armCloseProc(Arm*,DoorInfo*); static BOOL stopCloseProc(Stop*,DoorInfo*); static BOOL armOpenProc(Arm*,DoorInfo*); static BOOL stopOpenProc(Stop*,DoorInfo*);
static void demoProc(Actor*); static BOOL genocideCase(Actor*); static BOOL stopOpen(Actor*); static void setStopDemo(Actor*); static BOOL feelerCase(Actor*); static BOOL stopClose(Actor*);
static void setEvent(Actor*); static BOOL actionWait(Actor*); static BOOL actionStopClose(Actor*); static BOOL actionDemo(Actor*); static void setKey(Actor*); static BOOL actionInit(Actor*);
static void stopDraw(Stop*,DoorInfo*); static BOOL draw(Actor*); static BOOL drawWrapper(Actor*); static void stopExecute(Stop*,DoorInfo*); static BOOL execute(Actor*); static BOOL isDelete(Actor*);
static void armEnd(Arm*); static void stopEnd(Stop*); static BOOL remove(Actor*); static Arm* armConstruct(Arm*); static Stop* stopConstruct(Stop*);
static void armInit(Arm*); static void stopInit(Stop*,DoorInfo*); static BOOL createInit(Actor*); static s32 create(Actor*); static s32 createWrapper(Actor*);
static void sinit(); static void stringDtor(void*,u32); static void armDtor(Arm*,u32); static void actorDtor(Actor*,u32); static void stringTerminate(void*);
static u8 type(Actor* a) { return gabi::call<u8>(0x0252AA2C,a); }
static u8 frontSwitch(Actor* a) { return gabi::call<u8>(0x0252AA14,a); }
static u8 backSwitch(Actor* a) { return gabi::call<u8>(0x0252AA20,a); }
static u8 frontRoom(Actor* a) { return gabi::call<u8>(0x0252A37C,a); }
static u8 backRoom(Actor* a) { return gabi::call<u8>(0x0252A388,a); }
static BOOL switchOn(u8 sw,u8 room) { u32 save=gabi::load<u32>(0x101F84DC); return gabi::call<BOOL>(0x025BA0C0,gabi::at<void>(save+0x20),sw,room); }
static void setSwitch(u8 sw,u8 room) { u32 save=gabi::load<u32>(0x101F84DC); gabi::call(0x025B9E38,gabi::at<void>(save+0x20),sw,room); }
static bool roomActive(u8 room) { dComIfGp_get(); return (gabi::load<u8>(0x1047E8E8+(u32)room*0x22C)&1)!=0; }
static void sound(DoorInfo* a,u32 id) { if(a && gabi::ea(&a->eyePos)) { s8 room=a->current.roomNo; s32 reverb=dComIfGp_getReverb(room); gabi::call(0x025E1A40,id,&a->eyePos,0,reverb); } }
static void copyMatrix(u32 src,u32 dst) { f32 values[12]; for(u32 i=0;i<12;++i) values[i]=gabi::load<f32>(src+4*i); for(u32 i=0;i<12;++i) gabi::store<f32>(dst+4*i,values[i]); }
static u32 jointMatrix(u32 model,u32 joint) { u32 block=gabi::load<u32>(model+0x2C); u16 flags=gabi::load<u16>(block+4); gabi::store<u16>(block+4,flags|0x10); return gabi::load<u32>(block+0x10)+joint*0x30; }
static BOOL nodeCallback(void* joint,s32 stage) {
    WWHD_FUNC(0x0219623C,BOOL,joint,stage);
    if(stage==0) {
        auto jointInfo=gabi::call<void*>(0x027F7878,joint); u32 model=gabi::load<u32>(0x104B462C); u32 arm=gabi::load<u32>(model+0xB8); u16 index=gabi::load<u16>(gabi::ea(jointInfo)+4);
        if(arm && (u32)index-1<4) {
            u32 source=jointMatrix(model,index),matrix=gabi::load<u32>(0x1018C7B0); gabi::call(0x028E90D4,gabi::at<void>(source),gabi::at<void>(matrix));
            f32 amplitude=gabi::fmadds((f32)index,1000.0f,500.0f); s16 phase=gabi::load<s16>(arm+0x196);
            u16 angle=(u16)((s32)phase*2000-(s32)index*14000); f32 sine=gabi::load<f32>(0x104A44F8+((u32)angle>>3)*8);
            s16 rotation=(s16)gabi::ftoi(gabi::fmuls_ppc(sine,amplitude)); matrix=gabi::load<u32>(0x1018C7B0); gabi::call(0x025F1C28,gabi::at<void>(matrix),rotation);
            phase=gabi::load<s16>(arm+0x196); angle=(u16)((s32)phase*1500-(s32)index*14000); sine=gabi::load<f32>(0x104A44F8+((u32)angle>>3)*8);
            rotation=(s16)gabi::ftoi(gabi::fmuls_ppc(sine,amplitude)); matrix=gabi::load<u32>(0x1018C7B0); gabi::call(0x025F1C5C,gabi::at<void>(matrix),rotation);
            u32 destination=jointMatrix(model,index); matrix=gabi::load<u32>(0x1018C7B0); copyMatrix(matrix,destination);
            matrix=gabi::load<u32>(0x1018C7B0); gabi::call(0x028E90D4,gabi::at<void>(matrix),gabi::at<void>(0x104B4868));
        }
    }
    return TRUE;
}
VERIFY(0x0219623C,nodeCallback);
static BOOL makeKey(Actor* a) { WWHD_FUNC(0x0219641C,BOOL,a); return type(a)==2; }
VERIFY(0x0219641C,makeKey);
static BOOL makeStop(Actor* a) { WWHD_FUNC(0x02196448,BOOL,a); if(backSwitch(a)!=0xFF) return TRUE; if(makeKey(a)) return FALSE; return frontSwitch(a)!=0xFF; }
VERIFY(0x02196448,makeStop);
static s32 stopFront(Actor* a) { WWHD_FUNC(0x02196D2C,s32,a); u8 kind=type(a),sw=frontSwitch(a),room=frontRoom(a); if(sw==0xFF) return 0; if(kind>1) return 0; if(!roomActive(room)) return -1; return !switchOn(sw,room); }
VERIFY(0x02196D2C,stopFront);
static s32 stopBack(Actor* a) { WWHD_FUNC(0x02196E10,s32,a); u8 sw=backSwitch(a),room=backRoom(a); if(sw==0xFF) return 0; if(!roomActive(room)) return -1; return !switchOn(sw,room); }
VERIFY(0x02196E10,stopBack);
static void setStop(Actor* a) { WWHD_FUNC(0x02196EE0,void,a); if(makeStop(a)) { u8 front=a->front; a->stop.side=front; if(!front) { a->stop.enabled=(u8)stopFront(a); a->stop.otherEnabled=(u8)stopBack(a); } else { a->stop.enabled=(u8)stopBack(a); a->stop.otherEnabled=(u8)stopFront(a); } } }
VERIFY(0x02196EE0,setStop);
static BOOL genocideCase(Actor* a) { WWHD_FUNC(0x02197D2C,BOOL,a); u8 kind=type(a); return !a->front?kind==1:kind==2||kind==4; }
VERIFY(0x02197D2C,genocideCase);
static void setStopDemo(Actor* a) { WWHD_FUNC(0x02197F14,void,a); a->action=(u8)a->front!=0; }
VERIFY(0x02197F14,setStopDemo);
static BOOL feelerCase(Actor* a) { WWHD_FUNC(0x02197F28,BOOL,a); u8 kind=type(a); return !a->front && kind>=3 && kind<=4; }
VERIFY(0x02197F28,feelerCase);
static BOOL stopOpen(Actor* a) {
    WWHD_FUNC(0x02197D98,BOOL,a);
    u8 sw,room; if(!a->front) { sw=frontSwitch(a); room=frontRoom(a); } else { sw=backSwitch(a); room=backRoom(a); }
    if(genocideCase(a)) {
        auto play=dComIfGp_get(); if(gabi::load<u8>(gabi::ea(play)+0x5292)) return FALSE;
        play=dComIfGp_get(); BOOL visible=gabi::call<BOOL>(0x025C4334,gabi::at<void>(gabi::ea(play)+0x51CC),room);
        if(visible && !gabi::call<void*>(0x025D98E8,(s8)room)) { u8 timer=a->enemyTimer; if(timer) { a->enemyTimer=timer-1; return FALSE; } if(sw!=0xFF) setSwitch(sw,room); return TRUE; }
        a->enemyTimer=0x41; return FALSE;
    }
    return sw!=0xFF && switchOn(sw,room);
}
VERIFY(0x02197D98,stopOpen);
static BOOL stopClose(Actor* a) {
    WWHD_FUNC(0x02197F8C,BOOL,a); u8 kind=type(a); if(genocideCase(a)) return FALSE; if(feelerCase(a)) return FALSE;
    u8 sw,room; if(!a->front) { if(kind>=3 && kind<=4) return FALSE; sw=frontSwitch(a); room=frontRoom(a); } else { sw=backSwitch(a); room=backRoom(a); }
    if(sw==0xFF) return FALSE; return !switchOn(sw,room);
}
VERIFY(0x02197F8C,stopClose);
static BOOL isDelete(Actor* a) { WWHD_FUNC(0x02198978,BOOL,a); return TRUE; }
VERIFY(0x02198978,isDelete);
static void armEnd(Arm* a) { WWHD_FUNC(0x02198980,void,a); u32 smoke=gabi::ea(a)+8; u32 vt=gabi::load<u32>(smoke); gabi::call_ptr(gabi::load<u32>(vt+0x44),gabi::at<void>(smoke)); }
VERIFY(0x02198980,armEnd);
static void stopEnd(Stop* s) { WWHD_FUNC(0x02198990,void,s); for(u32 i=0;i<3;++i) armEnd(&s->arms[i]); }
VERIFY(0x02198990,stopEnd);
static void stringDtor(void* self,u32 flags) { WWHD_FUNC(0x021990B4,void,self,flags); if(self && (flags&1)) gabi::call(0x0273AF40,self); }
VERIFY(0x021990B4,stringDtor);
static void armDtor(Arm* a,u32 flags) { WWHD_FUNC(0x021990C8,void,a,flags); if(a) { gabi::call(0x02515A70,gabi::at<void>(gabi::ea(a)+0x64),2); gabi::call(0x02515860,&a->status,2); if(flags&1) gabi::call(0x0273AF40,a); } }
VERIFY(0x021990C8,armDtor);
static void actorDtor(Actor* a,u32 flags) { WWHD_FUNC(0x02199128,void,a,flags); if(a) { gabi::call(0x028F0164,&a->stop,3,0x1C4,gabi::at<void>(0x021990C8),0,0); gabi::call(0x027F3628,gabi::at<void>(gabi::ea(a)+0x444),0); gabi::call(0x025D50BC,a,0); if(flags&1) gabi::call(0x0273AF40,a); } }
VERIFY(0x02199128,actorDtor);
static void stringTerminate(void* self) { WWHD_FUNC(0x021991A8,void,self); }
VERIFY(0x021991A8,stringTerminate);
static void calcMatrix(Actor* a) {
    WWHD_FUNC(0x021966A0,void,a);
    u32 matrix=0x1048D0CC;
    gabi::call(0x028E93CC,gabi::at<void>(matrix), (f32)a->current.pos.x, (f32)a->current.pos.y, (f32)a->current.pos.z);
    gabi::call(0x025F1C28,gabi::at<void>(matrix), (s16)a->home.angle.y);
    gabi::call(0x025F24E0,0.0f, (f32)a->travel,0.0f);
    copyMatrix(matrix,gabi::ea(a->model.get())+0xC8);
}
VERIFY(0x021966A0,calcMatrix);
static void armCloseInit(Arm* a) {
    WWHD_FUNC(0x02196F54,void,a); a->scale.set(0,0,0); a->position.set(0,0,0);
    f32 random=gabi::call<f32>(0x02019788); a->started=0; a->angle=(s16)gabi::ftoi(gabi::fmuls_ppc(random,5.0f));
}
VERIFY(0x02196F54,armCloseInit);
static void stopCloseInit(Stop* s) { WWHD_FUNC(0x02196FC8,void,s); s->moving=1; for(u32 i=0;i<3;++i) armCloseInit(&s->arms[i]); }
VERIFY(0x02196FC8,stopCloseInit);
static void armOpenInit(Arm* a) {
    WWHD_FUNC(0x02197018,void,a); a->scale.set(1,1,1); a->position.set(1,1,1);
    f32 random=gabi::call<f32>(0x02019788); a->started=0; a->angle=(s16)gabi::ftoi(gabi::fmuls_ppc(random,5.0f));
}
VERIFY(0x02197018,armOpenInit);
static void stopOpenInit(Stop* s) { WWHD_FUNC(0x0219708C,void,s); s->moving=1; for(u32 i=0;i<3;++i) armOpenInit(&s->arms[i]); }
VERIFY(0x0219708C,stopOpenInit);
static void openInit(Actor* a) {
    WWHD_FUNC(0x02196BFC,void,a); gabi::call(0x0252A3A0,a,1); a->flags|=1;
    auto play=dComIfGp_get(); gabi::call(0x020087EC,gabi::at<void>(gabi::ea(play)+0x12A0),a->background.get());
    a->travel=0; a->speedF=0; sound(a,0x6907);
}
VERIFY(0x02196BFC,openInit);
static void closeInit(Actor* a) {
    WWHD_FUNC(0x02196C88,void,a); a->flags|=2; auto play=dComIfGp_get();
    if(gabi::call<s32>(0x024EEA6C,gabi::at<void>(gabi::ea(play)+0x12A0),a->background.get(),a))
        gabi::call(0x0273AA24,gabi::at<void>(0x10012ACC),0x41E,gabi::at<void>(0x10012AC8));
    gabi::store<u8>(0x1047A964,0); sound(a,0x6908);
}
VERIFY(0x02196C88,closeInit);
static BOOL openProc(Actor* a) {
    WWHD_FUNC(0x021970DC,BOOL,a); gabi::call(0x0200F5C8,&a->speedF,30.0f,4.0f);
    BOOL done=gabi::call<BOOL>(0x0200F5C8,&a->travel,300.0f, (f32)a->speedF)!=0; calcMatrix(a); return done;
}
VERIFY(0x021970DC,openProc);
static void openEnd(Actor* a) { WWHD_FUNC(0x02197154,void,a); sound(a,0x6909); u16 flags=a->flags; a->speedF=0; a->travel=300; a->flags=flags&~1; }
VERIFY(0x02197154,openEnd);
static BOOL closeProc(Actor* a) {
    WWHD_FUNC(0x021971CC,BOOL,a); gabi::call(0x0200F5C8,&a->speedF,60.0f,6.0f);
    BOOL done=gabi::call<BOOL>(0x0200F5C8,&a->travel,0.0f, (f32)a->speedF)!=0; calcMatrix(a); return done;
}
VERIFY(0x021971CC,closeProc);
static void closeEnd(Actor* a) {
    WWHD_FUNC(0x02197244,void,a); gabi::Local<cXyz> shock; gabi::Local<u8[16]> callerLinkage;
    a->flags&=~2; gabi::call(0x0252A550,a); auto play=dComIfGp_get(); shock->set(0,1,0);
    gabi::call(0x025CB374,gabi::at<void>(gabi::ea(play)+0x599C),4,-33,shock.get()); sound(a,0x690A);
}
VERIFY(0x02197244,closeEnd);
static void armInit(Arm* a) {
    WWHD_FUNC(0x02198D48,void,a); gabi::call(0x02515F14,&a->status,255,255,gabi::at<void>(0));
    gabi::call(0x02516518,&a->cylinder,gabi::at<void>(0x101B80C0));
    gabi::store<u32>(gabi::ea(a)+0xA8,gabi::ea(&a->status)); a->state=1; gabi::store<u8>(gabi::ea(a)+0x19,0);
}
VERIFY(0x02198D48,armInit);
static void stopInit(Stop* s,DoorInfo* door) {
    WWHD_FUNC(0x02198DAC,void,s,door); s8 room=gabi::load<s8>(gabi::ea(door)+0x1C9);
    gabi::call(0x0255FFF4,gabi::at<void>(gabi::ea(s)+0x550),room,255); for(u32 i=0;i<3;++i) armInit(&s->arms[i]);
}
VERIFY(0x02198DAC,stopInit);
static BOOL remove(Actor* a) {
    WWHD_FUNC(0x021989D8,BOOL,a); if(a->heap) { auto play=dComIfGp_get(); gabi::call(0x020087EC,gabi::at<void>(gabi::ea(play)+0x12A0),a->background.get()); }
    stopEnd(&a->stop); gabi::call(0x025204C8,&a->phase,gabi::at<void>(0x10012B48));
    if(makeKey(a)) gabi::call(0x0252B494,&a->key); gabi::call(0x0252B3CC,&a->smoke); return TRUE;
}
VERIFY(0x021989D8,remove);
static void stopDraw(Stop* s,DoorInfo* door) {
    WWHD_FUNC(0x021984F4,void,s,door);
    auto env=gabi::call<void*>(0x02555D0C); gabi::call(0x025626A4,env,0,&door->current.pos,&s->tev);
    for(u32 i=0;i<3;++i) {
        Arm* arm=&s->arms[i]; auto morph=arm->morph.get();
        if(morph) { auto model=gabi::at<void>(gabi::load<u32>(gabi::ea(morph)+0x90)); env=gabi::call<void*>(0x02555D0C); gabi::call(0x02562F5C,env,model,&s->tev); gabi::call(0x025E5590,arm->morph.get()); }
        auto cap=arm->cap.get();
        if(cap) { auto model=gabi::at<void>(gabi::load<u32>(gabi::ea(cap)+0x90)); env=gabi::call<void*>(0x02555D0C); gabi::call(0x02562F5C,env,model,&s->tev); gabi::call(0x025E54D8,arm->cap.get()); }
    }
}
VERIFY(0x021984F4,stopDraw);
static BOOL draw(Actor* a) {
    WWHD_FUNC(0x0219859C,BOOL,a);
    if(gabi::call<BOOL>(0x0252B0FC,a,0)) {
        auto env=gabi::call<void*>(0x02555D0C); gabi::call(0x025626A4,env,0,&a->current.pos,&a->tevStr);
        env=gabi::call<void*>(0x02555D0C); gabi::call(0x02562F5C,env,a->model.get(),&a->tevStr);
        auto play=dComIfGp_get(); gabi::store<u32>(0x104B4634,gabi::load<u32>(gabi::ea(play)+0x5D70));
        play=dComIfGp_get(); gabi::store<u32>(0x104B4638,gabi::load<u32>(gabi::ea(play)+0x5D74));
        gabi::call(0x025E2DE0,a->model.get(),0);
        play=dComIfGp_get(); gabi::store<u32>(0x104B4634,gabi::load<u32>(gabi::ea(play)+0x5D78));
        play=dComIfGp_get(); gabi::store<u32>(0x104B4638,gabi::load<u32>(gabi::ea(play)+0x5D7C));
        if(a->key.enabled) gabi::call(0x0252B9D4,&a->key,a);
        if(a->stop.enabled) stopDraw(&a->stop,a);
    }
    return TRUE;
}
VERIFY(0x0219859C,draw);
static BOOL drawWrapper(Actor* a) { WWHD_FUNC(0x02198674,BOOL,a); return draw(a); }
VERIFY(0x02198674,drawWrapper);
static Arm* armConstruct(Arm* a) {
    WWHD_FUNC(0x02198A58,Arm*,a); if(!a) a=gabi::call<Arm*>(0x0273AD10,0x1C4); if(!a) return a;
    u32 base=gabi::ea(a); gabi::call(0x025A5B18,gabi::at<void>(base+8),1);
    gabi::call(0x0200BD2C,&a->status); gabi::call(0x02515DA0,gabi::at<void>(base+0x44));
    gabi::store<u32>(base+0x40,0x1004AE88); gabi::store<u32>(base+0x44,0x1004AEC0);
    gabi::call(0x02515FB8,&a->cylinder); gabi::store<u32>(base+0x178,0x100015A8); gabi::store<u32>(base+0x174,0x10012A28);
    gabi::call(0x02018590,gabi::at<void>(base+0x17C));
    gabi::store<u32>(base+0xA0,0x1004B108); gabi::store<u32>(base+0x190,0x1004B150); gabi::store<u32>(base+0x178,0x1004B160);
    f32 random=gabi::call<f32>(0x02019788); a->phase=(s16)gabi::ftoi(gabi::fmuls_ppc(random,65536.0f));
    random=gabi::call<f32>(0x02019788); f32 delta=gabi::fmuls_ppc(gabi::fsubs_ppc(random,0.5f),12000.0f);
    a->scale.set(1,1,1); a->position.set(1,1,1); a->phaseSpeed=(s16)gabi::ftoi(delta); return a;
}
VERIFY(0x02198A58,armConstruct);
static void sinit() {
    WWHD_FUNC(0x02199020,void); for(u32 i=0;i<4;++i) gabi::store<u32>(0x10464B18+i*4,0);
    gabi::call(0x028F026C,gabi::at<void>(0x101B8104));
    gabi::store<f32>(0x10464B0C,-3.1415927410125732f); gabi::store<f32>(0x10464B10,3.1415927410125732f);
    gabi::call(0x028ED6F8,gabi::at<void>(0x10464B14)); gabi::call(0x028F026C,gabi::at<void>(0x101B8110));
    gabi::call(0x028EAB2C,gabi::at<void>(0x10464B15)); gabi::call(0x028F026C,gabi::at<void>(0x101B811C));
}
VERIFY(0x02199020,sinit);
static void setKey(Actor* a) {
    WWHD_FUNC(0x02198418,void,a); if(makeKey(a)) {
        u8 sw=frontSwitch(a); u32 save=gabi::load<u32>(0x101F84DC);
        if(!gabi::call<BOOL>(0x025BA0C0,gabi::at<void>(save+0x20),sw,-1)) { gabi::call(0x0252B848,&a->key); return; }
    }
    gabi::call(0x0252B5A8,&a->key);
}
VERIFY(0x02198418,setKey);
static Stop* stopConstruct(Stop* s) {
    WWHD_FUNC(0x02198B8C,Stop*,s); if(!s) s=gabi::call<Stop*>(0x0273AD10,0x718); if(!s) return s;
    gabi::call(0x028EFFD0,s,3,0x1C4,gabi::at<void>(0x02198A58));
    f32 leading[6],trailing[8]; u8 bytes[4]; s16 shorts[4]; u32 prototype=0x1016E414;
    for(u32 i=0;i<6;++i) { leading[i]=gabi::load<f32>(prototype+i*4); gabi::store<f32>(gabi::ea(s)+0x550+i*4,leading[i]); }
    for(u32 i=0;i<4;++i) { bytes[i]=gabi::load<u8>(prototype+0x18+i); gabi::store<u8>(gabi::ea(s)+0x568+i,bytes[i]); }
    for(u32 i=0;i<4;++i) { shorts[i]=gabi::load<s16>(prototype+0x1C+i*2); gabi::store<s16>(gabi::ea(s)+0x56C+i*2,shorts[i]); }
    for(u32 i=0;i<8;++i) trailing[i]=gabi::load<f32>(prototype+0x24+i*4);
    for(u32 base : {0x550u,0x610u,0x694u}) {
        for(u32 i=0;i<6;++i) gabi::store<f32>(gabi::ea(s)+base+i*4,leading[i]);
        for(u32 i=0;i<4;++i) gabi::store<u8>(gabi::ea(s)+base+0x18+i,bytes[i]);
        for(u32 i=0;i<4;++i) gabi::store<s16>(gabi::ea(s)+base+0x1C+i*2,shorts[i]);
        for(u32 i=0;i<8;++i) gabi::store<f32>(gabi::ea(s)+base+0x24+i*4,trailing[i]);
    }
    return s;
}
VERIFY(0x02198B8C,stopConstruct);
static void stopMatrix(Stop* s,DoorInfo* door) {
    WWHD_FUNC(0x021969A4,void,s,door); if(s->enabled) {
        armMatrix(&s->arms[0],door,0.0f,100.0f,s->side);
        armMatrix(&s->arms[1],door,100.0f,75.0f,s->side);
        armMatrix(&s->arms[2],door,-100.0f,75.0f,s->side);
    }
}
VERIFY(0x021969A4,stopMatrix);
static void armMatrix(Arm* a,DoorInfo* door,f32 x,f32 z,u32 side) {
    WWHD_FUNC(0x02196770,void,a,door,x,z,side); gabi::Local<cXyz> origin; gabi::Local<u8[16]> callerLinkage;
    auto morph=a->morph.get(); if(morph) {
        u32 model=gabi::load<u32>(gabi::ea(morph)+0x90); f32 scaleX=a->scale.x,scaleY=a->scale.y,scaleZ=a->scale.z;
        gabi::store<f32>(model+0xBC,scaleX); gabi::store<f32>(model+0xC0,scaleY); gabi::store<f32>(model+0xC4,scaleZ);
        auto cap=a->cap.get(); model=gabi::load<u32>(gabi::ea(cap)+0x90);
        scaleX=a->position.x; scaleY=a->position.y; scaleZ=a->position.z;
        gabi::store<f32>(model+0xBC,scaleX); gabi::store<f32>(model+0xC4,scaleZ); gabi::store<f32>(model+0xC0,scaleY);
        u32 matrix=0x1048D0CC;
        gabi::call(0x028E93CC,gabi::at<void>(matrix),(f32)door->current.pos.x,(f32)door->current.pos.y,(f32)door->current.pos.z);
        gabi::call(0x025F1C28,gabi::at<void>(matrix),(s16)door->current.angle.y);
        if(side==1) gabi::call(0x025F1C28,gabi::at<void>(matrix),0x7FFF);
        gabi::call(0x025F24E0,x,0.0f,z); gabi::call(0x025F1C28,gabi::at<void>(matrix),(s16)a->phaseSpeed);
        model=gabi::load<u32>(gabi::ea(a->morph.get())+0x90); copyMatrix(matrix,model+0xC8);
        model=gabi::load<u32>(gabi::ea(a->cap.get())+0x90); copyMatrix(matrix,model+0xC8);
        origin->set(0,0,0); gabi::call(0x028E8F64,gabi::at<void>(matrix),origin.get(),&a->previousPosition);
        gabi::call(0x020182E0,gabi::at<void>(gabi::ea(a)+0x17C),&a->previousPosition);
        f32 radius=gabi::fmuls_ppc(50.0f,(f32)a->position.x); gabi::call(0x020184DC,gabi::at<void>(gabi::ea(a)+0x17C),radius);
    }
}
VERIFY(0x02196770,armMatrix);
static BOOL createHeap(Actor* a) {
    WWHD_FUNC(0x02196A70,BOOL,a);
    auto data=gabi::call<void*>(0x0252447C,gabi::at<void>(0x10012A9C),gabi::at<void>(0x10012A48));
    if(!data) gabi::call(0x0273AA24,gabi::at<void>(0x10012AA4),0x383,gabi::at<void>(0x10012AB4));
    auto model=gabi::call<J3DModel*>(0x025E38E0,data,0,0x11020203); a->model=model; if(!model) return FALSE;
    if(makeKey(a) && !gabi::call<BOOL>(0x0252B834,&a->key,0)) return FALSE;
    if(makeStop(a)) for(u32 i=0;i<3;++i) if(!drawSet(&a->stop.arms[i])) return FALSE;
    auto background=gabi::call<dBgW*>(0x024F23F4,gabi::at<void>(0)); a->background=background; if(!background) return FALSE;
    auto collision=gabi::call<void*>(0x0252447C,gabi::at<void>(0x10012A9C),gabi::at<void>(0x10012A54)); if(!collision) return FALSE;
    calcMatrix(a); gabi::call(0x0252B854,&a->key,a); stopMatrix(&a->stop,a);
    model=a->model.get(); auto matrix=model?gabi::at<void>(gabi::ea(model)+0xC8):nullptr;
    return gabi::call<s32>(0x0200A030,a->background.get(),collision,1,matrix)==0;
}
VERIFY(0x02196A70,createHeap);
static BOOL heapCallback(Actor* a) { WWHD_FUNC(0x02196BF8,BOOL,a); return createHeap(a); }
VERIFY(0x02196BF8,heapCallback);
static BOOL actionStopClose(Actor* a) {
    WWHD_FUNC(0x02198338,BOOL,a); if(stopCloseProc(&a->stop,a)) a->openState=1; stopMatrix(&a->stop,a); return TRUE;
}
VERIFY(0x02198338,actionStopClose);
static BOOL actionDemo(Actor* a) {
    WWHD_FUNC(0x0219838C,BOOL,a); s16 event=a->eventIds[a->action]; auto play=dComIfGp_get();
    if(gabi::call<BOOL>(0x025440C8,gabi::at<void>(gabi::ea(play)+0x52C4),event)) {
        a->openState=1; play=dComIfGp_get(); u16 flags=gabi::load<u16>(gabi::ea(play)+0x52B8); gabi::store<u16>(gabi::ea(play)+0x52B8,flags|8);
        a->shape_angle.y=a->current.angle.y;
    } else demoProc(a);
    return TRUE;
}
VERIFY(0x0219838C,actionDemo);
static BOOL actionInit(Actor* a) {
    WWHD_FUNC(0x02198498,BOOL,a); setKey(a); gabi::call(0x0252B854,&a->key,a); setStop(a); stopMatrix(&a->stop,a); actionWait(a); a->openState=1; return TRUE;
}
VERIFY(0x02198498,actionInit);
static BOOL createInit(Actor* a) {
    WWHD_FUNC(0x02198E08,BOOL,a); auto play=dComIfGp_get();
    if(gabi::call<s32>(0x024EEA6C,gabi::at<void>(gabi::ea(play)+0x12A0),a->background.get(),a))
        gabi::call(0x0273AA24,gabi::at<void>(0x10012B30),0x45D,gabi::at<void>(0x10012B2C));
    f32 attentionY=gabi::load<f32>(gabi::ea(a)+0x394),eyeY=a->eyePos.y; u8 room=a->current.roomNo;
    gabi::store<u8>(gabi::ea(a)+0x1C9,room); a->travel=0; gabi::store<u32>(gabi::ea(a)+0x39C,0x20);
    a->eyePos.y=gabi::fadds_ppc(eyeY,150.0f); a->openState=0; gabi::store<f32>(gabi::ea(a)+0x394,gabi::fadds_ppc(attentionY,150.0f));
    calcMatrix(a); gabi::call(0x024F43DC,a->background.get()); auto background=a->background.get(); room=frontRoom(a);
    gabi::store<u16>(gabi::ea(background)+0xB8,room); gabi::call(0x0252A9E0,a,1); stopInit(&a->stop,a); return TRUE;
}
VERIFY(0x02198E08,createInit);
static s32 create(Actor* a) {
    WWHD_FUNC(0x02198EEC,s32,a); s32 phase=gabi::call<s32>(0x02520460,&a->phase,gabi::at<void>(0x10012B48)); u32 condition=gabi::load<u32>(gabi::ea(a)+0x2E4);
    if(!(condition&8)) {
        if(a) { gabi::call(0x0252A244,a); gabi::store<u32>(gabi::ea(a)+0xB4,0x10012A38);
            gabi::call(0x0252B190,&a->smoke); gabi::call(0x0252B3D0,&a->key); stopConstruct(&a->stop); condition=gabi::load<u32>(gabi::ea(a)+0x2E4); }
        gabi::store<u32>(gabi::ea(a)+0x2E4,condition|8);
    }
    if(phase!=4) return phase;
    if(makeKey(a)) { phase=gabi::call<s32>(0x0252B484,&a->key); if(phase!=4) return phase; }
    a->current.roomNo=frontRoom(a);
    if(!gabi::call<BOOL>(0x025D63E8,a,gabi::at<void>(0x02196BF8),0xD960)) { a->background=nullptr; return 5; }
    createInit(a); return 4;
}
VERIFY(0x02198EEC,create);
static s32 createWrapper(Actor* a) { WWHD_FUNC(0x0219901C,s32,a); return create(a); }
VERIFY(0x0219901C,createWrapper);
static BOOL execute(Actor* a) {
    WWHD_FUNC(0x0219884C,BOOL,a);
    if(!gabi::load<u32>(0x101FDAC8)) { gabi::store<u32>(0x101FDAC8,1); gabi::call(0xC000A848,gabi::at<void>(0x101FDACC),gabi::at<void>(0x101B8090),0x10); }
    u32 state=gabi::call<u32>(0x0252AC98,a);
    switch(state) {
    case 0: a->openState=0; break;
    case 1: gabi::call(0x0252AD50,a); demoProc(a); break;
    case 2: gabi::call_ptr(gabi::load<u32>(0x101FDACC+(u32)a->openState*4),a); break;
    default: gabi::call(0x0273AA24,gabi::at<void>(0x10012A60),0x59C,gabi::at<void>(0x10012A0C)); break;
    }
    u8 enabled=a->stop.enabled; a->secondaryRoom=gabi::load<u8>(0x1047E6C8); if(enabled) stopExecute(&a->stop,a); return TRUE;
}
VERIFY(0x0219884C,execute);
static BOOL stopCloseProc(Stop* s,DoorInfo* door) {
    WWHD_FUNC(0x02197600,BOOL,s,door); if(!s->moving) return TRUE; BOOL done=TRUE;
    for(u32 i=0;i<3;++i) if(!armCloseProc(&s->arms[i],door)) done=FALSE;
    if(done) s->moving=0; return done;
}
VERIFY(0x02197600,stopCloseProc);
static BOOL armOpenProc(Arm* a,DoorInfo* door) {
    WWHD_FUNC(0x02197694,BOOL,a,door); s16 timer=a->angle; if(timer>0) { a->angle=timer-1; return FALSE; }
    if(((f32)a->scale.y>0.1f)) {
        if(!a->started) { a->started=1; sound(door,0x694D); }
        gabi::call(0x0200EDC8,&a->scale.y,0.25f,0.3f); f32 value=a->scale.y; a->scale.z=value; a->scale.x=value; return FALSE;
    }
    f32 capY=a->position.y; a->scale.set(0,0,0);
    if((capY>0.1f)) { gabi::call(0x0200EDC8,&a->position.y,0.5f,0.3f); f32 value=a->position.y; a->position.z=value; a->position.x=value; return FALSE; }
    a->position.set(0,0,0); return TRUE;
}
VERIFY(0x02197694,armOpenProc);
static BOOL stopOpenProc(Stop* s,DoorInfo* door) {
    WWHD_FUNC(0x0219783C,BOOL,s,door); if(!s->moving) return TRUE; BOOL done=TRUE;
    for(u32 i=0;i<3;++i) if(!armOpenProc(&s->arms[i],door)) done=FALSE;
    if(done) { s->enabled=0; s->moving=0; } return done;
}
VERIFY(0x0219783C,stopOpenProc);
static BOOL armCloseProc(Arm* a,DoorInfo* door) {
    WWHD_FUNC(0x021972E0,BOOL,a,door); gabi::Local<csXyz> rotation; gabi::Local<u8[16]> callerLinkage;
    s16 timer=a->angle; if(timer>0) { a->angle=timer-1; return FALSE; }
    if(((f32)a->position.y<0.9f)) {
        gabi::call(0x0200ED84,&a->position.y,1.0f,0.5f,0.3f); f32 value=a->position.y; a->position.x=value; a->position.z=value; return FALSE;
    }
    u8 started=a->started; a->position.set(1,1,1);
    if(!started) {
        a->started=1; sound(door,0x694B); auto play=dComIfGp_get(); auto particle=gabi::at<void>(gabi::load<u32>(gabi::ea(play)+0x5AB0));
        auto color=gabi::at<void>(gabi::ea(door)+0x1A8);
        gabi::call(0x025A847C,particle,0,0x816C,&a->previousPosition,gabi::at<void>(0),gabi::at<void>(0),255,gabi::at<void>(0),-1,color,color,gabi::at<void>(0));
        rotation->x=door->shape_angle.x; u8 front=door->front; s16 yaw=door->shape_angle.y; rotation->z=door->shape_angle.z; s8 room=door->current.roomNo;
        rotation->y=front==1?(s16)((s32)yaw+0x7FFF):yaw; play=dComIfGp_get(); particle=gabi::at<void>(gabi::load<u32>(gabi::ea(play)+0x5AB0));
        gabi::call(0x025A847C,particle,2,0xA16D,&a->previousPosition,rotation.get(),gabi::at<void>(0),185,gabi::at<void>(gabi::ea(a)+8),room,gabi::at<void>(0),gabi::at<void>(0),gabi::at<void>(0));
    }
    if(((f32)a->scale.y<0.9f)) {
        gabi::call(0x0200ED84,&a->scale.y,1.0f,0.3f,0.3f); f32 value=a->scale.y; a->scale.x=value; a->scale.z=value; return FALSE;
    }
    a->scale.set(1,1,1); return TRUE;
}
VERIFY(0x021972E0,armCloseProc);
struct ResourceName { gabi::be<u32> text,vtable; };
static BOOL drawSet(Arm* a) {
    WWHD_FUNC(0x021964B8,BOOL,a); gabi::Local<ResourceName> bodyName,capName; gabi::Local<u8[16]> callerLinkage;
    bodyName->vtable=0x10012A10; bodyName->text=0x10012A84;
    auto resource=gabi::call<void*>(0x026066C4,gabi::at<void>(gabi::load<u32>(0x101F4F28)),bodyName.get(),4);
    auto morph=gabi::call<mDoExt_McaMorf*>(0x025E4F64,gabi::at<void>(0),resource,gabi::at<void>(0),gabi::at<void>(0),gabi::at<void>(0),2,gabi::at<void>(0),-1,gabi::at<void>(0),gabi::at<void>(0),0,0x11020203,1.0f);
    a->morph=morph; if(!morph || !gabi::load<u32>(gabi::ea(morph)+0x90)) return FALSE;
    u32 model=gabi::load<u32>(gabi::ea(morph)+0x90); gabi::store<u32>(model+0xB8,gabi::ea(a));
    auto data=gabi::at<void>(gabi::load<u32>(gabi::load<u32>(gabi::ea(a->morph.get())+0x90)+0xAC));
    auto header=gabi::call<void*>(0x027F3F94,data); u16 count=gabi::load<u16>(gabi::ea(header)+8);
    for(u16 index=0;index<count;++index) {
        model=gabi::load<u32>(gabi::ea(a->morph.get())+0x90); u32 modelData=gabi::load<u32>(model+0xAC);
        u32 nodes=gabi::load<u32>(modelData+8),nodeCount=gabi::load<u32>(modelData+4);
        if(index<nodeCount) nodes+=(u32)index*0x1C;
        gabi::store<u32>(nodes+8,0x0219623C);
        data=gabi::at<void>(gabi::load<u32>(gabi::load<u32>(gabi::ea(a->morph.get())+0x90)+0xAC));
        header=gabi::call<void*>(0x027F3F94,data); count=gabi::load<u16>(gabi::ea(header)+8);
    }
    capName->vtable=0x10012A10; capName->text=0x10012A84;
    resource=gabi::call<void*>(0x026066C4,gabi::at<void>(gabi::load<u32>(0x101F4F28)),capName.get(),3);
    auto cap=gabi::call<mDoExt_McaMorf*>(0x025E4F64,gabi::at<void>(0),resource,gabi::at<void>(0),gabi::at<void>(0),gabi::at<void>(0),2,gabi::at<void>(0),-1,gabi::at<void>(0),gabi::at<void>(0),0,0x11020203,1.0f);
    morph=a->morph.get(); a->cap=cap; return morph && gabi::load<u32>(gabi::ea(morph)+0x90)!=0;
}
VERIFY(0x021964B8,drawSet);
static void setEvent(Actor* a) {
    WWHD_FUNC(0x0219807C,void,a); u8 other=a->stop.otherEnabled;
    if(!a->front) { a->action=2; if(other==255) { other=(u8)stopBack(a); a->stop.otherEnabled=other; } }
    else { a->action=3; if(other==255) { other=(u8)stopFront(a); a->stop.otherEnabled=other; } }
    if(a->stop.enabled) return;
    u8 keyEnabled=a->key.enabled; if(other==1) a->action=(u8)a->action+2;
    if(keyEnabled) { u32 save=gabi::load<u32>(0x101F84DC); if(!gabi::load<u8>(save+0x7B8)) return; }
    u8 sw=frontSwitch(a);
    if(feelerCase(a) && sw!=255) { u8 room=frontRoom(a); if(!switchOn(sw,room)) return; }
    if(gabi::call<BOOL>(0x0252AE04,a,12100.0f,12100.0f,62500.0f)) {
        u8 action=a->action; u16 flags=gabi::load<u16>(gabi::ea(a)+0xFA); s16 event=a->eventIds[action];
        gabi::store<s16>(gabi::ea(a)+0xFC,event); u8 tool=a->toolIds[action]; gabi::store<u16>(gabi::ea(a)+0xFA,flags|4); gabi::store<u8>(gabi::ea(a)+0xFE,tool);
    }
}
VERIFY(0x0219807C,setEvent);
static BOOL actionWait(Actor* a) {
    WWHD_FUNC(0x021981E4,BOOL,a); u16 command=gabi::load<u16>(gabi::ea(a)+0xF8);
    if(command==3) { gabi::call(0x0252A2DC,a,1); a->openState=3; demoProc(a); return TRUE; }
    if(a->stop.enabled) {
        if(command==2) {
            auto play=dComIfGp_get(); a->staff=gabi::call<s32>(0x02542D88,gabi::at<void>(gabi::ea(play)+0x52C4),gabi::at<void>(0x10012B10),0,0);
            u8 front=a->front; s16 yaw=a->current.angle.y; a->shape_angle.y=yaw; if(front==1) a->shape_angle.y=(s16)((s32)yaw+0x7FFF);
            a->openState=3; demoProc(a); return TRUE;
        }
        if(stopOpen(a)) { setStopDemo(a); u8 action=a->action; u8 tool=a->toolIds[action]; s16 event=a->eventIds[action]; gabi::call(0x025D7A58,a,event,tool,0xFFFF,0,1); }
        return TRUE;
    }
    if(stopClose(a)) { a->stop.enabled=1; stopCloseInit(&a->stop); stopMatrix(&a->stop,a); a->openState=2; }
    else setEvent(a);
    return TRUE;
}
VERIFY(0x021981E4,actionWait);
static void stopExecute(Stop* s,DoorInfo* door) {
    WWHD_FUNC(0x02198678,void,s,door);
    for(u32 i=0;i<3;++i) {
        Arm* a=&s->arms[i]; auto morph=a->morph.get();
        if(morph) {
            u32 model=gabi::load<u32>(gabi::ea(morph)+0x90); auto data=gabi::at<void>(gabi::load<u32>(model+0xAC));
            auto header=gabi::call<void*>(0x027F3F94,data); u16 count=gabi::load<u16>(gabi::ea(header)+8);
            for(u16 index=0;index<count;++index) {
                model=gabi::load<u32>(gabi::ea(a->morph.get())+0x90); u32 modelData=gabi::load<u32>(model+0xAC);
                u32 nodes=gabi::load<u32>(modelData+8),nodeCount=gabi::load<u32>(modelData+4); if(index<nodeCount) nodes+=(u32)index*0x1C;
                gabi::store<u32>(nodes+8,0x0219623C); data=gabi::at<void>(gabi::load<u32>(gabi::load<u32>(gabi::ea(a->morph.get())+0x90)+0xAC));
                header=gabi::call<void*>(0x027F3F94,data); count=gabi::load<u16>(gabi::ea(header)+8);
            }
            gabi::call(0x025E55A0,a->morph.get()); a->phase=(s16)((s32)a->phase+1);
        }
        auto play=dComIfGp_get(); gabi::call(0x0200E240,gabi::at<void>(gabi::ea(play)+0x26A4),&a->cylinder);
        if(((f32)a->scale.x>0.9f)) {
            u8 timer=a->soundTimer; if(timer) a->soundTimer=timer-1;
            else { sound(door,0x3823); f32 random=gabi::call<f32>(0x02019788); a->soundTimer=(u8)gabi::ftoi(gabi::fmadds(random,30.0f,50.0f)); }
        } else a->soundTimer=0;
    }
}
VERIFY(0x02198678,stopExecute);
static void cutEnd(Actor* a) { s32 staff=a->staff; auto play=dComIfGp_get(); gabi::call(0x02543280,gabi::at<void>(gabi::ea(play)+0x52C4),staff); }
static void demoProc(Actor* a) {
    WWHD_FUNC(0x021978D4,void,a); u32 action=gabi::call<u32>(0x0252A684,a); s32 staff=a->staff; auto play=dComIfGp_get();
    if(gabi::call<BOOL>(0x025447C8,gabi::at<void>(gabi::ea(play)+0x52C4),staff)) {
        switch(action) {
        case 1: stopOpenInit(&a->stop); break;
        case 2: setStop(a); if(a->stop.enabled) stopCloseInit(&a->stop); break;
        case 3: openInit(a); break;
        case 4: closeInit(a); break;
        case 5: gabi::call(0x0252B1E8,&a->smoke,a); break;
        case 6: gabi::call(0x0252B3CC,&a->smoke); break;
        case 7: gabi::call(0x0252A6D0,a); break;
        case 8: gabi::call(0x0252B4A4,&a->key,a); break;
        }
    }
    switch(action) {
    case 1: if(stopOpenProc(&a->stop,a)) cutEnd(a); stopMatrix(&a->stop,a); break;
    case 2: if(stopCloseProc(&a->stop,a)) cutEnd(a); stopMatrix(&a->stop,a); break;
    case 3: if(!(a->flags&1)) cutEnd(a); else if(openProc(a)) { openEnd(a); cutEnd(a); } break;
    case 4: if(!(a->flags&2)) cutEnd(a); else if(closeProc(a)) { closeEnd(a); cutEnd(a); } break;
    case 5: gabi::call(0x0252B2D8,&a->smoke,a); cutEnd(a); break;
    case 8: if(gabi::call<BOOL>(0x0252B5B4,&a->key)) cutEnd(a); gabi::call(0x0252B854,&a->key,a); break;
    case 19:
        play=dComIfGp_get(); if(gabi::load<u16>(gabi::ea(play)+0x52B8)&1) {
            a->openState=1; play=dComIfGp_get(); u16 flags=gabi::load<u16>(gabi::ea(play)+0x52B8); gabi::store<u16>(gabi::ea(play)+0x52B8,flags|8);
            a->shape_angle.y=a->current.angle.y; play=dComIfGp_get(); flags=gabi::load<u16>(gabi::ea(play)+0x52B8); gabi::store<u16>(gabi::ea(play)+0x52B8,flags&~1);
            play=dComIfGp_get(); if(gabi::call<BOOL>(0x025449B0,gabi::at<void>(gabi::ea(play)+0x52C4))) { play=dComIfGp_get(); gabi::call(0x0254351C,gabi::at<void>(gabi::ea(play)+0x52E8)); }
        }
        cutEnd(a); break;
    default: cutEnd(a); break;
    }
}
VERIFY(0x021978D4,demoProc);
}
