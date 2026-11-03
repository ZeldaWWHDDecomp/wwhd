// Out-of-line parameter methods retained in WWHD's shadow pre-pass code.
#include "agl/ShadowParameterInstances.h"
using namespace agl::shadow_parameter;

/* 02791A50 */
void ParameterBaseHooks::emptyHook() {}

/* 02791A54 */
void* ParameterBaseHooks::valueAddress() {
    return static_cast<void*>(this + 1);
}

/* 02791A5C */
u32 BooleanParameter::kind() const { return 0; }

/* 02791A64 */
void* BooleanParameter::valueAddress() { return &mValue; }

/* 02791A6C */
void* BooleanParameter::alternateValueAddress() { return &mValue; }

/* 02791A74 */
u32 BooleanParameter::valueSize() const { return sizeof(mValue); }

/* 02791A7C */
u32 IntegerParameter::kind() const { return 2; }

/* 02791A84 */
void* IntegerParameter::valueAddress() { return &mValue; }

/* 02791A8C */
void* IntegerParameter::alternateValueAddress() { return &mValue; }

/* 02791A94 */
u32 IntegerParameter::valueSize() const { return sizeof(mValue); }

/* 02791A9C */
u32 FloatParameter::kind() const { return 1; }

/* 02791AA4 */
void* FloatParameter::valueAddress() { return &mValue; }

/* 02791AAC */
void* FloatParameter::alternateValueAddress() { return &mValue; }

/* 02791AB4 */
u32 FloatParameter::valueSize() const { return sizeof(mValue); }

/* WWHD glue
*/
