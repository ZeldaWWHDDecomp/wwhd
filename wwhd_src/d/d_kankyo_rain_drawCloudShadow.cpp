#include "d_kankyo_rain_private.h"
namespace d_kankyo_rain_cpp {
static void drawCloudShadow() {
    WWHD_FUNC(0x0257556C,void);
    u32 colors=environment(),packet=ld(environment()+0xA84);
    gabi::call(0x02578348);
    if((s32)ld(packet+0x9C)<=0) return;
    if(!ld(gameInfo()+0x5FA4)) return;
    gabi::Local<Matrix_l> cameraMatrix,rotation;
    gabi::Local<be<u32>> color0,color1;
    gabi::Local<ColorF_l> floatColor;
    gabi::Local<Vec_l> offset,transformed;
    gabi::Local<Quad_l> quad;
    gabi::call(0x028E91EC,ld(gameInfo()+0x5FA4)+0x1E4,cameraMatrix.get());
    u32 rgb=0xB70;
    if(gabi::load<u8>(environment()+0xA7D)==3 || gabi::load<u8>(environment()+0xA7D)==4) rgb=0xB88;
    // Alpha remains the native stack byte until the per-particle update below.
    for(u32 c=0;c<3;c++) gabi::store<u8>(gabi::ea(color0.get())+c,gabi::load<u8>(colors+rgb+c));
    for(u32 c=0;c<3;c++) gabi::store<u8>(gabi::ea(color1.get())+c,gabi::load<u8>(colors+rgb+c));
    gabi::call(0x0255F84C);
    gabi::call(0x028E98C0,rotation.get(),90,gabi::fmuls_ppc(lf(packet+0x98C),lf(0x1004FB7C)));
    gabi::call(0x028E9108,cameraMatrix.get(),rotation.get(),cameraMatrix.get());
    s32 index=0,drawn=0;
    u32 geometryOffset=0,uniformOffset=0;
    f32 zero=lf(0x1004F528),one=lf(0x1004F550),threshold=lf(0x1004F5B8),alphaScale=lf(0x1004F5AC);
    for(;index<(s32)ld(packet+0x9C);index++) {
        u32 effect=packet+0xA0+(u32)index*76;
        f32 alpha=lf(effect+0x40),radius=lf(effect+0x44);
        if(alpha<threshold) continue;
        gabi::store<u8>(gabi::ea(color0.get())+3,(u8)gabi::ftoi(gabi::fmuls_ppc(alpha,alphaScale)));
        gabi::call(0x0257015C,floatColor.get(),color0.get());
        gabi::call(0x027FC500,packet+0x9818+uniformOffset,floatColor.get(),0);
        gabi::call(0x0257015C,floatColor.get(),color1.get());
        gabi::call(0x027FC500,packet+0x9818+uniformOffset,floatColor.get(),1);
        f32 worldX=gabi::fadds_ppc(lf(effect+0x10),lf(effect+4));
        f32 worldY=gabi::fadds_ppc(lf(effect+0x14),lf(effect+8));
        f32 worldZ=gabi::fadds_ppc(lf(effect+0x18),lf(effect+12));
        f32 negative=-radius;
        for(u32 corner=0;corner<4;corner++) {
            offset->x=corner==0||corner==3?negative:radius;
            offset->y=corner<2?radius:negative;
            offset->z=zero;
            gabi::call(0x028E8F64,cameraMatrix.get(),offset.get(),transformed.get());
            quad->vertices[corner].x=gabi::fadds_ppc(worldX,(f32)transformed->x);
            quad->vertices[corner].y=gabi::fadds_ppc(worldY,(f32)transformed->y);
            quad->vertices[corner].z=gabi::fadds_ppc(worldZ,(f32)transformed->z);
        }
        u32 geometry=packet+0x994+geometryOffset,selected=ld(geometry+0x4A8);
        u32 buffer=ld(geometry+selected*596),end=buffer+64;
        // The HD renderer clears two cache lines before filling four 20-byte vertices.
        for(u32 cursor=buffer;cursor<end;cursor+=32)
            for(u32 word=0;word<32;word+=4) gabi::store<u32>((cursor&~31u)+word,0);
        selected=ld(geometry+0x4A8);buffer=ld(geometry+selected*596);
        for(u32 corner=0;corner<4;corner++) {
            for(u32 component=0;component<12;component+=4)
                gabi::store<u32>(buffer+corner*20+component,ld(gabi::ea(quad.get())+corner*12+component));
            sf(buffer+corner*20+12,corner==0||corner==3?zero:one);
            sf(buffer+corner*20+16,corner<2?zero:one);
        }
        selected=ld(geometry+0x4A8);u32 slot=geometry+selected*596;
        gabi::call(0x027B5E94,slot+4,0,ld(slot+0x150));
        gabi::store<u32>(geometry+0x4A8,ld(geometry+0x4A8)==0?1:0);
        geometryOffset+=0x4C0;uniformOffset+=0x370;drawn++;
    }
    gabi::call(0x025820F4,packet,drawn);
}
VERIFY(0x0257556C,drawCloudShadow);
}
