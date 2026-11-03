// Projection-shadow parameter updates in WWHD.
#include "agl/ProjectionShadow.h"
#include "bindings.h"
using namespace agl::projection_shadow;
static Pair2 sDefaultAnimationPair;
static u8 sProjectionControlTable;
static agl::parameter_tree::ValueVtable sProjectionParameterLinkTable;
// Preserve exact floating-point payloads through typed big-endian words.
static u32 readFloatWord(const be<f32>& value) {
    return static_cast<u32>(reinterpret_cast<const be<u32>&>(value));
}
static void writeFloatWord(be<f32>& value, u32 bits) {
    reinterpret_cast<be<u32>&>(value) = bits;
}

/* 02791E24 */
Matrix34* ProjectionShadow::rebuildBiasMatrix() {
    gabi::AutoFrame frame{0x02791E24u};
    gabi::FrameLocal<Vector3> work(8);
    gabi::FrameLocal<Vector3> translation(0x14);
    f32 bias = mRotationBias.mValue;
    f32 animation = mAnimationAngle;
    work->mX = 0.0f;
    f32 angle = gabi::fadds_ppc(bias, animation);
    work->mY = 0.0f;
    work->mZ = angle;
    Matrix34* matrix = &mBiasMatrix;
    setProjectionBiasRotation(matrix, work.get());
    f32 scaleX = mScaleBias.mValue.mX;
    f32 half = 0.5f;
    f32 inverseX = gabi::fdivs_ppc(half, scaleX);
    f32 scaleY = mScaleBias.mValue.mY;
    f32 inverseY = gabi::fdivs_ppc(-0.5f, scaleY);
    work->mX = inverseX;
    work->mZ = 1.0f;
    work->mY = inverseY;
    scaleProjectionBias(matrix, matrix, work.get());
    f32 biasX = mTranslationBias.mValue.mX;
    f32 biasY = mTranslationBias.mValue.mY;
    f32 x = gabi::fadds_ppc(biasX, half);
    f32 offsetX = mAnimationOffset.mX;
    f32 y = gabi::fadds_ppc(biasY, half);
    f32 offsetY = mAnimationOffset.mY;
    x = gabi::fadds_ppc(x, offsetX);
    f32 swingX = mSwingOffset.mX;
    y = gabi::fadds_ppc(y, offsetY);
    f32 swingY = mSwingOffset.mY;
    x = gabi::fadds_ppc(x, swingX);
    translation->mZ = 0.0f;
    y = gabi::fadds_ppc(y, swingY);
    translation->mX = x;
    translation->mY = y;
    translateProjectionBias(matrix, translation.get(), matrix);
    return matrix;
}

/* 02791F44 */
Matrix34* ProjectionShadow::resetAnimation() {
    gabi::AutoFrame frame{0x02791F44u};
    u32 x = readFloatWord(sDefaultAnimationPair.mX);
    writeFloatWord(mAnimationOffset.mX, x);
    u32 y = readFloatWord(sDefaultAnimationPair.mY);
    mAnimationAngle = 0.0f;
    writeFloatWord(mAnimationOffset.mY, y);
    x = readFloatWord(sDefaultAnimationPair.mX);
    writeFloatWord(mSwingOffset.mX, x);
    y = readFloatWord(sDefaultAnimationPair.mY);
    writeFloatWord(mSwingOffset.mY, y);
    x = readFloatWord(sDefaultAnimationPair.mX);
    writeFloatWord(mTranslationOffset.mX, x);
    y = readFloatWord(sDefaultAnimationPair.mY);
    writeFloatWord(mTranslationOffset.mY, y);
    return rebuildBiasMatrix();
}

/* 02791F88 */
Matrix34* ProjectionShadow::applySettings() {
    gabi::AutoFrame frame{0x02791F88u};
    f32 angle = mRotationBias.mValue;
    constexpr f32 kRadiansToDegrees = 57.2957763671875f;
    u8 dirty = mTexture.dirty;
    f32 angleDegrees = gabi::fmuls_ppc(angle, kRadiansToDegrees);
    f32 speed = mRotationSpeed.mValue;
    u8 repeat = mRepeat.mValue;
    f32 speedDegrees = gabi::fmuls_ppc(speed, kRadiansToDegrees);
    mRotationBiasDegrees = angleDegrees;
    u8 animated = mAnimationEnabled;
    mRotationSpeedDegrees = speedDegrees;
    u32 addressMode = repeat ? 0 : 2;
    mTexture.settings.modes[4] = addressMode;
    dirty |= 2;
    mTexture.settings.modes[3] = addressMode;
    u32 filterMode = animated ? 2 : 1;
    mTexture.dirty = dirty;
    mTexture.settings.modes[5] = addressMode;
    mTexture.settings.modes[2] = filterMode;
    mTexture.settings.modes[1] = 1;
    dirty |= 4;
    mTexture.dirty = dirty;
    mTexture.settings.modes[0] = 1;
    return resetAnimation();
}

/* 02792008 */
Matrix34* ProjectionShadow::applyFromParameterIo(void* parameterIo) {
    gabi::AutoFrame frame{0x02792008u};
    ProjectionShadow* shadow = reinterpret_cast<ProjectionShadow*>(
        static_cast<u8*>(parameterIo) - offsetof(ProjectionShadow, mRoot));
    return shadow->applySettings();
}

/* 02792010 */
void ProjectionShadow::destroy(ProjectionShadow* shadow, u32 flags) {
    gabi::AutoFrame frame{0x02792010u};
    if (shadow == nullptr) return;
    shadow->mRoot.mTree.mTable = gabi::ea(&sProjectionControlTable);
    shadow->mParameterContainer.mVtable = &sProjectionParameterLinkTable;
    agl::TextureSampler::destroy(&shadow->mTexture, 2);
    if (flags & 1) operator_delete(shadow);
}

/* WWHD glue
data sDefaultAnimationPair 104A0908
data sProjectionControlTable 10166498
data sProjectionParameterLinkTable 10166094
ext 02782C20 void setProjectionBiasRotation(agl::projection_shadow::Matrix34*,const agl::projection_shadow::Vector3*)
ext 02782D70 void scaleProjectionBias(agl::projection_shadow::Matrix34*,const agl::projection_shadow::Matrix34*,const agl::projection_shadow::Vector3*)
ext 0279298C void translateProjectionBias(agl::projection_shadow::Matrix34*,const agl::projection_shadow::Vector3*,const agl::projection_shadow::Matrix34*)
*/
