/**
 * d_a_npc_sarace.cpp (WWHD)
 * NPC - Loot the Sailor (Boating Course)
 *
 * The GameCube decompilation (zeldaret/tww
 * src/d/actor/d_a_npc_sarace.cpp) has only Nonmatching placeholders for this unit, so every
 * function is written from the WWHD code (cking.rpx), keeping the GameCube names, and verified
 * against it.
 */
#include "d/actor/d_a_npc_sarace.h"

/* guest string literals and data (.rodata/.data) */
#define ARC_NAME_CREATE STR(0x10021AA0)  /* "Sarace" (resLoad) */
#define ARC_NAME_DELETE STR(0x10021AC0)  /* "Sarace" (resDelete) */
#define ARC_NAME_ANM STR(0x10021A80)     /* "Sarace" (dNpc_setAnm) */
#define ARC_NAME_HIO STR(0x10021AA8)     /* HIO child name */
#define ARC_NAME_EVCUT STR(0x10021A98)   /* "Sarace" (setActorInfo2) */
#define FILE_NAME STR(0x100219F8)        /* "d_a_npc_sarace.cpp" */
#define EVENT_NAME_ORDER STR(0x10021ACC) /* "SARACE_EXPCAM" */
#define EVENT_NAME_END STR(0x10021AFC)   /* "SARACE_EXPCAM" */
#define STAGE_OCEAN STR(0x10021AF4)      /* "Ocean" */
static const u32 SAFESTRING_VT = 0x10021948; /* this unit's sead::SafeString vtable */
static const u32 L_CYL_SRC = 0x101C61F0;
static const u32 HIO = 0x10468620;           /* daNpc_Sarace_HIO_c l_HIO (+0: child number) */
static const u32 MTX = 0x1048D0CC;           /* mDoMtx_stack_c::now */
static const u32 SHIP_RACE_RUPEE = 0x101D5F18;  /* daNpc_Sarace_c::ship_race_rupee */
static const u32 SHIP_RACE_RESULT = 0x101D5F1C; /* daNpc_Sarace_c::ship_race_result */

enum : u32 {
    FN_nodeCallBack = 0x022DBA64,
    FN_CallbackCreateHeap = 0x022DC150,
    FN_dummy_action = 0x022DD728,
    FN_wait_action = 0x022DD750,
    FN_event_endCheck_action = 0x022DD844,
};

template <class T> static T rd(u32 p, u32 off) { return gabi::load<T>(p + off); }
template <class T> static void wr(u32 p, u32 off, T v) { gabi::store<T>(p + off, v); }
static u32 play() { return gabi::call<u32>(0x025200D4); }
static u32 saveEvent() { return gabi::load<u32>(0x101F84DC) + 0x644; }
static void copyWords(u32 dst, u32 src, int n) {
    u32 v[12];
    for (int i = 0; i < n; ++i) v[i] = gabi::load<u32>(src + 4 * i);
    for (int i = 0; i < n; ++i) gabi::store<u32>(dst + 4 * i, v[i]);
}
/* lfs.../stfs... copy (through FPRs: the recompiled code quiets signalling NaNs) */
static void copyFloats(u32 dst, u32 src, int n) {
    f32 v[12];
    for (int i = 0; i < n; ++i) v[i] = gabi::load<f32>(src + 4 * i);
    for (int i = 0; i < n; ++i) gabi::store<f32>(dst + 4 * i, v[i]);
}
/* J3DModel joint matrix [index] (sets the HD "matrices dirty" flag of the matrix block) */
static u32 jointMtx(u32 model, u32 index) {
    u32 block = rd<u32>(model, 0x2C);
    u32 matrices = rd<u32>(block, 0x10);
    wr<u16>(block, 4, rd<u16>(block, 4) | 0x10);
    return matrices + index * 0x30;
}
static u32 resGet(u32 name, s32 index) {
    gabi::Local<SafeString> key;
    key->mStringTop = name; key->__vtbl = SAFESTRING_VT;
    return gabi::call<u32>(0x026066C4, gabi::load<u32>(0x101F4F28), key.get(), index);
}

/* inline setAction(int (daNpc_Sarace_c::*)(void*), void*): GHS pointer-to-member compare, then
 * the old action is called with mActionStatus -1 and the new one with 0 */
static void setAction(daNpc_Sarace_c* a, u32 func) {
    u32 p = gabi::ea(a);
    s16 index = a->mAction.i;
    bool callOld = true;
    if (index == -1) {
        if (a->mAction.d == 0 && a->mAction.f == func) return;
    } else if (index == 0) {
        callOld = false;
    }
    if (callOld) {
        s16 delta = a->mAction.d; index = a->mAction.i;
        u32 receiver = p + (s32)delta;
        a->mActionStatus = -1;
        if (index < 0) gabi::call_ptr(a->mAction.f, receiver, 0);
        else {
            u32 vt = gabi::load<u32>(receiver + (s32)rd<s16>(p, 0x8AA));
            gabi::call_ptr(gabi::load<u32>(vt + (s32)index * 8 + 4), receiver, 0);
        }
    }
    a->mAction.d = 0; a->mAction.i = -1; a->mAction.f = func; a->mActionStatus = 0;
    gabi::call_ptr(a->mAction.f, p + (s32)a->mAction.d, 0);
}

/* 022DBA64 */
BOOL nodeCallBack(J3DNode* node, s32 calcTiming) {
    WWHD_FUNC(0x022DBA64, BOOL, node, calcTiming);
    if (calcTiming != 0) return TRUE;
    u32 model = gabi::load<u32>(0x104B462C);
    u32 actor = rd<u32>(model, 0xB8);
    if (!actor) return TRUE;
    u32 joint = gabi::call<u32>(0x027F7878, node);
    u32 jntNo = rd<u16>(joint, 4);
    gabi::call(0x028E90D4, jointMtx(model, jntNo), MTX);
    if (jntNo == (u32)(s32)rd<s8>(actor, 0x3B4)) {
        /* static const cXyz l_offsetAttPos (zero), initialised on first use */
        if (!gabi::load<u32>(0x1046868C)) {
            gabi::store<f32>(0x10468680, 0.0f); gabi::store<f32>(0x10468688, 0.0f);
            gabi::store<u32>(0x1046868C, 1); gabi::store<f32>(0x10468684, 0.0f);
        }
        gabi::Local<cXyz> eyeOffset;
        eyeOffset->x = 24.0f; eyeOffset->y = 14.0f; eyeOffset->z = 0.0f;
        gabi::call(0x028E8F64, MTX, 0x10468680, actor + 0x878);
        gabi::call(0x025F1BF4, MTX, rd<s16>(actor, 0x3AE));
        gabi::call(0x025F1C5C, MTX, (s16)-rd<s16>(actor, 0x3AC));
        gabi::call(0x028E8F64, MTX, eyeOffset.get(), actor + 0x86C);
    } else if (jntNo == (u32)(s32)rd<s8>(actor, 0x3B5)) {
        gabi::call(0x025F1BF4, MTX, rd<s16>(actor, 0x3B2));
        gabi::call(0x025F1C5C, MTX, (s16)-rd<s16>(actor, 0x3B0));
    }
    gabi::call(0x028E90D4, MTX, 0x104B4868);
    copyFloats(jointMtx(model, jntNo), MTX, 12);
    return TRUE;
}
VERIFY(0x022DBA64, nodeCallBack);

/* 022DBCBC */
/* GameCube initTexPatternAnm(bool): the incoming register is forwarded unchanged */
BOOL daNpc_Sarace_initTexPatternAnm(daNpc_Sarace_c* a, u32 modify) {
    WWHD_FUNC(0x022DBCBC, BOOL, a, modify);
    u32 p = gabi::ea(a);
    u32 modelData = rd<u32>(rd<u32>(rd<u32>(p, 0x7E4), 0x90), 0xAC);
    u32 pattern = resGet(0x100219B8, gabi::load<s32>(0x10021944));
    wr<u32>(p, 0x7F0, pattern);
    if (!pattern) {
        gabi::call(0x0273AA24, STR(0x100219C0), 0xFC, STR(0x100219D4));
        pattern = rd<u32>(p, 0x7F0);
    }
    if (!gabi::call<s32>(0x025E789C, p + 0x7F4, modelData, pattern, 1, 2, 1.0f, 0, -1, (u32)modify, 0))
        return FALSE;
    a->mBtpFrame = 0; a->mBlinkTimer = 0;
    return TRUE;
}
VERIFY(0x022DBCBC, daNpc_Sarace_initTexPatternAnm);

static u32 jointNameTable(u32 modelData) {
    u32 header = gabi::call<u32>(0x027F68FC, modelData);
    u32 offset = rd<u32>(header, 0x10);
    return offset ? header + 0x10 + offset : 0;
}
/* 022DBDC0 */
BOOL daNpc_Sarace_CreateHeap(daNpc_Sarace_c* a) {
    WWHD_FUNC(0x022DBDC0, BOOL, a);
    u32 p = gabi::ea(a);
    u32 modelData = resGet(0x100219E8, 0xE);
    if (!modelData) gabi::call(0x0273AA24, FILE_NAME, 0x3F8, STR(0x10021A0C));
    u32 bck = resGet(0x100219E8, 9);
    u32 morf = gabi::call<u32>(0x025E4F64, 0, modelData, 0, 0, bck, 2, 1.0f, 0, -1, 1, 0, 0, 0x11020203);
    if (morf) {
        wr<u32>(p, 0x44C, morf);
        if (rd<u32>(morf, 0x90)) goto morfOk;
    }
    wr<u32>(p, 0x44C, 0);
    return FALSE;
morfOk:
    {
        s8 head = gabi::call<s8>(0x027DF9B0, jointNameTable(modelData), STR(0x100219F0)); /* "head" */
        wr<s8>(p, 0x3B4, head);
        if (head < 0) gabi::call(0x0273AA24, FILE_NAME, 0x409, STR(0x10021A20));
        s8 backbone = gabi::call<s8>(0x027DF9B0, jointNameTable(modelData), STR(0x10021A3C)); /* "backbone" */
        wr<s8>(p, 0x3B5, backbone);
        if (backbone < 0) gabi::call(0x0273AA24, FILE_NAME, 0x40B, STR(0x10021A48));
    }
    u32 headData = resGet(0x100219E8, 0x11);
    if (!headData) gabi::call(0x0273AA24, FILE_NAME, 0x419, STR(0x10021A68));
    u32 headBck = resGet(0x100219E8, 7);
    u32 headMorf = gabi::call<u32>(0x025E4F64, 0, headData, 0, 0, headBck, 2, 1.0f, 0, -1, 1, 0, 0, 0x11020203);
    wr<u32>(p, 0x7E4, headMorf);
    if (!headMorf || !rd<u32>(headMorf, 0x90)) return FALSE;
    a->field_0x8AC = 0;
    if (!daNpc_Sarace_initTexPatternAnm(a, 0)) return FALSE;
    for (u16 i = 0; i < rd<u16>(gabi::call<u32>(0x027F3F94, modelData), 8); i = (u16)(i + 1)) {
        if ((u32)i == (u32)(s32)rd<s8>(p, 0x3B4) || (u32)i == (u32)(s32)rd<s8>(p, 0x3B5)) {
            u32 data = rd<u32>(rd<u32>(rd<u32>(p, 0x44C), 0x90), 0xAC);
            u32 count = rd<u32>(data, 4), joints = rd<u32>(data, 8);
            if ((u32)i < count) joints += (u32)i * 0x1C;
            wr<u32>(joints, 8, FN_nodeCallBack);
        }
    }
    wr<u32>(rd<u32>(rd<u32>(p, 0x44C), 0x90), 0xB8, p);
    gabi::call(0x024EFF44, p + 0x614, 30.0f, 0.0f);
    gabi::call(0x024F06B4, p + 0x450, p + 0x314, p + 0x300, a, 1, p + 0x614, p + 0x33C, 0, 0);
    return TRUE;
}
VERIFY(0x022DBDC0, daNpc_Sarace_CreateHeap);

/* 022DC150 CallbackCreateHeap: a tail branch to CreateHeap */
BOOL daNpc_Sarace_CallbackCreateHeap(daNpc_Sarace_c* a) {
    WWHD_FUNC(0x022DC150, BOOL, a);
    return daNpc_Sarace_CreateHeap(a);
}
VERIFY(0x022DC150, daNpc_Sarace_CallbackCreateHeap);

/* 022DC154 set_mtx (unnamed by the matcher; GameCube 0x173C) */
void daNpc_Sarace_set_mtx(daNpc_Sarace_c* a) {
    WWHD_FUNC(0x022DC154, void, a);
    u32 p = gabi::ea(a);
    u32 model = rd<u32>(rd<u32>(p, 0x44C), 0x90);
    u32 headModel = rd<u32>(rd<u32>(p, 0x7E4), 0x90);
    gabi::call(0x028E93CC, MTX, rd<f32>(p, 0x314), rd<f32>(p, 0x318), rd<f32>(p, 0x31C));
    gabi::call(0x025F1C28, MTX, rd<s16>(p, 0x322));
    copyFloats(model + 0xC8, MTX, 12);
    gabi::call(0x025E55A0, rd<u32>(p, 0x44C));
    copyFloats(headModel + 0xC8, jointMtx(model, (u32)(s32)rd<s8>(p, 0x3B4)), 12);
    gabi::call(0x025E55A0, rd<u32>(p, 0x7E4));
}
VERIFY(0x022DC154, daNpc_Sarace_set_mtx);

/* 022DC2C0 */
/* GameCube setAnm(s8, f32); GHS uses the incoming register unextended (table index, compare) */
void daNpc_Sarace_setAnm(daNpc_Sarace_c* a, s32 idx, f32 morf) {
    WWHD_FUNC(0x022DC2C0, void, a, idx, morf);
    s32 cur = a->mAnmIdx;
    if (morf < 0.0f) morf = gabi::load<f32>(0x101C6240 + idx * 4);
    if ((u32)idx == (u32)cur || cur == -1) return;
    a->mAnmIdx = (s8)idx;
    gabi::call(0x0259D454, a->mpMorf.get(), gabi::load<s32>(0x101C6234 + idx * 4), morf,
               gabi::load<f32>(0x101C624C + idx * 4), gabi::load<s32>(0x10021990 + idx * 4), -1, ARC_NAME_ANM);
    s32 i = a->mAnmIdx;
    gabi::call(0x0259D454, a->mpHeadMorf.get(), gabi::load<s32>(0x101C6234 + i * 4), morf,
               gabi::load<f32>(0x101C624C + i * 4), gabi::load<s32>(0x1002199C + i * 4), -1, ARC_NAME_ANM);
}
VERIFY(0x022DC2C0, daNpc_Sarace_setAnm);

/* 022DC3C8 */
BOOL daNpc_Sarace_CreateInit(daNpc_Sarace_c* a) {
    WWHD_FUNC(0x022DC3C8, BOOL, a);
    u32 p = gabi::ea(a);
    u16 ay = rd<u16>(p, 0x322), az = rd<u16>(p, 0x324), ax = rd<u16>(p, 0x320);
    wr<u16>(p, 0x88A, ay); wr<u16>(p, 0x888, ax);
    wr<f32>(p, 0x374, -30.0f);
    wr<u32>(p, 0x39C, 10);
    wr<u16>(p, 0x88C, az);
    setAction(a, FN_wait_action);
    u32 z = rd<u32>(p, 0x31C), x = rd<u32>(p, 0x314);
    wr<u32>(p, 0x880, z); wr<u32>(p, 0x878, x); wr<u32>(p, 0x874, z);
    u32 y = rd<u32>(p, 0x318);
    wr<u32>(p, 0x86C, x); wr<u32>(p, 0x870, y); wr<u32>(p, 0x87C, y);
    gabi::call(0x02515F14, p + 0x654, 0xFF, 0xFF, a);
    gabi::call(0x02516518, p + 0x690, L_CYL_SRC);
    wr<u32>(p, 0x6D4, p + 0x654);
    gabi::call(0x025A15AC, a, 60.0f, 150.0f);
    a->field_0x8A0 = 0;
    gabi::call(0x0259F814, p + 0x3E0, ARC_NAME_EVCUT, a);
    wr<u8>(p, 0x38B, 0xAD); wr<u8>(p, 0x389, 0xAD);
    daNpc_Sarace_set_mtx(a);
    a->mEventMsgNo = 0; a->mBuoyId[1] = 0xFFFFFFFF; a->mBuoyId[0] = 0xFFFFFFFF;
    daNpc_Sarace_setAnm(a, 0, -1.0f);
    if (rd<s16>(play(), 0x513C) == 1 && rd<s8>(play(), 0x513E) == 0x30 && gabi::load<s32>(SHIP_RACE_RESULT) != 0) {
        a->mEventOrder = 1; a->mEventMsgNo = 0xFB4;
        gabi::call(0x025D76A8, a);
    }
    gabi::call(0x025E535C, a->mpMorf.get(), p + 0x37C, 0, 0);
    gabi::call(0x025E55A0, a->mpMorf.get());
    gabi::call(0x025E535C, a->mpHeadMorf.get(), 0, 0, 0);
    gabi::call(0x025E55A0, a->mpHeadMorf.get());
    return TRUE;
}
VERIFY(0x022DC3C8, daNpc_Sarace_CreateInit);

/* 022DC65C */
s32 daNpc_Sarace__create(daNpc_Sarace_c* a) {
    WWHD_FUNC(0x022DC65C, s32, a);
    u32 p = gabi::ea(a);
    u32 flags = rd<u32>(p, 0x2E4);
    if (!(flags & 8)) {
        if (a) {
            gabi::call(0x025A1458, a);
            wr<u32>(p, 0xB4, 0x10021B28);
            gabi::call(0x025E7820, p + 0x7F4);
            flags = rd<u32>(p, 0x2E4);
        }
        wr<u32>(p, 0x2E4, flags | 8);
    }
    s32 phase = gabi::call<s32>(0x02520460, &a->mPhs, ARC_NAME_CREATE);
    if (phase != 4) return phase;
    if (!gabi::call<s32>(0x025D63E8, a, FN_CallbackCreateHeap, 0x2760)) return 5;
    u32 model = rd<u32>(rd<u32>(p, 0x44C), 0x90);
    wr<u32>(p, 0x348, model ? model + 0xC8 : 0);
    if (gabi::load<s8>(HIO) < 0) gabi::store<s8>(HIO, gabi::call<s8>(0x025F0A10, ARC_NAME_HIO, HIO));
    if (!daNpc_Sarace_CreateInit(a)) return 5;
    return phase;
}
VERIFY(0x022DC65C, daNpc_Sarace__create);

/* 022DC778 */
s32 daNpc_Sarace_Create(daNpc_Sarace_c* a) {
    WWHD_FUNC(0x022DC778, s32, a);
    return daNpc_Sarace__create(a);
}
VERIFY(0x022DC778, daNpc_Sarace_Create);

/* 022DC77C */
BOOL daNpc_Sarace__delete(daNpc_Sarace_c* a) {
    WWHD_FUNC(0x022DC77C, BOOL, a);
    gabi::call(0x025204C8, &a->mPhs, ARC_NAME_DELETE);
    u32 morf = gabi::ea(a->mpMorf.get());
    if (morf) gabi::call(0x025E563C, morf);
    s8 child = gabi::load<s8>(HIO);
    if (child >= 0) {
        gabi::call(0x025F0A18, child);
        gabi::store<s8>(HIO, -1);
    }
    return TRUE;
}
VERIFY(0x022DC77C, daNpc_Sarace__delete);

/* 022DC7E4 */
BOOL daNpc_Sarace_Delete(daNpc_Sarace_c* a) {
    WWHD_FUNC(0x022DC7E4, BOOL, a);
    return daNpc_Sarace__delete(a);
}
VERIFY(0x022DC7E4, daNpc_Sarace_Delete);

static s32 btpFrameMax(daNpc_Sarace_c* a) {
    u32 anm = gabi::ea(a->mpTexPattern.get());
    return gabi::call_ptr<s32>(rd<u32>(rd<u32>(anm, 4), 0x14), anm);
}
/* 022DC7E8 */
void daNpc_Sarace_playTexPatternAnm(daNpc_Sarace_c* a) {
    WWHD_FUNC(0x022DC7E8, void, a);
    if (gabi::call<s16>(0x02055B64, &a->mBlinkTimer) != 0) return;
    s32 frameMax = btpFrameMax(a);
    if ((s32)a->mBtpFrame >= frameMax) {
        s32 max = btpFrameMax(a);
        a->mBtpFrame = (u8)(a->mBtpFrame - max);
        f32 r = gabi::call<f32>(0x020198D8, 100.0f);
        a->mBlinkTimer = (s16)gabi::ftoi(r + 30.0f);
    } else {
        a->mBtpFrame = (u8)(a->mBtpFrame + 1);
    }
}
VERIFY(0x022DC7E8, daNpc_Sarace_playTexPatternAnm);

/* 022DC8AC checkOrder (unnamed by the matcher; the matcher's checkOrder is wrong, GameCube 0x7D8) */
void daNpc_Sarace_checkOrder(daNpc_Sarace_c* a) {
    WWHD_FUNC(0x022DC8AC, void, a);
    u16 condition = rd<u16>(gabi::ea(a), 0xF8);
    if (condition == 2) {
        if (a->mEventOrder == 3) {
            setAction(a, FN_event_endCheck_action);
            a->mEventOrder = 0;
        }
    } else if (condition == 1) {
        s8 order = a->mEventOrder;
        if (order == 1 || order == 2) {
            a->mEventOrder = 0;
            a->mbTalkRequest = 1;
        }
    }
}
VERIFY(0x022DC8AC, daNpc_Sarace_checkOrder);

/* 022DCA3C */
void daNpc_Sarace_eventOrder(daNpc_Sarace_c* a) {
    WWHD_FUNC(0x022DCA3C, void, a);
    s8 order = a->mEventOrder;
    if (order == 3) {
        gabi::call(0x025D77DC, a, EVENT_NAME_ORDER, 1, 0xFFFF);
        return;
    }
    if (order == 1 || order == 2) {
        u32 p = gabi::ea(a);
        order = a->mEventOrder;
        wr<u16>(p, 0xFA, rd<u16>(p, 0xFA) | 1);
        if (order == 1) gabi::call(0x025D76A8, a);
    }
}
VERIFY(0x022DCA3C, daNpc_Sarace_eventOrder);

/* 022DCA94 */
BOOL daNpc_Sarace__execute(daNpc_Sarace_c* a) {
    WWHD_FUNC(0x022DCA94, BOOL, a);
    u32 p = gabi::ea(a);
    s16 hio[9];
    for (int i = 0; i < 9; ++i) hio[i] = gabi::load<s16>(HIO + 8 + 2 * i);
    gabi::call(0x0259E08C, p + 0x3AC, hio[1], hio[3], hio[5], hio[7], hio[0], hio[2], hio[4], hio[6], hio[8]);
    daNpc_Sarace_playTexPatternAnm(a);
    gabi::call(0x025E535C, a->mpMorf.get(), 0, 0, 0);
    gabi::call(0x025E535C, a->mpHeadMorf.get(), 0, 0, 0);
    daNpc_Sarace_checkOrder(a);
    s16 index = a->mAction.i, delta = a->mAction.d;
    u32 receiver = p + (s32)delta;
    if (index < 0) gabi::call_ptr(a->mAction.f, receiver, 0);
    else {
        u32 vt = gabi::load<u32>(receiver + (s32)rd<s16>(p, 0x8AA));
        gabi::call_ptr(gabi::load<u32>(vt + (s32)index * 8 + 4), receiver, 0);
    }
    gabi::call(0x0259F858, p + 0x3E0);
    daNpc_Sarace_eventOrder(a);
    gabi::call(0x025D6870, a, p + 0x654);
    gabi::call(0x024F08A8, p + 0x450, play() + 0x12A0);
    wr<u8>(p, 0x1C9, gabi::call<u8>(0x024EF130, play() + 0x12A0, p + 0x538));
    wr<u8>(p, 0x1CA, gabi::call<u8>(0x024EEEB8, play() + 0x12A0, p + 0x538));
    daNpc_Sarace_set_mtx(a);
    gabi::call(0x025A15AC, a, 60.0f, 150.0f);
    return TRUE;
}
VERIFY(0x022DCA94, daNpc_Sarace__execute);

/* 022DCBF8 */
BOOL daNpc_Sarace_Execute(daNpc_Sarace_c* a) {
    WWHD_FUNC(0x022DCBF8, BOOL, a);
    return daNpc_Sarace__execute(a);
}
VERIFY(0x022DCBF8, daNpc_Sarace_Execute);

/* 022DCBFC */
BOOL daNpc_Sarace__draw(daNpc_Sarace_c* a) {
    WWHD_FUNC(0x022DCBFC, BOOL, a);
    u32 p = gabi::ea(a);
    u32 model = rd<u32>(rd<u32>(p, 0x44C), 0x90);
    u32 headModel = rd<u32>(rd<u32>(p, 0x7E4), 0x90);
    u32 headData = rd<u32>(headModel, 0xAC);
    gabi::call(0x025626A4, gabi::call<u32>(0x02555D0C), 0, p + 0x314, p + 0x110);
    gabi::call(0x02562F5C, gabi::call<u32>(0x02555D0C), model, p + 0x110);
    gabi::call(0x02562F5C, gabi::call<u32>(0x02555D0C), headModel, p + 0x110);
    gabi::call(0x025E7B3C, p + 0x7F4, headData, (u32)a->mBtpFrame);
    gabi::call(0x025E5590, a->mpMorf.get());
    gabi::call(0x025E5590, a->mpHeadMorf.get());
    wr<u32>(headData, 0x38, 0);
    gabi::call(0x025BED80, 0x82, a, 1.0f, 1.0f, 1.0f);
    return TRUE;
}
VERIFY(0x022DCBFC, daNpc_Sarace__draw);

/* 022DCCCC */
BOOL daNpc_Sarace_Draw(daNpc_Sarace_c* a) {
    WWHD_FUNC(0x022DCCCC, BOOL, a);
    return daNpc_Sarace__draw(a);
}
VERIFY(0x022DCCCC, daNpc_Sarace_Draw);

/* 022DCCD0 chkAttention(cXyz pos, s16 angle): the cXyz is passed by pointer to a copy */
u8 daNpc_Sarace_chkAttention(daNpc_Sarace_c* a, cXyz* pos, s16 angle) {
    WWHD_FUNC(0x022DCCD0, u8, a, pos, angle);
    u32 player = rd<u32>(play(), 0x5B2C);
    f32 dz = rd<f32>(player, 0x31C) - pos->z;
    f32 radius = gabi::load<f32>(HIO + 0x24);
    f32 dx = rd<f32>(player, 0x314) - pos->x;
    s32 limit = gabi::load<s16>(HIO + 0x20);
    f32 dist = gabi::call<f32>(0x028F4384, gabi::fmadds(dx, dx, dz * dz));
    s16 target = gabi::call<s16>(0x020195B0, dx, dz);
    if (a->mbAttention) {
        radius = radius + 40.0f;
        limit += 0x71C;
    }
    s16 diff = (s16)(target - angle);
    s32 mag = diff < 0 ? -(s32)diff : diff;
    return limit > mag && radius > dist;
}
VERIFY(0x022DCCD0, daNpc_Sarace_chkAttention);

/* 022DCDFC */
u16 daNpc_Sarace_next_msgStatus(daNpc_Sarace_c* a, be<u32>* msgNo) {
    WWHD_FUNC(0x022DCDFC, u16, a, msgNo);
    u32 msg = *msgNo;
    u32 msgObject = gabi::load<u32>(0x101F4B5C);
    switch (msg) {
    case 0xFA1: case 0xFA2: case 0xFA5: case 0xFA7: case 0xFB1: case 0xFB2: case 0xFB4: case 0xFB5: case 0xFB7:
        *msgNo = msg + 1; break;
    case 0xFB8: *msgNo = 0xFB3; break;
    case 0xFA3: {
        s32 choice = rd<s32>(msgObject, 0x948);
        if (choice == 2) { *msgNo = 0xFA4; break; }
        u32 save = gabi::load<u32>(0x101F84DC);
        if (choice == 0) {
            if (rd<u16>(save, 0x24) < 30) { *msgNo = 0xFAF; break; }
            u32 pl = play();
            wr<s32>(pl, 0x5B48, rd<s32>(pl, 0x5B48) - 30);
            gabi::call(0x025B8B7C, saveEvent(), 0x2820);
            *msgNo = 0xFB0;
        } else if (!gabi::call<BOOL>(0x025B8B94, save + 0x644, 0x2808)) {
            gabi::call(0x025B8B68, saveEvent(), 0x2808);
            *msgNo = 0xFAD;
        } else {
            *msgNo = 0xFAE;
        }
        break;
    }
    case 0xFA8:
        if (gabi::call<BOOL>(0x025B8B94, saveEvent(), 0x2820)) { *msgNo = 0xFAA; break; }
        *msgNo = 0xFA9; break;
    case 0xFAA: *msgNo = 0xFA9; break;
    case 0xFA9: case 0xFB3: *msgNo = 0xFA3; break;
    default: return 0x10;
    }
    return 0xF;
}
VERIFY(0x022DCDFC, daNpc_Sarace_next_msgStatus);

/* 022DCFC8 */
u32 daNpc_Sarace_getMsg(daNpc_Sarace_c* a) {
    WWHD_FUNC(0x022DCFC8, u32, a);
    u32 msg = a->mEventMsgNo;
    if (msg != 0) {
        if (msg == 0xFB4) {
            s32 result = gabi::load<s32>(SHIP_RACE_RESULT);
            if (result == 1) { a->mEventMsgNo = 0; return 0xFB7; }
            if (result == 3) { a->mEventMsgNo = 0; return 0xFB1; }
            s16 rupee = gabi::load<s16>(SHIP_RACE_RUPEE + 2);
            wr<s16>(play(), 0x5BA0, rupee);
            msg = a->mEventMsgNo;
        }
        a->mEventMsgNo = 0;
        return msg;
    }
    if (!gabi::call<BOOL>(0x025B8B94, saveEvent(), 0x2810)) {
        gabi::call(0x025B8B68, saveEvent(), 0x2810);
        return 0xFA1;
    }
    if (gabi::call<BOOL>(0x025B8B94, saveEvent(), 0x2840)) return 0xFA2;
    return 0xFA1;
}
VERIFY(0x022DCFC8, daNpc_Sarace_getMsg);

/* 022DD0B0 anmAtr(u16): the attribute is ignored, the requested animation comes from the play object */
void daNpc_Sarace_anmAtr(daNpc_Sarace_c* a, u16 attr) {
    WWHD_FUNC(0x022DD0B0, void, a, attr);
    u8 req = rd<u8>(play(), 0x5BC5);
    if (req == 0) daNpc_Sarace_setAnm(a, 0, -1.0f);
    else if (req == 1) daNpc_Sarace_setAnm(a, 1, gabi::load<f32>(HIO + 0x2C));
    else if (req == 2) daNpc_Sarace_setAnm(a, 2, gabi::load<f32>(HIO + 0x30));
    wr<u8>(play(), 0x5BC5, 0xFF);
}
VERIFY(0x022DD0B0, daNpc_Sarace_anmAtr);

/* 022DD148 */
void daNpc_Sarace_setAttention(daNpc_Sarace_c* a) {
    WWHD_FUNC(0x022DD148, void, a);
    u32 p = gabi::ea(a);
    copyFloats(p + 0x37C, p + 0x86C, 3);
    f32 y = gabi::fadds_ppc(rd<f32>(p, 0x87C), gabi::load<f32>(HIO + 0x1C));
    f32 z = rd<f32>(p, 0x880), x = rd<f32>(p, 0x878);
    wr<f32>(p, 0x398, z); wr<f32>(p, 0x394, y); wr<f32>(p, 0x390, x);
}
VERIFY(0x022DD148, daNpc_Sarace_setAttention);

/* 022DD188 */
void daNpc_Sarace_lookBack(daNpc_Sarace_c* a) {
    WWHD_FUNC(0x022DD188, void, a);
    u32 p = gabi::ea(a);
    f32 eyeX = 0.0f, eyeY = 0.0f, eyeZ = 0.0f;
    u32 target = 0;
    s16 yrot = rd<s16>(p, 0x322);
    s8 mode = a->mMode;
    gabi::Local<cXyz> eye, tgt, playerEye, playerEye2;
    if ((u32)(s32)mode >= 1 && (u32)(s32)mode <= 2) {
        bool track;
        if (mode == 2) {
            wr<u8>(p, 0x3B6, 1);
            track = a->mbAttention != 0;
            if (!track) {
                gabi::call(0x0259D54C, playerEye.get(), gabi::load<f32>(HIO + 4));
                copyWords(gabi::ea(tgt.get()), gabi::ea(playerEye.get()), 3);
                s16 angle = gabi::call<s16>(0x0200F93C, p + 0x314, tgt.get());
                gabi::call(0x0200F428, p + 0x322, angle, 4, 0x1800);
                track = a->mbAttention != 0;
            }
        } else {
            track = a->mbAttention != 0;
        }
        if (track) {
            gabi::call(0x0259D54C, playerEye2.get(), gabi::load<f32>(HIO + 4));
            eyeY = rd<f32>(p, 0x380);
            copyWords(gabi::ea(tgt.get()), gabi::ea(playerEye2.get()), 3);
            eyeX = rd<f32>(p, 0x314);
            target = gabi::ea(tgt.get());
            eyeZ = rd<f32>(p, 0x31C);
        }
    }
    eye->x = eyeX; eye->y = eyeY; eye->z = eyeZ;
    if (rd<u8>(p, 0x3B6) == 0) {
        a->mLookAngle = 0;
        gabi::call(0x0259DED0, p + 0x3AC, p + 0x322, target, eye.get(), yrot, gabi::load<s16>(HIO + 0x1A), 1);
    } else {
        gabi::call(0x0259DED0, p + 0x3AC, p + 0x322, target, eye.get(), yrot, gabi::load<s16>(HIO + 0x1A), 0);
    }
}
VERIFY(0x022DD188, daNpc_Sarace_lookBack);

/* 022DD35C */
void daNpc_Sarace_wait01(daNpc_Sarace_c* a) {
    WWHD_FUNC(0x022DD35C, void, a);
    if (a->mbTalkRequest) { a->mMode = 2; return; }
    if (a->mEventOrder == 0) a->mEventOrder = 2;
}
VERIFY(0x022DD35C, daNpc_Sarace_wait01);

/* 022DD390 */
void daNpc_Sarace_talk01(daNpc_Sarace_c* a) {
    WWHD_FUNC(0x022DD390, void, a);
    u32 p = gabi::ea(a);
    if (gabi::call<u16>(0x025A11EC, a, 1) == 0x12) {
        u32 msg = a->mCurrMsgNo;
        a->mMode = 1;
        if (msg == 0xFA4) {
            /* static cXyz l_buoyPos[2], initialised on first use */
            if (!gabi::load<u32>(0x10468690)) {
                gabi::store<u32>(0x10468690, 1);
                gabi::store<f32>(0x10468668, gabi::load<f32>(0x10021AE4));
                gabi::store<f32>(0x1046865C, gabi::load<f32>(0x10021AE4));
                gabi::store<f32>(0x10468660, gabi::load<f32>(0x10021AE8));
                gabi::store<f32>(0x1046866C, gabi::load<f32>(0x10021AF0));
                gabi::store<f32>(0x10468658, gabi::load<f32>(0x10021AE0));
                gabi::store<f32>(0x10468664, gabi::load<f32>(0x10021AEC));
            }
            u32 pl = play();
            wr<u16>(pl, 0x52B8, rd<u16>(pl, 0x52B8) | 8);
            a->mEventOrder = 3;
            a->mBuoyId[0] = gabi::call<u32>(0x025D5834, 0x1C9, 0x017F0101, 0x10468658, -1, 0, 0, -1, 0);
            u32 id = gabi::call<u32>(0x025D5834, 0x1C9, 0x007F0101, 0x10468664, -1, 0, 0, -1, 0);
            a->mbTalkRequest = 0;
            a->mBuoyId[1] = id;
            return;
        }
        if (msg == 0xFB0) {
            gabi::call(0x025B8B68, saveEvent(), 0x2840);
            gabi::call(0x0252012C, STAGE_OCEAN, 1, 0, 0, 0.0f, 0, 1, 0);
            setAction(a, FN_dummy_action);
            a->mbTalkRequest = 0;
            return;
        }
        u32 pl = play();
        wr<u16>(pl, 0x52B8, rd<u16>(pl, 0x52B8) | 8);
        gabi::store<s32>(SHIP_RACE_RUPEE, 0);
        gabi::store<s32>(SHIP_RACE_RESULT, 0);
        daNpc_Sarace_setAnm(a, 0, -1.0f);
        a->mbTalkRequest = 0;
        return;
    }
    if (a->mCurrMsgNo == 0xFB6 && rd<u8>(play(), 0x5BD2) == 1) {
        s32 rupee = gabi::load<s32>(SHIP_RACE_RUPEE);
        u32 pl = play();
        wr<s32>(pl, 0x5B48, rd<s32>(pl, 0x5B48) + rupee);
        u32 count = gabi::call<u8>(0x025B8BB0, saveEvent(), 0xAAFF);
        s32 next = (s32)count + 1;
        if (next > 12) next = 12;
        u8 value = (u8)next;
        gabi::call(0x025B8AF4, saveEvent(), 0xAAFF, value);
        if (count != (u32)value) gabi::call(0x025B8B68, saveEvent(), 0x2820);
    }
    (void)p;
}
VERIFY(0x022DD390, daNpc_Sarace_talk01);

/* 022DD728 */
BOOL daNpc_Sarace_dummy_action(daNpc_Sarace_c* a, void* prm) {
    WWHD_FUNC(0x022DD728, BOOL, a, prm);
    if (a->mActionStatus == 0) {
        a->mMode = 1;
        a->mActionStatus = (s8)(a->mActionStatus + 1);
    }
    return TRUE;
}
VERIFY(0x022DD728, daNpc_Sarace_dummy_action);

/* 022DD750 */
BOOL daNpc_Sarace_wait_action(daNpc_Sarace_c* a, void* prm) {
    WWHD_FUNC(0x022DD750, BOOL, a, prm);
    u32 p = gabi::ea(a);
    s8 status = a->mActionStatus;
    if (status == 0) {
        a->mMode = 1;
        a->mActionStatus = (s8)(a->mActionStatus + 1);
        return TRUE;
    }
    if (status == -1) return TRUE;
    gabi::Local<cXyz> pos;
    s16 angle = (s16)(rd<s16>(p, 0x322) + rd<s16>(p, 0x3AE) + rd<s16>(p, 0x3B2));
    copyFloats(gabi::ea(pos.get()), p + 0x314, 3);
    u8 attention = daNpc_Sarace_chkAttention(a, pos.get(), angle);
    s8 mode = a->mMode;
    a->mbAttention = attention;
    if (mode == 1) daNpc_Sarace_wait01(a);
    else if (mode == 2) daNpc_Sarace_talk01(a);
    daNpc_Sarace_lookBack(a);
    daNpc_Sarace_setAttention(a);
    return TRUE;
}
VERIFY(0x022DD750, daNpc_Sarace_wait_action);

/* 022DD844 */
BOOL daNpc_Sarace_event_endCheck_action(daNpc_Sarace_c* a, void* prm) {
    WWHD_FUNC(0x022DD844, BOOL, a, prm);
    s8 status = a->mActionStatus;
    if (status == 0) { a->mActionStatus = 1; return TRUE; }
    if (status == -1) return TRUE;
    if (!gabi::call<BOOL>(0x0254457C, play() + 0x52C4, EVENT_NAME_END)) return TRUE;
    a->mEventMsgNo = 0xFA7;
    u32 pl = play();
    wr<u16>(pl, 0x52B8, rd<u16>(pl, 0x52B8) | 8);
    a->mEventOrder = 1;
    setAction(a, FN_wait_action);
    gabi::Local<u32> id0, id1;
    u32 buoy0 = 0, buoy1 = 0;
    u32 id = a->mBuoyId[0];
    gabi::store<u32>(gabi::ea(id0.get()), id);
    if (id != 0xFFFFFFFF) buoy0 = gabi::call<u32>(0x025D5218, 0x025E1234, id0.get());
    id = a->mBuoyId[1];
    gabi::store<u32>(gabi::ea(id1.get()), id);
    if (id != 0xFFFFFFFF) buoy1 = gabi::call<u32>(0x025D5218, 0x025E1234, id1.get());
    if (buoy0) wr<u8>(buoy0, 0x591, 1);
    if (buoy1) wr<u8>(buoy1, 0x591, 1);
    return TRUE;
}
VERIFY(0x022DD844, daNpc_Sarace_event_endCheck_action);

/* 022DDA2C */
void* daNpc_Sarace_HIO_ct(void* self) {
    WWHD_FUNC(0x022DDA2C, void*, self);
    u32 p = gabi::ea(self);
    if (!p) {
        p = gabi::call<u32>(0x0273AD10, 0x38);
        if (!p) return nullptr;
    }
    wr<u32>(p, 0x34, 0x10021980);
    gabi::call(0x0259DA18, p + 4);
    wr<s16>(p, 0xA, 0); wr<f32>(p, 4, -20.0f); wr<f32>(p, 0x24, 400.0f); wr<s16>(p, 0x14, -6000);
    wr<u8>(p, 0x22, 0); wr<s16>(p, 0x12, -2000); wr<s16>(p, 0x1A, 0x640); wr<s16>(p, 8, 4000);
    wr<s16>(p, 0xC, 6000); wr<s16>(p, 0x16, -7000); wr<s16>(p, 0x18, 1000); wr<s16>(p, 0x20, 0x4000);
    wr<f32>(p, 0x1C, 45.0f); wr<f32>(p, 0x30, 8.0f); wr<s16>(p, 0xE, 7000); wr<f32>(p, 0x2C, 11.0f);
    wr<s8>(p, 0, -1); wr<s16>(p, 0x10, -2000);
    return gabi::at<void>(p);
}
VERIFY(0x022DDA2C, daNpc_Sarace_HIO_ct);

/* 022DDB1C __sinit_d_a_npc_sarace_cpp */
void daNpc_Sarace_sinit() {
    WWHD_FUNC(0x022DDB1C, void);
    gabi::store<u32>(0x10468678, 0); gabi::store<u32>(0x10468670, 0);
    gabi::store<u32>(0x1046867C, 0); gabi::store<u32>(0x10468674, 0);
    gabi::call(0x028F026C, 0x101C6258);
    gabi::store<f32>(0x10468614, gabi::load<f32>(0x10021B20));
    gabi::store<f32>(0x10468618, gabi::load<f32>(0x10021B24));
    gabi::call(0x028ED6F8, 0x1046861C);
    gabi::call(0x028F026C, 0x101C6264);
    gabi::call(0x028EAB2C, 0x1046861D);
    gabi::call(0x028F026C, 0x101C6270);
    daNpc_Sarace_HIO_ct(gabi::at<void>(HIO));
}
VERIFY(0x022DDB1C, daNpc_Sarace_sinit);

/* 022DDBBC SafeString deleting destructor (this unit's copy) */
void daNpc_Sarace_SafeString_delete(void* self, u32 flags) {
    WWHD_FUNC(0x022DDBBC, void, self, flags);
    if (self && (flags & 1)) gabi::call(0x0273AF40, self);
}
VERIFY(0x022DDBBC, daNpc_Sarace_SafeString_delete);

/* 022DDBD0 */
BOOL daNpc_Sarace_IsDelete(daNpc_Sarace_c* a) {
    WWHD_FUNC(0x022DDBD0, BOOL, a);
    return TRUE;
}
VERIFY(0x022DDBD0, daNpc_Sarace_IsDelete);

/* 022DDBD8 daNpc_Sarace_c deleting destructor */
void daNpc_Sarace_dtor(daNpc_Sarace_c* a, u32 flags) {
    WWHD_FUNC(0x022DDBD8, void, a, flags);
    if (!a) return;
    u32 p = gabi::ea(a);
    gabi::call(0x02515A70, p + 0x690, 2);
    gabi::call(0x02515860, p + 0x654, 2);
    gabi::call(0x02018034, p + 0x628, 2);
    wr<u32>(p, 0x470, 0x10021960); wr<u32>(p, 0x464, 0x10021970);
    gabi::call(0x024EFD9C, p + 0x450, 0);
    gabi::call(0x025D50BC, a, 0);
    if (flags & 1) gabi::call(0x0273AF40, a);
}
VERIFY(0x022DDBD8, daNpc_Sarace_dtor);

/* 022DDC74 SafeString empty virtual (this unit's copy) */
void daNpc_Sarace_SafeString_empty(void* self) {
    WWHD_FUNC(0x022DDC74, void, self);
}
VERIFY(0x022DDC74, daNpc_Sarace_SafeString_empty);
