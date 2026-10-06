/**
 * d_a_obj_firewall.cpp (WWHD)
 * Object - ring-shaped fire wall (daObjFirewall_c)
 *
 * Written from the WWHD code (all GameCube functions are stubs) with the
 * GameCube decompilation (zeldaret/tww src/d/actor/d_a_obj_firewall.cpp) as reference,
 * verified against cking.rpx.
 */
#include "bindings.h"
#include "d/actor/d_a_obj_firewall.h"
namespace {
    template<class T> T field(u32 actor,u32 offset) { return gabi::load<T>(actor+offset);
    }
    template<class T> void put(u32 actor,u32 offset,T value) { gabi::store<T>(actor+offset,value);
    }
    u32 play() { return gabi::call<u32>(0x025200D4);
    }
    void setAction(u32 actor,u32 action) {
        put<s16>(actor,0x680,0);
        put<s16>(actor,0x682,-1);
        put<u32>(actor,0x684,action);
    }
    void releaseBackground(u32 actor) {
        if (field<u32>(actor,0xf4)) {
            u32 bg=field<u32>(actor,0x524);
            if(bg) {
                if(gabi::load<u32>(bg)<0x100) { u32 world=play();
                    bg=field<u32>(actor,0x524);
                    gabi::call<void>(0x020087EC,world+0x12a0,bg);
                }
                put<u32>(actor,0x524,0);
            }
        }
    }
}

static void particle_delete(u32 actor) {
    WWHD_FUNC(0x02341BBC,void,actor);
    for(u32 i=0;i<12;++i) {
        u32 offset=0x614+4*i, emitter=field<u32>(actor,offset);
        if(emitter) { u32 flags=gabi::load<u32>(emitter+0x254);
            gabi::store<u32>(emitter+0x5c,~0u);
            gabi::store<u32>(emitter+0x254,flags|1);
            put<u32>(actor,offset,0);
        }
    }
}
VERIFY(0x02341BBC,particle_delete);

static void seStart(u32 actor,u32 sound) {
    WWHD_FUNC(0x02341C68,void,actor,sound);
    for(u32 i=0;i<8;++i) gabi::call<void>(0x025E19CC,sound,actor+0x68c+12*i);
}
VERIFY(0x02341C68,seStart);

static void set_se(u32 actor,u32 enabled) {
    WWHD_FUNC(0x02341CC0,void,actor,enabled);
    u8 old=field<u8>(actor,0x68a);
    if(old!=enabled) seStart(actor,old?0x6950:0x694e);
    else if(old) seStart(actor,0x614f);
    put<u8>(actor,0x68a,enabled);
}
VERIFY(0x02341CC0,set_se);

static void setup_put_the_fire_out(u32 actor) {
    WWHD_FUNC(0x02341D34,void,actor);
    if(field<u8>(actor,0x6ed)) {particle_delete(actor);
        releaseBackground(actor);
        set_se(actor,0);
        put<u8>(actor,0x6ed,0);
    }
}
VERIFY(0x02341D34,setup_put_the_fire_out);

static void seDelete(u32 actor) {
    WWHD_FUNC(0x02341DC4,void,actor);
    if(field<u8>(actor,0x6ec)) for(u32 i=0;i<8;++i) gabi::call<void>(0x025E1B34,actor+0x68c+12*i);
}
VERIFY(0x02341DC4,seDelete);

static BOOL firewall_delete(u32 actor) {
    WWHD_FUNC(0x02341E18,BOOL,actor);
    gabi::call<void>(0x025204C8,actor+0x3ac,0x10028a20u);
    releaseBackground(actor);
    setup_put_the_fire_out(actor);
    seDelete(actor);
    return TRUE;
}
VERIFY(0x02341E18,firewall_delete);

static BOOL firewall_draw(u32 actor) {
    WWHD_FUNC(0x0234218C,BOOL,actor);
    u32 model=field<u32>(actor,0x520);
    float frame=field<float>(actor,0x52c);
    u32 data=gabi::load<u32>(model+0xac);
    gabi::call<void>(0x025E7FC4,actor+0x528,data,frame);
    model=field<u32>(actor,0x520);
    frame=field<float>(actor,0x5a0);
    data=gabi::load<u32>(model+0xac);
    gabi::call<void>(0x025E83FC,actor+0x59c,data,frame);
    gabi::call<void>(0x025E2DE0,field<u32>(actor,0x520),0);
    return TRUE;
}
VERIFY(0x0234218C,firewall_draw);

static void retire_act_proc(u32 actor) {
    WWHD_FUNC(0x02342DD8,void,actor);
    gabi::call<void>(0x025E742C,actor+0x59c);
    if((field<u8>(actor,0x5ab)&1)||field<float>(actor,0x59c)==0) {
        u32 save=gabi::load<u32>(0x101f84dc);
        gabi::call<void>(0x025B8B68,save+0x644,0x2c01);
        gabi::call<void>(0x025D57E0,actor);
    }
}
VERIFY(0x02342DD8,retire_act_proc);

static void appear_act_proc(u32 actor) {
    WWHD_FUNC(0x02342B6C,void,actor);
    gabi::call<void>(0x025E742C,actor+0x59c);
    if((field<u8>(actor,0x5ab)&1)||field<float>(actor,0x59c)==0) {
        s16 event=field<s16>(actor,0x688);
        put<s16>(actor,0x680,0);
        put<float>(actor,0x67c,1);
        put<s16>(actor,0x682,-1);
        put<u32>(actor,0x684,event==-1?0x02342c88:0x02342c04);
    }
    set_se(actor,1);
}
VERIFY(0x02342B6C,appear_act_proc);

static void demo_end_wait_act_proc(u32 actor) {
    WWHD_FUNC(0x02342C04,void,actor);
    s16 event=field<s16>(actor,0x688);
    u32 world=play();
    if(gabi::call<u32>(0x025440C8,world+0x52c4,event)) {
        world=play();
        u16 flags=gabi::load<u16>(world+0x52b8);
        gabi::store<u16>(world+0x52b8,flags|8);
        setAction(actor,0x02342c88);
    }
    set_se(actor,1);
}
VERIFY(0x02342C04,demo_end_wait_act_proc);

static u32 particle_ctor(u32 particle) {
    WWHD_FUNC(0x02342EE4,u32,particle);
    if(!particle) particle=gabi::call<u32>(0x0273AD10,0x24);
    if(particle) put<float>(particle,0x20,1);
    return particle;
}
VERIFY(0x02342EE4,particle_ctor);

static void particle_dtor(u32 particle,u32 flags) {
    WWHD_FUNC(0x02342F24,void,particle,flags);
    if(particle&&(flags&1))gabi::call<void>(0x0273AF40,particle);
}
VERIFY(0x02342F24,particle_dtor);

static void firewall_dtor(u32 actor,u32 flags) {
    WWHD_FUNC(0x02342F38,void,actor,flags);
    if(actor) {gabi::call<void>(0x02515A70,actor+0x3f0,2);
        gabi::call<void>(0x02515860,actor+0x3b4,2);
        gabi::call<void>(0x025D50BC,actor,0);
        if(flags&1)gabi::call<void>(0x0273AF40,actor);
    }
}
VERIFY(0x02342F38,firewall_dtor);

static void empty_virtual() { WWHD_FUNC(0x02342FA4,void);
}
VERIFY(0x02342FA4,empty_virtual);

static u32 param_extract(u32 actor,u32 width,u32 shift) {
    WWHD_FUNC(0x02342FA8,u32,actor,width,shift);
    u32 params=field<u32>(actor,0xb0);
    u32 mask=(width&32)?0u:1u<<(width&31);
    return ((shift&32)?0u:params>>(shift&31))&(mask-1);
}
VERIFY(0x02342FA8,param_extract);

static void init_mtx(u32 actor) {
    WWHD_FUNC(0x023418D0,void,actor);
    float y=field<float>(actor,0x334),x=field<float>(actor,0x330);
    u32 model=field<u32>(actor,0x520);
    float z=field<float>(actor,0x338);
    put<float>(model,0xc0,y);
    put<float>(model,0xc4,z);
    put<float>(model,0xbc,x);
    gabi::call<void>(0x028E93CC,0x1048d0ccu,0.f,0.f,0.f);
    float matrix[12];
    for(u32 i=0;i<12;++i)matrix[i]=gabi::load<float>(0x1048d0cc+4*i);
    model=field<u32>(actor,0x520);
    for(u32 i=0;i<12;++i)put<float>(model,0xc8+4*i,matrix[i]);
}
VERIFY(0x023418D0,init_mtx);

static u32 firewall_ctor(u32 actor) {
    WWHD_FUNC(0x02341614,u32,actor);
    if(!actor)actor=gabi::call<u32>(0x0273AD10,0x6f8);
    if(actor) {
        gabi::call<void>(0x025D4ED0,actor);
        put<u32>(actor,0xb4,0x10028a74);
        gabi::call<void>(0x0200BD2C,actor+0x3b4);
        gabi::call<void>(0x02515DA0,actor+0x3d0);
        put<u32>(actor,0x3cc,0x1004ae88);
        put<u32>(actor,0x3d0,0x1004aec0);
        gabi::call<void>(0x02515FB8,actor+0x3f0);
        put<u32>(actor,0x504,0x100015a8);
        put<u32>(actor,0x500,0x10028a64);
        gabi::call<void>(0x02018590,actor+0x508);
        put<u32>(actor,0x42c,0x1004b108);
        put<u32>(actor,0x504,0x1004b160);
        put<u32>(actor,0x51c,0x1004b150);
        gabi::call<void>(0x025E7C6C,actor+0x528);
        gabi::call<void>(0x025E80D0,actor+0x59c);
        gabi::call<void>(0x028EFFD0,actor+0x648,1,0x24,0x02342ee4u);
    }return actor;
}
VERIFY(0x02341614,firewall_ctor);

static void firewall_static_init() {
    WWHD_FUNC(0x02342E44,void);
    gabi::store<u32>(0x10469b98,0);
    gabi::store<u32>(0x10469b90,0);
    gabi::store<u32>(0x10469b9c,0);
    gabi::store<u32>(0x10469b94,0);
    gabi::call<void>(0x028F026C,0x101c9418u);
    gabi::store<float>(0x10469b80,-3.1415927410125732f);
    gabi::store<float>(0x10469b84,3.1415927410125732f);
    gabi::call<void>(0x028ED6F8,0x10469b8cu);
    gabi::call<void>(0x028F026C,0x101c9424u);
    gabi::call<void>(0x028EAB2C,0x10469b8du);
    gabi::call<void>(0x028F026C,0x101c9430u);
    gabi::store<u32>(0x10469b88,0xe00);
}
VERIFY(0x02342E44,firewall_static_init);

static BOOL firewall_create(u32 actor) {
    WWHD_FUNC(0x023419A0,BOOL,actor);
    u32 condition=field<u32>(actor,0x2e4);
    if(!(condition&8)) {if(actor) {firewall_ctor(actor);
            condition=field<u32>(actor,0x2e4);
        }put<u32>(actor,0x2e4,condition|8);
    }
    s32 phase=gabi::call<s32>(0x02520460,actor+0x3ac,0x10028a20u);
    if(phase!=4)return phase;
    if(!gabi::call<u32>(0x025D63E8,actor,0x023418ccu,0x1420))return 5;
    u32 model=field<u32>(actor,0x520);
    put<u32>(actor,0x348,model?model+0xc8:0);
    init_mtx(actor);
    gabi::call<void>(0x02515F14,actor+0x3b4,0xff,0xff,actor);
    gabi::call<void>(0x02516518,actor+0x3f0,0x10028aa4u);
    u32 flags=field<u32>(actor,0x430);
    put<u32>(actor,0x434,actor+0x3b4);
    put<u32>(actor,0x430,flags|2);
    for(u32 i=0;i<8;++i) {
        float scale=field<float>(actor,0x330),sine=gabi::load<float>(0x104a44f8+0x2000*i),x=field<float>(actor,0x314);
        float sx=sine*1000.f,cosine=gabi::load<float>(0x104a44fc+0x2000*i);
        // Preserve PPC fmadds rounding and operand order. NaN payloads are not
        // selected here; real actor fields are four-byte aligned.
        float px=gabi::fmadds(sx,scale,x);
        float y=field<float>(actor,0x318),cz=cosine*1000.f,z=field<float>(actor,0x31c);
        put<float>(actor,0x68c+12*i,px);
        put<float>(actor,0x690+12*i,y);
        put<float>(actor,0x694+12*i,gabi::fmadds(cz,scale,z));
    }
    put<u8>(actor,0x6ec,1);
    u32 sw=param_extract(actor,8,0);
    put<u32>(actor,0x644,sw);
    u32 save=gabi::load<u32>(0x101f84dc);
    if(gabi::call<u32>(0x025B8B94,save+0x644,0x3520)==1) {
        put<u32>(actor,0x6f0,1);
        u32 name=gabi::load<u32>(0x101c9440),world=play();
        s32 event=gabi::call<s32>(0x02543F10,world+0x52c4,name,0xff);
        put<s16>(actor,0x688,event);
        setAction(actor,0x02342990);
    }else {put<s16>(actor,0x680,0);
        put<s16>(actor,0x688,-1);
        put<s16>(actor,0x682,-1);
        put<u32>(actor,0x6f0,0);
        put<u32>(actor,0x684,0x02342878);
    }
    return phase;
}
VERIFY(0x023419A0,firewall_create);

static BOOL Firewall_Create(u32 actor) {WWHD_FUNC(0x02341BB8,BOOL,actor);
    return firewall_create(actor);
}
VERIFY(0x02341BB8,Firewall_Create);

static BOOL Firewall_Delete(u32 actor) {WWHD_FUNC(0x02341EA0,BOOL,actor);
    return firewall_delete(actor);
}
VERIFY(0x02341EA0,Firewall_Delete);

static BOOL Firewall_Draw(u32 actor) {WWHD_FUNC(0x023421EC,BOOL,actor);
    return firewall_draw(actor);
}
VERIFY(0x023421EC,Firewall_Draw);

static BOOL Firewall_IsDelete(u32 actor) {WWHD_FUNC(0x023421F0,BOOL,actor);
    return TRUE;
}
VERIFY(0x023421F0,Firewall_IsDelete);

static void setPointLight(u32 actor) {
    WWHD_FUNC(0x02341EA4,void,actor);
    s16 index=field<s16>(actor,0x682);
    bool lit=false;
    if(index==-1&&field<s16>(actor,0x680)==0) {u32 action=field<u32>(actor,0x684);
        lit=action==0x02342c88||action==0x02342c04||action==0x02342b6c;
    }
    if(lit) {float random=gabi::call<float>(0x020198D8,0.5f);
        gabi::call<void>(0x0200ED84,actor+0x67c,random+1.f,0.5f,0.04f);
    }
    if(lit) {
        u32 y=field<u32>(actor,0x670),x=field<u32>(actor,0x66c);
        put<u16>(actor,0x654,500);
        put<u32>(actor,0x648,x);
        put<u32>(actor,0x64c,y);
        u32 z=field<u32>(actor,0x674);
        put<u16>(actor,0x656,200);
        put<u16>(actor,0x658,120);
        put<u32>(actor,0x650,z);
    }else {
        u32 z=field<u32>(actor,0x674),y=field<u32>(actor,0x670),x=field<u32>(actor,0x66c);
        put<float>(actor,0x67c,0);
        put<u32>(actor,0x648,x);
        put<u32>(actor,0x64c,y);
        put<u16>(actor,0x654,500);
        put<u16>(actor,0x656,200);
        put<u16>(actor,0x658,120);
        put<u32>(actor,0x650,z);
    }
    float radius=(gabi::load<float>(0x1047bcd0)+400.f)*field<float>(actor,0x67c);
    put<float>(actor,0x65c,float(s16(gabi::ftoi(radius))));
    put<float>(actor,0x660,250);
    float strength=field<float>(actor,0x67c);
    if(strength>1.f)gabi::call<void>(0x0255FDA4,0,3,1.f);
    else if(strength>0.5f)gabi::call<void>(0x0255FDA4,0,3,strength);
    else gabi::call<void>(0x0255FDA4,3,0,1.f-strength);
}
VERIFY(0x02341EA4,setPointLight);

static BOOL firewall_execute(u32 actor) {
    WWHD_FUNC(0x023420EC,BOOL,actor);
    u32 bg=field<u32>(actor,0x524);
    if(bg&&gabi::load<u32>(bg)<0x100)gabi::call<void>(0x024F43DC,bg);
    gabi::call<void>(0x025E742C,actor+0x528);
    setPointLight(actor);
    s16 index=field<s16>(actor,0x682),adjust=field<s16>(actor,0x680);
    u32 object=actor+adjust,target;
    if(index<0)target=field<u32>(actor,0x684);
    else {s16 offset=field<s16>(actor,0x686);
        u32 vtable=gabi::load<u32>(object+offset);
        target=gabi::load<u32>(vtable+8*index+4);
    }
    gabi::call_ptr<void>(target,object);
    return TRUE;
}
VERIFY(0x023420EC,firewall_execute);

static BOOL Firewall_Execute(u32 actor) {WWHD_FUNC(0x02342188,BOOL,actor);
    return firewall_execute(actor);
}
VERIFY(0x02342188,Firewall_Execute);

static BOOL create_heap(u32 actor) {
    WWHD_FUNC(0x02341704,BOOL,actor);
    auto resource=[](s32 index) {gabi::Local<SafeString> name;
        name->mStringTop=0x10028a20;
        name->__vtbl=0x10028a4c;
        u32 controller=gabi::load<u32>(0x101f4f28);
        return gabi::call<u32>(0x026066C4,controller,name.get(),index);
    };
    u32 data=resource(6),btk=resource(12),brk=resource(9);
    if(!data||!btk||!brk) {gabi::call<void>(0x0273AA24,0x10028af0u,0x171,0x10028aecu);
        return FALSE;
    }
    u32 model=gabi::call<u32>(0x025E38E0,data,0x80000,0x11000222);
    put<u32>(actor,0x520,model);
    u32 mesh=resource(15);
    model=field<u32>(actor,0x520);
    u32 bg=gabi::call<u32>(0x024F2478,mesh,1,model?model+0xc8:0);
    put<u32>(actor,0x524,bg);
    u32 btkOk=gabi::call<u32>(0x025E7CE0,actor+0x528,data,btk,1,2,1.f,0,-1,0,0);
    u32 brkOk=gabi::call<u32>(0x025E8154,actor+0x59c,data,brk,1,0,1.f,0,-1,0,0);
    return field<u32>(actor,0x520)&&field<u32>(actor,0x524)&&btkOk&&brkOk;
}
VERIFY(0x02341704,create_heap);

static BOOL solidHeapCB(u32 actor) {WWHD_FUNC(0x023418CC,BOOL,actor);
    return create_heap(actor);
}
VERIFY(0x023418CC,solidHeapCB);

static void particle_set(u32 actor) {
    WWHD_FUNC(0x023424AC,void,actor);
    gabi::Local<csXyz> rotation;
    gabi::call<void>(0x0201A478,rotation.get(),0,0,0);
    const s16 angles[11]={0x1555,s16(-0x1555),0x4000,s16(-0x4000),0x6aaa,s16(-0x6aaa),0,s16(-0x2aaa),0x2aaa,0x5555,s16(-0x5555)};
    for(u32 i=0;i<11;++i) {
        u32 offset=0x614+4*i;
        if(!field<u32>(actor,offset)) {
            rotation->y=angles[i];
            u32 world=play(),control=gabi::load<u32>(world+0x5ab0);
            u32 emitter=gabi::call<u32>(0x025A847C,control,0,i<6?0x82b6:0x82b7,actor+0x314,rotation.get(),0,0xff,0,-1,0,0,0);
            put<u32>(actor,offset,emitter);
        }
    }
    if(!field<u32>(actor,0x640)) {
        u32 world=play(),control=gabi::load<u32>(world+0x5ab0);
        u32 emitter=gabi::call<u32>(0x025A847C,control,0,0x82b8,actor+0x314,0,0,0xff,0,-1,0,0,0);
        put<u32>(actor,0x640,emitter);
    }
}
VERIFY(0x023424AC,particle_set);

static void setup_burn_up(u32 actor) {
    WWHD_FUNC(0x0234276C,void,actor);
    particle_set(actor);
    u32 bg=field<u32>(actor,0x524);
    if(bg&&gabi::load<u32>(bg)>=0x100) {u32 world=play();
        bg=field<u32>(actor,0x524);
        gabi::call<void>(0x024EEA6C,world+0x12a0,bg,actor);
    }
    set_se(actor,1);
    float scale=field<float>(actor,0x330),x=field<float>(actor,0x314),z=field<float>(actor,0x31c),y=field<float>(actor,0x318);
    float distance=1000.f*scale;
    gabi::call<void>(0x028E93CC,0x1048d0ccu,x,y,z);
    gabi::call<void>(0x025F1C28,0x1048d0ccu,0);
    gabi::call<void>(0x025F24E0,distance,0.f,0.f);
    put<float>(actor,0x66c,gabi::load<float>(0x1048d0d8));
    put<float>(actor,0x670,gabi::load<float>(0x1048d0e8));
    put<float>(actor,0x674,gabi::load<float>(0x1048d0f8));
    put<u32>(actor,0x684,0x02342b6c);
    put<s16>(actor,0x682,-1);
    put<s16>(actor,0x680,0);
    put<u8>(actor,0x6ed,1);
}
VERIFY(0x0234276C,setup_burn_up);

static bool sameGuestString(u32 left,u32 right) {
    for(u32 i=0;;++i) {u8 a=gabi::load<u8>(left+i),b=gabi::load<u8>(right+i);
        if(a!=b)return false;
        if(!a)return true;
    }
}

static void set_pl_se(u32 actor) {
    WWHD_FUNC(0x0234268C,void,actor);
    u32 world=play();
    s32 staff=gabi::call<s32>(0x02542D88,world+0x52c4,0x10028b4cu,0,0);
    if(staff==-1)return;
    world=play();
    u32 cut=gabi::call<u32>(0x02544830,world+0x52c4,staff);
    u32 index=field<u32>(actor,0x6f4),name=gabi::load<u32>(0x101c93e0+index*4);
    if(!sameGuestString(cut,name))return;
    world=play();
    u32 player=gabi::load<u32>(world+0x5b2c);
    if(!player)return;
    u32 vt=gabi::load<u32>(player+0xb4),target=gabi::load<u32>(vt+0xe4);
    index=field<u32>(actor,0x6f4);
    u32 sound=gabi::load<u32>(0x101c93ec+4*index);
    gabi::call_ptr<void>(target,player,sound);
    put<u32>(actor,0x6f4,field<u32>(actor,0x6f4)+1);
}
VERIFY(0x0234268C,set_pl_se);

static void wait_act_proc(u32 actor) {
    WWHD_FUNC(0x02342878,void,actor);
    u32 name=gabi::load<u32>(0x101c943c),manager=play()+0x52c4,world=play();
    s32 event=gabi::call<s32>(0x02543F10,world+0x52c4,name,0xff);
    if(!gabi::call<u32>(0x02544044,manager,event))return;
    world=play();
    s32 staff=gabi::call<s32>(0x02542D88,world+0x52c4,0x10028b54u,0,0);
    if(staff==-1)return;
    world=play();
    u32 cut=gabi::call<u32>(0x02544830,world+0x52c4,staff);
    if(sameGuestString(cut,0x10028b5c)) {u32 save=gabi::load<u32>(0x101f84dc);
        gabi::call<void>(0x025B8B68,save+0x644,0x3520);
        setup_burn_up(actor);
    }
    else set_pl_se(actor);
}
VERIFY(0x02342878,wait_act_proc);

static void wait3_act_proc(u32 actor) {
    WWHD_FUNC(0x02342A8C,void,actor);
    u32 name=gabi::load<u32>(0x101c9440),manager=play()+0x52c4,world=play();
    s32 event=gabi::call<s32>(0x02543F10,world+0x52c4,name,0xff);
    if(!gabi::call<u32>(0x02544044,manager,event))return;
    world=play();
    s32 staff=gabi::call<s32>(0x02542D88,world+0x52c4,0x10028b6cu,0,0);
    if(staff==-1)return;
    world=play();
    u32 cut=gabi::call<u32>(0x02544830,world+0x52c4,staff);
    if(sameGuestString(cut,0x10028b74))setup_burn_up(actor);
}
VERIFY(0x02342A8C,wait3_act_proc);

static void wait2_act_proc(u32 actor) {
    WWHD_FUNC(0x02342990,void,actor);
    u32 world=play(),player=gabi::load<u32>(world+0x5b2c);
    if(!player)return;
    gabi::Local<cXyz> delta,flat;
    gabi::call<void>(0x0201ADE0,player+0x314,delta.get(),actor+0x314);
    flat->x=delta->x;
    flat->y=0;
    flat->z=delta->z;
    float square=gabi::call<float>(0x028E8DD0,flat.get());
    float distance=gabi::call<float>(0x028F4384,square);
    if(distance<950.f&&field<float>(player,0x31c)<-7000.f) {
        if(field<u16>(actor,0xf8)==2) {put<s16>(actor,0x682,-1);
            put<u32>(actor,0x684,0x02342a8c);
            put<s16>(actor,0x680,0);
        }
        else {s16 event=field<s16>(actor,0x688);
            gabi::call<void>(0x025D7A58,actor,event,0xff,0xffff,0,1);
        }
    }
}
VERIFY(0x02342990,wait2_act_proc);

static void burn_wait_act_proc(u32 actor) {
    WWHD_FUNC(0x02342C88,void,actor);
    gabi::call<void>(0x02515E50,actor+0x3d0);
    gabi::call<void>(0x023421F8,actor);
    u32 sw=field<u32>(actor,0x644);
    if(sw!=0xff) {
        s8 room=field<s8>(actor,0x2fe);
        u32 save=gabi::load<u32>(0x101f84dc);
        if(gabi::call<u32>(0x025BA0C0,save+0x20,sw,room)==1) {
            save=gabi::load<u32>(0x101f84dc);
            gabi::call<void>(0x025B8B68,save+0x644,0x2c01);
            gabi::Local<SafeString> name;
            name->mStringTop=0x10028a20;
            name->__vtbl=0x10028a4c;
            u32 controller=gabi::load<u32>(0x101f4f28);
            u32 brk=gabi::call<u32>(0x026066C4,controller,name.get(),9);
            if(!brk)gabi::call<void>(0x0273AA24,0x10028b7cu,0x4e9,0x10028b94u);
            u32 model=field<u32>(actor,0x520),data=gabi::load<u32>(model+0xac);
            gabi::call<void>(0x025E8154,actor+0x59c,data,brk,1,0,-1.f,0,-1,1,0);
            setup_put_the_fire_out(actor);
            setAction(actor,0x02342dd8);
            return;
        }
    }
    set_se(actor,1);
}
VERIFY(0x02342C88,burn_wait_act_proc);

static void registCollisionTable(u32 actor) {
    WWHD_FUNC(0x023421F8,void,actor);
    u32 world=play(),player=gabi::load<u32>(world+0x5b2c);
    float ax=field<float>(actor,0x314),px=field<float>(player,0x314),az=field<float>(actor,0x31c),pz=field<float>(player,0x31c);
    s32 angle=gabi::call<s32>(0x020195B0,px-ax,pz-az);
    s32 absolute=std::abs(s32(s16(angle))),limit=gabi::load<s32>(0x10469b88);
    float inset=100;
    if(absolute<limit) {
        u16 theta=gabi::ftoi((float(absolute)/float(limit))*16384.f);
        float cosine=gabi::load<float>(0x104a44fc+8*(theta>>3));
        inset=gabi::fmadds(30.f,cosine,100.f);
    }else {
        for(u32 i=0;i<5;++i) {s16 base=gabi::load<s16>(0x101c93d4+2*i);
            s32 diff=std::abs(s32(s16(angle-base)));
            if(diff<0x1a00) {u16 theta=gabi::ftoi((float(diff)/6656.f)*16384.f);
                float cosine=gabi::load<float>(0x104a44fc+8*(theta>>3));
                inset=gabi::fmadds(115.f,cosine,100.f);
                if(inset==-1.f)inset=100.f;
                break;
            }
        }
    }
    gabi::Local<cXyz> center,delta;
    center->x=field<float>(actor,0x314);
    float z=field<float>(actor,0x31c),y=field<float>(actor,0x318);
    float scaleX=field<float>(actor,0x330),scaleY=field<float>(actor,0x334);
    center->y=y-300.f;
    center->z=z;
    float height=gabi::fmadds(10000.f,scaleY,300.f);
    gabi::call<void>(0x020182E0,actor+0x508,center.get());
    gabi::call<void>(0x020184DC,actor+0x508,gabi::fmsubs(1000.f,scaleX,inset));
    gabi::call<void>(0x02018428,actor+0x508,height);
    gabi::call<void>(0x0201ADE0,actor+0x314,delta.get(),player+0x314);
    u32 dx=gabi::load<u32>(gabi::ea(delta.get())),dy=gabi::load<u32>(gabi::ea(delta.get())+4);
    put<u32>(actor,0x46c,dx);
    u32 dz=gabi::load<u32>(gabi::ea(delta.get())+8);
    put<u32>(actor,0x470,dy);
    put<u32>(actor,0x474,dz);
    world=play();
    gabi::call<void>(0x0200E240,world+0x26a4,actor+0x3f0);
}
VERIFY(0x023421F8,registCollisionTable);
