#pragma once
#include "wwhd.h"
namespace mDoExtHeap {
enum HeapType : u32 { HEAP_SOLID=0x534C4944, HEAP_UNIT=0x554E4954 };
enum RenderFlag : u32 { VIEW_OVERRIDE_ENABLED=0x40, VIEW_OVERRIDE_INLINE=0x80, PROJECTION_CUSTOM=0x1000 };
// HD: slot labels describe their use in this unit's allocation/adjustment paths.
enum VirtualSlot : u32 { DESTROY=0x24, ALLOCATE=0x34, GET_START=0x5C, ADJUST_QUERY=0x6C, PARENT_CAN_ADJUST=0x94, CAN_ADJUST=0x9C };

struct Address {
  u8 opaque;
};
struct Word {
  be<u32> mValue;
};
struct Byte {
  be<u8> mValue;
};
struct HeapSlots {
  be<u32> mHeap[2];
};
struct HeapNames {
  be<u32> mName[2];
};
struct HeapBounds {
  be<u32> mSolidHead, mSolidTail;
};
// HD: debug name, direct parent and allocation flags precede solid-heap bounds.
struct Heap {
  u8 padding0[0xC];
  be<u32> mpVtable, mName;
  u8 padding14[0x10];
  be<u32> mpParent;
  u8 padding28[0x68];
  be<u32> mAllocationMode, mSolidHead, mSolidTail;
};
struct HeapVtable {
  be<u32> mSlots[40];
};
struct ViewMatrix {
  be<u32> mWords[12];
};
struct ProjectionMatrix {
  be<u32> mWords[16];
};
struct RecordArray {
  u8 records[2][8];
};
struct RenderManager {
  u8 padding0[0x1020];
  be<u32> mViewCount, mpViewList;
};
struct RenderView {
  u8 padding0[0x48];
  be<u32> mpViewMatrix, mpProjectionMatrix, mFlags;
  u8 padding54[0x30];
  ViewMatrix mViewMatrix;
  u8 paddingB4[0xB0];
  be<u32> mpCustomProjection;
};
WWHD_OFFSET(Heap, mSolidHead, 0x94);
WWHD_OFFSET(RenderView, mpCustomProjection, 0x164);
WWHD_SIZE(ViewMatrix, 48);
WWHD_SIZE(ProjectionMatrix, 64);
WWHD_SIZE(RecordArray, 16);
} // namespace mDoExtHeap
template <class T> inline T *mDoExtHeap_at(u32 address) {
  return gabi::at<T>(address);
}
/* inlined into mDoExt_alignSolidHeap, mDoExt_createSolidHeap,
 * mDoExt_adjustSolidHeap and mDoExt_destroySolidHeap. HD virtual-slot layout. */
inline u32 mDoExtHeap_virtualTarget(u32 object, u32 slot) {
  auto *heap = mDoExtHeap_at<mDoExtHeap::Heap>(object);
  return mDoExtHeap_at<mDoExtHeap::HeapVtable>(heap->mpVtable)
      ->mSlots[slot / 4];
}
u32 mDoExtHeap_current();
u32 mDoExtHeap_type(u32);
void mDoExtHeap_report(void *);
void mDoExtHeap_assert(void *, u32, void *);
void mDoExtHeap_modelUpdate(void *);
void mDoExtHeap_drawModelCall(void *, void *);
void mDoExtHeap_modelEntry(void *);
u32 mDoExtHeap_getGameCall(s32);
u32 mDoExtHeap_getGame0Call();
u32 mDoExtHeap_setCurrent(u32);
void mDoExtHeap_setGameCall(s32);
void mDoExtHeap_copyViewCall(void *, void *);
u32 mDoExtHeap_projectionMtx(void *);
void mDoExtHeap_copyProjection(void *, void *);
void mDoExtHeap_draw(void *);
u32 mDoExtHeap_createExp(u32, void *, u32);
u32 mDoExtHeap_getCommandCall();
u32 mDoExtHeap_createSolidCall(u32, u32, u32);
u32 mDoExtHeap_becomeCurrent(void *);
u32 mDoExtHeap_setCurrentCall(void *);
u32 mDoExtHeap_createSolidToCurrentCall(u32, u32, u32);
u32 mDoExtHeap_adjustSolidCall(void *);
void mDoExtHeap_restoreCurrentCall();
void mDoExtHeap_constructRecords(void *, u32, u32, void *, u32, u32);
void mDoExtHeap_restoreHeapBounds(void *, void *);
u32 mDoExtHeap_headerSize(u32);
u32 mDoExtHeap_createSolid(u32, void *, u32);
void mDoExtHeap_alignSolidCall(void *, u32);
u32 mDoExtHeap_adjustSize(void *);
inline u32 mDoExtHeap_type(void *heap) {
  return mDoExtHeap_type(gabi::ea(heap));
}
inline u32 mDoExtHeap_createSolidCall(u32 size, void *parent, u32 align) {
  return mDoExtHeap_createSolidCall(size, gabi::ea(parent), align);
}
