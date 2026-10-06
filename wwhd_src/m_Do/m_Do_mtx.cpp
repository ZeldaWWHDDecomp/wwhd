// WWHD matrix helpers; see wwhd_src/README.md.
// Complete contiguous translation unit, including the unnamed XYZrotS entry.
// External SDK/vector helpers remain ordinary guest calls. Matrix temporaries
// are 48 bytes and vector temporaries 12 bytes, matching the real pointee sizes.
#include "gabi.h"
using namespace gabi;
struct Matrix { be<f32> v[12]; };
struct Vector { be<f32> v[3]; };
static f32 F(u32 p,u32 o=0) { return load<f32>(p+o); }
static void S(u32 p,u32 o,f32 x) { store<f32>(p+o,x); }
static void copyVec(u32 d,u32 p) { u32 x=load<u32>(p), y=load<u32>(p+4), z=load<u32>(p+8); store<u32>(d,x);store<u32>(d+4,y);store<u32>(d+8,z); }
static void loadVec(u32 d,u32 p) { f32 x=F(p),y=F(p,4),z=F(p,8);S(d,0,x);S(d,4,y);S(d,8,z); }
void mDoMtx_ZrotS(u32 p,s32 a) {
    WWHD_FUNC(0x025F181C, void,p,a);
    u32 t=0x104A44F8u+((u16)a>>3)*8u;
    f32 sn=F(t), cs=F(t,4), zero=F(0x10059044u), one=F(0x10059048u);
    S(p,0,cs);
    S(p,4,-sn);
    S(p,8,zero);
    S(p,12,zero);
    S(p,16,sn);
    S(p,20,cs);
    S(p,24,zero);
    S(p,28,zero);
    S(p,32,zero);
    S(p,36,zero);
    S(p,40,one);
    S(p,44,zero);
}
VERIFY(0x025F181C,mDoMtx_ZrotS);
void mDoMtx_YrotS(u32 p,s32 a) {
    WWHD_FUNC(0x025F1884, void,p,a);
    u32 t=0x104A44F8u+((u16)a>>3)*8u;
    f32 sn=F(t), cs=F(t,4), zero=F(0x10059044u), one=F(0x10059048u);
    S(p,0,cs);
    S(p,4,zero);
    S(p,8,sn);
    S(p,12,zero);
    S(p,16,zero);
    S(p,20,one);
    S(p,24,zero);
    S(p,28,zero);
    S(p,32,-sn);
    S(p,36,zero);
    S(p,40,cs);
    S(p,44,zero);
}
VERIFY(0x025F1884,mDoMtx_YrotS);
void mDoMtx_XrotS(u32 p,s32 a) {
    WWHD_FUNC(0x025F18EC, void,p,a);
    u32 t=0x104A44F8u+((u16)a>>3)*8u;
    f32 sn=F(t), cs=F(t,4), zero=F(0x10059044u), one=F(0x10059048u);
    S(p,0,one);
    S(p,4,zero);
    S(p,8,zero);
    S(p,12,zero);
    S(p,16,zero);
    S(p,20,cs);
    S(p,24,-sn);
    S(p,28,zero);
    S(p,32,zero);
    S(p,36,sn);
    S(p,40,cs);
    S(p,44,zero);
}
VERIFY(0x025F18EC,mDoMtx_XrotS);
void mDoMtx_XYZrotS(u32 p,s32 x,s32 y,s32 z) {
    WWHD_FUNC(0x025F1954,void,p,x,y,z);
    Local<Matrix> m;
    if (z) call<void>(0x025F181C,p,z); else call<void>(0x028E9098,p);
    if (y) { call<void>(0x025F1884,m.get(),y); call<void>(0x028E9108,p,m.get(),p); }
    if (x) { call<void>(0x025F18EC,m.get(),x); call<void>(0x028E9108,p,m.get(),p); }
}
VERIFY(0x025F1954,mDoMtx_XYZrotS);
void mDoMtx_XYZrotM(u32 p,s32 x,s32 y,s32 z) {
    WWHD_FUNC(0x025F19F8,void,p,x,y,z);
    Local<Matrix> m;
    if (z) { call<void>(0x025F181C,m.get(),z); call<void>(0x028E9108,p,m.get(),p); }
    if (y) { call<void>(0x025F1884,m.get(),y); call<void>(0x028E9108,p,m.get(),p); }
    if (x) { call<void>(0x025F18EC,m.get(),x); call<void>(0x028E9108,p,m.get(),p); }
}
VERIFY(0x025F19F8,mDoMtx_XYZrotM);
void mDoMtx_ZXYrotS(u32 p,s32 x,s32 y,s32 z) {
    WWHD_FUNC(0x025F1AA4,void,p,x,y,z);
    Local<Matrix> m;
    if (y) call<void>(0x025F1884,p,y); else call<void>(0x028E9098,p);
    if (x) { call<void>(0x025F18EC,m.get(),x); call<void>(0x028E9108,p,m.get(),p); }
    if (z) { call<void>(0x025F181C,m.get(),z); call<void>(0x028E9108,p,m.get(),p); }
}
VERIFY(0x025F1AA4,mDoMtx_ZXYrotS);
void mDoMtx_ZXYrotM(u32 p,s32 x,s32 y,s32 z) {
    WWHD_FUNC(0x025F1B48,void,p,x,y,z);
    Local<Matrix> m;
    if (y) { call<void>(0x025F1884,m.get(),y); call<void>(0x028E9108,p,m.get(),p); }
    if (x) { call<void>(0x025F18EC,m.get(),x); call<void>(0x028E9108,p,m.get(),p); }
    if (z) { call<void>(0x025F181C,m.get(),z); call<void>(0x028E9108,p,m.get(),p); }
}
VERIFY(0x025F1B48,mDoMtx_ZXYrotM);
void mDoMtx_XrotM(u32 p,s32 a) {
    WWHD_FUNC(0x025F1BF4,void,p,a);
    Local<Matrix> m;
    call<void>(0x025F18EC,m.get(),a);
    call<void>(0x028E9108,p,m.get(),p);
}
VERIFY(0x025F1BF4,mDoMtx_XrotM);
void mDoMtx_YrotM(u32 p,s32 a) {
    WWHD_FUNC(0x025F1C28,void,p,a);
    Local<Matrix> m;
    call<void>(0x025F1884,m.get(),a);
    call<void>(0x028E9108,p,m.get(),p);
}
VERIFY(0x025F1C28,mDoMtx_YrotM);
void mDoMtx_ZrotM(u32 p,s32 a) {
    WWHD_FUNC(0x025F1C5C,void,p,a);
    Local<Matrix> m;
    call<void>(0x025F181C,m.get(),a);
    call<void>(0x028E9108,p,m.get(),p);
}
VERIFY(0x025F1C5C,mDoMtx_ZrotM);
void mDoMtx_lookAt(u32 p,u32 eye,u32 target,s32 angle) {
    WWHD_FUNC(0x025F1C90,void,p,eye,target,angle);
    Local<Vector> e,t,n,u,b,tmp,alt;
    Local<Matrix> rot;
    loadVec(e.a,eye);loadVec(t.a,target);
    call<void>(0x0201ADE0,e.get(),tmp.get(),t.get());
    copyVec(n.a,tmp.a);
    call<void>(0x0201B31C,n.get(),t.get());
    f32 zero=F(0x10059044u), one=F(0x10059048u);
    S(u.a,0,zero);S(u.a,4,one);S(u.a,8,zero);
    call<void>(0x0201B080,u.get(),t.get(),n.get());copyVec(u.a,t.a);
    if (call<s32>(0x0201B47C,u.get())==0) {
        S(u.a,4,zero);S(u.a,8,zero);S(u.a,0,-F(n.a,4));
        call<void>(0x0201B080,u.get(),alt.get(),n.get());copyVec(u.a,alt.a);
        call<void>(0x0201B31C,u.get(),alt.get());
    }
    call<void>(0x0201B080,n.get(),t.get(),u.get());copyVec(b.a,t.a);
    call<void>(0x0201B31C,b.get(),t.get());
    f32 ux=F(u.a),uy=F(u.a,4),uz=F(u.a,8);
    S(p,0,ux);S(p,8,uz);S(p,4,uy);
    f32 dot=call<f32>(0x028E8F44,u.get(),e.get());
    f32 bx=F(b.a),by=F(b.a,4),bz=F(b.a,8);
    S(p,16,bx);S(p,24,bz);S(p,20,by);S(p,12,-dot);
    dot=call<f32>(0x028E8F44,b.get(),e.get());
    f32 nx=F(n.a),ny=F(n.a,4),nz=F(n.a,8);
    S(p,40,nz);S(p,32,nx);S(p,36,ny);S(p,28,-dot);
    dot=call<f32>(0x028E8F44,n.get(),e.get());S(p,44,-dot);
    call<void>(0x025F181C,rot.get(),angle);
    call<void>(0x028E9108,rot.get(),p,p);
}
VERIFY(0x025F1C90,mDoMtx_lookAt);
void mDoMtx_lookAtUp(u32 p,u32 eye,u32 target,u32 up,s32 angle) {
    WWHD_FUNC(0x025F1EAC,void,p,eye,target,up,angle);
    Local<Vector> e,t,u,n;Local<Matrix> rot;
    loadVec(e.a,eye);loadVec(t.a,target);loadVec(u.a,up);
    call<void>(0x0201ADE0,e.get(),n.get(),t.get());
    s32 ok=call<s32>(0x0201B47C,n.get());
    f32 one=F(0x10059048u);
    if (!ok) S(t.a,8,F(t.a,8)+one);
    // Ordered comparisons preserve the reference's unordered-NaN fallback.
    f32 eps=F(0x100030B8u);
    if (std::fabs(F(u.a))<eps && std::fabs(F(u.a,4))<eps && std::fabs(F(u.a,8))<eps) S(u.a,4,one);
    call<void>(0x028E9684,p,e.get(),u.get(),t.get());
    call<void>(0x025F181C,rot.get(),angle);
    call<void>(0x028E9108,rot.get(),p,p);
}
VERIFY(0x025F1EAC,mDoMtx_lookAtUp);
void mDoMtx_concatProjView(u32 p,u32 v,u32 d) {
    WWHD_FUNC(0x025F1FC4,void,p,v,d);
    call<void>(0x028E9108,p,v,d);
    for (u32 i=0;i<4;i++) {
        f32 b=F(p,52), c=F(v,16+i*4), a=F(p,48), x=F(v,i*4), z=F(p,56), y=F(v,32+i*4);
        f32 r=fmadds(z,y,fmadds(a,x,b*c));
        if (i==3) r=r+F(p,60);
        S(d,48+i*4,r);
    }
}
VERIFY(0x025F1FC4,mDoMtx_concatProjView);
s32 mDoMtx_inverseTranspose(u32 p,u32 d) {
    WWHD_FUNC(0x025F20B0,s32,p,d);
    f32 h=F(p,36),a=F(p),g=F(p,32),e=F(p,20),b=F(p,4),f=F(p,24),c=F(p,8),i=F(p,40),dd=F(p,16);
    f32 ah=a*h, ae=a*e, bf=b*f;
    f32 determinant=fmadds(ae,i,bf*g);
    determinant=fmadds(c*dd,h,determinant);
    f32 ge=g*e;
    determinant=fnmsubs(ge,c,determinant);
    determinant=fnmsubs(dd*b,i,determinant);
    determinant=fnmsubs(ah,f,determinant);
    f32 zero=F(0x10059044u);
    if (determinant==zero) return 0;
    f32 inv=F(0x10059048u)/determinant;
    f32 r02=fmsubs(dd,h,ge)*inv;
    f32 r01=-fmsubs(dd,i,g*f)*inv;
    f32 r00=fmsubs(e,i,h*f)*inv;
    f32 r10=-fmsubs(b,i,h*c)*inv;
    f32 r11=fmsubs(a,i,g*c)*inv;
    f32 r20=fnmsubs(e,c,bf)*inv;
    S(d,32,r20);
    f32 d2=F(p,16), c2=F(p,8), a2=F(p), f2=F(p,24);
    f32 r21=-fmsubs(a2,f2,d2*c2)*inv;
    f32 r12=-fnmsubs(g,b,ah)*inv;
    S(d,36,r21);
    f32 b2=F(p,4),d3=F(p,16),a3=F(p),e2=F(p,20);
    S(d,8,r02);S(d,12,zero);S(d,20,r11);S(d,24,r12);S(d,4,r01);
    f32 r22=fmsubs(a3,e2,d3*b2)*inv;
    S(d,0,r00);S(d,16,r10);S(d,40,r22);S(d,44,zero);S(d,28,zero);
    return 1;
}
VERIFY(0x025F20B0,mDoMtx_inverseTranspose);
// Keep each component load after the preceding store: output can alias inputs.
void mDoMtx_QuatConcat(u32 a,u32 b,u32 d) {
    WWHD_FUNC(0x025F2258,void,a,b,d);
    f32 w=fmsubs(F(a,12),F(b,12),F(a)*F(b));
    w=fnmsubs(F(a,4),F(b,4),w);w=fnmsubs(F(a,8),F(b,8),w);S(d,12,w);
    f32 x=F(a)*F(b,12);x=fmadds(F(a,12),F(b),x);x=fmadds(F(a,4),F(b,8),x);x=fnmsubs(F(a,8),F(b,4),x);S(d,0,x);
    f32 y=F(a,4)*F(b,12);y=fmadds(F(a,12),F(b,4),y);y=fmadds(F(a,8),F(b),y);y=fnmsubs(F(a),F(b,8),y);S(d,4,y);
    f32 z=F(a,8)*F(b,12);z=fmadds(F(a,12),F(b,8),z);z=fmadds(F(a),F(b,4),z);z=fnmsubs(F(a,4),F(b),z);S(d,8,z);
}
VERIFY(0x025F2258,mDoMtx_QuatConcat);
void mDoMtx_MtxToRot(u32 p,u32 d) {
    WWHD_FUNC(0x025F232C,void,p,d);
    f32 z=F(p,40),x=F(p,8);
    f32 len=call<f32>(0x028F4384,fmadds(x,x,z*z));
    s32 rx=call<s32>(0x020195B0,-F(p,24),len);store<s16>(d,(s16)rx);
    if (rx==0x4000 || rx==-0x4000) {
        store<s16>(d+4,0);
        f32 a=-F(p,32),b=F(p);
        store<s16>(d+2,(s16)call<s32>(0x020195B0,a,b));
    } else {
        f32 a=F(p,8),b=F(p,40);store<s16>(d+2,(s16)call<s32>(0x020195B0,a,b));
        a=F(p,16);b=F(p,20);store<s16>(d+4,(s16)call<s32>(0x020195B0,a,b));
    }
}
VERIFY(0x025F232C,mDoMtx_MtxToRot);
s32 mDoMtx_push() {
    WWHD_FUNC(0x025F23EC,s32);
    u32 end=load<u32>(0x101F48EC),next=load<u32>(0x101F48E8);
    if (next>=end) {call<void>(0x0273AA24,0x1005904Cu,0x2DBu,0x1005905Cu);return 0;}
    next=load<u32>(0x101F48E8);store<u32>(0x101F48E8,next+48);
    call<void>(0x028E90D4,0x1048D0CCu,next);return 1;
}
VERIFY(0x025F23EC,mDoMtx_push);
s32 mDoMtx_pop() {
    WWHD_FUNC(0x025F2468,s32);
    u32 next=load<u32>(0x101F48E8);
    if (next<=0x1048D0FCu) {call<void>(0x0273AA24,0x10059078u,0x2F1u,0x10059068u);return 0;}
    next-=48;store<u32>(0x101F48E8,next);call<void>(0x028E90D4,next,0x1048D0CCu);return 1;
}
VERIFY(0x025F2468,mDoMtx_pop);
void mDoMtx_transM(f32 x,f32 y,f32 z) {
    WWHD_FUNC(0x025F24E0,void,x,y,z);
    Local<Matrix> m;call<void>(0x028E93CC,m.get(),x,y,z);call<void>(0x028E9108,0x1048D0CCu,m.get(),0x1048D0CCu);
}
VERIFY(0x025F24E0,mDoMtx_transM);
void mDoMtx_scaleM(f32 x,f32 y,f32 z) {
    WWHD_FUNC(0x025F2518,void,x,y,z);
    Local<Matrix> m;call<void>(0x028E945C,m.get(),x,y,z);call<void>(0x028E9108,0x1048D0CCu,m.get(),0x1048D0CCu);
}
VERIFY(0x025F2518,mDoMtx_scaleM);
void mDoMtx_lYrotM(s32 a) {
    WWHD_FUNC(0x025F2550,void,a);
    Local<Matrix> m;call<void>(0x025F1884,m.get(),a>>16);call<void>(0x028E9108,0x1048D0CCu,m.get(),0x1048D0CCu);
}
VERIFY(0x025F2550,mDoMtx_lYrotM);
void mDoMtx_rYrotM(f32 a) {
    WWHD_FUNC(0x025F2590,void,a);
    Local<Matrix> m;call<void>(0x028E98C0,m.get(),0x59u,a);call<void>(0x028E9108,0x1048D0CCu,m.get(),0x1048D0CCu);
}
VERIFY(0x025F2590,mDoMtx_rYrotM);
void mDoMtx_quatM(u32 q) {
    WWHD_FUNC(0x025F25CC,void,q);
    Local<Matrix> m;call<void>(0x028E8C78,m.get(),q);call<void>(0x028E9108,0x1048D0CCu,m.get(),0x1048D0CCu);
}
VERIFY(0x025F25CC,mDoMtx_quatM);

/* 025F2608 __sinit_m_Do_mtx_cpp: the header statics
 * (block 1048D0B0, records 101F48C4, {-pi, pi} from 10059088), then mDoMtx_stack_c's stack
 * pointers (next = buffer 1048D0FC, end = buffer + 0x300) and the quaternion stack object at
 * 1048D3FC (first quaternion zeroed, next/end pointers at +0 / +0x114 / +0x118). */
static void __sinit_m_Do_mtx_cpp() {
    WWHD_FUNC(0x025F2608, void);
    const u32 bss = 0x1048D0B0, rec = 0x101F48C4, ro = 0x10059088;
    gabi::store<u32>(bss + 0x18, 0); gabi::store<u32>(bss + 0x14, 0); gabi::store<u32>(bss + 0x10, 0); gabi::store<u32>(bss + 0xC, 0);
    gabi::call(0x028F026C, rec);
    f32 lo = gabi::load<f32>(ro), hi = gabi::load<f32>(ro + 4);
    gabi::store<f32>(bss, lo);
    gabi::store<f32>(bss + 4, hi);
    gabi::call(0x028ED6F8, bss + 8);
    gabi::call(0x028F026C, rec + 0xC);
    gabi::call(0x028EAB2C, bss + 9);
    gabi::call(0x028F026C, rec + 0x18);
    const u32 buffer = 0x1048D0FC;
    gabi::store<u32>(0x101F48E8, buffer);
    f32 zero = gabi::load<f32>(0x10059044);
    gabi::store<u32>(0x101F48EC, buffer + 0x300);
    const u32 q = 0x1048D3FC;
    gabi::store<f32>(q + 8, zero);
    gabi::store<f32>(q + 0x10, zero);
    gabi::store<u32>(q + 0x118, q + 0x114);
    gabi::store<u32>(q + 0x114, q + 0x14);
    gabi::store<f32>(q + 0xC, zero);
    gabi::store<u32>(q, q + 4);
    gabi::store<f32>(q + 4, zero);
}
VERIFY(0x025F2608, __sinit_m_Do_mtx_cpp);
