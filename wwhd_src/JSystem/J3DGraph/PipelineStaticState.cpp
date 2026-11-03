// Per-TU HD pipeline static state; borrowed data have no initializers.
#include "JSystem/J3DGraph/PipelineStaticState.h"
#include "bindings.h"
#include "sead/FileDevice.h"
using namespace j3d::pipeline;
static StaticWords_l sModelState;
static ExitRegistration_l sModelExit;
static StaticWords_l sResourceState;
static ExitRegistration_l sResourceExit;
static StaticWords_l sValueState;
static ExitRegistration_l sValueExit;
static StaticWords_l sDrawState;
static ExitRegistration_l sDrawExit;
static StaticWords_l sQueueState;
static ExitRegistration_l sQueueExit;
static StaticWords_l sSurfaceNameState;
static ExitRegistration_l sSurfaceNameExit;
struct SurfaceNameTable_l { sead::SafeText mNames[19]; };
WWHD_SIZE(SurfaceNameTable_l,19*8);
static SurfaceNameTable_l sSurfaceNames;
static sead::SafeTextVtable sSurfaceNameMethods;
void registerPipelineStatic(void*);
namespace j3d::pipeline {
/* 027F7498 */
void PipelineStaticState::initializeModelState() {
    gabi::AutoFrame frame{0x027F7498u};
    sModelState.mWords[2]=0;
    sModelState.mWords[0]=0;
    sModelState.mWords[3]=0;
    sModelState.mWords[1]=0;
    registerPipelineStatic(&sModelExit);
}
/* 027F7880 */
void PipelineStaticState::initializeResourceState() {
    gabi::AutoFrame frame{0x027F7880u};
    sResourceState.mWords[2]=0;
    sResourceState.mWords[0]=0;
    sResourceState.mWords[3]=0;
    sResourceState.mWords[1]=0;
    registerPipelineStatic(&sResourceExit);
}
/* 027F7A94 */
void PipelineStaticState::initializeValueState() {
    gabi::AutoFrame frame{0x027F7A94u};
    sValueState.mWords[2]=0;
    sValueState.mWords[0]=0;
    sValueState.mWords[3]=0;
    sValueState.mWords[1]=0;
    registerPipelineStatic(&sValueExit);
}
/* 027F7D50 */
void PipelineStaticState::initializeDrawState() {
    gabi::AutoFrame frame{0x027F7D50u};
    sDrawState.mWords[2]=0;
    sDrawState.mWords[0]=0;
    sDrawState.mWords[3]=0;
    sDrawState.mWords[1]=0;
    registerPipelineStatic(&sDrawExit);
}
/* 027F9C70 */
void PipelineStaticState::initializeQueueState() {
    gabi::AutoFrame frame{0x027F9C70u};
    sQueueState.mWords[2]=0;
    sQueueState.mWords[0]=0;
    sQueueState.mWords[3]=0;
    sQueueState.mWords[1]=0;
    registerPipelineStatic(&sQueueExit);
}
/* 027F8F14 */
void PipelineStaticState::initializeSurfaceNames() {
    gabi::AutoFrame frame{0x027F8F14u};
    sSurfaceNameState.mWords[3]=0;
    sSurfaceNameState.mWords[2]=0;
    sSurfaceNameState.mWords[1]=0;
    sSurfaceNameState.mWords[0]=0;
    registerPipelineStatic(&sSurfaceNameExit);
    const char* names[19]={
        "NormalizedLinearDepth",
        "HalfNLD",
        "HalfDepth",
        "GBuffer",
        "Fader",
        "FXAA",
        "Kagerou",
        "Pause",
        "MultiFilterDRC",
        "PictureBox",
        "PictureBoxZ",
        "PictureBoxTmp",
        "Bloom",
        "Blur",
        "TV",
        "Vrkumo",
        "VrkumoBloom",
        "DebugCaptureTV",
        "DebugCaptureDRC",
    };
    for(u32 index=0;index!=19;++index) {
        sSurfaceNames.mNames[index].mVtable=&sSurfaceNameMethods;
        sSurfaceNames.mNames[index].mText.v=gabi::guest_str(names[index]);
    }
}
}
/* WWHD glue
data sModelState 104B48FC
data sModelExit 101F992C
data sResourceState 104B490C
data sResourceExit 101F9938
data sValueState 104B491C
data sValueExit 101F9944
data sDrawState 104B492C
data sDrawExit 101F9950
data sQueueState 104B49E4
data sQueueExit 101F9970
data sSurfaceNameState 104B493C
data sSurfaceNameExit 101F995C
data sSurfaceNames 104B494C
data sSurfaceNameMethods 1016E988
str 1016EA98 "NormalizedLinearDepth"
str 1016EA24 "HalfNLD"
str 1016EAB0 "HalfDepth"
str 1016EA2C "GBuffer"
str 1016EA3C "Fader"
str 1016EA54 "FXAA"
str 1016EA34 "Kagerou"
str 1016EA44 "Pause"
str 1016EACC "MultiFilterDRC"
str 1016EADC "PictureBox"
str 1016EA80 "PictureBoxZ"
str 1016EABC "PictureBoxTmp"
str 1016EA4C "Bloom"
str 1016EA5C "Blur"
str 1016EA64 "TV"
str 1016EA68 "Vrkumo"
str 1016EA8C "VrkumoBloom"
str 1016EAE8 "DebugCaptureTV"
str 1016EA70 "DebugCaptureDRC"
ext 028F026C void registerPipelineStatic(void*)
*/
