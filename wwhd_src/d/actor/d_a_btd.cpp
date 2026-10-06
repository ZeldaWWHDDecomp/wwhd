/**
 * d_a_btd.cpp (WWHD)
 * Boss - Gohma (Dragon Roost Cavern)
 *
 * Written against the WWHD code with the GameCube
 * decompilation (zeldaret/tww src/d/actor/d_a_btd.cpp) as reference, and verified against
 * cking.rpx. Function names follow the GameCube source; `move`
 * (020F6004) is GameCube move() with wait/attack/damage inlined (the matcher calls it damage).
 * damage_check is in d_a_btd_damage.cpp. hahen_set (020F0EB4, matcher: hahen_set_s) inlines hahen_set_s.
 */
#include "d/actor/d_a_btd.h"
#include "gabi.h"

namespace {
template<class T> T rd(u32 p, u32 off=0) { return gabi::load<T>(p+off); }
template<class T> void wr(u32 p, u32 off, T value) { gabi::store<T>(p+off,value); }
u32 play() { return gabi::call<u32>(0x025200D4); }
u32 matrix() { return rd<u32>(0x1018C7B0); }
void copy3(u32 dst,u32 src) {
    u32 x=rd<u32>(src), y=rd<u32>(src,4), z=rd<u32>(src,8);
    wr<u32>(dst,0,x); wr<u32>(dst,4,y); wr<u32>(dst,8,z);
}
u32 resource(u32 name,s32 index) {
    struct SafeString_l { be<u32> name, vtable; };
    gabi::Local<SafeString_l> text;
    text->name=name; text->vtable=0x1000B9F4;
    u32 controller=rd<u32>(0x101F4F28);
    return gabi::call<u32>(0x026066C4,controller,text.get(),index);
}
}

void wave_set(btd_class* self) {
    WWHD_FUNC(0x020F02D4,void,self);
    s32 room=rd<s8>(gabi::ea(self),0x326);
    s32 reverb=gabi::call<s32>(0x02520540,room);
    gabi::call<void>(0x025E1A40,0x583Eu,0x104629E4u,0,reverb);
    wr<f32>(gabi::ea(self),0x70C0,0.0f);
}
VERIFY(0x020F02D4,wave_set);

void* get_anm(btd_class* self) {
    WWHD_FUNC(0x020F032C,void*,self);
    if(self->phase!=0) return self->state==11 ? self->deathMorf.get() : self->phase2Morf.get();
    return self->phase1Morf.get();
}
VERIFY(0x020F032C,get_anm);
void* get_btk(btd_class* self) {
    WWHD_FUNC(0x020F035C,void*,self);
    if(self->phase!=0) return self->state==11 ? self->deathBtk.get() : self->phase2Btk.get();
    return self->phase1Btk.get();
}
VERIFY(0x020F035C,get_btk);
void* get_brk(btd_class* self) {
    WWHD_FUNC(0x020F038C,void*,self);
    if(self->phase!=0) return self->state==11 ? self->deathBrk.get() : self->phase2Brk.get();
    return self->phase1Brk.get();
}
VERIFY(0x020F038C,get_brk);
void anm_init(btd_class* self,s32 animation,f32 blend,u8 loop,f32 speed,s32 sound) {
    WWHD_FUNC(0x020F03BC,void,self,animation,blend,loop,speed,sound);
    void* morf=get_anm(self);
    u32 animationResource=resource(0x1000BA6C,animation);
    u32 soundResource= sound>=0 ? resource(0x1000BA6C,sound) : 0;
    gabi::call<void>(0x025E4A98,morf,animationResource,loop,blend,speed,0.0f,-1.0f,soundResource);
}
VERIFY(0x020F03BC,anm_init);

void eff_off(btd_class* self) {
    WWHD_FUNC(0x020F0C38,void,self);
    u32 p=gabi::ea(self);
    for(u32 i=0;i<7;++i) if(rd<s16>(p,0x6150+2*i)>2) wr<s16>(p,0x6150+2*i,2);
}
VERIFY(0x020F0C38,eff_off);

void* k_a_d_sub(void* actor,void* context) {
    WWHD_FUNC(0x020F1378,void*,actor,context);
    s32 isActor=gabi::call<s32>(0x025D4604,actor);
    u32 p=gabi::ea(actor);
    if(isActor && p && rd<s16>(p,8)==0xFA && rd<s32>(p,0xB0)==0x511) return actor;
    return nullptr;
}
VERIFY(0x020F1378,k_a_d_sub);
void* dr2_a_d_sub(void* actor,void* context) {
    WWHD_FUNC(0x020F13D4,void*,actor,context);
    s32 isActor=gabi::call<s32>(0x025D4604,actor);
    u32 p=gabi::ea(actor);
    if(isActor && p && rd<s16>(p,8)==0xDF) return actor;
    return nullptr;
}
VERIFY(0x020F13D4,dr2_a_d_sub);
void* wepon_s_sub(void* actor,void* context) {
    WWHD_FUNC(0x020F1424,void*,actor,context);
    s32 isActor=gabi::call<s32>(0x025D4604,actor);
    u32 p=gabi::ea(actor);
    if(!p) return nullptr;
    s16 id=rd<s16>(p,8);
    return (isActor && id==0x1BE) || id==0x1B0 ? actor : nullptr;
}
VERIFY(0x020F1424,wepon_s_sub);

BOOL daBtd_IsDelete(btd_class* self) {
    WWHD_FUNC(0x020F34A0,BOOL,self);
    return TRUE;
}
VERIFY(0x020F34A0,daBtd_IsDelete);
void* smokeEcallBack_ct(void* self) {
    WWHD_FUNC(0x020F40A0,void*,self);
    return gabi::call<void*>(0x025A5B18,self,1);
}
VERIFY(0x020F40A0,smokeEcallBack_ct);
void SafeString_dt(void* self,s32 flags) {
    WWHD_FUNC(0x020F4910,void,self,flags);
    if(self && (flags&1)) gabi::call<void>(0x0273AF40,self);
}
VERIFY(0x020F4910,SafeString_dt);
void smokeEcallBack_dt(void* self,s32 flags) {
    WWHD_FUNC(0x020F98E4,void,self,flags);
    if(self && (flags&1)) gabi::call<void>(0x0273AF40,self);
}
VERIFY(0x020F98E4,smokeEcallBack_dt);
void SafeString_v14(void* self) {
    WWHD_FUNC(0x020F99F4,void,self);
}
VERIFY(0x020F99F4,SafeString_v14);

BOOL daBtd_Delete(btd_class* self) {
    WWHD_FUNC(0x020F34A8,BOOL,self);
    u32 p=gabi::ea(self);
    gabi::call<void>(0x025204C8,p+0x3C8,0x1000BC44u);
    gabi::call<void>(0x025204C8,p+0x3D0,0x1000BC48u);
    s32 child=rd<s8>(0x104629FC);
    gabi::call<void>(0x025F0A18,child);
    for(u32 i=0;i<19;++i) gabi::call<void>(0x025E1B34,p+0x5FC0+i*12);
    for(u32 i=0;i<50;++i) gabi::call<void>(0x025E1B34,p+0x6954+i*36);
    for(u32 i=0;i<19;++i) gabi::call<void>(0x025E1B34,p+0x424+i*12);
    u32 peg=rd<u32>(0x104629C0);
    if(peg) gabi::call<void>(0x025E1B34,peg+0x5C4);
    gabi::call<void>(0x025E1B34,0x104629E4u);
    gabi::call<void>(0x0255BA9C,p+0x61D8);
    return TRUE;
}
VERIFY(0x020F34A8,daBtd_Delete);

void* dCcD_Cyl_ct(void* self) {
    WWHD_FUNC(0x020F9858,void*,self);
    u32 p=gabi::ea(self);
    if(!p) p=gabi::call<u32>(0x0273AD10,0x130);
    if(p) {
        gabi::call<void>(0x02515FB8,p);
        wr<u32>(p,0x114,0x100015A8); wr<u32>(p,0x110,0x1000BA0C);
        gabi::call<void>(0x02018590,p+0x118);
        wr<u32>(p,0x3C,0x1004B108); wr<u32>(p,0x12C,0x1004B150);
        wr<u32>(p,0x114,0x1004B160);
    }
    return gabi::at<void>(p);
}
VERIFY(0x020F9858,dCcD_Cyl_ct);
void btd_class_dt(btd_class* self,s32 flags) {
    WWHD_FUNC(0x020F98F8,void,self,flags);
    u32 p=gabi::ea(self);
    if(!p) return;
    gabi::call<void>(0x028F0164,p+0x6178,3,0x20,0x020F98E4u,0,0);
    gabi::call<void>(0x028F0164,p+0x5884,6,0x130,0x02515A70u,0,0);
    gabi::call<void>(0x028F0164,p+0x1DEC,50,0x12C,0x02515AE8u,0,0);
    gabi::call<void>(0x02515AE8,p+0x1CC0,2);
    gabi::call<void>(0x02515AE8,p+0x1B94,2);
    gabi::call<void>(0x028F0164,p+0x550,19,0x12C,0x02515AE8u,0,0);
    gabi::call<void>(0x02515860,p+0x514,2);
    gabi::call<void>(0x025D50BC,p,0);
    if(flags&1) gabi::call<void>(0x0273AF40,p);
}
VERIFY(0x020F98F8,btd_class_dt);

void* daBtd_HIO_ct(void* self) {
    WWHD_FUNC(0x020F4744,void*,self);
    u32 p=gabi::ea(self);
    if(!p) p=gabi::call<u32>(0x0273AD10,0x58);
    if(p) {
        wr<f32>(p,0x18,0.5f); wr<f32>(p,0x2C,4.0f); wr<s8>(p,0,-1);
        wr<f32>(p,0x14,0.5f); wr<f32>(p,0x24,150.0f);
        wr<f32>(p,0x10,100.0f); wr<f32>(p,0x30,20.0f);
        wr<f32>(p,0x20,0.2f); wr<f32>(p,0x1C,0.2f);
        wr<s16>(p,0x28,10); wr<u8>(p,1,0); wr<s16>(p,8,150);
        wr<u32>(p,0x54,0x1000BA1C); wr<u8>(p,0x50,0);
        wr<s16>(p,0x3E,255); wr<u8>(p,3,0); wr<u8>(p,2,0); wr<u8>(p,0xC,0);
        wr<f32>(p,0x34,6.0f); wr<s16>(p,0x38,182);
        wr<f32>(p,0x44,12.0f); wr<f32>(p,0x48,35.0f);
        wr<s16>(p,0x3A,156); wr<s16>(p,0xA,5); wr<s16>(p,0x42,36);
        wr<s16>(p,0x3C,86); wr<f32>(p,0x4C,1.0f); wr<f32>(p,4,1.0f);
        wr<s16>(p,0x40,142);
    }
    return gabi::at<void>(p);
}
VERIFY(0x020F4744,daBtd_HIO_ct);
void __sinit_d_a_btd_cpp() {
    WWHD_FUNC(0x020F4870,void);
    wr<u32>(0x104629D4,8,0); wr<u32>(0x104629D4,0,0);
    wr<u32>(0x104629D4,12,0); wr<u32>(0x104629D4,4,0);
    gabi::call<void>(0x028F026C,0x10192ECCu);
    wr<f32>(0x104629C4,0,rd<f32>(0x1000BD48));
    wr<f32>(0x104629C8,0,rd<f32>(0x1000BD4C));
    gabi::call<void>(0x028ED6F8,0x104629D0u);
    gabi::call<void>(0x028F026C,0x10192ED8u);
    gabi::call<void>(0x028EAB2C,0x104629D1u);
    gabi::call<void>(0x028F026C,0x10192EE4u);
    daBtd_HIO_ct(gabi::at<void>(0x104629FC));
}
VERIFY(0x020F4870,__sinit_d_a_btd_cpp);

BOOL daBtd_Draw(btd_class* self) {
    WWHD_FUNC(0x020F0A74,BOOL,self);
    u32 p=gabi::ea(self); u8 blur=rd<u8>(p,0x7059);
    if(blur>1) {
        wr<u8>(0x101F4826,0,blur);
        gabi::call<void>(0x025F064C);
    } else if(blur==1) {
        wr<u8>(p,0x7059,0); wr<u8>(0x101F4825,0,0);
    }
    s16 angle=rd<s16>(p,0x322);
    gabi::call<void>(0x025BEBB8,0xC7,p,p+0x37C,angle,1.0f,1.0f,1.0f);
    if(rd<u8>(p,0x7090)) {
        u32 env=gabi::call<u32>(0x02555D0C);
        gabi::call<void>(0x025626A4,env,0,p+0x7094,p+0x620C);
        u32 model=rd<u32>(rd<u32>(p,0x3FC),0x90);
        env=gabi::call<u32>(0x02555D0C);
        gabi::call<void>(0x02562F5C,env,model,p+0x620C);
        u32 btk=rd<u32>(p,0x400); u32 data=rd<u32>(model,0xAC); f32 frame=rd<f32>(btk,4);
        gabi::call<void>(0x025E7FC4,btk,data,frame);
        u32 brk=rd<u32>(p,0x404); data=rd<u32>(model,0xAC); frame=rd<f32>(brk,4);
        gabi::call<void>(0x025E83FC,brk,data,frame);
        u32 morf=rd<u32>(p,0x3FC); gabi::call<void>(0x025E5590,morf);
        return TRUE;
    }
    u32 morf=gabi::ea(get_anm(self));
    u32 btk=gabi::ea(get_btk(self));
    u32 brk=gabi::ea(get_brk(self));
    u32 env=gabi::call<u32>(0x02555D0C);
    gabi::call<void>(0x025626A4,env,0,p+0x37C,p+0x620C);
    u32 model=rd<u32>(morf,0x90);
    env=gabi::call<u32>(0x02555D0C);
    gabi::call<void>(0x02562F5C,env,model,p+0x620C);
    f32 frame=rd<f32>(btk,4); u32 data=rd<u32>(model,0xAC);
    gabi::call<void>(0x025E7FC4,btk,data,frame);
    frame=rd<f32>(brk,4); data=rd<u32>(model,0xAC);
    gabi::call<void>(0x025E83FC,brk,data,frame);
    gabi::call<void>(0x025E5590,morf);
    for(u32 i=0;i<35;++i) {
        u32 piece=p+0x63D8+40*i;
        if(rd<u8>(piece,4)) {
            env=gabi::call<u32>(0x02555D0C);
            u32 pieceModel=rd<u32>(piece);
            gabi::call<void>(0x02562F5C,env,pieceModel,p+0x620C);
            pieceModel=rd<u32>(piece);
            gabi::call<void>(0x025E2DA8,pieceModel);
        }
    }
    return TRUE;
}
VERIFY(0x020F0A74,daBtd_Draw);

void smoke_set_s(btd_class* self,cXyz* position,csXyz* angles) {
    WWHD_FUNC(0x020F0DFC,void,self,position,angles);
    u32 p=gabi::ea(self);
    s32 room=rd<s8>(p,0x326);
    u32 control=rd<u32>(play(),0x5AB0);
    gabi::call<void>(0x025A847C,control,2,0xA0AB,position,angles,0,0xB9,p+0x6178,room,0,0,0);
    room=rd<s8>(p,0x326);
    control=rd<u32>(play(),0x5AB0);
    gabi::call<void>(0x025A847C,control,2,0xA0AC,position,angles,0,0xB9,p+0x6198,room,0,0,0);
}
VERIFY(0x020F0DFC,smoke_set_s);

void kubi_calc(btd_class* self) {
    WWHD_FUNC(0x020F1288,void,self);
    u32 p=gabi::ea(self);
    f32 x=rd<f32>(p,0x7094),y=rd<f32>(p,0x7098),z=rd<f32>(p,0x709C);
    gabi::call<void>(0x0200FAD8,0,x,y,z);
    s16 angle=rd<s16>(p,0x70BA); u32 m=matrix();
    gabi::call<void>(0x025F1C28,m,angle);
    angle=rd<s16>(p,0x70B8); m=matrix();
    gabi::call<void>(0x025F1BF4,m,angle);
    m=matrix(); angle=rd<s16>(p,0x70BC);
    gabi::call<void>(0x025F1C5C,m,angle);
    u32 morf=rd<u32>(p,0x3FC); m=matrix(); u32 model=rd<u32>(morf,0x90);
    f32 values[12]; for(u32 i=0;i<12;++i) values[i]=rd<f32>(m,4*i);
    // The source matrix is fully loaded before any destination component is stored.
    const u8 storeOrder[]={7,3,6,4,9,5,10,0,1,8,2,11};
    for(u8 i:storeOrder) wr<f32>(model,0xC8+4*i,values[i]);
    morf=rd<u32>(p,0x3FC); gabi::call<void>(0x025E55A0,morf);
    u32 btk=rd<u32>(p,0x400); gabi::call<void>(0x025E742C,btk);
    u32 brk=rd<u32>(p,0x404); gabi::call<void>(0x025E742C,brk);
}
VERIFY(0x020F1288,kubi_calc);

void sibuki_set(btd_class* self) {
    WWHD_FUNC(0x020F10B4,void,self);
    u32 p=gabi::ea(self); gabi::Local<cXyz> local;
    local->x=0.0f; wr<u8>(p,0x7058,0);
    for(u32 i=0;i<50;++i) {
        u32 spray=p+0x6950+36*i;
        if(rd<s8>(spray)!=0) continue;
        wr<u8>(spray,0,1);
        u32 m=matrix(); f32 random=gabi::call<f32>(0x02019918,32768.0f);
        gabi::call<void>(0x025F1884,m,(s16)gabi::ftoi(random));
        random=gabi::call<f32>(0x02019918,200.0f);
        f32 radius=gabi::fadds_ppc(random,300.0f);
        f32 debug=rd<f32>(0x1047B60C,0x38);
        local->z=gabi::fadds_ppc(radius,debug);
        local->y=gabi::call<f32>(0x020198D8,50.0f);
        gabi::call<void>(0x0200FCD8,local.get(),spray+4);
        f32 forward=rd<f32>(0x104629FC,0x44);
        random=gabi::call<f32>(0x020198D8,gabi::fmuls_ppc(forward,0.5f));
        f32 upward=rd<f32>(0x104629FC,0x48);
        f32 vz=gabi::fadds_ppc(forward,random);
        local->z=vz;
        random=gabi::call<f32>(0x020198D8,gabi::fmuls_ppc(upward,0.5f));
        local->y=gabi::fadds_ppc(upward,random);
        gabi::call<void>(0x0200FCD8,local.get(),spray+0x10);
        m=matrix(); random=gabi::call<f32>(0x02019918,32768.0f);
        gabi::call<void>(0x025F1884,m,(s16)gabi::ftoi(random));
    }
}
VERIFY(0x020F10B4,sibuki_set);

namespace {
void writeJointMatrix(u32 model,u32 index,bool finalBranch=false) {
    u32 block=rd<u32>(model,0x2C), source=matrix();
    u16 flags=rd<u16>(block,4); u32 matrices=rd<u32>(block,0x10);
    wr<u16>(block,4,flags|0x10);
    f32 values[12]; for(u32 i=0;i<12;++i) values[i]=rd<f32>(source,4*i);
    const u8 regular[]={0,4,8,5,6,7,1,2,3,9,10,11};
    const u8 final[]={0,1,2,3,5,6,7,4,8,9,10,11};
    for(u8 i: finalBranch?final:regular) wr<f32>(matrices+index*48,4*i,values[i]);
    source=matrix(); gabi::call<void>(0x028E90D4,source,0x104B4868u);
}
}
BOOL nodeCallBack(void* node,s32 timing) {
    WWHD_FUNC(0x020F04EC,BOOL,node,timing);
    if(timing!=0) return TRUE;
    u32 joint=gabi::call<u32>(0x027F7878,node);
    u32 index=rd<u16>(joint,4);
    if(index>=73) {
        gabi::call<void>(0x0273AA24,0x1000BA9Cu,0x2FF,0x1000BAA8u);
        if(index>=73) return TRUE;
    }
    u32 model=rd<u32>(0x104B462C);
    u32 actor=rd<u32>(model,0xB8);
    s32 role=rd<s8>(0x10192C2C,index);
    if(!actor || role<0) return TRUE;
    u32 block=rd<u32>(model,0x2C); u16 flags=rd<u16>(block,4);
    u32 matrices=rd<u32>(block,0x10); wr<u16>(block,4,flags|0x10);
    u32 m=matrix(); gabi::call<void>(0x028E90D4,matrices+index*48,m);
    gabi::Local<cXyz> local;
    constexpr u32 debug=0x1047B608;
    if(role==100) {
        f32 z=rd<f32>(debug,0x34),y=rd<f32>(debug,0x30),x=rd<f32>(debug,0x2C);
        local->z=z; local->y=y; local->x=x;
        gabi::call<void>(0x0200FCD8,local.get(),actor+0x508);
        if(rd<u8>(actor,0x7090)==0) {
            u32 bx=rd<u32>(actor,0x508),by=rd<u32>(actor,0x50C);
            wr<u32>(actor,0x37C,bx); wr<u32>(actor,0x380,by);
            f32 eyeY=rd<f32>(actor,0x380),eyeX=rd<f32>(actor,0x37C);
            u32 bz=rd<u32>(actor,0x510); wr<f32>(actor,0x390,eyeX);
            wr<u32>(actor,0x384,bz); f32 eyeZ=rd<f32>(actor,0x384);
            wr<f32>(actor,0x394,gabi::fadds_ppc(eyeY,50.0f)); wr<f32>(actor,0x398,eyeZ);
        }
        y=rd<f32>(debug,0x30); z=rd<f32>(debug,0x34);
        x=rd<f32>(debug,0x2C);
        local->x=x; local->y=gabi::fadds_ppc(y,-50.0f); local->z=gabi::fadds_ppc(z,200.0f);
        gabi::call<void>(0x0200FCD8,local.get(),actor+0x61D8);
        s16 angle=rd<s16>(actor,0x5FB6); m=matrix(); gabi::call<void>(0x025F1C28,m,angle);
        writeJointMatrix(model,index);
        return TRUE;
    }
    if(role==101 || role==102) {
        s16 angle=rd<s16>(actor,0x422);
        if(role==102) angle=(s16)gabi::ftoi(gabi::fmuls_ppc((f32)angle,-0.6818f));
        m=matrix(); gabi::call<void>(0x025F1BF4,m,angle);
        writeJointMatrix(model,index);
        return TRUE;
    }
    if(role<=1) {local->x=0.0f;local->y=0.0f;local->z=300.0f;}
    else if(role==2) {
        f32 y=rd<f32>(debug,0xC),x=rd<f32>(debug,8),z=rd<f32>(debug,0x10);
        local->x=x;local->y=gabi::fadds_ppc(y,50.0f);local->z=gabi::fadds_ppc(z,-100.0f);
    } else if(role==8) {
        f32 x=rd<f32>(debug,0x1C); local->z=0.0f;local->y=100.0f;local->x=gabi::fadds_ppc(x,350.0f);
    } else if(role==10) {
        f32 x=rd<f32>(debug,0x1C);local->y=-100.0f;local->z=0.0f;local->x=-gabi::fadds_ppc(x,350.0f);
    } else {
        f32 x=rd<f32>(debug,0x20),y=rd<f32>(debug,0x24),z=rd<f32>(debug,0x28);
        local->x=x;local->z=z;local->y=y;
    }
    gabi::call<void>(0x0200FCD8,local.get(),actor+0x424+(u32)role*12);
    if(role==2) {
        s16 angle=rd<s16>(actor,0x5FB0); m=matrix(); gabi::call<void>(0x025F1C28,m,angle);
        angle=rd<s16>(actor,0x5FB2); m=matrix(); gabi::call<void>(0x025F1BF4,m,angle);
        angle=rd<s16>(actor,0x5FB0); m=matrix(); gabi::call<void>(0x025F1C5C,m,angle);
        writeJointMatrix(model,index,true);
    }
    return TRUE;
}
VERIFY(0x020F04EC,nodeCallBack);

void hahen_set2(btd_class* self) {
    WWHD_FUNC(0x020F0C68,void,self);
    u32 p=gabi::ea(self); gabi::Local<cXyz> position;
    f32 debug=rd<f32>(0x1047B608,0x40);
    f32 x=rd<f32>(p,0x484),y=rd<f32>(p,0x488),z=rd<f32>(p,0x48C);
    f32 height=gabi::fadds_ppc(debug,100.0f);
    position->z=z;position->x=x;position->y=gabi::fsubs_ppc(y,height);
    u32 control=rd<u32>(play(),0x5AB0);
    gabi::call<void>(0x025A847C,control,0,0x80AA,position.get(),0,0,255,0,-1,0,0,0);
    debug=rd<f32>(0x1047B608,0x40); x=rd<f32>(p,0x49C);
    height=gabi::fadds_ppc(debug,100.0f); y=rd<f32>(p,0x4A0); z=rd<f32>(p,0x4A4);
    position->x=x;position->z=z;position->y=gabi::fsubs_ppc(y,height);
    control=rd<u32>(play(),0x5AB0);
    gabi::call<void>(0x025A847C,control,0,0x80AA,position.get(),0,0,255,0,-1,0,0,0);
    s32 room=rd<s8>(p,0x326),reverb=gabi::call<s32>(0x02520540,room);
    gabi::call<void>(0x025E1A40,0x581B,p+0x484,0,reverb);
    room=rd<s8>(p,0x326);reverb=gabi::call<s32>(0x02520540,room);
    gabi::call<void>(0x025E1A40,0x581B,p+0x49C,0,reverb);
    u32 game=play(); s16 strength=rd<s16>(0x1047B608,0x84);
    position->x=0.0f;position->y=1.0f;position->z=0.0f;
    gabi::call<void>(0x025CB374,game+0x599C,(s32)strength+5,-33,position.get());
}
VERIFY(0x020F0C68,hahen_set2);

void btd_kankyo(btd_class* self) {
    WWHD_FUNC(0x020F94DC,void,self);
    u32 p=gabi::ea(self); s16 timer=rd<s16>(p,0x6208);
    if(timer) {wr<s16>(p,0x6208,(s16)(timer-1));wr<u8>(p,0x6207,2);}
    f32 time=rd<f32>(0x104629FC,0x2C); gabi::call<void>(0x0255FE50,time);
    u8 state=rd<u8>(p,0x6207); u32 palette=0;
    switch(state) {
    case 0: gabi::call<void>(0x0200EDC8,p+0x61FC,1.0f,0.04f); break;
    case 1:
        gabi::call<void>(0x0200ED84,p+0x6200,1.0f,1.0f,0.065f);
        wr<u8>(p,0x6204,(u8)rd<s16>(0x104629FC,0x38));
        wr<u8>(p,0x6205,(u8)rd<s16>(0x104629FC,0x3A));
        wr<u8>(p,0x6206,(u8)rd<s16>(0x104629FC,0x3C)); break;
    case 2:
        gabi::call<void>(0x0200ED84,p+0x61FC,1.0f,1.0f,0.2f);
        gabi::call<void>(0x0200ED84,p+0x6200,1.0f,1.0f,0.22500001f);
        time=rd<f32>(0x104629FC,0x30); gabi::call<void>(0x0255FE50,time);
        wr<u8>(p,0x6204,(u8)rd<s16>(0x104629FC,0x3E));
        wr<u8>(p,0x6205,(u8)rd<s16>(0x104629FC,0x40));
        wr<u8>(p,0x6206,(u8)rd<s16>(0x104629FC,0x42));break;
    case 3: gabi::call<void>(0x0200ED84,p+0x61FC,0.6f,1.0f,0.01f); break;
    case 4: gabi::call<void>(0x0200ED84,p+0x61FC,1.0f,1.0f,0.2f); break;
    case 5: gabi::call<void>(0x0200ED84,p+0x61FC,1.0f,1.0f,0.005f); palette=1;break;
    case 6: wr<f32>(p,0x61FC,0.0f);palette=2;break;
    case 7:
        gabi::call<void>(0x0200ED84,p+0x61FC,1.0f,1.0f,0.03333f);
        time=rd<f32>(0x104629FC,0x34);palette=2;gabi::call<void>(0x0255FE50,time);break;
    case 8: gabi::call<void>(0x0200EDC8,p+0x61FC,1.0f,0.06666f);palette=3;break;
    }
    u8 red=rd<u8>(p,0x6204); f32 brightness=rd<f32>(p,0x6200);
    u8 r=(u8)gabi::ftoi(gabi::fmuls_ppc((f32)red,brightness));
    u8 green=rd<u8>(p,0x6205),blue=rd<u8>(p,0x6206);
    u8 g=(u8)gabi::ftoi(gabi::fmuls_ppc((f32)green,brightness));
    wr<u16>(p,0x61E4,r);wr<u16>(p,0x61E6,g);
    u8 b=(u8)gabi::ftoi(gabi::fmuls_ppc((f32)blue,brightness));wr<u16>(p,0x61E8,b);
    f32 radius=rd<f32>(0x1047B608,0x28);
    wr<f32>(p,0x61EC,gabi::fmuls_ppc(gabi::fadds_ppc(radius,5000.0f),brightness));
    f32 fluctuation=rd<f32>(0x1047B608,0x2C);wr<f32>(p,0x61F0,fluctuation);
    f32 blend=rd<f32>(p,0x61FC);
    const u8 from[]={0,0,6,0},to[]={3,5,7,7};
    gabi::call<void>(0x0255FDA4,from[palette],to[palette],blend);
    wr<u8>(p,0x6207,0);gabi::call<void>(0x0200EDC8,p+0x6200,1.0f,0.025f);
}
VERIFY(0x020F94DC,btd_kankyo);

void btd_effect(btd_class* self) {
    WWHD_FUNC(0x020F90C4,void,self);
    u32 p=gabi::ea(self),morf=gabi::ea(get_anm(self));
    for(u32 i=0;i<5;++i) {
        u32 emitter=rd<u32>(p,0x6120+4*i);
        if(emitter) {
            u32 joint=rd<u32>(0x10192BC4,4*i),model=rd<u32>(morf,0x90);
            u32 block=rd<u32>(model,0x2C);u16 flags=rd<u16>(block,4);u32 matrices=rd<u32>(block,0x10);
            wr<u16>(block,4,flags|0x10);u32 m=matrix();
            gabi::call<void>(0x028E90D4,matrices+joint*48,m);
            if(rd<u8>(p,0x408)>=2) gabi::call<void>(0x0200FAD8,0,0.0f,-10000.0f,0.0f);
            emitter=rd<u32>(p,0x6120+4*i);m=matrix();
            gabi::call<void>(0x028249B0,m,emitter+0x1F0,emitter+0x22C);
        } else {
            u16 id=rd<u16>(0x10192C00,2*i);u32 control=rd<u32>(play(),0x5AB0);
            emitter=gabi::call<u32>(0x025A847C,control,0,id,p+0x314,0,0,255,0,-1,0,0,0);
            wr<u32>(p,0x6120+4*i,emitter);
        }
    }
    for(u32 i=0;i<7;++i) {
        s16 duration=rd<s16>(p,0x6150+2*i);
        if(duration) {
            wr<s16>(p,0x6150+2*i,(s16)(duration-1));
            u32 emitter=rd<u32>(p,0x6134+4*i);
            if(emitter) {
                u32 model=rd<u32>(morf,0x90),block=rd<u32>(model,0x2C);
                u32 jointOffset=0xAE0;
                if(i>=4) jointOffset=rd<s8>(p,0x412) ? 0xD80 : 0xCC0;
                u16 flags=rd<u16>(block,4);u32 matrices=rd<u32>(block,0x10);
                wr<u16>(block,4,flags|0x10);
                gabi::call<void>(0x028249B0,matrices+jointOffset,emitter+0x1F0,emitter+0x22C);
            } else if(i==6) {
                s8 side=rd<s8>(p,0x412),room=rd<s8>(p,0x326);
                u32 id=side ? 0xA0B4 : 0xA0B5,control=rd<u32>(play(),0x5AB0);
                emitter=gabi::call<u32>(0x025A847C,control,2,id,p+0x314,0,0,185,p+0x61B8,(s32)room,0,0,0);
                wr<u32>(p,0x614C,emitter);
            } else {
                u32 table=rd<s8>(p,0x412) ? 0x10192C1C : 0x10192C0C;
                u16 id=rd<u16>(table,2*i);u32 control=rd<u32>(play(),0x5AB0);
                emitter=gabi::call<u32>(0x025A847C,control,0,id,p+0x314,0,0,255,0,-1,0,0,0);
                wr<u32>(p,0x6134+4*i,emitter);
                if(i==5) {
                    emitter=rd<u32>(p,0x6148);u32 flags=rd<u32>(emitter,0x254);
                    wr<u32>(emitter,0x254,flags|0x40);
                }
            }
        } else {
            u32 emitter=rd<u32>(p,0x6134+4*i);
            if(emitter) {
                if(i!=3 && i!=6) {
                    if(i==5) {
                        u32 last=rd<u32>(p,0x6148);u32 flags=rd<u32>(last,0x254);
                        wr<u32>(last,0x254,flags&~0x40u);emitter=rd<u32>(p,0x6134+4*i);
                    }
                    u32 flags=rd<u32>(emitter,0x254);wr<s32>(emitter,0x5C,-1);wr<u32>(emitter,0x254,flags|1);
                }
                wr<u32>(p,0x6134+4*i,0);
            }
        }
    }
}
VERIFY(0x020F90C4,btd_effect);

btd_class* btd_class_ct(btd_class* self) {
    WWHD_FUNC(0x020F40A8,btd_class*,self);
    u32 p=gabi::ea(self);
    if(!p) p=gabi::call<u32>(0x0273AD10,0x70D8);
    if(!p) return nullptr;
    gabi::call<void>(0x025D4ED0,p);wr<u32>(p,0xB4,0x1000BA2C);
    gabi::call<void>(0x0200BD2C,p+0x514);gabi::call<void>(0x02515DA0,p+0x530);
    wr<u32>(p,0x52C,0x1004AE88);wr<u32>(p,0x530,0x1004AEC0);
    gabi::call<void>(0x028EFFD0,p+0x550,19,0x12C,0x025166F0u);
    gabi::call<void>(0x025166F0,p+0x1B94);gabi::call<void>(0x025166F0,p+0x1CC0);
    gabi::call<void>(0x028EFFD0,p+0x1DEC,50,0x12C,0x025166F0u);
    gabi::call<void>(0x028EFFD0,p+0x5884,6,0x130,0x020F9858u);
    gabi::call<void>(0x028EFFD0,p+0x6178,3,0x20,0x020F40A0u);
    wr<f32>(p,0x61F8,1.0f);
    constexpr u32 defaults=0x1016E414;
    f32 f[14];u8 b[4];s16 h[4];
    for(u32 i=0;i<6;++i) {f[i]=rd<f32>(defaults,4*i);wr<f32>(p,0x620C+4*i,f[i]);}
    for(u32 i=0;i<4;++i) {b[i]=rd<u8>(defaults,0x18+i);wr<u8>(p,0x6224+i,b[i]);}
    for(u32 i=0;i<4;++i) {h[i]=rd<s16>(defaults,0x1C+2*i);wr<s16>(p,0x6228+2*i,h[i]);}
    for(u32 i=0;i<8;++i) {f[6+i]=rd<f32>(defaults,0x24+4*i);if(i<7) wr<f32>(p,0x6230+4*i,f[6+i]);}
    wr<s16>(p,0x6370,h[2]);wr<f32>(p,0x624C,f[13]);
    wr<u8>(p,0x62E6,b[2]);wr<f32>(p,0x630C,f[13]);
    wr<f32>(p,0x62DC,f[4]);wr<f32>(p,0x62D8,f[3]);wr<u8>(p,0x6368,b[0]);
    wr<f32>(p,0x62E0,f[5]);wr<f32>(p,0x62F8,f[8]);wr<f32>(p,0x635C,f[3]);
    wr<u8>(p,0x6369,b[1]);wr<s16>(p,0x636C,h[0]);wr<s16>(p,0x62E8,h[0]);
    wr<f32>(p,0x6374,f[6]);wr<f32>(p,0x6300,f[10]);wr<s16>(p,0x62EE,h[3]);
    wr<f32>(p,0x6378,f[7]);wr<u8>(p,0x62E7,b[3]);wr<f32>(p,0x62F0,f[6]);
    wr<u8>(p,0x62E5,b[1]);wr<f32>(p,0x6358,f[2]);wr<f32>(p,0x62D0,f[1]);
    wr<f32>(p,0x6308,f[12]);wr<u8>(p,0x636B,b[3]);wr<f32>(p,0x62D4,f[2]);
    wr<f32>(p,0x6360,f[4]);wr<s16>(p,0x62EC,h[2]);wr<u8>(p,0x62E4,b[0]);
    wr<f32>(p,0x6350,f[0]);wr<s16>(p,0x6372,h[3]);wr<f32>(p,0x62CC,f[0]);
    wr<u8>(p,0x636A,b[2]);wr<f32>(p,0x637C,f[8]);wr<s16>(p,0x62EA,h[1]);
    wr<s16>(p,0x636E,h[1]);wr<f32>(p,0x6364,f[5]);wr<f32>(p,0x6354,f[1]);
    wr<f32>(p,0x62F4,f[7]);wr<f32>(p,0x6304,f[11]);wr<f32>(p,0x62FC,f[9]);
    wr<f32>(p,0x6380,f[9]);wr<f32>(p,0x6384,f[10]);wr<f32>(p,0x6388,f[11]);
    wr<f32>(p,0x638C,f[12]);wr<f32>(p,0x6390,f[13]);
    return gabi::at<btd_class>(p);
}
VERIFY(0x020F40A8,btd_class_ct);

s32 daBtd_Create(btd_class* self) {
    WWHD_FUNC(0x020F4308,s32,self);
    u32 p=gabi::ea(self),flags=rd<u32>(p,0x2E4);
    if(!(flags&8)) {
        if(p) {btd_class_ct(self);flags=rd<u32>(p,0x2E4);}
        wr<u32>(p,0x2E4,flags|8);
    }
    s32 status=gabi::call<s32>(0x02520460,p+0x3C8,0x1000BD00u);
    if(status!=4) return status;
    status=gabi::call<s32>(0x02520460,p+0x3D0,0x1000BD04u);
    if(status!=4) return status;
    wr<u32>(0x104629BC,0,0);wr<u32>(0x104629C0,0,0);
    wr<u8>(p,0x618A,1);wr<u8>(p,0x61AA,1);wr<u8>(p,0x61CA,1);
    u32 save=rd<u32>(0x101F84DC);
    s32 completed=gabi::call<s32>(0x025B9100,save+0x798,3);
    if(completed && rd<u8>(play(),0x5134)!=0x58) return 5;
    if(!gabi::call<s32>(0x025D63E8,p,0x020F3594u,0x5B8C0)) return 5;
    wr<u32>(p,0x39C,4);
    s32 child=gabi::call<s32>(0x025F0A10,0x1000BD18u,0x104629FCu);
    wr<u8>(0x104629FC,0,(u8)child);
    u32 stts=p+0x514;gabi::call<void>(0x02515F14,stts,255,255,p);
    for(u32 i=0;i<19;++i) {u32 sph=p+0x550+0x12C*i;gabi::call<void>(0x0251677C,sph,0x10192D88u);wr<u32>(sph,0x44,stts);}
    wr<u32>(p,0x1BD8,stts);gabi::call<void>(0x0251677C,p+0x1B94,0x10192DC8u);
    wr<u32>(p,0x1D04,stts);gabi::call<void>(0x0251677C,p+0x1CC0,0x10192E08u);
    for(u32 i=0;i<50;++i) {u32 sph=p+0x1DEC+0x12C*i;gabi::call<void>(0x0251677C,sph,0x10192E48u);wr<u32>(sph,0x44,stts);}
    for(u32 i=0;i<6;++i) {u32 cyl=p+0x5884+0x130*i;gabi::call<void>(0x02516518,cyl,0x10192E88u);wr<u32>(cyl,0x44,stts);}
    f32 random=gabi::call<f32>(0x020198D8,50.0f);
    f32 duration=rd<f32>(0x104629FC,0x10);
    wr<s16>(p,0x414,(s16)gabi::ftoi(gabi::fadds_ppc(random,duration)));
    gabi::call<void>(0x0255B9C8,p+0x61D8);wr<u8>(p,0x3A1,10);wr<u8>(p,0x3A0,10);
    save=rd<u32>(0x101F84DC);completed=gabi::call<s32>(0x025B9100,save+0x798,5);
    bool active=completed!=0;
    if(!active) active=rd<u8>(play(),0x5134)==0x58;
    if(active) {
        save=rd<u32>(0x101F84DC);gabi::call<void>(0x025B8B7C,save+0x1178,0x480);
        wr<f32>(p,0x70C4,-50.0f);wr<f32>(p,0x70C8,0.0f);
        u8 stage=rd<u8>(play(),0x5134);
        gabi::call<void>(0x025E18EC,stage==0x58?0x8000004Bu:0x80000003u);
        random=gabi::call<f32>(0x020198D8,50.0f);
        flags=rd<u32>(p,0x2E0);
        wr<s16>(p,0x414,(s16)gabi::ftoi(gabi::fadds_ppc(random,170.0f)));
        wr<u32>(p,0x2E0,flags|0x4020);wr<u8>(p,0x70D4,1);
    } else {
        wr<s16>(p,0x40C,10);wr<f32>(p,0x70C4,-50.0f);wr<f32>(p,0x70C8,-30.0f);
        s16 delay=rd<s16>(0x1047B608,0x88);wr<s16>(p,0x414,(s16)(delay+150));
        f32 height=rd<f32>(0x1047B608,0x50);flags=rd<u32>(p,0x2E0);
        wr<u8>(p,0x70D4,1);wr<u32>(p,0x2E0,flags|0x4020);
        wr<f32>(p,0x70C0,gabi::fadds_ppc(height,2300.0f));
    }
    u32 game=play();s32 magma=gabi::call<s32>(0x02524C10,game+0x12A0);
    if(magma) {
        gabi::Local<cXyz> position;position->x=0.0f;position->z=0.0f;position->y=-300.0f;
        u32 packet=rd<u32>(play(),0x5AB4);s32 room=rd<s8>(p,0x326);
        u32 floor=gabi::call<u32>(0x0258CC64,packet,position.get(),p+0x330,room,-250);
        wr<u32>(p,0x70D0,floor);
        if(!floor) gabi::call<void>(0x0273AA24,0x1000BD0Cu,0x189A,0x1000BD24u);
    }
    gabi::call<void>(0x025D5834,0x18D,0x10FF00,p+0x314,-1,0,0,-1,0);
    f32 time=rd<f32>(0x104629FC,0x2C);gabi::call<void>(0x0255FE50,time);
    return 4;
}
VERIFY(0x020F4308,daBtd_Create);

void hahen_set(btd_class* self,s32 side) {
    WWHD_FUNC(0x020F0EB4,void,self,side);
    u32 p=gabi::ea(self);s16 x=rd<s16>(p,0x320),y=rd<s16>(p,0x322),z=rd<s16>(p,0x324);
    wr<s16>(p,0x6172,y);wr<s16>(p,0x6174,z);wr<s16>(p,0x6170,x);
    wr<s16>(p,0x6172,(s16)(y+(side?0x4000:-0x4000)));
    u32 source=p+(side?0x49C:0x484);
    // cXyz assignment: copied as words (lwz/stw), so a signalling-NaN bit pattern is kept
    u32 px=rd<u32>(source),pyBits=rd<u32>(source,4),pz=rd<u32>(source,8);
    wr<u32>(p,0x6164,px);wr<u32>(p,0x6168,pyBits);wr<u32>(p,0x616C,pz);
    f32 py=rd<f32>(source,4);
    f32 debug=rd<f32>(0x1047B648);
    wr<f32>(p,0x6168,gabi::fsubs_ppc(py,gabi::fadds_ppc(debug,100.0f)));
    u32 modelData=resource(0x1000B9EC,0x30),animation=resource(0x1000B9EC,0x66);
    u32 control=rd<u32>(play(),0x5AB0);
    u32 emitter=gabi::call<u32>(0x025A847C,control,0,0x80A8,p+0x6164,p+0x6170,0,255,0,-1,0,0,0);
    if(emitter) {
        wr<f32>(emitter,0x23C,4.0f);wr<f32>(emitter,0x240,4.0f);wr<f32>(emitter,0x238,4.0f);
        u32 modelEmitter=gabi::call<u32>(0x025A3BDC,0,emitter,modelData,1,p+0x620C,animation,1,0);
        if(modelEmitter) {control=rd<u32>(play(),0x5AB0);u32 list=rd<u32>(control,0x130);gabi::call<void>(0x0200FE78,list,modelEmitter);}
    }
    control=rd<u32>(play(),0x5AB0);
    gabi::call<void>(0x025A847C,control,0,0x80A9,p+0x6164,0,0,255,0,-1,0,0,0);
    smoke_set_s(self,gabi::at<cXyz>(p+0x6164),gabi::at<csXyz>(p+0x6170));
}
VERIFY(0x020F0EB4,hahen_set);

namespace {
void actorSound(u32 p,u32 id) {
    if(!(p+0x37C)) return;
    s32 actorId=p?(s32)rd<u32>(p,4):-1;
    s32 room=rd<s8>(p,0x326);s32 reverb=gabi::call<s32>(0x02520540,room);
    gabi::call<void>(0x025E1AA4,id,p+0x37C,actorId,0,reverb);
}
// Most monster-sound sites in move test the actor pointer itself (cmpwi r31,0) before the sound position
// test (addic. r31,0x37C); the others test only the position.
void actorSoundNN(u32 p,u32 id) { if(p) actorSound(p,id); }
}
void startdemo(btd_class* self) {
    WWHD_FUNC(0x020F4924,void,self);
    u32 p=gabi::ea(self),player=rd<u32>(play(),0x5B2C);
    wr<s16>(p,0x418,30);
    u32 target=rd<u32>(play(),0x5B2C);s16 angle=gabi::call<s16>(0x025D6894,p,target);wr<s16>(p,0x322,angle);
    gabi::Local<cXyz> center,difference;
    center->z=909.0f;center->x=-1534.0f;center->y=0.0f;
    gabi::call<void>(0x0201ADE0,player+0x314,difference.get(),center.get());
    u16 mode=(u16)rd<s16>(p,0x40E);
    if(mode==0) {
        if(rd<s16>(p,0x705A)!=0) return;
        wr<u8>(p,0x70CC,0);wr<u8>(p,0x6207,6);wr<f32>(p,0x318,-10000.0f);
        if(rd<u16>(p,0xF8)==2) {wr<s16>(p,0x705A,1);gabi::call<void>(0x025E1944);return;}
        f32 magnitude=gabi::call<f32>(0x028E8DD0,difference.get());f32 distance=gabi::call<f32>(0x028F4384,magnitude);
        f32 radius=rd<f32>(0x1047B608,0x3C);
        if(distance>gabi::fadds_ppc(radius,200.0f)) {
            gabi::call<void>(0x025D7B24,p,2,0xFFFF,0);
            u16 flags=rd<u16>(p,0xFA);wr<u16>(p,0xFA,flags|2);
        }
        return;
    }
    if(mode==1) {
        wr<u8>(p,0x6207,8);anm_init(self,0x2D,1.0f,0,1.0f,-1);
        f32 y=rd<f32>(p,0x2F0);wr<s16>(p,0x40E,2);wr<f32>(p,0x318,y);
        actorSound(p,0x483D);
        center->x=0.0f;center->y=0.0f;center->z=0.0f;
        u32 control=rd<u32>(play(),0x5AB0);
        gabi::call<void>(0x025A847C,control,0,0x80BD,center.get(),p+0x320,0,255,0,-1,0,0,0);
        return;
    }
    if(mode!=2) return;
    s16 timer=rd<s16>(p,0x705E);
    if(timer<=70) {timer=rd<s16>(p,0x705E);wr<u8>(p,0x6207,8);}
    if(timer==40 || timer==220) {actorSound(p,0x4837);timer=rd<s16>(p,0x705E);}
    if(timer==215) {wave_set(self);wr<f32>(p,0x708C,40.0f);hahen_set2(self);timer=rd<s16>(p,0x705E);}
    if((u32)((s32)timer-70)<126) {
        timer=rd<s16>(p,0x705E);wr<s16>(p,0x6156,3);
        if(timer>=90) {
            if(p+0x37C) {s32 room=rd<s8>(p,0x326),reverb=gabi::call<s32>(0x02520540,room);gabi::call<void>(0x025E1A40,0x501F,p+0x37C,0,reverb);}
            timer=rd<s16>(p,0x705E);wr<u8>(p,0x6207,2);
        }
    }
    u32 morf=rd<u32>(p,0x3D8);if(timer>=70) wr<s16>(p,0x6154,3);
    if(!(rd<u8>(morf,0xA7)&1) && rd<f32>(morf,0x98)!=0.0f) return;
    if(rd<f32>(0x1047B608,0x24)!=0.0f) return;
    wr<s16>(p,0x40C,0);wr<s16>(p,0x705A,150);wr<s16>(p,0x40E,0);
    f32 random=gabi::call<f32>(0x020198D8,50.0f);
    wr<s16>(p,0x414,(s16)gabi::ftoi(gabi::fadds_ppc(random,70.0f)));
    u8 stage=rd<u8>(play(),0x5134);gabi::call<void>(0x025E18EC,stage==0x58?0x8000004Bu:0x80000003u);
    u32 save=rd<u32>(0x101F84DC);gabi::call<void>(0x025B9098,save+0x798,5);
}
VERIFY(0x020F4924,startdemo);



namespace {
void installGohmaJoints(u32 actor,u32 morfOffset) {
    u32 morf=rd<u32>(actor,morfOffset),model=rd<u32>(morf,0x90),data=rd<u32>(model,0xAC);
    u32 jointInfo=gabi::call<u32>(0x027F3F94,data);u32 count=rd<u16>(jointInfo,8),index=0;
    while(index<count) {
        if(index<73 && rd<s8>(0x10192C2C,index)>=0) {
            morf=rd<u32>(actor,morfOffset);model=rd<u32>(morf,0x90);data=rd<u32>(model,0xAC);
            u32 entries=rd<u32>(data,4),joint=rd<u32>(data,8);
            if(index<entries) joint+=index*28;
            wr<u32>(joint,8,0x020F04EC);
        }
        morf=rd<u32>(actor,morfOffset);model=rd<u32>(morf,0x90);
        index=(index+1)&0xFFFF;data=rd<u32>(model,0xAC);
        jointInfo=gabi::call<u32>(0x027F3F94,data);count=rd<u16>(jointInfo,8);
    }
    morf=rd<u32>(actor,morfOffset);model=rd<u32>(morf,0x90);wr<u32>(model,0xB8,actor);
}
u32 newGohmaAnimation(u32 actor,u32 offset,bool texture,u32 assertionLine,u32 assertionText,bool assertion) {
    u32 handle=gabi::call<u32>(0x0273AD10,texture?0x74:0x78);
    if(handle) handle=gabi::call<u32>(texture?0x025E7C6C:0x025E80D0,handle);
    wr<u32>(actor,offset,handle);
    if(!handle && assertion) gabi::call<void>(0x0273AA24,0x1000BC54u,assertionLine,assertionText);
    return handle;
}
s32 initGohmaAnimation(u32 actor,u32 offset,u32 model,u32 asset,bool texture,s32 loop,s32 second) {
    u32 animation=resource(0x1000BC50,asset);
    u32 handle,data;
    // Phase one's native code loads the animation handle before the model data;
    // the other groups load data first. Neither load crosses a guest call.
    handle=rd<u32>(actor,offset);data=rd<u32>(model,0xAC);
    return gabi::call<s32>(texture?0x025E7CE0:0x025E8154,handle,data,animation,1,loop,1.0f,0,-1,second);
}
}
BOOL useHeapInit(btd_class* self) {
    WWHD_FUNC(0x020F3594,BOOL,self);
    u32 p=gabi::ea(self);
    u32 modelData=resource(0x1000BC50,0x40),animation=resource(0x1000BC50,0x33);
    u32 morf=gabi::call<u32>(0x025E4F64,0,modelData,0,0,animation,2,1.0f,0,-1,0,0,0,0x11020203u);
    wr<u32>(p,0x3D8,morf);if(!morf || !rd<u32>(morf,0x90)) return FALSE;
    installGohmaJoints(p,0x3D8);
    u32 model=rd<u32>(rd<u32>(p,0x3D8),0x90);
    newGohmaAnimation(p,0x3DC,true,0x156F,0x1000BC60,true);
    if(!initGohmaAnimation(p,0x3DC,model,0x52,true,2,0)) return FALSE;
    newGohmaAnimation(p,0x3E0,false,0x1583,0x1000BC6C,true);
    if(!initGohmaAnimation(p,0x3E0,model,0x48,false,2,0)) return FALSE;
    modelData=resource(0x1000BC50,0x42);animation=resource(0x1000BC50,0x33);
    morf=gabi::call<u32>(0x025E4F64,0,modelData,0,0,animation,2,1.0f,0,-1,0,0,0,0x11020203u);
    wr<u32>(p,0x3E4,morf);if(!morf || !rd<u32>(morf,0x90)) return FALSE;
    installGohmaJoints(p,0x3E4);model=rd<u32>(rd<u32>(p,0x3E4),0x90);
    newGohmaAnimation(p,0x3E8,true,0x15AF,0x1000BC78,true);
    if(!initGohmaAnimation(p,0x3E8,model,0x53,true,0,0)) return FALSE;
    initGohmaAnimation(p,0x3E8,model,0x56,true,2,1);
    newGohmaAnimation(p,0x3EC,false,0x15CE,0x1000BC88,true);
    if(!initGohmaAnimation(p,0x3EC,model,0x49,false,0,0)) return FALSE;
    initGohmaAnimation(p,0x3EC,model,0x4F,false,2,1);
    modelData=resource(0x1000BC50,0x45);animation=resource(0x1000BC50,0x33);
    morf=gabi::call<u32>(0x025E4F64,0,modelData,0,0,animation,2,1.0f,0,-1,0,0,0,0x11020203u);
    wr<u32>(p,0x3F0,morf);if(!morf || !rd<u32>(morf,0x90)) return FALSE;
    installGohmaJoints(p,0x3F0);model=rd<u32>(rd<u32>(p,0x3F0),0x90);
    newGohmaAnimation(p,0x3F4,true,0x1606,0x1000BC98,true);
    if(!initGohmaAnimation(p,0x3F4,model,0x53,true,0,0)) return FALSE;
    newGohmaAnimation(p,0x3F8,false,0x161D,0x1000BCAC,true);
    if(!initGohmaAnimation(p,0x3F8,model,0x49,false,0,0)) return FALSE;
    modelData=resource(0x1000BC50,0x41);
    morf=gabi::call<u32>(0x025E4F64,0,modelData,0,0,0,2,1.0f,0,-1,0,0,0,0x11020203u);
    wr<u32>(p,0x3FC,morf);if(!morf || !rd<u32>(morf,0x90)) return FALSE;
    gabi::call<void>(0x0200FC74,0,0.0f,0.0f,0.0f);
    morf=rd<u32>(p,0x3FC);u32 m=matrix();model=rd<u32>(morf,0x90);
    f32 values[12];for(u32 i=0;i<12;++i) values[i]=rd<f32>(m,4*i);
    const u8 order[]={5,3,6,7,2,0,9,8,1,10,4,11};
    for(u8 i:order) wr<f32>(model,0xC8+4*i,values[i]);
    morf=rd<u32>(p,0x3FC);gabi::call<void>(0x025E55A0,morf);
    model=rd<u32>(rd<u32>(p,0x3FC),0x90);
    if(!newGohmaAnimation(p,0x400,true,0,0,false)) return FALSE;
    if(!initGohmaAnimation(p,0x400,model,0x55,true,0,0)) return FALSE;
    if(!newGohmaAnimation(p,0x404,false,0,0,false)) return FALSE;
    if(!initGohmaAnimation(p,0x404,model,0x4E,false,0,0)) return FALSE;
    u32 large=resource(0x1000BC50,0x3C),small=resource(0x1000BC50,0x3D);
    if(!large || !small) gabi::call<void>(0x0273AA24,0x1000BC54u,0x166C,0x1000BCC0u);
    for(u32 i=0;i<35;++i) {
        u32 model=gabi::call<u32>(0x025E38E0,i<5?small:large,0,0x11020203u);
        u32 piece=p+0x63D8+i*40;wr<u32>(piece,0,model);
        if(!model) gabi::call<void>(0x0273AA24,0x1000BC54u,0x16A0,0x1000BCDCu);
        f32 x=gabi::call<f32>(0x020198D8,0.5f);x=gabi::fadds_ppc(x,1.0f);
        f32 y=gabi::call<f32>(0x020198D8,0.5f);y=gabi::fadds_ppc(y,1.0f);
        f32 z=gabi::call<f32>(0x020198D8,0.5f);model=rd<u32>(piece);z=gabi::fadds_ppc(z,1.0f);
        wr<f32>(model,0xBC,x);wr<f32>(model,0xC0,y);wr<f32>(model,0xC4,z);
    }
    return TRUE;
}
VERIFY(0x020F3594,useHeapInit);

namespace {
void btdActionAnimation(u32 p,s32 index,f32 blend,u8 loop=2,f32 speed=1.0f,s32 sound=-1) {
    gabi::call<void>(0x020F03BC,p,index,blend,loop,speed,sound);
}
void btdActionWait(u32 p) {
    u32 player=rd<u32>(play(),0x5B2C);
    play();
    s32 playerAngle=gabi::call<s32>(0x025D6894,p,rd<u32>(play(),0x5B2C));
    s16 delta=s16(rd<s16>(p,0x322)-playerAngle);
    u16 view=u16(delta<0?s16(-delta):delta);
    f32 height=rd<f32>(0x1047B608,0x1C)+400.0f;
    bool high=rd<f32>(player,0x318)>height;
    u16 state=u16(rd<s16>(p,0x40E));
    switch(state) {
    case 0:
        btdActionAnimation(p,0x33,20.0f);
        wr<s16>(p,0x40E,s16(rd<s16>(p,0x40E)+1));
        [[fallthrough]];
    case 1:
        if(view<0x1800 && rd<s16>(p,0x414)==0) {
            wr<s16>(p,0x40C,1); wr<s16>(p,0x40E,0);
            f32 choice=gabi::call<f32>(0x020198D8,1.0f);
            if(choice<0.5f) { wr<u8>(p,0x411,0); wr<u8>(p,0x412,1); }
            else { wr<u8>(p,0x411,5); wr<u8>(p,0x412,0); }
            u8 forced=rd<u8>(0x104629FC,0xC);
            if(forced==0) {
                if(view>0x400) {
                    f32 turnChoice=gabi::call<f32>(0x020198D8,1.0f);
                    if(turnChoice<0.5f || view>0x800) { wr<u8>(p,0x410,5); break; }
                }
                bool phase=rd<u8>(p,0x408)!=0;
                f32 first=gabi::call<f32>(0x020198D8,1.0f);
                if(first<rd<f32>(0x104629FC,phase?0x1C:0x14)) wr<u8>(p,0x410,0);
                else {
                    f32 second=gabi::call<f32>(0x020198D8,1.0f);
                    wr<u8>(p,0x410,second<rd<f32>(0x104629FC,phase?0x20:0x18)?1:2);
                }
            } else if(forced<=3) wr<u8>(p,0x410,forced-1);
            else if(forced==4) { wr<u8>(p,0x410,5);wr<u8>(p,0x411,5);wr<u8>(p,0x412,0); }
        } else {
            wr<s16>(p,0x5FBC,0x100);
            if(view>=0x6000 && rd<s16>(p,0x416)==0) {
                wr<s16>(p,0x40E,2); btdActionAnimation(p,0x15,10.0f);
                wr<s16>(p,0x414,150); wr<u8>(p,0x5FAE,1);
            } else {
                if(high) {
                    u32 peg=rd<u32>(0x104629BC);
                    wr<s16>(p,0x40E,peg!=0 && rd<s8>(peg,0x3A1)==3?10:20);
                }
                wr<u8>(p,0x5FAE,1);
            }
        }
        break;
    case 2:
        if(view<0x6000 || rd<s16>(p,0x414)==0) {wr<s16>(p,0x40E,0);wr<s16>(p,0x416,40);}
        break;
    case 10:
        btdActionAnimation(p,0x2F,20.0f);
        wr<s16>(p,0x40E,s16(rd<s16>(p,0x40E)+1));wr<s16>(p,0x414,300);
        {
            s16 timer=s16(gabi::ftoi(gabi::call<f32>(0x020198D8,80.0f)+90.0f));
            wr<s16>(p,0x416,timer);
            if(timer!=0) goto waitHigh;
        }
        [[fallthrough]];
    case 11:
        if((state==10 || rd<s16>(p,0x416)==0) && rd<s16>(p,0x414)>150) {
            actorSoundNN(p,0x483E);
            wr<s16>(p,0x416,s16(gabi::ftoi(gabi::call<f32>(0x020198D8,80.0f)+90.0f)));
        }
    waitHigh:
        wr<u8>(p,0x5FAE,3); wr<s16>(p,0x5FBC,0x100);
        {
            s16 facing=rd<s16>(player,0x32A), timer=rd<s16>(p,0x414);
            wr<s16>(p,0x5FB8,s16(facing+0x4000));
            if(!high) wr<s16>(p,0x40E,0);
            else { u32 peg=rd<u32>(0x104629BC); if(peg!=0 && rd<s8>(peg,0x3A1)!=3) wr<s16>(p,0x40E,0); }
            if(timer==0) {wr<u8>(p,0x410,3);wr<s16>(p,0x40C,1);wr<s16>(p,0x40E,0);}
        }
        break;
    case 20:
        btdActionAnimation(p,0x39,20.0f);
        wr<s16>(p,0x40E,s16(rd<s16>(p,0x40E)+1));wr<s16>(p,0x414,150);
        [[fallthrough]];
    case 21:
        {
            s16 timer=rd<s16>(p,0x414);
            wr<s16>(p,0x5FBC,0x100);wr<u8>(p,0x5FAE,1);
            if(!high) wr<s16>(p,0x40E,0);
            else {u32 peg=rd<u32>(0x104629BC); if(peg!=0 && rd<s8>(peg,0x3A1)==3) wr<s16>(p,0x40E,0);}
            if(timer==0) {wr<u8>(p,0x410,4);wr<s16>(p,0x40C,1);wr<s16>(p,0x40E,0);}
        }
        break;
    }
}
}

namespace {
bool btdActionStopped(u32 morf) { return (rd<u8>(morf,0xA7)&1)!=0 || rd<f32>(morf,0x98)==0.0f; }
void btdActionReset(u32 p) {
    wr<s16>(p,0x40C,0);wr<s16>(p,0x40E,0);
    f32 delay=gabi::call<f32>(0x020198D8,50.0f);
    wr<s16>(p,0x414,s16(gabi::ftoi(delay+rd<f32>(0x104629FC,0x10))));
}
void btdActionLevelSound(u32 p,u32 id,u32 position) {
    s32 reverb=gabi::call<s32>(0x02520540,s32(rd<s8>(p,0x326)));
    gabi::call<void>(0x025E1A40,id,position,0,reverb);
}
void btdActionBodyLevelSound(u32 p,u32 id) {
    if(p!=0 && p+0x37C!=0) btdActionLevelSound(p,id,p+0x37C);
}
void btdActionNextAnimation(u32 p,u32 table,f32 blend,u8 loop=0) {
    s8 index=s8(rd<u8>(p,0x411)+1);
    wr<u8>(p,0x411,u8(index));
    s32 animation=rd<s32>(table+u32(s32(index)*4));
    btdActionAnimation(p,animation,blend,loop);
}
void btdActionIncrement(u32 p) { wr<s16>(p,0x40E,s16(rd<s16>(p,0x40E)+1)); }
void btdActionJab(u32 p) {
    u32 morf=gabi::call<u32>(0x020F032C,p);
    switch(u16(rd<s16>(p,0x40E))) {
    case 0:
        btdActionAnimation(p,rd<s32>(0x10192B08+u32(s32(rd<s8>(p,0x411))*4)),5.0f,0);
        actorSoundNN(p,0x4834);btdActionIncrement(p);break;
    case 1:
        if(btdActionStopped(morf)) {
            btdActionNextAnimation(p,0x10192B08,0.0f);btdActionIncrement(p);
            s32 joint=rd<s8>(p,0x412)!=0?10:8;
            btdActionLevelSound(p,0x5818,p+0x424+u32(joint*12));
        }
        break;
    case 2:
        wr<u8>(p,0x5FAC,1);
        if(btdActionStopped(morf)) { btdActionNextAnimation(p,0x10192B08,0.0f);btdActionIncrement(p); }
        break;
    case 3: if(btdActionStopped(morf)) btdActionReset(p);break;
    }
}
void btdActionPunch(u32 p) {
    u32 morf=gabi::call<u32>(0x020F032C,p);
    s16 state=rd<s16>(p,0x40E);
    switch(u16(state)) {
    case 0:
        btdActionAnimation(p,rd<s32>(0x10192B28+u32(s32(rd<s8>(p,0x411))*4)),5.0f,0);
        btdActionIncrement(p);actorSoundNN(p,0x4835);state=rd<s16>(p,0x40E);break;
    case 1:
        if(btdActionStopped(morf)) {
            btdActionNextAnimation(p,0x10192B28,0.0f);btdActionIncrement(p);
            s32 joint=rd<s8>(p,0x412)!=0?10:8;
            btdActionLevelSound(p,0x5819,p+0x424+u32(joint*12));
            state=rd<s16>(p,0x40E);
        }
        break;
    case 2:
        wr<u8>(p,0x5FAC,2);
        if(btdActionStopped(morf)) {
            btdActionNextAnimation(p,0x10192B28,0.0f);btdActionIncrement(p);
            s32 joint=rd<s8>(p,0x412)!=0?10:8;
            btdActionLevelSound(p,0x581B,p+0x424+u32(joint*12));
            gabi::call<void>(0x020F0EB4,p,s32(rd<s8>(p,0x412)));
            u32 game=play();s32 magnitude=rd<s16>(0x1047B608,0x84)+5;
            gabi::Local<cXyz> direction;direction->x=0.0f;direction->y=1.0f;direction->z=0.0f;
            gabi::call<void>(0x025CB374,game+0x599C,magnitude,-33,direction.get());
            state=rd<s16>(p,0x40E);
        } else state=rd<s16>(p,0x40E);
        break;
    case 3:
        if(btdActionStopped(morf)) {
            btdActionNextAnimation(p,0x10192B28,0.0f,2);
            f32 random=gabi::call<f32>(0x020198D8,100.0f);
            f32 timer=random+rd<f32>(0x104629FC,0x24);
            state=s16(rd<s16>(p,0x40E)+1);
            wr<s16>(p,0x414,s16(gabi::ftoi(timer)));wr<s16>(p,0x40E,state);
        }
        break;
    case 4: {
        s32 joint=rd<s8>(p,0x412)!=0?10:8;
        if(gabi::ftoi(rd<f32>(morf,0x9C))==2) {
            btdActionLevelSound(p,0x581C,p+0x424+u32(joint*12));
            if(p+0x37C!=0) actorSound(p,0x4838);
        }
        if(rd<s16>(p,0x414)==0) {
            btdActionNextAnimation(p,0x10192B28,5.0f);btdActionIncrement(p);
            actorSoundNN(p,0x4839);btdActionLevelSound(p,0x581D,p+0x424+u32(joint*12));
            wr<s16>(p,0x615A,23);wr<s16>(p,0x6158,23);wr<s16>(p,0x615C,3);
        }
        state=rd<s16>(p,0x40E);break;
    }
    case 5:
        if(btdActionStopped(morf)) {
            wr<s16>(p,0x40C,0);wr<s16>(p,0x40E,0);
            f32 delay=gabi::call<f32>(0x020198D8,50.0f);
            f32 total=delay+rd<f32>(0x104629FC,0x10);
            state=rd<s16>(p,0x40E);wr<s16>(p,0x414,s16(gabi::ftoi(total)));
        }
        break;
    }
    if(state==3 || state==4) wr<u8>(p,0x5FA4,u8(rd<u8>(p,0x412)+2));
}
}

namespace {
void btdActionFireMatrix(u32 morf) {
    u32 block=rd<u32>(rd<u32>(morf,0x90),0x2C);
    u16 flags=rd<u16>(block,4);u32 matrices=rd<u32>(block,0x10);
    wr<u16>(block,4,flags|0x10);
    gabi::call<void>(0x028E90D4,matrices+0xAE0,matrix());
}
void btdActionFireCollision(u32 p,u32 morf,bool swept) {
    btdActionFireMatrix(morf);
    f32 extent=rd<f32>(p,0x5FA8);
    gabi::Local<cXyz> local,position;
    local->x=0.0f;local->y=-200.0f*extent;local->z=1000.0f*extent;
    gabi::call<void>(0x0200FCD8,local.get(),position.get());
    gabi::call<void>(0x02018C8C,p+0x1DD8,200.0f*rd<f32>(p,0x5FA8));
    if(swept) {
        bool first=rd<s16>(p,0x414)==1;
        gabi::call<void>(first?0x025167C0:0x025167E4,p+0x1CC0,position.get());
    } else gabi::call<void>(0x02018D40,p+0x1DD8,position.get());
    gabi::call<void>(0x0200E240,play()+0x26A4,p+0x1CC0);
}
void btdActionFire(u32 p) {
    u32 morf=gabi::call<u32>(0x020F032C,p);
    s16 state=rd<s16>(p,0x40E);
    wr<u8>(p,0x5FAE,2);
    switch(u16(state)) {
    case 0:
        btdActionAnimation(p,0xD,5.0f,0);btdActionIncrement(p);
        state=rd<s16>(p,0x40E);wr<f32>(p,0x70C0,0.0f);wr<s16>(p,0x414,40);break;
    case 1: {
        s16 target=s16(rd<s16>(0x1047B608,0x8C)+16);
        s16 timer=rd<s16>(p,0x414);
        if(timer==target) {gabi::call<void>(0x020F0C68,p);timer=rd<s16>(p,0x414);}
        if(timer==31) actorSoundNN(p,0x483A);
        if(btdActionStopped(morf)) {btdActionAnimation(p,0xE,0.0f,0);btdActionIncrement(p);}
        state=rd<s16>(p,0x40E);break;
    }
    case 2:
        if(btdActionStopped(morf)) {
            if(rd<u8>(p,0x408)!=0) {
                wr<s16>(p,0x40E,6);btdActionAnimation(p,0x11,0.0f,0);actorSoundNN(p,0x5810);
            } else {
                btdActionAnimation(p,0xF,0.0f,2);
                wr<s16>(p,0x414,s16(rd<s16>(0x104629FC,0x28)*9));wr<s16>(p,0x40E,3);
            }
        }
        state=rd<s16>(p,0x40E);break;
    case 3: {
        wr<u8>(p,0x6207,1);btdActionBodyLevelSound(p,0x501E);
        if(rd<s16>(p,0x414)>9) wr<s16>(p,0x6152,1);
        s16 spawnTimer=s16(rd<s16>(0x1047B608,0x8C)+20),timer=rd<s16>(p,0x414);
        wr<s16>(p,0x6150,spawnTimer);
        if(timer==0) {
            if(rd<u8>(p,0x408)!=0) {btdActionAnimation(p,0x11,0.0f,0);wr<s16>(p,0x40E,5);actorSoundNN(p,0x5810);}
            else {
                btdActionAnimation(p,0x10,0.0f,0);btdActionIncrement(p);
                wr<s16>(p,0x414,s16(rd<s16>(0x1047B608,0x86)+25));btdActionBodyLevelSound(p,0x581A);
            }
            wr<u32>(p,0x6140,0);
        }
        state=rd<s16>(p,0x40E);break;
    }
    case 4: {
        wr<s16>(p,0x6154,12);wr<s16>(p,0x6156,3);
        if(btdActionStopped(morf)) btdActionReset(p);
        if(rd<s16>(p,0x414)>1) wr<f32>(p,0x5FA8,0.0f);
        else {
            wr<u8>(p,0x6207,2);btdActionBodyLevelSound(p,0x501F);
            btdActionFireCollision(p,morf,true);
            f32 next=rd<f32>(p,0x5FA8)+0.1f;
            if(next>1.0f) wr<f32>(p,0x5FA8,0.0f);else wr<f32>(p,0x5FA8,next);
        }
        state=rd<s16>(p,0x40E);break;
    }
    case 5:
        wr<u8>(p,0x5FAC,3);
        if(btdActionStopped(morf)) {
            btdActionAnimation(p,0x10,0.0f,0);wr<s16>(p,0x40E,4);
            wr<s16>(p,0x414,s16(rd<s16>(0x1047B608,0x86)+25));btdActionBodyLevelSound(p,0x581A);
        }
        state=rd<s16>(p,0x40E);break;
    case 6:
        wr<u8>(p,0x5FAC,3);
        if(btdActionStopped(morf)) {
            btdActionAnimation(p,0xF,15.0f,2);
            wr<s16>(p,0x414,s16(rd<s16>(0x104629FC,0x28)*4));wr<s16>(p,0x40E,3);
        }
        state=rd<s16>(p,0x40E);break;
    }
    if(state>=2 || rd<s16>(p,0x414)<16) wr<u8>(p,0x5FA4,1);
}
}

namespace {
void btdActionHighFire(u32 p,bool sideFire) {
    u32 player=rd<u32>(play(),0x5B2C);
    u32 morf=gabi::call<u32>(0x020F032C,p);
    u16 state=u16(rd<s16>(p,0x40E));
    wr<u8>(p,0x5FAE,sideFire?1:3);
    switch(state) {
    case 0:
        btdActionAnimation(p,sideFire?0x36:0x30,5.0f,0);btdActionIncrement(p);wr<s16>(p,0x414,40);break;
    case 1:
        if(btdActionStopped(morf)) {
            btdActionAnimation(p,sideFire?0x37:0x31,1.0f,2);
            s16 duration=rd<s16>(0x104629FC,0x28),next=s16(rd<s16>(p,0x40E)+1);
            wr<s16>(p,0x414,s16(duration*(sideFire?7:9)));wr<s16>(p,0x40E,next);
        }
        break;
    case 2: {
        wr<u8>(p,0x6207,1);btdActionBodyLevelSound(p,0x501E);
        s16 timer=rd<s16>(p,0x414);wr<s16>(p,0x6152,1);
        s16 sprayTimer=s16(rd<s16>(0x1047B608,0x8C)+20);wr<s16>(p,0x6150,sprayTimer);
        if(timer==0) {
            btdActionAnimation(p,sideFire?0x38:0x32,5.0f,0);btdActionIncrement(p);
            wr<s16>(p,0x414,s16(rd<s16>(0x1047B608,0x86)+25));btdActionBodyLevelSound(p,0x581A);
            wr<u32>(p,0x6140,0);
        }
        break;
    }
    case 3: {
        if(sideFire) {
            gabi::Local<cXyz> difference;
            gabi::call<void>(0x0201ADE0,player+0x37C,difference.get(),p+0x37C);
            f32 z=difference->z,x=difference->x,y=difference->y;
            f32 length=gabi::call<f32>(0x028F4384,gabi::fmadds(x,x,z*z));
            s32 pitch=gabi::call<s32>(0x020195B0,y,length);
            wr<s16>(p,0x6154,12);wr<s16>(p,0x6156,3);
            s16 timer=rd<s16>(p,0x414);wr<s16>(p,0x5FB4,s16(-pitch-2500));
            if(timer!=0) wr<f32>(p,0x5FA8,0.0f);
            else {
                wr<u8>(p,0x6207,2);btdActionBodyLevelSound(p,0x501F);
                btdActionFireCollision(p,morf,false);
                f32 next=rd<f32>(p,0x5FA8)+0.1f;wr<f32>(p,0x5FA8,next);
                f32 limit=rd<f32>(0x1047B608,0x2C)+1.0f;
                if(next>limit) wr<f32>(p,0x5FA8,0.0f);
            }
        } else {
            s16 timer=rd<s16>(p,0x414);wr<s16>(p,0x6154,12);wr<s16>(p,0x6156,3);
            if(timer!=0) wr<f32>(p,0x5FA8,0.0f);
            else {
                wr<u8>(p,0x6207,2);btdActionBodyLevelSound(p,0x501F);
                btdActionFireCollision(p,morf,false);
                f32 next=rd<f32>(p,0x5FA8)+0.1f;
                // Native ble keeps unordered values; only an ordered greater result resets.
                wr<f32>(p,0x5FA8,next>1.0f?0.0f:next);
            }
        }
        if(btdActionStopped(morf)) btdActionReset(p);
        break;
    }
    }
    f32 threshold=rd<f32>(0x1047B608,0x1C)+400.0f;
    bool reset=rd<f32>(player,0x318)<threshold;
    if(!reset) {
        u32 peg=rd<u32>(0x104629BC);
        if(peg!=0) reset=sideFire?rd<s8>(peg,0x3A1)==3:rd<s8>(peg,0x3A1)!=3;
    }
    if(reset) btdActionReset(p);
    if(sideFire) wr<s16>(p,0x5FBC,0x200);
}
void btdActionTurningPunch(u32 p) {
    u32 morf=gabi::call<u32>(0x020F032C,p);
    switch(u16(rd<s16>(p,0x40E))) {
    case 0:
        btdActionAnimation(p,rd<s32>(0x10192B28+u32(s32(rd<s8>(p,0x411))*4)),5.0f,0);
        btdActionIncrement(p);actorSoundNN(p,0x4835);break;
    case 1:
        if(btdActionStopped(morf)) {
            btdActionNextAnimation(p,0x10192B28,0.0f);btdActionIncrement(p);
            s32 joint=rd<s8>(p,0x412)!=0?10:8;btdActionLevelSound(p,0x5819,p+0x424+u32(joint*12));
        }
        wr<s16>(p,0x5FBA,s16(rd<s16>(0x1047B608,0x84)+15));
        wr<s16>(p,0x5FBC,s16(rd<s16>(0x1047B608,0x86)+0xC00));break;
    case 2:
        wr<u8>(p,0x5FAC,2);
        if(btdActionStopped(morf)) {
            btdActionNextAnimation(p,0x10192B28,0.0f);btdActionIncrement(p);
            s32 joint=rd<s8>(p,0x412)!=0?10:8;btdActionLevelSound(p,0x581B,p+0x424+u32(joint*12));
            gabi::call<void>(0x020F0EB4,p,s32(rd<s8>(p,0x412)));
            u32 game=play();s32 magnitude=rd<s16>(0x1047B608,0x84)+5;
            gabi::Local<cXyz> direction;direction->x=0.0f;direction->y=1.0f;direction->z=0.0f;
            gabi::call<void>(0x025CB374,game+0x599C,magnitude,-33,direction.get());
        }
        break;
    case 3:
        if(btdActionStopped(morf)) {
            btdActionNextAnimation(p,0x10192B28,0.0f);
            f32 timer=gabi::call<f32>(0x020198D8,80.0f)+80.0f;
            s16 next=s16(rd<s16>(p,0x40E)+1);
            wr<s16>(p,0x414,s16(gabi::ftoi(timer)));wr<s16>(p,0x40E,next);
        }
        break;
    case 4: {
        s32 joint=rd<s8>(p,0x412)!=0?10:8;
        if(btdActionStopped(morf)) {
            btdActionNextAnimation(p,0x10192B28,5.0f);btdActionIncrement(p);
            actorSoundNN(p,0x4839);btdActionLevelSound(p,0x581D,p+0x424+u32(joint*12));
            wr<s16>(p,0x615A,23);wr<s16>(p,0x6158,23);wr<s16>(p,0x615C,3);
        }
        break;
    }
    case 5:
        if(btdActionStopped(morf)) {
            u8 forced=rd<u8>(0x104629FC,0xC);wr<s16>(p,0x40E,0);
            if(forced==4) wr<u8>(p,0x411,5);
            else {wr<s16>(p,0x40C,0);wr<s16>(p,0x414,0);}
        }
        break;
    }
}
}

namespace {
u32 btdActionParticle(u32 p,u16 id,u32 angles=0) {
    u32 control=rd<u32>(play(),0x5AB0);
    return gabi::call<u32>(0x025A847C,control,0,u32(id),p+0x314,angles,0,0xFF,0,-1,0,0,0);
}
void btdActionParticleMatrix(u32 morf,u32 emitter,u32 offset) {
    u32 model=rd<u32>(morf,0x90),block=rd<u32>(model,0x2C);
    u16 flags=rd<u16>(block,4);u32 matrices=rd<u32>(block,0x10);
    wr<u16>(block,4,flags|0x10);
    gabi::call<void>(0x028249B0,matrices+offset,emitter+0x1F0,emitter+0x22C);
}
void btdActionBreakShell(u32 p,u32 morf) {
    u8 count=rd<u8>(p,0x63D4);
    u32 model=rd<u32>(morf,0x90);
    u32 animation=resource(0x1000B9E4,rd<s32>(0x10192BB8+4*u32(count)));
    u32 data=rd<u32>(model,0xAC),brk=rd<u32>(p,0x3E0);
    gabi::call<void>(0x025E8154,brk,data,animation,1,2,1.0f,0,-1,1,0);
    wr<u8>(p,0x63D4,u8(rd<u8>(p,0x63D4)+1));
    btdActionBodyLevelSound(p,0x582C);
    for(u32 i=0;i<3;++i) {
        u16 id=rd<u16>(0x10192B00+2*i);u32 emitter=btdActionParticle(p,id);
        if(emitter) btdActionParticleMatrix(morf,emitter,0xA50);
    }
    if(rd<u8>(0x104629FC,2)) wr<u8>(p,0x63D4,3);
    u8 debug=rd<u8>(0x104629FC,3);
    if(debug && rd<u8>(p,0x63D4)>=3) wr<u8>(p,0x63D4,2);
}
void btdActionBurst(u32 p,u32 morf) {
    u32 dragon=rd<u32>(0x104629C0);wr<s16>(dragon,0x5D6,10);
    u32 game=play();gabi::call<void>(0x025E18EC,rd<u8>(game,0x5134)==0x58?0x80000054u:0x80000010u);
    for(u32 i=0;i<35;++i) {
        u32 piece=p+0x63D8+40*i;
        if(rd<u8>(piece,4)==0) {
            s32 joint=2;
            if(i>=5) joint=gabi::ftoi(gabi::call<f32>(0x020198D8,18.999000549316406f));
            u32 position=p+0x424+u32(joint*12);
            wr<u8>(piece,4,1);
            f32 x=rd<f32>(position);wr<f32>(piece,8,x);
            wr<f32>(piece,0xC,rd<f32>(position,4));wr<f32>(piece,0x10,rd<f32>(position,8));
            f32 random=gabi::call<f32>(0x02019918,rd<f32>(0x1047B608,0x10)+50.0f);
            wr<f32>(piece,8,x+random);
            random=gabi::call<f32>(0x02019918,rd<f32>(0x1047B608,0x10)+50.0f);
            wr<f32>(piece,0xC,rd<f32>(piece,0xC)+random);
            random=gabi::call<f32>(0x02019918,rd<f32>(0x1047B608,0x10)+50.0f);
            f32 z=rd<f32>(piece,0x10)+random,xNow=rd<f32>(piece,8);wr<f32>(piece,0x10,z);
            s32 angle=gabi::call<s32>(0x020195B0,xNow,z);u32 m=matrix();
            s16 angleRandom=s16(gabi::ftoi(gabi::call<f32>(0x02019918,2000.0f)));
            gabi::call<void>(0x025F1884,m,s32(s16(angle+angleRandom)));
            gabi::Local<cXyz> velocity;velocity->x=0.0f;
            f32 yRandom=gabi::call<f32>(0x020198D8,5.0f)+20.0f;
            velocity->y=yRandom+rd<f32>(0x1047B608,0xC);
            f32 zRandom=gabi::call<f32>(0x020198D8,5.0f)+20.0f;
            velocity->z=zRandom+rd<f32>(0x1047B608,8);
            gabi::call<void>(0x0200FCD8,velocity.get(),piece+0x14);
            wr<s16>(piece,0x22,s16(gabi::ftoi(gabi::call<f32>(0x020198D8,65536.0f))));
            wr<s16>(piece,0x20,s16(gabi::ftoi(gabi::call<f32>(0x020198D8,65536.0f))));
        }
    }
    for(u32 i=0;i<12;++i) {
        u32 emitter=btdActionParticle(p,rd<u16>(0x10192B70+2*i));
        if(emitter) btdActionParticleMatrix(morf,emitter,rd<u32>(0x10192B88+4*i)*0x30);
    }
}
void btdActionDamage(u32 p) {
    play();u32 morf=gabi::call<u32>(0x020F032C,p);
    s16 state=rd<s16>(p,0x40E);wr<s16>(p,0x418,5);
    bool hideAttention=false;
    if(state>=10) {state=rd<s16>(p,0x40E);wr<s16>(p,0x41C,10);hideAttention=state>11;}
    switch(u16(state)) {
    case 0:
        btdActionAnimation(p,9,5.0f,0);btdActionIncrement(p);
        [[fallthrough]];
    case 1:
        if(btdActionStopped(morf)) {
            btdActionAnimation(p,10,1.0f,2);
            s16 stun=rd<s16>(0x104629FC,8),next=s16(rd<s16>(p,0x40E)+1);
            wr<s16>(p,0x41A,stun);wr<s16>(p,0x40E,next);
        }
        break;
    case 2: if(rd<s16>(p,0x41A)==0) {
        wr<s16>(p,0x40C,0);wr<s16>(p,0x40E,0);
        wr<s16>(p,0x414,s16(gabi::ftoi(gabi::call<f32>(0x020198D8,50.0f)+50.0f)));
    } break;
    case 3:
        if(btdActionStopped(morf)) {btdActionAnimation(p,10,1.0f,2);wr<s16>(p,0x40E,2);}break;
    case 10:
        btdActionAnimation(p,11,5.0f,0);btdActionIncrement(p);actorSoundNN(p,0x4837);
        [[fallthrough]];
    case 11: case 42:
        if(btdActionStopped(morf)) {
            wr<s16>(p,0x40C,0);wr<s16>(p,0x40E,0);
            wr<s16>(p,0x414,s16(gabi::ftoi(gabi::call<f32>(0x020198D8,50.0f)+50.0f)));
        }
        break;
    case 20:
        btdActionAnimation(p,0x2A,3.0f,0);wr<f32>(p,0x70C0,0.0f);btdActionIncrement(p);break;
    case 21: case 22: {
        s32 timer;
        if(state==21) {
            if(!btdActionStopped(morf)) break;
            btdActionAnimation(p,0x2B,1.0f,2);
            s32 duration=rd<s16>(0x1047B608,0x8E)*5+100;
            s16 next=s16(rd<s16>(p,0x40E)+1);
            wr<s16>(p,0x414,s16(duration));wr<s16>(p,0x40E,next);timer=s16(duration);
        } else timer=rd<s16>(p,0x414);
        s32 target=rd<s16>(0x1047B608,0x8A)+40;
        if(timer==target) {btdActionBreakShell(p,morf);timer=rd<s16>(p,0x414);}
        if(timer==0) {
            if(rd<u8>(p,0x63D4)>=3) wr<s16>(p,0x40E,40);
            else {
                btdActionAnimation(p,0x2C,15.0f,0);btdActionIncrement(p);
                wr<f32>(0x104629E4,0,0.0f);wr<f32>(0x104629E4,4,0.0f);wr<f32>(0x104629E4,8,0.0f);
                btdActionLevelSound(p,0x582D,0x104629E4);if(p+0x37C!=0) actorSound(p,0x483E);
                wr<f32>(p,0x70C0,0.0f);wr<s16>(p,0x414,150);
            }
        }
        break;
    }
    case 23:
        {s16 timer=rd<s16>(p,0x414);if(timer==1 || timer==70) wr<f32>(p,0x70C0,0.0f);}
        if(btdActionStopped(morf)) {wr<s16>(p,0x40E,30);wr<s16>(p,0x414,0);}break;
    case 30:
        {s16 timer=rd<s16>(p,0x414);wr<u8>(p,0x70CC,0);
        if(timer==0) {
            btdActionAnimation(p,0x1E,1.0f,0);btdActionIncrement(p);wr<u8>(p,0x41E,0);
            btdActionLevelSound(p,0x5826,0x104629E4);if(p+0x37C!=0) actorSound(p,0x483D);
            wr<f32>(p,0x70C0,0.0f);wr<s16>(p,0x6208,20);wr<s16>(p,0x414,60);
        }}break;
    case 31:
        {s16 timer=rd<s16>(p,0x414);if(timer==55) {gabi::call<void>(0x020F10B4,p);timer=rd<s16>(p,0x414);}
        if(timer==1) wr<f32>(p,0x70C0,0.0f);
        if(btdActionStopped(morf)) {
            btdActionAnimation(p,0x1F,1.0f,0);btdActionIncrement(p);
            btdActionLevelSound(p,0x582E,0x104629E4);wr<f32>(p,0x70C0,0.0f);
        }}break;
    case 32:
        wr<u8>(p,0x70CC,0);
        if(btdActionStopped(morf)) {
            btdActionAnimation(p,0x20,3.0f,0);
            s32 room=rd<s8>(p,0x326);u32 dragon=rd<u32>(0x104629C0);
            s32 reverb=gabi::call<s32>(0x02520540,room);
            gabi::call<void>(0x025E1A40,0x5830,dragon+0x5C4,0,reverb);
            btdActionParticle(p,0x80BC,p+0x320);btdActionIncrement(p);wr<s16>(p,0x414,30);
        }
        break;
    case 33:
        if(rd<s16>(p,0x414)==20) {btdActionLevelSound(p,0x5831,0x104629E4);wr<f32>(p,0x70C0,0.0f);gabi::call<void>(0x020F10B4,p);}
        if(btdActionStopped(morf)) {
            wr<s16>(p,0x40C,0);wr<s16>(p,0x40E,0);
            wr<s16>(p,0x414,s16(gabi::ftoi(gabi::call<f32>(0x020198D8,50.0f)+50.0f)));
        }
        break;
    case 40:
        btdActionAnimation(p,0x34,3.0f,0);btdActionIncrement(p);btdActionBodyLevelSound(p,0x5832);
        gabi::call<void>(0x025E1904,30);wr<f32>(p,0x70C0,0.0f);wr<s16>(p,0x414,150);
        [[fallthrough]];
    case 41: {
        s16 target=s16(rd<s16>(0x1047B608,0x8A)+38),timer=rd<s16>(p,0x414);
        if(timer==target) {btdActionBurst(p,morf);timer=rd<s16>(p,0x414);}
        if(timer==101) {btdActionBodyLevelSound(p,0x5833);if(p!=0 && p+0x37C!=0) actorSound(p,0x483D);}
        if(btdActionStopped(morf)) {
            btdActionBodyLevelSound(p,0x5834);
            if(p!=0 && p+0x37C!=0) actorSound(p,0x4837);
            wr<u8>(p,0x408,1);btdActionAnimation(p,0x35,0.0f,0);btdActionIncrement(p);
        }
        break;
    }
    }
    if(hideAttention) wr<u32>(p,0x39C,0);
}
}

namespace {
void btdActionTrack(u32 p,u32 player) {
    if(rd<s16>(p,0x5FBC)!=0) {
        s16 rate=rd<s16>(p,0x5FBA);
        if(rate==0) {rate=10;wr<s16>(p,0x5FBA,rate);}
        u32 dragon=rd<u32>(0x104629C0);
        if(dragon!=0) {
            s16 action=rd<s16>(dragon,0x5D6);
            if(action==0 || action>=10) {
                s16 mode=rd<s16>(dragon,0x5E6);
                if(mode==0 || mode>=98) {
                    s16 target=rd<s16>(p,0x5FB8);
                    if(target==0) {
                        target=s16(gabi::call<s32>(0x025D6894,p,rd<u32>(play(),0x5B2C)));
                        rate=rd<s16>(p,0x5FBA);
                    }
                    s16 limit=rd<s16>(p,0x5FBC);
                    gabi::call<void>(0x0200F428,p+0x322,s32(target),s32(rate),s32(limit));
                }
            }
        }
        wr<s16>(p,0x5FB8,0);wr<s16>(p,0x5FBA,10);wr<s16>(p,0x5FBC,0);
    }
    s16 turn=0,half=0;
    if(rd<u8>(p,0x5FAE)!=0) {
        f32 z=rd<f32>(player,0x31C),x=rd<f32>(player,0x314);
        if(!(gabi::fmadds(x,x,z*z)<40000.0f)) {
            s32 angle=gabi::call<s32>(0x025D6894,p,rd<u32>(play(),0x5B2C));
            turn=s16(angle-rd<s16>(p,0x322));
            if(turn>5000) turn=5000;else if(turn<-5000) turn=-5000;
            gabi::Local<cXyz> difference;
            gabi::call<void>(0x0201ADE0,player+0x37C,difference.get(),p+0x37C);
            s32 eyeAngle=gabi::call<s32>(0x020195B0,f32(difference->x),f32(difference->z));
            s16 raw=s16(eyeAngle-rd<s16>(p,0x322));half=s16(s32(raw)/2);
            if(half>0x2000) half=0x2000;else if(half<-0x2000) half=-0x2000;
        }
    }
    gabi::call<void>(0x0200F428,p+0x5FB0,s32(turn),4,0x200);
    gabi::call<void>(0x0200F428,p+0x5FB6,s32(half),4,0x200);
    s16 pitch=rd<s16>(p,0x5FB4);
    gabi::call<void>(0x0200F428,p+0x5FB2,s32(pitch),4,0x100);
    wr<s16>(p,0x5FB4,0);wr<u8>(p,0x5FAE,0);
}
}
// HD's matcher name "damage" covers the complete action dispatcher, with its
// wait, six attack variants and damage substates inlined into this one body.
void move(btd_class* self) {
    WWHD_FUNC(0x020F6004,void,self);
    u32 p=gabi::ea(self),player=rd<u32>(play(),0x5B2C);
    wr<u8>(p,0x5FAC,0);wr<u32>(p,0x39C,4);
    switch(u16(rd<s16>(p,0x40C))) {
    case 0: btdActionWait(p);break;
    case 1:
        {s32 attack=rd<s8>(p,0x410);wr<s16>(p,0x418,5);
        switch(attack) {
        case 0:btdActionJab(p);break;
        case 1:btdActionPunch(p);break;
        case 2:btdActionFire(p);break;
        case 3:btdActionHighFire(p,false);break;
        case 4:btdActionHighFire(p,true);break;
        case 5:btdActionTurningPunch(p);break;
        }}break;
    case 2:btdActionDamage(p);break;
    case 10:gabi::call<void>(0x020F4924,p);wr<u32>(p,0x39C,0);break;
    case 11:gabi::call<void>(0x020F52B8,p);wr<u32>(p,0x39C,0);break;
    }
    gabi::call<void>(0x020F4D80,p);
    btdActionTrack(p,player);
}
VERIFY(0x020F6004,move);

// Native authority: 020F52B8..020F6000, 851 instructions / 3404 bytes.
#include "d/actor/d_a_btd.h"
#include "gabi.h"
#include <cmath>

namespace {
template<class T> T btdEndRead(u32 p,u32 off=0) {return gabi::load<T>(p+off);}
template<class T> void btdEndWrite(u32 p,u32 off,T v) {gabi::store<T>(p+off,v);}
u32 btdEndPlay() {return gabi::call<u32>(0x025200D4);}
u32 btdEndMatrix() {return btdEndRead<u32>(0x1018C7B0);}
void btdEndCopy3(u32 dst,u32 src) {
    u32 x=btdEndRead<u32>(src),y=btdEndRead<u32>(src,4),z=btdEndRead<u32>(src,8);
    btdEndWrite<u32>(dst,0,x);btdEndWrite<u32>(dst,4,y);btdEndWrite<u32>(dst,8,z);
}
u32 btdEndResource(u32 name,s32 index) {
    struct String {be<u32> name,table;}; gabi::Local<String> s;
    s->name=name;s->table=0x1000B9F4;
    return gabi::call<u32>(0x026066C4,btdEndRead<u32>(0x101F4F28),s.get(),index);
}
void btdEndSound(u32 actor,u32 position,u32 sound) {
    s32 room=btdEndRead<s8>(actor,0x326);
    s32 reverb=gabi::call<s32>(0x02520540,room);
    gabi::call<void>(0x025E1A40,sound,position,0,reverb);
}
void btdEndMonsterSound(u32 actor,u32 sound,bool allowNullActor=false) {
    if((!actor && !allowNullActor) || !(actor+0x37C)) return;
    s32 room=btdEndRead<s8>(actor,0x326);
    u32 id=actor ? btdEndRead<u32>(actor,4) : 0xFFFFFFFFu;
    s32 reverb=gabi::call<s32>(0x02520540,room);
    gabi::call<void>(0x025E1AA4,sound,actor+0x37C,id,0,reverb);
}
void btdEndShock(u32 debug,gabi::Local<cXyz>& axis) {
    u32 game=btdEndPlay();s32 power=btdEndRead<s16>(debug,0x84);
    axis->z=0.0f;axis->y=1.0f;axis->x=0.0f;
    gabi::call<void>(0x025CB374,game+0x599C,power+5,-33,axis.get());
}
void btdEndFollowEmitter(u32 actor,u32 emitter) {
    u32 model=btdEndRead<u32>(btdEndRead<u32>(actor,0x3FC),0x90);
    u32 block=btdEndRead<u32>(model,0x2C);
    u16 flags=btdEndRead<u16>(block,4);u32 matrices=btdEndRead<u32>(block,0x10);
    btdEndWrite<u16>(block,4,flags|0x10);
    gabi::call<void>(0x028249B0,matrices,emitter+0x1F0,emitter+0x22C);
}
}

void end(btd_class* self) {
    WWHD_FUNC(0x020F52B8,void,self);
    const u32 p=gabi::ea(self),debug=0x1047B608,center=0x104629E4;
    btdEndPlay();
    const u32 morf=gabi::call<u32>(0x020F032C,p);
    const s16 state=btdEndRead<s16>(p,0x40E);
    btdEndWrite<s16>(p,0x41C,10);
    switch(state) {
    case 0x32:
        if(btdEndRead<u8>(btdEndPlay(),0x5134)==0x58) {
            s32 room=btdEndRead<s8>(p,0x326);
            gabi::call<void>(0x02587EFC,0,room);
            btdEndSound(p,0,0x2888);
            u32 save=btdEndRead<u32>(0x101F84DC);
            gabi::call<void>(0x025B8B68,save+0x644,0x3240);
            save=btdEndRead<u32>(0x101F84DC);
            gabi::call<void>(0x025B8B68,save+0x1178,0x480);
            gabi::call<void>(0x025E1904,30);
        } else {
            btdEndWrite<s16>(p,0x40E,0x33);
            gabi::call<void>(0x020F03BC,p,12,0.0f,0,1.0f,-1);
            btdEndWrite<s16>(p,0x705C,0);btdEndWrite<s16>(p,0x705E,0);
            btdEndWrite<s16>(p,0x705A,100);
            s32 delay=btdEndRead<s16>(debug,0x86);
            btdEndWrite<s16>(p,0x414,(s16)(delay+540));
            gabi::call<void>(0x025E1904,20);
            gabi::call<void>(0x025E1934,0xC0000003u);
            gabi::call<void>(0x020F02D4,p);
        }
        return;
    case 0x33: {
        s16 frame=btdEndRead<s16>(p,0x705E);
        if(frame==3) {btdEndMonsterSound(p,0x4837);frame=btdEndRead<s16>(p,0x705E);}
        if(frame==64) {btdEndMonsterSound(p,0x483B);gabi::call<void>(0x020F02D4,p);frame=btdEndRead<s16>(p,0x705E);}
        if(frame==120) {if(p && p+0x37C) btdEndSound(p,p+0x37C,0x5837);gabi::call<void>(0x020F02D4,p);frame=btdEndRead<s16>(p,0x705E);}
        if(frame==160) {btdEndMonsterSound(p,0x4839);frame=btdEndRead<s16>(p,0x705E);}
        if(frame==200) {
            btdEndWrite<f32>(center,0,0.0f);btdEndWrite<f32>(center,4,300.0f);btdEndWrite<f32>(center,8,0.0f);
            btdEndSound(p,center,0x5838);btdEndMonsterSound(p,0x483C,true);
            frame=btdEndRead<s16>(p,0x705E);
        }
        if(frame==310) btdEndSound(p,center,0x5839);
        s16 timer=btdEndRead<s16>(p,0x414);
        if(timer==101) {
            gabi::call<void>(0x025E1944);
            u32 z=btdEndRead<u32>(p,0x384),y=btdEndRead<u32>(p,0x380);
            btdEndWrite<u32>(p,0x709C,z);
            u16 ax=btdEndRead<u16>(p,0x320);btdEndWrite<u32>(p,0x7098,y);btdEndWrite<u16>(p,0x70B8,ax);
            u16 ay=btdEndRead<u16>(p,0x322);btdEndWrite<u8>(p,0x7090,1);
            s16 next=btdEndRead<s16>(p,0x40E),count=btdEndRead<s16>(p,0x705A);
            btdEndWrite<f32>(p,0x7084,0.0f);btdEndWrite<u16>(p,0x70BA,ay);
            u32 x=btdEndRead<u32>(p,0x37C);btdEndWrite<s16>(p,0x40E,(s16)(next+1));btdEndWrite<u32>(p,0x7094,x);
            u16 az=btdEndRead<u16>(p,0x324);btdEndWrite<f32>(p,0x318,-10000.0f);btdEndWrite<u16>(p,0x70BC,az);
            btdEndWrite<s16>(p,0x414,70);btdEndWrite<s16>(p,0x705E,0);btdEndWrite<s16>(p,0x705A,(s16)(count+1));
            btdEndWrite<f32>(p,0x370,30.0f);btdEndSound(p,center,0x583A);
            u32 control=btdEndRead<u32>(btdEndPlay(),0x5AB0);
            u32 emitter=gabi::call<u32>(0x025A847C,control,0,0x80DE,p+0x7094,0,0,255,0,-1,0,0,0);
            btdEndWrite<u32>(p,0x6160,emitter);btdEndWrite<f32>(p,0x708C,30.0f);
            gabi::Local<cXyz> axis;btdEndShock(debug,axis);btdEndWrite<u8>(p,0x7059,180);
            return;
        }
        if(timer==460) {timer=btdEndRead<s16>(p,0x414);btdEndWrite<u8>(p,0x408,2);}
        if(timer==241) {
            u32 model=btdEndRead<u32>(morf,0x90);
            u32 animation=btdEndResource(0x1000B9E8,0x4A);
            u32 data=btdEndRead<u32>(model,0xAC),brk=btdEndRead<u32>(p,0x3F8);
            gabi::call<void>(0x025E8154,brk,data,animation,1,1.0f,0,0,-1,1,0);
            animation=btdEndResource(0x1000B9E8,0x54);
            u32 btk=btdEndRead<u32>(p,0x3F4);data=btdEndRead<u32>(model,0xAC);
            gabi::call<void>(0x025E7CE0,btk,data,animation,1,1.0f,0,0,-1,1,0);
            timer=btdEndRead<s16>(p,0x414);
        }
        if(timer==191) for(u32 i=0;i<19;++i) {
            u16 effect=btdEndRead<u16>(0x10192BD8,2*i);
            u32 control=btdEndRead<u32>(btdEndPlay(),0x5AB0);
            u32 emitter=gabi::call<u32>(0x025A847C,control,0,effect,p+0x314,0,0,255,0,-1,0,0,0);
            if(emitter) {
                u32 model=btdEndRead<u32>(morf,0x90),joint=btdEndRead<u32>(0x10192C78,4*i);
                u32 block=btdEndRead<u32>(model,0x2C),matrices=btdEndRead<u32>(block,0x10);
                u16 flags=btdEndRead<u16>(block,4);btdEndWrite<u16>(block,4,flags|0x10);
                gabi::call<void>(0x028249B0,matrices+joint*48,emitter+0x1F0,emitter+0x22C);
            }
        }
        return;
    }
    case 0x34: {
        gabi::Local<cXyz> difference,vector;
        btdEndWrite<u8>(p,0x70CC,0);
        gabi::call<void>(0x0201ADE0,p+0x70A0,difference.get(),p+0x7094);
        f32 z=difference->z,x=difference->x,y=difference->y;
        vector->x=x;vector->y=y;vector->z=z;
        f32 distance=gabi::call<f32>(0x028F4384,gabi::fmadds(x,x,gabi::fmuls_ppc(z,z)));
        f32 factor=gabi::fadds_ppc(btdEndRead<f32>(debug,0x40),3.5f);
        vector->y=gabi::fmadds(distance,factor,(f32)vector->y);
        s16 yaw=gabi::call<s16>(0x020195B0,(f32)vector->x,(f32)vector->z);
        z=vector->z;x=vector->x;btdEndWrite<s16>(p,0x70BA,yaw);
        distance=gabi::call<f32>(0x028F4384,gabi::fmadds(x,x,gabi::fmuls_ppc(z,z)));
        s32 pitch=gabi::call<s32>(0x020195B0,(f32)vector->y,distance);
        s16 negativePitch=(s16)(0u-(u32)pitch);
        yaw=btdEndRead<s16>(p,0x70BA);gabi::call<void>(0x025F1884,btdEndMatrix(),yaw);
        gabi::call<void>(0x025F1BF4,btdEndMatrix(),negativePitch);
        vector->x=0.0f;vector->y=0.0f;vector->z=btdEndRead<f32>(p,0x370);
        gabi::call<void>(0x0200FCD8,vector.get(),p+0x70AC);
        gabi::call<void>(0x028E8D88,p+0x7094,p+0x70AC,p+0x7094);
        // Native bge is !LT; only strict LT takes the update.
        if(btdEndRead<f32>(p,0x70B0)<0.0f) gabi::call<void>(0x0200ED84,p+0x370,300.0f,1.0f,2.0f);
        btdEndCopy3(p+0x37C,p+0x7094);
        gabi::call<void>(0x0201ADE0,p+0x70A0,difference.get(),p+0x7094);
        btdEndCopy3(gabi::ea(vector.get()),gabi::ea(difference.get()));
        f32 square=gabi::call<f32>(0x028E8DD0,vector.get());distance=gabi::call<f32>(0x028F4384,square);
        f32 speed=btdEndRead<f32>(p,0x370);
        if(distance<gabi::fadds_ppc(speed,speed)) btdEndWrite<s16>(p,0x40E,0x35);
        break;
    }
    case 0x35: {
        f32 vx=btdEndRead<f32>(p,0x70AC),targetX=btdEndRead<f32>(p,0x70A0);
        btdEndWrite<u8>(p,0x70CC,0);gabi::call<void>(0x0200ED84,p+0x7094,targetX,1.0f,std::fabs(vx));
        f32 vy=btdEndRead<f32>(p,0x70B0),y=btdEndRead<f32>(p,0x7098);
        y=gabi::fadds_ppc(y,vy);btdEndWrite<f32>(p,0x7098,y);
        f32 gravity=gabi::fadds_ppc(btdEndRead<f32>(debug,0x14),5.0f),targetY=btdEndRead<f32>(p,0x70A4);
        btdEndWrite<f32>(p,0x70B0,gabi::fsubs_ppc(vy,gravity));
        if(!(y>targetY)) {
            vy=btdEndRead<f32>(p,0x70B0);btdEndWrite<f32>(p,0x7098,targetY);
            if(vy<-50.0f) {
                f32 bounce=gabi::fadds_ppc(btdEndRead<f32>(debug,0x18),-0.3f);
                btdEndWrite<f32>(p,0x708C,40.0f);btdEndWrite<f32>(p,0x70B0,gabi::fmuls_ppc(vy,bounce));
                gabi::Local<cXyz> axis;btdEndShock(debug,axis);
                if(p && p+0x37C) btdEndSound(p,p+0x37C,0x583B);
                btdEndWrite<s16>(p,0x40E,0x36);btdEndWrite<s16>(p,0x414,50);
                constexpr u32 staticPosition=0x104629F0,initialized=0x104629CC;
                if(!btdEndRead<u32>(initialized)) {
                    btdEndWrite<u32>(initialized,0,1);
                    btdEndWrite<f32>(staticPosition,0,btdEndRead<f32>(p,0x7094));
                    btdEndWrite<f32>(staticPosition,4,btdEndRead<f32>(p,0x7098));
                    btdEndWrite<f32>(staticPosition,8,btdEndRead<f32>(p,0x709C));
                }
                btdEndWrite<f32>(staticPosition,4,gabi::fsubs_ppc(btdEndRead<f32>(staticPosition,4),100.0f));
                gabi::call<void>(0x020F0DFC,p,staticPosition,p+0x70B8);
                u32 emitter=btdEndRead<u32>(p,0x6160);
                if(emitter) {
                    u32 flags=btdEndRead<u32>(emitter,0x254);btdEndWrite<s32>(emitter,0x5C,-1);
                    btdEndWrite<u32>(emitter,0x254,flags|1);btdEndWrite<u32>(p,0x6160,0);
                }
            }
        }
        f32 vz=btdEndRead<f32>(p,0x70B4),targetZ=btdEndRead<f32>(p,0x70A8);
        gabi::call<void>(0x0200ED84,p+0x709C,targetZ,1.0f,std::fabs(vz));
        btdEndCopy3(p+0x37C,p+0x7094);
        break;
    }
    case 0x36: {
        btdEndWrite<u8>(p,0x70CC,0);
        gabi::call<void>(0x0200F428,p+0x70B8,0x2000,2,0x200);
        gabi::call<void>(0x0200F428,p+0x70BC,0x2000,2,0x200);
        f32 vy=btdEndRead<f32>(p,0x70B0),y=btdEndRead<f32>(p,0x7098);
        y=gabi::fadds_ppc(y,vy);btdEndWrite<f32>(p,0x7098,y);
        f32 gravity=gabi::fadds_ppc(btdEndRead<f32>(debug,0x14),5.0f),targetY=btdEndRead<f32>(p,0x70A4);
        f32 x=btdEndRead<f32>(p,0x7094);btdEndWrite<f32>(p,0x70B0,gabi::fsubs_ppc(vy,gravity));
        if(!(y>targetY)) {y=targetY;btdEndWrite<f32>(p,0x7098,y);btdEndWrite<f32>(p,0x70B0,0.0f);}
        s16 timer=btdEndRead<s16>(p,0x414);f32 z=btdEndRead<f32>(p,0x709C);
        btdEndWrite<f32>(p,0x37C,x);btdEndWrite<f32>(p,0x380,y);btdEndWrite<f32>(p,0x384,z);
        if(timer==1) {
            btdEndWrite<f32>(p,0x7098,gabi::fsubs_ppc(btdEndRead<f32>(p,0x7098),100.0f));
            if(p+0x37C) btdEndSound(p,p+0x37C,0x583C);
            gabi::call<void>(0x025D99E8,p,p+0x7094,15,2,255);
            btdEndWrite<f32>(p,0x7098,-10000.0f);btdEndWrite<s16>(p,0x414,150);btdEndWrite<s16>(p,0x40E,0x37);
        }
        gabi::call<void>(0x020F1288,p);return;
    }
    case 0x37: {
        s16 timer=btdEndRead<s16>(p,0x414);btdEndWrite<u8>(p,0x70CC,0);
        if(timer==2) {
            u32 dragon=btdEndRead<u32>(0x104629C0);btdEndWrite<u8>(dragon,0x558,1);
            timer=btdEndRead<s16>(p,0x414);btdEndWrite<f32>(p,0x70C8,-200.0f);
        }
        if(timer==1) {
            s16 count=btdEndRead<s16>(p,0x705A);btdEndWrite<s16>(p,0x40E,0x38);btdEndWrite<s16>(p,0x705A,(s16)(count+1));
        }
        return;
    }
    case 0x39:btdEndWrite<u8>(p,0x6207,6);return;
    default:return;
    }
    // Shared native tail for states34/35: tumble, calculate head, follow emitter.
    s32 ax=btdEndRead<s16>(debug,0x86),angle=btdEndRead<s16>(p,0x70B8);
    btdEndWrite<s16>(p,0x70B8,(s16)(angle+ax+1000));
    s32 az=btdEndRead<s16>(debug,0x88);angle=btdEndRead<s16>(p,0x70BC);
    btdEndWrite<s16>(p,0x70BC,(s16)(angle+az+1500));
    gabi::call<void>(0x020F1288,p);
    u32 emitter=btdEndRead<u32>(p,0x6160);if(emitter) btdEndFollowEmitter(p,emitter);
}
VERIFY(0x020F52B8,end);

// daBtd_Execute: 020F1494..020F349C, 8204 bytes (demo_camera inlined).
#include "d/actor/d_a_btd.h"
#include "gabi.h"
namespace {
template<class T> T btdExecR(u32 p,u32 off=0) {return gabi::load<T>(p+off);}
template<class T> void btdExecW(u32 p,u32 off,T v) {gabi::store<T>(p+off,v);}
u32 btdExecPlay() {return gabi::call<u32>(0x025200D4);}
u32 btdExecMatrix() {return btdExecR<u32>(0x1018C7B0);}
void btdExecCopy(u32 dst,u32 src) {u32 x=btdExecR<u32>(src),y=btdExecR<u32>(src,4),z=btdExecR<u32>(src,8);btdExecW<u32>(dst,0,x);btdExecW<u32>(dst,4,y);btdExecW<u32>(dst,8,z);}
void btdExecModelMatrix(u32 model,u32 source) {
    f32 v[12];for(u32 i=0;i<12;++i)v[i]=btdExecR<f32>(source,4*i);
    constexpr u8 order[]={0,1,2,3,4,8,5,6,7,9,10,11};
    for(u8 i:order)btdExecW<f32>(model,0xC8+4*i,v[i]);
}
void btdExecSetCollision(u32 shape) {u32 game=btdExecPlay();gabi::call<void>(0x0200E240,game+0x26A4,shape);}
void btdExecCalc(u32 address,f32 target,f32 rate,f32 step) {gabi::call<void>(0x0200ED84,address,target,rate,step);}
void btdExecSound(u32 actor,u32 position,u32 sound) {s32 room=btdExecR<s8>(actor,0x326);s32 reverb=gabi::call<s32>(0x02520540,room);gabi::call<void>(0x025E1A40,sound,position,0,reverb);}
void btdExecPlayerPosition(u32 player,u32 position,u32 actor,s32 angleOffset) {
    u32 table=btdExecR<u32>(player,0xB4),fn=btdExecR<u32>(table,0x114);
    s16 angle=(s16)(btdExecR<s16>(actor,0x322)+angleOffset);
    gabi::call_ptr<void>(fn,player,position,angle);
}
void btdExecQuake(u32 debug,s32 extra,gabi::Local<cXyz>& axis) {
    u32 game=btdExecPlay();s32 power=btdExecR<s16>(debug,0x8A);
    axis->x=0.0f;axis->y=1.0f;axis->z=0.0f;
    gabi::call<void>(0x025CB408,game+0x599C,power+extra,-33,axis.get());
}
void btdExecCameraStep(u32 actor,u32 offset,f32 target,f32 multiplier,f32 rate=0.1f) {
    f32 blend=btdExecR<f32>(actor,0x7084);
    btdExecCalc(actor+offset,target,rate,gabi::fmuls_ppc(multiplier,blend));
}
void btdExecDebrisMove(u32 actor) {
    for(u32 i=0;i<35;++i) {
        u32 piece=actor+0x63D8+40*i;if(!btdExecR<u8>(piece,4))continue;
        gabi::call<void>(0x028E8D88,piece+8,piece+0x14,piece+8);
        f32 x=btdExecR<f32>(piece,8);s16 ax=btdExecR<s16>(piece,0x20),ay=btdExecR<s16>(piece,0x22);
        f32 y=btdExecR<f32>(piece,12),vy=btdExecR<f32>(piece,0x18),z=btdExecR<f32>(piece,16);
        btdExecW<s16>(piece,0x20,(s16)(ax+0x900));btdExecW<s16>(piece,0x22,(s16)(ay+0x600));
        btdExecW<f32>(piece,0x18,gabi::fsubs_ppc(vy,3.0f));gabi::call<void>(0x0200FAD8,0,x,y,z);
        ay=btdExecR<s16>(piece,0x22);gabi::call<void>(0x025F1C28,btdExecMatrix(),ay);
        ax=btdExecR<s16>(piece,0x20);gabi::call<void>(0x025F1BF4,btdExecMatrix(),ax);
        u32 matrix=btdExecMatrix(),model=btdExecR<u32>(piece);btdExecModelMatrix(model,matrix);
        if(btdExecR<f32>(piece,12)<-100.0f)btdExecW<u8>(piece,4,0);
    }
}
void btdExecSimple(u32 position,u32 effect) {
    u32 control=btdExecR<u32>(btdExecPlay(),0x5AB0);
    gabi::call<void>(0x025A8D40,control,effect,position,255,0x101D5E98u,0x101D5E98u,0);
}
void btdExecSplashMove(u32 actor) {
    const u32 hio=0x104629FC;u32 player=btdExecR<u32>(btdExecPlay(),0x5B2C);
    gabi::Local<cXyz> difference;
    for(u32 i=0;i<50;++i) {
        u32 splash=actor+0x6950+36*i,sphere=actor+0x1DEC+0x12C*i;s8 status=btdExecR<s8>(splash);
        if(status==0)continue;
        if(status>2) {s8 next=(s8)(status+1);btdExecW<s8>(splash,0,next==30?0:next);continue;}
        bool track=i==0 && btdExecR<u8>(hio,0x50)!=0;
        bool updateCollision;
        if(track) {
            btdExecCalc(splash+4,btdExecR<f32>(player,0x314),1.0f,30.0f);
            btdExecCalc(splash+8,gabi::fadds_ppc(btdExecR<f32>(player,0x318),70.0f),1.0f,30.0f);
            btdExecCalc(splash+12,btdExecR<f32>(player,0x31C),1.0f,30.0f);
            status=btdExecR<s8>(splash);btdExecW<f32>(splash,0x14,-10.0f);updateCollision=true;
        } else {
            f32 gravity=btdExecR<f32>(hio,0x4C),vy=btdExecR<f32>(splash,0x14);
            btdExecW<f32>(splash,0x14,gabi::fsubs_ppc(vy,gravity));
            gabi::call<void>(0x028E8D88,splash+4,splash+0x10,splash+4);
            updateCollision=btdExecR<f32>(splash,0x14)<0.0f;
            if(updateCollision)status=btdExecR<s8>(splash);
        }
        bool grounded=false,hit=false;
        if(updateCollision) {
            if(status==1) {
                gabi::call<void>(0x025167C0,sphere,splash+4);
                btdExecW<u8>(splash,0,(u8)(btdExecR<u8>(splash)+1));btdExecSetCollision(sphere);
                grounded=btdExecR<f32>(splash,8)<0.0f;
            } else {
                gabi::call<void>(0x025167E4,sphere,splash+4);btdExecSetCollision(sphere);
                grounded=btdExecR<f32>(splash,8)<0.0f;
            }
        }
        if(!grounded)hit=gabi::call<s32>(0x025160DC,sphere)!=0;
        if(!grounded && !hit) {btdExecSimple(splash+4,0x8063);continue;}
        if(grounded)btdExecW<f32>(splash,8,0.0f);
        gabi::call<void>(0x0201ADE0,player+0x314,difference.get(),splash+4);btdExecW<u8>(splash,0,0);
        f32 square=gabi::call<f32>(0x028E8DD0,difference.get()),distance=gabi::call<f32>(0x028F4384,square);
        // Native blt and fallthrough of bge both require strict LT.
        bool close=distance<1500.0f;
        if(close) {
            s8 count=btdExecR<s8>(actor,0x7058);
            if(count<20) {
                btdExecW<u8>(actor,0x7058,(u8)(count+1));btdExecSimple(splash+4,0x8064);
                btdExecSound(actor,splash+4,0x6942);btdExecW<u8>(splash,0,0);
            }
        }
    }
}
void btdExecCamera(u32 actor);
}

BOOL daBtd_Execute(btd_class* self) {
    WWHD_FUNC(0x020F1494,BOOL,self);
    const u32 p=gabi::ea(self),debug=0x1047B608,hio=0x104629FC;
    u32 player=btdExecR<u32>(btdExecPlay(),0x5B2C);
    u8 stage=btdExecR<u8>(btdExecPlay(),0x5134);s8 force=btdExecR<s8>(p,0x70D4);
    if(stage==0x58)btdExecW<u8>(p,0x7059,50);
    if(force) {u32 camera=gabi::call<u32>(0x024F8044);gabi::call<void>(0x02514EE4,camera,0x1000BC3Cu,0);}
    u32 floor=btdExecR<u32>(p,0x70D0);f32 floorY=btdExecR<f32>(p,0x70C4);btdExecW<u8>(p,0x70CC,1);btdExecW<f32>(floor,0xB3C,floorY);
    f32 rise=gabi::fadds_ppc(btdExecR<f32>(debug,0x44),30.0f),height=btdExecR<f32>(p,0x70C0);
    height=gabi::fadds_ppc(height,rise);btdExecW<f32>(p,0x70C0,height);
    f32 ceiling=gabi::fadds_ppc(btdExecR<f32>(debug,0x50),2300.0f);
    if(height>ceiling)btdExecW<f32>(p,0x70C0,ceiling);
    u32 btk=gabi::call<u32>(0x020F035C,p),brk=gabi::call<u32>(0x020F038C,p);
    if(!btdExecR<u32>(0x104629BC))btdExecW<u32>(0x104629BC,0,gabi::call<u32>(0x025DE508,0x020F1378u,p));
    if(!btdExecR<u32>(0x104629C0))btdExecW<u32>(0x104629C0,0,gabi::call<u32>(0x025DE508,0x020F13D4u,p));
    s16 count=btdExecR<s16>(p,0x40A);btdExecW<u8>(p,0x38A,4);btdExecW<s16>(p,0x40A,(s16)(count+1));
    for(u32 i=0;i<4;++i) {s16 timer=btdExecR<s16>(p,0x414+2*i);if(timer)btdExecW<s16>(p,0x414+2*i,(s16)(timer-1));}
    s16 actionTimer=btdExecR<s16>(p,0x41C);if(actionTimer)btdExecW<s16>(p,0x41C,(s16)(actionTimer-1));
    gabi::call<void>(0x020F6004,p);
    f32 scale=btdExecR<f32>(hio,4);btdExecW<f32>(p,0x338,scale);btdExecW<f32>(p,0x334,scale);btdExecW<f32>(p,0x330,scale);
    u32 morf=gabi::call<u32>(0x020F032C,p),model=btdExecR<u32>(morf,0x90);
    f32 sz=btdExecR<f32>(p,0x338),sy=btdExecR<f32>(p,0x334);
    btdExecW<f32>(model,0xBC,scale);btdExecW<f32>(model,0xC0,sy);btdExecW<f32>(model,0xC4,sz);
    f32 z=btdExecR<f32>(p,0x31C),x=btdExecR<f32>(p,0x314),y=btdExecR<f32>(p,0x318);
    constexpr u32 stackMatrix=0x1048D0CC;gabi::call<void>(0x028E93CC,stackMatrix,x,y,z);
    s16 angle=btdExecR<s16>(p,0x322);gabi::call<void>(0x025F1C28,stackMatrix,angle);
    angle=btdExecR<s16>(p,0x320);gabi::call<void>(0x025F1BF4,stackMatrix,angle);
    angle=btdExecR<s16>(p,0x324);gabi::call<void>(0x025F1C5C,stackMatrix,angle);btdExecModelMatrix(model,stackMatrix);
    if(!btdExecR<u8>(hio,1))gabi::call<void>(0x025E535C,morf,p+0x37C,0,0);
    gabi::call<void>(0x025E55A0,morf);
    u8 phase=btdExecR<u8>(p,0x408);f32 radiusScale=phase?0.6f:1.0f;
    gabi::Local<cXyz> hidden,cylinderLocal,difference,vector;
    for(u32 i=0;i<19;++i) {
        u32 sphere=p+0x550+0x12C*i;
        if(phase && i!=8 && i!=10)btdExecW<u32>(sphere,0x94,btdExecR<u32>(sphere,0x94)&~1u);
        f32 radius=btdExecR<f32>(0x10192CC4,4*i);gabi::call<void>(0x02018C8C,sphere+0x118,gabi::fmuls_ppc(radius,radiusScale));
        if(btdExecR<u8>(p,0x7090)) {hidden->x=0.0f;hidden->y=-20000.0f;hidden->z=0.0f;gabi::call<void>(0x02018D40,sphere+0x118,hidden.get());}
        else gabi::call<void>(0x02018D40,sphere+0x118,p+0x424+12*i);
        if(i==8 || i==10) {
            u8 attack=btdExecR<u8>(p,0x5FAC);
            if(attack==1 || attack==2) {btdExecW<u32>(sphere,0,btdExecR<u32>(sphere)|1);attack=btdExecR<u8>(p,0x5FAC);btdExecW<u8>(sphere,0x14,attack==2?2:1);}
            else btdExecW<u32>(sphere,0,btdExecR<u32>(sphere)&~1u);
            u32 co=btdExecR<u32>(sphere,0x2C),target=btdExecR<u32>(sphere,0x18);
            btdExecW<u32>(sphere,0x2C,co&~1u);btdExecW<u32>(sphere,0x18,target&~1u);
        } else {
            bool attack=i<2 && btdExecR<u8>(p,0x5FAC)==3;
            u32 bits=btdExecR<u32>(sphere);btdExecW<u32>(sphere,0,attack?bits|1:bits&~1u);
            if(i<2 && !btdExecR<u8>(p,0x408)) {u32 co=btdExecR<u32>(sphere,0x2C),target=btdExecR<u32>(sphere,0x18);btdExecW<u32>(sphere,0x2C,co&~1u);btdExecW<u32>(sphere,0x18,target&~1u);}
        }
        btdExecSetCollision(sphere);if(i!=18)phase=btdExecR<u8>(p,0x408);
    }
    u8 hand=btdExecR<u8>(p,0x5FA4);s16 adjustment=0;f32 reach=1.0f;
    if(hand==2 || hand==3) {s32 delta=btdExecR<s16>(debug,0x8A)+4200;adjustment=(s16)(hand==2?-delta:delta);reach=gabi::fadds_ppc(btdExecR<f32>(debug,0x30),0.95f);}
    hidden->x=0.0f;hidden->y=-10000.0f;hidden->z=0.0f;
    for(u32 i=0;i<6;++i)gabi::call<void>(0x020182E0,p+0x599C+0x130*i,hidden.get());
    angle=(s16)(btdExecR<s16>(p,0x322)+adjustment);gabi::call<void>(0x025F1884,btdExecMatrix(),angle);
    for(u32 i=0;i<6;++i) {
        u32 cylinder=p+0x5884+0x130*i;hand=btdExecR<u8>(p,0x5FA4);
        if(hand==1 || (hand==2 && i<=2) || (hand==3 && i>=3)) {
            f32 lx=btdExecR<f32>(0x10192D10,4*i),ly=btdExecR<f32>(0x10192D28,4*i),lz=btdExecR<f32>(0x10192D40,4*i);
            cylinderLocal->x=lx;cylinderLocal->y=ly;cylinderLocal->z=gabi::fmuls_ppc(lz,reach);
            u32 position=p+0x60A4+12*i;gabi::call<void>(0x0200FCD8,cylinderLocal.get(),position);
            gabi::call<void>(0x028E8D88,position,p+0x314,position);gabi::call<void>(0x020182E0,cylinder+0x118,position);
            gabi::call<void>(0x02018428,cylinder+0x118,btdExecR<f32>(0x10192D58,4*i));
            gabi::call<void>(0x020184DC,cylinder+0x118,btdExecR<f32>(0x10192D70,4*i));
        }
        btdExecSetCollision(cylinder);
    }
    btdExecW<u8>(p,0x5FA4,0);u32 item=gabi::call<u32>(0x025DE508,0x020F1424u,p);
    if(item && btdExecR<f32>(item,0x370)>10.0f) {
        gabi::call<void>(0x0201ADE0,item+0x314,difference.get(),p+0x37C);btdExecCopy(gabi::ea(vector.get()),gabi::ea(difference.get()));
        f32 square=gabi::call<f32>(0x028E8DD0,vector.get()),distance=gabi::call<f32>(0x028F4384,square);
        if(distance<400.0f)btdExecW<s16>(p,0x420,20);
    }
    gabi::call<void>(0x0201ADE0,player+0x37C,difference.get(),p+0x37C);btdExecCopy(gabi::ea(vector.get()),gabi::ea(difference.get()));
    if(btdExecR<u8>(player,0x3AC)) {
        f32 square=gabi::call<f32>(0x028E8DD0,vector.get()),distance=gabi::call<f32>(0x028F4384,square);
        if(distance<400.0f)btdExecW<s16>(p,0x420,20);
    }
    s16 eyeTimer=btdExecR<s16>(p,0x420);
    if(eyeTimer && !btdExecR<u8>(p,0x408)) {btdExecW<s16>(p,0x420,(s16)(eyeTimer-1));gabi::call<void>(0x0200F428,p+0x422,13000,1,0x1000);}
    else gabi::call<void>(0x0200F428,p+0x422,0,4,0x400);
    if(!btdExecR<s16>(p,0x418)) {f32 random=gabi::call<f32>(0x020198D8,150.0f);btdExecW<s16>(p,0x418,(s16)gabi::ftoi(gabi::fadds_ppc(random,100.0f)));btdExecW<s16>(p,0x420,20);}
    if(btdExecR<u8>(p,0x408))btdExecW<u32>(p,0x1C28,btdExecR<u32>(p,0x1C28)&~1u);
    f32 radius=gabi::fadds_ppc(btdExecR<f32>(debug,0x1C),80.0f);gabi::call<void>(0x02018C8C,p+0x1CAC,radius);
    gabi::call<void>(0x02018D40,p+0x1CAC,p+0x508);btdExecSetCollision(p+0x1B94);
    gabi::call<void>(0x025E742C,btk);gabi::call<void>(0x025E742C,brk);
    u32 light=gabi::call<u32>(0x02555D0C);gabi::call<void>(0x025626A4,light,1,p+0x37C,p+0x620C);
    btdExecDebrisMove(p);btdExecSplashMove(p);btdExecCamera(p);
    gabi::call<void>(0x020F90C4,p);gabi::call<void>(0x020F94DC,p);return TRUE;
}
VERIFY(0x020F1494,daBtd_Execute);

namespace {
void btdExecCameraTarget(u32 p,u32 debug,gabi::Local<cXyz>& local,gabi::Local<cXyz>& transformed) {
    s16 angle=btdExecR<s16>(p,0x322);gabi::call<void>(0x025F1884,btdExecMatrix(),angle);
    f32 y=btdExecR<f32>(debug,0x28),x=btdExecR<f32>(debug,0x24),z=btdExecR<f32>(debug,0x2C);
    local->y=gabi::fmadds(y,0.1f,70.0f);local->x=gabi::fmadds(x,0.1f,-500.0f);local->z=gabi::fmadds(z,0.1f,2000.0f);
    gabi::call<void>(0x0200FCD8,local.get(),transformed.get());
    btdExecCalc(p+0x7060,(f32)transformed->x,0.1f,50.0f);
    btdExecCalc(p+0x7064,(f32)transformed->y,0.1f,50.0f);
    btdExecCalc(p+0x7068,(f32)transformed->z,0.1f,50.0f);
}
void btdExecCameraResume(u32 p,u32 body) {
    btdExecW<s16>(p,0x705A,0);gabi::call<void>(0x02514F38,body);gabi::call<void>(0x02515280,body,0);
    u32 g=btdExecPlay();btdExecW<u16>(g,0x52B8,btdExecR<u16>(g,0x52B8)|8);
    btdExecW<u32>(p,0x2E0,btdExecR<u32>(p,0x2E0)&~0x4000u);
}
void btdExecCamera(u32 p) {
    constexpr u32 debug=0x1047B608,center=0x104629E4;
    u32 player=btdExecR<u32>(btdExecPlay(),0x5B2C);
    s8 cameraID=btdExecR<s8>(btdExecPlay(),0x5B30);u32 game=btdExecPlay();
    s16 mainCount=btdExecR<s16>(p,0x705C),sub=btdExecR<s16>(p,0x705E),mode=btdExecR<s16>(p,0x705A);
    u32 camera=btdExecR<u32>(game+(u32)((s32)cameraID*52),0x5AF8),body=camera+0x248;
    btdExecW<s16>(p,0x705E,(s16)(sub+1));btdExecW<s16>(p,0x705C,(s16)(mainCount+1));
    gabi::Local<cXyz> local,transformed,normalResult,axis,resetCenter,resetEye;
    switch(mode) {
    case 1: {
        btdExecW<u8>(p,0x6207,6);s16 next=btdExecR<s16>(p,0x705A);btdExecW<s16>(p,0x705A,(s16)(next+1));
        btdExecW<f32>(p,0x7080,gabi::fadds_ppc(btdExecR<f32>(debug,0x38),50.0f));
        gabi::call<void>(0x02514F2C,body);gabi::call<void>(0x02515280,body,2);
        btdExecW<s16>(p,0x705E,0);btdExecW<f32>(p,0x7060,282.0f);btdExecW<s16>(p,0x705C,0);
        btdExecW<f32>(p,0x7064,2775.0f);btdExecW<f32>(p,0x7068,-582.0f);
        btdExecW<f32>(p,0x706C,-1200.0f);btdExecW<f32>(p,0x7070,0.0f);btdExecW<f32>(p,0x7074,660.0f);
        btdExecW<f32>(p,0x7084,0.0f);btdExecW<f32>(center,4,0.0f);btdExecW<f32>(center,8,0.0f);btdExecW<f32>(center,0,0.0f);
        [[fallthrough]];
    }
    case 2: {
        sub=btdExecR<s16>(p,0x705E);btdExecW<u8>(p,0x6207,6);
        if(sub>50) {f32 step=gabi::fadds_ppc(btdExecR<f32>(debug,0x20),5.0f);f32 y=btdExecR<f32>(p,0x7064);btdExecW<f32>(p,0x7064,gabi::fsubs_ppc(y,step));}
        local->x=-1373.0f;local->z=790.0f;local->y=btdExecR<f32>(player,0x318);
        btdExecPlayerPosition(player,gabi::ea(local.get()),p,0x8000);
        if(btdExecR<s16>(p,0x705E)!=111)break;
        btdExecW<s16>(p,0x705E,0);
        btdExecW<f32>(p,0x7060,-1728.0f);btdExecW<f32>(p,0x7064,111.0f);btdExecW<f32>(p,0x7068,844.0f);
        btdExecW<f32>(p,0x706C,-1397.0f);btdExecW<f32>(p,0x7070,110.0f);btdExecW<f32>(p,0x7074,685.0f);
        btdExecW<s16>(p,0x705A,3);[[fallthrough]];
    }
    case 3: {
        sub=btdExecR<s16>(p,0x705E);btdExecW<u8>(p,0x6207,6);if(sub<=50)break;
        btdExecCameraStep(p,0x706C,-1536.0f,139.0f);btdExecCameraStep(p,0x7070,411.0f,301.0f);btdExecCameraStep(p,0x7074,752.0f,67.0f);
        btdExecCalc(p+0x7084,1.0f,0.1f,0.005f);sub=btdExecR<s16>(p,0x705E);
        if(sub>=180 && !btdExecR<s16>(debug,0x8C)) {
            btdExecW<s16>(p,0x705E,0);btdExecW<f32>(p,0x7084,0.0f);
            btdExecW<f32>(p,0x7060,-2501.0f);btdExecW<f32>(p,0x7064,418.0f);btdExecW<f32>(p,0x7068,-32.0f);
            btdExecW<f32>(p,0x706C,-2139.0f);btdExecW<s16>(p,0x705A,4);btdExecW<f32>(p,0x7070,373.0f);btdExecW<f32>(p,0x7074,17.0f);btdExecW<f32>(p,0x7080,60.0f);
        } else {
            if(sub>=140) {
                btdExecW<u8>(p,0x6207,7);btdExecW<f32>(p,0x708C,20.0f);gabi::call<void>(0x025E1988,0x105E);
                if(btdExecR<s16>(p,0x705E)==140)btdExecQuake(debug,3,axis);
            }
            break;
        }
        [[fallthrough]];
    }
    case 4: {
        btdExecW<u8>(p,0x6207,7);btdExecW<f32>(p,0x708C,15.0f);gabi::call<void>(0x025E1988,0x105E);
        btdExecCameraStep(p,0x7060,-1763.0f,738.0f);btdExecCameraStep(p,0x7064,167.0f,251.0f);btdExecCameraStep(p,0x7068,812.0f,844.0f);
        btdExecCameraStep(p,0x706C,-1430.0f,709.0f);btdExecCameraStep(p,0x7070,123.0f,250.0f);btdExecCameraStep(p,0x7074,663.0f,646.0f);
        btdExecCalc(p+0x7084,gabi::fadds_ppc(btdExecR<f32>(debug,0x30),0.01f),0.1f,0.001f);
        sub=btdExecR<s16>(p,0x705E);
        if(sub>40) {btdExecCalc(p+0x70C8,0.0f,0.1f,1.0f);sub=btdExecR<s16>(p,0x705E);}
        if(sub==30) {btdExecSound(p,center,0x5825);sub=btdExecR<s16>(p,0x705E);}
        if(sub==80) {gabi::call<void>(0x020F02D4,p);sub=btdExecR<s16>(p,0x705E);}
        if(sub==145) {gabi::call<void>(0x020F02D4,p);sub=btdExecR<s16>(p,0x705E);}
        if(sub==150) {
            btdExecQuake(debug,4,axis);s32 room=btdExecR<s8>(p,0x326);
            btdExecW<s16>(p,0x40E,1);btdExecW<f32>(p,0x7084,0.0f);btdExecW<f32>(p,0x70C8,0.0f);
            btdExecW<s16>(p,0x705E,0);btdExecW<s16>(p,0x705A,5);
            s32 reverb=gabi::call<s32>(0x02520540,room);gabi::call<void>(0x025E1A40,0x5826,center,0,reverb);btdExecW<u8>(p,0x6207,8);
        }
        break;
    }
    case 5: {
        sub=btdExecR<s16>(p,0x705E);
        if(sub==10) {u32 g=btdExecPlay();gabi::call<void>(0x025CB610,g+0x599C,-1);sub=btdExecR<s16>(p,0x705E);}
        if(sub==50) {gabi::call<void>(0x020F02D4,p);sub=btdExecR<s16>(p,0x705E);}
        if(sub<10)break;
        if(sub<205) {
            btdExecCalc(p+0x706C,gabi::fmuls_ppc(btdExecR<f32>(p,0x37C),0.3f),0.2f,gabi::fmuls_ppc(200.0f,btdExecR<f32>(p,0x7084)));
            f32 eyeY=btdExecR<f32>(p,0x380),bias=btdExecR<f32>(debug,0x1C),floor=btdExecR<f32>(debug,0x20);
            f32 target=gabi::fsubs_ppc(gabi::fmadds(bias,0.1f,eyeY),100.0f),minimum=gabi::fadds_ppc(floor,300.0f);
            if(target<minimum)target=minimum;
            btdExecCalc(p+0x7070,target,0.2f,gabi::fmuls_ppc(200.0f,btdExecR<f32>(p,0x7084)));
            btdExecCalc(p+0x7074,gabi::fmuls_ppc(btdExecR<f32>(p,0x384),0.3f),0.2f,gabi::fmuls_ppc(200.0f,btdExecR<f32>(p,0x7084)));
        } else {
            // Native dedicated205 arm zeroes blend before its first calculation.
            f32 firstStep;
            if(sub==205) {firstStep=gabi::fmuls_ppc(2000.0f,0.0f);btdExecW<f32>(p,0x7084,0.0f);}
            else firstStep=gabi::fmuls_ppc(2000.0f,btdExecR<f32>(p,0x7084));
            btdExecCalc(p+0x7060,-1750.0f,0.2f,firstStep);
            btdExecCameraStep(p,0x7064,50.0f,2000.0f,0.2f);btdExecCameraStep(p,0x7068,851.0f,2000.0f,0.2f);
            btdExecCameraStep(p,0x706C,-299.0f,2000.0f,0.2f);btdExecCameraStep(p,0x7070,319.0f,2000.0f,0.2f);btdExecCameraStep(p,0x7074,248.0f,2000.0f,0.2f);
            if(btdExecR<s16>(p,0x705E)>=255)btdExecCalc(p+0x7080,45.0f,0.2f,1.5f);
        }
        btdExecCalc(p+0x7084,1.0f,0.1f,0.01f);break;
    }
    case 100: {
        if(btdExecR<u16>(p,0xF8)!=2) {gabi::call<void>(0x025D7B24,p,2,0xFFFF,0);btdExecW<u16>(p,0xFA,btdExecR<u16>(p,0xFA)|2);}
        else btdExecW<s16>(p,0x705A,101);
        btdExecW<f32>(p,0x7080,gabi::fadds_ppc(btdExecR<f32>(debug,0x38),50.0f));
        u32 view=btdExecR<u32>(btdExecPlay(),0x5AF8);
        // Native camera copies interleave each read and write, preserving aliases.
        for(u32 i=0;i<6;++i)btdExecW<u32>(p,0x7060+4*i,btdExecR<u32>(view,0xDC+4*i));
        gabi::call<void>(0x02514F2C,body);gabi::call<void>(0x02515280,body,2);break;
    }
    case 101: {
        btdExecCopy(gabi::ea(local.get()),player+0x314);local->y=0.0f;
        f32 square=gabi::call<f32>(0x028E8DD0,local.get()),distance=gabi::call<f32>(0x028F4384,square);
        if(distance<1400.0f) {
            gabi::call<void>(0x0201B31C,local.get(),normalResult.get());
            f32 x=local->x,z=local->z;local->x=gabi::fmuls_ppc(x,1400.0f);local->z=gabi::fmuls_ppc(z,1400.0f);
        }
        local->y=btdExecR<f32>(player,0x318);btdExecPlayerPosition(player,gabi::ea(local.get()),p,0x8000);
        btdExecCalc(p+0x7080,gabi::fadds_ppc(btdExecR<f32>(debug,0x18),50.0f),0.5f,3.0f);
        btdExecCalc(p+0x706C,gabi::fmuls_ppc(btdExecR<f32>(p,0x37C),0.3f),0.2f,200.0f);
        f32 eyeY=btdExecR<f32>(p,0x380),bias=btdExecR<f32>(debug,0x1C),minimum=gabi::fadds_ppc(btdExecR<f32>(debug,0x20),500.0f);
        f32 target=gabi::fsubs_ppc(gabi::fmadds(bias,0.1f,eyeY),500.0f);if(target<minimum)target=minimum;
        btdExecCalc(p+0x7070,target,0.2f,200.0f);btdExecCalc(p+0x7074,gabi::fmuls_ppc(btdExecR<f32>(p,0x384),0.3f),0.2f,200.0f);
        btdExecCameraTarget(p,debug,local,transformed);if(btdExecR<s16>(p,0x705E)>=320)btdExecW<u8>(p,0x6207,3);break;
    }
    case 102: {
        sub=btdExecR<s16>(p,0x705E);if(sub<60) {sub=btdExecR<s16>(p,0x705E);btdExecW<u8>(p,0x6207,4);}
        if(sub==90) {sub=btdExecR<s16>(p,0x705E);btdExecW<u8>(p,0x7059,1);}
        s16 delay=(s16)(btdExecR<s16>(debug,0x84)+90);
        if(sub>delay) {
            f32 blend=btdExecR<f32>(p,0x7084),eyeX=btdExecR<f32>(p,0x37C);
            btdExecCalc(p+0x706C,eyeX,gabi::fmuls_ppc(0.2f,blend),gabi::fmuls_ppc(200.0f,blend));
            f32 eyeY=btdExecR<f32>(p,0x380),bias=btdExecR<f32>(debug,0x1C);blend=btdExecR<f32>(p,0x7084);
            btdExecCalc(p+0x7070,gabi::fmadds(bias,0.1f,eyeY),gabi::fmuls_ppc(0.5f,blend),gabi::fmuls_ppc(500.0f,blend));
            blend=btdExecR<f32>(p,0x7084);f32 eyeZ=btdExecR<f32>(p,0x384);
            btdExecCalc(p+0x7074,eyeZ,gabi::fmuls_ppc(0.2f,blend),gabi::fmuls_ppc(200.0f,blend));btdExecCalc(p+0x7084,1.0f,1.0f,0.1f);
        }
        btdExecCameraTarget(p,debug,local,transformed);
        f32 x=btdExecR<f32>(debug,0x30),y=btdExecR<f32>(debug,0x34),z=btdExecR<f32>(debug,0x38);
        local->x=gabi::fmadds(x,0.1f,-250.0f);local->y=gabi::fmadds(y,0.1f,100.0f);local->z=gabi::fmadds(z,0.1f,1600.0f);
        gabi::call<void>(0x0200FCD8,local.get(),p+0x70A0);
        if(btdExecR<s16>(p,0x705E)>110) {
            x=btdExecR<f32>(debug,0x30);z=btdExecR<f32>(debug,0x38);
            local->y=0.0f;local->x=gabi::fmadds(x,0.1f,-50.0f);local->z=gabi::fmadds(z,0.1f,1650.0f);
            gabi::call<void>(0x0200FCD8,local.get(),player+0x314);btdExecPlayerPosition(player,player+0x314,p,-0x4000);
        }
        break;
    }
    case 103:
        btdExecW<f32>(p,0x7084,0.0f);btdExecW<f32>(p,0x706C,0.0f);btdExecW<s16>(p,0x705A,(s16)(mode+1));
        btdExecW<s16>(p,0x705E,0);btdExecW<f32>(p,0x7070,0.0f);btdExecW<f32>(p,0x7074,0.0f);
        btdExecW<s16>(p,0x707A,-6000);btdExecW<s16>(p,0x7078,-11000);btdExecW<f32>(p,0x7080,55.0f);btdExecW<f32>(p,0x7088,2800.0f);
        [[fallthrough]];
    case 104: {
        f32 blend=btdExecR<f32>(p,0x7084);s16 limit=(s16)gabi::ftoi(gabi::fmuls_ppc(22384.0f,blend));
        gabi::call<void>(0x0200F428,p+0x707A,0x4000,16,limit);
        blend=btdExecR<f32>(p,0x7084);s16 target=(s16)(btdExecR<s16>(debug,0x84)-6000);limit=(s16)gabi::ftoi(gabi::fmuls_ppc(5000.0f,blend));
        gabi::call<void>(0x0200F428,p+0x7078,target,16,limit);
        btdExecCalc(p+0x7088,500.0f,0.0625f,gabi::fmuls_ppc(2300.0f,btdExecR<f32>(p,0x7084)));
        s16 angle=btdExecR<s16>(p,0x707A);gabi::call<void>(0x025F1884,btdExecMatrix(),angle);
        angle=btdExecR<s16>(p,0x7078);gabi::call<void>(0x025F1BF4,btdExecMatrix(),angle);
        local->x=0.0f;local->z=btdExecR<f32>(p,0x7088);local->y=0.0f;gabi::call<void>(0x0200FCD8,local.get(),p+0x7060);
        btdExecCalc(p+0x7084,gabi::fadds_ppc(btdExecR<f32>(debug,0x30),0.0048f),0.1f,0.000048f);
        sub=btdExecR<s16>(p,0x705E);if(sub<3)break;
        if(sub==3) {
            if(p && p+0x37C)btdExecSound(p,p+0x37C,0x1863);
            if(btdExecR<u32>(0x10475628))btdExecW<u8>(0x10475652,0,0);
            if(btdExecR<s16>(p,0x705E)<3)break;
        }
        sub=btdExecR<s16>(p,0x705E);btdExecW<u8>(p,0x6207,5);
        if(sub==280) {
            u32 save=btdExecR<u32>(0x101F84DC);btdExecW<f32>(center,4,-50.0f);btdExecW<f32>(center,0,0.0f);btdExecW<f32>(center,8,0.0f);
            gabi::call<void>(0x025B9098,save+0x798,3);s32 room=btdExecR<s8>(p,0x326);
            gabi::call<void>(0x025D9874,center,0,room,0);sub=btdExecR<s16>(p,0x705E);btdExecW<u8>(p,0x70D4,0);
        }
        if(sub==450) {btdExecW<s16>(p,0x705A,151);btdExecW<s16>(p,0x40E,57);}
        break;
    }
    case 150:
        btdExecCopy(gabi::ea(resetEye.get()),p+0x7060);btdExecCopy(gabi::ea(resetCenter.get()),p+0x706C);
        gabi::call<void>(0x0251510C,body,resetCenter.get(),resetEye.get());btdExecCameraResume(p,body);break;
    case 151: {
        f32 x=btdExecR<f32>(player,0x37C),y=btdExecR<f32>(player,0x380),z=btdExecR<f32>(player,0x384);
        resetCenter->x=x;resetCenter->y=btdExecR<f32>(player,0x380);resetCenter->z=btdExecR<f32>(player,0x384);
        resetEye->x=gabi::fmuls_ppc(x,0.9f);resetEye->y=y;resetEye->z=gabi::fmuls_ppc(z,0.9f);
        gabi::call<void>(0x0251510C,body,resetCenter.get(),resetEye.get());
        btdExecCameraResume(p,body);break;
    }
    default:break;
    }
    if(!btdExecR<s16>(p,0x705A))return;
    sub=btdExecR<s16>(p,0x705E);
    // Native table indexes truncate16 then round down to an8-byte sin/cos pair.
    u32 sinIndex=(u32)(u16)((s32)sub*0x3300)&~7u,cosIndex=(u32)(u16)((s32)sub*0x3000)&~7u;
    f32 eyeY=btdExecR<f32>(p,0x7064);s16 count=btdExecR<s16>(p,0x40A);
    f32 amplitude=btdExecR<f32>(p,0x708C),centerX=btdExecR<f32>(p,0x706C);
    f32 sin=btdExecR<f32>(0x104A44F8,sinIndex),eyeX=btdExecR<f32>(p,0x7060);
    f32 fx=gabi::fmuls_ppc(sin,amplitude),cos=btdExecR<f32>(0x104A44F8,cosIndex+4),fy=gabi::fmuls_ppc(cos,amplitude);
    u32 rollIndex=(u32)(u16)((s32)count*0x1C00)&~7u;f32 centerY=btdExecR<f32>(p,0x7070);
    f32 rollCos=btdExecR<f32>(0x104A44F8,rollIndex+4),rollFloat=gabi::fmuls_ppc(gabi::fmuls_ppc(rollCos,amplitude),7.5f);
    f32 eyeZ=btdExecR<f32>(p,0x7068),centerZ=btdExecR<f32>(p,0x7074);
    s16 roll=(s16)gabi::ftoi(rollFloat);
    resetEye->y=gabi::fadds_ppc(eyeY,fy);resetCenter->x=gabi::fadds_ppc(centerX,fx);resetCenter->z=centerZ;
    resetEye->z=eyeZ;resetEye->x=gabi::fadds_ppc(eyeX,fx);resetCenter->y=gabi::fadds_ppc(centerY,fy);
    f32 fov=btdExecR<f32>(p,0x7080);gabi::call<void>(0x02514FE8,body,resetCenter.get(),resetEye.get(),roll,fov);
    f32 decay=gabi::fadds_ppc(btdExecR<f32>(debug,0x48),2.0f);gabi::call<void>(0x0200EDC8,p+0x708C,1.0f,decay);
    s32 main=btdExecR<s16>(p,0x705C);gabi::call<void>(0x027EC9E8,30,430,0x1000BA3Cu,main);
    s32 subReport=btdExecR<s16>(p,0x705E);gabi::call<void>(0x027EC9E8,30,430,0x1000BA50u,subReport);
    mode=btdExecR<s16>(p,0x705A);main=btdExecR<s16>(p,0x705C);
    if(mode<100) {
        if(main==2) {btdExecW<u32>(player,0x428,0);btdExecW<s16>(player,0x420,3);main=btdExecR<s16>(p,0x705C);}
        if(main==115) {btdExecW<u32>(player,0x430,26);main=btdExecR<s16>(p,0x705C);}
        if(main==310) {btdExecW<u32>(player,0x430,28);main=btdExecR<s16>(p,0x705C);}
        if(main==452) {btdExecW<u32>(player,0x430,24);main=btdExecR<s16>(p,0x705C);}
        if(main==465) {btdExecW<u32>(player,0x430,23);btdExecW<u32>(player,0x428,2);main=btdExecR<s16>(p,0x705C);}
        if(main==660) {btdExecW<u32>(player,0x430,27);main=btdExecR<s16>(p,0x705C);}
        if((u32)main==(u32)((s32)btdExecR<s16>(debug,0x86)+733)) {btdExecW<u32>(player,0x430,23);btdExecW<u32>(player,0x428,2);}
    } else {
        if(main==2) {btdExecW<u32>(player,0x428,0);btdExecW<s16>(player,0x420,3);main=btdExecR<s16>(p,0x705C);}
        if(main==10) {btdExecW<u32>(player,0x430,23);btdExecW<u32>(player,0x428,2);main=btdExecR<s16>(p,0x705C);}
        s32 bias=btdExecR<s16>(debug,0x84);
        if((u32)main==(u32)(bias+693)) {btdExecW<u32>(player,0x430,50);bias=btdExecR<s16>(debug,0x84);main=btdExecR<s16>(p,0x705C);}
        if((u32)main==(u32)(bias+705))btdExecW<u32>(player,0x430,29);
    }
}
}
