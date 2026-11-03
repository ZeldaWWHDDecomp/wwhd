/* Projection of box corners into the geometry utility's polygon builder. */
#include "agl/gx2/GeometryProjection.h"
using namespace agl::gx2util;
extern void constructGeometryArray(void*, u32, u32, u32);

extern void inverseGeometryProjection(ProjectionMatrix*, const ProjectionMatrix*);
static void buildGeometryCorners(void*, const GeometryCorners*, const GeometryByte*);
static Vector3 sDefaultGeometryExtent;
static Vector3 sDefaultPlaneNormal;
struct ProjectionValues { f32 m[16]; };
struct ProjectedCoordinates { f32 x, y, z; };
static ProjectedCoordinates projectGeometryCoordinates(const Vector3* point, const ProjectionValues& matrix) {
    f32 x = point->x.mValue;
    f32 y = point->y.mValue;
    f32 z = point->z.mValue;
    f32 weightedY = gabi::fmuls_ppc(matrix.m[13], y);
    f32 weightedXY = gabi::fmadds(matrix.m[12], x, weightedY);
    f32 weightedXYZ = gabi::fmadds(matrix.m[14], z, weightedXY);
    f32 w = gabi::fadds_ppc(weightedXYZ, matrix.m[15]);
    auto projectRow = [&](u32 row) {
        f32 scaledY = gabi::fdivs_ppc(matrix.m[row + 1], w);
        f32 termY = gabi::fmuls_ppc(scaledY, y);
        f32 scaledX = gabi::fdivs_ppc(matrix.m[row], w);
        f32 termXY = gabi::fmadds(scaledX, x, termY);
        f32 scaledZ = gabi::fdivs_ppc(matrix.m[row + 2], w);
        f32 termXYZ = gabi::fmadds(scaledZ, z, termXY);
        f32 translation = gabi::fdivs_ppc(matrix.m[row + 3], w);
        return gabi::fadds_ppc(termXYZ, translation);
    };
    f32 outX = projectRow(0);
    f32 outY = projectRow(4);
    f32 outZ = projectRow(8);
    return {outX, outY, outZ};
}
static void projectGeometryPoint(Vector3* point, const ProjectionValues& matrix) {
    auto projected = projectGeometryCoordinates(point, matrix);
    point->x.mValue = projected.x;
    point->y.mValue = projected.y;
    point->z.mValue = projected.z;
}

static void fillGeometryCorners(GeometryCorners* target, const GeometryBounds* bounds) {
    target->mPoints[0].x.mValue = bounds->mMinimum.x.mValue;
    target->mPoints[0].y.mValue = bounds->mMinimum.y.mValue;
    target->mPoints[0].z.mValue = bounds->mMinimum.z.mValue;
    target->mPoints[1].x.mValue = bounds->mMaximum.x.mValue;
    target->mPoints[1].y.mValue = bounds->mMinimum.y.mValue;
    target->mPoints[1].z.mValue = bounds->mMinimum.z.mValue;
    target->mPoints[2].x.mValue = bounds->mMaximum.x.mValue;
    target->mPoints[2].y.mValue = bounds->mMinimum.y.mValue;
    target->mPoints[2].z.mValue = bounds->mMaximum.z.mValue;
    target->mPoints[3].x.mValue = bounds->mMinimum.x.mValue;
    target->mPoints[3].y.mValue = bounds->mMinimum.y.mValue;
    target->mPoints[3].z.mValue = bounds->mMaximum.z.mValue;
    target->mPoints[4].x.mValue = bounds->mMinimum.x.mValue;
    target->mPoints[4].y.mValue = bounds->mMaximum.y.mValue;
    target->mPoints[4].z.mValue = bounds->mMinimum.z.mValue;
    target->mPoints[5].x.mValue = bounds->mMaximum.x.mValue;
    target->mPoints[5].y.mValue = bounds->mMaximum.y.mValue;
    target->mPoints[5].z.mValue = bounds->mMinimum.z.mValue;
    target->mPoints[6].x.mValue = bounds->mMaximum.x.mValue;
    target->mPoints[6].y.mValue = bounds->mMaximum.y.mValue;
    target->mPoints[6].z.mValue = bounds->mMaximum.z.mValue;
    target->mPoints[7].x.mValue = bounds->mMinimum.x.mValue;
    target->mPoints[7].y.mValue = bounds->mMaximum.y.mValue;
    target->mPoints[7].z.mValue = bounds->mMaximum.z.mValue;
}

/* 027D9C2C */
static void buildProjectedGeometryBox(void* object, const GeometryBounds* bounds, const ProjectionMatrix* matrix, const GeometryByte* color) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 value = color->mValue;
    gabi::FrameLocal<GeometryCorners> cornersStorage(0xC);
    auto& corners = *cornersStorage;
    constructGeometryArray(&corners, 8, 12, 0x027DA5E4);
    fillGeometryCorners(&corners, bounds);
    auto cached = ProjectionValues{};
    const be<f32>* elements = reinterpret_cast<const be<f32>*>(matrix);
    for (u32 index = 0; index < 16; ++index) cached.m[index] = elements[index];
    for (u32 index = 0; index < 8; ++index) projectGeometryPoint(&corners.mPoints[index], cached);
    gabi::FrameLocal<GeometryByte> storedColorStorage(0x8);
    auto& storedColor = *storedColorStorage;
    storedColor.mValue = value;
    buildGeometryCorners(object, &corners, &storedColor);
}

/* 027D9F1C */
static void buildDefaultGeometryBox(void* object, const ProjectionMatrix* projection) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<ProjectionMatrix> matrixStorage(0x3C);
    auto& matrix = *matrixStorage;
    inverseGeometryProjection(&matrix, projection);
    gabi::FrameLocal<GeometryBounds> boundsStorage(0x18);
    auto& bounds = *boundsStorage;
    bounds.mMinimum.x.mValue = -sDefaultGeometryExtent.x.mValue;
    bounds.mMinimum.y.mValue = -sDefaultGeometryExtent.y.mValue;
    bounds.mMinimum.z.mValue = -sDefaultGeometryExtent.z.mValue;
    bounds.mMaximum.x.mValue = sDefaultGeometryExtent.x.mValue;
    bounds.mMaximum.y.mValue = sDefaultGeometryExtent.y.mValue;
    bounds.mMaximum.z.mValue = sDefaultGeometryExtent.z.mValue;
    gabi::FrameLocal<Vector3> negativeExtentStorage(0x0C);
    gabi::FrameLocal<Vector3> negativeCopyStorage(0x30);
    negativeExtentStorage->x.mBits = (u32)bounds.mMinimum.x.mBits;
    negativeExtentStorage->y.mBits = (u32)bounds.mMinimum.y.mBits;
    negativeExtentStorage->z.mBits = (u32)bounds.mMinimum.z.mBits;
    negativeCopyStorage->x.mBits = (u32)negativeExtentStorage->x.mBits;
    negativeCopyStorage->y.mBits = (u32)negativeExtentStorage->y.mBits;
    negativeCopyStorage->z.mBits = (u32)negativeExtentStorage->z.mBits;
    gabi::FrameLocal<GeometryByte> colorStorage(0x9);
    auto& color = *colorStorage;
    color.mValue = 255;
    gabi::FrameLocal<GeometryByte> colorCopyStorage(0x08);
    colorCopyStorage->mValue = 255;
    buildProjectedGeometryBox(object, &bounds, &matrix, &color);
}

extern void transformGeometryPoint(const MatrixUtility*, const GeometryTriple*, GeometryTriple*);
static GeometryGroupSet* geometryActiveSet(const GeometryWorkspace* workspace) {
    u32 active = workspace->mActiveSet;
    return const_cast<GeometryGroupSet*>(workspace->mSets + (active < 2 ? active : 0));
}
static GeometryPointList* geometryPointGroup(const GeometryWorkspace* workspace, u32 index) {
    GeometryGroupSet* set = geometryActiveSet(workspace);
    auto entries = set->mEntries.get();
    return entries[index < (u32)set->mCapacity ? index : 0];
}
static GeometryTriple* geometryPoint(GeometryPointList* group, u32 index) {
    auto data = static_cast<GeometryTriple*>(group->mData.get());
    return data + (index < (u32)group->mCapacity ? index : 0);
}

/* 027D9FD4 */
static void growProjectedGeometryBounds(const GeometryWorkspace* workspace, GeometryBounds* bounds, const ProjectionMatrix* projection, const MatrixUtility* transform) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<GeometryTriple> temporaryStorage(0x8);
    auto& temporary = *temporaryStorage;
    for (s32 groupIndex = 0; groupIndex < (s32)geometryActiveSet(workspace)->mCount; ++groupIndex) {
        GeometryPointList* group = geometryPointGroup(workspace, groupIndex);
        for (s32 pointIndex = 0; pointIndex < (s32)group->mCount; ++pointIndex) {
            transformGeometryPoint(transform, geometryPoint(group, pointIndex), &temporary);
            auto cached = ProjectionValues{};
            const be<f32>* elements = reinterpret_cast<const be<f32>*>(projection);
            for (u32 index = 0; index < 16; ++index) cached.m[index] = elements[index];
            auto projected = reinterpret_cast<Vector3*>(&temporary);
            projectGeometryPoint(projected, cached);
            f32 x = projected->x.mValue;
            f32 y = projected->y.mValue;
            f32 z = projected->z.mValue;
            f32 minX = bounds->mMinimum.x.mValue;
            f32 minY = bounds->mMinimum.y.mValue;
            if (minX > x) bounds->mMinimum.x.mValue = x;
            f32 minZ = bounds->mMinimum.z.mValue;
            if (minY > y) bounds->mMinimum.y.mValue = y;
            f32 maxX = bounds->mMaximum.x.mValue;
            if (minZ > z) bounds->mMinimum.z.mValue = z;
            f32 maxY = bounds->mMaximum.y.mValue;
            if (maxX < x) bounds->mMaximum.x.mValue = x;
            f32 maxZ = bounds->mMaximum.z.mValue;
            if (maxY < y) bounds->mMaximum.y.mValue = y;
            if (maxZ < z) bounds->mMaximum.z.mValue = z;
        }
    }
}

/* 027DA23C */
static void findHighestGeometryPoint(Vector3* output, const GeometryWorkspace* workspace, const MatrixUtility* transform) {
    gabi::AutoFrame frame{gabi::current_function()};
    f32 bestX = -3.4028234663852886e38f;
    f32 bestY = -3.4028234663852886e38f;
    f32 bestZ = -3.4028234663852886e38f;
    f32 count = 0.0f;
    gabi::FrameLocal<Vector3> bestStorage(0x08);
    gabi::FrameLocal<Vector3> weightedStorage(0x38);
    gabi::FrameLocal<Vector3> averagedStorage(0x2C);
    gabi::FrameLocal<Vector3> averagedCopyStorage(0x20);
    bestStorage->x.mValue = bestX;
    bestStorage->y.mValue = bestY;
    bestStorage->z.mValue = bestZ;
    gabi::FrameLocal<GeometryTriple> temporaryStorage(0x14);
    auto& temporary = *temporaryStorage;
    for (s32 groupIndex = 0; groupIndex < (s32)geometryActiveSet(workspace)->mCount; ++groupIndex) {
        GeometryPointList* group = geometryPointGroup(workspace, groupIndex);
        for (s32 pointIndex = 0; pointIndex < (s32)group->mCount; ++pointIndex) {
            transformGeometryPoint(transform, geometryPoint(group, pointIndex), &temporary);
            f32 z = temporary.z;
            f32 tolerance = workspace->mTolerance;
            f32 difference = gabi::fsubs_ppc(bestZ, z);
            if (!(-tolerance > difference) && !(difference > tolerance)) {
                f32 weightedZ = gabi::fmuls_ppc(bestZ, count);
                f32 weightedX = gabi::fmuls_ppc(bestX, count);
                f32 weightedY = gabi::fmuls_ppc(bestY, count);
                weightedStorage->x.mValue = weightedX;
                weightedStorage->y.mValue = weightedY;
                weightedStorage->z.mValue = weightedZ;
                count = gabi::fadds_ppc(count, 1.0f);
                f32 factor = gabi::fdivs_ppc(1.0f, count);
                f32 x = gabi::fadds_ppc((f32)temporary.x, weightedX);
                f32 y = gabi::fadds_ppc((f32)temporary.y, weightedY);
                z = gabi::fadds_ppc(z, weightedZ);
                temporary.x = x;
                temporary.y = y;
                temporary.z = z;
                bestX = gabi::fmuls_ppc(x, factor);
                bestY = gabi::fmuls_ppc(y, factor);
                bestZ = gabi::fmuls_ppc(z, factor);
                averagedStorage->x.mValue = bestX;
                averagedStorage->y.mValue = bestY;
                averagedStorage->z.mValue = bestZ;
                averagedCopyStorage->x.mBits = (u32)averagedStorage->x.mBits;
                averagedCopyStorage->y.mBits = (u32)averagedStorage->y.mBits;
                averagedCopyStorage->z.mBits = (u32)averagedStorage->z.mBits;
            } else if (z > bestZ) {
                bestX = temporary.x;
                bestY = temporary.y;
                bestZ = z;
                count = 1.0f;
            }
            bestStorage->x.mValue = bestX;
            bestStorage->y.mValue = bestY;
            bestStorage->z.mValue = bestZ;
        }
    }
    output->x.mValue = bestX;
    output->y.mValue = bestY;
    output->z.mValue = bestZ;
}

/* 027D9AE8 */
static void buildTransformedGeometryBox(void* object, const GeometryBounds* bounds, const MatrixUtility* transform, const GeometryByte* color) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 value = color->mValue;
    gabi::FrameLocal<GeometryCorners> cornersStorage(0xC);
    auto& corners = *cornersStorage;
    constructGeometryArray(&corners, 8, 12, 0x027DA5E4);
    fillGeometryCorners(&corners, bounds);
    for (u32 index = 0; index < 8; ++index) {
        auto point = reinterpret_cast<GeometryTriple*>(&corners.mPoints[index]);
        transformGeometryPoint(transform, point, point);
    }
    gabi::FrameLocal<GeometryByte> storedColorStorage(0x8);
    auto& storedColor = *storedColorStorage;
    storedColor.mValue = value;
    buildGeometryCorners(object, &corners, &storedColor);
}

extern f32 normalizeGeometryVector(Vector3*);
extern void crossGeometryVectors(const Vector3*, const Vector3*, Vector3*);
static void clipGeometryByPlane(GeometryWorkspace*, const GeometryPlane*);
struct GeometryFace { be<u32> mIndex[3]; };
struct GeometryFaces { GeometryFace mFaces[6]; };
WWHD_SIZE(GeometryFace, 12);
WWHD_OFFSET(GeometryFaces, mFaces, 0);
WWHD_SIZE(GeometryFaces, 72);
static GeometryFaces sGeometryFaces;

/* 027D9764 */
static void buildGeometryCorners(void* object, const GeometryCorners* corners, const GeometryByte* color) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 mask = color->mValue;
    gabi::FrameLocal<GeometryFaces> facesStorage(0x9C);
    for (u32 face = 0; face < 6; ++face) {
        for (u32 corner = 0; corner < 3; ++corner) {
            facesStorage->mFaces[face].mIndex[corner] = (u32)sGeometryFaces.mFaces[face].mIndex[corner];
        }
    }
    gabi::FrameLocal<Vector3> centerStorage(0x84);
    auto& center = *centerStorage;
    gabi::FrameLocal<Vector3> centerSumStorage(0x78);
    centerSumStorage->x.mValue = gabi::fadds_ppc(corners->mPoints[0].x.mValue, corners->mPoints[6].x.mValue);
    centerSumStorage->y.mValue = gabi::fadds_ppc(corners->mPoints[0].y.mValue, corners->mPoints[6].y.mValue);
    centerSumStorage->z.mValue = gabi::fadds_ppc(corners->mPoints[0].z.mValue, corners->mPoints[6].z.mValue);
    gabi::FrameLocal<Vector3> centerSumCopyStorage(0x90);
    centerSumCopyStorage->x.mBits = (u32)centerSumStorage->x.mBits;
    centerSumCopyStorage->y.mBits = (u32)centerSumStorage->y.mBits;
    centerSumCopyStorage->z.mBits = (u32)centerSumStorage->z.mBits;
    center.x.mValue = gabi::fmuls_ppc(centerSumStorage->x.mValue, 0.5f);
    center.y.mValue = gabi::fmuls_ppc(centerSumStorage->y.mValue, 0.5f);
    center.z.mValue = gabi::fmuls_ppc(centerSumStorage->z.mValue, 0.5f);
    gabi::FrameLocal<Vector3> centerCopyStorage(0x54);
    centerCopyStorage->x.mBits = (u32)center.x.mBits;
    centerCopyStorage->y.mBits = (u32)center.y.mBits;
    centerCopyStorage->z.mBits = (u32)center.z.mBits;
    gabi::FrameLocal<Vector3> edgeDifferenceStorage(0x30);
    gabi::FrameLocal<Vector3> middleDifferenceStorage(0x3C);
    gabi::FrameLocal<Vector3> negativeNormalStorage(0x24);
    gabi::FrameLocal<Vector3> negativeCopyStorage(0x48);
    gabi::FrameLocal<GeometryPlane> planeStorage(0x8);
    auto& plane = *planeStorage;
    plane.mOffset = 0.0f;
    plane.mNormal.x.mBits = (u32)sDefaultPlaneNormal.x.mBits;
    plane.mNormal.y.mBits = (u32)sDefaultPlaneNormal.y.mBits;
    plane.mNormal.z.mBits = (u32)sDefaultPlaneNormal.z.mBits;
    gabi::FrameLocal<Vector3> edgeAStorage(0x60);
    auto& edgeA = *edgeAStorage;
    gabi::FrameLocal<Vector3> edgeBStorage(0x6C);
    auto& edgeB = *edgeBStorage;
    gabi::FrameLocal<Vector3> normalStorage(0x18);
    auto& normal = *normalStorage;
    for (u32 face = 0; face < 6; ++face) {
        if (!(mask & (1u << face))) continue;
        const GeometryFace* indices = &facesStorage->mFaces[face];
        u32 lastIndex = indices->mIndex[2];
        u32 firstIndex = indices->mIndex[0];
        const Vector3* first = &corners->mPoints[firstIndex];
        const Vector3* last = &corners->mPoints[lastIndex];
        u32 middleIndex = indices->mIndex[1];
        edgeA.x.mValue = gabi::fsubs_ppc(last->x.mValue, first->x.mValue);
        edgeA.y.mValue = gabi::fsubs_ppc(last->y.mValue, first->y.mValue);
        edgeA.z.mValue = gabi::fsubs_ppc(last->z.mValue, first->z.mValue);
        edgeDifferenceStorage->x.mBits = (u32)edgeA.x.mBits;
        edgeDifferenceStorage->y.mBits = (u32)edgeA.y.mBits;
        edgeDifferenceStorage->z.mBits = (u32)edgeA.z.mBits;
        normalizeGeometryVector(&edgeA);
        const Vector3* middle = &corners->mPoints[middleIndex];
        edgeB.x.mValue = gabi::fsubs_ppc(middle->x.mValue, first->x.mValue);
        edgeB.y.mValue = gabi::fsubs_ppc(middle->y.mValue, first->y.mValue);
        edgeB.z.mValue = gabi::fsubs_ppc(middle->z.mValue, first->z.mValue);
        middleDifferenceStorage->x.mBits = (u32)edgeB.x.mBits;
        middleDifferenceStorage->y.mBits = (u32)edgeB.y.mBits;
        middleDifferenceStorage->z.mBits = (u32)edgeB.z.mBits;
        normalizeGeometryVector(&edgeB);
        crossGeometryVectors(&edgeA, &edgeB, &normal);
        f32 magnitude = normalizeGeometryVector(&normal);
        f32 x = normal.x.mValue;
        f32 y = normal.y.mValue;
        f32 z = normal.z.mValue;
        f32 square = gabi::fmadds(z, z, gabi::fmadds(x, x, gabi::fmuls_ppc(y, y)));
        f32 error = gabi::fsubs_ppc(square, 1.0f);
        bool unit = !(-1.1920928955078125e-6f > error) && !(error > 1.1920928955078125e-6f);
        if (!(magnitude == magnitude) || !(magnitude > 0.0f) || !unit) continue;
        f32 xy = gabi::fadds_ppc(gabi::fmuls_ppc(x, first->x.mValue), gabi::fmuls_ppc(y, first->y.mValue));
        f32 offset = gabi::fmadds(z, first->z.mValue, xy);
        f32 centerXY = gabi::fadds_ppc(gabi::fmuls_ppc(x, center.x.mValue), gabi::fmuls_ppc(y, center.y.mValue));
        f32 distance = gabi::fsubs_ppc(gabi::fmadds(z, center.z.mValue, centerXY), offset);
        if (distance > 0.0f) {
            negativeNormalStorage->x.mBits = (u32)normal.x.mBits ^ 0x80000000;
            negativeNormalStorage->y.mBits = (u32)normal.y.mBits ^ 0x80000000;
            negativeNormalStorage->z.mBits = (u32)normal.z.mBits ^ 0x80000000;
            negativeCopyStorage->x.mBits = (u32)negativeNormalStorage->x.mBits;
            negativeCopyStorage->y.mBits = (u32)negativeNormalStorage->y.mBits;
            negativeCopyStorage->z.mBits = (u32)negativeNormalStorage->z.mBits;
            plane.mNormal.x.mValue = -((f32)normal.x.mValue);
            plane.mNormal.y.mValue = -((f32)normal.y.mValue);
            plane.mNormal.z.mValue = -((f32)normal.z.mValue);
            plane.mOffset = -offset;
        } else {
            plane.mNormal.x.mValue = x;
            plane.mNormal.y.mValue = y;
            plane.mNormal.z.mValue = z;
            plane.mOffset = offset;
        }
        clipGeometryByPlane(static_cast<GeometryWorkspace*>(object), &plane);
    }
}

extern GeometryPointList* takeGeometrySlot(void*);
static void clipGeometryPolygon(GeometryWorkspace*, GeometryPointList*, GeometryPointList*, GeometryPointList*, const GeometryPlane*);

/* 027D95C4 */
static void clipGeometryByPlane(GeometryWorkspace* workspace, const GeometryPlane* plane) {
    gabi::AutoFrame frame{gabi::current_function()};
    auto loose = reinterpret_cast<GeometryPointGroups*>(workspace);
    loose->mCount = 0;
    for (s32 index = 0; index < (s32)geometryActiveSet(workspace)->mCount; ++index) {
        u32 next = ((u32)workspace->mActiveSet + 1) & 1;
        GeometryPointList* polygon = takeGeometrySlot(&workspace->mSets[next]);
        GeometryPointList* cuts = takeGeometrySlot(workspace);
        if (polygon == NULL || cuts == NULL) break;
        GeometryPointList* source = geometryPointGroup(workspace, index);
        clipGeometryPolygon(workspace, polygon, cuts, source, plane);
        if ((s32)polygon->mCount == 0) {
            next = ((u32)workspace->mActiveSet + 1) & 1;
            workspace->mSets[next].mCount = (s32)workspace->mSets[next].mCount - 1;
            loose->mCount = (s32)loose->mCount - 1;
        }
    }
    u32 next = ((u32)workspace->mActiveSet + 1) & 1;
    u32 joined = loose->joinPairs(&workspace->mSets[next]);
    next = ((u32)workspace->mActiveSet + 1) & 1;
    if (joined != 0) {
        workspace->mActiveSet = next;
        next = (next + 1) & 1;
    }
    workspace->mSets[next].mCount = 0;
}

struct GeometrySegment { GeometryTriple mFirst, mSecond; };
struct GeometryRatio { be<f32> mValue; };
extern u32 intersectGeometryPlane(GeometrySegment*, const GeometryPlane*, GeometryRatio*);
static f32 geometryPlaneDistance(const GeometryPlane* plane, const GeometryTriple* point) {
    f32 xy = gabi::fadds_ppc(gabi::fmuls_ppc(plane->mNormal.x.mValue, point->x), gabi::fmuls_ppc(plane->mNormal.y.mValue, point->y));
    return gabi::fsubs_ppc(gabi::fmadds(plane->mNormal.z.mValue, point->z, xy), plane->mOffset);
}
static void copyGeometryTriple(GeometryTriple* target, const GeometryTriple* source) {
    auto targetWords = reinterpret_cast<be<u32>*>(target);
    const auto sourceWords = reinterpret_cast<const be<u32>*>(source);
    targetWords[0] = sourceWords[0];
    targetWords[1] = sourceWords[1];
    targetWords[2] = sourceWords[2];
}
static void appendClippedPoint(GeometryPointList* list, const GeometryTriple* source) {
    u32 count = list->mCount;
    u32 capacity = list->mCapacity;
    auto data = static_cast<GeometryTriple*>(list->mData.get());
    list->mCount = count + 1;
    GeometryTriple* target = data + (count < capacity ? count : 0);
    target->x = source->x;
    target->y = source->y;
    target->z = source->z;
}
static void interpolateGeometryPoint(GeometryTriple* output, GeometryTriple* difference,
                                     GeometryTriple* differenceCopy, GeometryTriple* scaled,
                                     GeometryTriple* scaledCopy, GeometryTriple* outputCopy,
                                     const GeometryTriple* first, const GeometryTriple* second, f32 ratio) {
    difference->x = gabi::fsubs_ppc(second->x, first->x);
    difference->y = gabi::fsubs_ppc(second->y, first->y);
    difference->z = gabi::fsubs_ppc(second->z, first->z);
    copyGeometryTriple(differenceCopy, difference);
    scaled->x = gabi::fmuls_ppc(differenceCopy->x, ratio);
    scaled->y = gabi::fmuls_ppc(differenceCopy->y, ratio);
    scaled->z = gabi::fmuls_ppc(differenceCopy->z, ratio);
    copyGeometryTriple(scaledCopy, scaled);
    output->x = gabi::fadds_ppc(first->x, scaledCopy->x);
    output->y = gabi::fadds_ppc(first->y, scaledCopy->y);
    output->z = gabi::fadds_ppc(first->z, scaledCopy->z);
    copyGeometryTriple(outputCopy, output);
}

/* 027D8CC4 */
static void clipGeometryPolygon(GeometryWorkspace* workspace, GeometryPointList* output, GeometryPointList* cuts, GeometryPointList* source, const GeometryPlane* plane) {
    gabi::AutoFrame frame{gabi::current_function()};
    auto data = static_cast<GeometryTriple*>(source->mData.get());
    f32 distance = geometryPlaneDistance(plane, data);
    s32 count = source->mCount;
    gabi::FrameLocal<GeometrySegment> enteringStorage(0xA0);
    auto& entering = *enteringStorage;
    gabi::FrameLocal<GeometrySegment> leavingStorage(0xB8);
    auto& leaving = *leavingStorage;
    gabi::FrameLocal<GeometryRatio> enterRatioStorage(0x20);
    auto& enterRatio = *enterRatioStorage;
    gabi::FrameLocal<GeometryRatio> leaveRatioStorage(0x24);
    auto& leaveRatio = *leaveRatioStorage;
    gabi::FrameLocal<GeometryTriple> enterDifferenceStorage(0x28);
    gabi::FrameLocal<GeometryTriple> enterDifferenceCopyStorage(0x7C);
    gabi::FrameLocal<GeometryTriple> enterScaledStorage(0x34);
    gabi::FrameLocal<GeometryTriple> enterScaledCopyStorage(0x70);
    gabi::FrameLocal<GeometryTriple> enterOutputCopyStorage(0x08);
    gabi::FrameLocal<GeometryTriple> leaveDifferenceStorage(0x4C);
    gabi::FrameLocal<GeometryTriple> leaveDifferenceCopyStorage(0x94);
    gabi::FrameLocal<GeometryTriple> leaveScaledStorage(0x58);
    gabi::FrameLocal<GeometryTriple> leaveScaledCopyStorage(0x88);
    gabi::FrameLocal<GeometryTriple> leaveOutputCopyStorage(0x14);
    gabi::FrameLocal<GeometryTriple> enterPointStorage(0x40);
    auto& enterPoint = *enterPointStorage;
    gabi::FrameLocal<GeometryTriple> leavePointStorage(0x64);
    auto& leavePoint = *leavePointStorage;
    for (s32 index = 0; index < count; ++index) {
        s32 nextIndex = (index + 1) % count;
        u32 capacity = source->mCapacity;
        const GeometryTriple* first = data + ((u32)index < capacity ? index : 0);
        const GeometryTriple* next = data + ((u32)nextIndex < capacity ? nextIndex : 0);
        bool outside = distance > 0.0f;
        distance = geometryPlaneDistance(plane, next);
        bool nextOutside = distance > 0.0f;
        if (!outside && !nextOutside) appendClippedPoint(output, next);
        else if (outside && !nextOutside) {
            copyGeometryTriple(&entering.mFirst, first);
            copyGeometryTriple(&entering.mSecond, next);
            if (intersectGeometryPlane(&entering, plane, &enterRatio) == 1) {
                interpolateGeometryPoint(&enterPoint, enterDifferenceStorage, enterDifferenceCopyStorage,
                                         enterScaledStorage, enterScaledCopyStorage, enterOutputCopyStorage,
                                         first, next, enterRatio.mValue);
                appendClippedPoint(output, &enterPoint);
                appendClippedPoint(cuts, &enterPoint);
            }
            appendClippedPoint(output, next);
        } else if (!outside && nextOutside) {
            copyGeometryTriple(&leaving.mFirst, first);
            copyGeometryTriple(&leaving.mSecond, next);
            if (intersectGeometryPlane(&leaving, plane, &leaveRatio) == 1) {
                interpolateGeometryPoint(&leavePoint, leaveDifferenceStorage, leaveDifferenceCopyStorage,
                                         leaveScaledStorage, leaveScaledCopyStorage, leaveOutputCopyStorage,
                                         first, next, leaveRatio.mValue);
                appendClippedPoint(output, &leavePoint);
                appendClippedPoint(cuts, &leavePoint);
            }
        }
        count = source->mCount;
        data = static_cast<GeometryTriple*>(source->mData.get());
    }
}

extern u32 inverseGeometryTransform(const MatrixUtility*, MatrixUtility*);
extern void resetGeometryWorkspace(GeometryWorkspace*);

/* 027D85E8 */
static void initializeGeometryFrustum(GeometryWorkspace* workspace, const MatrixUtility* view, const ProjectionMatrix* projection, f32 nearDepth, f32 farDepth) {
    gabi::AutoFrame frame{gabi::current_function()};
    f32 a = projection->mRows[2].w.mValue;
    f32 b = projection->mRows[2].z.mValue;
    f32 c = projection->mRows[3].z.mValue;
    f32 d = projection->mRows[3].w.mValue;
    f32 nearZ = gabi::fdivs_ppc(gabi::fnmsubs(b, nearDepth, a), gabi::fnmsubs(c, nearDepth, d));
    f32 farZ = gabi::fdivs_ppc(gabi::fnmsubs(b, farDepth, a), gabi::fnmsubs(c, farDepth, d));
    gabi::FrameLocal<GeometryCorners> cornersStorage(0x60);
    auto& corners = *cornersStorage;
    for (u32 index = 0; index < 8; ++index) {
        corners.mPoints[index].x.mValue = (index == 1 || index == 2 || index == 5 || index == 6) ? 1.0f : -1.0f;
        corners.mPoints[index].y.mValue = (index == 2 || index == 3 || index == 6 || index == 7) ? 1.0f : -1.0f;
        corners.mPoints[index].z.mValue = index < 4 ? nearZ : farZ;
    }
    gabi::FrameLocal<ProjectionMatrix> inverseProjectionStorage(0x20);
    auto& inverseProjection = *inverseProjectionStorage;
    inverseGeometryProjection(&inverseProjection, projection);
    gabi::FrameLocal<MatrixUtility> inverseViewStorage(0xC0);
    auto& inverseView = *inverseViewStorage;
    inverseGeometryTransform(view, &inverseView);
    for (u32 index = 0; index < 4; ++index) {
        auto cached = ProjectionValues{};
        const be<f32>* elements = reinterpret_cast<const be<f32>*>(&inverseProjection);
        for (u32 word = 0; word < 16; ++word) cached.m[word] = elements[word];
        auto nearPoint = projectGeometryCoordinates(&corners.mPoints[index], cached);
        workspace->mCorners.mPoints[index].x.mValue = nearPoint.x;
        workspace->mCorners.mPoints[index].y.mValue = nearPoint.y;
        workspace->mCorners.mPoints[index].z.mValue = nearPoint.z;
        for (u32 word = 0; word < 16; ++word) cached.m[word] = elements[word];
        auto farPoint = projectGeometryCoordinates(&corners.mPoints[index + 4], cached);
        workspace->mCorners.mPoints[index + 4].x.mValue = farPoint.x;
        workspace->mCorners.mPoints[index + 4].y.mValue = farPoint.y;
        workspace->mCorners.mPoints[index + 4].z.mValue = farPoint.z;
    }
    gabi::FrameLocal<Vector3> differenceStorage(0x08);
    auto& difference = *differenceStorage;
    gabi::FrameLocal<Vector3> distanceCopyStorage(0x14);
    auto& distanceCopy = *distanceCopyStorage;
    f32 maximum = 0.0f;
    workspace->mDiameter = maximum;
    for (u32 first = 0; first < 8; ++first) {
        for (u32 second = first + 1; second < 8; ++second) {
            const Vector3* aPoint = &workspace->mCorners.mPoints[first];
            const Vector3* bPoint = &workspace->mCorners.mPoints[second];
            f32 x = gabi::fsubs_ppc(aPoint->x.mValue, bPoint->x.mValue);
            f32 y = gabi::fsubs_ppc(aPoint->y.mValue, bPoint->y.mValue);
            difference.x.mValue = x;
            difference.y.mValue = y;
            f32 z = gabi::fsubs_ppc(aPoint->z.mValue, bPoint->z.mValue);
            distanceCopy.y.mBits = (u32)difference.y.mBits;
            distanceCopy.x.mBits = (u32)difference.x.mBits;
            difference.z.mValue = z;
            distanceCopy.z.mBits = (u32)difference.z.mBits;
            f32 square = gabi::fmadds(z, z, gabi::fmadds(x, x, gabi::fmuls_ppc(y, y)));
            if (maximum < square) { maximum = square; workspace->mDiameter = maximum; }
        }
    }
    if (0.0f < maximum) {
        f64 estimate = gabi::frsqrte(maximum);
        f32 square = (f32)gabi::fmul_ppc(estimate, round25(estimate));
        f32 half = (f32)gabi::fmul_ppc(estimate, 0.5);
        f32 correction = gabi::fnmsubs(square, maximum, 3.0f);
        f32 refined = gabi::fmuls_ppc(correction, half);
        workspace->mDiameter = gabi::fmuls_ppc(refined, maximum);
    }
    for (u32 index = 0; index < 4; ++index) {
        auto first = reinterpret_cast<GeometryTriple*>(&workspace->mCorners.mPoints[index]);
        auto second = reinterpret_cast<GeometryTriple*>(&workspace->mCorners.mPoints[index + 4]);
        transformGeometryPoint(&inverseView, first, first);
        transformGeometryPoint(&inverseView, second, second);
    }
    resetGeometryWorkspace(workspace);
}

/* WWHD glue
 ext 028E91EC u32 inverseGeometryTransform(const agl::gx2util::MatrixUtility*, agl::gx2util::MatrixUtility*)
 ext 027D7F44 void resetGeometryWorkspace(agl::gx2util::GeometryWorkspace*)
 ext 027E9D28 u32 intersectGeometryPlane(GeometrySegment*, const agl::gx2util::GeometryPlane*, GeometryRatio*)
 ext 027D7EA8 agl::gx2util::GeometryPointList* takeGeometrySlot(void*)
 data sGeometryFaces 1016C398
 ext 025155D8 f32 normalizeGeometryVector(agl::gx2util::Vector3*)
 ext 028E8D4C void crossGeometryVectors(const agl::gx2util::Vector3*, const agl::gx2util::Vector3*, agl::gx2util::Vector3*)
 ext 028E8F64 void transformGeometryPoint(const agl::gx2util::MatrixUtility*, const agl::gx2util::GeometryTriple*, agl::gx2util::GeometryTriple*)
 data sDefaultGeometryExtent 104A0958
 data sDefaultPlaneNormal 104A0934
 ext 028EFFD0 void constructGeometryArray(void*, u32, u32, u32)
 ext 0274EDD4 void inverseGeometryProjection(agl::gx2util::ProjectionMatrix*, const agl::gx2util::ProjectionMatrix*)
*/
