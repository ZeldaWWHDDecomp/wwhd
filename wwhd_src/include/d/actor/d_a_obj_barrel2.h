// Barrel and sea mine layout; GameCube field names with the HD actor allocation.
// HD: Attr_c remains 0x74; the actor fields shift by 0x11C after the 0x3AC base,
// growing Act_c from 0x4A8 to 0x5C4. Private prefixes describe only verified access.
#pragma once
#include "bindings.h"
#include "d/actor/d_a_obj_buoyflag.h"
#include "f_op/f_op_camera.h"
#include "gc_api/gc_api.h"
namespace daObjBarrel2 {
struct Attr_c {
    be<u16> m00, m02;
    be<u32> m04;
    be<f32> m08;
    be<f32> m0C;
    be<f32> m10;
    be<f32> m14;
    be<f32> m18;
    be<f32> m1C;
    be<f32> m20;
    be<f32> m24;
    be<f32> m28;
    be<f32> m2C;
    be<f32> m30;
    be<f32> m34;
    be<f32> m38;
    be<f32> m3C;
    be<f32> m40;
    be<f32> m44;
    be<f32> m48;
    be<f32> m4C;
    be<f32> m50;
    be<f32> m54;
    be<s16> m58;
    u8 _5a[2];
    be<f32> m5C;
    be<f32> m60;
    be<f32> m64;
    be<f32> m68;
    be<f32> m6C;
    be<f32> m70;
};
struct Act_c : fopAc_ac_c {
    enum Prm_e {
        PRM_ITEM_SAVE_W = 7,
        PRM_ITEM_SAVE_S = 16,
        PRM_ITEM_NO_W = 6,
        PRM_ITEM_NO_S = 0,
        PRM_BUOY_W = 1,
        PRM_BUOY_S = 8,
        PRM_TYPE_W = 2,
        PRM_TYPE_S = 24,
        PRM_TEXTURE_W = 1,
        PRM_TEXTURE_S = 10,
        PRM_COMING_W = 1,
        PRM_COMING_S = 28
    };
    enum Mode_e {
        MODE_AFL = 0,
        MODE_EXIT_V = 1,
        MODE_EXIT_H = 2,
        MODE_EXIT_MINE = 3,
        MODE_EXPLODE = 4,
        MODE_DEMO_BREAK0 = 5,
        MODE_DEMO_BREAK1 = 6,
        MODE_DEMO_EXPLODE0 = 7,
        MODE_DEMO_EXPLODE1 = 8
    };
    bool demo_mode_chk();
    void mode_afl_init();
    void mode_exit_v_init();
    void mode_exit_h_init();
    void mode_exit_mine_init();
    void mode_demo_break1_init();
    void mode_demo_explode1_init();
    void mode_demo_explode0_init();
    void mode_demo_break0_init();
    void mode_explode_init();
    bool _delete();
    void item_give();
    u32 item_delete();
    void item_drop();
    void item_connect_check();
    void init_mtx();
    BOOL mode_proc_call();
    bool _draw();
    void cull_set_move();
    void cull_set_draw();
    void set_pos_y();
    bool mine_chk_range_explode();
    bool mine_chk_range_flash();
    bool mode_exit_v();
    bool mode_exit_h();
    bool mode_exit_mine();
    bool _execute();
    bool mode_demo_break1();
    bool mode_demo_explode1();
    bool mode_demo_break0();
    bool mode_demo_explode0();
    void item_drop_init(f32 speed);
    void set_item_position();
    void buoy_jump(f32 speed);
    bool mine_chk_range_damage();
    bool mode_explode();
    void afl_sway();
    bool create_heap();
    void set_water_pos();
    void eff_break();
    void eff_explode();
    void set_mtx();
    bool mode_afl();
    void execute_sub();
    s32 _create();

    /* 0x3AC */ request_of_phase_process_class mPhase;
    /* 0x3B4 */ gptr<J3DModel> m298;
    /* 0x3B8 */ gptr<mDoExt_brkAnm> m29C;
    /* 0x3BC */ dCcD_Stts mStts;
    /* 0x3F8 */ dCcD_Cyl mCyl;
    /* 0x528 */ be<s32> m40C;
    /* 0x52C */ be<s32> m410;
    /* 0x530 */ be<f32> m414;
    /* 0x534 */ be<f32> m418;
    /* 0x538 */ be<f32> m41C;
    /* 0x53C */ cXyz m420;
    /* 0x548 */ be<f32> m42C;
    /* 0x54C */ be<f32> m430;
    /* 0x550 */ be<f32> m434;
    /* 0x554 */ be<f32> m438;
    /* 0x558 */ be<s16> m43C;
    u8 _55a[2];
    /* 0x55C */ be<f32> m440;
    /* 0x560 */ be<f32> m444;
    /* 0x564 */ be<f32> m448;
    /* 0x568 */ be<f32> m44C;
    /* 0x56C */ be<f32> m450;
    /* 0x570 */ be<f32> m454;
    /* 0x574 */ be<u32> mItemId;
    /* 0x578 */ be<f32> m45C;
    /* 0x57C */ be<u32> m460;
    /* 0x580 */ be<s32> m464;
    /* 0x584 */ be<u8> m468;
    u8 _585;
    /* 0x586 */ be<s16> m46A;
    /* 0x588 */ be<u8> m46C;
    /* 0x589 */ be<u8> m46D;
    u8 _58e[2];
    /* 0x58C */ be<s32> m470;
    /* 0x590 */ be<u8> m474;
    /* 0x591 */ be<u8> m475;
    /* 0x592 */ be<u8> m476;
    u8 _593;
    /* 0x594 */ Mtx34 m478;
};
} // namespace daObjBarrel2
WWHD_SIZE(daObjBarrel2::Attr_c, 0x74);
WWHD_OFFSET(daObjBarrel2::Act_c, mPhase, 0x3AC);
WWHD_OFFSET(daObjBarrel2::Act_c, mStts, 0x3BC);
WWHD_OFFSET(daObjBarrel2::Act_c, mCyl, 0x3F8);
WWHD_OFFSET(daObjBarrel2::Act_c, m40C, 0x528);
WWHD_OFFSET(daObjBarrel2::Act_c, m474, 0x590);
WWHD_OFFSET(daObjBarrel2::Act_c, m478, 0x594);
WWHD_SIZE(daObjBarrel2::Act_c, 0x5C4);

void Barrel_assertion(const char *, s32, const char *);
void Barrel_raceItemForceGet(fopAc_ac_c *);
u32 Barrel_deleteActor(fopAc_ac_c *);
f32 Barrel_playerDistance2(fopAc_ac_c *, void *);
BOOL Barrel_searchById(u32, gptr<fopAc_ac_c> *);
BOOL Barrel_checkOffsetPos(fopAc_ac_c *);
void Barrel_polyInfoDestroy(void *, s32);
void Barrel_cylinderDestroy(dCcD_Cyl *, s32);
void Barrel_statusDestroy(dCcD_Stts *, s32);
void Barrel_actorDestroy(fopAc_ac_c *, s32);
void Barrel_destructorChain(void *);
void Barrel_byteConstructor(void *);
void Barrel_otherByteConstructor(void *);
void Barrel_setCullSphere(fopAc_ac_c *, f32, f32, f32, f32);
f32 Barrel_actorDistanceXZ2(fopAc_ac_c *, void *);
BOOL Barrel_cullingCheck(fopAc_ac_c *);
BOOL Barrel_eventEndCheck(void *, const char *);
s32 Barrel_eventIndex(void *, const char *, u8);
void *Barrel_eventData(void *, s32);
void Barrel_orderOtherEvent(fopAc_ac_c *, const char *, u16, u16);
void Barrel_endOffsetPos(fopAc_ac_c *, f32, cXyz *, f32, f32, csXyz *);
void Barrel_setItemMatrix(fopAc_ac_c *, cXyz *);
void Barrel_startOffsetPos(fopAc_ac_c *);
void *Barrel_searchActor(void *, be<u32> *);
void Barrel_copyMatrix(void *, void *);
f32 Barrel_innerProduct(cXyz *, cXyz *);
s32 Barrel_reverb(s8);
void Barrel_sound(u32, void *, s32, s32);
f32 Barrel_sqrt(f32);
void *Barrel_allocate(u32);
void *Barrel_brkConstruct(void *);
s32 Barrel_brkInit(void *, J3DModelData *, void *, s32, s32, f32, s32, s32, s32, s32);
BOOL Barrel_seaCheckArea(f32, f32);
f32 Barrel_seaCalcWave(f32, f32);
void Barrel_outProduct(cXyz *, cXyz *, cXyz *);
void Barrel_normalizeZP(cXyz *, cXyz *);
void Barrel_waterCheckConstruct(void *);
BOOL Barrel_specialGroupCheck(void *, void *);
void Barrel_normalize(cXyz *, cXyz *);
void *Barrel_particleSet(void *, s32, u32, cXyz *, csXyz *, cXyz *, u8, void *, s32, void *, void *,
                         void *);
void Barrel_particleBombSmoke(void *, u32, cXyz *, csXyz *, cXyz *, u8);
void Barrel_createWaterPillar(cXyz *, f32, f32, s32);
void Barrel_scaleVector(cXyz *, cXyz *, f32);
void Barrel_quaternionRotBaseY(void *, cXyz *);
void Barrel_quaternionMatrix(void *);
void Barrel_zxyRotation(Mtx34 *, s16, s16, s16);
void Barrel_zRotation(Mtx34 *, s16);
void Barrel_matrixVectorSR(Mtx34 *, cXyz *, cXyz *);
void Barrel_setCylinderCenter(void *, cXyz *);
void Barrel_registerCollision(void *, dCcD_Cyl *);
BOOL Barrel_brkPlay(mDoExt_brkAnm *);
void Barrel_statusConstruct(void *);
void Barrel_gameStatusConstruct(void *);
void Barrel_cylinderConstruct(dCcD_Cyl *);
void Barrel_cylinderShapeConstruct(void *);
BOOL Barrel_entrySolidHeap(fopAc_ac_c *, void *, u32);
void Barrel_setCylinder(dCcD_Cyl *, void *);
void Barrel_setCylinderRadius(void *, f32);
void Barrel_setCylinderHeight(void *, f32);
void Barrel_angleConstruct(csXyz *, s16, s16, s16);
u32 Barrel_createRaceItem(cXyz *, s32, s32, s8, csXyz *, cXyz *, BOOL);
u32 Barrel_createChild(s32, u32, u32, cXyz *, s8, csXyz *, cXyz *, s32, void *);
u32 PrmAbstract(fopAc_ac_c *, u32, u32);

namespace barrel2_hd {
struct Attributes {
    daObjBarrel2::Attr_c mData[4];
};
struct ItemPointer {
    gptr<fopAc_ac_c> mpActor;
};
using ModeFunction = BOOL (daObjBarrel2::Act_c::*)();
using ModePtmf = gabi::ptmf<ModeFunction>;
struct ModeTable {
    ModePtmf mData[9];
};
struct Ship : fopAc_ac_c {
    u8 mPrefix[0x644 - 0x3AC];
    be<u32> mFlags;
};
struct WaterCheck {
    u8 mPrefix[0x10];
    cBgS_PolyInfo mPolyInfo;
    be<u32> mVtable;
    be<u32> mPolyVtable;
    u8 mPadding[8];
    be<u32> mGroupVtable;
    u8 mPadding2[4];
    cXyz mPos;
    be<f32> mUpperY;
    be<f32> mHeight;
    u8 mTail[4];
};
struct DestructorChain {
    be<u32> mNext, mObject, mDestructor;
};
struct Angles {
    csXyz mZero;
};
struct SinCos {
    be<f32> mSin, mCos;
};
struct SinTable {
    SinCos mData[8192];
};
struct Emitter {
    u8 mPrefix[0x58];
    be<f32> mSpread;
    u8 mPadding[4];
    be<u16> mLifeTime;
    u8 mPadding2[10];
    be<f32> mAwayFromAxisSpeed;
    be<f32> mDirectionalSpeed;
};
struct ShapeVtables {
    u8 mPrefix[0x110];
    be<u32> mShapeVtable;
    be<u32> mAabVtable;
};
WWHD_SIZE(Attributes, 0x1D0);
WWHD_SIZE(ItemPointer, 4);
WWHD_SIZE(ModeTable, 72);
WWHD_SIZE(Ship, 0x648);
WWHD_OFFSET(Ship, mFlags, 0x644);
WWHD_OFFSET(WaterCheck, mPos, 0x38);
WWHD_OFFSET(WaterCheck, mHeight, 0x48);
WWHD_SIZE(WaterCheck, 0x50);
WWHD_SIZE(DestructorChain, 12);
WWHD_SIZE(Angles, 6);
WWHD_SIZE(SinCos, 8);
WWHD_SIZE(SinTable, 0x10000);
WWHD_OFFSET(Emitter, mLifeTime, 0x60);
WWHD_OFFSET(Emitter, mAwayFromAxisSpeed, 0x6C);
WWHD_SIZE(Emitter, 0x74);
WWHD_SIZE(ShapeVtables, 0x118);
} // namespace barrel2_hd

namespace barrel2_hd {
struct InitializerData {
    be<f32> mMinAngle, mMaxAngle;
    be<u8> mByte, mOtherByte;
    u8 mPadding[2];
    be<u32> mWords[4];
};
struct DestructorChains {
    DestructorChain mData[4];
};
WWHD_SIZE(InitializerData, 0x1C);
WWHD_SIZE(DestructorChains, 48);
enum : u32 {
    WaterVtable = 0x10025778,
    WaterPolyVtable = 0x10025798,
    WaterGroupVtable = 0x10025768,
    ActorVtable = 0x100257D8,
    StatusVtable = 0x1004AE88,
    GameStatusVtable = 0x1004AEC0,
    AabBaseVtable = 0x100015A8,
    ShapeVtable = 0x10025758,
    AabVtable = 0x1004B160,
    HitInfoVtable = 0x1004B108,
    CylinderVtable = 0x1004B150,
    ResourceStringVtable = 0x10025740
};
} // namespace barrel2_hd

namespace barrel2_hd {
enum : u32 { SearchByIdJudge = 0x025E1234 };
struct CylinderSource {
    dCcD_SrcCyl mData;
};
} // namespace barrel2_hd

WWHD_OFFSET(daObjBarrel2::Act_c, m298, 0x3B4);
WWHD_OFFSET(daObjBarrel2::Act_c, m29C, 0x3B8);
WWHD_OFFSET(daObjBarrel2::Act_c, m410, 0x52C);
WWHD_OFFSET(daObjBarrel2::Act_c, m414, 0x530);
WWHD_OFFSET(daObjBarrel2::Act_c, m41C, 0x538);
WWHD_OFFSET(daObjBarrel2::Act_c, m420, 0x53C);
WWHD_OFFSET(daObjBarrel2::Act_c, m43C, 0x558);
WWHD_OFFSET(daObjBarrel2::Act_c, m440, 0x55C);
WWHD_OFFSET(daObjBarrel2::Act_c, m450, 0x56C);
WWHD_OFFSET(daObjBarrel2::Act_c, m454, 0x570);
WWHD_OFFSET(daObjBarrel2::Act_c, mItemId, 0x574);
WWHD_OFFSET(daObjBarrel2::Act_c, m45C, 0x578);
WWHD_OFFSET(daObjBarrel2::Act_c, m460, 0x57C);
WWHD_OFFSET(daObjBarrel2::Act_c, m464, 0x580);
WWHD_OFFSET(daObjBarrel2::Act_c, m46A, 0x586);
WWHD_OFFSET(daObjBarrel2::Act_c, m46C, 0x588);
WWHD_OFFSET(daObjBarrel2::Act_c, m470, 0x58C);
