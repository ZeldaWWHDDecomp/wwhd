/** Model rendering and heap utilities with tww names; private source. */
#include "m_Do/m_Do_ext_heap_model.h"
// HD: multiple game heaps, draw-time view/projection overrides and expanded heap layouts.
// HD: preserve word-address returns, unsigned alignment wrap and all virtual-call arguments.
static mDoExtHeap::Word l_previousGameHeap;
static mDoExtHeap::Word l_zeldaHeap;
static mDoExtHeap::Word l_previousZeldaHeap;
static mDoExtHeap::Word l_archiveHeap;
static mDoExtHeap::Word l_previousArchiveHeap;
static mDoExtHeap::Word l_commandHeap;
static mDoExtHeap::Word l_previousCommandHeap;
static mDoExtHeap::Word mDoExt_SaveCurrentHeap;
static mDoExtHeap::Word l_renderManager;
static mDoExtHeap::Word l_lastSolidHeap;
static mDoExtHeap::Word l_lastSolidAlignment;
static mDoExtHeap::Byte l_adjustCreatedHeap;
static mDoExtHeap::Address l_notSolidHeapMessage;
static mDoExtHeap::Address l_createGameHeapFile;
static mDoExtHeap::Address l_createGameIndexCondition;
static mDoExtHeap::Address l_gameHeapExistsCondition;
static mDoExtHeap::Address l_getGameHeapFile;
static mDoExtHeap::Address l_gameHeapIndexCondition;
static mDoExtHeap::Address l_zeldaHeapExistsCondition;
static mDoExtHeap::Address l_zeldaHeapName;
static mDoExtHeap::Address l_zeldaHeapFile;
static mDoExtHeap::Address l_archiveHeapExistsCondition;
static mDoExtHeap::Address l_archiveHeapName;
static mDoExtHeap::Address l_archiveHeapFile;
static mDoExtHeap::Address l_commandHeapExistsCondition;
static mDoExtHeap::Address l_commandHeapName;
static mDoExtHeap::Address l_commandHeapFile;
static mDoExtHeap::Address l_firstAllocationCondition;
static mDoExtHeap::Address l_alignHeapFile;
static mDoExtHeap::Address l_heapNotNullCondition;
static mDoExtHeap::Address l_setCurrentHeapFile;
static mDoExtHeap::Address l_savedHeapEmptyCondition;
static mDoExtHeap::Address l_solidToCurrentFile;
static mDoExtHeap::Address l_savedHeapNotNullCondition;
static mDoExtHeap::Address l_restoreCurrentFile;
static mDoExtHeap::HeapSlots gameHeap;
static mDoExtHeap::HeapNames l_heapNames;
static mDoExtHeap::ViewMatrix l_viewMatrix;
static mDoExtHeap::ProjectionMatrix l_projectionMatrix;
static mDoExtHeap::ViewMatrix l_defaultView;
static mDoExtHeap::ProjectionMatrix l_defaultProjection;
static mDoExtHeap::RecordArray l_gameHeapRecords;
static mDoExtHeap::RecordArray l_gameHeapAlignmentRecords;
static mDoExtHeap::Address l_recordConstructor;
/* 025E2AF8 */
u32 isCurrentSolidHeap() {

  u32 heap = mDoExtHeap_current();
  u32 type = mDoExtHeap_type(heap);
  if (type == mDoExtHeap::HEAP_SOLID)
    return 1;
  mDoExtHeap_report(gabi::at<void>(gabi::ea(&l_notSolidHeapMessage)));
  return 0;
}

/* 025E2B54 */
void mDoExt_copyViewMatrix(void *dst, void *src) {

  f32 v[12];
  for (u32 i = 0; i < 12; i++)
    v[i] = *mDoExtHeap_at<be<f32>>(gabi::ea(src) + i * 4);
  for (u32 i = 0; i < 12; i++)
    *mDoExtHeap_at<be<f32>>(gabi::ea(dst) + i * 4) = v[i];
}

/* 025E2DA8 */
void mDoExt_modelUpdate(void *self) {

  mDoExtHeap_modelUpdate(self);
  mDoExtHeap_drawModelCall(self, gabi::at<void>(0));
}

/* 025E2DE0 */
void mDoExt_modelUpdateDL(void *self, void *data) {

  mDoExtHeap_modelUpdate(self);
  mDoExtHeap_drawModelCall(self, data);
}

/* 025E2E24 */
void mDoExt_modelEntry(void *self) {

  mDoExtHeap_modelEntry(self);
  mDoExtHeap_drawModelCall(self, gabi::at<void>(0));
}

/* 025E2E5C */
void mDoExt_modelEntryDL(void *self) {

  mDoExtHeap_modelEntry(self);
  mDoExtHeap_drawModelCall(self, gabi::at<void>(0));
}

/* 025E2F64 */
u32 mDoExt_getGameHeapForIndex(s32 index) {

  if (index >= 2)
    mDoExtHeap_assert(gabi::at<void>(gabi::ea(&l_getGameHeapFile)), 0xB0E,
                      gabi::at<void>(gabi::ea(&l_gameHeapIndexCondition)));
  return *mDoExtHeap_at<be<u32>>(gabi::ea(&gameHeap) + (u32(index) << 2));
}

/* 025E2FBC */
u32 mDoExt_getGameHeap() { return mDoExtHeap_getGameCall(0); }

/* 025E2FC4 */
void mDoExt_setGameHeapForIndex(s32 ignored) {

  u32 heap = mDoExtHeap_getGame0Call();
  u32 prev = mDoExtHeap_setCurrent(heap);
  l_previousGameHeap.mValue = prev;
}

/* 025E2FF0 */
void mDoExt_setGameHeap() { mDoExtHeap_setGameCall(0); }

/* 025E2FF8 */
u32 mDoExt_getPreviousHeap() { return l_previousGameHeap.mValue; }

/* 025E2BF4 */
void mDoExt_drawModel(void *model, void *supplied) {

  gabi::Local<mDoExtHeap::ViewMatrix> savedView;
  gabi::Local<mDoExtHeap::ProjectionMatrix> savedProj;
  mDoExtHeap_copyViewCall(savedView.get(),
                          gabi::at<void>(gabi::ea(&l_viewMatrix)));
  for (u32 i = 0; i < 16; i++)
    *mDoExtHeap_at<be<u32>>(gabi::ea(savedProj.get()) + i * 4) =
        *mDoExtHeap_at<be<u32>>(gabi::ea(&l_projectionMatrix) + i * 4);
  u32 data = gabi::ea(supplied);
  if (!data) {
    u32 manager = l_renderManager.mValue;
    u32 count = mDoExtHeap_at<mDoExtHeap::RenderManager>(manager)->mViewCount;
    u32 list = mDoExtHeap_at<mDoExtHeap::RenderManager>(manager)->mpViewList;
    if (count > 1)
      list += 4;
    data = *mDoExtHeap_at<be<u32>>(list);
  }
  u32 flags = mDoExtHeap_at<mDoExtHeap::RenderView>(data)->mFlags, view;
  if ((flags & mDoExtHeap::VIEW_OVERRIDE_ENABLED) && (flags & mDoExtHeap::VIEW_OVERRIDE_INLINE))
    view = data + 0x84;
  else {
    view = mDoExtHeap_at<mDoExtHeap::RenderView>(data)->mpViewMatrix;
    if (!view)
      view = gabi::ea(&l_defaultView);
  }
  for (u32 i = 0; i < 12; i++)
    *mDoExtHeap_at<be<u32>>(gabi::ea(&l_viewMatrix) + i * 4) =
        *mDoExtHeap_at<be<u32>>(view + i * 4);
  flags = mDoExtHeap_at<mDoExtHeap::RenderView>(data)->mFlags;
  u32 projection;
  if (flags & mDoExtHeap::PROJECTION_CUSTOM)
    projection =
        mDoExtHeap_at<mDoExtHeap::RenderView>(data)->mpCustomProjection;
  else {
    projection =
        mDoExtHeap_at<mDoExtHeap::RenderView>(data)->mpProjectionMatrix;
    if (!projection)
      projection = gabi::ea(&l_defaultProjection);
  }
  u32 transformed = mDoExtHeap_projectionMtx(gabi::at<void>(projection));
  mDoExtHeap_copyProjection(gabi::at<void>(transformed),
                            gabi::at<void>(gabi::ea(&l_projectionMatrix)));
  mDoExtHeap_draw(model);
  for (u32 i = 0; i < 12; i++)
    *mDoExtHeap_at<be<u32>>(gabi::ea(&l_viewMatrix) + i * 4) =
        *mDoExtHeap_at<be<u32>>(gabi::ea(savedView.get()) + i * 4);
  mDoExtHeap_copyProjection(savedProj.get(),
                            gabi::at<void>(gabi::ea(&l_projectionMatrix)));
}

/* 025E2E94 */
u32 mDoExt_createGameHeap(s32 index, u32 size, void *parent) {

  if (index >= 2)
    mDoExtHeap_assert(gabi::at<void>(gabi::ea(&l_createGameHeapFile)), 0xAE2,
                      gabi::at<void>(gabi::ea(&l_createGameIndexCondition)));
  u32 offset = u32(index) << 2, slot = gabi::ea(&gameHeap) + offset;
  if (*mDoExtHeap_at<be<u32>>(slot) && size)
    mDoExtHeap_assert(gabi::at<void>(gabi::ea(&l_createGameHeapFile)), 0xAE4,
                      gabi::at<void>(gabi::ea(&l_gameHeapExistsCondition)));
  u32 heap = mDoExtHeap_createExp(size, parent, 1);
  *mDoExtHeap_at<be<u32>>(slot) = heap;
  if (heap) {
    mDoExtHeap_at<mDoExtHeap::Heap>(heap)->mSolidHead = 0;
    u32 obj = *mDoExtHeap_at<be<u32>>(slot),
        v = *mDoExtHeap_at<be<u32>>(gabi::ea(&l_heapNames) + offset);
    mDoExtHeap_at<mDoExtHeap::Heap>(obj)->mName = v;
    obj = *mDoExtHeap_at<be<u32>>(slot);
    v = mDoExtHeap_at<mDoExtHeap::Heap>(obj)->mAllocationMode;
    mDoExtHeap_at<mDoExtHeap::Heap>(obj)->mAllocationMode = v | 1;
    heap = *mDoExtHeap_at<be<u32>>(slot);
  }
  return heap;
}

/* 025E3004 */
u32 mDoExt_createZeldaHeap(u32 size, void *parent) {

  if (l_zeldaHeap.mValue && size)
    mDoExtHeap_assert(gabi::at<void>(gabi::ea(&l_zeldaHeapFile)), 0xB2E,
                      gabi::at<void>(gabi::ea(&l_zeldaHeapExistsCondition)));
  u32 heap = mDoExtHeap_createExp(size, parent, 1);
  l_zeldaHeap.mValue = heap;
  if (heap) {
    mDoExtHeap_at<mDoExtHeap::Heap>(heap)->mName = gabi::ea(&l_zeldaHeapName);
    heap = l_zeldaHeap.mValue;
    u32 flags = mDoExtHeap_at<mDoExtHeap::Heap>(heap)->mAllocationMode;
    mDoExtHeap_at<mDoExtHeap::Heap>(heap)->mAllocationMode = flags | 1;
    heap = l_zeldaHeap.mValue;
  }
  return heap;
}

/* 025E30DC */
u32 mDoExt_createArchiveHeap(u32 size, void *parent) {

  if (l_archiveHeap.mValue && size)
    mDoExtHeap_assert(gabi::at<void>(gabi::ea(&l_archiveHeapFile)), 0xB6F,
                      gabi::at<void>(gabi::ea(&l_archiveHeapExistsCondition)));
  u32 heap = mDoExtHeap_createExp(size, parent, 1);
  l_archiveHeap.mValue = heap;
  if (heap) {
    mDoExtHeap_at<mDoExtHeap::Heap>(heap)->mName = gabi::ea(&l_archiveHeapName);
    heap = l_archiveHeap.mValue;
    u32 flags = mDoExtHeap_at<mDoExtHeap::Heap>(heap)->mAllocationMode;
    mDoExtHeap_at<mDoExtHeap::Heap>(heap)->mAllocationMode = flags | 1;
    heap = l_archiveHeap.mValue;
  }
  return heap;
}

/* 025E31B4 */
u32 mDoExt_createCommandHeap(u32 size, void *parent) {

  if (l_commandHeap.mValue && size)
    mDoExtHeap_assert(gabi::at<void>(gabi::ea(&l_commandHeapFile)), 0xBB3,
                      gabi::at<void>(gabi::ea(&l_commandHeapExistsCondition)));
  u32 heap = mDoExtHeap_createExp(size, parent, 1);
  l_commandHeap.mValue = heap;
  if (heap) {
    mDoExtHeap_at<mDoExtHeap::Heap>(heap)->mSolidHead = 0;
    heap = l_commandHeap.mValue;
    mDoExtHeap_at<mDoExtHeap::Heap>(heap)->mName = gabi::ea(&l_commandHeapName);
    heap = l_commandHeap.mValue;
    u32 flags = mDoExtHeap_at<mDoExtHeap::Heap>(heap)->mAllocationMode;
    mDoExtHeap_at<mDoExtHeap::Heap>(heap)->mAllocationMode = flags | 1;
    heap = l_commandHeap.mValue;
  }
  return heap;
}

/* 025E30AC */
void mDoExt_setZeldaHeap() {
  u32 heap = l_zeldaHeap.mValue;
  u32 prev = mDoExtHeap_setCurrent(heap);
  l_previousZeldaHeap.mValue = prev;
}

/* 025E3184 */
void mDoExt_setArchiveHeap() {
  u32 heap = l_archiveHeap.mValue;
  u32 prev = mDoExtHeap_setCurrent(heap);
  l_previousArchiveHeap.mValue = prev;
}

/* 025E3268 */
u32 mDoExt_getCommandHeap() { return l_commandHeap.mValue; }

/* 025E3274 */
void mDoExt_setCommandHeap() {
  u32 heap = mDoExtHeap_getCommandCall();
  u32 old = mDoExtHeap_setCurrent(heap);
  l_previousCommandHeap.mValue = old;
}

/* 025E3528 */
u32 mDoExt_createSolidHeapFromGame(u32 size, u32 alignment) {
  u32 parent = mDoExtHeap_getGame0Call();
  return mDoExtHeap_createSolidCall(size, parent, alignment);
}

/* 025E3570 */
u32 mDoExt_setCurrentHeap(void *heap) {
  if (!gabi::ea(heap))
    mDoExtHeap_assert(gabi::at<void>(gabi::ea(&l_setCurrentHeapFile)), 0xDCC,
                      gabi::at<void>(gabi::ea(&l_heapNotNullCondition)));
  return mDoExtHeap_becomeCurrent(heap);
}

/* 025E35BC */
u32 mDoExt_createSolidHeapToCurrent(u32 size, void *parent, u32 align) {

  u32 heap = mDoExtHeap_createSolidCall(size, parent, align);
  if (heap) {
    if (mDoExt_SaveCurrentHeap.mValue)
      mDoExtHeap_assert(gabi::at<void>(gabi::ea(&l_solidToCurrentFile)), 0xD34,
                        gabi::at<void>(gabi::ea(&l_savedHeapEmptyCondition)));
    u32 prev = mDoExtHeap_current();
    mDoExt_SaveCurrentHeap.mValue = prev;
    mDoExtHeap_setCurrentCall(gabi::at<void>(heap));
  }
  return heap;
}

/* 025E3630 */
u32 mDoExt_createSolidHeapFromGameToCurrent(u32 size, u32 alignment) {
  u32 heap = mDoExtHeap_getGame0Call();
  return mDoExtHeap_createSolidToCurrentCall(size, heap, alignment);
}

/* 025E37D8 */
u32 mDoExt_restoreCurrentHeap() {
  u32 heap = mDoExt_SaveCurrentHeap.mValue;
  if (!heap) {
    mDoExtHeap_assert(gabi::at<void>(gabi::ea(&l_restoreCurrentFile)), 0xE07,
                      gabi::at<void>(gabi::ea(&l_savedHeapNotNullCondition)));
    heap = mDoExt_SaveCurrentHeap.mValue;
  }
  u32 result = mDoExtHeap_becomeCurrent(gabi::at<void>(heap));
  mDoExt_SaveCurrentHeap.mValue = 0;
  return result;
}

/* 025E3834 */
u32 mDoExt_adjustSolidHeapToCurrent(void *heap) {
  u32 result = mDoExtHeap_adjustSolidCall(heap);
  mDoExtHeap_restoreCurrentCall();
  return result;
}

/* 025E3898 */
void mDoExt_initGameHeapRecords() {
  mDoExtHeap_constructRecords(gabi::at<void>(gabi::ea(&l_gameHeapRecords)), 2,
                              8, gabi::at<void>(gabi::ea(&l_recordConstructor)),
                              0, 0);
}

/* 025E38BC */
void mDoExt_initGameHeapAlignmentRecords() {
  mDoExtHeap_constructRecords(
      gabi::at<void>(gabi::ea(&l_gameHeapAlignmentRecords)), 2, 8,
      gabi::at<void>(gabi::ea(&l_recordConstructor)), 0, 0);
}

/* 025E32A0 */
void mDoExt_alignSolidHeap(void *heap, u32 alignment) {

  u32 obj = gabi::ea(heap);
  if (!obj || alignment <= 4)
    return;
  gabi::Local<mDoExtHeap::HeapBounds> saved;
  u32 end = mDoExtHeap_at<mDoExtHeap::Heap>(obj)->mSolidTail,
      begin = mDoExtHeap_at<mDoExtHeap::Heap>(obj)->mSolidHead;
  saved->mSolidHead = begin;
  saved->mSolidTail = end;
  u32 allocated =
      gabi::call_ptr<u32>(mDoExtHeap_virtualTarget(obj, mDoExtHeap::ALLOCATE), heap, 1, 4);
  mDoExtHeap_restoreHeapBounds(heap, saved.get());
  if (!allocated)
    return;
  u32 start = gabi::call_ptr<u32>(mDoExtHeap_virtualTarget(obj, mDoExtHeap::GET_START), heap),
      header = mDoExtHeap_headerSize(0);
  if (allocated != start + header)
    mDoExtHeap_assert(gabi::at<void>(gabi::ea(&l_alignHeapFile)), 0xC5D,
                      gabi::at<void>(gabi::ea(&l_firstAllocationCondition)));
  u32 delta = ((allocated + alignment - 1) & ~(alignment - 1)) - allocated;
  if (delta)
    gabi::call_ptr(mDoExtHeap_virtualTarget(obj, mDoExtHeap::ALLOCATE), heap, delta, 4);
}

/* 025E33A8 */
u32 mDoExt_createSolidHeap(u32 requested, void *parent, u32 alignment) {

  if (!alignment)
    alignment = 32;
  u32 pobj = gabi::ea(parent);
  if (!pobj)
    pobj = mDoExtHeap_current();
  bool unit = mDoExtHeap_type(gabi::at<void>(pobj)) == mDoExtHeap::HEAP_UNIT;
  u32 heap;
  if (!requested || requested == 0xFFFFFFFF) {
    heap = mDoExtHeap_createSolid(0xFFFFFFFF, gabi::at<void>(pobj), 0);
  } else {
    u32 size = (requested + 3) & ~3u;
    size += mDoExtHeap_headerSize(4);
    if (alignment > 4 && !unit)
      size += alignment - 4;
    heap = mDoExtHeap_createSolid(size, gabi::at<void>(pobj), 0);
  }
  if (heap) {
    gabi::call_ptr<u32>(mDoExtHeap_virtualTarget(heap, mDoExtHeap::GET_START),
                        gabi::at<void>(heap));
    mDoExtHeap_headerSize(0);
    if (!unit)
      mDoExtHeap_alignSolidCall(gabi::at<void>(heap), alignment);
    if (l_adjustCreatedHeap.mValue)
      gabi::call_ptr(mDoExtHeap_virtualTarget(heap, mDoExtHeap::ADJUST_QUERY),
                     gabi::at<void>(heap));
  }
  l_lastSolidHeap.mValue = heap;
  l_lastSolidAlignment.mValue = alignment;
  return heap;
}

/* 025E3678 */
u32 mDoExt_adjustSolidHeap(void *heap) {

  u32 obj = gabi::ea(heap);
  if (!obj)
    return 0;
  bool same = l_lastSolidHeap.mValue == obj;
  u32 target = mDoExtHeap_virtualTarget(obj, mDoExtHeap::CAN_ADJUST), alignment = 0;
  if (same)
    alignment = l_lastSolidAlignment.mValue;
  u32 check = gabi::call_ptr<u32>(target, heap);
  if (check) {
    u32 parent = mDoExtHeap_at<mDoExtHeap::Heap>(obj)->mpParent;
    check = gabi::call_ptr<u32>(mDoExtHeap_virtualTarget(parent, mDoExtHeap::PARENT_CAN_ADJUST),
                                gabi::at<void>(parent));
  }
  if (!check)
    return gabi::call_ptr<u32>(mDoExtHeap_virtualTarget(obj, mDoExtHeap::ADJUST_QUERY), heap);
  gabi::call_ptr(mDoExtHeap_virtualTarget(obj, mDoExtHeap::ADJUST_QUERY), heap);
  u32 reduced = mDoExtHeap_adjustSize(heap);
  gabi::call_ptr(mDoExtHeap_virtualTarget(obj, mDoExtHeap::ADJUST_QUERY), heap);
  u32 start = gabi::call_ptr<u32>(mDoExtHeap_virtualTarget(obj, mDoExtHeap::GET_START), heap);
  u32 header = mDoExtHeap_headerSize(0), base = start + header;
  if (alignment)
    return reduced + base - ((base + alignment - 1) & ~(alignment - 1));
  return reduced;
}

/* 025E3868 */
void mDoExt_destroySolidHeap(void *heap) {

  if (l_lastSolidHeap.mValue == gabi::ea(heap)) {
    l_lastSolidHeap.mValue = 0;
    l_lastSolidAlignment.mValue = 0;
  }
  gabi::call_ptr(mDoExtHeap_virtualTarget(gabi::ea(heap), mDoExtHeap::DESTROY), heap);
}
/* WWHD glue (tools/verify/autoglue.py)
ext 027EC230 u32 mDoExtHeap_current()
ext 027EC23C u32 mDoExtHeap_type(u32)
ext 025F2710 void mDoExtHeap_report(void*)
ext 0273AA24 void mDoExtHeap_assert(void*, u32, void*)
ext 027F4FE4 void mDoExtHeap_modelUpdate(void*)
ext 025E2BF4 void mDoExtHeap_drawModelCall(void*, void*)
ext 027F4F1C void mDoExtHeap_modelEntry(void*)
ext 025E2F64 u32 mDoExtHeap_getGameCall(s32)
ext 025E2FBC u32 mDoExtHeap_getGame0Call()
ext 027EC1FC u32 mDoExtHeap_setCurrent(u32)
ext 025E2FC4 void mDoExtHeap_setGameCall(s32)
ext 025E2B54 void mDoExtHeap_copyViewCall(void*, void*)
ext 0274D83C u32 mDoExtHeap_projectionMtx(void*)
ext 028E8970 void mDoExtHeap_copyProjection(void*, void*)
ext 027F55FC void mDoExtHeap_draw(void*)
ext 027EBCB0 u32 mDoExtHeap_createExp(u32, void*, u32)
ext 025E3268 u32 mDoExtHeap_getCommandCall()
ext 025E33A8 u32 mDoExtHeap_createSolidCall(u32, u32, u32)
ext 027EC220 u32 mDoExtHeap_becomeCurrent(void*)
ext 025E3570 u32 mDoExtHeap_setCurrentCall(void*)
ext 025E35BC u32 mDoExtHeap_createSolidToCurrentCall(u32, u32, u32)
ext 025E3678 u32 mDoExtHeap_adjustSolidCall(void*)
ext 025E37D8 void mDoExtHeap_restoreCurrentCall()
ext 028F0164 void mDoExtHeap_constructRecords(void*, u32, u32, void*, u32, u32)
ext 02755478 void mDoExtHeap_restoreHeapBounds(void*, void*)
ext 02754C90 u32 mDoExtHeap_headerSize(u32)
ext 027EC464 u32 mDoExtHeap_createSolid(u32, void*, u32)
ext 025E32A0 void mDoExtHeap_alignSolidCall(void*, u32)
ext 027EC460 u32 mDoExtHeap_adjustSize(void*)
data l_previousGameHeap 1048CF30
data l_zeldaHeap 1048CF34
data l_previousZeldaHeap 1048CF38
data l_archiveHeap 1048CF3C
data l_previousArchiveHeap 1048CF40
data l_commandHeap 1048CF44
data l_previousCommandHeap 1048CF48
data mDoExt_SaveCurrentHeap 1048CED8
data l_renderManager 101F95D0
data l_lastSolidHeap 101F4754
data l_lastSolidAlignment 101F4758
data l_adjustCreatedHeap 101F47B8
data l_notSolidHeapMessage 100586E4
data l_createGameHeapFile 1005879C
data l_createGameIndexCondition 100587AC
data l_gameHeapExistsCondition 100587C4
data l_getGameHeapFile 10058800
data l_gameHeapIndexCondition 10058810
data l_zeldaHeapExistsCondition 10058828
data l_zeldaHeapName 1005884C
data l_zeldaHeapFile 10058858
data l_archiveHeapExistsCondition 10058868
data l_archiveHeapName 1005888C
data l_archiveHeapFile 10058898
data l_commandHeapExistsCondition 100588A8
data l_commandHeapName 100588CC
data l_commandHeapFile 100588D8
data l_firstAllocationCondition 100588F0
data l_alignHeapFile 10058910
data l_heapNotNullCondition 10058920
data l_setCurrentHeapFile 1005892C
data l_savedHeapEmptyCondition 1005893C
data l_solidToCurrentFile 1005895C
data l_savedHeapNotNullCondition 1005896C
data l_restoreCurrentFile 1005898C
data gameHeap 1048CF28
data l_heapNames 101F475C
data l_viewMatrix 104B45F8
data l_projectionMatrix 104B470C
data l_defaultView 104A2098
data l_defaultProjection 104A20FC
data l_gameHeapRecords 1048CEE8
data l_gameHeapAlignmentRecords 1048CEF8
data l_recordConstructor 025EE034
*/
