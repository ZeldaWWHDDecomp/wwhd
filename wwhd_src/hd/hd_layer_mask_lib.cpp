/* agl render-layer instance emitted in the hd_layer_mask translation unit (HDTU-LIB): the name of a
 * render step, a SafeString returned by value from a function-local table of 17 name(s) that the
 * first call copies out of the initialised data.
 * Reconstruction from the game instructions; */
#include "bindings.h"
#include "sead/HdTuInstances.h"

static u32 sRenderStepNamesGuard;
static char* sRenderStepNames[17] = {};  // filled on first use
static const char* sRenderStepNameData[] = {"UpdateGPUBuffer", "LightPrePass", "SSAO", "ShadowPrePass", "Opa", "Xlu", "Monotone", "Debug", "DOF", "Bloom", "DisableBloom", "ColorCorrection", "FXAA", "Blur", "Capture", "PictureBox", "Text(Debug)"};
static sead::SafeTextVtable sSafeStringVtable;

/* 027338E0 */
sead::SafeText* sead::instances::layer_mask::Layer::getRenderStepName(SafeText* name, s32 step) const {
    if (sRenderStepNamesGuard == 0) {
        sRenderStepNamesGuard = 1;
        memcpy_g(sRenderStepNames, sRenderStepNameData, sizeof(sRenderStepNames));
    }
    const char* text = sRenderStepNames[step];
    if (name == NULL) {
        name = static_cast<SafeText*>(operator_new(sizeof(SafeText)));
    }
    if (name != NULL) {
        name->mText = reinterpret_cast<be<u8>*>(const_cast<char*>(text));
        name->mVtable = &sSafeStringVtable;
    }
    return name;
}

/* WWHD glue
data sRenderStepNamesGuard 101FD9B0 nocheck
data sRenderStepNames 101FDE64 nocheck
data sRenderStepNameData 101F87F0
data sSafeStringVtable 10141D98 nocheck
*/
