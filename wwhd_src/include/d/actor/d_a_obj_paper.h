/* Notice-board/message actor, WWHD layout.  HD: the actor base grows by 0x11C. The retained actor omits
 * GameCube's mpMsg, uses the global message manager, and has size 0x534
 * (GameCube 0x41C). */
#pragma once
#include "gc_api/gc_api.h"
namespace daObjPaper {
enum Type_e { Opaper_e, Ppos_e, Piwa_e };
enum Resource_e {
  dRes_INDEX_OPAPER_BDL_OPAPER_e = 3,
  dRes_INDEX_PPOS_BDL_PPOS_e = 3,
  dRes_INDEX_PIWA_BDL_PIWA_e = 3
};
enum HdState_e {
  CAMERA_ATTENTION_TALK_e = 4,
  MESSAGE_DESTROYED_e = 0x13,
  PAPER_MAP_ICON_MASK_e = 0x3F,
  PAPER_MAP_ICON_e = 0x38
};
struct Attr_c {
  /* 0x00 */ gptr<const char> mResName;
  /* 0x04 */ be<s32> mHeapSize;
  /* 0x08 */ be<s16> mModelId;
  /* 0x0A */ be<s16> mEyeOffset;
  /* 0x0C */ be<s16> mAttentionOffset;
  /* 0x0E */ be<s16> mCullSphereRadius;
  /* 0x10 */ be<s16> mCullSphereYOffset;
  /* 0x12 */ be<u8> mAttentionDist1;
  /* 0x13 */ be<u8> mAttentionDist2;
  /* 0x14 */ be<u8> mTevType;
  /* 0x15 */ u8 mPadding;
  /* 0x16 */ be<s16> mColCylinderRadius;
  /* 0x18 */ be<s16> mColCylinderHeight;
  /* 0x1A */ u8 mTailPadding[2];
};
struct Act_c : fopAc_ac_c {
  enum Prm_e {
    PRM_MSG_NO_W = 16,
    PRM_MSG_NO_S = 0,
    PRM_TYPE_W = 4,
    PRM_TYPE_S = 16
  };
  enum Act_Mode_e {
    ActMode_WAIT_e,
    ActMode_TALKBEGIN_e,
    ActMode_GETMSG_e,
    ActMode_TALKWAIT_e
  };
  Act_c();
  bool create_heap();
  void set_mtx();
  void init_mtx();
  void mode_wait_init();
  s32 _create();
  BOOL _delete();
  void damage_cc_proc();
  BOOL _execute();
  BOOL _draw();
  void mode_talk0_init();
  void mode_wait();
  void mode_talk1_init();
  void mode_talk0();
  void mode_talk2_init();
  void mode_talk1();
  void mode_talk2();
  /* 0x3AC */ request_of_phase_process_class mPhs;
  /* 0x3B4 */ gptr<J3DModel> mpModel;
  /* 0x3B8 */ dCcD_Cyl mCylinderCol;
  /* 0x4E8 */ dCcD_Stts mColStatus;
  /* 0x524 */ be<u8> mbHasCc;
  /* 0x525 */ u8 mPadding[3];
  /* 0x528 */ be<s32> mMode;
  /* 0x52C */ be<s32> mMsgId;
  /* 0x530 */ be<s32> mType;
};
} // namespace daObjPaper
WWHD_SIZE(daObjPaper::Attr_c, 0x1C);
WWHD_OFFSET(daObjPaper::Act_c, mCylinderCol, 0x3B8);
WWHD_OFFSET(daObjPaper::Act_c, mColStatus, 0x4E8);
WWHD_OFFSET(daObjPaper::Act_c, mType, 0x530);
WWHD_SIZE(daObjPaper::Act_c, 0x534);

// HD: the cylinder's shape-attribute base contains two constructor vtable
// slots.
struct PaperShapeAttrHD {
  u8 mPadding[0x18];
  be<u32> mCylinderVtable, mShapeVtable;
};
struct PaperCylinderSourceHD {
  be<u32> words[17];
};
struct PaperModeEntryHD {
  using ModeFunc = void (daObjPaper::Act_c::*)();
  gabi::ptmf<ModeFunc> function;
};
struct PaperHeaderZerosHD {
  be<u32> words[4];
};
struct PaperRegistrationHD {
  u8 bytes[12];
};
struct PaperMessageManagerHD {
  s32 messageSet(u32 message, cXyz *position);
  s32 getStatus();
  void setStatus(s32 status);
};
void Paper_constructGObjInf(dCcD_Cyl *);
void Paper_constructCylinder(cM3dGCyl *);
void Paper_constructStts(dCcD_Stts *);
void Paper_constructGStts(be<u32> *);
void Paper_destructStts(dCcD_Stts *, s32);
void Paper_destructCylinder(dCcD_Cyl *, s32);
