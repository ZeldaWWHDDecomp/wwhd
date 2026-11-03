// Per-TU parameter accessors, reconstructed from their own return instructions.
#include "agl/UtilityParameterViews.h"

/* 027C4C1C */
u32 agl::utility_parameters::ScalarParameter::valueKind() const {
    gabi::AutoFrame frame{gabi::current_function()};
    return static_cast<u32>(agl::env::ParameterValueKind::Scalar);
}

/* 027C4C24 */
be<f32>* agl::utility_parameters::ScalarParameter::value() {
    gabi::AutoFrame frame{gabi::current_function()};
    return &mValue;
}

/* 027C4C2C */
const be<f32>* agl::utility_parameters::ScalarParameter::value() const {
    gabi::AutoFrame frame{gabi::current_function()};
    return &mValue;
}

/* 027C4C34 */
u32 agl::utility_parameters::ScalarParameter::valueBytes() const {
    gabi::AutoFrame frame{gabi::current_function()};
    return 4;
}

/* 027C4C3C */
u32 agl::utility_parameters::VectorParameter::valueKind() const {
    gabi::AutoFrame frame{gabi::current_function()};
    return static_cast<u32>(agl::env::ParameterValueKind::Vector3);
}

/* 027C4C44 */
agl::env::ParameterVector3* agl::utility_parameters::VectorParameter::value() {
    gabi::AutoFrame frame{gabi::current_function()};
    return &mValue;
}

/* 027C4C4C */
const agl::env::ParameterVector3* agl::utility_parameters::VectorParameter::value() const {
    gabi::AutoFrame frame{gabi::current_function()};
    return &mValue;
}

/* 027C4C54 */
u32 agl::utility_parameters::VectorParameter::valueBytes() const {
    gabi::AutoFrame frame{gabi::current_function()};
    return 12;
}

/* 027CA9D8 */
u32 agl::utility_parameters::MatrixModeParameter::valueKind() const {
    gabi::AutoFrame frame{gabi::current_function()};
    return static_cast<u32>(agl::env::ParameterValueKind::Integer);
}

/* 027CA9E0 */
be<s32>* agl::utility_parameters::MatrixModeParameter::value() {
    gabi::AutoFrame frame{gabi::current_function()};
    return &mValue;
}

/* 027CA9E8 */
const be<s32>* agl::utility_parameters::MatrixModeParameter::value() const {
    gabi::AutoFrame frame{gabi::current_function()};
    return &mValue;
}

/* 027CA9F0 */
u32 agl::utility_parameters::MatrixModeParameter::valueBytes() const {
    gabi::AutoFrame frame{gabi::current_function()};
    return 4;
}

/* WWHD glue
*/
