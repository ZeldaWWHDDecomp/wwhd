/* The SSAO and shadow-map sources' copies of the agl parameter value accessors and text helpers (one set per
 * namespace, see agl/ScreenEffectParameters.h), and the translation helper of the screen-effect matrices. */
#include "agl/ScreenEffectParameters.h"
void* allocateEffectParameterStorage(u32);
void releaseEffectParameterStorage(void*);
/* 02792878 */
void agl::effect_parameters::Text::destroy(agl::effect_parameters::Text* text, u32 flags) {
    if (text != NULL && (flags & 1))
        gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseEffectParameterStorage), text);
}
/* 0279288C */
void agl::effect_parameters::Text::assureTerminationImpl() {}
/* 02792894 */
u32 agl::effect_parameters::BooleanParameter::getType() const { return agl::ParameterType_Bool; }
/* 0279289C */
be<u8>* agl::effect_parameters::BooleanParameter::getValue() { return &mValue; }
/* 027928A4 */
const be<u8>* agl::effect_parameters::BooleanParameter::getValue() const { return &mValue; }
/* 027928AC */
u32 agl::effect_parameters::BooleanParameter::getValueSize() const { return 1; }
/* 027928B4 */
u32 agl::effect_parameters::IntegerParameter::getType() const { return agl::ParameterType_Int; }
/* 027928BC */
be<s32>* agl::effect_parameters::IntegerParameter::getValue() { return &mValue; }
/* 027928C4 */
const be<s32>* agl::effect_parameters::IntegerParameter::getValue() const { return &mValue; }
/* 027928CC */
u32 agl::effect_parameters::IntegerParameter::getValueSize() const { return 4; }
/* 027928D4 */
u32 agl::effect_parameters::ScalarParameter::getType() const { return agl::ParameterType_F32; }
/* 027928DC */
be<f32>* agl::effect_parameters::ScalarParameter::getValue() { return &mValue; }
/* 027928E4 */
const be<f32>* agl::effect_parameters::ScalarParameter::getValue() const { return &mValue; }
/* 027928EC */
u32 agl::effect_parameters::ScalarParameter::getValueSize() const { return 4; }
/* 027928F4 */
u32 agl::effect_parameters::PairParameter::getType() const { return agl::ParameterType_Vec2; }
/* 027928FC */
be<u32>* agl::effect_parameters::PairParameter::getValue() { return mValue; }
/* 02792904 */
const be<u32>* agl::effect_parameters::PairParameter::getValue() const { return mValue; }
/* 0279290C */
u32 agl::effect_parameters::PairParameter::getValueSize() const { return 8; }
/* 02795EA0 */
void agl::ssao_parameters::Text::destroy(agl::ssao_parameters::Text* text, u32 flags) {
    if (text != NULL && (flags & 1))
        gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseEffectParameterStorage), text);
}
/* 02795EB4 */
void* agl::ssao_storage::allocateOrReuseStorage(void* storage) {
    gabi::AutoFrame frame{0x02795EB4};
    if (storage == NULL) storage = allocateEffectParameterStorage(8);
    return storage;
}
/* 02795EE0 */
void agl::ssao_parameters::Text::assureTerminationImpl() {}
/* 02795EE8 */
u32 agl::ssao_parameters::BooleanParameter::getType() const { return agl::ParameterType_Bool; }
/* 02795EF0 */
be<u8>* agl::ssao_parameters::BooleanParameter::getValue() { return &mValue; }
/* 02795EF8 */
const be<u8>* agl::ssao_parameters::BooleanParameter::getValue() const { return &mValue; }
/* 02795F00 */
u32 agl::ssao_parameters::BooleanParameter::getValueSize() const { return 1; }
/* 02795F08 */
u32 agl::ssao_parameters::IntegerParameter::getType() const { return agl::ParameterType_Int; }
/* 02795F10 */
be<s32>* agl::ssao_parameters::IntegerParameter::getValue() { return &mValue; }
/* 02795F18 */
const be<s32>* agl::ssao_parameters::IntegerParameter::getValue() const { return &mValue; }
/* 02795F20 */
u32 agl::ssao_parameters::IntegerParameter::getValueSize() const { return 4; }
/* 02795F28 */
u32 agl::ssao_parameters::ScalarParameter::getType() const { return agl::ParameterType_F32; }
/* 02795F30 */
be<f32>* agl::ssao_parameters::ScalarParameter::getValue() { return &mValue; }
/* 02795F38 */
const be<f32>* agl::ssao_parameters::ScalarParameter::getValue() const { return &mValue; }
/* 02795F40 */
u32 agl::ssao_parameters::ScalarParameter::getValueSize() const { return 4; }
/* 0279298C */
void agl::screen_effect::addTranslationToMatrix(agl::screen_effect::AffineMatrix* output,
                                               const agl::screen_effect::Vector3* translation,
                                               const agl::screen_effect::AffineMatrix* source) {
    // Preserve the row-by-row copy order when either input overlaps the output.
    for (u32 row = 0; row < 3; ++row) {
        output->mRows[row][0] = source->mRows[row][0];
        output->mRows[row][1] = source->mRows[row][1];
        output->mRows[row][2] = source->mRows[row][2];
        f32 offset = source->mRows[row][3];
        f32 component = translation->mComponents[row];
        output->mRows[row][3] = gabi::fadds_ppc(offset, component);
    }
}
/* 02797DC8 */
void agl::shadow_parameters::Text::destroy(Text* text, u32 flags) {
    if (text != NULL && (flags & 1) != 0)
        gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseEffectParameterStorage), text);
}
/* 02797DDC */
void agl::shadow_parameters::Text::assureTerminationImpl() {}
/* 02797DE4 */
u32 agl::shadow_parameters::BooleanParameter::getType() const { return agl::ParameterType_Bool; }
/* 02797DEC */
be<u8>* agl::shadow_parameters::BooleanParameter::getValue() { return &mValue; }
/* 02792890 */
void agl::effect_parameters::Parameter::emptyHook() {}
/* 02795EE4 */
void agl::ssao_parameters::Parameter::emptyHook() {}
/* 02797DE0 */
void agl::shadow_parameters::Parameter::emptyHook() {}
/* WWHD glue
ext 0273AF40 void releaseEffectParameterStorage(void*) callback
ext 0273AD10 void* allocateEffectParameterStorage(u32)
*/
