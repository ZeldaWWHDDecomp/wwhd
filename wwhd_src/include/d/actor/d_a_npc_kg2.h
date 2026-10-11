/* daNpc_Kg2_c (Salvatore, cannon minigame on Windfall), WWHD layout.
 *
 * The GameCube decompilation has no layout for this class (all functions are Nonmatching
 * placeholders); the layout below is measured from the WWHD code of d_a_npc_kg2.
 * - HD: shared fopNpc_npc_c base is 0x7DC; actor size is 0x934.
 * - two mDoExt_btpAnm (HD 0x74 bytes, constructor 025E7820): the held item's and the face's
 * - GHS pointer to member mAction (8 bytes) at 0x910 */
#pragma once
#include "bindings.h"
#include "gc_api/tu.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/f_op/f_op_msg.h"

struct daNpc_Kg2_c : fopNpc_npc_c {
    BOOL event_wait_action(void* arg);
    BOOL wait_action(void* arg);
    void talk01();
    void wait01();
    void lookBack();
    void setAttention();
    void anmAtr(u16 atr);
    u32 getMsg();
    u16 next_msgStatus(be<u32>* msg);
    u8 chkAttention(cXyz* pos, s16 angle);
    BOOL _draw();
    BOOL _execute();
    void subAnm();
    void eventOrder();
    BOOL processMove();
    BOOL privateCut();
    BOOL evn_talk();
    BOOL evn_setAnm();
    BOOL evn_createItem_init(s32 staff);
    BOOL evn_talk_init(s32 staff);
    BOOL evn_jnt_lock_init(s32 staff);
    BOOL evn_setAnm_init(s32 staff);
    void checkOrder();
    void playTexPatternAnm();
    BOOL _delete();
    cPhs_State _create();
    BOOL CreateInit();
    void setAnm(s8 anm, f32 morf);
    void set_mtx();
    BOOL CreateHeap();
    BOOL initTexPatternAnm(u32 modify);
    /* 0x7DC */ be<s8> mItemJntNum; /* joint the held item hangs from */
    /* 0x7DD */ u8 _7DD[3];
    /* 0x7E0 */ request_of_phase_process_class mPhs;
    /* 0x7E8 */ gptr<J3DModel> mpItemModel;
    /* 0x7EC */ mDoExt_btpAnm mItemBtpAnm; /* mDoExt_btpAnm */
    /* 0x860 */ gptr<J3DAnmTexPattern> mpBtp;
    /* 0x864 */ mDoExt_btpAnm mBtpAnm; /* mDoExt_btpAnm */
    /* 0x8D8 */ be<u8> mBtpFrame;
    /* 0x8D9 */ u8 _8D9;
    /* 0x8DA */ be<s16> mBlinkTimer;
    /* 0x8DC */ cXyz mAttentionPos; /* head joint offset; copied to eyePos by setAttention */
    /* 0x8E8 */ cXyz mHeadPos;
    /* 0x8F4 */ be<s16> mLookSpeed;
    /* 0x8F6 */ u8 _8F6[2];
    /* 0x8F8 */ be<s16> mInitAngle[3];
    /* 0x8FE */ u8 _8FE[2];
    /* 0x900 */ be<u8> mbAttention;
    /* 0x901 */ be<u8> mbTalkRequest;
    /* 0x902 */ u8 _902[6];
    /* 0x908 */ be<u8> mbGreeted;
    /* 0x909 */ be<u8> mbTicketBought;
    /* 0x90A */ be<u8> mbItemShown;
    /* 0x90B */ be<u8> mItemBtpFrame;
    /* 0x90C */ be<u8> mbNight;
    /* 0x90D */ be<s8> mEvnLoopCount;
    /* 0x90E */ u8 _90E[2];
    /* 0x910 */ gabi::ptmf<void (daNpc_Kg2_c::*)(s32)> mAction;
    /* 0x918 */ be<s8> mBtpIdx;
    /* 0x919 */ be<s8> mAnmIdx;
    /* 0x91A */ u8 _91A[2];
    /* 0x91C */ be<u32> mNextMsgNo;
    /* 0x920 */ be<s8> mEventOrder;
    /* 0x921 */ u8 _921[3];
    /* 0x924 */ be<s32> mEventNo;
    /* 0x928 */ be<s16> mEventIds[4];
    /* 0x930 */ be<s8> mMode;
    /* 0x931 */ u8 _931[2];
    /* 0x933 */ be<s8> mActionState;
};
WWHD_OFFSET(daNpc_Kg2_c, mPhs, 0x7E0);
WWHD_OFFSET(daNpc_Kg2_c, mpBtp, 0x860);
WWHD_OFFSET(daNpc_Kg2_c, mBtpFrame, 0x8D8);
WWHD_OFFSET(daNpc_Kg2_c, mAction, 0x910);
WWHD_OFFSET(daNpc_Kg2_c, mEventIds, 0x928);
WWHD_OFFSET(daNpc_Kg2_c, mActionState, 0x933);

/* daNpc_Kg2_HIO_c (0x34 bytes, global at 0x10467628) */
struct daNpc_Kg2_HIO_c {
    /* 0x00 */ be<s8> mNo;
    /* 0x01 */ u8 _01[3];
    /* 0x04 */ be<f32> mEyeOffsetY;  /* overlays the start of a sub-object built by 0259DA18 */
    /* 0x08 */ be<s16> mJntParam[9]; /* dNpc_JntCtrl_c::setParam arguments */
    /* 0x1A */ be<s16> mLookSpeed;
    /* 0x1C */ be<f32> mEyeHeight;
    /* 0x20 */ be<s16> mAttentionAngle;
    /* 0x22 */ be<u8> mAttentionDebug;
    /* 0x23 */ u8 _23;
    /* 0x24 */ be<f32> mAttentionDist;
    /* 0x28 */ u8 _28[4];
    /* 0x2C */ be<u8> mDrawDebug;
    /* 0x2D */ u8 _2D[3];
    /* 0x30 */ be<u32> __vtbl;
};
WWHD_SIZE(daNpc_Kg2_HIO_c, 0x34);

WWHD_SIZE(daNpc_Kg2_c, 0x934);

struct Kg2JointResource {
    u8 pad[4];
    be<u16> mIndex;
};
struct Kg2ModelDataView {
    u8 pad[4];
    be<u32> mJointNum;
    be<u32> mJointArray;
    u8 padC[0x2C];
    be<u32> mTexPattern;
};
struct Kg2JointView {
    u8 pad[8];
    be<u32> mCallback;
    u8 tail[0x10];
};
struct Kg2ResourceHeader {
    u8 pad[0x10];
    be<u32> mNameOffset;
};
struct Kg2BtpView {
    u8 pad[4];
    be<u32> mVtable;
};
struct Kg2BtpVtable {
    u8 pad[0x14];
    be<u32> mGetFrameMax;
};
struct Kg2Words {
    be<u32> mWord[12];
};
struct Kg2Floats {
    be<f32> mValue[12];
};
struct Kg2DebugOffsets {
    cXyz mHead;
    be<u32> mHeadGuard;
};
struct Kg2HeaderStatics {
    be<u32> mInitial[2];
    be<u8> mAngle, mVector;
    u8 pad[2];
    be<u32> mZero[4];
};
struct Kg2InitialWords {
    be<u32> mFirst, mSecond;
};
struct Kg2Substance {
    be<u8> mByte[4];
};
inline Kg2Words* kg2Words(u32 a) { return gabi::at<Kg2Words>(a); }
inline Kg2Floats* kg2Floats(u32 a) { return gabi::at<Kg2Floats>(a); }
inline daNpc_Kg2_c* kg2Actor(u32 a) { return gabi::at<daNpc_Kg2_c>(a); }
inline daNpc_Kg2_HIO_c* kg2HioAt(u32 a) { return gabi::at<daNpc_Kg2_HIO_c>(a); }
inline J3DModel* kg2Model(u32 a) { return gabi::at<J3DModel>(a); }
inline mDoExt_McaMorf* kg2Morf(u32 a) { return gabi::at<mDoExt_McaMorf>(a); }
inline J3DMtxBuffer_hd* kg2Buffer(u32 a) { return gabi::at<J3DMtxBuffer_hd>(a); }
inline J3DFrameCtrl* kg2Frame(u32 a) { return gabi::at<J3DFrameCtrl>(a); }
inline Kg2ModelDataView* kg2Data(u32 a) { return gabi::at<Kg2ModelDataView>(a); }
inline Kg2JointView* kg2Joint(u32 a) { return gabi::at<Kg2JointView>(a); }
inline Kg2JointResource* kg2JointResource(u32 a) { return gabi::at<Kg2JointResource>(a); }
inline Kg2ResourceHeader* kg2Header(u32 a) { return gabi::at<Kg2ResourceHeader>(a); }
inline Kg2BtpView* kg2Btp(u32 a) { return gabi::at<Kg2BtpView>(a); }
inline Kg2BtpVtable* kg2BtpVtable(u32 a) { return gabi::at<Kg2BtpVtable>(a); }
inline dComIfG_inf_c* kg2Play(u32 a) { return gabi::at<dComIfG_inf_c>(a); }
inline dSv_info_c* kg2Save(u32 a) { return gabi::at<dSv_info_c>(a + 0x20); }
inline msg_class* kg2Message(u32 a) { return gabi::at<msg_class>(a); }
inline be<u32>& kg2Word(u32 a) { return *gabi::at<be<u32>>(a); }
inline be<f32>& kg2Float(u32 a) { return *gabi::at<be<f32>>(a); }
inline Kg2Substance* kg2Substance(u32 a) { return gabi::at<Kg2Substance>(a); }
inline daNpc_Kg2_HIO_c& kg2Hio() { return *gabi::at<daNpc_Kg2_HIO_c>(0x10467628); }
inline gptr<daNpc_Kg2_c>& kg2Pointer() { return *gabi::at<gptr<daNpc_Kg2_c>>(0x101D5F24); }
inline be<u8>& kg2CanonGameResult() { return *gabi::at<be<u8>>(0x101D5F42); }
inline be<u32>& kg2MessagePointer() { return *gabi::at<be<u32>>(0x101F4B5C); }
inline Kg2HeaderStatics& kg2HeaderStatics() { return *gabi::at<Kg2HeaderStatics>(0x1046760C); }
inline Kg2InitialWords& kg2InitialWords() { return *gabi::at<Kg2InitialWords>(0x1001C1E0); }
inline Kg2DebugOffsets& kg2DebugOffsets() { return *gabi::at<Kg2DebugOffsets>(0x1046765C); }
inline cXyz& kg2EyeDebug() { return *gabi::at<cXyz>(0x1047BBB0); }
inline Mtx34* kg2Matrix() { return gabi::at<Mtx34>(0x1048D0CC); }
inline void* kg2Opaque(u32 a) { return gabi::at<void>(a); }

// HD: these private bindings retain the verified argument and return widths.
u32 Kg2Core_envLight(void);
BOOL Kg2Core_checkPass(J3DFrameCtrl*, f32);
u8 Kg2Core_getEventReg(dSv_event_c*, u32);
BOOL Kg2Core_isEventBit(dSv_event_c*, u32);
void Kg2Core_onEventBit(dSv_event_c*, u32);
void* Kg2Core_getSubstance(dEvent_manager_c*, s32, const char*, s32);
u32 Kg2Core_allocate(s32);
void Kg2Core_initHio(void*);
u32 Kg2Core_jointResource(J3DNode*);
void Kg2Core_copyMatrix(Mtx34*, Mtx34*);
void Kg2Core_multVec(Mtx34*, cXyz*, cXyz*);
void Kg2Core_rotateX(Mtx34*, s16);
void Kg2Core_rotateZ(Mtx34*, s16);
void Kg2Core_assertion(const char*, s32, const char*);
BOOL Kg2Core_initBtp(mDoExt_btpAnm*, J3DModelData*, J3DAnmTexPattern*, s32, s32, f32, s32, s32, u32,
                     s32);
u32 Kg2Core_modelResource(J3DModelData*);
u32 Kg2Core_newMorf(s32, u32, s32, s32, u32, s32, s32, s32, f32, s32, s32, s32, u32);
s8 Kg2Core_jointIndex(void*, const char*);
u32 Kg2Core_newModel(J3DModelData*, u32, u32);
void Kg2Core_setWall(dBgS_AcchCir*, f32, f32);
void Kg2Core_setAcch(dBgS_ObjAcch*, cXyz*, cXyz*, daNpc_Kg2_c*, s32, dBgS_AcchCir*, cXyz*, s32,
                     s32);
void Kg2Core_translateMatrix(Mtx34*, f32, f32, f32);
void Kg2Core_rotateY(Mtx34*, s16);
void Kg2Core_calcMorf(mDoExt_McaMorf*);
void Kg2Core_translateMultiply(f32, f32, f32);
void Kg2Core_rotateXYZ(Mtx34*, s16, s16, s16);
void Kg2Core_setAnm(mDoExt_McaMorf*, s32, f32, f32, s32, s32, u32);
void Kg2Core_initStts(dCcD_Stts*, s32, s32, daNpc_Kg2_c*);
void Kg2Core_setCylinder(dCcD_Cyl*, const void*);
void Kg2Core_actorInfo(dNpc_EventCut_c*, const char*, daNpc_Kg2_c*);
s16 Kg2Core_eventIdx(dEvent_manager_c*, const char*, s32);
void Kg2Core_initNpc(daNpc_Kg2_c*);
void Kg2Core_initBtpObject(mDoExt_btpAnm*);
BOOL Kg2Core_entryHeap(daNpc_Kg2_c*, u32, u32);
s8 Kg2Core_createHioChild(const char*, daNpc_Kg2_HIO_c*);
void Kg2Core_stopAnime(mDoExt_McaMorf*);
void Kg2Core_deleteHioChild(s8);
s16 Kg2Core_timer(be<s16>*);
f32 Kg2Core_random(f32);
s32 Kg2Core_createItem(cXyz*, u32, s32, s32, s32, s32, s32);
s32 Kg2Core_staffId(dEvent_manager_c*, const char*, s32, s32);
s32 Kg2Core_actIdx(dEvent_manager_c*, s32, const void*, s32, s32, s32);
void Kg2Core_cutEnd(dEvent_manager_c*, s32);
BOOL Kg2Core_isAdvance(dEvent_manager_c*, s32);
void Kg2Core_orderSpeak(daNpc_Kg2_c*);
void Kg2Core_orderOther(daNpc_Kg2_c*, s16, s32, s32, s32, s32);
s32 Kg2Core_reverb(s32);
void Kg2Core_sound(u32, cXyz*, s32, s32);
void Kg2Core_setJointParam(dNpc_JntCtrl_c*, s16, s16, s16, s16, s16, s16, s16, s16, s16);
void Kg2Core_playMorf(mDoExt_McaMorf*, cXyz*, s32, s32);
void Kg2Core_posMove(daNpc_Kg2_c*, dCcD_Stts*);
void Kg2Core_correctPosition(dBgS_ObjAcch*, dBgS*);
u8 Kg2Core_roomId(dBgS*, cBgS_PolyInfo*);
u8 Kg2Core_polyColor(dBgS*, cBgS_PolyInfo*);
void Kg2Core_settingTev(u32, s32, cXyz*, dKy_tevstr_c*);
void Kg2Core_lightTev(u32, J3DModel*, dKy_tevstr_c*);
void Kg2Core_entryBtp(mDoExt_btpAnm*, J3DModelData*, u32);
void Kg2Core_entryDL(mDoExt_McaMorf*);
void Kg2Core_updateModel(J3DModel*, s32);
void Kg2Core_registerFigure(s32, daNpc_Kg2_c*, cXyz*, s16, f32, f32, f32);
f32 Kg2Core_squareRoot(f32);
s16 Kg2Core_atan2s(f32, f32);
s32 Kg2Core_days(void);
void Kg2Core_playerEye(cXyz*, f32);
void Kg2Core_approachAngle(be<s16>*, s16, s32, s32);
void Kg2Core_lookAt(dNpc_JntCtrl_c*, be<s16>*, cXyz*, cXyz*, s16, s16, s32);
BOOL Kg2Core_endCheck(dEvent_manager_c*, s16);
void Kg2Core_setEventReg(dSv_event_c*, u32, u8);
void Kg2Core_registerGlobal(void*);
void Kg2Core_initAngle(be<u8>*);
void Kg2Core_initVector(be<u8>*);
void Kg2Core_release(void*);
void Kg2Core_destroyCylinder(dCcD_Cyl*, s32);
void Kg2Core_destroyStts(dCcD_Stts*, s32);
void Kg2Core_destroyCircle(cM3dGCir_l*, s32);
void Kg2Core_destroyAcch(dBgS_ObjAcch*, s32);
void Kg2Core_destroyActor(daNpc_Kg2_c*, s32);

enum : u32 { Kg2GroupVtable = 0x1001BF94, Kg2PolyVtable = 0x1001BFA4 };

// HD: unit-private image data, distinct from other translation units.
enum : u32 {
    Kg2SafeStringVtable = 0x1001BF7C,
    Kg2HioVtable = 0x1001BFB4,
    Kg2ActorVtable = 0x1001C1E8,
    Kg2CylinderSource = 0x101BF14C,
    Kg2CutTable = 0x101BF244,
    Kg2HeaderRecord = 0x101BF258,
    Kg2AngleRecord = 0x101BF264,
    Kg2VectorRecord = 0x101BF270,
    Kg2AngleStatic = 0x10467614,
    Kg2VectorStatic = 0x10467615,
    Kg2CurrentMatrix = 0x104B4868,
};
inline J3DAnmTexPattern* kg2Pattern(u32 a) { return gabi::at<J3DAnmTexPattern>(a); }
inline const char* kg2String(u32 a) { return gabi::at<const char>(a); }
WWHD_SIZE(Kg2JointResource, 0x6);
WWHD_SIZE(Kg2ModelDataView, 0x3c);
WWHD_SIZE(Kg2JointView, 0x1c);
WWHD_SIZE(Kg2ResourceHeader, 0x14);
WWHD_SIZE(Kg2BtpView, 0x8);
WWHD_SIZE(Kg2BtpVtable, 0x18);
WWHD_SIZE(Kg2Words, 0x30);
WWHD_SIZE(Kg2Floats, 0x30);
WWHD_SIZE(Kg2DebugOffsets, 0x10);
WWHD_SIZE(Kg2HeaderStatics, 0x1c);
WWHD_SIZE(Kg2InitialWords, 0x8);
WWHD_SIZE(Kg2Substance, 0x4);
WWHD_OFFSET(Kg2JointResource, mIndex, 0x4);
WWHD_OFFSET(Kg2ModelDataView, mJointArray, 0x8);
WWHD_OFFSET(Kg2ModelDataView, mTexPattern, 0x38);
WWHD_OFFSET(Kg2JointView, mCallback, 0x8);
WWHD_OFFSET(Kg2ResourceHeader, mNameOffset, 0x10);
WWHD_OFFSET(Kg2BtpView, mVtable, 0x4);
WWHD_OFFSET(Kg2BtpVtable, mGetFrameMax, 0x14);
WWHD_OFFSET(Kg2DebugOffsets, mHeadGuard, 0xc);
WWHD_OFFSET(Kg2HeaderStatics, mZero, 0xc);

// HD: views for ABI-only guest pointers passed to the private bindings.
template <typename T> inline T* kg2Address(u32 address) { return gabi::at<T>(address); }

// HD: GHS embeds these direct PTMF targets, rather than a table.
enum : u32 { ACT_wait = 0x0226A474, ACT_event_wait = 0x0226A574 };

enum Kg2Resource { KG2_BDL_BODY = 5, KG2_BDL_ITEM = 6, KG2_BTP_ITEM = 10, KG2_BCK_WAIT = 26 };
enum Kg2Mode { KG2_MODE_WAIT = 1, KG2_MODE_TALK = 2 };
enum Kg2Event {
    KG2_EVENT_START = 0,
    KG2_EVENT_CLEAR = 1,
    KG2_EVENT_ITEM = 2,
    KG2_EVENT_GAME = 3,
    KG2_EVENT_NONE = 4
};
