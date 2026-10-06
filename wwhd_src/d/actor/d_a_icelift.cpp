#include "d/actor/d_a_icelift.h"
#include "bindings.h"
// Guest callees need a linkage area below addressable caller temporaries.
// GABI retains register packing and, for arguments beyond r10, the stack
// argument area at callee SP+8. This extra aligned frame protects our locals.
template<class R = void, class... A>
static R iliftAbiCall(u32 target, A... args) {
    u32 callerSp = gabi::cpu->r[1];
    gabi::Local<be<u32>[4]> linkage;
    gabi::store<u32>(linkage.a, callerSp);
    return gabi::call<R>(target, args...);
}
// Natural calls between this actor's functions use the same outgoing frame.
#undef WWHD_FUNC
#define WWHD_FUNC(addr, R, ...) \
    if (gabi::Activation::nested()) return iliftAbiCall<R>(addr __VA_OPT__(, ) __VA_ARGS__); \
    gabi::Activation wwhd_activation_

// Per-TU helpers and resources belong to the ice-lift binary unit.
static u32 archive(daIlift_c* a) { return gabi::load<u32>(0x101B7938 + u32(a->type)*4); }
static cXyz* vec(u32 a) { return gabi::at<cXyz>(a); }
static f32 sine(u16 a) { return gabi::load<f32>(0x104A44F8 + (u32(a)>>3)*8); }
static f32 cosine(u16 a) { return gabi::load<f32>(0x104A44FC + (u32(a)>>3)*8); }
static void quatCopy(IliftQuat* d,u32 s) { for(u32 i=0;i<4;i++) gabi::store<u32>(gabi::ea(d)+i*4,gabi::load<u32>(s+i*4)); }

void iliftRide(void* bg, daIlift_c* a, fopAc_ac_c* rider) {
    WWHD_FUNC(0x0217C874, void, bg, a, rider);
    gabi::Local<cXyz> offset;
    iliftAbiCall<void>(0x0201ADE0,&rider->current.pos,offset.get(),&a->current.pos);
    a->riderOffset.copy(*offset);
    if (!rider || gabi::load<s16>(gabi::ea(rider)+8)!=0xA8) return;
    gabi::Local<cXyz> up, cross;
    up->y=1.0f; up->z=0.0f; up->x=0.0f;
    a->waveTime=0;
    a->riderPresent=1;
    iliftAbiCall<void>(0x0201B080,offset.get(),cross.get(),up.get());
    offset->copy(*cross);
    f32 mag=iliftAbiCall<f32>(0x028E8DD0,offset.get());
    a->riderRadius=iliftAbiCall<f32>(0x028F4384,mag);
    if (!iliftAbiCall<s32>(0x0201B47C,offset.get())) return;
    s16 target=s16(gabi::ftoi(-(f32(a->riderRadius)*4.0f)));
    a->waveTarget=target;
    iliftAbiCall<void>(0x0200F428,&a->waveAngle,target,8,0x200);
    f32 s=sine(u16(a->waveAngle));
    f32 x=offset->x, y=offset->y, z=offset->z;
    a->targetQuat.x=x*s; a->targetQuat.y=y*s; a->targetQuat.z=z*s;
    a->targetQuat.w=cosine(u16(a->waveAngle));
    a->waveDecay=10.0f;
}
VERIFY(0x0217C874,iliftRide);

BOOL iliftCreateHeap(daIlift_c* a) {
    WWHD_FUNC(0x0217C9E0, BOOL, a);
    gabi::Local<SafeString> name;
    u32 ty=a->type;
    u32 control=gabi::load<u32>(0x101F4F28);
    name->mStringTop=gabi::load<u32>(0x101B7938+ty*4); name->__vtbl=0x10011BA8;
    u32 index=gabi::load<u32>(0x10011C60+ty*4);
    u32 data=iliftAbiCall<u32>(0x026066C4,control,name.get(),index);
    if (!data) iliftAbiCall<void>(0x0273AA24,STR(0x10011C04),0xEB,STR(0x10011C14));
    u32 model=iliftAbiCall<u32>(0x025E38E0,data,0x80000,0x11000022);
    a->modelPtr=model;
    if (!model) return FALSE;
    u32 bg=iliftAbiCall<u32>(0x024F23F4,0);
    a->bgPtr=bg;
    if (!bg) return FALSE;
    ty=a->type; control=gabi::load<u32>(0x101F4F28);
    name->mStringTop=gabi::load<u32>(0x101B7938+ty*4); name->__vtbl=0x10011BA8;
    index=gabi::load<u32>(0x10011C68+ty*4);
    data=iliftAbiCall<u32>(0x026066C4,control,name.get(),index);
    if (iliftAbiCall<s32>(0x0200A030,u32(a->bgPtr),data,1,&a->bgMatrix[0])) return FALSE;
    gabi::store<u32>(u32(a->bgPtr)+0xA8,0x024EE708);
    gabi::store<u32>(u32(a->bgPtr)+0xB0,0x0217C874);
    return TRUE;
}
VERIFY(0x0217C9E0,iliftCreateHeap);
BOOL iliftCheckHeap(daIlift_c* a) { WWHD_FUNC(0x0217CB18,BOOL,a); return iliftCreateHeap(a); }
VERIFY(0x0217CB18,iliftCheckHeap);

void iliftSetMtx(daIlift_c* a) {
    WWHD_FUNC(0x0217CB1C,void,a);
    f32 x=a->scale.x,y=a->scale.y,z=a->scale.z;
    u32 m=a->modelPtr;
    gabi::store<f32>(m+0xBC,x); gabi::store<f32>(m+0xC0,y); gabi::store<f32>(m+0xC4,z);
    x=a->current.pos.x; y=a->current.pos.y; z=a->current.pos.z;
    iliftAbiCall<void>(0x028E93CC,0x1048D0CC,x,y,z);
    iliftAbiCall<void>(0x025F25CC,&a->currentQuat);
    f32 matrix[12]; for(u32 i=0;i<12;i++) matrix[i]=gabi::load<f32>(0x1048D0CC+i*4);
    m=a->modelPtr;
    for(u32 i=0;i<12;i++) gabi::store<f32>(m+0xC8+i*4,matrix[i]);
    x=a->scale.x; y=a->scale.y;
    iliftAbiCall<void>(0x025F2518,x,y,x);
    iliftAbiCall<void>(0x028E90D4,0x1048D0CC,&a->bgMatrix[0]);
}
VERIFY(0x0217CB1C,iliftSetMtx);

void iliftCreateInit(daIlift_c* a) {
    WWHD_FUNC(0x0217CC0C,void,a);
    u32 m=a->modelPtr;
    a->cullMtx=m ? m+0xC8 : 0;
    iliftAbiCall<void>(0x025D674C,a,-200.0f,-250.0f,-200.0f,200.0f,250.0f,200.0f);
    a->cullSizeFar=1.0f;
    iliftAbiCall<void>(0x024EFF44,&a->acchCircle[0],30.0f,30.0f);
    iliftAbiCall<void>(0x024F06B4,&a->acch[0],&a->current.pos,&a->old.pos,a,1,&a->acchCircle[0],&a->speed,0,0);
    gabi::store<u32>(gabi::ea(a)+0x414,gabi::load<u32>(gabi::ea(a)+0x414)&~0x408u);
    a->gravity=-5.0f;
    quatCopy(&a->targetQuat,0x101E9C38);
    quatCopy(&a->currentQuat,0x101E9C38);
    u32 param=a->mParameters;
    a->pathIndex=(param>>4)&0xFF;
    if (u8(a->pathIndex)!=0xFF) {
        u32 p=iliftAbiCall<u32>(0x025AAF88,u32(u8(a->pathIndex)),s32(a->current.roomNo));
        a->pathPtr=p;
        if (p) {
            a->pointDirection=1; a->pointIndex=1;
            u32 pts=gabi::load<u32>(p+8);
            f32 x=gabi::load<f32>(pts+0x14),y=gabi::load<f32>(pts+0x18),z=gabi::load<f32>(pts+0x1C);
            a->targetPoint.x=x; a->targetPoint.y=y; a->targetPoint.z=z;
            a->previousPoint.x=x; a->previousPoint.y=y; a->previousPoint.z=z;
            a->current.pos.x=gabi::load<f32>(gabi::load<u32>(p+8)+4);
            a->current.pos.y=gabi::load<f32>(gabi::load<u32>(p+8)+8);
            a->current.pos.z=gabi::load<f32>(gabi::load<u32>(p+8)+12);
        } else a->pathIndex=0xFF;
    }
    a->switchIndex=(u32(a->mParameters)>>12)&0xFF;
    u32 play=iliftAbiCall<u32>(0x025200D4);
    iliftAbiCall<void>(0x024EEA6C,play+0x12A0,u32(a->bgPtr),a);
    iliftSetMtx(a);
    iliftAbiCall<void>(0x024F43DC,u32(a->bgPtr));
}
VERIFY(0x0217CC0C,iliftCreateInit);

s32 iliftCreate(daIlift_c* a) {
    WWHD_FUNC(0x0217CE04,s32,a);
    if (!(u32(a->actor_condition)&8)) {
        if (a) {
            iliftAbiCall<void>(0x025D4ED0,a);
            a->__vtbl=0x10011BF0;
            iliftAbiCall<void>(0x024F0474,&a->acch[0]);
            gabi::store<u32>(gabi::ea(a)+0x3FC,0x10011BC0);
            gabi::store<u32>(gabi::ea(a)+0x40C,0x10011BD0);
            gabi::store<u32>(gabi::ea(a)+0x400,0x10011BE0);
            gabi::store<u8>(gabi::ea(a)+0x404,1);
            iliftAbiCall<void>(0x024EFE94,&a->acchCircle[0]);
        }
        a->actor_condition=u32(a->actor_condition)|8;
    }
    a->type=u32(a->mParameters)&15;
    if (u8(a->type)>=2) return 5;
    s32 phase=iliftAbiCall<s32>(0x02520460,&a->phase,archive(a));
    if (phase!=4) return phase;
    u32 heapSize=gabi::load<u32>(0x10011C70+u32(a->type)*4);
    if (!iliftAbiCall<s32>(0x025D63E8,a,0x0217CB18,heapSize)) return 5;
    iliftCreateInit(a);
    return phase;
}
VERIFY(0x0217CE04,iliftCreate);
s32 iliftCreateWrapper(daIlift_c* a) { WWHD_FUNC(0x0217CF28,s32,a); return iliftCreate(a); }
VERIFY(0x0217CF28,iliftCreateWrapper);
BOOL iliftDelete(daIlift_c* a) {
    WWHD_FUNC(0x0217CF2C,BOOL,a);
    if (a->heap) {
        u32 play=iliftAbiCall<u32>(0x025200D4);
        iliftAbiCall<void>(0x020087EC,play+0x12A0,u32(a->bgPtr));
    }
    iliftAbiCall<void>(0x025204C8,&a->phase,archive(a));
    return TRUE;
}
VERIFY(0x0217CF2C,iliftDelete);
BOOL iliftDeleteWrapper(daIlift_c* a) { WWHD_FUNC(0x0217CF8C,BOOL,a); return iliftDelete(a); }
VERIFY(0x0217CF8C,iliftDeleteWrapper);
BOOL iliftDraw(daIlift_c* a) {
    WWHD_FUNC(0x0217CF90,BOOL,a);
    u32 env=iliftAbiCall<u32>(0x02555D0C);
    iliftAbiCall<void>(0x025626A4,env,1,&a->current.pos,&a->tevStr);
    env=iliftAbiCall<u32>(0x02555D0C);
    iliftAbiCall<void>(0x02562F5C,env,u32(a->modelPtr),&a->tevStr);
    u32 play=iliftAbiCall<u32>(0x025200D4);
    gabi::store<u32>(0x104B4634,gabi::load<u32>(play+0x5D70));
    play=iliftAbiCall<u32>(0x025200D4);
    gabi::store<u32>(0x104B4638,gabi::load<u32>(play+0x5D74));
    iliftAbiCall<void>(0x025E2DE0,u32(a->modelPtr),0);
    play=iliftAbiCall<u32>(0x025200D4);
    gabi::store<u32>(0x104B4634,gabi::load<u32>(play+0x5D78));
    play=iliftAbiCall<u32>(0x025200D4);
    gabi::store<u32>(0x104B4638,gabi::load<u32>(play+0x5D7C));
    return TRUE;
}
VERIFY(0x0217CF90,iliftDraw);
BOOL iliftDrawWrapper(daIlift_c* a) { WWHD_FUNC(0x0217D028,BOOL,a); return iliftDraw(a); }
VERIFY(0x0217D028,iliftDrawWrapper);

void iliftNextPoint(daIlift_c* a) {
    WWHD_FUNC(0x0217D02C,void,a);
    if (u8(a->pathIndex)==0xFF) return;
    s32 next=s8(u8(a->pointIndex)+u8(a->pointDirection));
    a->pointIndex=next;
    u32 p=a->pathPtr;
    if (gabi::load<u8>(p+5)&1) {
        u16 count=gabi::load<u16>(p);
        if (next>s32(s8(count))-1) a->pointIndex=0;
        else if (next<0) a->pointIndex=count-1;
    } else {
        u16 count=gabi::load<u16>(p);
        if (next>s32(count)-1) {
            a->pointDirection=-1;
            a->pointIndex=gabi::load<u16>(u32(a->pathPtr))-2;
        } else if (next<0) { a->pointDirection=1; a->pointIndex=1; }
    }
    a->previousPoint.copy(a->targetPoint);
    u32 pts=gabi::load<u32>(u32(a->pathPtr)+8)+u32(s32(a->pointIndex)*16);
    a->targetPoint.x=gabi::load<f32>(pts+4);
    a->targetPoint.y=gabi::load<f32>(pts+8);
    a->targetPoint.z=gabi::load<f32>(pts+12);
}
VERIFY(0x0217D02C,iliftNextPoint);

void iliftNormalMove(daIlift_c* a) {
    WWHD_FUNC(0x0217D11C,void,a);
    u32 state=a->moveState;
    if (state==0) {
        a->moveTimer=0; a->moveState=1;
        iliftNextPoint(a);
    }
    if (state<=1) {
        a->moveTimer=u16(a->moveTimer)+1;
        f32 goal=a->targetSpeed;
        if (iliftAbiCall<f32>(0x0200ECD4,&a->moveSpeed,goal,0.25f,1.0f,1.0f)==0.0f) a->moveState=2;
    } else if (state==2) {
        gabi::Local<cXyz> diff;
        a->moveTimer=0;
        iliftAbiCall<void>(0x0201ADE0,&a->current.pos,diff.get(),&a->targetPoint);
        f32 mag=iliftAbiCall<f32>(0x028E8DD0,diff.get());
        f32 distance=iliftAbiCall<f32>(0x028F4384,mag);
        if (distance<50.0f) a->moveState=3;
    } else if (state==3) {
        f32 goal=f32(a->targetSpeed)/2.2f;
        a->moveTimer=u16(a->moveTimer)+1;
        if (iliftAbiCall<f32>(0x0200ECD4,&a->moveSpeed,goal,0.25f,1.0f,1.0f)==0.0f) a->moveState=0;
    }
    f32 speed=a->moveSpeed;
    iliftAbiCall<void>(0x0200F164,&a->current.pos,&a->targetPoint,1.0f,speed);
}
VERIFY(0x0217D11C,iliftNormalMove);
void iliftPathMove(daIlift_c* a) { WWHD_FUNC(0x0217D294,void,a); if(u8(a->pathIndex)!=0xFF) iliftNormalMove(a); }
VERIFY(0x0217D294,iliftPathMove);

void iliftWave(daIlift_c* a) {
    WWHD_FUNC(0x0217D2A4,void,a);
    gabi::Local<cXyz> up;
    up->z=0.0f; up->y=1.0f; up->x=0.0f;
    u32 play=iliftAbiCall<u32>(0x025200D4);
    u32 player=gabi::load<u32>(play+0x5B2C);
    if (u8(a->riderPresent)) return;
    if (u8(a->riderPrevious)) {
        gabi::Local<cXyz> delta, horizontal;
        iliftAbiCall<void>(0x0201ADE0,&a->previousPlayerPosition,delta.get(),vec(player+0x314));
        horizontal->x=delta->x; horizontal->y=0.0f; horizontal->z=delta->z;
        f32 mag=iliftAbiCall<f32>(0x028E8DD0,horizontal.get());
        f32 distance=iliftAbiCall<f32>(0x028F4384,mag);
        f32 factor=distance/18.0f;
        f32 tilt=f32(a->riderRadius)*4.0f;
        tilt=tilt*10.0f;
        a->riderRadius=0.0f;
        a->waveTarget=s16(gabi::ftoi(-(tilt*factor)));
        if (u8(a->riderPresent)) return;
    }
    s16 angle=a->waveTarget;
    s32 time=a->waveTime;
    u16 phase=u16(u32(time)<<10)&0xFC00;
    f32 target=(f32(angle)*cosine(phase))/f32(time);
    iliftAbiCall<void>(0x0200F378,&a->waveAngle,s16(gabi::ftoi(target)),4,0x800,0x400);
    f32 s=sine(u16(a->waveAngle));
    gabi::Local<cXyz> cross, normalized;
    iliftAbiCall<void>(0x0201B080,&a->riderOffset,cross.get(),up.get());
    iliftAbiCall<void>(0x0201B12C,cross.get(),normalized.get());
    cross->copy(*normalized);
    s32 nonzero=iliftAbiCall<s32>(0x0201AFD8,cross.get(),vec(0x101FFBA8));
    time=a->waveTime;
    if (nonzero) {
        f32 x=cross->x,y=cross->y,z=cross->z;
        a->targetQuat.x=x*s; a->targetQuat.y=y*s; a->targetQuat.z=z*s;
        a->targetQuat.w=cosine(u16(a->waveAngle));
    }
    if (time>120) a->waveTarget=0;
}
VERIFY(0x0217D2A4,iliftWave);

BOOL iliftExecute(daIlift_c* a) {
    WWHD_FUNC(0x0217D4E0,BOOL,a);
    u32 play=iliftAbiCall<u32>(0x025200D4);
    u32 count=a->frameCount;
    u32 player=gabi::load<u32>(play+0x5B2C);
    u32 time=a->waveTime;
    a->frameCount=count+1; a->waveTime=time+1; a->targetSpeed=4.0f;
    iliftPathMove(a); iliftWave(a);
    iliftAbiCall<void>(0x028E9BC0,&a->currentQuat,&a->targetQuat,&a->currentQuat,0.25f);
    a->riderPrevious=u8(a->riderPresent);
    u32 sw=a->switchIndex;
    a->riderPresent=0;
    if (sw!=0xFF) {
        u32 save=gabi::load<u32>(0x101F84DC);
        s32 on=iliftAbiCall<s32>(0x025BA0C0,save+0x20,sw,s32(a->home.roomNo));
        iliftAbiCall<void>(0x024F43EC,u32(a->bgPtr),0x40,on ? 0xF : 0x15);
        iliftAbiCall<void>(0x024F43EC,u32(a->bgPtr),0x41,on ? 0 : 9);
    }
    iliftSetMtx(a);
    iliftAbiCall<void>(0x024F43DC,u32(a->bgPtr));
    a->previousPlayerPosition.copy(*vec(player+0x314));
    return TRUE;
}
VERIFY(0x0217D4E0,iliftExecute);
BOOL iliftExecuteWrapper(daIlift_c* a) { WWHD_FUNC(0x0217D608,BOOL,a); return iliftExecute(a); }
VERIFY(0x0217D608,iliftExecuteWrapper);

void iliftStaticInit() {
    WWHD_FUNC(0x0217D60C,void);
    gabi::store<u32>(0x104647C8,0); gabi::store<u32>(0x104647C0,0);
    gabi::store<u32>(0x104647CC,0); gabi::store<u32>(0x104647C4,0);
    iliftAbiCall<void>(0x028F026C,0x101B7914);
    gabi::store<f32>(0x104647B4,-3.1415927410125732f);
    gabi::store<f32>(0x104647B8,3.1415927410125732f);
    iliftAbiCall<void>(0x028ED6F8,0x104647BC);
    iliftAbiCall<void>(0x028F026C,0x101B7920);
    iliftAbiCall<void>(0x028EAB2C,0x104647BD);
    iliftAbiCall<void>(0x028F026C,0x101B792C);
}
VERIFY(0x0217D60C,iliftStaticInit);
void iliftSafeStringDtor(SafeString* p,u32 flags) { WWHD_FUNC(0x0217D6A0,void,p,flags); if(p && (flags&1)) iliftAbiCall<void>(0x0273AF40,p); }
VERIFY(0x0217D6A0,iliftSafeStringDtor);
BOOL iliftIsDelete(daIlift_c* a) { WWHD_FUNC(0x0217D6B4,BOOL,a); return TRUE; }
VERIFY(0x0217D6B4,iliftIsDelete);
void iliftDtor(daIlift_c* a,u32 flags) {
    WWHD_FUNC(0x0217D6BC,void,a,flags);
    if (!a) return;
    iliftAbiCall<void>(0x02018034,gabi::ea(a)+0x5C4,2);
    gabi::store<u32>(gabi::ea(a)+0x40C,0x10011BD0);
    gabi::store<u32>(gabi::ea(a)+0x400,0x10011BE0);
    iliftAbiCall<void>(0x024EFD9C,&a->acch[0],0);
    iliftAbiCall<void>(0x025D50BC,a,0);
    if (flags&1) iliftAbiCall<void>(0x0273AF40,a);
}
VERIFY(0x0217D6BC,iliftDtor);
void iliftSafeStringVirtual(SafeString* a) { WWHD_FUNC(0x0217D740,void,a); }
VERIFY(0x0217D740,iliftSafeStringVirtual);
