#include "gabi.h"
#include <cmath>
using namespace gabi;

// Local vectors retain their complete 12-byte callee payloads.
struct LibVec {
    be<f32> x,y,z;
};

static void rawcopy(LibVec* d, LibVec* s) {
    for (u32 i=0; i<12; i+=4) store<u32>(ea(d)+i,load<u32>(ea(s)+i));
}

static void localcopy(LibVec* d, LibVec* s) {
    u32 y=load<u32>(ea(s)+4),z=load<u32>(ea(s)+8),x=load<u32>(ea(s));
    store<u32>(ea(d)+4,y);
    store<u32>(ea(d)+8,z);
    store<u32>(ea(d),x);
}

static f32 length(LibVec* v) {
    f32 q=call<f32>(0x028E8DD0,v);
    return call<f32>(0x028F4384,q);
}

static f32 distance(LibVec* a,LibVec* b) {
    f32 q=call<f32>(0x028E8DE8,a,b);
    return call<f32>(0x028F4384,q);
}

static void flatten(LibVec* d,LibVec* s,f32 zero) {
    f32 x=s->x,z=s->z;
    d->x=x;
    d->y=zero;
    d->z=z;
}

void cLib_memSet_hd(void* p,u32 v,u32 n) {
    WWHD_FUNC(0x0200ECCC,void,p,v,n);
    call<void>(0xC0009990,p,v&255,n);
}

VERIFY(0x0200ECCC,cLib_memSet_hd);

f32 cLib_addCalc_hd(be<f32>* p,f32 target,f32 rate,f32 maximum,f32 minimum) {

    WWHD_FUNC(0x0200ECD4,f32,p,target,rate,maximum,minimum);

    f32 x=*p;
    if(x==target)return std::fabs(fsubs_ppc(target,x));

    f32 diff=fsubs_ppc(target,x),step=fmuls_ppc(diff,rate);

    if(!(step<minimum) || !(step>-minimum)) {
        if(step>maximum)step=maximum;
        if(step<-maximum)step=-maximum;
        x=fadds_ppc(x,step);
        *p=x;
        return std::fabs(fsubs_ppc(target,x));
    }

    if(step>load<f32>(0x10001EBC)) {
        x=fadds_ppc(x,minimum);
        if(x>target)x=target;
    }

    else {
        x=fadds_ppc(x,-minimum);
        if(x<target)x=target;
    }

    *p=x;
    return std::fabs(fsubs_ppc(target,x));

}

VERIFY(0x0200ECD4,cLib_addCalc_hd);

void cLib_addCalc2_hd(be<f32>*p,f32 target,f32 rate,f32 maximum) {
    WWHD_FUNC(0x0200ED84,void,p,target,rate,maximum);
    f32 x=*p;
    if(x==target)return;
    f32 step=fmuls_ppc(fsubs_ppc(target,x),rate);
    if(step>maximum){
        *p=fadds_ppc(x,maximum);
        return;
    }
    if(step<-maximum)step=-maximum;
    *p=fadds_ppc(x,step);
}

VERIFY(0x0200ED84,cLib_addCalc2_hd);

void cLib_addCalc0_hd(be<f32>*p,f32 rate,f32 maximum) {
    WWHD_FUNC(0x0200EDC8,void,p,rate,maximum);
    f32 x=*p,step=fmuls_ppc(x,rate);
    if(step>maximum){
        *p=fsubs_ppc(x,maximum);
        return;
    }
    if(step<-maximum)step=-maximum;
    *p=fsubs_ppc(x,step);
}

VERIFY(0x0200EDC8,cLib_addCalc0_hd);

f32 cLib_addCalcPos_hd(LibVec*p,LibVec*t,f32 rate,f32 maximum,f32 minimum) {

    WWHD_FUNC(0x0200EE00,f32,p,t,rate,maximum,minimum);

    if(call<s32>(0x0201AFD8,p,t)!=0) {
        Local<LibVec> delta,step;
        call<void>(0x0201ADE0,p,delta.get(),t);
        localcopy(step.get(),delta.get());
        f32 d=length(step.get());

        if(!(d<minimum)) {
            f32 scaled=fmuls_ppc(d,rate);
            call<void>(0x028E8E64,step.get(),step.get(),rate);
            if(!(std::fabs(scaled)<load<f32>(0x10001EC0))) {
                if(scaled>maximum || scaled<minimum){
                    if(!(scaled>maximum))maximum=minimum;
                    call<void>(0x028E8E64,step.get(),step.get(),maximum/scaled);
                }
                call<void>(0x028E8DAC,p,step.get(),p);
                return distance(p,t);
            }
        }

        rawcopy(p,t);
    }

    return distance(p,t);

}

VERIFY(0x0200EE00,cLib_addCalcPos_hd);

f32 cLib_addCalcPosXZ_hd(LibVec*p,LibVec*t,f32 rate,f32 maximum,f32 minimum) {

    WWHD_FUNC(0x0200EF78,f32,p,t,rate,maximum,minimum);

    f32 zero=load<f32>(0x10001EBC);

    if(!(p->x==t->x && p->z==t->z)) {
        Local<LibVec> delta,flat;
        call<void>(0x0201ADE0,p,delta.get(),t);
        flatten(flat.get(),delta.get(),zero);
        f32 d=length(flat.get());

        if(!(d<minimum)) {
            f32 scaled=fmuls_ppc(d,rate);
            call<void>(0x028E8E64,delta.get(),delta.get(),rate);
            if(!(std::fabs(scaled)<load<f32>(0x10001EC0))) {
                if(scaled>maximum || scaled<minimum){
                    if(!(scaled>maximum))maximum=minimum;
                    call<void>(0x028E8E64,delta.get(),delta.get(),maximum/scaled);
                }
                f32 x=p->x,a=delta->x,b=delta->z;
                f32 nx=fsubs_ppc(x,a);
                f32 z=p->z,nz=fsubs_ppc(z,b);
                p->x=nx;
                p->z=nz;
                Local<LibVec> remaining,restflat;
                call<void>(0x0201ADE0,p,remaining.get(),t);
                f32 rz=remaining->z,rx=remaining->x;
                restflat->z=rz;
                restflat->x=rx;
                restflat->y=zero;
                return length(restflat.get());
            }
        }

        // A target snap copies components without arithmetic, preserving payload bits.
        store<u32>(ea(p),load<u32>(ea(t)));
        store<u32>(ea(p)+8,load<u32>(ea(t)+8));
    }

    Local<LibVec> remaining,flat;
    call<void>(0x0201ADE0,p,remaining.get(),t);
    flatten(flat.get(),remaining.get(),zero);
    return length(flat.get());

}

VERIFY(0x0200EF78,cLib_addCalcPosXZ_hd);

void cLib_addCalcPos2_hd(LibVec*p,LibVec*t,f32 rate,f32 maximum) {

    WWHD_FUNC(0x0200F164,void,p,t,rate,maximum);

    if(call<s32>(0x0201AFD8,p,t)==0)return;
    Local<LibVec> delta,scaled,step,normal;
    call<void>(0x0201ADE0,p,delta.get(),t);
    call<void>(0x0201AE48,delta.get(),scaled.get(),rate);
    localcopy(step.get(),scaled.get());
    f32 d=length(step.get());
    if(d>maximum){
        call<void>(0x0201B12C,step.get(),normal.get());
        localcopy(step.get(),normal.get());
        call<void>(0x028E8E64,step.get(),step.get(),maximum);
    }
    call<void>(0x028E8DAC,p,step.get(),p);

}

VERIFY(0x0200F164,cLib_addCalcPos2_hd);

void cLib_addCalcPosXZ2_hd(LibVec*p,LibVec*t,f32 rate,f32 maximum) {

    WWHD_FUNC(0x0200F268,void,p,t,rate,maximum);

    if(p->x==t->x && p->z==t->z)return;
    Local<LibVec> delta,step,flat;
    call<void>(0x0201ADE0,p,delta.get(),t);
    call<void>(0x0201AE48,delta.get(),step.get(),rate);
    flatten(flat.get(),step.get(),load<f32>(0x10001EBC));
    f32 d=length(flat.get());
    if(std::fabs(d)<load<f32>(0x10001EC0))return;
    if(d>maximum)call<void>(0x028E8E64,step.get(),step.get(),maximum/d);
    f32 x=p->x,a=step->x,z=p->z;
    f32 nx=fsubs_ppc(x,a),b=step->z,nz=fsubs_ppc(z,b);
    p->x=nx;
    p->z=nz;

}

VERIFY(0x0200F268,cLib_addCalcPosXZ2_hd);

// Integer parameters stay full register width; HD truncates only explicit halfword operations.
// PPC signed division by zero/overflow returns a sign-derived sentinel.
static s32 divsafe(s32 a,s32 b){
    return !b || (a==INT32_MIN && b==-1)?(a<0?-1:0):a/b;
}

s16 cLib_addCalcAngleS_hd(be<s16>*p,s32 target,s32 divisor,s32 maximum,s32 minimum) {

    WWHD_FUNC(0x0200F378,s16,p,target,divisor,maximum,minimum);

    s32 x=*p;
    s16 diff=(s16)((u32)target-(u32)x);
    if(x==target)return diff;
    s32 step=(s16)divsafe(diff,divisor);

    if(step>minimum || step<(s32)(0u-(u32)minimum)){
        if(step>maximum)step=maximum;
        if(step<(s32)(0u-(u32)maximum))step=(s16)(0u-(u32)maximum);
        x=(s32)((u32)x+(u32)step);
        *p=(s16)x;
        return (s16)((u32)target-(u32)x);
    }

    if(diff>=0){
        x=(s16)((u32)x+(u32)minimum);
        s16 rem=(s16)((u32)target-(u32)x);
        if(rem>0){
            *p=(s16)x;
            return rem;
        }
    }

    else{
        x=(s16)((u32)x-(u32)minimum);
        s16 rem=(s16)((u32)target-(u32)x);
        if(rem<0){
            *p=(s16)x;
            return rem;
        }
    }

    *p=target;
    return 0;

}

VERIFY(0x0200F378,cLib_addCalcAngleS_hd);

void cLib_addCalcAngleS2_hd(be<s16>*p,s32 target,s32 divisor,s32 maximum) {
    WWHD_FUNC(0x0200F428,void,p,target,divisor,maximum);
    s32 x=*p,step=(s16)divsafe((s16)((u32)target-(u32)x),divisor);
    if(step>maximum)step=maximum;
    else if(step<(s32)(0u-(u32)maximum))step=(s32)(0u-(u32)maximum);
    *p=(s16)((u32)x+(u32)step);
}

VERIFY(0x0200F428,cLib_addCalcAngleS2_hd);

s32 cLib_addCalcAngleL_hd(be<s32>*p,s32 target,s32 divisor,s32 maximum,s32 minimum) {

    WWHD_FUNC(0x0200F474,s32,p,target,divisor,maximum,minimum);

    u32 x=(u32)(s32)*p;
    s32 diff=(s32)((u32)target-x);
    if(x==(u32)target)return diff;
    s32 step=divsafe(diff,divisor),nmin=(s32)(0u-(u32)minimum),nmax=(s32)(0u-(u32)maximum);

    if(step>minimum || step<nmin){
        if(step>maximum)step=maximum;
        if(step<nmax)step=nmax;
        *p=(s32)(x+(u32)step);
    }

    else {
        u32 next=diff>=0?x+(u32)minimum:x-(u32)minimum;
        s32 rem=(s32)((u32)target-next);
        *p=((diff>=0 && rem>0)||(diff<0 && rem<0))?(s32)next:target;
    }

    return diff;

}

VERIFY(0x0200F474,cLib_addCalcAngleL_hd);

s32 cLib_chaseUC_hd(be<u8>*p,s32 target,s32 step) {
    WWHD_FUNC(0x0200F4FC,s32,p,target,step);
    s32 x=*p;
    if(step==0)return x==target;
    if(x>target)step=(s16)(0u-(u32)step);
    x=(s16)((u32)x+(u32)step);
    s32 prod=(s32)((u32)((u32)x-(u32)target)*(u32)(s32)step);
    if(prod<0){
        *p=(u8)x;
        return 0;
    }
    *p=target;
    return 1;
}

VERIFY(0x0200F4FC,cLib_chaseUC_hd);

s32 cLib_chaseS_hd(be<s16>*p,s32 target,s32 step) {
    WWHD_FUNC(0x0200F564,s32,p,target,step);
    s32 x=*p;
    if(step==0)return x==target;
    if(x>target)step=(s16)(0u-(u32)step);
    x=(s16)((u32)x+(u32)step);
    s32 prod=(s32)((u32)((u32)x-(u32)target)*(u32)(s32)step);
    if(prod<0){
        *p=(s16)x;
        return 0;
    }
    *p=target;
    return 1;
}

VERIFY(0x0200F564,cLib_chaseS_hd);

s32 cLib_chaseF_hd(be<f32>*p,f32 target,f32 step) {
    WWHD_FUNC(0x0200F5C8,s32,p,target,step);
    f32 zero=load<f32>(0x10001EBC),x=*p;
    if(step==zero)return x==target;
    if(x>target)step=-step;
    f32 next=fadds_ppc(x,step),prod=fmuls_ppc(fsubs_ppc(next,target),step);
    if(prod<zero){
        *p=next;
        return 0;
    }
    *p=target;
    return 1;
}

VERIFY(0x0200F5C8,cLib_chaseF_hd);

s32 cLib_chasePos_hd(LibVec*p,LibVec*t,f32 step) {
    WWHD_FUNC(0x0200F62C,s32,p,t,step);
    if(step==load<f32>(0x10001EBC))return call<s32>(0x0201AF98,p,t)!=0;
    Local<LibVec> delta,scaled;
    call<void>(0x0201ADE0,p,delta.get(),t);
    f32 d=length(delta.get());
    if(std::fabs(d)<load<f32>(0x10001EC0)||!(d>step)){
        rawcopy(p,t);
        return 1;
    }
    call<void>(0x0201AE48,delta.get(),scaled.get(),step/d);
    call<void>(0x028E8DAC,p,scaled.get(),p);
    return 0;
}

VERIFY(0x0200F62C,cLib_chasePos_hd);

s32 cLib_chasePosXZ_hd(LibVec*p,LibVec*t,f32 step) {
    WWHD_FUNC(0x0200F764,s32,p,t,step);
    Local<LibVec> delta,flat,scaled;
    call<void>(0x0201ADE0,p,delta.get(),t);
    f32 x=delta->x,zero=load<f32>(0x10001EBC),z=delta->z;
    delta->y=zero;
    flat->y=zero;
    flat->x=x;
    flat->z=z;
    f32 d=length(flat.get()),epsilon=load<f32>(0x10001EC0);
    if(step==zero)return std::fabs(d)<epsilon;
    if(std::fabs(d)<epsilon||!(d>step)){
        rawcopy(p,t);
        return 1;
    }
    call<void>(0x0201AE48,delta.get(),scaled.get(),step/d);
    call<void>(0x028E8DAC,p,scaled.get(),p);
    return 0;
}

VERIFY(0x0200F764,cLib_chasePosXZ_hd);

s32 cLib_chaseAngleS_hd(be<s16>*p,s32 target,s32 step) {
    WWHD_FUNC(0x0200F8D0,s32,p,target,step);
    s32 x=*p;
    if(step==0)return x==target;
    if((s16)((u32)x-(u32)target)>0)step=(s16)(0u-(u32)step);
    x=(s16)((u32)x+(u32)step);
    s32 prod=(s32)((u32)(s32)(s16)((u32)x-(u32)target)*(u32)(s32)step);
    if(prod<0){
        *p=(s16)x;
        return 0;
    }
    *p=target;
    return 1;
}

VERIFY(0x0200F8D0,cLib_chaseAngleS_hd);

s16 cLib_targetAngleY_hd(LibVec*p,LibVec*t) {
    WWHD_FUNC(0x0200F93C,s16,p,t);
    f32 x=p->x,tx=t->x,z=p->z;
    f32 dx=fsubs_ppc(tx,x),tz=t->z,dz=fsubs_ppc(tz,z);
    return call<s16>(0x020195B0,dx,dz);
}

VERIFY(0x0200F93C,cLib_targetAngleY_hd);

s16 cLib_targetAngleY2_hd(LibVec*p,LibVec*t) {
    WWHD_FUNC(0x0200F958,s16,p,t);
    f32 x=p->x,tx=t->x,z=p->z;
    f32 dx=fsubs_ppc(tx,x),tz=t->z,dz=fsubs_ppc(tz,z);
    return call<s16>(0x020195B0,dx,dz);
}

VERIFY(0x0200F958,cLib_targetAngleY2_hd);

s16 cLib_targetAngleX_hd(LibVec*p,LibVec*t) {
    WWHD_FUNC(0x0200F974,s16,p,t);
    Local<LibVec> delta,flat;
    call<void>(0x0201ADE0,t,delta.get(),p);
    f32 x=delta->x,zero=load<f32>(0x10001EBC);
    flat->x=x;
    f32 z=delta->z;
    flat->y=zero;
    flat->z=z;
    f32 d=length(flat.get());
    f32 y=delta->y;
    return call<s16>(0x020195B0,y,d);
}

VERIFY(0x0200F974,cLib_targetAngleX_hd);

static void offset(LibVec*d,LibVec*base,u16 angle,LibVec*o) {
    u32 tab=0x104A44F8u+(angle>>3)*8u;
    f32 oz=o->z,s=load<f32>(tab),ox=o->x;
    f32 zs=fmuls_ppc(oz,s),c=load<f32>(tab+4),dx=fmadds(ox,c,zs),bx=base->x;
    d->x=fadds_ppc(bx,dx);
    f32 by=base->y,oy=o->y;
    d->y=fadds_ppc(by,oy);
    ox=o->x;
    oz=o->z;
    f32 xs=fmuls_ppc(ox,s),dz=fmsubs(oz,c,xs),bz=base->z;
    d->z=fadds_ppc(bz,dz);
}

void cLib_offsetPos_hd(LibVec*d,LibVec*base,s16 angle,LibVec*o) {
    WWHD_FUNC(0x0200F9D4,void,d,base,angle,o);
    offset(d,base,(u16)angle,o);
}

VERIFY(0x0200F9D4,cLib_offsetPos_hd);

void cLib_offsetPos2_hd(LibVec*d,LibVec*base,s16 angle,LibVec*o) {
    WWHD_FUNC(0x0200FA40,void,d,base,angle,o);
    offset(d,base,(u16)angle,o);
}

VERIFY(0x0200FA40,cLib_offsetPos2_hd);

s32 cLib_distanceAngleS_hd(s16 a,s16 b) {
    WWHD_FUNC(0x0200FAAC,s32,a,b);
    s32 d=(s16)(a-b);
    return d<0?-d:d;
}

VERIFY(0x0200FAAC,cLib_distanceAngleS_hd);

// GameCube MtxInit resets the current matrix-stack pointer.
void MtxInit_hd() {
    WWHD_FUNC(0x0200FAC4,void);
    store<u32>(0x1018C7B0,0x101FF5F0);
}

VERIFY(0x0200FAC4,MtxInit_hd);


struct LibMtx { be<f32> element[12]; };
void MtxTrans_hd(f32 x, f32 y, f32 z, u32 concat) {
    WWHD_FUNC(0x0200FAD8, void, x, y, z, concat);
    if (concat == 0) {
        call<void>(0x028E93CC, at<LibMtx>(load<u32>(0x1018C7B0)), x, y, z);
    } else {
        Local<LibMtx> temporary;
        call<void>(0x028E93CC, temporary.get(), x, y, z);
        LibMtx* current = at<LibMtx>(load<u32>(0x1018C7B0));
        call<void>(0x028E9108, current, temporary.get(), current);
    }
}
VERIFY(0x0200FAD8, MtxTrans_hd);
void MtxRotX_hd(f32 angle, u32 concat) {
    WWHD_FUNC(0x0200FB3C, void, angle, concat);
    if (concat == 0) {
        call<void>(0x028E98C0, at<LibMtx>(load<u32>(0x1018C7B0)), 0x58, angle);
    } else {
        Local<LibMtx> temporary;
        call<void>(0x028E98C0, temporary.get(), 0x58, angle);
        LibMtx* current = at<LibMtx>(load<u32>(0x1018C7B0));
        call<void>(0x028E9108, current, temporary.get(), current);
    }
}
VERIFY(0x0200FB3C, MtxRotX_hd);
void MtxRotY_hd(f32 angle, u32 concat) {
    WWHD_FUNC(0x0200FBA4, void, angle, concat);
    if (concat == 0) {
        call<void>(0x028E98C0, at<LibMtx>(load<u32>(0x1018C7B0)), 0x59, angle);
    } else {
        Local<LibMtx> temporary;
        call<void>(0x028E98C0, temporary.get(), 0x59, angle);
        LibMtx* current = at<LibMtx>(load<u32>(0x1018C7B0));
        call<void>(0x028E9108, current, temporary.get(), current);
    }
}
VERIFY(0x0200FBA4, MtxRotY_hd);
void MtxRotZ_hd(f32 angle, u32 concat) {
    WWHD_FUNC(0x0200FC0C, void, angle, concat);
    if (concat == 0) {
        call<void>(0x028E98C0, at<LibMtx>(load<u32>(0x1018C7B0)), 0x5A, angle);
    } else {
        Local<LibMtx> temporary;
        call<void>(0x028E98C0, temporary.get(), 0x5A, angle);
        LibMtx* current = at<LibMtx>(load<u32>(0x1018C7B0));
        call<void>(0x028E9108, current, temporary.get(), current);
    }
}
VERIFY(0x0200FC0C, MtxRotZ_hd);
void MtxScale_hd(f32 x, f32 y, f32 z, u32 concat) {
    WWHD_FUNC(0x0200FC74, void, x, y, z, concat);
    if (concat == 0) {
        call<void>(0x028E945C, at<LibMtx>(load<u32>(0x1018C7B0)), x, y, z);
    } else {
        Local<LibMtx> temporary;
        call<void>(0x028E945C, temporary.get(), x, y, z);
        LibMtx* current = at<LibMtx>(load<u32>(0x1018C7B0));
        call<void>(0x028E9108, current, temporary.get(), current);
    }
}
VERIFY(0x0200FC74, MtxScale_hd);
void MtxPosition_hd(LibVec* source, LibVec* result) {
    WWHD_FUNC(0x0200FCD8, void, source, result);
    call<void>(0x028E8F64, at<LibMtx>(load<u32>(0x1018C7B0)), source, result);
}
VERIFY(0x0200FCD8, MtxPosition_hd);
void MtxPush_hd() {
    WWHD_FUNC(0x0200FCF0, void);
    Local<LibMtx> temporary;
    call<void>(0x028E90D4, at<LibMtx>(load<u32>(0x1018C7B0)), temporary.get());
    u32 next = load<u32>(0x1018C7B0) + 0x30;
    store<u32>(0x1018C7B0, next);
    call<void>(0x028E90D4, temporary.get(), at<LibMtx>(next));
}
VERIFY(0x0200FCF0, MtxPush_hd);
void MtxPull_hd() {
    WWHD_FUNC(0x0200FD38, void);
    store<u32>(0x1018C7B0, load<u32>(0x1018C7B0) - 0x30u);
}
VERIFY(0x0200FD38, MtxPull_hd);
void c_lib_static_init_hd() {
    WWHD_FUNC(0x0200FD4C, void);
    store<u32>(0x101FF5E8, 0);
    store<u32>(0x101FF5E0, 0);
    store<u32>(0x101FF5EC, 0);
    store<u32>(0x101FF5E4, 0);
    call<void>(0x028F026C, at<void>(0x1018C78C));
    f32 a = load<f32>(0x10001ED0), b = load<f32>(0x10001ED4);
    store<f32>(0x101FF5D4, a);
    store<f32>(0x101FF5D8, b);
    call<void>(0x028ED6F8, at<void>(0x101FF5DC));
    call<void>(0x028F026C, at<void>(0x1018C798));
    call<void>(0x028EAB2C, at<void>(0x101FF5DD));
    call<void>(0x028F026C, at<void>(0x1018C7A4));
}
VERIFY(0x0200FD4C, c_lib_static_init_hd);
