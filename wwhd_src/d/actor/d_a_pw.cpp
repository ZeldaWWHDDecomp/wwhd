/* d_a_pw.cpp (WWHD): Poe enemy, verified against cking.rpx. */
#include "d/actor/d_a_pw.h"
using namespace gabi;

namespace {
template<class T> T read(u32 a,u32 off=0) { return load<T>(a+off); }
template<class T> void write(u32 a,u32 off,T value) { store<T>(a+off,value); }
f32 add(f32 a,f32 b) { volatile double x=(double)a,y=(double)b; return (f32)(x+y); }
f32 sub(f32 a,f32 b) { volatile double x=(double)a,y=(double)b; return (f32)(x-y); }
}

void pw_first_mode_change(pw_class* actor) {
    WWHD_FUNC(0x0244B3AC,void,actor);
    u32 a=ea(actor),flags=read<u32>(a,0x2E0);
    s16 yaw=read<s16>(a,0x32A);
    write<u8>(a,0x45C,0); write<s16>(a,0x4A8,yaw); write<s16>(a,0x322,yaw);
    write<s16>(a,0x4B8,100); write<u32>(a,0x39C,4);
    write<s16>(a,0x4B6,100); write<u32>(a,0x2E0,flags|0x20);
}
VERIFY(0x0244B3AC,pw_first_mode_change);

s32 pw_is_delete(pw_class* actor) {
    WWHD_FUNC(0x0244DD64,s32,actor); return 1;
}
VERIFY(0x0244DD64,pw_is_delete);

void pw_background_check(pw_class* actor) {
    WWHD_FUNC(0x0244A768,void,actor);
    u32 a=ea(actor); call(0x024EFF44,a+0x4D8,70.f,140.f);
    f32 y=read<f32>(a,0x318),offset=read<f32>(a,0x4BC),oldY=read<f32>(a,0x304);
    write<f32>(a,0x318,sub(y,offset)); write<f32>(a,0x304,sub(oldY,offset));
    u32 play=call<u32>(0x025200D4); call(0x024F08A8,a+0x518,play+0x12A0);
    offset=read<f32>(a,0x4BC); y=read<f32>(a,0x318); oldY=read<f32>(a,0x304);
    write<f32>(a,0x318,add(y,offset)); write<f32>(a,0x304,add(oldY,offset));
}
VERIFY(0x0244A768,pw_background_check);

s32 pw_delete(pw_class* actor) {
    WWHD_FUNC(0x0244DD6C,s32,actor);
    u32 a=ea(actor); call(0x025204C8,a+0x3C8,0x10038514u);
    if(read<u32>(a,0xF4)) call(0x025E563C,read<u32>(a,0x3D0));
    if(read<u8>(a,0x45F)) { write<u8>(a,0x45F,0); write<u8>(0x101CF0A4,0,0); }
    u32 vtable=read<u32>(a,0x6E0); call_ptr<void>(read<u32>(vtable,0x44),a+0x6E0);
    call(0x02041C30,a+0xD58);
    if(read<s16>(a,0x484)==0x51) {
        u32 play=call<u32>(0x025200D4),player=read<u32>(play,0x5B34);
        write<u32>(player,0x3BC,read<u32>(player,0x3BC)&~0x100u);
    }
    return 1;
}
VERIFY(0x0244DD6C,pw_delete);

void pw_small_delete(u32 object,u32 flags) {
    WWHD_FUNC(0x0244E94C,void,object,flags);
    if(object && (flags&1)) call(0x0273AF40,object);
}
VERIFY(0x0244E94C,pw_small_delete);

void pw_empty_virtual(pw_class* actor) {
    WWHD_FUNC(0x02450D7C,void,actor);
}
VERIFY(0x02450D7C,pw_empty_virtual);

void pw_alpha_animation(pw_class* actor) {
    WWHD_FUNC(0x0244A9E4,void,actor);
    u32 a=ea(actor);
    if(!read<u8>(a,0x45A)) {
        write<s16>(a,0x4AA,(s16)(read<s16>(a,0x4AA)+0x400));
        call(0x0200F428,a+0x4B8,100,1,10);
        f32 base=(f32)read<s16>(a,0x4B8);
        u16 angle=read<u16>(a,0x4AA);
        f32 sine=read<f32>(0x104A44F8+((angle>>3)*8));
        f32 alpha=fmadds(sine,read<f32>(0x10038470),base);
        write<s16>(a,0x4B6,(s16)ftoi(alpha));
    }
}
VERIFY(0x0244A9E4,pw_alpha_animation);

void pw_color_to_float(u32 output,u32 color) {
    WWHD_FUNC(0x02449ECC,void,output,color);
    f32 red=(f32)read<u8>(color),green=(f32)read<u8>(color,1);
    f32 blue=(f32)read<u8>(color,2),alpha=(f32)read<u8>(color,3);
    red=(f32)((double)red/255.0); green=(f32)((double)green/255.0);
    blue=(f32)((double)blue/255.0); alpha=(f32)((double)alpha/255.0);
    write<f32>(output,0,red); write<f32>(output,4,green);
    write<f32>(output,8,blue); write<f32>(output,12,alpha);
}
VERIFY(0x02449ECC,pw_color_to_float);

u32 pw_fire_constructor(u32 object) {
    WWHD_FUNC(0x0244E184,u32,object);
    if(!object) { object=call<u32>(0x0273AD10,0x22Cu); if(!object) return 0; }
    if(object+0x8C==0) call<u32>(0x0273AD10,12u);
    call(0x0200BD2C,object+0xA0); call(0x02515DA0,object+0xBC);
    write<u32>(object,0xB8,0x1004AE88); write<u32>(object,0xBC,0x1004AEC0);
    call(0x025166F0,object+0xDC); write<f32>(object,0x228,1.f);
    return object;
}
VERIFY(0x0244E184,pw_fire_constructor);

u32 pw_constructor(u32 a) {
    WWHD_FUNC(0x0244E210,u32,a);
    if(!a) { a=call<u32>(0x0273AD10,0xF8Cu); if(!a) return 0; }
    call(0x025D4ED0,a); write<u32>(a,0xB4,0x1003841C);
    call(0x024EFE94,a+0x4D8); call(0x024F0474,a+0x518);
    write<u32>(a,0x528,0x100383AC); write<u32>(a,0x538,0x100383BC);
    write<u32>(a,0x52C,0x100383CC); write<u8>(a,0x530,1);
    call(0x025A5894,a+0x6E0,0u,0u);
    call(0x0200BD2C,a+0x708); call(0x02515DA0,a+0x724);
    write<u32>(a,0x720,0x1004AE88); write<u32>(a,0x724,0x1004AEC0);
    call(0x02515FB8,a+0x744);
    write<u32>(a,0x858,0x100015A8); write<u32>(a,0x854,0x1003837C);
    call(0x02018590,a+0x85C);
    write<u32>(a,0x780,0x1004B108); write<u32>(a,0x858,0x1004B160); write<u32>(a,0x870,0x1004B150);
    call(0x025166F0,a+0x874); call(0x0200BD2C,a+0x9D0); call(0x02515DA0,a+0x9EC);
    write<u32>(a,0x9E8,0x1004AE88); write<u32>(a,0x9EC,0x1004AEC0);
    call(0x02515FB8,a+0xA0C);
    write<u32>(a,0xB1C,0x1003837C); write<u32>(a,0xB20,0x100015A8);
    call(0x02018590,a+0xB24);
    write<u32>(a,0xB38,0x1004B150); write<u32>(a,0xB20,0x1004B160); write<u32>(a,0xA48,0x1004B108);
    call(0x024EFE94,a+0xB54); call(0x024F0474,a+0xB94);
    write<u8>(a,0xBAC,1); write<u32>(a,0xBB4,0x100383BC);
    write<u32>(a,0xBA8,0x100383CC); write<u32>(a,0xBA4,0x100383AC);
    call(0x0244E184,a+0xD58); call(0x025E895C,a+0xF84); return a;
}
VERIFY(0x0244E210,pw_constructor);

void pw_static_initialize() {
    WWHD_FUNC(0x0244E8B8,void);
    write<u32>(0x1046D3F4,8,0); write<u32>(0x1046D3F4,0,0);
    write<u32>(0x1046D3F4,12,0); write<u32>(0x1046D3F4,4,0);
    call(0x028F026C,0x101CF180u);
    f32 low=read<f32>(0x10038530),high=read<f32>(0x10038534);
    write<f32>(0x1046D3E8,0,low); write<f32>(0x1046D3EC,0,high);
    call(0x028ED6F8,0x1046D3F0u); call(0x028F026C,0x101CF18Cu);
    call(0x028EAB2C,0x1046D3F1u); call(0x028F026C,0x101CF198u);
}
VERIFY(0x0244E8B8,pw_static_initialize);

void pw_destructor(u32 a,u32 flags) {
    WWHD_FUNC(0x02450C68,void,a,flags);
    if(!a) return;
    call(0x025E89F8,a+0xF84,2); call(0x02515AE8,a+0xE34,2);
    call(0x02515860,a+0xDF8,2);
    write<u32>(a,0xBB4,0x100383BC); write<u32>(a,0xBA8,0x100383CC);
    call(0x024EFD9C,a+0xB94,0); call(0x02018034,a+0xB68,2);
    call(0x02515A70,a+0xA0C,2); call(0x02515860,a+0x9D0,2);
    call(0x02515AE8,a+0x874,2); call(0x02515A70,a+0x744,2);
    call(0x02515860,a+0x708,2);
    write<u32>(a,0x538,0x100383BC); write<u32>(a,0x52C,0x100383CC);
    call(0x024EFD9C,a+0x518,0); call(0x02018034,a+0x4EC,2);
    call(0x025D50BC,a,0); if(flags&1) call(0x0273AF40,a);
}
VERIFY(0x02450C68,pw_destructor);

s32 pw_jalhalla_merge_check(pw_class* actor) {
    WWHD_FUNC(0x0244A5A8,s32,actor);
    u32 a=ea(actor),id=read<u32>(a,0x488); if(id==0xFFFFFFFF) return 0;
    Local<be<u32>> found;
    if(!call<s32>(0x025D54C4,id,found.get())) return 0;
    u32 other=found->get(); if(!other || read<s16>(other,8)!=0xD3 || !read<u8>(other,0x505)) return 0;
    u32 flags=read<u32>(a,0x744),target=read<u32>(a,0x75C);
    write<u8>(a,0x45C,0); write<u32>(a,0x744,flags&~1u); write<u32>(a,0x75C,target&~1u);
    call(0x0251621C,a+0x744);
    s16 angle=call<s16>(0x025D6894,a,found->get());
    write<s16>(a,0x4A8,angle); write<s16>(a,0x482,5); write<s16>(a,0x484,0xAA); return 1;
}
VERIFY(0x0244A5A8,pw_jalhalla_merge_check);

s32 pw_jalhalla_down_check(pw_class* actor) {
    WWHD_FUNC(0x0244A67C,s32,actor);
    u32 a=ea(actor);
    if(read<s8>(a,0x3A1)>0 || read<u8>(a,0x461)) return 0;
    u32 id=read<u32>(a,0x488); if(id==0xFFFFFFFF) return 0;
    Local<be<u32>> found;
    if(!call<s32>(0x025D54C4,id,found.get())) return 0;
    u32 other=found->get(); if(!other || read<s16>(other,8)!=0xD3) return 0;
    if(read<s16>(other,0x562)==0x6F && read<s16>(other,0x56A)>3) {
        write<u8>(other,0x3A1,(u8)(read<u8>(other,0x3A1)-1));
        if(read<s8>(found->get(),0x3A1)<=0) write<u8>(a,0x460,1);
        write<u8>(a,0x461,1); return 0;
    }
    write<u8>(a,0x3A1,4); return 1;
}
VERIFY(0x0244A67C,pw_jalhalla_down_check);

void pw_animation_init(pw_class* actor,s32 animation,f32 blend,u32 loop,f32 speed,s32 sound) {
    WWHD_FUNC(0x0244A37C,void,actor,animation,blend,loop,speed,sound);
    u32 a=ea(actor); write<s32>(a,0x490,animation);
    struct String { be<u32> data; be<u32> vtable; };
    Local<String> archive;
    archive->data=0x10038448; archive->vtable=0x10038364;
    u32 transform=call<u32>(0x026066C4,read<u32>(0x101F4F28),archive.get(),animation),soundData=0;
    if(sound>=0) {
        Local<String> soundArchive; soundArchive->data=0x10038448; soundArchive->vtable=0x10038364;
        soundData=call<u32>(0x026066C4,read<u32>(0x101F4F28),soundArchive.get(),sound);
    }
    call(0x025E4A98,read<u32>(a,0x3D0),transform,loop,blend,speed,0.f,-1.f,soundData);
}
VERIFY(0x0244A37C,pw_animation_init);

void pw_move_sound(pw_class* actor) {
    WWHD_FUNC(0x0244B308,void,actor);
    u32 a=ea(actor); f32 volume=(f32)((double)read<f32>(a,0x370)*4.0);
    u32 level=!(volume<2147483648.f) ? (u32)ftoi(sub(volume,2147483648.f))+0x80000000u : (u32)ftoi(volume);
    if(a+0x37C) { s32 reverb=call<s32>(0x02520540,(s32)read<s8>(a,0x326)); call(0x025E1A40,0x7051u,a+0x37C,level,reverb); }
}
VERIFY(0x0244B308,pw_move_sound);

s32 pw_range_check(pw_class* actor) {
    WWHD_FUNC(0x0244B058,s32,actor);
    u32 a=ea(actor); if(read<u32>(a,0x488)!=0xFFFFFFFF) return 0;
    bool path=read<u8>(a,0x475)!=0xFF && read<u32>(a,0x464)!=0;
    f32 z=read<f32>(a,path?0x420:0x414),actorZ=read<f32>(a,0x31C),actorX=read<f32>(a,0x314);
    f32 dz=sub(z,actorZ),x=read<f32>(a,path?0x418:0x40C),dx=sub(x,actorX);
    f32 squareZ=(f32)((double)dz*dz);
    f32 radius=path?500.f:1000.f;
    f32 distance=call<f32>(0x028F4384,fmadds(dx,dx,squareZ));
    bool far=path ? !(distance<=radius) : distance>radius;
    if(!far) {
        if(read<s16>(a,0x484)==0x5B || !call<s32>(0x0244ACBC,a,1)) return 0;
    }
    write<s16>(a,0x4A8,call<s16>(0x020195B0,dx,dz)); return 1;
}
VERIFY(0x0244B058,pw_range_check);

void pw_lantern_break(pw_class* actor) {
    WWHD_FUNC(0x0244A4A8,void,actor);
    u32 a=ea(actor),play=call<u32>(0x025200D4),id=read<u32>(a,0x48C);
    u32 player=read<u32>(play,0x5B2C); write<u16>(a,0x462,0);
    if(id==0xFFFFFFFF) return;
    Local<be<u32>> found;
    if(!call<s32>(0x025D54C4,id,found.get())) return;
    u32 lantern=load<u32>(ea(found.get()));
    if(!lantern || read<s16>(lantern,8)!=0xC1) return;
    f32 px=read<f32>(player,0x314),lx=read<f32>(lantern,0x314);
    f32 pz=read<f32>(player,0x31C),lz=read<f32>(lantern,0x31C);
    s32 angle=call<s32>(0x020195B0,sub(lx,px),sub(lz,pz));
    call(0x025F1884,read<u32>(0x1018C7B0),angle);
    struct Vec { be<f32> x,y,z; };
    Local<Vec> velocity; velocity->x=0.f; velocity->y=0.f; velocity->z=20.f;
    call(0x0200FCD8,velocity.get(),load<u32>(ea(found.get()))+0x33C);
    write<f32>(load<u32>(ea(found.get())),0x340,10.f);
    write<u8>(lantern,0x484,5); write<u32>(a,0x48C,0xFFFFFFFF);
}
VERIFY(0x0244A4A8,pw_lantern_break);

s32 pw_light_range_check(pw_class* actor,s32 mode) {
    WWHD_FUNC(0x0244ACBC,s32,actor,mode);
    u32 a=ea(actor),play=call<u32>(0x025200D4);
    f32 x=read<f32>(a,0x314); u32 player=read<u32>(play,0x5B2C);
    struct Vec { be<f32> x,y,z; };
    Local<Vec> position; position->x=x; position->y=read<f32>(a,0x318);
    f32 z=read<f32>(a,0x31C); position->z=z;
    if(mode==0) {
        f32 ax=read<f32>(a,0x314),px=read<f32>(player,0x314);
        f32 az=read<f32>(a,0x31C),pz=read<f32>(player,0x31C);
        f32 dx=(f32)((double)sub(ax,px)*0.33000001311302185f);
        f32 dz=(f32)((double)sub(az,pz)*0.33000001311302185f);
        for(s32 i=0;i<3;++i) {
            position->x=add(x,dx); position->z=add(z,dz);
            play=call<u32>(0x025200D4);
            if(call<s32>(0x0252A038,play+0x5A20,position.get())) return 1;
            x=position->x; z=position->z;
        }
        return 0;
    }
    call(0x025F1884,read<u32>(0x1018C7B0),(s32)read<s16>(a,0x322));
    Local<Vec> offset; offset->x=0.f; offset->y=0.f; offset->z=200.f;
    call(0x0200FCD8,offset.get(),position.get());
    call(0x028E8D88,position.get(),a+0x314,position.get());
    play=call<u32>(0x025200D4);
    return call<s32>(0x0252A038,play+0x5A20,position.get())!=0;
}
VERIFY(0x0244ACBC,pw_light_range_check);

void pw_next_action_check(pw_class* actor) {
    WWHD_FUNC(0x0244B1B8,void,actor);
    u32 a=ea(actor),play=call<u32>(0x025200D4),flags=read<u32>(a,0x7D8);
    u32 player=read<u32>(play,0x5B2C);
    write<u32>(a,0x76C,0x900000); write<u32>(a,0x7D8,flags&~2u);
    struct Vec { be<f32> x,y,z; };
    Local<Vec> target; target->x=read<f32>(player,0x314);
    target->y=read<f32>(player,0x318); target->z=read<f32>(player,0x31C);
    if(!call<s32>(0x0244A7F0,a,target.get(),1) && !call<s32>(0x0244B058,a)) {
        play=call<u32>(0x025200D4);
        f32 distance=call<f32>(0x025D68EC,a,read<u32>(play,0x12A0+0x488C));
        if(distance<500.f) {
            f32 playerY=read<f32>(player,0x318),actorY=read<f32>(a,0x318);
            f32 difference=sub(actorY,playerY);
            if(__builtin_fabsf(difference)<100.f) {
                write<s16>(a,0x482,1); write<s16>(a,0x484,0x20); return;
            }
        }
    }
    f32 random=call<f32>(0x020198D8,70.f);
    s32 timer=ftoi(add(random,70.f)); u8 path=read<u8>(a,0x475);
    write<s16>(a,0x498,(s16)timer);
    if(path==0xFF) {
        f32 x=read<f32>(a,0x314),z=read<f32>(a,0x31C);
        write<f32>(a,0x40C,x); write<f32>(a,0x414,z);
    }
    write<s16>(a,0x482,0); write<s16>(a,0x484,0xD);
}
VERIFY(0x0244B1B8,pw_next_action_check);

void pw_draw_sub(pw_class* actor) {
    WWHD_FUNC(0x02449DA8,void,actor);
    u32 a=ea(actor),morf=read<u32>(a,0x3D0);
    f32 sy=read<f32>(a,0x334); u32 model=read<u32>(morf,0x90);
    f32 sx=read<f32>(a,0x330),sz=read<f32>(a,0x338);
    write<f32>(model,0xBC,sx); write<f32>(model,0xC4,sz); write<f32>(model,0xC0,sy);
    constexpr u32 matrix=0x1048D0CC;
    f32 offset=read<f32>(a,0x4C0),y=read<f32>(a,0x318),x=read<f32>(a,0x314),z=read<f32>(a,0x31C);
    call(0x028E93CC,matrix,x,add(y,offset),z);
    call(0x025F1C28,matrix,(s32)read<s16>(a,0x32A));
    call(0x025F1BF4,matrix,(s32)read<s16>(a,0x328));
    call(0x025F1C5C,matrix,(s32)read<s16>(a,0x32C));
    f32 values[12]; for(u32 i=0;i<12;++i) values[i]=read<f32>(matrix,i*4);
    const u32 order[12]={0,10,8,5,11,9,3,1,2,7,6,4};
    for(u32 i:order) write<f32>(model,0xC8+i*4,values[i]);
    call(0x025E55A0,read<u32>(a,0x3D0)); call(0x02041570,a+0xD58);
    u32 lighting=call<u32>(0x02555D0C);
    call(0x025626A4,lighting,0,a+0x314,a+0x110);
}
VERIFY(0x02449DA8,pw_draw_sub);

s32 pw_node_callback(u32 joint,s32 phase) {
    WWHD_FUNC(0x02449C60,s32,joint,phase);
    if(phase) return 1;
    u32 info=call<u32>(0x027F7878,joint),model=read<u32>(0x104B462C);
    u32 actor=read<u32>(model,0xB8); u16 index=read<u16>(info,4);
    if(!actor || index!=0x17) return 1;
    u32 data=read<u32>(model,0x2C); u16 flags=read<u16>(data,4);
    u32 matrices=read<u32>(data,0x10); write<u16>(data,4,flags|0x10);
    call(0x028E90D4,matrices+0x450,read<u32>(0x1018C7B0));
    u32 x=read<u32>(actor,0x3E8),y=read<u32>(actor,0x3EC);
    write<u32>(actor,0x400,x); u32 z=read<u32>(actor,0x3F0);
    write<u32>(actor,0x404,y); write<u32>(actor,0x408,z);
    struct Vec { be<f32> x,y,z; };
    Local<Vec> origin; origin->x=0.f; origin->y=0.f; origin->z=0.f;
    call(0x0200FCD8,origin.get(),actor+0x3E8);
    data=read<u32>(model,0x2C); u32 matrix=read<u32>(0x1018C7B0);
    flags=read<u16>(data,4); matrices=read<u32>(data,0x10);
    write<u16>(data,4,flags|0x10);
    f32 values[12]; for(u32 i=0;i<12;++i) values[i]=read<f32>(matrix,i*4);
    const u32 order[12]={8,6,9,5,11,0,7,4,2,1,3,10};
    for(u32 i:order) write<f32>(matrices,0x450+i*4,values[i]);
    call(0x028E90D4,read<u32>(0x1018C7B0),0x104B4868u);
    return 1;
}
VERIFY(0x02449C60,pw_node_callback);

void pw_float_motion(pw_class* actor) {
    WWHD_FUNC(0x0244AA98,void,actor);
    u32 a=ea(actor),play=call<u32>(0x025200D4);
    f32 ground=read<f32>(a,0x5AC);
    s16 angle=(s16)(read<s16>(a,0x4AC)+700);
    f32 height=read<f32>(a,0x410); u32 player=read<u32>(play,0x5B2C);
    write<s16>(a,0x4AC,angle);
    if(ground!=-1000000000.f) {
        play=call<u32>(0x025200D4);
        if(call<s32>(0x02008254,play+0x12A0,a+0x600)) {
            play=call<u32>(0x025200D4);
            if(call<s32>(0x024EF0BC,play+0x12A0,a+0x600)==4) {
                if(read<s16>(a,0x482)==1) height=read<f32>(player,0x318);
                write<f32>(a,0x318,add(height,30.f)); return;
            }
        }
        angle=read<s16>(a,0x4AC);
    }
    u8 airborne=read<u8>(a,0x455); s16 mode=read<s16>(a,0x484);
    if(!airborne) height=read<f32>(a,0x5AC);
    if(mode==0x21 || mode==0x28) height=read<f32>(player,0x318);
    f32 sine=read<f32>(0x104A44F8,((u16)angle>>3)*8);
    call(0x0200ED84,a+0x318,fmadds(sine,30.f,add(height,30.f)),1.f,3.f);
    if(read<s16>(a,0x484)==0x26) {
        u16 phase=(u16)(read<s16>(a,0x4AE)+1000);
        f32 z=read<f32>(a,0x444),x=read<f32>(a,0x43C);
        write<u16>(a,0x4AE,phase);
        f32 sx=read<f32>(0x104A44F8,(phase>>3)*8),sz=read<f32>(0x104A44F8,(phase>>3)*8);
        f32 targetX=fmadds(sx,50.f,add(x,50.f)),targetZ=fmadds(sz,50.f,add(z,50.f));
        call(0x0200ED84,a+0x314,targetX,1.f,5.f);
        call(0x0200ED84,a+0x31C,targetZ,1.f,5.f);
    }
}
VERIFY(0x0244AA98,pw_float_motion);

void pw_adjust_distance(pw_class* actor) {
    WWHD_FUNC(0x0244AE40,void,actor);
    u32 a=ea(actor),play=call<u32>(0x025200D4),player=read<u32>(play,0x5B2C);
    play=call<u32>(0x025200D4);
    if(call<f32>(0x025D68EC,a,read<u32>(play,0x5B2C))>300.f) return;
    u32 matrix=read<u32>(0x1018C7B0); play=call<u32>(0x025200D4);
    s32 angle=call<s32>(0x025D6894,a,read<u32>(play,0x5B2C));
    call(0x025F1884,matrix,angle);
    struct Vec { be<f32> x,y,z; };
    Local<Vec> offset,near,far;
    offset->x=0.f; offset->y=0.f; offset->z=-300.f;
    call(0x0200FCD8,offset.get(),near.get());
    call(0x028E8D88,near.get(),player+0x314,near.get());
    play=call<u32>(0x025200D4);
    if(call<s32>(0x0252A038,play+0x5A20,near.get())) return;
    offset->x=0.f; offset->y=0.f; offset->z=-400.f;
    call(0x0200FCD8,offset.get(),far.get());
    call(0x028E8D88,far.get(),player+0x314,far.get());
    play=call<u32>(0x025200D4);
    if(call<s32>(0x0252A038,play+0x5A20,far.get())) return;
    f32 speed=__builtin_fabsf(read<f32>(player,0x370));
    f32 volume=(f32)((double)speed*4.f);
    u32 level=!(volume<2147483648.f)?(u32)ftoi(sub(volume,2147483648.f))+0x80000000u:(u32)ftoi(volume);
    if(level>100) level=100;
    if(a && a+0x37C) {
        s32 reverb=call<s32>(0x02520540,(s32)read<s8>(a,0x326));
        call(0x025E1A40,0x7051u,a+0x37C,level,reverb);
    }
    f32 step=(f32)((double)speed*1.5f);
    call(0x0200ED84,a+0x314,(f32)near->x,1.f,step);
    call(0x0200ED84,a+0x31C,(f32)near->z,1.f,step);
}
VERIFY(0x0244AE40,pw_adjust_distance);

s32 pw_line_check(pw_class* actor,u32 target,s32 mode) {
    WWHD_FUNC(0x0244A7F0,s32,actor,target,mode);
    u32 a=ea(actor); struct Check { u8 storage[0x6C]; }; Local<Check> line;
    u32 l=ea(line.get()); call(0x02008FEC,line.get());
    write<u32>(l,0x10,0x100383DC); write<u8>(l,0x5D,0);
    f32 x=read<f32>(target),y=read<f32>(target,4),z=read<f32>(target,8);
    write<f32>(a,0x430,x); write<f32>(a,0x434,y);
    write<u32>(l,4,l+0x64); write<u8>(l,0x5F,0); write<u32>(l,0x20,0x100383EC);
    write<u8>(l,0x60,0); write<f32>(a,0x438,z); write<u8>(l,0x61,0);
    write<u8>(l,0x5C,0); write<u8>(l,0x62,0); write<u32>(l,0x68,1);
    write<u32>(l,0,l+0x58); write<u8>(l,0x5E,0);
    write<u32>(l,0x58,0x1003840C); write<u32>(l,0x64,0x100383FC);
    struct Vec { be<f32> x,y,z; }; Local<Vec> offset,sum;
    if(!mode) {
        call(0x025F1884,read<u32>(0x1018C7B0),(s32)read<s16>(a,0x322));
        offset->x=0.f; offset->y=0.f; offset->z=200.f;
        call(0x0200FCD8,offset.get(),target);
        call(0x0201AD78,target,sum.get(),a+0x314);
        x=sum->x; y=sum->y; write<f32>(a,0x430,x);
        z=sum->z; write<f32>(a,0x434,y); write<f32>(a,0x438,z);
    }
    y=add(y,80.f); x=read<f32>(a,0x430); write<f32>(a,0x434,y);
    write<f32>(target,0,x); write<f32>(target,4,y);
    z=read<f32>(a,0x438); write<f32>(target,8,z);
    write<u32>(a,0x424,read<u32>(a,0x314));
    y=read<f32>(a,0x318); u32 zbits=read<u32>(a,0x31C);
    write<u32>(a,0x42C,zbits); write<f32>(a,0x428,add(y,80.f));
    call(0x024F1AFC,line.get(),a+0x424,target,a);
    u32 play=call<u32>(0x025200D4);
    bool hit=call<s32>(0x02008860,play+0x12A0,line.get())!=0;
    if(hit) {
        s16 yaw=read<s16>(a,0x322);
        write<u32>(l,0x20,0x1003838C); write<u32>(l,0x64,0x1003839C);
        write<u32>(l,0x58,0x1003840C); write<u16>(a,0x4A8,(u16)(yaw+0x8000));
    } else {
        write<u32>(l,0x58,0x1003840C); write<u32>(l,0x20,0x1003838C); write<u32>(l,0x64,0x1003839C);
    }
    call(0x02008B4C,line.get(),0); return hit;
}
VERIFY(0x0244A7F0,pw_line_check);

void pw_lantern_motion(pw_class* actor) {
    WWHD_FUNC(0x0244F730,void,actor);
    u32 a=ea(actor);
    if(read<s16>(a,0x462)<=0 || read<s16>(a,0x49E)!=0) return;
    u32 id=read<u32>(a,0x48C); if(id==0xFFFFFFFF) return;
    Local<be<u32>> found;
    if(!call<s32>(0x025D54C4,id,found.get())) return;
    u32 lantern=load<u32>(ea(found.get()));
    if(!lantern || read<s16>(lantern,8)!=0xC1) return;
    write<u32>(lantern,0x314,read<u32>(a,0x3E8));
    write<u32>(lantern,0x318,read<u32>(a,0x3EC));
    write<u32>(lantern,0x31C,read<u32>(a,0x3F0));
    write<s16>(load<u32>(ea(found.get())),0x322,read<s16>(a,0x32A));
    u8 mode=read<u8>(a,0x45D);
    if(mode==0 || mode==1) {
        call(0x0200F428,a+0x4B2,mode==0?3000:4000,1,100);
        call(0x0200ED84,a+0x4CC,mode==0?5000.f:10000.f,1.f,1000.f);
        call(0x0200ED84,a+0x4D4,mode==0?-1000.f:-10000.f,1.f,1000.f);
        mode=read<u8>(a,0x45D);
    }
    if(mode!=2) {
        u16 angle=(u16)(read<s16>(a,0x4B0)+read<s16>(a,0x4B2));
        f32 amplitude=read<f32>(a,0x4CC); write<u16>(a,0x4B0,angle);
        f32 sine=read<f32>(0x104A44F8,(angle>>3)*8);
        write<s16>(lantern,0x3DA,(s16)ftoi((f32)((double)sine*amplitude)));
        angle=read<u16>(a,0x4B0);
        f32 cosine=read<f32>(0x104A44F8,(angle>>3)*8+4),depth=read<f32>(a,0x4D4);
        write<s16>(lantern,0x3DE,(s16)ftoi((f32)((double)cosine*depth)));
        write<u32>(load<u32>(ea(found.get())),0x39C,0);
    }
    write<u32>(a,0x3F4,read<u32>(lantern,0x3F4));
    write<u32>(a,0x3F8,read<u32>(lantern,0x3F8));
    write<u32>(a,0x3FC,read<u32>(lantern,0x3FC));
}
VERIFY(0x0244F730,pw_lantern_motion);

s32 pw_heap_initialize(pw_class* actor) {
    WWHD_FUNC(0x0244DE14,s32,actor);
    u32 a=ea(actor); struct String { be<u32> data,vtable; };
    Local<String> archive[6];
    auto resource=[&](u32 slot,s32 index) {
        archive[slot]->data=0x10038517; archive[slot]->vtable=0x10038364;
        return call<u32>(0x026066C4,read<u32>(0x101F4F28),archive[slot].get(),index);
    };
    u32 modelData=resource(0,0x27),animation=resource(1,0x23);
    u32 morf=call<u32>(0x025E4F64,0u,modelData,0u,0u,animation,2,1.f,0,-1,1,0u,0x80000u,0x37441422u);
    write<u32>(a,0x3D0,morf); if(!morf || !read<u32>(morf,0x90)) return 0;
    write<u32>(read<u32>(morf,0x90),0xB8,a);
    u32 info=call<u32>(0x027F3F94,read<u32>(read<u32>(read<u32>(a,0x3D0),0x90),0xAC));
    u16 index=0; morf=read<u32>(a,0x3D0);
    while(index<read<u16>(info,8)) {
        u32 table=read<u32>(read<u32>(morf,0x90),0xAC);
        u32 count=read<u32>(table,4),entry=read<u32>(table,8);
        if(index<count) entry+=index*0x1C;
        write<u32>(entry,8,0x02449C60);
        morf=read<u32>(a,0x3D0); index=(u16)(index+1);
        info=call<u32>(0x027F3F94,read<u32>(read<u32>(morf,0x90),0xAC));
        morf=read<u32>(a,0x3D0);
    }
    u32 model=read<u32>(morf,0x90),btk=call<u32>(0x0273AD10,0x74);
    if(btk) btk=call<u32>(0x025E7820,btk);
    write<u32>(a,0x3D8,btk); if(!btk) return 0;
    u32 currentModel=read<u32>(read<u32>(a,0x3D0),0x90),btkData=resource(2,0x2F);
    if(!call<s32>(0x025E789C,read<u32>(a,0x3D8),read<u32>(currentModel,0xAC),btkData,1,1,1.f,0,-1,0,0)) return 0;
    const u32 fields[3]={0x3DC,0x3E0,0x3E4};
    const s32 resources[3]={0x2C,0x2B,0x2A};
    for(u32 i=0;i<3;++i) {
        u32 brk=call<u32>(0x0273AD10,0x78);
        if(brk) brk=call<u32>(0x025E80D0,brk);
        write<u32>(a,fields[i],brk); if(!brk) return 0;
        u32 data=resource(i+3,resources[i]);
        if(!call<s32>(0x025E8154,read<u32>(a,fields[i]),read<u32>(model,0xAC),data,1,i?2:0,1.f,0,-1,0,0)) return 0;
    }
    return call<s32>(0x025E8A48,a+0xF84,read<u32>(read<u32>(a,0x3D0),0x90))!=0;
}
VERIFY(0x0244DE14,pw_heap_initialize);

void pw_possession_action(pw_class* actor) {
    WWHD_FUNC(0x0244FE24,void,actor);
    u32 a=ea(actor),play=call<u32>(0x025200D4),player=read<u32>(play,0x5B2C);
    play=call<u32>(0x025200D4); s16 mode=read<s16>(a,0x484);
    f32 playerZ=read<f32>(player,0x31C); u32 controlled=read<u32>(play,0x5B34);
    f32 playerX=read<f32>(player,0x314);
    if(mode!=0x50 && mode!=0x51) { call(0x0200F428,a+0x4B6,255,1,10); return; }
    auto sound=[&](u32 id) {
        s32 reverb=call<s32>(0x02520540,(s32)read<s8>(a,0x326));
        call(0x025E1A40,id,a+0x37C,0,reverb);
    };
    if(mode==0x50) {
        for(u32 off=0x4A0;off<=0x4A6;off+=2) write<s16>(a,off,0);
        u32 flags=read<u32>(a,0x2E0); mode=read<s16>(a,0x484);
        write<s16>(a,0x494,300); write<u8>(a,0x45A,1);
        write<u32>(a,0x2E0,flags|0x4000); write<s16>(a,0x484,(s16)(mode+1));
        if(a+0x37C) sound(0x107F);
    } else if(a && a+0x37C) sound(0x107F);
    write<s16>(a,0x4A0,(s16)(read<s16>(a,0x4A0)+700));
    write<s16>(a,0x32A,(s16)(read<s16>(a,0x32A)+1000));
    bool leave=true;
    if(player==controlled && read<s16>(a,0x494)!=0) {
        play=call<u32>(0x025200D4);
        if(!call<s32>(0x0252A038,play+0x5A20,a+0x314)) {
            play=call<u32>(0x025200D4);
            if(!(read<u32>(play,0x5CDC)&0x2000) && read<s16>(controlled,0x3B0)==0 && !(read<u32>(controlled,0x3C0)&0x2000000)) leave=false;
        }
    }
    bool hit=false;
    if(call<s32>(0x025162A4,a+0x744)) {
        u32 object=call<u32>(0x02516300,a+0x744);
        if(object && (read<u32>(object,0x10)&0x800000)) hit=true;
    }
    if(hit || leave) {
        if(read<u8>(a,0x45F)) {
            write<u8>(a,0x45F,0); write<u8>(0x101CF0A4,0,0);
            if(a+0x37C) sound(0x5933);
        }
        write<s16>(a,0x4AA,0); write<s16>(a,0x4B6,255); write<s16>(a,0x4A6,1);
        write<u32>(controlled,0x3BC,read<u32>(controlled,0x3BC)&~0x100u);
        write<s16>(a,0x484,0x3C); write<s16>(a,0x482,2);
    } else {
        write<u32>(controlled,0x3BC,read<u32>(controlled,0x3BC)|0x100);
        u16 phase=read<u16>(a,0x4A0); f32 y=read<f32>(player,0x318);
        f32 sine=read<f32>(0x104A44F8,(phase>>3)*8);
        write<f32>(a,0x31C,playerZ); write<f32>(a,0x314,playerX);
        write<f32>(a,0x318,fmadds(sine,10.f,add(y,100.f)));
    }
    call(0x0200F428,a+0x4B6,255,1,10);
}
VERIFY(0x0244FE24,pw_possession_action);

s32 pw_draw(pw_class* actor) {
    WWHD_FUNC(0x02449F80,s32,actor);
    u32 a=ea(actor),model=read<u32>(read<u32>(a,0x3D0),0x90),data=read<u32>(model,0xAC);
    u32 lighting=call<u32>(0x02555D0C); call(0x02562F5C,lighting,model,a+0x110);
    call(0x025BED80,0xB8,a,1.f,1.f,1.f);
    u32 info=call<u32>(0x027F3F8C,data); u16 index=0;
    struct Color { be<f32> r,g,b,a; }; Local<Color> linear,converted;
    while(index<read<u16>(info,0x24)) {
        u32 count=read<u32>(data,0xC),material=read<u32>(data,0x10);
        if(index<count) material+=index*0x39C;
        u32 object=read<u32>(material,0x18);
        u32 color=call_ptr<u32>(read<u32>(read<u32>(object,4),0x4C),object,3);
        write<u8>(color,3,(u8)read<s16>(a,0x4B6));
        object=read<u32>(material,0x18);
        color=call_ptr<u32>(read<u32>(read<u32>(object,4),0x4C),object,3);
        object=read<u32>(material,0x18);
        call_ptr(read<u32>(read<u32>(object,4),0x3C),object,3,color);
        call(0x02449ECC,linear.get(),color);
        call(0x0274D458,converted.get(),linear.get(),1.f);
        write<u32>(material,0xA0,read<u32>(material,0xA0)|0x400);
        u32 output=call<u32>(0x027F9F0C,material+0xA0,10);
        f32 alpha=(f32)((double)(f32)read<u8>(color,3)/255.f);
        f32 b=converted->b,g=converted->g,r=converted->r;
        write<f32>(output,4,g); write<f32>(output,8,b); write<f32>(output,0,r); write<f32>(output,12,alpha);
        index=(u16)(index+1); info=call<u32>(0x027F3F8C,data);
    }
    constexpr u32 drawState=0x104B4634;
    u32 play;
    if(!read<u8>(a,0x45A)) {
        play=call<u32>(0x025200D4); write<u32>(drawState,0,read<u32>(play,0x5D84));
        play=call<u32>(0x025200D4); write<u32>(drawState,4,read<u32>(play,0x5D88));
        call(0x027F58E0,model,1);
    } else call(0x027F58E0,model,0);
    if(read<s16>(a,0x9AE)>20) {
        call(0x0259138C,read<u32>(a,0x3D0),-1,a+0xF84);
        play=call<u32>(0x025200D4); write<u32>(drawState,0,read<u32>(play,0x5D78));
        play=call<u32>(0x025200D4); write<u32>(drawState,4,read<u32>(play,0x5D7C)); return 1;
    }
    call<u32>(0x025200D4);
    u8 visible=read<u8>(a,0x45A); data=read<u32>(model,0xAC);
    u32 brk=read<u32>(a,visible?0x3DC:(read<u8>(a,0x45B)?0x3E0:0x3E4));
    call(0x025E83FC,brk,data,read<f32>(brk,4));
    u32 btk=read<u32>(a,0x3D8); s16 frame=(s16)ftoi(read<f32>(btk,4));
    call(0x025E7B3C,btk,read<u32>(model,0xAC),(s32)frame);
    u8 texture=read<u8>(a,0x457); write<f32>(read<u32>(a,0x3D8),4,(f32)texture);
    call(0x025E5590,read<u32>(a,0x3D0));
    read<u8>(a,0x45A); write<u32>(read<u32>(model,0xAC),0x48,0);
    write<u32>(read<u32>(model,0xAC),0x38,0);
    play=call<u32>(0x025200D4); write<u32>(drawState,0,read<u32>(play,0x5D78));
    play=call<u32>(0x025200D4); write<u32>(drawState,4,read<u32>(play,0x5D7C));
    call(read<u8>(a,0x45A)?0x025E8EC0:0x025E8CD8,a+0xF84); return 1;
}
VERIFY(0x02449F80,pw_draw);

s32 pw_create(pw_class* actor) {
    WWHD_FUNC(0x0244E398,s32,actor);
    u32 a=ea(actor),flags=read<u32>(a,0x2E4);
    if(!(flags&8)) {
        if(a) { call(0x0244E210,a); flags=read<u32>(a,0x2E4); }
        write<u32>(a,0x2E4,flags|8);
    }
    s32 phase=call<s32>(0x02520460,a+0x3C8,0x10038528u);
    if(phase!=4) return phase;
    u32 parameters=read<u32>(a,0xB0);
    write<u8>(a,0x454,(u8)parameters); write<u8>(a,0x455,(parameters>>8)&1);
    write<u8>(a,0x456,(parameters>>16)&255); write<u8>(a,0x457,(parameters>>9)&127);
    write<u8>(a,0x475,(u8)(parameters>>24));
    if((u8)parameters==255) write<u8>(a,0x454,0);
    u8 radius=(parameters>>16)&255;
    write<f32>(a,0x4C4,radius==255?1000.f:(f32)((double)(f32)radius*10.f));
    if(read<u8>(a,0x457)==127) write<u8>(a,0x457,0);
    if(!call<s32>(0x025D63E8,a,0x0244DE14u,0x2540)) return 5;
    write<u8>(a,0x2DE,13); write<f32>(a,0x4C8,add(read<f32>(a,0x4C4),5000.f));
    s16 overrideType=read<s16>(0x1047BB1A); u8 path=read<u8>(a,0x475);
    if(overrideType) write<u8>(a,0x454,(u8)(overrideType-1));
    if(path!=255) write<u32>(a,0x464,call<u32>(0x025AAF88,(u32)path,(s32)read<s8>(a,0x326)));
    u8 texture=read<u8>(a,0x457); u32 x=read<u32>(a,0x314),y=read<u32>(a,0x318);
    if(texture>5) write<u8>(a,0x457,0);
    write<u32>(a,0x3E8,x); u32 z=read<u32>(a,0x31C);
    write<u32>(a,0x3EC,y); write<u32>(a,0x3F0,z);
    u32 play=call<u32>(0x025200D4);
    write<u32>(a,0x3A4,call<u32>(0x0200E814,play+0x50AC,0x10038524u,0));
    x=read<u32>(a,0x314); write<u8>(a,0x3A0,4); write<u8>(a,0x3A1,4);
    y=read<u32>(a,0x318); write<u32>(a,0x40C,x); write<u32>(a,0x410,y);
    write<s16>(a,0x4B4,4); z=read<u32>(a,0x31C);
    u32 morf=read<u32>(a,0x3D0); write<u32>(a,0x414,z); write<s16>(a,0x4BA,0x1000);
    u32 model=read<u32>(morf,0x90); if(model) model+=0xC8;
    write<u32>(a,0x348,model); call(0x025D674C,a,-100.f,-50.f,-50.f,100.f,200.f,100.f);
    write<u32>(a,0x39C,4);
    call(0x024F06B4,a+0x518,a+0x314,a+0x300,a,1,a+0x4D8,a+0x33C,0,0);
    call(0x02515F14,a+0x708,0,1,a);
    write<u32>(a,0x9A0,a); write<f32>(a,0xB40,50.f);
    write<u32>(a,0xD64,read<u32>(a,0x3D0)); write<u32>(a,0xD58,a); write<f32>(a,0xB3C,200.f);
    for(u32 i=0;i<10;++i) {
        write<u8>(a,0xD68+i,read<u8>(0x101CF130,i));
        write<f32>(a,0xD74+i*4,read<f32>(0x101CF108,i*4));
    }
    call(0x02516518,a+0x744,0x101CF13Cu); write<u32>(a,0x788,a+0x708);
    call(0x0251677C,a+0x874,0x101CF0C8u);
    x=read<u32>(a,0x314); s16 yaw=read<s16>(a,0x322);
    write<u32>(a,0x488,0xFFFFFFFF); write<u32>(a,0x48C,0xFFFFFFFF);
    write<u32>(a,0x8B8,a+0x708); write<s16>(a,0x4A8,yaw);
    u32 sphere=read<u32>(a,0x874); write<u8>(a,0x3A9,1); write<s16>(a,0x482,0);
    write<u32>(a,0x3F4,x); u32 cylinder=read<u32>(a,0x7D8); u8 type=read<u8>(a,0x454);
    write<s16>(a,0x484,10); y=read<u32>(a,0x318); write<s16>(a,0x462,-1);
    write<u32>(a,0x3F8,y); write<u32>(a,0x874,sphere&~1u);
    z=read<u32>(a,0x31C); write<u32>(a,0x7D8,cylinder&~2u); write<u32>(a,0x3FC,z);
    if(type==1 || type==2) {
        u32 status=read<u32>(a,0x2E0); write<u32>(a,0x39C,0);
        write<s16>(a,0x484,type==1?0:9); write<u32>(a,0x2E0,status&~0x20u);
    } else if(type==3 || type==4) {
        u32 parent=read<u32>(a,0x2E8); write<u32>(a,0x488,parent); if(parent==0xFFFFFFFF) return 5;
        cylinder=read<u32>(a,0x7D8); type=read<u8>(a,0x454);
        write<s16>(a,0x4B6,255); write<s16>(a,0x4AA,0); write<u8>(a,0x3A0,4);
        write<u32>(a,0x76C,0xFF3DFEFF); write<u32>(a,0x7D8,cylinder|2);
        write<s16>(a,0x462,0); write<u8>(a,0x3A1,4);
        if(type==3) { write<s16>(a,0x482,0); write<s16>(a,0x484,0x6E); write<u8>(a,0x45A,1); }
        else {
            write<u32>(a,0x2E0,read<u32>(a,0x2E0)|0x4000);
            write<s16>(a,0x4AC,(s16)ftoi(call<f32>(0x02019918,16384.f)));
            f32 random=call<f32>(0x02019918,150.f); write<f32>(a,0x314,add(read<f32>(a,0x314),random));
            random=call<f32>(0x02019918,150.f); f32 currentZ=read<f32>(a,0x31C);
            write<s16>(a,0x482,5); write<s16>(a,0x484,150); write<f32>(a,0x31C,add(currentZ,random));
        }
    }
    call(0x0244A768,a); call(0x02449DA8,a); return phase;
}
VERIFY(0x0244E398,pw_create);

void pw_demo_action(pw_class* actor) {
    WWHD_FUNC(0x0244F960,void,actor);
    u32 a=ea(actor),play=call<u32>(0x025200D4),player=read<u32>(play,0x5B2C);
    play=call<u32>(0x025200D4); s32 cameraIndex=read<s8>(play,0x5B30);
    play=call<u32>(0x025200D4);
    f32 playerY=read<f32>(player,0x318); s16 mode=read<s16>(a,0x484);
    f32 playerX=read<f32>(player,0x314),playerZ=read<f32>(player,0x31C);
    u32 camera=read<u32>(play+(u32)(cameraIndex*0x34),0x5AF8);
    if((u32)(s32)mode<0x46u || (u32)(s32)mode>0x49u) return;
    if(mode==0x46) {
        u32 flags=read<u32>(a,0x2E0); s16 current=read<s16>(a,0x484);
        write<u32>(a,0x2E0,flags|0x4000); write<s16>(a,0x484,(s16)(current+1));
        mode=0x47;
    }
    if(mode==0x47) {
        if(read<u16>(a,0xF8)!=2) {
            play=call<u32>(0x025200D4); write<u16>(play,0x52B8,read<u16>(play,0x52B8)|1);
            call(0x025D7B24,a,2,65535,0); write<u16>(a,0xFA,read<u16>(a,0xFA)|2); return;
        }
        write<u32>(player,0x428,0); write<s16>(player,0x420,3); write<u32>(player,0x430,1);
        call(0x02514F2C,camera+0x248); call(0x02515280,camera+0x248,2);
        play=call<u32>(0x025200D4);
        write<s16>(a,0x4A8,call<s16>(0x025D6894,a,read<u32>(play,0x12A0+0x488C)));
        if(a+0x37C) {
            u32 id=a?read<u32>(a,4):0xFFFFFFFF;
            s32 reverb=call<s32>(0x02520540,(s32)read<s8>(a,0x326));
            call(0x025E1AA4,0x48FD,a+0x37C,id,0,reverb);
            reverb=call<s32>(0x02520540,(s32)read<s8>(a,0x326));
            call(0x025E1A40,0x5932,a+0x37C,0,reverb);
        }
        call(0x0244A37C,a,0x21,4.f,0,1.f,-1);
        write<s16>(a,0x484,(s16)(read<s16>(a,0x484)+1)); return;
    }
    if(mode==0x48) {
        call(0x0200ED84,a+0x314,playerX,1.f,5.f);
        call(0x0200ED84,a+0x31C,read<f32>(player,0x31C),1.f,5.f);
        u32 morf=read<u32>(a,0x3D0);
        if(!(read<u8>(morf,0xA7)&1) && read<f32>(morf,0x98)!=0.f) return;
        call(0x0244A37C,a,0x22,4.f,2,1.f,-1);
        write<s16>(a,0x32A,0); write<s16>(a,0x32C,0); write<f32>(a,0x31C,playerZ);
        write<u8>(a,0x45C,1); write<u32>(a,0x39C,0); write<f32>(a,0x318,add(playerY,100.f));
        write<f32>(a,0x314,playerX); write<s16>(a,0x328,0);
        s16 yaw=read<s16>(player,0x32A),current=read<s16>(a,0x484);
        write<s16>(a,0x32A,yaw); write<s16>(a,0x494,30); write<s16>(a,0x484,(s16)(current+1)); return;
    }
    if(read<s16>(a,0x494)!=0) return;
    call<u32>(0x025200D4); play=call<u32>(0x025200D4);
    call(0x025CB610,play+0x599C,32); call(0x02514F38,camera+0x248); call(0x02515280,camera+0x248,0);
    write<s16>(player,0x420,2); write<u32>(player,0x430,1);
    play=call<u32>(0x025200D4); write<u16>(play,0x52B8,read<u16>(play,0x52B8)|8);
    u32 flags=read<u32>(a,0x2E0); write<s16>(a,0x484,0x50); write<s16>(a,0x482,4);
    write<u32>(a,0x2E0,flags&~0x4000u);
}
VERIFY(0x0244F960,pw_demo_action);

s32 pw_lantern_collision(pw_class* actor) {
    WWHD_FUNC(0x0244F28C,s32,actor);
    u32 a=ea(actor),play=call<u32>(0x025200D4); s16 held=read<s16>(a,0x462);
    u32 player=read<u32>(play,0x5B2C); if(held<=0) return 0;
    u32 id=read<u32>(a,0x48C); if(id==0xFFFFFFFF) return 0;
    Local<be<u32>> found; if(!call<s32>(0x025D54C4,id,found.get())) return 0;
    u32 lantern=load<u32>(ea(found.get())); if(!lantern || read<s16>(lantern,8)!=0xC1) return 0;
    s16 before=read<s16>(a,0x4B4); call(0x02515E50,a+0x724); write<u8>(a,0x459,0);
    auto sound=[&](u32 id) { if(a && a+0x37C) {
        s32 reverb=call<s32>(0x02520540,(s32)read<s8>(a,0x326)); call(0x025E1A40,id,a+0x37C,64,reverb);
    }};
    s16 health;
    if(!call<s32>(0x025162A4,a+0x874)) {
        health=read<s16>(a,0x4B4); write<u8>(a,0x45E,0);
    } else {
        if(read<u8>(a,0x45E)) return 0;
        u32 object=call<u32>(0x02516300,a+0x874); if(!object) return 0;
        write<u8>(a,0x45E,1); u32 type=read<u32>(object,0x10);
        if(type==2) {
            sound(0x2803); health=(s16)(read<s16>(a,0x4B4)-1); write<s16>(a,0x4B4,health);
            u8 attack=read<u8>(player,0x3AC);
            bool strong=(attack>=6 && attack<=10)||attack==12||(attack>=14 && attack<=16)||attack==21||attack==23;
            if(strong) { health=(s16)(read<s16>(a,0x4B4)-1); write<u8>(a,0x459,1); write<s16>(a,0x4B4,health); }
        } else if(type==0x200000) {
            health=read<s16>(a,0x4B4); write<u8>(a,0x459,3);
        } else if(type==0x40 || type==0x80) {
            if(type==0x40) write<u8>(a,0x459,4);
            health=(s16)(read<s16>(a,0x4B4)-1); write<s16>(a,0x4B4,health);
            sound(0x2833); health=read<s16>(a,0x4B4);
        } else if(type==0x10000) {
            sound(0x2855); write<u8>(a,0x459,7);
            if(read<u8>(player,0x3AC)==17) write<u8>(a,0x459,8);
            health=0; write<s16>(a,0x4B4,0);
        } else if(type==0x20) {
            health=(s16)(read<s16>(a,0x4B4)-2); write<u8>(a,0x459,6); write<s16>(a,0x4B4,health);
        } else {
            if(type==0x40000 || type==0x80000 || type==0x100000)
                write<s16>(a,0x4B4,(s16)(read<s16>(a,0x4B4)-2));
            health=(s16)(read<s16>(a,0x4B4)-1); write<u8>(a,0x459,0); write<s16>(a,0x4B4,health);
            sound(0x2834); health=read<s16>(a,0x4B4);
        }
    }
    if(before==health) return 0;
    if(health<=0) {
        struct Vec { be<f32> x,y,z; }; Local<Vec> position,scale;
        f32 x=read<f32>(a,0x940); write<s16>(a,0x4B4,0); position->x=x;
        position->y=read<f32>(a,0x944); position->z=read<f32>(a,0x948);
        play=call<u32>(0x025200D4);
        call(0x025A847C,read<u32>(play,0x5AB0),0,16,position.get(),0,0,255,0,-1,0,0,0);
        scale->x=1.f; scale->y=1.f; scale->z=1.f;
        play=call<u32>(0x025200D4);
        call(0x025A847C,read<u32>(play,0x5AB0),0,15,position.get(),player+0x328,scale.get(),255,0,-1,0,0,0);
    }
    write<s16>(a,0x482,2); write<s16>(a,0x484,50); return 1;
}
VERIFY(0x0244F28C,pw_lantern_collision);

s32 pw_body_collision(pw_class* actor) {
    WWHD_FUNC(0x0244E960,s32,actor);
    u32 a=ea(actor),play=call<u32>(0x025200D4),player=read<u32>(play,0x5B2C);
    call(0x02515E50,a+0x724); s8 health=read<s8>(a,0x3A1); write<u8>(a,0x458,0);
    if(health<=0 || read<s16>(a,0x482)==5) return 0;
    play=call<u32>(0x025200D4);
    if(call<s32>(0x0252A038,play+0x5A20,a+0x314)) {
        write<u8>(a,0x458,9);
        if(read<s16>(a,0x4B6)!=255 && !read<s16>(a,0x49E) && read<s16>(a,0x462)!=-1) {
            s16 mode=read<s16>(a,0x484);
            if(mode!=0x37 && mode!=0x3C && mode!=0x3D && read<u32>(a,0x488)==0xFFFFFFFF) {
                write<s16>(a,0x482,2); write<s16>(a,0x484,0x34); return 1;
            }
        }
    }
    if(!call<s32>(0x025162A4,a+0x744)) return 0;
    u32 object=call<u32>(0x02516300,a+0x744); if(!object) return 0;
    struct Vec { be<f32> x,y,z; }; Local<Vec> position,scale;
    f32 x=read<f32>(a,0x810),z=read<f32>(a,0x818),y=read<f32>(a,0x814);
    position->z=z; position->y=y; position->x=x;
    struct Attack { u8 storage[0x1C]; }; Local<Attack> attack; u32 at=ea(attack.get());
    write<u32>(at,0x14,0); write<u32>(at,0,call<u32>(0x02516300,a+0x744));
    u8 visible=read<u8>(a,0x45A); u32 type=read<u32>(object,0x10);
    auto sound=[&](u32 id) { if(a+0x37C) {
        s32 reverb=call<s32>(0x02520540,(s32)read<s8>(a,0x326)); call(0x025E1A40,id,a+0x37C,32,reverb);
    }};
    auto particle=[&](s32 id,u32 p,u32 rotation,u32 size) {
        u32 state=call<u32>(0x025200D4);
        call(0x025A847C,read<u32>(state,0x5AB0),0,id,p,rotation,size,255,0,-1,0,0,0);
    };
    auto finalSpecial=[&]() -> s32 {
        u8 kind=read<u8>(a,0x458);
        if(kind!=4 && kind!=12) return 0;
        write<s16>(a,0x482,2); write<s16>(a,0x484,0x3A); return 1;
    };
    if(!visible) {
        if(type&0x800000) write<u8>(a,0x458,9);
        else if(type&0x100000) {
            write<f32>(a,0xB4C,1.f); write<f32>(a,0x9A8,80.f); write<u8>(a,0x458,10);
            write<u32>(a,0x39C,0); write<u8>(a,0x9A6,1); write<s16>(a,0x4B6,255);
            call(0x0244A67C,a); call(0x0244A4A8,a);
        } else return finalSpecial();
    } else {
        switch(type) {
        case 0x800000: return finalSpecial();
        case 0x8000:
            sound(0x2834); write<u8>(a,0x458,11); write<s16>(a,0x484,27); return 0;
        case 0x200000:
            write<u8>(a,0x458,3); write<s16>(a,0x482,2); write<s16>(a,0x484,0x38); return 1;
        case 0x8000000:
            if(read<s8>(a,0x3A9)>0) {
                u8 previous=read<u8>(a,0x3A1); write<u8>(a,0x3A1,10);
                write<u32>(at,0,call<u32>(0x02516300,a+0x744)); call(0x025192A8,a,attack.get());
                write<u8>(a,0x3A1,previous);
            }
            particle(0x27B,a+0x390,0,0); write<u8>(a,0x458,12); sound(0x2834); return finalSpecial();
        case 0x40:
            write<u8>(a,0x458,4); particle(0x27B,a+0x390,0,0); sound(0x2833); return finalSpecial();
        case 0x80: case 0x1000000: sound(0x2833); break;
        case 2: case 0x400: case 0x800: case 0x4000000: case 0x10000000: {
            sound(0x2803); u8 move=read<u8>(player,0x3AC);
            bool strong=(move>=5 && move<=10)||move==12||(move>=14 && move<=16)||move==21||move==23||(move>=25 && move<=27)||(move>=30 && move<=31);
            if(strong) write<u8>(a,0x458,1); break;
        }
        case 0x10000:
            sound(0x2855); write<u8>(a,0x458,7);
            if(read<u8>(player,0x3AC)==17) write<u8>(a,0x458,8); break;
        case 0x20: write<u8>(a,0x458,6); break;
        case 0x80000:
            write<s16>(a,0x9A4,200); call(0x02041C30,a+0xD58); write<u8>(a,0x458,5); sound(0x2834);
            write<u8>(a,0x3A1,0); write<u32>(a,0x39C,0);
            if(call<s32>(0x0244A67C,a)) return finalSpecial();
            write<s16>(a,0x482,2); write<s16>(a,0x484,0x3C); return 1;
        case 0x100000:
            write<f32>(a,0x9A8,80.f); write<u8>(a,0x9A6,1); write<u8>(a,0x458,5);
            write<u32>(a,0x39C,0); write<f32>(a,0xB4C,1.f); sound(0x2834); call(0x0244A67C,a); return finalSpecial();
        case 0x200: case 0x40000:
            write<s16>(a,0xD5C,100); write<u8>(a,0x458,0); sound(0x2834); break;
        default: write<u8>(a,0x458,0); sound(0x2834); break;
        }
    }
    if(!read<u8>(a,0x45A)) {
        if(read<s16>(a,0x484)!=0x35) { write<s16>(a,0x482,2); write<s16>(a,0x484,0x34); }
        return 1;
    }
    call(0x025192A8,a,attack.get()); u8 kind=read<u8>(a,0x458);
    if(kind==1 || kind==7 || kind==8 || read<s8>(a,0x3A1)<=0) {
        if(call<s32>(0x0244A67C,a)) return 0;
        particle(16,ea(position.get()),0,0); scale->x=2.f; scale->y=2.f; scale->z=2.f;
        particle(15,ea(position.get()),player+0x328,ea(scale.get()));
        if(read<u8>(a,0x458)==7) {
            write<s16>(a,0x482,2); write<f32>(a,0x374,-3.f); write<f32>(a,0x370,0.f);
            write<s16>(a,0x484,0x3E); call(0x0244A37C,a,0x1C,3.f,0,1.f,-1); return 1;
        }
    } else particle(13,ea(position.get()),player+0x328,0);
    if(read<s16>(a,0x484)!=0x37) { write<s16>(a,0x482,2); write<s16>(a,0x484,0x36); }
    return 1;
}
VERIFY(0x0244E960,pw_body_collision);

void pw_big_demo_action(pw_class* actor) {
    WWHD_FUNC(0x024500C0,void,actor);
    u32 a=ea(actor); call<u32>(0x025200D4);
    Local<be<u32>> id,found; u32 parentId=read<u32>(a,0x488),parent=0;
    *id.get()=parentId;
    if(parentId!=0xFFFFFFFF) { parent=call<u32>(0x025D5218,0x025E1234u,id.get()); parentId=read<u32>(a,0x488); }
    *found.get()=parent;
    s32 searched=call<s32>(0x025D54C4,parentId,found.get()); parent=load<u32>(ea(found.get()));
    if(!parent || (searched && read<s16>(parent,8)!=0xD3)) return;
    s16 mode=read<s16>(a,0x484); constexpr u32 parameters=0x1047B608;
    auto animation=[&](s32 index,f32 blend,u32 loop,s32 sound) { call(0x0244A37C,a,index,blend,loop,1.f,sound); };
    auto advance=[&]() { write<s16>(a,0x484,(s16)(read<s16>(a,0x484)+1)); };
    auto voice=[&](u32 id,bool pair) {
        if(!(a+0x37C)) return;
        u32 process=a?read<u32>(a,4):0xFFFFFFFF; s32 reverb=call<s32>(0x02520540,(s32)read<s8>(a,0x326));
        call(0x025E1AA4,id,a+0x37C,process,0,reverb);
        if(pair) { reverb=call<s32>(0x02520540,(s32)read<s8>(a,0x326)); call(0x025E1A40,id==0x48F4?0x5930:0x5932,a+0x37C,0,reverb); }
    };
    auto frameVoice=[&]() { if(call<s32>(0x027F2BF8,read<u32>(a,0x3D0)+0x98,0.f) && a) voice(0x48F1,false); };
    auto angleToParent=[&]() { return call<s32>(0x025D6894,a,load<u32>(ea(found.get()))); };
    if(mode==150) {
        for(u32 off=0x4A0;off<=0x4A6;off+=2) write<s16>(a,off,0);
        write<s16>(a,0x4B6,255); write<s16>(a,0x4AA,0);
        write<f32>(a,0x370,add(read<f32>(parameters,12),17.f)); animation(27,7.f,2,8); advance(); mode=151;
    }
    if(mode==151) {
        frameVoice();
        if(!read<s16>(a,0x49A)) {
            f32 random=call<f32>(0x02019918,16384.f); s16 yaw=read<s16>(a,0x4A8);
            write<s16>(a,0x4A8,(s16)(yaw+(s16)ftoi(random)));
            random=call<f32>(0x020198D8,15.f); write<s16>(a,0x49A,(s16)ftoi(add(random,15.f)));
        }
        if(!read<s16>(a,0x496)) {
            struct Vec { be<f32> x,y,z; }; Local<Vec> endpoint;
            endpoint->x=read<f32>(a,0x314); endpoint->y=read<f32>(a,0x318); endpoint->z=read<f32>(a,0x31C);
            bool turn=call<s32>(0x0244A7F0,a,endpoint.get(),0)!=0;
            if(!turn) {
                f32 homeZ=read<f32>(a,0x414),currentZ=read<f32>(a,0x31C),currentX=read<f32>(a,0x314);
                f32 dz=sub(homeZ,currentZ),dx=sub(read<f32>(a,0x40C),currentX);
                f32 length=call<f32>(0x028F4384,fmadds(dx,dx,(f32)((double)dz*dz)));
                if(length>add(read<f32>(parameters,0x530),1000.f)) {
                    write<s16>(a,0x4A8,call<s16>(0x020195B0,dx,dz)); turn=true;
                }
            }
            if(turn) {
                f32 random=call<f32>(0x020198D8,20.f); write<s16>(a,0x496,(s16)ftoi(add(random,20.f)));
                random=call<f32>(0x020198D8,20.f); write<s16>(a,0x49A,(s16)ftoi(add(random,20.f)));
            }
        }
        if(read<u8>(parent,0x506)) { f32 random=call<f32>(0x020198D8,7.f); animation(0x24,add(random,2.f),2,-1); advance(); }
        return;
    }
    if(mode==152) {
        call(0x0200EDC8,a+0x370,1.f,2.f);
        if(read<u8>(parent,0x506)==2) {
            u32 play=call<u32>(0x025200D4); s32 yaw=call<s32>(0x025D6894,a,read<u32>(play,0x12A0+0x488C));
            s16 current=read<s16>(a,0x484); write<s16>(a,0x4A8,(s16)yaw); write<s16>(a,0x484,(s16)(current+1));
        } return;
    }
    if(mode==153) {
        if(read<u8>(parent,0x506)==3) { f32 random=call<f32>(0x020198D8,7.f); animation(25,add(random,2.f),0,-1); advance(); } return;
    }
    if(mode==154) { if(read<u8>(parent,0x506)==4) { animation(27,7.f,2,8); advance(); } return; }
    if(mode==155) {
        frameVoice(); s16 alpha=(s16)(read<s16>(a,0x4B6)-3);
        write<f32>(a,0x370,add(read<f32>(parameters,0x534),10.f)); write<s16>(a,0x4B6,alpha<100?100:alpha);
        u8 stage=read<u8>(parent,0x506);
        if(stage==4) {
            s16 target=(s16)ftoi(add(read<f32>(parameters,0x538),2000.f));
            s16 step=(s16)ftoi(add(read<f32>(parameters,0x53C),100.f));
            call(0x0200F428,a+0x4BA,(s32)target,1,(s32)step);
            write<f32>(a,0x370,add(read<f32>(parameters,0x540),40.f));
            write<s16>(a,0x4A8,(s16)angleToParent()); return;
        }
        if(stage==5) {
            write<s16>(a,0x4BA,(s16)ftoi(add(read<f32>(parameters,0x544),1000.f)));
            s32 yaw=angleToParent(); s16 timer=read<s16>(a,0x4A0); write<s16>(a,0x4A8,(s16)(yaw-0x8000));
            if(!timer) write<s16>(a,0x4A0,(s16)ftoi(call<f32>(0x020198D8,add(read<f32>(parameters,0x54C),40.f)))); return;
        }
        if(stage==6) {
            s16 timer=read<s16>(a,0x4A0); if(timer>0) { write<s16>(a,0x4A0,(s16)(timer-1)); return; }
            animation(32,1.f,2,-1); voice(0x48F4,true);
            write<s16>(a,0x4BA,(s16)ftoi(add(read<f32>(parameters,0x548),2000.f)));
            write<s16>(a,0x4A8,(s16)angleToParent()); write<s16>(a,0x484,171);
        } return;
    }
    if(mode==170) {
        animation(32,4.f,2,-1); if(a) voice(0x48F4,true); write<u8>(a,0x45A,0);
        f32 speed=add(read<f32>(parameters,0x24),20.f); s16 current=read<s16>(a,0x484);
        write<f32>(a,0x370,speed); parent=load<u32>(ea(found.get())); write<s16>(a,0x484,(s16)(current+1)); mode=171;
    }
    if(mode==171) {
        s16 alpha=(s16)(read<s16>(a,0x4B6)-3); write<s16>(a,0x4B6,alpha<100?100:alpha); if(!parent) return;
        s32 yaw=call<s32>(0x025D6894,a,parent); parent=load<u32>(ea(found.get()));
        f32 az=read<f32>(a,0x31C); write<s16>(a,0x4A8,(s16)yaw);
        f32 pz=read<f32>(parent,0x31C),px=read<f32>(parent,0x314),ax=read<f32>(a,0x314);
        f32 dz=sub(pz,az),dx=sub(px,ax);
        f32 distance=call<f32>(0x028F4384,fmadds(dx,dx,(f32)((double)dz*dz)));
        if(!(distance<add(read<f32>(parameters,0x2C),200.f))) return;
        write<f32>(a,0x370,0.f); voice(0x48FD,true); animation(33,0.f,0,-1); advance(); return;
    }
    if(mode==172) {
        if(parent) {
            f32 height=add(add(read<f32>(parent,0x318),200.f),read<f32>(parameters,0x28));
            call(0x0200ED84,a+0x318,height,1.f,10.f);
            call(0x0200ED84,a+0x314,read<f32>(load<u32>(ea(found.get())),0x314),1.f,10.f);
            call(0x0200ED84,a+0x31C,read<f32>(load<u32>(ea(found.get())),0x31C),1.f,10.f);
        }
        u32 morf=read<u32>(a,0x3D0);
        if(!(read<u8>(morf,0xA7)&1) && read<f32>(morf,0x98)!=0.f) return;
        parent=load<u32>(ea(found.get()));
        if(parent) { s16 count=read<s16>(parent,0x57E); write<s16>(parent,0x580,1); write<s16>(parent,0x57E,(s16)(count+1)); }
        call(0x025D57E0,a);
    }
}
VERIFY(0x024500C0,pw_big_demo_action);

// Execute's ordinary movement states. External actor helpers retain their HD calls.
static void pw_idle_states(u32 a) {
    constexpr u32 parameters=0x1047B608;
    u32 play=call<u32>(0x025200D4),player=read<u32>(play,0x5B2C);
    play=call<u32>(0x025200D4); s16 mode=read<s16>(a,0x484); u32 camera=read<u32>(play,0x5AF8);
    struct Vec { be<f32> x,y,z; }; Local<Vec> vector;
    Local<be<u32>> found;
    auto animation=[&](s32 index,f32 blend,u32 loop,s32 sound=-1) { call(0x0244A37C,a,index,blend,loop,1.f,sound); };
    auto advance=[&]() { s16 next=(s16)(read<s16>(a,0x484)+1); write<s16>(a,0x484,next); return next; };
    auto finished=[&]() { u32 morf=read<u32>(a,0x3D0); return (read<u8>(morf,0xA7)&1) || read<f32>(morf,0x98)==0.f; };
    auto distance=[&]() { u32 p=call<u32>(0x025200D4); return call<f32>(0x025D68EC,a,read<u32>(p,0x5B2C)); };
    auto angle=[&](bool background) { u32 p=call<u32>(0x025200D4); return call<s32>(0x025D6894,a,read<u32>(p,background?0x12A0+0x488C:0x5B2C)); };
    auto randomTimer=[&](u32 off,f32 span) { f32 r=call<f32>(0x020198D8,span); write<s16>(a,off,(s16)ftoi(add(r,span))); };
    auto endpoint=[&]() { vector->x=read<f32>(a,0x314); vector->y=read<f32>(a,0x318); vector->z=read<f32>(a,0x31C); };
    auto particle=[&](u32 id,u32 position,u32 rotation,u32 callback) {
        u32 p=call<u32>(0x025200D4); return call<u32>(0x025A847C,read<u32>(p,0x5AB0),0,id,position,rotation,0,255,callback,-1,0,0,0);
    };
    switch(mode) {
    case 0: animation(35,7.f,2); write<s16>(a,0x462,0); advance(); goto done;
    case 1:
        if(!(distance()<500.f)) goto done;
        animation(16,3.f,0); { s32 yaw=angle(true); write<s16>(a,0x322,(s16)yaw); advance();
        write<s16>(a,0x4A8,(s16)yaw); write<f32>(a,0x4C0,-80.f); write<s16>(a,0x32A,(s16)yaw); write<u8>(a,0x45C,1); } goto done;
    case 2: {
        write<s16>(a,0x32A,(s16)(read<s16>(a,0x32A)+0x1000)); call(0x0200EDC8,a+0x4C0,1.f,3.f);
        s16 alpha=(s16)(read<s16>(a,0x4B6)+7); write<s16>(a,0x4B6,alpha>100?100:alpha);
        if(!call<s32>(0x027F2BF8,read<u32>(a,0x3D0)+0x98,25.f)) goto done;
        write<f32>(a,0x4C0,0.f); call(0x0244B3AC,a); write<s16>(a,0x4A8,(s16)angle(true));
        u32 emitter=particle(0x82EE,a+0x3E8,a+0x328,0);
        if(emitter) {
            u32 data=read<u32>(read<u32>(read<u32>(a,0x3D0),0x90),0x2C);
            u16 flags=read<u16>(data,4); u32 matrices=read<u32>(data,0x10); write<u16>(data,4,flags|0x10);
            call(0x028249B0,matrices+0x450,emitter+0x1F0,emitter+0x22C);
        }
        write<u32>(a,0x39C,0); write<s16>(a,0x484,10); return;
    }
    case 9: write<s16>(a,0x4B6,0); write<s16>(a,0x4B8,0); advance(); [[fallthrough]];
    case 10: {
        if(read<u8>(a,0x454)==1) write<s16>(a,0x4A8,(s16)angle(true));
        u32 id=call<u32>(0x025D5834,0xC1,0xFF000001u,a+0x3E8,(s32)read<s8>(a,0x326),0,0,-1,0);
        write<u32>(a,0x48C,id); if(id==0xFFFFFFFF) goto done;
        write<s16>(a,0x49E,5); u8 type=read<u8>(a,0x454);
        if(type==1) { write<s16>(a,0x484,11); return; }
        if(type==2) { write<u8>(a,0x45D,2); write<s16>(a,0x484,8); return; }
        write<s16>(a,0x462,1); write<s16>(a,0x484,13); goto done;
    }
    case 11: {
        call(0x02563F64,a+0x314,camera+0xDC,vector.get()); u32 id=read<u32>(a,0x48C);
        f32 x=read<f32>(a,0x3E8),vx=vector->x,z=read<f32>(a,0x3F0),vz=vector->z,y=read<f32>(a,0x3EC);
        write<f32>(a,0x6F4,fmadds(vx,150.f,x)); write<f32>(a,0x6F8,y); write<f32>(a,0x6FC,fmadds(vz,150.f,z));
        if(id==0xFFFFFFFF || !call<s32>(0x025D54C4,id,found.get())) goto done;
        u32 lantern=load<u32>(ea(found.get())); if(!lantern || read<s16>(lantern,8)!=0xC1) goto done;
        write<u32>(lantern,0x314,read<u32>(a,0x3E8)); write<u32>(lantern,0x318,read<u32>(a,0x3EC)); write<u32>(lantern,0x31C,read<u32>(a,0x3F0));
        write<s16>(load<u32>(ea(found.get())),0x322,read<s16>(a,0x32A));
        lantern=load<u32>(ea(found.get())); for(u32 off=0x330;off<=0x338;off+=4) write<f32>(lantern,off,1.f);
        if(!finished()) goto done;
        write<u8>(a,0x45D,0); write<u8>(a,0x454,0); call(0x0244B3AC,a);
        write<s16>(a,0x49E,3); write<s16>(a,0x462,1); write<s16>(a,0x484,13); goto done;
    }
    case 6: {
        s16 alpha=(s16)(read<s16>(a,0x4B6)+10); u32 morf=read<u32>(a,0x3D0); write<s16>(a,0x4B6,alpha>100?100:alpha);
        if(!(read<u8>(morf,0xA7)&1) && read<f32>(morf,0x98)!=0.f) goto done;
        write<u8>(a,0x454,0); write<s16>(a,0x462,1); call(0x0244B3AC,a); write<s16>(a,0x484,13); goto done;
    }
    case 7: {
        f32 d=distance(); if(!(d<read<f32>(a,0x4C4))) goto done;
        s32 yaw=angle(true); write<s16>(a,0x322,(s16)yaw); write<s16>(a,0x4A8,(s16)yaw);
        write<s16>(a,0x32A,read<s16>(a,0x4A8)); animation(18,3.f,0); write<s16>(a,0x484,6); return;
    }
    case 8: {
        write<s16>(a,0x49C,2); write<s16>(a,0x462,1); f32 d=distance(); if(!(d<read<f32>(a,0x4C8))) goto done;
        animation(17,3.f,2); write<s16>(a,0x484,7); return;
    }
    case 13: {
        for(u32 off=0x4A0;off<=0x4A6;off+=2) write<s16>(a,off,0);
        write<u8>(a,0x45C,0); write<u8>(a,0x45D,0); randomTimer(0x494,60.f);
        s16 held=read<s16>(a,0x462); s32 current=read<s32>(a,0x490),desired=held==1?35:36;
        if(current!=desired) animation(desired,7.f,2); advance(); [[fallthrough]];
    }
    case 14:
        call(0x0200EDC8,a+0x370,1.f,1.f); if(read<s16>(a,0x494)) goto done; advance(); [[fallthrough]];
    case 15: {
        randomTimer(0x494,120.f); s32 current=read<s32>(a,0x490),desired=read<s16>(a,0x462)==1?20:21;
        if(current!=desired) animation(desired,7.f,2);
        f32 r=call<f32>(0x02019918,16384.f); s16 yaw=read<s16>(a,0x4A8); u8 path=read<u8>(a,0x475);
        write<s16>(a,0x4A8,(s16)(yaw+(s16)ftoi(r)));
        write<s16>(a,0x484,path!=255&&read<u32>(a,0x464)?20:16); goto done;
    }
    case 16:
        write<f32>(a,0x370,5.f); call(0x0244B308,a); if(read<s16>(a,0x496)) goto done;
        endpoint(); if(call<s32>(0x0244A7F0,a,vector.get(),0)||call<s32>(0x0244B058,a)) { write<s16>(a,0x496,10); goto done; }
        if(!read<s16>(a,0x494)) write<s16>(a,0x484,15); goto done;
    case 20: {
        write<u32>(a,0x418,read<u32>(a,0x314)); write<u32>(a,0x41C,read<u32>(a,0x318)); write<u32>(a,0x420,read<u32>(a,0x31C));
        if(read<u8>(a,0x475)==255 || !read<u32>(a,0x464)) goto done;
        write<f32>(a,0x370,5.f); call(0x0244B308,a);
        u32 path=read<u32>(a,0x464); s32 index=read<s8>(a,0x474);
        f32 z=read<f32>(a,0x31C),x=read<f32>(a,0x314); u32 point=read<u32>(path,8)+(u32)(index*16);
        f32 dx=sub(read<f32>(point,4),x),dz=sub(read<f32>(point,12),z);
        s32 yaw=call<s32>(0x020195B0,dx,dz); write<s16>(a,0x4A8,(s16)yaw);
        if(call<f32>(0x028F4384,fmadds(dx,dx,(f32)((double)dz*dz)))<80.f) {
            s32 next=(s8)(read<u8>(a,0x474)+1); path=read<u32>(a,0x464); write<u8>(a,0x474,(u8)next);
            if(next>=read<u16>(path)) write<u8>(a,0x474,0);
        } goto done;
    }
    case 25: write<s16>(a,0x4A8,(s16)angle(false)); animation(15,9.f,0); advance(); write<f32>(a,0x370,-20.f); [[fallthrough]];
    case 26:
        call(0x0200EDC8,a+0x370,1.f,1.f);
        if(__builtin_fabsf(read<f32>(a,0x370))<0.2f) { write<s16>(a,0x482,1); write<s16>(a,0x484,37); } goto done;
    case 27:
        animation(19,3.f,2); write<f32>(a,0x370,0.f); write<f32>(a,0x33C,0.f); advance(); write<f32>(a,0x340,0.f); write<f32>(a,0x344,0.f); [[fallthrough]];
    case 28: if(!(read<u32>(a,0x2E0)&0x100000)) { write<s16>(a,0x484,90); goto done; } goto done;
    case 100: animation(23,6.f,0); advance(); goto done;
    case 101: if(!finished()) goto done; write<s16>(a,0x484,90); [[fallthrough]];
    case 90: {
        write<f32>(a,0x370,12.f); write<s16>(a,0x494,360); u32 parent=read<u32>(a,0x488); s32 current=read<s32>(a,0x490);
        if(parent!=0xFFFFFFFF) write<f32>(a,0x370,add(read<f32>(parameters,12),9.f));
        if(current!=27) animation(27,7.f,2,8); write<u32>(a,0x39C,4);
        s32 yaw=angle(false); s16 state=read<s16>(a,0x484); write<u8>(a,0x45C,0);
        write<s16>(a,0x4A8,(s16)(yaw+0x8000)); write<s16>(a,0x484,(s16)(state+1)); [[fallthrough]];
    }
    case 91: {
        if(call<s32>(0x027F2BF8,read<u32>(a,0x3D0)+0x98,0.f) && a && a+0x37C) {
            u32 id=read<u32>(a,4); s32 reverb=call<s32>(0x02520540,(s32)read<s8>(a,0x326)); call(0x025E1AA4,0x48F1,a+0x37C,id,0,reverb);
        }
        if(!read<s16>(a,0x49A)) { f32 r=call<f32>(0x02019918,16384.f); s16 yaw=read<s16>(a,0x4A8); write<s16>(a,0x4A8,(s16)(yaw+(s16)ftoi(r))); randomTimer(0x49A,15.f); }
        if(!read<s16>(a,0x496)) { endpoint(); if(call<s32>(0x0244A7F0,a,vector.get(),0)||call<s32>(0x0244B058,a)) { randomTimer(0x496,20.f); randomTimer(0x49A,20.f); } }
        if(!read<s16>(a,0x494)&&read<u32>(a,0x488)==0xFFFFFFFF) {
            play=call<u32>(0x025200D4);
            if(!call<s32>(0x0252A038,play+0x5A20,a+0x314)) {
                write<u32>(a,0x7D8,read<u32>(a,0x7D8)&~2u); animation(11,3.f,0);
                s16 state=read<s16>(a,0x484); u8 altered=read<u8>(a,0x45B); write<u8>(a,0x45A,0); write<s16>(a,0x484,(s16)(state+1));
                if(altered) {
                    u32 model=read<u32>(read<u32>(a,0x3D0),0x90); write<u8>(a,0x45B,0);
                    struct String { be<u32> data,vtable; }; Local<String> archive; archive->data=0x1003835C; archive->vtable=0x10038364;
                    u32 data=call<u32>(0x026066C4,read<u32>(0x101F4F28),archive.get(),42);
                    call(0x025E8154,read<u32>(a,0x3E4),read<u32>(model,0xAC),data,1,0,1.f,0,-1,1,0);
                }
            }
        }
        call(0x0244A5A8,a); goto done;
    }
    case 92: {
        if(a && a+0x37C) { s32 reverb=call<s32>(0x02520540,(s32)read<s8>(a,0x326)); call(0x025E1A40,0x5930,a+0x37C,0,reverb); }
        call(0x0200EDC8,a+0x370,1.f,1.f); s16 alpha=(s16)(read<s16>(a,0x4B6)-3); write<s16>(a,0x4B6,alpha);
        if(alpha<100 && finished()) { write<s16>(a,0x4AA,0); write<s16>(a,0x4B6,100); write<s16>(a,0x4B8,100); call(0x0244B1B8,a); } goto done;
    }
    case 110: {
        for(u32 i=0;i<3;++i) write<u32>(a,0x6F4+i*4,read<u32>(a,0x314+i*4));
        for(u32 i=0;i<3;++i) write<u16>(a,0x700+i*2,read<u16>(a,0x328+i*2));
        particle(0x8445,a+0x6F4,a+0x700,a+0x6E0); animation(12,7.f,2);
        f32 base=add(read<f32>(parameters,16),45.f); write<f32>(a,0x340,base);
        f32 r=call<f32>(0x020198D8,add(read<f32>(parameters,20),15.f)); write<f32>(a,0x340,add(base,r));
        write<f32>(a,0x374,add(read<f32>(parameters,24),-2.f)); base=add(read<f32>(parameters,28),10.f); write<f32>(a,0x370,base);
        r=call<f32>(0x020198D8,add(read<f32>(parameters,32),10.f)); write<f32>(a,0x370,add(base,r));
        u32 id=read<u32>(a,0x488);
        if(id!=0xFFFFFFFF&&call<s32>(0x025D54C4,id,found.get())) {
            u32 parent=load<u32>(ea(found.get()));
            if(parent&&read<s16>(parent,8)==0xD3&&read<s16>(parent,0x512)==2) {
                write<f32>(a,0x374,add(read<f32>(parameters,24),-2.f)); base=add(read<f32>(parameters,28),8.f); write<f32>(a,0x370,base);
                r=call<f32>(0x020198D8,add(read<f32>(parameters,32),5.f)); write<f32>(a,0x370,add(base,r));
            }
        }
        advance(); write<u8>(a,0x45C,1); goto done;
    }
    case 111:
        if(read<u32>(a,0x6E4)) { for(u32 i=0;i<3;++i) write<u32>(a,0x6F4+i*4,read<u32>(a,0x314+i*4)); for(u32 i=0;i<3;++i) write<u16>(a,0x700+i*2,read<u16>(a,0x328+i*2)); }
        if(!(read<u32>(a,0x540)&0x20)) goto done;
        call(0x025A5AC8,a+0x6E0); write<f32>(a,0x370,0.f); write<f32>(a,0x33C,0.f); write<f32>(a,0x340,0.f); write<f32>(a,0x344,0.f); write<f32>(a,0x374,0.f);
        animation(29,0.f,0); advance(); [[fallthrough]];
    case 112: if(finished()) write<s16>(a,0x484,90); goto done;
    default: goto done;
    }
done:
    mode=read<s16>(a,0x484); if(mode<=11) return;
    if(!read<s16>(a,0x498) && mode<90 && (mode==14||mode==16||mode==20)) {
        bool attack=false;
        if(!call<s32>(0x0244B058,a)) {
            play=call<u32>(0x025200D4);
            if(call<f32>(0x025D68EC,a,read<u32>(play,0x12A0+0x488C))<500.f && __builtin_fabsf(sub(read<f32>(a,0x318),read<f32>(player,0x318)))<100.f) {
                vector->x=read<f32>(player,0x314); vector->y=read<f32>(player,0x318); vector->z=read<f32>(player,0x31C);
                if(!call<s32>(0x0244A7F0,a,vector.get(),1) && (read<s16>(a,0x462)==1 || !read<u8>(0x101CF0A4))) attack=true;
            }
        }
        if(attack) { write<s16>(a,0x482,1); write<s16>(a,0x484,30); call(0x0244A9E4,a); }
        else { mode=read<s16>(a,0x484); if((u32)(mode-10)<80) call(0x0244A9E4,a); }
    } else if((u32)(mode-10)<80) call(0x0244A9E4,a);
    if(read<u32>(a,0x488)==0xFFFFFFFF) call(0x0244AA98,a);
}

static void pw_attack_states(u32 a) {
    u32 play=call<u32>(0x025200D4); s16 mode=read<s16>(a,0x484); u32 player=read<u32>(play,0x5B2C);
    auto animation=[&](s32 index,f32 blend,u32 loop,s32 sound=-1) { call(0x0244A37C,a,index,blend,loop,1.f,sound); };
    auto advance=[&]() { write<s16>(a,0x484,(s16)(read<s16>(a,0x484)+1)); };
    auto finished=[&]() { u32 morf=read<u32>(a,0x3D0); return (read<u8>(morf,0xA7)&1)||read<f32>(morf,0x98)==0.f; };
    auto angle=[&]() { u32 p=call<u32>(0x025200D4); return call<s32>(0x025D6894,a,read<u32>(p,0x5B2C)); };
    auto voice=[&](s32 id) { if(a && a+0x37C) { u32 process=read<u32>(a,4); s32 reverb=call<s32>(0x02520540,(s32)read<s8>(a,0x326)); call(0x025E1AA4,id,a+0x37C,process,0,reverb); } };
    auto retreat=[&]() {
        f32 r=call<f32>(0x020198D8,70.f); write<s16>(a,0x498,(s16)ftoi(add(r,70.f)));
        if(read<u8>(a,0x475)==255) { f32 x=read<f32>(a,0x314),z=read<f32>(a,0x31C); write<f32>(a,0x40C,x); write<f32>(a,0x414,z); }
        write<s16>(a,0x482,0); write<s16>(a,0x484,13);
    };
    switch(mode) {
    case 30:
        animation(read<s16>(a,0x462)==1?24:25,6.f,0); advance(); write<u8>(a,0x45C,0); break;
    case 31:
        call(0x0244AE40,a); write<s16>(a,0x4A8,(s16)angle()); call(0x0200EDC8,a+0x370,1.f,0.3f);
        if(!finished()) break; advance(); [[fallthrough]];
    case 32:
        call(0x0244AE40,a); for(u32 off=0x4A0;off<=0x4A6;off+=2) write<s16>(a,off,0); write<u8>(a,0x45C,0);
        if(read<s16>(a,0x462)==1) { animation(12,6.f,0,7); voice(0x48F3); write<u8>(a,0x45D,1); write<s16>(a,0x484,33); }
        else { animation(31,3.f,2); write<s16>(a,0x494,100); write<s16>(a,0x484,40); } break;
    case 33:
        call(0x0244AE40,a); call(0x0200EDC8,a+0x370,1.f,0.3f); write<s16>(a,0x4A8,(s16)angle());
        if(call<s32>(0x027F2BF8,read<u32>(a,0x3D0)+0x98,23.f)) { advance(); write<f32>(a,0x370,24.f); } break;
    case 34: {
        call(0x0244B308,a); if(call<s32>(0x0244ACBC,a,0)||call<s32>(0x0244ACBC,a,1)) goto end_attack;
        if(call<s32>(0x027F2BF8,read<u32>(a,0x3D0)+0x98,28.f)) { write<u32>(a,0x874,read<u32>(a,0x874)|1); write<u32>(a,0x878,1); }
        u32 morf=read<u32>(a,0x3D0);
        if(!(read<f32>(morf,0x9C)<=28.f) && (read<u32>(a,0x8C8)&1)) {
            write<u32>(a,0x874,read<u32>(a,0x874)&~1u); write<s16>(a,0x482,0); write<s16>(a,0x484,25); return;
        }
        if(call<s32>(0x027F2BF8,morf+0x98,40.f)) { u32 flags=read<u32>(a,0x874); s16 current=read<s16>(a,0x484); write<u32>(a,0x874,flags&~1u); write<s16>(a,0x484,(s16)(current+1)); } break;
    }
    case 35:
        call(0x0200EDC8,a+0x370,1.f,5.f); if(__builtin_fabsf(read<f32>(a,0x370))<0.2f) goto end_attack; break;
    case 40:
        call(0x0200EDC8,a+0x370,1.f,0.3f); write<s16>(a,0x4A8,(s16)angle()); if(read<s16>(a,0x494)) break;
        write<s16>(a,0x494,300); animation(32,4.f,2); voice(0x48F4); advance(); break;
    case 41: {
        write<s16>(a,0x4A8,(s16)angle()); if(call<s32>(0x0244ACBC,a,0)||call<s32>(0x0244ACBC,a,1)) goto end_attack;
        call(0x0200ED84,a+0x370,15.f,1.f,3.f); call(0x0244B308,a);
        if(!read<s16>(a,0x494)) { animation(22,6.f,0); write<s16>(a,0x484,36); break; }
        struct Vec { be<f32> x,y,z; }; Local<Vec> target;
        target->x=read<f32>(player,0x314); target->y=read<f32>(player,0x318); target->z=read<f32>(player,0x31C);
        if(call<s32>(0x0244A7F0,a,target.get(),1)) retreat(); break;
    }
    case 36: call(0x0200EDC8,a+0x370,1.f,1.f); if(finished()) goto end_attack; break;
    case 37: {
        for(u32 off=0x4A0;off<=0x4A6;off+=2) write<s16>(a,off,0);
        s16 held=read<s16>(a,0x462); write<f32>(a,0x370,0.f); s32 desired=held==1?35:36;
        if(read<s32>(a,0x490)!=desired) animation(desired,7.f,2);
        f32 r=call<f32>(0x020198D8,70.f); write<s16>(a,0x498,(s16)ftoi(add(r,70.f)));
        write<u8>(a,0x45C,0); write<u32>(a,0x43C,read<u32>(a,0x314)); write<u32>(a,0x440,read<u32>(a,0x318)); write<u32>(a,0x444,read<u32>(a,0x31C));
        write<u8>(a,0x45D,0); write<s16>(a,0x4AE,0); advance(); [[fallthrough]];
    }
    case 38:
        call(0x0244AE40,a); write<s16>(a,0x4A8,(s16)angle()); call(0x0200EDC8,a+0x370,1.f,1.f);
        if(!read<s16>(a,0x498)) { write<f32>(a,0x370,0.f); call(0x0244B1B8,a); } break;
    default: break;
    }
    goto checks;
end_attack: write<s16>(a,0x484,37);
checks:
    if(!read<s16>(a,0x462)) {
        if(read<u8>(0x101CF0A4)==1) retreat();
        else {
            mode=read<s16>(a,0x484);
            if((mode==40||mode==41||mode==38)&&call<s32>(0x025160DC,a+0x744)) {
                u32 other=call<u32>(0x02515BBC,a+0x794);
                if(other && other==player) {
                    play=call<u32>(0x025200D4);
                    if(other==read<u32>(play,0x5B34)) {
                        write<f32>(a,0x370,0.f); write<u32>(a,0x744,read<u32>(a,0x744)&~1u);
                        write<u32>(a,0x76C,0x800000); write<u8>(0x101CF0A4,0,1); write<u8>(a,0x45F,1);
                        write<s16>(a,0x482,3); write<s16>(a,0x484,70);
                    }
                }
            }
        }
    }
    call(0x0244A9E4,a); call(0x0244AA98,a);
}

static void pw_damage_states(u32 a) {
    constexpr u32 parameters=0x1047B608;
    u32 play=call<u32>(0x025200D4); s16 mode=read<s16>(a,0x484);
    u32 model=read<u32>(read<u32>(a,0x3D0),0x90),player=read<u32>(play,0x5B2C);
    auto animation=[&](s32 index,f32 blend,u32 loop) { call(0x0244A37C,a,index,blend,loop,1.f,-1); };
    auto advance=[&]() { write<s16>(a,0x484,(s16)(read<s16>(a,0x484)+1)); };
    auto finished=[&]() { u32 morf=read<u32>(a,0x3D0); return (read<u8>(morf,0xA7)&1)||read<f32>(morf,0x98)==0.f; };
    auto angle=[&]() { u32 p=call<u32>(0x025200D4); return call<s32>(0x025D6894,a,read<u32>(p,0x5B2C)); };
    auto sound=[&](s32 id) { if(a+0x37C) { s32 reverb=call<s32>(0x02520540,(s32)read<s8>(a,0x326)); call(0x025E1A40,id,a+0x37C,0,reverb); } };
    auto voice=[&](s32 id) { if(a+0x37C) { u32 process=a?read<u32>(a,4):0xFFFFFFFF; s32 reverb=call<s32>(0x02520540,(s32)read<s8>(a,0x326)); call(0x025E1AA4,id,a+0x37C,process,0,reverb); } };
    struct String { be<u32> data,vtable; }; Local<String> archive[4];
    auto material=[&](u32 slot,u32 field,s32 resource,u32 loop) {
        archive[slot]->vtable=0x10038364; archive[slot]->data=0x1003835F;
        u32 data=call<u32>(0x026066C4,read<u32>(0x101F4F28),archive[slot].get(),resource);
        call(0x025E8154,read<u32>(a,field),read<u32>(model,0xAC),data,1,loop,1.f,0,-1,1,0);
    };
    switch(mode) {
    case 50: {
        write<u8>(a,0x454,0); write<u8>(a,0x45C,1); u32 flags=read<u32>(a,0x2E0);
        write<u8>(a,0x45D,1); write<u32>(a,0x39C,4); write<u32>(a,0x2E0,flags|0x20);
        s32 yaw=angle(); write<s16>(a,0x322,(s16)yaw); write<s16>(a,0x4A8,(s16)yaw);
        write<s16>(a,0x4AA,0); write<s16>(a,0x4B6,200); write<s16>(a,0x32A,(s16)yaw); write<s16>(a,0x4B8,200); write<s16>(a,0x49A,3);
        if(!read<s16>(a,0x4B4)) call(0x0244A4A8,a);
        write<f32>(a,0x370,-20.f); animation(15,3.f,0); advance(); break;
    }
    case 51:
        call(0x0200EDC8,a+0x370,1.f,1.f); if(finished()) { write<f32>(a,0x370,0.f); call(0x0244B1B8,a); } break;
    case 52: {
        call(0x0244A4A8,a); s16 yaw=read<s16>(a,0x322); write<s16>(a,0x4A8,yaw); write<u8>(a,0x45C,1); write<s16>(a,0x32A,yaw);
        for(u32 off=0x4A0;off<=0x4A6;off+=2) write<s16>(a,off,0);
        if(read<s32>(a,0x490)!=19) animation(19,3.f,2);
        write<s16>(a,0x494,30); write<f32>(a,0x370,0.f); advance(); [[fallthrough]];
    }
    case 53: {
        u8 hit=read<u8>(a,0x458);
        if(hit==9||hit==10) {
            write<s16>(a,0x494,10); write<s16>(a,0x4B6,(s16)(read<s16>(a,0x4B6)+5));
            if(!read<u8>(a,0x45B)) { write<u8>(a,0x45B,1); material(0,0x3E0,43,2); }
            if(read<u8>(a,0x458)==10||read<s16>(a,0x4B6)>=255) {
                write<s16>(a,0x4B6,255); write<s16>(a,0x4AA,0); if(a) { sound(0x5936); voice(0x48F2); }
                u32 flags=read<u32>(a,0x7D8); write<u8>(a,0x45A,1); write<u32>(a,0x7D8,flags|2); material(1,0x3DC,44,0);
                write<s16>(a,0x482,0); write<s16>(a,0x484,100); write<u32>(a,0x76C,0xFF3DFEFF); return;
            }
            if(a) sound(0x5135);
        } else {
            if(read<u8>(a,0x45B)) { write<u8>(a,0x45B,0); material(2,0x3E4,42,0); }
            write<s16>(a,0x4B8,0); write<s16>(a,0x4B6,0); write<s16>(a,0x4AA,0); call(0x0244B1B8,a);
        } break;
    }
    case 54: {
        write<u8>(a,0x45C,1); write<s16>(a,0x49C,0); call(0x025A5AC8,a+0x6E0);
        s16 yaw=read<s16>(a,0x32A); write<s16>(a,0x4A8,yaw); write<s16>(a,0x322,(s16)angle());
        write<s16>(a,0x32A,read<s16>(a,0x4A8)); s8 health=read<s8>(a,0x3A1); write<f32>(a,0x370,-24.f);
        if(health<=0) {
            u32 flags=read<u32>(a,0x744),target=read<u32>(a,0x75C); write<u32>(a,0x744,flags&~1u); write<u32>(a,0x75C,target&~1u);
            call(0x0251621C,a+0x744); write<f32>(a,0x370,(f32)((double)read<f32>(a,0x370)*1.5f));
        }
        voice(0x48F5); animation(14,3.f,0); advance(); break;
    }
    case 55:
        if(call<s32>(0x0244A5A8,a)) break;
        call(0x0200EDC8,a+0x370,1.f,1.f); if(!finished()) break;
        { s8 health=read<s8>(a,0x3A1); write<f32>(a,0x370,0.f); write<s16>(a,0x484,health>0?90:60); if(health>0) write<s16>(a,0x482,0); } return;
    case 56:
        write<s16>(a,0x322,read<s16>(player,0x32A)); write<u8>(a,0x45C,1); write<f32>(a,0x370,24.f); write<f32>(read<u32>(a,0x3D0),0x98,0.25f); advance(); [[fallthrough]];
    case 57:
        if(call<s32>(0x0244A5A8,a)) break;
        call(0x0200EDC8,a+0x370,1.f,1.f);
        if(read<f32>(a,0x370)<0.2f) {
            s16 yaw=read<s16>(a,0x32A); write<f32>(a,0x370,0.f); write<s16>(a,0x322,yaw);
            write<u8>(a,0x45C,0); write<f32>(read<u32>(a,0x3D0),0x98,1.f); write<s16>(a,0x482,0); write<s16>(a,0x484,90); return;
        } break;
    case 58: {
        u8 kind=read<u8>(a,0x458); write<u8>(a,0x45C,1); write<f32>(a,0x370,0.f);
        if(kind==4) { call(0x02041CAC,a); sound(0x50BC); write<s16>(a,0x494,(s16)(read<s16>(parameters,0x8E)+75)); }
        animation(26,3.f,0); advance(); [[fallthrough]];
    }
    case 59:
        if(!call<s32>(0x0244A5A8,a)&&!read<s16>(a,0x494)) { write<s16>(a,0x482,0); write<s16>(a,0x484,90); return; } break;
    case 60:
        animation(30,2.f,2); write<u8>(a,0x45A,0); write<u32>(a,0x39C,0); call(0x025A5AC8,a+0x6E0);
        if(!read<s16>(a,0x4A6)) { sound(0x5934); voice(0x48F6); }
        if(read<u8>(a,0x45B)) { write<u8>(a,0x45B,0); material(3,0x3E4,42,0); }
        write<f32>(a,0x374,0.6f); advance(); [[fallthrough]];
    case 61: {
        s16 alpha=(s16)(read<s16>(a,0x4B6)-8); write<s16>(a,0x4B6,alpha); if(alpha>=0) break;
        s16 disappear=read<s16>(a,0x4A6); write<s16>(a,0x4AA,0); write<s16>(a,0x4B6,0);
        if(!disappear) {
            struct Vec { be<f32> x,y,z; }; Local<Vec> position;
            position->x=read<f32>(a,0x314); f32 y=read<f32>(a,0x318); position->y=y; position->z=read<f32>(a,0x31C); position->y=add(y,60.f);
            call(0x025D99E8,a,position.get(),5,0,255);
        }
        call(0x025D57E0,a); s32 room=read<s8>(a,0x2FE); u32 save=read<u32>(0x101F84DC);
        call(0x025BA5D4,save+32,(u32)read<u16>(a,0x2D8),room); break;
    }
    case 62: if(finished()) { write<s16>(a,0x484,60); return; } break;
    default: break;
    }
    mode=read<s16>(a,0x484);
    if(mode<=51) { if(!read<s16>(a,0x49A)) call(0x0244A9E4,a); call(0x0244AA98,a); }
}

s32 pw_execute(pw_class* actor) {
    WWHD_FUNC(0x0244B3E4,s32,actor);
    u32 a=ea(actor); call(0x025DA088,a,60,13,45);
    if(call<s32>(0x020402C8,a+0x9A0)) {
        u32 morf=read<u32>(a,0x3D0); constexpr u32 matrix=0x1048D0CC;
        f32 values[12]; for(u32 i=0;i<12;++i) values[i]=read<f32>(matrix,i*4);
        u32 model=read<u32>(morf,0x90); const u32 order[12]={0,1,2,3,4,8,5,6,7,9,10,11};
        for(u32 i:order) write<f32>(model,0xC8+i*4,values[i]);
        call(0x025E55A0,read<u32>(a,0x3D0)); return 1;
    }
    for(u32 off=0x494;off<=0x49E;off+=2) { s16 timer=read<s16>(a,off); if(timer) write<s16>(a,off,(s16)(timer-1)); }
    s16 action=read<s16>(a,0x482);
    switch(action) {
    case 0: pw_idle_states(a); break;
    case 1: pw_attack_states(a); break;
    case 2: pw_damage_states(a); break;
    case 3: call(0x0244F960,a); break;
    case 4: call(0x0244FE24,a); break;
    case 5: call(0x024500C0,a); break;
    default: break;
    }
    if(!read<u8>(a,0x45C)) {
        s16 mode=read<s16>(a,0x484); s32 step=(mode==15||mode==16||mode==20)?500:0x1000;
        if(read<s16>(a,0x482)==5) step=read<s16>(a,0x4BA);
        call(0x0200F428,a+0x322,(s32)read<s16>(a,0x4A8),1,step);
        call(0x0200F428,a+0x32A,(s32)read<s16>(a,0x322),1,step);
    }
    u8 visible=read<u8>(a,0x45A); u32 animation;
    if(visible) animation=read<u32>(a,0x3DC);
    else animation=read<u32>(a,read<u8>(a,0x45B)?0x3E0:0x3E4);
    call(0x025E742C,animation);
    call(0x025F1884,read<u32>(0x1018C7B0),(s32)read<s16>(a,0x322));
    call(0x025F1BF4,read<u32>(0x1018C7B0),(s32)read<s16>(a,0x320));
    struct Vec { be<f32> x,y,z; }; Local<Vec> velocity,transformed,position;
    velocity->x=0.f; velocity->y=0.f; velocity->z=read<f32>(a,0x370);
    call(0x0200FCD8,velocity.get(),transformed.get());
    f32 gravity=read<f32>(a,0x374),x=transformed->x,y=read<f32>(a,0x340);
    write<f32>(a,0x33C,x); f32 nextY=add(y,gravity),z=transformed->z; write<f32>(a,0x344,z);
    write<f32>(a,0x340,nextY<-100.f?-100.f:nextY);
    Local<be<u32>> found;
    if(read<s16>(a,0x462)==1) {
        u32 id=read<u32>(a,0x48C);
        if(id!=0xFFFFFFFF && call<s32>(0x025D54C4,id,found.get())) {
            u32 lantern=load<u32>(ea(found.get()));
            if(lantern && read<s16>(lantern,8)==0xC1) {
                x=read<f32>(a,0x3F4); write<f32>(a,0x390,x); y=read<f32>(a,0x3F8); write<f32>(a,0x394,y);
                z=read<f32>(a,0x3FC); write<f32>(a,0x398,z); write<f32>(a,0x394,add(y,40.f));
                write<u32>(a,0x37C,read<u32>(a,0x3F4)); write<u32>(a,0x380,read<u32>(a,0x3F8)); write<u32>(a,0x384,read<u32>(a,0x3FC));
            }
        }
    } else {
        x=read<f32>(a,0x314); y=read<f32>(a,0x318); write<f32>(a,0x390,x); write<f32>(a,0x37C,x);
        z=read<f32>(a,0x31C); write<f32>(a,0x398,z); write<f32>(a,0x384,z);
        write<f32>(a,0x394,add(y,200.f)); write<f32>(a,0x380,add(y,100.f));
    }
    position->x=read<f32>(a,0x314); position->y=read<f32>(a,0x318); position->z=read<f32>(a,0x31C);
    call(0x020182E0,a+0x85C,position.get());
    action=read<s16>(a,0x482);
    if(action==4) {
        call(0x02018428,a+0x85C,100.f); call(0x020184DC,a+0x85C,add(read<f32>(0x1047B608,0x4C0),40.f));
    } else { call(0x02018428,a+0x85C,200.f); call(0x020184DC,a+0x85C,80.f); }
    u32 play=call<u32>(0x025200D4); call(0x0200E240,play+0x26A4,a+0x744);
    action=read<s16>(a,0x482);
    if(action!=3&&action!=5&&action!=4) {
        s16 mode=read<s16>(a,0x484);
        if(mode!=61&&mode!=110&&mode!=111) call(0x0244E960,a);
    }
    call(0x025D6800,a,read<s16>(a,0x462)==1?a+0x708:0u);
    s16 mode=read<s16>(a,0x484); bool advanceAnimation=true;
    if(mode!=81&&mode!=61&&mode!=8) { call(0x0244A768,a); if(read<s16>(a,0x484)<2) advanceAnimation=false; }
    if(advanceAnimation && !read<s16>(a,0x49C)) {
        if(read<f32>(a,0x5AC)!=-1000000000.f) {
            s32 material=0;
            if(read<u32>(a,0x540)&0x20) { play=call<u32>(0x025200D4); material=call<s32>(0x024EECAC,play+0x12A0,a+0x600); }
            s32 reverb=call<s32>(0x02520540,(s32)read<s8>(a,0x326)); call(0x025E535C,read<u32>(a,0x3D0),a+0x37C,material,reverb);
        } else call(0x025E535C,read<u32>(a,0x3D0),0,0,0);
    }
    call(0x02449DA8,a);
    if(read<s16>(a,0x462)==1) {
        call(0x0244F730,a);
        if(!read<s16>(a,0x49E)) {
            call(0x0244F28C,a); u32 id=read<u32>(a,0x48C);
            if(id!=0xFFFFFFFF && call<s32>(0x025D54C4,id,found.get())) {
                u32 lantern=load<u32>(ea(found.get()));
                if(lantern&&read<s16>(lantern,8)==0xC1) {
                    call(0x02018D40,a+0x98C,a+0x3F4); call(0x02018C8C,a+0x98C,40.f);
                    play=call<u32>(0x025200D4); call(0x0200E240,play+0x26A4,a+0x874);
                }
            }
        }
    }
    return 1;
}
VERIFY(0x0244B3E4,pw_execute);
