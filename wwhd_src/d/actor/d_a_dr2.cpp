/**
 * d_a_dr2.cpp (WWHD)
 * NPC - Gohma fight - Valoo (body & tail) + lava pit & ceiling rock
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_dr2.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define SAFESTRING_VTBL 0x1000E0D0
#define DR2_VTBL 0x1000E0F8     /* HD: dr2_class vtable */
#define DR2_HIO_VTBL 0x1000E0E8 /* daDr2_HIO_c vtable */
#define FILE_NAME STR(0x1000E1E0) /* "d_a_dr2.cpp" */
#define ASSERT_FILE STR(0x1000E118)

enum {
    dRes_INDEX_DR2_BCK_DR_BOSS_DEMO1_e = 9,
    dRes_INDEX_DR2_BDL_IWA00_e = 0xE,
    dRes_INDEX_DR2_BMD_DR_e = 0x17,
    dRes_INDEX_DR2_BMD_DR_SIPPO_e = 0x18,
    dRes_INDEX_DR2_BMD_GAN_MAGMA_e = 0x1B,
    dRes_INDEX_DR2_BMD_MBYO1_e = 0x1C,
    dRes_INDEX_DR2_BMD_MBYO2_e = 0x1D,
    dRes_INDEX_DR2_BRK_MBYO2_e = 0x20,
    dRes_INDEX_DR2_BTK_GAN_MAGMA_e = 0x23,
    dRes_INDEX_DR2_BTK_MBYO1_e = 0x24,
    dRes_INDEX_DR2_BTK_MBYO2_e = 0x25,
    dRes_INDEX_DR2_DZB_MBYO1_e = 0x28,
    dRes_INDEX_DR2_DZB_MBYO2_e = 0x29,
};
enum { DR_JNT_J_DR_HANE_L2_e = 11 };
enum {
    JA_SE_CM_BTD_STN_FALL = 0x5828,
    JA_SE_CM_BTD_ROCK_FALL = 0x582A,
    JA_SE_CM_BTD_ROCK_HIT = 0x582B,
    JA_SE_CM_BTD_ROCK_ATTACH = 0x582F,
};
enum {
    dPa_name_ID_AK_SN_BTDDRIPMAGMA00 = 0x80B7,
    dPa_name_ID_AK_SN_BTDBROKENROCK00 = 0x80B9,
    dPa_name_ID_AK_SN_O_BTDBROKENROCKTAIL00 = 0x8062,
    dPa_name_ID_AK_ST_BTDSMOKE02 = 0xA0B8,
};
enum { fpcNm_BTD_e = 0xEA, fpcNm_KUI_e = 0xFA };

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* HD J3D: j3dSys.mModel at 0x104B462C, J3DSys::mCurrentMtx at 0x104B4868; a model's joint
 * matrices live in a block at +0x2C (+0x4 flags, +0x10 matrices), user area at +0xB8 */
static inline Mtx34* J3DModel_getAnmMtx(J3DModel* m, s32 jntNo) {
    u32 blk = gabi::load<u32>(gabi::ea(m) + 0x2C);
    gabi::store<u16>(blk + 4, (u16)(gabi::load<u16>(blk + 4) | 0x10)); /* HD: marks the joint matrices dirty */
    return gabi::at<Mtx34>(gabi::load<u32>(blk + 0x10) + jntNo * 0x30);
}
static inline void J3DModel_setAnmMtx(J3DModel* m, s32 jntNo, Mtx34* src) { mtx_copy(J3DModel_getAnmMtx(m, jntNo), src); }
static inline Mtx34* J3DSys_mCurrentMtx() { return gabi::at<Mtx34>(0x104B4868); }
/* 027F3F94 (the matcher names it __nw): returns the model data's joint tree; joint count (u16) at +8 */
static inline u16 J3DModelData_getJointNum(J3DModelData* d) { return gabi::load<u16>(gabi::ea(gabi::call<void*>(0x027F3F94, d)) + 8); }
/* J3DModelData::getJointNodePointer(i)->setCallBack(cb) (HD: nodes of 0x1C bytes from +8; index checked against +4) */
static inline void setJointCallBack(J3DModelData* d, u16 i, u32 cb) {
    u32 data = gabi::ea(d);
    u32 n = gabi::load<u32>(data + 4);
    u32 p = gabi::load<u32>(data + 8);
    if (i < n) p += i * 0x1C;
    gabi::store<u32>(p + 8, cb);
}
static inline void MtxRotY(f32 rad, u8 concat) { gabi::call(0x0200FBA4, rad, concat); }
static inline void mDoExt_modelUpdate(J3DModel* m) { gabi::call(0x025E2DA8, m); }
static inline s16 cM_rad2s(f32 x) { return gabi::call<s16>(0x02019510, x); }
static inline f32 cM_fsin(f32 x) { return cM_ssin(cM_rad2s(x)); }
static inline void PSMTXInverse(const Mtx34* a, Mtx34* b) { gabi::call(0x028E91EC, a, b); }
static inline void PSMTXScale(Mtx34* m, f32 x, f32 y, f32 z) { gabi::call(0x028E945C, m, x, y, z); }
static inline void PSMTXConcat(const Mtx34* a, const Mtx34* b, Mtx34* ab) { gabi::call(0x028E9108, a, b, ab); }
static inline s32 JUTNameTab_getIndex(u32 tab, const char* name) { return gabi::call<s32>(0x027DF9B0, tab, name); }
static inline void mDoExt_brkAnm_entry_l(void* a, J3DModelData* d, f32 frame) { gabi::call(0x025E83FC, a, d, frame); }
static inline void mDoExt_btkAnm_entry_l(void* a, J3DModelData* d, f32 frame) { gabi::call(0x025E7FC4, a, d, frame); }
static inline f32 anm_getFrame(void* a) { return gabi::load<f32>(gabi::ea(a) + 4); }
/* JPABaseEmitter (HD inlines) */
static inline void JPABaseEmitter_becomeInvalidEmitter(JPABaseEmitter* em) {
    u32 e = gabi::ea(em);
    u32 f = gabi::load<u32>(e + 0x254);
    gabi::store<s32>(e + 0x5C, -1);
    gabi::store<u32>(e + 0x254, f | 1);
}
static inline bool JPABaseEmitter_isEnableDeleteEmitter(JPABaseEmitter* em) {
    u32 e = gabi::ea(em);
    return (gabi::load<u32>(e + 0x254) & 8) && gabi::load<u32>(e + 0x1B4) + gabi::load<u32>(e + 0x1C0) == 0;
}
static inline void JPABaseEmitter_setRate(JPABaseEmitter* em, f32 r) { gabi::store<f32>(gabi::ea(em) + 0x34, r); }
/* dComIfGp_particle_setToon: dPa_control_c::set with group 2 (setup = room number) */
static inline JPABaseEmitter* particle_setToon(u16 id, const cXyz* pos, const csXyz* angle, const cXyz* scale, u8 alpha,
                                               dPa_levelEcallBack* cb, s8 setup) {
    dPa_control_c* pa = dComIfGp_getParticle();
    return dPa_control_set(pa, 2, id, pos, angle, scale, alpha, cb, setup, nullptr, nullptr, nullptr);
}
/* HD inline fopAcM_seStartCurrent: tests the actor (optionally) and &current.pos */
static inline void seStartCurrent(fopAc_ac_c* a, u32 id, bool check_actor) {
    if (check_actor && a == nullptr) return;
    if (gabi::ea(&a->current.pos) != 0)
        mDoAud_seStart(id, &a->current.pos, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(a)));
}
/* dVibration_c::StartShock(int, int, cXyz) (the cXyz by pointer to a copy); REG read after the accessor */
static inline void StartShock_reg(s32 add, s32 flags, cXyz* pos) {
    dVibration_c* v = dComIfGp_getVibration();
    s32 strength = REG0_S(2) + add;
    gabi::call(0x025CB374, v, strength, flags, pos);
}
static inline void mDoExt_baseAnm_play_l(void* a) { gabi::call(0x025E742C, a); }
/* HD inline anm setFrame: frame (+4), the animation's frame (through the pointer at +ptrOff) and a
 * functor at +fnOff ({f32 result, f32 a, f32 b, ?, fn, arg}) re-evaluated, then 027DF40C(&functor) */
static inline void anm_setFrame_hd(void* anm, f32 frame, u32 ptrOff, u32 fnOff) {
    u32 a = gabi::ea(anm);
    u32 fp = gabi::load<u32>(a + ptrOff);
    gabi::store<f32>(a + 4, frame);
    gabi::store<f32>(fp, frame);
    u32 fn = gabi::load<u32>(a + fnOff);
    u32 target = gabi::load<u32>(fn + 0x10);
    f32 p3 = gabi::load<f32>(fn + 8);
    f32 p2 = gabi::load<f32>(fn + 4);
    u32 arg = gabi::load<u32>(fn + 0x14);
    f32 res = gabi::call_ptr<f32>(target, arg, frame, p2, p3);
    gabi::store<f32>(fn, res);
    gabi::call(0x027DF40C, a + fnOff);
}
/* btkAnm setFrame from an s16 (HD: clamped to the end frame) */
static inline void btk_setFrame_clamped(void* btk, f32 frame) {
    if (frame > (f32)gabi::load<s16>(gabi::ea(btk) + 0xA))
        frame = (f32)gabi::load<s16>(gabi::ea(btk) + 0xA);
    anm_setFrame_hd(btk, frame, 0x68, 0x10);
}
/* JPABaseParticle global position (HD: read directly) */
static inline u32 JPABaseEmitter_getParticleListFirst(JPABaseEmitter* e) { return gabi::load<u32>(gabi::ea(e) + 0x1AC); }

/* daDr2_HIO_c (HD: vtable after the members) */
struct daDr2_HIO_c {
    /* 0x00 */ be<s8> mNo;
    /* 0x01 */ u8 _01[3];
    /* 0x04 */ be<f32> m08;
    /* 0x08 */ be<u32> __vtbl;
};
WWHD_SIZE(daDr2_HIO_c, 0xC);
static daDr2_HIO_c& l_HIO() { return *gabi::at<daDr2_HIO_c>(0x10463C50); }

static f32 hsx(int i) { return gabi::load<f32>(0x101B4778 + 4 * i); }
static f32 hsz(int i) { return gabi::load<f32>(0x101B4790 + 4 * i); }

struct iwa_hahen_s {
    /* 0x00 */ gptr<J3DModel> mpModel;
    /* 0x04 */ be<u8> unk_04;
    /* 0x05 */ u8 _05[3];
    /* 0x08 */ cXyz unk_08;
    /* 0x14 */ cXyz unk_14;
    /* 0x20 */ csXyz unk_20;
    /* 0x26 */ u8 _26[2];
};
WWHD_SIZE(iwa_hahen_s, 0x28);

/* GameCube +0x11C throughout */
struct dr2_class : fopEn_enemy_c {
    /* 0x3C8 */ request_of_phase_process_class mPhase;
    /* 0x3D0 */ gptr<mDoExt_McaMorf> mpMorf1;
    /* 0x3D4 */ gptr<mDoExt_McaMorf> mpMorf2;
    /* 0x3D8 */ be<s16> unk_2BC;
    /* 0x3DA */ u8 _3DA[2];
    /* 0x3DC */ iwa_hahen_s mRockFragments[6];
    /* 0x4CC */ cXyz unk_3B0;
    /* 0x4D8 */ be<s16> unk_3BC[12];
    /* 0x4F0 */ be<s16> unk_3D4[12];
    /* 0x508 */ be<f32> unk_3EC;
    /* 0x50C */ be<f32> unk_3F0;
    /* 0x510 */ be<f32> unk_3F4;
    /* 0x514 */ be<f32> unk_3F8;
    /* 0x518 */ be<u32> unk_3FC;
    /* 0x51C */ be<s16> unk_400[5];
    /* 0x526 */ be<u8> unk_40A;
    /* 0x527 */ u8 _527;
    /* 0x528 */ be<f32> unk_40C;
    /* 0x52C */ be<f32> unk_410;
    /* 0x530 */ be<f32> unk_414;
    /* 0x534 */ gptr<J3DModel> unk_418;
    /* 0x538 */ gptr<J3DModel> unk_41C;
    /* 0x53C */ gptr<void> unk_420; /* mDoExt_btkAnm* */
    /* 0x540 */ be<s16> unk_424;
    /* 0x542 */ u8 _542[2];
    /* 0x544 */ gptr<J3DModel> unk_428;
    /* 0x548 */ gptr<J3DModel> unk_42C;
    /* 0x54C */ gptr<void> unk_430; /* mDoExt_btkAnm* */
    /* 0x550 */ gptr<void> unk_434; /* mDoExt_btkAnm* */
    /* 0x554 */ gptr<void> unk_438; /* mDoExt_brkAnm* */
    /* 0x558 */ be<u8> unk_43C;
    /* 0x559 */ u8 _559[3];
    /* 0x55C */ Mtx34 unk_440;
    /* 0x58C */ Mtx34 unk_470;
    /* 0x5BC */ gptr<dBgW> mpBgW1;
    /* 0x5C0 */ gptr<dBgW> mpBgW2;
    /* 0x5C4 */ cXyz unk_4A8;
    /* 0x5D0 */ csXyz unk_4B4;
    /* 0x5D6 */ be<s16> unk_4BA;
    /* 0x5D8 */ u8 _5D8[4];
    /* 0x5DC */ be<f32> unk_4C0;
    /* 0x5E0 */ be<u8> unk_4C4;
    /* 0x5E1 */ u8 _5E1;
    /* 0x5E2 */ be<s16> unk_4C6;
    /* 0x5E4 */ be<s16> unk_4C8;
    /* 0x5E6 */ be<s16> unk_4CA;
    /* 0x5E8 */ be<s16> unk_4CC;
    /* 0x5EA */ be<s16> unk_4CE;
    /* 0x5EC */ be<f32> unk_4D0;
    /* 0x5F0 */ gptr<JPABaseEmitter> unk_4D4;
    /* 0x5F4 */ dPa_followEcallBack unk_4D8;
    /* 0x608 */ u8 unk_4EC[0x20]; /* dPa_smokeEcallBack (HD 0x20, vtable at +0) */
    /* 0x628 */ be<u8> unk_50C;
    /* 0x629 */ be<u8> unk_50D;
    /* 0x62A */ be<u8> unk_50E;
    /* 0x62B */ u8 _62B;
    /* 0x62C */ be<s16> unk_510;
    /* 0x62E */ u8 _62E[2];
    /* 0x630 */ gptr<fopAc_ac_c> unk_514; /* btd_class* */
};
WWHD_OFFSET(dr2_class, mRockFragments, 0x3DC);
WWHD_OFFSET(dr2_class, unk_40A, 0x526);
WWHD_OFFSET(dr2_class, mpBgW1, 0x5BC);
WWHD_OFFSET(dr2_class, unk_4D8, 0x5F4);
WWHD_OFFSET(dr2_class, unk_50C, 0x628);
WWHD_SIZE(dr2_class, 0x634);

/* btd_class fields read here (HD: GameCube +0x128) */
static inline u32 btd_m02FC(fopAc_ac_c* btd, int i) { return gabi::ea(btd) + 0x424 + 12 * i; }
static inline f32 btd_m02FC_y(fopAc_ac_c* btd, int i) { return gabi::load<f32>(btd_m02FC(btd, i) + 4); }
static inline u32 btd_m02F6(fopAc_ac_c* btd) { return gabi::ea(btd) + 0x41E; }

/* 0212D548 daDr2_HIO_c::daDr2_HIO_c (HD: allocates when this == NULL) */
static daDr2_HIO_c* daDr2_HIO_c_ct(daDr2_HIO_c* i_this) {
    WWHD_FUNC(0x0212D548, daDr2_HIO_c*, i_this);
    if (i_this == nullptr) {
        i_this = (daDr2_HIO_c*)operator_new(0xC);
        if (i_this == nullptr)
            return i_this;
    }
    i_this->mNo = -1;
    i_this->__vtbl = DR2_HIO_VTBL;
    i_this->m08 = 1.0f;
    return i_this;
}
VERIFY(0x0212D548, daDr2_HIO_c_ct);

/* 0212AACC */
static BOOL nodeCallBack(J3DNode* node, int calcTiming) {
    WWHD_FUNC(0x0212AACC, BOOL, node, calcTiming);
    if (calcTiming == 0 /* J3DNodeCBCalcTiming_In */) {
        J3DJoint* joint = J3DNode_toJoint(node);
        J3DModel* model = gabi::at<J3DModel>(gabi::load<u32>(0x104B462C)); /* j3dSys.getModel() */
        dr2_class* i_this = gabi::at<dr2_class>(gabi::load<u32>(gabi::ea(model) + 0xB8));
        s32 jntNo = gabi::load<u16>(gabi::ea(joint) + 4);

        if (i_this != nullptr) {
            PSMTXCopy(J3DModel_getAnmMtx(model, jntNo), calc_mtx());

            if (i_this->unk_40A == 3) {
                /* HD: joints 2..11 (ASHI_L1..KOSHI) */
                if ((u32)(jntNo - 2) <= 9) {
                    cMtx_YrotM(calc_mtx(), i_this->unk_3BC[jntNo]);
                    cMtx_ZrotM(calc_mtx(), i_this->unk_3D4[jntNo]);
                    J3DModel_setAnmMtx(model, jntNo, calc_mtx());
                    PSMTXCopy(calc_mtx(), J3DSys_mCurrentMtx());
                }
            } else if (i_this->unk_40A == 1) {
                MtxScale(i_this->unk_40C, 1.0f, 1.0f, true);
                J3DModel_setAnmMtx(model, jntNo, calc_mtx());

                if ((u32)jntNo <= 4) {
                    MtxRotY(i_this->unk_3EC, 1);
                    MtxRotZ(i_this->unk_3F0, 1);
                    J3DModel_setAnmMtx(model, jntNo, calc_mtx());
                    PSMTXCopy(calc_mtx(), J3DSys_mCurrentMtx());
                }
            } else if ((u32)(jntNo - 4) <= 7) {
                if (i_this->unk_40A == 2) {
                    cMtx_YrotM(calc_mtx(), i_this->unk_3BC[0]);
                    cMtx_ZrotM(calc_mtx(), i_this->unk_3D4[0]);
                } else {
                    MtxRotY(i_this->unk_3EC, 1);
                    MtxRotZ(i_this->unk_3F0, 1);
                }
                J3DModel_setAnmMtx(model, jntNo, calc_mtx());
                PSMTXCopy(calc_mtx(), J3DSys_mCurrentMtx());
            }

            if (jntNo == DR_JNT_J_DR_HANE_L2_e) {
                gabi::Local<cXyz> sp08;
                sp08->x = REG0_F(0) + 215.0f; /* 210.0f + 5.0f */
                sp08->y = REG0_F(1);
                sp08->z = REG0_F(2) + -5.0f; /* -10.0f + 5.0f */
                MtxPosition(sp08.get(), &i_this->unk_3B0);
            }
        }
    }
    return TRUE;
}
VERIFY(0x0212AACC, nodeCallBack);

/* hahen_draw (inlined) */
static inline void hahen_draw(dr2_class* i_this) {
    iwa_hahen_s* fragment = i_this->mRockFragments;
    for (s32 i = 0; i < 6; i++, fragment++) {
        if (fragment->unk_04) {
            dScnKy_env_light_c* env = dKy_getEnvlight();
            setLightTevColorType(env, fragment->mpModel, &i_this->tevStr);
            mDoExt_modelUpdate(fragment->mpModel);
        }
    }
}

/* iwa_draw (inlined) */
static inline void iwa_draw(dr2_class* i_this) {
    if (i_this->unk_4BA < 10) {
        dScnKy_env_light_c* env = dKy_getEnvlight();
        setLightTevColorType(env, i_this->unk_418, &i_this->tevStr);
        mDoExt_modelUpdateDL(i_this->unk_418);
        if (i_this->unk_424 != 0) {
            J3DModelData* modelData = J3DModel_getModelData(i_this->unk_41C);
            void* btk = i_this->unk_420;
            mDoExt_btkAnm_entry_l(btk, modelData, anm_getFrame(btk));
            env = dKy_getEnvlight();
            setLightTevColorType(env, i_this->unk_41C, &i_this->tevStr);
            mDoExt_modelUpdateDL(i_this->unk_41C);
        }
    }
    hahen_draw(i_this);
}

/* HD only: the lava floor's texture matrix follows the camera (material "...", 0x1000E108) */
static inline void yuka_texmtx_hd(dr2_class* i_this) {
    u32 tabp = gabi::load<u32>(gabi::ea(i_this->unk_42C.get()) + 0x14);
    s32 off = gabi::load<s32>(tabp + 0x18);
    u32 tab = 0;
    if (off != 0) tab = tabp + 0x18 + off;
    s32 idx = JUTNameTab_getIndex(tab, STR(0x1000E108));
    u32 m = gabi::ea(i_this->unk_42C.get());
    u32 md = gabi::load<u32>(m + 0xAC);
    u32 shapePkt = gabi::load<u32>(m + 0x34) + idx * 0x3C;
    u32 n = gabi::load<u32>(md + 0xC);
    u32 mat = gabi::load<u32>(md + 0x10);
    if ((u32)(u16)idx < n) mat += (u16)idx * 0x39C;
    u32 obj = gabi::load<u32>(mat + 0x14);
    u32 texMtx = gabi::call_ptr<u32>(gabi::load<u32>(gabi::load<u32>(obj) + 0x14), obj, 2);

    gabi::Local<Mtx34> inv;
    PSMTXInverse(gabi::at<Mtx34>(0x104B45F8), inv.get());
    PSMTXScale(mDoMtx_stack_c::get(), 0.2f, 0.2f, 1.0f);
    mDoMtx_XrotM(mDoMtx_stack_c::get(), 0x4000);
    PSMTXConcat(mDoMtx_stack_c::get(), inv.get(), mDoMtx_stack_c::get());
    u32 s = gabi::ea(mDoMtx_stack_c::get());
    /* adjacent lfs/stfs pairs: the recompiled code copies the bits (no NaN quieting) */
    for (int k = 0; k < 11; k++) gabi::store<u32>(texMtx + 0x24 + 4 * k, gabi::load<u32>(s + 4 * k));
    f32 m23 = gabi::load<f32>(s + 0x2C);
    gabi::store<f32>(texMtx + 0x5C, 0.0f);
    gabi::store<f32>(texMtx + 0x60, 1.0f);
    gabi::store<f32>(texMtx + 0x58, 0.0f);
    gabi::store<f32>(texMtx + 0x54, 0.0f);
    gabi::store<f32>(texMtx + 0x50, m23);
    gabi::Local<Mtx34> tmp;
    mtx_copy(tmp.get(), mDoMtx_stack_c::get());
    PSMTXCopy(tmp.get(), gabi::at<Mtx34>(texMtx + 0x64));
    u32 p = gabi::call<u32>(0x027FA678, shapePkt, 2);
    if (p != 0) gabi::store<u32>(p, texMtx + 0x64);
}

/* yuka_draw (inlined) */
static inline void yuka_draw(dr2_class* i_this) {
    J3DModelData* modelData;

    if (!i_this->unk_43C) {
        dScnKy_env_light_c* env = dKy_getEnvlight();
        setLightTevColorType(env, i_this->unk_428, &i_this->tevStr);
        modelData = J3DModel_getModelData(i_this->unk_428);
        void* btk = i_this->unk_430;
        mDoExt_btkAnm_entry_l(btk, modelData, anm_getFrame(btk));
        mDoExt_modelUpdateDL(i_this->unk_428);
    } else {
        dScnKy_env_light_c* env = dKy_getEnvlight();
        setLightTevColorType(env, i_this->unk_42C, &i_this->tevStr);
        yuka_texmtx_hd(i_this);
        modelData = J3DModel_getModelData(i_this->unk_42C);
        void* btk = i_this->unk_434;
        mDoExt_btkAnm_entry_l(btk, modelData, anm_getFrame(btk));
        modelData = J3DModel_getModelData(i_this->unk_42C);
        void* brk = i_this->unk_438;
        mDoExt_brkAnm_entry_l(brk, modelData, anm_getFrame(brk));
        mDoExt_modelUpdateDL(i_this->unk_42C);
    }
}

/* dr_draw (inlined) */
static inline void dr_draw(dr2_class* i_this) {
    if (i_this->unk_50C) {
        J3DModel* model = i_this->mpMorf2->getModel();
        mDoMtx_stack_c::transS(0.0f, REG0_F(5) + 19720.0f /* HD: 10000.0f + 9720.0f folded */, 0.0f);

        gabi::Local<cXyz> sp08;
        f32 s = REG0_F(6) + 1.0f;
        sp08->x = sp08->y = sp08->z = s;
        J3DModel_setBaseScale(model, sp08.get());
        J3DModel_setBaseTRMtx(model, mDoMtx_stack_c::get());

        settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &i_this->current.pos, &i_this->tevStr);
        setLightTevColorType(dKy_getEnvlight(), model, &i_this->tevStr);

        i_this->mpMorf2->play(nullptr, 0, 0);
        i_this->mpMorf2->calc();
        i_this->mpMorf2->entryDL();
    }
}

/* 0212AEEC */
static BOOL daDr2_Draw(dr2_class* i_this) {
    WWHD_FUNC(0x0212AEEC, BOOL, i_this);
    J3DModel* model = i_this->mpMorf1->getModel();
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &i_this->current.pos, &i_this->tevStr);
    setLightTevColorType(dKy_getEnvlight(), model, &i_this->tevStr);
    i_this->mpMorf1->entryDL();
    iwa_draw(i_this);
    yuka_draw(i_this);
    dr_draw(i_this);
    return TRUE;
}
VERIFY(0x0212AEEC, daDr2_Draw);

/* 0212B380 */
static void* s_a_d_sub(void* ac1, void* ac2) {
    WWHD_FUNC(0x0212B380, void*, ac1, ac2);
    /* HD: fopAcM_GetName checks the pointer */
    if (fopAc_IsActor(ac1) && ac1 != nullptr && fpcM_GetName(ac1) == fpcNm_BTD_e) {
        return ac1;
    }
    return nullptr;
}
VERIFY(0x0212B380, s_a_d_sub);

/* hahen_move (inlined) */
static inline void hahen_move(dr2_class* i_this) {
    iwa_hahen_s* fragment = &i_this->mRockFragments[0];

    for (s32 i = 0; i < 6; i++, fragment++) {
        if (fragment->unk_04) {
            PSVECAdd(&fragment->unk_08, &fragment->unk_14, &fragment->unk_08);
            if (i & 1) {
                fragment->unk_20.z = (s16)(fragment->unk_20.z + (REG0_S(4) + 300));
                fragment->unk_20.x = (s16)(fragment->unk_20.x + (REG0_S(3) + 256));
            } else {
                fragment->unk_20.z = (s16)(fragment->unk_20.z - (REG0_S(4) + 300));
                fragment->unk_20.x = (s16)(fragment->unk_20.x - (REG0_S(3) + 256));
            }
            fragment->unk_14.y = fragment->unk_14.y - (REG0_F(7) + 0.5f);
            MtxTrans(fragment->unk_08.x, fragment->unk_08.y, fragment->unk_08.z, false);
            cMtx_YrotM(calc_mtx(), fragment->unk_20.y);
            cMtx_XrotM(calc_mtx(), fragment->unk_20.x);
            cMtx_ZrotM(calc_mtx(), fragment->unk_20.z);

            J3DModel_setBaseTRMtx(fragment->mpModel, calc_mtx());

            if (fragment->unk_08.y < -100.0f) {
                fragment->unk_04 = false;
            }
        }
    }
}

/* iwa_move (inlined) */
static inline void iwa_move(dr2_class* i_this) {
    fopAc_ac_c* a_this = i_this;

    fopAc_ac_c* btd = i_this->unk_514;
    if (btd == nullptr) {
        btd = (fopAc_ac_c*)fpcM_Search(0x0212B380 /* s_a_d_sub */, i_this);
        i_this->unk_514 = btd;
    }

    if (i_this->unk_4CA != 0) {
        i_this->unk_4CA = (s16)(i_this->unk_4CA - 1);
    }

    switch (i_this->unk_4BA) {
    case -1:
        if (std::fabs((f32)(a_this->current.pos.y - a_this->home.pos.y)) > 200.0f) {
            i_this->unk_410 = 0.0f;
        }

        if (i_this->unk_4C8 == 0) {
            cLib_addCalc2(&a_this->current.pos.y, a_this->home.pos.y, 0.5f, 30.0f);

            if (std::fabs((f32)(a_this->current.pos.y - a_this->home.pos.y)) < 1.0f) {
                i_this->unk_4BA = 0;
            }
        } else {
            i_this->unk_4C8 = (s16)(i_this->unk_4C8 - 1);
        }
        // Fall-through
    case 0:
        cLib_addCalcAngleS2(&i_this->unk_4B4.x, 0, 1, 0x100);
        cLib_addCalcAngleS2(&i_this->unk_4B4.y, 0, 1, 0x100);

        if (i_this->unk_40A == 1 || i_this->unk_4CA != 0) {
            if (i_this->unk_4CC == 0) {
                i_this->unk_4CC = (s16)(REG0_S(5) + 3);
                i_this->unk_4CE = (s16)gabi::ftoi(cM_rndFX(REG0_F(7) + 300.0f));
                i_this->unk_4D0 = cM_rndFX(REG0_F(6) + 20.0f);
                if (!i_this->unk_50E) {
                    i_this->unk_510 = 20;
                    i_this->unk_50D = (u8)(REG0_S(5) + 1);
                    i_this->unk_50E = true;
                    seStartCurrent(a_this, JA_SE_CM_BTD_STN_FALL, true);
                }
            } else {
                i_this->unk_4CC = (s16)(i_this->unk_4CC - 1);
            }
        } else {
            i_this->unk_4D0 = 0.0f;
            i_this->unk_4CE = 0;
        }

        cLib_addCalcAngleS2(&i_this->unk_4B4.z, i_this->unk_4CE, 1, (s16)(REG0_S(6) + 0x100));
        cLib_addCalc2(&i_this->unk_4A8.y, (i_this->unk_4D0 + a_this->home.pos.y) - 50.0f, 0.5f, 50.0f);

        if (i_this->unk_4CA == 1) {
            i_this->unk_4BA = (s16)(i_this->unk_4BA + 1);
            s8 roomNo = fopAcM_GetRoomNo(a_this);
            i_this->unk_510 = 1;
            i_this->unk_50D = (u8)(REG0_S(2) + 30);
            i_this->unk_50E = true;
            particle_setToon(dPa_name_ID_AK_ST_BTDSMOKE02, &i_this->unk_4A8, &i_this->unk_4B4, nullptr, 0xB9,
                             (dPa_levelEcallBack*)i_this->unk_4EC, roomNo);
            seStartCurrent(a_this, JA_SE_CM_BTD_ROCK_FALL, false);
        }
        break;

    case 1: {
        if (btd == nullptr) /* JUT_ASSERT(0x280, btd != NULL) */
            JUT_ASSERT_fail(ASSERT_FILE, 0x280, STR(0x1000E124));
        f32 spy = a_this->speed.y;
        i_this->unk_4A8.y = i_this->unk_4A8.y + spy;
        a_this->speed.y = spy - (REG0_F(2) + 10.0f);

        f32 fVar11 = btd_m02FC_y(btd, 3) + REG0_F(3);
        if (i_this->unk_4A8.y < fVar11) {
            i_this->unk_4A8.y = fVar11;
            i_this->unk_4BA = 2;
            i_this->unk_400[0] = 0x32;
            gabi::store<u8>(btd_m02F6(btd), 1);
            seStartCurrent(a_this, JA_SE_CM_BTD_ROCK_HIT, false);
            gabi::Local<cXyz> up;
            up->x = 0.0f;
            up->y = 1.0f;
            up->z = 0.0f;
            StartShock_reg(5, -0x21, up.get());
        }
        break;
    }
    case 2: {
        if (btd == nullptr) /* JUT_ASSERT(0x2A5, btd != NULL) */
            JUT_ASSERT_fail(ASSERT_FILE, 0x2A5, STR(0x1000E124));
        i_this->unk_40A = 3;
        cLib_addCalc2(&i_this->unk_414, 2000.0f, 0.5f, 100.0f);

        i_this->unk_4A8.y = btd_m02FC_y(btd, 3) + REG0_F(3);
        i_this->unk_4B4.y = i_this->unk_514->current.angle.y;

        f32 lim = REG0_F(6) + 100.0f;
        if (btd_m02FC_y(btd, 3) < lim) {
            i_this->unk_4C0 = lim - btd_m02FC_y(btd, 3);
            i_this->unk_4C0 = i_this->unk_4C0 * (REG0_F(7) + 0.3f);
        }

        gabi::Local<cXyz> sp4C;
        cXyz_mi(gabi::at<cXyz>(btd_m02FC(btd, 3)), sp4C.get(), gabi::at<cXyz>(btd_m02FC(btd, 4)));
        f32 y = sp4C->y + ((REG0_F(5) + -330.0f) + i_this->unk_4C0);
        f32 x = sp4C->x, z = sp4C->z;
        f32 xz = std_sqrtf(gabi::fmadds(x, x, z * z));
        cLib_addCalcAngleS2(&i_this->unk_4B4.x, (s16)-cM_atan2s(y, xz), 1, 0x200);

        if (!gabi::load<u8>(btd_m02F6(btd))) {
            i_this->unk_4BA = 3;
            dComIfGp_particle_set(dPa_name_ID_AK_SN_BTDDRIPMAGMA00, &i_this->unk_4A8, &i_this->unk_4B4, nullptr, 0xFF,
                                  (dPa_levelEcallBack*)&i_this->unk_4D8);
            i_this->unk_424 = 1;
        }
        break;
    }
    case 3:
        if (btd == nullptr) /* JUT_ASSERT(0x2E8, btd != NULL) */
            JUT_ASSERT_fail(ASSERT_FILE, 0x2E8, STR(0x1000E124));
        i_this->unk_4A8.y = btd_m02FC_y(btd, 8) + 100.0f + REG0_F(9);
        cLib_addCalcAngleS2(&i_this->unk_4B4.x, 0, 1, 0x100);
        cLib_addCalcAngleS2(&i_this->unk_4B4.y, 0, 1, 0x100);

        if (i_this->unk_4A8.y > a_this->home.pos.y - 1200.0f) {
            a_this->current.pos.y = a_this->current.pos.y + (REG0_F(9) + 100.0f);
            i_this->unk_40A = 0;
        }

        if (i_this->unk_4A8.y > a_this->home.pos.y - 50.0f) {
            i_this->unk_4A8.y = a_this->home.pos.y - 50.0f;
            a_this->speed.y = 0.0f;
            i_this->unk_4BA = -1;
            i_this->unk_4C0 = 0.0f;
            i_this->unk_4C8 = 0x82;
            dPa_followEcallBack_end(&i_this->unk_4D8); /* HD: unk_4D8.remove() is end() */
            s8 roomNo = fopAcM_GetRoomNo(a_this);
            particle_setToon(dPa_name_ID_AK_ST_BTDSMOKE02, &i_this->unk_4A8, &i_this->unk_4B4, nullptr, 0xB9,
                             (dPa_levelEcallBack*)i_this->unk_4EC, roomNo);

            i_this->unk_510 = 2;
            i_this->unk_50D = (u8)(REG0_S(2) + 10);
            i_this->unk_50E = true;

            gabi::Local<cXyz> up;
            up->x = 0.0f;
            up->y = 1.0f;
            up->z = 0.0f;
            StartShock_reg(5, -0x21, up.get());
            seStartCurrent(a_this, JA_SE_CM_BTD_ROCK_ATTACH, false);
        }
        break;

    case 10: {
        i_this->unk_4BA = (s16)(i_this->unk_4BA + 1);

        iwa_hahen_s* fragment = &i_this->mRockFragments[0];
        for (s32 i = 0; i < 6; i++, fragment++) {
            if (!fragment->unk_04) {
                fragment->unk_04 = true;
                fragment->unk_08.copy(i_this->unk_4A8);
                fragment->unk_20.x = i_this->unk_4B4.x;
                fragment->unk_20.y = i_this->unk_4B4.y;
                fragment->unk_20.z = i_this->unk_4B4.z;
                cMtx_YrotS(calc_mtx(), fragment->unk_20.y);
                cMtx_XrotM(calc_mtx(), fragment->unk_20.x);
                gabi::Local<cXyz> sp58;
                f32 k = REG0_F(2) + 3.0f;
                sp58->x = hsx(i) * k;
                sp58->y = 0.0f;
                sp58->z = hsz(i) * k;
                MtxPosition(sp58.get(), &fragment->unk_14);
            }
        }
    }
        // Fall-through
    case 11:
        i_this->unk_40A = 0;
        break;
    }

    switch (i_this->unk_50E) {
    case 1:
        if (i_this->unk_4D4 != nullptr) {
            JPABaseEmitter_becomeInvalidEmitter(i_this->unk_4D4);
        }

        i_this->unk_4D4 = dComIfGp_particle_set(dPa_name_ID_AK_SN_BTDBROKENROCK00, &i_this->unk_4A8);
        if (i_this->unk_4D4 != nullptr) {
            JPABaseEmitter_setRate(i_this->unk_4D4, (f32)(u8)i_this->unk_50D);
            i_this->unk_50E = (u8)(i_this->unk_50E + 1);
        }
        break;

    case 2: {
        if (i_this->unk_510 != 0) {
            i_this->unk_510 = (s16)(i_this->unk_510 - 1);
            if (i_this->unk_510 == 0 && i_this->unk_4D4 != nullptr) {
                JPABaseEmitter_becomeInvalidEmitter(i_this->unk_4D4);
            }
        }

        JPABaseEmitter* em = i_this->unk_4D4;
        if (em != nullptr) {
            if (JPABaseEmitter_isEnableDeleteEmitter(em)) {
                i_this->unk_50E = false;
                i_this->unk_4D4 = nullptr;
                break;
            }
        }

        u32 link = JPABaseEmitter_getParticleListFirst(em);
        while (link != 0) {
            u32 ptcl = gabi::load<u32>(link);
            gabi::Local<cXyz> sp40;
            f32 py = gabi::load<f32>(ptcl + 0x2C);
            f32 pz = gabi::load<f32>(ptcl + 0x30);
            f32 px = gabi::load<f32>(ptcl + 0x28);
            sp40->z = pz;
            sp40->y = py;
            sp40->x = px;
            dComIfGp_particle_setSimple(dPa_name_ID_AK_SN_O_BTDBROKENROCKTAIL00, sp40.get(), 0xB9);
            link = gabi::load<u32>(link + 0xC);
        }
        break;
    }
    }

    if (i_this->unk_424 != 0) {
        btk_setFrame_clamped(i_this->unk_420, (f32)i_this->unk_424);
        s16 n = (s16)(i_this->unk_424 + 1);
        if (n > 180) {
            i_this->unk_424 = 0;
        } else {
            i_this->unk_424 = n;
        }
    }

    hahen_move(i_this);
}

/* move (inlined) */
static inline void move(dr2_class* i_this) {
    fopAc_ac_c* a_this = i_this;
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    fopAc_ac_c* foundActor = fopAcM_SearchByID(i_this->unk_3FC);

    if (i_this->unk_40A == 3) {
        for (s32 i = 2; i < 12; i++) {
            i_this->unk_3BC[i] = (s16)gabi::ftoi(cM_ssin(i_this->unk_2BC * 2800 + i * 3000) * i_this->unk_414);
            i_this->unk_3D4[i] = (s16)gabi::ftoi(cM_scos(i_this->unk_2BC * 2300 + i * 2000) * i_this->unk_414);
        }
    } else {
        i_this->unk_40A = 0;
        if (foundActor != nullptr && foundActor->health == 3) {
            /* daPy_py_c::getLeftHandPos() (HD: +0x3F0) - foundActor->current.pos */
            gabi::Local<cXyz> hand;
            cXyz* lh = gabi::at<cXyz>(gabi::ea(player) + 0x3F0);
            hand->x = lh->x;
            hand->y = lh->y;
            hand->z = lh->z;
            gabi::Local<cXyz> diff;
            cXyz_mi(hand.get(), diff.get(), &foundActor->current.pos);
            gabi::Local<cXyz> sp30;
            sp30->copy(*diff);
            gabi::Local<cXyz> sp24;

            cMtx_YrotS(calc_mtx(), (s16)-player->shape_angle.y);
            MtxPosition(sp30.get(), sp24.get());

            s32 atan = cM_atan2s(sp24->z, -sp24->y);
            i_this->current.angle.y = player->shape_angle.y;
            s16 atan2 = (s16)gabi::ftoi((f32)atan * (REG0_F(3) + -0.125f));

            cLib_addCalcAngleS2(&i_this->unk_3BC[0], atan2, 0x10, 0x40);
            cLib_addCalcAngleS2(&i_this->unk_3D4[0], 0, 0x10, 0x40);
            i_this->unk_40A = 2;
        } else if (foundActor != nullptr && foundActor->health == 2) {
            i_this->unk_40A = 1;
            s16 ang = fopAcM_searchPlayerAngleY(a_this);
            i_this->current.angle.y = ang;
            player->shape_angle.y = (s16)(ang - -0x8000);
            player->current.angle.y = (s16)(i_this->current.angle.y - -0x8000);

            cLib_addCalc2(&i_this->unk_3EC, REG0_F(9) + -0.175f, 0.5f, REG0_F(10) + 0.0125f);
            cLib_addCalc0(&i_this->unk_3F0, 0.5f, 0.10000000149011612f);

            if (i_this->unk_3EC < -0.1f) {
                cLib_addCalc2(&i_this->unk_40C, 1.04f, 0.1f, REG0_F(14) + 0.005f);
            }
        } else {
            f32 t = i_this->unk_3F4 + 0.04f;
            i_this->current.angle.y = i_this->home.angle.y;
            i_this->unk_40C = 0.98f;
            i_this->unk_3F4 = t;
            if (t > 6.2831855f) {
                i_this->unk_3EC = i_this->unk_3EC - 6.2831855f;
            }

            t = i_this->unk_3F8 + 0.05f;
            i_this->unk_3F8 = t;
            if (t > 6.2831855f) {
                i_this->unk_3F0 = i_this->unk_3F0 - 6.2831855f;
            }

            i_this->unk_3EC = cM_fsin(i_this->unk_3F4) * i_this->unk_410;
            i_this->unk_3F0 = cM_fsin(i_this->unk_3F8) * i_this->unk_410;

            if (foundActor != nullptr && foundActor->health == 1) {
                cLib_addCalc2(&i_this->unk_410, 0.0f, 0.5f, 0.001f);
                i_this->unk_4CC = 20;
            } else {
                cLib_addCalc2(&i_this->unk_410, 0.048f, 0.5f, 0.001f);
            }
        }
    }

    i_this->mpMorf1->play(&a_this->eyePos, 0, 0);
    i_this->mpMorf1->calc();
}

/* dr_move (inlined) */
static inline void dr_move(dr2_class* i_this) {
    if (i_this->unk_4C6 != 0) {
        i_this->unk_4C6 = (s16)(i_this->unk_4C6 - 1);
    }

    switch (i_this->unk_4C4) {
    case 0:
        if (i_this->unk_50C) {
            i_this->unk_4C6 = 0;
            J3DAnmTransform* pAnimRes =
                (J3DAnmTransform*)dComIfG_getObjectRes(STR(0x1000E0CC) /* "Dr2" */, dRes_INDEX_DR2_BCK_DR_BOSS_DEMO1_e, SAFESTRING_VTBL);
            i_this->mpMorf2->setAnm(pAnimRes, 0, 1.0f, 1.0f, 0.0f, -1.0f, nullptr);
            i_this->unk_4C4 = 1;
        }
        break;

    case 1:
        if (!i_this->unk_50C) {
            i_this->unk_4C4 = false;
        }
        break;
    }
}

/* yuka_move (inlined) */
static inline void yuka_move(dr2_class* i_this) {
    /* dComIfGs_isStageBossEnemy(): HD dSv_memBit_c::isDungeonItem(3) on the save info (+0x798) */
    if (gabi::call<BOOL>(0x025B9100, gabi::load<u32>(0x101F84DC) + 0x798, 3) &&
        gabi::load<u8>(dComIfGp_ea() + 0x5134) != 'X' /* dComIfGp_getStartStageName()[0] */) {
        i_this->unk_43C = true;
        anm_setFrame_hd(i_this->unk_434, 299.0f, 0x68, 0x10);
        anm_setFrame_hd(i_this->unk_438, 299.0f, 0x10, 0x20);
    }

    gabi::Local<Mtx34> sp08;
    f32 s = REG0_F(9) + 1.0f;
    PSMTXScale(sp08.get(), s, s, s);

    if (i_this->unk_43C) {
        MtxTrans(0.0f, -10000.0f, 0.0f, false);
    } else {
        MtxTrans(0.0f, 0.0f, 0.0f, false);
        mDoExt_baseAnm_play_l(i_this->unk_430);
    }

    J3DModel_setBaseTRMtx(i_this->unk_428, calc_mtx());
    PSMTXConcat(calc_mtx(), sp08.get(), &i_this->unk_440);

    dBgW_Move(i_this->mpBgW1);
    if (!i_this->unk_43C) {
        MtxTrans(0.0f, -10000.0f, 0.0f, false);
    } else {
        MtxTrans(0.0f, 0.0f, 0.0f, false);
        mDoExt_baseAnm_play_l(i_this->unk_434);
        mDoExt_baseAnm_play_l(i_this->unk_438);
    }

    J3DModel_setBaseTRMtx(i_this->unk_42C, calc_mtx());
    PSMTXConcat(calc_mtx(), sp08.get(), &i_this->unk_470);
    dBgW_Move(i_this->mpBgW2);
}

/* 0212B3D0 */
static BOOL daDr2_Execute(dr2_class* i_this) {
    WWHD_FUNC(0x0212B3D0, BOOL, i_this);
    i_this->unk_2BC = (s16)(i_this->unk_2BC + 1);

    for (s32 i = 0; i < 5; i++) {
        if (i_this->unk_400[i] != 0) {
            i_this->unk_400[i] = (s16)(i_this->unk_400[i] - 1);
        }
    }

    move(i_this);

    fopAcM_SearchByID(i_this->unk_3FC);
    f32 s = l_HIO().m08;
    i_this->scale.z = s;
    i_this->scale.y = s;
    i_this->scale.x = s;

    J3DModel* model = i_this->mpMorf1->getModel();
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &i_this->current.pos, &i_this->tevStr);
    setLightTevColorType(dKy_getEnvlight(), model, &i_this->tevStr);
    J3DModel_setBaseScale(model, &i_this->scale);
    mDoMtx_stack_c::transS(i_this->current.pos.x, i_this->current.pos.y, i_this->current.pos.z);
    mDoMtx_stack_c::YrotM(i_this->current.angle.y);
    J3DModel_setBaseTRMtx(model, mDoMtx_stack_c::get());

    iwa_move(i_this);

    mDoMtx_stack_c::transS(i_this->unk_4A8.x, i_this->unk_4A8.y, i_this->unk_4A8.z);

    mDoMtx_stack_c::YrotM(i_this->unk_4B4.y);
    mDoMtx_XrotM(mDoMtx_stack_c::get(), i_this->unk_4B4.x);
    mDoMtx_stack_c::YrotM(i_this->home.angle.y);
    mDoMtx_ZrotM(mDoMtx_stack_c::get(), i_this->unk_4B4.z);
    mDoMtx_stack_c::YrotM((s16)-i_this->unk_4B4.y);

    J3DModel_setBaseTRMtx(i_this->unk_418, mDoMtx_stack_c::get());
    J3DModel_setBaseTRMtx(i_this->unk_41C, mDoMtx_stack_c::get());

    dr_move(i_this);
    yuka_move(i_this);
    return TRUE;
}
VERIFY(0x0212B3D0, daDr2_Execute);

/* 0212CD08 */
static BOOL daDr2_IsDelete(dr2_class*) {
    WWHD_FUNC(0x0212CD08, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x0212CD08, daDr2_IsDelete);

/* 0212CD10 */
static BOOL daDr2_Delete(dr2_class* i_this) {
    WWHD_FUNC(0x0212CD10, BOOL, i_this);
    dComIfG_resDelete(&i_this->mPhase, STR(0x1000E1D8) /* "Dr2" */);
    mDoHIO_deleteChild(l_HIO().mNo);
    i_this->unk_4D8.remove();
    gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(gabi::ea(i_this->unk_4EC)) + 0x44), i_this->unk_4EC); /* unk_4EC.remove() */
    mDoAud_seDeleteObject(&i_this->unk_4A8);
    dBgS* bgs = dComIfG_Bgsp();
    cBgS_Release(bgs, i_this->mpBgW1);
    bgs = dComIfG_Bgsp();
    cBgS_Release(bgs, i_this->mpBgW2);
    return TRUE;
}
VERIFY(0x0212CD10, daDr2_Delete);

static inline void* new_btkAnm() {
    void* p = operator_new(0x74);
    if (p != nullptr) p = gabi::call<void*>(0x025E7C6C, p); /* mDoExt_btkAnm::mDoExt_btkAnm */
    return p;
}
static inline BOOL btkAnm_init(void* a, J3DModelData* d, void* key, s32 mode) {
    return gabi::call<BOOL>(0x025E7CE0, a, d, key, (u32)1, mode, 1.0f, (s16)0, (s16)-1, (u32)0, (s32)0);
}

/* 0212CDAC */
static BOOL useHeapInit(fopAc_ac_c* a_this) {
    WWHD_FUNC(0x0212CDAC, BOOL, a_this);
    dr2_class* i_this = (dr2_class*)a_this;
#define ARC STR(0x1000E1DC) /* "Dr2" */

    J3DModelData* md = (J3DModelData*)dComIfG_getObjectRes(ARC, dRes_INDEX_DR2_BMD_DR_SIPPO_e, SAFESTRING_VTBL);
    i_this->mpMorf1 = mDoExt_McaMorf::create(nullptr, md, nullptr, nullptr, nullptr, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, 0,
                                             nullptr, 0, 0x11020203);

    mDoExt_McaMorf* morf = i_this->mpMorf1;
    if ((morf == nullptr) || (morf->getModel() == nullptr)) {
        return FALSE;
    }

    for (u16 i = 0; i < J3DModelData_getJointNum(J3DModel_getModelData(i_this->mpMorf1->getModel())); i++) {
        setJointCallBack(J3DModel_getModelData(i_this->mpMorf1->getModel()), i, 0x0212AACC /* nodeCallBack */);
    }
    gabi::store<u32>(gabi::ea(i_this->mpMorf1->getModel()) + 0xB8, gabi::ea(i_this)); /* setUserArea */

    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(ARC, dRes_INDEX_DR2_BDL_IWA00_e, SAFESTRING_VTBL);
    if (modelData == nullptr) /* JUT_ASSERT(1404, modelData != NULL) */
        JUT_ASSERT_fail(FILE_NAME, 0x57C, STR(0x1000E1EC));
    i_this->unk_418 = mDoExt_J3DModel__create(modelData, 0, 0x11020203);
    if (i_this->unk_418 == nullptr) {
        return FALSE;
    }

    modelData = (J3DModelData*)dComIfG_getObjectRes(ARC, dRes_INDEX_DR2_BMD_GAN_MAGMA_e, SAFESTRING_VTBL);
    if (modelData == nullptr)
        JUT_ASSERT_fail(FILE_NAME, 0x585, STR(0x1000E1EC));
    i_this->unk_41C = mDoExt_J3DModel__create(modelData, 0, 0x11020203);
    if (i_this->unk_41C == nullptr) {
        return FALSE;
    }

    i_this->unk_420 = new_btkAnm();
    if (i_this->unk_420 == nullptr) {
        return FALSE;
    }
    {
        J3DModel* m = i_this->unk_41C;
        void* srtKey = dComIfG_getObjectRes(ARC, dRes_INDEX_DR2_BTK_GAN_MAGMA_e, SAFESTRING_VTBL);
        if (!btkAnm_init(i_this->unk_420, J3DModel_getModelData(m), srtKey, J3DFrameCtrl::EMode_NONE)) {
            return FALSE;
        }
    }

    for (s32 i = 0; i < 6; i++) {
        /* static s32 hahen_model[] = { 15, 16, 17, 18, 19, 20 }; (0x101B47C8) */
        s32 idx = gabi::load<s32>(0x101B47C8 + 4 * i);
        modelData = (J3DModelData*)dComIfG_getObjectRes(ARC, idx, SAFESTRING_VTBL);
        i_this->mRockFragments[i].mpModel = mDoExt_J3DModel__create(modelData, 0, 0x11020203);
        if (i_this->mRockFragments[i].mpModel == nullptr) {
            return FALSE;
        }
    }

    {
        J3DModelData* d2 = (J3DModelData*)dComIfG_getObjectRes(ARC, dRes_INDEX_DR2_BMD_DR_e, SAFESTRING_VTBL);
        J3DAnmTransform* a2 = (J3DAnmTransform*)dComIfG_getObjectRes(ARC, dRes_INDEX_DR2_BCK_DR_BOSS_DEMO1_e, SAFESTRING_VTBL);
        i_this->mpMorf2 = mDoExt_McaMorf::create(nullptr, d2, nullptr, nullptr, a2, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, 0,
                                                 nullptr, 0, 0x11020203);
    }
    morf = i_this->mpMorf2;
    if (morf == nullptr || morf->getModel() == nullptr) {
        return FALSE;
    }

    modelData = (J3DModelData*)dComIfG_getObjectRes(ARC, dRes_INDEX_DR2_BMD_MBYO1_e, SAFESTRING_VTBL);
    if (modelData == nullptr)
        JUT_ASSERT_fail(FILE_NAME, 0x5C0, STR(0x1000E1EC));
    i_this->unk_428 = mDoExt_J3DModel__create(modelData, 0, 0x11020203);
    if (i_this->unk_428 == nullptr) {
        return FALSE;
    }

    modelData = (J3DModelData*)dComIfG_getObjectRes(ARC, dRes_INDEX_DR2_BMD_MBYO2_e, SAFESTRING_VTBL);
    if (modelData == nullptr)
        JUT_ASSERT_fail(FILE_NAME, 0x5C7, STR(0x1000E1EC));
    i_this->unk_42C = mDoExt_J3DModel__create(modelData, 0, 0x11020203);
    if (i_this->unk_42C == nullptr) {
        return FALSE;
    }

    i_this->unk_430 = new_btkAnm();
    if (i_this->unk_430 == nullptr) {
        return FALSE;
    }
    {
        J3DModel* m = i_this->unk_428;
        void* srtKey = dComIfG_getObjectRes(ARC, dRes_INDEX_DR2_BTK_MBYO1_e, SAFESTRING_VTBL);
        if (!btkAnm_init(i_this->unk_430, J3DModel_getModelData(m), srtKey, J3DFrameCtrl::EMode_LOOP)) {
            return FALSE;
        }
    }

    i_this->unk_434 = new_btkAnm();
    if (i_this->unk_434 == nullptr) {
        return FALSE;
    }
    {
        J3DModel* m = i_this->unk_42C;
        void* srtKey = dComIfG_getObjectRes(ARC, dRes_INDEX_DR2_BTK_MBYO2_e, SAFESTRING_VTBL);
        if (!btkAnm_init(i_this->unk_434, J3DModel_getModelData(m), srtKey, J3DFrameCtrl::EMode_NONE)) {
            return FALSE;
        }
    }

    {
        void* p = operator_new(0x78);
        if (p != nullptr) p = gabi::call<void*>(0x025E80D0, p); /* mDoExt_brkAnm::mDoExt_brkAnm */
        i_this->unk_438 = p;
    }
    if (i_this->unk_438 == nullptr) {
        return FALSE;
    }
    {
        J3DModel* m = i_this->unk_42C;
        void* tevRegKey = dComIfG_getObjectRes(ARC, dRes_INDEX_DR2_BRK_MBYO2_e, SAFESTRING_VTBL);
        if (!gabi::call<BOOL>(0x025E8154, i_this->unk_438.get(), J3DModel_getModelData(m), tevRegKey, (u32)1, (s32)0, 1.0f, (s16)0,
                              (s16)-1, (u32)0, (s32)0)) {
            return FALSE;
        }
    }

    i_this->mpBgW1 = new_dBgW();
    if (i_this->mpBgW1 == nullptr) {
        return FALSE;
    }

    i_this->mpBgW2 = new_dBgW();
    if (i_this->mpBgW2 == nullptr) {
        return FALSE;
    }

    cBgD_t* cBgD = (cBgD_t*)dComIfG_getObjectRes(ARC, dRes_INDEX_DR2_DZB_MBYO1_e, SAFESTRING_VTBL);
    cBgW_Set(i_this->mpBgW1, cBgD, cBgW_MOVE_BG_e, &i_this->unk_440);
    gabi::store<u32>(gabi::ea(i_this->mpBgW1.get()) + 0xA8, 0x024EE658); /* SetCrrFunc(dBgS_MoveBGProc_Typical) */

    cBgD = (cBgD_t*)dComIfG_getObjectRes(ARC, dRes_INDEX_DR2_DZB_MBYO2_e, SAFESTRING_VTBL);
    cBgW_Set(i_this->mpBgW2, cBgD, cBgW_MOVE_BG_e, &i_this->unk_470);
    gabi::store<u32>(gabi::ea(i_this->mpBgW2.get()) + 0xA8, 0x024EE658);

    return TRUE;
#undef ARC
}
VERIFY(0x0212CDAC, useHeapInit);

/* 0212D394 */
static cPhs_State daDr2_Create(fopAc_ac_c* a_this) {
    WWHD_FUNC(0x0212D394, cPhs_State, a_this);
    dr2_class* i_this = (dr2_class*)a_this;

    /* fopAcM_ct(&i_this->actor, dr2_class) */
    if (!fopAcM_CheckCondition(i_this, fopAcCnd_INIT_e)) {
        if (i_this != nullptr) {
            fopAc_ac_c_ct(i_this);
            i_this->__vtbl = DR2_VTBL;
            dPa_followEcallBack_ct(&i_this->unk_4D8, 0, 0);
            gabi::call(0x025A5B18, i_this->unk_4EC, (u8)1); /* dPa_smokeEcallBack::dPa_smokeEcallBack(1) */
        }
        fopAcM_OnCondition(i_this, fopAcCnd_INIT_e);
    }

    cPhs_State ret = dComIfG_resLoad(&i_this->mPhase, STR(0x1000E200) /* "Dr2" */);
    if (ret == cPhs_COMPLEATE_e) {
        gabi::store<u8>(gabi::ea(i_this->unk_4EC) + 0x12, 1); /* unk_4EC.setFollowOff() */
        if (!fopAcM_entrySolidHeap(a_this, 0x0212CDAC /* useHeapInit */, 0xDFE0)) {
            return cPhs_ERROR_e;
        }

        dBgS* bgs = dComIfG_Bgsp();
        if (dBgS_Regist(bgs, i_this->mpBgW1, a_this)) {
            return cPhs_ERROR_e;
        }

        bgs = dComIfG_Bgsp();
        if (dBgS_Regist(bgs, i_this->mpBgW2, a_this)) {
            return cPhs_ERROR_e;
        }

        l_HIO().mNo = mDoHIO_createChild(STR(0x1000E204) /* "ドラゴンシッポ" */, &l_HIO());
        /* fopAcM_Create(fpcNm_KUI_e, NULL, params) (HD inline) */
        u32 params = gabi::call<u32>(0x025D5600); /* fopAcM_CreateAppend */
        gabi::store<u32>(params, 0x511);
        gabi::store<s8>(params + 0x21, a_this->current.roomNo);
        u32 layer = gabi::call<u32>(0x025DED64); /* fpcLy_CurrentLayer */
        i_this->unk_3FC = gabi::call<u32>(0x025E14A8, layer, (s16)fpcNm_KUI_e, (u32)0, (u32)0, params); /* fpcSCtRq_Request */
        i_this->unk_4A8.x = a_this->home.pos.x;
        i_this->unk_4A8.y = a_this->home.pos.y + REG0_F(7) - 50.0f;
        i_this->unk_4A8.z = a_this->home.pos.z;
    }
    return ret;
}
VERIFY(0x0212D394, daDr2_Create);

/* 0212D59C */
static void __sinit_d_a_dr2_cpp() {
    WWHD_FUNC(0x0212D59C, void, (u32)0);
    sinit_header_statics(0x10463C34, 0x101B47E0);
    daDr2_HIO_c_ct(&l_HIO()); /* static daDr2_HIO_c l_HIO */
}
VERIFY(0x0212D59C, __sinit_d_a_dr2_cpp);

/* 0212D63C: deleting destructor of a class with a trivial destructor (daDr2_HIO_c) */
static void trivial_dt(void* p, s32 flags) {
    WWHD_FUNC(0x0212D63C, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x0212D63C, trivial_dt);

/* 0212D650: dr2_class deleting destructor (HD) */
static void dr2_class_dt(dr2_class* p, s32 flags) {
    WWHD_FUNC(0x0212D650, void, p, flags);
    if (p != nullptr) {
        gabi::call(0x025D50BC, p, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(p);
    }
}
VERIFY(0x0212D650, dr2_class_dt);

/* 0212D6A4: empty virtual */
static void empty_virtual(void* p) {
    WWHD_FUNC(0x0212D6A4, void, p);
}
VERIFY(0x0212D6A4, empty_virtual);
