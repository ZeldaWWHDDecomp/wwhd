/**
 * d_a_npc_ko1_d1.cpp (WWHD)
 * NPC - Joel & Zill (Outset Island)
 *
 * The GameCube decompilation (zeldaret/tww
 * src/d/actor/d_a_npc_ko1.cpp) has only "Nonmatching" stubs for this actor: the functions are
 * written from the WWHD code (cking.rpx) and verified against it, keeping the GameCube names.
 * Part D1: setStt and the state functions wait_*, walk_*, swim_*, attk_*, down_1.
 */
#define SAFESTRING_VTBL 0x1001C87C /* this TU's sead::SafeString vtable */
#include "d/actor/d_a_npc_ko1.h"

enum { fpcNm_ITEM_e = 0xA8, fpcNm_140_e = 0x140 };

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* dNpc_PathRun_c (d_npc) */
/* 0259E778 dNpc_PathRun_c::getPoint(u8): cXyz through a hidden result pointer (r4) */
static inline void dNpc_PathRun_getPoint(dNpc_PathRun_l* p, cXyz* out, u8 idx) { gabi::call(0x0259E778, p, out, idx); }
static inline bool dNpc_PathRun_setInfDrct(dNpc_PathRun_l* p, u32 path) { return gabi::call<bool>(0x0259E730, p, path); }
static inline bool dNpc_PathRun_setNearPathIndx(dNpc_PathRun_l* p, cXyz* pos, f32 r) { return gabi::call<bool>(0x0259EE7C, p, pos, r); }
/* g_Counter.mCounter0 (HD 0x101FF558) */
static inline u32 g_Counter0() { return gabi::load<u32>(0x101FF558); }

/* ---- other parts of d_a_npc_ko1 (called by address) ---- */
/* 022770F0 searchByID(fpc_ProcID) (the matcher calls it cLib_calcTimer<s>, see report) */
static inline fopAc_ac_c* ko1_searchByID(daNpc_Ko1_c* i_this, u32 id) { return gabi::call<fopAc_ac_c*>(0x022770F0, i_this, id); }
static inline bool ko1_chk_talk(daNpc_Ko1_c* i_this) { return gabi::call<bool>(0x02277070, i_this); }
static inline s32 ko1_chk_manzai_1(daNpc_Ko1_c* i_this) { return gabi::call<s32>(0x02277124, i_this); }
static inline bool ko1_chk_start_swim(daNpc_Ko1_c* i_this) { return gabi::call<bool>(0x02277DEC, i_this); }
static inline u32 ko1_get_crsActorID(daNpc_Ko1_c* i_this) { return gabi::call<u32>(0x02277EC0, i_this); }
/* chk_areaIn(f32 r, cXyz pos): the cXyz is passed as a pointer to a copy */
static inline bool ko1_chk_areaIn(daNpc_Ko1_c* i_this, f32 r, cXyz* pos) { return gabi::call<bool>(0x02277F58, i_this, r, pos); }
/* set_tgtPos(cXyz): cXyz result through a hidden pointer (r4), the argument a pointer to a copy (r5) */
static inline void ko1_set_tgtPos(daNpc_Ko1_c* i_this, cXyz* out, cXyz* pos) { gabi::call(0x02277C44, i_this, out, pos); }
static inline void ko1_clrSpd(daNpc_Ko1_c* i_this) { gabi::call(0x0227810C, i_this); }
static inline void ko1_setAnm(daNpc_Ko1_c* i_this) { gabi::call(0x02276D84, i_this); }
static inline void ko1_setAnm_NUM(daNpc_Ko1_c* i_this, s32 num, s32 tex) { gabi::call(0x02276D18, i_this, num, tex); }

/* ---- file statics ---- */
/* l_HIO.mChild[mType] (daNpc_Ko1_childHIO_c, 0x60 each at 0x104677E8): float parameters */
static inline f32 l_HIO_f(daNpc_Ko1_c* i_this, u32 off) { return gabi::load<f32>(0x104677E8 + (s32)i_this->mType * 0x60 + off); }
/* fopNpc_npc_c field_0x7d8[0] (u8 flag) */
static inline u8 get7D8(daNpc_Ko1_c* i_this) { return gabi::load<u8>(gabi::ea(i_this) + 0x7D8); }
static inline void set7D8(daNpc_Ko1_c* i_this, u8 v) { gabi::store<u8>(gabi::ea(i_this) + 0x7D8, v); }
/* mObjAcch: the water height (dBgS_Acch +0x1BC) */
static inline f32 acch_waterY(daNpc_Ko1_c* i_this) { return gabi::load<f32>(gabi::ea(i_this) + 0x60C); }
/* integer word copy of a cXyz at a guest address (bit-exact; works for a null actor + offset) */
static inline void copy_xyz(cXyz* dst, u32 src) {
    for (u32 i = 0; i < 12; i += 4) gabi::store<u32>(gabi::ea(dst) + i, gabi::load<u32>(src + i));
}
/* float copy (lfs/stfs) of a cXyz into a stack temporary passed by value */
static inline void fcopy_xyz(cXyz* dst, const cXyz& src) {
    dst->x = src.x;
    dst->y = src.y;
    dst->z = src.z;
}

/* 02278134 */
void daNpc_Ko1_c::setStt(s8 i_status) {
    WWHD_FUNC(0x02278134, void, this, i_status);
    fopAc_ac_c* actor = ko1_searchByID(this, m924);
    m9BE = 0;
    s8 oldStatus = mA13;
    mA13 = i_status;
    u32 status = (u32)(s32)i_status;
    switch (status) {
    case 1:
    case 2:
    case 0xC:
    case 0x16:
    case 0x1D:
        if (status == 1) {
            m9C2 = (s16)gabi::ftoi(cM_rndF(90.0f) + 90.0f);
        } else if (status == 2) {
            m9C4 = (s16)((g_Counter0() & 3) + 1);
        }
        if (oldStatus != 3) {
            m_jnt.mbTrn = 1;
            m9C8 = mInitialAngle.y;
            mA15 = 3;
            m9E6 = 0;
        }
        mA12 = 0;
        mA08 = 0;
        ko1_clrSpd(this);
        ko1_setAnm(this);
        break;
    case 3:
        mA15 = 1;
        m9E6 = 0;
        mA12 = 0;
        m_jnt.mbTrn = 1;
        mA08 = 0;
        ko1_clrSpd(this);
        mA0C = 0xFF;
        mA0D = 0xFF;
        mA14 = oldStatus;
        ko1_setAnm(this);
        break;
    case 4:
    case 0xB: {
        if (status == 4) {
            m9C2 = (s16)gabi::ftoi(cM_rndF(180.0f) + 180.0f);
        } else if (status == 0xB) {
            if (m8B4 != 0) {
                dNpc_PathRun_setInfDrct(&mPathRun, m8B4);
                dNpc_PathRun_setNearPathIndx(&mPathRun, &current.pos, 0.0f);
                m8B4 = 0;
            }
        }
        gabi::Local<cXyz> pnt;
        dNpc_PathRun_getPoint(&mPathRun, pnt, mPathRun.mIdx);
        m964.copy(*pnt);
        mA15 = 0;
        m9E6 = 1;
        mA12 = 0;
        mA08 = 1;
        m9DB = 0;
        gravity = -4.5f;
        m99C = l_HIO_f(this, 0x28);
        m9A4 = l_HIO_f(this, 0x2C);
        m9AC = l_HIO_f(this, 0x30);
        ko1_setAnm(this);
        break;
    }
    case 5:
        mA15 = 0;
        m9E6 = 0;
        mA12 = 0;
        mA08 = 0;
        ko1_clrSpd(this);
        ko1_setAnm(this);
        break;
    case 6:
    case 0xD:
    case 0x17: {
        if (status == 6 && mPathRun.mPath.get() != nullptr) {
            m8B4 = gabi::ea(mPathRun.mPath.get());
            dNpc_PathRun_setInfDrct(&mPathRun, 0);
        }
        fopAc_ac_c* player = dComIfGp_getLinkPlayer();
        copy_xyz(&m964, gabi::ea(player) + 0x314);
        mA15 = 1;
        m9E6 = 1;
        mA12 = 0;
        mA08 = 2;
        m9DB = 0;
        gravity = -4.5f;
        m99C = l_HIO_f(this, 0x38);
        m9A4 = l_HIO_f(this, 0x3C);
        m9AC = l_HIO_f(this, 0x40);
        ko1_setAnm(this);
        break;
    }
    case 7: {
        if (mPathRun.mPath.get() != nullptr) {
            m8B4 = gabi::ea(mPathRun.mPath.get());
            dNpc_PathRun_setInfDrct(&mPathRun, 0);
        }
        fopAc_ac_c* player = dComIfGp_getLinkPlayer();
        copy_xyz(&m964, gabi::ea(player) + 0x314);
        mA15 = 1;
        m9E6 = 1;
        m9A0 = -4.0f;
        m9A4 = 0.0f;
        m99C = 0.0f;
        mA12 = 0;
        mA08 = 3;
        m9DB = 0;
        speed.y = -4.0f;
        speedF = 0.0f;
        gravity = 0.0f;
        m9AC = l_HIO_f(this, 0x44);
        ko1_setAnm(this);
        break;
    }
    case 8: {
        if (m8B4 != 0) {
            dNpc_PathRun_setInfDrct(&mPathRun, m8B4);
            dNpc_PathRun_setNearPathIndx(&mPathRun, &current.pos, 0.0f);
            m8B4 = 0;
        }
        gabi::Local<cXyz> pnt;
        dNpc_PathRun_getPoint(&mPathRun, pnt, mPathRun.mIdx);
        m964.copy(*pnt);
        mA15 = 0;
        m9E6 = 1;
        mA12 = 0;
        mA08 = 3;
        speed.y = -4.0f;
        m9A0 = -4.0f;
        m9A4 = 0.0f;
        speedF = 0.0f;
        gravity = 0.0f;
        m9DB = 0;
        m99C = 0.0f;
        m9AC = l_HIO_f(this, 0x44);
        ko1_setAnm(this);
        break;
    }
    case 9:
    case 0x10:
    case 0x12:
    case 0x19:
        mA15 = 0;
        m9E6 = 0;
        speed.y = 10.0f;
        mA12 = 0;
        mA09 = mA08;
        mA08 = 4;
        speedF = -3.0f;
        gravity = -1.6f;
        m9DC = 1;
        m9A4 = 0.1f;
        ko1_setAnm(this);
        break;
    case 0xA:
    case 0xE:
    case 0x1A:
        mA15 = 1;
        m9E6 = 0;
        mA12 = 0;
        mA08 = 0;
        ko1_clrSpd(this);
        ko1_setAnm(this);
        break;
    case 0xF:
    case 0x18:
        m964.copy(mInitialPos);
        gravity = -4.5f;
        mA15 = 0;
        m9E6 = 1;
        mA12 = 0;
        mA08 = 1;
        m9DB = 0;
        m99C = l_HIO_f(this, 0x28);
        m9A4 = l_HIO_f(this, 0x2C);
        m9AC = l_HIO_f(this, 0x30);
        ko1_setAnm(this);
        break;
    case 0x11:
        if (actor == nullptr) /* JUT_ASSERT(2679, actor != NULL) */
            JUT_ASSERT_fail(STR(0x1001CCBC), 0xA77, STR(0x1001CCCC));
        copy_xyz(&m964, gabi::ea(actor) + 0x314);
        mA15 = 2;
        copy_xyz(&m958, gabi::ea(actor) + 0x314);
        m958.y = gabi::load<f32>(gabi::ea(actor) + 0x380); /* actor->eyePos.y */
        m9E6 = 1;
        mA12 = 0;
        mA08 = 2;
        m9DB = 0;
        gravity = -4.5f;
        m99C = l_HIO_f(this, 0x38);
        m9A4 = l_HIO_f(this, 0x3C);
        m9AC = l_HIO_f(this, 0x40);
        ko1_setAnm(this);
        break;
    case 0x13:
        if (actor != nullptr) {
            mA15 = 2;
            copy_xyz(&m958, gabi::ea(actor) + 0x314);
            m958.y = actor->eyePos.y;
            m9E6 = 0;
            mA12 = 0;
            mA08 = 0;
        } else {
            mA15 = 0;
            m9E6 = 0;
            mA12 = 0;
            mA08 = 0;
        }
        ko1_clrSpd(this);
        ko1_setAnm(this);
        break;
    case 0x14:
        mA0C = 0xFF;
        mA0D = 0xFF;
        mA14 = oldStatus;
        ko1_setAnm(this);
        break;
    case 0x1B:
    case 0x1C:
        m9C2 = (s16)gabi::ftoi(cM_rndF(180.0f) + 180.0f);
        m9C4 = (s16)cLib_getRndValue(3, 10);
        m9D5 = 0;
        ko1_setAnm(this);
        break;
    default:
        ko1_setAnm(this);
        break;
    }
}
VERIFY(0x02278134, &daNpc_Ko1_c::setStt);

/* 0227896C */
BOOL daNpc_Ko1_c::wait_1() {
    WWHD_FUNC(0x0227896C, BOOL, this);
    if (m9E5 != 0) {
        if (ko1_chk_talk(this)) {
            setStt(3);
        }
        return TRUE;
    }
    m9C8 = mInitialAngle.y;
    mA12 = 2;
    mA15 = 3;
    if (mA0F == 6) {
        if ((s8)m9D0 != 0) {
            cLib_calcTimer(&m9C4);
        }
        if (m9C4 == 0 || m9E4 != 0) {
            m9C4 = (s16)((g_Counter0() & 3) + 1);
            m9C2 = (s16)gabi::ftoi(cM_rndF(90.0f) + 90.0f);
            ko1_setAnm_NUM(this, 0, 1);
        }
    } else {
        if (m9E4 != 0) {
            m9BE = 0x3C;
        }
        if (cLib_calcTimer(&m9BE)) {
            mA15 = 1;
            return TRUE;
        }
        if (!cLib_calcTimer(&m9C2)) {
            ko1_setAnm_NUM(this, 6, 1);
        }
        m_jnt.mbTrn = 1;
    }
    return TRUE;
}
VERIFY(0x0227896C, &daNpc_Ko1_c::wait_1);

/* 02278AE0 */
BOOL daNpc_Ko1_c::wait_2() {
    WWHD_FUNC(0x02278AE0, BOOL, this);
    if ((s8)m9D0 != 0) {
        cLib_calcTimer(&m9C4);
        if (m9C4 == 0 || m9E4 != 0 || m9E5 != 0) {
            setStt(1);
        }
    }
    return TRUE;
}
VERIFY(0x02278AE0, &daNpc_Ko1_c::wait_2);

/* 02278B50 */
BOOL daNpc_Ko1_c::wait_3() {
    WWHD_FUNC(0x02278B50, BOOL, this);
    if ((s8)m9D0 != 0) {
        setStt(4);
    }
    return TRUE;
}
VERIFY(0x02278B50, &daNpc_Ko1_c::wait_3);

/* 02278B84 */
BOOL daNpc_Ko1_c::wait_4() {
    WWHD_FUNC(0x02278B84, BOOL, this);
    if (m9E5 != 0) {
        if (ko1_chk_talk(this)) {
            setStt(3);
        }
        return TRUE;
    }
    if (ko1_chk_start_swim(this)) {
        setStt(7);
        return TRUE;
    }
    gabi::Local<cXyz> pos;
    fcopy_xyz(pos, current.pos);
    if (ko1_chk_areaIn(this, l_HIO_f(this, 0x50), pos)) {
        mA12 = 2;
        return TRUE;
    }
    gabi::Local<cXyz> pos2;
    fcopy_xyz(pos2, m97C);
    if (!ko1_chk_areaIn(this, l_HIO_f(this, 0x58), pos2)) {
        setStt(0xB);
    } else {
        setStt(6);
    }
    return TRUE;
}
VERIFY(0x02278B84, &daNpc_Ko1_c::wait_4);

/* 02278CB4 */
BOOL daNpc_Ko1_c::wait_5(s8 i_nextStatus) {
    WWHD_FUNC(0x02278CB4, BOOL, this, i_nextStatus);
    if (get7D8(this) == 1) {
        set7D8(this, 2);
        setStt(0x14);
        mA15 = 1;
        m9E6 = 0;
        mA12 = 0;
        mA08 = 0;
        ko1_clrSpd(this);
        return TRUE;
    }
    gabi::Local<cXyz> pos;
    fcopy_xyz(pos, m97C);
    if (ko1_chk_areaIn(this, l_HIO_f(this, 0x48), pos)) {
        setStt(i_nextStatus);
    }
    return TRUE;
}
VERIFY(0x02278CB4, &daNpc_Ko1_c::wait_5);

/* 02278D7C */
BOOL daNpc_Ko1_c::wait_6() {
    WWHD_FUNC(0x02278D7C, BOOL, this);
    if (m9E5 != 0) {
        if (ko1_chk_talk(this) && ko1_chk_manzai_1(this)) {
            setStt(3);
        }
        return TRUE;
    }
    if (get7D8(this) == 1) {
        set7D8(this, 2);
        setStt(0x14);
        return TRUE;
    }
    gabi::Local<cXyz> pos;
    fcopy_xyz(pos, current.pos);
    if (ko1_chk_areaIn(this, l_HIO_f(this, 0x50), pos)) {
        mA12 = 2;
        return TRUE;
    }
    gabi::Local<cXyz> pos2;
    fcopy_xyz(pos2, m97C);
    if (!ko1_chk_areaIn(this, l_HIO_f(this, 0x4C), pos2)) {
        setStt(0xF);
    } else {
        setStt(0xD);
    }
    return TRUE;
}
VERIFY(0x02278D7C, &daNpc_Ko1_c::wait_6);

/* 02278EC0 */
BOOL daNpc_Ko1_c::wait_7() {
    WWHD_FUNC(0x02278EC0, BOOL, this);
    fopAc_ac_c* actor = ko1_searchByID(this, m924);
    if (actor == nullptr) /* JUT_ASSERT(2927, actor != NULL) */
        JUT_ASSERT_fail(STR(0x1001CCE0), 0xB6F, STR(0x1001CCF0));
    if (m9E5 != 0) {
        if (ko1_chk_talk(this) && ko1_chk_manzai_1(this)) {
            setStt(3);
        }
        return TRUE;
    }
    if (get7D8(this) == 1) {
        set7D8(this, 2);
        setStt(0x14);
        mA15 = 1;
        m9E6 = 0;
        mA12 = 0;
        mA08 = 0;
        ko1_clrSpd(this);
        return TRUE;
    }
    gabi::Local<cXyz> pos;
    fcopy_xyz(pos, current.pos);
    if (ko1_chk_areaIn(this, l_HIO_f(this, 0x50), pos)) {
        mA15 = 1;
    } else {
        mA15 = 2;
        copy_xyz(&m958, gabi::ea(actor) + 0x314);
        m958.y = gabi::load<f32>(gabi::ea(actor) + 0x380); /* actor->eyePos.y */
    }
    /* distance in XZ from this actor to the partner */
    cXyz_mi(gabi::at<cXyz>(gabi::ea(actor) + 0x314), pos, &current.pos);
    gabi::Local<cXyz> xz;
    xz->x = pos->x;
    xz->y = 0.0f;
    xz->z = pos->z;
    f32 dist = std_sqrtf(PSVECSquareMag(xz));
    if (dist < l_HIO_f(this, 0x50)) {
        mA12 = 2;
    } else {
        setStt(0x11);
    }
    return TRUE;
}
VERIFY(0x02278EC0, &daNpc_Ko1_c::wait_7);

/* 022790E4 */
BOOL daNpc_Ko1_c::wait_9() {
    WWHD_FUNC(0x022790E4, BOOL, this);
    if (m9E5 != 0) {
        if (ko1_chk_talk(this)) {
            setStt(3);
        }
        return TRUE;
    }
    gabi::Local<cXyz> pos;
    fcopy_xyz(pos, current.pos);
    if (ko1_chk_areaIn(this, l_HIO_f(this, 0x50), pos)) {
        mA12 = 2;
        return TRUE;
    }
    gabi::Local<cXyz> pos2;
    fcopy_xyz(pos2, m97C);
    if (!ko1_chk_areaIn(this, l_HIO_f(this, 0x4C), pos2)) {
        setStt(0x18);
    } else {
        setStt(0x17);
    }
    return TRUE;
}
VERIFY(0x022790E4, &daNpc_Ko1_c::wait_9);

/* 022791F4 */
BOOL daNpc_Ko1_c::wait_a() {
    WWHD_FUNC(0x022791F4, BOOL, this);
    fopAc_ac_c* actor = ko1_searchByID(this, m924);
    if (actor == nullptr) /* JUT_ASSERT(3015, actor != NULL) */
        JUT_ASSERT_fail(STR(0x1001CD04), 0xBC7, STR(0x1001CD14));
    if (m9E5 != 0) {
        if (ko1_chk_talk(this) && ko1_chk_manzai_1(this)) {
            setStt(3);
        }
        return TRUE;
    }
    if (get7D8(this) == 1) {
        set7D8(this, 2);
        setStt(0x14);
        return TRUE;
    }
    mA12 = 2;
    if (m9E4 != 0) {
        m9BE = 0x3C;
    }
    if (cLib_calcTimer(&m9BE)) {
        mA15 = 1;
        return TRUE;
    }
    mA15 = 3;
    m9C8 = mInitialAngle.y;
    m_jnt.mbTrn = 1;
    return TRUE;
}
VERIFY(0x022791F4, &daNpc_Ko1_c::wait_a);

/* 02279300 */
BOOL daNpc_Ko1_c::walk_1() {
    WWHD_FUNC(0x02279300, BOOL, this);
    m9DB = 0;
    gabi::Local<cXyz> pnt;
    dNpc_PathRun_getPoint(&mPathRun, pnt, mPathRun.mIdx);
    m964.copy(*pnt);
    gabi::Local<cXyz> pos;
    fcopy_xyz(pos, m97C);
    if (ko1_chk_areaIn(this, l_HIO_f(this, 0x54), pos)) {
        setStt(6);
    } else if (!cLib_calcTimer(&m9C2)) {
        setStt(5);
    }
    return TRUE;
}
VERIFY(0x02279300, &daNpc_Ko1_c::walk_1);

/* 022793C8 */
BOOL daNpc_Ko1_c::walk_2(s8 i_status1, s8 i_status2) {
    WWHD_FUNC(0x022793C8, BOOL, this, i_status1, i_status2);
    if (m9DB != 0) {
        setStt(i_status1);
        return TRUE;
    }
    if (get7D8(this) == 1) {
        setStt(0xE);
        return TRUE;
    }
    gabi::Local<cXyz> pos;
    fcopy_xyz(pos, m97C);
    if (ko1_chk_areaIn(this, l_HIO_f(this, 0x48), pos)) {
        setStt(i_status2);
        return TRUE;
    }
    m964.copy(mInitialPos);
    return TRUE;
}
VERIFY(0x022793C8, &daNpc_Ko1_c::walk_2);

/* 0227949C */
BOOL daNpc_Ko1_c::walk_3() {
    WWHD_FUNC(0x0227949C, BOOL, this);
    if (m9DB != 0) {
        setStt(4);
        return TRUE;
    }
    gabi::Local<cXyz> pos;
    fcopy_xyz(pos, m97C);
    if (ko1_chk_areaIn(this, l_HIO_f(this, 0x54), pos)) {
        setStt(6);
        return TRUE;
    }
    gabi::Local<cXyz> pnt;
    dNpc_PathRun_getPoint(&mPathRun, pnt, mPathRun.mIdx);
    m964.copy(*pnt);
    if (ko1_chk_start_swim(this)) {
        setStt(8);
    }
    return TRUE;
}
VERIFY(0x0227949C, &daNpc_Ko1_c::walk_3);

/* 02279578 */
BOOL daNpc_Ko1_c::swim_1() {
    WWHD_FUNC(0x02279578, BOOL, this);
    f32 depth = acch_waterY(this) - current.pos.y;
    if (depth < 49.0f) {
        m9A0 = -4.0f;
        if (m9DB == 0) {
            speedF = 8.0f;
        }
    } else if (depth > 58.0f) {
        m9A0 = 4.0f;
    }
    if (!ko1_chk_start_swim(this)) {
        setStt(6);
        return TRUE;
    }
    gabi::Local<cXyz> pos;
    fcopy_xyz(pos, m97C);
    if (!ko1_chk_areaIn(this, l_HIO_f(this, 0x58), pos)) {
        setStt(8);
        return TRUE;
    }
    m9DB = 0;
    fopAc_ac_c* player = dComIfGp_getLinkPlayer();
    copy_xyz(&m964, gabi::ea(player) + 0x314);
    return TRUE;
}
VERIFY(0x02279578, &daNpc_Ko1_c::swim_1);

/* 022796B4 */
BOOL daNpc_Ko1_c::swim_2() {
    WWHD_FUNC(0x022796B4, BOOL, this);
    f32 depth = acch_waterY(this) - current.pos.y;
    if (depth < 49.0f) {
        m9A0 = -4.0f;
        if (m9DB == 0) {
            speedF = 8.0f;
        }
    } else if (depth > 58.0f) {
        m9A0 = 4.0f;
    }
    if (!ko1_chk_start_swim(this)) {
        setStt(0xB);
        return TRUE;
    }
    gabi::Local<cXyz> pos;
    fcopy_xyz(pos, m97C);
    if (ko1_chk_areaIn(this, l_HIO_f(this, 0x54), pos)) {
        setStt(7);
        return TRUE;
    }
    m9DB = 0;
    gabi::Local<cXyz> pnt;
    dNpc_PathRun_getPoint(&mPathRun, pnt, mPathRun.mIdx);
    m964.copy(*pnt);
    return TRUE;
}
VERIFY(0x022796B4, &daNpc_Ko1_c::swim_2);

/* 022797FC */
BOOL daNpc_Ko1_c::attk_1() {
    WWHD_FUNC(0x022797FC, BOOL, this);
    m9DB = 0;
    u32 id = ko1_get_crsActorID(this);
    m930 = id;
    if (id != fpcM_ERROR_PROCESS_ID_e) {
        fopAc_ac_c* actor = ko1_searchByID(this, id);
        if (actor != nullptr && fpcM_GetName(actor) == fpcNm_ITEM_e) {
            setStt(9);
            return TRUE;
        }
    }
    gabi::Local<cXyz> pos;
    fcopy_xyz(pos, m97C);
    if (!ko1_chk_areaIn(this, l_HIO_f(this, 0x58), pos)) {
        setStt(0xB);
        return TRUE;
    }
    fopAc_ac_c* player = dComIfGp_getLinkPlayer();
    gabi::Local<cXyz> ppos;
    fcopy_xyz(ppos, player->current.pos);
    ko1_set_tgtPos(this, pos, ppos);
    m964.copy(*pos);
    if (ko1_chk_start_swim(this)) {
        setStt(7);
    }
    return TRUE;
}
VERIFY(0x022797FC, &daNpc_Ko1_c::attk_1);

/* 0227992C */
BOOL daNpc_Ko1_c::attk_2(s8 i_status1, s8 i_status2) {
    WWHD_FUNC(0x0227992C, BOOL, this, i_status1, i_status2);
    m9DB = 0;
    if (get7D8(this) == 1) {
        setStt(0xE);
        return TRUE;
    }
    u32 id = ko1_get_crsActorID(this);
    m930 = id;
    if (id != fpcM_ERROR_PROCESS_ID_e) {
        fopAc_ac_c* actor = ko1_searchByID(this, id);
        if (actor != nullptr && fpcM_GetName(actor) == fpcNm_ITEM_e) {
            setStt(i_status1);
            return TRUE;
        }
    }
    gabi::Local<cXyz> pos;
    fcopy_xyz(pos, m97C);
    if (!ko1_chk_areaIn(this, l_HIO_f(this, 0x4C), pos)) {
        setStt(i_status2);
        return TRUE;
    }
    fopAc_ac_c* player = dComIfGp_getLinkPlayer();
    copy_xyz(&m964, gabi::ea(player) + 0x314);
    return TRUE;
}
VERIFY(0x0227992C, &daNpc_Ko1_c::attk_2);

/* 02279A58 */
BOOL daNpc_Ko1_c::attk_3() {
    WWHD_FUNC(0x02279A58, BOOL, this);
    fopAc_ac_c* actor = ko1_searchByID(this, m924);
    if (actor == nullptr) /* JUT_ASSERT(3286, actor != NULL) */
        JUT_ASSERT_fail(STR(0x1001CD38), 0xCD6, STR(0x1001CD48));
    m9DB = 0;
    if (get7D8(this) == 1) {
        setStt(0x13);
        return TRUE;
    }
    u32 id = ko1_get_crsActorID(this);
    m930 = id;
    if (id != fpcM_ERROR_PROCESS_ID_e) {
        fopAc_ac_c* target = ko1_searchByID(this, id);
        if (target != nullptr && (fpcM_GetName(target) == fpcNm_140_e || fpcM_GetName(target) == fpcNm_ITEM_e)) {
            setStt(0x12);
            return TRUE;
        }
    }
    gabi::Local<cXyz> apos;
    u32 a = gabi::ea(actor);
    apos->x = gabi::load<f32>(a + 0x314);
    apos->y = gabi::load<f32>(a + 0x318);
    apos->z = gabi::load<f32>(a + 0x31C);
    gabi::Local<cXyz> tgt;
    ko1_set_tgtPos(this, tgt, apos);
    m964.copy(*tgt);
    m958.copy(*tgt);
    m958.y = gabi::load<f32>(a + 0x380); /* actor->eyePos.y */
    return TRUE;
}
VERIFY(0x02279A58, &daNpc_Ko1_c::attk_3);

/* 02279B90 */
BOOL daNpc_Ko1_c::down_1(s8 i_status) {
    WWHD_FUNC(0x02279B90, BOOL, this, i_status);
    if (m9DB != 0) {
        setStt(i_status);
    }
    return TRUE;
}
VERIFY(0x02279B90, &daNpc_Ko1_c::down_1);
