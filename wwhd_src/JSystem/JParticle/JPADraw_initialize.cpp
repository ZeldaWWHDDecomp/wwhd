// Draw initialization starts from tww JPADraw::initialize, followed by HD shader selection.
#include "JSystem/JParticle/JPADraw_initialize.h"
#include "JSystem/JParticle/JPA_emitter_calc.h"
using namespace JPADrawExtension;
static const u32 sInitializeSafeStringVtable;
u32 JPADrawEmitterViewIndex(jpa_emitter_calc::JPABaseEmitter*);
void JPADrawInitializeBeforeCallbacks(JPADraw_l*,const JPADrawVisitorDefFlags_l&);
void JPADrawInitializeAfterCallbacks(JPADraw_l*,const JPADrawVisitorDefFlags_l&);
void JPADrawInitializeCalcVisitors(JPADraw_l*,const JPADrawVisitorDefFlags_l&);
void JPADrawInitializeTextureName(JPADraw_l*,SafeString*,u32);
jpa_emitter_extension::JPAShaderMaterialSet* JPADrawGetProgramArchive();
s32 JPADrawFindProgram(jpa_emitter_extension::JPAShaderProgram*,const SafeString*);
void JPADrawPrepareProgram(jpa_emitter_extension::JPAShaderMaterial*,jpa_emitter_extension::JPAShaderResource*,u32);
void JPADrawInitializeAssert(const char*,s32,const char*);
/* 02829F10 */
bool JPADraw_l::initialize(jpa_emitter_calc::JPABaseEmitter* emtr,JPAMathDraw::JPATextureResource* texRes) {
    u32 view = JPADrawEmitterViewIndex(emtr);
    dc.mpAlternateClipBoards[1] = &drawAlternateClipBoards()[1];
    dc.mpAlternateClipBoards[0] = &drawAlternateClipBoards()[0];
    dc.pbe = reinterpret_cast<JPAMathDraw::JPABaseEmitterHD*>(emtr);
    dc.pcb = gabi::ea(&drawViewClipBoards()[view]);
    dc.pbsp = reinterpret_cast<JPAMathDraw::JPABaseShape*>(emtr->mpDataLinkInfo->mpBaseShape.get());
    dc.pesp = reinterpret_cast<JPAMathDraw::JPAExtraShape*>(emtr->mpDataLinkInfo->mpExtraShape.get());
    dc.pssp = reinterpret_cast<JPAMathDraw::JPASweepShape*>(emtr->mpDataLinkInfo->mpSweepShape.get());
    dc.mpTextureResource = texRes;
    dc.petx = reinterpret_cast<JPAMathDraw::JPAExTexShape*>(emtr->mpDataLinkInfo->mpExTexShape.get());
    dc.mpDraw = gabi::ea(this);
    dc.mpActiveParticles = gabi::ea(&reinterpret_cast<jpa_emitter_extension::JPABaseEmitter*>(emtr)->particles().mActive);
    dc.pTexIdx = emtr->mpDataLinkInfo->textureMap.get();
    mDrawFlags = 0;
    mScaleOut = 1.0f;
    if (baseShape()->isEnablePrm()) {
        if (!baseShape()->isEnablePrmAnm()) mPrmColor = baseShape()->getPrmColor();
    } else mPrmColor = 0xFFFFFFFF;
    if (baseShape()->isEnableEnv()) {
        if (!baseShape()->isEnableEnvAnm()) mEnvColor = baseShape()->getEnvColor();
    } else mEnvColor = 0xFFFFFFFF;
    execEmtrCVisNum = 0;
    execEmtrVisNum = 0;
    mpExecEmtrVis[0] = nullptr;
    execEmtrPVisNum = 0;
    execPtclVisNum = 0;
    execChldVisNum = 0;
    calcChldVisNum = 0;
    calcEmtrVisNum = 0;
    calcPtclVisNum = 0;
    for (u32 i = 0; i < 5; ++i) mpExecEmtrPVis[i] = nullptr;
    for (u32 i = 0; i < 3; ++i) mpExecEmtrCVis[i] = nullptr;
    for (u32 i = 0; i < 4; ++i) mpCalcEmtrVis[i] = nullptr;
    for (u32 i = 0; i < 5; ++i) mpExecPtclVis[i] = nullptr;
    for (u32 i = 0; i < 4; ++i) mpExecChldVis[i] = nullptr;
    for (u32 i = 0; i < 10; ++i) mpCalcPtclVis[i] = nullptr;
    for (u32 i = 0; i < 2; ++i) mpCalcChldVis[i] = nullptr;
    hdExecChldVisNum = 0;
    hdExecPtclVisNum = 0;
    hdExecEmtrCVisNum = 0;
    hdExecEmtrPVisNum = 0;
    for (u32 i = 0; i < 2; ++i) mpHDExecEmtrPVis[i] = nullptr;
    for (u32 i = 0; i < 2; ++i) mpHDExecEmtrCVis[i] = nullptr;
    for (u32 i = 0; i < 2; ++i) mpHDExecPtclVis[i] = nullptr;
    for (u32 i = 0; i < 2; ++i) mpHDExecChldVis[i] = nullptr;
    JPADrawVisitorDefFlags_l flags;
    flags.mbIsEnableDrawParent = !sweepShape() || sweepShape()->isEnableDrawParent();
    flags.mbHasPrmAnm = baseShape()->isEnablePrm() && baseShape()->isEnablePrmAnm();
    flags.mbHasEnvAnm = baseShape()->isEnableEnv() && baseShape()->isEnableEnvAnm();
    flags.mbIsStripe = baseShape()->getType() == DrawStripe || baseShape()->getType() == DrawStripeCross;
    flags.mbIsPointOrLine = baseShape()->getType() == DrawPoint || baseShape()->getType() == DrawLine;
    flags.mbIsEnableAlpha = extraShape() && extraShape()->isEnableAlpha();
    JPADrawInitializeBeforeCallbacks(this,flags);
    auto& vc = drawVisitors();
    mpExecEmtrPVis[u8(execEmtrPVisNum)] = &vc.mExecCallBack.emitter;
    execEmtrPVisNum = u8(execEmtrPVisNum) + 1;
    mpExecEmtrCVis[u8(execEmtrCVisNum)] = &vc.mExecCallBack.emitter;
    execEmtrCVisNum = u8(execEmtrCVisNum) + 1;
    mpExecPtclVis[u8(execPtclVisNum)] = &vc.mExecCallBack.particle;
    execPtclVisNum = u8(execPtclVisNum) + 1;
    mpExecChldVis[u8(execChldVisNum)] = &vc.mExecCallBack.particle;
    execChldVisNum = u8(execChldVisNum) + 1;
    mpHDExecEmtrPVis[u8(hdExecEmtrPVisNum)] = &vc.mHDExecPair.emitter;
    hdExecEmtrPVisNum = u8(hdExecEmtrPVisNum) + 1;
    mpHDExecEmtrCVis[u8(hdExecEmtrCVisNum)] = &vc.mHDExecPair.emitter;
    hdExecEmtrCVisNum = u8(hdExecEmtrCVisNum) + 1;
    mpHDExecPtclVis[u8(hdExecPtclVisNum)] = &vc.mHDExecPair.particle;
    hdExecPtclVisNum = u8(hdExecPtclVisNum) + 1;
    mpHDExecChldVis[u8(hdExecChldVisNum)] = &vc.mHDExecPair.particle;
    hdExecChldVisNum = u8(hdExecChldVisNum) + 1;
    JPADrawInitializeAfterCallbacks(this,flags);
    JPADrawInitializeCalcVisitors(this,flags);
    emtr = reinterpret_cast<jpa_emitter_calc::JPABaseEmitter*>(dc.pbe.get());
    auto linkInfo = emtr->mpDataLinkInfo.get();
    u32 textureIndex = baseShape()->getTextureIndex();
    SafeString textureName;
    JPADrawInitializeTextureName(this,&textureName,u16(linkInfo->textureMap.get()[textureIndex]));
    mTextureCopyEnabled = 0;
    auto sameTexture = [&](SafeString* rhs) {
        auto vtable = gabi::at<JPADrawSafeStringVtable_l>(u32(textureName.__vtbl));
        gabi::call_ptr<void>(vtable->mAssureTermination,&textureName);
        vtable = gabi::at<JPADrawSafeStringVtable_l>(u32(textureName.__vtbl));
        gabi::call_ptr<void>(vtable->mAssureTermination,&textureName);
        const be<u8>* lhsText = gabi::at<const be<u8>>(u32(textureName.mTextPtr));
        vtable = gabi::at<JPADrawSafeStringVtable_l>(u32(rhs->__vtbl));
        gabi::call_ptr<void>(vtable->mAssureTermination,rhs);
        const be<u8>* rhsText = gabi::at<const be<u8>>(u32(rhs->mTextPtr));
        if (lhsText == rhsText) return true;
        for (u32 i = 0; i < 0x40001; ++i) {
            u8 left = u8(lhsText[i]),right = u8(rhsText[i]);
            if (left != right) return false;
            if (!left) return true;
        }
        return false;
    };
    // HD: program records use the base record as the out-of-range fallback.
    auto resolveProgram = [&](const char* name) {
        SafeString key;
        key.__vtbl = gabi::ea(&sInitializeSafeStringVtable);
        key.mTextPtr = gabi::guest_addr(name);
        auto archive = JPADrawGetProgramArchive();
        s32 index = JPADrawFindProgram(archive->mProgram.get(),&key);
        if (index < 0) return static_cast<jpa_emitter_extension::JPAShaderMaterial*>(nullptr);
        u32 count = archive->mCount;
        auto programs = archive->mMaterials.get();
        auto program = u32(index) < count ? &programs[index] : programs;
        if (!u8(program->mReady)) {
            auto resource = archive->mProgram.get();
            jpa_emitter_extension::JPAShaderResource* data = nullptr;
            if (u32(index) < u32(resource->mResourceCount)) data = &resource->mResources.get()[index];
            JPADrawPrepareProgram(program,data,0);
            count = archive->mCount;
            programs = archive->mMaterials.get();
        }
        return u32(index) < count ? &programs[index] : programs;
    };
    SafeString kageroName;
    kageroName.__vtbl = gabi::ea(&sInitializeSafeStringVtable);
    kageroName.mTextPtr = gabi::guest_addr("AK_kagerouSwap00");
    if (sameTexture(&kageroName)) {
        if (dc.petx.get() && dc.petx->getIndTexMode()) {
            bool secondTexture = dc.petx->isEnableSecondTex() != 0;
            auto destination = reinterpret_cast<jpa_emitter_calc::JPABaseEmitter*>(dc.pbe.get());
            auto program = resolveProgram(secondTexture ? "kagero" : "kagero_nomask");
            destination->mGpuState.mDrawContext = gabi::ea(program);
            mTextureCopyEnabled = 1;
        }
    } else {
        SafeString tailName;
        tailName.__vtbl = gabi::ea(&sInitializeSafeStringVtable);
        tailName.mTextPtr = gabi::guest_addr("AK_shipTailMain");
        if (sameTexture(&tailName)) {
            auto destination = reinterpret_cast<jpa_emitter_calc::JPABaseEmitter*>(dc.pbe.get());
            auto program = resolveProgram("track_effect");
            destination->mGpuState.mDrawContext = gabi::ea(program);
        }
    }
    emtr = reinterpret_cast<jpa_emitter_calc::JPABaseEmitter*>(dc.pbe.get());
    u16 resourceIndex = emtr->mGpuState.mResourceIndex;
    if (resourceIndex == 0x461 || resourceIndex == 0x462) {
        auto program = resolveProgram("jparticle_fire");
        emtr->mGpuState.mDrawContext = gabi::ea(program);
        emtr = reinterpret_cast<jpa_emitter_calc::JPABaseEmitter*>(dc.pbe.get());
    }
    emtr->mGpuState.mTextureCopyEnabled = u8(mTextureCopyEnabled);
    if (u8(execEmtrVisNum) > 1) JPADrawInitializeAssert("JPADraw.cpp",0x11D,"execEmtrVisNum <= 1");
    if (u8(execEmtrPVisNum) > 5) JPADrawInitializeAssert("JPADraw.cpp",0x11E,"execEmtrPVisNum <= 5");
    if (u8(execEmtrCVisNum) > 3) JPADrawInitializeAssert("JPADraw.cpp",0x11F,"execEmtrCVisNum <= 3");
    if (u8(calcEmtrVisNum) > 4) JPADrawInitializeAssert("JPADraw.cpp",0x120,"calcEmtrVisNum <= 4");
    if (u8(execPtclVisNum) > 5) JPADrawInitializeAssert("JPADraw.cpp",0x121,"execPtclVisNum <= 5");
    if (u8(execChldVisNum) > 4) JPADrawInitializeAssert("JPADraw.cpp",0x122,"execChldVisNum <= 4");
    if (u8(calcPtclVisNum) > 10) JPADrawInitializeAssert("JPADraw.cpp",0x123,"calcPtclVisNum <= 10");
    if (u8(calcChldVisNum) > 2) JPADrawInitializeAssert("JPADraw.cpp",0x124,"calcChldVisNum <= 2");
    return true;
}
/* WWHD glue
data sInitializeSafeStringVtable 10171F28
str 10171F44
str 10171F4C
str 10171F5C
str 10171F68
str 10171F7C
str 10171F90
str 10171FA4
str 10171FB8
str 10171FCC
str 10171FDC
str 10171FF0
str 10172000
str 10172010
str 10172028
str 10172040
ext 0281E950 u32 JPADrawEmitterViewIndex(jpa_emitter_calc::JPABaseEmitter*)
ext 02828308 void JPADrawInitializeBeforeCallbacks(JPADraw_l*,const JPADrawVisitorDefFlags_l&)
ext 02828A70 void JPADrawInitializeAfterCallbacks(JPADraw_l*,const JPADrawVisitorDefFlags_l&)
ext 028294B4 void JPADrawInitializeCalcVisitors(JPADraw_l*,const JPADrawVisitorDefFlags_l&)
ext 02829EAC void JPADrawInitializeTextureName(JPADraw_l*,SafeString*,u32)
ext 027FFCBC jpa_emitter_extension::JPAShaderMaterialSet* JPADrawGetProgramArchive()
ext 027B90AC s32 JPADrawFindProgram(jpa_emitter_extension::JPAShaderProgram*,const SafeString*)
ext 02800B0C void JPADrawPrepareProgram(jpa_emitter_extension::JPAShaderMaterial*,jpa_emitter_extension::JPAShaderResource*,u32)
ext 0273AA24 void JPADrawInitializeAssert(const char*,s32,const char*)
*/
