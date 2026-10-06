// WWHD collision-data primitives; see wwhd_src/README.md.
#include "gabi.h"
using namespace gabi;
struct CcVector { be<f32> xyz[3]; };
struct CcTriangle { u8 bytes[56]; };
static f32 F(u32 p,u32 n=0) { return load<f32>(p+n); }
static void S(u32 p,u32 n,f32 v) { store<f32>(p+n,v); }
static void copyVector(u32 d,u32 p) {
    u32 x=load<u32>(p),y=load<u32>(p+4),z=load<u32>(p+8);
    store<u32>(d,x);store<u32>(d+4,y);store<u32>(d+8,z);
}
static void floatVector(u32 d,u32 p) {
    f32 x=F(p),y=F(p,4),z=F(p,8);S(d,0,x);S(d,4,y);S(d,8,z);
}
// Espresso slw uses the low six shift bits, and returns zero when bit five is set.
static u32 shiftOne(u32 n) { return (n&32) ? 0u : (1u<<(n&31)); }
static u32 divideAxis(u32 p,u32 b,u32 axis,u32 last,bool rejectOutside) {
    u32 base=axis*4, flag=0x1Cu+axis*12;
    if (load<u8>(p+flag)) return (1u<<(last+1))-1;
    f32 origin=F(p,base), low=F(b,base), high=F(b,base+12), inv=F(p,flag+8);
    s32 lo=ftoi((low-origin)*inv),hi=ftoi((high-origin)*inv);
    if (rejectOutside && ((lo<0 && hi<0) || (lo>(s32)last && hi>(s32)last))) return 0;
    if (hi>(s32)last) hi=(s32)last;
    u32 mask=shiftOne((u32)hi+1u)-1u;
    if (lo>0) mask &= ~(shiftOne((u32)lo-1u)-1u);
    return mask;
}
// Shape attributes contain a geometry object at +0x20; its triangle base
// copies normal/d through floating loads and vertices through integer loads.
static void makeTriangle(u32 d,u32 p) {
    for (u32 i=0;i<4;i++) S(d,i*4,F(p,0x20+i*4));
    store<u32>(d+16,0x1000136Cu);
    for (u32 i=0;i<9;i++) store<u32>(d+20+i*4,load<u32>(p+0x34+i*4));
}
u32 cc_0200B56C(u32 p) {
    WWHD_FUNC(0x0200B56C,u32,p);
    if (!p) p=call<u32>(0x0273AD10,24u);
    if (p) {
        store<u32>(p+20,0x100011DCu);call<void>(0x020189A0,p);
    }
    return p;
}
VERIFY(0x0200B56C,cc_0200B56C);

u32 cc_0200B5C0(u32 p) {
    WWHD_FUNC(0x0200B5C0,u32,p);
    if (!p) p=call<u32>(0x0273AD10,28u);
    if (p) {
        store<u32>(p+24,0x100011FCu);
    }
    return p;
}
VERIFY(0x0200B5C0,cc_0200B5C0);

void cc_0200B600(u32 p,u32 flags) {
    WWHD_FUNC(0x0200B600,void,p,flags);
    if (!p) return;
    store<u32>(p+56,0x10001718u);call<void>(0x0201819C,p+32,0u);
    if (flags&1u) call<void>(0x0273AF40,p);
}
VERIFY(0x0200B600,cc_0200B600);

void cc_0200B660(u32 p,u32 flags) {
    WWHD_FUNC(0x0200B660,void,p,flags);
    if (!p) return;
    store<u32>(p+48,0x10001728u);
    if (flags&1u) call<void>(0x0273AF40,p);
}
VERIFY(0x0200B660,cc_0200B660);

void cc_0200B680(u32 p,u32 flags) {
    WWHD_FUNC(0x0200B680,void,p,flags);
    if (!p) return;
    store<u32>(p+52,0x100017D8u);
    if (flags&1u) call<void>(0x0273AF40,p);
}
VERIFY(0x0200B680,cc_0200B680);

void cc_0200B6A0(u32 p,u32 flags) {
    WWHD_FUNC(0x0200B6A0,void,p,flags);
    if (!p) return;
    store<u32>(p+48,0x100017E8u);
    if (flags&1u) call<void>(0x0273AF40,p);
}
VERIFY(0x0200B6A0,cc_0200B6A0);

u32 cc_0200B6C0(u32 p) {
    WWHD_FUNC(0x0200B6C0,u32,p);
    if (!p) p=call<u32>(0x0273AD10,8u);
    if (p) {
        store<u32>(p,0u);store<u32>(p+4,0x10001520u);
    }
    return p;
}
VERIFY(0x0200B6C0,cc_0200B6C0);

void cc_0200B708(u32 p,u32 flags) {
    WWHD_FUNC(0x0200B708,void,p,flags);
    if (!p) return;
    if (flags&1u) call<void>(0x0273AF40,p);
}
VERIFY(0x0200B708,cc_0200B708);

void cc_0200B7C0(u32 p,u32 flags) {
    WWHD_FUNC(0x0200B7C0,void,p,flags);
    if (!p) return;
    if (flags&1u) call<void>(0x0273AF40,p);
}
VERIFY(0x0200B7C0,cc_0200B7C0);

void cc_0200BD18(u32 p,u32 flags) {
    WWHD_FUNC(0x0200BD18,void,p,flags);
    if (!p) return;
    if (flags&1u) call<void>(0x0273AF40,p);
}
VERIFY(0x0200BD18,cc_0200BD18);

void cc_0200BD88(u32 p,u32 flags) {
    WWHD_FUNC(0x0200BD88,void,p,flags);
    if (!p) return;
    if (flags&1u) call<void>(0x0273AF40,p);
}
VERIFY(0x0200BD88,cc_0200BD88);

void cc_0200C110(u32 p,u32 flags) {
    WWHD_FUNC(0x0200C110,void,p,flags);
    if (!p) return;
    if (flags&1u) call<void>(0x0273AF40,p);
}
VERIFY(0x0200C110,cc_0200C110);

u32 cCcD_DivideInfo_Chk(u32 p,u32 q) {
    WWHD_FUNC(0x0200B71C,u32,p,q);
    u32 m=load<u32>(p)&load<u32>(q);
    return (m&0x7FFu) && (m&0xFFE00000u) && (m&0x1FF800u);
}
VERIFY(0x0200B71C,cCcD_DivideInfo_Chk);

u32 cc_0200B750(u32 p) {
    WWHD_FUNC(0x0200B750,u32,p);
    if (!p) p=call<u32>(0x0273AD10,64u);
    if (p) {
        f32 z=F(0x1000137Cu);
        store<u8>(p+28,0);S(p,44,z);store<u32>(p+24,0x100017F8u);S(p,36,z);
        store<u8>(p+40,0);S(p,48,z);store<u8>(p+52,0);S(p,32,z);S(p,60,z);S(p,56,z);
    }
    return p;
}
VERIFY(0x0200B750,cc_0200B750);

void cCcD_DivideArea_SetArea(u32 p,u32 box) {
    WWHD_FUNC(0x0200B7D4,void,p,box);
    call<void>(0x02017D08,p,box,box+12);
    f32 x=(F(p,12)-F(p))*F(0x10001380u),eps=F(0x100030B8u),one=F(0x10001384u);
    S(p,32,x);store<u8>(p+28,std::fabs(x)<eps);
    if (!(std::fabs(x)<eps)) S(p,36,one/F(p,32));
    f32 y=(F(p,16)-F(p,4))*F(0x10001388u);
    S(p,44,y);store<u8>(p+40,std::fabs(y)<eps);
    if (!(std::fabs(y)<eps)) S(p,48,one/F(p,44));
    f32 z=(F(p,20)-F(p,8))*F(0x10001380u);
    S(p,56,z);store<u8>(p+52,std::fabs(z)<eps);
    if (!(std::fabs(z)<eps)) S(p,60,one/F(p,56));
}
VERIFY(0x0200B7D4,cCcD_DivideArea_SetArea);

void cCcD_DivideArea_CalcDivideInfo(u32 p,u32 out,u32 box,u32 all) {
    WWHD_FUNC(0x0200B8CC,void,p,out,box,all);
    if (all) {store<u32>(out,0xFFFFFFFFu);return;}
    u32 m=divideAxis(p,box,0,10,false);
    m|=divideAxis(p,box,1,9,false)<<11;
    m|=divideAxis(p,box,2,10,false)<<21;
    store<u32>(out,m);
}
VERIFY(0x0200B8CC,cCcD_DivideArea_CalcDivideInfo);

void cCcD_DivideArea_CalcDivideInfoOverArea(u32 p,u32 out,u32 box) {
    WWHD_FUNC(0x0200BAC0,void,p,out,box);
    u32 m=divideAxis(p,box,0,10,true);
    m|=divideAxis(p,box,1,9,true)<<11;
    m|=divideAxis(p,box,2,10,true)<<21;
    store<u32>(out,m);
}
VERIFY(0x0200BAC0,cCcD_DivideArea_CalcDivideInfoOverArea);

u32 cc_0200BD2C(u32 p) {
    WWHD_FUNC(0x0200BD2C,u32,p);
    if (!p) p=call<u32>(0x0273AD10,28u);
    if (p) {
        store<u32>(p+16,0u);store<u32>(p+24,0x10001530u);store<u8>(p+23,0);store<u32>(p+12,0u);store<u8>(p+21,0);store<u8>(p+22,0);store<u8>(p+20,0);
    }
    return p;
}
VERIFY(0x0200BD2C,cc_0200BD2C);

void cCcD_Stts_Ct(u32 p) {
    WWHD_FUNC(0x0200BD9C,void,p);
    f32 z=F(0x1000137Cu);
    store<u32>(p+12,0u);store<u32>(p+16,0xFFFFFFFFu);store<u8>(p+20,0);
    S(p,0,z);store<u8>(p+21,0);S(p,4,z);store<u8>(p+22,0);S(p,8,z);
}
VERIFY(0x0200BD9C,cCcD_Stts_Ct);

void cCcD_Stts_Init(u32 p,s32 weight,s32 flags,u32 actor,u32 id) {
    WWHD_FUNC(0x0200BDD0,void,p,weight,flags,actor,id);
    u32 v=load<u32>(p+24),fn=load<u32>(v+36);
    call_ptr<void>(fn,p,weight,flags,actor,id);
    store<u8>(p+20,(u8)weight);store<u8>(p+21,(u8)flags);store<u32>(p+12,actor);store<u32>(p+16,id);
}
VERIFY(0x0200BDD0,cCcD_Stts_Init);

void cCcD_Stts_PlusCcMove(u32 p,f32 x,f32 y,f32 z) {
    WWHD_FUNC(0x0200BE28,void,p,x,y,z);
    f32 a=F(p)+x,c=F(p,8)+z,b=F(p,4)+y;
    S(p,8,c);S(p,0,a);S(p,4,b);
    if (std::isnan(a)) call<void>(0x0273AA24,0x10001398u,0x1E7u,0x100013A4u);
    if (std::isnan(F(p,4))) call<void>(0x0273AA24,0x10001398u,0x1E8u,0x100013ECu);
    if (std::isnan(F(p,8))) call<void>(0x0273AA24,0x10001398u,0x1E9u,0x10001434u);
    f32 lo=F(0x1000138Cu),hi=F(0x10001390u);
    if (!(lo<F(p) && F(p)<hi && lo<F(p,4) && F(p,4)<hi && lo<F(p,8) && F(p,8)<hi))
        call<void>(0x0273AA24,0x10001398u,0x1EBu,0x1000147Cu);
}
VERIFY(0x0200BE28,cCcD_Stts_PlusCcMove);

void cc_0200BF6C(u32 p) {
    WWHD_FUNC(0x0200BF6C,void,p);
    store<u8>(p+22,0);
}
VERIFY(0x0200BF6C,cc_0200BF6C);

u32 cc_0200BF78(u32 p) {
    WWHD_FUNC(0x0200BF78,u32,p);
    if (!p) p=call<u32>(0x0273AD10,24u);
    if (p) {
        store<u32>(p+8,0u);store<u32>(p+12,0x10001568u);store<u32>(p+16,0u);store<u8>(p+20,0);store<u32>(p+4,0u);store<u32>(p,0u);
    }
    return p;
}
VERIFY(0x0200BF78,cc_0200BF78);

u32 cc_0200BFD0(u32 p) {
    WWHD_FUNC(0x0200BFD0,u32,p);
    if (!p) p=call<u32>(0x0273AD10,20u);
    if (p) {
        store<u32>(p+16,0u);store<u32>(p+12,0x10001578u);store<u32>(p+4,0u);store<u32>(p+8,0u);store<u32>(p,0u);
    }
    return p;
}
VERIFY(0x0200BFD0,cc_0200BFD0);

u32 cc_0200C034(u32 p) {
    WWHD_FUNC(0x0200C034,u32,p);
    if (!p) p=call<u32>(0x0273AD10,80u);
    if (p) {
        store<u32>(p+60,0x10001598u);call<void>(0x0200BF78,p);call<void>(0x0200BFD0,p+24);
        store<u32>(p+44,0u);store<u32>(p+48,0u);store<u32>(p+52,0u);store<u32>(p+68,0u);
        store<u32>(p+56,0x10001588u);store<u32>(p+60,0x10001648u);store<u32>(p+64,0u);
        call<void>(0x0200B6C0,p+72);store<u32>(p+64,0u);
    }
    return p;
}
VERIFY(0x0200C034,cc_0200C034);

void cCcD_Obj_Set(u32 p,u32 q) {
    WWHD_FUNC(0x0200C0D4,void,p,q);
    store<u32>(p+64,load<u32>(q+0));
    store<u32>(p+0,load<u32>(q+12));
    store<u32>(p+16,load<u32>(q+4));
    store<u8>(p+20,load<u8>(q+8));
    store<u32>(p+24,load<u32>(q+20));
    store<u32>(p+40,load<u32>(q+16));
    store<u32>(p+44,load<u32>(q+24));

}
VERIFY(0x0200C0D4,cCcD_Obj_Set);

u32 cc_0200C124(u32 p,u32 q,u32 out) {
    WWHD_FUNC(0x0200C124,u32,p,q,out);
    u32 v=load<u32>(q+28),fn=load<u32>(v+36);
    return call_ptr<u32>(fn,q,p,out);
}
VERIFY(0x0200C124,cc_0200C124);

u32 cc_0200C20C(u32 p,u32 q,u32 out) {
    WWHD_FUNC(0x0200C20C,u32,p,q,out);
    u32 v=load<u32>(q+28),fn=load<u32>(v+92);
    return call_ptr<u32>(fn,q,p,out);
}
VERIFY(0x0200C20C,cc_0200C20C);

u32 cc_0200C548(u32 p,u32 q,u32 out) {
    WWHD_FUNC(0x0200C548,u32,p,q,out);
    u32 v=load<u32>(q+28),fn=load<u32>(v+44);
    return call_ptr<u32>(fn,q,p,out);
}
VERIFY(0x0200C548,cc_0200C548);

u32 cc_0200C63C(u32 p,u32 q,u32 out) {
    WWHD_FUNC(0x0200C63C,u32,p,q,out);
    u32 v=load<u32>(q+28),fn=load<u32>(v+100);
    return call_ptr<u32>(fn,q,p,out);
}
VERIFY(0x0200C63C,cc_0200C63C);

u32 cc_0200C79C(u32 p,u32 q,u32 out) {
    WWHD_FUNC(0x0200C79C,u32,p,q,out);
    u32 v=load<u32>(q+28),fn=load<u32>(v+60);
    return call_ptr<u32>(fn,q,p,out);
}
VERIFY(0x0200C79C,cc_0200C79C);

u32 cc_0200C7EC(u32 p,u32 q,u32 out) {
    WWHD_FUNC(0x0200C7EC,u32,p,q,out);
    u32 v=load<u32>(q+28),fn=load<u32>(v+116);
    return call_ptr<u32>(fn,q,p,out);
}
VERIFY(0x0200C7EC,cc_0200C7EC);

u32 cc_0200CA18(u32 p,u32 q,u32 out) {
    WWHD_FUNC(0x0200CA18,u32,p,q,out);
    u32 v=load<u32>(q+28),fn=load<u32>(v+68);
    return call_ptr<u32>(fn,q,p,out);
}
VERIFY(0x0200CA18,cc_0200CA18);

u32 cc_0200CA68(u32 p,u32 q,u32 out) {
    WWHD_FUNC(0x0200CA68,u32,p,q,out);
    u32 v=load<u32>(q+28),fn=load<u32>(v+124);
    return call_ptr<u32>(fn,q,p,out);
}
VERIFY(0x0200CA68,cc_0200CA68);

u32 cc_0200C140(u32 p,u32 q,u32 out) {
    WWHD_FUNC(0x0200C140,u32,p,q,out);
    return call<u32>(0x020159EC,p+32,q+32,out);
}
VERIFY(0x0200C140,cc_0200C140);

u32 cc_0200C14C(u32 p,u32 q,u32 out) {
    WWHD_FUNC(0x0200C14C,u32,p,q,out);
    return call<u32>(0x02015CF0,p+32,q+32,out);
}
VERIFY(0x0200C14C,cc_0200C14C);

u32 cc_0200C200(u32 p,u32 q,u32 out) {
    WWHD_FUNC(0x0200C200,u32,p,q,out);
    return call<u32>(0x020168D4,p+32,q+32,out);
}
VERIFY(0x0200C200,cc_0200C200);

u32 cc_0200C614(u32 p,u32 q,u32 out) {
    WWHD_FUNC(0x0200C614,u32,p,q,out);
    return call<u32>(0x0201923C,p+32,q+32,out);
}
VERIFY(0x0200C614,cc_0200C614);

u32 cc_0200C620(u32 p,u32 q,u32 out) {
    WWHD_FUNC(0x0200C620,u32,p,q,out);
    return call<u32>(0x02016A20,p+32,q+32,out);
}
VERIFY(0x0200C620,cc_0200C620);

u32 cc_0200C62C(u32 p,u32 q,u32 out) {
    WWHD_FUNC(0x0200C62C,u32,p,q,out);
    return call<u32>(0x02014200,q+32,p+32,out);
}
VERIFY(0x0200C62C,cc_0200C62C);

u32 cc_0200C7B8(u32 p,u32 q,u32 out) {
    WWHD_FUNC(0x0200C7B8,u32,p,q,out);
    return call<u32>(0x02015CF0,q+32,p+32,out);
}
VERIFY(0x0200C7B8,cc_0200C7B8);

u32 cc_0200C7C8(u32 p,u32 q,u32 out) {
    WWHD_FUNC(0x0200C7C8,u32,p,q,out);
    return call<u32>(0x020156D0,p+32,q+32,out);
}
VERIFY(0x0200C7C8,cc_0200C7C8);

u32 cc_0200C7D4(u32 p,u32 q,u32 out) {
    WWHD_FUNC(0x0200C7D4,u32,p,q,out);
    return call<u32>(0x020186C4,p+32,q+32,out);
}
VERIFY(0x0200C7D4,cc_0200C7D4);

u32 cc_0200C7E0(u32 p,u32 q,u32 out) {
    WWHD_FUNC(0x0200C7E0,u32,p,q,out);
    return call<u32>(0x020186C8,p+32,q+32,out);
}
VERIFY(0x0200C7E0,cc_0200C7E0);

u32 cc_0200C84C(u32 p,u32 q,u32 out) {
    WWHD_FUNC(0x0200C84C,u32,p,q,out);
    return call<u32>(0x02014AF8,p+32,q+32,out);
}
VERIFY(0x0200C84C,cc_0200C84C);

u32 cc_0200C858(u32 p,u32 q,u32 out) {
    WWHD_FUNC(0x0200C858,u32,p,q,out);
    return call<u32>(0x020135C8,p+32,q+32,out);
}
VERIFY(0x0200C858,cc_0200C858);

u32 cc_0200CA34(u32 p,u32 q,u32 out) {
    WWHD_FUNC(0x0200CA34,u32,p,q,out);
    return call<u32>(0x020168D4,q+32,p+32,out);
}
VERIFY(0x0200CA34,cc_0200CA34);

u32 cc_0200CA44(u32 p,u32 q,u32 out) {
    WWHD_FUNC(0x0200CA44,u32,p,q,out);
    return call<u32>(0x02014200,p+32,q+32,out);
}
VERIFY(0x0200CA44,cc_0200CA44);

u32 cc_0200CA50(u32 p,u32 q,u32 out) {
    WWHD_FUNC(0x0200CA50,u32,p,q,out);
    return call<u32>(0x02018F6C,p+32,q+32,out);
}
VERIFY(0x0200CA50,cc_0200CA50);

u32 cc_0200CA5C(u32 p,u32 q,u32 out) {
    WWHD_FUNC(0x0200CA5C,u32,p,q,out);
    return call<u32>(0x02018F9C,p+32,q+32,out);
}
VERIFY(0x0200CA5C,cc_0200CA5C);

u32 cc_0200CAC8(u32 p,u32 q,u32 out) {
    WWHD_FUNC(0x0200CAC8,u32,p,q,out);
    return call<u32>(0x020135C8,q+32,p+32,out);
}
VERIFY(0x0200CAC8,cc_0200CAC8);

u32 cc_0200CAD8(u32 p,u32 q,u32 out) {
    WWHD_FUNC(0x0200CAD8,u32,p,q,out);
    return call<u32>(0x02013B78,p+32,q+32,out);
}
VERIFY(0x0200CAD8,cc_0200CAD8);

u32 cc_0200C228(u32 p,u32 q,u32 out) {
    WWHD_FUNC(0x0200C228,u32,p,q,out);
    Local<CcVector> hit;S(out,0,F(0x1000137Cu));
    return call<u32>(0x020159EC,p+32,q+32,hit.get());
}
VERIFY(0x0200C228,cc_0200C228);

u32 cc_0200C268(u32 p,u32 q,u32 out) {
    WWHD_FUNC(0x0200C268,u32,p,q,out);
    Local<CcVector> hit;S(out,0,F(0x1000137Cu));
    return call<u32>(0x02015CF0,p+32,q+32,hit.get());
}
VERIFY(0x0200C268,cc_0200C268);

u32 cc_0200C2A0(u32 p,u32 q,u32 out) {
    WWHD_FUNC(0x0200C2A0,u32,p,q,out);
    Local<CcVector> hit;S(out,0,F(0x1000137Cu));
    return call<u32>(0x020168D4,p+32,q+32,hit.get());
}
VERIFY(0x0200C2A0,cc_0200C2A0);

u32 cc_0200C808(u32 p,u32 q,u32 out) {
    WWHD_FUNC(0x0200C808,u32,p,q,out);
    Local<CcVector> hit;S(out,0,F(0x1000137Cu));
    return call<u32>(0x02015CF0,q+32,p+32,hit.get());
}
VERIFY(0x0200C808,cc_0200C808);

u32 cc_0200CA84(u32 p,u32 q,u32 out) {
    WWHD_FUNC(0x0200CA84,u32,p,q,out);
    Local<CcVector> hit;S(out,0,F(0x1000137Cu));
    return call<u32>(0x020168D4,q+32,p+32,hit.get());
}
VERIFY(0x0200CA84,cc_0200CA84);

u32 cc_0200C158(u32 p,u32 q,u32 out) {
    WWHD_FUNC(0x0200C158,u32,p,q,out);
    Local<CcTriangle> tri;makeTriangle(tri.a,q);
    return call<u32>(0x02016CE8,p+32,tri.get(),out);
}
VERIFY(0x0200C158,cc_0200C158);

u32 cc_0200C564(u32 p,u32 q,u32 out) {
    WWHD_FUNC(0x0200C564,u32,p,q,out);
    Local<CcTriangle> tri;makeTriangle(tri.a,p);
    return call<u32>(0x02016CE8,q+32,tri.get(),out);
}
VERIFY(0x0200C564,cc_0200C564);

u32 cc_0200C024() {
    WWHD_FUNC(0x0200C024,u32);
    return 0;
}
VERIFY(0x0200C024,cc_0200C024);

u32 cc_0200C02C() {
    WWHD_FUNC(0x0200C02C,u32);
    return 0;
}
VERIFY(0x0200C02C,cc_0200C02C);

u32 cc_0200C260() {
    WWHD_FUNC(0x0200C260,u32);
    return 0;
}
VERIFY(0x0200C260,cc_0200C260);

u32 cc_0200C658() {
    WWHD_FUNC(0x0200C658,u32);
    return 0;
}
VERIFY(0x0200C658,cc_0200C658);

u32 cc_0200C660() {
    WWHD_FUNC(0x0200C660,u32);
    return 0;
}
VERIFY(0x0200C660,cc_0200C660);

u32 cc_0200C668() {
    WWHD_FUNC(0x0200C668,u32);
    return 0;
}
VERIFY(0x0200C668,cc_0200C668);

u32 cc_0200C670() {
    WWHD_FUNC(0x0200C670,u32);
    return 0;
}
VERIFY(0x0200C670,cc_0200C670);

u32 cc_0200C844() {
    WWHD_FUNC(0x0200C844,u32);
    return 0;
}
VERIFY(0x0200C844,cc_0200C844);

u32 cc_0200CAC0() {
    WWHD_FUNC(0x0200CAC0,u32);
    return 0;
}
VERIFY(0x0200CAC0,cc_0200CAC0);

u32 cc_0200CCE4() {
    WWHD_FUNC(0x0200CCE4,u32);
    return 0;
}
VERIFY(0x0200CCE4,cc_0200CCE4);

u32 cc_0200CCFC() {
    WWHD_FUNC(0x0200CCFC,u32);
    return 0;
}
VERIFY(0x0200CCFC,cc_0200CCFC);

u32 cc_0200CD04() {
    WWHD_FUNC(0x0200CD04,u32);
    return 0;
}
VERIFY(0x0200CD04,cc_0200CD04);

u32 cc_0200C794(u32 p) {
    WWHD_FUNC(0x0200C794,u32,p);
    return p+32;
}
VERIFY(0x0200C794,cc_0200C794);

u32 cc_0200CA10(u32 p) {
    WWHD_FUNC(0x0200CA10,u32,p);
    return p+32;
}
VERIFY(0x0200CA10,cc_0200CA10);

u32 cCcD_ShapeAttr_GetCoCP() {
    WWHD_FUNC(0x0200CCF0,u32);
    return 0x101FF504u;
}
VERIFY(0x0200CCF0,cCcD_ShapeAttr_GetCoCP);

void cc_0200CCEC() {
    WWHD_FUNC(0x0200CCEC,void);

}
VERIFY(0x0200CCEC,cc_0200CCEC);

void cCcD_CpsAttr_CalcAabBox(u32 p) {
    WWHD_FUNC(0x0200C2D8,void,p);
    call<void>(0x02017DAC,p);call<void>(0x02017E74,p,p+32);call<void>(0x02017E74,p,p+44);
    f32 radius=F(p,60);call<void>(0x02017F54,p,radius);
}
VERIFY(0x0200C2D8,cCcD_CpsAttr_CalcAabBox);

u32 cCcD_CpsAttr_GetNVec(u32 p,u32 point,u32 out) {
    WWHD_FUNC(0x0200C328,u32,p,point,out);
    Local<CcVector> line,relative,nearest;
    call<void>(0x028E8DAC,p+44,p+32,line.get());
    f32 length=call<f32>(0x028E8F44,line.get(),line.get());
    f32 eps=F(0x100030B8u);
    if (std::fabs(length)<eps) return 0;
    call<void>(0x028E8DAC,point,p+32,relative.get());
    f32 dot=call<f32>(0x028E8F44,relative.get(),line.get());
    f32 fraction=dot/length,zero=F(0x1000137Cu);
    if (fraction<zero) copyVector(nearest.a,p+32);
    else if (fraction>F(0x10001384u)) copyVector(nearest.a,p+44);
    else {
        call<void>(0x028E8E64,line.get(),line.get(),fraction);
        call<void>(0x028E8D88,line.get(),p+32,nearest.get());
    }
    call<void>(0x028E8DAC,point,nearest.get(),out);
    f32 mag=call<f32>(0x028E8E10,out);
    if (std::fabs(mag)<eps) {S(out,4,zero);S(out,0,zero);S(out,8,zero);return 0;}
    call<void>(0x028E8EF0,out,out);return 1;
}
VERIFY(0x0200C328,cCcD_CpsAttr_GetNVec);

void cCcD_TriAttr_CalcAabBox(u32 p) {
    WWHD_FUNC(0x0200C678,void,p);
    Local<CcVector> vertex;call<void>(0x02017DAC,p);
    floatVector(vertex.a,p+52);call<void>(0x02017E74,p,vertex.get());
    floatVector(vertex.a,p+64);call<void>(0x02017E74,p,vertex.get());
    floatVector(vertex.a,p+76);call<void>(0x02017E74,p,vertex.get());
}
VERIFY(0x0200C678,cCcD_TriAttr_CalcAabBox);

u32 cCcD_TriAttr_GetNVec(u32 p,u32 point,u32 out) {
    WWHD_FUNC(0x0200C710,u32,p,point,out);
    f32 dot=call<f32>(0x028E8F44,p+32,point);
    f32 side=dot+F(p,44),zero=F(0x1000137Cu);
    store<u32>(out,load<u32>(p+32));store<u32>(out+4,load<u32>(p+36));store<u32>(out+8,load<u32>(p+40));
    if (side<zero) call<void>(0x028E8E64,out,out,F(0x10001514u));
    return 1;
}
VERIFY(0x0200C710,cCcD_TriAttr_GetNVec);

void cCcD_CylAttr_CalcAabBox(u32 p) {
    WWHD_FUNC(0x0200C864,void,p);
    Local<CcVector> low,high;
    f32 r=F(p,44),x=F(p,32),z=F(p,40),y=F(p,36),h=F(p,48);
    S(low.a,0,x-r);S(low.a,4,y);S(low.a,8,z-r);
    S(high.a,4,y+h);S(high.a,0,x+r);S(high.a,8,z+r);
    call<void>(0x02017D08,p,low.get(),high.get());
}
VERIFY(0x0200C864,cCcD_CylAttr_CalcAabBox);

u32 cCcD_CylAttr_GetNVec(u32 p,u32 point,u32 out) {
    WWHD_FUNC(0x0200C8CC,u32,p,point,out);
    Local<CcVector> nearest;
    f32 y=F(point,4),base=F(p,36),x=F(p,32);
    S(nearest.a,0,x);S(nearest.a,4,base);S(nearest.a,8,F(p,40));
    if (base>y) {}
    else {f32 height=F(p,48);if ((base+height)<y) S(nearest.a,4,base+height);else S(nearest.a,4,y);}
    call<void>(0x028E8DAC,point,nearest.get(),out);
    f32 mag=call<f32>(0x028E8E10,out),eps=F(0x100030B8u);
    if (std::fabs(mag)<eps) {f32 zero=F(0x1000137Cu);S(out,0,zero);S(out,4,zero);S(out,8,zero);return 0;}
    call<void>(0x028E8EF0,out,out);return 1;
}
VERIFY(0x0200C8CC,cCcD_CylAttr_GetNVec);

void cCcD_SphAttr_CalcAabBox(u32 p) {
    WWHD_FUNC(0x0200CAE4,void,p);
    Local<CcVector> low,high;
    f32 x=F(p,32),y=F(p,36),z=F(p,40),r=F(p,44);
    S(low.a,0,x-r);S(low.a,4,y-r);S(low.a,8,z-r);
    S(high.a,0,x+r);S(high.a,4,y+r);S(high.a,8,z+r);
    call<void>(0x02017D08,p,low.get(),high.get());
}
VERIFY(0x0200CAE4,cCcD_SphAttr_CalcAabBox);

u32 cCcD_SphAttr_GetNVec(u32 p,u32 point,u32 out) {
    WWHD_FUNC(0x0200CB7C,u32,p,point,out);
    S(out,0,F(point)-F(p,32));S(out,4,F(point,4)-F(p,36));S(out,8,F(point,8)-F(p,40));
    f32 mag=call<f32>(0x028E8E10,out),eps=F(0x100030B8u);
    if (std::fabs(mag)<eps) {f32 zero=F(0x1000137Cu);S(out,0,zero);S(out,4,zero);S(out,8,zero);return 0;}
    call<void>(0x028E8EF0,out,out);return 1;
}
VERIFY(0x0200CB7C,cCcD_SphAttr_GetNVec);

void cCcD_static_init() {
    WWHD_FUNC(0x0200CC2C,void);
    store<u32>(0x101FF500u,0u);store<u32>(0x101FF4FCu,0u);store<u32>(0x101FF4F8u,0u);store<u32>(0x101FF4F4u,0u);
    call<void>(0x028F026C,0x1018C690u);
    f32 lo=F(0x10001518u),hi=F(0x1000151Cu);S(0x101FF4E8u,0,lo);S(0x101FF4ECu,0,hi);
    call<void>(0x028ED6F8,0x101FF4F0u);call<void>(0x028F026C,0x1018C69Cu);
    call<void>(0x028EAB2C,0x101FF4F1u);call<void>(0x028F026C,0x1018C6A8u);
    f32 y=F(0x101FFBA8u,4),x=F(0x101FFBA8u),z=F(0x101FFBA8u,8);
    S(0x101FF504u,0,x);S(0x101FF504u,8,z);S(0x101FF504u,4,y);
}
VERIFY(0x0200CC2C,cCcD_static_init);
