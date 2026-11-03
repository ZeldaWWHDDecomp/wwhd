/* Runtime-type checks of the light collections: each type node is set up on first use (guard word, then the
 * node's table) before it is compared by address; the derived type first, then the collection base, then the
 * common agl base.
 * References: WWHD's own instructions only. */
#include "agl/LightCollections.h"
using namespace agl;

typedef be<u32> RuntimeTypeGuard;
// The spatial light type and the three derived tables are this unit's own (data only, no initializer).
static RuntimeTypeGuard sSpatialExTypeGuard;
static sead::RuntimeType sSpatialExType;
static u8 sDirectedLightTypeTable;
static u8 sSpatialLightTypeTable;
static u8 sPairedLightTypeTable;
// Shared with other agl translation units: borrowed views with no initializer.
static RuntimeTypeGuard sSelectionExTypeGuard_l;
static sead::RuntimeType sSelectionExType_l;
static RuntimeTypeGuard sCurveExTypeGuard_l;
static sead::RuntimeType sCurveExType_l;
static RuntimeTypeGuard sSelectionBaseGuard_l;
static sead::RuntimeType sSelectionBaseType_l;
static RuntimeTypeGuard sSpatialBaseGuard_l;
static sead::RuntimeType sSpatialBaseType_l;
// The collection-base and common-base tables this unit stores (other units store their own copies).
static u8 sCollectionBaseTable_l;
static u8 sAglCommonBaseTable_l;

// First use of a runtime-type node: mark the guard, then point the node at its table.
template <class Guard>
static bool isType(Guard& guard, sead::RuntimeType& node, void* table, const sead::RuntimeType* type) {
    if (guard == 0) {
        guard = 1;
        node.mTable = table;
    }
    return type == &node;
}

/* 027D1E1C */
u32 agl::DirectedLightCollection::matchesType(const sead::RuntimeType* type) const {
    if (isType(sSelectionExTypeGuard_l, sSelectionExType_l, &sDirectedLightTypeTable, type)) return 1;
    if (isType(sSelectionBaseGuard_l, sSelectionBaseType_l, &sCollectionBaseTable_l, type)) return 1;
    return isType(sAglCommonBaseGuard, sAglCommonBaseType, &sAglCommonBaseTable_l, type);
}
/* 027D2BD4 */
u32 agl::SpatialLightCollection::matchesType(const sead::RuntimeType* type) const {
    if (isType(sSpatialExTypeGuard, sSpatialExType, &sSpatialLightTypeTable, type)) return 1;
    if (isType(sSpatialBaseGuard_l, sSpatialBaseType_l, &sCollectionBaseTable_l, type)) return 1;
    return isType(sAglCommonBaseGuard, sAglCommonBaseType, &sAglCommonBaseTable_l, type);
}
/* 027D3898 */
u32 agl::PairedLightCollection::matchesType(const sead::RuntimeType* type) const {
    if (isType(sCurveExTypeGuard_l, sCurveExType_l, &sPairedLightTypeTable, type)) return 1;
    if (isType(sCurveBaseGuard, sCurveBaseType, &sCollectionBaseTable_l, type)) return 1;
    return isType(sAglCommonBaseGuard, sAglCommonBaseType, &sAglCommonBaseTable_l, type);
}
/* WWHD glue
data sSpatialExTypeGuard 101FD810
data sSpatialExType 101FDFEC
data sDirectedLightTypeTable 1016A898
data sSpatialLightTypeTable 1016A8A8
data sPairedLightTypeTable 1016A8B8
data sSelectionExTypeGuard_l 101FD80C
data sSelectionExType_l 101FDFE8
data sCurveExTypeGuard_l 101FD814
data sCurveExType_l 101FDFE0
data sSelectionBaseGuard_l 101FD8D8
data sSelectionBaseType_l 101FDFF4
data sSpatialBaseGuard_l 101FD8DC
data sSpatialBaseType_l 101FDFF8
data sCollectionBaseTable_l 1016A8C8
data sAglCommonBaseTable_l 1016A848
*/
