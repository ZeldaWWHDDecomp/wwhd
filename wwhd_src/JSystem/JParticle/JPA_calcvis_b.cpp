#include "gabi.h"
using namespace gabi;
// CC0 zeldaret/tww JPADrawVisitor.cpp templates; HD draw parameters are inline
// in the particle at +0x8C. Virtual entries use the GHS eight-byte format.
namespace {
static u32 block(u32 context,u32 offset) { return load<u32>(context+offset); }
static u32 vtable(u32 object) { return load<u32>(object); }
static f32 extra(u32 context,u32 slot) {
    u32 object=block(context,0x14),target=load<u32>(vtable(object)+slot);
    return call_ptr<f32>(target,object);
}
static f32 sine(f32 angle) {
    u32 index=(u16)ftoi(angle);
    return load<f32>(0x104A44F8+((index>>3)<<3));
}
static f32 phase(u32 particle) {
    s32 shifted=(s32)((u32)ftoi(load<f32>(particle+0x78))<<14);
    return (f32)shifted*load<f32>(particle+0xB0);
}
static void alpha(u32 particle,f32 factor) {
    f32 value=load<f32>(particle+0xAC)*factor,zero=load<f32>(0x10172238);
    store<f32>(particle+0xAC,value);
    // Original bge skips the clamp when less-than is clear, including unordered.
    if(value<zero) store<f32>(particle+0xAC,zero);
}
static s32 divide(s32 a,s32 b) {
    if(!b || (a==INT32_MIN && b==-1)) return 0;
    return a/b;
}
static void texture(u32 context,u32 index) {
    u32 table=block(context,0x2C),draw=block(context,0x20);
    store<u16>(draw+0xF0,load<u16>(table+(index<<1)));
}
}
void JPADrawCalcAlpha_calc(u32 self,u32 context,u32 particle) {
    WWHD_FUNC(0x028371F8,void,self,context,particle);
    u32 object=block(context,0x14),target=load<u32>(vtable(object)+0xCC);
    f32 time=load<f32>(particle+0x80);
    f32 inTime=call_ptr<f32>(target,object);
    object=block(context,0x14);u32 vt=vtable(object);
    if(time<inTime) {
        f32 rate=call_ptr<f32>(load<u32>(vt+0x114),object);
        target=load<u32>(vt+0xDC);object=block(context,0x14);
        f32 scaled=rate*time;
        f32 value=call_ptr<f32>(target,object);
        store<f32>(particle+0xAC,scaled+value);
    } else {
        f32 outTime=call_ptr<f32>(load<u32>(vt+0xD4),object);
        object=block(context,0x14);vt=vtable(object);
        if(time>outTime) {
            f32 rate=call_ptr<f32>(load<u32>(vt+0x11C),object);
            target=load<u32>(vt+0xD4);object=block(context,0x14);
            f32 end=call_ptr<f32>(target,object);
            target=load<u32>(vt+0xE4);object=block(context,0x14);
            f32 delta=time-end;
            f32 base=call_ptr<f32>(target,object);
            store<f32>(particle+0xAC,fmadds(rate,delta,base));
        } else store<f32>(particle+0xAC,call_ptr<f32>(load<u32>(vt+0xE4),object));
    }
}
VERIFY(0x028371F8,JPADrawCalcAlpha_calc);
void JPADrawCalcAlphaFlickNrmSin_calc(u32 self,u32 context,u32 particle) {
    WWHD_FUNC(0x02837320,void,self,context,particle);
    f32 theta=phase(particle),one=load<f32>(0x1017223C);
    f32 param1=extra(context,0xF4);
    f32 wave=sine(theta*(one-param1));
    f32 param3=extra(context,0x104);
    f32 half=load<f32>(0x10172254);
    f32 amplitude=((wave-one)*half)*param3;
    alpha(particle,fmadds(amplitude,load<f32>(particle+0xB0),one));
}
VERIFY(0x02837320,JPADrawCalcAlphaFlickNrmSin_calc);
void JPADrawCalcAlphaFlickAddSin_calc(u32 self,u32 context,u32 particle) {
    WWHD_FUNC(0x02837468,void,self,context,particle);
    f32 theta=phase(particle),one=load<f32>(0x1017223C);
    f32 param1=extra(context,0xF4),wave1=sine(theta*(one-param1));
    f32 param2=extra(context,0xFC),wave2=sine(theta*(one-param2));
    f32 param3=extra(context,0x104),two=load<f32>(0x101722AC),half=load<f32>(0x10172254);
    f32 amplitude=(((wave1+wave2)-two)*half)*param3;
    alpha(particle,fmadds(amplitude,load<f32>(particle+0xB0),two)*half);
}
VERIFY(0x02837468,JPADrawCalcAlphaFlickAddSin_calc);
void JPADrawCalcAlphaFlickMultSin_calc(u32 self,u32 context,u32 particle) {
    WWHD_FUNC(0x02837614,void,self,context,particle);
    f32 theta=phase(particle),param3=extra(context,0x104);
    f32 half=load<f32>(0x10172254);
    f32 amplitude=(half*param3)*load<f32>(particle+0xB0),one=load<f32>(0x1017223C);
    f32 param1=extra(context,0xF4),wave1=sine(theta*(one-param1));
    f32 param2=extra(context,0xFC),wave2=sine(theta*(one-param2));
    alpha(particle,fmadds(wave1-one,amplitude,one)*fmadds(wave2-one,amplitude,one));
}
VERIFY(0x02837614,JPADrawCalcAlphaFlickMultSin_calc);
void JPADrawCalcTextureAnmIndexNormal_calc(u32 self,u32 context) {
    WWHD_FUNC(0x028377C8,void,self,context);
    u32 object=block(context,0x10),emitter=block(context,0xC),vt=vtable(object);
    s32 tick=ftoi(load<f32>(emitter+0x194));
    s32 count=call_ptr<s32>(load<u32>(vt+0x134),object);
    if((s32)((u32)count-1)<tick) {
        object=block(context,0x10);
        count=call_ptr<s32>(load<u32>(vtable(object)+0x134),object);
        tick=(s32)((u32)count-1);
    }
    u32 target=load<u32>(vt+0x144);object=block(context,0x10);
    texture(context,call_ptr<u32>(target,object,(u8)tick));
}
VERIFY(0x028377C8,JPADrawCalcTextureAnmIndexNormal_calc);
void JPADrawCalcTextureAnmIndexRepeat_calc(u32 self,u32 context) {
    WWHD_FUNC(0x0283787C,void,self,context);
    u32 emitter=block(context,0xC),object=block(context,0x10),vt=vtable(object);
    s32 tick=ftoi(load<f32>(emitter+0x194));
    s32 count=call_ptr<s32>(load<u32>(vt+0x134),object),q=divide(tick,count);
    u32 target=load<u32>(vt+0x144),frame=(u32)tick-(u32)q*(u32)count;
    texture(context,call_ptr<u32>(target,object,(u8)frame));
}
VERIFY(0x0283787C,JPADrawCalcTextureAnmIndexRepeat_calc);
void JPADrawCalcTextureAnmIndexReverse_calc(u32 self,u32 context) {
    WWHD_FUNC(0x02837920,void,self,context);
    u32 object=block(context,0x10),emitter=block(context,0xC),vt=vtable(object);
    s32 tick=ftoi(load<f32>(emitter+0x194));
    s32 count=call_ptr<s32>(load<u32>(vt+0x134),object);
    s32 maxFrame=(s32)((u32)count-1),q=divide(tick,maxFrame);
    object=block(context,0x10);
    u32 rem=(u32)tick-(u32)q*(u32)maxFrame,odd=(u32)q&1;
    u32 target=load<u32>(vtable(object)+0x144),frame=rem+odd*(u32)maxFrame-2*(odd*rem);
    texture(context,call_ptr<u32>(target,object,(u8)frame));
}
VERIFY(0x02837920,JPADrawCalcTextureAnmIndexReverse_calc);
void JPADrawCalcTextureAnmIndexMerge_calc(u32 self,u32 context) {
    WWHD_FUNC(0x028379D0,void,self,context);
    u32 object=block(context,0x10);
    u32 index=call_ptr<u32>(load<u32>(vtable(object)+0x13C),object);
    store<u16>(block(context,0x20)+0xF0,(u16)index);
}
VERIFY(0x028379D0,JPADrawCalcTextureAnmIndexMerge_calc);
void JPADrawCalcTextureAnmIndexRandom_calc(u32 self,u32 context) {
    WWHD_FUNC(0x02837A14,void,self,context);
    u32 object=block(context,0x10);
    u32 index=call_ptr<u32>(load<u32>(vtable(object)+0x13C),object);
    store<u16>(block(context,0x20)+0xF0,(u16)index);
}
VERIFY(0x02837A14,JPADrawCalcTextureAnmIndexRandom_calc);
void JPADrawCalcChildAlphaOut_calc(u32 self,u32 context,u32 particle) {
    WWHD_FUNC(0x02837A58,void,self,context,particle);
    store<f32>(particle+0xAC,load<f32>(0x1017223C)-load<f32>(particle+0x80));
}
VERIFY(0x02837A58,JPADrawCalcChildAlphaOut_calc);
void JPADrawCalcChildScaleOut_calc(u32 self,u32 context,u32 particle) {
    WWHD_FUNC(0x02837A70,void,self,context,particle);
    f32 time=load<f32>(particle+0x80),one=load<f32>(0x1017223C),scale=load<f32>(particle+0x98);
    f32 fade=one-time,random=load<f32>(particle+0xB0);
    store<f32>(particle+0x9C,scale*fade);store<f32>(particle+0xA0,random*fade);
}
VERIFY(0x02837A70,JPADrawCalcChildScaleOut_calc);
