/**
 * d_a_npc_bj1.cpp (WWHD)
 * NPC - the Koroks of the Forest Haven
 *
 * The GameCube decompilation (zeldaret/tww
 * src/d/actor/d_a_npc_bj1.cpp) has only "Nonmatching" stubs for this actor, so the functions are
 * written from the WWHD code, verified against cking.rpx. Names follow the GameCube symbols;
 * fields without a known meaning are named after their HD offset.
 */
#include "d/actor/d_a_npc_bj1.h"

#define BJ1_SAFESTRING_VTBL 0x10016B84 /* this TU's sead::SafeString vtable */
#define BJ1_ARC STR(0x10016C2C)         /* "Bj" (the Korok archive) */
#define BJ1_ASSERT_FILE STR(0x10016C38)

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* resources by id (HD: sead::SafeString key, per-TU vtable) */
static inline void* bj1_getIDRes(u32 arc, s32 id) {
    gabi::Local<SafeString> key;
    key->__vtbl = BJ1_SAFESTRING_VTBL;
    key->mStringTop = arc;
    return gabi::call<void*>(0x026067F4, dComIfG_resControl(), key.get(), id);
}
static inline void* bj1_getObjectIDRes(s32 id) { return bj1_getIDRes(0x10016C2C /* "Bj" */, id); }
/* J3DModelData (HD): 027F68FC returns the joint name table header (self-relative offset at +0x10) */
static inline u32 J3DModelData_getJointName(J3DModelData* d) {
    u32 h = gabi::call<u32>(0x027F68FC, d);
    u32 off = gabi::load<u32>(h + 0x10);
    return off != 0 ? h + 0x10 + off : 0;
}
static inline s8 JUTNameTab_getIndex(u32 tab, u32 name) { return gabi::call<s8>(0x027DF9B0, tab, name); }
/* HD J3D: a model's joint matrices live in a block at +0x2C (+0x4 flags, +0x10 matrices);
 * every access marks them dirty */
static inline u32 bj1_anmMtx(J3DModel* model, u32 jntNo) {
    u32 blk = gabi::load<u32>(gabi::ea(model) + 0x2C);
    gabi::store<u16>(blk + 4, (u16)(gabi::load<u16>(blk + 4) | 0x10));
    return gabi::load<u32>(blk + 0x10) + jntNo * 0x30;
}
/* matrix assignment through lfs/stfs (all loads, then all stores): the recompiled pair goes through
 * a double, which quiets an SNaN */
static inline void bj1_mtxCopy(u32 dst, u32 src) {
    u32 t[12];
    for (int i = 0; i < 12; i++) t[i] = gabi::load<u32>(src + 4 * i);
    for (int i = 0; i < 12; i++) {
        u32 v = t[i];
        if ((v & 0x7F800000u) == 0x7F800000u && (v & 0x003FFFFFu) != 0 && !(v & 0x00400000u)) v |= 0x00400000u;
        gmem_stf32(dst + 4 * i, v);
    }
}
static inline J3DModel* j3dSys_getModel() { return gabi::at<J3DModel>(gabi::load<u32>(0x104B462C)); }
static const u32 J3DSys_mCurrentMtx = 0x104B4868;
static inline u32 jntNo_of(J3DNode* node) { return gabi::load<u16>(gabi::ea(J3DNode_toJoint(node)) + 4); }
static inline void mDoMtx_XYZrotM(Mtx34* m, s16 x, s16 y, s16 z) { gabi::call(0x025F19F8, m, x, y, z); }
static inline void PSMTXConcat(const Mtx34* a, const Mtx34* b, Mtx34* ab) { gabi::call(0x028E9108, a, b, ab); }

/* ---- file statics ---- */
/* searchActor_Jb results */
static const u32 L_JB_NUM = 0x10465D78;
static const u32 L_JB_ACTOR = 0x1046617C; /* fopAc_ac_c*[20] */

/* 021F2D78 */
static void* searchActor_Jb(void* i_actor, void*) {
    WWHD_FUNC(0x021F2D78, void*, i_actor, (void*)nullptr);
    if (gabi::load<s32>(L_JB_NUM) < 0x14 && fopAc_IsActor(i_actor) && i_actor != nullptr &&
        fpcM_GetName(i_actor) == 0xD5 && gabi::load<u8>(gabi::ea(i_actor) + 0x3D4) != 0) {
        s32 n = gabi::load<s32>(L_JB_NUM);
        gabi::store<s32>(L_JB_NUM, n + 1);
        gabi::store<u32>(L_JB_ACTOR + n * 4, gabi::ea(i_actor));
    }
    return nullptr;
}
VERIFY(0x021F2D78, searchActor_Jb);

/* 021F2E04 */
void daNpc_Bj1_c::nodeBj1Control(J3DNode* node, J3DModel* model) {
    WWHD_FUNC(0x021F2E04, void, this, node, model);
    /* an unused function-local static cXyz (26, 26, 0) */
    if (gabi::load<s32>(0x104661CC) == 0) {
        gabi::store<f32>(0x10465D98, 26.0f);
        gabi::store<f32>(0x10465DA0, 0.0f);
        gabi::store<f32>(0x10465D9C, 26.0f);
        gabi::store<s32>(0x104661CC, 1);
    }
    u32 jnt = jntNo_of(node);
    Mtx34* calc = mDoMtx_stack_c::get();
    PSMTXCopy(gabi::at<Mtx34>(bj1_anmMtx(model, jnt)), calc);
    if (jnt == (u32)(s32)m7E4) {
        mDoMtx_XrotM(calc, m_jnt.mAngles[0][1]);
        mDoMtx_ZrotM(calc, (s16)-m_jnt.mAngles[0][0]);
        mDoMtx_stack_c::scaleM(m900, m904, m908);
        PSMTXMultVec(calc, gabi::at<cXyz>(gabi::ea(this) + 0x8F4), gabi::at<cXyz>(gabi::ea(this) + 0x8D0));
    }
    if (jnt == (u32)(s32)m7E5) {
        mDoMtx_XrotM(calc, m_jnt.mAngles[1][1]);
        mDoMtx_ZrotM(calc, (s16)-m_jnt.mAngles[1][0]);
        mDoMtx_stack_c::scaleM(m90C, m910, m914);
    }
    if (jnt == (u32)(s32)m7E7) {
        PSMTXCopy(calc, gabi::at<Mtx34>(gabi::ea(this) + 0x814));
    }
    PSMTXCopy(calc, gabi::at<Mtx34>(J3DSys_mCurrentMtx));
    bj1_mtxCopy(bj1_anmMtx(model, jnt), gabi::ea(calc));
}
VERIFY(0x021F2E04, &daNpc_Bj1_c::nodeBj1Control);

/* 021F2FD4 */
static BOOL nodeCallBack_Bj1(J3DNode* node, int calcTiming) {
    WWHD_FUNC(0x021F2FD4, BOOL, node, calcTiming);
    if (calcTiming == 0) {
        J3DModel* model = j3dSys_getModel();
        daNpc_Bj1_c* i_this = gabi::at<daNpc_Bj1_c>(gabi::load<u32>(gabi::ea(model) + 0xB8));
        if (i_this != nullptr) {
            i_this->nodeBj1Control(node, model);
        }
    }
    return TRUE;
}
VERIFY(0x021F2FD4, nodeCallBack_Bj1);

/* 021F301C */
void daNpc_Bj1_c::nodePrpControl(J3DNode* node, J3DModel* model) {
    WWHD_FUNC(0x021F301C, void, this, node, model);
    u32 jnt = jntNo_of(node);
    Mtx34* calc = mDoMtx_stack_c::get();
    if (jnt == (u32)(s32)m7FD) {
        PSMTXCopy(gabi::at<Mtx34>(gabi::ea(this) + 0x814), calc);
        mDoMtx_stack_c::transM(-1.5f, 4.5f, -4.2f);
        mDoMtx_XYZrotM(calc, -0x4000, -0x5555, 0);
        PSMTXCopy(calc, gabi::at<Mtx34>(J3DSys_mCurrentMtx));
        bj1_mtxCopy(bj1_anmMtx(model, jnt), gabi::ea(calc));
    }
    if (jnt == (u32)(s32)m7FC) {
        mDoMtx_YrotS(calc, m804);
        u32 blk = gabi::load<u32>(gabi::ea(model) + 0x2C);
        u32 mats = gabi::load<u32>(blk + 0x10);
        gabi::store<u16>(blk + 4, (u16)(gabi::load<u16>(blk + 4) | 0x10));
        PSMTXConcat(gabi::at<Mtx34>(mats + jnt * 0x30), calc, calc);
        PSMTXCopy(calc, gabi::at<Mtx34>(J3DSys_mCurrentMtx));
        bj1_mtxCopy(bj1_anmMtx(model, jnt), gabi::ea(calc));
    }
}
VERIFY(0x021F301C, &daNpc_Bj1_c::nodePrpControl);

/* 021F31FC */
static BOOL nodeCallBack_Prp(J3DNode* node, int calcTiming) {
    WWHD_FUNC(0x021F31FC, BOOL, node, calcTiming);
    if (calcTiming == 0) {
        J3DModel* model = j3dSys_getModel();
        daNpc_Bj1_c* i_this = gabi::at<daNpc_Bj1_c>(gabi::load<u32>(gabi::ea(model) + 0xB8));
        if (i_this != nullptr) {
            i_this->nodePrpControl(node, model);
        }
    }
    return TRUE;
}
VERIFY(0x021F31FC, nodeCallBack_Prp);

/* the McaMorf's virtual deleting destructor (vtable slot +0xC) */
static inline void bj1_morfDelete(mDoExt_McaMorf* m) {
    gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(gabi::ea(m)) + 0xC), m, 3);
}

/* 021F3244 */
J3DModelData* daNpc_Bj1_c::create_Anm() {
    WWHD_FUNC(0x021F3244, J3DModelData*, this);
    u32 t = (u32)(s32)mType;
    J3DModelData* modelData;
    if (t < 3) {
        if (t == 1) {
            modelData = (J3DModelData*)bj1_getObjectIDRes(0x17);
        } else if (t == 2) {
            modelData = (J3DModelData*)bj1_getObjectIDRes(0x18);
        } else {
            modelData = (J3DModelData*)bj1_getObjectIDRes(0x26);
        }
    } else if (t == 3) {
        modelData = (J3DModelData*)bj1_getObjectIDRes(0x17);
    } else if (t < 6) {
        modelData = (J3DModelData*)bj1_getObjectIDRes(0x26);
    } else if (t < 8) {
        modelData = (J3DModelData*)bj1_getObjectIDRes(0x18);
    } else if (t == 8) {
        modelData = (J3DModelData*)bj1_getObjectIDRes(0x17);
    } else {
        modelData = (J3DModelData*)bj1_getObjectIDRes(0x26);
    }
    if (modelData == nullptr) {
        JUT_ASSERT_fail(BJ1_ASSERT_FILE, 0x1119, STR(0x10016C48));
    }
    J3DAnmTransform* anm = (J3DAnmTransform*)bj1_getObjectIDRes(0xA);
    mpMorf = mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, anm, 2, 1.0f, 0, -1, 1, nullptr, 0x80000,
                                    0x11020203);
    mDoExt_McaMorf* morf = mpMorf;
    if (morf == nullptr) {
        return nullptr;
    }
    if (morf->mpModel.get() == nullptr) {
        if (morf != nullptr) bj1_morfDelete(morf);
        mpMorf = nullptr;
        return nullptr;
    }
    m7E4 = JUTNameTab_getIndex(J3DModelData_getJointName(modelData), 0x10016C30);
    if (m7E4 < 0) JUT_ASSERT_fail(BJ1_ASSERT_FILE, 0x1133, STR(0x10016C5C));
    m7E5 = JUTNameTab_getIndex(J3DModelData_getJointName(modelData), 0x10016C70);
    if (m7E5 < 0) JUT_ASSERT_fail(BJ1_ASSERT_FILE, 0x1136, STR(0x10016C7C));
    m7E6 = JUTNameTab_getIndex(J3DModelData_getJointName(modelData), 0x10016C1C);
    if (m7E6 < 0) JUT_ASSERT_fail(BJ1_ASSERT_FILE, 0x1139, STR(0x10016C94));
    m7E7 = JUTNameTab_getIndex(J3DModelData_getJointName(modelData), 0x10016C24);
    if (m7E7 < 0) JUT_ASSERT_fail(BJ1_ASSERT_FILE, 0x113C, STR(0x10016CAC));
    return modelData;
}
VERIFY(0x021F3244, &daNpc_Bj1_c::create_Anm);

/* 021F3564 */
J3DModelData* daNpc_Bj1_c::create_prp_Anm() {
    WWHD_FUNC(0x021F3564, J3DModelData*, this);
    J3DModelData* modelData = (J3DModelData*)bj1_getIDRes(0x10016CCC, 0x29);
    if (modelData == nullptr) {
        JUT_ASSERT_fail(STR(0x10016CD8), 0x1159, STR(0x10016CE8));
    }
    J3DAnmTransform* anm = (J3DAnmTransform*)bj1_getIDRes(0x10016CCC, 0x28);
    mpPrpMorf = mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, anm, 2, 1.0f, 0, -1, 1, nullptr,
                                       0x80000, 0x11000022);
    mDoExt_McaMorf* morf = mpPrpMorf;
    if (morf == nullptr) {
        return nullptr;
    }
    if (morf->mpModel.get() == nullptr) {
        if (morf != nullptr) bj1_morfDelete(morf);
        mpPrpMorf = nullptr;
        return nullptr;
    }
    m7FC = JUTNameTab_getIndex(J3DModelData_getJointName(modelData), 0x10016CD0);
    if (m7FC < 0) JUT_ASSERT_fail(STR(0x10016CD8), 0x116F, STR(0x10016CFC));
    m7FD = JUTNameTab_getIndex(J3DModelData_getJointName(modelData), 0x10016CC4);
    if (m7FD < 0) JUT_ASSERT_fail(STR(0x10016CD8), 0x1172, STR(0x10016D10));
    return modelData;
}
VERIFY(0x021F3564, &daNpc_Bj1_c::create_prp_Anm);

/* item model resource ids per type (-1: none), .data */
static inline s32 bj1_itmTblA(s32 t) { return gabi::load<s32>(0x101BBBC4 + t * 4); }
static inline s32 bj1_itmTblB(s32 t) { return gabi::load<s32>(0x101BBBE8 + t * 4); }
static inline s32 bj1_itmTblC(s32 t) { return gabi::load<s32>(0x101BBC0C + t * 4); }
#define BJ1_ITM_ARC 0x10016D28

/* 021F374C */
BOOL daNpc_Bj1_c::create_itm_Mdl() {
    WWHD_FUNC(0x021F374C, BOOL, this);
    mpMdl7F0 = nullptr;
    mpMdl7E8 = nullptr;
    mpMdl7EC = nullptr;
    s32 id = bj1_itmTblA(mType);
    if (id >= 0) {
        J3DModelData* data = (J3DModelData*)bj1_getIDRes(BJ1_ITM_ARC, id);
        if (data == nullptr) JUT_ASSERT_fail(STR(0x10016D2C), 0x11B2, STR(0x10016D3C));
        mpMdl7EC = mDoExt_J3DModel__create(data, 0x80000, 0x11000022);
        if (mpMdl7EC.get() == nullptr) return FALSE;
        if (bj1_itmTblA(mType) == 0x19) {
            m7F4 = JUTNameTab_getIndex(J3DModelData_getJointName(data), 0x10016D50);
            if (m7F4 < 0) JUT_ASSERT_fail(STR(0x10016D2C), 0x11C0, STR(0x10016D5C));
        }
    }
    id = bj1_itmTblB(mType);
    if (id >= 0) {
        J3DModelData* data = (J3DModelData*)bj1_getIDRes(BJ1_ITM_ARC, id);
        if (data == nullptr) JUT_ASSERT_fail(STR(0x10016D2C), 0x11C7, STR(0x10016D3C));
        mpMdl7F0 = mDoExt_J3DModel__create(data, 0x80000, 0x11000022);
        if (mpMdl7F0.get() == nullptr) return FALSE;
        if (bj1_itmTblB(mType) == 0x1A) {
            m7F5 = JUTNameTab_getIndex(J3DModelData_getJointName(data), 0x10016D74);
            if (m7F5 < 0) JUT_ASSERT_fail(STR(0x10016D2C), 0x11D5, STR(0x10016D80));
        }
    }
    J3DModelData* data = (J3DModelData*)bj1_getIDRes(BJ1_ITM_ARC, bj1_itmTblC(mType));
    if (data == nullptr) JUT_ASSERT_fail(STR(0x10016D2C), 0x11DB, STR(0x10016D3C));
    mpMdl7E8 = mDoExt_J3DModel__create(data, 0x80000, 0x11000022);
    return mpMdl7E8.get() != nullptr;
}
VERIFY(0x021F374C, &daNpc_Bj1_c::create_itm_Mdl);

static inline u16 J3DModelData_getJointNum_l(J3DModelData* d) { return gabi::load<u16>(gabi::call<u32>(0x027F3F94, d) + 8); }
/* morf->getModel()->getModelData()->getJointNodePointer(i)->setCallBack(cb) (HD: bounds-checked) */
static inline void bj1_setJointCallBack(mDoExt_McaMorf* morf, u32 i, u32 cb) {
    u32 d = gabi::load<u32>(gabi::ea(morf->mpModel.get()) + 0xAC);
    u32 count = gabi::load<u32>(d + 4);
    u32 p = gabi::load<u32>(d + 8);
    if (i < count) p += i * 0x1C;
    gabi::store<u32>(p + 8, cb);
}

/* 021F39FC */
BOOL daNpc_Bj1_c::CreateHeap() {
    WWHD_FUNC(0x021F39FC, BOOL, this);
    J3DModelData* modelData = create_Anm();
    if (modelData == nullptr) {
        return FALSE;
    }
    J3DModelData* prpData = create_prp_Anm();
    if (prpData == nullptr) {
        mpMorf = nullptr;
        return FALSE;
    }
    if (!create_itm_Mdl()) {
        mpMorf = nullptr;
        mpPrpMorf = nullptr;
        return FALSE;
    }
    for (u16 i = 0; i < J3DModelData_getJointNum_l(prpData); i++) {
        if (i == (u32)(s32)m7FC || i == (u32)(s32)m7FD) {
            bj1_setJointCallBack(mpPrpMorf, i, 0x021F31FC /* nodeCallBack_Prp */);
        }
    }
    gabi::store<u32>(gabi::ea(mpPrpMorf->mpModel.get()) + 0xB8, gabi::ea(this)); /* setUserArea */
    for (u16 i = 0; i < J3DModelData_getJointNum_l(modelData); i++) {
        if (i == (u32)(s32)m7E4 || i == (u32)(s32)m7E5 || i == (u32)(s32)m7E7) {
            bj1_setJointCallBack(mpMorf, i, 0x021F2FD4 /* nodeCallBack_Bj1 */);
        }
    }
    gabi::store<u32>(gabi::ea(mpMorf->mpModel.get()) + 0xB8, gabi::ea(this)); /* setUserArea */
    mAcchCir.SetWall(30.0f, 40.0f);
    mObjAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, nullptr, nullptr);
    return TRUE;
}
VERIFY(0x021F39FC, &daNpc_Bj1_c::CreateHeap);

/* 021F3C24 CheckCreateHeap */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x021F3C24, BOOL, i_this);
    return static_cast<daNpc_Bj1_c*>(i_this)->CreateHeap();
}
VERIFY(0x021F3C24, CheckCreateHeap);

/* 021F3C28 */
BOOL daNpc_Bj1_c::charDecide(int prm) {
    WWHD_FUNC(0x021F3C28, BOOL, this, prm);
    u32 n = (u32)(s32)(fpcM_GetName(this) - 0x150); /* PROC_NPC_BJ1 .. PROC_NPC_BJ9 */
    mType = -1;
    m9C5 = 0xFF;
    if (n > 8) {
        return FALSE;
    }
    if (n == 0) {
        mType = 0;
        m9C5 = (prm == 1);
        return TRUE;
    }
    if (n == 6) {
        mType = 6;
        m9C5 = 0xC;
        return TRUE;
    }
    mType = (s8)n;
    /* types 1..5, 7, 8: the message set is 2n (or 2n+1 with parameter 1); BJ8/BJ9 shifted by one */
    u8 base = n < 6 ? (u8)(2 * n) : (u8)(2 * n - 1);
    m9C5 = prm == 1 ? (u8)(base + 1) : base;
    return TRUE;
}
VERIFY(0x021F3C28, &daNpc_Bj1_c::charDecide);

/* 021F4544: select a 16-byte animation record from the HD table. */
void daNpc_Bj1_c::setAnm_NUM(int index) {
    WWHD_FUNC(0x021F4544, void, this, index);
    u32 record = 0x101BBC30u + (u32)index * 0x10u;
    gabi::call(0x021F4468, this, gabi::at<anm_prm_c>(record));
}
VERIFY(0x021F4544, &daNpc_Bj1_c::setAnm_NUM);

/* 021F7F40 */
void daNpc_Bj1_c::chg_anmAtr(u8 attribute) {
    WWHD_FUNC(0x021F7F40, void, this, attribute);
    if (attribute < 11 && attribute != m9BD) {
        m9BD = attribute;
        gabi::call(0x021F7F28, this);
    }
}
VERIFY(0x021F7F40, &daNpc_Bj1_c::chg_anmAtr);

/* 021F7F5C */
void daNpc_Bj1_c::control_anmAtr() {
    WWHD_FUNC(0x021F7F5C, void, this);
    u8 attribute = m9BD;
    if ((attribute == 6 || attribute == 7) && m94C != 0) {
        m9BD = 0;
        gabi::call(0x021F4544, this, 0);
    } else if (attribute == 10 && m94C != 0) {
        m9BD = 2;
        gabi::call(0x021F4544, this, 3);
    }
}
VERIFY(0x021F7F5C, &daNpc_Bj1_c::control_anmAtr);

// Proposed ordinary source additions ONLY; root owns adoption and tests.
// Header declaration must change void eInit_prmFloat(f32*, f32) to f32.
f32 daNpc_Bj1_c::eInit_prmFloat(f32* value, f32 fallback) {
    WWHD_FUNC(0x021F5D68, f32, this, value, fallback);
    return value ? gabi::load<f32>(gabi::ea(value)) : fallback;
}
VERIFY(0x021F5D68, &daNpc_Bj1_c::eInit_prmFloat);

void daNpc_Bj1_c::eInit_setEvTimer(int* value) {
    WWHD_FUNC(0x021F597C, void, this, value);
    m940 = 0;
    if (value)
        m940 = (s16)gabi::load<s32>(gabi::ea(value));
}
VERIFY(0x021F597C, &daNpc_Bj1_c::eInit_setEvTimer);

static BOOL daNpc_Bj1_IsDelete(fopAc_ac_c* actor) {
    WWHD_FUNC(0x021F7EE4, BOOL, actor);
    return 1;
}
VERIFY(0x021F7EE4, daNpc_Bj1_IsDelete);

/* 021F7F28: attribute is the full unsigned actor byte. */
void daNpc_Bj1_c::setAnm_ATR() {
    WWHD_FUNC(0x021F7F28, void, this);
    u32 record = 0x101BBF78u + (u32)(u8)m9BD * 0x10u;
    gabi::call(0x021F4468, this, gabi::at<anm_prm_c>(record));
}
VERIFY(0x021F7F28, &daNpc_Bj1_c::setAnm_ATR);

/* 021F7EEC: animation tag is signed before selecting its 16-byte record. */
BOOL daNpc_Bj1_c::setAnm() {
    WWHD_FUNC(0x021F7EEC, BOOL, this);
    s32 tag = (s8)(u8)m9C1;
    u32 record = 0x101BBEE8u + (u32)tag * 0x10u;
    gabi::call(0x021F4468, this, gabi::at<anm_prm_c>(record));
    return 1;
}
VERIFY(0x021F7EEC, &daNpc_Bj1_c::setAnm);

// Proposal only; root owns all stage adoption/testing.
void daNpc_Bj1_c::eInit_CHG_PTH_(int* pathNumber, int* pointNumber) {
    WWHD_FUNC(0x021F5EA4, void, this, pathNumber, pointNumber);
    u32 a = gabi::ea(this);
    gabi::call<void>(0x0259E6D0, gabi::at<void>(a + 0x844),
                     gabi::load<u8>(a + 0x94F),
                     gabi::load<s8>(a + 0x326), 1);
    if (gabi::load<u32>(a + 0x844) && pathNumber) {
        s32 number = gabi::load<s32>(gabi::ea(pathNumber));
        if (number > 0) {
            u32 path = gabi::call<u32>(0x0259E744,
                                      gabi::at<void>(a + 0x844),
                                      gabi::load<s8>(a + 0x326));
            s32 wanted = number - 1;
            bool found = path != 0;
            for (s32 i = 0; found && i < wanted; ++i) {
                path = gabi::call<u32>(0x025AB070, gabi::at<void>(path),
                                       gabi::load<s8>(a + 0x326));
                found = path != 0;
            }
            if (found)
                gabi::call<void>(0x0259E730, gabi::at<void>(a + 0x844),
                                 gabi::at<void>(path));
        }
    }
    if (pointNumber) {
        u8 point = gabi::load<u8>(gabi::ea(pointNumber) + 3);
        u32 count = gabi::call<u32>(0x0259EDB8, gabi::at<void>(a + 0x844));
        if ((u32)point >= count)
            point = (u8)(gabi::call<u32>(0x0259EDB8,
                                       gabi::at<void>(a + 0x844)) - 1u);
        gabi::store<u8>(a + 0x849, point);
    }
}
VERIFY(0x021F5EA4, &daNpc_Bj1_c::eInit_CHG_PTH_);

void daNpc_Bj1_c::eInit_END_MOV_() {
    WWHD_FUNC(0x021F5FBC, void, this);
    gabi::call<void>(0x021F4544, this, 0);
    u32 a = gabi::ea(this);
    f32 value = gabi::load<f32>(0x10016C08);
    gabi::store<u8>(a + 0x9BA, 0);
    gabi::store<f32>(a + 0x91C, value);
    gabi::store<f32>(a + 0x370, value);
    gabi::store<f32>(a + 0x924, value);
}
VERIFY(0x021F5FBC, &daNpc_Bj1_c::eInit_END_MOV_);

void daNpc_Bj1_c::eInit_SET_TNE_() {
    WWHD_FUNC(0x021F6008, void, this);
    gabi::call<void>(0x021F4544, this, 9);
    gabi::store<u8>(gabi::ea(this) + 0x9C3, 0);
}
VERIFY(0x021F6008, &daNpc_Bj1_c::eInit_SET_TNE_);


/* 021F8A14 */
u32 daNpc_Bj1_c::getMsg_BJ4_0() {
    WWHD_FUNC(0x021F8A14, u32, this);
    u32 save = gabi::load<u32>(0x101F84DCu) + 0x644u;
    BOOL enabled = gabi::call<BOOL>(0x025B8B94, gabi::at<void>(save), 0xC08);
    return enabled ? 0x13FFu : 0x13FDu;
}
VERIFY(0x021F8A14, &daNpc_Bj1_c::getMsg_BJ4_0);

/* 021F8A60 */
u32 daNpc_Bj1_c::getMsg_BJ5_0() {
    WWHD_FUNC(0x021F8A60, u32, this);
    u32 save = gabi::load<u32>(0x101F84DCu) + 0x644u;
    BOOL enabled = gabi::call<BOOL>(0x025B8B94, gabi::at<void>(save), 0xC04);
    return enabled ? 0x13F7u : 0x13F5u;
}
VERIFY(0x021F8A60, &daNpc_Bj1_c::getMsg_BJ5_0);

/* 021F8AAC */
u32 daNpc_Bj1_c::getMsg_BJ6_0() {
    WWHD_FUNC(0x021F8AAC, u32, this);
    u32 save = gabi::load<u32>(0x101F84DCu) + 0x644u;
    BOOL enabled = gabi::call<BOOL>(0x025B8B94, gabi::at<void>(save), 0xC02);
    return enabled ? 0x1405u : 0x1402u;
}
VERIFY(0x021F8AAC, &daNpc_Bj1_c::getMsg_BJ6_0);

// Proposal only. Root selects exact Local-frame observer contract before adoption.
void daNpc_Bj1_c::eInit_MOV_(f32* first, f32* second, f32* third, int* timer) {
    WWHD_FUNC(0x021F5D78, void, this, first, second, third, timer);
    u32 a = gabi::ea(this);
    u8 pointIndex = gabi::load<u8>(a + 0x849);
    gabi::call<void>(0x021F4544, this, 8);
    f32 fallback = gabi::load<f32>(0x10016C08);
    gabi::store<u8>(a + 0x9C3, 0);
    f32 value = gabi::call<f32>(0x021F5D68, this, first, fallback);
    gabi::store<f32>(a + 0x92C, value);
    value = gabi::call<f32>(0x021F5D68, this, second, fallback);
    gabi::store<f32>(a + 0x91C, value);
    value = gabi::call<f32>(0x021F5D68, this, third, fallback);
    gabi::store<f32>(a + 0x924, value);
    struct PointFrame { be<u32> linkage[2]; cXyz point; };
    gabi::Local<PointFrame> result;
    u32 out = result.a + 8;
    gabi::call<void>(0x0259E778, gabi::at<void>(a + 0x844),
                     gabi::at<cXyz>(out), pointIndex);
    u32 z = gabi::load<u32>(out + 8);
    u32 x = gabi::load<u32>(out);
    gabi::store<u32>(a + 0x8F0, z);
    u32 y = gabi::load<u32>(out + 4);
    gabi::store<u32>(a + 0x8E8, x);
    gabi::store<u32>(a + 0x8EC, y);
    gabi::call<void>(0x021F597C, this, timer);
    gabi::store<u8>(a + 0x95A, 0);
    gabi::store<u8>(a + 0x9BA, 1);
}
VERIFY(0x021F5D78, &daNpc_Bj1_c::eInit_MOV_);

void daNpc_Bj1_c::eInit_JMP_(f32* speed, f32* gravity) {
    WWHD_FUNC(0x021F5E48, void, this, speed, gravity);
    u32 a = gabi::ea(this);
    f32 fallback = gabi::load<f32>(0x10016C08);
    gabi::store<u8>(a + 0x9B9, 1);
    f32 value = gabi::call<f32>(0x021F5D68, this, speed, fallback);
    gabi::store<f32>(a + 0x340, value);
    value = gabi::call<f32>(0x021F5D68, this, gravity, fallback);
    gabi::store<f32>(a + 0x374, value);
    gabi::call<void>(0x021F4544, this, 4);
}
VERIFY(0x021F5E48, &daNpc_Bj1_c::eInit_JMP_);

void daNpc_Bj1_c::eInit_SET_ANM_(int* animation, f32* speed) {
    WWHD_FUNC(0x021F60AC, void, this, animation, speed);
    if (!animation)
        return;
    s32 number = (s8)gabi::load<u8>(gabi::ea(animation) + 3);
    if ((u32)number >= 11u)
        return;
    gabi::call<void>(0x021F4544, this, number);
    if (speed) {
        u32 morf = gabi::load<u32>(gabi::ea(this) + 0x44C);
        f32 value = gabi::load<f32>(gabi::ea(speed));
        gabi::call<void>(0x025E4A54, gabi::at<void>(morf), value);
    }
}
VERIFY(0x021F60AC, &daNpc_Bj1_c::eInit_SET_ANM_);

/* 021F4344 */
void daNpc_Bj1_c::delPrtcl_danceLR() {
    WWHD_FUNC(0x021F4344, void, this);
    u32 emitter = m99C;
    if (emitter != 0) {
        u32 flags = gabi::load<u32>(emitter + 0x254);
        gabi::store<u32>(emitter + 0x5C, 0xFFFFFFFFu);
        gabi::store<u32>(emitter + 0x254, flags | 1u);
        m99C = 0;
    }
    emitter = m9A0;
    if (emitter != 0) {
        u32 flags = gabi::load<u32>(emitter + 0x254);
        gabi::store<u32>(emitter + 0x5C, 0xFFFFFFFFu);
        gabi::store<u32>(emitter + 0x254, flags | 1u);
        m9A0 = 0;
    }
}
VERIFY(0x021F4344, &daNpc_Bj1_c::delPrtcl_danceLR);

/* 021F8190 */
BOOL daNpc_Bj1_c::chk_partsNotMove() {
    WWHD_FUNC(0x021F8190, BOOL, this);
    u32 actor = gabi::ea(this);
    if ((s16)m932 != gabi::load<s16>(actor + 0x3B2)) return 1;
    return (s16)m930 != gabi::load<s16>(actor + 0x3AE) ? 1 : 0;
}
VERIFY(0x021F8190, &daNpc_Bj1_c::chk_partsNotMove);

/* 021F81C0 */
BOOL daNpc_Bj1_c::getMaskInf(u8* output) {
    WWHD_FUNC(0x021F81C0, BOOL, this, output);
    u32 destination = gabi::ea(output);
    if (destination == 0) return 0;
    u8 mask;
    switch ((s8)(u8)m9C5) {
    case 1: mask = 0x01; break;
    case 3: mask = 0x02; break;
    case 5: mask = 0x04; break;
    case 7: mask = 0x08; break;
    case 9: mask = 0x10; break;
    case 11: mask = 0x20; break;
    case 14: mask = 0x40; break;
    case 16: mask = 0x80; break;
    default: return 0;
    }
    gabi::store<u8>(destination, mask);
    return 1;
}
VERIFY(0x021F81C0, &daNpc_Bj1_c::getMaskInf);

/* 021F8958 */
u32 daNpc_Bj1_c::getMsg_BJ2_0() {
    WWHD_FUNC(0x021F8958, u32, this);
    u32 save = gabi::load<u32>(0x101F84DCu) + 0x644u;
    BOOL enabled = gabi::call<BOOL>(0x025B8B94, gabi::at<void>(save), 0x920);
    return enabled ? 0x13F4u : 0x13F2u;
}
VERIFY(0x021F8958, &daNpc_Bj1_c::getMsg_BJ2_0);

/* 021F89A4 */
u32 daNpc_Bj1_c::getMsg_BJ3_0() {
    WWHD_FUNC(0x021F89A4, u32, this);
    if ((u8)m95B != 0) return 0x13FBu;
    u32 save = gabi::load<u32>(0x101F84DCu) + 0x644u;
    BOOL enabled = gabi::call<BOOL>(0x025B8B94, gabi::at<void>(save), 0xC20);
    return enabled ? 0x13FAu : 0x13F8u;
}
VERIFY(0x021F89A4, &daNpc_Bj1_c::getMsg_BJ3_0);

/* 021F8D48 */
u32 daNpc_Bj1_c::getMsg_BJ9_0() {
    WWHD_FUNC(0x021F8D48, u32, this);
    u32 save = gabi::load<u32>(0x101F84DCu) + 0x644u;
    BOOL enabled = gabi::call<BOOL>(0x025B8B94, gabi::at<void>(save), 0xC10);
    return enabled ? 0x13F0u : 0x13EDu;
}
VERIFY(0x021F8D48, &daNpc_Bj1_c::getMsg_BJ9_0);

/* Each event query reloads the singleton, as the reference does. */
static inline BOOL bj1_eventFlag(u32 flag) {
    u32 save = gabi::load<u32>(0x101F84DCu) + 0x644u;
    return gabi::call<BOOL>(0x025B8B94, gabi::at<void>(save), flag);
}

u32 daNpc_Bj1_c::getMsg_BJ1_0() {
    WWHD_FUNC(0x021F88A4, u32, this);
    if (!bj1_eventFlag(0x604)) return 0x140E;
    s32 stage = gabi::call<s32>(0x0257E5D4);
    u32 save = gabi::load<u32>(0x101F84DCu) + 0x644u;
    if (stage == 1) {
        return gabi::call<BOOL>(0x025B8B94, gabi::at<void>(save), 0xC80) ? 0x1414 : 0x1412;
    }
    return gabi::call<BOOL>(0x025B8B94, gabi::at<void>(save), 0x904) ? 0x1411 : 0x1410;
}
VERIFY(0x021F88A4, &daNpc_Bj1_c::getMsg_BJ1_0);

u32 daNpc_Bj1_c::getMsg_BJ7_0() {
    WWHD_FUNC(0x021F8AF8, u32, this);
    if ((u8)m952 != 0) { m952 = 0; return 0x142B; }
    if ((u8)m953 != 0) { m953 = 0; return 0x142C; }
    u8 state = m94E;
    if (state == 70) return bj1_eventFlag(0x1B80) ? 0x1424 : 0x1422;
    if (state != 255) return 0x1421;
    u32 inventory = gabi::load<u32>(0x101F84DCu) + 0xD4u;
    BOOL hasItem = gabi::call<BOOL>(0x025B7D90, gabi::at<void>(inventory), 2);
    u32 save = gabi::load<u32>(0x101F84DCu) + 0x644u;
    BOOL progressed = gabi::call<BOOL>(0x025B8B94, gabi::at<void>(save), 0xD08);
    if (hasItem) {
        if (!progressed) return 0x1433;
        if (!bj1_eventFlag(0x1C80)) return 0x1431;
    } else if (!progressed) {
        return 0x1416;
    }
    return bj1_eventFlag(0x1B80) ? 0x141E : 0x141F;
}
VERIFY(0x021F8AF8, &daNpc_Bj1_c::getMsg_BJ7_0);

u32 daNpc_Bj1_c::getMsg_BJ8_0() {
    WWHD_FUNC(0x021F8C94, u32, this);
    if (!bj1_eventFlag(0xD40)) return 0x1406;
    s32 stage = gabi::call<s32>(0x0257E5D4);
    u32 save = gabi::load<u32>(0x101F84DCu) + 0x644u;
    if (stage == 7) {
        return gabi::call<BOOL>(0x025B8B94, gabi::at<void>(save), 0xD10) ? 0x140D : 0x140B;
    }
    return gabi::call<BOOL>(0x025B8B94, gabi::at<void>(save), 0xD20) ? 0x140A : 0x1409;
}
VERIFY(0x021F8C94, &daNpc_Bj1_c::getMsg_BJ8_0);

u32 daNpc_Bj1_c::getMsg_Corog() {
    WWHD_FUNC(0x021F8D94, u32, this);
    BOOL first = gabi::call<BOOL>(0x021F82C0, this, 0x9AFF);
    u32 save = gabi::load<u32>(0x101F84DCu) + 0x644u;
    if (first) {
        if (gabi::call<BOOL>(0x025B8B94, gabi::at<void>(save), 0x102)) {
            return gabi::call<BOOL>(0x021F82C0, this, 0x99FF) ? 0x1486 : 0x1485;
        }
        if (!gabi::call<BOOL>(0x021F82C0, this, 0x9EFF)) {
            return gabi::call<BOOL>(0x021F82C0, this, 0x98FF) ? 0x149E : 0x1487;
        }
        if (gabi::call<BOOL>(0x021F82C0, this, 0x96FF)) return 0x1484;
        save = gabi::load<u32>(0x101F84DCu) + 0x644u;
        u32 bits = gabi::call<u32>(0x025B8BB0, gabi::at<void>(save), 0x9EFF);
        u16 count = 0;
        for (int i = 0; i < 8; ++i) {
            count += (bits & 1u) != 0;
            bits = ((bits >> 1) | (bits << 31)) & 0xFFu;
        }
        u32 player = gabi::call<u32>(0x025200D4);
        gabi::store<u16>(player + 0x5BA0, (u16)(8 - count));
        return 0x1482;
    }
    if (gabi::call<BOOL>(0x025B8B94, gabi::at<void>(save), 0x2E10)) {
        return gabi::call<BOOL>(0x021F82C0, this, 0x97FF) ? 0x1494 : 0x1497;
    }
    return gabi::call<BOOL>(0x021F82C0, this, 0x97FF) ? 0x1494 : 0x148A;
}
VERIFY(0x021F8D94, &daNpc_Bj1_c::getMsg_Corog);

// Proposal only; root owns ordinary baseline/adoption.
void daNpc_Bj1_c::eInit_PLYER_MOV_1_() {
    WWHD_FUNC(0x021F5C78, void, this);
    u32 a = gabi::ea(this);
    u32 scene = gabi::call<u32>(0x025200D4);
    u32 player = gabi::load<u32>(scene + 0x5B2C);
    s16 angle = gabi::call<s16>(0x0200F93C, gabi::at<cXyz>(a + 0x8BC),
                               gabi::at<cXyz>(player + 0x314));
    s16 delta = (s16)((s32)angle - (s32)gabi::load<s16>(a + 0x8CA));
    s32 absolute = delta < 0 ? -(s32)delta : (s32)delta;
    struct MoveFrame { be<u32> linkage[2]; cXyz output; cXyz input; };
    gabi::Local<MoveFrame> local;
    u32 output = local.a + 8, input = local.a + 20;
    if (absolute > 4096) {
        f32 x = gabi::load<f32>(a + 0x314);
        f32 y = gabi::load<f32>(a + 0x318);
        f32 z = gabi::load<f32>(a + 0x31C);
        gabi::call<void>(0x028E93CC, gabi::at<Mtx34>(0x1048D0CC), x, y, z);
        s16 yaw = gabi::load<s16>(a + 0x8CA);
        gabi::call<void>(0x025F1C28, gabi::at<Mtx34>(0x1048D0CC), yaw);
        f32 zero = gabi::load<f32>(0x10016C08);
        f32 distance = gabi::load<f32>(0x10016E24);
        gabi::store<f32>(input + 4, zero);
        gabi::store<f32>(input + 8, distance);
        gabi::store<f32>(input, zero);
        gabi::call<void>(0x028E8F64, gabi::at<Mtx34>(0x1048D0CC),
                         gabi::at<cXyz>(input), gabi::at<cXyz>(output));
    } else {
        f32 y = gabi::load<f32>(player + 0x318);
        f32 z = gabi::load<f32>(player + 0x31C);
        f32 x = gabi::load<f32>(player + 0x314);
        gabi::store<f32>(output + 8, z);
        gabi::store<f32>(output + 4, y);
        gabi::store<f32>(output, x);
    }
    scene = gabi::call<u32>(0x025200D4);
    gabi::call<void>(0x02543714, gabi::at<void>(scene + 0x52C4),
                     gabi::at<cXyz>(output));
}
VERIFY(0x021F5C78, &daNpc_Bj1_c::eInit_PLYER_MOV_1_);

u32 daNpc_Bj1_c::getMsg() {
    WWHD_FUNC(0x021F8F8C, u32, this);
    switch ((s8)(u8)m9C5) {
    case 0: return gabi::call<u32>(0x021F88A4, this);
    case 2: return gabi::call<u32>(0x021F8958, this);
    case 4: return gabi::call<u32>(0x021F89A4, this);
    case 6: return gabi::call<u32>(0x021F8A14, this);
    case 8: return gabi::call<u32>(0x021F8A60, this);
    case 10: return gabi::call<u32>(0x021F8AAC, this);
    case 12: return gabi::call<u32>(0x021F8AF8, this);
    case 13: return gabi::call<u32>(0x021F8C94, this);
    case 15: return gabi::call<u32>(0x021F8D48, this);
    case 1: case 3: case 5: case 7: case 9: case 11: case 14: case 16:
        return gabi::call<u32>(0x021F8D94, this);
    default: return 0;
    }
}
VERIFY(0x021F8F8C, &daNpc_Bj1_c::getMsg);

s32 daNpc_Bj1_c::isEventEntry() {
    WWHD_FUNC(0x021F5714, s32, this);
    u32 name = gabi::load<u32>(gabi::ea(this) + 0x84C);
    u32 player = gabi::call<u32>(0x025200D4);
    return gabi::call<s32>(0x02542D88, gabi::at<void>(player + 0x52C4u), name, 0, 0);
}
VERIFY(0x021F5714, &daNpc_Bj1_c::isEventEntry);

void daNpc_Bj1_c::checkOrder() {
    WWHD_FUNC(0x021F5568, void, this);
    u32 actor = gabi::ea(this);
    u16 state = gabi::load<u16>(actor + 0xF8);
    if (state == 2) {
        s16 index = m93C;
        s32 event = gabi::load<s16>(actor + 0x936u + (u32)(s32)index * 2u);
        u32 player = gabi::call<u32>(0x025200D4);
        if (gabi::call<BOOL>(0x0254407C, gabi::at<void>(player + 0x52C4u), event)) {
            if ((s16)m93C == 2) {
                gabi::store<u32>(actor + 0x2E0, gabi::load<u32>(actor + 0x2E0) & 0xFFFFBFFFu);
            }
            m9C0 = 0;
        }
    } else if (state == 1) {
        s8 order = (s8)(u8)m9C0;
        if (order == 1 || order == 2) { m9C0 = 0; m965 = 1; }
    }
}
VERIFY(0x021F5568, &daNpc_Bj1_c::checkOrder);

void daNpc_Bj1_c::eventOrder() {
    WWHD_FUNC(0x021F7828, void, this);
    u32 actor = gabi::ea(this);
    s8 order = (s8)(u8)m9C0;
    if (order == 1 || order == 2) {
        s8 kind = (s8)(u8)m9C5;
        s8 capturedOrder = (s8)(u8)m9C0;
        u16 flags = gabi::load<u16>(actor + 0xFA) | 1u;
        gabi::store<u16>(actor + 0xFA, flags);
        if (kind == 12) gabi::store<u16>(actor + 0xFA, (u16)(flags | 0x20u));
        if (capturedOrder == 1) gabi::call(0x025D76A8, this);
        return;
    }
    if (order < 3) return;
    s16 index = (s16)(order - 3);
    m93C = index;
    s32 event = gabi::load<s16>(actor + 0x936u + (u32)(s32)index * 2u);
    gabi::call(0x025D7A58, this, event, 255, 65535, 0, 1);
}
VERIFY(0x021F7828, &daNpc_Bj1_c::eventOrder);

BOOL daNpc_Bj1_c::setAnm_anm(anm_prm_c* prm) {
    WWHD_FUNC(0x021F4468, BOOL, this, prm);
    s8 current = (s8)(u8)m9BF;
    s8 animation = prm->mAnmNum;
    if (current == animation) return 1;
    m9BF = (u8)animation;
    s32 resource = gabi::call<s32>(0x021F4330, this, (s32)animation);
    s32 loop = prm->mLoopMode;
    void* morph = gabi::at<void>(gabi::load<u32>(gabi::ea(this) + 0x44C));
    f32 speed = prm->mSpeed;
    f32 blend = prm->mMorf;
    gabi::call(0x0259D24C, morph, loop, blend, speed, resource, -1, STR(0x10016DCC));
    if ((s8)(u8)m9BF == 7) {
        gabi::call(0x021F4394, this);
        m998 = 0;
        m9A4 = 30;
        f32 zero = gabi::load<f32>(0x10016C08);
        m94C = 0;
        m94D = 0;
        m918 = zero;
    } else {
        gabi::call(0x021F4344, this);
        f32 zero = gabi::load<f32>(0x10016C08);
        m94D = 0;
        m918 = zero;
        m94C = 0;
    }
    return 1;
}
VERIFY(0x021F4468, &daNpc_Bj1_c::setAnm_anm);

void daNpc_Bj1_c::setAnm_prp(s8 mode) {
    WWHD_FUNC(0x021F70C0, void, this, mode);
    f32 zero = gabi::load<f32>(0x10016C08);
    f32 end = gabi::load<f32>(0x10016EC0);
    if (mode != 1 && mode != 2) {
        __atomic_thread_fence(__ATOMIC_ACQUIRE);
        return;
    }
    gabi::Local<SafeString> key;
    key->__vtbl = 0x10016B84;
    u32 manager = gabi::load<u32>(0x101F4F28);
    key->mStringTop = 0x10016EC4;
    void* resource = gabi::call<void*>(0x026067F4, gabi::at<void>(manager), key.get(), 40);
    if (mode == 1) {
        f32 speed = gabi::load<f32>(0x10016C18);
        void* morph = gabi::at<void>(gabi::load<u32>(gabi::ea(this) + 0x7F8));
        gabi::call(0x025E4A98, morph, resource, 0, zero, speed, zero, end, 0);
    } else {
        void* morph = gabi::at<void>(gabi::load<u32>(gabi::ea(this) + 0x7F8));
        gabi::call(0x025E4A98, morph, resource, 2, zero, end, zero, end, 0);
        f32 frame = gabi::load<f32>(0x10016DD0);
        u32 reloadedMorph = gabi::load<u32>(gabi::ea(this) + 0x7F8);
        gabi::store<f32>(reloadedMorph + 0x9C, frame);
    }
    s32 room = gabi::load<s8>(gabi::ea(this) + 0x326);
    s32 reverb = gabi::call<s32>(0x02520540, room);
    gabi::call(0x025E1A40, mode == 1 ? 0x58C0 : 0x58C1,
               gabi::at<void>(gabi::ea(this) + 0x314), 0, reverb);
    if (mode == 1) m95D = 1;
    __atomic_thread_fence(__ATOMIC_ACQUIRE);
}
VERIFY(0x021F70C0, &daNpc_Bj1_c::setAnm_prp);

struct Bj1InitActionWords { be<u32> first; be<u32> second; };
BOOL daNpc_Bj1_c::init_BJ4_0() {
    WWHD_FUNC(0x021F3EF0, BOOL, this);
    u32 inventory = gabi::load<u32>(0x101F84DC) + 0xD4u;
    if (gabi::call<BOOL>(0x025B7D90, gabi::at<void>(inventory), 2)) return 0;
    u32 save = gabi::load<u32>(0x101F84DC) + 0x644u;
    if (!gabi::call<BOOL>(0x025B8B94, gabi::at<void>(save), 0x1801)) return 0;
    u32 first = gabi::load<u32>(0x10016B40);
    u32 second = gabi::load<u32>(0x10016B44);
    gabi::Local<Bj1InitActionWords> action;
    action->first = first;
    action->second = second;
    gabi::call(0x021F3DC4, this, action.get(), 0);
    return 1;
}
VERIFY(0x021F3EF0, &daNpc_Bj1_c::init_BJ4_0);

// Proposal only; no stage files modified.

BOOL daNpc_Bj1_c::init_BJ6_0() {
    WWHD_FUNC(0x021F3F8C, BOOL, this);
    BOOL ready = gabi::call<u32>(0x025B7D90, gabi::at<void>(gabi::load<u32>(0x101F84DC) + 0xD4), 2) == 0;
    if (ready) {
        // Reload saved-global pointer after the first call.
        ready = gabi::call<u32>(0x025B8B94, gabi::at<void>(gabi::load<u32>(0x101F84DC) + 0x644), 0x1801) != 0;
    }
    if (ready) {
        u32 flags = gabi::load<u32>(gabi::ea(this) + 0x2E0);
        gabi::store<u32>(gabi::ea(this) + 0x2E0, (flags & ~0x80u) | 0x4000u);
        u32 word0 = gabi::load<u32>(0x10016B48);
        u32 word1 = gabi::load<u32>(0x10016B4C);
        struct ActionFrame { be<u32> linkage[2]; be<u32> words[2]; };
        gabi::Local<ActionFrame> frame;
        gabi::store<u32>(frame.a + 8, word0);
        gabi::store<u32>(frame.a + 12, word1);
        (void)gabi::call<BOOL>(0x021F3DC4, this, gabi::at<ProcFunc_l>(frame.a + 8), (void*)nullptr);
    }
    return ready; // Normalized gate result; independent of setter return.
}
VERIFY(0x021F3F8C, &daNpc_Bj1_c::init_BJ6_0);

BOOL daNpc_Bj1_c::init_BJX_0() {
    WWHD_FUNC(0x021F4038, BOOL, this);
    BOOL ready = gabi::call<u32>(0x025B7D90, gabi::at<void>(gabi::load<u32>(0x101F84DC) + 0xD4), 2) == 0;
    if (ready) {
        // Reload saved-global pointer after the first call.
        ready = gabi::call<u32>(0x025B8B94, gabi::at<void>(gabi::load<u32>(0x101F84DC) + 0x644), 0x1801) != 0;
    }
    if (ready) {
        u32 word0 = gabi::load<u32>(0x10016B58);
        u32 word1 = gabi::load<u32>(0x10016B5C);
        struct ActionFrame { be<u32> linkage[2]; be<u32> words[2]; };
        gabi::Local<ActionFrame> frame;
        gabi::store<u32>(frame.a + 8, word0);
        gabi::store<u32>(frame.a + 12, word1);
        (void)gabi::call<BOOL>(0x021F3DC4, this, gabi::at<ProcFunc_l>(frame.a + 8), (void*)nullptr);
    }
    return ready; // Normalized gate result; independent of setter return.
}
VERIFY(0x021F4038, &daNpc_Bj1_c::init_BJX_0);

BOOL daNpc_Bj1_c::init_BJX_1() {
    WWHD_FUNC(0x021F40D4, BOOL, this);
    BOOL ready = gabi::call<u32>(0x025B7D90, gabi::at<void>(gabi::load<u32>(0x101F84DC) + 0xD4), 2) != 0;
    if (ready) {
        u32 word0 = gabi::load<u32>(0x10016B58);
        u32 word1 = gabi::load<u32>(0x10016B5C);
        struct ActionFrame { be<u32> linkage[2]; be<u32> words[2]; };
        gabi::Local<ActionFrame> frame;
        gabi::store<u32>(frame.a + 8, word0);
        gabi::store<u32>(frame.a + 12, word1);
        (void)gabi::call<BOOL>(0x021F3DC4, this, gabi::at<ProcFunc_l>(frame.a + 8), (void*)nullptr);
    }
    return ready; // Normalized gate result; independent of setter return.
}
VERIFY(0x021F40D4, &daNpc_Bj1_c::init_BJX_1);

BOOL daNpc_Bj1_c::init_BJ7_0() {
    WWHD_FUNC(0x021F42B0, BOOL, this);
    BOOL ready = gabi::call<u32>(0x025B8B94, gabi::at<void>(gabi::load<u32>(0x101F84DC) + 0x644), 0x1801) != 0;
    if (ready) {
        gabi::call<void>(0x021F414C, this); // Before either descriptor read.
        u32 word0 = gabi::load<u32>(0x10016B50);
        u32 word1 = gabi::load<u32>(0x10016B54);
        struct ActionFrame { be<u32> linkage[2]; be<u32> words[2]; };
        gabi::Local<ActionFrame> frame;
        gabi::store<u32>(frame.a + 8, word0);
        gabi::store<u32>(frame.a + 12, word1);
        (void)gabi::call<BOOL>(0x021F3DC4, this, gabi::at<ProcFunc_l>(frame.a + 8), (void*)nullptr);
    }
    return ready; // Normalized gate result; independent of setter return.
}
VERIFY(0x021F42B0, &daNpc_Bj1_c::init_BJ7_0);

/* Exact GHS member descriptor dispatch, including slot-zero virtual dispatch.
 * set_action does not insert a null guard for its newly installed descriptor. */
static void bj1_invokeAction(u32 descriptor, u32 actor, void* argument) {
    s16 index = gabi::load<s16>(descriptor + 2);
    s16 adjustment = gabi::load<s16>(descriptor);
    u32 adjusted = actor + (u32)(s32)adjustment;
    u32 function;
    if (index < 0) {
        function = gabi::load<u32>(descriptor + 4);
    } else {
        s16 vtableOffset = gabi::load<s16>(descriptor + 6);
        u32 table = gabi::load<u32>(adjusted + (u32)(s32)vtableOffset);
        function = gabi::load<u32>(table + (u32)(s32)index * 8u + 4u);
    }
    gabi::call_ptr(function, gabi::at<void>(adjusted), argument);
}

BOOL daNpc_Bj1_c::set_action(ProcFunc_l* incoming, void* argument) {
    WWHD_FUNC(0x021F3DC4, BOOL, this, incoming, argument);
    u32 actor = gabi::ea(this), current = actor + 0x80Cu, source = gabi::ea(incoming);
    s16 oldIndex = gabi::load<s16>(current + 2);
    s16 newIndex = gabi::load<s16>(source + 2);
    s16 newAdjustment;
    u32 newFunction;
    if (oldIndex == newIndex) {
        if (oldIndex == 0) return 1;
        newAdjustment = gabi::load<s16>(source);
        s16 oldAdjustment = gabi::load<s16>(current);
        newFunction = gabi::load<u32>(source + 4);
        if (oldAdjustment == newAdjustment && gabi::load<u32>(current + 4) == newFunction) return 1;
    } else {
        newFunction = gabi::load<u32>(source + 4);
        newAdjustment = gabi::load<s16>(source);
    }
    if (oldIndex != 0) {
        m9C6 = 9;
        bj1_invokeAction(current, actor, argument);
    }
    // Incoming values remain captured across the old callback.
    gabi::store<u32>(current + 4, newFunction);
    gabi::store<s16>(current, newAdjustment);
    gabi::store<s16>(current + 2, newIndex);
    m9C6 = 0;
    bj1_invokeAction(current, actor, argument);
    return 1;
}
VERIFY(0x021F3DC4, &daNpc_Bj1_c::set_action);

void daNpc_Bj1_c::setStt(s8 state) {
    WWHD_FUNC(0x021F919C, void, this, state);
    u32 actor = gabi::ea(this);
    s8 previous = (s8)(u8)m9C1;
    f32 zero = gabi::load<f32>(0x10016C08);
    m942 = 0;
    m9C1 = (u8)state;
    switch ((u32)(s32)state) {
    case 2:
        if ((s8)(u8)m9C5 == 4) {
            u32 target = gabi::call<u32>(0x021F58D8, this, gabi::load<u32>(actor + 0x8B8));
            if (target != 0) m95B = gabi::load<u8>(target + 0x3D6) != 0;
        }
        m9C2 = (u8)previous;
        m9BD = 255;
        gabi::store<u8>(actor + 0x3B6, 1);
        m9C3 = 1;
        return;
    case 3: {
        u32 table = 0x10465DA4u + (u32)(s32)(s8)mType * 108u;
        m9C3 = 0;
        m91C = gabi::load<f32>(table + 0x44);
        f32 next = gabi::load<f32>(table + 0x48);
        m959 = 0;
        m924 = next;
        m9B8 = 6;
        gabi::store<f32>(actor + 0x374, zero);
        gabi::call(0x021F70C0, this, 1);
        m95C = 1;
        break;
    }
    case 4:
        m91C = zero;
        m9C3 = 1;
        m966 = 1;
        break;
    case 5:
        m944 = 90;
        break;
    case 6: {
        u32 table = 0x10465DA4u + (u32)(s32)(s8)mType * 108u;
        m91C = gabi::load<f32>(table + 0x68);
        m924 = gabi::load<f32>(table + 0x6C);
        f32 next = gabi::load<f32>(table + 0x70);
        m9BA = 1;
        m92C = next;
        break;
    }
    default: break;
    }
    gabi::call(0x021F7EEC, this);
}
VERIFY(0x021F919C, &daNpc_Bj1_c::setStt);

BOOL daNpc_Bj1_c::wait_1() {
    WWHD_FUNC(0x021F9358, BOOL, this);
    u32 actor = gabi::ea(this);
    s8 order = (s8)(u8)m9C0;
    if (order == 1 || order >= 3) return 1;
    if ((u8)m965 != 0) {
        if (gabi::call<BOOL>(0x021F8078, this)) gabi::call(0x021F919C, this, 2);
        return 1;
    }
    u8 reset = m964;
    m9C0 = 2;
    if (reset != 0) {
        m942 = 60;
    }
    BOOL timer = gabi::call<BOOL>(0x02055B64, gabi::at<void>(actor + 0x942));
    if (timer) {
        m9C3 = 1;
        m966 = 0;
        return 1;
    }
    s16 angle = gabi::load<s16>(actor + 0x8CA);
    m9C3 = 3;
    m94A = angle;
    gabi::store<u8>(actor + 0x3B6, 1);
    return 1;
}
VERIFY(0x021F9358, &daNpc_Bj1_c::wait_1);

BOOL daNpc_Bj1_c::wait_2() {
    WWHD_FUNC(0x021F9424, BOOL, this);
    u32 actor = gabi::ea(this);
    s8 order = (s8)(u8)m9C0;
    if (order == 1 || order >= 3) return 1;
    if ((u8)m965 != 0) {
        if (gabi::call<BOOL>(0x021F8078, this)) gabi::call(0x021F919C, this, 2);
        return 1;
    }
    u8 reset = m964;
    m9C0 = 2;
    if (reset != 0) {
        m942 = 60;
        m944 = 90;
    }
    BOOL timer = gabi::call<BOOL>(0x02055B64, gabi::at<void>(actor + 0x942));
    if (timer) {
        m9C3 = 1;
        m966 = 0;
        return 1;
    }
    if (!gabi::call<BOOL>(0x02055B64, gabi::at<void>(actor + 0x944))) gabi::call(0x021F919C, this, 3);
    return 1;
}
VERIFY(0x021F9424, &daNpc_Bj1_c::wait_2);

BOOL daNpc_Bj1_c::wait_3() {
    WWHD_FUNC(0x021F94FC, BOOL, this);
    u32 actor = gabi::ea(this);
    u32 table = 0x10465DB0u + (u32)(s32)(s8)mType * 108u;
    s32 target = gabi::load<s16>(actor + 0x8CA);
    s32 step = gabi::load<s16>(table + 0x52);
    s32 maximum = gabi::load<s16>(table + 0x54);
    gabi::call(0x0200F378, gabi::at<void>(actor + 0x322), target, step, maximum, 0);
    s8 order = (s8)(u8)m9C0;
    if (order == 1 || order >= 3) return 1;
    if ((u8)m965 != 0) {
        if (gabi::call<BOOL>(0x021F8078, this)) gabi::call(0x021F919C, this, 2);
        return 1;
    }
    u8 reset = m964;
    m9C0 = 2;
    if (reset != 0) {
        m942 = 60;
    }
    BOOL timer = gabi::call<BOOL>(0x02055B64, gabi::at<void>(actor + 0x942));
    if (timer) {
        f32 distance = gabi::load<f32>(0x10016EE4);
        if (gabi::call<BOOL>(0x021F80F8, this, distance)) {
            m9C3 = 1;
            m966 = 0;
            return 1;
        }
    }
    s16 angle = gabi::load<s16>(actor + 0x8CA);
    m9C3 = 3;
    m94A = angle;
    gabi::store<u8>(actor + 0x3B6, 1);
    return 1;
}
VERIFY(0x021F94FC, &daNpc_Bj1_c::wait_3);

BOOL daNpc_Bj1_c::wait_4() {
    WWHD_FUNC(0x021F960C, BOOL, this);
    u32 actor = gabi::ea(this);
    s8 order = (s8)(u8)m9C0;
    if (order == 1 || order >= 3) return 1;
    if ((u8)m965 != 0) {
        if (gabi::call<BOOL>(0x021F8078, this)) gabi::call(0x021F919C, this, 2);
        return 1;
    }
    u32 save = gabi::load<u32>(0x101F84DCu) + 0x644u;
    if (!gabi::call<BOOL>(0x025B8B94, gabi::at<void>(save), 0x2902)) {
        u32 inventory = gabi::load<u32>(0x101F84DCu) + 0x71u;
        if (gabi::call<BOOL>(0x025B5D40, gabi::at<void>(inventory), 6, 0)) {
            m9C0 = 5;
            return 1;
        }
    }
    u8 reset = m964;
    m9C0 = 2;
    if (reset != 0) {
        m942 = 60;
    }
    BOOL timer = gabi::call<BOOL>(0x02055B64, gabi::at<void>(actor + 0x942));
    if (timer) {
        m9C3 = 1;
        m966 = 0;
        return 1;
    }
    s16 angle = gabi::load<s16>(actor + 0x8CA);
    m9C3 = 3;
    m94A = angle;
    gabi::store<u8>(actor + 0x3B6, 1);
    return 1;
}
VERIFY(0x021F960C, &daNpc_Bj1_c::wait_4);

// Proposal only; root is sole source writer.

void daNpc_Bj1_c::setPrtcl_drugPot_1() {
    WWHD_FUNC(0x021F414C, void, this);
    u32 actor = gabi::ea(this);
    struct QueryFrame { be<u32> linkage[2]; be<u32> args[4]; be<s16> angle; be<u16> pad; be<f32> x; be<f32> z; };
    gabi::Local<QueryFrame> frame;
    s8 room = gabi::load<s8>(actor + 0x326);
    if (gabi::call<s32>(0x02520630, room, gabi::at<f32>(frame.a + 28),
                        gabi::at<f32>(frame.a + 32), gabi::at<s16>(frame.a + 24)) == 0) return;
    f32 z = gabi::load<f32>(frame.a + 32);
    s16 angle = gabi::load<s16>(frame.a + 24);
    gabi::store<f32>(actor + 0x978, z);
    f32 x = gabi::load<f32>(frame.a + 28);
    gabi::store<s16>(actor + 0x96A, angle);
    gabi::store<f32>(actor + 0x970, x);
    f32 zero = gabi::load<f32>(0x10016C08);
    gabi::store<s16>(actor + 0x968, 0);
    gabi::store<s16>(actor + 0x96C, 0);
    room = gabi::load<s8>(actor + 0x326);
    gabi::store<f32>(actor + 0x974, zero);
    {
        u32 scene = gabi::call<u32>(0x025200D4);
        u32 control = gabi::load<u32>(scene + 0x5AB0);
        u32 emitter = gabi::call<u32>(0x025A847C, gabi::at<void>(control), 0, 0x8176,
                         gabi::at<cXyz>(actor + 0x970), gabi::at<csXyz>(actor + 0x968),
                         (void*)nullptr, 0xFF, (void*)nullptr, room,
                         (void*)nullptr, (void*)nullptr, (void*)nullptr);
        room = gabi::load<s8>(actor + 0x326); // Before storing preceding emitter.
        gabi::store<u32>(actor + 0x98C, emitter);
    }
    {
        u32 scene = gabi::call<u32>(0x025200D4);
        u32 control = gabi::load<u32>(scene + 0x5AB0);
        u32 emitter = gabi::call<u32>(0x025A847C, gabi::at<void>(control), 0, 0x8177,
                         gabi::at<cXyz>(actor + 0x970), gabi::at<csXyz>(actor + 0x968),
                         (void*)nullptr, 0xFF, (void*)nullptr, room,
                         (void*)nullptr, (void*)nullptr, (void*)nullptr);
        room = gabi::load<s8>(actor + 0x326); // Before storing preceding emitter.
        gabi::store<u32>(actor + 0x990, emitter);
    }
    {
        u32 scene = gabi::call<u32>(0x025200D4);
        u32 control = gabi::load<u32>(scene + 0x5AB0);
        u32 emitter = gabi::call<u32>(0x025A847C, gabi::at<void>(control), 0, 0x8178,
                         gabi::at<cXyz>(actor + 0x970), gabi::at<csXyz>(actor + 0x968),
                         (void*)nullptr, 0xFF, (void*)nullptr, room,
                         (void*)nullptr, (void*)nullptr, (void*)nullptr);
        gabi::store<u32>(actor + 0x994, emitter);
    }
}
VERIFY(0x021F414C, &daNpc_Bj1_c::setPrtcl_drugPot_1);

void daNpc_Bj1_c::setPrtcl_drugPot_2() {
    WWHD_FUNC(0x021F4558, void, this);
    u32 actor = gabi::ea(this);
    struct QueryFrame { be<u32> linkage[2]; be<u32> args[4]; be<s16> angle; be<u16> pad; be<f32> x; be<f32> z; };
    gabi::Local<QueryFrame> frame;
    s8 room = gabi::load<s8>(actor + 0x326);
    if (gabi::call<s32>(0x02520630, room, gabi::at<f32>(frame.a + 28),
                        gabi::at<f32>(frame.a + 32), gabi::at<s16>(frame.a + 24)) == 0) return;
    f32 z = gabi::load<f32>(frame.a + 32);
    s16 angle = gabi::load<s16>(frame.a + 24);
    gabi::store<f32>(actor + 0x978, z);
    f32 x = gabi::load<f32>(frame.a + 28);
    gabi::store<s16>(actor + 0x96A, angle);
    gabi::store<f32>(actor + 0x970, x);
    f32 zero = gabi::load<f32>(0x10016C08);
    gabi::store<s16>(actor + 0x968, 0);
    gabi::store<s16>(actor + 0x96C, 0);
    room = gabi::load<s8>(actor + 0x326);
    gabi::store<f32>(actor + 0x974, zero);
    {
        u32 scene = gabi::call<u32>(0x025200D4);
        u32 control = gabi::load<u32>(scene + 0x5AB0);
        u32 emitter = gabi::call<u32>(0x025A847C, gabi::at<void>(control), 0, 0x822D,
                         gabi::at<cXyz>(actor + 0x970), gabi::at<csXyz>(actor + 0x968),
                         (void*)nullptr, 0xFF, (void*)nullptr, room,
                         (void*)nullptr, (void*)nullptr, (void*)nullptr);
        gabi::store<u32>(actor + 0x998, emitter);
    }
}
VERIFY(0x021F4558, &daNpc_Bj1_c::setPrtcl_drugPot_2);

void daNpc_Bj1_c::setPrtcl_danceLR() {
    WWHD_FUNC(0x021F4394, void, this);
    u32 actor = gabi::ea(this);
    gabi::call<void>(0x021F4344, this);
    s8 room = gabi::load<s8>(actor + 0x326);
    {
        u32 scene = gabi::call<u32>(0x025200D4);
        u32 control = gabi::load<u32>(scene + 0x5AB0);
        u32 emitter = gabi::call<u32>(0x025A847C, gabi::at<void>(control), 0, 0x8223,
                         gabi::at<cXyz>(actor + 0x314), (void*)nullptr,
                         (void*)nullptr, 0xFF, (void*)nullptr, room,
                         (void*)nullptr, (void*)nullptr, (void*)nullptr);
        room = gabi::load<s8>(actor + 0x326); // Before storing first emitter.
        gabi::store<u32>(actor + 0x99C, emitter);
    }
    {
        u32 scene = gabi::call<u32>(0x025200D4);
        u32 control = gabi::load<u32>(scene + 0x5AB0);
        u32 emitter = gabi::call<u32>(0x025A847C, gabi::at<void>(control), 0, 0x8224,
                         gabi::at<cXyz>(actor + 0x314), (void*)nullptr,
                         (void*)nullptr, 0xFF, (void*)nullptr, room,
                         (void*)nullptr, (void*)nullptr, (void*)nullptr);
        gabi::store<u32>(actor + 0x9A0, emitter);
    }
}
VERIFY(0x021F4394, &daNpc_Bj1_c::setPrtcl_danceLR);

void daNpc_Bj1_c::flwPrtcl_danceLR() {
    WWHD_FUNC(0x021F4864, void, this);
    u32 actor = gabi::ea(this);
    {
        u32 emitter = gabi::load<u32>(actor + 0x99C);
        if (emitter != 0) {
            u32 morf = gabi::load<u32>(actor + 0x44C);
            s8 joint = gabi::load<s8>(actor + 0x7E6);
            u32 model = gabi::load<u32>(morf + 0x90);
            u32 block = gabi::load<u32>(model + 0x2C);
            u32 matrices = gabi::load<u32>(block + 0x10);
            u16 flags = gabi::load<u16>(block + 4);
            u32 matrix = matrices + (u32)((s32)joint * 48);
            gabi::store<u16>(block + 4, (u16)(flags | 0x10));
            gabi::call<void>(0x028249B0, gabi::at<Mtx34>(matrix),
                             gabi::at<cXyz>(emitter + 0x1F0), gabi::at<cXyz>(emitter + 0x22C));
        }
    }
    {
        u32 emitter = gabi::load<u32>(actor + 0x9A0);
        if (emitter != 0) {
            u32 morf = gabi::load<u32>(actor + 0x44C);
            s8 joint = gabi::load<s8>(actor + 0x7E7);
            u32 model = gabi::load<u32>(morf + 0x90);
            u32 block = gabi::load<u32>(model + 0x2C);
            u32 matrices = gabi::load<u32>(block + 0x10);
            u16 flags = gabi::load<u16>(block + 4);
            u32 matrix = matrices + (u32)((s32)joint * 48);
            gabi::store<u16>(block + 4, (u16)(flags | 0x10));
            gabi::call<void>(0x028249B0, gabi::at<Mtx34>(matrix),
                             gabi::at<cXyz>(emitter + 0x1F0), gabi::at<cXyz>(emitter + 0x22C));
        }
    }
}
VERIFY(0x021F4864, &daNpc_Bj1_c::flwPrtcl_danceLR);

// Proposal only; root owns adoption/tests.
BOOL daNpc_Bj1_c::chkReg(u16 reg) {
    WWHD_FUNC(0x021F82C0, BOOL, this, reg);
    struct MaskFrame { be<u32> linkage[2]; u8 mask; };
    gabi::Local<MaskFrame> frame;
    if (gabi::call<BOOL>(0x021F81C0, this, gabi::at<u8>(frame.a + 8)) == 0) return 0;
    u32 saved = gabi::load<u32>(0x101F84DC);
    u32 bits = gabi::call<u32>(0x025B8BB0, gabi::at<void>(saved + 0x644), reg);
    u8 mask = gabi::load<u8>(frame.a + 8); // After the getter call.
    return (bits & mask) != 0;
}
VERIFY(0x021F82C0, &daNpc_Bj1_c::chkReg);

void daNpc_Bj1_c::setReg(u16 reg) {
    WWHD_FUNC(0x021F8314, void, this, reg);
    struct MaskFrame { be<u32> linkage[2]; u8 mask; };
    gabi::Local<MaskFrame> frame;
    if (gabi::call<BOOL>(0x021F81C0, this, gabi::at<u8>(frame.a + 8)) == 0) return;
    u32 saved = gabi::load<u32>(0x101F84DC);
    u32 bits = gabi::call<u32>(0x025B8BB0, gabi::at<void>(saved + 0x644), reg);
    saved = gabi::load<u32>(0x101F84DC); // Reload before reading private mask.
    u8 mask = gabi::load<u8>(frame.a + 8);
    gabi::call<void>(0x025B8AF4, gabi::at<void>(saved + 0x644), reg, bits | mask);
}
VERIFY(0x021F8314, &daNpc_Bj1_c::setReg);

// Only selected bounded function; root owns stage adoption/tests.
BOOL daNpc_Bj1_c::chk_talk() {
    WWHD_FUNC(0x021F8078, BOOL, this);
    u32 actor = gabi::ea(this);
    gabi::store<u8>(actor + 0x94E, 0xFF);
    u32 scene = gabi::call<u32>(0x025200D4);
    u32 eventState = gabi::load<u8>(scene + 0x52B0);
    if (eventState - 1u > 3u) return 1;
    scene = gabi::call<u32>(0x025200D4);
    if (gabi::call<u32>(0x02544950, gabi::at<void>(scene + 0x52C4)) == 0) return 0;
    scene = gabi::call<u32>(0x025200D4);
    u8 talkState = gabi::load<u8>(scene + 0x52B1);
    gabi::store<u8>(actor + 0x94E, talkState);
    return 1;
}
VERIFY(0x021F8078, &daNpc_Bj1_c::chk_talk);

// Ordinary source proposal only; root owns adoption and controls.
BOOL daNpc_Bj1_c::createInit() {
    WWHD_FUNC(0x021F4D48, BOOL, this);
    u32 a = gabi::ea(this);
    for (u32 i = 0; i < 3; ++i) {
        u32 name = gabi::load<u32>(0x101BBBB8 + 4 * i);
        u32 scene = gabi::call<u32>(0x025200D4);
        u32 result = gabi::call<u32>(0x02543F10, gabi::at<void>(scene + 0x52C4), name, 0xFF);
        gabi::store<u16>(a + 0x936 + 2 * i, (u16)result);
    }
    s8 type = gabi::load<s8>(a + 0x9C4);
    gabi::store<u32>(a + 0x39C, 10);
    if ((u32)(s32)type > 8) return 0;
    u32 path;
    switch (type) {
    case 0: case 1: case 7: {
        s8 currentType = gabi::load<s8>(a + 0x9C4);
        u32 params = gabi::load<u32>(a + 0xB0);
        gabi::store<u8>(a + 0x389, 0xAB);
        gabi::store<u8>(a + 0x38B, 0xAB);
        u32 row = 0x101BBCF0 + (u32)((s32)currentType * 36);
        gabi::store<f32>(a + 0x900, gabi::load<f32>(row + 0));
        gabi::store<f32>(a + 0x904, gabi::load<f32>(row + 4));
        gabi::store<f32>(a + 0x908, gabi::load<f32>(row + 8));
        f32 row3 = gabi::load<f32>(row + 12);
        gabi::store<f32>(a + 0x90C, row3);
        gabi::store<f32>(a + 0x910, gabi::load<f32>(row + 16));
        gabi::store<f32>(a + 0x914, gabi::load<f32>(row + 20));
        f32 row6 = gabi::load<f32>(row + 24);
        u32 actorX = gabi::load<u32>(a + 0x314);
        gabi::store<f32>(a + 0x8F4, row6);
        f32 row7 = gabi::load<f32>(row + 28);
        f32 speed = gabi::load<f32>(0x10016DE0);
        gabi::store<f32>(a + 0x8F8, row7);
        f32 row8 = gabi::load<f32>(row + 32);
        gabi::store<u32>(a + 0x8DC, actorX);
        gabi::store<f32>(a + 0x8FC, row8);
        path = (params >> 16) & 0xFF;
        u32 actorZ = gabi::load<u32>(a + 0x31C);
        u32 actorY = gabi::load<u32>(a + 0x318);
        gabi::store<u32>(a + 0x8E4, actorZ);
        gabi::store<u8>(a + 0x94F, (u8)path);
        gabi::store<u32>(a + 0x8E0, actorY);
        gabi::store<f32>(a + 0x374, speed);
        break;
    }
    case 2: case 3: case 5: case 8: {
        s8 currentType = gabi::load<s8>(a + 0x9C4);
        u32 params = gabi::load<u32>(a + 0xB0);
        gabi::store<u8>(a + 0x389, 0xAA);
        gabi::store<u8>(a + 0x38B, 0xAA);
        u32 row = 0x101BBCF0 + (u32)((s32)currentType * 36);
        gabi::store<f32>(a + 0x900, gabi::load<f32>(row + 0));
        gabi::store<f32>(a + 0x904, gabi::load<f32>(row + 4));
        gabi::store<f32>(a + 0x908, gabi::load<f32>(row + 8));
        f32 row3 = gabi::load<f32>(row + 12);
        gabi::store<f32>(a + 0x90C, row3);
        gabi::store<f32>(a + 0x910, gabi::load<f32>(row + 16));
        gabi::store<f32>(a + 0x914, gabi::load<f32>(row + 20));
        f32 row6 = gabi::load<f32>(row + 24);
        u32 actorX = gabi::load<u32>(a + 0x314);
        gabi::store<f32>(a + 0x8F4, row6);
        f32 row7 = gabi::load<f32>(row + 28);
        f32 speed = gabi::load<f32>(0x10016DE0);
        gabi::store<f32>(a + 0x8F8, row7);
        f32 row8 = gabi::load<f32>(row + 32);
        gabi::store<u32>(a + 0x8DC, actorX);
        gabi::store<f32>(a + 0x8FC, row8);
        path = (params >> 16) & 0xFF;
        u32 actorZ = gabi::load<u32>(a + 0x31C);
        u32 actorY = gabi::load<u32>(a + 0x318);
        gabi::store<u32>(a + 0x8E4, actorZ);
        gabi::store<u8>(a + 0x94F, (u8)path);
        gabi::store<u32>(a + 0x8E0, actorY);
        gabi::store<f32>(a + 0x374, speed);
        break;
    }
    case 4: {
        s8 currentType = gabi::load<s8>(a + 0x9C4);
        gabi::store<u8>(a + 0x389, 0xA7);
        gabi::store<u8>(a + 0x38B, 0xAB);
        u32 row = 0x101BBCF0 + (u32)((s32)currentType * 36);
        gabi::store<f32>(a + 0x900, gabi::load<f32>(row + 0));
        gabi::store<f32>(a + 0x904, gabi::load<f32>(row + 4));
        gabi::store<f32>(a + 0x908, gabi::load<f32>(row + 8));
        f32 row3 = gabi::load<f32>(row + 12);
        u32 actorZ = gabi::load<u32>(a + 0x31C);
        gabi::store<f32>(a + 0x90C, row3);
        gabi::store<f32>(a + 0x910, gabi::load<f32>(row + 16));
        gabi::store<f32>(a + 0x914, gabi::load<f32>(row + 20));
        f32 row6 = gabi::load<f32>(row + 24);
        u32 actorX = gabi::load<u32>(a + 0x314);
        gabi::store<f32>(a + 0x8F4, row6);
        f32 row7 = gabi::load<f32>(row + 28);
        f32 speed = gabi::load<f32>(0x10016DE0);
        gabi::store<f32>(a + 0x8F8, row7);
        f32 row8 = gabi::load<f32>(row + 32);
        gabi::store<u32>(a + 0x8DC, actorX);
        gabi::store<f32>(a + 0x8FC, row8);
        gabi::store<f32>(a + 0x374, speed);
        u32 params = gabi::load<u32>(a + 0xB0);
        u32 actorY = gabi::load<u32>(a + 0x318);
        gabi::store<u32>(a + 0x8E4, actorZ);
        path = (params >> 16) & 0xFF;
        gabi::store<u32>(a + 0x8E0, actorY);
        gabi::store<u8>(a + 0x94F, (u8)path);
        break;
    }
    case 6: {
        s8 currentType = gabi::load<s8>(a + 0x9C4);
        u32 params = gabi::load<u32>(a + 0xB0);
        gabi::store<u8>(a + 0x389, 0x5A);
        gabi::store<u8>(a + 0x38B, 0x5A);
        u32 actorX = gabi::load<u32>(a + 0x314);
        u32 row = 0x101BBCF0 + (u32)((s32)currentType * 36);
        gabi::store<f32>(a + 0x900, gabi::load<f32>(row + 0));
        gabi::store<f32>(a + 0x904, gabi::load<f32>(row + 4));
        gabi::store<f32>(a + 0x908, gabi::load<f32>(row + 8));
        f32 row3 = gabi::load<f32>(row + 12);
        gabi::store<f32>(a + 0x90C, row3);
        gabi::store<f32>(a + 0x910, gabi::load<f32>(row + 16));
        gabi::store<f32>(a + 0x914, gabi::load<f32>(row + 20));
        f32 row6 = gabi::load<f32>(row + 24);
        u32 actorZ = gabi::load<u32>(a + 0x31C);
        gabi::store<f32>(a + 0x8F4, row6);
        f32 row7 = gabi::load<f32>(row + 28);
        f32 speed = gabi::load<f32>(0x10016DE0);
        gabi::store<f32>(a + 0x8F8, row7);
        f32 row8 = gabi::load<f32>(row + 32);
        gabi::store<f32>(a + 0x374, speed);
        gabi::store<f32>(a + 0x8FC, row8);
        gabi::store<u32>(a + 0x8DC, actorX);
        u32 actorY = gabi::load<u32>(a + 0x318);
        gabi::store<u32>(a + 0x8E4, actorZ);
        path = (params >> 16) & 0xFF;
        gabi::store<u32>(a + 0x8E0, actorY);
        gabi::store<u8>(a + 0x94F, (u8)path);
        break;
    }
    default: return 0;
    }
    if (path != 255) {
        s8 room = gabi::load<s8>(a + 0x326);
        gabi::call<void>(0x0259E6D0, gabi::at<void>(a + 0x844), path, room, 1);
        if (gabi::load<u32>(a + 0x844) == 0) return 0;
        u32 flags = gabi::load<u32>(a + 0x2E0);
        gabi::store<u32>(a + 0x2E0, flags & ~0x80u);
    }
    s8 eventType = gabi::load<s8>(a + 0x9C5);
    u32 eventName = gabi::load<u32>(0x101BBE34 + (u32)((s32)eventType * 4));
    gabi::call<void>(0x0259F814, gabi::at<void>(a + 0x84C), eventName, this);
    eventType = gabi::load<s8>(a + 0x9C5);
    gabi::store<u8>(a + 0x9BF, 11);
    if ((u32)(s32)eventType > 16) return 0;
    BOOL initialized;
    switch (eventType) {
    case 6: initialized = gabi::call<BOOL>(0x021F3EF0, this); break;
    case 10: initialized = gabi::call<BOOL>(0x021F3F8C, this); break;
    case 0: case 2: case 4: case 8: case 13: case 15:
        initialized = gabi::call<BOOL>(0x021F4038, this); break;
    case 1: case 3: case 5: case 7: case 9: case 11: case 14: case 16:
        initialized = gabi::call<BOOL>(0x021F40D4, this); break;
    case 12: initialized = gabi::call<BOOL>(0x021F42B0, this); break;
    default: return 0;
    }
    if (!initialized) return 0;
    u16 zAngle = gabi::load<u16>(a + 0x324);
    u16 yAngle = gabi::load<u16>(a + 0x322);
    u16 xAngle = gabi::load<u16>(a + 0x320);
    gabi::store<u16>(a + 0x32C, zAngle);
    gabi::store<u16>(a + 0x32A, yAngle);
    gabi::store<u16>(a + 0x328, xAngle);
    gabi::call<void>(0x02515F14, gabi::at<void>(a + 0x654), 0xFF, 0xFF, this);
    gabi::store<u32>(a + 0x6D4, a + 0x654);
    gabi::call<void>(0x02516518, gabi::at<void>(a + 0x690), gabi::at<void>(0x101EA190));
    u32 morf = gabi::load<u32>(a + 0x44C);
    f32 zero = gabi::load<f32>(0x10016C08);
    gabi::call<void>(0x025E4A54, gabi::at<void>(morf), zero);
    gabi::call<void>(0x021F4974, this, 1);
    return 1;
}
VERIFY(0x021F4D48, &daNpc_Bj1_c::createInit);

// Proposal only, full dispatcher reference port.
u32 daNpc_Bj1_c::next_msgStatus(be<u32>* message) {
    WWHD_FUNC(0x021F8384, u32, this, message);
    u32 actor = gabi::ea(this);
    u32 msg = gabi::load<u32>(gabi::ea(message));
    switch (msg) {
    case 0x13ED: gabi::store<u32>(gabi::ea(message), 0x13EE); return 15;
    case 0x13EE: gabi::store<u32>(gabi::ea(message), 0x13EF); return 15;
    case 0x13F0: gabi::store<u32>(gabi::ea(message), 0x13F1); return 15;
    case 0x13F2: gabi::store<u32>(gabi::ea(message), 0x13F3); return 15;
    case 0x13F5: gabi::store<u32>(gabi::ea(message), 0x13F6); return 15;
    case 0x13F8: gabi::store<u32>(gabi::ea(message), 0x13F9); return 15;
    case 0x13FB: gabi::store<u32>(gabi::ea(message), 0x13FC); return 15;
    case 0x13FD: gabi::store<u32>(gabi::ea(message), 0x13FE); return 15;
    case 0x13FF: gabi::store<u32>(gabi::ea(message), 0x1400); return 15;
    case 0x1400: gabi::store<u32>(gabi::ea(message), 0x1401); return 15;
    case 0x1402: gabi::store<u32>(gabi::ea(message), 0x1403); return 15;
    case 0x1403: gabi::store<u32>(gabi::ea(message), 0x1404); return 15;
    case 0x1406: gabi::store<u32>(gabi::ea(message), 0x1407); return 15;
    case 0x1407: gabi::store<u32>(gabi::ea(message), 0x1408); return 15;
    case 0x140B: gabi::store<u32>(gabi::ea(message), 0x140C); return 15;
    case 0x140E: gabi::store<u32>(gabi::ea(message), 0x140F); return 15;
    case 0x1412: gabi::store<u32>(gabi::ea(message), 0x1413); return 15;
    case 0x1414: gabi::store<u32>(gabi::ea(message), 0x1415); return 15;
    case 0x1416: gabi::store<u32>(gabi::ea(message), 0x1417); return 15;
    case 0x1417: gabi::store<u32>(gabi::ea(message), 0x1418); return 15;
    case 0x1418: gabi::store<u32>(gabi::ea(message), 0x1419); return 15;
    case 0x1419: gabi::store<u32>(gabi::ea(message), 0x141A); return 15;
    case 0x141A: gabi::store<u32>(gabi::ea(message), 0x141B); return 15;
    case 0x141B: gabi::store<u32>(gabi::ea(message), 0x141C); return 15;
    case 0x141C: gabi::store<u32>(gabi::ea(message), 0x141D); return 15;
    case 0x141F: gabi::store<u32>(gabi::ea(message), 0x1420); return 15;
    case 0x1422: gabi::store<u32>(gabi::ea(message), 0x1423); return 15;
    case 0x1425: gabi::store<u32>(gabi::ea(message), 0x1426); return 15;
    case 0x1428: gabi::store<u32>(gabi::ea(message), 0x1429); return 15;
    case 0x142C: gabi::store<u32>(gabi::ea(message), 0x142D); return 15;
    case 0x142E: gabi::store<u32>(gabi::ea(message), 0x142F); return 15;
    case 0x1431: gabi::store<u32>(gabi::ea(message), 0x1432); return 15;
    case 0x1433: gabi::store<u32>(gabi::ea(message), 0x1417); return 15;
    case 0x1482: gabi::store<u32>(gabi::ea(message), 0x1483); return 15;
    case 0x1487: gabi::store<u32>(gabi::ea(message), 0x1488); return 15;
    case 0x1488: gabi::store<u32>(gabi::ea(message), 0x1489); return 15;
    case 0x148A: gabi::store<u32>(gabi::ea(message), 0x148B); return 15;
    case 0x148B: gabi::store<u32>(gabi::ea(message), 0x148C); return 15;
    case 0x148C: gabi::store<u32>(gabi::ea(message), 0x148D); return 15;
    case 0x148D: gabi::store<u32>(gabi::ea(message), 0x148E); return 15;
    case 0x148E: gabi::store<u32>(gabi::ea(message), 0x148F); return 15;
    case 0x148F: gabi::store<u32>(gabi::ea(message), 0x1490); return 15;
    case 0x1490: gabi::store<u32>(gabi::ea(message), 0x1491); return 15;
    case 0x1491: gabi::store<u32>(gabi::ea(message), 0x1492); return 15;
    case 0x1492: gabi::store<u32>(gabi::ea(message), 0x1493); return 15;
    case 0x1494: gabi::store<u32>(gabi::ea(message), 0x1495); return 15;
    case 0x1495: gabi::store<u32>(gabi::ea(message), 0x1496); return 15;
    case 0x1497: gabi::store<u32>(gabi::ea(message), 0x1498); return 15;
    case 0x1498: gabi::store<u32>(gabi::ea(message), 0x1499); return 15;
    case 0x1499: gabi::store<u32>(gabi::ea(message), 0x149A); return 15;
    case 0x149A: gabi::store<u32>(gabi::ea(message), 0x149B); return 15;
    case 0x149B: gabi::store<u32>(gabi::ea(message), 0x149C); return 15;
    case 0x149C: gabi::store<u32>(gabi::ea(message), 0x149D); return 15;
    case 0x1423: {
        u32 saved = gabi::load<u32>(0x101F84DC);
        gabi::call<void>(0x025B8B68, gabi::at<void>(saved + 0x644), 0x1B80);
        // Fall through to shared decision; saved-global is reloaded there.
        break;
    }
    case 0x1424: break;
    default: return 16;
    }
    u32 saved = gabi::load<u32>(0x101F84DC);
    u8 count = gabi::load<u8>(saved + 0xBD);
    u32 next;
    if (count < 4) {
        u8 flag = gabi::load<u8>(actor + 0x950);
        next = flag == 0 ? 0x1425 : 0x1427;
    } else {
        if (gabi::call<u32>(0x025B5C54, gabi::at<void>(saved + 0x5C)) != 0) {
            next = 0x1428;
        } else {
            u8 flag = gabi::load<u8>(actor + 0x951);
            next = flag == 0 ? 0x142E : 0x1430;
        }
    }
    gabi::store<u32>(gabi::ea(message), next);
    return 15;
}
VERIFY(0x021F8384, &daNpc_Bj1_c::next_msgStatus);

BOOL daNpc_Bj1_c::talk_1() {
    WWHD_FUNC(0x021F98C4, BOOL, this);
    u32 actor = gabi::ea(this);
    BOOL result = gabi::call<BOOL>(0x021F8190, this);
    gabi::call(0x025A11EC, this, 1);
    if (gabi::load<u8>(actor + 0x7CC) == 0) return result;
    u32 message = gabi::load<u32>(0x101F4B5C);
    if (gabi::call<u32>(0x025F795C, gabi::at<void>(message)) != 19) return result;
    u32 id = gabi::load<u32>(actor + 0x7C0);
    u32 flag = 0;
    s8 previous;
    bool captured = false;
    switch (id) {
    case 0x140F: flag = 0x604; break;
    case 0x1410: flag = 0x904; break;
    case 0x1413: flag = 0xC80; break;
    case 0x13F3: flag = 0x920; break;
    case 0x13F9: flag = 0xC20; break;
    case 0x13FE: flag = 0xC08; break;
    case 0x13F6: flag = 0xC04; break;
    case 0x1404: flag = 0xC02; break;
    case 0x1408: flag = 0xD40; break;
    case 0x141D: flag = 0xD08; break;
    case 0x1432: flag = 0x1C80; break;
    case 0x140C: flag = 0xD10; break;
    case 0x1409: flag = 0xD20; break;
    case 0x13EF: flag = 0xC10; break;
    case 0x1426:
        previous = (s8)(u8)m9C2;
        captured = true;
        m950 = 1;
        break;
    case 0x1429: {
        u32 player = gabi::call<u32>(0x025200D4);
        s16 value = gabi::load<s16>(player + 0x5B72);
        gabi::store<u16>(player + 0x5B72, (u16)((s32)value - 4));
        previous = (s8)(u8)m9C2;
        m9C0 = 3;
        m965 = 0;
        m94E = 255;
        gabi::call(0x021F919C, this, (s32)previous);
        m942 = 60;
        gabi::call(0x021F5754, this);
        return result;
    }
    case 0x142B:
        previous = (s8)(u8)m9C2;
        captured = true;
        m94E = 255;
        m9C0 = 4;
        m965 = 0;
        gabi::call(0x021F919C, this, (s32)previous);
        m942 = 60;
        gabi::call(0x021F5754, this);
        return result;
    case 0x142F:
        previous = (s8)(u8)m9C2;
        captured = true;
        m951 = 1;
        break;
    case 0x1493: case 0x149D: {
        u32 save = gabi::load<u32>(0x101F84DC) + 0x644u;
        gabi::call(0x025B8B68, gabi::at<void>(save), 0x2E10);
        save = gabi::load<u32>(0x101F84DC) + 0x644u;
        gabi::call(0x025B8B68, gabi::at<void>(save), 0x1D40);
        gabi::call(0x021F8314, this, 0x97FF);
        break;
    }
    case 0x1483: gabi::call(0x021F8314, this, 0x96FF); break;
    case 0x1485: gabi::call(0x021F8314, this, 0x99FF); break;
    case 0x1489: gabi::call(0x021F8314, this, 0x98FF); break;
    default: break;
    }
    if (flag != 0) {
        u32 save = gabi::load<u32>(0x101F84DC) + 0x644u;
        gabi::call(0x025B8B68, gabi::at<void>(save), flag);
    }
    if (!captured) previous = (s8)(u8)m9C2;
    m94E = 255;
    m965 = 0;
    gabi::call(0x021F919C, this, (s32)previous);
    m942 = 60;
    gabi::call(0x021F5754, this);
    return result;
}
VERIFY(0x021F98C4, &daNpc_Bj1_c::talk_1);

BOOL daNpc_Bj1_c::fall01() {
    WWHD_FUNC(0x021F9844, BOOL, this);
    u32 actor = gabi::ea(this);
    s8 phase = (s8)(u8)m9B8;
    m9C0 = 0;
    m959 = 0;
    if (phase == 0) {
        gabi::call(0x021F919C, this, 5);
    } else if (phase == 1 && gabi::ftoi(gabi::load<f32>(actor + 0x370)) == 0) {
        f32 zero = gabi::load<f32>(0x10016C08);
        m9B8 = 2;
        gabi::store<f32>(actor + 0x370, zero);
    }
    return 1;
}
VERIFY(0x021F9844, &daNpc_Bj1_c::fall01);

BOOL daNpc_Bj1_c::walk_1() {
    WWHD_FUNC(0x021FA138, BOOL, this);
    u32 actor = gabi::ea(this);
    s8 phase = (s8)(u8)m9BA;
    m95A = 0;
    if (phase == 0) {
        u32 y = gabi::load<u32>(actor + 0x318);
        u32 x = gabi::load<u32>(actor + 0x314);
        f32 zero = gabi::load<f32>(0x10016C08);
        u16 az = gabi::load<u16>(actor + 0x324);
        m91C = zero;
        gabi::store<u16>(actor + 0x8CC, az);
        gabi::store<f32>(actor + 0x370, zero);
        m924 = zero;
        u16 ax = gabi::load<u16>(actor + 0x320);
        u16 ay = gabi::load<u16>(actor + 0x322);
        gabi::store<u16>(actor + 0x8C8, ax);
        gabi::store<u16>(actor + 0x8CA, ay);
        gabi::store<u32>(actor + 0x8BC, x);
        u32 z = gabi::load<u32>(actor + 0x31C);
        gabi::store<u32>(actor + 0x8C0, y);
        gabi::store<u32>(actor + 0x8C4, z);
        gabi::call(0x021F919C, this, 7);
    } else {
        u32 point = gabi::load<u8>(actor + 0x849);
        gabi::Local<cXyz> position;
        gabi::call(0x0259E778, gabi::at<void>(actor + 0x844), position.get(), point);
        u32 z = gabi::load<u32>(position.a + 8);
        u32 x = gabi::load<u32>(position.a);
        gabi::store<u32>(actor + 0x8F0, z);
        u32 y = gabi::load<u32>(position.a + 4);
        gabi::store<u32>(actor + 0x8E8, x);
        gabi::store<u32>(actor + 0x8EC, y);
        m966 = 1;
    }
    return 1;
}
VERIFY(0x021FA138, &daNpc_Bj1_c::walk_1);

void daNpc_Bj1_c::set_pthPoint(u8 requested) {
    WWHD_FUNC(0x021F9114, void, this, requested);
    u32 actor = gabi::ea(this);
    if (gabi::load<u32>(actor + 0x844) == 0) return;
    u32 count = gabi::call<u32>(0x0259EDB8, gabi::at<void>(actor + 0x844));
    u32 selected = requested;
    if (count < selected) selected = count;
    gabi::Local<cXyz> position;
    u32 byteIndex = selected & 255u;
    gabi::store<u8>(actor + 0x849, (u8)byteIndex);
    gabi::call(0x0259E778, gabi::at<void>(actor + 0x844), position.get(), byteIndex);
    u32 y = gabi::load<u32>(position.a + 4);
    u32 x = gabi::load<u32>(position.a);
    u32 z = gabi::load<u32>(position.a + 8);
    gabi::store<u32>(actor + 0x314, x);
    gabi::store<u32>(actor + 0x31C, z);
    gabi::store<u32>(actor + 0x318, y);
    gabi::call(0x0259ED58, gabi::at<void>(actor + 0x844));
}
VERIFY(0x021F9114, &daNpc_Bj1_c::set_pthPoint);

// Proposal only; root owns source adoption and baseline testing.
BOOL daNpc_Bj1_c::chkAttention() {
    WWHD_FUNC(0x021F908C, BOOL, this);
    u32 actor = gabi::ea(this);
    u32 scene = gabi::call<u32>(0x025200D4);
    u32 attention = scene + 0x5804;
    u32 selected;
    if (gabi::call<u32>(0x024EDFCC, gabi::at<void>(attention)) != 0)
        selected = gabi::call<u32>(0x024EC8D0, gabi::at<void>(attention), 0);
    else
        selected = gabi::call<u32>(0x024EE464, gabi::at<void>(attention), 0);
    return selected == actor;
}
VERIFY(0x021F908C, &daNpc_Bj1_c::chkAttention);


// Ordinary source proposal only, no active-stage writes.
BOOL daNpc_Bj1_c::eMove_SET_TNE_() {
    WWHD_FUNC(0x021F6818, BOOL, this);
    u32 actor = gabi::ea(this);
    f32 frame = gabi::load<f32>(0x10016EB4);
    u32 morf = gabi::load<u32>(actor + 0x44C);
    if (gabi::call<u32>(0x027F2BF8, gabi::at<void>(morf + 0x98), frame) != 0)
        gabi::call<void>(0x021F65B8, this);
    return gabi::load<s8>(actor + 0x94C) != 0;
}
VERIFY(0x021F6818, &daNpc_Bj1_c::eMove_SET_TNE_);

BOOL daNpc_Bj1_c::eMove_PTH_MOV_() {
    WWHD_FUNC(0x021F6874, BOOL, this);
    u32 actor = gabi::ea(this);
    BOOL done = gabi::load<s8>(actor + 0x9BA) == 0;
    if (!done) {
        if (gabi::load<u8>(actor + 0x95A) != 0) {
            u8 point = gabi::load<u8>(actor + 0x849);
            struct PointFrame { be<u32> linkage[2]; cXyz output; };
            gabi::Local<PointFrame> local;
            gabi::call<void>(0x0259E778, gabi::at<void>(actor + 0x844),
                             gabi::at<cXyz>(local.a + 8), point);
            u32 x = gabi::load<u32>(local.a + 8);
            u32 z = gabi::load<u32>(local.a + 16);
            gabi::store<u32>(actor + 0x8E8, x);
            u32 y = gabi::load<u32>(local.a + 12);
            gabi::store<u32>(actor + 0x8F0, z);
            gabi::store<u32>(actor + 0x8EC, y);
        }
        gabi::store<u8>(actor + 0x95A, 0);
    }
    return done;
}
VERIFY(0x021F6874, &daNpc_Bj1_c::eMove_PTH_MOV_);

u32 daNpc_Bj1_c::event_action() {
    WWHD_FUNC(0x021F68F8, u32, this);
    switch (gabi::load<s8>(gabi::ea(this) + 0x9BC)) {
    case 0: return gabi::call<u32>(0x021F6504, this);
    case 2: return gabi::call<u32>(0x021F6558, this);
    case 3: return gabi::call<u32>(0x021F65A4, this);
    case 6: return gabi::call<u32>(0x021F6818, this);
    case 8: return gabi::call<u32>(0x021F6874, this);
    default: return 1;
    }
}
VERIFY(0x021F68F8, &daNpc_Bj1_c::event_action);

void daNpc_Bj1_c::anmAtr(u16 attribute) {
    WWHD_FUNC(0x021F7FB0, void, this, attribute);
    u32 actor = gabi::ea(this);
    if (attribute == 6) {
        if (gabi::load<s8>(actor + 0x9C7) == 0) {
            gabi::store<u8>(actor + 0x9BD, 255);
            u32 scene = gabi::call<u32>(0x025200D4);
            u8 value = gabi::load<u8>(scene + 0x5BC5);
            gabi::call<void>(0x021F7F40, this, value);
            u8 phase = gabi::load<u8>(actor + 0x9C7);
            gabi::store<u8>(actor + 0x9C7, (u8)(phase + 1));
        }
        u32 scene = gabi::call<u32>(0x025200D4);
        u8 value = gabi::load<u8>(scene + 0x5BC6);
        scene = gabi::call<u32>(0x025200D4);
        gabi::store<u8>(scene + 0x5BC6, 255);
        if (value != 255 && gabi::load<u8>(actor + 0x9BE) != value)
            gabi::store<u8>(actor + 0x9BE, value);
    } else if (attribute == 14) {
        gabi::store<u8>(actor + 0x9C7, 0);
    }
    gabi::call<void>(0x021F7F5C, this);
}
VERIFY(0x021F7FB0, &daNpc_Bj1_c::anmAtr);

BOOL daNpc_Bj1_c::_delete() {
    WWHD_FUNC(0x021F53D8, BOOL, this);
    u32 a = gabi::ea(this);
    gabi::call(0x025204C8, gabi::at<void>(a + 0x7DC), STR(0x10016E1B));
    if (gabi::load<u32>(a + 0xF4) != 0) {
        u32 morph = gabi::load<u32>(a + 0x44C);
        if (morph) gabi::call(0x025E563C, gabi::at<void>(morph));
        morph = gabi::load<u32>(a + 0x7F8);
        if (morph) gabi::call(0x025E563C, gabi::at<void>(morph));
    }
    gabi::call(0x021F5364, this); // Immediate leaf preserves incoming r3.
    gabi::call(0x021F4344, this);
    return 1;
}
VERIFY(0x021F53D8, &daNpc_Bj1_c::_delete);
static BOOL daNpc_Bj1_Delete(daNpc_Bj1_c* self) {
    WWHD_FUNC(0x021F544C, BOOL, self);
    return gabi::call<BOOL>(0x021F53D8, self);
}
VERIFY(0x021F544C, daNpc_Bj1_Delete);

BOOL daNpc_Bj1_c::_execute() {
    WWHD_FUNC(0x021F7944, BOOL, this);
    u32 a = gabi::ea(this);
    if (gabi::load<u8>(a + 0x95E) == 0) {
        u16 ax = gabi::load<u16>(a + 0x320), ay = gabi::load<u16>(a + 0x322);
        gabi::store<u16>(a + 0x8C8, ax);
        u32 x = gabi::load<u32>(a + 0x314), y = gabi::load<u32>(a + 0x318);
        gabi::store<u32>(a + 0x8BC, x);
        u16 az = gabi::load<u16>(a + 0x324);
        gabi::store<u16>(a + 0x8CA, ay); gabi::store<u16>(a + 0x8CC, az);
        u32 z = gabi::load<u32>(a + 0x31C);
        gabi::store<u32>(a + 0x8C0, y); gabi::store<u32>(a + 0x8C4, z);
        gabi::store<u8>(a + 0x95E, 1);
    }
    u32 table = 0x10465DB0u + (u32)(s32)gabi::load<s8>(a + 0x9C4) * 108u;
    s32 p5=gabi::load<s16>(table+0xE), p6=gabi::load<s16>(table+0x10), p7=gabi::load<s16>(table+0x12);
    s32 p9=gabi::load<s16>(table+6), p8=gabi::load<s16>(table+4), p4=gabi::load<s16>(table+0xC);
    s32 p11=gabi::load<s16>(table+0xA), p10=gabi::load<s16>(table+8), p12=gabi::load<s16>(table+0x14);
    gabi::call(0x0259E08C, gabi::at<void>(a+0x3AC), p4,p5,p6,p7,p8,p9,p10,p11,p12);
    if (gabi::load<u8>(a+0x954) != 0 && gabi::load<u8>(a+0x2DC) == 0) return 1;
    gabi::store<u8>(a+0x957,0); gabi::store<u8>(a+0x954,0);
    gabi::call(0x021F54FC,this); gabi::call(0x021F5568,this);
    if (!gabi::call<BOOL>(0x021F563C,this)) {
        u32 scene=gabi::call<u32>(0x025200D4);
        bool eventPath=false;
        if (gabi::load<u8>(scene+0x5292) != 0 && gabi::load<u16>(a+0xF8) != 1) {
            s32 staff=gabi::call<s32>(0x021F5714,this);
            if (staff >= 0) { gabi::call(0x021F6CC4,this,staff); eventPath=true; }
        }
        if (!eventPath) bj1_invokeAction(a+0x80C,a,(void*)nullptr); // Existing exact call_ptr helper; no null guard.
        if (!gabi::call<BOOL>(0x021F7270,this)) gabi::call(0x021F7684,this);
        if (gabi::load<u8>(a+0x957)==0) gabi::call(0x025D6870,this,gabi::at<void>(a+0x654));
        if (gabi::load<u8>(a+0x956)==0) {
            u16 x=gabi::load<u16>(a+0x320),y=gabi::load<u16>(a+0x322);
            gabi::store<u16>(a+0x328,x);
            u16 z=gabi::load<u16>(a+0x324);
            gabi::store<u16>(a+0x32A,y);gabi::store<u16>(a+0x32C,z);
        }
    }
    gabi::call(0x021F7828,this);
    if (gabi::load<u8>(a+0x95C)!=0) {
        s8 type=gabi::load<s8>(a+0x9C4);
        s32 angle=gabi::load<s16>(a+0x804), delta=gabi::load<s16>(a+0x806);
        f32 zero=gabi::load<f32>(0x10016C08);
        table=0x10465DB0u+(u32)(s32)type*108u;
        gabi::store<u16>(a+0x804,(u16)(angle+delta));
        s32 denominator=gabi::load<s16>(table+0x44);
        f64 value=zero;
        if (denominator!=0) {
            delta=gabi::load<s16>(a+0x806);
            f64 bias=gabi::load<f64>(0x10016DD8);
            f32 numerator=(f32)(u64_as_f64(0x4330000000000000ull|((u32)delta^0x80000000u))-bias);
            f32 divisor=(f32)(u64_as_f64(0x4330000000000000ull|((u32)denominator^0x80000000u))-bias);
            f32 ratio=(f32)((f64)numerator/(f64)divisor);
            f32 scale=gabi::load<f32>(0x10016ECC);
            f32 scaled=(f32)((f64)ratio*round25((f64)scale));
            f32 comparison=(f32)((f64)scaled-(f64)scale);
            value=ppc_fsel(comparison,scaled,scale);
            value=ppc_fsel(value,zero,value);
        }
        f32 split=gabi::load<f32>(0x10016ED0);
        u32 soundValue;
        if (value < (f64)split) soundValue=(u32)gabi::ftoi(value);
        else soundValue=(u32)gabi::ftoi((f32)(value-(f64)split))+0x80000000u;
        s32 room=gabi::load<s8>(a+0x326);
        s32 reverb=gabi::call<s32>(0x02520540,room);
        gabi::call(0x025E1A40,0x50BF,gabi::at<void>(a+0x314),soundValue,reverb);
    }
    gabi::call(0x021F4974,this,0);
    if (gabi::load<u8>(a+0x967)==0) gabi::call(0x021F78B0,this);
    return 1;
}
VERIFY(0x021F7944, &daNpc_Bj1_c::_execute);
static BOOL daNpc_Bj1_Execute(daNpc_Bj1_c* self) {
    WWHD_FUNC(0x021F7C70, BOOL, self);
    return gabi::call<BOOL>(0x021F7944,self);
}
VERIFY(0x021F7C70, daNpc_Bj1_Execute);

BOOL daNpc_Bj1_c::_draw() {
    WWHD_FUNC(0x021F7C74, BOOL, this);
    u32 a=gabi::ea(this);
    u8 hidden=gabi::load<u8>(a+0x954);
    u32 morph=gabi::load<u32>(a+0x44C);
    u32 model=gabi::load<u32>(morph+0x90); // This read occurs even when hidden.
    if (hidden!=0 || gabi::load<u8>(a+0x955)!=0) return 1;
    u32 lighting=gabi::call<u32>(0x02555D0C);
    gabi::call(0x025626A4,gabi::at<void>(lighting),0,gabi::at<void>(a+0x314),gabi::at<void>(a+0x110));
    lighting=gabi::call<u32>(0x02555D0C);
    gabi::call(0x02562F5C,gabi::at<void>(lighting),gabi::at<void>(model),gabi::at<void>(a+0x110));
    gabi::call(0x025200D4);
    gabi::call(0x025E5590,gabi::at<void>(gabi::load<u32>(a+0x44C)));
    if (gabi::load<u32>(a+0x7EC)!=0) {
        lighting=gabi::call<u32>(0x02555D0C);
        gabi::call(0x02562F5C,gabi::at<void>(lighting),gabi::at<void>(gabi::load<u32>(a+0x7EC)),gabi::at<void>(a+0x110));
        gabi::call(0x025E2E5C,gabi::at<void>(gabi::load<u32>(a+0x7EC)));
    }
    if (gabi::load<u32>(a+0x7F0)!=0) {
        lighting=gabi::call<u32>(0x02555D0C);
        gabi::call(0x02562F5C,gabi::at<void>(lighting),gabi::at<void>(gabi::load<u32>(a+0x7F0)),gabi::at<void>(a+0x110));
        gabi::call(0x025E2E5C,gabi::at<void>(gabi::load<u32>(a+0x7F0)));
    }
    lighting=gabi::call<u32>(0x02555D0C);
    gabi::call(0x02562F5C,gabi::at<void>(lighting),gabi::at<void>(gabi::load<u32>(a+0x7E8)),gabi::at<void>(a+0x110));
    gabi::call(0x025E2E5C,gabi::at<void>(gabi::load<u32>(a+0x7E8)));
    if (gabi::load<u8>(a+0x95C)!=0) {
        u32 prop=gabi::load<u32>(a+0x7F8);
        lighting=gabi::call<u32>(0x02555D0C);
        u32 propModel=gabi::load<u32>(prop+0x90);
        gabi::call(0x02562F5C,gabi::at<void>(lighting),gabi::at<void>(propModel),gabi::at<void>(a+0x110));
        gabi::call(0x025E5590,gabi::at<void>(gabi::load<u32>(a+0x7F8)));
    }
    s8 type=gabi::load<s8>(a+0x9C4);
    if ((u32)(s32)type<=8) {
        f32 scale=gabi::load<f32>(0x10016C18);
        u32 color=gabi::load<u8>(0x10016ED4u+(u32)(s32)type);
        gabi::call(0x025BED80,color,this,scale,scale,scale);
        type=gabi::load<s8>(a+0x9C4);
    }
    if (gabi::load<u8>(0x10465DCCu+(u32)(s32)type*108u)==0) return 1;
    if (gabi::load<u32>(0x101FDA50)==0) {
        gabi::store<u32>(0x101FDA50,1); memcpy_g(gabi::at<void>(0x101FEBF4),gabi::at<void>(0x10016B70),4);
    }
    if (gabi::load<u32>(0x101FDAC0)==0) {
        gabi::store<u32>(0x101FDAC0,1); memcpy_g(gabi::at<void>(0x101FEBF8),gabi::at<void>(0x10016B74),4);
    }
    if (gabi::load<u32>(0x101FDA48)==0) {
        gabi::store<u32>(0x101FDA48,1); memcpy_g(gabi::at<void>(0x101FEBEC),gabi::at<void>(0x10016B78),4);
    }
    if (gabi::load<s8>(a+0x9C5)==6 && gabi::load<u32>(0x101FDA48)==0) {
        gabi::store<u32>(0x101FDA48,1); memcpy_g(gabi::at<void>(0x101FEBEC),gabi::at<void>(0x10016B78),4);
    }
    if (gabi::load<u32>(0x101FDA44)==0) {
        gabi::store<u32>(0x101FDA44,1); memcpy_g(gabi::at<void>(0x101FEBE8),gabi::at<void>(0x10016B7C),4);
    }
    return 1;
}
VERIFY(0x021F7C74, &daNpc_Bj1_c::_draw);
static BOOL daNpc_Bj1_Draw(daNpc_Bj1_c* self) {
    WWHD_FUNC(0x021F7EE0, BOOL, self);
    return gabi::call<BOOL>(0x021F7C74,self);
}
VERIFY(0x021F7EE0, daNpc_Bj1_Draw);

// Proposal only; root owns source adoption and baseline tests.
void daNpc_Bj1_c::privateCut(int event) {
    WWHD_FUNC(0x021F6980, void, this, event);
    if (event == -1) return;
    u32 actor = gabi::ea(this);
    u32 scene = gabi::call<u32>(0x025200D4);
    s8 cut = (s8)gabi::call<u32>(0x02542EDC, gabi::at<void>(scene + 0x52C4),
                               event, gabi::at<void>(0x101BBE9C), 1, 1, 0);
    gabi::store<s8>(actor + 0x9BB, cut);
    scene = gabi::call<u32>(0x025200D4);
    if (cut == -1) {
        gabi::call<void>(0x02543280, gabi::at<void>(scene + 0x52C4), event);
        return;
    }
    if (gabi::call<u32>(0x025447C8, gabi::at<void>(scene + 0x52C4), event) != 0) {
        if (gabi::load<s8>(actor + 0x9BB) != 0) {
            scene = gabi::call<u32>(0x025200D4);
            gabi::call<void>(0x02543280, gabi::at<void>(scene + 0x52C4), event);
            return;
        }
        gabi::call<void>(0x021F6114, this, event);
    }
    if (gabi::load<s8>(actor + 0x9BB) == 0) {
        if (gabi::call<u32>(0x021F68F8, this) == 0) return;
    }
    scene = gabi::call<u32>(0x025200D4);
    gabi::call<void>(0x02543280, gabi::at<void>(scene + 0x52C4), event);
}
VERIFY(0x021F6980, &daNpc_Bj1_c::privateCut);

void daNpc_Bj1_c::endEvent() {
 WWHD_FUNC(0x021F5754, void, this);
 u32 scene=gabi::call<u32>(0x025200D4);
 u16 flags=gabi::load<u16>(scene+0x52B8);
 gabi::store<u16>(scene+0x52B8,(u16)(flags|8));
 gabi::store<u8>(gabi::ea(this)+0x9BD,255);
}
VERIFY(0x021F5754,&daNpc_Bj1_c::endEvent);
void daNpc_Bj1_c::eInit_setShapeAngleY(int* input,s16 angle) {
 WWHD_FUNC(0x021F594C,void,this,input,angle);
 u32 actor=gabi::ea(this);
 gabi::store<u8>(actor+0x956,0);
 if (!input) return;
 u32 value=gabi::load<u32>(gabi::ea(input));
 u8 enabled=(value==1);
 gabi::store<u8>(actor+0x956,enabled);
 if(enabled) gabi::store<s16>(actor+0x32A,angle);
}
VERIFY(0x021F594C,&daNpc_Bj1_c::eInit_setShapeAngleY);
void daNpc_Bj1_c::setAttention(bool force) {
 WWHD_FUNC(0x021F4914,void,this,force);
 u32 a=gabi::ea(this);
 s8 type=gabi::load<s8>(a+0x9C4);
 u32 flag=gabi::load<u32>(a+0x960);
 f32 z=gabi::load<f32>(a+0x31C),y=gabi::load<f32>(a+0x318),x=gabi::load<f32>(a+0x314);
 f32 height=gabi::load<f32>(0x10465DC8u+(u32)(s32)type*108u);
 gabi::store<f32>(a+0x398,z);
 f32 lifted=(f32)((f64)y+(f64)height);
 gabi::store<f32>(a+0x390,x);
 gabi::store<f32>(a+0x394,lifted);
 if(flag==0 && !force)return;
 f32 bz=gabi::load<f32>(a+0x8D8),by=gabi::load<f32>(a+0x8D4);
 gabi::store<f32>(a+0x384,bz);
 f32 bx=gabi::load<f32>(a+0x8D0);
 gabi::store<f32>(a+0x380,by);
 gabi::store<f32>(a+0x37C,bx);
}
VERIFY(0x021F4914,&daNpc_Bj1_c::setAttention);

cPhs_State daNpc_Bj1_c::_create() {
    WWHD_FUNC(0x021F5220, cPhs_State, this);
    u32 actor = gabi::ea(this);
    u32 status = gabi::load<u32>(actor + 0x2E4);
    if ((status & 8u) == 0) {
        if (actor != 0) {
            gabi::call(0x025A1458, this);
            gabi::store<u32>(actor + 0xB4, 0x10016EF0);
            gabi::call(0x0259F740, gabi::at<void>(actor + 0x84C));
            status = gabi::load<u32>(actor + 0x2E4);
        }
        gabi::store<u32>(actor + 0x2E4, status | 8u);
    }
    s32 phase = gabi::call<s32>(0x02520460, gabi::at<void>(actor + 0x7DC), STR(0x10016E18));
    if (phase != 4) return (cPhs_State)phase;
    u32 subtype = gabi::load<u8>(actor + 0xB3);
    if (!gabi::call<BOOL>(0x021F3C28, this, subtype)) return (cPhs_State)5;
    s32 type = gabi::load<s8>(actor + 0x9C4);
    u32 heapSize = gabi::load<u32>(0x101BBE78u + (u32)type * 4u);
    if (!gabi::call<BOOL>(0x025D63E8, this, 0x021F3C24, heapSize)) return (cPhs_State)5;
    u32 morph = gabi::load<u32>(actor + 0x44C);
    u32 model = gabi::load<u32>(morph + 0x90);
    u32 cullMatrix = model != 0 ? model + 0xC8u : 0;
    f32 upperXZ = gabi::load<f32>(0x10016E0C);
    f32 lowerXZ = gabi::load<f32>(0x10016E08);
    f32 upperY = gabi::load<f32>(0x10016E14);
    f32 lowerY = gabi::load<f32>(0x10016E10);
    gabi::store<u32>(actor + 0x348, cullMatrix);
    gabi::call(0x025D674C, this, lowerXZ, lowerY, lowerXZ, upperXZ, upperY, upperXZ);
    if (!gabi::call<BOOL>(0x021F4D48, this)) return (cPhs_State)5;
    return (cPhs_State)phase;
}
VERIFY(0x021F5220, &daNpc_Bj1_c::_create);

static cPhs_State daNpc_Bj1_Create(daNpc_Bj1_c* self) {
    WWHD_FUNC(0x021F5360, cPhs_State, self);
    return gabi::call<cPhs_State>(0x021F5220, self);
}
VERIFY(0x021F5360, daNpc_Bj1_Create);

// Proposal only, retain unused void* action ABI.

BOOL daNpc_Bj1_c::wait_action1(void* unused) {
    WWHD_FUNC(0x021FA1FC, BOOL, this, unused);
    u32 actor = gabi::ea(this);
    s8 phase = gabi::load<s8>(actor + 0x9C6);
    if (phase == 0) {
        gabi::call<void>(0x021F919C, this, 1);
        u8 current = gabi::load<u8>(actor + 0x9C6);
        gabi::store<u8>(actor + 0x9C6, (u8)(current + 1));
        return 1;
    }
    if ((u32)(s32)phase > 3) return 1;
    u32 attention = gabi::call<u32>(0x021F908C, this);
    s8 state = gabi::load<s8>(actor + 0x9C1);
    gabi::store<u8>(actor + 0x964, (u8)attention);
    switch (state) {
    case 1: {
        u32 result = gabi::call<u32>(0x021F9358, this);
        gabi::store<u32>(actor + 0x960, result);
        break;
    }
    case 2: {
        u32 result = gabi::call<u32>(0x021F98C4, this);
        gabi::store<u32>(actor + 0x960, result);
        break;
    }
    default: break;
    }
    gabi::call<void>(0x021F6A54, this);
    return 1;
}
VERIFY(0x021FA1FC, &daNpc_Bj1_c::wait_action1);

BOOL daNpc_Bj1_c::wait_action2(void* unused) {
    WWHD_FUNC(0x021FA2B8, BOOL, this, unused);
    u32 actor = gabi::ea(this);
    s8 phase = gabi::load<s8>(actor + 0x9C6);
    if (phase == 0) {
        gabi::call<void>(0x021F919C, this, 5);
        u8 current = gabi::load<u8>(actor + 0x9C6);
        gabi::store<u8>(actor + 0x9C6, (u8)(current + 1));
        return 1;
    }
    if ((u32)(s32)phase > 3) return 1;
    u32 attention = gabi::call<u32>(0x021F908C, this);
    s8 state = gabi::load<s8>(actor + 0x9C1);
    gabi::store<u8>(actor + 0x964, (u8)attention);
    switch (state) {
    case 2: {
        u32 result = gabi::call<u32>(0x021F98C4, this);
        gabi::store<u32>(actor + 0x960, result);
        break;
    }
    case 3: {
        u32 result = gabi::call<u32>(0x021F9728, this);
        gabi::store<u32>(actor + 0x960, result);
        break;
    }
    case 4: {
        u32 result = gabi::call<u32>(0x021F9844, this);
        gabi::store<u32>(actor + 0x960, result);
        break;
    }
    case 5: {
        u32 result = gabi::call<u32>(0x021F9424, this);
        gabi::store<u32>(actor + 0x960, result);
        break;
    }
    default: break;
    }
    gabi::call<void>(0x021F6A54, this);
    return 1;
}
VERIFY(0x021FA2B8, &daNpc_Bj1_c::wait_action2);

BOOL daNpc_Bj1_c::wait_action3(void* unused) {
    WWHD_FUNC(0x021FA398, BOOL, this, unused);
    u32 actor = gabi::ea(this);
    s8 phase = gabi::load<s8>(actor + 0x9C6);
    if (phase == 0) {
        gabi::call<void>(0x021F9114, this, 0);
        gabi::call<void>(0x021F919C, this, 6);
        u8 current = gabi::load<u8>(actor + 0x9C6);
        gabi::store<u8>(actor + 0x9C6, (u8)(current + 1));
        return 1;
    }
    if ((u32)(s32)phase > 3) return 1;
    u32 attention = gabi::call<u32>(0x021F908C, this);
    s8 state = gabi::load<s8>(actor + 0x9C1);
    gabi::store<u8>(actor + 0x964, (u8)attention);
    switch (state) {
    case 2: {
        u32 result = gabi::call<u32>(0x021F98C4, this);
        gabi::store<u32>(actor + 0x960, result);
        break;
    }
    case 6: {
        u32 result = gabi::call<u32>(0x021FA138, this);
        gabi::store<u32>(actor + 0x960, result);
        break;
    }
    case 7: {
        u32 result = gabi::call<u32>(0x021F94FC, this);
        gabi::store<u32>(actor + 0x960, result);
        break;
    }
    default: break;
    }
    gabi::call<void>(0x021F6A54, this);
    return 1;
}
VERIFY(0x021FA398, &daNpc_Bj1_c::wait_action3);

BOOL daNpc_Bj1_c::wait_action4(void* unused) {
    WWHD_FUNC(0x021FA478, BOOL, this, unused);
    u32 actor = gabi::ea(this);
    s8 phase = gabi::load<s8>(actor + 0x9C6);
    if (phase == 0) {
        gabi::call<void>(0x021F9114, this, 0);
        gabi::call<void>(0x021F919C, this, 8);
        u8 current = gabi::load<u8>(actor + 0x9C6);
        gabi::store<u8>(actor + 0x9C6, (u8)(current + 1));
        return 1;
    }
    if ((u32)(s32)phase > 3) return 1;
    u32 attention = gabi::call<u32>(0x021F908C, this);
    s8 state = gabi::load<s8>(actor + 0x9C1);
    gabi::store<u8>(actor + 0x964, (u8)attention);
    switch (state) {
    case 2: {
        u32 result = gabi::call<u32>(0x021F98C4, this);
        gabi::store<u32>(actor + 0x960, result);
        break;
    }
    case 8: {
        u32 result = gabi::call<u32>(0x021F960C, this);
        gabi::store<u32>(actor + 0x960, result);
        break;
    }
    default: break;
    }
    gabi::call<void>(0x021F6A54, this);
    return 1;
}
VERIFY(0x021FA478, &daNpc_Bj1_c::wait_action4);

void daNpc_Bj1_c::setMtx_anmProc() {
    WWHD_FUNC(0x021F4628, void, this);
    u32 a=gabi::ea(this);
    s8 animation=gabi::load<s8>(a+0x9BF);
    if (animation==4) {
        if (gabi::load<s8>(a+0x9B9)==0) return;
        f32 direction=gabi::load<f32>(a+0x340),zero=gabi::load<f32>(0x10016C08);
        u32 morph=gabi::load<u32>(a+0x44C);
        bool negative=direction<zero;
        f32 boundary=gabi::load<f32>(0x10016DD0),frame=gabi::load<f32>(morph+0x9C);
        if (negative) {
            if (!(frame<boundary)) return;
            s32 end=gabi::load<s16>(morph+0xA2);
            f64 bias=gabi::load<f64>(0x10016DD8);
            f32 converted=(f32)(u64_as_f64(0x4330000000000000ull|((u32)end^0x80000000u))-bias);
            s16 truncated=(s16)gabi::ftoi(converted);
            f32 newFrame=(f32)(u64_as_f64(0x4330000000000000ull|((u32)(s32)truncated^0x80000000u))-bias);
            gabi::store<f32>(morph+0x9C,newFrame);
        } else if (!(frame<boundary)) gabi::store<f32>(morph+0x9C,boundary);
    } else if (animation==7) {
        if (gabi::load<s8>(a+0x94C)==0 || gabi::load<u32>(a+0x998)!=0) return;
        if (gabi::call<BOOL>(0x02055B64,gabi::at<void>(a+0x9A4))) return;
        s32 room=gabi::load<s8>(a+0x326),reverb=gabi::call<s32>(0x02520540,room);
        gabi::call(0x025E1A40,0x69F8,gabi::at<void>(a+0x314),0,reverb);
        gabi::call(0x021F4544,this,5); gabi::call(0x021F4558,this);
    }
}
VERIFY(0x021F4628,&daNpc_Bj1_c::setMtx_anmProc);

/* Matrix assignment captures all source words first, then quiets lfs/stfs NaNs;
 * storeOrder preserves the actual reference's permutation of independent words. */
static void bj1_storeMatrixOrdered(u32 destination,u32 source,const u8* storeOrder) {
    u32 words[12];
    for (int i=0;i<12;++i) words[i]=gabi::load<u32>(source+4u*i);
    for (int j=0;j<12;++j) {
        u32 i=storeOrder[j],word=words[i];
        if ((word&0x7F800000u)==0x7F800000u && (word&0x003FFFFFu)!=0 && !(word&0x00400000u)) word|=0x00400000u;
        gmem_stf32(destination+4u*i,word);
    }
}
static u32 bj1_jointMatrix(u32 actor,u32 morphOffset,u32 jointOffset) {
    u32 morph=gabi::load<u32>(actor+morphOffset);
    s32 joint=gabi::load<s8>(actor+jointOffset);
    u32 model=gabi::load<u32>(morph+0x90),block=gabi::load<u32>(model+0x2C);
    u32 matrices=gabi::load<u32>(block+0x10);
    u16 flags=gabi::load<u16>(block+4);
    gabi::store<u16>(block+4,(u16)(flags|0x10));
    return matrices+(u32)joint*48u;
}
void daNpc_Bj1_c::setPrtcl_peraProOpen() {
    WWHD_FUNC(0x021F4798,void,this);
    u32 a=gabi::ea(this),matrix=bj1_jointMatrix(a,0x7F8,0x7FC);
    gabi::call(0x028E90D4,gabi::at<void>(matrix),gabi::at<void>(0x1048D0CC));
    gabi::store<f32>(a+0x97C,gabi::load<f32>(0x1048D0D8));
    gabi::store<f32>(a+0x980,gabi::load<f32>(0x1048D0E8));
    f32 z=gabi::load<f32>(0x1048D0F8);
    s32 room=gabi::load<s8>(a+0x326);
    gabi::store<f32>(a+0x984,z);
    u32 scene=gabi::call<u32>(0x025200D4),control=gabi::load<u32>(scene+0x5AB0);
    u32 emitter=gabi::call<u32>(0x025A847C,gabi::at<void>(control),0,0x830B,gabi::at<void>(a+0x97C),gabi::at<void>(a+0x320),(void*)nullptr,255,(void*)nullptr,room,(void*)nullptr,(void*)nullptr,(void*)nullptr);
    gabi::store<u32>(a+0x988,emitter);
}
VERIFY(0x021F4798,&daNpc_Bj1_c::setPrtcl_peraProOpen);

void daNpc_Bj1_c::setMtx(int initializing) {
    WWHD_FUNC(0x021F4974,void,this,initializing);
    u32 a=gabi::ea(this),stack=0x1048D0CC;
    if (gabi::load<u8>(a+0x967)==0) {
        u32 mode=0;
        if (gabi::load<u32>(a+0x478)&0x20u) {
            u32 scene=gabi::call<u32>(0x025200D4);
            mode=gabi::call<u32>(0x024EECAC,gabi::at<void>(scene+0x12A0),gabi::at<void>(a+0x538));
        }
        s32 room=gabi::load<s8>(a+0x326),reverb=gabi::call<s32>(0x02520540,room);
        u32 morph=gabi::load<u32>(a+0x44C);
        u32 result=gabi::call<u32>(0x025E535C,gabi::at<void>(morph),gabi::at<void>(a+0x37C),mode,reverb);
        morph=gabi::load<u32>(a+0x44C);
        f32 previous=gabi::load<f32>(a+0x918);
        gabi::store<u8>(a+0x94C,(u8)result);
        f32 frame=gabi::load<f32>(morph+0x9C);
        if (frame<previous) {morph=gabi::load<u32>(a+0x44C);gabi::store<u8>(a+0x94C,1);}
        gabi::store<f32>(a+0x918,gabi::load<f32>(morph+0x9C));
        gabi::call(0x021F4628,this);
        gabi::call(0x025E535C,gabi::at<void>(gabi::load<u32>(a+0x7F8)),gabi::at<void>(a+0x37C),0,0);
        if (gabi::load<u8>(a+0x95D)!=0) {gabi::call(0x021F4798,this);gabi::store<u8>(a+0x95D,0);}
        u32 scene=gabi::call<u32>(0x025200D4);
        gabi::call(0x024F08A8,gabi::at<void>(a+0x450),gabi::at<void>(scene+0x12A0));
    }
    u32 scene=gabi::call<u32>(0x025200D4);
    u32 first=gabi::call<u32>(0x024EF130,gabi::at<void>(scene+0x12A0),gabi::at<void>(a+0x538));
    gabi::store<u8>(a+0x1C9,(u8)first);
    scene=gabi::call<u32>(0x025200D4);
    u32 second=gabi::call<u32>(0x024EEEB8,gabi::at<void>(scene+0x12A0),gabi::at<void>(a+0x538));
    f32 x=gabi::load<f32>(a+0x314),y=gabi::load<f32>(a+0x318),z=gabi::load<f32>(a+0x31C);
    gabi::store<u8>(a+0x1CA,(u8)second);
    gabi::call(0x028E93CC,gabi::at<void>(stack),x,y,z);
    s32 yaw=gabi::load<s16>(a+0x322);
    gabi::call(0x025F1C28,gabi::at<void>(stack),yaw);
    u32 morph=gabi::load<u32>(a+0x44C),model=gabi::load<u32>(morph+0x90);
    static const u8 bodyOrder[12]={0,4,7,9,2,5,3,8,1,10,6,11};
    bj1_storeMatrixOrdered(model+0xC8,stack,bodyOrder);
    gabi::call(0x025E55A0,gabi::at<void>(gabi::load<u32>(a+0x44C)));
    gabi::call(0x025E55A0,gabi::at<void>(gabi::load<u32>(a+0x7F8)));
    static const u8 attachmentOrder[12]={2,7,5,9,4,10,6,0,3,1,8,11};
    if (gabi::load<u32>(a+0x7EC)!=0) {
        u32 matrix=bj1_jointMatrix(a,0x44C,0x7E6);
        gabi::call(0x028E90D4,gabi::at<void>(matrix),gabi::at<void>(stack));
        u32 destination=gabi::load<u32>(a+0x7EC);
        bj1_storeMatrixOrdered(destination+0xC8,stack,attachmentOrder);
        gabi::call(0x027F4D5C,gabi::at<void>(gabi::load<u32>(a+0x7EC)));
    }
    if (gabi::load<u32>(a+0x7F0)!=0) {
        u32 matrix=bj1_jointMatrix(a,0x44C,0x7E7);
        gabi::call(0x028E90D4,gabi::at<void>(matrix),gabi::at<void>(stack));
        u32 destination=gabi::load<u32>(a+0x7F0);
        bj1_storeMatrixOrdered(destination+0xC8,stack,attachmentOrder);
        gabi::call(0x027F4D5C,gabi::at<void>(gabi::load<u32>(a+0x7F0)));
    }
    u32 matrix=bj1_jointMatrix(a,0x44C,0x7E4);
    gabi::call(0x028E90D4,gabi::at<void>(matrix),gabi::at<void>(stack));
    u32 destination=gabi::load<u32>(a+0x7E8);
    bj1_storeMatrixOrdered(destination+0xC8,stack,attachmentOrder);
    gabi::call(0x027F4D5C,gabi::at<void>(gabi::load<u32>(a+0x7E8)));
    gabi::call(0x021F4864,this);
    gabi::call(0x021F4914,this,initializing);
}
VERIFY(0x021F4974,&daNpc_Bj1_c::setMtx);

// Proposal only; append after root review. Native raw argument order preserved.
BOOL daNpc_Bj1_c::partner_srch_sub(u32 callback) {
    WWHD_FUNC(0x021F5450, BOOL, this, callback);
    u32 a=gabi::ea(this);
    gabi::store<u32>(a+0x8B8,0xFFFFFFFFu);
    gabi::store<u32>(0x10465D78,0);
    for (u32 i=0;i<20;++i) gabi::store<u32>(0x1046617C+4*i,0);
    gabi::call(0x025DE508,gabi::at<void>(callback),this);
    if (gabi::load<u32>(0x10465D78)==0) return 0;
    u32 actor=gabi::load<u32>(0x1046617C);
    u32 id=actor ? gabi::load<u32>(actor+4) : 0xFFFFFFFFu;
    gabi::store<u32>(a+0x8B8,id);
    return 1;
}
VERIFY(0x021F5450,&daNpc_Bj1_c::partner_srch_sub);

void daNpc_Bj1_c::partner_srch() {
    WWHD_FUNC(0x021F54FC, void, this);
    u32 a=gabi::ea(this);
    if (gabi::load<s8>(a+0x9C6)==1 && gabi::load<s8>(a+0x9C5)==4) {
        // Retain an observed callee boundary; do not inline callback enumeration.
        u32 result=gabi::call<u32>(0x021F5450,this,0x021F2D78u);
        if (result!=0) {
            u8 phase=gabi::load<u8>(a+0x9C6);
            gabi::store<u8>(a+0x9C6,(u8)(phase+1));
        }
    }
}
VERIFY(0x021F54FC,&daNpc_Bj1_c::partner_srch);

void daNpc_Bj1_c::bj_clcMovSpd() {
    WWHD_FUNC(0x021F7604, void, this);
    u32 a=gabi::ea(this);
    if (gabi::load<s8>(a+0x9B9)==0) {
        s32 direction=gabi::call<s32>(0x0200F93C,gabi::at<void>(a+0x314),gabi::at<void>(a+0x8E8));
        s32 type=gabi::load<s8>(a+0x9C4);
        u32 prm=0x10465DB0u+(u32)(type*108);
        s32 maximum=gabi::load<s16>(prm+0x54);
        s32 scale=gabi::load<s16>(prm+0x52);
        gabi::call(0x0200F378,gabi::at<void>(a+0x322),direction,scale,maximum,0);
    }
    // Match PPC load order: +924 precedes +91C; FPR order is target then step.
    f32 step=gabi::load<f32>(a+0x924);
    f32 target=gabi::load<f32>(a+0x91C);
    gabi::call(0x0200F5C8,gabi::at<void>(a+0x370),target,step);
}
VERIFY(0x021F7604,&daNpc_Bj1_c::bj_clcMovSpd);

// Returns the demo flag +0x967 (021F56F4 lbz r3); 0 when no demo actor is set (021F5670). The caller (execute, 021F...: see
// d_a_npc_bj1.cpp, call to 0x021F563C) tests it: a void declaration left r3 = this, so the Korok talk in Ocrogh never started (game test).
BOOL daNpc_Bj1_c::demo() {
    WWHD_FUNC(0x021F563C, BOOL, this);
    u32 a=gabi::ea(this);
    if (gabi::load<u8>(a+0x2DC)==0) {
        if (gabi::load<u8>(a+0x967)!=0) gabi::store<u8>(a+0x967,0);
        return 0;
    }
    u32 demoId=gabi::load<u8>(a+0x2DC);
    gabi::store<u8>(a+0x967,1);
    if (demoId!=0 && demoId<=32) {
        u32 manager=gabi::load<u32>(0x101D5FFC);
        if (manager==0) {
            gabi::call(0x0273AA24,gabi::at<void>(0x10016BF8),0x23A,gabi::at<void>(0x10016BDC));
            manager=gabi::load<u32>(0x101D5FFC);
        }
        gabi::call(0x02526E70,gabi::at<void>(manager),demoId);
    }
    u32 morph=gabi::load<u32>(a+0x44C);
    gabi::call(0x02527028,this,0x6A,gabi::at<void>(morph),gabi::at<void>(0x10016E1E),0,0,0,0);
    return gabi::load<u8>(a+0x967);
}
VERIFY(0x021F563C,&daNpc_Bj1_c::demo);

void daNpc_Bj1_c::setCollision_SP_() {
    WWHD_FUNC(0x021F78B0, void, this);
    u32 a=gabi::ea(this);
    gabi::call(0x020182E0,gabi::at<void>(a+0x7A8),gabi::at<void>(a+0x314));
    s32 type=gabi::load<s8>(a+0x9C4);
    u32 offset=(u32)type<<3;
    f32 height=gabi::load<f32>(0x101BBEA0u+offset+4);
    gabi::call(0x020184DC,gabi::at<void>(a+0x7A8),height);
    type=gabi::load<s8>(a+0x9C4); // native reload after first setter
    offset=(u32)type<<3;
    f32 radius=gabi::load<f32>(0x101BBEA0u+offset);
    gabi::call(0x02018428,gabi::at<void>(a+0x7A8),radius);
    u32 scene=gabi::call<u32>(0x025200D4);
    gabi::call(0x0200E240,gabi::at<void>(scene+0x26A4),gabi::at<void>(a+0x690));
}
VERIFY(0x021F78B0,&daNpc_Bj1_c::setCollision_SP_);

void daNpc_Bj1_c::bj_clcFlySpd() {
    WWHD_FUNC(0x021F6E20, void, this);
    u32 a=gabi::ea(this);
    s32 state=gabi::load<s8>(a+0x9B8);
    if (state==1) {
        s32 direction=gabi::call<s32>(0x0200F93C,gabi::at<void>(a+0x314),gabi::at<void>(a+0x8E8));
        s32 type=gabi::load<s8>(a+0x9C4);
        u32 prm=0x10465DB0u+(u32)(type*108);
        s32 maximum=gabi::load<s16>(prm+0x28);
        s32 scale=gabi::load<s16>(prm+0x26);
        gabi::call(0x0200F378,gabi::at<void>(a+0x322),direction,scale,maximum,0);
        type=gabi::load<s8>(a+0x9C4);
        f32 currentY=gabi::load<f32>(a+0x318);
        f32 targetY=gabi::load<f32>(a+0x8EC);
        prm=0x10465DB0u+(u32)(type*108);
        f32 delta=gabi::fsubs_ppc(targetY,currentY);
        f32 limit=gabi::load<f32>(prm+0x2C);
        bool rising=delta>limit; // PPC GT bit: unordered yields false
        gabi::store<u8>(a+0x958,(u8)rising);
        f32 target=gabi::load<f32>(prm+0x30);
        f32 step=gabi::load<f32>(prm+0x34);
        if (!rising) target=-target;
        gabi::call(0x0200F5C8,gabi::at<void>(a+0x340),target,step);
    } else if (state==2) {
        s32 type=gabi::load<s8>(a+0x9C4);
        u32 prm=0x10465DA4u+(u32)(type*108);
        f32 target=gabi::load<f32>(prm+0x3C);
        f32 step=gabi::load<f32>(prm+0x40);
        gabi::call(0x0200F5C8,gabi::at<void>(a+0x340),-target,-step);
    }
    f32 step=gabi::load<f32>(a+0x924);
    f32 target=gabi::load<f32>(a+0x91C);
    gabi::call(0x0200F5C8,gabi::at<void>(a+0x370),target,step);
}
VERIFY(0x021F6E20,&daNpc_Bj1_c::bj_clcFlySpd);

BOOL daNpc_Bj1_c::chk_drct(f32 range) {
    WWHD_FUNC(0x021F80F8, BOOL, this, range);
    u32 a=gabi::ea(this);
    u32 scene=gabi::call<u32>(0x025200D4);
    u32 player=gabi::load<u32>(scene+0x5B2C);
    s32 direction=gabi::call<s32>(0x0200F93C,gabi::at<void>(a+0x314),gabi::at<void>(player+0x314));
    f32 scale=gabi::load<f32>(0x10016EE0);
    s32 facing=gabi::load<s16>(a+0x322);
    // Preserve fmuls frA/frC order and fctiwz saturation/invalid behavior.
    f32 scaled=gabi::fmuls_ppc(range,scale);
    s32 delta=(s16)(direction-facing);
    s32 converted=(s32)(u32)ppc_fctiwz((f64)scaled);
    s32 threshold=(s16)(u16)converted;
    s32 magnitude=delta<0 ? -delta : delta;
    return magnitude<threshold;
}
VERIFY(0x021F80F8,&daNpc_Bj1_c::chk_drct);

u32 daNpc_Bj1_c::eInit_calcRelativPos(cXyz* destination,cXyz* relative,void* angles) {
    WWHD_FUNC(0x021F5794,u32,this,destination,relative,angles);
    u32 a=gabi::ea(this),dst=gabi::ea(destination),rel=gabi::ea(relative),ang=gabi::ea(angles);
    s32 yaw;
    if (ang!=0) {
        s32 extra=gabi::load<s16>(ang+2);
        s32 base=gabi::load<s16>(a+0x32A);
        yaw=(s16)(base+extra);
    } else yaw=gabi::load<s16>(a+0x32A);
    // Contiguous input/output guest stack buffers preserve native callee argument ABI.
    struct RelativeFrame { u8 bytes[24]; };
    gabi::Local<RelativeFrame> frame;
    u32 input=gabi::ea(frame.get()),output=input+12;
    f32 z=gabi::load<f32>(a+0x31C),x,y;
    if (rel!=0) {
        f32 rx=gabi::load<f32>(rel);
        x=gabi::load<f32>(a+0x314);
        f32 ry=gabi::load<f32>(rel+4);
        gabi::store<f32>(input,rx);gabi::store<f32>(input+4,ry);
        f32 rz=gabi::load<f32>(rel+8);
        y=gabi::load<f32>(a+0x318);
        gabi::store<f32>(input+8,rz);
    } else {
        y=gabi::load<f32>(a+0x318);
        f32 zero=gabi::load<f32>(0x10016C08);
        x=gabi::load<f32>(a+0x314);
        gabi::store<f32>(input,zero);gabi::store<f32>(input+8,zero);gabi::store<f32>(input+4,zero);
    }
    gabi::call(0x028E93CC,gabi::at<void>(0x1048D0CC),x,y,z);
    gabi::call(0x025F1C28,gabi::at<void>(0x1048D0CC),yaw);
    gabi::call(0x028E8F64,gabi::at<void>(0x1048D0CC),gabi::at<void>(input),gabi::at<void>(output));
    if (dst==0) {dst=gabi::call<u32>(0x0273AD10,12);if (dst==0) return 0;}
    for (u32 i=0;i<3;++i) {f32 v=gabi::load<f32>(output+4*i);gabi::store<f32>(dst+4*i,v);}
    return dst;
}
VERIFY(0x021F5794,&daNpc_Bj1_c::eInit_calcRelativPos);

void daNpc_Bj1_c::bj_nMove() {
    WWHD_FUNC(0x021F7684,void,this);
    u32 a=gabi::ea(this);
    if (gabi::load<u8>(a+0x95A)!=0) {gabi::store<u8>(a+0x957,1);return;}
    s32 state=gabi::load<s8>(a+0x9B9);
    if (state==1) {gabi::store<u8>(a+0x9B9,2);return;}
    if (state==2) {
        if ((gabi::load<u32>(a+0x478)&0x20)==0) return;
        gabi::call(0x021F4544,this,8);
        gabi::store<u8>(a+0x9B9,0);
        f32 speed=gabi::load<f32>(0x10016C08),fall=gabi::load<f32>(0x10016DE0);
        gabi::store<f32>(a+0x370,speed);gabi::store<f32>(a+0x374,fall);
        return;
    }
    if (gabi::load<s8>(a+0x9BA)!=1 || state!=0) return;
    gabi::call(0x021F7604,this);
    s32 type=gabi::load<s8>(a+0x9C4);
    f32 speed=gabi::load<f32>(a+0x370);
    f32 scale=gabi::load<f32>(0x10465E08u+(u32)(type*108));
    f32 animationSpeed=gabi::fmuls_ppc(speed,scale);
    f32 minimum=gabi::load<f32>(0x10016EC8);
    u32 morph=gabi::load<u32>(a+0x44C);
    if (animationSpeed<minimum) animationSpeed=minimum;
    gabi::store<f32>(morph+0x98,animationSpeed);
    u32 result=gabi::call<u32>(0x021F6F40,this,0);
    if (result==1) {gabi::store<u8>(a+0x95A,1);return;}
    if (result==2) {gabi::store<u8>(a+0x9BA,0);gabi::store<u8>(a+0x95A,1);}
}
VERIFY(0x021F7684,&daNpc_Bj1_c::bj_nMove);

void daNpc_Bj1_c::event_proc(int argument) {
    WWHD_FUNC(0x021F6CC4,void,this,argument);
    u32 a=gabi::ea(this);
    s32 index=gabi::load<s16>(a+0x93C);
    s32 event=gabi::load<s16>(a+0x936+((u32)index<<1));
    u32 scene=gabi::call<u32>(0x025200D4);
    u32 ended=gabi::call<u32>(0x025440C8,gabi::at<void>(scene+0x52C4),event);
    if (ended!=0) {
        u32 current=(u32)(s32)gabi::load<s16>(a+0x93C);
        if (current==0) {gabi::store<u8>(a+0x9C0,1);gabi::store<u8>(a+0x952,1);}
        else if (current==1) {gabi::store<u8>(a+0x9C0,1);gabi::store<u8>(a+0x953,1);}
        else if (current==2) {
            u32 save=gabi::load<u32>(0x101F84DC);
            gabi::call(0x025B8B68,gabi::at<void>(save+0x644),0x2902);
        }
        gabi::call(0x021F5754,this);
        return;
    }
    u32 processed=gabi::call<u32>(0x0259F858,gabi::at<void>(a+0x84C));
    if (processed==0) gabi::call(0x021F6980,this,argument);
    gabi::call(0x021F6A54,this);
}
VERIFY(0x021F6CC4,&daNpc_Bj1_c::event_proc);

BOOL daNpc_Bj1_c::flyMov() {
    WWHD_FUNC(0x021F9728,BOOL,this);
    u32 a=gabi::ea(this);
    s32 state=gabi::load<s8>(a+0x9B8);
    gabi::store<u8>(a+0x9C0,0);gabi::store<u8>(a+0x959,0);
    if (state!=1) return 1;
    if (gabi::call<u32>(0x02055B64,gabi::at<void>(a+0x93E))!=0) return 1;
    struct FlyFrame {u8 bytes[24];};gabi::Local<FlyFrame> frame;
    u32 horizontal=gabi::ea(frame.get()),difference=horizontal+12;
    u32 scene=gabi::call<u32>(0x025200D4),player=gabi::load<u32>(scene+0x5B2C);
    gabi::call(0x0201ADE0,gabi::at<void>(a+0x314),gabi::at<void>(difference),gabi::at<void>(player+0x314));
    f32 x=gabi::load<f32>(difference),zero=gabi::load<f32>(0x10016C08);
    gabi::store<f32>(horizontal,x);
    f32 z=gabi::load<f32>(difference+8);
    gabi::store<f32>(horizontal+4,zero);gabi::store<f32>(horizontal+8,z);
    f64 squared=gabi::call<f64>(0x028E8DD0,gabi::at<void>(horizontal));
    f64 distance=gabi::call<f64>(0x028F4384,squared);
    scene=gabi::call<u32>(0x025200D4);player=gabi::load<u32>(scene+0x5B2C);
    s32 heading=gabi::call<s32>(0x0200F93C,gabi::at<void>(a+0x314),gabi::at<void>(player+0x314));
    s32 type=gabi::load<s8>(a+0x9C4),facing=gabi::load<s16>(a+0x322);
    s32 delta=(s16)(heading-facing);
    u32 prm=0x10465DB0u+(u32)(type*108);
    f32 radius=gabi::load<f32>(prm+0x4C);
    s32 magnitude=(s16)(delta<0 ? -delta : delta);
    if (distance<(f64)radius) {
        s32 limit=gabi::load<s16>(prm+0x50);
        if (magnitude<limit) gabi::call(0x021F919C,this,4);
    }
    return 1;
}
VERIFY(0x021F9728,&daNpc_Bj1_c::flyMov);

BOOL daNpc_Bj1_c::createSeed() {
    WWHD_FUNC(0x021F65B8,BOOL,this);
    u32 a=gabi::ea(this);
    struct SeedFrame {u8 bytes[56];};gabi::Local<SeedFrame> frame;
    u32 base=gabi::ea(frame.get()),angles=base,scale=base+8,position=base+20,choices=base+32,results=base+40;
    for (u32 i=0;i<4;++i) {gabi::store<u32>(results+4*i,0);gabi::store<u32>(a+0x9A8+4*i,0xFFFFFFFFu);}
    u32 morph=gabi::load<u32>(a+0x44C);
    s32 joint=gabi::load<s8>(a+0x7E7);
    u32 model=gabi::load<u32>(morph+0x90),block=gabi::load<u32>(model+0x2C);
    u32 matrices=gabi::load<u32>(block+0x10);
    u16 flags=gabi::load<u16>(block+4);
    gabi::store<u16>(block+4,(u16)(flags|0x10));
    gabi::call(0x028E90D4,gabi::at<void>(matrices+(u32)(joint*48)),gabi::at<void>(0x1048D0CC));
    f32 px=gabi::load<f32>(0x1048D0D8);
    f32 addY=gabi::load<f32>(0x10016EA8);
    f32 randomX=gabi::load<f32>(0x10016C18);
    f32 pz=gabi::load<f32>(0x1048D0F8);
    f32 randomY=gabi::load<f32>(0x10016EAC);
    gabi::store<f32>(position,px);
    f32 speed=gabi::load<f32>(0x10016EA4);
    f32 uniform=gabi::load<f32>(0x10016EA0);
    f32 py=gabi::load<f32>(0x1048D0E8);
    gabi::store<f32>(position+8,pz);
    f32 addX=gabi::load<f32>(0x10016EB0);
    gabi::store<f32>(position+4,py);
    u32 made=0;
    for (u32 i=0;i<4;++i) {
        gabi::store<s16>(choices,-0x1000);gabi::store<s16>(choices+6,0x1000);
        gabi::store<s16>(choices+2,-0xA00);gabi::store<s16>(choices+4,0xA00);
        gabi::call(0x0201A478,gabi::at<void>(angles),0,0,0);
        s32 selected=gabi::load<s16>(choices+2*i),facing=gabi::load<s16>(a+0x322);
        gabi::store<f32>(scale+4,uniform);gabi::store<f32>(scale,uniform);gabi::store<f32>(scale+8,uniform);
        gabi::store<s16>(angles+2,(s16)(facing+selected));
        f64 rx=gabi::call<f64>(0x02019918,randomX);
        f64 ry=gabi::call<f64>(0x02019918,randomY);
        f32 vy=(f32)(ry+(f64)addY),vx=(f32)(rx+(f64)addX);
        s32 room=gabi::load<s8>(a+0x326);
        u32 actor=gabi::call<u32>(0x025D8E6C,gabi::at<void>(position),0x46,room,gabi::at<void>(angles),gabi::at<void>(scale),1,vx,vy,speed);
        gabi::store<u32>(results+4*i,actor);
        if (actor==0) break;
        u32 actorFlags=gabi::load<u32>(actor+0x2E0);
        gabi::store<u32>(actor+0x2E0,actorFlags|0x4000);
        actor=gabi::load<u32>(results+4*i);
        u32 id=actor ? gabi::load<u32>(actor+4) : 0xFFFFFFFFu;
        ++made;gabi::store<u32>(a+0x9A8+4*i,id);
    }
    return made==4;
}
VERIFY(0x021F65B8,&daNpc_Bj1_c::createSeed);

u32 daNpc_Bj1_c::bj_movPass(int updateHeight) {
    WWHD_FUNC(0x021F6F40,u32,this,updateHeight);
    u32 a=gabi::ea(this),oldIndex=0,result=0;
    struct PassFrame {u8 bytes[36];};gabi::Local<PassFrame> frame;
    u32 vector=gabi::ea(frame.get()),position=vector+12,horizontal=vector+24;
    u32 path=gabi::load<u32>(a+0x844);
    bool loop=false;
    if (path!=0) {
        u8 flags=gabi::load<u8>(path+5);
        oldIndex=gabi::load<u8>(a+0x849);
        loop=(flags&1)!=0;
    }
    if (loop) {
        f32 x=gabi::load<f32>(a+0x314);
        u32 flag=gabi::load<u8>(a+0x84A);
        gabi::store<f32>(position,x);
        f32 y=gabi::load<f32>(a+0x318),z=gabi::load<f32>(a+0x31C);
        gabi::store<f32>(position+4,y);gabi::store<f32>(position+8,z);
        u32 crossed=gabi::call<u32>(0x0259E838,gabi::at<void>(a+0x844),gabi::at<void>(position),(u32)(flag!=0));
        if (crossed==0) return 0;
        gabi::call(0x0259ED58,gabi::at<void>(a+0x844));
        result=1;
        if (updateHeight==0) return result;
    } else {
        gabi::call(0x0201ADE0,gabi::at<void>(a+0x8E8),gabi::at<void>(vector),gabi::at<void>(a+0x314));
        f32 x=gabi::load<f32>(vector),zero=gabi::load<f32>(0x10016C08);
        gabi::store<f32>(horizontal,x);
        f32 z=gabi::load<f32>(vector+8);
        gabi::store<f32>(horizontal+4,zero);gabi::store<f32>(horizontal+8,z);
        f64 squared=gabi::call<f64>(0x028E8DD0,gabi::at<void>(horizontal));
        f64 distance=gabi::call<f64>(0x028F4384,squared);
        f32 threshold=gabi::load<f32>(a+0x92C);
        if (distance>(f64)threshold) return 0; // unordered continues
        path=gabi::load<u32>(a+0x844);
        result=1;
        if (path==0) return result;
        if (gabi::call<u32>(0x0259ED58,gabi::at<void>(a+0x844))==0) result=2;
        if (updateHeight==0) return result;
    }
    u32 currentIndex=gabi::load<u8>(a+0x849);
    gabi::call(0x0259E778,gabi::at<void>(a+0x844),gabi::at<void>(vector),currentIndex);
    f32 firstY=gabi::load<f32>(vector+4);
    gabi::call(0x0259E778,gabi::at<void>(a+0x844),gabi::at<void>(vector),oldIndex);
    f32 secondY=gabi::load<f32>(vector+4);
    gabi::store<u8>(a+0x958,(u8)!(secondY>firstY));
    return result;
}
VERIFY(0x021F6F40,&daNpc_Bj1_c::bj_movPass);

void daNpc_Bj1_c::lookBack() {
    WWHD_FUNC(0x021F6A54,void,this);
    u32 a=gabi::ea(this);
    u32 mode=(u32)(s32)gabi::load<s8>(a+0x9C3);
    s32 heading=gabi::load<s16>(a+0x322);
    f32 zero=gabi::load<f32>(0x10016C08);
    gabi::store<s16>(a+0x934,(s16)heading);
    s32 roll=gabi::load<s16>(a+0x3B2);
    struct LookFrame {u8 bytes[36];};gabi::Local<LookFrame> frame;
    u32 target=gabi::ea(frame.get()),origin=target+12,scratch=target+24;
    gabi::store<f32>(target+8,zero);gabi::store<s16>(a+0x932,(s16)roll);
    s32 pitch=gabi::load<s16>(a+0x3AE);
    gabi::store<f32>(target,zero);
    f32 y=gabi::load<f32>(a+0x380);
    gabi::store<f32>(target+4,zero);
    f32 x=gabi::load<f32>(a+0x314);
    u32 flag=gabi::load<u8>(a+0x966);
    f32 z=gabi::load<f32>(a+0x31C);
    gabi::store<s16>(a+0x930,(s16)pitch);
    u32 targetArg=0;
    if (mode==1) {
        f32 distance=gabi::load<f32>(0x10016E10);
        gabi::call(0x0259D54C,gabi::at<void>(scratch),distance);
        s32 type=gabi::load<s8>(a+0x9C4);
        u32 tz=gabi::load<u32>(scratch+8);
        x=gabi::load<f32>(a+0x314);
        u32 ty=gabi::load<u32>(scratch+4);
        gabi::store<u32>(target+8,tz);
        z=gabi::load<f32>(a+0x31C);y=gabi::load<f32>(a+0x380);
        gabi::store<u32>(target+4,ty);
        u32 tx=gabi::load<u32>(scratch);
        s32 limit=gabi::load<s16>(0x10465DC6u+(u32)(type*108));
        gabi::store<u32>(target,tx);targetArg=target;
        gabi::call(0x0200F428,gabi::at<void>(a+0x948),limit,4,0x800);
    } else if (mode==2) {
        u32 tz=gabi::load<u32>(a+0x8E4);
        x=gabi::load<f32>(a+0x314);gabi::store<u32>(target+8,tz);
        s32 type=gabi::load<s8>(a+0x9C4);
        u32 tx=gabi::load<u32>(a+0x8DC),ty=gabi::load<u32>(a+0x8E0);
        gabi::store<u32>(target+4,ty);gabi::store<u32>(target,tx);
        s32 limit=gabi::load<s16>(0x10465DC6u+(u32)(type*108));
        z=gabi::load<f32>(a+0x31C);targetArg=target;
        gabi::call(0x0200F428,gabi::at<void>(a+0x948),limit,4,0x800);
    } else {
        if (mode==3) heading=gabi::load<s16>(a+0x94A);
        s32 type=gabi::load<s8>(a+0x9C4);
        s32 limit=gabi::load<s16>(0x10465DC6u+(u32)(type*108));
        gabi::call(0x0200F428,gabi::at<void>(a+0x948),limit,4,0x800);
    }
    u32 enabled=gabi::load<u8>(a+0x3B6);
    s32 limit;
    if (enabled==0) {gabi::store<s16>(a+0x948,0);limit=0;}
    else limit=gabi::load<s16>(a+0x948);
    gabi::store<f32>(origin,x);gabi::store<f32>(origin+4,y);gabi::store<f32>(origin+8,z);
    gabi::call(0x0259DED0,gabi::at<void>(a+0x3AC),gabi::at<void>(a+0x322),gabi::at<void>(targetArg),gabi::at<void>(origin),heading,limit,flag);
}
VERIFY(0x021F6A54,&daNpc_Bj1_c::lookBack);

u32 daNpc_Bj1_c::anmNum_toResID(int index) {
    WWHD_FUNC(0x021F4330,u32,this,index);
    return gabi::load<u32>(0x10016DA0u+((u32)index<<2));
}
VERIFY(0x021F4330,&daNpc_Bj1_c::anmNum_toResID);
void daNpc_Bj1_c::eInit_setLocFlag(int* value) {
    WWHD_FUNC(0x021F590C,void,this,value);
    u32 a=gabi::ea(this),v=gabi::ea(value);
    gabi::store<u8>(a+0x966,0);
    if(v==0)return;
    u32 flag=gabi::load<u32>(v);
    if(flag==1)gabi::store<u8>(a+0x966,1);
    else if(flag==2)gabi::store<u8>(a+0x3B6,1);
}
VERIFY(0x021F590C,&daNpc_Bj1_c::eInit_setLocFlag);
static BOOL bj1_moveStateIsZero(daNpc_Bj1_c* actor) {
    WWHD_FUNC(0x021F65A4,BOOL,actor);
    return gabi::load<s8>(gabi::ea(actor)+0x9B9)==0;
}
VERIFY(0x021F65A4,&bj1_moveStateIsZero);

// Local qualified name: actual omitted entry, donor symbol absent.
static u32 bj1_drugpotCleanupEntry(u32 actor) {
    WWHD_FUNC(0x021F5364, u32, actor);
    // Reload each actor pointer after the previous cleanup stores, preserving aliases.
    for (u32 offset = 0x98C; offset != 0x998; offset += 4) {
        u32 pot = gabi::load<u32>(actor + offset);
        if (pot != 0) {
            u32 flags = gabi::load<u32>(pot + 0x254);
            gabi::store<u32>(pot + 0x5C, 0xFFFFFFFFu);
            gabi::store<u32>(pot + 0x254, flags | 1u);
            gabi::store<u32>(actor + offset, 0);
        }
    }
    return actor; // r3 unchanged on every native path.
}
VERIFY(0x021F5364, &bj1_drugpotCleanupEntry);

fopAc_ac_c* daNpc_Bj1_c::searchByID(fpc_ProcID id) {
    WWHD_FUNC(0x021F58D8, fopAc_ac_c*, this, id);
    gabi::Local<be<u32>> result;
    *result = 0;
    gabi::call<void>(0x025D54C4, id, result.get());
    return gabi::at<fopAc_ac_c>(*result);
}
VERIFY(0x021F58D8, &daNpc_Bj1_c::searchByID);

static BOOL bj1_deleteSeedBody(daNpc_Bj1_c* self) {
    u32 actor = gabi::ea(self);
    for (u32 offset = 0; offset != 16; offset += 4) {
        u32 id = gabi::load<u32>(actor + 0x9A8 + offset);
        u32 found = gabi::call<u32>(0x021F58D8, self, id);
        if (found != 0) {
            gabi::store<u32>(actor + 0x9A8 + offset, 0xFFFFFFFFu);
            gabi::call<void>(0x025D57E0, found);
        }
    }
    return 1;
}
BOOL daNpc_Bj1_c::deleteSeed() {
    WWHD_FUNC(0x021F6040,BOOL,this);
    return bj1_deleteSeedBody(this);
}
VERIFY(0x021F6040, &daNpc_Bj1_c::deleteSeed);

// The actual four-byte tail branch dispatches6040 at the callee boundary.
static BOOL bj1_deleteSeedTailAlias(daNpc_Bj1_c* actor) {
    WWHD_FUNC(0x021F60A8, BOOL, actor);
    return gabi::call<BOOL>(0x021F6040,actor);
}
VERIFY(0x021F60A8, &bj1_deleteSeedTailAlias);

u32 daNpc_Bj1_c::eMove_MOV_() {
    WWHD_FUNC(0x021F6504, u32, this);
    u32 actor = gabi::ea(this);
    if (gabi::load<s16>(actor + 0x940) >= 0)
        return gabi::call<u32>(0x02055B64, gabi::at<void>(actor + 0x940)) == 0 ? 1u : 0u;
    return (u32)(gabi::load<u8>(actor + 0x3B6) ^ 1u);
}
VERIFY(0x021F6504, &daNpc_Bj1_c::eMove_MOV_);

u32 daNpc_Bj1_c::eMove_JMP_() {
    WWHD_FUNC(0x021F6558, u32, this);
    u32 actor = gabi::ea(this);
    if (gabi::load<s16>(actor + 0x940) >= 0)
        return gabi::call<u32>(0x02055B64, gabi::at<void>(actor + 0x940)) == 0 ? 1u : 0u;
    return gabi::load<u8>(actor + 0x95A);
}
VERIFY(0x021F6558, &daNpc_Bj1_c::eMove_JMP_);

void daNpc_Bj1_c::eInit_ATTENTION_(int* modePtr,int* loc,int* shape,cXyz* relative,int* angles,int* point,int* timer) {
    WWHD_FUNC(0x021F5998,void,this,modePtr,loc,shape,relative,angles,point,timer);
    u32 a=gabi::ea(this);
    if (!modePtr) return;
    u32 mode=gabi::load<u32>(gabi::ea(modePtr));
    s32 baseYaw=gabi::load<s16>(a+0x32A);
    struct AttentionFrame {u8 bytes[12];};gabi::Local<AttentionFrame> frame;
    u32 tmp=gabi::ea(frame.get());
    auto directionToPoint=[&]() -> s32 {return gabi::call<s32>(0x0200F93C,gabi::at<void>(a+0x314),gabi::at<void>(a+0x8DC));};
    // Chain the actual returned r3, rather than silently restoring this between callees.
    auto finish=[&](u32 carryAngle,bool explicitBase) {
        u32 carry=gabi::call<u32>(0x021F590C,this,loc,carryAngle);
        u32 angle=explicitBase ? (u32)baseYaw : gabi::cpu->r[5];
        carry=gabi::call<u32>(0x021F594C,gabi::at<void>(carry),shape,angle);
        gabi::call(0x021F597C,gabi::at<void>(carry),timer);
    };
    switch(mode) {
    case 0: finish(gabi::ea(relative),true);return;
    case 1: {
        gabi::store<u8>(a+0x9C3,1);
        u32 scene=gabi::call<u32>(0x025200D4),player=gabi::load<u32>(scene+0x5B2C);
        s32 direction=gabi::call<s32>(0x0200F93C,gabi::at<void>(a+0x314),gabi::at<void>(player+0x314));
        finish((u32)direction,false);return;
    }
    case 2: {
        if (!relative) return;
        gabi::store<u8>(a+0x9C3,2);
        s32 direction=gabi::call<s32>(0x0200F93C,gabi::at<void>(a+0x314),relative);
        finish((u32)direction,false);return;
    }
    case 3: {
        gabi::store<u8>(a+0x9C3,2);
        gabi::call(0x021F5794,this,gabi::at<void>(tmp),relative,angles);
        u32 z=gabi::load<u32>(tmp+8),y=gabi::load<u32>(tmp+4),x=gabi::load<u32>(tmp);
        gabi::store<u32>(a+0x8E0,y);gabi::store<u32>(a+0x8DC,x);gabi::store<u32>(a+0x8E4,z);
        finish((u32)directionToPoint(),false);return;
    }
    case 4: {
        if (!angles) return;
        gabi::store<u8>(a+0x9C3,3);
        s32 direction=gabi::load<s16>(gabi::ea(angles)+2);
        finish((u32)direction,false);return;
    }
    case 5: {
        if (gabi::load<u32>(a+0x844)==0) return;
        u32 index=gabi::load<u8>(a+0x849);
        if (point) index=gabi::load<u8>(gabi::ea(point)+3);
        gabi::call(0x0259E778,gabi::at<void>(a+0x844),gabi::at<void>(tmp),index);
        u32 carryAngle=gabi::cpu->r[5];
        u32 z=gabi::load<u32>(tmp+8),x=gabi::load<u32>(tmp);
        gabi::store<u8>(a+0x9C3,2);gabi::store<u32>(a+0x8DC,x);
        u32 y=gabi::load<u32>(tmp+4);
        gabi::store<u32>(a+0x8E4,z);gabi::store<u32>(a+0x8E0,y);
        finish(carryAngle,true);return;
    }
    case 6: {
        u32 id=gabi::load<u32>(a+0x8B8);
        u32 partner=gabi::call<u32>(0x021F58D8,this,id);
        if (partner==0) return;
        gabi::store<u8>(a+0x9C3,2);
        u32 x=gabi::load<u32>(partner+0x37C);gabi::store<u32>(a+0x8DC,x);
        u32 y=gabi::load<u32>(partner+0x380);gabi::store<u32>(a+0x8E0,y);
        u32 z=gabi::load<u32>(partner+0x384);gabi::store<u32>(a+0x8E4,z);
        finish((u32)directionToPoint(),false);return;
    }
    case 7: {
        f32 y=gabi::load<f32>(a+0x380);
        u32 x=gabi::load<u32>(a+0x970),z=gabi::load<u32>(a+0x978);
        gabi::store<f32>(a+0x8E0,y);gabi::store<u32>(a+0x8E4,z);
        gabi::store<u8>(a+0x9C3,2);gabi::store<u32>(a+0x8DC,x);
        finish((u32)directionToPoint(),false);return;
    }
    default:gabi::store<u8>(a+0x9C3,0);finish(gabi::ea(relative),true);return;
    }
}
VERIFY(0x021F5998,&daNpc_Bj1_c::eInit_ATTENTION_);

void daNpc_Bj1_c::event_actionInit(int event) {
    WWHD_FUNC(0x021F6114,void,this,event);
    u32 a=gabi::ea(this);
    auto get=[&](u32 name,int type) -> u32 {
        u32 scene=gabi::call<u32>(0x025200D4);
        return gabi::call<u32>(0x0254487C,gabi::at<void>(scene+0x52C4),event,gabi::at<void>(name),type);
    };
    u32 selector=get(0x10016E28,3);
    if(selector==0)return;
    u32 value=gabi::load<u32>(selector);
    s32 action=(s8)value;
    gabi::store<u32>(a+0x960,0);gabi::store<u8>(a+0x9BC,(u8)value);
    switch((u32)action) {
    case 0: {
        u32 p0=get(0x10016E30,3),p1=get(0x10016E38,3),p2=get(0x10016E40,3);
        u32 p3=get(0x10016E48,1),p4=get(0x10016E50,3),p5=get(0x10016E58,3),p6=get(0x10016E60,3);
        gabi::store<u32>(a+0x960,1);
        gabi::call(0x021F5998,this,gabi::at<void>(p0),gabi::at<void>(p1),gabi::at<void>(p2),gabi::at<void>(p3),gabi::at<void>(p4),gabi::at<void>(p5),gabi::at<void>(p6));return;
    }
    case 1:gabi::call(0x021F5C78,this);return;
    case 2:case 8: {
        u32 p0=get(0x10016E68,0),p1=get(0x10016E70,0),p2=get(0x10016E78,0),p3=get(0x10016E60,3);
        gabi::store<u32>(a+0x960,1);
        gabi::call(0x021F5D78,this,gabi::at<void>(p0),gabi::at<void>(p1),gabi::at<void>(p2),gabi::at<void>(p3));return;
    }
    case 3: {
        u32 p0=get(0x10016E70,0),p1=get(0x10016E80,0);
        gabi::store<u32>(a+0x960,1);gabi::call(0x021F5E48,this,gabi::at<void>(p0),gabi::at<void>(p1));return;
    }
    case 4: {
        u32 p0=get(0x10016E88,3),p1=get(0x10016E58,3);
        gabi::call(0x021F5EA4,this,gabi::at<void>(p0),gabi::at<void>(p1));return;
    }
    case 5:gabi::call(0x021F5FBC,this);return;
    case 6:gabi::call(0x021F6008,this);return;
    case 7:gabi::call(0x021F60A8,this);return;
    case 9: {
        u32 p0=get(0x10016E90,3),p1=get(0x10016E98,0);
        gabi::call(0x021F60AC,this,gabi::at<void>(p0),gabi::at<void>(p1));return;
    }
    default:return;
    }
}
VERIFY(0x021F6114,&daNpc_Bj1_c::event_actionInit);

BOOL daNpc_Bj1_c::bj_flyMove() {
    WWHD_FUNC(0x021F7270,BOOL,this);
    u32 a=gabi::ea(this);
    s32 state=gabi::load<s8>(a+0x9B8);
    if(state==0)return 0;
    if(gabi::load<u8>(a+0x959)!=0){gabi::store<u8>(a+0x957,1);return 1;}
    struct FlyMoveFrame{u8 bytes[12];};gabi::Local<FlyMoveFrame> frame;
    u32 tmp=gabi::ea(frame.get());
    if(gabi::load<u32>(a+0x844)!=0) {
        u32 index=gabi::load<u8>(a+0x849);
        gabi::call(0x0259E778,gabi::at<void>(a+0x844),gabi::at<void>(tmp),index);
        u32 x=gabi::load<u32>(tmp),z=gabi::load<u32>(tmp+8);
        gabi::store<u32>(a+0x8E8,x);
        u32 y=gabi::load<u32>(tmp+4);
        state=gabi::load<s8>(a+0x9B8);
        gabi::store<u32>(a+0x8EC,y);gabi::store<u32>(a+0x8F0,z);
    }
    if(state==1||state==2||state==7) {
        s32 type=gabi::load<s8>(a+0x9C4),angle=gabi::load<s16>(a+0x806);
        u32 prm=0x10465DB0u+(u32)(type*108);
        s32 step=gabi::load<s16>(prm+0x46),updated=(s16)(angle+step);
        s32 maximum=gabi::load<s16>(prm+0x44);
        if(maximum<updated)updated=maximum;
        state=gabi::load<s8>(a+0x9B8);
        gabi::store<s16>(a+0x806,(s16)updated);
    }
    f32 zero=gabi::load<f32>(0x10016C08);
    switch((u32)state) {
    case 1: {
        gabi::call(0x021F6E20,this);
        u32 result=gabi::call<u32>(0x021F6F40,this,1);
        if(result>=1&&result<=2)gabi::store<u8>(a+0x959,1);
        return 1;
    }
    case 2: {
        gabi::call(0x021F6E20,this);
        s32 type=gabi::load<s8>(a+0x9C4);
        f32 ground=gabi::load<f32>(a+0x4E4),height=gabi::load<f32>(a+0x318);
        f32 delta=gabi::fsubs_ppc(height,ground),threshold=gabi::load<f32>(0x10465DB0u+(u32)(type*108)+0x40);
        bool landed=delta<threshold;
        gabi::store<u8>(a+0x959,(u8)landed);
        if(landed){gabi::call(0x021F4544,this,1);gabi::store<f32>(a+0x340,zero);gabi::store<u8>(a+0x9B8,3);}return 1;
    }
    case 3: {
        s32 type=gabi::load<s8>(a+0x9C4),angle=gabi::load<s16>(a+0x806);
        s32 step=gabi::load<s16>(0x10465DB0u+(u32)(type*108)+0x46);
        s32 updated=(s16)(angle-step);if(updated<0)updated=0;
        bool done=updated==0;gabi::store<s16>(a+0x806,(s16)updated);gabi::store<u8>(a+0x959,(u8)done);
        if(done){gabi::call(0x021F70C0,this,2);gabi::store<u8>(a+0x9B8,4);}return 1;
    }
    case 4: {
        u32 morph=gabi::load<u32>(a+0x7F8);
        bool done=gabi::call<u32>(0x027F2BF8,gabi::at<void>(morph+0x98),zero)!=0;
        gabi::store<u8>(a+0x959,(u8)done);
        if(done){gabi::store<u8>(a+0x95C,0);f32 falling=gabi::load<f32>(0x10016DE0);gabi::store<u8>(a+0x9B8,5);gabi::store<f32>(a+0x374,falling);}return 1;
    }
    case 5: {
        bool done=(gabi::load<u32>(a+0x478)&0x20)!=0;
        gabi::store<u8>(a+0x959,(u8)done);if(done)gabi::store<u8>(a+0x9B8,0);return 1;
    }
    case 6: {
        u32 morph=gabi::load<u32>(a+0x7F8);
        u8 flags=gabi::load<u8>(morph+0xA7);
        bool done=(flags&1)!=0;
        if(!done)done=gabi::load<f32>(morph+0x98)==zero;
        gabi::store<u8>(a+0x959,(u8)done);if(done)gabi::store<u8>(a+0x9B8,7);return 1;
    }
    case 7: {
        s32 type=gabi::load<s8>(a+0x9C4),angle=gabi::load<s16>(a+0x806);
        s32 limit=gabi::load<s16>(0x10465DB0u+(u32)(type*108)+0x4A);
        bool ready=angle>limit;gabi::store<u8>(a+0x959,(u8)ready);
        if(ready){gabi::call(0x021F4544,this,10);type=gabi::load<s8>(a+0x9C4);s32 timer=gabi::load<s16>(0x10465DB0u+(u32)(type*108)+0x24);gabi::store<u8>(a+0x9B8,1);gabi::store<s16>(a+0x93E,(s16)timer);}return 1;
    }
    default:return 1;
    }
}
VERIFY(0x021F7270,&daNpc_Bj1_c::bj_flyMove);

static u32 bj1_parameterRecordCtor(void* storage) {
    WWHD_FUNC(0x021FA534,u32,storage);
    u32 a=gabi::ea(storage);
    if(a==0){a=gabi::call<u32>(0x0273AD10,0x6C);if(a==0)return 0;}
    gabi::store<u32>(a,0x10016BBC);return a;
}
VERIFY(0x021FA534,bj1_parameterRecordCtor);

static u32 bj1_parameterCollectionCtor(void* storage) {
    WWHD_FUNC(0x021FA574,u32,storage);
    u32 a=gabi::ea(storage);
    if(a==0){a=gabi::call<u32>(0x0273AD10,0x3D8);if(a==0)return 0;}
    gabi::store<u32>(a,0x10016BCC);
    gabi::call(0x028EFFD0,gabi::at<void>(a+0xC),9,0x6C,gabi::at<void>(0x021FA534));
    for(u32 i=0;i<9;++i){
        u32 record=a+0xC+i*0x6C;
        gabi::store<u32>(record+0x68,i);
        memcpy_g(gabi::at<void>(record+4),gabi::at<void>(0x101BC028+i*0x64),0x64);
    }
    gabi::store<u32>(a+8,0xFFFFFFFFu);gabi::store<u8>(a+4,0xFF);return a;
}
VERIFY(0x021FA574,bj1_parameterCollectionCtor);

static void bj1_sinit() {
    WWHD_FUNC(0x021FA624,void);
    gabi::store<u32>(0x10465D90,0);gabi::store<u32>(0x10465D88,0);
    gabi::store<u32>(0x10465D94,0);gabi::store<u32>(0x10465D8C,0);
    gabi::call(0x028F026C,gabi::at<void>(0x101BC3AC));
    f32 lower=gabi::load<f32>(0x10016EE8),upper=gabi::load<f32>(0x10016EEC);
    gabi::store<f32>(0x10465D7C,lower);gabi::store<f32>(0x10465D80,upper);
    gabi::call(0x028ED6F8,gabi::at<void>(0x10465D84));
    gabi::call(0x028F026C,gabi::at<void>(0x101BC3B8));
    gabi::call(0x028EAB2C,gabi::at<void>(0x10465D85));
    gabi::call(0x028F026C,gabi::at<void>(0x101BC3C4));
    gabi::call(0x021FA574,gabi::at<void>(0x10465DA4));
}
VERIFY(0x021FA624,bj1_sinit);

static u32 bj1_safeStringDestructor(void* storage,u32 flags) {
    WWHD_FUNC(0x021FA6C4,u32,storage,flags);
    u32 a=gabi::ea(storage);
    if(a==0 || (flags&1)==0)return a;
    return gabi::call<u32>(0x0273AF40,storage,flags);
}
VERIFY(0x021FA6C4,bj1_safeStringDestructor);

static u32 bj1_actorDestructor(void* storage,u32 flags) {
    WWHD_FUNC(0x021FA6D8,u32,storage,flags);
    u32 a=gabi::ea(storage);if(a==0)return 0;
    gabi::call(0x02515A70,gabi::at<void>(a+0x690),2);
    gabi::call(0x02515860,gabi::at<void>(a+0x654),2);
    gabi::call(0x02018034,gabi::at<void>(a+0x628),2);
    gabi::store<u32>(a+0x470,0x10016B9C);gabi::store<u32>(a+0x464,0x10016BAC);
    gabi::call(0x024EFD9C,gabi::at<void>(a+0x450),0);
    u32 result=gabi::call<u32>(0x025D50BC,storage,0);
    if((flags&1)!=0)result=gabi::call<u32>(0x0273AF40,storage);
    return result;
}
VERIFY(0x021FA6D8,bj1_actorDestructor);

static u32 bj1_safeStringEmptyVirtual(void* storage) {
    WWHD_FUNC(0x021FA774,u32,storage);
    return gabi::ea(storage);
}
VERIFY(0x021FA774,bj1_safeStringEmptyVirtual);
