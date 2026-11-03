// Nullable construction of a referenced string and an otherwise untouched storage block.
#include "agl/EnvironmentConstructionViews.h"
using namespace agl::env;
static sead::SafeTextVtable sEnvironmentReferencedTextMethods;
void* allocateEnvironmentConstructionStorage(u32);

/* 0271B1AC */
EnvironmentReferencedText_l* EnvironmentReferencedText_l::construct(EnvironmentReferencedText_l* object,be<u8>* text) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (object == NULL) object = static_cast<EnvironmentReferencedText_l*>(allocateEnvironmentConstructionStorage(sizeof(EnvironmentReferencedText_l)));
    if (object != NULL) {
        object->mText = text;
        object->mVtable = &sEnvironmentReferencedTextMethods;
    }
    return object;
}
/* 02738024 */
EnvironmentAllocationStorage_l* EnvironmentAllocationStorage_l::construct(EnvironmentAllocationStorage_l* object) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (object == NULL) object = static_cast<EnvironmentAllocationStorage_l*>(allocateEnvironmentConstructionStorage(sizeof(EnvironmentAllocationStorage_l)));
    return object;
}
/* WWHD glue
 ext 0273AD10 void* allocateEnvironmentConstructionStorage(u32)
 data sEnvironmentReferencedTextMethods 1013F708
*/
