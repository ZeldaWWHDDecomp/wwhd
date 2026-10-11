/* Metronome, WWHD screen/state controller layout. GameCube dMetronome_c is a
 * draw-list object with embedded fopMsgM panes; HD stores pane pointers instead.
 * GameCube timing/score/particle names are retained where the operations match.
 */
#pragma once
#include "SSystem/SComponent/c_xyz.h"
#include "gabi.h"
namespace Metronome {
using namespace gabi;
// HD: the metronome is a screen/state controller with separate three-, four- and six-beat panes.
struct Pane {
  u8 base[0x50];
  be<u8> active;
  u8 padding[0xb];
  be<u32> note;
  u8 tail[4];
  be<u8> visible;
};
struct Vec2 {
  be<f32> x, y;
};
struct dMetronome_c {
  void closeFlag();
  void open();
  void animationPlay(u32 i);
  void animationFrame(u32 i, u32 frame);
  void beatPlay();
  void melodyHide();
  void openInit();
  void shrinkPlay();
  void shrinkInit();
  u32 isShrinkEnd();
  void timerInit();
  void demoInit();
  void endInit();
  void closeInit();
  s32 getMetronomeTimer();
  void metronomeInit();
  void waitInit();
  void metronomeMove();
  void openMove();
  void closeMove();
  void _delete();
  void _move();
  void _draw(u32 port);
  void waitMove();
  void shrinkMove();
  void timerMove();
  void melodyFlash();
  void melodyGuideShow(u32 note, u32 index);
  void melodyShow();
  void melodyDemo();
  void demoMove();
  u32 loadScreen(u32 file, u32 archive, u32 arg6, u32 arg7, u32 arg8);
  u32 createPane(u32 strings);
  u32 screenSet(u32 archive, u32 a, u32 b);

  be<u32> base, vtable;
  u8 prefix[0x18 - 8];
  be<u32> state;
  u8 stateData[4];
  be<u32> action;
  u8 reserved[8];
  be<u32> descriptor, descriptorVtable, archive;
  u8 screenData[0x44 - 0x38];
  be<u32> screen, paneCount, panes;
  be<u32> pane_timing[21], pane_note3[3], pane_note4[4], pane_note6[6];
  be<u32> pane_target3[3], pane_target4[4], pane_target6[6], mNote[6],
      mpEmitter;
  Vec2 mGuideOffset, mProjectOffset;
  be<f32> mGuideScale, mGuideParticleSize, mTactScale, mTactParticleSize,
      mCurRate;
  be<u32> mCurTimer, mInputWait;
  union {
    be<u32> mNoteIndex;
    be<u8> mNoteIndexBytes[4];
  };
  be<u32> mWaitTimer, mDemoTimer, mDemoIndex;
  be<u8> mBeat, mPrevBeat, mAnimationBeat, mbOpen, mLastInput, mTimerSide,
      mInputActive, mTimerActive;
};
WWHD_SIZE(dMetronome_c, 0x16c);
WWHD_OFFSET(dMetronome_c, mpEmitter, 0x124);
WWHD_OFFSET(dMetronome_c, mBeat, 0x164);
struct String {
  be<u32> text, vtable;
};
struct Emitter {
  u8 base[0x5c];
  be<u32> mMaxFrame;
  u8 data[0x1b4 - 0x60];
  be<u32> particleCount;
  u8 lists[8];
  be<u32> childParticleCount;
  u8 parameters[0x220 - 0x1c4];
  cXyz mGlobalScale, mGlobalTranslation, mParticleScale;
  u8 stateData[0x254 - 0x244];
  be<u32> mStatus;
  u8 statusTail[0xa];
  be<u8> mDrawGroup;
};
WWHD_OFFSET(Emitter, mStatus, 0x254);
struct Screen {
  be<u32> base, vtable;
  u8 contents[0xd4 - 8];
  be<u32> animation;
  u8 resources[8];
  be<u32> resourceVtable;
};
struct Play {
  u8 base[0x5ab0];
  be<u32> particleControl;
  u8 data[0x5b2c - 0x5ab4];
  be<u32> player;
  u8 reserved[4];
  be<u32> link;
};
struct Player {
  u8 base[0xb4];
  be<u32> vtable;
  u8 state[0x3c0 - 0xb8];
  be<u32> flags;
  u8 tact[0x691c - 0x3c4];
  be<s16> tactNote;
};
template <class T> inline T *view(u32 address) { return gabi::at<T>(address); }
inline dMetronome_c *controller(u32 address) {
  return view<dMetronome_c>(address);
}
inline be<u32> *animationIndices() { return view<be<u32>>(0x100e338c); }
inline be<u8> &tactOpen() { return *view<be<u8>>(0x1047b097); }
inline be<u32> &registered() { return *view<be<u32>>(0x101fd8e4); }
inline be<u32> &stateId() { return *view<be<u32>>(0x101fdd50); }
// HD: constants retain their guest bit patterns where constructor copies are word stores.
inline be<u32> &zeroBits() { return *view<be<u32>>(0x100e3460); }
inline be<u32> &oneBits() { return *view<be<u32>>(0x100e3440); }
inline be<f32> &one() { return *view<be<f32>>(0x100e3440); }
inline be<f32> &timerScale() { return *view<be<f32>>(0x100e3450); }
// HD: state descriptors contain three Metrowerks member-function pointers.
struct MemberFunction {
  be<s16> adjustment, slot;
  union {
    be<u32> function;
    struct {
      be<u16> high;
      be<s16> vtableOffset;
    } virtualEntry;
  };
};
struct StateDescriptor {
  be<u32> id;
  String name;
  MemberFunction initialize, execute, draw, reserved;
  be<u32> base;
};
WWHD_SIZE(StateDescriptor, 0x30);
struct StaticData {
  be<u32> position[2];
  be<u8> constructedA, constructedB;
  u8 padding[2];
  be<u32> cleared[4];
  String animationResources[4], animationNames[8], layoutNames[4],
      paneNames[34], paneTypes[34];
  StateDescriptor states[7];
  String layoutFilename;
};
WWHD_OFFSET(StaticData, states, 0x434 - 0x178);
inline StaticData &staticData() { return *view<StaticData>(0x1048e178); }

enum ConstantAddress : u32 {
  GuideScale = 0x100e3464,
  GuideParticleSize = 0x100e3468,
  StaticPositionX = 0x100e346c,
  StaticPositionY = 0x100e3470
};
inline be<u32> &constantBits(u32 address) { return *view<be<u32>>(address); }
enum GuestAddress : u32 {
  CloseState = 0x1048E434,
  OpenState = 0x1048E464,
  WaitState = 0x1048E494,
  ShrinkState = 0x1048E4C4,
  TimerState = 0x1048E4F4,
  DemoState = 0x1048E524,
  EndState = 0x1048E554,
  ControllerVtable = 0x100E37B8,
  DescriptorVtable = 0x100E3938,
  ControllerDescriptor = 0x100E3928,
  StringVtable = 0x100E3374,
  CreateTimingPane = 0x026200A8,
  CreateNotePane = 0x02620158,
  CreateBeatPane = 0x02620208,
  TimingPaneType = 0x1048E5B0,
  NotePaneType = 0x1048E5E4,
  BeatPaneType = 0x1048E6F0,
  AnimationNames = 0x1048E1B4,
  AnimationResources = 0x1048E194,
  PaneNames = 0x1048E214,
  LayoutName = 0x1048E1F4,
  LayoutFilename = 0x1048E584,
  StaticA = 0x1048E180,
  StaticB = 0x1048E181,
  StaticDestructor0 = 0x101F5194,
  StaticDestructor1 = 0x101F51A0,
  StaticDestructor2 = 0x101F51AC,
  StateBase = 0x101FF32C,
  StateNameVtable = 0x100E33F4,
  MainAnimationResource = 0x100E370C,
  Beat3AnimationResource = 0x100E348C,
  Beat4AnimationResource = 0x100E349C,
  Beat6AnimationResource = 0x100E34AC,
  OpenAnimationName = 0x100E347C,
  BeatAnimationName = 0x100E3474,
  ShrinkAnimationName = 0x100E3718,
  TimingPaneName01 = 0x100E34BC,
  TimingPaneName02 = 0x100E34CC,
  TimingPaneName03 = 0x100E34DC,
  TimingPaneName04 = 0x100E34EC,
  EndAnimationName = 0x100E3484,
  TimingPaneName05 = 0x100E34FC,
  TimingPaneName06 = 0x100E350C,
  TimingPaneName07 = 0x100E351C,
  TimingPaneName08 = 0x100E352C,
  TimingPaneName09 = 0x100E353C,
  TimingPaneName10 = 0x100E354C,
  TimingPaneName11 = 0x100E355C,
  TimingPaneName12 = 0x100E356C,
  TimingPaneName13 = 0x100E357C,
  TimingPaneName14 = 0x100E358C,
  TimingPaneName15 = 0x100E359C,
  TimingPaneName16 = 0x100E35AC,
  TimingPaneName17 = 0x100E35BC,
  TimingPaneName18 = 0x100E35CC,
  TimingPaneName19 = 0x100E35DC,
  TimingPaneName20 = 0x100E35EC,
  TimingPaneName21 = 0x100E35FC,
  Beat3PaneName1 = 0x100E360C,
  Beat3PaneName2 = 0x100E361C,
  Beat3PaneName3 = 0x100E362C,
  Beat4PaneName1 = 0x100E363C,
  Beat4PaneName2 = 0x100E364C,
  Beat4PaneName3 = 0x100E365C,
  Beat4PaneName4 = 0x100E366C,
  Beat6PaneName1 = 0x100E367C,
  TimingPaneTypeName = 0x100E3724,
  Beat6PaneName2 = 0x100E368C,
  Beat6PaneName3 = 0x100E369C,
  Beat6PaneName4 = 0x100E36AC,
  Beat6PaneName5 = 0x100E36BC,
  Beat6PaneName6 = 0x100E36CC,
  EchoPaneTypeName = 0x100E3738,
  LayoutResourceName = 0x100E36FC,
  BeatPaneTypeName = 0x100E374C,
  CloseStateName = 0x100E375C,
  OpenStateName = 0x100E36DC,
  WaitStateName = 0x100E376C,
  ShrinkStateName = 0x100E377C,
  DemoStateName = 0x100E3790,
  TimerStateName = 0x100E36EC,
  EndStateName = 0x100E37A0
};

enum VirtualSlot : u32 {
  StringAssure = 0x14,
  ScreenSet = 0x7c,
  DeleteBase = 0x84,
  UpdateBase = 0x8c,
  DrawBase = 0x9c,
  CreatePanes = 0xa4,
  DeletePanes = 0xac,
  UpdatePanes = 0xb4,
  DrawPanes = 0xc4,
  GetMelody = 0x2c,
  LoadResource = 0x14,
  FindPane = 0x5c,
  SelectState = 0x14,
  EnterState = 0x1c
};
inline u32 virtualTarget(u32 table, u32 slot) {
  return *view<be<u32>>(table + slot);
}
inline u32 controllerTarget(u32 p, u32 slot) {
  return virtualTarget(controller(p)->vtable, slot);
}
inline be<u32> *notePanes(u32 p, u32 beat) {
  auto *c = controller(p);
  return beat == 3 ? c->pane_note3 : beat == 4 ? c->pane_note4 : c->pane_note6;
}
inline be<u32> *targetPanes(u32 p, u32 beat) {
  auto *c = controller(p);
  return beat == 3   ? c->pane_target3
         : beat == 4 ? c->pane_target4
                     : c->pane_target6;
}
struct WordPair {
  be<u32> x, y;
};
struct PaneStrings {
  u8 base[8];
  String type;
  u8 reserved[4];
  String name;
};
struct ScreenOwnerVtable {
  u8 base[0xc];
  be<u32> screen;
};
struct PaneScreen {
  u8 base[8];
  be<u32> vtable;
};
struct StateObject {
  be<u32> vtable;
};
struct Descriptor {
  be<s32> value;
  be<u32> reserved, vtable;
};
struct HeapOwner {
  u8 base[0x10];
  be<u32> heap;
};
inline u32 currentHeap() {
  return view<HeapOwner>(*view<be<u32>>(0x1018c404))->heap;
}
inline u32 memberTarget(u32 desc, u32 base, u32 off, u32 &self, u32 &r9,
                        u32 &r10) {
  auto *member = view<MemberFunction>(desc + off);
  s32 slot = member->slot;
  r9 = u32(s32(member->adjustment));
  self = base + r9;
  if (slot < 0)
    return member->function;
  s32 vt = member->virtualEntry.vtableOffset;
  r9 = u32(slot) * 8;
  r10 = *view<be<u32>>(self + u32(vt)) + r9;
  return view<WordPair>(r10)->y;
}

enum ParticleId : u32 { BatonGuideParticle = 0x23e, BatonTactParticle = 0x23f };
enum AnimationIndex : u32 {
  AnmOpen,
  AnmBeat3,
  AnmBeat4,
  AnmBeat6,
  AnmShrink3,
  AnmShrink4,
  AnmShrink6,
  AnmEnd
};
enum StateFlag : u32 {
  EmitterStop = 1,
  EmitterDeleteEnabled = 8,
  EmitterImmortal = 0x40,
  TactInput = 0x01000000
};
constexpr u32 StringCompareLimit = 0x40001;
// HD: keep the dispatch register payload (including adjusted r9/r10) visible to the callee.
namespace sdk {
/* 020063C0 [v d_metronome] */
template <class R = void, class... A> inline R changeState(A... args) {
  return gabi::call<R>(0x020063C0, args...);
}
/* 020053E4 [v d_metronome] */
template <class R = void, class... A> inline R playAnimation(A... args) {
  return gabi::call<R>(0x020053E4, args...);
}
/* 0200552C [v d_metronome] */
template <class R = void, class... A> inline R setAnimationFrame(A... args) {
  return gabi::call<R>(0x0200552C, args...);
}
/* 025E1EFC [v d_metronome] */
template <class R = void, class... A> inline R getBeat(A... args) {
  return gabi::call<R>(0x025E1EFC, args...);
}
/* 0261D968 [v d_metronome] */
template <class R = void, class... A> inline R invokeAnimationPlay(A... args) {
  return gabi::call<R>(0x0261D968, args...);
}
/* 0261D98C [v d_metronome] */
template <class R = void, class... A> inline R invokeAnimationFrame(A... args) {
  return gabi::call<R>(0x0261D98C, args...);
}
/* 0261D9E4 [v d_metronome] */
template <class R = void, class... A> inline R invokeBeatPlay(A... args) {
  return gabi::call<R>(0x0261D9E4, args...);
}
/* 0261DA70 [v d_metronome] */
template <class R = void, class... A> inline R invokeHide(A... args) {
  return gabi::call<R>(0x0261DA70, args...);
}
/* 02702654 [v d_metronome] */
template <class R = void, class... A> inline R resetPane(A... args) {
  return gabi::call<R>(0x02702654, args...);
}
/* 0261E5A8 [v d_metronome] */
template <class R = void, class... A> inline R invokeShrinkPlay(A... args) {
  return gabi::call<R>(0x0261E5A8, args...);
}
/* 02005840 [v d_metronome] */
template <class R = void, class... A> inline R isAnimationEnd(A... args) {
  return gabi::call<R>(0x02005840, args...);
}
/* 025200D4 [v d_metronome] */
template <class R = void, class... A> inline R getPlay(A... args) {
  return gabi::call<R>(0x025200D4, args...);
}
/* 024430C0 [v d_metronome] */
template <class R = void, class... A> inline R getTactMetronomeRate(A... args) {
  return gabi::call<R>(0x024430C0, args...);
}
/* 0261DCD0 [v d_metronome] */
template <class R = void, class... A> inline R invokeTimer(A... args) {
  return gabi::call<R>(0x0261DCD0, args...);
}
/* 0261DDBC [v d_metronome] */
template <class R = void, class... A> inline R invokeTimerReset(A... args) {
  return gabi::call<R>(0x0261DDBC, args...);
}
/* 026F91F8 [v d_metronome] */
template <class R = void, class... A> inline R deleteScreen(A... args) {
  return gabi::call<R>(0x026F91F8, args...);
}
/* 025AF2A4 [v d_metronome] */
template <class R = void, class... A> inline R isMessageBusy(A... args) {
  return gabi::call<R>(0x025AF2A4, args...);
}
/* 02006364 [v d_metronome] */
template <class R = void, class... A> inline R moveState(A... args) {
  return gabi::call<R>(0x02006364, args...);
}
/* 0261DE20 [v d_metronome] */
template <class R = void, class... A> inline R invokeTimerMove(A... args) {
  return gabi::call<R>(0x0261DE20, args...);
}
/* 0261E1A0 [v d_metronome] */
template <class R = void, class... A> inline R invokeMelodyShow(A... args) {
  return gabi::call<R>(0x0261E1A0, args...);
}
/* 0261E638 [v d_metronome] */
template <class R = void, class... A> inline R invokeShrinkDone(A... args) {
  return gabi::call<R>(0x0261E638, args...);
}
/* 02620B68 [v d_metronome] */
template <class R = void, class... A> inline R showBeatEffect(A... args) {
  return gabi::call<R>(0x02620B68, args...);
}
/* 02704D2C [v d_metronome] */
template <class R = void, class... A> inline R getPaneCenter(A... args) {
  return gabi::call<R>(0x02704D2C, args...);
}
/* 025A847C [v d_metronome] */
template <class R = void, class... A> inline R createParticle(A... args) {
  return gabi::call<R>(0x025A847C, args...);
}
/* 025F0EA4 [v d_metronome] */
template <class R = void, class... A> inline R project(A... args) {
  return gabi::call<R>(0x025F0EA4, args...);
}
/* 0261DE98 [v d_metronome] */
template <class R = void, class... A>
inline R invokeMelodyGuideShow(A... args) {
  return gabi::call<R>(0x0261DE98, args...);
}
/* 02443144 [v d_metronome] */
template <class R = void, class... A> inline R getTactTopPos(A... args) {
  return gabi::call<R>(0x02443144, args...);
}
/* 0244311C [v d_metronome] */
template <class R = void, class... A> inline R checkTactLastInput(A... args) {
  return gabi::call<R>(0x0244311C, args...);
}
/* 0261E05C [v d_metronome] */
template <class R = void, class... A> inline R invokeMelodyFlash(A... args) {
  return gabi::call<R>(0x0261E05C, args...);
}
/* 0262057C [v d_metronome] */
template <class R = void, class... A> inline R showTimingEcho(A... args) {
  return gabi::call<R>(0x0262057C, args...);
}
/* 024431E0 [v d_metronome] */
template <class R = void, class... A> inline R checkTactPlayMelody(A... args) {
  return gabi::call<R>(0x024431E0, args...);
}
/* 025E1F68 [v d_metronome] */
template <class R = void, class... A> inline R getMelodyPattern(A... args) {
  return gabi::call<R>(0x025E1F68, args...);
}
/* 0261E834 [v d_metronome] */
template <class R = void, class... A> inline R invokeMelodyDemo(A... args) {
  return gabi::call<R>(0x0261E834, args...);
}
/* 0273AD10 [v d_metronome] */
template <class R = void, class... A> inline R allocate(A... args) {
  return gabi::call<R>(0x0273AD10, args...);
}
/* 026F89E8 [v d_metronome] */
template <class R = void, class... A>
inline R constructScreenController(A... args) {
  return gabi::call<R>(0x026F89E8, args...);
}
/* 026F88F0 [v d_metronome] */
template <class R = void, class... A>
inline R destroyScreenController(A... args) {
  return gabi::call<R>(0x026F88F0, args...);
}
/* 0273AF40 [v d_metronome] */
template <class R = void, class... A> inline R release(A... args) {
  return gabi::call<R>(0x0273AF40, args...);
}
/* 0273B050 [v d_metronome] */
template <class R = void, class... A> inline R allocateAligned(A... args) {
  return gabi::call<R>(0x0273B050, args...);
}
/* 026FADB0 [v d_metronome] */
template <class R = void, class... A> inline R constructScreen(A... args) {
  return gabi::call<R>(0x026FADB0, args...);
}
/* 02004E04 [v d_metronome] */
template <class R = void, class... A> inline R registerAnimation(A... args) {
  return gabi::call<R>(0x02004E04, args...);
}
/* 026F92C0 [v d_metronome] */
template <class R = void, class... A> inline R createBasePane(A... args) {
  return gabi::call<R>(0x026F92C0, args...);
}
/* 026F90D8 [v d_metronome] */
template <class R = void, class... A> inline R initializePaneNames(A... args) {
  return gabi::call<R>(0x026F90D8, args...);
}
/* 028F026C [v d_metronome] */
template <class R = void, class... A> inline R registerDestructor(A... args) {
  return gabi::call<R>(0x028F026C, args...);
}
/* 028ED6F8 [v d_metronome] */
template <class R = void, class... A> inline R constructStaticA(A... args) {
  return gabi::call<R>(0x028ED6F8, args...);
}
/* 028EAB2C [v d_metronome] */
template <class R = void, class... A> inline R constructStaticB(A... args) {
  return gabi::call<R>(0x028EAB2C, args...);
}
} // namespace sdk
} // namespace Metronome

namespace Metronome {
WWHD_OFFSET(Pane, active, 0x50);
WWHD_OFFSET(Pane, note, 0x5c);
WWHD_OFFSET(Pane, visible, 0x64);
WWHD_SIZE(Vec2, 8);
WWHD_SIZE(String, 8);
WWHD_OFFSET(Emitter, mMaxFrame, 0x5c);
WWHD_OFFSET(Emitter, particleCount, 0x1b4);
WWHD_OFFSET(Emitter, childParticleCount, 0x1c0);
WWHD_OFFSET(Emitter, mGlobalScale, 0x220);
WWHD_OFFSET(Emitter, mGlobalTranslation, 0x22c);
WWHD_OFFSET(Emitter, mParticleScale, 0x238);
WWHD_OFFSET(Emitter, mDrawGroup, 0x262);
WWHD_OFFSET(Screen, animation, 0xd4);
WWHD_OFFSET(Screen, resourceVtable, 0xe0);
WWHD_OFFSET(Play, particleControl, 0x5ab0);
WWHD_OFFSET(Play, player, 0x5b2c);
WWHD_OFFSET(Play, link, 0x5b34);
WWHD_OFFSET(Player, vtable, 0xb4);
WWHD_OFFSET(Player, flags, 0x3c0);
WWHD_OFFSET(Player, tactNote, 0x691c);
WWHD_SIZE(MemberFunction, 8);
WWHD_OFFSET(StateDescriptor, initialize, 12);
WWHD_OFFSET(StateDescriptor, execute, 20);
WWHD_OFFSET(StateDescriptor, draw, 28);
WWHD_SIZE(WordPair, 8);
WWHD_OFFSET(PaneStrings, type, 8);
WWHD_OFFSET(PaneStrings, name, 0x14);
} // namespace Metronome
