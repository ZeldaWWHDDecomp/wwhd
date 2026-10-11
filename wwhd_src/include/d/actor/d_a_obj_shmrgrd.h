/* Spiked Skull Hammer switch. HD: fields after the actor prefix move by +0x118.
 * Retail unknown field m85C retained. */
#pragma once
#include "bindings.h"
#include "gc_api/d/d_com_inf_game.h"
#include "d/d_particle.h"
struct daObjShmrgrd_c : fopAc_ac_c {
    static const char M_arcname[8];
    enum Mode_e { MODE_UPPER, MODE_U_L, MODE_LOWER };
    enum Prm_e { PRM_SWSAVE_W = 8, PRM_SWSAVE_S = 0 };
    daObjShmrgrd_c *search_target_next(daObjShmrgrd_c *);
    daObjShmrgrd_c *search_gap();
    void set_gap_co();
    void set_damage();
    void eff_crush();
    void mode_lower();
    s32 create_heap();
    s32 _create();
    bool _delete();
    s32 _execute();
    s32 _draw();
    /* 0x3AC */ gptr<daObjShmrgrd_c> mpNext;
    /* 0x3B0 */ gptr<J3DModel> mpModel;
    /* 0x3B4 */ request_of_phase_process_class mPhs;
    /* 0x3BC */ dCcD_Cyl mCylCo;
    /* 0x4EC */ dCcD_Stts mSttsCo;
    /* 0x528 */ dCcD_Cyl mCylTg;
    /* 0x658 */ dCcD_Stts mSttsTg;
    /* 0x694 */ dCcD_Cyl mCylAt;
    /* 0x7C4 */ dCcD_Stts mSttsAt;
    /* 0x800 */ dCcD_Cyl mCylGapCo;
    /* 0x930 */ dCcD_Stts mSttsGapCo;
    /* 0x96C */ be<s32> mMode;
    /* 0x970 */ be<s16> mCrushTimer;
    /* 0x972 */ be<u8> mCrushState;
    /* 0x973 */ u8 pad;
    /* 0x974 */ be<f32> m85C;
    /* 0x978 */ be<f32> mScaleY;
    /* 0x97C */ be<f32> mAngleZ;
    /* 0x980 */ be<f32> mAngleX;
    /* 0x984 */ be<f32> mAngleSpeedZ;
    /* 0x988 */ be<f32> mAngleSpeedX;
    /* 0x98C */ be<f32> mTargetHFrac;
    /* 0x990 */ be<f32> mCurHFrac;
    /* 0x994 */ be<f32> mVSpeed;
    /* 0x998 */ be<f32> mTopPos;
    /* 0x99C */ dPa_smokeEcallBack mSmokeCb;
    /* 0x9BC */ Mtx34 mMtx;
    /* 0x9EC */ gptr<dBgW> mpBgW;
    void mode_lower_init();
    void mode_upper_init();
    void mode_u_l_init();
    void mode_upper();
    void mode_u_l();
    void vib_proc();
    void crush_proc();
    void calc_top_pos();
    void vib_start(s16, f32);
    void crush_start();
    void register_list();
    void leave_list();
    s32 check_player_angle(fopAc_ac_c *);
    void init_mtx();
    void set_mtx();
};
WWHD_OFFSET(daObjShmrgrd_c, mpNext, 0x3AC);
WWHD_OFFSET(daObjShmrgrd_c, mpModel, 0x3B0);
WWHD_OFFSET(daObjShmrgrd_c, mPhs, 0x3B4);
WWHD_OFFSET(daObjShmrgrd_c, mCylCo, 0x3BC);
WWHD_OFFSET(daObjShmrgrd_c, mSttsCo, 0x4EC);
WWHD_OFFSET(daObjShmrgrd_c, mCylTg, 0x528);
WWHD_OFFSET(daObjShmrgrd_c, mSttsTg, 0x658);
WWHD_OFFSET(daObjShmrgrd_c, mCylAt, 0x694);
WWHD_OFFSET(daObjShmrgrd_c, mSttsAt, 0x7C4);
WWHD_OFFSET(daObjShmrgrd_c, mCylGapCo, 0x800);
WWHD_OFFSET(daObjShmrgrd_c, mSttsGapCo, 0x930);
WWHD_OFFSET(daObjShmrgrd_c, mMode, 0x96C);
WWHD_OFFSET(daObjShmrgrd_c, mCrushTimer, 0x970);
WWHD_OFFSET(daObjShmrgrd_c, mCrushState, 0x972);
WWHD_OFFSET(daObjShmrgrd_c, pad, 0x973);
WWHD_OFFSET(daObjShmrgrd_c, m85C, 0x974);
WWHD_OFFSET(daObjShmrgrd_c, mScaleY, 0x978);
WWHD_OFFSET(daObjShmrgrd_c, mAngleZ, 0x97C);
WWHD_OFFSET(daObjShmrgrd_c, mAngleX, 0x980);
WWHD_OFFSET(daObjShmrgrd_c, mAngleSpeedZ, 0x984);
WWHD_OFFSET(daObjShmrgrd_c, mAngleSpeedX, 0x988);
WWHD_OFFSET(daObjShmrgrd_c, mTargetHFrac, 0x98C);
WWHD_OFFSET(daObjShmrgrd_c, mCurHFrac, 0x990);
WWHD_OFFSET(daObjShmrgrd_c, mVSpeed, 0x994);
WWHD_OFFSET(daObjShmrgrd_c, mTopPos, 0x998);
WWHD_OFFSET(daObjShmrgrd_c, mSmokeCb, 0x99C);
WWHD_OFFSET(daObjShmrgrd_c, mMtx, 0x9BC);
WWHD_OFFSET(daObjShmrgrd_c, mpBgW, 0x9EC);
WWHD_SIZE(daObjShmrgrd_c, 0x9F0);

void shmrgrdSound(u32, cXyz *, s32, s32);

s32 shmrgrdTargetAngle(cXyz *, cXyz *);
s32 shmrgrdIsSwitch(dSv_info_c *, s32, s32);
void shmrgrdOnSwitch(dSv_info_c *, s32, s32);
void shmrgrdDelete(void *);
void shmrgrdDestroyActor(fopAc_ac_c *, s32);
void shmrgrdSubtract(const cXyz *, cXyz *, const cXyz *);
s32 shmrgrdNormalizeRS(cXyz *);
f32 shmrgrdSqrt(f32);
void shmrgrdHitSeStart(cXyz *, s32, dCcD_Cyl *, s32);
void shmrgrdHitEffHibana(fopAc_ac_c *, dCcD_Cyl *);
daObjShmrgrd_c *shmrgrdAllocate(u32);
void shmrgrdConstructSmoke(dPa_smokeEcallBack *, s32);
dBgW *shmrgrdBgNewSet(cBgD_t *, u32, Mtx34 *);
void shmrgrdZXYrotM(Mtx34 *, s16, s16, s16);
void shmrgrdQuatRotBaseY2(Quaternion *, cXyz *);
void shmrgrdRegisterCollision(cCcS *, dCcD_GObjInf *);

enum {
    ShmrgrdActorVtable = 0x1002F458,
    ShmrgrdShapeVtable = 0x100015A8,
    ShmrgrdAabVtable = 0x1002F448,
    ShmrgrdGeometryVtable = 0x1004B150,
    ShmrgrdCylinderShapeVtable = 0x1004B160,
    ShmrgrdHitVtable = 0x1004B108,
    dRes_INDEX_SHMRGRD_BDL_SHMRGRD_e = 4,
    dRes_INDEX_SHMRGRD_DZB_HGBASE_e = 7,
    SHMRGRD_JNT_HIT_e = 2,
};
struct ShmrgrdShapeView {
    u8 _00[0x18];
    be<u32> mAabVtable, mShapeVtable;
};
WWHD_SIZE(ShmrgrdShapeView, 0x20);
void *shmrgrdGetResource(u8 *, be<u32> *, s32);
void shmrgrdConstructGObj(dCcD_GObjInf *);
void shmrgrdConstructCyl(cM3dGCyl *);

struct ShmrgrdRegistration {
    be<u32> words[3];
};
struct ShmrgrdEmitterView {
    u8 _00[0x14];
    cXyz mLocalTranslation;
    u8 _20[0x14];
    be<f32> mRate;
    u8 _38[0x24];
    be<u32> mMaxFrame;
    be<u16> mLifeTime;
    u8 _62[6];
    be<f32> mInitialVelOmni, mInitialVelAxis;
};
WWHD_OFFSET(ShmrgrdEmitterView, mRate, 0x34);
WWHD_OFFSET(ShmrgrdEmitterView, mMaxFrame, 0x5C);
WWHD_OFFSET(ShmrgrdEmitterView, mInitialVelAxis, 0x6C);
void shmrgrdRegisterDestructor(ShmrgrdRegistration *);
void shmrgrdConstructSignedChar(be<u8> *);
void shmrgrdConstructUnsignedChar(be<u8> *);

struct ShmrgrdSrcCyl {
    u8 mObj[0x30];
    struct {
        cXyz mCenter;
        be<f32> mRadius, mHeight;
    } mCyl;
};
WWHD_SIZE(ShmrgrdSrcCyl, 0x44);
struct ShmrgrdJointView {
    u8 _00[4];
    be<u16> mJntNo;
};
WWHD_OFFSET(ShmrgrdJointView, mJntNo, 4);

struct ShmrgrdPlayerView {
    u8 _00[0x3AC];
    be<u8> mCutType;
};
WWHD_OFFSET(ShmrgrdPlayerView, mCutType, 0x3AC);

// HD: private constants retain the retail names of the missing cut and sound enums.
namespace ShmrgrdCut {
enum {
    CUT_TYPE_HAMMER_SIDESWING = 0x11,
    CUT_TYPE_HAMMER_FRONTSWING = 0x12,
    CUT_TYPE_JUMPCUT_HAMMER = 0x13
};
}
enum {
    JA_SE_OBJ_HAMMER_SW_ON = 0x69B2,
    JA_SE_OBJ_HAMMER_SW_SWING = 0x69B8,
    ShmrgrdInitialized = 8
};
WWHD_OFFSET(ShmrgrdShapeView, mAabVtable, 0x18);
WWHD_OFFSET(ShmrgrdShapeView, mShapeVtable, 0x1C);
WWHD_OFFSET(ShmrgrdSrcCyl, mCyl, 0x30);
WWHD_OFFSET(ShmrgrdEmitterView, mLocalTranslation, 0x14);
WWHD_OFFSET(ShmrgrdEmitterView, mLifeTime, 0x60);
WWHD_OFFSET(ShmrgrdEmitterView, mInitialVelOmni, 0x68);
WWHD_SIZE(ShmrgrdRegistration, 12);
