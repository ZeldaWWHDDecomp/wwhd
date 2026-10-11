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

    /* 0x3AC */ request_of_phase_process_class unk_290;
    /* 0x3B4 */ gptr<J3DModel> unk_298;
    /* 0x3B8 */ dCcD_Stts unk_29C;
    /* 0x3F4 */ dCcD_Cyl unk_2D8;
    /* 0x524 */ AleafBckAnm unk_408;
    /* 0x5B0 */ be<u8> unk_418;
    /* 0x5B1 */ be<u8> unk_419;
    /* 0x5B2 */ be<u8> unk_41A;
    /* 0x5B3 */ be<u8> unk_41B;
    /* 0x5B4 */ be<s32> unk_41C;
    /* 0x5B8 */ be<s32> unk_420;
    /* 0x5BC */ be<u32> unk_424;
    /* 0x5C0 */ be<u32> unk_428;
    /* 0x5C4 */ be<f32> unk_42C;
};
WWHD_OFFSET(daAleaf_c, unk_29C, 0x3B8);
WWHD_OFFSET(daAleaf_c, unk_2D8, 0x3F4);
WWHD_OFFSET(daAleaf_c, unk_408, 0x524);
WWHD_OFFSET(daAleaf_c, unk_418, 0x5B0);
WWHD_OFFSET(daAleaf_c, unk_42C, 0x5C4);
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
