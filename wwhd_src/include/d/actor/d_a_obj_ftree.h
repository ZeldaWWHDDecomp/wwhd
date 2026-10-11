#pragma once
#include "gc_api/gc_api.h"
namespace daObjFtree {
struct _ftree_seach_info_;
enum Mode : u32 {
    MODE_NONE = 0,
    MODE_WAIT_S = 1,
    MODE_WAIT_M = 2,
    MODE_WAIT_L = 3,
    MODE_CHANGE_SL = 4,
    MODE_CHANGE_SL2 = 5,
    MODE_CHANGE_LS = 6,
    MODE_CHANGE_LS2 = 7,
    MODE_CHANGE_SM = 8,
    MODE_CHANGE_MS = 9,
    MODE_PIKUPIKU_S = 10,
    MODE_PIKUPIKU_M = 11,
    MODE_PIKUPIKU_L = 12,
    MODE_COUNT = 13
};
struct Act_c : fopAc_ac_c {
    void XyEvent_init();
    u32 XyEvent_set(s32 event);
    u32 is_brought();
    void get_ftree_info(daObjFtree::_ftree_seach_info_* info);
    u32 iam_last();
    void unset_broughtID(s32 id);
    void set_broughtID(s32 id);
    void set_brought();
    void unset_brought();
    u32 _delete();
    u32 PlayStopJointAnimation();
    u32 PlayStopColorAnimation();
    u32 action_none_init(s32 parameter);
    u32 param_get_tree_idx(u32 width, u32 shift);
    u32 process_init(u32 mode, s32 parameter);
    void process_main();
    void action_changeMS_main();
    void action_changeSM_main();
    void action_changeLS2_main();
    void action_changeSL_main();
    s32 XyEventCallBack(s32 index);
    u32 action_pikupikuS_init(s32 repetitions);
    void action_pikupikuS_main();
    u32 action_pikupikuM_init(s32 duration);
    u32 action_pikupikuL_init(s32 duration);
    void action_pikupikuM_main();
    void action_pikupikuL_main();
    u32 action_waitL_init(s32 parameter);
    u32 action_changeLS_init(s32 parameter);
    u32 action_changeSM_init(s32 parameter);
    u32 action_changeMS_init(s32 parameter);
    void action_waitL_main();
    u32 SetJointAnimation(s32 animation, f32 rate, f32 morph, s32 endOffset);
    u32 NodeCallBack_Effect(void* node, s32 phase);
    u32 _execute();
    u32 estimate_water();
    void talk_ct();
    void XyEvent_exe();
    void set_first_stat();
    void set_mtx();
    u32 action_waitS_init(s32 parameter);
    u32 action_waitM_init(s32 parameter);
    void action_waitS_main();
    void action_waitM_main();
    u32 launch_heart_part();
    void talk_main();
    u32 _draw();
    u32 action_changeSL2_init(s32 parameter);
    u32 action_changeLS2_init(s32 parameter);
    u32 action_changeSL_init(s32 parameter);
    void get_tev_material0_color(void* modelData, u32 colorSlot, be<s16>* red, be<s16>* green, be<s16>* blue);
    void set_tev_color(void* tableObject, s32 colorIndex, s32 red, s32 green, s32 blue);
    void set_collision();
    u32 place_heart_part();
    void action_changeSL2_main();
    void action_changeLS_main();
    u32 create_heap();
    u32 _create();

    be<s16> m294, m296, m298, m29A;
    request_of_phase_process_class mPhs;
    be<s16> effectPending;
    be<u8> smallVisible, largeVisible;
    Mtx34 m2A8;                                                      // 0x3C0
    be<u32> mpMorf, mpModel;                                         // 0x3F0, 0x3F4
    u8 mBrkAnm[0x78];                                                // 0x3F8
    dBgS_GndChk mGndChk;                                             // 0x470
    be<f32> m34C;                                                    // 0x4C4
    be<s16> mEventCam0Idx, mEventCam1Idx, mEventCam2Idx, m356, m358; // 0x4C8
    u8 _4D2[2];
    dCcD_Stts m35C;                                    // 0x4D4
    dCcD_Cyl m398;                                     // 0x510
    be<f32> m4C8;                                      // 0x640
    dCcD_Stts m4CC;                                    // 0x644
    dCcD_Cyl m508;                                     // 0x680
    be<u32> brought;                                   // 0x7B0
    be<f32> largeScale, colorBlend;                    // 0x7B4, 0x7B8
    be<u32> treeId;                                    // 0x7BC
    be<s16> heartPlaced, heartLaunchTimer, eventTimer; // 0x7C0
    be<s16> m654[4], cachedSmallColor[4], m664[4];
    be<s16> m66C[4], m674[4]; // 0x7DE, 0x7E6
    u8 _7EE[2];
    be<u32> previousCollision, mMode; // 0x7F0, 0x7F4
    be<s16> colorDelay;
    u8 _7FA[2];
    be<s32> repetitions, completedRepetitions; // 0x7FC, 0x800
    be<s16> mediumDuration, mediumElapsed, mediumPhase, m696, m698;
    be<s16> largeDuration, largeElapsed, largePhase, m6A0, m6A2;
    be<u32> messageId; // 0x818
    u8 _81C[4];
};
struct _ftree_seach_info_ {
    be<u32> total, brought;
};
} // namespace daObjFtree
WWHD_SIZE(daObjFtree::Act_c, 0x820);
static_assert(offsetof(daObjFtree::Act_c, mPhs) == 0x3B4);
static_assert(offsetof(daObjFtree::Act_c, effectPending) == 0x3BC);
static_assert(offsetof(daObjFtree::Act_c, m2A8) == 0x3C0);
static_assert(offsetof(daObjFtree::Act_c, mpMorf) == 0x3F0);
static_assert(offsetof(daObjFtree::Act_c, m398) == 0x510);
static_assert(offsetof(daObjFtree::Act_c, m674) == 0x7E6);
static_assert(offsetof(daObjFtree::Act_c, mMode) == 0x7F4);
static_assert(offsetof(daObjFtree::Act_c, messageId) == 0x818);

struct FtreeVectorWords {
    be<u32> x, y, z;
};
WWHD_SIZE(FtreeVectorWords, 12);
inline FtreeVectorWords& ftree_words(cXyz* value) {
    return gabi::object_at<FtreeVectorWords>(gabi::ea(value));
}
struct FtreeShapeAttr {
    u8 prefix[0x18];
    be<u32> shapeVtable, cylinderVtable;
};
WWHD_OFFSET(FtreeShapeAttr, shapeVtable, 0x18);
WWHD_SIZE(FtreeShapeAttr, 0x20);

struct FtreeColorNameBuffer {
    be<u32> string, vtable, size;
    be<u8> characters[32];
};
struct FtreeFloatColor {
    be<f32> r, g, b, a;
};

// Exact verified argument widths at external call boundaries.
unsigned int ftree_getPlay();
int ftree_dSv_event_c__getEventReg(void*, int);
int ftree_dSv_event_c__isEventBit(void*, int);
unsigned int ftree_daObjFtree__is_broughtID(unsigned int);
int ftree_daObjFtree__is_broughtID_2(unsigned int);
unsigned int ftree_dSv_event_c__getEventReg_2(void*, int);
void ftree_dSv_event_c__setEventReg(void*, int, unsigned int);
void ftree_dSv_event_c__setEventReg_2(void*, int, unsigned char);
void ftree_dSv_event_c__onEventBit(void*, int);
void ftree_dComIfG_resDelete(void*, const char*);
unsigned int ftree_mDoExt_McaMorf__play(void*, int, int, int);
unsigned int ftree_mDoExt_baseAnm__play(void*);
void ftree_dl(void*);
void ftree_dBgS_Acch___dBgS_Acch(void*, int);
void ftree_destroyCollisionStatus(void*, int);
void ftree_cBgS_Chk___cBgS_Chk(void*, int);
void ftree_fopAc_ac_c___fopAc_ac_c(daObjFtree::Act_c*, int);
void ftree_dl_2(daObjFtree::Act_c*);
float ftree_cM_rndF(float);
void ftree_playTreeSound(int, void*);
void ftree_register_global_object(void*);
void ftree_initCommonByteA(void*);
void ftree_initCommonByteB(void*);
int ftree_dKy_rain_check();
unsigned int ftree_dRes_control_c__getRes(void*, SafeString*, int);
void ftree_mDoExt_McaMorf__setAnm(void*, void*, int, float, float, float, float, int);
unsigned int ftree_getJoint(void*);
void ftree_PSMTXCopy(void*, void*);
void ftree_PSMTXCopy_2(void*, Mtx34*);
void ftree_PSMTXMultVec(Mtx34*, cXyz*, cXyz*);
unsigned int ftree_dPa_control_c__set(void*, int, int, cXyz*, int, int, int, int, int, int, int, int);
void ftree_csXyz__csXyz(csXyz*, short, short, int);
void ftree_mDoMtx_ZXYrotM(void*, short, short, short);
void ftree_csXyz__csXyz_2(csXyz*, int, short, short);
void ftree_fopAcM_rollPlayerCrash(daObjFtree::Act_c*, int, float);
void ftree_cLib_addCalcAngleS2(void*, short, int, int);
void ftree_getPlay_2();
int ftree_cM_atan2s(float, float);
void ftree_PSMTXTrans(void*, float, float, float);
void ftree_mDoMtx_YrotM(void*, short);
float ftree_PSVECSquareDistance(cXyz*, void*);
int ftree_dEvent_manager_c__endCheck(void*, short);
int ftree_dSv_player_collect_c__isSymbol(void*, int);
void ftree_JUTAssertion__getSDevice(const char*, int, const char*);
void ftree_J3DModel__calc(void*);
unsigned int ftree_initBrkAnimation(void*, void*, void*, int, int, int, int, int, float, int);
unsigned int ftree_dCcD_GObjInf__ChkCoHit(void*);
unsigned int ftree_dSv_player_item_record_c__getTimer(void*);
unsigned int ftree_fopAcM_fastCreateItem(cXyz*, int, signed char, csXyz*, cXyz*, int, int, float, float,
                                         float);
unsigned int ftree_fopMsgM_messageSet(void*, unsigned int, void*);
unsigned int ftree_fopMsgM_SearchByID(void*);
void ftree_setMessageStatus(void*, int);
float ftree_PSVECSquareDistance_2(void*, void*);
void ftree_cXyz____mi(void*, cXyz*, void*);
unsigned int ftree_dScnKy_env_light_c__setLightTevColorType();
void ftree_dScnKy_env_light_c__settingTevStruct(void*, int, void*, void*);
void ftree_setLightTevColor(void*, void*, void*);
void ftree_mDoExt_brkAnm__entry(void*, void*, float);
void ftree_mDoExt_McaMorf__updateDL(void*);
void ftree_mDoExt_modelUpdateDL(void*, int);
void ftree_formatColorName(FtreeColorNameBuffer*, const char*, unsigned int);
int ftree_findMaterialParameter(void*, void*);
void ftree_convertMaterialRgb(FtreeFloatColor*, FtreeFloatColor*, float);
unsigned int ftree_getMaterialColorStorage(void*, unsigned int);
int ftree_dCcD_GObjInf__ChkTgHit(void*);
void ftree_daObj__HitSeStart(void*, signed char, void*, int);
void ftree_daObj__HitEff_kikuzu(daObjFtree::Act_c*, void*);
void ftree_dKy_Sound_set(cXyz*, int, unsigned int, int);
void ftree_dCcD_GObjInf__ClrTgHit(void*);
void ftree_cM3dGCyl__SetR(void*, float);
void ftree_cM3dGCyl__SetH(void*, float);
void ftree_cM3dGCyl__SetC(void*, cXyz*);
void ftree_cCcS__Set(void*, void*);
void ftree_cM3dGCyl__SetC_2(void*, void*);
unsigned int ftree_dSv_event_c__isEventBit_2(void*, int);
void ftree_csXyz__csXyz_3(csXyz*, int, int, int);
void ftree_cLib_addCalc(void*, float, float, float, float);
unsigned int ftree_mDoExt_McaMorf__mDoExt_McaMorf(int, void*, int, int, void*, int, int, int, float, int, int,
                                                  int, int);
unsigned int ftree_mDoExt_J3DModel__create(void*, int, int);
void ftree_fopAc_ac_c__fopAc_ac_c(daObjFtree::Act_c*);
void ftree_mDoExt_brkAnm__init(void*);
void ftree_cBgS_GndChk__cBgS_GndChk(void*);
void ftree_constructCollisionStatus(void*);
void ftree_dCcD_GStts__dCcD_GStts(void*);
void ftree_dCcD_GObjInf__dCcD_GObjInf(void*);
void ftree_constructCylinder(void*);
unsigned int ftree_dComIfG_resLoad(void*, const char*);
unsigned int ftree_fopAcM_entrySolidHeap(daObjFtree::Act_c*, int, int);
float ftree_cBgS__GroundCross(void*, void*);
void ftree_fopAcM_setCullSizeBox(daObjFtree::Act_c*, float, float, float, float, float, float);
void ftree_dCcD_Stts__Init(void*, int, int, daObjFtree::Act_c*);
void ftree_dCcD_Cyl__Set(void*, void*);
int ftree_dEvent_manager_c__getEventIdx(void*, const char*, int);

inline be<u32>& ftree_treeMapInitialized() { return gabi::object_at<be<u32>>(0x101FDBD0); }

inline be<u8>& ftree_treeMapSize() { return gabi::object_at<be<u8>>(0x101FEC14); }

inline be<u32>& ftree_saveData() { return gabi::object_at<be<u32>>(0x101F84DC); }

inline be<u32>& ftree_resourceControl() { return gabi::object_at<be<u32>>(0x101F4F28); }

inline be<u32>& ftree_j3dSystem() { return gabi::object_at<be<u32>>(0x104B462C); }

inline be<u32>& ftree_messageObject() { return gabi::object_at<be<u32>>(0x101F4B5C); }

inline be<u32>& ftree_opaqueDrawBuffer() { return gabi::object_at<be<u32>>(0x104B4634); }

inline be<u32>& ftree_translucentDrawBuffer() { return gabi::object_at<be<u32>>(0x104B4638); }

inline be<u32>& ftree_targetVectorX() { return gabi::object_at<be<u32>>(0x101FFBA8); }

inline be<u32>& ftree_targetVectorY() { return gabi::object_at<be<u32>>(0x101FFBAC); }

inline be<u32>& ftree_targetVectorZ() { return gabi::object_at<be<u32>>(0x101FFBB0); }

inline be<f32>& ftree_smallIdleRateRange() { return gabi::object_at<be<f32>>(0x1002953C); }

inline be<f32>& ftree_smallIdleRateBase() { return gabi::object_at<be<f32>>(0x10029540); }

inline be<f32>& ftree_smallRepeatRateRange() { return gabi::object_at<be<f32>>(0x10029544); }

inline be<f32>& ftree_hioMinimum() { return gabi::object_at<be<f32>>(0x10029548); }

inline be<f32>& ftree_hioMaximum() { return gabi::object_at<be<f32>>(0x1002954C); }

inline be<f32>& ftree_minimumScale() { return gabi::object_at<be<f32>>(0x100292C8); }

inline be<f32>& ftree_shrinkAnimationRate() { return gabi::object_at<be<f32>>(0x100294D4); }

inline be<f32>& ftree_colorBlendStep() { return gabi::object_at<be<f32>>(0x10029528); }

inline be<f32>& ftree_scaleBlendRange() { return gabi::object_at<be<f32>>(0x1002952C); }

inline be<f32>& ftree_scaleApproachRate() { return gabi::object_at<be<f32>>(0x10029530); }

inline be<f32>& ftree_scaleMinimumStep() { return gabi::object_at<be<f32>>(0x10029534); }

inline be<u32>& ftree_hioWord10469BD0() { return gabi::object_at<be<u32>>(0x10469BD0); }

inline be<u32>& ftree_hioWord10469BC8() { return gabi::object_at<be<u32>>(0x10469BC8); }

inline be<u32>& ftree_hioWord10469BD4() { return gabi::object_at<be<u32>>(0x10469BD4); }

inline be<u32>& ftree_hioWord10469BCC() { return gabi::object_at<be<u32>>(0x10469BCC); }

inline be<f32>& ftree_hioFloat10469BBC() { return gabi::object_at<be<f32>>(0x10469BBC); }

inline be<f32>& ftree_hioFloat10469BC0() { return gabi::object_at<be<f32>>(0x10469BC0); }

// HD: play-object offsets used by this unit differ from the common GameCube prefix.
struct FtreePlayView {
    u8 prefix[0x52B8];
    be<u16> eventFlags;
    u8 padding52BA[0x5AB0 - 0x52BA];
    be<u32> particleControl;
    u8 padding5AB4[0x5B2C - 0x5AB4];
    be<u32> player;
    u8 padding5B30[0x5BBB - 0x5B30];
    be<u8> eventState[0x5D70 - 0x5BBB];
    be<u32> opaqueList, translucentList, defaultOpaqueList, defaultTranslucentList;
};
WWHD_OFFSET(FtreePlayView, player, 0x5B2C);
inline FtreePlayView& ftree_play(u32 address) { return gabi::object_at<FtreePlayView>(address); }
inline be<u8>& ftree_treeId(u32 index) { return gabi::object_at<be<u8>>(0x10029154 + index); }
inline be<u32>& ftree_message(u32 mode) { return gabi::object_at<be<u32>>(0x101C96C8 + mode * 4); }
inline Mtx34& ftree_workMatrix() { return gabi::object_at<Mtx34>(0x1048D0CC); }

// HD: rendering views retain the SDK pointer indirections and store order.
struct FtreeMorphView {
    u8 prefix[0x90];
    be<u32> model;
    u8 padding94[8];
    be<f32> frame;
    u8 paddingA0[2];
    be<s16> endFrame;
};
WWHD_OFFSET(FtreeMorphView, frame, 0x9C);
struct FtreeModelView {
    u8 prefix[0x2C];
    be<u32> matrixBuffer;
    u8 padding30[4];
    be<u32> materialEntries;
    u8 padding38[0xAC - 0x38];
    be<u32> modelData;
    u8 paddingB0[8];
    be<u32> userArea;
    cXyz baseScale;
    Mtx34 baseTransform;
};
WWHD_OFFSET(FtreeModelView, baseTransform, 0xC8);
inline FtreeModelView& ftree_model(u32 address) { return gabi::object_at<FtreeModelView>(address); }
inline FtreeMorphView& ftree_morph(u32 address) { return gabi::object_at<FtreeMorphView>(address); }
inline J3DMtxBuffer_hd& ftree_matrixBuffer(u32 address) { return gabi::object_at<J3DMtxBuffer_hd>(address); }
inline J3DModelData& ftree_modelData(u32 address) { return gabi::object_at<J3DModelData>(address); }
inline daObjFtree::Act_c& ftree_actor(u32 address) { return gabi::object_at<daObjFtree::Act_c>(address); }
inline fopAc_ac_c& ftree_baseActor(u32 address) { return gabi::object_at<fopAc_ac_c>(address); }

struct FtreeActionRecord {
    be<s16> adjustment, slot;
    union {
        be<u32> target;
        struct {
            be<s16> reserved, vtableOffset;
        } indirect;
    };
};
WWHD_SIZE(FtreeActionRecord, 8);
struct FtreeVtableEntry {
    be<u32> adjustment, target;
};
struct FtreeResource {
    be<u32> reserved, vtable;
};
struct FtreeResourceVtable {
    u8 prefix[0x14];
    be<u32> getEndFrame;
};
struct FtreePlayerVtable {
    u8 prefix[0x114];
    be<u32> prepareTalk;
};
struct FtreeJointView {
    u8 prefix[4];
    be<u16> index;
    u8 padding6[2];
    be<u32> callback;
};
struct FtreeMaterialRoot {
    u8 prefix[8];
    be<u32> listObject;
};
struct FtreeMaterialList {
    u8 prefix[0x10];
    be<u32> material;
};
struct FtreeMaterial {
    be<u32> description, next;
    u8 padding8[0x18 - 8];
    be<u32> colorBlock;
    u8 padding1C[0xA0 - 0x1C];
    be<u32> colorFlags;
};
struct FtreeMaterialDescription {
    u8 prefix[0xC];
    be<u16> index;
};
struct FtreeMaterialEntry {
    be<u32> resource;
    u8 prefix4[0x28 - 4];
    be<u32> colors;
    u8 tail[0x3C - 0x2C];
};
WWHD_SIZE(FtreeMaterialEntry, 0x3C);
struct FtreeMaterialResource {
    u8 prefix[0x34];
    be<u32> colorOffset, nameOffset;
};
struct FtreeColorEntry {
    be<u16> type, offset;
    u8 tail[16];
};
WWHD_SIZE(FtreeColorEntry, 20);
struct FtreeColorBlock {
    be<u32> reserved, vtable;
};
struct FtreeColorBlockVtable {
    u8 prefix[0x24];
    be<u32> setColor;
    u8 padding28[12];
    be<u32> getColor;
};
struct FtreeModelDataFlags {
    u8 prefix[0x48];
    be<u32> materialFlags;
};
struct FtreeSinCos {
    be<f32> sine, cosine;
};

inline Mtx34& ftree_jointWorkMatrix() { return gabi::object_at<Mtx34>(0x104B4868); }

inline void* ftree_resourceManager() { return gabi::at<void>(static_cast<u32>(ftree_resourceControl())); }

inline void* ftree_eventManager(u32 play) { return gabi::at<void>(play + 0x52C4); }

inline void* ftree_collisionServer(u32 play) { return gabi::at<void>(play + 0x26A4); }

inline void* ftree_backgroundServer(u32 play) { return gabi::at<void>(play + 0x12A0); }

inline void* ftree_saveCollect() { return gabi::at<void>(static_cast<u32>(ftree_saveData()) + 0xD4); }

inline void* ftree_saveItemRecord() { return gabi::at<void>(static_cast<u32>(ftree_saveData()) + 0x86); }

inline void* ftree_messagePointer(u32 messageObject) { return gabi::at<void>(messageObject); }

inline void* ftree_materialNameTable(u32 table) { return gabi::at<void>(table); }

inline void* ftree_formattedName(u32 address) { return gabi::at<void>(address); }

inline void* ftree_jointAnimation(u32 animation) { return gabi::at<void>(animation); }

inline void* ftree_hioDestructorRecordA() { return gabi::at<void>(0x101C9654); }

inline void* ftree_hioDestructorRecordB() { return gabi::at<void>(0x101C9660); }

inline void* ftree_hioDestructorRecordC() { return gabi::at<void>(0x101C966C); }

inline void* ftree_hioByteA() { return gabi::at<void>(0x10469BC4); }

inline void* ftree_hioByteB() { return gabi::at<void>(0x10469BC5); }

u32 ftree_nodeEffectCallback(void* node, s32 phase);

u32 ftree_nodeMediumCallback(void* node, s32 phase);

u32 ftree_nodeLargeCallback(void* node, s32 phase);

u32 ftree_heapCallback(daObjFtree::Act_c* actor);

u32 ftree_checkEvent(daObjFtree::Act_c* actor, s32 index);

s32 ftree_eventCallback(daObjFtree::Act_c* actor, s32 event);

// HD: this translation unit owns these SDK vtable instances.
namespace ftree_tu {
inline constexpr u32 archiveName = 0x10029550;
inline constexpr u32 safeString = 0x10029044;
inline constexpr u32 bufferedSafeString = 0x1002908C;
inline constexpr u32 sinCosTable = 0x104A44F8;
inline constexpr u32 actor = 0x10029144;
inline constexpr u32 groundCheck = 0x10029104;
inline constexpr u32 polyPassCheck = 0x10029134;
inline constexpr u32 polyInfo = 0x10029114;
inline constexpr u32 groupPassCheck = 0x10029124;
inline constexpr u32 collisionStatus = 0x1004AE88;
inline constexpr u32 globalCollisionStatus = 0x1004AEC0;
inline constexpr u32 baseCylinder = 0x100015A8;
inline constexpr u32 shapeAttribute = 0x100290A4;
inline constexpr u32 collisionHitInfo = 0x1004B108;
inline constexpr u32 collisionCylinder = 0x1004B160;
inline constexpr u32 cylinder = 0x1004B150;
inline constexpr u32 destroyPolyInfo = 0x100290D4;
inline constexpr u32 destroyPolyPassCheck = 0x100290F4;
inline constexpr u32 destroyGroupPassCheck = 0x100290B4;
} // namespace ftree_tu
