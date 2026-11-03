// Borrowed runtime storage views pending canonical dependency adoption; first-use tables are TU-specific.
#include "agl/RenderSelection.h"
namespace agl {
struct SelectionIdentityWord_l { be<u32> mValue; };
WWHD_SIZE(SelectionIdentityWord_l, 4);
static SelectionIdentityWord_l sSelectionTypeGuard;
static sead::RuntimeType sSelectionType;
static SelectionIdentityWord_l sSelectionTypeTable;
static SelectionIdentityWord_l sSelectionBaseTypeGuard;
static sead::RuntimeType sSelectionBaseType;
static SelectionIdentityWord_l sSelectionBaseTypeTable;
static SelectionIdentityWord_l sSelectionRootTypeTable;
/* 027D1110 */
u32 agl::RenderSelection_l::matchesRuntimeType(const sead::RuntimeType* type) const {
    if (sSelectionTypeGuard.mValue == 0) {
        sSelectionTypeGuard.mValue = 1;
        sSelectionType.mTable = &sSelectionTypeTable;
    }
    if (type == &sSelectionType)
        return 1;
    if (sSelectionBaseTypeGuard.mValue == 0) {
        sSelectionBaseTypeGuard.mValue = 1;
        sSelectionBaseType.mTable = &sSelectionBaseTypeTable;
    }
    if (type == &sSelectionBaseType)
        return 1;
    // The common agl base type (sead/RuntimeStatics.cpp), first initialised by whichever caller runs first.
    if (sAglCommonBaseGuard == 0) {
        sAglCommonBaseGuard = 1;
        sAglCommonBaseType.mTable = &sSelectionRootTypeTable;
    }
    return type == &sAglCommonBaseType;
}
}
/* WWHD glue
data sSelectionTypeGuard 101FD808
data sSelectionType 101FDFE4
data sSelectionTypeTable 1016A888
data sSelectionBaseTypeGuard 101FD8D4
data sSelectionBaseType 101FDFF0
data sSelectionBaseTypeTable 1016A8C8
data sSelectionRootTypeTable 1016A848
*/
