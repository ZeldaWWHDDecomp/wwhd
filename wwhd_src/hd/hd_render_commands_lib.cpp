/* agl render-layer instance emitted in the hd_render_commands translation unit (HDTU-LIB): the name of a
 * render step, a SafeString returned by value from a function-local table of 4 name(s) that the
 * first call copies out of the initialised data.
 * Reconstruction from the game instructions; */
#include "bindings.h"
#include "sead/HdTuInstances.h"

static u32 sRenderStepNamesGuard;
static char* sRenderStepNames[4] = {};  // filled on first use
static const char* sRenderStepNameData[] = {"Shadow", "UpdateGPUBuffer", "ShadowPrePass", "GBuffer"};
static sead::SafeTextVtable sSafeStringVtable;

/* 02738230 */
sead::SafeText* sead::instances::render_commands::Layer::getRenderStepName(SafeText* name, s32 step) const {
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
data sRenderStepNamesGuard 101FD9B8 nocheck
data sRenderStepNames 101FDEE8 nocheck
data sRenderStepNameData 101F8958
data sSafeStringVtable 10142828 nocheck
*/
