/**
 * d_a_wbird.cpp (WWHD)
 * Wind Waker "wind bird" (the wind direction demo after playing the Wind's Requiem)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_wbird.cpp) to the WWHD layout and verified against cking.rpx.
 */
#include "bindings.h"

#define WBIRD_VTBL 0x10042A2C

enum {
    JA_SE_TAKT_WIND_DISP = 0x872,
    JA_SE_TAKT_WIND_CANCEL = 0x873,
    JA_SE_TAKT_WIND_DECIDE = 0x875,
    JA_SE_TAKT_WIND_DEMO = 0x876,
    JA_SE_TAKT_WIND_END = 0x877,
    JA_SE_SYS_WTAKT_WIND_AMB = 0x1074,
};

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline void mDoAud_seStart_1(u32 id) { gabi::call(0x025E1988, id); } /* HD: seStart(id), no position */
static inline cXyz* dKyw_get_wind_vec() { return gabi::call<cXyz*>(0x0257DAA8); }
static inline void dKyw_custom_windpower(f32 p) { gabi::call(0x0257E594, p); }
static inline void dKyw_tact_wind_set_go() { gabi::call(0x0257E52C); }
static inline BOOL fopAcM_orderChangeEvent(fopAc_ac_c* a, const char* name, u16 flag, u16 hind) {
    return gabi::call<BOOL>(0x025D78FC, a, name, flag, hind);
}
/* c_angle / c_sxyz (out of line in HD; constructors allocate when this == NULL) */
static inline void cSGlobe_ct(void* g, const cXyz* v) { gabi::call(0x02007324, g, v); }           /* cSGlobe(const cXyz&) */
static inline void cSAngle_ct_copy(be<s16>* a, const be<s16>* b) { gabi::call(0x02006644, a, b); } /* cSAngle(const cSAngle&) */
static inline void cSAngle_sub_s(const be<s16>* a, be<s16>* res, s16 v) { gabi::call(0x02006908, a, res, v); } /* operator-(s16) */
static inline void cSAngle_addeq(be<s16>* a, const be<s16>* b) { gabi::call(0x020068CC, a, b); }
static inline void cSAngle_subeq(be<s16>* a, const be<s16>* b) { gabi::call(0x020068E0, a, b); }
static inline f32 cSAngle_Sin(const be<s16>* a) { return gabi::call<f32>(0x02006814, a); }
static inline f32 cSAngle_Cos(const be<s16>* a) { return gabi::call<f32>(0x02006838, a); }
#define cSAngle__0 gabi::at<be<s16>>(0x101FF354)
#define cSAngle__90 gabi::at<be<s16>>(0x101FF358)
/* cM_ssin/cM_scos: the sin/cos table at 0x104A44F8 ({sin, cos} per 8 steps); HD indexes it with
 * the u16 angle */
static inline f32 cM_ssin_u(u16 a) { return gabi::load<f32>(0x104A44F8 + (a >> 3) * 8); }
static inline f32 cM_scos_u(u16 a) { return gabi::load<f32>(0x104A44F8 + (a >> 3) * 8 + 4); }

/* cSGlobe: {f32 R, cSAngle V, cSAngle U} */
struct cSGlobe_l {
    /* 0x0 */ be<f32> mRadius;
    /* 0x4 */ be<s16> mAzimuth;   /* V */
    /* 0x6 */ be<s16> mInclination; /* U */
};

struct daWbird_c : fopAc_ac_c {
    void setAction(u8 action) { mAction = action; }
    void calcMtx();
    void setStartPos();
    BOOL CreateInit();
    cPhs_State create();
    void actionEnd();
    void actionMove();
    void actionSelect();

    /* 0x3AC */ u8 _3AC[0x3B8 - 0x3AC];   /* GameCube 0x290 */
    /* 0x3B8 */ be<u8> mAction;
    /* 0x3B9 */ be<u8> field_0x29D;
    /* 0x3BA */ be<s16> field_0x29E;
    /* 0x3BC */ be<f32> field_0x2A0;
    /* 0x3C0 */ be<s16> mAngle;
    /* 0x3C2 */ be<s16> mEventIdx;
};
WWHD_OFFSET(daWbird_c, mAction, 0x3B8);
WWHD_OFFSET(daWbird_c, mEventIdx, 0x3C2);
WWHD_SIZE(daWbird_c, 0x3C4);

static inline fopAc_ac_c* player_l() { return gabi::at<fopAc_ac_c>(gabi::load<u32>(dComIfGp_ea() + PLAY_PLAYER)); }

/* 024DE930 */
void daWbird_c::calcMtx() {
    WWHD_FUNC(0x024DE930, void, this);
    f32 z = current.pos.z, y = current.pos.y, x = current.pos.x;
    eyePos.z = z;
    gabi::store<f32>(gabi::ea(this) + 0x394, y); /* attention_info.position */
    gabi::store<f32>(gabi::ea(this) + 0x390, x);
    gabi::store<f32>(gabi::ea(this) + 0x398, z);
    eyePos.x = x;
    eyePos.y = y;
}
VERIFY(0x024DE930, &daWbird_c::calcMtx);

/* 024DE334 */
void daWbird_c::setStartPos() {
    WWHD_FUNC(0x024DE334, void, this);
    fopAc_ac_c* player = player_l();
    cXyz* wind = dKyw_get_wind_vec();
    u32 envLight = gabi::ea(dKy_getEnvlight());

    gabi::Local<cSGlobe_l> globe;
    cSGlobe_ct(globe, wind);
    gabi::Local<be<s16>> angle;
    cSAngle_ct_copy(angle, &globe->mInclination);
    gabi::Local<be<s16>> a;
    cSAngle_sub_s(&globe->mInclination, a, player->current.angle.y);
    if (*a.get() > *cSAngle__0) {
        cSAngle_addeq(angle, cSAngle__90);
    } else {
        cSAngle_subeq(angle, cSAngle__90);
    }

    f32 px = player->current.pos.x;
    current.pos.x = px;
    current.pos.y = player->current.pos.y;
    current.pos.z = player->current.pos.z;
    f32 c = cSAngle_Cos(angle);
    current.pos.x = gabi::fmadds(20.0f, c, px);
    f32 s = cSAngle_Sin(angle);
    f32 x = current.pos.x;
    f32 z = current.pos.z;
    home.pos.x = x;
    f32 y = current.pos.y;
    z = gabi::fmadds(20.0f, s, z);
    home.pos.y = y;
    home.pos.z = z;
    current.pos.z = z;
    current.pos.y = y + 150.0f;

    u16 wx = gabi::load<u16>(envLight + 0xA24); /* mWind.mTactWindAngleX */
    u16 wy = gabi::load<u16>(envLight + 0xA26); /* mWind.mTactWindAngleY */
    f32 cx = cM_scos_u(wx);
    f32 sp18x = cx * cM_scos_u(wy);
    f32 sp18z = cx * cM_ssin_u(wy);
    s16 iVar5 = cM_atan2s(sp18x, sp18z);

    /* HD: the flight time is 60 frames (GameCube 80) */
    f32 fVar1 = (f32)(s16)60 * 0.5f;
    f32 spx = 40.0f * sp18x;
    f32 spz = 40.0f * sp18z;
    f32 g = 1.0f * fVar1;
    speed.x = spx;
    f32 pz = current.pos.z;
    f32 pxx = current.pos.x;
    speed.z = spz;
    f32 py = current.pos.y;
    current.angle.y = iVar5;
    current.pos.z = gabi::fnmsubs(fVar1, spz, pz);
    current.pos.x = gabi::fnmsubs(fVar1, spx, pxx);
    field_0x29E = 60 + 30;
    shape_angle.y = iVar5;
    field_0x2A0 = 1.0f;
    current.pos.y = gabi::fmadds(fVar1 * fVar1, 0.5f, py);
    speed.y = -g;
}
VERIFY(0x024DE334, &daWbird_c::setStartPos);

/* 024DE9F0 */
BOOL daWbird_c::CreateInit() {
    WWHD_FUNC(0x024DE9F0, BOOL, this);
    s8 room = current.roomNo;
    setAction(3);
    tevStr.mRoomNo = room;
    fopAcM_orderChangeEvent(this, gabi::at<const char>(0x10042A94) /* "TACT_WINDOW" */, 0, 0xFFFF);
    field_0x29E = 0;
    calcMtx();
    return TRUE;
}
VERIFY(0x024DE9F0, &daWbird_c::CreateInit);

/* 024DEA54 */
cPhs_State daWbird_c::create() {
    WWHD_FUNC(0x024DEA54, cPhs_State, this);
    /* fopAcM_ct(this, daWbird_c) */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            fopAc_ac_c_ct(this);
            __vtbl = WBIRD_VTBL;
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    CreateInit();
    return cPhs_COMPLEATE_e;
}
VERIFY(0x024DEA54, &daWbird_c::create);

/* 024DE330 empty (unidentified; GameCube actionWait is empty and inlined into execute) */
static void daWbird_empty_024DE330() {
    WWHD_FUNC(0x024DE330, void);
}
VERIFY(0x024DE330, daWbird_empty_024DE330);

/* 024DE880 */
void daWbird_c::actionEnd() {
    WWHD_FUNC(0x024DE880, void, this);
    s16 idx = mEventIdx;
    BOOL end = gabi::call<BOOL>(0x025440C8, gabi::at<void>(dComIfGp_ea() + PLAY_EVTMANAGER), idx); /* endCheck */
    u32 play = dComIfGp_ea();
    if (end) {
        /* dComIfGp_event_reset() */
        gabi::store<u16>(play + 0x52B8, (u16)(gabi::load<u16>(play + 0x52B8) | 8));
        fopAcM_delete(this);
    } else {
        s32 staff = gabi::call<s32>(0x02542D88, gabi::at<void>(play + PLAY_EVTMANAGER), gabi::at<const char>(0x10042A8C) /* "WINDMAN" */, 0, 0);
        dComIfGp_evmng_cutEnd(staff);
    }
}
VERIFY(0x024DE880, &daWbird_c::actionEnd);

/* 024DE768 */
void daWbird_c::actionMove() {
    WWHD_FUNC(0x024DE768, void, this);
    fopAc_ac_c* player = player_l();
    if (field_0x29D) {
        field_0x29D = false;
        mDoAud_seStart_1(JA_SE_TAKT_WIND_DEMO);
    }
    if (field_0x29E > 0) {
        field_0x29E = field_0x29E - 1;
        /* HD: 30 / 20 (GameCube 60 / 70, and the wind power fades below 15) */
        if (field_0x29E > 30) {
            fopAcM_posMove(this, nullptr);
            f32 g = field_0x2A0;
            speed.y = speed.y + g;
        }
        if (field_0x29E == 20) {
            dKyw_tact_wind_set_go();
        }
    } else {
        dKyw_custom_windpower(0.0f);
        dComIfGp_evmng_cutEnd(dComIfGp_evmng_getMyStaffId(gabi::at<const char>(0x10042A84) /* "WINDMAN" */, nullptr, 0));
        /* player->changeDemoMoveAngle(mAngle) */
        gabi::store<s16>(gabi::ea(player) + 0x422, mAngle);
        setAction(1);
        mDoAud_seStart_1(JA_SE_TAKT_WIND_END);
    }
}
VERIFY(0x024DE768, &daWbird_c::actionMove);

/* 024DE55C */
void daWbird_c::actionSelect() {
    WWHD_FUNC(0x024DE55C, void, this);
    s16 sVar2 = field_0x29E;
    if (sVar2 == 10) {
        /* dComIfGp_setOperateWindOn() */
        gabi::store<u8>(dComIfGp_ea() + 0x5BD0, 2);
        mDoAud_seStart_1(JA_SE_TAKT_WIND_DISP);
        field_0x29E = field_0x29E + 1;
    } else if (sVar2 > 10) {
        mDoAud_seStart_1(JA_SE_SYS_WTAKT_WIND_AMB);
        u8 op = gabi::load<u8>(dComIfGp_ea() + 0x5BD0); /* dComIfGp_getOperateWind() */
        if (op == 1) {
            mDoAud_seStart_1(JA_SE_TAKT_WIND_DECIDE);
            setStartPos();
            setAction(2);
            fopAc_ac_c* player = player_l();
            mAngle = player->shape_angle.y;
            /* player->setPlayerPosAndAngle(&player->current.pos, current.angle.y + 0x7FFF) (virtual) */
            u32 fn = gabi::load<u32>(player->__vtbl + 0x114);
            gabi::call_ptr(fn, player, &player->current.pos, (s16)(current.angle.y + 0x7FFF));
            u32 ship = gabi::load<u32>(dComIfGp_ea() + PLAY_PLAYER_STATUS0) & 0x10000; /* daPyStts0_SHIP_RIDE_e */
            /* the event index is passed on as the raw r3 of getEventIdx (not re-extended) */
            s32 idx;
            if (ship != 0) {
                idx = gabi::call<s32>(0x02543F10, dComIfGp_getPEvtManager(), gabi::at<const char>(0x10042A5C) /* "TACT_WINDOW2_SHIP" */, 0xFF);
            } else {
                idx = gabi::call<s32>(0x02543F10, dComIfGp_getPEvtManager(), gabi::at<const char>(0x10042A70) /* "TACT_WINDOW2" */, 0xFF);
            }
            mEventIdx = (s16)idx;
            gabi::call<BOOL>(0x025D7874, this, idx, 0, 0xFFFF); /* fopAcM_orderChangeEventId */
            /* player->cancelOriginalDemo() */
            u32 p = gabi::ea(player_l());
            gabi::store<s16>(p + 0x420, 2);
            gabi::store<u32>(p + 0x430, 1);
            dKyw_custom_windpower(1.0f);
            field_0x29D = true;
        } else if (op < 1) {
            mDoAud_seStart_1(JA_SE_TAKT_WIND_CANCEL);
            u32 play = dComIfGp_ea();
            gabi::store<u16>(play + 0x52B8, (u16)(gabi::load<u16>(play + 0x52B8) | 8)); /* dComIfGp_event_reset() */
            fopAcM_delete(this);
        }
    } else {
        field_0x29E = sVar2 + 1;
    }
}
VERIFY(0x024DE55C, &daWbird_c::actionSelect);

/* 024DE958 daWbird_Execute (inlines execute) */
static BOOL daWbird_Execute(daWbird_c* i_this) {
    WWHD_FUNC(0x024DE958, BOOL, i_this);
    switch (i_this->mAction) {
    case 3:
        i_this->actionSelect();
        break;
    case 2:
        i_this->actionMove();
        break;
    case 1:
        i_this->actionEnd();
        break;
    default:
        break; /* actionWait */
    }
    i_this->calcMtx();
    return TRUE;
}
VERIFY(0x024DE958, daWbird_Execute);

/* 024DE9E0 */
static BOOL daWbird_IsDelete(daWbird_c*) {
    WWHD_FUNC(0x024DE9E0, BOOL, (daWbird_c*)nullptr);
    return TRUE;
}
VERIFY(0x024DE9E0, daWbird_IsDelete);

/* 024DE9E8 daWbird_Delete (HD: no destructor call) */
static BOOL daWbird_Delete(daWbird_c*) {
    WWHD_FUNC(0x024DE9E8, BOOL, (daWbird_c*)nullptr);
    return TRUE;
}
VERIFY(0x024DE9E8, daWbird_Delete);

/* 024DEABC */
static cPhs_State daWbird_Create(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x024DEABC, cPhs_State, i_this);
    return static_cast<daWbird_c*>(i_this)->create();
}
VERIFY(0x024DEABC, daWbird_Create);

/* 024DEB54 */
static BOOL daWbird_Draw(daWbird_c*) {
    WWHD_FUNC(0x024DEB54, BOOL, (daWbird_c*)nullptr);
    return TRUE;
}
VERIFY(0x024DEB54, daWbird_Draw);

/* 024DE2DC daWbird_c::~daWbird_c (deleting) */
static void daWbird_c_dt(daWbird_c* i_this, s32 flags) {
    WWHD_FUNC(0x024DE2DC, void, i_this, flags);
    if (i_this != nullptr) {
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x024DE2DC, daWbird_c_dt);

/* 024DEAC0 __sinit_d_a_wbird_cpp */
static void __sinit_d_a_wbird_cpp() {
    WWHD_FUNC(0x024DEAC0, void);
    sinit_header_statics(0x1046EBC8, 0x101D332C);
}
VERIFY(0x024DEAC0, __sinit_d_a_wbird_cpp);

/* ---- leftover functions of the translation unit ---- */

/* 024DEB5C daWbird_c::~daWbird_c (deleting; vtable slot 10042A38): only the fopAc_ac_c base.
 * The vtable holding this slot is written by daWbird_c::create (+0x34). Note: 024DE2DC above (same
 * code, also named daWbird_c_dt) sits in the vtable slot 10042940 that daWarpmj_c::CreateHeap uses,
 * i.e. it is probably the d_a_warpmj translation unit's destructor; behaviour is identical. */
static void daWbird_c_dt_024DEB5C(void* p, s32 flags) {
    WWHD_FUNC(0x024DEB5C, void, p, flags);
    if (p != nullptr) {
        gabi::call(0x025D50BC, p, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1) operator_delete(p);
    }
}
VERIFY(0x024DEB5C, daWbird_c_dt_024DEB5C);
