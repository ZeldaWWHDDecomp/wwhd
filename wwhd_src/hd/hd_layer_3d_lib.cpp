/* agl render-layer instance emitted in the hd_layer_3d translation unit (HDTU-LIB): the name of a
 * render step, a SafeString returned by value from a function-local table of 2 name(s) that the
 * first call copies out of the initialised data.
 * Reconstruction from the game instructions; */
#include "bindings.h"
#include "sead/HdTuInstances.h"

static u32 sRenderStepNamesGuard;
static char* sRenderStepNames[2] = {};  // filled on first use
static const char* sRenderStepNameData[] = {"Layout", "Debug"};
static sead::SafeTextVtable sSafeStringVtable;

/* 0272FA70 */
sead::SafeText* sead::instances::layer_3d::Layer::getRenderStepName(SafeText* name, s32 step) const {
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
data sRenderStepNamesGuard 101FDDD0 nocheck
data sRenderStepNames 101FDE5C nocheck
data sRenderStepNameData 101F87C4
data sSafeStringVtable 10141BFC nocheck
*/
