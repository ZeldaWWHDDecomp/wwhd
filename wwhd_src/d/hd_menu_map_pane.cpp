/* HD menu map pane part and map-coordinate conversion. */
#include "d/hd_menu_map_pane.h"

#include "d/hd_screen_map_display_coordinator.h"

// These fields are not exposed by the shared HdPadMap header yet.
struct hdMenuChartScaleView_l { u8 _header[0x4B4]; gptr<be<f32>> mpScale; };
struct hdMenuChartOffsets_l : HdPadMap {
    hdMenuPaneVector mMenuMapOffset;
    hdMenuPaneVector mDungeonMapOffset;
};
WWHD_OFFSET(hdMenuChartScaleView_l, mpScale, 0x4B4);
WWHD_OFFSET(hdMenuChartOffsets_l, mMenuMapOffset, 0x50C);
WWHD_OFFSET(hdMenuChartOffsets_l, mDungeonMapOffset, 0x514);
static be<f32>* chartScale(HdPadMap* chart) {
    return reinterpret_cast<hdMenuChartScaleView_l*>(chart)->mpScale;
}
static hdMenuChartOffsets_l* chartOffsets(HdPadMap* chart) {
    return reinterpret_cast<hdMenuChartOffsets_l*>(chart);
}
struct hdMenuMapSystem_l {
    u8 _header[0x218];
    gptr<hdMapDisplayCoordinator_c> mpSettings;
};
struct hdMenuMapDelegate_l {
    u8 _header[0x84];
    gfn mCalc;
    u8 _calcTail[0x14];
    gfn mDispatch;
    u8 _dispatchTail[0x84];
    gfn mPositionOffset;
    u8 _positionTail[4];
    gfn mScaleOffset;
    u8 _scaleTail[4];
    gfn mRotationOffset;
    u8 _rotationTail[4];
    gfn mAdditionalOffset;
};
WWHD_OFFSET(hdMenuMapDelegate_l, mCalc, 0x84);
WWHD_OFFSET(hdMenuMapDelegate_l, mDispatch, 0x9C);
WWHD_OFFSET(hdMenuMapDelegate_l, mPositionOffset, 0x124);
WWHD_OFFSET(hdMenuMapDelegate_l, mAdditionalOffset, 0x13C);
struct hdMenuLocation_l { u8 _header[0xC]; be<u32> mFlags; };
struct hdMenuWorldVtable_l { u8 _header[0x15C]; gfn mGetLocation; };
struct hdMenuWorld_l { gptr<hdMenuWorldVtable_l> mVtable; };
struct hdMenuGameInfo_l { u8 _header[0x5150]; hdMenuWorld_l mWorld; };
struct hdMenuMapConstants_l {
    hdMenuPaneVector mMenuOffset;
    hdMenuPaneVector mDungeonOffset;
    be<f32> mNegativePi;
    be<f32> mPositivePi;
    hdMenuPaneVector mLargeBounds;
    hdMenuPaneVector mSmallBounds;
    be<u8> mRandom;
    be<u8> mUtility;
    u8 _padding[2];
    be<u32> mStorage[4];
    hdMenuPaneVector mDungeonStageOffsets[6];
};
WWHD_OFFSET(hdMenuMapConstants_l, mStorage, 0x2C);
WWHD_OFFSET(hdMenuMapConstants_l, mDungeonStageOffsets, 0x3C);
struct hdMenuMapRegistration_l { u8 mStorage[12]; };
static hdMenuMapSystem_l* sMenuMapSystem;
static hdMenuMapConstants_l sMenuMapConstants;
static hdMenuMapRegistration_l sMapStorageRegistration;
static hdMenuMapRegistration_l sMapRandomRegistration;
static hdMenuMapRegistration_l sMapUtilityRegistration;
static u8 sMapPartVtable;
static u8 sMapPartOwnerVtable;
static u8 sMapPartSecondaryVtable;
extern void executeMenuMapState(hdStateMachine*);
extern void* mapPaneAllocate(u32);
extern void mapPaneDelete(void*);
extern void constructMapPaneBase(hdMenuMapPanePart*);
extern void destroyMapPaneBase(hdMenuMapPanePart*, u32);
extern void calculateMapPaneBase(hdMenuMapPanePart*);
extern hdMenuGameInfo_l* getMenuMapGameInfo();
extern void convertMenuMapPosition(hdMenuPaneVector*, const hdMenuPaneVector*, hdMenuPaneVector*, f32);
extern void registerMapPaneStatic(hdMenuMapRegistration_l*);
extern void constructMapPaneRandom(be<u8>*);
extern void constructMapPaneUtility(be<u8>*);

static hdMenuMapDelegate_l* mapPaneDelegate(hdMenuMapPanePart* self) {
    return reinterpret_cast<hdMenuMapDelegate_l*>(self->mVtable.get());
}

/* 0268B728 */
void hdMenuMapPanePart::destroy(u32 flags) {
    if (this == NULL) return;
    mVtable = reinterpret_cast<hdPartsVtbl_c*>(&sMapPartVtable);
    mSecondaryVtable = &sMapPartSecondaryVtable;
    mStateHolder.mHolderVtable = &sMapPartOwnerVtable;
    mControl.reset();
    mpMap = NULL;
    mpChart = NULL;
    destroyMapPaneBase(this, 0);
    if ((flags & 1) != 0) mapPaneDelete(this);
}

/* 0268B7B0 */
hdMenuMapPanePart* hdMenuMapPanePart::construct(hdMenuMapPanePart* self) {
    if (self == NULL) {
        self = static_cast<hdMenuMapPanePart*>(mapPaneAllocate(sizeof(hdMenuMapPanePart)));
        if (self == NULL) return NULL;
    }
    constructMapPaneBase(self);
    self->mStateHolder.mHolderVtable = &sMapPartOwnerVtable;
    self->mVtable = reinterpret_cast<hdPartsVtbl_c*>(&sMapPartVtable);
    self->mSecondaryVtable = &sMapPartSecondaryVtable;
    self->mSuppressDungeonStageOffset = 0;
    return self;
}

/* 0268B824 */
void hdMenuMapPanePart::calc() {
    gabi::call_ptr(mapPaneDelegate(this)->mCalc.get(), this);
    calculateMapPaneBase(this);
}

/* 0268B864 */
void hdMenuMapPanePart::execute() { executeMenuMapState(stateMachine()); }

/* 0268B86C */
void hdMenuMapPanePart::dispatch() { gabi::call_ptr(mapPaneDelegate(this)->mDispatch.get(), this); }

/* 0268B87C */
void hdMenuMapPanePart::calculateOffset() {
    hdMenuPaneVector offset;
    offset.x = 0.0f;
    offset.y = 0.0f;
    gabi::call_ptr(mapPaneDelegate(this)->mPositionOffset.get(), this, &mControl.mPosition, &offset);
    gabi::call_ptr(mapPaneDelegate(this)->mScaleOffset.get(), this, &offset, &offset);
    gabi::call_ptr(mapPaneDelegate(this)->mRotationOffset.get(), this, &offset, &offset);
    gabi::call_ptr(mapPaneDelegate(this)->mAdditionalOffset.get(), this, &offset, &offset);
    f32 x = offset.x;
    f32 y = offset.y;
    mControl.mBasePosition.x = x;
    mControl.mBasePosition.y = y;
}

/* 0268B930 */
void hdMenuMapPanePart::updateRotation() {
    u32 rotated = sMenuMapSystem->mpSettings->isDungeonOrTowerMap();
    HdPadMap* chart = mpChart;
    if (chart != NULL) {
        f32 rotation = chart->mAngle;
        mControl.mRotationOffset = rotated != 0 ? rotation - 64.99500274658203f : rotation;
    }
}

/* 0268B9A4 */
f32 hdMenuMapPanePart::getMapScale() {
    f32 scale = 1.0f;
    HdPadMap* chart = mpChart;
    if (chart != NULL && chartScale(chart) != NULL) {
        scale = *chartScale(chart);
        if (sMenuMapSystem->mpSettings->isTowerMap() != 0) scale = 0.02672800049185753f;
        else if (sMenuMapSystem->mpSettings->isDungeonMapForced() != 0) scale = 0.030890900641679764f;
    }
    return scale;
}

/* 0268BA4C */
void hdMenuMapPanePart::addDungeonStageOffset(const hdMenuPaneVector* input, hdMenuPaneVector* output) {
    hdMenuWorld_l* world = &getMenuMapGameInfo()->mWorld;
    hdMenuLocation_l* location = gabi::call_ptr<hdMenuLocation_l*>(world->mVtable->mGetLocation.get(), world);
    if (location != NULL) {
        world = &getMenuMapGameInfo()->mWorld;
        location = gabi::call_ptr<hdMenuLocation_l*>(world->mVtable->mGetLocation.get(), world);
        if (((u32(location->mFlags) >> 16) & 7) == 6 && mSuppressDungeonStageOffset == 0) {
            u32 stageKind = sMenuMapSystem->mpSettings->mStageKind;
            if (stageKind >= 6) return;
            f32 x = input->x;
            if (stageKind == 1) {
                f32 offsetX = sMenuMapConstants.mDungeonStageOffsets[1].x;
                f32 y = input->y;
                f32 offsetY = sMenuMapConstants.mDungeonStageOffsets[1].y;
                f32 resultX = x * 1.5f + offsetX;
                f32 resultY = y * 1.5f + offsetY;
                output->x = resultX;
                output->y = resultY;
            } else {
                f32 y = input->y;
                f32 offsetX = sMenuMapConstants.mDungeonStageOffsets[stageKind].x;
                f32 offsetY = sMenuMapConstants.mDungeonStageOffsets[stageKind].y;
                f32 resultX = x + offsetX;
                f32 resultY = y + offsetY;
                output->x = resultX;
                output->y = resultY;
            }
            return;
        }
    }
    output->x = input->x;
    output->y = input->y;
}

/* 0268BBCC */
void hdMenuMapPanePart::transformMapPoint(const hdMenuPaneVector* input, hdMenuPaneVector* output) {
    if (output == NULL || mpChart.get() == NULL) return;
    hdMenuPaneVector offset;
    offset.x = 0.0f;
    offset.y = 0.0f;
    if (sMenuMapSystem->mpSettings->isTowerMap() != 0) {
        offset.x = f32(offset.x) + f32(sMenuMapConstants.mMenuOffset.x);
        offset.y = f32(offset.y) + f32(sMenuMapConstants.mMenuOffset.y);
        HdPadMap* chart = mpChart;
        offset.x = f32(offset.x) + f32(chartOffsets(chart)->mMenuMapOffset.x);
        offset.y = f32(offset.y) + f32(chartOffsets(chart)->mMenuMapOffset.y);
    } else if (sMenuMapSystem->mpSettings->isDungeonMapForced() != 0) {
        offset.x = f32(offset.x) + f32(sMenuMapConstants.mDungeonOffset.x);
        offset.y = f32(offset.y) + f32(sMenuMapConstants.mDungeonOffset.y);
        HdPadMap* chart = mpChart;
        offset.x = f32(offset.x) + f32(chartOffsets(chart)->mDungeonMapOffset.x);
        offset.y = f32(offset.y) + f32(chartOffsets(chart)->mDungeonMapOffset.y);
    }
    convertMenuMapPosition(output, input, &offset, -64.99500274658203f);
}

/* 0268BCFC */
void hdMenuMapPanePart::getPanePosition(const hdMenuPaneVector* input, hdMenuPaneVector* output) {
    HdPadMap* chart = mpChart;
    if (chart == NULL || output == NULL || chartScale(chart) == NULL) return;
    f32 scale = getMapScale();
    hdMenuPaneVector point;
    point.x = 0.0f;
    point.y = 0.0f;
    addDungeonStageOffset(input, &point);
    f32 x = gabi::fmuls_ppc(f32(point.x), scale);
    f32 y = gabi::fmuls_ppc(f32(point.y), scale);
    output->x = x;
    output->y = -y;
    if (sMenuMapSystem->mpSettings->isDungeonOrTowerMap() != 0) transformMapPoint(output, output);
}

/* 0268BDE4 */
static void initializeMenuMapPaneStatics() {
    for (s32 i = 3; i >= 0; --i) sMenuMapConstants.mStorage[i] = 0;
    registerMapPaneStatic(&sMapStorageRegistration);
    sMenuMapConstants.mNegativePi = -3.1415927410125732f;
    sMenuMapConstants.mPositivePi = 3.1415927410125732f;
    constructMapPaneRandom(&sMenuMapConstants.mRandom);
    registerMapPaneStatic(&sMapRandomRegistration);
    constructMapPaneUtility(&sMenuMapConstants.mUtility);
    registerMapPaneStatic(&sMapUtilityRegistration);
    sMenuMapConstants.mSmallBounds.x = 10000.0f;
    sMenuMapConstants.mMenuOffset.x = 30.799999237060547f;
    sMenuMapConstants.mDungeonStageOffsets[0].x = 0.0f;
    sMenuMapConstants.mDungeonStageOffsets[0].y = 0.0f;
    sMenuMapConstants.mDungeonStageOffsets[1].x = 321.0f;
    sMenuMapConstants.mDungeonStageOffsets[1].y = 750.0f;
    sMenuMapConstants.mDungeonStageOffsets[2].x = -346.0f;
    sMenuMapConstants.mDungeonStageOffsets[2].y = -10776.0f;
    sMenuMapConstants.mDungeonStageOffsets[3].x = -3500.0f;
    sMenuMapConstants.mDungeonStageOffsets[3].y = -5800.0f;
    sMenuMapConstants.mDungeonOffset.x = 25.600000381469727f;
    sMenuMapConstants.mLargeBounds.x = 50000.0f;
    sMenuMapConstants.mMenuOffset.y = 24.799999237060547f;
    sMenuMapConstants.mLargeBounds.y = 50000.0f;
    sMenuMapConstants.mDungeonStageOffsets[4].x = 6150.0f;
    sMenuMapConstants.mSmallBounds.y = 10000.0f;
    sMenuMapConstants.mDungeonOffset.y = 46.79999923706055f;
    sMenuMapConstants.mDungeonStageOffsets[4].y = 4200.0f;
    sMenuMapConstants.mDungeonStageOffsets[5].x = 0.0f;
    sMenuMapConstants.mDungeonStageOffsets[5].y = -3540.0f;
}

/* WWHD glue (tools/verify/autoglue.py): menu map pane
 data sMenuMapSystem 101F8344
 data sMenuMapConstants 10496590
 data sMapStorageRegistration 101F632C
 data sMapRandomRegistration 101F6338
 data sMapUtilityRegistration 101F6344
 data sMapPartVtable 100F9C18
 data sMapPartOwnerVtable 100F9D58
 data sMapPartSecondaryVtable 100F9D68
 ext 02006364 void executeMenuMapState(hdStateMachine*)
 ext 0273AD10 void* mapPaneAllocate(u32)
 ext 0273AF40 void mapPaneDelete(void*)
 ext 02685134 void constructMapPaneBase(hdMenuMapPanePart*)
 ext 02684E94 void destroyMapPaneBase(hdMenuMapPanePart*,u32)
 ext 026F91F8 void calculateMapPaneBase(hdMenuMapPanePart*)
 ext 02689304 u32 hdMapDisplayCoordinator_c::isDungeonOrTowerMap()
 ext 026892E0 u32 hdMapDisplayCoordinator_c::isTowerMap()
 ext 0268705C u32 hdMapDisplayCoordinator_c::isDungeonMapForced()
 ext 025200D4 hdMenuGameInfo_l* getMenuMapGameInfo()
 ext 026893C8 void convertMenuMapPosition(hdMenuPaneVector*,const hdMenuPaneVector*,hdMenuPaneVector*,f32)
 ext 028F026C void registerMapPaneStatic(hdMenuMapRegistration_l*)
 ext 028ED6F8 void constructMapPaneRandom(be<u8>*)
 ext 028EAB2C void constructMapPaneUtility(be<u8>*)
*/
