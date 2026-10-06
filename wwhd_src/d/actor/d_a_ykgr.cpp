/**
 * d_a_ykgr.cpp (WWHD)
 * Heat haze (Dragon Roost mountain "kagerou"): one screen-space particle emitter whose strength
 * follows the player's distance to a path.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_ykgr.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define ACT_VTBL 0x10043058     /* HD: daYkgr_c vtable */
#define SAFESTRING_VTBL 0x10043040
#define HIO_VTBL 0x10043068
#define CB_VTBL 0x100430A4      /* dPa_YkgrPcallBack vtable */

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 028E8DE8 PSVECSquareDistance(const Vec*, const Vec*) */
static inline f32 PSVECSquareDistance(const cXyz* a, const cXyz* b) { return gabi::call<f32>(0x028E8DE8, a, b); }
/* 025B7D90 dSv_player_collect_c::isSymbol(u8) (save info + 0xD4) */
static inline BOOL dComIfGs_isSymbol(u8 no) { return gabi::call<BOOL>(0x025B7D90, gabi::load<u32>(0x101F84DC) + 0xD4, no); }
/* 028249B0 JPASetRMtxTVecfromMtx(const Mtx, Mtx rot, Vec* trans) (JPABaseEmitter::setGlobalRTMatrix inline) */
static inline void JPASetRMtxTVecfromMtx(Mtx34* m, u32 rot, u32 trans) { gabi::call(0x028249B0, m, rot, trans); }
/* camera_class* dComIfGp_getCamera(0) at play+0x5AF8: eye +0xDC, angle x +0x234, angle y +0x236 */
static inline u32 dComIfGp_getCamera0() { return gabi::load<u32>(dComIfGp_ea() + 0x5AF8); }
/* sead::SafeString operator== (HD inline; as d_a_mo2.h SafeString_eq): cstr() of both operands
 * through the vtable (slot 0x14; the left one twice), pointer compare, bounded strcmp */
static bool SafeString_eq(const char* a, const char* b, u32 vtbl) {
    gabi::Local<SafeString> sa;
    sa->mStringTop = gabi::ea(a);
    sa->__vtbl = vtbl;
    gabi::Local<SafeString> sb;
    sb->mStringTop = gabi::ea(b);
    sb->__vtbl = vtbl;
    gabi::call_ptr(gabi::load<u32>(sa->__vtbl + 0x14), sa.get());
    gabi::call_ptr(gabi::load<u32>(sa->__vtbl + 0x14), sa.get());
    gabi::call_ptr(gabi::load<u32>(sb->__vtbl + 0x14), sb.get());
    u32 pa = sa->mStringTop;
    u32 pb = sb->mStringTop;
    if (pa == pb) return true;
    for (u32 i = 0; i < 0x40001; i++) {
        u8 ca = gabi::load<u8>(pa + i);
        if (ca != gabi::load<u8>(pb + i)) return false;
        if (ca == 0) return true;
    }
    return false;
}

struct daYkgr_HIO_c {
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ be<s8> mNo;
    /* 0x05 */ u8 _05[3];
    /* 0x08 */ be<s32> m08;
    /* 0x0C */ be<s32> m0C;
    /* 0x10 */ be<f32> m10;
    /* 0x14 */ be<f32> m14;
    /* 0x18 */ be<f32> m18;
    /* 0x1C */ be<f32> m1C;
    /* 0x20 */ be<f32> m20;
    /* 0x24 */ be<f32> m24;
};
static daYkgr_HIO_c& l_HIO() { return *gabi::at<daYkgr_HIO_c>(0x1046EC60); }

struct dPa_YkgrPcallBack {
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ be<f32> m04;
    /* 0x08 */ be<f32> m08;
    /* 0x0C */ be<f32> m0C;
    /* 0x10 */ be<f32> m10;
    /* 0x14 */ be<f32> m14;
    /* 0x18 */ be<f32> m18;
    /* 0x1C */ be<s8> m1C;
    void setParam(f32 arg1);
};
static dPa_YkgrPcallBack* YkgrCB() { return gabi::at<dPa_YkgrPcallBack>(0x1046EC88); }

/* static members */
static be<u32>& m_emitter() { return *gabi::at<be<u32>>(0x10475628); }
static be<f32>& m_aim_rate() { return *gabi::at<be<f32>>(0x1047562C); }
static be<u32>& m_path() { return *gabi::at<be<u32>>(0x10475630); }
static be<u8>& m_alpha_flag() { return *gabi::at<be<u8>>(0x10475652); }
static be<u8>& m_alpha() { return *gabi::at<be<u8>>(0x10475653); }

struct daYkgr_c : fopAc_ac_c {
    f32 getPosRate();
    cPhs_State _create();

    /* 0x3AC */ request_of_phase_process_class mPhase;
    /* 0x3B4 */ Mtx34 mMtx;
    /* 0x3E4 */ u8 m3E4[4];
    /* 0x3E8 */ be<f32> m2CC;
    /* 0x3EC */ be<f32> m2D0;
};
WWHD_OFFSET(daYkgr_c, m2D0, 0x3EC);
WWHD_SIZE(daYkgr_c, 0x3F0);

/* 024EA58C. HD: the s8 m1C is the truncated parameter, and the result is halved once more */
void dPa_YkgrPcallBack::setParam(f32 arg1) {
    WWHD_FUNC(0x024EA58C, void, this, arg1);
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
VERIFY(0x024EA58C, &dPa_YkgrPcallBack::setParam);

/* 024EA984 */
f32 daYkgr_c::getPosRate() {
    WWHD_FUNC(0x024EA984, f32, this);
    if (m_path() == 0) {
        return 0.0f;
    }
    f32 fVar4 = 3.4028235e38f; /* FLOAT_MAX */
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    u32 path = m_path();
    f32 px = player->current.pos.x;
    f32 pz = player->current.pos.z;
    u32 num = gabi::load<u16>(path + 0);
    u32 pnt = gabi::load<u32>(path + 8);
    gabi::Local<cXyz> sp30;
    gabi::Local<cXyz> sp24;
    for (; num != 0; num--, pnt += 0x10) {
        /* cXyz::absXZ */
        sp24->x = gabi::load<f32>(pnt + 4);
        sp24->y = 0.0f;
        sp24->z = gabi::load<f32>(pnt + 0xC);
        sp30->x = px;
        sp30->y = 0.0f;
        sp30->z = pz;
        f32 abs = std_sqrtf(PSVECSquareDistance(sp30, sp24));
        if (abs < fVar4) {
            fVar4 = abs;
        }
    }
    f32 hi = l_HIO().m20;
    f32 lo = l_HIO().m24;
    if (fVar4 > hi) {
        fVar4 = hi;
    } else if (fVar4 < lo) {
        fVar4 = lo;
    }
    return 1.0f - (fVar4 - lo) / (hi - lo);
}
VERIFY(0x024EA984, &daYkgr_c::getPosRate);

/* 024EA6A0 daYkgr_c::_create (daYkgrCreate). HD: "Adanmae" compared as sead::SafeString;
 * no HIO child; the inline start()/stop() of the fresh emitter test m_emitter again */
cPhs_State daYkgr_c::_create() {
    WWHD_FUNC(0x024EA6A0, cPhs_State, this);
    u32 prm = mParameters;
    u8 uVar4 = (prm >> 0x14) & 0xF;
    /* fopAcM_ct(this, daYkgr_c) */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            fopAc_ac_c_ct(this);
            __vtbl = ACT_VTBL;
        }
        prm = mParameters;
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    u8 pathIndex = (prm >> 8) & 0xFF;
    u32 path = 0;
    if (pathIndex != 0xFF) {
        path = gabi::ea(dPath_GetRoomPath(pathIndex, fopAcM_GetRoomNo(this)));
    }
    m_path() = path;

    if (SafeString_eq(STR(0x10043034) /* "Adanmae" */, gabi::at<const char>(dComIfGp_ea() + 0x5134), SAFESTRING_VTBL) &&
        dComIfGs_isSymbol(1 /* dSymbol_DIN_e */)) {
        return 3; /* cPhs_STOP_e */
    }

    if (m_emitter() == 0) {
        fopAc_ac_c* player = dComIfGp_getPlayer(0);
        current.pos.copy(player->current.pos);
        /* dComIfGp_particle_setProjection(dPa_name::ID_AK_SP_MTDRAGONKAGEROU, &current.pos) */
        JPABaseEmitter* emitter = dPa_control_set(dComIfGp_getParticle(), 4, 0xC06D, &current.pos, nullptr, nullptr, 0xFF,
                                                  nullptr, -1, nullptr, nullptr, nullptr);
        m_emitter() = gabi::ea(emitter);
        if (emitter == nullptr) {
            return cPhs_ERROR_e;
        }
        gabi::store<u32>(gabi::ea(emitter) + 0x1E8, gabi::ea(YkgrCB())); /* setParticleCallBackPtr */
        YkgrCB()->setParam(-3.0f);
        fopAcM_setStageLayer(this);
        if (uVar4 == 1) {
            u32 e = m_emitter();
            m_alpha() = 0xFF;                      /* setAlpha */
            gabi::store<u8>(e + 0x247, 0xFF);      /* setGlobalAlpha */
            m2CC = 0.5f;
            m2D0 = 0.0f;
            if (m_emitter() != 0) m_alpha_flag() = 1; /* start() */
        } else {
            u32 e = m_emitter();
            m_alpha() = 0;
            gabi::store<u8>(e + 0x247, 0);
            m2CC = 0.5f;
            m2D0 = 0.0f;
            if (m_emitter() != 0) m_alpha_flag() = 0; /* stop() */
        }
    } else {
        m_alpha_flag() = (uVar4 == 1) ? 1 : 0; /* start() / stop() */
        return 3; /* cPhs_STOP_e */
    }
    return cPhs_COMPLEATE_e;
}
VERIFY(0x024EA6A0, &daYkgr_c::_create);

/* 024EA97C */
static BOOL daYkgrDelete(void* v_this) {
    WWHD_FUNC(0x024EA97C, BOOL, v_this);
    return true; /* _delete() */
}
VERIFY(0x024EA97C, daYkgrDelete);

/* 024EAAE8 daYkgrExecute (_execute inlined) */
static BOOL daYkgrExecute(void* v_this) {
    WWHD_FUNC(0x024EAAE8, BOOL, v_this);
    daYkgr_c* i_this = (daYkgr_c*)v_this;
    cLib_addCalc2(&i_this->m2CC, m_aim_rate(), 0.25f, 0.05f);
    cLib_addCalc2(&m_aim_rate(), l_HIO().m1C, 0.25f, 0.05f);
    f32 rate = i_this->getPosRate();
    cLib_addCalc2(&i_this->m2D0, rate, 0.25f, 0.05f);

    f32 fVar1 = gabi::fmadds(0.5f, i_this->m2CC, 0.5f * i_this->m2D0);
    f32 tmp2 = gabi::fmadds(fVar1, l_HIO().m18, (1.0f - fVar1) * l_HIO().m14);
    YkgrCB()->setParam(tmp2);

    u8 flag = m_alpha_flag();
    s32 alpha = m_alpha();
    if (flag == 0) {
        if (alpha != 0) {
            s32 step = l_HIO().m0C;
            if (alpha > step) {
                m_alpha() = (u8)(alpha - step);
            } else {
                m_alpha() = 0;
            }
        }
    } else if ((u32)alpha < 0xFF) {
        s32 step = l_HIO().m0C;
        if (alpha < 0xFF - step) {
            m_alpha() = (u8)(alpha + step);
        } else {
            m_alpha() = 0xFF;
        }
    }
    return true;
}
VERIFY(0x024EAAE8, daYkgrExecute);

/* 024EAC68 daYkgrDraw (_draw and set_mtx inlined) */
static BOOL daYkgrDraw(void* v_this) {
    WWHD_FUNC(0x024EAC68, BOOL, v_this);
    daYkgr_c* i_this = (daYkgr_c*)v_this;
    if (m_alpha() == 0) {
        return true;
    }
    u32 camera = dComIfGp_getCamera0();
    i_this->current.pos.copy(*gabi::at<cXyz>(camera + 0xDC)); /* fopCamM_GetEye_p */
    i_this->current.angle.y = gabi::load<s16>(camera + 0x236);  /* fopCamM_GetAngleY */
    i_this->current.angle.x = gabi::load<s16>(camera + 0x234);  /* fopCamM_GetAngleX */
    mDoMtx_stack_c::transS(i_this->current.pos.x, i_this->current.pos.y, i_this->current.pos.z);
    mDoMtx_stack_c::YrotM(i_this->current.angle.y);
    mDoMtx_XrotM(mDoMtx_stack_c::get(), i_this->current.angle.x);
    PSMTXCopy(mDoMtx_stack_c::get(), &i_this->mMtx);
    u32 e = m_emitter();
    if (e != 0) {
        JPASetRMtxTVecfromMtx(&i_this->mMtx, e + 0x1F0, e + 0x22C); /* setGlobalRTMatrix */
        u32 e2 = m_emitter();
        gabi::store<u8>(e2 + 0x247, m_alpha());                     /* setGlobalAlpha */
    }
    return true;
}
VERIFY(0x024EAC68, daYkgrDraw);

/* 024EAD4C: dPa_YkgrPcallBack::draw. HD: one call (GX2 indirect matrix + alpha input) with the
 * callback's matrix (m04) and scale exponent (m1C) */
static void YkgrPcallBack_draw(dPa_YkgrPcallBack* cb, u32 emitter, u32 ctx) {
    WWHD_FUNC(0x024EAD4C, void, cb, emitter, ctx);
    gabi::call(0x028256C0, ctx, 0, &cb->m04, (s32)(s8)cb->m1C);
}
VERIFY(0x024EAD4C, YkgrPcallBack_draw);

/* 024EAD68 */
static void __sinit_d_a_ykgr_cpp() {
    WWHD_FUNC(0x024EAD68, void, (u32)0);
    sinit_header_statics_z(0x1046EC54, 0x101D3C3C, 0x1046ECA8);
    /* daYkgr_HIO_c::daYkgr_HIO_c (l_HIO) and dPa_YkgrPcallBack::dPa_YkgrPcallBack (YkgrCB), interleaved */
    daYkgr_HIO_c& h = l_HIO();
    dPa_YkgrPcallBack* cb = YkgrCB();
    h.mNo = 0;
    h.m10 = -16.0f;
    h.m14 = -5.0f;
    h.m18 = -3.0f;
    h.m1C = 0.0f;
    cb->m1C = 1;
    h.m20 = 1500.0f;
    h.__vtbl = HIO_VTBL;
    h.m08 = 0xFF;
    h.m24 = 500.0f;
    h.m0C = 3;
    cb->__vtbl = CB_VTBL;
    cb->m04 = 0.5f;
    cb->m08 = 0.0f;
    cb->m0C = 0.0f;
    cb->m10 = 0.0f;
    cb->m14 = 0.5f;
    cb->m18 = 0.0f;
    __register_global_object(0x101D3C60);
}
VERIFY(0x024EAD68, __sinit_d_a_ykgr_cpp);

/* 024EAEBC: deleting destructor of a class with a trivial destructor */
static void trivial_dt(void* p, s32 flags) {
    WWHD_FUNC(0x024EAEBC, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x024EAEBC, trivial_dt);

/* 024EAED0 */
static BOOL daYkgrIsDelete(void* i_this) {
    WWHD_FUNC(0x024EAED0, BOOL, i_this);
    return TRUE;
}
VERIFY(0x024EAED0, daYkgrIsDelete);

/* 024EAED8, 024EAEDC, 024EAEE0: dPa_YkgrPcallBack's empty virtuals (vtable 0x100430A4 slots) */
static void YkgrPcallBack_empty0(void* p) {
    WWHD_FUNC(0x024EAED8, void, p);
}
VERIFY(0x024EAED8, YkgrPcallBack_empty0);
static void YkgrPcallBack_empty1(void* p) {
    WWHD_FUNC(0x024EAEDC, void, p);
}
VERIFY(0x024EAEDC, YkgrPcallBack_empty1);
static void YkgrPcallBack_empty2(void* p) {
    WWHD_FUNC(0x024EAEE0, void, p);
}
VERIFY(0x024EAEE0, YkgrPcallBack_empty2);

/* 024EAEE4: daYkgr_c deleting destructor */
static void daYkgr_c_dt(daYkgr_c* i_this, s32 flags) {
    WWHD_FUNC(0x024EAEE4, void, i_this, flags);
    if (i_this != nullptr) {
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x024EAEE4, daYkgr_c_dt);

/* 024EAF38: dPa_YkgrPcallBack deleting destructor (trivial) */
static void YkgrPcallBack_dt(void* p, s32 flags) {
    WWHD_FUNC(0x024EAF38, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x024EAF38, YkgrPcallBack_dt);

/* ---- leftover functions of the translation unit ---- */

/* 024EAF4C sead::SafeStringBase<char>::assureTerminationImpl_ (empty); vtable slot 10043054, after the destructor 024EAEBC */
static void ykgr_SafeString_assureTermination(void* p) {
    WWHD_FUNC(0x024EAF4C, void, p);
}
VERIFY(0x024EAF4C, ykgr_SafeString_assureTermination);
