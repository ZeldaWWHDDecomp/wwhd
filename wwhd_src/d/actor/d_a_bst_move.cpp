#include "d/actor/d_a_bst.h"
#include <cmath>

// Inlined fly action 020EBAF4–020EC1C8, pending integration.
#include "d/actor/d_a_bst.h"
static void bstFlyAnimator(u32 a,u32 table,u32 offset,bool brk,s32 loop,f32 one,u32 resourceName=0x1000B6FC) {
    u8 part=gabi::load<u8>(a+0x3D0);u32 resources=gabi::load<u32>(0x101F4F28);
    u32 morph=gabi::load<u32>(a+0x3D4),model=gabi::load<u32>(morph+0x90);
    u16 id=gabi::load<u16>(table+part*2);gabi::Local<be<u32>[2]> name;
    (*name)[0]=resourceName;(*name)[1]=0x1000B714;
    void* resource=gabi::call<void*>(0x026066C4,gabi::at<void>(resources),name.get(),id);
    u32 data=gabi::load<u32>(model+0xAC),controller=gabi::load<u32>(a+offset);
    gabi::call(brk?0x025E8154:0x025E7CE0,gabi::at<void>(controller),gabi::at<void>(data),resource,1,loop,0,-1,1,one,0);
}
static void bstDispatchFly(bst_class* actor,f32 zero,f32 one,f32 half,f32 ten) {
    u32 a=gabi::ea(actor),t=0x1047B608;void* play=gabi::call<void*>(0x025200D4);
    u8 part=gabi::load<u8>(a+0x3D0);u32 player=gabi::load<u32>(gabi::ea(play)+0x5B2C);
    s16 timer=gabi::load<s16>(a+0x133C);
    if(part && gabi::load<s16>(a+0x130C)==6) gabi::store<u8>(a+0x3A1,4);
    gabi::store<s16>(a+0x130C,1);
    if(timer==0) gabi::call(0x020E4790,actor,1);
    u32 head=gabi::load<u32>(0x10462988);
    bool face=gabi::load<s8>(head+0x30CE)!=0;
    if(!face && (gabi::load<s16>(a+0x1308)&31)==0) face=gabi::call<f32>(0x020198D8,one)<half;
    if(face) gabi::store<s16>(a+0x131C,gabi::load<s16>(player+0x32A));
    s16 state=gabi::load<s16>(a+0x130E);
    if(state==0) {
        part=gabi::load<u8>(a+0x3D0);u16 animation=gabi::load<u16>(0x10192878+part*2);
        gabi::call(0x020E47D0,actor,animation,ten,2,one,-1);
        bstFlyAnimator(a,0x101928C0,0x3DC,true,0,one);
        s16 frame=gabi::load<s16>(t+0x90);u32 controller=gabi::load<u32>(a+0x3DC);
        gabi::store<f32>(controller+4,(f32)((s32)frame+29));
        state=gabi::load<s16>(a+0x130E);gabi::store<u8>(a+0x3F0,1);gabi::store<u8>(a+0x3E0,1);
        gabi::store<s16>(a+0x130E,(s16)(state+1));
    } else {
        if(state==1) {
            bstFlyAnimator(a,0x101928B0,0x3DC,true,2,one);
            bstFlyAnimator(a,0x101928A8,0x3D8,false,2,one);
            state=gabi::load<s16>(a+0x130E);gabi::store<f32>(a+0x1324,zero);
            gabi::store<s16>(a+0x130E,(s16)(state+1));state=2;
        }
        if(state==2) {
            s16 yaw=gabi::load<s16>(a+0x131C);u32 matrix=gabi::load<u32>(0x1018C7B0);
            gabi::call(0x025F1884,gabi::at<void>(matrix),yaw);
            f32 height=gabi::load<f32>(t+0x14),eightHundred=gabi::load<f32>(0x1000B968),twoHundred=gabi::load<f32>(0x1000B874);
            f32 depth=gabi::load<f32>(t+0x18);gabi::Local<cXyz> local,transformed,sum;
            local->y=gabi::fadds_ppc(height,twoHundred);
            part=gabi::load<u8>(a+0x3D0);
            if(!part) {
                local->x=zero;local->z=gabi::fadds_ppc(depth,gabi::load<f32>(0x1000B96C));
                head=gabi::load<u32>(0x10462988);
                if(gabi::load<s8>(head+0x30B0)==11) local->y=gabi::fadds_ppc(height,gabi::load<f32>(0x1000B970));
            } else {
                local->z=gabi::fadds_ppc(depth,eightHundred);
                f32 side=gabi::fadds_ppc(gabi::load<f32>(t+0x10),gabi::load<f32>(0x1000B968));
                if(part==1)local->x=-side;else if(part==2)local->x=side;
            }
            gabi::call(0x0200FCD8,local.get(),transformed.get());
            gabi::call(0x0201AD78,gabi::at<void>(player+0x314),sum.get(),transformed.get());
            f32 targetX=sum->x;part=gabi::load<u8>(a+0x3D0);
            gabi::store<f32>(a+0x1310,targetX);f32 targetY=sum->y,targetZ=sum->z;
            gabi::store<f32>(a+0x1314,targetY);gabi::store<f32>(a+0x1318,targetZ);
            if(!part) {
                local->z=targetZ;local->y=zero;local->x=targetX;
                f32 square=gabi::call<f32>(0x028E8DD0,local.get()),radius=gabi::call<f32>(0x028F4384,square);
                f32 limit=gabi::fadds_ppc(gabi::load<f32>(t+0x2C),gabi::load<f32>(0x1000B960));
                if(radius>limit) {
                    f32 ratio=limit/radius;targetX=gabi::load<f32>(a+0x1310);targetZ=gabi::load<f32>(a+0x1318);
                    gabi::store<f32>(a+0x1310,gabi::fmuls_ppc(targetX,ratio));gabi::store<f32>(a+0x1318,gabi::fmuls_ppc(targetZ,ratio));
                }
            }
            gabi::call(0x0201ADE0,gabi::at<void>(a+0x1310),sum.get(),gabi::at<void>(a+0x314));local->copy(*sum);
            f32 square=gabi::call<f32>(0x028E8DD0,local.get()),distance=gabi::call<f32>(0x028F4384,square);
            f32 boundary=gabi::fadds_ppc(gabi::load<f32>(t+0x38),gabi::load<f32>(0x1000B878));
            if(distance>boundary) {
                part=gabi::load<u8>(a+0x3D0);bool headFlight=false;
                if(!part) {head=gabi::load<u32>(0x10462988);headFlight=gabi::load<s8>(head+0x30B0)==11;}
                f32 speed=headFlight?ten:gabi::load<f32>(0x1000B888);
                f32 acceleration=gabi::fadds_ppc(gabi::load<f32>(t+0x3C),gabi::load<f32>(0x1000B924));
                gabi::call(0x0200ED84,gabi::at<void>(a+0x370),speed,one,acceleration);
            } else {
                f32 decay=gabi::fadds_ppc(gabi::load<f32>(t+0x40),one);
                gabi::call(0x0200EDC8,gabi::at<void>(a+0x370),one,decay);
            }
            f32 turn=gabi::fadds_ppc(gabi::load<f32>(t+0x1C),gabi::load<f32>(0x1000B974));gabi::store<f32>(a+0x1320,turn);
            play=gabi::call<void*>(0x025200D4);u32 currentPlayer=gabi::load<u32>(gabi::ea(play)+0x5B2C);
            yaw=gabi::call<s16>(0x025D6894,actor,gabi::at<void>(currentPlayer));
            gabi::call(0x0200F428,gabi::at<void>(a+0x32A),yaw,10,0x400);
            part=gabi::load<u8>(a+0x3D0);bool headFlight=false;
            if(!part) {head=gabi::load<u32>(0x10462988);headFlight=gabi::load<s8>(head+0x30B0)==11;}
            gabi::call(0x0200F428,gabi::at<void>(a+0x328),headFlight?0:3000,10,0x100);
            gabi::call(0x0200F428,gabi::at<void>(a+0x32C),0,8,0x200);
            f32 shake=gabi::fadds_ppc(gabi::load<f32>(t+0x34),gabi::load<f32>(0x1000B890));
            f32 acceleration=gabi::load<f32>(0x1000B924);
            gabi::call(0x0200ED84,gabi::at<void>(a+0x1328),shake,one,acceleration);
        }
    }
    gabi::call(0x020E4CD0,actor,0);
    if(gabi::load<u8>(a+0x3D0)==0 && gabi::load<s16>(a+0x30B8)==0) {
        u32 save=gabi::load<u32>(0x101F84DC);
        if(gabi::load<u8>(save+0x89)==0 || gabi::load<u8>(save+0x8A)==0) {
            gabi::store<s16>(a+0x30B8,600);gabi::store<s16>(a+0x130A,20);gabi::store<s16>(a+0x130E,0);
        }
    }
}

// Inlined damage and sleep actions, pending integration.
static void bstDispatchSound(u32 a,u32 sound) {
    if(a+0x37C) {
        s8 room=gabi::load<s8>(a+0x326),reverb=gabi::call<s8>(0x02520540,room);
        gabi::call(0x025E1A40,sound,gabi::at<void>(a+0x37C),0,reverb);
    }
}
static void bstDispatchDamage(bst_class* actor,f32 zero,f32 one) {
    u32 a=gabi::ea(actor);s16 state=gabi::load<s16>(a+0x130E);
    gabi::store<s16>(a+0x133C,10);
    if(state==0) {
        u8 part=gabi::load<u8>(a+0x3D0);
        if(part) {
            u16 animation=gabi::load<u16>(0x10192880+part*2);
            gabi::call(0x020E47D0,actor,animation,one,0,one,-1);
        }
        bstFlyAnimator(a,0x101928C0,0x3DC,true,0,one,0x1000B704);
        bstFlyAnimator(a,0x101928B8,0x3D8,false,0,one,0x1000B704);
        state=gabi::load<s16>(a+0x130E);gabi::store<s16>(a+0x130E,(s16)(state+1));
        gabi::store<s16>(a+0x1330,30);
    } else if(state==1 && gabi::load<s16>(a+0x1330)==0) {
        u8 part=gabi::load<u8>(a+0x3D0);
        if(part && gabi::load<s8>(a+0x3A1)<=0) {
            part=gabi::load<u8>(a+0x3D0);gabi::store<s16>(a+0x130A,6);
            gabi::store<s16>(a+0x1332,300);
            u32 hand=gabi::load<u32>(part==1?0x10462978:0x1046297C);
            if(gabi::load<s16>(hand+0x130A)!=6) gabi::call(0x025E18EC,part==1?0x80000120:0x80000121);
        } else {
            gabi::store<f32>(a+0x370,zero);gabi::store<s16>(a+0x130A,1);
        }
        gabi::store<s16>(a+0x130E,0);
    }
}
static void bstDispatchSleep(bst_class* actor,f32 zero,f32 one,f32 ten) {
    u32 a=gabi::ea(actor),t=0x1047B608;
    gabi::store<s16>(a+0x130C,6);gabi::call(0x020E4790,actor,1);
    gabi::store<u32>(a+0x39C,0);f32 fifty=gabi::load<f32>(0x1000B890);
    gabi::store<s16>(a+0x133C,10);f32 shake=gabi::load<f32>(t+0x34),two=gabi::load<f32>(0x1000B924);
    gabi::call(0x0200ED84,gabi::at<void>(a+0x1328),gabi::fadds_ppc(shake,fifty),one,two);
    s16 state=gabi::load<s16>(a+0x130E);
    if(state==0) {
        f32 twenty=gabi::load<f32>(0x1000B94C);u8 part=gabi::load<u8>(a+0x3D0);
        u16 animation=gabi::load<u16>(0x10192888+part*2);
        gabi::call(0x020E47D0,actor,animation,twenty,2,one,-1);
        bstFlyAnimator(a,0x101928E8,0x3DC,true,0,one,0x1000B700);
        bstFlyAnimator(a,0x101928E0,0x3D8,false,0,one,0x1000B700);
        state=gabi::load<s16>(a+0x130E);gabi::store<s16>(a+0x130E,(s16)(state+1));
        bstDispatchSound(a,0x58DC);state=1;
    }
    if(state==1) {
        if(gabi::load<s16>(a+0x1330)==0) {
            f32 x=gabi::call<f32>(0x02019918,gabi::load<f32>(0x1000B974));gabi::store<f32>(a+0x1310,x);
            f32 y=gabi::call<f32>(0x020198D8,gabi::load<f32>(0x1000B874));
            f32 sixHundred=gabi::load<f32>(0x1000B8C8);u8 part=gabi::load<u8>(a+0x3D0);
            y=gabi::fadds_ppc(y,sixHundred);gabi::store<f32>(a+0x1314,y);
            if(part==1) gabi::store<f32>(a+0x1314,gabi::fadds_ppc(y,gabi::load<f32>(0x1000B970)));
            f32 z=gabi::call<f32>(0x02019918,gabi::load<f32>(0x1000B974));gabi::store<f32>(a+0x1318,z);
            f32 duration=gabi::call<f32>(0x020198D8,gabi::load<f32>(0x1000B888));
            gabi::store<s16>(a+0x1330,(s16)gabi::ftoi(duration));
        }
        f32 turn=gabi::fadds_ppc(gabi::load<f32>(t+0x1C),gabi::load<f32>(0x1000B8B0));gabi::store<f32>(a+0x1320,turn);
        f32 speed=gabi::load<f32>(t+0x20);s16 duration=gabi::load<s16>(a+0x1332);
        gabi::store<f32>(a+0x370,gabi::fadds_ppc(speed,ten));
        if(duration==0) {
            gabi::store<s16>(a+0x130A,1);gabi::store<s16>(a+0x130E,0);bstDispatchSound(a,0x58D5);
        }
    }
    gabi::call(0x020E4CD0,actor,0);
}

// Inlined head damage action 020EC670–020ED130, pending integration.
static void bstFixedAnimator(u32 a,u32 id,u32 offset,bool brk,f32 one) {
    u32 morph=gabi::load<u32>(a+0x3D4),resources=gabi::load<u32>(0x101F4F28),model=gabi::load<u32>(morph+0x90);
    gabi::Local<be<u32>[2]> name;(*name)[0]=0x1000B708;(*name)[1]=0x1000B714;
    void* resource=gabi::call<void*>(0x026066C4,gabi::at<void>(resources),name.get(),id);
    u32 controller=gabi::load<u32>(a+offset),data=gabi::load<u32>(model+0xAC);
    gabi::call(brk?0x025E8154:0x025E7CE0,gabi::at<void>(controller),gabi::at<void>(data),resource,1,0,0,-1,1,one,0);
}
static u32 bstHeldBomb(u32 a) {
    gabi::Local<be<u32>> id;*id=gabi::load<u32>(a+0x30BC);
    if((u32)*id==0xFFFFFFFF) return 0;
    return gabi::call<u32>(0x025D5218,gabi::at<void>(0x025E1234),id.get());
}
static void bstRetireEmitter(u32 a,u32 offset) {
    u32 emitter=gabi::load<u32>(a+offset);
    if(emitter) {
        u32 flags=gabi::load<u32>(emitter+0x254);gabi::store<u32>(emitter+0x5C,0xFFFFFFFF);
        gabi::store<u32>(emitter+0x254,flags|1);gabi::store<u32>(a+offset,0);
    }
}
static void bstDispatchHeadDamage(bst_class* actor,f32 zero,f32 one) {
    u32 a=gabi::ea(actor),t=0x1047B608;
    gabi::store<s16>(a+0x133C,10);gabi::store<s16>(a+0x30A8,10);gabi::store<s16>(a+0x30AA,10);
    gabi::call(0x0200F428,gabi::at<void>(a+0x328),0,8,0x200);
    gabi::call(0x0200F428,gabi::at<void>(a+0x32C),0,8,0x200);
    gabi::call(0x0200EDC8,gabi::at<void>(a+0x1328),one,gabi::load<f32>(0x1000B8B8));
    s16 state=gabi::load<s16>(a+0x130E);s16 entryState=state;bool processBomb=state==4;
    if(state==0) {
        gabi::store<f32>(a+0x370,zero);gabi::store<f32>(a+0x340,zero);
        gabi::store<s16>(a+0x130E,(s16)(state+1));gabi::store<s16>(a+0x1330,40);
        bstFixedAnimator(a,0x39,0x3DC,true,one);bstFixedAnimator(a,0x54,0x3D8,false,one);
        bstDispatchSound(a,0x58DD);gabi::call(0x025E18EC,0x80000123);state=1;
    }
    if(state==1) {
        gabi::Local<cXyz> horizontal;
        horizontal->x=gabi::load<f32>(a+0x314);horizontal->y=zero;horizontal->z=gabi::load<f32>(a+0x31C);
        f32 square=gabi::call<f32>(0x028E8DD0,horizontal.get()),radius=gabi::call<f32>(0x028F4384,square);
        f32 limit=gabi::fadds_ppc(gabi::load<f32>(t+0x2C),gabi::load<f32>(0x1000B960));
        if(radius>limit) {
            f32 ratio=limit/radius,step=gabi::load<f32>(0x1000B888),target=gabi::fmuls_ppc((f32)horizontal->x,ratio);
            gabi::call(0x0200ED84,gabi::at<void>(a+0x314),target,gabi::load<f32>(0x1000B87C),step);
            target=gabi::fmuls_ppc((f32)horizontal->z,ratio);
            gabi::call(0x0200ED84,gabi::at<void>(a+0x31C),target,gabi::load<f32>(0x1000B87C),gabi::load<f32>(0x1000B888));
        }
        if(gabi::load<s16>(a+0x1330)==0) {
            f32 y=gabi::load<f32>(a+0x318),speed=gabi::load<f32>(a+0x340);y=gabi::fadds_ppc(y,speed);
            f32 five=gabi::load<f32>(0x1000B8B8);gabi::store<f32>(a+0x318,y);
            f32 acceleration=gabi::fadds_ppc(gabi::load<f32>(t+0x3C),five);speed=gabi::fsubs_ppc(speed,acceleration);
            gabi::store<f32>(a+0x340,speed);f32 floor=gabi::load<f32>(t+0x40);
            if(!(y>floor)) {
                gabi::store<f32>(a+0x318,floor);void* play=gabi::call<void*>(0x025200D4);
                s32 strength=gabi::load<s16>(t+0x84)+7;gabi::Local<cXyz> impulse;
                impulse->z=zero;impulse->y=one;impulse->x=zero;
                gabi::call(0x025CB374,gabi::at<void>(gabi::ea(play)+0x599C),strength,-33,impulse.get());
                s8 room=gabi::load<s8>(a+0x326);play=gabi::call<void*>(0x025200D4);u32 control=gabi::load<u32>(gabi::ea(play)+0x5AB0);
                gabi::call(0x025A847C,gabi::at<void>(control),2,0xA1DB,gabi::at<void>(a+0x314),nullptr,nullptr,185,gabi::at<void>(a+0x3134),room,0,nullptr,nullptr);
                gabi::store<s16>(a+0x135E,(s16)(gabi::load<s16>(t+0x8E)+4));
                bstDispatchSound(a,0x698A);bstDispatchSound(a,0x58DE);
                gabi::call(0x020E47D0,actor,21,one,0,one,-1);
                state=gabi::load<s16>(a+0x130E);gabi::store<f32>(a+0x340,zero);gabi::store<s16>(a+0x130E,(s16)(state+1));
            }
        }
    } else if(state==2) {
        u32 morph=gabi::load<u32>(a+0x3D4);
        if((gabi::load<u8>(morph+0xA7)&1) || gabi::load<f32>(morph+0x98)==zero) {
            gabi::call(0x020E47D0,actor,22,one,2,one,-1);state=gabi::load<s16>(a+0x130E);
            gabi::store<s16>(a+0x1330,300);gabi::store<s16>(a+0x130E,(s16)(state+1));
        }
    } else if(state==3) {
        u32 bomb=0;
        if(gabi::call<BOOL>(0x02516464,gabi::at<void>(a+0x16D4))) {
            u32 hit=gabi::call<u32>(0x025163BC,gabi::at<void>(a+0x16D4));
            if(hit) {
                u32 info=gabi::load<u32>(hit+0x44);
                if(info) bomb=gabi::load<u32>(info+0xC);
            }
        }
        bool captured=false;
        if(bomb && gabi::load<s16>(bomb+8)==0x126 && !gabi::call<BOOL>(0x020CB678,gabi::at<void>(bomb))) {
            if(gabi::call<s32>(0x020CB648,gabi::at<void>(bomb))>1) {
                gabi::store<u32>(a+0x30BC,gabi::load<u32>(bomb+4));
                gabi::call(0x020CB6A8,gabi::at<void>(bomb));gabi::call(0x020CB978,gabi::at<void>(bomb),2);
                gabi::call(0x020CB710,gabi::at<void>(bomb));gabi::call(0x020CB860,gabi::at<void>(bomb),200);
                state=gabi::load<s16>(a+0x130E);gabi::store<s16>(a+0x130E,(s16)(state+1));gabi::store<s16>(a+0x1330,10);captured=true;
            }
        }
        if(captured) processBomb=true;
        else if(gabi::load<s16>(a+0x1330)==0) {
            gabi::store<s16>(a+0x130A,8);gabi::store<s16>(a+0x130E,0);gabi::call(0x025E18EC,0x80000122);
        }
    }
    bool checkExplosion=entryState==5;
    if(processBomb) {
        u32 bomb=bstHeldBomb(a);
        if(bomb) {
            for(u32 offset:{0x374u,0x370u,0x33Cu,0x340u,0x344u}) gabi::store<f32>(bomb+offset,zero);
            for(u32 offset:{0x320u,0x322u,0x324u,0x328u,0x32Au,0x32Cu}) gabi::store<s16>(bomb+offset,0);
            f32 target=gabi::load<f32>(a+0x314),step=gabi::load<f32>(0x1000B888);
            gabi::call(0x0200ED84,gabi::at<void>(bomb+0x314),target,one,step);
            target=gabi::fadds_ppc(gabi::load<f32>(a+0x318),gabi::load<f32>(0x1000B95C));
            target=gabi::fadds_ppc(target,gabi::load<f32>(t+0x34));step=gabi::load<f32>(0x1000B888);
            gabi::call(0x0200ED84,gabi::at<void>(bomb+0x318),target,one,step);
            target=gabi::load<f32>(a+0x31C);step=gabi::load<f32>(0x1000B888);
            gabi::call(0x0200ED84,gabi::at<void>(bomb+0x31C),target,one,step);
        }
        if(gabi::load<s16>(a+0x1330)==0) {
            gabi::call(0x020E47D0,actor,10,one,0,one,-1);state=gabi::load<s16>(a+0x130E);
            gabi::store<s16>(a+0x130E,(s16)(state+1));void* play=gabi::call<void*>(0x025200D4);
            u32 control=gabi::load<u32>(gabi::ea(play)+0x5AB0);
            u32 emitter=gabi::call<u32>(0x025A847C,gabi::at<void>(control),0,0x81E3,gabi::at<void>(a+0x314),nullptr,nullptr,255,nullptr,-1,0,nullptr,nullptr);
            gabi::store<u32>(a+0x3114,emitter);
            if(a) bstDispatchSound(a,0x58E1);
            u32 morph=gabi::load<u32>(a+0x3D4);gabi::store<s16>(a+0x1330,60);
            checkExplosion=true;
        }
    }
    if(entryState==5) {
        s16 timer=gabi::load<s16>(a+0x1330);
        if(timer==50 && a) {bstDispatchSound(a,0x58E3);timer=gabi::load<s16>(a+0x1330);}
        if(timer==30 && a) bstDispatchSound(a,0x698B);
    }
    if(checkExplosion) {
        u32 morph=gabi::load<u32>(a+0x3D4);
        if((gabi::load<u8>(morph+0xA7)&1) || gabi::load<f32>(morph+0x98)==zero) {
            bstRetireEmitter(a,0x3114);u32 bomb=bstHeldBomb(a);
            if(bomb) gabi::call(0x025D57E0,gabi::at<void>(bomb));
            s8 health=(s8)(gabi::load<u8>(a+0x3A1)-1);gabi::store<s8>(a+0x3A1,health);
            if(health<=0 || gabi::load<u8>(0x10462982)!=0) {
                void* play=gabi::call<void*>(0x025200D4);gabi::store<f32>(gabi::ea(play)+0x5B44,zero);
                gabi::store<s16>(a+0x130A,22);gabi::store<s16>(a+0x130E,0);gabi::store<s8>(a+0x30CE,50);
                gabi::call(0x020E47D0,actor,11,one,0,one,-1);
                gabi::store<s16>(gabi::load<u32>(0x10462978)+0x130A,22);
                gabi::store<s16>(gabi::load<u32>(0x10462978)+0x130E,10);
                gabi::store<s16>(gabi::load<u32>(0x1046297C)+0x130A,22);
                gabi::store<s16>(gabi::load<u32>(0x1046297C)+0x130E,10);
                bstDispatchSound(a,0x2828);gabi::call(0x025E1904,30);gabi::call(0x025E1934,0xC0000019);
            } else {
                gabi::call(0x020E47D0,actor,9,one,0,one,-1);state=gabi::load<s16>(a+0x130E);
                gabi::store<s16>(a+0x130E,(s16)(state+1));
            }
            bstDispatchSound(a,0x698C);
            for(u32 i=0;i<4;++i) {
                u16 effect=gabi::load<u16>(0x10192850+i*2);void* play=gabi::call<void*>(0x025200D4);
                u32 control=gabi::load<u32>(gabi::ea(play)+0x5AB0);
                u32 emitter=gabi::call<u32>(0x025A847C,gabi::at<void>(control),0,effect,gabi::at<void>(a+0x314),nullptr,nullptr,255,nullptr,-1,0,nullptr,nullptr);
                gabi::store<u32>(a+0x3118+i*4,emitter);
            }
            gabi::store<s16>(a+0x1336,81);
        }
    } else if(entryState==6) {
        u32 morph=gabi::load<u32>(a+0x3D4);s32 frame=gabi::ftoi(gabi::load<f32>(morph+0x9C));
        if(frame==8 && a) {bstDispatchSound(a,0x698D);morph=gabi::load<u32>(a+0x3D4);}
        if((gabi::load<u8>(morph+0xA7)&1) || gabi::load<f32>(morph+0x98)==zero) {
            state=gabi::load<s16>(a+0x130E);gabi::store<s16>(a+0x1330,30);
            gabi::store<s16>(a+0x130E,(s16)(state+1));gabi::call(0x025E18EC,0x80000023);
        }
    } else if(entryState==7 && gabi::load<s16>(a+0x1330)==0) {
        gabi::store<s16>(a+0x130A,8);gabi::store<s16>(a+0x130E,0);
    }
    for(u32 i=0;i<5;++i) {
        u32 emitter=gabi::load<u32>(a+0x3114+i*4);if(!emitter) continue;
        if(i>0 && gabi::load<s16>(a+0x1336)==1) {
            u32 flags=gabi::load<u32>(emitter+0x254);gabi::store<u32>(emitter+0x5C,0xFFFFFFFF);
            gabi::store<u32>(emitter+0x254,flags|1);gabi::store<u32>(a+0x3114+i*4,0);
        } else {
            u32 morph=gabi::load<u32>(a+0x3D4),model=gabi::load<u32>(morph+0x90),block=gabi::load<u32>(model+0x2C);
            u16 flags=gabi::load<u16>(block+4);u32 matrices=gabi::load<u32>(block+0x10);
            gabi::store<u16>(block+4,flags|0x10);
            gabi::call(0x028249B0,gabi::at<void>(matrices),gabi::at<void>(emitter+0x1F0),gabi::at<void>(emitter+0x22C));
        }
    }
}

// Inlined head recovery 020ED130–020ED348, pending integration.
static void bstDispatchRecover(bst_class* actor,f32 zero,f32 one) {
    u32 a=gabi::ea(actor),t=0x1047B608;s16 state=gabi::load<s16>(a+0x130E);
    if(state==0) {
        gabi::store<s16>(a+0x130E,(s16)(state+1));s16 duration=gabi::load<s16>(t+0x80);
        gabi::store<f32>(a+0x132C,zero);gabi::store<f32>(a+0x1324,zero);
        gabi::store<s16>(a+0x1330,(s16)(duration+40));state=1;
    }
    if(state==1) {
        f32 strength=gabi::load<f32>(t+0x18),fourThousand=gabi::load<f32>(0x1000B92C),hundred=gabi::load<f32>(0x1000B8B4);
        f32 step=gabi::load<f32>(t+0x1C);
        gabi::call(0x0200ED84,gabi::at<void>(a+0x132C),gabi::fadds_ppc(strength,fourThousand),one,gabi::fadds_ppc(step,hundred));
        if(gabi::load<s16>(a+0x1330)==0) {
            state=gabi::load<s16>(a+0x130E);gabi::store<s16>(a+0x130E,(s16)(state+1));
            gabi::store<s16>(a+0x1330,(s16)(gabi::load<s16>(t+0x82)+80));
        }
    } else if(state==2) {
        f32 step=gabi::fadds_ppc(gabi::load<f32>(t+0x20),gabi::load<f32>(0x1000B890));
        gabi::call(0x0200EDC8,gabi::at<void>(a+0x132C),one,step);
        if(gabi::load<s16>(a+0x1330)==0) {
            gabi::store<f32>(a+0x370,zero);gabi::store<f32>(a+0x1324,zero);gabi::store<s16>(a+0x130A,1);
            gabi::store<s16>(a+0x130E,0);gabi::store<s8>(a+0x30A7,2);gabi::store<s8>(a+0x30A6,2);
        }
    }
    s16 frequency=gabi::load<s16>(t+0x8C),frame=gabi::load<s16>(a+0x1308);
    f32 strength=gabi::load<f32>(a+0x132C),sine=gabi::load<f32>(0x104A44F8+((u16)(frame*(frequency+700))>>3)*8);
    gabi::store<s16>(a+0x328,(s16)gabi::ftoi(gabi::fmuls_ppc(sine,strength)));
    frequency=gabi::load<s16>(t+0x8C);frame=gabi::load<s16>(a+0x1308);strength=gabi::load<f32>(a+0x132C);
    sine=gabi::load<f32>(0x104A44F8+((u16)(frame*(frequency+600))>>3)*8);
    f32 fourHundred=gabi::load<f32>(0x1000B93C),forty=gabi::load<f32>(0x1000B88C);
    s16 roll=(s16)gabi::ftoi(gabi::fmuls_ppc(sine,strength));f32 progress=gabi::load<f32>(a+0x1324);
    gabi::store<s16>(a+0x32C,roll);f32 height=gabi::load<f32>(t+0x34);
    f32 step=gabi::fmuls_ppc(forty,progress),ratio=gabi::load<f32>(0x1000B864);
    gabi::call(0x0200ED84,gabi::at<void>(a+0x318),gabi::fadds_ppc(height,fourHundred),ratio,step);
    f32 shake=gabi::fadds_ppc(gabi::load<f32>(t+0x34),gabi::load<f32>(0x1000B890));step=gabi::load<f32>(0x1000B924);
    gabi::call(0x0200ED84,gabi::at<void>(a+0x1328),shake,one,step);
    gabi::call(0x0200ED84,gabi::at<void>(a+0x1324),one,one,gabi::load<f32>(0x1000B8EC));
}

// Inlined stay action 020EB658–020EBAF4; pending integration.
#include "d/actor/d_a_bst.h"
static void bstDispatchStay(bst_class* actor,f32 zero,f32 one,f32 ten) {
    u32 a=gabi::ea(actor),t=0x1047B608;s16 state=gabi::load<s16>(a+0x130E);
    gabi::store<u32>(a+0x39C,0);gabi::store<s16>(a+0x133C,10);
    if(state==0 || state==2) {
        u8 part=gabi::load<u8>(a+0x3D0);f32 two=gabi::load<f32>(0x1000B924);
        u16 animation=gabi::load<u16>((state==0?0x10192870:0x10192890)+part*2);
        gabi::call(0x020E47D0,actor,animation,two,state==0?2:0,one,-1);
        state=gabi::load<s16>(a+0x130E);gabi::store<s16>(a+0x130E,(s16)(state+1));return;
    }
    if(state!=4 && state!=5) return;
    if(state==4) {
        gabi::Local<cXyz> local,transformed,sum;local->x=zero;local->y=zero;
        u8 part=gabi::load<u8>(a+0x3D0);
        if(part) {
            u16 animation=gabi::load<u16>(0x10192898+part*2);
            gabi::call(0x020E47D0,actor,animation,ten,0,one,-1);
            local->z=gabi::fadds_ppc(gabi::load<f32>(t+0x34),gabi::load<f32>(0x1000B93C));
        } else local->z=gabi::load<f32>(0x1000B8D8);
        part=gabi::load<u8>(a+0x3D0);u16 effect=gabi::load<u16>(0x101928A0+part*2);
        s8 room=gabi::load<s8>(a+0x326);void* play=gabi::call<void*>(0x025200D4);
        u32 control=gabi::load<u32>(gabi::ea(play)+0x5AB0);
        gabi::call(0x025A847C,gabi::at<void>(control),2,effect,gabi::at<void>(a+0x314),gabi::at<void>(a+0x320),nullptr,185,gabi::at<void>(a+0x3134),room,0,nullptr,nullptr);
        play=gabi::call<void*>(0x025200D4);s16 strength=gabi::load<s16>(t+0x84);
        gabi::Local<cXyz> impulse;impulse->z=zero;impulse->y=one;impulse->x=zero;
        gabi::call(0x025CB374,gabi::at<void>(gabi::ea(play)+0x599C),strength+3,-33,impulse.get());
        f32 shake=gabi::fadds_ppc(gabi::load<f32>(t+0x30),gabi::load<f32>(0x1000B964));
        u32 head=gabi::load<u32>(0x10462988);gabi::store<f32>(head+0x3100,shake);
        state=gabi::load<s16>(a+0x130E);gabi::store<s16>(a+0x130E,(s16)(state+1));
        u32 matrix=gabi::load<u32>(0x1018C7B0);s16 yaw=gabi::load<s16>(a+0x322);
        gabi::call(0x025F1884,gabi::at<void>(matrix),yaw);
        gabi::call(0x0200FCD8,local.get(),transformed.get());
        gabi::call(0x0201AD78,gabi::at<void>(a+0x314),sum.get(),transformed.get());
        actor->mTargetPos.copy(*sum);gabi::store<s16>(a+0x1336,15);
    }
    if(gabi::load<u8>(a+0x3D0)!=0 && gabi::load<s16>(a+0x1336)!=0) {
        for(u32 i=0;i<15;++i) {
            u32 morph=gabi::load<u32>(a+0x3D4),model=gabi::load<u32>(morph+0x90),block=gabi::load<u32>(model+0x2C);
            u16 flags=gabi::load<u16>(block+4);u32 matrices=gabi::load<u32>(block+0x10);
            gabi::store<u16>(block+4,flags|0x10);u32 matrix=gabi::load<u32>(0x1018C7B0);
            gabi::call(0x028E90D4,gabi::at<void>(matrices+(i+2)*0x30),gabi::at<void>(matrix));
            gabi::Local<cXyz> origin,position;origin->x=zero;origin->y=zero;origin->z=zero;
            gabi::call(0x0200FCD8,origin.get(),position.get());void* play=gabi::call<void*>(0x025200D4);
            u32 control=gabi::load<u32>(gabi::ea(play)+0x5AB0);
            gabi::call(0x025A8D40,gabi::at<void>(control),0x81D5,position.get(),255,gabi::at<void>(0x101D5E98),gabi::at<void>(0x101D5E98),0);
        }
    }
    f32 target=gabi::load<f32>(a+0x1310),ratio=gabi::load<f32>(0x1000B864);
    gabi::call(0x0200ED84,gabi::at<void>(a+0x314),target,ratio,ten);
    ratio=gabi::load<f32>(0x1000B864);target=gabi::load<f32>(a+0x1318);
    gabi::call(0x0200ED84,gabi::at<void>(a+0x31C),target,ratio,ten);
}

// Down attack, reconstructed from 020ED348–020ED9E4.
static void bstDownSound(u32 a,u32 sound) {
 if(a+0x314) {s8 room=gabi::load<s8>(a+0x326);s8 reverb=gabi::call<s8>(0x02520540,room);gabi::call(0x025E1A40,sound,gabi::at<void>(a+0x314),0,reverb);}
}
static void bstDownApproach(u32 a,u32 axis,f32 half) {
 f32 velocity=gabi::load<f32>(a+0x33C+axis),target=gabi::load<f32>(a+0x1310+axis);
 gabi::call(0x0200ED84,gabi::at<void>(a+0x314+axis),target,half,std::fabs(velocity));
}
static f32 bstDownDistance(u32 a) {
 gabi::Local<cXyz> delta,copy;gabi::call(0x0201ADE0,gabi::at<void>(a+0x1310),delta.get(),gabi::at<void>(a+0x314));copy->copy(*delta);
 f32 square=gabi::call<f32>(0x028E8DD0,copy.get());return gabi::call<f32>(0x028F4384,square);
}
static void bstDispatchDown(bst_class* actor,f32 zero,f32 one,f32 half,f32 ten) {
 u32 a=gabi::ea(actor),t=0x1047B608;u32 play=gabi::ea(gabi::call<void*>(0x025200D4));u32 player=gabi::load<u32>(play+0x5B2C);
 bool normalEnd=false,abort=false;gabi::call(0x0200F428,gabi::at<void>(a+0x328),0,10,0x200);
 s16 state=gabi::load<s16>(a+0x130E);
 if(state==0) {
  gabi::store<f32>(a+0x1324,zero);gabi::store<s16>(a+0x130E,(s16)(state+1));
  f32 x=gabi::load<f32>(player+0x314);gabi::store<f32>(a+0x1310,x);f32 y=gabi::load<f32>(player+0x318);gabi::store<f32>(a+0x1314,y);
  f32 z=gabi::load<f32>(player+0x31C),threeHundred=gabi::load<f32>(0x1000B878);gabi::store<f32>(a+0x1318,z);gabi::store<s16>(a+0x1330,100);
  f32 height=gabi::load<f32>(t+0x24),targetY=gabi::load<f32>(a+0x1314);gabi::store<f32>(a+0x1314,gabi::fadds_ppc(targetY,gabi::fadds_ppc(height,threeHundred)));state=1;
 }
 if(state==1) {
  f32 step=gabi::fadds_ppc(gabi::load<f32>(t+0x3C),one);gabi::call(0x0200ED84,gabi::at<void>(a+0x370),gabi::load<f32>(0x1000B88C),one,step);
  f32 turn=gabi::fadds_ppc(gabi::load<f32>(t+0x1C),gabi::load<f32>(0x1000B930));gabi::store<f32>(a+0x1320,turn);
  f32 distance=bstDownDistance(a),boundary=gabi::fadds_ppc(gabi::load<f32>(t+0x38),gabi::load<f32>(0x1000B874));
  if(distance<boundary || gabi::load<s16>(a+0x1330)==0) gabi::store<s16>(a+0x130E,(s16)(gabi::load<s16>(a+0x130E)+1));
  gabi::call(0x0200F428,gabi::at<void>(a+0x32A),gabi::load<s16>(a+0x322),4,0x400);gabi::call(0x020E4CD0,actor,0);
 } else if(state==2) {
  bstDownApproach(a,0,half);bstDownApproach(a,4,half);bstDownApproach(a,8,half);
  gabi::call(0x0200F428,gabi::at<void>(a+0x32A),gabi::load<s16>(a+0x322),4,0x4000);
  f32 distance=bstDownDistance(a),boundary=gabi::fadds_ppc(gabi::load<f32>(t+0x3C),gabi::load<f32>(0x1000B890));
  if(distance<boundary) {
   u8 part=gabi::load<u8>(a+0x3D0);u16 animation=gabi::load<u16>(0x101928C8+part*2);gabi::call(0x020E47D0,actor,animation,ten,2,one,-1);
   gabi::store<s16>(a+0x130E,(s16)(gabi::load<s16>(a+0x130E)+1));gabi::store<f32>(a+0x340,zero);gabi::store<s16>(a+0x1330,30);
   s16 debug=gabi::load<s16>(t+0x80),yaw=gabi::load<s16>(a+0x32A);part=gabi::load<u8>(a+0x3D0);
   gabi::store<s16>(a+0x30A2,(s16)(yaw+(debug==1?0x4000:-0x4000)));gabi::store<s16>(a+0x30A4,part==2?0x4000:-0x4000);bstDownSound(a,0x58D8);
  }
 } else if(state==3) {
  gabi::call(0x0200ED84,gabi::at<void>(a+0x1348),gabi::load<f32>(0x1000B938),one,gabi::load<f32>(0x1000B978));
  bstDownApproach(a,0,half);bstDownApproach(a,8,half);
  gabi::call(0x0200F428,gabi::at<void>(a+0x32C),gabi::load<s16>(a+0x30A4),4,0x1000);gabi::call(0x0200F428,gabi::at<void>(a+0x32A),gabi::load<s16>(a+0x30A2),4,0x1000);
  f32 y=gabi::load<f32>(a+0x318),velocity=gabi::load<f32>(a+0x340);s16 timer=gabi::load<s16>(a+0x1330);gabi::store<f32>(a+0x318,gabi::fadds_ppc(y,velocity));
  if(timer) {
   f32 acceleration=gabi::fadds_ppc(gabi::load<f32>(t+0x40),half);velocity=gabi::load<f32>(a+0x340);u32 flags=gabi::load<u32>(a+0x13CC);gabi::store<f32>(a+0x340,gabi::fadds_ppc(velocity,acceleration));abort=(flags&0x10)!=0;
  } else {
   gabi::call(0x020E474C,actor,1);f32 acceleration=gabi::fadds_ppc(gabi::load<f32>(t+0x14),gabi::load<f32>(0x1000B8D4));velocity=gabi::load<f32>(a+0x340);gabi::store<f32>(a+0x340,gabi::fsubs_ppc(velocity,acceleration));
   f32 floor=gabi::fadds_ppc(gabi::load<f32>(t+0x18),gabi::load<f32>(0x1000B898));y=gabi::load<f32>(a+0x318);
   if(!(y>floor)) {
    gabi::store<f32>(a+0x318,floor);gabi::store<f32>(a+0x340,zero);play=gabi::ea(gabi::call<void*>(0x025200D4));s16 strength=gabi::load<s16>(t+0x84);gabi::Local<cXyz> impulse;impulse->x=zero;impulse->y=one;impulse->z=zero;
    gabi::call(0x025CB374,gabi::at<void>(play+0x599C),strength+5,-33,impulse.get());gabi::store<s16>(a+0x135E,(s16)(gabi::load<s16>(t+0x8E)+10));bstDownSound(a,0x6992);
    gabi::store<s16>(a+0x130E,(s16)(gabi::load<s16>(a+0x130E)+1));gabi::store<s16>(a+0x1330,30);
    gabi::Local<cXyz> position;position->x=gabi::load<f32>(a+0x314);position->y=gabi::load<f32>(a+0x318);position->z=gabi::load<f32>(a+0x31C);position->y=one;
    play=gabi::ea(gabi::call<void*>(0x025200D4));u32 control=gabi::load<u32>(play+0x5AB0);gabi::call(0x025A847C,gabi::at<void>(control),4,0xC1D6,position.get(),nullptr,nullptr,255,nullptr,-1,0,nullptr,nullptr);
    s8 room=gabi::load<s8>(a+0x326);play=gabi::ea(gabi::call<void*>(0x025200D4));control=gabi::load<u32>(play+0x5AB0);gabi::call(0x025A847C,gabi::at<void>(control),2,0xA1D8,position.get(),nullptr,nullptr,185,gabi::at<void>(a+0x3134),room,0,nullptr,nullptr);
   }
  }
 } else if(state==4) {
  gabi::call(0x0200ED84,gabi::at<void>(a+0x1348),gabi::load<f32>(0x1000B938),one,gabi::load<f32>(0x1000B978));
  if(gabi::load<s16>(a+0x1330)==0) {normalEnd=true;abort=true;}
 } else gabi::call(0x020E4CD0,actor,0);
 if(!normalEnd) abort=abort || (gabi::load<u32>(a+0x13CC)&0x10)!=0;
 if(abort) {
  gabi::store<s16>(a+0x130A,1);gabi::store<s16>(a+0x130E,0);if(a) bstDownSound(a,0x58D9);
  if(!normalEnd) {u8 part=gabi::load<u8>(a+0x3D0);u32 index=2u-part;if(index<2) {u32 address=0x10462978+index*4,other=gabi::load<u32>(address);if(gabi::load<s16>(other+0x130A)==10) {gabi::store<s16>(other+0x130A,1);other=gabi::load<u32>(address);gabi::store<s16>(other+0x130E,0);other=gabi::load<u32>(address);gabi::store<f32>(other+0x370,zero);}}}
 }
}

void daBst_move(bst_class* actor) {
 WWHD_FUNC(0x020EB4BC,void,actor);
 u32 a=gabi::ea(actor);u8 part=gabi::load<u8>(a+0x3D0);f32 zero=gabi::load<f32>(0x1000B854);
 if(part) {u32 head=gabi::load<u32>(0x10462988);s8 phase=gabi::load<s8>(head+0x30CE);if((u32)((s32)phase-1)<9) {s8 action=gabi::load<s8>(head+0x30B0);gabi::store<s16>(a+0x130A,action==11?1:6);gabi::store<s16>(a+0x130E,0);gabi::store<f32>(a+0x370,zero);return;}}
 gabi::call(0x020E474C,actor,0);gabi::call(0x020E4790);
 u32 flags=gabi::load<u32>(a+0x15A4);f32 one=gabi::load<f32>(0x1000B860);s16 action=gabi::load<s16>(a+0x130A);gabi::store<u32>(a+0x15A4,flags&~1u);
 f32 half=gabi::load<f32>(0x1000B86C),ten=gabi::load<f32>(0x1000B8BC);bool movement=false;
 switch(action) {
 case 0:bstDispatchStay(actor,zero,one,ten);movement=true;break;
 case 1:bstDispatchFly(actor,zero,one,half,ten);movement=true;break;
 case 5:bstDispatchDamage(actor,zero,one);break;
 case 6:bstDispatchSleep(actor,zero,one,ten);break;
 case 7:bstDispatchHeadDamage(actor,zero,one);break;
 case 8:bstDispatchRecover(actor,zero,one);break;
 case 10:bstDispatchDown(actor,zero,one,half,ten);movement=true;break;
 case 11:gabi::call(0x020E888C,actor);movement=true;break;
 case 12:gabi::call(0x020E90F0,actor);movement=true;break;
 case 13:gabi::call(0x020E9AA0,actor);movement=true;break;
 case 14:gabi::call(0x020E9F08,actor);break;
 case 20:gabi::call(0x020EACE0,actor);break;
 case 22:gabi::call(0x020EB174,actor);break;
 default:break;
 }
 if(movement) {
  gabi::Local<cXyz> delta;gabi::call(0x0201ADE0,gabi::at<void>(a+0x314),delta.get(),gabi::at<void>(a+0x300));
  f32 square=gabi::call<f32>(0x028E8DD0,delta.get()),distance=gabi::call<f32>(0x028F4384,square);f32 scaled=gabi::fmuls_ppc(distance,gabi::load<f32>(0x1000B97C)),limit=gabi::load<f32>(0x1000B980);
  u32 intensity=!(scaled<limit)?(u32)gabi::ftoi(gabi::fsubs_ppc(scaled,limit))+0x80000000u:(u32)gabi::ftoi(scaled);
  part=gabi::load<u8>(a+0x3D0);if(intensity>100)intensity=100;gabi::call(0x020E880C,actor,part?0x7036:0x7038,intensity);
 }
 gabi::call(0x020EA6B4,actor);
 if(a+0x1568) {
  f32 offset=gabi::load<f32>(a+0x1568),x=gabi::load<f32>(a+0x314);x=gabi::fadds_ppc(x,offset);f32 y=gabi::load<f32>(a+0x318);gabi::store<f32>(a+0x314,x);
  offset=gabi::load<f32>(a+0x156C);gabi::store<f32>(a+0x318,gabi::fadds_ppc(y,offset));offset=gabi::load<f32>(a+0x1570);f32 z=gabi::load<f32>(a+0x31C);gabi::store<f32>(a+0x31C,gabi::fadds_ppc(z,offset));
 }
 f32 recoil=gabi::load<f32>(a+0x1354),threshold=gabi::load<f32>(0x1000B984);
 if(recoil>threshold) {
  gabi::Local<cXyz> local,delta;local->x=zero;local->y=zero;local->z=recoil;u32 matrix=gabi::load<u32>(0x1018C7B0);s16 yaw=gabi::load<s16>(a+0x1358);gabi::call(0x025F1884,gabi::at<void>(matrix),yaw);
  matrix=gabi::load<u32>(0x1018C7B0);s16 pitch=gabi::load<s16>(a+0x135A);gabi::call(0x025F1BF4,gabi::at<void>(matrix),pitch);gabi::call(0x0200FCD8,local.get(),delta.get());
  gabi::call(0x028E8D88,gabi::at<void>(a+0x314),delta.get(),gabi::at<void>(a+0x314));gabi::call(0x0200EDC8,gabi::at<void>(a+0x1354),one,gabi::load<f32>(0x1000B988));
 }
}
VERIFY(0x020EB4BC,daBst_move);
