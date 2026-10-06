/**
 * d_a_warpf.cpp (WWHD)
 * Boss warp flower / blue warp light after a boss (and the Tower of the Gods / Earth and Wind
 * Temple variants).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_warpf.cpp) to the WWHD layout and verified against cking.rpx.
 * "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define FILE_NAME STR(0x100422C0)        /* "d_a_warpf.cpp" (CreateHeap asserts) */
#define MSG_MODELDATA STR(0x100422D0)    /* "modelData != (0)" */
#define MSG_PBRK STR(0x1004229C)         /* "pbrk != (0)" */
#define MSG_PBCK STR(0x100422A8)         /* "pbck != (0)" */
#define MSG_PBTK STR(0x100422B4)         /* "pbtk != (0)" */
#define SAFESTRING_VTBL 0x10042244       /* this TU's sead::SafeString vtable */
#define BCK_VTBL 0x1004225C              /* this TU's mDoExt_bckAnm vtable */
#define ACT_VTBL 0x10042284              /* HD: daWarpf_c vtable (deleting destructor 024D8144) */

/* static const char* m_arcname[16] (.data) / static const f32 m_warp_size[16] (.rodata) */
#define M_ARCNAME 0x101D2F04
#define M_WARP_SIZE 0x10042430
/* get_earth_pos: l_earth_pos / l_earth_pos_2nd (.rodata) */
#define L_EARTH_POS 0x100422E4
#define L_EARTH_POS_2ND 0x10042324
/* demo_proc: event_init_tbl / event_action_tbl (pointers to member functions, 8 bytes each),
 * action_table (6 names) */
#define EVENT_INIT_TBL 0x101D2E18
#define EVENT_ACTION_TBL 0x101D2E48
#define ACTION_TABLE 0x101D2E98

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

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 0254DA50 checkItemGet(u8 item, BOOL) */
static inline BOOL checkItemGet(u8 item, BOOL p) { return gabi::call<BOOL>(0x0254DA50, item, p); }
/* 02520C0C dComIfGs_checkGetItem(u8) */
static inline BOOL dComIfGs_checkGetItem(u8 item) { return gabi::call<BOOL>(0x02520C0C, item); }
/* dComIfGs_isEventBit: dSv_event_c at *(0x101F84DC) + 0x644 */
static inline BOOL dComIfGs_isEventBit(u16 flag) {
    return dSv_event_isEventBit(gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x644), flag);
}
/* dComIfGs_isStageBossEnemy(): dSv_memBit_c::isDungeonItem(3), memBit at *(0x101F84DC) + 0x798 */
static inline BOOL dComIfGs_isStageBossEnemy() { return gabi::call<BOOL>(0x025B9100, gabi::load<u32>(0x101F84DC) + 0x798, 3); }
/* dComIfGp_getStageStagInfo(): virtual getStagInfo (vtable +0x15C) of the stage data at play+0x5150 */
static inline u32 dComIfGp_getStageStagInfo() {
    u32 dt = dComIfGp_ea() + PLAY_STAGEDATA;
    return gabi::call_ptr<u32>(gabi::load<u32>(gabi::load<u32>(dt) + 0x15C), dt);
}
/* dStage_stagInfo_GetSaveTbl: bits 1..7 of byte +9 */
static inline u32 dStage_stagInfo_GetSaveTbl(u32 info) { return (gabi::load<u8>(info + 9) >> 1) & 0x7F; }
/* 025DAE00 fopKyM_fastCreate(s16 name, u32 param, cXyz* pos, cXyz* scale, void* cb) */
static inline u32 fopKyM_fastCreate(s16 name, u32 param, cXyz* pos, cXyz* scale, void* cb) {
    return gabi::call<u32>(0x025DAE00, name, param, pos, scale, cb);
}
enum { fpcNm_LEVEL_SE_e = 0x18 };
/* dLevelSe_c (HD): flag byte +0x101 (bit 8: stopped, bit 4: reverb set), reverb +0x100, +0xFC */
static inline void dLevelSe_seStop(u32 se) { gabi::store<u8>(se + 0x101, (u8)(gabi::load<u8>(se + 0x101) | 8)); }
static inline void dLevelSe_seStart(u32 se) { gabi::store<u8>(se + 0x101, (u8)(gabi::load<u8>(se + 0x101) & 0xF7)); }
/* JPABaseEmitter (HD): becomeInvalidEmitter = stopCreateParticle (flags +0x254 |= 1) + maxFrame (+0x5C) = -1 */
static inline void JPABaseEmitter_becomeInvalidEmitter(u32 e) {
    u32 f = gabi::load<u32>(e + 0x254);
    gabi::store<u32>(e + 0x5C, 0xFFFFFFFFu);
    gabi::store<u32>(e + 0x254, f | 1);
}
static inline void JPABaseEmitter_setMaxFrame(u32 e, s32 n) { gabi::store<s32>(e + 0x5C, n); }
static inline void JPABaseEmitter_setGlobalAlpha(u32 e, u8 a) { gabi::store<u8>(e + 0x247, a); }
/* mDoExt_brkAnm (HD): constructor 025E80D0 allocates when this == NULL (the matcher calls it init);
 * init 025E8154 (this, data, brk, bool anmPlay, mode, f32 speed, s16 start, s16 end, bool, int (stack));
 * the frame control (rate +0, frame +4, end +0xA) is first */
static inline u32 new_mDoExt_brkAnm() { return gabi::call<u32>(0x025E80D0, (u32)0); }
static inline BOOL mDoExt_brkAnm_init(u32 a, J3DModelData* d, void* brk, bool play, s32 mode, f32 speed, s16 start, s16 end, bool b, s32 i) {
    return gabi::call<BOOL>(0x025E8154, a, d, brk, play, mode, speed, start, end, b, i);
}
/* 025E83FC brkAnm::entry(J3DModelData*, f32); HD: 025E8480 brkAnm::entry(J3DModel*, f32) */
static inline void mDoExt_brkAnm_entry_model(u32 a, J3DModel* m, f32 frame) { gabi::call(0x025E8480, a, m, frame); }
/* mDoExt_btkAnm (HD 0x74): constructor 025E7C6C allocates when this == NULL */
static inline u32 new_mDoExt_btkAnm() { return gabi::call<u32>(0x025E7C6C, (u32)0); }
/* mDoExt_bckAnm inline constructor (HD 0x8C, as d_a_arrow_iceeff) */
static inline void mDoExt_bckAnm_ct(u32 p, u32 tu_vtbl) {
    gabi::call(0x027F2BC0, p, 0); /* J3DFrameCtrl::init */
    gabi::store<u32>(p + 0x10, 0x1016E54C);
    gabi::call(0x027DA984, p + 0x14);
    gabi::store<u32>(p + 0x80, 0);
    gabi::store<u32>(p + 0x58, 0);
    gabi::store<u32>(p + 0x48, 0x1016D820);
    gabi::store<u32>(p + 0x84, 0);
    gabi::store<u32>(p + 0x10, tu_vtbl);
    gabi::store<u32>(p + 0x7C, 0);
    gabi::store<u32>(p + 0x88, 0);
}
static inline f32 anm_getRate(u32 a) { return gabi::load<f32>(a + 0); }
static inline void anm_setRate(u32 a, f32 r) { gabi::store<f32>(a + 0, r); }
static inline f32 anm_getFrame(u32 a) { return gabi::load<f32>(a + 4); }
static inline void anm_setFrame(u32 a, f32 f) { gabi::store<f32>(a + 4, f); }
static inline f32 anm_getEndFrame(u32 a) { return (f32)gabi::load<s16>(a + 0xA); }
/* 0211D2F8 cLib_calcTimer<int> */
static inline s32 cLib_calcTimer(be<s32>* t) { return gabi::call<s32>(0x0211D2F8, t); }
/* 024F1350 dBgS_ObjGndChk_Func(cXyz&) */
static inline f32 dBgS_ObjGndChk_Func(cXyz* pos) { return gabi::call<f32>(0x024F1350, pos); }
/* 02543714 dEvent_manager_c::setGoal(cXyz*) */
static inline void dComIfGp_evmng_setGoal(cXyz* pos) { gabi::call(0x02543714, dComIfGp_getPEvtManager(), pos); }
/* 02542EDC dEvent_manager_c::getMyActIdx(staffId, names, n, force, int) */
static inline s32 dComIfGp_evmng_getMyActIdx(s32 staff, u32 names, s32 n, s32 force, s32 p) {
    return gabi::call<s32>(0x02542EDC, dComIfGp_getPEvtManager(), staff, names, n, force, p);
}
/* dComIfGp_event_runCheck(): u8 at play+0x5292 */
static inline bool dComIfGp_event_runCheck() { return gabi::load<u8>(dComIfGp_ea() + 0x5292) != 0; }
/* dComIfGp_getCamera(0): play+0x5AF8; dCam_getAngleX 024F8008, dCam_getAngleY 024F8000 */
static inline u32 dComIfGp_getCamera0() { return gabi::load<u32>(dComIfGp_ea() + 0x5AF8); }
static inline s16 dCam_getAngleX(u32 cam) { return gabi::call<s16>(0x024F8008, cam); }
static inline s16 dCam_getAngleY(u32 cam) { return gabi::call<s16>(0x024F8000, cam); }
/* daPy_py_c::checkPlayerFly(): virtual, vtable (+0xB4) slot +0x4C */
static inline BOOL daPy_checkPlayerFly(fopAc_ac_c* p) {
    return gabi::call_ptr<BOOL>(gabi::load<u32>(gabi::load<u32>(gabi::ea(p) + 0xB4) + 0x4C), p);
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
static inline u32 ptmf_call_i(u32 entry, void* self, s32 arg_ea_src) {
    s16 delta = gabi::load<s16>(entry);
    s16 idx = gabi::load<s16>(entry + 2);
    void* p = gabi::at<void>(gabi::ea(self) + delta);
    if (idx < 0)
        return gabi::call_ptr<u32>(gabi::load<u32>(entry + 4), p, arg_ea_src);
    u32 vt = gabi::load<u32>(gabi::ea(p) + gabi::load<s16>(entry + 6));
    return gabi::call_ptr<u32>(gabi::load<u32>(vt + idx * 8 + 4), p, arg_ea_src);
}
/* HD: fopAcM_seStart without the actor/eyePos null checks */
static inline void seStart_eye(fopAc_ac_c* a, u32 id, u32 param) {
    s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(a));
    mDoAud_seStart(id, &a->eyePos, param, reverb);
}

struct daWarpf_c : fopAc_ac_c {
    BOOL CreateHeap();
    bool _delete();
    BOOL checkEndDemo();
    cPhs_State CreateInit();
    cPhs_State _create();
    bool _execute();
    void eventOrder();
    void checkOrder();
    void demo_proc();
    void initWait(int);
    BOOL actWait(int);
    void initWarpStart(int);
    BOOL actWarpStart(int);
    void initWarpMode_1(int);
    BOOL actWarpMode_1(int);
    void initWarpMode_2(int);
    BOOL actWarpMode_2(int);
    void initWarpMode_3(int);
    BOOL actWarpMode_3(int);
    void initEndWait(int);
    BOOL actEndWait(int);
    BOOL check_warp_event();
    f32 get_distance();
    f32 get_earth_pos();
    void set_effect();
    void set_effect_wind00();
    u32 get_angle_wind01();
    void anim_play();
    void setEndAnim();
    void set_se();
    void set_mtx();
    bool _draw();

    const char* arcname() { return gabi::at<const char>(gabi::load<u32>(M_ARCNAME + 4 * mStageNo)); }
    u32 getSetType() { return fopAcM_GetParam(this) >> 0x1C; }

    /* 0x3AC */ be<u32> m290;  /* JPABaseEmitter* */
    /* 0x3B0 */ be<u32> m294;
    /* 0x3B4 */ be<u32> m298;
    /* 0x3B8 */ be<u32> m29C;
    /* 0x3BC */ request_of_phase_process_class mPhase;
    /* 0x3C4 */ gptr<J3DModel> m2A8;
    /* 0x3C8 */ be<u32> m2AC;  /* mDoExt_brkAnm* */
    /* 0x3CC */ be<u32> m2B0;  /* mDoExt_bckAnm* */
    /* 0x3D0 */ be<u32> m2B4;  /* mDoExt_btkAnm* */
    /* 0x3D4 */ be<u32> m2B8;  /* mDoExt_brkAnm* */
    /* 0x3D8 */ be<u32> m2BC;  /* mDoExt_brkAnm* */
    /* 0x3DC */ gptr<J3DModel> m2C0;
    /* 0x3E0 */ be<s32> mStaffID;
    /* 0x3E4 */ be<s32> mWarpTimer;
    /* 0x3E8 */ u8 m2CC[4];
    /* 0x3EC */ be<s32> m2D0;
    /* 0x3F0 */ u8 m2D4[2];
    /* 0x3F2 */ be<s16> m2D6;
    /* 0x3F4 */ be<s32> m2D8;
    /* 0x3F8 */ be<s32> mStageNo;
    /* 0x3FC */ be<s32> m2E0;
    /* 0x400 */ be<s32> m2E4;
    /* 0x404 */ be<u32> m2E8;  /* dLevelSe_c* */
    /* 0x408 */ cXyz m2EC;
};
WWHD_OFFSET(daWarpf_c, mPhase, 0x3BC);
WWHD_OFFSET(daWarpf_c, mStaffID, 0x3E0);
WWHD_OFFSET(daWarpf_c, m2D6, 0x3F2);
WWHD_OFFSET(daWarpf_c, mStageNo, 0x3F8);
WWHD_OFFSET(daWarpf_c, m2EC, 0x408);
WWHD_SIZE(daWarpf_c, 0x414);

/* 024D6790 */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x024D6790, BOOL, i_this);
    return ((daWarpf_c*)i_this)->CreateHeap();
}
VERIFY(0x024D6790, CheckCreateHeap);

/* 024D6114 */
BOOL daWarpf_c::CreateHeap() {
    WWHD_FUNC(0x024D6114, BOOL, this);
    J3DModelData* modelData;
    void* pbrk;
    void* pbck;
    void* pbtk;

    switch ((u32)(s32)mStageNo) {
    case STAGE_TOTG: {
        modelData = (J3DModelData*)dComIfG_getObjectRes(arcname(), dRes_INDEX_YSBWP00_BDL_YSBWP00_e, SAFESTRING_VTBL);
        if (modelData == nullptr)
            JUT_ASSERT_fail(FILE_NAME, 343, MSG_MODELDATA);

        m2A8 = mDoExt_J3DModel__create(modelData, 0x80000, 0x11000222);
        if (!m2A8)
            return FALSE;

        pbrk = dComIfG_getObjectRes(arcname(), dRes_INDEX_YSBWP00_BRK_YSBWP00_e, SAFESTRING_VTBL);
        if (pbrk == nullptr)
            JUT_ASSERT_fail(FILE_NAME, 358, MSG_PBRK);

        u32 brk = new_mDoExt_brkAnm();
        m2AC = brk;
        if (brk == 0 || !mDoExt_brkAnm_init(brk, modelData, pbrk, true, J3DFrameCtrl::EMode_NONE, 1.0f, 0, -1, false, 0))
            return FALSE;

        pbck = dComIfG_getObjectRes(arcname(), dRes_INDEX_YSBWP00_BCK_YSBWP00_e, SAFESTRING_VTBL);
        if (pbck == nullptr)
            JUT_ASSERT_fail(FILE_NAME, 374, MSG_PBCK);

        u32 bck = gabi::ea(operator_new(0x8C));
        if (bck != 0)
            mDoExt_bckAnm_ct(bck, BCK_VTBL);
        m2B0 = bck;
        if (bck == 0 ||
            !gabi::at<mDoExt_bckAnm>(bck)->init(modelData, (J3DAnmTransform*)pbck, true, J3DFrameCtrl::EMode_NONE, 1.0f, 0, -1, false))
            return FALSE;

        pbtk = dComIfG_getObjectRes(arcname(), dRes_INDEX_YSBWP00_BTK_YSBWP00_e, SAFESTRING_VTBL);
        if (pbtk == nullptr)
            JUT_ASSERT_fail(FILE_NAME, 391, MSG_PBTK);

        u32 btk = new_mDoExt_btkAnm();
        m2B4 = btk;
        if (btk == 0 || !gabi::at<mDoExt_btkAnm>(btk)->init(modelData, (J3DAnmTextureSRTKey*)pbtk, true,
                                                              J3DFrameCtrl::EMode_NONE, 1.0f, 0, -1, false, 0))
            return FALSE;
        break;
    }

    case STAGE_ET:
    case STAGE_WT:
        if (!checkEndDemo()) {
            m2C0 = nullptr;

            modelData = (J3DModelData*)dComIfG_getObjectRes(arcname(), dRes_INDEX_GTFGLOW_BDL_GTFGLOW00_e, SAFESTRING_VTBL);
            if (modelData == nullptr)
                JUT_ASSERT_fail(FILE_NAME, 411, MSG_MODELDATA);

            m2A8 = mDoExt_J3DModel__create(modelData, 0x80000, 0x11000022);
            if (!m2A8)
                return FALSE;

            pbrk = dComIfG_getObjectRes(arcname(), dRes_INDEX_GTFGLOW_BRK_GTFGLOW00_e, SAFESTRING_VTBL);
            if (pbrk == nullptr)
                JUT_ASSERT_fail(FILE_NAME, 424, MSG_PBRK);

            u32 brk = new_mDoExt_brkAnm();
            m2AC = brk;
            if (brk == 0 || !mDoExt_brkAnm_init(brk, modelData, pbrk, true, J3DFrameCtrl::EMode_NONE, 1.0f, 0, -1, false, 0))
                return FALSE;
            anm_setRate(m2AC, 1.0f);

            pbrk = dComIfG_getObjectRes(arcname(), dRes_INDEX_GTFGLOW_BRK_GTFGLOW01_e, SAFESTRING_VTBL);
            if (pbrk == nullptr)
                JUT_ASSERT_fail(FILE_NAME, 441, MSG_PBRK);

            brk = new_mDoExt_brkAnm();
            m2B8 = brk;
            if (brk == 0 || !mDoExt_brkAnm_init(brk, modelData, pbrk, true, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, false, 0))
                return FALSE;
            anm_setRate(m2B8, 0.0f);

            pbrk = dComIfG_getObjectRes(arcname(), dRes_INDEX_GTFGLOW_BRK_GTFGLOW02_e, SAFESTRING_VTBL);
            if (pbrk == nullptr)
                JUT_ASSERT_fail(FILE_NAME, 457, MSG_PBRK);

            brk = new_mDoExt_brkAnm();
            m2BC = brk;
            if (brk == 0 || !mDoExt_brkAnm_init(brk, modelData, pbrk, true, J3DFrameCtrl::EMode_NONE, 1.0f, 0, -1, false, 0))
                return FALSE;
            anm_setRate(m2BC, 0.0f);
        } else {
            modelData = (J3DModelData*)dComIfG_getObjectRes(arcname(), dRes_INDEX_GTFGLOW_BDL_GDEMO29_A00_e, SAFESTRING_VTBL);
            if (modelData == nullptr)
                JUT_ASSERT_fail(FILE_NAME, 474, MSG_MODELDATA);

            m2A8 = mDoExt_J3DModel__create(modelData, 0x80000, 0x11000022);
            if (!m2A8)
                return FALSE;

            pbrk = dComIfG_getObjectRes(arcname(), dRes_INDEX_GTFGLOW_BRK_GDEMO29_A01_e, SAFESTRING_VTBL);
            if (pbrk == nullptr)
                JUT_ASSERT_fail(FILE_NAME, 489, MSG_PBRK);

            u32 brk = new_mDoExt_brkAnm();
            m2AC = brk;
            if (brk == 0 || !mDoExt_brkAnm_init(brk, modelData, pbrk, true, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, false, 0))
                return FALSE;
            anm_setRate(m2AC, 1.0f);

            modelData = (J3DModelData*)dComIfG_getObjectRes(arcname(), dRes_INDEX_GTFGLOW_BDL_GDEMO29_B00_e, SAFESTRING_VTBL);
            if (modelData == nullptr)
                JUT_ASSERT_fail(FILE_NAME, 504, MSG_MODELDATA);

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
VERIFY(0x024D6114, &daWarpf_c::CreateHeap);

/* 024D717C */
bool daWarpf_c::_delete() {
    WWHD_FUNC(0x024D717C, bool, this);
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
VERIFY(0x024D717C, &daWarpf_c::_delete);

/* 024D602C */
BOOL daWarpf_c::checkEndDemo() {
    WWHD_FUNC(0x024D602C, BOOL, this);
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
VERIFY(0x024D602C, &daWarpf_c::checkEndDemo);

/* 024D6EEC */
cPhs_State daWarpf_c::CreateInit() {
    WWHD_FUNC(0x024D6EEC, cPhs_State, this);
    current.pos.y = get_earth_pos();
    if (!dComIfGs_isStageBossEnemy()) {
        return cPhs_ERROR_e;
    }

    set_effect();

    if (!checkEndDemo()) {
        m2D6 = dComIfGp_evmng_getEventIdx(STR(0x10042398) /* "WARP_WIND" */, 0xFF);
    } else {
        m2D6 = dComIfGp_evmng_getEventIdx(STR(0x10042388) /* "WARP_WIND_AFTER" */, 0xFF);
        setEndAnim();
    }
    return cPhs_COMPLEATE_e;
}
VERIFY(0x024D6EEC, &daWarpf_c::CreateInit);

/* 024D6FB8 */
cPhs_State daWarpf_c::_create() {
    WWHD_FUNC(0x024D6FB8, cPhs_State, this);
    /* fopAcM_ct(this, daWarpf_c); HD: fopAc_ac_c has a vtable */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            fopAc_ac_c_ct(this);
            __vtbl = ACT_VTBL;
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }

    mStageNo = dStage_stagInfo_GetSaveTbl(dComIfGp_getStageStagInfo());
    /* HD: JUT_ASSERT(0x2CD, mSceneNo >= 0 && mSceneNo < 16) */
    if ((u32)(s32)mStageNo >= 16)
        JUT_ASSERT_fail(STR(0x100423A4), 0x2CD, STR(0x100423B4));
    cPhs_State phase = cPhs_COMPLEATE_e;

    if (gabi::ea(arcname()) != 0) {
        phase = dComIfG_resLoad(&mPhase, arcname());
        if (phase == cPhs_COMPLEATE_e) {
            if (!fopAcM_entrySolidHeap(this, 0x024D6790 /* CheckCreateHeap */, 0x5000)) {
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
            gabi::store<u8>(p + 0x100, (u8)reverb);
            gabi::store<u32>(p + 0xFC, 0);
            gabi::store<u8>(p + 0x101, (u8)(gabi::load<u8>(p + 0x101) | 4));
        }
    }
    return phase;
}
VERIFY(0x024D6FB8, &daWarpf_c::_create);

/* 024D7B0C */
bool daWarpf_c::_execute() {
    WWHD_FUNC(0x024D7B0C, bool, this);
    m2E4 = m2E4 + 1;
    checkOrder();
    demo_proc();
    set_se();
    eventOrder();
    anim_play();
    return true;
}
VERIFY(0x024D7B0C, &daWarpf_c::_execute);

/* 024D79F4 */
void daWarpf_c::eventOrder() {
    WWHD_FUNC(0x024D79F4, void, this);
    if (m2D0 == 1) {
        fopAcM_orderOtherEventId(this, m2D6, 0xFF, 0xFFFF, 0, 1);
        eventInfo_onCondition(this, 2 /* dEvtCnd_UNK2_e */);
    }
}
VERIFY(0x024D79F4, &daWarpf_c::eventOrder);

/* 024D7628 */
void daWarpf_c::checkOrder() {
    WWHD_FUNC(0x024D7628, void, this);
    if (eventInfo_checkCommandDemoAccrpt(this)) {
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
VERIFY(0x024D7628, &daWarpf_c::checkOrder);

/* 024D770C */
void daWarpf_c::demo_proc() {
    WWHD_FUNC(0x024D770C, void, this);
    mStaffID = dComIfGp_evmng_getMyStaffId(STR(0x100423D4) /* "Warpf" */, nullptr, 0);
    if (dComIfGp_event_runCheck() && gabi::load<u16>(gabi::ea(this) + 0xF8) != 1 /* !checkCommandTalk */ && mStaffID != -1) {
        s32 actIdx = dComIfGp_evmng_getMyActIdx(mStaffID, ACTION_TABLE, 6, 0, 0);

        if (actIdx == -1) {
            dComIfGp_evmng_cutEnd(mStaffID);
        } else {
            if (dComIfGp_evmng_getIsAddvance(mStaffID)) {
                ptmf_call_i(EVENT_INIT_TBL + actIdx * 8, this, mStaffID);
            }

            BOOL ret = ptmf_call_i(EVENT_ACTION_TBL + actIdx * 8, this, mStaffID);
            if (ret) {
                dComIfGp_evmng_cutEnd(mStaffID);
            }
        }
    }
}
VERIFY(0x024D770C, &daWarpf_c::demo_proc);

/* 024D7BCC */
void daWarpf_c::initWait(int staffIdx) {
    WWHD_FUNC(0x024D7BCC, void, this, staffIdx);
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

    gabi::Local<cXyz> local_18;
    f32 x = current.pos.x;
    f32 y = current.pos.y;
    f32 z = current.pos.z;
    local_18->x = x;
    local_18->z = z;
    local_18->y = y + 1000.0f;

    local_18->y = dBgS_ObjGndChk_Func(local_18.get());
    dComIfGp_evmng_setGoal(local_18.get());
}
VERIFY(0x024D7BCC, &daWarpf_c::initWait);

/* 024D7C98 */
BOOL daWarpf_c::actWait(int staffIdx) {
    WWHD_FUNC(0x024D7C98, BOOL, this, staffIdx);
    if (m2E4 % 10 == 0) {
        set_effect_wind00();
    }
    return TRUE;
}
VERIFY(0x024D7C98, &daWarpf_c::actWait);

/* 024D7CE4 (not named by the matcher) */
void daWarpf_c::initWarpStart(int staffIdx) {
    WWHD_FUNC(0x024D7CE4, void, this, staffIdx);
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    m2EC.copy(player->current.pos);
}
VERIFY(0x024D7CE4, &daWarpf_c::initWarpStart);

/* 024D8130 (not named by the matcher) */
BOOL daWarpf_c::actWarpStart(int staffIdx) {
    WWHD_FUNC(0x024D8130, BOOL, this, staffIdx);
    return TRUE;
}
VERIFY(0x024D8130, &daWarpf_c::actWarpStart);

/* 024D7D2C (not named by the matcher) */
void daWarpf_c::initWarpMode_1(int staffIdx) {
    WWHD_FUNC(0x024D7D2C, void, this, staffIdx);
    mWarpTimer = 180;
}
VERIFY(0x024D7D2C, &daWarpf_c::initWarpMode_1);

/* csXyz returned in r3:r4 (GHS small-struct return): r3 = x:y, r4 = z:padding */
struct angle_ret {
    be<u32> w0;
    be<u32> w1;
};
static inline void store_angle(angle_ret* a, u32 r3, u32 r4) {
    a->w0 = r3;
    a->w1 = r4;
}

/* 024D7D38 */
BOOL daWarpf_c::actWarpMode_1(int staffIdx) {
    WWHD_FUNC(0x024D7D38, BOOL, this, staffIdx);
    if (cLib_calcTimer(&mWarpTimer) == 0) {
        if (mStageNo != STAGE_TOTG && mStageNo != STAGE_ET && mStageNo != STAGE_WT) {
            gabi::Local<cXyz> local_18;
            gabi::Local<angle_ret> local_20;
            f32 x = current.pos.x;
            f32 y = current.pos.y;
            local_18->x = x;
            f32 z = current.pos.z;
            local_18->y = y;
            local_18->z = z;
            u32 r3 = get_angle_wind01();
            u32 r4 = gabi::cpu->r[4];
            f32 ey = m2EC.y;
            store_angle(local_20.get(), r3, r4);
            local_18->y = ey + 100.0f;

            dComIfGp_particle_set(ID_AK_SN_BOSSWARPWIND01_820B, local_18.get(), gabi::at<csXyz>(gabi::ea(local_20.get())));
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
VERIFY(0x024D7D38, &daWarpf_c::actWarpMode_1);

/* 024D7E58 (not named by the matcher) */
void daWarpf_c::initWarpMode_2(int staffIdx) {
    WWHD_FUNC(0x024D7E58, void, this, staffIdx);
    mWarpTimer = 5;
}
VERIFY(0x024D7E58, &daWarpf_c::initWarpMode_2);

/* 024D7E64 */
BOOL daWarpf_c::actWarpMode_2(int staffIdx) {
    WWHD_FUNC(0x024D7E64, BOOL, this, staffIdx);
    if (cLib_calcTimer(&mWarpTimer) == 0) {
        if (mStageNo != STAGE_TOTG && mStageNo != STAGE_ET && mStageNo != STAGE_WT) {
            gabi::Local<cXyz> local_18;
            gabi::Local<angle_ret> local_20;
            f32 x = current.pos.x;
            f32 y = current.pos.y;
            f32 z = current.pos.z;
            local_18->y = y;
            local_18->z = z;
            local_18->x = x;
            u32 r3 = get_angle_wind01();
            u32 r4 = gabi::cpu->r[4];
            f32 ey = m2EC.y;
            store_angle(local_20.get(), r3, r4);
            local_18->y = ey + 120.0f;
            u32 za = gabi::ea(local_20.get()) + 4;
            gabi::store<s16>(za, (s16)(gabi::load<s16>(za) + 0x38E));

            dComIfGp_particle_set(ID_AK_SN_BOSSWARPWIND01_820B, local_18.get(), gabi::at<csXyz>(gabi::ea(local_20.get())));
        }
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x024D7E64, &daWarpf_c::actWarpMode_2);

/* 024D7F5C (not named by the matcher) */
void daWarpf_c::initWarpMode_3(int staffIdx) {
    WWHD_FUNC(0x024D7F5C, void, this, staffIdx);
    mWarpTimer = 5;
}
VERIFY(0x024D7F5C, &daWarpf_c::initWarpMode_3);

/* 024D7F68 */
BOOL daWarpf_c::actWarpMode_3(int staffIdx) {
    WWHD_FUNC(0x024D7F68, BOOL, this, staffIdx);
    if (cLib_calcTimer(&mWarpTimer) == 0) {
        if (mStageNo != STAGE_TOTG && mStageNo != STAGE_ET && mStageNo != STAGE_WT) {
            gabi::Local<cXyz> local_18;
            gabi::Local<angle_ret> local_20;
            f32 x = current.pos.x;
            f32 y = current.pos.y;
            local_18->x = x;
            f32 z = current.pos.z;
            local_18->y = y;
            local_18->z = z;
            u32 r3 = get_angle_wind01();
            u32 r4 = gabi::cpu->r[4];
            f32 ey = m2EC.y;
            store_angle(local_20.get(), r3, r4);
            local_18->y = ey + 140.0f;
            u32 ya = gabi::ea(local_20.get()) + 2;
            gabi::store<s16>(ya, (s16)(gabi::load<s16>(ya) + 0x71C));

            dComIfGp_particle_set(ID_AK_SN_BOSSWARPWIND01_820B, local_18.get(), gabi::at<csXyz>(gabi::ea(local_20.get())));
        }
        mDoAud_seStart(JA_SE_LK_BOSS_WARP_EFF_ST, nullptr, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(this)));
        /* onEndDemo(): empty */
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x024D7F68, &daWarpf_c::actWarpMode_3);

/* 024D8138 (not named by the matcher) */
void daWarpf_c::initEndWait(int staffIdx) {
    WWHD_FUNC(0x024D8138, void, this, staffIdx);
}
VERIFY(0x024D8138, &daWarpf_c::initEndWait);

/* 024D813C (not named by the matcher) */
BOOL daWarpf_c::actEndWait(int staffIdx) {
    WWHD_FUNC(0x024D813C, BOOL, this, staffIdx);
    return TRUE;
}
VERIFY(0x024D813C, &daWarpf_c::actEndWait);

/* 024D7544 */
BOOL daWarpf_c::check_warp_event() {
    WWHD_FUNC(0x024D7544, BOOL, this);
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    f32 abs = absXZ_diff(&player->current.pos, &current.pos);

    if (daPy_checkPlayerFly(player)) {
        return FALSE;
    }

    if (abs < get_distance())
        return TRUE;
    return FALSE;
}
VERIFY(0x024D7544, &daWarpf_c::check_warp_event);

/* 024D6804 */
f32 daWarpf_c::get_distance() {
    WWHD_FUNC(0x024D6804, f32, this);
    if (mStageNo == STAGE_ET || mStageNo == STAGE_WT) {
        if (checkEndDemo())
            return 190.0f;
    }
    return gabi::load<f32>(M_WARP_SIZE + 4 * mStageNo);
}
VERIFY(0x024D6804, &daWarpf_c::get_distance);

/* 024D6794 */
f32 daWarpf_c::get_earth_pos() {
    WWHD_FUNC(0x024D6794, f32, this);
    BOOL end = checkEndDemo();
    s32 stageNo = mStageNo;
    if (!end) {
        f32 earthPos = gabi::load<f32>(L_EARTH_POS + 4 * stageNo);
        if (stageNo == STAGE_FW && getSetType()) {
            earthPos = 0.0f;
        }
        return earthPos;
    }
    return gabi::load<f32>(L_EARTH_POS_2ND + 4 * stageNo);
}
VERIFY(0x024D6794, &daWarpf_c::get_earth_pos);

/* 024D6940 */
void daWarpf_c::set_effect() {
    WWHD_FUNC(0x024D6940, void, this);
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    f32 twok = 2000.0f;
    s32 iVar2 = 20;
    f32 abs = absXZ_diff(&player->current.pos, &current.pos);

    if (abs < twok && abs > get_distance()) {
        f32 tmp = abs - get_distance();
        iVar2 = gabi::ftoi(gabi::fmadds(tmp / twok, 17.0f, 3.0f));
    }

    switch ((u32)(s32)mStageNo) {
    case STAGE_TOTG:
        if (m290 == 0) {
            if (getSetType() == 0) {
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
                if (getSetType() && m2B8 != 0) {
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
        if (t - ppc_divw(t, (u32)iVar2) * (u32)iVar2 == 0) {
            set_effect_wind00();
        }
        if (m294 == 0) {
            m294 = gabi::ea(dComIfGp_particle_set(ID_AK_SN_BOSSWARPKIRAKIRA00_820C, &current.pos));
        }
        break;
    }
    }
}
VERIFY(0x024D6940, &daWarpf_c::set_effect);

/* 024D688C */
void daWarpf_c::set_effect_wind00() {
    WWHD_FUNC(0x024D688C, void, this);
    if (mStageNo == STAGE_TOTG || mStageNo == STAGE_WT || mStageNo == STAGE_ET) {
        return;
    }

    u32 emitter = gabi::ea(dComIfGp_particle_set(ID_AK_SN_BOSSWARPWIND00_820A, &current.pos));
    if (emitter != 0) {
        JPABaseEmitter_setMaxFrame(emitter, 1);
        JPABaseEmitter_setGlobalAlpha(emitter, (u8)gabi::ftoi(cM_rndF(127.9f) + 128.0f));
    }
}
VERIFY(0x024D688C, &daWarpf_c::set_effect_wind00);

/* 024D7B6C: returns csXyz{angleX, angleY + 0x8000, 0} in r3:r4. HD: current.angle is not read */
u32 daWarpf_c::get_angle_wind01() {
    WWHD_FUNC(0x024D7B6C, u32, this);
    s16 x = dCam_getAngleX(dComIfGp_getCamera0());
    s16 y = dCam_getAngleY(dComIfGp_getCamera0()) + 0x8000;
    gabi::cpu->r[4] = 0; /* z = 0, padding (not compared: only r3 is) */
    return ((u32)(u16)x << 16) | (u16)y;
}
VERIFY(0x024D7B6C, &daWarpf_c::get_angle_wind01);

/* 024D7A54 */
void daWarpf_c::anim_play() {
    WWHD_FUNC(0x024D7A54, void, this);
    if (mStageNo == STAGE_TOTG) {
        if (m2AC != 0) {
            mDoExt_baseAnm_play(gabi::at<void>(m2AC));
        }
        if (m2B0 != 0) {
            mDoExt_baseAnm_play(gabi::at<void>(m2B0));
            /* GameCube bug kept: tests m2B0 again before playing m2B4 */
            if (m2B0 != 0) {
                mDoExt_baseAnm_play(gabi::at<void>(m2B4));
            }
        }
    } else if (mStageNo == STAGE_ET || mStageNo == STAGE_WT) {
        if (m2AC != 0) {
            mDoExt_baseAnm_play(gabi::at<void>(m2AC));
        }
        if (m2B8 != 0) {
            if (m2E4 >= 120) {
                anm_setRate(m2B8, 1.0f);
            }
            mDoExt_baseAnm_play(gabi::at<void>(m2B8));
        }
    }
}
VERIFY(0x024D7A54, &daWarpf_c::anim_play);

/* 024D6D94 */
void daWarpf_c::setEndAnim() {
    WWHD_FUNC(0x024D6D94, void, this);
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
VERIFY(0x024D6D94, &daWarpf_c::setEndAnim);

/* 024D78D0 */
void daWarpf_c::set_se() {
    WWHD_FUNC(0x024D78D0, void, this);
    s32 t = m2E0 + 1;
    if (t > 20000) {
        t = 20000;
    }
    m2E0 = t;

    switch ((u32)(s32)m2D8) {
    case 0:
        if (getSetType() == 0) {
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
VERIFY(0x024D78D0, &daWarpf_c::set_se);

/* 024D7258 */
void daWarpf_c::set_mtx() {
    WWHD_FUNC(0x024D7258, void, this);
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
VERIFY(0x024D7258, &daWarpf_c::set_mtx);

/* 024D73B4 */
bool daWarpf_c::_draw() {
    WWHD_FUNC(0x024D73B4, bool, this);
    switch ((u32)(s32)mStageNo) {
    case STAGE_TOTG:
        set_mtx();
        settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
        setLightTevColorType(dKy_getEnvlight(), m2A8, &tevStr);

        if (m2AC != 0) {
            mDoExt_brkAnm_entry(gabi::at<mDoExt_brkAnm>(m2AC), J3DModel_getModelData(m2A8), anm_getFrame(m2AC));
        }
        if (m2B0 != 0) {
            gabi::at<mDoExt_bckAnm>(m2B0)->entry(J3DModel_getModelData(m2A8), anm_getFrame(m2B0));
        }
        if (m2B4 != 0) {
            mDoExt_btkAnm_entry(gabi::at<mDoExt_btkAnm>(m2B4), J3DModel_getModelData(m2A8), anm_getFrame(m2B4));
        }

        mDoExt_modelUpdateDL(m2A8);
        break;

    case STAGE_ET:
    case STAGE_WT:
        set_mtx();
        settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);

        if (m2A8) {
            setLightTevColorType(dKy_getEnvlight(), m2A8, &tevStr);

            /* HD: the brk entries take the model (025E8480) */
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
VERIFY(0x024D73B4, &daWarpf_c::_draw);

/* 024D7178 */
static cPhs_State daWarpf_Create(void* i_this) {
    WWHD_FUNC(0x024D7178, cPhs_State, i_this);
    return ((daWarpf_c*)i_this)->_create();
}
VERIFY(0x024D7178, daWarpf_Create);

/* 024D7254 */
static BOOL daWarpf_Delete(void* i_this) {
    WWHD_FUNC(0x024D7254, BOOL, i_this);
    return ((daWarpf_c*)i_this)->_delete();
}
VERIFY(0x024D7254, daWarpf_Delete);

/* 024D7540 */
static BOOL daWarpf_Draw(void* i_this) {
    WWHD_FUNC(0x024D7540, BOOL, i_this);
    return ((daWarpf_c*)i_this)->_draw();
}
VERIFY(0x024D7540, daWarpf_Draw);

/* 024D7B68 */
static BOOL daWarpf_Execute(void* i_this) {
    WWHD_FUNC(0x024D7B68, BOOL, i_this);
    return ((daWarpf_c*)i_this)->_execute();
}
VERIFY(0x024D7B68, daWarpf_Execute);

/* 024D8128 */
static BOOL daWarpf_IsDelete(void*) {
    WWHD_FUNC(0x024D8128, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x024D8128, daWarpf_IsDelete);

/* 024D8144: daWarpf_c deleting destructor (HD, compiler-generated) */
static void daWarpf_c_dtor(daWarpf_c* p, s32 flags) {
    WWHD_FUNC(0x024D8144, void, p, flags);
    if (p != nullptr) {
        gabi::call(0x025D50BC, p, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(p);
    }
}
VERIFY(0x024D8144, daWarpf_c_dtor);

/* 024D8114: this TU's sead::SafeString deleting destructor (compiler-generated) */
static void SafeString_dtor(SafeString* p, s32 flags) {
    WWHD_FUNC(0x024D8114, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x024D8114, SafeString_dtor);

/* 024D8080: __sinit_d_a_warpf_cpp (HD header statics only) */
static void sinit_d_a_warpf() {
    WWHD_FUNC(0x024D8080, void);
    sinit_header_statics(0x1046EB10, 0x101D2EB0);
}
VERIFY(0x024D8080, sinit_d_a_warpf);
