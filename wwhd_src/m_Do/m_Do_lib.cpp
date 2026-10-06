// WWHD utility and projection helpers; see wwhd_src/README.md.
#include "gabi.h"
using namespace gabi;
struct LibColor { be<f32> rgba[4]; };
struct LibVector { be<f32> xyz[3]; };
struct LibPoint { be<f32> xy[2]; };
// The rectangle includes a disposer pointer and vtable after its four floats.
struct LibRectangle { u8 bytes[24]; };
static f32 F(u32 p,u32 n=0) { return load<f32>(p+n); }
static void S(u32 p,u32 n,f32 x) { store<f32>(p+n,x); }
// Later call effects can overwrite only part of a depth word, exposing a NaN
// payload. The two reference multiply-add sites choose product/accumulator
// NaNs in different host operand orders; preserve each observed site order.
static f32 projectMadProduct(f32 a,f32 c,f32 b) {
    if (std::isnan(a)) return ppc_qnan(a);
    if (std::isnan(c)) return ppc_qnan(c);
    if (std::isnan(b)) return ppc_qnan(b);
    return fmadds(a,c,b);
}
static f32 projectMadAccumulator(f32 a,f32 c,f32 b) {
    if (std::isnan(b)) return ppc_qnan(b);
    return projectMadProduct(a,c,b);
}

void mDoLib_colorToFloat(u32 out,u32 color) {
    WWHD_FUNC(0x025F0A1C,void,out,color);
    f32 scale=F(0x10058F68u);
    f32 r=(f32)load<u8>(color)/scale,g=(f32)load<u8>(color+1)/scale;
    f32 b=(f32)load<u8>(color+2)/scale,a=(f32)load<u8>(color+3)/scale;
    S(out,0,r);S(out,4,g);S(out,8,b);S(out,12,a);
}
VERIFY(0x025F0A1C,mDoLib_colorToFloat);

void mDoLib_setMaterialColor(u32 model,u32 alpha,u32 blend) {
    WWHD_FUNC(0x025F0AD0,void,model,alpha,blend);
    Local<LibColor> color,linear;
    u32 resource=load<u32>(model),offset=load<u32>(resource+32);
    u32 state=offset ? resource+32+offset : 0;
    call<void>(0x027E212C,state,3u);
    call<void>(0x027E19B4,state+24,4u);call<void>(0x027E19E4,state+24,4u);
    call<void>(0x027E19C4,state+24,5u);call<void>(0x027E19F4,state+24,5u);
    call<void>(0x027E18F8,state+8,blend ? 1u : 0u);
    u32 block=load<u32>(model+24),vt=load<u32>(block+4),fn=load<u32>(vt+76);
    u32 rgba=call_ptr<u32>(fn,block,3u);
    store<u8>(rgba+3,(u8)alpha);
    block=load<u32>(model+24);vt=load<u32>(block+4);fn=load<u32>(vt+60);
    call_ptr<void>(fn,block,3u,rgba);
    call<void>(0x025F0A1C,color.get(),rgba);
    f32 one=F(0x10058F78u);call<void>(0x0274D458,linear.get(),color.get(),one);
    store<u32>(model+160,load<u32>(model+160)|0x400u);
    u32 target=call<u32>(0x027F9F0C,model+160,10u);
    f32 a=(f32)load<u8>(rgba+3)/F(0x10058F68u);
    f32 y=F(linear.a,4),x=F(linear.a),z=F(linear.a,8);
    S(target,4,y);S(target,8,z);S(target,0,x);S(target,12,a);
}
VERIFY(0x025F0AD0,mDoLib_setMaterialColor);

void mDoLib_project(u32 src,u32 dst) {
    WWHD_FUNC(0x025F0C48,void,src,dst);
    u32 game=call<u32>(0x025200D4);
    f32 zero=F(0x10058F90u);
    if (!load<u32>(game+0x5FA4)) {S(dst,4,zero);S(dst,8,zero);S(dst,0,zero);return;}
    u32 matrix=call<u32>(0x02524628);call<void>(0x028E8F64,matrix,src,dst);
    matrix=call<u32>(0x02524628);f32 x=F(src),m0=F(matrix,48);
    matrix=call<u32>(0x02524628);f32 y=F(src,4),m1=F(matrix,52);
    f32 w=projectMadProduct(x,m0,fmuls_ppc(y,m1));
    matrix=call<u32>(0x02524628);f32 z=F(src,8),m2=F(matrix,56);w=projectMadAccumulator(z,m2,w);
    matrix=call<u32>(0x02524628);f32 depth=F(dst,8),m3=F(matrix,60);w=fadds_ppc(w,m3);
    if (!(depth<zero)) depth=zero;
    f32 half=F(0x10058F94u),scale;
    if (w>zero) {scale=half/w;S(dst,8,depth*scale);}
    else {
        f32 large=F(0x10058F98u);
        if (w==zero) S(dst,8,depth*large);else S(dst,8,depth*(half/w));
        scale=large;
    }
    game=call<u32>(0x025200D4);u32 viewport=load<u32>(game+0x5FA0);
    f32 ox=F(viewport),oy=F(viewport,4),width=F(viewport,8),height;
    if (ox!=zero) {
        height=F(viewport,12);
        ox=fmsubs(ox+ox+width,half,F(0x10058FA0u));width=F(0x10058FA4u);
    } else height=F(viewport,12);
    f32 px=F(dst),py=F(dst,4);
    if (oy!=zero) {oy=fmsubs(oy+oy+height,half,F(0x10058FA8u));height=F(0x10058FACu);}
    f32 sx=fmadds(px,scale,half),sy=fnmsubs(py,scale,half);
    S(dst,0,fmadds(sx,width,ox));S(dst,4,fmadds(sy,height,oy));
}
VERIFY(0x025F0C48,mDoLib_project);

void mDoLib_projectScreen(u32 src,u32 dst) {
    WWHD_FUNC(0x025F0EA4,void,src,dst);
    Local<LibRectangle> rectangle;Local<LibVector> point;Local<LibPoint> screen;
    u32 system=load<u32>(0x101F95D0u),count=load<u32>(system+0x1020),list=load<u32>(system+0x1024);
    if (count>1) list+=4;
    u32 camera=load<u32>(list),flags=load<u32>(camera+80),view;
    if ((flags&0x40u) && (flags&0x80u)) view=camera+132;
    else {view=load<u32>(camera+72);if (!view) view=0x104A2098u;}
    u32 projection;
    if (flags&0x1000u) projection=load<u32>(camera+0x164);
    else {projection=load<u32>(camera+76);if (!projection) projection=0x104A20FCu;}
    f32 zero=F(0x10058F90u),width=F(0x10058FA4u),height=F(0x10058FACu);
    call<void>(0x0274F424,rectangle.get(),zero,zero,width,height);
    f32 x=F(src),y=F(src,4),z=F(src,8);S(point.a,0,x);S(point.a,4,y);S(point.a,8,z);
    call<void>(0x0274CB50,view,screen.get(),point.get(),projection,rectangle.get());
    f32 sx=F(screen.a),sy=F(screen.a,4);S(dst,0,sx);S(dst,8,zero);S(dst,4,sy);
}
VERIFY(0x025F0EA4,mDoLib_projectScreen);

void mDoLib_projectViewport(u32 src,u32 dst,u32 viewport) {
    WWHD_FUNC(0x025F1018,void,src,dst,viewport);
    u32 game=call<u32>(0x025200D4),old=load<u32>(game+0x5FA0);
    game=call<u32>(0x025200D4);store<u32>(game+0x5FA0,viewport);
    call<void>(0x025F0C48,src,dst);
    game=call<u32>(0x025200D4);store<u32>(game+0x5FA0,old);
}
VERIFY(0x025F1018,mDoLib_projectViewport);

void mDoLib_copyViewRecord(u32 out) {
    WWHD_FUNC(0x025F1084,void,out);
    f32 y=F(0x104B45C0u,0x138),depth=F(0x104B45C0u,0x140),width=F(0x104B45C0u,0x13C);
    f32 height=F(0x104B45C0u,0x144),upper=F(0x104B45C0u,0x134),lower=F(0x104B45C0u,0x130);
    S(out,0,depth);S(out,4,y);S(out,8,height);S(out,12,width);S(out,16,lower);S(out,20,upper);
    for (u32 i=24;i<40;i+=4) store<u32>(out+i,0u);
}
VERIFY(0x025F1084,mDoLib_copyViewRecord);

void mDoLib_pos2camera(u32 src,u32 dst) {
    WWHD_FUNC(0x025F1108,void,src,dst);
    u32 game=call<u32>(0x025200D4),view=load<u32>(game+0x5FA4);
    call<void>(0x028E8F64,view+0x144,src,dst);
}
VERIFY(0x025F1108,mDoLib_pos2camera);

u32 mDoLib_cnvind32(u32 x) {
    WWHD_FUNC(0x025F1154,u32,x);
    return (x>>24) | ((x>>8)&0xFF00u) | ((x<<8)&0xFF0000u) | (x<<24);
}
VERIFY(0x025F1154,mDoLib_cnvind32);
u16 mDoLib_cnvind16(u32 x) {
    WWHD_FUNC(0x025F1188,u16,x);
    return (u16)((x>>8)&0xFFu) | (u16)(x<<8);
}
VERIFY(0x025F1188,mDoLib_cnvind16);

void mDoLib_clipper_setup(f32 fov,f32 aspect,f32 nearPlane,f32 farPlane) {
    WWHD_FUNC(0x025F11AC,void,fov,aspect,nearPlane,farPlane);
    S(0x1048D04Cu,0,farPlane);S(0x1048CFF0u,72,fov);S(0x1048CFF0u,76,aspect);
    S(0x1048CFF0u,80,nearPlane);S(0x1048CFF0u,84,farPlane);
    call<void>(0x0283801C,0x1048CFF0u);
    u32 index=(u16)ftoi(fov*F(0x10058FB0u));u32 table=0x104A44F8u+(index>>3)*8;
    f32 sn=F(table),cs=F(table,4);S(0x1048D050u,0,cs/sn);
}
VERIFY(0x025F11AC,mDoLib_clipper_setup);

void mDoLib_static_init() {
    WWHD_FUNC(0x025F124C,void);
    store<u32>(0x1048CFE8u,0u);store<u32>(0x1048CFE0u,0u);store<u32>(0x1048CFECu,0u);store<u32>(0x1048CFE4u,0u);
    call<void>(0x028F026C,0x101F4850u);
    f32 lo=F(0x10058FB8u),hi=F(0x10058FBCu);S(0x1048CFD4u,0,lo);S(0x1048CFD8u,0,hi);
    call<void>(0x028ED6F8,0x1048CFDCu);call<void>(0x028F026C,0x101F485Cu);
    call<void>(0x028EAB2C,0x1048CFDDu);call<void>(0x028F026C,0x101F4868u);
    store<u32>(0x1048CFF0u+88,0x10058F80u);call<void>(0x02838000,0x1048CFF0u);
}
VERIFY(0x025F124C,mDoLib_static_init);
