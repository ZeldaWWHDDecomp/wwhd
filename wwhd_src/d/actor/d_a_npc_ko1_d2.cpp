/**
 * d_a_npc_ko1_d2.cpp (WWHD)
 * NPC - Joel & Zill (Outset Island), part D2
 *
 * The GameCube decompilation (zeldaret/tww
 * src/d/actor/d_a_npc_ko1.cpp) has only "Nonmatching" stubs for this actor: the functions are
 * written from the WWHD code (cking.rpx) and verified against it, keeping the GameCube names.
 * Part D2: talk_1/talk_2/manzai/neru_*, the action functions (hana_action*, wait_action*) and the
 * compiler-generated tail of the translation unit (HIO constructors, __sinit, destructors).
 */
#define SAFESTRING_VTBL 0x1001C87C /* this TU's sead::SafeString vtable */
#include "d/actor/d_a_npc_ko1.h"

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 025F7DB0 messageSet(mgr, msgNo, cXyz* pos) -> id (GameCube fopMsgM_messageSet; see d_a_kanban_exec.cpp) */
static inline u32 fopMsgM_messageSet(u32 mgr, u32 msgNo, cXyz* pos) { return gabi::call<u32>(0x025F7DB0, mgr, msgNo, pos); }
/* 025F795C: the message manager's current status (the matcher calls it fopMsgM_SearchByID) */
static inline u32 fopMsgM_getStatus(u32 mgr) { return gabi::call<u32>(0x025F795C, mgr); }
/* 028EFFD0 __construct_array(array, n, size, ctor) (GHS runtime) */
static inline void __construct_array(void* p, s32 n, u32 size, u32 ctor) { gabi::call(0x028EFFD0, p, n, size, ctor); }

/* ---- functions of the other parts (called by address) ---- */
enum : u32 {
    ADDR_endEvent = 0x02275738,          /* part B */
    ADDR_lookBack = 0x022758B8,          /* part B */
    ADDR_setAnm_NUM = 0x02276D18,        /* part C */
    ADDR_set_balloonAnm_NUM = 0x02276D04, /* part C (unnamed by the matcher) */
    ADDR_setAnm = 0x02276D84,            /* part C */
    ADDR_searchByID = 0x022770F0,        /* part C (the matcher calls it cLib_calcTimer<s>) */
    ADDR_chk_partsNotMove = 0x0227732C,  /* part C */
    ADDR_chkAttention = 0x02277A30,      /* part C */
    ADDR_check_landOn = 0x02277AB8,      /* part C */
    ADDR_ko_setPthPos = 0x02277B9C,      /* part C */
    ADDR_setPrtcl_HanaPachi = 0x02277FE8, /* part C */
    ADDR_setStt = 0x02278134,            /* part D1 */
    ADDR_wait_1 = 0x0227896C,
    ADDR_wait_2 = 0x02278AE0,
    ADDR_wait_3 = 0x02278B50,
    ADDR_wait_4 = 0x02278B84,
    ADDR_wait_5 = 0x02278CB4,
    ADDR_wait_6 = 0x02278D7C,
    ADDR_wait_7 = 0x02278EC0,
    ADDR_wait_9 = 0x022790E4,
    ADDR_wait_a = 0x022791F4,
    ADDR_walk_1 = 0x02279300,
    ADDR_walk_2 = 0x022793C8,
    ADDR_walk_3 = 0x0227949C,
    ADDR_swim_1 = 0x02279578,
    ADDR_swim_2 = 0x022796B4,
    ADDR_attk_1 = 0x022797FC,
    ADDR_attk_2 = 0x0227992C,
    ADDR_attk_3 = 0x02279A58,
    ADDR_down_1 = 0x02279B90,
};

/* fopNpc_npc_c fields not in the shared layout (HD): +0x7D0 partner process id, +0x7D4 the
 * message status (u16), +0x7D8 the two-NPC conversation state (0: none, 2: talking, 3: end) */
static inline u32 npc_partnerID(fopNpc_npc_c* n) { return gabi::load<u32>(gabi::ea(n) + 0x7D0); }
static inline u16 npc_msgStatus(fopNpc_npc_c* n) { return gabi::load<u16>(gabi::ea(n) + 0x7D4); }
static inline u8 npc_talkState(fopNpc_npc_c* n) { return gabi::load<u8>(gabi::ea(n) + 0x7D8); }
static inline void npc_setTalkState(fopNpc_npc_c* n, u8 v) { gabi::store<u8>(gabi::ea(n) + 0x7D8, v); }

/* the HD vtable (+0xB4) of daNpc_Ko1_c (0x1001CD88): 8-byte entries; +0x1C getMsg, +0x24 anmAtr */
static inline u32 vcall_getMsg(daNpc_Ko1_c* a) {
    u32 vt = gabi::load<u32>(gabi::ea(a) + 0xB4);
    return gabi::call_ptr<u32>(gabi::load<u32>(vt + 0x1C), a);
}
static inline void vcall_anmAtr(daNpc_Ko1_c* a, u16 status) {
    u32 vt = gabi::load<u32>(gabi::ea(a) + 0xB4);
    gabi::call_ptr(gabi::load<u32>(vt + 0x24), a, (u32)status);
}

/* 02279BC0 */
BOOL daNpc_Ko1_c::talk_1() {
    WWHD_FUNC(0x02279BC0, BOOL, this);
    u32 msgMgr = gabi::load<u32>(0x101F4B5C);
    BOOL ret = gabi::call<s32>(ADDR_chk_partsNotMove, this); /* chk_partsNotMove() */
    u16 status = 0;
    if (mbHasMsg) {
        status = (u16)fopMsgM_getStatus(msgMgr);
    }
    gabi::store<u16>(gabi::ea(this) + 0x7D4, status);
    if (npc_talkState(this) == 2 && mCurrMsgBsPcId == fpcM_ERROR_PROCESS_ID_e) {
        /* HD: the first speaker of a two-NPC conversation opens the message itself */
        mCurrMsgNo = vcall_getMsg(this);
        fopAc_ac_c* talkActor = gabi::at<fopAc_ac_c>(gabi::load<u32>(dComIfGp_ea() + 0x5C38));
        u32 id = fopMsgM_messageSet(msgMgr, mCurrMsgNo, &talkActor->eyePos);
        mbHasMsg = 0;
        mCurrMsgBsPcId = id;
        return ret;
    }
    m9B8 = 0xFFFFFFFF;
    talk(1);
    if (mbHasMsg && fopMsgM_getStatus(msgMgr) == 0x13 /* fopMsgStts_MSG_DESTROYED_e */) {
        switch (mCurrMsgNo) {
        case 0xAF1:
            dComIfGs_onEventBit(0x0210);
            break;
        case 0xAFF:
            dComIfGs_onEventBit(0x3801);
            break;
        case 0xB06:
            dComIfGs_onEventBit(0x3340);
            break;
        case 0xB55:
            dComIfGs_onEventBit(0x0220);
            break;
        case 0xB57:
            dComIfGs_onEventBit(0x0208);
            dComIfGs_onEventBit(0x0240);
            break;
        case 0xB62:
            dComIfGs_onEventBit(0x3101);
            break;
        case 0xB64:
            dComIfGs_onEventBit(0x2C04);
            m9DD = 1;
            break;
        }
        m9E5 = 0;
        m9D3 = 0xFF;
        switch (mSpecificType) {
        case 1:
        case 3:
        case 6:
        case 7:
            gabi::call(ADDR_setStt, this, (s32)0x15); /* setStt(0x15) */
            return ret;
        }
        gabi::call(ADDR_setStt, this, (s32)mA14); /* setStt(mA14) */
        m9BE = 60;
        gabi::call(ADDR_endEvent, this); /* endEvent() */
    }
    return ret;
}
VERIFY(0x02279BC0, &daNpc_Ko1_c::talk_1);

/* 02279EF8 */
BOOL daNpc_Ko1_c::talk_2() {
    WWHD_FUNC(0x02279EF8, BOOL, this);
    /* the partners (process ids at 0x924, m92C of them) still talking are told to end */
    u32 nIdle = 0;
    s32 i = 0;
    u32 n;
    while ((s32)i < (s32)(n = m92C)) {
        fopNpc_npc_c* partner = gabi::call<fopNpc_npc_c*>(ADDR_searchByID, this, gabi::load<u32>(gabi::ea(this) + 0x924 + 4 * i));
        if (partner == nullptr) /* JUT_ASSERT(3423, partner != NULL) */
            JUT_ASSERT_fail(STR(0x1001CD5C), 0xD5F, STR(0x1001CD6C));
        if (npc_talkState(partner) != 0) {
            npc_setTalkState(partner, 3);
            i++;
        } else {
            i++;
            nIdle++;
        }
    }
    if (nIdle == n) {
        npc_setTalkState(this, 0);
        gabi::call(ADDR_setStt, this, (s32)mA14); /* setStt(mA14) */
        gabi::call(ADDR_endEvent, this);          /* endEvent() */
    }
    return TRUE;
}
VERIFY(0x02279EF8, &daNpc_Ko1_c::talk_2);

/* 02279FC0 */
BOOL daNpc_Ko1_c::manzai() {
    WWHD_FUNC(0x02279FC0, BOOL, this);
    u32 play = dComIfGp_ea() + 0x5C30;
    switch (npc_talkState(this)) {
    case 2: {
        fopNpc_npc_c* partner = gabi::call<fopNpc_npc_c*>(ADDR_searchByID, this, npc_partnerID(this));
        /* play + 0x5C34: index into the talk actor list (HD) */
        u32 idx = gabi::load<u32>(play + 4);
        if (gabi::ea(this) != gabi::load<u32>(play + idx * 4 + 4)) {
            /* the partner speaks: follow its message status */
            if (mA0C == 0xFF) {
                return TRUE;
            }
            if (mSpecificType == 1 || mSpecificType == 6) {
                mA15 = 1;
                mA0C = 0xFF;
                m_jnt.mbTrn = 1;
                return TRUE;
            }
            s8 prev = mA14;
            mA15 = 3;
            m9C8 = mInitialAngle.y;
            mA13 = prev;
            m_jnt.mbTrn = 1;
            gabi::call(ADDR_setAnm, this); /* setAnm() */
            mA13 = 0x14;
            mA0C = 0xFF;
            return TRUE;
        }
        if (partner != nullptr) {
            m9B8 = partner->mCurrMsgNo;
            vcall_anmAtr(this, npc_msgStatus(partner));
        }
        return TRUE;
    }
    case 3:
        actor_status &= ~0x4000u;
        gabi::call(ADDR_setStt, this, (s32)mA14); /* setStt(mA14) */
        npc_setTalkState(this, 0);
        return TRUE;
    default:
        return TRUE;
    }
}
VERIFY(0x02279FC0, &daNpc_Ko1_c::manzai);

/* 0227A110 */
BOOL daNpc_Ko1_c::neru_1() {
    WWHD_FUNC(0x0227A110, BOOL, this);
    s32 a_anm_num_tbl[2] = {7, 8};
    if (cLib_calcTimer(&m9C2) == 0) {
        u8 n = m9D5 ^ 1;
        m9D5 = n;
        gabi::call(ADDR_setAnm_NUM, this, a_anm_num_tbl[n & 1], (s32)1); /* setAnm_NUM(.., 1) */
        m9C2 = (s16)gabi::ftoi(cM_rndF(180.0f) + 180.0f);
    }
    return TRUE;
}
VERIFY(0x0227A110, &daNpc_Ko1_c::neru_1);

/* 0227A1C8 */
BOOL daNpc_Ko1_c::neru_2() {
    WWHD_FUNC(0x0227A1C8, BOOL, this);
    s32 a_anm_num_tbl[3] = {9, 0xC, 0xA};
    switch (m9D5) {
    case 0:
        if (cLib_calcTimer(&m9C2) == 0) {
            if (cLib_calcTimer(&m9C4) == 0) {
                m9D5 = 1;
                gabi::call(ADDR_setAnm_NUM, this, a_anm_num_tbl[1], (s32)1);
                gabi::call(ADDR_set_balloonAnm_NUM, this, (s32)1);
                m9C4 = (s16)cLib_getRndValue(3, 10);
            }
            m9C2 = (s16)gabi::ftoi(cM_rndF(180.0f) + 180.0f);
        }
        return TRUE;
    case 1:
        if ((s8)m9D0 != 0) {
            m9D5 = 2;
            gabi::call(ADDR_setAnm_NUM, this, a_anm_num_tbl[2], (s32)1);
        } else if (mpMorf->mFrameCtrl.checkPass(2.0f)) {
            gabi::call(ADDR_setPrtcl_HanaPachi, this); /* setPrtcl_HanaPachi() */
            mDoAud_seStart(0x592C, &eyePos, 0, dComIfGp_getReverb(current.roomNo));
        }
        return TRUE;
    case 2:
        if ((s8)m9D0 != 0) {
            m9D5 = 0;
            gabi::call(ADDR_setAnm_NUM, this, a_anm_num_tbl[0], (s32)1);
            gabi::call(ADDR_set_balloonAnm_NUM, this, (s32)0);
        }
        return TRUE;
    default:
        return TRUE;
    }
}
VERIFY(0x0227A1C8, &daNpc_Ko1_c::neru_2);

/* the action functions: mA18 is the phase (0: init, 1..3: run, else (e.g. 9 on exit): nothing);
 * mA13 is the status set by setStt; m9E0 keeps the status function's result */
#define KO1_ACTION_INIT(stt)                                    \
    if (mA18 == 0) {                                            \
        gabi::call(ADDR_setStt, this, (s32)(stt));              \
        mA18 = mA18 + 1;                                        \
        return TRUE;                                            \
    }                                                           \
    if ((u32)(s32)mA18 > 3) {                                   \
        return TRUE;                                            \
    }                                                           \
    m9E4 = (u8)gabi::call<u32>(ADDR_chkAttention, this); /* chkAttention() */

/* 0227A374 */
BOOL daNpc_Ko1_c::hana_action1(void*) {
    WWHD_FUNC(0x0227A374, BOOL, this, (void*)nullptr);
    if (mA18 == 0) {
        gabi::call(ADDR_ko_setPthPos, this); /* ko_setPthPos() */
        gabi::call(ADDR_setStt, this, (s32)4);
        mA18 = mA18 + 1;
        return TRUE;
    }
    if ((u32)(s32)mA18 > 3) {
        return TRUE;
    }
    m9E4 = (u8)gabi::call<u32>(ADDR_chkAttention, this);
    switch ((u32)(s32)mA13) {
    case 3:
        m9E0 = talk_1();
        break;
    case 4:
        m9E0 = gabi::call<u32>(ADDR_walk_1, this);
        break;
    case 5:
        m9E0 = gabi::call<u32>(ADDR_wait_3, this);
        break;
    case 6:
        m9E0 = gabi::call<u32>(ADDR_attk_1, this);
        break;
    case 7:
        m9E0 = gabi::call<u32>(ADDR_swim_1, this);
        break;
    case 8:
        m9E0 = gabi::call<u32>(ADDR_swim_2, this);
        break;
    case 9:
        m9E0 = gabi::call<u32>(ADDR_down_1, this, (s32)10);
        break;
    case 10:
        m9E0 = gabi::call<u32>(ADDR_wait_4, this);
        break;
    case 11:
        m9E0 = gabi::call<u32>(ADDR_walk_3, this);
        break;
    }
    gabi::call(ADDR_lookBack, this); /* lookBack() */
    return TRUE;
}
VERIFY(0x0227A374, &daNpc_Ko1_c::hana_action1);

/* 0227A4DC */
BOOL daNpc_Ko1_c::hana_action2(void*) {
    WWHD_FUNC(0x0227A4DC, BOOL, this, (void*)nullptr);
    KO1_ACTION_INIT(0x13)
    switch ((u32)(s32)mA13) {
    case 3:
        m9E0 = talk_1();
        break;
    case 0x11:
        m9E0 = gabi::call<u32>(ADDR_attk_3, this);
        break;
    case 0x12:
        m9E0 = gabi::call<u32>(ADDR_down_1, this, (s32)0x13);
        break;
    case 0x13:
        m9E0 = gabi::call<u32>(ADDR_wait_7, this);
        break;
    case 0x14:
        m9E0 = manzai();
        break;
    case 0x15:
        m9E0 = talk_2();
        break;
    }
    gabi::call(ADDR_lookBack, this);
    return TRUE;
}
VERIFY(0x0227A4DC, &daNpc_Ko1_c::hana_action2);

/* 0227A60C */
BOOL daNpc_Ko1_c::hana_action3(void*) {
    WWHD_FUNC(0x0227A60C, BOOL, this, (void*)nullptr);
    KO1_ACTION_INIT(0x16)
    switch ((u32)(s32)mA13) {
    case 3:
        m9E0 = talk_1();
        break;
    case 0x16:
        m9E0 = gabi::call<u32>(ADDR_wait_5, this, (s32)0x17);
        break;
    case 0x17:
        m9E0 = gabi::call<u32>(ADDR_attk_2, this, (s32)0x19, (s32)0x18);
        break;
    case 0x18:
        m9E0 = gabi::call<u32>(ADDR_walk_2, this, (s32)0x16, (s32)0x17);
        break;
    case 0x19:
        m9E0 = gabi::call<u32>(ADDR_down_1, this, (s32)0x1A);
        break;
    case 0x1A:
        m9E0 = gabi::call<u32>(ADDR_wait_9, this);
        break;
    }
    gabi::call(ADDR_lookBack, this);
    return TRUE;
}
VERIFY(0x0227A60C, &daNpc_Ko1_c::hana_action3);

/* 0227A750 */
BOOL daNpc_Ko1_c::hana_action4(void*) {
    WWHD_FUNC(0x0227A750, BOOL, this, (void*)nullptr);
    KO1_ACTION_INIT(0x1D)
    switch ((u32)(s32)mA13) {
    case 3:
        m9E0 = talk_1();
        break;
    case 0x14:
        m9E0 = manzai();
        break;
    case 0x15:
        m9E0 = talk_2();
        break;
    case 0x1D:
        m9E0 = gabi::call<u32>(ADDR_wait_a, this);
        break;
    }
    gabi::call(ADDR_lookBack, this);
    return TRUE;
}
VERIFY(0x0227A750, &daNpc_Ko1_c::hana_action4);

/* 0227A830 */
BOOL daNpc_Ko1_c::hana_action5(void*) {
    WWHD_FUNC(0x0227A830, BOOL, this, (void*)nullptr);
    KO1_ACTION_INIT(0x1C)
    if (mA13 == 0x1C) {
        m9E0 = neru_2();
    }
    gabi::call(ADDR_lookBack, this);
    return TRUE;
}
VERIFY(0x0227A830, &daNpc_Ko1_c::hana_action5);

/* 0227A8C8 */
BOOL daNpc_Ko1_c::wait_action1(void*) {
    WWHD_FUNC(0x0227A8C8, BOOL, this, (void*)nullptr);
    if (mA18 == 0) {
        gabi::call(ADDR_setStt, this, (s32)1);
        mA18 = mA18 + 1;
        return TRUE;
    }
    if ((u32)(s32)mA18 > 3) {
        return TRUE;
    }
    if (!dComIfGs_isEventBit(0x0104)) {
        gabi::call(ADDR_check_landOn, this); /* check_landOn() */
    }
    m9E4 = (u8)gabi::call<u32>(ADDR_chkAttention, this);
    switch ((u32)(s32)mA13) {
    case 1:
        m9E0 = gabi::call<u32>(ADDR_wait_1, this);
        break;
    case 2:
        m9E0 = gabi::call<u32>(ADDR_wait_2, this);
        break;
    case 3:
        m9E0 = talk_1();
        break;
    }
    gabi::call(ADDR_lookBack, this);
    return TRUE;
}
VERIFY(0x0227A8C8, &daNpc_Ko1_c::wait_action1);

/* 0227A9C0 */
BOOL daNpc_Ko1_c::wait_action2(void*) {
    WWHD_FUNC(0x0227A9C0, BOOL, this, (void*)nullptr);
    KO1_ACTION_INIT(0xC)
    switch ((u32)(s32)mA13) {
    case 3:
        m9E0 = talk_1();
        break;
    case 0xC:
        m9E0 = gabi::call<u32>(ADDR_wait_5, this, (s32)0xD);
        break;
    case 0xD:
        m9E0 = gabi::call<u32>(ADDR_attk_2, this, (s32)0x10, (s32)0xF);
        break;
    case 0xE:
        m9E0 = gabi::call<u32>(ADDR_wait_6, this);
        break;
    case 0xF:
        m9E0 = gabi::call<u32>(ADDR_walk_2, this, (s32)0xC, (s32)0xD);
        break;
    case 0x10:
        m9E0 = gabi::call<u32>(ADDR_down_1, this, (s32)0xE);
        break;
    case 0x14:
        m9E0 = manzai();
        break;
    case 0x15:
        m9E0 = talk_2();
        break;
    }
    gabi::call(ADDR_lookBack, this);
    return TRUE;
}
VERIFY(0x0227A9C0, &daNpc_Ko1_c::wait_action2);

/* 0227AB28 */
BOOL daNpc_Ko1_c::wait_action3(void*) {
    WWHD_FUNC(0x0227AB28, BOOL, this, (void*)nullptr);
    KO1_ACTION_INIT(0x1D)
    switch ((u32)(s32)mA13) {
    case 3:
        m9E0 = talk_1();
        break;
    case 0x14:
        m9E0 = manzai();
        break;
    case 0x15:
        m9E0 = talk_2();
        break;
    case 0x1D:
        m9E0 = gabi::call<u32>(ADDR_wait_a, this);
        break;
    }
    gabi::call(ADDR_lookBack, this);
    return TRUE;
}
VERIFY(0x0227AB28, &daNpc_Ko1_c::wait_action3);

/* 0227AC08 */
BOOL daNpc_Ko1_c::wait_action4(void*) {
    WWHD_FUNC(0x0227AC08, BOOL, this, (void*)nullptr);
    KO1_ACTION_INIT(0x1B)
    if (mA13 == 0x1B) {
        m9E0 = neru_1();
    }
    gabi::call(ADDR_lookBack, this);
    return TRUE;
}
VERIFY(0x0227AC08, &daNpc_Ko1_c::wait_action4);

/* ---- compiler-generated tail of the translation unit ---- */

/* daNpc_Ko1_childHIO_c (HD 0x60: vtable, parameter block copied from .data, index at +0x5C) */
struct daNpc_Ko1_childHIO_c {
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ u8 mPrm[0x58];
    /* 0x5C */ be<s32> mNo;
};
WWHD_SIZE(daNpc_Ko1_childHIO_c, 0x60);
/* daNpc_Ko1_HIO_c (HD 0xCC) */
struct daNpc_Ko1_HIO_c {
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ be<s8> mNo;
    /* 0x05 */ u8 _05[3];
    /* 0x08 */ be<s32> m08;
    /* 0x0C */ daNpc_Ko1_childHIO_c mChild[2];
};
WWHD_SIZE(daNpc_Ko1_HIO_c, 0xCC);

/* 0227ACA0 daNpc_Ko1_childHIO_c::daNpc_Ko1_childHIO_c (HD: allocates when this == NULL) */
static daNpc_Ko1_childHIO_c* daNpc_Ko1_childHIO_c_ct(daNpc_Ko1_childHIO_c* i_this) {
    WWHD_FUNC(0x0227ACA0, daNpc_Ko1_childHIO_c*, i_this);
    if (i_this == nullptr) {
        i_this = (daNpc_Ko1_childHIO_c*)operator_new(0x60);
        if (i_this == nullptr)
            return i_this;
    }
    i_this->__vtbl = 0x1001C954;
    return i_this;
}
VERIFY(0x0227ACA0, daNpc_Ko1_childHIO_c_ct);

/* 0227ACE0 daNpc_Ko1_HIO_c::daNpc_Ko1_HIO_c (HD: allocates when this == NULL) */
static daNpc_Ko1_HIO_c* daNpc_Ko1_HIO_c_ct(daNpc_Ko1_HIO_c* i_this) {
    WWHD_FUNC(0x0227ACE0, daNpc_Ko1_HIO_c*, i_this);
    if (i_this == nullptr) {
        i_this = (daNpc_Ko1_HIO_c*)operator_new(0xCC);
        if (i_this == nullptr)
            return i_this;
    }
    i_this->__vtbl = 0x1001C964;
    __construct_array(i_this->mChild, 2, 0x60, 0x0227ACA0 /* daNpc_Ko1_childHIO_c_ct */);
    for (int i = 0; i < 2; i++) {
        i_this->mChild[i].mNo = i;
        memcpy_g(i_this->mChild[i].mPrm, gabi::at<u8>(0x101BFC74 + 0x58 * i), 0x58); /* a_prm_tbl[i]; 028FEAC0 memcpy */
    }
    i_this->m08 = -1;
    i_this->mNo = -1;
    return i_this;
}
VERIFY(0x0227ACE0, daNpc_Ko1_HIO_c_ct);

/* 0227AD90: static initialisation of the translation unit */
static void __sinit_d_a_npc_ko1_cpp() {
    WWHD_FUNC(0x0227AD90, void);
    sinit_header_statics(0x104677B4, 0x101BFD24);
    daNpc_Ko1_HIO_c_ct(gabi::at<daNpc_Ko1_HIO_c>(0x104677DC)); /* static daNpc_Ko1_HIO_c l_HIO */
}
VERIFY(0x0227AD90, __sinit_d_a_npc_ko1_cpp);

/* 0227AE30: sead::SafeString deleting destructor (this TU's copy; vtable 0x1001C87C slot +0xC) */
static void SafeString_dt(SafeString* i_this, s32 flags) {
    WWHD_FUNC(0x0227AE30, void, i_this, flags);
    if (i_this != nullptr && (flags & 1))
        operator_delete(i_this);
}
VERIFY(0x0227AE30, SafeString_dt);

/* 0227AE44: daNpc_Ko1_c deleting destructor (compiler-generated, HD virtual destructor; vtable
 * 0x1001CD88 slot +0xC): only the fopNpc_npc_c members have destructors */
static void daNpc_Ko1_c_dt(daNpc_Ko1_c* i_this, s32 flags) {
    WWHD_FUNC(0x0227AE44, void, i_this, flags);
    if (i_this != nullptr) {
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        gabi::call(0x02018034, gabi::ea(&i_this->mAcchCir) + 0x14, 2); /* cM3dGCir::~cM3dGCir (mAcchCir.m_cir) */
        gabi::store<u32>(gabi::ea(&i_this->mObjAcch) + 0x20, 0x1001C8F4);
        gabi::store<u32>(gabi::ea(&i_this->mObjAcch) + 0x14, 0x1001C904);
        gabi::call(0x024EFD9C, &i_this->mObjAcch, 0); /* dBgS_Acch::~dBgS_Acch */
        gabi::call(0x025D50BC, i_this, 0);             /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x0227AE44, daNpc_Ko1_c_dt);

/* 0227AEE0: sead::SafeString::assureTerminationImpl_ (this TU's copy; empty) */
static void SafeString_assureTerminationImpl(SafeString*) {
    WWHD_FUNC(0x0227AEE0, void, (SafeString*)nullptr);
}
VERIFY(0x0227AEE0, SafeString_assureTerminationImpl);
