/* Forest Haven acorn leaf, HD layout: actor members +0x11C from tww.
 * Original tww unknown member names retained. */
#pragma once
#include "gc_api/gc_api.h"

enum { dRes_INDEX_VIGAH_BCK_VIGAH_e = 4, dRes_INDEX_VIGAH_BDL_VIGAH_e = 7 };

// HD: the animation calculator follows the reordered frame controller. Private view until the shared layout is verified.
struct AleafBckAnm : mDoExt_baseAnm {
    /* 0x10 */ be<u32> mMtxCalcVtable;
    /* 0x14 */ u8 mMtxCalcBase[0x34];
    /* 0x48 */ be<u32> mBlendVtable;
    /* 0x4C */ u8 mBlendState[0xC];
    /* 0x58 */ be<u32> mBlendFlags;
    /* 0x5C */ u8 mTransformState[0x20];
    /* 0x7C */ be<u32> mAnmFlags;
    /* 0x80 */ be<u32> mAnmIndex;
    /* 0x84 */ be<u32> mAnmData;
    /* 0x88 */ be<u32> mAnmTarget;
    BOOL init(J3DModelData*, J3DAnmTransform*, bool, s32, f32, s16, s16, bool);
    void entry(J3DModelData*, f32);
};
WWHD_OFFSET(AleafBckAnm, mMtxCalcVtable, 0x10);
WWHD_OFFSET(AleafBckAnm, mBlendVtable, 0x48);
WWHD_OFFSET(AleafBckAnm, mBlendFlags, 0x58);
WWHD_OFFSET(AleafBckAnm, mAnmFlags, 0x7C);
WWHD_OFFSET(AleafBckAnm, mAnmTarget, 0x88);
WWHD_SIZE(AleafBckAnm, 0x8C);

struct daAleaf_c : fopAc_ac_c {
    daAleaf_c();
    static const char m_arcname[];
    BOOL CreateHeap();
    void CreateInit();
    void create_acorn();
    fpc_ProcID create_acorn_sub(bool);
    cPhs_State _create();
    void set_mtx();
    bool _execute();
    bool _draw();

    /* 0x3AC */ request_of_phase_process_class mPhs;  /* was unk_290: _create 02047948 loads the m_arcname resource into it (dComIfG_resLoad); daAleaf_Delete 02047AD4 releases it (tww the same) */
    /* 0x3B4 */ gptr<J3DModel> mpModel;  /* was unk_298: CreateHeap 020475B4 creates the leaf model; set_mtx 020476D0 sets its scale / matrix, _draw 02047B04 draws it (tww the same) */
    /* 0x3B8 */ dCcD_Stts mStts;  /* was unk_29C: CreateInit 02047878 inits it (255, 255) and gives it to mCyl; constructed in the constructor, destroyed in daAleaf_dtor 02047FBC (tww the same) */
    /* 0x3F4 */ dCcD_Cyl mCyl;  /* was unk_2D8: CreateInit 02047878 sets it from l_cyl_src with mStts; constructed / destroyed with the actor (tww the same) */
    /* 0x524 */ AleafBckAnm mBck;  /* was unk_408: CreateHeap 020475B4 sets up the leaf's bck (EMode_RESET); _execute 02047C00 restarts and plays it while mBckPlay, _draw 02047B04 enters it (tww the same) */
    /* 0x5B0 */ be<u8> mBckPlay;  /* was unk_418: _execute 02047C00 sets it (and the bck rate 1) when the leaf is disturbed (acorn appears / falls off, the player steps close, a dropped object lands near) and clears it when the bck play ends (tww the same) */
    /* 0x5B1 */ be<u8> mAcornStuckOld;  /* was unk_419: _execute 02047C00 stores whether the acorn (Tsubo child mAcornId) still sticks (prm_get_stick); when it was stuck last frame and is not now, the leaf sways (tww the same) */
    /* 0x5B2 */ be<u8> unk_41A;
    /* 0x5B3 */ be<u8> unk_41B;
    /* 0x5B4 */ be<s32> mAcornWaitCount;  /* was unk_41C: _execute 02047C00 counts it up while the acorn exists and at 69 plays the sway once (then 70); create_acorn_sub 020477B0 clears it (tww the same) */
    /* 0x5B8 */ be<s32> mRespawnTimer;  /* was unk_420: create_acorn 02047B8C creates a new acorn only while it is 0 and the old one is gone, then sets 60 and counts it down (tww the same) */
    /* 0x5BC */ be<u32> mAcornId;  /* was unk_424: create_acorn_sub 020477B0 stores the process id of the acorn (a Tsubo child); create_acorn 02047B8C / _execute 02047C00 search it; CreateInit 02047878 sets the error id (tww the same) */
    /* 0x5C0 */ be<u32> mGrabIdOld;  /* was unk_428: _execute 02047C00 stores the player's grab actor id each frame; when the player has just let go (no grab now) an object dropped within 70 sways the leaf; CreateInit 02047878 sets the error id (tww the same) */
    /* 0x5C4 */ be<f32> mPlayerDistOld;  /* was unk_42C: _execute 02047C00 stores the XZ distance to the player; a step closer by more than 3 within 50 (and at the same height) sways the leaf (tww the same) */
};
WWHD_OFFSET(daAleaf_c, mStts, 0x3B8);
WWHD_OFFSET(daAleaf_c, mCyl, 0x3F4);
WWHD_OFFSET(daAleaf_c, mBck, 0x524);
WWHD_OFFSET(daAleaf_c, mBckPlay, 0x5B0);
WWHD_OFFSET(daAleaf_c, mPlayerDistOld, 0x5C4);
WWHD_SIZE(daAleaf_c, 0x5C8);

void AleafFrameCtrl_init(J3DFrameCtrl*, s32);
void AleafMtxCalc_construct(void*);
void AleafMtxCalc_destroy(be<u32>*, s32);
void Aleaf_XrotS(Mtx34*, s16);
fpc_ProcID Aleaf_createChildFromOffset(s16, fpc_ProcID, u32, cXyz*, s32, csXyz*, cXyz*, s32, u32);
BOOL Aleaf_SearchByID(fpc_ProcID, gptr<fopAc_ac_c>*);
// HD: these callbacks forward the entire r3; the inner method's bool does not extend it.
BOOL Aleaf_drawResult(void*);
BOOL Aleaf_executeResult(void*);

struct AleafPlayerVtable {
    u8 baseSlots[0xBC];
    be<u32> getGrabActorID;
};
WWHD_OFFSET(AleafPlayerVtable, getGrabActorID, 0xBC);
inline fpc_ProcID Aleaf_getGrabActorID(fopAc_ac_c* player) {
    auto* vtable = gabi::at<AleafPlayerVtable>(player->__vtbl);
    return gabi::call_ptr<fpc_ProcID>(vtable->getGrabActorID, player);
}
struct AleafJoint {
    u8 base[0x14];
    be<u32> mMtxCalc;
};
WWHD_OFFSET(AleafJoint, mMtxCalc, 0x14);
inline void Aleaf_removeAnimation(J3DModelData* data) {
    // HD: root joint pointer is read directly, without the indexed accessor's count load.
    auto* joint = gabi::at<AleafJoint>(data->mJointNodes);
    joint->mMtxCalc = 0;
}
