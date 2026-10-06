#include "d/actor/d_a_bst.h"

static void kumiSound(u32 actor,u32 sound,bool checkActor=false) {
    if ((!checkActor || actor) && actor+0x314) {
        s8 room=gabi::load<s8>(actor+0x326);
        s8 reverb=gabi::call<s8>(0x02520540,room);
        gabi::call(0x025E1A40,sound,gabi::at<void>(actor+0x314),0,reverb);
    }
}

static f32 kumiDistance(u32 first,u32 second) {
    gabi::Local<cXyz> result,difference;
    gabi::call(0x0201ADE0,gabi::at<void>(first),result.get(),gabi::at<void>(second));
    difference->copy(*result);
    f32 square=gabi::call<f32>(0x028E8DD0,difference.get());
    return gabi::call<f32>(0x028F4384,square);
}

static void kumi_attack(bst_class* actor) {
    WWHD_FUNC(0x020E90F0,void,actor);
    u32 address=gabi::ea(actor),tuning=0x1047B608;
    void* play=gabi::call<void*>(0x025200D4);
    u32 player=gabi::load<u32>(gabi::ea(play)+0x5B2C);
    gabi::call(0x0200F428,gabi::at<void>(address+0x328),0,10,0x200);
    f32 one=gabi::load<f32>(0x1000B860),ten=gabi::load<f32>(0x1000B8BC);
    f32 thirty=gabi::load<f32>(0x1000B888);
    gabi::call(0x0200ED84,gabi::at<void>(address+0x1344),thirty,one,ten);
    f32 target=gabi::load<f32>(0x1000B934);
    gabi::call(0x0200ED84,gabi::at<void>(address+0x134C),target,one,ten);
    thirty=gabi::load<f32>(0x1000B888);target=gabi::load<f32>(0x1000B938);
    gabi::call(0x0200ED84,gabi::at<void>(address+0x1348),target,one,thirty);
    s16 state=gabi::load<s16>(address+0x130E);
    f32 zero=gabi::load<f32>(0x1000B854),hundred=gabi::load<f32>(0x1000B8B4);
    bool move=true,success=false;
    switch (state) {
    case 0: {
        u8 part=gabi::load<u8>(address+0x3D0);
        gabi::store<f32>(address+0x1324,zero);gabi::store<s16>(address+0x130E,(s16)(state+1));
        s16 yaw=gabi::load<s16>(player+0x32A);gabi::store<s16>(address+0x1350,yaw);
        u16 animation=gabi::load<u16>(0x101928D8+part*2);
        gabi::call(0x020E47D0,actor,animation,ten,2,one,-1);
        s16 duration=gabi::load<s16>(tuning+0x82);gabi::store<s16>(address+0x1330,(s16)(duration+70));
        [[fallthrough]];
    }
    case 1: {
        f32 distance=kumiDistance(address+0x1310,address+0x314);
        f32 boundary=gabi::load<f32>(tuning+0x38)+gabi::load<f32>(0x1000B874);
        f32 acceleration=gabi::load<f32>(tuning+0x3C);
        if (distance>boundary) {
            f32 two=gabi::load<f32>(0x1000B924),forty=gabi::load<f32>(0x1000B88C);
            gabi::call(0x0200ED84,gabi::at<void>(address+0x370),forty,one,acceleration+two);
        } else {
            f32 two=gabi::load<f32>(0x1000B924);
            gabi::call(0x0200EDC8,gabi::at<void>(address+0x370),one,acceleration+two);
        }
        f32 turn=gabi::load<f32>(tuning+0x1C)+gabi::load<f32>(0x1000B928);
        s16 yaw=gabi::load<s16>(address+0x1350);gabi::store<f32>(address+0x1320,turn);
        u32 matrix=gabi::load<u32>(0x1018C7B0);gabi::call(0x025F1884,gabi::at<void>(matrix),yaw);
        u8 part=gabi::load<u8>(address+0x3D0);
        f32 x=gabi::load<f32>(tuning+0x10)+gabi::load<f32>(0x1000B8B0);
        if (part==1) x=-x;
        f32 y=gabi::load<f32>(tuning+0x14)+gabi::load<f32>(0x1000B93C);
        gabi::Local<cXyz> local,transformed,result;
        local->x=x;local->z=zero;local->y=y;
        gabi::call(0x0200FCD8,local.get(),transformed.get());
        gabi::call(0x0201AD78,gabi::at<void>(player+0x314),result.get(),transformed.get());
        yaw=gabi::load<s16>(address+0x1350);
        actor->mTargetPos.copy(*result);
        gabi::call(0x0200F428,gabi::at<void>(address+0x32A),(s16)(yaw+(part==1 ? 0x4000 : -0x4000)),10,0x800);
        if (gabi::load<s16>(address+0x1330)==0) {
            s16 current=gabi::load<s16>(address+0x130E);f32 fiveHundred=gabi::load<f32>(0x1000B8B0);
            gabi::store<s16>(address+0x130E,(s16)(current+1));
            f32 px=gabi::load<f32>(player+0x314);gabi::store<f32>(address+0x1310,px);
            f32 py=gabi::load<f32>(player+0x318);gabi::store<f32>(address+0x1314,py);
            f32 pz=gabi::load<f32>(player+0x31C);gabi::store<f32>(address+0x1318,pz);
            f32 height=gabi::load<f32>(tuning+0x14)+fiveHundred;
            gabi::store<f32>(address+0x1314,py+height);
            play=gabi::call<void*>(0x025200D4);u32 currentPlayer=gabi::load<u32>(gabi::ea(play)+0x5B2C);
            s16 angle=gabi::call<s16>(0x025D6894,actor,gabi::at<void>(currentPlayer));
            gabi::store<s16>(address+0x322,angle);
        }
        break;
    }
    case 2: {
        f32 speed=gabi::load<f32>(tuning+0x40),acceleration=gabi::load<f32>(tuning+0x44);
        f32 eight=gabi::load<f32>(0x1000B940);
        gabi::call(0x0200ED84,gabi::at<void>(address+0x370),speed+hundred,one,acceleration+eight);
        f32 turn=gabi::load<f32>(tuning+0x1C)+gabi::load<f32>(0x1000B8D8);gabi::store<f32>(address+0x1320,turn);
        f32 distance=kumiDistance(address+0x1310,address+0x314);
        f32 boundary=gabi::load<f32>(tuning+0x48)+hundred;
        if (distance<boundary) {
            s16 current=gabi::load<s16>(address+0x130E);gabi::store<s16>(address+0x130E,(s16)(current+1));
        }
        break;
    }
    case 3: {
        u8 part=gabi::load<u8>(address+0x3D0);s16 yaw=gabi::load<s16>(address+0x1350);
        gabi::call(0x0200F428,gabi::at<void>(address+0x32A),(s16)(yaw+(part==1 ? 0x4000 : -0x4000)),1,0x1000);
        f32 speed=gabi::load<f32>(address+0x33C),goal=gabi::load<f32>(address+0x1310);
        gabi::call(0x0200ED84,gabi::at<void>(address+0x314),goal,one,std::fabs(speed));
        speed=gabi::load<f32>(address+0x340);goal=gabi::load<f32>(address+0x1314);
        gabi::call(0x0200ED84,gabi::at<void>(address+0x318),goal,one,std::fabs(speed)+ten);
        speed=gabi::load<f32>(address+0x344);goal=gabi::load<f32>(address+0x1318);
        gabi::call(0x0200ED84,gabi::at<void>(address+0x31C),goal,one,std::fabs(speed));
        u32 left=gabi::load<u32>(0x10462978),right=gabi::load<u32>(0x1046297C);
        f32 distance=kumiDistance(left+0x314,right+0x314);
        if (distance<one) {
            s16 current=gabi::load<s16>(address+0x130E);gabi::store<s16>(address+0x130E,(s16)(current+1));
            s16 duration=gabi::load<s16>(tuning+0x86);gabi::store<s16>(address+0x1330,(s16)(duration+10));
            kumiSound(address,0x58DA);gabi::store<f32>(address+0x340,zero);
        }
        move=false;break;
    }
    case 4: {
        f32 velocity=gabi::load<f32>(address+0x340),y=gabi::load<f32>(address+0x318);
        s16 timer=gabi::load<s16>(address+0x1330);gabi::store<f32>(address+0x318,y+velocity);
        if (timer>1) {
            f32 acceleration=gabi::load<f32>(tuning+0x40),current=gabi::load<f32>(address+0x340);
            u8 part=gabi::load<u8>(address+0x3D0);gabi::store<f32>(address+0x340,current+(acceleration+one));
            s16 roll=part==2 ? -0x1000 : 0x1000;gabi::store<s16>(address+0x30A4,roll);
            gabi::call(0x0200F428,gabi::at<void>(address+0x32C),roll,10,0x200);
        } else {
            u8 part=gabi::load<u8>(address+0x3D0);s16 roll=part==2 ? 0x4000 : -0x4000;
            gabi::store<s16>(address+0x30A4,roll);gabi::call(0x0200F428,gabi::at<void>(address+0x32C),roll,1,0x1000);
            gabi::call(0x020E474C,actor,1);
            f32 acceleration=gabi::load<f32>(tuning+0x14)+gabi::load<f32>(0x1000B8D4);
            velocity=gabi::load<f32>(address+0x340);gabi::store<f32>(address+0x340,velocity-acceleration);
            f32 floor=gabi::load<f32>(tuning+0x18)+gabi::load<f32>(0x1000B898);
            y=gabi::load<f32>(address+0x318);
            if (!(y>floor)) {
                gabi::store<f32>(address+0x318,floor);part=gabi::load<u8>(address+0x3D0);gabi::store<f32>(address+0x340,zero);
                if (part==1) {
                    gabi::Local<cXyz> impact,impulse;
                    impact->x=gabi::load<f32>(address+0x1310);impact->y=one;impact->z=gabi::load<f32>(address+0x1318);
                    play=gabi::call<void*>(0x025200D4);u32 particles=gabi::load<u32>(gabi::ea(play)+0x5AB0);
                    gabi::call(0x025A847C,gabi::at<void>(particles),4,0xC1D6,impact.get(),nullptr,nullptr,255,nullptr,-1,0,nullptr,nullptr);
                    s8 room=gabi::load<s8>(address+0x326);play=gabi::call<void*>(0x025200D4);particles=gabi::load<u32>(gabi::ea(play)+0x5AB0);
                    gabi::call(0x025A847C,gabi::at<void>(particles),2,0xA1D9,impact.get(),nullptr,nullptr,185,gabi::at<void>(address+0x3134),room,0,nullptr,nullptr);
                    play=gabi::call<void*>(0x025200D4);s32 strength=gabi::load<s16>(tuning+0x84)+5;
                    impulse->x=zero;impulse->y=one;impulse->z=zero;
                    gabi::call(0x025CB374,gabi::at<void>(gabi::ea(play)+0x599C),strength,-33,impulse.get());
                    kumiSound(address,0x6993);
                }
                s16 current=gabi::load<s16>(address+0x130E),duration=gabi::load<s16>(tuning+0x8E);
                gabi::store<s16>(address+0x130E,(s16)(current+1));gabi::store<s16>(address+0x135E,(s16)(duration+10));
                gabi::store<s16>(address+0x1330,40);
            }
        }
        move=false;break;
    }
    case 5:
        if (gabi::load<s16>(address+0x1330)==0) success=true;
        move=false;break;
    }
    if (move) gabi::call(0x020E4CD0,actor,0);
    bool collision=false;
    if (!success) collision=(gabi::load<u32>(address+0x13CC)&0x10)!=0;
    if (!success && !collision) return;
    s16 yaw=gabi::load<s16>(address+0x322);
    gabi::store<f32>(address+0x370,zero);gabi::store<s16>(address+0x130A,1);
    gabi::store<s16>(address+0x130E,0);gabi::store<s16>(address+0x322,(s16)(yaw-0x8000));
    f32 random=gabi::call<f32>(0x020198D8,hundred);
    gabi::store<s16>(address+0x133E,(s16)gabi::ftoi(random+hundred));gabi::store<s16>(address+0x133C,40);
    kumiSound(address,0x58DB);
    if (success) return;
    u32 other=2u-gabi::load<u8>(address+0x3D0);
    if (other>=2) return;
    u32 global=0x10462978+other*4,hand=gabi::load<u32>(global);
    if (gabi::load<s16>(hand+0x130A)!=12) return;
    gabi::store<s16>(hand+0x130A,1);hand=gabi::load<u32>(global);gabi::store<s16>(hand+0x130E,0);
    hand=gabi::load<u32>(global);gabi::store<f32>(hand+0x370,zero);
    hand=gabi::load<u32>(global);yaw=gabi::load<s16>(hand+0x322);gabi::store<s16>(hand+0x322,(s16)(yaw-0x8000));
    hand=gabi::load<u32>(global);random=gabi::call<f32>(0x020198D8,hundred);
    gabi::store<s16>(hand+0x133E,(s16)gabi::ftoi(random+hundred));hand=gabi::load<u32>(global);gabi::store<s16>(hand+0x133C,40);
    hand=gabi::load<u32>(global);kumiSound(hand,0x58DB,true);
}
VERIFY(0x020E90F0,kumi_attack);
