#include "d_kankyo_rain_private.h"
namespace d_kankyo_rain_cpp {
static void drawPoison() {
    WWHD_FUNC(0x025720D8,void);
    environment();u32 packet=ld(environment()+0xA6C);
    if(!ld(gameInfo()+0x5FA4)) return;
    gabi::Local<Matrix_l> matrix,rotation;
    gabi::Local<be<u32>> color0,color1;
    gabi::Local<ColorF_l> floatColor;
    gabi::Local<Vec_l> offset,transformed;
    gabi::Local<Quad_l> quad;
    gabi::call(0x028E91EC,ld(gameInfo()+0x5FA4)+0x1E4,matrix.get());
    const u8 initial0[3]={45,136,170},initial1[3]={109,60,205};
    for(u32 c=0;c<3;c++) {gabi::store<u8>(gabi::ea(color0.get())+c,initial0[c]);gabi::store<u8>(gabi::ea(color1.get())+c,initial1[c]);}
    gabi::call(0x0255F84C);
    gabi::call(0x028E98C0,rotation.get(),90,gabi::fmuls_ppc(lf(packet+0xBC38),lf(0x1004FB7C)));
    gabi::call(0x028E9108,matrix.get(),rotation.get(),matrix.get());
    s32 count=(s32)ld(environment()+0xA68);
    f32 zero=lf(0x1004F528),one=lf(0x1004F550),threshold=lf(0x1004F5B8),alphaScale=lf(0x1004F5AC);
    for(s32 i=0;i<count;i++) {
        u32 particle=packet+0x98+(u32)i*48;
        gabi::store<u8>(packet+0xBC3C+(u32)i,0);
        f32 alpha=lf(particle+0x24),radius=lf(particle+0x28);
        if(alpha>threshold) {
            f32 phase=(f32)(s32)((u32)i*4000);
            f32 counter=(f32)(s32)ld(ld(environment()+0xA6C)+0xBC30);
            u16 angle=(u16)gabi::ftoi(gabi::fmadds(counter,lf(0x1004F7CC),phase));
            f32 cosine=std::fabs(lf(0x104A44F8+(angle&0xFFF8)+4));
            f32 squared=gabi::fmuls_ppc(cosine,cosine);
            f32 common=gabi::fmuls_ppc(squared,lf(0x1004FB90));
            f32 c0[3]={gabi::fadds_ppc(lf(0x1004FC88),common),gabi::fadds_ppc(lf(0x1004FC84),common),gabi::fmadds(squared,lf(0x1004FC7C),lf(0x1004FC80))};
            f32 c1[3]={gabi::fmadds(squared,lf(0x1004FC74),lf(0x1004FC78)),gabi::fmadds(squared,lf(0x1004FC6C),lf(0x1004FC70)),gabi::fmadds(squared,lf(0x1004FB90),alphaScale)};
            for(u32 c=0;c<3;c++) {gabi::store<u8>(gabi::ea(color0.get())+c,(u8)gabi::ftoi(c0[c]));gabi::store<u8>(gabi::ea(color1.get())+c,(u8)gabi::ftoi(c1[c]));}
            gabi::store<u8>(gabi::ea(color0.get())+3,(u8)gabi::ftoi(gabi::fmuls_ppc(lf(particle+0x24),alphaScale)));
            f32 world[3];for(u32 c=0;c<3;c++) world[c]=gabi::fadds_ppc(lf(packet+0xBC18+c*4),lf(particle+4+c*4));
            f32 negative=-radius;
            for(u32 corner=0;corner<4;corner++) {
                offset->x=corner==0||corner==3?negative:radius;offset->y=corner<2?radius:negative;offset->z=zero;
                gabi::call(0x028E8F64,matrix.get(),offset.get(),transformed.get());
                for(u32 c=0;c<3;c++) sf(gabi::ea(quad.get())+corner*12+c*4,gabi::fadds_ppc(world[c],lf(gabi::ea(transformed.get())+c*4)));
            }
            gabi::store<u8>(packet+0xBC3C+(u32)i,1);
            gabi::call(0x0257015C,floatColor.get(),color0.get());
            gabi::call(0x027FC500,packet+0x134E28+(u32)i*0x370,floatColor.get(),0);
            gabi::call(0x0257015C,floatColor.get(),color1.get());
            gabi::call(0x027FC500,packet+0x134E28+(u32)i*0x370,floatColor.get(),1);
            u32 geometry=packet+0xC028+(u32)i*0x4C0,selected=ld(geometry+0x4A8),buffer=ld(geometry+selected*596),end=buffer+64;
            for(u32 cursor=buffer;cursor<end;cursor+=32)
                for(u32 word=0;word<32;word+=4) gabi::store<u32>((cursor&~31u)+word,0);
            selected=ld(geometry+0x4A8);buffer=ld(geometry+selected*596);
            for(u32 corner=0;corner<4;corner++) {
                for(u32 c=0;c<12;c+=4) gabi::store<u32>(buffer+corner*20+c,ld(gabi::ea(quad.get())+corner*12+c));
                sf(buffer+corner*20+12,corner==0||corner==3?zero:one);sf(buffer+corner*20+16,corner<2?zero:one);
            }
            selected=ld(geometry+0x4A8);u32 slot=geometry+selected*596;
            gabi::call(0x027B5E94,slot+4,0,ld(slot+0x150));
            gabi::store<u32>(geometry+0x4A8,ld(geometry+0x4A8)==0?1:0);
        }
        count=(s32)ld(environment()+0xA68);
    }
    gabi::call(0x025817B0,packet);
}
VERIFY(0x025720D8,drawPoison);
}
