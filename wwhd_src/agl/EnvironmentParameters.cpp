// Environment parameter type, value-address and byte-count accessors from local WWHD instructions.
#include "agl/EnvironmentParameters.h"
using namespace agl::env;

/* 0276F230 */
u32 EnvironmentScalarParameter::valueKind() const { return u32(ParameterValueKind::Scalar); }
/* 0276F238 */
be<f32>* EnvironmentScalarParameter::value() { return &mValue; }
/* 0276F240 */
const be<f32>* EnvironmentScalarParameter::value() const { return &mValue; }
/* 0276F248 */
u32 EnvironmentScalarParameter::valueBytes() const { return sizeof(mValue); }

/* 0276F250 */
u32 EnvironmentVectorParameter::valueKind() const { return u32(ParameterValueKind::Vector3); }
/* 0276F258 */
ParameterVector3* EnvironmentVectorParameter::value() { return &mValue; }
/* 0276F260 */
const ParameterVector3* EnvironmentVectorParameter::value() const { return &mValue; }
/* 0276F268 */
u32 EnvironmentVectorParameter::valueBytes() const { return sizeof(mValue); }

/* 0276F270 */
u32 EnvironmentColorParameter::valueKind() const { return u32(ParameterValueKind::Color); }
/* 0276F278 */
ParameterColor* EnvironmentColorParameter::value() { return &mValue; }
/* 0276F280 */
const ParameterColor* EnvironmentColorParameter::value() const { return &mValue; }
/* 0276F288 */
u32 EnvironmentColorParameter::valueBytes() const { return sizeof(mValue); }

/* WWHD glue
*/
