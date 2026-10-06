#include "d_kankyo_rain_private.h"
namespace d_kankyo_rain_cpp {
static void sunMove() {
    WWHD_FUNC(0x02565E0C,void);
    u32 sun=ld(environment()+0xA34),flare=ld(environment()+0xA38),camera=ld(gameInfo()+0x5AF8);
    gameInfo();u32 info=gameInfo();u32 secondaryCamera=ld(info+0x5AF8);
    f32 zero=lf(0x1004F528),blend=zero;
    u32 stageMode=0,centralVisible=0,visible=0;
    gabi::Local<SafeString_l> stage,current;
    gabi::Local<Vec_l> direction,screen,point;
    gabi::Local<ProjectParams_l> params;
    stage->text=0x1004F64C;stage->vtable=0x1004F3AC;
    current->text=gameInfo()+0x5134;current->vtable=0x1004F3AC;
    safe_string_callback(stage.get());safe_string_callback(stage.get());
    u32 expected=(u32)stage->text;
    safe_string_callback(current.get());
    bool same=equal_terminated_strings(expected,(u32)current->text);
    bool alternate;
    if(same)alternate=gabi::load<s16>(environment()+0x20)!=0;
    else {
        u32 object=gameInfo()+0x5150;
        u32 returned=gabi::call_ptr<u32>(ld(ld(object)+0x15C),object);
        stageMode=(ld(returned+12)>>16)&7;
        alternate=gabi::load<s16>(environment()+0x20)!=0;
    }
    if(stageMode==2)alternate=true;
    u32 env=environment();
    gabi::call(0x02563F64,camera+0xDC,env+(alternate?0xB38:0x14),direction.get());
    f32 distanceScale=lf(0x1004F614);
    sf(sun+0x98,gabi::fmadds((f32)direction->x,distanceScale,lf(camera+0xDC)));
    f32 sunY=gabi::fmadds((f32)direction->y,distanceScale,lf(camera+0xE0));sf(sun+0x9C,sunY);
    sf(sun+0xA0,gabi::fmadds((f32)direction->z,distanceScale,lf(camera+0xE4)));
    f32 height=gabi::fsubs_ppc(sunY,lf(camera+0xE0))/distanceScale,one=lf(0x1004F550);
    if(height<zero)height=gabi::fsubs_ppc(one,zero);
    else {if(!(height<one))height=one;height=gabi::fsubs_ppc(one,height);}
    height=gabi::fnmsubs(height,height,one);
    u8 delay=gabi::load<u8>(sun+0xC4);if(delay)gabi::store<u8>(sun+0xC4,delay-1);
    gabi::store<u8>(sun+0xC5,0);
    f32 half=lf(0x1004F57C),small=lf(0x1004F580);
    if(lf(environment()+0x1020)>lf(0x1004F618)&&lf(environment()+0x1020)<lf(0x1004F61C)) {
        gabi::call(0x02563DD8,params.get());
        gabi::call(0x0200ECD4,sun+0xCC,one,half,lf(0x1004F588),small);
        f32 top=secondaryCamera?gabi::fmuls_ppc(lf(secondaryCamera+0x844),lf(0x1004F620)):zero;
        gabi::call(0x025F1018,sun+0x98,screen.get(),params.get());
        f32 depthLimitZero=zero,bottom=gabi::fsubs_ppc(lf(0x1004F624),top);
        f64 horizontalLimit=ldbl(0x1004F628);
        u32 misses=0;
        for(u32 i=0;i<5;i++) {
            f32 x=gabi::fsubs_ppc((f32)screen->x,lf(0x1004F654+i*8));
            f32 y=gabi::fsubs_ppc((f32)screen->y,lf(0x1004F658+i*8));
            if(x>depthLimitZero&&(f64)x<horizontalLimit&&y>top&&y<bottom) {
                u32 result=sun+0xB0+i*4;
                if(ld(result)>=0xFFFFFF) {visible=(visible+1)&255;if(i==0)centralVisible=(centralVisible+1)&255;}
                gabi::call(0x0252E34C,gameInfo()+0x60EC,(s32)(s16)gabi::ftoi(x),(s32)(s16)gabi::ftoi(y),result);
            } else misses++;
        }
        if(visible) {
            delay=gabi::load<u8>(sun+0xC4);
            if(misses&&centralVisible) {visible=5;centralVisible=1;}
            if(delay<5)gabi::store<u8>(sun+0xC4,delay+2);
            gabi::store<u8>(sun+0xC5,1);
        }
        sf(flare+0x110,lf(flare+0x118));sf(flare+0x114,lf(flare+0x11C));
        sf(flare+0x118,lf(0x1004F630));sf(flare+0x11C,zero);
        point->x=(f32)screen->x;point->y=lf(0x1004F5F4);point->z=zero;
        f32 squared=gabi::call<f32>(0x028E8DE8,point.get(),screen.get());
        blend=(f32)(gabi::call<f64>(0x028F4384,(f64)squared)/(f64)lf(0x1004F5FC));if(blend>one)blend=one;
        point->x=lf(0x1004F5F0);point->y=(f32)screen->y;
        squared=gabi::call<f32>(0x028E8DE8,point.get(),screen.get());
        f32 side=(f32)(gabi::call<f64>(0x028F4384,(f64)squared)/(f64)lf(0x1004F634));if(side>one)side=one;
        f32 edge=gabi::fnmsubs(gabi::fadds_ppc(side,blend),half,one);
        blend=gabi::fmuls_ppc(edge,edge);sf(flare+0x124,gabi::fsubs_ppc(one,blend));blend=gabi::fmuls_ppc(blend,blend);
    } else {
        gabi::call(0x0200ECD4,sun+0xCC,zero,half,lf(0x1004F588),small);
        gabi::store<u8>(sun+0xC4,0);gabi::store<u8>(sun+0xC5,0);visible=0;
    }
    if(gabi::load<u8>(environment()+0x1092))centralVisible=visible=0;
    else if(gabi::load<u8>(environment()+0x108D)&&lf(environment()+0xFC8)>half)centralVisible=visible=0;
    if(stageMode==2)centralVisible=visible=0;
    bool daylight=lf(environment()+0x1020)<lf(0x1004F638);
    if(!daylight)daylight=lf(environment()+0x1020)>lf(0x1004F63C);
    u32 noFlare;
    if(daylight) {
        gabi::call(0x0200ECD4,sun+0xC8,zero,half,lf(0x1004F598),lf(0x1004F588));visible=0;
        sf(sun+0xC8,zero);gabi::store<u32>(sun+0xE8,3);noFlare=1;
    } else if(centralVisible&&visible==5) {
        gabi::call(0x0200ECD4,sun+0xC8,one,half,half,lf(0x1004F588));
        u32 wait=ld(sun+0xE8);
        if(wait) {wait--;gabi::store<u32>(sun+0xE8,wait);}
        if(wait)noFlare=0;
        else {gabi::call(0x0200ECD4,sun+0xC8,lf(0x1004F640),lf(0x1004F554),lf(0x1004F554),small);noFlare=0;}
    } else if(!centralVisible) {
        gabi::call(0x0200ECD4,sun+0xC8,zero,half,lf(0x1004F598),lf(0x1004F588));
        if(!visible) {sf(sun+0xC8,zero);gabi::store<u32>(sun+0xE8,3);noFlare=1;}
        else noFlare=visible<2?1:0;
    } else noFlare=visible<2?1:0;
    gabi::store<u8>(ld(environment()+0xA38)+0x12A,noFlare);
    if(lf(sun+0x9C)>zero&&!gabi::load<u8>(ld(environment()+0xA38)+0x12A)) {
        gabi::call(0x0255A3D8,gabi::fnmsubs(blend,lf(sun+0xC8),one));
        gabi::call(0x0255A418,gabi::fnmsubs(blend,lf(sun+0xC8),one));
        gabi::call(0x0255A458,gabi::fmadds(gabi::fmuls_ppc(gabi::fmuls_ppc(blend,lf(sun+0xC8)),height),half,one));
        gabi::call(0x0255A51C,gabi::fmadds(gabi::fmuls_ppc(gabi::fmuls_ppc(blend,lf(sun+0xC8)),height),half,one));
    }
    if(gabi::call<u32>(0x02565DAC)) {
        height=gabi::fsubs_ppc(lf(sun+0x9C),lf(camera+0xE0))/lf(0x1004F644);
        height=gabi::fmuls_ppc(gabi::fmuls_ppc(height,height),lf(0x1004F648));if(height>one)height=one;
        gabi::call(0x0200ECD4,sun+0xD0,height,lf(0x1004F554),small,lf(0x1004F58C));
    } else gabi::call(0x0200ECD4,sun+0xD0,zero,lf(0x1004F554),small,lf(0x1004F58C));
}
VERIFY(0x02565E0C,sunMove);
}
