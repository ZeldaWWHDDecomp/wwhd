/** Chart-screen extension pending MAP-sea_chart#2 canonical adoption. No GameCube counterpart. */
#pragma once
#include "d/hd_parts.h"
#include "d/hd_sea_chart_link_pos.h"
struct hdMapChartPaneNames_l { hdUiSafeString mNames[6]; };
WWHD_SIZE(hdMapChartPaneNames_l, 0x30);
/* the chart screens derive from the merged sea-chart LinkPos screen (hd_sea_chart_link_pos.h, MAP-sea_chart#2) */
using hdMapChartBase_c = hdSeaChartLinkPos_c;
struct hdMapChartDesc_l {
    u8 mDescriptorPrefix[0x14];
    hdUiSafeString mName;
    u8 mDescriptorTail[0xC];
};
struct hdMapChartChildNames_l { hdUiSafeString mNames[3]; };
WWHD_OFFSET(hdMapChartDesc_l, mName, 0x14);
WWHD_SIZE(hdMapChartDesc_l, 0x28);
WWHD_SIZE(hdMapChartChildNames_l, 0x18);
struct hdEnemyTower_c : hdMapChartBase_c {
    static hdEnemyTower_c* construct(hdEnemyTower_c*);
    static void destroy(hdEnemyTower_c*, u32 flags);
    void configure();
    u32 createNamedChild(hdMapChartDesc_l* desc);
    u32 load(void* context, void* resource, void* option);
    u32 loadLayout(void* name, u32 first, u32 second, u32 third, void* desc);
    void finalize();
    void executeState();
};
WWHD_SIZE(hdEnemyTower_c, 0x3BC);

void chartUpdateMachine(hdStateMachine* machine);
void* chartAllocate(u32 size);
/* defined in the merged hd_sea_chart_link_pos.cpp */
void seaChartLinkDestroy(hdSeaChartLinkPos_c* self, s32 flags);
hdSeaChartLinkPos_c* seaChartLinkConstruct(hdSeaChartLinkPos_c* self);

struct hdMapChartArchives_l { hdUiSafeString mNames[4]; };
struct hdMapChartAnimationNames_l { hdUiSafeString mNames[2]; };
struct hdMapChartAnimationRemap_l { be<u32> mIndices[2]; };
WWHD_SIZE(hdMapChartArchives_l, 0x20);
WWHD_SIZE(hdMapChartAnimationNames_l, 0x10);
WWHD_SIZE(hdMapChartAnimationRemap_l, 8);
void chartBindAnimation(hdAnimSet* set, u32 index, hdUiSafeString* group, hdUiSafeString* name);

// Static chart configuration and runtime registration storage; no image initializers are copied.
// These lfs/stfs configuration copies preserve payload bits in the recompiled original.
union hdMapChartScalar_l { be<f32> mValue; be<u32> mBits; };
WWHD_SIZE(hdMapChartScalar_l, 4);
struct hdMapChartStaticSettings_l {
    hdMapChartScalar_l mMinimumAngle, mMaximumAngle;
    hdMapChartScalar_l mDistances[4];
    be<u8> mSignedHeader, mUnsignedHeader;
    u8 mPadding[2];
    be<u32> mRegistrationWork[4];
};
struct hdMapChartSettingConstants_l { hdMapChartScalar_l mMinimumAngle, mMaximumAngle, mFarDistance, mNearDistance; };
struct hdMapChartRegistration_l { u8 mNode[12]; };
struct hdMapChartAnimationName_l { hdUiSafeString mName; };
WWHD_OFFSET(hdMapChartStaticSettings_l, mMinimumAngle, 0);
WWHD_OFFSET(hdMapChartStaticSettings_l, mMaximumAngle, 4);
WWHD_OFFSET(hdMapChartStaticSettings_l, mDistances, 8);
WWHD_OFFSET(hdMapChartStaticSettings_l, mSignedHeader, 0x18);
WWHD_OFFSET(hdMapChartStaticSettings_l, mUnsignedHeader, 0x19);
WWHD_OFFSET(hdMapChartStaticSettings_l, mRegistrationWork, 0x1C);
WWHD_SIZE(hdMapChartStaticSettings_l, 0x2C);
WWHD_SIZE(hdMapChartSettingConstants_l, 0x10);
WWHD_SIZE(hdMapChartRegistration_l, 12);
WWHD_SIZE(hdMapChartAnimationName_l, 8);
void chartRegisterGlobal(hdMapChartRegistration_l*);
void chartInitializeSignedHeader(be<u8>*);
void chartInitializeUnsignedHeader(be<u8>*);
