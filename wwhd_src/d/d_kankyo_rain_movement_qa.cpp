// Exact coordinator3903f54d bodies; QA extraction only.
#include "d_kankyo_rain_private.h"
namespace d_kankyo_rain_cpp {
static void dKyr_lenzflare_move() {
    WWHD_FUNC(0x02565B70,void);
    u32 sun=ld(environment()+0xA34),lens=ld(environment()+0xA38);
    u32 camera=ld(gameInfo()+0x5AF8);
    gameInfo();
    gabi::Local<ProjectParams_l> projection;
    gabi::Local<Vec_l> eye, sunDirection, projected, center, screenDirection, cameraDirection;
    gabi::call(0x02563DD8,projection.get());
    if(lf(sun+0xC8)<lf(0x1004F5E8)) return;
    f32 distance=lf(0x1004F5EC);
    gabi::call(0x0256401C,camera,eye.get(),distance,distance);
    gabi::call(0x02563F64,eye.get(),sun+0x98,sunDirection.get());
    for(u32 destination=0xA4;destination<=0xBC;destination+=12)
        for(u32 component=0;component<12;component+=4)
            gabi::store<u32>(lens+destination+component,ld(sun+0x98+component));
    gabi::call(0x025F1018,lens+0xA4,projected.get(),projection.get());
    center->x=lf(0x1004F5F0); center->y=lf(0x1004F5F4); center->z=lf(0x1004F528);
    gabi::call(0x02563F64,center.get(),projected.get(),screenDirection.get());
    s32 angle=gabi::call<s32>(0x020195B0,(f32)screenDirection->x,(f32)screenDirection->y);
    sf(lens+0x120,gabi::fmadds((f32)angle,lf(0x1004F5F8),lf(0x1004F5DC)));
    gabi::call(0x02563F64,camera+0xDC,camera+0xE8,cameraDirection.get());
    f32 magnitude=gabi::call<f32>(0x028E8DE8,sunDirection.get(),cameraDirection.get());
    magnitude=gabi::call<f32>(0x028F4384,magnitude);
    f32 size=gabi::fmadds(magnitude,lf(0x1004F5FC),lf(0x1004F600));
    f32 displacement=lf(0x1004F604),step=lf(0x1004F608);
    // HD advances flare spacing by alternating increments.
    for(u32 i=1;i<=6;i++) {
        u32 parity=i%2,destination=lens+0xBC+i*12;
        f32 x=gabi::fmuls_ppc((f32)sunDirection->x,size);
        sf(destination,gabi::fnmsubs(displacement,x,lf(sun+0x98)));
        f32 y=gabi::fmuls_ppc((f32)sunDirection->y,size);
        sf(destination+4,gabi::fnmsubs(displacement,y,lf(sun+0x9C)));
        f32 z=gabi::fmuls_ppc((f32)sunDirection->z,size);
        f32 next=gabi::fadds_ppc((f32)parity,step);
        sf(destination+8,gabi::fnmsubs(displacement,z,lf(sun+0xA0)));
        displacement=gabi::fadds_ppc(displacement,next);
    }
}
VERIFY(0x02565B70,dKyr_lenzflare_move);

static void dKyr_snow_init() {
    WWHD_FUNC(0x025688C4,void);
    u32 camera=ld(gameInfo()+0x5AF8);
    gabi::Local<SafeString_l> stageName,currentStage;
    stageName->text=0x1004F758;stageName->vtable=0x1004F3AC;
    currentStage->text=gameInfo()+0x5134;currentStage->vtable=0x1004F3AC;
    safe_string_callback(stageName.get());safe_string_callback(stageName.get());
    u32 expected=(u32)stageName->text;
    safe_string_callback(currentStage.get());
    bool special=equal_terminated_strings(expected,(u32)currentStage->text);
    gabi::Local<be<u32>> heapGuard;
    gabi::call(0x025F01D8,heapGuard.get(),0x1004F768u,special?0x14E4D10u:0xA26D10u,0);
    u32 packet=gabi::call<u32>(0x0273AE48,0x92B7E0u,32);
    if(packet) packet=gabi::call<u32>(0x0257FE7C,packet);
    gabi::store<u32>(environment()+0xA50,packet);
    if(!snowPacket()) {
        gabi::call(0x025F02D0,heapGuard.get());
        gabi::call(0x025F0270,heapGuard.get(),2);
        return;
    }
    gabi::Local<SafeString_l> archiveName,resourceName;
    archiveName->text=special?0x1004F79Cu:0x1004F760u;archiveName->vtable=0x1004F3AC;
    resourceName->text=special?0x1004F784u:0x1004F7BCu;resourceName->vtable=0x1004F3AC;
    u32 resource=gabi::call<u32>(0x026124B0,ld(0x101F4F7C),archiveName.get(),resourceName.get(),0);
    u32 shader=gabi::call<u32>(0x027E2DC0,resource),relative=ld(shader+0x24);
    u32 uniform=gabi::call<u32>(0x027DFA24,relative?shader+0x24+relative:0,special?0x1004F7ACu:0x1004F790u);
    gabi::store<u32>(snowPacket()+0x98,uniform);
    gabi::call(0x02580318,snowPacket());
    for(u32 i=0;i<250;i++) gabi::store<u8>(snowPacket()+0x9C+i*56,0);
    gabi::store<u16>(snowPacket()+0x3760,0);
    packet=snowPacket();
    for(u32 component=0;component<12;component+=4)
        gabi::store<u32>(packet+0x374C+component,ld(camera+0xDC+component));
    gabi::call(0x025F0270,heapGuard.get(),2);
}
VERIFY(0x025688C4,dKyr_snow_init);

static void dKyr_thunder_move() {
    WWHD_FUNC(0x02577138,void);
    u32 thunder=environment()+0xAB0,camera=ld(gameInfo()+0x5AF8);
    f32 half=lf(0x1004F57C),zero=lf(0x1004F528),one=lf(0x1004F550),nearScale=lf(0x1004F8D0);
    u8 state=gabi::load<u8>(thunder+1);
    switch(state) {
    case 0:
        sf(thunder+8,zero);sf(thunder+12,zero);sf(thunder+16,zero);
        if(gabi::call<f32>(0x020198D8,one)<lf(0x1004FD18)) {
            gabi::store<u8>(thunder+1,11);
            thunder_color(0x025567FC,thunder,0x1004F544);
            gabi::call(0x02556910,90,160,245,gabi::fmuls_ppc(lf(thunder+8),nearScale));
            thunder_color(0x02556A24,thunder,0x1004FD24);
            return;
        }
        if(gabi::call<f32>(0x020198D8,one)<lf(0x1004FB08)&&(s32)ld(environment()+0xAB4)<10) {
            for(u32 i=0;i<12;i+=4) gabi::store<u32>(thunder+0x18+i,ld(camera+0xDC+i));
            gabi::store<u16>(thunder+0x24,0);gabi::store<u16>(thunder+0x26,0);gabi::store<u16>(thunder+0x28,0);
            sf(thunder+0x2C,lf(0x1004FD1C));sf(thunder+0x30,lf(0x1004F574));
            gabi::call(0x0255B9C8,thunder+0x18);
            state=(u8)(gabi::load<u8>(thunder+1)+1);gabi::store<u8>(thunder+1,state);
        } else state=gabi::load<u8>(thunder+1);
        break;
    case 1:case 11: {
        gabi::call(0x0200ECD4,thunder+8,one,lf(0x1004F844),lf(0x1004F554),lf(0x1004F58C));
        f32 random=gabi::call<f32>(0x020198D8,half);
        gabi::call(0x0200ECD4,thunder+12,gabi::fadds_ppc(random,half),half,lf(0x1004F844),lf(0x1004F580));
        // Native branches on less-than; unordered values also advance the state.
        if(!(lf(thunder+8)<one)) {
            state=gabi::load<u8>(thunder+1);
            if(state<10) {gabi::call(0x025E19CC,0x69F7,0);state=gabi::load<u8>(thunder+1);}
            gabi::store<u8>(thunder+1,(u8)(state+1));
        }
        if(gabi::call<f32>(0x020198D8,one)<lf(0x1004F570)) gabi::call(0x025DADA4,0x1B4,-1,0,0,0);
        state=gabi::load<u8>(thunder+1);
        break;
    }
    case 2:case 12: {
        gabi::call(0x0200ECD4,thunder+8,zero,lf(0x1004F588),lf(0x1004F590),lf(0x1004F58C));
        f32 random=gabi::call<f32>(0x020198D8,half);
        gabi::call(0x0200ECD4,thunder+12,gabi::fadds_ppc(random,half),half,lf(0x1004F844),lf(0x1004F580));
        if(!(lf(thunder+8)>zero)) {
            if(gabi::load<u8>(thunder+1)<10) gabi::call(0x0255BA9C,thunder+0x18);
            gabi::store<u8>(thunder+1,0);
            if((s32)ld(environment()+0xAB4)==0) gabi::store<u8>(environment()+0xAB0,0);
        }
        state=gabi::load<u8>(thunder+1);
        break;
    }
    }
    if(!state) return;
    if(state>=10) {
        thunder_color(0x025567FC,thunder,0x1004F544);
        gabi::call(0x02556910,90,160,245,gabi::fmuls_ppc(lf(thunder+8),nearScale));
        thunder_color(0x02556A24,thunder,0x1004FD24);
        return;
    }
    f32 intensity=gabi::fmuls_ppc(lf(thunder+8),lf(thunder+12));
    f32 blue=gabi::fmuls_ppc(intensity,nearScale);
    gabi::store<u32>(thunder+0x18,ld(camera+0xDC));
    f32 green=gabi::fmuls_ppc(intensity,lf(0x1004F554));
    f32 y=gabi::fadds_ppc(lf(camera+0xE0),lf(0x1004F574));
    f32 red=gabi::fmuls_ppc(intensity,lf(0x1004F590));
    f32 multiplier=lf(0x1004FD20);
    s16 greenColor=(s16)gabi::ftoi(gabi::fmuls_ppc(multiplier,green));
    s16 redColor=(s16)gabi::ftoi(gabi::fmuls_ppc(multiplier,red));
    sf(thunder+0x1C,y);gabi::store<u32>(thunder+0x20,ld(camera+0xE4));
    s16 blueColor=(s16)gabi::ftoi(gabi::fmuls_ppc(multiplier,blue));
    gabi::store<s16>(thunder+0x24,redColor);gabi::store<s16>(thunder+0x26,greenColor);gabi::store<s16>(thunder+0x28,blueColor);
    if(!gabi::load<u8>(environment()+0x10A0)) {
        gabi::call(0x025548F0,90,160,245,gabi::fmuls_ppc(lf(thunder+8),half));
        gabi::call(0x02554A04,90,160,245,gabi::fmuls_ppc(lf(thunder+8),half));
    }
    thunder_color(0x02554C2C,thunder,0x1004F8E4,50,120,255);
    gabi::call(0x02554D40,90,160,245,gabi::fmuls_ppc(lf(thunder+8),nearScale));
    gabi::call(0x02554E54,90,160,245,gabi::fmuls_ppc(lf(thunder+8),nearScale));
    thunder_color(0x025567FC,thunder,0x1004F598);
    gabi::call(0x02556910,90,160,245,gabi::fmuls_ppc(lf(thunder+8),half));
    thunder_color(0x02556A24,thunder,0x1004F584);
}
VERIFY(0x02577138,dKyr_thunder_move);
}
