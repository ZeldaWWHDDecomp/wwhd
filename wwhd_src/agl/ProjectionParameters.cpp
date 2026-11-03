// Projection parameter type, value-address and byte-count accessors from local WWHD instructions.
#include "agl/ProjectionParameters.h"
using namespace agl::env;

/* 0276D830 */
u32 ProjectionIntegerParameter::valueKind() const { return u32(ParameterValueKind::Integer); }
/* 0276D838 */
be<s32>* ProjectionIntegerParameter::value() { return &mValue; }
/* 0276D840 */
const be<s32>* ProjectionIntegerParameter::value() const { return &mValue; }
/* 0276D848 */
u32 ProjectionIntegerParameter::valueBytes() const { return sizeof(mValue); }

/* 0276D850 */
u32 ProjectionScalarParameter::valueKind() const { return u32(ParameterValueKind::Scalar); }
/* 0276D858 */
be<f32>* ProjectionScalarParameter::value() { return &mValue; }
/* 0276D860 */
const be<f32>* ProjectionScalarParameter::value() const { return &mValue; }
/* 0276D868 */
u32 ProjectionScalarParameter::valueBytes() const { return sizeof(mValue); }

/* 0276D870 */
u32 ProjectionVectorParameter::valueKind() const { return u32(ParameterValueKind::Vector3); }
/* 0276D878 */
ParameterVector3* ProjectionVectorParameter::value() { return &mValue; }
/* 0276D880 */
const ParameterVector3* ProjectionVectorParameter::value() const { return &mValue; }
/* 0276D888 */
u32 ProjectionVectorParameter::valueBytes() const { return sizeof(mValue); }

/* WWHD glue
*/
