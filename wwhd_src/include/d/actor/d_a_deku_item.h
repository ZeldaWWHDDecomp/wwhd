#pragma once
#include "gc_api/gc_api.h"
namespace DekuItem {
enum GuestAddress : u32 {
  ArcName = 0x1000d6a0,
  SafeStringVtable = 0x1000d574,
  AssertFile = 0x1000d614,
  ModelAssertion = 0x1000d628,
  AnimationAssertion = 0x1000d608,
  MatrixStack = 0x1048d0cc,
  CylinderSource = 0x101b4228,
  TransformVtable = 0x1016e54c,
  MaterialVtable = 0x1016d820,
  BckVtable = 0x1000d59c,
  ActorVtable = 0x1000d5f4,
  AcchVtable = 0x1000d5c4,
  PolygonVtable = 0x1000d5d4,
  CheckVtable = 0x1000d5e4,
  StatusVtable = 0x1004ae88,
  GlobalStatusVtable = 0x1004aec0,
  ShapeBaseVtable = 0x100015a8,
  BoxVtable = 0x1000d58c,
  CylinderShapeVtable = 0x1004b160,
  CylinderHitVtable = 0x1004b108,
  CylinderGeometryVtable = 0x1004b150,
  HeapCallback = 0x0211e890,
  EventCheckName = 0x1000d650,
  EventOrderName = 0x1000d680,
  AngleDestructor = 0x101b426c,
  NullDestructor = 0x101b4278,
  OneDestructor = 0x101b4284,
};
enum ResourceIndex {
  LeafModel = 8,
  LeafOpenAnimation = 4,
  LeafIdleAnimation = 5
};
enum EffectId { LeafShine = 0x820f, LeafSound = 0x7052, LeafItem = 0x34 };

// HD: inline animation storage includes transform, material-table and blend ownership.
struct BckAnm {
  J3DFrameCtrl mFrameCtrl;
  be<u32> transformVtable;
  u8 transform[0x48 - 0x14];
  be<u32> materialVtable;
  u8 material[0x58 - 0x4c];
  be<u32> animation;
  u8 blend[0x7c - 0x5c];
  be<u32> oldFrame, model, callback, weight;
};
WWHD_SIZE(BckAnm, 0x8c);
struct Acch {
  u8 base[0x10];
  be<u32> vtable, checkVtable;
  be<u8> object;
  u8 collision[7];
  be<u32> polygonVtable;
  u8 work[0x1c4 - 0x24];
};
struct AcchCir {
  u8 wall[0x14];
  u8 circle[0x40 - 0x14];
};
struct Status {
  u8 collision[0x18];
  be<u32> vtable, globalVtable;
  u8 work[0x3c - 0x20];
};
struct Cylinder {
  u8 object[0x3c];
  be<u32> hitVtable;
  u8 hit[4];
  gptr<Status> status;
  u8 contacts[0x110 - 0x48];
  be<u32> boxVtable, shapeVtable;
  cXyz center;
  be<f32> radius, height;
  be<u32> geometryVtable;
};
WWHD_SIZE(Cylinder, 0x130);
// HD: model instance prefix and base transform are larger than the GC layout.
struct Model {
  u8 instance[0xac];
  be<u32> data;
  u8 material[0xbc - 0xb0];
  cXyz scale;
  be<f32> matrix[12];
};
struct Emitter {
  u8 simulation[0x5c];
  be<u32> userWork;
  u8 particle[0x22c - 0x60];
  cXyz translation;
  u8 render[0x254 - 0x238];
  be<u32> flags;
  u8 resource[0x262 - 0x258];
  be<u8> kind;
};
struct Save {
  u8 header[0x20];
  u8 memory[0x644 - 0x20];
  u8 event[1];
};
// HD: unit-local view of the play fields used here; offsets include HD subsystems.
struct Play {
  u8 base[0x12a0];
  u8 background[0x26a4 - 0x12a0];
  u8 collision[0x52a0 - 0x26a4];
  be<u32> itemPartner;
  u8 eventData[0x52b8 - 0x52a4];
  be<u16> eventFlag;
  u8 eventPad[0x52c4 - 0x52ba];
  u8 eventManager[0x5ab0 - 0x52c4];
  be<u32> particles;
};
struct ModeProc {
  be<s16> adjustment, slot;
  union {
    be<u32> function;
    struct {
      be<s16> reserved, vtableOffset;
    } virtualCall;
  };
};
struct VirtualEntry {
  be<u32> adjustment, function;
};
struct MathConstants {
  be<f32> minimum, maximum;
  u8 nullByte, oneByte;
  u8 pad[2];
  be<u32> zeroA, zeroB, zeroC, zeroD;
};
struct ResourceLocal {
  be<u32> linkage[2];
  struct {
    be<u32> text, vtable;
  } name;
};
template <class T> inline T *view(u32 address) { return gabi::at<T>(address); }
inline Save *saveView() { return view<Save>(*view<be<u32>>(0x101f84dc)); }
inline Model *modelView(u32 address) { return view<Model>(address); }
inline Play *playView(u32 address) { return view<Play>(address); }
inline be<f32> *matrixStack() { return view<be<f32>>(0x1048d0cc); }
inline ModeProc *modeTable() { return view<ModeProc>(0x1000d660); }
inline MathConstants *constants() { return view<MathConstants>(0x10463ac4); }
inline u32 resourceController() { return *view<be<u32>>(0x101f4f28); }
inline u32 tableFunction(u32 object, s16 offset, s16 slot) {
  u32 table = *view<be<u32>>(object + (s32)offset);
  return view<VirtualEntry>(table)[(s32)slot].function;
}
inline void invokeMode(u32 target, void *object) {
  gabi::call_ptr(target, object);
}
namespace sdk {
/* 026066C4 [v d_a_deku_item] */
template <class R = void, class... A> inline R getObjectResource(A... args) {
  return gabi::call<R>(0x026066C4, args...);
}
/* 0273AA24 [v d_a_deku_item] */
template <class R = void, class... A> inline R assertion(A... args) {
  return gabi::call<R>(0x0273AA24, args...);
}
/* 025E38E0 [v d_a_deku_item] */
template <class R = void, class... A> inline R createModel(A... args) {
  return gabi::call<R>(0x025E38E0, args...);
}
/* 025E8508 [v d_a_deku_item] */
template <class R = void, class... A> inline R initBck(A... args) {
  return gabi::call<R>(0x025E8508, args...);
}
/* 028E93CC [v d_a_deku_item] */
template <class R = void, class... A> inline R translateMatrix(A... args) {
  return gabi::call<R>(0x028E93CC, args...);
}
/* 025F1C28 [v d_a_deku_item] */
template <class R = void, class... A> inline R rotateYMatrix(A... args) {
  return gabi::call<R>(0x025F1C28, args...);
}
/* 025D674C [v d_a_deku_item] */
template <class R = void, class... A> inline R setCullBox(A... args) {
  return gabi::call<R>(0x025D674C, args...);
}
/* 02515F14 [v d_a_deku_item] */
template <class R = void, class... A> inline R initStatus(A... args) {
  return gabi::call<R>(0x02515F14, args...);
}
/* 02516518 [v d_a_deku_item] */
template <class R = void, class... A> inline R setCylinder(A... args) {
  return gabi::call<R>(0x02516518, args...);
}
/* 024EFF44 [v d_a_deku_item] */
template <class R = void, class... A> inline R setWall(A... args) {
  return gabi::call<R>(0x024EFF44, args...);
}
/* 024F06B4 [v d_a_deku_item] */
template <class R = void, class... A> inline R setAcch(A... args) {
  return gabi::call<R>(0x024F06B4, args...);
}
/* 027F2BC0 [v d_a_deku_item] */
template <class R = void, class... A> inline R initFrameCtrl(A... args) {
  return gabi::call<R>(0x027F2BC0, args...);
}
/* 027DA984 [v d_a_deku_item] */
template <class R = void, class... A> inline R constructTransform(A... args) {
  return gabi::call<R>(0x027DA984, args...);
}
/* 025D4ED0 [v d_a_deku_item] */
template <class R = void, class... A> inline R constructActor(A... args) {
  return gabi::call<R>(0x025D4ED0, args...);
}
/* 024F0474 [v d_a_deku_item] */
template <class R = void, class... A> inline R constructAcch(A... args) {
  return gabi::call<R>(0x024F0474, args...);
}
/* 024EFE94 [v d_a_deku_item] */
template <class R = void, class... A> inline R constructAcchCir(A... args) {
  return gabi::call<R>(0x024EFE94, args...);
}
/* 0200BD2C [v d_a_deku_item] */
template <class R = void, class... A> inline R constructStatus(A... args) {
  return gabi::call<R>(0x0200BD2C, args...);
}
/* 02515DA0 [v d_a_deku_item] */
template <class R = void, class... A>
inline R constructGlobalStatus(A... args) {
  return gabi::call<R>(0x02515DA0, args...);
}
/* 02515FB8 [v d_a_deku_item] */
template <class R = void, class... A> inline R constructCylinder(A... args) {
  return gabi::call<R>(0x02515FB8, args...);
}
/* 02018590 [v d_a_deku_item] */
template <class R = void, class... A> inline R constructGeometry(A... args) {
  return gabi::call<R>(0x02018590, args...);
}
/* 025BA494 [v d_a_deku_item] */
template <class R = void, class... A> inline R isItem(A... args) {
  return gabi::call<R>(0x025BA494, args...);
}
/* 025B8B94 [v d_a_deku_item] */
template <class R = void, class... A> inline R isEventBit(A... args) {
  return gabi::call<R>(0x025B8B94, args...);
}
/* 02520460 [v d_a_deku_item] */
template <class R = void, class... A> inline R loadResource(A... args) {
  return gabi::call<R>(0x02520460, args...);
}
/* 025D63E8 [v d_a_deku_item] */
template <class R = void, class... A> inline R entrySolidHeap(A... args) {
  return gabi::call<R>(0x025D63E8, args...);
}
/* 025204C8 [v d_a_deku_item] */
template <class R = void, class... A> inline R deleteResource(A... args) {
  return gabi::call<R>(0x025204C8, args...);
}
/* 02555D0C [v d_a_deku_item] */
template <class R = void, class... A> inline R environment(A... args) {
  return gabi::call<R>(0x02555D0C, args...);
}
/* 025626A4 [v d_a_deku_item] */
template <class R = void, class... A> inline R settingTevStruct(A... args) {
  return gabi::call<R>(0x025626A4, args...);
}
/* 02562F5C [v d_a_deku_item] */
template <class R = void, class... A> inline R setLightTevColorType(A... args) {
  return gabi::call<R>(0x02562F5C, args...);
}
/* 025E86B8 [v d_a_deku_item] */
template <class R = void, class... A> inline R entryBck(A... args) {
  return gabi::call<R>(0x025E86B8, args...);
}
/* 025E2DE0 [v d_a_deku_item] */
template <class R = void, class... A> inline R modelUpdateDL(A... args) {
  return gabi::call<R>(0x025E2DE0, args...);
}
/* 025200D4 [v d_a_deku_item] */
template <class R = void, class... A> inline R play(A... args) {
  return gabi::call<R>(0x025200D4, args...);
}
/* 025445B8 [v d_a_deku_item] */
template <class R = void, class... A> inline R eventStartCheck(A... args) {
  return gabi::call<R>(0x025445B8, args...);
}
/* 0254457C [v d_a_deku_item] */
template <class R = void, class... A> inline R eventEndCheck(A... args) {
  return gabi::call<R>(0x0254457C, args...);
}
/* 025D57E0 [v d_a_deku_item] */
template <class R = void, class... A> inline R deleteActor(A... args) {
  return gabi::call<R>(0x025D57E0, args...);
}
/* 025BA384 [v d_a_deku_item] */
template <class R = void, class... A> inline R onItem(A... args) {
  return gabi::call<R>(0x025BA384, args...);
}
/* 025D77DC [v d_a_deku_item] */
template <class R = void, class... A> inline R orderOtherEvent(A... args) {
  return gabi::call<R>(0x025D77DC, args...);
}
/* 025D6870 [v d_a_deku_item] */
template <class R = void, class... A> inline R moveActor(A... args) {
  return gabi::call<R>(0x025D6870, args...);
}
/* 024F08A8 [v d_a_deku_item] */
template <class R = void, class... A> inline R correctPosition(A... args) {
  return gabi::call<R>(0x024F08A8, args...);
}
/* 02516464 [v d_a_deku_item] */
template <class R = void, class... A> inline R checkCoHit(A... args) {
  return gabi::call<R>(0x02516464, args...);
}
/* 025A847C [v d_a_deku_item] */
template <class R = void, class... A> inline R setParticle(A... args) {
  return gabi::call<R>(0x025A847C, args...);
}
/* 02520540 [v d_a_deku_item] */
template <class R = void, class... A> inline R getReverb(A... args) {
  return gabi::call<R>(0x02520540, args...);
}
/* 025E1A40 [v d_a_deku_item] */
template <class R = void, class... A> inline R startSound(A... args) {
  return gabi::call<R>(0x025E1A40, args...);
}
/* 020182E0 [v d_a_deku_item] */
template <class R = void, class... A> inline R setCylinderCenter(A... args) {
  return gabi::call<R>(0x020182E0, args...);
}
/* 0200E240 [v d_a_deku_item] */
template <class R = void, class... A> inline R registerCollision(A... args) {
  return gabi::call<R>(0x0200E240, args...);
}
/* 025DA884 [v d_a_deku_item] */
template <class R = void, class... A> inline R offDraw(A... args) {
  return gabi::call<R>(0x025DA884, args...);
}
/* 025D7E88 [v d_a_deku_item] */
template <class R = void, class... A> inline R createDemoItem(A... args) {
  return gabi::call<R>(0x025D7E88, args...);
}
/* 028F026C [v d_a_deku_item] */
template <class R = void, class... A> inline R registerDestructor(A... args) {
  return gabi::call<R>(0x028F026C, args...);
}
/* 028ED6F8 [v d_a_deku_item] */
template <class R = void, class... A> inline R constructNullByte(A... args) {
  return gabi::call<R>(0x028ED6F8, args...);
}
/* 028EAB2C [v d_a_deku_item] */
template <class R = void, class... A> inline R constructOneByte(A... args) {
  return gabi::call<R>(0x028EAB2C, args...);
}
/* 0273AF40 [v d_a_deku_item] */
template <class R = void, class... A> inline R deleteObject(A... args) {
  return gabi::call<R>(0x0273AF40, args...);
}
/* 02515A70 [v d_a_deku_item] */
template <class R = void, class... A> inline R destroyCylinder(A... args) {
  return gabi::call<R>(0x02515A70, args...);
}
/* 02515860 [v d_a_deku_item] */
template <class R = void, class... A> inline R destroyStatus(A... args) {
  return gabi::call<R>(0x02515860, args...);
}
/* 02018034 [v d_a_deku_item] */
template <class R = void, class... A> inline R destroyCircle(A... args) {
  return gabi::call<R>(0x02018034, args...);
}
/* 024EFD9C [v d_a_deku_item] */
template <class R = void, class... A> inline R destroyAcch(A... args) {
  return gabi::call<R>(0x024EFD9C, args...);
}
/* 027F3628 [v d_a_deku_item] */
template <class R = void, class... A> inline R destroyTransform(A... args) {
  return gabi::call<R>(0x027F3628, args...);
}
/* 025D50BC [v d_a_deku_item] */
template <class R = void, class... A> inline R destroyActor(A... args) {
  return gabi::call<R>(0x025D50BC, args...);
}
} // namespace sdk
} // namespace DekuItem
struct daDekuItem_c : fopAc_ac_c {
  enum Mode_e {
    Mode_WAIT_e,
    Mode_GETDEMO_INIT_e,
    Mode_GETDEMO_WAIT_e,
    Mode_GETDEMO_e
  };
  request_of_phase_process_class mPhs;
  gptr<DekuItem::Model> mpModel;
  DekuItem::BckAnm mBck1, mBck2;
  DekuItem::Acch mAcch;
  DekuItem::AcchCir mAcchCir;
  DekuItem::Status mStts;
  DekuItem::Cylinder mCyl;
  be<s32> mMode, unk630, mItemBitNo;
  be<u32> mItemPID;
  gptr<DekuItem::Emitter> mpEmitter;
  BOOL CreateHeap();
  void set_mtx();
  void CreateInit();
  s32 _create();
  BOOL _delete();
  BOOL _draw();
  void checkOrder();
  void mode_proc_call();
  void eventOrder();
  BOOL _execute();
  void mode_wait();
  void mode_getdemo_init();
  void mode_getdemo_wait();
  void mode_getdemo();
  void destroy(u32);
};
WWHD_OFFSET(daDekuItem_c, mBck1, 0x3b8);
WWHD_OFFSET(daDekuItem_c, mCyl, 0x710);
WWHD_OFFSET(daDekuItem_c, mMode, 0x840);
WWHD_OFFSET(daDekuItem_c, mpEmitter, 0x850);
WWHD_SIZE(daDekuItem_c, 0x854);
