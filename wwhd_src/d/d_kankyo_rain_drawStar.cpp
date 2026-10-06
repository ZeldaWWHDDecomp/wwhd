#include "d_kankyo_rain_private.h"
namespace d_kankyo_rain_cpp {
static void drawStar() {
    WWHD_FUNC(0x02574144,void);
    u32 env=environment(),packet=ld(environment()+0xA60),camera=ld(gameInfo()+0x5AF8);
    gabi::Local<ProjectParams_l> params,starParams;
    gabi::Local<Matrix_l> view,rotation;
    gabi::Local<Vec_l> reference,position,projected,input,output;
    gabi::Local<Quad_l> triangle;
    gabi::call(0x02563DD8,params.get());
    if(!ld(0x10477588)) {
        gabi::store<u32>(0x10477588,1);
        static constexpr s32 angles[16][3]={{0x32C8,0x2904,-16000},{0x24B8,0x2648,-12646},{0x27D8,0x2E18,-13525},{0x283C,0x348A,-13525},{0x3A98,0x47E0,-16162},{0x30D4,0x4D58,-15000},{0x23DB,0x4330,-14404},{0x251C,0x2648,-12646},{-7421,0x791D,0x496E},{-10937,0x6D60,0x3A98},{-10000,0x6146,0x47E0},{-9400,0x57E4,0x3E1C},{-9179,0x5334,0x37DC},{-10300,0x55F0,0x5208},{-16000,0x639C,0x4E20},{0,0x7530,0x4A38}};
        for(u32 i=0;i<16;i++)gabi::call(0x0201A478,0x10477520+i*6,angles[i][0],angles[i][1],angles[i][2]);
    }
    if(!gabi::load<s16>(packet+0xCC))return;
    if(!ld(gameInfo()+0x5FA4))return;
    gabi::call(0x028E91EC,ld(gameInfo()+0x5FA4)+0x1E4,view.get());
    gabi::call(0x025F1018,env+0xB44,reference.get(),params.get());
    gabi::call(0x028E98C0,rotation.get(),90,gabi::fmuls_ppc((f32)ld(packet+0xD0),lf(0x1004FB7C)));
    gabi::call(0x028E9108,view.get(),rotation.get(),view.get());
    f32 cameraY=lf(camera+0xE0),cameraX=lf(camera+0xDC),zero=lf(0x1004F528),cameraZ=lf(camera+0xE4),fade=zero;
    u32 info=gameInfo();f32 one=lf(0x1004F550);
    if(ld(info+0x5FA4)) {
        fade=lf(ld(gameInfo()+0x5FA4)+0xD4)/lf(0x1004F6EC);
        if(!(fade<one))fade=one;
        fade=gabi::fsubs_ppc(one,fade);
    }
    f32 sizeBase=lf(0x1004FC98),half=lf(0x1004F57C);
    f32 radius=gabi::fmuls_ppc(gabi::fnmsubs(lf(0x1004F740),fade,sizeBase),half);
    f32 bottom=-gabi::fmuls_ppc(radius,half);
    for(u32 vertex=0;vertex<3;vertex++) {
        input->x=vertex==0?zero:vertex==1?radius:-radius;
        input->y=vertex==0?radius:bottom;input->z=zero;
        gabi::call(0x028E8F64,view.get(),input.get(),output.get());
        triangle->vertices[vertex].x=gabi::fadds_ppc(cameraX,(f32)output->x);
        triangle->vertices[vertex].y=gabi::fadds_ppc(cameraY,(f32)output->y);
        triangle->vertices[vertex].z=gabi::fadds_ppc(cameraZ,(f32)output->z);
    }
    f32 spiral=zero;
    f32 distanceLimit=gabi::fmadds(lf(0x1004F6BC),gabi::fmuls_ppc(fade,fade),lf(0x1004F55C));
    f32 minCoord=lf(0x1004FCAC),spiralShrink=lf(0x1004FCB0),two=lf(0x1004F52C),maxX=lf(0x1004F634),maxY=lf(0x1004F7CC),distanceFactor=lf(0x1004F800),tableScale=lf(0x1004F54C);
    u32 phase=0,phaseStep=0;
    for(s32 i=0;i<gabi::load<s16>(packet+0xCC);i++) {
        u32 flags=packet+0x49A608;
        gabi::store<u8>(packet+0xD4+(u32)i,0);
        if(i<16)gabi::store<u8>(flags+(u32)i,i==6||i==8?0:1);
        else {u32 kind=(u32)i&7;gabi::store<u8>(flags+(u32)i,kind<2?2:kind);}
        f32 x,y,z,scale;
        if(i<16) {
            u32 angle=0x10477520+(u32)i*6;
            x=(f32)gabi::load<s16>(angle);y=(f32)gabi::load<s16>(angle+2);z=(f32)gabi::load<s16>(angle+4);
            scale=gabi::fmuls_ppc(lf(packet+0xC0),lf(i>7?0x1004FCB4:0x1004F6D8));
            scale=gabi::fnmsubs(fade,gabi::fmuls_ppc(scale,half),scale);
        } else {
            scale=gabi::fmadds(gabi::fmuls_ppc(gabi::fmuls_ppc((f32)(i&63),lf(0x1004FCB8)),lf(0x1004F640)),lf(packet+0xC0),sizeBase);
            if((i&15)==2) {scale=gabi::fmuls_ppc(scale,lf(0x1004FB8C));gabi::store<u8>(flags+(u32)i,2);}
            u32 table=0x104A44F8+(((phase-0x8000)&0xFFFFu)>>3)*8;
            f32 shrink=gabi::fnmsubs(spiral,spiralShrink,one);
            f32 px=gabi::fmuls_ppc(gabi::fmuls_ppc(lf(table),tableScale),shrink),pz=gabi::fmuls_ppc(gabi::fmuls_ppc(lf(table+4),tableScale),shrink);
            f32 term=spiral/two;term=gabi::fmuls_ppc(term,gabi::fmuls_ppc(term,term));
            f32 increment=gabi::fmadds(term,distanceFactor,one);
            y=gabi::fadds_ppc(spiral,lf(0x1004FB84));
            spiral=gabi::fadds_ppc(spiral,increment);phase+=phaseStep;phaseStep+=2500;
            x=-px;z=pz;
            if(spiral>two)spiral=gabi::fmuls_ppc((f32)i/lf(0x1004F540),lf(0x1004F6D4));
        }
        gabi::call(0x02563DD8,starParams.get());
        position->x=gabi::fadds_ppc(cameraX,x);position->y=gabi::fadds_ppc(cameraY,y);position->z=gabi::fadds_ppc(cameraZ,z);
        gabi::call(0x025F1018,position.get(),projected.get(),starParams.get());
        f32 squared=gabi::call<f32>(0x028E8DE8,reference.get(),projected.get());
        f64 distance=gabi::call<f64>(0x028F4384,(f64)squared);
        f32 screenX=(f32)projected->x,screenY=(f32)projected->y;
        if(!(screenX>minCoord&&screenX<maxX&&screenY>minCoord&&screenY<maxY&&distance>distanceLimit))continue;
        u32 geometry=packet+0x8A8+(u32)i*0x968;
        u32 selected=ld(geometry+0x950),slot=geometry+selected*596;
        u32 buffer=ld(slot),end=buffer+32;
        if(buffer<end)for(u32 cursor=buffer;cursor<end;cursor+=32)for(u32 k=0;k<32;k+=4)gabi::store<u32>((cursor&~31u)+k,0);
        selected=ld(geometry+0x950);slot=geometry+selected*596;buffer=ld(slot);
        for(u32 vertex=0;vertex<3;vertex++)for(u32 axis=0;axis<3;axis++) {
            f32 origin=axis==0?cameraX:axis==1?cameraY:cameraZ;
            f32 offset=gabi::fmuls_ppc(gabi::fsubs_ppc(lf(gabi::ea(triangle.get())+vertex*12+axis*4),origin),scale);
            sf(buffer+vertex*12+axis*4,gabi::fadds_ppc(lf(gabi::ea(position.get())+axis*4),offset));
        }
        selected=ld(geometry+0x950);slot=geometry+selected*596;buffer=ld(slot+0x4A8);end=buffer+32;
        if(buffer<end)for(u32 cursor=buffer;cursor<end;cursor+=32)for(u32 k=0;k<32;k+=4)gabi::store<u32>((cursor&~31u)+k,0);
        selected=ld(geometry+0x950);slot=geometry+selected*596;buffer=ld(slot+0x4A8);
        for(u32 vertex=0;vertex<3;vertex++)for(u32 axis=0;axis<3;axis++) {
            f32 origin=axis==0?cameraX:axis==1?cameraY:cameraZ;
            f32 offset=gabi::fmuls_ppc(gabi::fsubs_ppc(lf(gabi::ea(triangle.get())+vertex*12+axis*4),origin),scale);
            sf(buffer+vertex*12+axis*4,gabi::fsubs_ppc(lf(gabi::ea(position.get())+axis*4),offset));
        }
        selected=ld(geometry+0x950);slot=geometry+selected*596;
        for(u32 face=0;face<2;face++) {gabi::call(0x027B5E94,slot+4,0,ld(slot+0x150));slot+=0x4A8;}
        gabi::store<u32>(geometry+0x950,ld(geometry+0x950)==0?1:0);
        gabi::store<u8>(packet+0xD4+(u32)i,1);
    }
    gabi::call(0x0258115C,packet);
}
VERIFY(0x02574144,drawStar);
}
