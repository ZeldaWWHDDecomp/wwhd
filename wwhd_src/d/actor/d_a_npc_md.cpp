/**
 * d_a_npc_md.cpp (WWHD)
 * Player - Medli
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_md.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 *
 * The other functions are in d_a_npc_md_<part>.cpp (separate verification units).
 */
#include "d/actor/d_a_npc_md.h"

/* 022854CC */
void daNpc_Md_c::setNpcAction(ProcFunc_l* actionFunc, void* arg) {
    WWHD_FUNC(0x022854CC, void, this, actionFunc, arg);
    gabi::store<u8>(MD_M_FLYING, 0); /* offFlying() */
    mCurrPlayerActionFunc.f = 0;
    mCurrPlayerActionFunc.d = 0;
    mCurrPlayerActionFunc.i = 0;
    gabi::Local<ProcFunc_l> fn; /* by-value copy, two words */
    md_pmf_load(fn, gabi::ea(actionFunc));
    setAction(&mCurrNpcActionFunc, fn, arg);
}
VERIFY(0x022854CC, &daNpc_Md_c::setNpcAction);

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 025E1A7C mDoAud_monsSeStart(id, pos, procId, reverb) (fopAcM_monsSeStart inlined) */
static inline void mDoAud_monsSeStart_md(u32 id, cXyz* pos, u32 procId, s32 reverb) { gabi::call(0x025E1A7C, id, pos, procId, reverb); }
/* 0201B080 cXyz::outprod(const Vec&) const: result through a hidden pointer (r4) */
static inline void cXyz_outprod_md(const cXyz* a, cXyz* out, const cXyz* b) { gabi::call(0x0201B080, a, out, b); }

static inline u32 md_procId(fopAc_ac_c* a) { return gabi::load<u32>(gabi::ea(a) + 4); } /* base.base.mBsPcId */
static inline be<u32>& md_attnFlags(fopAc_ac_c* a) { return *gabi::at<be<u32>>(gabi::ea(a) + 0x39C); }

/* 0229216C */
BOOL daNpc_Md_c::deleteNpcAction(void*) {
    WWHD_FUNC(0x0229216C, BOOL, this, (void*)nullptr);
    if (mActionStatus == ACTION_STARTING) {
        mActionStatus = 1; /* ACTION_ONGOING_1 */
    } else if (mActionStatus != ACTION_ENDING) {
        fopAcM_delete(this);
    }
    return TRUE;
}
VERIFY(0x0229216C, &daNpc_Md_c::deleteNpcAction);

/* 02294C00 */
BOOL daNpc_Md_c::carryPlayerAction(void*) {
    WWHD_FUNC(0x02294C00, BOOL, this, (void*)nullptr);
    if (mActionStatus == ACTION_STARTING) {
        clearStatus(daMdStts_UNK2 | daMdStts_UNK4 | daMdStts_FLY);
        setAnm(0x24);
        setBitStatus(daMdStts_UNK1);
        setHane02Emitter();
        s32 reverb = dComIfGp_getReverb(current.roomNo);
        mDoAud_monsSeStart_md(0x48A5 /* JA_SE_CV_MD_LIFT_UP */, &current.pos, md_procId(this), reverb);
        m30F8 = 75.0f;
        mAcchCir[1].SetWall(60.0f, 20.0f);
        mActionStatus = mActionStatus + 1;
    } else if (mActionStatus == ACTION_ENDING) {
        deleteHane02Emitter();
    }
    return TRUE;
}
VERIFY(0x02294C00, &daNpc_Md_c::carryPlayerAction);

/* 02292650 */
f32 daNpc_Md_c::checkWallJump(s16 param_1) {
    WWHD_FUNC(0x02292650, f32, this, param_1);
    f32 temp = checkForwardGroundY(param_1) - current.pos.y;
    if (0.0f < temp && temp < 80.0f) {
        return std_sqrtf(temp) * 3.6f;
    }
    return -1.0f;
}
VERIFY(0x02292650, &daNpc_Md_c::checkWallJump);

/* 022926D0 */
void daNpc_Md_c::routeAngCheck(cXyz* param_1, be<s16>* param_2) {
    WWHD_FUNC(0x022926D0, void, this, param_1, param_2);
    gabi::Local<cXyz> temp;
    cXyz_outprod_md(&m32A4, temp, param_1);
    s16 angle = cM_atan2s(temp->x, temp->z);
    if ((!(m32A4.y < 1.0f) && cLib_distanceAngleS(angle, *param_2) > 0x4000) ||
        temp->y * fopAcM_searchPlayerDistanceY(this) < 0.0f) {
        angle += 0x8000;
    }
    *param_2 = angle;
}
VERIFY(0x022926D0, &daNpc_Md_c::routeAngCheck);

/* 02295434 HD: the destructor is called through the vtable by the framework */
static BOOL daNpc_Md_Delete(daNpc_Md_c* i_this) {
    WWHD_FUNC(0x02295434, BOOL, i_this);
    return TRUE;
}
VERIFY(0x02295434, daNpc_Md_Delete);

/* ---- compiler-generated: this TU's copies of the inline daPy_py_c / daPy_npc_c virtuals
 * (vtable 0x1001E088) and other empty inlines; written from the WWHD code ---- */
static void md_inline_022954F0(daNpc_Md_c* i_this) {
    WWHD_FUNC(0x022954F0, void, i_this);
}
VERIFY(0x022954F0, md_inline_022954F0);
static void md_inline_022954F4(daNpc_Md_c* i_this) {
    WWHD_FUNC(0x022954F4, void, i_this);
}
VERIFY(0x022954F4, md_inline_022954F4);
static void md_inline_022954F8(daNpc_Md_c* i_this) {
    WWHD_FUNC(0x022954F8, void, i_this);
}
VERIFY(0x022954F8, md_inline_022954F8);
static s32 md_inline_022954FC(daNpc_Md_c* i_this) {
    WWHD_FUNC(0x022954FC, s32, i_this);
    return -1;
}
VERIFY(0x022954FC, md_inline_022954FC);
static s32 md_inline_02295504(daNpc_Md_c* i_this) {
    WWHD_FUNC(0x02295504, s32, i_this);
    return 0;
}
VERIFY(0x02295504, md_inline_02295504);
static s32 md_inline_0229550C(daNpc_Md_c* i_this) {
    WWHD_FUNC(0x0229550C, s32, i_this);
    return 0;
}
VERIFY(0x0229550C, md_inline_0229550C);
static s32 md_inline_02295514(daNpc_Md_c* i_this) {
    WWHD_FUNC(0x02295514, s32, i_this);
    return 0;
}
VERIFY(0x02295514, md_inline_02295514);
static s32 md_inline_0229551C(daNpc_Md_c* i_this) {
    WWHD_FUNC(0x0229551C, s32, i_this);
    return 0;
}
VERIFY(0x0229551C, md_inline_0229551C);
static s32 md_inline_02295524(daNpc_Md_c* i_this) {
    WWHD_FUNC(0x02295524, s32, i_this);
    return 0;
}
VERIFY(0x02295524, md_inline_02295524);
static s32 md_inline_0229552C(daNpc_Md_c* i_this) {
    WWHD_FUNC(0x0229552C, s32, i_this);
    return 0;
}
VERIFY(0x0229552C, md_inline_0229552C);
static s32 md_inline_02295534(daNpc_Md_c* i_this) {
    WWHD_FUNC(0x02295534, s32, i_this);
    return 0;
}
VERIFY(0x02295534, md_inline_02295534);
static s32 md_inline_0229553C(daNpc_Md_c* i_this) {
    WWHD_FUNC(0x0229553C, s32, i_this);
    return 0;
}
VERIFY(0x0229553C, md_inline_0229553C);
static s32 md_inline_02295544(daNpc_Md_c* i_this) {
    WWHD_FUNC(0x02295544, s32, i_this);
    return 0;
}
VERIFY(0x02295544, md_inline_02295544);
static void md_inline_0229554C(daNpc_Md_c* i_this) {
    WWHD_FUNC(0x0229554C, void, i_this);
}
VERIFY(0x0229554C, md_inline_0229554C);
static s32 md_inline_02295550(daNpc_Md_c* i_this) {
    WWHD_FUNC(0x02295550, s32, i_this);
    return 0;
}
VERIFY(0x02295550, md_inline_02295550);
static s32 md_inline_02295558(daNpc_Md_c* i_this) {
    WWHD_FUNC(0x02295558, s32, i_this);
    return -1;
}
VERIFY(0x02295558, md_inline_02295558);
static s32 md_inline_02295560(daNpc_Md_c* i_this) {
    WWHD_FUNC(0x02295560, s32, i_this);
    return -1;
}
VERIFY(0x02295560, md_inline_02295560);
static s32 md_inline_02295568(daNpc_Md_c* i_this) {
    WWHD_FUNC(0x02295568, s32, i_this);
    return -1;
}
VERIFY(0x02295568, md_inline_02295568);
static s32 md_inline_02295570(daNpc_Md_c* i_this) {
    WWHD_FUNC(0x02295570, s32, i_this);
    return 0;
}
VERIFY(0x02295570, md_inline_02295570);
static s32 md_inline_02295578(daNpc_Md_c* i_this) {
    WWHD_FUNC(0x02295578, s32, i_this);
    return 0;
}
VERIFY(0x02295578, md_inline_02295578);
static s32 md_inline_02295580(daNpc_Md_c* i_this) {
    WWHD_FUNC(0x02295580, s32, i_this);
    return 0;
}
VERIFY(0x02295580, md_inline_02295580);
static s32 md_inline_02295588(daNpc_Md_c* i_this) {
    WWHD_FUNC(0x02295588, s32, i_this);
    return 0;
}
VERIFY(0x02295588, md_inline_02295588);
static void md_inline_02295590(daNpc_Md_c* i_this) {
    WWHD_FUNC(0x02295590, void, i_this);
}
VERIFY(0x02295590, md_inline_02295590);
static void md_inline_02295594(daNpc_Md_c* i_this) {
    WWHD_FUNC(0x02295594, void, i_this);
}
VERIFY(0x02295594, md_inline_02295594);
static void md_inline_02295598(daNpc_Md_c* i_this) {
    WWHD_FUNC(0x02295598, void, i_this);
}
VERIFY(0x02295598, md_inline_02295598);
static s32 md_inline_0229559C(daNpc_Md_c* i_this) {
    WWHD_FUNC(0x0229559C, s32, i_this);
    return 0;
}
VERIFY(0x0229559C, md_inline_0229559C);
static void md_inline_022955A4(daNpc_Md_c* i_this) {
    WWHD_FUNC(0x022955A4, void, i_this);
}
VERIFY(0x022955A4, md_inline_022955A4);
static void md_inline_022955A8(daNpc_Md_c* i_this) {
    WWHD_FUNC(0x022955A8, void, i_this);
}
VERIFY(0x022955A8, md_inline_022955A8);
static void md_inline_022955AC(daNpc_Md_c* i_this) {
    WWHD_FUNC(0x022955AC, void, i_this);
}
VERIFY(0x022955AC, md_inline_022955AC);
static s32 md_inline_022955B0(daNpc_Md_c* i_this) {
    WWHD_FUNC(0x022955B0, s32, i_this);
    return 0;
}
VERIFY(0x022955B0, md_inline_022955B0);
/* 022955B8 getGroundY(): mAcch.GetGroundH() */
static f32 md_getGroundY(daNpc_Md_c* i_this) {
    WWHD_FUNC(0x022955B8, f32, i_this);
    return i_this->mAcch.GetGroundH();
}
VERIFY(0x022955B8, md_getGroundY);
/* 022955C0 getLeftHandMatrix(): cullMtx */
static u32 md_getLeftHandMatrix(daNpc_Md_c* i_this) {
    WWHD_FUNC(0x022955C0, u32, i_this);
    return i_this->cullMtx;
}
VERIFY(0x022955C0, md_getLeftHandMatrix);
/* 022955C8 getRightHandMatrix(): cullMtx */
static u32 md_getRightHandMatrix(daNpc_Md_c* i_this) {
    WWHD_FUNC(0x022955C8, u32, i_this);
    return i_this->cullMtx;
}
VERIFY(0x022955C8, md_getRightHandMatrix);
/* 022955D0 getBaseAnimeFrameRate() */
static f32 md_getBaseAnimeFrameRate(daNpc_Md_c* i_this) {
    WWHD_FUNC(0x022955D0, f32, i_this);
    return 1.0f;
}
VERIFY(0x022955D0, md_getBaseAnimeFrameRate);
/* 022955DC getBaseAnimeFrame() */
static f32 md_getBaseAnimeFrame(daNpc_Md_c* i_this) {
    WWHD_FUNC(0x022955DC, f32, i_this);
    return 0.0f;
}
VERIFY(0x022955DC, md_getBaseAnimeFrame);
/* 022955E8 empty inline */
static void md_inline_022955E8(daNpc_Md_c* i_this) {
    WWHD_FUNC(0x022955E8, void, i_this);
}
VERIFY(0x022955E8, md_inline_022955E8);


/* ---- HIO (constructors allocate when this == NULL) ---- */
/* 02294CD0 */
static daNpc_Md_HIO6_l* daNpc_Md_HIO6_c_ct(daNpc_Md_HIO6_l* i_this) {
    WWHD_FUNC(0x02294CD0, daNpc_Md_HIO6_l*, i_this);
    if (i_this == nullptr) {
        i_this = (daNpc_Md_HIO6_l*)operator_new(0x18);
        if (i_this == nullptr)
            return i_this;
    }
    i_this->__vtbl = 0x1001D7D8;
    i_this->m04 = 500.0f;
    i_this->m08 = 430;
    i_this->m0A = 9000;
    i_this->m0C = -5000;
    i_this->m0E = -9000;
    i_this->m10 = 0;
    i_this->m12 = 0;
    i_this->m14 = 0;
    i_this->m16 = 0;
    return i_this;
}
VERIFY(0x02294CD0, daNpc_Md_HIO6_c_ct);

/* 02294D50 */
static daNpc_Md_HIO5_l* daNpc_Md_HIO5_c_ct(daNpc_Md_HIO5_l* i_this) {
    WWHD_FUNC(0x02294D50, daNpc_Md_HIO5_l*, i_this);
    if (i_this == nullptr) {
        i_this = (daNpc_Md_HIO5_l*)operator_new(0xC);
        if (i_this == nullptr)
            return i_this;
    }
    i_this->__vtbl = 0x1001D7F0;
    i_this->m4 = 16.0f;
    i_this->m8 = 0.6f;
    return i_this;
}
VERIFY(0x02294D50, daNpc_Md_HIO5_c_ct);

/* 02294DA8 */
static daNpc_Md_HIO4_l* daNpc_Md_HIO4_c_ct(daNpc_Md_HIO4_l* i_this) {
    WWHD_FUNC(0x02294DA8, daNpc_Md_HIO4_l*, i_this);
    if (i_this == nullptr) {
        i_this = (daNpc_Md_HIO4_l*)operator_new(0xC);
        if (i_this == nullptr)
            return i_this;
    }
    i_this->__vtbl = 0x1001D808;
    i_this->m4 = 86.0f;
    i_this->m8 = 0;
    return i_this;
}
VERIFY(0x02294DA8, daNpc_Md_HIO4_c_ct);

/* 02294DFC (USA values: DEMO_SELECT second) */
static daNpc_Md_HIO3_l* daNpc_Md_HIO3_c_ct(daNpc_Md_HIO3_l* i_this) {
    WWHD_FUNC(0x02294DFC, daNpc_Md_HIO3_l*, i_this);
    if (i_this == nullptr) {
        i_this = (daNpc_Md_HIO3_l*)operator_new(0x28);
        if (i_this == nullptr)
            return i_this;
    }
    i_this->__vtbl = 0x1001D820;
    i_this->m04 = 0.8f;
    i_this->m08 = 0.2f;
    i_this->m0C = -5.0f;
    i_this->m10 = -5.0f;
    i_this->m14 = 0.0f;
    i_this->m18 = 0;
    i_this->m1A = 0;
    i_this->m1C = 0;
    i_this->m1E = 3800;
    i_this->m20 = 5800;
    i_this->m22 = 0x7FFF;
    i_this->m24 = -0x7FFF;
    return i_this;
}
VERIFY(0x02294DFC, daNpc_Md_HIO3_c_ct);

/* 02294EA0 */
static daNpc_Md_HIO2_l* daNpc_Md_HIO2_c_ct(daNpc_Md_HIO2_l* i_this) {
    WWHD_FUNC(0x02294EA0, daNpc_Md_HIO2_l*, i_this);
    if (i_this == nullptr) {
        i_this = (daNpc_Md_HIO2_l*)operator_new(0x2C);
        if (i_this == nullptr)
            return i_this;
    }
    i_this->__vtbl = 0x1001D838;
    i_this->m26 = 0x19;
    i_this->m28 = 0x7FFF;
    i_this->m2A = 1;
    i_this->m04 = 5.0f;
    i_this->m08 = 17.0f;
    i_this->m0C = 7.0f;
    i_this->m10 = 6.0f;
    i_this->m14 = 0.7f;
    i_this->m20 = 3300;
    i_this->m22 = 2900;
    i_this->m24 = 5;
    i_this->m18 = 2.6f;
    i_this->m1C = 0.6f;
    return i_this;
}
VERIFY(0x02294EA0, daNpc_Md_HIO2_c_ct);

/* 0259DA18 dNpc_HIO_c::dNpc_HIO_c */
static inline void dNpc_HIO_c_ct(dNpc_HIO_l* p) { gabi::call(0x0259DA18, p); }

/* 02294F64 */
static daNpc_Md_HIO_l* daNpc_Md_HIO_c_ct(daNpc_Md_HIO_l* i_this) {
    WWHD_FUNC(0x02294F64, daNpc_Md_HIO_l*, i_this);
    if (i_this == nullptr) {
        i_this = (daNpc_Md_HIO_l*)operator_new(0x1CC);
        if (i_this == nullptr)
            return i_this;
    }
    i_this->__vtbl = 0x1001D850;
    daNpc_Md_HIO2_c_ct(&i_this->m008);
    daNpc_Md_HIO3_c_ct(&i_this->m034);
    daNpc_Md_HIO4_c_ct(&i_this->m05C);
    daNpc_Md_HIO5_c_ct(&i_this->m068);
    daNpc_Md_HIO6_c_ct(&i_this->m074);
    dNpc_HIO_c_ct(&i_this->mNpc);
    i_this->mNpc.m04 = -25.0f;
    i_this->mNpc.mMaxHeadX = 0x2328;
    i_this->mNpc.mMaxHeadY = 0x2328;
    i_this->mNpc.mMaxBackboneX = 0x0;
    i_this->mNpc.mMaxBackboneY = 0x1F40;
    i_this->mNpc.mMinHeadX = -0x2328;
    i_this->mNpc.mMinHeadY = -0x2328;
    i_this->mNpc.mMinBackboneX = 0x0;
    i_this->mNpc.mMinBackboneY = -0x1F40;
    i_this->mNpc.mMaxTurnStep = 0x1000;
    i_this->mNpc.mMaxHeadTurnVel = 0x800;
    i_this->mNpc.mAttnYOffset = 130.0f;
    i_this->mNpc.mMaxAttnAngleY = 0x4000;
    i_this->mNpc.m22 = 0x0;
    i_this->mNpc.mMaxAttnDistXZ = 150.0f;
    i_this->mpActor = nullptr;
    i_this->m0B8 = 770.0f;
    i_this->m0BC = 250.0f;
    i_this->m0C0 = -450.0f;
    i_this->m0C4 = 90.0f;
    i_this->m0C8 = 400.0f;
    i_this->m0CC = 600.0f;
    i_this->m0D0 = 0.05f;
    i_this->m0D4 = 17.0f;
    i_this->m0D8 = 0.0f;
    i_this->m0DC = 0.5f;
    i_this->m0E0 = 0.3f;
    i_this->m0E4 = 0.9f;
    i_this->m0E8 = 15.0f;
    i_this->m0EC = 22.0f;
    i_this->m0F0 = 9.0f;
    i_this->m0F4 = 4.0f;
    i_this->m0F8 = -1.51367f;
    i_this->m0FC = -1.09863f;
    i_this->m100 = -0.56152f;
    i_this->m104 = 0.1f;
    i_this->m108 = 0.2f;
    i_this->m10C = 10.0f;
    i_this->m110 = -50.0f;
    i_this->m114 = 1.3f;
    i_this->m128 = 1200.0f;
    i_this->m12C = 1500.0f;
    i_this->m118 = 1.0f;
    i_this->m11C = 1.5f;
    i_this->m120 = 2.0f;
    i_this->m124 = 1.6f;
    i_this->m130 = 3.0f;
    i_this->m134 = 1.25f;
    i_this->m138 = 0.2f;
    i_this->m13C = 100.0f;
    i_this->m140 = 50.0f;
    i_this->m144 = 9.0f;
    i_this->m1B4 = 0x17;
    i_this->m1B6 = 0x14;
    i_this->m1C7 = 0x0;
    i_this->m148 = 0.9f;
    i_this->m1C6 = 0x1;
    i_this->m14C = 6.0f;
    i_this->m1B8 = 0x4000;
    i_this->m150 = 10.0f;
    i_this->m1BA = 0x78;
    i_this->m1BC = 0x1F4;
    i_this->m154 = 5.0f;
    i_this->m158 = 2.2f;
    i_this->m15C = -11.681f;
    i_this->m160 = 1.3f;
    i_this->m164 = 88.692f;
    i_this->m168 = 57.066f;
    i_this->m16C = 179.286f;
    i_this->m1C8 = 0x0;
    i_this->m170 = 0.0f;
    i_this->m174 = 1.0f;
    i_this->m178 = 1.5f;
    i_this->m17C = -100.0f;
    i_this->m180 = 100.0f;
    i_this->m184 = 10.0f;
    i_this->m188 = 1.0f;
    i_this->m18C = -10.0f;
    i_this->m1BE = 0x46;
    i_this->m1C0 = 0x5;
    i_this->m1C2 = 0x1C2;
    i_this->m1C4 = 0x96;
    i_this->m190 = 43.0f;
    i_this->m194 = 14.65f;
    i_this->m198 = 13.99f;
    i_this->m19C = 0.0f;
    i_this->m1A0 = -98.0f;
    i_this->m1A4 = 0.0f;
    i_this->m1A8 = 3.0f;
    i_this->m1AC = 1000.0f;
    i_this->m1B0 = 1000.0f;
    i_this->mNo = -1;
    return i_this;
}
VERIFY(0x02294F64, daNpc_Md_HIO_c_ct);

/* 02295330 __sinit_d_a_npc_md_cpp (new: compiler-generated) */
static void __sinit_d_a_npc_md_cpp() {
    WWHD_FUNC(0x02295330, void);
    sinit_header_statics(0x10467A38, 0x101C1EEC);
    daNpc_Md_HIO_c_ct(&l_HIO());
    /* l_ms_light_local_start(5, 7, 0), l_ms_light_local_vec(0, 0, -10000) */
    cXyz* start = gabi::at<cXyz>(0x10467A60);
    cXyz* vec = gabi::at<cXyz>(0x10467A54);
    start->x = 5.0f;
    gabi::store<f32>(0x101C03EC, 70.0f); /* HD: l_fan_light_cps_src radius set at run time */
    vec->x = 0.0f;
    vec->y = 0.0f;
    start->y = 7.0f;
    start->z = 0.0f;
    vec->z = -10000.0f;
}
VERIFY(0x02295330, __sinit_d_a_npc_md_cpp);

/* 02295420 deleting destructor of a class without member destructors (vtable 0x1001D6A0;
 * new: compiler-generated) */
static void md_dtor_02295420(void* i_this, s32 flags) {
    WWHD_FUNC(0x02295420, void, i_this, flags);
    if (i_this != nullptr && (flags & 1)) {
        operator_delete(i_this);
    }
}
VERIFY(0x02295420, md_dtor_02295420);

/* 0229543C deleting destructor: member at +0x14 (destructor 02018034); not referenced
 * (new: compiler-generated) */
static void md_dtor_0229543C(void* i_this, s32 flags) {
    WWHD_FUNC(0x0229543C, void, i_this, flags);
    if (i_this != nullptr) {
        gabi::call(0x02018034, gabi::ea(i_this) + 0x14, 2);
        if (flags & 1) {
            operator_delete(i_this);
        }
    }
}
VERIFY(0x0229543C, md_dtor_0229543C);

/* 02295490 deleting destructor: members at +0x158 (027BF880) and +4 (027B5CBC); not referenced
 * (new: compiler-generated) */
static void md_dtor_02295490(void* i_this, s32 flags) {
    WWHD_FUNC(0x02295490, void, i_this, flags);
    if (i_this != nullptr) {
        gabi::call(0x027BF880, gabi::ea(i_this) + 0x158, 2);
        gabi::call(0x027B5CBC, gabi::ea(i_this) + 4, 2);
        if (flags & 1) {
            operator_delete(i_this);
        }
    }
}
VERIFY(0x02295490, md_dtor_02295490);

/* 02055B64 cLib_calcTimer<s16> (out of line) */
static inline s16 md_calcTimer(be<s16>* t) { return gabi::call<s16>(0x02055B64, t); }
/* setNpcAction / setPlayerAction with a PMF constant (by-value copy on the stack) */
#define MD_SET_NPC_ACTION(pmf)              \
    do {                                    \
        gabi::Local<ProcFunc_l> fn_;        \
        md_pmf_load(fn_, pmf);              \
        setNpcAction(fn_, nullptr);         \
    } while (0)
#define MD_SET_PLAYER_ACTION(pmf)           \
    do {                                    \
        gabi::Local<ProcFunc_l> fn_;        \
        md_pmf_load(fn_, pmf);              \
        setPlayerAction(fn_, nullptr);      \
    } while (0)
/* chkAttention(cXyz pos, ...): pos by value (a stack copy through FPRs) */
/* the result is a byte register (GHS bool): callers store it unnormalised */
static inline u8 md_chkAttention(daNpc_Md_c* i_this, cXyz* pos, s16 angle, int p) {
    gabi::Local<cXyz> tmp;
    f32 x = pos->x, y = pos->y;
    tmp->x = x;
    f32 z = pos->z;
    tmp->y = y;
    tmp->z = z;
    return gabi::call<u8>(0x0228DAF4, i_this, tmp.get(), angle, p);
}

/* 02291F50 */
BOOL daNpc_Md_c::piyo2NpcAction(void*) {
    WWHD_FUNC(0x02291F50, BOOL, this, (void*)nullptr);
    if (mActionStatus == ACTION_STARTING) {
        md_attnFlags(this) |= 0xA; /* fopAc_Attn_LOCKON_TALK_e | fopAc_Attn_ACTION_SPEAK_e */
        maxFallSpeed = -100.0f;
        gravity = l_HIO().m0F4;
        setAnm(0x14);
        clearStatus(daMdStts_UNK1 | daMdStts_UNK2 | daMdStts_UNK4 | daMdStts_FLY);
        shape_angle.x = 0;
        shape_angle.z = 0;
        speedF = 0.0f;
        m30F8 = 120.0f;
        m3144 = l_HIO().m1BA;
        if (mType == 4 || mType == 5) { /* isTypeEdaichi() || isTypeM_Dai() */
            m3144 = (s16)(m3144 >> 1);
        }
        mAcchCir[1].SetWall(60.0f, 20.0f);
        mActionStatus = mActionStatus + 1;
    } else if (mActionStatus != ACTION_ENDING) {
        fopAcM_seStart(this, 0x50BC /* JA_SE_CM_MD_PIYO */, 0);
        s16 angle = shape_angle.y + mJntCtrl.mAngles[0][1] + mJntCtrl.mAngles[1][1];
        if (md_calcTimer(&m3144) == 0) {
            MD_SET_NPC_ACTION(PMF_waitNpcAction);
        }
        gabi::Local<be<s32>> local_28;
        u8 att = md_chkAttention(this, &current.pos, angle, 0);
        m312C = att;
        *local_28 = att;
        if (att) {
            if ((u8)mType <= 3) { /* Atorizk, Adanmae, M_Dra09, Sea */
                md_attnFlags(this) |= 0xA;
            } else {
                md_attnFlags(this) &= ~0xA;
            }
            if (mType == 1) { /* isTypeAdanmae() */
                mCurEventMode = 2;
            }
        }
        NpcCall(local_28);
        lookBack(1, 1, 0);
        current.angle.y = shape_angle.y;
        setAttention(true);
    }
    return TRUE;
}
VERIFY(0x02291F50, &daNpc_Md_c::piyo2NpcAction);

/* 0259DED0 dNpc_JntCtrl_c::lookAtTarget(s16* outY, cXyz* target, cXyz eye (pointer to a copy), s16, s16, bool) */
static inline void md_lookAtTarget(dNpc_JntCtrl_c* j, be<s16>* outY, cXyz* target, cXyz* eye, s16 yrot, s16 vel, bool headOnly) {
    gabi::call(0x0259DED0, j, outY, target, eye, yrot, vel, headOnly);
}

/* 022921B4 HD: the last phase waits for actor condition bit 2 (GameCube: a 60-frame timer) */
BOOL daNpc_Md_c::demoFlyNpcAction(void*) {
    WWHD_FUNC(0x022921B4, BOOL, this, (void*)nullptr);
    if (mActionStatus == ACTION_STARTING) {
        md_attnFlags(this) = 0;
        mAcchCir[1].SetWall(60.0f, 20.0f);
        current.angle.y = 0x5700;
        mActionStatus = mActionStatus + 1;
    } else if (mActionStatus == ACTION_ENDING) {
        emitterDelete(m0508);
    } else {
        md_attnFlags(this) = 0;
        if (mActionStatus == ACTION_ONGOING_1) {
            if (mJntCtrl.mbTrn != 0) { /* trnChk() */
                cLib_addCalcAngleS2(&m3110, l_HIO().mNpc.mMaxHeadTurnVel, 4, 0x800);
            } else {
                m3110 = 0;
                setAnm(0x18);
                mActionStatus = mActionStatus + 1;
            }
            gabi::Local<cXyz> local_1c;
            gabi::Local<cXyz> eye;
            local_1c->z = 150.0f;
            local_1c->x = -1511.0f;
            local_1c->y = 14500.0f;
            f32 x = current.pos.x, y = current.pos.y, z = current.pos.z;
            eye->x = x;
            eye->y = y;
            eye->z = z;
            md_lookAtTarget(&mJntCtrl, &shape_angle.y, local_1c, eye, current.angle.y, m3110, false);
        } else if (mActionStatus == ACTION_ONGOING_2) {
            if (m312A != 0) {
                setAnm(0x19);
                setBitStatus(daMdStts_UNK1);
                setWingEmitter();
                speed.y = 0.0f;
                gravity = 1.0f;
                maxFallSpeed = 100.0f;
                mActionStatus = mActionStatus + 1;
            }
        } else if (mActionStatus == ACTION_ONGOING_3) {
            if (m312A != 0) {
                setAnm(10);
            }
            if (current.pos.y > 14500.0f) {
                setAnm(0xb);
                maxFallSpeed = 0.0f;
                mActionStatus = mActionStatus + 1;
                speedF = 17.0f;
                speed.y = 0.0f;
                gravity = 0.0f;
            }
        } else if (mActionStatus == 4 && (actor_condition & 4)) {
            fopAcM_delete(this);
        }
        setAttention(true);
    }
    return TRUE;
}
VERIFY(0x022921B4, &daNpc_Md_c::demoFlyNpcAction);

/* this TU's copies of the dBgS_GndChk / dBgS_LinChk vtables */
static const dBgS_GndChk_vt MD_GNDCHK_VT = {0x1001D718, 0x1001D728, 0x1001D748, 0x1001D738};
static const dBgS_LinChk_vt MD_LINCHK_VT = {0x1001D758, 0x1001D768, 0x1001D788, 0x1001D778};
static inline void md_GndChk_dt(void* c) {
    u32 b = gabi::ea(c);
    gabi::store<u32>(b + 0x20, 0x1001D728);
    gabi::store<u32>(b + 0x40, 0x1001D748);
    gabi::store<u32>(b + 0x4C, 0x1001D708);
    gabi::call(0x02008DAC, c, 0); /* cBgS_Chk::~cBgS_Chk */
}
static inline void md_LinChk_dt(void* c) {
    u32 b = gabi::ea(c);
    gabi::store<u32>(b + 0x58, 0x1001D788);
    gabi::store<u32>(b + 0x64, 0x1001D708);
    gabi::store<u32>(b + 0x20, 0x1001D6F8);
    gabi::call(0x02008B4C, c, 0); /* cBgS_LinChk::~cBgS_LinChk */
}
struct md_chk54 { u8 _[0x54]; };
struct md_chk6C { u8 _[0x6C]; };

/* 0229249C */
f32 daNpc_Md_c::checkForwardGroundY(s16 param_1) {
    WWHD_FUNC(0x0229249C, f32, this, param_1);
    if (mAcchCir[0].ChkWallHit()) {
        void* pla = dBgS_GetTriPla(dComIfG_Bgsp(), &mAcchCir[0]);
        if (pla != nullptr && cLib_distanceAngleS(param_1, cM_atan2s(gabi::load<f32>(gabi::ea(pla)), gabi::load<f32>(gabi::ea(pla) + 8))) > 0x4000) {
            gabi::Local<md_chk54> gnd_chk;
            dBgS_GndChk_ct(gnd_chk, MD_GNDCHK_VT, false);
            u32 g = gabi::ea(gnd_chk.get());
            gabi::store<u32>(g + 0x30, gabi::load<u32>(g + 0x30) & ~2u); /* OffWall() */
            f32 x = gabi::fmadds(80.0f, cM_ssin(param_1), current.pos.x);
            f32 y = current.pos.y + 80.0f;
            f32 z = gabi::fmadds(80.0f, cM_scos(param_1), current.pos.z);
            cXyz* pos = gabi::at<cXyz>(g + 0x24); /* SetPos */
            pos->x = x;
            pos->y = y;
            pos->z = z;
            f32 ret = cBgS_GroundCross(dComIfG_Bgsp(), gnd_chk);
            md_GndChk_dt(gnd_chk);
            return ret;
        }
    }
    return -10000000.0f;
}
VERIFY(0x0229249C, &daNpc_Md_c::checkForwardGroundY);

/* 0229278C */
void daNpc_Md_c::routeWallCheck(cXyz* param_1, cXyz* param_2, be<s16>* param_3) {
    WWHD_FUNC(0x0229278C, void, this, param_1, param_2, param_3);
    gabi::Local<md_chk6C> lin_chk;
    dBgS_LinChk_ct(lin_chk, MD_LINCHK_VT, false);
    dBgS_LinChk_Set(lin_chk, param_1, param_2, nullptr);
    if (cBgS_LineCross(dComIfG_Bgsp(), lin_chk)) {
        void* pla = dBgS_GetTriPla(dComIfG_Bgsp(), gabi::at<u8>(gabi::ea(lin_chk.get()) + 0x14));
        if (pla != nullptr) {
            routeAngCheck(gabi::at<cXyz>(gabi::ea(pla)), param_3);
        }
    }
    md_LinChk_dt(lin_chk);
}
VERIFY(0x0229278C, &daNpc_Md_c::routeWallCheck);

/* 0229325C HD: the reverb reaches mDoAud_monsSeStart as its 4th argument (GameCube bug fixed) */
BOOL daNpc_Md_c::hitNpcAction(void* r29) {
    WWHD_FUNC(0x0229325C, BOOL, this, r29);
    if (mActionStatus == ACTION_STARTING) {
        mDamageFogTimer = 5 * 30;
        s32 reverb = dComIfGp_getReverb(current.roomNo);
        mDoAud_monsSeStart_md(0x48A9 /* JA_SE_CV_MD_CRASH */, &current.pos, md_procId(this), reverb);
        s16 angle = 0;
        if (r29 != nullptr) {
            angle = *(be<s16>*)r29;
        }
        current.angle.y = angle;
        speedF = 10.0f;
        speed.y = 20.0f;
        md_attnFlags(this) &= ~0x10u; /* fopAc_Attn_ACTION_CARRY_e */
        mAcchCir[1].SetWall(60.0f, 20.0f);
        clearStatus(daMdStts_UNK1 | daMdStts_UNK4);
        setBitStatus(daMdStts_UNK2);
        m30F8 = 120.0f;
        setAnm(0xD);
        mActionStatus = mActionStatus + 1;
    } else if (mActionStatus != ACTION_ENDING) {
        if (mAcch.ChkGroundHit()) {
            speedF = 0.0f;
            MD_SET_NPC_ACTION(PMF_02291E50 /* land03NpcAction */);
        }
        setAttention(true);
    }
    return TRUE;
}
VERIFY(0x0229325C, &daNpc_Md_c::hitNpcAction);

/* 022933A0 */
BOOL daNpc_Md_c::jumpNpcAction(void* param_1) {
    WWHD_FUNC(0x022933A0, BOOL, this, param_1);
    if (mActionStatus == ACTION_STARTING) {
        clearStatus(daMdStts_UNK1 | daMdStts_UNK2 | daMdStts_UNK4 | daMdStts_FLY);
        setBitStatus(daMdStts_UNK4);
        if (param_1 != nullptr) {
            speed.y = *(be<f32>*)param_1;
        }
        shape_angle.x = 0;
        shape_angle.z = 0;
        speedF = 4.0f;
        gravity = l_HIO().m0F4;
        maxFallSpeed = -100.0f;
        setAnm(0xc);
        mAcchCir[1].SetWall(60.0f, 20.0f);
        mActionStatus = mActionStatus + 1;
    } else if (mActionStatus != ACTION_ENDING) {
        if (mActionStatus == ACTION_ONGOING_1) {
            if (mAcch.ChkGroundHit()) {
                speedF = 0.0f;
                setAnm(0xe);
                m312A = 0;
                mActionStatus = mActionStatus + 1;
            }
        } else if (m312A != 0) {
            speedF = 0.0f;
            MD_SET_NPC_ACTION(PMF_waitNpcAction);
        }
        setAttention(true);
    }
    return TRUE;
}
VERIFY(0x022933A0, &daNpc_Md_c::jumpNpcAction);

/* 0200FA40 cLib_offsetPos(cXyz* dst, const cXyz* base, s16 angle, const cXyz* offset) */
static inline void md_cLib_offsetPos(cXyz* dst, cXyz* base, s16 angle, cXyz* ofs) { gabi::call(0x0200FA40, dst, base, angle, ofs); }

/* 0229310C HD-only npc action (new; PMF 0x1001D548): once no event runs, Medli is placed
 * behind the player (offset (-100, 0, -150) in the player's frame), takes the current room as
 * her home and goes back to waitNpcAction */
BOOL daNpc_Md_c::npcAction_0229310C(void*) {
    WWHD_FUNC(0x0229310C, BOOL, this, (void*)nullptr);
    if (mActionStatus == ACTION_STARTING) {
        speedF = 0.0f;
        mActionStatus = mActionStatus + 1;
    } else if (mActionStatus != ACTION_ENDING) {
        s8 stayNo = gabi::load<s8>(0x1047E6C8); /* dStage_roomControl_c::mStayNo */
        current.roomNo = stayNo;
        if (gabi::load<u8>(dComIfGp_ea() + 0x5292) == 0) { /* !dComIfGp_event_runCheck() */
            /* function-local static cXyz, initialised on first use */
            if (gabi::load<u32>(0x10467C64) == 0) {
                gabi::store<u32>(0x10467C64, 1);
                cXyz* o = gabi::at<cXyz>(0x10467A84);
                o->x = -100.0f;
                o->y = 0.0f;
                o->z = -150.0f;
            }
            fopAc_ac_c* player = dComIfGp_getPlayer(0);
            s16 angle = player->shape_angle.y;
            shape_angle.y = angle;
            md_cLib_offsetPos(&current.pos, &player->current.pos, angle, gabi::at<cXyz>(0x10467A84));
            old.pos.copy(current.pos);
            home.pos.copy(current.pos);
            home.roomNo = stayNo;
            MD_SET_NPC_ACTION(PMF_waitNpcAction);
        }
    }
    return TRUE;
}
VERIFY(0x0229310C, &daNpc_Md_c::npcAction_0229310C);

/* 02293DDC */
BOOL daNpc_Md_c::hitPlayerAction(void* param_1) {
    WWHD_FUNC(0x02293DDC, BOOL, this, param_1);
    if (mActionStatus == ACTION_STARTING) {
        {
            gabi::Local<cXyz> dir;
            dir->set(0.0f, 1.0f, 0.0f);
            dComIfGp_getVibration_StartShock(5, -0x21, dir);
        }
        mDamageFogTimer = 5 * 30;
        s32 reverb = dComIfGp_getReverb(current.roomNo);
        mDoAud_monsSeStart_md(0x48A9 /* JA_SE_CV_MD_CRASH */, &current.pos, md_procId(this), reverb);
        s16 sVar2 = 0;
        if (param_1 != nullptr) {
            sVar2 = *(be<s16>*)param_1;
        }
        current.angle.y = sVar2;
        speedF = 10.0f;
        speed.y = 20.0f;
        maxFallSpeed = -100.0f;
        gravity = l_HIO().m0F4;
        md_attnFlags(this) &= ~0x10u; /* fopAc_Attn_ACTION_CARRY_e */
        mAcchCir[1].SetWall(60.0f, 20.0f);
        clearStatus(daMdStts_UNK1 | daMdStts_UNK4);
        setBitStatus(daMdStts_UNK2);
        m30F8 = 120.0f;
        setAnm(0xd);
        mActionStatus = mActionStatus + 1;
    } else if (mActionStatus != ACTION_ENDING) {
        if (mActionStatus == ACTION_ONGOING_1) {
            if (mAcch.ChkGroundHit()) {
                speedF = 0.0f;
                setAnm(0xf);
                m312A = 0;
                mActionStatus = mActionStatus + 1;
            }
        } else if (m312A != 0) {
            m4E4 = m4E4 | 1; /* returnLink() */
        }
        setAttention(true);
    }
    return TRUE;
}
VERIFY(0x02293DDC, &daNpc_Md_c::hitPlayerAction);

/* 02294838 */
BOOL daNpc_Md_c::landPlayerAction(void*) {
    WWHD_FUNC(0x02294838, BOOL, this, (void*)nullptr);
    if (mActionStatus == ACTION_STARTING) {
        s32 reverb = dComIfGp_getReverb(current.roomNo);
        mDoAud_monsSeStart_md(0x48A7 /* JA_SE_CV_MD_LANDING */, &current.pos, md_procId(this), reverb);
        if (checkStatus(daMdStts_UNK1)) {
            m3135 = m3135 | 1; /* setBitEffectStatus(1) */
        }
        clearStatus(daMdStts_UNK1 | daMdStts_UNK2 | daMdStts_UNK4 | daMdStts_FLY);
        setAnm(0xe);
        shape_angle.x = 0;
        shape_angle.z = 0;
        gravity = l_HIO().m0F4;
        maxFallSpeed = -100.0f;
        speedF = 0.0f;
        speed.y = 0.0f;
        mAcchCir[1].SetWall(60.0f, 20.0f);
        mActionStatus = mActionStatus + 1;
    } else if (mActionStatus != ACTION_ENDING) {
        if (m312A != 0) {
            speedF = 0.0f;
            MD_SET_PLAYER_ACTION(PMF_waitPlayerAction);
        }
        setAttention(true);
    }
    return TRUE;
}
VERIFY(0x02294838, &daNpc_Md_c::landPlayerAction);

/* 02293FA8 */
BOOL daNpc_Md_c::jumpPlayerAction(void* param_1) {
    WWHD_FUNC(0x02293FA8, BOOL, this, param_1);
    if (mActionStatus == ACTION_STARTING) {
        clearStatus(daMdStts_UNK1 | daMdStts_UNK2 | daMdStts_UNK4 | daMdStts_FLY);
        setBitStatus(daMdStts_UNK4);
        if (param_1 != nullptr) {
            speed.y = *(be<f32>*)param_1;
        }
        shape_angle.x = 0;
        shape_angle.z = 0;
        gravity = l_HIO().m0F4;
        maxFallSpeed = -100.0f;
        setAnm(0xc);
        mAcchCir[1].SetWall(60.0f, 20.0f);
        mActionStatus = mActionStatus + 1;
        m3150 = current.pos.y;
    } else if (mActionStatus != ACTION_ENDING) {
        if (mActionStatus == ACTION_ONGOING_1) {
            if (mAcch.ChkGroundHit()) {
                if (!(m3150 - current.pos.y < l_HIO().m1B0)) { /* NaN: shakes */
                    gabi::Local<cXyz> dir;
                    dir->set(0.0f, 1.0f, 0.0f);
                    dComIfGp_getVibration_StartShock(6, -0x21, dir);
                }
                speedF = 0.0f;
                setAnm(0xe);
                m312A = 0;
                mActionStatus = mActionStatus + 1;
            }
        } else if (m312A != 0) {
            MD_SET_PLAYER_ACTION(PMF_waitPlayerAction);
        }
        setAttention(true);
    }
    return TRUE;
}
VERIFY(0x02293FA8, &daNpc_Md_c::jumpPlayerAction);

/* 020079B4 CPad_GET_STICK_VALUE(port) (HD: out of line) */
static inline f32 md_stickValue(s32 port) { return gabi::call<f32>(0x020079B4, port); }
/* setPlayerAction(PMF, &arg) */
static inline void md_setPlayerAction(daNpc_Md_c* i_this, u32 pmf, void* arg) {
    gabi::Local<ProcFunc_l> fn;
    md_pmf_load(fn, pmf);
    i_this->setPlayerAction(fn, arg);
}

/* 02293B5C */
BOOL daNpc_Md_c::walkPlayerAction(void*) {
    WWHD_FUNC(0x02293B5C, BOOL, this, (void*)nullptr);
    if (mActionStatus == ACTION_STARTING) {
        setBitStatus(daMdStts_UNK4);
        setAnm(2);
        mMaxNormalSpeed = l_HIO().m008.m08;
        mAcchCir[1].SetWall(60.0f, 20.0f);
        mActionStatus = mActionStatus + 1;
    } else if (mActionStatus != ACTION_ENDING && !flyCheck()) {
        gabi::Local<cXyz> cStack_30;
        gabi::Local<be<f32>> local_64;
        f32 dVar4 = md_stickValue(0);
        s16 sVar3 = getStickAngY(0);
        walkProc(dVar4, sVar3);
        int iVar1 = calcStickPos(sVar3, cStack_30);
        if (iVar1 == 0) {
            cLib_addCalcAngleS(&shape_angle.y, current.angle.y, l_HIO().m008.m24, (s16)(l_HIO().m008.m20 * 2), (s16)(l_HIO().m008.m22 * 2));
        }
        {
            gabi::Local<be<s16>> tempAngle;
            gabi::Local<cXyz> eye;
            f32 z = current.pos.z, y = current.pos.y, x = current.pos.x;
            eye->z = z;
            eye->x = x;
            s16 ang = shape_angle.y;
            eye->y = y;
            *tempAngle = ang;
            md_lookAtTarget(&mJntCtrl, tempAngle, cStack_30, eye, ang, l_HIO().mNpc.mMaxTurnStep, false);
            if (iVar1 > 0) {
                shape_angle.y = *tempAngle;
            }
        }
        if (speedF < 0.001f || iVar1 != 0) {
            MD_SET_PLAYER_ACTION(PMF_waitPlayerAction);
        } else if (!mAcch.ChkGroundHit()) {
            *local_64 = 10.0f;
            md_setPlayerAction(this, PMF_jumpPlayerAction, local_64);
        } else if (mAcch.ChkWallHit()) {
            f32 v = checkWallJump(current.angle.y);
            *local_64 = v;
            if (!(v < 0.0f)) {
                md_setPlayerAction(this, PMF_jumpPlayerAction, local_64);
                return TRUE;
            }
        }
        setAttention(true);
    }
    return TRUE;
}
VERIFY(0x02293B5C, &daNpc_Md_c::walkPlayerAction);

#define MD_M_MIRROR 0x101D5F3F /* bool daNpc_Md_c::m_mirror */
/* dAttention_c::Lockon(): LockonTruth() || (flags (+0x20) & 0x20000000); attention at play+0x5804 */
static inline bool md_attention_Lockon(u32 att) {
    if (gabi::call<u8>(0x024EDFCC, att) != 0) /* dAttention_c::LockonTruth */
        return true;
    return (gabi::load<u32>(att + 0x20) & 0x20000000) != 0;
}

/* 02294984 */
BOOL daNpc_Md_c::mkamaePlayerAction(void*) {
    WWHD_FUNC(0x02294984, BOOL, this, (void*)nullptr);
    if (mActionStatus == ACTION_STARTING) {
        clearStatus(daMdStts_UNK1 | daMdStts_UNK2 | daMdStts_UNK4 | daMdStts_FLY);
        setAnm(0x1F);
        shape_angle.x = 0;
        shape_angle.z = 0;
        gravity = l_HIO().m0F4;
        maxFallSpeed = -100.0f;
        speedF = 0.0f;
        speed.y = 0.0f;
        mAcchCir[1].SetWall(60.0f, 20.0f);
        mActionStatus = mActionStatus + 1;
    } else if (mActionStatus == ACTION_ENDING) {
        gabi::store<u8>(MD_M_MIRROR, 0); /* offMirror() */
    } else {
        if (mActionStatus == ACTION_ONGOING_1) {
            if (m312A != 0) {
                speedF = 0.0f;
                setAnm(0x20);
                gabi::store<u8>(MD_M_MIRROR, 1); /* onMirror() */
                mActionStatus = mActionStatus + 1;
            }
        } else if (mActionStatus == ACTION_ONGOING_2) {
            if (mirrorCancelCheck() || !checkStatus(daMdStts_LIGHT_BODY_HIT)) {
                setAnm(0x21);
                m312A = 0;
                gabi::store<u8>(MD_M_MIRROR, 0);
                mActionStatus = mActionStatus + 1;
            } else {
                u32 attention = dComIfGp_ea() + 0x5804; /* dComIfGp_getAttention() */
                if (!(md_stickValue(0) < l_HIO().m104) || md_attention_Lockon(attention)) {
                    m311A = getStickAngY(FALSE);
                    m310C = md_stickValue(0);
                } else {
                    m311A = 0;
                    m310C = 0.0f;
                }
                lookBackWaist(m311A, m310C);
            }
        } else if (m312A != 0) {
            MD_SET_PLAYER_ACTION(PMF_waitPlayerAction);
        }
        setAttention(true);
    }
    return TRUE;
}
VERIFY(0x02294984, &daNpc_Md_c::mkamaePlayerAction);

/* mDoExt_McaMorf2::isMorf(): the morf rate (+0xB4) below 1 */
static inline bool md_isMorf(mDoExt_McaMorf2* m) { return gabi::load<f32>(gabi::ea(m) + 0xB4) < 1.0f; }

/* 02293840 */
BOOL daNpc_Md_c::waitPlayerAction(void*) {
    WWHD_FUNC(0x02293840, BOOL, this, (void*)nullptr);
    if (mActionStatus == ACTION_STARTING) {
        clearStatus(daMdStts_UNK1 | daMdStts_UNK2 | daMdStts_FLY);
        setBitStatus(daMdStts_UNK4);
        setAnm(0);
        speedF = 0.0f;
        mAcchCir[1].SetWall(60.0f, 20.0f);
        m311A = 0;
        m310C = 0.0f;
        m3114 = 0;
        m3116 = 0;
        mActionStatus = mActionStatus + 1;
    } else if (mActionStatus != ACTION_ENDING && !flyCheck()) {
        u32 attention = dComIfGp_ea() + 0x5804; /* dComIfGp_getAttention() */
        if (!(md_stickValue(0) < l_HIO().m104) || md_attention_Lockon(attention)) {
            s16 stickAngle = getStickAngY(0);
            cLib_addCalcAngleS(&current.angle.y, stickAngle, l_HIO().m008.m26, l_HIO().m008.m28, l_HIO().m008.m2A);
            gabi::Local<cXyz> stickPos;
            int temp = calcStickPos(stickAngle, stickPos);
            if (temp == 0 || !(md_stickValue(0) < l_HIO().m108)) {
                shape_angle.y = current.angle.y;
            }
            {
                gabi::Local<be<s16>> tempAngle;
                gabi::Local<cXyz> eye;
                f32 x = current.pos.x, y = current.pos.y, z = current.pos.z;
                s16 ang = shape_angle.y;
                eye->x = x;
                eye->y = y;
                eye->z = z;
                *tempAngle = ang;
                md_lookAtTarget(&mJntCtrl, tempAngle, stickPos, eye, ang, l_HIO().mNpc.mMaxTurnStep, false);
                if (temp > 0) {
                    shape_angle.y = *tempAngle;
                }
            }
            current.angle.y = shape_angle.y;
            if (!(md_stickValue(0) < l_HIO().m108)) {
                if (temp == 0) {
                    current.angle.y = stickAngle;
                    MD_SET_PLAYER_ACTION(PMF_walkPlayerAction);
                }
            }
        } else {
            gabi::Local<cXyz> zero;
            cXyz* z0 = gabi::at<cXyz>(0x101FFBA8); /* cXyz::Zero */
            f32 y = z0->y, x = z0->x, z = z0->z;
            zero->y = y;
            zero->z = z;
            zero->x = x;
            md_lookAtTarget(&mJntCtrl, &shape_angle.y, nullptr, zero, shape_angle.y, 0, false);
            waitGroundCheck();
            current.angle.y = shape_angle.y;
        }
        setAttention(md_isMorf(mpMorf.get()));
    }
    return TRUE;
}
VERIFY(0x02293840, &daNpc_Md_c::waitPlayerAction);

/* 02293500 */
BOOL daNpc_Md_c::escapeNpcAction(void*) {
    WWHD_FUNC(0x02293500, BOOL, this, (void*)nullptr);
    if (mActionStatus == ACTION_STARTING) {
        setBitStatus(daMdStts_UNK4);
        md_attnFlags(this) |= 0x10; /* fopAc_Attn_ACTION_CARRY_e */
        mAcchCir[1].SetWall(60.0f, 20.0f);
        if (m3104 == 0x11 || m3104 == 0x16 || m3104 == 0x17) {
            setAnm(0x13);
            mActionStatus = mActionStatus + 1;
        } else if (m3104 == 0x20) {
            setAnm(0x21);
            m312A = 0;
            mActionStatus = mActionStatus + 1;
        } else {
            setAnm(2);
            mActionStatus = mActionStatus + 2;
        }
        if (!mAcch.ChkGroundHit()) {
            f32 gndY = mAcch.GetGroundH();
            f32 delta = gndY - current.pos.y;
            if (delta < 0.0f && !(delta < -30.1f)) {
                speed.y = 0.0f;
                current.pos.y = gndY;
                mAcch.m_flags = mAcch.m_flags | 0x20; /* SetGroundHit() */
            }
        }
        m3144 = 0;
        m3146 = 0x1e;
    } else if (mActionStatus != ACTION_ENDING) {
        if (mActionStatus == ACTION_ONGOING_1) {
            if (m312A != 0) {
                setAnm(2);
                mActionStatus = mActionStatus + 1;
            }
        } else {
            m312C = 1;
            md_calcTimer(&m3146);
            gabi::Local<be<s16>> local_38;
            s16 sVar5;
            if (m313F != 0) {
                s16 ofs = (m313F < 0) ? (s16)-0x4000 : (s16)0x4000;
                sVar5 = (s16)(cM_atan2s(m30C4.x, m30C4.z) + ofs);
                *local_38 = sVar5;
                m3144 = sVar5;
            } else if (m3146 == 0) {
                MD_SET_NPC_ACTION(PMF_waitNpcAction);
                return TRUE;
            } else {
                sVar5 = m3144;
                *local_38 = sVar5;
            }
            if (routeCheck(250000.0f, local_38)) {
                cLib_distanceAngleS(sVar5, *local_38);
            }
            walkProc(1.0f, *local_38);
            cLib_addCalcAngleS(&shape_angle.y, current.angle.y, l_HIO().m008.m24, (s16)(l_HIO().m008.m20 * 2), (s16)(l_HIO().m008.m22 * 2));
            s16 temp4 = shape_angle.y;
            if (speedF < 0.001f) {
                MD_SET_NPC_ACTION(PMF_waitNpcAction);
            } else {
                shape_angle.y = temp4;
            }
            lookBack(0, 0, 0);
            setAttention(true);
        }
    }
    return TRUE;
}
VERIFY(0x02293500, &daNpc_Md_c::escapeNpcAction);

/* setNpcAction(PMF, &arg) */
static inline void md_setNpcAction(daNpc_Md_c* i_this, u32 pmf, void* arg) {
    gabi::Local<ProcFunc_l> fn;
    md_pmf_load(fn, pmf);
    i_this->setNpcAction(fn, arg);
}

/* 022928A8 */
BOOL daNpc_Md_c::routeCheck(f32 param_1, be<s16>* param_2) {
    WWHD_FUNC(0x022928A8, BOOL, this, param_1, param_2);
    gabi::Local<be<f32>> temp2;
    if (!mAcch.ChkGroundHit()) {
        gabi::Local<cXyz> temp;
        f32 x = current.pos.x, z = current.pos.z;
        temp->x = x;
        temp->z = z;
        current.pos.z = old.pos.z; /* struct copy (integer words) */
        gabi::store<u32>(gabi::ea(&current.pos.x), gabi::load<u32>(gabi::ea(&old.pos.x)));
        f32 y = current.pos.y;
        speedF = 0.0f;
        temp->y = y;
        m3131 = 1;
        gabi::store<u32>(gabi::ea(&current.pos.y), gabi::load<u32>(gabi::ea(&old.pos.y)));
        gabi::Local<md_chk6C> lin_chk;
        dBgS_LinChk_ct(lin_chk, MD_LINCHK_VT, false);
        dBgS_LinChk_Set(lin_chk, temp, &current.pos, nullptr);
        if (cBgS_LineCross(dComIfG_Bgsp(), lin_chk)) {
            void* pla = dBgS_GetTriPla(dComIfG_Bgsp(), gabi::at<u8>(gabi::ea(lin_chk.get()) + 0x14));
            if (pla != nullptr) {
                s16 nrm = cM_atan2s(gabi::load<f32>(gabi::ea(pla)), gabi::load<f32>(gabi::ea(pla) + 8));
                if (cLib_distanceAngleS(*param_2, nrm) > 0x4000) {
                    md_LinChk_dt(lin_chk);
                    return TRUE;
                }
            }
        }
        if (mAcch.GetGroundH() - temp->y < -100.0f) {
            md_LinChk_dt(lin_chk);
            return FALSE;
        }
        *temp2 = 20.0f;
        md_setNpcAction(this, PMF_jumpNpcAction, temp2);
        md_LinChk_dt(lin_chk);
    } else {
        if (mAcch.ChkWallHit()) {
            f32 t = checkWallJump(*param_2);
            *temp2 = t;
            if (!(t < 0.0f)) {
                md_setNpcAction(this, PMF_jumpNpcAction, temp2);
                return TRUE;
            }
            if (param_1 > 360000.0f /* SQUARE(600.0f) */) {
                return FALSE;
            }
        }
        gabi::Local<cXyz> temp;
        gabi::Local<cXyz> tempB;
        s16 a = *param_2;
        f32 y = current.pos.y + 80.0f;
        temp->x = current.pos.x;
        tempB->x = gabi::fmadds(80.0f, cM_ssin(a), current.pos.x);
        tempB->y = y;
        temp->y = y;
        temp->z = current.pos.z;
        tempB->z = gabi::fmadds(80.0f, cM_scos(a), current.pos.z);
        routeWallCheck(temp, tempB, param_2);
    }
    return TRUE;
}
VERIFY(0x022928A8, &daNpc_Md_c::routeCheck);

/* 02445438 daPy_npc_c::setRestart(s8) */
static inline void md_setRestart(daNpc_Md_c* i_this, s8 opt) { gabi::call(0x02445438, i_this, opt); }
/* 0207A9A0 cLib_calcTimer<u8> (out of line) */
static inline u8 md_calcTimerU8(be<u8>* t) { return gabi::call<u8>(0x0207A9A0, t); }
/* 021E1E78 cLib_getRndValue<int>(base, range) (another TU's copy) */
static inline s32 md_getRndValue(s32 base, s32 range) { return gabi::call<s32>(0x021E1E78, base, range); }

/* 02292C48 HD: when the player is at a door (eventInfo command 3), Medli switches to the
 * HD-only action 0229310C instead of only refusing to follow */
BOOL daNpc_Md_c::searchNpcAction(void*) {
    WWHD_FUNC(0x02292C48, BOOL, this, (void*)nullptr);
    if (mActionStatus == ACTION_STARTING) {
        md_attnFlags(this) |= 0x10; /* fopAc_Attn_ACTION_CARRY_e */
        setBitStatus(daMdStts_UNK4);
        mAcchCir[1].SetWall(60.0f, 20.0f);
        if (m3104 == 0x11 || m3104 == 0x16 || m3104 == 0x17) {
            setAnm(0x13);
            mActionStatus = mActionStatus + 1;
        } else if (m3104 == 0x20) {
            setAnm(0x21);
            m312A = 0;
            mActionStatus = mActionStatus + 1;
        } else {
            setAnm(2);
            mActionStatus = mActionStatus + 2;
        }
        if (!mAcch.ChkGroundHit()) {
            f32 gndY = mAcch.GetGroundH();
            f32 delta = gndY - current.pos.y;
            if (delta < 0.0f && !(delta < -30.1f)) {
                speed.y = 0.0f;
                current.pos.y = gndY;
                mAcch.m_flags = mAcch.m_flags | 0x20; /* SetGroundHit() */
            }
        }
    } else if (mActionStatus != ACTION_ENDING) {
        if (mActionStatus == ACTION_ONGOING_1) {
            if (m312A != 0) {
                setAnm(2);
                mActionStatus = mActionStatus + 1;
            }
        } else {
            m312C = 1;
            fopAc_ac_c* player = dComIfGp_getPlayer(0);
            if (gabi::load<u16>(gabi::ea(player) + 0xF8) == 3) { /* eventInfo.checkCommandDoor() */
                MD_SET_NPC_ACTION(PMF_0229310C);
                return TRUE;
            }
            f32 dist_sq = fopAcM_searchPlayerDistanceXZ2(this);
            f32 temp;
            f32 sq = l_HIO().m0C4 * l_HIO().m0C4;
            if (dist_sq < sq) {
                temp = 0.0f;
            } else {
                f32 fVar2 = dist_sq - sq;
                if (fVar2 > 90000.0f) {
                    fVar2 = 90000.0f;
                }
                temp = fVar2 / 90000.0f;
                if (temp < 0.5f) {
                    temp = 0.5f;
                }
            }
            gabi::Local<be<s16>> adjustedAngle;
            s16 angle = fopAcM_searchPlayerAngleY(this);
            *adjustedAngle = angle;
            BOOL temp3 = FALSE;
            if (routeCheck(dist_sq, adjustedAngle) && cLib_distanceAngleS(angle, *adjustedAngle) <= 0x2000) {
                temp3 = TRUE;
            }
            if (!temp3 || (gabi::load<u32>(dComIfGp_ea() + 0x5CD8) & 0x02000101) /* dComIfGp_checkPlayerStatus0 */ ||
                (gabi::load<u32>(gabi::ea(player) + 0x3C0) & 0x10000) /* player->checkAttentionLock() */) {
                mNoResetFlg1 = mNoResetFlg1 & ~2u; /* offNpcCallCommand() */
                temp = 0.0f;
                s32 reverb = dComIfGp_getReverb(current.roomNo);
                mDoAud_monsSeStart_md(0x491E /* JA_SE_CV_MD_LEFT_ALONE */, &current.pos, md_procId(this), reverb);
                if (!temp3) {
                    MD_SET_NPC_ACTION(PMF_kyohiNpcAction);
                    return TRUE;
                }
            } else {
                md_setRestart(this, 2);
                if (md_calcTimerU8(&m3130) == 0) {
                    m312F = m312F ^ 1;
                    m3130 = (u8)md_getRndValue(8, 20);
                }
            }
            walkProc(temp, *adjustedAngle);
            cLib_addCalcAngleS(&shape_angle.y, current.angle.y, l_HIO().m008.m24, (s16)(l_HIO().m008.m20 * 2), (s16)(l_HIO().m008.m22 * 2));
            gabi::Local<be<s16>> temp4;
            *temp4 = shape_angle.y;
            gabi::Local<cXyz> vec2;
            dNpc_playerEyePos(vec2, l_HIO().mNpc.m04);
            gabi::Local<cXyz> vec;
            f32 ey = eyePos.y, z = current.pos.z;
            vec->y = ey;
            vec->z = z;
            s16 ang = shape_angle.y;
            vec->x = current.pos.x;
            md_lookAtTarget(&mJntCtrl, temp4, vec2, vec, ang, l_HIO().mNpc.mMaxTurnStep, false);
            if (speedF < 0.001f) {
                MD_SET_NPC_ACTION(PMF_waitNpcAction);
            } else {
                shape_angle.y = *temp4;
            }
            lookBack(1, 0, 0);
            setAttention(true);
        }
    }
    return TRUE;
}
VERIFY(0x02292C48, &daNpc_Md_c::searchNpcAction);

#define MD_M_FLYINGTIMER 0x101D5F38 /* s16 daNpc_Md_c::m_flyingTimer */
/* 02526C94 daNpc_Md_c::getMaxFlyingTimer() (static, out of line in another TU) */
static inline s16 md_getMaxFlyingTimer() { return gabi::call<s16>(0x02526C94); }
/* 02007898 / 020078BC CPad_CHECK_TRIG_A / _B(port) (HD: out of line) */
static inline BOOL md_trigA(s32 port) { return gabi::call<BOOL>(0x02007898, port); }
static inline BOOL md_trigB(s32 port) { return gabi::call<BOOL>(0x020078BC, port); }
/* dComIfGs_isEventBit / onEventBit: dSv_event_c at *(0x101F84DC) + 0x644 */
static inline dSv_event_c* md_event() { return gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x644); }

/* 02294160 */
BOOL daNpc_Md_c::flyPlayerAction(void*) {
    WWHD_FUNC(0x02294160, BOOL, this, (void*)nullptr);
    be<s16>* flyingTimer = gabi::at<be<s16>>(MD_M_FLYINGTIMER);
    if (mActionStatus == ACTION_STARTING) {
        setAnm(0x18);
        m3150 = current.pos.y + 1500.0f;
        m3144 = l_HIO().m1BE;
        m3146 = l_HIO().m1C0;
        *flyingTimer = md_getMaxFlyingTimer();
        speedF = 0.0f;
        mAcchCir[1].SetWall(60.0f, 60.0f);
        gabi::store<u8>(MD_M_FLYING, 1); /* onFlying() */
        mActionStatus = mActionStatus + 1;
    } else if (mActionStatus == ACTION_ENDING) {
        emitterDelete(m0508);
        deleteHane03Emitter();
        gabi::store<u8>(MD_M_FLYING, 0); /* offFlying() */
    } else {
        if (md_trigA(0)) {
            m3146 = l_HIO().m1C0;
        } else {
            md_calcTimer(&m3146);
        }
        if (md_trigA(0)) {
            m3144 = l_HIO().m1BE;
        } else {
            md_calcTimer(&m3144);
        }
        if (mActionStatus == ACTION_ONGOING_1) {
            if (m312A != 0) {
                setBitStatus(daMdStts_UNK1);
                setWingEmitter();
                setAnm(0x19);
                speedF = 0.0f;
                speed.y = 20.0f;
                gravity = l_HIO().m188;
                maxFallSpeed = l_HIO().m18C;
                m3144 = l_HIO().m1BE;
                m3146 = l_HIO().m1C0;
                mActionStatus = mActionStatus + 1;
            }
        } else {
            md_calcTimer(flyingTimer); /* calcFlyingTimer() */
            if (dSv_event_isEventBit(md_event(), 0x3320) && *flyingTimer < 300) {
                dSv_event_onEventBit(md_event(), 0x3310);
            }
            if (*flyingTimer == 0 || m3144 == 0 || md_trigB(0)) {
                if (checkStatus(daMdStts_UNK1)) {
                    m3135 = m3135 | 1; /* setBitEffectStatus(1) */
                }
                MD_SET_PLAYER_ACTION(PMF_jumpPlayerAction);
                if (*flyingTimer == 0) {
                    s32 reverb = dComIfGp_getReverb(current.roomNo);
                    mDoAud_monsSeStart_md(0x4920 /* JA_SE_CV_MD_FLY_END */, &current.pos, md_procId(this), reverb);
                }
                return TRUE;
            }
            if (m3104 != 0x2e && m312A != 0) {
                setAnm(7);
            }
            if (*flyingTimer < l_HIO().m1C4 && m3104 == 7) {
                setAnm(0x2e);
                setHane03Emitter();
            }
            u32 attention = dComIfGp_ea() + 0x5804; /* dComIfGp_getAttention() */
            if (!(md_stickValue(0) < l_HIO().m104) || md_attention_Lockon(attention)) {
                f32 dVar8 = md_stickValue(0) * l_HIO().m10C;
                if (0.0f < dVar8 && dVar8 < l_HIO().m0D8) {
                    dVar8 = l_HIO().m0D8;
                }
                s16 sVar2 = getStickAngY(0);
                cLib_distanceAngleS(sVar2, current.angle.y);
                f32 dVar7 = l_HIO().m0DC;
                if (!(md_stickValue(0) < l_HIO().m104)) {
                    cLib_addCalcAngleS(&current.angle.y, sVar2, 8, 0x2000, 0x400);
                }
                gabi::Local<cXyz> cStack_44;
                int iVar3 = calcStickPos(sVar2, cStack_44);
                if (iVar3 == 0) {
                    cLib_addCalcAngleS(&shape_angle.y, current.angle.y, 8, 0x2000, 0x400);
                }
                {
                    gabi::Local<be<s16>> tempAngle;
                    gabi::Local<cXyz> eye;
                    f32 z = current.pos.z, y = current.pos.y, x = current.pos.x;
                    eye->z = z;
                    eye->x = x;
                    s16 ang = shape_angle.y;
                    eye->y = y;
                    *tempAngle = ang;
                    md_lookAtTarget(&mJntCtrl, tempAngle, cStack_44, eye, ang, l_HIO().mNpc.mMaxTurnStep, false);
                    if (iVar3 > 0) {
                        shape_angle.y = *tempAngle;
                    } else if (md_stickValue(0) < l_HIO().m104) {
                        cLib_addCalcAngleS(&current.angle.y, shape_angle.y, 8, 0x2000, 0x400);
                    }
                }
                f32 maxF = l_HIO().m0D4;
                dVar8 = (dVar8 - maxF >= 0.0f) ? maxF : dVar8; /* fsel */
                cLib_chaseF(&speedF, dVar8, dVar7);
            } else {
                gabi::Local<cXyz> zero;
                cXyz* z0 = gabi::at<cXyz>(0x101FFBA8); /* cXyz::Zero */
                f32 x = z0->x, y = z0->y, z = z0->z;
                zero->x = x;
                zero->y = y;
                zero->z = z;
                md_lookAtTarget(&mJntCtrl, &shape_angle.y, nullptr, zero, shape_angle.y, 0, false);
                current.angle.y = shape_angle.y;
            }
            gravity = l_HIO().m188;
            if (m3146 != 0) {
                maxFallSpeed = 10.0f;
            } else {
                maxFallSpeed = l_HIO().m18C;
            }
            if (!(current.pos.y < m3150)) {
                current.pos.y = m3150;
            }
            if (mAcch.ChkGroundHit()) {
                *flyingTimer = 0;
                MD_SET_PLAYER_ACTION(PMF_landPlayerAction);
            }
        }
        if (!checkStatus(daMdStts_UNK2)) {
            if (checkStatus(daMdStts_UNK1)) {
                s16 sVar2 = shape_angle.y - current.angle.y;
                if (sVar2 < 0x6000 && sVar2 > -0x6000) {
                    if (sVar2 > 0x2000) {
                        sVar2 = 0x2000;
                    } else if (sVar2 < -0x2000) {
                        sVar2 = -0x2000;
                    }
                    cLib_addCalcAngleS(&shape_angle.z, sVar2, 8, 0x2000, 0x400);
                    cLib_addCalcAngleS(&shape_angle.x, 0, 8, 0x2000, 0x400);
                } else {
                    cLib_addCalcAngleS(&shape_angle.z, 0, 8, 0x2000, 0x400);
                    cLib_addCalcAngleS(&shape_angle.x, -0x2000, 8, 0x2000, 0x400);
                }
            } else {
                shape_angle.x = 0;
                current.angle.y = shape_angle.y;
                shape_angle.z = 0;
            }
        }
        setAttention(true);
    }
    return TRUE;
}
VERIFY(0x02294160, &daNpc_Md_c::flyPlayerAction);
