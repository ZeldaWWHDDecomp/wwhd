/**
 * d_a_ykgr.cpp (WWHD)
 * Heat haze (Dragon Roost mountain "kagerou"): one screen-space particle emitter whose strength
 * follows the player's distance to a path.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_ykgr.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_ykgr.h"
const gc_tu_data gc_tu = { .safeString = 0x10043040 };
static daYkgr_HIO_c l_HIO;
static dPa_YkgrPcallBack YkgrCB;
// HD: these class statics are mapped as TU data by the current glue tool.
static YkgrEmitter_view* m_emitter;
static f32 m_aim_rate;
using YkgrPath = path_layout::dPath;
static YkgrPath* m_path;
static u8 m_alpha_flag;
static u8 m_alpha;

/* 024EA58C */
// HD: HD: the s8 m1C is the truncated parameter, and the result is halved once more
void dPa_YkgrPcallBack::setParam(f32 arg1) {
    /* GHS: ble/bge on the negated comparisons (NaN returns) */
    if (!(arg1 > -17.0f)) return;
    if (!(arg1 < 47.0f)) return;
    f32 v;
    s8 c;
    if (!(arg1 < 0.0f)) { /* blt */
        c = (s8)gabi::ftoi(arg1);
        v = gabi::fmadds(arg1 - (f32)c, 0.5f, 0.5f) * 0.5f;
    } else {
        f32 tmp = arg1 - 1.0f;
        c = (s8)gabi::ftoi(tmp);
        v = gabi::fmadds(tmp - (f32)c, 0.5f, 1.0f) * 0.5f;
    }
    m1C = c;
    m14 = v;
    m04 = v;
    m08 = 0.0f;
    m0C = 0.0f;
    m10 = 0.0f;
    m18 = 0.0f;
}

/* 024EA984 */
f32 daYkgr_c::getPosRate() {
    if (m_path == nullptr) {
        return 0.0f;
    }
    f32 fVar4 = FLOAT_MAX;
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    YkgrPath* path = m_path;
    f32 px = player->current.pos.x;
    f32 pz = player->current.pos.z;
    u32 num = path->m_num;
    path_layout::dPnt* pnt = path->m_points;
    cXyz sp30;
    cXyz sp24;
    for (; num != 0; num--, pnt++) {
        /* cXyz::absXZ */
        sp24.x = pnt->m_position.x;
        sp24.y = 0.0f;
        sp24.z = pnt->m_position.z;
        sp30.x = px;
        sp30.y = 0.0f;
        sp30.z = pz;
        f32 abs = std_sqrtf(PSVECSquareDistance(&sp30, &sp24));
        if (abs < fVar4) {
            fVar4 = abs;
        }
    }
    f32 hi = l_HIO.m20;
    f32 lo = l_HIO.m24;
    if (fVar4 > hi) {
        fVar4 = hi;
    } else if (fVar4 < lo) {
        fVar4 = lo;
    }
    return 1.0f - (fVar4 - lo) / (hi - lo);
}

/* 024EA6A0 */
// HD: SafeString stage comparison, no HIO child; guarded parameter reload and emitter retest.
cPhs_State daYkgr_c::_create() {
    u32 prm = mParameters;
    u8 uVar4 = (prm >> 0x14) & 0xF;
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (gabi::ea(this) != 0) new (this) daYkgr_c();
        prm = mParameters;
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    u8 pathIndex = (prm >> 8) & 0xFF;
    YkgrPath* path = nullptr;
    if (pathIndex != 0xFF) {
        path = (YkgrPath*)dPath_GetRoomPath(pathIndex, fopAcM_GetRoomNo(this));
    }
    m_path = path;

    if (SafeString_eq("Adanmae", dComIfGp_getStartStageName(), gc_tu.safeString) &&
        dComIfGs_isSymbol(dSymbol_DIN_e)) {
        return cPhs_STOP_e;
    }

    if (m_emitter == nullptr) {
        fopAc_ac_c* player = dComIfGp_getPlayer(0);
        current.pos.copy(player->current.pos);
        /* dComIfGp_particle_setProjection(dPa_name::ID_AK_SP_MTDRAGONKAGEROU, &current.pos) */
        JPABaseEmitter* emitter = dComIfGp_getParticle()->set(4, dPa_name::ID_AK_SP_MTDRAGONKAGEROU, &current.pos, nullptr, nullptr, 0xFF,
                                                  nullptr, -1, nullptr, nullptr, nullptr);
        m_emitter = (YkgrEmitter_view*)emitter;
        if (emitter == nullptr) {
            return cPhs_ERROR_e;
        }
        ((YkgrEmitter_view*)emitter)->setParticleCallBackPtr(&YkgrCB);
        YkgrCB.setParam(-3.0f);
        fopAcM_setStageLayer(this);
        if (uVar4 == 1) {
            YkgrEmitter_view* e = m_emitter;
            m_alpha = 0xFF;                      /* setAlpha */
            e->setGlobalAlpha(0xFF);
            m2CC = 0.5f;
            m2D0 = 0.0f;
            if (m_emitter != nullptr) m_alpha_flag = 1; /* start() */
        } else {
            YkgrEmitter_view* e = m_emitter;
            m_alpha = 0;
            e->setGlobalAlpha(0);
            m2CC = 0.5f;
            m2D0 = 0.0f;
            if (m_emitter != nullptr) m_alpha_flag = 0; /* stop() */
        }
    } else {
        m_alpha_flag = (uVar4 == 1) ? 1 : 0; /* start() / stop() */
        return cPhs_STOP_e;
    }
    return cPhs_COMPLEATE_e;
}

/* 024EA97C */
static BOOL daYkgrDelete(void* v_this) {
    return true; /* _delete() */
}

/* 024EAAE8 */
// HD: daYkgrExecute (_execute inlined)
static BOOL daYkgrExecute(void* v_this) {
    daYkgr_c* i_this = (daYkgr_c*)v_this;
    cLib_addCalc2(&i_this->m2CC, m_aim_rate, 0.25f, 0.05f);
    cLib_addCalc2(&m_aim_rate, l_HIO.m1C, 0.25f, 0.05f);
    f32 rate = i_this->getPosRate();
    cLib_addCalc2(&i_this->m2D0, rate, 0.25f, 0.05f);

    f32 fVar1 = gabi::fmadds(0.5f, i_this->m2CC, 0.5f * i_this->m2D0);
    f32 tmp2 = gabi::fmadds(fVar1, l_HIO.m18, (1.0f - fVar1) * l_HIO.m14);
    YkgrCB.setParam(tmp2);

    u8 flag = m_alpha_flag;
    s32 alpha = m_alpha;
    if (flag == 0) {
        if (alpha != 0) {
            s32 step = l_HIO.m0C;
            if (alpha > step) {
                m_alpha = (u8)(alpha - step);
            } else {
                m_alpha = 0;
            }
        }
    } else if ((u32)alpha < 0xFF) {
        s32 step = l_HIO.m0C;
        if (alpha < 0xFF - step) {
            m_alpha = (u8)(alpha + step);
        } else {
            m_alpha = 0xFF;
        }
    }
    return true;
}

/* 024EAC68 */
// HD: daYkgrDraw (_draw and set_mtx inlined)
static BOOL daYkgrDraw(void* v_this) {
    daYkgr_c* i_this = (daYkgr_c*)v_this;
    if (m_alpha == 0) {
        return true;
    }
    YkgrCamera_view* camera = (YkgrCamera_view*)dComIfGp_getCamera(0);
    i_this->current.pos.copy(camera->mEye);
    i_this->current.angle.y = camera->mAngle.y;
    i_this->current.angle.x = camera->mAngle.x;
    mDoMtx_stack_c::transS(i_this->current.pos.x, i_this->current.pos.y, i_this->current.pos.z);
    mDoMtx_stack_c::YrotM(i_this->current.angle.y);
    mDoMtx_XrotM(mDoMtx_stack_c::get(), i_this->current.angle.x);
    PSMTXCopy(mDoMtx_stack_c::get(), &i_this->mMtx);
    YkgrEmitter_view* e = m_emitter;
    if (e != nullptr) {
        e->setGlobalRTMatrix(&i_this->mMtx);
        YkgrEmitter_view* e2 = m_emitter;
        e2->setGlobalAlpha(m_alpha);
    }
    return true;
}

/* 024EAD4C */
// HD: one GX2 indirect-matrix/alpha-input call with the matrix and signed scale exponent.
static void YkgrPcallBack_draw(dPa_YkgrPcallBack* cb, u32 emitter, u32 ctx) {
    JPASetIndTexMtx(ctx, 0, &cb->m04, (s32)(s8)cb->m1C);
}

/* 024EAD68 */
static void __sinit_d_a_ykgr_cpp() {
    sinit_header_statics_z(0x1046EC54, 0x101D3C3C, 0x1046ECA8);
    /* daYkgr_HIO_c::daYkgr_HIO_c (l_HIO) and dPa_YkgrPcallBack::dPa_YkgrPcallBack (YkgrCB), interleaved */
    daYkgr_HIO_c& h = l_HIO;
    dPa_YkgrPcallBack* cb = &YkgrCB;
    h.mNo = 0;
    h.m10 = -16.0f;
    h.m14 = -5.0f;
    h.m18 = -3.0f;
    h.m1C = 0.0f;
    cb->m1C = 1;
    h.m20 = 1500.0f;
    h.__vtbl = 0x10043068;
    h.m08 = 0xFF;
    h.m24 = 500.0f;
    h.m0C = 3;
    cb->__vtbl = 0x100430A4;
    cb->m04 = 0.5f;
    cb->m08 = 0.0f;
    cb->m0C = 0.0f;
    cb->m10 = 0.0f;
    cb->m14 = 0.5f;
    cb->m18 = 0.0f;
    __register_global_object(0x101D3C60);
}

/* 024EAEBC */
// HD: deleting destructor of a class with a trivial destructor
static void trivial_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

/* 024EAED0 */
static BOOL daYkgrIsDelete(void* i_this) {
    return TRUE;
}

/* 024EAED8, 024EAEDC, 024EAEE0: dPa_YkgrPcallBack's empty virtuals (vtable 0x100430A4 slots) */
/* 024EAED8 */
static void YkgrPcallBack_empty0(void* p) {
}
/* 024EAEDC */
static void YkgrPcallBack_empty1(void* p) {
}
/* 024EAEE0 */
static void YkgrPcallBack_empty2(void* p) {
}

/* 024EAEE4 */
// HD: daYkgr_c deleting destructor
static void daYkgr_c_dt(daYkgr_c* i_this, s32 flags) {
    if (i_this != nullptr) {
        __dt((fopAc_ac_c*)i_this);
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 024EAF38 */
// HD: dPa_YkgrPcallBack deleting destructor (trivial)
static void YkgrPcallBack_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

/* ---- leftover functions of the translation unit ---- */

/* 024EAF4C */
// HD: sead::SafeStringBase<char>::assureTerminationImpl_ (empty); vtable slot 10043054, after the destructor 024EAEBC
static void ykgr_SafeString_assureTermination(void* p) {
}

/* WWHD glue (tools/verify/autoglue.py)
str 10043034 "Adanmae"
data l_HIO 1046EC60
data YkgrCB 1046EC88
data m_emitter 10475628
data m_aim_rate 1047562C
data m_path 10475630
data m_alpha_flag 10475652
data m_alpha 10475653
ext 028E8DE8 f32 PSVECSquareDistance(const cXyz*, const cXyz*)
ext 028256C0 void JPASetIndTexMtx(u32, s32, be<f32>*, s32)
*/
