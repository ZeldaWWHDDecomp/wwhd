#include "d_kankyo_rain_private.h"
namespace d_kankyo_rain_cpp {
static void drawHousi() {
    WWHD_FUNC(0x025727BC,void);
    u32 packet=ld(environment()+0xA78);
    if(!gabi::load<s16>(packet+0x5E68)) return;
    gabi::Local<be<u32>> color0,color1;
    gabi::Local<ColorF_l> floatColor;
    gabi::Local<Matrix_l> matrix,rotation;
    gabi::Local<Vec_l> offset,transformed;
    gabi::Local<Quad_l> quad;
    *color0=0xE5FFC8FF;*color1=0x43D2CAFF;
    f32 alphaScale=lf(0x1004F5AC),size=lf(0x1004F800);
    if(!ld(gameInfo()+0x5FA4)) return;
    gabi::call(0x028E91EC,ld(gameInfo()+0x5FA4)+0x1E4,matrix.get());
    f32 rotationScale=lf(0x1004FB7C);
    for(u32 pass=0;pass<2;pass++) {
        for(u32 i=0;i<300;i++) gabi::store<u8>(packet+0x5E70+pass*300+i,0);
        gabi::call(0x028E98C0,rotation.get(),90,gabi::fmuls_ppc((f32)ld(0x101E9A24),rotationScale));
        gabi::call(0x028E9108,matrix.get(),rotation.get(),matrix.get());
        s16 count=gabi::load<s16>(packet+0x5E68);
        f32 phaseScale=lf(0x1004F6B8),zero=lf(0x1004F528),opacityScale=lf(0x1004F6EC),texOne=lf(0x1004F7FC),heightRange=lf(0x1004F6D4),deform=lf(0x1004FC8C),heightMax=lf(0x1004F558),one=lf(0x1004F550),negativeSize=-size;
        for(s32 i=0;i<count;i++) {
            u32 game=gameInfo(),particle=packet+0x9C+(u32)i*80;
            f32 worldX=gabi::fadds_ppc(lf(particle+0x10),lf(particle+4));
            f32 worldY=gabi::fadds_ppc(lf(particle+0x14),lf(particle+8));
            f32 worldZ=gabi::fadds_ppc(lf(particle+0x18),lf(particle+12));
            u32 player=ld(game+0x5B2C);
            if(pass==1) {
                if(i==0) for(u32 c=0;c<3;c++) {gabi::store<u8>(gabi::ea(color0.get())+c,0);gabi::store<u8>(gabi::ea(color1.get())+c,0);}
                f32 playerY=lf(player+0x318),alpha=lf(particle+0x40);
                if(worldY>gabi::fadds_ppc(playerY,heightMax)||worldY<gabi::fsubs_ppc(playerY,heightRange)||!(alpha>zero)) {count=gabi::load<s16>(packet+0x5E68);continue;}
                f32 ratio=(f32)((f64)gabi::fsubs_ppc(worldY,playerY)/(f64)heightMax);
                f32 faded=gabi::fmuls_ppc(gabi::fmuls_ppc(alpha,opacityScale),gabi::fsubs_ppc(one,ratio));
                gabi::store<u8>(gabi::ea(color0.get())+3,(u8)gabi::ftoi(faded));
                worldY=gabi::fsubs_ppc(lf(player+0x318),heightRange);
            } else gabi::store<u8>(gabi::ea(color0.get())+3,(u8)gabi::ftoi(gabi::fmuls_ppc(lf(particle+0x40),alphaScale)));
            u32 angleX=gabi::call<u32>(0x02019510,gabi::fmuls_ppc(lf(particle+0x28),phaseScale));
            f32 dx=gabi::fmuls_ppc(lf(0x104A44F8+(angleX&0xFFF8)),deform);
            u32 angleY=gabi::call<u32>(0x02019510,gabi::fmuls_ppc(lf(particle+0x2C),phaseScale));
            f32 dy=gabi::fmuls_ppc(lf(0x104A44F8+(angleY&0xFFF8)),deform);
            f32 cornerX[4]={gabi::fsubs_ppc(size,dy),gabi::fsubs_ppc(dy,size),gabi::fsubs_ppc(dy,size),gabi::fsubs_ppc(size,dy)};
            f32 cornerY[4]={gabi::fsubs_ppc(size,dx),gabi::fadds_ppc(size,dx),gabi::fsubs_ppc(dx,size),gabi::fsubs_ppc(negativeSize,dx)};
            for(u32 c=0;c<4;c++) {
                offset->x=cornerX[c];offset->y=cornerY[c];offset->z=zero;
                gabi::call(0x028E8F64,matrix.get(),offset.get(),transformed.get());
                quad->vertices[c].x=gabi::fadds_ppc(worldX,(f32)transformed->x);quad->vertices[c].y=gabi::fadds_ppc(worldY,(f32)transformed->y);quad->vertices[c].z=gabi::fadds_ppc(worldZ,(f32)transformed->z);
            }
            u32 index=pass*300+(u32)i;
            gabi::store<u8>(packet+0x5E70+index,1);
            gabi::call(0x0257015C,floatColor.get(),color0.get());gabi::call(0x027FC500,packet+0xB82D0+index*0x370,floatColor.get(),0);
            gabi::call(0x0257015C,floatColor.get(),color1.get());gabi::call(0x027FC500,packet+0xB82D0+index*0x370,floatColor.get(),1);
            u32 geometry=packet+0x60CC+index*0x4C0,selected=ld(geometry+0x4A8),buffer=ld(geometry+selected*596),end=buffer+64;
            for(u32 cursor=buffer;cursor<end;cursor+=32) for(u32 word=0;word<32;word+=4) gabi::store<u32>((cursor&~31u)+word,0);
            selected=ld(geometry+0x4A8);buffer=ld(geometry+selected*596);
            for(u32 c=0;c<4;c++) {for(u32 component=0;component<12;component+=4) gabi::store<u32>(buffer+c*20+component,ld(gabi::ea(quad.get())+c*12+component));sf(buffer+c*20+12,c==0||c==3?zero:texOne);sf(buffer+c*20+16,c<2?zero:texOne);}
            selected=ld(geometry+0x4A8);u32 slot=geometry+selected*596;gabi::call(0x027B5E94,slot+4,0,ld(slot+0x150));gabi::store<u32>(geometry+0x4A8,ld(geometry+0x4A8)==0?1:0);
            count=gabi::load<s16>(packet+0x5E68);
        }
    }
    gabi::call(0x02581C34,packet);
}
VERIFY(0x025727BC,drawHousi);
}
