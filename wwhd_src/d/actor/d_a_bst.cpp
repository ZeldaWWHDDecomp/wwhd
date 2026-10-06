#include "d/actor/d_a_bst.h"

static BOOL message_set(bst_class* actor, u32 number) {
    WWHD_FUNC(0x020E46A0, BOOL, actor, number);
    if (gabi::load<u32>(0x1046298C) != 0xFFFFFFFF) return FALSE;
    u32 control = gabi::load<u32>(0x101F4B5C);
    u32 message = gabi::call<u32>(0x025F7DB0, gabi::at<void>(control), number,
                                gabi::at<void>(gabi::ea(actor) + 0x37C));
    gabi::store<u32>(0x1046298C, message);
    if (message != 0xFFFFFFFF) gabi::store<u8>(0x10462998, 0);
    return TRUE;
}
VERIFY(0x020E46A0, message_set);

static void message_end() {
    WWHD_FUNC(0x020E471C, void);
    u32 control = gabi::load<u32>(0x101F4B5C);
    if (control && control + 0x14) gabi::store<u8>(control + 0x6AB, 1);
    gabi::store<u8>(0x10462998, 1);
}
VERIFY(0x020E471C, message_end);

static void set_hand_AT(bst_class* actor, u8 enabled) {
    WWHD_FUNC(0x020E474C, void, actor, enabled);
    for (u32 i=0; i<19; ++i) {
        u32 sphere = gabi::ea(actor) + 0x1804 + i*0x12C;
        u32 flags = gabi::load<u32>(sphere);
        gabi::store<u8>(sphere+0x6F, 1);
        gabi::store<u32>(sphere, enabled ? flags|1 : flags&~1u);
    }
}
VERIFY(0x020E474C, set_hand_AT);

static void set_hand_CO(bst_class* actor, u8 enabled) {
    WWHD_FUNC(0x020E4790, void, actor, enabled);
    for (u32 i=0; i<19; ++i) {
        u32 flagsAddress = gabi::ea(actor)+0x1830+i*0x12C;
        u32 flags = gabi::load<u32>(flagsAddress);
        gabi::store<u32>(flagsAddress, enabled ? flags|1 : flags&~1u);
    }
}
VERIFY(0x020E4790, set_hand_CO);

static BOOL daBst_IsDelete(bst_class* actor) {
    WWHD_FUNC(0x020E7188, BOOL, actor);
    return TRUE;
}
VERIFY(0x020E7188, daBst_IsDelete);

static void* bstHioConstructor(void* self) {
    WWHD_FUNC(0x020E8718, void*, self);
    if (!self) self=gabi::call<void*>(0x0273AD10, 8);
    if (self) {
        u32 address=gabi::ea(self);
        gabi::store<s8>(address,-1);
        gabi::store<u32>(address+4,0x1000B80C);
        gabi::store<u8>(address+2,0);
        gabi::store<u8>(address+1,0);
    }
    return self;
}
VERIFY(0x020E8718, bstHioConstructor);

static void bstStaticInit() {
    WWHD_FUNC(0x020E876C, void);
    gabi::store<u32>(0x104629A4,0);
    gabi::store<u32>(0x1046299C,0);
    gabi::store<u32>(0x104629A8,0);
    gabi::store<u32>(0x104629A0,0);
    gabi::call(0x028F026C,gabi::at<void>(0x10192AAC));
    f32 lower=gabi::load<f32>(0x1000B918);
    f32 upper=gabi::load<f32>(0x1000B91C);
    gabi::store<f32>(0x10462990,lower);
    gabi::store<f32>(0x10462994,upper);
    gabi::call(0x028ED6F8,gabi::at<void>(0x10462999));
    gabi::call(0x028F026C,gabi::at<void>(0x10192AB8));
    gabi::call(0x028EAB2C,gabi::at<void>(0x1046299A));
    gabi::call(0x028F026C,gabi::at<void>(0x10192AC4));
    bstHioConstructor(gabi::at<void>(0x10462980));
}
VERIFY(0x020E876C, bstStaticInit);

static void bstSoundStart(bst_class* actor,u32 sound,u32 parameter) {
    WWHD_FUNC(0x020E880C, void, actor, sound, parameter);
    u32 address=gabi::ea(actor);
    if (address && address+0x37C) {
        s8 room=gabi::load<s8>(address+0x326);
        s8 reverb=gabi::call<s8>(0x02520540,room);
        gabi::call(0x025E1A40,sound,gabi::at<void>(address+0x37C),parameter,reverb);
    }
}
VERIFY(0x020E880C, bstSoundStart);

static void bstHioDestructor(void* self,u32 flags) {
    WWHD_FUNC(0x020E8878, void, self, flags);
    if (self && (flags&1)) gabi::call(0x0273AF40,self);
}
VERIFY(0x020E8878, bstHioDestructor);

static void bstDestructor(bst_class* actor,u32 flags) {
    WWHD_FUNC(0x020F01C8, void, actor, flags);
    if (!actor) return;
    u32 address=gabi::ea(actor);
    gabi::call(0x028F0164,gabi::at<void>(address+0x2E48),2,0x12C,0x02515AE8,0,0);
    gabi::call(0x028F0164,gabi::at<void>(address+0x1804),19,0x12C,0x02515AE8,0,0);
    gabi::call(0x02515A70,gabi::at<void>(address+0x16D4),2);
    gabi::call(0x02515A70,gabi::at<void>(address+0x15A4),2);
    gabi::call(0x02515860,gabi::at<void>(address+0x1568),2);
    gabi::store<u32>(address+0x13C4,0x1000B7AC);
    gabi::store<u32>(address+0x13B8,0x1000B7BC);
    gabi::call(0x024EFD9C,gabi::at<void>(address+0x13A4),0);
    gabi::call(0x02018034,gabi::at<void>(address+0x1378),2);
    gabi::call(0x028F0164,gabi::at<void>(address+0x724),10,0x12C,0x02515AE8,0,0);
    gabi::call(0x025D50BC,actor,0);
    if (flags&1) gabi::call(0x0273AF40,actor);
}
VERIFY(0x020F01C8, bstDestructor);

static void bstEmptyVirtual(void* self) {
    WWHD_FUNC(0x020F02D0, void, self);
}
VERIFY(0x020F02D0, bstEmptyVirtual);

static void* bstResource(s32 index,u32 archive=0x1000B85C) {
    gabi::Local<SafeString> name;
    name->mStringTop=archive;
    name->__vtbl=0x1000B714;
    u32 control=gabi::load<u32>(0x101F4F28);
    return gabi::call<void*>(0x026066C4,gabi::at<void>(control),name.get(),index);
}

static void anm_init(bst_class* actor,s32 index,f32 morph,u8 loop,f32 speed,s32 sound) {
    WWHD_FUNC(0x020E47D0,void,actor,index,morph,loop,speed,sound);
    void* animation=bstResource(index);
    void* audio=sound>=0 ? bstResource(sound) : nullptr;
    u32 controller=gabi::load<u32>(gabi::ea(actor)+0x3D4);
    f32 first=gabi::load<f32>(0x1000B854),last=gabi::load<f32>(0x1000B858);
    gabi::call(0x025E4A98,gabi::at<void>(controller),animation,loop,morph,speed,first,last,audio);
}
VERIFY(0x020E47D0,anm_init);

static BOOL nodeCallBackHead(void* node,s32 timing) {
    WWHD_FUNC(0x020E48F8,BOOL,node,timing);
    if (timing==0) {
        void* joint=gabi::call<void*>(0x027F7878,node);
        u32 model=gabi::load<u32>(0x104B462C);
        u32 actor=gabi::load<u32>(model+0xB8);
        u16 number=gabi::load<u16>(gabi::ea(joint)+4);
        if (actor) {
            u32 block=gabi::load<u32>(model+0x2C);
            u32 matrices=gabi::load<u32>(block+0x10);
            u16 flags=gabi::load<u16>(block+4);
            gabi::store<u16>(block+4,flags|0x10);
            u32 current=gabi::load<u32>(0x1018C7B0);
            gabi::call(0x028E90D4,gabi::at<void>(matrices+number*0x30),gabi::at<void>(current));
            if (number==9 || number==12 || number==10 || number==11) {
                s16 angle=gabi::load<s16>(actor+(number==9 || number==12 ? 0x30AE : 0x30AC));
                if (number==9 || number==10) angle=(s16)-angle;
                current=gabi::load<u32>(0x1018C7B0);
                gabi::call(0x025F1C28,gabi::at<void>(current),angle);
            }
            block=gabi::load<u32>(model+0x2C);
            current=gabi::load<u32>(0x1018C7B0);
            flags=gabi::load<u16>(block+4);
            matrices=gabi::load<u32>(block+0x10);
            gabi::store<u16>(block+4,flags|0x10);
            mtx_copy(gabi::at<Mtx34>(matrices+number*0x30),gabi::at<Mtx34>(current));
            current=gabi::load<u32>(0x1018C7B0);
            gabi::call(0x028E90D4,gabi::at<void>(current),gabi::at<void>(0x104B4868));
        }
    }
    return TRUE;
}
VERIFY(0x020E48F8,nodeCallBackHead);

static void pos_move(bst_class* actor,u8 straight) {
    WWHD_FUNC(0x020E4CD0,void,actor,straight);
    u32 address=gabi::ea(actor);
    gabi::Local<cXyz> vector;
    if (!straight) {
        gabi::Local<cXyz> difference;
        gabi::call(0x0201ADE0,gabi::at<void>(address+0x1310),difference.get(),gabi::at<void>(address+0x314));
        f32 x=difference->x,y=difference->y,z=difference->z;
        vector->x=x;vector->y=y;vector->z=z;
        s16 yaw=gabi::call<s16>(0x020195B0,x,z);
        z=vector->z;x=vector->x;
        f32 distance=gabi::call<f32>(0x028F4384,gabi::fmadds(x,x,z*z));
        y=vector->y;
        s16 pitch=(s16)-gabi::call<s16>(0x020195B0,y,distance);
        f32 scale=gabi::load<f32>(address+0x1324);
        f32 limit=gabi::load<f32>(address+0x1320);
        s16 divisor=(s16)(gabi::load<s16>(0x1047B68E)+5);
        gabi::call(0x0200F428,gabi::at<void>(address+0x322),yaw,divisor,(s16)gabi::ftoi(limit*scale));
        limit=gabi::load<f32>(address+0x1320);
        scale=gabi::load<f32>(address+0x1324);
        divisor=(s16)(gabi::load<s16>(0x1047B68E)+5);
        gabi::call(0x0200F428,gabi::at<void>(address+0x320),pitch,divisor,(s16)gabi::ftoi(limit*scale));
    }
    f32 one=gabi::load<f32>(0x1000B860);
    f32 step=gabi::load<f32>(0x1000B864);
    gabi::call(0x0200ED84,gabi::at<void>(address+0x1324),one,one,step);
    f32 zero=gabi::load<f32>(0x1000B854);
    vector->y=zero;vector->x=zero;
    f32 speed=gabi::load<f32>(address+0x370);
    u32 matrix=gabi::load<u32>(0x1018C7B0);
    vector->z=speed;
    s16 yaw=gabi::load<s16>(address+0x322);
    gabi::call(0x025F1884,gabi::at<void>(matrix),yaw);
    s16 pitch=gabi::load<s16>(address+0x320);
    matrix=gabi::load<u32>(0x1018C7B0);
    gabi::call(0x025F1BF4,gabi::at<void>(matrix),pitch);
    gabi::call(0x0200FCD8,vector.get(),gabi::at<void>(address+0x33C));
    gabi::call(0x028E8D88,gabi::at<void>(address+0x314),gabi::at<void>(address+0x33C),gabi::at<void>(address+0x314));
}
VERIFY(0x020E4CD0,pos_move);

static BOOL player_way_check(bst_class* actor) {
    WWHD_FUNC(0x020E4E48,BOOL,actor);
    void* play=gabi::call<void*>(0x025200D4);
    u32 player=gabi::load<u32>(gabi::ea(play)+0x5B2C);
    s16 angle=gabi::load<s16>(gabi::ea(actor)+0x32A);
    s16 playerAngle=gabi::load<s16>(player+0x32A);
    s16 difference=(s16)(angle-playerAngle);
    if (difference<0) difference=(s16)-difference;
    return (u16)difference>=0x4000;
}
VERIFY(0x020E4E48,player_way_check);

static BOOL daBst_Delete(bst_class* actor) {
    WWHD_FUNC(0x020E7190,BOOL,actor);
    u32 address=gabi::ea(actor);
    gabi::call(0x025204C8,gabi::at<void>(address+0x3C8),STR(0x1000B8F8));
    if (gabi::load<u8>(address+0x3331)) {
        s8 child=gabi::load<s8>(0x10462980);
        gabi::store<u8>(0x10192910,0);
        gabi::call(0x025F0A18,child);
    }
    if (gabi::load<u8>(address+0x3D0)==0) {
        gabi::Local<be<u32>> id;
        for (u32 i=0;i<2;++i) {
            u32 value=gabi::load<u32>(address+0x30C4+i*4);
            *id=value;
            void* attachment=nullptr;
            if (value!=0xFFFFFFFF) attachment=gabi::call<void*>(0x025D5218,0x025E1234,id.get());
            if (attachment) gabi::call(0x025D57E0,attachment);
        }
    }
    u32 callback=gabi::load<u32>(address+0x3134);
    u32 target=gabi::load<u32>(callback+0x44);
    gabi::call(target,gabi::at<void>(address+0x3134));
    for (u32 i=0;i<10;++i) gabi::call(0x025E1B34,gabi::at<void>(address+0x5EC+i*12));
    return TRUE;
}
VERIFY(0x020E7190,daBst_Delete);

static void beam_eff_set(cXyz* position,s16 direction,u8 oriented) {
    WWHD_FUNC(0x020E4EA4,void,position,direction,oriented);
    gabi::Local<csXyz> angles;
    if (oriented) gabi::call(0x0201A478,angles.get(),(s16)-0x4000,direction,(s16)0);
    for (u32 i=0;i<3;++i) {
        void* play=gabi::call<void*>(0x025200D4);
        u32 particles=gabi::load<u32>(gabi::ea(play)+0x5AB0);
        u32 effect=i==0 ? 0x81BF : 0xA1BF+i;
        gabi::call(0x025A847C,gabi::at<void>(particles),0,effect,position,
                   oriented ? angles.get() : nullptr,nullptr,255,nullptr,-1,0,nullptr,nullptr);
    }
}
VERIFY(0x020E4EA4,beam_eff_set);

static void end_brkbtk_set(bst_class* actor) {
    WWHD_FUNC(0x020E5068,void,actor);
    const u32 actorGlobals[3]={0x10462978,0x1046297C,0x10462988};
    const s32 brkMain[3]={0x40,0x48,0x39},btkMain[3]={0x5A,0x62,0x54};
    const s32 brkPedestal[3]={0x4B,0x4E,0x43},btkPedestal[3]={0x65,0x68,0x5D};
    f32 speed=0;
    for (u32 pedestal=0;pedestal<2;++pedestal) {
        for (u32 part=0;part<3;++part) {
            for (u32 texture=0;texture<2;++texture) {
                u32 boss=gabi::load<u32>(actorGlobals[part]);
                u32 model;
                if (pedestal) model=gabi::load<u32>(boss+0x3E4);
                else {
                    u32 morph=gabi::load<u32>(boss+0x3D4);
                    model=gabi::load<u32>(morph+0x90);
                }
                s32 index=pedestal ? (texture ? btkPedestal[part] : brkPedestal[part])
                                  : (texture ? btkMain[part] : brkMain[part]);
                void* animation=bstResource(index,0x1000B868);
                // The first BRK setup loads its model data before the speed constant;
                // subsequent BTK setups fetch the actor before reading model data.
                u32 data;
                if (!texture) data=gabi::load<u32>(model+0xAC);
                if (!pedestal && !part && !texture) speed=gabi::load<f32>(0x1000B860);
                boss=gabi::load<u32>(actorGlobals[part]);
                if (texture) data=gabi::load<u32>(model+0xAC);
                u32 controller=gabi::load<u32>(boss+(pedestal ? (texture ? 0x3E8 : 0x3EC)
                                                                          : (texture ? 0x3D8 : 0x3DC)));
                gabi::call(texture ? 0x025E7CE0 : 0x025E8154,gabi::at<void>(controller),
                           gabi::at<void>(data),animation,1,0,0,-1,1,speed,0);
            }
        }
    }
}
VERIFY(0x020E5068,end_brkbtk_set);

struct BstTevDefaults {
    f32 initial[6],tail[8];
    u8 flags[4];
    s16 angles[4];
};

static void bstCopyTevDefaults(u32 address,const BstTevDefaults& defaults) {
    for (u32 i=0;i<6;++i) gabi::store<f32>(address+i*4,defaults.initial[i]);
    for (u32 i=0;i<4;++i) gabi::store<u8>(address+0x18+i,defaults.flags[i]);
    for (u32 i=0;i<4;++i) gabi::store<s16>(address+0x1C+i*2,defaults.angles[i]);
    for (u32 i=0;i<8;++i) gabi::store<f32>(address+0x24+i*4,defaults.tail[i]);
}

static bst_class* bstConstructor(bst_class* actor) {
    WWHD_FUNC(0x020E78C4,bst_class*,actor);
    if (!actor) actor=gabi::call<bst_class*>(0x0273AD10,0x3334);
    if (!actor) return nullptr;
    u32 address=gabi::ea(actor);
    gabi::call(0x025D4ED0,actor);
    gabi::store<u32>(address+0xB4,0x1000B81C);
    BstTevDefaults defaults;
    // The HD constructor keeps these defaults in registers across every
    // subobject constructor, including the smoke callback's constructor.
    for (u32 i=0;i<6;++i) {
        defaults.initial[i]=gabi::load<f32>(0x1016E414+i*4);
        gabi::store<f32>(address+0x3F4+i*4,defaults.initial[i]);
    }
    for (u32 i=0;i<4;++i) {
        defaults.flags[i]=gabi::load<u8>(0x1016E42C+i);
        gabi::store<u8>(address+0x40C+i,defaults.flags[i]);
    }
    for (u32 i=0;i<4;++i) {
        defaults.angles[i]=gabi::load<s16>(0x1016E430+i*2);
        gabi::store<s16>(address+0x410+i*2,defaults.angles[i]);
    }
    for (u32 i=0;i<8;++i) {
        defaults.tail[i]=gabi::load<f32>(0x1016E438+i*4);
        gabi::store<f32>(address+0x418+i*4,defaults.tail[i]);
    }
    bstCopyTevDefaults(address+0x4B4,defaults);
    bstCopyTevDefaults(address+0x538,defaults);
    gabi::call(0x028EFFD0,gabi::at<void>(address+0x724),10,0x12C,0x025166F0);
    gabi::call(0x024EFE94,gabi::at<void>(address+0x1364));
    gabi::call(0x024F0474,gabi::at<void>(address+0x13A4));
    gabi::store<u32>(address+0x13B4,0x1000B79C);
    gabi::store<u32>(address+0x13B8,0x1000B7BC);
    gabi::store<u32>(address+0x13C4,0x1000B7AC);
    gabi::store<u8>(address+0x13BC,1);
    gabi::call(0x0200BD2C,gabi::at<void>(address+0x1568));
    gabi::call(0x02515DA0,gabi::at<void>(address+0x1584));
    gabi::store<u32>(address+0x1580,0x1004AE88);
    gabi::store<u32>(address+0x1584,0x1004AEC0);
    gabi::call(0x02515FB8,gabi::at<void>(address+0x15A4));
    gabi::store<u32>(address+0x16B8,0x100015A8);
    gabi::store<u32>(address+0x16B4,0x1000B72C);
    gabi::call(0x02018590,gabi::at<void>(address+0x16BC));
    gabi::store<u32>(address+0x15E0,0x1004B108);
    gabi::store<u32>(address+0x16B8,0x1004B160);
    gabi::store<u32>(address+0x16D0,0x1004B150);
    gabi::call(0x02515FB8,gabi::at<void>(address+0x16D4));
    gabi::store<u32>(address+0x17E4,0x1000B72C);
    gabi::store<u32>(address+0x17E8,0x100015A8);
    gabi::call(0x02018590,gabi::at<void>(address+0x17EC));
    gabi::store<u32>(address+0x1800,0x1004B150);
    gabi::store<u32>(address+0x1710,0x1004B108);
    gabi::store<u32>(address+0x17E8,0x1004B160);
    gabi::call(0x028EFFD0,gabi::at<void>(address+0x1804),19,0x12C,0x025166F0);
    gabi::call(0x028EFFD0,gabi::at<void>(address+0x2E48),2,0x12C,0x025166F0);
    gabi::call(0x025A5B18,gabi::at<void>(address+0x3134),1);
    bstCopyTevDefaults(address+0x3154,defaults);
    bstCopyTevDefaults(address+0x3214,defaults);
    bstCopyTevDefaults(address+0x3298,defaults);
    return actor;
}
VERIFY(0x020E78C4,bstConstructor);

static void bstEmitterMatrix(u32 address,u32 emitter) {
    u32 morph=gabi::load<u32>(address+0x3D4);
    u32 model=gabi::load<u32>(morph+0x90);
    u32 block=gabi::load<u32>(model+0x2C);
    u16 flags=gabi::load<u16>(block+4);
    u32 matrix=gabi::load<u32>(block+0x10);
    gabi::store<u16>(block+4,flags|0x10);
    gabi::call(0x028249B0,gabi::at<void>(matrix),gabi::at<void>(emitter+0x1F0),gabi::at<void>(emitter+0x22C));
}

static void end_demo(bst_class* actor) {
    WWHD_FUNC(0x020EB174,void,actor);
    u32 address=gabi::ea(actor);
    gabi::store<u32>(address+0x39C,0);
    s16 mode=gabi::load<s16>(address+0x130E);
    gabi::store<s16>(address+0x133C,10);
    f32 speed=gabi::load<f32>(0x1000B860);
    switch (mode) {
    case 0:
        gabi::store<s16>(address+0x130E,(s16)(mode+1));
        [[fallthrough]];
    case 1: {
        u32 morph=gabi::load<u32>(address+0x3D4);
        f32 frame=gabi::load<f32>(morph+0x9C);
        if (gabi::ftoi(frame)==20 && address+0x37C) {
            s8 room=gabi::load<s8>(address+0x326);
            s8 reverb=gabi::call<s8>(0x02520540,room);
            gabi::call(0x025E1A40,0x698E,gabi::at<void>(address+0x37C),0,reverb);
        }
        break;
    }
    case 2: {
        anm_init(actor,13,speed,0,speed,-1);
        s16 value=gabi::load<s16>(address+0x130E);
        gabi::store<s16>(address+0x130E,(s16)(value+1));
        void* play=gabi::call<void*>(0x025200D4);
        u32 particles=gabi::load<u32>(gabi::ea(play)+0x5AB0);
        u32 emitter=gabi::call<u32>(0x025A847C,gabi::at<void>(particles),0,0x81E8,
                      gabi::at<cXyz>(address+0x314),nullptr,nullptr,255,nullptr,-1,0,nullptr,nullptr);
        gabi::store<u32>(address+0x3128,emitter);
        if (emitter) bstEmitterMatrix(address,emitter);
        break;
    }
    case 10: {
        u8 part=gabi::load<u8>(address+0x3D0);
        s32 index=gabi::load<u16>(part ? 0x10192870+part*2 : 0x10192878);
        f32 morph=gabi::load<f32>(0x1000B924);
        anm_init(actor,index,morph,part ? 0 : 2,speed,-1);
        s16 value=gabi::load<s16>(address+0x130E);
        gabi::store<s16>(address+0x130E,(s16)(value+1));
        [[fallthrough]];
    }
    case 11: {
        f32 factor=gabi::load<f32>(0x1000B8E4);
        f32 five=gabi::load<f32>(0x1000B8B8);
        for (u32 i=0;i<3;++i) {
            f32 limit=gabi::load<f32>(0x1047B634)+five;
            f32 target=gabi::load<f32>(address+0x2EC+i*4);
            gabi::call(0x0200ED84,gabi::at<void>(address+0x314+i*4),target,factor,limit);
        }
        s16 angle=gabi::load<s16>(address+0x2FA);
        gabi::call(0x0200F428,gabi::at<void>(address+0x32A),angle,10,128);
        angle=gabi::load<s16>(address+0x2F8);
        gabi::call(0x0200F428,gabi::at<void>(address+0x328),angle,10,128);
        angle=gabi::load<s16>(address+0x2FC);
        gabi::call(0x0200F428,gabi::at<void>(address+0x32C),angle,10,128);
        break;
    }
    }
    if (gabi::load<u8>(address+0x3D0)==0) {
        for (u32 i=0;i<4;++i) {
            u32 emitter=gabi::load<u32>(address+0x3118+i*4);
            if (!emitter) continue;
            if (gabi::load<s16>(address+0x1336)==1) {
                u32 flags=gabi::load<u32>(emitter+0x254);
                gabi::store<s32>(emitter+0x5C,-1);
                gabi::store<u32>(emitter+0x254,flags|1);
                gabi::store<u32>(address+0x3118+i*4,0);
            } else bstEmitterMatrix(address,emitter);
        }
    }
}
VERIFY(0x020EB174,end_demo);

static void beam_move(bst_class* actor) {
    WWHD_FUNC(0x020EDC8C,void,actor);
    u32 address=gabi::ea(actor);
    gabi::Local<dBgS_GndChk> ground;
    const dBgS_GndChk_vt groundVtable={0x1000B75C,0x1000B76C,0x1000B78C,0x1000B77C};
    dBgS_GndChk_ct(ground.get(),groundVtable,false);
    f32 zero=gabi::load<f32>(0x1000B854);
    f32 above=gabi::load<f32>(0x1000B8B0);
    f32 one=gabi::load<f32>(0x1000B860);
    f32 half=gabi::load<f32>(0x1000B86C);
    f32 five=gabi::load<f32>(0x1000B8B8);
    f32 fifty=gabi::load<f32>(0x1000B890);
    f32 ceiling=gabi::load<f32>(0x1000B930);
    for (u32 i=0;i<10;++i) {
        if (gabi::load<s8>(address+0x718+i)==0) continue;
        u32 position=address+0x5EC+i*12,velocity=address+0x664+i*12;
        gabi::call(0x028E8D88,gabi::at<void>(position),gabi::at<void>(velocity),gabi::at<void>(position));
        f32 z=gabi::load<f32>(position+8),y=gabi::load<f32>(position+4),x=gabi::load<f32>(position);
        u32 model=gabi::load<u32>(address+0x5C4+i*4);
        gabi::call(0x028E93CC,gabi::at<void>(0x1048D0CC),x,y,z);
        s16 yaw=gabi::load<s16>(address + 0x6DE + i*6);
        gabi::call(0x025F1C28,gabi::at<void>(0x1048D0CC),yaw);
        s16 pitch=gabi::load<s16>(address+0x6DC+i*6);
        gabi::call(0x025F1BF4,gabi::at<void>(0x1048D0CC),pitch);
        f32 length=gabi::load<f32>(address+0x12DC+i*4);
        gabi::call(0x025F2518,one,one,length);
        mtx_copy(gabi::at<Mtx34>(model+0xC8),gabi::at<Mtx34>(0x1048D0CC));
        f32 target=gabi::load<f32>(0x1047B620)+five;
        f32 step=gabi::load<f32>(0x1047B624)+half;
        gabi::call(0x0200ED84,gabi::at<void>(address+0x12DC+i*4),target,one,step);
        y=gabi::load<f32>(position+4);z=gabi::load<f32>(position+8);x=gabi::load<f32>(position);
        u32 groundAddress=gabi::ea(ground.get());
        gabi::store<f32>(groundAddress+0x2C,z);
        gabi::store<f32>(groundAddress+0x24,x);
        gabi::store<f32>(groundAddress+0x28,y+above);
        bool hit=false;
        void* play=gabi::call<void*>(0x025200D4);
        f32 height=gabi::call<f32>(0x02008974,gabi::at<void>(gabi::ea(play)+0x12A0),ground.get());
        y=gabi::load<f32>(position+4);
        if (!(y>height)) {
            gabi::store<f32>(position+4,height);
            beam_eff_set(gabi::at<cXyz>(position),0,0);
            hit=true;
        } else {
            gabi::Local<dBgS_LinChk> line;
            const dBgS_LinChk_vt lineVtable={0x1000B7CC,0x1000B7DC,0x1000B7FC,0x1000B7EC};
            dBgS_LinChk_ct(line.get(),lineVtable,false);
            gabi::Local<cXyz> difference,start,end;
            gabi::call(0x0201ADE0,gabi::at<void>(position),difference.get(),gabi::at<void>(velocity));
            start->x=(f32)difference->x;
            start->z=(f32)difference->z;
            f32 elevated=(f32)difference->y+fifty;
            start->y=elevated;
            end->copy(*gabi::at<cXyz>(position));
            end->y=elevated;
            gabi::call(0x024F1AFC,line.get(),start.get(),end.get(),nullptr);
            play=gabi::call<void*>(0x025200D4);
            BOOL crosses=gabi::call<BOOL>(0x02008860,gabi::at<void>(gabi::ea(play)+0x12A0),line.get());
            u32 lineAddress=gabi::ea(line.get());
            if (crosses) {
                gabi::at<cXyz>(position)->copy(*gabi::at<cXyz>(lineAddress+0x30));
            }
            gabi::store<u32>(lineAddress+0x58,0x1000B7FC);
            gabi::store<u32>(lineAddress+0x64,0x1000B74C);
            gabi::store<u32>(lineAddress+0x20,0x1000B73C);
            gabi::call(0x02008B4C,line.get(),0);
            if (crosses) {
                yaw=gabi::load<s16>(address + 0x6DE + i*6);
                beam_eff_set(gabi::at<cXyz>(position),yaw,1);
                hit=true;
            }
        }
        y=gabi::load<f32>(position+4);
        if (y>ceiling) gabi::store<s8>(address+0x718+i,0);
        if (hit) {
            gabi::store<s8>(address+0x718+i,0);
            play=gabi::call<void*>(0x025200D4);
            s32 strength=gabi::load<s16>(0x1047B68C)+3;
            gabi::Local<cXyz> impulse;
            impulse->z=zero;impulse->y=one;impulse->x=zero;
            gabi::call(0x025CB374,gabi::at<void>(gabi::ea(play)+0x599C),strength,-33,impulse.get());
            s8 room=gabi::load<s8>(address+0x326);
            s8 reverb=gabi::call<s8>(0x02520540,room);
            gabi::call(0x025E1A40,0x6986,gabi::at<void>(position),0,reverb);
        }
        u32 sphere=address+0x724+i*0x12C;
        if (gabi::load<s8>(address+0x718+i)==1) {
            gabi::call(0x025167C0,gabi::at<void>(sphere),gabi::at<void>(position));
            u8 state=gabi::load<u8>(address+0x718+i);
            gabi::store<u8>(address+0x718+i,(u8)(state+1));
        } else gabi::call(0x025167E4,gabi::at<void>(sphere),gabi::at<void>(position));
        play=gabi::call<void*>(0x025200D4);
        gabi::call(0x0200E240,gabi::at<void>(gabi::ea(play)+0x26A4),gabi::at<void>(sphere));
    }
    u32 groundAddress=gabi::ea(ground.get());
    gabi::store<u32>(groundAddress+0x40,0x1000B78C);
    gabi::store<u32>(groundAddress+0x4C,0x1000B74C);
    gabi::store<u32>(groundAddress+0x20,0x1000B76C);
    gabi::call(0x02008DAC,ground.get(),0);
}
VERIFY(0x020EDC8C,beam_move);

static BOOL daBst_Draw(bst_class* actor) {
    WWHD_FUNC(0x020E4A64,BOOL,actor);
    u32 address=gabi::ea(actor);
    if (gabi::load<u8>(address+0x3D0)) {
        u32 boss=gabi::load<u32>(0x10462988);
        s8 state=gabi::load<s8>(boss+0x30CE);
        if ((u32)((s32)state-1)<9) return TRUE;
    }
    u32 morph=gabi::load<u32>(address+0x3D4);
    u32 model=gabi::load<u32>(morph+0x90);
    void* env=gabi::call<void*>(0x02555D0C);
    gabi::call(0x025626A4,env,0,gabi::at<void>(address+0x314),gabi::at<void>(address+0x110));
    env=gabi::call<void*>(0x02555D0C);
    gabi::call(0x02562F5C,env,gabi::at<void>(model),gabi::at<void>(address+0x110));
    u32 animator=gabi::load<u32>(address+0x3D8);
    u32 data=gabi::load<u32>(model+0xAC);
    f32 frame=gabi::load<f32>(animator+4);
    gabi::call(0x025E7FC4,gabi::at<void>(animator),gabi::at<void>(data),frame);
    animator=gabi::load<u32>(address+0x3DC);data=gabi::load<u32>(model+0xAC);frame=gabi::load<f32>(animator+4);
    gabi::call(0x025E83FC,gabi::at<void>(animator),gabi::at<void>(data),frame);
    morph=gabi::load<u32>(address+0x3D4);
    gabi::call(0x025E5590,gabi::at<void>(morph));
    data=gabi::load<u32>(model+0xAC);gabi::store<u32>(data+0x44,0);
    data=gabi::load<u32>(model+0xAC);gabi::store<u32>(data+0x48,0);
    model=gabi::load<u32>(address+0x3E4);
    animator=gabi::load<u32>(address+0x3E8);data=gabi::load<u32>(model+0xAC);frame=gabi::load<f32>(animator+4);
    gabi::call(0x025E7FC4,gabi::at<void>(animator),gabi::at<void>(data),frame);
    animator=gabi::load<u32>(address+0x3EC);frame=gabi::load<f32>(animator+4);data=gabi::load<u32>(model+0xAC);
    gabi::call(0x025E83FC,gabi::at<void>(animator),gabi::at<void>(data),frame);
    env=gabi::call<void*>(0x02555D0C);
    gabi::call(0x025626A4,env,0,gabi::at<void>(address+0x2EC),gabi::at<void>(address+0x3F4));
    env=gabi::call<void*>(0x02555D0C);
    gabi::call(0x02562F5C,env,gabi::at<void>(model),gabi::at<void>(address+0x3F4));
    gabi::call(0x025E2DE0,gabi::at<void>(model),0);
    if (gabi::load<u8>(address+0x3D0)==0) {
        s16 blur=gabi::load<s16>(address+0x30CC);
        if (blur>1) {gabi::store<u8>(0x101F4826,(u8)blur);gabi::call(0x025F064C);}
        else if (blur==1) {gabi::store<s16>(address+0x30CC,0);gabi::store<u8>(0x101F4825,0);}
        for (u32 i=0;i<10;++i) if (gabi::load<s8>(address+0x718+i)) {
            u32 beam=gabi::load<u32>(address+0x5C4+i*4);
            gabi::call(0x025E2DE0,gabi::at<void>(beam),0);
        }
        if (gabi::load<u32>(0x104629B8)==0) {
            f32 zero=gabi::load<f32>(0x1000B854);
            gabi::store<u32>(0x104629B8,1);
            gabi::store<f32>(0x104629AC,zero);gabi::store<f32>(0x104629B4,zero);gabi::store<f32>(0x104629B0,zero);
        }
        env=gabi::call<void*>(0x02555D0C);
        gabi::call(0x025626A4,env,1,gabi::at<void>(0x104629AC),gabi::at<void>(address+0x3154));
        env=gabi::call<void*>(0x02555D0C);
        model=gabi::load<u32>(address+0x3328);
        gabi::call(0x02562F5C,env,gabi::at<void>(model),gabi::at<void>(address+0x3154));
        model=gabi::load<u32>(address+0x3328);animator=gabi::load<u32>(address+0x332C);
        data=gabi::load<u32>(model+0xAC);frame=gabi::load<f32>(animator+4);
        gabi::call(0x025E83FC,gabi::at<void>(animator),gabi::at<void>(data),frame);
        model=gabi::load<u32>(address+0x3328);gabi::call(0x025E2DE0,gabi::at<void>(model),0);
        env=gabi::call<void*>(0x02555D0C);model=gabi::load<u32>(address+0x331C);
        gabi::call(0x02562F5C,env,gabi::at<void>(model),gabi::at<void>(address+0x3154));
        model=gabi::load<u32>(address+0x331C);animator=gabi::load<u32>(address+0x3324);
        data=gabi::load<u32>(model+0xAC);frame=gabi::load<f32>(animator+4);
        gabi::call(0x025E83FC,gabi::at<void>(animator),gabi::at<void>(data),frame);
        model=gabi::load<u32>(address+0x331C);animator=gabi::load<u32>(address+0x3320);
        data=gabi::load<u32>(model+0xAC);frame=gabi::load<f32>(animator+4);
        gabi::call(0x025E7FC4,gabi::at<void>(animator),gabi::at<void>(data),frame);
        model=gabi::load<u32>(address+0x331C);gabi::call(0x025E2DE0,gabi::at<void>(model),0);
    }
    f32 one=gabi::load<f32>(0x1000B860);
    gabi::call(0x025BED80,201,actor,one,one,one);
    return TRUE;
}
VERIFY(0x020E4A64,daBst_Draw);

static u32 bstNewAnimator(bool texture) {
    void* result=gabi::call<void*>(0x0273AD10,texture ? 0x74 : 0x78);
    if (result) result=gabi::call<void*>(texture ? 0x025E7C6C : 0x025E80D0,result);
    return gabi::ea(result);
}

static BOOL bstInitAnimator(u32 actorAddress,u32 offset,u32 model,void* resource,bool texture,
                            f32 speed,s32 loop=0,s32 mode=0) {
    u32 controller=gabi::load<u32>(actorAddress+offset);
    return gabi::call<BOOL>(texture ? 0x025E7CE0 : 0x025E8154,gabi::at<void>(controller),
                gabi::at<void>(model),resource,1,loop,0,-1,mode,speed,0);
}

static BOOL useHeapInit(bst_class* actor) {
    WWHD_FUNC(0x020E7278,BOOL,actor);
    u32 address=gabi::ea(actor);
    u8 part=gabi::load<u8>(address+0x3D0);
    s32 index=gabi::load<u16>(0x10192860+part*2);
    void* modelData=bstResource(index,0x1000B8FC);
    part=gabi::load<u8>(address+0x3D0);
    index=gabi::load<u16>(0x10192870+part*2);
    void* animation=bstResource(index,0x1000B8FC);
    f32 speed=gabi::load<f32>(0x1000B860);
    u32 morph=gabi::call<u32>(0x025E4F64,nullptr,modelData,nullptr,nullptr,animation,2,0,-1,1,nullptr,0,0x11020203,speed);
    gabi::store<u32>(address+0x3D4,morph);
    if (!morph) return FALSE;
    if (!gabi::load<u32>(morph+0x90)) return FALSE;
    part=gabi::load<u8>(address+0x3D0);
    if (!part) {
        u32 model=gabi::load<u32>(morph+0x90);
        u32 data=gabi::load<u32>(model+0xAC);
        void* joints=gabi::call<void*>(0x027F3F94,gabi::at<void>(data));
        u16 count=gabi::load<u16>(gabi::ea(joints)+8);
        for (u16 i=0;i<count;++i) {
            if ((u32)(i-9u)<4) {
                morph=gabi::load<u32>(address+0x3D4);model=gabi::load<u32>(morph+0x90);data=gabi::load<u32>(model+0xAC);
                u32 limit=gabi::load<u32>(data+4);
                u32 joint=gabi::load<u32>(data+8);
                if (i<limit) joint+=i*0x1C;
                gabi::store<u32>(joint+8,0x020E48F8);
            }
            morph=gabi::load<u32>(address+0x3D4);model=gabi::load<u32>(morph+0x90);data=gabi::load<u32>(model+0xAC);
            joints=gabi::call<void*>(0x027F3F94,gabi::at<void>(data));
            count=gabi::load<u16>(gabi::ea(joints)+8);
        }
        morph=gabi::load<u32>(address+0x3D4);model=gabi::load<u32>(morph+0x90);
        gabi::store<u32>(model+0xB8,address);
        morph=gabi::load<u32>(address+0x3D4);
    }
    u32 model=gabi::load<u32>(morph+0x90);
    u32 data=gabi::load<u32>(model+0xAC);
    u32 created=gabi::call<u32>(0x025E38E0,gabi::at<void>(data),0,0x11020203);
    gabi::store<u32>(address+0x5BC,created);
    if (!created) return FALSE;
    u32 controller=bstNewAnimator(true);gabi::store<u32>(address+0x3D8,controller);
    if (!controller) return FALSE;
    part=gabi::load<u8>(address+0x3D0);
    morph=gabi::load<u32>(address+0x3D4);index=gabi::load<u16>(0x101928F0+part*2);model=gabi::load<u32>(morph+0x90);
    animation=bstResource(index,0x1000B8FC);
    controller=gabi::load<u32>(address+0x3D8);data=gabi::load<u32>(model+0xAC);
    if (!gabi::call<BOOL>(0x025E7CE0,gabi::at<void>(controller),gabi::at<void>(data),animation,1,0,0,-1,0,speed,0)) return FALSE;
    controller=bstNewAnimator(false);gabi::store<u32>(address+0x3DC,controller);
    if (!controller) return FALSE;
    part=gabi::load<u8>(address+0x3D0);morph=gabi::load<u32>(address+0x3D4);
    index=gabi::load<u16>(0x101928F8+part*2);model=gabi::load<u32>(morph+0x90);
    animation=bstResource(index,0x1000B8FC);
    controller=gabi::load<u32>(address+0x3DC);data=gabi::load<u32>(model+0xAC);
    if (!gabi::call<BOOL>(0x025E8154,gabi::at<void>(controller),gabi::at<void>(data),animation,1,0,0,-1,0,speed,0)) return FALSE;
    if (gabi::load<u8>(address+0x3D0)==0) {
        modelData=bstResource(0x32,0x1000B8FC);
        for (u32 i=0;i<10;++i) {
            created=gabi::call<u32>(0x025E38E0,modelData,0,0x11020203);
            gabi::store<u32>(address+0x5C4+i*4,created);
            if (!created) return FALSE;
        }
        modelData=bstResource(0x2D,0x1000B8FC);
        created=gabi::call<u32>(0x025E38E0,modelData,0,0x11020203);
        gabi::store<u32>(address+0x331C,created);
        if (!created) return FALSE;
        controller=bstNewAnimator(true);gabi::store<u32>(address+0x3320,controller);
        if (!controller) return FALSE;
        animation=bstResource(0x56,0x1000B8FC);
        if (!bstInitAnimator(address,0x3320,gabi::ea(modelData),animation,true,speed,2)) return FALSE;
        controller=bstNewAnimator(false);gabi::store<u32>(address+0x3324,controller);
        if (!controller) return FALSE;
        animation=bstResource(0x3B,0x1000B8FC);
        f32 zero=gabi::load<f32>(0x1000B854);
        if (!bstInitAnimator(address,0x3324,gabi::ea(modelData),animation,false,zero)) return FALSE;
        modelData=bstResource(0x2E,0x1000B8FC);
        created=gabi::call<u32>(0x025E38E0,modelData,0,0x11020203);gabi::store<u32>(address+0x3328,created);
        if (!created) return FALSE;
        controller=bstNewAnimator(false);gabi::store<u32>(address+0x332C,controller);
        if (!controller) return FALSE;
        animation=bstResource(0x3C,0x1000B8FC);
        if (!bstInitAnimator(address,0x332C,gabi::ea(modelData),animation,false,zero,2)) return FALSE;
    }
    part=gabi::load<u8>(address+0x3D0);index=gabi::load<u16>(0x10192868+part*2);
    modelData=bstResource(index,0x1000B8FC);
    created=gabi::call<u32>(0x025E38E0,modelData,0,0x11020203);gabi::store<u32>(address+0x3E4,created);
    if (!created) return FALSE;
    controller=bstNewAnimator(true);gabi::store<u32>(address+0x3E8,controller);
    if (!controller) return FALSE;
    part=gabi::load<u8>(address+0x3D0);index=gabi::load<u16>(0x10192900+part*2);animation=bstResource(index,0x1000B8FC);
    if (!bstInitAnimator(address,0x3E8,gabi::ea(modelData),animation,true,speed)) return FALSE;
    controller=bstNewAnimator(false);gabi::store<u32>(address+0x3EC,controller);
    if (!controller) return FALSE;
    part=gabi::load<u8>(address+0x3D0);index=gabi::load<u16>(0x10192908+part*2);animation=bstResource(index,0x1000B8FC);
    return bstInitAnimator(address,0x3EC,gabi::ea(modelData),animation,false,speed) ? TRUE : FALSE;
}
VERIFY(0x020E7278,useHeapInit);

static void bstDamageSound(u32 address,u32 sound,bool testActor) {
    if ((!testActor || address) && address+0x37C) {
        s8 room=gabi::load<s8>(address+0x326);
        s8 reverb=gabi::call<s8>(0x02520540,room);
        gabi::call(0x025E1A40,sound,gabi::at<void>(address+0x37C),0,reverb);
    }
}

static void bstDamageRecoil(u32 address,u32 player,s16 duration,f32 seventy,f32 zero,bool head) {
    f32 height=gabi::load<f32>(0x1047B63C)+seventy;
    gabi::store<f32>(address+0x1354,height);
    s16 timer=gabi::load<s16>(0x1047B68C);
    gabi::store<s16>(address+0x135C,(s16)(timer+duration));
    gabi::Local<cXyz> difference;
    gabi::call(0x0201ADE0,gabi::at<void>(address+0x314),difference.get(),gabi::at<void>(player+0x314));
    f32 x=difference->x,z=difference->z;
    s16 yaw=gabi::call<s16>(0x020195B0,x,z);
    z=difference->z;x=difference->x;
    f32 square=gabi::fmadds(x,x,z*z);
    gabi::store<s16>(address+0x1358,yaw);
    f32 distance=gabi::call<f32>(0x028F4384,square);
    f32 y=difference->y;
    s16 pitch=(s16)-gabi::call<s16>(0x020195B0,y,distance);
    if (head) {
        gabi::store<s16>(address+0x135A,pitch);
        gabi::store<f32>(address+0x370,zero);
    } else {
        gabi::store<f32>(address+0x370,zero);
        gabi::store<s16>(address+0x135A,pitch);
    }
}

static void bstDamageParticles(bst_class* actor,u32 impact,f32 two) {
    void* play=gabi::call<void*>(0x025200D4);
    u32 control=gabi::load<u32>(gabi::ea(play)+0x5AB0);
    gabi::call(0x025A847C,gabi::at<void>(control),0,16,gabi::at<void>(impact),nullptr,nullptr,255,nullptr,-1,0,nullptr,nullptr);
    gabi::Local<cXyz> scale;
    gabi::Local<csXyz> angle;
    scale->x=two;scale->y=two;scale->z=two;
    angle->x=0;angle->z=0;
    play=gabi::call<void*>(0x025200D4);
    u32 player=gabi::load<u32>(gabi::ea(play)+0x5B2C);
    angle->y=gabi::call<s16>(0x025D6894,actor,gabi::at<void>(player));
    play=gabi::call<void*>(0x025200D4);control=gabi::load<u32>(gabi::ea(play)+0x5AB0);
    gabi::call(0x025A847C,gabi::at<void>(control),0,13,gabi::at<void>(impact),angle.get(),scale.get(),255,nullptr,-1,0,nullptr,nullptr);
    gabi::Local<cXyz> flash;
    flash->x=gabi::load<f32>(impact);flash->y=gabi::load<f32>(impact+4);flash->z=gabi::load<f32>(impact+8);
    gabi::call(0x0255F554,flash.get(),1);
}

static void damage_check(bst_class* actor) {
    WWHD_FUNC(0x020EA6B4,void,actor);
    u32 address=gabi::ea(actor);
    void* play=gabi::call<void*>(0x025200D4);
    u32 player=gabi::load<u32>(gabi::ea(play)+0x5B2C);
    gabi::call(0x02515E50,gabi::at<void>(address+0x1584));
    gabi::Local<be<u32>[7]> hitInfo; /* CcAtInfo 0x1C: cc_at_check 025192A8 writes +0x18 (stack guard sweep 2026-10-05) */
    gabi::store<u32>(gabi::ea(hitInfo.get())+0x14,0);
    if (gabi::load<s16>(address+0x133A)==0) {
        bool found=false;
        u32 target=0;
        if (gabi::load<u8>(address+0x3D0)==0) {
            if (gabi::call<BOOL>(0x025162A4,gabi::at<void>(address+0x15A4))) {
                target=gabi::call<u32>(0x02516300,gabi::at<void>(address+0x15A4));found=true;
            }
        } else {
            for (u32 i=0;i<19;++i) {
                u32 sphere=address+0x1804+i*0x12C;
                if (gabi::call<BOOL>(0x025162A4,gabi::at<void>(sphere))) {
                    target=gabi::call<u32>(0x02516300,gabi::at<void>(sphere));found=true;
                }
            }
        }
        if (found) {
            gabi::store<u32>(gabi::ea(hitInfo.get()),target);
            gabi::call(0x02518CC8,actor,gabi::at<void>(target),0x42);
            gabi::store<s16>(address+0x133A,7);
        }
    }
    if (gabi::load<s16>(address+0x133C)) return;
    f32 seventy=gabi::load<f32>(0x1000B894);
    u8 part=gabi::load<u8>(address+0x3D0);
    f32 two=gabi::load<f32>(0x1000B924),zero=gabi::load<f32>(0x1000B854);
    if (!part) {
        u32 eyes=address+0x2E48;
        BOOL hit=gabi::call<BOOL>(0x025162A4,gabi::at<void>(eyes));
        if (!hit && !gabi::call<BOOL>(0x025162A4,gabi::at<void>(eyes+0x12C))) return;
        gabi::store<s16>(address+0x133C,10);
        if (!player_way_check(actor)) return;
        u32 eye=0,target;
        if (gabi::call<BOOL>(0x025162A4,gabi::at<void>(eyes))) target=gabi::call<u32>(0x02516300,gabi::at<void>(eyes));
        else {target=gabi::call<u32>(0x02516300,gabi::at<void>(eyes+0x12C));eye=1;}
        gabi::store<u32>(gabi::ea(hitInfo.get()),target);
        if (gabi::load<s8>(address+0x3108)!=1) gabi::store<s8>(address+0x3108,8);
        gabi::store<s16>(address+0x130E,0);gabi::store<s16>(address+0x130A,5);
        bstDamageSound(address,0x6985,true);
        s8 health=(s8)(gabi::load<s8>(address+0x30A6+eye)-1);
        gabi::store<s8>(address+0x30A6+eye,health);
        if (health<=0) {
            if (gabi::load<s8>(address+0x30A7-eye)<=0) {
                gabi::store<s16>(address+0x130E,0);gabi::store<s16>(address+0x130A,7);
            }
            bstDamageSound(address,0x6995,true);
        }
        bstDamageRecoil(address,player,7,seventy,zero,true);
        bstDamageParticles(actor,eyes+eye*0x12C+0xCC,two);
    } else {
        if (!gabi::call<BOOL>(0x025162A4,gabi::at<void>(address+0x16D4))) return;
        gabi::store<s16>(address+0x133C,10);
        if (!player_way_check(actor)) return;
        u32 target=gabi::call<u32>(0x02516300,gabi::at<void>(address+0x16D4));
        gabi::store<u32>(gabi::ea(hitInfo.get()),target);
        gabi::store<u32>(gabi::ea(hitInfo.get())+0x14,address+0x17A0);
        u32 result=gabi::call<u32>(0x025192A8,actor,hitInfo.get());
        gabi::store<u32>(gabi::ea(hitInfo.get())+4,result);
        bstDamageSound(address,0x6985,true);
        if (gabi::load<s8>(address+0x3A1)<=0) bstDamageSound(address,0x6995,false);
        bstDamageRecoil(address,player,15,seventy,zero,false);
        bstDamageParticles(actor,address+0x17A0,two);
        part=gabi::load<u8>(address+0x3D0);
        u32 other=2u-part;
        gabi::store<s16>(address+0x130A,5);gabi::store<s16>(address+0x130E,0);
        if (other<2) {
            u32 global=0x10462978+other*4;
            u32 hand=gabi::load<u32>(global);
            if (gabi::load<s16>(hand+0x130A)>=10) {
                gabi::store<s16>(hand+0x130A,1);
                hand=gabi::load<u32>(global);gabi::store<s16>(hand+0x130E,0);
                hand=gabi::load<u32>(global);gabi::store<f32>(hand+0x370,zero);
            }
        }
    }
}
VERIFY(0x020EA6B4,damage_check);

static void bstCopyActorLighting(u32 source,u32 destination) {
    // Memberwise assignment leaves the padding and unused HD light slots alone.
    const u32 floatRanges[][2]={{0,0x18},{0x24,0x44},{0xA8,0xB4},{0xC0,0xD8},{0xE4,0x104},{0x144,0x15C},{0x168,0x188}};
    const u32 byteRanges[][2]={{0x18,0x1C},{0xB4,0xBD},{0xD8,0xDC},{0x15C,0x160}};
    const u32 halfRanges[][2]={{0x1C,0x24},{0x90,0x98},{0xA0,0xA8},{0xDC,0xE4},{0x160,0x168}};
    const u32 wordRanges[][2]={{0x84,0x90},{0x98,0xA0}};
    for (const auto& range:floatRanges) for (u32 i=range[0];i<range[1];i+=4) gabi::store<f32>(destination+i,gabi::load<f32>(source+i));
    for (const auto& range:byteRanges) for (u32 i=range[0];i<range[1];++i) gabi::store<u8>(destination+i,gabi::load<u8>(source+i));
    for (const auto& range:halfRanges) for (u32 i=range[0];i<range[1];i+=2) gabi::store<u16>(destination+i,gabi::load<u16>(source+i));
    for (const auto& range:wordRanges) for (u32 i=range[0];i<range[1];i+=4) gabi::store<u32>(destination+i,gabi::load<u32>(source+i));
}

static s32 daBst_Create(bst_class* actor) {
    WWHD_FUNC(0x020E7DC0,s32,actor);
    u32 address=gabi::ea(actor);
    u32 status=gabi::load<u32>(address+0x2E4);
    if (!(status&8)) {
        if (actor) {bstConstructor(actor);status=gabi::load<u32>(address+0x2E4);}
        gabi::store<u32>(address+0x2E4,status|8);
    }
    s32 phase=gabi::call<s32>(0x02520460,gabi::at<void>(address+0x3C8),STR(0x1000B904));
    if (phase==4) {
        u32 parameters=gabi::load<u32>(address+0xB0);
        gabi::store<u8>(address+0x3146,1);gabi::store<u8>(address+0x3D0,(u8)parameters);
        if (!gabi::call<BOOL>(0x025D63E8,actor,0x020E7278,0x96000)) return 5;
        gabi::store<u32>(address+0x39C,4);gabi::store<u8>(address+0x38A,4);
        if (gabi::load<u8>(0x10192910)==0) {
            gabi::store<u8>(address+0x3331,1);gabi::store<u8>(0x10192910,1);
            u8 child=gabi::call<u8>(0x025F0A10,STR(0x1000B908),gabi::at<void>(0x10462980));
            gabi::store<u8>(0x10462980,child);
        }
        gabi::call(0x024F06B4,gabi::at<void>(address+0x13A4),gabi::at<void>(address+0x314),gabi::at<void>(address+0x300),actor,1,gabi::at<void>(address+0x1364),gabi::at<void>(address+0x33C),nullptr,nullptr);
        f32 radius=gabi::load<f32>(0x1000B8BC),height=gabi::load<f32>(0x1000B874);
        gabi::store<u8>(address+0x13B0,0);
        gabi::call(0x024EFF44,gabi::at<void>(address+0x1364),height,radius);
        u8 part=gabi::load<u8>(address+0x3D0);
        if (!part) {gabi::store<u32>(0x10462988,address);gabi::store<u32>(0x1046298C,0xFFFFFFFF);part=gabi::load<u8>(address+0x3D0);}
        if (part==1) {gabi::store<u32>(0x10462978,address);part=gabi::load<u8>(address+0x3D0);}
        if (part==2) {gabi::store<u32>(0x1046297C,address);part=gabi::load<u8>(address+0x3D0);}
        gabi::call(0x02515F14,gabi::at<void>(address+0x1568),part ? 230 : 255,255,actor);
        gabi::call(0x02516518,gabi::at<void>(address+0x16D4),gabi::at<void>(0x10192A68));
        part=gabi::load<u8>(address+0x3D0);gabi::store<u32>(address+0x1718,address+0x1568);
        if (!part) {
            gabi::call(0x02516518,gabi::at<void>(address+0x15A4),gabi::at<void>(0x10192A24));
            gabi::store<u32>(address+0x15E8,address+0x1568);
            for (u32 i=0;i<2;++i) {
                u32 sphere=address+0x2E48+i*0x12C;
                gabi::call(0x0251677C,gabi::at<void>(sphere),gabi::at<void>(0x101929A4));
                gabi::store<u32>(sphere+0x44,address+0x1568);
            }
            gabi::store<u8>(address+0x3A1,3);gabi::store<u8>(address+0x30A7,2);
            gabi::store<u8>(address+0x3A0,3);gabi::store<u8>(address+0x30A6,2);
            for (u32 i=0;i<2;++i) {
                s8 room=gabi::load<s8>(address+0x326);
                u32 attachment=gabi::call<u32>(0x025D5834,448,100,gabi::at<void>(address+0x314),room,nullptr,nullptr,-1,nullptr);
                gabi::store<u32>(address+0x30C4+i*4,attachment);
            }
            for (u32 i=0;i<10;++i) {
                u32 sphere=address+0x724+i*0x12C;
                gabi::call(0x0251677C,gabi::at<void>(sphere),gabi::at<void>(0x101929E4));
                gabi::store<u32>(sphere+0x44,address+0x1568);
            }
            u32 save=gabi::load<u32>(0x101F84DC);
            if (!gabi::call<BOOL>(0x025B9100,gabi::at<void>(save+0x798),3)) {
                save=gabi::load<u32>(0x101F84DC);
                if (gabi::call<BOOL>(0x025B9100,gabi::at<void>(save+0x798),5)) {
                    f32 one=gabi::load<f32>(0x1000B860),zero=gabi::load<f32>(0x1000B854);
                    gabi::store<f32>(address+0x3104,one);
                    gabi::Local<cXyz> center;center->x=zero;center->y=zero;center->z=zero;
                    gabi::store<u8>(address+0x3108,1);
                    void* play=gabi::call<void*>(0x025200D4);
                    u32 particles=gabi::load<u32>(gabi::ea(play)+0x5AB0);
                    u32 emitter=gabi::call<u32>(0x025A847C,gabi::at<void>(particles),0,0x81E9,center.get(),nullptr,nullptr,255,nullptr,-1,0,nullptr,nullptr);
                    gabi::store<u32>(address+0x312C,emitter);
                    u32 room=gabi::load<u32>(0x101FFC78);gabi::store<u8>(room+0x1E44,1);
                }
            }
        } else {
            for (u32 i=0;i<19;++i) {
                u32 sphere=address+0x1804+i*0x12C;
                gabi::call(0x0251677C,gabi::at<void>(sphere),gabi::at<void>(0x10192964));
                gabi::store<u32>(sphere+0x44,address+0x1568);
            }
            gabi::store<u8>(address+0x3A1,4);gabi::store<u8>(address+0x3A0,4);
        }
    }
    f32 spread=gabi::load<f32>(0x1000B900);
    s16 counter=(s16)gabi::ftoi(gabi::call<f32>(0x02019918,spread));
    bstCopyActorLighting(address+0x110,address+0x3F4);
    gabi::store<s16>(address+0x1308,counter);
    bstCopyActorLighting(address+0x110,address+0x3154);
    return phase;
}
VERIFY(0x020E7DC0,daBst_Create);
