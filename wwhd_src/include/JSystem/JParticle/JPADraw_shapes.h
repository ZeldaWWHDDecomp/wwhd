// Draw-side virtual interfaces use the tww roles and Cafe paired function/delta slots.
#pragma once
#include "wwhd.h"
namespace JPADrawExtension {
enum JPADrawShapeType { DrawPoint,DrawLine,DrawBillboard,DrawDirection,DrawDirectionCross,DrawStripe,DrawStripeCross,DrawRotation,DrawRotationCross,DrawDirBillboard,DrawYBillboard };
struct JPADrawBaseShapeVtable_l {
    u8 mBeforeGetType[0x14];
    be<u32> getType;
    u8 mBeforeGetDirType[0x4];
    be<u32> getDirType;
    u8 mBeforeGetRotType[0x4];
    be<u32> getRotType;
    u8 mBeforeGetBasePlaneType[0x4];
    be<u32> getBasePlaneType;
    u8 mBeforeGetBaseSizeX[0x4];
    be<u32> getBaseSizeX;
    u8 mBeforeGetBaseSizeY[0x4];
    be<u32> getBaseSizeY;
    u8 mBeforeGetLoopOffset[0x4];
    be<u32> getLoopOffset;
    u8 mBeforeGetColLoopOffset[0x4];
    be<u32> getColLoopOffset;
    u8 mBeforeGetTexLoopOffset[0x4];
    be<u32> getTexLoopOffset;
    u8 mBeforeIsEnableGlobalColAnm[0x4];
    be<u32> isEnableGlobalColAnm;
    u8 mBeforeIsEnableGlobalTexAnm[0x4];
    be<u32> isEnableGlobalTexAnm;
    u8 mBeforeGetListOrder[0x4];
    be<u32> getListOrder;
    u8 mBeforeGetChildOrder[0x4];
    be<u32> getChildOrder;
    u8 mBeforeGetTevMode[0x4];
    be<u32> getTevMode;
    u8 mBeforeGetBlendMode1[0x1C];
    be<u32> getBlendMode1;
    u8 mBeforeGetSrcBlendFactor1[0x4];
    be<u32> getSrcBlendFactor1;
    u8 mBeforeGetDstBlendFactor1[0x4];
    be<u32> getDstBlendFactor1;
    u8 mBeforeIsEnableZCmp[0xC];
    be<u32> isEnableZCmp;
    u8 mBeforeGetZCmpFunction[0x4];
    be<u32> getZCmpFunction;
    u8 mBeforeIsEnableZCmpUpdate[0x4];
    be<u32> isEnableZCmpUpdate;
    u8 mBeforeGetAlphaCmpComp0[0xC];
    be<u32> getAlphaCmpComp0;
    u8 mBeforeGetAlphaCmpRef0[0x14];
    be<u32> getAlphaCmpRef0;
    u8 mBeforeIsEnableAnmTone[0xC];
    be<u32> isEnableAnmTone;
    u8 mBeforeIsEnableProjection[0x4];
    be<u32> isEnableProjection;
    u8 mBeforeIsEnableTextureAnm[0xC];
    be<u32> isEnableTextureAnm;
    u8 mBeforeTextureIsEmpty[0x4];
    be<u32> textureIsEmpty;
    u8 mBeforeGetTextureAnmType[0x4];
    be<u32> getTextureAnmType;
    u8 mBeforeGetTextureAnmKeyNum[0x4];
    be<u32> getTextureAnmKeyNum;
    u8 mBeforeGetTextureIndex[0x4];
    be<u32> getTextureIndex;
    u8 mBeforeIsEnablePrm[0xC];
    be<u32> isEnablePrm;
    u8 mBeforeIsEnableEnv[0x4];
    be<u32> isEnableEnv;
    u8 mBeforeIsEnablePrmAnm[0x4];
    be<u32> isEnablePrmAnm;
    u8 mBeforeIsEnableEnvAnm[0x4];
    be<u32> isEnableEnvAnm;
    u8 mBeforeGetPrmColor[0x4];
    be<u32> getPrmColor;
    u8 mBeforeGetEnvColor[0xC];
    be<u32> getEnvColor;
    u8 mBeforeGetColorRegAnmType[0xC];
    be<u32> getColorRegAnmType;
    u8 mBeforeGetColorRegAnmMaxFrm[0x4];
    be<u32> getColorRegAnmMaxFrm;
    u8 mBeforeIsEnableTexScrollAnm[0x4];
    be<u32> isEnableTexScrollAnm;
    u8 mBeforeGetTilingX[0x4];
    be<u32> getTilingX;
    u8 mBeforeGetTilingY[0x4];
    be<u32> getTilingY;
    u8 mBeforeGetTexStaticTransX[0x4];
    be<u32> getTexStaticTransX;
    u8 mBeforeGetTexStaticTransY[0x4];
    be<u32> getTexStaticTransY;
    u8 mBeforeGetTexStaticScaleX[0x4];
    be<u32> getTexStaticScaleX;
    u8 mBeforeGetTexStaticScaleY[0x4];
    be<u32> getTexStaticScaleY;
    u8 mBeforeGetTexScrollTransX[0x4];
    be<u32> getTexScrollTransX;
    u8 mBeforeGetTexScrollTransY[0x4];
    be<u32> getTexScrollTransY;
    u8 mBeforeGetTexScrollScaleX[0x4];
    be<u32> getTexScrollScaleX;
    u8 mBeforeGetTexScrollScaleY[0x4];
    be<u32> getTexScrollScaleY;
    u8 mBeforeGetTexScrollRotate[0x4];
    be<u32> getTexScrollRotate;
};
struct JPADrawBaseShape_l {
    gptr<JPADrawBaseShapeVtable_l> __vtbl;
    u32 getType() { return gabi::call_ptr<u32>(__vtbl->getType,this); }
    u32 getDirType() { return gabi::call_ptr<u32>(__vtbl->getDirType,this); }
    u32 getRotType() { return gabi::call_ptr<u32>(__vtbl->getRotType,this); }
    u32 getBasePlaneType() { return gabi::call_ptr<u32>(__vtbl->getBasePlaneType,this); }
    f32 getBaseSizeX() { return gabi::call_ptr<f32>(__vtbl->getBaseSizeX,this); }
    f32 getBaseSizeY() { return gabi::call_ptr<f32>(__vtbl->getBaseSizeY,this); }
    u32 getLoopOffset() { return gabi::call_ptr<u32>(__vtbl->getLoopOffset,this); }
    u32 getColLoopOffset() { return gabi::call_ptr<u32>(__vtbl->getColLoopOffset,this); }
    u32 getTexLoopOffset() { return gabi::call_ptr<u32>(__vtbl->getTexLoopOffset,this); }
    u32 isEnableGlobalColAnm() { return gabi::call_ptr<u32>(__vtbl->isEnableGlobalColAnm,this); }
    u32 isEnableGlobalTexAnm() { return gabi::call_ptr<u32>(__vtbl->isEnableGlobalTexAnm,this); }
    u32 getListOrder() { return gabi::call_ptr<u32>(__vtbl->getListOrder,this); }
    u32 getChildOrder() { return gabi::call_ptr<u32>(__vtbl->getChildOrder,this); }
    u32 getTevMode() { return gabi::call_ptr<u32>(__vtbl->getTevMode,this); }
    u32 getBlendMode1() { return gabi::call_ptr<u32>(__vtbl->getBlendMode1,this); }
    u32 getSrcBlendFactor1() { return gabi::call_ptr<u32>(__vtbl->getSrcBlendFactor1,this); }
    u32 getDstBlendFactor1() { return gabi::call_ptr<u32>(__vtbl->getDstBlendFactor1,this); }
    u32 isEnableZCmp() { return gabi::call_ptr<u32>(__vtbl->isEnableZCmp,this); }
    u32 getZCmpFunction() { return gabi::call_ptr<u32>(__vtbl->getZCmpFunction,this); }
    u32 isEnableZCmpUpdate() { return gabi::call_ptr<u32>(__vtbl->isEnableZCmpUpdate,this); }
    u32 getAlphaCmpComp0() { return gabi::call_ptr<u32>(__vtbl->getAlphaCmpComp0,this); }
    u32 getAlphaCmpRef0() { return gabi::call_ptr<u32>(__vtbl->getAlphaCmpRef0,this); }
    u32 isEnableAnmTone() { return gabi::call_ptr<u32>(__vtbl->isEnableAnmTone,this); }
    u32 isEnableProjection() { return gabi::call_ptr<u32>(__vtbl->isEnableProjection,this); }
    u32 isEnableTextureAnm() { return gabi::call_ptr<u32>(__vtbl->isEnableTextureAnm,this); }
    u32 textureIsEmpty() { return gabi::call_ptr<u32>(__vtbl->textureIsEmpty,this); }
    u32 getTextureAnmType() { return gabi::call_ptr<u32>(__vtbl->getTextureAnmType,this); }
    u32 getTextureAnmKeyNum() { return gabi::call_ptr<u32>(__vtbl->getTextureAnmKeyNum,this); }
    u32 getTextureIndex() { return gabi::call_ptr<u32>(__vtbl->getTextureIndex,this); }
    u32 isEnablePrm() { return gabi::call_ptr<u32>(__vtbl->isEnablePrm,this); }
    u32 isEnableEnv() { return gabi::call_ptr<u32>(__vtbl->isEnableEnv,this); }
    u32 isEnablePrmAnm() { return gabi::call_ptr<u32>(__vtbl->isEnablePrmAnm,this); }
    u32 isEnableEnvAnm() { return gabi::call_ptr<u32>(__vtbl->isEnableEnvAnm,this); }
    u32 getPrmColor() { return gabi::call_ptr<u32>(__vtbl->getPrmColor,this); }
    u32 getEnvColor() { return gabi::call_ptr<u32>(__vtbl->getEnvColor,this); }
    u32 getColorRegAnmType() { return gabi::call_ptr<u32>(__vtbl->getColorRegAnmType,this); }
    u32 getColorRegAnmMaxFrm() { return gabi::call_ptr<u32>(__vtbl->getColorRegAnmMaxFrm,this); }
    u32 isEnableTexScrollAnm() { return gabi::call_ptr<u32>(__vtbl->isEnableTexScrollAnm,this); }
    f32 getTilingX() { return gabi::call_ptr<f32>(__vtbl->getTilingX,this); }
    f32 getTilingY() { return gabi::call_ptr<f32>(__vtbl->getTilingY,this); }
    f32 getTexStaticTransX() { return gabi::call_ptr<f32>(__vtbl->getTexStaticTransX,this); }
    f32 getTexStaticTransY() { return gabi::call_ptr<f32>(__vtbl->getTexStaticTransY,this); }
    f32 getTexStaticScaleX() { return gabi::call_ptr<f32>(__vtbl->getTexStaticScaleX,this); }
    f32 getTexStaticScaleY() { return gabi::call_ptr<f32>(__vtbl->getTexStaticScaleY,this); }
    f32 getTexScrollTransX() { return gabi::call_ptr<f32>(__vtbl->getTexScrollTransX,this); }
    f32 getTexScrollTransY() { return gabi::call_ptr<f32>(__vtbl->getTexScrollTransY,this); }
    f32 getTexScrollScaleX() { return gabi::call_ptr<f32>(__vtbl->getTexScrollScaleX,this); }
    f32 getTexScrollScaleY() { return gabi::call_ptr<f32>(__vtbl->getTexScrollScaleY,this); }
    f32 getTexScrollRotate() { return gabi::call_ptr<f32>(__vtbl->getTexScrollRotate,this); }
};
WWHD_OFFSET(JPADrawBaseShapeVtable_l,getType,0x14);
WWHD_OFFSET(JPADrawBaseShapeVtable_l,getDirType,0x1C);
WWHD_OFFSET(JPADrawBaseShapeVtable_l,getRotType,0x24);
WWHD_OFFSET(JPADrawBaseShapeVtable_l,getBasePlaneType,0x2C);
WWHD_OFFSET(JPADrawBaseShapeVtable_l,getBaseSizeX,0x34);
WWHD_OFFSET(JPADrawBaseShapeVtable_l,getBaseSizeY,0x3C);
WWHD_OFFSET(JPADrawBaseShapeVtable_l,getLoopOffset,0x44);
WWHD_OFFSET(JPADrawBaseShapeVtable_l,getColLoopOffset,0x4C);
WWHD_OFFSET(JPADrawBaseShapeVtable_l,getTexLoopOffset,0x54);
WWHD_OFFSET(JPADrawBaseShapeVtable_l,isEnableGlobalColAnm,0x5C);
WWHD_OFFSET(JPADrawBaseShapeVtable_l,isEnableGlobalTexAnm,0x64);
WWHD_OFFSET(JPADrawBaseShapeVtable_l,getListOrder,0x6C);
WWHD_OFFSET(JPADrawBaseShapeVtable_l,getChildOrder,0x74);
WWHD_OFFSET(JPADrawBaseShapeVtable_l,getTevMode,0x7C);
WWHD_OFFSET(JPADrawBaseShapeVtable_l,getBlendMode1,0x9C);
WWHD_OFFSET(JPADrawBaseShapeVtable_l,getSrcBlendFactor1,0xA4);
WWHD_OFFSET(JPADrawBaseShapeVtable_l,getDstBlendFactor1,0xAC);
WWHD_OFFSET(JPADrawBaseShapeVtable_l,isEnableZCmp,0xBC);
WWHD_OFFSET(JPADrawBaseShapeVtable_l,getZCmpFunction,0xC4);
WWHD_OFFSET(JPADrawBaseShapeVtable_l,isEnableZCmpUpdate,0xCC);
WWHD_OFFSET(JPADrawBaseShapeVtable_l,getAlphaCmpComp0,0xDC);
WWHD_OFFSET(JPADrawBaseShapeVtable_l,getAlphaCmpRef0,0xF4);
WWHD_OFFSET(JPADrawBaseShapeVtable_l,isEnableAnmTone,0x104);
WWHD_OFFSET(JPADrawBaseShapeVtable_l,isEnableProjection,0x10C);
WWHD_OFFSET(JPADrawBaseShapeVtable_l,isEnableTextureAnm,0x11C);
WWHD_OFFSET(JPADrawBaseShapeVtable_l,textureIsEmpty,0x124);
WWHD_OFFSET(JPADrawBaseShapeVtable_l,getTextureAnmType,0x12C);
WWHD_OFFSET(JPADrawBaseShapeVtable_l,getTextureAnmKeyNum,0x134);
WWHD_OFFSET(JPADrawBaseShapeVtable_l,getTextureIndex,0x13C);
WWHD_OFFSET(JPADrawBaseShapeVtable_l,isEnablePrm,0x14C);
WWHD_OFFSET(JPADrawBaseShapeVtable_l,isEnableEnv,0x154);
WWHD_OFFSET(JPADrawBaseShapeVtable_l,isEnablePrmAnm,0x15C);
WWHD_OFFSET(JPADrawBaseShapeVtable_l,isEnableEnvAnm,0x164);
WWHD_OFFSET(JPADrawBaseShapeVtable_l,getPrmColor,0x16C);
WWHD_OFFSET(JPADrawBaseShapeVtable_l,getEnvColor,0x17C);
WWHD_OFFSET(JPADrawBaseShapeVtable_l,getColorRegAnmType,0x18C);
WWHD_OFFSET(JPADrawBaseShapeVtable_l,getColorRegAnmMaxFrm,0x194);
WWHD_OFFSET(JPADrawBaseShapeVtable_l,isEnableTexScrollAnm,0x19C);
WWHD_OFFSET(JPADrawBaseShapeVtable_l,getTilingX,0x1A4);
WWHD_OFFSET(JPADrawBaseShapeVtable_l,getTilingY,0x1AC);
WWHD_OFFSET(JPADrawBaseShapeVtable_l,getTexStaticTransX,0x1B4);
WWHD_OFFSET(JPADrawBaseShapeVtable_l,getTexStaticTransY,0x1BC);
WWHD_OFFSET(JPADrawBaseShapeVtable_l,getTexStaticScaleX,0x1C4);
WWHD_OFFSET(JPADrawBaseShapeVtable_l,getTexStaticScaleY,0x1CC);
WWHD_OFFSET(JPADrawBaseShapeVtable_l,getTexScrollTransX,0x1D4);
WWHD_OFFSET(JPADrawBaseShapeVtable_l,getTexScrollTransY,0x1DC);
WWHD_OFFSET(JPADrawBaseShapeVtable_l,getTexScrollScaleX,0x1E4);
WWHD_OFFSET(JPADrawBaseShapeVtable_l,getTexScrollScaleY,0x1EC);
WWHD_OFFSET(JPADrawBaseShapeVtable_l,getTexScrollRotate,0x1F4);
struct JPADrawExtraShapeVtable_l {
    u8 mBeforeIsEnableScale[0x14];
    be<u32> isEnableScale;
    u8 mBeforeIsDiffXY[0x4];
    be<u32> isDiffXY;
    u8 mBeforeIsEnableScaleAnmX[0x4];
    be<u32> isEnableScaleAnmX;
    u8 mBeforeIsEnableScaleAnmY[0x4];
    be<u32> isEnableScaleAnmY;
    u8 mBeforeIsEnableScaleBySpeedX[0x4];
    be<u32> isEnableScaleBySpeedX;
    u8 mBeforeIsEnableScaleBySpeedY[0x4];
    be<u32> isEnableScaleBySpeedY;
    u8 mBeforeGetScaleInTiming[0x4];
    be<u32> getScaleInTiming;
    u8 mBeforeGetScaleOutTiming[0x4];
    be<u32> getScaleOutTiming;
    u8 mBeforeGetScaleInValueX[0x4];
    be<u32> getScaleInValueX;
    u8 mBeforeGetScaleInValueY[0x4];
    be<u32> getScaleInValueY;
    u8 mBeforeGetAnmTypeX[0x4];
    be<u32> getAnmTypeX;
    u8 mBeforeGetAnmTypeY[0x4];
    be<u32> getAnmTypeY;
    u8 mBeforeGetAnmCycleX[0x4];
    be<u32> getAnmCycleX;
    u8 mBeforeGetAnmCycleY[0x4];
    be<u32> getAnmCycleY;
    u8 mBeforeGetIncreaseRateX[0x4];
    be<u32> getIncreaseRateX;
    u8 mBeforeGetIncreaseRateY[0x4];
    be<u32> getIncreaseRateY;
    u8 mBeforeGetDecreaseRateX[0x4];
    be<u32> getDecreaseRateX;
    u8 mBeforeGetDecreaseRateY[0x4];
    be<u32> getDecreaseRateY;
    u8 mBeforeGetPivotX[0x4];
    be<u32> getPivotX;
    u8 mBeforeGetPivotY[0x4];
    be<u32> getPivotY;
    u8 mBeforeGetRandomScale[0x4];
    be<u32> getRandomScale;
    u8 mBeforeIsEnableAlpha[0x4];
    be<u32> isEnableAlpha;
    u8 mBeforeIsEnableSinWave[0x4];
    be<u32> isEnableSinWave;
    u8 mBeforeGetAlphaInTiming[0x4];
    be<u32> getAlphaInTiming;
    u8 mBeforeGetAlphaOutTiming[0x4];
    be<u32> getAlphaOutTiming;
    u8 mBeforeGetAlphaInValue[0x4];
    be<u32> getAlphaInValue;
    u8 mBeforeGetAlphaBaseValue[0x4];
    be<u32> getAlphaBaseValue;
    u8 mBeforeGetAlphaWaveType[0x4];
    be<u32> getAlphaWaveType;
    u8 mBeforeGetAlphaWaveParam1[0x4];
    be<u32> getAlphaWaveParam1;
    u8 mBeforeGetAlphaWaveParam2[0x4];
    be<u32> getAlphaWaveParam2;
    u8 mBeforeGetAlphaWaveParam3[0x4];
    be<u32> getAlphaWaveParam3;
    u8 mBeforeGetAlphaWaveRandom[0x4];
    be<u32> getAlphaWaveRandom;
    u8 mBeforeGetAlphaIncreaseRate[0x4];
    be<u32> getAlphaIncreaseRate;
    u8 mBeforeGetAlphaDecreaseRate[0x4];
    be<u32> getAlphaDecreaseRate;
    u8 mBeforeIsEnableRotate[0x4];
    be<u32> isEnableRotate;
    u8 mBeforeGetRotateAngle[0x4];
    be<u32> getRotateAngle;
    u8 mBeforeGetRotateSpeed[0x4];
    be<u32> getRotateSpeed;
    u8 mBeforeGetRotateRandomAngle[0x4];
    be<u32> getRotateRandomAngle;
    u8 mBeforeGetRotateRandomSpeed[0x4];
    be<u32> getRotateRandomSpeed;
    u8 mBeforeGetRotateDirection[0x4];
    be<u32> getRotateDirection;
};
struct JPADrawExtraShape_l {
    gptr<JPADrawExtraShapeVtable_l> __vtbl;
    u32 isEnableScale() { return gabi::call_ptr<u32>(__vtbl->isEnableScale,this); }
    u32 isDiffXY() { return gabi::call_ptr<u32>(__vtbl->isDiffXY,this); }
    u32 isEnableScaleAnmX() { return gabi::call_ptr<u32>(__vtbl->isEnableScaleAnmX,this); }
    u32 isEnableScaleAnmY() { return gabi::call_ptr<u32>(__vtbl->isEnableScaleAnmY,this); }
    u32 isEnableScaleBySpeedX() { return gabi::call_ptr<u32>(__vtbl->isEnableScaleBySpeedX,this); }
    u32 isEnableScaleBySpeedY() { return gabi::call_ptr<u32>(__vtbl->isEnableScaleBySpeedY,this); }
    f32 getScaleInTiming() { return gabi::call_ptr<f32>(__vtbl->getScaleInTiming,this); }
    f32 getScaleOutTiming() { return gabi::call_ptr<f32>(__vtbl->getScaleOutTiming,this); }
    f32 getScaleInValueX() { return gabi::call_ptr<f32>(__vtbl->getScaleInValueX,this); }
    f32 getScaleInValueY() { return gabi::call_ptr<f32>(__vtbl->getScaleInValueY,this); }
    u32 getAnmTypeX() { return gabi::call_ptr<u32>(__vtbl->getAnmTypeX,this); }
    u32 getAnmTypeY() { return gabi::call_ptr<u32>(__vtbl->getAnmTypeY,this); }
    u32 getAnmCycleX() { return gabi::call_ptr<u32>(__vtbl->getAnmCycleX,this); }
    u32 getAnmCycleY() { return gabi::call_ptr<u32>(__vtbl->getAnmCycleY,this); }
    f32 getIncreaseRateX() { return gabi::call_ptr<f32>(__vtbl->getIncreaseRateX,this); }
    f32 getIncreaseRateY() { return gabi::call_ptr<f32>(__vtbl->getIncreaseRateY,this); }
    f32 getDecreaseRateX() { return gabi::call_ptr<f32>(__vtbl->getDecreaseRateX,this); }
    f32 getDecreaseRateY() { return gabi::call_ptr<f32>(__vtbl->getDecreaseRateY,this); }
    u32 getPivotX() { return gabi::call_ptr<u32>(__vtbl->getPivotX,this); }
    u32 getPivotY() { return gabi::call_ptr<u32>(__vtbl->getPivotY,this); }
    f32 getRandomScale() { return gabi::call_ptr<f32>(__vtbl->getRandomScale,this); }
    u32 isEnableAlpha() { return gabi::call_ptr<u32>(__vtbl->isEnableAlpha,this); }
    u32 isEnableSinWave() { return gabi::call_ptr<u32>(__vtbl->isEnableSinWave,this); }
    f32 getAlphaInTiming() { return gabi::call_ptr<f32>(__vtbl->getAlphaInTiming,this); }
    f32 getAlphaOutTiming() { return gabi::call_ptr<f32>(__vtbl->getAlphaOutTiming,this); }
    f32 getAlphaInValue() { return gabi::call_ptr<f32>(__vtbl->getAlphaInValue,this); }
    f32 getAlphaBaseValue() { return gabi::call_ptr<f32>(__vtbl->getAlphaBaseValue,this); }
    u32 getAlphaWaveType() { return gabi::call_ptr<u32>(__vtbl->getAlphaWaveType,this); }
    f32 getAlphaWaveParam1() { return gabi::call_ptr<f32>(__vtbl->getAlphaWaveParam1,this); }
    f32 getAlphaWaveParam2() { return gabi::call_ptr<f32>(__vtbl->getAlphaWaveParam2,this); }
    f32 getAlphaWaveParam3() { return gabi::call_ptr<f32>(__vtbl->getAlphaWaveParam3,this); }
    f32 getAlphaWaveRandom() { return gabi::call_ptr<f32>(__vtbl->getAlphaWaveRandom,this); }
    f32 getAlphaIncreaseRate() { return gabi::call_ptr<f32>(__vtbl->getAlphaIncreaseRate,this); }
    f32 getAlphaDecreaseRate() { return gabi::call_ptr<f32>(__vtbl->getAlphaDecreaseRate,this); }
    u32 isEnableRotate() { return gabi::call_ptr<u32>(__vtbl->isEnableRotate,this); }
    f32 getRotateAngle() { return gabi::call_ptr<f32>(__vtbl->getRotateAngle,this); }
    f32 getRotateSpeed() { return gabi::call_ptr<f32>(__vtbl->getRotateSpeed,this); }
    f32 getRotateRandomAngle() { return gabi::call_ptr<f32>(__vtbl->getRotateRandomAngle,this); }
    f32 getRotateRandomSpeed() { return gabi::call_ptr<f32>(__vtbl->getRotateRandomSpeed,this); }
    f32 getRotateDirection() { return gabi::call_ptr<f32>(__vtbl->getRotateDirection,this); }
};
WWHD_OFFSET(JPADrawExtraShapeVtable_l,isEnableScale,0x14);
WWHD_OFFSET(JPADrawExtraShapeVtable_l,isDiffXY,0x1C);
WWHD_OFFSET(JPADrawExtraShapeVtable_l,isEnableScaleAnmX,0x24);
WWHD_OFFSET(JPADrawExtraShapeVtable_l,isEnableScaleAnmY,0x2C);
WWHD_OFFSET(JPADrawExtraShapeVtable_l,isEnableScaleBySpeedX,0x34);
WWHD_OFFSET(JPADrawExtraShapeVtable_l,isEnableScaleBySpeedY,0x3C);
WWHD_OFFSET(JPADrawExtraShapeVtable_l,getScaleInTiming,0x44);
WWHD_OFFSET(JPADrawExtraShapeVtable_l,getScaleOutTiming,0x4C);
WWHD_OFFSET(JPADrawExtraShapeVtable_l,getScaleInValueX,0x54);
WWHD_OFFSET(JPADrawExtraShapeVtable_l,getScaleInValueY,0x5C);
WWHD_OFFSET(JPADrawExtraShapeVtable_l,getAnmTypeX,0x64);
WWHD_OFFSET(JPADrawExtraShapeVtable_l,getAnmTypeY,0x6C);
WWHD_OFFSET(JPADrawExtraShapeVtable_l,getAnmCycleX,0x74);
WWHD_OFFSET(JPADrawExtraShapeVtable_l,getAnmCycleY,0x7C);
WWHD_OFFSET(JPADrawExtraShapeVtable_l,getIncreaseRateX,0x84);
WWHD_OFFSET(JPADrawExtraShapeVtable_l,getIncreaseRateY,0x8C);
WWHD_OFFSET(JPADrawExtraShapeVtable_l,getDecreaseRateX,0x94);
WWHD_OFFSET(JPADrawExtraShapeVtable_l,getDecreaseRateY,0x9C);
WWHD_OFFSET(JPADrawExtraShapeVtable_l,getPivotX,0xA4);
WWHD_OFFSET(JPADrawExtraShapeVtable_l,getPivotY,0xAC);
WWHD_OFFSET(JPADrawExtraShapeVtable_l,getRandomScale,0xB4);
WWHD_OFFSET(JPADrawExtraShapeVtable_l,isEnableAlpha,0xBC);
WWHD_OFFSET(JPADrawExtraShapeVtable_l,isEnableSinWave,0xC4);
WWHD_OFFSET(JPADrawExtraShapeVtable_l,getAlphaInTiming,0xCC);
WWHD_OFFSET(JPADrawExtraShapeVtable_l,getAlphaOutTiming,0xD4);
WWHD_OFFSET(JPADrawExtraShapeVtable_l,getAlphaInValue,0xDC);
WWHD_OFFSET(JPADrawExtraShapeVtable_l,getAlphaBaseValue,0xE4);
WWHD_OFFSET(JPADrawExtraShapeVtable_l,getAlphaWaveType,0xEC);
WWHD_OFFSET(JPADrawExtraShapeVtable_l,getAlphaWaveParam1,0xF4);
WWHD_OFFSET(JPADrawExtraShapeVtable_l,getAlphaWaveParam2,0xFC);
WWHD_OFFSET(JPADrawExtraShapeVtable_l,getAlphaWaveParam3,0x104);
WWHD_OFFSET(JPADrawExtraShapeVtable_l,getAlphaWaveRandom,0x10C);
WWHD_OFFSET(JPADrawExtraShapeVtable_l,getAlphaIncreaseRate,0x114);
WWHD_OFFSET(JPADrawExtraShapeVtable_l,getAlphaDecreaseRate,0x11C);
WWHD_OFFSET(JPADrawExtraShapeVtable_l,isEnableRotate,0x124);
WWHD_OFFSET(JPADrawExtraShapeVtable_l,getRotateAngle,0x12C);
WWHD_OFFSET(JPADrawExtraShapeVtable_l,getRotateSpeed,0x134);
WWHD_OFFSET(JPADrawExtraShapeVtable_l,getRotateRandomAngle,0x13C);
WWHD_OFFSET(JPADrawExtraShapeVtable_l,getRotateRandomSpeed,0x144);
WWHD_OFFSET(JPADrawExtraShapeVtable_l,getRotateDirection,0x14C);
struct JPADrawSweepShapeVtable_l {
    u8 mBeforeGetType[0x14];
    be<u32> getType;
    u8 mBeforeGetDirType[0x4];
    be<u32> getDirType;
    u8 mBeforeGetRotType[0x4];
    be<u32> getRotType;
    u8 mBeforeGetBasePlaneType[0x4];
    be<u32> getBasePlaneType;
    u8 mBeforeGetLife[0x4];
    be<u32> getLife;
    u8 mBeforeGetRate[0x4];
    be<u32> getRate;
    u8 mBeforeGetTiming[0x4];
    be<u32> getTiming;
    u8 mBeforeGetStep[0x4];
    be<u32> getStep;
    u8 mBeforeGetPosRndm[0x4];
    be<u32> getPosRndm;
    u8 mBeforeGetVelInfRate[0x4];
    be<u32> getVelInfRate;
    u8 mBeforeGetBaseVel[0x4];
    be<u32> getBaseVel;
    u8 mBeforeGetBaseVelRndm[0x4];
    be<u32> getBaseVelRndm;
    u8 mBeforeGetGravity[0x4];
    be<u32> getGravity;
    u8 mBeforeIsEnableField[0x4];
    be<u32> isEnableField;
    u8 mBeforeIsEnableDrawParent[0x4];
    be<u32> isEnableDrawParent;
    u8 mBeforeIsClipOn[0x4];
    be<u32> isClipOn;
    u8 mBeforeIsEnableScaleOut[0x4];
    be<u32> isEnableScaleOut;
    u8 mBeforeIsEnableAlphaOut[0x4];
    be<u32> isEnableAlphaOut;
    u8 mBeforeIsEnableRotate[0x4];
    be<u32> isEnableRotate;
    u8 mBeforeIsInheritedScale[0x4];
    be<u32> isInheritedScale;
    u8 mBeforeIsInheritedAlpha[0x4];
    be<u32> isInheritedAlpha;
    u8 mBeforeIsInheritedRGB[0x4];
    be<u32> isInheritedRGB;
    u8 mBeforeGetTextureIndex[0x4];
    be<u32> getTextureIndex;
    u8 mBeforeGetScaleX[0x4];
    be<u32> getScaleX;
    u8 mBeforeGetScaleY[0x4];
    be<u32> getScaleY;
    u8 mBeforeGetPrm[0x4];
    be<u32> getPrm;
    u8 mBeforeGetEnv[0x4];
    be<u32> getEnv;
    u8 mBeforeGetPrmAlpha[0x4];
    be<u32> getPrmAlpha;
    u8 mBeforeGetEnvAlpha[0x4];
    be<u32> getEnvAlpha;
    u8 mBeforeGetRotateSpeed[0x4];
    be<u32> getRotateSpeed;
    u8 mBeforeGetInheritScale[0x4];
    be<u32> getInheritScale;
    u8 mBeforeGetInheritAlpha[0x4];
    be<u32> getInheritAlpha;
    u8 mBeforeGetInheritRGB[0x4];
    be<u32> getInheritRGB;
};
struct JPADrawSweepShape_l {
    gptr<JPADrawSweepShapeVtable_l> __vtbl;
    u32 getType() { return gabi::call_ptr<u32>(__vtbl->getType,this); }
    u32 getDirType() { return gabi::call_ptr<u32>(__vtbl->getDirType,this); }
    u32 getRotType() { return gabi::call_ptr<u32>(__vtbl->getRotType,this); }
    u32 getBasePlaneType() { return gabi::call_ptr<u32>(__vtbl->getBasePlaneType,this); }
    u32 getLife() { return gabi::call_ptr<u32>(__vtbl->getLife,this); }
    u32 getRate() { return gabi::call_ptr<u32>(__vtbl->getRate,this); }
    f32 getTiming() { return gabi::call_ptr<f32>(__vtbl->getTiming,this); }
    u32 getStep() { return gabi::call_ptr<u32>(__vtbl->getStep,this); }
    f32 getPosRndm() { return gabi::call_ptr<f32>(__vtbl->getPosRndm,this); }
    f32 getVelInfRate() { return gabi::call_ptr<f32>(__vtbl->getVelInfRate,this); }
    f32 getBaseVel() { return gabi::call_ptr<f32>(__vtbl->getBaseVel,this); }
    f32 getBaseVelRndm() { return gabi::call_ptr<f32>(__vtbl->getBaseVelRndm,this); }
    f32 getGravity() { return gabi::call_ptr<f32>(__vtbl->getGravity,this); }
    u32 isEnableField() { return gabi::call_ptr<u32>(__vtbl->isEnableField,this); }
    u32 isEnableDrawParent() { return gabi::call_ptr<u32>(__vtbl->isEnableDrawParent,this); }
    u32 isClipOn() { return gabi::call_ptr<u32>(__vtbl->isClipOn,this); }
    u32 isEnableScaleOut() { return gabi::call_ptr<u32>(__vtbl->isEnableScaleOut,this); }
    u32 isEnableAlphaOut() { return gabi::call_ptr<u32>(__vtbl->isEnableAlphaOut,this); }
    u32 isEnableRotate() { return gabi::call_ptr<u32>(__vtbl->isEnableRotate,this); }
    u32 isInheritedScale() { return gabi::call_ptr<u32>(__vtbl->isInheritedScale,this); }
    u32 isInheritedAlpha() { return gabi::call_ptr<u32>(__vtbl->isInheritedAlpha,this); }
    u32 isInheritedRGB() { return gabi::call_ptr<u32>(__vtbl->isInheritedRGB,this); }
    u32 getTextureIndex() { return gabi::call_ptr<u32>(__vtbl->getTextureIndex,this); }
    f32 getScaleX() { return gabi::call_ptr<f32>(__vtbl->getScaleX,this); }
    f32 getScaleY() { return gabi::call_ptr<f32>(__vtbl->getScaleY,this); }
    u32 getPrm() { return gabi::call_ptr<u32>(__vtbl->getPrm,this); }
    u32 getEnv() { return gabi::call_ptr<u32>(__vtbl->getEnv,this); }
    u32 getPrmAlpha() { return gabi::call_ptr<u32>(__vtbl->getPrmAlpha,this); }
    u32 getEnvAlpha() { return gabi::call_ptr<u32>(__vtbl->getEnvAlpha,this); }
    f32 getRotateSpeed() { return gabi::call_ptr<f32>(__vtbl->getRotateSpeed,this); }
    f32 getInheritScale() { return gabi::call_ptr<f32>(__vtbl->getInheritScale,this); }
    f32 getInheritAlpha() { return gabi::call_ptr<f32>(__vtbl->getInheritAlpha,this); }
    f32 getInheritRGB() { return gabi::call_ptr<f32>(__vtbl->getInheritRGB,this); }
};
WWHD_OFFSET(JPADrawSweepShapeVtable_l,getType,0x14);
WWHD_OFFSET(JPADrawSweepShapeVtable_l,getDirType,0x1C);
WWHD_OFFSET(JPADrawSweepShapeVtable_l,getRotType,0x24);
WWHD_OFFSET(JPADrawSweepShapeVtable_l,getBasePlaneType,0x2C);
WWHD_OFFSET(JPADrawSweepShapeVtable_l,getLife,0x34);
WWHD_OFFSET(JPADrawSweepShapeVtable_l,getRate,0x3C);
WWHD_OFFSET(JPADrawSweepShapeVtable_l,getTiming,0x44);
WWHD_OFFSET(JPADrawSweepShapeVtable_l,getStep,0x4C);
WWHD_OFFSET(JPADrawSweepShapeVtable_l,getPosRndm,0x54);
WWHD_OFFSET(JPADrawSweepShapeVtable_l,getVelInfRate,0x5C);
WWHD_OFFSET(JPADrawSweepShapeVtable_l,getBaseVel,0x64);
WWHD_OFFSET(JPADrawSweepShapeVtable_l,getBaseVelRndm,0x6C);
WWHD_OFFSET(JPADrawSweepShapeVtable_l,getGravity,0x74);
WWHD_OFFSET(JPADrawSweepShapeVtable_l,isEnableField,0x7C);
WWHD_OFFSET(JPADrawSweepShapeVtable_l,isEnableDrawParent,0x84);
WWHD_OFFSET(JPADrawSweepShapeVtable_l,isClipOn,0x8C);
WWHD_OFFSET(JPADrawSweepShapeVtable_l,isEnableScaleOut,0x94);
WWHD_OFFSET(JPADrawSweepShapeVtable_l,isEnableAlphaOut,0x9C);
WWHD_OFFSET(JPADrawSweepShapeVtable_l,isEnableRotate,0xA4);
WWHD_OFFSET(JPADrawSweepShapeVtable_l,isInheritedScale,0xAC);
WWHD_OFFSET(JPADrawSweepShapeVtable_l,isInheritedAlpha,0xB4);
WWHD_OFFSET(JPADrawSweepShapeVtable_l,isInheritedRGB,0xBC);
WWHD_OFFSET(JPADrawSweepShapeVtable_l,getTextureIndex,0xC4);
WWHD_OFFSET(JPADrawSweepShapeVtable_l,getScaleX,0xCC);
WWHD_OFFSET(JPADrawSweepShapeVtable_l,getScaleY,0xD4);
WWHD_OFFSET(JPADrawSweepShapeVtable_l,getPrm,0xDC);
WWHD_OFFSET(JPADrawSweepShapeVtable_l,getEnv,0xE4);
WWHD_OFFSET(JPADrawSweepShapeVtable_l,getPrmAlpha,0xEC);
WWHD_OFFSET(JPADrawSweepShapeVtable_l,getEnvAlpha,0xF4);
WWHD_OFFSET(JPADrawSweepShapeVtable_l,getRotateSpeed,0xFC);
WWHD_OFFSET(JPADrawSweepShapeVtable_l,getInheritScale,0x104);
WWHD_OFFSET(JPADrawSweepShapeVtable_l,getInheritAlpha,0x10C);
WWHD_OFFSET(JPADrawSweepShapeVtable_l,getInheritRGB,0x114);
}
