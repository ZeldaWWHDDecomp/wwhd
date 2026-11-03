// AGL pixel-transfer records, derived from the assigned WWHD instructions.
#pragma once
#include "agl/VertexInput.h"
#include "agl/CaptureImage.h"
#include "sead/Viewport.h"
namespace agl {
// Exact embedded storage extent from the owning transfer object's constructor.
// AGL#14 owns the color-buffer methods; the bytes are not a duplicate complete class definition.
struct TransferColorStorage_l { u8 mStorage[0x148]; };
struct PixelChannels { be<u32> mValues[4]; };
struct PixelBufferTransfer {
    gptr<be<u32>> mPixels;
    be<u32> mFormat,mRowStride;
    TransferColorStorage_l mColorBuffer;
    gptr<sead::DisposerVtable> mVtable;
    static PixelBufferTransfer* create(PixelBufferTransfer* storage);
    void release();
    void finish();
    void destroy(u32 flags);
    void readChannels(PixelChannels* output,u32 x,u32 y);
    void prepare(const CaptureSurfacePrefix_l*,sead::RegistryHeap*);
    void copy(const CaptureSurfacePrefix_l*);
};
}
WWHD_SIZE(agl::TransferColorStorage_l,0x148);
WWHD_SIZE(agl::PixelChannels,16);
WWHD_SIZE(agl::PixelBufferTransfer,0x158);
WWHD_OFFSET(agl::PixelBufferTransfer,mFormat,4);
WWHD_OFFSET(agl::PixelBufferTransfer,mRowStride,8);
WWHD_OFFSET(agl::PixelBufferTransfer,mColorBuffer,0xC);
WWHD_OFFSET(agl::PixelBufferTransfer,mVtable,0x154);
