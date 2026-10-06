#include "d_kankyo_rain_private.h"
namespace d_kankyo_rain_cpp {
static void snowMove() {
    WWHD_FUNC(0x02568BD4,void);
    u32 packet=ld(environment()+0xA50),camera=ld(gameInfo()+0x5AF8);
    gameInfo();
    gabi::Local<Vec_l> wind,center,heading,position,wrapDirection;
    gabi::Local<DoublePos_l> delta;
    gabi::call(0x0257DB28,wind.get());
    u32 wanted=ld(environment()+0xA4C);
    s32 count=gabi::load<s16>(packet+0x3760);
    if(count>(s32)wanted) {if(!count)return;}
    else {count=gabi::load<s16>(environment()+0xA4E);gabi::store<u16>(packet+0x3760,(u16)count);if(!count)return;}
    f32 depth=lf(0x1004F7CC);
    gabi::call(0x0256401C,camera,center.get(),depth,depth);
    f32 squared=gabi::call<f32>(0x028E8DE8,packet+0x374C,camera+0xDC);
    if(gabi::call<f64>(0x028F4384,(f64)squared)>lf(0x1004F7D0))gabi::store<u16>(packet+0x3760,0);
    for(u32 axis=0;axis<3;axis++)gabi::store<u32>(packet+0x374C+axis*4,ld(camera+0xDC+axis*4));
    u32 windDirection=gabi::call<u32>(0x0257DAA8);
    f32 power=gabi::call<f32>(0x02578348);
    delta->x=(f64)gabi::fsubs_ppc(lf(camera+0xE8),lf(camera+0xDC));delta->y=ldbl(0x1004F530);delta->z=(f64)gabi::fsubs_ppc(lf(camera+0xF0),lf(camera+0xE4));
    gabi::call(0x02563E48,delta.get(),heading.get());
    f32 zero=lf(0x1004F528);
    f32 turn=gabi::call<f32>(0x02010CFC,zero,zero,-lf(windDirection),-lf(windDirection+8),(f32)heading->x,(f32)heading->z);
    sf(packet+0x375C,turn);
    f32 projection=gabi::fmadds(lf(windDirection),(f32)heading->x,gabi::fmuls_ppc(lf(windDirection+8),(f32)heading->z));
    f32 one=lf(0x1004F550);
    f32 strength=gabi::fmuls_ppc(gabi::fmuls_ppc(gabi::fmuls_ppc(gabi::fsubs_ppc(one,std::fabs(projection)),power),gabi::fsubs_ppc(one,std::fabs(lf(windDirection+4)))),std::fabs(turn));
    count=gabi::load<s16>(packet+0x3760);sf(packet+0x3758,strength);if(count-1<0)return;
    f32 approach=lf(0x1004F554),phaseStep=lf(0x1004F734),phaseRange=lf(0x1004F6D8),randomStep=lf(0x1004F588),half=lf(0x1004F57C),wrapJitter=lf(0x1004F7DC),fallRange=lf(0x1004F7D4),sizeBase=lf(0x1004F730),small=lf(0x1004F580),range=lf(0x1004F5FC),unit=lf(0x1004F5D0),fadeOffset=lf(0x1004F7D8),jitterScale=lf(0x1004F744);
    for(s32 i=count-1;i>=0;i--) {
        u32 particle=packet+0x9C+(u32)i*56;
        f32 fall=-gabi::fadds_ppc(gabi::call<f32>(0x020198D8,fallRange),jitterScale);
        f32 size=gabi::fadds_ppc((f32)(i&15),sizeBase);
        u8 state=gabi::load<u8>(particle);
        if(!state) {
            sf(particle+0x28,size);sf(particle+0x24,fall);gabi::store<u16>(particle+0x34,0);
            sf(particle+0x10,(f32)center->x);sf(particle+0x14,(f32)center->y);sf(particle+0x18,(f32)center->z);
            sf(particle+4,gabi::call<f32>(0x02019918,range));sf(particle+8,range);sf(particle+12,gabi::call<f32>(0x02019918,range));
            sf(particle+0x2C,zero);sf(particle+0x1C,gabi::call<f32>(0x020198D8,phaseRange));
            f32 phase=gabi::call<f32>(0x020198D8,phaseRange);
            gabi::store<u8>(particle,gabi::load<u8>(particle)+1);sf(particle+0x20,phase);
        } else if(state<=3) {
            f32 noise=gabi::call<f32>(0x02019918,randomStep);
            gabi::call(0x0200ECD4,particle+0x28,gabi::fsubs_ppc(lf(particle+0x28),noise),half,randomStep,small);
            if(gabi::load<u8>(particle)!=3||lf(particle+0x2C)<small) {
                f32 sizeNow=lf(particle+0x28);
                sf(particle+4,gabi::fmadds((f32)wind->x,sizeNow,lf(particle+4)));
                sf(particle+12,gabi::fmadds((f32)wind->z,sizeNow,lf(particle+12)));
                sf(particle+8,gabi::fadds_ppc(lf(particle+8),gabi::fmadds((f32)wind->y,sizeNow,lf(particle+0x24))));
                u32 angle=gabi::call<u32>(0x02019510,lf(particle+0x1C));
                sf(particle+4,gabi::fmadds(lf(0x104A44F8+((angle&0xFFFF)>>3)*8),jitterScale,lf(particle+4)));
                angle=gabi::call<u32>(0x02019510,lf(particle+0x20));
                f32 phase=gabi::fadds_ppc(lf(particle+0x1C),phaseStep),phaseZ=gabi::fadds_ppc(lf(particle+0x20),phaseStep);
                sf(particle+12,gabi::fmadds(lf(0x104A44F8+((angle&0xFFFF)>>3)*8),jitterScale,lf(particle+12)));
                sf(particle+0x1C,phase);sf(particle+0x20,phaseZ);
            }
            position->x=gabi::fadds_ppc(lf(particle+0x10),lf(particle+4));position->y=gabi::fadds_ppc(lf(particle+0x14),lf(particle+8));position->z=gabi::fadds_ppc(lf(particle+0x18),lf(particle+12));
            squared=gabi::call<f32>(0x028E8DE8,position.get(),center.get());
            f64 distance=gabi::call<f64>(0x028F4384,(f64)squared);
            s16 delay=gabi::load<s16>(particle+0x34);
            if(!delay&&distance>range) {
                sf(particle+0x28,size);sf(particle+0x24,fall);gabi::store<u16>(particle+0x34,10);
                sf(particle+0x10,(f32)center->x);sf(particle+0x14,(f32)center->y);sf(particle+0x18,(f32)center->z);
                squared=gabi::call<f32>(0x028E8DE8,position.get(),center.get());distance=gabi::call<f64>(0x028F4384,(f64)squared);
                if(distance>unit) {
                    sf(particle+4,gabi::call<f32>(0x02019918,range));sf(particle+8,gabi::call<f32>(0x02019918,range));
                    f32 z=gabi::call<f32>(0x02019918,range);gabi::store<u8>(particle,1);sf(particle+12,z);
                } else {
                    f32 noise=gabi::call<f32>(0x02019918,wrapJitter);
                    gabi::call(0x02563F0C,position.get(),center.get(),wrapDirection.get());
                    f32 radius=gabi::fadds_ppc(noise,range);
                    sf(particle+4,gabi::fmuls_ppc((f32)wrapDirection->x,radius));sf(particle+8,gabi::fmuls_ppc((f32)wrapDirection->y,radius));
                    f32 z=gabi::fmuls_ppc((f32)wrapDirection->z,radius);gabi::store<u8>(particle,1);sf(particle+12,z);
                }
            } else if(delay)gabi::store<u16>(particle+0x34,(u16)(delay-1));
        }
        position->x=gabi::fadds_ppc(lf(particle+0x10),lf(particle+4));position->y=gabi::fadds_ppc(lf(particle+0x14),lf(particle+8));position->z=gabi::fadds_ppc(lf(particle+0x18),lf(particle+12));
        f32 alpha;
        bool fadeOut;
        if(gabi::load<u8>(particle)==3) {
            alpha=zero;fadeOut=i>(s32)(ld(environment()+0xA4C)-1);
        } else {
            squared=gabi::call<f32>(0x028E8DE8,position.get(),camera+0xDC);
            f32 value=(f32)(gabi::call<f64>(0x028F4384,(f64)squared)+(f64)fadeOffset);
            value=value>=0.0f?value:zero;
            alpha=gabi::fsubs_ppc(one,value/lf(0x1004F7E0));
            if(alpha>one) {alpha=one;fadeOut=i>(s32)(ld(environment()+0xA4C)-1);}
            else {if(alpha<zero)alpha=zero;fadeOut=i>(s32)(ld(environment()+0xA4C)-1);}
        }
        if(fadeOut)gabi::call(0x0200ECD4,particle+0x2C,zero,approach,randomStep,small);
        else sf(particle+0x2C,alpha);
        if(i>(s32)(ld(environment()+0xA4C)-1)&&lf(particle+0x2C)<small&&(u32)i==(u32)(gabi::load<s16>(packet+0x3760)-1))gabi::store<u16>(packet+0x3760,(u16)(gabi::load<s16>(packet+0x3760)-1));
    }
}
VERIFY(0x02568BD4,snowMove);
}
