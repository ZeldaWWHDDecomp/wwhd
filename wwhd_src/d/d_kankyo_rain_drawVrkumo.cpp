#include "d_kankyo_rain_private.h"
namespace d_kankyo_rain_cpp {
static void drawVrkumo(u32 drawMtx,u32 color,u32 images) {
    WWHD_FUNC(0x02575B6C,void,drawMtx,color,images);
    // Full native caller workspace: the rendering setup objects extend through4C8.
    gabi::Local<u8[0x4C8]> work;u32 sp=gabi::ea(work.get());
    environment();environment();u32 packet=ld(environment()+0xA94),camera=ld(gameInfo()+0x5AF8);gameInfo();
    auto stage=[&](u32 literal,u32 first,u32 second) {
        gabi::store<u32>(sp+first,literal);gabi::store<u32>(sp+first+4,0x1004F3AC);
        gabi::store<u32>(sp+second,gameInfo()+0x5134);gabi::store<u32>(sp+second+4,0x1004F3AC);
        gabi::call_ptr(ld(ld(sp+first+4)+0x14),sp+first);gabi::call_ptr(ld(ld(sp+first+4)+0x14),sp+first);
        u32 left=ld(sp+first);gabi::call_ptr(ld(ld(sp+second+4)+0x14),sp+second);u32 right=ld(sp+second);
        return left==right||equal_terminated_strings(left,right);
    };
    bool special=stage(0x1004FD08,0x1C0,0x314);
    for(u32 i=0;i<3;i++)gabi::store<u8>(sp+0x1C8+i,gabi::load<u8>(environment()+0xB98+i));
    gabi::call(0x0257015C,sp+0x384,sp+0x1C8);
    gabi::call(0x0274D458,sp+0x188,sp+0x384,lf(environment()+0x10DC));
    for(u32 i=0;i<3;i++)gabi::store<u8>(sp+0x1CC+i,gabi::load<u8>(environment()+0xB9C+i));
    gabi::call(0x0257015C,sp+0x384,sp+0x1CC);
    gabi::call(0x0274D458,sp+0x148,sp+0x384,lf(environment()+0x10E0));
    sf(sp+0xB8,lf(0x10145180));sf(sp+0xC0,lf(0x1014517C));sf(sp+0xB4,lf(0x10145180));sf(sp+0xBC,lf(0x10145180));
    gabi::call(0x02750250,sp+0x1F8);gabi::call(0x02563DD8,sp+0x1D0);
    u32 sun=ld(environment()+0xA34);if(sun)gabi::call(0x025F1018,sun+0x98,sp+0xD0,sp+0x1D0);
    if(!ld(gameInfo()+0x5FA4))return;
    gabi::call(0x028E91EC,ld(gameInfo()+0x5FA4)+0x1E4,sp+0x324);
    sf(sp+0x48,gabi::fmuls_ppc((f32)gabi::load<s16>(camera+0x100),lf(0x1004F5F8)));
    bool farStage=!stage(0x1004FD10,0x1C0,0x31C);
    auto stageInfo=[&]() {u32 game=gameInfo();return gabi::call_ptr<u32>(ld(ld(game+0x5150)+0x15C),game+0x5150);};
    if(farStage)farStage=stageInfo()!=0;
    f32 farPlane=farStage?gabi::fsubs_ppc(lf(stageInfo()+4),lf(0x1004F594)):lf(0x1004FCC8);
    const u32 manager=0x104B45C0;u32 cache=gabi::call<u32>(0x027F29D4,manager),material=ld(ld(packet+0x11D8));
    if(material!=ld(cache+4)) {
        u8 flags=gabi::load<u8>(material);u32 previous=ld(cache);
        if(flags&2){gabi::store<u8>(material,flags&~2);gabi::call(0x027BB9E0,material,0);}
        u32 shader=ld(ld(material+0x7C)+0x28);if(previous!=shader)gabi::call(0x027B9F68,shader);
        if(ld(material+0xC))gabi::call(0xC00060E0,ld(material+4),ld(material+0xC));else gabi::call(0x027BB7CC,material);
        gabi::store<u32>(cache+4,material);gabi::store<u32>(cache,shader);
    }
    for(u32 off=4;off<=64;off+=4)gabi::store<u32>(sp+0x454+off,ld(manager+0x148+off));
    u32 global=ld(0x101F95D0),list=ld(global+0x1024);if(ld(global+0x1020)>1)list+=4;u32 active=ld(list);
    sf(sp+0x30,lf(0x1004F5E8));
    if(active) {
        u32 view;if(!(ld(active+0x50)&0x1000)){view=ld(active+0x4C);if(!view)view=0x104A20FC;}else view=ld(active+0x164);
        gabi::call(0x027389F8,sp+0x394);
        gabi::call(0x0274E04C,sp+0x394,lf(view+0x94),lf(view+0x98),lf(view+0x9C),lf(view+0xAC));
        sf(sp+0x444,lf(view+0xB0));sf(sp+0x448,lf(view+0xB4));gabi::store<u8>(sp+0x394,1);
        if(!ld(0x101FD9F0)){gabi::store<u32>(0x101FD9F0,1);gabi::store<u32>(0x101FDCD4,0x1004F4E4);}
        if(gabi::call_ptr<u32>(ld(ld(view+0x90)+0xC),view)&&view)gabi::call(0x02738AC0,sp+0x394,view+0xB8);
        sf(sp+0x428,lf(sp+0x30));sf(sp+0x42C,lf(0x1004FCCC));gabi::store<u8>(sp+0x394,1);
        u32 matrix=gabi::call<u32>(0x0274D83C,sp+0x394);gabi::call(0x028E8970,matrix,sp+0x458);gabi::call(0x02738A6C,sp+0x394,2);
    }
    u32 setup=packet+0xAFBCC;gabi::call(0x02575ACC,sp+0x498,drawMtx);
    gabi::call(0x027FDA54,setup,0,sp+0x498,sp+0x458,ld(manager+0x148)+0x240);
    u32 state=ld(setup+4);for(u32 off=0;off<16;off+=4)gabi::store<u32>(state+0x1E4+off,ld(0x104A01CC+off));
    sf(ld(setup+4)+0x1F4,lf(0x1004FCD0));sf(ld(setup+4)+0x1F8,lf(0x1004FCD0));gabi::call(0x027FDFF4,setup,0);
    state=ld(setup+4);u32 uniform=state+0x10+ld(state+0x4C)*28,resource=ld(packet+0x11D8),binding=ld(resource+0xC)?ld(resource+0x10):0;
    s16 vertex=gabi::load<s16>(binding+0xC),pixel=gabi::load<s16>(binding+0xE),geometry=gabi::load<s16>(binding+0x10);u32 data=ld(uniform+4),size=ld(uniform+0xC);
    if(pixel!=-1)gabi::call(0xC0006900,pixel,size,data);
    if(vertex!=-1)gabi::call(0xC0006A38,vertex,size,data);
    if(geometry!=-1)gabi::call(0xC00068A8,geometry,size,data);
    gabi::call(0x027FB678,setup+0xC);gabi::call_ptr(ld(ld(setup+0x18)+0x2C),setup+0xC,ld(packet+0x11D8));

    // HD fixes the pass to1 and the pass count to1; retain its three texture layers.
    const u32 stride=0x574E0,selectOffset=stride;
    u32 geometryBase=packet+0x11DC+stride+24;
    for(s32 layer=0;layer<3;layer++) {
        if(layer==0){gabi::store<u32>(sp+0x2E4,23);gabi::call(0x027505B4,sp+0x1F8);}
        gabi::store<u32>(sp+0x204,3);gabi::store<u32>(sp+0x218,0);gabi::store<u32>(sp+0x21C,0);
        gabi::store<u32>(sp+0x208,4);gabi::store<u32>(sp+0x20C,4);gabi::store<u32>(sp+0x210,5);gabi::store<u32>(sp+0x214,5);
        gabi::call(0x027505DC,sp+0x1F8);gabi::store<u8>(sp+0x2D8,1);gabi::call(0x02750520,sp+0x1F8);
        gabi::store<u8>(sp+0x1F8,1);gabi::store<u8>(sp+0x1F9,0);gabi::call(0x02750534,sp+0x1F8);
        if(layer==0){gabi::call(0x028E98C0,sp+0x354,0x5A,gabi::fmuls_ppc(lf(sp+0x48),lf(0x1004FB7C)));gabi::call(0x028E9108,sp+0x324,sp+0x354,sp+0x324);}
        const f32 one=lf(0x1004F550),zero=lf(0x1004F528),half=lf(0x1004F57C),sizeBase=lf(0x1004FCD8),sizeGain=lf(0x1004FCDC),layerFactor=lf(0x1004F590),shapeGain=lf(0x1004F844),pointMin=lf(0x1004F588),pitchLimit=lf(0x1004FCD4);
        sf(sp+0x20,lf(0x1004F5B8));sf(sp+0x44,sizeGain);sf(sp+0x3C,lf(0x1004FCE0));sf(sp+0x2C,lf(0x1004FCEC));sf(sp+0x34,lf(0x1004FCE8));sf(sp+0x38,lf(0x1004FCE4));sf(sp+0x28,lf(0x1004FCF0));sf(sp+0x18,lf(0x1004FCF4));sf(sp+0x4C,gabi::fmuls_ppc((f32)layer,layerFactor));
        for(u32 i=0;i<100;i++) {
            u32 particle=packet+0xA4+i*44;
            if(!(lf(particle+0x20)>lf(sp+0x3C)))continue;
            if(!special&&gabi::load<s16>(camera+0x234)>-6000&&lf(ld(gameInfo()+0x5FA4)+0xD4)>lf(0x1004F6EC)) {
                gabi::call(0x02563DD8,sp+0x198);gabi::call(0x0201AD78,camera+0xDC,sp+0xDC,particle+4);
                for(u32 off=0;off<12;off+=4)gabi::store<u32>(sp+0xE8+off,ld(sp+0xDC+off));
                gabi::call(0x025F1018,sp+0xE8,sp+0xC4,sp+0x198);
                if(!(lf(sp+0xC4)>lf(sp+0x28)&&lf(sp+0xC4)<lf(sp+0x2C)&&lf(sp+0xC8)>lf(sp+0x34)&&lf(sp+0xC8)<lf(sp+0x38)))continue;
            }
            f32 variation=gabi::fmuls_ppc((f32)((layer+i)&15),lf(0x1004FCF8));
            f32 shape=-gabi::fmadds(gabi::fmuls_ppc(variation,variation),variation,-one);
            f32 weather=gabi::fmadds(sizeBase,lf(environment()+0xA90),lf(sp+0x44));
            f32 angularSize=gabi::fmuls_ppc(gabi::fmuls_ppc(shape,weather),lf(particle+0x24));
            u16 angle=(u16)gabi::call<u32>(0x02019510,gabi::fmadds(lf(packet+0x11D4),lf(sp+0x30),(f32)layer));
            f32 sine=lf(0x104A44F8+(angle>>3)*8);
            angularSize=gabi::fmadds(gabi::fmuls_ppc(pointMin,angularSize),gabi::fmuls_ppc(sine,lf(particle+0x24)),angularSize);
            f32 verticalSize=gabi::fmadds(angularSize,lf(particle+0x1C),angularSize),smallOffset=gabi::fmuls_ppc(angularSize,lf(0x1004F544)),wideOffset=gabi::fmuls_ppc(angularSize,shapeGain);
            f32 pitchOffset=zero,yawOffset=zero;
            if(layer==1){switch(i&3){case 0:pitchOffset=smallOffset;yawOffset=wideOffset;break;case 1:pitchOffset=-smallOffset;yawOffset=smallOffset;break;case 2:pitchOffset=wideOffset;yawOffset=-smallOffset;break;case 3:pitchOffset=-wideOffset;yawOffset=wideOffset;break;}}
            else if(layer==2){switch(i&3){case 0:pitchOffset=wideOffset;yawOffset=smallOffset;break;case 1:pitchOffset=-wideOffset;yawOffset=wideOffset;break;case 2:pitchOffset=smallOffset;yawOffset=-wideOffset;break;case 3:pitchOffset=-smallOffset;yawOffset=smallOffset;break;}}
            f32 x=lf(particle+4),y=lf(particle+8),z=lf(particle+12);
            f32 horizontal=gabi::call<f32>(0x028F4384,gabi::fmadds(x,x,gabi::fmuls_ppc(z,z)));
            f32 yaw=gabi::call<f32>(0x028F4D28,x,z);f32 pitch=gabi::call<f32>(0x028F4D28,y,horizontal);
            pitch=gabi::fadds_ppc(pitch,pitchOffset);yaw=gabi::fadds_ppc(yaw,yawOffset);
            f32 q=pitch/lf(0x1004FCFC);if(q>one)q=one;f32 qcube=gabi::fmuls_ppc(q,gabi::fmuls_ppc(q,q));
            f32 pitchCenter=gabi::fmadds(gabi::fmuls_ppc(verticalSize,lf(0x1004FC98)),gabi::fmadds(qcube,lf(0x1004FD04),one),pitch);
            f32 sideSize=gabi::fmuls_ppc(gabi::fmuls_ppc(angularSize,sizeBase),gabi::fmadds(qcube,lf(0x1004FD00),one));
            f32 tallSize=gabi::fmuls_ppc(gabi::fmuls_ppc(angularSize,sizeBase),gabi::fadds_ppc(gabi::fadds_ppc(qcube,qcube),one));
            auto corner=[&](u32 output,f32 cp,f32 cy,bool clamp) {
                if(clamp&&cp>pitchLimit)cp=pitchLimit;
                f32 c=gabi::call<f32>(0x028F4BE0,cp),ss=gabi::call<f32>(0x028F43F8,cy),vx=gabi::fmuls_ppc(c,ss);
                f32 vy=gabi::call<f32>(0x028F43F8,cp);c=gabi::call<f32>(0x028F4BE0,cp);f32 cs=gabi::call<f32>(0x028F4BE0,cy),vz=gabi::fmuls_ppc(c,cs);
                sf(output,gabi::fmuls_ppc(vx,farPlane));sf(output+4,gabi::fmuls_ppc(vy,farPlane));sf(output+8,gabi::fmuls_ppc(vz,farPlane));
            };
            corner(sp+0x78,pitchCenter,gabi::fadds_ppc(yaw,sideSize),true);
            corner(sp+0x84,pitchCenter,gabi::fsubs_ppc(yaw,sideSize),true);
            corner(sp+0x90,pitch,gabi::fsubs_ppc(yaw,tallSize),false);
            corner(sp+0x9C,pitch,gabi::fadds_ppc(yaw,tallSize),false);
            for(u32 off=0;off<48;off+=12)gabi::call(0x028E8D88,sp+0x78+off,camera+0xDC,sp+0x78+off);
            f32 fade=one;
            if(gabi::load<u8>(environment()+0x1092)==0&&sun) {
                gabi::call(0x0201AD78,sp+0x78,sp+0x124,sp+0x84);gabi::call(0x0201AE48,sp+0x124,sp+0x118,half);
                gabi::call(0x0201AD78,sp+0x90,sp+0x13C,sp+0x9C);gabi::call(0x0201AE48,sp+0x13C,sp+0x130,half);
                gabi::call(0x0201AD78,sp+0x118,sp+0x10C,sp+0x130);gabi::call(0x0201AE48,sp+0x10C,sp+0x100,half);
                for(u32 off=0;off<12;off+=4)gabi::store<u32>(sp+0xF4+off,ld(sp+0x100+off));
                gabi::call(0x025F1018,sp+0xF4,sp+0xA8,sp+0x1D0);sf(sp+0xB0,lf(sp+0xD8));gabi::call(0x028E8DE8,sp+0xD0,sp+0xA8);
                fade=gabi::call<f32>(0x028F4384);if(fade<lf(0x1004F558))fade=zero;else if(fade<lf(0x1004F52C))fade=gabi::fsubs_ppc(fade,lf(0x1004F558))/lf(0x1004F558);else fade=one;
            }
            f32 blend=gabi::fsubs_ppc(one,lf(particle+0x24));if(blend>lf(0x1004F588))blend=gabi::fsubs_ppc(blend,lf(sp+0x4C));
            gabi::call(0x0274D380,sp+0x168,sp+0x188,sp+0x148);gabi::call(0x0274D458,sp+0x158,sp+0x168,blend);gabi::call(0x0274D314,sp+0xB4,sp+0x148,sp+0x158);
            sf(sp+0xC0,gabi::fmuls_ppc(lf(particle+0x20),fade));if(!(lf(particle+0x20)>lf(sp+0x20)))continue;
            u32 mat=packet+0xAFFE8+i*0x370; if(layer==0){gabi::call(0x027FC500,mat,sp+0xB4,0);gabi::call(0x027FB678,mat);gabi::call(0x0274D458,sp+0x178,sp+0xB4,lf(0x1004F844));gabi::call(0x027FC500,mat,sp+0x178,1);}
            gabi::call_ptr(ld(ld(mat+0xC)+0x2C),mat,ld(packet+0x11D8));
            resource=ld(packet+0x11D8);u32 textures=ld(resource+0x14)?ld(resource+0x18):0;
            gabi::call(0x027BE53C,packet+0xC5958+layer*0x198,textures+4,-1,0);
            u32 geom=geometryBase,selection=ld(geom+selectOffset),slot=geom+(2*(layer*100+i)+selection)*596;
            u32 buffer=ld(slot);if(buffer<buffer+64){for(u32 cursor=buffer;cursor<buffer+64;cursor+=32)for(u32 word=0;word<32;word+=4)gabi::store<u32>((cursor&~31u)+word,0);slot=geom+(2*(layer*100+i)+ld(geom+selectOffset))*596;}
            buffer=ld(slot);
            for(u32 vertexIndex=0;vertexIndex<4;vertexIndex++) {
                for(u32 component=0;component<3;component++)sf(buffer+vertexIndex*20+component*4,lf(sp+0x78+vertexIndex*12+component*4));
                sf(buffer+vertexIndex*20+12,vertexIndex==1||vertexIndex==2?one:zero);sf(buffer+vertexIndex*20+16,vertexIndex>=2?one:zero);
            }
            gabi::store<u32>(geom+selectOffset,ld(geom+selectOffset)==0);u32 selected=ld(geom+selectOffset)==0;
            gabi::call(0x027BFE5C,geom+(2*(layer*100+i)+selected)*596+0x158);
            gabi::store<u32>(geom+selectOffset,ld(geom+selectOffset)==0);
            u32 draw=0x104B4CFC;if(ld(draw+0xC))gabi::call(0xC0006178,ld(draw+4),ld(draw+0xC),ld(draw),ld(draw+8),0,1);
        }
    }
    gabi::call(0x0255F84C);
    u32 flushBase=packet+0x11DC,selector=flushBase+selectOffset,flush=flushBase+ld(selector)*596+4;
    for(u32 i=0;i<300;i++){gabi::call(0x027B5E94,flush,0,ld(flush+0x14C));flush+=0x4A8;}
    gabi::store<u32>(selector,ld(selector)==0);
    flushBase+=stride+24;selector=flushBase+selectOffset;flush=flushBase+ld(selector)*596+4;
    for(u32 i=0;i<300;i++){gabi::call(0x027B5E94,flush,0,ld(flush+0x14C));flush+=0x4A8;}
    gabi::store<u32>(selector,ld(selector)==0);gabi::call(0x02750370,manager+0x18C);
}
VERIFY(0x02575B6C,drawVrkumo);
}
