// WWHD viewport mapping, reconstructed from the local game instructions.
#include "sead/Viewport.h"
#include <cfloat>

static u32 sViewportTable;
static u32 sDefaultDevicePosture;
void* allocateViewport(u32 size);

/* 0274F358 */
sead::Viewport* sead::Viewport::createEmpty(Viewport* object) {
    if (object == NULL) object = (Viewport*)allocateViewport(sizeof(Viewport));
    if (object == NULL) return object;
    object->mBottom = FLT_MAX;
    object->mRight = -FLT_MAX;
    object->mLeft = FLT_MAX;
    object->mTop = -FLT_MAX;
    object->mTable = &sViewportTable;
    object->mBottom = FLT_MAX;
    object->mOrientation = sDefaultDevicePosture;
    object->mTop = -FLT_MAX;
    object->mLeft = FLT_MAX;
    object->mRight = -FLT_MAX;
    return object;
}

/* 0274F424 */
sead::Viewport* sead::Viewport::createRect(Viewport* object, f32 x, f32 y, f32 width, f32 height) {
    if (object == NULL) object = (Viewport*)allocateViewport(sizeof(Viewport));
    if (object == NULL) return object;
    f32 right = gabi::fadds_ppc(x, width);
    f32 top = gabi::fadds_ppc(y, height);
    if (x < right) {
        object->mRight = right;
        object->mLeft = x;
    } else {
        object->mRight = x;
        object->mLeft = right;
    }
    // The unordered branches follow PowerPC's bge/blt tests.
    if (y < top) {
        object->mTop = top;
        object->mBottom = y;
    } else {
        object->mBottom = top;
        object->mTop = y;
    }
    object->mTable = &sViewportTable;
    object->mOrientation = sDefaultDevicePosture;
    return object;
}

/* 0274F53C */
void sead::Viewport::setSize(const RenderVector2* size) {
    u32 orientation = mOrientation;
    if (orientation > 5) return;
    f32 width, height;
    if (orientation == 1 || orientation == 2) {
        width = size->y;
        height = size->x;
    } else {
        width = size->x;
        height = size->y;
    }
    if (0.0f < width) {
        mRight = width;
        mLeft = 0.0f;
    } else {
        mRight = 0.0f;
        mLeft = width;
    }
    if (0.0f < height) {
        mBottom = 0.0f;
        mTop = height;
    } else {
        mBottom = height;
        mTop = 0.0f;
    }
}

/* 0274F5F0 */
sead::Viewport* sead::Viewport::createSurface(Viewport* object, const RenderVector2* size) {
    if (object == NULL) object = (Viewport*)allocateViewport(sizeof(Viewport));
    if (object == NULL) return object;
    object->mLeft = FLT_MAX;
    object->mRight = -FLT_MAX;
    object->mBottom = FLT_MAX;
    object->mTop = -FLT_MAX;
    object->mTable = &sViewportTable;
    object->mOrientation = sDefaultDevicePosture;
    object->setSize(size);
    return object;
}

/* 0274F698 */
void sead::Viewport::mapPosition(RenderVector2* output, const RenderSurface* surface) const {
    reinterpret_cast<be<u32>&>(output->x) = (u32)reinterpret_cast<const be<u32>&>(mLeft);
    reinterpret_cast<be<u32>&>(output->y) = (u32)reinterpret_cast<const be<u32>&>(mBottom);
    u32 orientation = mOrientation;
    switch (orientation) {
    case 1: {
        f32 left = mLeft, right = mRight, height = surface->mHeight;
        f32 width = gabi::fsubs_ppc(right, left);
        f32 x = output->x;
        f32 base = gabi::fsubs_ppc(height, width);
        reinterpret_cast<be<u32>&>(output->x) = (u32)reinterpret_cast<const be<u32>&>(output->y);
        output->y = gabi::fsubs_ppc(base, x);
        break;
    }
    case 2: {
        f32 bottom = mBottom, top = mTop;
        f32 height = gabi::fsubs_ppc(top, bottom);
        f32 width = surface->mWidth;
        f32 base = gabi::fsubs_ppc(width, height);
        f32 x = gabi::fsubs_ppc(base, (f32)output->y);
        reinterpret_cast<be<u32>&>(output->y) = (u32)reinterpret_cast<const be<u32>&>(output->x);
        output->x = x;
        break;
    }
    case 3: {
        f32 left = mLeft, bottom = mBottom, right = mRight, top = mTop;
        f32 width = gabi::fsubs_ppc(right, left);
        f32 surfaceWidth = surface->mWidth;
        f32 height = gabi::fsubs_ppc(top, bottom);
        f32 surfaceHeight = surface->mHeight;
        f32 baseX = gabi::fsubs_ppc(surfaceWidth, width);
        f32 oldX = output->x;
        f32 baseY = gabi::fsubs_ppc(surfaceHeight, height);
        f32 x = gabi::fsubs_ppc(baseX, oldX);
        f32 y = gabi::fsubs_ppc(baseY, (f32)output->y);
        output->x = x;
        output->y = y;
        break;
    }
    case 4: {
        f32 right = mRight, left = mLeft;
        f32 width = gabi::fsubs_ppc(right, left);
        f32 base = gabi::fsubs_ppc((f32)surface->mWidth, width);
        f32 x = gabi::fsubs_ppc(base, (f32)output->x);
        reinterpret_cast<be<u32>&>(output->y) = (u32)reinterpret_cast<const be<u32>&>(output->y);
        output->x = x;
        break;
    }
    case 5: {
        f32 bottom = mBottom, top = mTop, surfaceHeight = surface->mHeight;
        f32 height = gabi::fsubs_ppc(top, bottom);
        f32 base = gabi::fsubs_ppc(surfaceHeight, height);
        output->y = gabi::fsubs_ppc(base, (f32)output->y);
        break;
    }
    default: break;
    }
    f32 x = gabi::fdivs_ppc((f32)output->x, (f32)surface->mWidth);
    output->x = x;
    f32 y = gabi::fdivs_ppc((f32)output->y, (f32)surface->mHeight);
    output->y = y;
    f32 width = gabi::fsubs_ppc((f32)surface->mRight, (f32)surface->mLeft);
    f32 mappedX = gabi::fmuls_ppc(x, width);
    output->x = mappedX;
    f32 height = gabi::fsubs_ppc((f32)surface->mTop, (f32)surface->mBottom);
    f32 mappedY = gabi::fmuls_ppc(y, height);
    output->y = mappedY;
    output->x = gabi::fadds_ppc(mappedX, (f32)surface->mLeft);
    output->y = gabi::fadds_ppc(mappedY, (f32)surface->mBottom);
}

/* 0274F8D8 */
void sead::Viewport::mapExtent(RenderVector2* output, const RenderSurface* surface) const {
    f32 right = mRight, left = mLeft, top = mTop, bottom = mBottom;
    f32 width = gabi::fsubs_ppc(right, left);
    f32 height = gabi::fsubs_ppc(top, bottom);
    output->x = width;
    output->y = height;
    u32 orientation = mOrientation;
    if (orientation >= 1 && orientation <= 2) {
        f32 oldX = output->x;
        output->x = height;
        output->y = oldX;
    }
    f32 x = gabi::fdivs_ppc((f32)output->x, (f32)surface->mWidth);
    output->x = x;
    f32 y = gabi::fdivs_ppc((f32)output->y, (f32)surface->mHeight);
    output->y = y;
    f32 mappedWidth = gabi::fsubs_ppc((f32)surface->mRight, (f32)surface->mLeft);
    output->x = gabi::fmuls_ppc(x, mappedWidth);
    f32 mappedHeight = gabi::fsubs_ppc((f32)surface->mTop, (f32)surface->mBottom);
    output->y = gabi::fmuls_ppc(y, mappedHeight);
}

/* 0274FAE0 */
void sead::Viewport::scaledHalfExtent(RenderVector2* output, const RenderVector2* scale) const {
    f32 width = gabi::fsubs_ppc((f32)mRight, (f32)mLeft);
    f32 halfWidth = gabi::fmuls_ppc(width, 0.5f);
    output->x = gabi::fmuls_ppc(halfWidth, (f32)scale->x);
    f32 height = gabi::fsubs_ppc((f32)mTop, (f32)mBottom);
    f32 halfHeight = gabi::fmuls_ppc(height, 0.5f);
    output->y = gabi::fmuls_ppc(halfHeight, (f32)scale->y);
}

/* WWHD glue
 data sViewportTable 10145368
 data sDefaultDevicePosture 101F8B1C
 ext 0273AD10 void* allocateViewport(u32)
*/
