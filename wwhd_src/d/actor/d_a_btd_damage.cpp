/**
 * d_a_btd_damage.cpp (WWHD)
 * Boss - Gohma: damage_check
 *
 * Written against the WWHD code with the GameCube
 * decompilation (zeldaret/tww src/d/actor/d_a_btd.cpp) as reference, and verified against
 * cking.rpx.
 */
#include "d/actor/d_a_btd.h"
#include "gabi.h"
namespace {
template<class T> T rd(u32 p,u32 off=0) {return gabi::load<T>(p+off);}
template<class T> void wr(u32 p,u32 off,T v) {gabi::store<T>(p+off,v);}
u32 play() {return gabi::call<u32>(0x025200D4);}
void actorSound(u32 p,u32 id) {
    if(!(p+0x37C)) return;
    s32 actorId=p?(s32)rd<u32>(p,4):-1;
    s32 room=rd<s8>(p,0x326);s32 reverb=gabi::call<s32>(0x02520540,room);
    gabi::call<void>(0x025E1AA4,id,p+0x37C,actorId,0,reverb);
}
}

void damage_check(btd_class* self) {
    WWHD_FUNC(0x020F4D80,void,self);
    u32 p=gabi::ea(self);play();
    if(rd<u8>(p,0x41E)==1) {
        wr<u8>(p,0x41E,2);wr<s16>(p,0x40E,20);wr<s16>(p,0x40C,2);
        if(p) actorSound(p,0x483B);
        gabi::call<void>(0x020F0C38,self);return;
    }
    s32 hit=gabi::call<s32>(0x025162A4,p+0x1B94);
    if(hit && rd<u8>(p,0x408)==0 && rd<s16>(p,0x41C)==0) {
        wr<s16>(p,0x41C,10);u32 obj=gabi::call<u32>(0x02516300,p+0x1B94);
        gabi::call<void>(0x02518CC8,p,obj,0x40);
    }
    hit=gabi::call<s32>(0x025162A4,p+0x1B94);
    if(hit && rd<u8>(p,0x408)==1) {
        u32 flags=rd<u32>(p,0x1C28);s16 cooldown=rd<s16>(p,0x41C);wr<u32>(p,0x1C28,flags|2);
        if(cooldown==0) {
            wr<s16>(p,0x41C,7);u32 obj=gabi::call<u32>(0x02516300,p+0x1B94);
            struct AttackInfo_l {be<u32> object,actor;be<u8> damage,dead,type,pad;be<s16> angle[3];be<u16> cut;be<u32> position;be<s32> sound;};
            gabi::Local<AttackInfo_l> info;
            info->object=obj;info->position=p+0x1C60;
            gabi::call<void>(0x02518DB0,info.get());
            u8 health=rd<u8>(p,0x3A1),damage=info->damage,total=rd<u8>(p,0x5FAD);
            s8 remaining=(s8)(health-damage);
            wr<u8>(p,0x5FAD,(u8)(total+damage));wr<s8>(p,0x3A1,remaining);
            if(remaining<=0 || rd<u8>(0x104629FC,2)!=0) {
                u32 game=play();wr<f32>(game,0x5B44,0.0f);
                s16 pause=rd<s16>(0x1047B696);wr<u8>(0x101EACB7,0,(u8)(pause+8));
                u32 control=rd<u32>(play(),0x5AB0);
                gabi::call<void>(0x025A847C,control,0,0x10,p+0x508,0,0,255,0,-1,0,0,0);
                gabi::Local<csXyz> angles;gabi::Local<cXyz> scale;
                angles->x=0;angles->z=0;scale->z=2.0f;scale->y=2.0f;scale->x=2.0f;
                u32 player=rd<u32>(play(),0x5B2C);s16 angle=gabi::call<s16>(0x025D6894,p,player);angles->y=angle;
                control=rd<u32>(play(),0x5AB0);
                gabi::call<void>(0x025A847C,control,0,0xD,p+0x508,angles.get(),scale.get(),255,0,-1,0,0,0);
                wr<s16>(p,0x40C,11);wr<s16>(p,0x40E,50);
                if(p+0x37C) {s32 room=rd<s8>(p,0x326),reverb=gabi::call<s32>(0x02520540,room);gabi::call<void>(0x025E1A40,0x2828,p+0x37C,0,reverb);}
                return;
            }
            if(rd<s16>(p,0x40C)==2) {
                s8 totalDamage=rd<s8>(p,0x5FAD);s16 threshold=rd<s16>(0x104629FC,0xA);
                if(totalDamage>=threshold) {wr<u8>(p,0x5FAD,0);wr<s16>(p,0x40E,10);}
                else {
                    s32 animation=0x13;
                    if(info->type==1) {
                        u32 player=rd<u32>(play(),0x5B2C);s16 angle=gabi::call<s16>(0x025D6894,p,player);
                        s16 facing=rd<s16>(p,0x322),delta=(s16)(angle-facing);
                        if((u32)((s32)delta+255)>=511) animation=delta>0?0x14:0x12;
                    }
                    gabi::call<void>(0x020F03BC,self,animation,1.0f,u8(0),1.0f,-1);wr<s16>(p,0x40E,3);
                }
            } else {wr<s16>(p,0x40E,0);wr<s16>(p,0x40C,2);}
            actorSound(p,0x4836);gabi::call<void>(0x020F0C38,self);
        }
    }
    for(u32 i=0;i<19;++i) {
        s16 cooldown=rd<s16>(p,0x60EC+2*i);
        if(cooldown) {wr<s16>(p,0x60EC+2*i,(s16)(cooldown-1));continue;}
        u32 sph=p+0x550+0x12C*i;
        if(gabi::call<s32>(0x025162A4,sph)) {
            u32 flags=rd<u32>(sph,0x94);wr<u32>(sph,0x94,flags|2);wr<s16>(p,0x60EC+2*i,10);
            u32 src=p+0x424+12*i,dst=p+0x5FC0+12*i;
            // Native component copies are sequential, including any guest aliases.
            wr<u32>(dst,0,rd<u32>(src));wr<u32>(dst,4,rd<u32>(src,4));wr<u32>(dst,8,rd<u32>(src,8));
            u32 obj=gabi::call<u32>(0x02516300,sph);gabi::call<void>(0x02518D40,p,dst,obj,0x42);
            break;
        }
    }
    for(u32 i=0;i<6;++i) {
        s16 cooldown=rd<s16>(p,0x6112+2*i);
        if(cooldown) {wr<s16>(p,0x6112+2*i,(s16)(cooldown-1));continue;}
        u32 cyl=p+0x5884+0x130*i;
        if(gabi::call<s32>(0x025162A4,cyl)) {
            u32 flags=rd<u32>(cyl,0x94);wr<u32>(cyl,0x94,flags|2);wr<s16>(p,0x6112+2*i,10);
            u32 obj=gabi::call<u32>(0x02516300,cyl);gabi::call<void>(0x02518D40,p,p+0x60A4+12*i,obj,0x42);return;
        }
    }
}
VERIFY(0x020F4D80,damage_check);
