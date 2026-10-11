/* daNpcPhoto_c (Lenzo, Windfall photographer), WWHD layout.
 *
 * GameCube -> WWHD, measured from the constructor 022CC418 (allocates 0xB50) and the verified
 * functions: fopNpc_npc_c is 0x7DC (d/d_npc.h), so members are +0x118 up to mBtpAnm (0x7F0);
 * mDoExt_btpAnm grew from 0x14 to 0x74 and mShadowId (GameCube 0x6EC) is gone (HD shadows), so
 * everything from mPathRun (GameCube 0x6F0) on is +0x174. 0xB4E is an HD-only byte (zeroed by
 * the constructor). Size 0xB50 (GameCube 0x9DC). */
#pragma once
#include "gc_api/gc_api.h"
#include "d/d_path.h"
#include "d/d_mesg_core.h"
#include "d/actor/d_a_tag_photo.h"

#define PHOTO_SAFESTRING_VTBL 0x10020C40 /* this TU's sead::SafeString vtable */
#define PHOTO_VTBL 0x10020FFC            /* daNpcPhoto_c vtable (constructor) */
#define PHOTO_AAB_VTBL 0x10020C58        /* this TU's cM3dGAab vtable */

struct sPhotoAnmDat {
    /* 0x00 */ be<u8> field_0x00;
    /* 0x01 */ be<u8> field_0x01;
    /* 0x02 */ be<u8> field_0x02;
};
WWHD_SIZE(sPhotoAnmDat, 3);

/* l_npc_dat (.data 0x101C521C) */
struct NpcDatStruct {
    /* 0x00 */ be<f32> field_0x00;
    /* 0x04 */ be<s16> field_0x04;
    /* 0x06 */ be<s16> field_0x06;
    /* 0x08 */ be<s16> field_0x08;
    /* 0x0A */ be<s16> field_0x0A;
    /* 0x0C */ be<s16> field_0x0C;
    /* 0x0E */ be<s16> field_0x0E;
    /* 0x10 */ be<s16> field_0x10;
    /* 0x12 */ be<s16> field_0x12;
    /* 0x14 */ be<s16> field_0x14;
    /* 0x16 */ be<s16> field_0x16;
    /* 0x18 */ be<s16> field_0x18;
    /* 0x1A */ u8 mPadding1A[2];
    /* 0x1C */ be<f32> field_0x1C;
    /* 0x20 */ be<f32> field_0x20;
    /* 0x24 */ be<f32> field_0x24;
    /* 0x28 */ be<s16> field_0x28;
    /* 0x2A */ u8 mPadding2A[2];
    /* 0x2C */ be<f32> field_0x2C;
    /* 0x30 */ be<f32> field_0x30;
    /* 0x34 */ be<f32> field_0x34;
    /* 0x38 */ be<f32> field_0x38;
    /* 0x3C */ be<f32> field_0x3C;
    /* 0x40 */ be<f32> field_0x40;
    /* 0x44 */ be<s16> field_0x44;
    /* 0x46 */ be<s16> field_0x46;
    /* 0x48 */ be<s16> field_0x48;
    /* 0x4A */ be<s16> field_0x4A;
    /* 0x4C */ be<s16> field_0x4C;
    /* 0x4E */ be<s16> field_0x4E;
    /* 0x50 */ be<s16> field_0x50;
    /* 0x52 */ be<u8> field_0x52;
    /* 0x53 */ be<u8> field_0x53;
};
WWHD_SIZE(NpcDatStruct, 0x54);

struct daNpcPhoto_c : fopNpc_npc_c {
    enum Prm_e {
        PRM_RAIL_ID_W = 0x8,
        PRM_RAIL_ID_S = 0x10,
        PRM_ARG0_W = 0x8,
        PRM_ARG0_S = 0x0,
    };

    request_of_phase_process_class* getPhaseP() { return &mPhs1; }

    cPhs_State _create();
    BOOL createHeap();
    cPhs_State createInit();
    bool _delete();
    bool _draw();
    bool _execute();
    u8 executeCommon(); /* bool, returned as loaded */
    void executeSetMode(u32); /* u8 */
    void executeWait();
    void executeTalk();
    void executeWalk();
    void executeTurn();
    void checkOrder();
    void eventOrder();
    void eventMove();
    void privateCut();
    void eventMesSetInit(int);
    bool eventMesSet();
    void eventSeSetInit(int);
    void eventPosSetInit();
    void eventGetItemInit();
    bool eventGetItem();
    void eventSetAngleInit();
    void eventSetEyeInit();
    bool eventSetEye();
    void eventTurnToPlayerInit();
    bool eventTurnToPlayer();
    void eventClrHanmeInit();
    void eventGetPhotoInit();
    bool eventGetPhoto();
    void eventMesSetUbInit(int);
    u32 eventMesSetUb(); /* bool; returns eventMesSet's register as is */
    bool eventLookUb();
    u16 talk2(int);
    void setMsgCamera();
    u16 next_msgStatus(be<u32>*);
    u32 getMsg();
    void setMessage(u32);
    void setAnmFromMsgTag();
    u8 getPrmRailID();
    u8 getPrmArg0();
    void setMtx();
    void chkAttention();
    void lookBack();
    BOOL initTexPatternAnm(u32, int); /* bool */
    void playTexPatternAnm();
    void playAnm();
    void setAnm(u32, int, f32); /* u8 */
    bool setAnmTbl(sPhotoAnmDat*);
    s16 XyCheckCB(int);
    s16 XyEventCB(int);
    BOOL isPhotoOk();
    BOOL isPhotoDxOk();
    void setCollision(dCcD_Cyl*, cXyz*, f32, f32); /* cXyz by value: pointer to a copy */

    /* 0x7DC */ request_of_phase_process_class mPhs1;
    /* 0x7E4 */ request_of_phase_process_class mPhs2;
    /* 0x7EC */ gptr<J3DAnmTexPattern> m_head_tex_pattern;
    /* 0x7F0 */ mDoExt_btpAnm mBtpAnm;          /* mDoExt_btpAnm (HD 0x74) */
    /* 0x864 */ dNpc_PathRun_c mPathRun;   // HD: no mShadowId before it
    /* 0x86C */ dCcD_Cyl field_0x6F8[2];
    /* 0xACC */ cXyz field_0x958;
    /* 0xAD8 */ cXyz mLookAtPos;
    /* 0xAE4 */ cXyz mEyePos;
    /* 0xAF0 */ gptr<sPhotoAnmDat> mpAnmDat;
    /* 0xAF4 */ gptr<be<u32>> field_0x980;
    /* 0xAF8 */ be<f32> field_0x984;
    /* 0xAFC */ be<f32> field_0x988;
    /* 0xB00 */ be<f32> field_0x98C;
    /* 0xB04 */ be<s32> mMsgNno;
    /* 0xB08 */ be<u8> field_0x994;
    /* 0xB09 */ u8 _B09;
    /* 0xB0A */ be<s16> mPhotoLinkBackEventIdx;
    /* 0xB0C */ be<s16> mPhotoGetItemEventIdx;
    /* 0xB0E */ be<s16> mPhotoGetItem2EventIdx;
    /* 0xB10 */ be<s16> mPhotoGetPhotoEventIdx;
    /* 0xB12 */ be<s16> mPhotoGalleryEventIdx;
    /* 0xB14 */ be<s16> mPhotoCounterTalk0EventIdx;
    /* 0xB16 */ be<s16> mPhotoCounterTalk1EventIdx;
    /* 0xB18 */ be<s16> mPhotoDateUB4EventIdx;
    /* 0xB1A */ be<s16> field_0x9A6;
    /* 0xB1C */ be<s16> field_0x9A8;
    /* 0xB1E */ u8 _B1E[2];
    /* 0xB20 */ be<s16> field_0x9AC;
    /* 0xB22 */ be<s16> field_0x9AE;
    /* 0xB24 */ be<s16> field_0x9B0;
    /* 0xB26 */ be<s16> field_0x9B2;
    /* 0xB28 */ be<s16> mTimer;
    /* 0xB2A */ be<s16> field_0x9B6;
    /* 0xB2C */ be<s16> field_0x9B8;
    /* 0xB2E */ be<s16> field_0x9BA;
    /* 0xB30 */ be<u8> field_0x9BC;
    /* 0xB31 */ be<u8> field_0x9BD;
    /* 0xB32 */ be<u8> field_0x9BE;
    /* 0xB33 */ be<u8> mFrame;
    /* 0xB34 */ be<u8> field_0x9C0;
    /* 0xB35 */ be<u8> field_0x9C1;
    /* 0xB36 */ be<u8> field_0x9C2;
    /* 0xB37 */ be<u8> field_0x9C3;
    /* 0xB38 */ be<u8> field_0x9C4;
    /* 0xB39 */ be<u8> mItemNo;
    /* 0xB3A */ be<u8> field_0x9C6;
    /* 0xB3B */ be<u8> field_0x9C7;
    /* 0xB3C */ be<u8> field_0x9C8;
    /* 0xB3D */ be<u8> field_0x9C9;
    /* 0xB3E */ be<s8> field_0x9CA;
    /* 0xB3F */ be<s8> mActIdx;
    /* 0xB40 */ be<u8> field_0x9CC;
    /* 0xB41 */ be<u8> field_0x9CD;
    /* 0xB42 */ u8 _B42[2];
    /* 0xB44 */ gptr<be<s8>> field_0x9D0;       /* u8* (camera index list) */
    /* 0xB48 */ be<u8> field_0x9D4;
    /* 0xB49 */ be<u8> field_0x9D5;
    /* 0xB4A */ be<s8> field_0x9D6;
    /* 0xB4B */ be<u8> field_0x9D7;
    /* 0xB4C */ be<u8> field_0x9D8;
    /* 0xB4D */ u8 _B4D;
    /* 0xB4E */ be<u8> mReplaceDateMessages;            /* HD only: zeroed by the constructor */
    /* 0xB4F */ u8 _B4F;
};
WWHD_OFFSET(daNpcPhoto_c, mPhs1, 0x7DC);
WWHD_OFFSET(daNpcPhoto_c, mBtpAnm, 0x7F0);
WWHD_OFFSET(daNpcPhoto_c, mPathRun, 0x864);
WWHD_OFFSET(daNpcPhoto_c, field_0x6F8, 0x86C);
WWHD_OFFSET(daNpcPhoto_c, field_0x958, 0xACC);
WWHD_OFFSET(daNpcPhoto_c, mMsgNno, 0xB04);
WWHD_OFFSET(daNpcPhoto_c, field_0x9AE, 0xB22);
WWHD_OFFSET(daNpcPhoto_c, mFrame, 0xB33);
WWHD_OFFSET(daNpcPhoto_c, field_0x9D0, 0xB44);
WWHD_OFFSET(daNpcPhoto_c, mReplaceDateMessages, 0xB4E);
WWHD_SIZE(daNpcPhoto_c, 0xB50);


// HD fields beyond the shared declarations, measured from the original accesses.
struct PhotoSelectedPicture {
    u8 mReserved[0x5BEB];
    be<u8> mSlot;
};
struct PhotoPlayerDemoAngle {
    u8 mReserved[0x422];
    be<s16> mDemoMoveAngle;
};
struct PhotoModelMaterialState {
    u8 mReserved[0x38];
    be<u32> mTexturePattern;
};
struct PhotoPictureSave {
    u8 mReserved[0x12C0];
    u8 mPictureData;
};
struct PhotoPictureAlbum {
    u8 mReserved[0x3C030C];
    be<u8> mPictureNum;
};
u32 photo_getPictureSave(u32);
u32 photo_getFreePictureSlot(u32);
void photo_markPictureSlot(u32, u32);
u32 photo_getPicture(u32, u32);
void photo_setPictureDay(u32, s32);
void photo_erasePicture(u32, u8);
u32 photo_eventMesSetRegister(daNpcPhoto_c*);
void photo_actorConstruct(fopNpc_npc_c*);
void photo_destroyArray(void*, u32, u32, u32, u32, u32);
void photo_actorDestroy(fopAc_ac_c*, s32);
s8 photo_eventGetActIdx(dEvent_manager_c*, s32, u32, s32, BOOL, s32);
u32 photo_runEventName(dEvent_manager_c*);
inline u32 photo_album() {
    return photo_getPictureSave(gabi::ea(&gabi::object_at<PhotoPictureSave>(dComIfGs_ea()).mPictureData));
}

/* ---- local bindings (SHARED-CANDIDATE; the same as in d_a_npc_people.h) ---- */
/* save info: event flags at *(0x101F84DC) + 0x644, temporary flags at + 0x1178 */


/* 02520C0C dComIfGs_checkGetItem(u8) */

// HD: dComIfGs_getPictureNum() is a byte of the picture save data (02720144 returns it)
static inline u8 photo_getPictureNum() { return gabi::object_at<PhotoPictureAlbum>(photo_album()).mPictureNum; }


static inline s8 photo_getMyActIdx(s32 staffId, u32 tbl, s32 n, BOOL force, s32 nameType) {
    return photo_eventGetActIdx(dComIfGp_getPEvtManager(), staffId, tbl, n, force, nameType);
}
/* resources by id (HD: sead::SafeString key, per-TU vtable) */

struct PhotoAnimationVtable {
    u8 mReserved[0x14];
    gfn getFrameMax;
};
struct PhotoAnimation {
    be<f32> mFrame;
    gptr<PhotoAnimationVtable> __vtbl;
};

static inline s32 J3DAnm_getFrameMax(void* p) {
    auto& animation = gabi::object_at<PhotoAnimation>(gabi::ea(p));
    return gabi::call_ptr<s32>(animation.__vtbl->getFrameMax.get(), p);
}

s32 mDoExt_btpAnm_init(void*, J3DModelData*, void*, s32, s32, f32, s32, s32, u32, s32);


void dNpc_JntCtrl_lookAtTarget(dNpc_JntCtrl_c*, be<s16>*, cXyz*, cXyz*, s16, s16, u8);
/* 0259D624 dNpc_calc_DisXZ_AngY(cXyz, cXyz (pointers to copies), f32* dist, s16* angle) */

/* dNpc_PathRun_c */


/* HD message manager (*(0x101F4B5C)) */
static inline u32 photo_msgManager() { return gabi::object_at<be<u32>>(0x101F4B5C); }
u32 msgMng_getStatus(u32);
void msgMng_setStatus(u32, u32);
u32 msgMng_messageSet(u32, u32, cXyz*);
/* J3DModelData (HD) */

s8 JUTNameTab_getIndex(u32, const char*);


/* eventInfo (fopAc_ac_c + 0xF8): mCommand u16 at 0xF8, mEventId s16 at 0xFC */
static inline u16 eventInfo_getCommand(fopAc_ac_c* a) { return a->eventInfo.mCommand; }
static inline void eventInfo_setEventId(fopAc_ac_c* a, s16 id) { a->eventInfo.mEventId = id; }
/* 025D9F38 fopAcM_searchFromName(name, param mask, param) */
fopAc_ac_c* fopAcM_searchFromName(const char*, u32, u32);
/* 02554288 dKy_get_dayofweek */
s32 dKy_get_dayofweek();
/* dEvt_control_c (play + 0x51D0): mPtTalk at +0xCC; 0253F124 getPId */


// HD: one TU owns these guest tables; all four source parts share their references.
extern be<s32> (&l_bck_ix_tbl)[5];
extern be<s32> (&l_btp_ix_tbl)[2];
extern sPhotoAnmDat (&l_npc_anm_wait)[1];
extern sPhotoAnmDat (&l_npc_anm_talk)[1];
extern sPhotoAnmDat (&l_npc_anm_walk)[1];
extern sPhotoAnmDat (&l_npc_anm_spit)[2];
extern sPhotoAnmDat (&l_npc_anm_talk2)[2];

extern be<u32> (&l_msg_1st_talk)[4];
extern be<u32> (&l_msg_2nd_talk)[2];
extern be<u32> (&l_msg_1st_photo)[3];
extern be<u32> (&l_msg_2nd_photo)[3];
extern be<u32> (&l_msg_1st_order)[11];
extern be<s16> (&l_msg_1st_order_c)[5];
extern be<u32> (&l_msg_1st_order_not_end)[3];
extern be<u16> (&l_msg_1st_order_not_end_c)[1];
extern be<u32> (&l_msg_2nd_order)[4];
extern be<u8> (&l_msg_2nd_order_c)[3];
extern be<u32> (&l_msg_2nd_order_not_end)[3];
extern be<u16> (&l_msg_2nd_order_not_end_c)[1];
extern be<u32> (&l_msg_3rd_order)[5];
extern be<u32> (&l_msg_3rd_order_c)[1];
extern be<u32> (&l_msg_3rd_order_not_end)[3];
extern be<u16> (&l_msg_3rd_order_not_end_c)[1];
extern be<u32> (&l_msg_color)[3];
extern be<u16> (&l_msg_color_c)[1];
extern be<u32> (&l_msg_week_1st)[6];
extern be<u32> (&l_msg_week)[7];
extern be<u32> (&l_msg_buy_photo)[2];
extern be<u32> (&l_msg_get_photo)[2];
extern be<u32> (&l_msg_down)[2];
extern be<u32> (&l_msg_ub4)[2];
extern be<u32> (&l_msg_1st_order_xy)[4];
extern be<u32> (&l_msg_2nd_order_xy)[4];
extern be<u32> (&l_msg_3rd_order_xy)[3];
extern be<u32> (&l_msg_color_xy)[4];
extern be<u32> (&l_msg_xy_buy_photo)[9];
extern cXyz (&l_counter_pos)[2];
extern cXyz (&l_gallery_pos)[2];
extern cXyz (&l_msg_camera)[6];

// Component access keeps sinit's original scalar store order.
inline be<f32>& photo_positionComponent(cXyz& position, int axis) {
    if (axis == 0) return position.x;
    if (axis == 1) return position.y;
    return position.z;
}

enum dRes_ID_PO {
    dRes_ID_PO_BCK_WAIT01_e = 0,
    dRes_ID_PO_BCK_SPIT_e = 4,
    dRes_ID_PO_BCK_TALK01_e = 5,
    dRes_ID_PO_BCK_TALK02_e = 6,
    dRes_ID_PO_BCK_WALK01_e = 7,
    dRes_ID_PO_BDL_PO_e = 1,
    dRes_ID_PO_BTP_MABA01_e = 2,
    dRes_ID_PO_BTP_MABA02_e = 3,
};
extern NpcDatStruct& l_npc_dat;
extern const gptr<char> (&l_arcname_tbl)[1];
extern const gptr<char>& l_npc_staff_id;

// HD out-of-line bindings retain the verified widths of this unit.
struct dSnap_Obj;
void dSnap_Obj_ct(dSnap_Obj*);
void dSnap_Obj_SetInf(dSnap_Obj*, u8, fopAc_ac_c*, u8, u8, s16);
void dSnap_Obj_SetGeo(dSnap_Obj*, cXyz*, f32, f32, s16);
void dSnap_RegistSnapObj(dSnap_Obj*);
cPhs_State dComLbG_PhaseHandler(request_of_phase_process_class*, u32, void*);
void __construct_array(void*, u32, u32, u32);
void __destroy_arr(void*, u32, u32, u32, u32);
void mDoAud_seStart_1(u32);
dPath* dPath_GetNextRoomPath(dPath*, s32);
u32 dPath_GetPnt(dPath*, s32);
u32 fopAcM_createItemForPresentDemo(cXyz*, s32, u8, s32, s32, csXyz*, cXyz*);
void cLib_addCalcPosXZ(cXyz*, const cXyz*, f32, f32, f32);
u32 dCam_getBody();
void dCamera_Stop(u32);
void dCamera_Start(u32);
void dCamera_Set(u32, cXyz*, cXyz*, f32, s16);

// HD: the joint returned by J3DNode_toJoint exposes its number at this offset.
struct PhotoJointNumber {
    be<u32> mData;
    be<u16> mNumber;
};
struct PhotoSafeStringVtable {
    u8 mReserved[0x14];
    gfn assureTermination;
};


struct PhotoMessageVtable {
    u8 mReserved[0x14];
    gfn nextMsgStatus;
    u8 mReserved18[4];
    gfn getMsg;
    u8 mReserved20[4];
    gfn setAnmAtr;
};

extern be<u32> (&l_msg_1st_talk_photo)[5];
extern be<u32> (&l_msg_talk_photo)[7];
extern be<u32> (&l_msg_2F)[3];

struct PsoData {
    /* 0x00 */ be<f32> field_0x00;
    /* 0x04 */ be<f32> field_0x04;
    /* 0x08 */ be<f32> field_0x08;
    /* 0x0C */ be<f32> field_0x0C;
    /* 0x10 */ be<f32> field_0x10;
    /* 0x14 */ be<s16> field_0x14;
    /* 0x16 */ be<u8> field_0x16;
    /* 0x17 */ be<u8> field_0x17;
};
WWHD_SIZE(PsoData, 0x18);
extern PsoData& l_pso_photo;
struct PhotoCylinderSource { be<u32> mWords[17]; };
extern PhotoCylinderSource& l_cyl_src2;
struct PhotoPhaseHandler { gfn handler; };
extern PhotoPhaseHandler (&l_method)[3];
void photo_cylElementDestroy(dCcD_Cyl*, s32);

// GameCube enum entries used here but not yet present in the shared subsets.
enum {
    fpcNm_PLAYER_e = 0xA8,
    dItemNo_PICTO_BOX_e = 0x23,
    dItemNo_DELUXE_PICTO_BOX_e = 0x26,
    dItemNo_FIREFLY_BOTTLE_e = 0x58,
    dItemNo_COLLECT_MAP_20_e = 0xEB,
    fopMsgStts_MSG_TYPING_e = 6,
    fopMsgStts_MSG_DISPLAYED_e = 0xE,
    fopMsgStts_MSG_DESTROYED_e = 0x13,
    DSNAP_TYPE_UNK05 = 5,
    DSNAP_TYPE_NPC_PHOTO = 0x5E,
    PHOTO_HD_ATTENTION = 0x01000000,
};
