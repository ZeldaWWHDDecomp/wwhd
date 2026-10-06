/* d_a_bpw.cpp (WWHD): Jalhalla (big Poe boss).
 * Ported from the GameCube decompilation (zeldaret/tww
 * src/d/actor/d_a_bpw.cpp) to the WWHD layout and code, verified against cking.rpx.
 * Field offsets are the GameCube ones + 0x11C (see d_a_bpw.h); comments name the GameCube
 * fields. "HD:" marks where WWHD's code differs from the GameCube source. */
#include "d/actor/d_a_bpw.h"
using namespace gabi;

namespace {
template<class T> T read(u32 a,u32 off=0) { return load<T>(a+off); }
template<class T> void write(u32 a,u32 off,T value) { store<T>(a+off,value); }
inline f32 add(f32 a,f32 b) { return fadds_ppc(a,b); }
inline f32 sub(f32 a,f32 b) { return fsubs_ppc(a,b); }
inline f32 mul(f32 a,f32 b) { return fmuls_ppc(a,b); }
inline void copy32(u32 dst,u32 doff,u32 src,u32 soff) { write<u32>(dst,doff,read<u32>(src,soff)); }
/* lfs/stfs copy (the load quiets a signalling NaN) */
inline void copyf(u32 dst,u32 doff,u32 src,u32 soff) { write<f32>(dst,doff,read<f32>(src,soff)); }
struct Vec { be<f32> x,y,z; };
struct SafeString { be<u32> data; be<u32> vtable; };
constexpr u32 calc_mtx_ptr=0x1018C7B0;   // mDoMtx_stack / calc_mtx pointer
constexpr u32 stack_now=0x1048D0CC;      // mDoMtx_stack_c::now
inline u32 dComIfGp_get() { return call<u32>(0x025200D4); }
inline u32 player_actor() { return read<u32>(dComIfGp_get(),0x5B2C); }
inline f32 cM_ssin(u16 angle) { return read<f32>(0x104A44F8,(u32)(angle>>3)*8); }
inline f32 cM_scos(u16 angle) { return read<f32>(0x104A44FC,(u32)(angle>>3)*8); }
/* dComIfG_getObjectRes("BPW", index): the archive name is a sead::SafeString temporary
 * {data, vtable}; every use has its own "BPW" literal (str) */
inline u32 getObjectRes(u32 str,u32 index) {
    Local<SafeString> name; name->data=str; name->vtable=0x1000B07C;
    return call<u32>(0x026066C4,read<u32>(0x101F4F28),name.get(),index);
}
}

/* ---- compiler-generated / out-of-line inline helpers of this TU ---- */

/* 020D60C0: array element constructor of m5E0 (dPa_smokeEcallBack(1)) */
u32 bpw_smoke_ctor(u32 object) {
    WWHD_FUNC(0x020D60C0,u32,object);
    return call<u32>(0x025A5B18,object,1);
}
VERIFY(0x020D60C0,bpw_smoke_ctor);

/* 020D6D84 / 020DEEAC: deleting destructors of empty-destructor classes */
void bpw_delete_6D84(u32 object,u32 flags) {
    WWHD_FUNC(0x020D6D84,void,object,flags);
    if(object && (flags&1)) call(0x0273AF40,object);
}
VERIFY(0x020D6D84,bpw_delete_6D84);

void bpw_smoke_dtor(u32 object,u32 flags) {
    WWHD_FUNC(0x020DEEAC,void,object,flags);
    if(object && (flags&1)) call(0x0273AF40,object);
}
VERIFY(0x020DEEAC,bpw_smoke_dtor);

/* 020DEFAC: empty virtual */
void bpw_empty_virtual(u32 object) {
    WWHD_FUNC(0x020DEFAC,void,object);
}
VERIFY(0x020DEFAC,bpw_empty_virtual);

/* 020D1280: GXColor -> four floats (/255) */
void bpw_color_to_float(u32 output,u32 color) {
    WWHD_FUNC(0x020D1280,void,output,color);
    f32 r=(f32)read<u8>(color),g=(f32)read<u8>(color,1);
    f32 b=(f32)read<u8>(color,2),a=(f32)read<u8>(color,3);
    write<f32>(output,0,r/255.f); write<f32>(output,4,g/255.f);
    write<f32>(output,8,b/255.f); write<f32>(output,12,a/255.f);
}
VERIFY(0x020D1280,bpw_color_to_float);

/* 020D6D18: fopAcM_seStart(actor, sfx, param) (HD: eyePos null check) */
void fopAcM_seStart(u32 actor,u32 sfx,u32 param) {
    WWHD_FUNC(0x020D6D18,void,actor,sfx,param);
    if(actor && actor+0x37C) {
        s32 reverb=call<s32>(0x02520540,(s32)read<s8>(actor,0x326));
        call(0x025E1A40,sfx,actor+0x37C,param,reverb);
    }
}
VERIFY(0x020D6D18,fopAcM_seStart);

/* 020D6C84: __sinit_d_a_bpw_cpp */
void bpw_sinit() {
    WWHD_FUNC(0x020D6C84,void);
    write<u32>(0x1046276C,8,0); write<u32>(0x1046276C,0,0);
    write<u32>(0x1046276C,12,0); write<u32>(0x1046276C,4,0);
    call(0x028F026C,0x1019266Cu);
    f32 low=read<f32>(0x1000B258),high=read<f32>(0x1000B25C);
    write<f32>(0x10462760,0,low); write<f32>(0x10462764,0,high);
    call(0x028ED6F8,0x10462768u); call(0x028F026C,0x10192678u);
    call(0x028EAB2C,0x10462769u); call(0x028F026C,0x10192684u);
}
VERIFY(0x020D6C84,bpw_sinit);

/* 020D60C8: bpw_class::bpw_class() (allocates when this == NULL) */
u32 bpw_class_ct(u32 a) {
    WWHD_FUNC(0x020D60C8,u32,a);
    if(!a) { a=call<u32>(0x0273AD10,0xE9Cu); if(!a) return 0; }
    call(0x025D4ED0,a);
    write<f32>(a,0x624,1.f); write<u32>(a,0xB4,0x1000B124);
    call(0x025A5B18,a+0x628,1);
    for(u32 o=0x648;o<=0x6E8;o+=0x14) call(0x025A5894,a+o,0,0);
    call(0x028EFFD0,a+0x6FC,4,0x20,0x020D60C0u);
    call(0x024EFE94,a+0x7A4); call(0x024F0474,a+0x7E4);
    write<u32>(a,0x7F8,0x1000B0D4); write<u32>(a,0x7F4,0x1000B0B4);
    write<u32>(a,0x804,0x1000B0C4); write<u8>(a,0x7FC,1);
    call(0x0200BD2C,a+0x9A8); call(0x02515DA0,a+0x9C4);
    write<u32>(a,0x9C0,0x1004AE88); write<u32>(a,0x9C4,0x1004AEC0);
    call(0x025166F0,a+0x9E4); call(0x025166F0,a+0xB10);
    call(0x025166F0,a+0xC3C); call(0x025166F0,a+0xD68);
    call(0x025E895C,a+0xE94);
    return a;
}
VERIFY(0x020D60C8,bpw_class_ct);

/* 020DEEC0: bpw_class::~bpw_class() (deleting) */
void bpw_class_dt(u32 a,u32 flags) {
    WWHD_FUNC(0x020DEEC0,void,a,flags);
    if(!a) return;
    call(0x025E89F8,a+0xE94,2);
    call(0x02515AE8,a+0xD68,2); call(0x02515AE8,a+0xC3C,2);
    call(0x02515AE8,a+0xB10,2); call(0x02515AE8,a+0x9E4,2);
    call(0x02515860,a+0x9A8,2);
    write<u32>(a,0x804,0x1000B0C4); write<u32>(a,0x7F8,0x1000B0D4);
    call(0x024EFD9C,a+0x7E4,0); call(0x02018034,a+0x7B8,2);
    call(0x028F0164,a+0x6FC,4,0x20,0x020DEEACu,0,0);
    call(0x025D50BC,a,0);
    if(flags&1) call(0x0273AF40,a);
}
VERIFY(0x020DEEC0,bpw_class_dt);

/* ---- d_a_bpw.cpp ---- */

/* 020D57A0 */
BOOL daBPW_IsDelete(bpw_class* i_this) {
    WWHD_FUNC(0x020D57A0,BOOL,i_this);
    return TRUE;
}
VERIFY(0x020D57A0,daBPW_IsDelete);

/* 020D57A8: HD: remove() is the callbacks' virtual at vtable+0x44 */
BOOL daBPW_Delete(bpw_class* i_this) {
    WWHD_FUNC(0x020D57A8,BOOL,i_this);
    u32 a=ea(i_this);
    call(0x025E1B34,a+0x474);   // mDoAud_seDeleteObject(&m358)
    call(0x025E1B34,a+0x4E0);   // mDoAud_seDeleteObject(&m3C4)
    call(0x025204C8,a+0x3C8,0x1000B228u);  // dComIfG_resDeleteDemo(&mPhase, "BPW")
    auto remove=[&](u32 o) { u32 vt=read<u32>(a,o); call_ptr<void>(read<u32>(vt,0x44),a+o); };
    if(read<u8>(a,0x4F8)==1) remove(0x6E8);    // m5CC
    for(u32 i=0;i<4;i++) remove(0x6FC+i*0x20); // m5E0[i]
    remove(0x684); remove(0x698); remove(0x6AC); remove(0x670);
    remove(0x6C0); remove(0x6D4); remove(0x65C); remove(0x648); remove(0x628);
    if(read<u8>(a,0x4F9)==1) call(0x0255A374,a+0x604);  // dKy_plight_cut(&mLightInfluence)
    if(read<s16>(a,0x57A)!=0) {   // mSomeCountdownTimers[8]: player->offConfuse()
        u32 player=player_actor();
        write<u32>(player,0x3BC,read<u32>(player,0x3BC)&~0x100u);
    }
    return TRUE;
}
VERIFY(0x020D57A8,daBPW_Delete);

/* 020D0FEC */
void draw_SUB(bpw_class* i_this) {
    WWHD_FUNC(0x020D0FEC,void,i_this);
    u32 a=ea(i_this),model=read<u32>(read<u32>(a,0x3D0),0x90);
    copyf(model,0xBC,a,0x330); copyf(model,0xC4,a,0x338); copyf(model,0xC0,a,0x334);
    f32 y=read<f32>(a,0x318),oz=read<f32>(a,0x4D0),ox=read<f32>(a,0x4C8),x=read<f32>(a,0x314);
    f32 oy=read<f32>(a,0x4CC),z=read<f32>(a,0x31C);
    call(0x028E93CC,stack_now,add(x,ox),add(y,oy),add(z,oz));
    call(0x025F1C28,stack_now,(s32)read<s16>(a,0x4F4));
    call(0x025F1BF4,stack_now,(s32)read<s16>(a,0x4F2));
    call(0x025F1C28,stack_now,(s32)(s16)-read<s16>(a,0x4F4));
    call(0x025F1C28,stack_now,(s32)read<s16>(a,0x32A));
    call(0x025F1BF4,stack_now,(s32)read<s16>(a,0x328));
    call(0x025F1C5C,stack_now,(s32)read<s16>(a,0x32C));
    f32 m[12]; for(u32 i=0;i<12;i++) m[i]=read<f32>(stack_now,i*4);
    for(u32 i=0;i<12;i++) write<f32>(model,0xC8+i*4,m[i]);
    u8 type=read<u8>(a,0x4F8);
    if(type==2) return;
    if(type!=3) call(0x025E55A0,read<u32>(a,0x3D0));
    u32 env=call<u32>(0x02555D0C);
    call(0x025626A4,env,0,a+0x314,a+0x110);
}
VERIFY(0x020D0FEC,draw_SUB);

/* 020D1158 */
void kantera_draw_SUB(bpw_class* i_this) {
    WWHD_FUNC(0x020D1158,void,i_this);
    u32 a=ea(i_this),model=read<u32>(read<u32>(a,0x3D0),0x90);
    copyf(model,0xBC,a,0x330); copyf(model,0xC4,a,0x338); copyf(model,0xC0,a,0x334);
    call(0x0200FAD8,read<f32>(a,0x314),read<f32>(a,0x318),read<f32>(a,0x31C),0);
    call(0x025F1C28,read<u32>(calc_mtx_ptr),(s32)read<s16>(a,0x32A));
    call(0x025F1BF4,read<u32>(calc_mtx_ptr),(s32)read<s16>(a,0x328));
    call(0x025F1C5C,read<u32>(calc_mtx_ptr),(s32)read<s16>(a,0x32C));
    call(0x0200FAD8,read<f32>(a,0x4C8),read<f32>(a,0x4CC),read<f32>(a,0x4D0),1);
    u32 mtx=read<u32>(calc_mtx_ptr);
    f32 m[12]; for(u32 i=0;i<12;i++) m[i]=read<f32>(mtx,i*4);
    for(u32 i=0;i<12;i++) write<f32>(model,0xC8+i*4,m[i]);
    call(0x025E55A0,read<u32>(a,0x3D0));
    u32 env=call<u32>(0x02555D0C);
    call(0x025626A4,env,0,a+0x314,a+0x110);
}
VERIFY(0x020D1158,kantera_draw_SUB);

/* 020D1A64 */
void anm_init(bpw_class* i_this,s32 bckFileIdx,f32 morf,u8 loopMode,f32 speed,s32 soundFileIdx) {
    WWHD_FUNC(0x020D1A64,void,i_this,bckFileIdx,morf,loopMode,speed,soundFileIdx);
    u32 a=ea(i_this);
    write<s32>(a,0x514,bckFileIdx);   // m3F8
    if(soundFileIdx>=0) {
        u32 bck=getObjectRes(0x1000B160,bckFileIdx);
        u32 snd=getObjectRes(0x1000B160,soundFileIdx);
        call(0x025E4A98,read<u32>(a,0x3D0),bck,(u32)loopMode,morf,speed,0.f,-1.f,snd);
    } else {
        u32 bck=getObjectRes(0x1000B160,bckFileIdx);
        call(0x025E4A98,read<u32>(a,0x3D0),bck,(u32)loopMode,morf,speed,0.f,-1.f,0u);
    }
}
VERIFY(0x020D1A64,anm_init);

/* 020D1B90: HD: keeps the boss in front of a z limit (global at 1047BBC4 + 1800) */
void BG_check(bpw_class* i_this) {
    WWHD_FUNC(0x020D1B90,void,i_this);
    u32 a=ea(i_this);
    call(0x024EFF44,a+0x7A4,read<f32>(a,0x5B4),read<f32>(a,0x5B8));  // mAcchCir.SetWall(m498, m49C)
    f32 off=read<f32>(a,0x5A8);   // m48C
    write<f32>(a,0x318,sub(read<f32>(a,0x318),off));
    write<f32>(a,0x304,sub(read<f32>(a,0x304),off));
    u32 play=dComIfGp_get();
    call(0x024F08A8,a+0x7E4,play+0x12A0);
    off=read<f32>(a,0x5A8);
    f32 y=read<f32>(a,0x318),oy=read<f32>(a,0x304);
    write<f32>(a,0x318,add(y,off)); write<f32>(a,0x304,add(oy,off));
    f32 lim=add(read<f32>(0x1047BBC4),1800.f);
    if(read<f32>(a,0x31C)>lim) {
        write<f32>(a,0x31C,lim);
        write<f32>(a,0x308,add(read<f32>(0x1047BBC4),1800.f));
    }
}
VERIFY(0x020D1B90,BG_check);

/* 020D1C40 */
void alpha_anime(bpw_class* i_this) {
    WWHD_FUNC(0x020D1C40,void,i_this);
    u32 a=ea(i_this);
    if(read<u8>(a,0x4FC)!=0) return;   // m3E0
    u32 play=dComIfGp_get();
    if(call<s32>(0x0252A038,play+0x5A20,a+0x314)) {   // dComIfGp_getDetect().chk_light(&current.pos)
        call(0x0200F428,a+0x59A,0,1,0x14);    // cLib_addCalcAngleS2(&m47E, 0, 1, 0x14)
        write<s16>(a,0x59C,0); write<s16>(a,0x594,0);
    } else {
        write<s16>(a,0x594,(s16)(read<s16>(a,0x594)+0x400));
        call(0x0200F428,a+0x59C,0x96,1,10);
        f32 base=(f32)read<s16>(a,0x59C);
        f32 v=fmadds(cM_ssin(read<u16>(a,0x594)),30.f,base);
        write<s16>(a,0x59A,(s16)ftoi(v));
    }
}
VERIFY(0x020D1C40,alpha_anime);

/* 020D1D3C: HD: calls dComIfGp_get() (result unused) */
void fuwafuwa_calc(bpw_class* i_this) {
    WWHD_FUNC(0x020D1D3C,void,i_this);
    u32 a=ea(i_this);
    dComIfGp_get();
    s16 t=(s16)(read<s16>(a,0x598)+700);   // m47C
    f32 ground=read<f32>(a,0x878);          // mAcch.GetGroundH()
    write<s16>(a,0x598,t);
    f32 y=add(ground,30.f);
    write<f32>(a,0x318,y);
    write<f32>(a,0x318,fmadds(cM_ssin((u16)t),30.f,y));
}
VERIFY(0x020D1D3C,fuwafuwa_calc);

/* tail of the joint callbacks: store calc_mtx back into the joint's anim matrix, then
 * J3DSys::mCurrentMtx (HD: J3DModel matrix block at modelData+0x10, dirty flag 0x10) */
static void bpw_joint_store(u32 model,u32 jnt) {
    u32 data=read<u32>(model,0x2C),mtx=read<u32>(calc_mtx_ptr);
    u16 flags=read<u16>(data,4); u32 mats=read<u32>(data,0x10);
    write<u16>(data,4,(u16)(flags|0x10));
    f32 m[12]; for(u32 i=0;i<12;i++) m[i]=read<f32>(mtx,i*4);
    for(u32 i=0;i<12;i++) write<f32>(mats+jnt*0x30,i*4,m[i]);
    call(0x028E90D4,read<u32>(calc_mtx_ptr),0x104B4868u);
}
static void bpw_joint_load(u32 model,u32 jnt) {
    u32 data=read<u32>(model,0x2C);
    u32 mats=read<u32>(data,0x10);
    write<u16>(data,4,(u16)(read<u16>(data,4)|0x10));
    call(0x028E90D4,mats+jnt*0x30,read<u32>(calc_mtx_ptr));
}

/* 020D0C04 */
BOOL body_nodeCallBack(u32 node,s32 calcTiming) {
    WWHD_FUNC(0x020D0C04,BOOL,node,calcTiming);
    if(calcTiming!=0) return TRUE;
    u32 joint=call<u32>(0x027F7878,node);
    u32 model=read<u32>(0x104B462C),a=read<u32>(model,0xB8);
    u32 jnt=read<u16>(joint,4);
    if(!a) return TRUE;
    if(!(jnt==0 || jnt==2 || jnt==0x1D || jnt==0x1E || jnt==0x25)) return TRUE;
    bpw_joint_load(model,jnt);
    Local<Vec> v;
    if(jnt==0) {          // BPW_ALLROOT
        v->x=0.f; v->y=read<f32>(a,0x5BC); v->z=0.f;
        call(0x0200FCD8,v.get(),a+0x474);  // m358
        v->x=0.f; v->y=-50.f; v->z=0.f;
        call(0x0200FCD8,v.get(),a+0x48C);  // m370
    } else {
        v->x=0.f; v->y=0.f; v->z=0.f;
        u32 dst=jnt==2?0x4B0:jnt==0x1D?0x4D4:jnt==0x1E?0x498:0x4E0;  // m394, m3B8, mChildActorPos, m3C4
        call(0x0200FCD8,v.get(),a+dst);
    }
    bpw_joint_store(model,jnt);
    return TRUE;
}
VERIFY(0x020D0C04,body_nodeCallBack);

/* 020D0DF0 */
BOOL kantera_nodeCallBack(u32 node,s32 calcTiming) {
    WWHD_FUNC(0x020D0DF0,BOOL,node,calcTiming);
    if(calcTiming!=0) return TRUE;
    u32 joint=call<u32>(0x027F7878,node);
    u32 model=read<u32>(0x104B462C),a=read<u32>(model,0xB8);
    u32 jnt=read<u16>(joint,4);
    if(!a) return TRUE;
    if(jnt!=0 && jnt!=1) return TRUE;
    bpw_joint_load(model,jnt);
    if(jnt==1) {          // BPW_KAN1_JNT_KANTERA
        call(0x025F1C28,read<u32>(calc_mtx_ptr),(s32)read<s16>(a,0x4EE));
        call(0x025F1BF4,read<u32>(calc_mtx_ptr),(s32)read<s16>(a,0x4EC));
        call(0x025F1C5C,read<u32>(calc_mtx_ptr),(s32)read<s16>(a,0x4F0));
        Local<Vec> v; v->x=0.f; v->y=-100.f; v->z=0.f;
        call(0x0200FCD8,v.get(),a+0x474);  // m358
    }
    bpw_joint_store(model,jnt);
    return TRUE;
}
VERIFY(0x020D0DF0,kantera_nodeCallBack);

/* 020D1DA8: HD: calls dComIfGp_get() first (result unused) */
void kankyou_hendou(bpw_class* i_this) {
    WWHD_FUNC(0x020D1DA8,void,i_this);
    u32 a=ea(i_this);
    dComIfGp_get();
    f32 target=0.f; u32 col=1;
    switch(read<u8>(a,0x507)) {   // mKankyouHendouState
    case 0: break;
    case 1: target=1.f; if(read<s16>(a,0x574)==1) write<u8>(a,0x507,0); break;
    case 2: target=1.f; col=2; if(read<s16>(a,0x574)==1) write<u8>(a,0x507,4); break;
    case 3: target=1.f; col=3; if(read<s16>(a,0x574)==1) write<u8>(a,0x507,5); break;
    case 4: col=2; break;
    case 5: col=3; break;
    case 6: target=1.f; col=0; break;
    default: break;
    }
    call(0x0200ED84,a+0x508,target,1.f,0.1f);   // cLib_addCalc2(&m3EC, ...)
    u32 base=read<u8>(a,0x507)<6?4:3;
    f32 v=read<f32>(a,0x508);
    if(v>0.5f) call(0x0255FDA4,base,col,v);
    else call(0x0255FDA4,col,base,sub(1.f,v));
}
VERIFY(0x020D1DA8,kankyou_hendou);

/* 020D2058 */
void noroi_check(bpw_class* i_this) {
    WWHD_FUNC(0x020D2058,void,i_this);
    u32 a=ea(i_this);
    u32 player=player_actor();
    if(read<u32>(player,0x3BC)&0x100) return;    // checkConfuse()
    if(read<s16>(a,0x59A)==0) return;            // m47E
    s16 t=read<s16>(a,0x568);
    if(t!=1 && t!=2 && t!=0) return;
    if(read<s16>(a,0x512)!=0 && read<u8>(a,0x502)==0 && read<s16>(a,0x560)!=2) {
        s16 st=read<s16>(a,0x562);
        if(st==10 || st==11 || st==12 || st==13) return;
        u32 anm=read<u32>(a,0x514);
        if(!(anm==0x1C || anm==0x1D || anm==0x2A || anm==0x2B)) return;
        u32 play=dComIfGp_get();
        f32 d=call<f32>(0x025D68EC,a,read<u32>(play+0x12A0,0x488C));
        if(d<530.f) { write<s16>(a,0x560,2); write<s16>(a,0x562,0x4D); }
        return;
    }
    s16 st=read<s16>(a,0x562);
    if(!(st==5 || st==6 || read<s16>(a,0x560)==2)) return;
    if(read<s16>(a,0x570)!=0) return;
    if(!call<s32>(0x025160DC,a+0xB10)) return;   // mBodyAtSph.ChkAtHit()
    u32 hit=call<u32>(0x02515BBC,a+0xB60);       // GetAtHitAc()
    if(!hit || hit!=player) return;
    write<u32>(a,0x9FC,read<u32>(a,0x9FC)&~1u);  // mBodyCoSph.OffTgSetBit()
    write<f32>(a,0x5AC,0.f); write<f32>(a,0x370,0.f);
    call(0x0251621C,a+0x9E4);                    // mBodyCoSph.ClrTgHit()
    write<u32>(player,0x3BC,read<u32>(player,0x3BC)|0x100);  // onConfuse()
    struct SVec { be<u16> x,y,z; };
    Local<SVec> ang;
    ang->x=read<u16>(a,0x328); ang->y=read<u16>(a,0x32A);
    write<s16>(a,0x570,0x14);
    ang->z=read<u16>(a,0x32C);
    s32 camId=read<s8>(dComIfGp_get(),0x5B30);
    u32 play=dComIfGp_get();
    u32 camera=read<u32>(play+camId*0x34,0x5AF8);
    copyf(a,0x77C,player,0x3D8); copyf(a,0x784,player,0x3E0); copyf(a,0x780,player,0x3DC);  // mFire1DousaPos = getHeadTopPos()
    play=dComIfGp_get();
    call(0x025A847C,read<u32>(play,0x5AB0),0,0x27B,a+0x77C,0,0,0xFF,0,-1,0,0,0);
    ang->y=read<u16>(camera,0x236);              // fopCamM_GetAngleY
    call(0x025D5834,0xD3,3,a+0x77C,(s32)read<s8>(a,0x326),ang.get(),0,-1,0);
}
VERIFY(0x020D2058,noroi_check);

/* 020D22B8 */
void fire_and_emitter_clear(bpw_class* i_this) {
    WWHD_FUNC(0x020D22B8,void,i_this);
    u32 a=ea(i_this);
    u32 id=read<u32>(a,0x51C);   // m400
    if(id!=0xFFFFFFFF) {
        Local<be<u32>> key; key->set(id);
        u32 other=call<u32>(0x025D5218,0x025E1234u,key.get());   // fopAcM_SearchByID
        if(other && read<u8>(other,0x4F9)==0) {
            call(0x025D57E0,other);
            write<u32>(a,0x51C,0xFFFFFFFF);
        }
    }
    call(0x025A5AC8,a+0x684); call(0x025A5AC8,a+0x698); call(0x025A5AC8,a+0x6AC);
    call(0x025A5AC8,a+0x65C); call(0x025A5AC8,a+0x648);
    call(0x025A5F88,a+0x628);
}
VERIFY(0x020D22B8,fire_and_emitter_clear);

static void brk_init(u32 a,u32 brkOff,u32 model,u32 str,u32 index) {
    u32 res=getObjectRes(str,index);
    u32 brk=read<u32>(a,brkOff),md=read<u32>(model,0xAC);
    call(0x025E8154,brk,md,res,1,0,1.f,0,-1,1,0);   // mDoExt_brkAnm::init(modelData, res, true, NONE, 1.0, 0, -1, true, 0)
}

/* 020D2350: HD: calls dComIfGp_get() first (result unused) */
void noroi_brk_check(bpw_class* i_this,u8 mode) {
    WWHD_FUNC(0x020D2350,void,i_this,mode);
    u32 a=ea(i_this);
    dComIfGp_get();
    u32 model=read<u32>(read<u32>(a,0x3D0),0x90);
    switch(mode) {
    case 0:
        if(read<u8>(a,0x4FE)!=2) { write<u8>(a,0x4FE,2); brk_init(a,0x3E4,model,0x1000B180,0x37); }
        break;
    case 1:
        if(read<u8>(a,0x4FE)==2) { write<u8>(a,0x4FE,1); brk_init(a,0x3E8,model,0x1000B180,0x36); }
        break;
    case 2:
        write<u8>(a,0x4FE,0); brk_init(a,0x3EC,model,0x1000B180,0x33);
        break;
    }
}
VERIFY(0x020D2350,noroi_brk_check);

/* 020D24F8: HD: calls dComIfGp_get() first (result unused) */
BOOL next_att_wait_check(bpw_class* i_this) {
    WWHD_FUNC(0x020D24F8,BOOL,i_this);
    u32 a=ea(i_this);
    dComIfGp_get();
    s16 t=read<s16>(a,0x568);
    if(t!=1 && t!=2 && t!=0) { write<s16>(a,0x560,0); write<s16>(a,0x562,10); return TRUE; }
    write<s16>(a,0x560,0); write<s16>(a,0x562,0); write<s16>(a,0x568,1);
    return FALSE;
}
VERIFY(0x020D24F8,next_att_wait_check);

/* 020D257C: HD: calls dComIfGp_get() first (result unused) */
void next_status_clear(bpw_class* i_this,u8 keepAlpha) {
    WWHD_FUNC(0x020D257C,void,i_this,keepAlpha);
    u32 a=ea(i_this);
    dComIfGp_get();
    write<f32>(a,0x374,0.f);                      // gravity
    write<f32>(a,0x33C,0.f); write<f32>(a,0x340,0.f); write<f32>(a,0x344,0.f);  // speed
    write<f32>(a,0x370,0.f); write<f32>(a,0x5AC,0.f);  // speedF, m490
    write<u8>(a,0x502,0); write<u8>(a,0x501,0);   // m3E6, m3E5
    write<f32>(a,0x5C4,add(read<f32>(0x1047BAD0),220.f));  // m4A8 = REG8_F(16) + 220
    write<f32>(a,0x4CC,340.f);                    // m3AC.y
    noroi_brk_check(i_this,1);
    u8 e1=read<u8>(a,0x4FD);
    write<s16>(a,0x328,0);
    write<f32>(a,0x5C0,300.f);                    // m4A4
    if(keepAlpha==0) { write<s16>(a,0x59C,0); write<s16>(a,0x594,0); write<u8>(a,0x4FC,0); }
    if(e1!=0) {
        u32 model=read<u32>(read<u32>(a,0x3D0),0x90);
        write<u8>(a,0x4FD,0);
        brk_init(a,0x3EC,model,0x1000B198,0x33);
    }
    if(read<s16>(a,0x568)==0) write<s16>(a,0x568,1);
    write<u32>(a,0xA78,read<u32>(a,0xA78)&~1u);   // mBodyCoSph.OffTgShield()
    write<u32>(a,0xB10,read<u32>(a,0xB10)|1u);    // mBodyAtSph.OnAtSPrmBit(Set)
    write<u32>(a,0xB14,1);                        // mBodyAtSph.OnAtHitBit()
    write<u32>(a,0x9FC,read<u32>(a,0x9FC)|1u);    // mBodyCoSph.OnTgSetBit()
    write<u32>(a,0xA10,read<u32>(a,0xA10)&~1u);   // mBodyCoSph.ClrCoSet()
    write<f32>(a,0x5B4,350.f); write<f32>(a,0x5B8,400.f);
    write<s16>(a,0x596,0x500);                    // m47A
}
VERIFY(0x020D257C,next_status_clear);

static u32 save_info() { return read<u32>(0x101F84DC)+0x20; }
static void onSwitch(u32 a,s32 no) { call(0x025B9E38,save_info(),no,(s32)read<s8>(a,0x326)); }
static void offSwitch(u32 a,s32 no) { call(0x025B9F7C,save_info(),no,(s32)read<s8>(a,0x326)); }
constexpr u32 REG0=0x1047B608;   // HIO register block (REG0_S(n) at +0x80+2n)

/* 020D271C: HD: dComIfGp_get() first (unused); timer 240 + rnd(120) (GameCube USA values) */
void light_on_off(bpw_class* i_this) {
    WWHD_FUNC(0x020D271C,void,i_this);
    u32 a=ea(i_this);
    dComIfGp_get();
    s32 idx=ftoi(call<f32>(0x020198D8,2.99f));
    s16 state=read<s16>(a,0x512);   // m3F6
    if(state==0) return;
    u8 sw=read<u8>(a,0x4FA);        // mLightState
    if(state==3) { onSwitch(a,sw); onSwitch(a,sw+1); onSwitch(a,sw+2); return; }
    if(state==4) { offSwitch(a,sw); offSwitch(a,sw+1); offSwitch(a,sw+2); return; }
    if(read<s16>(a,0x578)!=0) return;   // mSomeCountdownTimers[7]
    idx=ftoi(mul((f32)idx,3.f));
    constexpr u32 light_on_dt=0x10192658;
    s32 no=sw+read<s16>(light_on_dt+idx*2);
    if(state==2) onSwitch(a,no); else offSwitch(a,no);
    if(read<s16>(REG0+0x82)!=0) offSwitch(a,no);
    idx++;
    no=read<u8>(a,0x4FA)+read<s16>(light_on_dt+idx*2);
    if(read<s16>(a,0x512)==2) onSwitch(a,no); else offSwitch(a,no);
    if(read<s16>(REG0+0x82)!=0) offSwitch(a,no);
    idx++;
    no=read<u8>(a,0x4FA)+read<s16>(light_on_dt+idx*2);
    if(read<s16>(a,0x512)==2) offSwitch(a,no); else onSwitch(a,no);
    if(read<s16>(REG0+0x82)!=0) offSwitch(a,no);
    s32 t=read<s16>(REG0+0x88)+240;
    write<s16>(a,0x578,(s16)t);
    f32 r=call<f32>(0x020198D8,(f32)(read<s16>(REG0+0x8A)+120));
    write<s16>(a,0x578,(s16)(t+(s16)ftoi(r)));
}
VERIFY(0x020D271C,light_on_off);

constexpr u32 get_check_count=0x1046275C, check_info=0x1046277C;

/* 020D2A48 */
u32 skull_search_sub(u32 actor,u32 unused) {
    WWHD_FUNC(0x020D2A48,u32,actor,unused);
    if(read<s32>(get_check_count)>=100) return 0;
    if(!call<s32>(0x025D4604,actor)) return 0;   // fopAc_IsActor
    if(!actor || read<s16>(actor,8)!=0xCF) return 0;   // fpcNm_BL_e (skull)
    if(read<s8>(actor,0x3A1)<=0) return 0;
    s32 n=read<s32>(get_check_count);
    write<s32>(get_check_count,0,n+1);
    write<u32>(check_info+n*4,0,actor);
    return 0;
}
VERIFY(0x020D2A48,skull_search_sub);

/* 020D2AD4 */
void search_get_skull(bpw_class* i_this,u8 mode) {
    WWHD_FUNC(0x020D2AD4,void,i_this,mode);
    u32 a=ea(i_this);
    write<s32>(get_check_count,0,0);
    call(0x025DE508,0x020D2A48u,a);   // fpcM_Search(skull_search_sub, i_this)
    s32 n=read<s32>(get_check_count);
    if(n==0) return;
    for(s32 i=0;i<n;) {
        u32 sk=read<u32>(check_info+i*4);
        if(read<u32>(sk,0x2E0)&0x2000) return;   // fopAcM_checkCarryNow
        if(mode==0) {
            f32 d=call<f32>(0x025D68EC,a,sk);
            if(d>300.f) {
                s16 ang=call<s16>(0x025D6894,a,sk);
                s16 diff=call<s16>(0x0200FAAC,(s32)read<s16>(a,0x322),(s32)ang);
                if(diff<22000) {
                    write<s16>(sk,0x322,call<s16>(0x025D6894,sk,a));
                    f32 r=call<f32>(0x02019918,5.f);
                    write<f32>(sk,0x370,add(r,10.f));
                    u32 id=sk?read<u32>(sk,4):0xFFFFFFFFu;
                    write<f32>(sk,0x340,0.f);
                    write<s16>(sk,0x328,(s16)(read<s16>(sk,0x328)-((id&3)*0x300+0x1000)));
                }
            } else write<f32>(sk,0x370,0.f);
        } else if(mode==1) {
            s16 ang=call<s16>(0x025D6894,a,sk);
            s16 diff=call<s16>(0x0200FAAC,(s32)read<s16>(a,0x322),(s32)ang);
            if(diff<22000) {
                if(read<s16>(a,0x562)==0x35) {
                    f32 r=call<f32>(0x02019918,3.f);
                    f32 vy=read<f32>(sk,0x340);
                    write<f32>(sk,0x370,add(r,3.f));
                    if(vy>20.f) write<f32>(sk,0x340,20.f);
                } else {
                    f32 r=call<f32>(0x02019918,5.f);
                    write<f32>(sk,0x370,add(r,20.f));
                    f32 r2=call<f32>(0x020198D8,5.f);
                    f32 vy=add(read<f32>(sk,0x340),add(r2,5.f));
                    write<f32>(sk,0x340,vy);
                    if(vy>20.f) write<f32>(sk,0x340,20.f);
                }
                s16 ang2=call<s16>(0x025D6894,a,sk);
                write<s16>(sk,0x322,ang2);
                f32 r=call<f32>(0x02019918,4096.f);
                s16 sx=read<s16>(sk,0x328);
                write<s16>(sk,0x322,(s16)(ang2+(s16)ftoi(r)));
                write<s16>(sk,0x328,(s16)(sx-0x2000));
            }
        } else if(mode==2) {
            write<f32>(sk,0x370,0.f);
        } else { i++; continue; }   // n not re-read
        n=read<s32>(get_check_count);
        i++;
    }
}
VERIFY(0x020D2AD4,search_get_skull);

/* 020D2DAC: HD: |speedF| read before the distance check */
void maai_sub(bpw_class* i_this) {
    WWHD_FUNC(0x020D2DAC,void,i_this);
    u32 a=ea(i_this);
    u32 player=player_actor();
    f32 spd=__builtin_fabsf(read<f32>(player,0x370));
    f32 d=call<f32>(0x025D68EC,a,player_actor());
    if(d>500.f) return;
    u32 mtx=read<u32>(calc_mtx_ptr);
    s16 ang=call<s16>(0x025D6894,a,player_actor());
    call(0x025F1884,mtx,(s32)ang);
    Local<Vec> v,out;
    v->y=0.f; v->z=-500.f; v->x=0.f;
    call(0x0200FCD8,v.get(),out.get());
    call(0x028E8D88,out.get(),player+0x314,out.get());
    spd=mul(spd,1.5f);
    call(0x0200ED84,a+0x314,(f32)out->x,1.f,spd);
    call(0x0200ED84,a+0x31C,(f32)out->z,1.f,spd);
}
VERIFY(0x020D2DAC,maai_sub);

/* 020D2ED8 */
s32 kantera_pos_search(bpw_class* i_this) {
    WWHD_FUNC(0x020D2ED8,s32,i_this);
    u32 a=ea(i_this);
    u32 id=read<u32>(a,0x518);   // m3FC
    if(id==0xFFFFFFFF) return 0;
    Local<be<u32>> key; key->set(id);
    u32 k=call<u32>(0x025D5218,0x025E1234u,key.get());
    if(!k) return 0;
    f32 dx=sub(read<f32>(k,0x314),read<f32>(a,0x468));
    f32 dz=sub(read<f32>(k,0x31C),read<f32>(a,0x470));
    u32 mtx=read<u32>(calc_mtx_ptr);
    s16 ang=call<s16>(0x020195B0,dx,dz);
    call(0x025F1884,mtx,(s32)ang);
    Local<Vec> v,out;
    v->x=-120.f; v->y=-20.f; v->z=-320.f;
    call(0x0200FCD8,v.get(),out.get());
    call(0x028E8D88,out.get(),k+0x314,out.get());
    call(0x0200ED84,a+0x314,(f32)out->x,1.f,120.f);
    call(0x0200ED84,a+0x318,(f32)out->y,1.f,120.f);
    call(0x0200ED84,a+0x31C,(f32)out->z,1.f,120.f);
    s16 ang2=call<s16>(0x020195B0,dx,dz);
    f32 ey=sub(read<f32>(a,0x318),(f32)out->y);
    f32 ex=sub(read<f32>(a,0x314),(f32)out->x);
    f32 ez=sub(read<f32>(a,0x31C),(f32)out->z);
    f32 s=fmadds(ez,ez,fmadds(ex,ex,mul(ey,ey)));
    write<s16>(a,0x592,ang2);   // m476
    f32 len=call<f32>(0x028F4384,s);
    if(len<2.f) return 1;
    return 0;
}
VERIFY(0x020D2ED8,kantera_pos_search);

/* 020D30EC */
void kantera_calc(bpw_class* i_this) {
    WWHD_FUNC(0x020D30EC,void,i_this);
    u32 a=ea(i_this);
    u32 player=player_actor();
    u32 id=read<u32>(a,0x518);
    if(id==0xFFFFFFFF) return;
    Local<be<u32>> key; key->set(id);
    u32 k=call<u32>(0x025D5218,0x025E1234u,key.get());
    if(!k) return;
    s16 t=read<s16>(a,0x568);   // mAttWaitTimer
    switch((u32)(s32)t) {
    case 0:
        call(0x0200F428,k+0x328,(s32)(s16)(read<s16>(a,0x328)+read<s16>(REG0+0x740)),1,0x1000);
        call(0x0200F428,k+0x32A,(s32)(s16)(read<s16>(a,0x32A)+read<s16>(REG0+0x742)),1,0x1000);
        call(0x0200F428,k+0x32C,(s32)(s16)(read<s16>(a,0x32C)-0x4000),1,0x1000);
        call(0x0200F428,k+0x4EC,(s32)read<s16>(REG0+0x746),1,0x1000);
        call(0x0200F428,k+0x4EE,0x2000,1,0x1000);
        call(0x0200F428,k+0x4F0,(s32)read<s16>(REG0+0x74A),1,0x1000);
        if(read<s16>(a,0x568)==4) return;
        break;
    case 1:
        if(read<s16>(a,0x560)==0x14) break;
        call(0x0200F428,a+0x5A0,3000,1,100);
        call(0x0200ED84,a+0x4A4,2000.f,1.f,1000.f);
        call(0x0200ED84,a+0x4AC,2000.f,1.f,1000.f);
        if(read<s16>(a,0x568)==4) return;
        break;
    case 2:
        call(0x0200F428,a+0x5A0,6000,1,100);
        call(0x0200ED84,a+0x4A4,6000.f,1.f,1000.f);
        call(0x0200ED84,a+0x4AC,6000.f,1.f,1000.f);
        if(read<s16>(a,0x568)==4) return;
        break;
    case 3: {
        s16 py=read<s16>(player,0x32A);
        write<s16>(k,0x562,2); write<s16>(k,0x322,py);
        s16 nt=(s16)(read<s16>(a,0x568)+1);
        write<s16>(a,0x568,nt);
        if(nt==4) return;
        break;
    }
    default:
        if(t==4) return;
        break;
    }
    copy32(k,0x314,a,0x498); copy32(k,0x318,a,0x49C); copy32(k,0x31C,a,0x4A0);  // current.pos = mChildActorPos
    if(read<s16>(a,0x568)==0) return;
    call(0x0200F428,k+0x328,(s32)read<s16>(a,0x328),1,0x1000);
    call(0x0200F428,k+0x32A,(s32)(s16)(read<s16>(a,0x32A)+0x4000),1,0x1000);
    call(0x0200F428,k+0x32C,(s32)read<s16>(a,0x32C),1,0x1000);
    s16 ph=(s16)(read<s16>(a,0x59E)+read<s16>(a,0x5A0));   // m482 += m484
    f32 amp=read<f32>(a,0x4A4);
    write<s16>(a,0x59E,ph);
    call(0x0200F428,k+0x4EC,(s32)(s16)ftoi(mul(cM_ssin((u16)ph),amp)),1,0x1000);
    call(0x0200F428,k+0x4EE,0,1,0x1000);
    u16 ph2=read<u16>(a,0x59E);
    f32 amp2=read<f32>(a,0x4AC);
    call(0x0200F428,k+0x4F0,(s32)(s16)ftoi(mul(cM_scos(ph2),amp2)),1,0x1000);
}
VERIFY(0x020D30EC,kantera_calc);

/* new mDoExt_McaMorf(modelData, NULL, NULL, bck, mode, 1.0f, 0, -1, 1, NULL, 0x80000, 0x37441422) */
static u32 new_McaMorf(u32 md,u32 bck,u32 mode) {
    return call<u32>(0x025E4F64,0u,md,0u,0u,bck,mode,1.f,0u,-1,1,0u,0x80000u,0x37441422u);
}
/* joint callback loop: J3DModelData joint tree (027F3F94) +8 = joint count; node[i].callback = cb */
static void set_joint_callbacks(u32 a,u32 cb) {
    u32 morf=read<u32>(a,0x3D0);
    u32 tree=call<u32>(0x027F3F94,read<u32>(read<u32>(morf,0x90),0xAC));
    u16 n=read<u16>(tree,8);
    morf=read<u32>(a,0x3D0);
    for(u16 i=0;i<n;) {
        u32 md=read<u32>(read<u32>(morf,0x90),0xAC);
        u32 cnt=read<u32>(md,4),node=read<u32>(md,8);
        if(i<cnt) node+=i*0x1C;
        write<u32>(node,8,cb);
        i++;
        tree=call<u32>(0x027F3F94,read<u32>(read<u32>(read<u32>(a,0x3D0),0x90),0xAC));
        n=read<u16>(tree,8);
        morf=read<u32>(a,0x3D0);
    }
}
static u32 new_brkAnm() {
    u32 p=call<u32>(0x0273AD10,0x78u);
    if(p) p=call<u32>(0x025E80D0,p);
    return p;
}

/* 020D5930 */
BOOL boss_useHeapInit(bpw_class* i_this) {
    WWHD_FUNC(0x020D5930,BOOL,i_this);
    u32 a=ea(i_this);
    constexpr u32 str=0x1000B22C;
    u32 md=getObjectRes(str,0x2E);
    u32 bck=getObjectRes(str,0x2A);
    u32 morf=new_McaMorf(md,bck,2);
    write<u32>(a,0x3D0,morf);
    if(!morf || !read<u32>(morf,0x90)) return FALSE;
    write<u32>(read<u32>(morf,0x90),0xB8,a);   // setUserArea(this)
    set_joint_callbacks(a,0x020D0C04u);         // body_nodeCallBack
    u32 model=read<u32>(read<u32>(a,0x3D0),0x90);
    static const struct { u32 off,res,mode; } brks[]={{0x3D8,0x34,0},{0x3E0,0x35,2},{0x3E4,0x37,0},{0x3E8,0x36,0},{0x3EC,0x33,2}};
    for(auto& b:brks) {
        u32 p=new_brkAnm();
        write<u32>(a,b.off,p);
        if(!p) return FALSE;
        u32 res=getObjectRes(str,b.res);
        u32 brk=read<u32>(a,b.off),mdata=read<u32>(model,0xAC);
        if(!call<s32>(0x025E8154,brk,mdata,res,1,b.mode,1.f,0,-1,0,0)) return FALSE;
    }
    u32 m=read<u32>(read<u32>(a,0x3D0),0x90);
    if(!call<s32>(0x025E8A48,a+0xE94,m)) return FALSE;   // mInvisibleModel.create(model)
    return TRUE;
}
VERIFY(0x020D5930,boss_useHeapInit);

/* 020D5D04 */
BOOL kantera_useHeapInit(bpw_class* i_this) {
    WWHD_FUNC(0x020D5D04,BOOL,i_this);
    u32 a=ea(i_this);
    constexpr u32 str=0x1000B230;
    u32 md=getObjectRes(str,0x2F);
    u32 morf=new_McaMorf(md,0,1);
    write<u32>(a,0x3D0,morf);
    if(!morf || !read<u32>(morf,0x90)) return FALSE;
    write<u32>(read<u32>(morf,0x90),0xB8,a);
    set_joint_callbacks(a,0x020D0DF0u);         // kantera_nodeCallBack
    u32 model=read<u32>(read<u32>(a,0x3D0),0x90);
    u32 p=new_brkAnm();
    write<u32>(a,0x3DC,p);                      // mpLanternGlowBrkAnm
    if(!p) return FALSE;
    u32 res=getObjectRes(str,0x32);
    u32 brk=read<u32>(a,0x3DC),mdata=read<u32>(model,0xAC);
    if(!call<s32>(0x025E8154,brk,mdata,res,1,2,1.f,0,-1,0,0)) return FALSE;
    return TRUE;
}
VERIFY(0x020D5D04,kantera_useHeapInit);

/* 020D5EF0 */
BOOL fire_useHeapInit(bpw_class* i_this) {
    WWHD_FUNC(0x020D5EF0,BOOL,i_this);
    u32 a=ea(i_this);
    u32 md=getObjectRes(0x1000B234,0x2F);
    u32 morf=new_McaMorf(md,0,1);
    write<u32>(a,0x3D0,morf);
    if(!morf || !read<u32>(morf,0x90)) return FALSE;
    return TRUE;
}
VERIFY(0x020D5EF0,fire_useHeapInit);

/* 020D5FC0 */
BOOL tori_useHeapInit(bpw_class* i_this) {
    WWHD_FUNC(0x020D5FC0,BOOL,i_this);
    u32 a=ea(i_this);
    constexpr u32 str=0x1000B238;
    u32 md=getObjectRes(str,0x2E);
    u32 bck=getObjectRes(str,0x29);
    u32 morf=new_McaMorf(md,bck,1);
    write<u32>(a,0x3D0,morf);
    if(!morf) return FALSE;
    u32 model=read<u32>(morf,0x90);
    if(!model) return FALSE;
    if(!call<s32>(0x025E8A48,a+0xE94,model)) return FALSE;
    return TRUE;
}
VERIFY(0x020D5FC0,tori_useHeapInit);

/* fopAcM_seStart, inline (HD: actor and eyePos null checks) */
static void se_start(u32 a,u32 sfx,u32 param) {
    if(a && a+0x37C) {
        s32 reverb=call<s32>(0x02520540,(s32)read<s8>(a,0x326));
        call(0x025E1A40,sfx,a+0x37C,param,reverb);
    }
}

/* 020D6D98 (HD: placed after daBPW_Create) */
BOOL body_atari_check(bpw_class* i_this) {
    WWHD_FUNC(0x020D6D98,BOOL,i_this);
    u32 a=ea(i_this);
    u32 player=player_actor();
    write<u8>(a,0x4FB,0);   // mHitType
    if(read<s16>(a,0x59A)==0) return FALSE;
    call(0x02515E50,a+0x9C4);   // mStts.Move()
    if(!call<s32>(0x025162A4,a+0x9E4)) return FALSE;   // mBodyCoSph.ChkTgHit()
    u32 hit=call<u32>(0x02516300,a+0x9E4);
    if(!hit) return FALSE;
    call<u32>(0x02516300,a+0x9E4);
    write<u8>(a,0x4FB,1);
    if(read<u8>(a,0x4FC)==0) {   // m3E0
        u32 type=read<u32>(hit,0x10);
        if(type==0x100000 || type==0x800000) {   // AT_TYPE_LIGHT_ARROW / AT_TYPE_LIGHT
            write<u8>(a,0x4FB,type==0x100000?0xB:0xA);
            if(read<s16>(a,0x560)!=3) { write<s16>(a,0x560,3); write<s16>(a,0x562,0x50); }
        }
        return TRUE;
    }
    if(read<s16>(a,0x562)!=0x53) return FALSE;
    u32 anm=7;   // BOYON_S1
    switch(read<u32>(hit,0x10)) {
    case 2: case 0x400: case 0x800: case 0x4000000: case 0x10000000: {   // swords
        se_start(a,0x2803,0x44);
        u8 cut=read<u8>(player,0x3AC);
        if((cut>=5 && cut<=10) || cut==12 || (cut>=14 && cut<=16) || cut==21 || cut==23 ||
           (cut>=25 && cut<=27) || cut==30 || cut==31) { write<u8>(a,0x4FB,2); anm=6; }
        break;
    }
    case 0x8000:      // hookshot
        se_start(a,0x2834,0x44); write<u8>(a,0x4FB,0xC); break;
    case 0x200000:    // wind
        write<u8>(a,0x4FB,4); return FALSE;
    case 0x800000:    // light
        write<u8>(a,0x4FB,10); return FALSE;
    case 0x40:        // boomerang
        write<u8>(a,0x4FB,5);
        /* fallthrough */
    case 0x80: case 0x1000000:   // mace, boko stick
        se_start(a,0x2833,0x44); break;
    case 0x10000:     // skull hammer
        se_start(a,0x2855,0x44);
        write<u8>(a,0x4FB,8);
        if(read<u8>(player,0x3AC)==0x11) write<u8>(a,0x4FB,9);
        anm=6; break;
    case 0x20:        // bomb
        write<u8>(a,0x4FB,7); anm=6; break;
    case 0x80000:     // ice arrow
        write<u8>(a,0x4FB,6); se_start(a,0x2834,0x44); break;
    case 0x100000:    // light arrow
        write<u8>(a,0x4FB,0xB); se_start(a,0x2834,0x44); break;
    default:
        write<u8>(a,0x4FB,0); se_start(a,0x2834,0x44); break;
    }
    anm_init(i_this,anm,2.f,0,1.f,-1);
    Local<Vec> pos,scale;
    scale->y=2.f;
    f32 py=read<f32>(a,0xAB4),px=read<f32>(a,0xAB0);   // *mBodyCoSph.GetTgHitPosP()
    pos->y=py; pos->x=px;
    scale->z=2.f;
    pos->z=read<f32>(a,0xAB8);
    scale->x=2.f;
    u32 play=dComIfGp_get();
    call(0x025A847C,read<u32>(play,0x5AB0),0,0xC,pos.get(),player+0x328,scale.get(),0xFF,0,-1,0,0,0);
    return FALSE;
}
VERIFY(0x020D6D98,body_atari_check);

/* HD material loop (body_draw / torituki_draw): K color 3 alpha = m47E, then the HD material
 * constant-color block is refreshed from it (vcalls on the material's TEV block at +0x18). */
static void bpw_material_alpha(u32 a,u32 md) {
    u16 i=0;
    u32 t=call<u32>(0x027F3F8C,md);
    u16 n=read<u16>(t,0x24);
    Local<be<f32>[4]> rgba; Local<be<f32>[4]> conv;
    while(i<n) {
        u32 cnt=read<u32>(md,0xC),mat=read<u32>(md,0x10);
        if(i<cnt) mat+=i*0x39C;
        u32 tev=read<u32>(mat,0x18);
        u32 c=call_ptr<u32>(read<u32>(read<u32>(tev,4),0x4C),tev,3);   // getTevKColor(3)
        write<u8>(c,3,(u8)read<s16>(a,0x59A));
        tev=read<u32>(mat,0x18);
        c=call_ptr<u32>(read<u32>(read<u32>(tev,4),0x4C),tev,3);
        tev=read<u32>(mat,0x18);
        call_ptr<void>(read<u32>(read<u32>(tev,4),0x3C),tev,3,c);       // setTevKColor(3, c)
        call(0x020D1280,rgba.get(),c);
        call(0x0274D458,conv.get(),rgba.get(),1.f);
        write<u32>(mat,0xA0,read<u32>(mat,0xA0)|0x400);
        u32 dst=call<u32>(0x027F9F0C,mat+0xA0,10);
        f32 alpha=(f32)read<u8>(c,3)/255.f;
        f32 r=(*conv)[0],g=(*conv)[1],b=(*conv)[2];
        write<f32>(dst,4,g); write<f32>(dst,8,b); write<f32>(dst,0,r); write<f32>(dst,0xC,alpha);
        i++;
        t=call<u32>(0x027F3F8C,md);
        n=read<u16>(t,0x24);
    }
}
/* dComIfGd_setListMaskOff / dComIfGd_setList (HD: two list pointers copied from the play object) */
static void setListMaskOff() {
    write<u32>(0x104B4634,0,read<u32>(dComIfGp_get(),0x5D84));
    write<u32>(0x104B4638,0,read<u32>(dComIfGp_get(),0x5D88));
}
static void setList() {
    write<u32>(0x104B4634,0,read<u32>(dComIfGp_get(),0x5D78));
    write<u32>(0x104B4638,0,read<u32>(dComIfGp_get(),0x5D7C));
}
static void brk_entry(u32 brk,u32 md) { call(0x025E83FC,brk,md,read<f32>(brk,4)); }

/* 020D1334: inlines body_draw, kantera_draw, damage_ball_draw, torituki_draw.
 * HD: no ground shadows (setShadow/addRealShadow), no alpha-model spheres for the lantern and the
 * damage ball (only their matrices are still computed), no Ganondorf figure registration; the
 * brk remove() is a store of 0 to modelData+0x48; material alpha goes through the HD TEV block. */
BOOL daBPW_Draw(bpw_class* i_this) {
    WWHD_FUNC(0x020D1334,BOOL,i_this);
    u32 a=ea(i_this);
    u32 model=read<u32>(read<u32>(a,0x3D0),0x90);
    s16 blur=read<s16>(a,0x524);   // m408
    if(blur>1) { write<u8>(0x101F4826,0,(u8)blur); call(0x025F064C); }   // setBlureRate, onBlure
    else if(blur==1) { write<s16>(a,0x524,0); write<u8>(0x101F4825,0,0); }   // offBlure
    if(read<u8>(a,0x4F8)!=2) {
        u32 env=call<u32>(0x02555D0C);
        call(0x02562F5C,env,model,a+0x110);   // setLightTevColorType(model, &tevStr)
    }
    dComIfGp_get();
    switch(read<u8>(a,0x4F8)) {
    case 0: {   // body_draw
        u32 m=read<u32>(read<u32>(a,0x3D0),0x90),md=read<u32>(m,0xAC);
        bpw_material_alpha(a,md);
        if(read<u8>(a,0x4FC)==0) { setListMaskOff(); call(0x027F58E0,m,1); }
        else call(0x027F58E0,m,0);
        u32 md2=read<u32>(m,0xAC);
        u32 brk;
        if(read<u8>(a,0x4FC)) brk=read<u32>(a,0x3D8);
        else if(read<u8>(a,0x4FD)) brk=read<u32>(a,0x3E0);
        else { u8 e2=read<u8>(a,0x4FE); brk=read<u32>(a,e2==2?0x3E4:e2==1?0x3E8:0x3EC); }
        brk_entry(brk,md2);
        call(0x025E5590,read<u32>(a,0x3D0));   // entryDL
        setList();
        if(read<u8>(a,0x4FC)==0) call(0x025E8CD8,a+0xE94);   // mInvisibleModel.entryMaskOff()
        else call(0x025E8EC0,a+0xE94);                       // mInvisibleModel.entry()
        u8 e0=read<u8>(a,0x4FC);
        u32 md3=read<u32>(m,0xAC);
        if(e0==0 && read<u8>(a,0x4FD)==0) read<u8>(a,0x4FE);
        write<u32>(md3,0x48,0);   // brk remove()
        if(read<s16>(a,0x59A)==0) return TRUE;
        if(read<u32>(a,0x2E0)&0x2000) return TRUE;   // fopAcM_checkCarryNow
        call(0x025BED80,0xCB,a,1.f,1.f,1.f);          // dSnap_RegistFig(DSNAP_TYPE_BPW)
        return TRUE;
    }
    case 1: {   // kantera_draw
        u32 morf=read<u32>(a,0x3D0);
        Local<Vec> zero,pos;
        u32 m=read<u32>(morf,0x90);
        zero->x=0.f; zero->z=0.f; zero->y=0.f;
        if(read<u8>(a,0x503)==0) { brk_entry(read<u32>(a,0x3DC),read<u32>(m,0xAC)); morf=read<u32>(a,0x3D0); }
        call(0x025E5590,morf);
        if(read<u8>(a,0x503)!=0) return TRUE;
        write<u32>(read<u32>(m,0xAC),0x48,0);
        if(read<u8>(a,0x503)!=0) return TRUE;
        u32 bm=read<u32>(read<u32>(a,0x3D0),0x90);
        u32 mtx=read<u32>(calc_mtx_ptr);
        call(0x028E90D4,bm?bm+0xC8:0u,mtx);
        call(0x0200FCD8,zero.get(),pos.get());
        call(0x0200FAD8,(f32)pos->x,(f32)pos->y,(f32)pos->z,0);
        call(0x0200FC74,3.f,3.f,3.f,1);
        call(0x028E90D4,read<u32>(calc_mtx_ptr),a+0x3F0);   // m2D4
        return TRUE;
    }
    case 2: {   // damage_ball_draw
        if(read<u8>(a,0x4F9)!=1) return TRUE;
        call(0x0200FAD8,read<f32>(a,0x314),read<f32>(a,0x318),read<f32>(a,0x31C),0);
        call(0x0200FC74,0.8f,0.8f,0.8f,1);
        call(0x025F1C28,read<u32>(calc_mtx_ptr),(s32)read<s16>(a,0x32A));
        call(0x028E90D4,read<u32>(calc_mtx_ptr),a+0x3F0);
        return TRUE;
    }
    case 3: {   // torituki_draw
        u32 md=read<u32>(read<u32>(read<u32>(a,0x3D0),0x90),0xAC);
        bpw_material_alpha(a,md);
        setListMaskOff();
        call(0x025E54D8,read<u32>(a,0x3D0));   // updateDL
        setList();
        call(0x025E8CD8,a+0xE94);
        return TRUE;
    }
    }
    return TRUE;
}
VERIFY(0x020D1334,daBPW_Draw);

static void set_mtx_and_cull(u32 a,f32 lo,f32 hi) {
    u32 m=read<u32>(read<u32>(a,0x3D0),0x90);
    write<u32>(a,0x348,m?m+0xC8:0u);   // fopAcM_SetMtx(getBaseTRMtx())
    call(0x025D674C,a,lo,lo,lo,hi,hi,hi);
}
static void acch_set(u32 a) {
    call(0x024F06B4,a+0x7E4,a+0x314,a+0x300,a,1,a+0x7A4,a+0x33C,0,0);
}
static u32 actor_id(u32 a) { return a?read<u32>(a,4):0xFFFFFFFFu; }
/* dComIfGp_getStartStageName()[0] != 'X' && (REG0_S(9) != 0 || !dComIfGs_isStageBossDemo()) */
static bool start_demo_check() {
    if(read<u8>(dComIfGp_get(),0x5134)==0x58) return false;
    if(read<s16>(REG0+0x92)!=0) return true;
    return !call<s32>(0x025B9100,read<u32>(0x101F84DC)+0x798,5);
}

/* 020D6268: inlines fopAcM_ct, body/kantera/damage_ball/tori_create_init.
 * HD: no shadow id; the start-stage check reads the play object's stage name. */
s32 daBPW_Create(bpw_class* i_this) {
    WWHD_FUNC(0x020D6268,s32,i_this);
    u32 a=ea(i_this);
    struct SVec { be<u16> x,y,z; };
    Local<SVec> sp18;
    sp18->y=read<u16>(a,0x32A);
    u32 cond=read<u32>(a,0x2E4);
    sp18->z=read<u16>(a,0x32C); sp18->x=read<u16>(a,0x328);
    if(!(cond&8)) {   // fopAcM_ct
        if(a) { bpw_class_ct(a); cond=read<u32>(a,0x2E4); }
        write<u32>(a,0x2E4,cond|8);
    }
    s32 res=call<s32>(0x02520460,a+0x3C8,0x1000B250u);   // dComIfG_resLoad(&mPhase, "BPW")
    if(res!=4) return res;
    u32 p=read<u32>(a,0xB0);
    write<u8>(a,0x4F8,(u8)p);
    u8 p2=(u8)(read<u32>(a,0xB0)>>8);
    write<u8>(a,0x4F9,p2);
    write<u8>(a,0x4FA,(u8)(read<u32>(a,0xB0)>>16));
    u8 type=(u8)p;
    if(type==0xFF) { write<u8>(a,0x4F8,0); type=0; }
    if(p2==0xFF) { type=read<u8>(a,0x4F8); write<u8>(a,0x4F9,0); }
    switch(type) {
    case 0: {   // body
        if(call<s32>(0x025B9100,read<u32>(0x101F84DC)+0x798,3)) return 5;   // dComIfGs_isStageBossEnemy()
        if(!call<s32>(0x025D63E8,a,0x020D5930u,0x36E0u)) return 5;
        dComIfGp_get();
        write<s8>(a,0x3A0,15); write<s8>(a,0x3A1,15);
        if(read<s16>(REG0+0x82)!=0) { write<s8>(a,0x3A0,1); write<s8>(a,0x3A1,1); write<s16>(a,0x512,2); }
        copy32(a,0x468,a,0x314); copy32(a,0x46C,a,0x318); copy32(a,0x470,a,0x31C);   // mBodyPos
        set_mtx_and_cull(a,-300.f,300.f);
        write<u8>(a,0x38A,4);
        write<u32>(a,0x39C,4);
        write<u32>(a,0x2E0,read<u32>(a,0x2E0)|0x4000000);   // fopAcStts_BOSS_e
        acch_set(a);
        call(0x02515F14,a+0x9A8,0xFE,1,a);   // mStts.Init(0xfe, 1, actor)
        write<u8>(a,0x38C,0x25);
        write<u32>(a,0x2E0,read<u32>(a,0x2E0)|0x10000);
        call(0x0251677C,a+0x9E4,0x10192558u);   // mBodyCoSph.Set(body_co_sph_src)
        write<u32>(a,0xA10,read<u32>(a,0xA10)&~1u);
        write<u32>(a,0xA28,a+0x9A8);
        call(0x0251677C,a+0xB10,0x10192598u);   // mBodyAtSph.Set(body_at_sph_src)
        write<u32>(a,0xB54,a+0x9A8);
        write<s16>(a,0x592,read<s16>(a,0x322));
        write<u32>(a,0x368,read<u32>(read<u32>(a,0x3D0),0x90));
        write<f32>(a,0x5C0,300.f); write<f32>(a,0x5BC,0.f);
        for(u32 i=0;i<4;i++) write<u8>(a,0x70D+i*0x20,0);   // m5E0[i].setRateOff(0)
        write<s16>(a,0x568,1);
        write<f32>(a,0x4CC,340.f);
        write<f32>(a,0x5C4,add(read<f32>(0x1047BAD0),220.f));
        copy32(a,0x498,a,0x314); copy32(a,0x49C,a,0x318); copy32(a,0x4A0,a,0x31C);   // mChildActorPos
        for(u32 i=0;i<3;i++) {
            copy32(a,0x420+i*12,a,0x314); copy32(a,0x424+i*12,a,0x318); copy32(a,0x428+i*12,a,0x31C);
            copy32(a,0x444+i*12,a,0x314); copy32(a,0x448+i*12,a,0x318); copy32(a,0x44C+i*12,a,0x31C);
        }
        write<f32>(a,0x5B4,350.f); write<f32>(a,0x5B8,400.f);
        call(0x025E535C,read<u32>(a,0x3D0),0,0,0);   // mpMorf->play(NULL, 0, 0)
        BG_check(i_this);
        draw_SUB(i_this);
        sp18->y=(u16)(read<s16>(ea(sp18.get())+2)+0x4000);
        s32 room=read<s8>(a,0x326);
        u32 kid=call<u32>(0x025D5A20,0xD3,actor_id(a),1,a+0x498,room,sp18.get(),0,-1,0);
        write<u32>(a,0x518,kid);
        write<s16>(a,0x568,1);
        kantera_calc(i_this);
        if(start_demo_check()) {
            s32 hp=read<s8>(a,0x3A1);
            Local<SVec> ang;
            ang->x=read<u16>(a,0x328);
            f32 q=65536.f/(f32)hp;
            ang->y=read<u16>(a,0x32A); ang->z=read<u16>(a,0x32C);
            write<u32>(a,0x2E0,read<u32>(a,0x2E0)|0x4000);
            Local<Vec> pos;
            f32 ground=read<f32>(a,0x878);
            pos->x=read<f32>(a,0x314);
            s16 spread=(s16)ftoi(q);
            pos->z=read<f32>(a,0x31C);
            pos->y=add(ground,20.f);
            s16 cur=(s16)ftoi(call<f32>(0x02019918,32768.f));
            s16 kind=(s16)ftoi(call<f32>(0x020198D8,4.99f));
            f32 jitter=mul((f32)spread,0.25f);
            u32 prm=((u32)kind<<9)|0xFF000004u;
            u32 k=(u32)(s32)kind;
            for(u32 i=0;i<15;i++) {
                s32 r=read<s8>(a,0x326);
                u32 id=call<u32>(0x025D5A20,0xD4,actor_id(a),prm,pos.get(),r,ang.get(),0,-1,0);
                cur=(s16)(cur+spread);
                write<u32>(a,0x5C8+i*4,id);   // mChildPoeIds[i]
                ang->y=(u16)cur;
                f32 j=call<f32>(0x02019918,jitter);
                ang->y=(u16)(s16)(cur+(s16)ftoi(j));
                k++;
                if(k>5) k=0;
                prm=(k<<9)|0xFF000004u;
            }
            write<s16>(a,0x512,3);
            light_on_off(i_this);
            write<u8>(a,0x507,3); write<s16>(a,0x574,0); write<f32>(a,0x508,1.f);
            kankyou_hendou(i_this);
            write<s16>(a,0x560,0x14); write<s16>(a,0x562,200);
        } else {
            u32 bgm=read<u8>(dComIfGp_get(),0x5134)==0x58?0x80000049u:0x80000048u;
            call(0x025E18EC,bgm);   // mDoAud_bgmStart(PAST_BIG_POW / BIG_POW)
            kankyou_hendou(i_this);
            write<s16>(a,0x560,0); write<s16>(a,0x562,0);
        }
        break;
    }
    case 1: {   // kantera
        if(!call<s32>(0x025D63E8,a,0x020D5D04u,0x14C0u)) return 5;
        dComIfGp_get();
        set_mtx_and_cull(a,-100.f,100.f);
        acch_set(a);
        call(0x02515F14,a+0x9A8,100,1,a);
        call(0x0251677C,a+0xC3C,0x101925D8u);   // mKanteraCoSph.Set(kantera_co_sph_src)
        u32 parent=read<u32>(a,0x2E8);
        write<s16>(a,0x560,0);
        write<u32>(a,0xC68,read<u32>(a,0xC68)&~1u);
        write<u32>(a,0xC80,a+0x9A8);
        write<u32>(a,0x520,parent);   // m404
        write<s16>(a,0x562,0);
        if(parent==0xFFFFFFFF) call(0x025D57E0,a);
        write<u32>(a,0x2E0,read<u32>(a,0x2E0)|0x4000);
        if(start_demo_check()) {
            write<u8>(a,0x503,1);
            anm_init(i_this,0x21,read<f32>(REG0+0xA38),0,0.f,-1);   // REG18_F(4)
        }
        write<f32>(a,0x5B4,0.f); write<f32>(a,0x5B8,200.f);
        BG_check(i_this);
        kantera_draw_SUB(i_this);
        break;
    }
    case 2: {   // damage ball
        if(!call<s32>(0x025D63E8,a,0x020D5EF0u,0x14C0u)) return 5;
        copy32(a,0x474,a,0x314); copy32(a,0x478,a,0x318); copy32(a,0x47C,a,0x31C);   // m358
        dComIfGp_get();
        set_mtx_and_cull(a,-100.f,100.f);
        acch_set(a);
        call(0x02515F14,a+0x9A8,0xFE,1,a);
        call(0x0251677C,a+0xD68,0x10192618u);   // mDamageBallCoSph.Set(damage_ball_co_sph_src)
        u8 p2b=read<u8>(a,0x4F9);
        write<u32>(a,0xDAC,a+0x9A8);
        if(p2b==0) {
            write<u8>(a,0xDD7,9); write<s16>(a,0x560,0); write<s16>(a,0x562,10); write<u8>(a,0xD7C,6);
        } else if(p2b==1) {
            call(0x025564B4,a+0x604);   // dKy_plight_set(&mLightInfluence)
            write<u8>(a,0xDD7,8); write<s16>(a,0x560,1); write<s16>(a,0x562,0x14);
        }
        BG_check(i_this);
        draw_SUB(i_this);
        break;
    }
    case 3: {   // torituki
        if(!call<s32>(0x025D63E8,a,0x020D5FC0u,0x10000u)) return 5;
        write<u8>(a,0x4FC,1);
        write<f32>(a,0x330,0.f); write<f32>(a,0x334,0.f); write<f32>(a,0x338,0.f);
        u32 player=player_actor();
        copy32(a,0x468,a,0x314); copy32(a,0x46C,a,0x318); copy32(a,0x470,a,0x31C);
        write<u32>(a,0x2E0,read<u32>(a,0x2E0)|0x4000);
        set_mtx_and_cull(a,-300.f,300.f);
        write<u32>(a,0x39C,0);
        copyf(a,0x318,player,0x3DC); copyf(a,0x314,player,0x3D8); copyf(a,0x31C,player,0x3E0);   // getHeadTopPos()
        call(0x02515F14,a+0x9A8,0,1,a);
        draw_SUB(i_this);
        break;
    }
    }
    return res;
}
VERIFY(0x020D6268,daBPW_Create);

/* fopAcM_seStart with only the eyePos null check (the compiler knew this != NULL) */
static void se_start_e(u32 a,u32 sfx,u32 param) {
    if(a+0x37C) {
        s32 reverb=call<s32>(0x02520540,(s32)read<s8>(a,0x326));
        call(0x025E1A40,sfx,a+0x37C,param,reverb);
    }
}
/* fopAcM_monsSeStart, inline: mDoAud_monsSeStart(sfx, &eyePos, fopAcM_GetID(this), param, reverb)
 * (HD: eyePos null check; checkThis: also this != NULL) */
static void mons_se(u32 a,u32 sfx,u32 param,bool checkThis) {
    if(checkThis && !a) return;
    if(!(a+0x37C)) return;
    s32 room=read<s8>(a,0x326);
    u32 id=actor_id(a);
    s32 reverb=call<s32>(0x02520540,room);
    call(0x025E1AA4,sfx,a+0x37C,id,param,reverb);
}
static bool morf_isStop(u32 morf) {   // mDoExt_McaMorf::isStop (frame control at +0x98)
    return (read<u8>(morf,0xA7)&1) || read<f32>(morf,0x98)==0.f;
}
static bool brk_isStop(u32 brk) {     // mDoExt_brkAnm::isStop (frame control at +0)
    return (read<u8>(brk,0xF)&1) || read<f32>(brk,0)==0.f;
}
static s16 reg_ftoi(u32 off,f32 k) { return (s16)ftoi(add(read<f32>(REG0+off),k)); }

/* 020D72BC: inlines wall_HIT_check. HD: m5E0[0].remove() -> end(); smoke via dPa set (group 2) */
void action_damage(bpw_class* i_this) {
    WWHD_FUNC(0x020D72BC,void,i_this);
    u32 a=ea(i_this);
    u32 player=player_actor();
    u32 morf=read<u32>(a,0x3D0);
    u32 model=read<u32>(morf,0x90);
    constexpr u32 str=0x1000B078;
    auto speed_tail=[&]{ call(0x0200ED84,a+0x370,read<f32>(a,0x5AC),1.f,1.f); };
    auto brk_init_l=[&](u32 off,u32 res,u32 mode) {
        u32 r=getObjectRes(str,res);
        u32 brk=read<u32>(a,off),md=read<u32>(model,0xAC);
        call(0x025E8154,brk,md,r,1,mode,1.f,0,-1,1,0);
    };
    auto cancel_carry=[&]{
        call(0x025D9D24,a);   // fopAcM_cancelCarryNow
        write<u32>(a,0x39C,read<u32>(a,0x39C)&~0x10u);
        write<s16>(a,0x322,read<s16>(a,0x32A));
        next_status_clear(i_this,0);
        next_att_wait_check(i_this);
    };
    switch(read<s16>(a,0x562)) {
    case 0x50: {
        search_get_skull(i_this,2);
        if(read<s16>(a,0x574)==0) {
            switch(read<u8>(a,0x507)) {
            case 1: write<s16>(a,0x574,(s16)ftoi(add((f32)read<s16>(REG0+0xC52),90.f))); break;
            case 2: write<s16>(a,0x574,(s16)(read<s16>(REG0+0xC50)+10)); break;
            case 3: write<s16>(a,0x574,(s16)(read<s16>(REG0+0xC54)+10)); break;
            }
        }
        next_status_clear(i_this,1);
        write<f32>(a,0x5C0,300.f);
        fire_and_emitter_clear(i_this);
        write<s16>(a,0x594,0); write<s16>(a,0x59A,200);
        write<f32>(a,0x5AC,0.f); write<f32>(a,0x370,0.f); write<s16>(a,0x59C,200);
        anm_init(i_this,0x10,15.f,2,1.f,-1);   // HIT1
        write<s16>(a,0x57C,0);
        u32 play=dComIfGp_get();
        s16 ang=call<s16>(0x025D6894,a,read<u32>(play+0x12A0,0x488C));
        s16 st=read<s16>(a,0x562);
        write<s16>(a,0x322,ang); write<s16>(a,0x32A,ang); write<s16>(a,0x592,ang);
        write<s16>(a,0x562,(s16)(st+1));
    }
    /* fallthrough */
    case 0x51: {
        u8 hit=read<u8>(a,0x4FB);
        bool solid=false;
        if(hit==0xB) {
            se_start(a,0x5961,0);   // JA_SE_CM_BPW_SOLID_END
            write<s16>(a,0x59A,0xFF); write<s16>(a,0x594,0);
            solid=true;
        } else if(hit==0xA) {
            write<s16>(a,0x59A,(s16)(read<s16>(a,0x59A)+1));
            se_start(a,0x5160,0);   // JA_SE_CM_BPW_BECOME_SOLID
            if(read<s16>(a,0x57C)==0) {
                mons_se(a,0x4963,0,true);   // JA_SE_CV_BPW_LIGHT_DAMAGE
                write<s16>(a,0x57C,30);
            }
            if(read<u8>(a,0x4FD)==0) {
                write<u8>(a,0x4FD,1); write<u8>(a,0x4FE,0);
                brk_init_l(0x3E0,0x35,2);   // HIT
            }
            if(read<s16>(a,0x59A)<0xFF) break;
            write<s16>(a,0x59A,0xFF); write<s16>(a,0x594,0);
            solid=true;
        }
        if(solid) {
            se_start(a,0x5961,0);
            s16 t=read<s16>(a,0x568);
            write<s16>(a,0x56A,0x12);
            write<u8>(a,0x4FC,1);
            if((u32)(s32)t<=2) write<s16>(a,0x568,3);
            brk_init_l(0x3D8,0x34,0);   // HIRARU1
            anm_init(i_this,0xB,0.f,0,0.f,-1);   // DOSUN1
            write<u32>(a,0xB10,read<u32>(a,0xB10)&~1u);   // mBodyAtSph.ClrAtSet()
            write<s16>(a,0x562,(s16)(read<s16>(a,0x562)+1));
            write<f32>(a,0x5BC,50.f);   // m4A0
            speed_tail(); return;
        }
        write<s16>(a,0x56E,reg_ftoi(0x6E4,90.f));   // mSomeCountdownTimers[2] = REG12_F(7) + 90
        BOOL r=next_att_wait_check(i_this);
        u32 anm=read<u32>(a,0x514);
        if(!r) { write<s16>(a,0x560,1); write<s16>(a,0x562,0x14); }
        if(anm!=0x2A) anm_init(i_this,0x2A,15.f,2,1.f,-1);   // WAIT1
        if(read<u8>(a,0x4FD)!=0) {
            write<u8>(a,0x4FD,0);
            brk_init_l(0x3EC,0x33,0);   // DEFAULT
        }
        break;
    }
    case 0x52: {
        if(brk_isStop(read<u32>(a,0x3D8))) mons_se(a,0x4964,0,true);   // JA_SE_CV_BPW_BECOME_SOLID
        if(read<s16>(a,0x56A)!=0) break;
        call(0x0200ED84,a+0x5A8,-120.f,1.f,10.f);
        write<f32>(a,0x374,-2.f);
        if(!(read<u32>(a,0x80C)&0x20)) break;   // mAcch.ChkGroundHit()
        write<f32>(a,0x374,0.f);
        write<f32>(a,0x33C,0.f); write<f32>(a,0x340,0.f); write<f32>(a,0x344,0.f);
        write<u32>(a,0xA78,read<u32>(a,0xA78)|1u);   // mBodyCoSph.OnTgShield()
        write<u8>(a,0x504,1);
        write<f32>(a,0x5A8,-120.f);
        write<u32>(a,0xA10,read<u32>(a,0xA10)|1u);   // mBodyCoSph.OnCoSetBit()
        write<s16>(a,0x576,reg_ftoi(0xBDC,20.f));    // timers[6] = REG21_F(1) + 20
        write<s16>(a,0x562,0x53);
        if(a+0x37C) {
            se_start_e(a,0x5962,0);    // JA_SE_CM_BPW_SOLID_FALL
            mons_se(a,0x4968,0,false); // JA_SE_CV_BPW_LANDING
        }
        write<s16>(a,0x56C,0);
        anm_init(i_this,0xB,0.f,0,1.f,-1);
        write<f32>(a,0x374,0.f);
        speed_tail(); return;
    }
    case 0x53: {
        if(read<u32>(a,0x514)!=0xC && morf_isStop(morf)) {
            anm_init(i_this,0xC,15.f,2,1.f,-1);   // DOSUN_WAIT1
            write<s16>(a,0x56C,450);
            write<u32>(a,0x39C,read<u32>(a,0x39C)|0x10);
            write<s16>(a,0x7A0,read<s16>(a,0x32A));
        }
        if(read<u32>(a,0x2E0)&0x2000) {
            anm_init(i_this,0x18,15.f,2,1.f,-1);  // JITA1
            write<u32>(a,0xA10,read<u32>(a,0xA10)&~1u);
            s16 t1=read<s16>(a,0x56C);
            write<f32>(a,0x4CC,300.f);
            write<s16>(a,0x322,read<s16>(player,0x32A));
            write<s16>(a,0x596,0); write<s16>(a,0x57C,0);
            write<s16>(a,0x562,(s16)(read<s16>(a,0x562)+1));
            if(t1!=1) break;
        } else if(read<s16>(a,0x56C)!=1) break;
        cancel_carry();
        speed_tail(); return;
    }
    case 0x54: {
        s16 py=read<s16>(player,0x32A);
        if(read<s16>(REG0+0x506)!=0) {   // REG8_S(3)
            s16 v=(s16)(read<s16>(a,0x7A0)+(py-read<s16>(a,0x322)));
            write<s16>(a,0x32A,v); write<s16>(a,0x7A0,v);
            write<s16>(a,0x322,read<s16>(player,0x32A));
            if(read<s16>(a,0x57C)==0) { mons_se(a,0x4965,0,false); write<s16>(a,0x57C,0x20); }
        } else {
            write<s16>(a,0x322,py);
            call(0x0200F428,a+0x32A,(s32)read<s16>(player,0x32A),1,0x700);
            if(read<s16>(a,0x57C)==0) { mons_se(a,0x4965,0,false); write<s16>(a,0x57C,0x20); }
        }
        if(read<u32>(a,0x2E0)&0x2000) {
            if(read<s16>(a,0x56C)!=1) break;
            cancel_carry();
            speed_tail(); return;
        }
        write<f32>(a,0x374,-3.f); write<f32>(a,0x340,10.f);
        write<f32>(a,0x4CC,340.f); write<f32>(a,0x5AC,120.f);
        write<s16>(a,0x596,0);
        u32 flags=read<u32>(a,0x39C);
        if(read<s16>(REG0+0x506)!=0) {
            write<s16>(a,0x4F4,read<s16>(player,0x32A));
            write<s16>(a,0x328,-0x4000);
        } else {
            write<s16>(a,0x328,0);
        }
        write<s16>(a,0x32C,0);
        write<u32>(a,0x39C,flags&~0x10u);
        write<f32>(a,0x5B8,300.f);
        write<u8>(a,0x501,1);
        write<u32>(a,0xA10,read<u32>(a,0xA10)|1u);
        mons_se(a,0x4966,0,false);   // JA_SE_CV_BPW_THROWN
        anm_init(i_this,0x27,0.f,2,1.f,-1);   // TAMA1
        write<s16>(a,0x562,0x55);
        speed_tail(); return;
    }
    case 0x55: {
        se_start(a,0x5163,0);   // JA_SE_CM_BPW_ROLLING
        if(read<s16>(REG0+0x506)!=0) write<s16>(a,0x4F2,(s16)(read<s16>(a,0x4F2)+5000));
        else write<s16>(a,0x328,(s16)(read<s16>(a,0x328)+0x1000));
        /* wall_HIT_check */
        u32 pl=player_actor();
        f32 sc=add(read<f32>(REG0+0xBE8),6.f);   // REG21_F(4) + 6
        Local<Vec> scale; scale->y=sc; scale->z=sc; scale->x=sc;
        if(!(read<u32>(a,0x7B4)&2)) break;   // mAcchCir.ChkWallHit()
        Local<Vec> p0,p1,p2;
        call(0x02008570,dComIfGp_get()+0x12A0,a+0x7A4,p0.get(),p1.get(),p2.get());
        s32 attr=call<s32>(0x024EF0F4,dComIfGp_get()+0x12A0,a+0x7A4);
        u32 play=dComIfGp_get();
        call(0x025A847C,read<u32>(play,0x5AB0),0,0xF,p0.get(),pl+0x328,scale.get(),0xFF,0,-1,0,0,0);
        s32 res;
        if(attr==9) { write<u8>(a,0x501,0); write<s16>(a,0x524,0xB5); res=2; }   // dBgS_Attr_DAMAGE_e
        else { res=1; write<u8>(a,0x501,0); }
        write<s16>(a,0x4F2,0); write<s16>(a,0x4F4,0); write<s16>(a,0x4F6,0);
        u32 cam=read<u32>(dComIfGp_get(),0x5AF8);
        call(0x025052BC,cam+0x248,actor_id(a));   // ForceLockOff
        f32 dx=sub(read<f32>(a,0x468),read<f32>(a,0x314));
        f32 dz=sub(read<f32>(a,0x470),read<f32>(a,0x31C));
        write<s16>(a,0x322,call<s16>(0x020195B0,dx,dz));
        if(res==2) {
            if(a+0x37C) { se_start_e(a,0x5964,0); mons_se(a,0x4967,0,false); }   // DAMAGE_TOGE, CV_DAMAGE
            write<u8>(a,0x504,2);
            write<s16>(a,0x576,reg_ftoi(0xBD8,20.f));
            write<s16>(a,0x562,100);
        } else {
            if(a+0x37C) { se_start_e(a,0x5968,0); mons_se(a,0x4968,0,false); }   // BOUND_NO_DMG, LANDING
            write<s16>(a,0x328,0);
            write<u8>(a,0x504,1);
            write<s16>(a,0x576,reg_ftoi(0xBDC,20.f));
            write<s16>(a,0x562,0x5A);
        }
        speed_tail(); return;
    }
    case 0x5A:
        write<f32>(a,0x340,add(read<f32>(REG0+0x6D8),30.f));
        write<f32>(a,0x374,-add(read<f32>(REG0+0x6DC),3.f));
        write<f32>(a,0x370,add(read<f32>(REG0+0x6E0),20.f));
        write<f32>(a,0x5AC,add(read<f32>(REG0+0x6E0),20.f));
        write<s16>(a,0x596,0);
        anm_init(i_this,0xB,0.f,0,1.f,-1);
        write<s16>(a,0x562,(s16)(read<s16>(a,0x562)+1));
        speed_tail(); return;
    case 0x5B:
        call(0x0200F428,a+0x328,0,10,0x2000);
        if(!(read<u32>(a,0x80C)&0x20)) break;
        write<s16>(a,0x562,(s16)(read<s16>(a,0x562)+1));
        /* fallthrough */
    case 0x5C:
        call(0x0200F428,a+0x328,0,10,0x2000);
        if(!morf_isStop(read<u32>(a,0x3D0))) break;
        write<s16>(a,0x56A,0); write<s16>(a,0x56C,0);
        next_status_clear(i_this,0);
        next_att_wait_check(i_this);
        speed_tail(); return;
    case 100:
        write<s16>(a,0x596,0);
        write<f32>(a,0x370,5.f); write<f32>(a,0x374,-3.f);
        write<f32>(a,0x5AC,5.f); write<f32>(a,0x340,70.f);
        anm_init(i_this,0xA,15.f,2,1.f,-1);   // DAMAGE1
        write<s16>(a,0x562,(s16)(read<s16>(a,0x562)+1));
        speed_tail(); return;
    case 0x65: {
        write<s16>(a,0x32A,(s16)(read<s16>(a,0x32A)+0x1500));
        call(0x0200F428,a+0x328,0,10,0x2000);
        if(!(read<u32>(a,0x80C)&0x20)) break;
        write<u8>(a,0x504,2);
        s16 t6=reg_ftoi(0xBD8,20.f);
        f32 ground=read<f32>(a,0x878);
        write<s16>(a,0x576,t6);
        copy32(a,0x77C,a,0x314); write<f32>(a,0x780,ground); copy32(a,0x784,a,0x31C);
        call(0x025A5F88,a+0x6FC);   // m5E0[0].remove()
        s32 room=read<s8>(a,0x326);
        u32 play=dComIfGp_get();
        u32 em=call<u32>(0x025A847C,read<u32>(play,0x5AB0),2,0xA404,a+0x77C,a+0x328,0,0xB9,a+0x6FC,room,0,0,0);
        if(em) {
            Local<be<u32>> c1,c2;
            call(0x025602F0,c1.get(),c2.get());   // dKy_get_seacolor
            u32 c1a=ea(c1.get()),c2a=ea(c2.get());
            u8 b=read<u8>(c1a,2),r=read<u8>(c1a,0),g=read<u8>(c1a,1);
            write<u8>(em,0x244,r); write<u8>(em,0x245,g); write<u8>(em,0x246,b);
            u8 b2=read<u8>(c2a,2),r2=read<u8>(c2a,0),g2=read<u8>(c2a,1);
            write<u8>(em,0x248,r2); write<u8>(em,0x249,g2); write<u8>(em,0x24A,b2);
        }
        se_start_e(a,0x5965,0);   // JA_SE_CM_BPW_DAMAGE_EXPLODE
        play=dComIfGp_get();
        call(0x025A847C,read<u32>(play,0x5AB0),0,0x8444,a+0x314,0,0,0xFF,0,-1,0,0,0);
        struct SVec { be<u16> x,y,z; };
        Local<SVec> ang; Local<Vec> pos;
        ang->x=read<u16>(a,0x328); ang->y=read<u16>(a,0x32A); ang->z=read<u16>(a,0x32C);
        pos->x=read<f32>(a,0x314); pos->y=read<f32>(a,0x318); pos->z=read<f32>(a,0x31C);
        s32 hp=read<s8>(a,0x3A1);
        write<u8>(a,0x505,0);
        f32 q=65536.f/(f32)hp;
        pos->y=add(read<f32>(a,0x878),20.f);
        for(u32 i=0;i<15;i++) write<u32>(a,0x5C8+i*4,0xFFFFFFFF);
        s16 spread=(s16)ftoi(q);
        s16 cur=(s16)ftoi(call<f32>(0x02019918,32768.f));
        s16 kind=(s16)ftoi(call<f32>(0x020198D8,4.99f));
        u32 k=(u32)(s32)kind;
        u32 prm=(k<<9)|0xFF000003u;
        s32 n=read<s8>(a,0x3A1);
        if(0<n) {
            f32 jitter=mul((f32)spread,0.25f);
            for(s32 i=0;i<n;) {
                s32 r=read<s8>(a,0x326);
                u32 id=call<u32>(0x025D5A20,0xD4,actor_id(a),prm,pos.get(),r,ang.get(),0,-1,0);
                cur=(s16)(cur+spread);
                write<u32>(a,0x5C8+i*4,id);
                ang->y=(u16)cur;
                f32 j=call<f32>(0x02019918,jitter);
                ang->y=(u16)(s16)(cur+(s16)ftoi(j));
                k++;
                if(k>5) k=0;
                n=read<s8>(a,0x3A1);
                i++;
                prm=(k<<9)|0xFF000003u;
            }
        }
        write<s16>(a,0x560,4); write<s16>(a,0x562,0x6E);
        break;
    }
    }
    speed_tail();
}
VERIFY(0x020D72BC,action_damage);

/* HIO registers: REGn_F(i) / REGn_S(i) */
static constexpr u32 REGF(u32 n,u32 i) { return REG0+n*0x90+8+4*i; }
static constexpr u32 REGS(u32 n,u32 i) { return REG0+n*0x90+0x80+2*i; }
static f32 regf(u32 n,u32 i,f32 k) { return add(read<f32>(REGF(n,i)),k); }
static s16 regf_t(u32 n,u32 i,f32 k) { return (s16)ftoi(regf(n,i,k)); }
/* if (REG20_S(1) == 0) REG20_S(0) = 1; if (REG20_S(0) == 0) -> false; REG20_S(0) = 0 */
static bool reg20_step() {
    if(read<s16>(REGS(20,1))!=0 && read<s16>(REGS(20,0))==0) return false;
    write<s16>(REGS(20,0),0,0);
    return true;
}
/* fopAcM_SearchByID inline (HD: no search for fpcM_ERROR_PROCESS_ID_e) */
static u32 search_by_id(u32 id) {
    Local<be<u32>> key; key->set(id);
    if(id==0xFFFFFFFF) return 0;
    return call<u32>(0x025D5218,0x025E1234u,key.get());
}
/* cLib_addCalc2(p, target, scale, fabs(*p - target) * k * m43C) */
static void demo_calc(u32 a,u32 off,f32 target,f32 scale,u32 kn,u32 ki,f32 kb) {
    f32 v=read<f32>(a,off);
    f32 d=__builtin_fabsf(sub(v,target));
    f32 step=mul(mul(d,regf(kn,ki,kb)),read<f32>(a,0x558));
    call(0x0200ED84,a+off,target,scale,step);
}
static void set_cam_pos(u32 a,f32 cx,f32 cy,f32 cz,f32 ex,f32 ey,f32 ez) {
    write<f32>(a,0x528,cx); write<f32>(a,0x52C,cy); write<f32>(a,0x530,cz);
    write<f32>(a,0x534,ex); write<f32>(a,0x538,ey); write<f32>(a,0x53C,ez);
}

/* 020DB380. HD: SearchByID inline skips the search for an invalid id; the player demo calls are
 * inline stores (demo type 0x420, mode 0x430); camera via the play object's camera table. */
void action_start_demo(bpw_class* i_this) {
    WWHD_FUNC(0x020DB380,void,i_this);
    u32 a=ea(i_this);
    u32 player=player_actor();
    s32 camId=read<s8>(dComIfGp_get(),0x5B30);
    u32 cam=read<u32>(dComIfGp_get()+camId*0x34,0x5AF8);
    s16 st=read<s16>(a,0x562);
    auto timer=[&](u32 i)->s16 { return read<s16>(a,0x56A+i*2); };
    switch(st) {
    case 200:
        write<f32>(a,0x330,0.f); write<f32>(a,0x334,0.f); write<f32>(a,0x338,0.f);
        write<s16>(a,0x59A,0);
        write<s16>(a,0x562,(s16)(read<s16>(a,0x562)+1));
        /* fallthrough */
    case 0xC9:
        if(read<u16>(a,0xF8)!=2) {   // !eventInfo.checkCommandDemoAccrpt()
            u32 play=dComIfGp_get();
            write<u16>(play,0x52B8,(u16)(read<u16>(play,0x52B8)|1));   // dComIfGp_event_onEventFlag(1)
            call(0x025D7B24,a,2,0xFFFF,0);   // fopAcM_orderPotentialEvent(STAFF_ALL)
            write<u16>(a,0xFA,(u16)(read<u16>(a,0xFA)|2));   // onCondition(dEvtCnd_UNK2_e)
            break;
        }
        write<u32>(player,0x428,0);       // changeOriginalDemo()
        write<u16>(player,0x420,3);
        write<u32>(player,0x430,0x31);    // changeDemoMode(DEMO_S_SURP_e)
        write<f32>(a,0x4CC,140.f);
        call(0x02514F2C,cam+0x248);       // Stop()
        call(0x02515280,cam+0x248,2);     // SetTrimSize(2)
        write<f32>(a,0x55C,regf(6,0,55.f));
        write<s16>(a,0x56A,regf_t(9,0,40.f));
        anm_init(i_this,8,0.f,2,1.f,-1);  // CORE1
        write<s16>(a,0x562,(s16)(read<s16>(a,0x562)+1));
        /* fallthrough */
    case 0xCA:
        set_cam_pos(a,72.f,103.f,1618.f,89.f,108.f,1788.f);
        if(timer(0)!=0) break;
        if(!reg20_step()) break;
        write<s16>(a,0x56A,regf_t(9,1,50.f));
        write<s16>(a,0x562,(s16)(read<s16>(a,0x562)+1));
        /* fallthrough */
    case 0xCB:
        write<f32>(a,0x55C,regf(6,1,50.f));
        set_cam_pos(a,1646.f,1215.f,-72.f,1767.f,1321.f,-129.f);
        if(timer(0)!=0) break;
        if(!reg20_step()) break;
        write<s16>(a,0x56A,regf_t(9,2,40.f));
        write<s16>(a,0x56C,regf_t(9,3,20.f));
        write<s16>(a,0x562,(s16)(read<s16>(a,0x562)+1));
        /* fallthrough */
    case 0xCC:
        if(timer(1)==1) { write<u8>(a,0x506,1); call(0x025E1944); }   // mDoAud_bgmStreamPlay()
        if(timer(0)!=0) break;
        write<s16>(a,0x56A,regf_t(9,4,38.f));
        write<s16>(a,0x56C,regf_t(9,5,28.f));
        write<s16>(a,0x562,(s16)(read<s16>(a,0x562)+1));
        /* fallthrough */
    case 0xCD:
        write<f32>(a,0x55C,regf(6,2,55.f));
        set_cam_pos(a,72.f,103.f,1618.f,89.f,108.f,1788.f);
        if(timer(1)==0) write<u8>(a,0x506,2);
        if(timer(0)!=0) break;
        write<u32>(player,0x430,0x18);    // changeDemoMode(DEMO_SURPRISED_e)
        if(!reg20_step()) break;
        write<s16>(a,0x56A,regf_t(18,0,40.f));
        write<s16>(a,0x56C,regf_t(18,1,110.f));
        write<f32>(a,0x558,0.f);
        write<s16>(a,0x56E,regf_t(18,11,20.f));
        write<s16>(a,0x562,(s16)(read<s16>(a,0x562)+1));
        /* fallthrough */
    case 0xCE:
        if(timer(2)==1) write<u8>(a,0x506,3);
        if(timer(0)!=0) break;
        demo_calc(a,0x55C,regf(6,3,55.f),1.f,18,2,0.05f);
        demo_calc(a,0x528,0.f,0.1f,18,2,0.05f);
        demo_calc(a,0x52C,233.f,0.1f,18,2,0.05f);
        demo_calc(a,0x530,552.f,0.1f,18,2,0.05f);
        demo_calc(a,0x534,0.f,1.f,18,2,0.05f);
        demo_calc(a,0x538,270.f,1.f,18,2,0.05f);
        demo_calc(a,0x53C,722.f,1.f,18,2,0.05f);
        call(0x0200ED84,a+0x558,1.f,1.f,0.04f);
        write<u8>(a,0x506,4);
        if(timer(1)!=0) break;
        if(!reg20_step()) break;
        write<u8>(a,0x506,5);
        write<s16>(a,0x56A,regf_t(18,3,30.f));
        write<s16>(a,0x59C,0x96);
        se_start(a,0x6A3C,0);   // JA_SE_OBJ_BPW_MASK_APPEAR
        write<s16>(a,0x562,(s16)(read<s16>(a,0x562)+1));
        /* fallthrough */
    case 0xCF: {
        call(0x0200F428,a+0x59A,(s32)read<s16>(a,0x59C),1,5);
        call(0x0200ED84,a+0x330,1.f,1.f,0.1f);
        f32 s=read<f32>(a,0x330);
        write<f32>(a,0x338,s); write<f32>(a,0x334,s);
        if(timer(0)!=0) break;
        write<u8>(a,0x506,6);
        if(!reg20_step()) break;
        write<s16>(a,0x574,(s16)(read<s16>(REGS(21,2))+10));
        write<s16>(a,0x562,(s16)(read<s16>(a,0x562)+1));
        break;
    }
    case 0xD0:
        if(read<u32>(a,0x514)==9) {
            if(morf_isStop(read<u32>(a,0x3D0))) anm_init(i_this,8,15.f,2,1.f,-1);
            call(0x0200ED84,a+0x4CC,regf(18,12,100.f),1.f,regf(18,13,10.f));
            se_start(a,0x623D,0);   // JA_SE_OBJ_BPW_MASK_LAUGH
        } else {
            call(0x0200ED84,a+0x4CC,140.f,1.f,regf(18,13,10.f));
        }
        if(read<s16>(a,0x580)!=0) { anm_init(i_this,9,0.f,2,1.f,-1); write<s16>(a,0x580,0); }   // CORE_NIGE1
        if(read<s16>(a,0x57E)<15) break;
        if(!reg20_step()) break;
        write<s16>(a,0x56A,regf_t(18,4,30.f));
        write<s16>(a,0x56C,regf_t(18,5,55.f));
        write<f32>(a,0x558,0.f);
        write<s16>(a,0x562,(s16)(read<s16>(a,0x562)+1));
        /* fallthrough */
    case 0xD1: {
        if(timer(1)!=0) break;
        demo_calc(a,0x55C,regf(6,4,65.f),1.f,18,8,0.08f);
        demo_calc(a,0x528,281.f,0.1f,18,8,0.08f);
        demo_calc(a,0x52C,93.f,0.1f,18,8,0.08f);
        demo_calc(a,0x530,725.f,0.1f,18,8,0.08f);
        demo_calc(a,0x534,336.f,1.f,18,8,0.08f);
        demo_calc(a,0x538,30.f,1.f,18,8,0.08f);
        demo_calc(a,0x53C,877.f,1.f,18,8,0.08f);
        call(0x0200ED84,a+0x558,1.f,1.f,0.04f);
        if(!reg20_step()) break;
        if(timer(0)!=0) break;
        anm_init(i_this,0x1F,regf(18,7,10.f),0,1.f,-1);   // OPENING1
        u32 k=search_by_id(read<u32>(a,0x518));
        if(k) anm_init((bpw_class*)at<bpw_class>(k),0x21,regf(18,7,10.f),0,1.f,-1);   // OPENING_KAN1
        copy32(a,0x77C,a,0x4D4); copy32(a,0x780,a,0x4D8); copy32(a,0x784,a,0x4DC);   // mFire1DousaPos = m3B8
        copy32(a,0x794,a,0x328); write<u16>(a,0x798,read<u16>(a,0x32C));             // mFire1DousaRot = shape_angle
        if(read<u32>(a,0x674)==0) {   // m554.getEmitter() == NULL
            u32 play=dComIfGp_get();
            call(0x025A847C,read<u32>(play,0x5AB0),0,0x845A,a+0x77C,a+0x794,0,0xFF,a+0x670,-1,0,0,0);
        }
        se_start_e(a,0x5966,0);   // JA_SE_CM_BPW_MASK_TO_BPW
        write<s16>(a,0x562,(s16)(read<s16>(a,0x562)+1));
    }
        /* fallthrough */
    case 0xD2:
        if(read<u32>(a,0x674)!=0) {
            copy32(a,0x77C,a,0x4D4); copy32(a,0x780,a,0x4D8); copy32(a,0x784,a,0x4DC);
            copy32(a,0x794,a,0x328); write<u16>(a,0x798,read<u16>(a,0x32C));
        }
        demo_calc(a,0x55C,regf(6,4,65.f),1.f,18,8,0.08f);
        demo_calc(a,0x528,281.f,0.1f,18,8,0.08f);
        demo_calc(a,0x52C,93.f,0.1f,18,8,0.08f);
        demo_calc(a,0x530,725.f,0.1f,18,8,0.08f);
        demo_calc(a,0x534,336.f,1.f,18,8,0.08f);
        demo_calc(a,0x538,30.f,1.f,18,8,0.08f);
        demo_calc(a,0x53C,877.f,1.f,18,8,0.08f);
        call(0x0200ED84,a+0x558,1.f,1.f,0.04f);
        call(0x0200ED84,a+0x4CC,340.f,1.f,5.f);
        if(!reg20_step()) break;
        if(!morf_isStop(read<u32>(a,0x3D0))) break;
        call(0x025A5AC8,a+0x670);   // m554.remove()
        write<s16>(a,0x56A,regf_t(6,5,40.f));
        anm_init(i_this,0x20,0.f,2,1.f,-1);   // OPENING2
        {
            u32 k=search_by_id(read<u32>(a,0x518));
            if(k) anm_init((bpw_class*)at<bpw_class>(k),0x22,0.f,2,1.f,-1);   // OPENING_KAN2
        }
        write<s16>(a,0x562,(s16)(read<s16>(a,0x562)+1));
        /* fallthrough */
    case 0xD3:
        if(timer(0)!=0) break;
        if(!reg20_step()) break;
        write<s16>(a,0x562,(s16)(read<s16>(a,0x562)+1));
        /* fallthrough */
    case 0xD4: {
        if(timer(0)!=0) break;
        call(0x025CB610,dComIfGp_get()+0x599C,0x20);   // StopQuake(0x20)
        u32 k=search_by_id(read<u32>(a,0x518));
        if(k) anm_init((bpw_class*)at<bpw_class>(k),0x1A,0.f,0,1.f,-1);   // KAN_DEFAULT1
        write<s16>(a,0x56C,0);
        write<s16>(a,0x512,4);
        light_on_off(i_this);
        Local<Vec> center,eye;
        center->y=read<f32>(a,0x52C); eye->x=read<f32>(a,0x534);
        center->z=read<f32>(a,0x530); center->x=read<f32>(a,0x528);
        eye->y=read<f32>(a,0x538); eye->z=read<f32>(a,0x53C);
        call(0x0251510C,cam+0x248,center.get(),eye.get());   // Reset(m40C, m418)
        call(0x02514F38,cam+0x248);                          // Start()
        call(0x02515280,cam+0x248,0);                        // SetTrimSize(0)
        write<u16>(player,0x420,2); write<u32>(player,0x430,1);   // cancelOriginalDemo()
        u32 play=dComIfGp_get();
        write<u16>(play,0x52B8,(u16)(read<u16>(play,0x52B8)|8));  // dComIfGp_event_reset()
        call(0x025E18EC,0x80000048u);                        // mDoAud_bgmStart(JA_BGM_BIG_POW)
        call(0x025B9098,read<u32>(0x101F84DC)+0x798,5);      // dComIfGs_onStageBossDemo()
        write<s16>(a,0x560,0); write<s16>(a,0x512,0);
        write<u32>(a,0x2E0,read<u32>(a,0x2E0)&~0x4000u);
        write<s16>(a,0x562,0);
        break;
    }
    default: break;
    }
    u32 anm=read<u32>(a,0x514);
    if(anm==0x1F) {
        if(call<s32>(0x027F2BF8,read<u32>(a,0x3D0)+0x98,170.f)) mons_se(a,0x4961,0,true);   // CV_BPW_LAUGH_ENTER
        anm=read<u32>(a,0x514);
    }
    bool swing=(anm==0x1F && !(read<f32>(read<u32>(a,0x3D0),0x9C)<170.f)) || (anm!=0x1F && anm==0x20);
    if(swing) {
        if(a && a+0x37C) {
            s32 reverb=call<s32>(0x02520540,(s32)read<s8>(a,0x326));
            call(0x025E1A40,0x515Fu,a+0x37C,0u,reverb);   // JA_SE_CM_BPW_SWING_KANTERA
            anm=read<u32>(a,0x514);
        }
        if((anm==0x1F && !(read<f32>(read<u32>(a,0x3D0),0x9C)<60.f)) || (anm!=0x1F && anm==0x20)) se_start(a,0x7057,0);
    } else if(anm==0x1F && !(read<f32>(read<u32>(a,0x3D0),0x9C)<60.f)) {
        se_start(a,0x7057,0);   // JA_SE_OBJ_BPW_FLYING
    }
    if(read<s16>(a,0x562)>=0xCA) {
        Local<Vec> center,eye;
        center->x=read<f32>(a,0x528); center->y=read<f32>(a,0x52C);
        eye->z=read<f32>(a,0x53C); eye->y=read<f32>(a,0x538); eye->x=read<f32>(a,0x534);
        center->z=read<f32>(a,0x530);
        call(0x02514F88,cam+0x248,center.get(),eye.get(),read<f32>(a,0x55C),0);   // Set(m40C, m418, m440, 0)
        if(read<s16>(a,0x562)>=0xD1) {
            u32 k=search_by_id(read<u32>(a,0x518));
            if(k) {
                call(0x025E535C,read<u32>(k,0x3D0),0,0,0);
                if(call<s32>(0x027F2BF8,read<u32>(k,0x3D0)+0x98,regf(8,9,128.f))) {
                    se_start(a,0x596E,0);   // JA_SE_CM_BPW_KANTERA_APPEAR
                    write<u8>(k,0x503,0);
                }
            }
        }
    }
}
VERIFY(0x020DB380,action_start_demo);

void body_execute(bpw_class* i_this);
void action_bunri_dousa(bpw_class* i_this);

static void dPa_set(u32 id,u32 pos,u32 angle,u32 scale,u32 cb) {
    u32 play=dComIfGp_get();
    call(0x025A847C,read<u32>(play,0x5AB0),0,id,pos,angle,scale,0xFF,cb,-1,0,0,0);
}
/* SetC(center); SetR(radiusOff ? this->radius : 70); dComIfG_Ccsp()->Set(sph) */
static void ccs_set(u32 a,u32 sph,u32 center,u32 radiusOff) {
    call(0x02018D40,sph+0x118,center);   // cM3dGSph::SetC
    f32 r=radiusOff?read<f32>(a,radiusOff):70.f;
    call(0x02018C8C,sph+0x118,r);        // cM3dGSph::SetR
    call(0x0200E240,dComIfGp_get()+0x26A4,sph);   // dComIfG_Ccsp()->Set(sph)
}
static void clamp1(f32& v) { if(v>1.f) v=1.f; else if(v<-1.f) v=-1.f; }

/* 020D3464: inlines kantera_execute (+ action_kantera_dousa, kantera_atari_check),
 * damage_ball_execute (+ action_b_fire_1/2_dousa), torituki_execute, vib_mode_check.
 * HD: the damage-ball light is refreshed in every fire-2 state; dPa callbacks end() instead of
 * remove(); SearchByID skips invalid ids. */
BOOL daBPW_Execute(bpw_class* i_this) {
    WWHD_FUNC(0x020D3464,BOOL,i_this);
    u32 a=ea(i_this);
    if(read<u8>(dComIfGp_get(),0x5134)==0x58) write<s16>(a,0x524,0x32);
    else { s16 b=read<s16>(a,0x524); if(b>2) write<s16>(a,0x524,(s16)(b-2)); }
    for(u32 i=0;i<10;i++) { s16 t=read<s16>(a,0x56A+i*2); if(t!=0) write<s16>(a,0x56A+i*2,(s16)(t-1)); }
    Local<Vec> spd,spdOut,cen;
    switch(read<u8>(a,0x4F8)) {
    case 0:
        body_execute(i_this);
        light_on_off(i_this);
        break;
    case 1: {   /* kantera_execute */
        Local<Vec> c;
        c->x=read<f32>(a,0x474); c->y=read<f32>(a,0x478); c->z=read<f32>(a,0x47C);
        if(read<s16>(a,0x560)==0) {   /* action_kantera_dousa */
            dComIfGp_get(); dComIfGp_get(); dComIfGp_get();
            switch(read<s16>(a,0x562)) {
            case 0:
                for(u32 i=0;i<10;i++) write<s16>(a,0x57E + i*2,0);
                write<u8>(a,0x500,0);
                write<s16>(a,0x562,(s16)(read<s16>(a,0x562)+1));
                write<f32>(a,0x5B4,50.f);
                write<f32>(a,0x4C8,0.f); write<f32>(a,0x4CC,0.f); write<f32>(a,0x4D0,0.f);
                write<f32>(a,0x5B8,200.f);
                write<u32>(a,0xC68,read<u32>(a,0xC68)&~1u);   // mKanteraCoSph.ClrCoSet()
                write<f32>(a,0x5A8,60.f);
                break;
            case 2:
                write<f32>(a,0x340,10.f); write<f32>(a,0x370,10.f);
                write<f32>(a,0x4CC,140.f); write<f32>(a,0x374,-3.f);
                write<s16>(a,0x562,(s16)(read<s16>(a,0x562)+1));
                /* fallthrough */
            case 3:
                switch(read<s16>(a,0x57E)) {   // m462
                case 0:
                    if(read<u32>(a,0x80C)&0x20) {
                        write<u32>(a,0xC68,read<u32>(a,0xC68)|1u);   // OnCoSetBit
                        write<f32>(a,0x340,17.f); write<f32>(a,0x370,0.f);
                        se_start_e(a,0x6A3E,0);   // JA_SE_OBJ_BPW_KANTERA_FALL
                        write<s16>(a,0x57E,(s16)(read<s16>(a,0x57E)+1));
                    }
                    break;
                case 1:
                    write<s16>(a,0x32A,(s16)(read<s16>(a,0x32A)+0x1000));
                    call(0x0200F428,a+0x328,0x2000,1,0x700);
                    call(0x0200F428,a+0x32C,0x4000,1,0x600);
                    if(call<s16>(0x0200FAAC,(s32)read<s16>(a,0x32C),0x4000)<0x100 && (read<u32>(a,0x80C)&0x20)) {
                        write<s16>(a,0x57E,(s16)(read<s16>(a,0x57E)+1));
                        write<s16>(a,0x328,0x2000); write<s16>(a,0x32C,0x4000);
                    }
                    break;
                case 2:
                    call(0x0200F428,a+0x4EC,0,1,0x500);
                    call(0x0200F428,a+0x4F0,0,1,0x500);
                    break;
                }
                break;
            case 4:
                if(read<u32>(a,0x80C)&0x10) write<s16>(a,0x322,(s16)(read<s16>(a,0x322)-0x4000));   // ChkWallHit
                write<s16>(a,0x32A,(s16)(read<s16>(a,0x32A)+read<s16>(a,0x580)));
                call(0x0200F428,a+0x580,0,1,0x200);
                call(0x0200EDC8,a+0x370,1.f,3.f);
                if(read<f32>(a,0x370)<1.f) {
                    write<s16>(a,0x322,read<s16>(a,0x32A));
                    write<f32>(a,0x370,0.f);
                    write<s16>(a,0x57E,1); write<s16>(a,0x562,3);
                }
                break;
            case 5: {
                u32 p=search_by_id(read<u32>(a,0x520));   // m404
                if(p) write<s16>(a,0x322,call<s16>(0x025D6894,a,p));
                call(0x0200EDC8,a+0x370,1.f,3.f);
                if(read<f32>(a,0x370)<1.f) { write<u8>(a,0x500,0); write<s16>(a,0x562,3); }
                break;
            }
            }
            s16 st=read<s16>(a,0x562);
            if(read<u8>(a,0x500)!=0 && st!=5) {
                write<f32>(a,0x370,15.f);
                write<s16>(a,0x4EC,0); write<s16>(a,0x4EE,0);
                write<s16>(a,0x562,5);
                write<s16>(a,0x4F0,0);
                st=5;
            }
            if(st!=4) {   /* kantera_atari_check */
                call(0x02515E50,a+0x9C4);
                if(call<s32>(0x025162A4,a+0xC3C)) {
                    u32 pl=player_actor();
                    u32 hit=call<u32>(0x02516300,a+0xC3C);
                    if(hit && read<u32>(hit,0x10)==0x10000) {   // AT_TYPE_SKULL_HAMMER
                        write<f32>(a,0x370,60.f);
                        write<s16>(a,0x322,read<s16>(pl,0x32A));
                        write<u8>(a,0x4FB,8);
                        if(read<u8>(pl,0x3AC)==0x11) {
                            write<u8>(a,0x4FB,9);
                            write<s16>(a,0x322,(s16)(read<s16>(pl,0x32A)-0x4000));
                        }
                        se_start_e(a,0x2855,0x42);   // JA_SE_LK_HAMMER_HIT
                        write<s16>(a,0x4EC,0); write<s16>(a,0x4EE,0);
                        write<s16>(a,0x580,0x2000); write<s16>(a,0x562,4);
                        write<s16>(a,0x4F0,0);
                    }
                }
            }
        }
        ccs_set(a,a+0xC3C,ea(c.get()),0);
        if(read<u8>(a,0x503)==0) call(0x025E742C,read<u32>(a,0x3DC));   // mpLanternGlowBrkAnm->play()
        if(call<s32>(0x025162A4,a+0xC3C)) {
            u32 hit=call<u32>(0x02516300,a+0xC3C);
            u32 sfx;
            switch(read<u32>(hit,0x10)) {
            case 2: case 0x400: case 0x800: case 0x4000000: case 0x10000000: sfx=0x2803; break;
            case 0x40: case 0x80: case 0x2000: case 0x1000000: sfx=0x2833; break;
            default: sfx=0x2834; break;
            }
            se_start(a,sfx,0x42);
        }
        if(read<s16>(a,0x56A)==0) {
            write<s16>(a,0x56A,(s16)ftoi(call<f32>(0x020198D8,5.f)));
            f32 r=call<f32>(0x020198D8,5.f);
            write<s16>(a,0x59A,(s16)ftoi(add(r,8.f)));
        }
        f32 x=read<f32>(a,0x474),y=read<f32>(a,0x478),z=read<f32>(a,0x47C);
        write<f32>(a,0x390,x); write<f32>(a,0x398,z); write<f32>(a,0x394,add(y,50.f));   // attention_info.position
        write<f32>(a,0x37C,x);                                                            // eyePos
        y=read<f32>(a,0x478); write<f32>(a,0x380,y);
        write<f32>(a,0x384,read<f32>(a,0x47C));
        write<f32>(a,0x380,add(y,50.f));
        break;
    }
    case 2: {   /* damage_ball_execute */
        s16 act=read<s16>(a,0x560);
        if(act==0) {   /* action_b_fire_1_dousa */
            dComIfGp_get();
            struct LinChk { u8 storage[0x6C]; };
            Local<LinChk> lc; u32 l=ea(lc.get());
            call(0x02008FEC,lc.get());
            for(u32 o=0x5C;o<=0x62;o++) write<u8>(l,o,0);
            write<u32>(l,0,l+0x58); write<u32>(l,0x68,1);
            write<u32>(l,0x10,0x1000B0E4); write<u32>(l,4,l+0x64);
            s16 st=read<s16>(a,0x562);
            if(st==10 || st==11) {
                write<u32>(l,0x20,0x1000B0F4); write<u32>(l,0x58,0x1000B114); write<u32>(l,0x64,0x1000B104);
                if(st==10) {
                    for(u32 i=0;i<10;i++) write<s16>(a,0x57E + i*2,0);
                    u32 e=read<u32>(a,0x688);
                    write<f32>(a,0x5B4,0.f); write<f32>(a,0x5B8,40.f);
                    if(!e) dPa_set(0x8310,a+0x77C,a+0x794,0,a+0x684);   // BPWFLAMETHROWER00
                    if(!read<u32>(a,0x69C)) dPa_set(0x8311,a+0x77C,a+0x794,0,a+0x698);   // BPWFLAMETHROWER01
                    write<s16>(a,0x56A,5);
                    write<s16>(a,0x562,(s16)(read<s16>(a,0x562)+1));
                    copy32(a,0x4BC,a,0x314); copy32(a,0x4C0,a,0x318); copy32(a,0x4C4,a,0x31C);   // m3A0 = pos
                }
                s16 ph=(s16)(read<s16>(a,0x57E)+(s16)(read<s16>(REGS(6,0))+0x5DC));
                write<s16>(a,0x57E,ph);
                write<f32>(a,0x5B0,(f32)(s32)(read<s16>(REGS(6,1))+0x32));   // m494
                f32 amp=add((f32)read<s16>(REGS(6,2)),2000.f);
                s16 d=(s16)ftoi(mul(cM_ssin((u16)ph),amp));
                s16 ry=(s16)(read<s16>(a,0x32A)+d);
                write<s16>(a,0x32A,ry);
                copy32(a,0x420,a,0x314); copy32(a,0x424,a,0x318); copy32(a,0x428,a,0x31C);   // m304[0] = pos
                call(0x025F1884,read<u32>(calc_mtx_ptr),(s32)ry);
                call(0x025F1BF4,read<u32>(calc_mtx_ptr),(s32)read<s16>(a,0x328));
                Local<Vec> v; v->x=0.f; v->y=0.f; v->z=1000.f;
                call(0x0200FCD8,v.get(),a+0x444);
                call(0x028E8D88,a+0x444,a+0x314,a+0x444);
                call(0x024F1AFC,lc.get(),a+0x420,a+0x444,a);   // linChk.Set(m304, m328, actor)
                if(call<s32>(0x02008860,dComIfGp_get()+0x12A0,lc.get())) {
                    u32 play=dComIfGp_get();
                    u32 pla=call<u32>(0x020084C8,play+0x12A0,(u32)read<u16>(l,0x16),(u32)read<u16>(l,0x14));   // GetTriPla(bg index +0x16, poly index +0x14)
                    f32 cx=read<f32>(l,0x30),cy=read<f32>(l,0x34),cz=read<f32>(l,0x38);
                    write<f32>(a,0x788,cx); write<f32>(a,0x78C,cy); write<f32>(a,0x790,cz);   // m66C = *GetCrossP()
                    if(pla && read<f32>(pla,4)==1.f) {
                        write<s16>(a,0x79A,0);
                    } else {
                        f32 dx=sub(read<f32>(a,0x420),cx),dz=sub(read<f32>(a,0x428),cz);
                        write<s16>(a,0x79A,0x4000);
                        write<s16>(a,0x79C,call<s16>(0x020195B0,dx,dz));
                    }
                    if(!read<u32>(a,0x6B0)) dPa_set(0x8312,a+0x788,a+0x79A,0,a+0x6AC);   // BPWFLAMETHROWER02
                } else {
                    call(0x025A5AC8,a+0x6AC);   // m590.remove()
                }
                copy32(a,0x77C,a,0x314); copy32(a,0x780,a,0x318); copy32(a,0x784,a,0x31C);   // mFire1DousaPos
                write<u16>(a,0x794,read<u16>(a,0x328)); write<u16>(a,0x796,read<u16>(a,0x32A)); write<u16>(a,0x798,read<u16>(a,0x32C));
                f32 mx=read<f32>(a,0x444),my=read<f32>(a,0x448),mz=read<f32>(a,0x44C);
                f32 tx=mx,ty=my,tz=mz;
                if(read<s16>(a,0x580)!=0) { tx=read<f32>(a,0x420); ty=read<f32>(a,0x424); tz=read<f32>(a,0x428); }
                call(0x0200ED84,a+0x4BC,tx,1.f,mul(__builtin_fabsf(sub(read<f32>(a,0x420),mx)),0.3f));
                call(0x0200ED84,a+0x4C0,ty,1.f,mul(__builtin_fabsf(sub(read<f32>(a,0x424),read<f32>(a,0x448))),0.3f));
                call(0x0200ED84,a+0x4C4,tz,1.f,mul(__builtin_fabsf(sub(read<f32>(a,0x428),read<f32>(a,0x44C))),0.3f));
                f32 ey=sub(read<f32>(a,0x4C0),ty),ex=sub(read<f32>(a,0x4BC),tx),ez=sub(read<f32>(a,0x4C4),tz);
                f32 len=call<f32>(0x028F4384,fmadds(ez,ez,fmadds(ex,ex,mul(ey,ey))));
                if(len<2.f) write<s16>(a,0x580,(s16)((read<s16>(a,0x580)+1)&1));
            }
            write<u32>(l,0x58,0x1000B114); write<u32>(l,0x64,0x1000B0A4); write<u32>(l,0x20,0x1000B094);
            call(0x02008B4C,lc.get(),0);   // ~dBgS_LinChk
            if(call<s32>(0x025160DC,a+0xD68)) {   // mDamageBallCoSph.ChkAtHit()
                u32 pl=player_actor();
                u32 ac=call<u32>(0x02515BBC,a+0xDB8);
                if(ac && ac==pl) write<u8>(0x10192534,0,1);   // GOUEN_FIRE_HIT
            }
            copy32(ea(cen.get()),0,a,0x4BC); copy32(ea(cen.get()),4,a,0x4C0); copy32(ea(cen.get()),8,a,0x4C4);
            ccs_set(a,a+0xD68,ea(cen.get()),0x5B0);
        } else if(act==1) {   /* action_b_fire_2_dousa */
            u32 pl=player_actor();
            struct SVec { be<u16> x,y,z; };
            Local<SVec> l38;
            l38->x=read<u16>(a,0x328); l38->y=read<u16>(a,0x32A); l38->z=read<u16>(a,0x32C);
            switch(read<s16>(a,0x562)) {
            case 0x14: {
                write<u32>(a,0x2E0,read<u32>(a,0x2E0)|0x4000);
                for(u32 i=0;i<10;i++) write<s16>(a,0x57E + i*2,0);
                u32 e=read<u32>(a,0x6C4);
                write<f32>(a,0x5B4,0.f); write<f32>(a,0x5B8,40.f);
                if(!e) dPa_set(0x82D1,a+0x77C,a+0x794,0,a+0x6C0);   // BPWFIRE00
                if(!read<u32>(a,0x6D8)) dPa_set(0x82D2,a+0x77C,a+0x794,0,a+0x6D4);   // BPWFIRE01
                f32 dx=sub(read<f32>(pl,0x314),read<f32>(a,0x314));
                f32 dz=sub(read<f32>(pl,0x31C),read<f32>(a,0x31C));
                s16 ang=call<s16>(0x020195B0,dx,dz);
                write<s16>(a,0x322,ang);
                f32 r=call<f32>(0x02019918,14384.f);
                ang=(s16)(ang+(s16)ftoi(r));
                write<f32>(a,0x370,15.f);
                write<s16>(a,0x322,ang); write<s16>(a,0x32A,ang);
                r=call<f32>(0x02019918,20.f);
                write<f32>(a,0x370,add(15.f,r)); write<f32>(a,0x340,25.f);
                r=call<f32>(0x02019918,10.f);
                write<s16>(a,0x328,-0x2000);
                write<f32>(a,0x340,add(25.f,r)); write<f32>(a,0x374,-3.f);
                write<f32>(a,0x5B0,(f32)(s32)(read<s16>(REGS(6,3))+0x1E));
                write<u16>(a,0x794,read<u16>(a,0x328)); write<u16>(a,0x796,read<u16>(a,0x32A)); write<u16>(a,0x798,read<u16>(a,0x32C));
                copy32(a,0x77C,a,0x314); copy32(a,0x780,a,0x318); copy32(a,0x784,a,0x31C);
                write<s16>(a,0x562,(s16)(read<s16>(a,0x562)+1));
                break;
            }
            case 0x15: {
                call(0x0200F428,a+0x328,0x4000,1,3000);
                copy32(a,0x77C,a,0x314); copy32(a,0x780,a,0x318);
                write<u16>(a,0x794,read<u16>(a,0x328)); write<u16>(a,0x796,read<u16>(a,0x32A));
                u32 fl=read<u32>(a,0x80C);
                copy32(a,0x784,a,0x31C);
                write<u16>(a,0x798,read<u16>(a,0x32C));
                if(fl&0x20) {
                    se_start_e(a,0x6A3B,0);   // JA_SE_OBJ_BPW_FIRE_EXPLODE
                    Local<Vec> p;
                    u32 pa=ea(p.get());
                    copy32(pa,0,a,0x314); copy32(pa,4,a,0x318);
                    write<f32>(pa,4,read<f32>(a,0x878));
                    copy32(pa,8,a,0x31C);
                    l38->x=0; l38->z=0;
                    dPa_set(0x82D5,pa,ea(l38.get()),0,0);   // BPWFLOORFIRE01
                    dPa_set(0x82D6,pa,ea(l38.get()),0,0);   // BPWFLOORFIRE02
                    dPa_set(0x82D3,pa,a+0x320,0,0);         // BPWHITFIRE00
                    call(0x025A5AC8,a+0x6C0); call(0x025A5AC8,a+0x6D4);
                    if(read<s16>(a,0x56C)==0) {
                        write<s16>(a,0x56C,(s16)ftoi(call<f32>(0x020198D8,5.f)));
                        f32 r=call<f32>(0x020198D8,5.f);
                        write<s16>(a,0x59A,(s16)ftoi(add(r,8.f)));
                    }
                    write<s16>(a,0x56A,0x5A);
                    write<f32>(a,0x370,0.f); write<f32>(a,0x374,0.f);
                    write<f32>(a,0x33C,0.f); write<f32>(a,0x340,0.f); write<f32>(a,0x344,0.f);
                    write<f32>(a,0x5B0,(f32)(s32)(read<s16>(REGS(6,4))+0x23));
                    write<s16>(a,0x562,0x16);
                } else if((fl&0x10) || call<s32>(0x025160DC,a+0xD68)) {
                    dPa_set(0x82D3,a+0x314,a+0x320,0,0);
                    write<s16>(a,0x562,0x17);
                }
                break;
            }
            case 0x16: {
                write<s16>(a,0x32A,(s16)(read<s16>(a,0x32A)+0x100));
                s16 t0=read<s16>(a,0x56A);
                if(t0>10) {
                    if(read<s16>(a,0x56C)==0) {
                        write<s16>(a,0x56C,(s16)ftoi(call<f32>(0x020198D8,5.f)));
                        f32 r=call<f32>(0x020198D8,5.f);
                        write<s16>(a,0x59A,(s16)ftoi(add(r,8.f)));
                        t0=read<s16>(a,0x56A);
                    }
                } else {
                    s16 al=read<s16>(a,0x59A);
                    if(al>0) { write<s16>(a,0x59A,(s16)(al-1)); t0=read<s16>(a,0x56A); }
                }
                if(t0==0) call(0x025D57E0,a);
                else se_start_e(a,0x614F,0);   // JA_SE_OBJ_BAR_FRAME_BURN
                break;
            }
            case 0x17:
                call(0x025A5AC8,a+0x6C0); call(0x025A5AC8,a+0x6D4);
                call(0x025D57E0,a);
                break;
            }
            copy32(ea(cen.get()),0,a,0x314); copy32(ea(cen.get()),4,a,0x318); copy32(ea(cen.get()),8,a,0x31C);
            copy32(a,0x604,a,0x314); copy32(a,0x608,a,0x318); copy32(a,0x60C,a,0x31C);   // mLightInfluence.mPos
            write<s16>(a,0x610,300); write<s16>(a,0x612,0x96); write<s16>(a,0x614,0x14);
            write<f32>(a,0x618,450.f); write<f32>(a,0x61C,250.f);
            ccs_set(a,a+0xD68,ea(cen.get()),0x5B0);
        } else {
            ccs_set(a,a+0xD68,ea(cen.get()),0x5B0);
        }
        break;
    }
    case 3: {   /* torituki_execute */
        u32 pl0=player_actor();
        switch(read<s16>(a,0x562)) {
        case 0:
            call(0x025E18EC,read<u8>(dComIfGp_get(),0x5134)==0x58?0x80000150u:0x80000140u);
            write<s16>(a,0x562,(s16)(read<s16>(a,0x562)+1));
            /* fallthrough */
        case 1: {
            call(0x0200ED84,a+0x330,0.4f,1.f,0.1f);
            f32 s=read<f32>(a,0x330);
            write<f32>(a,0x338,s); write<f32>(a,0x334,s);
            if(s<0.3f) break;
            anm_init(i_this,0x29,15.f,2,1.f,-1);   // TORITUKI1
            write<s16>(a,0x57A,0x96);
            write<f32>(a,0x330,0.4f); write<f32>(a,0x334,0.4f); write<f32>(a,0x338,0.4f);
            write<s16>(a,0x562,(s16)(read<s16>(a,0x562)+1));
        }
            /* fallthrough */
        case 2: {
            write<s16>(a,0x32A,(s16)(read<s16>(a,0x32A)+1000));
            if(read<s16>(a,0x57A)==0) break;
            u32 pl=player_actor();
            bool done=read<s16>(a,0x57A)==1;
            if(!done) done=call<s32>(0x0252A038,dComIfGp_get()+0x5A20,a+0x314)!=0;
            if(!done) done=(read<u32>(dComIfGp_get(),0x5CDC)&0x2000)!=0;
            if(!done) done=read<s16>(pl,0x3B0)!=0;
            if(!done) done=(read<u32>(pl,0x3C0)&0x02000000)!=0;
            if(!done) break;
            write<u32>(pl,0x3BC,read<u32>(pl,0x3BC)&~0x100u);   // offConfuse()
            write<s16>(a,0x57A,0);
            call(0x025E18EC,read<u8>(dComIfGp_get(),0x5134)==0x58?0x80000049u:0x80000048u);
            write<s16>(a,0x562,(s16)(read<s16>(a,0x562)+1));
            break;
        }
        case 3: {
            call(0x0200EDC8,a+0x330,1.f,0.1f);
            f32 s=read<f32>(a,0x330);
            write<f32>(a,0x338,s); write<f32>(a,0x334,s);
            if(s<0.1f) call(0x025D57E0,a);
            break;
        }
        }
        call(0x025E535C,read<u32>(a,0x3D0),0,0,0);
        call(0x0200F428,a+0x59A,0xFF,1,10);
        if(read<s16>(a,0x562)==2) {
            f32 hx=read<f32>(pl0,0x3D8),hz=read<f32>(pl0,0x3E0),hy=read<f32>(pl0,0x3DC);
            write<f32>(a,0x314,hx);
            s16 t=(s16)(read<s16>(a,0x598)+700);
            write<s16>(a,0x598,t);
            u16 ph=read<u16>(a,0x598);
            write<f32>(a,0x31C,hz);
            f32 y=add(hy,100.f);
            write<f32>(a,0x318,y);
            write<f32>(a,0x318,fmadds(cM_ssin(ph),30.f,y));
        }
        break;
    }
    }
    call(0x025F1884,read<u32>(calc_mtx_ptr),(s32)read<s16>(a,0x322));
    spd->x=0.f; spd->y=0.f; spd->z=read<f32>(a,0x370);
    call(0x0200FCD8,spd.get(),spdOut.get());
    f32 g=read<f32>(a,0x374);
    write<f32>(a,0x33C,(f32)spdOut->x);
    f32 vy=add(read<f32>(a,0x340),g);
    write<f32>(a,0x344,(f32)spdOut->z);
    write<f32>(a,0x340,vy);
    if(vy<-100.f) write<f32>(a,0x340,-100.f);
    switch(read<u8>(a,0x4F8)) {
    case 0:
        call(0x025D6800,a,(read<u32>(a,0xA10)&1)?a+0x9A8:0u);   // fopAcM_posMove(ChkCoSet ? GetCCMoveP : NULL)
        BG_check(i_this);
        draw_SUB(i_this);
        break;
    case 1: {
        if(read<s16>(a,0x562)!=1) { call(0x025D6800,a,a+0x9A8); BG_check(i_this); }
        kantera_draw_SUB(i_this);
        if(read<u8>(a,0x503)!=0) break;
        u32 em=read<u32>(a,0x6EC);
        if(!em) {
            dPa_set(0x1EA,a+0x474,0,a+0x330,a+0x6E8);   // ID_AK_JN_TORCH
            copy32(a,0x77C,a,0x474); copy32(a,0x780,a,0x478); copy32(a,0x784,a,0x47C);
            em=read<u32>(a,0x6EC);
        }
        if(!em) break;
        f32 k=add(read<f32>(REGF(0,4)),-0.03f);
        f32 vx=mul(sub(read<f32>(a,0x474),read<f32>(a,0x77C)),k);
        clamp1(vx);
        f32 vz=mul(sub(read<f32>(a,0x47C),read<f32>(a,0x784)),k);
        clamp1(vz);
        write<f32>(em,0x28,vx); write<f32>(em,0x2C,0.1f); write<f32>(em,0x30,vz);   // setDirection
        Local<Vec> vel;
        call(0x0201ADE0,a+0x314,vel.get(),a+0x300);   // current.pos - old.pos
        f32 vy2=vel->y,vx2=vel->x,vz2=vel->z;
        f32 len=call<f32>(0x028F4384,fmadds(vz2,vz2,fmadds(vx2,vx2,mul(vy2,vy2))));
        f32 sy=fmadds(len,0.05f,1.f);
        if(sy>2.f) sy=2.f;
        write<f32>(em,0x238,1.f); write<f32>(em,0x23C,sy); write<f32>(em,0x240,1.f);   // setGlobalParticleScale
        copy32(a,0x77C,a,0x474); copy32(a,0x780,a,0x478); copy32(a,0x784,a,0x47C);
        s32 reverb=call<s32>(0x02520540,(s32)read<s8>(a,0x326));
        call(0x025E1A40,0x6103u,a+0x474,0u,reverb);   // mDoAud_seStart(JA_SE_OBJ_TORCH_BURNING, &m358)
        break;
    }
    case 2:
        call(0x025D6800,a,0u);
        BG_check(i_this);
        draw_SUB(i_this);
        break;
    case 3:
        draw_SUB(i_this);
        break;
    }
    if(read<u8>(a,0x4F8)!=0) return TRUE;
    kantera_calc(i_this);
    dComIfGp_get();
    u8 vib=read<u8>(a,0x504);
    switch(vib) {   /* vib_mode_check */
    case 1: case 2: {
        u32 play=dComIfGp_get();
        Local<Vec> dir; dir->x=0.f; dir->y=1.f; dir->z=0.f;
        call(0x025CB374,play+0x599C,vib==1?5:3,-0x21,dir.get());
        write<u8>(a,0x504,0);
        break;
    }
    case 3:
        if(read<s16>(a,0x576)==0) {
            call(0x025CB610,dComIfGp_get()+0x599C,-1);
            write<u8>(a,0x504,0);
        }
        break;
    }
    kankyou_hendou(i_this);
    return TRUE;
}
VERIFY(0x020D3464,daBPW_Execute);

/* inline dBgS_LinChk construction / destruction on the stack (HD vtables) */
static void linchk_ct(u32 l) {
    call(0x02008FEC,l);   // cBgS_LinChk::ct
    for(u32 o=0x5C;o<=0x62;o++) write<u8>(l,o,0);
    write<u32>(l,0x64,0x1000B104); write<u32>(l,0x68,1);
    write<u32>(l,0x58,0x1000B114); write<u32>(l,0,l+0x58); write<u32>(l,4,l+0x64);
    write<u32>(l,0x10,0x1000B0E4); write<u32>(l,0x20,0x1000B0F4);
}
static void linchk_dt(u32 l) {
    write<u32>(l,0x58,0x1000B114); write<u32>(l,0x64,0x1000B0A4); write<u32>(l,0x20,0x1000B094);
    call(0x02008B4C,l,0);
}
/* JPABaseEmitter::setGlobalSRTMatrix(model->getAnmMtx(2)) (HD: dirty flag on the joint block) */
static void emitter_srt_jnt2(u32 a,u32 em) {
    u32 model=read<u32>(read<u32>(a,0x3D0),0x90);
    u32 data=read<u32>(model,0x2C);
    u16 f=read<u16>(data,4); u32 mats=read<u32>(data,0x10);
    write<u16>(data,4,(u16)(f|0x10));
    call(0x02824890,mats+0x60,em+0x1F0,em+0x220,em+0x22C);
}
/* dComIfGp_particle_setToon(id, &mFire1DousaPos, &shape_angle, NULL, 0xB9, cb, room) + sea colors */
static void toon_smoke(u32 a,u32 id,u32 cb) {
    s32 room=read<s8>(a,0x326);
    u32 play=dComIfGp_get();
    u32 em=call<u32>(0x025A847C,read<u32>(play,0x5AB0),2,id,a+0x77C,a+0x328,0,0xB9,cb,room,0,0,0);
    if(em) {
        Local<be<u32>> c1,c2;
        call(0x025602F0,c1.get(),c2.get());   // dKy_get_seacolor
        u32 c1a=ea(c1.get()),c2a=ea(c2.get());
        u8 b=read<u8>(c1a,2),r=read<u8>(c1a,0),g=read<u8>(c1a,1);
        write<u8>(em,0x244,r); write<u8>(em,0x245,g); write<u8>(em,0x246,b);
        u8 b2=read<u8>(c2a,2),r2=read<u8>(c2a,0),g2=read<u8>(c2a,1);
        write<u8>(em,0x248,r2); write<u8>(em,0x249,g2); write<u8>(em,0x24A,b2);
    }
}
static void fire_pos_ground(u32 a) {   // mFire1DousaPos = current.pos; .y = mAcch.GetGroundH()
    copy32(a,0x77C,a,0x314); write<f32>(a,0x780,read<f32>(a,0x878)); copy32(a,0x784,a,0x31C);
}
static u32 player_by_play() { return read<u32>(dComIfGp_get()+0x12A0,0x488C); }
/* daPy_py_c::setOutPower(power, angle, 1): player vtable +0xEC */
static void set_out_power(u32 a,u32 pl,f32 power,s16 add_) {
    u32 vt=read<u32>(pl,0xB4);
    s16 ang=call<s16>(0x025D6894,a,player_by_play());
    call_ptr<void>(read<u32>(vt,0xEC),pl,(s32)(s16)(ang+add_),1,power);
}
static void inc_state(u32 a) { write<s16>(a,0x562,(s16)(read<s16>(a,0x562)+1)); }

/* gouen_maai_sub (inline) */
static void gouen_maai_sub(u32 a) {
    u32 pl=player_actor();
    f32 step=regf(9,7,2.f);
    f32 d=call<f32>(0x025D68EC,a,player_actor());
    if(d>regf(9,9,500.f)) {
        u32 mtx=read<u32>(calc_mtx_ptr);
        s16 ang=call<s16>(0x025D6894,a,player_by_play());
        call(0x025F1884,mtx,(s32)ang);
        Local<Vec> v,out;
        f32 z=regf(9,9,500.f);
        v->x=0.f; v->y=0.f; v->z=z;
        call(0x0200FCD8,v.get(),out.get());
        call(0x028E8D88,out.get(),pl+0x314,out.get());
        call(0x0200ED84,a+0x314,(f32)out->x,1.f,step);
        call(0x0200ED84,a+0x31C,(f32)out->z,1.f,step);
    }
    d=call<f32>(0x025D68EC,a,player_actor());
    if(d<regf(9,10,200.f)) {
        u32 mtx=read<u32>(calc_mtx_ptr);
        s16 ang=call<s16>(0x025D6894,a,player_by_play());
        call(0x025F1884,mtx,(s32)ang);
        Local<Vec> v,out;
        f32 z=-regf(9,10,200.f);
        f32 step2=regf(9,8,10.f);
        v->y=0.f; v->x=0.f; v->z=z;
        call(0x0200FCD8,v.get(),out.get());
        call(0x028E8D88,out.get(),pl+0x314,out.get());
        call(0x0200ED84,a+0x314,(f32)out->x,1.f,step2);
        call(0x0200ED84,a+0x31C,(f32)out->z,1.f,step2);
    }
}

/* action_dousa (inline in body_execute) */
static void action_dousa(bpw_class* i_this) {
    u32 a=ea(i_this);
    u32 pl=player_actor();
    struct LinChk { u8 storage[0x6C]; };
    Local<LinChk> lc; u32 l=ea(lc.get());
    linchk_ct(l);
    auto clear_timers=[&]{ for(u32 i=0;i<10;i++) write<s16>(a,0x57E + i*2,0); };
    bool idle=false;   // states 10..13 skip the angle/alpha part
    switch(read<s16>(a,0x562)) {
    case 0: {
        write<f32>(a,0x5A4,1.f);
        search_get_skull(i_this,2);
        clear_timers();
        if(read<u32>(a,0x514)!=0x2A) anm_init(i_this,0x2A,15.f,2,1.f,-1);   // WAIT1
        write<s16>(a,0x56A,10);
        f32 r=call<f32>(0x020198D8,10.f);
        s16 m=read<s16>(a,0x512);
        write<s16>(a,0x56A,(s16)((s16)ftoi(r)+10));
        if((u32)(s32)m<=2) write<s16>(a,0x596,read<s16>(0x1000B458+m*2));   // m47A = {0x100,0x300,0x500}[m3F6]
        inc_state(a);
    }
        /* fallthrough */
    case 1:
        if(read<s16>(a,0x56A)!=0) break;
        inc_state(a);
        /* fallthrough */
    case 2: {
        write<s16>(a,0x56A,0x41);
        write<f32>(a,0x5AC,10.f);
        f32 r=call<f32>(0x020198D8,65.f);
        s16 m=read<s16>(a,0x512);
        write<f32>(a,0x5A4,1.f);
        u32 anm=read<u32>(a,0x514);
        write<s16>(a,0x56A,(s16)((s16)ftoi(r)+0x41));
        if(m==2) {
            write<f32>(a,0x5AC,mul(read<f32>(a,0x5AC),regf(8,0,1.35f)));
            write<f32>(a,0x5A4,5.f);
        }
        if(anm!=0x1D) anm_init(i_this,0x1D,15.f,2,1.f,-1);   // MOVEF1
        inc_state(a);
    }
        /* fallthrough */
    case 3: {
        if(read<s16>(a,0x56A)!=0) break;
        s16 m=read<s16>(a,0x512);
        write<f32>(a,0x5AC,0.f);
        if(m==0) { write<s16>(a,0x560,1); write<s16>(a,0x562,0x14); break; }
        if((u32)(s32)m>2) break;
        if(call<s32>(0x0252A038,dComIfGp_get()+0x5A20,pl+0x314)) { write<s16>(a,0x560,1); write<s16>(a,0x562,0x28); break; }
        if(read<s16>(REGS(12,9))==0 && call<f32>(0x02019788)<0.5f) {
            f32 dz=sub(read<f32>(a,0x470),read<f32>(a,0x31C));
            f32 dx=sub(read<f32>(a,0x468),read<f32>(a,0x314));
            if(call<f32>(0x028F4384,fmadds(dx,dx,mul(dz,dz)))<1100.f) {
                write<s16>(a,0x560,2); write<s16>(a,0x562,0x46); break;
            }
        }
        write<s16>(a,0x562,4);
        break;
    }
    case 4:
        anm_init(i_this,0x1C,regf(12,0,15.f),0,1.f,-1);   // MOVEB1
        write<s16>(a,0x56A,regf_t(12,1,40.f));
        noroi_brk_check(i_this,0);
        mons_se(a,0x4969,0,true);   // JA_SE_CV_BPW_CURSE
        inc_state(a);
        /* fallthrough */
    case 5:
        maai_sub(i_this);
        if(read<s16>(a,0x56A)!=0) break;
        write<u8>(a,0x502,1);
        write<f32>(a,0x370,regf(8,12,75.f));
        write<f32>(a,0x5A4,regf(8,13,2.f));
        write<f32>(a,0x5AC,0.f);
        anm_init(i_this,0x1E,15.f,2,1.f,-1);   // MOVES1
        inc_state(a);
        /* fallthrough */
    case 6: {
        if(read<f32>(a,0x370)<1.f) write<f32>(a,0x370,0.f);
        s16 ang=read<s16>(a,0x322);
        call(0x025F1884,read<u32>(calc_mtx_ptr),(s32)ang);
        Local<Vec> v; v->x=0.f; v->y=0.f; v->z=500.f;
        call(0x0200FCD8,v.get(),a+0x444);
        call(0x028E8D88,a+0x444,a+0x314,a+0x444);
        f32 x=read<f32>(a,0x314),my=read<f32>(a,0x448),z=read<f32>(a,0x31C),y=read<f32>(a,0x318);
        write<f32>(a,0x420,x); write<f32>(a,0x448,add(my,400.f)); write<f32>(a,0x428,z); write<f32>(a,0x424,add(y,400.f));
        call(0x024F1AFC,l,a+0x420,a+0x444,a);
        bool stop;
        if(call<s32>(0x02008860,dComIfGp_get()+0x12A0,l)) { write<f32>(a,0x370,0.f); stop=true; }
        else stop=read<f32>(a,0x370)==0.f;
        if(stop) {
            noroi_brk_check(i_this,1);
            next_status_clear(i_this,1);
            if(read<u32>(pl,0x3BC)&0x100) { write<s16>(a,0x560,1); write<s16>(a,0x562,0x14); }
            else write<s16>(a,0x562,0);
        }
        break;
    }
    case 10:
        search_get_skull(i_this,2);
        clear_timers();
        if(read<u32>(a,0x514)!=0x2A) anm_init(i_this,0x2A,15.f,2,1.f,-1);
        inc_state(a);
        /* fallthrough */
    case 11:
        call(0x0200F428,a+0x59A,0,1,10);
        if(read<s16>(a,0x59A)<2) {
            write<s16>(a,0x59A,0);
            if(kantera_pos_search(i_this)) inc_state(a);
        }
        break;
    case 12:
        call(0x0200F428,a+0x59A,0x96,1,0x1E);
        if(read<s16>(a,0x59A)>=0x95) {
            write<s16>(a,0x59A,0x96); write<s16>(a,0x59C,0x96);
            anm_init(i_this,0xF,15.f,0,1.f,-1);   // HIROU1
            kantera_pos_search(i_this);
            inc_state(a);
        }
        break;
    case 13:
        if(read<s16>(a,0x57E)==0) kantera_pos_search(i_this);
        if(call<s32>(0x027F2BF8,read<u32>(a,0x3D0)+0x98,12.f)) {
            se_start(a,0x5967,0);   // JA_SE_CM_BPW_GET_KANTERA
            u32 id=read<u32>(a,0x518);
            if(id!=0xFFFFFFFF) {
                u32 k=search_by_id(id);
                if(k) {
                    write<f32>(k,0x374,0.f); write<f32>(k,0x370,0.f);
                    write<f32>(k,0x33C,0.f); write<f32>(k,0x340,0.f); write<f32>(k,0x344,0.f);
                    write<u32>(k,0x39C,0); write<s16>(k,0x562,0);
                    write<s16>(a,0x57E,1);
                }
            }
            write<s16>(a,0x59E,0); write<s16>(a,0x568,1);
            write<f32>(a,0x4A4,0.f); write<f32>(a,0x4A8,0.f); write<f32>(a,0x4AC,0.f);
        }
        if(morf_isStop(read<u32>(a,0x3D0))) {
            next_status_clear(i_this,1);
            write<s16>(a,0x56E,0);
            write<s16>(a,0x560,1); write<s16>(a,0x562,0x14);
        }
        break;
    }
    (void)idle;
    write<f32>(a,0x5BC,0.f);
    call(0x0200ED84,a+0x370,read<f32>(a,0x5AC),1.f,read<f32>(a,0x5A4));
    call(0x0200EDC8,a+0x5A8,1.f,10.f);
    u32 st=(u32)(s32)read<s16>(a,0x562);
    if(st==6) { alpha_anime(i_this); fuwafuwa_calc(i_this); }
    else if(!(st>=10 && st<=13)) {
        write<s16>(a,0x592,call<s16>(0x025D6894,a,player_by_play()));
        alpha_anime(i_this); fuwafuwa_calc(i_this);
    }
    noroi_check(i_this);
    linchk_dt(l);
}

/* action_kougeki (inline in body_execute) */
static void action_kougeki(bpw_class* i_this) {
    u32 a=ea(i_this);
    u32 pl=player_actor();
    switch(read<s16>(a,0x562)) {
    case 0x14:
        if(read<s16>(a,0x56E)!=0) break;
        for(u32 i=0;i<10;i++) write<s16>(a,0x57E + i*2,0);
        if(read<u32>(a,0x514)!=0x2B) anm_init(i_this,0x2B,15.f,2,1.f,-1);   // WARAU1
        write<f32>(a,0x5BC,0.f);
        write<s16>(a,0x596,0x500); write<s16>(a,0x56A,30);
        inc_state(a);
        /* fallthrough */
    case 0x15: {
        write<s16>(a,0x592,call<s16>(0x025D6894,a,player_actor()));
        maai_sub(i_this);
        if(read<s16>(a,0x56A)!=0) break;
        s16 m=read<s16>(a,0x512);
        if(m==0 || ((u32)(s32)m<=2 && !(read<u32>(pl,0x3BC)&0x100))) {
            write<s16>(a,0x562,0x28);
            if(call<f32>(0x025D68EC,a,player_by_play())<1050.f) write<s16>(a,0x562,0x3C);
        } else if((u32)(s32)m<=2) {
            write<s16>(a,0x562,0x3C);
        }
        if(read<u8>(a,0x505)!=0) { write<s16>(a,0x562,0x32); write<u8>(a,0x505,0); }
        break;
    }
    case 0x28: {
        if(read<s16>(a,0x56E)!=0) break;
        anm_init(i_this,0x26,15.f,2,1.f,-1);   // SUIKOMU1
        write<s16>(a,0x56A,30);
        f32 r=call<f32>(0x020198D8,30.f);
        u32 em=read<u32>(a,0x660);
        write<s16>(a,0x56A,(s16)((s16)ftoi(r)+30));
        if(em) dPa_set(0x82CE,a+0x4B0,0,0,a+0x65C);   // BPWINHALE01 (only when the emitter exists, as GameCube)
        inc_state(a);
    }
        /* fallthrough */
    case 0x29: {
        s32 reverb=call<s32>(0x02520540,(s32)read<s8>(a,0x326));
        call(0x025E1A40,0x7054u,a+0x4E0,0u,reverb);   // JA_SE_OBJ_BPW_BREATH_IN at m3C4
        write<s16>(a,0x592,call<s16>(0x025D6894,a,player_actor()));
        search_get_skull(i_this,0);
        u32 em=read<u32>(a,0x660);
        if(em) emitter_srt_jnt2(a,em);
        if(read<s16>(a,0x56C)==0) {
            u32 play=dComIfGp_get();
            u32 e2=call<u32>(0x025A847C,read<u32>(play,0x5AB0),0,0x82CD,a+0x4B0,a+0x328,0,0xFF,0,-1,0,0,0);   // BPWINHALE00
            if(e2) emitter_srt_jnt2(a,e2);
            write<s16>(a,0x56C,6);
        }
        if(read<s16>(a,0x56A)!=0) {
            if(call<f32>(0x025D68EC,a,player_by_play())>300.f) set_out_power(a,pl,10.f,(s16)0x8000);
        } else {
            call(0x025A5AC8,a+0x65C);   // m540.remove()
            anm_init(i_this,0x15,15.f,0,1.f,-1);   // IKI1
            inc_state(a);
        }
        break;
    }
    case 0x2A: {
        write<s16>(a,0x592,call<s16>(0x025D6894,a,player_actor()));
        search_get_skull(i_this,2);
        if(!morf_isStop(read<u32>(a,0x3D0))) break;
        write<s16>(a,0x56A,0x5A);
        f32 r=call<f32>(0x020198D8,90.f);
        u32 em=read<u32>(a,0x64C);
        write<s16>(a,0x56A,(s16)((s16)ftoi(r)+0x5A));
        if(!em) dPa_set(0x82CF,a+0x4B0,0,0,a+0x648);   // BPWBLOW00
        fire_pos_ground(a);
        write<u16>(a,0x794,read<u16>(a,0x328)); write<u16>(a,0x796,read<u16>(a,0x32A)); write<u16>(a,0x798,read<u16>(a,0x32C));
        if(!read<u32>(a,0x62C)) {
            s32 room=read<s8>(a,0x326);
            u32 play=dComIfGp_get();
            call(0x025A847C,read<u32>(play,0x5AB0),2,0xA2D0,a+0x77C,a+0x794,0,0xA0,a+0x628,room,0,0,0);   // BPWBLOWSMOKE00
        }
        anm_init(i_this,0x17,0.f,2,1.f,-1);   // IKI_WAIT1
        inc_state(a);
        break;
    }
    case 0x2B: {
        s32 reverb=call<s32>(0x02520540,(s32)read<s8>(a,0x326));
        call(0x025E1A40,0x7055u,a+0x4E0,0u,reverb);   // JA_SE_OBJ_BPW_BREATH_OUT
        write<s16>(a,0x592,call<s16>(0x025D6894,a,player_actor()));
        search_get_skull(i_this,1);
        u32 em=read<u32>(a,0x64C);
        if(em) emitter_srt_jnt2(a,em);
        fire_pos_ground(a);
        write<u16>(a,0x794,read<u16>(a,0x328)); write<u16>(a,0x796,read<u16>(a,0x32A));
        s16 t0=read<s16>(a,0x56A);
        write<u16>(a,0x798,read<u16>(a,0x32C));
        if(t0!=0) set_out_power(a,pl,14.f,0);
        else {
            search_get_skull(i_this,2);
            fire_and_emitter_clear(i_this);
            next_att_wait_check(i_this);
            write<s16>(a,0x56E,regf_t(12,7,90.f));
        }
        break;
    }
    case 0x32:
        anm_init(i_this,0x26,15.f,2,1.f,-1);
        write<s16>(a,0x56A,0x4B);
        write<u8>(a,0x507,2);
        write<s16>(a,0x56C,regf_t(8,2,20.f));
        inc_state(a);
        /* fallthrough */
    case 0x33: {
        s32 reverb=call<s32>(0x02520540,(s32)read<s8>(a,0x326));
        call(0x025E1A40,0x7054u,a+0x4E0,0u,reverb);
        if(read<s16>(a,0x56C)!=0) write<s16>(a,0x592,call<s16>(0x025D6894,a,player_by_play()));
        write<s16>(a,0x596,regf_t(21,2,140.f));
        search_get_skull(i_this,0);
        if(read<s16>(a,0x56A)!=0) {
            if(call<f32>(0x025D68EC,a,player_by_play())>300.f) set_out_power(a,pl,10.f,(s16)0x8000);
        } else {
            anm_init(i_this,0x11,15.f,0,1.f,-1);   // HONOO1
            inc_state(a);
        }
        break;
    }
    case 0x34: {
        write<s16>(a,0x592,call<s16>(0x025D6894,a,player_actor()));
        write<s16>(a,0x596,regf_t(21,2,140.f));
        search_get_skull(i_this,2);
        if(!morf_isStop(read<u32>(a,0x3D0))) break;
        u32 k=search_by_id(read<u32>(a,0x518));
        if(!k) break;
        u32 id=call<u32>(0x025D5834,0xD3,2,k+0x474,(s32)read<s8>(a,0x326),a+0x320,0,-1,0);
        write<u32>(a,0x51C,id);
        write<s16>(a,0x56A,0x46);
        f32 r=call<f32>(0x020198D8,70.f);
        write<s16>(a,0x56A,(s16)((s16)ftoi(r)+0x46));
        write<s16>(a,0x596,0x300); write<s16>(a,0x574,0);
        write<u8>(0x10192534,0,0);   // GOUEN_FIRE_HIT = 0
        anm_init(i_this,0x13,0.f,2,1.f,-1);   // HONOO_WAIT1
        inc_state(a);
        break;
    }
    case 0x35: {
        s32 reverb=call<s32>(0x02520540,(s32)read<s8>(a,0x326));
        call(0x025E1A40,0x7056u,a+0x4E0,0u,reverb);   // JA_SE_OBJ_BPW_FIREBLAST
        write<s16>(a,0x592,call<s16>(0x025D6894,a,player_actor()));
        gouen_maai_sub(a);
        u32 k=search_by_id(read<u32>(a,0x518));
        if(k) {
            u32 f=search_by_id(read<u32>(a,0x51C));
            if(f) {
                copy32(f,0x314,k,0x474); copy32(f,0x318,k,0x478); copy32(f,0x31C,k,0x47C);
                write<u16>(f,0x320,read<u16>(a,0x328)); write<u16>(f,0x322,read<u16>(a,0x32A)); write<u16>(f,0x324,read<u16>(a,0x32C));
                write<s16>(f,0x32A,read<s16>(a,0x32A));
                f32 dz=sub(read<f32>(pl,0x31C),read<f32>(f,0x31C));
                f32 dx=sub(read<f32>(pl,0x314),read<f32>(f,0x314));
                f32 dy=sub(read<f32>(pl,0x318),read<f32>(f,0x318));
                f32 h=call<f32>(0x028F4384,fmadds(dx,dx,mul(dz,dz)));
                write<s16>(f,0x328,(s16)-call<s16>(0x020195B0,dy,h));
            }
        }
        write<s16>(a,0x596,regf_t(21,2,140.f));
        if(read<s16>(REGS(8,4))!=0) break;
        if(read<u8>(0x10192534)==0 && read<s16>(a,0x56A)!=0) break;
        fire_and_emitter_clear(i_this);
        next_status_clear(i_this,1);
        next_att_wait_check(i_this);
        write<u8>(0x10192534,0,0);
        write<s16>(a,0x56A,0);
        write<s16>(a,0x574,(s16)(read<s16>(REGS(21,0))+10));
        break;
    }
    case 0x3C: {
        write<s16>(a,0x56A,0x28);
        f32 r=call<f32>(0x020198D8,40.f);
        u32 anm=read<u32>(a,0x514);
        write<u8>(a,0x507,1);
        write<s16>(a,0x56A,(s16)((s16)ftoi(r)+0x28));
        write<s16>(a,0x574,0); write<s16>(a,0x568,2);
        if(anm!=5) {
            anm_init(i_this,5,15.f,2,1.f,-1);   // ATTACK_KAN1
            mons_se(a,0x4962,0,true);           // JA_SE_CV_BPW_ATTACK
        }
        inc_state(a);
    }
        /* fallthrough */
    case 0x3D: {
        maai_sub(i_this);
        write<s16>(a,0x592,call<s16>(0x025D6894,a,player_actor()));
        se_start(a,0x515F,0);   // JA_SE_CM_BPW_SWING_KANTERA
        if(read<s16>(a,0x56C)==0) {
            se_start(a,0x6A3A,0);   // JA_SE_OBJ_BPW_FIRE_OUT
            u32 k=search_by_id(read<u32>(a,0x518));
            if(k) {
                call(0x025F1884,read<u32>(calc_mtx_ptr),(s32)read<s16>(a,0x32A));
                Local<Vec> v,out; v->x=0.f; v->y=0.f; v->z=20.f;
                call(0x0200FCD8,v.get(),out.get());
                call(0x028E8D88,out.get(),k+0x474,out.get());
                u32 id=call<u32>(0x025D5834,0xD3,0x102,out.get(),(s32)read<s8>(a,0x326),a+0x320,0,-1,0);
                write<u32>(a,0x51C,id);
                write<s16>(a,0x56C,(s16)(read<s16>(REGS(12,4))+7));
                if(read<u32>(pl,0x3BC)&0x100) write<s16>(a,0x56C,(s16)(read<s16>(REGS(12,4))+9));
            }
        }
        if(read<s16>(a,0x56A)==0) {
            write<s16>(a,0x568,1);
            write<s16>(a,0x574,(s16)ftoi(add((f32)read<s16>(REGS(21,1)),90.f)));
            if(!(read<u32>(pl,0x3BC)&0x100)) next_att_wait_check(i_this);
            else write<s16>(a,0x562,0x28);
        }
        break;
    }
    }
    call(0x0200EDC8,a+0x370,1.f,1.f);
    alpha_anime(i_this);
    fuwafuwa_calc(i_this);
}

/* action_karada_taore (inline in body_execute) */
static void action_karada_taore(bpw_class* i_this) {
    u32 a=ea(i_this);
    u32 pl=player_actor();
    switch(read<s16>(a,0x562)) {
    case 0x46:
        for(u32 i=0;i<10;i++) write<s16>(a,0x57E + i*2,0);
        if(read<u32>(a,0x514)!=0x28) anm_init(i_this,0x28,15.f,0,1.f,-1);   // TAME1
        mons_se(a,0x4969,0,true);   // JA_SE_CV_BPW_CURSE
        noroi_brk_check(i_this,0);
        write<f32>(a,0x370,0.f);
        search_get_skull(i_this,2);
        inc_state(a);
        /* fallthrough */
    case 0x47: {
        s16 ang=call<s16>(0x025D6894,a,player_by_play());
        u32 morf=read<u32>(a,0x3D0);
        write<s16>(a,0x592,ang);
        if(!call<s32>(0x027F2BF8,morf+0x98,26.f)) break;
        fire_pos_ground(a);
        call(0x025A5F88,a+0x71C);   // m5E0[1].remove()
        toon_smoke(a,0xA401,a+0x71C);   // BPWJUMPSMOKE00
        se_start_e(a,0x596A,0);   // JA_SE_CM_BPW_J_BPRESS_JUMP
        write<f32>(a,0x340,regf(8,6,100.f));
        write<f32>(a,0x374,-regf(8,7,6.f));
        f32 sp=regf(8,8,30.f);
        inc_state(a);
        write<f32>(a,0x5C4,500.f);
        write<f32>(a,0x370,sp);
        break;
    }
    case 0x48:
        if(morf_isStop(read<u32>(a,0x3D0)) && read<u32>(a,0x514)!=0x19) {
            anm_init(i_this,0x19,0.f,2,1.f,-1);   // JUMP1
            write<s16>(a,0x568,0);
            inc_state(a);
        }
        break;
    case 0x49:
        if(!(read<u32>(a,0x80C)&0x20)) break;
        se_start(a,0x596B,0);   // JA_SE_CM_BPW_J_BPRESS_PRESS
        write<f32>(a,0x33C,0.f); write<f32>(a,0x340,0.f); write<f32>(a,0x344,0.f);
        write<f32>(a,0x370,0.f);
        write<s16>(a,0x562,0x4A);
        /* fallthrough */
    case 0x4A: {
        fire_and_emitter_clear(i_this);
        write<s16>(a,0x59A,200); write<s16>(a,0x59C,200);
        write<f32>(a,0x5C0,100.f);
        search_get_skull(i_this,2);
        write<f32>(a,0x374,-3.f);
        anm_init(i_this,0x23,0.f,0,1.f,-1);   // PRESS1
        write<u8>(a,0x504,2);
        s16 t6=regf_t(21,0,20.f);
        f32 ground=read<f32>(a,0x878);
        copy32(a,0x77C,a,0x314);
        write<s16>(a,0x576,t6);
        write<f32>(a,0x780,ground);
        copy32(a,0x784,a,0x31C);
        call(0x025A5F88,a+0x73C);   // m5E0[2].remove()
        toon_smoke(a,0xA402,a+0x73C);   // BPWPRESSSMOKE00
        inc_state(a);
        break;
    }
    case 0x4B:
        call(0x0200ED84,a+0x4CC,240.f,1.f,10.f);
        write<s16>(a,0x59A,200); write<s16>(a,0x59C,200);
        if(!(read<u32>(a,0x80C)&0x20)) break;
        write<f32>(a,0x33C,0.f); write<f32>(a,0x340,0.f); write<f32>(a,0x344,0.f);
        write<f32>(a,0x370,0.f); write<f32>(a,0x374,0.f);
        inc_state(a);
        /* fallthrough */
    case 0x4C: {
        u32 anm=read<u32>(a,0x514);
        if(anm==0x23) {
            if(!(read<f32>(read<u32>(a,0x3D0),0x9C)<regf(8,1,40.f)) && read<s16>(a,0x568)==0) write<s16>(a,0x568,1);
            call(0x0200ED84,a+0x4CC,340.f,1.f,10.f);
        } else if(anm==0x25) {
            if(!(read<f32>(read<u32>(a,0x3D0),0x9C)<regf(8,1,30.f)) && read<s16>(a,0x568)==0) write<s16>(a,0x568,1);
        }
        if(!morf_isStop(read<u32>(a,0x3D0))) break;
        write<f32>(a,0x4CC,340.f);
        noroi_brk_check(i_this,1);
        next_status_clear(i_this,1);
        if(!next_att_wait_check(i_this) && (read<u32>(pl,0x3BC)&0x100)) {
            write<s16>(a,0x560,1); write<s16>(a,0x562,0x14);
        }
        break;
    }
    case 0x4D:
        fire_and_emitter_clear(i_this);
        write<s16>(a,0x59A,200); write<s16>(a,0x59C,200);
        write<f32>(a,0x5C0,100.f);
        write<f32>(a,0x5C4,regf(8,16,220.f));
        search_get_skull(i_this,2);
        write<f32>(a,0x33C,0.f); write<f32>(a,0x340,0.f); write<f32>(a,0x344,0.f);
        write<f32>(a,0x370,0.f);
        noroi_brk_check(i_this,0);
        anm_init(i_this,0x24,regf(12,2,1.f),0,1.f,-1);   // SONOBA_PRESS1
        mons_se(a,0x4969,0,false);
        write<s16>(a,0x568,0);
        write<f32>(a,0x370,14.75f);
        inc_state(a);
        break;
    case 0x4E: {
        call(0x0200ED84,a+0x4CC,240.f,1.f,10.f);
        if(!(read<f32>(read<u32>(a,0x3D0),0x9C)>20.f)) {
            s16 ang=call<s16>(0x025D6894,a,player_by_play());
            write<s16>(a,0x592,ang);
        }
        if(!(read<f32>(read<u32>(a,0x3D0),0x9C)<40.f)) call(0x0200EDC8,a+0x370,1.f,0.55f);
        if(call<s32>(0x027F2BF8,read<u32>(a,0x3D0)+0x98,40.f)) {
            fire_pos_ground(a);
            call(0x025A5F88,a+0x75C);   // m5E0[3].remove()
            toon_smoke(a,0xA403,a+0x75C);   // BPWPRESSSMOKE01
            write<f32>(a,0x374,-3.f);
            se_start_e(a,0x5969,0);   // JA_SE_CM_BPW_BODY_PRESS
            write<u8>(a,0x504,1);
            write<s16>(a,0x576,regf_t(21,1,20.f));
            write<f32>(a,0x5C4,regf(8,17,400.f));
        }
        if(morf_isStop(read<u32>(a,0x3D0))) { write<s16>(a,0x56A,5); inc_state(a); }
        break;
    }
    case 0x4F:
        if(read<s16>(a,0x56A)!=0) break;
        anm_init(i_this,0x25,regf(12,2,1.f),0,1.f,-1);   // SONOBA_PRESS2
        write<s16>(a,0x562,0x4C);
        break;
    }
    if(read<s16>(a,0x562)>=0x4A) alpha_anime(i_this);
    noroi_check(i_this);
}

/* 020DC704: body_execute (matcher: action_karada_taore). Inlines action_dousa, action_kougeki,
 * action_karada_taore, gouen_maai_sub, wall checks. HD: the curse/flying sounds go through the
 * out-of-line fopAcM_seStart 020D6D18. */
void body_execute(bpw_class* i_this) {
    WWHD_FUNC(0x020DC704,void,i_this);
    u32 a=ea(i_this);
    Local<Vec> c;
    c->x=read<f32>(a,0x474); c->y=read<f32>(a,0x478); c->z=read<f32>(a,0x47C);   // local_28 = m358
    if(read<s16>(REGS(8,2))!=0) {
        f32 x=read<f32>(a,0x314),y=read<f32>(a,0x318),z=read<f32>(a,0x31C);
        write<f32>(a,0x390,x); write<f32>(a,0x370,0.f); write<f32>(a,0x398,z);
        write<f32>(a,0x394,add(y,700.f)); write<f32>(a,0x380,add(y,300.f));
        write<f32>(a,0x37C,x); write<f32>(a,0x384,z);
        call(0x02018D40,a+0xAFC,c.get()); call(0x02018C8C,a+0xAFC,240.f);
        call(0x0200E240,dComIfGp_get()+0x26A4,a+0x9E4);
        copy32(ea(c.get()),0,a,0x48C); copy32(ea(c.get()),4,a,0x490); copy32(ea(c.get()),8,a,0x494);
        call(0x02018D40,a+0xC28,c.get()); call(0x02018C8C,a+0xC28,read<f32>(a,0x5C4));
        call(0x0200E240,dComIfGp_get()+0x26A4,a+0xB10);
        return;
    }
    switch(read<s16>(a,0x560)) {
    case 0: action_dousa(i_this); break;
    case 1: action_kougeki(i_this); break;
    case 2: action_karada_taore(i_this); break;
    case 3: action_damage(i_this); break;
    case 4: action_bunri_dousa(i_this); break;
    case 0x14: action_start_demo(i_this); break;
    }
    if(read<u8>(a,0x501)==0) {
        call(0x0200F428,a+0x322,(s32)read<s16>(a,0x592),1,(s32)read<s16>(a,0x596));
        call(0x0200F428,a+0x32A,(s32)read<s16>(a,0x322),1,(s32)read<s16>(a,0x596));
    }
    call(0x025E535C,read<u32>(a,0x3D0),0,0,0);
    if(read<u8>(a,0x4FC)) call(0x025E742C,read<u32>(a,0x3D8));
    else if(read<u8>(a,0x4FD)) call(0x025E742C,read<u32>(a,0x3E0));
    else {
        u8 e2=read<u8>(a,0x4FE);
        if(e2==2) { call(0x025E742C,read<u32>(a,0x3E4)); fopAcM_seStart(a,0x7058,0); }   // JA_SE_OBJ_BPW_FLYING_CURSE
        else if(e2==1) {
            u32 brk=read<u32>(a,0x3E8);
            if(brk_isStop(brk)) noroi_brk_check(i_this,2);
            else { call(0x025E742C,brk); fopAcM_seStart(a,0x7058,0); }
        } else {
            call(0x025E742C,read<u32>(a,0x3EC));
            if(read<s16>(a,0x560)!=0x14) fopAcM_seStart(a,0x7057,0);   // JA_SE_OBJ_BPW_FLYING
        }
    }
    f32 y=read<f32>(a,0x318),z=read<f32>(a,0x31C);
    write<f32>(a,0x394,y); write<f32>(a,0x398,z);
    f32 x=read<f32>(a,0x314);
    write<f32>(a,0x390,x);
    f32 h=read<f32>(a,0x5C0);
    write<f32>(a,0x37C,x);
    write<f32>(a,0x380,y);
    write<f32>(a,0x394,add(y,add(add(h,h),100.f)));
    write<f32>(a,0x384,z);
    write<f32>(a,0x380,add(y,read<f32>(a,0x5C0)));
    call(0x02018D40,a+0xAFC,c.get());
    call(0x02018C8C,a+0xAFC,add((f32)read<s16>(REGS(0,6)),260.f));
    call(0x0200E240,dComIfGp_get()+0x26A4,a+0x9E4);
    copy32(ea(c.get()),0,a,0x48C); copy32(ea(c.get()),4,a,0x490); copy32(ea(c.get()),8,a,0x494);
    call(0x02018D40,a+0xC28,c.get());
    call(0x02018C8C,a+0xC28,read<f32>(a,0x5C4));
    call(0x0200E240,dComIfGp_get()+0x26A4,a+0xB10);
    body_atari_check(i_this);
}
VERIFY(0x020DC704,body_execute);

/* player->setPlayerPosAndAngle(&pos, angle): player vtable +0x114 */
static void set_player_pos(u32 pl,u32 posLocal,s16 angle) {
    call_ptr<void>(read<u32>(read<u32>(pl,0xB4),0x114),pl,posLocal,(s32)angle);
}
/* sp70 = (-36, player y, -777); setPlayerPosAndAngle(&sp70, atan2s(player - mBodyPos) + 0x8000) */
static void player_to_room_body(u32 a,u32 pl) {
    Local<Vec> p;
    f32 bz=read<f32>(a,0x470),pz=read<f32>(pl,0x31C),bx=read<f32>(a,0x468),px=read<f32>(pl,0x314);
    p->x=-36.f; p->y=read<f32>(pl,0x318); p->z=-777.f;
    u32 vt=read<u32>(pl,0xB4);
    s16 ang=call<s16>(0x020195B0,sub(px,bx),sub(pz,bz));
    call_ptr<void>(read<u32>(vt,0x114),pl,p.get(),(s32)(s16)(ang+0x8000));
}
/* sp70 = (-36, player y, -777); setPlayerPosAndAngle(&sp70, searchPlayerAngleY(actor) + 0x8000) */
static void player_to_room_face(u32 a,u32 pl) {
    Local<Vec> p;
    p->x=-36.f; p->y=read<f32>(pl,0x318); p->z=-777.f;
    u32 vt=read<u32>(pl,0xB4);
    s16 ang=call<s16>(0x025D6894,a,player_actor());
    call_ptr<void>(read<u32>(vt,0x114),pl,p.get(),(s32)(s16)(ang+0x8000));
}
static void cam_reset_start(u32 a,u32 cam,u32 pl) {
    call(0x025CB610,dComIfGp_get()+0x599C,0x20);   // StopQuake(0x20)
    Local<Vec> center,eye;
    eye->z=read<f32>(a,0x53C); center->y=read<f32>(a,0x52C);
    eye->x=read<f32>(a,0x534); center->x=read<f32>(a,0x528);
    eye->y=read<f32>(a,0x538); center->z=read<f32>(a,0x530);
    call(0x0251510C,cam+0x248,center.get(),eye.get());
    call(0x02514F38,cam+0x248);
    call(0x02515280,cam+0x248,0);
    write<u16>(pl,0x420,2); write<u32>(pl,0x430,1);   // cancelOriginalDemo()
    u32 play=dComIfGp_get();
    write<u16>(play,0x52B8,(u16)(read<u16>(play,0x52B8)|8));   // dComIfGp_event_reset()
}
static bool demo_accept(u32 a) {   // eventInfo.checkCommandDemoAccrpt(), else order the event
    if(read<u16>(a,0xF8)==2) return true;
    u32 play=dComIfGp_get();
    write<u16>(play,0x52B8,(u16)(read<u16>(play,0x52B8)|1));
    call(0x025D7B24,a,2,0xFFFF,0);
    write<u16>(a,0xFA,(u16)(read<u16>(a,0xFA)|2));
    return false;
}

/* 020D8564. HD: hp phase thresholds from a table (m3F6 0..2); the camera targets move toward
 * REG + constant (the GameCube decompilation passes the distance); SearchByID skips invalid ids;
 * player demo calls are inline stores; m5E0/m554 remove() -> end(). */
void action_bunri_dousa(bpw_class* i_this) {
    WWHD_FUNC(0x020D8564,void,i_this);
    u32 a=ea(i_this);
    u32 pl=player_actor();
    s32 camId=read<s8>(dComIfGp_get(),0x5B30);
    u32 cam=read<u32>(dComIfGp_get()+camId*0x34,0x5AF8);
    auto timer=[&](u32 i)->s16 { return read<s16>(a,0x56A+i*2); };
    auto set_timer=[&](u32 i,s16 v) { write<s16>(a,0x56A+i*2,v); };
    auto poe=[&](u32 i)->u32 { return search_by_id(read<u32>(a,0x5C8+i*4)); };
    switch(read<s16>(a,0x562)) {
    case 0x6E:
        for(u32 i=0;i<10;i++) write<s16>(a,0x57E + i*2,0);
        set_timer(5,0); set_timer(0,0); write<u8>(a,0x507,3); set_timer(1,0); set_timer(2,0);
        write<s16>(a,0x598,0);   // m47C
        anm_init(i_this,8,0.f,2,1.f,-1);   // CORE1
        write<u32>(a,0xB10,read<u32>(a,0xB10)&~1u);
        write<u32>(a,0x9FC,read<u32>(a,0x9FC)&~1u);
        write<u32>(a,0xA10,read<u32>(a,0xA10)&~1u);
        call(0x0251621C,a+0x9E4);   // mBodyCoSph.ClrTgHit()
        write<f32>(a,0x374,0.f);
        write<f32>(a,0x33C,0.f); write<f32>(a,0x340,0.f); write<f32>(a,0x344,0.f);
        write<f32>(a,0x370,0.f);
        write<f32>(a,0x330,0.f); write<f32>(a,0x334,0.f); write<f32>(a,0x338,0.f);
        {
            s16 st=read<s16>(a,0x562);
            copy32(a,0x314,a,0x468); copy32(a,0x318,a,0x46C);
            write<s16>(a,0x596,0x500);
            write<u32>(a,0x39C,0);
            copy32(a,0x31C,a,0x470);
            set_timer(0,0x1A4);
            write<s16>(a,0x562,(s16)(st+1));
            write<u8>(a,0x502,0); write<u8>(a,0x501,0);
        }
        /* fallthrough */
    case 0x6F: {
        s16 m=read<s16>(a,0x512);
        s32 thr=0;
        if((u32)(s32)m<=2) thr=read<s8>(0x1000B3BC+m);   // {10, 5, 0}
        s32 hp=read<s8>(a,0x3A1);
        if(hp<=0) {
            if(read<u8>(dComIfGp_get(),0x5134)==0x58) {
                call(0x025B8B68,read<u32>(0x101F84DC)+0x644,0x3210);   // onEventBit(JALHALLA_TRIALS_CLEAR)
                call(0x025B8B68,read<u32>(0x101F84DC)+0x1178,0x480);   // onTmpBit(UNK_0480)
                call(0x02587EFC,0,(s32)read<s8>(a,0x326));             // dLib_setNextStageBySclsNum
                s32 reverb=call<s32>(0x02520540,(s32)read<s8>(a,0x326));
                call(0x025E1A40,0x2888u,0u,0u,reverb);                 // JA_SE_LK_B_BOSS_WARP
            } else {
                write<u32>(a,0x2E0,read<u32>(a,0x2E0)|0x4000);
                write<u32>(a,0xA10,read<u32>(a,0xA10)&~1u);
                write<u32>(a,0x9FC,read<u32>(a,0x9FC)&~1u);
                call(0x0251621C,a+0x9E4);
                write<s16>(a,0x562,0x78);
            }
            break;
        }
        if(timer(0)!=0 && hp>thr) break;
        write<u32>(a,0xA10,read<u32>(a,0xA10)|1u);   // OnCoSetBit
        write<s16>(a,0x562,(s16)(read<s16>(a,0x562)+1));
        write<u32>(a,0x2E0,read<u32>(a,0x2E0)|0x4000);
    }
        /* fallthrough */
    case 0x70:
        for(s32 i=0;i<read<s8>(a,0x3A1);i++) {
            u32 p=poe(i);
            if(p && read<s8>(p,0x3A1)>0) write<s8>(p,0x3A1,0x14);
        }
        write<s16>(a,0x59A,0);
        inc_state(a);
        /* fallthrough */
    case 0x71: {
        if(!demo_accept(a)) break;
        write<u32>(pl,0x428,0); write<u32>(pl,0x430,1); write<u16>(pl,0x420,3);   // changeOriginalDemo, DEMO_N_WAIT
        anm_init(i_this,8,0.f,2,1.f,-1);
        call(0x02514F2C,cam+0x248); call(0x02515280,cam+0x248,2);
        write<f32>(a,0x55C,50.f);
        Local<Vec> p; p->x=0.f;
        write<s16>(a,0x59C,0x96); write<s16>(a,0x56E,0xF);
        p->z=1625.f;
        write<s16>(a,0x590,1); write<u8>(a,0x4FC,0);
        p->y=0.f;
        u32 vt=read<u32>(pl,0xB4);
        s16 ang=call<s16>(0x025D6894,pl,a);
        call_ptr<void>(read<u32>(vt,0x114),pl,p.get(),(s32)ang);
        inc_state(a);
    }
        /* fallthrough */
    case 0x72: {
        set_cam_pos(a,156.f,515.f,1001.f,181.f,603.f,1196.f);
        if(!reg20_step()) break;
        if(timer(2)!=0) break;
        s16 ang=call<s16>(0x025D6894,a,player_actor());
        write<s16>(a,0x592,ang); write<s16>(a,0x322,ang); write<s16>(a,0x32A,ang);
        call(0x0200F428,a+0x59A,(s32)read<s16>(a,0x59C),1,0xF);
        call(0x0200ED84,a+0x330,1.f,1.f,0.1f);
        f32 s=read<f32>(a,0x330);
        write<f32>(a,0x338,s); write<f32>(a,0x334,s);
        if(read<u8>(a,0x505)==0 && read<f32>(a,0x330)>0.9f) {
            write<f32>(a,0x330,1.f); write<f32>(a,0x334,1.f);
            write<u8>(a,0x505,1);
            write<f32>(a,0x338,1.f);
            set_timer(5,(s16)(read<s16>(REGS(21,2))+10));
            set_timer(1,300);
            for(s32 i=0;i<read<s8>(a,0x3A1);i++) {
                u32 p=poe(i);
                if(p) write<u32>(p,0x2E0,read<u32>(p,0x2E0)|0x4000);
            }
        }
        if(read<u32>(a,0x514)==9) {
            if(morf_isStop(read<u32>(a,0x3D0))) anm_init(i_this,8,15.f,2,1.f,-1);
            se_start_e(a,0x623D,0);   // JA_SE_OBJ_BPW_MASK_LAUGH
        }
        {
            Local<Vec> p; p->x=0.f; p->y=0.f; p->z=1625.f;
            u32 vt=read<u32>(pl,0xB4);
            s16 a2=call<s16>(0x025D6894,pl,a);
            call_ptr<void>(read<u32>(vt,0x114),pl,p.get(),(s32)a2);
        }
        if(read<s16>(a,0x580)!=0) { anm_init(i_this,9,0.f,0,1.f,-1); write<s16>(a,0x580,0); }
        if(timer(1)==1 || read<s16>(a,0x57E)>=read<s8>(a,0x3A1)) {
            anm_init(i_this,9,0.f,2,1.f,-1);   // CORE_NIGE1
            set_timer(0,30);
            inc_state(a);
        }
        write<f32>(a,0x4CC,140.f);
        fuwafuwa_calc(i_this);
        break;
    }
    case 0x73:
        if(timer(0)!=0) break;
        if(!reg20_step()) break;
        anm_init(i_this,0x14,15.f,0,1.f,-1);   // HUKKATU1
        se_start(a,0x5966,0);   // JA_SE_CM_BPW_MASK_TO_BPW
        inc_state(a);
        write<f32>(a,0x558,0.f);
        break;
    case 0x74:
        demo_calc(a,0x528,regf(20,12,152.f),1.f,20,18,0.08f);
        demo_calc(a,0x52C,regf(20,13,498.f),1.f,20,18,0.08f);
        demo_calc(a,0x530,regf(20,14,983.f),1.f,20,18,0.08f);
        demo_calc(a,0x534,regf(20,15,181.f),1.f,20,18,0.08f);
        demo_calc(a,0x538,regf(20,16,504.f),1.f,20,18,0.08f);
        demo_calc(a,0x53C,regf(20,17,1196.f),1.f,20,18,0.08f);
        call(0x0200ED84,a+0x558,1.f,1.f,0.04f);
        call(0x0200EDC8,a+0x5A8,1.f,10.f);
        call(0x0200ED84,a+0x4CC,340.f,1.f,10.f);
        if(!morf_isStop(read<u32>(a,0x3D0))) break;
        if(!reg20_step()) break;
        anm_init(i_this,0x2A,15.f,2,1.f,-1);   // WAIT1
        inc_state(a);
        set_timer(0,0x14);
        break;
    case 0x75: {
        if(!reg20_step()) break;
        if(timer(0)!=0) break;
        cam_reset_start(a,cam,pl);
        next_status_clear(i_this,1);
        next_att_wait_check(i_this);
        s32 hp=read<s8>(a,0x3A1);
        if(hp<=5) write<s16>(a,0x512,2);
        else if(hp<=10) write<s16>(a,0x512,1);
        write<u32>(a,0x39C,4);
        write<u32>(a,0x2E0,read<u32>(a,0x2E0)&~0x4000u);
        break;
    }
    case 0x78: {
        if(!demo_accept(a)) break;
        set_timer(5,3); write<s16>(a,0x512,3);
        for(u32 i=0;i<10;i++) write<s16>(a,0x57E + i*2,0);
        f32 by=read<f32>(a,0x46C);
        write<f32>(a,0x374,-3.f);
        write<f32>(a,0x33C,0.f); write<f32>(a,0x340,0.f); write<f32>(a,0x344,0.f);
        write<f32>(a,0x370,0.f);
        write<f32>(a,0x318,add(add(by,200.f),read<f32>(REGF(18,0))));
        f32 r1=read<f32>(REGF(18,1));
        write<f32>(a,0x330,0.f); write<f32>(a,0x334,0.f); write<f32>(a,0x338,0.f);
        write<f32>(a,0x5A8,r1);
        write<u8>(a,0x502,0); write<u8>(a,0x501,0);
        anm_init(i_this,8,15.f,2,1.f,-1);
        u32 kid=read<u32>(a,0x518);
        if(kid!=0xFFFFFFFF) {
            Local<be<u32>> key; key->set(kid);
            u32 k=call<u32>(0x025D5218,0x025E1234u,key.get());
            call(0x025D99E8,k,k+0x314,5,3,0xFF);   // fopAcM_createDisappear
            call(0x025D57E0,k);
        }
        for(s32 i=0;i<read<s8>(a,0x3A0);i++) {
            u32 p=poe(i);
            if(p && read<u8>(p,0x460)!=0) {   // pw_class m344
                write<u32>(p,0x2E0,read<u32>(p,0x2E0)|0x4000);
                write<s16>(a,0x57E,(s16)i);
                copy32(a,0x540,p,0x314); copy32(a,0x544,p,0x318); copy32(a,0x548,p,0x31C);
                write<s16>(a,0x580,call<s16>(0x025D6894,p,player_by_play()));
            }
        }
        write<s16>(a,0x59A,0); write<s16>(a,0x59C,0x96);
        write<u32>(pl,0x428,0); write<u32>(pl,0x430,0x1A); write<u16>(pl,0x420,3);   // DEMO_LOOKUP
        call(0x02514F2C,cam+0x248); call(0x02515280,cam+0x248,2);
        call(0x025E1904,0x1E);   // mDoAud_bgmStop(30)
        write<s16>(a,0x590,1);
        set_timer(0,regf_t(20,0,135.f));
        inc_state(a);
        {
            u32 p=poe(read<s16>(a,0x57E));
            if(p) { copy32(a,0x528,p,0x314); copy32(a,0x52C,p,0x318); copy32(a,0x530,p,0x31C); }
        }
    }
        /* fallthrough */
    case 0x79: {
        u32 p=poe(read<s16>(a,0x57E));
        if(p) {
            copy32(a,0x540,p,0x314); copy32(a,0x544,p,0x318); copy32(a,0x548,p,0x31C);
            write<s16>(a,0x580,call<s16>(0x025D6894,p,player_by_play()));
        }
        f32 mx=read<f32>(a,0x540);
        write<f32>(a,0x55C,50.f);
        f32 mz=read<f32>(a,0x548),my=read<f32>(a,0x544);
        write<f32>(a,0x528,mx); write<f32>(a,0x530,mz);
        call(0x0200ED84,a+0x52C,add(add(my,150.f),read<f32>(REGF(12,17))),0.1f,10.f);
        call(0x025F1884,read<u32>(calc_mtx_ptr),(s32)read<s16>(a,0x580));
        Local<Vec> v,out;
        v->x=0.f; v->y=0.f; v->z=800.f;
        call(0x0200FCD8,v.get(),out.get());
        f32 ox=add((f32)out->x,read<f32>(a,0x540));
        f32 oz=add((f32)out->z,read<f32>(a,0x548));
        f32 oy=fmadds(read<f32>(a,0x544),0.3f,(f32)out->y);
        out->x=ox; out->z=oz; out->y=oy;
        write<f32>(a,0x534,ox); write<f32>(a,0x538,add(oy,100.f)); write<f32>(a,0x53C,oz);
        if(timer(0)!=0) break;
        set_timer(0,regf_t(20,1,25.f));
        if(!reg20_step()) break;
        write<u32>(pl,0x430,1);   // DEMO_N_WAIT
        {
            Local<Vec> q;
            q->x=-36.f;
            f32 m424x=read<f32>(a,0x540);
            q->y=read<f32>(pl,0x318); q->z=-777.f;
            f32 px=read<f32>(pl,0x314),pz=read<f32>(pl,0x31C),m424z=read<f32>(a,0x548);
            u32 vt=read<u32>(pl,0xB4);
            s16 ang=call<s16>(0x020195B0,sub(px,m424x),sub(pz,m424z));
            call_ptr<void>(read<u32>(vt,0x114),pl,q.get(),(s32)ang);
        }
        inc_state(a);
        break;
    }
    case 0x7A: {
        s16 ang=call<s16>(0x025D6894,a,player_actor());
        write<s16>(a,0x32A,ang); write<s16>(a,0x322,ang); write<s16>(a,0x592,ang);
        call(0x0200ED84,a+0x55C,regf(20,2,50.f),1.f,0.5f);
        set_cam_pos(a,-484.f,628.f,-1495.f,-527.f,698.f,-1645.f);
        player_to_room_face(a,pl);
        s16 t0=timer(0);
        if(t0!=0) {
            if(t0==1) se_start_e(a,0x6A3C,0);   // JA_SE_OBJ_BPW_MASK_APPEAR
            break;
        }
        write<u32>(pl,0x430,0x31);   // DEMO_S_SURP
        call(0x0200ED84,a+0x330,1.f,1.f,0.1f);
        call(0x0200F428,a+0x59A,(s32)read<s16>(a,0x59C),1,0xF);
        f32 s=read<f32>(a,0x330);
        write<f32>(a,0x338,s); write<f32>(a,0x334,s);
        if(!reg20_step()) break;
        if(read<f32>(a,0x330)<0.9f) break;
        write<f32>(a,0x330,1.f); write<f32>(a,0x334,1.f); write<f32>(a,0x338,1.f);
        set_timer(0,regf_t(20,9,10.f));
        set_timer(1,regf_t(20,10,100.f));
        set_timer(4,regf_t(12,12,30.f));
        write<f32>(a,0x558,0.f);
        inc_state(a);
    }
        /* fallthrough */
    case 0x7B:
        if(timer(0)!=0) {
            set_timer(1,regf_t(20,10,100.f));
            set_timer(4,regf_t(12,12,30.f));
            break;
        }
        call(0x0200ED84,a+0x55C,regf(20,11,50.f),1.f,0.5f);
        demo_calc(a,0x528,regf(20,12,-114.f),0.1f,20,18,0.02f);
        demo_calc(a,0x52C,regf(20,13,95.f),0.1f,20,18,0.02f);
        demo_calc(a,0x530,regf(20,14,-293.f),0.1f,20,18,0.02f);
        demo_calc(a,0x534,regf(20,15,-169.f),1.f,20,18,0.02f);
        demo_calc(a,0x538,regf(20,16,98.f),1.f,20,18,0.02f);
        demo_calc(a,0x53C,regf(20,17,-462.f),1.f,20,18,0.02f);
        call(0x0200ED84,a+0x558,1.f,1.f,0.04f);
        if(read<u32>(a,0x514)==0x1B) {
            if(call<s32>(0x027F2BF8,read<u32>(a,0x3D0)+0x98,1.f)) mons_se(a,0x496A,0,true);    // LOOK_RIGHT
            if(call<s32>(0x027F2BF8,read<u32>(a,0x3D0)+0x98,41.f)) mons_se(a,0x496B,0,true);   // LOOK_LEFT
            if(call<s32>(0x027F2BF8,read<u32>(a,0x3D0)+0x98,76.f)) mons_se(a,0x496C,0,true);   // SURPRISE
        }
        if(timer(4)==1) anm_init(i_this,0x1B,8.f,0,1.f,-1);   // KYORO_GARN1
        if(timer(1)!=0) break;
        if(!reg20_step()) break;
        inc_state(a);
        /* fallthrough */
    case 0x7C:
        if(read<u32>(a,0x514)==0x1B) {
            if(call<s32>(0x027F2BF8,read<u32>(a,0x3D0)+0x98,1.f)) mons_se(a,0x496A,0,true);
            if(call<s32>(0x027F2BF8,read<u32>(a,0x3D0)+0x98,41.f)) mons_se(a,0x496B,0,true);
            if(call<s32>(0x027F2BF8,read<u32>(a,0x3D0)+0x98,76.f)) mons_se(a,0x496C,0,true);
        }
        if(!morf_isStop(read<u32>(a,0x3D0))) break;
        set_timer(0,regf_t(19,0,60.f));
        inc_state(a);
        /* fallthrough */
    case 0x7D:
        call(0x0200ED84,a+0x55C,regf(19,1,50.f),1.f,0.5f);
        set_cam_pos(a,61.f,125.f,-159.f,-146.f,39.f,-965.f);
        if(timer(0)!=0) break;
        if(!reg20_step()) break;
        anm_init(i_this,9,8.f,2,1.f,-1);
        write<f32>(a,0x558,0.f);
        set_timer(0,4);
        inc_state(a);
        /* fallthrough */
    case 0x7E: {
        if(timer(0)==0) se_start(a,0x516C,0);   // JA_SE_CM_BPW_MASK_RUN_AWAY
        call(0x0200ED84,a+0x55C,regf(19,9,50.f),1.f,0.5f);
        demo_calc(a,0x528,regf(19,10,-471.f),0.1f,19,12,0.05f);
        demo_calc(a,0x52C,regf(19,10,120.f),0.1f,19,12,0.05f);
        demo_calc(a,0x530,regf(19,11,-498.f),0.1f,19,12,0.05f);
        demo_calc(a,0x534,regf(20,15,115.f),1.f,19,12,0.05f);
        demo_calc(a,0x538,regf(20,16,58.f),1.f,19,12,0.05f);
        demo_calc(a,0x53C,regf(20,17,-1062.f),1.f,19,12,0.05f);
        call(0x0200ED84,a+0x558,1.f,1.f,0.04f);
        call(0x0200ED84,a+0x314,-1130.f,1.f,regf(19,13,20.f));
        call(0x0200ED84,a+0x31C,30.f,1.f,regf(19,13,20.f));
        f32 dx=sub(read<f32>(a,0x314),-1130.f);
        f32 dz=sub(read<f32>(a,0x31C),30.f);
        write<s16>(a,0x592,(s16)(call<s16>(0x020195B0,dx,dz)+0x8000));
        player_to_room_face(a,pl);
        if(!(call<f32>(0x028F4384,fmadds(dx,dx,mul(dz,dz)))<5.f)) break;
        write<f32>(a,0x374,0.f);
        write<f32>(a,0x33C,0.f); write<f32>(a,0x340,0.f); write<f32>(a,0x344,0.f);
        write<f32>(a,0x370,0.f);
        call(0x0200F428,a+0x592,-0x8000,1,0x2000);
        if(!reg20_step()) break;
        write<s16>(a,0x598,0);
        set_timer(0,regf_t(18,5,63.f));
        set_timer(1,regf_t(18,6,75.f));
        call(0x025E1934,0xC0000003u);   // mDoAud_bgmStreamPrepare(JA_STRM_BOSS_CLEAR)
        inc_state(a);
        write<f32>(a,0x558,0.f);
        break;
    }
    case 0x7F: {
        if(timer(0)!=0) se_start(a,0x516C,0);
        if(timer(1)==1) call(0x025E1944);   // mDoAud_bgmStreamPlay()
        u16 ph=(u16)ftoi(add((f32)read<s16>(a,0x598),regf(18,2,700.f)));
        write<u16>(a,0x598,ph);
        f32 sn=cM_ssin(ph);
        write<f32>(a,0x4C8,mul(regf(18,3,30.f),sn));
        write<f32>(a,0x4D0,mul(regf(18,4,30.f),sn));
        call(0x0200F428,a+0x592,-0x8000,1,0x2000);
        write<f32>(a,0x340,regf(19,14,30.f));
        call(0x0200ED84,a+0x55C,regf(19,15,50.f),1.f,0.5f);
        demo_calc(a,0x528,regf(19,16,-538.f),0.1f,19,19,0.04f);
        demo_calc(a,0x52C,regf(19,17,748.f),0.1f,19,19,0.04f);
        demo_calc(a,0x530,regf(19,18,-567.f),0.1f,19,19,0.04f);
        call(0x0200ED84,a+0x558,1.f,1.f,0.04f);
        if(timer(0)==1) {
            write<s16>(a,0x512,4);
            se_start_e(a,0x6A3F,0);   // JA_SE_OBJ_BPW_LIGHT_ON
            set_timer(5,0); write<u8>(a,0x507,3);
            anm_init(i_this,0xE,regf(12,10,8.f),2,1.f,-1);   // GATAGATA1
        }
        if(!(read<f32>(a,0x318)>2000.f)) break;
        write<f32>(a,0x370,0.f);
        write<f32>(a,0x33C,0.f); write<f32>(a,0x340,0.f); write<f32>(a,0x344,0.f);
        if(!reg20_step()) break;
        write<s16>(a,0x512,4);
        mons_se(a,0x496D,0,false);   // JA_SE_CV_BPW_DIE
        set_timer(0,regf_t(18,8,40.f));
        inc_state(a);
        break;
    }
    case 0x80:
        if(timer(1)==1) call(0x025E1944);
        if(timer(0)!=0) break;
        write<f32>(a,0x4C8,0.f); write<f32>(a,0x4CC,0.f); write<f32>(a,0x4D0,0.f);
        write<s16>(a,0x59C,0xFF);
        inc_state(a);
        /* fallthrough */
    case 0x81:
        if(timer(1)==1) call(0x025E1944);
        call(0x0200F428,a+0x59A,(s32)read<s16>(a,0x59C),1,5);
        call(0x0200ED84,a+0x55C,regf(18,9,50.f),1.f,0.5f);
        write<s16>(a,0x592,-0x483F); write<s16>(a,0x32A,-0x483F);
        write<s16>(a,0x328,-0x2337); write<s16>(a,0x32C,0x3E9);
        set_cam_pos(a,-1030.f,2017.f,119.f,-1332.f,2722.f,-153.f);
        if(read<s16>(a,0x59A)<0xFE) break;
        if(!reg20_step()) break;
        write<u8>(a,0x4FC,1); write<s16>(a,0x59A,0xFF);
        write<f32>(read<u32>(a,0x3D0),0x98,0.f);   // mpMorf->setPlaySpeed(0)
        set_timer(0,regf_t(18,17,40.f));
        inc_state(a);
        /* fallthrough */
    case 0x82:
        if(timer(0)!=0) break;
        set_timer(0,regf_t(18,18,20.f));
        write<f32>(a,0x374,-1.f);
        inc_state(a);
        /* fallthrough */
    case 0x83:
        call(0x0200F428,a+0x328,(s32)(s16)ftoi(regf(18,19,49152.f)),1,0x300);
        if(timer(0)!=0) break;
        inc_state(a);
        write<f32>(a,0x374,-3.f);
        /* fallthrough */
    case 0x84:
        call(0x0200ED84,a+0x55C,regf(6,7,50.f),1.f,0.5f);
        set_cam_pos(a,-893.f,1874.f,66.f,142.f,68.f,-1348.f);
        if(!reg20_step()) break;
        write<u32>(pl,0x430,0x1D);   // DEMO_UNK_029
        write<f32>(a,0x558,0.f);
        inc_state(a);
        /* fallthrough */
    case 0x85:
        call(0x0200F428,a+0x328,(s32)(s16)ftoi(regf(18,19,49152.f)),1,0x300);
        call(0x0200F428,a+0x32C,0,1,0x300);
        call(0x0200ED84,a+0x55C,regf(6,7,50.f),1.f,0.5f);
        demo_calc(a,0x528,regf(6,8,-1352.f),0.1f,6,6,0.05f);
        demo_calc(a,0x52C,regf(6,9,174.f),0.1f,6,6,0.05f);
        demo_calc(a,0x530,regf(6,10,680.f),0.1f,6,6,0.05f);
        call(0x0200ED84,a+0x558,1.f,1.f,0.04f);
        call(0x0200EDC8,a+0x4CC,1.f,3.f);
        call(0x0200ED84,a+0x5A8,-120.f,1.f,3.f);
        player_to_room_face(a,pl);
        if(add(add(read<f32>(a,0x878),1350.f),read<f32>(REGF(6,19)))<read<f32>(a,0x318)) break;
        if(!reg20_step()) break;
        write<f32>(a,0x558,0.f);
        inc_state(a);
        /* fallthrough */
    case 0x86: {
        call(0x0200ED84,a+0x55C,regf(6,11,50.f),1.f,0.5f);
        demo_calc(a,0x528,regf(6,12,-2340.f),0.1f,6,18,0.1f);
        {   // m40C.y: target REG12_F(13) + 186, scale 0.5, step from REG6_F(13) + 36
            f32 v=read<f32>(a,0x52C);
            f32 d=__builtin_fabsf(sub(v,regf(6,13,36.f)));
            f32 k=regf(6,18,0.1f);
            f32 step=mul(mul(d,k),read<f32>(a,0x558));
            call(0x0200ED84,a+0x52C,regf(12,13,186.f),0.5f,step);
        }
        demo_calc(a,0x530,regf(6,14,1442.f),0.1f,6,18,0.1f);
        demo_calc(a,0x534,regf(6,15,-744.f),1.f,6,18,0.1f);
        demo_calc(a,0x538,regf(6,16,74.f),1.f,6,18,0.1f);
        demo_calc(a,0x53C,regf(6,17,-432.f),1.f,6,18,0.1f);
        call(0x0200ED84,a+0x558,1.f,1.f,0.04f);
        call(0x0200EDC8,a+0x4CC,1.f,3.f);
        call(0x0200ED84,a+0x5A8,-120.f,1.f,3.f);
        if(add(read<f32>(a,0x878),read<f32>(REGF(12,14)))<read<f32>(a,0x318)) break;
        if(!reg20_step()) break;
        u32 play=dComIfGp_get();
        u32 em=call<u32>(0x025A847C,read<u32>(play,0x5AB0),0,0x8456,a+0x314,a+0x328,0,0xFF,0,-1,0,0,0);   // BPWBREAKMASK00
        if(em) {
            u32 env=call<u32>(0x02555D0C);
            u8 b=read<u8>(env,0xB72),r=read<u8>(env,0xB70),g=read<u8>(env,0xB71);
            write<u8>(em,0x244,r); write<u8>(em,0x245,g); write<u8>(em,0x246,b);
        }
        se_start_e(a,0x583C,0);   // JA_SE_CM_BOSS_EXPLODE
        {
            Local<Vec> p;
            f32 k=regf(8,5,80.f);
            p->x=read<f32>(a,0x314);
            f32 y=read<f32>(a,0x318);
            p->y=y; p->z=read<f32>(a,0x31C);
            p->y=add(y,k);
            call(0x025D99E8,a,p.get(),0xF,2,0xFF);   // fopAcM_createDisappear(HEART_CONTAINER)
        }
        if(a+0x37C) {
            s32 reverb=call<s32>(0x02520540,(s32)read<s8>(a,0x326));
            call(0x025E1A40,0x596Du,a+0x37C,0u,reverb);   // JA_SE_CM_BPW_MASK_BREAK
        }
        write<f32>(a,0x330,0.f); write<f32>(a,0x334,0.f);
        inc_state(a);
        write<f32>(a,0x338,0.f);
        set_timer(0,regf_t(12,16,120.f));
    }
        /* fallthrough */
    case 0x87: {
        if(timer(0)!=0) {
            call(0x0200ED84,a+0x52C,regf(12,13,186.f),0.3f,regf(12,14,100.f));
            break;
        }
        player_to_room_body(a,pl);
        write<f32>(a,0x528,0.f); write<f32>(a,0x52C,0.f); write<f32>(a,0x530,0.f);
        f32 rad=regf(9,1,1400.f);
        write<f32>(a,0x50C,rad);
        u16 an=(u16)ftoi(regf(9,2,-15000.f));
        write<u16>(a,0x510,an);
        write<f32>(a,0x534,mul(cM_ssin(an),rad));
        write<f32>(a,0x538,regf(9,3,900.f));
        write<f32>(a,0x53C,mul(cM_scos(an),rad));
        if(!reg20_step()) break;
        write<s16>(a,0x57E,0);
        if(read<s16>(REGS(0,3))==0) call(0x025B9098,read<u32>(0x101F84DC)+0x798,3);   // dComIfGs_onStageBossEnemy()
        set_timer(1,regf_t(9,4,80.f));
        set_timer(2,regf_t(9,5,470.f));
        set_timer(9,0);
        inc_state(a);
    }
        /* fallthrough */
    case 0x88: {
        player_to_room_body(a,pl);
        s16 t9=timer(9),t2=timer(2);
        if(t9==1) { write<f32>(a,0x508,0.f); write<u8>(a,0x507,6); }
        if(t2!=0) {
            if(timer(1)==1) {
                call(0x025D9874,a+0x468,0,(s32)read<s8>(a,0x326),0);   // fopAcM_createWarpFlower
                set_timer(9,(s16)(read<s16>(REGS(6,5))+30));
            }
            call(0x0200ED84,a+0x55C,regf(9,6,50.f),1.f,0.5f);
            s16 an=(s16)(read<s16>(a,0x510)+(s16)ftoi(regf(9,7,33.f)));
            if(an>0) an=0;
            write<s16>(a,0x510,an);
            call(0x0200ED84,a+0x50C,regf(9,8,600.f),1.f,regf(9,9,2.f));
            u16 a1=read<u16>(a,0x510);
            write<f32>(a,0x534,mul(cM_ssin(a1),read<f32>(a,0x50C)));
            call(0x0200ED84,a+0x538,regf(9,10,900.f),1.f,regf(9,11,10.f));
            u16 a2=read<u16>(a,0x510);
            f32 c=cM_scos(a2);
            write<f32>(a,0x53C,mul(c,read<f32>(a,0x50C)));
            break;
        }
        if(!(read<f32>(REGF(9,13))==0.f)) { write<s16>(a,0x562,0x87); set_timer(0,0); break; }
        if(!(read<f32>(REGF(9,12))==0.f)) { write<s16>(a,0x562,0x87); write<f32>(REGF(9,12),0,0.f); set_timer(0,0); break; }
        if(!reg20_step()) break;
        cam_reset_start(a,cam,pl);
        call(0x025D57E0,a);
        break;
    }
    }
    if(read<s16>(a,0x562)>=0x78) {
        write<f32>(a,0x5B4,0.f); write<f32>(a,0x4CC,100.f); write<f32>(a,0x5B8,160.f);
    }
    if(read<s16>(a,0x590)!=0) {
        Local<Vec> center,eye;
        eye->x=read<f32>(a,0x534); center->y=read<f32>(a,0x52C);
        eye->z=read<f32>(a,0x53C); center->x=read<f32>(a,0x528);
        eye->y=read<f32>(a,0x538); center->z=read<f32>(a,0x530);
        call(0x02514F88,cam+0x248,center.get(),eye.get(),read<f32>(a,0x55C),0);
    }
}
VERIFY(0x020D8564,action_bunri_dousa);
