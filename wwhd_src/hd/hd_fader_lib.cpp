/* agl render-layer instance emitted in the hd_fader translation unit (HDTU-LIB): the name of a
 * render step, a SafeString returned by value from a function-local table of 2 name(s) that the
 * first call copies out of the initialised data.
 * Reconstruction from the game instructions; */
#include "bindings.h"
#include "sead/HdTuInstances.h"

static u32 sRenderStepNamesGuard;
static char* sRenderStepNames[2] = {};  // filled on first use
static const char* sRenderStepNameData[] = {"ErrorViewer", "Debug"};
static sead::SafeTextVtable sSafeStringVtable;

/* 02734F20 */
sead::SafeText* sead::instances::fader::Layer::getRenderStepName(SafeText* name, s32 step) const {
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
data sRenderStepNamesGuard 101FD98C nocheck
data sRenderStepNames 101FDEC0 nocheck
data sRenderStepNameData 101F88D0
data sSafeStringVtable 101423DC nocheck
*/
