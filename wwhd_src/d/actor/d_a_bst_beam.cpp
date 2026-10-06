#include "d/actor/d_a_bst.h"

static void beamSound(u32 a,u32 id,bool actorCheck=true) {
    if ((!actorCheck || a) && a+0x37C) {
        s8 room=gabi::load<s8>(a+0x326);
        s8 reverb=gabi::call<s8>(0x02520540,room);
        gabi::call(0x025E1A40,id,gabi::at<void>(a+0x37C),0,reverb);
    }
}

// Gohdan tracks Link while charging, then releases a stream of moving beams.
static void beam_attack(bst_class* actor) {
    WWHD_FUNC(0x020E9F08,void,actor);
    u32 a=gabi::ea(actor),t=0x1047B60C;
    void* play=gabi::call<void*>(0x025200D4);
    s16 timer=gabi::load<s16>(a+0x30B8);
    u32 player=gabi::load<u32>(gabi::ea(play)+0x5B2C);
    gabi::store<s16>(a+0x30A8,10);gabi::store<s16>(a+0x30AA,10);
    if (timer<120) gabi::store<s16>(a+0x30B8,120);
    gabi::Local<cXyz> difference;
    gabi::call(0x0201ADE0,gabi::at<void>(player+0x314),difference.get(),gabi::at<void>(a+0x314));
    f32 dx=difference->x,dz=difference->z;
    f32 dy=gabi::fadds_ppc((f32)difference->y,gabi::load<f32>(a+0x1304));
    s16 yaw=gabi::call<s16>(0x020195B0,dx,dz);
    gabi::call(0x0200F428,gabi::at<void>(a+0x32A),yaw,4,0x800);
    // Espresso fmadds forms the sum in double precision before rounding single.
    f32 square=(f32)((f64)dx*(f64)dx+(f64)gabi::fmuls_ppc(dz,dz));
    f32 distance=gabi::call<f32>(0x028F4384,square);
    s16 pitch=gabi::call<s16>(0x020195B0,dy,distance);
    gabi::call(0x0200F428,gabi::at<void>(a+0x328),(s16)-pitch,4,0x800);
    f32 constant=gabi::load<f32>(0x1000B93C),ratio=gabi::load<f32>(0x1000B864);
    f32 height=gabi::load<f32>(t+0x30),step=gabi::load<f32>(0x1000B888);
    gabi::call(0x0200ED84,gabi::at<void>(a+0x318),gabi::fadds_ppc(height,constant),ratio,step);
    s16 state=gabi::load<s16>(a+0x130E);
    f32 zero=gabi::load<f32>(0x1000B854),one=gabi::load<f32>(0x1000B860);
    bool finished=false;
    switch(state) {
    case 0:
        gabi::store<s16>(a+0x130E,(s16)(state+1));
        gabi::call(0x020E47D0,actor,7,gabi::load<f32>(0x1000B8B8),0,one,-1);
        beamSound(a,0x58DE);break;
    case 1: {
        u32 morph=gabi::load<u32>(a+0x3D4);
        if ((gabi::load<u8>(morph+0xA7)&1) || gabi::load<f32>(morph+0x98)==zero) {
            gabi::store<s16>(a+0x130E,(s16)(state+1));
            gabi::call(0x020E47D0,actor,8,one,2,one,-1);
            gabi::store<s16>(a+0x1330,45);
            for(u32 i=0;i<2;++i) {
                u16 effect=gabi::load<u16>(0x10192858+i*2);
                play=gabi::call<void*>(0x025200D4);
                u32 control=gabi::load<u32>(gabi::ea(play)+0x5AB0);
                u32 emitter=gabi::call<u32>(0x025A847C,gabi::at<void>(control),0,effect,gabi::at<void>(a+0x314),nullptr,nullptr,255,nullptr,-1,0,nullptr,nullptr);
                gabi::store<u32>(a+0x310C+i*4,emitter);
            }
        }
        break;
    }
    case 2: {
        beamSound(a,0x50DF);
        timer=gabi::load<s16>(a+0x1330);gabi::store<u8>(a+0x3108,5);
        if(timer==0) {
            for(u32 i=0;i<2;++i) {
                u32 emitter=gabi::load<u32>(a+0x310C+i*4);
                if(emitter) {
                    u32 flags=gabi::load<u32>(emitter+0x254);
                    gabi::store<u32>(emitter+0x5C,0xFFFFFFFF);
                    gabi::store<u32>(emitter+0x254,flags|1);
                    gabi::store<u32>(a+0x310C+i*4,0);
                }
            }
            s16 current=gabi::load<s16>(a+0x130E);
            gabi::store<s16>(a+0x130E,(s16)(current+1));
            gabi::call(0x020E47D0,actor,6,one,2,one,-1);
            gabi::store<s16>(a+0x1330,(s16)(gabi::load<s16>(t+0x84)+50));
        }
        break;
    }
    case 3: {
        f32 accel=gabi::load<f32>(t+0x18),twenty=gabi::load<f32>(0x1000B94C);
        f32 target=gabi::load<f32>(t+0x1C),rate=gabi::load<f32>(0x1000B8E4);
        gabi::call(0x0200ED84,gabi::at<void>(a+0x1304),target,rate,gabi::fadds_ppc(accel,twenty));
        timer=gabi::load<s16>(a+0x1330);
        if((timer&3)==0) {
            f32 speedConstant=gabi::load<f32>(0x1000B950);
            gabi::store<u8>(a+0x3108,6);
            for(u32 i=0;i<10;++i) {
                if(gabi::load<s8>(a+0x718+i)!=0) continue;
                gabi::store<u8>(a+0x718+i,1);
                u32 morph=gabi::load<u32>(a+0x3D4);
                s32 joint=gabi::load<s16>(t+0x86)+3;
                u32 model=gabi::load<u32>(morph+0x90),block=gabi::load<u32>(model+0x2C);
                u16 flags=gabi::load<u16>(block+4);u32 matrices=gabi::load<u32>(block+0x10);
                gabi::store<u16>(block+4,flags|0x10);
                u32 matrix=gabi::load<u32>(0x1018C7B0);
                gabi::call(0x028E90D4,gabi::at<void>(matrices+joint*0x30),gabi::at<void>(matrix));
                gabi::Local<cXyz> local;local->x=zero;local->y=zero;local->z=zero;
                gabi::call(0x0200FCD8,local.get(),gabi::at<void>(a+0x5EC+i*12));
                pitch=gabi::load<s16>(a+0x328);gabi::store<s16>(a+0x6DC+i*6,pitch);
                yaw=gabi::load<s16>(a+0x32A);gabi::store<s16>(a+0x6DE + i*6,yaw);
                gabi::store<s16>(a+0x6E0+i*6,gabi::load<s16>(a+0x32C));
                matrix=gabi::load<u32>(0x1018C7B0);gabi::call(0x025F1884,gabi::at<void>(matrix),yaw);
                matrix=gabi::load<u32>(0x1018C7B0);pitch=gabi::load<s16>(a+0x6DC+i*6);
                gabi::call(0x025F1BF4,gabi::at<void>(matrix),pitch);
                f32 speed=gabi::load<f32>(t+0x34);
                local->x=zero;local->y=zero;local->z=gabi::fadds_ppc(speed,speedConstant);
                gabi::call(0x0200FCD8,local.get(),gabi::at<void>(a+0x664+i*12));
                gabi::store<f32>(a+0x12DC+i*4,one);break;
            }
            beamSound(a,0x58E0);
            timer=gabi::load<s16>(a+0x1330);
        }
        finished=timer==0;break;
    }
    }
    play=gabi::call<void*>(0x025200D4);
    player=gabi::load<u32>(gabi::ea(play)+0x5B2C);
    distance=gabi::call<f32>(0x025D68EC,actor,gabi::at<void>(player));
    f32 boundary=gabi::fadds_ppc(gabi::load<f32>(t+0x28),gabi::load<f32>(0x1000B8B0));
    if(distance<boundary || finished) {
        gabi::store<u8>(a+0x3108,8);gabi::store<s16>(a+0x130A,1);
        gabi::store<s16>(a+0x130E,0);gabi::store<f32>(a+0x370,zero);
        beamSound(a,0x58E1,false);
        s8 health=gabi::load<s8>(a+0x3A1);f32 hundred=gabi::load<f32>(0x1000B8B4),wait;
        if(health==0) wait=gabi::fadds_ppc(gabi::call<f32>(0x020198D8,gabi::load<f32>(0x1000B890)),hundred);
        else if(health==1) { f32 random=gabi::call<f32>(0x020198D8,gabi::load<f32>(0x1000B894));wait=gabi::fadds_ppc(random,gabi::load<f32>(0x1000B954)); }
        else if(health==2) { f32 random=gabi::call<f32>(0x020198D8,gabi::load<f32>(0x1000B958));wait=gabi::fadds_ppc(random,gabi::load<f32>(0x1000B8E0)); }
        else { f32 base=gabi::load<f32>(0x1000B898);f32 max=health==3?gabi::load<f32>(0x1000B95C):hundred;wait=gabi::fadds_ppc(gabi::call<f32>(0x020198D8,max),base); }
        gabi::store<s16>(a+0x30B4,(s16)gabi::ftoi(wait));
    }
    for(u32 i=0;i<2;++i) {
        u32 emitter=gabi::load<u32>(a+0x310C+i*4);
        if(!emitter) continue;
        u32 morph=gabi::load<u32>(a+0x3D4),model=gabi::load<u32>(morph+0x90),block=gabi::load<u32>(model+0x2C);
        u16 flags=gabi::load<u16>(block+4);u32 matrices=gabi::load<u32>(block+0x10);
        gabi::store<u16>(block+4,flags|0x10);
        gabi::call(0x028249B0,gabi::at<void>(matrices),gabi::at<void>(emitter+0x1F0),gabi::at<void>(emitter+0x22C));
    }
}

VERIFY(0x020E9F08,beam_attack);
