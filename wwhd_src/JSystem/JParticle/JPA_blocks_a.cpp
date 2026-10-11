/** Archive-backed particle key and dynamics getters, using JPA1 names and layouts. Private source. */
#include "JSystem/JParticle/JPA_blocks_a.h"


/* 028484B4 */
u8 jpa_blocks_a::JPAKeyBlockArc::getID() {
    return mpData->mID;
}
/* 028484C0 */
bool jpa_blocks_a::JPAKeyBlockArc::isLoopEnable() {
    return mpData->mbLoopEnable != 0;
}
/* 028484D4 */
u8 jpa_blocks_a::JPAKeyBlockArc::getNumber() {
    return mpData->mNumber;
}
/* 028484E0 */
const be<f32>* jpa_blocks_a::JPAKeyBlockArc::getKeyDataPtr() {
    return mpKeyData;
}

/* 0284B4D8 */
u32 jpa_blocks_a::JPADynamicsBlockArc::getDataFlag() {
    return mpData->mFlags;
}
/* 0284B4E4 */
u32 jpa_blocks_a::JPADynamicsBlockArc::getUseKeyFlag() {
    return 0;
}
/* 0284B4EC */
void jpa_blocks_a::JPADynamicsBlockArc::getEmitterScl(JPABlockVector* out) {
    const JPADynamicsBlockData* data = mpData;
    // HD: lfs/stfs copies retain signaling NaN bits and use this load/store order.
    u32 x = data->mEmitterScl.x.mBits;
    u32 z = data->mEmitterScl.z.mBits;
    u32 y = data->mEmitterScl.y.mBits;
    out->z.mBits = z;
    out->x.mBits = x;
    out->y.mBits = y;
}
/* 0284B50C */
void jpa_blocks_a::JPADynamicsBlockArc::getEmitterRot(JPABlockRotation* out) {
    const JPADynamicsBlockData* data = mpData;
    s16 z = data->mEmitterRot.z;
    s16 y = data->mEmitterRot.y;
    s16 x = data->mEmitterRot.x;
    out->z = z;
    out->x = x;
    out->y = y;
}
/* 0284B52C */
void jpa_blocks_a::JPADynamicsBlockArc::getEmitterTrs(JPABlockVector* out) {
    const JPADynamicsBlockData* data = mpData;
    u32 x = data->mEmitterTrs.x.mBits;
    u32 z = data->mEmitterTrs.z.mBits;
    u32 y = data->mEmitterTrs.y.mBits;
    out->z.mBits = z;
    out->x.mBits = x;
    out->y.mBits = y;
}
/* 0284B54C */
void jpa_blocks_a::JPADynamicsBlockArc::getEmitterDir(JPABlockVector* out) {
    const JPADynamicsBlockData* data = mpData;
    u32 x = data->mEmitterDir.x.mBits;
    u32 z = data->mEmitterDir.z.mBits;
    u32 y = data->mEmitterDir.y.mBits;
    out->z.mBits = z;
    out->x.mBits = x;
    out->y.mBits = y;
}
/* 0284B56C */
u32 jpa_blocks_a::JPADynamicsBlockArc::getVolumeType() {
    return ((u32)mpData->mFlags >> JPA_VOLUME_TYPE_SHIFT) & JPA_VOLUME_TYPE_MASK;
}
/* 0284B57C */
u32 jpa_blocks_a::JPADynamicsBlockArc::getVolumeSize() {
    return mpData->mVolumeSize;
}
/* 0284B600 */
f32 jpa_blocks_a::JPADynamicsBlockArc::getInitVelOmni() {
    return mpData->mInitVelOmni;
}
/* 0284B60C */
f32 jpa_blocks_a::JPADynamicsBlockArc::getInitVelAxis() {
    return mpData->mInitVelAxis;
}
/* 0284B618 */
f32 jpa_blocks_a::JPADynamicsBlockArc::getInitVelRndm() {
    return mpData->mInitVelRndm;
}
/* 0284B624 */
f32 jpa_blocks_a::JPADynamicsBlockArc::getInitVelDir() {
    return mpData->mInitVelDir;
}
/* 0284B630 */
f32 jpa_blocks_a::JPADynamicsBlockArc::getSpread() {
    return mpData->mSpread;
}
/* 0284B63C */
f32 jpa_blocks_a::JPADynamicsBlockArc::getInitVelRatio() {
    return mpData->mInitVelRatio;
}

/* WWHD glue: no per-TU constants or external calls. */
