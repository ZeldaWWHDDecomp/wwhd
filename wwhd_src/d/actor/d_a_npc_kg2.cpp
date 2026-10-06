/**
 * d_a_npc_kg2.cpp (WWHD)
 * NPC - Salvatore (Cannon Minigame, Windfall)
 *
 * The GameCube decompilation of this unit is only
 * Nonmatching placeholders, so every function is written from the WWHD code (disassembly and
 * the recompiled C), keeping the GameCube names and member order.
 */
#include "d/actor/d_a_npc_kg2.h"
#include <cmath>

/* ---- local bindings (SHARED-CANDIDATE) ---- */
template <class T> static T rd(u32 p, u32 off) { return gabi::load<T>(p + off); }
template <class T> static void wr(u32 p, u32 off, T v) { gabi::store<T>(p + off, v); }
static u32 play() { return gabi::call<u32>(0x025200D4); }                 /* dComIfGp_get */
static u32 evtMng() { return play() + 0x52C4; }
static u32 saveInfo() { return gabi::load<u32>(0x101F84DC); }
static u32 envLight() { return gabi::call<u32>(0x02555D0C); }
static BOOL checkPass(u32 frameCtrl, f32 frame) { return gabi::call<BOOL>(0x027F2BF8, frameCtrl, frame); }
static u32 getEventReg(u32 save, u32 reg) { return gabi::call<u8>(0x025B8BB0, save, reg); }
static BOOL isEventBit(u32 save, u32 bit) { return gabi::call<BOOL>(0x025B8B94, save, bit); }
static void onEventBit(u32 save, u32 bit) { gabi::call(0x025B8B68, save, bit); }
static void* getMySubstanceP(s32 staff, u32 name, s32 type) { return gabi::call<void*>(0x0254487C, evtMng(), staff, name, type); }
static u32 objectRes(u32 name, s32 index) { return gabi::ea(dComIfG_getObjectRes(STR(name), index, 0x1001BF7C)); }
static void copyWords(u32 dst, u32 src, int n) {
    u32 v[12];
    for (int i = 0; i < n; ++i) v[i] = gabi::load<u32>(src + 4 * i);
    for (int i = 0; i < n; ++i) gabi::store<u32>(dst + 4 * i, v[i]);
}
/* lfs/stfs copy (matrices): float stores, so the harness's NaN payload rule applies */
static void copyFloats(u32 dst, u32 src, int n) {
    f32 v[12];
    for (int i = 0; i < n; ++i) v[i] = gabi::load<f32>(src + 4 * i);
    for (int i = 0; i < n; ++i) gabi::store<f32>(dst + 4 * i, v[i]);
}
/* J3DModel joint matrix i (marks the matrix block dirty) */
static u32 jointMtx(u32 model, u32 offs) {
    u32 block = rd<u32>(model, 0x2C);
    u16 flags = rd<u16>(block, 4);
    u32 base = rd<u32>(block, 0x10);
    wr<u16>(block, 4, flags | 0x10);
    return base + offs;
}
static f32 endFrameMinus1(u32 morf) { return (f32)rd<s16>(morf, 0xA2) - 1.0f; }

constexpr u32 HIO = 0x10467628;             /* l_HIO */
constexpr u32 MTX = 0x1048D0CC;             /* mDoMtx_stack_c::now */
constexpr u32 l_kg2_pointer = 0x101D5F24;
constexpr u32 canon_game_result = 0x101D5F42;
constexpr u32 ACT_wait = 0x0226A474, ACT_event_wait = 0x0226A574;

/* daNpc_Kg2_c::setAction (inline): GHS pointer-to-member compare, old action called with -1 state */
static void setAction(daNpc_Kg2_c* a, u32 fn) {
    s16 i = a->mAction.i;
    if (i == -1) {
        if (a->mAction.d == 0 && a->mAction.f == fn) return;
    } else if (i == 0) {
        a->mActionState = 0; a->mAction.f = fn; a->mAction.d = 0; a->mAction.i = -1;
        gabi::call_ptr(a->mAction.f, a, 0);
        return;
    }
    s16 d = a->mAction.d; i = a->mAction.i;
    u32 self = gabi::ea(a) + (s32)d;
    a->mActionState = -1;
    if (i < 0) gabi::call_ptr(a->mAction.f, self, 0);
    else {
        u32 vtbl = rd<u32>(self, (u32)(s32)rd<s16>(gabi::ea(a), 0x916));
        gabi::call_ptr(rd<u32>(vtbl, (u32)(s32)i * 8 + 4), self, 0);
    }
    a->mAction.d = 0; a->mAction.i = -1; a->mAction.f = fn; a->mActionState = 0;
    gabi::call_ptr(a->mAction.f, a, 0);
}

/* 0226A8B8 */
daNpc_Kg2_HIO_c* daNpc_Kg2_HIO_ct(daNpc_Kg2_HIO_c* self) {
    WWHD_FUNC(0x0226A8B8, daNpc_Kg2_HIO_c*, self);
    u32 p = gabi::ea(self);
    if (!p) {
        p = gabi::call<u32>(0x0273AD10, 0x34);
        if (!p) return nullptr;
    }
    wr<u32>(p, 0x30, 0x1001BFB4);
    gabi::call(0x0259DA18, p + 4);
    wr<s16>(p, 8, 2500); wr<s16>(p, 0xE, 8000); wr<s16>(p, 0x12, -2000); wr<s16>(p, 0x10, -2500);
    wr<s16>(p, 0xA, 2000); wr<f32>(p, 0x1C, 35.0f); wr<u8>(p, 0x2C, 0); wr<s16>(p, 0x14, -7000);
    wr<s16>(p, 0x18, 0x1000); wr<s16>(p, 0x1A, 1000); wr<s8>(p, 0, -1); wr<s16>(p, 0xC, 7000);
    wr<f32>(p, 0x24, 400.0f); wr<u8>(p, 0x22, 0); wr<f32>(p, 4, -20.0f); wr<s16>(p, 0x20, 0x4000);
    wr<s16>(p, 0x16, -8000);
    return gabi::at<daNpc_Kg2_HIO_c>(p);
}
VERIFY(0x0226A8B8, daNpc_Kg2_HIO_ct);

/* 02267B60 */
BOOL daNpc_Kg2_nodeCallBack(J3DNode* node, s32 timing) {
    WWHD_FUNC(0x02267B60, BOOL, node, timing);
    if (timing != 0) return TRUE;
    u32 model = gabi::load<u32>(0x104B462C);   /* j3dSys model */
    u32 actor = rd<u32>(model, 0xB8);
    if (!actor) return TRUE;
    u32 jnt = gabi::call<u32>(0x027F7878, node);
    u32 jntNo = rd<u16>(jnt, 4);
    u32 offs = jntNo * 0x30;
    gabi::call(0x028E90D4, jointMtx(model, offs), MTX);          /* PSMTXCopy */
    if (jntNo == (u32)(s32)rd<s8>(actor, 0x3B4)) {
        constexpr u32 headOffs = 0x1046765C, headGuard = 0x10467668;
        f32 gx = gabi::load<f32>(0x1047BBB0), gy = gabi::load<f32>(0x1047BBB4);
        u32 gz = gabi::load<u32>(0x1047BBB8);
        gabi::Local<cXyz> eyeOffs;
        gabi::store<u32>(gabi::ea(eyeOffs.get()) + 8, gz);
        eyeOffs->x = gabi::fadds_ppc(gx, 24.0f);
        eyeOffs->y = gabi::fadds_ppc(gy, 5.0f);
        if (gabi::load<u32>(headGuard) == 0) {
            gabi::store<u32>(headGuard, 1);
            gabi::store<f32>(headOffs + 8, 0.0f); gabi::store<f32>(headOffs, 24.0f); gabi::store<f32>(headOffs + 4, -16.0f);
        }
        gabi::call(0x028E8F64, MTX, eyeOffs.get(), actor + 0x8E8);  /* PSMTXMultVec */
        gabi::call(0x025F1BF4, MTX, rd<s16>(actor, 0x3AE));          /* mDoMtx_XrotM */
        gabi::call(0x025F1C5C, MTX, (s16)-rd<s16>(actor, 0x3AC));    /* mDoMtx_ZrotM */
        gabi::call(0x028E8F64, MTX, headOffs, actor + 0x8DC);
        gabi::call(0x028E8F64, MTX, eyeOffs.get(), actor + 0x390);
        wr<f32>(actor, 0x394, gabi::fadds_ppc(rd<f32>(actor, 0x394), gabi::load<f32>(HIO + 0x1C)));
    } else if (jntNo == (u32)(s32)rd<s8>(actor, 0x3B5)) {
        gabi::call(0x025F1BF4, MTX, rd<s16>(actor, 0x3B2));
        gabi::call(0x025F1C5C, MTX, (s16)-rd<s16>(actor, 0x3B0));
    }
    gabi::call(0x028E90D4, MTX, 0x104B4868);
    copyFloats(jointMtx(model, offs), MTX, 12);
    return TRUE;
}
VERIFY(0x02267B60, daNpc_Kg2_nodeCallBack);

/* 02267E04 */
BOOL daNpc_Kg2_initTexPatternAnm(daNpc_Kg2_c* a, u32 modify) {
    WWHD_FUNC(0x02267E04, BOOL, a, modify);
    s32 idx = a->mBtpIdx;
    u32 modelData = rd<u32>(rd<u32>(gabi::ea(a->mpMorf.get()), 0x90), 0xAC);
    s32 res = gabi::load<s32>(0x1001C000 + 4 * idx);
    u32 btp = objectRes(0x1001C028, res);
    a->mpBtp = gabi::at<J3DAnmTexPattern>(btp);
    if (!btp) {
        gabi::call(0x0273AA24, STR(0x1001C02C), 0x12E, STR(0x1001C03C));
        btp = gabi::ea(a->mpBtp.get());
    }
    if (!gabi::call<BOOL>(0x025E789C, a->mBtpAnm, modelData, btp, 1, 2, 1.0f, 0, -1, (u32)modify, 0))
        return FALSE;
    a->mBtpFrame = 0; a->mBlinkTimer = 0;
    return TRUE;
}
VERIFY(0x02267E04, daNpc_Kg2_initTexPatternAnm);

static u32 jointNameTable(u32 modelData) {
    u32 header = gabi::call<u32>(0x027F68FC, modelData);
    u32 offset = rd<u32>(header, 0x10);
    return offset ? header + 0x10 + offset : 0;
}

/* 02267F18 */
BOOL daNpc_Kg2_CreateHeap(daNpc_Kg2_c* a) {
    WWHD_FUNC(0x02267F18, BOOL, a);
    u32 p = gabi::ea(a);
    u32 data = objectRes(0x1001C058, 5);
    if (!data) gabi::call(0x0273AA24, STR(0x1001C064), 0x395, STR(0x1001C080));
    u32 anm = objectRes(0x1001C058, 0x1A);
    u32 morf = gabi::call<u32>(0x025E4F64, 0, data, 0, 0, anm, 2, 0, -1, 1.0f, 1, 0, 0, 0x11020203);
    wr<u32>(p, 0x44C, morf);
    if (!morf || !rd<u32>(morf, 0x90)) return FALSE;
    s8 jnt = gabi::call<s8>(0x027DF9B0, jointNameTable(data), STR(0x1001C05C));
    wr<s8>(p, 0x3B4, jnt);
    if (jnt < 0) gabi::call(0x0273AA24, STR(0x1001C064), 0x3A6, STR(0x1001C094));
    jnt = gabi::call<s8>(0x027DF9B0, jointNameTable(data), STR(0x1001C074));
    wr<s8>(p, 0x3B5, jnt);
    if (jnt < 0) gabi::call(0x0273AA24, STR(0x1001C064), 0x3AB, STR(0x1001C0B0));
    jnt = gabi::call<s8>(0x027DF9B0, jointNameTable(data), STR(0x1001C050));
    a->mItemJntNum = jnt;
    if (jnt < 0) gabi::call(0x0273AA24, STR(0x1001C064), 0x3AF, STR(0x1001C0D0));
    a->mBtpIdx = 0;
    if (!daNpc_Kg2_initTexPatternAnm(a, 0)) return FALSE;
    u32 itemData = objectRes(0x1001C058, 6);
    u32 item = gabi::call<u32>(0x025E38E0, itemData, 0x80000, 0x11020002);   /* mDoExt_J3DModel__create */
    a->mpItemModel = gabi::at<J3DModel>(item);
    if (!item) return FALSE;
    u32 itemBtp = objectRes(0x1001C058, 10);
    if (!gabi::call<BOOL>(0x025E789C, a->mItemBtpAnm, itemData, itemBtp, 1, 2, 0.0f, 0, -1, 0, 0)) return FALSE;
    u32 modelData = rd<u32>(rd<u32>(rd<u32>(p, 0x44C), 0x90), 0xAC);
    u32 idx = (u16)(s32)rd<s8>(p, 0x3B4);
    u32 joints = rd<u32>(modelData, 8);
    if (idx < rd<u32>(modelData, 4)) joints += idx * 0x1C;
    wr<u32>(joints, 8, 0x02267B60);
    idx = (u16)(s32)rd<s8>(p, 0x3B5);
    joints = rd<u32>(modelData, 8);
    if (idx < rd<u32>(modelData, 4)) joints += idx * 0x1C;
    wr<u32>(joints, 8, 0x02267B60);
    wr<u32>(rd<u32>(rd<u32>(p, 0x44C), 0x90), 0xB8, p);
    gabi::call(0x024EFF44, p + 0x614, 30.0f, 0.0f);              /* dBgS_AcchCir::SetWall */
    gabi::call(0x024F06B4, p + 0x450, p + 0x314, p + 0x300, a, 1, p + 0x614, p + 0x33C, 0, 0); /* dBgS_Acch::Set */
    return TRUE;
}
VERIFY(0x02267F18, daNpc_Kg2_CreateHeap);

/* 02268290 */
BOOL daNpc_Kg2_CallbackCreateHeap(daNpc_Kg2_c* a) {
    WWHD_FUNC(0x02268290, BOOL, a);
    return daNpc_Kg2_CreateHeap(a);
}
VERIFY(0x02268290, daNpc_Kg2_CallbackCreateHeap);

/* 02268294 */
void daNpc_Kg2_set_mtx(daNpc_Kg2_c* a) {
    WWHD_FUNC(0x02268294, void, a);
    u32 p = gabi::ea(a);
    u32 model = rd<u32>(rd<u32>(p, 0x44C), 0x90);
    gabi::call(0x028E93CC, MTX, rd<f32>(p, 0x314), rd<f32>(p, 0x318), rd<f32>(p, 0x31C));   /* PSMTXTrans */
    gabi::call(0x025F1C28, MTX, rd<s16>(p, 0x322));                                         /* mDoMtx_YrotM */
    copyFloats(model + 0xC8, MTX, 12);
    gabi::call(0x025E55A0, rd<u32>(p, 0x44C));                                              /* McaMorf::calc */
    if (a->mbItemShown) {
        s32 jnt = a->mItemJntNum;
        gabi::call(0x028E90D4, jointMtx(model, (u32)(jnt * 0x30)), MTX);
        gabi::call(0x025F24E0, 23.46f, -22.26f, -47.05f);                                     /* transM */
        gabi::call(0x025F19F8, MTX, (s16)0x1F4B, (s16)-0x4F00, (s16)0x1F4B);                  /* XYZrotM */
        copyFloats(gabi::ea(a->mpItemModel.get()) + 0xC8, MTX, 12);
    }
}
VERIFY(0x02268294, daNpc_Kg2_set_mtx);

/* 02268430 */
void daNpc_Kg2_setAnm(daNpc_Kg2_c* a, s8 anm, f32 morf) {
    WWHD_FUNC(0x02268430, void, a, anm, morf);
    s32 idx = anm;
    if (morf < 0.0f) morf = gabi::load<f32>(0x101BF1CC + 4 * idx);
    s32 cur = a->mAnmIdx;
    if ((u32)idx != (u32)cur && cur != -1) {
        a->mAnmIdx = anm;
        gabi::call(0x0259D454, a->mpMorf.get(), gabi::load<s32>(0x101BF190 + 4 * idx), morf,
                   gabi::load<f32>(0x101BF208 + 4 * idx), gabi::load<s32>(0x1001BFC4 + 4 * idx), -1,
                   STR(0x1001C0FC));                                                      /* dNpc_setAnm */
        cur = a->mAnmIdx;
        if (cur == 12) {
            u32 m = gabi::ea(a->mpMorf.get());
            s16 end = (s16)gabi::ftoi(endFrameMinus1(m));
            wr<f32>(m, 0x9C, (f32)end);
            wr<f32>(gabi::ea(a->mpMorf.get()), 0x98, -1.0f);
            cur = a->mAnmIdx;
        }
    }
    wr<u8>(gabi::ea(a), 0x3B7, cur == 2 ? 0 : 1);
    wr<u8>(gabi::ea(a), 0x3B8, 1);
}
VERIFY(0x02268430, daNpc_Kg2_setAnm);

/* 0226856C */
BOOL daNpc_Kg2_CreateInit(daNpc_Kg2_c* a) {
    WWHD_FUNC(0x0226856C, BOOL, a);
    u32 p = gabi::ea(a);
    wr<f32>(p, 0x374, -30.0f);
    a->mInitAngle[1] = rd<u16>(p, 0x322);
    a->mInitAngle[0] = rd<u16>(p, 0x320);
    a->mInitAngle[2] = rd<u16>(p, 0x324);
    wr<u8>(p, 0x389, 0x6E); wr<u32>(p, 0x39C, 10); wr<u8>(p, 0x38B, 0x6E);
    setAction(a, ACT_wait);
    copyWords(p + 0x8DC, p + 0x314, 3);
    gabi::call(0x02515F14, p + 0x654, 0xFF, 0xFF, a);      /* dCcD_Stts::Init */
    gabi::call(0x02516518, p + 0x690, 0x101BF14C);         /* dCcD_Cyl::Set (l_cyl_src) */
    wr<u32>(p, 0x6D4, p + 0x654);
    a->setCollision(60.0f, 150.0f);
    gabi::call(0x0259F814, p + 0x3E0, STR(0x1001C114), a); /* setActorInfo2 */
    a->mbTicketBought = 0; a->mbItemShown = 0; wr<u32>(p, 0x448, p + 0x3AC);
    a->mNextMsgNo = 0; a->mEventOrder = 0; a->mbGreeted = 0;
    wr<u8>(p, 0x3B8, 1); a->mItemBtpFrame = 0; wr<u8>(p, 0x3B7, 1); a->mEventNo = 4;
    a->mEventIds[0] = gabi::call<s16>(0x02543F10, evtMng(), STR(0x1001C124), 0xFF);
    a->mEventIds[1] = gabi::call<s16>(0x02543F10, evtMng(), STR(0x1001C134), 0xFF);
    a->mEventIds[2] = gabi::call<s16>(0x02543F10, evtMng(), STR(0x1001C118), 0xFF);
    a->mEventIds[3] = gabi::call<s16>(0x02543F10, evtMng(), STR(0x1001C144), 0xFF);
    daNpc_Kg2_set_mtx(a);
    f32 time = rd<f32>(saveInfo(), 0x44);
    if (time < 105.0f || !(time < 300.0f)) {
        a->mbNight = 1;
        daNpc_Kg2_setAnm(a, 13, 0.0f);
    } else {
        a->mbNight = 0;
        daNpc_Kg2_setAnm(a, 1, -1.0f);
    }
    gabi::store<u32>(l_kg2_pointer, p);
    return TRUE;
}
VERIFY(0x0226856C, daNpc_Kg2_CreateInit);

/* 02268864 */
cPhs_State daNpc_Kg2_create(daNpc_Kg2_c* a) {
    WWHD_FUNC(0x02268864, cPhs_State, a);
    u32 p = gabi::ea(a);
    u32 flags = rd<u32>(p, 0x2E4);
    if (!(flags & 8)) {
        if (a) {
            gabi::call(0x025A1458, a);                 /* fopNpc_npc_c::fopNpc_npc_c */
            wr<u32>(p, 0xB4, 0x1001C1E8);
            gabi::call(0x025E7820, a->mItemBtpAnm);    /* mDoExt_btpAnm::mDoExt_btpAnm */
            gabi::call(0x025E7820, a->mBtpAnm);
            flags = rd<u32>(p, 0x2E4);
        }
        wr<u32>(p, 0x2E4, flags | 8);
    }
    cPhs_State phase = dComIfG_resLoad(&a->mPhs, STR(0x1001C150));
    if (phase != cPhs_COMPLEATE_e) return phase;
    if (!gabi::call<BOOL>(0x025D63E8, a, 0x02268290, 0x2D00)) return cPhs_ERROR_e;
    u32 model = rd<u32>(rd<u32>(p, 0x44C), 0x90);
    wr<u32>(p, 0x348, model ? model + 0xC8 : 0);
    if (gabi::load<s8>(HIO) < 0) gabi::store<s8>(HIO, gabi::call<s8>(0x025F0A10, STR(0x1001C154), HIO));
    if (!daNpc_Kg2_CreateInit(a)) return cPhs_ERROR_e;
    return phase;
}
VERIFY(0x02268864, daNpc_Kg2_create);

/* 02268988 */
cPhs_State daNpc_Kg2_Create(daNpc_Kg2_c* a) { WWHD_FUNC(0x02268988, cPhs_State, a); return daNpc_Kg2_create(a); }
VERIFY(0x02268988, daNpc_Kg2_Create);

/* 0226898C */
BOOL daNpc_Kg2_delete(daNpc_Kg2_c* a) {
    WWHD_FUNC(0x0226898C, BOOL, a);
    u32 p = gabi::ea(a);
    dComIfG_resDelete(&a->mPhs, STR(0x1001C168));
    if (rd<u32>(p, 0xF4)) {
        u32 morf = rd<u32>(p, 0x44C);
        if (morf) gabi::call(0x025E563C, morf);       /* McaMorf::stopZelAnime */
    }
    s8 no = gabi::load<s8>(HIO);
    gabi::store<u32>(l_kg2_pointer, 0);
    if (no >= 0) {
        gabi::call(0x025F0A18, no);                   /* mDoHIO_deleteChild */
        gabi::store<s8>(HIO, -1);
    }
    return TRUE;
}
VERIFY(0x0226898C, daNpc_Kg2_delete);

/* 02268A0C */
BOOL daNpc_Kg2_Delete(daNpc_Kg2_c* a) { WWHD_FUNC(0x02268A0C, BOOL, a); return daNpc_Kg2_delete(a); }
VERIFY(0x02268A0C, daNpc_Kg2_Delete);

/* 02268A10 */
void daNpc_Kg2_playTexPatternAnm(daNpc_Kg2_c* a) {
    WWHD_FUNC(0x02268A10, void, a);
    if (a->mbNight == 1 && a->mBtpIdx == 0 && a->mBtpFrame == 2) {
        a->mBtpFrame = 0; a->mBtpIdx = 4;
        daNpc_Kg2_initTexPatternAnm(a, 1);
        return;
    }
    if (gabi::call<s16>(0x02055B64, &a->mBlinkTimer) != 0) return;   /* cLib_calcTimer */
    if (a->mBtpIdx == 4) {
        if (a->mBtpFrame == 0) {
            a->mBtpFrame = 1;
            f32 r = gabi::call<f32>(0x020198D8, 150.0f);                  /* cM_rndF */
            a->mBlinkTimer = (s16)gabi::ftoi(r + 150.0f);
        } else {
            a->mBtpFrame = 0;
            f32 r = gabi::call<f32>(0x020198D8, 150.0f);
            a->mBlinkTimer = (s16)((s16)gabi::ftoi(r + 150.0f) * 2);
        }
        return;
    }
    u32 btp = gabi::ea(a->mpBtp.get());
    s32 max = gabi::call_ptr<s32>(rd<u32>(rd<u32>(btp, 4), 0x14), btp);
    if ((s32)a->mBtpFrame >= max) {
        btp = gabi::ea(a->mpBtp.get());
        max = gabi::call_ptr<s32>(rd<u32>(rd<u32>(btp, 4), 0x14), btp);
        a->mBtpFrame = (u8)(a->mBtpFrame - max);
        f32 r = gabi::call<f32>(0x020198D8, 100.0f);
        a->mBlinkTimer = (s16)gabi::ftoi(r + 30.0f);
    } else {
        a->mBtpFrame = (u8)(a->mBtpFrame + 1);
    }
}
VERIFY(0x02268A10, daNpc_Kg2_playTexPatternAnm);

/* 02268C20 */
void daNpc_Kg2_checkOrder(daNpc_Kg2_c* a) {
    WWHD_FUNC(0x02268C20, void, a);
    u16 cond = rd<u16>(gabi::ea(a), 0xF8);
    if (cond == 2) {
        setAction(a, ACT_event_wait);
        a->mEventOrder = 0;
    } else if (cond == 1) {
        s8 order = a->mEventOrder;
        if (order == 1 || order == 2) {
            a->mEventOrder = 0;
            a->mbTalkRequest = 1;
        }
    }
}
VERIFY(0x02268C20, daNpc_Kg2_checkOrder);

/* 02268DA0 */
BOOL daNpc_Kg2_evn_setAnm_init(daNpc_Kg2_c* a, s32 staff) {
    WWHD_FUNC(0x02268DA0, BOOL, a, staff);
    u32 anm = gabi::ea(getMySubstanceP(staff, 0x1001C170, 3));
    u32 loop = gabi::ea(getMySubstanceP(staff, 0x1001C178, 3));
    u32 morf = gabi::ea(getMySubstanceP(staff, 0x1001C180, 0));
    if (anm) {
        f32 m = -1.0f;
        if (morf) m = gabi::load<f32>(morf);
        daNpc_Kg2_setAnm(a, rd<s8>(anm, 3), m);
        a->mEvnLoopCount = loop ? rd<s8>(loop, 3) : 0;
    }
    return TRUE;
}
VERIFY(0x02268DA0, daNpc_Kg2_evn_setAnm_init);

/* 02268E88 */
BOOL daNpc_Kg2_evn_jnt_lock_init(daNpc_Kg2_c* a, s32 staff) {
    WWHD_FUNC(0x02268E88, BOOL, a, staff);
    u32 v = gabi::ea(getMySubstanceP(staff, 0x1001C188, 3));
    u32 lock = v ? gabi::load<u32>(v) : 0;
    u32 p = gabi::ea(a);
    if (lock < 2) { wr<u8>(p, 0x3B7, (u8)lock); wr<u8>(p, 0x3B8, 0); }
    else if (lock == 2) { wr<u8>(p, 0x3B7, 0); wr<u8>(p, 0x3B8, 1); }
    else if (lock == 3) { wr<u8>(p, 0x3B7, 1); wr<u8>(p, 0x3B8, 1); }
    return TRUE;
}
VERIFY(0x02268E88, daNpc_Kg2_evn_jnt_lock_init);

/* 02268F38 */
BOOL daNpc_Kg2_evn_talk_init(daNpc_Kg2_c* a, s32 staff) {
    WWHD_FUNC(0x02268F38, BOOL, a, staff);
    u32 v = gabi::ea(getMySubstanceP(staff, 0x1001C18C, 3));
    a->mbHasMsg = 0;
    a->mCurrMsgBsPcId = 0xFFFFFFFF;
    a->mNextMsgNo = v ? gabi::load<u32>(v) : 0;
    return TRUE;
}
VERIFY(0x02268F38, daNpc_Kg2_evn_talk_init);

/* 02268FAC */
BOOL daNpc_Kg2_evn_createItem_init(daNpc_Kg2_c* a, s32 staff) {
    WWHD_FUNC(0x02268FAC, BOOL, a, staff);
    u32 p = gabi::ea(a);
    u32 count = getEventReg(saveInfo() + 0x644, 0xB703);
    s32 id;
    if (count <= 2) {
        u8 item = gabi::load<u8>(0x1001C194 + count);
        id = gabi::call<s32>(0x025D7DEC, p + 0x314, item, 0, -1, (s32)rd<s8>(p, 0x326), 0, 0);
    } else {
        id = gabi::call<s32>(0x025D7DEC, p + 0x314, 6, 0, -1, (s32)rd<s8>(p, 0x326), 0, 0);
    }
    if (id != -1) wr<s32>(play(), 0x52A0, id);
    return TRUE;
}
VERIFY(0x02268FAC, daNpc_Kg2_evn_createItem_init);

/* 02269068 */
BOOL daNpc_Kg2_evn_setAnm(daNpc_Kg2_c* a) {
    WWHD_FUNC(0x02269068, BOOL, a);
    u32 morf = gabi::ea(a->mpMorf.get());
    BOOL passed;
    if (a->mAnmIdx == 12) passed = checkPass(morf + 0x98, 1.0f);
    else passed = checkPass(morf + 0x98, endFrameMinus1(morf));
    if (passed) {
        s8 n = (s8)(a->mEvnLoopCount - 1);
        a->mEvnLoopCount = n;
        return n <= 0;
    }
    return a->mEvnLoopCount <= 0;
}
VERIFY(0x02269068, daNpc_Kg2_evn_setAnm);

/* 02269128 */
BOOL daNpc_Kg2_evn_talk(daNpc_Kg2_c* a) {
    WWHD_FUNC(0x02269128, BOOL, a);
    return a->talk(1) == 0x12;
}
VERIFY(0x02269128, daNpc_Kg2_evn_talk);

/* 02269158 */
BOOL daNpc_Kg2_privateCut(daNpc_Kg2_c* a) {
    WWHD_FUNC(0x02269158, BOOL, a);
    u32 name = rd<u32>(gabi::ea(a), 0x3E0);
    s32 staff = gabi::call<s32>(0x02542D88, evtMng(), name, 0, 0);      /* getMyStaffId */
    if (staff == -1) return FALSE;
    s32 act = gabi::call<s32>(0x02542EDC, evtMng(), staff, 0x101BF244, 5, 1, 0);   /* getMyActIdx */
    u32 ev = evtMng();
    if (act == -1) {
        gabi::call(0x02543280, ev, staff);                                  /* cutEnd */
        return TRUE;
    }
    if (gabi::call<BOOL>(0x025447C8, ev, staff)) {                          /* getIsAddvance */
        switch ((u32)act) {
        case 0: daNpc_Kg2_evn_setAnm_init(a, staff); break;
        case 1: daNpc_Kg2_evn_jnt_lock_init(a, staff); break;
        case 3: daNpc_Kg2_evn_talk_init(a, staff); break;
        case 4: daNpc_Kg2_evn_createItem_init(a, staff); break;
        }
    }
    switch ((u32)act) {
    case 0: if (!daNpc_Kg2_evn_setAnm(a)) return TRUE; break;
    case 3: if (!daNpc_Kg2_evn_talk(a)) return TRUE; break;
    }
    gabi::call(0x02543280, evtMng(), staff);
    return TRUE;
}
VERIFY(0x02269158, daNpc_Kg2_privateCut);

/* 022692EC */
BOOL daNpc_Kg2_processMove(daNpc_Kg2_c* a) {
    WWHD_FUNC(0x022692EC, BOOL, a);
    s16 i = a->mAction.i;
    u32 self = gabi::ea(a) + (s32)(s16)a->mAction.d;
    if (i < 0) gabi::call_ptr(a->mAction.f, self, 0);
    else {
        u32 vtbl = rd<u32>(self, (u32)(s32)rd<s16>(gabi::ea(a), 0x916));
        gabi::call_ptr(rd<u32>(vtbl, (u32)(s32)i * 8 + 4), self, 0);
    }
    if (a->mEventCut.cutProc()) return TRUE;
    return daNpc_Kg2_privateCut(a) ? TRUE : FALSE;
}
VERIFY(0x022692EC, daNpc_Kg2_processMove);

/* 022693A8 */
void daNpc_Kg2_eventOrder(daNpc_Kg2_c* a) {
    WWHD_FUNC(0x022693A8, void, a);
    u32 p = gabi::ea(a);
    s8 order = a->mEventOrder;
    if (order == 1 || order == 2) {
        order = a->mEventOrder;
        wr<u16>(p, 0xFA, rd<u16>(p, 0xFA) | 1);
        if (order == 1) gabi::call(0x025D76A8, a);    /* fopAcM_orderSpeakEvent */
    } else if (order == 3) {
        s16 id = a->mEventIds[(s32)a->mEventNo];
        gabi::call(0x025D7A58, a, id, 0xFF, 0xFFFF, 0, 1);   /* fopAcM_orderOtherEventId */
    }
}
VERIFY(0x022693A8, daNpc_Kg2_eventOrder);

/* 02269410 */
void daNpc_Kg2_subAnm(daNpc_Kg2_c* a) {
    WWHD_FUNC(0x02269410, void, a);
    u32 p = gabi::ea(a);
    if (a->mBtpIdx == 4 && p != 0 && p + 0x37C != 0) {
        s32 reverb = gabi::call<s32>(0x02520540, (s32)rd<s8>(p, 0x326));   /* dComIfGp_getReverb */
        gabi::call(0x025E1A40, 0x4114, p + 0x37C, 0, reverb);                /* seStart */
    }
    s8 anm = a->mAnmIdx;
    if (anm == 3 || anm == 13) {
        if (checkPass(rd<u32>(p, 0x44C) + 0x98, 1.0f)) {
            s8 cur = a->mAnmIdx;
            a->mbItemShown = 1;
            a->mItemBtpFrame = cur == 3 ? 3 : 4;
            return;
        }
        anm = a->mAnmIdx;
    }
    if (anm == 9) {
        if (checkPass(rd<u32>(p, 0x44C) + 0x98, 30.0f)) {
            a->mbItemShown = 1; a->mItemBtpFrame = 2;
            return;
        }
        anm = a->mAnmIdx;
    }
    if (anm == 12) {
        if (checkPass(rd<u32>(p, 0x44C) + 0x98, 1.0f)) {
            a->mbItemShown = 0;
            daNpc_Kg2_setAnm(a, 1, -1.0f);
            return;
        }
        anm = a->mAnmIdx;
    }
    if (anm == 13) {
        u32 morf = rd<u32>(p, 0x44C);
        if (checkPass(morf + 0x98, endFrameMinus1(morf))) daNpc_Kg2_setAnm(a, 4, -1.0f);
    }
}
VERIFY(0x02269410, daNpc_Kg2_subAnm);

/* 02269690 */
BOOL daNpc_Kg2_execute(daNpc_Kg2_c* a) {
    WWHD_FUNC(0x02269690, BOOL, a);
    u32 p = gabi::ea(a);
    s16 prm[9];
    for (int i = 0; i < 9; ++i) prm[i] = gabi::load<s16>(HIO + 8 + 2 * i);
    gabi::call(0x0259E08C, p + 0x3AC, prm[1], prm[3], prm[5], prm[7], prm[0], prm[2], prm[4], prm[6], prm[8]);
    daNpc_Kg2_playTexPatternAnm(a);
    gabi::call(0x025E535C, rd<u32>(p, 0x44C), p + 0x37C, 0, 0);       /* McaMorf::play */
    gabi::call(0x025E55A0, rd<u32>(p, 0x44C));                        /* McaMorf::calc */
    daNpc_Kg2_checkOrder(a);
    daNpc_Kg2_processMove(a);
    daNpc_Kg2_eventOrder(a);
    daNpc_Kg2_subAnm(a);
    gabi::call(0x025D6870, a, p + 0x654);                             /* fopAcM_posMoveF */
    gabi::call(0x024F08A8, p + 0x450, play() + 0x12A0);               /* dBgS_Acch::CrrPos */
    wr<u8>(p, 0x1C9, gabi::call<u8>(0x024EF130, play() + 0x12A0, p + 0x538));   /* dBgS::GetRoomId */
    wr<u8>(p, 0x1CA, gabi::call<u8>(0x024EEEB8, play() + 0x12A0, p + 0x538));   /* dBgS::GetPolyColor */
    daNpc_Kg2_set_mtx(a);
    a->setCollision(60.0f, 150.0f);
    return TRUE;
}
VERIFY(0x02269690, daNpc_Kg2_execute);

/* 022697A0 */
BOOL daNpc_Kg2_Execute(daNpc_Kg2_c* a) { WWHD_FUNC(0x022697A0, BOOL, a); return daNpc_Kg2_execute(a); }
VERIFY(0x022697A0, daNpc_Kg2_Execute);

/* 022697A4 */
BOOL daNpc_Kg2_draw(daNpc_Kg2_c* a) {
    WWHD_FUNC(0x022697A4, BOOL, a);
    u32 p = gabi::ea(a);
    u32 model = rd<u32>(rd<u32>(p, 0x44C), 0x90);
    u32 data = rd<u32>(model, 0xAC);
    gabi::call(0x025626A4, envLight(), 0, p + 0x314, p + 0x110);     /* settingTevStruct */
    gabi::call(0x02562F5C, envLight(), model, p + 0x110);            /* setLightTevColorType */
    gabi::call(0x025E7B3C, a->mBtpAnm, data, (u32)a->mBtpFrame);    /* mDoExt_btpAnm::entry */
    gabi::call(0x025E5590, rd<u32>(p, 0x44C));                       /* McaMorf::entryDL */
    wr<u32>(data, 0x38, 0);
    if (a->mbItemShown) {
        u32 itemData = rd<u32>(gabi::ea(a->mpItemModel.get()), 0xAC);
        u32 env = envLight();
        J3DModel* item = a->mpItemModel.get();
        gabi::call(0x02562F5C, env, item, p + 0x110);
        gabi::call(0x025E7B3C, a->mItemBtpAnm, itemData, (u32)a->mItemBtpFrame);
        gabi::call(0x025E2DE0, a->mpItemModel.get(), 0);              /* mDoExt_modelUpdateDL */
        wr<u32>(itemData, 0x38, 0);
    }
    gabi::call(0x025BEBB8, 0x81, a, p + 0x314, rd<s16>(p, 0x322), 1.0f, 1.0f, 1.0f);   /* dSnap_RegistFig */
    return TRUE;
}
VERIFY(0x022697A4, daNpc_Kg2_draw);

/* 022698A0 */
BOOL daNpc_Kg2_Draw(daNpc_Kg2_c* a) { WWHD_FUNC(0x022698A0, BOOL, a); return daNpc_Kg2_draw(a); }
VERIFY(0x022698A0, daNpc_Kg2_Draw);

/* 022698A4 */
u8 daNpc_Kg2_chkAttention(daNpc_Kg2_c* a, cXyz* pos, s16 angle) {
    WWHD_FUNC(0x022698A4, u8, a, pos, angle);
    u32 player = rd<u32>(play(), 0x5B2C);
    f32 dz = rd<f32>(player, 0x31C) - pos->z;
    f32 dx = rd<f32>(player, 0x314) - pos->x;
    f32 radius = gabi::load<f32>(HIO + 0x24);
    s32 limit = gabi::load<s16>(HIO + 0x20);
    f32 dist = gabi::call<f32>(0x028F4384, gabi::fmadds(dx, dx, dz * dz));   /* sqrtf */
    s16 target = gabi::call<s16>(0x020195B0, dx, dz);                         /* cM_atan2s */
    if (a->mbAttention) {
        radius = radius + 40.0f;
        limit += 0x71C;
    }
    s16 diff = (s16)(target - angle);
    s32 mag = diff < 0 ? -(s32)diff : diff;
    return limit > mag && radius > dist;
}
VERIFY(0x022698A4, daNpc_Kg2_chkAttention);

/* 022699D0 */
u16 daNpc_Kg2_next_msgStatus(daNpc_Kg2_c* a, be<u32>* msg) {
    WWHD_FUNC(0x022699D0, u16, a, msg);
    u32 cur = *msg;
    u32 msgMng = gabi::load<u32>(0x101F4B5C);
    switch (cur) {
    case 0x3139:
        if (getEventReg(saveInfo() + 0x644, 0xB703) == 0) *msg = 0x313E;
        else if (gabi::call<s32>(0x0258839C) < 4) *msg = 0x313B;          /* dLib_getIplDaysFromSaveTime */
        else if (isEventBit(saveInfo() + 0x1178, 0x102)) *msg = 0x313C;
        else { onEventBit(saveInfo() + 0x1178, 0x102); *msg = 0x313D; }
        break;
    case 0x313A: case 0x313B: case 0x313C: case 0x313D: case 0x313E:
        *msg = 0x313F; break;
    case 0x313F: case 0x314F:
        if (rd<u32>(msgMng, 0x948) == 0) {
            if (rd<u16>(saveInfo(), 0x24) < 50) *msg = 0x3143;
            else {
                u32 g = play(); wr<s32>(g, 0x5B48, rd<s32>(g, 0x5B48) - 50);
                wr<u8>(play(), 0x5BB9, 25);
                *msg = 0x3144;
            }
        } else *msg = 0x3142;
        break;
    case 0x3141:
        if (!a->mbGreeted) { a->mbGreeted = 1; *msg = 0x3139; }
        else *msg = 0x313A;
        break;
    case 0x3149:
        if (rd<u32>(msgMng, 0x948) == 0) {
            onEventBit(saveInfo() + 0x644, 0x2508);
            a->mbTicketBought = 1; *msg = 0x314B;
        } else *msg = 0x314A;
        break;
    case 0x314A: *msg = 0x3148; break;
    case 0x314D: *msg = 0x3147; break;
    case 0x3140: case 0x3145: case 0x3146: case 0x3147: case 0x3148: case 0x314C: case 0x314E:
    case 0x3150: case 0x3152: case 0x3154: case 0x3156: case 0x3158:
        *msg = cur + 1; break;
    default:
        return 0x10;
    }
    return 0xF;
}
VERIFY(0x022699D0, daNpc_Kg2_next_msgStatus);

/* 02269C44 */
u32 daNpc_Kg2_getMsg(daNpc_Kg2_c* a) {
    WWHD_FUNC(0x02269C44, u32, a);
    if (a->mNextMsgNo != 0) {
        u32 count = getEventReg(saveInfo() + 0x644, 0xB703);
        u32 msg = a->mNextMsgNo;
        if (msg == 0x3145) {
            if (a->mbTicketBought == 1) msg = 0x314B;
            else if (!isEventBit(saveInfo() + 0x644, 0x2508)) {
                msg = a->mNextMsgNo; a->mNextMsgNo = 0;
                return msg;
            } else msg = 0x314C;
        } else if (msg == 0x3150) {
            if (count != 0) msg = 0x3154;
        } else if (msg == 0x3152) {
            if (count == 1) msg = 0x3156;
            else if (count >= 2) msg = 0x3158;
        }
        a->mNextMsgNo = 0;
        return msg;
    }
    if (a->mbNight == 1) return 0x315B;
    if (isEventBit(saveInfo() + 0x644, 0x2540) && !isEventBit(saveInfo() + 0x644, 0x2520)) {
        onEventBit(saveInfo() + 0x644, 0x2520);
        return 0x3140;
    }
    if (!a->mbGreeted) { a->mbGreeted = 1; return 0x3139; }
    return 0x313A;
}
VERIFY(0x02269C44, daNpc_Kg2_getMsg);

/* 02269DB0 */
void daNpc_Kg2_anmAtr(daNpc_Kg2_c* a, u16 atr) {
    WWHD_FUNC(0x02269DB0, void, a, atr);
    u8 req = rd<u8>(play(), 0x5BC5);
    if (a->mbNight == 1) return;
    s8 anm = -1, btp = 0;
    switch (req) {
    case 0: anm = 0; btp = 0; break;
    case 1: anm = 1; btp = 0; break;
    case 2: anm = 2; btp = 1; break;
    case 3: anm = 3; btp = 0; break;
    case 4: anm = 5; btp = 1; break;
    case 5: anm = 6; btp = 1; break;
    case 6: anm = 7; btp = 2; break;
    case 7: anm = 8; btp = 3; break;
    case 8: anm = 9; btp = 0; break;
    case 9: anm = 10; btp = 1; break;
    case 10: anm = 11; btp = 2; break;
    case 11: anm = 14; btp = 2; break;
    }
    if (anm >= 0) {
        daNpc_Kg2_setAnm(a, anm, -1.0f);
        a->mBtpIdx = btp;
        daNpc_Kg2_initTexPatternAnm(a, 1);
    }
    if (a->mAnmIdx == 3) {
        u32 morf = gabi::ea(a->mpMorf.get());
        if (checkPass(morf + 0x98, endFrameMinus1(morf))) {
            daNpc_Kg2_setAnm(a, 4, -1.0f);
            a->mBtpIdx = 0;
            daNpc_Kg2_initTexPatternAnm(a, 1);
        }
    }
    wr<u8>(play(), 0x5BC5, 0xFF);
}
VERIFY(0x02269DB0, daNpc_Kg2_anmAtr);

/* 0226A0B0 */
void daNpc_Kg2_setAttention(daNpc_Kg2_c* a) {
    WWHD_FUNC(0x0226A0B0, void, a);
    copyWords(gabi::ea(a) + 0x37C, gabi::ea(a) + 0x8DC, 3);
}
VERIFY(0x0226A0B0, daNpc_Kg2_setAttention);

/* 0226A0CC */
void daNpc_Kg2_lookBack(daNpc_Kg2_c* a) {
    WWHD_FUNC(0x0226A0CC, void, a);
    u32 p = gabi::ea(a);
    s32 mode = a->mMode;
    u32 ex = 0, ey = 0, ez = 0;   /* 0.0f */
    u32 target = 0;
    s16 angle = rd<s16>(p, 0x322);
    gabi::Local<cXyz> eye, dst, eyePos;
    if ((u32)mode >= 1 && (u32)mode <= 2) {
        if (mode == 2) wr<u8>(p, 0x3B6, 1);
        if (a->mbAttention) {
            gabi::call(0x0259D54C, eyePos.get(), gabi::load<f32>(HIO + 4));   /* dNpc_playerEyePos */
            copyWords(gabi::ea(dst.get()), gabi::ea(eyePos.get()), 3);
            ex = rd<u32>(p, 0x314); ey = rd<u32>(p, 0x380); ez = rd<u32>(p, 0x31C);
            target = gabi::ea(dst.get());
        }
    }
    if (rd<u8>(p, 0x3B6)) {
        gabi::call(0x0200F428, p + 0x8F4, gabi::load<s16>(HIO + 0x1A), 4, 0x800);   /* cLib_addCalcAngleS2 */
        s16 speed = a->mLookSpeed;
        u32 e = gabi::ea(eye.get());
        gabi::store<u32>(e, ex); gabi::store<u32>(e + 4, ey); gabi::store<u32>(e + 8, ez);
        gabi::call(0x0259DED0, p + 0x3AC, p + 0x322, target, eye.get(), angle, speed, 1);   /* lookAtTarget */
    } else {
        a->mLookSpeed = 0;
        u32 e = gabi::ea(eye.get());
        gabi::store<u32>(e, ex); gabi::store<u32>(e + 4, ey); gabi::store<u32>(e + 8, ez);
        gabi::call(0x0259DED0, p + 0x3AC, p + 0x322, target, eye.get(), angle, 0, 1);
    }
}
VERIFY(0x0226A0CC, daNpc_Kg2_lookBack);

/* 0226A258 */
void daNpc_Kg2_wait01(daNpc_Kg2_c* a) {
    WWHD_FUNC(0x0226A258, void, a);
    f32 time = rd<f32>(saveInfo(), 0x44);
    if (time < 105.0f || !(time < 300.0f)) {
        if (a->mbNight == 0) {
            a->mbNight = 1;
            daNpc_Kg2_setAnm(a, 13, -1.0f);
            a->mBtpIdx = 0;
            daNpc_Kg2_initTexPatternAnm(a, 1);
        }
    } else if (a->mbNight == 1) {
        a->mbNight = 0;
        daNpc_Kg2_setAnm(a, 12, -1.0f);
        a->mBtpIdx = 0;
        daNpc_Kg2_initTexPatternAnm(a, 1);
    }
    if (a->mbTalkRequest) {
        a->mMode = 2;
        return;
    }
    u32 player = rd<u32>(play(), 0x5B2C);
    f32 dy = std::fabs(rd<f32>(gabi::ea(a), 0x318) - rd<f32>(player, 0x318));
    if (a->mbAttention && dy < 50.0f) {
        s8 anm = a->mAnmIdx;
        if (anm != 13 && anm != 12) a->mEventOrder = 2;
    }
}
VERIFY(0x0226A258, daNpc_Kg2_wait01);

/* 0226A394 */
void daNpc_Kg2_talk01(daNpc_Kg2_c* a) {
    WWHD_FUNC(0x0226A394, void, a);
    if (a->talk(1) != 0x12) return;
    a->mMode = 1;
    u32 g = play();
    wr<u16>(g, 0x52B8, rd<u16>(g, 0x52B8) | 8);
    u32 msgNo = a->mCurrMsgNo;
    a->mbTalkRequest = 0;
    if (msgNo == 0x3144) {
        a->mEventNo = 0; a->mEventOrder = 3;
        return;
    }
    if (a->mbNight == 0) {
        daNpc_Kg2_setAnm(a, 1, -1.0f);
        a->mBtpIdx = 0;
        daNpc_Kg2_initTexPatternAnm(a, 1);
    } else {
        daNpc_Kg2_setAnm(a, 4, -1.0f);
    }
}
VERIFY(0x0226A394, daNpc_Kg2_talk01);

static void chkAttentionHere(daNpc_Kg2_c* a) {
    u32 p = gabi::ea(a);
    s16 angle = (s16)(rd<s16>(p, 0x322) + rd<s16>(p, 0x3AE) + rd<s16>(p, 0x3B2));
    gabi::Local<cXyz> pos;
    copyWords(gabi::ea(pos.get()), p + 0x314, 3);
    a->mbAttention = daNpc_Kg2_chkAttention(a, pos.get(), angle);
}

/* 0226A474 */
BOOL daNpc_Kg2_wait_action(daNpc_Kg2_c* a, void*) {
    WWHD_FUNC(0x0226A474, BOOL, a);
    s8 st = a->mActionState;
    if (st == 0) {
        a->mMode = 1;
        a->mActionState = (s8)(a->mActionState + 1);
        return TRUE;
    }
    if (st == -1) return TRUE;
    chkAttentionHere(a);
    s8 mode = a->mMode;
    a->mEventOrder = 0;
    if ((u32)mode == 1) daNpc_Kg2_wait01(a);
    else if ((u32)mode == 2) daNpc_Kg2_talk01(a);
    daNpc_Kg2_lookBack(a);
    daNpc_Kg2_setAttention(a);
    return TRUE;
}
VERIFY(0x0226A474, daNpc_Kg2_wait_action);

/* 0226A574 */
BOOL daNpc_Kg2_event_wait_action(daNpc_Kg2_c* a, void*) {
    WWHD_FUNC(0x0226A574, BOOL, a);
    s8 st = a->mActionState;
    if (st == 0) {
        a->mMode = 1;
        a->mActionState = (s8)(a->mActionState + 1);
        return TRUE;
    }
    if (st == -1) return TRUE;
    chkAttentionHere(a);
    daNpc_Kg2_lookBack(a);
    daNpc_Kg2_setAttention(a);
    s16 ev = a->mEventIds[(s32)a->mEventNo];
    if (!gabi::call<BOOL>(0x025440C8, evtMng(), ev)) return TRUE;   /* endCheck */
    switch (a->mEventNo) {
    case 0:
        a->mEventOrder = 3; a->mEventNo = 3;
        break;
    case 3:
        if (gabi::load<u8>(canon_game_result) == 1) { a->mEventOrder = 3; a->mEventNo = 1; }
        else { a->mEventOrder = 1; a->mNextMsgNo = 0x314E; }
        break;
    case 1: {
        a->mEventOrder = 3; a->mEventNo = 2;
        s32 n = (s32)getEventReg(saveInfo() + 0x644, 0xB703) + 1;
        if (n > 3) n = 3;
        gabi::call(0x025B8AF4, saveInfo() + 0x644, 0xB703, (u8)n);    /* setEventReg */
        break;
    }
    case 2:
        a->mEventOrder = 1; a->mNextMsgNo = 0x315A;
        break;
    default:
        a->mEventNo = 4;
        break;
    }
    u32 g = play();
    wr<u16>(g, 0x52B8, rd<u16>(g, 0x52B8) | 8);
    setAction(a, ACT_wait);
    return TRUE;
}
VERIFY(0x0226A574, daNpc_Kg2_event_wait_action);

/* 0226A99C */
void daNpc_Kg2_sinit() {
    WWHD_FUNC(0x0226A99C, void);
    gabi::store<u32>(0x10467620, 0); gabi::store<u32>(0x10467618, 0);
    gabi::store<u32>(0x10467624, 0); gabi::store<u32>(0x1046761C, 0);
    gabi::call(0x028F026C, 0x101BF258);                   /* __register_global_object */
    gabi::store<u32>(0x1046760C, gabi::load<u32>(0x1001C1E0));
    gabi::store<u32>(0x10467610, gabi::load<u32>(0x1001C1E4));
    gabi::call(0x028ED6F8, 0x10467614);
    gabi::call(0x028F026C, 0x101BF264);
    gabi::call(0x028EAB2C, 0x10467615);
    gabi::call(0x028F026C, 0x101BF270);
    daNpc_Kg2_HIO_ct(gabi::at<daNpc_Kg2_HIO_c>(HIO));
}
VERIFY(0x0226A99C, daNpc_Kg2_sinit);

/* 0226AA3C daNpc_Kg2_HIO_c deleting destructor */
void daNpc_Kg2_HIO_dt(daNpc_Kg2_HIO_c* self, u32 flags) {
    WWHD_FUNC(0x0226AA3C, void, self, flags);
    if (self && (flags & 1)) gabi::call(0x0273AF40, self);
}
VERIFY(0x0226AA3C, daNpc_Kg2_HIO_dt);

/* 0226AA50 */
BOOL daNpc_Kg2_IsDelete(daNpc_Kg2_c* a) { WWHD_FUNC(0x0226AA50, BOOL, a); return TRUE; }
VERIFY(0x0226AA50, daNpc_Kg2_IsDelete);

/* 0226AA58 daNpc_Kg2_c deleting destructor */
void daNpc_Kg2_dt(daNpc_Kg2_c* a, u32 flags) {
    WWHD_FUNC(0x0226AA58, void, a, flags);
    if (!a) return;
    u32 p = gabi::ea(a);
    gabi::call(0x02515A70, p + 0x690, 2);       /* dCcD_Cyl dtor */
    gabi::call(0x02515860, p + 0x654, 2);       /* dCcD_Stts dtor */
    gabi::call(0x02018034, p + 0x628, 2);
    wr<u32>(p, 0x470, 0x1001BF94); wr<u32>(p, 0x464, 0x1001BFA4);
    gabi::call(0x024EFD9C, p + 0x450, 0);       /* dBgS_Acch dtor */
    gabi::call(0x025D50BC, a, 0);               /* fopAc_ac_c dtor */
    if (flags & 1) gabi::call(0x0273AF40, a);
}
VERIFY(0x0226AA58, daNpc_Kg2_dt);

/* 0226AAF4 empty virtual */
void daNpc_Kg2_emptyVirtual(void* self) { WWHD_FUNC(0x0226AAF4, void, self); }
VERIFY(0x0226AAF4, daNpc_Kg2_emptyVirtual);
