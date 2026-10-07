#include "gabi.h"
using namespace gabi;
// CC0 tww JPAField.cpp template, adjusted to the HD field layout (-4).
namespace {
struct Vec { f32 x,y,z; };
struct GuestVec { be<f32> x,y,z; };
struct GuestMtx { be<f32> v[12]; };
static Vec readVec(u32 a) { return {load<f32>(a),load<f32>(a+4),load<f32>(a+8)}; }
static void writeVec(u32 a,Vec v) { store<f32>(a,v.x);store<f32>(a+4,v.y);store<f32>(a+8,v.z); }
static f32 square(Vec v) { return fmadds(v.z,v.z,fmadds(v.x,v.x,v.y*v.y)); }
static f32 dot(Vec a,Vec b) { return fmadds(a.z,b.z,fmadds(a.x,b.x,a.y*b.y)); }
static Vec scale(Vec v,f32 s) { return {v.x*s,v.y*s,v.z*s}; }
static Vec cross(Vec a,Vec b) { return {fmsubs(a.y,b.z,a.z*b.y),fmsubs(a.z,b.x,a.x*b.z),fmsubs(a.x,b.y,a.y*b.x)}; }
static f32 root(f32 s) { return (f32)call<f64>(0x028F37F0,s); }
// The binary BLE also skips the normalisation path for unordered values.
static void normalizeAt(u32 a,f32 epsilon,f32 one) {
    f32 s=square(readVec(a));
    if (s>epsilon) { f32 inv=one/root(s);Vec v=readVec(a);writeVec(a,scale(v,inv)); }
}
static void basePre(u32 self,u32 data) { call<void>(0x02822C38,self,data); }
static void baseVel(u32 self,u32 data,u32 particle) { call<void>(0x02822B54,self,data,particle); }
static void transform(u32 src,u32 dst) { u32 info=load<u32>(0x104B5948);call<void>(0x028E8F64,info+0x38,src,dst); }
static f32 randomFloat() {
    u32 info=load<u32>(0x104B5948),emitter=load<u32>(info+4);
    u32 seed=load<u32>(emitter+0x1EC)*0x19660D+0x3C6EF35F;
    store<u32>(emitter+0x1EC,seed);
    return std::bit_cast<f32>((seed>>9)|0x3F800000);
}
}
void JPAVortexField_preCalc(u32 self,u32 data) {
    WWHD_FUNC(0x0282356C,void,self,data);
    basePre(self,data);transform(data+0x5C,data+0x20);
    f32 epsilon=load<f32>(0x10171084),one=load<f32>(0x10171078);
    normalizeAt(data+0x20,epsilon,one);
    f32 z=load<f32>(data+0x58);f32 s=z*z;
    store<f32>(data+0x74,s);store<f32>(data+0x78,one/s);
}
VERIFY(0x0282356C,JPAVortexField_preCalc);
void JPAVortexField_calc(u32 self,u32 data,u32 particle) {
    WWHD_FUNC(0x02823640,void,self,data,particle);
    Vec axis=readVec(data+0x20),pos=readVec(particle+0x1C);
    f32 d=dot(axis,pos);
    Vec force={fnmsubs(axis.x,d,pos.x),fnmsubs(axis.y,d,pos.y),fnmsubs(axis.z,d,pos.z)};
    f32 sq=square(force),power=sq,limit=load<f32>(data+0x74);
    if (power>limit) power=limit;
    f32 temp=power*load<f32>(data+0x78),one=load<f32>(0x10171078);
    f32 speed=fmadds(one-temp,load<f32>(data+0x68),temp*load<f32>(data+0x6C));
    f32 epsilon=load<f32>(0x10171084);
    if (sq>epsilon) { force=scale(force,one/root(sq));axis=readVec(data+0x20); }
    Vec vel=scale(cross(force,axis),speed);writeVec(data+0x14,vel);
    baseVel(self,data,particle);
}
VERIFY(0x02823640,JPAVortexField_calc);
u32 JPAVortexField_isItinRange(u32 self,u32 data,f32 distance) {
    WWHD_FUNC(0x02824508,u32,self,data,distance);return 1;
}
VERIFY(0x02824508,JPAVortexField_isItinRange);
void JPAConvectionField_preCalc(u32 self,u32 data) {
    WWHD_FUNC(0x028237D4,void,self,data);
    FrameLocal<GuestVec> axis(8);
    basePre(self,data);
    Vec direction=readVec(data+0x5C),position=readVec(data+0x50);
    Vec a=cross(position,direction),p=cross(a,direction);
    writeVec(axis.a,a);writeVec(data+0x50,p);
    f32 epsilon=load<f32>(0x10171084),one=load<f32>(0x10171078);
    normalizeAt(data+0x50,epsilon,one);
    transform(data+0x50,data+0x20);transform(data+0x5C,data+0x2C);transform(axis.a,data+0x38);
    normalizeAt(data+0x20,epsilon,one);normalizeAt(data+0x2C,epsilon,one);normalizeAt(data+0x38,epsilon,one);
}
VERIFY(0x028237D4,JPAConvectionField_preCalc);
void JPAConvectionField_calc(u32 self,u32 data,u32 particle) {
    WWHD_FUNC(0x02823A28,void,self,data,particle);
    Vec pos=readVec(particle+0x1C),a=readVec(data+0x20),b=readVec(data+0x38);
    f32 da=dot(a,pos),db=dot(b,pos);
    Vec n={fmadds(b.x,db,a.x*da),fmadds(b.y,db,a.y*da),fmadds(b.z,db,a.z*da)};
    f32 length=load<f32>(data+0x74),epsilon=load<f32>(0x10171084),zero=load<f32>(0x1017107C),one=load<f32>(0x10171078);
    f32 s=square(n);Vec n2;
    if(s>epsilon) { f32 k=(one/root(s))*length;n2=scale(n,k);pos=readVec(particle+0x1C); }
    else n2={zero,zero,zero};
    Vec delta={pos.x-n2.x,pos.y-n2.y,pos.z-n2.z};
    Vec axis=cross(readVec(data+0x2C),n2),vel=cross(axis,delta);
    writeVec(data+0x14,vel);f32 mag=load<f32>(data+0x68);s=square(vel);
    if(s>epsilon) { f32 k=(one/root(s))*mag;writeVec(data+0x14,scale(readVec(data+0x14),k)); }
    f32 extra=load<f32>(data+0x78);
    if(extra!=zero) {
        s=square(delta);if(s>epsilon) delta=scale(delta,(one/root(s))*extra);
        vel=readVec(data+0x14);writeVec(data+0x14,{vel.x+delta.x,vel.y+delta.y,vel.z+delta.z});
    }
    baseVel(self,data,particle);
}
VERIFY(0x02823A28,JPAConvectionField_calc);
u32 JPAConvectionField_isItinRange(u32 self,u32 data,f32 distance) {
    WWHD_FUNC(0x02824510,u32,self,data,distance);return 1;
}
VERIFY(0x02824510,JPAConvectionField_isItinRange);
void JPARandomField_calc(u32 self,u32 data,u32 particle) {
    WWHD_FUNC(0x02823CDC,void,self,data,particle);
    s32 age=ftoi(load<f32>(particle+0x78));
    if(age!=0) {u32 cycle=load<u8>(data+0x95);if(!cycle||age%(s32)cycle!=0)return;}
    f32 x=randomFloat(),y=randomFloat(),z=randomFloat();
    f32 one=load<f32>(0x10171078),half=load<f32>(0x1017108C),mag=load<f32>(data+0x68);
    writeVec(data+0x14,{((x-one)-half)*mag,((y-one)-half)*mag,((z-one)-half)*mag});
    baseVel(self,data,particle);
}
VERIFY(0x02823CDC,JPARandomField_calc);
void JPADragField_init(u32 self,u32 data,u32 particle) {
    WWHD_FUNC(0x02823E14,void,self,data,particle);
    f32 random=randomFloat(),one=load<f32>(0x10171078),half=load<f32>(0x1017108C);
    f32 drag=fmadds(load<f32>(data+0x6C),(random-one)-half,load<f32>(data+0x68));
    if(drag>one)drag=one;store<f32>(particle+0x84,drag);
}
VERIFY(0x02823E14,JPADragField_init);
void JPADragField_calc(u32 self,u32 data,u32 particle) {
    WWHD_FUNC(0x02823E8C,void,self,data,particle);
    if(load<u32>(particle+0xCC)&4) {
        f32 drag=load<f32>(particle+0x88),fieldDrag=load<f32>(particle+0x84);store<f32>(particle+0x88,drag*fieldDrag);
    } else {
        f32 affect=call<f32>(0x02822AC0,self,data,load<f32>(particle+0x80));
        f32 one=load<f32>(0x10171078),fieldDrag=load<f32>(particle+0x84);
        f32 factor=fnmsubs(affect,one-fieldDrag,one),drag=load<f32>(particle+0x88);
        store<f32>(particle+0x88,drag*factor);
    }
}
VERIFY(0x02823E8C,JPADragField_calc);
void JPASpinField_preCalc(u32 self,u32 data) {
    WWHD_FUNC(0x02823EF0,void,self,data);
    FrameLocal<GuestMtx> matrix(8);
    basePre(self,data);transform(data+0x5C,data+0x50);
    f32 sq=square(readVec(data+0x50)),epsilon=load<f32>(0x10171084);
    if(sq>epsilon) {
        f32 r=root(sq),one=load<f32>(0x10171078);writeVec(data+0x50,scale(readVec(data+0x50),one/r));
    }
    call<void>(0x028E9838,matrix.a,data+0x50,load<f32>(data+0x68));
    // Pure FPR copies preserve the original single-precision bit patterns.
    for(u32 column=0;column<3;++column)for(u32 row=0;row<3;++row)
        store<u32>(data+0x20+column*12+row*4,load<u32>(matrix.a+row*16+column*4));
}
VERIFY(0x02823EF0,JPASpinField_preCalc);
void JPASpinField_calc(u32 self,u32 data,u32 particle) {
    WWHD_FUNC(0x02823FF0,void,self,data,particle);
    FrameLocal<GuestMtx> matrix(8);FrameLocal<GuestVec> position(0x38);
    for(u32 column=0;column<3;++column)for(u32 row=0;row<3;++row)
        store<u32>(matrix.a+row*16+column*4,load<u32>(data+0x20+column*12+row*4));
    u32 zero=load<u32>(0x1017107C);
    store<u32>(matrix.a+12,zero);store<u32>(matrix.a+28,zero);store<u32>(matrix.a+44,zero);
    call<void>(0x028E9044,matrix.a,particle+0x1C,position.a);
    Vec next=readVec(position.a),old=readVec(particle+0x1C);writeVec(data+0x14,{next.x-old.x,next.y-old.y,next.z-old.z});
    baseVel(self,data,particle);
}
VERIFY(0x02823FF0,JPASpinField_calc);
u32 JPASpinField_isItinRange(u32 self,u32 data,f32 distance) {
    WWHD_FUNC(0x02824518,u32,self,data,distance);return 1;
}
VERIFY(0x02824518,JPASpinField_isItinRange);
void JPAFieldManager_init(u32 self,u32 particle) {
    WWHD_FUNC(0x02824230,void,self,particle);
    for(u32 link=load<u32>(self);link;link=load<u32>(link+12)) {
        u32 data=load<u32>(link),field=load<u32>(data),vt=load<u32>(field),fn=load<u32>(vt+0x14);
        call_ptr<void>(fn,field,data,particle);
    }
}
VERIFY(0x02824230,JPAFieldManager_init);
void JPAFieldManager_preCalc(u32 self) {
    WWHD_FUNC(0x02824294,void,self);
    for(u32 link=load<u32>(self);link;link=load<u32>(link+12)) {
        u32 data=load<u32>(link),field=load<u32>(data),vt=load<u32>(field),fn=load<u32>(vt+0x1C);
        call_ptr<void>(fn,field,data);
    }
}
VERIFY(0x02824294,JPAFieldManager_preCalc);
void JPAFieldManager_calc(u32 self,u32 particle) {
    WWHD_FUNC(0x028242E8,void,self,particle);
    for(u32 link=load<u32>(self);link;link=load<u32>(link+12)) {
        u32 data=load<u32>(link);
        if(load<u16>(data+0x90)&0x80) {
            Vec pos=readVec(particle+0x28),origin=readVec(data+0x50);
            u32 field=load<u32>(data),vt=load<u32>(field),fn=load<u32>(vt+0x2C);
            Vec delta={pos.x-origin.x,pos.y-origin.y,pos.z-origin.z};
            if(!call_ptr<u32>(fn,field,data,square(delta)))continue;
        }
        u32 field=load<u32>(data),vt=load<u32>(field),fn=load<u32>(vt+0x24);
        call_ptr<void>(fn,field,data,particle);
    }
}
VERIFY(0x028242E8,JPAFieldManager_calc);
