/** Joint hit geometry and nearest-joint selection.
 * Verified against both joint-hit specs; guest stack slots and GHS float operations are explicit. */
#include "d/d_jnt_hit.h"
using namespace gabi;
static bool isCylinder(s16 type) {
    return type == JntHitType_CYL_e || type == JntHitType_CYL2_e || type == JntHitType_CYL_THROW_e ||
           type == JntHitType_CYL_UNK_e || type == JntHitType_CYL_DELETE_e;
}
static bool isSphere(s16 type) {
    return type == JntHitType_SPH_e || type == JntHitType_SPH_THROW_e ||
           type == JntHitType_SPH_UNK_e || type == JntHitType_SPH_DELETE_e;
}
using FloatConstant = be<f32>;
static FloatConstant l_hioRadius;
static FloatConstant l_zeroFloat;
static FloatConstant l_epsilon;
static FloatConstant l_nearest;
static JntHitHD::SinCosTable l_sincosTable;
static FloatConstant l_hioRange;
static cXyz l_zeroVector;
static u8 l_hioVtable;
static JntHitHD::PiPair l_pi;
static JntHitHD::HeaderStatics l_headerStatics;

// HD: explicit nullable construction preserves GHS allocation and store order.
/* 025528D8 */
static JntHit_c* JntHit_c__ct(JntHit_c* i_this) {
    if (!i_this) i_this = static_cast<JntHit_c*>(operator_new(sizeof(JntHit_c)));
    if (i_this) {
        i_this->mpOffsets = nullptr;
        i_this->mpRadiuses = nullptr;
        i_this->mpShapeTypes = nullptr;
        i_this->field_0x1C = nullptr;
        i_this->mpSearchData = nullptr;
        i_this->mpJointIndexes = nullptr;
        i_this->mMaxNum = 0;
        i_this->mpModel = nullptr;
    }
    return i_this;
}

/* 02552930 */
u32 JntHit_c::CreateInit() {
    s32 count = mMaxNum;
    u32 posCount = 0;
    __jnt_hit_data_c* data = mpSearchData;
    for (s32 i = 0; i < count; ++i) {
        s16 type = data[i].mShapeType;
        if (isCylinder(type)) posCount += 2;
        else if (isSphere(type)) ++posCount;
    }
    mpShapeTypes = static_cast<be<s16>*>(JntHit_newArray(u32(count) * 2));
    // HD: vector array construction passes four explicit allocator arguments.
    mpOffsets = JntHit_constructVectorArray(0, posCount, sizeof(cXyz), 0);
    mpRadiuses = static_cast<be<f32>*>(JntHit_newArray(u32(s32(mMaxNum)) * 4));
    s32 jointCount = mMaxNum;
    be<s16>* joints = static_cast<be<s16>*>(JntHit_newArray(u32(jointCount) * 2));
    be<s16>* types = mpShapeTypes;
    mpJointIndexes = joints;
    if (!types) return 0;
    cXyz* offsets = mpOffsets;
    if (!offsets) return 0;
    be<f32>* radiuses = mpRadiuses;
    if (!radiuses || !joints) return 0;
    data = mpSearchData;
    for (s32 i = 0; i < s32(mMaxNum); ++i) {
        *types = s16(data->mShapeType);
        *joints = s16(data->mJointIndex);
        // HD: radius copying uses integer words, preserving signaling NaN bits.
        *reinterpret_cast<be<u32>*>(radiuses) = u32(*reinterpret_cast<be<u32>*>(&data->mRadius));
        s16 type = *types;
        if (isCylinder(type)) {
            *offsets = data->mpOffsets[0];
            offsets[1] = data->mpOffsets[1];
            ++offsets;
        } else if (isSphere(type)) {
            *offsets = data->mpOffsets[0];
        }
        ++offsets;
        ++types;
        ++joints;
        ++radiuses;
        ++data;
    }
    return 1;
}

/* 02552B60 */
static JntHit_c* JntHit_create(J3DModel* model, __jnt_hit_data_c* jntHitData, u32 count) {
    // HD: count retains its full caller register until the halfword store.
    JntHit_c* hit = static_cast<JntHit_c*>(operator_new(sizeof(JntHit_c)));
    if (!hit) return nullptr;
    hit = JntHit_c__ct(hit);
    if (!hit) return nullptr;
    hit->mpModel = model;
    hit->mMaxNum = count;
    hit->mpSearchData = jntHitData;
    return hit->CreateInit() ? hit : nullptr;
}

/* 02552BE8 */
static JntHit_HIO_c* JntHit_HIO_c__ct(JntHit_HIO_c* i_this) {
    if (!i_this) i_this = static_cast<JntHit_HIO_c*>(operator_new(sizeof(JntHit_HIO_c)));
    if (!i_this) return nullptr;
    i_this->mNo = 0;
    f32 radius = l_hioRadius;
    i_this->field_0x24 = 0;
    i_this->m0C = radius;
    i_this->m08 = 0;
    f32 range = l_hioRange;
    i_this->m06 = 0;
    i_this->__vtbl = &l_hioVtable;
    // HD: retain the interleaved vector-word loads and float stores.
    be<u32>* zero = reinterpret_cast<be<u32>*>(&l_zeroVector);
    be<u32>* vector = reinterpret_cast<be<u32>*>(&i_this->m10);
    u32 x = zero[0];
    f32 value = l_zeroFloat;
    vector[0] = x;
    vector[1] = u32(zero[1]);
    u32 z = zero[2];
    i_this->m24 = range;
    vector[2] = z;
    i_this->m20 = value;
    i_this->m1C = value;
    i_this->mNo = 0xFF;
    return i_this;
}

static void jnt_copy(cXyz* destination, const cXyz* source) {
    *destination = *source;
}

static void jnt_direction(cXyz* direction, csXyz* rotation) {
    u32 ax = u16(rotation->x) >> 3;
    u32 ay = u16(rotation->y) >> 3;
    f32 cosX = l_sincosTable.m[ax].cos;
    f32 sinY = l_sincosTable.m[ay].sin;
    f32 sinX = l_sincosTable.m[ax].sin;
    f32 cosY = l_sincosTable.m[ay].cos;
    direction->x = fmuls_ppc(cosX, sinY);
    direction->y = -sinX;
    direction->z = fmuls_ppc(cosX, cosY);
}

static void jnt_finish(cXyz* point, csXyz* rotation, cXyz* outpos, csXyz* outrot) {
    Mtx34* matrix = mDoMtx_stack_c::get();
    JntHit_inverse(matrix, matrix);
    f32 z = point->z;
    f32 x = point->x;
    f32 y = point->y;
    mDoMtx_stack_c::transM(x, y, z);
    mDoMtx_stack_c::ZXYrotM(*rotation);
    // HD: the cylinder entries copy translation as integer words; the sphere uses floats.
    be<u32>* output = reinterpret_cast<be<u32>*>(outpos);
    output[0] = u32(*reinterpret_cast<be<u32>*>(&matrix->m[0][3]));
    output[1] = u32(*reinterpret_cast<be<u32>*>(&matrix->m[1][3]));
    output[2] = u32(*reinterpret_cast<be<u32>*>(&matrix->m[2][3]));
    JntHit_MtxToRot(matrix, outrot);
}

static void jnt_matrix(J3DModel* model, s32 joint) {
    J3DMtxBuffer_hd* buffer = model->mpMtxBuffer;
    u32 matrices = buffer->mpAnmMtx;
    u16 flags = buffer->mFlags;
    buffer->mFlags = flags | 0x10;
    // HD: retain the 32-bit wrapping joint offset, including the no-hit index -1.
    Mtx34* source = gabi::at<Mtx34>(matrices + u32(joint) * sizeof(Mtx34));
    PSMTXCopy(source, mDoMtx_stack_c::get());
}

/* 02552C88 */
u32 JntHit_c::SphHitPosAngleOffset(cXyz* position, csXyz* rotation, cXyz* outPosition,
                                    csXyz* outRotation, cXyz* center, f32 radius) {
    Local<cXyz> direction, scaled, point, normalized;
    u32 ax = u16(rotation->x) >> 3;
    u32 ay = u16(rotation->y) >> 3;
    f32 cosX = l_sincosTable.m[ax].cos;
    f32 sinY = l_sincosTable.m[ay].sin;
    f32 sinX = l_sincosTable.m[ax].sin;
    f32 cosY = l_sincosTable.m[ay].cos;
    // HD: this entry uses ordinary float products, unlike the cylinder helper.
    direction->x = cosX * sinY;
    direction->y = -sinX;
    direction->z = cosX * cosY;
    f32 dot = PSVECDotProduct(position, direction.get());
    f32 magnitudeSquared = PSVECSquareMag(position);
    f32 discriminant = fmadds(dot, dot, radius * radius) - magnitudeSquared;
    if (!(discriminant >= 0)) discriminant = l_zeroFloat;
    f32 root = std_sqrtf(discriminant);
    f32 scale = -dot - root;
    cXyz_ml(direction.get(), scaled.get(), scale);
    jnt_copy(point.get(), scaled.get());
    PSVECAdd(point.get(), position, point.get());
    f32 square = PSVECSquareMag(point.get());
    f32 length = std_sqrtf(square);
    if (length > radius) {
        // HD: the normalization output has its own guest slot, even though it is not copied back.
        JntHit_normalizeZP(point.get(), normalized.get());
        PSVECScale(point.get(), point.get(), radius);
    }
    PSVECAdd(point.get(), center, point.get());
    Mtx34* matrix = mDoMtx_stack_c::get();
    JntHit_inverse(matrix, matrix);
    f32 z = point->z;
    f32 x = point->x;
    f32 y = point->y;
    mDoMtx_stack_c::transM(x, y, z);
    mDoMtx_stack_c::ZXYrotM(*rotation);
    // HD: the sphere copies float values; the cylinder helper copies integer words.
    outPosition->x = f32(matrix->m[0][3]);
    outPosition->y = f32(matrix->m[1][3]);
    outPosition->z = f32(matrix->m[2][3]);
    JntHit_MtxToRot(matrix, outRotation);
    return 1;
}

/* 02552E50 */
u32 JntHit_c::CylHitPosAngleOffset(cXyz* position, csXyz* rotation, cXyz* outPosition,
                                    csXyz* outRotation, cXyz* base, cXyz* end, f32 radius) {
    Local<cXyz> delta, axis, projection, radial, direction, projectedDirection,
                scaled, point, corrected, auxiliary, secondAuxiliary;
    cXyz_mi(end, delta.get(), base);
    JntHit_normZP(delta.get(), axis.get());
    f32 square = PSVECSquareMag(delta.get());
    f32 length = std_sqrtf(square);
    f32 along = PSVECDotProduct(position, axis.get());
    jnt_copy(radial.get(), position);
    jnt_copy(projection.get(), axis.get());
    PSVECScale(projection.get(), projection.get(), along);
    PSVECSubtract(radial.get(), projection.get(), radial.get());
    jnt_direction(direction.get(), rotation);
    f32 alignment = PSVECDotProduct(direction.get(), axis.get());
    jnt_copy(projectedDirection.get(), direction.get());
    jnt_copy(projection.get(), axis.get());
    PSVECScale(projection.get(), projection.get(), alignment);
    PSVECSubtract(projectedDirection.get(), projection.get(), projectedDirection.get());
    u32 normal = JntHit_normalizeRS(projectedDirection.get());
    f32 zero = l_zeroFloat;
    bool adjust;
    if (normal) {
        f32 dot = PSVECDotProduct(radial.get(), projectedDirection.get());
        f32 radialSquare = PSVECSquareMag(radial.get());
        f32 discriminant = fsubs_ppc(fmadds(dot, dot, fmuls_ppc(radius, radius)), radialSquare);
        if (!(discriminant >= 0)) discriminant = zero;
        f32 root = std_sqrtf(discriminant);
        cXyz_ml(direction.get(), scaled.get(), fsubs_ppc(-dot, root));
        jnt_copy(point.get(), scaled.get());
        PSVECAdd(point.get(), position, point.get());
        adjust = discriminant == zero;
    } else {
        jnt_copy(point.get(), position);
        adjust = true;
    }
    if (adjust) {
        jnt_copy(corrected.get(), point.get());
        f32 dot = PSVECDotProduct(point.get(), axis.get());
        // HD: GHS reuses delta in the normalized branch; the other branch has a separate slot.
        cXyz* temporary = normal ? delta.get() : secondAuxiliary.get();
        cXyz_ml(axis.get(), temporary, dot);
        PSVECSubtract(corrected.get(), temporary, corrected.get());
        JntHit_normalizeZP(corrected.get(), temporary);
        cXyz_ml(corrected.get(), temporary, radius);
        jnt_copy(corrected.get(), temporary);
        f32 dot2 = PSVECDotProduct(point.get(), axis.get());
        cXyz_ml(axis.get(), temporary, dot2);
        PSVECAdd(corrected.get(), temporary, corrected.get());
        jnt_copy(point.get(), corrected.get());
    }
    f32 longitudinal = PSVECDotProduct(point.get(), axis.get());
    f32 correction = zero;
    if (longitudinal < zero) correction = -longitudinal;
    else if (longitudinal > length) correction = fsubs_ppc(length, longitudinal);
    cXyz_ml(axis.get(), auxiliary.get(), correction);
    PSVECAdd(point.get(), auxiliary.get(), point.get());
    PSVECAdd(point.get(), base, point.get());
    jnt_finish(point.get(), rotation, outPosition, outRotation);
    return 1;
}

/* 02553388 */
u32 JntHit_c::Cyl2HitPosAngleOffset(cXyz* position, csXyz* rotation, cXyz* outPosition,
                                     csXyz* outRotation, cXyz* base, cXyz* end, f32 radius) {
    Local<cXyz> delta, axis, direction, scaled, sum, point, radial, projection, saved, changed;
    cXyz_mi(end, delta.get(), base);
    JntHit_normZP(delta.get(), axis.get());
    f32 square = PSVECSquareMag(delta.get());
    f32 length = std_sqrtf(square);
    f32 longitudinal = PSVECDotProduct(position, axis.get());
    jnt_direction(direction.get(), rotation);
    f32 alignment = PSVECDotProduct(direction.get(), axis.get());
    if (!(std::fabs(alignment) < f32(l_epsilon))) {
        f32 zero = l_zeroFloat;
        f32 scale = alignment > zero ? -longitudinal / alignment : fsubs_ppc(length, longitudinal) / alignment;
        jnt_copy(scaled.get(), direction.get());
        PSVECScale(scaled.get(), scaled.get(), scale);
        cXyz_pl(position, sum.get(), scaled.get());
        jnt_copy(point.get(), sum.get());
        jnt_copy(radial.get(), sum.get());
    } else {
        jnt_copy(point.get(), position);
        jnt_copy(radial.get(), position);
    }
    f32 along = PSVECDotProduct(radial.get(), axis.get());
    jnt_copy(projection.get(), axis.get());
    PSVECScale(projection.get(), projection.get(), along);
    PSVECSubtract(radial.get(), projection.get(), radial.get());
    jnt_copy(saved.get(), radial.get());
    f32 radialSquare = PSVECSquareMag(radial.get());
    f32 radialLength = std_sqrtf(radialSquare);
    if (radialLength > radius && JntHit_normalizeRS(radial.get())) {
        PSVECScale(radial.get(), radial.get(), radius);
        cXyz_mi(saved.get(), changed.get(), radial.get());
        jnt_copy(radial.get(), changed.get());
        PSVECSubtract(point.get(), radial.get(), point.get());
    }
    PSVECAdd(point.get(), base, point.get());
    jnt_finish(point.get(), rotation, outPosition, outRotation);
    return 1;
}

/* 025537B4 */
u32 JntHit_c::HitBufferUpdate(be<s32>* index, cXyz* best, u32 joint, csXyz* rotation, cXyz* point) {
    Local<cXyz> transformed, direction, difference;
    J3DModel* model = mpModel;
    jnt_matrix(model, joint);
    mDoMtx_stack_c::multVec(point, transformed.get());
    if (*index >= 0) {
        u32 ax = u16(rotation->x) >> 3;
        u32 ay = u16(rotation->y) >> 3;
        // HD: this entry loads sinX/sinY before cosX/cosY.
        f32 sinX = l_sincosTable.m[ax].sin;
        f32 sinY = l_sincosTable.m[ay].sin;
        f32 cosX = l_sincosTable.m[ax].cos;
        f32 cosY = l_sincosTable.m[ay].cos;
        direction->y = -sinX;
        direction->x = cosX * sinY;
        direction->z = cosX * cosY;
        cXyz_mi(transformed.get(), difference.get(), best);
        f32 dot = PSVECDotProduct(difference.get(), direction.get());
        f32 zero = l_zeroFloat;
        if (!(dot < zero)) return 0;
    }
    // HD: all three words are loaded before the index and vector stores.
    be<u32>* words = reinterpret_cast<be<u32>*>(transformed.get());
    u32 y = words[1];
    u32 z = words[2];
    u32 x = words[0];
    *index = joint;
    words = reinterpret_cast<be<u32>*>(best);
    words[0] = x;
    words[1] = y;
    words[2] = z;
    return 1;
}


static bool jnt_throw(s16 type) {
    return type == JntHitType_CYL_THROW_e || type == JntHitType_SPH_THROW_e;
}
static bool jnt_delete(s16 type) {
    return type == JntHitType_CYL_DELETE_e || type == JntHitType_SPH_DELETE_e;
}

/* 025538E4 */
s32 JntHit_c::searchJntHitPosAngleOffset(cXyz* pos, csXyz* rotation, cXyz* outpos, csXyz* outrot) {
    // HD: retain all ten vector slots, including the unused transformed slot, in GHS order.
    Local<cXyz> a, b, delta, axis, relative, cross, best, savedpos, transformed, temp;
    Local<csXyz> savedrot;
    Local<be<s32>> bestjoint;
    *bestjoint.get() = JntHitIdx_NONE_e;
    be<s16>* types = mpShapeTypes;
    be<s16>* joints = mpJointIndexes;
    be<f32>* radii = mpRadiuses;
    cXyz* offsets = mpOffsets;
    J3DModel* model = mpModel;
    f32 nearest = l_nearest;
    s32 chosen = JntHitIdx_NONE_e;
    s32 hit = 0;
    s32 hitpos = 0;
    s32 offsetindex = 0;
    f32 zero = l_zeroFloat;
    f32 epsilon = l_epsilon;
    for (s32 i = 0; i < s32(mMaxNum); ++i) {
        jnt_matrix(model, *joints);
        s16 shape = *types;
        bool record = false;
        if (isCylinder(shape)) {
            mDoMtx_stack_c::multVec(offsets, a.get());
            mDoMtx_stack_c::multVec(offsets + 1, b.get());
            cXyz_mi(b.get(), delta.get(), a.get());
            jnt_copy(axis.get(), delta.get());
            cXyz_mi(pos, temp.get(), a.get());
            jnt_copy(relative.get(), temp.get());
            if (!JntHit_normalizeRS(axis.get())) {
                f32 square = PSVECSquareMag(relative.get());
                f32 length = std_sqrtf(square);
                f32 radius = *radii;
                f32 distance = fsubs_ppc(length, radius);
                // HD: unordered comparisons also clamp to zero.
                if (!(distance >= 0)) distance = zero;
                if (!(distance > nearest)) {
                    chosen = *joints;
                    nearest = distance;
                    hit = i;
                    hitpos = offsetindex;
                    if (distance < radius) {
                        SphHitPosAngleOffset(relative.get(), rotation, outpos, outrot, a.get(), radius);
                        record = HitBufferUpdate(bestjoint.get(), best.get(), chosen, rotation, outpos) != 0;
                    }
                }
            } else {
                JntHit_cross(axis.get(), cross.get(), relative.get());
                f32 crossSquare = PSVECSquareMag(cross.get());
                f32 crossLength = std_sqrtf(crossSquare);
                f32 radius = *radii;
                f32 radial = fsubs_ppc(crossLength, radius);
                if (!(radial >= 0)) radial = zero;
                f32 longitudinal = PSVECDotProduct(axis.get(), relative.get());
                f32 excess = zero;
                f32 distance;
                if (longitudinal < zero) {
                    excess = -longitudinal;
                    distance = std_sqrtf(fmadds(longitudinal, longitudinal, fmuls_ppc(radial, radial)));
                } else {
                    f32 deltaSquare = PSVECSquareMag(delta.get());
                    f32 deltaLength = std_sqrtf(deltaSquare);
                    if (longitudinal > deltaLength) {
                        // HD: GHS calls both length helpers again on this branch.
                        f32 deltaSquare2 = PSVECSquareMag(delta.get());
                        f32 deltaLength2 = std_sqrtf(deltaSquare2);
                        excess = fsubs_ppc(longitudinal, deltaLength2);
                        distance = std_sqrtf(fmadds(excess, excess, fmuls_ppc(radial, radial)));
                    } else {
                        distance = std_sqrtf(fmuls_ppc(radial, radial));
                    }
                }
                if (!(distance > nearest)) {
                    chosen = *joints;
                    nearest = distance;
                    hit = i;
                    hitpos = offsetindex;
                    if (std::fabs(excess) < epsilon && std::fabs(radial) < epsilon) {
                        shape = *types;
                        if (!jnt_throw(shape)) {
                            if (shape == JntHitType_CYL_e) {
                                CylHitPosAngleOffset(relative.get(), rotation, outpos, outrot,
                                                     a.get(), b.get(), *radii);
                            } else if (shape == JntHitType_CYL2_e) {
                                Cyl2HitPosAngleOffset(relative.get(), rotation, outpos, outrot,
                                                      a.get(), b.get(), *radii);
                            }
                            if (jnt_delete(*types)) return JntHitIdx_DELETE_e;
                            record = HitBufferUpdate(bestjoint.get(), best.get(), chosen, rotation, outpos) != 0;
                        }
                    }
                }
            }
            offsets += 2;
            offsetindex += 2;
        } else if (isSphere(shape)) {
            mDoMtx_stack_c::multVec(offsets, a.get());
            cXyz_mi(pos, temp.get(), a.get());
            jnt_copy(relative.get(), temp.get());
            f32 square = PSVECSquareMag(relative.get());
            f32 length = std_sqrtf(square);
            f32 radius = *radii;
            f32 distance = fsubs_ppc(length, radius);
            if (!(distance >= 0)) distance = zero;
            if (!(distance > nearest)) {
                chosen = *joints;
                nearest = distance;
                hit = i;
                hitpos = offsetindex;
                if (std::fabs(distance) < epsilon) {
                    SphHitPosAngleOffset(relative.get(), rotation, outpos, outrot, a.get(), radius);
                    shape = *types;
                    if (jnt_delete(shape)) return JntHitIdx_DELETE_e;
                    if (!jnt_throw(shape)) {
                        record = HitBufferUpdate(bestjoint.get(), best.get(), chosen, rotation, outpos) != 0;
                    }
                }
            }
            ++offsets;
            ++offsetindex;
        }
        if (record) {
            jnt_copy(savedpos.get(), outpos);
            savedrot->x = s16(outrot->x);
            savedrot->y = s16(outrot->y);
            savedrot->z = s16(outrot->z);
        }
        ++types;
        ++joints;
        ++radii;
    }
    s32 winner = *bestjoint.get();
    if (winner >= 0) {
        jnt_copy(outpos, savedpos.get());
        outrot->x = s16(savedrot->x);
        outrot->y = s16(savedrot->y);
        outrot->z = s16(savedrot->z);
        return winner;
    }
    types = mpShapeTypes;
    radii = mpRadiuses;
    offsets = mpOffsets;
    jnt_matrix(model, chosen);
    s16 shape = types[hit];
    if (jnt_throw(shape)) return JntHitIdx_NONE_e;
    if (jnt_delete(shape)) return JntHitIdx_DELETE_e;
    cXyz* offset = offsets + u32(hitpos);
    if (shape == JntHitType_CYL_e) {
        mDoMtx_stack_c::multVec(offset, a.get());
        mDoMtx_stack_c::multVec(offset + 1, b.get());
        cXyz_mi(pos, temp.get(), a.get());
        jnt_copy(relative.get(), temp.get());
        CylHitPosAngleOffset(relative.get(), rotation, outpos, outrot, a.get(), b.get(), radii[hit]);
    } else if (shape == JntHitType_SPH_e) {
        mDoMtx_stack_c::multVec(offset, a.get());
        cXyz_mi(pos, temp.get(), a.get());
        jnt_copy(relative.get(), temp.get());
        SphHitPosAngleOffset(relative.get(), rotation, outpos, outrot, a.get(), radii[hit]);
    } else {
        mDoMtx_stack_c::multVec(offset, a.get());
        mDoMtx_stack_c::multVec(offset + 1, b.get());
        cXyz_mi(pos, temp.get(), a.get());
        jnt_copy(relative.get(), temp.get());
        Cyl2HitPosAngleOffset(relative.get(), rotation, outpos, outrot, a.get(), b.get(), radii[hit]);
    }
    return chosen;
}

// HD: compiler-generated header statics.
/* 025541F4 */
static void __sinit_d_jnt_hit_cpp() {
    l_headerStatics.mZero[2] = 0;
    l_headerStatics.mZero[0] = 0;
    l_headerStatics.mZero[3] = 0;
    l_headerStatics.mZero[1] = 0;
    __register_global_object(0x101E8C9C);
    f32 negative = l_pi.negative;
    f32 positive = l_pi.positive;
    l_headerStatics.mNegativePi = negative;
    l_headerStatics.mPositivePi = positive;
    JntHitFirstHeaderObject_ct(&l_headerStatics.mFirstObject);
    __register_global_object(0x101E8CA8);
    JntHitSecondHeaderObject_ct(&l_headerStatics.mSecondObject);
    __register_global_object(0x101E8CB4);
}

/* WWHD glue (tools/verify/autoglue.py):
data l_hioRadius 1004EF1C
data l_zeroFloat 1004EF20
data l_epsilon 1004EF28
data l_nearest 1004EF2C
data l_sincosTable 104A44F8
data l_hioRange 1004EF24
data l_zeroVector 101FFBA8
data l_hioVtable 1004EF0C
data l_pi 1004EF34
data l_headerStatics 10475A30
ext 028E8DAC void PSVECSubtract(const cXyz*, const cXyz*, cXyz*)
ext 0201B12C void JntHit_normZP(const cXyz*, cXyz*)
ext 0201B3C0 void JntHit_normalizeZP(cXyz*, cXyz*)
ext 0201B47C u32 JntHit_normalizeRS(cXyz*)
ext 0201B080 void JntHit_cross(const cXyz*, cXyz*, const cXyz*)
ext 028E91EC void JntHit_inverse(Mtx34*, Mtx34*)
ext 025F232C void JntHit_MtxToRot(Mtx34*, csXyz*)
ext 0273ADAC void* JntHit_newArray(u32)
ext 028EFFD0 cXyz* JntHit_constructVectorArray(u32, u32, u32, u32)
ext 028ED6F8 void JntHitFirstHeaderObject_ct(be<u8>*)
ext 028EAB2C void JntHitSecondHeaderObject_ct(be<u8>*)
*/
