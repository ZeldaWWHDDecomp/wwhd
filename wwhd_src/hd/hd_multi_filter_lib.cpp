/* agl render-layer instance emitted in the hd_multi_filter translation unit (HDTU-LIB): the name of a
 * render step, a SafeString returned by value from a function-local table of 3 name(s) that the
 * first call copies out of the initialised data.
 * Reconstruction from the game instructions; */
#include "bindings.h"
#include "sead/HdTuInstances.h"

static u32 sRenderStepNamesGuard;
static char* sRenderStepNames[3] = {};  // filled on first use
static const char* sRenderStepNameData[] = {"Fader", "Fader2", "Capture"};
static sead::SafeTextVtable sSafeStringVtable;

/* 027353FC */
sead::SafeText* sead::instances::multi_filter::Layer::getRenderStepName(SafeText* name, s32 step) const {
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
data sRenderStepNamesGuard 101FDDF8 nocheck
data sRenderStepNames 101FDECC nocheck
data sRenderStepNameData 101F88FC
data sSafeStringVtable 1014253C nocheck
*/
