#pragma once
#include "wwhd.h"
using namespace gabi;

// HD: private complete views; shared J3DGraph.h intentionally exposes only an opaque buffer API.
namespace drawBufferHD {
struct J3DPacket;
using PacketSlot = gptr<J3DPacket>;
struct PacketVtable {
    u8 padding[0x2C];
    gfn draw;
};
struct J3DPacket {
    /* 0x00 */ u8 padding0[0x0C];
    /* 0x0C */ gptr<PacketVtable> __vtbl;
    /* 0x10 */ gptr<J3DPacket> mpNextPacket;
    u8 padding14[0x80];
    /* 0x94 */ gptr<PacketSlot> mpEntrySlot; // HD: backlink to the owning bucket.
};
struct HeapVtable {
    u8 padding0[0x34];
    gfn alloc;
    u8 padding38[4];
    gfn free;
};
struct Heap {
    u8 padding0[0x0C];
    gptr<HeapVtable> __vtbl;
};
struct Vec {
    be<f32> x, y, z;
};
class J3DDrawBuffer {
  public:
    enum EDrawType { DRAW_HEAD, DRAW_TAIL };
    enum ESortType { SORT_MAT, SORT_MAT_ANM, SORT_Z, SORT_MODEL, SORT_INVALID, SORT_NON };
    // HD: retain the full r3 results, context arguments, and 32-bit entry indices.
    u32 initialize();
    u32 allocBuffer(u32 bufSize);
    u32 frameInit();
    u32 entryZSort(J3DPacket* i_packet);
    u32 entryNonSort(J3DPacket* i_packet);
    u32 entryImm(J3DPacket* i_packet, u32 index);
    u32 remove(J3DPacket* i_packet, u32 index);
    u32 draw(void* context);
    u32 drawHead(void* context);
    u32 drawTail(void* context);
    u32 isEmpty();
    PacketSlot* getEntrySlot(u32 index) {
        u32 count = mEntryTableSize;
        u32 base = mpBuffer.v.get();
        return at<PacketSlot>(base + (index < count ? index * 4u : 0u));
    }
    // HD: count and buffer swap their GameCube offsets.
    /* 0x00 */ be<u32> mEntryTableSize;
    /* 0x04 */ gptr<PacketSlot> mpBuffer;
    /* 0x08 */ be<u32> mDrawMode;
    /* 0x0C */ be<u32> mSortMode;
    /* 0x10 */ be<f32> mZNear;
    /* 0x14 */ be<f32> mZFar;
    /* 0x18 */ be<f32> mZRatio;
    /* 0x1C */ gptr<Mtx34> mpZMtx;
    /* 0x20 */ gptr<J3DPacket> mpCallBackPacket;
};
} // namespace drawBufferHD
WWHD_OFFSET(drawBufferHD::J3DPacket, __vtbl, 0x0C);
WWHD_OFFSET(drawBufferHD::J3DPacket, mpNextPacket, 0x10);
WWHD_OFFSET(drawBufferHD::J3DPacket, mpEntrySlot, 0x94);
WWHD_OFFSET(drawBufferHD::Heap, __vtbl, 0x0C);
WWHD_OFFSET(drawBufferHD::HeapVtable, alloc, 0x34);
WWHD_OFFSET(drawBufferHD::HeapVtable, free, 0x3C);
WWHD_OFFSET(drawBufferHD::PacketVtable, draw, 0x2C);
WWHD_SIZE(drawBufferHD::Vec, 0x0C);
WWHD_OFFSET(drawBufferHD::J3DDrawBuffer, mEntryTableSize, 0);
WWHD_OFFSET(drawBufferHD::J3DDrawBuffer, mpBuffer, 4);
WWHD_OFFSET(drawBufferHD::J3DDrawBuffer, mDrawMode, 8);
WWHD_OFFSET(drawBufferHD::J3DDrawBuffer, mSortMode, 0x0C);
WWHD_OFFSET(drawBufferHD::J3DDrawBuffer, mZNear, 0x10);
WWHD_OFFSET(drawBufferHD::J3DDrawBuffer, mZFar, 0x14);
WWHD_OFFSET(drawBufferHD::J3DDrawBuffer, mZRatio, 0x18);
WWHD_OFFSET(drawBufferHD::J3DDrawBuffer, mpZMtx, 0x1C);
WWHD_OFFSET(drawBufferHD::J3DDrawBuffer, mpCallBackPacket, 0x20);
WWHD_SIZE(drawBufferHD::J3DDrawBuffer, 0x24);
