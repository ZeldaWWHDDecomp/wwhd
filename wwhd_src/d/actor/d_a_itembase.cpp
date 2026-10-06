/**
 * d_a_itembase.cpp (WWHD)
 * Item - Base Item Class
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_itembase.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_itembase.h"

#define SAFESTRING_VTBL 0x10012054 /* this TU's copy of the sead::SafeString vtable */
#define FILE_NAME STR(0x10012128)  /* "d_a_itembase.cpp" */

enum {
    dItemNo_SMALL_KEY_e = 0x15,
    dItemNo_BOMB_BAG_e = 0x31,
    dItemNo_SKULL_HAMMER_e = 0x33,
};

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 0252CA4C dDlst_texSpecmapST(cXyz* eye, dKy_tevstr_c*, J3DModel*, f32): HD passes the model,
 * GameCube the model data */
static inline void dDlst_texSpecmapST(cXyz* eye, dKy_tevstr_c* tev, J3DModel* model, f32 scale) {
    gabi::call(0x0252CA4C, eye, tev, model, scale);
}
/* mDoGph_gInf_c::isMonotone(): the byte at 0x101F4829 */
static inline bool mDoGph_isMonotone() { return gabi::load<u8>(0x101F4829) != 0; }

/* daPy_lk_c::getBombBrk()->setFrame(f) (HD inline): the player's bomb brk at link+0x44D4 (frame
 * control first), its J3DAnmTevRegKey frame (through the pointer at +0x10) and an HD frame helper
 * object (pointer at +0x20) whose frame is normalised by its callback, then 027DF40C(&helper). */
static inline void daPy_bombBrk_setFrame0(u32 link) {
    u32 anmFrame = gabi::load<u32>(link + 0x44E4);
    gabi::store<f32>(link + 0x44D8, 0.0f);
    gabi::store<f32>(anmFrame, 0.0f);
    u32 helper = gabi::load<u32>(link + 0x44F4);
    u32 fn = gabi::load<u32>(helper + 0x10);
    f32 end = gabi::load<f32>(helper + 8);
    f32 start = gabi::load<f32>(helper + 4);
    f32 f = gabi::call_ptr<f32>(fn, gabi::load<u32>(helper + 0x14), 0.0f, start, end);
    gabi::store<f32>(helper, f);
    gabi::call(0x027DF40C, link + 0x44F4);
}

/* 02183788 */
BOOL daItemBase_c::DeleteBase(const char* resName) {
    WWHD_FUNC(0x02183788, BOOL, this, resName);
    dComIfG_resDelete(&mPhs, resName);
    return TRUE;
}
VERIFY(0x02183788, &daItemBase_c::DeleteBase);

/* 021841BC */
BOOL daItemBase_c::clothCreate() {
    WWHD_FUNC(0x021841BC, BOOL, this);
    return TRUE;
}
VERIFY(0x021841BC, &daItemBase_c::clothCreate);

/* 021833D4. HD: no setShadow (HD shadows) */
BOOL daItemBase_c::DrawBase() {
    WWHD_FUNC(0x021833D4, BOOL, this);
    gabi::call_ptr(vfn(daItemBase_VT_SETTEVSTR), this);
    gabi::call_ptr(vfn(daItemBase_VT_ANIMENTRY), this);
    gabi::call_ptr(vfn(daItemBase_VT_SETLISTSTART), this);
    gabi::call_ptr(vfn(daItemBase_VT_SETTINGBEFOREDRAW), this);

    mDoExt_modelUpdateDL(mpModel);

    if (mpModelArrow[0]) {
        mDoExt_modelUpdateDL(mpModelArrow[0]);
    }
    if (mpModelArrow[1]) {
        mDoExt_modelUpdateDL(mpModelArrow[1]);
    }

    setListEnd();
    return TRUE;
}
VERIFY(0x021833D4, &daItemBase_c::DrawBase);

/* 02183488. HD: rupees go to their own translucent list (play+0x5D94); the GameCube lists are
 * play+0x5D84/0x5D88 (setListMaskOff) and play+0x5D58/0x5D60 (setListP1) */
void daItemBase_c::setListStart() {
    WWHD_FUNC(0x02183488, void, this);
    if (isRupee(m_itemNo)) {
        gabi::store<u32>(0x104B4638, gabi::load<u32>(dComIfGp_ea() + 0x5D94));
        return;
    }
    bool monotone = mDoGph_isMonotone();
    u32 play = dComIfGp_ea();
    if (!monotone) {
        gabi::store<u32>(0x104B4634, gabi::load<u32>(play + 0x5D84));
        gabi::store<u32>(0x104B4638, gabi::load<u32>(dComIfGp_ea() + 0x5D88));
    } else {
        gabi::store<u32>(0x104B4634, gabi::load<u32>(play + 0x5D58));
        gabi::store<u32>(0x104B4638, gabi::load<u32>(dComIfGp_ea() + 0x5D60));
    }
}
VERIFY(0x02183488, &daItemBase_c::setListStart);

/* 02183390 */
void daItemBase_c::setListEnd() {
    WWHD_FUNC(0x02183390, void, this);
    dComIfGd_setList();
}
VERIFY(0x02183390, &daItemBase_c::setListEnd);

/* 02183520 */
void daItemBase_c::settingBeforeDraw() {
    WWHD_FUNC(0x02183520, void, this);
    if (isBomb(m_itemNo)) {
        u32 link = gabi::load<u32>(dComIfGp_ea() + PLAY_PLAYERPTR); /* daPy_getPlayerLinkActorClass() */
        daPy_bombBrk_setFrame0(link);
        /* mpModel->getModelData()->getJointNodePointer(BOMB_JNT_VBOMB_MODEL_e)->setMtxCalc(NULL) */
        u32 md = gabi::ea(J3DModel_getModelData(mpModel));
        gabi::store<u32>(gabi::load<u32>(md + 8) + 0x14, 0);
    }

    if (m_itemNo == dItemNo_BOMB_BAG_e || m_itemNo == dItemNo_SKULL_HAMMER_e || m_itemNo == dItemNo_SMALL_KEY_e) {
        dDlst_texSpecmapST(&eyePos, &tevStr, mpModel, 1.0f);
    }
}
VERIFY(0x02183520, &daItemBase_c::settingBeforeDraw);

/* 021835F4 */
void daItemBase_c::setTevStr() {
    WWHD_FUNC(0x021835F4, void, this);
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    dScnKy_env_light_c* light = dKy_getEnvlight();
    setLightTevColorType(light, mpModel, &tevStr);

    for (int i = 0; i < 2; i++) {
        if (!mpModelArrow[i]) {
            continue;
        }
        light = dKy_getEnvlight();
        setLightTevColorType(light, mpModelArrow[i], &tevStr);
    }
}
VERIFY(0x021835F4, &daItemBase_c::setTevStr);

/* 0218368C */
void daItemBase_c::animEntry() {
    WWHD_FUNC(0x0218368C, void, this);
    if (mpBrkAnm1) {
        int constantFrame = dItem_data::getTevFrm(m_itemNo);
        J3DModelData* modelData = J3DModel_getModelData(mpModel);
        if (constantFrame != -1) {
            mDoExt_brkAnm_entry(mpBrkAnm1, modelData, (f32)constantFrame);
        } else {
            mDoExt_brkAnm_entry(mpBrkAnm1, modelData, anm_frameCtrl(mpBrkAnm1)->getFrame());
        }
    }
    if (mpBtkAnm1) {
        mDoExt_btkAnm_entry(mpBtkAnm1, J3DModel_getModelData(mpModel), anm_frameCtrl(mpBtkAnm1)->getFrame());
    }
    if (mpBrkAnm2) {
        mDoExt_brkAnm_entry(mpBrkAnm2, J3DModel_getModelData(mpModel), anm_frameCtrl(mpBrkAnm2)->getFrame());
    }
    if (mpBtkAnm2) {
        mDoExt_btkAnm_entry(mpBtkAnm2, J3DModel_getModelData(mpModel), anm_frameCtrl(mpBtkAnm2)->getFrame());
    }
    if (mpBckAnm) {
        mpBckAnm->entry(J3DModel_getModelData(mpModel), anm_frameCtrl(mpBckAnm)->getFrame());
    }
}
VERIFY(0x0218368C, &daItemBase_c::animEntry);

/* 021837B0 */
void daItemBase_c::animPlay(f32 brk1Speed, f32 brk2Speed, f32 btk1Speed, f32 btk2Speed, f32 bckSpeed) {
    WWHD_FUNC(0x021837B0, void, this, brk1Speed, brk2Speed, btk1Speed, btk2Speed, bckSpeed);
    if (mpBrkAnm1 && dItem_data::getTevFrm(m_itemNo) == -1) {
        anm_frameCtrl(mpBrkAnm1)->setRate(brk1Speed);
        mDoExt_baseAnm_play(mpBrkAnm1);
    }

    if (mpBtkAnm1) {
        anm_frameCtrl(mpBtkAnm1)->setRate(btk1Speed);
        mDoExt_baseAnm_play(mpBtkAnm1);
    }

    if (mpBrkAnm2) {
        anm_frameCtrl(mpBrkAnm2)->setRate(brk2Speed);
        mDoExt_baseAnm_play(mpBrkAnm2);
    }

    if (mpBtkAnm2) {
        anm_frameCtrl(mpBtkAnm2)->setRate(btk2Speed);
        mDoExt_baseAnm_play(mpBtkAnm2);
    }

    if (mpBckAnm) {
        anm_frameCtrl(mpBckAnm)->setRate(bckSpeed);
        mDoExt_baseAnm_play(mpBckAnm);
    }
}
VERIFY(0x021837B0, &daItemBase_c::animPlay);

/* 02184114 */
static void __sinit_d_a_itembase_cpp() {
    WWHD_FUNC(0x02184114, void, (u32)0);
    sinit_header_statics(0x10464830, 0x101B7A90);
}
VERIFY(0x02184114, __sinit_d_a_itembase_cpp);

/* 021841A8: deleting destructor of a class with a trivial destructor (sead::SafeString, slot 1 of
 * this TU's SafeString vtable) */
static void SafeString_dt(void* p, s32 flags) {
    WWHD_FUNC(0x021841A8, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x021841A8, SafeString_dt);

/* 021841C4: sead::SafeString::assureTerminationImpl_ (empty; slot 2 of this TU's SafeString
 * vtable) */
static void SafeString_assureTerminationImpl(void* p) {
    WWHD_FUNC(0x021841C4, void, p);
}
VERIFY(0x021841C4, SafeString_assureTerminationImpl);

/* ---- CreateItemHeap helpers ---- */
/* PowerPC slw / srw (6-bit shift amount) */
static inline u32 ppc_slw(u32 x, u32 n) { return (n & 0x20) ? 0 : x << (n & 0x1F); }
static inline u32 ppc_srw(u32 x, u32 n) { return (n & 0x20) ? 0 : x >> (n & 0x1F); }
/* sead::SafeString {const char* mStringTop; vtable} built in a frame slot */
static inline void safestring_set(u32 slot, u32 str) {
    gabi::store<u32>(slot + 4, SAFESTRING_VTBL);
    gabi::store<u32>(slot, str);
}
/* sead::SafeString::cstr(): virtual assureTerminationImpl_ (vtable +0x14), then mStringTop */
static inline void safestring_assure(u32 slot) { gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(slot + 4) + 0x14), slot); }
/* sead::SafeStringBase<char>::isEqual (inline), cMaximumLength 0x40000 */
static bool safestring_isEqual(u32 a, u32 b) {
    gabi::call(0x021841C4, a); /* assureTerminationImpl_ (this TU's copy, called directly) */
    safestring_assure(a);
    u32 sa = gabi::load<u32>(a);
    safestring_assure(b);
    u32 sb = gabi::load<u32>(b);
    if (sa == sb)
        return true;
    for (u32 k = 0; k < 0x40001; k++) {
        u8 ca = gabi::load<u8>(sa + k);
        if (ca != gabi::load<u8>(sb + k))
            return false;
        if (ca == 0)
            return true;
    }
    return false;
}
/* relative pointer (nn::g3d style): base + offset, NULL for offset 0 */
static inline u32 rel_ptr(u32 base) {
    s32 off = gabi::load<s32>(base);
    return off != 0 ? base + off : 0;
}
/* inline mDoExt_bckAnm::mDoExt_bckAnm() (HD, 0x8C bytes) */
static void bckAnm_ct(u32 b) {
    gabi::call(0x027F2BC0, b, 0); /* J3DFrameCtrl::init */
    gabi::store<u32>(b + 0x10, 0x1016E54C);
    gabi::call(0x027DA984, b + 0x14);
    gabi::store<u32>(b + 0x80, 0);
    gabi::store<u32>(b + 0x58, 0);
    gabi::store<u32>(b + 0x48, 0x1016D820);
    gabi::store<u32>(b + 0x84, 0);
    gabi::store<u32>(b + 0x10, 0x1001206C); /* this TU's vtable */
    gabi::store<u32>(b + 0x7C, 0);
    gabi::store<u32>(b + 0x88, 0);
}
static inline BOOL btkAnm_init(u32 a, J3DModelData* d, void* key, s32 play) {
    return gabi::call<BOOL>(0x025E7CE0, a, d, key, play, (s32)J3DFrameCtrl::EMode_LOOP, 1.0f, (s16)0, (s16)-1, false, (s32)0);
}
static inline BOOL brkAnm_init(u32 a, J3DModelData* d, void* key, s32 play) {
    return gabi::call<BOOL>(0x025E8154, a, d, key, play, (s32)J3DFrameCtrl::EMode_LOOP, 1.0f, (s16)0, (s16)-1, false, (s32)0);
}

/* 021838D0. HD:
 *  - item 0x77 (the model "Vho"): every material of the model's texture table named "Vho" gets the
 *    texture "Vho2" of the resource file "Vho.bfres" (an entry built on the stack and copied, with
 *    its self-relative offsets rebased), and its bit is set in the table's mask;
 *  - rupees: the material "SC_lupy_outside" loses bit 0 of its HD draw flags, the model loses
 *    flag 0x2, and every material packet gets the model's flags (027F596C);
 *  - the animations are created with `new` (HD constructors allocate when this == NULL; bckAnm's
 *    constructor is inline). */
BOOL daItemBase_c::CreateItemHeap(const char* resName, s16 resIdx, s16 btkAnm1, s16 btkAnm2, s16 brkAnm1, s16 brkAnm2,
                                  s16 bckAnm, s16 param_8) {
    WWHD_FUNC(0x021838D0, BOOL, this, resName, resIdx, btkAnm1, btkAnm2, brkAnm1, brkAnm2, bckAnm, param_8);
    GuestFrame frame(0xE8);
    const u32 sp = frame.sp;

    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(resName, resIdx, SAFESTRING_VTBL);
    if (modelData == nullptr) /* JUT_ASSERT(97, modelData != NULL) */
        JUT_ASSERT_fail(FILE_NAME, 0x61, STR(0x1001213C));

    mpModel = mDoExt_J3DModel__create(modelData, 0, 0x11020203);
    if (!mpModel) {
        return FALSE;
    }

    switch (m_itemNo) {
    case 0x12: /* dItemNo_ARROW_30_e */
        mpModelArrow[0] = mDoExt_J3DModel__create(modelData, 0x80000, 0x11000002);
        if (!mpModelArrow[0]) {
            return FALSE;
        }
    case 0x11: /* dItemNo_ARROW_20_e */
        mpModelArrow[1] = mDoExt_J3DModel__create(modelData, 0x80000, 0x11000002);
        if (!mpModelArrow[1]) {
            return FALSE;
        }
        break;
    default:
        mpModelArrow[0] = NULL;
        mpModelArrow[1] = NULL;
    }

    if (m_itemNo == 0x77) {
        u32 md = gabi::ea(J3DModel_getModelData(mpModel));
        u32 texTable = gabi::load<u32>(md + 0x30);
        if (texTable == 0)
            return FALSE;
        u32 nameTab = gabi::load<u32>(gabi::ea(J3DModel_getModelData(mpModel)) + 0x34);
        if (nameTab == 0)
            return FALSE;
        const u32 entryTmp = sp + 0x34;
        for (u16 i = 0; i < gabi::load<u16>(texTable); i++) {
            u32 name = gabi::call<u32>(0x027ED1F0, nameTab, (u32)i); /* JUTNameTab::getName */
            if (name == 0) /* JUT_ASSERT(160, name != NULL) */
                JUT_ASSERT_fail(FILE_NAME, 0xA0, STR(0x100120EC));
            bool match = false;
            if (name != 0) {
                safestring_set(sp + 0x14, 0x100120D0); /* "Vho" */
                safestring_set(sp + 0x1C, name);
                match = safestring_isEqual(sp + 0x14, sp + 0x1C);
            }
            if (!match)
                continue;
            safestring_set(sp + 0x24, gabi::ea(resName));
            safestring_set(sp + 0x2C, 0x1001211C); /* "Vho.bfres" */
            u32 file = gabi::call<u32>(0x026124B0, gabi::load<u32>(0x101F4F7C), sp + 0x24, sp + 0x2C, 0);
            u32 tex = gabi::call<u32>(0x027DFA24, rel_ptr(file + 0x24), 0x100120D4); /* "Vho2" */
            if (tex == 0) /* JUT_ASSERT(167, res_tex) */
                JUT_ASSERT_fail(FILE_NAME, 0xA7, STR(0x100120C8));
            gabi::store<u32>(entryTmp + 0x20, tex);
            gabi::store<u16>(entryTmp + 0x2, (u16)gabi::load<u32>(tex + 8));
            gabi::store<u8>(entryTmp + 0x8, 0);
            gabi::store<u16>(entryTmp + 0x4, (u16)gabi::load<u32>(tex + 0xC));
            u32 entry = gabi::load<u32>(texTable + 4) + i * 0x24;
            for (int k = 0; k < 9; k++) gabi::store<u32>(entry + 4 * k, gabi::load<u32>(entryTmp + 4 * k));
            entry = gabi::load<u32>(texTable + 4) + i * 0x24;
            gabi::store<u32>(entry + 0x1C, gabi::load<u32>(entry + 0x1C) + entryTmp - entry);
            entry = gabi::load<u32>(texTable + 4) + i * 0x24;
            gabi::store<u32>(entry + 0xC, gabi::load<u32>(entry + 0xC) + entryTmp - entry);
            entry = gabi::load<u32>(texTable + 4) + i * 0x24;
            gabi::store<u32>(entry + 0x20, gabi::load<u32>(entryTmp + 0x20));
            /* mask |= (u64)one << i, a 128-bit mask at +8 (two words per half) */
            u32 hi = gabi::load<u32>(texTable + 0x18);
            u32 lo = gabi::load<u32>(texTable + 0x1C);
            if (i < 0x40) {
                u32 n = i;
                u32 shi = ppc_slw(lo, n + 0x20) | (ppc_slw(hi, n) | ppc_srw(lo, 0x20 - n));
                u32 slo = ppc_slw(lo, n);
                gabi::store<u32>(texTable + 0x8, gabi::load<u32>(texTable + 0x8) | shi);
                gabi::store<u32>(texTable + 0xC, gabi::load<u32>(texTable + 0xC) | slo);
            } else {
                u32 n = i - 0x40;
                u32 shi = ppc_slw(lo, n + 0x20) | (ppc_slw(hi, n) | ppc_srw(lo, 0x20 - n));
                u32 slo = ppc_slw(lo, n);
                gabi::store<u32>(texTable + 0x14, gabi::load<u32>(texTable + 0x14) | slo);
                gabi::store<u32>(texTable + 0x10, gabi::load<u32>(texTable + 0x10) | shi);
            }
        }
    } else if (isRupee(m_itemNo)) {
        u32 md = gabi::ea(J3DModel_getModelData(mpModel));
        safestring_set(sp + 0x58, 0x100120DC); /* "SC_lupy_outside" */
        u32 matTable = gabi::load<u32>(md);
        gabi::call(0x021841C4, sp + 0x58); /* cstr(): assureTerminationImpl_ */
        s32 idx = gabi::call<s32>(0x027DF9B0, rel_ptr(matTable + 0x18), gabi::load<u32>(sp + 0x58)); /* JUTNameTab::getIndex */
        u32 mat;
        if (idx < 0) {
            mat = 0;
        } else {
            mat = gabi::load<u32>(md + 0x10);
            if ((u32)idx < gabi::load<u32>(md + 0xC))
                mat += idx * 0x39C;
        }
        if (mat != 0) {
            u16 matIdx = gabi::load<u16>(gabi::load<u32>(mat) + 0xC);
            u32 m = gabi::ea(mpModel);
            u32 matPacket = 0;
            if (matIdx < gabi::load<u32>(m + 0x138))
                matPacket = gabi::load<u32>(m + 0x13C) + matIdx * 0x38;
            gabi::store<u32>(matPacket + 0x30, gabi::load<u32>(matPacket + 0x30) & ~1u);
        }
        u32 m = gabi::ea(mpModel);
        u32 flags = gabi::load<u32>(m + 0x74) & ~2u;
        gabi::store<u32>(m + 0x74, flags);
        gabi::call(0x027F596C, m, flags); /* J3DModel (HD): set the flags of every material packet */
    }

    void* pbtk;
    mpBtkAnm1 = NULL;
    if (btkAnm1 != -1) {
        pbtk = dComIfG_getObjectRes(resName, btkAnm1, SAFESTRING_VTBL);
        if (pbtk == nullptr) /* JUT_ASSERT(196, pbtk != NULL) */
            JUT_ASSERT_fail(FILE_NAME, 0xC4, STR(0x100120F8));
        mpBtkAnm1 = gabi::call<mDoExt_btkAnm*>(0x025E7C6C, 0); /* new mDoExt_btkAnm() */
        if (!mpBtkAnm1 || !btkAnm_init(gabi::ea(mpBtkAnm1), modelData, pbtk, TRUE)) {
            return FALSE;
        }
    }

    mpBtkAnm2 = NULL;
    if (btkAnm2 != -1) {
        pbtk = dComIfG_getObjectRes(resName, btkAnm2, SAFESTRING_VTBL);
        if (pbtk == nullptr) /* JUT_ASSERT(212, pbtk != NULL) */
            JUT_ASSERT_fail(FILE_NAME, 0xD4, STR(0x100120F8));
        mpBtkAnm2 = gabi::call<mDoExt_btkAnm*>(0x025E7C6C, 0);
        if (!mpBtkAnm2 || !btkAnm_init(gabi::ea(mpBtkAnm2), modelData, pbtk, TRUE)) {
            return FALSE;
        }
    }

    void* pbrk;
    mpBrkAnm1 = NULL;
    if (brkAnm1 != -1) {
        pbrk = dComIfG_getObjectRes(resName, brkAnm1, SAFESTRING_VTBL);
        if (pbrk == nullptr) /* JUT_ASSERT(229, pbrk != NULL) */
            JUT_ASSERT_fail(FILE_NAME, 0xE5, STR(0x10012104));
        s8 tevFrm = dItem_data::getTevFrm(m_itemNo);
        BOOL shouldAnimate = TRUE;
        if (tevFrm != -1) {
            shouldAnimate = FALSE;
        }
        mpBrkAnm1 = gabi::call<mDoExt_brkAnm*>(0x025E80D0, 0); /* new mDoExt_brkAnm() */
        if (!mpBrkAnm1 || !brkAnm_init(gabi::ea(mpBrkAnm1), modelData, pbrk, shouldAnimate)) {
            return FALSE;
        }
    }

    mpBrkAnm2 = NULL;
    if (brkAnm2 != -1) {
        pbrk = dComIfG_getObjectRes(resName, brkAnm2, SAFESTRING_VTBL);
        if (pbrk == nullptr) /* JUT_ASSERT(254, pbrk != NULL) */
            JUT_ASSERT_fail(FILE_NAME, 0xFE, STR(0x10012104));
        mpBrkAnm2 = gabi::call<mDoExt_brkAnm*>(0x025E80D0, 0);
        if (!mpBrkAnm2 || !brkAnm_init(gabi::ea(mpBrkAnm2), modelData, pbrk, TRUE)) {
            return FALSE;
        }
    }

    J3DAnmTransform* pbck;
    mpBckAnm = NULL;
    if (bckAnm != -1) {
        pbck = (J3DAnmTransform*)dComIfG_getObjectRes(resName, bckAnm, SAFESTRING_VTBL);
        if (pbck == nullptr) /* JUT_ASSERT(269, pbck != NULL) */
            JUT_ASSERT_fail(FILE_NAME, 0x10D, STR(0x10012110));
        u32 p = gabi::ea(operator_new(0x8C)); /* new mDoExt_bckAnm() */
        if (p != 0)
            bckAnm_ct(p);
        mpBckAnm = gabi::at<mDoExt_bckAnm>(p);
        if (!mpBckAnm || !mpBckAnm->init(modelData, pbck, TRUE, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, false)) {
            return FALSE;
        }
    }

    if (!gabi::call_ptr<BOOL>(vfn(daItemBase_VT_CLOTHCREATE), this)) {
        return FALSE;
    }

    return TRUE;
}
VERIFY(0x021838D0, &daItemBase_c::CreateItemHeap);
