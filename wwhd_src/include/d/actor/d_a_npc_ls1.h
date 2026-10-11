/* daNpc_Ls1_c (Aryll), WWHD layout. 
 *
 * GameCube -> WWHD, measured from the functions of d_a_npc_ls1: fopNpc_npc_c is 0x7DC. Members +0x118 up to mTelescopeScale/mpMatAnms/mArcName; mShadowID (GameCube
 * 0x6E4) is gone, so mpLsHandModel.. are +0x114; mDoExt_btkAnm and mDoExt_btpAnm grew from 0x14
 * to 0x74 each (+0x180 / +0x1E0 after them) and mCurrProcFunc is 8 bytes (GHS) instead of 12,
 * so everything from mHand_L_Mtx on is +0x1D0. Size 0xA28 (GameCube 0x858). */
#pragma once
#include "bindings.h"
#include "SSystem/SComponent/c_m2d_g_box.h"
#include "gc_api/tu.h"
#include "gc_api/f_op/f_op_actor.h"
#include "gc_api/d/d_com_inf_game.h"
#include "d/actor/d_a_player.h"
#include "d/d_camera.h"

struct daNpc_Ls1_c;
using Ls1ProcFunc = gabi::ptmf<BOOL (daNpc_Ls1_c::*)(void*)>;

/* daNpc_Ls1_matAnm_c : J3DMaterialAnm, HD size 0x80 (constructor 0227DFF0), vtable at +0x68 */
struct daNpc_Ls1_matAnm_c {
    /* 0x00 */ u8 _00[0x68];
    /* 0x68 */ be<u32> __vtbl;
    /* 0x6C */ cXy mOffset;
    /* 0x74 */ u8 m74[0x7C - 0x74];
    /* 0x7C */ be<u8> mbMove;
    /* 0x7D */ u8 m7D[3];
};
WWHD_SIZE(daNpc_Ls1_matAnm_c, 0x80);

struct daNpc_Ls1_c : fopNpc_npc_c {
    enum AnmNum {
        /* 0x00 */ ANMNUM_WAIT01_e,
        /* 0x01 */ ANMNUM_WAIT02_e,
        /* 0x02 */ ANMNUM_WAIT03_e,
        /* 0x03 */ ANMNUM_WAIT04_e,
        /* 0x04 */ ANMNUM_WAIT05_e,
        /* 0x05 */ ANMNUM_WAIT06_e,
        /* 0x06 */ ANMNUM_WATASU_e,
        /* 0x07 */ ANMNUM_FURIMUKI_e,
        /* 0x08 */ ANMNUM_NOZOKU_e,
        /* 0x09 */ ANMNUM_GET_e,
        /* 0x0A */ ANMNUM_WAIT07_e,
        /* 0x0B */ ANMNUM_TALK01_e,
        /* 0x0C */ ANMNUM_DEMOWAIT_e,
        /* 0x0D */ ANMNUM_DEMOLOOK_e,
        /* 0x0E */ ANMNUM_NULL_e,
    };

    enum BckNum {
        /* 0x00 */ BCKNUM_WAIT01_e,
        /* 0x01 */ BCKNUM_WAIT02_e,
        /* 0x02 */ BCKNUM_WAIT03_e,
        /* 0x03 */ BCKNUM_WAIT04_e,
        /* 0x04 */ BCKNUM_WAIT05_e,
        /* 0x05 */ BCKNUM_WAIT06_e,
        /* 0x06 */ BCKNUM_WATASU_e,
        /* 0x07 */ BCKNUM_FURIMUKI_e,
        /* 0x08 */ BCKNUM_NOZOKU_e,
        /* 0x09 */ BCKNUM_GET_e,
        /* 0x0A */ BCKNUM_WAIT07_e,
        /* 0x0B */ BCKNUM_TALK01_e,
        /* 0x0C */ BCKNUM_DEMOWAIT_e,
        /* 0x0D */ BCKNUM_DEMOLOOK_e,
        /* 0x0E */ BCKNUM_NULL_e,
    };

    enum BtpNum {
        /* 0x00 */ BTPNUM_FUAN_e,
        /* 0x01 */ BTPNUM_MABA_e,
        /* 0x02 */ BTPNUM_FUAN02_e,
        /* 0x03 */ BTPNUM_KIZUKU_e,
        /* 0x04 */ BTPNUM_GET_e,
        /* 0x05 */ BTPNUM_NGWARAI_e,
        /* 0x06 */ BTPNUM_NOZOKU_e,
        /* 0x07 */ BTPNUM_OKORI_e,
        /* 0x08 */ BTPNUM_WARAI_e,
        /* 0x09 */ BTPNUM_DEMOLOOK_e,
        /* 0x0A */ BTPNUM_DEMOWAIT_e,
        /* 0x0B */ BTPNUM_NULL_e,
    };

    enum BtkNum {
        /* 0x00 */ BTKNUM_DEFAULT_e,
        /* 0x01 */ BTKNUM_DEMOLOOK_e,
        /* 0x02 */ BTKNUM_DEMOWAIT_e,
        /* 0x03 */ BTKNUM_NULL_e,
    };


    struct anm_prm_c {
        /* 0x00 */ be<s8> bckNum;
        /* 0x01 */ be<s8> btpNum;
        /* 0x02 */ u8 _02[2];
        /* 0x04 */ be<f32> morf;
        /* 0x08 */ be<f32> speed;
        /* 0x0C */ be<s32> loopMode;
    };

    void _nodeCB_Head(J3DNode*, J3DModel*);
    void _nodeCB_BackBone(J3DNode*, J3DModel*);
    void _nodeCB_Hand_L(J3DNode*, J3DModel*);
    void _nodeCB_Hand_R(J3DNode*, J3DModel*);
    void _Ls_hand_nodeCB_Hand_L(J3DNode*, J3DModel*);
    void _Ls_hand_nodeCB_Hand_R(J3DNode*, J3DModel*);
    bool init_LS1_0();
    bool init_LS1_1();
    bool init_LS1_2();
    bool init_LS1_3();
    bool init_LS1_4();
    bool createInit();
    void play_animation();
    void setMtx(u32);
    int bckResID(int);
    int btpResID(int);
    int btkResID(int);
    u32 setBtp(s8, u32);
    void setMat();
    u32 setBtk(s8, u32);
    u32 init_texPttrnAnm(s8, u32);
    void play_btp_anm();
    void eye_ctrl();
    void play_btk_anm();
    void setAnm_anm(anm_prm_c*);
    void setAnm_NUM(int, BOOL);
    void setAnm();
    u32 chngAnmTag();
    void chngAnmAtr(u8);
    void ctrlAnmAtr();
    void setAnm_ATR(BOOL);
    void anmAtr(u16);
    u16 next_msgStatus(be<u32>*);
    u32 getMsg_LS1_0();
    u32 getMsg();
    void eventOrder();
    void checkOrder();
    u8 chk_talk();
    u8 chk_parts_notMov();
    fopAc_ac_c* searchByID(fpc_ProcID, be<s32>*);
    u8 partner_search_sub(u32);
    void partner_search();
    void setEyeCtrl();
    void clrEyeCtrl();
    void lookBack();
    u8 chkAttention();
    void setAttention(u32);
    u8 decideType(int);
    void cut_init_LOK_PLYER(int);
    void cut_init_PLYER_MOV(int);
    void cut_init_ANM_CHG(int);
    u32 cut_move_WAI();
    void privateCut(int);
    void endEvent();
    s32 isEventEntry();
    void event_proc(int);
    BOOL set_action(Ls1ProcFunc*, void*);
    void setStt(s8);
    u8 chk_areaIN(f32, f32, s16, cXyz*);
    void get_playerEvnPos(cXyz*, int);
    u8 chkTelescope_sph(cXyz*, f32, f32);
    u8 chkTelescope(cXyz*, f32, f32);
    u8 telescope_proc();
    BOOL wait_1();
    BOOL wait_2();
    BOOL wait_3();
    BOOL wait_4();
    BOOL talk_1();
    BOOL wait_action1(void*);
    BOOL demo_action1(void*);
    u8 demo();
    BOOL _draw();
    BOOL _execute();
    BOOL _delete();
    cPhs_State _create();
    BOOL bodyCreateHeap();
    BOOL handCreateHeap();
    BOOL itemCreateHeap();
    BOOL CreateHeap();

    /* 0x7DC */ request_of_phase_process_class mPhs;
    /* 0x7E4 */ be<s8> m_hed_jnt_num;
    /* 0x7E5 */ be<s8> m_bbone_jnt_num;
    /* 0x7E6 */ be<s8> m_hnd_L_jnt_num;
    /* 0x7E7 */ be<s8> m_hnd_R_jnt_num;
    /* 0x7E8 */ gptr<J3DModel> mpTelescopeModel;
    /* 0x7EC */ be<f32> mTelescopeScale;
    /* 0x7F0 */ gptr<daNpc_Ls1_matAnm_c> mpMatAnms[2];
    /* 0x7F8 */ char mArcName[3];
    /* 0x7FB */ u8 _7FB;
    /* 0x7FC */ gptr<J3DModel> mpLsHandModel;     /* HD: no mShadowID before it */
    /* 0x800 */ be<s8> m_lsHnd_L_jnt_num;
    /* 0x801 */ be<s8> m_lsHnd_R_jnt_num;
    /* 0x802 */ u8 _802[2];
    /* 0x804 */ mDoExt_btkAnm mBtkAnm;                 /* mDoExt_btkAnm (HD 0x74) */
    /* 0x878 */ be<u8> mBtkFrame;
    /* 0x879 */ u8 _879[3];
    /* 0x87C */ mDoExt_btpAnm mBtpAnm;                 /* mDoExt_btpAnm (HD 0x74) */
    /* 0x8F0 */ be<u8> mBtpFrame;
    /* 0x8F1 */ u8 _8F1;
    /* 0x8F2 */ be<s16> mTimer1;
    /* 0x8F4 */ Ls1ProcFunc mCurrProcFunc;
    /* 0x8FC */ Mtx34 mHand_L_Mtx;
    /* 0x92C */ Mtx34 mHand_R_Mtx;
    /* 0x95C */ be<u32> mBm1ProcID;
    /* 0x960 */ be<u32> m790;
    /* 0x964 */ cXyz m794;
    /* 0x970 */ csXyz m7A0;
    /* 0x976 */ csXyz mAngle;
    /* 0x97C */ u8 m7AC[8];
    /* 0x984 */ cXyz mTransformedEyePos;
    /* 0x990 */ cXyz mPlayerEyePos;
    /* 0x99C */ cXyz m7CC[2];
    /* 0x9B4 */ cXyz m7E4;
    /* 0x9C0 */ u8 m7F0[0x10];
    /* 0x9D0 */ be<f32> mPrevMorfFrame;
    /* 0x9D4 */ be<s16> mActorAngleY;
    /* 0x9D6 */ be<s16> mJointHeadY;
    /* 0x9D8 */ be<s16> mJointBackboneY;
    /* 0x9DA */ u8 m80A[2];
    /* 0x9DC */ be<s32> mbSetEyePos;
    /* 0x9E0 */ be<u32> m810;
    /* 0x9E4 */ be<s16> mEventIDTbl[4];
    /* 0x9EC */ be<s16> mEventIndex;
    /* 0x9EE */ be<s16> mTimer2;
    /* 0x9F0 */ be<u8> m820;                      /* HD: used by telescope_proc */
    /* 0x9F1 */ be<u8> m821;                      /* HD: used by telescope_proc */
    /* 0x9F2 */ be<s16> mTimer3;
    /* 0x9F4 */ be<s16> mTimer4;
    /* 0x9F6 */ be<s16> mTimer5;
    /* 0x9F8 */ u8 m828[6];
    /* 0x9FE */ be<s16> m82E;
    /* 0xA00 */ be<s8> mbMorfAnimStopped;
    /* 0xA01 */ be<u8> m831;
    /* 0xA02 */ be<u8> mItemNo;
    /* 0xA03 */ be<u8> m833;
    /* 0xA04 */ be<u8> m834;
    /* 0xA05 */ be<u8> m835;
    /* 0xA06 */ be<u8> m836;
    /* 0xA07 */ be<u8> mbResLoadIsComplete;
    /* 0xA08 */ be<u8> m838;
    /* 0xA09 */ be<u8> mbEyeCtrlSet;
    /* 0xA0A */ be<u8> m83A;
    /* 0xA0B */ be<u8> m83B;
    /* 0xA0C */ be<u8> m83C;
    /* 0xA0D */ be<u8> m83D;
    /* 0xA0E */ be<u8> mbAttention;
    /* 0xA0F */ be<u8> m83F;
    /* 0xA10 */ be<u8> m840;
    /* 0xA11 */ be<u8> m841;
    /* 0xA12 */ be<s16> mHalfHeadAngleY;
    /* 0xA14 */ be<s16> mHalfHeadAngleX;
    /* 0xA16 */ be<s16> m846;
    /* 0xA18 */ be<s16> m848;
    /* 0xA1A */ be<s8> mActionIndex;
    /* 0xA1B */ be<u8> m84B;
    /* 0xA1C */ be<u8> mMesgAnimeTag;
    /* 0xA1D */ be<s8> mBtpNum;
    /* 0xA1E */ be<s8> mBtkNum;
    /* 0xA1F */ be<s8> mBckNum;
    /* 0xA20 */ be<s8> m850;
    /* 0xA21 */ be<s8> m851;
    /* 0xA22 */ be<s8> m852;
    /* 0xA23 */ be<s8> m853;
    /* 0xA24 */ be<s8> m854;
    /* 0xA25 */ be<s8> mType;
    /* 0xA26 */ be<s8> m856;
    /* 0xA27 */ be<s8> m857;
};
WWHD_OFFSET(daNpc_Ls1_c, mpLsHandModel, 0x7FC);
WWHD_OFFSET(daNpc_Ls1_c, mBtkAnm, 0x804);
WWHD_OFFSET(daNpc_Ls1_c, mBtpAnm, 0x87C);
WWHD_OFFSET(daNpc_Ls1_c, mCurrProcFunc, 0x8F4);
WWHD_OFFSET(daNpc_Ls1_c, mHand_L_Mtx, 0x8FC);
WWHD_OFFSET(daNpc_Ls1_c, mTransformedEyePos, 0x984);
WWHD_OFFSET(daNpc_Ls1_c, mPrevMorfFrame, 0x9D0);
WWHD_OFFSET(daNpc_Ls1_c, mbEyeCtrlSet, 0xA09);
WWHD_OFFSET(daNpc_Ls1_c, m857, 0xA27);
WWHD_SIZE(daNpc_Ls1_c, 0xA28);

s16 ls1Guest_cLib_calcTimer(be<s16>*);
s32 ls1Guest_cLib_getRndValue(s32, s32);
s32 ls1Guest_mDoExt_btpAnm_init(void*, J3DModelData*, void*, s32, s32, f32, s32, s32, u32, s32);
s32 ls1Guest_mDoExt_btkAnm_init(void*, J3DModelData*, void*, s32, s32, f32, s32, s32, u32, s32);
bool ls1Guest_dAttention_LockonTruth(dAttention_c*);
fopAc_ac_c* ls1Guest_dAttention_LockonTarget(dAttention_c*, s32);
fopAc_ac_c* ls1Guest_dAttention_ActionTarget(dAttention_c*, s32);
s8 ls1Guest_dBgS_GetRoomId(dBgS*, void*);
u8 ls1Guest_dBgS_GetPolyColor(dBgS*, void*);
u32 ls1Guest_dBgS_GetMtrlSndId(dBgS*, void*);
void ls1Guest_playerEyePosition(cXyz*, f32);
s8 ls1Guest_JUTNameTab_getIndex(u32, const char*);
f32 ls1Guest_cLib_addCalc(be<f32>*, f32, f32, f32, f32);
void ls1Guest_mDoMtx_XYZrotM(Mtx34*, s16, s16, s16);
void ls1Guest_mDoLib_project(cXyz*, cXyz*);
void ls1Guest_fopMsgM_getScopeMode(u32);
BOOL ls1Guest_fopMsgM_releaseScopeMode(u32);
void ls1Guest_fopMsgM_scopeMessageSet(u32, u32);
void ls1Guest_dNpc_JntCtrl_lookAtTarget_2(dNpc_JntCtrl_c*, be<s16>*, cXyz*, cXyz*, s16, s16, u8);

struct daNpc_Ls1_HIO_c {
    struct hio_prm_c {
        /* 0x00 */ be<s16> mMaxHeadX;
        /* 0x02 */ be<s16> mMaxHeadY;
        /* 0x04 */ be<s16> mMinHeadX;
        /* 0x06 */ be<s16> mMinHeadY;
        /* 0x08 */ be<s16> mMaxBackBoneX;
        /* 0x0A */ be<s16> mMaxBackBoneY;
        /* 0x0C */ be<s16> mMinBackBoneX;
        /* 0x0E */ be<s16> mMinBackBoneY;
        /* 0x10 */ be<s16> mMaxTurnStep;
        /* 0x12 */ be<s16> m12;
        /* 0x14 */ be<f32> mAttPosOffsetY;
        /* 0x18 */ be<u8> m18;
        /* 0x19 */ u8 _19[3];
        /* 0x1C */ be<f32> m1C;
        /* 0x20 */ be<f32> m29;
        /* 0x24 */ be<f32> m24;
        /* 0x28 */ be<f32> m28;
        /* 0x2C */ be<f32> m2C;
        /* 0x30 */ be<s16> m30;
        /* 0x32 */ u8 _32[2];
        /* 0x34 */ be<f32> mPlayerEyePosOffsetY;
    };
    /* 0x00 */ be<u32> __vtbl; /* HD: vtable first */
    /* 0x04 */ be<s8> m04;
    /* 0x05 */ u8 _05[3];
    /* 0x08 */ be<s32> m08;
    /* 0x0C */ hio_prm_c mPrm;
};
WWHD_SIZE(daNpc_Ls1_HIO_c, 0x44);

struct Ls1Word { be<u32> mValue; };
struct Ls1SignedWord { be<s32> mValue; };
struct Ls1ActorSearch { gptr<fopAc_ac_c> mActors[20]; };
struct Ls1BtpResources { be<s32> mResources[11]; };
struct Ls1BtkResources { be<s32> mResources[3]; };
struct Ls1BckResources { be<s32> mResources[14]; };
struct Ls1BtkMapping { be<s8> mAnimations[11]; };
struct Ls1MessageBtpMapping { be<u8> mAnimations[3]; };
struct Ls1EventNames { gptr<const char> mNames[4]; };
struct Ls1EyeMaterialNames { SafeString mNames[2]; };
struct Ls1Animations { daNpc_Ls1_c::anm_prm_c mAnimations[14]; };
struct Ls1StateAnimations { daNpc_Ls1_c::anm_prm_c mAnimations[6]; };
struct Ls1AttributeAnimations { daNpc_Ls1_c::anm_prm_c mAnimations[17]; };
struct Ls1EventPositions { cXyz mPositions[2]; };
struct Ls1HeapSizes { be<u32> mSizes[2]; };
// HD: this actor reads the texture-SRT animation pointer at +0x68 of its work object.
struct Ls1BtkWork { u8 padding[0x68]; gptr<void> mpAnm; };
WWHD_OFFSET(Ls1BtkWork, mpAnm, 0x68);
struct Ls1JointNumber { u8 padding[4]; be<u16> mJointNo; };
WWHD_OFFSET(Ls1JointNumber, mJointNo, 4);
struct Ls1AnimationVtable { u8 padding[0x14]; gfn mFrameMax; };
struct Ls1Animation { be<f32> mFrame; gptr<Ls1AnimationVtable> mVtable; };
struct Ls1TuData {
    u32 mActorVtable, mMaterialVtable, mHioVtable;
};
extern const Ls1TuData ls1_tu;

void ls1Guest_node_head(daNpc_Ls1_c*, J3DNode*, J3DModel*);
void ls1Guest_node_backbone(daNpc_Ls1_c*, J3DNode*, J3DModel*);
void ls1Guest_node_leftHand(daNpc_Ls1_c*, J3DNode*, J3DModel*);
void ls1Guest_node_rightHand(daNpc_Ls1_c*, J3DNode*, J3DModel*);
void ls1Guest_node_leftHandModel(daNpc_Ls1_c*, J3DNode*, J3DModel*);
void ls1Guest_node_rightHandModel(daNpc_Ls1_c*, J3DNode*, J3DModel*);

void ls1Guest_destroyCircle(void*, s32);
BOOL ls1Guest_normalize(cXyz*);
daNpc_Ls1_matAnm_c* ls1Guest_constructMaterialAnimation(daNpc_Ls1_matAnm_c*);
u32 ls1Guest_initWaitRegister(daNpc_Ls1_c*);
u32 ls1Guest_initDemoRegister(daNpc_Ls1_c*);
bool ls1Guest_initType2(daNpc_Ls1_c*);
bool ls1Guest_initType3(daNpc_Ls1_c*);
bool ls1Guest_initType4(daNpc_Ls1_c*);
void ls1Guest_destroyAcch(dBgS_ObjAcch*, s32);
u32 ls1Guest_groundMaterialSound(cXyz*, f32);
void ls1Guest_cameraStop(u32);
void ls1Guest_cameraStart(u32);
void ls1Guest_subjectLockOn(u32, fopAc_ac_c*);
void ls1Guest_subjectLockOff(u32);
BOOL ls1Guest_checkGetItem(u32);
void* ls1Guest_demoActor(u32, u32);
void ls1Guest_setDemoData(daNpc_Ls1_c*, u32, mDoExt_McaMorf*, const char*, s32, u32, u32, s32);
void* ls1Guest_demoBtpData(void*, const char*);
void* ls1Guest_demoBtkData(void*, const char*);
s8 ls1Guest_eventActionIndex(void*, s32, u32, s32, BOOL, s32);
void ls1Guest_constructNpc(daNpc_Ls1_c*);
void ls1Guest_destroyActor(daNpc_Ls1_c*, s32);
BOOL ls1Guest_searchActorByID(fpc_ProcID, gptr<fopAc_ac_c>*);
void ls1Guest_bgmStart(u32);
void ls1Guest_bgmStop(s32);
void ls1Guest_bgmStreamPrepare(u32);
void ls1Guest_bgmStreamPlay();
void ls1Guest_constructBtp(mDoExt_btpAnm*);
void ls1Guest_constructBtk(mDoExt_btkAnm*);
u32 ls1Guest_messageStatus(u32);
s32 ls1Guest_materialIndex(u32, const char*);
u32 ls1Guest_textureName(u32, u32);

// HD: eye materials carry a SafeString name and a material-animation pointer.
struct Ls1SafeStringVtable { u8 padding[0x14]; gfn mAssureTermination; };
struct Ls1ResourceHeader { u8 padding[0x18]; be<u32> mMaterialNamesOffset; };
struct Ls1Material { u8 padding[0x24]; gptr<daNpc_Ls1_matAnm_c> mMaterialAnimation; };
WWHD_OFFSET(Ls1Material, mMaterialAnimation, 0x24);
WWHD_OFFSET(Ls1ResourceHeader, mMaterialNamesOffset, 0x18);

// HD: material resources use offsets relative to each offset word.
struct Ls1RelativeOffset {
    be<u32> mOffset;
    u32 address() { u32 offset = mOffset; return offset ? gabi::ea(this) + offset : 0; }
};
struct Ls1TextureEntry {
    u8 padding[2]; be<u16> mSrtOffset; be<s32> mAnimationIndex;
    u8 padding8[4]; be<u16> mLinkedEntry; u8 paddingE[6];
};
struct Ls1TextureResource {
    u8 padding[0x34]; Ls1RelativeOffset mEntries; Ls1RelativeOffset mNames;
};
struct Ls1TextureSrt { u8 padding[0x10]; be<f32> mTranslationX, mTranslationY; };
struct Ls1MaterialTexture {
    gptr<Ls1TextureResource> mResource; be<u16> mFlags; u8 padding6[6];
    gptr<be<u32>> mChangedEntries; u8 padding10[0x18]; gptr<u8> mSrtData;
};
WWHD_SIZE(Ls1TextureEntry, 0x14);
WWHD_OFFSET(Ls1MaterialTexture, mChangedEntries, 0xC);
WWHD_OFFSET(Ls1MaterialTexture, mSrtData, 0x28);
WWHD_OFFSET(Ls1TextureResource, mEntries, 0x34);
struct Ls1DemoActor { u8 padding[4]; be<u16> mFlags; u8 padding6[0x22]; be<s32> mShapeId; };
WWHD_OFFSET(Ls1DemoActor, mShapeId, 0x28);

// HD: camera state follows the outer camera actor at +0x248.
struct Ls1Camera { u8 padding[0x248]; dCamera_c mCamera; };
struct Ls1CameraInfo { u8 padding[0xC]; be<f32> mZoomScale; };
struct Ls1ScopeState { be<u8> mScopeType; u8 padding[0x51]; be<u8> mScopeWipeFlag; };
struct Ls1MessageManager { u8 padding[0x928]; be<u8> mForceSend; };
struct Ls1Bm1 { u8 padding[0x9F0]; be<u8> mOdoroki, mTelescopeDemo, mFocus; };
struct Ls1View { u8 padding[0xD4]; be<f32> mFovy; };
WWHD_OFFSET(Ls1Camera, mCamera, 0x248);
WWHD_OFFSET(Ls1CameraInfo, mZoomScale, 0xC);
WWHD_OFFSET(Ls1ScopeState, mScopeWipeFlag, 0x52);
WWHD_OFFSET(Ls1MessageManager, mForceSend, 0x928);
WWHD_OFFSET(Ls1Bm1, mTelescopeDemo, 0x9F1);

struct Ls1ModelDataTail { u8 padding[0x30]; be<u32> mTexture, mTextureNames, mBtpAnimation; u8 padding3C[8]; be<u32> mBtkAnimation; };
struct Ls1DeletingVtable { u8 padding[0xC]; gfn mDestructor; };
struct Ls1DeletingObject { gptr<Ls1DeletingVtable> mVtable; };
struct Ls1BtkMaterialInfo { u8 padding[0x14]; be<u16> mMaterialNum; };
struct Ls1BtkAnimation { u8 padding[0xC]; gptr<Ls1BtkMaterialInfo> mMaterialInfo; };
struct Ls1EventFrame { u8 padding[8]; cXyz mResult; cXyz mPositions[2]; u8 padding2C[0x14]; };
WWHD_SIZE(Ls1EventFrame, 0x40);
WWHD_OFFSET(Ls1ModelDataTail, mBtkAnimation, 0x44);

// HD: ResTIMG includes a ninth word, preserved by setResTIMG.
struct Ls1TextureImage {
    be<u32> mHeader[3]; be<u32> mPaletteOffset; be<u32> mSettings[3];
    be<u32> mImageOffset; be<u32> mHdWord;
};
struct Ls1Texture {
    be<u16> mCount; u8 padding2[2]; gptr<Ls1TextureImage> mImages;
    be<u32> mChangedMask[4]; be<u32> mBaseMaskHigh, mBaseMaskLow;
};
WWHD_SIZE(Ls1TextureImage, 0x24);
WWHD_OFFSET(Ls1TextureImage, mImageOffset, 0x1C);
WWHD_OFFSET(Ls1Texture, mChangedMask, 8);

struct Ls1GameInfo { u8 padding[0x5FA4]; gptr<Ls1View> mpDrawlistView; };
WWHD_OFFSET(Ls1GameInfo, mpDrawlistView, 0x5FA4);
struct Ls1BodyTextureName { be<u8> mName[9]; };
enum Ls1ResourceId { dRes_ID_LS_BDL_LSHAND_e = 0xC, dRes_ID_LS_BDL_LS_e = 0xD, dRes_ID_LS_BTI_LSBODY02_e = 0x1D };
enum Ls1LinkResourceIndex { dRes_INDEX_LINK_BDL_TELESCOPE_e = 0x2F };

// Layout checks for the HD views and resource tables above.
WWHD_SIZE(Ls1Word, 0x4);
WWHD_SIZE(Ls1SignedWord, 0x4);
WWHD_SIZE(Ls1ActorSearch, 0x50);
WWHD_SIZE(Ls1BtpResources, 0x2C);
WWHD_SIZE(Ls1BtkResources, 0xC);
WWHD_SIZE(Ls1BckResources, 0x38);
WWHD_SIZE(Ls1BtkMapping, 0xB);
WWHD_SIZE(Ls1MessageBtpMapping, 0x3);
WWHD_SIZE(Ls1EventNames, 0x10);
WWHD_SIZE(Ls1EyeMaterialNames, 0x10);
WWHD_SIZE(Ls1Animations, 0xE0);
WWHD_SIZE(Ls1StateAnimations, 0x60);
WWHD_SIZE(Ls1AttributeAnimations, 0x110);
WWHD_SIZE(Ls1EventPositions, 0x18);
WWHD_SIZE(Ls1HeapSizes, 0x8);
WWHD_SIZE(Ls1BtkWork, 0x6C);
WWHD_SIZE(Ls1JointNumber, 0x6);
WWHD_SIZE(Ls1AnimationVtable, 0x18);
WWHD_SIZE(Ls1Animation, 0x8);
WWHD_SIZE(Ls1SafeStringVtable, 0x18);
WWHD_SIZE(Ls1ResourceHeader, 0x1C);
WWHD_SIZE(Ls1Material, 0x28);
WWHD_SIZE(Ls1RelativeOffset, 0x4);
WWHD_SIZE(Ls1TextureResource, 0x3C);
WWHD_SIZE(Ls1TextureSrt, 0x18);
WWHD_SIZE(Ls1MaterialTexture, 0x2C);
WWHD_SIZE(Ls1DemoActor, 0x2C);
WWHD_SIZE(Ls1CameraInfo, 0x10);
WWHD_SIZE(Ls1ScopeState, 0x53);
WWHD_SIZE(Ls1MessageManager, 0x929);
WWHD_SIZE(Ls1Bm1, 0x9F3);
WWHD_SIZE(Ls1View, 0xD8);
WWHD_SIZE(Ls1ModelDataTail, 0x48);
WWHD_SIZE(Ls1DeletingVtable, 0x10);
WWHD_SIZE(Ls1DeletingObject, 0x4);
WWHD_SIZE(Ls1BtkMaterialInfo, 0x16);
WWHD_SIZE(Ls1BtkAnimation, 0x10);
WWHD_SIZE(Ls1BodyTextureName, 0x9);
