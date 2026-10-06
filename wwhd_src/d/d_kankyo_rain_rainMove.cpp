#include "d_kankyo_rain_private.h"
namespace d_kankyo_rain_cpp {
static void rainMove() {
    WWHD_FUNC(0x02566B88,void);
    u32 packet=ld(environment()+0xA44),camera=ld(gameInfo()+0x5AF8);gameInfo();
    gabi::Local<Vec_l> wind,center,direction,position,forward1,forward2;
    gabi::call(0x0257DB28,wind.get());
    auto stageEquals=[&](u32 literal) {
        gabi::Local<SafeString_l> name,current;
        name->text=literal;name->vtable=0x1004F3AC;
        current->text=gameInfo()+0x5134;current->vtable=0x1004F3AC;
        safe_string_callback(name.get());safe_string_callback(name.get());
        u32 left=name->text;safe_string_callback(current.get());u32 right=current->text;
        return left==right||equal_terminated_strings(left,right);
    };
    u32 stageType=0;
    if(!stageEquals(0x1004F700)) {
        u32 stage=gameInfo()+0x5150;
        stage=gabi::call_ptr<u32>(ld(ld(stage)+0x15C),stage);
        stageType=(ld(stage+12)>>16)&7;
    }
    s32 count=gabi::load<s16>(packet+0x3758);
    if(count>(s32)ld(environment()+0xA40)) {if(gabi::load<s16>(packet+0x3758)==0)return;}
    else {s16 wanted=gabi::load<s16>(environment()+0xA42);gabi::store<s16>(packet+0x3758,wanted);if(wanted==0)return;}
    gabi::call(0x02563F64,camera+0xE8,packet+0x376C,direction.get());
    auto distance=[&](u32 a,u32 b) {gabi::call(0x028E8DE8,a,b);return gabi::call<f32>(0x028F4384);};
    f32 eyeChange=distance(packet+0x3760,camera+0xDC),zero=lf(0x1004F528),one=lf(0x1004F550),step;
    const f32 ten=lf(0x1004F6B8),fifty=lf(0x1004F564);
    if(eyeChange>ten)eyeChange=gabi::fsubs_ppc(eyeChange,ten)/fifty;else eyeChange=zero;
    if(eyeChange>one) {
        for(u32 off=0;off<12;off+=4)gabi::store<u32>(packet+0x3760+off,ld(camera+0xDC+off));
        eyeChange=one;
        f32 centerChange=distance(packet+0x376C,camera+0xE8);
        if(centerChange>ten) {
            centerChange=gabi::fsubs_ppc(centerChange,ten)/fifty;
            if(centerChange>one)centerChange=one;
        }else centerChange=zero;
        step=lf(0x1004F580);
        gabi::call(0x0200ECD4,packet+0x3784,gabi::fmuls_ppc(eyeChange,centerChange),lf(0x1004F554),lf(0x1004F588),step);
    } else {
        for(u32 off=0;off<12;off+=4)gabi::store<u32>(packet+0x3760+off,ld(camera+0xDC+off));
        f32 centerChange=distance(packet+0x376C,camera+0xE8);
        if(centerChange>ten) {centerChange=gabi::fsubs_ppc(centerChange,ten)/fifty;if(centerChange>one)centerChange=one;}else centerChange=zero;
        step=lf(0x1004F580);
        gabi::call(0x0200ECD4,packet+0x3784,gabi::fmuls_ppc(eyeChange,centerChange),lf(0x1004F554),lf(0x1004F588),step);
    }
    const f32 small=lf(0x1004F588);
    if(lf(packet+0x3784)>lf(0x1004F584))sf(packet+0x3784,lf(0x1004F584));
    gabi::call(0x0200ECD4,packet+0x3778,(f32)direction->x,lf(0x1004F554),small,step);
    gabi::call(0x0200ECD4,packet+0x377C,(f32)direction->y,lf(0x1004F554),small,step);
    gabi::call(0x0200ECD4,packet+0x3780,(f32)direction->z,lf(0x1004F554),small,step);
    for(u32 off=0;off<12;off+=4)gabi::store<u32>(packet+0x376C+off,ld(camera+0xE8+off));
    gabi::call(0x0256401C,camera,center.get(),lf(0x1004F6BC),lf(0x1004F5D0));
    gabi::call<u32>(0x0257DAA8);gabi::call<f32>(0x02578348);
    sf(packet+0x3754,zero);sf(packet+0x3750,zero);gabi::store<u8>(packet+0x379C,0);
    u32 overhead=0,front=0,farFront=0,radial=0;f32 radialLimit=0;
    bool special=stageType==2&&!stageEquals(0x1004F708)&&!stageEquals(0x1004F6F8);
    if(!special) {
        u32 checkCamera=ld(gameInfo()+0x5AF8);gameInfo();
        gabi::Local<GroundCheck_l> ground;gabi::Local<RoofCheck_l> roof;gabi::Local<Vec_l> cameraPoint;
        u32 g=gabi::ea(ground.get()),r=gabi::ea(roof.get());
        gabi::call(0x02008E0C,ground.get());
        gabi::store<u8>(g+0x45,0);gabi::store<u32>(g+0x4C,0x1004F494);
        gabi::store<u8>(g+0x46,0);gabi::store<u32>(g,g+0x40);gabi::store<u8>(g+0x47,0);
        gabi::store<u8>(g+0x48,0);gabi::store<u8>(g+0x49,0);gabi::store<u32>(g+4,g+0x4C);
        gabi::store<u32>(g+0x40,0x1004F4A4);gabi::store<u8>(g+0x4A,0);gabi::store<u32>(g+0x50,15);
        gabi::store<u32>(g+0x20,0x1004F484);gabi::store<u32>(g+0x10,0x1004F474);gabi::store<u8>(g+0x44,1);
        gabi::call(0x024EE7AC,roof.get(),1);
        cameraPoint->x=lf(checkCamera+0xDC);cameraPoint->z=lf(checkCamera+0xE4);
        f32 y=gabi::fadds_ppc(lf(checkCamera+0xE0),fifty);
        sf(r+0x38,(f32)cameraPoint->x);sf(r+0x3C,y);sf(r+0x40,(f32)cameraPoint->z);
        overhead=gabi::call<f32>(0x024EF6E8,gameInfo()+0x12A0,roof.get())!=lf(0x1004F630);
        sf(g+0x24,(f32)cameraPoint->x);sf(g+0x28,gabi::fadds_ppc(y,lf(0x1004F594)));sf(g+0x2C,(f32)cameraPoint->z);
        if(gabi::call<f32>(0x02008974,gameInfo()+0x12A0,ground.get())>gabi::fadds_ppc(lf(checkCamera+0xE0),fifty))overhead=1;
        gabi::store<u32>(r+0x30,0x1004F3E4);gabi::store<u32>(r+0x20,0x1004F4B4);gabi::store<u32>(r+0x24,0x1004F4D4);
        gabi::call(0x02008B4C,r+0x10,0);gabi::store<u32>(g+0x40,0x1004F424);gabi::store<u32>(g+0x4C,0x1004F3E4);gabi::store<u32>(g+0x20,0x1004F404);
        gabi::call(0x02008DAC,ground.get(),0);
        front=gabi::call<u32>(0x02566974,forward1.get(),lf(0x1004F6BC));
        farFront=gabi::call<u32>(0x02566974,forward2.get(),lf(0x1004F6C0));
        if(overhead)gabi::store<u8>(packet+0x379C,gabi::load<u8>(packet+0x379C)|1);
        else if(front)gabi::store<u8>(packet+0x379C,gabi::load<u8>(packet+0x379C)|2);
        if(stageEquals(0x1004F708)) {gabi::store<u8>(packet+0x379C,0);overhead=front=farFront=0;}
    }else {
        gabi::store<u8>(packet+0x379C,gabi::load<u8>(packet+0x379C)|1);radial=1;
        if(stageEquals(0x1004F710))radialLimit=lf(0x1004F6C4);
        else if(stageEquals(0x1004F718))radialLimit=lf(0x1004F6C8);
        else radialLimit=lf(0x1004F6CC);
        if(stageEquals(0x1004F6F0))radial=2;
    }
    auto fade=[&](u32 off,u32 blocked) {
        if(blocked)gabi::call(0x0200ECD4,packet+off,zero,lf(0x1004F57C),lf(0x1004F554),step);
        else gabi::call(0x0200ECD4,packet+off,one,small,small,lf(0x1004F58C));
    };
    fade(0x3790,overhead);fade(0x3794,front);fade(0x3798,farFront);
    count=gabi::load<s16>(packet+0x3758);if(count<=0)return;
    const f32 scatter=lf(0x1004F6D0),fallBase=lf(0x1004F6E0),bottom=lf(0x1004F52C),accel=lf(0x1004F6D4),jitter=lf(0x1004F6DC),vertical=lf(0x1004F6E4);
    auto sumPosition=[&](u32 p) {position->x=gabi::fadds_ppc(lf(p+0x10),lf(p+4));position->y=gabi::fadds_ppc(lf(p+0x14),lf(p+8));position->z=gabi::fadds_ppc(lf(p+0x18),lf(p+12));};
    auto copyCenter=[&](u32 p) {gabi::store<u32>(p+0x10,ld(gabi::ea(center.get())));gabi::store<u32>(p+0x14,ld(gabi::ea(center.get())+4));gabi::store<u32>(p+0x18,ld(gabi::ea(center.get())+8));};
    for(s32 i=count-1;i>=0;--i) {
        u32 p=packet+0xA0+(u32)i*56;u8 state=gabi::load<u8>(p);sf(p+0x14,(f32)center->y);
        if(state==0) {
            sf(p+0x24,-gabi::fadds_ppc(gabi::call<f32>(0x020198D8,ten),fallBase));gabi::store<u16>(p+0x34,0);copyCenter(p);
            sf(p+4,gabi::call<f32>(0x02019918,scatter));sf(p+8,gabi::call<f32>(0x020198D8,lf(0x1004F5D0)));sf(p+12,gabi::call<f32>(0x02019918,scatter));sf(p+0x28,one);
            sf(p+0x1C,gabi::call<f32>(0x020198D8,lf(0x1004F6D8)));sf(p+0x20,gabi::call<f32>(0x020198D8,lf(0x1004F6D8)));
            gabi::call(0x0256691C,packet,i);gabi::store<u8>(p,gabi::load<u8>(p)+1);sumPosition(p);
        }else if(state<=3) {
            f32 random=gabi::call<f32>(0x02019918,small);
            gabi::call(0x0200ECD4,p+0x24,gabi::fsubs_ppc(lf(p+0x24),random),lf(0x1004F57C),small,step);
            f32 a=gabi::fmuls_ppc(lf(packet+0x3778),lf(packet+0x3784));a=gabi::fmadds(a,ten,(f32)wind->x);a=gabi::fmadds((f32)(i&7),jitter,a);sf(p+4,gabi::fmadds(a,accel,lf(p+4)));
            a=gabi::fmuls_ppc(lf(packet+0x377C),lf(packet+0x3784));a=gabi::fmadds(a,ten,(f32)wind->y);a=gabi::fadds_ppc(a,vertical);sf(p+8,gabi::fmadds(a,accel,lf(p+8)));
            a=gabi::fmuls_ppc(lf(packet+0x3780),lf(packet+0x3784));a=gabi::fmadds(a,ten,(f32)wind->z);a=gabi::fmadds((f32)(i&3),jitter,a);sf(p+12,gabi::fmadds(a,accel,lf(p+12)));
            position->x=gabi::fadds_ppc(lf(p+0x10),lf(p+4));position->y=center->y;position->z=gabi::fadds_ppc(lf(p+0x18),lf(p+12));
            f32 dist=distance(gabi::ea(position.get()),gabi::ea(center.get()));s16 timer=gabi::load<s16>(p+0x34);
            if(timer!=0)gabi::store<u16>(p+0x34,(u16)(timer-1));
            else {
                if(dist>scatter) {
                    gabi::store<u16>(p+0x34,10);sf(p+0x24,-gabi::fadds_ppc(gabi::call<f32>(0x020198D8,ten),fallBase));copyCenter(p);
                    if(distance(gabi::ea(position.get()),gabi::ea(center.get()))>lf(0x1004F6E8)) {
                        sf(p+4,gabi::call<f32>(0x02019918,scatter));sf(p+8,gabi::call<f32>(0x02019918,scatter));sf(p+12,gabi::call<f32>(0x02019918,scatter));
                    }else {
                        f32 radius=gabi::fadds_ppc(gabi::call<f32>(0x02019918,lf(0x1004F6EC)),scatter);
                        gabi::call(0x02563F0C,position.get(),center.get(),direction.get());
                        sf(p+4,gabi::fmuls_ppc((f32)direction->x,radius));sf(p+8,gabi::fmuls_ppc((f32)direction->y,radius));sf(p+12,gabi::fmuls_ppc((f32)direction->z,radius));
                    }
                    gabi::store<u8>(p,1);gabi::call(0x0256691C,packet,i);
                }
                position->y=gabi::fadds_ppc(lf(p+0x14),lf(p+8));
                if((f32)position->y<gabi::fadds_ppc(lf(p+0x30),accel)) {
                    copyCenter(p);sf(p+4,gabi::call<f32>(0x02019918,scatter));sf(p+8,bottom);sf(p+12,gabi::call<f32>(0x02019918,scatter));gabi::call(0x0256691C,packet,i);gabi::store<u16>(p+0x34,10);
                }
            }
            sumPosition(p);
        }else sumPosition(p);
        if(i>(s32)(ld(environment()+0xA40)-1)&&(u32)i==(u32)(gabi::load<s16>(packet+0x3758)-1))gabi::store<u16>(packet+0x3758,(u16)(gabi::load<s16>(packet+0x3758)-1));
        f32 alpha=one;
        if(overhead||lf(packet+0x3790)<one) {
            gabi::Local<Vec_l> horizontal;horizontal->x=position->x;horizontal->y=lf(camera+0xE0);horizontal->z=position->z;
            if(distance(camera+0xDC,gabi::ea(horizontal.get()))<scatter)alpha=lf(packet+0x3790);
        }
        if(front||lf(packet+0x3794)<one) {
            gabi::Local<Vec_l> horizontal;horizontal->x=position->x;horizontal->y=forward1->y;horizontal->z=position->z;
            if(distance(gabi::ea(forward1.get()),gabi::ea(horizontal.get()))<lf(0x1004F5FC))alpha=gabi::fmuls_ppc(alpha,lf(packet+0x3794));
        }
        if(farFront||lf(packet+0x3798)<one) {
            gabi::Local<Vec_l> horizontal;horizontal->x=position->x;horizontal->y=forward2->y;horizontal->z=position->z;
            if(distance(gabi::ea(forward2.get()),gabi::ea(horizontal.get()))<lf(0x1004F5FC))alpha=gabi::fmuls_ppc(alpha,lf(packet+0x3798));
        }
        if(radial) {
            gabi::Local<Vec_l> horizontal;horizontal->x=position->x;horizontal->y=zero;horizontal->z=position->z;
            gabi::call(0x028E8DD0,horizontal.get());f32 d=gabi::fsubs_ppc(gabi::call<f32>(0x028F4384),radialLimit);alpha=d>=0?alpha:zero;
            if(radial==2){d=gabi::fsubs_ppc((f32)position->y,lf(0x1004F6C8));alpha=d>=0?alpha:zero;}
        }
        sf(p+0x28,alpha);
    }
}
VERIFY(0x02566B88,rainMove);
}
