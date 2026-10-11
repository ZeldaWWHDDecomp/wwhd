/* Fire Mountain exterior, WWHD. */
#pragma once
#include "gc_api/gc_api.h"
namespace daObjVolcano {
enum { AT_TYPE_ICE_ARROW = 0x80000 };
enum ParticleName {
  ID_AK_SN_O_FIRESHAFTHEAD = 0x805A,
  ID_AK_SN_O_FIRESHAFTBODY = 0x805B,
  ID_AK_SN_O_FIRESHAFTFOOT = 0x805C,
  ID_AK_SN_MINIKAZANICE00 = 0x8191,
  ID_AK_SN_MINIKAZANSTEAM00 = 0x8194
};
enum SoundName {
  JA_SE_OBJ_VOLCANO_ERUPTION = 0x61CD,
  JA_SE_ATM_VOLCANO_MAGMA = 0x1073,
  JA_SE_OBJ_VOLCANO_CHILL = 0x69CE,
  JA_SE_OBJ_VOLCANO_ICE_BRK = 0x69CF,
  JA_SE_READ_RIDDLE_1 = 0x806
};

namespace HD {
struct Color {
  be<u8> r, g, b, a;
};
struct Emitter {
  u8 pad0[0x5C];
  be<u32> mUserWork;
  u8 pad60[0x1E4];
  Color mPrmColor, mEnvColor;
  u8 pad24C[8];
  be<u32> mFlags;
};
struct Environment {
  u8 pad0[0xB7C];
  Color mBG2_C0, mBG2_K0;
};
struct Save {
  u8 pad0[0x1148];
  be<s8> mRestartRoom;
  u8 pad1149[0x13];
  be<s16> mStartPoint;
};
struct Word {
  be<u32> value;
};
struct Shape {
  u8 pad0[4];
  be<u8> mVisible;
};
struct Material {
  u8 pad0[4];
  be<u32> mpNext, mpShape;
  u8 padC[0xC];
  be<u32> mpTev;
  u8 pad1C[0x84];
  be<u32> mUniformFlags;
};
struct Tev {
  u8 pad0[4];
  be<u32> mpVtable;
};
struct TevVtable {
  u8 pad0[0x3C];
  be<u32> mSetKColor;
  u8 pad40[0xC];
  be<u32> mGetKColor;
};
struct ModelData {
  u8 pad0[4];
  be<u32> mJointNum, mpJoints;
};
struct Joint {
  u8 pad0[0x10];
  be<u32> mpMaterial;
  u8 pad14[8];
};
struct Game {
  u8 pad0[0x12A0];
  u8 mBg[0x26A4 - 0x12A0];
  u8 mCollision[0x52B8 - 0x26A4];
  be<u16> mEventFlags;
  u8 pad52BA[10];
  u8 mEventManager[0x5AB0 - 0x52C4];
  be<u32> mpParticle;
  u8 pad5AB4[0x2BC];
  be<u32> mBGOpa, mBGXlu, mOpa, mXlu;
};
struct DrawLists {
  be<u32> mOpa, mXlu;
};
struct HeaderStatics {
  be<f32> mMinAngle, mMaxAngle;
  be<u8> mDirection, mAxis;
  u8 padA[2];
  be<u32> mWords[4];
};
/* 025E80D0: HD SDK binding used by this unit. */
template <class... A> inline void brkConstruct(A... args) {
  gabi::call(0x025E80D0, args...);
}
/* 025E1B34: HD SDK binding used by this unit. */
template <class... A> inline void deleteAudioObject(A... args) {
  gabi::call(0x025E1B34, args...);
}
/* 025E8154: HD SDK binding used by this unit. */
template <class... A> inline BOOL brkInit(A... args) {
  return gabi::call<BOOL>(0x025E8154, args...);
}
/* 028E93CC: HD SDK binding used by this unit. */
template <class... A> inline void matrixTranslate(A... args) {
  gabi::call(0x028E93CC, args...);
}
/* 025F1B48: HD SDK binding used by this unit. */
template <class... A> inline void matrixZXYrot(A... args) {
  gabi::call(0x025F1B48, args...);
}
/* 028E90D4: HD SDK binding used by this unit. */
template <class... A> inline void matrixCopy(A... args) {
  gabi::call(0x028E90D4, args...);
}
/* 020087EC: HD SDK binding used by this unit. */
template <class... A> inline void releaseBg(A... args) {
  gabi::call(0x020087EC, args...);
}
/* 024EEA6C: HD SDK binding used by this unit. */
template <class... A> inline void registerBg(A... args) {
  gabi::call(0x024EEA6C, args...);
}
/* 025A847C: HD SDK binding used by this unit. */
template <class... A> inline u32 setParticle(A... args) {
  return gabi::call<u32>(0x025A847C, args...);
}
/* 02543F10: HD SDK binding used by this unit. */
template <class... A> inline s16 getEventIdx(A... args) {
  return gabi::call<s16>(0x02543F10, args...);
}
/* 025D7A58: HD SDK binding used by this unit. */
template <class... A> inline void orderEvent(A... args) {
  gabi::call(0x025D7A58, args...);
}
/* 025440C8: HD SDK binding used by this unit. */
template <class... A> inline BOOL endCheck(A... args) {
  return gabi::call<BOOL>(0x025440C8, args...);
}
/* 0200F5C8: HD SDK binding used by this unit. */
template <class... A> inline void chaseF(A... args) {
  gabi::call(0x0200F5C8, args...);
}
/* 025E1988: HD SDK binding used by this unit. */
template <class... A> inline void startSystemSound(A... args) {
  gabi::call(0x025E1988, args...);
}
/* 0200E240: HD SDK binding used by this unit. */
template <class... A> inline void setCollision(A... args) {
  gabi::call(0x0200E240, args...);
}
/* 02555D0C: HD SDK binding used by this unit. */
template <class... A> inline u32 getEnvLight(A... args) {
  return gabi::call<u32>(0x02555D0C, args...);
}
/* 0274D458: HD SDK binding used by this unit. */
template <class... A> inline void linearToSrgb(A... args) {
  gabi::call(0x0274D458, args...);
}
/* 027F9F0C: HD SDK binding used by this unit. */
template <class... A> inline u32 getMaterialUniform(A... args) {
  return gabi::call<u32>(0x027F9F0C, args...);
}
/* 025626A4: HD SDK binding used by this unit. */
template <class... A> inline void settingTevStruct(A... args) {
  gabi::call(0x025626A4, args...);
}
/* 02562F5C: HD SDK binding used by this unit. */
template <class... A> inline void setLightTevColor(A... args) {
  gabi::call(0x02562F5C, args...);
}
/* 025E2DE0: HD SDK binding used by this unit. */
template <class... A> inline void modelUpdateDL(A... args) {
  gabi::call(0x025E2DE0, args...);
}
/* 025E83FC: HD SDK binding used by this unit. */
template <class... A> inline void brkEntry(A... args) {
  gabi::call(0x025E83FC, args...);
}
/* 028F026C: HD SDK binding used by this unit. */
template <class... A> inline void safeStringConstruct(A... args) {
  gabi::call(0x028F026C, args...);
}
/* 028ED6F8: HD SDK binding used by this unit. */
template <class... A> inline void directionConstruct(A... args) {
  gabi::call(0x028ED6F8, args...);
}
/* 028EAB2C: HD SDK binding used by this unit. */
template <class... A> inline void axisConstruct(A... args) {
  gabi::call(0x028EAB2C, args...);
}
/* 0273AF40: HD SDK binding used by this unit. */
template <class... A> inline void deleteObject(A... args) {
  gabi::call(0x0273AF40, args...);
}
/* 025D50BC: HD SDK binding used by this unit. */
template <class... A> inline void moveBgDestruct(A... args) {
  gabi::call(0x025D50BC, args...);
}
template <class T> inline T *view(u32 address) { return gabi::at<T>(address); }
inline Game *game(u32 address) { return view<Game>(address); }
WWHD_OFFSET(Game, mpParticle, 0x5AB0);
WWHD_OFFSET(Game, mBGOpa, 0x5D70);
WWHD_OFFSET(Emitter, mPrmColor, 0x244);
WWHD_OFFSET(Emitter, mFlags, 0x254);
WWHD_OFFSET(Material, mUniformFlags, 0xA0);
WWHD_SIZE(Joint, 0x1C);
} // namespace HD

struct Brk_l : mDoExt_baseAnm {
  u8 padding10[0x78 - 0x10];
};
struct FloatColor {
  be<f32> r, g, b, a;
};
struct Act_c : dBgS_MoveBgActor {
  enum Prm_e { PRM_SWSAVE_W = 8, PRM_SWSAVE_S = 8 };
  enum State {
    FIRE,
    FREEZE_DEMO_WAIT,
    FREEZE_DEMO_MAIN,
    FREEZE,
    FIRE_DEMO_WAIT,
    FIRE_DEMO_MAIN,
    FAIL_DEMO_WAIT,
    FAIL_DEMO_MAIN
  };
  enum Resource {
    dRes_INDEX_YKZYG_BDL_QKZYG_e = 6,
    dRes_INDEX_YKZYG_BDL_YMNKZ00_e = 7,
    dRes_INDEX_YKZYG_BRK_YMNKZ00_e = 10,
    dRes_INDEX_YKZYG_BTK_QKZYG_e = 13,
    dRes_INDEX_YKZYG_BTK_YMNKZ00_e = 14
  };

  gptr<HD::Emitter> mEmitters[10];
  request_of_phase_process_class mPhase;
  gptr<J3DModel> mpModel;
  mDoExt_btkAnm mBtk;
  gptr<J3DModel> mpFireModel;
  mDoExt_btkAnm mFireBtk;
  Brk_l mFireBrk;
  dCcD_Stts mStts;
  dCcD_Cyl mCyl;
  cXyz mParticleScale, mFirePos, mHeadPos, mMiddlePos, mFootPos;
  be<s32> mTimer;
  be<f32> mHeight, mRadius;
  be<u8> mFireVisible;
  u8 padding72D[3];
  be<f32> mOpacity;
  be<s16> mFreezeEvent, mFireEvent;
  be<s32> mState;
  s32 Mthd_Create();
  BOOL Mthd_Delete();
  BOOL CreateHeap();
  BOOL Create();
  BOOL Delete();
  void set_mtx();
  void init_mtx();
  void StopFire();
  void StartFire();
  void daObjVolcano_fire_main();
  void daObjVolcano_freeze_demo_wait();
  void daObjVolcano_freeze_demo_main();
  void daObjVolcano_freeze_main();
  void daObjVolcano_fire_demo_wait();
  void daObjVolcano_fire_demo_main();
  void daObjVolcano_fail_demo_wait();
  void daObjVolcano_fail_demo_main();
  BOOL Execute(gptr<Mtx34> *);
  BOOL Draw();
};
WWHD_OFFSET(Act_c, mBtk, 0x414);
WWHD_OFFSET(Act_c, mFireBrk, 0x500);
WWHD_OFFSET(Act_c, mStts, 0x578);
WWHD_OFFSET(Act_c, mCyl, 0x5B4);
WWHD_OFFSET(Act_c, mFirePos, 0x6F0);
WWHD_OFFSET(Act_c, mState, 0x738);
WWHD_SIZE(Act_c, 0x73C);
} // namespace daObjVolcano
