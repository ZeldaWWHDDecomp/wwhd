/* Magtail main-source views. HD-only offsets keep the verified load/store order.
 * The common Magtail layout is shared by separately claimed units and stays unchanged. */
#pragma once
#include "d/actor/d_a_mt.h"
#include "gc_api/tu.h"
#include "gc_api/f_op/f_op_actor.h"
#include "d/d_cc_uty.h"
struct MtAnmVtable {
    /* 0x000 */ u8 pad[0x1C];
    /* 0x01C */ gfn mSearchUpdateMaterialID;
};
struct MtAnmView {
    /* 0x000 */ be<f32> mFrame;
    /* 0x004 */ gptr<MtAnmVtable> mpVtable;
    void searchUpdateMaterialID(void *self, J3DModelData *data) {
        gabi::call_ptr(mpVtable->mSearchUpdateMaterialID.get(), self, data);
    }
};
struct MtJointView {
    /* 0x000 */ u8 pad[4];
    /* 0x004 */ be<u16> mJntNo;
};
struct MtJointNodeView {
    /* 0x000 */ u8 pad[8];
    /* 0x008 */ be<u32> mCallBack;
    /* 0x00C */ u8 tail[0x10];
};
struct MtJointTreeView {
    /* 0x000 */ u8 pad[8];
    /* 0x008 */ be<u16> mJointNum;
};
struct MtEmitterView {
    /* 0x000 */ u8 pad[0x5C];
    /* 0x05C */ be<s32> mMaxFrame;
    /* 0x060 */ u8 pad2[0x254 - 0x60];
    /* 0x254 */ be<u32> mStatus;
};
struct MtAppendView {
    /* 0x000 */ be<u32> mParameters;
    /* 0x004 */ cXyz mPosition;
    /* 0x010 */ csXyz mAngle;
    /* 0x016 */ u8 pad[0x21 - 0x16];
    /* 0x021 */ be<s8> mRoomNo;
};
struct MtAcchView {
    /* 0x000 */ u8 pad[0x14];
    /* 0x014 */ be<u32> mVtable;
    /* 0x018 */ u8 pad2[8];
    /* 0x020 */ be<u32> mSecondaryVtable;
};
struct MtAcchCirView {
    /* 0x000 */ u8 pad[0x14];
    /* 0x014 */ u8 mCircle[0x2C];
};
struct MtPathPoint {
    /* 0x000 */ u8 pad[4];
    /* 0x004 */ cXyz mPosition;
};
struct MtPathView {
    /* 0x000 */ u8 pad[8];
    /* 0x008 */ gptr<MtPathPoint> mpPoints;
};
struct MtSinCosEntry {
    /* 0x000 */ be<f32> sin, cos;
};
WWHD_SIZE(MtAnmView, 8);
WWHD_SIZE(MtSinCosEntry, 8);
WWHD_OFFSET(MtAnmVtable, mSearchUpdateMaterialID, 0x1C);
WWHD_OFFSET(MtJointView, mJntNo, 4);
WWHD_OFFSET(MtJointNodeView, mCallBack, 8);
WWHD_OFFSET(MtJointTreeView, mJointNum, 8);
WWHD_OFFSET(MtEmitterView, mMaxFrame, 0x5C);
WWHD_OFFSET(MtEmitterView, mStatus, 0x254);
WWHD_OFFSET(MtAppendView, mPosition, 4);
WWHD_OFFSET(MtAppendView, mAngle, 0x10);
WWHD_OFFSET(MtAppendView, mRoomNo, 0x21);
WWHD_OFFSET(MtAcchView, mVtable, 0x14);
WWHD_OFFSET(MtAcchView, mSecondaryVtable, 0x20);
WWHD_OFFSET(MtAcchCirView, mCircle, 0x14);
WWHD_OFFSET(MtPathPoint, mPosition, 4);
WWHD_OFFSET(MtPathView, mpPoints, 8);
enum { MT_SPH_CONSTRUCTOR = 0x025166F0, MT_SPH_DESTRUCTOR = 0x02515AE8 };

// HD: sound and figure IDs differ from the GameCube audio table.
enum { MT_SE_FALL_WATER = 0x5820, MT_SE_DAMAGE = 0x4842, MT_SNAP_TYPE = 0xAF };

void mt_monsSeStart(u32 id, cXyz *pos, u32 actorId, u32 param, s32 reverb);
mDoExt_btkAnm *mt_btkAnm_ct(mDoExt_btkAnm *p);
u32 mt_followEcallBack_ct(dPa_followEcallBack *p, u8 a, u8 b);
void mt_construct_array(void *p, u32 n, u32 size, u32 ctor);
void mt_destroy_arr(void *p, u32 n, u32 size, u32 dtor, s32 a, s32 b);
void mt_Sph_ct(dCcD_Sph *p);
void mt_Sph_dt(dCcD_Sph *p, s32 flags);
void mt_Acch_dt(dBgS_ObjAcch *p, s32 flags);
void mt_Actor_dt(mt_class *p, s32 flags);
void mt_Cir_dt(void *p, s32 flags);

struct MtModelView {
    /* 0x000 */ u8 pad[0xB8];
    /* 0x0B8 */ gptr<mt_class> mpActor;
};
struct MtModelDataView {
    /* 0x000 */ u8 pad[4];
    /* 0x004 */ be<u32> mNodeNum;
    /* 0x008 */ gptr<MtJointNodeView> mpNodes;
};
WWHD_OFFSET(MtModelView, mpActor, 0xB8);
WWHD_OFFSET(MtModelDataView, mNodeNum, 4);
WWHD_OFFSET(MtModelDataView, mpNodes, 8);
WWHD_SIZE(MtJointNodeView, 0x1C);
MtJointTreeView *mtJointTree(J3DModelData *);
WWHD_SIZE(MtAnmVtable, 0x20);
WWHD_SIZE(MtJointView, 6);
WWHD_SIZE(MtJointTreeView, 0xA);
WWHD_SIZE(MtEmitterView, 0x258);
WWHD_SIZE(MtAppendView, 0x22); // Partial view through the room number.
WWHD_SIZE(MtAcchView, 0x24);
WWHD_SIZE(MtAcchCirView, 0x40);
WWHD_SIZE(MtPathPoint, 0x10);
WWHD_SIZE(MtPathView, 0xC);
WWHD_SIZE(MtModelView, 0xBC);
WWHD_SIZE(MtModelDataView, 0xC);

enum { MT_BURST_EFFECT = 0x8096 };

// GameCube supplies no joint/state labels for this placeholder implementation.
enum { MT_JNT_BEND_FIRST = 2, MT_JNT_BEND_SECOND = 3, MT_JNT_TWIST_FIRST = 4, MT_JNT_TWIST_SECOND = 5 };
enum { MT_STATE_FIGHT = 1, MT_STATE_DEAD = 3, MT_INITIAL_BCK = 10 };
