/* Tetra demo unit views. GameCube names are retained in the shared actor header.
 * HD adds self-relative animation resources, packed material links, and SafeString dispatch.
 * The BTP animation pointer is at +0x10; HD reads the BTK pointer at +0x68.
 * These private views cover only fields used by this source. */
#pragma once
#include "JSystem/J3DGraph.h"
#include "d/actor/d_a_npc_zl1.h"
#include "d/actor/d_a_player.h"
#include "gc_api/tu.h"

namespace Zl1Demo {
enum CutAction {
    ACT_LOK_PLYER,
    ACT_LOK_PARTNER,
    ACT_CHG_ANM_ATR,
    ACT_PLYER_TRN_PARTNER,
    ACT_PLYER_TRN_TETRA,
    ACT_MAJYU_START,
    ACT_MAJYU_END,
    ACT_OKIRU,
    ACT_OKIRU_2,
    ACT_DRW_ONOFF,
    ACT_PLYER_DRW_ONOFF,
    ACT_JMP_OFF,
    ACT_OMAMORI_ONOFF,
    ACT_SURPRISED,
    ACT_COUNT,
};
enum {
    JA_SE_ITM_OMAMORI_BLINK = 0x852,
    JA_SE_ITM_OMAMORI_TETLA = 0x854,
    EVENT_0801 = 0x801,
    EVENT_0802 = 0x802,
    EVENT_0804 = 0x804,
    HYRULE_COURTYARD_CUTSCENE = 0x3804,
    SAFE_STRING_MAX_LENGTH = 0x40001,
    DEMO_ACTOR_COUNT = 0x20,
    MATERIAL_ANIMATION_MASK = 0x3FFF8000,
    MATERIAL_INDEX_MASK = 0x7FFF,
    MATERIAL_ANIMATION_SHIFT = 15,
    ENABLE_TRANS_e = 2,
    ENABLE_ROTATE_e = 8,
    ENABLE_ANM_e = 0x20,
    ENABLE_ANM_FRAME_e = 0x40,
};
struct CutTable {
    gptr<const char> names[ACT_COUNT];
};
struct TargetOffsets {
    cXyz values[12];
};
struct StageName {
    char name[8];
};
struct PlayStageView {
    /* 0x0000 */ u8 _0000[0x5134];
    /* 0x5134 */ StageName mStartStage;
};
WWHD_OFFSET(PlayStageView, mStartStage, 0x5134);
WWHD_SIZE(PlayStageView, 0x513C);
WWHD_SIZE(CutTable, 0x38);
WWHD_SIZE(TargetOffsets, 0x90);
WWHD_SIZE(StageName, 8);
struct SafeStringVtable {
    /* 0x00 */ u8 _00[0x14];
    /* 0x14 */ be<u32> assureTermination;
};
WWHD_OFFSET(SafeStringVtable, assureTermination, 0x14);
WWHD_SIZE(SafeStringVtable, 0x18);
inline cXyz& playerHand(fopAc_ac_c* actor) { return static_cast<daPy_py_c*>(actor)->mRightHandPos; }

template <typename T> struct RelativePointer {
    be<u32> mOffset;
    T* get() { return gabi::at<T>(mOffset ? gabi::ea(this) + (u32)mOffset : 0); }
};
WWHD_SIZE(RelativePointer<void>, 4);

struct MaterialResource {
    /* 0x00 */ u8 _00[0xC];
    /* 0x0C */ RelativePointer<be<u8>> mName;
    /* 0x10 */ u8 _10[0xC];
};
WWHD_OFFSET(MaterialResource, mName, 0xC);
WWHD_SIZE(MaterialResource, 0x1C);
struct PatternResource {
    /* 0x00 */ u8 _00[0x16];
    /* 0x16 */ be<u16> mMaterialCount;
    /* 0x18 */ u8 _18[0x14];
    /* 0x2C */ RelativePointer<MaterialResource> mMaterials;
};
WWHD_OFFSET(PatternResource, mMaterialCount, 0x16);
WWHD_OFFSET(PatternResource, mMaterials, 0x2C);
WWHD_SIZE(PatternResource, 0x30);
struct PatternAnimation {
    /* 0x00 */ u8 _00[0x10];
    /* 0x10 */ gptr<void> mpAnm;
    /* 0x14 */ u8 _14[0x40];
    /* 0x54 */ gptr<be<u32>> mpMaterialLinks;
    /* 0x58 */ u8 _58[8];
    /* 0x60 */ gptr<PatternResource> mpResource;
    /* 0x64 */ u8 _64[0x10];
};
WWHD_OFFSET(PatternAnimation, mpAnm, 0x10);
WWHD_OFFSET(PatternAnimation, mpMaterialLinks, 0x54);
WWHD_OFFSET(PatternAnimation, mpResource, 0x60);
WWHD_SIZE(PatternAnimation, 0x74);
struct TextureAnimation {
    /* 0x00 */ u8 _00[0x68];
    /* 0x68 */ gptr<void> mpAnm;
    /* 0x6C */ u8 _6C[8];
};
WWHD_OFFSET(TextureAnimation, mpAnm, 0x68);
WWHD_SIZE(TextureAnimation, 0x74);
struct MaterialNameHeader {
    /* 0x00 */ u8 _00[0x18];
    /* 0x18 */ RelativePointer<void> mNames;
};
WWHD_OFFSET(MaterialNameHeader, mNames, 0x18);
WWHD_SIZE(MaterialNameHeader, 0x1C);
struct DemoActor {
    /* 0x00 */ u8 _00[4];
    /* 0x04 */ be<u16> mFlags;
    /* 0x06 */ u8 _06[2];
    /* 0x08 */ cXyz mTrans;
};
WWHD_OFFSET(DemoActor, mFlags, 4);
WWHD_OFFSET(DemoActor, mTrans, 8);
WWHD_SIZE(DemoActor, 0x14);
struct DemoObject;
struct DemoObjectPointer {
    gptr<DemoObject> pointer;
};
WWHD_SIZE(DemoObjectPointer, 4);
} // namespace Zl1Demo
void Zl1Demo_seStart(u32);
void Zl1Demo_StartShock(dVibration_c*, s32, s32, cXyz*);
void Zl1Demo_cutEnd(dEvent_manager_c*, s32);
BOOL Zl1Demo_getIsAddvance(dEvent_manager_c*, s32);
BOOL Zl1Demo_SearchByID(fpc_ProcID, gptr<fopAc_ac_c>*);
void Zl1Demo_playerEyePos(cXyz*, f32);
void Zl1Demo_assureTerminationImpl(SafeString*);
Zl1Demo::DemoActor* Zl1Demo_getActor(Zl1Demo::DemoObject*, u8);
void* Zl1Demo_getP_BtpData(Zl1Demo::DemoActor*, const char*);
void* Zl1Demo_getP_BtkData(Zl1Demo::DemoActor*, const char*);
Zl1Demo::MaterialNameHeader* Zl1Demo_getMaterialName(J3DModelData*);
s32 Zl1Demo_getMaterialIndex(void*, u32);
u32 Zl1Demo_GetGndMtrlSndId(cXyz*, f32);
void dKy_set_vrboxcol_ratio(f32);
void dKy_set_fogcol_ratio(f32);
void dKy_set_bgcol_ratio(f32);
void dKy_fog_startendz_set(f32, f32, f32);
