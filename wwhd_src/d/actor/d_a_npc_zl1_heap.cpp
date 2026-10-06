/**
 * d_a_npc_zl1_heap.cpp (WWHD)
 * NPC - Tetra: resources, heap, construction and static data.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_zl1.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_npc_zl1.h"

/* ---- local bindings ---- */
/* 028F040C strcpy */
static inline void strcpy_g(u32 dst, u32 src) { gabi::call(0x028F040C, dst, src); }
/* strcat, inlined by GHS: find the end of dst, copy src with its terminator */
static inline void strcat_inl(u32 dst, u32 src) {
    u32 d = dst;
    while (gabi::load<u8>(d) != 0) d++;
    u8 c;
    do {
        c = gabi::load<u8>(src++);
        gabi::store<u8>(d++, c);
    } while (c != 0);
}
static inline u32 selfrel(u32 base) { u32 off = gabi::load<u32>(base); return off != 0 ? base + off : 0; }
/* a sead::SafeString's virtual assureTermination (vtable slot +0x14) */
static inline void SafeString_assure(SafeString* s) { gabi::call_ptr(gabi::load<u32>(s->__vtbl + 0x14), s); }

/* 0230242C daNpc_Zl1_matAnm_c::daNpc_Zl1_matAnm_c (HD: allocates when this == NULL; the
 * J3DMaterialAnm base constructor is inlined: only the vtable is written) */
static daNpc_Zl1_matAnm_c* daNpc_Zl1_matAnm_c_ct(daNpc_Zl1_matAnm_c* i_this) {
    WWHD_FUNC(0x0230242C, daNpc_Zl1_matAnm_c*, i_this);
    if (i_this == nullptr) {
        i_this = (daNpc_Zl1_matAnm_c*)operator_new(0x80);
        if (i_this == nullptr)
            return i_this;
    }
    i_this->field_0x7C = 0;
    i_this->mOffsetX = 0.0f;
    i_this->__vtbl = 0x10024298;
    i_this->mOffsetY = 0.0f;
    return i_this;
}
VERIFY(0x0230242C, daNpc_Zl1_matAnm_c_ct);

/* 02302484 btpResID (not named by the matcher) */
u32 daNpc_Zl1_c::btpResID(int param_1) {
    WWHD_FUNC(0x02302484, u32, this, param_1);
    /* a_BTPName_tbl (.data 0x101C70F0), l_BTPName (0x10468BE0) */
    strcpy_g(0x10468BE0, gabi::load<u32>(0x101C70F0 + param_1 * 4));
    strcat_inl(0x10468BE0, 0x10023C78 /* ".btp" */);
    return 0x10468BE0;
}
VERIFY(0x02302484, &daNpc_Zl1_c::btpResID);

/* 02302834 btkResID (not named by the matcher) */
u32 daNpc_Zl1_c::btkResID(int param_1) {
    WWHD_FUNC(0x02302834, u32, this, param_1);
    /* a_BTKName_tbl (.data 0x101C7134), l_BTKName (0x10468C00) */
    strcpy_g(0x10468C00, gabi::load<u32>(0x101C7134 + param_1 * 4));
    strcat_inl(0x10468C00, 0x10023D10 /* ".btk" */);
    return 0x10468C00;
}
VERIFY(0x02302834, &daNpc_Zl1_c::btkResID);

/* 0230600C bckResID (not named by the matcher) */
u32 daNpc_Zl1_c::bckResID(int param_1) {
    WWHD_FUNC(0x0230600C, u32, this, param_1);
    /* a_BCKName_tbl (.data 0x101C7188), l_BCKName (0x10468BC0) */
    strcpy_g(0x10468BC0, gabi::load<u32>(0x101C7188 + param_1 * 4));
    strcat_inl(0x10468BC0, 0x1002405C /* ".bck" */);
    return 0x10468BC0;
}
VERIFY(0x0230600C, &daNpc_Zl1_c::bckResID);

/* 023028A4 setMat (not named by the matcher). HD: the eye materials are found by name (static
 * SafeStrings "eyeL"/"eyeR" at 0x10468BA4) instead of the btk's update material ids. */
void daNpc_Zl1_c::setMat() {
    WWHD_FUNC(0x023028A4, void, this);
    u32 btk = gabi::load<u32>(gabi::ea(mBtkAnm) + 0x68); /* mBtkAnm.getBtkAnm() */
    u16 material_num = gabi::load<u16>(gabi::load<u32>(btk + 0xC) + 0x14);
    J3DModel* model = mpMorf->getModel();
    for (u16 i = 0; i < material_num; i++) {
        SafeString* name_obj = gabi::at<SafeString>(0x10468BA4 + i * 8);
        J3DModelData* md = J3DModel_getModelData_l(model);
        u32 hdr = gabi::load<u32>(gabi::ea(md));
        SafeString_assure(name_obj);
        s32 idx = gabi::call<s32>(0x027DF9B0, selfrel(hdr + 0x18), gabi::at<const char>(name_obj->mStringTop)); /* JUTNameTab::getIndex */
        u32 mat;
        if (idx < 0) {
            mat = 0;
        } else {
            mat = gabi::load<u32>(gabi::ea(md) + 0x10);
            if ((u32)idx < gabi::load<u32>(gabi::ea(md) + 0xC))
                mat += idx * 0x39C;
        }
        field_0x6D4[i] = gabi::at<daNpc_Zl1_matAnm_c>(gabi::load<u32>(mat + 0x24)); /* getMaterialAnm() */
    }
}
VERIFY(0x023028A4, &daNpc_Zl1_c::setMat);

/* 02302984 setBtk (not named by the matcher). The number is passed on as the full register
 * (int; GameCube s8). */
static u32 setBtk_l(daNpc_Zl1_c* i_this, s32 param_1, u32 param_2) {
    WWHD_FUNC(0x02302984, u32, i_this, param_1, param_2);
    J3DModel* model = i_this->mpMorf->getModel();
    if (param_1 < 0) {
        return false;
    }
    void* a_btk = dComIfG_getObjectRes_name(i_this->mArcName, gabi::at<const char>(gabi::call<u32>(0x02302834, i_this, param_1) /* btkResID */));
    /* HD: no JUT_ASSERT */
    i_this->field_0x848 = (s8)param_1;
    i_this->mBtkAnmFrame = 0;
    if (mDoExt_btkAnm_init(i_this->mBtkAnm, J3DModel_getModelData_l(model), a_btk, 1, 0, 1.0f, 0, -1, param_2, 0) != 0) {
        if (!param_2) {
            i_this->setMat();
        }
        return true;
    }
    return false;
}
VERIFY(0x02302984, setBtk_l);

/* a bounded strcmp of two SafeStrings (sead::SafeString::isEqual, HD inline) */
static inline bool SafeString_isEqual(u32 a, u32 b) {
    if (a == b)
        return true;
    for (u32 n = 0x40001; n != 0; n--) {
        u8 c0 = gabi::load<u8>(a);
        u8 c1 = gabi::load<u8>(b);
        if (c0 != c1)
            return false;
        if (c0 == 0)
            return true;
        a++;
        b++;
    }
    return false;
}

/* 023024F4 setBtp (not named by the matcher). HD: the pattern animation is set up twice (mBtpAnm
 * and the HD mBtpAnm2), and mBtpAnm2's material table is relinked: every material of the btp
 * except "zelda_mouth" is linked to its model material. The number is passed on as the full
 * register. */
static u32 setBtp_l(daNpc_Zl1_c* i_this, s32 param_1, u32 param_2) {
    WWHD_FUNC(0x023024F4, u32, i_this, param_1, param_2);
    J3DModel* model = i_this->mpMorf->getModel();
    if (param_1 < 0) {
        return false;
    }
    void* a_btp = dComIfG_getObjectRes_name(i_this->mArcName, gabi::at<const char>(gabi::call<u32>(0x02302484, i_this, param_1) /* btpResID */));
    /* HD: no JUT_ASSERT */
    i_this->field_0x847 = (s8)param_1;
    i_this->mBtpAnmFrame = 0;
    i_this->mTimer = 0;
    s32 ret = mDoExt_btpAnm_init(i_this->mBtpAnm, J3DModel_getModelData_l(model), a_btp, 1, 0, 1.0f, 0, -1, param_2, 0);
    mDoExt_btpAnm_init(i_this->mBtpAnm2, J3DModel_getModelData_l(model), a_btp, 1, 0, 1.0f, 0, -1, param_2, 0);
    u32 anm = gabi::load<u32>(gabi::ea(i_this->mBtpAnm2) + 0x60); /* mBtpAnm2.getBtpAnm() */
    J3DModelData* md = J3DModel_getModelData_l(i_this->mpMorf->getModel());
    u16 n = gabi::load<u16>(anm + 0x16); /* update material count */
    u32 tblp = gabi::ea(i_this->mBtpAnm2) + 0x54; /* material links (u32[]) */
    for (u32 i = 0; i < n; i++) {
        u32 entry = selfrel(anm + 0x2C) + i * 0x1C;
        gabi::Local<SafeString> name;
        name->mStringTop = selfrel(entry + 0xC);
        name->__vtbl = ZL1_SAFESTRING_VTBL;
        u32 h = gabi::call<u32>(0x027F3F8C, md); /* the material name table header */
        SafeString_assure(name);
        s32 idx = gabi::call<s32>(0x027DF9B0, selfrel(h + 0x18), gabi::at<const char>(name->mStringTop));
        gabi::Local<SafeString> mouth;
        mouth->mStringTop = 0x10023D04; /* "zelda_mouth" */
        mouth->__vtbl = ZL1_SAFESTRING_VTBL;
        SafeString_assure(name);
        SafeString_assure(name);
        u32 a = name->mStringTop;
        SafeString_assure(mouth);
        bool eq = SafeString_isEqual(a, mouth->mStringTop);
        if (!eq) {
            u32 t = gabi::load<u32>(tblp);
            gabi::store<u32>(t + i * 4, gabi::load<u32>(t + i * 4) & 0x3FFF8000);
            u32 nx = idx + 1;
            t = gabi::load<u32>(tblp);
            gabi::store<u32>(t + i * 4, gabi::load<u32>(t + i * 4) | (nx & 0x7FFF));
            t = gabi::load<u32>(tblp);
            gabi::store<u32>(t + nx * 4, gabi::load<u32>(t + nx * 4) & 0xC0007FFF);
            t = gabi::load<u32>(tblp);
            gabi::store<u32>(t + nx * 4, gabi::load<u32>(t + nx * 4) | ((i << 15) & 0x3FFF8000));
        } else {
            u32 t = gabi::load<u32>(tblp);
            gabi::store<u32>(t + i * 4, gabi::load<u32>(t + i * 4) | 0xC0007FFF);
            t = gabi::load<u32>(tblp);
            gabi::store<u32>(t + idx * 4, gabi::load<u32>(t + idx * 4) | 0x3FFF8000);
        }
    }
    return ret != 0;
}
VERIFY(0x023024F4, setBtp_l);

/* 02302A90 */
u32 daNpc_Zl1_c::init_texPttrnAnm(s8 param_1, u32 param_2) {
    WWHD_FUNC(0x02302A90, u32, this, param_1, param_2);
    u32 ret = false;
    if (setBtp_l(this, param_1, param_2) != 0) {
        /* a_btk_num_tbl (.rodata 0x10023D68) */
        ret = setBtk_l(this, gabi::load<s8>(0x10023D68 + param_1), param_2);
    }
    return ret;
}
VERIFY(0x02302A90, &daNpc_Zl1_c::init_texPttrnAnm);

/* 02303400 */
BOOL daNpc_Zl1_c::itemCreateHeap() {
    WWHD_FUNC(0x02303400, BOOL, this);
    if (field_0x84F != 1) {
        mpModel = nullptr;
        return TRUE;
    }
    J3DModelData* a_mdl_dat = (J3DModelData*)dComIfG_getObjectRes_name(mArcName, STR(0x10023EAC) /* "cloth.bdl" */);
    if (a_mdl_dat == nullptr) /* JUT_ASSERT(0x10E6, a_mdl_dat != NULL) */
        JUT_ASSERT_fail(STR(0x10023E9C), 0x10E6, STR(0x10023EB8));
    mpModel = mDoExt_J3DModel__create(a_mdl_dat, 0x80000, 0x11000022);
    return mpModel.get() != nullptr;
}
VERIFY(0x02303400, &daNpc_Zl1_c::itemCreateHeap);

/* 023034DC */
BOOL daNpc_Zl1_c::CreateHeap() {
    WWHD_FUNC(0x023034DC, BOOL, this);
    if (bodyCreateHeap() == FALSE) {
        return FALSE;
    }
    if (itemCreateHeap() == FALSE) {
        mpMorf = nullptr;
        return FALSE;
    }
    mLightInfluence2.mPower = 0.0f;
    mLightInfluence1.mPower = 0.0f;
    gabi::call(0x0255A2B8, &mLightInfluence1); /* dKy_plight_priority_set */
    gabi::call(0x0255B9C8, &mLightInfluence2); /* dKy_efplight_set */
    mAcchCir.SetWall(30.0f, 50.0f);
    mObjAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, nullptr, nullptr);
    return TRUE;
}
VERIFY(0x023034DC, &daNpc_Zl1_c::CreateHeap);

/* 023035BC searchActor_Branch (not named by the matcher) */
static void* searchActor_Branch(void* i_actorP, void*) {
    WWHD_FUNC(0x023035BC, void*, i_actorP, (void*)nullptr);
    if (l_check_wrk() < 20 && fopAc_IsActor(i_actorP) && i_actorP != nullptr && fpcM_GetName(i_actorP) == 0x1A9 /* fpcNm_BRANCH_e */) {
        s32 n = l_check_wrk();
        l_check_wrk() = n + 1;
        l_check_inf()[n] = (fopAc_ac_c*)i_actorP;
    }
    return nullptr;
}
VERIFY(0x023035BC, searchActor_Branch);

/* 0230363C searchActor_Bm1 (not named by the matcher) */
static void* searchActor_Bm1(void* i_actorP, void*) {
    WWHD_FUNC(0x0230363C, void*, i_actorP, (void*)nullptr);
    if (l_check_wrk() < 20 && fopAc_IsActor(i_actorP) && i_actorP != nullptr && fpcM_GetName(i_actorP) == 0x146 /* fpcNm_NPC_BM1_e */) {
        s32 n = l_check_wrk();
        l_check_wrk() = n + 1;
        l_check_inf()[n] = (fopAc_ac_c*)i_actorP;
    }
    return nullptr;
}
VERIFY(0x0230363C, searchActor_Bm1);

/* 023036BC / 023036E0 / 02303704: destruction of the function-local static SafeString arrays
 * (registered at exit): __destroy_arr(arr, n, 8, SafeString_dt) */
static void destroy_arr_10468B24() {
    WWHD_FUNC(0x023036BC, void, (u32)0);
    gabi::call(0x028F0164, 0x10468B24, 2, 8, 0x0230A228, 0, 0);
}
VERIFY(0x023036BC, destroy_arr_10468B24);
static void destroy_arr_10468B34() {
    WWHD_FUNC(0x023036E0, void, (u32)0);
    gabi::call(0x028F0164, 0x10468B34, 6, 8, 0x0230A228, 0, 0);
}
VERIFY(0x023036E0, destroy_arr_10468B34);
static void destroy_arr_10468B64() {
    WWHD_FUNC(0x02303704, void, (u32)0);
    gabi::call(0x028F0164, 0x10468B64, 6, 8, 0x0230A228, 0, 0);
}
VERIFY(0x02303704, destroy_arr_10468B64);

/* 02303728 daNpc_Zl1_c::daNpc_Zl1_c (out of line in HD; allocates when this == NULL) */
static daNpc_Zl1_c* daNpc_Zl1_c_ct(daNpc_Zl1_c* i_this) {
    WWHD_FUNC(0x02303728, daNpc_Zl1_c*, i_this);
    if (i_this == nullptr) {
        i_this = (daNpc_Zl1_c*)operator_new(0xD74);
        if (i_this == nullptr)
            return i_this;
    }
    gabi::call(0x025A1458, i_this);                    /* fopNpc_npc_c::fopNpc_npc_c */
    i_this->__vtbl = ZL1_VTBL;
    gabi::call(0x025E7C6C, i_this->mBtkAnm);           /* mDoExt_btkAnm::mDoExt_btkAnm */
    gabi::call(0x025E7820, i_this->mBtpAnm);           /* mDoExt_btpAnm::mDoExt_btpAnm */
    gabi::call(0x025E7820, i_this->mBtpAnm2);
    gabi::call(0x025A9084, i_this->mRippleCallBack);   /* dPa_rippleEcallBack::dPa_rippleEcallBack */
    i_this->mLightInfluence2.mHD20 = 1.0f;
    i_this->mLightInfluence1.mHD20 = 1.0f;
    /* mDoExt_offCupOnAupPacket / mDoExt_onCupOffAupPacket: J3DPacket base constructor, then the
     * derived vtable at +0xC */
    gabi::call(0x027F1278, i_this->mOffCupOnAupPacket1);
    gabi::store<u32>(gabi::ea(i_this->mOffCupOnAupPacket1) + 0xC, 0x10058D10);
    gabi::call(0x027F1278, i_this->mOffCupOnAupPacket2);
    gabi::store<u32>(gabi::ea(i_this->mOffCupOnAupPacket2) + 0xC, 0x10058D10);
    gabi::call(0x027F1278, i_this->mOnCupOffAupPacket1);
    gabi::store<u32>(gabi::ea(i_this->mOnCupOffAupPacket1) + 0xC, 0x10058D40);
    gabi::call(0x027F1278, i_this->mOnCupOffAupPacket2);
    gabi::store<u32>(gabi::ea(i_this->mOnCupOffAupPacket2) + 0xC, 0x10058D40);
    return i_this;
}
VERIFY(0x02303728, daNpc_Zl1_c_ct);

/* 023052B4 */
cPhs_State daNpc_Zl1_c::_create() {
    WWHD_FUNC(0x023052B4, cPhs_State, this);
    /* fopAcM_ct(this, daNpc_Zl1_c) (HD: the constructor is out of line) */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            daNpc_Zl1_c_ct(this);
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    if (!decideType(fopAcM_GetParam(this) & 0xFF)) {
        return cPhs_ERROR_e;
    }
    cPhs_State state = dComIfG_resLoad(&mPhs, mArcName);
    mStateIsComplaete = state == cPhs_COMPLEATE_e;
    if (!mStateIsComplaete) {
        return state;
    }
    /* a_siz_tbl (.data 0x101C7180) */
    if (!fopAcM_entrySolidHeap(this, 0x023035B8 /* CheckCreateHeap */, gabi::load<u32>(0x101C7180 + field_0x84E * 4))) {
        return cPhs_ERROR_e;
    }
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpMorf->getModel())); /* fopAcM_SetMtx */
    fopAcM_setCullSizeBox(this, -50.0f, -20.0f, -50.0f, 50.0f, 140.0f, 50.0f);
    if (!createInit()) {
        return cPhs_ERROR_e;
    }
    return state;
}
VERIFY(0x023052B4, &daNpc_Zl1_c::_create);

/* 023086FC daNpc_Zl1_matAnm_c::calc(J3DMaterial*) (not named by the matcher; HD: the texture
 * matrix is found through the material's resource tables by name; the GameCube loops over 8) */
static void daNpc_Zl1_matAnm_c_calc(daNpc_Zl1_matAnm_c* i_this, void* i_material) {
    WWHD_FUNC(0x023086FC, void, i_this, i_material);
    u32 mat = gabi::ea(i_material);
    u32 hdr = gabi::load<u32>(mat);
    s32 idx = gabi::call<s32>(0x027DF9B0, selfrel(hdr + 0x38), STR(0x10024254));
    hdr = gabi::load<u32>(mat);
    u32 entry = selfrel(hdr + 0x34) + idx * 0x14;
    if (gabi::load<s32>(entry + 4) >= 0) {
        gabi::store<u16>(mat + 4, gabi::load<u16>(mat + 4) | 4);
        u32 bits = gabi::load<u32>(mat + 0xC) + ((idx >> 5) << 2);
        gabi::store<u32>(bits, gabi::load<u32>(bits) | (1u << (idx & 31)));
        hdr = gabi::load<u32>(mat);
    }
    u32 idx2 = gabi::load<u16>(entry + 0xC);
    u32 entry2 = selfrel(hdr + 0x34) + idx2 * 0x14;
    if (gabi::load<s32>(entry2 + 4) >= 0) {
        gabi::store<u16>(mat + 4, gabi::load<u16>(mat + 4) | 4);
        u32 bits = gabi::load<u32>(mat + 0xC) + (((s32)idx2 >> 5) << 2);
        gabi::store<u32>(bits, gabi::load<u32>(bits) | (1u << (idx2 & 31)));
    }
    u32 srt = gabi::load<u32>(mat + 0x28) + gabi::load<u16>(entry + 2);
    if (i_this->field_0x7C != 0) {
        gabi::store<f32>(srt + 0x10, i_this->mOffsetX);
        gabi::store<f32>(srt + 0x14, i_this->mOffsetY);
    }
}
VERIFY(0x023086FC, daNpc_Zl1_matAnm_c_calc);

/* 0230A0EC daNpc_Zl1_HIO_c::daNpc_Zl1_HIO_c (HD: allocates when this == NULL) */
static daNpc_Zl1_HIO_c* daNpc_Zl1_HIO_c_ct(daNpc_Zl1_HIO_c* i_this) {
    WWHD_FUNC(0x0230A0EC, daNpc_Zl1_HIO_c*, i_this);
    if (i_this == nullptr) {
        i_this = (daNpc_Zl1_HIO_c*)operator_new(0x64);
        if (i_this == nullptr)
            return i_this;
    }
    i_this->__vtbl = 0x10023C18;
    memcpy_g(&i_this->mPrmTbl, gabi::at<u8>(0x101C7574), 0x58); /* a_prm_tbl */
    i_this->mNo = -1;
    i_this->field_0x8 = -1;
    return i_this;
}
VERIFY(0x0230A0EC, daNpc_Zl1_HIO_c_ct);

/* 0230A158: static initialisation of the translation unit */
static void __sinit_d_a_npc_zl1_cpp() {
    WWHD_FUNC(0x0230A158, void, (u32)0);
    /* header statics (the zeroed object at 0x10468B94) */
    const u32 P = 0x10468B18, D = 0x101C75CC;
    for (int i = 0; i < 4; i++) gabi::store<u32>(0x10468B94 + 4 * i, 0);
    __register_global_object(D);
    gabi::store<f32>(P + 4, 3.1415927f);
    gabi::store<f32>(P, -3.1415927f);
    gabi::call(0x028ED6F8, P + 8);
    __register_global_object(D + 0xC);
    gabi::call(0x028EAB2C, P + 9);
    __register_global_object(D + 0x18);
    /* HD: two static sead::SafeStrings naming the eye materials (setMat) */
    gabi::store<u32>(0x10468BA8, ZL1_SAFESTRING_VTBL);
    gabi::store<u32>(0x10468BA4, 0x10024288); /* "eyeL" */
    gabi::store<u32>(0x10468BB0, ZL1_SAFESTRING_VTBL);
    gabi::store<u32>(0x10468BAC, 0x10024290); /* "eyeR" */
    daNpc_Zl1_HIO_c_ct(&l_HIO()); /* static daNpc_Zl1_HIO_c l_HIO */
}
VERIFY(0x0230A158, __sinit_d_a_npc_zl1_cpp);

/* 0230A228: sead::SafeString deleting destructor (this TU's copy) */
static void SafeString_dt(SafeString* i_this, s32 flags) {
    WWHD_FUNC(0x0230A228, void, i_this, flags);
    if (i_this != nullptr && (flags & 1))
        operator_delete(i_this);
}
VERIFY(0x0230A228, SafeString_dt);

/* 0230A23C: daNpc_Zl1_c deleting destructor (compiler-generated, HD virtual destructor) */
static void daNpc_Zl1_c_dt(daNpc_Zl1_c* i_this, s32 flags) {
    WWHD_FUNC(0x0230A23C, void, i_this, flags);
    if (i_this != nullptr) {
        gabi::call(0x027F13DC, i_this->mOnCupOffAupPacket2, 0); /* J3DPacket::~J3DPacket */
        gabi::call(0x027F13DC, i_this->mOnCupOffAupPacket1, 0);
        gabi::call(0x027F13DC, i_this->mOffCupOnAupPacket2, 0);
        gabi::call(0x027F13DC, i_this->mOffCupOnAupPacket1, 0);
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        gabi::call(0x02018034, gabi::ea(&i_this->mAcchCir) + 0x14, 2); /* cM3dGCir::~cM3dGCir (mAcchCir.m_cir) */
        gabi::store<u32>(gabi::ea(&i_this->mObjAcch) + 0x20, 0x10023BB8);
        gabi::store<u32>(gabi::ea(&i_this->mObjAcch) + 0x14, 0x10023BC8);
        gabi::call(0x024EFD9C, &i_this->mObjAcch, 0);
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x0230A23C, daNpc_Zl1_c_dt);

/* 0230A308: sead::SafeString::assureTerminationImpl_ (this TU's copy; empty) */
static void SafeString_assureTerminationImpl(SafeString*) {
    WWHD_FUNC(0x0230A308, void, (SafeString*)nullptr);
}
VERIFY(0x0230A308, SafeString_assureTerminationImpl);

/* material by name: md->getMaterialNodePointer(getMaterialName()->getIndex(name)) (HD inline;
 * materials are 0x39C bytes; an index past the count gives the first material) */
static inline u32 materialByIndex(u32 md, s32 idx) {
    if (idx < 0)
        return 0;
    u32 mat = gabi::load<u32>(md + 0x10);
    if ((u32)idx < gabi::load<u32>(md + 0xC))
        mat += idx * 0x39C;
    return mat;
}
/* joint by name (HD inline; joints are 0x1C bytes; an index past the count gives the first) */
static inline u32 jointByName(u32 md, const char* name) {
    s32 r = gabi::call<s32>(0x027DF9B0, J3DModelData_getJointName(gabi::at<J3DModelData>(md)), name);
    u32 idx = (u16)r;
    u32 joint = gabi::load<u32>(md + 8);
    if (idx < gabi::load<u32>(md + 4))
        joint += idx * 0x1C;
    return joint;
}
/* function-local static sead::SafeString arrays (initialised on first use, destroyed at exit) */
static inline void static_names_init(u32 guard, u32 arr, const u32* names, int n, u32 dtor_rec) {
    if (gabi::load<u32>(guard) == 0) {
        gabi::store<u32>(guard, 1);
        for (int i = 0; i < n; i++) {
            gabi::store<u32>(arr + i * 8 + 4, ZL1_SAFESTRING_VTBL);
            gabi::store<u32>(arr + i * 8, names[i]);
        }
        __register_global_object(dtor_rec);
    }
}
/* the material index of a SafeString name (027F68F4: the material name table header) */
static inline s32 materialIndexOf(u32 md, SafeString* name) {
    u32 h = gabi::call<u32>(0x027F68F4, md);
    SafeString_assure(name);
    return gabi::call<s32>(0x027DF9B0, selfrel(h + 0x18), gabi::at<const char>(name->mStringTop));
}
/* modelData->getJointNodePointer(jnt)->setCallBack(cb) (HD inline) */
static inline void setJointCallBack(daNpc_Zl1_c* i_this, s8 jnt, u32 cb) {
    J3DModelData* md = J3DModel_getModelData_l(i_this->mpMorf->getModel());
    u32 idx = (u16)(s16)jnt;
    u32 n = gabi::load<u32>(gabi::ea(md) + 4);
    u32 joint = gabi::load<u32>(gabi::ea(md) + 8);
    if (idx < n)
        joint += idx * 0x1C;
    gabi::store<u32>(joint + 8, cb);
}

/* 02302B08 */
BOOL daNpc_Zl1_c::bodyCreateHeap() {
    WWHD_FUNC(0x02302B08, BOOL, this);
    J3DModelData* a_mdl_dat = (J3DModelData*)dComIfG_getObjectRes_name(mArcName, STR(0x10023D9C) /* "zl.bdl" */);
    if (a_mdl_dat == nullptr) /* JUT_ASSERT(0x104C, a_mdl_dat != NULL) */
        JUT_ASSERT_fail(STR(0x10023DC4), 0x104C, STR(0x10023E10));
    u32 md = gabi::ea(a_mdl_dat);
    /* HD: only the two eye materials ("eyeL"/"eyeR", the static SafeStrings at 0x10468BA4) get a
     * daNpc_Zl1_matAnm_c (GameCube: every material) */
    for (int i = 0; i < 2; i++) {
        SafeString* name_obj = gabi::at<SafeString>(0x10468BA4 + i * 8);
        u32 hdr = gabi::load<u32>(md);
        SafeString_assure(name_obj);
        s32 idx = gabi::call<s32>(0x027DF9B0, selfrel(hdr + 0x18), gabi::at<const char>(name_obj->mStringTop));
        u32 mat = materialByIndex(md, idx);
        daNpc_Zl1_matAnm_c* anm = (daNpc_Zl1_matAnm_c*)operator_new(0x80);
        if (anm != nullptr)
            anm = daNpc_Zl1_matAnm_c_ct(anm);
        gabi::store<u32>(mat + 0x24, gabi::ea(anm)); /* mat->setMaterialAnm() */
    }
    mpMorf = mDoExt_McaMorf::create(nullptr, a_mdl_dat, nullptr, nullptr, nullptr, -1 /* EMode_NULL */, 1.0f, 0, -1, 1, nullptr, 0x80000, 0x11020222);
    if (mpMorf.get() == nullptr) {
        return FALSE;
    }
    if (mpMorf->getModel() == nullptr || !init_texPttrnAnm(1, false)) {
        mpMorf = nullptr; /* HD: the morf is not deleted */
        return FALSE;
    }
    /* HD: the joints are found by name (GameCube: joint numbers 0, 0xB, 0xC) */
    mJoint1 = gabi::at<J3DJoint>(jointByName(md, STR(0x10023E24) /* "world_root" */));
    mJoint2 = gabi::at<J3DJoint>(jointByName(md, STR(0x10023DA4) /* "zl_eye" */));
    mJoint3 = gabi::at<J3DJoint>(jointByName(md, STR(0x10023D7C) /* "zl_mayu" */));
    /* HD: the indices of two head materials ("zelda_zhead", "zelda_zhead_2_") */
    static const u32 head_names[2] = {0x10023DD4, 0x10023E30};
    static_names_init(0x10468CD8, 0x10468B24, head_names, 2, 0x101C715C);
    mB0C = materialIndexOf(md, gabi::at<SafeString>(0x10468B24));
    mB10 = materialIndexOf(md, gabi::at<SafeString>(0x10468B2C));
    /* HD: the eye and eyebrow materials are found by name (GameCube: the meshes of the joints) */
    static const u32 eye_names[6] = {0x10023E40, 0x10023E4C, 0x10023DAC, 0x10023E58, 0x10023E64, 0x10023DB4};
    static_names_init(0x10468CDC, 0x10468B34, eye_names, 6, 0x101C7168);
    for (int no = 0; no < 6; no++) {
        SafeString* name_obj = gabi::at<SafeString>(0x10468B34 + no * 8);
        u32 hdr = gabi::load<u32>(md);
        SafeString_assure(name_obj);
        s32 idx = gabi::call<s32>(0x027DF9B0, selfrel(hdr + 0x18), gabi::at<const char>(name_obj->mStringTop));
        u32 mat = materialByIndex(md, idx);
        field_0x860[no] = mat;
        field_0x890[no] = gabi::load<u32>(mat + 8); /* getShape() */
    }
    static const u32 mayu_names[6] = {0x10023DE0, 0x10023DEC, 0x10023D84, 0x10023DF8, 0x10023E04, 0x10023D8C};
    static_names_init(0x10468CE0, 0x10468B64, mayu_names, 6, 0x101C7174);
    for (int no = 0; no < 6; no++) {
        SafeString* name_obj = gabi::at<SafeString>(0x10468B64 + no * 8);
        u32 hdr = gabi::load<u32>(md);
        SafeString_assure(name_obj);
        s32 idx = gabi::call<s32>(0x027DF9B0, selfrel(hdr + 0x18), gabi::at<const char>(name_obj->mStringTop));
        u32 mat = materialByIndex(md, idx);
        field_0x878[no] = mat;
        field_0x8A8[no] = gabi::load<u32>(mat + 8);
    }
    m_hed_jnt_num = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), STR(0x10023DBC) /* "head" */);
    if (m_hed_jnt_num < 0)
        JUT_ASSERT_fail(STR(0x10023DC4), 0x10C6, STR(0x10023E70));
    m_bbone_jnt_num = JUTNameTab_getIndex(J3DModelData_getJointName(a_mdl_dat), STR(0x10023D94) /* "chest" */);
    if (m_bbone_jnt_num < 0)
        JUT_ASSERT_fail(STR(0x10023DC4), 0x10C8, STR(0x10023E84));
    setJointCallBack(this, m_hed_jnt_num, 0x02302278 /* nodeCB_Head */);
    setJointCallBack(this, m_bbone_jnt_num, 0x023023E4 /* nodeCB_BackBone */);
    gabi::store<u32>(gabi::ea(mpMorf->getModel()) + 0xB8, gabi::ea(this)); /* setUserArea(this) */
    return TRUE;
}
VERIFY(0x02302B08, &daNpc_Zl1_c::bodyCreateHeap);
