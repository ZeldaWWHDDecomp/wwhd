/**
 * d_a_warpf.cpp (WWHD)
 * Boss warp flower / blue warp light after a boss (and the Tower of the Gods / Earth and Wind
 * Temple variants).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_warpf.cpp) to the WWHD layout and verified against cking.rpx.
 * "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_warpf.h"
static const u32 m_arcname[16] = {
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x10042230,
    0x10042238,
    0x10042238,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000
};
static const f32 m_warp_size[16] = {
    100.0f,
    100.0f,
    100.0f,
    100.0f,
    100.0f,
    200.0f,
    100.0f,
    100.0f,
    100.0f,
    100.0f,
    100.0f,
    100.0f,
    100.0f,
    100.0f,
    100.0f,
    100.0f
};
static const f32 l_earth_pos[16] = {
    0.0f,
    0.0f,
    0.0f,
    -100.0f,
    100.0f,
    0.0f,
    0.0f,
    300.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f
};
static const f32 l_earth_pos_2nd[16] = {
    0.0f,
    0.0f,
    0.0f,
    -100.0f,
    0.0f,
    0.0f,
    0.0f,
    300.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f
};
static u32 action_table[6] = {
    0x100423DC,
    0x100423E4,
    0x100423F0,
    0x100423F8,
    0x10042400,
    0x10042408
};
static WarpfInitEntry event_init_tbl[6] = {
    {&daWarpf_c::initWait},
    {&daWarpf_c::initWarpStart},
    {&daWarpf_c::initWarpMode_1},
    {&daWarpf_c::initWarpMode_2},
    {&daWarpf_c::initWarpMode_3},
    {&daWarpf_c::initEndWait}
};
static WarpfActionEntry event_action_tbl[6] = {
    {&daWarpf_c::actWait},
    {&daWarpf_c::actWarpStart},
    {&daWarpf_c::actWarpMode_1},
    {&daWarpf_c::actWarpMode_2},
    {&daWarpf_c::actWarpMode_3},
    {&daWarpf_c::actEndWait}
};
const gc_tu_data gc_tu = {.safeString = 0x10042244};
static const WarpfTU warpf_tu = {0x1004225C, 0x10042284, 0x1016E54C, 0x1016D820, 0x024D6790, 0x1046EB10, 0x101D2EB0};
const char* daWarpf_c::arcname() {return Warpf_view<const char>(m_arcname[u32(s32(mStageNo))]);}

enum {
    STAGE_DRC = 3,
    STAGE_FW = 4,
    STAGE_TOTG = 5,
    STAGE_ET = 6,
    STAGE_WT = 7,
};

enum {
    /* Ysbwp00 */
    dRes_INDEX_YSBWP00_BCK_YSBWP00_e = 6,
    dRes_INDEX_YSBWP00_BDL_YSBWP00_e = 9,
    dRes_INDEX_YSBWP00_BRK_YSBWP00_e = 0xC,
    dRes_INDEX_YSBWP00_BTK_YSBWP00_e = 0xF,
    /* Gtfglow */
    dRes_INDEX_GTFGLOW_BDL_GDEMO29_A00_e = 5,
    dRes_INDEX_GTFGLOW_BDL_GDEMO29_B00_e = 6,
    dRes_INDEX_GTFGLOW_BDL_GTFGLOW00_e = 7,
    dRes_INDEX_GTFGLOW_BRK_GDEMO29_A01_e = 0xB,
    dRes_INDEX_GTFGLOW_BRK_GTFGLOW00_e = 0xC,
    dRes_INDEX_GTFGLOW_BRK_GTFGLOW01_e = 0xD,
    dRes_INDEX_GTFGLOW_BRK_GTFGLOW02_e = 0xE,
};

enum {
    dItemNo_MASTER_SWORD_2_e = 0x3A,
    dItemNo_MASTER_SWORD_3_e = 0x3E,
    dItemNo_PEARL_DIN_e = 0x6A,
    dItemNo_PEARL_FARORE_e = 0x6B,
};

enum {
    ID_AK_SN_BSTWARP00 = 0x81C8,
    ID_AK_SN_BOSSWARPWIND00_820A = 0x820A,
    ID_AK_SN_BOSSWARPWIND01_820B = 0x820B,
    ID_AK_SN_BOSSWARPKIRAKIRA00_820C = 0x820C,
    ID_IT_SN_SHINDENTF_LIGHT00 = 0x841D,
    ID_IT_SN_SHINDENTF_STAR00 = 0x841E,
    ID_IT_SN_DEMO29_STAR00 = 0x8423,
};

enum {
    JA_SE_LK_BOSS_WARP_EFF_ST = 0x287D,
    JA_SE_CM_BST_WARP_BEAM = 0x698F,
    JA_SE_OBJ_BOSS_WARP_APPEAR = 0x69E5,
    JA_SE_OBJ_WARP_EFF_SUS = 0x7025,
    JA_SE_OBJ_BOSS_WARP_LV = 0x704A,
    JA_SE_OBJ_BOSS_TF_WARP_SUS = 0x7059,
};

/* Unit-specific HD operations. Shared accessors come from gc_api. */

/* dStage_stagInfo_GetSaveTbl: bits 1..7 of byte +9 */
static inline u32 Warpf_getSaveTable(void* info) { return (Warpf_view<WarpfStageInfo>(gabi::ea(info))->mSaveTable >> 1) & 0x7F; }
/* 025DAE00 fopKyM_fastCreate(s16 name, u32 param, cXyz* pos, cXyz* scale, void* cb) */
static inline u32 fopKyM_fastCreate(s16 name, u32 param, cXyz* pos, cXyz* scale, void* cb) {
    return Warpf_fastCreate(name, param, pos, scale, cb);
}
enum { fpcNm_LEVEL_SE_e = 0x18 };
/* dLevelSe_c (HD): flag byte +0x101 (bit 8: stopped, bit 4: reverb set), reverb +0x100, +0xFC */
static inline void dLevelSe_seStop(u32 se) { Warpf_view<dLevelSe_c>(se)->mFlag = u8(Warpf_view<dLevelSe_c>(se)->mFlag) | dLevelSe_c::STOPPED; }
static inline void dLevelSe_seStart(u32 se) { Warpf_view<dLevelSe_c>(se)->mFlag = u8(Warpf_view<dLevelSe_c>(se)->mFlag) & ~dLevelSe_c::STOPPED; }
/* JPABaseEmitter (HD): becomeInvalidEmitter = stopCreateParticle (flags +0x254 |= 1) + maxFrame (+0x5C) = -1 */
static inline void JPABaseEmitter_becomeInvalidEmitter(u32 e) {
    u32 f = u32(Warpf_view<WarpfEmitter>(e)->mFlags);
    Warpf_view<WarpfEmitter>(e)->mMaxFrame = -1;
    Warpf_view<WarpfEmitter>(e)->mFlags = f | 1;
}
static inline void JPABaseEmitter_setMaxFrame(u32 e, s32 n) { Warpf_view<WarpfEmitter>(e)->mMaxFrame = n; }
static inline void JPABaseEmitter_setGlobalAlpha(u32 e, u8 a) { Warpf_view<WarpfEmitter>(e)->mGlobalAlpha = a; }
/* mDoExt_brkAnm (HD): constructor 025E80D0 allocates when this == NULL (the matcher calls it init);
 * init 025E8154 (this, data, brk, bool anmPlay, mode, f32 speed, s16 start, s16 end, bool, int (stack));
 * the frame control (rate +0, frame +4, end +0xA) is first */
static inline u32 new_mDoExt_brkAnm() { return Warpf_brkConstruct((u32)0); }
static inline BOOL mDoExt_brkAnm_init(u32 a, J3DModelData* d, void* brk, bool play, s32 mode, f32 speed, s16 start, s16 end, bool b, s32 i) {
    return Warpf_brkInit(a, d, brk, play, mode, speed, start, end, b, i);
}
/* 025E83FC brkAnm::entry(J3DModelData*, f32); HD: 025E8480 brkAnm::entry(J3DModel*, f32) */
static inline void mDoExt_brkAnm_entry_model(u32 a, J3DModel* m, f32 frame) { Warpf_brkEntryModel(a, m, frame); }
/* mDoExt_btkAnm (HD 0x74): constructor 025E7C6C allocates when this == NULL */
static inline u32 new_mDoExt_btkAnm() { return Warpf_btkConstruct((u32)0); }
/* mDoExt_bckAnm inline constructor (HD 0x8C, as d_a_arrow_iceeff) */
static inline void mDoExt_bckAnm_ct(u32 p, u32 tu_vtbl) {
    Warpf_frameConstruct(p, 0); /* J3DFrameCtrl::init */
    Warpf_view<WarpfBckAnimation>(p)->mVtable = warpf_tu.mBaseAnmVtable;
    Warpf_matrixConstruct(&Warpf_view<WarpfBckAnimation>(p)->mMatrix);
    Warpf_view<WarpfBckAnimation>(p)->mBlend = 0;
    Warpf_view<WarpfBckAnimation>(p)->mMatrixAnm = 0;
    Warpf_view<WarpfBckAnimation>(p)->mMatrixVtable = warpf_tu.mMatrixVtable;
    Warpf_view<WarpfBckAnimation>(p)->mJoint = 0;
    Warpf_view<WarpfBckAnimation>(p)->mVtable = tu_vtbl;
    Warpf_view<WarpfBckAnimation>(p)->mFrameData = 0;
    Warpf_view<WarpfBckAnimation>(p)->mModel = 0;
}
static inline f32 anm_getRate(u32 a) { return Warpf_view<mDoExt_baseAnm>(a)->getPlaySpeed(); }
static inline void anm_setRate(u32 a, f32 r) { Warpf_view<mDoExt_baseAnm>(a)->setPlaySpeed(r); }
static inline f32 anm_getFrame(u32 a) { return Warpf_view<mDoExt_baseAnm>(a)->getFrame(); }
static inline void anm_setFrame(u32 a, f32 f) { Warpf_view<mDoExt_baseAnm>(a)->setFrame(f); }
static inline f32 anm_getEndFrame(u32 a) { return Warpf_view<mDoExt_baseAnm>(a)->getEndFrame(); }
/* 0211D2F8 cLib_calcTimer<int> */
static inline s32 cLib_calcTimer(be<s32>* t) { return Warpf_calcTimer(t); }
/* 024F1350 dBgS_ObjGndChk_Func(cXyz&) */
static inline f32 dBgS_ObjGndChk_Func(cXyz* pos) { return Warpf_groundCheck(pos); }

/* dComIfGp_getCamera(0): play+0x5AF8; dCam_getAngleX 024F8008, dCam_getAngleY 024F8000 */

static inline s16 dCam_getAngleX(u32 cam) { return Warpf_cameraAngleX(cam); }
static inline s16 dCam_getAngleY(u32 cam) { return Warpf_cameraAngleY(cam); }
/* daPy_py_c::checkPlayerFly(): virtual, vtable (+0xB4) slot +0x4C */
static inline BOOL daPy_checkPlayerFly(fopAc_ac_c* p) {
    return gabi::call_ptr<BOOL>(Warpf_view<WarpfPlayerVtable>(p->__vtbl)->mCheckFly, p);
}
/* (player->current.pos - pos).absXZ(): cXyz::operator- into a temporary, then sqrt of the XZ square magnitude */
static inline f32 absXZ_diff(cXyz* a, cXyz* b) {
    gabi::Local<cXyz> d;
    cXyz_mi(a, d.get(), b);
    gabi::Local<cXyz> xz;
    f32 x = d->x;
    xz->x = x;
    xz->y = 0.0f;
    f32 z = d->z;
    xz->z = z;
    return std_sqrtf(PSVECSquareMag(xz.get()));
}
/* pointer to member function with one int argument: {s16 delta, s16 vtable index, u32 fn / vtable offset} */
// HD: fopAcM_seStart without the actor/eyePos null checks

static inline void seStart_eye(fopAc_ac_c* a, u32 id, u32 param) {
    s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(a));
    mDoAud_seStart(id, &a->eyePos, param, reverb);
}

/* 024D6790 */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    return ((daWarpf_c*)i_this)->CreateHeap();
}

/* 024D6114 */
BOOL daWarpf_c::CreateHeap() {
    J3DModelData* modelData;
    void* pbrk;
    void* pbck;
    void* pbtk;

    switch ((u32)(s32)mStageNo) {
    case STAGE_TOTG: {
        modelData = (J3DModelData*)dComIfG_getObjectRes(arcname(), dRes_INDEX_YSBWP00_BDL_YSBWP00_e, gc_tu.safeString);
        if (modelData == nullptr)
            JUT_ASSERT_fail("d_a_warpf.cpp", 343, "modelData != (0)");

        m2A8 = mDoExt_J3DModel__create(modelData, 0x80000, 0x11000222);
        if (!m2A8)
            return FALSE;

        pbrk = dComIfG_getObjectRes(arcname(), dRes_INDEX_YSBWP00_BRK_YSBWP00_e, gc_tu.safeString);
        if (pbrk == nullptr)
            JUT_ASSERT_fail("d_a_warpf.cpp", 358, "pbrk != (0)");

        u32 brk = new_mDoExt_brkAnm();
        m2AC = brk;
        if (brk == 0 || !mDoExt_brkAnm_init(brk, modelData, pbrk, true, J3DFrameCtrl::EMode_NONE, 1.0f, 0, -1, false, 0))
            return FALSE;

        pbck = dComIfG_getObjectRes(arcname(), dRes_INDEX_YSBWP00_BCK_YSBWP00_e, gc_tu.safeString);
        if (pbck == nullptr)
            JUT_ASSERT_fail("d_a_warpf.cpp", 374, "pbck != (0)");

        u32 bck = gabi::ea(operator_new(0x8C));
        if (bck != 0)
            mDoExt_bckAnm_ct(bck, warpf_tu.mBckVtable);
        m2B0 = bck;
        if (bck == 0 ||
            !Warpf_view<mDoExt_bckAnm>(bck)->init(modelData, (J3DAnmTransform*)pbck, true, J3DFrameCtrl::EMode_NONE, 1.0f, 0, -1, false))
            return FALSE;

        pbtk = dComIfG_getObjectRes(arcname(), dRes_INDEX_YSBWP00_BTK_YSBWP00_e, gc_tu.safeString);
        if (pbtk == nullptr)
            JUT_ASSERT_fail("d_a_warpf.cpp", 391, "pbtk != (0)");

        u32 btk = new_mDoExt_btkAnm();
        m2B4 = btk;
        if (btk == 0 || !Warpf_view<mDoExt_btkAnm>(btk)->init(modelData, (J3DAnmTextureSRTKey*)pbtk, true,
                                                              J3DFrameCtrl::EMode_NONE, 1.0f, 0, -1, false, 0))
            return FALSE;
        break;
    }

    case STAGE_ET:
    case STAGE_WT:
        if (!checkEndDemo()) {
            m2C0 = nullptr;

            modelData = (J3DModelData*)dComIfG_getObjectRes(arcname(), dRes_INDEX_GTFGLOW_BDL_GTFGLOW00_e, gc_tu.safeString);
            if (modelData == nullptr)
                JUT_ASSERT_fail("d_a_warpf.cpp", 411, "modelData != (0)");

            m2A8 = mDoExt_J3DModel__create(modelData, 0x80000, 0x11000022);
            if (!m2A8)
                return FALSE;

            pbrk = dComIfG_getObjectRes(arcname(), dRes_INDEX_GTFGLOW_BRK_GTFGLOW00_e, gc_tu.safeString);
            if (pbrk == nullptr)
                JUT_ASSERT_fail("d_a_warpf.cpp", 424, "pbrk != (0)");

            u32 brk = new_mDoExt_brkAnm();
            m2AC = brk;
            if (brk == 0 || !mDoExt_brkAnm_init(brk, modelData, pbrk, true, J3DFrameCtrl::EMode_NONE, 1.0f, 0, -1, false, 0))
                return FALSE;
            anm_setRate(m2AC, 1.0f);

            pbrk = dComIfG_getObjectRes(arcname(), dRes_INDEX_GTFGLOW_BRK_GTFGLOW01_e, gc_tu.safeString);
            if (pbrk == nullptr)
                JUT_ASSERT_fail("d_a_warpf.cpp", 441, "pbrk != (0)");

            brk = new_mDoExt_brkAnm();
            m2B8 = brk;
            if (brk == 0 || !mDoExt_brkAnm_init(brk, modelData, pbrk, true, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, false, 0))
                return FALSE;
            anm_setRate(m2B8, 0.0f);

            pbrk = dComIfG_getObjectRes(arcname(), dRes_INDEX_GTFGLOW_BRK_GTFGLOW02_e, gc_tu.safeString);
            if (pbrk == nullptr)
                JUT_ASSERT_fail("d_a_warpf.cpp", 457, "pbrk != (0)");

            brk = new_mDoExt_brkAnm();
            m2BC = brk;
            if (brk == 0 || !mDoExt_brkAnm_init(brk, modelData, pbrk, true, J3DFrameCtrl::EMode_NONE, 1.0f, 0, -1, false, 0))
                return FALSE;
            anm_setRate(m2BC, 0.0f);
        } else {
            modelData = (J3DModelData*)dComIfG_getObjectRes(arcname(), dRes_INDEX_GTFGLOW_BDL_GDEMO29_A00_e, gc_tu.safeString);
            if (modelData == nullptr)
                JUT_ASSERT_fail("d_a_warpf.cpp", 474, "modelData != (0)");

            m2A8 = mDoExt_J3DModel__create(modelData, 0x80000, 0x11000022);
            if (!m2A8)
                return FALSE;

            pbrk = dComIfG_getObjectRes(arcname(), dRes_INDEX_GTFGLOW_BRK_GDEMO29_A01_e, gc_tu.safeString);
            if (pbrk == nullptr)
                JUT_ASSERT_fail("d_a_warpf.cpp", 489, "pbrk != (0)");

            u32 brk = new_mDoExt_brkAnm();
            m2AC = brk;
            if (brk == 0 || !mDoExt_brkAnm_init(brk, modelData, pbrk, true, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, false, 0))
                return FALSE;
            anm_setRate(m2AC, 1.0f);

            modelData = (J3DModelData*)dComIfG_getObjectRes(arcname(), dRes_INDEX_GTFGLOW_BDL_GDEMO29_B00_e, gc_tu.safeString);
            if (modelData == nullptr)
                JUT_ASSERT_fail("d_a_warpf.cpp", 504, "modelData != (0)");

            m2C0 = mDoExt_J3DModel__create(modelData, 0x80000, 0x11000022);
            if (!m2C0)
                return FALSE;

            m2B8 = 0;
            m2BC = 0;
        }
        break;
    }
    return TRUE;
}

/* 024D717C */
bool daWarpf_c::_delete() {
    if (m290 != 0) {
        JPABaseEmitter_becomeInvalidEmitter(m290);
        m290 = 0;
    }
    if (m294 != 0) {
        JPABaseEmitter_becomeInvalidEmitter(m294);
        m294 = 0;
    }
    if (m298 != 0) {
        JPABaseEmitter_becomeInvalidEmitter(m298);
        m298 = 0;
    }
    if (m29C != 0) {
        JPABaseEmitter_becomeInvalidEmitter(m29C);
        m29C = 0;
    }

    if (gabi::ea(arcname()) != 0) {
        dComIfG_resDelete(&mPhase, arcname());
    }
    return true;
}

/* 024D602C */
BOOL daWarpf_c::checkEndDemo() {
    BOOL ret = 0;

    switch ((u32)(s32)mStageNo) {
    case STAGE_DRC:
        if (checkItemGet(dItemNo_PEARL_DIN_e, TRUE))
            ret = TRUE;
        break;
    case STAGE_FW:
        if (checkItemGet(dItemNo_PEARL_FARORE_e, TRUE))
            ret = TRUE;
        break;
    case STAGE_TOTG:
        if (dComIfGs_isEventBit(0x2D10))
            ret = TRUE;
        break;
    case STAGE_ET:
        if (checkItemGet(dItemNo_MASTER_SWORD_2_e, TRUE))
            ret = TRUE;
        break;
    case STAGE_WT:
        if (dComIfGs_checkGetItem(dItemNo_MASTER_SWORD_3_e))
            ret = TRUE;
        break;
    }
    return ret;
}

/* 024D6EEC */
cPhs_State daWarpf_c::CreateInit() {
    current.pos.y = get_earth_pos();
    if (!dComIfGs_isStageBossEnemy()) {
        return cPhs_ERROR_e;
    }

    set_effect();

    if (!checkEndDemo()) {
        m2D6 = dComIfGp_evmng_getEventIdx("WARP_WIND" /* "WARP_WIND" */, 0xFF);
    } else {
        m2D6 = dComIfGp_evmng_getEventIdx("WARP_WIND_AFTER" /* "WARP_WIND_AFTER" */, 0xFF);
        setEndAnim();
    }
    return cPhs_COMPLEATE_e;
}

/* 024D6FB8 */
cPhs_State daWarpf_c::_create() {
    // HD: fopAc_ac_c has a vtable; retain the explicit constructor sequence.
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (gabi::ea(this) != 0) {
            fopAc_ac_c_ct(this);
            __vtbl = warpf_tu.mActorVtable;
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }

    mStageNo = Warpf_getSaveTable(dComIfGp_getStageStagInfo());
    // HD: JUT_ASSERT(0x2CD, mSceneNo >= 0 && mSceneNo < 16)

    if ((u32)(s32)mStageNo >= 16)
        JUT_ASSERT_fail("d_a_warpf.cpp", 0x2CD, "mSceneNo >= 0 && mSceneNo < 16");
    cPhs_State phase = cPhs_COMPLEATE_e;

    if (gabi::ea(arcname()) != 0) {
        phase = dComIfG_resLoad(&mPhase, arcname());
        if (phase == cPhs_COMPLEATE_e) {
            if (!fopAcM_entrySolidHeap(this, warpf_tu.mHeapCallback /* CheckCreateHeap */, 0x5000)) {
                return cPhs_ERROR_e;
            }
        }
    }

    if (phase == cPhs_COMPLEATE_e) {
        if (CreateInit() == cPhs_ERROR_e) {
            return cPhs_ERROR_e;
        }

        u32 seNum = JA_SE_OBJ_BOSS_WARP_LV;
        if (mStageNo == STAGE_TOTG) {
            seNum = JA_SE_OBJ_WARP_EFF_SUS;
        } else if (mStageNo == STAGE_ET || mStageNo == STAGE_WT) {
            seNum = JA_SE_OBJ_BOSS_TF_WARP_SUS;
        }

        u32 se = fopKyM_fastCreate(fpcNm_LEVEL_SE_e, seNum, &current.pos, nullptr, nullptr);
        m2E8 = se;
        if (se != 0) {
            dLevelSe_seStop(se);
            /* setReverb(0, reverb) */
            s8 roomNo = fopAcM_GetRoomNo(this);
            u32 p = m2E8; /* reloaded before the reverb lookup */
            s32 reverb = dComIfGp_getReverb(roomNo);
            Warpf_view<dLevelSe_c>(p)->mReverb = s8(reverb);
            Warpf_view<dLevelSe_c>(p)->mSoundParam = 0;
            Warpf_view<dLevelSe_c>(p)->mFlag = u8(Warpf_view<dLevelSe_c>(p)->mFlag) | dLevelSe_c::CUSTOM_REVERB;
        }
    }
    return phase;
}

/* 024D7B0C */
bool daWarpf_c::_execute() {
    m2E4 = m2E4 + 1;
    checkOrder();
    demo_proc();
    set_se();
    eventOrder();
    anim_play();
    return true;
}

/* 024D79F4 */
void daWarpf_c::eventOrder() {
    if (m2D0 == 1) {
        fopAcM_orderOtherEventId(this, m2D6, 0xFF, 0xFFFF, 0, 1);
        eventInfo.onCondition(dEvtCnd_UNK2_e);
    }
}

/* 024D7628 */
void daWarpf_c::checkOrder() {
    if (eventInfo.checkCommandDemoAccrpt()) {
        if (dComIfGp_evmng_startCheck(m2D6) && m2D0 != 0) {
            m2D0 = 0;
        }

        if (dComIfGp_evmng_endCheck(m2D6)) {
            dComIfGp_event_reset();
            fopAcM_delete(this);
        }
    } else if (m2D0 == 0) {
        if (check_warp_event()) {
            m2D0 = 1;
        }
        set_effect();
    }
}

/* 024D770C */
void daWarpf_c::demo_proc() {
    mStaffID = dComIfGp_evmng_getMyStaffId("Warpf" /* "Warpf" */, nullptr, 0);
    if (dComIfGp_event_runCheck() && !eventInfo.checkCommandTalk() && mStaffID != -1) {
        s32 actIdx = dComIfGp_evmng_getMyActIdx(mStaffID, reinterpret_cast<gptr<char>*>(&action_table), 6, 0, 0);

        if (actIdx == -1) {
            dComIfGp_evmng_cutEnd(mStaffID);
        } else {
            if (dComIfGp_evmng_getIsAddvance(mStaffID)) {
                (this->*event_init_tbl[u32(actIdx)].mFunction)(mStaffID);
            }

            BOOL ret = (this->*event_action_tbl[u32(actIdx)].mFunction)(mStaffID);
            if (ret) {
                dComIfGp_evmng_cutEnd(mStaffID);
            }
        }
    }
}

/* 024D7BCC */
void daWarpf_c::initWait(int staffIdx) {
    if (m294 != 0) {
        JPABaseEmitter_becomeInvalidEmitter(m294);
        m294 = 0;
    }
    if (m298 != 0) {
        JPABaseEmitter_becomeInvalidEmitter(m298);
        m298 = 0;
    }
    if (m29C != 0) {
        JPABaseEmitter_becomeInvalidEmitter(m29C);
        m29C = 0;
    }

    gabi::Local<cXyz> position;
    f32 x = current.pos.x;
    f32 y = current.pos.y;
    f32 z = current.pos.z;
    position->x = x;
    position->z = z;
    position->y = y + 1000.0f;

    position->y = dBgS_ObjGndChk_Func(position.get());
    dComIfGp_evmng_setGoal(position.get());
}

/* 024D7C98 */
BOOL daWarpf_c::actWait(int staffIdx) {
    if (m2E4 % 10 == 0) {
        set_effect_wind00();
    }
    return TRUE;
}

/* 024D7CE4 */
void daWarpf_c::initWarpStart(int staffIdx) {
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    m2EC.copy(player->current.pos);
}

/* 024D8130 */
BOOL daWarpf_c::actWarpStart(int staffIdx) {
    return TRUE;
}

/* 024D7D2C */
void daWarpf_c::initWarpMode_1(int staffIdx) {
    mWarpTimer = 180;
}

// HD: csXyz uses r3:r4 (x:y, z:padding) in the GHS small-struct return convention.
struct angle_ret {
    be<u32> w0;
    be<u32> w1;
};
static inline csXyz* Warpf_angle(angle_ret* a) {return reinterpret_cast<csXyz*>(a);}
static inline void store_angle(angle_ret* a, u32 r3, u32 r4) {
    a->w0 = r3;
    a->w1 = r4;
}

/* 024D7D38 */
BOOL daWarpf_c::actWarpMode_1(int staffIdx) {
    if (cLib_calcTimer(&mWarpTimer) == 0) {
        if (mStageNo != STAGE_TOTG && mStageNo != STAGE_ET && mStageNo != STAGE_WT) {
            gabi::Local<cXyz> position;
            gabi::Local<angle_ret> windAngle;
            f32 x = current.pos.x;
            f32 y = current.pos.y;
            position->x = x;
            f32 z = current.pos.z;
            position->y = y;
            position->z = z;
            u32 r3 = get_angle_wind01();
            u32 r4 = gabi::cpu->r[4];
            f32 ey = m2EC.y;
            store_angle(windAngle.get(), r3, r4);
            position->y = ey + 100.0f;

            dComIfGp_particle_set(ID_AK_SN_BOSSWARPWIND01_820B, position.get(), Warpf_angle(windAngle.get()));
        }
        return TRUE;
    }

    if (mWarpTimer == 80) {
        for (s32 i = 0; i < 6; i++) {
            set_effect_wind00();
        }
    }

    set_effect();
    return FALSE;
}

/* 024D7E58 */
void daWarpf_c::initWarpMode_2(int staffIdx) {
    mWarpTimer = 5;
}

/* 024D7E64 */
BOOL daWarpf_c::actWarpMode_2(int staffIdx) {
    if (cLib_calcTimer(&mWarpTimer) == 0) {
        if (mStageNo != STAGE_TOTG && mStageNo != STAGE_ET && mStageNo != STAGE_WT) {
            gabi::Local<cXyz> position;
            gabi::Local<angle_ret> windAngle;
            f32 x = current.pos.x;
            f32 y = current.pos.y;
            f32 z = current.pos.z;
            position->y = y;
            position->z = z;
            position->x = x;
            u32 r3 = get_angle_wind01();
            u32 r4 = gabi::cpu->r[4];
            f32 ey = m2EC.y;
            store_angle(windAngle.get(), r3, r4);
            position->y = ey + 120.0f;
            Warpf_angle(windAngle.get())->z = s16(Warpf_angle(windAngle.get())->z) + 0x38E;

            dComIfGp_particle_set(ID_AK_SN_BOSSWARPWIND01_820B, position.get(), Warpf_angle(windAngle.get()));
        }
        return TRUE;
    }
    return FALSE;
}

/* 024D7F5C */
void daWarpf_c::initWarpMode_3(int staffIdx) {
    mWarpTimer = 5;
}

/* 024D7F68 */
BOOL daWarpf_c::actWarpMode_3(int staffIdx) {
    if (cLib_calcTimer(&mWarpTimer) == 0) {
        if (mStageNo != STAGE_TOTG && mStageNo != STAGE_ET && mStageNo != STAGE_WT) {
            gabi::Local<cXyz> position;
            gabi::Local<angle_ret> windAngle;
            f32 x = current.pos.x;
            f32 y = current.pos.y;
            position->x = x;
            f32 z = current.pos.z;
            position->y = y;
            position->z = z;
            u32 r3 = get_angle_wind01();
            u32 r4 = gabi::cpu->r[4];
            f32 ey = m2EC.y;
            store_angle(windAngle.get(), r3, r4);
            position->y = ey + 140.0f;
            Warpf_angle(windAngle.get())->y = s16(Warpf_angle(windAngle.get())->y) + 0x71C;

            dComIfGp_particle_set(ID_AK_SN_BOSSWARPWIND01_820B, position.get(), Warpf_angle(windAngle.get()));
        }
        mDoAud_seStart(JA_SE_LK_BOSS_WARP_EFF_ST, nullptr, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(this)));
        onEndDemo();
        return TRUE;
    }
    return FALSE;
}

/* 024D8138 */
void daWarpf_c::initEndWait(int staffIdx) {
}

/* 024D813C */
BOOL daWarpf_c::actEndWait(int staffIdx) {
    return TRUE;
}

/* 024D7544 */
BOOL daWarpf_c::check_warp_event() {
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    f32 distance = absXZ_diff(&player->current.pos, &current.pos);

    if (daPy_checkPlayerFly(player)) {
        return FALSE;
    }

    if (distance < get_distance())
        return TRUE;
    return FALSE;
}

/* 024D6804 */
f32 daWarpf_c::get_distance() {
    if (mStageNo == STAGE_ET || mStageNo == STAGE_WT) {
        if (checkEndDemo())
            return 190.0f;
    }
    return f32(m_warp_size[u32(s32(mStageNo))]);
}

/* 024D6794 */
f32 daWarpf_c::get_earth_pos() {
    BOOL end = checkEndDemo();
    s32 stageNo = mStageNo;
    if (!end) {
        f32 earthPos = f32(l_earth_pos[u32(stageNo)]);
        if (stageNo == STAGE_FW && daWarpf_prm::getSetType(this)) {
            earthPos = 0.0f;
        }
        return earthPos;
    }
    return f32(l_earth_pos_2nd[u32(stageNo)]);
}

/* 024D6940 */
void daWarpf_c::set_effect() {
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    f32 effectRange = 2000.0f;
    s32 effectInterval = 20;
    f32 distance = absXZ_diff(&player->current.pos, &current.pos);

    if (distance < effectRange && distance > get_distance()) {
        f32 tmp = distance - get_distance();
        // HD: keep the fused multiply-add and integer truncation.
        effectInterval = gabi::ftoi(gabi::fmadds(tmp / effectRange, 17.0f, 3.0f));
    }

    switch ((u32)(s32)mStageNo) {
    case STAGE_TOTG:
        if (m290 == 0) {
            if (daWarpf_prm::getSetType(this) == 0) {
                dComIfGp_particle_set(ID_AK_SN_BSTWARP00, &current.pos);
                dComIfGp_particle_set(ID_AK_SN_BSTWARP00 + 1, &current.pos);
                dComIfGp_particle_set(ID_AK_SN_BSTWARP00 + 2, &current.pos);
            }
            m290 = gabi::ea(dComIfGp_particle_set(ID_AK_SN_BSTWARP00 + 3, &current.pos));
        }
        break;

    case STAGE_ET:
    case STAGE_WT:
        if (m298 == 0) {
            if (!checkEndDemo()) {
                if (daWarpf_prm::getSetType(this) && m2B8 != 0) {
                    anm_setRate(m2B8, 1.0f);
                }
                m298 = gabi::ea(dComIfGp_particle_set(ID_IT_SN_SHINDENTF_LIGHT00, &current.pos, &current.angle));
                m29C = gabi::ea(dComIfGp_particle_set(ID_IT_SN_SHINDENTF_STAR00, &current.pos, &current.angle));
            } else if (m29C == 0) {
                m29C = gabi::ea(dComIfGp_particle_set(ID_IT_SN_DEMO29_STAR00, &current.pos, &current.angle));
            }
        }
        break;

    default: {
        u32 t = (u32)(s32)m2E4;
        if (t - ppc_divw(t, (u32)effectInterval) * (u32)effectInterval == 0) {
            set_effect_wind00();
        }
        if (m294 == 0) {
            m294 = gabi::ea(dComIfGp_particle_set(ID_AK_SN_BOSSWARPKIRAKIRA00_820C, &current.pos));
        }
        break;
    }
    }
}

/* 024D688C */
void daWarpf_c::set_effect_wind00() {
    if (mStageNo == STAGE_TOTG || mStageNo == STAGE_WT || mStageNo == STAGE_ET) {
        return;
    }

    u32 emitter = gabi::ea(dComIfGp_particle_set(ID_AK_SN_BOSSWARPWIND00_820A, &current.pos));
    if (emitter != 0) {
        JPABaseEmitter_setMaxFrame(emitter, 1);
        JPABaseEmitter_setGlobalAlpha(emitter, (u8)gabi::ftoi(cM_rndF(127.9f) + 128.0f));
    }
}

/* 024D7B6C */
u32 daWarpf_c::get_angle_wind01() {
    s16 x = dCam_getAngleX(gabi::ea(dComIfGp_getCamera(0)));
    s16 y = dCam_getAngleY(gabi::ea(dComIfGp_getCamera(0))) + 0x8000;
    gabi::cpu->r[4] = 0; /* z = 0, padding (not compared: only r3 is) */
    return ((u32)(u16)x << 16) | (u16)y;
}

/* 024D7A54 */
void daWarpf_c::anim_play() {
    if (mStageNo == STAGE_TOTG) {
        if (m2AC != 0) {
            mDoExt_baseAnm_play(Warpf_view<void>(m2AC));
        }
        if (m2B0 != 0) {
            mDoExt_baseAnm_play(Warpf_view<void>(m2B0));
            /* GameCube bug kept: tests m2B0 again before playing m2B4 */
            if (m2B0 != 0) {
                mDoExt_baseAnm_play(Warpf_view<void>(m2B4));
            }
        }
    } else if (mStageNo == STAGE_ET || mStageNo == STAGE_WT) {
        if (m2AC != 0) {
            mDoExt_baseAnm_play(Warpf_view<void>(m2AC));
        }
        if (m2B8 != 0) {
            if (m2E4 >= 120) {
                anm_setRate(m2B8, 1.0f);
            }
            mDoExt_baseAnm_play(Warpf_view<void>(m2B8));
        }
    }
}

/* 024D6D94 */
void daWarpf_c::setEndAnim() {
    switch ((u32)(s32)mStageNo) {
    case STAGE_TOTG:
        if (m2AC != 0) {
            anm_setFrame(m2AC, anm_getEndFrame(m2AC));
        }
        if (m2B0 != 0) {
            anm_setFrame(m2B0, anm_getEndFrame(m2B0));
        }
        if (m2B4 != 0) {
            anm_setFrame(m2B4, anm_getEndFrame(m2B4));
        }
        break;

    case STAGE_ET:
    case STAGE_WT:
        if (!checkEndDemo() && m2AC != 0) {
            anm_setFrame(m2AC, anm_getEndFrame(m2AC));
        }
        break;
    }
}

/* 024D78D0 */
void daWarpf_c::set_se() {
    s32 t = m2E0 + 1;
    if (t > 20000) {
        t = 20000;
    }
    m2E0 = t;

    switch ((u32)(s32)m2D8) {
    case 0:
        if (daWarpf_prm::getSetType(this) == 0) {
            if (mStageNo == STAGE_TOTG) {
                seStart_eye(this, JA_SE_CM_BST_WARP_BEAM, 0);
            } else if (mStageNo != STAGE_WT && mStageNo != STAGE_ET) {
                seStart_eye(this, JA_SE_OBJ_BOSS_WARP_APPEAR, 0);
            }
        }
        m2D8 = 1;
        break;

    case 1:
        break;
    }

    if (mStageNo == STAGE_TOTG) {
        if (m2E0 >= 45 && m2E8 != 0) {
            dLevelSe_seStart(m2E8);
        }
    } else if (m2E8 != 0) {
        dLevelSe_seStart(m2E8);
    }
}

/* 024D7258 */
void daWarpf_c::set_mtx() {
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    if (m2A8) {
        J3DModel_setBaseScale(m2A8, &scale);
        J3DModel_setBaseTRMtx(m2A8, mDoMtx_stack_c::get());
    }

    if (m2C0) {
        J3DModel_setBaseScale(m2C0, &scale);
        J3DModel_setBaseTRMtx(m2C0, mDoMtx_stack_c::get());
    }
}

/* 024D73B4 */
bool daWarpf_c::_draw() {
    switch ((u32)(s32)mStageNo) {
    case STAGE_TOTG:
        set_mtx();
        settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
        setLightTevColorType(dKy_getEnvlight(), m2A8, &tevStr);

        if (m2AC != 0) {
            mDoExt_brkAnm_entry(Warpf_view<mDoExt_brkAnm>(m2AC), J3DModel_getModelData(m2A8), anm_getFrame(m2AC));
        }
        if (m2B0 != 0) {
            Warpf_view<mDoExt_bckAnm>(m2B0)->entry(J3DModel_getModelData(m2A8), anm_getFrame(m2B0));
        }
        if (m2B4 != 0) {
            mDoExt_btkAnm_entry(Warpf_view<mDoExt_btkAnm>(m2B4), J3DModel_getModelData(m2A8), anm_getFrame(m2B4));
        }

        mDoExt_modelUpdateDL(m2A8);
        break;

    case STAGE_ET:
    case STAGE_WT:
        set_mtx();
        settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);

        if (m2A8) {
            setLightTevColorType(dKy_getEnvlight(), m2A8, &tevStr);

            // HD: the brk entries take the model (025E8480)

            if (m2AC != 0) {
                mDoExt_brkAnm_entry_model(m2AC, m2A8, anm_getFrame(m2AC));
            }
            if (m2B8 != 0 && anm_getRate(m2B8) > 0.0f) {
                mDoExt_brkAnm_entry_model(m2B8, m2A8, anm_getFrame(m2B8));
            }

            mDoExt_modelUpdateDL(m2A8);
        }

        if (m2C0) {
            setLightTevColorType(dKy_getEnvlight(), m2C0, &tevStr);
            mDoExt_modelUpdateDL(m2C0);
        }
        break;
    }
    return true;
}

/* 024D7178 */
static cPhs_State daWarpf_Create(void* i_this) {
    return ((daWarpf_c*)i_this)->_create();
}

/* 024D7254 */
static BOOL daWarpf_Delete(void* i_this) {
    return ((daWarpf_c*)i_this)->_delete();
}

/* 024D7540 */
static BOOL daWarpf_Draw(void* i_this) {
    return ((daWarpf_c*)i_this)->_draw();
}

/* 024D7B68 */
static BOOL daWarpf_Execute(void* i_this) {
    return ((daWarpf_c*)i_this)->_execute();
}

/* 024D8128 */
static BOOL daWarpf_IsDelete(void*) {
    return TRUE;
}

/* 024D8144 */
static void daWarpf_c_dtor(daWarpf_c* p, s32 flags) {
    if (p != nullptr) {
        Warpf_actorDestruct(p, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(p);
    }
}

/* 024D8114 */
static void SafeString_dtor(SafeString* p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

/* 024D8080 */
static void sinit_d_a_warpf() {
    sinit_header_statics(warpf_tu.mHeaderStorage, warpf_tu.mHeaderRecords);
}

/* WWHD glue
data m_arcname 101D2F04
data m_warp_size 10042430
data l_earth_pos 100422E4
data l_earth_pos_2nd 10042324
data action_table 101D2E98
data event_init_tbl 101D2E18
data event_action_tbl 101D2E48
str 100422C0 "d_a_warpf.cpp" in 024D6114
str 100422D0 "modelData != (0)"
str 1004229C "pbrk != (0)"
str 100422A8 "pbck != (0)"
str 100422B4 "pbtk != (0)"
str 10042398 "WARP_WIND"
str 10042388 "WARP_WIND_AFTER"
str 100423A4 "d_a_warpf.cpp" in 024D6FB8
str 100423B4 "mSceneNo >= 0 && mSceneNo < 16"
str 100423D4 "Warpf"
ext 025DAE00 u32 Warpf_fastCreate(s16,u32,cXyz*,cXyz*,void*)
ext 025E80D0 u32 Warpf_brkConstruct(u32)
ext 025E8154 BOOL Warpf_brkInit(u32,J3DModelData*,void*,bool,s32,f32,s16,s16,bool,s32)
ext 025E8480 void Warpf_brkEntryModel(u32,J3DModel*,f32)
ext 025E7C6C u32 Warpf_btkConstruct(u32)
ext 027F2BC0 void Warpf_frameConstruct(u32,s32)
ext 027DA984 void Warpf_matrixConstruct(void*)
ext 0211D2F8 s32 Warpf_calcTimer(be<s32>*)
ext 024F1350 f32 Warpf_groundCheck(cXyz*)
ext 024F8008 s16 Warpf_cameraAngleX(u32)
ext 024F8000 s16 Warpf_cameraAngleY(u32)
ext 025D50BC void Warpf_actorDestruct(daWarpf_c*,s32)
*/
