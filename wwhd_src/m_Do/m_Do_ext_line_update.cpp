/** m_Do_ext_line_update.cpp — line material vertex updates.
 * Based on tww m_Do/m_Do_ext.cpp.
 */
#include "m_Do/m_Do_ext_line_update.h"
using namespace line_layout;
static f32 lineZero;
static f32 lineHalf;

static void copyPosition(cXyz* destination, const cXyz* source) {
    // HD: floating loads/stores here; vector work copies below are integer bit copies.
    destination->x = source->x;
    destination->y = source->y;
    destination->z = source->z;
}
static void setAttributes(VertexTable* table, u32 index, const cXyz* tangent,
                          f32 x, f32 y, f32 z, f32 zero, bool reverse) {
    Vertex* vertex = table->getVertex(index);
    copyPosition(&vertex->mTangent, tangent);
    vertex = table->getVertex(index);
    if (reverse) {
        vertex->mNormal.y = y;
        vertex->mNormal.z = z;
        vertex->mNormalW = zero;
        vertex->mNormal.x = x;
    } else {
        vertex->mNormal.x = x;
        vertex->mNormal.y = y;
        vertex->mNormal.z = z;
        vertex->mNormalW = zero;
    }
}

/* 025EA548 */
void line_layout::mDoExt_3DlineMat0_c::update(u32 count, const be<u32>* color, u32 taper, dKy_tevstr_c* lighting, f32 width) {
    mColor = *color;
    u32 capacity = mMaxSegments;
    mpTevStr = lighting;
    mNumSegments = count;
    if (count > capacity)
        mNumSegments = capacity;
    CameraView* camera = lineGetView();
    mDoExt_3Dline_c* line = mpLines;
    f32 zero = lineZero;
    f32 step = taper ? width / static_cast<f32>(taper) : zero;
    LineWork work; // HD: keep the original shared stack slots, including carried normal values.
    cXyz* tangent = &work.tangent;
    cXyz* cross = &work.cross;
    cXyz* normal = &work.normal;
    cXyz* temp = &work.temp;
    cXyz* eye = &work.eye;
    cXyz* out = &work.out;
    cXyz* norm = &work.norm;
    cXyz* partial = &work.partial;
    cXyz* averagePlus = &work.averagePlus;
    cXyz* averageMinus = &work.averageMinus;
    cXyz* scaled = &work.scaled;
    cXyz* norm2 = &work.norm2;
    for (u32 i = 0; i < mNumLines; ++i, ++line) {
        f32 half = lineHalf;
        f32 currentWidth = width;
        cXyz* point = line->mpSegments;

        *temp = point[1] - point[0];
        tangent->copy(*temp);
        cross->copy(*temp);
        *temp = point[0] - camera->mEye;
        eye->copy(*temp);
        *temp = cross->outprod(*eye);
        cross->copy(*temp);
        PSVECSquareMag(cross);
        f32 magnitude = lineSquareRoot();
        *temp = tangent->outprod(*cross);
        tangent->copy(*temp);
        f32 nx, ny, nz;
        if (lineNonzero(magnitude)) {
            lineNormalize(tangent, norm);
            tangent->copy(*norm);
            lineNormalize(cross, norm);
            nz = norm->z;
            normal->z = nz;
            nx = norm->x;
            normal->x = nx;
            ny = norm->y;
            normal->y = ny;
            PSVECScale(cross, cross, currentWidth / magnitude);
        } else {
            nz = normal->z;
            nx = normal->x;
            ny = normal->y;
        }
        MeshBuffer* root = line->mpMeshBuffer;
        VertexTable* table = root->getVertices();
        Vertex* destination = table->mpVertices;
        *out = point[0] + *cross;
        copyPosition(&destination->mPosition, out);
        setAttributes(table, 0, tangent, nx, ny, nz, zero, true);
        destination = table->getVertex(1);
        *out = point[0] - *cross;
        copyPosition(&destination->mPosition, out);
        setAttributes(table, 1, tangent, nx, ny, nz, zero, false);
        ++point;

        u32 index = 2;
        *temp = point[0] + *cross;
        averagePlus->copy(*temp);
        *temp = point[0] - *cross;
        averageMinus->copy(*temp);
        s32 remaining = static_cast<s32>(mNumSegments) - 2;
        for (s32 n = remaining; n > 0; --n) {
            if (n < static_cast<s32>(taper))
                currentWidth = gabi::fsubs_ppc(currentWidth, step);
            *partial = point[1] - point[0];
            cross->copy(*partial);
            tangent->copy(*partial);
            *partial = point[0] - camera->mEye;
            eye->copy(*partial);
            *partial = cross->outprod(*eye);
            cross->copy(*partial);
            PSVECSquareMag(cross);
            magnitude = lineSquareRoot();
            *partial = tangent->outprod(*cross);
            tangent->copy(*partial);
            if (lineNonzero(magnitude)) {
                lineNormalize(tangent, norm2);
                tangent->copy(*norm2);
                lineNormalize(cross, norm2);
                nx = norm2->x;
                ny = norm2->y;
                normal->x = nx;
                nz = norm2->z;
                normal->y = ny;
                magnitude = currentWidth / magnitude;
                normal->z = nz;
            }
            PSVECScale(cross, cross, magnitude);
            *partial = point[0] + *cross;
            lineAddVectors(averagePlus, partial, averagePlus);
            *partial = point[0] - *cross;
            lineAddVectors(averageMinus, partial, averageMinus);
            destination = table->getVertex(index);
            lineScale(averagePlus, scaled, half);
            copyPosition(&destination->mPosition, scaled);
            setAttributes(table, index, tangent, nx, ny, nz, zero, true);
            ++index;
            destination = table->getVertex(index);
            lineScale(averageMinus, scaled, half);
            copyPosition(&destination->mPosition, scaled);
            setAttributes(table, index, tangent, nx, ny, nz, zero, false);
            ++point;

            ++index;
            *partial = point[0] + *cross;
            averagePlus->copy(*partial);
            *partial = point[0] - *cross;
            averageMinus->copy(*partial);
        }
        cXyz* last = taper ? point : averagePlus;
        destination = table->getVertex(index);
        copyPosition(&destination->mPosition, last);
        setAttributes(table, index, tangent, nx, ny, nz, zero, false);
        ++index;
        destination = table->getVertex(index);
        copyPosition(&destination->mPosition, taper ? point : averageMinus);
        setAttributes(table, index, tangent, nx, ny, nz, zero, true);
    }
    // HD: finalization changes each mesh's selected buffer; do it after all geometry.
    for (u32 i = 0; i < mNumLines; ++i)
        lineFinalize(mpLines[i].mpMeshBuffer, 0, 1);
    lineMaterialSetup(this, mpTevStr, &mColor);
}

/* 025EAF58 */
void line_layout::mDoExt_3DlineMat0_c::updateWidths(u32 count, const be<u32>* color, dKy_tevstr_c* lighting) {
    mColor = *color;
    u32 capacity = mMaxSegments;
    mpTevStr = lighting;
    mNumSegments = count;
    if (count > capacity)
        mNumSegments = capacity;
    CameraView* camera = lineGetView();
    mDoExt_3Dline_c* line = mpLines;
    f32 zero = lineZero;

    WidthWork work; // HD: keep the original shared stack slots, including carried normal values.
    cXyz* tangent = &work.tangent;
    cXyz* cross = &work.cross;
    cXyz* normal = &work.normal;
    cXyz* temp = &work.temp;
    cXyz* eye = &work.eye;
    cXyz* out = &work.out;
    cXyz* norm = &work.norm;
    cXyz* partial = &work.partial;
    cXyz* averagePlus = &work.averagePlus;
    cXyz* averageMinus = &work.averageMinus;
    cXyz* scaled = &work.scaled;
    cXyz* norm2 = &work.norm2;
    for (u32 i = 0; i < mNumLines; ++i, ++line) {
        f32 half = lineHalf;
        be<u8>* widths = line->mpSize;
        cXyz* point = line->mpSegments;
        if (!widths)
            JUTAssertion_showAssert("m_Do_ext.cpp", 0x1C03, "size_p != (0)");
        *temp = point[1] - point[0];
        tangent->copy(*temp);
        cross->copy(*temp);
        *temp = point[0] - camera->mEye;
        eye->copy(*temp);
        *temp = cross->outprod(*eye);
        cross->copy(*temp);
        PSVECSquareMag(cross);
        f32 magnitude = lineSquareRoot();
        *temp = tangent->outprod(*cross);
        tangent->copy(*temp);
        f32 nx, ny, nz;
        if (lineNonzero(magnitude)) {
            lineNormalize(tangent, norm);
            tangent->copy(*norm);
            lineNormalize(cross, norm);
            nz = norm->z;
            normal->z = nz;
            nx = norm->x;
            normal->x = nx;
            ny = norm->y;
            normal->y = ny;
            PSVECScale(cross, cross, static_cast<f32>(*widths) / magnitude);
        } else {
            nz = normal->z;
            nx = normal->x;
            ny = normal->y;
        }
        MeshBuffer* root = line->mpMeshBuffer;
        VertexTable* table = root->getVertices();
        Vertex* destination = table->mpVertices;
        *out = point[0] + *cross;
        copyPosition(&destination->mPosition, out);
        setAttributes(table, 0, tangent, nx, ny, nz, zero, true);
        destination = table->getVertex(1);
        *out = point[0] - *cross;
        copyPosition(&destination->mPosition, out);
        setAttributes(table, 1, tangent, nx, ny, nz, zero, false);
        ++point;
        ++widths;
        u32 index = 2;
        *temp = point[0] + *cross;
        averagePlus->copy(*temp);
        *temp = point[0] - *cross;
        averageMinus->copy(*temp);
        s32 remaining = static_cast<s32>(mNumSegments) - 2;
        for (s32 n = remaining; n > 0; --n) {

            *partial = point[1] - point[0];
            cross->copy(*partial);
            tangent->copy(*partial);
            *partial = point[0] - camera->mEye;
            eye->copy(*partial);
            *partial = cross->outprod(*eye);
            cross->copy(*partial);
            PSVECSquareMag(cross);
            magnitude = lineSquareRoot();
            *partial = tangent->outprod(*cross);
            tangent->copy(*partial);
            if (lineNonzero(magnitude)) {
                lineNormalize(tangent, norm2);
                tangent->copy(*norm2);
                lineNormalize(cross, norm2);
                nx = norm2->x;
                ny = norm2->y;
                normal->x = nx;
                nz = norm2->z;
                normal->y = ny;
                magnitude = static_cast<f32>(*widths) / magnitude;
                normal->z = nz;
            }
            PSVECScale(cross, cross, magnitude);
            *partial = point[0] + *cross;
            lineAddVectors(averagePlus, partial, averagePlus);
            *partial = point[0] - *cross;
            lineAddVectors(averageMinus, partial, averageMinus);
            destination = table->getVertex(index);
            lineScale(averagePlus, scaled, half);
            copyPosition(&destination->mPosition, scaled);
            setAttributes(table, index, tangent, nx, ny, nz, zero, true);
            ++index;
            destination = table->getVertex(index);
            lineScale(averageMinus, scaled, half);
            copyPosition(&destination->mPosition, scaled);
            setAttributes(table, index, tangent, nx, ny, nz, zero, false);
            ++point;
            ++widths;
            ++index;
            *partial = point[0] + *cross;
            averagePlus->copy(*partial);
            *partial = point[0] - *cross;
            averageMinus->copy(*partial);
        }
        cXyz* last = averagePlus;
        destination = table->getVertex(index);
        copyPosition(&destination->mPosition, last);
        setAttributes(table, index, tangent, nx, ny, nz, zero, false);
        ++index;
        destination = table->getVertex(index);
        copyPosition(&destination->mPosition, averageMinus);
        setAttributes(table, index, tangent, nx, ny, nz, zero, true);
    }
    // HD: finalization changes each mesh's selected buffer; do it after all geometry.
    for (u32 i = 0; i < mNumLines; ++i)
        lineFinalize(mpLines[i].mpMeshBuffer, 0, 1);
    lineMaterialSetup(this, mpTevStr, &mColor);
}

/* WWHD glue (tools/verify/autoglue.py)
data lineZero 100586DC
data lineHalf 100586D8
str 10058BAC
str 10058B9C
ext 025EA530 u32 lineNonzero(f32 value)
ext 028F4384 f32 lineSquareRoot()
ext 0201B084 void lineNormalize(cXyz* vector, cXyz* result)
ext 0201AE48 void lineScale(cXyz* vector, cXyz* result, f32 scale)
ext 028E8D88 void lineAddVectors(cXyz* a, cXyz* b, cXyz* result)
ext 027FF1D8 void lineFinalize(line_layout::MeshBuffer* buffer, u32 first, u32 count)
ext 025E9670 void lineMaterialSetup(line_layout::mDoExt_3DlineMat0_c* material, dKy_tevstr_c* lighting, be<u32>* color)
*/
