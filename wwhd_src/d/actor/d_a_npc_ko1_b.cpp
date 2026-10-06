/**
 * d_a_npc_ko1_b.cpp (WWHD)
 * NPC - Joel & Zill (Outset Island), part B
 *
 * The GameCube decompilation (zeldaret/tww
 * src/d/actor/d_a_npc_ko1.cpp) has only "Nonmatching" stubs for this actor: the functions are
 * written from the WWHD code (cking.rpx) and verified against it, keeping the GameCube names.
 * Part B: setMtx, create/delete, events, route checks, movement, execute, draw.
 */
#define SAFESTRING_VTBL 0x1001C87C /* this TU's sead::SafeString vtable */
#include "d/actor/d_a_npc_ko1.h"

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 0201B080 cXyz::outprod(const Vec&) const: result through a hidden pointer (r4) */
static inline void cXyz_outprod(const cXyz* a, cXyz* out, const cXyz* b) { gabi::call(0x0201B080, a, out, b); }
/* 02008DAC cBgS_Chk::~cBgS_Chk (the dBgS_GndChk destructor's tail) */
static inline void cBgS_Chk_dt(void* chk, s32 flags) { gabi::call(0x02008DAC, chk, flags); }

/* 025E535C mDoExt_McaMorf::play(pos, se, reverb): the reverb register is passed on as returned
 * by dComIfGp_getReverb (no s8 extension) */
static inline BOOL McaMorf_play(mDoExt_McaMorf* m, cXyz* pos, u32 se, s32 reverb) { return gabi::call<BOOL>(0x025E535C, m, pos, se, reverb); }
static inline void McaMorf_calc(mDoExt_McaMorf* m) { gabi::call(0x025E55A0, m); }
/* 02527028 dDemo_setDemoData(actor, flags, morf, arc, n, ids, p6, p7) */
static inline BOOL dDemo_setDemoData(fopAc_ac_c* a, u8 flags, mDoExt_McaMorf* morf, const char* arc, s32 n, void* ids, u32 p6, s8 p7) {
    return gabi::call<BOOL>(0x02527028, a, flags, morf, arc, n, ids, p6, p7);
}

/* 0259E6D0 dNpc_PathRun_c::setInf(pathIdx, roomNo, forwards) */
static inline BOOL dNpc_PathRun_setInf(dNpc_PathRun_l* p, u8 path, s8 room, u8 fwd) { return gabi::call<BOOL>(0x0259E6D0, p, path, room, fwd); }

/* static initialisation of a 4-byte function-local constant on first use */
static inline void local_static_init(u32 guard, u32 dst, u32 src) {
    if (gabi::load<u32>(guard) == 0) {
        gabi::store<u32>(guard, 1);
        memcpy_g(gabi::at<u8>(dst), gabi::at<u8>(src), 4); /* 028FEAC0 memcpy */
    }
}

/* dNpc_PathRun_c (HD: same 8 bytes) */
static inline BOOL dNpc_PathRun_chkPointPass(dNpc_PathRun_l* p, cXyz* pos, u32 dir) { return gabi::call<BOOL>(0x0259E838, p, pos, dir); }
static inline BOOL dNpc_PathRun_nextIdxAuto(dNpc_PathRun_l* p) { return gabi::call<BOOL>(0x0259ED58, p); }

/* ---- file statics ---- */
/* l_check_wrk / l_check_inf[20]: search results of searchActor_* (part A) */
static be<s32>& l_check_wrk() { return *gabi::at<be<s32>>(0x104677B0); }
static gptr<fopAc_ac_c>* l_check_inf() { return gabi::at<gptr<fopAc_ac_c>>(0x104678A8); }

/* daNpc_Ko1_childHIO_c[]: one 0x60 block per mType at 0x104677E8 (HD: the HIO's vtable is not
 * in front of the child blocks) */
static inline u32 l_HIO_child(s32 type) { return 0x104677E8 + type * 0x60; }

/* this TU's dBgS_LinChk / dBgS_GndChk vtables (inline constructors and destructors) */
static const dBgS_LinChk_vt l_linChk_vt = {0x1001C914, 0x1001C924, 0x1001C944, 0x1001C934};
static const dBgS_GndChk_vt l_gndChk_vt = {0x1001C8B4, 0x1001C8C4, 0x1001C8E4, 0x1001C8D4};

#define KO1_VTBL 0x1001CD88 /* daNpc_Ko1_c vtable (HD: merged with fopNpc_npc_c's) */

/* 02274E28 */
bool daNpc_Ko1_c::createInit() {
    WWHD_FUNC(0x02274E28, bool, this);
    gabi::store<u32>(gabi::ea(this) + 0x39C, 0xA); /* attention_info.flags = LOCKON_TALK | ACTION_SPEAK */
    switch ((u32)(s32)mType) {
    case 0:
        gabi::store<u8>(gabi::ea(this) + 0x38B, 0xA9); /* attention_info.distances[SPEAK] */
        gabi::store<u8>(gabi::ea(this) + 0x389, 0xA9); /* attention_info.distances[TALK] */
        break;
    case 1:
        gabi::store<u8>(gabi::ea(this) + 0x389, 0xA7);
        gabi::store<u8>(gabi::ea(this) + 0x38B, 0xA9);
        break;
    }
    gravity = -4.5f;
    u8 pathIdx = (fopAcM_GetParam(this) >> 16) & 0xFF;
    mInitialAngle.y = current.angle.y;
    mInitialPos.copy(current.pos);
    m958.copy(current.pos);
    m97C.copy(current.pos);
    mInitialAngle.x = current.angle.x;
    mInitialAngle.z = current.angle.z;
    s32 weight = 0xFF;
    if (pathIdx != 0xFF) {
        dNpc_PathRun_setInf(&mPathRun, pathIdx, current.roomNo, 1);
        if (mPathRun.mPath.get() == nullptr) {
            return false;
        }
        actor_status &= ~0x80u; /* fopAcM_OffStatus(this, fopAcStts_NOCULLEXEC_e) */
        weight = 0xF0;
    }
    /* l_staff_name (.data 0x101BF7A8): "HNA" x5, "BOU" x4 */
    mEventCut.setActorInfo2(gabi::at<const char>(gabi::load<u32>(0x101BF7A8 + mSpecificType * 4)), this);
    mA0F = 0xE;
    bool ok;
    switch ((u32)(s32)mSpecificType) {
    case 0:
        ok = gabi::call<bool>(0x022743EC, this); /* init_HNA_0() */
        break;
    case 1:
        ok = gabi::call<bool>(0x02274498, this); /* init_HNA_1() */
        weight = 0xF0;
        break;
    case 2:
        ok = gabi::call<bool>(0x0227452C, this); /* init_HNA_2() */
        weight = 0xF0;
        break;
    case 3:
        ok = gabi::call<bool>(0x022745B4, this); /* init_HNA_3() */
        break;
    case 4:
        ok = gabi::call<bool>(0x02274658, this); /* init_HNA_4() */
        break;
    case 5:
        ok = gabi::call<bool>(0x022746E8, this); /* init_BOU_0() */
        break;
    case 6:
        ok = gabi::call<bool>(0x02274778, this); /* init_BOU_1() */
        weight = 0xF0;
        break;
    case 7:
        ok = gabi::call<bool>(0x02274808, this); /* init_BOU_2() */
        break;
    case 8:
        ok = gabi::call<bool>(0x022748B0, this); /* init_BOU_3() */
        break;
    default:
        return false;
    }
    if (!ok) {
        return false;
    }
    m946.x = current.angle.x;
    m946.y = current.angle.y;
    m946.z = current.angle.z;
    shape_angle.x = current.angle.x;
    shape_angle.y = current.angle.y;
    shape_angle.z = current.angle.z;
    mStts.Init(weight, 0xFF, this);
    mCyl.SetStts(&mStts);
    mCyl.Set(gabi::at<dCcD_SrcCyl>(0x101EA190)); /* l_cyl_src */
    mpMorf->setMorf(0.0f);
    mpMorf824->setMorf(0.0f);
    setMtx(1);
    return true;
}
VERIFY(0x02274E28, &daNpc_Ko1_c::createInit);

/* 02275180 */
cPhs_State daNpc_Ko1_c::_create() {
    WWHD_FUNC(0x02275180, cPhs_State, this);
    /* fopAcM_SetupActor(this, daNpc_Ko1_c) */
    if (!fopAcM_CheckCondition(this, 8 /* fopAcCnd_INIT_e */)) {
        if (this != nullptr) {
            gabi::call(0x025A1458, this); /* fopNpc_npc_c::fopNpc_npc_c */
            __vtbl = KO1_VTBL;
            gabi::call(0x025E7820, mBtpAnm);    /* mDoExt_btpAnm::mDoExt_btpAnm */
            gabi::call(0x0259F740, &mEventCut); /* dNpc_EventCut_c::dNpc_EventCut_c */
            gabi::call(0x025A9084, &mRipple);   /* dPa_rippleEcallBack::dPa_rippleEcallBack */
        }
        fopAcM_OnCondition(this, 8);
    }
    cPhs_State phase = dComIfG_resLoad(&mPhs, STR(0x1001CB78));
    if (phase != cPhs_COMPLEATE_e) {
        return phase;
    }
    if (!gabi::call<u32>(0x02274250, this, fopAcM_GetParam(this) & 0xFF) /* charDecide(param & 0xFF) */) {
        return cPhs_ERROR_e;
    }
    /* a_heap_size_tbl (.data 0x101BF7CC) */
    if (!fopAcM_entrySolidHeap(this, 0x0227424C /* CheckCreateHeap */, gabi::load<u32>(0x101BF7CC + mType * 4))) {
        return cPhs_ERROR_e;
    }
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpMorf->getModel())); /* fopAcM_SetMtx */
    fopAcM_setCullSizeBox(this, -50.0f, -20.0f, -50.0f, 50.0f, 120.0f, 50.0f);
    if (!createInit()) {
        return cPhs_ERROR_e;
    }
    return phase;
}
VERIFY(0x02275180, &daNpc_Ko1_c::_create);

/* 022752D0 */
static cPhs_State daNpc_Ko1_Create(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x022752D0, cPhs_State, i_this);
    return ((daNpc_Ko1_c*)i_this)->_create();
}
VERIFY(0x022752D0, daNpc_Ko1_Create);

/* 022752D4 */
BOOL daNpc_Ko1_c::_delete() {
    WWHD_FUNC(0x022752D4, BOOL, this);
    dComIfG_resDelete(&mPhs, STR(0x1001CB7B));
    if (heap.get() != nullptr) {
        if (mpMorf.get() != nullptr)
            mpMorf->stopZelAnime();
        if (mpMorf824.get() != nullptr)
            mpMorf824->stopZelAnime();
        if (mpMorf81C.get() != nullptr)
            mpMorf81C->stopZelAnime();
    }
    dPa_rippleEcallBack_end((dPa_rippleEcallBack*)&mRipple);
    return TRUE;
}
VERIFY(0x022752D4, &daNpc_Ko1_c::_delete);

/* 02275354 */
static BOOL daNpc_Ko1_Delete(daNpc_Ko1_c* i_this) {
    WWHD_FUNC(0x02275354, BOOL, i_this);
    return i_this->_delete();
}
VERIFY(0x02275354, daNpc_Ko1_Delete);

/* 02275358 */
u32 daNpc_Ko1_c::partner_srch_sub(u32 i_judgeFunc) {
    WWHD_FUNC(0x02275358, u32, this, i_judgeFunc);
    l_check_wrk() = 0;
    for (int i = 0; i < 20; i++) {
        l_check_inf()[i] = nullptr;
    }
    fpcM_Search(i_judgeFunc, this);
    u32 id = 0xFFFFFFFF;
    if (l_check_wrk() != 0) {
        id = fopAcM_GetID(l_check_inf()[0].get());
    }
    return id;
}
VERIFY(0x02275358, &daNpc_Ko1_c::partner_srch_sub);

/* 022753EC */
void daNpc_Ko1_c::partner_srch() {
    WWHD_FUNC(0x022753EC, void, this);
    if (mA18 != 1)
        return;
    switch ((u32)(s32)mSpecificType) {
    case 1:
        m924 = partner_srch_sub(0x022731F0 /* searchActor_Ko_Bou */);
        m92C = 1;
        break;
    case 3:
        m924 = partner_srch_sub(0x022731F0 /* searchActor_Ko_Bou */);
        m928 = partner_srch_sub(0x02273270 /* searchActor_Ob */);
        m92C = 2;
        break;
    case 6:
        m924 = partner_srch_sub(0x02273170 /* searchActor_Ko_Hna */);
        m92C = 1;
        break;
    case 7:
        m924 = partner_srch_sub(0x02273170 /* searchActor_Ko_Hna */);
        m928 = partner_srch_sub(0x02273270 /* searchActor_Ob */);
        m92C = 2;
        break;
    }
}
VERIFY(0x022753EC, &daNpc_Ko1_c::partner_srch);

/* 02275504 */
void daNpc_Ko1_c::checkOrder() {
    WWHD_FUNC(0x02275504, void, this);
    u16 cmd = gabi::load<u16>(gabi::ea(this) + 0xF8); /* eventInfo.mCommand */
    if (cmd == 2 /* checkCommandDemoAccrpt() */) {
        return;
    }
    if (cmd == 1 /* checkCommandTalk() */ && (mA12 == 1 || mA12 == 2)) {
        mA12 = 0;
        m9E5 = 1;
    }
}
VERIFY(0x02275504, &daNpc_Ko1_c::checkOrder);

/* 022756F8 */
s32 daNpc_Ko1_c::isEventEntry() {
    WWHD_FUNC(0x022756F8, s32, this);
    const char* name = gabi::at<const char>(mEventCut.mpEvtStaffName);
    return dComIfGp_evmng_getMyStaffId(name, nullptr, 0);
}
VERIFY(0x022756F8, &daNpc_Ko1_c::isEventEntry);

/* 02275738 */
void daNpc_Ko1_c::endEvent() {
    WWHD_FUNC(0x02275738, void, this);
    dComIfGp_event_reset();
    mA0C = 0xFF;
    mA0D = 0xFF;
}
VERIFY(0x02275738, &daNpc_Ko1_c::endEvent);

/* 0227577C */
void daNpc_Ko1_c::event_actionInit(s32 i_staffIdx) {
    WWHD_FUNC(0x0227577C, void, this, i_staffIdx);
    be<s32>* prm = (be<s32>*)dComIfGp_evmng_getMyIntegerP(i_staffIdx, STR(0x1001CB84));
    if (prm != nullptr) {
        mA0B = (u8)(s32)*prm;
    }
}
VERIFY(0x0227577C, &daNpc_Ko1_c::event_actionInit);

/* 022757DC */
BOOL daNpc_Ko1_c::event_action() {
    WWHD_FUNC(0x022757DC, BOOL, this);
    return TRUE;
}
VERIFY(0x022757DC, &daNpc_Ko1_c::event_action);

/* 022757E4 */
void daNpc_Ko1_c::privateCut(s32 i_staffIdx) {
    WWHD_FUNC(0x022757E4, void, this, i_staffIdx);
    if (i_staffIdx == -1) {
        return;
    }
    /* a_cut_tbl (.data 0x101BF7D4) [1] */
    s8 actIdx = dComIfGp_evmng_getMyActIdx(i_staffIdx, 0x101BF7D4, 1, TRUE, 0);
    mA0A = actIdx;
    if (actIdx != -1) {
        if (dComIfGp_evmng_getIsAddvance(i_staffIdx) && mA0A == 0) {
            event_actionInit(i_staffIdx);
        }
        if (mA0A == 0 && !event_action()) {
            return;
        }
    }
    dComIfGp_evmng_cutEnd(i_staffIdx);
}
VERIFY(0x022757E4, &daNpc_Ko1_c::privateCut);

/* 02275B28 */
void daNpc_Ko1_c::event_proc(s32 i_staffIdx) {
    WWHD_FUNC(0x02275B28, void, this, i_staffIdx);
    if (!mEventCut.cutProc()) {
        privateCut(i_staffIdx);
    }
    lookBack();
}
VERIFY(0x02275B28, &daNpc_Ko1_c::event_proc);

/* 02276578 */
void daNpc_Ko1_c::eventOrder() {
    WWHD_FUNC(0x02276578, void, this);
    if (mA12 == 1 || mA12 == 2) {
        u32 a = gabi::ea(this) + 0xFA;
        gabi::store<u16>(a, gabi::load<u16>(a) | 1); /* eventInfo.onCondition(dEvtCnd_CANTALK_e) */
        if (mA12 == 1) {
            fopAcM_orderSpeakEvent(this);
        }
    }
}
VERIFY(0x02276578, &daNpc_Ko1_c::eventOrder);

/* 02276848 */
static BOOL daNpc_Ko1_Execute(daNpc_Ko1_c* i_this) {
    WWHD_FUNC(0x02276848, BOOL, i_this);
    return i_this->_execute();
}
VERIFY(0x02276848, daNpc_Ko1_Execute);

/* 02276B20 */
static BOOL daNpc_Ko1_Draw(daNpc_Ko1_c* i_this) {
    WWHD_FUNC(0x02276B20, BOOL, i_this);
    return i_this->_draw();
}
VERIFY(0x02276B20, daNpc_Ko1_Draw);

/* 02276B24 */
static BOOL daNpc_Ko1_IsDelete(daNpc_Ko1_c*) {
    WWHD_FUNC(0x02276B24, BOOL, (daNpc_Ko1_c*)nullptr);
    return TRUE;
}
VERIFY(0x02276B24, daNpc_Ko1_IsDelete);

/* 022758B8 */
void daNpc_Ko1_c::lookBack() {
    WWHD_FUNC(0x022758B8, void, this);
    s16 desiredYRot = current.angle.y;
    m9B4 = desiredYRot;
    m9B2 = m_jnt.mAngles[1][1];
    m9B0 = m_jnt.mAngles[0][1];
    gabi::Local<cXyz> dstPos;
    dstPos->set(0.0f, 0.0f, 0.0f);
    f32 eyeY = eyePos.y;
    f32 posX = current.pos.x;
    u8 headOnly = m9E6;
    cXyz* dstPosP = nullptr;
    f32 posZ = current.pos.z;
    switch ((u32)(s32)mA15) {
    case 1: {
        gabi::Local<cXyz> eye;
        dNpc_playerEyePos_l(eye, -20.0f);
        posX = current.pos.x;
        posZ = current.pos.z;
        eyeY = eyePos.y;
        dstPos->copy(*eye);
        dstPosP = dstPos;
        break;
    }
    case 2:
        dstPos->copy(m958);
        posX = current.pos.x;
        posZ = current.pos.z;
        dstPosP = dstPos;
        break;
    case 3:
        desiredYRot = m9C8;
        break;
    }
    cLib_addCalcAngleS2(&m9C6, gabi::load<s16>(l_HIO_child(mType) + 0x16), 4, 0x800);
    s16 maxVel;
    if (!m_jnt.mbTrn) {
        m9C6 = 0;
        maxVel = 0;
    } else {
        maxVel = m9C6;
    }
    gabi::Local<cXyz> eyeCopy;
    eyeCopy->set(posX, eyeY, posZ);
    dNpc_JntCtrl_lookAtTarget(&m_jnt, &current.angle.y, dstPosP, eyeCopy, desiredYRot, maxVel, headOnly);
}
VERIFY(0x022758B8, &daNpc_Ko1_c::lookBack);

/* 02275B80 */
f32 daNpc_Ko1_c::chk_ForwardGroundY(s16 i_angle) {
    WWHD_FUNC(0x02275B80, f32, this, i_angle);
    cXyz* pla = (cXyz*)dBgS_GetTriPla(dComIfG_Bgsp(), &mAcchCir); /* the wall polygon's plane normal */
    if (pla != nullptr && cLib_distanceAngleS(i_angle, cM_atan2s(pla->x, pla->z)) > 0x4000) {
        gabi::Local<dBgS_GndChk> gndChk;
        u32 b = gabi::ea(gndChk.get());
        gabi::call(0x02008E0C, gndChk.get()); /* cBgS_GndChk::cBgS_GndChk */
        for (int i = 0; i < 7; i++) gabi::store<u8>(b + 0x44 + i, 0);
        gabi::store<u32>(b + 0x50, 1);
        gabi::store<u32>(b + 0x4, b + 0x4C);
        gabi::store<u32>(b + 0x0, b + 0x40);
        gabi::store<u32>(b + 0x4C, l_gndChk_vt.v4C);
        gabi::store<u32>(b + 0x40, l_gndChk_vt.v40);
        gabi::store<u32>(b + 0x20, l_gndChk_vt.v20);
        gabi::store<u32>(b + 0x10, l_gndChk_vt.v10);
        /* gndChk.SetPos(pos + 80 in direction i_angle, 80 up) */
        f32 y = current.pos.y + 80.0f;
        f32 x = gabi::fmadds(80.0f, cM_ssin((u16)i_angle), current.pos.x);
        f32 z = gabi::fmadds(80.0f, cM_scos((u16)i_angle), current.pos.z);
        gabi::store<u32>(b + 0x30, gabi::load<u32>(b + 0x30) & ~2u);
        gabi::store<f32>(b + 0x2C, z);
        gabi::store<f32>(b + 0x24, x);
        gabi::store<f32>(b + 0x28, y);
        f32 groundY = cBgS_GroundCross(dComIfG_Bgsp(), gndChk);
        /* ~dBgS_GndChk */
        gabi::store<u32>(b + 0x20, l_gndChk_vt.v20);
        gabi::store<u32>(b + 0x40, l_gndChk_vt.v40);
        gabi::store<u32>(b + 0x4C, 0x1001C8A4);
        cBgS_Chk_dt(gndChk, 0);
        return groundY;
    }
    return -10000000.0f;
}
VERIFY(0x02275B80, &daNpc_Ko1_c::chk_ForwardGroundY);

/* 02275D28 */
f32 daNpc_Ko1_c::chk_wallJump(s16 i_angle) {
    WWHD_FUNC(0x02275D28, f32, this, i_angle);
    f32 h = chk_ForwardGroundY(i_angle);
    if (0.0f < h && h < 100.0f) { /* bge: NaN takes the -1 path */
        return std_sqrtf(h) * 3.2f;
    }
    return -1.0f;
}
VERIFY(0x02275D28, &daNpc_Ko1_c::chk_wallJump);

/* 02275D90 */
void daNpc_Ko1_c::chk_routeAngle(cXyz* i_wallNrm, be<s16>* io_angle) {
    WWHD_FUNC(0x02275D90, void, this, i_wallNrm, io_angle);
    gabi::Local<cXyz> side;
    cXyz_outprod(&m988, side, i_wallNrm);
    s16 angle = cM_atan2s(side->x, side->z);
    if (!(m988.y < 0.999f) && cLib_distanceAngleS(angle, *io_angle) > 0x4000) {
        angle = (s16)(angle - 0x8000);
    } else if (side->y * (m964.y - current.pos.y) < 0.0f) {
        angle = (s16)(angle - 0x8000);
    }
    *io_angle = angle;
}
VERIFY(0x02275D90, &daNpc_Ko1_c::chk_routeAngle);

/* 02275E44 */
void daNpc_Ko1_c::routeWallCheck(cXyz* i_start, cXyz* i_end, be<s16>* io_angle) {
    WWHD_FUNC(0x02275E44, void, this, i_start, i_end, io_angle);
    gabi::Local<dBgS_LinChk> linChk;
    dBgS_LinChk_ct(linChk, l_linChk_vt, false);
    dBgS_LinChk_Set(linChk, i_start, i_end, nullptr);
    if (cBgS_LineCross(dComIfG_Bgsp(), linChk)) {
        cXyz* pla = (cXyz*)dBgS_GetTriPla(dComIfG_Bgsp(), gabi::at<u8>(gabi::ea(linChk.get()) + 0x14));
        if (pla != nullptr) {
            chk_routeAngle(pla, io_angle);
        }
    }
    /* ~dBgS_LinChk */
    u32 b = gabi::ea(linChk.get());
    gabi::store<u32>(b + 0x58, l_linChk_vt.v58);
    gabi::store<u32>(b + 0x64, 0x1001C8A4);
    gabi::store<u32>(b + 0x20, 0x1001C894);
    cBgS_LinChk_dt(linChk, 0);
}
VERIFY(0x02275E44, &daNpc_Ko1_c::routeWallCheck);

/* 02275F60 */
/* i_distSq (squared xz distance to the target) is not used */
BOOL daNpc_Ko1_c::routeCheck(f32 i_distSq, be<s16>* io_angle) {
    WWHD_FUNC(0x02275F60, BOOL, this, i_distSq, io_angle);
    if (mA08 != 3 && (gabi::load<u32>(gabi::ea(this) + 0x478) & 0x10) /* mObjAcch.ChkWallHit() */) {
        chk_wallJump(*io_angle);
    }
    f32 y = current.pos.y + 80.0f;
    u16 a = (u16)*io_angle;
    gabi::Local<cXyz> start;
    start->set(current.pos.x, y, current.pos.z);
    gabi::Local<cXyz> end;
    end->set(gabi::fmadds(80.0f, cM_ssin(a), current.pos.x), y, gabi::fmadds(80.0f, cM_scos(a), current.pos.z));
    routeWallCheck(start, end, io_angle);
    return TRUE;
}
VERIFY(0x02275F60, &daNpc_Ko1_c::routeCheck);

/* 02276028 */
void daNpc_Ko1_c::ko_clcMovSpd() {
    WWHD_FUNC(0x02276028, void, this);
    gabi::Local<cXyz> diff;
    cXyz_mi(&m964, diff, &current.pos);
    gabi::Local<cXyz> diffXZ;
    diffXZ->set(diff->x, 0.0f, diff->z);
    f32 distSq = PSVECSquareMag(diffXZ);
    gabi::Local<be<s16>> angle;
    *angle = cLib_targetAngleY(&current.pos, &m964);
    if (!routeCheck(distSq, angle)) {
        return;
    }
    switch (mA13) {
    case 4:
    case 0xB:
    case 0x18:
        /* zigzag */
        if (!cLib_calcTimer(&m9C0)) {
            m9D4 = m9D4 ^ 1;
            m9C0 = (s16)cLib_getRndValue(8, 0x14);
        }
        *angle = (s16)(*angle + (m9D4 != 0 ? -0x2000 : 0x2000));
        break;
    }
    cLib_chaseAngleS(&current.angle.y, *angle, gabi::load<s16>(l_HIO_child(mType) + 0x20));
    cLib_chaseF(&speedF, m99C, m9A4);
}
VERIFY(0x02276028, &daNpc_Ko1_c::ko_clcMovSpd);

/* 02276198 */
void daNpc_Ko1_c::setPlaySpd(f32 i_speed) {
    WWHD_FUNC(0x02276198, void, this, i_speed);
    gabi::store<f32>(gabi::ea(mpMorf824.get()) + 0x98, i_speed); /* frame control rate */
    gabi::store<f32>(gabi::ea(mpMorf.get()) + 0x98, i_speed);
}
VERIFY(0x02276198, &daNpc_Ko1_c::setPlaySpd);

/* 022761AC */
/* 0: moving, 1: point reached, 2: end of the path */
s32 daNpc_Ko1_c::ko_movPass() {
    WWHD_FUNC(0x022761AC, s32, this);
    dPath* path = mPathRun.mPath;
    if (path != nullptr && (gabi::load<u8>(gabi::ea(path) + 5) & 1)) {
        gabi::Local<cXyz> pos;
        pos->set(current.pos.x, current.pos.y, current.pos.z);
        if (dNpc_PathRun_chkPointPass(&mPathRun, pos, mPathRun.mbDir != 0)) {
            dNpc_PathRun_nextIdxAuto(&mPathRun);
            return 1;
        }
        return 0;
    }
    gabi::Local<cXyz> diff;
    cXyz_mi(&m964, diff, &current.pos);
    gabi::Local<cXyz> diffXZ;
    diffXZ->set(diff->x, 0.0f, diff->z);
    f32 dist = std_sqrtf(PSVECSquareMag(diffXZ));
    s32 ret = 0;
    if (!(dist > m9AC)) {
        ret = 1;
        if (mPathRun.mPath.get() != nullptr && !dNpc_PathRun_nextIdxAuto(&mPathRun)) {
            ret = 2;
        }
    }
    return ret;
}
VERIFY(0x022761AC, &daNpc_Ko1_c::ko_movPass);

/* 022762AC */
void daNpc_Ko1_c::ko_clcSwmSpd() {
    WWHD_FUNC(0x022762AC, void, this);
    gabi::Local<cXyz> diff;
    cXyz_mi(&m964, diff, &current.pos);
    gabi::Local<cXyz> diffXZ;
    diffXZ->set(diff->x, 0.0f, diff->z);
    f32 distSq = PSVECSquareMag(diffXZ);
    gabi::Local<be<s16>> angle;
    *angle = cLib_targetAngleY(&current.pos, &m964);
    if (!routeCheck(distSq, angle)) {
        return;
    }
    cLib_chaseAngleS(&current.angle.y, *angle, gabi::load<s16>(l_HIO_child(mType) + 0x20));
    cLib_chaseF(&speed.y, m9A0, 1.6f);
    cLib_chaseF(&speedF, 0.0f, 0.4f);
}
VERIFY(0x022762AC, &daNpc_Ko1_c::ko_clcSwmSpd);

/* 022763A8 */
void daNpc_Ko1_c::ko_nMove() {
    WWHD_FUNC(0x022763A8, void, this);
    s32 pass;
    switch ((u32)(s32)mA08) {
    case 0:
        break;
    case 1:
    case 2: {
        ko_clcMovSpd();
        u32 hio = l_HIO_child(mType);
        f32 spd = speedF;
        f32 rate, maxRate;
        if (mA08 == 1) {
            rate = gabi::load<f32>(hio + 0x24);
            maxRate = gabi::load<f32>(hio + 0x28) * rate;
        } else {
            rate = gabi::load<f32>(hio + 0x34);
            maxRate = gabi::load<f32>(hio + 0x38) * rate;
        }
        f32 play = spd * rate;
        play = (play - maxRate >= 0.0f) ? maxRate : play; /* fsel */
        if (play < 0.5f) {
            play = 0.5f;
        }
        setPlaySpd(play);
        pass = ko_movPass();
        if (pass == 1) {
            m9DB = 1;
            m9D9 = 1;
            return;
        }
        if (pass == 2) {
            m9DB = 1;
            mA08 = 0;
            m9D9 = 1;
            return;
        }
        break;
    }
    case 3:
        ko_clcSwmSpd();
        pass = ko_movPass();
        if (pass == 1) {
            m9DB = 1;
            m9D9 = 1;
            return;
        }
        if (pass == 2) {
            m9DB = 1;
            mA08 = 0;
            m9D9 = 1;
            return;
        }
        break;
    case 4:
        cLib_chaseF(&speedF, 0.1f, m9A4);
        if (m9DC == 0 && (gabi::load<u32>(gabi::ea(this) + 0x478) & 0x20) /* mObjAcch.ChkGroundHit() */) {
            mA08 = mA09;
            speedF = 0.0f;
            m9DB = 1;
            speed.y = 0.0f;
            m9D9 = 1;
            gravity = -4.5f;
            return;
        }
        break;
    }
    if (m9DB != 0) {
        m9D9 = 1;
    }
}
VERIFY(0x022763A8, &daNpc_Ko1_c::ko_nMove);

/* 02274A7C */
/* i_setEyePos (GameCube bool) is passed on to setAttention unnormalised: typed u32 */
void daNpc_Ko1_c::setMtx(u32 i_setEyePos) {
    WWHD_FUNC(0x02274A7C, void, this, i_setEyePos);
    if (m9E7 == 0) {
        u32 sndId = 0;
        gabi::call(0x02274948, this); /* plyTexPttrnAnm() */
        if (gabi::load<u32>(gabi::ea(this) + 0x478) & 0x20 /* mObjAcch.ChkGroundHit() */) {
            sndId = dBgS_GetMtrlSndId(dComIfG_Bgsp(), gabi::at<cBgS_PolyInfo>(gabi::ea(this) + 0x538) /* ground poly */);
        }
        s32 reverb = dComIfGp_getReverb(current.roomNo);
        m9D0 = (u8)McaMorf_play(mpMorf, &eyePos, sndId, reverb);
        mDoExt_McaMorf* morf = mpMorf;
        s8 a0F = mA0F;
        if (morf->getFrame() < m994) {
            morf = mpMorf;
            m9D0 = 1;
        }
        m994 = morf->getFrame();
        if (a0F == 4 && mpMorf->checkFrame(10.0f)) {
            mDoAud_seStart(0x5817, &eyePos, 0, dComIfGp_getReverb(current.roomNo));
        }
        McaMorf_play(mpMorf824, &eyePos, 0, 0);
        mDoExt_McaMorf* morf81C = mpMorf81C;
        if (morf81C != nullptr && m9D5 < 2) {
            morf81C->mFrameCtrl.mFrame = (f32)(s16)gabi::ftoi(mpMorf->getFrame());
        }
        m9DC = (gabi::load<u32>(gabi::ea(this) + 0x478) >> 5) & 1; /* mObjAcch.ChkGroundHit() */
        mObjAcch.CrrPos(dComIfG_Bgsp());
    }
    cBgS_PolyInfo* gndPoly = gabi::at<cBgS_PolyInfo>(gabi::ea(this) + 0x538);
    tevStr.mRoomNo = dBgS_GetRoomId(dComIfG_Bgsp(), gndPoly);
    gabi::store<u8>(gabi::ea(&tevStr) + 0xBA, dBgS_GetPolyColor(dComIfG_Bgsp(), gndPoly)); /* tevStr.mEnvrIdxOverride */

    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(m946.y);
    J3DModel_setBaseTRMtx(mpMorf->getModel(), mDoMtx_stack_c::get());
    McaMorf_calc(mpMorf);
    {
        J3DModel* model = mpMorf->getModel();
        s8 jnt = m7E4;
        J3DModel* dst = mpMorf824->getModel(); /* read before getAnmMtx's flag store */
        J3DModel_setBaseTRMtx(dst, getAnmMtx(model, jnt));
    }
    McaMorf_calc(mpMorf824);
    if (mpMorf81C.get() != nullptr) {
        McaMorf_calc(mpMorf81C);
    }
    J3DModel* itemModel = mpModel820;
    if (itemModel != nullptr) {
        J3DModel_setBaseTRMtx(itemModel, getAnmMtx(mpMorf->getModel(), m7E6));
        J3DModel_calc(mpModel820);
    }
    gabi::call(0x02274A1C, this, i_setEyePos); /* setAttention(i_setEyePos) */
}
VERIFY(0x02274A7C, &daNpc_Ko1_c::setMtx);

/* 02275544 */
u8 daNpc_Ko1_c::demo() {
    WWHD_FUNC(0x02275544, u8, this);
    if (demoActorID == 0) {
        if (m9E7 != 0) {
            m9E7 = 0;
        }
        return 0;
    }
    u8 id = demoActorID;
    m9E7 = 1;
    /* dComIfGp_demo_getActor(demoActorID): HD inline with a range check and the demo object
     * (0x101D5FFC) asserted */
    void* demo_actor = nullptr;
    if (id != 0 && id <= 0x20) {
        u32 obj = gabi::load<u32>(0x101D5FFC);
        if (obj == 0) { /* JUT_ASSERT(570, m_object != NULL) (d_demo.h) */
            JUT_ASSERT_fail(STR(0x1001C984), 0x23A, STR(0x1001C974));
            obj = gabi::load<u32>(0x101D5FFC);
        }
        demo_actor = gabi::call<void*>(0x02526E70, obj, id); /* dDemo_object_c::getActor */
    }
    if (m_hed_tex_pttrn.get() != nullptr) {
        u8 frame = (u8)(mBlinkFrame + 1);
        mBlinkFrame = frame;
        if ((s32)frame >= J3DAnm_getFrameMax(m_hed_tex_pttrn)) {
            mBlinkFrame = (u8)J3DAnm_getFrameMax(m_hed_tex_pttrn);
        }
    }
    if (demo_actor != nullptr) {
        J3DAnmTexPattern* demopattern = gabi::call<J3DAnmTexPattern*>(0x02527828, demo_actor, STR(0x1001CB7E)); /* getP_BtpData */
        if (demopattern != nullptr) {
            m_hed_tex_pttrn = demopattern;
            J3DModelData* md = J3DModel_getModelData_l(mpMorf824->getModel());
            if (mDoExt_btpAnm_init(mBtpAnm, md, demopattern, 1, 2, 1.0f, 0, -1, 1, 0)) {
                mBlinkFrame = 0;
                mBtpNum = 4;
            }
        }
    }
    dDemo_setDemoData(this, 0x6A, mpMorf, STR(0x1001CB7E), 0, nullptr, 0, 0);
    return m9E7;
}
VERIFY(0x02275544, &daNpc_Ko1_c::demo);

/* 0227684C */
BOOL daNpc_Ko1_c::_draw() {
    WWHD_FUNC(0x0227684C, BOOL, this);
    J3DModel* hedModel = mpMorf824->getModel();
    J3DModelData* hedData = J3DModel_getModelData_l(hedModel);
    J3DModel* model = mpMorf->getModel();
    if (m9D7 != 0 || m9DA != 0) {
        return TRUE;
    }
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), model, &tevStr);
    dComIfGp_get(); /* HD: unused */
    if ((u32)(s32)mType <= 1) {
        mpMorf->entryDL();
    }
    mDoExt_btpAnm_entry((mDoExt_btpAnm*)(void*)mBtpAnm, hedData, mBlinkFrame);
    mpMorf824->entryDL();
    gabi::store<u32>(gabi::ea(hedData) + 0x38, 0); /* mBtpAnm.remove(hedData) */
    setLightTevColorType(dKy_getEnvlight(), hedModel, &tevStr);
    if (mpMorf81C.get() != nullptr) {
        mpMorf81C->entryDL();
        mDoExt_McaMorf* morf = mpMorf81C;
        setLightTevColorType(dKy_getEnvlight(), morf->getModel(), &tevStr);
    }
    if (mpModel820.get() != nullptr) {
        setLightTevColorType(dKy_getEnvlight(), mpModel820, &tevStr);
        mDoExt_modelEntryDL(mpModel820);
    }
    /* HD: no shadowDraw() */
    switch ((u32)(s32)mType) {
    case 0:
        dSnap_RegistFig(0x52, this, 1.0f, 1.0f, 1.0f);
        break;
    case 1:
        dSnap_RegistFig(0x51, this, 1.0f, 1.0f, 1.0f);
        break;
    }
    /* debug leftovers: function-local static colors initialised on first use */
    if (gabi::load<u8>(l_HIO_child(mType) + 0x1C)) {
        local_static_init(0x101FDA50, 0x101FEBF4, 0x1001C868);
        local_static_init(0x101FDAC0, 0x101FEBF8, 0x1001C86C);
        local_static_init(0x101FDA48, 0x101FEBEC, 0x1001C870);
        local_static_init(0x101FDA50, 0x101FEBF4, 0x1001C868);
        local_static_init(0x101FDA48, 0x101FEBEC, 0x1001C870);
        local_static_init(0x101FDA44, 0x101FEBE8, 0x1001C874);
    }
    return TRUE;
}
VERIFY(0x0227684C, &daNpc_Ko1_c::_draw);

/* 022765B0 */
BOOL daNpc_Ko1_c::_execute() {
    WWHD_FUNC(0x022765B0, BOOL, this);
    if (!mbRanExecute) {
        mInitialPos.copy(current.pos);
        mInitialAngle.x = current.angle.x;
        mInitialAngle.y = current.angle.y;
        mInitialAngle.z = current.angle.z;
        mbRanExecute = 1;
    }
    u32 hio = l_HIO_child(mType);
    m_jnt.setParam(gabi::load<s16>(hio + 0xC), gabi::load<s16>(hio + 0xE), gabi::load<s16>(hio + 0x10),
                   gabi::load<s16>(hio + 0x12), gabi::load<s16>(hio + 0x4), gabi::load<s16>(hio + 0x6),
                   gabi::load<s16>(hio + 0x8), gabi::load<s16>(hio + 0xA), gabi::load<s16>(hio + 0x14));
    if (m9D7 != 0 && demoActorID == 0) {
        return TRUE;
    }
    m9D9 = 0;
    m9D7 = 0;
    partner_srch();
    checkOrder();
    if (!demo()) {
        s32 staffIdx;
        if (dComIfGp_event_runCheck() && gabi::load<u16>(gabi::ea(this) + 0xF8) != 1 /* !eventInfo.checkCommandTalk() */ &&
            (staffIdx = isEventEntry()) >= 0) {
            event_proc(staffIdx);
        } else {
            pmf_call(this, &mCurrProcFunc, nullptr); /* (this->*mCurrProcFunc)(NULL) */
        }
        if (!m9D9) {
            ko_nMove();
            fopAcM_posMoveF(this, gabi::at<cXyz>(gabi::ea(&mStts))); /* mStts.GetCCMoveP() */
        }
        void* pla = dBgS_GetTriPla(dComIfG_Bgsp(), gabi::at<u8>(gabi::ea(this) + 0x538) /* ground poly */);
        u8 keepShape = m9D8;
        if (pla != nullptr) {
            m988.copy(*(cXyz*)pla); /* the ground plane normal */
        }
        if (!keepShape) {
            m946.z = current.angle.z;
            shape_angle.y = current.angle.y;
            m946.y = current.angle.y;
            shape_angle.x = current.angle.x;
            shape_angle.z = current.angle.z;
            m946.x = current.angle.x;
        }
        gabi::Local<cXyz> diff;
        cXyz_mi(&current.pos, diff, &m97C);
        gabi::Local<cXyz> diffXZ;
        diffXZ->set(diff->x, 0.0f, diff->z);
        if (std_sqrtf(PSVECSquareMag(diffXZ)) > 3000.0f) {
            fopAcM_delete(this);
            return TRUE;
        }
    }
    eventOrder();
    setMtx(0);
    if (!m9E7) {
        setCollision(30.0f, 80.0f);
    }
    return TRUE;
}
VERIFY(0x022765B0, &daNpc_Ko1_c::_execute);
