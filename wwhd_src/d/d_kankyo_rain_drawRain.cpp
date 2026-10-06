#include "d_kankyo_rain_private.h"
namespace d_kankyo_rain_cpp {
static void drawRain() {
    WWHD_FUNC(0x025712A8,void);
    u32 packet=ld(environment()+0xA44),camera=ld(gameInfo()+0x5AF8);
    gabi::Local<Vec_l> wind,offset,center,scaled,delta,input,output;
    gabi::Local<Matrix_l> view,rotation;
    gabi::Local<be<u32>> primary,secondary;
    gabi::Local<ColorF_l> color;
    gabi::Local<Quad_l> quad;
    gabi::call(0x0257DB28,wind.get());
    if(ld(environment()+0xA4C))return;
    f32 zero=lf(0x1004F528);
    offset->x=zero;offset->y=lf(0x1004FC5C);offset->z=zero;
    if(!gabi::load<s16>(packet+0x3758))return;
    *primary=0xFFFFFF07u;*secondary=0x80808007u;
    if(!ld(gameInfo()+0x5FA4))return;
    gabi::call(0x028E91EC,ld(gameInfo()+0x5FA4)+0x1E4,view.get());
    gabi::call(0x028E98C0,rotation.get(),90,gabi::fmuls_ppc((f32)ld(0x101E99F4),lf(0x1004FB7C)));
    gabi::call(0x028E9108,view.get(),rotation.get(),view.get());
    f32 one=lf(0x1004F550),shift=lf(0x1004F6DC),distanceScale=lf(0x1004F7D0),uvDepth=lf(0x1004FB84),widthScale=lf(0x1004F6B8),step=lf(0x1004F624),base=lf(0x1004F5F0);
    for(s32 i=0;i<gabi::load<s16>(packet+0x3758);i++) {
        u32 particle=packet+0xA0+(u32)i*56;
        gabi::store<u8>(packet+0x379D+(u32)i,0);
        if(!(lf(particle+40)>zero))continue;
        center->x=gabi::fadds_ppc(lf(particle+16),lf(particle+4));
        center->y=gabi::fadds_ppc(lf(particle+20),lf(particle+8));
        center->z=gabi::fadds_ppc(lf(particle+24),lf(particle+12));
        gabi::call(0x0201AE48,packet+0x3778,scaled.get(),lf(packet+0x3784));
        gabi::call(0x0201AE48,scaled.get(),delta.get(),lf(0x1004FC60));
        gabi::call(0x028E8D88,center.get(),delta.get(),center.get());
        f32 squared=gabi::call<f32>(0x028E8DE8,center.get(),camera+0xDC);
        f32 ratio=(f32)(gabi::call<f64>(0x028F4384,(f64)squared)/(f64)distanceScale);
        if(ratio>one)ratio=one;
        gabi::store<u8>(gabi::ea(primary.get())+3,(u8)gabi::ftoi(gabi::fmuls_ppc(gabi::fmuls_ppc(lf(0x1004F808),lf(particle+40)),gabi::fsubs_ppc(one,ratio))));
        gabi::store<u8>(packet+0x379D+(u32)i,1);
        gabi::call(0x0257015C,color.get(),primary.get());
        u32 material=packet+0x176A20+(u32)i*0x370;
        gabi::call(0x027FC500,material,color.get(),0);
        gabi::call(0x0257015C,color.get(),secondary.get());
        gabi::call(0x027FC500,material,color.get(),1);
        squared=gabi::call<f32>(0x028E8DE8,center.get(),camera+0xDC);
        ratio=gabi::fadds_ppc((f32)(gabi::call<f64>(0x028F4384,(f64)squared)/(f64)distanceScale),lf(0x1004F588));
        if(ratio>one)ratio=one;
        f32 speed=lf(packet+0x3784);
        f32 x=gabi::fadds_ppc(gabi::fmadds(gabi::fmuls_ppc(lf(packet+0x3778),speed),widthScale,(f32)wind->x),gabi::fmadds((f32)(i&7),shift,(f32)offset->x));
        f32 z=gabi::fadds_ppc(gabi::fmadds(gabi::fmuls_ppc(lf(packet+0x3780),speed),widthScale,(f32)wind->z),gabi::fmadds((f32)(i&3),shift,(f32)offset->z));
        f32 y=gabi::fadds_ppc(gabi::fmadds(lf(packet+0x377C),speed,(f32)wind->y),(f32)offset->y);
        f32 thickness=gabi::fmadds(lf(0x1004F920),ratio,lf(0x1004F730));
        f32 dx=gabi::fmuls_ppc(x,thickness),dy=gabi::fmuls_ppc(y,thickness),dz=gabi::fmuls_ppc(z,thickness);
        f32 halfWidth=gabi::fadds_ppc(((f32)i/lf(0x1004F54C)),lf(0x1004F7F0));
        for(u32 corner=0;corner<4;corner++) {
            input->x=corner==0||corner==3?halfWidth:-halfWidth;input->y=zero;input->z=zero;
            gabi::call(0x028E8F64,view.get(),input.get(),output.get());
            f32 px=gabi::fadds_ppc((f32)center->x,(f32)output->x),py=gabi::fadds_ppc((f32)center->y,(f32)output->y),pz=gabi::fadds_ppc((f32)center->z,(f32)output->z);
            quad->vertices[corner].x=corner<2?gabi::fsubs_ppc(px,dx):px;
            quad->vertices[corner].y=corner<2?gabi::fsubs_ppc(py,dy):py;
            quad->vertices[corner].z=corner<2?gabi::fsubs_ppc(pz,dz):pz;
        }
        f32 angle=lf(0x1004F574);
        for(u32 copy=0;copy<4;copy++) {
            if(!ld(0x10477580)) {
                f32 values[12]={angle,zero,zero,zero,angle,angle,angle,base,angle,uvDepth,step,uvDepth};
                for(u32 k=0;k<12;k++)sf(0x10477420+k*4,values[k]);
                gabi::store<u32>(0x10477580,1);
            }
            u32 geometry=packet+0x389C+(u32)i*0x1300+copy*0x4C0;
            u32 selected=ld(geometry+0x4A8),buffer=ld(geometry+selected*596),end=buffer+64;
            if(buffer<end)for(u32 cursor=buffer;cursor<end;cursor+=32)for(u32 k=0;k<32;k+=4)gabi::store<u32>((cursor&~31u)+k,0);
            selected=ld(geometry+0x4A8);buffer=ld(geometry+selected*596);
            for(u32 corner=0;corner<4;corner++) {
                for(u32 axis=0;axis<3;axis++)sf(buffer+corner*20+axis*4,gabi::fadds_ppc(lf(gabi::ea(quad.get())+corner*12+axis*4),lf(0x10477420+copy*12+axis*4)));
                sf(buffer+corner*20+12,corner==0||corner==3?zero:one);
                sf(buffer+corner*20+16,corner<2?zero:one);
            }
            selected=ld(geometry+0x4A8);u32 slot=geometry+selected*596;
            gabi::call(0x027B5E94,slot+4,0,ld(slot+0x150));
            gabi::store<u32>(geometry+0x4A8,ld(geometry+0x4A8)==0?1:0);
        }
    }
    gabi::call(0x0257F804,packet);
}
VERIFY(0x025712A8,drawRain);
}
