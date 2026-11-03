// Emitter-resource tables, adapted from tww JPAResourceManager.cpp.
#include "JSystem/JParticle/JPAResourceManager.h"
#include "sead/ElementAt.h"
void* JPAResourceNew(u32);
void* JPAResourceArrayNew(u32, void*, s32);
void JPAResourceAssert(const char*, s32, const char*);
void* JPAResourceCurrentHeap();
void JPAResourceDelete(void*);
void JPAResourceRegister(void*);
void* JPAResourceLookupTexture(const void*,const char*);
void JPAResourceStoreTexture(void*,void*);
void JPAResourceDestroyArray(void*,u32,u32,u32,u32,u32);
void* JPAResourceConstructArray(void*,u32,u32,u32,u32);
JPATextureNameEntry* JPATextureNameEntryConstruct(JPATextureNameEntry*);
void JPATextureNameEntryDestroy(JPATextureNameEntry*,u32);
namespace {
struct TextureNameTable { JPATextureNameEntry entries[194]; };
static TextureNameTable sTextureNames;
static const u32 sNameVtable;
static const u32 sArrayCookie;
struct ResourceStaticBlock { be<u32> words[4]; };
static ResourceStaticBlock sStaticBlock;
static u32 sStaticRegistration;
static u32 sTextureNamesRegistration;
// HD: these names are code-defined registration keys, not texture asset data.
static const char* const sTextureNameLiterals[194] = {
    "AK_am00",
    "AK_batta00",
    "AK_batta01",
    "AK_bird00",
    "AK_bird01",
    "AK_birdStatue00",
    "AK_birdStatue01",
    "AK_bkmFloor00",
    "AK_bkmFloor01",
    "AK_bkmTutiKusa00",
    "AK_bkmTutiKusa01",
    "AK_blurInd00",
    "AK_blurMask00",
    "AK_bmos00",
    "AK_bpw00",
    "AK_bpw01",
    "AK_bpw02",
    "AK_bstImpactInd",
    "AK_bstImpactMask",
    "AK_btd00",
    "AK_btd01",
    "AK_btd02",
    "AK_darkShot00",
    "AK_ganonBossDoor00",
    "AK_ganonBossDoor01",
    "AK_ganonBossDoor02",
    "AK_gizagiza00",
    "AK_grass00",
    "AK_greatFairy00",
    "AK_hinoko",
    "AK_hit01",
    "AK_hit02",
    "AK_hitFlash00",
    "AK_honoo000",
    "AK_honoo001",
    "AK_honoo002",
    "AK_honoo003",
    "AK_honoo004",
    "AK_honoo005",
    "AK_honoo006",
    "AK_honoo007",
    "AK_honoo008",
    "AK_honoo009",
    "AK_houshi00",
    "AK_houshi01",
    "AK_kagerouInd01",
    "AK_kagerouIndMM00",
    "AK_kagerouMask00",
    "AK_kagerouSwap00",
    "AK_kazanbai00",
    "AK_kikuzu00",
    "AK_kikuzu01",
    "AK_kindanVine00",
    "AK_kirakira00",
    "AK_kirakira01",
    "AK_kirakira02",
    "AK_lightButton00",
    "AK_magma00",
    "AK_majuCeramic00",
    "AK_omen00",
    "AK_omen01",
    "AK_onpu00",
    "AK_onpu01",
    "AK_onpu02",
    "AK_onpu03",
    "AK_rock",
    "AK_roundAttackMain",
    "AK_roundAttackMain3",
    "AK_roundAttackMask",
    "AK_sand00",
    "AK_shipTailInd",
    "AK_shipTailMain",
    "AK_shipTailMask",
    "AK_shipWave00",
    "AK_sibouFlash00",
    "AK_sibouFlash01",
    "AK_sibouFlash02",
    "AK_sibouSpirit00",
    "AK_sirenShaft00",
    "AK_sirenShaft01",
    "AK_smallSmoke00",
    "AK_smallSmoke01",
    "AK_smoke01",
    "AK_smoke02",
    "AK_smoke03",
    "AK_smoke04",
    "AK_smoke05",
    "AK_splash01",
    "AK_splash02",
    "AK_titleWind00",
    "AK_tuba00",
    "AK_warpFlash00",
    "AK_warpHoleFlower",
    "AK_wind00",
    "AK_windChest00",
    "AK_windOctopus00",
    "bitya",
    "blur_mask_ia4_06",
    "deftex",
    "dk_feather00",
    "flash00",
    "flash02",
    "Flash_test00",
    "glow01",
    "glow02",
    "hamon2_02",
    "hamon2_03",
    "happa01",
    "happa02",
    "hibana",
    "icerock32",
    "kf_yuka01",
    "kinohahen00",
    "koishi00",
    "kusa_half",
    "kusa_half2",
    "lkblur_col_12",
    "mag_hahen",
    "mizukatamari00",
    "mizukatamari01",
    "mizutama00",
    "mizutama01",
    "mizutama02",
    "mizutama03",
    "mizuyuka",
    "puchiFlash",
    "quarter",
    "ring_halo00",
    "ring_halo01",
    "sankaku",
    "senko_IA",
    "shikaku",
    "smallcloud",
    "smoke01",
    "smoke02",
    "smoke03",
    "suiteki",
    "suiteki01",
    "toon",
    "tori_no_hane01",
    "Txm_tuboH_1",
    "Txm_tuboH_2",
    "Txm_tuboH_3",
    "Txm_tuboH_4",
    "zel_3en_ia8",
    "zel_bikon_ia4_00",
    "zel_bikon_ia4_01",
    "zel_block_ia4_00",
    "zel_block_ia4_01",
    "zel_dekuLeaf_ia4_00",
    "zel_enchu_ia400",
    "zel_en_ia4_00",
    "zel_exSmoke_ia4_00",
    "zel_feather_rgb00",
    "zel_gbacursor_i8",
    "zel_gIFlash_ia8_00",
    "zel_hahen_ia4_00",
    "zel_hahen_ia4_01",
    "zel_hahen_ia4_05",
    "zel_hahen_ia4_06",
    "zel_hahen_ia4_07",
    "zel_hanapachi_i8",
    "zel_hasunoura_ia8",
    "zel_hringia8_00",
    "zel_indwp_ia8_00",
    "zel_kKusa_ia4_00",
    "zel_maru_ia8_00",
    "zel_matsu_ci8_00",
    "zel_mizunure_ia8",
    "zel_mizutama_demo",
    "zel_moko_ia4_00",
    "zel_moko_ia4_01",
    "zel_ngrad_i4_00",
    "zel_pearlGlow_ia8",
    "zel_piyo_ia4_00",
    "zel_piyo_ia4_01",
    "zel_ring_ia8_01",
    "zel_ring_quarter01",
    "zel_snow_i4_00",
    "zel_star4_i8",
    "zel_star_ia4_00",
    "zel_suna_ia4_00",
    "zel_swimGrad_ia8_05",
    "zel_sWood_ia4_00",
    "zel_takaraF_ia8_00",
    "zel_tfFly_oppo",
    "zel_tingle",
    "zel_toge_ci8_00",
    "zel_warptex_ia8",
    "zel_woodtx_ia4_00",
    "zel_woodtx_ia4_01",
    "zel_wpot_bitya_ia4",
    "zel_yodapota_ia4_00",
    "zel_yodare_ia4_00",
};
}
void JPAResourceArchiveLoad(const void*, void*, gptr<JPAEmitterResource>*, gptr<JPATextureResource>*, void*);

/* 02826A74 */
JPATextureResource* JPATextureResourceConstruct(JPATextureResource* self, u32 num, void* heap, void* context) {
    if (!self) {
        self = static_cast<JPATextureResource*>(JPAResourceNew(sizeof(JPATextureResource)));
        if (!self) return nullptr;
    }
    self->maxNum = num;
    self->registNum = 0;
    self->defaultTex.imgBuf = nullptr;
    void* array = JPAResourceArrayNew(num * 4, heap, 4);
    self->pTexResArray = static_cast<gptr<JPATextureArc>*>(array);
    if (!array) JPAResourceAssert("JPAResourceManager.cpp", 223, "pTexResArray");
    self->defaultTex.initialize(heap);
    array = JPAResourceArrayNew(u32(self->maxNum) * 4, heap, 4);
    u32 count = self->maxNum;
    self->mDescriptors = static_cast<gptr<void>*>(array);
    u32 size = count * 16 + u32(sArrayCookie);
    void* storage = JPAResourceArrayNew(size, heap, 4);
    JPATextureNameEntry* names = nullptr;
    if (storage) {
        // HD: the compiler array cookie is read again after allocation.
        auto first = static_cast<u8*>(storage) + u32(sArrayCookie);
        names = static_cast<JPATextureNameEntry*>(JPAResourceConstructArray(first, count, 16,
            gabi::guest_fn(reinterpret_cast<const void*>(JPATextureNameEntryConstruct)), 0));
    }
    self->mNames = names;
    self->mContext = context;
    self->mDefaultTextureIndex = -1;
    return self;
}

static void assureStringTermination(SafeString* string) {
    // The canonical SafeString stores the vtable as a scalar guest address.
    auto table = gabi::at<JPAStringVtable>(u32(string->__vtbl));
    gabi::call_ptr<void>(table->assureTermination, string);
}
static const be<u8>* stringBytes(u32 address) { return gabi::at<const be<u8>>(address); }
static bool equalTextureNames(SafeString* name, SafeString* expected) {
    assureStringTermination(name);
    assureStringTermination(name);
    u32 before = name->mTextPtr;
    assureStringTermination(expected);
    u32 expectedPointer = expected->mTextPtr;
    if (before == expectedPointer) return true;
    const be<u8>* left = stringBytes(name->mTextPtr);
    const be<u8>* right = stringBytes(expectedPointer);
    constexpr u32 MAX_STRING_COMPARE = 0x40001;
    for (u32 i = 0; i < MAX_STRING_COMPARE; ++i) {
        u32 a = left[i], b = right[i];
        if (a != b) return false;
        if (a == 0) return true;
    }
    return false;
}

/* 02826B98 */
void JPATextureResource::registration(void* texture, SafeString* name) {
    if (u32(registNum) >= u32(maxNum))
        JPAResourceAssert("JPAResourceManager.cpp", 302, "registNum < maxNum");
    auto context = static_cast<JPATextureContext*>(mContext.get());
    assureStringTermination(name);
    s32 offset = context->mTextureDataOffset;
    const void* base = nullptr;
    if (offset) base = reinterpret_cast<const u8*>(&context->mTextureDataOffset) + offset;
    auto buffer = reinterpret_cast<const char*>(stringBytes(name->mTextPtr));
    void* data = JPAResourceLookupTexture(base, buffer);
    JPAResourceStoreTexture(texture, data);
    sead::elementAt(mDescriptors.get(), u32(registNum)) = texture;
    sead::elementAt(mNames.get(), u32(registNum)).mName.mTextPtr = u32(name->mTextPtr);
    sead::elementAt(mNames.get(), u32(registNum)).mMode = 2;
    sead::elementAt(mNames.get(), u32(registNum)).mFlags = 2;
    for (u32 i = 0; i < 194; ++i) {
        if (!equalTextureNames(name, &sTextureNames.entries[i].mName)) continue;
        SafeString defaultName;
        defaultName.__vtbl = gabi::ea(&sNameVtable);
        defaultName.mTextPtr = gabi::guest_addr("deftex");
        if (equalTextureNames(name, &defaultName)) mDefaultTextureIndex = u32(registNum);
        sead::elementAt(mNames.get(), u32(registNum)).mMode = u32(sTextureNames.entries[i].mMode);
        sead::elementAt(mNames.get(), u32(registNum)).mFlags = u32(sTextureNames.entries[i].mFlags);
        registNum = u32(registNum) + 1;
        return;
    }
    registNum = u32(registNum) + 1;
}

/* 02826E60 */
JPAEmitterResource* JPAEmitterResourceConstruct(JPAEmitterResource* self, u32 num, void* heap) {
    // HD: retain the compiler constructor entry's null-this allocation path.
    if (!self) {
        self = static_cast<JPAEmitterResource*>(JPAResourceNew(sizeof(JPAEmitterResource)));
        if (!self) return nullptr;
    }
    self->maxNum = num;
    self->registNum = 0;
    void* array = JPAResourceArrayNew(num * 4, heap, 4);
    self->pEmtrResArray = static_cast<gptr<JPAEmitterData>*>(array);
    if (!array) JPAResourceAssert("JPAResourceManager.cpp", 348, "pEmtrResArray");
    return self;
}

/* 02826EF8 */
JPAEmitterData* JPAEmitterResource::getByUserIndex(u32 userIndex) {
    // HD: the comparison consumes the full incoming argument register.
    u32 count = registNum;
    auto array = pEmtrResArray.get();
    for (u32 i = 0; i < count; ++i) {
        JPAEmitterData* data = array[i];
        if (u32(data->userIndex) == userIndex) return data;
    }
    return nullptr;
}

/* 02826F2C */
void JPAEmitterResource::registration(JPAEmitterData* res, u32 userIndex) {
    if (u32(registNum) >= u32(maxNum))
        JPAResourceAssert("JPAResourceManager.cpp", 362, "registNum < maxNum");
    if (u32(registNum) < u32(maxNum)) {
        res->userIndex = u16(userIndex);
        u32 index = registNum;
        sead::elementAt(pEmtrResArray, index) = res;
    }
    registNum = u32(registNum) + 1;
}

/* 02826FC4 */
bool JPAEmitterResource::checkUserIndexDuplication(u32 userIndex) {
    u32 count = registNum;
    auto array = pEmtrResArray.get();
    for (u32 i = 0; i < count; ++i)
        if (u32(array[i]->userIndex) == userIndex) return true;
    return false;
}

/* 02827000 */
JPAResourceManager* JPAResourceManagerConstruct(JPAResourceManager* self, const void* data, void* heap, void* context) {
    if (!self) {
        self = static_cast<JPAResourceManager*>(JPAResourceNew(sizeof(JPAResourceManager)));
        if (!self) return nullptr;
    }
    // HD: Cafe additionally forwards the resource-conversion context to the loader.
    if (!heap) heap = JPAResourceCurrentHeap();
    self->pHeap = heap;
    JPAResourceArchiveLoad(data, heap, &self->pEmtrRes, &self->pTexRes, context);
    return self;
}

/* 028270AC */
void __sinit_JPAResourceManager_cpp() {
    sStaticBlock.words[3] = 0;
    sStaticBlock.words[2] = 0;
    sStaticBlock.words[1] = 0;
    sStaticBlock.words[0] = 0;
    JPAResourceRegister(&sStaticRegistration);
    for (u32 i = 0; i < 194; ++i) {
        sTextureNames.entries[i].mName.__vtbl = gabi::ea(&sNameVtable);
        sTextureNames.entries[i].mName.mTextPtr = gabi::guest_addr(sTextureNameLiterals[i]);
    }
    JPAResourceRegister(&sTextureNamesRegistration);
}

// HD: compiler-generated texture-name-table lifetime helpers.
/* 02827D34 */
void JPATextureNameTableDestroy() {
    JPAResourceDestroyArray(sTextureNames.entries, 194, sizeof(JPATextureNameEntry),
        gabi::guest_fn(reinterpret_cast<const void*>(JPATextureNameEntryDestroy)), 0, 0);
}
/* 02827D58 */
void JPATextureSafeStringDestroy(void* self, u32 deleting) {
    if (self && (deleting & 1)) JPAResourceDelete(self);
}
/* 02827D6C */
JPATextureNameEntry* JPATextureNameEntryConstruct(JPATextureNameEntry* self) {
    if (!self) {
        self = static_cast<JPATextureNameEntry*>(JPAResourceNew(sizeof(JPATextureNameEntry)));
        if (!self) return nullptr;
    }
    SafeString* name = &self->mName;
    if (gabi::ea(name) == 0) {
        name = static_cast<SafeString*>(JPAResourceNew(sizeof(SafeString)));
        if (!name) return self;
    }
    name->__vtbl = gabi::ea(&sNameVtable);
    name->mTextPtr = gabi::guest_addr("");
    return self;
}
/* 02827DDC */
void JPATextureNameEntryDestroy(JPATextureNameEntry* self, u32 deleting) {
    if (self && (deleting & 1)) JPAResourceDelete(self);
}
/* 02827DF0 */
void JPATextureSafeStringAssureTermination(void*) {}

/* WWHD glue
str 10171318 "JPAResourceManager.cpp" in 02826B98
str 10171330 "registNum < maxNum" in 02826B98
str 10171310 "deftex" in 02826B98
ext 027DFA24 void* JPAResourceLookupTexture(const void*,const char*)
ext 02773798 void JPAResourceStoreTexture(void*,void*)
data sStaticBlock 104B5990
data sStaticRegistration 101FA094
data sTextureNamesRegistration 101FA0A0
ext 028F026C void JPAResourceRegister(void*)
str 101713A4 "AK_am00" in 028270AC
str 101719E0 "AK_batta00" in 028270AC
str 101719EC "AK_batta01" in 028270AC
str 1017176C "AK_bird00" in 028270AC
str 10171778 "AK_bird01" in 028270AC
str 1017143C "AK_birdStatue00" in 028270AC
str 1017144C "AK_birdStatue01" in 028270AC
str 10171784 "AK_bkmFloor00" in 028270AC
str 10171794 "AK_bkmFloor01" in 028270AC
str 101719F8 "AK_bkmTutiKusa00" in 028270AC
str 10171A0C "AK_bkmTutiKusa01" in 028270AC
str 10171A20 "AK_blurInd00" in 028270AC
str 101717A4 "AK_blurMask00" in 028270AC
str 101717B4 "AK_bmos00" in 028270AC
str 10171A30 "AK_bpw00" in 028270AC
str 10171A3C "AK_bpw01" in 028270AC
str 10171A48 "AK_bpw02" in 028270AC
str 1017145C "AK_bstImpactInd" in 028270AC
str 10171A54 "AK_bstImpactMask" in 028270AC
str 10171A68 "AK_btd00" in 028270AC
str 10171A74 "AK_btd01" in 028270AC
str 10171A80 "AK_btd02" in 028270AC
str 101717C0 "AK_darkShot00" in 028270AC
str 10171A8C "AK_ganonBossDoor00" in 028270AC
str 10171AA0 "AK_ganonBossDoor01" in 028270AC
str 10171AB4 "AK_ganonBossDoor02" in 028270AC
str 101717D0 "AK_gizagiza00" in 028270AC
str 10171AC8 "AK_grass00" in 028270AC
str 1017146C "AK_greatFairy00" in 028270AC
str 101717E0 "AK_hinoko" in 028270AC
str 10171AD4 "AK_hit01" in 028270AC
str 10171AE0 "AK_hit02" in 028270AC
str 101717EC "AK_hitFlash00" in 028270AC
str 101715FC "AK_honoo000" in 028270AC
str 10171608 "AK_honoo001" in 028270AC
str 10171614 "AK_honoo002" in 028270AC
str 10171620 "AK_honoo003" in 028270AC
str 1017162C "AK_honoo004" in 028270AC
str 10171638 "AK_honoo005" in 028270AC
str 10171644 "AK_honoo006" in 028270AC
str 10171650 "AK_honoo007" in 028270AC
str 1017165C "AK_honoo008" in 028270AC
str 10171668 "AK_honoo009" in 028270AC
str 10171674 "AK_houshi00" in 028270AC
str 10171680 "AK_houshi01" in 028270AC
str 1017147C "AK_kagerouInd01" in 028270AC
str 101717FC "AK_kagerouIndMM00" in 028270AC
str 10171AEC "AK_kagerouMask00" in 028270AC
str 10171B00 "AK_kagerouSwap00" in 028270AC
str 10171810 "AK_kazanbai00" in 028270AC
str 1017168C "AK_kikuzu00" in 028270AC
str 10171698 "AK_kikuzu01" in 028270AC
str 1017148C "AK_kindanVine00" in 028270AC
str 10171820 "AK_kirakira00" in 028270AC
str 10171830 "AK_kirakira01" in 028270AC
str 10171840 "AK_kirakira02" in 028270AC
str 10171B14 "AK_lightButton00" in 028270AC
str 10171B28 "AK_magma00" in 028270AC
str 10171B34 "AK_majuCeramic00" in 028270AC
str 10171850 "AK_omen00" in 028270AC
str 1017185C "AK_omen01" in 028270AC
str 10171868 "AK_onpu00" in 028270AC
str 10171874 "AK_onpu01" in 028270AC
str 10171880 "AK_onpu02" in 028270AC
str 1017188C "AK_onpu03" in 028270AC
str 101713AC "AK_rock" in 028270AC
str 10171B48 "AK_roundAttackMain" in 028270AC
str 101716A4 "AK_roundAttackMain3" in 028270AC
str 10171B5C "AK_roundAttackMask" in 028270AC
str 10171898 "AK_sand00" in 028270AC
str 10171B70 "AK_shipTailInd" in 028270AC
str 1017149C "AK_shipTailMain" in 028270AC
str 101714AC "AK_shipTailMask" in 028270AC
str 101718A4 "AK_shipWave00" in 028270AC
str 101714BC "AK_sibouFlash00" in 028270AC
str 101714CC "AK_sibouFlash01" in 028270AC
str 101714DC "AK_sibouFlash02" in 028270AC
str 10171B80 "AK_sibouSpirit00" in 028270AC
str 101714EC "AK_sirenShaft00" in 028270AC
str 101714FC "AK_sirenShaft01" in 028270AC
str 1017150C "AK_smallSmoke00" in 028270AC
str 1017151C "AK_smallSmoke01" in 028270AC
str 10171B94 "AK_smoke01" in 028270AC
str 10171BA0 "AK_smoke02" in 028270AC
str 10171BAC "AK_smoke03" in 028270AC
str 10171BB8 "AK_smoke04" in 028270AC
str 10171BC4 "AK_smoke05" in 028270AC
str 101716B8 "AK_splash01" in 028270AC
str 101716C4 "AK_splash02" in 028270AC
str 10171BD0 "AK_titleWind00" in 028270AC
str 101718B4 "AK_tuba00" in 028270AC
str 10171BE0 "AK_warpFlash00" in 028270AC
str 101718C0 "AK_warpHoleFlower" in 028270AC
str 101718D4 "AK_wind00" in 028270AC
str 10171BF0 "AK_windChest00" in 028270AC
str 10171C00 "AK_windOctopus00" in 028270AC
str 1017140C "bitya" in 028270AC
str 10171C14 "blur_mask_ia4_06" in 028270AC
str 10171414 "deftex" in 028270AC
str 10171C28 "dk_feather00" in 028270AC
str 101713B4 "flash00" in 028270AC
str 101713BC "flash02" in 028270AC
str 10171C38 "Flash_test00" in 028270AC
str 1017141C "glow01" in 028270AC
str 10171424 "glow02" in 028270AC
str 101718E0 "hamon2_02" in 028270AC
str 101718EC "hamon2_03" in 028270AC
str 101713C4 "happa01" in 028270AC
str 101713CC "happa02" in 028270AC
str 1017142C "hibana" in 028270AC
str 101718F8 "icerock32" in 028270AC
str 10171904 "kf_yuka01" in 028270AC
str 101716D0 "kinohahen00" in 028270AC
str 10171C48 "koishi00" in 028270AC
str 10171910 "kusa_half" in 028270AC
str 10171C54 "kusa_half2" in 028270AC
str 1017191C "lkblur_col_12" in 028270AC
str 1017192C "mag_hahen" in 028270AC
str 10171C60 "mizukatamari00" in 028270AC
str 10171C70 "mizukatamari01" in 028270AC
str 10171C80 "mizutama00" in 028270AC
str 10171C8C "mizutama01" in 028270AC
str 10171C98 "mizutama02" in 028270AC
str 10171CA4 "mizutama03" in 028270AC
str 10171CB0 "mizuyuka" in 028270AC
str 10171CBC "puchiFlash" in 028270AC
str 101713D4 "quarter" in 028270AC
str 101716DC "ring_halo00" in 028270AC
str 101716E8 "ring_halo01" in 028270AC
str 101713DC "sankaku" in 028270AC
str 10171CC8 "senko_IA" in 028270AC
str 101713E4 "shikaku" in 028270AC
str 10171CD4 "smallcloud" in 028270AC
str 101713EC "smoke01" in 028270AC
str 101713F4 "smoke02" in 028270AC
str 101713FC "smoke03" in 028270AC
str 10171404 "suiteki" in 028270AC
str 10171938 "suiteki01" in 028270AC
str 10171434 "toon" in 028270AC
str 10171CE0 "tori_no_hane01" in 028270AC
str 101716F4 "Txm_tuboH_1" in 028270AC
str 10171700 "Txm_tuboH_2" in 028270AC
str 1017170C "Txm_tuboH_3" in 028270AC
str 10171718 "Txm_tuboH_4" in 028270AC
str 10171724 "zel_3en_ia8" in 028270AC
str 10171CF0 "zel_bikon_ia4_00" in 028270AC
str 10171D04 "zel_bikon_ia4_01" in 028270AC
str 10171D18 "zel_block_ia4_00" in 028270AC
str 10171D2C "zel_block_ia4_01" in 028270AC
str 10171730 "zel_dekuLeaf_ia4_00" in 028270AC
str 1017152C "zel_enchu_ia400" in 028270AC
str 10171944 "zel_en_ia4_00" in 028270AC
str 10171D40 "zel_exSmoke_ia4_00" in 028270AC
str 10171954 "zel_feather_rgb00" in 028270AC
str 10171D54 "zel_gbacursor_i8" in 028270AC
str 10171D68 "zel_gIFlash_ia8_00" in 028270AC
str 10171D7C "zel_hahen_ia4_00" in 028270AC
str 10171D90 "zel_hahen_ia4_01" in 028270AC
str 10171DA4 "zel_hahen_ia4_05" in 028270AC
str 10171DB8 "zel_hahen_ia4_06" in 028270AC
str 10171DCC "zel_hahen_ia4_07" in 028270AC
str 10171DE0 "zel_hanapachi_i8" in 028270AC
str 10171968 "zel_hasunoura_ia8" in 028270AC
str 1017153C "zel_hringia8_00" in 028270AC
str 10171DF4 "zel_indwp_ia8_00" in 028270AC
str 10171E08 "zel_kKusa_ia4_00" in 028270AC
str 1017154C "zel_maru_ia8_00" in 028270AC
str 10171E1C "zel_matsu_ci8_00" in 028270AC
str 10171E30 "zel_mizunure_ia8" in 028270AC
str 1017197C "zel_mizutama_demo" in 028270AC
str 1017155C "zel_moko_ia4_00" in 028270AC
str 1017156C "zel_moko_ia4_01" in 028270AC
str 1017157C "zel_ngrad_i4_00" in 028270AC
str 10171990 "zel_pearlGlow_ia8" in 028270AC
str 1017158C "zel_piyo_ia4_00" in 028270AC
str 1017159C "zel_piyo_ia4_01" in 028270AC
str 101715AC "zel_ring_ia8_01" in 028270AC
str 10171E44 "zel_ring_quarter01" in 028270AC
str 10171E58 "zel_snow_i4_00" in 028270AC
str 10171E68 "zel_star4_i8" in 028270AC
str 101715BC "zel_star_ia4_00" in 028270AC
str 101715CC "zel_suna_ia4_00" in 028270AC
str 10171744 "zel_swimGrad_ia8_05" in 028270AC
str 10171E78 "zel_sWood_ia4_00" in 028270AC
str 10171E8C "zel_takaraF_ia8_00" in 028270AC
str 10171EA0 "zel_tfFly_oppo" in 028270AC
str 10171EB0 "zel_tingle" in 028270AC
str 101715DC "zel_toge_ci8_00" in 028270AC
str 101715EC "zel_warptex_ia8" in 028270AC
str 101719A4 "zel_woodtx_ia4_00" in 028270AC
str 101719B8 "zel_woodtx_ia4_01" in 028270AC
str 10171EBC "zel_wpot_bitya_ia4" in 028270AC
str 10171758 "zel_yodapota_ia4_00" in 028270AC
str 101719CC "zel_yodare_ia4_00" in 028270AC
str 101712E0 "JPAResourceManager.cpp" in 02826A74
str 101712F8 "pTexResArray" in 02826A74
data sArrayCookie 101FCD08
ext 028EFF8C void* JPAResourceConstructArray(void*,u32,u32,u32,u32)
str 10000160 ""
data sTextureNames 101FA0AC
data sNameVtable 101712C8
ext 0273AF40 void JPAResourceDelete(void*)
ext 028F0164 void JPAResourceDestroyArray(void*,u32,u32,u32,u32,u32)
str 10171354 "JPAResourceManager.cpp" in 02826E60
str 10171344 "pEmtrResArray" in 02826E60
str 1017136C "JPAResourceManager.cpp" in 02826F2C
str 10171384 "registNum < maxNum" in 02826F2C
ext 0273AD10 void* JPAResourceNew(u32)
ext 0273B0D4 void* JPAResourceArrayNew(u32,void*,s32)
ext 0273AA24 void JPAResourceAssert(const char*,s32,const char*)
ext 027EC230 void* JPAResourceCurrentHeap()
ext 02847D14 void JPAResourceArchiveLoad(const void*,void*,gptr<JPAEmitterResource>*,gptr<JPATextureResource>*,void*)
*/
