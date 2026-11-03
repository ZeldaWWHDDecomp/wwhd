// JPA1 visitor selection starts from tww JPADraw.cpp, using Cafe virtual slots.
#include "JSystem/JParticle/JPADraw.h"
using namespace JPADrawExtension;
static JPAMathDraw::JPACalcVisitor* calcVisitor(JPADrawVisitor_l* visitor) {
    return reinterpret_cast<JPAMathDraw::JPACalcVisitor*>(visitor);
}
enum JPACalcAnimationType { CalcNormal,CalcRepeat,CalcReverse,CalcMerge,CalcRandom };
enum JPAAlphaWaveType { AlphaNormal,AlphaAdd,AlphaMultiply };
/* 028294B4 */
void JPADraw_l::setDrawCalcVisitors(const JPADrawVisitorDefFlags_l& flags) {
    auto& vc = drawVisitors();
    if (baseShape()->isEnableGlobalColAnm() && (s32(flags.mbHasPrmAnm) || s32(flags.mbHasEnvAnm))) {
        switch (baseShape()->getColorRegAnmType()) {
        case CalcNormal: mpCalcEmtrVis[calcEmtrVisNum++] = calcVisitor(&vc.mCalcColorAnmFrameNormal.emitter); break;
        case CalcRepeat: mpCalcEmtrVis[calcEmtrVisNum++] = calcVisitor(&vc.mCalcColorAnmFrameRepeat.emitter); break;
        case CalcReverse: mpCalcEmtrVis[calcEmtrVisNum++] = calcVisitor(&vc.mCalcColorAnmFrameReverse.emitter); break;
        case CalcMerge: mpCalcEmtrVis[calcEmtrVisNum++] = calcVisitor(&vc.mCalcColorAnmFrameMerge.emitter); break;
        case CalcRandom: mpCalcEmtrVis[calcEmtrVisNum++] = calcVisitor(&vc.mCalcColorAnmFrameRandom.emitter); break;
        }
        if (s32(flags.mbHasPrmAnm)) mpCalcEmtrVis[calcEmtrVisNum++] = calcVisitor(&vc.mCalcColorPrm.emitter);
        if (s32(flags.mbHasEnvAnm)) mpCalcEmtrVis[calcEmtrVisNum++] = calcVisitor(&vc.mCalcColorEnv.emitter);
    }
    if (!baseShape()->textureIsEmpty() && baseShape()->isEnableTextureAnm() && baseShape()->isEnableGlobalTexAnm()) {
        switch (baseShape()->getTextureAnmType()) {
        case CalcNormal: mpCalcEmtrVis[calcEmtrVisNum++] = calcVisitor(&vc.mCalcTextureAnmIndexNormal.emitter); break;
        case CalcRepeat: mpCalcEmtrVis[calcEmtrVisNum++] = calcVisitor(&vc.mCalcTextureAnmIndexRepeat.emitter); break;
        case CalcReverse: mpCalcEmtrVis[calcEmtrVisNum++] = calcVisitor(&vc.mCalcTextureAnmIndexReverse.emitter); break;
        case CalcMerge: mpCalcEmtrVis[calcEmtrVisNum++] = calcVisitor(&vc.mCalcTextureAnmIndexMerge.emitter); break;
        case CalcRandom: mpCalcEmtrVis[calcEmtrVisNum++] = calcVisitor(&vc.mCalcTextureAnmIndexRandom.emitter); break;
        }
    }
    if (extraShape() && extraShape()->isEnableScale()) {
        if (extraShape()->isEnableScaleAnmX()) {
            if (extraShape()->getAnmTypeX()) mpCalcPtclVis[calcPtclVisNum++] = calcVisitor(&vc.mCalcScaleAnmTimingReverseX);
            else mpCalcPtclVis[calcPtclVisNum++] = calcVisitor(&vc.mCalcScaleAnmTimingRepeatX);
        } else mpCalcPtclVis[calcPtclVisNum++] = calcVisitor(&vc.mCalcScaleAnmTimingNormal);
        if (extraShape()->isEnableScaleBySpeedX() && baseShape()->getType() != DrawLine)
            mpCalcPtclVis[calcPtclVisNum++] = calcVisitor(&vc.mCalcScaleXBySpeed);
        else mpCalcPtclVis[calcPtclVisNum++] = calcVisitor(&vc.mCalcScaleX);
        if (baseShape()->getType() != DrawPoint) {
            if (!extraShape()->isDiffXY()) mpCalcPtclVis[calcPtclVisNum++] = calcVisitor(&vc.mCalcScaleCopyX2Y);
            else {
                if (extraShape()->isEnableScaleAnmY()) {
                    if (extraShape()->getAnmTypeY()) mpCalcPtclVis[calcPtclVisNum++] = calcVisitor(&vc.mCalcScaleAnmTimingReverseY);
                    else mpCalcPtclVis[calcPtclVisNum++] = calcVisitor(&vc.mCalcScaleAnmTimingRepeatY);
                } else if (extraShape()->isEnableScaleAnmX()) mpCalcPtclVis[calcPtclVisNum++] = calcVisitor(&vc.mCalcScaleAnmTimingNormal);
                if (extraShape()->isEnableScaleBySpeedY()) mpCalcPtclVis[calcPtclVisNum++] = calcVisitor(&vc.mCalcScaleYBySpeed);
                else mpCalcPtclVis[calcPtclVisNum++] = calcVisitor(&vc.mCalcScaleY);
            }
        }
    }
    if (!baseShape()->isEnableGlobalColAnm()) {
        if (s32(flags.mbHasPrmAnm) || s32(flags.mbHasEnvAnm)) {
            switch (baseShape()->getColorRegAnmType()) {
            case CalcNormal: mpCalcPtclVis[calcPtclVisNum++] = calcVisitor(&vc.mCalcColorAnmFrameNormal.particle); break;
            case CalcRepeat: mpCalcPtclVis[calcPtclVisNum++] = calcVisitor(&vc.mCalcColorAnmFrameRepeat.particle); break;
            case CalcReverse: mpCalcPtclVis[calcPtclVisNum++] = calcVisitor(&vc.mCalcColorAnmFrameReverse.particle); break;
            case CalcMerge: mpCalcPtclVis[calcPtclVisNum++] = calcVisitor(&vc.mCalcColorAnmFrameMerge.particle); break;
            case CalcRandom: mpCalcPtclVis[calcPtclVisNum++] = calcVisitor(&vc.mCalcColorAnmFrameRandom.particle); break;
            }
            if (s32(flags.mbHasPrmAnm)) mpCalcPtclVis[calcPtclVisNum++] = calcVisitor(&vc.mCalcColorPrm.particle);
            if (s32(flags.mbHasEnvAnm)) mpCalcPtclVis[calcPtclVisNum++] = calcVisitor(&vc.mCalcColorEnv.particle);
        }
    } else mpCalcPtclVis[calcPtclVisNum++] = calcVisitor(&vc.mCalcColorCopyFromEmitter);
    if (s32(flags.mbIsEnableAlpha) && !s32(flags.mbIsStripe)) {
        mpCalcPtclVis[calcPtclVisNum++] = calcVisitor(&vc.mCalcAlpha);
        // HD: the binary retains a null-extra-shape check even when the alpha flag is set.
        if (extraShape() && extraShape()->isEnableSinWave()) {
            switch (extraShape()->getAlphaWaveType()) {
            case AlphaNormal: mpCalcPtclVis[calcPtclVisNum++] = calcVisitor(&vc.mCalcAlphaFlickNrmSin); break;
            case AlphaAdd: mpCalcPtclVis[calcPtclVisNum++] = calcVisitor(&vc.mCalcAlphaFlickAddSin); break;
            case AlphaMultiply: mpCalcPtclVis[calcPtclVisNum++] = calcVisitor(&vc.mCalcAlphaFlickMultSin); break;
            }
        }
    }
    if (!baseShape()->textureIsEmpty() && baseShape()->isEnableTextureAnm() && !baseShape()->isEnableGlobalTexAnm()) {
        switch (baseShape()->getTextureAnmType()) {
        case CalcNormal: mpCalcPtclVis[calcPtclVisNum++] = calcVisitor(&vc.mCalcTextureAnmIndexNormal.particle); break;
        case CalcRepeat: mpCalcPtclVis[calcPtclVisNum++] = calcVisitor(&vc.mCalcTextureAnmIndexRepeat.particle); break;
        case CalcReverse: mpCalcPtclVis[calcPtclVisNum++] = calcVisitor(&vc.mCalcTextureAnmIndexReverse.particle); break;
        case CalcMerge: mpCalcPtclVis[calcPtclVisNum++] = calcVisitor(&vc.mCalcTextureAnmIndexMerge.particle); break;
        case CalcRandom: mpCalcPtclVis[calcPtclVisNum++] = calcVisitor(&vc.mCalcTextureAnmIndexRandom.particle); break;
        }
    }
    if (sweepShape()) {
        if (sweepShape()->isEnableAlphaOut()) mpCalcChldVis[calcChldVisNum++] = calcVisitor(&vc.mCalcChildAlphaOut);
        if (sweepShape()->isEnableScaleOut()) mpCalcChldVis[calcChldVisNum++] = calcVisitor(&vc.mCalcChildScaleOut);
    }
}


/* 02828308 */
void JPADraw_l::setDrawExecVisitorsBeforeCB(const JPADrawVisitorDefFlags_l& flags) {
    auto& vc = drawVisitors();
    auto& e = execution();
    if (dc.petx.get()) e.mpExecEmtrVis[e.execEmtrVisNum++] = &vc.mLoadExTexture;
    if (s32(flags.mbIsPointOrLine)) e.mpExecEmtrPVis[e.execEmtrPVisNum++] = &vc.mGenIdtMtx;
    else if (baseShape()->isEnableProjection()) {
        if (baseShape()->isEnableTexScrollAnm()) e.mpExecEmtrPVis[e.execEmtrPVisNum++] = &vc.mGenPrjTexMtx;
        else e.mpExecEmtrPVis[e.execEmtrPVisNum++] = &vc.mGenPrjMtx;
    } else if (baseShape()->isEnableTexScrollAnm()) {
        if (s32(flags.mbIsStripe)) e.mpExecEmtrPVis[e.execEmtrPVisNum++] = &vc.mSetTexMtx.emitter;
        else e.mpExecEmtrPVis[e.execEmtrPVisNum++] = &vc.mGenTexMtx0;
    } else e.mpExecEmtrPVis[e.execEmtrPVisNum++] = &vc.mGenIdtMtx;

    if (baseShape()->isEnableGlobalColAnm()) {
        if (s32(flags.mbIsStripe) || !s32(flags.mbIsEnableAlpha)) e.mpExecEmtrPVis[e.execEmtrPVisNum++] = &vc.mRegisterColorPrmEnv;
        else if (s32(flags.mbIsEnableAlpha)) e.mpExecEmtrPVis[e.execEmtrPVisNum++] = &vc.mRegisterColorEnv;
    } else if ((!baseShape()->isEnablePrm() || !baseShape()->isEnablePrmAnm()) && !s32(flags.mbIsEnableAlpha)) {
        if (!baseShape()->isEnableEnv() || !baseShape()->isEnableEnvAnm()) e.mpExecEmtrPVis[e.execEmtrPVisNum++] = &vc.mRegisterColorPrmEnv;
        else e.mpExecEmtrPVis[e.execEmtrPVisNum++] = &vc.mRegisterColorPrm;
    } else if (!baseShape()->isEnableEnv() || !baseShape()->isEnableEnvAnm()) e.mpExecEmtrPVis[e.execEmtrPVisNum++] = &vc.mRegisterColorEnv;
    if (!baseShape()->textureIsEmpty() && (!baseShape()->isEnableTextureAnm() || baseShape()->isEnableGlobalTexAnm()))
        e.mpExecEmtrPVis[e.execEmtrPVisNum++] = &vc.mLoadTexture.emitter;
    else if (baseShape()->textureIsEmpty()) e.mpExecEmtrPVis[e.execEmtrPVisNum++] = &vc.mLoadDefaultTexture;
    if (sweepShape() && !sweepShape()->isEnableAlphaOut() && !sweepShape()->isInheritedAlpha() && !sweepShape()->isInheritedRGB())
        e.mpExecEmtrCVis[e.execEmtrCVisNum++] = &vc.mRegisterColorChildPrmEnv;
    if (!baseShape()->isEnableGlobalColAnm()) {
        if (s32(flags.mbHasPrmAnm)) {
            if (s32(flags.mbHasEnvAnm)) e.mpExecPtclVis[e.execPtclVisNum++] = &vc.mRegisterPrmColor;
            else e.mpExecPtclVis[e.execPtclVisNum++] = &vc.mRegisterPrmColorAnm;
        } else if (s32(flags.mbIsEnableAlpha)) {
            if (s32(flags.mbHasEnvAnm)) e.mpExecPtclVis[e.execPtclVisNum++] = &vc.mRegisterPrmAlpha;
            else e.mpExecPtclVis[e.execPtclVisNum++] = &vc.mRegisterPrmAlphaAnm;
        } else if (s32(flags.mbHasEnvAnm)) e.mpExecPtclVis[e.execPtclVisNum++] = &vc.mRegisterEnvColorAnm;
    } else if (s32(flags.mbIsEnableAlpha) && !s32(flags.mbIsStripe)) e.mpExecPtclVis[e.execPtclVisNum++] = &vc.mRegisterPrmAlphaAnm;
    if (!baseShape()->textureIsEmpty() && baseShape()->isEnableTextureAnm() && !baseShape()->isEnableGlobalTexAnm())
        e.mpExecPtclVis[e.execPtclVisNum++] = &vc.mLoadTexture.particle;
    switch (baseShape()->getType()) {
    case DrawPoint:
        if (extraShape() && extraShape()->isEnableScale()) e.mpExecPtclVis[e.execPtclVisNum++] = &vc.mSetPointSize;
        break;
    case DrawLine:
        if (extraShape() && extraShape()->isEnableScale()) e.mpExecPtclVis[e.execPtclVisNum++] = &vc.mSetLineWidth;
        break;
    case DrawStripe: case DrawStripeCross: break;
    case DrawBillboard: case DrawDirection: case DrawDirectionCross: case DrawRotation: case DrawRotationCross: case DrawDirBillboard: case DrawYBillboard:
        if (baseShape()->isEnableTexScrollAnm() && !baseShape()->isEnableProjection()) e.mpExecPtclVis[e.execPtclVisNum++] = &vc.mSetTexMtx.particle;
        break;
    }
    if (sweepShape()) {
        if (sweepShape()->isEnableAlphaOut() || sweepShape()->isInheritedAlpha() || sweepShape()->isInheritedRGB())
            e.mpExecChldVis[e.execChldVisNum++] = &vc.mRegisterPrmColor;
        switch (sweepShape()->getType()) {
        case DrawPoint: e.mpExecChldVis[e.execChldVisNum++] = &vc.mSetPointSize; break;
        case DrawLine: e.mpExecChldVis[e.execChldVisNum++] = &vc.mSetLineWidth; break;
        }
    }
}

/* 02828A70 */
void JPADraw_l::setDrawExecVisitorsAfterCB(const JPADrawVisitorDefFlags_l& flags) {
    auto& vc = drawVisitors();
    auto& e = execution();
    if (s32(flags.mbIsEnableDrawParent)) {
        switch (baseShape()->getType()) {
        case DrawPoint:
            e.mpExecPtclVis[e.execPtclVisNum++] = &vc.mExecPoint;
            e.mpHDExecPtclVis[e.hdExecPtclVisNum++] = &vc.mHDExec[HDPoint];
            break;
        case DrawLine: e.mpExecPtclVis[e.execPtclVisNum++] = &vc.mExecLine; break;
        case DrawBillboard:
            e.mpHDExecPtclVis[e.hdExecPtclVisNum++] = &vc.mHDExec[HDQuad];
            if (extraShape() && extraShape()->isEnableRotate()) e.mpExecPtclVis[e.execPtclVisNum++] = &vc.mExecRotBillBoard;
            else e.mpExecPtclVis[e.execPtclVisNum++] = &vc.mExecBillBoard;
            break;
        case DrawDirection:
            e.mpHDExecPtclVis[e.hdExecPtclVisNum++] = &vc.mHDExec[HDQuad];
            if (extraShape() && extraShape()->isEnableRotate()) e.mpExecPtclVis[e.execPtclVisNum++] = &vc.mExecRotDirectional;
            else e.mpExecPtclVis[e.execPtclVisNum++] = &vc.mExecDirectional;
            break;
        case DrawDirectionCross:
            e.mpHDExecPtclVis[e.hdExecPtclVisNum++] = &vc.mHDExec[HDCross];
            if (extraShape() && extraShape()->isEnableRotate()) e.mpExecPtclVis[e.execPtclVisNum++] = &vc.mExecRotDirectionalCross;
            else e.mpExecPtclVis[e.execPtclVisNum++] = &vc.mExecDirectionalCross;
            break;
        case DrawYBillboard:
            e.mpHDExecPtclVis[e.hdExecPtclVisNum++] = &vc.mHDExec[HDQuad];
            if (extraShape() && extraShape()->isEnableRotate()) e.mpExecPtclVis[e.execPtclVisNum++] = &vc.mExecRotYBillBoard;
            else e.mpExecPtclVis[e.execPtclVisNum++] = &vc.mExecYBillBoard;
            break;
        case DrawRotation:
            e.mpExecPtclVis[e.execPtclVisNum++] = &vc.mExecRotation;
            e.mpHDExecPtclVis[e.hdExecPtclVisNum++] = &vc.mHDExec[HDQuad];
            break;
        case DrawRotationCross:
            e.mpExecPtclVis[e.execPtclVisNum++] = &vc.mExecRotationCross;
            e.mpHDExecPtclVis[e.hdExecPtclVisNum++] = &vc.mHDExec[HDCross];
            break;
        case DrawDirBillboard:
            e.mpExecPtclVis[e.execPtclVisNum++] = &vc.mExecDirBillBoard;
            e.mpHDExecPtclVis[e.hdExecPtclVisNum++] = &vc.mHDExec[HDQuad];
            break;
        case DrawStripe:
            e.mpExecEmtrPVis[e.execEmtrPVisNum++] = &vc.mExecStripe;
            e.mpHDExecEmtrPVis[e.hdExecEmtrPVisNum++] = &vc.mHDExec[HDStripe];
            break;
        case DrawStripeCross:
            e.mpExecEmtrPVis[e.execEmtrPVisNum++] = &vc.mExecStripeCross;
            e.mpHDExecEmtrPVis[e.hdExecEmtrPVisNum++] = &vc.mHDExec[HDStripeCross];
            break;
        }
    }
    if (sweepShape()) {
        switch (sweepShape()->getType()) {
        case DrawPoint:
            e.mpExecChldVis[e.execChldVisNum++] = &vc.mExecPoint;
            e.mpHDExecChldVis[e.hdExecChldVisNum++] = &vc.mHDExec[HDPoint];
            break;
        case DrawLine: e.mpExecChldVis[e.execChldVisNum++] = &vc.mExecLine; break;
        case DrawBillboard:
            e.mpHDExecChldVis[e.hdExecChldVisNum++] = &vc.mHDExec[HDQuad];
            if ((extraShape() && extraShape()->isEnableRotate()) || sweepShape()->isEnableRotate()) e.mpExecChldVis[e.execChldVisNum++] = &vc.mExecRotBillBoard;
            else e.mpExecChldVis[e.execChldVisNum++] = &vc.mExecBillBoard;
            break;
        case DrawDirection:
            e.mpHDExecChldVis[e.hdExecChldVisNum++] = &vc.mHDExec[HDQuad];
            if ((extraShape() && extraShape()->isEnableRotate()) || sweepShape()->isEnableRotate()) e.mpExecChldVis[e.execChldVisNum++] = &vc.mExecRotDirectional;
            else e.mpExecChldVis[e.execChldVisNum++] = &vc.mExecDirectional;
            break;
        case DrawDirectionCross:
            e.mpHDExecChldVis[e.hdExecChldVisNum++] = &vc.mHDExec[HDCross];
            if ((extraShape() && extraShape()->isEnableRotate()) || sweepShape()->isEnableRotate()) e.mpExecChldVis[e.execChldVisNum++] = &vc.mExecRotDirectionalCross;
            else e.mpExecChldVis[e.execChldVisNum++] = &vc.mExecDirectionalCross;
            break;
        case DrawYBillboard:
            e.mpHDExecChldVis[e.hdExecChldVisNum++] = &vc.mHDExec[HDQuad];
            if ((extraShape() && extraShape()->isEnableRotate()) || sweepShape()->isEnableRotate()) e.mpExecChldVis[e.execChldVisNum++] = &vc.mExecRotYBillBoard;
            else e.mpExecChldVis[e.execChldVisNum++] = &vc.mExecYBillBoard;
            break;
        case DrawRotation:
            e.mpExecChldVis[e.execChldVisNum++] = &vc.mExecRotation;
            e.mpHDExecChldVis[e.hdExecChldVisNum++] = &vc.mHDExec[HDQuad];
            break;
        case DrawRotationCross:
            e.mpExecChldVis[e.execChldVisNum++] = &vc.mExecRotationCross;
            e.mpHDExecChldVis[e.hdExecChldVisNum++] = &vc.mHDExec[HDCross];
            break;
        case DrawDirBillboard:
            e.mpExecChldVis[e.execChldVisNum++] = &vc.mExecDirBillBoard;
            e.mpHDExecChldVis[e.hdExecChldVisNum++] = &vc.mHDExec[HDQuad];
            break;
        case DrawStripe:
            e.mpExecEmtrCVis[e.execEmtrCVisNum++] = &vc.mExecStripe;
            e.mpHDExecEmtrCVis[e.hdExecEmtrCVisNum++] = &vc.mHDExec[HDStripe];
            break;
        case DrawStripeCross:
            e.mpExecEmtrCVis[e.execEmtrCVisNum++] = &vc.mExecStripeCross;
            e.mpHDExecEmtrCVis[e.hdExecEmtrCVisNum++] = &vc.mHDExec[HDStripeCross];
            break;
        }
    }
}
/* WWHD glue: visitor and virtual-interface calls use the unit-owned native bindings. */
