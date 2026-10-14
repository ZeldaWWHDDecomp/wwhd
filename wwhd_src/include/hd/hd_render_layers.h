/** HD renderer layer factories and shader support. Library classes remain opaque. */
#pragma once
#include "SSystem/SComponent/c_m2d_g_box.h"
#include "hd/hd_render_resources.h"
struct hdRenderLayerFactory_c {
    hdLayerBase_c* createShadow(u32 index, void* owner, u32 flags, void* heap);
    hdLayerBase_c* createScene(u32 index, void* owner, u32 flags, void* heap);
    hdLayerBase_c* createSceneMask(u32 index, void* owner, u32 flags, void* heap);
    hdLayerBase_c* createUI(u32 index, void* owner, u32 flags, void* heap);
    hdLayerBase_c* createParticle(u32 index, void* owner, u32 flags, void* heap);
    hdLayerBase_c* createUIForeground(u32 index, void* owner, u32 flags, void* heap);
    hdLayerBase_c* createFade(u32 index, void* owner, u32 flags, void* heap);
    hdLayerBase_c* createDRCScene(u32 index, void* owner, u32 flags, void* heap);
    hdLayerBase_c* createDRCSceneMask(u32 index, void* owner, u32 flags, void* heap);
    hdLayerBase_c* createDRCUI(u32 index, void* owner, u32 flags, void* heap);
    hdLayerBase_c* createDRCUIForeground(u32 index, void* owner, u32 flags, void* heap);
};
struct hdLayerMatrix_l {
    be<u32> mValues[9];
};
WWHD_SIZE(hdLayerMatrix_l, 0x24);
struct hdLayerMatrixBuffer_l {
    be<u32> mCount;
    be<u32> mCapacity;
    gptr<gptr<hdLayerMatrix_l>> mpMatrices;
    void copyMatrices(hdLayerMatrix_l* matrices);
};
WWHD_OFFSET(hdLayerMatrixBuffer_l, mpMatrices, 8);
WWHD_SIZE(hdLayerMatrixBuffer_l, 12);

struct hdShaderLocation_l {
    be<u32> mName;
    be<u32> mVtable;
    be<u16> mLocations[4];
};
WWHD_SIZE(hdShaderLocation_l, 0x10);
struct hdShaderBackend_c {
    u8 mDisposer[12];
    be<u32> mVtable;
    be<u32> mArchiveHeader[4];
    gptr<struct hdShaderProgram_l> mpPrograms[7];
    u8 mTextureState[0xF4];
    hdShaderLocation_l mLocations[14];
    be<u32> mVertexState[0x24];
    be<u32> mIndexState[0x66];
    be<u8> mReady;
    u8 mPadding[3];
    static hdShaderBackend_c* create(void* heap);
    void* prepareDepth(void* texture, void* previous, f32 nearClip, f32 farClip);
    void* prepareColor(void* texture, void* unused, void* color, void* transform, void* previous);
    void* prepareScaled(void* texture, void* size, void* color, void* transform, void* previous);
    void* prepareMatrix(void* texture, void* matrix, void* previous);
    void destroy(u32 flags);
};
WWHD_OFFSET(hdShaderBackend_c, mTextureState, 0x3C);
WWHD_OFFSET(hdShaderBackend_c, mLocations, 0x130);
WWHD_OFFSET(hdShaderBackend_c, mVertexState, 0x210);
WWHD_OFFSET(hdShaderBackend_c, mIndexState, 0x2A0);
WWHD_SIZE(hdShaderBackend_c, 0x43C);
