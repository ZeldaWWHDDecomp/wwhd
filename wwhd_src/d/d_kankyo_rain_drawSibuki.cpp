#include "d_kankyo_rain_private.h"
namespace d_kankyo_rain_cpp {
static void drawSibuki() {
    WWHD_FUNC(0x02571B1C,void);
    u32 camera=ld(gameInfo()+0x5AF8),packet=ld(environment()+0xA44);
    if(ld(environment()+0xA4C)) return;
    if(!ld(gameInfo()+0x5FA4)) return;
    gabi::Local<Matrix_l> inverse;
    gabi::call(0x028E91EC,ld(gameInfo()+0x5FA4)+0x1E4,inverse.get());
    f32 zero=lf(0x1004F528),target=lf(0x1004F52C);
    if(gabi::load<u8>(packet+0x379C)&1) target=zero;
    gabi::call(0x0200ECD4,packet+0x378C,target,lf(0x1004F554),lf(0x1004F728),lf(0x1004F58C));
    gabi::Local<Vec_l> eye,dir;
    gabi::call(0x02563F68,camera,eye.get(),lf(0x1004F5B0),lf(0x1004F53C));
    gabi::call(0x02563F64,camera+0xDC,camera+0xE8,dir.get());
    f32 direction=gabi::fadds_ppc((f32)dir->y,lf(0x1004F844));
    dir->y=direction;
    f32 one=lf(0x1004F550),fade=one;
    if(direction>zero) fade=direction<lf(0x1004F57C)?gabi::fsubs_ppc(one,gabi::fadds_ppc(direction,direction)):zero;
    u8 alpha=(u8)gabi::ftoi(gabi::fmuls_ppc(lf(packet+0x378C),fade));
    if(!alpha) return;
    gabi::Local<be<u32>> color;
    gabi::store<u8>(gabi::ea(color.get()),180);gabi::store<u8>(gabi::ea(color.get())+1,200);
    gabi::store<u8>(gabi::ea(color.get())+2,220);gabi::store<u8>(gabi::ea(color.get())+3,alpha);
    gabi::Local<ColorF_l> floatColor;
    gabi::call(0x0257015C,floatColor.get(),color.get());
    gabi::call(0x027FC500,packet+0x1AC580,floatColor.get(),0);
    gabi::call(0x027FC500,packet+0x1AC580,floatColor.get(),1);
    f32 spread=lf(0x1004F554);
    if(ld(gameInfo()+0x5FA4)) {
        f32 ratio=(f32)((f64)lf(ld(gameInfo()+0x5FA4)+0xD4)/(f64)lf(0x1004F6D4));
        if(!(ratio<one)) ratio=one;
        spread=gabi::fsubs_ppc(one,ratio);
    }
    u32 geometry=packet+0x12C69C;
    gabi::Local<Quad_l> quad;
    s32 i=0;
    if(i>=((s32)ld(environment()+0xA40)>>1)) {gabi::call(0x0257FB4C,packet);return;}
    f32 randomSize=lf(0x1004F6B8),yRandomRange=lf(0x1004F7D0),positionRange=lf(0x1004FC68),positionScale=lf(0x1004FC64);
    do {
        f32 size=gabi::fmadds(gabi::call<f32>(0x020198D8,randomSize),spread,randomSize);
        f32 dx=gabi::fmuls_ppc(gabi::call<f32>(0x02019918,positionRange),positionScale);
        f32 dy=gabi::call<f32>(0x02019918,yRandomRange);
        f32 dz=gabi::call<f32>(0x02019918,positionRange);
        f32 x=gabi::fadds_ppc((f32)eye->x,dx),y=gabi::fadds_ppc((f32)eye->y,dy);
        f32 z=gabi::fmadds(dz,positionScale,(f32)eye->z);
        f32 left=gabi::fsubs_ppc(x,size),right=gabi::fadds_ppc(x,size);
        f32 bottom=gabi::fsubs_ppc(z,size),top=gabi::fadds_ppc(z,size);
        quad->vertices[0].x=left;quad->vertices[0].y=y;quad->vertices[0].z=bottom;
        quad->vertices[1].x=right;quad->vertices[1].y=y;quad->vertices[1].z=bottom;
        quad->vertices[2].x=right;quad->vertices[2].y=y;quad->vertices[2].z=top;
        quad->vertices[3].x=left;quad->vertices[3].y=y;quad->vertices[3].z=top;
        u32 selected=ld(geometry+0x4A8),buffer=ld(geometry+selected*596),end=buffer+64;
        for(u32 cursor=buffer;cursor<end;cursor+=32)
            for(u32 word=0;word<32;word+=4) gabi::store<u32>((cursor&~31u)+word,0);
        selected=ld(geometry+0x4A8);buffer=ld(geometry+selected*596);
        f32 texOne=lf(0x1004F7FC);
        for(u32 corner=0;corner<4;corner++) {
            for(u32 component=0;component<12;component+=4)
                gabi::store<u32>(buffer+corner*20+component,ld(gabi::ea(quad.get())+corner*12+component));
            sf(buffer+corner*20+12,corner==0||corner==3?zero:texOne);
            sf(buffer+corner*20+16,corner<2?zero:texOne);
        }
        selected=ld(geometry+0x4A8);u32 slot=geometry+selected*596;
        gabi::call(0x027B5E94,slot+4,0,ld(slot+0x150));
        gabi::store<u32>(geometry+0x4A8,ld(geometry+0x4A8)==0?1:0);
        geometry+=0x4C0;i++;
    } while(i<((s32)ld(environment()+0xA40)>>1));
    gabi::call(0x0257FB4C,packet);
}
VERIFY(0x02571B1C,drawSibuki);
}
