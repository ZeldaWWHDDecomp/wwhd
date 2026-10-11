/* Bokoblin main-unit private HD bindings and prefix layouts.
 * Shared bk_class layout retains the GameCube member names and verified HD deltas. */
#pragma once
#include "d/actor/d_a_bk_local.h"
#include "d/d_path.h"
#include "gc_api/d/d_com_inf_game.h"

// HD: model prefix views and joint-matrix block.
struct bkMainMatrixBlock {
    /* 0x00 */ u8 _00[4];
    /* 0x04 */ be<u16> mFlags;
    /* 0x06 */ u8 _06[0x10 - 6];
    /* 0x10 */ gptr<Mtx34> mpMtx;
};
struct bkMainModelView {
    /* 0x00 */ u8 _00[0x2C];
    /* 0x2C */ gptr<bkMainMatrixBlock> mpMtxBlock;
    /* 0x30 */ u8 _30[0xAC - 0x30];
    /* 0xAC */ be<u32> mModelData;
    /* 0xB0 */ u8 _B0[8];
    /* 0xB8 */ be<u32> mUserArea;
    /* 0xBC */ cXyz mBaseScale;
};
static Mtx34* bk_getAnmMtx(J3DModel* m, s32 jntNo) {
    bkMainMatrixBlock* blk = ((bkMainModelView*)m)->mpMtxBlock;
    blk->mFlags |= 0x10; /* HD: marks the joint matrices dirty */
    return gabi::at<Mtx34>(gabi::ea(blk->mpMtx.get()) + jntNo * 0x30);
}
enum { fpcNm_OBJ_SEARCH_e = 0x42, fpcNm_ITEM_e = 0xFF, dItemNo_DROPPED_SWORD_e = 0x3D };
static inline bool daObj_Search_getFindFlag() {
    return gabi::load<u8>(0x1046BE20) != 0;
} /* daObj_Search::Act_c static */
static inline u8 daItem_getItemNo(void* item) { return gabi::load<u8>(gabi::ea(item) + 0x74E); }
static inline f32 REG7_F(int i) { return REG_F(7, i); }
static inline s16 REG7_S(int i) { return REG_S(7, i); }
static inline f32 REG8_F(int i) { return REG_F(8, i); }

/* dBgS_GndChk (stack object, 0x54): HD layout, this TU's vtables (SHARED-CANDIDATE layout) */
struct bkMainGroundCheck {
    /* 0x00 */ be<u32> mpPolyPassChk; /* -> +0x40 */
    /* 0x04 */ be<u32> mpGrpPassChk;  /* -> +0x4C */
    /* 0x08 */ u8 _08[8];
    /* 0x10 */ be<u32> __vtbl_10;
    /* 0x14 */ u8 _14[0x20 - 0x14];
    /* 0x20 */ be<u32> __vtbl_20;
    /* 0x24 */ cXyz m_pos;
    /* 0x30 */ u8 _30[0x40 - 0x30];
    /* 0x40 */ be<u32> __vtbl_40; /* dBgS_PolyPassChk */
    /* 0x44 */ be<u8> mPass[7];
    /* 0x4B */ u8 _4B;
    /* 0x4C */ be<u32> __vtbl_4C; /* dBgS_GrpPassChk */
    /* 0x50 */ be<u32> mGrp;
};
WWHD_SIZE(bkMainGroundCheck, 0x54);
static void bk_GndChk_ct(bkMainGroundCheck* c) {
    gabi::call(0x02008E0C, c); /* cBgS_GndChk::cBgS_GndChk */
    c->__vtbl_10 = 0x100094BC;
    c->__vtbl_20 = 0x100094CC;
    for (int i = 0; i < 7; i++)
        c->mPass[i] = 0;
    c->__vtbl_4C = 0x100094DC;
    c->__vtbl_40 = 0x100094EC;
    c->mpPolyPassChk = gabi::ea(c) + 0x40;
    c->mpGrpPassChk = gabi::ea(c) + 0x4C;
    c->mGrp = 1;
}
static void bk_GndChk_dt(bkMainGroundCheck* c) {
    c->__vtbl_20 = 0x100094CC;
    c->__vtbl_40 = 0x100094EC;
    c->__vtbl_4C = 0x100094AC;
    gabi::call(0x02008DAC, c, 0); /* cBgS_GndChk::~cBgS_GndChk */
}
static f32 bk_GroundCross(bkMainGroundCheck* c) { return cBgS_GroundCross(dComIfG_Bgsp(), c); }
static inline void MtxRotY(f32 rad, u8 concat) { gabi::call(0x0200FBA4, rad, concat); }

/* dPath / dPnt (d_path) */
using bkMainPathPoint = path_layout::dPnt;
using bkMainPath = path_layout::dPath;
struct bkMainPathFlags {
    be<u8> v[0x100];
};

// HD: emitter prefix fields used by the smoke effects; guest stores preserve null/wrap behavior.
struct bkMainEmitterView {
    u8 mPadding[0x34];
    be<f32> mRate;
    u8 mPadding38[0x58 - 0x38];
    be<f32> mSpread;
    be<s32> mMaxFrame;
    u8 mPadding60[0x220 - 0x60];
    cXyz mGlobalScale;
    u8 mPadding22C[0x238 - 0x22C];
    cXyz mGlobalParticleScale;
};
WWHD_OFFSET(bkMainEmitterView, mRate, 0x34);
WWHD_OFFSET(bkMainEmitterView, mSpread, 0x58);
WWHD_OFFSET(bkMainEmitterView, mMaxFrame, 0x5C);
WWHD_OFFSET(bkMainEmitterView, mGlobalScale, 0x220);
WWHD_OFFSET(bkMainEmitterView, mGlobalParticleScale, 0x238);
WWHD_SIZE(bkMainEmitterView, 0x244);
/* JPABaseEmitter (HD offsets): the fields set inline here */
static inline void JPA_setRate(JPABaseEmitter* e, f32 r) {
    gabi::store<f32>(gabi::ea(e) + offsetof(bkMainEmitterView, mRate), r);
}
static inline void JPA_setSpread(JPABaseEmitter* e, f32 s) {
    gabi::store<f32>(gabi::ea(e) + offsetof(bkMainEmitterView, mSpread), s);
}
static inline void JPA_setMaxFrame(JPABaseEmitter* e, s32 f) {
    gabi::store<s32>(gabi::ea(e) + offsetof(bkMainEmitterView, mMaxFrame), f);
}
/* setGlobalScale: HD sets the global scale (0x220) and the particle scale (0x238) */
static inline void JPA_setGlobalScale(JPABaseEmitter* e, f32 x, f32 y, f32 z) {
    u32 a = gabi::ea(e);
    gabi::store<f32>(a + offsetof(bkMainEmitterView, mGlobalScale), x);
    gabi::store<f32>(a + offsetof(bkMainEmitterView, mGlobalScale) + 4, y);
    gabi::store<f32>(a + offsetof(bkMainEmitterView, mGlobalScale) + 8, z);
    gabi::store<f32>(a + offsetof(bkMainEmitterView, mGlobalParticleScale), x);
    gabi::store<f32>(a + offsetof(bkMainEmitterView, mGlobalParticleScale) + 4, y);
    gabi::store<f32>(a + offsetof(bkMainEmitterView, mGlobalParticleScale) + 8, z);
}
static inline void JPA_setGlobalParticleScale(JPABaseEmitter* e, f32 x, f32 y, f32 z) {
    u32 a = gabi::ea(e);
    gabi::store<f32>(a + offsetof(bkMainEmitterView, mGlobalParticleScale), x);
    gabi::store<f32>(a + offsetof(bkMainEmitterView, mGlobalParticleScale) + 4, y);
    gabi::store<f32>(a + offsetof(bkMainEmitterView, mGlobalParticleScale) + 8, z);
}
static inline s32 dBgS_GetAttributeCode(void* polyInfo) {
    return gabi::call<s32>(0x024EF0F4, dComIfG_Bgsp(), polyInfo);
}
enum { dBgS_Attr_GRASS_e = 4, dBgS_Attr_SAND_e = 0xB };
enum { dPa_ID_AK_JT_ELEMENTSMOKE00 = 0x2022, dPa_ID_AK_JN_ELEMENTKUSA00 = 0x24 };

static inline J3DModel* j3dSys_getModel() {
    return gabi::at<J3DModel>(gabi::load<u32>(0x104B462C));
}
static inline Mtx34* J3DSys_mCurrentMtx() { return gabi::at<Mtx34>(0x104B4868); }
static inline void bk_setAnmMtx(J3DModel* m, s32 jntNo, Mtx34* src) {
    mtx_copy(bk_getAnmMtx(m, jntNo), src);
}

/* ---- daBk_Draw (HD: no blob shadow, no material-table variants; the eye/hat/body shapes are
 * found by material name) ---- */
static inline s16 REG6_S(int i) { return REG_S(6, i); }
static inline s16 REG8_S(int i) { return REG_S(8, i); }
static inline void dMat_ice_entryDL(mDoExt_McaMorf* morf, s32 p, void* p2) {
    gabi::call(0x0259138C, morf, p, p2);
}
/* 027F3F94 (matcher: __nw): self-relative pointer at *this + 0xC (the joint table header; +8 joint
 * count) */
static inline u32 J3DModelData_jointTable(u32 md) { return gabi::call<u32>(0x027F3F94, md); }
static inline u32 J3DModel_modelData(J3DModel* m) { return gabi::load<u32>(gabi::ea(m) + 0xAC); }
/* sead::Buffer-style element access: out-of-range indices give element 0 */
static inline u32 bk_bufAt(u32 base, u32 count, u32 i, u32 size) {
    return i < count ? base + i * size : base;
}

/* J3DModelData: material by name (JUTNameTab::getIndex 027DF9B0), its shape (+8) */
static u32 bk_getMaterialShape(u32 md, const char* name) {
    u32 res = gabi::load<u32>(md);
    gabi::Local<SafeString> key;
    key->__vtbl = SAFESTRING_VTBL;
    key->mStringTop = gabi::guest_addr(name);
    gabi::call_ptr(0x020A8A98, key.get());
    u32 off = gabi::load<u32>(res + 0x18);
    u32 tab = off != 0 ? res + 0x18 + off : 0;
    s32 idx = gabi::call<s32>(0x027DF9B0, tab, key->mStringTop.get());
    u32 mat;
    if (idx >= 0) {
        u32 count = gabi::load<u32>(md + 0xC);
        u32 base = gabi::load<u32>(md + 0x10);
        mat = bk_bufAt(base, count, (u32)idx, 0x39C);
    } else {
        mat = 0;
    }
    return gabi::load<u32>(mat + 8);
}
static inline void J3DShape_show(u32 shape) { gabi::store<u8>(shape + 4, 1); }
static inline void J3DShape_hide(u32 shape) { gabi::store<u8>(shape + 4, 0); }

enum { DSNAP_TYPE_BK = 0xAA };

/* ---- useHeapInit ---- */
static inline void* bk_getRes(s32 idx) { return dComIfG_getObjectRes("Bk", idx, SAFESTRING_VTBL); }
static inline mDoExt_btpAnm* mDoExt_btpAnm_ct(void* p) {
    return gabi::call<mDoExt_btpAnm*>(0x025E7820, p);
}
static inline BOOL mDoExt_btpAnm_init(mDoExt_btpAnm* a, J3DModelData* md, void* btp, BOOL anmPlay,
                                      s32 loopMode, f32 speed, s16 startF, s16 endF, bool modify,
                                      BOOL entry) {
    return gabi::call<BOOL>(0x025E789C, a, md, btp, anmPlay, loopMode, speed, startF, endF, modify,
                            entry);
}
static inline JntHit_c* JntHit_create(J3DModel* m, u32 data, s16 num) {
    return gabi::call<JntHit_c*>(0x02552B60, m, data, num);
}
/* HD resource indices of the Bk archive (HD has separate models instead of material tables) */
enum {
    dRes_HD_BK_BDL_BK_PINK = 0x61,
    dRes_HD_BK_BDL_BK_GREEN = 0x60,
    dRes_HD_BK_BMD_KB_KEN = 0x5F,
    dRes_HD_BK_BMD_KB_BOKO = 0x5E,
};

/* ---- compiler-generated / out-of-line inline copies of this TU ---- */

/* ---- constructors / destructor ---- */
#define BK_AAB_VTBL 0x1000948C /* this TU's cM3dGAab vtable */
static inline void dPa_smokeEcallBack_ct(void* p, u8 a) { gabi::call(0x025A5B18, p, a); }
static inline void dCcD_Sph_ct(dCcD_Sph* s) { gabi::call(0x025166F0, s); }
/* dBgS_ObjAcch::dBgS_ObjAcch (inline): dBgS_Acch, then the vtables and a flag (this TU's copies) */
static inline void dBgS_ObjAcch_ct(dBgS_ObjAcch* a) {
    gabi::call(0x024F0474, a); /* dBgS_Acch::dBgS_Acch */
    u32 p = gabi::ea(a);
    gabi::store<u32>(p + 0x10, 0x100094FC);
    gabi::store<u32>(p + 0x14, 0x1000951C);
    gabi::store<u32>(p + 0x20, 0x1000950C);
    gabi::store<u8>(p + 0x18, 1);
}
static inline void dBgS_ObjAcch_dt(dBgS_ObjAcch* a) {
    u32 p = gabi::ea(a);
    gabi::store<u32>(p + 0x20, 0x1000950C);
    gabi::store<u32>(p + 0x14, 0x1000951C);
    gabi::call(0x024EFD9C, a, 0); /* dBgS_Acch::~dBgS_Acch */
}
/* dBgS_AcchCir::~dBgS_AcchCir (inline): its cM3dGCir member (+0x14) */
static inline void dBgS_AcchCir_dt(dBgS_AcchCir* c) {
    gabi::call(0x02018034, gabi::ea(c) + 0x14, 2);
}
static inline void dCcD_Sph_dt(dCcD_Sph* s, s32 flags) { gabi::call(0x02515AE8, s, flags); }

/* ---- daBk_Create ---- */
/* HD strcmp(dComIfGp_getStartStageName(), name) == 0: SafeString compare (the virtual at vtable
 * +0x14 twice on the literal, once on the stage name at play+0x5134, then at most 0x40001 bytes) */
static inline void bk_SafeString_vcall(SafeString* s) {
    gabi::call_ptr(gabi::load<u32>(s->__vtbl + 0x14), s);
}
static bool bk_isStartStage(const char* name) {
    gabi::Local<SafeString> a;
    a->__vtbl = SAFESTRING_VTBL;
    a->mStringTop = gabi::guest_addr(name);
    u32 stage = dComIfGp_ea() + PLAY_OFF(mCurStage.mName);
    gabi::Local<SafeString> b;
    b->mStringTop = stage;
    b->__vtbl = SAFESTRING_VTBL;
    bk_SafeString_vcall(a);
    bk_SafeString_vcall(a);
    u32 pa = a->mStringTop;
    bk_SafeString_vcall(b);
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
static inline void fopAcM_SetMin(fopAc_ac_c* a, f32 x, f32 y, f32 z) {
    gabi::call(0x025D672C, a, x, y, z);
}
static inline void fopAcM_SetMax(fopAc_ac_c* a, f32 x, f32 y, f32 z) {
    gabi::call(0x025D673C, a, x, y, z);
}
static inline s32 dComIfGp_CharTbl_GetNameIndex(const char* name, s32 p) {
    return gabi::call<s32>(0x0200E814, gabi::at<u8>(dComIfGp_ea() + PLAY_NAMETBL), name, p);
}
static inline be<u32>& ken() { return *gabi::at<be<u32>>(0x1046231C); }
enum { fopAcStts_HD_4000 = 0x4000, fopAcStts_BOSS_e = 0x4000000 };

static const f32 bkHioDefaultsA8[10] = {1.0f, 1.0f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f};
static const f32 bkHioDefaultsD8[10] = {1.0f, 1.0f, 1.0f, 0.8f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f};

// HD: the model uses buffer counts and guest pointers instead of the GameCube node arrays.
struct bkMainModelDataView {
    be<u32> mResource;
    be<u32> mJointCount;
    be<u32> mJoints;
    be<u32> mMaterialCount;
    be<u32> mMaterials;
};
struct bkMainJointView {
    u8 mPadding[4];
    be<u16> mJntNo;
    u8 mPadding6[2];
    be<u32> mCallBack;
    u8 mPaddingC[4];
    be<u32> mMesh;
    u8 mPadding14[8];
};
struct bkMainMaterialView {
    u8 mPadding[4];
    be<u32> mNext;
    be<u32> mShape;
};
struct bkMainJointTableView {
    u8 mPadding[8];
    be<u16> mJointNum;
};
WWHD_OFFSET(bkMainMatrixBlock, mFlags, 4);
WWHD_OFFSET(bkMainMatrixBlock, mpMtx, 0x10);
WWHD_SIZE(bkMainMatrixBlock, 0x14);
WWHD_OFFSET(bkMainModelView, mpMtxBlock, 0x2C);
WWHD_OFFSET(bkMainModelView, mModelData, 0xAC);
WWHD_OFFSET(bkMainModelView, mUserArea, 0xB8);
WWHD_OFFSET(bkMainModelView, mBaseScale, 0xBC);
WWHD_SIZE(bkMainModelView, 0xC8);
WWHD_OFFSET(bkMainModelDataView, mJointCount, 4);
WWHD_OFFSET(bkMainModelDataView, mJoints, 8);
WWHD_SIZE(bkMainModelDataView, 0x14);
WWHD_OFFSET(bkMainJointView, mJntNo, 4);
WWHD_OFFSET(bkMainJointView, mCallBack, 8);
WWHD_OFFSET(bkMainJointView, mMesh, 0x10);
WWHD_SIZE(bkMainJointView, 0x1C);
WWHD_OFFSET(bkMainMaterialView, mNext, 4);
WWHD_OFFSET(bkMainMaterialView, mShape, 8);
WWHD_SIZE(bkMainMaterialView, 0xC);
WWHD_OFFSET(bkMainJointTableView, mJointNum, 8);
WWHD_SIZE(bkMainJointTableView, 0xA);
WWHD_OFFSET(bkMainGroundCheck, m_pos, 0x24);
WWHD_OFFSET(bkMainGroundCheck, mPass, 0x44);
WWHD_OFFSET(bkMainGroundCheck, mGrp, 0x50);
WWHD_SIZE(bkMainPathPoint, 0x10);
WWHD_OFFSET(bkMainPathPoint, m_position, 4);
WWHD_SIZE(bkMainPath, 0xC);
WWHD_OFFSET(bkMainPath, m_points, 8);
WWHD_SIZE(bkMainPathFlags, 0x100);
// HD: guest loads keep null and wrapping address semantics for the prefix views.
static inline u16 bkMain_jointNo(J3DJoint* joint) {
    return gabi::load<u16>(gabi::ea(joint) + offsetof(bkMainJointView, mJntNo));
}
static inline u16 bkMain_jointCount(J3DModel* model) {
    return gabi::load<u16>(J3DModelData_jointTable(J3DModel_modelData(model)) +
                           offsetof(bkMainJointTableView, mJointNum));
}
static inline u32 bkMain_joint(u32 data, u32 index) {
    u32 base = gabi::load<u32>(data + offsetof(bkMainModelDataView, mJoints));
    u32 count = gabi::load<u32>(data + offsetof(bkMainModelDataView, mJointCount));
    return bk_bufAt(base, count, index, sizeof(bkMainJointView));
}
static inline u32 bkMain_rootJoint(u32 data) {
    return gabi::load<u32>(data + offsetof(bkMainModelDataView, mJoints));
}
static inline u32 bkMain_mesh(u32 joint) {
    return gabi::load<u32>(joint + offsetof(bkMainJointView, mMesh));
}
static inline u32 bkMain_shape(u32 material) {
    return gabi::load<u32>(material + offsetof(bkMainMaterialView, mShape));
}
static inline u32 bkMain_nextMaterial(u32 material) {
    return gabi::load<u32>(material + offsetof(bkMainMaterialView, mNext));
}
static inline void bkMain_setCallBack(u32 joint, u32 callback) {
    gabi::store<u32>(joint + offsetof(bkMainJointView, mCallBack), callback);
}
static inline cXyz* bkMain_baseScale(J3DModel* model) {
    return gabi::at<cXyz>(gabi::ea(model) + offsetof(bkMainModelView, mBaseScale));
}
static inline void bkMain_setUserArea(J3DModel* model, bk_class* actor) {
    gabi::store<u32>(gabi::ea(model) + offsetof(bkMainModelView, mUserArea), gabi::ea(actor));
}
static inline void bkMain_entryIn(J3DModel* model, u32 joint) {
    gabi::call(0x027F583C, model, joint);
}
static inline void bkMain_monsSeStart(u32 id, cXyz* pos, u32 pid, u32 param, s32 reverb) {
    gabi::call(0x025E1AA4, id, pos, pid, param, reverb);
}
static inline void bkMain_actorDestructor(bk_class* actor) { gabi::call(0x025D50BC, actor, 0); }
bk_class* bk_class_ct(bk_class*);

struct bkMainSphereSource {
    be<u32> mFlags;
    be<u32> mAtType;
    be<u8> mAtp;
    be<u8> mAtpPadding[3]; // HD: the checked source record includes the padding bytes.
    be<u32> mAtSPrm;
    be<u32> mTgType;
    be<u32> mTgSPrm;
    be<u32> mCoSPrm;
    be<u32> mGObjAtFlags;
    be<u32> mGObjAtSPrm;
    be<u32> mGObjTgFlags;
    be<u32> mGObjTgSPrm;
    be<u32> mGObjCoSPrm;
    be<f32> mCenter[3];
    be<f32> mRadius;
};
struct bkMainCylinderSource {
    be<u32> mFlags;
    be<u32> mAtType;
    be<u8> mAtp;
    be<u8> mAtpPadding[3]; // HD: the checked source record includes the padding bytes.
    be<u32> mAtSPrm;
    be<u32> mTgType;
    be<u32> mTgSPrm;
    be<u32> mCoSPrm;
    be<u32> mGObjAtFlags;
    be<u32> mGObjAtSPrm;
    be<u32> mGObjTgFlags;
    be<u32> mGObjTgSPrm;
    be<u32> mGObjCoSPrm;
    be<f32> mCenter[3];
    be<f32> mRadius;
    be<f32> mHeight;
};
WWHD_OFFSET(bkMainSphereSource, mAtp, 8);
WWHD_OFFSET(bkMainSphereSource, mTgType, 0x10);
WWHD_OFFSET(bkMainSphereSource, mCenter, 0x30);
WWHD_SIZE(bkMainSphereSource, 0x40);
WWHD_OFFSET(bkMainCylinderSource, mCenter, 0x30);
WWHD_OFFSET(bkMainCylinderSource, mHeight, 0x40);
WWHD_SIZE(bkMainCylinderSource, 0x44);
struct bkMainJointHitSource {
    be<s16> mShapeType;
    be<s16> mJointIndex;
    be<f32> mRadius;
    be<u32> mpOffsets;
};
WWHD_OFFSET(bkMainJointHitSource, mpOffsets, 8);
WWHD_SIZE(bkMainJointHitSource, 0xC);
struct bkMainHeaderStatics {
    be<f32> mNegativePi;
    be<f32> mPositivePi;
    u8 mPadding[2];
    u8 mFirstObject;
    u8 mSecondObject;
};
WWHD_OFFSET(bkMainHeaderStatics, mFirstObject, 0xA);
WWHD_OFFSET(bkMainHeaderStatics, mSecondObject, 0xB);
WWHD_SIZE(bkMainHeaderStatics, 0xC);
struct bkMainLineResult {
    u8 mPadding[0x14];
    u8 mPolyInfo[0x1C];
    cXyz mCross;
};
WWHD_OFFSET(bkMainLineResult, mPolyInfo, 0x14);
WWHD_OFFSET(bkMainLineResult, mCross, 0x30);
WWHD_SIZE(bkMainLineResult, 0x3C);
static inline cXyz* bkMain_lineCross(dBgS_LinChk_l* line) {
    return gabi::at<cXyz>(gabi::ea(line) + offsetof(bkMainLineResult, mCross));
}
static inline void* bkMain_polyInfo(dBgS_LinChk_l* line) {
    return gabi::at<u8>(gabi::ea(line) + offsetof(bkMainLineResult, mPolyInfo));
}
static inline f32 bkMain_eyeFrame(mDoExt_btpAnm* anm) {
    return gabi::load<f32>(gabi::ea(anm) + offsetof(mDoExt_btpAnm, mFrameCtrl) +
                           offsetof(J3DFrameCtrl, mFrame));
}
static inline bkMainHeaderStatics& bkMain_headerStatics() {
    return *gabi::at<bkMainHeaderStatics>(0x10462324);
}
static inline be<u32>* bkMain_headerZeros() { return gabi::at<be<u32>>(0x10462358); }
static inline void* bkMain_headerRecord(u32 index) { return gabi::at<u8>(0x10191870 + 12 * index); }
static inline void bkMain_firstHeaderObject(u8* object) { gabi::call(0x028ED6F8, object); }
static inline void bkMain_secondHeaderObject(u8* object) { gabi::call(0x028EAB2C, object); }
static inline u32 bkMain_hioVtable() { return 0x1000956C; }
static inline u32 bkMain_actorVtable() { return 0x1000957C; }
static inline void bkMain_initializeFireLight(enemyfire_l* fire) {
    // HD: LIGHT_INFLUENCE grew by four bytes; its final float is initialized to one.
    gabi::store<f32>(gabi::ea(fire) + offsetof(enemyfire_l, mLight) + 0x20, 1.0f);
}

static inline bk_class* bkMain_actor(J3DModel* model) {
    return gabi::at<bk_class>(((bkMainModelView*)model)->mUserArea);
}
static inline J3DModelData* bkMain_modelData(J3DModel* model) {
    return gabi::at<J3DModelData>(J3DModel_modelData(model));
}
