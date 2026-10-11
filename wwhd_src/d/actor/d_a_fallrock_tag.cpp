/**
 * Tag - falling rocks: spawns rocks while a schedule bit is active.
 * WWHD, GameCube-style source verified against the original (unit d_a_fallrock_tag).
 */
#include "d/actor/d_a_fallrock_tag.h"

enum { fpcNm_FallRock_e = 0x1A6, JA_SE_ATM_RAKUBAN = 0x105F };

// HD: the table is folded into constants; unused GameCube fields are omitted.
const daFallRockTag_Data_c daFallRockTag_c::m_data = {250.0f, 0.3f, 0.8f, 90, 3};
const f32 daFallRockTag_c::m_div_num = 6.0f;

/* inlined into daFallRockTag_Draw */
BOOL daFallRockTag_c::draw() {
    return TRUE;
}

/* 02131F90 */
static BOOL daFallRockTag_Draw(daFallRockTag_c* i_this) {
    return i_this->draw();
}

/* inlined into daFallRockTag_Execute */
BOOL daFallRockTag_c::execute() {
    int endTime = ftoi((f32)fallRock_getScheduleSeconds(dComIfGp_getStageStagInfo()) / m_div_num) * 30;
    u8 schbit = dKy_get_schbit();
    if (schbit & mSchBit) {
        if (endTime < dKy_get_schbit_timer()) {
            int timer = dKy_get_schbit_timer();
            timer -= getData()->mTimerOffset;
            if (timer % (30 / getData()->mRockNumPerSec) == 0) {
                f32 range = getData()->mRange * scale.x;
                cXyz pos;
                pos.x = cM_rndFX(range);
                pos.y = 0.0f;
                pos.z = cM_rndFX(range - std::fabs((f32)pos.x));
                f32 scaleMin = getData()->mScaleMin;
                // HD: preserve the random result as the left operand of the addition.
                f32 s = cM_rndF(getData()->mScaleMax - scaleMin) + scaleMin;
                cXyz rockScale;
                rockScale.x = rockScale.y = rockScale.z = s;
                csXyz angle;
                angle.x = (s16)ftoi(cM_rndF(32767.0f));
                angle.y = (s16)ftoi(cM_rndF(32767.0f));
                angle.z = (s16)ftoi(cM_rndF(32767.0f));
                createRock(&pos, &rockScale, &angle, fopAcM_GetRoomNo(this), 0);
                fopAcM_seStart(this, JA_SE_ATM_RAKUBAN, 0);
            }
        } else {
            field_0x298 = 0;
        }
    }
    return TRUE;
}

/* 02131C88 */
static BOOL daFallRockTag_Execute(daFallRockTag_c* i_this) {
    return i_this->execute();
}

/* 02131E7C */
static BOOL daFallRockTag_IsDelete(daFallRockTag_c*) {
    return TRUE;
}

/* 02131E84 */
// HD: the destructor no longer unlinks a dynamic module.
static BOOL daFallRockTag_Delete(daFallRockTag_c* i_this) {
    return TRUE;
}

/* inlined into daFallRockTag_Create (fopAcM_ct) */
daFallRockTag_c::daFallRockTag_c() {
    fopAc_ac_c_ct(this);
    __vtbl = __vt;
}

/* inlined into daFallRockTag_Create */
cPhs_State daFallRockTag_c::create() {
    // HD: no dynamic-module link is needed.
    fopAcM_ct(this, daFallRockTag_c);
    mSchBit = fopAcM_GetParam(this);
    fopAcM_offDraw(this);
    return cPhs_COMPLEATE_e;
}

/* 02131E8C */
static cPhs_State daFallRockTag_Create(fopAc_ac_c* i_this) {
    return ((daFallRockTag_c*)i_this)->create();
}

/* 02131C14 */
void daFallRockTag_c::createRock(cXyz* i_pos, cXyz* i_scale, csXyz* i_angle, int i_roomNo, u32 i_params) {
    cXyz pos;
    pos.x = current.pos.x + i_pos->x;
    pos.y = current.pos.y + i_pos->y;
    pos.z = current.pos.z + i_pos->z;
    fopAcM_create(fpcNm_FallRock_e, i_params, &pos, i_roomNo, i_angle, i_scale, -1, 0);
}

/* ---------------- HD: compiler-generated code of this translation unit ---------------- */

/* 02131EFC */
static void __sinit_d_a_fallrock_tag_cpp() {
    sinit_header_statics(0x10463CDC, 0x101B4AB0);
}

/* 02131F98 */
// HD: compiler-generated deleting destructor, including the base-class destructor.
static void daFallRockTag_c_dt(daFallRockTag_c* i_this, s32 flags) {
    if (i_this != nullptr) {
        i_this->__vtbl = daFallRockTag_c::__vt;
        __dt(static_cast<fopAc_ac_c*>(i_this));
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* WWHD glue (tools/verify/autoglue.py): existing schedule getter bindings.
ext 025602A8 u8 dKy_get_schbit()
ext 025602CC s32 dKy_get_schbit_timer()
*/
