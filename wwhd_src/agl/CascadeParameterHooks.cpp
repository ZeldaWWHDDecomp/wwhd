// Parameter data/type/size hooks identified by the tables installed by CascadeShadow::create.
#include "bindings.h"
#include "agl/CascadeShadow.h"
/* 027990C0 */
void agl::CascadeScalarPrefix::emptyHook() {}
/* 027990C4 */
u32 agl::CascadeBoolParameter::typeCode() { return 0; }
/* 027990CC */
be<u8>* agl::CascadeBoolParameter::valueAddress() { return gabi::at<be<u8>>(gabi::ea(this) + offsetof(CascadeBoolParameter, mValue)); }
/* 027990D4 */
be<u8>* agl::CascadeBoolParameter::alternateValueAddress() { return gabi::at<be<u8>>(gabi::ea(this) + offsetof(CascadeBoolParameter, mValue)); }
/* 027990DC */
u32 agl::CascadeBoolParameter::byteSize() { return 1; }
/* 027990E4 */
u32 agl::CascadeWordParameter::typeCode() { return 2; }
/* 027990EC */
be<u32>* agl::CascadeWordParameter::valueAddress() { return gabi::at<be<u32>>(gabi::ea(this) + offsetof(CascadeWordParameter, mValue)); }
/* 027990F4 */
be<u32>* agl::CascadeWordParameter::alternateValueAddress() { return gabi::at<be<u32>>(gabi::ea(this) + offsetof(CascadeWordParameter, mValue)); }
/* 027990FC */
u32 agl::CascadeWordParameter::byteSize() { return 4; }
/* 02799104 */
u32 agl::CascadeFloatParameter::typeCode() { return 1; }
/* 0279910C */
be<f32>* agl::CascadeFloatParameter::valueAddress() { return gabi::at<be<f32>>(gabi::ea(this) + offsetof(CascadeFloatParameter, mValue)); }
/* 02799114 */
be<f32>* agl::CascadeFloatParameter::alternateValueAddress() { return gabi::at<be<f32>>(gabi::ea(this) + offsetof(CascadeFloatParameter, mValue)); }
/* 0279911C */
u32 agl::CascadeFloatParameter::byteSize() { return 4; }
/* 02799124 */
u32 agl::CascadeWordView::typeCode() { return 16; }
/* 0279912C */
be<u32>* agl::CascadeWordView::valueAddress() { return gabi::at<be<u32>>(gabi::ea(this) + offsetof(CascadeWordView, mValue)); }
/* 02799134 */
be<u32>* agl::CascadeWordView::alternateValueAddress() { return gabi::at<be<u32>>(gabi::ea(this) + offsetof(CascadeWordView, mValue)); }
/* 0279913C */
u32 agl::CascadeWordView::byteSize() { return 4; }
/* 02799144 */
u32 agl::CascadeArrayParameter::typeCode() { return 14; }
/* 0279914C */
u32 agl::CascadeArrayParameter::byteSize() { return (u32)mCount << 2; }
/* 02799158 */
void* agl::CascadeArrayParameter::data() { return mData; }
/* 02799160 */
void* agl::CascadeArrayParameter::alternateData() { return mData; }
/* 02799168 */
u32 agl::parameter_tree::Value::constantOneHook() { return 1; }
/* 02799170 */
void agl::parameter_tree::Value::emptyHook() {}
/* 02799174 */
u32 agl::parameter_tree::Value::alternateConstantOneHook() { return 1; }
/* 0279917C */
void agl::parameter_tree::Value::alternateEmptyHook() {}
/* 02799180 */
u32 agl::parameter_tree::Value::matchesRecord(const gptr<MatchRecord>* record) {
    const MatchRecord* value = *record;
    u32 ownHash = mNameHash;
    return ownHash == (u32)value->mComparisonValue;
}
/* 0279919C */
u32 agl::parameter_tree::Node::constantOneHook() { return 1; }
/* 027991A4 */
void agl::parameter_tree::Node::emptyHook() {}
/* 027991A8 */
u32 agl::parameter_tree::Node::matchesRecord(const gptr<MatchRecord>* record) {
    const MatchRecord* value = *record;
    u32 ownHash = mNameHash;
    return ownHash == (u32)value->mSecondaryValue;
}
/* 027991C4 */
void agl::parameter_tree::Node::trailingEmptyHook() {}
/* 027991C8 */
void agl::parameter_tree::Node::alternateTrailingEmptyHook() {}
/* 027991CC */
void agl::parameter_tree::Node::finalEmptyHook() {}
/* WWHD glue
*/
