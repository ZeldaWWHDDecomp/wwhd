/* d_event: event control (dEvt_control_c, dEvt_info_c), WWHD.
 *
 * Translation unit 0253E948..02540508, from the image: d_ev_camera ends with its __sinit
 * (0253E89C) and the inline copies of its sead::SafeString vtable 1004CDBC (0253E930,
 * 0253E944); d_event starts with the HD dEvt_info_c constructor (0253E948), follows the
 * GameCube order (dEvt_info_c and the constructor first) and ends with its __sinit (02540474);
 * d_event_data starts at 02540508. Ported from the GameCube d_event.cpp; HD-only functions are
 * written from the WWHD code.
 *
 * dEvt_control_c (HD 0xF4, layout unchanged; play + 0x51D0). dEvt_order_c 0x18 (mEventType 0,
 * mFlag 2, mHindFlag 4, mActor1 8, mActor2 0xC, mEventId 0x10, mPriority 0x12, mNextOrderIdx
 * 0x14, mEventInfoIdx 0x15). dEvt_info_c (HD 0x18: GameCube members + vtable 1004D764 at
 * 0x14; at actor + 0xF8: command 0xF8, condition 0xFA, event id 0xFC, map tool id 0xFE, XY event
 * callback 0x100, XY check callback 0x104, photo event callback 0x108).
 * HD event types: a fourth item button (type 9, talk button 4) is inserted after SHOWITEM_Z, so
 * CATCH is 0xA, TREASURE 0xB, PHOTO 0xC and CHANGE 0xD. */
#include "bindings.h"

namespace d_event_cpp {

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline void JUT_ASSERT_l(u32 file, s32 line, u32 msg) { gabi::call(0x0273AA24, file, line, msg); }
static inline u32 operator_new_l(u32 size) { return gabi::call<u32>(0x0273AD10, size); }
static inline void memzero_l(u32 p, u32 n) { gabi::call(0x028F521C, p, n); }
/* dEvent_manager_c (play + 0x52C4) */
static inline u32 evmng() { return gabi::ea(dComIfGp_get()) + 0x52C4; }
static inline u32 evmng_getEventIdx_l(u32 m, u32 name, u32 evNo) { return gabi::call<u32>(0x02543F10, m, name, evNo); }
static inline u32 evmng_getEventData_l(u32 m, u32 idx) { return gabi::call<u32>(0x02544044, m, idx); }
static inline s32 evmng_getMyStaffId_l(u32 m, u32 name, u32 ac, s32 tag) { return gabi::call<s32>(0x02542D88, m, name, ac, tag); }
static inline void evmng_cutEnd_l(u32 m, s32 staff) { gabi::call(0x02543280, m, staff); }
static inline void evmng_endProc_l(u32 m, u32 id, s32 x) { gabi::call(0x0254465C, m, id, x); }
static inline void evmng_cancelStaff_l(u32 m, u32 name) { gabi::call(0x02544944, m, name); }
static inline void evmng_issueStaff_l(u32 m, u32 name) { gabi::call(0x02544908, m, name); }
static inline BOOL evmng_order_l(u32 m, u32 id) { return gabi::call<BOOL>(0x02544534, m, id); }
static inline u32 evmng_getEventEndSound_l(u32 m, u32 id) { return gabi::call<u32>(0x02544628, m, id); }
static inline u32 fopAcM_getItemEventPartner_l(u32 ac) { return gabi::call<u32>(0x025D7C98, ac); }
static inline void daItemBase_c_dead_l(u32 ac) { gabi::call(0x0218432C, ac); }
static inline void dVibration_StopQuake_l(u32 v, s32 x) { gabi::call(0x025CB610, v, x); }
static inline void mDoAud_seStart_l(u32 id) { gabi::call(0x025E1988, id); }
static inline u32 dComIfGs_getPictureInfo_l(u32 p) { return gabi::call<u32>(0x02720144, p); }
/* dStage_stageDt_c (play + 0x5150) virtual getEventInfo (slot 0x1CC) */
static inline u32 stage_getEventInfo() {
    u32 s = gabi::ea(dComIfGp_get()) + 0x5150;
    return gabi::call_ptr<u32>(gabi::load<u32>(gabi::load<u32>(s) + 0x1CC), s);
}

static inline u32 ld(u32 a) { return gabi::load<u32>(a); }
static inline u16 ld16(u32 a) { return gabi::load<u16>(a); }
static inline s16 lds16(u32 a) { return gabi::load<s16>(a); }
static inline u8 ld8(u32 a) { return gabi::load<u8>(a); }
static inline s8 lds8(u32 a) { return gabi::load<s8>(a); }
static inline void st(u32 a, u32 v) { gabi::store<u32>(a, v); }
static inline void st16(u32 a, u16 v) { gabi::store<u16>(a, v); }
static inline void st8(u32 a, u8 v) { gabi::store<u8>(a, v); }
static inline void stf(u32 a, f32 v) { gabi::store<f32>(a, v); }
static inline u32 play() { return gabi::ea(dComIfGp_get()); }

/* dEvt_control_c */
enum : u32 { E_COUNT = 0xC0, E_FIRST = 0xC1, E_MODE = 0xC2, E_ENDPROC = 0xC3, E_PT1 = 0xC4, E_PT2 = 0xC8, E_PTTALK = 0xCC,
             E_PTITEM = 0xD0, E_GETITEM = 0xD4, E_HIND = 0xD6, E_EVENTID = 0xD8, E_ENDSOUND = 0xDA, E_DB = 0xDB,
             E_DC = 0xDC, E_DEBUGSTB = 0xDD, E_DE = 0xDE, E_INFOIDX = 0xDF, E_TALKBTN = 0xE0, E_ITEMNO = 0xE1,
             E_INPHOTO = 0xE2, E_CULL = 0xE4, E_FLAG = 0xE8, E_MSTICK = 0xEA, E_CSTICK = 0xEF };
/* dEvt_order_c */
enum : u32 { O_TYPE = 0, O_FLAG = 2, O_HIND = 4, O_ACTOR1 = 8, O_ACTOR2 = 0xC, O_EVENTID = 0x10, O_PRIO = 0x12,
             O_NEXT = 0x14, O_INFOIDX = 0x15 };
/* actor: eventInfo */
enum : u32 { AC_CMD = 0xF8, AC_COND = 0xFA, AC_EVENTID = 0xFC, AC_XYEVENTCB = 0x100, AC_PHOTOCB = 0x108 };

static constexpr u32 FILE_ = 0x1004D788;

/* 0253E948: HD dEvt_info_c::dEvt_info_c() */
static u32 dEvt_info_c_ct(u32 i_this) {
    WWHD_FUNC(0x0253E948, u32, i_this);
    if (i_this == 0) {
        i_this = operator_new_l(0x18);
        if (i_this == 0) return 0;
    }
    st(i_this + 0x10, 0);
    st16(i_this + 4, 0xFFFF);
    st8(i_this + 6, 0xFF);
    st16(i_this + 2, 0);
    st(i_this + 0x14, 0x1004D764);
    st(i_this + 8, 0);
    st(i_this + 0xC, 0);
    st16(i_this + 0, 0);
    return i_this;
}
VERIFY(0x0253E948, dEvt_info_c_ct);

/* 0253E9B0 */
static void dEvt_info_c_setEventName(u32 i_this, u32 evtName) {
    WWHD_FUNC(0x0253E9B0, void, i_this, evtName);
    if (evtName == 0) st16(i_this + 4, 0xFFFF);
    else st16(i_this + 4, (u16)evmng_getEventIdx_l(evmng(), evtName, 0xFF));
}
VERIFY(0x0253E9B0, dEvt_info_c_setEventName);

/* 0253EA0C: HD returns the event data (its name is at offset 0) */
static u32 dEvt_info_c_getEventName(u32 i_this) {
    WWHD_FUNC(0x0253EA0C, u32, i_this);
    if (lds16(i_this + 4) == -1) return 0;
    u32 m = evmng();
    return evmng_getEventData_l(m, (u32)(s32)lds16(i_this + 4));
}
VERIFY(0x0253EA0C, dEvt_info_c_getEventName);

/* 0253EA70 */
static void dEvt_control_c_remove(u32 i_this) {
    WWHD_FUNC(0x0253EA70, void, i_this);
    st8(i_this + E_TALKBTN, 0);
    stf(i_this + E_CULL, 0.0f);
    st8(i_this + E_DB, 0);
    st8(i_this + E_DE, 0xFF);
    st8(i_this + E_ENDPROC, 0);
    st8(i_this + E_INFOIDX, 0xFF);
    st8(i_this + E_MODE, 0);
    st8(i_this + E_ITEMNO, 0xFF);
    st8(i_this + E_DEBUGSTB, 0);
    st8(i_this + E_COUNT, 0);
    st8(i_this + E_ENDSOUND, 0);
    st8(i_this + E_INPHOTO, 0);
    st16(i_this + E_FLAG, 0);
}
VERIFY(0x0253EA70, dEvt_control_c_remove);

/* 0253EAB8: HD value-initialises the members first */
static u32 dEvt_control_c_ct(u32 i_this) {
    WWHD_FUNC(0x0253EAB8, u32, i_this);
    if (i_this == 0) {
        i_this = operator_new_l(0xF4);
        if (i_this == 0) return 0;
    }
    st8(i_this + E_DC, 0);
    stf(i_this + E_CULL, 0.0f);
    st8(i_this + E_ENDSOUND, 0);
    st8(i_this + E_ITEMNO, 0);
    st8(i_this + E_ENDPROC, 0);
    st8(i_this + E_DB, 0);
    st(i_this + E_PTITEM, 0);
    st16(i_this + E_EVENTID, 0);
    st8(i_this + E_MODE, 0);
    st8(i_this + E_COUNT, 0);
    st8(i_this + E_TALKBTN, 0);
    st(i_this + E_PT1, 0);
    st8(i_this + E_INFOIDX, 0);
    st8(i_this + E_DEBUGSTB, 0);
    st(i_this + E_PTTALK, 0);
    st16(i_this + E_HIND, 0);
    st16(i_this + E_FLAG, 0);
    st8(i_this + E_INPHOTO, 0);
    st8(i_this + E_GETITEM, 0);
    st(i_this + E_PT2, 0);
    st8(i_this + E_FIRST, 0);
    st8(i_this + E_DE, 0);
    memzero_l(i_this + E_MSTICK, 0xA);
    dEvt_control_c_remove(i_this);
    return i_this;
}
VERIFY(0x0253EAB8, dEvt_control_c_ct);

/* 0253EB70 */
static s32 dEvt_control_c_giveItemCut(u32 i_this, u32 item) {
    WWHD_FUNC(0x0253EB70, s32, i_this, item);
    s32 staffIdx = evmng_getMyStaffId_l(evmng(), 0x1004D778 /* "GIVEMAN" */, 0, 0);
    if (staffIdx == -1) return 0;
    evmng_cutEnd_l(evmng(), staffIdx);
    st8(i_this + E_GETITEM, (u8)item);
    return 1;
}
VERIFY(0x0253EB70, dEvt_control_c_giveItemCut);

/* 0253EC0C: HD: orders other than priority 1 are refused while play+0x5BAC is 0, and nothing is
 * queued behind a priority-1 order at the head (the slot is written, the count not raised); the
 * priority-0 assert is gone */
static s32 dEvt_control_c_order(u32 i_this, u32 eventType, u32 priority, u32 flag, u32 hindFlag, u32 ac1, u32 ac2,
                                u32 eventIdx) {
    WWHD_FUNC(0x0253EC0C, s32, i_this, eventType, priority, flag, hindFlag, ac1, ac2, eventIdx);
    u8 info = ld8(gabi::cpu->r[1] + 8 + 3); /* 9th argument (u8 eventInfoIdx), on the stack */
    if (priority != 1) {
        if (ld16(play() + 0x5BAC) == 0) return 0;
    }
    s32 cnt = lds8(i_this + E_COUNT);
    if (cnt >= 8) return 0;
    u32 o = i_this + cnt * 0x18;
    st16(o + O_TYPE, (u16)eventType);
    st16(o + O_PRIO, (u16)priority);
    st16(o + O_FLAG, (u16)flag);
    st(o + O_ACTOR1, ac1);
    st(o + O_ACTOR2, ac2);
    st16(o + O_EVENTID, (u16)eventIdx);
    st16(o + O_HIND, (u16)hindFlag);
    st8(o + O_INFOIDX, info);
    s32 c = lds8(i_this + E_COUNT);
    if (c == 0) {
        st8(i_this + E_FIRST, 0);
        st8(o + O_NEXT, 0xFF);
    } else {
        s32 qi = lds8(i_this + E_FIRST);
        u32 q = i_this + qi * 0x18;
        u16 qp = ld16(q + O_PRIO);
        if (qp == 1) return 0;
        u16 np = ld16(o + O_PRIO);
        if (np < qp) {
            st8(i_this + E_FIRST, (u8)c);
            st8(o + O_NEXT, (u8)qi);
        } else {
            s32 nx;
            while ((nx = lds8(q + O_NEXT)) >= 0) {
                if (np < ld16(i_this + nx * 0x18 + O_PRIO)) break;
                q = i_this + nx * 0x18;
            }
            st8(o + O_NEXT, (u8)nx);
            st8(q + O_NEXT, ld8(i_this + E_COUNT));
        }
    }
    st8(i_this + E_COUNT, (u8)(ld8(i_this + E_COUNT) + 1));
    return 1;
}
VERIFY(0x0253EC0C, dEvt_control_c_order);

/* 0253ED80 */
static s32 dEvt_control_c_orderOld(u32 i_this, u32 eventType, u32 priority, u32 flag, u32 hindFlag, u32 ac1, u32 ac2,
                                   u32 eventName) {
    WWHD_FUNC(0x0253ED80, s32, i_this, eventType, priority, flag, hindFlag, ac1, ac2, eventName);
    u32 idx = evmng_getEventIdx_l(evmng(), eventName, 0xFF);
    return gabi::call<s32>(0x0253EC0C, i_this, eventType, priority, flag, hindFlag, ac1, ac2, idx, 0xFFu); /* order */
}
VERIFY(0x0253ED80, dEvt_control_c_orderOld);

/* 0253EE04 */
static u32 dEvt_control_c_convPId(u32 i_this, u32 pid) {
    WWHD_FUNC(0x0253EE04, u32, i_this, pid);
    gabi::Local<be<u32>> key;
    *key = pid;
    if (pid == 0xFFFFFFFFu) return 0;
    return gabi::call<u32>(0x025D5218, 0x025E1234u, key.get());
}
VERIFY(0x0253EE04, dEvt_control_c_convPId);

static inline void end_partners(u32 i_this) {
    u32 a1 = dEvt_control_c_convPId(i_this, ld(i_this + E_PT1));
    if (a1 != 0) st16(a1 + AC_CMD, 0);
    u32 a2 = dEvt_control_c_convPId(i_this, ld(i_this + E_PT2));
    if (a2 != 0) st16(a2 + AC_CMD, 0);
    s16 id = lds16(i_this + E_EVENTID);
    if (id != -1) {
        evmng_endProc_l(evmng(), (u32)(s32)id, 1);
        st16(i_this + E_EVENTID, 0xFFFF);
    }
}

/* 0253EE40 */
static BOOL dEvt_control_c_talkEnd(u32 i_this) {
    WWHD_FUNC(0x0253EE40, BOOL, i_this);
    end_partners(i_this);
    u32 p = fopAcM_getItemEventPartner_l(0);
    if (p != 0 && (lds16(p + 8) == 0xFF || lds16(p + 8) == 0x101)) daItemBase_c_dead_l(p);
    return TRUE;
}
VERIFY(0x0253EE40, dEvt_control_c_talkEnd);

/* 0253EF08 */
static BOOL dEvt_control_c_demoEnd(u32 i_this) {
    WWHD_FUNC(0x0253EF08, BOOL, i_this);
    end_partners(i_this);
    evmng_cancelStaff_l(evmng(), 0x1004D780 /* "ALL" */);
    return TRUE;
}
VERIFY(0x0253EF08, dEvt_control_c_demoEnd);

/* 0253EFA8 */
static BOOL dEvt_control_c_endProc(u32 i_this) {
    WWHD_FUNC(0x0253EFA8, BOOL, i_this);
    switch (ld8(i_this + E_MODE)) {
    case 1: dEvt_control_c_talkEnd(i_this); break;
    case 2: dEvt_control_c_demoEnd(i_this); break;
    case 3: break;
    default: JUT_ASSERT_l(FILE_, 0x34C, 0x1004D784); break;
    }
    st8(i_this + E_MODE, 0);
    st8(i_this + E_DE, 0xFF);
    st8(i_this + E_INFOIDX, 0xFF);
    st8(i_this + E_TALKBTN, 0);
    st8(i_this + E_INPHOTO, 0);
    st8(i_this + E_ITEMNO, 0xFF);
    st16(i_this + E_FLAG, 0);
    return TRUE;
}
VERIFY(0x0253EFA8, dEvt_control_c_endProc);

/* 0253F0B0 */
static BOOL dEvt_control_c_checkChange(u32 i_this) {
    WWHD_FUNC(0x0253F0B0, BOOL, i_this);
    if (lds8(i_this + E_COUNT) == 0) return FALSE;
    s32 next = lds8(i_this + E_FIRST);
    do {
        u32 o = i_this + next * 0x18;
        next = lds8(o + O_NEXT);
        if (ld16(o + O_TYPE) == 0xD) return TRUE;
    } while (next >= 0);
    return FALSE;
}
VERIFY(0x0253F0B0, dEvt_control_c_checkChange);

/* 0253F0F8 */
static BOOL dEvt_control_c_beforeFlagProc(u32 i_this, u32 order) {
    WWHD_FUNC(0x0253F0F8, BOOL, i_this, order);
    u16 f = ld16(order + O_FLAG);
    u32 a2 = ld(order + O_ACTOR2);
    if ((f & 4) && !(ld16(a2 + AC_COND) & 1)) return FALSE;
    return TRUE;
}
VERIFY(0x0253F0F8, dEvt_control_c_beforeFlagProc);

/* 0253F124 */
static u32 dEvt_control_c_getPId(u32 i_this, u32 ac) {
    WWHD_FUNC(0x0253F124, u32, i_this, ac);
    if (ac == 0) return 0xFFFFFFFF;
    return ld(ac + 4);
}
VERIFY(0x0253F124, dEvt_control_c_getPId);

/* 0253F138 */
static void dEvt_control_c_setParam(u32 i_this, u32 order) {
    WWHD_FUNC(0x0253F138, void, i_this, order);
    u32 info = stage_getEventInfo();
    st(i_this + E_PT1, dEvt_control_c_getPId(i_this, ld(order + O_ACTOR1)));
    st(i_this + E_PT2, dEvt_control_c_getPId(i_this, ld(order + O_ACTOR2)));
    st16(i_this + E_EVENTID, (u16)lds16(order + O_EVENTID));
    st16(i_this + E_HIND, ld16(order + O_HIND));
    u32 p = play();
    u32 a1 = ld(order + O_ACTOR1);
    u32 who = (ld(p + 0x5B2C) != a1) ? O_ACTOR1 : O_ACTOR2;
    st(i_this + E_PTTALK, dEvt_control_c_getPId(i_this, ld(order + who)));
    st(i_this + E_PTITEM, dEvt_control_c_getPId(i_this, ld(order + who)));
    st8(i_this + E_INFOIDX, ld8(order + O_INFOIDX));
    u8 idx = ld8(order + O_INFOIDX);
    if (idx == 0xFF || info == 0 || !((s32)idx < (s32)ld(info + 0))) {
        st8(i_this + E_DE, 0xFF);
        st16(i_this + E_FLAG, 0);
        stf(i_this + E_CULL, 1.0f);
    } else {
        u8 v = ld8(ld(info + 4) + idx * 0x18 + 0x10);
        st16(i_this + E_FLAG, 0);
        st8(i_this + E_DE, v);
        stf(i_this + E_CULL, 1.0f);
    }
}
VERIFY(0x0253F138, dEvt_control_c_setParam);

/* 0253F290 */
static void dEvt_control_c_afterFlagProc(u32 i_this, u32 order) {
    WWHD_FUNC(0x0253F290, void, i_this, order);
    u16 f = ld16(order + O_FLAG);
    if (f & 1) {
        st(i_this + E_PT2, dEvt_control_c_getPId(i_this, 0));
        f = ld16(order + O_FLAG);
    }
    if (f & 2) {
        evmng_issueStaff_l(evmng(), 0x1004D798 /* "ALL" */);
        f = ld16(order + O_FLAG);
    }
    if (f & 8) st16(i_this + E_FLAG, ld16(i_this + E_FLAG) | 0x20);
}
VERIFY(0x0253F290, dEvt_control_c_afterFlagProc);

/* 0253F318 */
static BOOL dEvt_control_c_demoCheck(u32 i_this, u32 order) {
    WWHD_FUNC(0x0253F318, BOOL, i_this, order);
    u32 a2 = ld(order + O_ACTOR2);
    u32 a1 = ld(order + O_ACTOR1);
    s16 eventId = lds16(order + O_EVENTID);
    if (a2 == 0) {
        JUT_ASSERT_l(0x1004D7A0, 0x256, 0x1004D79C);
        return FALSE;
    }
    if (!dEvt_control_c_beforeFlagProc(i_this, order)) return FALSE;
    if (!evmng_order_l(evmng(), (u32)(s32)eventId)) return FALSE;
    if (a1 != 0) st16(a1 + AC_CMD, 2);
    st16(a2 + AC_CMD, 2);
    st8(i_this + E_MODE, 2);
    dEvt_control_c_setParam(i_this, order);
    dEvt_control_c_afterFlagProc(i_this, order);
    u32 snd = evmng_getEventEndSound_l(evmng(), (u32)(s32)eventId);
    if (snd != 0 && ld8(i_this + E_ENDSOUND) == 0) {
        st8(i_this + E_ENDSOUND, (u8)snd);
        mDoAud_seStart_l(0x806);
    }
    return TRUE;
}
VERIFY(0x0253F318, dEvt_control_c_demoCheck);

/* 0253F434 */
static BOOL dEvt_control_c_changeProc(u32 i_this) {
    WWHD_FUNC(0x0253F434, BOOL, i_this);
    if (lds8(i_this + E_COUNT) == 0) return FALSE;
    s32 next = lds8(i_this + E_FIRST);
    st8(i_this + E_COUNT, 0);
    do {
        u32 o = i_this + next * 0x18;
        u16 t = ld16(o + O_TYPE);
        next = lds8(o + O_NEXT);
        if (t == 0xD && dEvt_control_c_demoCheck(i_this, o)) return TRUE;
    } while (next >= 0);
    return FALSE;
}
VERIFY(0x0253F434, dEvt_control_c_changeProc);

/* 0253F4D4 */
static BOOL dEvt_control_c_commonCheck(u32 i_this, u32 order, u32 cond, u32 cmd) {
    WWHD_FUNC(0x0253F4D4, BOOL, i_this, order, cond, cmd);
    u32 a1 = ld(order + O_ACTOR1);
    u32 a2 = ld(order + O_ACTOR2);
    if (a1 != 0 && (ld16(a1 + AC_COND) & cond) == cond && a2 != 0 && (ld16(a2 + AC_COND) & cond) == cond) {
        st16(a1 + AC_CMD, (u16)cmd);
        st16(a2 + AC_CMD, (u16)cmd);
        dEvt_control_c_setParam(i_this, order);
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x0253F4D4, dEvt_control_c_commonCheck);

/* 0253F54C */
static BOOL dEvt_control_c_talkCheck(u32 i_this, u32 order) {
    WWHD_FUNC(0x0253F54C, BOOL, i_this, order);
    u32 a2 = ld(order + O_ACTOR2);
    if (!dEvt_control_c_commonCheck(i_this, order, 1, 1)) return FALSE;
    u32 id = (u32)(s32)lds16(i_this + E_EVENTID);
    st8(i_this + E_MODE, 1);
    if ((s32)id == -1) {
        if (a2 != 0 && dEvt_info_c_getEventName(a2 + AC_CMD) != 0) {
            id = (u32)(s32)lds16(a2 + AC_EVENTID);
        } else {
            id = evmng_getEventIdx_l(evmng(), 0x1004D7BC /* "DEFAULT_TALK" */, 0xFF);
        }
        st16(i_this + E_EVENTID, (u16)id);
    }
    if (!evmng_order_l(evmng(), id)) JUT_ASSERT_l(0x1004D7B0, 0x170, 0x1004D7AC);
    return TRUE;
}
VERIFY(0x0253F54C, dEvt_control_c_talkCheck);

/* 0253F648: HD TREASURE is type 0xB */
static BOOL dEvt_control_c_doorCheck(u32 i_this, u32 order) {
    WWHD_FUNC(0x0253F648, BOOL, i_this, order);
    if (!dEvt_control_c_commonCheck(i_this, order, 4, 3)) return FALSE;
    st8(i_this + E_MODE, 2);
    u32 a2 = dEvt_control_c_convPId(i_this, ld(i_this + E_PT2));
    s32 id = lds16(i_this + E_EVENTID);
    bool ok = true;
    if (id == -1) {
        if (a2 == 0) ok = false;
        else {
            id = lds16(a2 + AC_EVENTID);
            if (id == -1) ok = false;
            else st16(i_this + E_EVENTID, (u16)id);
        }
    }
    if (ok) {
        if (evmng_getEventData_l(evmng(), (u32)id) == 0) ok = false;
    }
    if (ok) {
        s16 id2 = lds16(i_this + E_EVENTID);
        if (!evmng_order_l(evmng(), (u32)(s32)id2)) JUT_ASSERT_l(0x1004D7D0, 0x2F7, 0x1004D7CC);
    } else {
        u16 f = ld16(i_this + E_FLAG);
        st16(i_this + E_EVENTID, 0xFFFF);
        st16(i_this + E_FLAG, f | 8);
    }
    if (ld16(order + O_TYPE) == 0xB) {
        u32 p = play();
        st16(p + 0x52B8, ld16(p + 0x52B8) | 4); /* dComIfGp_event_onEventFlag(4) */
    }
    return TRUE;
}
VERIFY(0x0253F648, dEvt_control_c_doorCheck);

/* 0253F784 */
static BOOL dEvt_control_c_potentialCheck(u32 i_this, u32 order) {
    WWHD_FUNC(0x0253F784, BOOL, i_this, order);
    u32 a1 = ld(order + O_ACTOR1);
    u32 a2 = ld(order + O_ACTOR2);
    if (a1 == 0 || a2 == 0) JUT_ASSERT_l(0x1004D7E0, 0x2B7, 0x1004D7DC);
    if (!dEvt_control_c_beforeFlagProc(i_this, order)) return FALSE;
    st16(a1 + AC_CMD, 2);
    st8(i_this + E_MODE, 2);
    dEvt_control_c_setParam(i_this, order);
    dEvt_control_c_afterFlagProc(i_this, order);
    return TRUE;
}
VERIFY(0x0253F784, dEvt_control_c_potentialCheck);

/* 0253F84C */
static BOOL dEvt_control_c_itemCheck(u32 i_this, u32 order) {
    WWHD_FUNC(0x0253F84C, BOOL, i_this, order);
    if (!dEvt_control_c_commonCheck(i_this, order, 8, 4)) return FALSE;
    st8(i_this + E_MODE, 2);
    u32 id = evmng_getEventIdx_l(evmng(), 0x1004D7F0 /* "DEFAULT_GETITEM" */, 0xFF);
    st16(i_this + E_EVENTID, (u16)id);
    if (!evmng_order_l(evmng(), id)) JUT_ASSERT_l(0x1004D800, 0x321, 0x1004D7EC);
    return TRUE;
}
VERIFY(0x0253F84C, dEvt_control_c_itemCheck);

/* 0253F8FC: HD: a fourth item button (type 9, talk button 4) */
static BOOL dEvt_control_c_talkXyCheck(u32 i_this, u32 order) {
    WWHD_FUNC(0x0253F8FC, BOOL, i_this, order);
    u16 t = ld16(order + O_TYPE);
    u32 a2 = ld(order + O_ACTOR2);
    s32 itemBtn;
    switch (t) {
    case 6: itemBtn = 0; break;
    case 7: itemBtn = 1; break;
    case 9: itemBtn = 3; break;
    default: itemBtn = 2; break;
    }
    if (ld8(play() + 0x5BBB + itemBtn) == 0xFF) return FALSE;
    if (a2 == 0 || !(ld16(a2 + AC_COND) & 0x20)) return FALSE;
    if (!dEvt_control_c_commonCheck(i_this, order, 1, 1)) return FALSE;
    st8(i_this + E_MODE, 1);
    st8(i_this + E_ITEMNO, ld8(play() + 0x5BBB + itemBtn));
    switch (ld16(order + O_TYPE)) {
    case 6: st8(i_this + E_TALKBTN, 1); break;
    case 7: st8(i_this + E_TALKBTN, 2); break;
    case 9: st8(i_this + E_TALKBTN, 4); break;
    default: st8(i_this + E_TALKBTN, 3); break;
    }
    u32 id;
    u32 cb = ld(a2 + AC_XYEVENTCB);
    if (cb == 0 || (s32)(id = gabi::call_ptr<u32>(cb, a2, itemBtn)) == -1)
        id = evmng_getEventIdx_l(evmng(), 0x1004D810 /* "DEFAULT_TALK_XY" */, 0xFF);
    st16(i_this + E_EVENTID, (u16)id);
    if (!evmng_order_l(evmng(), id)) JUT_ASSERT_l(0x1004D820, 0x1C8, 0x1004D80C);
    return TRUE;
}
VERIFY(0x0253F8FC, dEvt_control_c_talkXyCheck);

/* 0253FAE8 */
static BOOL dEvt_control_c_photoCheck_order(u32 i_this, u32 order) {
    WWHD_FUNC(0x0253FAE8, BOOL, i_this, order);
    u32 a2 = ld(order + O_ACTOR2);
    st8(i_this + E_INPHOTO, 0);
    if (ld8(play() + 0x5BE8) != 1) {
        play(); /* the status is read again for the == 0 test (same result) */
        return FALSE;
    }
    if (a2 == 0) JUT_ASSERT_l(0x1004D838, 0x1DC, 0x1004D830);
    if (!dEvt_control_c_commonCheck(i_this, order, 1, 1)) return FALSE;
    u32 id;
    u32 cb = ld(a2 + AC_PHOTOCB);
    if (cb == 0 || (s32)(id = gabi::call_ptr<u32>(cb, a2, 0)) == -1)
        id = evmng_getEventIdx_l(evmng(), 0x1004D844 /* "DEFAULT_TALK" */, 0xFF);
    st16(i_this + E_EVENTID, (u16)id);
    if (!evmng_order_l(evmng(), id)) JUT_ASSERT_l(0x1004D838, 0x1EC, 0x1004D82C);
    st8(i_this + E_MODE, 1);
    st8(i_this + E_INPHOTO, 1);
    return TRUE;
}
VERIFY(0x0253FAE8, dEvt_control_c_photoCheck_order);

/* 0253FC2C */
static BOOL dEvt_control_c_catchCheck(u32 i_this, u32 order) {
    WWHD_FUNC(0x0253FC2C, BOOL, i_this, order);
    u32 a1 = ld(order + O_ACTOR1);
    u32 a2 = ld(order + O_ACTOR2);
    if (a1 == 0) return FALSE;
    if (a2 != 0 && !(ld16(a2 + AC_COND) & 0x40)) return FALSE; /* CANCATCH */
    st16(a1 + AC_CMD, 6);
    if (a2 != 0) st16(a2 + AC_CMD, 6);
    dEvt_control_c_setParam(i_this, order);
    u8 item = ld8(play() + 0x5948); /* dComIfGp_att_getCatchChgItem */
    s16 id = lds16(i_this + E_EVENTID);
    st8(i_this + E_ITEMNO, item);
    st8(i_this + E_MODE, 2);
    if (id != -1 && !evmng_order_l(evmng(), (u32)(s32)id)) JUT_ASSERT_l(0x1004D858, 0x21A, 0x1004D854);
    st16(i_this + E_FLAG, ld16(i_this + E_FLAG) | 0x80);
    return TRUE;
}
VERIFY(0x0253FC2C, dEvt_control_c_catchCheck);

/* 0253FD1C */
static BOOL dEvt_control_c_checkStart(u32 i_this) {
    WWHD_FUNC(0x0253FD1C, BOOL, i_this);
    if (lds8(i_this + E_COUNT) == 0) return FALSE;
    s32 idx = lds8(i_this + E_FIRST);
    st8(i_this + E_COUNT, 0);
    do {
        u32 o = i_this + idx * 0x18;
        u16 t = ld16(o + O_TYPE);
        idx = lds8(o + O_NEXT);
        switch (t) {
        case 0:
            if (dEvt_control_c_talkCheck(i_this, o)) return TRUE;
            break;
        case 2:
            if (dEvt_control_c_demoCheck(i_this, o)) return TRUE;
            break;
        case 1:
        case 0xB:
            if (dEvt_control_c_doorCheck(i_this, o)) return TRUE;
            break;
        case 3:
            st8(i_this + E_MODE, 3);
            dEvt_control_c_setParam(i_this, o);
            return TRUE;
        case 4:
            return dEvt_control_c_potentialCheck(i_this, o);
        case 5:
            return dEvt_control_c_itemCheck(i_this, o);
        case 6:
        case 7:
        case 8:
        case 9:
            if (dEvt_control_c_talkXyCheck(i_this, o)) return TRUE;
            break;
        case 0xC:
            if (dEvt_control_c_photoCheck_order(i_this, o)) return TRUE;
            break;
        case 0xA:
            if (dEvt_control_c_catchCheck(i_this, o)) return TRUE;
            break;
        case 0xD:
            break;
        default:
            JUT_ASSERT_l(0x1004D868, 0x3D4, 0x1004D864);
            break;
        }
    } while (idx >= 0);
    return FALSE;
}
VERIFY(0x0253FD1C, dEvt_control_c_checkStart);

/* 0253FF0C */
static BOOL dEvt_control_c_soundProc(u32 i_this) {
    WWHD_FUNC(0x0253FF0C, BOOL, i_this);
    if (ld8(i_this + E_ENDSOUND) != 0 && ld8(i_this + E_MODE) != 2) st8(i_this + E_ENDSOUND, 0);
    return TRUE;
}
VERIFY(0x0253FF0C, dEvt_control_c_soundProc);

/* 0253FF34 */
static BOOL dEvt_control_c_check(u32 i_this) {
    WWHD_FUNC(0x0253FF34, BOOL, i_this);
    u16 f = ld16(i_this + E_FLAG);
    u8 cnt = ld8(i_this + E_COUNT);
    u8 mode = ld8(i_this + E_MODE);
    st8(i_this + E_DB, 0);
    st8(i_this + E_DC, cnt);
    if (f & 8) {
        st8(i_this + E_ENDPROC, 1);
        st16(i_this + E_FLAG, ld16(i_this + E_FLAG) & 0xFFF7);
    }
    if (mode != 0 && ld8(i_this + E_ENDPROC) == 1) {
        st8(i_this + E_ENDPROC, 0);
        dEvt_control_c_endProc(i_this);
        mode = ld8(i_this + E_MODE);
    }
    if (dEvt_control_c_checkChange(i_this)) {
        if (mode != 0) {
            st8(i_this + E_ENDPROC, 0);
            dEvt_control_c_endProc(i_this);
        }
        dEvt_control_c_changeProc(i_this);
        mode = ld8(i_this + E_MODE);
    }
    if (mode == 0 && dEvt_control_c_checkStart(i_this)) dVibration_StopQuake_l(play() + 0x599C, -1);
    dEvt_control_c_soundProc(i_this);
    st8(i_this + E_COUNT, 0);
    return FALSE;
}
VERIFY(0x0253FF34, dEvt_control_c_check);

/* 02540024: HD: the item-button table is indexed by the order type (6..8) */
static BOOL dEvt_control_c_photoCheck(u32 i_this) {
    WWHD_FUNC(0x02540024, BOOL, i_this);
    s32 cnt = lds8(i_this + E_COUNT);
    s32 first = lds8(i_this + E_FIRST);
    if (cnt == 0) return FALSE;
    u32 o = i_this + first * 0x18;
    u16 t = ld16(o + O_TYPE);
    if (t < 6 || t > 8) return FALSE;
    u8 btn = ld8(0x1004D86E + t);
    if (ld8(play() + 0x5BBB + btn) != 0x23) {
        if (ld8(play() + 0x5BBB + btn) != 0x26) return FALSE;
    }
    u32 pic = dComIfGs_getPictureInfo_l(ld(0x101F84DC) + 0x12C0);
    if (ld8(pic + 0x3C030C) == 0) return FALSE; /* dComIfGs_getPictureNum */
    u32 a2 = ld(o + O_ACTOR2);
    if (a2 == 0) return FALSE;
    if (!(ld(a2 + 0x39C) & 0x01000000)) return FALSE;
    if (ld8(play() + 0x5BE8) == 2) return FALSE;
    u32 a1 = ld(o + O_ACTOR1);
    if (a1 == 0) return FALSE;
    if (!(ld16(a1 + AC_COND) & 1)) return FALSE;
    if (!(ld16(a2 + AC_COND) & 1)) return FALSE;
    st16(o + O_TYPE, 0xC);
    st8(play() + 0x5BE8, 2); /* dComIfGp_setPictureStatusOn */
    return TRUE;
}
VERIFY(0x02540024, dEvt_control_c_photoCheck);

/* 02540158 */
static s32 dEvt_control_c_moveApproval(u32 i_this, u32 ac) {
    WWHD_FUNC(0x02540158, s32, i_this, ac);
    u8 mode = ld8(i_this + E_MODE);
    if (mode == 0) return 1;
    switch (mode) {
    case 1:
        if (dEvt_control_c_convPId(i_this, ld(i_this + E_PT1)) == ac) return 2;
        if (dEvt_control_c_convPId(i_this, ld(i_this + E_PT2)) == ac) return 2;
        break;
    case 2:
        if (dEvt_control_c_convPId(i_this, ld(i_this + E_PT1)) == ac) return 2;
        if (dEvt_control_c_convPId(i_this, ld(i_this + E_PT2)) == ac) return 2;
        if (ld8(ac + 0x2DC) != 0) return 2;
        break;
    case 3:
        if (dEvt_control_c_convPId(i_this, ld(i_this + E_PT1)) == ac) return 2;
        break;
    }
    u32 status = ld(ac + 0x2E0);
    if (status & 0x8000) return 2;
    if (ld8(i_this + E_MODE) == 1 && (status & 0x40)) return 1;
    if (ld(0x101D6010) == 1) return 1; /* dComIfGp_demo_mode */
    if (status & 0x800) return 1;
    bool bossStop;
    if (ld8(play() + 0x5292) == 3 || ld8(play() + 0x5292) == 1) {
        status = ld(ac + 0x2E0);
        if (status & 0x04000000) return 0;
        bossStop = (ld16(i_this + E_FLAG) & 0x80) != 0;
    } else {
        bossStop = (ld16(i_this + E_FLAG) & 0x80) != 0;
        status = ld(ac + 0x2E0);
    }
    if (bossStop && (status & 0x04000000)) return 0;
    if (status & 0x4000) return 1;
    if (status & 0x2000) return 1;
    return 0;
}
VERIFY(0x02540158, dEvt_control_c_moveApproval);

/* 02540310 */
static s32 dEvt_control_c_compulsory(u32 i_this, u32 actor, u32 eventName, u32 p3) {
    WWHD_FUNC(0x02540310, s32, i_this, actor, eventName, p3);
    if (ld8(i_this + E_MODE) != 0) return 0;
    return dEvt_control_c_orderOld(i_this, 3, 1, 0, p3, actor, 0, eventName);
}
VERIFY(0x02540310, dEvt_control_c_compulsory);

/* 02540348 */
static u32 dEvt_control_c_getStageEventDt(u32 i_this) {
    WWHD_FUNC(0x02540348, u32, i_this);
    u32 info = stage_getEventInfo();
    if (ld8(i_this + E_MODE) == 0) return 0;
    if (info == 0) return 0;
    u8 idx = ld8(i_this + E_INFOIDX);
    if (idx == 0xFF) return 0;
    if (!((s32)idx < (s32)ld(info + 0))) return 0;
    return ld(info + 4) + idx * 0x18;
}
VERIFY(0x02540348, dEvt_control_c_getStageEventDt);

/* 025403D4 */
static u32 dEvt_control_c_nextStageEventDt(u32 i_this, u32 idxp) {
    WWHD_FUNC(0x025403D4, u32, i_this, idxp);
    u32 info = stage_getEventInfo();
    if (idxp == 0 || info == 0) return 0;
    u8 idx = ld8(idxp);
    if (idx == 0xFF) return 0;
    if (!((s32)idx < (s32)ld(info + 0))) return 0;
    return ld(info + 4) + idx * 0x18;
}
VERIFY(0x025403D4, dEvt_control_c_nextStageEventDt);

/* 0254045C */
static u8 dEvt_control_c_getTactFreeMStick(u32 i_this, s32 which) {
    WWHD_FUNC(0x0254045C, u8, i_this, which);
    return ld8(i_this + which + E_MSTICK);
}
VERIFY(0x0254045C, dEvt_control_c_getTactFreeMStick);

/* 02540468 */
static u8 dEvt_control_c_getTactFreeCStick(u32 i_this, s32 which) {
    WWHD_FUNC(0x02540468, u8, i_this, which);
    return ld8(i_this + which + E_CSTICK);
}
VERIFY(0x02540468, dEvt_control_c_getTactFreeCStick);

/* 02540474: static initialisers (header statics) */
static void __sinit_d_event_cpp() {
    WWHD_FUNC(0x02540474, void, (u32)0);
    sinit_header_statics(0x104758BC, 0x101D62A0);
}
VERIFY(0x02540474, __sinit_d_event_cpp);

} // namespace d_event_cpp
