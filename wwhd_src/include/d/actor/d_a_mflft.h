/* Dragon Roost flame lift. HD: actor fields move by 0x11C, and the line material
 * grows from 0x3C to 0x188. Numeric members retain GameCube names. */
#pragma once
#include "bindings.h"
#include "gc_api/m_Do/m_Do_ext.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/d/d_kankyo_wether.h"
#include "gc_api/f_op/f_op_actor_mng.h"
#include "d/d_cc_uty.h"
#define SAFESTRING_VTBL 0x100149EC
#define MFLFT_VTBL 0x10014BE8 /* HD: mflft_class vtable (dtor +0xC, setLiftUp +0x14) */
#define AAB_VTBL 0x10014A04   /* this TU's cM3dGAab vtable */

enum {
    dRes_INDEX_MFLFT_BDL_MFLFT_e = 4,
    dRes_INDEX_MFLFT_DZB_MFLFT_e = 7,
    dRes_INDEX_ALWAYS_BTI_ROPE_e = 0x7E,
};

/* mDoExt_3DlineMat1_c, HD 0x188: vtable at +0x130, the line array at +0x184 (0x10 per line:
 * +0 positions, +4 sizes) */
// HD: this material uses 0x10-byte lines rather than the shared 0x18-byte form.
struct MflftLine {
    gptr<cXyz> mpPositions;
    gptr<u8> mpSizes;
    u8 _08[8];
};
WWHD_SIZE(MflftLine, 0x10);
struct mDoExt_3DlineMat1_mflft_view {
    /* 0x000 */ u8 _000[0x130];
    /* 0x130 */ be<u32> __vtbl;
    /* 0x134 */ u8 _134[0x184 - 0x134];
    /* 0x184 */ gptr<MflftLine> mpLines;
    cXyz *getPos(s32 i) { return mpLines[i].mpPositions; }
    u8 *getSize(s32 i) { return mpLines[i].mpSizes; }
};
WWHD_SIZE(mDoExt_3DlineMat1_mflft_view, 0x188);

/* GameCube +0x11C up to mLineMat (HD 0x188 instead of 0x3C); size 0x9E4 */
struct mflft_class : fopAc_ac_c {
    /* 0x3AC */ request_of_phase_process_class mPhase;
    /* 0x3B4 */ be<s16> m298;
    /* 0x3B6 */ be<s16> m29A;
    /* 0x3B8 */ gptr<J3DModel> mpModel;
    /* 0x3BC */ be<u8> m2A0;
    /* 0x3BD */ be<u8> m2A1;
    /* 0x3BE */ u8 m2A2[2];
    /* 0x3C0 */ cXyz m2A4;
    /* 0x3CC */ be<f32> m2B0;
    /* 0x3D0 */ u8 m2B4[4];
    /* 0x3D4 */ be<f32> m2B8;
    /* 0x3D8 */ be<f32> m2BC;
    /* 0x3DC */ u8 m2C0[4];
    /* 0x3E0 */ be<f32> m2C4;
    /* 0x3E4 */ csXyz m2C8;
    /* 0x3EA */ be<s16> m2CE;
    /* 0x3EC */ be<f32> m2D0;
    /* 0x3F0 */ be<s8> m2D4[3];
    /* 0x3F3 */ u8 m2D7;
    /* 0x3F4 */ cXyz m2D8[3];
    /* 0x418 */ cXyz m2FC[3];
    /* 0x43C */ dCcD_Stts mStts;
    /* 0x478 */ dCcD_Cyl mCyls[3];
    /* 0x808 */ be<s16> m6EC[3];
    /* 0x80E */ be<s8> m6F2[3];
    /* 0x811 */ be<s8> m6F5;
    /* 0x812 */ be<s16> m6F6;
    /* 0x814 */ be<s16> m6F8;
    /* 0x816 */ be<s16> m6FA;
    /* 0x818 */ u8 m6FC[4];
    /* 0x81C */ Mtx34 m700;
    /* 0x84C */ gptr<dBgW> pm_bgw;
    /* 0x850 */ be<f32> m734;
    /* 0x854 */ mDoExt_3DlineMat1_mflft_view mLineMat;
    /* 0x9DC */ gptr<JPABaseEmitter> m774;
    /* 0x9E0 */ be<s8> m778;
    /* 0x9E1 */ u8 m779[3];

    void setLiftUp(cXyz *arg1);
};
WWHD_OFFSET(mflft_class, mStts, 0x43C);
WWHD_OFFSET(mflft_class, m6EC, 0x808);
WWHD_OFFSET(mflft_class, m700, 0x81C);
WWHD_OFFSET(mflft_class, mLineMat, 0x854);
WWHD_OFFSET(mflft_class, m778, 0x9E0);
WWHD_SIZE(mflft_class, 0x9E4);

void mflftConstructArray(void *, u32, u32, u32);
void mflftDestroyArray(void *, u32, u32, u32, u32);
be<f32> *mflftWindPower();
void mflftAngleAdd(const csXyz *, const csXyz *);
void mflftCullMin(fopAc_ac_c *, f32, f32, f32);
void mflftCullMax(fopAc_ac_c *, f32, f32, f32);
void mflftAttackPower(CcAtInfo *);
void mflftLineUpdate(mDoExt_3DlineMat1_mflft_view *, s32, const GXColor *, dKy_tevstr_c *);
BOOL mflftLineInit(mDoExt_3DlineMat1_mflft_view *, s32, s32, void *, s32);
void mflftGroundBaseDestroy(void *, s32);
void mflftShock(dVibration_c *, s32, s32, cXyz *);
void mflftLineConstruct(mDoExt_3DlineMat1_mflft_view *);
void mflftLineDestroy(mDoExt_3DlineMat1_mflft_view *, s32);
void mflftActorDestroy(fopAc_ac_c *, s32);

// HD: emitter channels used by the flame lift, absent from the shared prefix.
struct MflftEmitterView {
    u8 _00[0x34];
    be<f32> mRate;
    u8 _38[0x20];
    be<f32> mSpread;
    be<s32> mMaxFrame;
    u8 _60[0x1C];
    be<f32> mVolumeSweep;
    u8 _80[0x170];
    be<f32> mGlobalRotation[9];
    u8 _214[0xC];
    cXyz mGlobalScale, mGlobalTranslation, mGlobalParticleScale;
    u8 _244[0x10];
    be<u32> mStatus;
};
WWHD_OFFSET(MflftEmitterView, mRate, 0x34);
WWHD_OFFSET(MflftEmitterView, mMaxFrame, 0x5C);
WWHD_OFFSET(MflftEmitterView, mVolumeSweep, 0x7C);
WWHD_OFFSET(MflftEmitterView, mGlobalRotation, 0x1F0);
WWHD_OFFSET(MflftEmitterView, mGlobalScale, 0x220);
WWHD_OFFSET(MflftEmitterView, mGlobalTranslation, 0x22C);
WWHD_OFFSET(MflftEmitterView, mGlobalParticleScale, 0x238);
WWHD_OFFSET(MflftEmitterView, mStatus, 0x254);
struct MflftGroundView {
    u8 _00[0x20];
    be<u32> mPolyInfoVtable;
    cXyz mPosition;
    u8 _30[0x10];
    be<u32> mPolyPassVtable;
    u8 _44[8];
    be<u32> mGroupVtable, mGrp;
};
WWHD_SIZE(MflftGroundView, 0x54);
struct MflftBgView {
    u8 _00[0xA8];
    be<u32> mCorrectCallback;
    u8 _ac[4];
    gfn mRideCallback;
};
WWHD_OFFSET(MflftBgView, mCorrectCallback, 0xA8);
WWHD_OFFSET(MflftBgView, mRideCallback, 0xB0);
enum {
    MflftGroundPolyVtable = 0x10014A34,
    MflftGroundPassVtable = 0x10014A54,
    MflftGroundGroupVtable = 0x10014A14,
    MflftTypicalRotY = 0x024EE658,
    MflftCylinderDestructor = 0x02515A70,
};
void mflftSetRotationTranslation(Mtx34 *, be<f32> *, cXyz *);

struct MflftRegistration {
    be<u32> words[3];
};
WWHD_SIZE(MflftRegistration, 12);
void mflftRegisterDestructor(MflftRegistration *);
void mflftConstructSignedChar(be<u8> *);
void mflftConstructUnsignedChar(be<u8> *);

// HD: the ground-check constructor and deleting-destructor vtables of this TU.
static const dBgS_GndChk_vt GNDCHK_VT = {0x10014A24, 0x10014A34, 0x10014A54, 0x10014A44};
static const dBgS_GndChk_vt YOGAN_VT = {0x10014AA4, 0x10014AB4, 0x10014AD4, 0x10014AC4};

WWHD_OFFSET(mflft_class, mPhase, 0x3AC);
WWHD_OFFSET(mflft_class, m298, 0x3B4);
WWHD_OFFSET(mflft_class, m29A, 0x3B6);
WWHD_OFFSET(mflft_class, mpModel, 0x3B8);
WWHD_OFFSET(mflft_class, m2A0, 0x3BC);
WWHD_OFFSET(mflft_class, m2A1, 0x3BD);
WWHD_OFFSET(mflft_class, m2A2, 0x3BE);
WWHD_OFFSET(mflft_class, m2A4, 0x3C0);
WWHD_OFFSET(mflft_class, m2B0, 0x3CC);
WWHD_OFFSET(mflft_class, m2B4, 0x3D0);
WWHD_OFFSET(mflft_class, m2B8, 0x3D4);
WWHD_OFFSET(mflft_class, m2BC, 0x3D8);
WWHD_OFFSET(mflft_class, m2C0, 0x3DC);
WWHD_OFFSET(mflft_class, m2C4, 0x3E0);
WWHD_OFFSET(mflft_class, m2C8, 0x3E4);
WWHD_OFFSET(mflft_class, m2CE, 0x3EA);
WWHD_OFFSET(mflft_class, m2D0, 0x3EC);
WWHD_OFFSET(mflft_class, m2D4, 0x3F0);
WWHD_OFFSET(mflft_class, m2D7, 0x3F3);
WWHD_OFFSET(mflft_class, m2D8, 0x3F4);
WWHD_OFFSET(mflft_class, m2FC, 0x418);
WWHD_OFFSET(mflft_class, mCyls, 0x478);
WWHD_OFFSET(mflft_class, m6F2, 0x80E);
WWHD_OFFSET(mflft_class, m6F5, 0x811);
WWHD_OFFSET(mflft_class, m6F6, 0x812);
WWHD_OFFSET(mflft_class, m6F8, 0x814);
WWHD_OFFSET(mflft_class, m6FA, 0x816);
WWHD_OFFSET(mflft_class, m6FC, 0x818);
WWHD_OFFSET(mflft_class, pm_bgw, 0x84C);
WWHD_OFFSET(mflft_class, m734, 0x850);
WWHD_OFFSET(mflft_class, m774, 0x9DC);
WWHD_OFFSET(mflft_class, m779, 0x9E1);
namespace dPa_name {
enum {
    ID_AK_SN_MAGMALIFTSPLASH00 = 0x8231,
    ID_AK_SN_MAGMAISLAND01 = 0x8105,
    ID_AK_JN_OK = 0xD,
    ID_AK_JN_ELEMENTKIKUZU00 = 0x2B,
};
}
enum {
    JA_SE_OBJ_BAL_LIFT_LANDING = 0x69EB,
    JA_SE_LK_CUT_SBRIDGE_ROPE = 0x2838,
    JA_SE_LK_HIT_SBRIDGE_ROPE = 0x2837
};
WWHD_OFFSET(mDoExt_3DlineMat1_mflft_view, __vtbl, 0x130);
WWHD_OFFSET(mDoExt_3DlineMat1_mflft_view, mpLines, 0x184);
WWHD_OFFSET(MflftGroundView, mPosition, 0x24);
WWHD_OFFSET(MflftGroundView, mPolyInfoVtable, 0x20);
WWHD_OFFSET(MflftGroundView, mPolyPassVtable, 0x40);
WWHD_OFFSET(MflftGroundView, mGroupVtable, 0x4C);
WWHD_OFFSET(MflftGroundView, mGrp, 0x50);
WWHD_SIZE(MflftEmitterView, 0x258);

// HD: adjacent angle and position slots are observed together by the particle call.
struct MflftParticleFrame {
        csXyz sp18;
        u8 pad[2];
        cXyz sp48;
    };
WWHD_OFFSET(MflftParticleFrame,sp18,0);
WWHD_OFFSET(MflftParticleFrame,sp48,8);
WWHD_SIZE(MflftParticleFrame,20);
