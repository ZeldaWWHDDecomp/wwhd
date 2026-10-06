/* Retained HD environmental sound TU0252FCB4..025303B3.
 * Raw layouts follow HD; external calls mocked. */
#include "bindings.h"
namespace d_envse_cpp {
static u32 ld(u32 a){return gabi::load<u32>(a);}
static u8 lb(u32 a){return gabi::load<u8>(a);}
static u16 lh(u32 a){return gabi::load<u16>(a);}
static void st(u32 a,u32 v){gabi::store<u32>(a,v);}
static void copy(u32 d,u32 s){u32 x=ld(s),y=ld(s+4),z=ld(s+8);st(d,x);st(d+8,z);st(d+4,y);}
struct Vec {be<u32> w[3];};
struct Line {be<u32> w[7];};
static BOOL Draw(){
    WWHD_FUNC(0x0252FCB4,BOOL,(u32)0);
    return TRUE;
}
VERIFY(0x0252FCB4,Draw);
static void nearPath(u32 out,u32 pos,u32 path){
    WWHD_FUNC(0x0252FCBC,void,out,pos,path);
    gabi::Local<be<f32>> distance;
    gabi::Local<Vec> first,second;
    gabi::Local<Line> line;
    u32 lin=gabi::ea(line.get()),a=gabi::ea(first.get()),b=gabi::ea(second.get());
    st(lin+24,0x1004CC7C);
    f64 best=gabi::load<f32>(0x1004CC70);
    u32 nearest=0,point=ld(path+8),count=lh(path);
    for(u32 i=0;i<count;++i){
        f64 d=gabi::call<f64>(0x028E8DE8,pos,point+4);
        *distance=(f32)d;
        if(best>d){best=d;nearest=i;}
        count=lh(path);
        point+=16;
    }
    point=ld(path+8)+nearest*16;
    u32 hasFirst=0,hasSecond=0;
    if(nearest!=0){
        gabi::call(0x0201883C,lin,point-12,point+4);
        hasFirst=gabi::call<u32>(0x02010AE4,lin,pos,a,gabi::ea(distance.get()));
        count=lh(path);
    }
    if(nearest!=count-1){
        gabi::call(0x0201883C,lin,point+4,point+20);
        hasSecond=gabi::call<u32>(0x02010AE4,lin,pos,b,gabi::ea(distance.get()));
    }
    if(hasFirst){
        if(!hasSecond || gabi::call<f64>(0x028E8DE8,b,pos)>(f64)(f32)*distance) copy(out,a);
        else copy(out,b);
    }else if(hasSecond) copy(out,b);
    else {
        gabi::store<f32>(out,gabi::load<f32>(point+4));
        gabi::store<f32>(out+4,gabi::load<f32>(point+8));
        gabi::store<f32>(out+8,gabi::load<f32>(point+12));
    }
}
VERIFY(0x0252FCBC,nearPath);
static BOOL execute(u32 self){
    WWHD_FUNC(0x0252FEB8,BOOL,self);
    gabi::Local<Vec> eye,temp1,temp2;
    u32 e=gabi::ea(eye.get()),a=gabi::ea(temp1.get()),b=gabi::ea(temp2.get());
    s32 room=(s8)lb(0x1047E6C8);
    u32 play=gabi::call<u32>(0x025200D4);
    u32 roomdata=gabi::call<u32>(0x025C11DC,play+0x51CC,room);
    if(!roomdata)return TRUE;
    u32 sound=gabi::call_ptr<u32>(ld(ld(roomdata)+0x1BC),roomdata);
    if(!sound)return TRUE;
    u32 count=ld(sound),entry=ld(sound+4);
    play=gabi::call<u32>(0x025200D4);
    s32 camera=(s8)lb(play+0x5B30);
    play=gabi::call<u32>(0x025200D4);
    u32 cam=ld(play+(u32)camera*52+0x5AF8);
    gabi::call(0x0201AD78,cam+0x264,e,cam+0x7C0);
    f64 maximum=gabi::load<f32>(0x1004CC70);
    for(;count!=0;--count,entry+=28){
        u32 type=lb(entry+23),path;
        if(type==0 || type==2 || type==4){
            gabi::call(type==0?0x025E1B70:type==2?0x025E1BF8:0x025E1C20);
            path=gabi::call<u32>(0x025AAF88,(u32)lb(entry+24),room);
            while(path){
                gabi::call(0x0252FCBC,self+0xE0,e,path);
                gabi::call(type==0?0x025E1B7C:type==2?0x025E1BD0:0x025E1C2C,self+0xE0);
                path=gabi::call<u32>(0x025AB070,path,room);
            }
            if(type==0)gabi::call(0x025E1B8C,(u32)lb(entry+20));
            else if(type==2){u32 reverb=gabi::call<u32>(0x02520540,room);gabi::call(0x025E1BE0,(u32)lb(entry+20),reverb);}
        }else if(type==1 || type==3){
            copy(self+0xE0,e);
            f64 nearest=maximum;u32 arg=0;
            path=gabi::call<u32>(0x025AAF88,(u32)lb(entry+24),room);
            u32 tmp=type==1?a:b;
            while(path){
                gabi::call(0x0252FCBC,tmp,e,path);
                f64 d=gabi::call<f64>(0x028E8DE8,tmp,e);
                if(d<nearest){
                    if(type==1)arg=lb(path+4);
                    copy(self+0xE0,tmp);nearest=d;
                }
                path=gabi::call<u32>(0x025AB070,path,room);
            }
            if(type==1){
                if(ld(self+0xFC)==0 && nearest!=maximum){
                    gabi::call(0x025E1BA0,self+0xE0,arg);st(self+0x100,arg);
                }else gabi::call(0x025E1BB8,self+0xE0,ld(self+0x100));
                u16 frame=lh(self+0xFE);
                play=gabi::call<u32>(0x025200D4);
                gabi::store<u16>(play+0x5D00,frame);
                u32 ticks=ld(self+0xFC);
                st(self+0xFC,ticks==99?0:ticks+1);
            }else{u32 reverb=gabi::call<u32>(0x02520540,room);gabi::call(0x025E1C04,(u32)lb(entry+20),self+0xE0,reverb);}
        }
    }
    return TRUE;
}
VERIFY(0x0252FEB8,execute);
static BOOL Execute(u32 self){
    WWHD_FUNC(0x02530288,BOOL,self);
    return gabi::call<BOOL>(0x0252FEB8,self);
}
VERIFY(0x02530288,Execute);
static BOOL IsDelete(){
    WWHD_FUNC(0x0253028C,BOOL,(u32)0);
    return TRUE;
}
VERIFY(0x0253028C,IsDelete);
static BOOL Delete(u32 self){
    WWHD_FUNC(0x02530294,BOOL,self);
    gabi::call(0x025E1B34,self+0xE0);
    if(self)gabi::call(0x025DD630,self,0);
    return TRUE;
}
VERIFY(0x02530294,Delete);
static s32 Create(u32 self){
    WWHD_FUNC(0x025302DC,s32,self);
    if(self){gabi::call(0x025DD5F0,self);st(self+0xB4,0x1004CC8C);}
    return 4;
}
VERIFY(0x025302DC,Create);
static void sinit(){
    WWHD_FUNC(0x02530320,void,(u32)0);
    st(0x10475848,0);st(0x10475840,0);st(0x1047584C,0);st(0x10475844,0);
    gabi::call(0x028F026C,0x101D61A8);
    f32 a=gabi::load<f32>(0x1004CCA4),b=gabi::load<f32>(0x1004CCA8);
    gabi::store<f32>(0x10475834,a);gabi::store<f32>(0x10475838,b);
    gabi::call(0x028ED6F8,0x1047583C);gabi::call(0x028F026C,0x101D61B4);
    gabi::call(0x028EAB2C,0x1047583D);gabi::call(0x028F026C,0x101D61C0);
}
VERIFY(0x02530320,sinit);
}
