/**
 * d_a_obj_ekskz.cpp (WWHD)
 * Object - Stone statue blowing a strong gust of wind (Gale Isle); breaks when hit by the
 * Skull Hammer.
 *
 * The GameCube decompilation (zeldaret/tww
 * src/d/actor/d_a_obj_ekskz.cpp) has only "Nonmatching" stubs for this unit, so every function
 * here is written from the WWHD code (cking.rpx) and verified against it.
 */
#include "bindings.h"

#define M_arcname STR(0x10027E90)      /* "Ekskz" */
#define SAFESTRING_VTBL 0x10027D68
#define FILE_NAME STR(0x10027E34)      /* "d_a_obj_ekskz.cpp" */
#define ACT_VTBL 0x10027E98            /* daObjEkskz::Act_c vtable (HD virtual destructor) */
#define CYL_AAB_VTBL 0x10027D80        /* this TU's cM3dGAab vtable */
#define BCK_VTBL 0x10027D90            /* this TU's mDoExt_bckAnm vtable */
#define cyl_check_src 0x101C8FEC       /* dCcD_SrcCyl (.data) */
#define M_tmp_mtx gabi::at<Mtx34>(0x104697C0)

enum {
    dRes_INDEX_EKSKZ_BCK_e = 0x08,
    dRes_INDEX_EKSKZ_BDL_e = 0x0B,
    dRes_INDEX_EKSKZ_BDL_EFF_e = 0x0E,
    dRes_INDEX_EKSKZ_BRK_e = 0x11,
    dRes_INDEX_EKSKZ_BTK_e = 0x14,
    dRes_INDEX_EKSKZ_DZB_e = 0x17,
};

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 025E1988 HD: mDoAud_seStart(id) (one argument; also local in d_a_mo2.h) */
static inline void mDoAud_seStart_1(u32 id) { gabi::call(0x025E1988, id); }
/* 025E80D0 mDoExt_brkAnm::mDoExt_brkAnm (the matcher calls it init), 025E8154 init [as d_a_oq.h] */
static inline void mDoExt_brkAnm_ct(void* p) { gabi::call(0x025E80D0, p); }
static inline BOOL mDoExt_brkAnm_init(void* a, J3DModelData* d, void* brk, s32 anmPlay, s32 mode, f32 rate, s16 start,
                                      s16 end, bool modify, s32 entry) {
    return gabi::call<BOOL>(0x025E8154, a, d, brk, anmPlay, mode, rate, start, end, modify, entry);
}
/* 025E83FC mDoExt_brkAnm::entry(J3DModelData*, f32) */
static inline void mDoExt_brkAnm_entry_l(void* a, J3DModelData* d, f32 frame) { gabi::call(0x025E83FC, a, d, frame); }
/* 0255FFF4 dKy_tevstr_init(tevstr, roomNo, u8) [as d_a_kb.h] */
static inline void dKy_tevstr_init_l(dKy_tevstr_c* t, s8 roomNo, u8 p) { gabi::call(0x0255FFF4, t, roomNo, p); }
/* GHS array helpers */
static inline void __construct_array_l(void* p, u32 n, u32 size, u32 ctor) { gabi::call(0x028EFFD0, p, n, size, ctor); }
static inline void __destroy_arr_l(void* p, u32 n, u32 size, u32 dtor) { gabi::call(0x028F0164, p, n, size, dtor, 0, 0); }

/* dKy_tevstr_c::dKy_tevstr_c (HD, inline): three 0x44-byte light blocks at +0, +0xC0, +0x144
 * from the default at 0x1016E414 (as in d_a_kb / d_a_npc_bmsw) */
static inline void tevstr_ct(dKy_tevstr_c* t) {
    u32 d = gabi::ea(t);
    const u32 s = 0x1016E414;
    static const u32 blocks[3] = {0x00, 0xC0, 0x144};
    for (u32 o : blocks) {
        for (int i = 0; i < 6; i++) gabi::store<f32>(d + o + 4 * i, gabi::load<f32>(s + 4 * i));
        for (int i = 0; i < 4; i++) gabi::store<u8>(d + o + 0x18 + i, gabi::load<u8>(s + 0x18 + i));
        for (int i = 0; i < 4; i++) gabi::store<s16>(d + o + 0x1C + 2 * i, gabi::load<s16>(s + 0x1C + 2 * i));
        for (int i = 0; i < 8; i++) gabi::store<f32>(d + o + 0x24 + 4 * i, gabi::load<f32>(s + 0x24 + 4 * i));
    }
}
/* inline mDoExt_bckAnm::mDoExt_bckAnm() (HD, 0x8C bytes; as d_a_itembase) */
static inline void bckAnm_ct(u32 b) {
    gabi::call(0x027F2BC0, b, 0); /* J3DFrameCtrl::init */
    gabi::store<u32>(b + 0x10, 0x1016E54C);
    gabi::call(0x027DA984, b + 0x14);
    gabi::store<u32>(b + 0x80, 0);
    gabi::store<u32>(b + 0x7C, 0);
    gabi::store<u32>(b + 0x88, 0);
    gabi::store<u32>(b + 0x58, 0);
    gabi::store<u32>(b + 0x10, BCK_VTBL);
    gabi::store<u32>(b + 0x84, 0);
    gabi::store<u32>(b + 0x48, 0x1016D820);
}
/* JPABaseEmitter global colours (HD: prm rgb at +0x244, env rgb at +0x248) */
static inline void emitter_setColors(u32 self, u32 emitterSlot, u32 color) {
    u32 e = gabi::load<u32>(self + emitterSlot);
    u8 r = gabi::load<u8>(color), g = gabi::load<u8>(color + 1), b = gabi::load<u8>(color + 2);
    gabi::store<u8>(e + 0x244, r);
    gabi::store<u8>(e + 0x245, g);
    gabi::store<u8>(e + 0x246, b);
    g = gabi::load<u8>(color + 1);
    r = gabi::load<u8>(color);
    e = gabi::load<u32>(self + emitterSlot);
    b = gabi::load<u8>(color + 2);
    gabi::store<u8>(e + 0x249, g);
    gabi::store<u8>(e + 0x248, r);
    gabi::store<u8>(e + 0x24A, b);
}

namespace daObjEkskz {
struct Act_c : dBgS_MoveBgActor {
    enum Prm_e { PRM_SWSAVE_W = 8, PRM_SWSAVE_S = 0 };
    s32 prm_get_swSave();
    BOOL CreateHeap();
    BOOL Create();
    cPhs_State Mthd_Create();
    BOOL Delete();
    BOOL Mthd_Delete();
    void set_mtx();
    void init_mtx();
    BOOL Execute(Mtx34** mtx);
    BOOL Draw();

    /* 0x3E0 */ request_of_phase_process_class mPhs;
    /* 0x3E8 */ gptr<J3DModel> mpModel;              /* GameCube 0x2D0 */
    /* 0x3EC */ dCcD_Stts mStts;
    /* 0x428 */ dCcD_Cyl mCyl;
    /* 0x558 */ gptr<J3DModel> mpModelEff;
    /* 0x55C */ mDoExt_bckAnm mBckAnm;               /* HD 0x8C */
    /* 0x5E8 */ mDoExt_baseAnm mBrkAnm;              /* mDoExt_brkAnm, HD 0x78 */
    /* 0x5F8 */ u8 _5F8[0x660 - 0x5F8];
    /* 0x660 */ mDoExt_btkAnm mBtkAnm;               /* HD 0x74 */
    /* 0x6D4 */ be<u8> m480;                         /* broken */
    /* 0x6D5 */ u8 _6D5[3];
    /* 0x6D8 */ u8 mSmokeCallback[4][0x20];          /* dPa_smokeEcallBack[4]: vtable +0, emitter +4 */
    /* 0x758 */ be<u32> m758;
    /* 0x75C */ be<u32> m75C;
    /* 0x760 */ gptr<JPABaseEmitter> mpSmoke0;
    /* 0x764 */ be<u32> m764;
    /* 0x768 */ dKy_tevstr_c mTevStr;
};
WWHD_OFFSET(Act_c, mpModel, 0x3E8);
WWHD_OFFSET(Act_c, mCyl, 0x428);
WWHD_OFFSET(Act_c, mBckAnm, 0x55C);
WWHD_OFFSET(Act_c, mBtkAnm, 0x660);
WWHD_OFFSET(Act_c, mSmokeCallback, 0x6D8);
WWHD_OFFSET(Act_c, mTevStr, 0x768);
WWHD_SIZE(Act_c, 0x930);
}  // namespace daObjEkskz
using daObjEkskz::Act_c;

static inline u32 smoke(Act_c* a, int i) { return gabi::ea(a) + 0x6D8 + 0x20 * i; }

/* 0233C3B8: daObj::PrmAbstract<Act_c::Prm_e>(actor, width, shift) (HD: out of line, per TU) */
static u32 PrmAbstract(fopAc_ac_c* a, s32 width, s32 shift) {
    WWHD_FUNC(0x0233C3B8, u32, a, width, shift);
    u32 sh = (u32)shift & 63, wd = (u32)width & 63;
    u32 v = sh < 32 ? fopAcM_GetParam(a) >> sh : 0;
    u32 m = (wd < 32 ? 1u << wd : 0u) - 1;
    return v & m;
}
VERIFY(0x0233C3B8, PrmAbstract);
s32 Act_c::prm_get_swSave() { return PrmAbstract(this, PRM_SWSAVE_W, PRM_SWSAVE_S); }

/* 0233B4A0: dPa_smokeEcallBack array element constructor (__construct_array) */
static u32 smokeEcallBack_ct(void* p) {
    WWHD_FUNC(0x0233B4A0, u32, p);
    return gabi::call<u32>(0x025A5B18, p, (u8)1);
}
VERIFY(0x0233B4A0, smokeEcallBack_ct);

/* 0233B4A8: Act_c::Act_c (inline in fopAcM_ct; HD: allocates when this == NULL) */
static Act_c* Act_c_ct(Act_c* self) {
    WWHD_FUNC(0x0233B4A8, Act_c*, self);
    if (self == nullptr) {
        self = (Act_c*)operator_new(sizeof(Act_c));
        if (self == nullptr)
            return self;
    }
    dBgS_MoveBgActor::ct(self);
    self->__vtbl = ACT_VTBL;
    dCcD_Stts_ct(&self->mStts);
    dCcD_Cyl_ct(&self->mCyl, CYL_AAB_VTBL);
    bckAnm_ct(gabi::ea(&self->mBckAnm));
    mDoExt_brkAnm_ct(&self->mBrkAnm);
    mDoExt_btkAnm::ct(&self->mBtkAnm);
    __construct_array_l(self->mSmokeCallback, 4, 0x20, 0x0233B4A0);
    tevstr_ct(&self->mTevStr);
    return self;
}
VERIFY(0x0233B4A8, Act_c_ct);

/* 0233B74C */
cPhs_State Act_c::Mthd_Create() {
    WWHD_FUNC(0x0233B74C, cPhs_State, this);
    /* fopAcM_ct(this, Act_c) */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr)
            Act_c_ct(this);
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    mpSmoke0 = nullptr;
    m764 = 0;
    m75C = 0;
    m758 = 0;
    if (fopAcM_isSwitch(this, prm_get_swSave()))
        return (cPhs_State)3; /* cPhs_STOP_e: already broken */

    cPhs_State phase_state = dComIfG_resLoad(&mPhs, M_arcname);
    if (phase_state == cPhs_COMPLEATE_e) {
        phase_state = MoveBGCreate(M_arcname, dRes_INDEX_EKSKZ_DZB_e, 0, 0x28A0);
        if (!((phase_state == cPhs_COMPLEATE_e) || (phase_state == cPhs_ERROR_e))) /* JUT_ASSERT(0x13D, ...) */
            JUT_ASSERT_fail(STR(0x10027DB8), 0x13D, STR(0x10027DCC));
    }
    return phase_state;
}
VERIFY(0x0233B74C, &Act_c::Mthd_Create);

/* 0233B874 */
BOOL Act_c::Mthd_Delete() {
    WWHD_FUNC(0x0233B874, BOOL, this);
    BOOL ret = MoveBGDelete();
    /* base_process_class +0xD: the create result; 3 (cPhs_STOP_e) when Mthd_Create stopped
     * before loading the archive */
    if (gabi::load<u8>(gabi::ea(this) + 0xD) != 3)
        dComIfG_resDelete(&mPhs, M_arcname);
    return ret;
}
VERIFY(0x0233B874, &Act_c::Mthd_Delete);

/* 0233B8CC */
BOOL Act_c::CreateHeap() {
    WWHD_FUNC(0x0233B8CC, BOOL, this);
    J3DModelData* model_data = (J3DModelData*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_EKSKZ_BDL_e, SAFESTRING_VTBL);
    if (model_data == nullptr) /* JUT_ASSERT(0x90, model_data != NULL) */
        JUT_ASSERT_fail(FILE_NAME, 0x90, STR(0x10027E10));
    J3DModel* m = mDoExt_J3DModel__create(model_data, 0, 0x11020203);
    mpModel = m;
    if (m == nullptr)
        return FALSE;

    J3DModelData* model_data_eff = (J3DModelData*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_EKSKZ_BDL_EFF_e, SAFESTRING_VTBL);
    if (model_data_eff == nullptr) /* JUT_ASSERT(0x9B, model_data_eff != NULL) */
        JUT_ASSERT_fail(FILE_NAME, 0x9B, STR(0x10027E20));
    m = mDoExt_J3DModel__create(model_data_eff, 0, 0x11020203);
    mpModelEff = m;
    if (m == nullptr)
        return FALSE;

    J3DAnmTransform* bck = (J3DAnmTransform*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_EKSKZ_BCK_e, SAFESTRING_VTBL);
    if (bck == nullptr) /* JUT_ASSERT(0xA4, bck != NULL) */
        JUT_ASSERT_fail(FILE_NAME, 0xA4, STR(0x10027E48));
    if (!mBckAnm.init(model_data_eff, bck, true, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, false))
        return FALSE;

    J3DAnmTextureSRTKey* btk = (J3DAnmTextureSRTKey*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_EKSKZ_BTK_e, SAFESTRING_VTBL);
    if (btk == nullptr) /* JUT_ASSERT(0xAB, btk != NULL) */
        JUT_ASSERT_fail(FILE_NAME, 0xAB, STR(0x10027E54));
    if (!mBtkAnm.init(model_data_eff, btk, true, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, false, 0))
        return FALSE;

    void* brk = dComIfG_getObjectRes(M_arcname, dRes_INDEX_EKSKZ_BRK_e, SAFESTRING_VTBL);
    if (brk == nullptr) /* JUT_ASSERT(0xB2, brk != NULL) */
        JUT_ASSERT_fail(FILE_NAME, 0xB2, STR(0x10027E60));
    if (!mDoExt_brkAnm_init(&mBrkAnm, model_data_eff, brk, true, J3DFrameCtrl::EMode_NONE, 1.0f, 0, -1, false, 0))
        return FALSE;
    return TRUE;
}
VERIFY(0x0233B8CC, &Act_c::CreateHeap);

/* 0233BB30 */
void Act_c::set_mtx() {
    WWHD_FUNC(0x0233BB30, void, this);
    u16 ax = gabi::load<u16>(gabi::ea(this) + 0x320), ay = gabi::load<u16>(gabi::ea(this) + 0x322);
    shape_angle.x = (s16)ax;
    f32 x = current.pos.x;
    u16 az = gabi::load<u16>(gabi::ea(this) + 0x324);
    f32 y = current.pos.y, z = current.pos.z;
    shape_angle.y = (s16)ay;
    shape_angle.z = (s16)az;
    mDoMtx_stack_c::transS(x, y, z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y, shape_angle.z);
    J3DModel_setBaseTRMtx(mpModel, mDoMtx_stack_c::get());
    J3DModel_setBaseTRMtx(mpModelEff, mDoMtx_stack_c::get());
    PSMTXCopy(mDoMtx_stack_c::get(), M_tmp_mtx);
}
VERIFY(0x0233BB30, &Act_c::set_mtx);

/* 0233BC80 */
void Act_c::init_mtx() {
    WWHD_FUNC(0x0233BC80, void, this);
    J3DModel_setBaseScale(mpModel, &scale);
    J3DModel_setBaseScale(mpModelEff, &scale);
    set_mtx();
}
VERIFY(0x0233BC80, &Act_c::init_mtx);

/* 0233BCBC */
BOOL Act_c::Create() {
    WWHD_FUNC(0x0233BCBC, BOOL, this);
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpModel)); /* fopAcM_SetMtx */
    init_mtx();
    fopAcM_setCullSizeBox(this, -4000.0f, -500.0f, -4000.0f, 4000.0f, 500.0f, 4000.0f);
    mStts.Init(0xFF, 0xFF, this);
    mCyl.Set(gabi::at<dCcD_SrcCyl>(cyl_check_src));
    mCyl.SetC(&current.pos);
    mCyl.SetStts(&mStts);
    m480 = 0;
    dKy_tevstr_init_l(&mTevStr, home.roomNo, 0xFF);

    s8 room = current.roomNo;
    dPa_control_c* pa = dComIfGp_getParticle();
    dPa_control_set(pa, 2, 0xA2C2, &current.pos, &current.angle, nullptr, 0xA0,
                    gabi::at<dPa_levelEcallBack>(smoke(this, 0)), room, nullptr, nullptr, nullptr);
    room = current.roomNo;
    pa = dComIfGp_getParticle();
    dPa_control_set(pa, 2, 0xA2C3, &current.pos, &current.angle, nullptr, 0xA0,
                    gabi::at<dPa_levelEcallBack>(smoke(this, 1)), room, nullptr, nullptr, nullptr);

    /* the smoke takes the colour of the statue's light (mTevStr + 0x98) */
    if (gabi::load<u32>(smoke(this, 0) + 4) != 0)
        emitter_setColors(gabi::ea(this), 0x6DC, gabi::ea(&mTevStr) + 0x98);
    if (gabi::load<u32>(smoke(this, 1) + 4) != 0)
        emitter_setColors(gabi::ea(this), 0x6FC, gabi::ea(&mTevStr) + 0x98);

    gabi::store<u8>(gabi::ea(this) + 0x388, 0x31); /* attention_info.distances[0] */
    f32 y = current.pos.y + 200.0f;
    gabi::store<f32>(gabi::ea(this) + 0x394, y);   /* attention_info.position.y */
    eyePos.y = y;
    return TRUE;
}
VERIFY(0x0233BCBC, &Act_c::Create);

/* 0233BEC8 */
BOOL Act_c::Execute(Mtx34** mtx) {
    WWHD_FUNC(0x0233BEC8, BOOL, this, mtx);
    gabi::store<u32>(gabi::ea(this) + 0x39C, gabi::load<u32>(gabi::ea(this) + 0x39C) | 1); /* attention_info.flags */
    mBckAnm.play();
    mBtkAnm.play();
    dComIfG_Ccsp_Set(&mCyl);
    if (m480 == 0) {
        if (mCyl.ChkTgHit()) {
            mDoAud_seStart_1(0x806);
            dComIfGs_onSwitch(prm_get_swSave(), home.roomNo);
            m480 = 1;
            dPa_smokeEcallBack_end(gabi::at<dPa_smokeEcallBack>(smoke(this, 0)));
            dPa_smokeEcallBack_end(gabi::at<dPa_smokeEcallBack>(smoke(this, 1)));
            u32 c = gabi::ea(&tevStr) + 0x98;
            u8 r = gabi::load<u8>(c), g = gabi::load<u8>(c + 1), b = gabi::load<u8>(c + 2);
            JPABaseEmitter* e = dComIfGp_particle_set(0x82EC, &current.pos, &current.angle);
            mpSmoke0 = e;
            if (e != nullptr) {
                gabi::store<u8>(gabi::ea(e) + 0x244, r);
                gabi::store<u8>(gabi::ea(e) + 0x245, g);
                gabi::store<u8>(gabi::ea(e) + 0x246, b);
            }
            gabi::store<u8>(smoke(this, 3) + 0x15, 1);
            gabi::store<u8>(smoke(this, 3) + 0x12, 1);
            s8 room = current.roomNo;
            dPa_control_c* pa = dComIfGp_getParticle();
            dPa_control_set(pa, 2, 0xA2ED, &current.pos, &current.angle, nullptr, 0x80,
                            gabi::at<dPa_levelEcallBack>(smoke(this, 3)), room, nullptr, nullptr, nullptr);
            gabi::store<u32>(smoke(this, 3) + 0x1C, gabi::ea(&mTevStr));
        }
        if (m480 == 0)
            goto end;
    }
    mBrkAnm.play();
    if (mBrkAnm.mFrameCtrl.checkState(J3DFrameCtrl::STATE_STOP_E) || mBrkAnm.mFrameCtrl.getRate() == 0.0f)
        fopAcM_delete(this);
end:
    set_mtx();
    gabi::store<u32>(gabi::ea(mtx), gabi::ea(M_tmp_mtx));
    return TRUE;
}
VERIFY(0x0233BEC8, &Act_c::Execute);

/* 0233C0C8 */
BOOL Act_c::Draw() {
    WWHD_FUNC(0x0233C0C8, BOOL, this);
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &mTevStr);
    setLightTevColorType(dKy_getEnvlight(), mpModelEff, &mTevStr);
    mBckAnm.entry(J3DModel_getModelData(mpModelEff), mBckAnm.getFrame());
    mBtkAnm.entry(J3DModel_getModelData(mpModelEff), mBtkAnm.getFrame());
    mDoExt_brkAnm_entry_l(&mBrkAnm, J3DModel_getModelData(mpModelEff), mBrkAnm.getFrame());
    mDoExt_modelUpdateDL(mpModelEff);
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), mpModel, &tevStr);
    dComIfGd_setListBG();
    if (m480 == 0)
        mDoExt_modelUpdateDL(mpModel);
    dComIfGd_setList();
    return TRUE;
}
VERIFY(0x0233C0C8, &Act_c::Draw);

/* 0233C1D8 */
BOOL Act_c::Delete() {
    WWHD_FUNC(0x0233C1D8, BOOL, this);
    for (int i = 0; i < 4; i++) {
        /* mSmokeCallback[i].remove() (virtual, slot 0x44) */
        u32 cb = smoke(this, i);
        gabi::call_ptr<void>(gabi::load<u32>(gabi::load<u32>(cb) + 0x44), cb);
    }
    return TRUE;
}
VERIFY(0x0233C1D8, &Act_c::Delete);

/* method table (0x101C9084): Create, Delete, Execute, IsDelete, Draw (HD: tail branches) */
/* 0233C22C */
static cPhs_State Mthd_Create(void* i_this) {
    WWHD_FUNC(0x0233C22C, cPhs_State, i_this);
    return ((Act_c*)i_this)->Mthd_Create();
}
VERIFY(0x0233C22C, Mthd_Create);
/* 0233C230 */
static BOOL Mthd_Delete(void* i_this) {
    WWHD_FUNC(0x0233C230, BOOL, i_this);
    return ((Act_c*)i_this)->Mthd_Delete();
}
VERIFY(0x0233C230, Mthd_Delete);
/* 0233C234 */
static BOOL Mthd_Execute(void* i_this) {
    WWHD_FUNC(0x0233C234, BOOL, i_this);
    return ((Act_c*)i_this)->MoveBGExecute();
}
VERIFY(0x0233C234, Mthd_Execute);
/* 0233C238: virtual Draw */
static BOOL Mthd_Draw(void* i_this) {
    WWHD_FUNC(0x0233C238, BOOL, i_this);
    return ((Act_c*)i_this)->Draw_v();
}
VERIFY(0x0233C238, Mthd_Draw);
/* 0233C248: virtual IsDelete */
static BOOL Mthd_IsDelete(void* i_this) {
    WWHD_FUNC(0x0233C248, BOOL, i_this);
    return ((Act_c*)i_this)->IsDelete_v();
}
VERIFY(0x0233C248, Mthd_IsDelete);

/* 0233C258 */
static void __sinit_d_a_obj_ekskz_cpp() {
    WWHD_FUNC(0x0233C258, void, (u32)0);
    sinit_header_statics(0x104697A4, 0x101C9030);
}
VERIFY(0x0233C258, __sinit_d_a_obj_ekskz_cpp);

/* 0233C2EC: sead::SafeString deleting destructor (this TU's copy; trivial) */
static void SafeString_dt(void* p, s32 flags) {
    WWHD_FUNC(0x0233C2EC, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x0233C2EC, SafeString_dt);

/* 0233C300: dPa_smokeEcallBack array element deleting destructor (__destroy_arr; trivial) */
static void smokeEcallBack_dt(void* p, s32 flags) {
    WWHD_FUNC(0x0233C300, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x0233C300, smokeEcallBack_dt);

/* 0233C314: dBgS_MoveBgActor::IsDelete (this TU's copy) */
static BOOL dBgS_MoveBgActor_IsDelete(void* p) {
    WWHD_FUNC(0x0233C314, BOOL, p);
    return TRUE;
}
VERIFY(0x0233C314, dBgS_MoveBgActor_IsDelete);

/* 0233C31C: empty virtual in this TU's sead::SafeString vtable */
static void SafeString_empty(void* p) {
    WWHD_FUNC(0x0233C31C, void, p);
}
VERIFY(0x0233C31C, SafeString_empty);

/* 0233C320: Act_c deleting destructor (compiler-generated, HD virtual destructor) */
static void Act_c_dt(Act_c* i_this, s32 flags) {
    WWHD_FUNC(0x0233C320, void, i_this, flags);
    if (i_this != nullptr) {
        __destroy_arr_l(i_this->mSmokeCallback, 4, 0x20, 0x0233C300);
        gabi::call(0x027F3628, gabi::ea(&i_this->mBckAnm) + 0x10, 0); /* mDoExt_bckAnm's J3DMtxCalc part */
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x0233C320, Act_c_dt);
