/* Process base classes (f_pc_base.h / f_pc_leaf.h / f_pc_profile.h of zeldaret/tww): WWHD layout.
 *
 * GameCube -> WWHD: identical up to mParameters (0xB0). HD gives the process classes a C++ vtable
 * (virtual destructor) at 0xB4 where GameCube has mSubType: base_process_class is 0xB8 (its
 * constructor 025DD5F0 allocates 0xB8 and stores the vtable at 0xB4), and the sub type the leaf
 * functions write is at 0xB8, the start of leafdraw_class, which moves the leafdraw_class
 * members by 4 (mpDrawMtd 0xBC, mDwPi 0xC2; leafdraw_class is 0xC4 instead of 0xC0).
 * Evidence: fpcBs_Create 025DD414 (tags at 0x18/0x34/0x4C/0x68, mBsPcId, mProcName, mProfName,
 * mpProf, mpPcMtd, mpUserData, mParameters), fpcBs_Execute 025DD2B8 (layer at 0x2C, methods at
 * 0xA8), fpcLf_* 025DF2B8..025DF35C (mSubType 0xB8, mpDrawMtd 0xBC, mbUnk0 0xC0, mDwPi 0xC2),
 * fopAcM_ct (vtable at 0xB4). Members marked [g] are GameCube names whose WWHD use has not been
 * exercised by a verified function yet. */
#pragma once
#include "wwhd.h"

struct layer_class;
struct process_method_class;
struct process_profile_definition;

/* create_tag_class (0x14): node (0xC), tag data, in-use flag [g] */
struct create_tag_class {
    /* 0x00 */ u8 mNode[0xC];
    /* 0x0C */ be<u32> mpTagData;
    /* 0x10 */ be<s8> mbIsUse;
    /* 0x11 */ u8 _11[3];
};
WWHD_SIZE(create_tag_class, 0x14);

/* layer_management_tag_class (0x1C) */
struct layer_management_tag_class {
    /* 0x00 */ create_tag_class mCreateTag;
    /* 0x14 */ gptr<layer_class> mpLayer; /* [v fpcBs_Execute] */
    /* 0x18 */ be<u16> mNodeListID; /* [g] */
    /* 0x1A */ be<u16> mNodeListIdx; /* [g] */
};
WWHD_SIZE(layer_management_tag_class, 0x1C);

struct base_process_class {
    /* 0x00 */ be<s32> mBsType;
    /* 0x04 */ be<u32> mBsPcId;     /* fpc_ProcID */
    /* 0x08 */ be<s16> mProcName;
    /* 0x0A */ be<s8> mUnk0;
    /* 0x0B */ be<u8> mPauseFlag;   /* [g] */
    /* 0x0C */ be<s8> mInitState;
    /* 0x0D */ be<s8> mCreateResult; /* [g] */
    /* 0x0E */ be<s16> mProfName;
    /* 0x10 */ gptr<process_profile_definition> mpProf;
    /* 0x14 */ be<u32> mpCtRq;      /* create_request* [g] */
    /* 0x18 */ layer_management_tag_class mLyTg;
    /* 0x34 */ u8 mLnTg[0x18];      /* line_tag */
    /* 0x4C */ u8 mDtTg[0x1C];      /* delete_tag_class */
    /* 0x68 */ u8 mPi[0x40];        /* process_priority_class */
    /* 0xA8 */ gptr<process_method_class> mpPcMtd;
    /* 0xAC */ gptr<void> mpUserData;
    /* 0xB0 */ be<u32> mParameters;
    /* 0xB4 */ be<u32> __vtbl;      /* HD: C++ vtable (virtual destructor); GameCube: mSubType */
};
WWHD_SIZE(base_process_class, 0xB8);
WWHD_OFFSET(base_process_class, mLyTg, 0x18);
WWHD_OFFSET(base_process_class, mpPcMtd, 0xA8);
WWHD_OFFSET(base_process_class, mParameters, 0xB0);

/* leafdraw_class (HD 0xC4, GameCube 0xC0) */
struct leafdraw_class : base_process_class {
    /* 0xB8 */ be<s32> mSubType;    /* HD: here (GameCube: base_process_class 0xB4) */
    /* 0xBC */ be<u32> mpDrawMtd;   /* leafdraw_method_class* (GameCube 0xB8) */
    /* 0xC0 */ be<s8> mbUnk0;
    /* 0xC1 */ be<u8> mbUnk1;       /* [g] */
    /* 0xC2 */ be<s16> mDwPi;       /* draw_priority_class (GameCube 0xBE) */
};
WWHD_SIZE(leafdraw_class, 0xC4);
WWHD_OFFSET(leafdraw_class, mpDrawMtd, 0xBC);

/* Profiles (unchanged from GameCube: fpcBs_Create and fopAc_Create read these offsets) */
struct process_profile_definition {
    /* 0x00 */ be<u32> mLayerID;
    /* 0x04 */ be<u16> mListID;
    /* 0x06 */ be<u16> mListPrio;
    /* 0x08 */ be<s16> mProcName;
    /* 0x0A */ u8 _0A[2];
    /* 0x0C */ gptr<process_method_class> sub_method;
    /* 0x10 */ be<s32> mSize;
    /* 0x14 */ be<s32> mSizeOther;
    /* 0x18 */ be<u32> mParameters;
};
WWHD_SIZE(process_profile_definition, 0x1C);

struct leaf_process_profile_definition : process_profile_definition {
    /* 0x1C */ be<u32> sub_method;  /* leafdraw_method_class* (hides the base's) */
    /* 0x20 */ be<s16> mPriority;
    /* 0x22 */ u8 _22[2];
};
WWHD_SIZE(leaf_process_profile_definition, 0x24);

struct actor_process_profile_definition : leaf_process_profile_definition {
    /* 0x24 */ be<u32> sub_method;  /* actor_method_class* */
    /* 0x28 */ be<u32> status;
    /* 0x2C */ be<u8> group;
    /* 0x2D */ be<u8> cullType;
    /* 0x2E */ u8 _2E[2];
};
WWHD_SIZE(actor_process_profile_definition, 0x30);

/* 025DD258 (f_pc_base unit): the one declaration, GameCube signature */
BOOL fpcBs_Is_JustOfType(int i_typeA, int i_typeB);
