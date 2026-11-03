// This unit's copies of the parameter-tree and resource-record methods, named after the merged classes' own.
#include "agl/ShadowControlInstances.h"
#include "bindings.h"
using namespace agl::shadow_control;
static sead::DisposerVtable sParameterControlTable;
static u8 sParameterLinkTable;

/* 02791ABC */
void ControlRecord::destroy(ControlRecord* record, u32 flags) {
    gabi::AutoFrame frame{0x02791ABCu};
    if (record == nullptr) return;
    record->mVtable = &sParameterControlTable;
    record->mInterfaceMethods = &sParameterLinkTable;
    record->detach();
    static_cast<sead::RegisteredObject*>(record)->destroy(0);
    if (flags & 1) operator_delete(record);
}

/* 02791B30 */
u32 ValueHooks::constantOneHook() const { return 1; }
/* 02791B38 */
void ValueHooks::emptyHook() {}
/* 02791B3C */
u32 ValueHooks::alternateConstantOneHook() const { return 1; }
/* 02791B44 */
void ValueHooks::alternateEmptyHook() {}
/* 02791B48 */
u32 ValueHooks::matchesRecord(const gptr<MatchRecord>* record) const {
    const MatchRecord* value = *record;
    u32 ownHash = mNameHash;
    return ownHash == value->mComparisonValue;
}

/* 02791B64 */
u32 NodeHooks::constantOneHook() const { return 1; }
/* 02791B6C */
void NodeHooks::emptyHook() {}
/* 02791B70 */
u32 NodeHooks::matchesRecord(const gptr<MatchRecord>* record) const {
    const MatchRecord* value = *record;
    u32 ownHash = mNameHash;
    return ownHash == value->mSecondaryValue;
}
/* 02791B8C */
void NodeHooks::alternateEmptyHook() {}
/* 02791B90 */
void NodeHooks::trailingEmptyHook() {}
/* 02791B94 */
void NodeHooks::finalEmptyHook() {}

/* WWHD glue
data sParameterControlTable 101658EC
data sParameterLinkTable 101658FC
*/
