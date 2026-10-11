/* Utility projections and HD material helpers. */
#include "m_Do/m_Do_lib.h"

static f32 colorScale;
static f32 linearScale;
static f32 zero;
static f32 half;
static f32 large;
static f32 screenCenterX;
static f32 screenWidth;
static f32 screenCenterY;
static f32 screenHeight;
static f32 degreesToAngle;
static f32 initialLow;
static f32 initialHigh;
static f32 mSystemFar;
static f32 mFovyRate;
static J3DUClipper mClipper;
static u32 clipperVtable;
static mDoLibSinCosTable sincosTable;
static mDoLibCameraSystemPointer cameraSystem;
static Mtx34 defaultView;
static mDoLibMatrix44 defaultProjection;
static mDoLibViewRecord viewRecord;
static mDoLibInitWords initializedWords;
static mDoLibInitFloats initializedFloats;
static mDoLibRegistration wordsRegistration;
static mDoLibRegistration byteRegistration;
static mDoLibRegistration wideRegistration;
static u8 byteStringState;
static u8 wideStringState;

static mDoLibGameView* libGameView() {
    return reinterpret_cast<mDoLibGameView*>(dComIfGp_get());
}

// HD: later call effects can overwrite only part of a depth word, exposing a NaN
// payload. The two reference multiply-add sites choose product/accumulator
// NaNs in different host operand orders; preserve each observed site order.
static f32 projectMadProduct(f32 a,f32 c,f32 b) {
    if (std::isnan(a)) return gabi::ppc_qnan(a);
    if (std::isnan(c)) return gabi::ppc_qnan(c);
    if (std::isnan(b)) return gabi::ppc_qnan(b);
    return gabi::fmadds(a,c,b);
}
static f32 projectMadAccumulator(f32 a,f32 c,f32 b) {
    if (std::isnan(b)) return gabi::ppc_qnan(b);
    return projectMadProduct(a,c,b);
}


/* 025F0A1C */
void mDoLib_colorToFloat(mDoLibColor* out, GXColor* color) {
    f32 scale = colorScale;
    f32 r = f32(color->r) / scale;
    f32 g = f32(color->g) / scale;
    f32 b = f32(color->b) / scale;
    f32 a = f32(color->a) / scale;
    out->r = r;
    out->g = g;
    out->b = b;
    out->a = a;
}

/* 025F0AD0 */
// HD: convert the material's alpha color to the linear render-target color.
void mDoLib_setMaterialColor(mDoLibMaterial* model, u32 alpha, u32 blend) {
    gabi::Local<mDoLibColor> color, linear;
    mDoLibResource* resource = model->resource;
    u32 offset = resource->mRenderStateOffset;
    mDoLibRenderState* state = offset ? gabi::at<mDoLibRenderState>(
        gabi::ea(&resource->mRenderStateOffset) + offset) : NULL;
    libSetRenderMode(state, 3);
    libSetAlphaCompareA(&state->alphaState, 4);
    libSetAlphaCompareB(&state->alphaState, 4);
    libSetAlphaOperationA(&state->alphaState, 5);
    libSetAlphaOperationB(&state->alphaState, 5);
    libSetBlendState(&state->blendState, blend ? 1 : 0);
    mDoLibMaterialBlock* block = model->block;
    gfn* vtable = block->vtable;
    GXColor* rgba = gabi::call_ptr<GXColor*>(vtable[mDoLibMaterial_GetColor].get(), block, 3u);
    rgba->a = u8(alpha);
    block = model->block;
    vtable = block->vtable;
    gabi::call_ptr<void>(vtable[mDoLibMaterial_SetColor].get(), block, 3u, rgba);
    mDoLib_colorToFloat(color.get(), rgba);
    f32 one = linearScale;
    libLinearColor(linear.get(), color.get(), one);
    model->target.flags = u32(model->target.flags) | mDoLibMaterial_LinearColor;
    mDoLibColor* target = libMaterialTarget(&model->target, 10);
    f32 a = f32(rgba->a) / colorScale;
    f32 y = linear->g;
    f32 x = linear->r;
    f32 z = linear->b;
    target->g = y;
    target->b = z;
    target->r = x;
    target->a = a;
}

/* 025F0C48 */
void mDoLib_project(cXyz* src, cXyz* dst) {
    mDoLibGameView* game = libGameView();
    f32 z0 = zero;
    if (game->mpView.get() == NULL) {
        dst->y = z0;
        dst->z = z0;
        dst->x = z0;
        return;
    }
    mDoLibMatrix44* matrix = dComIfGd_getProjViewMtx();
    cMtx_multVec(matrix, src, dst);
    // HD: keep each accessor call and load boundary: callees may modify the inputs.
    matrix = dComIfGd_getProjViewMtx();
    f32 x = src->x;
    f32 m0 = matrix->m[3][0];
    matrix = dComIfGd_getProjViewMtx();
    f32 y = src->y;
    f32 m1 = matrix->m[3][1];
    f32 calcFloat = projectMadProduct(x, m0, gabi::fmuls_ppc(y, m1));
    matrix = dComIfGd_getProjViewMtx();
    f32 z = src->z;
    f32 m2 = matrix->m[3][2];
    calcFloat = projectMadAccumulator(z, m2, calcFloat);
    matrix = dComIfGd_getProjViewMtx();
    f32 depth = dst->z;
    f32 m3 = matrix->m[3][3];
    calcFloat = gabi::fadds_ppc(calcFloat, m3);
    // HD: unordered depth also clamps to zero.
    if (!(depth < z0)) depth = z0;
    f32 halfValue = half;
    f32 f3;
    if (calcFloat > z0) {
        f3 = halfValue / calcFloat;
        dst->z = depth * f3;
    } else {
        f32 largeValue = large;
        if (calcFloat == z0) dst->z = depth * largeValue;
        else dst->z = depth * (halfValue / calcFloat);
        f3 = largeValue;
    }
    game = libGameView();
    view_port_class* viewPort = game->mpViewPort;
    f32 xOffset = viewPort->mXOrig;
    f32 yOffset = viewPort->mYOrig;
    f32 xSize = viewPort->mWidth;
    f32 ySize;
    if (xOffset != z0) {
        ySize = viewPort->mHeight;
        xOffset = gabi::fmsubs(xOffset + xOffset + xSize, halfValue, screenCenterX);
        xSize = screenWidth;
    } else ySize = viewPort->mHeight;
    f32 px = dst->x;
    f32 py = dst->y;
    if (yOffset != z0) {
        yOffset = gabi::fmsubs(yOffset + yOffset + ySize, halfValue, screenCenterY);
        ySize = screenHeight;
    }
    f32 sx = gabi::fmadds(px, f3, halfValue);
    f32 sy = gabi::fnmsubs(py, f3, halfValue);
    dst->x = gabi::fmadds(sx, xSize, xOffset);
    dst->y = gabi::fmadds(sy, ySize, yOffset);
}

/* 025F0EA4 */
// HD: project through the active HD camera and screen rectangle.
void mDoLib_projectScreen(cXyz* src, cXyz* dst) {
    gabi::Local<mDoLibRectangle> rectangle;
    gabi::Local<cXyz> point;
    gabi::Local<mDoLibPoint> screen;
    mDoLibCameraSystem* system = cameraSystem.system;
    u32 count = system->cameraCount;
    gptr<mDoLibCamera>* cameras = system->cameras;
    if (count > 1) ++cameras;
    mDoLibCamera* camera = *cameras;
    u32 flags = camera->flags;
    Mtx34* view;
    if ((flags & mDoLibCamera_InlineViewA) && (flags & mDoLibCamera_InlineViewB)) view = &camera->inlineView;
    else {
        view = camera->view;
        if (view == NULL) view = &defaultView;
    }
    mDoLibMatrix44* projection;
    if (flags & mDoLibCamera_IndirectProjection) projection = camera->indirectProjection;
    else {
        projection = camera->projection;
        if (projection == NULL) projection = &defaultProjection;
    }
    f32 z0 = zero;
    f32 width = screenWidth;
    f32 height = screenHeight;
    libRectangleConstruct(rectangle.get(), z0, z0, width, height);
    f32 x = src->x;
    f32 y = src->y;
    f32 z = src->z;
    point->x = x;
    point->y = y;
    point->z = z;
    libProjectScreen(view, screen.get(), point.get(), projection, rectangle.get());
    f32 sx = screen->x;
    f32 sy = screen->y;
    dst->x = sx;
    dst->z = z0;
    dst->y = sy;
}

/* 025F1018 */
// HD: temporarily override the viewport around the projection call.
void mDoLib_projectViewport(cXyz* src, cXyz* dst, view_port_class* viewport) {
    mDoLibGameView* game = libGameView();
    view_port_class* old = game->mpViewPort;
    game = libGameView();
    game->mpViewPort = viewport;
    mDoLib_project(src, dst);
    game = libGameView();
    game->mpViewPort = old;
}

/* 025F1084 */
// HD: copy the current view rectangle and clear its scissor words.
void mDoLib_copyViewRecord(view_port_class* out) {
    f32 y = viewRecord.mYOrig;
    f32 depth = viewRecord.mXOrig;
    f32 width = viewRecord.mWidth;
    f32 height = viewRecord.mHeight;
    f32 upper = viewRecord.mFarZ;
    f32 lower = viewRecord.mNearZ;
    out->mXOrig = depth;
    out->mYOrig = y;
    out->mWidth = height;
    out->mHeight = width;
    out->mNearZ = lower;
    out->mFarZ = upper;
    out->mScissor.mXOrig = 0;
    out->mScissor.mYOrig = 0;
    out->mScissor.mWidth = 0;
    out->mScissor.mHeight = 0;
}

/* 025F1108 */
void mDoLib_pos2camera(cXyz* src, cXyz* dst) {
    mDoLibGameView* game = libGameView();
    view_class* view = game->mpView;
    cMtx_multVec(&view->mViewMtx, src, dst);
}

/* 025F1154 */
u32 mDoLib_cnvind32(u32 r3) {
    return (r3 >> 24) | ((r3 >> 8) & 0xFF00) | ((r3 << 8) & 0xFF0000) | (r3 << 24);
}

/* 025F1188 */
u16 mDoLib_cnvind16(u32 r3) {
    // HD: retain the full argument register before narrowing the swapped result.
    return u16((r3 >> 8) & 0xFF) | u16(r3 << 8);
}

/* 025F11AC */
// HD: explicit static-function ABI entry for mDoLib_clipper::setup.
void mDoLib_clipper_setup(f32 fovY, f32 aspect, f32 n, f32 f) {
    // HD: system far is stored before the clipper fields.
    mSystemFar = f;
    mClipper.mFovY = fovY;
    mClipper.mAspect = aspect;
    mClipper.mNear = n;
    mClipper.mFar = f;
    libClipperCalc(&mClipper);
    u32 index = u16(gabi::ftoi(fovY * degreesToAngle));
    f32 sn = sincosTable.entries[index >> 3].sine;
    f32 cs = sincosTable.entries[index >> 3].cosine;
    mFovyRate = cs / sn;
}

/* 025F124C */
static void __sinit_m_Do_lib_cpp() {
    // HD: compiler header statics precede clipper construction.
    initializedWords.third = 0;
    initializedWords.first = 0;
    initializedWords.fourth = 0;
    initializedWords.second = 0;
    libRegisterGlobal(&wordsRegistration);
    f32 lo = initialLow;
    f32 hi = initialHigh;
    initializedFloats.low = lo;
    initializedFloats.high = hi;
    libInitializeByteState(&byteStringState);
    libRegisterGlobal(&byteRegistration);
    libInitializeWideState(&wideStringState);
    libRegisterGlobal(&wideRegistration);
    mClipper.__vtbl = gabi::ea(&clipperVtable);
    libClipperInit(&mClipper);
}

/* WWHD glue
data colorScale 10058F68
data linearScale 10058F78
data zero 10058F90
data half 10058F94
data large 10058F98
data screenCenterX 10058FA0
data screenWidth 10058FA4
data screenCenterY 10058FA8
data screenHeight 10058FAC
data degreesToAngle 10058FB0
data initialLow 10058FB8
data initialHigh 10058FBC
data mSystemFar 1048D04C
data mFovyRate 1048D050
data mClipper 1048CFF0
data clipperVtable 10058F80
data sincosTable 104A44F8
data cameraSystem 101F95D0
data defaultView 104A2098
data defaultProjection 104A20FC
data viewRecord 104B45C0
data initializedWords 1048CFE0
data initializedFloats 1048CFD4
data wordsRegistration 101F4850
data byteRegistration 101F485C
data wideRegistration 101F4868
data byteStringState 1048CFDC
data wideStringState 1048CFDD
ext 027E212C void libSetRenderMode(mDoLibRenderState*, u32)
ext 027E19B4 void libSetAlphaCompareA(mDoLibAlphaState*, u32)
ext 027E19E4 void libSetAlphaCompareB(mDoLibAlphaState*, u32)
ext 027E19C4 void libSetAlphaOperationA(mDoLibAlphaState*, u32)
ext 027E19F4 void libSetAlphaOperationB(mDoLibAlphaState*, u32)
ext 027E18F8 void libSetBlendState(be<u32>*, u32)
ext 0274D458 void libLinearColor(mDoLibColor*, mDoLibColor*, f32)
ext 027F9F0C mDoLibColor* libMaterialTarget(mDoLibMaterialTarget*, u32)
ext 02524628 mDoLibMatrix44* dComIfGd_getProjViewMtx()
ext 028E8F64 void cMtx_multVec(void*, const cXyz*, cXyz*)
ext 0274F424 void libRectangleConstruct(mDoLibRectangle*, f32, f32, f32, f32)
ext 0274CB50 void libProjectScreen(Mtx34*, mDoLibPoint*, cXyz*, mDoLibMatrix44*, mDoLibRectangle*)
ext 0283801C void libClipperCalc(J3DUClipper*)
ext 02838000 void libClipperInit(J3DUClipper*)
ext 028F026C void libRegisterGlobal(mDoLibRegistration*)
ext 028ED6F8 void libInitializeByteState(void*)
ext 028EAB2C void libInitializeWideState(void*)
*/
