// Parameter payload accessors and the local parameter resource-match hooks.
// Layouts and bodies are derived from the game's own binary; no external reference used.
#include "bindings.h"
#include "agl/ParameterValues.h"
using namespace agl::parameter_value;

/* 0277C6E4 */
static void destroyParameterArchiveHooks(ParameterArchiveHooks* object, u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (object != NULL && (flags & 1)) operator_delete(object);
}
/* 0277C6F8 */
void agl::parameter_value::ParameterArchiveHooks::onApply() {
    gabi::AutoFrame frame{gabi::current_function()};
}
/* 0277C6FC */
void agl::parameter_value::ParameterValueHooks::onApply() {
    gabi::AutoFrame frame{gabi::current_function()};
}
/* 0277C700 */
u32 agl::parameter_value::BooleanParameter::valueKind() const {
    gabi::AutoFrame frame{gabi::current_function()};
    return agl::ParameterType_Bool;
}
/* 0277C708 */
be<u8>* agl::parameter_value::BooleanParameter::value() {
    gabi::AutoFrame frame{gabi::current_function()};
    return &mValue;
}
/* 0277C710 */
const be<u8>* agl::parameter_value::BooleanParameter::value() const {
    gabi::AutoFrame frame{gabi::current_function()};
    return &mValue;
}
/* 0277C718 */
u32 agl::parameter_value::BooleanParameter::valueSize() const {
    gabi::AutoFrame frame{gabi::current_function()};
    return sizeof(u8);
}
/* 0277C720 */
u32 agl::parameter_value::IntegerParameter::valueKind() const {
    gabi::AutoFrame frame{gabi::current_function()};
    return agl::ParameterType_Int;
}
/* 0277C728 */
be<s32>* agl::parameter_value::IntegerParameter::value() {
    gabi::AutoFrame frame{gabi::current_function()};
    return &mValue;
}
/* 0277C730 */
const be<s32>* agl::parameter_value::IntegerParameter::value() const {
    gabi::AutoFrame frame{gabi::current_function()};
    return &mValue;
}
/* 0277C738 */
u32 agl::parameter_value::IntegerParameter::valueSize() const {
    gabi::AutoFrame frame{gabi::current_function()};
    return sizeof(s32);
}
/* 0277C740 */
u32 agl::parameter_value::FloatParameter::valueKind() const {
    gabi::AutoFrame frame{gabi::current_function()};
    return agl::ParameterType_F32;
}
/* 0277C748 */
be<f32>* agl::parameter_value::FloatParameter::value() {
    gabi::AutoFrame frame{gabi::current_function()};
    return &mValue;
}
/* 0277C750 */
const be<f32>* agl::parameter_value::FloatParameter::value() const {
    gabi::AutoFrame frame{gabi::current_function()};
    return &mValue;
}
/* 0277C758 */
u32 agl::parameter_value::FloatParameter::valueSize() const {
    gabi::AutoFrame frame{gabi::current_function()};
    return sizeof(f32);
}
/* 0277C760 */
u32 agl::parameter_value::ColorParameter::valueKind() const {
    gabi::AutoFrame frame{gabi::current_function()};
    return agl::ParameterType_Color;
}
/* 0277C768 */
agl::env::ParameterColor* agl::parameter_value::ColorParameter::value() {
    gabi::AutoFrame frame{gabi::current_function()};
    return &mValue;
}
/* 0277C770 */
const agl::env::ParameterColor* agl::parameter_value::ColorParameter::value() const {
    gabi::AutoFrame frame{gabi::current_function()};
    return &mValue;
}
/* 0277C778 */
u32 agl::parameter_value::ColorParameter::valueSize() const {
    gabi::AutoFrame frame{gabi::current_function()};
    return sizeof(agl::env::ParameterColor);
}
/* 0277C780 */
u32 agl::parameter_value::ParameterArchiveHooks::acceptsRead() const {
    gabi::AutoFrame frame{gabi::current_function()};
    return 1;
}
/* 0277C788 */
void agl::parameter_value::ParameterArchiveHooks::beforeRead() {
    gabi::AutoFrame frame{gabi::current_function()};
}
/* 0277C78C */
u32 agl::parameter_value::ParameterArchiveHooks::acceptsWrite() const {
    gabi::AutoFrame frame{gabi::current_function()};
    return 1;
}
/* 0277C794 */
void agl::parameter_value::ParameterArchiveHooks::beforeWrite() {
    gabi::AutoFrame frame{gabi::current_function()};
}
/* 0277C798 */
u32 agl::parameter_value::ParameterArchiveHooks::matchesIdentifier(const gptr<agl::parameter_tree::MatchRecord>* record) const {
    gabi::AutoFrame frame{gabi::current_function()};
    const agl::parameter_tree::MatchRecord* key = *record;
    u32 expected = mNameHash;
    return (u32)key->mComparisonValue == expected;
}
/* 0277C7B4 */
u32 agl::parameter_value::ParameterCategoryHooks::acceptsCategory() const {
    gabi::AutoFrame frame{gabi::current_function()};
    return 1;
}
/* 0277C7BC */
void agl::parameter_value::ParameterCategoryHooks::beforeCategory() {
    gabi::AutoFrame frame{gabi::current_function()};
}
/* 0277C7C0 */
u32 agl::parameter_value::ParameterCategoryHooks::matchesCategory(const gptr<agl::parameter_tree::MatchRecord>* record) const {
    gabi::AutoFrame frame{gabi::current_function()};
    const agl::parameter_tree::MatchRecord* key = *record;
    u32 expected = mNameHash;
    return (u32)key->mSecondaryValue == expected;
}
/* 0277C7DC */
void agl::parameter_value::ParameterCategoryHooks::afterCategory() {
    gabi::AutoFrame frame{gabi::current_function()};
}
/* 0277C7E0 */
void agl::parameter_value::ParameterCategoryHooks::beginTraversal() {
    gabi::AutoFrame frame{gabi::current_function()};
}
/* 0277C7E4 */
void agl::parameter_value::ParameterCategoryHooks::endTraversal() {
    gabi::AutoFrame frame{gabi::current_function()};
}
/* WWHD glue
*/
