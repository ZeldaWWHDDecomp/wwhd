/** HD main scene state completion defaults, screen resources, and manager lifetime. */
#include "d/hd_scene_main_screens.h"
#include "gc_api/tu.h"
const gc_tu_data gc_tu = {.safeString = 0x1013F708};
static hdMainLayoutMgr_l* sLayoutManager;
static void* sResourceManager;
static hdMainScreenManager_c* sScreenManager;
static hdMainDisposer_l* sScreenDisposer;
static u32 sManagerVtable;
static u32 sDisposerVtable;

/* 027195F8 */
BOOL hdMainScene_c::isToPlayComplete() { return FALSE; }

/* 02719604 */
BOOL hdMainScene_c::isPlayComplete() { return FALSE; }

/* 02719610 */
BOOL hdMainScene_c::isToPauseDrcComplete() { return FALSE; }

/* 02719618 */
BOOL hdMainScene_c::isPauseDrcComplete() { return FALSE; }

/* 02719624 */
BOOL hdMainScene_c::isToMenuComplete() { return FALSE; }

/* 02719630 */
BOOL hdMainScene_c::isMenuComplete() { return FALSE; }

/* 0271963C */
BOOL hdMainScene_c::isMenuSubWindowComplete() { return FALSE; }

/* 02719648 */
BOOL hdMainScene_c::isToSaveComplete() { return FALSE; }

/* 02719650 */
BOOL hdMainScene_c::isSaveComplete() { return FALSE; }

/* 0271965C */
BOOL hdMainScene_c::isToOptionComplete() { return FALSE; }

/* 02719668 */
BOOL hdMainScene_c::isOptionComplete() { return FALSE; }

/* 02719674 */
BOOL hdMainScene_c::isToTaktComplete() { return FALSE; }

/* 02719680 */
BOOL hdMainScene_c::isTaktComplete() { return FALSE; }

/* 0271968C */
BOOL hdMainScene_c::isToTaktSignListComplete() { return FALSE; }

/* 02719698 */
BOOL hdMainScene_c::isTaktSignListComplete() { return FALSE; }

/* 027196A4 */
BOOL hdMainScene_c::isReturnTaktForProModeComplete() { return FALSE; }

/* 027196B0 */
BOOL hdMainScene_c::isTaktPlayDemoComplete() { return FALSE; }

/* 027196B8 */
BOOL hdMainScene_c::isControlWindComplete() { return FALSE; }

/* 027196C4 */
BOOL hdMainScene_c::isToWarpComplete() { return FALSE; }

/* 027196CC */
BOOL hdMainScene_c::isWarpComplete() { return FALSE; }

/* 027196D8 */
BOOL hdMainScene_c::isPhotoViewMiiverseComplete() { return FALSE; }

/* 027196E0 */
BOOL hdMainScene_c::isMiiverseEmpathingComplete() { return FALSE; }

/* 027196EC */
BOOL hdMainScene_c::isToMiiverseSettingComplete() { return FALSE; }

/* 027196F4 */
BOOL hdMainScene_c::isMiiverseSettingComplete() { return FALSE; }

/* 02719700 */
BOOL hdMainScene_c::isToConfirmMiiverseCommentJumpComplete() { return FALSE; }

/* 0271970C */
BOOL hdMainScene_c::isConfirmMiiverseCommentJumpComplete() { return FALSE; }

/* 02719718 */
BOOL hdMainScene_c::isToConfirmMiiverseProfileJumpComplete() { return FALSE; }

/* 02719724 */
BOOL hdMainScene_c::isConfirmMiiverseProfileJumpComplete() { return FALSE; }

/* 02719730 */
BOOL hdMainScene_c::isMiiverseProfileJumpComplete() { return FALSE; }

/* 0271973C */
BOOL hdMainScene_c::isToMiiverseExplainComplete() { return FALSE; }

/* 02719744 */
BOOL hdMainScene_c::isMiiverseExplainComplete() { return FALSE; }

/* 02719750 */
BOOL hdMainScene_c::isToTelescopeComplete() { return FALSE; }

/* 0271975C */
BOOL hdMainScene_c::isTelescopeComplete() { return FALSE; }

/* 02719768 */
BOOL hdMainScene_c::isToPictographBoxComplete() { return FALSE; }

/* 02719774 */
BOOL hdMainScene_c::isPictographBoxComplete() { return FALSE; }

/* 02719780 */
BOOL hdMainScene_c::isToPictographListComplete() { return FALSE; }

/* 02719788 */
BOOL hdMainScene_c::isPictographListComplete() { return FALSE; }

/* 02719794 */
BOOL hdMainScene_c::isReturnPictographBoxComplete() { return FALSE; }

/* 027197A0 */
BOOL hdMainScene_c::isExitPictographListComplete() { return FALSE; }

/* 027197AC */
BOOL hdMainScene_c::isToSelectPictographListComplete() { return FALSE; }

/* 027197B8 */
BOOL hdMainScene_c::isSelectPictographListComplete() { return FALSE; }

/* 027197C4 */
BOOL hdMainScene_c::isToFishManHintComplete() { return FALSE; }

/* 027197CC */
BOOL hdMainScene_c::isFishManHintComplete() { return FALSE; }

/* 027197D4 */
BOOL hdMainScene_c::isInputPasswordComplete() { return FALSE; }

/* 027197E0 */
BOOL hdMainScene_c::isDemoTauraArrivalComplete() { return FALSE; }

/* 027197EC */
BOOL hdMainScene_c::isToDemoFishManComplete() { return FALSE; }

/* 027197F8 */
BOOL hdMainScene_c::isDemoFishManComplete() { return FALSE; }

/* 02719804 */
BOOL hdMainScene_c::isToDemoTriforceComplete() { return FALSE; }

/* 0271980C */
BOOL hdMainScene_c::isDemoTriforceComplete() { return FALSE; }

/* 02719818 */
BOOL hdMainScene_c::isDemoPictBottleComplete() { return FALSE; }

/* 02719824 */
BOOL hdMainScene_c::isGameOverComplete() { return FALSE; }

/* 02719830 */
BOOL hdMainScene_c::isChangeToTvModeComplete() { return FALSE; }

/* 0271983C */
BOOL hdMainScene_c::isChangeToDrcModeComplete() { return FALSE; }

/* 02719844 */
BOOL hdMainScene_c::isExchangeProModeComplete() { return FALSE; }

/* 0271984C */
void* hdMainStateHandle_c::getDescriptor() { return mpDescriptor; }
/* 02719920 */
void* hdMainResourceOwner_c::attachOwner(void* owner) { mpOwner = owner; return &mpResource; }
/* 0271992C */
hdMainResourceOwner_c* hdMainResourceOwner_c::clearOutput(gptr<void>* output) { *output = NULL; return this; }
/* 02719938 */
hdMainResourceAllocator_c* hdMainResourceOwner_c::getAllocator() { return mpAllocator; }
/* 02719940 */
u32 hdMainResourceOwner_c::prepare() {
    hdMainResourceAllocator_c* allocator = mpAllocator;
    be<u32>* table = allocator->mpVtable;
    return gabi::tail_ptr<u32>(table[9], allocator, (void*)mpResource);
}
/* 0271995C */
u32 hdMainResourceOwner_c::finish() {
    hdMainResourceAllocator_c* allocator = mpAllocator;
    be<u32>* table = allocator->mpVtable;
    return gabi::tail_ptr<u32>(table[11], allocator, (void*)mpResource);
}
/* 02719978 */
void hdMainResourceOwner_c::release() {
    hdMainResourceAllocator_c* allocator = mpAllocator;
    be<u32>* table = allocator->mpVtable;
    gabi::call_ptr<void>(table[13], allocator, (void*)mpResource);
    mpAllocator = NULL;
}
/* 027199C0 */
static void refreshMainResources() { hdMainRefreshResources(sResourceManager); }
/* 027199CC */
static void registerMainScreens() {
    void* heap = sLayoutManager->mpHeap;
    gabi::Local<SafeString> key;
    SafeString* constructedKey = NULL;
    key->mStringTop = gabi::guest_str("Main_00");
    key->__vtbl = gc_tu.safeString;
    hdMainRegisterScreen(sLayoutManager, key, heap);
    key->mStringTop = gabi::guest_str("HeartAll_00");
    key->__vtbl = gc_tu.safeString;
    hdMainRegisterScreen(sLayoutManager, key, heap);
    key->mStringTop = gabi::guest_str("Heart_00");
    key->__vtbl = gc_tu.safeString;
    hdMainRegisterScreen(sLayoutManager, key, heap);
    key->mStringTop = gabi::guest_str("Rupy_00");
    key->__vtbl = gc_tu.safeString;
    hdMainRegisterScreen(sLayoutManager, key, heap);
    key->mStringTop = gabi::guest_str("MagicBar_00");
    key->__vtbl = gc_tu.safeString;
    hdMainRegisterScreen(sLayoutManager, key, heap);
    key->mStringTop = gabi::guest_str("DungeonKey_00");
    key->__vtbl = gc_tu.safeString;
    hdMainRegisterScreen(sLayoutManager, key, heap);
    key->mStringTop = gabi::guest_str("UnreadMiiverse_00");
    key->__vtbl = gc_tu.safeString;
    hdMainRegisterScreen(sLayoutManager, key, heap);
    key->mStringTop = gabi::guest_str("CompassClock_00");
    key->__vtbl = gc_tu.safeString;
    hdMainRegisterScreen(sLayoutManager, key, heap);
    key->mStringTop = gabi::guest_str("SwimTime_00");
    key->__vtbl = gc_tu.safeString;
    hdMainRegisterScreen(sLayoutManager, key, heap);
    key->mStringTop = gabi::guest_str("MenuItem_00");
    key->__vtbl = gc_tu.safeString;
    hdMainRegisterScreen(sLayoutManager, key, heap);
    key->mStringTop = gabi::guest_str("BtnItemIcon_00");
    key->__vtbl = gc_tu.safeString;
    hdMainRegisterScreen(sLayoutManager, key, heap);
    key->mStringTop = gabi::guest_str("BtnMenuArrow_00");
    key->__vtbl = gc_tu.safeString;
    hdMainRegisterScreen(sLayoutManager, key, heap);
    key->mStringTop = gabi::guest_str("BtnMenuSaveOption_00");
    key->__vtbl = gc_tu.safeString;
    hdMainRegisterScreen(sLayoutManager, key, heap);
    key->mStringTop = gabi::guest_str("BtnBack_00");
    key->__vtbl = gc_tu.safeString;
    hdMainRegisterScreen(sLayoutManager, key, heap);
    key->mStringTop = gabi::guest_str("BtnCollectIcon_00");
    key->__vtbl = gc_tu.safeString;
    hdMainRegisterScreen(sLayoutManager, key, heap);
    key->mStringTop = gabi::guest_str("BtnCollectIconMask_00");
    key->__vtbl = gc_tu.safeString;
    hdMainRegisterScreen(sLayoutManager, key, heap);
    key->mStringTop = gabi::guest_str("Collection_00");
    key->__vtbl = gc_tu.safeString;
    hdMainRegisterScreen(sLayoutManager, key, heap);
    key->mStringTop = gabi::guest_str("BtnMenuHeart_00");
    key->__vtbl = gc_tu.safeString;
    hdMainRegisterScreen(sLayoutManager, key, heap);
    key->mStringTop = gabi::guest_str("BtnMapIcon_00");
    key->__vtbl = gc_tu.safeString;
    hdMainRegisterScreen(sLayoutManager, key, heap);
    key->mStringTop = gabi::guest_str("IconSlideArea_00");
    key->__vtbl = gc_tu.safeString;
    hdMainRegisterScreen(sLayoutManager, key, heap);
    key->mStringTop = gabi::guest_str("BtnPearl_00");
    key->__vtbl = gc_tu.safeString;
    hdMainRegisterScreen(sLayoutManager, key, heap);
    key->mStringTop = gabi::guest_str("BtnTriforce_00");
    key->__vtbl = gc_tu.safeString;
    hdMainRegisterScreen(sLayoutManager, key, heap);
    key->mStringTop = gabi::guest_str("SlideAssistTV_00");
    key->__vtbl = gc_tu.safeString;
    hdMainRegisterScreen(sLayoutManager, key, heap);
    key->mStringTop = gabi::guest_str("MenuMiiverse_00");
    key->__vtbl = gc_tu.safeString;
    hdMainRegisterScreen(sLayoutManager, key, heap);
    key->mStringTop = gabi::guest_str("BtnMiiverseComment_00");
    key->__vtbl = gc_tu.safeString;
    hdMainRegisterScreen(sLayoutManager, key, heap);
    key->mStringTop = gabi::guest_str("BtnMiiverseYeah_00");
    key->__vtbl = gc_tu.safeString;
    hdMainRegisterScreen(sLayoutManager, key, heap);
    key->mStringTop = gabi::guest_str("BtnMiiversePicture_00");
    key->__vtbl = gc_tu.safeString;
    hdMainRegisterScreen(sLayoutManager, key, heap);
    key->mStringTop = gabi::guest_str("LoadingIcon_00");
    key->__vtbl = gc_tu.safeString;
    hdMainRegisterScreen(sLayoutManager, key, heap);
    key->mStringTop = gabi::guest_str("MenuBtns_00");
    key->__vtbl = gc_tu.safeString;
    hdMainRegisterScreen(sLayoutManager, key, heap);
    key->mStringTop = gabi::guest_str("BtnMenu_00");
    key->__vtbl = gc_tu.safeString;
    hdMainRegisterScreen(sLayoutManager, key, heap);
    key->mStringTop = gabi::guest_str("Cursor_00");
    key->__vtbl = gc_tu.safeString;
    hdMainRegisterScreen(sLayoutManager, key, heap);
    key->mStringTop = gabi::guest_str("Cursor_01");
    key->__vtbl = gc_tu.safeString;
    hdMainRegisterScreen(sLayoutManager, key, heap);
    key->mStringTop = gabi::guest_str("Cursor_02");
    key->__vtbl = gc_tu.safeString;
    hdMainRegisterScreen(sLayoutManager, key, heap);
    key->mStringTop = gabi::guest_str("CommandGuide_00");
    key->__vtbl = gc_tu.safeString;
    hdMainRegisterScreen(sLayoutManager, key, heap);
    key->mStringTop = gabi::guest_str("CommandA_00");
    key->__vtbl = gc_tu.safeString;
    hdMainRegisterScreen(sLayoutManager, key, heap);
    key->mStringTop = gabi::guest_str("GameOver_00");
    key->__vtbl = gc_tu.safeString;
    hdMainRegisterScreen(sLayoutManager, key, heap);
    key->mStringTop = gabi::guest_str("GameOverChar_00");
    key->__vtbl = gc_tu.safeString;
    hdMainRegisterScreen(sLayoutManager, key, heap);
    key->mStringTop = gabi::guest_str("PlaceName_00");
    key->__vtbl = gc_tu.safeString;
    hdMainRegisterScreen(sLayoutManager, key, heap);
    key->mStringTop = gabi::guest_str("Information_00");
    key->__vtbl = gc_tu.safeString;
    hdMainRegisterScreen(sLayoutManager, key, heap);
    key->mStringTop = gabi::guest_str("InformationDRC_00");
    key->__vtbl = gc_tu.safeString;
    hdMainRegisterScreen(sLayoutManager, key, heap);
    key->mStringTop = gabi::guest_str("MenuMap_00");
    key->__vtbl = gc_tu.safeString;
    hdMainRegisterScreen(sLayoutManager, key, heap);
    key->mStringTop = gabi::guest_str("MapAreaNow_00");
    key->__vtbl = gc_tu.safeString;
    hdMainRegisterScreen(sLayoutManager, key, heap);
    key->mStringTop = gabi::guest_str("BtnMenuArrow_00");
    key->__vtbl = gc_tu.safeString;
    hdMainRegisterScreen(sLayoutManager, key, heap);
    key->mStringTop = gabi::guest_str("BtnMapIcon_01");
    key->__vtbl = gc_tu.safeString;
    hdMainRegisterScreen(sLayoutManager, key, heap);
    key->mStringTop = gabi::guest_str("BtnMapBalloon_00");
    key->__vtbl = gc_tu.safeString;
    hdMainRegisterScreen(sLayoutManager, key, heap);
    key->mStringTop = gabi::guest_str("BtnFishMan_00");
    key->__vtbl = gc_tu.safeString;
    hdMainRegisterScreen(sLayoutManager, key, heap);
    key->mStringTop = gabi::guest_str("LinkPos_00");
    key->__vtbl = gc_tu.safeString;
    hdMainRegisterScreen(sLayoutManager, key, heap);
    key->mStringTop = gabi::guest_str("TreMap_00");
    key->__vtbl = gc_tu.safeString;
    hdMainRegisterScreen(sLayoutManager, key, heap);
    key->mStringTop = gabi::guest_str("Map_00");
    key->__vtbl = gc_tu.safeString;
    hdMainRegisterScreen(sLayoutManager, key, heap);
    key->mStringTop = gabi::guest_str("WarpMap_00");
    key->__vtbl = gc_tu.safeString;
    hdMainRegisterScreen(sLayoutManager, key, heap);
    key->mStringTop = gabi::guest_str("WarpArea_00");
    key->__vtbl = gc_tu.safeString;
    hdMainRegisterScreen(sLayoutManager, key, heap);
    key->mStringTop = gabi::guest_str("TincleTower_00");
    key->__vtbl = gc_tu.safeString;
    hdMainRegisterScreen(sLayoutManager, key, heap);
    key->mStringTop = gabi::guest_str("Fairy_00");
    key->__vtbl = gc_tu.safeString;
    hdMainRegisterScreen(sLayoutManager, key, heap);
    key->mStringTop = gabi::guest_str("HeartIsland_00");
    key->__vtbl = gc_tu.safeString;
    hdMainRegisterScreen(sLayoutManager, key, heap);
    key->mStringTop = gabi::guest_str("Squid_00");
    key->__vtbl = gc_tu.safeString;
    hdMainRegisterScreen(sLayoutManager, key, heap);
    key->mStringTop = gabi::guest_str("Triforce_00");
    key->__vtbl = gc_tu.safeString;
    hdMainRegisterScreen(sLayoutManager, key, heap);
    key->mStringTop = gabi::guest_str("Terry_00");
    key->__vtbl = gc_tu.safeString;
    hdMainRegisterScreen(sLayoutManager, key, heap);
    key->mStringTop = gabi::guest_str("SecretEntrance_00");
    key->__vtbl = gc_tu.safeString;
    hdMainRegisterScreen(sLayoutManager, key, heap);
    key->mStringTop = gabi::guest_str("Submarine_00");
    key->__vtbl = gc_tu.safeString;
    hdMainRegisterScreen(sLayoutManager, key, heap);
    key->mStringTop = gabi::guest_str("EnemyTower_00");
    key->__vtbl = gc_tu.safeString;
    hdMainRegisterScreen(sLayoutManager, key, heap);
    key->mStringTop = gabi::guest_str("Moon_00");
    key->__vtbl = gc_tu.safeString;
    hdMainRegisterScreen(sLayoutManager, key, heap);
    key->mStringTop = gabi::guest_str("HeartSea_00");
    key->__vtbl = gc_tu.safeString;
    hdMainRegisterScreen(sLayoutManager, key, heap);
    key->mStringTop = gabi::guest_str("GhostShip_00");
    key->__vtbl = gc_tu.safeString;
    hdMainRegisterScreen(sLayoutManager, key, heap);
    key->mStringTop = gabi::guest_str("DungeonMap_00");
    key->__vtbl = gc_tu.safeString;
    hdMainRegisterScreen(sLayoutManager, key, heap);
    key->mStringTop = gabi::guest_str("BtnFloor_00");
    key->__vtbl = gc_tu.safeString;
    hdMainRegisterScreen(sLayoutManager, key, heap);
    key->mStringTop = gabi::guest_str("BtnDungeonItemIcon_00");
    key->__vtbl = gc_tu.safeString;
    hdMainRegisterScreen(sLayoutManager, key, heap);
    key->mStringTop = gabi::guest_str("Searchlight_00");
    key->__vtbl = gc_tu.safeString;
    hdMainRegisterScreen(sLayoutManager, key, heap);
    key->mStringTop = gabi::guest_str("DungeonMapFloor_00");
    key->__vtbl = gc_tu.safeString;
    hdMainRegisterScreen(sLayoutManager, key, heap);
    key->mStringTop = gabi::guest_str("DungeonMapFloor_01");
    key->__vtbl = gc_tu.safeString;
    hdMainRegisterScreen(sLayoutManager, key, heap);
    key->mStringTop = gabi::guest_str("DungeonMapFloor_02");
    key->__vtbl = gc_tu.safeString;
    hdMainRegisterScreen(sLayoutManager, key, heap);
    key->mStringTop = gabi::guest_str("DungeonMapFloor_03");
    key->__vtbl = gc_tu.safeString;
    hdMainRegisterScreen(sLayoutManager, key, heap);
    key->mStringTop = gabi::guest_str("DungeonMapFloor_04");
    key->__vtbl = gc_tu.safeString;
    hdMainRegisterScreen(sLayoutManager, key, heap);
    key->mStringTop = gabi::guest_str("DungeonMapFloor_05");
    key->__vtbl = gc_tu.safeString;
    hdMainRegisterScreen(sLayoutManager, key, heap);
    key->mStringTop = gabi::guest_str("DungeonMapUp_00");
    key->__vtbl = gc_tu.safeString;
    hdMainRegisterScreen(sLayoutManager, key, heap);
    key->mStringTop = gabi::guest_str("DungeonMapDown_00");
    key->__vtbl = gc_tu.safeString;
    hdMainRegisterScreen(sLayoutManager, key, heap);
    key->mStringTop = gabi::guest_str("DungeonMapBoss_00");
    key->__vtbl = gc_tu.safeString;
    hdMainRegisterScreen(sLayoutManager, key, heap);
    key->mStringTop = gabi::guest_str("DungeonMapDoor_00");
    key->__vtbl = gc_tu.safeString;
    hdMainRegisterScreen(sLayoutManager, key, heap);
    key->mStringTop = gabi::guest_str("DungeonMapLink_00");
    key->__vtbl = gc_tu.safeString;
    hdMainRegisterScreen(sLayoutManager, key, heap);
    key->mStringTop = gabi::guest_str("DungeonMapMedley_00");
    key->__vtbl = gc_tu.safeString;
    hdMainRegisterScreen(sLayoutManager, key, heap);
    key->mStringTop = gabi::guest_str("DungeonMapMakore_00");
    key->__vtbl = gc_tu.safeString;
    hdMainRegisterScreen(sLayoutManager, key, heap);
    key->mStringTop = gabi::guest_str("DungeonMapShip_00");
    key->__vtbl = gc_tu.safeString;
    hdMainRegisterScreen(sLayoutManager, key, heap);
    key->mStringTop = gabi::guest_str("DungeonMapTreasure_00");
    key->__vtbl = gc_tu.safeString;
    hdMainRegisterScreen(sLayoutManager, key, heap);
    hdMainDestroyKey(key, 2);
    constructedKey = hdMainConstructKey(key, "DungeonMapStatue_00");
    hdMainRegisterScreen(sLayoutManager, constructedKey, heap);
    hdMainDestroyKey(key, 2);
    constructedKey = hdMainConstructKey(key, "MsgOnly_00");
    hdMainRegisterScreen(sLayoutManager, constructedKey, heap);
    hdMainDestroyKey(key, 2);
    constructedKey = hdMainConstructKey(key, "MsgWindowEvent_00");
    hdMainRegisterScreen(sLayoutManager, constructedKey, heap);
    hdMainDestroyKey(key, 2);
    constructedKey = hdMainConstructKey(key, "MsgWindowMain_00");
    hdMainRegisterScreen(sLayoutManager, constructedKey, heap);
    hdMainDestroyKey(key, 2);
    constructedKey = hdMainConstructKey(key, "MsgWindowScroll_00");
    hdMainRegisterScreen(sLayoutManager, constructedKey, heap);
    hdMainDestroyKey(key, 2);
    constructedKey = hdMainConstructKey(key, "ScrollKeyIcon_00");
    hdMainRegisterScreen(sLayoutManager, constructedKey, heap);
    hdMainDestroyKey(key, 2);
    constructedKey = hdMainConstructKey(key, "KeyIcon_00");
    hdMainRegisterScreen(sLayoutManager, constructedKey, heap);
    hdMainDestroyKey(key, 2);
    constructedKey = hdMainConstructKey(key, "MsgWindowEventFrame_00");
    hdMainRegisterScreen(sLayoutManager, constructedKey, heap);
    hdMainDestroyKey(key, 2);
    constructedKey = hdMainConstructKey(key, "BatonSignForWindow_00");
    hdMainRegisterScreen(sLayoutManager, constructedKey, heap);
    hdMainDestroyKey(key, 2);
    constructedKey = hdMainConstructKey(key, "SequenceWindow_00");
    hdMainRegisterScreen(sLayoutManager, constructedKey, heap);
    hdMainDestroyKey(key, 2);
    constructedKey = hdMainConstructKey(key, "BtnSequence_00");
    hdMainRegisterScreen(sLayoutManager, constructedKey, heap);
    hdMainDestroyKey(key, 2);
    constructedKey = hdMainConstructKey(key, "BtnSequence_01");
    hdMainRegisterScreen(sLayoutManager, constructedKey, heap);
    hdMainDestroyKey(key, 2);
    constructedKey = hdMainConstructKey(key, "KeyboardTextArea_00");
    hdMainRegisterScreen(sLayoutManager, constructedKey, heap);
    hdMainDestroyKey(key, 2);
    constructedKey = hdMainConstructKey(key, "KeyboardTextCursor");
    hdMainRegisterScreen(sLayoutManager, constructedKey, heap);
    hdMainDestroyKey(key, 2);
    constructedKey = hdMainConstructKey(key, "SequenceOption_00");
    hdMainRegisterScreen(sLayoutManager, constructedKey, heap);
    hdMainDestroyKey(key, 2);
    constructedKey = hdMainConstructKey(key, "BtnOption_00");
    hdMainRegisterScreen(sLayoutManager, constructedKey, heap);
    hdMainDestroyKey(key, 2);
    constructedKey = hdMainConstructKey(key, "MenuMiiverseContent_00");
    hdMainRegisterScreen(sLayoutManager, constructedKey, heap);
    hdMainDestroyKey(key, 2);
    constructedKey = hdMainConstructKey(key, "BtnMiiverseComment_00");
    hdMainRegisterScreen(sLayoutManager, constructedKey, heap);
    hdMainDestroyKey(key, 2);
    constructedKey = hdMainConstructKey(key, "BtnMiiverseYeah_00");
    hdMainRegisterScreen(sLayoutManager, constructedKey, heap);
    hdMainDestroyKey(key, 2);
    constructedKey = hdMainConstructKey(key, "BtnMiiversePicture_00");
    hdMainRegisterScreen(sLayoutManager, constructedKey, heap);
    hdMainDestroyKey(key, 2);
    constructedKey = hdMainConstructKey(key, "LoadingIcon_00");
    hdMainRegisterScreen(sLayoutManager, constructedKey, heap);
    hdMainDestroyKey(key, 2);
    constructedKey = hdMainConstructKey(key, "MenuMiiverseNewComment_00");
    hdMainRegisterScreen(sLayoutManager, constructedKey, heap);
    hdMainDestroyKey(key, 2);
    constructedKey = hdMainConstructKey(key, "BtnMiiverseNewComment_00");
    hdMainRegisterScreen(sLayoutManager, constructedKey, heap);
    hdMainDestroyKey(key, 2);
    constructedKey = hdMainConstructKey(key, "MenuMiiverseSetting_00");
    hdMainRegisterScreen(sLayoutManager, constructedKey, heap);
    hdMainDestroyKey(key, 2);
    constructedKey = hdMainConstructKey(key, "BtnMiiverseSetComment_00");
    hdMainRegisterScreen(sLayoutManager, constructedKey, heap);
    hdMainDestroyKey(key, 2);
    constructedKey = hdMainConstructKey(key, "ControlWind_00");
    hdMainRegisterScreen(sLayoutManager, constructedKey, heap);
    hdMainDestroyKey(key, 2);
    constructedKey = hdMainConstructKey(key, "ControlWindOK_00");
    hdMainRegisterScreen(sLayoutManager, constructedKey, heap);
    hdMainDestroyKey(key, 2);
    constructedKey = hdMainConstructKey(key, "BtnControlWind_00");
    hdMainRegisterScreen(sLayoutManager, constructedKey, heap);
    hdMainDestroyKey(key, 2);
    constructedKey = hdMainConstructKey(key, "CrawlingDirection_00");
    hdMainRegisterScreen(sLayoutManager, constructedKey, heap);
    hdMainDestroyKey(key, 2);
    constructedKey = hdMainConstructKey(key, "CrawlingDirectionArrow_00");
    hdMainRegisterScreen(sLayoutManager, constructedKey, heap);
    hdMainDestroyKey(key, 2);
    constructedKey = hdMainConstructKey(key, "Time_00");
    hdMainRegisterScreen(sLayoutManager, constructedKey, heap);
    hdMainDestroyKey(key, 2);
    constructedKey = hdMainConstructKey(key, "BossHP_00");
    hdMainRegisterScreen(sLayoutManager, constructedKey, heap);
    hdMainDestroyKey(key, 2);
    constructedKey = hdMainConstructKey(key, "EnemyHP_00");
    hdMainRegisterScreen(sLayoutManager, constructedKey, heap);
    hdMainDestroyKey(key, 2);
    constructedKey = hdMainConstructKey(key, "Arrow_00");
    hdMainRegisterScreen(sLayoutManager, constructedKey, heap);
    hdMainDestroyKey(key, 2);
    constructedKey = hdMainConstructKey(key, "Battery_00");
    hdMainRegisterScreen(sLayoutManager, constructedKey, heap);
    hdMainDestroyKey(key, 2);
    constructedKey = hdMainConstructKey(key, "RupySwordCounter_00");
    hdMainRegisterScreen(sLayoutManager, constructedKey, heap);
    hdMainDestroyKey(key, 2);
    constructedKey = hdMainConstructKey(key, "LoopBossIcon_00");
    hdMainRegisterScreen(sLayoutManager, constructedKey, heap);
    hdMainDestroyKey(key, 2);
    constructedKey = hdMainConstructKey(key, "Shortcut_00");
    hdMainRegisterScreen(sLayoutManager, constructedKey, heap);
    hdMainDestroyKey(key, 2);
    constructedKey = hdMainConstructKey(key, "Telescope_00");
    hdMainRegisterScreen(sLayoutManager, constructedKey, heap);
    hdMainDestroyKey(key, 2);
    constructedKey = hdMainConstructKey(key, "PictographBox_00");
    hdMainRegisterScreen(sLayoutManager, constructedKey, heap);
    hdMainDestroyKey(key, 2);
    constructedKey = hdMainConstructKey(key, "PictographList_00");
    hdMainRegisterScreen(sLayoutManager, constructedKey, heap);
    hdMainDestroyKey(key, 2);
    constructedKey = hdMainConstructKey(key, "BtnPictograph_00");
    hdMainRegisterScreen(sLayoutManager, constructedKey, heap);
    hdMainDestroyKey(key, 2);
    constructedKey = hdMainConstructKey(key, "BestShotMark_00");
    hdMainRegisterScreen(sLayoutManager, constructedKey, heap);
    hdMainDestroyKey(key, 2);
    constructedKey = hdMainConstructKey(key, "BatonInput_00");
    hdMainRegisterScreen(sLayoutManager, constructedKey, heap);
    hdMainDestroyKey(key, 2);
    constructedKey = hdMainConstructKey(key, "BatonInputPoint_00");
    hdMainRegisterScreen(sLayoutManager, constructedKey, heap);
    hdMainDestroyKey(key, 2);
    constructedKey = hdMainConstructKey(key, "BatonSign_00");
    hdMainRegisterScreen(sLayoutManager, constructedKey, heap);
    hdMainDestroyKey(key, 2);
    constructedKey = hdMainConstructKey(key, "BatonInputPoint_01");
    hdMainRegisterScreen(sLayoutManager, constructedKey, heap);
    hdMainDestroyKey(key, 2);
    constructedKey = hdMainConstructKey(key, "BatonSignList_00");
    hdMainRegisterScreen(sLayoutManager, constructedKey, heap);
    hdMainDestroyKey(key, 2);
    constructedKey = hdMainConstructKey(key, "FadeWipe_00");
    hdMainRegisterScreen(sLayoutManager, constructedKey, heap);
    hdMainDestroyKey(key, 2);
    constructedKey = hdMainConstructKey(key, "FadeWipe_01");
    hdMainRegisterScreen(sLayoutManager, constructedKey, heap);
    hdMainDestroyKey(key, 2);
    constructedKey = hdMainConstructKey(key, "PictographConfirm_00");
    hdMainRegisterScreen(sLayoutManager, constructedKey, heap);
    hdMainDestroyKey(key, 2);
    constructedKey = hdMainConstructKey(key, "Pictograph_00");
    hdMainRegisterScreen(sLayoutManager, constructedKey, heap);
    hdMainDestroyKey(key, 2);
    constructedKey = hdMainConstructKey(key, "Pause_00");
    hdMainRegisterScreen(sLayoutManager, constructedKey, heap);
    hdMainDestroyKey(key, 2);
    constructedKey = hdMainConstructKey(key, "BlurBG_00");
    hdMainRegisterScreen(sLayoutManager, constructedKey, heap);
    hdMainDestroyKey(key, 2);
    constructedKey = hdMainConstructKey(key, "NoOperateDRC_00");
    hdMainRegisterScreen(sLayoutManager, constructedKey, heap);
    hdMainDestroyKey(key, 2);
    constructedKey = hdMainConstructKey(key, "AuctionNumber_00");
    hdMainRegisterScreen(sLayoutManager, constructedKey, heap);
    hdMainDestroyKey(key, 2);
    constructedKey = hdMainConstructKey(key, "Auction_00");
    hdMainRegisterScreen(sLayoutManager, constructedKey, heap);
    hdMainDestroyKey(key, 2);
    constructedKey = hdMainConstructKey(key, "BattleshipGame_00");
    hdMainRegisterScreen(sLayoutManager, constructedKey, heap);
    key->mStringTop = gabi::guest_str("Battleship_00");
    key->__vtbl = gc_tu.safeString;
    hdMainRegisterScreen(sLayoutManager, key, heap);
    constructedKey = hdMainConstructKey(key, "SquidIcon_00");
    hdMainRegisterScreen(sLayoutManager, constructedKey, heap);
    constructedKey = hdMainConstructKey(key, "BoatRaceFailedChar_00");
    hdMainRegisterScreen(sLayoutManager, constructedKey, heap);
    key->mStringTop = gabi::guest_str("BoatRaceFailed_00");
    key->__vtbl = gc_tu.safeString;
    hdMainRegisterScreen(sLayoutManager, key, heap);
    hdMainDestroyKey(key, 2);
    key->mStringTop = gabi::guest_str("BoatRaceResult_00");
    key->__vtbl = gc_tu.safeString;
    hdMainRegisterScreen(sLayoutManager, key, heap);
    hdMainDestroyKey(key, 2);
    key->mStringTop = gabi::guest_str("BoatRaceStart_00");
    key->__vtbl = gc_tu.safeString;
    hdMainRegisterScreen(sLayoutManager, key, heap);
    hdMainDestroyKey(key, 2);
    key->mStringTop = gabi::guest_str("CannonGame_00");
    key->__vtbl = gc_tu.safeString;
    hdMainRegisterScreen(sLayoutManager, key, heap);
    hdMainDestroyKey(key, 2);
    key->mStringTop = gabi::guest_str("Cannonball_00");
    key->__vtbl = gc_tu.safeString;
    hdMainRegisterScreen(sLayoutManager, key, heap);
    hdMainDestroyKey(key, 2);
}
/* 0271AF5C */
static hdMainScreenManager_c* constructMainScreenManager(hdMainScreenManager_c* manager) {
    if (manager == NULL) {
        manager = (hdMainScreenManager_c*)operator_new(sizeof(hdMainScreenManager_c));
        if (manager == NULL) return NULL;
    }
    manager->mReady = 0;
    manager->mVtable = gabi::ea(&sManagerVtable);
    manager->mActive = 0;
    return manager;
}
/* 0271AFA8 */
static hdMainScreenManager_c* createMainScreenManager(void* heap) {
    hdMainScreenManager_c* manager = sScreenManager;
    if (manager == NULL) {
        manager = (hdMainScreenManager_c*)hdMainAllocate(sizeof(hdMainScreenManager_c), heap, 4);
        // GHS adds the embedded disposer offset in the 32-bit guest address space.
        u32 disposerAddress = gabi::ea(manager) + offsetof(hdMainScreenManager_c, mAllocator);
        hdMainDisposer_l* disposer = gabi::at<hdMainDisposer_l>(disposerAddress);
        if (disposer != NULL) {
            hdMainConstructDisposer(disposer, heap, 3);
            disposer->mVtable = gabi::ea(&sDisposerVtable);
        }
        sScreenDisposer = disposer;
        if (manager != NULL) manager = constructMainScreenManager(manager);
        sScreenManager = manager;
    }
    return manager;
}
/* 0271B048 */
static void destroyMainScreenManager(hdMainScreenManager_c* manager, s32 flags) {
    if (manager == NULL) return;
    manager->mVtable = gabi::ea(&sManagerVtable);
    if (flags & 1) operator_delete(manager);
}
/* 0271B068 */
static void destroyMainScreenDisposer(hdMainDisposer_l* disposer, s32 flags) {
    if (disposer == NULL) return;
    disposer->mVtable = gabi::ea(&sDisposerVtable);
    if (disposer == sScreenDisposer) {
        hdMainScreenManager_c* manager = sScreenManager;
        sScreenDisposer = NULL;
        destroyMainScreenManager(manager, 2);
        sScreenManager = NULL;
    }
    hdMainDestroyDisposer(disposer, 0);
    if (flags & 1) operator_delete(disposer);
}
struct hdMainGlobalRecord_l { be<u32> mWords[3]; };
struct hdMainStaticList_l { be<u32> mHead, mTail, mCount, mReserved; };
static hdMainStaticList_l sStaticList;
static hdMainGlobalRecord_l sListRecord;
static hdMainGlobalRecord_l sRandomRecord;
static hdMainGlobalRecord_l sThreadRecord;
static u32 sMinAngle;
static u32 sMaxAngle;
static u8 sRandomStatic;
static u8 sThreadStatic;
/* 0271B104 */
static void initializeMainScreenStatics() {
    sStaticList.mCount = 0;
    sStaticList.mHead = 0;
    sStaticList.mReserved = 0;
    sStaticList.mTail = 0;
    hdMainRegisterGlobal(&sListRecord);
    sMinAngle = 0xC0490FDB;
    sMaxAngle = 0x40490FDB;
    hdMainInitializeRandom(&sRandomStatic);
    hdMainRegisterGlobal(&sRandomRecord);
    hdMainInitializeThread(&sThreadStatic);
    hdMainRegisterGlobal(&sThreadRecord);
}
/* WWHD glue (tools/verify/autoglue.py): main screens, local bindings kept for integration
 data sLayoutManager 101F7274
 data sResourceManager 101F4B5C
 data sScreenManager 101F8378
 data sScreenDisposer 101F837C
 data sManagerVtable 1013F740
 data sDisposerVtable 10140218
 data sStaticList 1049F528
 data sListRecord 101F8354
 data sRandomRecord 101F8360
 data sThreadRecord 101F836C
 data sMinAngle 1049F51C
 data sMaxAngle 1049F520
 data sRandomStatic 1049F524
 data sThreadStatic 1049F525
 ext 025F77B0 void hdMainRefreshResources(void*)
 ext 026FB694 s32 hdMainRegisterScreen(hdMainLayoutMgr_l*, SafeString*, void*)
 ext 0271B1AC SafeString* hdMainConstructKey(SafeString*, const char*)
 ext 0271B198 void hdMainDestroyKey(SafeString*, s32)
 ext 0273B0D4 void* hdMainAllocate(u32, void*, s32)
 ext 02752B0C void hdMainConstructDisposer(hdMainDisposer_l*, void*, s32)
 ext 02752BEC void hdMainDestroyDisposer(hdMainDisposer_l*, s32)
 ext 028F026C void hdMainRegisterGlobal(hdMainGlobalRecord_l*)
 ext 028ED6F8 void hdMainInitializeRandom(void*)
 ext 028EAB2C void hdMainInitializeThread(void*)
 str 1013F750 "Main_00"
 str 1013F818 "HeartAll_00"
 str 1013FC7C "Heart_00"
 str 1013F758 "Rupy_00"
 str 1013F824 "MagicBar_00"
 str 1013F930 "DungeonKey_00"
 str 1013F940 "UnreadMiiverse_00"
 str 1013F778 "CompassClock_00"
 str 1013F830 "SwimTime_00"
 str 1013F83C "MenuItem_00"
 str 1013FC88 "BtnItemIcon_00"
 str 1013F788 "BtnMenuArrow_00"
 str 1013FC98 "BtnMenuSaveOption_00"
 str 1013FCB0 "BtnBack_00"
 str 1013F954 "BtnCollectIcon_00"
 str 1013F968 "BtnCollectIconMask_00"
 str 1013F980 "Collection_00"
 str 1013F798 "BtnMenuHeart_00"
 str 1013F990 "BtnMapIcon_00"
 str 1013FCBC "IconSlideArea_00"
 str 1013F848 "BtnPearl_00"
 str 1013FCD0 "BtnTriforce_00"
 str 1013FCE0 "SlideAssistTV_00"
 str 1013F7A8 "MenuMiiverse_00"
 str 1013F9A0 "BtnMiiverseComment_00"
 str 1013FCF4 "BtnMiiverseYeah_00"
 str 1013F9B8 "BtnMiiversePicture_00"
 str 1013FD08 "LoadingIcon_00"
 str 1013F854 "MenuBtns_00"
 str 1013FD18 "BtnMenu_00"
 str 1013F9D0 "Cursor_00"
 str 1013F9DC "Cursor_01"
 str 1013F9E8 "Cursor_02"
 str 1013F7B8 "CommandGuide_00"
 str 1013F860 "CommandA_00"
 str 1013F86C "GameOver_00"
 str 1013F7C8 "GameOverChar_00"
 str 1013FD24 "PlaceName_00"
 str 1013FD34 "Information_00"
 str 1013F9F4 "InformationDRC_00"
 str 1013FD44 "MenuMap_00"
 str 1013FA08 "MapAreaNow_00"
 str 1013FA18 "BtnMapIcon_01"
 str 1013FD50 "BtnMapBalloon_00"
 str 1013FA28 "BtnFishMan_00"
 str 1013FD64 "LinkPos_00"
 str 1013FA38 "TreMap_00"
 str 1013F770 "Map_00"
 str 1013FD70 "WarpMap_00"
 str 1013F878 "WarpArea_00"
 str 1013FD7C "TincleTower_00"
 str 1013FD8C "Fairy_00"
 str 1013FD98 "HeartIsland_00"
 str 1013FDA8 "Squid_00"
 str 1013F884 "Triforce_00"
 str 1013FDB4 "Terry_00"
 str 1013FA44 "SecretEntrance_00"
 str 1013FDC0 "Submarine_00"
 str 1013FA58 "EnemyTower_00"
 str 1013F760 "Moon_00"
 str 1013F890 "HeartSea_00"
 str 1013FDD0 "GhostShip_00"
 str 1013FA68 "DungeonMap_00"
 str 1013F89C "BtnFloor_00"
 str 1013FA78 "BtnDungeonItemIcon_00"
 str 1013FDE0 "Searchlight_00"
 str 1013FDF0 "DungeonMapFloor_00"
 str 1013FE04 "DungeonMapFloor_01"
 str 1013FE18 "DungeonMapFloor_02"
 str 1013FE2C "DungeonMapFloor_03"
 str 1013FE40 "DungeonMapFloor_04"
 str 1013FE54 "DungeonMapFloor_05"
 str 1013F7D8 "DungeonMapUp_00"
 str 1013FA90 "DungeonMapDown_00"
 str 1013FAA4 "DungeonMapBoss_00"
 str 1013FAB8 "DungeonMapDoor_00"
 str 1013FACC "DungeonMapLink_00"
 str 1013F8A8 "DungeonMapMedley_00"
 str 1013F8BC "DungeonMapMakore_00"
 str 1013FAE0 "DungeonMapShip_00"
 str 1013FAF4 "DungeonMapTreasure_00"
 str 1013F8D0 "DungeonMapStatue_00"
 str 1013FE68 "MsgOnly_00"
 str 1013FB0C "MsgWindowEvent_00"
 str 1013FE74 "MsgWindowMain_00"
 str 1013FE88 "MsgWindowScroll_00"
 str 1013FE9C "ScrollKeyIcon_00"
 str 1013FEB0 "KeyIcon_00"
 str 1013FEBC "MsgWindowEventFrame_00"
 str 1013FB20 "BatonSignForWindow_00"
 str 1013FB38 "SequenceWindow_00"
 str 1013FED4 "BtnSequence_00"
 str 1013FEE4 "BtnSequence_01"
 str 1013F8E4 "KeyboardTextArea_00"
 str 1013FEF4 "KeyboardTextCursor"
 str 1013FB4C "SequenceOption_00"
 str 1013FF08 "BtnOption_00"
 str 1013FF18 "MenuMiiverseContent_00"
 str 1013FB60 "MenuMiiverseNewComment_00"
 str 1013FF30 "BtnMiiverseNewComment_00"
 str 1013FF4C "MenuMiiverseSetting_00"
 str 1013FF64 "BtnMiiverseSetComment_00"
 str 1013FF80 "ControlWind_00"
 str 1013FF90 "ControlWindOK_00"
 str 1013FB7C "BtnControlWind_00"
 str 1013FFA4 "CrawlingDirection_00"
 str 1013FB90 "CrawlingDirectionArrow_00"
 str 1013F768 "Time_00"
 str 1013FBAC "BossHP_00"
 str 1013FFBC "EnemyHP_00"
 str 1013FFC8 "Arrow_00"
 str 1013FFD4 "Battery_00"
 str 1013F8F8 "RupySwordCounter_00"
 str 1013F7E8 "LoopBossIcon_00"
 str 1013F90C "Shortcut_00"
 str 1013FFE0 "Telescope_00"
 str 1013FFF0 "PictographBox_00"
 str 1013FBB8 "PictographList_00"
 str 10140004 "BtnPictograph_00"
 str 1013F7F8 "BestShotMark_00"
 str 1013FBCC "BatonInput_00"
 str 10140018 "BatonInputPoint_00"
 str 1014002C "BatonSign_00"
 str 1014003C "BatonInputPoint_01"
 str 10140050 "BatonSignList_00"
 str 1013F918 "FadeWipe_00"
 str 1013F924 "FadeWipe_01"
 str 10140064 "PictographConfirm_00"
 str 1013FBDC "Pictograph_00"
 str 1014007C "Pause_00"
 str 1013FBEC "BlurBG_00"
 str 1013F808 "NoOperateDRC_00"
 str 10140088 "AuctionNumber_00"
 str 1014009C "Auction_00"
 str 1013FBF8 "BattleshipGame_00"
 str 1013FC0C "Battleship_00"
 str 101400A8 "SquidIcon_00"
 str 1013FC1C "BoatRaceFailedChar_00"
 str 1013FC34 "BoatRaceFailed_00"
 str 1013FC48 "BoatRaceResult_00"
 str 101400B8 "BoatRaceStart_00"
 str 1013FC5C "CannonGame_00"
 str 1013FC6C "Cannonball_00"
*/
