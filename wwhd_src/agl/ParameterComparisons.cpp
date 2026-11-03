/* Parameter-tree object, list and owned-object hooks of the SSAO translation unit (two copies). */
#include "agl/ParameterComparisons.h"
/* 02792914 */
u32 agl::effect_parameters::ParameterObject::constantOneHook() const { return 1; }
/* 0279291C */
void agl::effect_parameters::ParameterObject::emptyHook() {}
/* 02792920 */
u32 agl::effect_parameters::ParameterObject::alternateConstantOneHook() const { return 1; }
/* 02792928 */
void agl::effect_parameters::ParameterObject::alternateEmptyHook() {}
/* 0279292C */
bool agl::effect_parameters::ParameterObject::matchesRecord(const gptr<parameter_tree::MatchRecord>* record) const {
    const parameter_tree::MatchRecord* object = *record;
    u32 expected = mNameHash;
    u32 actual = object->mComparisonValue;
    return actual == expected;
}
/* 02792948 */
u32 agl::effect_parameters::ParameterList::constantOneHook() const { return 1; }
/* 02792950 */
bool agl::effect_parameters::ParameterList::matchesRecord(const gptr<parameter_tree::MatchRecord>* record) const {
    const parameter_tree::MatchRecord* object = *record;
    u32 expected = mNameHash;
    u32 actual = object->mSecondaryValue;
    return actual == expected;
}
/* 0279296C */
void agl::effect_parameters::ParameterList::emptyHook() {}
/* 02792970 */
void agl::effect_parameters::ParameterList::trailingEmptyHook() {}
/* 02792974 */
void agl::effect_parameters::ParameterList::alternateTrailingEmptyHook() {}
// Tail call to the owning effect's (parameter root's) after-apply hook.
/* 02792978 */
void agl::effect_parameters::OwnedParameterObject::notifyOwnerApplied() {
    parameter_tree::Node* owner = mOwner;
    auto* methods = gabi::at<parameter_tree::NodeApplyDispatch>((u32)owner->mTable);
    gabi::tail_ptr<void>(methods->mAfterApply, owner);
}
/* 02795F48 */
u32 agl::ssao_parameters::ParameterObject::constantOneHook() const { return 1; }
/* 02795F50 */
void agl::ssao_parameters::ParameterObject::emptyHook() {}
/* 02795F54 */
u32 agl::ssao_parameters::ParameterObject::alternateConstantOneHook() const { return 1; }
/* 02795F5C */
void agl::ssao_parameters::ParameterObject::alternateEmptyHook() {}
/* 02795F60 */
bool agl::ssao_parameters::ParameterObject::matchesRecord(const gptr<parameter_tree::MatchRecord>* record) const {
    const parameter_tree::MatchRecord* object = *record;
    u32 expected = mNameHash;
    u32 actual = object->mComparisonValue;
    return actual == expected;
}
/* 02795F7C */
u32 agl::ssao_parameters::ParameterList::constantOneHook() const { return 1; }
/* 02795F84 */
bool agl::ssao_parameters::ParameterList::matchesRecord(const gptr<parameter_tree::MatchRecord>* record) const {
    const parameter_tree::MatchRecord* object = *record;
    u32 expected = mNameHash;
    u32 actual = object->mSecondaryValue;
    return actual == expected;
}
/* 02795FA0 */
void agl::ssao_parameters::ParameterList::emptyHook() {}
/* 02795FA4 */
void agl::ssao_parameters::ParameterList::trailingEmptyHook() {}
/* 02795FA8 */
void agl::ssao_parameters::ParameterList::alternateTrailingEmptyHook() {}
// Tail call to the owning effect's (parameter root's) after-apply hook.
/* 02796020 */
void agl::ssao_parameters::OwnedParameterObject::notifyOwnerApplied() {
    parameter_tree::Node* owner = mOwner;
    auto* methods = gabi::at<parameter_tree::NodeApplyDispatch>((u32)owner->mTable);
    gabi::tail_ptr<void>(methods->mAfterApply, owner);
}
/* WWHD glue
*/
