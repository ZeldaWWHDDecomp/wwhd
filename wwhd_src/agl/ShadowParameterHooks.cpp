// Shadow-parameter callback entries reconstructed from the assigned WWHD instructions.
#include "bindings.h"
#include "agl/ShadowParameterHooks.h"
static u32 sShadowParameterOwnerTable;
static u32 sShadowDisposerOwnerTable;
void finalizeShadowParameterOwner(void*);
void finalizeShadowOwnerDisposer(void*, u32);

// GHS forms these subobject addresses even for a null or wrapped receiver.
/* 02797DF4 */
be<u8>* agl::shadow_parameter::BooleanParameter::valueAddress() {
    return gabi::at<be<u8>>(gabi::ea(this) + (u32)offsetof(BooleanParameter, mValue));
}
/* 02797DFC */
u32 agl::shadow_parameter::BooleanParameter::byteSize() { return 1; }
/* 02797E04 */
u32 agl::shadow_parameter::IntegerParameter::typeCode() { return 2; }
/* 02797E0C */
be<u32>* agl::shadow_parameter::IntegerParameter::valueAddress() {
    return gabi::at<be<u32>>(gabi::ea(this) + (u32)offsetof(IntegerParameter, mValue));
}
/* 02797E14 */
be<u32>* agl::shadow_parameter::IntegerParameter::alternateValueAddress() {
    return gabi::at<be<u32>>(gabi::ea(this) + (u32)offsetof(IntegerParameter, mValue));
}
/* 02797E1C */
u32 agl::shadow_parameter::IntegerParameter::byteSize() { return 4; }
/* 02797E24 */
u32 agl::shadow_parameter::FloatParameter::typeCode() { return 1; }
/* 02797E2C */
be<f32>* agl::shadow_parameter::FloatParameter::valueAddress() {
    return gabi::at<be<f32>>(gabi::ea(this) + (u32)offsetof(FloatParameter, mValue));
}
/* 02797E34 */
be<f32>* agl::shadow_parameter::FloatParameter::alternateValueAddress() {
    return gabi::at<be<f32>>(gabi::ea(this) + (u32)offsetof(FloatParameter, mValue));
}
/* 02797E3C */
u32 agl::shadow_parameter::FloatParameter::byteSize() { return 4; }
/* 02797E44 */
u32 agl::shadow_parameter::ObjectHooks_l::constantOneHook() { return 1; }
/* 02797E4C */
void agl::shadow_parameter::ObjectHooks_l::emptyHook() {}
/* 02797E50 */
u32 agl::shadow_parameter::ObjectHooks_l::alternateConstantOneHook() { return 1; }
/* 02797E58 */
void agl::shadow_parameter::ObjectHooks_l::alternateEmptyHook() {}
/* 02797E5C */
u32 agl::shadow_parameter::ObjectHooks_l::matchesRecord(const gptr<MatchRecord>* record) {
    const MatchRecord* value = *record;
    u32 ownValue = mComparisonValue;
    return ownValue == (u32)value->mComparisonValue;
}
/* 02797E78 */
void agl::shadow_parameter::OwnerHooks_l::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) == 0) return;
    mParameterTable = gabi::ea(&sShadowParameterOwnerTable);
    mOwnerTable = gabi::ea(&sShadowDisposerOwnerTable);
    finalizeShadowParameterOwner(this);
    finalizeShadowOwnerDisposer(this, 0);
    if (flags & 1) operator_delete(this);
}
// The assigned callback forwards its fourth word without observing the other arguments.
/* 02799C68 */
u32 forwardShadowCallbackWord(u32 first, u32 second, u32 third, u32 selected) { return selected; }
/* 02799C70 */
void emptyShadowCallback() {}
/* WWHD glue
 data sShadowParameterOwnerTable 10166A64
 data sShadowDisposerOwnerTable 10166A74
 ext 027A0998 void finalizeShadowParameterOwner(void*)
 ext 02752BEC void finalizeShadowOwnerDisposer(void*, u32)
*/
