/**
 * d_a_npc_btsw.cpp (WWHD)
 * NPC - Baito (Dragon Roost postman, mail-sorting minigame)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_btsw.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_npc_btsw.h"

#include <cmath>

using gabi::load;
using gabi::store;

static inline u32 A(const void* p) { return gabi::ea(p); }
/* dComIfGp_get() (the play object singleton) */
static inline u32 btsw_play() { return gabi::ea(gabi::call<void*>(0x025200D4)); }
/* save info: *(0x101F84DC) */
static inline u32 btsw_save() { return load<u32>(0x101F84DC); }
static inline u32 btsw_isEventBit(u16 flag) { return gabi::call<u32>(0x025B8B94, btsw_save() + 0x644, (u32)flag); }
static inline void btsw_onEventBit(u16 flag) { gabi::call(0x025B8B68, btsw_save() + 0x644, (u32)flag); }
static inline void btsw_seStart(u32 id) { gabi::call(0x025E1988, id); } /* mDoAud_seStart (HD: no position) */
static inline void btsw_assert(u32 file, s32 line, u32 msg) { gabi::call(0x0273AA24, file, line, msg); }

/* dComIfG_getObjectIDRes(arc, id): HD dRes_control_c::getIDRes(SafeString, id) */
static u32 btsw_getIDRes(u32 arc, s32 id) {
    gabi::Local<SafeString> key;
    key->mStringTop = arc;
    key->__vtbl = BTSW_SAFESTRING_VTBL;
    return gabi::call<u32>(0x026067F4, load<u32>(0x101F4F28), key.get(), id);
}
/* modelData->getJointName()->getIndex(name) */
static s32 btsw_jointIndex(u32 modelData, u32 name) {
    u32 names = gabi::call<u32>(0x027F68FC, modelData);
    u32 disp = load<u32>(names + 0x10);
    return gabi::call<s32>(0x027DF9B0, disp ? names + 0x10 + disp : 0u, name);
}
/* J3DModel::getAnmMtx(jnt) (HD: the joint matrix block at +0x2C, marked dirty) */
static u32 btsw_getAnmMtx(u32 model, s32 jnt) {
    u32 blk = load<u32>(model + 0x2C);
    u16 flags = load<u16>(blk + 4);
    u32 arr = load<u32>(blk + 0x10);
    store<u16>(blk + 4, (u16)(flags | 0x10));
    return arr + (u32)(jnt * 0x30);
}
/* (this->*mCurrActionFunc)(arg) */
static void btsw_actionCall(daNpc_Btsw_c* t, u32 arg) {
    u32 a = A(t);
    s16 d = t->mCurrActionFunc.d;
    s16 i = t->mCurrActionFunc.i;
    u32 recv = a + (u32)(s32)d;
    u32 target;
    if (i < 0) {
        target = t->mCurrActionFunc.f;
    } else {
        s16 vofs = load<s16>(a + 0xD62);
        u32 vtbl = load<u32>(recv + (u32)(s32)vofs);
        target = load<u32>(vtbl + (u32)((s32)i * 8) + 4);
    }
    gabi::call_ptr<BOOL>(target, recv, arg);
}
/* setAction(&daNpc_Btsw_c::fn, NULL) */
static void btsw_setAction(daNpc_Btsw_c* t, u32 fn) {
    s16 i = t->mCurrActionFunc.i;
    if (i == -1 && t->mCurrActionFunc.d == 0 && t->mCurrActionFunc.f == fn) return;
    if (i != 0) {
        t->mActionStatus = daNpc_Btsw_c::ACTION_ENDING;
        btsw_actionCall(t, 0);
    }
    t->mCurrActionFunc.d = 0;
    t->mCurrActionFunc.i = -1;
    t->mCurrActionFunc.f = fn;
    t->mActionStatus = daNpc_Btsw_c::ACTION_STARTING;
    s16 d = t->mCurrActionFunc.d;
    gabi::call_ptr<BOOL>(t->mCurrActionFunc.f.get(), A(t) + (u32)(s32)d, 0u);
}
/* SwMail2_c::SetProc(&SwMail2_c::fn) */
static void btsw_mailSetProc(u32 m, u32 fn) {
    store<s16>(m + 0, 0);
    store<s16>(m + 2, -1);
    store<u32>(m + 4, fn);
}
/* SwMail2_c::CheckProc(&SwMail2_c::fn) */
static bool btsw_mailCheckProc(u32 m, u32 fn) {
    return load<s16>(m + 2) == -1 && load<s16>(m + 0) == 0 && load<u32>(m + 4) == fn;
}
/* calls through the mail pointers (field_0x920[i]->fn()) */
static inline void btsw_mailCall(u32 fn, u32 mail) { gabi::call(fn, mail); }
static inline void btsw_mailCall2(u32 fn, u32 mail, u32 arg) { gabi::call(fn, mail, arg); }
#define MAIL_init 0x02217E74u
#define MAIL_DummyInit 0x02218114u
#define MAIL_AppearInit 0x02219B24u
#define MAIL_ThrowInit 0x02219EA0u
#define MAIL_move 0x02219ADCu
#define MAIL_draw 0x022197C8u
#define MAIL_MailCreateInit 0x022181ECu
/* dComIfG_getTimerRestTimeMs(): HD frames left * 1000 / 30 */
static s32 btsw_getTimerRestTimeMs() {
    s32 limit = load<s32>(btsw_play() + 0x5CF8);
    s32 now = load<s32>(btsw_play() + 0x5CF4);
    return (s32)(((u32)limit - (u32)now) * 1000u) / 30;
}
/* dComIfGp_event_reset() */
static void btsw_eventReset() {
    u32 p = btsw_play();
    store<u16>(p + 0x52B8, (u16)(load<u16>(p + 0x52B8) | 8));
}

/* 0221798C */
static BOOL nodeCallBack(J3DNode* node, int calcTiming) {
    WWHD_FUNC(0x0221798C, BOOL, node, calcTiming);
    if (calcTiming != 0 /* J3DNodeCBCalcTiming_In */) return TRUE;
    u32 model = load<u32>(0x104B462C); /* j3dSys.getModel() */
    u32 i_this = load<u32>(model + 0xB8);
    if (i_this == 0) return TRUE;
    /* static cXyz a_att_pos_offst(0.0f, 0.0f, 0.0f); */
    f32 zero = load<f32>(0x10018744);
    if (load<u32>(0x10466A48) == 0) {
        store<f32>(0x10466940, zero);
        store<f32>(0x10466948, zero);
        store<u32>(0x10466A48, 1);
        store<f32>(0x10466944, zero);
    }
    /* static cXyz a_eye_pos_offst(10.0f, 20.0f, 0.0f); */
    if (load<u32>(0x10466A4C) == 0) {
        store<u32>(0x10466A4C, 1);
        store<f32>(0x1046694C, 10.0f);
        store<f32>(0x10466950, 20.0f);
        store<f32>(0x10466954, zero);
    }
    u32 jointNo = load<u16>(gabi::call<u32>(0x027F7878, node) + 4);
    const u32 stack = 0x1048D0CC; /* mDoMtx_stack_c::now */
    gabi::call(0x028E90D4, btsw_getAnmMtx(model, (s32)jointNo), stack); /* mDoMtx_stack_c::copy */
    if (jointNo == (u32)(s32)load<s8>(i_this + 0x3B4)) {
        gabi::call(0x028E8F64, stack, 0x10466940u, i_this + 0xD00); /* multVec(&a_att_pos_offst, &mAttPos) */
        gabi::Local<Mtx34> mtx;
        gabi::call(0x028E90D4, stack, mtx.get());
        f32 x = mtx->m[0][3];
        f32 z = mtx->m[2][3];
        mtx->m[2][3] = zero;
        f32 y = mtx->m[1][3];
        mtx->m[1][3] = zero;
        mtx->m[0][3] = zero;
        gabi::call(0x028E93CC, stack, x, y, z); /* transS(vec) */
        gabi::call(0x025F1C28, stack, (s32)load<s16>(i_this + 0x3AE)); /* YrotM(getHead_y()) */
        gabi::call(0x025F1BF4, stack, (s32)load<s16>(i_this + 0x3AC)); /* XrotM(getHead_x()) */
        gabi::call(0x028E9108, stack, mtx.get(), stack);             /* concat(mtx) */
        gabi::call(0x028E8F64, stack, 0x1046694Cu, i_this + 0x37C);  /* multVec(&a_eye_pos_offst, &eyePos) */
    } else if (jointNo == (u32)(s32)load<s8>(i_this + 0x3B5)) {
        gabi::call(0x025F1BF4, stack, (s32)load<s16>(i_this + 0x3B2));             /* XrotM(getBackbone_y()) */
        gabi::call(0x025F1C5C, stack, (s32)(s16)-(s32)load<s16>(i_this + 0x3B0)); /* ZrotM(-getBackbone_x()) */
    }
    gabi::call(0x028E90D4, stack, 0x104B4868u); /* J3DSys::mCurrentMtx */
    mtx_copy(gabi::at<Mtx34>(btsw_getAnmMtx(model, (s32)jointNo)), gabi::at<Mtx34>(stack)); /* setAnmMtx */
    return TRUE;
}
VERIFY(0x0221798C, nodeCallBack);

/* 02217C54: daNpc_Btsw_XyCheckCB (unnamed by the matcher) */
static s16 daNpc_Btsw_XyCheckCB(void* actor, int i_itemBtn) {
    WWHD_FUNC(0x02217C54, s16, actor, i_itemBtn);
    /* dComIfGp_getSelectItem(i_itemBtn) == dItemNo_NOTE_TO_MOM_e */
    return load<u8>(btsw_play() + (u32)i_itemBtn + 0x5BBB) == 0x99;
}
VERIFY(0x02217C54, daNpc_Btsw_XyCheckCB);

/* 02217C94 */
BOOL daNpc_Btsw_c::initTexPatternAnm(u32 i_modify) {
    WWHD_FUNC(0x02217C94, BOOL, this, i_modify);
    u32 a = A(this);
    u32 model = load<u32>(load<u32>(a + 0x44C) + 0x90);
    s32 btp = load<s32>(0x100186BC); /* HD: l_btp_ix_tbl[0] (GameCube l_btp_ix_tbl[field_0x9C4]) */
    u32 modelData = load<u32>(model + 0xAC);
    u32 pattern = btsw_getIDRes(0x10018754 /* "Btsw" */, btp);
    m_head_tex_pattern.v = pattern;
    if (pattern == 0) {
        btsw_assert(0x10018778, 0x154, 0x1001875C);
        pattern = m_head_tex_pattern.v;
    }
    /* mDoExt_btpAnm::init(modelData, pattern, TRUE, EMode_LOOP, 1.0f, 0, -1, i_modify, FALSE) */
    if (!gabi::call<BOOL>(0x025E789C, a + 0xA18, modelData, pattern, 1, 2, 0, -1, i_modify, 0, 1.0f)) return FALSE;
    mBtpFrame = 0;
    mBlinkTimer = 0;
    return TRUE;
}
VERIFY(0x02217C94, &daNpc_Btsw_c::initTexPatternAnm);

/* 02217D98 */
void SwMail2_c::set_mtx() {
    WWHD_FUNC(0x02217D98, void, this);
    u32 m = A(this);
    f32 x = gabi::fadds_ppc(mPos.x, mOffset.x);
    f32 y = gabi::fadds_ppc(mPos.y, mOffset.y);
    f32 z = gabi::fadds_ppc(mPos.z, mOffset.z);
    gabi::call(0x028E93CC, 0x1048D0CCu, x, y, z);                                               /* transS */
    gabi::call(0x025F1B48, 0x1048D0CCu, (s32)mAngle.x, (s32)mAngle.y, (s32)mAngle.z);          /* ZXYrotM */
    mtx_copy(gabi::at<Mtx34>(load<u32>(m + 8) + 0xC8), gabi::at<Mtx34>(0x1048D0CC));           /* setBaseTRMtx */
}
VERIFY(0x02217D98, &SwMail2_c::set_mtx);

/* 02217E74 */
void SwMail2_c::init() {
    WWHD_FUNC(0x02217E74, void, this);
    u32 m = A(this);
    mStep = 0;
    mNo = 0;
    mSpeed.copy(*gabi::at<cXyz>(0x101FFBA8));  /* cXyz::Zero */
    mOffset.copy(*gabi::at<cXyz>(0x101FFBA8));
    mAimNo = 0;
    u32 c = load<u32>(m + 0xB8);
    f32 cx = load<f32>(c), cy = load<f32>(c + 4), cz = load<f32>(c + 8);
    mPos.x = cx;
    mPos.z = cz;
    mPos.y = gabi::fsubs_ppc(cy, 200.0f);
    set_mtx();
}
VERIFY(0x02217E74, &SwMail2_c::init);

/* 02217EE4 */
u8 SwMail2_c::getNextNo(u8 previous_no) {
    WWHD_FUNC(0x02217EE4, u8, previous_no);
    f32 random = gabi::call<f32>(0x020198D8, 10.0f);
    u8 sameCount = load<u8>(0x101BD78D); /* m_same_count */
    f32 fVar4 = gabi::fnmsubs(1.25f, (f32)sameCount, 2.5f); /* 2.5f - m_same_count * 1.25f */
    s32 box_x = previous_no / 3;
    s32 box_y = previous_no - box_x * 3;
    f32 half = 0.5f;
    if (random < fVar4) {
        store<u8>(0x101BD78D, (u8)(sameCount + 1));
    } else {
        f32 new_threshold = gabi::fadds_ppc(2.5f, fVar4);
        if (random < new_threshold) {
            if (gabi::call<f32>(0x020198D8, 1.0f) > half) {
                if (box_y == 0 || box_y == 2) {
                    box_y = 1;
                } else {
                    box_y = 2;
                    if (gabi::call<f32>(0x020198D8, 1.0f) > half) box_y = 0;
                }
            } else {
                box_x = box_x == 0;
            }
        } else {
            f32 r = gabi::call<f32>(0x020198D8, 1.0f);
            if (r < load<f32>(0x100187A4) /* 0.3333f */) {
                if (box_y == 0 || box_y == 2) {
                    box_y = 1;
                } else {
                    box_y = 2;
                    if (gabi::call<f32>(0x020198D8, 1.0f) > half) box_y = 0;
                }
                box_x = box_x == 0;
            } else if (r < load<f32>(0x100187A8) /* 0.6666f */) {
                if (box_y == 0) {
                    box_y = 2;
                } else if (box_y == 2) {
                    box_y = 0;
                } else {
                    box_y = gabi::call<f32>(0x020198D8, 1.0f) > half ? 0 : 2;
                }
            } else {
                if (box_y == 0) {
                    box_y = 2;
                } else if (box_y == 2) {
                    box_y = 0;
                } else {
                    box_y = gabi::call<f32>(0x020198D8, 1.0f) > half ? 0 : 2;
                }
                box_x = box_x == 0;
            }
        }
        store<u8>(0x101BD78D, 0);
    }
    u8 res = (u8)(box_y + box_x * 3);
    store<u8>(0x101BD78C, res); /* m_no_buff */
    return res;
}
VERIFY(0x02217EE4, &SwMail2_c::getNextNo);

/* 02218114 */
void SwMail2_c::DummyInit() {
    WWHD_FUNC(0x02218114, void, this);
    u32 m = A(this);
    gabi::call<s16>(0x0200F93C, load<u32>(m + 0xB4), load<u32>(m + 0xB8)); /* cLib_targetAngleY (unused) */
    mAngle.y = -0x4000;
    mAngle.z = 0;
    mAngle.x = 0;
    u32 c = load<u32>(m + 0xB8);
    f32 cy = load<f32>(c + 4), cx = load<f32>(c), cz = load<f32>(c + 8);
    mPos.z = cz;
    mPos.y = gabi::fsubs_ppc(cy, 200.0f);
    mPos.x = gabi::fsubs_ppc(cx, 150.0f);
    u8 no = getNextNo(mNo);
    mNo = no;
    mBaseAngle.z = -0x4000;
    mBaseAngle.x = 0;
    mBaseAngle.y = -0x4000;
    mOffset.copy(*gabi::at<cXyz>(0x101FFBA8));
    btsw_mailSetProc(m, BTSW_MAIL_Dummy);
}
VERIFY(0x02218114, &SwMail2_c::DummyInit);

/* 022181EC */
BOOL SwMail2_c::MailCreateInit(cXyz* eye, cXyz* center) {
    WWHD_FUNC(0x022181EC, BOOL, this, eye, center);
    u32 m = A(this);
    u32 modelData = btsw_getIDRes(0x100187B0 /* "Btsw" */, 4);
    u32 model = gabi::call<u32>(0x025E38E0, modelData, 0x80000, 0x11020002); /* mDoExt_J3DModel__create */
    store<u32>(m + 8, model);
    if (model == 0) return FALSE;
    u32 pattern = btsw_getIDRes(0x100187B0, 0xA);
    /* field_0x10.init(modelData, texPattern, TRUE, EMode_LOOP, 0.0f, 0, -1, false, FALSE) */
    if (!gabi::call<BOOL>(0x025E789C, m + 0xC, modelData, pattern, 1, 2, 0, -1, 0, 0, 0.0f)) return FALSE;
    mpCenter.v = A(center);
    mNo = 0;
    mpEye.v = A(eye);
    init();
    DummyInit();
    return TRUE;
}
VERIFY(0x022181EC, &SwMail2_c::MailCreateInit);

/* 022182F4 */
BOOL daNpc_Btsw_c::CreateHeap() {
    WWHD_FUNC(0x022182F4, BOOL, this);
    u32 a = A(this);
    u32 modelData = btsw_getIDRes(0x100187D0 /* "Btsw" */, 0xD /* BDL_BN */);
    if (modelData == 0) btsw_assert(0x100187E0, 0x5BA, 0x100187F4);
    u32 bck = btsw_getIDRes(0x100187D0, 0x17 /* BCK_BN_WAIT02 */);
    u32 morf = gabi::call<u32>(0x025E4F64, 0u, modelData, 0u, 0u, bck, 2, 0, -1, 1, 0, 0x80000, 0x15020022, 1.0f);
    mpMorf.v = morf;
    if (morf == 0 || load<u32>(morf + 0x90) == 0) return FALSE;
    s8 head = (s8)btsw_jointIndex(modelData, 0x100187D8 /* "head" */);
    m_jnt.mHeadJntNum = head;
    if (head < 0) btsw_assert(0x100187E0, 0x5CB, 0x10018808);
    s8 backbone = (s8)btsw_jointIndex(modelData, 0x10018824 /* "backbone" */);
    m_jnt.mBackboneJntNum = backbone;
    if (backbone < 0) btsw_assert(0x100187E0, 0x5CD, 0x10018830);
    m_handL = (s8)btsw_jointIndex(modelData, 0x100187C0 /* "handL" */);
    m_handR = (s8)btsw_jointIndex(modelData, 0x100187C8 /* "handR" */);
    u32 letterData = btsw_getIDRes(0x100187D0, 0x19 /* BDL_BM_LETTER */);
    u32 letter = gabi::call<u32>(0x025E38E0, letterData, 0, 0x11020203);
    mpLetterModel.v = letter;
    if (letter == 0) return FALSE;
    mTexIdx = 0;
    if (!initTexPatternAnm(0)) return FALSE;
    /* HD: getJointNum() through 027F3F94 (named __nw by the matcher) */
    for (u32 i = 0; i < load<u16>(gabi::call<u32>(0x027F3F94, modelData) + 8); i = (u16)(i + 1)) {
        if (i == (u32)(s32)(s8)m_jnt.mHeadJntNum || i == (u32)(s32)(s8)m_jnt.mBackboneJntNum) {
            /* getJointNodePointer(i)->setCallBack(nodeCallBack) */
            u32 data = load<u32>(load<u32>(load<u32>(a + 0x44C) + 0x90) + 0xAC);
            u32 count = load<u32>(data + 4);
            u32 arr = load<u32>(data + 8);
            u32 node = i < count ? arr + i * 0x1C : arr;
            store<u32>(node + 8, 0x0221798C);
        }
    }
    store<u32>(load<u32>(load<u32>(a + 0x44C) + 0x90) + 0xB8, a); /* setUserArea(this) */
    mpMail[0].v = a + 0xA90;
    mpMail[2].v = a + 0xC08;
    mMailIdx = 0;
    mpMail[1].v = a + 0xB4C;
    u32 eyeP = a + 0xCD4, centerP = a + 0xCE0;
    if (!gabi::call<BOOL>(MAIL_MailCreateInit, a + 0xA90, eyeP, centerP)) return FALSE;
    if (!gabi::call<BOOL>(MAIL_MailCreateInit, mpMail[1].v.get(), eyeP, centerP)) return FALSE;
    if (!gabi::call<BOOL>(MAIL_MailCreateInit, mpMail[2].v.get(), eyeP, centerP)) return FALSE;
    u32 tevRegKey = btsw_getIDRes(0x100187D0, 8 /* BRK_SHOP_CURSOR01 */);
    u32 cursorData = btsw_getIDRes(0x100187D0, 5 /* BMD_SHOP_CURSOR01 */);
    u32 cursor = gabi::call<u32>(0x025BBD7C, cursorData, tevRegKey, 0.65f); /* ShopCursor_create */
    mpShopCursor = cursor;
    if (cursor == 0) return FALSE;
    store<u8>(cursor + 0xB4, 0); /* m54 */
    gabi::call(0x024EFF44, a + 0x614, 30.0f, 0.0f); /* mAcchCir.SetWall */
    gabi::call(0x024F06B4, a + 0x450, a + 0x314, a + 0x300, a, 1, a + 0x614, a + 0x33C, 0u, 0u); /* mObjAcch.Set */
    return TRUE;
}
VERIFY(0x022182F4, &daNpc_Btsw_c::CreateHeap);

/* 022186E8 */
static BOOL CallbackCreateHeap(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x022186E8, BOOL, i_this);
    return ((daNpc_Btsw_c*)i_this)->CreateHeap();
}
VERIFY(0x022186E8, CallbackCreateHeap);

/* 022186EC: daNpc_Btsw_c constructor (GameCube fopAcM_ct inline; HD out of line, allocates when
 * this == NULL) */
static daNpc_Btsw_c* daNpc_Btsw_c_ct(daNpc_Btsw_c* i_this) {
    WWHD_FUNC(0x022186EC, daNpc_Btsw_c*, i_this);
    u32 a = A(i_this);
    if (a == 0) {
        a = gabi::call<u32>(0x0273AD10, 0xD70);
        if (a == 0) return nullptr;
    }
    gabi::call(0x025A1458, a); /* fopNpc_npc_c::fopNpc_npc_c */
    store<u32>(a + 0xB4, BTSW_VTBL);
    /* dKy_tevstr_c constructor (HD): three copies of a 0x44-byte default block at +0, +0xC0, +0x144 */
    static const u8 kind[] = {4, 4, 4, 4, 4, 4, 1, 1, 1, 1, 2, 2, 2, 2, 4, 4, 4, 4, 4, 4, 4, 4};
    const u32 src = 0x1016E414;
    u32 off = 0;
    for (u32 k = 0; k < sizeof(kind); k++) {
        for (u32 copy = 0; copy < 3; copy++) {
            u32 dst = a + 0x7EC + (copy == 0 ? 0 : copy == 1 ? 0xC0 : 0x144) + off;
            if (kind[k] == 4) store<f32>(dst, load<f32>(src + off));
            else if (kind[k] == 2) store<s16>(dst, load<s16>(src + off));
            else store<u8>(dst, load<u8>(src + off));
        }
        off += kind[k];
    }
    gabi::call(0x025E7820, a + 0xA18); /* mDoExt_btpAnm constructors */
    gabi::call(0x025E7820, a + 0xA9C);
    gabi::call(0x025E7820, a + 0xB58);
    gabi::call(0x025E7820, a + 0xC14);
    /* SwCam2_c() */
    store<u8>(a + 0xCF2, 0);
    store<u8>(a + 0xCF0, 0);
    store<u8>(a + 0xCF1, 0);
    for (u32 i = 0; i < 12; i += 4) store<u32>(a + 0xCD4 + i, load<u32>(0x10466A64 + i)); /* camera_center_data[0][1] */
    for (u32 i = 0; i < 12; i += 4) store<u32>(a + 0xCE0 + i, load<u32>(0x10466AA0 + i)); /* camera_eye */
    store<f32>(a + 0xCEC, 58.0f);
    /* STControl() */
    store<u32>(a + 0xD40, 0x10050788);
    gabi::call(0x025885C4, a + 0xD1C, 0xF, 0xF, 0, 0, 0, 0x2000, 0.9f, 0.5f); /* setWaitParm */
    gabi::call(0x025885E8, a + 0xD1C);                                         /* init */
    return gabi::at<daNpc_Btsw_c>(a);
}
VERIFY(0x022186EC, daNpc_Btsw_c_ct);

/* 02218958 */
void daNpc_Btsw_c::set_mtx() {
    WWHD_FUNC(0x02218958, void, this);
    u32 a = A(this);
    f32 x = load<f32>(a + 0x314), z = load<f32>(a + 0x31C);
    u32 model = load<u32>(load<u32>(a + 0x44C) + 0x90);
    f32 y = load<f32>(a + 0x318);
    const u32 stack = 0x1048D0CC;
    gabi::call(0x028E93CC, stack, x, y, z);                     /* transS(current.pos) */
    gabi::call(0x025F1C28, stack, (s32)load<s16>(a + 0x322));   /* YrotM(current.angle.y) */
    mtx_copy(gabi::at<Mtx34>(model + 0xC8), gabi::at<Mtx34>(stack));
    s8 anm = mAnm;
    if (anm == 10 || anm == 9) {
        u32 morf = load<u32>(a + 0x44C);
        f32 vx, vy, vz;
        s16 rx, ry, rz;
        s8 jnt;
        if (load<f32>(morf + 0x9C) < 19.0f) { /* mpMorf->getFrame() */
            jnt = m_handR;
            if (anm == 9) {
                vx = 28.68f; vy = 4.68f; vz = -8.45f;
                rx = -0x43A2; ry = 0x9CA; rz = -0x233;
            } else {
                vx = 28.68f; vy = -0.43f; vz = -8.19f;
                rx = -0x43A2; ry = 0x9CA; rz = -0x2CB;
            }
        } else {
            if (anm == 9) {
                vx = 23.61f; vy = -5.08f; vz = -7.22f;
                rx = -0x49FC; ry = -0x458; rz = -0x568F;
            } else {
                vx = 24.9f; vy = 0.62f; vz = -7.51f;
                rx = -0x4C7F; ry = -0x7A; rz = -0x491D;
            }
            jnt = m_handL;
        }
        gabi::call(0x028E90D4, btsw_getAnmMtx(load<u32>(morf + 0x90), jnt), stack); /* copy(getAnmMtx(jnt)) */
        gabi::call(0x025F24E0, vx, vy, vz);                                         /* transM(vec) */
        gabi::call(0x025F19F8, stack, (s32)rx, (s32)ry, (s32)rz);                   /* XYZrotM */
        mtx_copy(gabi::at<Mtx34>(mpLetterModel.v + 0xC8), gabi::at<Mtx34>(stack));
    }
}
VERIFY(0x02218958, &daNpc_Btsw_c::set_mtx);

/* 02218D58 */
BOOL daNpc_Btsw_c::CreateInit() {
    WWHD_FUNC(0x02218D58, BOOL, this);
    u32 a = A(this);
    store<u32>(a + 0x39C, 10); /* attention_info.flags = LOCKON_TALK | ACTION_SPEAK */
    store<f32>(a + 0x374, -30.0f); /* gravity */
    /* field_0x96E = current.angle */
    mHomeAngle.z = (s16)load<u16>(a + 0x324);
    mHomeAngle.x = (s16)load<u16>(a + 0x320);
    mHomeAngle.y = (s16)load<u16>(a + 0x322);
    btsw_setAction(this, BTSW_wait_action);
    /* mAttPos = current.pos */
    store<u32>(a + 0xD00, load<u32>(a + 0x314));
    store<u32>(a + 0xD08, load<u32>(a + 0x31C));
    store<u32>(a + 0xD04, load<u32>(a + 0x318));
    gabi::call(0x02515F14, a + 0x654, 0xFF, 0xFF, a);  /* mStts.Init */
    gabi::call(0x02516518, a + 0x690, 0x101BD6A0u);    /* mCyl.Set(l_cyl_src) */
    store<u32>(a + 0x6D4, a + 0x654);                  /* mCyl.SetStts(&mStts) */
    gabi::call(0x025A15AC, a, 60.0f, 150.0f);          /* setCollision */
    field_0x9B4 = 0;
    store<u32>(a + 0x104, 0x02217C54); /* eventInfo.setXyCheckCB(daNpc_Btsw_XyCheckCB) */
    gabi::call(0x0259F814, a + 0x3E0, 0x10018890u /* "Btsw" */, a); /* mEventCut.setActorInfo2 */
    mMailActive = 0;
    mEventIdx = -1;
    mNextMsgNo = fpcM_ERROR_PROCESS_ID_e;
    store<u8>(a + 0x389, 0x6F); /* attention_info.distances[TALK] */
    mTimerCreated = 0;
    store<u8>(a + 0x38B, 0x6F); /* attention_info.distances[SPEAK] */
    gabi::call(0x025885C4, a + 0xD1C, 5, 2, 3, 2, 0, 0x2000, 1.0f, 0.9f); /* field_0x978.setWaitParm */
    set_mtx();
    gabi::call(0x025E55A0, mpMorf.v.get()); /* mpMorf->calc() */
    gabi::call(0x0255FFF4, a + 0x7EC, (s32)load<s8>(a + 0x2FE), 0xFF); /* dKy_tevstr_init(&field_0x6D4, home.roomNo, 0xFF) */
    return TRUE;
}
VERIFY(0x02218D58, &daNpc_Btsw_c::CreateInit);

/* 02218FA4 */
cPhs_State daNpc_Btsw_c::_create() {
    WWHD_FUNC(0x02218FA4, cPhs_State, this);
    u32 a = A(this);
    /* fopAcM_ct(this, daNpc_Btsw_c) */
    u32 cond = load<u32>(a + 0x2E4);
    if (!(cond & 8)) {
        if (a != 0) {
            gabi::call(0x022186EC, a);
            cond = load<u32>(a + 0x2E4);
        }
        store<u32>(a + 0x2E4, cond | 8);
    }
    cPhs_State res = gabi::call<cPhs_State>(0x02520460, a + 0x7E0, 0x10018898u /* "Btsw" */); /* dComIfG_resLoad */
    if (res == cPhs_COMPLEATE_e) {
        if (!gabi::call<BOOL>(0x025D63E8, a, 0x022186E8u, 0xCDA0)) return cPhs_ERROR_e; /* fopAcM_entrySolidHeap */
        u32 model = load<u32>(mpMorf.v + 0x90);
        store<u32>(a + 0x348, model ? model + 0xC8 : 0); /* fopAcM_SetMtx(this, getBaseTRMtx()) */
        if (load<s8>(BTSW_HIO) < 0) {
            /* mDoHIO_createChild("仕分けバイト君", &l_HIO) */
            store<u8>(BTSW_HIO, gabi::call<u8>(0x025F0A10, 0x100188A0u, BTSW_HIO));
        }
        if (!CreateInit()) return cPhs_ERROR_e;
    }
    return res;
}
VERIFY(0x02218FA4, &daNpc_Btsw_c::_create);

/* 022190B0 */
static cPhs_State daNpc_Btsw_Create(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x022190B0, cPhs_State, i_this);
    return ((daNpc_Btsw_c*)i_this)->_create();
}
VERIFY(0x022190B0, daNpc_Btsw_Create);

/* 022190B4 */
void SwMail2_c::SeDelete() {
    WWHD_FUNC(0x022190B4, void, this);
    gabi::call(0x025E1B34, A(this) + 0x80); /* mDoAud_seDeleteObject(&field_0x24) */
}
VERIFY(0x022190B4, &SwMail2_c::SeDelete);

/* 022190BC */
BOOL daNpc_Btsw_c::_delete() {
    WWHD_FUNC(0x022190BC, BOOL, this);
    u32 a = A(this);
    /* HD: no fopAcM_RegisterDeleteID */
    gabi::call(0x025204C8, a + 0x7E0, 0x100188B0u /* "Btsw" */); /* dComIfG_resDeleteDemo */
    if (load<u32>(a + 0xF4) != 0) { /* heap */
        u32 morf = mpMorf.v;
        if (morf != 0) gabi::call(0x025E563C, morf); /* stopZelAnime */
    }
    gabi::call(0x022190B4, a + 0xA90); /* mSwMail0.SeDelete() */
    gabi::call(0x022190B4, a + 0xB4C);
    gabi::call(0x022190B4, a + 0xC08);
    s8 no = load<s8>(BTSW_HIO);
    if (no >= 0) {
        gabi::call(0x025F0A18, (s32)no); /* mDoHIO_deleteChild */
        store<s8>(BTSW_HIO, -1);
    }
    return TRUE;
}
VERIFY(0x022190BC, &daNpc_Btsw_c::_delete);

/* 02219148 */
static BOOL daNpc_Btsw_Delete(daNpc_Btsw_c* i_this) {
    WWHD_FUNC(0x02219148, BOOL, i_this);
    return i_this->_delete();
}
VERIFY(0x02219148, daNpc_Btsw_Delete);

/* 0221914C */
void daNpc_Btsw_c::playTexPatternAnm() {
    WWHD_FUNC(0x0221914C, void, this);
    u32 a = A(this);
    if (gabi::call<u32>(0x02055B64, a + 0xCF8) != 0) return; /* cLib_calcTimer(&field_0x954) */
    u32 pattern = m_head_tex_pattern.v;
    s32 frameMax = gabi::call_ptr<s32>(load<u32>(load<u32>(pattern + 4) + 0x14), pattern);
    if ((s32)(u32)mBtpFrame < frameMax) {
        mBtpFrame = (u8)(mBtpFrame + 1);
        return;
    }
    pattern = m_head_tex_pattern.v;
    frameMax = gabi::call_ptr<s32>(load<u32>(load<u32>(pattern + 4) + 0x14), pattern);
    mBtpFrame = (u8)(mBtpFrame - (u32)frameMax);
    f32 random = gabi::call<f32>(0x020198D8, 100.0f);
    mBlinkTimer = (s16)gabi::ftoi(gabi::fadds_ppc(random, 30.0f));
}
VERIFY(0x0221914C, &daNpc_Btsw_c::playTexPatternAnm);

/* 02219210: daNpc_Btsw_c::checkOrder (unnamed by the matcher) */
void daNpc_Btsw_c::checkOrder() {
    WWHD_FUNC(0x02219210, void, this);
    u16 command = load<u16>(A(this) + 0xF8);
    if (command == 2) { /* eventInfo.checkCommandDemoAccrpt() */
        s8 order = mOrder;
        if (order == 3) {
            mOrder = 0;
            btsw_setAction(this, BTSW_getdemo_action);
        } else if (order == 4) {
            mOrder = 0;
            btsw_setAction(this, BTSW_dummy_event_action);
        }
        return;
    }
    if (command != 1) return; /* eventInfo.checkCommandTalk() */
    s8 order = mOrder;
    if (order != 1 && order != 2) return;
    mOrder = 0;
    mTalkAccepted = 1;
}
VERIFY(0x02219210, &daNpc_Btsw_c::checkOrder);

/* 0221949C */
void SwCam2_c::Move() {
    WWHD_FUNC(0x0221949C, void, this);
    u32 c = A(this);
    if (mActive == 0) return;
    /* camera_process_class* camera = dComIfGp_getCamera(dComIfGp_getPlayerCameraID(0)) */
    s32 camId = load<s8>(btsw_play() + 0x5B30);
    u32 play = btsw_play();
    s8 aimY = mAimY;
    s8 aimX = mAimX;
    u32 camera = load<u32>(play + (u32)(camId * 0x34) + 0x5AF8);
    /* cXyz center = camera_center_data[field_0x1D][field_0x1C + 1] */
    u32 src = 0x10466A58 + (u32)(aimY * 0x24) + (u32)(aimX * 0xC);
    gabi::Local<cXyz> center;
    f32 cy = load<f32>(src + 0x10), cx = load<f32>(src + 0xC), cz = load<f32>(src + 0x14);
    center->x = cx;
    center->z = cz;
    center->y = cy;
    gabi::call(0x0200EE00, c, center.get(), 0.25f, 10.0f, 1.0f); /* cLib_addCalcPos(&field_0x00, center, ...) */
    gabi::call(0x02514F44, camera + 0x248);                      /* camera->mCamera.Stay() */
    gabi::Local<cXyz> eye;
    gabi::Local<cXyz> ctr;
    f32 e1 = load<f32>(c + 4), c0 = load<f32>(c + 0xC);
    eye->y = e1;
    f32 c1 = load<f32>(c + 0x10);
    ctr->x = c0;
    f32 e0 = load<f32>(c + 0);
    ctr->y = c1;
    eye->x = e0;
    f32 c2 = load<f32>(c + 0x14), e2 = load<f32>(c + 8);
    f32 fovy = mFovy;
    eye->z = e2;
    ctr->z = c2;
    gabi::call(0x02514F88, camera + 0x248, eye.get(), ctr.get(), fovy, 0); /* camera->mCamera.Set(*getEyeP(), *getCenterP(), field_0x18, 0) */
}
VERIFY(0x0221949C, &SwCam2_c::Move);

/* 022195A4 */
void daNpc_Btsw_c::eventOrder() {
    WWHD_FUNC(0x022195A4, void, this);
    s8 order = mOrder;
    if (order == 3) {
        gabi::call(0x025D77DC, this, 0x100188C0u /* "GETMOTHERLETTER" */, 1, 0xFFFF); /* fopAcM_orderOtherEvent */
    } else if (order == 4) {
        gabi::call(0x025D7A58, this, (s32)mEventIdx, 0xFF, 0xFFFF, 0, 1); /* fopAcM_orderOtherEventId */
    } else if (order == 1 || order == 2) {
        s8 o = mOrder;
        u32 a = A(this);
        store<u16>(a + 0xFA, (u16)(load<u16>(a + 0xFA) | 0x21)); /* onCondition(CANTALK | CANTALKITEM) */
        if (o == 1) gabi::call(0x025D76A8, this); /* fopAcM_orderSpeakEvent */
    }
}
VERIFY(0x022195A4, &daNpc_Btsw_c::eventOrder);

/* 02219618 */
BOOL daNpc_Btsw_c::_execute() {
    WWHD_FUNC(0x02219618, BOOL, this);
    u32 a = A(this);
    const u32 h = BTSW_HIO;
    /* m_jnt.setParam(l_HIO.mNpc...) */
    s16 p1 = load<s16>(h + 0xA), p6 = load<s16>(h + 0xC), p2 = load<s16>(h + 0xE), p3 = load<s16>(h + 0x12);
    s16 p7 = load<s16>(h + 0x10), p4 = load<s16>(h + 0x16), p8 = load<s16>(h + 0x14), p5 = load<s16>(h + 8);
    s16 p9 = load<s16>(h + 0x18);
    gabi::call(0x0259E08C, a + 0x3AC, (s32)p1, (s32)p2, (s32)p3, (s32)p4, (s32)p5, (s32)p6, (s32)p7, (s32)p8, (s32)p9);
    gabi::call(0x025885C4, a + 0xD1C, 5, 2, 3, 2, 0, 0x2000, load<f32>(h + 0x4C), load<f32>(h + 0x50)); /* setWaitParm */
    playTexPatternAnm();
    gabi::call(0x025E535C, mpMorf.v.get(), a + 0x37C, 0, 0); /* mpMorf->play(&eyePos, 0, 0) */
    gabi::call(0x025E55A0, mpMorf.v.get());                  /* mpMorf->calc() */
    checkOrder();
    btsw_actionCall(this, 0); /* (this->*mCurrActionFunc)(NULL) */
    gabi::call(0x0221949C, a + 0xCD4); /* mSwCam.Move() */
    eventOrder();
    gabi::call(0x025D6870, a, a + 0x654); /* fopAcM_posMoveF(this, mStts.GetCCMoveP()) */
    u32 play = btsw_play();
    gabi::call(0x024F08A8, a + 0x450, play + 0x12A0); /* mObjAcch.CrrPos(*dComIfG_Bgsp()) */
    play = btsw_play();
    store<u8>(a + 0x1C9, (u8)gabi::call<u32>(0x024EF130, play + 0x12A0, a + 0x538)); /* tevStr.mRoomNo = GetRoomId(m_gnd) */
    play = btsw_play();
    store<u8>(a + 0x1CA, (u8)gabi::call<u32>(0x024EEEB8, play + 0x12A0, a + 0x538)); /* mEnvrIdxOverride = GetPolyColor(m_gnd) */
    set_mtx();
    gabi::call(0x025A15AC, a, 60.0f, 150.0f); /* setCollision */
    return TRUE;
}
VERIFY(0x02219618, &daNpc_Btsw_c::_execute);

/* 022197C4 */
static BOOL daNpc_Btsw_Execute(daNpc_Btsw_c* i_this) {
    WWHD_FUNC(0x022197C4, BOOL, i_this);
    return i_this->_execute();
}
VERIFY(0x022197C4, daNpc_Btsw_Execute);

/* 022197C8 */
void SwMail2_c::draw(void* tevStr) {
    WWHD_FUNC(0x022197C8, void, this, tevStr);
    u32 m = A(this);
    u32 modelData = load<u32>(load<u32>(m + 8) + 0xAC);
    gabi::call(0x025E7B3C, m + 0xC, modelData, (u32)mNo); /* field_0x10.entry(modelData, field_0x54) */
    u32 env = gabi::call<u32>(0x02555D0C);                 /* g_env_light */
    gabi::call(0x02562F5C, env, load<u32>(m + 8), tevStr); /* setLightTevColorType(mpModel, tevStr) */
    gabi::call(0x025E2DE0, load<u32>(m + 8), 0);           /* mDoExt_modelUpdateDL(mpModel) */
    store<u32>(modelData + 0x38, 0);                       /* field_0x10.remove(modelData) */
}
VERIFY(0x022197C8, &SwMail2_c::draw);

/* 02219840 */
BOOL daNpc_Btsw_c::_draw() {
    WWHD_FUNC(0x02219840, BOOL, this);
    u32 a = A(this);
    u32 model = load<u32>(load<u32>(a + 0x44C) + 0x90);
    u32 modelData = load<u32>(model + 0xAC);
    u32 env = gabi::call<u32>(0x02555D0C);
    gabi::call(0x025626A4, env, 0, a + 0x314, a + 0x110); /* settingTevStruct(TEV_TYPE_ACTOR, &current.pos, &tevStr) */
    env = gabi::call<u32>(0x02555D0C);
    gabi::call(0x02562F5C, env, model, a + 0x110);        /* setLightTevColorType(model, &tevStr) */
    gabi::call(0x025E7B3C, a + 0xA18, modelData, (u32)mBtpFrame); /* field_0x7E8.entry */
    gabi::call(0x025E5590, mpMorf.v.get());               /* mpMorf->entryDL() */
    s8 anm = mAnm;
    if (anm == 10 || anm == 9) {
        env = gabi::call<u32>(0x02555D0C);
        gabi::call(0x025626A4, env, 0, a + 0x314, a + 0x7EC);
        /* field_0x6D4.mColorK0 / mColorC0 */
        store<s16>(a + 0x880, 0xC0);
        store<u8>(a + 0x885, 0xAE);
        store<u8>(a + 0x886, 0xC0);
        store<s16>(a + 0x87E, 0xAE);
        store<u8>(a + 0x884, 0xC0);
        store<s16>(a + 0x87C, 0xC0);
        env = gabi::call<u32>(0x02555D0C);
        gabi::call(0x02562F5C, env, mpLetterModel.v.get(), a + 0x7EC);
        gabi::call(0x025E2DE0, mpLetterModel.v.get(), 0); /* mDoExt_modelUpdateDL(field_0x6D0) */
    }
    store<u32>(modelData + 0x38, 0); /* field_0x7E8.remove(modelData) */
    if (mMailActive != 0) {
        for (s32 i = 0; i < 3; i++) btsw_mailCall2(MAIL_draw, load<u32>(a + 0xCC4 + i * 4), a + 0x110);
    }
    gabi::call(0x025BD338, (u32)mpShopCursor); /* mpShopCursor->draw() */
    gabi::call(0x025BEBB8, 0x98 /* DSNAP_TYPE_NPC_BTSW */, a, a + 0x314, (s32)load<s16>(a + 0x322), 1.0f, 1.0f, 1.0f);
    /* HD: function-local statics (the GameCube's unused GXColor table), initialised on first use
     * behind an HIO flag */
    if (load<u8>(BTSW_HIO + 0x22) != 0) {
        if (load<u32>(0x101FDA50) == 0) {
            store<u32>(0x101FDA50, 1);
            gabi::call(0xC000A848, /* memcpy */ 0x101FEBF4u, 0x100186B0u, 4);
        }
        if (load<u32>(0x101FDAC0) == 0) {
            store<u32>(0x101FDAC0, 1);
            gabi::call(0xC000A848, /* memcpy */ 0x101FEBF8u, 0x100186B4u, 4);
        }
    }
    return TRUE;
}
VERIFY(0x02219840, &daNpc_Btsw_c::_draw);

/* 022199F8 */
static BOOL daNpc_Btsw_Draw(daNpc_Btsw_c* i_this) {
    WWHD_FUNC(0x022199F8, BOOL, i_this);
    return i_this->_draw();
}
VERIFY(0x022199F8, daNpc_Btsw_Draw);

/* 022199FC */
void SwMail2_c::set_mtx_throw() {
    WWHD_FUNC(0x022199FC, void, this);
    u32 m = A(this);
    f32 x = gabi::fadds_ppc(mPos.x, mOffset.x);
    f32 y = gabi::fadds_ppc(mPos.y, mOffset.y);
    f32 z = gabi::fadds_ppc(mPos.z, mOffset.z);
    gabi::call(0x028E93CC, 0x1048D0CCu, x, y, z);       /* transS */
    gabi::call(0x025F1BF4, 0x1048D0CCu, (s32)mAngle.x); /* XrotM */
    gabi::call(0x025F1C28, 0x1048D0CCu, (s32)mAngle.y); /* YrotM */
    mtx_copy(gabi::at<Mtx34>(load<u32>(m + 8) + 0xC8), gabi::at<Mtx34>(0x1048D0CC));
}
VERIFY(0x022199FC, &SwMail2_c::set_mtx_throw);

/* 02219ADC: SwMail2_c::move (unnamed by the matcher): (this->*mFunc)() */
void SwMail2_c::move() {
    WWHD_FUNC(0x02219ADC, void, this);
    u32 m = A(this);
    s16 i = mFunc.i;
    s16 d = mFunc.d;
    u32 recv = m + (u32)(s32)d;
    u32 target;
    if (i < 0) {
        target = mFunc.f;
    } else {
        u32 vtbl = load<u32>(recv + (u32)(s32)load<s16>(m + 6));
        target = load<u32>(vtbl + (u32)((s32)i * 8) + 4);
    }
    gabi::call_ptr(target, recv);
}
VERIFY(0x02219ADC, &SwMail2_c::move);

/* 02219B20: SwMail2_c::Dummy (unnamed by the matcher) */
void SwMail2_c::Dummy() {
    WWHD_FUNC(0x02219B20, void, this);
    set_mtx();
}
VERIFY(0x02219B20, &SwMail2_c::Dummy);

/* 02219B24 */
void SwMail2_c::AppearInit() {
    WWHD_FUNC(0x02219B24, void, this);
    u32 m = A(this);
    gabi::call<s16>(0x0200F93C, load<u32>(m + 0xB4), load<u32>(m + 0xB8)); /* cLib_targetAngleY (unused) */
    u32 c = load<u32>(m + 0xB8);
    u8 no = mNo;
    mAngle.y = -0x4000;
    mAngle.x = 0;
    mAngle.z = 0;
    f32 cy = load<f32>(c + 4), cx = load<f32>(c), cz = load<f32>(c + 8);
    mPos.x = cx;
    mPos.y = gabi::fsubs_ppc(cy, 200.0f);
    mPos.z = cz;
    no = getNextNo(no);
    mBaseAngle.z = -0x4000;
    mBaseAngle.x = 0;
    mBaseAngle.y = -0x4000;
    mNo = no;
    mOffset.copy(*gabi::at<cXyz>(0x101FFBA8));
    mStep = 0;
    btsw_seStart(0x69A5); /* JA_SE_LETTER_GAME_NEW */
    btsw_mailSetProc(m, BTSW_MAIL_Appear);
}
VERIFY(0x02219B24, &SwMail2_c::AppearInit);

/* 02219BFC: SwMail2_c::WaitInit (unnamed by the matcher) */
void SwMail2_c::WaitInit() {
    WWHD_FUNC(0x02219BFC, void, this);
    btsw_mailSetProc(A(this), BTSW_MAIL_Wait);
}
VERIFY(0x02219BFC, &SwMail2_c::WaitInit);

/* 02219C1C */
void SwMail2_c::Appear() {
    WWHD_FUNC(0x02219C1C, void, this);
    u32 m = A(this);
    s16 new_y = gabi::call<s16>(0x0200F93C, load<u32>(m + 0xB4), load<u32>(m + 0xB8));
    /* cXyz diff = *field_0x58 - *field_0x5C */
    gabi::Local<cXyz> diff;
    u32 rhs = load<u32>(m + 0xB8);
    gabi::call(0x0201ADE0, load<u32>(m + 0xB4), diff.get(), rhs);
    BOOL ok = gabi::call<BOOL>(0x0201B47C, diff.get()); /* normalizeRS */
    u32 c = load<u32>(m + 0xB8);
    if (!ok) {
        diff->x = 0.0f;
        diff->z = 1.0f;
        diff->y = 0.0f;
    }
    gabi::Local<cXyz> vec; /* cXyz vec = *field_0x5C */
    vec->copy(*gabi::at<cXyz>(c));
    gabi::Local<cXyz> scaled;
    gabi::call(0x0201AE48, diff.get(), scaled.get(), 120.0f); /* diff * 120.0f */
    diff->copy(*scaled.get());
    gabi::call(0x028E8D88, vec.get(), diff.get(), vec.get()); /* vec += diff */
    s16 bx = mBaseAngle.x, by = mBaseAngle.y;
    f32 vy = gabi::fsubs_ppc(vec->y, 40.0f);
    s16 bz = mBaseAngle.z;
    mAngle.x = bx;
    mAngle.y = (s16)(by + new_y);
    vec->y = vy;
    gabi::call(0x0200F428, m + 0xA8, (s32)bz, 4, 0x1000); /* cLib_addCalcAngleS2(&field_0x48.z, field_0x4E.z, 4, 0x1000) */
    f32 step = gabi::call<f32>(0x0200EE00, m + 0x80, vec.get(), 0.25f, 30.0f, 2.5f);
    if (std::fabs(step) < 2.5f) WaitInit();
    set_mtx();
}
VERIFY(0x02219C1C, &SwMail2_c::Appear);

/* 02219D88 */
void SwMail2_c::Wait() {
    WWHD_FUNC(0x02219D88, void, this);
    u32 m = A(this);
    s16 new_y = gabi::call<s16>(0x0200F93C, load<u32>(m + 0xB4), load<u32>(m + 0xB8));
    gabi::Local<cXyz> diff;
    u32 rhs = load<u32>(m + 0xB8);
    gabi::call(0x0201ADE0, load<u32>(m + 0xB4), diff.get(), rhs);
    BOOL ok = gabi::call<BOOL>(0x0201B47C, diff.get());
    u32 c = load<u32>(m + 0xB8);
    if (!ok) {
        diff->x = 0.0f;
        diff->z = 1.0f;
        diff->y = 0.0f;
    }
    mPos.copy(*gabi::at<cXyz>(c)); /* field_0x24 = *field_0x5C */
    gabi::Local<cXyz> scaled;
    gabi::call(0x0201AE48, diff.get(), scaled.get(), 120.0f);
    diff->copy(*scaled.get());
    gabi::call(0x028E8D88, m + 0x80, diff.get(), m + 0x80); /* field_0x24 += diff */
    s16 by = mBaseAngle.y;
    f32 py = mPos.y;
    s16 bz = mBaseAngle.z;
    s16 bx = mBaseAngle.x;
    mAngle.y = (s16)(by + new_y);
    mAngle.z = bz;
    mAngle.x = bx;
    mPos.y = gabi::fsubs_ppc(py, 40.0f);
    set_mtx();
}
VERIFY(0x02219D88, &SwMail2_c::Wait);

/* 02219EA0 */
void SwMail2_c::ThrowInit(cXyz* aim, u8 no) {
    WWHD_FUNC(0x02219EA0, void, this, aim, no);
    u32 m = A(this), p = A(aim);
    store<u32>(m + 0x8C, load<u32>(p));
    store<u32>(m + 0x90, load<u32>(p + 4));
    u32 z = load<u32>(p + 8);
    mFunc.d = 0;
    store<u32>(m + 0x94, z);
    mStep = 0;
    mFunc.i = -1;
    mFunc.f = BTSW_MAIL_Throw;
    mAimNo = no;
}
VERIFY(0x02219EA0, &SwMail2_c::ThrowInit);

/* 02219EE0 */
void SwMail2_c::EndInit() {
    WWHD_FUNC(0x02219EE0, void, this);
    u32 m = A(this);
    /* field_0x30 = cXyz::Zero; field_0x30.z = -10.0f */
    store<u32>(m + 0x8C, load<u32>(0x101FFBA8));
    u32 y = load<u32>(0x101FFBAC);
    mSpeed.z = -10.0f;
    store<u32>(m + 0x90, y);
    btsw_mailSetProc(m, BTSW_MAIL_End);
}
VERIFY(0x02219EE0, &SwMail2_c::EndInit);

/* 02219F20 */
void SwMail2_c::Throw() {
    WWHD_FUNC(0x02219F20, void, this);
    u32 m = A(this);
    u32 center = load<u32>(m + 0xB8);
    u8 step = mStep;
    /* HD: x_angle computed once before the branch */
    s16 x_angle = gabi::call<s16>(0x0200F974, m + 0x8C, center); /* cLib_targetAngleX(&field_0x30, field_0x5C) */
    if (step == 0) {
        u32 rhs = load<u32>(m + 0xB8);
        s16 y_angle = gabi::call<s16>(0x0200F93C, load<u32>(m + 0xB4), rhs);
        gabi::Local<cXyz> vec;
        vec->z = 0.0f;
        vec->x = -40.0f;
        vec->y = -50.0f;
        gabi::call(0x025F1AA4, 0x1048D0CCu, (s32)x_angle, (s32)y_angle, 0); /* ZXYrotS */
        gabi::Local<cXyz> multVec;
        gabi::call(0x028E8F64, 0x1048D0CCu, vec.get(), multVec.get());     /* multVec */
        gabi::call(0x028E8D88, multVec.get(), load<u32>(m + 0xB8), multVec.get()); /* multVec += *field_0x5C */
        f32 r = gabi::call<f32>(0x0200EE00, m + 0x80, multVec.get(), 0.5f, 30.0f, 2.0f);
        if (r < 1.0f) {
            mAngle.y = 0;
            mBaseAngle.x = 0;
            btsw_seStart(0x2800); /* JA_SE_LK_SW_KAZEKIRI_S */
            mStep = (u8)(mStep + 1);
        }
        set_mtx();
        return;
    }
    u8 no = mNo;
    s16 ay = mAngle.y;
    mAngle.z = 0;
    mAngle.x = (s16)-x_angle;
    mAngle.y = (s16)(ay + (s32)no * 0x80 + 0x1000);
    f32 pos_step = gabi::call<f32>(0x0200EE00, m + 0x80, m + 0x8C, 0.5f, load<f32>(BTSW_HIO + 0x40), 1.0f);
    s16 bx = (s16)(mBaseAngle.x + 4000);
    mBaseAngle.x = bx;
    f32 cosv = load<f32>(0x104A44FC + ((u32)(u16)bx >> 3) * 8); /* cM_scos */
    mOffset.y = gabi::fmuls_ppc(20.0f, gabi::fsubs_ppc(1.0f, cosv));
    if (pos_step < 1.0f) {
        gabi::call(0x025E19CC, 0x69A6, m + 0x80); /* mDoAud_seStart(JA_SE_LETTER_IN_BOX, &field_0x24) */
        u8 aim = mAimNo;
        u8 cur = mNo;
        u32 p = btsw_play() + 0x12A0;
        if (aim == cur) {
            /* dComIfGp_plusMiniGameRupee(1) (HD: clamped at 0) */
            s16 rupee = load<s16>(p + 0x4A4C);
            if ((s32)rupee + 1 > 0) store<s16>(p + 0x4A4C, (s16)(load<s16>(p + 0x4A4C) + 1));
            else store<s16>(p + 0x4A4C, 0);
            btsw_seStart(0x8A5); /* JA_SE_MINIGAME_RIGHT */
        } else {
            gabi::Local<cXyz> dir;
            dir->x = 0.0f;
            dir->y = 1.0f;
            dir->z = 0.0f;
            gabi::call(0x025CB374, p + 0x46FC, 3, 9, dir.get()); /* dComIfGp_getVibration().StartShock */
            btsw_seStart(0x8A6); /* JA_SE_MINIGAME_WRONG */
        }
        EndInit();
    }
    set_mtx_throw();
}
VERIFY(0x02219F20, &SwMail2_c::Throw);

/* 0221A1E0 */
void SwMail2_c::End() {
    WWHD_FUNC(0x0221A1E0, void, this);
    u32 m = A(this);
    u8 no = mNo;
    u8 aim = mAimNo;
    if (aim == no) {
        gabi::call(0x0200F428, m + 0xA6, 0, 4, 0x1000);
        gabi::Local<cXyz> target;
        f32 zx = load<f32>(0x101FFBA8), zz = load<f32>(0x101FFBB0), zy = load<f32>(0x101FFBAC);
        target->x = zx;
        target->z = zz;
        target->y = zy;
        gabi::call(0x0200F164, m + 0x98, target.get(), 0.25f, 5.0f); /* cLib_addCalcPos2 */
        set_mtx_throw();
        return;
    }
    gabi::call(0x0200F428, m + 0xA6, -0x8000, 4, 0x1000);
    gabi::Local<cXyz> target;
    f32 zx = load<f32>(0x101FFBA8), zy = load<f32>(0x101FFBAC), zz = load<f32>(0x101FFBB0);
    target->x = zx;
    target->y = zy;
    target->z = zz;
    gabi::call(0x0200F164, m + 0x98, target.get(), 0.25f, 5.0f);
    if (mPos.z < 995.0f) {
        f32 y = mPos.y;
        s16 targetAngle;
        if (y > 720.0f) {
            mSpeed.y = gabi::fsubs_ppc(mSpeed.y, 3.0f);
            targetAngle = -0x4000;
        } else {
            f32 sy = mSpeed.y, sz = mSpeed.z;
            f32 ny = gabi::fmuls_ppc(sy, 0.6f);
            f32 sx = mSpeed.x;
            f32 nz = gabi::fmuls_ppc(sz, 0.9f);
            mSpeed.y = ny;
            mSpeed.x = gabi::fmuls_ppc(sx, 0.9f);
            mSpeed.z = nz;
            targetAngle = 0;
        }
        if (!(mPos.y > 700.0f)) {
            mPos.y = 700.0f;
            mSpeed.y = 0.0f;
        }
        gabi::call(0x0200F428, m + 0xA4, (s32)targetAngle, 2, 0x800);
    }
    gabi::call(0x028E8E64, m + 0x8C, m + 0x8C, 0.9f);      /* field_0x30 *= 0.9f */
    gabi::call(0x028E8D88, m + 0x80, m + 0x8C, m + 0x80); /* field_0x24 += field_0x30 */
    set_mtx_throw();
}
VERIFY(0x0221A1E0, &SwMail2_c::End);

/* 0221A3CC */
void daNpc_Btsw_c::setAnm(s8 idx) {
    WWHD_FUNC(0x0221A3CC, void, this, idx);
    u32 a = A(this);
    s8 cur = mAnm;
    u32 i4 = (u32)((s32)idx * 4);
    f32 morf = load<f32>(0x101BD710 + i4); /* a_morf_frame_tbl[idx] */
    if (cur == 6 || cur == 5) morf = load<f32>(BTSW_HIO + 0x58); /* l_HIO.field_0x5C */
    if ((s32)idx == (s32)cur) return;
    mAnm = idx;
    /* dNpc_setAnmIDRes(mpMorf, a_play_mode_tbl[idx], morf, a_play_speed_tbl[idx], l_bck_ix_tbl[idx], -1, "Btsw") */
    gabi::call(0x0259D24C, load<u32>(a + 0x44C), load<u32>(0x101BD6E4 + i4), morf, load<f32>(0x101BD73C + i4),
               load<u32>(0x10018718 + i4), -1, 0x10018900u);
    s8 anm = mAnm;
    if (anm == 9 || anm == 10 || anm == 6 || anm == 5) {
        m_jnt.mbBackBoneLock = 1; /* onHeadLock(), onBackBoneLock() */
        m_jnt.mbHeadLock = 1;
    } else {
        m_jnt.mbHeadLock = 0;
        m_jnt.mbBackBoneLock = 0;
    }
}
VERIFY(0x0221A3CC, &daNpc_Btsw_c::setAnm);

/* 0221A49C */
bool daNpc_Btsw_c::chkAttention(cXyz* pos, s16 angle) {
    WWHD_FUNC(0x0221A49C, bool, this, pos, angle);
    u32 player = load<u32>(btsw_play() + 0x5B2C); /* dComIfGp_getPlayer(0) */
    f32 posZ = pos->z;
    f32 playerZ = load<f32>(player + 0x31C);
    f32 playerX = load<f32>(player + 0x314);
    f32 dz = gabi::fsubs_ppc(playerZ, posZ);
    f32 posX = pos->x;
    f32 maxAttnDistXZ = load<f32>(BTSW_HIO + 0x24);
    f32 dx = gabi::fsubs_ppc(playerX, posX);
    f32 sq = gabi::fmadds(dx, dx, gabi::fmuls_ppc(dz, dz));
    s32 maxAttnAngleY = load<s16>(BTSW_HIO + 0x20);
    f32 distXZ = gabi::call<f32>(0x028F4384, sq); /* std::sqrtf */
    s16 targetAngleY = gabi::call<s16>(0x020195B0, dx, dz); /* cM_atan2s */
    if (mHasAttention) {
        maxAttnDistXZ = gabi::fadds_ppc(maxAttnDistXZ, 40.0f);
        maxAttnAngleY += 0x71C; /* cM_deg2s(10.0f) */
    }
    s32 delta = (s16)(targetAngleY - angle);
    s32 absDelta = delta < 0 ? -delta : delta;
    return maxAttnAngleY > absDelta && maxAttnDistXZ > distXZ;
}
VERIFY(0x0221A49C, &daNpc_Btsw_c::chkAttention);

/* 0221A5C8 */
u16 daNpc_Btsw_c::next_msgStatus(be<u32>* pMsgNo) {
    WWHD_FUNC(0x0221A5C8, u16, this, pMsgNo);
    u32 msgNo = *pMsgNo;
    /* HD: mpCurrMsg->mSelectNum is read from a message manager global */
    u32 msgMgr = load<u32>(0x101F4B5C);
    switch (msgNo) {
    case 0x1A91:
    case 0x1A92:
    case 0x1A94:
    case 0x1A95:
    case 0x1A96:
    case 0x1A98:
    case 0x1A99:
    case 0x1A9A:
    case 0x1AA7:
    case 0x1AAD:
        *pMsgNo = msgNo + 1;
        break;
    case 0x1AB5:
        *pMsgNo = 0x1A93;
        break;
    case 0x1A93:
        if (load<u32>(msgMgr + 0x948) == 0) {
            btsw_onEventBit(0x2702);
            *pMsgNo = 0x1A94;
        } else {
            *pMsgNo = 0x1AA0;
        }
        break;
    case 0x1AA1:
        if (load<u32>(msgMgr + 0x948) == 0) {
            btsw_onEventBit(0x2702);
            *pMsgNo = 0x1A94;
        } else {
            *pMsgNo = 0x1AA2;
        }
        break;
    case 0x1AA8:
        *pMsgNo = load<u32>(msgMgr + 0x948) == 0 ? 0x1AA9 : 0x1AAA;
        break;
    case 0x1AA4:
    case 0x1AA5:
    case 0x1AA6:
        if (!btsw_isEventBit(0x2701)) {
            *pMsgNo = 0x1AA7;
        } else if (gabi::call<u32>(0x02586D24, 0xAC03)) { /* dLetter_isNoSend(LETTER_BAITOS_MOM) */
            *pMsgNo = 0x1AAB;
        } else if (!gabi::call<u32>(0x025B7D90, btsw_save() + 0xD4, 1)) { /* dComIfGs_isSymbol(dSymbol_DIN_e) */
            *pMsgNo = 0x1AB4;
        } else if (!gabi::call<u32>(0x02586E1C, 0xAC03)) { /* dLetter_isRead(LETTER_BAITOS_MOM) */
            *pMsgNo = 0x1AAD;
        } else if (!btsw_isEventBit(0x3104)) {
            *pMsgNo = 0x1AAC;
            btsw_onEventBit(0x3104);
        } else {
            *pMsgNo = 0x1AB3;
        }
        break;
    case 0x1AA9:
        *pMsgNo = 0x1A97;
        break;
    default:
        return 0x10; /* fopMsgStts_MSG_ENDS_e */
    }
    return 0xF; /* fopMsgStts_MSG_CONTINUES_e */
}
VERIFY(0x0221A5C8, &daNpc_Btsw_c::next_msgStatus);

/* 0221A840 */
u32 daNpc_Btsw_c::getMsg() {
    WWHD_FUNC(0x0221A840, u32, this);
    u32 previous = mNextMsgNo;
    if (previous != fpcM_ERROR_PROCESS_ID_e) {
        mNextMsgNo = fpcM_ERROR_PROCESS_ID_e;
        return previous;
    }
    if ((u32)(load<u8>(btsw_play() + 0x52B0) - 1) <= 3) return 0x1AA3; /* dComIfGp_event_chkTalkXY() */
    u32 msg;
    u32 save;
    if (!btsw_isEventBit(0x2704)) {
        btsw_onEventBit(0x2704);
        save = btsw_save();
        msg = load<u8>(save + 0x1C0) != 0 ? 0x1AB5 : 0x1A91; /* dComIfGs_getClearCount() */
    } else if (!btsw_isEventBit(0x2702)) {
        save = btsw_save();
        msg = 0x1AA1;
    } else {
        u32 reg = gabi::call<u32>(0x025B8BB0, btsw_save() + 0x644, 0xAB03); /* dComIfGs_getEventReg(UNK_AB03) */
        save = btsw_save();
        msg = reg < 3 ? 0x1AA5 : 0x1AA4;
    }
    gabi::call(0x025B8AF4, save + 0x644, 0xAB03, 0); /* dComIfGs_setEventReg(UNK_AB03, 0) */
    return msg;
}
VERIFY(0x0221A840, &daNpc_Btsw_c::getMsg);

/* 0221A960 */
void daNpc_Btsw_c::anmAtr(u16 i_msgStatus) {
    WWHD_FUNC(0x0221A960, void, this, i_msgStatus);
    u8 attr = load<u8>(btsw_play() + 0x5BC5); /* dComIfGp_getMesgAnimeAttrInfo() */
    if (attr <= 6) setAnm((s8)attr);
    u32 morf = mpMorf.v;
    f32 frame = (f32)load<s16>(morf + 0xA2) - 1.0f; /* getEndFrame() - 1.0f */
    if (gabi::call<u32>(0x027F2BF8, morf + 0x98, frame)) { /* checkFrame */
        s8 anm = mAnm;
        if (anm == 4 || anm == 6) setAnm(1);
    }
    store<u8>(btsw_play() + 0x5BC5, 0xFF);
}
VERIFY(0x0221A960, &daNpc_Btsw_c::anmAtr);

/* 0221AA18 */
void daNpc_Btsw_c::setAttention() {
    WWHD_FUNC(0x0221AA18, void, this);
    u32 a = A(this);
    f32 z = mAttPos.z, y = mAttPos.y;
    f32 offset = load<f32>(BTSW_HIO + 0x1C); /* l_HIO.mNpc.mAttnYOffset */
    f32 x = mAttPos.x;
    store<f32>(a + 0x398, z);
    store<f32>(a + 0x390, x);
    store<f32>(a + 0x394, gabi::fadds_ppc(y, offset));
}
VERIFY(0x0221AA18, &daNpc_Btsw_c::setAttention);

/* 0221AA40 */
void daNpc_Btsw_c::lookBack() {
    WWHD_FUNC(0x0221AA40, void, this);
    u32 a = A(this);
    s8 mode = mMode;
    f32 vx = 0.0f, vy = 0.0f, vz = 0.0f;
    u32 dstPos = 0;
    s16 desired_y_rot = load<s16>(a + 0x322);
    gabi::Local<cXyz> eye;
    gabi::Local<cXyz> vec2;
    if (mode == 1 || mode == 2) {
        if (mode == 2) m_jnt.mbTrn = 1; /* setTrn() */
        if (mHasAttention) {
            gabi::call(0x0259D54C, eye.get(), load<f32>(BTSW_HIO + 4)); /* dNpc_playerEyePos(l_HIO.mNpc.m04) */
            vec2->copy(*eye.get());
            dstPos = A(vec2.get());
            vx = load<f32>(a + 0x314);
            vy = load<f32>(a + 0x380); /* eyePos.y */
            vz = load<f32>(a + 0x31C);
        }
    }
    gabi::Local<cXyz> vec;
    s16 vel;
    if (m_jnt.mbTrn != 0) { /* trnChk() */
        gabi::call(0x0200F428, a + 0xD0E, (s32)load<s16>(BTSW_HIO + 0x1A), 4, 0x800); /* cLib_addCalcAngleS2(&field_0x96A, mMaxHeadTurnVel, 4, 0x800) */
        vel = mHeadTurnVel;
    } else {
        mHeadTurnVel = 0;
        vel = 0;
    }
    vec->x = vx;
    vec->y = vy;
    vec->z = vz;
    /* m_jnt.lookAtTarget(&current.angle.y, dstPos, vec, desired_y_rot, field_0x96A, true) */
    gabi::call(0x0259DED0, a + 0x3AC, a + 0x322, dstPos, vec.get(), (s32)desired_y_rot, (s32)vel, 1);
}
VERIFY(0x0221AA40, &daNpc_Btsw_c::lookBack);

/* 0221ABCC */
void daNpc_Btsw_c::wait01() {
    WWHD_FUNC(0x0221ABCC, void, this);
    if (mTalkAccepted) {
        mMode = 2;
        return;
    }
    u32 morf = mpMorf.v;
    f32 frame = (f32)load<s16>(morf + 0xA2) - 1.0f;
    if (gabi::call<u32>(0x027F2BF8, morf + 0x98, frame)) { /* mpMorf->checkFrame(getEndFrame() - 1.0f) */
        s8 count = (s8)(mWaitCount - 1);
        mWaitCount = count;
        if (count <= 0) {
            if (mAnm == 9) {
                mWaitCount = 1;
                setAnm(10);
            } else {
                f32 random = gabi::call<f32>(0x020198D8, 4.0f);
                mWaitCount = (s8)gabi::ftoi(gabi::fadds_ppc(random, 2.0f));
                setAnm(9);
            }
        }
    }
    if (mHasAttention) mOrder = 2;
}
VERIFY(0x0221ABCC, &daNpc_Btsw_c::wait01);

/* 0221ACDC */
void daNpc_Btsw_c::talk01() {
    WWHD_FUNC(0x0221ACDC, void, this);
    u32 a = A(this);
    if (mCurrMsgBsPcId == fpcM_ERROR_PROCESS_ID_e && (u32)(load<u8>(btsw_play() + 0x52B0) - 1) <= 3 /* chkTalkXY */ &&
        !gabi::call<u32>(0x02544950, btsw_play() + 0x52C4) /* dComIfGp_evmng_ChkPresentEnd() */)
        return;
    u32 talk_res = gabi::call<u32>(0x025A11EC, a, 1); /* talk(1) */
    /* HD: also for 0x1A97 */
    if (load<u8>(btsw_play() + 0x5BD2) == 1) { /* dComIfGp_checkMesgSendButton() */
        u32 msg = mCurrMsgNo;
        if (msg == 0x1A96 || msg == 0x1AA9 || msg == 0x1A97) {
            /* mSwCam.ActiveOn() */
            mSwCam.mActive = 1;
            mSwCam.field_0x00.copy(*gabi::at<cXyz>(0x10466A64)); /* camera_center_data[0][1] */
            mSwCam.field_0x0C.copy(*gabi::at<cXyz>(0x10466AA0)); /* camera_eye */
            mSwCam.mAimY = 0;
            mSwCam.mFovy = 58.0f;
            mSwCam.mAimX = 0;
        }
    }
    if (talk_res != 0x12 /* fopMsgStts_BOX_CLOSED_e */) return;
    mMode = 1;
    btsw_eventReset();
    setAnm(1);
    u32 msg = mCurrMsgNo;
    mWaitCount = 1;
    mTalkAccepted = 0;
    if (msg == 0x1A97) {
        btsw_setAction(this, BTSW_shiwake_game_action);
    } else if (msg == 0x1A9B) {
        mOrder = 3;
    }
}
VERIFY(0x0221ACDC, &daNpc_Btsw_c::talk01);

/* 0221AF2C */
BOOL daNpc_Btsw_c::wait_action(void* arg) {
    WWHD_FUNC(0x0221AF2C, BOOL, this, arg);
    u32 a = A(this);
    s8 status = mActionStatus;
    if (status == ACTION_STARTING) {
        mMode = 1;
        mWaitCount = 1;
        setAnm(1);
        mActionStatus = (s8)(mActionStatus + 1);
        return TRUE;
    }
    if (status == ACTION_ENDING) return TRUE;
    f32 px = load<f32>(a + 0x314);
    s16 head_y = load<s16>(a + 0x3AE);
    gabi::Local<cXyz> pos;
    pos->x = px;
    s16 angle_y = load<s16>(a + 0x322);
    s16 backbone_y = load<s16>(a + 0x3B2);
    f32 pz = load<f32>(a + 0x31C);
    f32 py = load<f32>(a + 0x318);
    pos->z = pz;
    pos->y = py;
    u8 att = gabi::call<u8>(0x0221A49C, a, pos.get(), (s32)(s16)(angle_y + head_y + backbone_y)); /* chkAttention */
    s8 mode = mMode;
    mHasAttention = att;
    mOrder = 0;
    if (mode == 1) wait01();
    else if (mode == 2) talk01();
    lookBack();
    setAttention();
    return TRUE;
}
VERIFY(0x0221AF2C, &daNpc_Btsw_c::wait_action);

/* 0221B03C */
BOOL daNpc_Btsw_c::dummy_event_action(void* arg) {
    WWHD_FUNC(0x0221B03C, BOOL, this, arg);
    u32 a = A(this);
    s8 status = mActionStatus;
    if (status == ACTION_STARTING) {
        s8 st = mActionStatus;
        mMode = 2;
        mActionStatus = (s8)(st + 1);
        return TRUE;
    }
    if (status == ACTION_ENDING) return TRUE;
    /* dComIfGp_evmng_getMyStaffId(mEventCut.getActorName()) */
    u32 name = load<u32>(a + 0x3E0);
    u32 staffIdx = gabi::call<u32>(0x02542D88, btsw_play() + 0x52C4, name, 0, 0);
    if (!gabi::call<u32>(0x0259F858, a + 0x3E0)) { /* mEventCut.cutProc() */
        gabi::call(0x02543280, btsw_play() + 0x52C4, staffIdx); /* dComIfGp_evmng_cutEnd */
    }
    s16 angle_y = load<s16>(a + 0x322);
    s16 backbone_y = load<s16>(a + 0x3B2);
    gabi::Local<cXyz> pos;
    f32 px = load<f32>(a + 0x314), py = load<f32>(a + 0x318);
    s16 head_y = load<s16>(a + 0x3AE);
    pos->x = px;
    f32 pz = load<f32>(a + 0x31C);
    pos->y = py;
    pos->z = pz;
    u8 att = gabi::call<u8>(0x0221A49C, a, pos.get(), (s32)(s16)(angle_y + head_y + backbone_y)); /* chkAttention */
    s16 eventIdx = mEventIdx;
    mHasAttention = att;
    if (gabi::call<u32>(0x025440C8, btsw_play() + 0x52C4, (s32)eventIdx)) { /* dComIfGp_evmng_endCheck(field_0x9C8) */
        mEventIdx = -1;
        mOrder = 3;
        btsw_eventReset();
        btsw_setAction(this, BTSW_wait_action);
    }
    lookBack();
    setAttention();
    return TRUE;
}
VERIFY(0x0221B03C, &daNpc_Btsw_c::dummy_event_action);

/* 0221B254 */
void daNpc_Btsw_c::TimerCountDown() {
    WWHD_FUNC(0x0221B254, void, this);
    s32 rest = btsw_getTimerRestTimeMs();
    s32 next = mNextTimerSe;
    if (next > rest) {
        if (next <= 10000) {
            btsw_seStart(0x8AD); /* JA_SE_MINIGAME_TIMER_30 */
            mNextTimerSe = mNextTimerSe - 1000;
        } else {
            btsw_seStart(0x8AE); /* JA_SE_MINIGAME_TIMER_10 */
            mNextTimerSe = mNextTimerSe - 10000;
        }
    }
}
VERIFY(0x0221B254, &daNpc_Btsw_c::TimerCountDown);

/* 0221B300: daNpc_Btsw_c::checkNextMailThrowOK (unnamed by the matcher) */
BOOL daNpc_Btsw_c::checkNextMailThrowOK() {
    WWHD_FUNC(0x0221B300, BOOL, this);
    u32 a = A(this);
    u8 mailIdx = mMailIdx;
    if (mailIdx < 2) mailIdx++;
    else mailIdx = 0;
    u32 mail = load<u32>(a + 0xCC4 + mailIdx * 4);
    if (btsw_mailCheckProc(mail, BTSW_MAIL_Dummy)) return TRUE;
    if (btsw_mailCheckProc(mail, BTSW_MAIL_End)) return TRUE;
    return FALSE;
}
VERIFY(0x0221B300, &daNpc_Btsw_c::checkNextMailThrowOK);

/* dComIfGp_endMiniGame(7) (HD inline) */
static void btsw_endMiniGame() {
    u32 p = btsw_play();
    u16 flags = load<u16>(p + 0x5CE8);
    store<u8>(p + 0x5CEA, 0);
    store<u8>(p + 0x5CEE, 0);
    store<u16>(p + 0x5CE8, (u16)(flags ^ 0x40));
    u32 g = load<u32>(0x101F8344);
    store<u8>(0x1047B09A, 0);
    gabi::call(0x026768E4, load<u32>(g + 0x178));
}
/* field_0x920[0..2]->move() */
static void btsw_moveMails(u32 a) {
    btsw_mailCall(MAIL_move, load<u32>(a + 0xCC4));
    btsw_mailCall(MAIL_move, load<u32>(a + 0xCC8));
    btsw_mailCall(MAIL_move, load<u32>(a + 0xCCC));
}

/* 0221B3A8 */
BOOL daNpc_Btsw_c::shiwake_game_action(void* arg) {
    WWHD_FUNC(0x0221B3A8, BOOL, this, arg);
    u32 a = A(this);
    /* static cXyz aim_pos_data[2][3] (0x104669B8) */
    if (load<u32>(0x10466A50) == 0) {
        const u32 d = 0x104669B8;
        store<u32>(0x10466A50, 1);
        static const f32 x[3] = {-40.0f, -139.0f, -240.0f};
        for (u32 r = 0; r < 2; r++)
            for (u32 c = 0; c < 3; c++) {
                u32 e = d + (r * 3 + c) * 0xC;
                store<f32>(e, x[c]);
                store<f32>(e + 4, r == 0 ? 772.0f : 855.0f);
                store<f32>(e + 8, 1035.0f);
            }
    }
    /* static cXyz cursor_pos_data[2][3] (0x10466A00) */
    if (load<u32>(0x10466A54) == 0) {
        const u32 d = 0x10466A00;
        static const f32 x[3] = {-52.0f, -140.0f, -227.0f};
        for (u32 r = 0; r < 2; r++)
            for (u32 c = 0; c < 3; c++) {
                u32 e = d + (r * 3 + c) * 0xC;
                store<f32>(e, x[c]);
                store<f32>(e + 4, r == 0 ? 804.0f : 889.0f);
                store<f32>(e + 8, 996.0f);
            }
        store<u32>(0x10466A54, 1);
    }
    const u32 h = BTSW_HIO;
    s8 status = mActionStatus;
    if (status == ACTION_STARTING) {
        u16 command = load<u16>(a + 0xF8);
        mCursorX = 0;
        mMode = 3;
        mCursorY = 0;
        if (command != 2) { /* !eventInfo.checkCommandDemoAccrpt() */
            gabi::call(0x025D77DC, a, 0x10018964u /* "SHIWAKEGAME" */, 1, 0xFFFF); /* fopAcM_orderOtherEvent */
            store<u16>(a + 0xFA, (u16)(load<u16>(a + 0xFA) | 2)); /* onCondition(dEvtCnd_UNK2_e) */
            return FALSE;
        }
        if (!mTimerCreated) {
            /* dTimer_createTimer(6, l_HIO.field_0x30, 3, 4, 221.0f, l_HIO.field_0x48, 32.0f, l_HIO.field_0x4C) */
            gabi::call(0x025C60F4, 6, (u32)load<u16>(h + 0x2C), 3, 4, 221.0f, load<f32>(h + 0x44), 32.0f, load<f32>(h + 0x48));
            mTimerCreated = 1;
            store<s16>(btsw_play() + 0x5CEC, 0); /* dComIfGp_setMiniGameRupee(0) */
            /* dComIfGp_startMiniGame(7) (HD inline) */
            u32 p = btsw_play();
            u16 flags = load<u16>(p + 0x5CE8);
            store<u8>(p + 0x5CEA, 7);
            store<u16>(p + 0x5CE8, (u16)(flags | 0x40));
            u32 g = load<u32>(0x101F8344);
            store<u8>(0x1047B09A, 1);
            gabi::call(0x02676900, load<u32>(g + 0x178));
        }
        if (load<u32>(btsw_play() + 0x5CF0) == 0) return FALSE; /* dComIfG_getTimerPtr() == NULL */
        /* dComIfG_TimerStart(6, l_HIO.field_0x32) (HD inline) */
        s16 time = load<s16>(h + 0x2E);
        if (load<s32>(btsw_play() + 0x5CFC) == 6) {
            u32 timer = load<u32>(btsw_play() + 0x5CF0);
            if (timer != 0) {
                if (time == 0) gabi::call(0x025C5864, timer);
                else gabi::call(0x025C6250, timer, (s32)time); /* dTimer_c::start */
            }
        }
        mMailActive = 1;
        btsw_mailCall(MAIL_init, load<u32>(a + 0xCC4));
        btsw_mailCall(MAIL_AppearInit, load<u32>(a + 0xCC4));
        btsw_mailCall(MAIL_init, load<u32>(a + 0xCC8));
        btsw_mailCall(MAIL_DummyInit, load<u32>(a + 0xCC8));
        btsw_mailCall(MAIL_init, load<u32>(a + 0xCCC));
        btsw_mailCall(MAIL_DummyInit, load<u32>(a + 0xCCC));
        s8 cy = mCursorY, cx = mCursorX;
        mMailIdx = 0;
        mActionStatus = (s8)(mActionStatus + 1);
        /* mpShopCursor->setPos(cursor_pos_data[field_0x9A1][1 + field_0x9A0]); show() */
        gabi::call(0x025BD31C, (u32)mpShopCursor, 0x10466A0Cu + (u32)((cy * 3 + cx) * 0xC));
        store<u8>(mpShopCursor + 0xB4, 1);
        mNextTimerSe = 20000;
        gabi::call(0x02618760, load<u32>(0x101F5088), 5, 5); /* HD: pad repeat settings for the game */
        return TRUE;
    }
    if (status == ACTION_ENDING) {
        mTimerCreated = 0;
        mMailActive = 0;
        gabi::call(0x02618774, load<u32>(0x101F5088)); /* HD: pad repeat reset */
        return TRUE;
    }
    if (btsw_getTimerRestTimeMs() > 0) {
        TimerCountDown();
        /* HD: field_0x978.checkTrigger() / check*Trigger() replaced by the pad's trigger words */
        u32 pad = load<u32>(0x101F5088);
        u32 hold = load<u32>(pad + 0x124);
        u32 trig = load<u32>(pad + 0x18) | load<u32>(pad + 0x20);
        if (hold & 0x00F00000) trig &= 0x00F00000;
        s8 cx = mCursorX;
        if (cx > -1 && (trig & 0x00440000)) { /* left */
            btsw_seStart(0x8B0); /* JA_SE_LETTER_GAME_CURSOR */
            mCursorX = (s8)(mCursorX - 1);
        } else if (cx < 1 && (trig & 0x00880000)) { /* right */
            btsw_seStart(0x8B0);
            mCursorX = (s8)(mCursorX + 1);
        }
        s8 cy = mCursorY;
        if (cy < 1 && (trig & 0x00110000)) { /* up */
            btsw_seStart(0x8B0);
            mCursorY = (s8)(mCursorY + 1);
        } else if (cy > 0 && (trig & 0x00220000)) { /* down */
            btsw_seStart(0x8B0);
            mCursorY = (s8)(mCursorY - 1);
        }
        if (btsw_mailCheckProc(load<u32>(a + 0xCC4 + (u32)mMailIdx * 4), BTSW_MAIL_Wait) &&
            load<u8>(load<u32>(btsw_play() + 0x5CF0) + 0x124) != 1 /* dComIfG_getTimerPtr()->getStatus() */ &&
            gabi::call<u32>(0x02007898, 0) /* CPad_CHECK_TRIG_A(0) */ && checkNextMailThrowOK()) {
            s8 col = mCursorY;
            s8 row = mCursorX;
            u32 mail = load<u32>(a + 0xCC4 + (u32)mMailIdx * 4);
            u32 src = 0x104669C4 + (u32)((col * 3 + row) * 0xC); /* aim_pos_data[field_0x9A1][field_0x9A0 + 1] */
            gabi::Local<cXyz> aim_pos;
            f32 ax = load<f32>(src), az = load<f32>(src + 8), ay = load<f32>(src + 4);
            aim_pos->z = az;
            aim_pos->x = ax;
            aim_pos->y = ay;
            gabi::call(MAIL_ThrowInit, mail, aim_pos.get(), (u32)(u8)(row - col * 3 + 4)); /* idx = row + 1 + (1 - col) * 3 */
            u8 mailIdx = mMailIdx;
            if (mailIdx < 2) {
                mMailIdx = (u8)(mailIdx + 1);
                btsw_seStart(0x8B1); /* JA_SE_LETTER_GAME_OK */
            } else {
                mMailIdx = 0;
                btsw_seStart(0x8B1);
            }
            btsw_mailCall(MAIL_AppearInit, load<u32>(a + 0xCC4 + (u32)mMailIdx * 4));
        }
        /* mSwCam.setAimIdx(field_0x9A0, field_0x9A1) */
        s8 cy2 = mCursorY;
        s8 cx2 = mCursorX;
        mSwCam.mAimY = cy2;
        mSwCam.mAimX = cx2;
        gabi::call(0x025BD31C, (u32)mpShopCursor, 0x10466A0Cu + (u32)((cy2 * 3 + cx2) * 0xC)); /* setPos */
        /* mpShopCursor->setScale(0.65f, 0.9f, 0.5f, 27.0f, 20.0f) */
        u32 cursor = mpShopCursor;
        store<f32>(cursor + 0xB0, 0.5f);
        store<f32>(cursor + 0xA8, 0.65f);
        store<f32>(cursor + 0xAC, 0.9f);
        store<f32>(cursor + 0x98, 27.0f);
        store<f32>(cursor + 0x9C, 20.0f);
        gabi::call(0x025BD290, (u32)mpShopCursor); /* anm_play */
        btsw_moveMails(a);
        return TRUE;
    }
    s8 st = mActionStatus;
    if (st == ACTION_ONGOING) {
        btsw_seStart(0x8AF); /* JA_SE_LETTER_GAME_TIMER_0 */
        u32 timer = load<u32>(btsw_play() + 0x5CF0);
        gabi::call(0x025C58A8, timer, (s32)load<s16>(h + 0x30)); /* dComIfG_getTimerPtr()->end(l_HIO.field_0x34) */
        store<u8>(mpShopCursor + 0xB4, 0); /* hide() */
        mActionStatus = (s8)(mActionStatus + 1);
        btsw_moveMails(a);
        return TRUE;
    }
    u32 timer = load<u32>(btsw_play() + 0x5CF0);
    if (st == ACTION_UNK_2) {
        if (gabi::call<u32>(0x025C6278, timer)) { /* deleteCheck */
            gabi::call(0x025C58D8, load<u32>(btsw_play() + 0x5CF0)); /* deleteRequest */
            mActionStatus = (s8)(mActionStatus + 1);
        }
        btsw_moveMails(a);
        return TRUE;
    }
    if (timer == 0) {
        u32 staff_id = gabi::call<u32>(0x02542D88, btsw_play() + 0x52C4, 0x1001894Cu /* "Btsw" */, 0, 0);
        gabi::call(0x02543280, btsw_play() + 0x52C4, staff_id); /* dComIfGp_evmng_cutEnd */
        btsw_eventReset();
        s16 rupees = load<s16>(btsw_play() + 0x5CEC); /* dComIfGp_getMiniGameRupee() */
        /* daNpc_Btsw_getGameEndMsg(rupees) (inline) */
        u32 msgNo;
        if (rupees == 0) msgNo = 0x1A9D;
        else if (rupees == 1) msgNo = 0x1A9E;
        else if (rupees < 25) msgNo = 0x1A9F;
        else msgNo = 0x1A98;
        mNextMsgNo = msgNo;
        if (rupees < 25) {
            mOrder = 1;
            btsw_setAction(this, BTSW_wait_action);
            mSwCam.mActive = 0; /* ActiveOff() */
            mEventIdx = -1;
        } else {
            mOrder = 4;
            u32 idx = gabi::call<u32>(0x02543F10, btsw_play() + 0x52C4, 0x10018954u /* "BTSW_TALK_SUGOI" */, 0xFF);
            mEventIdx = (s16)idx;
            btsw_setAction(this, BTSW_dummy_event_action);
            mSwCam.mActive = 0;
        }
        if (load<u8>(btsw_play() + 0x5CEA) == 7) btsw_endMiniGame(); /* dComIfGp_getMiniGameType() == 7 */
    }
    btsw_moveMails(a);
    return TRUE;
}
VERIFY(0x0221B3A8, &daNpc_Btsw_c::shiwake_game_action);

/* 0221BE68 */
BOOL daNpc_Btsw_c::getdemo_action(void* arg) {
    WWHD_FUNC(0x0221BE68, BOOL, this, arg);
    s8 status = mActionStatus;
    if (status == ACTION_STARTING) {
        mActionStatus = 1;
        return TRUE;
    }
    if (status == ACTION_ENDING) return TRUE;
    if (!gabi::call<u32>(0x0254457C, btsw_play() + 0x52C4, 0x10018970u /* "GETMOTHERLETTER" */)) return TRUE; /* dComIfGp_evmng_endCheck */
    btsw_eventReset();
    mOrder = 1;
    mNextMsgNo = 0x1A9C;
    btsw_onEventBit(0x2701);
    gabi::call(0x025B8AF4, btsw_save() + 0x644, 0x8AFF, 25); /* dComIfGs_setEventReg(UNK_8AFF, 25) */
    btsw_setAction(this, BTSW_wait_action);
    return TRUE;
}
VERIFY(0x0221BE68, &daNpc_Btsw_c::getdemo_action);

/* 0221C00C */
static void* daNpc_Btsw_HIO_ct(void* obj) {
    WWHD_FUNC(0x0221C00C, void*, obj);
    u32 h = gabi::ea(obj);
    if (h == 0) {
        h = gabi::call<u32>(0x0273AD10, 0x60);
        if (h == 0) return nullptr;
    }
    store<u32>(h + 0x5C, 0x10018708); /* vtable */
    gabi::call(0x0259DA18, h + 4);    /* dNpc_HIO_c::dNpc_HIO_c */
    /* mNpc */
    store<f32>(h + 0x04, 0.0f);   /* m04 */
    store<s16>(h + 0x08, 8000);   /* mMaxHeadX */
    store<s16>(h + 0x0A, 0);
    store<s16>(h + 0x0C, 2000);
    store<s16>(h + 0x0E, 8000);
    store<s16>(h + 0x10, -3000);
    store<s16>(h + 0x12, 0);
    store<s16>(h + 0x14, -2000);
    store<s16>(h + 0x16, -8000);
    store<s16>(h + 0x18, 2000);   /* mMaxTurnStep */
    store<s16>(h + 0x1A, 0x640);  /* mMaxHeadTurnVel */
    store<f32>(h + 0x1C, 40.0f);  /* mAttnYOffset */
    store<s16>(h + 0x20, 0x2000); /* mMaxAttnAngleY */
    store<u8>(h + 0x22, 0);       /* m22 */
    store<f32>(h + 0x24, 300.0f); /* mMaxAttnDistXZ */
    store<s16>(h + 0x2C, 0x1E);
    store<s16>(h + 0x2E, 0xF);
    store<s16>(h + 0x30, 0x3C);
    store<s16>(h + 0x32, 192); /* r_1, g_1, b_1, r_2, g_2, b_2 */
    store<s16>(h + 0x34, 174);
    store<s16>(h + 0x36, 192);
    store<s16>(h + 0x38, 192);
    store<s16>(h + 0x3A, 174);
    store<s16>(h + 0x3C, 192);
    store<f32>(h + 0x40, 40.0f);
    store<f32>(h + 0x44, 80.0f);
    store<f32>(h + 0x48, 50.0f);
    store<f32>(h + 0x4C, 0.8f);
    store<f32>(h + 0x50, 0.75f);
    store<u8>(h + 0x54, 0);
    store<f32>(h + 0x58, 10.0f);
    store<s8>(h + 0x00, -1); /* mNo */
    return gabi::at<void>(h);
}
VERIFY(0x0221C00C, daNpc_Btsw_HIO_ct);

/* 0221C158 */
static void __sinit_d_a_npc_btsw_cpp() {
    WWHD_FUNC(0x0221C158, void);
    store<u32>(0x1046693C, 0);
    store<u32>(0x10466938, 0);
    store<u32>(0x10466934, 0);
    store<u32>(0x10466930, 0);
    gabi::call(0x028F026C, 0x101BD768u); /* __register_global_object */
    store<f32>(0x10466914, load<f32>(0x10018998)); /* -pi */
    store<f32>(0x10466918, load<f32>(0x1001899C)); /* pi */
    gabi::call(0x028ED6F8, 0x1046692Cu);
    gabi::call(0x028F026C, 0x101BD774u);
    gabi::call(0x028EAB2C, 0x1046692Du);
    gabi::call(0x028F026C, 0x101BD780u);
    store<f32>(0x10466928, 10000.0f);
    store<f32>(0x1046691C, 50000.0f);
    store<f32>(0x10466920, 50000.0f);
    store<f32>(0x10466924, 10000.0f);
    gabi::call(0x0221C00C, BTSW_HIO); /* l_HIO */
    /* SwCam2_c::camera_center_data[2][3], SwCam2_c::camera_eye */
    static const f32 x[3] = {-104.0f, -139.0f, -173.0f};
    static const f32 z[2][3] = {{875.0f, 882.0f, 875.0f}, {868.0f, 875.0f, 868.0f}};
    for (u32 r = 0; r < 2; r++)
        for (u32 c = 0; c < 3; c++) {
            u32 e = 0x10466A58 + (r * 3 + c) * 0xC;
            store<f32>(e, x[c]);
            store<f32>(e + 4, r == 0 ? 805.0f : 825.0f);
            store<f32>(e + 8, z[r][c]);
        }
    store<f32>(0x10466AA0, -139.0f);
    store<f32>(0x10466AA4, 815.0f);
    store<f32>(0x10466AA8, 662.0f);
}
VERIFY(0x0221C158, __sinit_d_a_npc_btsw_cpp);

/* 0221C2D4: sead::SafeString deleting destructor (this TU's copy) */
static void SafeString_dtor(void* self, int flags) {
    WWHD_FUNC(0x0221C2D4, void, self, flags);
    if (self != nullptr && (flags & 1)) gabi::call(0x0273AF40, self); /* __dl */
}
VERIFY(0x0221C2D4, SafeString_dtor);

/* 0221C2E8 */
static BOOL daNpc_Btsw_IsDelete(daNpc_Btsw_c* i_this) {
    WWHD_FUNC(0x0221C2E8, BOOL, i_this);
    return TRUE;
}
VERIFY(0x0221C2E8, daNpc_Btsw_IsDelete);

/* 0221C2F0: daNpc_Btsw_c deleting destructor (HD virtual destructor) */
static void daNpc_Btsw_dtor(daNpc_Btsw_c* i_this, int flags) {
    WWHD_FUNC(0x0221C2F0, void, i_this, flags);
    u32 a = A(i_this);
    if (a == 0) return;
    gabi::call(0x025886D8, a + 0xD1C, 2); /* STControl::~STControl */
    gabi::call(0x02515A70, a + 0x690, 2); /* dCcD_Cyl::~dCcD_Cyl */
    gabi::call(0x02515860, a + 0x654, 2); /* dCcD_Stts::~dCcD_Stts */
    gabi::call(0x02018034, a + 0x628, 2); /* dBgS_AcchCir: cM3dGCir::~cM3dGCir */
    store<u32>(a + 0x470, 0x100186E8);    /* dBgS_ObjAcch vtables */
    store<u32>(a + 0x464, 0x100186F8);
    gabi::call(0x024EFD9C, a + 0x450, 0); /* dBgS_Acch::~dBgS_Acch */
    gabi::call(0x025D50BC, a, 0);         /* fopAc_ac_c::~fopAc_ac_c */
    if (flags & 1) gabi::call(0x0273AF40, a); /* __dl */
}
VERIFY(0x0221C2F0, daNpc_Btsw_dtor);

/* 0221C398: sead::SafeString::assureTerminated (this TU's copy; empty) */
static void SafeString_assureTerminated(void* self) {
    WWHD_FUNC(0x0221C398, void, self);
}
VERIFY(0x0221C398, SafeString_assureTerminated);
