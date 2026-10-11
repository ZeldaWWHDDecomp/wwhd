/**
 * d_a_npc_zl1_heap.cpp (WWHD)
 * NPC - Tetra: resources, heap, construction and static data.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_zl1.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_npc_zl1_heap.h"

const gc_tu_data gc_tu = {.safeString=ZL1_SAFESTRING_VTBL};
static const char* a_BTPName_tbl[] = {"hang", "maba_a", "zl_t01", "zl_t02", "zl_t03", "zl_t04", "zl_t05", "zl_t06", "fall", "hang_m1", "hang_m2", "hang_m3", "maba_a", "swoon", "wake", "zl_t07", "zl_t08"};
static const char* a_BTKName_tbl[] = {"center", "maba_a", "zl_st01", "zl_st02", "hang_m2", "hang_m3", "look", "swoon", "wake", "zl_t07"};
static const char* a_BCKName_tbl[] = {"wait", "wait02", "talk01", "talk02", "talk03", "fall", "hang", "hang_mov", "wait03", "walk", "land", "run", "look", "swoon", "wake", "wait04", "wait05"};
static const u32 a_siz_tbl[] = {0x0, 0x0};
static s32 l_checkWork = 0;
static fopAc_ac_c* l_checkActors[20] = {};
static char l_BCKName[32] = {};
static char l_BTPName[32] = {};
static char l_BTKName[32] = {};
static const s8 a_btk_num_tbl[] = {0, 1, 2, 1, 1, 1, 3, 1, 0, 0, 4, 5, 6, 7, 8, 9, 1};
static SafeString l_eyeMaterials[2] = {};

static SafeString l_headNames[2] = {};
static SafeString l_eyeNames[6] = {};
static SafeString l_eyebrowNames[6] = {};
static u32 l_headerZero[4] = {};
static u32 l_headGuard = {};
static u32 l_eyeGuard = {};
static u32 l_eyebrowGuard = {};
static daNpc_Zl1_HIO_c l_heapHIO = {};
static zl1Heap::HeaderStatics l_headerStatics = {};
static zl1Heap::DestructorRecord l_headDestructor = {};
static zl1Heap::DestructorRecord l_eyeDestructor = {};
static zl1Heap::DestructorRecord l_eyebrowDestructor = {};
static zl1Heap::DestructorRecord l_headerDestructors[3] = {};
static const daNpc_Zl1_HIO_c::hio_prm_c a_prm_tbl = {6370, 9000, -6370, -9000, 3000, 1000, -1820, -1000, 4096, 2048, 140.0f, 0, 0, 0, 0, 200.0f, 200.0f, 40, 20, 0, 60, 130.0f, 0.03999999910593033f, 10.0f, 0.0f, 1.0f, 1.0f, 200.0f, 0.25f, 25.0f, 10.0f, 384, 5000};

/* inlined into bckResID, btpResID and btkResID */
static void appendExtension(be<char>* destination, const char* source) {
    auto* end = reinterpret_cast<be<u8>*>(destination);
    auto* text = zl1Heap::bytes(gabi::guest_str(source));
    while (*end != 0) ++end;
    u8 value;
    do {
        value = *text++;
        *end++ = value;
    } while (value != 0);
}

/* a bounded strcmp of two SafeStrings (sead::SafeString::isEqual, HD inline) */
/* inlined into daNpc_Zl1_c::setBtp */
static bool SafeString_isEqual(const be<u8>* a, const be<u8>* b) {
    if (a == b) return true;
    for (u32 remaining = 0x40001; remaining != 0; --remaining) {
        u8 c0 = *a;
        u8 c1 = *b;
        if (c0 != c1) return false;
        if (c0 == 0) return true;
        ++a;
        ++b;
    }
    return false;
}

static void SafeString_dt(SafeString* i_this, s32 flags);

/* inlined into bodyCreateHeap */
static void initMaterialNames(be<u32>* guard, SafeString* names, const char* const* source,
                              int count, zl1Heap::DestructorRecord* destructor) {
    if (*guard == 0) {
        *guard = 1;
        for (int i = 0; i < count; ++i) {
            names[i].__vtbl = ZL1_SAFESTRING_VTBL;
            names[i].mStringTop = gabi::guest_str(source[i]);
        }
        __register_global_object(gabi::ea(destructor));
    }
}
/* inlined into bodyCreateHeap */
static s32 materialIndexOf(J3DModelData* data, SafeString* name) {
    auto* header = zl1Heap::resourceHeader(zl1Heap_materialNameHeader(gabi::ea(data)));
    zl1Heap::assure(name);
    return zl1Heap_nameIndex(gabi::ea(header->mMaterialNames.get()), zl1Heap::text(name->mStringTop));
}
/* inlined into bodyCreateHeap */
static zl1Heap::Material* materialByName(J3DModelData* data, SafeString* name) {
    auto* header = zl1Heap::modelData(data)->mpResource.get();
    zl1Heap::assure(name);
    s32 index = zl1Heap_nameIndex(gabi::ea(header->mMaterialNames.get()), zl1Heap::text(name->mStringTop));
    return zl1Heap::materialByIndex(data, index);
}

static daNpc_Zl1_c* daNpc_Zl1_c_ct(daNpc_Zl1_c* i_this);

// HD: null-this allocation and vtable-first HIO layout.
/* 0230A0EC */
static daNpc_Zl1_HIO_c* daNpc_Zl1_HIO_c_ct(daNpc_Zl1_HIO_c* i_this) {
    if (i_this == nullptr) {
        i_this = (daNpc_Zl1_HIO_c*)operator_new(0x64);
        if (i_this == nullptr)
            return i_this;
    }
    i_this->__vtbl = zl1Heap::hioVtable;
    memcpy_g(&i_this->mPrmTbl, &a_prm_tbl, sizeof(a_prm_tbl));
    i_this->mNo = -1;
    i_this->field_0x8 = -1;
    return i_this;
}

// HD: null-this allocation and the inlined base vtable store.
/* 0230242C */
static daNpc_Zl1_matAnm_c* daNpc_Zl1_matAnm_c_ct(daNpc_Zl1_matAnm_c* i_this) {
    if (i_this == nullptr) {
        i_this = (daNpc_Zl1_matAnm_c*)operator_new(0x80);
        if (i_this == nullptr)
            return i_this;
    }
    i_this->field_0x7C = 0;
    i_this->mOffset.x = 0.0f;
    i_this->__vtbl = zl1Heap::materialAnmVtable;
    i_this->mOffset.y = 0.0f;
    return i_this;
}

// HD: resolve the texture matrix by name rather than scanning eight matrix slots.
/* 023086FC */
static void daNpc_Zl1_matAnm_c_calc(daNpc_Zl1_matAnm_c* i_this, void* i_material) {
    auto* material = reinterpret_cast<zl1Heap::Material*>(i_material);
    auto* resource = material->mpResource.get();
    s32 index = zl1Heap_nameIndex(gabi::ea(resource->mTexMatrixNames.get()), "texmtx1");
    resource = material->mpResource.get();
    auto* entry = zl1Heap::texMatrix(resource, index);
    if (entry->mAnimationIndex >= 0) {
        material->mFlags = material->mFlags | 4;
        be<u32>* bits = &material->mpDirtyBits.get()[index >> 5];
        *bits = *bits | (1u << (index & 31));
        resource = material->mpResource.get();
    }
    u32 relatedIndex = entry->mRelatedMatrix;
    auto* related = zl1Heap::texMatrix(resource, relatedIndex);
    if (related->mAnimationIndex >= 0) {
        material->mFlags = material->mFlags | 4;
        be<u32>* bits = &material->mpDirtyBits.get()[(s32)relatedIndex >> 5];
        *bits = *bits | (1u << (relatedIndex & 31));
    }
    auto* srt = reinterpret_cast<zl1Heap::TextureSRT*>(material->mpTextureSRT.get() + (u16)entry->mSRTOffset);
    if (i_this->field_0x7C != 0) {
        srt->mTranslationX = i_this->mOffset.x;
        srt->mTranslationY = i_this->mOffset.y;
    }
}

/* 023035BC */
static void* searchActor_Branch(void* i_actorP, void*) {
    if (l_checkWork < zl1Heap::SearchMax && fopAc_IsActor(i_actorP) && i_actorP != nullptr && fpcM_GetName(i_actorP) == zl1Heap::Branch) {
        s32 n = l_checkWork;
        l_checkWork = n + 1;
        l_checkActors[n] = (fopAc_ac_c*)i_actorP;
    }
    return nullptr;
}

/* 0230363C */
static void* searchActor_Bm1(void* i_actorP, void*) {
    if (l_checkWork < zl1Heap::SearchMax && fopAc_IsActor(i_actorP) && i_actorP != nullptr && fpcM_GetName(i_actorP) == zl1Heap::Bm1) {
        s32 n = l_checkWork;
        l_checkWork = n + 1;
        l_checkActors[n] = (fopAc_ac_c*)i_actorP;
    }
    return nullptr;
}

/* 0230600C */
u32 daNpc_Zl1_c::bckResID(int param_1) {
    zl1Heap_strcpy(l_BCKName, a_BCKName_tbl[param_1]);
    appendExtension(l_BCKName, ".bck");
    return gabi::ea(l_BCKName);
}

/* 02302484 */
u32 daNpc_Zl1_c::btpResID(int param_1) {
    zl1Heap_strcpy(l_BTPName, a_BTPName_tbl[param_1]);
    appendExtension(l_BTPName, ".btp");
    return gabi::ea(l_BTPName);
}

/* 02302834 */
u32 daNpc_Zl1_c::btkResID(int param_1) {
    zl1Heap_strcpy(l_BTKName, a_BTKName_tbl[param_1]);
    appendExtension(l_BTKName, ".btk");
    return gabi::ea(l_BTKName);
}

// HD: preserve the full animation-number and modify-flag registers.
/* 023024F4 */
u32 daNpc_Zl1_c::setBtp(s32 param_1, u32 param_2) {
    J3DModel* model = mpMorf->getModel();
    if (param_1 < 0) {
        return false;
    }
    void* a_btp = zl1Heap::objectResource(mArcName, zl1Heap::text(zl1Heap_btpResID(this, param_1) /* btpResID */));
    // HD: the GameCube resource assertion is absent.
    field_0x847 = (s8)param_1;
    mBtpAnmFrame = 0;
    mTimer = 0;
    s32 ret = zl1Heap_btpInit(mBtpAnm, model->getModelData(), a_btp, 1, 0, 1.0f, 0, -1, param_2, 0);
    zl1Heap_btpInit(mBtpAnm2, model->getModelData(), a_btp, 1, 0, 1.0f, 0, -1, param_2, 0);
    auto* pattern = zl1Heap::btp(mBtpAnm2)->mpAnimation.get();
    J3DModelData* data = mpMorf->getModel()->getModelData();
    u16 materialNum = pattern->mUpdateMaterialCount;
    auto* links = &zl1Heap::btp(mBtpAnm2)->mpMaterialLinks;
    // HD: link both pattern-animation instances to the named model materials.
    for (u32 i = 0; i < materialNum; ++i) {
        zl1Heap::BtpEntry* entry = &pattern->mMaterials.get()[i];
        gabi::Local<SafeString> name;
        name->mStringTop = gabi::ea(entry->mName.get());
        name->__vtbl = ZL1_SAFESTRING_VTBL;
        auto* header = zl1Heap::resourceHeader(zl1Heap_patternNameHeader(data));
        zl1Heap::assure(name);
        s32 index = zl1Heap_nameIndex(gabi::ea(header->mMaterialNames.get()), zl1Heap::text(name->mStringTop));
        gabi::Local<SafeString> mouth;
        mouth->mStringTop = gabi::guest_str("zelda_mouth");
        mouth->__vtbl = ZL1_SAFESTRING_VTBL;
        zl1Heap::assure(name);
        zl1Heap::assure(name);
        u32 text = name->mStringTop;
        zl1Heap::assure(mouth);
        bool isMouth = SafeString_isEqual(zl1Heap::bytes(text), zl1Heap::bytes(mouth->mStringTop));
        // GHS reloads the link pointer between every masked update.
        if (!isMouth) {
            be<u32>* table = links->get();
            table[i] = table[i] & 0x3FFF8000;
            u32 next = index + 1;
            table = links->get();
            table[i] = table[i] | (next & 0x7FFF);
            table = links->get();
            table[next] = table[next] & 0xC0007FFF;
            table = links->get();
            table[next] = table[next] | ((i << 15) & 0x3FFF8000);
        } else {
            be<u32>* table = links->get();
            table[i] = table[i] | 0xC0007FFF;
            table = links->get();
            table[index] = table[index] | 0x3FFF8000;
        }
    }
    return ret != 0;
}

/* 023028A4 */
void daNpc_Zl1_c::setMat() {
    auto* animation = zl1Heap::btk(mBtkAnm)->mpAnimation.get();
    u16 materialNum = animation->mpMaterials->mCount;
    J3DModel* model = mpMorf->getModel();
    // HD: match eye materials by name rather than using the animation's material IDs.
    for (u16 i = 0; i < materialNum; ++i) {
        SafeString* name = &l_eyeMaterials[i];
        J3DModelData* data = model->getModelData();
        auto* resource = zl1Heap::modelData(data)->mpResource.get();
        zl1Heap::assure(name);
        s32 index = zl1Heap_nameIndex(gabi::ea(resource->mMaterialNames.get()), zl1Heap::text(name->mStringTop));
        zl1Heap::Material* material = zl1Heap::materialByIndex(data, index);
        field_0x6D4[i] = material->mpMaterialAnm.get();
    }
}

// HD: preserve the full animation-number and modify-flag registers.
/* 02302984 */
u32 daNpc_Zl1_c::setBtk(s32 param_1, u32 param_2) {
    J3DModel* model = mpMorf->getModel();
    if (param_1 < 0) {
        return false;
    }
    void* a_btk = zl1Heap::objectResource(mArcName, zl1Heap::text(zl1Heap_btkResID(this, param_1) /* btkResID */));
    // HD: the GameCube resource assertion is absent.
    field_0x848 = (s8)param_1;
    mBtkAnmFrame = 0;
    if (zl1Heap_btkInit(mBtkAnm, model->getModelData(), a_btk, 1, 0, 1.0f, 0, -1, param_2, 0) != 0) {
        if (!param_2) {
            setMat();
        }
        return true;
    }
    return false;
}

/* 02302A90 */
u32 daNpc_Zl1_c::init_texPttrnAnm(s8 param_1, u32 param_2) {
    u32 ret = false;
    if (setBtp(param_1, param_2) != 0) {
        ret = setBtk(a_btk_num_tbl[param_1], param_2);
    }
    return ret;
}

/* 023052B4 */
cPhs_State daNpc_Zl1_c::_create() {
    // HD: the actor constructor is out of line.
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
    if (!fopAcM_entrySolidHeap(this, zl1Heap::CheckCreateHeap, a_siz_tbl[field_0x84E])) {
        return cPhs_ERROR_e;
    }
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpMorf->getModel())); /* fopAcM_SetMtx */
    fopAcM_setCullSizeBox(this, -50.0f, -20.0f, -50.0f, 50.0f, 140.0f, 50.0f);
    if (!createInit()) {
        return cPhs_ERROR_e;
    }
    return state;
}

/* 02302B08 */
BOOL daNpc_Zl1_c::bodyCreateHeap() {
    J3DModelData* a_mdl_dat = (J3DModelData*)zl1Heap::objectResource(mArcName, "zl.bdl" /* "zl.bdl" */);
    if (a_mdl_dat == nullptr) /* JUT_ASSERT(0x104C, a_mdl_dat != NULL) */
        JUT_ASSERT_fail("d_a_npc_zl1.cpp", 0x104C, "a_mdl_dat != (0)");
    // HD: only the two eye materials get an animation object; GameCube uses every material.
    for (int i = 0; i < 2; i++) {
        SafeString* name_obj = &l_eyeMaterials[i];
        zl1Heap::Material* material = materialByName(a_mdl_dat, name_obj);
        daNpc_Zl1_matAnm_c* anm = (daNpc_Zl1_matAnm_c*)operator_new(0x80);
        if (anm != nullptr)
            anm = daNpc_Zl1_matAnm_c_ct(anm);
        material->mpMaterialAnm = anm;
    }
    mpMorf = mDoExt_McaMorf::create(nullptr, a_mdl_dat, nullptr, nullptr, nullptr, -1 /* EMode_NULL */, 1.0f, 0, -1, 1, nullptr, 0x80000, 0x11020222);
    if (mpMorf.get() == nullptr) {
        return FALSE;
    }
    if (mpMorf->getModel() == nullptr || !init_texPttrnAnm(1, false)) {
        // HD: clear the pointer without deleting the morph.
        mpMorf = nullptr;
        return FALSE;
    }
    // HD: find joints by name rather than fixed joint numbers.
    mJoint1 = zl1Heap::jointByName(a_mdl_dat, "world_root" /* "world_root" */);
    mJoint2 = zl1Heap::jointByName(a_mdl_dat, "zl_eye" /* "zl_eye" */);
    mJoint3 = zl1Heap::jointByName(a_mdl_dat, "zl_mayu" /* "zl_mayu" */);
    // HD: retain the two head-material indices.
    static const char* const head_names[] = {"zelda_zhead", "zelda_zhead_2_"};
    initMaterialNames(&l_headGuard, l_headNames, head_names, 2, &l_headDestructor);
    mHeadMaterialIndex = materialIndexOf(a_mdl_dat, &l_headNames[0]);
    mHeadOverlayMaterialIndex = materialIndexOf(a_mdl_dat, &l_headNames[1]);
    // HD: find eye and eyebrow materials by name rather than walking the joint meshes.
    static const char* const eye_names[] = {"eyeLdamB", "eyeLdamA", "eyeL", "eyeRdamB", "eyeRdamA", "eyeR"};
    initMaterialNames(&l_eyeGuard, l_eyeNames, eye_names, 6, &l_eyeDestructor);
    for (int no = 0; no < 6; no++) {
        SafeString* name = &l_eyeNames[no];
        zl1Heap::Material* material = materialByName(a_mdl_dat, name);
        field_0x860[no] = gabi::ea(material);
        field_0x890[no] = material->mShape;
    }
    static const char* const mayu_names[] = {"mayuLdamB", "mayuLdamA", "mayuL", "mayuRdamB", "mayuRdamA", "mayuR"};
    initMaterialNames(&l_eyebrowGuard, l_eyebrowNames, mayu_names, 6, &l_eyebrowDestructor);
    for (int no = 0; no < 6; no++) {
        SafeString* name = &l_eyebrowNames[no];
        zl1Heap::Material* material = materialByName(a_mdl_dat, name);
        field_0x878[no] = gabi::ea(material);
        field_0x8A8[no] = material->mShape;
    }
    m_hed_jnt_num = zl1Heap_jointNameIndex(zl1Heap::jointNames(a_mdl_dat), "head" /* "head" */);
    if (m_hed_jnt_num < 0)
        JUT_ASSERT_fail("d_a_npc_zl1.cpp", 0x10C6, "m_hed_jnt_num >= 0");
    m_bbone_jnt_num = zl1Heap_jointNameIndex(zl1Heap::jointNames(a_mdl_dat), "chest" /* "chest" */);
    if (m_bbone_jnt_num < 0)
        JUT_ASSERT_fail("d_a_npc_zl1.cpp", 0x10C8, "m_bbone_jnt_num >= 0");
    zl1Heap::setJointCallback(this, m_hed_jnt_num, zl1Heap::nodeCB_Head);
    zl1Heap::setJointCallback(this, m_bbone_jnt_num, zl1Heap::nodeCB_BackBone);
    mpMorf->getModel()->setUserArea(gabi::ea(this)); /* setUserArea(this) */
    return TRUE;
}

/* 02303400 */
BOOL daNpc_Zl1_c::itemCreateHeap() {
    if (field_0x84F != 1) {
        mpModel = nullptr;
        return TRUE;
    }
    J3DModelData* a_mdl_dat = (J3DModelData*)zl1Heap::objectResource(mArcName, "cloth.bdl" /* "cloth.bdl" */);
    if (a_mdl_dat == nullptr) /* JUT_ASSERT(0x10E6, a_mdl_dat != NULL) */
        JUT_ASSERT_fail("d_a_npc_zl1.cpp", 0x10E6, "a_mdl_dat != (0)");
    mpModel = mDoExt_J3DModel__create(a_mdl_dat, 0x80000, 0x11000022);
    return mpModel.get() != nullptr;
}

/* 023034DC */
BOOL daNpc_Zl1_c::CreateHeap() {
    if (bodyCreateHeap() == FALSE) {
        return FALSE;
    }
    if (itemCreateHeap() == FALSE) {
        mpMorf = nullptr;
        return FALSE;
    }
    mLightInfluence2.mPower = 0.0f;
    mLightInfluence1.mPower = 0.0f;
    zl1Heap_lightPriority(&mLightInfluence1); /* dKy_plight_priority_set */
    zl1Heap_effectLight(&mLightInfluence2); /* dKy_efplight_set */
    mAcchCir.SetWall(30.0f, 50.0f);
    mObjAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, nullptr, nullptr);
    return TRUE;
}

// HD: compiler-generated and template code of this translation unit.

/* 023036BC */
static void destroyHeadNames() {
    zl1Heap_destroyArray(gabi::ea(l_headNames), 2, sizeof(SafeString), gabi::guest_fn(reinterpret_cast<const void*>(SafeString_dt)), 0, 0);
}

/* 023036E0 */
static void destroyEyeNames() {
    zl1Heap_destroyArray(gabi::ea(l_eyeNames), 6, sizeof(SafeString), gabi::guest_fn(reinterpret_cast<const void*>(SafeString_dt)), 0, 0);
}

/* 02303704 */
static void destroyEyebrowNames() {
    zl1Heap_destroyArray(gabi::ea(l_eyebrowNames), 6, sizeof(SafeString), gabi::guest_fn(reinterpret_cast<const void*>(SafeString_dt)), 0, 0);
}

/* 02303728 */
static daNpc_Zl1_c* daNpc_Zl1_c_ct(daNpc_Zl1_c* i_this) {
    if (i_this == nullptr) {
        i_this = (daNpc_Zl1_c*)operator_new(0xD74);
        if (i_this == nullptr)
            return i_this;
    }
    zl1Heap_npcConstructor(i_this);                    /* fopNpc_npc_c::fopNpc_npc_c */
    i_this->__vtbl = zl1Heap::actorVtable;
    zl1Heap_btkConstructor(i_this->mBtkAnm);           /* mDoExt_btkAnm::mDoExt_btkAnm */
    zl1Heap_btpConstructor(i_this->mBtpAnm);           /* mDoExt_btpAnm::mDoExt_btpAnm */
    zl1Heap_btpConstructor(i_this->mBtpAnm2);
    zl1Heap_rippleConstructor(i_this->mRippleCallBack);   /* dPa_rippleEcallBack::dPa_rippleEcallBack */
    i_this->mLightInfluence2.mHD20 = 1.0f;
    i_this->mLightInfluence1.mHD20 = 1.0f;
    /* mDoExt_offCupOnAupPacket / mDoExt_onCupOffAupPacket: J3DPacket base constructor, then the
     * derived vtable at +0xC */
    zl1Heap_packetConstructor(i_this->mOffCupOnAupPacket1);
    zl1Heap::packet(i_this->mOffCupOnAupPacket1)->mVtable = zl1Heap::offCupVtable;
    zl1Heap_packetConstructor(i_this->mOffCupOnAupPacket2);
    zl1Heap::packet(i_this->mOffCupOnAupPacket2)->mVtable = zl1Heap::offCupVtable;
    zl1Heap_packetConstructor(i_this->mOnCupOffAupPacket1);
    zl1Heap::packet(i_this->mOnCupOffAupPacket1)->mVtable = zl1Heap::onCupVtable;
    zl1Heap_packetConstructor(i_this->mOnCupOffAupPacket2);
    zl1Heap::packet(i_this->mOnCupOffAupPacket2)->mVtable = zl1Heap::onCupVtable;
    return i_this;
}

/* 0230A158 */
static void __sinit_d_a_npc_zl1_cpp() {
    for (int i = 0; i < 4; ++i) l_headerZero[i] = 0;
    __register_global_object(gabi::ea(&l_headerDestructors[0]));
    // GHS stores the positive pi before the negative pi in this TU.
    l_headerStatics.mPi = 3.1415927f;
    l_headerStatics.mNegativePi = -3.1415927f;
    zl1Heap_headerFirst(&l_headerStatics.mFirst);
    __register_global_object(gabi::ea(&l_headerDestructors[1]));
    zl1Heap_headerSecond(&l_headerStatics.mSecond);
    __register_global_object(gabi::ea(&l_headerDestructors[2]));
    // HD: static eye-material names replace the GameCube material-number lookup.
    l_eyeMaterials[0].__vtbl = ZL1_SAFESTRING_VTBL;
    l_eyeMaterials[0].mStringTop = gabi::guest_str("eyeL");
    l_eyeMaterials[1].__vtbl = ZL1_SAFESTRING_VTBL;
    l_eyeMaterials[1].mStringTop = gabi::guest_str("eyeR");
    daNpc_Zl1_HIO_c_ct(&l_heapHIO);
}

/* 0230A228 */
static void SafeString_dt(SafeString* i_this, s32 flags) {
    if (i_this != nullptr && (flags & 1))
        operator_delete(i_this);
}

/* 0230A23C */
static void daNpc_Zl1_c_dt(daNpc_Zl1_c* i_this, s32 flags) {
    if (i_this != nullptr) {
        zl1Heap_packetDestructor(i_this->mOnCupOffAupPacket2, 0); /* J3DPacket::~J3DPacket */
        zl1Heap_packetDestructor(i_this->mOnCupOffAupPacket1, 0);
        zl1Heap_packetDestructor(i_this->mOffCupOnAupPacket2, 0);
        zl1Heap_packetDestructor(i_this->mOffCupOnAupPacket1, 0);
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        zl1Heap_circleDestructor(gabi::ea(&i_this->mAcchCir.m_cir), 2); /* cM3dGCir::~cM3dGCir (mAcchCir.m_cir) */
        zl1Heap::destroyAcch(&i_this->mObjAcch);
        zl1Heap_actorDestructor(i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 0230A308 */
static void SafeString_assureTerminationImpl(SafeString*) {
}

/* WWHD glue
data a_BTPName_tbl 101C70F0
data a_BTKName_tbl 101C7134
data a_BCKName_tbl 101C7188
data a_siz_tbl 101C7180
data l_BCKName 10468BC0 nocheck
data l_BTPName 10468BE0 nocheck
data l_BTKName 10468C00 nocheck
data a_btk_num_tbl 10023D68
data l_eyeMaterials 10468BA4 nocheck
str 10023C78 ".btp"
str 10023D10 ".btk"
str 1002405C ".bck"
ext 02302834 u32 zl1Heap_btkResID(daNpc_Zl1_c* actor, s32 number)
ext 02302484 u32 zl1Heap_btpResID(daNpc_Zl1_c* actor, s32 number)
ext 027DF9B0 s32 zl1Heap_nameIndex(u32 table, const char* name)
ext 027F3F8C u32 zl1Heap_patternNameHeader(J3DModelData* data)
ext 027F68F4 u32 zl1Heap_materialNameHeader(u32 data)
ext 0255A2B8 void zl1Heap_lightPriority(LIGHT_INFLUENCE* light)
ext 0255B9C8 void zl1Heap_effectLight(LIGHT_INFLUENCE* light)
ext 025A1458 void zl1Heap_npcConstructor(daNpc_Zl1_c* actor)
ext 025E7C6C void zl1Heap_btkConstructor(void* animation)
ext 025E7820 void zl1Heap_btpConstructor(void* animation)
ext 025A9084 void zl1Heap_rippleConstructor(void* callback)
ext 027F1278 void zl1Heap_packetConstructor(void* packet)
ext 027F13DC void zl1Heap_packetDestructor(void* packet, s32 flags)
ext 02018034 void zl1Heap_circleDestructor(u32 circle, s32 flags)
ext 025D50BC void zl1Heap_actorDestructor(daNpc_Zl1_c* actor, s32 flags)
ext 028F0164 void zl1Heap_destroyArray(u32 array, s32 count, s32 stride, u32 destructor, s32 flag1, s32 flag2)
ext 028F040C void zl1Heap_strcpy(void* destination, const char* source)
str 10023D04 "zelda_mouth"
str 10024254 "texmtx1"
str 10023EAC "cloth.bdl"
str 10023E9C "d_a_npc_zl1.cpp" in 02303400
str 10023EB8 "a_mdl_dat != (0)" in 02303400
str 10023D9C "zl.bdl"
str 10023DC4 "d_a_npc_zl1.cpp" in 02302B08
str 10023E10 "a_mdl_dat != (0)" in 02302B08
str 10023E24 "world_root"
str 10023DA4 "zl_eye"
str 10023D7C "zl_mayu"
str 10023DBC "head"
str 10023E70 "m_hed_jnt_num >= 0"
str 10023D94 "chest"
str 10023E84 "m_bbone_jnt_num >= 0"
data l_headNames 10468B24 nocheck
data l_eyeNames 10468B34 nocheck
data l_eyebrowNames 10468B64 nocheck
data l_headerZero 10468B94 nocheck
data l_headGuard 10468CD8 nocheck
data l_eyeGuard 10468CDC nocheck
data l_eyebrowGuard 10468CE0 nocheck
data l_heapHIO 10468C20 nocheck
data l_headerStatics 10468B18 nocheck
data l_headDestructor 101C715C nocheck
data l_eyeDestructor 101C7168 nocheck
data l_eyebrowDestructor 101C7174 nocheck
data l_headerDestructors 101C75CC nocheck
data a_prm_tbl 101C7574
str 10024288 "eyeL" in 0230A158
str 10024290 "eyeR" in 0230A158
ext 028ED6F8 void zl1Heap_headerFirst(void* object)
ext 028EAB2C void zl1Heap_headerSecond(void* object)
str 10023DD4 "zelda_zhead"
str 10023E30 "zelda_zhead_2_"
str 10023E40 "eyeLdamB"
str 10023E4C "eyeLdamA"
str 10023DAC "eyeL" in 02302B08
str 10023E58 "eyeRdamB"
str 10023E64 "eyeRdamA"
str 10023DB4 "eyeR" in 02302B08
str 10023DE0 "mayuLdamB"
str 10023DEC "mayuLdamA"
str 10023D84 "mayuL"
str 10023DF8 "mayuRdamB"
str 10023E04 "mayuRdamA"
str 10023D8C "mayuR"
data l_checkWork 10468B14 nocheck
data l_checkActors 10468C84 nocheck
ext 025E789C s32 zl1Heap_btpInit(void* animation, J3DModelData* data, void* resource, s32 play, s32 attribute, f32 rate, s32 start, s32 end, u32 modify, s32 entry)
ext 025E7CE0 s32 zl1Heap_btkInit(void* animation, J3DModelData* data, void* resource, s32 play, s32 attribute, f32 rate, s32 start, s32 end, u32 modify, s32 entry)
ext 027F68FC u32 zl1Heap_jointHeader(J3DModelData* data)
ext 027DF9B0 s8 zl1Heap_jointNameIndex(u32 table, const char* name)
*/
