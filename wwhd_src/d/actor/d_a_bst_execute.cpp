#include "d/actor/d_a_bst.h"
static void bstResumePedestal(u32 global,u32 controllerOffset,u32 resourceId,bool brk,f32 one) {
    u32 owner=gabi::load<u32>(global),model=gabi::load<u32>(owner+0x3E4);
    u32 resources=gabi::load<u32>(0x101F4F28);
    gabi::Local<be<u32>[2]> name;
    (*name)[0]=0x1000B70C;(*name)[1]=0x1000B714;
    void* resource=gabi::call<void*>(0x026066C4,gabi::at<void>(resources),name.get(),resourceId);
    u32 data=gabi::load<u32>(model+0xAC);owner=gabi::load<u32>(global);
    u32 controller=gabi::load<u32>(owner+controllerOffset);
    gabi::call(brk?0x025E8154:0x025E7CE0,gabi::at<void>(controller),gabi::at<void>(data),resource,1,2,0,-1,1,one,0);
}
static void bstExecuteHeadControl(bst_class* actor,f32 half,f32 one) {
    u32 a=gabi::ea(actor),t=0x1047B608;
    void* play=gabi::call<void*>(0x025200D4);
    s8 demo=gabi::load<s8>(a+0x30CE);u32 player=gabi::load<u32>(gabi::ea(play)+0x5B2C);
    if(demo) return;
    s16 timer=gabi::load<s16>(a+0x30B8);
    if(timer) gabi::store<s16>(a+0x30B8,(s16)(timer-1));
    for(u32 i=0;i<3;++i) {
        s16 value=gabi::load<s16>(a+0x30B2+i*2);
        if(value) gabi::store<s16>(a+0x30B2+i*2,(s16)(value-1));
    }
    s8 phase=gabi::load<s8>(a+0x30B0);
    if(phase==0) {gabi::store<s8>(a+0x30B0,1);return;}
    if(phase==1) {
        u32 save=gabi::load<u32>(0x101F84DC);
        BOOL complete=gabi::call<BOOL>(0x025B9100,gabi::at<void>(save+0x798),3);
        if(complete && gabi::load<s16>(t+4+0x86)==0) return;
        gabi::store<u32>(a+0x2E0,gabi::load<u32>(a+0x2E0)|0x20);
        save=gabi::load<u32>(0x101F84DC);
        if(gabi::call<BOOL>(0x025B9100,gabi::at<void>(save+0x798),5)) {
            gabi::store<u8>(a+0x3330,1);gabi::call(0x025E18EC,0x80000023);
            gabi::store<s16>(gabi::load<u32>(0x10462988)+0x130A,1);
            gabi::store<s16>(gabi::load<u32>(0x10462978)+0x130A,1);
            gabi::store<s16>(gabi::load<u32>(0x1046297C)+0x130A,1);
            gabi::store<s16>(gabi::load<u32>(0x10462988)+0x130E,0);
            gabi::store<s16>(gabi::load<u32>(0x1046297C)+0x130E,0);
            gabi::store<s16>(gabi::load<u32>(0x10462978)+0x130E,0);
            gabi::store<s8>(a+0x30B0,10);
            bstResumePedestal(0x10462978,0x3EC,0x4A,true,one);
            bstResumePedestal(0x10462978,0x3E8,0x64,false,one);
            bstResumePedestal(0x1046297C,0x3EC,0x4D,true,one);
            bstResumePedestal(0x1046297C,0x3E8,0x67,false,one);
            bstResumePedestal(0x10462988,0x3EC,0x42,true,one);
            bstResumePedestal(0x10462988,0x3E8,0x5C,false,one);
            u32 controller=gabi::load<u32>(a+0x332C);
            gabi::store<s16>(a+0x30B8,400);gabi::store<f32>(controller,one);
        } else {
            gabi::store<s16>(gabi::load<u32>(0x1046297C)+0x133C,10);
            gabi::store<s16>(gabi::load<u32>(0x10462978)+0x133C,10);
            gabi::store<s16>(gabi::load<u32>(0x10462988)+0x133C,10);
            f32 square=gabi::call<f32>(0x028E8DD0,gabi::at<void>(player+0x314));
            f32 distance=gabi::call<f32>(0x028F4384,square);
            if(distance<gabi::load<f32>(0x1000B870)) {
                gabi::store<s8>(a+0x30CE,10);gabi::store<s8>(a+0x30B0,2);
            }
        }
        return;
    }
    if(phase==10) {
        u32 status=gabi::load<u32>(a+0x2E0);
        gabi::store<s8>(a+0x30B0,(s8)(phase+1));
        gabi::store<u32>(a+0x2E0,status&~0x4000u);gabi::store<s16>(a+0x30B2,100);return;
    }
    if(phase==11) {
        s16 timer=gabi::load<s16>(a+0x30B2);u32 head=gabi::load<u32>(0x10462988);
        if(timer==0 && gabi::load<s16>(head+0x130A)!=14) {
            f32 twoHundred=gabi::load<f32>(0x1000B874),threeHundred=gabi::load<f32>(0x1000B878);
            gabi::call(0x024EFF44,gabi::at<void>(head+0x1364),twoHundred,threeHundred);
            twoHundred=gabi::load<f32>(0x1000B874);u32 hand=gabi::load<u32>(0x10462978);
            gabi::call(0x024EFF44,gabi::at<void>(hand+0x1364),twoHundred,twoHundred);
            twoHundred=gabi::load<f32>(0x1000B874);hand=gabi::load<u32>(0x1046297C);
            gabi::call(0x024EFF44,gabi::at<void>(hand+0x1364),twoHundred,twoHundred);
            f32 random=gabi::call<f32>(0x020198D8,one),threshold=gabi::load<f32>(0x1000B87C);
            auto setHand=[](u32 global,s16 action) {
                u32 hand=gabi::load<u32>(global);gabi::store<s16>(hand+0x130A,action);
                hand=gabi::load<u32>(global);gabi::store<s16>(hand+0x130E,0);
            };
            if(random<threshold || gabi::load<s16>(t+4+0x7E)!=0) {
                random=gabi::call<f32>(0x020198D8,one);u32 global=random<half?0x10462978:0x1046297C;
                hand=gabi::load<u32>(global);
                if(gabi::load<s16>(hand+0x130A)<10 && gabi::load<s16>(hand+0x133E)==0) setHand(global,13);
            } else {
                s8 health=gabi::load<s8>(a+0x3A1);threshold=gabi::load<f32>(0x1000B880);
                if(health==2) threshold=gabi::load<f32>(0x1000B884);
                else if(health==3) threshold=gabi::load<f32>(0x1000B87C);
                random=gabi::call<f32>(0x020198D8,one);
                bool pair=random<(health==1?half:threshold) || gabi::load<s16>(t+4+0x7C)!=0;
                s16 action=12;
                if(!pair) {
                    random=gabi::call<f32>(0x020198D8,one);pair=random<half;action=11;
                }
                if(pair) {
                    u32 first=gabi::load<u32>(0x10462978);
                    if(gabi::load<s16>(first+0x130A)<5) {
                        u32 second=gabi::load<u32>(0x1046297C);
                        if(gabi::load<s16>(second+0x130A)<5 && gabi::load<s16>(first+0x133E)==0 && gabi::load<s16>(second+0x133E)==0) {
                            setHand(0x10462978,action);setHand(0x1046297C,action);
                        }
                    }
                } else {
                    random=gabi::call<f32>(0x020198D8,one);u32 global=random<half?0x10462978:0x1046297C;
                    hand=gabi::load<u32>(global);
                    if(gabi::load<s16>(hand+0x130A)<5 && gabi::load<s16>(hand+0x133E)==0) setHand(global,10);
                }
            }
            s8 health=gabi::load<s8>(a+0x3A1);u32 maxAddress=health==0?0x1000B888:health==1?0x1000B88C:0x1000B890;
            f32 randomWait=gabi::call<f32>(0x020198D8,gabi::load<f32>(maxAddress));
            f32 base=gabi::load<f32>(health==0?0x1000B888:health==1?0x1000B890:0x1000B894);
            gabi::store<s16>(a+0x30B2,(s16)gabi::ftoi(gabi::fadds_ppc(randomWait,base)));
            head=gabi::load<u32>(0x10462988);
        }
        gabi::store<s16>(head+0x30AA,10);head=gabi::load<u32>(0x10462988);gabi::store<s16>(head+0x30A8,10);
        if(gabi::load<s16>(a+0x30B6)==0) {
            u32 hand=gabi::load<u32>(0x10462978);
            if(gabi::load<s16>(hand+0x130A)==6) {
                hand=gabi::load<u32>(0x1046297C);
                if(gabi::load<s16>(hand+0x130A)==6) {
                    gabi::store<s8>(a+0x30B0,12);
                    gabi::store<s16>(gabi::load<u32>(0x10462978)+0x1332,100);
                    gabi::store<s16>(gabi::load<u32>(0x1046297C)+0x1332,100);
                    head=gabi::load<u32>(0x10462988);
                    if(head && head+0x37C) {
                        s8 room=gabi::load<s8>(head+0x326),reverb=gabi::call<s8>(0x02520540,room);
                        gabi::call(0x025E1A40,0x58D7,gabi::at<void>(head+0x37C),0,reverb);
                    }
                    gabi::call(0x025E18EC,0x80000122);
                    f32 randomWait=gabi::call<f32>(0x020198D8,gabi::load<f32>(0x1000B890));
                    gabi::store<s16>(a+0x30B4,(s16)gabi::ftoi(gabi::fadds_ppc(randomWait,gabi::load<f32>(0x1000B898))));
                }
            }
        }
    } else if(phase==12) {
        u32 head;
        if(gabi::load<s16>(a+0x30B4)==0) {
            head=gabi::load<u32>(0x10462988);
            if(gabi::load<s16>(head+0x130A)==1) {
                gabi::store<s16>(head+0x130A,14);head=gabi::load<u32>(0x10462988);
                f32 minusFiveHundred=gabi::load<f32>(0x1000B89C);gabi::store<s16>(head+0x130E,0);
                f32 target=gabi::fadds_ppc(gabi::load<f32>(t+0x24),minusFiveHundred);
                head=gabi::load<u32>(0x10462988);gabi::store<f32>(head+0x1304,target);
            }
        }
        gabi::store<s16>(gabi::load<u32>(0x10462978)+0x130A,6);
        gabi::store<s16>(gabi::load<u32>(0x10462978)+0x1332,3);
        gabi::store<s16>(gabi::load<u32>(0x1046297C)+0x1332,3);
        head=gabi::load<u32>(0x10462988);
        if(gabi::load<s16>(head+0x130A)==7 && gabi::load<s16>(head+0x130E)>=7) {
            gabi::store<s8>(a+0x30B0,11);gabi::store<s16>(a+0x30B6,50);
        }
    }
}
#include "d/actor/d_a_bst.h"
static f32 bstSin(s32 angle) { return gabi::load<f32>(0x104A44F8+((u16)angle>>3)*8); }
static f32 bstCos(s32 angle) { return gabi::load<f32>(0x104A44FC+((u16)angle>>3)*8); }
static void bstExecuteTransformAndCollision(bst_class* actor,f32 zero,f32 half,f32 one) {
    u32 a=gabi::ea(actor),t=0x1047B608,matrix=0x1048D0CC;
    s16 frequency=gabi::load<s16>(t+4+0x88),frame=gabi::load<s16>(a+0x1308);
    f32 sx=bstSin(frame*(frequency+700));
    f32 magnitude=gabi::load<f32>(a+0x1328);
    f32 sy=bstSin(frame*(frequency+750)),cz=bstCos(frame*(frequency+720));
    gabi::call(0x0200EDC8,gabi::at<void>(a+0x1328),one,one);
    u32 morph=gabi::load<u32>(a+0x3D4);
    f32 scaleZ=gabi::load<f32>(a+0x338);u32 model=gabi::load<u32>(morph+0x90);
    f32 scaleX=gabi::load<f32>(a+0x330),scaleY=gabi::load<f32>(a+0x334);
    gabi::store<f32>(model+0xBC,scaleX);gabi::store<f32>(model+0xC0,scaleY);gabi::store<f32>(model+0xC4,scaleZ);
    f32 x=gabi::load<f32>(a+0x314),y=gabi::load<f32>(a+0x318),z=gabi::load<f32>(a+0x31C);
    gabi::call(0x028E93CC,gabi::at<void>(matrix),gabi::fmadds(sx,magnitude,x),gabi::fmadds(sy,magnitude,y),gabi::fmadds(cz,magnitude,z));
    s16 first=gabi::load<s16>(a+0x135C);frame=gabi::load<s16>(a+0x1308);
    f32 fiveHundred=gabi::load<f32>(0x1000B8B0),tuning=gabi::load<f32>(t+0x40);
    f32 firstStrength=gabi::fmuls_ppc((f32)first,gabi::fadds_ppc(tuning,fiveHundred));
    f32 sine=bstSin(frame*0x2100),cosine=bstCos(frame*0x2300);
    s16 second=gabi::load<s16>(a+0x135E);u32 head=gabi::load<u32>(0x10462988);
    s16 yaw=(s16)gabi::ftoi(gabi::fmuls_ppc(sine,firstStrength));
    s16 pitch=(s16)gabi::ftoi(gabi::fmuls_ppc(cosine,firstStrength));
    f32 oneFifty=gabi::load<f32>(0x1000B898);
    s16 headFrame=gabi::load<s16>(head+0x1308);
    f32 secondStrength=gabi::fmuls_ppc((f32)second,gabi::fadds_ppc(tuning,oneFifty));
    sine=bstSin(headFrame*0x3600);cosine=bstCos(headFrame*0x4300);
    s16 third=gabi::load<s16>(a+0x1360);
    s16 extraYaw=(s16)gabi::ftoi(gabi::fmuls_ppc(sine,secondStrength));
    s16 extraPitch=(s16)gabi::ftoi(gabi::fmuls_ppc(cosine,secondStrength));
    f32 hundred=gabi::load<f32>(0x1000B8B4);cosine=bstCos(headFrame*0x3A00);
    f32 thirdStrength=gabi::fmuls_ppc((f32)third,gabi::fadds_ppc(tuning,hundred));
    s16 lastPitch=(s16)gabi::ftoi(gabi::fmuls_ppc(cosine,thirdStrength));
    f32 thirty=gabi::load<f32>(0x1000B888),decay=gabi::load<f32>(t+4+0x10);
    yaw=(s16)(yaw+extraYaw);pitch=(s16)(pitch+extraPitch+lastPitch);
    gabi::call(0x0200EDC8,gabi::at<void>(a+0x132C),one,gabi::fadds_ppc(decay,thirty));
    s16 shape=gabi::load<s16>(a+0x32A);gabi::call(0x025F1C28,gabi::at<void>(matrix),(s16)(shape+yaw));
    shape=gabi::load<s16>(a+0x328);gabi::call(0x025F1BF4,gabi::at<void>(matrix),(s16)(shape+pitch));
    shape=gabi::load<s16>(a+0x32C);gabi::call(0x025F1C5C,gabi::at<void>(matrix),shape);
    x=gabi::load<f32>(a+0x1344);f32 ty=gabi::load<f32>(t+4+0x1C),tx=gabi::load<f32>(t+4+0x18);
    y=gabi::load<f32>(a+0x1348);f32 tz=gabi::load<f32>(t+4+0x20);z=gabi::load<f32>(a+0x134C);
    gabi::call(0x025F24E0,gabi::fadds_ppc(tx,x),gabi::fadds_ppc(ty,y),gabi::fadds_ppc(tz,z));
    gabi::call(0x0200EDC8,gabi::at<void>(a+0x1344),one,gabi::load<f32>(0x1000B8B8));
    gabi::call(0x0200EDC8,gabi::at<void>(a+0x1348),one,gabi::load<f32>(0x1000B8BC));
    gabi::call(0x0200EDC8,gabi::at<void>(a+0x134C),one,gabi::load<f32>(0x1000B8B8));
    f32 values[12];for(u32 i=0;i<12;++i) values[i]=gabi::load<f32>(matrix+i*4);
    for(u32 i=0;i<12;++i) gabi::store<f32>(model+0xC8+i*4,values[i]);
    for(u32 i=0;i<12;++i) values[i]=gabi::load<f32>(matrix+i*4);
    u32 decoration=gabi::load<u32>(a+0x5BC);
    for(u32 i=0;i<12;++i) gabi::store<f32>(decoration+0xC8+i*4,values[i]);
    morph=gabi::load<u32>(a+0x3D4);gabi::call(0x025E55A0,gabi::at<void>(morph));
    x=gabi::load<f32>(a+0x2EC);z=gabi::load<f32>(a+0x2F4);y=gabi::load<f32>(a+0x2F0);
    gabi::call(0x0200FAD8,x,y,z,0);
    shape=gabi::load<s16>(a+0x2FA);matrix=gabi::load<u32>(0x1018C7B0);
    gabi::call(0x025F1C28,gabi::at<void>(matrix),shape);
    shape=gabi::load<s16>(a+0x2F8);matrix=gabi::load<u32>(0x1018C7B0);
    gabi::call(0x025F1BF4,gabi::at<void>(matrix),shape);
    shape=gabi::load<s16>(a+0x2FC);matrix=gabi::load<u32>(0x1018C7B0);
    gabi::call(0x025F1C5C,gabi::at<void>(matrix),shape);
    matrix=gabi::load<u32>(0x1018C7B0);u32 pedestal=gabi::load<u32>(a+0x3E4);
    for(u32 i=0;i<12;++i) values[i]=gabi::load<f32>(matrix+i*4);
    for(u32 i=0;i<12;++i) gabi::store<f32>(pedestal+0xC8+i*4,values[i]);
    if(gabi::load<u8>(a+0x3D0)==0) {
        morph=gabi::load<u32>(a+0x3D4);model=gabi::load<u32>(morph+0x90);
        u32 block=gabi::load<u32>(model+0x2C);u16 flags=gabi::load<u16>(block+4);
        u32 matrices=gabi::load<u32>(block+0x10);gabi::store<u16>(block+4,flags|0x10);
        matrix=gabi::load<u32>(0x1018C7B0);gabi::call(0x028E90D4,gabi::at<void>(matrices+0x180),gabi::at<void>(matrix));
        f32 tx=gabi::load<f32>(t+4+4),minusThirty=gabi::load<f32>(0x1000B8C0),minusHundred=gabi::load<f32>(0x1000B8C4);
        f32 tz=gabi::load<f32>(t+4+0xC),ty=gabi::load<f32>(t+4+8);
        gabi::Local<cXyz> local,transformed;local->y=ty;local->x=gabi::fadds_ppc(tx,minusThirty);local->z=gabi::fadds_ppc(tz,minusHundred);
        gabi::call(0x0200FCD8,local.get(),transformed.get());
        gabi::call(0x020182E0,gabi::at<void>(a+0x16BC),transformed.get());
        f32 height=gabi::load<f32>(t+4+0x10),sixHundred=gabi::load<f32>(0x1000B8C8);
        gabi::call(0x02018428,gabi::at<void>(a+0x16BC),gabi::fadds_ppc(height,sixHundred));
        f32 radius=gabi::load<f32>(t+4+0x14),oneEighty=gabi::load<f32>(0x1000B8CC);
        gabi::call(0x020184DC,gabi::at<void>(a+0x16BC),gabi::fadds_ppc(radius,oneEighty));
        void* play=gabi::call<void*>(0x025200D4);
        gabi::call(0x0200E240,gabi::at<void>(gabi::ea(play)+0x26A4),gabi::at<void>(a+0x15A4));
        u32 collisionFlags=gabi::load<u32>(a+0x16EC);
        f32 oneFifty=gabi::load<f32>(0x1000B898),seventy=gabi::load<f32>(0x1000B894);
        gabi::store<u32>(a+0x16EC,collisionFlags&~1u);
        tx=gabi::load<f32>(t+4+0x18);ty=gabi::load<f32>(t+4+0x1C);tz=gabi::load<f32>(t+4+0x20);
        local->y=ty;local->x=gabi::fadds_ppc(tx,oneFifty);local->z=gabi::fadds_ppc(tz,seventy);
        gabi::call(0x0200FCD8,local.get(),transformed.get());
        gabi::call(0x020182E0,gabi::at<void>(a+0x17EC),transformed.get());
        height=gabi::load<f32>(t+4+0x24);gabi::call(0x02018428,gabi::at<void>(a+0x17EC),gabi::fadds_ppc(height,hundred));
        radius=gabi::load<f32>(t+4+0x28);gabi::call(0x020184DC,gabi::at<void>(a+0x17EC),gabi::fadds_ppc(radius,hundred));
        play=gabi::call<void*>(0x025200D4);gabi::call(0x0200E240,gabi::at<void>(gabi::ea(play)+0x26A4),gabi::at<void>(a+0x16D4));
        f32 eyeOffset=gabi::load<f32>(t+4+0x30),twoFifty=gabi::load<f32>(0x1000B8D0);
        local->z=zero;local->y=zero;local->x=gabi::fadds_ppc(eyeOffset,twoFifty);
        gabi::call(0x0200FCD8,local.get(),gabi::at<void>(a+0x37C));
        local->x=zero;local->y=zero;local->z=zero;
        f32 hiddenHeight=gabi::load<f32>(0x1000B8D8),sixty=gabi::load<f32>(0x1000B8D4);
        for(u32 i=0;i<2;++i) {
            morph=gabi::load<u32>(a+0x3D4);model=gabi::load<u32>(morph+0x90);block=gabi::load<u32>(model+0x2C);
            flags=gabi::load<u16>(block+4);matrices=gabi::load<u32>(block+0x10);gabi::store<u16>(block+4,flags|0x10);
            matrix=gabi::load<u32>(0x1018C7B0);gabi::call(0x028E90D4,gabi::at<void>(matrices+(i+4)*0x30),gabi::at<void>(matrix));
            gabi::call(0x0200FCD8,local.get(),transformed.get());
            s16 timer=gabi::load<s16>(a+0x30A8+i*2);bool open=false;
            if(!timer) open=gabi::load<s8>(a+0x30A6+i)>0;
            else gabi::store<s16>(a+0x30A8+i*2,(s16)(timer-1));
            if(open) gabi::call(0x0200F428,gabi::at<void>(a+0x30AC+i*2),0,8,0x200);
            else {
                s16 target=(s16)(gabi::load<s16>(t+4+0x8C)+11000);
                transformed->y=gabi::fadds_ppc((f32)transformed->y,hiddenHeight);
                gabi::call(0x0200F428,gabi::at<void>(a+0x30AC+i*2),target,1,0x400);
            }
            u32 sphere=a+0x2E48+i*0x12C;
            gabi::call(0x02018D40,gabi::at<void>(sphere+0x118),transformed.get());
            radius=gabi::load<f32>(t+4+0x3C);gabi::call(0x02018C8C,gabi::at<void>(sphere+0x118),gabi::fadds_ppc(radius,sixty));
            play=gabi::call<void*>(0x025200D4);gabi::call(0x0200E240,gabi::at<void>(gabi::ea(play)+0x26A4),gabi::at<void>(sphere));
        }
    } else {
        s16 action=gabi::load<s16>(a+0x130A);gabi::Local<cXyz> local,transformed;
        local->y=zero;local->z=zero;local->x=zero;f32 extraRadius=zero;
        if(action==13) extraRadius=gabi::fadds_ppc(gabi::load<f32>(t+4+0x44),gabi::load<f32>(0x1000B890));
        f32 fifty=gabi::load<f32>(0x1000B890);
        for(u32 i=0;i<15;++i) {
            morph=gabi::load<u32>(a+0x3D4);model=gabi::load<u32>(morph+0x90);
            u32 block=gabi::load<u32>(model+0x2C);u16 flags=gabi::load<u16>(block+4);
            u32 matrices=gabi::load<u32>(block+0x10);gabi::store<u16>(block+4,flags|0x10);
            matrix=gabi::load<u32>(0x1018C7B0);gabi::call(0x028E90D4,gabi::at<void>(matrices+(i+2)*0x30),gabi::at<void>(matrix));
            gabi::call(0x0200FCD8,local.get(),transformed.get());u32 sphere=a+0x1804+i*0x12C;
            gabi::call(0x02018D40,gabi::at<void>(sphere+0x118),transformed.get());
            f32 radius=gabi::fadds_ppc(gabi::load<f32>(t+4+0x3C),fifty);
            gabi::call(0x02018C8C,gabi::at<void>(sphere+0x118),gabi::fadds_ppc(radius,extraRadius));
            void* play=gabi::call<void*>(0x025200D4);
            gabi::call(0x0200E240,gabi::at<void>(gabi::ea(play)+0x26A4),gabi::at<void>(sphere));
        }
        morph=gabi::load<u32>(a+0x3D4);model=gabi::load<u32>(morph+0x90);
        u32 block=gabi::load<u32>(model+0x2C);u16 flags=gabi::load<u16>(block+4);
        u32 matrices=gabi::load<u32>(block+0x10);gabi::store<u16>(block+4,flags|0x10);
        matrix=gabi::load<u32>(0x1018C7B0);gabi::call(0x028E90D4,gabi::at<void>(matrices+0x330),gabi::at<void>(matrix));
        f32 sixtyFive=gabi::load<f32>(0x1000B8DC);
        for(u32 i=0;i<4;++i) {
            u8 part=gabi::load<u8>(a+0x3D0);f32 localX=gabi::load<f32>(0x10192934+i*4);
            f32 localY=gabi::load<f32>(0x10192944+i*4),localZ=gabi::load<f32>(0x10192954+i*4);
            local->x=part==2?-localX:localX;local->y=localY;local->z=localZ;
            gabi::call(0x0200FCD8,local.get(),transformed.get());u32 sphere=a+0x2998+i*0x12C;
            gabi::call(0x02018D40,gabi::at<void>(sphere+0x118),transformed.get());
            f32 radius=gabi::fadds_ppc(gabi::load<f32>(t+4+0x40),sixtyFive);
            gabi::call(0x02018C8C,gabi::at<void>(sphere+0x118),gabi::fadds_ppc(radius,extraRadius));
            void* play=gabi::call<void*>(0x025200D4);
            gabi::call(0x0200E240,gabi::at<void>(gabi::ea(play)+0x26A4),gabi::at<void>(sphere));
        }
        u8 part=gabi::load<u8>(a+0x3D0);f32 tx=gabi::fadds_ppc(gabi::load<f32>(t+4+0x40),hundred);
        f32 tz=gabi::fadds_ppc(gabi::load<f32>(t+4+0x48),gabi::load<f32>(0x1000B88C));
        local->x=part==2?-tx:tx;local->y=gabi::load<f32>(t+4+0x44);local->z=part==2?-tz:tz;
        gabi::call(0x0200FCD8,local.get(),transformed.get());
        tx=gabi::fadds_ppc(gabi::load<f32>(t+4+0x40),hundred);f32 ty=gabi::load<f32>(t+4+0x44);
        tz=gabi::fadds_ppc(gabi::load<f32>(t+4+0x4C),gabi::load<f32>(0x1000B8BC));
        local->x=part==2?-tx:tx;local->y=ty;local->z=part==2?tz:-tz;
        gabi::call(0x0200FCD8,local.get(),gabi::at<void>(a+0x37C));
        gabi::call(0x020182E0,gabi::at<void>(a+0x17EC),transformed.get());
        f32 height=gabi::load<f32>(t+4+0x24),oneForty=gabi::load<f32>(0x1000B8E0);
        gabi::call(0x02018428,gabi::at<void>(a+0x17EC),gabi::fadds_ppc(height,oneForty));
        f32 radius=gabi::load<f32>(t+4+0x28),oneFifty=gabi::load<f32>(0x1000B898);
        gabi::call(0x020184DC,gabi::at<void>(a+0x17EC),gabi::fadds_ppc(radius,oneFifty));
        void* play=gabi::call<void*>(0x025200D4);gabi::call(0x0200E240,gabi::at<void>(gabi::ea(play)+0x26A4),gabi::at<void>(a+0x16D4));
    }
    u32 eyeX=gabi::load<u32>(a+0x37C),eyeY=gabi::load<u32>(a+0x380);
    gabi::store<u32>(a+0x390,eyeX);u8 part=gabi::load<u8>(a+0x3D0);u32 eyeZ=gabi::load<u32>(a+0x384);
    gabi::store<u32>(a+0x394,eyeY);gabi::store<u32>(a+0x398,eyeZ);
    if(part==0) {
        gabi::call(0x020EDC8C,actor);gabi::call(0x020EE200,actor);
        s8 roomState=gabi::load<s8>(a+0x3108);
        if(roomState==0) gabi::call(0x0255FDA4,3,0,zero);
        else if((u32)roomState<=8) {
            s32 first=1,second=0;
            switch(roomState) {
            case 1:first=3;break;case 2:first=0;second=4;break;
            case 3:first=3;second=4;break;case 4:first=5;second=3;break;
            case 6:case 7:second=2;break;
            }
            f32 color=gabi::load<f32>(a+0x3104);
            gabi::call(0x0255FDA4,first,second,color);
            if(roomState==1 || roomState==2 || roomState==6 || roomState==8) {
                f32 step=half;
                if(roomState!=6) step=gabi::load<f32>(roomState==2?0x1000B8E8:0x1000B8E4);
                gabi::call(0x0200ED84,gabi::at<void>(a+0x3104),one,one,step);
                if(roomState==6 || roomState==8) {
                    color=gabi::load<f32>(a+0x3104);
                    if(!(color<one)) gabi::store<s8>(a+0x3108,roomState==6?7:1);
                }
            } else {
                f32 step=half;
                if(roomState!=7) step=gabi::load<f32>(roomState==3?0x1000B8EC:roomState==4?0x1000B864:0x1000B8E4);
                gabi::call(0x0200EDC8,gabi::at<void>(a+0x3104),one,step);
            }
        }
        u32 messageState=gabi::load<u32>(0x1046298C);
        u32 message=gabi::load<u32>(0x101F4B5C);
        if(messageState!=0xFFFFFFFF) {
            s32 state=gabi::call<s32>(0x025F795C,gabi::at<void>(message));
            if(state==14) {
                if(gabi::load<s8>(0x10462998)) {
                    gabi::call(0x025F74D0,gabi::at<void>(message),16);
                    gabi::store<u8>(message+0x921,1);
                }
            } else if(gabi::call<s32>(0x025F795C,gabi::at<void>(message))==18) {
                gabi::call(0x025F74D0,gabi::at<void>(message),19);
                gabi::store<u32>(0x1046298C,0xFFFFFFFF);
            }
        }
        bool soundStarted=false;
        for(u32 i=0;i<2;++i) {
            gabi::Local<be<u32>> id;*id=gabi::load<u32>(a+0x30C4+i*4);u32 attached=0;
            if((u32)*id!=0xFFFFFFFF) attached=gabi::call<u32>(0x025D5218,gabi::at<void>(0x025E1234),id.get());
            if(!attached) continue;
            morph=gabi::load<u32>(a+0x3D4);model=gabi::load<u32>(morph+0x90);
            u32 block=gabi::load<u32>(model+0x2C);u16 flags=gabi::load<u16>(block+4);
            u32 matrices=gabi::load<u32>(block+0x10);gabi::store<u16>(block+4,flags|0x10);
            matrix=gabi::load<u32>(0x1018C7B0);gabi::call(0x028E90D4,gabi::at<void>(matrices+(i+4)*0x30),gabi::at<void>(matrix));
            gabi::Local<cXyz> origin;origin->x=zero;origin->y=zero;origin->z=zero;
            gabi::call(0x0200FCD8,origin.get(),gabi::at<void>(attached+0x314));
            gabi::store<u8>(attached+0x3A0,2);u8 health=gabi::load<u8>(a+0x30A6+i);gabi::store<u8>(attached+0x3A1,health);
            bool active=false;
            if(gabi::load<s16>(a+0x30A8+i*2)==0 && gabi::load<s8>(a+0x30A6+i)>0) {
                s16 action=gabi::load<s16>(a+0x130A);active=action!=7 && action!=22 && action!=0;
            }
            if(!active) gabi::store<u32>(attached+0x39C,0);
            else {
                gabi::store<u32>(attached+0x39C,4);gabi::store<u8>(attached+0x38A,4);
                if(!soundStarted) {
                    soundStarted=true;
                    if(a+0x37C) {
                        s8 room=gabi::load<s8>(a+0x326);s8 reverb=gabi::call<s8>(0x02520540,room);
                        gabi::call(0x025E1A40,0x7038,gabi::at<void>(a+0x37C),0,reverb);
                    }
                }
            }
        }
    }
    // 020E7134 returns TRUE in the outer execute function.
}
#include "d/actor/d_a_bst.h"

static void bstExecuteHeadControl(bst_class*,f32,f32);
static void bstExecuteTransformAndCollision(bst_class*,f32,f32,f32);

static BOOL daBst_Execute(bst_class* actor) {
    WWHD_FUNC(0x020E54A0,BOOL,actor);
    u32 a=gabi::ea(actor);
    void* play=gabi::call<void*>(0x025200D4);
    f32 zero=gabi::load<f32>(0x1000B854);
    s16 frame=gabi::load<s16>(a+0x1308);
    u32 player=gabi::load<u32>(gabi::ea(play)+0x5B2C);
    gabi::Local<cXyz> origin;origin->x=zero;origin->y=zero;origin->z=zero;
    gabi::store<s16>(a+0x1308,(s16)(frame+1));
    for(u32 i=0;i<5;++i) {
        s16 value=gabi::load<s16>(a+0x1330+i*2);
        if(value) gabi::store<s16>(a+0x1330+i*2,(s16)(value-1));
    }
    s16 invulnerability=gabi::load<s16>(a+0x133C);
    s16 damage=gabi::load<s16>(a+0x133A);
    if(invulnerability) gabi::store<s16>(a+0x133C,(s16)(invulnerability-1));
    s16 firstHurt=gabi::load<s16>(a+0x135C);
    if(damage) gabi::store<s16>(a+0x133A,(s16)(damage-1));
    s16 secondHurt=gabi::load<s16>(a+0x135E);
    if(firstHurt) gabi::store<s16>(a+0x135C,(s16)(firstHurt-1));
    s16 thirdHurt=gabi::load<s16>(a+0x1360);
    if(secondHurt) gabi::store<s16>(a+0x135E,(s16)(secondHurt-1));
    s16 stateTimer=gabi::load<s16>(a+0x133E);
    if(thirdHurt) gabi::store<s16>(a+0x1360,(s16)(thirdHurt-1));
    if(stateTimer) gabi::store<s16>(a+0x133E,(s16)(stateTimer-1));
    u8 paused=gabi::load<u8>(0x10462981);
    f32 half=gabi::load<f32>(0x1000B86C),one=gabi::load<f32>(0x1000B860);
    if(!paused) {
        gabi::store<u32>(a+0x39C,4);
        if(gabi::load<u8>(a+0x3D0)==0) {
            if(gabi::load<s8>(a+0x3330)) {
                void* camera=gabi::call<void*>(0x024F8044);
                gabi::call(0x02514EE4,camera,gabi::at<void>(0x1000B8F0),nullptr);
            }
            bstExecuteHeadControl(actor,half,one);
            // 020E5FCC: drop head attention and retire charge effects when appropriate.
            if(gabi::load<s16>(a+0x130A)!=7) {
                gabi::store<u32>(a+0x39C,0);
            }
            if(gabi::load<s16>(a+0x130A)!=14) {
                for(u32 i=0;i<2;++i) {
                    u32 emitter=gabi::load<u32>(a+0x310C+i*4);
                    if(emitter) {
                        u32 flags=gabi::load<u32>(emitter+0x254);
                        gabi::store<u32>(emitter+0x5C,0xFFFFFFFF);
                        gabi::store<u32>(emitter+0x254,flags|1);
                        gabi::store<u32>(a+0x310C+i*4,0);
                    }
                }
                if(gabi::load<s8>(a+0x3108)==7) gabi::store<s8>(a+0x3108,8);
            }
            f32 playerHeight=gabi::load<f32>(player+0x318),limit=gabi::load<f32>(0x1000B8A0);
            u32 controller=gabi::load<u32>(a+0x3324);
            if(!(playerHeight>limit)) {
                gabi::store<f32>(controller,one);
                controller=gabi::load<u32>(a+0x3324);gabi::store<u8>(controller+0xE,2);
            } else gabi::store<u8>(controller+0xE,0);
            for(u32 offset:{0x332Cu,0x3324u,0x3320u}) {
                u32 animation=gabi::load<u32>(a+offset);gabi::call(0x025E742C,gabi::at<void>(animation));
            }
            gabi::call(0x028E93CC,gabi::at<void>(0x1048D0CC),zero,zero,zero);
            // Matrix copies retain all twelve loaded values before their writes.
            for(u32 offset:{0x331Cu,0x3328u}) {
                f32 matrix[12];for(u32 i=0;i<12;++i) matrix[i]=gabi::load<f32>(0x1048D0CC+i*4);
                u32 model=gabi::load<u32>(a+offset);
                for(u32 i=0;i<12;++i) gabi::store<f32>(model+0xC8+i*4,matrix[i]);
            }
        }
        gabi::call(0x020EB4BC,actor);
        s16 action=gabi::load<s16>(a+0x130A);
        if(action!=0 && action!=22) {
            play=gabi::call<void*>(0x025200D4);
            gabi::call(0x024F08A8,gabi::at<void>(a+0x13A4),gabi::at<void>(gabi::ea(play)+0x12A0));
            gabi::call(0x024F12A8,gabi::at<void>(a+0x13A4),zero);
        }
        u32 morph=gabi::load<u32>(a+0x3D4);
        gabi::call(0x025E535C,gabi::at<void>(morph),gabi::at<void>(a+0x37C),0,0);
        if(gabi::load<s8>(a+0x3E0)) {
            gabi::call(0x025E742C,gabi::at<void>(gabi::load<u32>(a+0x3DC)));
            gabi::call(0x025E742C,gabi::at<void>(gabi::load<u32>(a+0x3D8)));
        }
        if(gabi::load<s8>(a+0x3F0)) {
            gabi::call(0x025E742C,gabi::at<void>(gabi::load<u32>(a+0x3EC)));
            gabi::call(0x025E742C,gabi::at<void>(gabi::load<u32>(a+0x3E8)));
        }
    }
    bstExecuteTransformAndCollision(actor,zero,half,one);
    return TRUE;
}
VERIFY(0x020E54A0,daBst_Execute);
