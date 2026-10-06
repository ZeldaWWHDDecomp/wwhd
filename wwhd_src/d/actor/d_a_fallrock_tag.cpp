/**
 * d_a_fallrock_tag.cpp (WWHD)
 * Tag - falling rocks: spawns FallRock actors while a schedule bit is active.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_fallrock_tag.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define FALLROCKTAG_VTBL 0x1000E8C4 /* daFallRockTag_c vtable (HD virtual destructor) */

enum { fpcNm_FallRock_e = 0x1A6 };
enum { JA_SE_ATM_RAKUBAN = 0x105F };

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline u8 dKy_get_schbit() { return gabi::call<u8>(0x025602A8); }
static inline s32 dKy_get_schbit_timer() { return gabi::call<s32>(0x025602CC); }
/* the stage info: play+0x5150 (dStage_stageDt_c), virtual getStagInfo at vtable +0x15C (as d_a_tsubo) */
static inline u32 dComIfGp_getStageStagInfo() {
    u32 dt = dComIfGp_ea() + 0x5150;
    return gabi::call_ptr<u32>(gabi::load<u32>(gabi::load<u32>(dt) + 0x15C), dt);
}
/* dStage_stagInfo_GetSchSec: u8 at +0xF of the stage info */
static inline u8 dStage_stagInfo_GetSchSec(u32 info) { return gabi::load<u8>(info + 0xF); }

/* daFallRockTag_c::m_data / m_div_num: HD folds them into constants */
struct daFallRockTag_Data_c {
    f32 mRange = 250.0f;
    f32 mScaleMin = 0.3f;
    f32 mScaleMax = 0.8f;
    s16 mTimerOffset = 90;
    s16 mRockNumPerSec = 3;
};
static const daFallRockTag_Data_c m_data;
static const f32 m_div_num = 6.0f;

struct daFallRockTag_c : fopAc_ac_c {
    void createRock(cXyz*, cXyz*, csXyz*, int, u32);
    const daFallRockTag_Data_c* getData() { return &m_data; }

    /* 0x3AC */ u8 field_0x290[0x3B4 - 0x3AC];
    /* 0x3B4 */ be<s32> field_0x298;
    /* 0x3B8 */ u8 field_0x29c[2];
    /* 0x3BA */ be<u8> mSchBit;
    /* 0x3BB */ u8 field_0x29f;
};
WWHD_OFFSET(daFallRockTag_c, field_0x298, 0x3B4);
WWHD_OFFSET(daFallRockTag_c, mSchBit, 0x3BA);
WWHD_SIZE(daFallRockTag_c, 0x3BC);

/* 02131C14 */
void daFallRockTag_c::createRock(cXyz* i_pos, cXyz* i_scale, csXyz* i_angle, int i_roomNo, u32 i_params) {
    WWHD_FUNC(0x02131C14, void, this, i_pos, i_scale, i_angle, i_roomNo, i_params);
    gabi::Local<cXyz> pos;
    pos->x = current.pos.x + i_pos->x;
    pos->y = current.pos.y + i_pos->y;
    pos->z = current.pos.z + i_pos->z;
    fopAcM_create(fpcNm_FallRock_e, i_params, pos, i_roomNo, i_angle, i_scale, -1, 0);
}
VERIFY(0x02131C14, &daFallRockTag_c::createRock);

/* 02131C88: daFallRockTag_c::execute() inlined */
static BOOL daFallRockTag_Execute(daFallRockTag_c* i_this) {
    WWHD_FUNC(0x02131C88, BOOL, i_this);
    int endTime = gabi::ftoi((f32)dStage_stagInfo_GetSchSec(dComIfGp_getStageStagInfo()) / m_div_num) * 30;
    u8 schbit = dKy_get_schbit();
    if (schbit & i_this->mSchBit) {
        if (endTime < dKy_get_schbit_timer()) {
            int timer = dKy_get_schbit_timer();
            timer -= i_this->getData()->mTimerOffset;
            if (timer % (30 / i_this->getData()->mRockNumPerSec) == 0) {
                f32 range = i_this->getData()->mRange * i_this->scale.x;
                gabi::Local<cXyz> pos;
                pos->x = cM_rndFX(range);
                pos->y = 0.0f;
                pos->z = cM_rndFX(range - std::fabs((f32)pos->x));
                f32 scaleMin = i_this->getData()->mScaleMin;
                f32 s = cM_rndF(i_this->getData()->mScaleMax - scaleMin) + scaleMin;
                gabi::Local<cXyz> rockScale;
                rockScale->x = rockScale->y = rockScale->z = s;
                gabi::Local<csXyz> angle;
                angle->x = (s16)gabi::ftoi(cM_rndF(32767.0f));
                angle->y = (s16)gabi::ftoi(cM_rndF(32767.0f));
                angle->z = (s16)gabi::ftoi(cM_rndF(32767.0f));
                i_this->createRock(pos, rockScale, angle, fopAcM_GetRoomNo(i_this), 0);
                fopAcM_seStart(i_this, JA_SE_ATM_RAKUBAN, 0);
            }
        } else {
            i_this->field_0x298 = 0;
        }
    }
    return TRUE;
}
VERIFY(0x02131C88, daFallRockTag_Execute);

/* 02131E7C */
static BOOL daFallRockTag_IsDelete(daFallRockTag_c*) {
    WWHD_FUNC(0x02131E7C, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x02131E7C, daFallRockTag_IsDelete);

/* 02131E84: HD: ~daFallRockTag_c() no longer unlinks a dynamic module (no cDyl in HD) */
static BOOL daFallRockTag_Delete(daFallRockTag_c* i_this) {
    WWHD_FUNC(0x02131E84, BOOL, i_this);
    return TRUE;
}
VERIFY(0x02131E84, daFallRockTag_Delete);

/* 02131E8C: daFallRockTag_c::create() inlined. HD: no cDyl_LinkASync */
static cPhs_State daFallRockTag_Create(fopAc_ac_c* i_ac) {
    WWHD_FUNC(0x02131E8C, cPhs_State, i_ac);
    daFallRockTag_c* i_this = (daFallRockTag_c*)i_ac;
    if (!fopAcM_CheckCondition(i_this, fopAcCnd_INIT_e)) {
        if (i_this != nullptr) {
            fopAc_ac_c_ct(i_this);
            i_this->__vtbl = FALLROCKTAG_VTBL;
        }
        fopAcM_OnCondition(i_this, fopAcCnd_INIT_e);
    }
    i_this->mSchBit = fopAcM_GetParam(i_this);
    fopAcM_offDraw(i_this);
    return cPhs_COMPLEATE_e;
}
VERIFY(0x02131E8C, daFallRockTag_Create);

/* 02131EFC: static initialisation of the translation unit (header statics only) */
static void __sinit_d_a_fallrock_tag_cpp() {
    WWHD_FUNC(0x02131EFC, void, (u32)0);
    sinit_header_statics(0x10463CDC, 0x101B4AB0);
}
VERIFY(0x02131EFC, __sinit_d_a_fallrock_tag_cpp);

/* 02131F90 (placed after __sinit) */
static BOOL daFallRockTag_Draw(daFallRockTag_c* i_this) {
    WWHD_FUNC(0x02131F90, BOOL, i_this);
    return TRUE;
}
VERIFY(0x02131F90, daFallRockTag_Draw);

/* 02131F98: daFallRockTag_c deleting destructor (HD virtual destructor; the body is empty in HD) */
static void daFallRockTag_c_dt(daFallRockTag_c* i_this, s32 flags) {
    WWHD_FUNC(0x02131F98, void, i_this, flags);
    if (i_this != nullptr) {
        i_this->__vtbl = FALLROCKTAG_VTBL;
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x02131F98, daFallRockTag_c_dt);
