/**
 * d_a_npc_kk1_c.cpp (WWHD)
 * NPC - Mila (poor, Windfall): part C (talk animation attributes, messages, checks, the
 * wait/walk/comment actions).
 *
 * The GameCube TU is "Nonmatching": written from the
 * WWHD code (cking.rpx) and verified against it, with the GameCube names.
 */
#define SAFESTRING_VTBL 0x1001C250 /* this TU's sead::SafeString vtable */
#include <math.h>
#include "d/actor/d_a_npc_kk1.h"

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 0200F974 cLib_targetAngleX(const cXyz*, const cXyz*) */
static inline s16 cLib_targetAngleX(const cXyz* a, const cXyz* b) { return gabi::call<s16>(0x0200F974, a, b); }
/* 025D5928 fopAcM_fastCreate(name, param, pos, roomNo, angle, scale, subtype, createFunc, data) */
static inline fopAc_ac_c* fopAcM_fastCreate_l(s16 name, u32 param, cXyz* pos, s32 roomNo, csXyz* angle, cXyz* scale,
                                              s8 subtype, u32 createFunc, void* data) {
    return gabi::call<fopAc_ac_c*>(0x025D5928, name, param, pos, roomNo, angle, scale, subtype, createFunc, data);
}
/* 025163BC dCcD_GObjInf::GetCoHitObj: cCcD_Obj*; its dCcD_Stts* at +0x44, the stts' actor at +0xC */
static inline u32 dCcD_GetCoHitObj(dCcD_GObjInf* o) { return gabi::call<u32>(0x025163BC, o); }
/* 028E8DE8 PSVECSquareDistance */
static inline f32 PSVECSquareDistance(const cXyz* a, const cXyz* b) { return gabi::call<f32>(0x028E8DE8, a, b); }
/* 025E19CC mDoAud_seStart(id, pos) (two-argument form) */
static inline void mDoAud_seStart2(u32 id, cXyz* pos) { gabi::call(0x025E19CC, id, pos); }
/* 0259EC0C dNpc_PathRun_c::decIdx, 0259ED58 nextIdxAuto (bool), 0259EDB8 maxPoint */
static inline void dNpc_PathRun_decIdx(dNpc_PathRun_l* p) { gabi::call(0x0259EC0C, p); }
static inline u8 dNpc_PathRun_nextIdxAuto(dNpc_PathRun_l* p) { return gabi::call<u8>(0x0259ED58, p); }
static inline u8 dNpc_PathRun_maxPoint(dNpc_PathRun_l* p) { return gabi::call<u8>(0x0259EDB8, p); }
/* 0252624C (matcher: daObj_Roten_c::getCreateCount; static, no arguments) */
static inline s32 daObj_Roten_getCreateCount() { return gabi::call<s32>(0x0252624C); }
/* HD message manager (*101F4B5C): +0x948 the selected answer */
static inline u32 msgMng_getSelectNum() { return gabi::load<u32>(gabi::load<u32>(0x101F4B5C) + 0x948); }
/* g_Counter.mCounter0 */
static inline u32 g_Counter0() { return gabi::load<u32>(0x101FF558); }

/* ---- calls into the other parts of the TU (by address) ---- */
/* 0226CC2C daNpc_Kk1_c::searchByID(fpc_ProcID, int* o_notFound) (matcher: cLib_getRndValue<int>) */
static inline fopAc_ac_c* kk1_searchByID(daNpc_Kk1_c* self, u32 id, be<s32>* res) {
    return gabi::call<fopAc_ac_c*>(0x0226CC2C, self, id, res);
}
/* 0226B5B4 (unnamed): the current path point (dNpc_PathRun_c::getPoint(mIdx), shifted at point 0x11) */
static inline void kk1_getPathPoint(cXyz* out, dNpc_PathRun_l* p) { gabi::call(0x0226B5B4, out, p); }

/* ---- l_HIO (0x10467698, daNpc_Kk1_HIO_c 0x60: vtable, mNo, field_0x8, prm table at +0xC) ---- */
static inline s16 hio_s16(u32 off) { return gabi::load<s16>(0x10467698 + off); }
static inline f32 hio_f32(u32 off) { return gabi::load<f32>(0x10467698 + off); }

static inline fopAc_ac_c* linkPlayer() { return dComIfGp_getLinkPlayer(); }
static inline s32 abs_s16(s16 v) { return v < 0 ? -(s32)v : (s32)v; }

/* 0226EF4C */
void daNpc_Kk1_c::setAnm_ATR() {
    WWHD_FUNC(0x0226EF4C, void, this);
    anm_prm_c* tbl = gabi::at<anm_prm_c>(0x101BF51C); /* a_anm_prm_tbl (0x10 bytes each) */
    init_texPttrnAnm(tbl[(u8)mAC3].mTexNo, true);
    setAnm_anm(&tbl[(u8)mAC3]);
}
VERIFY(0x0226EF4C, &daNpc_Kk1_c::setAnm_ATR);

/* 0226EFB4 */
void daNpc_Kk1_c::chngAnmAtr(u8 i_attr) {
    WWHD_FUNC(0x0226EFB4, void, this, i_attr);
    if (mCurrMsgNo == 0x1CAC) {
        gabi::Local<be<s32>> notFound;
        fopAc_ac_c* partner = kk1_searchByID(this, m86C, notFound);
        if (partner != nullptr && *notFound == 0) {
            m8A8.x = partner->current.pos.x;
            f32 y = partner->current.pos.y;
            m8A8.y = y;
            m8A8.z = partner->current.pos.z;
            m934 = 1;
            mACA = 2;
            m8A8.y = y + (hio_f32(0x20) + 200.0f);
            m912 = hio_s16(0x2E);
        }
    }
    if (i_attr != mAC3 && i_attr < 0xD) {
        mAC3 = i_attr;
        setAnm_ATR();
    }
}
VERIFY(0x0226EFB4, &daNpc_Kk1_c::chngAnmAtr);

/* 0226F088 */
void daNpc_Kk1_c::ctrlAnmAtr() {
    WWHD_FUNC(0x0226F088, void, this);
    switch ((u8)mAC3) {
    case 8:
        if ((s8)m922 != 0) {
            current.angle.y = (s16)(current.angle.y - 0x8000);
            setAnm_NUM(0, 1);
            mpMorf->setMorf(0.0f);
            mAC3 = 0;
        }
        break;
    case 10:
        if ((s8)m922 != 0) {
            setAnm_NUM(0, 1);
            mAC3 = 0;
        }
        break;
    case 11:
        if ((s8)m922 != 0) {
            setAnm_NUM(6, 1);
            mAC3 = 6;
        }
        break;
    }
}
VERIFY(0x0226F088, &daNpc_Kk1_c::ctrlAnmAtr);

/* 0226F170 */
void daNpc_Kk1_c::anmAtr(u16 i_msgStatus) {
    WWHD_FUNC(0x0226F170, void, this, i_msgStatus);
    if (i_msgStatus == 6 /* fopMsgStts_MSG_TYPING_e */) {
        if ((s8)mACE == 0) {
            chngAnmAtr(dComIfGp_getMesgAnimeAttrInfo());
            mACE = mACE + 1;
        }
        u8 tag = dComIfGp_getMesgAnimeTagInfo();
        if (tag != 0xFF && tag != mAC4) {
            dComIfGp_clearMesgAnimeTagInfo();
            mAC4 = tag;
        }
    } else if (i_msgStatus == 14 /* fopMsgStts_MSG_DISPLAYED_e */) {
        mACE = 0;
    }
    ctrlAnmAtr();
}
VERIFY(0x0226F170, &daNpc_Kk1_c::anmAtr);

/* 0226F230 */
u16 daNpc_Kk1_c::next_msgStatus(be<u32>* pMsgNo) {
    WWHD_FUNC(0x0226F230, u16, this, pMsgNo);
    switch ((u32)*pMsgNo) {
    case 0x1C85: *pMsgNo = 0x1C86; break;
    case 0x1C86: *pMsgNo = 0x1CA7; break;
    case 0x1C88: *pMsgNo = 0x1C89; break;
    case 0x1C89: *pMsgNo = 0x1C8A; break;
    case 0x1C8A: *pMsgNo = 0x1CA9; break;
    case 0x1C8B:
        *pMsgNo = 0x1CAC;
        m928 = 1;
        break;
    case 0x1C8D: *pMsgNo = 0x1C8E; break;
    case 0x1C8E: *pMsgNo = 0x1C8F; break;
    case 0x1C8F: {
        u32 sel = msgMng_getSelectNum();
        if (sel == 0) {
            *pMsgNo = 0x1C90;
        } else if (sel == 1) {
            *pMsgNo = 0x1CA5;
            m91E = m91E + 1;
        }
        break;
    }
    case 0x1C90: *pMsgNo = 0x1C91; break;
    case 0x1C91: {
        u32 sel = msgMng_getSelectNum();
        if (sel == 0) {
            *pMsgNo = 0x1C93;
        } else if (sel == 1) {
            *pMsgNo = 0x1C92;
            m91E = m91E + 1;
        }
        break;
    }
    case 0x1C92: *pMsgNo = 0x1C91; break;
    case 0x1C93: *pMsgNo = 0x1C94; break;
    case 0x1C94: {
        u32 sel = msgMng_getSelectNum();
        if (sel == 0) {
            *pMsgNo = 0x1C96;
        } else if (sel == 1) {
            *pMsgNo = 0x1C95;
            m91E = m91E + 1;
        }
        break;
    }
    case 0x1C95: *pMsgNo = 0x1C96; break;
    case 0x1C96: *pMsgNo = 0x1C97; break;
    case 0x1C97: {
        u32 sel = msgMng_getSelectNum();
        if (sel == 0) {
            *pMsgNo = 0x1C98;
        } else if (sel == 1) {
            *pMsgNo = m91E > 1 ? 0x1C9A : 0x1C9B;
        }
        break;
    }
    case 0x1C98: {
        u32 sel = msgMng_getSelectNum();
        if (sel == 0) {
            *pMsgNo = 0x1C99;
        } else if (sel == 1) {
            return 0x10; /* fopMsgStts_MSG_ENDS_e */
        }
        break;
    }
    case 0x1C99: {
        u32 sel = msgMng_getSelectNum();
        if (sel == 0) {
            *pMsgNo = m91E > 1 ? 0x1C9C : 0x1C9D;
        } else if (sel == 1) {
            *pMsgNo = 0x1C9A;
        }
        break;
    }
    case 0x1C9B: *pMsgNo = 0x1C9D; break;
    case 0x1C9D: *pMsgNo = 0x1C9E; break;
    case 0x1C9E: *pMsgNo = 0x1CAA; break;
    case 0x1CA1: *pMsgNo = 0x1CA2; break;
    case 0x1CA2: *pMsgNo = 0x1CA3; break;
    case 0x1CA3: *pMsgNo = 0x1CA4; break;
    case 0x1CA4: *pMsgNo = 0x1CAB; break;
    case 0x1CA5: *pMsgNo = 0x1C8F; break;
    case 0x1CA7: *pMsgNo = 0x1CA8; break;
    case 0x1CAA: *pMsgNo = 0x1C9F; break;
    default:
        return 0x10; /* fopMsgStts_MSG_ENDS_e */
    }
    return 0xF; /* fopMsgStts_MSG_CONTINUES_e */
}
VERIFY(0x0226F230, &daNpc_Kk1_c::next_msgStatus);

/* 0226F538 */
u32 daNpc_Kk1_c::getMsg_KK1_0() {
    WWHD_FUNC(0x0226F538, u32, this);
    if (dKy_daynight_check() == 1) {
        if (m92A != 0) {
            m92A = 0;
            return 0x1CA1;
        }
        return 0x1C8B;
    }
    if (dComIfGs_isEventBit(0xE08)) {
        return 0x1C85;
    }
    if (dComIfGs_isEventBit(0xE10)) {
        return 0x1C87;
    }
    return 0x1C88;
}
VERIFY(0x0226F538, &daNpc_Kk1_c::getMsg_KK1_0);

/* 0226F61C */
u32 daNpc_Kk1_c::getMsg() {
    WWHD_FUNC(0x0226F61C, u32, this);
    u32 msg = 0;
    if ((s8)mACC == 0) {
        msg = getMsg_KK1_0();
    }
    return msg;
}
VERIFY(0x0226F61C, &daNpc_Kk1_c::getMsg);

/* 0226F654 */
bool daNpc_Kk1_c::chk_talk() {
    WWHD_FUNC(0x0226F654, bool, this);
    if (dComIfGp_event_chkTalkXY()) {
        if (dComIfGp_evmng_ChkPresentEnd()) {
            m924 = dComIfGp_event_getPreItemNo();
            return true;
        }
        return false;
    }
    m924 = 0xFF;
    return true;
}
VERIFY(0x0226F654, &daNpc_Kk1_c::chk_talk);

/* 0226F6EC */
u8 daNpc_Kk1_c::chk_parts_notMov() {
    WWHD_FUNC(0x0226F6EC, u8, this);
    if (m8E0.y != m_jnt.mAngles[0][1] || m8E0.z != m_jnt.mAngles[1][1] || m8E0.x != current.angle.y) {
        return 1;
    }
    return 0;
}
VERIFY(0x0226F6EC, &daNpc_Kk1_c::chk_parts_notMov);

/* 0226F72C */
BOOL daNpc_Kk1_c::chkAttention() {
    WWHD_FUNC(0x0226F72C, BOOL, this);
    dAttention_c* attention = dComIfGp_getAttention();
    if (dAttention_LockonTruth(attention)) {
        return this == (void*)dAttention_LockonTarget(attention, 0);
    }
    return this == (void*)dAttention_ActionTarget(attention, 0);
}
VERIFY(0x0226F72C, &daNpc_Kk1_c::chkAttention);

/* 0226F7B4 */
void daNpc_Kk1_c::createTama(f32 i_speed) {
    WWHD_FUNC(0x0226F7B4, void, this, i_speed);
    gabi::Local<csXyz> angle;
    csXyz_ct(angle, 0, 0, 0);
    gabi::Local<cXyz> start;
    start->x = eyePos.x;
    start->y = eyePos.y + 15.0f;
    start->z = eyePos.z;
    gabi::Local<cXyz> dist;
    dNpc_playerEyePos_l(dist, -20.0f);
    gabi::Local<cXyz> target;
    target->copy(*dist);
    cXyz_mi(target, dist, &eyePos);
    gabi::Local<cXyz> xz;
    xz->x = dist->x;
    xz->z = dist->z;
    xz->y = 0.0f;
    std_sqrtf(PSVECSquareMag(xz)); /* dist.absXZ(): unused */
    angle->y = cLib_targetAngleY(start, target);
    angle->x = cLib_targetAngleX(start, target);
    fopAc_ac_c* tama = fopAcM_fastCreate_l(0x1D0 /* PROC_KANTERA? tama */, 0, &eyePos, current.roomNo, angle, nullptr, -1, 0, nullptr);
    if (tama != nullptr) {
        gabi::store<f32>(gabi::ea(tama) + 0x71C, i_speed);
        gabi::store<u32>(gabi::ea(tama) + 0x3AC, gabi::load<u32>(gabi::ea(this) + 4)); /* fopAcM_GetID(this) */
        tama->speedF = 50.0f;
    }
}
VERIFY(0x0226F7B4, &daNpc_Kk1_c::createTama);

/* 0226F908 */
bool daNpc_Kk1_c::chk_areaIN(f32 i_radius, cXyz* i_center) {
    WWHD_FUNC(0x0226F908, bool, this, i_radius, i_center);
    gabi::Local<cXyz> diff;
    cXyz_mi(&linkPlayer()->current.pos, diff, i_center);
    gabi::Local<cXyz> xz;
    xz->z = diff->z;
    xz->y = 0.0f;
    xz->x = diff->x;
    f32 dist = std_sqrtf(PSVECSquareMag(xz));
    s16 angle = (s16)(cLib_targetAngleY(&current.pos, &linkPlayer()->current.pos) - current.angle.y);
    f32 tamaSpeed = i_radius;
    if (abs_s16(angle) > 0x4AAA) {
        tamaSpeed = i_radius * 0.5f;
    }
    bool in = dist < i_radius;
    if (in) {
        u32 link = gabi::ea(linkPlayer());
        if (link != gabi::ea(dComIfGp_getPlayer(0))) {
            u32 pl = gabi::ea(dComIfGp_getPlayer(0));
            gabi::store<u32>(pl + 0x600, gabi::load<u32>(pl + 0x600) | 1);
            return false;
        }
        if (g_Counter0() % 3 == 0) {
            createTama(tamaSpeed);
        }
    }
    return in;
}
VERIFY(0x0226F908, &daNpc_Kk1_c::chk_areaIN);

/* 0226FABC */
bool daNpc_Kk1_c::startEvent_check() {
    WWHD_FUNC(0x0226FABC, bool, this);
    gabi::Local<cXyz> pos;
    pos->x = current.pos.x;
    pos->y = current.pos.y;
    pos->z = current.pos.z;
    if (chk_areaIN(hio_f32(0x5C), pos)) {
        f32 dist = std_sqrtf(PSVECSquareDistance(&current.pos, &linkPlayer()->current.pos));
        if (dist < gabi::load<f32>(0x1047BB20) + 210.0f || field_0x7d6 != 0) {
            return true;
        }
    }
    return false;
}
VERIFY(0x0226FABC, &daNpc_Kk1_c::startEvent_check);

/* 0226FB70 */
BOOL daNpc_Kk1_c::chkHitPlayer() {
    WWHD_FUNC(0x0226FB70, BOOL, this);
    BOOL hit = FALSE;
    if (mCyl.ChkCoHit()) {
        u32 obj = dCcD_GetCoHitObj(&mCyl);
        if (obj != 0) {
            u32 stts = gabi::load<u32>(obj + 0x44);
            if (stts == 0) {
                return FALSE;
            }
            u32 actor = gabi::load<u32>(stts + 0xC);
            if (actor != 0) {
                hit = gabi::load<s16>(actor + 8) == 0xA8; /* fopAcM_GetName(actor) == PROC_PLAYER */
            }
        }
    }
    return hit;
}
VERIFY(0x0226FB70, &daNpc_Kk1_c::chkHitPlayer);

/* 0226FC1C */
u8 daNpc_Kk1_c::chk_attn() {
    WWHD_FUNC(0x0226FC1C, u8, this);
    gabi::Local<cXyz> diff;
    cXyz_mi(&current.pos, diff, &linkPlayer()->current.pos);
    gabi::Local<cXyz> xz;
    xz->y = 0.0f;
    xz->x = diff->x;
    xz->z = diff->z;
    f32 dist = std_sqrtf(PSVECSquareMag(xz));
    fopAc_ac_c* link = linkPlayer();
    f32 dy = current.pos.y - link->current.pos.y;
    s16 angle = (s16)(cLib_targetAngleY(&current.pos, &linkPlayer()->current.pos) - current.angle.y);
    u8 ret = 0;
    f32 maxAngle = (s8)mACA == 1 ? 90.0f : 60.0f;
    if (dist < 200.0f) {
        f32 deg = (f32)abs_s16(angle) / 182.04444885253906f;
        if (deg < maxAngle && fabsf(dy) < 300.0f) {
            ret = 1;
        }
    }
    return ret;
}
VERIFY(0x0226FC1C, &daNpc_Kk1_c::chk_attn);

/* 0226FE78 */
BOOL daNpc_Kk1_c::wait_1() {
    WWHD_FUNC(0x0226FE78, BOOL, this);
    if (m933 != 0) {
        if (chk_talk()) {
            setStt(2);
            setAnm_NUM(0, 1);
            m934 = 0;
            m_jnt.mbTrn = 1;
            mACA = 1;
        }
        return TRUE;
    }
    u8 onPath = m926;
    mACA = 0;
    m934 = 1;
    mAC7 = 2;
    if (onPath != 0) {
        gabi::Local<cXyz> pnt;
        kk1_getPathPoint(pnt, &mPathRun);
        s16 target = cLib_targetAngleY(&current.pos, pnt);
        cLib_addCalcAngleS(&current.angle.y, target, hio_s16(0x30), hio_s16(0x32), 0x80);
        if (abs_s16((s16)(target - current.angle.y)) < 0x1800) {
            setStt(3);
            m927 = 0;
        }
        return TRUE;
    }
    if ((s8)mAC6 == 0xB) {
        if ((s8)m922 != 0) {
            setAnm_NUM(0, 1);
            m914 = cLib_getRndValue(60, 30);
            m927 = 1;
        }
        return TRUE;
    }
    gabi::Local<cXyz> offset;
    offset->z = 0.0f;
    offset->x = -100.0f;
    offset->y = 0.0f;
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(m888.y);
    gabi::Local<cXyz> pnt;
    PSMTXMultVec(mDoMtx_stack_c::get(), offset, pnt);
    s16 target = cLib_targetAngleY(&current.pos, pnt);
    cLib_addCalcAngleS(&current.angle.y, target, hio_s16(0x30), hio_s16(0x32), 0x80);
    s16 diff = (s16)(target - current.angle.y);
    if (m927 == 0) {
        if (diff == 0) {
            setAnm_NUM(0xB, 1);
        }
        return TRUE;
    }
    if (cLib_calcTimer(&m914) != 0) {
        return TRUE;
    }
    if (mPathRun.mPath.get() != nullptr && dNpc_PathRun_maxPoint(&mPathRun) > 2 && daObj_Roten_getCreateCount() > 1) {
        m926 = 1;
        return TRUE;
    }
    m927 = 0;
    return TRUE;
}
VERIFY(0x0226FE78, &daNpc_Kk1_c::wait_1);

/* 022700E0 */
BOOL daNpc_Kk1_c::walk_1() {
    WWHD_FUNC(0x022700E0, BOOL, this);
    gabi::Local<cXyz> pnt;
    kk1_getPathPoint(pnt, &mPathRun);
    if (gabi::load<u8>(gabi::ea(mPathRun.mPath.get()) + 5) & 1) { /* the path is a loop */
        return TRUE;
    }
    gabi::Local<cXyz> diff;
    cXyz_mi(&current.pos, diff, pnt);
    gabi::Local<cXyz> xz;
    xz->y = 0.0f;
    xz->x = diff->x;
    xz->z = diff->z;
    f32 dist = std_sqrtf(PSVECSquareMag(xz));
    f32 speed;
    s16 oldAngle;
    if (m926 == 0) {
        s16 target = cLib_targetAngleY(&current.pos, pnt);
        oldAngle = current.angle.y;
        cLib_addCalcAngleS(&current.angle.y, target, hio_s16(0x30), hio_s16(0x32), 0x80);
        speed = hio_f32(0x38);
        if (m926 == 0) {
            speed = 0.0f;
        } else if (m933 != 0 || chk_attn()) {
            speed = 0.0f;
        }
    } else {
        if (dist < hio_f32(0x34)) {
            bool turn = true;
            if (dNpc_PathRun_nextIdxAuto(&mPathRun) == 1) {
                if (mPathRun.mbDir == 0) {
                    turn = false;
                } else {
                    s32 count = daObj_Roten_getCreateCount();
                    if ((s32)mPathRun.mIdx < count) {
                        turn = false;
                    } else {
                        dNpc_PathRun_decIdx(&mPathRun);
                        dNpc_PathRun_decIdx(&mPathRun);
                    }
                }
            }
            if (turn) {
                u8 dir = mPathRun.mbDir;
                m926 = 0;
                mPathRun.mbDir = dir ^ 1;
            }
        }
        s16 target = cLib_targetAngleY(&current.pos, pnt);
        oldAngle = current.angle.y;
        cLib_addCalcAngleS(&current.angle.y, target, hio_s16(0x30), hio_s16(0x32), 0x80);
        speed = hio_f32(0x38);
        if (m926 == 0) {
            speed = 0.0f;
        } else if (m933 != 0 || chk_attn()) {
            speed = 0.0f;
        }
    }
    cLib_chaseF(&speedF, speed, hio_f32(0x3C));
    f32 rate = speedF * hio_f32(0x40);
    mpMorf->setPlaySpeed((rate - 0.5f >= 0.0f) ? rate : 0.5f);
    if (gabi::ftoi(speed) == 0) {
        s32 curSpeed = gabi::ftoi(speedF);
        current.angle.y = oldAngle;
        if (curSpeed != 0)
            goto walking;
        if (m933 != 0) {
            if (chk_talk()) {
                setStt(1);
                setAnm_NUM(0, 1);
                m934 = 0;
                m_jnt.mbTrn = 1;
                mACA = 1;
            }
            return TRUE;
        }
        if (m926 == 0) {
            setStt(1);
        } else {
            setStt(7);
        }
        m927 = 0;
        m926 = 0;
        return TRUE;
    }
walking:
    m934 = 1;
    mAC7 = 2;
    mACA = 0;
    return TRUE;
}
VERIFY(0x022700E0, &daNpc_Kk1_c::walk_1);

/* 022703B4 */
BOOL daNpc_Kk1_c::wait_2() {
    WWHD_FUNC(0x022703B4, BOOL, this);
    cLib_addCalcAngleS(&current.angle.y, m888.y, 4, 0x800, 0x80);
    if (m933 != 0) {
        if (chk_talk()) {
            setStt(2);
            m934 = 0;
            m935 = 0;
            mACA = 1;
            m_jnt.mbTrn = 1;
        }
        return TRUE;
    }
    s8 state = (s8)mAC7;
    mACA = 0;
    m934 = 1;
    if (state == 3 || state == 4) {
        return TRUE;
    }
    if (m928 != 0 && m925 != 0xFF && dComIfGs_isSwitch(m925, current.roomNo)) {
        gabi::Local<be<s32>> notFound;
        fopAc_ac_c* partner = kk1_searchByID(this, m86C, notFound);
        if (partner != nullptr && *notFound == 0) {
            s16 angle = (s16)(cLib_targetAngleY(&partner->current.pos, &linkPlayer()->current.pos) - partner->current.angle.y);
            mAC7 = abs_s16(angle) >= 0x4000 ? 3 : 4;
            gabi::store<u8>(gabi::ea(&mStts) + 0x14, 0xD9); /* mStts weight */
            return TRUE;
        }
    }
    mAC7 = 2;
    return TRUE;
}
VERIFY(0x022703B4, &daNpc_Kk1_c::wait_2);

/* 0227051C */
void daNpc_Kk1_c::init_CMT_WAI() {
    WWHD_FUNC(0x0227051C, void, this);
    m914 = cLib_getRndValue(0x5A, 0xB4);
    mACA = 5;
    m934 = 1;
    setAnm_NUM(0, 1);
}
VERIFY(0x0227051C, &daNpc_Kk1_c::init_CMT_WAI);

/* the actor's position, as a by-value cXyz argument */
static inline void copy_pos(cXyz* dst, cXyz* src) {
    dst->x = src->x;
    dst->y = src->y;
    dst->z = src->z;
}
static inline bool not_talking(s8 state) { return state != 1 && state < 3; }

/* 02270570 */
void daNpc_Kk1_c::move_CMT_WAI() {
    WWHD_FUNC(0x02270570, void, this);
    s16 timer = cLib_calcTimer(&m914);
    s8 state = (s8)mAC7;
    if (timer == 0) {
        if (not_talking(state)) {
            gabi::Local<cXyz> pos;
            copy_pos(pos, &current.pos);
            if (chk_areaIN(hio_f32(0x5C), pos)) {
                mAC7 = 8;
                return;
            }
        }
        mAC2 = 0;
        setAnm_NUM(3, 1);
        m926 = 1;
        return;
    }
    if (not_talking(state) && startEvent_check()) {
        mAC7 = 9;
    }
}
VERIFY(0x02270570, &daNpc_Kk1_c::move_CMT_WAI);

/* 0227066C */
void daNpc_Kk1_c::init_CMT_TRN() {
    WWHD_FUNC(0x0227066C, void, this);
    m91A = current.angle.y;
    m914 = cLib_getRndValue(0x5A, 0xB4);
    m908 = hio_s16(0x2C);
    m904 = hio_s16(0x28);
    mACA = 0;
    m902 = hio_s16(0x2A);
    m934 = 1;
    setAnm_NUM(0, 1);
}
VERIFY(0x0227066C, &daNpc_Kk1_c::init_CMT_TRN);

/* 022706E4 */
void daNpc_Kk1_c::move_CMT_TRN() {
    WWHD_FUNC(0x022706E4, void, this);
    s16 backAngle = (s16)(m91A + 0x8000);
    s16 oldAngle = current.angle.y;
    if (cLib_calcTimer(&m908) == 0) {
        if (m914 == 0) {
            gabi::Local<cXyz> pnt;
            kk1_getPathPoint(pnt, &mPathRun);
            s16 target = cLib_targetAngleY(&current.pos, pnt);
            cLib_addCalcAngleS(&current.angle.y, target, hio_s16(0x30), hio_s16(0x32), 0x80);
            if (!not_talking((s8)mAC7)) {
                return;
            }
            if (startEvent_check()) {
                mAC7 = 9;
                return;
            }
            if (current.angle.y != target) {
                return;
            }
            mAC2 = 0;
            setAnm_NUM(3, 1);
            m926 = 1;
            return;
        }
        cLib_addCalcAngleS(&current.angle.y, backAngle, hio_s16(0x30), hio_s16(0x32), 0x80);
        s16 angle = current.angle.y;
        if (angle == backAngle) {
            if (angle != oldAngle) {
                mACA = 5;
                m934 = 1;
            }
            if (cLib_calcTimer(&m914) == 0) {
                if (not_talking((s8)mAC7)) {
                    gabi::Local<cXyz> pos;
                    copy_pos(pos, &current.pos);
                    if (chk_areaIN(hio_f32(0x5C), pos)) {
                        mAC7 = 8;
                    }
                }
                mACA = 0;
                m934 = 1;
            }
        }
    }
    if (not_talking((s8)mAC7) && startEvent_check()) {
        mAC7 = 9;
    }
}
VERIFY(0x022706E4, &daNpc_Kk1_c::move_CMT_TRN);

/* 022708B8 */
void daNpc_Kk1_c::init_CMT_PCK() {
    WWHD_FUNC(0x022708B8, void, this);
    setAnm_NUM(1, 1);
    s16 evId = m8F0;
    m8FC = 2;
    m914 = hio_s16(0x26);
    mACA = 0;
    m934 = 1;
    gabi::store<s16>(gabi::ea(this) + 0xFC, evId); /* eventInfo.setEventId() */
}
VERIFY(0x022708B8, &daNpc_Kk1_c::init_CMT_PCK);

/* 02270918 */
void daNpc_Kk1_c::move_CMT_PCK() {
    WWHD_FUNC(0x02270918, void, this);
    if (m914 == 0) {
        gabi::Local<cXyz> pnt;
        kk1_getPathPoint(pnt, &mPathRun);
        s16 target = cLib_targetAngleY(&current.pos, pnt);
        cLib_addCalcAngleS(&current.angle.y, target, hio_s16(0x30), hio_s16(0x32), 0x80);
        if (!not_talking((s8)mAC7)) {
            return;
        }
        if (startEvent_check()) {
            mAC7 = 9;
            return;
        }
        if (current.angle.y != target) {
            return;
        }
        mAC2 = 0;
        setAnm_NUM(3, 1);
        m926 = 1;
        return;
    }
    if (m933 != 0) {
        return;
    }
    if (not_talking((s8)mAC7) && chkHitPlayer()) {
        mAC7 = 1;
        return;
    }
    if (cLib_calcTimer(&m914) == 0) {
        setAnm_NUM(0, 1);
        mAC7 = 0;
        gabi::store<s16>(gabi::ea(this) + 0xFC, -1); /* eventInfo.setEventId(-1) */
        return;
    }
    mDoAud_seStart2(0x509B, &current.pos);
    mAC7 = 2;
}
VERIFY(0x02270918, &daNpc_Kk1_c::move_CMT_PCK);

/* 02270AE0 */
BOOL daNpc_Kk1_c::cmmt_1() {
    WWHD_FUNC(0x02270AE0, BOOL, this);
    switch ((s8)mAC2) {
    case 1:
        move_CMT_WAI();
        return TRUE;
    case 4:
        move_CMT_TRN();
        return TRUE;
    case 5:
        move_CMT_PCK();
        return TRUE;
    }
    if (not_talking((s8)mAC7) && (s8)mAC6 != 1 && startEvent_check()) {
        mAC7 = 9;
    }
    mACA = 0;
    m934 = 1;
    if (event_move(true)) {
        if (!not_talking((s8)mAC7)) {
            return TRUE;
        }
        switch ((s8)mAC2) {
        case 1:
            init_CMT_WAI();
            break;
        case 4:
            init_CMT_TRN();
            break;
        case 5:
            init_CMT_PCK();
            break;
        default:
            mAC2 = 0;
            setAnm_NUM(3, 1);
            m926 = 1;
            break;
        }
    }
    if (not_talking((s8)mAC7)) {
        mAC7 = 0;
    }
    return TRUE;
}
VERIFY(0x02270AE0, &daNpc_Kk1_c::cmmt_1);

/* 02270C7C */
BOOL daNpc_Kk1_c::wait_3() {
    WWHD_FUNC(0x02270C7C, BOOL, this);
    gabi::Local<cXyz> diff;
    cXyz_mi(&current.pos, diff, &linkPlayer()->current.pos);
    gabi::Local<cXyz> xz;
    xz->x = diff->x;
    xz->y = 0.0f;
    xz->z = diff->z;
    bool far = std_sqrtf(PSVECSquareMag(xz)) > 300.0f;
    m935 = far;
    if (far) {
        cLib_addCalcAngleS(&current.angle.y, m888.y, 4, 0x800, 0x80);
    }
    if (m933 != 0) {
        if (chk_talk()) {
            setStt(2);
            m934 = 0;
            m935 = 0;
            mACA = 1;
            m_jnt.mbTrn = 1;
        }
        return TRUE;
    }
    mACA = 0;
    m934 = 1;
    u8 attn = chk_attn();
    s8 state = (s8)mAC7;
    if (attn) {
        mACA = 1;
    }
    if (not_talking(state)) {
        mAC7 = 2;
    }
    return TRUE;
}
VERIFY(0x02270C7C, &daNpc_Kk1_c::wait_3);

/* 02270DA8 */
BOOL daNpc_Kk1_c::wait_4() {
    WWHD_FUNC(0x02270DA8, BOOL, this);
    gabi::Local<cXyz> diff;
    cXyz_mi(&current.pos, diff, &linkPlayer()->current.pos);
    gabi::Local<cXyz> xz;
    xz->x = diff->x;
    xz->y = 0.0f;
    xz->z = diff->z;
    f32 dist = std_sqrtf(PSVECSquareMag(xz));
    if (m933 != 0) {
        if (chk_talk()) {
            setStt(2);
            setAnm_NUM(0, 1);
            m934 = 0;
            m_jnt.mbTrn = 1;
            mACA = 1;
        }
        return TRUE;
    }
    mAC7 = 2;
    m934 = 0;
    mACA = 1;
    bool far = dist > 300.0f;
    m926 = far;
    if (far) {
        gabi::Local<cXyz> pnt;
        kk1_getPathPoint(pnt, &mPathRun);
        s16 target = cLib_targetAngleY(&current.pos, pnt);
        cLib_addCalcAngleS(&current.angle.y, target, hio_s16(0x30), hio_s16(0x32), 0x80);
        if (abs_s16((s16)(target - current.angle.y)) < 0x1800) {
            setStt(3);
            m927 = 0;
        }
        mACA = 0;
        m934 = 1;
    }
    return TRUE;
}
VERIFY(0x02270DA8, &daNpc_Kk1_c::wait_4);

/* 02270F28 */
BOOL daNpc_Kk1_c::talk_1() {
    WWHD_FUNC(0x02270F28, BOOL, this);
    BOOL moving = chk_parts_notMov();
    talk(1);
    if (mbHasMsg == 0) {
        return TRUE;
    }
    /* HD: the message status comes from the message manager */
    if (fopMsgM_getStatus() == 0x13 /* fopMsgStts_MSG_DESTROYED_e */) {
        s8 stt = (s8)mAC9;
        m933 = 0;
        m924 = 0xFF;
        setStt(stt);
        m90E = cLib_getRndValue(0xF, 0x1E);
        switch ((u32)mCurrMsgNo) {
        case 0x1CA9:
            dComIfGs_onEventBit(0xE10);
            endEvent();
            break;
        case 0x1CAB:
            mAC7 = 7;
            endEvent();
            break;
        case 0x1CAC:
            m912 = 0;
            m934 = 0;
            mACA = 1;
            endEvent();
            break;
        default:
            endEvent();
            break;
        }
    }
    if (cLib_calcTimer(&m912) != 0 && m912 == 1) {
        m934 = 0;
        mACA = 1;
    }
    return moving;
}
VERIFY(0x02270F28, &daNpc_Kk1_c::talk_1);

/* 02271084 */
BOOL daNpc_Kk1_c::wait_action1(void*) {
    WWHD_FUNC(0x02271084, BOOL, this, (void*)nullptr);
    s8 step = (s8)mACD;
    if (step == 0) {
        if (dKy_daynight_check() == 0) {
            m927 = 0;
            setStt(1);
        } else {
            setStt(4);
        }
        mACD = mACD + 1;
        return TRUE;
    }
    if ((u32)(s32)step > 3) {
        return TRUE;
    }
    m932 = chkAttention();
    switch ((u32)(s32)(s8)mAC8) {
    case 1: m8E8 = wait_1(); break;
    case 2: m8E8 = talk_1(); break;
    case 3: m8E8 = walk_1(); break;
    case 4: m8E8 = wait_2(); break;
    case 5: m8E8 = cmmt_1(); break;
    case 6: m8E8 = wait_3(); break;
    case 7: m8E8 = wait_4(); break;
    }
    return TRUE;
}
VERIFY(0x02271084, &daNpc_Kk1_c::wait_action1);
