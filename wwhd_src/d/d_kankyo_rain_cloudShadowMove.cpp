#include "d_kankyo_rain_private.h"
namespace d_kankyo_rain_cpp {
static void cloudShadowMove() {
    WWHD_FUNC(0x0256BB6C,void);
    u32 packet=ld(environment()+0xA84),camera=ld(gameInfo()+0x5AF8);
    gabi::Local<Vec_l> wind,center,position,pointWind,outputWind;
    gabi::Local<Matrix_l> inverse;
    gabi::call(0x0257DB28,wind.get());gameInfo();f32 power=gabi::call<f32>(0x02578348);
    if(ld(gameInfo()+0x5FA4)==0)return;
    gabi::call(0x028E91EC,ld(gameInfo()+0x5FA4)+0x1E4,inverse.get());
    if((s32)ld(packet+0x9C)>(s32)ld(environment()+0xA80)) {if(ld(packet+0x9C)==0)return;}
    else {s16 wanted=gabi::load<s16>(environment()+0xA82);gabi::store<u32>(packet+0x9C,(s32)wanted);if(wanted==0)return;}
    f32 depth=lf(0x1004F8C0);gabi::call(0x0256401C,camera,center.get(),depth,depth);
    u16 globalPhase=gabi::load<u16>(packet+0x988);
    f32 decay=gabi::fmadds(lf(0x1004F56C),power,lf(0x1004F56C));
    f32 counter=gabi::fsubs_ppc(lf(packet+0x98C),decay),zero=lf(0x1004F528);
    f32 globalSine=lf(0x104A44F8+(globalPhase>>3)*8);
    gabi::store<u16>(packet+0x988,globalPhase+0x104);
    if(counter<zero)counter=lf(0x1004F8C4);sf(packet+0x98C,counter);
    if((s32)ld(packet+0x9C)<=0)return;
    f32 countStep=gabi::fmadds(lf(0x1004F538),power,lf(0x1004F8D8));
    u16 phaseStep=(u16)gabi::ftoi(countStep);
    const f32 small=lf(0x1004F588),blend=lf(0x1004F554),wrapMul=lf(0x1004F8D4),pointMin=lf(0x1004F724),one=lf(0x1004F550),half=lf(0x1004F57C),radius=lf(0x1004F8C8);
    f32 alphaScale=gabi::fmadds(gabi::fadds_ppc(globalSine,one),lf(0x1004F8CC),lf(0x1004F8D0));
    const f32 jitter=lf(0x1004F590),initRadius=lf(0x1004F598),phaseMax=lf(0x1004F6D8),innerRadius=lf(0x1004F8DC),pointThreshold=lf(0x1004F728),pointGain=lf(0x1004F72C),scatter=lf(0x1004F544);
    auto stageEquals=[&](u32 literal,bool ignore=false) {
        gabi::Local<SafeString_l> name,current;
        name->text=literal;name->vtable=0x1004F3AC;current->text=gameInfo()+0x5134;current->vtable=0x1004F3AC;
        safe_string_callback(name.get());safe_string_callback(name.get());u32 left=name->text;safe_string_callback(current.get());u32 right=current->text;
        bool equal=left==right||equal_terminated_strings(left,right);(void)ignore;return equal;
    };
    auto sumPosition=[&](u32 p){position->x=gabi::fadds_ppc(lf(p+0x10),lf(p+4));position->y=gabi::fadds_ppc(lf(p+0x14),lf(p+8));position->z=gabi::fadds_ppc(lf(p+0x18),lf(p+12));};
    auto copyCenter=[&](u32 p){for(u32 off=0;off<12;off+=4)gabi::store<u32>(p+0x10+off,ld(gabi::ea(center.get())+off));};
    auto distance=[&](u32 a,u32 b){gabi::call(0x028E8DE8,a,b);return gabi::call<f32>(0x028F4384);};
    for(s32 i=0;i<(s32)ld(packet+0x9C);++i) {
        u32 p=packet+0xA0+(u32)i*76;u8 state=gabi::load<u8>(p);
        if(state==0) {
            f32 size=gabi::fadds_ppc(gabi::call<f32>(0x020198D8,initRadius),lf(0x1004F8E0));sf(p+0x48,size);sf(p+0x44,size);copyCenter(p);
            sf(p+4,gabi::call<f32>(0x02019918,radius));sf(p+8,gabi::call<f32>(0x02019918,radius));sf(p+12,gabi::call<f32>(0x02019918,radius));
            f32 speed=gabi::fadds_ppc(gabi::call<f32>(0x020198D8,lf(0x1004F584)),lf(0x1004F8E4));sf(p+0x38,speed);sf(p+0x40,zero);
            gabi::store<u16>(p+0x3C,(u16)gabi::ftoi(gabi::call<f32>(0x020198D8,lf(0x1004F59C))));
            sf(p+0x28,gabi::call<f32>(0x020198D8,phaseMax));sf(p+0x2C,gabi::call<f32>(0x020198D8,phaseMax));sf(p+0x30,gabi::call<f32>(0x020198D8,phaseMax));
            gabi::store<u8>(p,gabi::load<u8>(p)+1);sf(p+0x1C,zero);sf(p+0x20,zero);sf(p+0x24,zero);
        }
        if(state<=2) {
            sumPosition(p);gabi::store<u16>(p+0x3C,gabi::load<u16>(p+0x3C)+phaseStep);
            gabi::call(0x0200F5C8,p+0x34,lf(p+0x38),lf(0x1004F808));
            u8 mode=gabi::load<u8>(environment()+0xA7D);
            if(mode&1) {
                bool ordinary=!stageEquals(0x1004F900)||gabi::load<u8>(environment()+0x109E)!=0;
                if(ordinary) {
                    if(gabi::load<u8>(environment()+0xA7D)==1)sf(p+8,gabi::fmadds(lf(0x1004F730),lf(p+0x38),lf(p+8)));
                }else sf(p+8,gabi::fadds_ppc(lf(p+8),lf(p+0x38)));
            }
            f32 speed=gabi::load<u8>(environment()+0xA7D)==1?lf(0x1004F5B4):lf(0x1004F8E8);
            f32 windSpeed=lf(p+0x38);
            sf(p+4,gabi::fmadds(gabi::fmuls_ppc((f32)wind->x,speed),windSpeed,lf(p+4)));
            sf(p+8,gabi::fmadds(gabi::fmuls_ppc((f32)wind->y,speed),windSpeed,lf(p+8)));
            sf(p+12,gabi::fmadds(gabi::fmuls_ppc((f32)wind->z,speed),windSpeed,lf(p+12)));
            gabi::call(0x0257E128,outputWind.get(),position.get());
            pointWind->x=outputWind->x;pointWind->y=outputWind->y;pointWind->z=outputWind->z;
            for(u32 off=0;off<12;off+=4)if(lf(p+0x1C+off)<pointThreshold)sf(p+0x1C+off,gabi::fmadds(lf(gabi::ea(pointWind.get())+off),pointGain,lf(p+0x1C+off)));
            for(u32 off=0;off<12;off+=4)gabi::call(0x0200ECD4,p+0x1C+off,zero,blend,small,pointMin);
            for(u32 off=0;off<12;off+=4) {f32 factor=gabi::fadds_ppc(gabi::call<f32>(0x02019918,jitter),one);sf(p+4+off,gabi::fmadds(lf(p+0x1C+off),factor,lf(p+4+off)));}
            stageEquals(0x1004F910,true);
            bool special=stageEquals(0x1004F908);
            f32 wanderSpeed=gabi::fmuls_ppc(speed,special?initRadius:lf(0x1004F584));
            for(u32 off=0;off<12;off+=4)sf(p+4+off,gabi::fmadds(gabi::fsubs_ppc(lf(p+0x28+off)/phaseMax,half),wanderSpeed,lf(p+4+off)));
            copyCenter(p);sumPosition(p);
            if(distance(gabi::ea(position.get()),gabi::ea(center.get()))>lf(0x1004F8EC)) {
                copyCenter(p);gabi::call(0x02563F0C,position.get(),center.get(),pointWind.get());
                for(u32 off=0;off<12;off+=4){f32 random=gabi::call<f32>(0x02019918,scatter);sf(gabi::ea(pointWind.get())+off,gabi::fadds_ppc(lf(gabi::ea(pointWind.get())+off),random));}
                for(u32 off=0;off<12;off+=4)sf(p+4+off,gabi::fmuls_ppc(gabi::fmuls_ppc(lf(gabi::ea(pointWind.get())+off),radius),wrapMul));
                sf(p+0x40,zero);
            }
            sumPosition(p);
        }else {if(state==3)gabi::store<u8>(p,0);sumPosition(p);}
        f32 viewFactor=lf(ld(gameInfo()+0x5FA4)+0xD4)/lf(0x1004F7F8);if(viewFactor>one)viewFactor=one;
        f32 cameraDistance=distance(gabi::ea(position.get()),camera+0xDC)/lf(0x1004F8F0);
        f32 sine=lf(0x104A44F8+(gabi::load<u16>(p+0x3C)>>3)*8),baseSize=lf(p+0x48);
        f32 animated=gabi::fmadds(gabi::fmuls_ppc(baseSize,blend),sine,baseSize);
        sf(p+0x44,gabi::fmuls_ppc(gabi::fmuls_ppc(animated,gabi::fmadds(cameraDistance,lf(0x1004F8F4),one)),viewFactor));
        f32 dist=distance(gabi::ea(position.get()),gabi::ea(center.get())),alpha=one;
        if(dist<radius) {if(dist>innerRadius)alpha=gabi::fsubs_ppc(one,gabi::fsubs_ppc(dist,innerRadius)/lf(0x1004F8F8));}
        else alpha=zero;
        alpha=gabi::fmuls_ppc(gabi::fmuls_ppc(gabi::fmuls_ppc(alpha,alpha),alpha),cameraDistance);
        alpha=gabi::fmuls_ppc(alpha,alphaScale);
        f32 limit=lf(0x1004F8FC);
        if(gabi::load<u8>(environment()+0xA7D)==3)limit=blend;
        else if(gabi::load<u8>(environment()+0xA7D)==4)limit=lf(0x1004F734);
        if(i>(s32)(ld(environment()+0xA80)-1)) {
            alpha=zero;
            if(lf(p+0x40)<lf(0x1004F58C)&&(u32)i==ld(packet+0x9C)-1)gabi::store<u32>(packet+0x9C,ld(packet+0x9C)-1);
        }
        sf(p+0x40,gabi::fmuls_ppc(alpha,limit));
    }
}
VERIFY(0x0256BB6C,cloudShadowMove);
}
