#include "gabi.h"
using namespace gabi;
// CC0 zeldaret/tww JPADrawVisitor.cpp template. HD context owns its clipboard.
namespace {
static u32 ptr(u32 p,u32 off=0) { return load<u32>(p+off); }
static u32 vfn(u32 p,u32 slot) { return ptr(ptr(p),slot); }
static s32 quotient(s32 a,s32 b) { return !b || (a==INT32_MIN && b==-1) ? (a<0?-1:0) : a/b; }
static s32 remainder(s32 a,s32 b) { return (s32)((u32)a-(u32)quotient(a,b)*(u32)b); }

}
// 028363DC is the emitter draw callback visitor, not a calc visitor.
void JPADrawExecCallBack_exec(u32 self,u32 context) {
    WWHD_FUNC(0x028363DC,void,self,context);
    u32 emitter=ptr(context,0xC),callback=ptr(emitter,0x1E4);
    if(callback) tail_ptr<void>(vfn(callback,0x2C),callback,emitter);
}
VERIFY(0x028363DC,JPADrawExecCallBack_exec);
void JPADrawCalcScaleX_calc(u32 self,u32 context,u32 particle) {
    WWHD_FUNC(0x028363FC,void,self,context,particle);
    u32 block=ptr(context,0x14),target=vfn(block,0x44),clip=ptr(context);
    f32 timing=load<f32>(clip+0xEC),in=call_ptr<f32>(target,block);
    block=ptr(context,0x14);u32 vt=ptr(block);
    u32 dst=particle+0x9C;
    if(timing<in) {
        f32 rate=call_ptr<f32>(ptr(vt,0x84),block);
        target=ptr(vt,0x54);clip=ptr(context);
        timing=load<f32>(clip+0xEC);block=ptr(context,0x14);
        f32 product=fmuls_ppc(rate,timing);
        f32 value=call_ptr<f32>(target,block);
        store<f32>(dst,fmuls_ppc(load<f32>(particle+0x98),fadds_ppc(product,value)));
    } else {
        target=ptr(vt,0x4C);clip=ptr(context);
        f32 out=call_ptr<f32>(target,block);
        timing=load<f32>(clip+0xEC);
        if(timing>out) {
            block=ptr(context,0x14);vt=ptr(block);
            f32 rate=call_ptr<f32>(ptr(vt,0x94),block);
            target=ptr(vt,0x4C);block=ptr(context,0x14);clip=ptr(context);
            out=call_ptr<f32>(target,block);
            f32 value=fmadds(rate,load<f32>(clip+0xEC)-out,load<f32>(0x1017223C));
            store<f32>(dst,load<f32>(particle+0x98)*value);
        } else store<u32>(dst,load<u32>(particle+0x98));
    }

}
VERIFY(0x028363FC,JPADrawCalcScaleX_calc);
void JPADrawCalcScaleY_calc(u32 self,u32 context,u32 particle) {
    WWHD_FUNC(0x0283652C,void,self,context,particle);
    u32 block=ptr(context,0x14),target=vfn(block,0x44),clip=ptr(context);
    f32 timing=load<f32>(clip+0xEC),in=call_ptr<f32>(target,block);
    block=ptr(context,0x14);u32 vt=ptr(block);
    u32 dst=particle+0xA0;
    if(timing<in) {
        f32 rate=call_ptr<f32>(ptr(vt,0x8C),block);
        target=ptr(vt,0x5C);clip=ptr(context);
        timing=load<f32>(clip+0xEC);block=ptr(context,0x14);
        f32 product=fmuls_ppc(rate,timing);
        f32 value=call_ptr<f32>(target,block);
        store<f32>(dst,fmuls_ppc(load<f32>(particle+0x98),fadds_ppc(product,value)));
    } else {
        target=ptr(vt,0x4C);clip=ptr(context);
        f32 out=call_ptr<f32>(target,block);
        timing=load<f32>(clip+0xEC);
        if(timing>out) {
            block=ptr(context,0x14);vt=ptr(block);
            f32 rate=call_ptr<f32>(ptr(vt,0x9C),block);
            target=ptr(vt,0x4C);block=ptr(context,0x14);clip=ptr(context);
            out=call_ptr<f32>(target,block);
            f32 value=fmadds(rate,load<f32>(clip+0xEC)-out,load<f32>(0x1017223C));
            store<f32>(dst,load<f32>(particle+0x98)*value);
        } else store<u32>(dst,load<u32>(particle+0x98));
    }

}
VERIFY(0x0283652C,JPADrawCalcScaleY_calc);
void JPADrawCalcScaleXBySpeed_calc(u32 self,u32 context,u32 particle) {
    WWHD_FUNC(0x0283665C,void,self,context,particle);
    u32 block=ptr(context,0x14),target=vfn(block,0x44),clip=ptr(context);
    f32 vx=load<f32>(particle+0x34);f32 vy=load<f32>(particle+0x38);f32 vz=load<f32>(particle+0x3C);
    f32 timing=load<f32>(clip+0xEC),in=call_ptr<f32>(target,block);
    block=ptr(context,0x14);u32 vt=ptr(block);
    u32 dst=particle+0x9C;
    if(timing<in) {
        f32 rate=call_ptr<f32>(ptr(vt,0x84),block);
        target=ptr(vt,0x54);clip=ptr(context);
        timing=load<f32>(clip+0xEC);block=ptr(context,0x14);
        f32 product=fmuls_ppc(rate,timing);
        f32 value=call_ptr<f32>(target,block);
        store<f32>(dst,fmuls_ppc(load<f32>(particle+0x98),fadds_ppc(product,value)));
    } else {
        target=ptr(vt,0x4C);clip=ptr(context);
        f32 out=call_ptr<f32>(target,block);
        timing=load<f32>(clip+0xEC);
        if(timing>out) {
            block=ptr(context,0x14);vt=ptr(block);
            f32 rate=call_ptr<f32>(ptr(vt,0x94),block);
            target=ptr(vt,0x4C);block=ptr(context,0x14);clip=ptr(context);
            out=call_ptr<f32>(target,block);
            f32 value=fmadds(rate,load<f32>(clip+0xEC)-out,load<f32>(0x1017223C));
            store<f32>(dst,load<f32>(particle+0x98)*value);
        } else store<u32>(dst,load<u32>(particle+0x98));
    }

        f32 square=fmadds(vz,vz,fmadds(vx,vx,vy*vy));
        f32 length=(f32)call<f64>(0x028F37F0,square);
        f32 factor=length*load<f32>(0x101722A4);
        store<f32>(dst,load<f32>(dst)*factor);

}
VERIFY(0x0283665C,JPADrawCalcScaleXBySpeed_calc);
void JPADrawCalcScaleYBySpeed_calc(u32 self,u32 context,u32 particle) {
    WWHD_FUNC(0x02836864,void,self,context,particle);
    u32 block=ptr(context,0x14),target=vfn(block,0x44),clip=ptr(context);
    f32 vx=load<f32>(particle+0x34);f32 vy=load<f32>(particle+0x38);f32 vz=load<f32>(particle+0x3C);
    f32 timing=load<f32>(clip+0xEC),in=call_ptr<f32>(target,block);
    block=ptr(context,0x14);u32 vt=ptr(block);
    u32 dst=particle+0xA0;
    if(timing<in) {
        f32 rate=call_ptr<f32>(ptr(vt,0x8C),block);
        target=ptr(vt,0x5C);clip=ptr(context);
        timing=load<f32>(clip+0xEC);block=ptr(context,0x14);
        f32 product=fmuls_ppc(rate,timing);
        f32 value=call_ptr<f32>(target,block);
        store<f32>(dst,fmuls_ppc(load<f32>(particle+0x98),fadds_ppc(product,value)));
    } else {
        target=ptr(vt,0x4C);clip=ptr(context);
        f32 out=call_ptr<f32>(target,block);
        timing=load<f32>(clip+0xEC);
        if(timing>out) {
            block=ptr(context,0x14);vt=ptr(block);
            f32 rate=call_ptr<f32>(ptr(vt,0x9C),block);
            target=ptr(vt,0x4C);block=ptr(context,0x14);clip=ptr(context);
            out=call_ptr<f32>(target,block);
            f32 value=fmadds(rate,load<f32>(clip+0xEC)-out,load<f32>(0x1017223C));
            store<f32>(dst,load<f32>(particle+0x98)*value);
        } else store<u32>(dst,load<u32>(particle+0x98));
    }

        f32 square=fmadds(vz,vz,fmadds(vx,vx,vy*vy));
        f32 length=(f32)call<f64>(0x028F37F0,square);
        f32 factor=length*load<f32>(0x101722A4);
        store<f32>(dst,load<f32>(dst)*factor);

}
VERIFY(0x02836864,JPADrawCalcScaleYBySpeed_calc);
void JPADrawCalcScaleAnmTimingRepeatX_calc(u32 self,u32 context,u32 particle) {
    WWHD_FUNC(0x02836A88,void,self,context,particle);
    u32 block=ptr(context,0x14);s32 age=ftoi(load<f32>(particle+0x78));
    u32 vt=ptr(block),clip=ptr(context);
    s32 cycle=call_ptr<s32>(ptr(vt,0x74),block);
    s32 frame=remainder(age,cycle);
    u32 target=ptr(vt,0x74);block=ptr(context,0x14);
    f32 numerator=(f32)frame;
    cycle=call_ptr<s32>(target,block);
    store<f32>(clip+0xEC,numerator/(f32)cycle);
}
VERIFY(0x02836A88,JPADrawCalcScaleAnmTimingRepeatX_calc);
void JPADrawCalcScaleAnmTimingRepeatY_calc(u32 self,u32 context,u32 particle) {
    WWHD_FUNC(0x02836B74,void,self,context,particle);
    u32 block=ptr(context,0x14);s32 age=ftoi(load<f32>(particle+0x78));
    u32 vt=ptr(block),clip=ptr(context);
    s32 cycle=call_ptr<s32>(ptr(vt,0x7C),block);
    s32 frame=remainder(age,cycle);
    u32 target=ptr(vt,0x7C);block=ptr(context,0x14);
    f32 numerator=(f32)frame;
    cycle=call_ptr<s32>(target,block);
    store<f32>(clip+0xEC,numerator/(f32)cycle);
}
VERIFY(0x02836B74,JPADrawCalcScaleAnmTimingRepeatY_calc);
void JPADrawCalcScaleAnmTimingReverseX_calc(u32 self,u32 context,u32 particle) {
    WWHD_FUNC(0x02836C60,void,self,context,particle);
    u32 block=ptr(context,0x14);s32 age=ftoi(load<f32>(particle+0x78));
    s32 cycle=call_ptr<s32>(vfn(block,0x74),block);
    block=ptr(context,0x14);u32 vt=ptr(block),target=ptr(vt,0x74);
    f32 odd=(f32)((u32)quotient(age,cycle)&1);
    age=ftoi(load<f32>(particle+0x78));
    cycle=call_ptr<s32>(target,block);
    s32 frame=remainder(age,cycle);target=ptr(vt,0x74);block=ptr(context,0x14);
    f32 numerator=(f32)frame;
    cycle=call_ptr<s32>(target,block);
    f32 fraction=numerator/(f32)cycle;
    f32 result=odd+fnmsubs(odd+odd,fraction,fraction);
    store<f32>(ptr(context)+0xEC,result);
}
VERIFY(0x02836C60,JPADrawCalcScaleAnmTimingReverseX_calc);
void JPADrawCalcScaleAnmTimingReverseY_calc(u32 self,u32 context,u32 particle) {
    WWHD_FUNC(0x02836DC0,void,self,context,particle);
    u32 block=ptr(context,0x14);s32 age=ftoi(load<f32>(particle+0x78));
    s32 cycle=call_ptr<s32>(vfn(block,0x7C),block);
    block=ptr(context,0x14);u32 vt=ptr(block),target=ptr(vt,0x7C);
    f32 odd=(f32)((u32)quotient(age,cycle)&1);
    age=ftoi(load<f32>(particle+0x78));
    cycle=call_ptr<s32>(target,block);
    s32 frame=remainder(age,cycle);target=ptr(vt,0x7C);block=ptr(context,0x14);
    f32 numerator=(f32)frame;
    cycle=call_ptr<s32>(target,block);
    f32 fraction=numerator/(f32)cycle;
    f32 result=odd+fnmsubs(odd+odd,fraction,fraction);
    store<f32>(ptr(context)+0xEC,result);
}
VERIFY(0x02836DC0,JPADrawCalcScaleAnmTimingReverseY_calc);
void JPADrawCalcScaleCopyX2Y_calc(u32 self,u32 context,u32 particle) {
    WWHD_FUNC(0x02836A6C,void,self,context,particle);
    store<u32>(particle+0xA0,load<u32>(particle+0x9C));
}
VERIFY(0x02836A6C,JPADrawCalcScaleCopyX2Y_calc);
void JPADrawCalcScaleAnmTimingNormal_calc(u32 self,u32 context,u32 particle) {
    WWHD_FUNC(0x02836A78,void,self,context,particle);
    u32 clip=ptr(context);store<u32>(clip+0xEC,load<u32>(particle+0x80));
}
VERIFY(0x02836A78,JPADrawCalcScaleAnmTimingNormal_calc);
void JPADrawCalcColorPrm_calc(u32 self,u32 context) {
    WWHD_FUNC(0x02836F20,void,self,context);
    u32 block=ptr(context,0x10),target=vfn(block,0x174),clip=ptr(context);
    s32 frame=load<s16>(clip+0xF0);u32 draw=ptr(context,0x20);
    u32 color=call_ptr<u32>(target,block,frame);store<u32>(draw+0xE8,color);
}
VERIFY(0x02836F20,JPADrawCalcColorPrm_calc);
void JPADrawCalcColorEnv_calc(u32 self,u32 context) {
    WWHD_FUNC(0x02836F7C,void,self,context);
    u32 block=ptr(context,0x10),target=vfn(block,0x184),clip=ptr(context);
    s32 frame=load<s16>(clip+0xF0);u32 draw=ptr(context,0x20);
    u32 color=call_ptr<u32>(target,block,frame);store<u32>(draw+0xEC,color);
}
VERIFY(0x02836F7C,JPADrawCalcColorEnv_calc);
void JPADrawCalcColorCopyFromEmitter_calc(u32 self,u32 context,u32 particle) {
    WWHD_FUNC(0x02836FD8,void,self,context,particle);
    store<u32>(particle+0xB8,load<u32>(ptr(context,0x20)+0xE8));
    store<u32>(particle+0xBC,load<u32>(ptr(context,0x20)+0xEC));
}
VERIFY(0x02836FD8,JPADrawCalcColorCopyFromEmitter_calc);
void JPADrawCalcColorAnmFrameNormal_calc(u32 self,u32 context) {
    WWHD_FUNC(0x02837004,void,self,context);
    u32 block=ptr(context,0x10),emitter=ptr(context,0xC),target=vfn(block,0x194);
    s32 tick=ftoi(load<f32>(emitter+0x194));
    s32 max=call_ptr<s32>(target,block);
    if(tick>=max) { block=ptr(context,0x10);tick=call_ptr<s32>(vfn(block,0x194),block); }
    store<u16>(ptr(context)+0xF0,(u16)tick);
}
VERIFY(0x02837004,JPADrawCalcColorAnmFrameNormal_calc);
void JPADrawCalcColorAnmFrameRepeat_calc(u32 self,u32 context) {
    WWHD_FUNC(0x02837088,void,self,context);
    f32 boundary=load<f32>(0x101722A8),tick=load<f32>(ptr(context,0xC)+0x194);
    u32 age=tick<boundary?(u32)ftoi(tick):(u32)ftoi(tick-boundary)+0x80000000u;
    u32 block=ptr(context,0x10),target=vfn(block,0x194),clip=ptr(context);
    u32 divisor=(u32)call_ptr<s32>(target,block)+1;
    u32 q=divisor?age/divisor:0;
    store<u16>(clip+0xF0,(u16)(age-q*divisor));
}
VERIFY(0x02837088,JPADrawCalcColorAnmFrameRepeat_calc);
void JPADrawCalcColorAnmFrameReverse_calc(u32 self,u32 context) {
    WWHD_FUNC(0x02837150,void,self,context);
    u32 block=ptr(context,0x10),emitter=ptr(context,0xC),target=vfn(block,0x194);
    s32 age=ftoi(load<f32>(emitter+0x194)),cycle=call_ptr<s32>(target,block);
    u32 q=(u32)quotient(age,cycle),odd=q&1,frame=(u32)age-q*(u32)cycle;
    store<u16>(ptr(context)+0xF0,(u16)(frame+odd*(u32)cycle-2*odd*frame));
}
VERIFY(0x02837150,JPADrawCalcColorAnmFrameReverse_calc);
void JPADrawCalcColorAnmFrameMerge_calc(u32 self,u32 context) {
    WWHD_FUNC(0x028371D8,void,self,context);store<u16>(ptr(context)+0xF0,0);
}
VERIFY(0x028371D8,JPADrawCalcColorAnmFrameMerge_calc);
void JPADrawCalcColorAnmFrameRandom_calc(u32 self,u32 context) {
    WWHD_FUNC(0x028371E8,void,self,context);store<u16>(ptr(context)+0xF0,0);
}
VERIFY(0x028371E8,JPADrawCalcColorAnmFrameRandom_calc);
