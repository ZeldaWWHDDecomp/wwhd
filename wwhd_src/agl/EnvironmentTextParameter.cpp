// Text-parameter accessors used by the environment-set name and record type name.
#include "agl/EnvironmentObject.h"
using namespace agl::env;

/* 0276ED54 */
void EnvironmentTextParameter::onValueChanged() { }
/* 0276ED58 */
u32 EnvironmentTextParameter::valueKind() const { return 7; }
/* 0276ED60 */
be<u8>* EnvironmentTextParameter::value() {
    gabi::AutoFrame frame{0x0276ED60};
    mValue.assureTermination();
    return mValue.mText;
}
/* 0276EDA0 */
const be<u8>* EnvironmentTextParameter::value() const {
    gabi::AutoFrame frame{0x0276EDA0};
    const_cast<sead::FixedSafeText<32>*>(&mValue)->assureTermination();
    return mValue.mText;
}
/* 0276EDE0 */
u32 EnvironmentTextParameter::valueBytes() const { return sizeof(mValue.mStorage); }

/* WWHD glue
*/
