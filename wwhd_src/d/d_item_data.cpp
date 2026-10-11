/** Item resources and effect lookups, following zeldaret/tww names.
 * HD pointer-table initialization and assertion reloads retain verified behavior. */
#include "d/d_item_data.h"

static ItemArchiveNames l_archiveNames;
static ItemTextureNames l_textureNames;
static ItemResources l_itemResources;
static ItemFieldResources l_fieldResources;
static ItemInfos l_itemInfos;
static ItemEffects l_effects;
static ItemHeaderStatics l_resourceHeaderStatics;
static ItemHeaderStatics l_headerStatics;
static ItemStaticRecords l_resourceRecords;
static ItemStaticRecords l_records;
static ItemAngleBounds l_resourceAngles;
static ItemAngleBounds l_angles;

gptr<const char> (&dItem_data::item_arcname_tbl)[113] = l_archiveNames.mNames;
gptr<const char> (&dItem_data::item_texture_tbl)[116] = l_textureNames.mNames;
dItem_data_item_resource (&dItem_data::item_resource)[256] = l_itemResources.mItems;
dItem_data_field_item_res (&dItem_data::field_item_res)[256] = l_fieldResources.mItems;
dItem_data_item_info (&dItem_data::item_info)[256] = l_itemInfos.mItems;
dItem_data_effect_info (&dItem_data::effect_info)[129] = l_effects.mItems;

// HD: full-u32 effect indexing wraps before the guest-memory access.
static dItem_data_effect_info* itemEffect(u32 no) {
    return gabi::at<dItem_data_effect_info>(gabi::ea(&dItem_data::effect_info[0]) +
                                            no * (u32)sizeof(dItem_data_effect_info));
}

/* 02551B90 */
BOOL dItem_data::checkAppearEffect(u32 no) {
    if (no > item_data::dItemNo_TRIFORCE_MAP_8_e) {
        return FALSE;
    }
    return itemEffect(no)->m_appear != dPa_name::ID_SCENE_END;
}

/* 02551BBC */
u16 dItem_data::getAppearEffect(u32 no) {
    dItem_data_effect_info* effect = itemEffect(no);
    u16 value = effect->m_appear;
    if (value == dPa_name::ID_SCENE_END) {
        JUTAssertion::showAssert("d_item_data.cpp", 0x267B,
                        "effect_info[no].m_appear != dPa_name::ID_SCENE_END");
        value = effect->m_appear;
    }
    return value;
}

/* 02551C18 */
BOOL dItem_data::checkSpecialEffect(u32 no) {
    if (no > item_data::dItemNo_TRIFORCE_MAP_8_e) {
        return FALSE;
    }
    return itemEffect(no)->m_sp_effect != dPa_name::ID_SCENE_END;
}

/* 02551C48 */
u16 dItem_data::getSpecialEffect(u32 no) {
    dItem_data_effect_info* effect = itemEffect(no);
    u16 value = effect->m_sp_effect;
    if (value == dPa_name::ID_SCENE_END) {
        JUTAssertion::showAssert("d_item_data.cpp", 0x2693,
                        "effect_info[no].m_sp_effect != dPa_name::ID_SCENE_END");
        // HD: assertions are an observable call boundary; reload the mutable table entry.
        value = effect->m_sp_effect;
    }
    return value;
}

// HD: compiler-generated and template code of this translation unit.
/* 02551CA4 */
static void __sinit_d_item_data_resources() {
    l_resourceHeaderStatics.mZeroWords[3] = 0;
    l_resourceHeaderStatics.mZeroWords[2] = 0;
    l_resourceHeaderStatics.mZeroWords[1] = 0;
    l_resourceHeaderStatics.mZeroWords[0] = 0;
    Item_registerStatic(&l_resourceRecords.mEntries[0]);
    f32 negativePi = l_resourceAngles.mNegativePi, pi = l_resourceAngles.mPi;
    l_resourceHeaderStatics.mNegativePi = negativePi;
    l_resourceHeaderStatics.mPi = pi;
    Item_initFirstObject(&l_resourceHeaderStatics.mFirstObject);
    Item_registerStatic(&l_resourceRecords.mEntries[1]);
    Item_initSecondObject(&l_resourceHeaderStatics.mSecondObject);
    Item_registerStatic(&l_resourceRecords.mEntries[2]);
    // HD: retain the native loads and scatter order while binding pointers into item metadata.
    const char* textureGetRupy = dItem_data::item_texture_tbl[TEXTURE_IDX_GET_RUPY].get();
    const char* textureHeartUp01 = dItem_data::item_texture_tbl[TEXTURE_IDX_HEART_UP_01].get();
    const char* textureHeartUp02 = dItem_data::item_texture_tbl[TEXTURE_IDX_HEART_UP_02].get();
    const char* archiveVhkak = dItem_data::item_arcname_tbl[ARCNAME_IDX_VHKAK].get();
    const char* textureBottle00 = dItem_data::item_texture_tbl[TEXTURE_IDX_BOTTLE_00].get();
    dItem_data::item_resource[item_data::dItemNo_HEART_PIECE_e].mTexture = textureHeartUp02;
    dItem_data::item_resource[item_data::dItemNo_LARGE_MAGIC_e].mTexture = textureBottle00;
    const char* archiveVlupy = dItem_data::item_arcname_tbl[ARCNAME_IDX_VLUPY].get();
    const char* archiveVhutu = dItem_data::item_arcname_tbl[ARCNAME_IDX_VHUTU].get();
    dItem_data::item_resource[item_data::dItemNo_SMALL_MAGIC_e].mTexture = textureBottle00;
    dItem_data::item_resource[item_data::dItemNo_GREEN_RUPEE_e].mArcname = archiveVlupy;
    dItem_data::item_resource[item_data::dItemNo_BLUE_RUPEE_e].mTexture = textureGetRupy;
    const char* archiveAlways = dItem_data::item_arcname_tbl[ARCNAME_IDX_ALWAYS].get();
    dItem_data::item_resource[item_data::dItemNo_RED_RUPEE_e].mTexture = textureGetRupy;
    dItem_data::item_resource[item_data::dItemNo_YELLOW_RUPEE_e].mArcname = archiveVlupy;
    dItem_data::item_resource[item_data::dItemNo_BLUE_RUPEE_e].mArcname = archiveVlupy;
    dItem_data::item_resource[item_data::dItemNo_YELLOW_RUPEE_e].mTexture = textureGetRupy;
    dItem_data::item_resource[item_data::dItemNo_ORANGE_RUPEE_e].mArcname = archiveVlupy;
    dItem_data::item_resource[item_data::dItemNo_PURPLE_RUPEE_e].mArcname = archiveVlupy;
    dItem_data::item_resource[item_data::dItemNo_HEART_CONTAINER_e].mArcname = archiveVhutu;
    dItem_data::item_resource[item_data::dItemNo_HEART_CONTAINER_e].mTexture = textureHeartUp01;
    dItem_data::item_resource[item_data::dItemNo_SMALL_MAGIC_e].mArcname = archiveAlways;
    dItem_data::item_resource[item_data::dItemNo_HEART_PIECE_e].mArcname = archiveVhkak;
    dItem_data::item_resource[item_data::dItemNo_ORANGE_RUPEE_e].mTexture = textureGetRupy;
    dItem_data::item_resource[item_data::dItemNo_PURPLE_RUPEE_e].mTexture = textureGetRupy;
    const char* archiveBomb00 = dItem_data::item_arcname_tbl[ARCNAME_IDX_BOMB_00].get();
    dItem_data::item_resource[item_data::dItemNo_LARGE_MAGIC_e].mArcname = archiveAlways;
    dItem_data::item_resource[item_data::dItemNo_BOMB_5_e].mArcname = archiveBomb00;
    dItem_data::item_resource[item_data::dItemNo_BOMB_10_e].mArcname = archiveBomb00;
    dItem_data::item_resource[item_data::dItemNo_BOMB_20_e].mArcname = archiveBomb00;
    const char* textureBomb00 = dItem_data::item_texture_tbl[TEXTURE_IDX_BOMB_00].get();
    dItem_data::item_resource[item_data::dItemNo_RED_RUPEE_e].mArcname = archiveVlupy;
    dItem_data::item_resource[item_data::dItemNo_BOMB_5_e].mTexture = textureBomb00;
    dItem_data::item_resource[item_data::dItemNo_BOMB_10_e].mTexture = textureBomb00;
    dItem_data::item_resource[item_data::dItemNo_BOMB_20_e].mTexture = textureBomb00;
    const char* archiveVhapp = dItem_data::item_arcname_tbl[ARCNAME_IDX_VHAPP].get();
    const char* textureGetKey = dItem_data::item_texture_tbl[TEXTURE_IDX_GET_KEY].get();
    dItem_data::item_resource[item_data::dItemNo_JOY_PENDANT_e].mArcname = archiveVhapp;
    dItem_data::item_resource[item_data::dItemNo_TRIPLE_HEART_e].mTexture = textureBottle00;
    const char* textureBow01 = dItem_data::item_texture_tbl[TEXTURE_IDX_BOW_01].get();
    dItem_data::item_resource[item_data::dItemNo_BOMB_30_e].mArcname = archiveBomb00;
    dItem_data::item_resource[item_data::dItemNo_BOMB_30_e].mTexture = textureBomb00;
    dItem_data::item_resource[item_data::dItemNo_SILVER_RUPEE_e].mArcname = archiveVlupy;
    dItem_data::item_resource[item_data::dItemNo_SILVER_RUPEE_e].mTexture = textureGetRupy;
    dItem_data::item_resource[item_data::dItemNo_ARROW_10_e].mArcname = archiveAlways;
    dItem_data::item_resource[item_data::dItemNo_ARROW_10_e].mTexture = textureBow01;
    dItem_data::item_resource[item_data::dItemNo_HEART_PIECE_4_e].mTexture = textureHeartUp02;
    dItem_data::item_resource[item_data::dItemNo_HEART_PIECE_3_e].mTexture = textureHeartUp02;
    dItem_data::item_resource[item_data::dItemNo_ARROW_20_e].mArcname = archiveAlways;
    dItem_data::item_resource[item_data::dItemNo_ARROW_20_e].mTexture = textureBow01;
    dItem_data::item_resource[item_data::dItemNo_ARROW_30_e].mArcname = archiveAlways;
    dItem_data::item_resource[item_data::dItemNo_ARROW_30_e].mTexture = textureBow01;
    const char* archiveTele00 = dItem_data::item_arcname_tbl[ARCNAME_IDX_TELE_00].get();
    const char* textureBeast08 = dItem_data::item_texture_tbl[TEXTURE_IDX_BEAST_08].get();
    dItem_data::item_resource[item_data::dItemNo_TELESCOPE_e].mArcname = archiveTele00;
    dItem_data::item_resource[item_data::dItemNo_JOY_PENDANT_e].mTexture = textureBeast08;
    const char* textureTelescope = dItem_data::item_texture_tbl[TEXTURE_IDX_TELESCOPE].get();
    const char* archiveVtact = dItem_data::item_arcname_tbl[ARCNAME_IDX_VTACT].get();
    const char* archiveMiiBingHd = dItem_data::item_arcname_tbl[ARCNAME_IDX_MII_BING_HD].get();
    dItem_data::item_resource[item_data::dItemNo_WIND_WAKER_e].mArcname = archiveVtact;
    dItem_data::item_resource[item_data::dItemNo_NOENTRY_20_e].mArcname = archiveMiiBingHd;
    const char* textureBaton = dItem_data::item_texture_tbl[TEXTURE_IDX_BATON].get();
    const char* archiveFa = dItem_data::item_arcname_tbl[ARCNAME_IDX_FA].get();
    dItem_data::item_resource[item_data::dItemNo_WIND_WAKER_e].mTexture = textureBaton;
    dItem_data::item_resource[item_data::dItemNo_TINGLE_TUNER_e].mArcname = archiveMiiBingHd;
    dItem_data::item_resource[item_data::dItemNo_HEART_e].mTexture = textureBottle00;
    const char* archiveVkeyn = dItem_data::item_arcname_tbl[ARCNAME_IDX_VKEYN].get();
    dItem_data::item_resource[item_data::dItemNo_TELESCOPE_e].mTexture = textureTelescope;
    dItem_data::item_resource[item_data::dItemNo_NOENTRY_20_e].mTexture = textureBottle00;
    dItem_data::item_resource[item_data::dItemNo_SMALL_KEY_e].mTexture = textureGetKey;
    dItem_data::item_resource[item_data::dItemNo_SMALL_KEY_e].mArcname = archiveVkeyn;
    dItem_data::item_resource[item_data::dItemNo_HEART_PIECE_2_e].mTexture = textureHeartUp02;
    dItem_data::item_resource[item_data::dItemNo_RECOVER_FAIRY_e].mArcname = archiveFa;
    const char* textureCamera = dItem_data::item_texture_tbl[TEXTURE_IDX_CAMERA].get();
    const char* archiveVboxn = dItem_data::item_arcname_tbl[ARCNAME_IDX_VBOXN].get();
    dItem_data::item_resource[item_data::dItemNo_PICTO_BOX_e].mTexture = textureCamera;
    dItem_data::item_resource[item_data::dItemNo_PICTO_BOX_e].mArcname = archiveVboxn;
    const char* archiveVbage = dItem_data::item_arcname_tbl[ARCNAME_IDX_VBAGE].get();
    const char* textureCoverofbeast = dItem_data::item_texture_tbl[TEXTURE_IDX_COVEROFBEAST].get();
    dItem_data::item_resource[item_data::dItemNo_SPOILS_BAG_e].mArcname = archiveVbage;
    dItem_data::item_resource[item_data::dItemNo_SPOILS_BAG_e].mTexture = textureCoverofbeast;
    const char* textureGloves000X35 =
        dItem_data::item_texture_tbl[TEXTURE_IDX_GLOVES_00_0x35].get();
    const char* archiveVrope = dItem_data::item_arcname_tbl[ARCNAME_IDX_VROPE].get();
    dItem_data::item_resource[item_data::dItemNo_POWER_BRACELETS_e].mTexture = textureGloves000X35;
    dItem_data::item_resource[item_data::dItemNo_GRAPPLING_HOOK_e].mArcname = archiveVrope;
    const char* textureRope = dItem_data::item_texture_tbl[TEXTURE_IDX_ROPE].get();
    const char* archiveVboxa = dItem_data::item_arcname_tbl[ARCNAME_IDX_VBOXA].get();
    dItem_data::item_resource[item_data::dItemNo_GRAPPLING_HOOK_e].mTexture = textureRope;
    dItem_data::item_resource[item_data::dItemNo_DELUXE_PICTO_BOX_e].mArcname = archiveVboxa;
    const char* archiveVboot = dItem_data::item_arcname_tbl[ARCNAME_IDX_VBOOT].get();
    const char* textureShield02 = dItem_data::item_texture_tbl[TEXTURE_IDX_SHIELD_02].get();
    dItem_data::item_resource[item_data::dItemNo_IRON_BOOTS_e].mArcname = archiveVboot;
    const char* textureBoots00 = dItem_data::item_texture_tbl[TEXTURE_IDX_BOOTS_00].get();
    const char* archiveVmags = dItem_data::item_arcname_tbl[ARCNAME_IDX_VMAGS].get();
    dItem_data::item_resource[item_data::dItemNo_IRON_BOOTS_e].mTexture = textureBoots00;
    dItem_data::item_resource[item_data::dItemNo_MAGIC_ARMOR_e].mArcname = archiveVmags;
    dItem_data::item_resource[item_data::dItemNo_MAGIC_ARMOR_e].mTexture = textureShield02;
    const char* archiveVboml = dItem_data::item_arcname_tbl[ARCNAME_IDX_VBOML].get();
    const char* textureBoots01 = dItem_data::item_texture_tbl[TEXTURE_IDX_BOOTS_01].get();
    dItem_data::item_resource[item_data::dItemNo_WATER_BOOTS_e].mArcname = archiveVboml;
    dItem_data::item_resource[item_data::dItemNo_TRIPLE_HEART_e].mArcname = archiveAlways;
    dItem_data::item_resource[item_data::dItemNo_GREEN_RUPEE_e].mTexture = textureGetRupy;
    dItem_data::item_resource[item_data::dItemNo_HEART_e].mArcname = archiveAlways;
    dItem_data::item_resource[item_data::dItemNo_SUB_DUN_RUPEE_e].mArcname = archiveVlupy;
    dItem_data::item_resource[item_data::dItemNo_SUB_DUN_RUPEE_e].mTexture = textureGetRupy;
    const char* archiveVbagf = dItem_data::item_arcname_tbl[ARCNAME_IDX_VBAGF].get();
    dItem_data::item_resource[item_data::dItemNo_WATER_BOOTS_e].mTexture = textureBoots01;
    dItem_data::item_resource[item_data::dItemNo_BAIT_BAG_e].mArcname = archiveVbagf;
    const char* textureCoverofbait = dItem_data::item_texture_tbl[TEXTURE_IDX_COVEROFBAIT].get();
    const char* archiveBoom = dItem_data::item_arcname_tbl[ARCNAME_IDX_BOOM].get();
    dItem_data::item_resource[item_data::dItemNo_BAIT_BAG_e].mTexture = textureCoverofbait;
    dItem_data::item_resource[item_data::dItemNo_BOOMERANG_e].mArcname = archiveBoom;
    dItem_data::item_resource[item_data::dItemNo_TINGLE_TUNER_e].mTexture = textureBottle00;
    const char* textureBoomerang = dItem_data::item_texture_tbl[TEXTURE_IDX_BOOMERANG].get();
    const char* archiveVfuku = dItem_data::item_arcname_tbl[ARCNAME_IDX_VFUKU].get();
    dItem_data::item_resource[item_data::dItemNo_BOOMERANG_e].mTexture = textureBoomerang;
    dItem_data::item_resource[item_data::dItemNo_BARE_HAND_e].mArcname = archiveVfuku;
    dItem_data::item_resource[item_data::dItemNo_BOW_e].mTexture = textureBow01;
    const char* textureCamera2 = dItem_data::item_texture_tbl[TEXTURE_IDX_CAMERA_2].get();
    const char* archiveVbow = dItem_data::item_arcname_tbl[ARCNAME_IDX_VBOW].get();
    dItem_data::item_resource[item_data::dItemNo_DELUXE_PICTO_BOX_e].mTexture = textureCamera2;
    dItem_data::item_resource[item_data::dItemNo_BOW_e].mArcname = archiveVbow;
    const char* archiveVpowr = dItem_data::item_arcname_tbl[ARCNAME_IDX_VPOWR].get();
    const char* textureGloves000X34 =
        dItem_data::item_texture_tbl[TEXTURE_IDX_GLOVES_00_0x34].get();
    dItem_data::item_resource[item_data::dItemNo_POWER_BRACELETS_e].mArcname = archiveVpowr;
    dItem_data::item_resource[item_data::dItemNo_BARE_HAND_e].mTexture = textureGloves000X34;
    const char* archiveVhook = dItem_data::item_arcname_tbl[ARCNAME_IDX_VHOOK].get();
    dItem_data::item_resource[item_data::dItemNo_HOOKSHOT_e].mArcname = archiveVhook;
    const char* archiveVhamm = dItem_data::item_arcname_tbl[ARCNAME_IDX_VHAMM].get();
    const char* textureHookshot = dItem_data::item_texture_tbl[TEXTURE_IDX_HOOKSHOT].get();
    dItem_data::item_resource[item_data::dItemNo_SKULL_HAMMER_e].mArcname = archiveVhamm;
    dItem_data::item_resource[item_data::dItemNo_HOOKSHOT_e].mTexture = textureHookshot;
    const char* textureClothes = dItem_data::item_texture_tbl[TEXTURE_IDX_CLOTHES].get();
    const char* archiveVbagh = dItem_data::item_arcname_tbl[ARCNAME_IDX_VBAGH].get();
    const char* textureDelivery = dItem_data::item_texture_tbl[TEXTURE_IDX_DELIVERY].get();
    dItem_data::item_resource[item_data::dItemNo_DELIVERY_BAG_e].mArcname = archiveVbagh;
    dItem_data::item_resource[item_data::dItemNo_DELIVERY_BAG_e].mTexture = textureDelivery;
    const char* textureFan = dItem_data::item_texture_tbl[TEXTURE_IDX_FAN].get();
    const char* textureHammer01 = dItem_data::item_texture_tbl[TEXTURE_IDX_HAMMER_01].get();
    const char* archiveVleaf = dItem_data::item_arcname_tbl[ARCNAME_IDX_VLEAF].get();
    dItem_data::item_resource[item_data::dItemNo_SKULL_HAMMER_e].mTexture = textureHammer01;
    dItem_data::item_resource[item_data::dItemNo_DEKU_LEAF_e].mArcname = archiveVleaf;
    dItem_data::item_resource[item_data::dItemNo_DEKU_LEAF_e].mTexture = textureFan;
    const char* archiveVarfi = dItem_data::item_arcname_tbl[ARCNAME_IDX_VARFI].get();
    const char* textureArrowPower01 =
        dItem_data::item_texture_tbl[TEXTURE_IDX_ARROW_POWER_01].get();
    dItem_data::item_resource[item_data::dItemNo_FUKU_e].mArcname = archiveVfuku;
    const char* archiveVshin = dItem_data::item_arcname_tbl[ARCNAME_IDX_VSHIN].get();
    const char* archiveVswon = dItem_data::item_arcname_tbl[ARCNAME_IDX_VSWON].get();
    dItem_data::item_resource[item_data::dItemNo_NEW_FUKU_e].mTexture = textureClothes;
    dItem_data::item_resource[item_data::dItemNo_SWORD_e].mArcname = archiveVswon;
    const char* textureSword00 = dItem_data::item_texture_tbl[TEXTURE_IDX_SWORD_00].get();
    const char* archiveVarrl = dItem_data::item_arcname_tbl[ARCNAME_IDX_VARRL].get();
    dItem_data::item_resource[item_data::dItemNo_SWORD_e].mTexture = textureSword00;
    const char* textureSword01 = dItem_data::item_texture_tbl[TEXTURE_IDX_SWORD_01].get();
    const char* textureSword02 = dItem_data::item_texture_tbl[TEXTURE_IDX_SWORD_02].get();
    dItem_data::item_resource[item_data::dItemNo_MASTER_SWORD_1_e].mTexture = textureSword01;
    dItem_data::item_resource[item_data::dItemNo_MASTER_SWORD_2_e].mTexture = textureSword02;
    dItem_data::item_resource[item_data::dItemNo_BOMB_BAG_e].mTexture = textureBomb00;
    dItem_data::item_resource[item_data::dItemNo_FUKU_e].mTexture = textureClothes;
    dItem_data::item_resource[item_data::dItemNo_BOMB_BAG_e].mArcname = archiveBomb00;
    const char* textureShield00 = dItem_data::item_texture_tbl[TEXTURE_IDX_SHIELD_00].get();
    dItem_data::item_resource[item_data::dItemNo_SHIELD_e].mArcname = archiveVshin;
    dItem_data::item_resource[item_data::dItemNo_SHIELD_e].mTexture = textureShield00;
    const char* archiveVshim = dItem_data::item_arcname_tbl[ARCNAME_IDX_VSHIM].get();
    const char* textureShield01 = dItem_data::item_texture_tbl[TEXTURE_IDX_SHIELD_01].get();
    dItem_data::item_resource[item_data::dItemNo_MIRROR_SHIELD_e].mArcname = archiveVshim;
    dItem_data::item_resource[item_data::dItemNo_MIRROR_SHIELD_e].mTexture = textureShield01;
    dItem_data::item_resource[item_data::dItemNo_DROPPED_SWORD_e].mArcname = archiveVswon;
    dItem_data::item_resource[item_data::dItemNo_DROPPED_SWORD_e].mTexture = textureSword00;
    const char* textureSword03 = dItem_data::item_texture_tbl[TEXTURE_IDX_SWORD_03].get();
    const char* textureArrowPower02 =
        dItem_data::item_texture_tbl[TEXTURE_IDX_ARROW_POWER_02].get();
    dItem_data::item_resource[item_data::dItemNo_MAGIC_ARROW_e].mArcname = archiveVarfi;
    dItem_data::item_resource[item_data::dItemNo_MAGIC_ARROW_e].mTexture = textureArrowPower01;
    dItem_data::item_resource[item_data::dItemNo_LIGHT_ARROW_e].mArcname = archiveVarrl;
    dItem_data::item_resource[item_data::dItemNo_LIGHT_ARROW_e].mTexture = textureArrowPower02;
    dItem_data::item_resource[item_data::dItemNo_MASTER_SWORD_3_e].mTexture = textureSword03;
    dItem_data::item_resource[item_data::dItemNo_HEART_PIECE_ALT_e].mArcname = archiveVhkak;
    dItem_data::item_resource[item_data::dItemNo_HEART_PIECE_ALT_e].mTexture = textureHeartUp02;
    const char* archiveVamuk = dItem_data::item_arcname_tbl[ARCNAME_IDX_VAMUK].get();
    dItem_data::item_resource[item_data::dItemNo_PIRATES_CHARM_e].mArcname = archiveVamuk;
    const char* textureAmulet00 = dItem_data::item_texture_tbl[TEXTURE_IDX_AMULET_00].get();
    const char* archiveVdoku = dItem_data::item_arcname_tbl[ARCNAME_IDX_VDOKU].get();
    dItem_data::item_resource[item_data::dItemNo_GRASS_BALL_e].mTexture = textureBottle00;
    dItem_data::item_resource[item_data::dItemNo_SKULL_NECKLACE_e].mArcname = archiveVdoku;
    const char* textureBeast01 = dItem_data::item_texture_tbl[TEXTURE_IDX_BEAST_01].get();
    const char* archiveVboko = dItem_data::item_arcname_tbl[ARCNAME_IDX_VBOKO].get();
    dItem_data::item_resource[item_data::dItemNo_SKULL_NECKLACE_e].mTexture = textureBeast01;
    const char* archiveVbelt = dItem_data::item_arcname_tbl[ARCNAME_IDX_VBELT].get();
    dItem_data::item_resource[item_data::dItemNo_BOKOBABA_SEED_e].mArcname = archiveVboko;
    const char* textureBeast02 = dItem_data::item_texture_tbl[TEXTURE_IDX_BEAST_02].get();
    const char* archiveVhane = dItem_data::item_arcname_tbl[ARCNAME_IDX_VHANE].get();
    dItem_data::item_resource[item_data::dItemNo_BOKOBABA_SEED_e].mTexture = textureBeast02;
    dItem_data::item_resource[item_data::dItemNo_GOLDEN_FEATHER_e].mArcname = archiveVhane;
    dItem_data::item_resource[item_data::dItemNo_KNIGHTS_CREST_e].mArcname = archiveVbelt;
    const char* archiveVchuj = dItem_data::item_arcname_tbl[ARCNAME_IDX_VCHUJ].get();
    dItem_data::item_resource[item_data::dItemNo_PIRATES_CHARM_e].mTexture = textureAmulet00;
    dItem_data::item_resource[item_data::dItemNo_RED_JELLY_e].mArcname = archiveVchuj;
    dItem_data::item_resource[item_data::dItemNo_GREEN_JELLY_e].mArcname = archiveVchuj;
    dItem_data::item_resource[item_data::dItemNo_BLUE_JELLY_e].mArcname = archiveVchuj;
    const char* textureBeast03 = dItem_data::item_texture_tbl[TEXTURE_IDX_BEAST_03].get();
    const char* textureBeast04 = dItem_data::item_texture_tbl[TEXTURE_IDX_BEAST_04].get();
    dItem_data::item_resource[item_data::dItemNo_GOLDEN_FEATHER_e].mTexture = textureBeast03;
    dItem_data::item_resource[item_data::dItemNo_KNIGHTS_CREST_e].mTexture = textureBeast04;
    const char* textureBeast05 = dItem_data::item_texture_tbl[TEXTURE_IDX_BEAST_05].get();
    const char* textureDungeonMap = dItem_data::item_texture_tbl[TEXTURE_IDX_DUNGEON_MAP].get();
    const char* textureBeast06 = dItem_data::item_texture_tbl[TEXTURE_IDX_BEAST_06].get();
    dItem_data::item_resource[item_data::dItemNo_RED_JELLY_e].mTexture = textureBeast05;
    dItem_data::item_resource[item_data::dItemNo_GREEN_JELLY_e].mTexture = textureBeast06;
    const char* textureBeast07 = dItem_data::item_texture_tbl[TEXTURE_IDX_BEAST_07].get();
    const char* archiveVdunm = dItem_data::item_arcname_tbl[ARCNAME_IDX_VDUNM].get();
    dItem_data::item_resource[item_data::dItemNo_BLUE_JELLY_e].mTexture = textureBeast07;
    dItem_data::item_resource[item_data::dItemNo_MAP_e].mArcname = archiveVdunm;
    const char* archiveVdunc = dItem_data::item_arcname_tbl[ARCNAME_IDX_VDUNC].get();
    dItem_data::item_resource[item_data::dItemNo_MAP_e].mTexture = textureDungeonMap;
    dItem_data::item_resource[item_data::dItemNo_COMPASS_e].mArcname = archiveVdunc;
    const char* archiveVbins = dItem_data::item_arcname_tbl[ARCNAME_IDX_VBINS].get();
    const char* textureAmulet01 = dItem_data::item_texture_tbl[TEXTURE_IDX_AMULET_01].get();
    const char* archiveVamuy = dItem_data::item_arcname_tbl[ARCNAME_IDX_VAMUY].get();
    dItem_data::item_resource[item_data::dItemNo_HEROS_CHARM_e].mTexture = textureAmulet01;
    dItem_data::item_resource[item_data::dItemNo_HEROS_CHARM_e].mArcname = archiveVamuy;
    const char* textureCompass = dItem_data::item_texture_tbl[TEXTURE_IDX_COMPASS].get();
    dItem_data::item_resource[item_data::dItemNo_COMPASS_e].mTexture = textureCompass;
    const char* archiveKey01 = dItem_data::item_arcname_tbl[ARCNAME_IDX_KEY_01].get();
    const char* textureBottle01 = dItem_data::item_texture_tbl[TEXTURE_IDX_BOTTLE_01].get();
    dItem_data::item_resource[item_data::dItemNo_BOSS_KEY_e].mArcname = archiveKey01;
    const char* archiveMedB = dItem_data::item_arcname_tbl[ARCNAME_IDX_MED_B].get();
    const char* archiveMedR = dItem_data::item_arcname_tbl[ARCNAME_IDX_MED_R].get();
    dItem_data::item_resource[item_data::dItemNo_EMPTY_BOTTLE_e].mTexture = textureBottle00;
    dItem_data::item_resource[item_data::dItemNo_RED_POTION_e].mArcname = archiveMedR;
    const char* archiveMedG = dItem_data::item_arcname_tbl[ARCNAME_IDX_MED_G].get();
    dItem_data::item_resource[item_data::dItemNo_RED_POTION_e].mTexture = textureBottle01;
    dItem_data::item_resource[item_data::dItemNo_GREEN_POTION_e].mArcname = archiveMedG;
    dItem_data::item_resource[item_data::dItemNo_BLUE_POTION_e].mArcname = archiveMedB;
    dItem_data::item_resource[item_data::dItemNo_HALF_SOUP_BOTTLE_e].mArcname = archiveVbins;
    dItem_data::item_resource[item_data::dItemNo_SOUP_BOTTLE_e].mArcname = archiveVbins;
    const char* textureBottle09 = dItem_data::item_texture_tbl[TEXTURE_IDX_BOTTLE_09].get();
    const char* textureBottle04 = dItem_data::item_texture_tbl[TEXTURE_IDX_BOTTLE_04].get();
    const char* textureBottle05 = dItem_data::item_texture_tbl[TEXTURE_IDX_BOTTLE_05].get();
    dItem_data::item_resource[item_data::dItemNo_SOUP_BOTTLE_e].mTexture = textureBottle04;
    const char* textureBottle02 = dItem_data::item_texture_tbl[TEXTURE_IDX_BOTTLE_02].get();
    dItem_data::item_resource[item_data::dItemNo_WATER_BOTTLE_e].mTexture = textureBottle05;
    dItem_data::item_resource[item_data::dItemNo_GREEN_POTION_e].mTexture = textureBottle02;
    const char* textureBottle03 = dItem_data::item_texture_tbl[TEXTURE_IDX_BOTTLE_03].get();
    const char* archiveVbiny = dItem_data::item_arcname_tbl[ARCNAME_IDX_VBINY].get();
    dItem_data::item_resource[item_data::dItemNo_BLUE_POTION_e].mTexture = textureBottle03;
    dItem_data::item_resource[item_data::dItemNo_FAIRY_BOTTLE_e].mArcname = archiveVbiny;
    const char* archiveVbinw = dItem_data::item_arcname_tbl[ARCNAME_IDX_VBINW].get();
    const char* textureBottle06 = dItem_data::item_texture_tbl[TEXTURE_IDX_BOTTLE_06].get();
    dItem_data::item_resource[item_data::dItemNo_WATER_BOTTLE_e].mArcname = archiveVbinw;
    dItem_data::item_resource[item_data::dItemNo_FAIRY_BOTTLE_e].mTexture = textureBottle06;
    const char* archiveVbinh = dItem_data::item_arcname_tbl[ARCNAME_IDX_VBINH].get();
    const char* textureBottle07 = dItem_data::item_texture_tbl[TEXTURE_IDX_BOTTLE_07].get();
    dItem_data::item_resource[item_data::dItemNo_FIREFLY_BOTTLE_e].mArcname = archiveVbinh;
    dItem_data::item_resource[item_data::dItemNo_FIREFLY_BOTTLE_e].mTexture = textureBottle07;
    const char* archiveFwbin = dItem_data::item_arcname_tbl[ARCNAME_IDX_FWBIN].get();
    const char* textureBottle08 = dItem_data::item_texture_tbl[TEXTURE_IDX_BOTTLE_08].get();
    dItem_data::item_resource[item_data::dItemNo_HALF_SOUP_BOTTLE_e].mTexture = textureBottle09;
    const char* archiveVtri1 = dItem_data::item_arcname_tbl[ARCNAME_IDX_VTRI1].get();
    const char* textureBossKey = dItem_data::item_texture_tbl[TEXTURE_IDX_BOSS_KEY].get();
    dItem_data::item_resource[item_data::dItemNo_TRIFORCE1_e].mArcname = archiveVtri1;
    dItem_data::item_resource[item_data::dItemNo_EMPTY_BSHIP_e].mTexture = textureBottle00;
    dItem_data::item_resource[item_data::dItemNo_BOSS_KEY_e].mTexture = textureBossKey;
    const char* textureTriforce00 = dItem_data::item_texture_tbl[TEXTURE_IDX_TRIFORCE_00].get();
    const char* archiveVtri2 = dItem_data::item_arcname_tbl[ARCNAME_IDX_VTRI2].get();
    dItem_data::item_resource[item_data::dItemNo_TRIFORCE1_e].mTexture = textureTriforce00;
    const char* textureTriforce03 = dItem_data::item_texture_tbl[TEXTURE_IDX_TRIFORCE_03].get();
    dItem_data::item_resource[item_data::dItemNo_TRIFORCE2_e].mArcname = archiveVtri2;
    dItem_data::item_resource[item_data::dItemNo_TRIFORCE4_e].mTexture = textureTriforce03;
    const char* textureGodSymbol02 = dItem_data::item_texture_tbl[TEXTURE_IDX_GOD_SYMBOL_02].get();
    const char* archiveVtri6 = dItem_data::item_arcname_tbl[ARCNAME_IDX_VTRI6].get();
    const char* archiveVtri7 = dItem_data::item_arcname_tbl[ARCNAME_IDX_VTRI7].get();
    dItem_data::item_resource[item_data::dItemNo_TRIFORCE6_e].mArcname = archiveVtri6;
    dItem_data::item_resource[item_data::dItemNo_TRIFORCE7_e].mArcname = archiveVtri7;
    dItem_data::item_resource[item_data::dItemNo_FOREST_WATER_e].mArcname = archiveFwbin;
    const char* archiveVtri8 = dItem_data::item_arcname_tbl[ARCNAME_IDX_VTRI8].get();
    const char* archiveShinjuN = dItem_data::item_arcname_tbl[ARCNAME_IDX_SHINJU_N].get();
    dItem_data::item_resource[item_data::dItemNo_TRIFORCE8_e].mArcname = archiveVtri8;
    dItem_data::item_resource[item_data::dItemNo_PEARL_NAYRU_e].mArcname = archiveShinjuN;
    const char* textureTriforce04 = dItem_data::item_texture_tbl[TEXTURE_IDX_TRIFORCE_04].get();
    const char* textureTriforce05 = dItem_data::item_texture_tbl[TEXTURE_IDX_TRIFORCE_05].get();
    dItem_data::item_resource[item_data::dItemNo_TRIFORCE5_e].mTexture = textureTriforce04;
    dItem_data::item_resource[item_data::dItemNo_TRIFORCE6_e].mTexture = textureTriforce05;
    const char* textureTriforce06 = dItem_data::item_texture_tbl[TEXTURE_IDX_TRIFORCE_06].get();
    const char* archiveBin = dItem_data::item_arcname_tbl[ARCNAME_IDX_BIN].get();
    dItem_data::item_resource[item_data::dItemNo_TRIFORCE7_e].mTexture = textureTriforce06;
    dItem_data::item_resource[item_data::dItemNo_EMPTY_BOTTLE_e].mArcname = archiveBin;
    const char* textureTriforce01 = dItem_data::item_texture_tbl[TEXTURE_IDX_TRIFORCE_01].get();
    dItem_data::item_resource[item_data::dItemNo_FOREST_WATER_e].mTexture = textureBottle08;
    const char* textureTriforce07 = dItem_data::item_texture_tbl[TEXTURE_IDX_TRIFORCE_07].get();
    const char* archiveShinjuF = dItem_data::item_arcname_tbl[ARCNAME_IDX_SHINJU_F].get();
    dItem_data::item_resource[item_data::dItemNo_PEARL_NAYRU_e].mTexture = textureGodSymbol02;
    dItem_data::item_resource[item_data::dItemNo_TRIFORCE8_e].mTexture = textureTriforce07;
    const char* textureTriforce02 = dItem_data::item_texture_tbl[TEXTURE_IDX_TRIFORCE_02].get();
    dItem_data::item_resource[item_data::dItemNo_TRIFORCE2_e].mTexture = textureTriforce01;
    dItem_data::item_resource[item_data::dItemNo_TRIFORCE3_e].mTexture = textureTriforce02;
    const char* archiveShinjuD = dItem_data::item_arcname_tbl[ARCNAME_IDX_SHINJU_D].get();
    const char* textureGodSymbol00 = dItem_data::item_texture_tbl[TEXTURE_IDX_GOD_SYMBOL_00].get();
    dItem_data::item_resource[item_data::dItemNo_PEARL_DIN_e].mArcname = archiveShinjuD;
    dItem_data::item_resource[item_data::dItemNo_PEARL_DIN_e].mTexture = textureGodSymbol00;
    const char* archiveVho = dItem_data::item_arcname_tbl[ARCNAME_IDX_VHO].get();
    dItem_data::item_resource[item_data::dItemNo_PEARL_FARORE_e].mArcname = archiveShinjuF;
    dItem_data::item_resource[item_data::dItemNo_NOENTRY_119_e].mArcname = archiveVho;
    dItem_data::item_resource[item_data::dItemNo_SAIL_e].mArcname = archiveVho;
    const char* archiveVtri3 = dItem_data::item_arcname_tbl[ARCNAME_IDX_VTRI3].get();
    const char* textureGodSymbol01 = dItem_data::item_texture_tbl[TEXTURE_IDX_GOD_SYMBOL_01].get();
    dItem_data::item_resource[item_data::dItemNo_TRIFORCE3_e].mArcname = archiveVtri3;
    dItem_data::item_resource[item_data::dItemNo_SONG_OF_PASSING_e].mTexture = textureBaton;
    dItem_data::item_resource[item_data::dItemNo_SONG_OF_PASSING_e].mArcname = archiveVamuk;
    dItem_data::item_resource[item_data::dItemNo_WIND_GODS_ARIA_e].mTexture = textureBaton;
    dItem_data::item_resource[item_data::dItemNo_WIND_GODS_ARIA_e].mArcname = archiveVamuk;
    dItem_data::item_resource[item_data::dItemNo_EARTH_GODS_LYRIC_e].mTexture = textureBaton;
    dItem_data::item_resource[item_data::dItemNo_EARTH_GODS_LYRIC_e].mArcname = archiveVamuk;
    dItem_data::item_resource[item_data::dItemNo_COMMAND_MELODY_e].mTexture = textureBaton;
    dItem_data::item_resource[item_data::dItemNo_COMMAND_MELODY_e].mArcname = archiveVamuk;
    dItem_data::item_resource[item_data::dItemNo_BALLAD_OF_GALES_e].mTexture = textureBaton;
    dItem_data::item_resource[item_data::dItemNo_BALLAD_OF_GALES_e].mArcname = archiveVamuk;
    dItem_data::item_resource[item_data::dItemNo_WINDS_REQUIEM_e].mTexture = textureBaton;
    dItem_data::item_resource[item_data::dItemNo_WINDS_REQUIEM_e].mArcname = archiveVamuk;
    dItem_data::item_resource[item_data::dItemNo_KNOWLEDGE_TF_e].mTexture = textureAmulet00;
    const char* textureSail00 = dItem_data::item_texture_tbl[TEXTURE_IDX_SAIL_00].get();
    const char* archiveVmapt = dItem_data::item_arcname_tbl[ARCNAME_IDX_VMAPT].get();
    dItem_data::item_resource[item_data::dItemNo_SAIL_e].mTexture = textureSail00;
    dItem_data::item_resource[item_data::dItemNo_TRIFORCE_MAP_1_e].mArcname = archiveVmapt;
    dItem_data::item_resource[item_data::dItemNo_TRIFORCE_MAP_2_e].mArcname = archiveVmapt;
    dItem_data::item_resource[item_data::dItemNo_TRIFORCE_MAP_3_e].mArcname = archiveVmapt;
    dItem_data::item_resource[item_data::dItemNo_TRIFORCE_MAP_4_e].mArcname = archiveVmapt;
    dItem_data::item_resource[item_data::dItemNo_TRIFORCE_MAP_5_e].mArcname = archiveVmapt;
    const char* archiveVtri4 = dItem_data::item_arcname_tbl[ARCNAME_IDX_VTRI4].get();
    dItem_data::item_resource[item_data::dItemNo_KNOWLEDGE_TF_e].mArcname = archiveVamuk;
    dItem_data::item_resource[item_data::dItemNo_TRIFORCE4_e].mArcname = archiveVtri4;
    const char* archiveVtri5 = dItem_data::item_arcname_tbl[ARCNAME_IDX_VTRI5].get();
    const char* textureCmapTri2 = dItem_data::item_texture_tbl[TEXTURE_IDX_CMAP_TRI2].get();
    dItem_data::item_resource[item_data::dItemNo_TRIFORCE5_e].mArcname = archiveVtri5;
    dItem_data::item_resource[item_data::dItemNo_TRIFORCE_MAP_1_e].mTexture = textureCmapTri2;
    dItem_data::item_resource[item_data::dItemNo_TRIFORCE_MAP_2_e].mTexture = textureCmapTri2;
    const char* archiveVesa = dItem_data::item_arcname_tbl[ARCNAME_IDX_VESA].get();
    const char* archiveVobj01 = dItem_data::item_arcname_tbl[ARCNAME_IDX_VOBJ01].get();
    const char* textureDelivery02 = dItem_data::item_texture_tbl[TEXTURE_IDX_DELIVERY_02].get();
    dItem_data::item_resource[item_data::dItemNo_SEA_FLOWER_e].mArcname = archiveVobj01;
    dItem_data::item_resource[item_data::dItemNo_SEA_FLOWER_e].mTexture = textureDelivery02;
    const char* archiveVhyoi = dItem_data::item_arcname_tbl[ARCNAME_IDX_VHYOI].get();
    const char* archiveVobj02 = dItem_data::item_arcname_tbl[ARCNAME_IDX_VOBJ02].get();
    const char* textureDelivery03 = dItem_data::item_texture_tbl[TEXTURE_IDX_DELIVERY_03].get();
    dItem_data::item_resource[item_data::dItemNo_EXOTIC_FLOWER_e].mArcname = archiveVobj02;
    dItem_data::item_resource[item_data::dItemNo_EXOTIC_FLOWER_e].mTexture = textureDelivery03;
    const char* textureBait01 = dItem_data::item_texture_tbl[TEXTURE_IDX_BAIT_01].get();
    dItem_data::item_resource[item_data::dItemNo_BIRD_BAIT_5_e].mArcname = archiveVesa;
    dItem_data::item_resource[item_data::dItemNo_BIRD_BAIT_5_e].mTexture = textureBait01;
    dItem_data::item_resource[item_data::dItemNo_NOENTRY_119_e].mTexture = textureSail00;
    const char* textureBait02 = dItem_data::item_texture_tbl[TEXTURE_IDX_BAIT_02].get();
    dItem_data::item_resource[item_data::dItemNo_HYOI_PEAR_e].mArcname = archiveVhyoi;
    dItem_data::item_resource[item_data::dItemNo_HYOI_PEAR_e].mTexture = textureBait02;
    dItem_data::item_resource[item_data::dItemNo_PEARL_FARORE_e].mTexture = textureGodSymbol01;
    const char* archiveVobj03 = dItem_data::item_arcname_tbl[ARCNAME_IDX_VOBJ03].get();
    const char* textureDelivery04 = dItem_data::item_texture_tbl[TEXTURE_IDX_DELIVERY_04].get();
    dItem_data::item_resource[item_data::dItemNo_HEROS_FLAG_e].mArcname = archiveVobj03;
    dItem_data::item_resource[item_data::dItemNo_HEROS_FLAG_e].mTexture = textureDelivery04;
    const char* archiveVobj04 = dItem_data::item_arcname_tbl[ARCNAME_IDX_VOBJ04].get();
    dItem_data::item_resource[item_data::dItemNo_TRIFORCE_MAP_3_e].mTexture = textureCmapTri2;
    dItem_data::item_resource[item_data::dItemNo_TRIFORCE_MAP_4_e].mTexture = textureCmapTri2;
    dItem_data::item_resource[item_data::dItemNo_TRIFORCE_MAP_5_e].mTexture = textureCmapTri2;
    dItem_data::item_resource[item_data::dItemNo_TRIFORCE_MAP_6_e].mArcname = archiveVmapt;
    dItem_data::item_resource[item_data::dItemNo_TRIFORCE_MAP_6_e].mTexture = textureCmapTri2;
    dItem_data::item_resource[item_data::dItemNo_TRIFORCE_MAP_7_e].mArcname = archiveVmapt;
    dItem_data::item_resource[item_data::dItemNo_TRIFORCE_MAP_7_e].mTexture = textureCmapTri2;
    dItem_data::item_resource[item_data::dItemNo_TRIFORCE_MAP_8_e].mArcname = archiveVmapt;
    dItem_data::item_resource[item_data::dItemNo_TRIFORCE_MAP_8_e].mTexture = textureCmapTri2;
    const char* textureDelivery05 = dItem_data::item_texture_tbl[TEXTURE_IDX_DELIVERY_05].get();
    dItem_data::item_resource[item_data::dItemNo_BIG_CATCH_FLAG_e].mArcname = archiveVobj04;
    dItem_data::item_resource[item_data::dItemNo_BIG_CATCH_FLAG_e].mTexture = textureDelivery05;
    const char* archiveVobj05 = dItem_data::item_arcname_tbl[ARCNAME_IDX_VOBJ05].get();
    const char* textureDelivery06 = dItem_data::item_texture_tbl[TEXTURE_IDX_DELIVERY_06].get();
    dItem_data::item_resource[item_data::dItemNo_BIG_SALE_FLAG_e].mArcname = archiveVobj05;
    dItem_data::item_resource[item_data::dItemNo_BIG_SALE_FLAG_e].mTexture = textureDelivery06;
    const char* archiveVobj06 = dItem_data::item_arcname_tbl[ARCNAME_IDX_VOBJ06].get();
    const char* textureDelivery01 = dItem_data::item_texture_tbl[TEXTURE_IDX_DELIVERY_01].get();
    dItem_data::item_resource[item_data::dItemNo_PINWHEEL_e].mArcname = archiveVobj06;
    dItem_data::item_resource[item_data::dItemNo_TOWN_FLOWER_e].mTexture = textureDelivery01;
    const char* textureDelivery07 = dItem_data::item_texture_tbl[TEXTURE_IDX_DELIVERY_07].get();
    const char* textureDelivery08 = dItem_data::item_texture_tbl[TEXTURE_IDX_DELIVERY_08].get();
    dItem_data::item_resource[item_data::dItemNo_PINWHEEL_e].mTexture = textureDelivery07;
    const char* archiveVobj07 = dItem_data::item_arcname_tbl[ARCNAME_IDX_VOBJ07].get();
    dItem_data::item_resource[item_data::dItemNo_SICKLE_MOON_FLAG_e].mTexture = textureDelivery08;
    dItem_data::item_resource[item_data::dItemNo_SICKLE_MOON_FLAG_e].mArcname = archiveVobj07;
    const char* archiveVobj09 = dItem_data::item_arcname_tbl[ARCNAME_IDX_VOBJ09].get();
    const char* archiveVobj08 = dItem_data::item_arcname_tbl[ARCNAME_IDX_VOBJ08].get();
    const char* textureDelivery09 = dItem_data::item_texture_tbl[TEXTURE_IDX_DELIVERY_09].get();
    dItem_data::item_resource[item_data::dItemNo_SKULL_TOWER_IDOL_e].mArcname = archiveVobj08;
    dItem_data::item_resource[item_data::dItemNo_SKULL_TOWER_IDOL_e].mTexture = textureDelivery09;
    dItem_data::item_resource[item_data::dItemNo_FOUNTAIN_IDOL_e].mArcname = archiveVobj09;
    const char* archiveVobj00 = dItem_data::item_arcname_tbl[ARCNAME_IDX_VOBJ00].get();
    const char* textureDelivery10 = dItem_data::item_texture_tbl[TEXTURE_IDX_DELIVERY_10].get();
    dItem_data::item_resource[item_data::dItemNo_TOWN_FLOWER_e].mArcname = archiveVobj00;
    dItem_data::item_resource[item_data::dItemNo_FOUNTAIN_IDOL_e].mTexture = textureDelivery10;
    const char* archiveVobj10 = dItem_data::item_arcname_tbl[ARCNAME_IDX_VOBJ10].get();
    const char* textureDelivery11 = dItem_data::item_texture_tbl[TEXTURE_IDX_DELIVERY_11].get();
    dItem_data::item_resource[item_data::dItemNo_POSTMAN_STATUE_e].mArcname = archiveVobj10;
    dItem_data::item_resource[item_data::dItemNo_POSTMAN_STATUE_e].mTexture = textureDelivery11;
    const char* textureDelivery14 = dItem_data::item_texture_tbl[TEXTURE_IDX_DELIVERY_14].get();
    const char* textureDelivery15 = dItem_data::item_texture_tbl[TEXTURE_IDX_DELIVERY_15].get();
    dItem_data::item_resource[item_data::dItemNo_NOTE_TO_MOM_e].mTexture = textureDelivery14;
    dItem_data::item_resource[item_data::dItemNo_MAGGIES_LETTER_e].mTexture = textureDelivery15;
    const char* textureDelivery16 = dItem_data::item_texture_tbl[TEXTURE_IDX_DELIVERY_16].get();
    const char* textureDelivery13 = dItem_data::item_texture_tbl[TEXTURE_IDX_DELIVERY_13].get();
    dItem_data::item_resource[item_data::dItemNo_MOBLINS_LETTER_e].mTexture = textureDelivery16;
    dItem_data::item_resource[item_data::dItemNo_FATHER_LETTER_e].mTexture = textureDelivery13;
    const char* archiveVleof = dItem_data::item_arcname_tbl[ARCNAME_IDX_VLEOF].get();
    const char* archiveVlemg = dItem_data::item_arcname_tbl[ARCNAME_IDX_VLEMG].get();
    dItem_data::item_resource[item_data::dItemNo_NOTE_TO_MOM_e].mArcname = archiveVleof;
    dItem_data::item_resource[item_data::dItemNo_MAGGIES_LETTER_e].mArcname = archiveVlemg;
    const char* textureDelivery17 = dItem_data::item_texture_tbl[TEXTURE_IDX_DELIVERY_17].get();
    const char* archiveVlemo = dItem_data::item_arcname_tbl[ARCNAME_IDX_VLEMO].get();
    dItem_data::item_resource[item_data::dItemNo_CABANA_DEED_e].mTexture = textureDelivery17;
    dItem_data::item_resource[item_data::dItemNo_MOBLINS_LETTER_e].mArcname = archiveVlemo;
    const char* textureDelivery18 = dItem_data::item_texture_tbl[TEXTURE_IDX_DELIVERY_18].get();
    const char* textureTingleFigure = dItem_data::item_texture_tbl[TEXTURE_IDX_TINGLE_FIGURE].get();
    dItem_data::item_resource[item_data::dItemNo_COMPLIMENTARY_ID_e].mTexture = textureDelivery18;
    const char* archiveVmant = dItem_data::item_arcname_tbl[ARCNAME_IDX_VMANT].get();
    const char* archiveVbeso = dItem_data::item_arcname_tbl[ARCNAME_IDX_VBESO].get();
    const char* archiveVtin5 = dItem_data::item_arcname_tbl[ARCNAME_IDX_VTIN5].get();
    dItem_data::item_resource[item_data::dItemNo_FILL_UP_COUPON_e].mArcname = archiveVmant;
    dItem_data::item_resource[item_data::dItemNo_TINGLE_STATUE_5_e].mTexture = textureTingleFigure;
    const char* textureDelivery12 = dItem_data::item_texture_tbl[TEXTURE_IDX_DELIVERY_12].get();
    dItem_data::item_resource[item_data::dItemNo_CABANA_DEED_e].mArcname = archiveVbeso;
    const char* archiveVohom = dItem_data::item_arcname_tbl[ARCNAME_IDX_VOHOM].get();
    const char* textureDelivery19 = dItem_data::item_texture_tbl[TEXTURE_IDX_DELIVERY_19].get();
    dItem_data::item_resource[item_data::dItemNo_COMPLIMENTARY_ID_e].mArcname = archiveVohom;
    dItem_data::item_resource[item_data::dItemNo_FILL_UP_COUPON_e].mTexture = textureDelivery19;
    dItem_data::item_resource[item_data::dItemNo_LEGENDARY_PICTOGRAPH_e].mArcname = archiveVboxa;
    dItem_data::item_resource[item_data::dItemNo_LEGENDARY_PICTOGRAPH_e].mTexture = textureCamera;
    const char* archiveVlett = dItem_data::item_arcname_tbl[ARCNAME_IDX_VLETT].get();
    const char* archiveVobj11 = dItem_data::item_arcname_tbl[ARCNAME_IDX_VOBJ11].get();
    dItem_data::item_resource[item_data::dItemNo_SHOP_GURU_STATUE_e].mTexture = textureDelivery12;
    dItem_data::item_resource[item_data::dItemNo_SHOP_GURU_STATUE_e].mArcname = archiveVobj11;
    dItem_data::item_resource[item_data::dItemNo_FATHER_LETTER_e].mArcname = archiveVlett;
    const char* archiveVtin1 = dItem_data::item_arcname_tbl[ARCNAME_IDX_VTIN1].get();
    dItem_data::item_resource[item_data::dItemNo_SALVAGE_ITEM_2_e].mArcname = archiveVamuk;
    dItem_data::item_resource[item_data::dItemNo_SALVAGE_ITEM_2_e].mTexture = textureAmulet00;
    dItem_data::item_resource[item_data::dItemNo_SALVAGE_ITEM_3_e].mArcname = archiveVamuk;
    dItem_data::item_resource[item_data::dItemNo_SALVAGE_ITEM_3_e].mTexture = textureAmulet00;
    dItem_data::item_resource[item_data::dItemNo_XXX_039_e].mArcname = archiveVamuk;
    dItem_data::item_resource[item_data::dItemNo_XXX_039_e].mTexture = textureAmulet00;
    dItem_data::item_resource[item_data::dItemNo_TINGLE_STATUE_1_e].mArcname = archiveVtin1;
    const char* archiveVtin2 = dItem_data::item_arcname_tbl[ARCNAME_IDX_VTIN2].get();
    dItem_data::item_resource[item_data::dItemNo_TINGLE_STATUE_1_e].mTexture = textureTingleFigure;
    dItem_data::item_resource[item_data::dItemNo_TINGLE_STATUE_2_e].mArcname = archiveVtin2;
    dItem_data::item_resource[item_data::dItemNo_TINGLE_STATUE_3_e].mTexture = textureTingleFigure;
    const char* archiveVsail = dItem_data::item_arcname_tbl[ARCNAME_IDX_VSAIL].get();
    dItem_data::item_resource[item_data::dItemNo_TINGLE_STATUE_5_e].mArcname = archiveVtin5;
    dItem_data::item_resource[item_data::dItemNo_TINGLE_STATUE_4_e].mTexture = textureTingleFigure;
    dItem_data::item_resource[item_data::dItemNo_MAX_RUPEE_UP2_e].mArcname = archiveVsail;
    const char* archiveVsaim = dItem_data::item_arcname_tbl[ARCNAME_IDX_VSAIM].get();
    dItem_data::item_resource[item_data::dItemNo_HURRICANE_SPIN_e].mArcname = archiveVswon;
    const char* archiveVbomm = dItem_data::item_arcname_tbl[ARCNAME_IDX_VBOMM].get();
    dItem_data::item_resource[item_data::dItemNo_MAX_RUPEE_UP1_e].mArcname = archiveVsaim;
    dItem_data::item_resource[item_data::dItemNo_MAX_BOMB_UP1_e].mArcname = archiveVbomm;
    const char* textureBombpouch1 = dItem_data::item_texture_tbl[TEXTURE_IDX_BOMBPOUCH_1].get();
    dItem_data::item_resource[item_data::dItemNo_TINGLE_STATUE_2_e].mTexture = textureTingleFigure;
    dItem_data::item_resource[item_data::dItemNo_MAX_BOMB_UP1_e].mTexture = textureBombpouch1;
    const char* textureBombpouch2 = dItem_data::item_texture_tbl[TEXTURE_IDX_BOMBPOUCH_2].get();
    const char* textureBigPurse = dItem_data::item_texture_tbl[TEXTURE_IDX_BIG_PURSE].get();
    dItem_data::item_resource[item_data::dItemNo_MAX_BOMB_UP2_e].mArcname = archiveVboml;
    dItem_data::item_resource[item_data::dItemNo_MAX_RUPEE_UP1_e].mTexture = textureBigPurse;
    const char* archiveVyatm = dItem_data::item_arcname_tbl[ARCNAME_IDX_VYATM].get();
    dItem_data::item_resource[item_data::dItemNo_MAX_BOMB_UP2_e].mTexture = textureBombpouch2;
    dItem_data::item_resource[item_data::dItemNo_MAX_ARROW_UP1_e].mArcname = archiveVyatm;
    const char* textureArrowcase1 = dItem_data::item_texture_tbl[TEXTURE_IDX_ARROWCASE_1].get();
    const char* textureMaxPurse = dItem_data::item_texture_tbl[TEXTURE_IDX_MAX_PURSE].get();
    dItem_data::item_resource[item_data::dItemNo_MAX_RUPEE_UP2_e].mTexture = textureMaxPurse;
    const char* archiveVtin4 = dItem_data::item_arcname_tbl[ARCNAME_IDX_VTIN4].get();
    const char* archiveVyatl = dItem_data::item_arcname_tbl[ARCNAME_IDX_VYATL].get();
    dItem_data::item_resource[item_data::dItemNo_TINGLE_STATUE_4_e].mArcname = archiveVtin4;
    const char* archiveVtin3 = dItem_data::item_arcname_tbl[ARCNAME_IDX_VTIN3].get();
    dItem_data::item_resource[item_data::dItemNo_MAX_ARROW_UP1_e].mTexture = textureArrowcase1;
    dItem_data::item_resource[item_data::dItemNo_TINGLE_STATUE_3_e].mArcname = archiveVtin3;
    const char* textureArrowcase2 = dItem_data::item_texture_tbl[TEXTURE_IDX_ARROWCASE_2].get();
    dItem_data::item_resource[item_data::dItemNo_MAX_ARROW_UP2_e].mArcname = archiveVyatl;
    dItem_data::item_resource[item_data::dItemNo_MAX_ARROW_UP2_e].mTexture = textureArrowcase2;
    const char* archiveVmapq = dItem_data::item_arcname_tbl[ARCNAME_IDX_VMAPQ].get();
    dItem_data::item_resource[item_data::dItemNo_TINGLE_RUPEE_1_e].mArcname = archiveVlupy;
    dItem_data::item_resource[item_data::dItemNo_TINGLE_RUPEE_1_e].mTexture = textureGetRupy;
    dItem_data::item_resource[item_data::dItemNo_TINGLE_RUPEE_2_e].mArcname = archiveVlupy;
    dItem_data::item_resource[item_data::dItemNo_TINGLE_RUPEE_2_e].mTexture = textureGetRupy;
    dItem_data::item_resource[item_data::dItemNo_TINGLE_RUPEE_3_e].mArcname = archiveVlupy;
    dItem_data::item_resource[item_data::dItemNo_TINGLE_RUPEE_3_e].mTexture = textureGetRupy;
    dItem_data::item_resource[item_data::dItemNo_TINGLE_RUPEE_4_e].mArcname = archiveVlupy;
    dItem_data::item_resource[item_data::dItemNo_TINGLE_RUPEE_4_e].mTexture = textureGetRupy;
    dItem_data::item_resource[item_data::dItemNo_TINGLE_RUPEE_5_e].mArcname = archiveVlupy;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_61_e].mArcname = archiveVmapq;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_63_e].mArcname = archiveVlupy;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_62_e].mTexture = textureGetRupy;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_64_e].mArcname = archiveVlupy;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_63_e].mTexture = textureGetRupy;
    dItem_data::item_resource[item_data::dItemNo_LITHOGRAPH_6_e].mArcname = archiveVlupy;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_64_e].mTexture = textureGetRupy;
    dItem_data::item_resource[item_data::dItemNo_LITHOGRAPH_5_e].mArcname = archiveVlupy;
    dItem_data::item_resource[item_data::dItemNo_LITHOGRAPH_6_e].mTexture = textureGetRupy;
    dItem_data::item_resource[item_data::dItemNo_LITHOGRAPH_4_e].mArcname = archiveVlupy;
    dItem_data::item_resource[item_data::dItemNo_LITHOGRAPH_5_e].mTexture = textureGetRupy;
    dItem_data::item_resource[item_data::dItemNo_LITHOGRAPH_3_e].mArcname = archiveVlupy;
    dItem_data::item_resource[item_data::dItemNo_LITHOGRAPH_4_e].mTexture = textureGetRupy;
    dItem_data::item_resource[item_data::dItemNo_LITHOGRAPH_3_e].mTexture = textureGetRupy;
    dItem_data::item_resource[item_data::dItemNo_LITHOGRAPH_2_e].mTexture = textureGetRupy;
    dItem_data::item_resource[item_data::dItemNo_LITHOGRAPH_2_e].mArcname = archiveVlupy;
    const char* textureCmapHint2 = dItem_data::item_texture_tbl[TEXTURE_IDX_CMAP_HINT2].get();
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_60_e].mArcname = archiveVmapq;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_58_e].mArcname = archiveVmapq;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_59_e].mArcname = archiveVmapq;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_57_e].mArcname = archiveVmapq;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_61_e].mTexture = textureCmapHint2;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_60_e].mTexture = textureCmapHint2;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_59_e].mTexture = textureCmapHint2;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_58_e].mTexture = textureCmapHint2;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_57_e].mTexture = textureCmapHint2;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_56_e].mArcname = archiveVmapq;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_56_e].mTexture = textureCmapHint2;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_55_e].mArcname = archiveVmapq;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_55_e].mTexture = textureCmapHint2;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_54_e].mArcname = archiveVmapq;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_54_e].mTexture = textureCmapHint2;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_53_e].mArcname = archiveVmapq;
    dItem_data::item_resource[item_data::dItemNo_TINGLE_RUPEE_6_e].mArcname = archiveVlupy;
    dItem_data::item_resource[item_data::dItemNo_LITHOGRAPH_1_e].mTexture = textureGetRupy;
    dItem_data::item_resource[item_data::dItemNo_LITHOGRAPH_1_e].mArcname = archiveVlupy;
    dItem_data::item_resource[item_data::dItemNo_TINGLE_RUPEE_6_e].mTexture = textureGetRupy;
    dItem_data::item_resource[item_data::dItemNo_TINGLE_RUPEE_5_e].mTexture = textureGetRupy;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_53_e].mTexture = textureCmapHint2;
    const char* textureCmapTreasure20X6A =
        dItem_data::item_texture_tbl[TEXTURE_IDX_CMAP_TREASURE2_0x6A].get();
    const char* archiveVmapc = dItem_data::item_arcname_tbl[ARCNAME_IDX_VMAPC].get();
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_39_e].mTexture =
        textureCmapTreasure20X6A;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_52_e].mArcname = archiveVmapc;
    const char* archiveVmapg = dItem_data::item_arcname_tbl[ARCNAME_IDX_VMAPG].get();
    const char* textureCmapPhantomship2 =
        dItem_data::item_texture_tbl[TEXTURE_IDX_CMAP_PHANTOMSHIP2].get();
    const char* textureCmapTingle2 = dItem_data::item_texture_tbl[TEXTURE_IDX_CMAP_TINGLE2].get();
    const char* archiveVmapr = dItem_data::item_arcname_tbl[ARCNAME_IDX_VMAPR].get();
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_52_e].mTexture = textureCmapTingle2;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_51_e].mArcname = archiveVmapr;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_36_e].mArcname = archiveVmapg;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_40_e].mTexture =
        textureCmapTreasure20X6A;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_41_e].mTexture =
        textureCmapTreasure20X6A;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_42_e].mTexture =
        textureCmapTreasure20X6A;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_43_e].mTexture =
        textureCmapTreasure20X6A;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_44_e].mTexture =
        textureCmapTreasure20X6A;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_45_e].mTexture =
        textureCmapTreasure20X6A;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_35_e].mArcname = archiveVmapc;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_36_e].mTexture =
        textureCmapPhantomship2;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_35_e].mTexture = textureCmapTingle2;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_34_e].mArcname = archiveVmapr;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_34_e].mTexture =
        textureCmapTreasure20X6A;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_33_e].mArcname = archiveVmapr;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_33_e].mTexture =
        textureCmapTreasure20X6A;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_32_e].mArcname = archiveVmapr;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_32_e].mTexture =
        textureCmapTreasure20X6A;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_31_e].mArcname = archiveVmapr;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_31_e].mTexture =
        textureCmapTreasure20X6A;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_30_e].mArcname = archiveVmapr;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_30_e].mTexture =
        textureCmapTreasure20X6A;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_29_e].mArcname = archiveVmapr;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_46_e].mTexture =
        textureCmapTreasure20X6A;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_47_e].mTexture =
        textureCmapTreasure20X6A;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_62_e].mArcname = archiveVlupy;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_38_e].mArcname = archiveVmapr;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_39_e].mArcname = archiveVmapr;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_38_e].mTexture =
        textureCmapTreasure20X6A;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_37_e].mTexture =
        textureCmapTreasure20X6A;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_49_e].mArcname = archiveVmapr;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_48_e].mArcname = archiveVmapr;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_47_e].mArcname = archiveVmapr;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_46_e].mArcname = archiveVmapr;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_45_e].mArcname = archiveVmapr;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_40_e].mArcname = archiveVmapr;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_41_e].mArcname = archiveVmapr;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_42_e].mArcname = archiveVmapr;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_50_e].mArcname = archiveVmapr;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_29_e].mTexture =
        textureCmapTreasure20X6A;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_28_e].mArcname = archiveVmapr;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_28_e].mTexture =
        textureCmapTreasure20X6A;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_27_e].mArcname = archiveVmapr;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_27_e].mTexture =
        textureCmapTreasure20X6A;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_26_e].mArcname = archiveVmapr;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_26_e].mTexture =
        textureCmapTreasure20X6A;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_25_e].mArcname = archiveVmapr;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_25_e].mTexture =
        textureCmapTreasure20X6A;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_24_e].mArcname = archiveVmapr;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_24_e].mTexture =
        textureCmapTreasure20X6A;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_23_e].mArcname = archiveVmapr;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_23_e].mTexture =
        textureCmapTreasure20X6A;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_22_e].mArcname = archiveVmapr;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_22_e].mTexture =
        textureCmapTreasure20X6A;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_21_e].mArcname = archiveVmapr;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_21_e].mTexture =
        textureCmapTreasure20X6A;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_51_e].mTexture =
        textureCmapTreasure20X6A;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_50_e].mTexture =
        textureCmapTreasure20X6A;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_49_e].mTexture =
        textureCmapTreasure20X6A;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_48_e].mTexture =
        textureCmapTreasure20X6A;
    const char* archiveVmaph = dItem_data::item_arcname_tbl[ARCNAME_IDX_VMAPH].get();
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_20_e].mArcname = archiveVmapr;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_18_e].mArcname = archiveVmaph;
    const char* textureCmapTreasure20X69 =
        dItem_data::item_texture_tbl[TEXTURE_IDX_CMAP_TREASURE2_0x69].get();
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_37_e].mArcname = archiveVmapr;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_18_e].mTexture =
        textureCmapTreasure20X69;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_17_e].mArcname = archiveVmaph;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_17_e].mTexture =
        textureCmapTreasure20X69;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_16_e].mArcname = archiveVmaph;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_44_e].mArcname = archiveVmapr;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_43_e].mArcname = archiveVmapr;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_16_e].mTexture =
        textureCmapTreasure20X69;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_15_e].mArcname = archiveVmaph;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_15_e].mTexture =
        textureCmapTreasure20X69;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_14_e].mArcname = archiveVmaph;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_14_e].mTexture =
        textureCmapTreasure20X69;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_13_e].mArcname = archiveVmaph;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_13_e].mTexture =
        textureCmapTreasure20X69;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_12_e].mArcname = archiveVmaph;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_12_e].mTexture =
        textureCmapTreasure20X69;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_11_e].mArcname = archiveVmaph;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_11_e].mTexture =
        textureCmapTreasure20X69;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_10_e].mArcname = archiveVmaph;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_10_e].mTexture =
        textureCmapTreasure20X69;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_09_e].mTexture =
        textureCmapTreasure20X69;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_08_e].mArcname = archiveVmapr;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_08_e].mTexture =
        textureCmapTreasure20X6A;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_07_e].mArcname = archiveVmapr;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_09_e].mArcname = archiveVmaph;
    const char* archiveLink = dItem_data::item_arcname_tbl[ARCNAME_IDX_LINK].get();
    dItem_data::field_item_res[item_data::dItemNo_HEART_e].mArc = archiveAlways;
    dItem_data::field_item_res[item_data::dItemNo_LARGE_MAGIC_e].mArc = archiveAlways;
    dItem_data::field_item_res[item_data::dItemNo_BOMB_10_e].mArc = archiveLink;
    dItem_data::field_item_res[item_data::dItemNo_BOMB_5_e].mArc = archiveLink;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_19_e].mTexture =
        textureCmapTreasure20X6A;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_19_e].mArcname = archiveVmapr;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_20_e].mTexture =
        textureCmapTreasure20X6A;
    dItem_data::field_item_res[item_data::dItemNo_GREEN_RUPEE_e].mArc = archiveAlways;
    dItem_data::field_item_res[item_data::dItemNo_BLUE_RUPEE_e].mArc = archiveAlways;
    dItem_data::field_item_res[item_data::dItemNo_YELLOW_RUPEE_e].mArc = archiveAlways;
    dItem_data::field_item_res[item_data::dItemNo_RED_RUPEE_e].mArc = archiveAlways;
    dItem_data::field_item_res[item_data::dItemNo_BOMB_30_e].mArc = archiveLink;
    dItem_data::field_item_res[item_data::dItemNo_BOMB_20_e].mArc = archiveLink;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_07_e].mTexture =
        textureCmapTreasure20X6A;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_06_e].mArcname = archiveVmapr;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_06_e].mTexture =
        textureCmapTreasure20X6A;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_05_e].mArcname = archiveVmapt;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_05_e].mTexture = textureCmapTri2;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_04_e].mArcname = archiveVmapr;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_04_e].mTexture =
        textureCmapTreasure20X6A;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_03_e].mArcname = archiveVmapt;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_03_e].mTexture = textureCmapTri2;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_02_e].mArcname = archiveVmapr;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_02_e].mTexture =
        textureCmapTreasure20X6A;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_01_e].mArcname = archiveVmapt;
    dItem_data::item_resource[item_data::dItemNo_COLLECT_MAP_01_e].mTexture = textureCmapTri2;
    dItem_data::field_item_res[item_data::dItemNo_ARROW_10_e].mArc = archiveAlways;
    dItem_data::field_item_res[item_data::dItemNo_MAGIC_ARROW_e].mArc = archiveLink;
    dItem_data::field_item_res[item_data::dItemNo_LIGHT_ARROW_e].mArc = archiveLink;
    dItem_data::field_item_res[item_data::dItemNo_SKULL_NECKLACE_e].mArc = archiveAlways;
    dItem_data::field_item_res[item_data::dItemNo_BOKOBABA_SEED_e].mArc = archiveAlways;
    dItem_data::field_item_res[item_data::dItemNo_GOLDEN_FEATHER_e].mArc = archiveAlways;
    dItem_data::field_item_res[item_data::dItemNo_KNIGHTS_CREST_e].mArc = archiveAlways;
    dItem_data::field_item_res[item_data::dItemNo_RED_JELLY_e].mArc = archiveAlways;
    dItem_data::field_item_res[item_data::dItemNo_GREEN_JELLY_e].mArc = archiveAlways;
    dItem_data::field_item_res[item_data::dItemNo_BLUE_JELLY_e].mArc = archiveAlways;
    dItem_data::field_item_res[item_data::dItemNo_SMALL_MAGIC_e].mArc = archiveAlways;
    dItem_data::field_item_res[item_data::dItemNo_HEART_CONTAINER_e].mArc = archiveAlways;
    dItem_data::field_item_res[item_data::dItemNo_HEART_PIECE_e].mArc = archiveAlways;
    dItem_data::field_item_res[item_data::dItemNo_ORANGE_RUPEE_e].mArc = archiveAlways;
    dItem_data::field_item_res[item_data::dItemNo_PURPLE_RUPEE_e].mArc = archiveAlways;
    dItem_data::field_item_res[item_data::dItemNo_TRIPLE_HEART_e].mArc = archiveAlways;
    dItem_data::field_item_res[item_data::dItemNo_JOY_PENDANT_e].mArc = archiveAlways;
    dItem_data::field_item_res[item_data::dItemNo_ARROW_20_e].mArc = archiveAlways;
    dItem_data::field_item_res[item_data::dItemNo_SILVER_RUPEE_e].mArc = archiveAlways;
    dItem_data::field_item_res[item_data::dItemNo_DEKU_LEAF_e].mArc = archiveVleaf;
    dItem_data::field_item_res[item_data::dItemNo_SWORD_e].mArc = archiveVswon;
    const char* archiveFdai0X58 = dItem_data::item_arcname_tbl[ARCNAME_IDX_FDAI_0x58].get();
    const char* archiveFdai0X59 = dItem_data::item_arcname_tbl[ARCNAME_IDX_FDAI_0x59].get();
    dItem_data::field_item_res[item_data::dItemNo_TOWN_FLOWER_e].mArc = archiveFdai0X58;
    dItem_data::field_item_res[item_data::dItemNo_SEA_FLOWER_e].mArc = archiveFdai0X59;
    const char* archiveFdai0X5B = dItem_data::item_arcname_tbl[ARCNAME_IDX_FDAI_0x5B].get();
    dItem_data::field_item_res[item_data::dItemNo_ARROW_30_e].mArc = archiveAlways;
    dItem_data::field_item_res[item_data::dItemNo_NOENTRY_19_e].mArc = archiveAlways;
    dItem_data::field_item_res[item_data::dItemNo_NOENTRY_20_e].mArc = archiveAlways;
    dItem_data::field_item_res[item_data::dItemNo_SMALL_KEY_e].mArc = archiveAlways;
    dItem_data::field_item_res[item_data::dItemNo_RECOVER_FAIRY_e].mArc = archiveAlways;
    dItem_data::field_item_res[item_data::dItemNo_SHIELD_e].mArc = archiveVshin;
    dItem_data::field_item_res[item_data::dItemNo_DROPPED_SWORD_e].mArc = archiveVswon;
    const char* archiveFdai0X5A = dItem_data::item_arcname_tbl[ARCNAME_IDX_FDAI_0x5A].get();
    dItem_data::field_item_res[item_data::dItemNo_EXOTIC_FLOWER_e].mArc = archiveFdai0X5B;
    dItem_data::field_item_res[item_data::dItemNo_HEROS_FLAG_e].mArc = archiveFdai0X5B;
    dItem_data::field_item_res[item_data::dItemNo_BIG_CATCH_FLAG_e].mArc = archiveFdai0X5B;
    dItem_data::field_item_res[item_data::dItemNo_BIG_SALE_FLAG_e].mArc = archiveFdai0X5B;
    dItem_data::field_item_res[item_data::dItemNo_PINWHEEL_e].mArc = archiveFdai0X5A;
    const char* archiveFdai0X5C = dItem_data::item_arcname_tbl[ARCNAME_IDX_FDAI_0x5C].get();
    dItem_data::field_item_res[item_data::dItemNo_POSTMAN_STATUE_e].mArc = archiveFdai0X5A;
    dItem_data::field_item_res[item_data::dItemNo_SKULL_TOWER_IDOL_e].mArc = archiveFdai0X5C;
    dItem_data::field_item_res[item_data::dItemNo_SICKLE_MOON_FLAG_e].mArc = archiveFdai0X5B;
    const char* archiveFdai0X64 = dItem_data::item_arcname_tbl[ARCNAME_IDX_FDAI_0x64].get();
    dItem_data::field_item_res[item_data::dItemNo_SHOP_GURU_STATUE_e].mArc = archiveFdai0X5A;
    dItem_data::field_item_res[item_data::dItemNo_BIRD_BAIT_5_e].mArc = archiveAlways;
    dItem_data::field_item_res[item_data::dItemNo_HYOI_PEAR_e].mArc = archiveVhyoi;
    dItem_data::field_item_res[item_data::dItemNo_FOUNTAIN_IDOL_e].mArc = archiveFdai0X64;
}

/* 02552844 */
static void __sinit_d_item_data_cpp() {
    l_headerStatics.mZeroWords[2] = 0;
    l_headerStatics.mZeroWords[0] = 0;
    l_headerStatics.mZeroWords[3] = 0;
    l_headerStatics.mZeroWords[1] = 0;
    Item_registerStatic(&l_records.mEntries[0]);
    f32 negativePi = l_angles.mNegativePi, pi = l_angles.mPi;
    l_headerStatics.mNegativePi = negativePi;
    l_headerStatics.mPi = pi;
    Item_initFirstObject(&l_headerStatics.mFirstObject);
    Item_registerStatic(&l_records.mEntries[1]);
    Item_initSecondObject(&l_headerStatics.mSecondObject);
    Item_registerStatic(&l_records.mEntries[2]);
}

// clang-format off
/* WWHD glue
data l_archiveNames 101E42E0
data l_textureNames 101E44A4
data l_itemResources 101E4674
data l_fieldResources 101E6A74
data l_itemInfos 101E8674
data l_effects 101E8A74
data l_resourceHeaderStatics 104759F8
data l_headerStatics 10475A14
data l_resourceRecords 101E42BC
data l_records 101E8C78
data l_resourceAngles 1004EEF4
data l_angles 1004EF00
str 1004EE68 "d_item_data.cpp" in 02551BBC
str 1004EE78 "effect_info[no].m_appear != dPa_name::ID_SCENE_END" in 02551BBC
str 1004EEAC "d_item_data.cpp" in 02551C48
str 1004EEBC "effect_info[no].m_sp_effect != dPa_name::ID_SCENE_END" in 02551C48
ext 028F026C void Item_registerStatic(void* descriptor)
ext 028ED6F8 void Item_initFirstObject(u8* object)
ext 028EAB2C void Item_initSecondObject(u8* object)
*/
// clang-format on
