/**
 * d_a_wz.cpp (WWHD)
 * Enemy - Wizzrobe / Mini-Boss - Wizzrobe (Wind Temple)
 *
 * The GameCube decompilation (zeldaret/tww
 * src/d/actor/d_a_wz.cpp) has only "Nonmatching" placeholders for this unit, so every function
 * here is written from the WWHD code (cking.rpx) and verified against it. GameCube names are
 * kept where the matcher found them; the GameCube helpers hontai_draw / summon_door_draw /
 * damage_ball_draw / action_dousa / action_itai / summon_call_sub / sea_water_check /
 * action_tama_dousa are inlined into their WWHD callers.
 */
#include "d/actor/d_a_wz.h"

#define M_arcname STR(0x10042DC8)     /* "Wz" */
#define SAFESTRING_VTBL 0x10042D38    /* this TU's sead::SafeString vtable */
#define ACT_VTBL 0x10042D90           /* HD: wz_class vtable */

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline void* wzRes(u32 arc, s32 index) { return dComIfG_getObjectRes(STR(arc), index, SAFESTRING_VTBL); }
static inline void enemy_fire(void* f) { gabi::call(0x02041570, f); }        /* 02041570 enemy_fire(enemyfire*) */
static inline void enemy_fire_remove(void* f) { gabi::call(0x02041C30, f); } /* 02041C30 */
static inline void mDoAud_subBgmStop() { gabi::call(0x025E1928); }
static inline void dBgS_AcchCir_SetWall(void* c, f32 h, f32 r) { gabi::call(0x024EFF44, c, h, r); }
/* mDoExt_McaMorf::mDoExt_McaMorf (HD: this first; NULL allocates) */
static inline u32 new_McaMorf(void* data, void* anm, s32 loop, f32 morf, s32 i1, s32 i2, s32 i3, u32 modelFlag, u32 dlFlag) {
    return gabi::call<u32>(0x025E4F64, (void*)nullptr, data, (void*)nullptr, (void*)nullptr, anm, loop, morf, i1, i2, i3, (void*)nullptr,
                           modelFlag, dlFlag);
}
/* 027F3F94: J3DModelData joint-tree header (u16 joint count at +8; matcher name __nw is wrong) */
static inline u16 modelData_jointNum(u32 data) { return gabi::load<u16>(gabi::call<u32>(0x027F3F94, gabi::at<void>(data)) + 8); }
static inline u32 morf_model(u32 morf) { return gabi::load<u32>(morf + 0x90); }
/* J3DModel: user area +0xB8, model data +0xAC, matrix buffer +0x2C (flags u16 +4, anm matrices +0x10) */
/* 025E1AA4 mDoAud_monsSeStart(id, pos, procId, param, reverb) */
static inline void mDoAud_monsSeStart(u32 id, cXyz* pos, u32 procId, u32 param, s32 reverb) { gabi::call(0x025E1AA4, id, pos, procId, param, reverb); }
/* 025D5A20 fopAcM_createChild(name, parentId, param, pos, roomNo, angle, scale, subtype, createFunc) */
static inline fpc_ProcID fopAcM_createChild(s16 name, fpc_ProcID parent, u32 param, cXyz* pos, s32 roomNo, csXyz* angle, cXyz* scale, s8 subtype,
                                            u32 createFunc) {
    return gabi::call<fpc_ProcID>(0x025D5A20, name, parent, param, pos, roomNo, angle, scale, subtype, createFunc);
}
static inline void cpy_u32(u32 dst, u32 src) { gabi::store<u32>(dst, gabi::load<u32>(src)); }
static inline s32 dComIfGp_CharTbl_GetNameIndex_wz(const char* name) { return gabi::call<s32>(0x0200E814, gabi::at<u8>(dComIfGp_ea() + 0x50AC), name, 0); }
/* 024E9D00 / 024E78AC: this TU's SafeString virtuals */

/* ---- joint callbacks ---- */
static BOOL wz_jointCallBack(J3DNode* node, s32 timing, u16 jnt, u32 mtx_off, u32 out_off) {
    if (timing == 0) {
        J3DJoint* joint = J3DNode_toJoint(node);
        u32 model = gabi::load<u32>(0x104B462C); /* j3dSys.mModel */
        u32 actor = gabi::load<u32>(model + 0xB8);
        u16 no = gabi::load<u16>(gabi::ea(joint) + 4);
        if (actor != 0 && no == jnt) {
            u32 blk = gabi::load<u32>(model + 0x2C);
            gabi::store<u16>(blk + 4, (u16)(gabi::load<u16>(blk + 4) | 0x10));
            PSMTXCopy(gabi::at<Mtx34>(gabi::load<u32>(blk + 0x10) + mtx_off), calc_mtx());
            gabi::Local<cXyz> zero;
            zero->x = 0.0f;
            zero->y = 0.0f;
            zero->z = 0.0f;
            MtxPosition(zero.get(), gabi::at<cXyz>(actor + out_off));
            blk = gabi::load<u32>(model + 0x2C);
            Mtx34* calc = calc_mtx();
            gabi::store<u16>(blk + 4, (u16)(gabi::load<u16>(blk + 4) | 0x10));
            mtx_copy(gabi::at<Mtx34>(gabi::load<u32>(blk + 0x10) + mtx_off), calc);
            PSMTXCopy(calc_mtx(), gabi::at<Mtx34>(0x104B4868));
        }
    }
    return TRUE;
}

/* 024E1EE8 */
static BOOL nodeCallBack(J3DNode* node, s32 timing) {
    WWHD_FUNC(0x024E1EE8, BOOL, node, timing);
    return wz_jointCallBack(node, timing, 0x11, 0x330, 0x418); /* hand: mHandPos */
}
VERIFY(0x024E1EE8, nodeCallBack);

/* 024E2018 */
static BOOL rod_nodeCallBack(J3DNode* node, s32 timing) {
    WWHD_FUNC(0x024E2018, BOOL, node, timing);
    return wz_jointCallBack(node, timing, 2, 0x60, 0x43C); /* rod tip: mRodTipPos */
}
VERIFY(0x024E2018, rod_nodeCallBack);

/* a model's base matrix := joint `jnt` of another model; the destination model and the source
 * matrix base are read before the dirty flag (u16) is stored */
static void wz_jointMtxCopy(be<u32>& srcMorf, u32 jnt, be<u32>& dstMorf) {
    u32 blk = gabi::load<u32>(morf_model(srcMorf) + 0x2C);
    u32 dst = morf_model(dstMorf);
    u16 fl = gabi::load<u16>(blk + 4);
    u32 mtx = gabi::load<u32>(blk + 0x10);
    gabi::store<u16>(blk + 4, (u16)(fl | 0x10));
    mtx_copy(gabi::at<Mtx34>(dst + 0xC8), gabi::at<Mtx34>(mtx + jnt * 0x30));
}

/* 024E2148 */
void draw_SUB(wz_class* i_this) {
    WWHD_FUNC(0x024E2148, void, i_this);
    u32 morf = i_this->mpMorf;
    f32 sz = i_this->scale.z;
    u32 model = morf_model(morf);
    f32 sx = i_this->scale.x, sy = i_this->scale.y;
    gabi::store<f32>(model + 0xBC, sx);
    gabi::store<f32>(model + 0xC0, sy);
    gabi::store<f32>(model + 0xC4, sz);
    mDoMtx_stack_c::transS(i_this->current.pos.x, i_this->current.pos.y, i_this->current.pos.z);
    mDoMtx_stack_c::YrotM(i_this->shape_angle.y);
    mDoMtx_XrotM(mDoMtx_stack_c::get(), i_this->shape_angle.x);
    mDoMtx_ZrotM(mDoMtx_stack_c::get(), i_this->shape_angle.z);
    mtx_copy(gabi::at<Mtx34>(model + 0xC8), mDoMtx_stack_c::get());
    McaMorf_calc(gabi::at<mDoExt_McaMorf_c>(i_this->mpMorf));
    if (i_this->mBehaviorType < 10) {
        if (i_this->mIsMiniBoss) {
            /* the second body follows joint 0x13 */
            wz_jointMtxCopy(i_this->mpMorf, 0x13, i_this->mpMorf2);
            McaMorf_calc(gabi::at<mDoExt_McaMorf_c>(i_this->mpMorf2));
        }
        u32 rodMorf = i_this->mpRodMorf;
        f32 rx = i_this->mRodScaleX;
        u32 rod = morf_model(rodMorf);
        f32 rz = i_this->mRodScaleZ, ry = i_this->mRodScaleY;
        gabi::store<f32>(rod + 0xBC, rx);
        gabi::store<f32>(rod + 0xC0, ry);
        gabi::store<f32>(rod + 0xC4, rz);
        /* the rod is held in the hand (joint 0x11) */
        wz_jointMtxCopy(i_this->mpMorf, 0x11, i_this->mpRodMorf);
        McaMorf_calc(gabi::at<mDoExt_McaMorf_c>(i_this->mpRodMorf));
        enemy_fire(i_this->mEnemyFire);
    }
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &i_this->current.pos, &i_this->tevStr);
}
VERIFY(0x024E2148, draw_SUB);

/* 024E23B4: GXColor -> four floats (r, g, b, a) / 255 (HD material colour helper) */
struct wzColorF {
    be<f32> r, g, b, a;
};
static void wz_colorToF(wzColorF* out, const GXColor* in) {
    WWHD_FUNC(0x024E23B4, void, out, in);
    f32 r = (f32)(u8)in->r / 255.0f;
    f32 g = (f32)(u8)in->g / 255.0f;
    f32 b = (f32)(u8)in->b / 255.0f;
    f32 a = (f32)(u8)in->a / 255.0f;
    out->r = r;
    out->g = g;
    out->b = b;
    out->a = a;
}
VERIFY(0x024E23B4, wz_colorToF);

/* 024E2A90 */
void anm_init(wz_class* i_this, int bckFileIdx, f32 morf, u8 loopMode, f32 playSpeed, int soundFileIdx) {
    WWHD_FUNC(0x024E2A90, void, i_this, bckFileIdx, morf, loopMode, playSpeed, soundFileIdx);
    i_this->mCurrBckIdx = bckFileIdx;
    if (soundFileIdx >= 0) {
        void* anm = wzRes(0x10042DC8, bckFileIdx);
        void* snd = wzRes(0x10042DC8, soundFileIdx);
        McaMorf_setAnm(gabi::at<mDoExt_McaMorf_c>(i_this->mpMorf), (J3DAnmTransform*)anm, loopMode, morf, playSpeed, 0.0f, -1.0f, snd);
    } else {
        void* anm = wzRes(0x10042DC8, bckFileIdx);
        McaMorf_setAnm(gabi::at<mDoExt_McaMorf_c>(i_this->mpMorf), (J3DAnmTransform*)anm, loopMode, morf, playSpeed, 0.0f, -1.0f,
                       nullptr);
    }
}
VERIFY(0x024E2A90, anm_init);

/* 024E2BBC */
void rod_size_set(wz_class* i_this, u8 shrink) {
    WWHD_FUNC(0x024E2BBC, void, i_this, shrink);
    if (shrink == 0) {
        cLib_addCalc2(&i_this->mRodScaleX, 1.0f, 0.2f, 0.5f);
        cpy_u32(gabi::ea(&i_this->mRodScaleZ), gabi::ea(&i_this->mRodScaleX));
        cpy_u32(gabi::ea(&i_this->mRodScaleY), gabi::ea(&i_this->mRodScaleX));
    } else {
        cLib_addCalc0(&i_this->mRodScaleX, 0.2f, 0.5f);
        cpy_u32(gabi::ea(&i_this->mRodScaleY), gabi::ea(&i_this->mRodScaleX));
        cpy_u32(gabi::ea(&i_this->mRodScaleZ), gabi::ea(&i_this->mRodScaleX));
    }
}
VERIFY(0x024E2BBC, rod_size_set);

/* 024E359C */
void BG_check(wz_class* i_this) {
    WWHD_FUNC(0x024E359C, void, i_this);
    dBgS_AcchCir_SetWall(i_this->mAcchCir, i_this->mAcchWallH, i_this->mAcchWallR);
    i_this->mCorrectionOffsetY = 20.0f;
    i_this->old.pos.y = i_this->old.pos.y - 20.0f;
    i_this->current.pos.y = i_this->current.pos.y - 20.0f;
    dBgS_Acch_CrrPos(i_this->mAcch, dComIfG_Bgsp());
    f32 off = i_this->mCorrectionOffsetY;
    i_this->current.pos.y = i_this->current.pos.y + off;
    i_this->old.pos.y = i_this->old.pos.y + off;
}
VERIFY(0x024E359C, BG_check);

/* 024E3950 (cXyz by value: pointer to the caller's copy) */
void next_tama_move(wz_class* i_this, const cXyz* pos) {
    WWHD_FUNC(0x024E3950, void, i_this, pos);
    cpy_u32(gabi::ea(&i_this->mTamaTarget.x), gabi::ea(&pos->x));
    cpy_u32(gabi::ea(&i_this->mTamaTarget.y), gabi::ea(&pos->y));
    i_this->speedF = 0.0f;
    i_this->m469 = 0x68;
    u32 at = gabi::ea(i_this->mBallSph); /* cCcD_ObjAt SPrm: OffAtSetBit */
    gabi::store<u32>(at, gabi::load<u32>(at) & ~1u);
    cpy_u32(gabi::ea(&i_this->mTamaTarget.z), gabi::ea(&pos->z));
}
VERIFY(0x024E3950, next_tama_move);

/* 024E61D8 */
static BOOL daWZ_IsDelete(wz_class*) {
    WWHD_FUNC(0x024E61D8, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x024E61D8, daWZ_IsDelete);

/* 024E78C0 */
void fuwafuwa_calc(wz_class* i_this) {
    WWHD_FUNC(0x024E78C0, void, i_this);
    s16 a = (s16)(i_this->mFuwaAngle + 0x7D0);
    f32 base = i_this->mBaseY + 15.0f;
    i_this->mFuwaAngle = a;
    i_this->current.pos.y = gabi::fmadds(cM_ssin(a), 5.0f, base);
}
VERIFY(0x024E78C0, fuwafuwa_calc);

/* ---- compiler-generated ---- */

/* 024E78AC: sead::SafeString deleting destructor (this TU's copy) */
static void wz_SafeString_dt(void* p, s32 flags) {
    WWHD_FUNC(0x024E78AC, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x024E78AC, wz_SafeString_dt);

/* 024E9D00: SafeString empty virtual */
static void wz_SafeString_empty(void* p) {
    WWHD_FUNC(0x024E9D00, void, p);
}
VERIFY(0x024E9D00, wz_SafeString_empty);

/* 024E7818: __sinit_d_a_wz_cpp (header statics only) */
static void __sinit_d_a_wz_cpp() {
    WWHD_FUNC(0x024E7818, void);
    sinit_header_statics(0x1046EC1C, 0x101D3B48);
}
VERIFY(0x024E7818, __sinit_d_a_wz_cpp);

/* 024E6984: enemyfire constructor (0x22C: stts at +0xA0, GStts +0xBC, dCcD_Sph +0xDC) */
static void* enemyfire_ct(void* p_) {
    WWHD_FUNC(0x024E6984, void*, p_);
    u32 p = gabi::ea(p_);
    if (p == 0) {
        p = gabi::ea(operator_new(0x22C));
        if (p == 0)
            return nullptr;
    }
    if (p + 0x8C == 0) /* inline sub-object constructor with this == NULL */
        operator_new(0xC);
    gabi::call(0x0200BD2C, gabi::at<void>(p + 0xA0)); /* cCcD_Stts */
    gabi::call(0x02515DA0, gabi::at<void>(p + 0xBC)); /* dCcD_GStts */
    gabi::store<u32>(p + 0xB8, 0x1004AE88);
    gabi::store<u32>(p + 0xBC, 0x1004AEC0);
    gabi::call(0x025166F0, gabi::at<void>(p + 0xDC)); /* dCcD_Sph */
    gabi::store<f32>(p + 0x228, 1.0f);
    return gabi::at<void>(p);
}
VERIFY(0x024E6984, enemyfire_ct);

/* 024E6A10: wz_class::wz_class() */
static void* wz_class_ct(void* p_) {
    WWHD_FUNC(0x024E6A10, void*, p_);
    u32 p = gabi::ea(p_);
    if (p == 0) {
        p = gabi::ea(operator_new(0x10B0));
        if (p == 0)
            return nullptr;
    }
    auto m = [&](u32 off) { return gabi::at<void>(p + off); };
    fopAc_ac_c_ct(gabi::at<fopAc_ac_c>(p));
    gabi::store<u32>(p + 0xB4, ACT_VTBL);
    gabi::call(0x024EFE94, m(0x580)); /* dBgS_AcchCir */
    gabi::call(0x024F0474, m(0x5C0)); /* dBgS_Acch */
    gabi::store<u32>(p + 0x5D0, 0x10042D60);
    gabi::store<u32>(p + 0x5E0, 0x10042D70);
    gabi::store<u32>(p + 0x5D4, 0x10042D80);
    gabi::store<u8>(p + 0x5D8, 1);
    gabi::call(0x0200BD2C, m(0x784));
    gabi::call(0x02515DA0, m(0x7A0));
    gabi::store<u32>(p + 0x79C, 0x1004AE88);
    gabi::store<u32>(p + 0x7A0, 0x1004AEC0);
    gabi::call(0x02515FB8, m(0x7C0)); /* dCcD_GObjInf (dCcD_Cyl) */
    gabi::store<u32>(p + 0x8D4, 0x100015A8);
    gabi::store<u32>(p + 0x8D0, 0x10042D50);
    gabi::call(0x02018590, m(0x8D8));
    gabi::store<u32>(p + 0x7FC, 0x1004B108);
    gabi::store<u32>(p + 0x8EC, 0x1004B150);
    gabi::store<u32>(p + 0x8D4, 0x1004B160);
    gabi::call(0x025166F0, m(0x8F0)); /* dCcD_Sph */
    gabi::store<f32>(p + 0xA3C, 1.0f);
    gabi::call(0x0200BD2C, m(0xA70));
    gabi::call(0x02515DA0, m(0xA8C));
    gabi::store<u32>(p + 0xA88, 0x1004AE88);
    gabi::store<u32>(p + 0xA8C, 0x1004AEC0);
    gabi::call(0x02515FB8, m(0xAAC));
    gabi::store<u32>(p + 0xBC0, 0x100015A8);
    gabi::store<u32>(p + 0xBBC, 0x10042D50);
    gabi::call(0x02018590, m(0xBC4));
    gabi::store<u32>(p + 0xBC0, 0x1004B160);
    gabi::store<u32>(p + 0xBD8, 0x1004B150);
    gabi::store<u32>(p + 0xAE8, 0x1004B108);
    gabi::call(0x024EFE94, m(0xBF4));
    gabi::call(0x024F0474, m(0xC34));
    gabi::store<u32>(p + 0xC44, 0x10042D60);
    gabi::store<u32>(p + 0xC48, 0x10042D80);
    gabi::store<u32>(p + 0xC54, 0x10042D70);
    gabi::store<u8>(p + 0xC4C, 1);
    enemyfire_ct(m(0xDF8));
    for (u32 off = 0x1038; off <= 0x1088; off += 0x14)
        dPa_followEcallBack_ct(gabi::at<dPa_followEcallBack>(p + off), 0, 0);
    gabi::call(0x025E895C, m(0x109C)); /* mDoExt_invisibleModel */
    gabi::call(0x025E895C, m(0x10A4));
    return gabi::at<void>(p);
}
VERIFY(0x024E6A10, wz_class_ct);

/* 024E9BE0: wz_class deleting destructor */
static void wz_class_dt(void* p_, s32 flags) {
    WWHD_FUNC(0x024E9BE0, void, p_, flags);
    u32 p = gabi::ea(p_);
    if (p == 0)
        return;
    auto m = [&](u32 off) { return gabi::at<void>(p + off); };
    gabi::call(0x025E89F8, m(0x10A4), 2);
    gabi::call(0x025E89F8, m(0x109C), 2);
    gabi::call(0x02515AE8, m(0xED4), 2);
    gabi::call(0x02515860, m(0xE98), 2);
    gabi::store<u32>(p + 0xC54, 0x10042D70);
    gabi::store<u32>(p + 0xC48, 0x10042D80);
    gabi::call(0x024EFD9C, m(0xC34), 0);
    gabi::call(0x02018034, m(0xC08), 2);
    gabi::call(0x02515A70, m(0xAAC), 2);
    gabi::call(0x02515860, m(0xA70), 2);
    gabi::call(0x02515AE8, m(0x8F0), 2);
    gabi::call(0x02515A70, m(0x7C0), 2);
    gabi::call(0x02515860, m(0x784), 2);
    gabi::store<u32>(p + 0x5E0, 0x10042D70);
    gabi::store<u32>(p + 0x5D4, 0x10042D80);
    gabi::call(0x024EFD9C, m(0x5C0), 0);
    gabi::call(0x02018034, m(0x594), 2);
    gabi::call(0x025D50BC, m(0), 0);
    if (flags & 1)
        operator_delete(p_);
}
VERIFY(0x024E9BE0, wz_class_dt);

/* ---- delete / heap ---- */

/* HD strcmp(dComIfGp_getStartStageName(), name) == 0: SafeString compare (as d_a_bk) */
static inline void wz_SafeString_vcall(SafeString* s) { gabi::call_ptr(gabi::load<u32>(s->__vtbl + 0x14), s); }
static bool wz_isStartStage(const char* name) {
    gabi::Local<SafeString> a;
    a->mStringTop = gabi::ea(name);
    a->__vtbl = SAFESTRING_VTBL;
    u32 stage = dComIfGp_ea() + 0x5134;
    gabi::Local<SafeString> b;
    b->__vtbl = SAFESTRING_VTBL;
    b->mStringTop = stage;
    wz_SafeString_vcall(a);
    wz_SafeString_vcall(a);
    u32 pa = a->mStringTop;
    wz_SafeString_vcall(b);
    u32 pb = b->mStringTop;
    if (pa == pb)
        return true;
    for (u32 i = 0; i < 0x40001; i++) {
        u8 ca = gabi::load<u8>(pa + i);
        u8 cb = gabi::load<u8>(pb + i);
        if (ca != cb)
            return false;
        if (ca == 0)
            return true;
    }
    return false;
}

/* 024E61E0 */
static BOOL daWZ_Delete(wz_class* i_this) {
    WWHD_FUNC(0x024E61E0, BOOL, i_this);
    if (i_this->health == -0x80) { /* killed */
        if (i_this->mIsMiniBoss)
            mDoAud_subBgmStop();
        if (!wz_isStartStage(STR(0x10042E38) /* "kazeMB" */) && i_this->mDisableSpawnOnDeathSwitch != 0xFF)
            dComIfGs_onSwitch(i_this->mDisableSpawnOnDeathSwitch, i_this->current.roomNo);
    }
    u8 type = i_this->mBehaviorType;
    if (type == 12) {
        /* a summoned Wizzrobe: tell the summoner */
        fopAc_ac_c* parent = fopAcM_SearchByID(i_this->mParentId);
        if (parent != nullptr)
            gabi::store<u8>(gabi::ea(parent) + 0x470, 0);
        dComIfG_resDelete((request_of_phase_process_class*)i_this->mPhase, STR(0x10042E34) /* "Wzb" */);
    } else if (type == 13) {
        dComIfG_resDelete((request_of_phase_process_class*)i_this->mPhase, STR(0x10042E34) /* "Wzb" */);
    } else {
        dComIfG_resDelete((request_of_phase_process_class*)i_this->mPhase, STR(0x10042E40) /* "Wz" */);
    }
    type = i_this->mBehaviorType;
    if (type == 10 || type == 11)
        dKy_plight_cut((LIGHT_INFLUENCE*)i_this->mLight);
    for (int i = 0; i < 5; i++)
        gabi::at<dPa_followEcallBack>(gabi::ea(i_this->mFollowCb) + 0x14 * i)->remove();
    if (i_this->mBehaviorType < 10)
        enemy_fire_remove(i_this->mEnemyFire);
    return TRUE;
}
VERIFY(0x024E61E0, daWZ_Delete);

/* 024E6420 */
static BOOL useHeapInit(fopAc_ac_c* a_this) {
    WWHD_FUNC(0x024E6420, BOOL, a_this);
    wz_class* i_this = (wz_class*)a_this;
    void* bmd = wzRes(0x10042E43, 0x18);
    void* bck = wzRes(0x10042E43, 0x13);
    i_this->mpMorf = new_McaMorf(bmd, bck, 2, 1.0f, 0, -1, 1, 0x80000, 0x37441422);
    if (i_this->mpMorf == 0 || morf_model(i_this->mpMorf) == 0)
        return FALSE;
    gabi::store<u32>(morf_model(i_this->mpMorf) + 0xB8, gabi::ea(i_this));
    for (u16 i = 0; i < modelData_jointNum(gabi::load<u32>(morf_model(i_this->mpMorf) + 0xAC)); i = (u16)(i + 1)) {
        u32 data = gabi::load<u32>(morf_model(i_this->mpMorf) + 0xAC);
        u32 num = gabi::load<u32>(data + 4);
        u32 jnt = gabi::load<u32>(data + 8);
        if (i < num)
            jnt += i * 0x1C;
        gabi::store<u32>(jnt + 8, 0x024E1EE8 /* nodeCallBack */);
    }
    if (i_this->mBehaviorType > 10)
        return TRUE;
    if (i_this->mIsMiniBoss) {
        void* bmd2 = wzRes(0x10042E43, 0x19);
        i_this->mpMorf2 = new_McaMorf(bmd2, nullptr, 0, 0.0f, 0, -1, 1, 0x80000, 0x37441422);
        if (i_this->mpMorf2 == 0 || morf_model(i_this->mpMorf2) == 0)
            return FALSE;
        if (!gabi::call<BOOL>(0x025E8A48, i_this->mInvisModel2, gabi::at<void>(morf_model(i_this->mpMorf2))))
            return FALSE;
    }
    void* rod = wzRes(0x10042E43, 0x1A);
    i_this->mpRodMorf = new_McaMorf(rod, nullptr, 2, 0.0f, 0, -1, 1, 0, 0x11020203);
    if (i_this->mpRodMorf == 0 || morf_model(i_this->mpRodMorf) == 0)
        return FALSE;
    gabi::store<u32>(morf_model(i_this->mpRodMorf) + 0xB8, gabi::ea(i_this));
    for (u16 i = 0; i < modelData_jointNum(gabi::load<u32>(morf_model(i_this->mpRodMorf) + 0xAC)); i = (u16)(i + 1)) {
        u32 data = gabi::load<u32>(morf_model(i_this->mpRodMorf) + 0xAC);
        u32 num = gabi::load<u32>(data + 4);
        u32 jnt = gabi::load<u32>(data + 8);
        if (i < num)
            jnt += i * 0x1C;
        gabi::store<u32>(jnt + 8, 0x024E2018 /* rod_nodeCallBack */);
    }
    u32 model = morf_model(i_this->mpMorf);
    void* brk = operator_new(0x78);
    if (brk != nullptr)
        brk = gabi::call<void*>(0x025E80D0, brk); /* mDoExt_brkAnm */
    i_this->mpBrkAnm = gabi::ea(brk);
    if (brk == nullptr)
        return FALSE;
    void* res = wzRes(0x10042E43, 0x1D);
    if (!gabi::call<BOOL>(0x025E8154, gabi::at<void>(i_this->mpBrkAnm), gabi::at<void>(gabi::load<u32>(model + 0xAC)), res, 1, 0, 1.0f, 0, -1,
                          0, 0))
        return FALSE;
    if (!gabi::call<BOOL>(0x025E8A48, i_this->mInvisModel, gabi::at<void>(morf_model(i_this->mpMorf))))
        return FALSE;
    return TRUE;
}
VERIFY(0x024E6420, useHeapInit);

/* 024E67FC: the summoner (types 12/13, archive "Wzb") */
static BOOL useHeapInit2(fopAc_ac_c* a_this) {
    WWHD_FUNC(0x024E67FC, BOOL, a_this);
    wz_class* i_this = (wz_class*)a_this;
    void* bmd = wzRes(0x10042E48, 8);
    void* bck = wzRes(0x10042E48, 5);
    i_this->mpMorf = new_McaMorf(bmd, bck, 0, 1.0f, 0, -1, 1, 0x80000, 0x19000222);
    if (i_this->mpMorf == 0 || morf_model(i_this->mpMorf) == 0)
        return FALSE;
    u32 model = morf_model(i_this->mpMorf);
    void* btk = operator_new(0x74);
    if (btk != nullptr)
        btk = gabi::call<void*>(0x025E7C6C, btk); /* mDoExt_btkAnm */
    i_this->mpBtkAnm = gabi::ea(btk);
    if (btk == nullptr)
        return FALSE;
    void* res = wzRes(0x10042E48, 0xB);
    if (!gabi::call<BOOL>(0x025E7CE0, gabi::at<void>(i_this->mpBtkAnm), gabi::at<void>(gabi::load<u32>(model + 0xAC)), res, 1, 0, 1.0f, 0, -1,
                          0, 0))
        return FALSE;
    return TRUE;
}
VERIFY(0x024E67FC, useHeapInit2);

/* ---- draw ---- */

/* HD material colour: the material's kColor 3 (virtual +0x4C on the TEV block at +0x18) gets the
 * actor's alpha, is written back (virtual +0x3C), and its float copy goes to the HD colour block
 * (027F9F0C(&mat->m_flags, 10), the flags get 0x400) */
static u32 wz_tevKColor(u32 mat) {
    u32 blk = gabi::load<u32>(mat + 0x18);
    return gabi::call_ptr<u32>(gabi::load<u32>(gabi::load<u32>(blk + 4) + 0x4C), gabi::at<void>(blk), 3);
}
static void wz_setMaterialAlpha(wz_class* i_this, u32 data) {
    for (u16 i = 0; i < gabi::load<u16>(gabi::call<u32>(0x027F3F8C, gabi::at<void>(data)) + 0x24); i = (u16)(i + 1)) {
        u32 mat = gabi::load<u32>(data + 0x10);
        if (i < gabi::load<u32>(data + 0xC))
            mat += i * 0x39C;
        u32 c = wz_tevKColor(mat);
        gabi::store<u8>(c + 3, (u8)i_this->mAlpha);
        c = wz_tevKColor(mat);
        u32 blk = gabi::load<u32>(mat + 0x18);
        gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(blk + 4) + 0x3C), gabi::at<void>(blk), 3, gabi::at<void>(c));
        gabi::Local<wzColorF> in;
        gabi::Local<wzColorF> conv;
        wz_colorToF(in.get(), gabi::at<GXColor>(c));
        gabi::call(0x0274D458, conv.get(), in.get(), 1.0f);
        gabi::store<u32>(mat + 0xA0, gabi::load<u32>(mat + 0xA0) | 0x400);
        u32 out = gabi::call<u32>(0x027F9F0C, gabi::at<void>(mat + 0xA0), 10);
        f32 a = (f32)gabi::load<u8>(c + 3) / 255.0f;
        f32 r = conv->r, g = conv->g, b = conv->b; /* lfs/stfs: a signalling NaN is quieted */
        gabi::store<f32>(out + 4, g);
        gabi::store<f32>(out + 8, b);
        gabi::store<f32>(out + 0, r);
        gabi::store<f32>(out + 0xC, a);
    }
}
static inline void J3DModel_setHD58E0(u32 model, u32 v) { gabi::call(0x027F58E0, gabi::at<void>(model), v); }
static inline void invisibleModel_entry(void* m) { gabi::call(0x025E8BC4, m); }
static inline void invisibleModel_entryOpa(void* m) { gabi::call(0x025E8EC0, m); }

/* 024E2468 (GameCube daWZ_Draw with hontai_draw / summon_door_draw / damage_ball_draw inlined) */
static BOOL daWZ_Draw(wz_class* i_this) {
    WWHD_FUNC(0x024E2468, BOOL, i_this);
    u8 type = i_this->mBehaviorType;
    if (type == 10 || type == 11) {
        /* damage ball: only the matrix */
        if (i_this->m510 == 0)
            return TRUE;
        MtxTrans(i_this->current.pos.x, i_this->current.pos.y, i_this->current.pos.z, 0);
        MtxScale(0.6f, 0.6f, 0.6f, 1);
        mDoMtx_YrotM(calc_mtx(), i_this->shape_angle.y);
        PSMTXCopy(calc_mtx(), &i_this->mBallMtx);
        return TRUE;
    }
    u32 model = morf_model(i_this->mpMorf);
    if (type >= 10) {
        /* summon door */
        setLightTevColorType(dKy_getEnvlight(), gabi::at<J3DModel>(model), &i_this->tevStr);
        u32 btk = i_this->mpBtkAnm;
        gabi::call(0x025E7FC4, gabi::at<void>(btk), gabi::at<void>(gabi::load<u32>(model + 0xAC)), gabi::load<f32>(btk + 4));
        gabi::call(0x025E54D8, gabi::at<void>(i_this->mpMorf)); /* mDoExt_McaMorf::updateDL */
        gabi::store<u32>(gabi::load<u32>(model + 0xAC) + 0x44, 0);
        return TRUE;
    }
    u32 data = gabi::load<u32>(model + 0xAC);
    u32 rodModel = morf_model(i_this->mpRodMorf);
    if (i_this->mEnableSpawnSwitch != 0xFF && !dComIfGs_isSwitch(i_this->mEnableSpawnSwitch, gabi::load<s8>(0x1047E6C8)))
        return TRUE;
    if (!i_this->mIsMiniBoss && i_this->mAlpha >= 0x80)
        dSnap_RegistFig(0xC4, i_this, 1.0f, 1.0f, 1.0f);
    setLightTevColorType(dKy_getEnvlight(), gabi::at<J3DModel>(model), &i_this->tevStr);
    setLightTevColorType(dKy_getEnvlight(), gabi::at<J3DModel>(rodModel), &i_this->tevStr);
    wz_setMaterialAlpha(i_this, data);
    if (i_this->mIsMiniBoss) {
        u32 model2 = morf_model(i_this->mpMorf2);
        u32 data2 = gabi::load<u32>(model2 + 0xAC);
        setLightTevColorType(dKy_getEnvlight(), gabi::at<J3DModel>(model2), &i_this->tevStr);
        wz_setMaterialAlpha(i_this, data2);
        if (i_this->mAlpha >= 0x80)
            dSnap_RegistFig(0xC5, i_this, 1.0f, 1.0f, 1.0f);
    }
    u32 translucent = i_this->mAlpha != 0xFF ? 1 : 0;
    J3DModel_setHD58E0(model, translucent);
    if (i_this->mIsMiniBoss)
        J3DModel_setHD58E0(morf_model(i_this->mpMorf2), translucent);
    if (i_this->mIceTimer > 20) {
        /* frozen */
        if (i_this->mAlpha != 0xFF) {
            gabi::call(0x0259138C, gabi::at<void>(i_this->mpMorf), -1, 0); /* dMat_ice_c::entryDL */
            if (i_this->mIsMiniBoss)
                gabi::call(0x0259138C, gabi::at<void>(i_this->mpMorf2), -1, 0);
        } else {
            gabi::call(0x02591414, gabi::at<void>(i_this->mpMorf), -1, i_this->mInvisModel);
            if (i_this->mIsMiniBoss)
                gabi::call(0x02591414, gabi::at<void>(i_this->mpMorf2), -1, i_this->mInvisModel2);
        }
        return TRUE;
    }
    dComIfGp_get();
    u32 brk = i_this->mpBrkAnm;
    gabi::call(0x025E83FC, gabi::at<void>(brk), gabi::at<void>(gabi::load<u32>(model + 0xAC)), gabi::load<f32>(brk + 4));
    f32 frame = (f32)(s32)(i_this->mBehaviorType + i_this->mIsMiniBoss);
    brk = i_this->mpBrkAnm;
    if (i_this->m46D)
        frame = 3.0f;
    gabi::store<f32>(brk + 4, frame);
    McaMorf_entryDL(gabi::at<mDoExt_McaMorf_c>(i_this->mpMorf));
    if (i_this->mIsMiniBoss) {
        McaMorf_entryDL(gabi::at<mDoExt_McaMorf_c>(i_this->mpMorf2));
        if (i_this->mAlpha != 0xFF)
            invisibleModel_entry(i_this->mInvisModel2);
        else
            invisibleModel_entryOpa(i_this->mInvisModel2);
    }
    gabi::store<u32>(gabi::load<u32>(model + 0xAC) + 0x48, 0);
    if (i_this->mAlpha != 0xFF)
        invisibleModel_entry(i_this->mInvisModel);
    else
        invisibleModel_entryOpa(i_this->mInvisModel);
    if (i_this->mAlpha == 0)
        return TRUE;
    McaMorf_entryDL(gabi::at<mDoExt_McaMorf_c>(i_this->mpRodMorf));
    return TRUE;
}
VERIFY(0x024E2468, daWZ_Draw);

/* ---- attacks ---- */

/* 024E3624 */
void weapon_shoot(wz_class* i_this, u8 kind) {
    WWHD_FUNC(0x024E3624, void, i_this, kind);
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    gabi::Local<csXyz> angle;
    for (int k = 0; k < 2; k++) { /* the shape angle is copied twice */
        angle->x = i_this->shape_angle.x;
        angle->y = i_this->shape_angle.y;
        angle->z = i_this->shape_angle.z;
    }
    cXyz* eye = &i_this->eyePos;
    if (eye != nullptr) {
        s8 room = i_this->current.roomNo;
        u32 id = fopAcM_GetID(i_this);
        mDoAud_monsSeStart(0x48DA, eye, id, 0, dComIfGp_getReverb(room));
    }
    if (kind == 0) {
        /* three fire balls in a fan */
        f32 dz = player->current.pos.z - i_this->mRodTipPos.z;
        f32 dx = player->current.pos.x - i_this->mRodTipPos.x;
        f32 dy = (player->current.pos.y + 50.0f) - i_this->mRodTipPos.y;
        angle->x = (s16)-cM_atan2s(dy, std_sqrtf(gabi::fmadds(dx, dx, dz * dz)));
        s16 spread = -3000;
        for (int i = 3; i != 0; i--) {
            angle->y = (s16)(cM_atan2s(dx, dz) + spread);
            fopAcM_create(0xD0, 0xFFFFFF0A, &i_this->mRodTipPos, i_this->current.roomNo, angle.get(), nullptr, -1, 0);
            spread = (s16)(spread + 3000);
        }
        if (eye != nullptr)
            mDoAud_seStart(0x590A, eye, 0, dComIfGp_getReverb(i_this->current.roomNo));
    } else if (kind == 1) {
        /* summon: a child at the summon point */
        if (i_this->mSummonSw == 0xFF || i_this->m540 == 0)
            return;
        f32 dz = i_this->mSummonPos.z - i_this->mRodTipPos.z;
        f32 dx = i_this->mSummonPos.x - i_this->mRodTipPos.x;
        f32 dy = (i_this->mSummonPos.y + 300.0f) - i_this->mRodTipPos.y;
        gabi::Local<cXyz> scale;
        angle->x = (s16)-cM_atan2s(dy, std_sqrtf(gabi::fmadds(dx, dx, dz * dz)));
        angle->y = cM_atan2s(dx, dz);
        scale->x = 1.0f;
        scale->z = 1.0f;
        scale->y = 1.0f;
        if (eye != nullptr)
            mDoAud_seStart(0x590B, eye, 0, dComIfGp_getReverb(i_this->current.roomNo));
        s8 room = i_this->current.roomNo;
        if (fopAcM_createChild(0xD0, fopAcM_GetID(i_this), 0xFFFFFF0B, &i_this->mRodTipPos, room, angle.get(), scale.get(), 0, 0) !=
            fpcM_ERROR_PROCESS_ID_e)
            i_this->m470 = 1;
    }
}
VERIFY(0x024E3624, weapon_shoot);

/* ---- damage ---- */

/* CcAtInfo (GameCube 0x1C): the hit object at +0, pParticlePos at +0x14 */
struct wzCcAtInfo {
    be<u32> mpObj;
    u8 _04[0x10];
    be<u32> pParticlePos;
    u8 _18[4];
};
static inline BOOL dCcD_GObjInf_ChkTgHit(void* o) { return gabi::call<BOOL>(0x025162A4, o); }
static inline u32 dCcD_GObjInf_GetTgHitObj(void* o) { return gabi::call<u32>(0x02516300, o); }
static inline void dCcD_GObjInf_ClrTgHit(void* o) { gabi::call(0x0251621C, o); }
static inline void dCcD_GStts_Move(void* s) { gabi::call(0x02515E50, s); }
static inline void cc_at_check(fopAc_ac_c* a, wzCcAtInfo* info) { gabi::call(0x025192A8, a, info); }
/* fopAcM_seStart with the HD actor/eyePos null checks; `checkActor` false where the WWHD code
 * only tests the eyePos address */
static inline void wz_se(wz_class* a, u32 id, u32 param, bool checkActor = true) {
    if ((!checkActor || gabi::ea(a) != 0) && gabi::ea(a) + 0x37C != 0)
        mDoAud_seStart(id, &a->eyePos, param, dComIfGp_getReverb(a->current.roomNo));
}
/* the hit sound pair of the stun hits */
static inline void wz_stunSe(wz_class* a, bool checkActor) {
    if ((!checkActor || gabi::ea(a) != 0) && gabi::ea(a) + 0x37C != 0) {
        mDoAud_seStart(0x2833, &a->eyePos, 0x44, dComIfGp_getReverb(a->current.roomNo));
        s8 room = a->current.roomNo;
        u32 id = fopAcM_GetID(a);
        mDoAud_monsSeStart(0x48D8, &a->eyePos, id, 0, dComIfGp_getReverb(room));
    }
}
static inline void wz_followEnd(wz_class* a, int i) { dPa_followEcallBack_end(gabi::at<dPa_followEcallBack>(gabi::ea(a->mFollowCb) + 0x14 * i)); }

/* 024E2C34 */
BOOL body_atari_check(wz_class* i_this) {
    WWHD_FUNC(0x024E2C34, BOOL, i_this);
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    dCcD_GStts_Move(i_this->mStts + 0x1C);
    i_this->mHitKind = 0;
    if (!dCcD_GObjInf_ChkTgHit(i_this->mBodyCyl)) {
        i_this->mHitLock = 0;
        return FALSE;
    }
    u32 hit = dCcD_GObjInf_GetTgHitObj(i_this->mBodyCyl);
    if (hit == 0 || i_this->mHitLock != 0)
        return FALSE;
    gabi::Local<wzCcAtInfo> info;
    info->pParticlePos = 0;
    i_this->mHitLock = 1;
    u32 type = gabi::load<u32>(hit + 0x10);
    switch (type) {
    case 0x2: { /* sword */
        wz_se(i_this, 0x2803, 0x20);
        i_this->mHitKind = 0;
        u8 mode = gabi::load<u8>(gabi::ea(player) + 0x3AC);
        if ((mode >= 5 && mode <= 10) || mode == 12 || (mode >= 14 && mode <= 16) || mode == 21 || mode == 23 || (mode >= 25 && mode <= 27) ||
            (mode >= 30 && mode <= 31))
            i_this->mHitKind = 1;
        break;
    }
    case 0x20:
        i_this->mHitKind = 6;
        break;
    case 0x40: {
        i_this->mHitKind = 4;
        dPa_control_set(dComIfGp_getParticle(), 0, 0x27B, gabi::at<cXyz>(gabi::ea(i_this) + 0x390), nullptr, nullptr, 0xFF, nullptr, -1,
                        nullptr, nullptr, nullptr);
        wz_stunSe(i_this, true);
        i_this->m468 = 1;
        i_this->m469 = 10;
        return TRUE;
    }
    case 0x80:
        wz_se(i_this, 0x2833, 0x20);
        break;
    case 0x200:
    case 0x40000:
        gabi::store<s16>(gabi::ea(i_this->mEnemyFire) + 4, 100); /* enemyfire: burn timer */
        break;
    case 0x10000:
        wz_se(i_this, 0x2855, 0x20);
        i_this->mHitKind = 7;
        if (gabi::load<u8>(gabi::ea(player) + 0x3AC) == 0x11)
            i_this->mHitKind = 8;
        break;
    case 0x80000: /* ice arrow */
        wz_followEnd(i_this, 2);
        wz_followEnd(i_this, 3);
        wz_followEnd(i_this, 4);
        gabi::store<u32>(gabi::ea(i_this) + 0x39C, 0);
        i_this->mA44 = 200;
        i_this->mRodScaleY = 0.0f;
        i_this->mRodScaleZ = 0.0f;
        i_this->m468 = 1;
        i_this->mA4C = 2;
        i_this->m469 = 10;
        i_this->mRodScaleX = 0.0f;
        return TRUE;
    case 0x100000: /* light arrow */
        i_this->mBEC = 1.0f;
        i_this->mA48 = 80.0f;
        i_this->mA46 = 1;
        {
            u32 a = gabi::ea(i_this) + 0x7D8; /* body cylinder: OffTgSetBit */
            gabi::store<u32>(gabi::ea(i_this) + 0x39C, 0);
            gabi::store<u32>(a, gabi::load<u32>(a) & ~1u);
        }
        dCcD_GObjInf_ClrTgHit(i_this->mBodyCyl);
        rod_size_set(i_this, 1);
        wz_followEnd(i_this, 2);
        wz_followEnd(i_this, 3);
        wz_followEnd(i_this, 4);
        break;
    case 0x200000:
        i_this->mHitKind = 3;
        wz_stunSe(i_this, true);
        i_this->m468 = 1;
        i_this->m469 = 10;
        return TRUE;
    case 0x8000000:
        if (i_this->stealItemLeft > 0) {
            s8 health = i_this->health;
            i_this->health = 10;
            info->mpObj = dCcD_GObjInf_GetTgHitObj(i_this->mBodyCyl);
            cc_at_check(i_this, info.get());
            i_this->health = health;
            i_this->m46F = (u8)(i_this->m46F + 1);
        }
        dPa_control_set(dComIfGp_getParticle(), 0, 0x27B, gabi::at<cXyz>(gabi::ea(i_this) + 0x390), nullptr, nullptr, 0xFF, nullptr, -1,
                        nullptr, nullptr, nullptr);
        i_this->mHitKind = 9;
        wz_stunSe(i_this, false);
        i_this->m468 = 1;
        i_this->m469 = 10;
        return TRUE;
    default:
        i_this->mHitKind = 0;
        wz_se(i_this, 0x2834, 0x20);
        break;
    }
    gabi::Local<cXyz> pos;
    {
        f32 x = gabi::load<f32>(gabi::ea(i_this) + 0x88C), y = gabi::load<f32>(gabi::ea(i_this) + 0x890), z = gabi::load<f32>(gabi::ea(i_this) + 0x894);
        pos->x = x;
        pos->y = y;
        pos->z = z;
    }
    info->mpObj = dCcD_GObjInf_GetTgHitObj(i_this->mBodyCyl);
    cc_at_check(i_this, info.get());
    u8 kind = i_this->mHitKind;
    if (kind == 1 || kind == 7 || kind == 8 || i_this->health <= 0) {
        dPa_control_set(dComIfGp_getParticle(), 0, 0x10, pos.get(), nullptr, nullptr, 0xFF, nullptr, -1, nullptr, nullptr, nullptr);
        gabi::Local<cXyz> scale;
        scale->x = 2.0f;
        scale->y = 2.0f;
        scale->z = 2.0f;
        dPa_control_set(dComIfGp_getParticle(), 0, 0xF, pos.get(), &player->shape_angle, scale.get(), 0xFF, nullptr, -1, nullptr, nullptr,
                        nullptr);
    } else {
        dPa_control_set(dComIfGp_getParticle(), 0, 0xD, pos.get(), &player->shape_angle, nullptr, 0xFF, nullptr, -1, nullptr, nullptr, nullptr);
    }
    i_this->m468 = 1;
    i_this->m469 = 10;
    return TRUE;
}
VERIFY(0x024E2C34, body_atari_check);

/* ---- the summoner (types 12/13) ---- */

static inline void wz_morfPlay(wz_class* i_this) { gabi::call(0x025E535C, gabi::at<void>(i_this->mpMorf), (void*)nullptr, 0, 0); }
/* the summon smoke: the emitter's two scale vectors follow the actor scale */
static void wz_summonSmoke(wz_class* i_this, u16 id) {
    JPABaseEmitter* e = dPa_control_set(dComIfGp_getParticle(), 0, id, &i_this->current.pos, nullptr, nullptr, 0xFF, nullptr, -1, nullptr, nullptr,
                                        nullptr);
    f32 x = i_this->scale.x, y = i_this->scale.y, z = i_this->scale.z;
    if (e != nullptr) {
        u32 p = gabi::ea(e);
        gabi::store<f32>(p + 0x220, x);
        gabi::store<f32>(p + 0x224, y);
        gabi::store<f32>(p + 0x228, z);
        gabi::store<f32>(p + 0x238, x);
        gabi::store<f32>(p + 0x23C, y);
        gabi::store<f32>(p + 0x240, z);
    }
}
#define WZ_SUMMON_NUM 0x101D38B0   /* s32 per row: enemies to summon */
#define WZ_SUMMON_NAME 0x101D35E0  /* s16 per row: process name (0x7FFF ends a group) */
#define WZ_SUMMON_PRM 0x101D36D0   /* u32 per row: parameters */
#define WZ_SUMMON_COLOR 0x101D35B8 /* u32[3]: Moblin/Bokoblin colour variants */
/* one summon group: create up to `num` enemies in the free child slots */
template <class Prm>
static void wz_summonGroup(wz_class* i_this, s32 num, s32 row, cXyz* pos, csXyz* angle, Prm prm) {
    s32 n = 0;
    if (n >= num)
        return;
    for (int i = 0;;) {
        if ((s32)i_this->mChildId[i] == -1) {
            s16 name;
            u32 param = prm(&name);
            i_this->mChildId[i] = fopAcM_create(name, param, pos, i_this->current.roomNo, angle, nullptr, -1, 0);
            if ((s32)i_this->mChildId[i] != -1) {
                i_this->mChildFlag[i] = 1;
                f32 x = i_this->current.pos.x;
                pos->x = x;
                pos->y = i_this->current.pos.y;
                pos->z = i_this->current.pos.z;
                n++;
                pos->x = x + cM_rndFX(100.0f);
                pos->y = pos->y + cM_rndFX(100.0f);
                pos->z = pos->z + cM_rndFX(100.0f);
            }
        }
        i++;
        if (i >= 20 || n >= num)
            break;
    }
}

/* 024E9384 */
void action_summon_dousa(wz_class* i_this) {
    WWHD_FUNC(0x024E9384, void, i_this);
    dComIfGp_get();
    u8 cnt = 0;
    u8 st = i_this->m469;
    if (st < 200) {
    } else if (st == 200) {
        /* open the door */
        for (int i = 0; i < 4; i++)
            i_this->m4FA[i] = 0;
        for (int i = 5; i != 0; i--)
            wz_summonSmoke(i_this, 0x8283);
        if (gabi::ea(i_this) + 0x37C != 0)
            mDoAud_seStart(0x69E0, &i_this->eyePos, 0, dComIfGp_getReverb(i_this->current.roomNo));
        wz_summonSmoke(i_this, 0x8284);
        i_this->m4F0 = 0x28;
        for (int i = 0; i < 20; i++) {
            i_this->mChildId[i] = 0xFFFFFFFF;
            i_this->mChildFlag[i] = 0;
        }
        i_this->m469 = (u8)(i_this->m469 + 1);
        wz_morfPlay(i_this);
        mDoExt_baseAnm_play(gabi::at<void>(i_this->mpBtkAnm));
        return;
    } else if (st < 202) {
        /* summon the enemies of the mini-boss's current row */
        if (i_this->mBehaviorType == 12) {
            wz_class* parent = (wz_class*)fopAcM_SearchByID(i_this->mParentId);
            if (parent != nullptr && i_this->m4F0 == 1) {
                s32 row = parent->mSummonWave * 8 + parent->mSummonSlot * 2;
                s32 num = gabi::load<s32>(WZ_SUMMON_NUM + row * 4);
                gabi::Local<cXyz> pos;
                gabi::Local<csXyz> angle;
                angle->z = 0;
                angle->x = 0;
                angle->y = 0;
                angle->y = fopAcM_searchActorAngleY(i_this, dComIfGp_getPlayer(0));
                pos->copy(i_this->current.pos);
                if (row == 0xBF)
                    angle->x = 0x80;
                wz_summonGroup(i_this, num, row, pos, angle, [&](s16* name) {
                    *name = gabi::load<s16>(WZ_SUMMON_NAME + row * 2);
                    u32 param = gabi::load<u32>(WZ_SUMMON_PRM + row * 4);
                    if (*name == 0xCE && (param & 0xA00) == 0) {
                        s32 r = gabi::ftoi(cM_rndF(2.99f));
                        param |= (gabi::load<u32>(WZ_SUMMON_COLOR + r * 4) << 8) + 0xA00;
                        *name = gabi::load<s16>(WZ_SUMMON_NAME + row * 2);
                    }
                    return param;
                });
                s32 slot = parent->mSummonSlot;
                s32 wave = parent->mSummonWave;
                parent->mSummonSlot = (slot + 1) & 3;
                if (wave == 7 || wave == 13) {
                    s16 c = (s16)(parent->mSummonCount + 1);
                    parent->mSummonCount = c;
                    if (c > 7)
                        parent->mSummonWave = 10;
                }
                row++;
                if (gabi::load<s16>(WZ_SUMMON_NAME + row * 2) != 0x7FFF) {
                    /* the row's second group */
                    angle->x = 0;
                    angle->y = 0;
                    num = gabi::load<s32>(WZ_SUMMON_NUM + row * 4);
                    angle->z = 0;
                    angle->y = fopAcM_searchActorAngleY(i_this, dComIfGp_getPlayer(0));
                    pos->copy(i_this->current.pos);
                    wz_summonGroup(i_this, num, row, pos, angle, [&](s16* name) {
                        u32 param = gabi::load<u32>(WZ_SUMMON_PRM + row * 4);
                        if (row == 0xCE) {
                            param |= 0xA00;
                            s32 r = gabi::ftoi(cM_rndF(3.19f));
                            param |= gabi::load<u32>(WZ_SUMMON_COLOR + r * 4) << 8;
                        }
                        *name = gabi::load<s16>(WZ_SUMMON_NAME + row * 2);
                        return param;
                    });
                }
            }
        }
        mDoExt_baseAnm* btk = gabi::at<mDoExt_baseAnm>(i_this->mpBtkAnm);
        if (btk->mFrameCtrl.checkState(J3DFrameCtrl::STATE_STOP_E) || btk->mFrameCtrl.getRate() == 0.0f) {
            i_this->m469 = (u8)(i_this->m469 + 1);
            if (i_this->mBehaviorType == 13)
                fopAcM_delete(i_this);
        }
    } else if (st == 202) {
        /* wait until every summoned enemy is gone or out of the arena */
        for (int i = 0; i < 20; i++) {
            if (i_this->mChildFlag[i] == 0) {
                cnt = (u8)(cnt + 1);
                continue;
            }
            fopAc_ac_c* child = fopAcM_SearchByID(i_this->mChildId[i]);
            bool inStage = false;
            if (child != nullptr)
                inStage = wz_isStartStage(STR(0x10042D30));
            bool keep;
            if (inStage) {
                f32 x = child->current.pos.x, z = child->current.pos.z;
                f32 d = std_sqrtf(gabi::fmadds(x, x, z * z));
                keep = false;
                if (!(d > 1800.0f)) {
                    f32 y = child->current.pos.y;
                    if (!(y < -200.0f) && !(y > 2100.0f))
                        keep = true;
                }
            } else {
                keep = child != nullptr;
            }
            if (!keep) {
                i_this->mChildFlag[i] = 0;
                i_this->mChildId[i] = 0xFFFFFFFF;
            }
        }
        if (cnt == 20) {
            fopAc_ac_c* parent = fopAcM_SearchByID(i_this->mParentId);
            if (parent != nullptr)
                gabi::store<u8>(gabi::ea(parent) + 0x470, 0);
            fopAcM_delete(i_this);
        }
    }
    wz_morfPlay(i_this);
    mDoExt_baseAnm_play(gabi::at<void>(i_this->mpBtkAnm));
}
VERIFY(0x024E9384, action_summon_dousa);

/* ---- create ---- */

static inline u32 wz_ld32(wz_class* a, u32 off) { return gabi::load<u32>(gabi::ea(a) + off); }
static inline void wz_st32(wz_class* a, u32 off, u32 v) { gabi::store<u32>(gabi::ea(a) + off, v); }
static inline void wz_st8(wz_class* a, u32 off, u8 v) { gabi::store<u8>(gabi::ea(a) + off, v); }
static inline void wz_st16(wz_class* a, u32 off, s16 v) { gabi::store<s16>(gabi::ea(a) + off, v); }
static inline void wz_stf(wz_class* a, u32 off, f32 v) { gabi::store<f32>(gabi::ea(a) + off, v); }
static u32 ppcSlw(u32 x, u32 n) { n &= 63; return n < 32 ? x << n : 0; }
static u32 ppcSrw(u32 x, u32 n) { n &= 63; return n < 32 ? x >> n : 0; }

/* a summoned Wizzrobe takes the summon data of the parent */
static void wz_copySummonData(wz_class* i_this, wz_class* p) {
    cpy_u32(gabi::ea(i_this) + 0x454, gabi::ea(p) + 0x454);
    cpy_u32(gabi::ea(i_this) + 0x458, gabi::ea(p) + 0x458);
    cpy_u32(gabi::ea(i_this) + 0x45C, gabi::ea(p) + 0x45C);
    cpy_u32(gabi::ea(i_this) + 0x474, gabi::ea(p) + 0x474);
    i_this->m470 = (u8)p->m470;
    i_this->mSummonSlot = (s32)p->mSummonSlot;
    i_this->mSummonWave = (s32)p->mSummonWave;
}
/* the summoner's frame: the HD texture swap keeps a ResTIMG at entry SP - 0xA4 whose relative
 * offsets record its own address, so the frame is laid out as the original's (0xB0 bytes) */
struct wzCreateFrame {
    u8 _00[0xC];
    u8 timg[0x20];       /* 0x0C: ResTIMG (0x24 with the next word) */
    be<u32> texObj;      /* 0x2C */
    u8 _30[0xB0 - 0x30];
};
/* HD: the summon door's "__dummy" texture is replaced by a system texture (027F81A4(.., 6)) */
static void wz_swapDummyTexture(wzCreateFrame* f, u32 tex, u32 names) {
    for (u16 i = 0; i < gabi::load<u16>(tex);) {
        u32 name = gabi::call<u32>(0x027ED1F0, gabi::at<void>(names), i);
        if (name == 0) {
            JUT_ASSERT_fail(STR(0x10042E8C) /* "d_a_wz.cpp" */, 0x1086, STR(0x10042E80) /* "name != (0)" */);
            i = (u16)(i + 1);
            continue;
        }
        u32 p = name, q = 0x10042E58; /* "__dummy" */
        u8 a, b;
        do {
            a = gabi::load<u8>(p++);
            b = gabi::load<u8>(q++);
        } while (a == b && a != 0);
        if (a == b) {
            u32 image = gabi::call<u32>(0x027F81A4, gabi::at<void>(gabi::load<u32>(0x101F9968)), 6);
            u32 obj = gabi::call<u32>(0x02773680, gabi::at<void>(gabi::call<u32>(0x0273AD10, 0xC0)), gabi::at<void>(image));
            u32 local = gabi::ea(f) + 0xC;
            f->texObj = obj;
            gabi::store<u16>(local + 2, (u16)gabi::load<u32>(obj + 8));
            gabi::store<u8>(local + 8, 0);
            gabi::store<u16>(local + 4, (u16)gabi::load<u32>(obj + 0xC));
            u32 off = i * 0x24;
            u32 dest = gabi::load<u32>(tex + 4) + off;
            for (int j = 0; j < 3; j++) {
                cpy_u32(dest + j * 12, local + j * 12);
                cpy_u32(dest + j * 12 + 4, local + j * 12 + 4);
                cpy_u32(dest + j * 12 + 8, local + j * 12 + 8);
            }
            dest = gabi::load<u32>(tex + 4) + off;
            gabi::store<u32>(dest + 0x1C, gabi::load<u32>(dest + 0x1C) + local - dest);
            dest = gabi::load<u32>(tex + 4) + off;
            gabi::store<u32>(dest + 0xC, gabi::load<u32>(dest + 0xC) + local - dest);
            dest = gabi::load<u32>(tex + 4) + off;
            gabi::store<u32>(dest + 0x20, f->texObj);
            u32 hi = gabi::load<u32>(tex + 0x18), lo = gabi::load<u32>(tex + 0x1C);
            if (i < 0x40) {
                u32 upper = ppcSlw(lo, i + 32) | (ppcSlw(hi, i) | ppcSrw(lo, 32 - i));
                u32 lower = ppcSlw(lo, i);
                u32 a8 = gabi::load<u32>(tex + 8), aC = gabi::load<u32>(tex + 0xC);
                i = (u16)(i + 1);
                gabi::store<u32>(tex + 8, a8 | upper);
                gabi::store<u32>(tex + 0xC, aC | lower);
                continue;
            }
            u32 n = i - 0x40;
            u32 a10 = gabi::load<u32>(tex + 0x10);
            u32 upper = ppcSlw(lo, n + 32) | (ppcSlw(hi, n) | ppcSrw(lo, 32 - n));
            u32 a14 = gabi::load<u32>(tex + 0x14);
            gabi::store<u32>(tex + 0x10, a10 | upper);
            gabi::store<u32>(tex + 0x14, a14 | ppcSlw(lo, n));
        }
        i = (u16)(i + 1);
    }
}

/* 024E6BEC */
static cPhs_State daWZ_Create(fopAc_ac_c* a_this) {
    WWHD_FUNC(0x024E6BEC, cPhs_State, a_this);
    gabi::Local<wzCreateFrame> frame;
    wz_class* i_this = (wz_class*)a_this;
    u32 cond = a_this->actor_condition;
    if (!(cond & 8)) {
        if (a_this != nullptr) {
            wz_class_ct(a_this);
            cond = a_this->actor_condition;
        }
        a_this->actor_condition = cond | 8;
    }
    u32 prm = a_this->mParameters;
    s16 az = a_this->current.angle.z;
    i_this->mBehaviorType = (u8)prm;
    i_this->mDisableSpawnOnDeathSwitch = (u8)(prm >> 8);
    i_this->mEnableSpawnSwitch = (u8)(prm >> 16);
    wz_st8(i_this, 0x53D, (u8)(prm >> 24)); /* path */
    i_this->mEnemySummonTableIndex = (u8)az;
    cPhs_State phase;
    if ((u8)prm == 12 || (u8)prm == 13)
        phase = dComIfG_resLoad((request_of_phase_process_class*)i_this->mPhase, STR(0x10042E60) /* "Wzb" */);
    else
        phase = dComIfG_resLoad((request_of_phase_process_class*)i_this->mPhase, STR(0x10042E74) /* "Wz" */);
    u8 type = i_this->mBehaviorType;
    if (type == 0xFF) {
        i_this->mBehaviorType = 0;
    } else {
        if (type == 2) {
            i_this->mBehaviorType = 1;
            type = 1;
            i_this->mIsMiniBoss = 1;
        }
        if (type == 3) {
            i_this->mBehaviorType = 0;
            i_this->m46D = 1;
        }
    }
    if (i_this->mEnemySummonTableIndex == 0xFF)
        i_this->mEnemySummonTableIndex = 0;

    if (phase != cPhs_COMPLEATE_e) {
        if (i_this->mBehaviorType == 12 && gabi::load<u8>(gabi::ea(i_this) + 0x10AC) == 0) {
            u32 id = a_this->parentActorID;
            i_this->mParentId = id;
            if (id == 0xFFFFFFFF)
                return cPhs_ERROR_e;
            fopAc_ac_c* p = fopAcM_SearchByID(id);
            if (p == nullptr)
                return phase;
            u32 pid = gabi::load<u32>(gabi::ea(p) + 0x460);
            wz_st8(i_this, 0x10AC, 1);
            i_this->mParentId = pid;
        }
        if (phase == cPhs_ERROR_e && i_this->mBehaviorType == 12) {
            u32 id = a_this->parentActorID;
            i_this->mParentId = id;
            if (id == 0xFFFFFFFF)
                return cPhs_ERROR_e;
            fopAc_ac_c* p = fopAcM_SearchByID(id);
            if (p != nullptr)
                gabi::store<u8>(gabi::ea(p) + 0x470, 0);
        }
        return phase;
    }

    type = i_this->mBehaviorType;
    bool checkSw = type < 10;
    if (type > 10) {
        type = i_this->mBehaviorType;
        wz_st32(i_this, 0x900, 0x400); /* ball sphere */
        wz_class* p;
        if (type == 12) {
            if (gabi::load<u8>(gabi::ea(i_this) + 0x10AC) != 0)
                goto heap2;
            u32 id = a_this->parentActorID;
            i_this->mParentId = id;
            if (id == 0xFFFFFFFF)
                return cPhs_ERROR_e;
            p = (wz_class*)fopAcM_SearchByID(id);
            if (p == nullptr)
                return cPhs_ERROR_e;
            i_this->mParentId = p->mParentId;
            if (fopAcM_SearchByID(i_this->mParentId) == nullptr)
                return cPhs_ERROR_e;
            wz_st8(i_this, 0x10AC, 1);
        } else {
            u32 id = a_this->parentActorID;
            i_this->mParentId = id;
            if (id == 0xFFFFFFFF)
                return cPhs_ERROR_e;
            p = (wz_class*)fopAcM_SearchByID(id);
            if (p == nullptr)
                return cPhs_ERROR_e;
        }
        wz_copySummonData(i_this, p);
        type = i_this->mBehaviorType;
        checkSw = type < 10;
    }
    if (checkSw) {
        if (i_this->mDisableSpawnOnDeathSwitch == 0xFF)
            goto heap1;
        if (dComIfGs_isSwitch(i_this->mDisableSpawnOnDeathSwitch, gabi::load<s8>(0x1047E6C8)))
            return cPhs_ERROR_e;
        type = i_this->mBehaviorType;
    }
    if (type == 12 || type == 13)
        goto heap2;
heap1:
    if (!fopAcM_entrySolidHeap(a_this, 0x024E6420 /* useHeapInit */, type < 10 ? 0x3440 : 0x3640))
        return cPhs_ERROR_e;
    goto created;
heap2: {
    if (!fopAcM_entrySolidHeap(a_this, 0x024E67FC /* useHeapInit2 */, 0xCC0)) {
        fopAc_ac_c* p = fopAcM_SearchByID(i_this->mParentId);
        if (p != nullptr)
            gabi::store<u8>(gabi::ea(p) + 0x470, 0);
        return cPhs_ERROR_e;
    }
    u32 morf = i_this->mpMorf;
    if (i_this->mBehaviorType == 13)
        a_this->actor_status |= 0x4000;
    u32 data = gabi::load<u32>(morf_model(morf) + 0xAC);
    u32 tex = gabi::load<u32>(data + 0x30);
    u32 names = 0;
    if (tex != 0)
        names = gabi::load<u32>(data + 0x34);
    if (tex == 0 || names == 0) {
        fopAc_ac_c* p = fopAcM_SearchByID(i_this->mParentId);
        if (p != nullptr)
            gabi::store<u8>(gabi::ea(p) + 0x470, 0);
        return cPhs_ERROR_e;
    }
    wz_swapDummyTexture(frame, tex, names);
}
created: {
    a_this->current.angle.z = 0;
    u32 morf = i_this->mpMorf;
    a_this->shape_angle.z = 0;
    wz_st32(i_this, 0x39C, 0);
    u32 model = morf_model(morf);
    a_this->cullMtx = model != 0 ? model + 0xC8 : 0;
    fopAcM_setCullSizeBox(a_this, -100.0f, -50.0f, -50.0f, 100.0f, 200.0f, 100.0f);
    if (i_this->mBehaviorType < 10) {
        u8 t = i_this->mBehaviorType;
        a_this->stealItemLeft = 3;
        a_this->max_health = 4;
        a_this->health = 4;
        wz_st8(i_this, 0x38A, 4); /* attention_info distance */
        if (t < 1) {
            a_this->itemTableIdx = dComIfGp_CharTbl_GetNameIndex_wz(STR(0x10042E64) /* "wiz_r" */);
        } else if (t == 1) {
            a_this->max_health = 8;
            a_this->health = 8;
            s32 idx = dComIfGp_CharTbl_GetNameIndex_wz(STR(0x10042E6C) /* "wiz_s" */);
            u8 mini = i_this->mIsMiniBoss;
            u8 tbl = i_this->mEnemySummonTableIndex;
            a_this->itemTableIdx = idx;
            i_this->m470 = 0;
            i_this->mSummonWave = tbl;
            if (mini == 1) {
                a_this->max_health = 12;
                i_this->mSummonWave = 7;
                a_this->actor_status |= 0x4000000;
                a_this->health = 12;
            } else {
                i_this->mSummonSlot = (s16)gabi::ftoi(cM_rndF(3.99f)) & 3;
            }
        }
        if (gabi::load<s16>(0x1047BB0A) != 0) { /* HD debug: invincible */
            a_this->max_health = 0x7F;
            a_this->health = 0x7F;
        }
        u8 pathNo = gabi::load<u8>(gabi::ea(i_this) + 0x53D);
        if (pathNo != 0xFF) {
            u32 path = gabi::ea(dPath_GetRoomPath(pathNo, a_this->current.roomNo));
            wz_st32(i_this, 0x52C, path);
            if (path != 0) {
                f32 r = cM_rndF((f32)gabi::load<u16>(path));
                path = wz_ld32(i_this, 0x52C);
                u32 idx = (u32)gabi::ftoi(r);
                if (idx == gabi::load<u16>(path))
                    idx--;
                u32 pnt = gabi::load<u32>(path + 8) + idx * 16;
                s8 room = a_this->current.roomNo;
                a_this->current.pos.x = gabi::load<f32>(pnt + 4);
                a_this->current.pos.y = gabi::load<f32>(pnt + 8);
                a_this->current.pos.z = gabi::load<f32>(pnt + 0xC);
                wz_st32(i_this, 0x540, gabi::call<u32>(0x025AB070, gabi::at<void>(wz_ld32(i_this, 0x52C)), room)); /* dPath_GetNextRoomPath */
            }
        }
        cpy_u32(gabi::ea(i_this) + 0x448, gabi::ea(&a_this->current.pos.x));
        cpy_u32(gabi::ea(i_this) + 0x44C, gabi::ea(&a_this->current.pos.y));
        cpy_u32(gabi::ea(i_this) + 0x450, gabi::ea(&a_this->current.pos.z));
        draw_SUB(i_this);
        gabi::call(0x02516518, i_this->mBodyCyl, gabi::at<void>(0x101D3B04)); /* dCcD_Cyl::Set */
        wz_st32(i_this, 0x804, gabi::ea(i_this->mStts));
        wz_st32(i_this, 0x7D8, wz_ld32(i_this, 0x7D8) & ~1u);
        dCcD_GObjInf_ClrTgHit(i_this->mBodyCyl);
        s16 ang = fopAcM_searchActorAngleY(a_this, dComIfGp_getPlayer(0));
        a_this->current.angle.y = ang;
        wz_st16(i_this, 0x502, ang);
        a_this->shape_angle.y = ang;
        wz_stf(i_this, 0xBE0, 80.0f);
        wz_stf(i_this, 0xBDC, 50.0f);
        i_this->mAcchWallH = 100.0f;
        wz_st32(i_this, 0xE04, i_this->mpMorf); /* enemyfire: morf */
        wz_st32(i_this, 0xA40, gabi::ea(i_this));
        i_this->mAcchWallR = 110.0f;
        wz_st32(i_this, 0xDF8, gabi::ea(i_this)); /* enemyfire: actor */
        for (int i = 0; i < 10; i++) {
            wz_st8(i_this, 0xE08 + i, gabi::load<u8>(0x101D3AF8 + i));
            wz_stf(i_this, 0xE14 + 4 * i, gabi::load<f32>(0x101D3A90 + 4 * i));
        }
        i_this->mAlpha = 0;
        i_this->mRodScaleX = 0.0f;
        i_this->mRodScaleY = 0.0f;
        i_this->mRodScaleZ = 0.0f;
        if (wz_isStartStage(STR(0x10042E78) /* "kazeMB" */)) {
            i_this->m468 = 3;
            if (!i_this->mIsMiniBoss) {
                a_this->actor_status |= 0x4000;
                i_this->m469 = 0x3C;
            } else {
                a_this->current.pos.x = 0.0f;
                a_this->current.pos.y = 0.0f;
                a_this->current.pos.z = 0.0f;
                gabi::store<u8>(0x101D3594, 0);
                i_this->m469 = 0x32;
            }
        } else {
            i_this->m468 = 0;
            i_this->m469 = 0;
        }
    } else if (i_this->mBehaviorType == 12 || i_this->mBehaviorType == 13) {
        i_this->m468 = 200;
        i_this->m469 = 200;
    } else {
        i_this->mAcchWallH = 0.0f;
        i_this->mAcchWallR = 40.0f;
        gabi::call(0x0251677C, i_this->mBallSph, gabi::at<void>(0x101D3AB8)); /* dCcD_Sph::Set */
        wz_st32(i_this, 0x934, gabi::ea(i_this->mStts));
        i_this->m468 = 100;
        wz_st16(i_this, 0x4F4, 300);
        i_this->m469 = 100;
    }
    type = i_this->mBehaviorType;
    if (type == 12 || type == 13)
        return phase;
    gabi::call(0x024F06B4, i_this->mAcch, &a_this->current.pos, &a_this->old.pos, a_this, 1, i_this->mAcchCir, &a_this->speed, (void*)nullptr,
               (void*)nullptr); /* dBgS_Acch::Set */
    gabi::call(0x02515F14, i_this->mStts, 0xFE, 1, a_this); /* dCcD_Stts::Init */
    BG_check(i_this);
    type = i_this->mBehaviorType;
    if (type == 10 || type == 11)
        dKy_plight_set((LIGHT_INFLUENCE*)i_this->mLight);
    return phase;
}
}
VERIFY(0x024E6BEC, daWZ_Create);

/* ---- mini-boss intro / defeat demo ---- */

/* HD debug parameter block (all zero in the shipped game: offsets added to the demo constants) */
#define WZ_DBG 0x1047B608
static inline f32 wzDbgF(u32 off) { return gabi::load<f32>(WZ_DBG + off); }
static inline s16 wzDbgS(u32 off) { return gabi::load<s16>(WZ_DBG + off); }
static inline void dCamera_Stop_wz(u32 cam) { gabi::call(0x02514F2C, gabi::at<void>(cam)); }
static inline void dCamera_Start_wz(u32 cam) { gabi::call(0x02514F38, gabi::at<void>(cam)); }
static inline void dCamera_SetTrimSize_wz(u32 cam, s32 s) { gabi::call(0x02515280, gabi::at<void>(cam), s); }
static inline void dCamera_Reset_wz(u32 cam, cXyz* center, cXyz* eye) { gabi::call(0x0251510C, gabi::at<void>(cam), center, eye); }
static inline void dCamera_Set_wz(u32 cam, cXyz* center, cXyz* eye, f32 fovy, s16 bank) { gabi::call(0x02514F88, gabi::at<void>(cam), center, eye, fovy, bank); }
static inline void dComIfGp_StopQuake_wz(s32 a) { gabi::call(0x025CB610, gabi::at<void>(dComIfGp_ea() + 0x599C), a); }
static inline void mDoAud_bgmAllMute_wz(s32 t) { gabi::call(0x025E1960, t); }
static inline void mDoAud_subBgmStart_wz(u32 id) { gabi::call(0x025E1918, id); }
static inline BOOL fopAcM_orderPotentialEvent_wz(fopAc_ac_c* a, u16 type, u16 flag, u16 p) { return gabi::call<BOOL>(0x025D7B24, a, type, flag, p); }
/* the player's virtual setPlayerPosAndAngle (vtable +0x114); the vtable is read before the angle is computed */
static inline void wz_playerSetPos(fopAc_ac_c* player, u32 vt, cXyz* pos, s16 ang) {
    gabi::call_ptr(gabi::load<u32>(vt + 0x114), player, pos, ang);
}
/* cLib_addCalc2 toward a target with a step proportional to the distance */
static inline void wz_camCalc(be<f32>* v, f32 target, f32 rateAdd, f32 rate) {
    f32 d = *v - target;
    f32 r = rateAdd + rate;
    cLib_addCalc2(v, target, 1.0f, std::fabs(d) * r);
}
static inline void wz_demoSe(wz_class* i_this, u32 id) {
    if (gabi::ea(i_this) + 0x37C != 0)
        mDoAud_seStart(id, &i_this->eyePos, 0, dComIfGp_getReverb(i_this->current.roomNo));
}
static inline void wz_demoMonsSe(wz_class* i_this) {
    if (gabi::ea(i_this) + 0x37C != 0) {
        s8 room = i_this->current.roomNo;
        u32 id = fopAcM_GetID(i_this);
        mDoAud_monsSeStart(0x48D6, &i_this->eyePos, id, 0, dComIfGp_getReverb(room));
    }
}
/* 024E7904 */
void action_demo(wz_class* i_this) {
    WWHD_FUNC(0x024E7904, void, i_this);
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    s32 camId = gabi::load<s8>(dComIfGp_ea() + 0x5B30);
    u32 cam = gabi::load<u32>(dComIfGp_ea() + camId * 0x34 + 0x5AF8) + 0x248; /* dCamera_c */
    u32 base = gabi::ea(i_this);
    auto F = [&](u32 off) { return gabi::at<be<f32>>(base + off); };
    auto S16 = [&](u32 off) { return gabi::at<be<s16>>(base + off); };
    gabi::Local<cXyz> dpos; /* where the defeat effect appears */
    {
        f32 x = i_this->current.pos.x;
        f32 add = wzDbgF(0x70C) + 160.0f;
        dpos->x = x;
        f32 y = i_this->current.pos.y;
        dpos->y = y;
        dpos->y = y + add;
        dpos->z = i_this->current.pos.z;
    }
    u8 st = i_this->m469;
    switch (st) {
    case 0x32: {
        /* wait for the player */
        f32 d = fopAcM_searchActorDistance(i_this, dComIfGp_getPlayer(0));
        if (d > wzDbgF(0x63C) + 1250.0f)
            break;
        cpy_u32(gabi::ea(&i_this->eyePos.x), gabi::ea(&player->current.pos.x));
        cpy_u32(gabi::ea(&i_this->eyePos.y), gabi::ea(&player->current.pos.y));
        cpy_u32(gabi::ea(&i_this->eyePos.z), gabi::ea(&player->current.pos.z));
        wz_demoSe(i_this, 0x5909);
        s16 ang = fopAcM_searchActorAngleY(i_this, dComIfGp_getPlayer(0));
        i_this->current.angle.y = ang;
        i_this->shape_angle.y = ang;
        *S16(0x502) = ang;
        mDoAud_bgmAllMute_wz(0x1E);
        i_this->current.pos.x = 0.0f;
        i_this->m469 = 0x46;
        *F(0x44C) = 0.0f;
        *F(0x450) = 0.0f;
        i_this->current.pos.y = 0.0f;
        *F(0x448) = 0.0f;
        i_this->current.pos.z = 0.0f;
        break;
    }
    case 0x3C: {
        u32 sub = (u32)(s32)(s16)i_this->m4FA[0];
        if (sub < 1) {
            if (gabi::load<u8>(0x101D3594) == 0)
                break;
            anm_init(i_this, 0x10, 5.0f, 2, 1.0f, -1);
            i_this->current.pos.x = -250.0f;
            i_this->current.pos.y = 20.0f;
            i_this->current.pos.z = 100.0f;
            s16 ang = fopAcM_searchActorAngleY(i_this, dComIfGp_getPlayer(0));
            i_this->current.angle.y = ang;
            i_this->shape_angle.y = ang;
            *S16(0x502) = ang;
            gabi::store<u8>(0x101D3594, 0);
            i_this->m4FA[0] = (s16)(i_this->m4FA[0] + 1);
        } else if (sub == 1) {
            s16 a = (s16)(i_this->mAlpha + 8);
            if (a <= 0xFF) {
                i_this->mAlpha = a;
                break;
            }
            s16 n = i_this->m4FA[0];
            i_this->mAlpha = 0xFF;
            i_this->m4FA[0] = (s16)(n + 1);
        } else if (sub == 2) {
            if (gabi::load<u8>(0x101D3594) == 0)
                break;
            gabi::store<u8>(0x101D3594, 0);
            i_this->actor_status &= ~0x4000u;
            i_this->m468 = 0;
            i_this->m469 = 6;
        }
        break;
    }
    case 0x46: {
        i_this->actor_status |= 0x4000;
        if (gabi::load<u16>(base + 0xF8) != 2) {
            /* request the demo event */
            u32 play = dComIfGp_ea();
            gabi::store<u16>(play + 0x52B8, (u16)(gabi::load<u16>(play + 0x52B8) | 1));
            fopAcM_orderPotentialEvent_wz(i_this, 0, 0xFFFF, 0);
            gabi::store<u16>(base + 0xFA, (u16)(gabi::load<u16>(base + 0xFA) | 2));
            break;
        }
        gabi::store<u32>(gabi::ea(player) + 0x428, 0);
        gabi::store<s16>(gabi::ea(player) + 0x420, 3);
        dCamera_Stop_wz(cam);
        dCamera_SetTrimSize_wz(cam, 2);
        s8 h = i_this->health;
        *F(0x57C) = 50.0f;
        if (h > 0) {
            /* the intro */
            gabi::store<u32>(gabi::ea(player) + 0x430, 0x18);
            s16 t = (s16)gabi::ftoi(wzDbgF(0x640) + 45.0f);
            i_this->current.pos.x = 0.0f;
            i_this->m4F0 = t;
            i_this->current.pos.y = 0.0f;
            i_this->m469 = 0x50;
            i_this->current.pos.z = 0.0f;
            break;
        }
        /* defeated: freeze the summoned enemies */
        i_this->current.angle.y = (s16)(fopAcM_searchActorAngleY(i_this, dComIfGp_getPlayer(0)) + 0x8000);
        i_this->shape_angle.y = fopAcM_searchActorAngleY(i_this, dComIfGp_getPlayer(0));
        for (int i = 0; i < 20; i++) {
            if (i_this->mChildFlag[i] == 0)
                continue;
            fopAc_ac_c* c = fopAcM_SearchByID(i_this->mChildId[i]);
            if (c == nullptr)
                continue;
            u32 s = c->actor_status;
            if (s & 0x4000)
                continue;
            c->actor_status = s & ~0x4000u;
            wz_st8(i_this, 0x4DC + i, 1);
        }
        u32 vt = gabi::load<u32>(gabi::ea(player) + 0xB4);
        s16 ang = (s16)(fopAcM_searchActorAngleY(i_this, dComIfGp_getPlayer(0)) + 0x8000);
        wz_playerSetPos(player, vt, &player->current.pos, ang);
        if (i_this->mCurrBckIdx != 0xD) {
            anm_init(i_this, 5, 5.0f, 0, 1.0f, -1);
            i_this->m469 = 0x5A;
            i_this->speedF = 40.0f;
        } else {
            i_this->m469 = 0x5B;
        }
        break;
    }
    case 0x50: {
        if (i_this->m4F0 != 0)
            break;
        cLib_addCalc2(F(0x57C), wzDbgF(0x644) + 50.0f, 1.0f, wzDbgF(0x700) + 0.5f);
        u32 vt = gabi::load<u32>(gabi::ea(player) + 0xB4);
        s16 ang = (s16)(fopAcM_searchActorAngleY(i_this, dComIfGp_getPlayer(0)) + 0x8000);
        wz_playerSetPos(player, vt, &player->current.pos, ang);
        *F(0x554) = wzDbgF(0x648);
        *F(0x558) = wzDbgF(0x64C) + 125.0f;
        *F(0x55C) = wzDbgF(0x650) + -29.0f;
        *F(0x560) = wzDbgF(0x654) + 1470.0f;
        *F(0x564) = wzDbgF(0x658) + 917.0f;
        *F(0x568) = wzDbgF(0x65C) + -222.0f;
        i_this->m4F0 = (s16)gabi::ftoi(wzDbgF(0x660) + 20.0f);
        anm_init(i_this, 0x12, 0.0f, 2, 1.0f, -1);
        i_this->m469 = (u8)(i_this->m469 + 1);
    }
        [[fallthrough]];
    case 0x51: {
        if (wzDbgS(0x90) != 0) {
            /* debug: hold the camera */
            *F(0x554) = wzDbgF(0x648);
            *F(0x558) = wzDbgF(0x64C) + 125.0f;
            *F(0x55C) = wzDbgF(0x650) + -29.0f;
            *F(0x560) = wzDbgF(0x654) + 1470.0f;
            *F(0x564) = wzDbgF(0x658) + 917.0f;
            *F(0x568) = wzDbgF(0x65C) + -222.0f;
            i_this->m4F0 = (s16)gabi::ftoi(wzDbgF(0x660) + 20.0f);
            break;
        }
        if (i_this->m4F0 == 0) {
            u32 sub = (u32)(s32)(s16)i_this->m4FA[0];
            if (sub < 2) {
                s16 yrot;
                u32 dbg;
                if (sub < 1) {
                    gabi::store<u32>(gabi::ea(player) + 0x430, 0x14);
                    i_this->m4F0 = (s16)gabi::ftoi(wzDbgF(0x664) + 60.0f);
                    Mtx34* m = calc_mtx();
                    yrot = player->shape_angle.y;
                    mDoMtx_YrotS(m, yrot);
                    dbg = 0x668;
                } else {
                    gabi::store<u32>(gabi::ea(player) + 0x430, 0x19);
                    i_this->m4F0 = (s16)gabi::ftoi(wzDbgF(0x66C) + 45.0f);
                    yrot = (s16)(player->shape_angle.y + 0x8000);
                    mDoMtx_YrotS(calc_mtx(), yrot);
                    dbg = 0x670;
                }
                gabi::Local<cXyz> v;
                gabi::Local<cXyz> out;
                gabi::Local<cXyz> sum;
                v->x = 0.0f;
                f32 z = wzDbgF(dbg) + 40000.0f;
                v->y = 0.0f;
                v->z = z;
                MtxPosition(v, out);
                cXyz_pl(out, sum, &player->current.pos);
                i_this->eyePos.copy(*sum);
                wz_demoSe(i_this, 0x5909);
                i_this->m4FA[0] = (s16)(i_this->m4FA[0] + 1);
            }
        }
        cLib_addCalc2(F(0x57C), wzDbgF(0x674) + 50.0f, 1.0f, wzDbgF(0x700) + 0.5f);
        wz_camCalc(F(0x554), wzDbgF(0x678) + 150.0f, wzDbgF(0x684), 0.01f);
        wz_camCalc(F(0x558), wzDbgF(0x67C) + 158.0f, wzDbgF(0x684), 0.01f);
        wz_camCalc(F(0x55C), wzDbgF(0x680) + 114.0f, wzDbgF(0x684), 0.01f);
        wz_camCalc(F(0x560), (f32)(s32)(wzDbgS(0x6B0) + 0x492), wzDbgF(0x684), 0.01f);
        wz_camCalc(F(0x564), (f32)(s32)(wzDbgS(0x6B2) + 0x2CD), wzDbgF(0x684), 0.01f);
        wz_camCalc(F(0x568), (f32)(s32)(wzDbgS(0x6B4) - 0x336), wzDbgF(0x684), 0.01f);
        s16 sub = i_this->m4FA[0];
        if (sub < 2)
            break;
        if (i_this->m4F0 <= (s16)(wzDbgS(0x6B6) + 0x14)) {
            if (sub == 2) {
                wz_demoSe(i_this, 0x5909);
                i_this->m4FA[0] = 3;
            }
            s16 a = (s16)(i_this->mAlpha + (wzDbgS(0x6B8) + 10));
            if (a > 0xFF)
                i_this->mAlpha = 0xFF;
            else
                i_this->mAlpha = a;
        }
        if (wzDbgS(0x92) != 0)
            break;
        if (i_this->m4F0 != 0)
            break;
        gabi::store<u32>(gabi::ea(player) + 0x430, 0x18);
        i_this->m4F0 = (s16)(wzDbgS(0x6BA) + 0x1E);
        i_this->m469 = (u8)(i_this->m469 + 1);
        break;
    }
    case 0x52: {
        s16 a = (s16)(i_this->mAlpha + (wzDbgS(0x6BC) + 5));
        s16 t = i_this->m4F0;
        if (a > 0xFF)
            i_this->mAlpha = 0xFF;
        else
            i_this->mAlpha = a;
        if (t != 0)
            break;
        i_this->m469 = (u8)(i_this->m469 + 1);
        break;
    }
    case 0x53: {
        f32 x = i_this->current.pos.x;
        f32 y = i_this->current.pos.y + 15.0f;
        f32 z = i_this->current.pos.z;
        *F(0x448) = x;
        *F(0x450) = z;
        *F(0x44C) = y;
        anm_init(i_this, 0x11, 0.0f, 0, 1.0f, -1);
        i_this->m469 = (u8)(i_this->m469 + 1);
    }
        [[fallthrough]];
    case 0x54: {
        s16 ang = fopAcM_searchActorAngleY(i_this, dComIfGp_getPlayer(0));
        i_this->current.angle.y = ang;
        i_this->shape_angle.y = ang;
        gabi::Local<cXyz> ppos;
        ppos->y = 0.0f;
        ppos->z = 850.0f;
        *S16(0x502) = ang;
        ppos->x = 50.0f;
        u32 vt = gabi::load<u32>(gabi::ea(player) + 0xB4);
        s16 pang = fopAcM_searchActorAngleY(player, i_this);
        wz_playerSetPos(player, vt, ppos, pang);
        cLib_addCalc2(F(0x57C), wzDbgF(0x48C) + 50.0f, 1.0f, wzDbgF(0x700) + 0.5f);
        *F(0x554) = wzDbgF(0x490) + -108.0f;
        *F(0x558) = wzDbgF(0x494) + 174.0f;
        *F(0x55C) = wzDbgF(0x498) + 231.0f;
        *F(0x560) = wzDbgF(0x49C) + 199.0f;
        *F(0x564) = wzDbgF(0x4A0) + 50.0f;
        *F(0x568) = wzDbgF(0x4A4) + 1092.0f;
        s16 a = (s16)(i_this->mAlpha + (s16)gabi::ftoi(wzDbgF(0x4A8) + 5.0f));
        if (a < 0xFF) {
            i_this->mAlpha = a;
            break;
        }
        i_this->mAlpha = 0xFF;
        if (wzDbgS(0x92) != 0) {
            gabi::store<f32>(i_this->mpMorf + 0x98, 0.0f); /* stop the animation */
            break;
        }
        i_this->m4F0 = (s16)gabi::ftoi(wzDbgF(0x6F8));
        gabi::store<f32>(i_this->mpMorf + 0x98, 1.0f);
        i_this->m469 = (u8)(i_this->m469 + 1);
    }
        [[fallthrough]];
    case 0x55:
        if (i_this->m4F0 != 0)
            break;
        gabi::store<f32>(i_this->mpMorf + 0x98, 1.0f);
        i_this->m469 = (u8)(i_this->m469 + 1);
        [[fallthrough]];
    case 0x56: {
        cLib_addCalc2(F(0x57C), wzDbgF(0x4B0) + 45.0f, 1.0f, wzDbgF(0x704) + 0.2f);
        wz_camCalc(F(0x554), wzDbgF(0x4B4), wzDbgF(0x4CC), 0.025f);
        wz_camCalc(F(0x558), wzDbgF(0x4B8) + 135.0f, wzDbgF(0x4CC), 0.025f);
        wz_camCalc(F(0x55C), wzDbgF(0x4BC) + -60.0f, wzDbgF(0x4CC), 0.025f);
        wz_camCalc(F(0x560), wzDbgF(0x4C0) + 50.0f, wzDbgF(0x4CC), 0.025f);
        wz_camCalc(F(0x564), wzDbgF(0x4C4) + 110.0f, wzDbgF(0x4CC), 0.025f);
        wz_camCalc(F(0x568), wzDbgF(0x4C8) + 330.0f, wzDbgF(0x4CC), 0.025f);
        if (!gabi::at<J3DFrameCtrl>(i_this->mpMorf + 0x98)->checkPass(159.0f))
            break;
        if (wzDbgS(0x92) != 0) {
            gabi::store<f32>(i_this->mpMorf + 0x98, 0.0f);
            break;
        }
        f32 sp = wzDbgF(0x6F0) + 10.0f;
        i_this->m4FA[0] = 0;
        i_this->m4FA[1] = 0;
        i_this->m4F0 = 2;
        i_this->m469 = (u8)(i_this->m469 + 1);
        i_this->speedF = -sp;
        break;
    }
    case 0x57: {
        cLib_addCalc0(&i_this->speedF, 1.0f, wzDbgF(0x6F4) + 0.3f);
        if (wzDbgS(0x508) != 0) {
            i_this->m4F0 = 1;
            gabi::store<s16>(WZ_DBG + 0x508, 0);
        }
        bool bgm;
        if (i_this->m4FA[0] == 0 && i_this->m4F0 == 0) {
            /* open the two summon doors */
            gabi::Local<cXyz> pos;
            gabi::Local<cXyz> scale;
            scale->y = 4.0f;
            pos->z = 100.0f;
            scale->z = 4.0f;
            pos->x = 215.0f;
            pos->y = 180.0f;
            scale->x = 4.0f;
            {
                s8 room = i_this->current.roomNo;
                if (fopAcM_createChild(0xD0, fopAcM_GetID(i_this), 0xFFFFFF0D, pos, room, &i_this->current.angle, scale, 0, 0) ==
                    fpcM_ERROR_PROCESS_ID_e)
                    break;
            }
            pos->y = 180.0f;
            pos->z = 100.0f;
            pos->x = -250.0f;
            {
                s8 room = i_this->current.roomNo;
                if (fopAcM_createChild(0xD0, fopAcM_GetID(i_this), 0xFFFFFF0D, pos, room, &i_this->current.angle, scale, 0, 0) ==
                    fpcM_ERROR_PROCESS_ID_e)
                    break;
            }
            i_this->m4FA[0] = 1;
            *S16(0x4F2) = 0x28;
            s16 t = (s16)gabi::ftoi(wzDbgF(0x6EC) + 70.0f);
            *S16(0x4F4) = t;
            bgm = t == 1;
        } else {
            bgm = *S16(0x4F4) == 1;
        }
        if (bgm)
            mDoAud_subBgmStart_wz(0x80000019);
        if (i_this->m4FA[0] != 0 && i_this->m4FA[1] == 0 && *S16(0x4F2) == 0) {
            /* the mini-boss itself */
            gabi::Local<cXyz> pos;
            pos->y = 0.0f;
            pos->z = 100.0f;
            pos->x = 215.0f;
            u32 id = fopAcM_create(0xBF, 0xFFFFFF2C, pos, i_this->current.roomNo, &i_this->current.angle, nullptr, -1, 0);
            i_this->mParentId = id;
            if (id == 0xFFFFFFFF)
                goto calc57;
            i_this->m4FA[1] = 1;
            gabi::store<u8>(0x101D3594, 1);
        }
        if ((s32)i_this->mParentId != -1) {
            fopAc_ac_c* p = fopAcM_SearchByID(i_this->mParentId);
            if (p != nullptr) {
                u32 s = p->actor_status;
                p->speedF = 0.0f;
                p->actor_status = s | 0x4000;
            }
        }
    calc57:
        cLib_addCalc2(F(0x57C), wzDbgF(0x6C8) + 65.0f, 1.0f, wzDbgF(0x708) + 2.5f);
        wz_camCalc(F(0x554), wzDbgF(0x6CC), wzDbgF(0x6E4), 0.3f);
        wz_camCalc(F(0x558), wzDbgF(0x6D0) + 90.0f, wzDbgF(0x6E4), 0.3f);
        wz_camCalc(F(0x55C), wzDbgF(0x6D4) + 440.0f, wzDbgF(0x6E4), 0.3f);
        wz_camCalc(F(0x560), wzDbgF(0x6D8), wzDbgF(0x6E4), 0.3f);
        wz_camCalc(F(0x564), wzDbgF(0x6DC) + 50.0f, wzDbgF(0x6E4), 0.3f);
        wz_camCalc(F(0x568), wzDbgF(0x6E0) + 570.0f, wzDbgF(0x6E4), 0.3f);
        {
            mDoExt_McaMorf* morf = gabi::at<mDoExt_McaMorf>(i_this->mpMorf);
            if (!(morf->mFrameCtrl.mState & 1) && !(morf->mFrameCtrl.getRate() == 0.0f)) {
                if (!morf->mFrameCtrl.checkPass(wzDbgF(0x6E8) + 274.0f))
                    break;
            }
        }
        if (wzDbgS(0x92) != 0)
            break;
        /* the fight starts */
        dComIfGp_StopQuake_wz(0x20);
        {
            gabi::Local<cXyz> center;
            gabi::Local<cXyz> eye;
            center->z = *F(0x55C);
            center->x = *F(0x554);
            eye->x = *F(0x560);
            eye->y = *F(0x564);
            center->y = *F(0x558);
            eye->z = *F(0x568);
            dCamera_Reset_wz(cam, center, eye);
        }
        dCamera_Start_wz(cam);
        dCamera_SetTrimSize_wz(cam, 0);
        gabi::store<s16>(gabi::ea(player) + 0x420, 2);
        gabi::store<u32>(gabi::ea(player) + 0x430, 1);
        {
            u32 play = dComIfGp_ea();
            gabi::store<u16>(play + 0x52B8, (u16)(gabi::load<u16>(play + 0x52B8) | 8));
        }
        wz_demoMonsSe(i_this);
        {
            fopAc_ac_c* p = fopAcM_SearchByID(i_this->mParentId);
            if (p != nullptr)
                p->actor_status &= ~0x4000u;
        }
        if (wzDbgS(0x752) == 0) {
            i_this->m470 = 1;
            wz_st8(i_this, 0x46E, 1);
            gabi::store<u8>(0x101D3594, 1);
        } else {
            i_this->mParentId = 0xFFFFFFFF;
            gabi::store<u8>(0x101D3594, 1);
        }
        i_this->m468 = 0;
        i_this->m469 = 6;
        i_this->actor_status &= ~0x4000u;
        break;
    }
    case 0x5A: {
        rod_size_set(i_this, 1);
        u32 vt = gabi::load<u32>(gabi::ea(player) + 0xB4);
        s16 ang = (s16)(fopAcM_searchActorAngleY(i_this, dComIfGp_getPlayer(0)) + 0x8000);
        wz_playerSetPos(player, vt, &player->current.pos, ang);
        cLib_addCalc0(&i_this->speedF, 1.0f, 2.0f);
        mDoExt_McaMorf* morf = gabi::at<mDoExt_McaMorf>(i_this->mpMorf);
        if (!(morf->mFrameCtrl.mState & 1) && !(morf->mFrameCtrl.getRate() == 0.0f))
            break;
        i_this->m469 = (u8)(i_this->m469 + 1);
        i_this->speedF = 0.0f;
    }
        [[fallthrough]];
    case 0x5B: {
        if (wzDbgS(0x748) != 0)
            break;
        /* defeated: the summoned enemies vanish */
        for (int i = 0; i < 20; i++) {
            if (gabi::load<u8>(base + 0x4DC + i) == 0)
                continue;
            fopAc_ac_c* c = fopAcM_SearchByID(i_this->mChildId[i]);
            if (c != nullptr)
                c->actor_status |= 0x4000;
            wz_st8(i_this, 0x4DC + i, 0);
        }
        fopAcM_createDisappear(i_this, dpos, 10, 0, i_this->stealItemBitNo);
        u32 tg = wz_ld32(i_this, 0x7D8);
        i_this->mRodScaleX = 0.0f;
        i_this->mRodScaleY = 0.0f;
        i_this->mRodScaleZ = 0.0f;
        u32 s = i_this->actor_status;
        wz_st32(i_this, 0x39C, 0);
        wz_st32(i_this, 0x7D8, tg & ~1u);
        i_this->speedF = 0.0f;
        i_this->scale.x = 0.0f;
        i_this->scale.y = 0.0f;
        i_this->actor_status = s & ~0x20u;
        i_this->mAlpha = 0;
        i_this->scale.z = 0.0f;
        dCcD_GObjInf_ClrTgHit(i_this->mBodyCyl);
        i_this->m4F0 = (s16)(wzDbgS(0x746) + 0x1E);
        i_this->m469 = (u8)(i_this->m469 + 1);
    }
        [[fallthrough]];
    case 0x5C: {
        if (i_this->m4F0 != 0)
            break;
        dComIfGp_StopQuake_wz(0x20);
        {
            gabi::Local<cXyz> center;
            gabi::Local<cXyz> eye;
            eye->z = *F(0x568);
            center->y = *F(0x558);
            eye->x = *F(0x560);
            center->x = *F(0x554);
            eye->y = *F(0x564);
            center->z = *F(0x55C);
            dCamera_Reset_wz(cam, center, eye);
        }
        dCamera_Start_wz(cam);
        dCamera_SetTrimSize_wz(cam, 0);
        gabi::store<s16>(gabi::ea(player) + 0x420, 2);
        gabi::store<u32>(gabi::ea(player) + 0x430, 1);
        {
            u32 play = dComIfGp_ea();
            gabi::store<u16>(play + 0x52B8, (u16)(gabi::load<u16>(play + 0x52B8) | 8));
        }
        if (wz_isStartStage(STR(0x10042D28) /* "kazeMB" */) && i_this->mIsMiniBoss)
            mDoAud_subBgmStop();
        if (i_this->m470 != 0) {
            i_this->m468 = 1;
            i_this->m469 = 0x2B;
            break;
        }
        fopAcM_delete(i_this);
        if (wz_isStartStage(STR(0x10042D28)))
            break;
        if (i_this->mDisableSpawnOnDeathSwitch != 0xFF)
            dComIfGs_onSwitch(i_this->mDisableSpawnOnDeathSwitch, i_this->current.roomNo);
        dComIfGs_onActor(i_this->setID, i_this->home.roomNo);
        break;
    }
    default:
        break;
    }

    /* the demo camera */
    if (i_this->health <= 0) {
        f32 x = i_this->current.pos.x, y = i_this->current.pos.y, z = i_this->current.pos.z;
        *F(0x554) = x;
        *F(0x55C) = z;
        *F(0x558) = y + 50.0f;
        Mtx34* m = calc_mtx();
        mDoMtx_YrotS(m, fopAcM_searchActorAngleY(i_this, dComIfGp_getPlayer(0)));
        gabi::Local<cXyz> v;
        gabi::Local<cXyz> out;
        v->x = 0.0f;
        v->y = 0.0f;
        v->z = 400.0f;
        MtxPosition(v, out);
        PSVECAdd(out, &i_this->current.pos, out);
        f32 ex = out->x - 100.0f;
        f32 ey = out->y + 400.0f;
        *F(0x560) = ex;
        f32 ez = out->z - 300.0f;
        *F(0x564) = ey;
        *F(0x568) = ez;
    }
    if (i_this->mCurrBckIdx == 0x11 && gabi::at<J3DFrameCtrl>(i_this->mpMorf + 0x98)->checkPass(70.0f))
        wz_demoMonsSe(i_this);
    s8 h = i_this->health;
    u8 st2 = i_this->m469;
    if (h > 0 ? st2 >= 0x51 : st2 >= 0x5A) {
        gabi::Local<cXyz> center;
        gabi::Local<cXyz> eye;
        eye->x = *F(0x560);
        center->y = *F(0x558);
        eye->z = *F(0x568);
        center->x = *F(0x554);
        eye->y = *F(0x564);
        center->z = *F(0x55C);
        dCamera_Set_wz(cam, center, eye, *F(0x57C), 0);
    }
}
VERIFY(0x024E7904, action_demo);

/* ---- execute (GameCube action_dousa / action_itai / action_tama_dousa / summon_call_sub /
 * sea_water_check inlined) ---- */

static inline BOOL enemy_ice_wz(void* ice) { return gabi::call<BOOL>(0x020402C8, ice); }
static inline void JPASetRMtxSTVecfromMtx_wz(u32 mtx, u32 r, u32 s, u32 t) {
    gabi::call(0x02824890, gabi::at<void>(mtx), gabi::at<void>(r), gabi::at<void>(s), gabi::at<void>(t));
}
static inline BOOL dBgS_ChkMoveBG_wz(void* polyInfo) { return gabi::call<BOOL>(0x024EEABC, dComIfG_Bgsp(), polyInfo); }
static inline BOOL dCcD_GObjInf_ChkAtHit_wz(void* o) { return gabi::call<BOOL>(0x025160DC, o); }
static inline void dKy_arrowcol_chg_on_wz(void* a, s32 b) { gabi::call(0x0255F3E8, a, b); }
static inline void fopAcM_setGbaName_wz(fopAc_ac_c* a, u8 itemNo, u8 n0, u8 n1) { gabi::call(0x025DA088, a, itemNo, n0, n1); }
static inline void fopAcM_cancelCarryNow_wz(fopAc_ac_c* a) { gabi::call(0x025D9D24, a); }
static inline void enemy_piyo_set_wz(fopAc_ac_c* a) { gabi::call(0x02041CAC, a); }
static inline void cCcS_Set_wz(void* obj) { gabi::call(0x0200E240, gabi::at<void>(dComIfGp_ea() + 0x26A4), obj); }
static inline void dCcMassS_Mng_Set_wz(void* obj, u8 p) { gabi::call(0x02516C14, gabi::at<void>(dComIfGp_ea() + 0x4EF8), obj, p); }

/* the rod-tip emitters follow the rod tip (rod joint 2) */
static void wz_rodEmitterMtx(wz_class* i_this, u32 em) {
    u32 model = morf_model(i_this->mpRodMorf);
    u32 blk = gabi::load<u32>(model + 0x2C);
    u16 fl = gabi::load<u16>(blk + 4);
    u32 mtx = gabi::load<u32>(blk + 0x10);
    gabi::store<u16>(blk + 4, (u16)(fl | 0x10));
    JPASetRMtxSTVecfromMtx_wz(mtx + 0x60, em + 0x1F0, em + 0x220, em + 0x22C);
}
static inline u32 wz_cbEmitter(wz_class* i_this, int i) { return gabi::load<u32>(gabi::ea(i_this->mFollowCb) + 0x14 * i + 4); }
static inline void wz_rodParticle(wz_class* i_this, u16 id, int cb) {
    dPa_control_set(dComIfGp_getParticle(), 0, id, &i_this->mRodTipPos, nullptr, nullptr, 0xFF,
                    gabi::at<dPa_levelEcallBack>(gabi::ea(i_this->mFollowCb) + 0x14 * cb), -1, nullptr, nullptr, nullptr);
}
/* the summoner colours of the rod light: prim (0x101D35C8) and env (0x101D35D4) per summon kind */
static void wz_rodColor(wz_class* i_this, int cb) {
    u32 em = wz_cbEmitter(i_this, cb);
    u32 t = 0x101D35C8 + gabi::load<u8>(gabi::ea(i_this) + 0x514) * 4;
    u8 c2 = gabi::load<u8>(t + 2), c0 = gabi::load<u8>(t), c1 = gabi::load<u8>(t + 1);
    gabi::store<u8>(em + 0x244, c0);
    gabi::store<u8>(em + 0x245, c1);
    gabi::store<u8>(em + 0x246, c2);
    t = 0x101D35D4 + gabi::load<u8>(gabi::ea(i_this) + 0x514) * 4;
    em = wz_cbEmitter(i_this, cb);
    c2 = gabi::load<u8>(t + 2), c0 = gabi::load<u8>(t), c1 = gabi::load<u8>(t + 1);
    gabi::store<u8>(em + 0x248, c0);
    gabi::store<u8>(em + 0x249, c1);
    gabi::store<u8>(em + 0x24A, c2);
    wz_rodEmitterMtx(i_this, wz_cbEmitter(i_this, cb));
}
static inline bool wz_morfStopped(wz_class* i_this) {
    mDoExt_McaMorf* m = gabi::at<mDoExt_McaMorf>(i_this->mpMorf);
    return (m->mFrameCtrl.mState & 1) || m->mFrameCtrl.getRate() == 0.0f;
}
static inline void wz_monsSe(wz_class* i_this, u32 id) {
    if (gabi::ea(i_this) != 0 && gabi::ea(i_this) + 0x37C != 0) {
        s8 room = i_this->current.roomNo;
        u32 pid = fopAcM_GetID(i_this);
        mDoAud_monsSeStart(id, &i_this->eyePos, pid, 0, dComIfGp_getReverb(room));
    }
}
static inline void wz_followEnd2(wz_class* i_this, int a, int b) {
    wz_followEnd(i_this, a);
    wz_followEnd(i_this, b);
}
static inline void wz_tamaNext(wz_class* i_this, f32 x, f32 y, f32 z) {
    gabi::Local<cXyz> p;
    p->x = x;
    p->y = y;
    p->z = z;
    next_tama_move(i_this, p);
}
#define WZ_514(t) gabi::load<u8>(gabi::ea(t) + 0x514)
static inline u8 wz_ld8(wz_class* a, u32 off) { return gabi::load<u8>(gabi::ea(a) + off); }

/* 024E398C */
static BOOL daWZ_Execute(wz_class* i_this) {
    WWHD_FUNC(0x024E398C, BOOL, i_this);
    u32 base = gabi::ea(i_this);
    auto T = [&](int i) { return gabi::at<be<s16>>(base + 0x4F0 + 2 * i); };
    auto F = [&](u32 off) { return gabi::at<be<f32>>(base + off); };
    if (i_this->mBehaviorType < 10 && i_this->mEnableSpawnSwitch != 0xFF &&
        !dComIfGs_isSwitch(i_this->mEnableSpawnSwitch, gabi::load<s8>(0x1047E6C8)))
        return TRUE;
    if (wzDbgS(0x752) == 0 && wz_ld8(i_this, 0x46E) != 0) {
        if (fopAcM_SearchByID(i_this->mParentId) == nullptr) {
            i_this->m470 = 0;
            wz_st8(i_this, 0x46E, 0);
            i_this->mParentId = 0xFFFFFFFF;
        }
    }
    {
        u8 type = i_this->mBehaviorType;
        cpy_u32(base + 0xA1C, gabi::ea(&i_this->current.pos.x)); /* the light follows */
        cpy_u32(base + 0xA20, gabi::ea(&i_this->current.pos.y));
        cpy_u32(base + 0xA24, gabi::ea(&i_this->current.pos.z));
        if (type == 10 || type == 11) {
            if (type == 10) {
                wz_st16(i_this, 0xA28, (s16)gabi::ftoi(wzDbgF(0x14) + 300.0f));
                wz_st16(i_this, 0xA2A, (s16)gabi::ftoi(wzDbgF(0x18) + 50.0f));
                wz_st16(i_this, 0xA2C, (s16)gabi::ftoi(wzDbgF(0x1C)));
            } else {
                wz_st16(i_this, 0xA28, (s16)gabi::ftoi(wzDbgF(0x20) + 300.0f));
                wz_st16(i_this, 0xA2A, (s16)gabi::ftoi(wzDbgF(0x24) + 300.0f));
                wz_st16(i_this, 0xA2C, (s16)gabi::ftoi(wzDbgF(0x28) + 20.0f));
            }
            *F(0xA30) = wzDbgF(0x2C) + 550.0f;
            *F(0xA34) = wzDbgF(0x30) + 200.0f;
            type = i_this->mBehaviorType;
        }
        if (type < 10) {
            fopAcM_setGbaName_wz(i_this, 0x3C, 0x11, 0x2F);
            if (enemy_ice_wz(gabi::at<void>(base + 0xA40))) {
                /* frozen: the models only follow the ice matrix */
                u32 model = morf_model(i_this->mpMorf);
                f32 m[12];
                for (int k = 0; k < 12; k++)
                    m[k] = gabi::load<f32>(0x1048D0CC + 4 * k);
                for (int k = 0; k < 12; k++)
                    gabi::store<f32>(model + 0xC8 + 4 * k, m[k]);
                McaMorf_calc(gabi::at<mDoExt_McaMorf_c>(i_this->mpMorf));
                if (i_this->mIsMiniBoss) {
                    u32 blk = gabi::load<u32>(morf_model(i_this->mpMorf) + 0x2C);
                    u32 model2 = morf_model(i_this->mpMorf2);
                    u16 fl = gabi::load<u16>(blk + 4);
                    u32 src = gabi::load<u32>(blk + 0x10) + 0x390;
                    gabi::store<u16>(blk + 4, (u16)(fl | 0x10));
                    for (int k = 0; k < 12; k++)
                        m[k] = gabi::load<f32>(src + 4 * k);
                    for (int k = 0; k < 12; k++)
                        gabi::store<f32>(model2 + 0xC8 + 4 * k, m[k]);
                    McaMorf_calc(gabi::at<mDoExt_McaMorf_c>(i_this->mpMorf2));
                }
                enemy_fire_remove(i_this->mEnemyFire);
                rod_size_set(i_this, 1);
                BG_check(i_this);
                return TRUE;
            }
        }
    }
    for (int i = 0; i < 5; i++) {
        s16 t = *T(i);
        if (t != 0)
            *T(i) = (s16)(t - 1);
    }

    switch (i_this->m468) {
    case 0: { /* action_dousa */
        dComIfGp_get();
        switch (i_this->m469) {
        case 0: {
            f32 d = fopAcM_searchActorDistance(i_this, dComIfGp_getPlayer(0));
            if (d > 7500.0f)
                goto dousa_end;
            for (int i = 0; i < 4; i++)
                i_this->m4FA[i] = 0;
            if (i_this->mBehaviorType == 1) {
                u32 pnt = gabi::load<u32>(wz_ld32(i_this, 0x540) + 8) + wz_ld32(i_this, 0x474) * 16;
                *F(0x454) = gabi::load<f32>(pnt + 4);
                *F(0x458) = gabi::load<f32>(pnt + 8);
                wz_st8(i_this, 0x514, 1);
                *F(0x45C) = gabi::load<f32>(pnt + 0xC);
            }
            i_this->scale.x = 1.0f;
            i_this->scale.y = 1.0f;
            i_this->scale.z = 1.0f;
            wz_demoSe(i_this, 0x5909);
            anm_init(i_this, 0x10, 5.0f, 2, 1.0f, -1);
            u32 s = i_this->actor_status;
            wz_st32(i_this, 0x39C, 4);
            i_this->actor_status = s | 0x20;
            i_this->m469 = (u8)(i_this->m469 + 1);
        }
            [[fallthrough]];
        case 1: {
            s16 a = (s16)(i_this->mAlpha + 8);
            if (a < 0xFF) {
                i_this->mAlpha = a;
                goto dousa_tail;
            }
            i_this->mAlpha = 0xFF;
            i_this->m469 = (u8)(i_this->m469 + 1);
        }
            [[fallthrough]];
        case 2: {
            *T(0) = 0x5A;
            u8 f = 0;
            wz_st8(i_this, 0x514, 0);
            if (i_this->mBehaviorType == 1 && i_this->m470 == 0) {
                f32 dx = *F(0x454) - i_this->current.pos.x;
                f32 dz = *F(0x45C) - i_this->current.pos.z;
                wz_st8(i_this, 0x514, 1);
                s16 a = cM_atan2s(dx, dz);
                f = WZ_514(i_this);
                wz_st16(i_this, 0x502, a);
            }
            if (f >= 1) {
                if (f == 1)
                    anm_init(i_this, 0x13, 5.0f, 2, 1.0f, -1);
            } else {
                anm_init(i_this, 0x14, 5.0f, 2, 1.0f, -1);
            }
            wz_rodParticle(i_this, 0x8287, 2);
            u32 em = wz_cbEmitter(i_this, 2);
            if (em != 0)
                wz_rodEmitterMtx(i_this, em);
            i_this->m469 = (u8)(i_this->m469 + 1);
            wz_st32(i_this, 0x7D8, wz_ld32(i_this, 0x7D8) | 1);
            goto dousa_tail;
        }
        case 3: {
            u32 em = wz_cbEmitter(i_this, 2);
            if (em != 0)
                wz_rodEmitterMtx(i_this, em);
            rod_size_set(i_this, 0);
            if (*T(0) != 0)
                goto dousa_end;
            i_this->m469 = (u8)(i_this->m469 + 1);
            wz_followEnd(i_this, 2);
            goto dousa_tail;
        }
        case 4: {
            anm_init(i_this, 6, 5.0f, 0, 1.0f, -1);
            if (wz_cbEmitter(i_this, 3) != 0 || (wz_rodParticle(i_this, 0x8288, 3), wz_cbEmitter(i_this, 3) != 0))
                wz_rodColor(i_this, 3);
            if (wz_cbEmitter(i_this, 4) != 0 || (wz_rodParticle(i_this, 0x8289, 3), wz_cbEmitter(i_this, 4) != 0))
                wz_rodColor(i_this, 4);
            u8 f = WZ_514(i_this);
            if (f < 1)
                wz_se(i_this, 0x590C, 0);
            else if (f == 1)
                wz_se(i_this, 0x590D, 0);
            i_this->m469 = (u8)(i_this->m469 + 1);
            goto dousa_tail;
        }
        case 5: {
            u32 em = wz_cbEmitter(i_this, 3);
            if (em != 0)
                wz_rodEmitterMtx(i_this, em);
            em = wz_cbEmitter(i_this, 4);
            if (em != 0)
                wz_rodEmitterMtx(i_this, em);
            u32 morf;
            if (WZ_514(i_this) != 1) {
                morf = i_this->mpMorf;
                if (gabi::load<f32>(morf + 0x9C) < 38.0f) {
                    wz_st16(i_this, 0x502, fopAcM_searchActorAngleY(i_this, dComIfGp_getPlayer(0)));
                    morf = i_this->mpMorf;
                }
            } else {
                morf = i_this->mpMorf;
            }
            if (gabi::at<J3DFrameCtrl>(morf + 0x98)->checkPass(35.0f)) {
                u8 f = WZ_514(i_this);
                if (f < 1 || (f == 1 && i_this->m470 != 0))
                    weapon_shoot(i_this, 0);
                else if (f == 1)
                    weapon_shoot(i_this, 1);
            }
            if (!wz_morfStopped(i_this))
                goto dousa_end;
            wz_monsSe(i_this, 0x48D6);
            i_this->m469 = 6;
            wz_followEnd2(i_this, 3, 4);
            goto dousa_tail;
        }
        case 6: {
            i_this->speed.x = 0.0f;
            i_this->speed.y = 0.0f;
            i_this->speed.z = 0.0f;
            i_this->speedF = 0.0f;
            fopAcM_cancelCarryNow_wz(i_this);
            u32 v = wz_ld32(i_this, 0x39C);
            i_this->shape_angle.x = 0;
            i_this->current.angle.x = 0;
            i_this->shape_angle.z = 0;
            wz_st32(i_this, 0x39C, v & ~0x10u);
            i_this->current.angle.z = 0;
            anm_init(i_this, 0x10, 5.0f, 2, 1.0f, -1);
            u32 s = i_this->actor_status;
            u32 tg = wz_ld32(i_this, 0x7D8);
            wz_st32(i_this, 0x39C, 0);
            i_this->actor_status = s & ~0x20u;
            wz_st32(i_this, 0x7D8, tg & ~1u);
            dCcD_GObjInf_ClrTgHit(i_this->mBodyCyl);
            i_this->current.angle.y = i_this->shape_angle.y;
            i_this->m469 = (u8)(i_this->m469 + 1);
        }
            [[fallthrough]];
        case 7: {
            s16 a = (s16)(i_this->current.angle.y - (wzDbgS(0x506) + 0x700));
            i_this->current.angle.y = a;
            i_this->shape_angle.y = a;
            wz_st16(i_this, 0x502, a);
            rod_size_set(i_this, 1);
            s16 al = (s16)(i_this->mAlpha - 8);
            i_this->mAlpha = al;
            if (al > 0)
                goto dousa_end;
            if (i_this->m470 != 0)
                *T(1) = (s16)gabi::ftoi(cM_rndF(100.0f) + 100.0f);
            i_this->m469 = (u8)(i_this->m469 + 1);
            i_this->scale.x = 0.0f;
            i_this->scale.y = 0.0f;
            i_this->scale.z = 0.0f;
            i_this->mAlpha = 0;
        }
            [[fallthrough]];
        case 8: {
            if (*T(1) != 0)
                goto dousa_end;
            u32 path;
            if (gabi::load<u8>(base + 0x53D) != 0xFF && (path = wz_ld32(i_this, 0x52C)) != 0) {
                /* warp to a random point of the path */
                f32 r = cM_rndF((f32)gabi::load<u16>(path));
                path = wz_ld32(i_this, 0x52C);
                u32 idx = (u32)gabi::ftoi(r);
                wz_st32(i_this, 0x474, idx);
                if (idx == gabi::load<u16>(path)) {
                    path = wz_ld32(i_this, 0x52C);
                    idx--;
                    wz_st32(i_this, 0x474, idx);
                }
                u32 pnt = gabi::load<u32>(path + 8) + idx * 16;
                f32 x = gabi::load<f32>(pnt + 4);
                i_this->current.pos.x = x;
                f32 y = gabi::load<f32>(pnt + 8);
                u32 wx = gabi::load<u32>(base + 0x314);
                i_this->current.pos.y = y;
                f32 z = gabi::load<f32>(pnt + 0xC);
                i_this->mRodScaleX = 0.0f;
                i_this->current.pos.z = z;
                u32 wz = gabi::load<u32>(base + 0x31C);
                i_this->old.pos.x = x;
                i_this->mRodScaleY = 0.0f;
                i_this->mRodScaleZ = 0.0f;
                i_this->m469 = 0;
                wz_st32(i_this, 0x448, wx);
                wz_st32(i_this, 0x44C, gabi::load<u32>(base + 0x318));
                i_this->old.pos.y = y;
                i_this->old.pos.z = z;
                wz_st32(i_this, 0x450, wz);
                goto dousa_tail;
            }
            f32 r = cM_rndFX(200.0f);
            f32 hx = *F(0x448);
            f32 hy = *F(0x44C);
            i_this->current.pos.x = hx + r;
            i_this->current.pos.y = hy;
            r = cM_rndFX(200.0f);
            f32 hz = *F(0x450);
            i_this->m469 = 0;
            i_this->current.pos.z = hz + r;
            i_this->mRodScaleX = 0.0f;
            i_this->mRodScaleY = 0.0f;
            i_this->mRodScaleZ = 0.0f;
            goto dousa_end;
        }
        default:
            goto dousa_end;
        }
    dousa_end:
        if (WZ_514(i_this) == 1)
            goto atari;
    dousa_tail:
        if (WZ_514(i_this) == 1)
            goto atari;
        if (i_this->m469 >= 4)
            goto atari;
        wz_st16(i_this, 0x502, fopAcM_searchActorAngleY(i_this, dComIfGp_getPlayer(0)));
        body_atari_check(i_this);
        goto typecheck;
    }
    case 1: { /* action_itai */
        fopAc_ac_c* player = dComIfGp_getPlayer(0);
        gabi::Local<cXyz> dpos;
        dpos->x = i_this->current.pos.x;
        dpos->y = i_this->current.pos.y;
        dpos->z = i_this->current.pos.z;
        bool eye = false;
        switch (i_this->m469) {
        case 10: {
            s16 py = player->shape_angle.y;
            i_this->current.angle.y = py;
            wz_st16(i_this, 0x502, py);
            i_this->shape_angle.y = fopAcM_searchActorAngleY(i_this, dComIfGp_getPlayer(0));
            wz_followEnd(i_this, 2);
            wz_followEnd2(i_this, 3, 4);
            for (int i = 0; i < 4; i++)
                i_this->m4FA[i] = 0;
            u8 k = i_this->mHitKind;
            if (k == 7) {
                u32 tg = wz_ld32(i_this, 0x7D8);
                i_this->speedF = 0.0f;
                wz_st32(i_this, 0x7D8, tg & ~1u);
                dCcD_GObjInf_ClrTgHit(i_this->mBodyCyl);
                i_this->mRodScaleX = 0.0f;
                i_this->mRodScaleY = 0.0f;
                i_this->mRodScaleZ = 0.0f;
                wz_st32(i_this, 0x39C, 0);
                anm_init(i_this, 0xD, 5.0f, 0, 1.0f, -1);
                i_this->m469 = 0x14;
            } else if (k == 1 || k == 8) {
                anm_init(i_this, 9, 5.0f, 0, 1.0f, -1);
                i_this->m469 = 0xB;
                i_this->speedF = 48.0f;
            } else if (k == 3) {
                anm_init(i_this, 8, 5.0f, 0, 1.0f, -1);
                i_this->m469 = 0xC;
                i_this->speedF = 32.0f;
            } else if (k == 4) {
                enemy_piyo_set_wz(i_this);
                wz_se(i_this, 0x50BC, 0, false);
                anm_init(i_this, 0xC, 5.0f, 0, 1.0f, -1);
                i_this->speedF = 28.0f;
                *T(0) = 0x4B;
                i_this->m469 = 0xD;
            } else if (k == 9) {
                anm_init(i_this, 0xC, 5.0f, 0, 1.0f, -1);
                i_this->speedF = 28.0f;
                i_this->m469 = 0xC;
            } else {
                anm_init(i_this, 9, 5.0f, 0, 1.0f, -1);
                i_this->m469 = 0xB;
                i_this->speedF = 32.0f;
            }
            eye = gabi::ea(i_this) + 0x37C != 0;
            if (i_this->mHitKind != 7 && i_this->health <= 0) {
                if (eye)
                    wz_monsSe(i_this, 0x48D9);
                i_this->m469 = 0x28;
                if (i_this->mIsMiniBoss)
                    goto boss_die;
                goto itai_end;
            }
            if (eye)
                wz_monsSe(i_this, 0x48D7);
            goto itai_end;
        }
        case 11: {
            cLib_addCalc0(&i_this->speedF, 1.0f, 2.0f);
            f32 v = i_this->speedF;
            if (std::fabs(v) < 0.2f)
                goto itai_stop;
            goto itai_end;
        }
        case 12:
            cLib_addCalc0(&i_this->speedF, 1.0f, 2.0f);
            if (wz_morfStopped(i_this))
                goto itai_stop;
            goto itai_end;
        case 13: {
            cLib_addCalc0(&i_this->speedF, 1.0f, 2.0f);
            if (*T(0) != 0)
                goto itai_end;
            wz_st32(i_this, 0x7D8, wz_ld32(i_this, 0x7D8) & ~1u);
            dCcD_GObjInf_ClrTgHit(i_this->mBodyCyl);
            i_this->m468 = 0;
            i_this->m469 = 6;
            goto itai_end;
        }
        itai_stop:
            {
                u32 tg = wz_ld32(i_this, 0x7D8);
                i_this->speedF = 0.0f;
                wz_st32(i_this, 0x7D8, tg & ~1u);
                dCcD_GObjInf_ClrTgHit(i_this->mBodyCyl);
                i_this->m468 = 0;
                i_this->m469 = 6;
                goto itai_end;
            }
        case 20: {
            if (!wz_morfStopped(i_this))
                goto itai_end;
            if (i_this->health > 0) {
                anm_init(i_this, 0xE, 0.0f, 0, 1.0f, -1);
                i_this->m469 = 0x15;
                goto itai_end;
            }
            wz_monsSe(i_this, 0x48D9);
            i_this->m469 = 0x2A;
            if (!i_this->mIsMiniBoss)
                goto itai_end;
            goto boss_die;
        }
        case 21:
            if (!wz_morfStopped(i_this))
                goto itai_end;
            anm_init(i_this, 0xF, 0.0f, 0, 1.0f, -1);
            i_this->m469 = 0x16;
            goto itai_end;
        case 22: {
            s16 a = (s16)(i_this->mAlpha - 8);
            if (a < 0)
                i_this->mAlpha = 0;
            else
                i_this->mAlpha = a;
            mDoExt_McaMorf* m = gabi::at<mDoExt_McaMorf>(i_this->mpMorf);
            (void)m;
            if (!wz_morfStopped(i_this))
                goto itai_end;
            i_this->m468 = 0;
            i_this->m469 = 6;
            i_this->mAlpha = 0;
            goto itai_end;
        }
        case 0x28: {
            s16 py = player->shape_angle.y;
            wz_st16(i_this, 0x502, py);
            i_this->current.angle.y = py;
            s16 a = fopAcM_searchActorAngleY(i_this, dComIfGp_getPlayer(0));
            i_this->shape_angle.y = a;
            i_this->speedF = 40.0f;
            anm_init(i_this, 5, 5.0f, 0, 1.0f, -1);
            i_this->m469 = (u8)(i_this->m469 + 1);
        }
            [[fallthrough]];
        case 0x29:
            rod_size_set(i_this, 1);
            cLib_addCalc0(&i_this->speedF, 1.0f, 2.0f);
            if (!wz_morfStopped(i_this))
                goto itai_end;
            i_this->m469 = (u8)(i_this->m469 + 1);
            i_this->speedF = 0.0f;
            [[fallthrough]];
        case 0x2A: {
            if (i_this->m470 == 0) {
                f32 y = dpos->y;
                y = i_this->mCurrBckIdx == 5 ? y + 160.0f : y + 20.0f;
                dpos->y = y;
                fopAcM_createDisappear(i_this, dpos, 10, 0, i_this->stealItemBitNo);
                fopAcM_delete(i_this);
                if (wz_isStartStage(STR(0x10042D20) /* "kazeMB" */))
                    goto itai_end;
                goto itai_sw;
            }
            s32 bck = i_this->mCurrBckIdx;
            i_this->mAlpha = 0;
            i_this->speedF = 0.0f;
            f32 y = dpos->y;
            y = bck == 5 ? y + 160.0f : y + 20.0f;
            dpos->y = y;
            fopAcM_createDisappear(i_this, dpos, 10, 0, i_this->stealItemBitNo);
            u32 s = i_this->actor_status;
            u32 tg = wz_ld32(i_this, 0x7D8);
            i_this->scale.x = 0.0f;
            i_this->scale.y = 0.0f;
            i_this->scale.z = 0.0f;
            i_this->actor_status = s & ~0x20u;
            i_this->mRodScaleX = 0.0f;
            i_this->mRodScaleY = 0.0f;
            i_this->mRodScaleZ = 0.0f;
            wz_st32(i_this, 0x39C, 0);
            wz_st32(i_this, 0x7D8, tg & ~1u);
            dCcD_GObjInf_ClrTgHit(i_this->mBodyCyl);
            i_this->m469 = (u8)(i_this->m469 + 1);
            goto itai_end;
        }
        case 0x2B:
            if (i_this->m470 != 0)
                goto itai_end;
            fopAcM_delete(i_this);
            if (wz_isStartStage(STR(0x10042D20)))
                goto itai_end;
        itai_sw:
            if (i_this->mDisableSpawnOnDeathSwitch != 0xFF)
                dComIfGs_onSwitch(i_this->mDisableSpawnOnDeathSwitch, i_this->current.roomNo);
            dComIfGs_onActor(i_this->setID, i_this->home.roomNo);
            goto itai_end;
        default:
            goto itai_end;
        }
    boss_die:
        i_this->m468 = 3;
        i_this->m469 = 0x46;
    itai_end:
        if (i_this->health > 0)
            goto atari;
        goto typecheck;
    }
    case 3:
        action_demo(i_this);
        goto typecheck;
    case 100: { /* action_tama_dousa: the fire ball / the summon ball */
        dComIfGp_get();
        gabi::Local<csXyz> ang;
        gabi::Local<cXyz> scl;
        ang->x = i_this->current.angle.x;
        ang->y = i_this->current.angle.y;
        ang->z = i_this->current.angle.z;
        scl->x = i_this->scale.x;
        scl->y = i_this->scale.y;
        scl->z = i_this->scale.z;
        u8 st = i_this->m469;
        if (st == 0x64) {
            for (int i = 0; i < 4; i++)
                i_this->m4FA[i] = 0;
            *F(0x528) = 25.0f;
            cpy_u32(base + 0x1024, base + 0x314);
            cpy_u32(base + 0x1028, base + 0x318);
            cpy_u32(base + 0x102C, base + 0x31C);
            gabi::store<u16>(base + 0x1030, gabi::load<u16>(base + 0x328));
            u8 type = i_this->mBehaviorType;
            gabi::store<u16>(base + 0x1032, gabi::load<u16>(base + 0x32A));
            gabi::store<u16>(base + 0x1034, gabi::load<u16>(base + 0x32C));
            if (type == 10 || type == 11) {
                u16 id0 = type == 10 ? 0x8238 : 0x8280;
                if (wz_cbEmitter(i_this, 0) == 0)
                    dPa_control_set(dComIfGp_getParticle(), 0, id0, gabi::at<cXyz>(base + 0x1024), gabi::at<csXyz>(base + 0x1030), nullptr, 0xFF,
                                    gabi::at<dPa_levelEcallBack>(base + 0x1038), -1, nullptr, nullptr, nullptr);
                if (wz_cbEmitter(i_this, 1) == 0)
                    dPa_control_set(dComIfGp_getParticle(), 0, (u16)(id0 + 1), gabi::at<cXyz>(base + 0x1024), gabi::at<csXyz>(base + 0x1030),
                                    nullptr, 0xFF, gabi::at<dPa_levelEcallBack>(base + 0x104C), -1, nullptr, nullptr, nullptr);
            }
            i_this->speedF = 45.0f;
            i_this->m469 = (u8)(i_this->m469 + 1);
            st = 0x65;
        }
        if (st == 0x65) {
            u32 flags;
            if (i_this->mBehaviorType == 11) {
                /* the summon ball flies to the summon point */
                wz_se(i_this, 0x7048, 0, false);
                f32 ty = *F(0x458) + 300.0f;
                f32 dy = ty - i_this->current.pos.y;
                f32 dx = *F(0x454) - i_this->current.pos.x;
                f32 dz = *F(0x45C) - i_this->current.pos.z;
                f32 d = std_sqrtf(gabi::fmadds(dz, dz, gabi::fmadds(dx, dx, dy * dy)));
                if (d < 50.0f) {
                    i_this->speedF = 0.0f;
                    i_this->m469 = 0x66;
                    goto typecheck;
                }
                flags = wz_ld32(i_this, 0x5E8);
            } else {
                wz_se(i_this, 0x7047, 0, false);
                cpy_u32(base + 0x1024, base + 0x314);
                cpy_u32(base + 0x1028, base + 0x318);
                cpy_u32(base + 0x102C, base + 0x31C);
                bool splash;
                if (daSea_ChkArea(i_this->current.pos.x, i_this->current.pos.z)) {
                    f32 h = daSea_calcWave(i_this->current.pos.x, i_this->current.pos.z) + 50.0f;
                    splash = i_this->current.pos.y < h;
                    flags = 0;
                } else {
                    flags = wz_ld32(i_this, 0x5E8);
                    if (!(flags & 0x1000))
                        goto tama_wall;
                    f32 h = *F(0x77C) + 50.0f;
                    splash = i_this->current.pos.y < h;
                }
                if (splash) {
                    gabi::Local<cXyz> s2;
                    s2->x = 2.0f;
                    s2->y = 2.0f;
                    s2->z = 2.0f;
                    dPa_control_set(dComIfGp_getParticle(), 0, 0x35A, &i_this->current.pos, &i_this->current.angle, s2, 0xFF, nullptr, -1, nullptr,
                                    nullptr, nullptr);
                    wz_followEnd2(i_this, 0, 1);
                    fopAcM_delete(i_this);
                }
                flags = wz_ld32(i_this, 0x5E8);
            }
        tama_wall:
            if (flags & 0x20) {
                if (i_this->mBehaviorType != 11 && !dBgS_ChkMoveBG_wz(gabi::at<void>(base + 0x6A8))) {
                    *F(0x528) = 40.0f;
                    i_this->speedF = 0.0f;
                    i_this->m469 = 0x66;
                    goto typecheck;
                }
                wz_tamaNext(i_this, i_this->current.pos.x, i_this->current.pos.y, i_this->current.pos.z);
                goto typecheck;
            }
            if (*T(2) == 0 || (flags & 0x10)) {
                wz_tamaNext(i_this, i_this->current.pos.x, i_this->current.pos.y, i_this->current.pos.z);
                goto typecheck;
            }
            if ((wz_ld32(i_this, 0x944) & 1) || dCcD_GObjInf_ChkAtHit_wz(i_this->mBallSph)) {
                f32 z = *F(0x968), x = *F(0x960), y = *F(0x964);
                wz_tamaNext(i_this, x, y, z);
                goto typecheck;
            }
            if (dCcD_GObjInf_ChkTgHit(i_this->mBallSph)) {
                f32 z = *F(0x9C4), x = *F(0x9BC), y = *F(0x9C0);
                wz_tamaNext(i_this, x, y, z);
            }
            goto typecheck;
        }
        if (st == 0x66) {
            gabi::Local<cXyz> pos;
            cpy_u32(gabi::ea(pos.get()), base + 0x314);
            cpy_u32(gabi::ea(pos.get()) + 4, base + 0x318);
            cpy_u32(gabi::ea(pos.get()) + 8, base + 0x31C);
            pos->y = *F(0x654); /* the ground */
            ang->z = 0;
            u8 type = i_this->mBehaviorType;
            ang->x = 0;
            if (type == 10) {
                dPa_control_set(dComIfGp_getParticle(), 0, 0x823C, pos, ang, nullptr, 0xFF, nullptr, -1, nullptr, nullptr, nullptr);
                dPa_control_set(dComIfGp_getParticle(), 0, 0x823D, pos, ang, nullptr, 0xFF, nullptr, -1, nullptr, nullptr, nullptr);
                dPa_control_set(dComIfGp_getParticle(), 0, 0x823A, pos, &i_this->current.angle, nullptr, 0xFF, nullptr, -1, nullptr, nullptr,
                                nullptr);
                if (*T(1) == 0) {
                    *T(1) = (s16)gabi::ftoi(cM_rndF(5.0f));
                    i_this->m510 = (s16)gabi::ftoi(cM_rndF(5.0f) + 8.0f);
                }
                if (!wz_isStartStage(STR(0x10042D1C) /* "sea" */))
                    dKy_arrowcol_chg_on_wz(nullptr, 0);
                i_this->actor_status |= 0x4000;
                *T(0) = 0x50;
                wz_se(i_this, 0x69DF, 0, false);
            } else if (type == 11) {
                /* the summon ball opens a summon door */
                gabi::Local<cXyz> s5;
                s5->y = 5.0f;
                s5->z = 5.0f;
                s5->x = 5.0f;
                cpy_u32(gabi::ea(pos.get()), base + 0x314);
                cpy_u32(gabi::ea(pos.get()) + 4, base + 0x318);
                cpy_u32(gabi::ea(pos.get()) + 8, base + 0x31C);
                int n;
                for (n = 10; n != 0; n--) {
                    s8 room = i_this->current.roomNo;
                    if (fopAcM_createChild(0xD0, fopAcM_GetID(i_this), 0xFFFFFF0C, pos, room, &i_this->current.angle, s5, 0, 0) !=
                        fpcM_ERROR_PROCESS_ID_e)
                        break;
                }
                if (n == 0) {
                    fopAc_ac_c* p = fopAcM_SearchByID(i_this->mParentId);
                    if (p != nullptr)
                        gabi::store<u8>(gabi::ea(p) + 0x470, 0);
                }
                u32 at = wz_ld32(i_this, 0x8F0), tg = wz_ld32(i_this, 0x908);
                wz_st32(i_this, 0x8F0, at & ~1u);
                wz_st32(i_this, 0x908, tg & ~1u);
                dCcD_GObjInf_ClrTgHit(i_this->mBallSph);
                i_this->scale.x = 0.0f;
                i_this->scale.y = 0.0f;
                i_this->scale.z = 0.0f;
                *T(0) = (s16)(wzDbgS(0x80) + 0x1E);
            }
            wz_followEnd2(i_this, 0, 1);
            i_this->m469 = (u8)(i_this->m469 + 1);
            goto typecheck;
        }
        if (st == 0x67) {
            s16 t;
            if (i_this->mBehaviorType == 10) {
                i_this->shape_angle.y = (s16)(i_this->shape_angle.y + 0x100);
                t = *T(0);
                if (t > 10) {
                    if (*T(1) != 0)
                        goto tama67_se;
                    *T(1) = (s16)gabi::ftoi(cM_rndF(5.0f));
                    i_this->m510 = (s16)gabi::ftoi(cM_rndF(5.0f) + 8.0f);
                    t = *T(0);
                    if (t != 0)
                        goto tama67_se;
                    fopAcM_delete(i_this);
                    goto typecheck;
                }
                s16 c = i_this->m510;
                if (c > 0) {
                    i_this->m510 = (s16)(c - 1);
                    t = *T(0);
                }
            } else {
                t = *T(0);
            }
            if (t == 0) {
                fopAcM_delete(i_this);
                goto typecheck;
            }
        tama67_se: {
            u8 type = i_this->mBehaviorType;
            if (type < 10)
                goto post;
            if (type > 10)
                goto post10;
            if (gabi::ea(i_this) + 0x37C == 0)
                goto post10;
            mDoAud_seStart(0x614F, &i_this->eyePos, 0, dComIfGp_getReverb(i_this->current.roomNo));
            goto typecheck;
        }
        }
        if (st == 0x68) {
            u8 type = i_this->mBehaviorType;
            if (type == 10) {
                dPa_control_set(dComIfGp_getParticle(), 0, 0x823A, &i_this->mTamaTarget, &i_this->current.angle, nullptr, 0xFF, nullptr, -1, nullptr,
                                nullptr, nullptr);
            } else if (type == 11) {
                dPa_control_set(dComIfGp_getParticle(), 0, 0x8282, &i_this->mTamaTarget, &i_this->current.angle, nullptr, 0xFF, nullptr, -1, nullptr,
                                nullptr, nullptr);
                fopAc_ac_c* p = fopAcM_SearchByID(i_this->mParentId);
                if (p != nullptr)
                    gabi::store<u8>(gabi::ea(p) + 0x470, 0);
            }
            wz_followEnd2(i_this, 0, 1);
            fopAcM_delete(i_this);
        }
        goto typecheck;
    }
    case 200:
        action_summon_dousa(i_this);
        goto typecheck;
    default:
        goto typecheck;
    }

atari:
    body_atari_check(i_this);
typecheck:
    if (i_this->mBehaviorType >= 10)
        goto post10;
post:
    if (i_this->m468 != 1 && i_this->m468 != 3) {
        cLib_addCalcAngleS2(&i_this->current.angle.y, gabi::load<s16>(base + 0x502), 1, 0x500);
        cLib_addCalcAngleS2(&i_this->shape_angle.y, i_this->current.angle.y, 1, 0x500);
    }
    wz_morfPlay(i_this);
post10: {
    Mtx34* m = calc_mtx();
    mDoMtx_YrotS(m, i_this->current.angle.y);
    mDoMtx_XrotM(calc_mtx(), i_this->current.angle.x);
    gabi::Local<cXyz> v;
    gabi::Local<cXyz> out;
    v->x = 0.0f;
    v->y = 0.0f;
    v->z = i_this->speedF;
    MtxPosition(v, out);
    i_this->speed.x = out->x;
    if (i_this->mBehaviorType >= 10)
        i_this->speed.y = out->y;
    f32 g = i_this->gravity;
    f32 sy = i_this->speed.y + g;
    i_this->speed.z = out->z;
    if (sy < -100.0f)
        sy = -100.0f;
    i_this->speed.y = sy;
}
    fopAcM_posMove(i_this, nullptr);
    i_this->eyePos.copy(i_this->current.pos);
    if (i_this->mBehaviorType < 10) {
        BG_check(i_this);
        if (i_this->m468 != 1 && i_this->m468 != 3 && i_this->m469 != 7)
            fuwafuwa_calc(i_this);
        {
            f32 y = i_this->current.pos.y + 190.0f;
            f32 z = i_this->current.pos.z, x = i_this->current.pos.x;
            *F(0x398) = z;
            *F(0x390) = x;
            *F(0x394) = y;
            f32 add = wzDbgF(0x710) + 120.0f;
            i_this->eyePos.y = i_this->eyePos.y + add;
        }
        gabi::call(0x020182E0, gabi::at<void>(base + 0x8D8), &i_this->current.pos); /* cM3dGCyl::SetC */
        gabi::call(0x02018428, gabi::at<void>(base + 0x8D8), 170.0f);              /* SetH */
        gabi::call(0x020184DC, gabi::at<void>(base + 0x8D8), 50.0f);               /* SetR */
        cCcS_Set_wz(i_this->mBodyCyl);
    } else {
        u8 type = i_this->mBehaviorType;
        if (type != 12 && type != 13) {
            BG_check(i_this);
            gabi::call(0x02018D40, gabi::at<void>(base + 0xA08), &i_this->current.pos); /* cM3dGSph::SetC */
            gabi::call(0x02018C8C, gabi::at<void>(base + 0xA08), (f32)*F(0x528));      /* SetR */
            cCcS_Set_wz(i_this->mBallSph);
            dCcMassS_Mng_Set_wz(i_this->mBallSph, 1);
        }
    }
    draw_SUB(i_this);
    cpy_u32(base + 0x1024, base + 0x314);
    cpy_u32(base + 0x1028, base + 0x318);
    cpy_u32(base + 0x102C, base + 0x31C);
    gabi::store<u16>(base + 0x1030, gabi::load<u16>(base + 0x328));
    gabi::store<u16>(base + 0x1032, gabi::load<u16>(base + 0x32A));
    gabi::store<u16>(base + 0x1034, gabi::load<u16>(base + 0x32C));
    return TRUE;
}
VERIFY(0x024E398C, daWZ_Execute);
