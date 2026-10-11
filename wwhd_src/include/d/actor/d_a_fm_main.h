/* Floormaster main: private layouts for the verified HD fields.
 * HIO is 0x15C (GC 0x158); its JointHit HIO subobject grows by four bytes.
 * J3D model/node, player virtuals and particles use the HD layouts asserted below.
 * The actor layout shared by all four Floormaster parts stays in d_a_fm.h. */
#pragma once
#include "d/actor/d_a_fm.h"
#include "gc_api/f_op/f_op_actor_mng.h"
#include "gc_api/d/d_com_inf_game.h"
namespace fm_main {
struct HIO {
    be<u32> __vtbl;
    /* 0x004 */ be<s8> field_0x004;
    /* 0x005 */ be<u8> field_0x005;
    /* 0x006 */ be<u8> field_0x006;
    /* 0x007 */ be<u8> field_0x007;
    /* 0x008 */ be<u8> field_0x008;
    /* 0x009 */ be<u8> field_0x009;
    /* 0x00A */ be<u8> field_0x00A;
    /* 0x00B */ be<u8> field_0x00B;
    /* 0x00C */ be<u8> field_0x00C;
    /* 0x00D */ be<u8> field_0x00D;
    /* 0x00E */ be<u8> field_0x00E;
    /* 0x00F */ be<u8> field_0x00F[6];
    /* 0x015 */ u8 field_0x015[0x01F - 0x015];
    /* 0x01F */ be<u8> field_0x01F;
    /* 0x020 */ cXyz field_0x020;
    /* 0x02C */ cXyz field_0x02C;
    /* 0x038 */ cXyz field_0x038;
    /* 0x044 */ cXyz field_0x044;
    /* 0x050 */ cXyz field_0x050;
    /* 0x05C */ csXyz field_0x05C;
    /* 0x062 */ be<s16> mFreezeDuration;
    /* 0x064 */ be<s16> mMaxHealth;
    /* 0x066 */ be<s16> field_0x066;
    /* 0x068 */ be<s16> field_0x068;
    /* 0x06A */ be<s16> field_0x06A;
    /* 0x06C */ be<f32> mSpAttackVJumpHeight;
    /* 0x070 */ be<f32> mSpAttackVJumpRadius;
    /* 0x074 */ be<s16> mSpAttackVJumpStartFrame;
    /* 0x076 */ be<s16> mSpAttackVJumpEndFrame;
    /* 0x078 */ be<f32> mSpAttackJumpHeight;
    /* 0x07C */ be<f32> mSpAttackJumpRadius;
    /* 0x080 */ be<s16> mSpAttackJumpStartFrame;
    /* 0x082 */ be<s16> mSpAttackJumpEndFrame;
    /* 0x084 */ be<s16> field_0x084;
    /* 0x086 */ be<s16> field_0x086;
    /* 0x088 */ be<s16> field_0x088;
    /* 0x08A */ be<s16> field_0x08A;
    /* 0x08C */ be<s16> field_0x08C;
    /* 0x08E */ be<s16> field_0x08E;
    /* 0x090 */ be<s16> field_0x090;
    /* 0x092 */ be<s16> field_0x092;
    /* 0x094 */ be<s16> field_0x094;
    /* 0x096 */ be<s16> field_0x096;
    /* 0x098 */ be<s16> field_0x098;
    /* 0x09A */ be<s16> field_0x09A;
    /* 0x09C */ be<s16> field_0x09C;
    /* 0x09E */ be<s16> field_0x09E;
    /* 0x0A0 */ be<s16> field_0x0A0;
    /* 0x0A2 */ be<s16> field_0x0A2;
    /* 0x0A4 */ be<s16> field_0x0A4;
    /* 0x0A6 */ be<s16> field_0x0A6;
    /* 0x0A8 */ be<f32> field_0x0A8;
    /* 0x0AC */ be<f32> field_0x0AC;
    /* 0x0B0 */ be<f32> field_0x0B0;
    /* 0x0B4 */ be<f32> field_0x0B4;
    /* 0x0B8 */ be<f32> field_0x0B8;
    /* 0x0BC */ be<f32> field_0x0BC;
    /* 0x0C0 */ be<f32> field_0x0C0;
    /* 0x0C4 */ be<f32> field_0x0C4;
    /* 0x0C8 */ be<f32> field_0x0C8;
    /* 0x0CC */ be<f32> field_0x0CC;
    /* 0x0D0 */ be<f32> field_0x0D0;
    /* 0x0D4 */ be<f32> field_0x0D4;
    /* 0x0D8 */ be<f32> field_0x0D8;
    /* 0x0DC */ be<f32> field_0x0DC;
    /* 0x0E0 */ be<f32> field_0x0E0;
    /* 0x0E4 */ be<f32> field_0x0E4;
    /* 0x0E8 */ be<f32> field_0x0E8;
    /* 0x0EC */ be<f32> field_0x0EC;
    /* 0x0F0 */ be<f32> field_0x0F0;
    /* 0x0F4 */ be<f32> field_0x0F4;
    /* 0x0F8 */ be<f32> field_0x0F8;
    /* 0x0FC */ be<f32> field_0x0FC;
    /* 0x100 */ be<s16> field_0x100;
    u8 mPadding102[2];
    /* 0x104 */ cXyz field_0x104;
    /* 0x110 */ be<f32> field_0x110;
    /* 0x114 */ be<f32> field_0x114;
    /* 0x118 */ be<f32> field_0x118;
    /* 0x11C */ be<s16> field_0x11C;
    /* 0x11E */ be<s16> field_0x11E;
    /* 0x120 */ be<f32> field_0x120;
    /* 0x124 */ be<f32> field_0x124;
    /* 0x128 */ be<f32> field_0x128;
    /* 0x12C */ be<f32> field_0x12C;
    /* 0x130 */ u8 field_0x130[0x2C];};
WWHD_OFFSET(HIO, field_0x004, 0x4);
WWHD_OFFSET(HIO, field_0x005, 0x5);
WWHD_OFFSET(HIO, field_0x006, 0x6);
WWHD_OFFSET(HIO, field_0x007, 0x7);
WWHD_OFFSET(HIO, field_0x008, 0x8);
WWHD_OFFSET(HIO, field_0x009, 0x9);
WWHD_OFFSET(HIO, field_0x00A, 0xA);
WWHD_OFFSET(HIO, field_0x00B, 0xB);
WWHD_OFFSET(HIO, field_0x00C, 0xC);
WWHD_OFFSET(HIO, field_0x00D, 0xD);
WWHD_OFFSET(HIO, field_0x00E, 0xE);
WWHD_OFFSET(HIO, field_0x00F, 0xF);
WWHD_OFFSET(HIO, field_0x015, 0x15);
WWHD_OFFSET(HIO, field_0x01F, 0x1F);
WWHD_OFFSET(HIO, field_0x020, 0x20);
WWHD_OFFSET(HIO, field_0x02C, 0x2C);
WWHD_OFFSET(HIO, field_0x038, 0x38);
WWHD_OFFSET(HIO, field_0x044, 0x44);
WWHD_OFFSET(HIO, field_0x050, 0x50);
WWHD_OFFSET(HIO, field_0x05C, 0x5C);
WWHD_OFFSET(HIO, mFreezeDuration, 0x62);
WWHD_OFFSET(HIO, mMaxHealth, 0x64);
WWHD_OFFSET(HIO, field_0x066, 0x66);
WWHD_OFFSET(HIO, field_0x068, 0x68);
WWHD_OFFSET(HIO, field_0x06A, 0x6A);
WWHD_OFFSET(HIO, mSpAttackVJumpHeight, 0x6C);
WWHD_OFFSET(HIO, mSpAttackVJumpRadius, 0x70);
WWHD_OFFSET(HIO, mSpAttackVJumpStartFrame, 0x74);
WWHD_OFFSET(HIO, mSpAttackVJumpEndFrame, 0x76);
WWHD_OFFSET(HIO, mSpAttackJumpHeight, 0x78);
WWHD_OFFSET(HIO, mSpAttackJumpRadius, 0x7C);
WWHD_OFFSET(HIO, mSpAttackJumpStartFrame, 0x80);
WWHD_OFFSET(HIO, mSpAttackJumpEndFrame, 0x82);
WWHD_OFFSET(HIO, field_0x084, 0x84);
WWHD_OFFSET(HIO, field_0x086, 0x86);
WWHD_OFFSET(HIO, field_0x088, 0x88);
WWHD_OFFSET(HIO, field_0x08A, 0x8A);
WWHD_OFFSET(HIO, field_0x08C, 0x8C);
WWHD_OFFSET(HIO, field_0x08E, 0x8E);
WWHD_OFFSET(HIO, field_0x090, 0x90);
WWHD_OFFSET(HIO, field_0x092, 0x92);
WWHD_OFFSET(HIO, field_0x094, 0x94);
WWHD_OFFSET(HIO, field_0x096, 0x96);
WWHD_OFFSET(HIO, field_0x098, 0x98);
WWHD_OFFSET(HIO, field_0x09A, 0x9A);
WWHD_OFFSET(HIO, field_0x09C, 0x9C);
WWHD_OFFSET(HIO, field_0x09E, 0x9E);
WWHD_OFFSET(HIO, field_0x0A0, 0xA0);
WWHD_OFFSET(HIO, field_0x0A2, 0xA2);
WWHD_OFFSET(HIO, field_0x0A4, 0xA4);
WWHD_OFFSET(HIO, field_0x0A6, 0xA6);
WWHD_OFFSET(HIO, field_0x0A8, 0xA8);
WWHD_OFFSET(HIO, field_0x0AC, 0xAC);
WWHD_OFFSET(HIO, field_0x0B0, 0xB0);
WWHD_OFFSET(HIO, field_0x0B4, 0xB4);
WWHD_OFFSET(HIO, field_0x0B8, 0xB8);
WWHD_OFFSET(HIO, field_0x0BC, 0xBC);
WWHD_OFFSET(HIO, field_0x0C0, 0xC0);
WWHD_OFFSET(HIO, field_0x0C4, 0xC4);
WWHD_OFFSET(HIO, field_0x0C8, 0xC8);
WWHD_OFFSET(HIO, field_0x0CC, 0xCC);
WWHD_OFFSET(HIO, field_0x0D0, 0xD0);
WWHD_OFFSET(HIO, field_0x0D4, 0xD4);
WWHD_OFFSET(HIO, field_0x0D8, 0xD8);
WWHD_OFFSET(HIO, field_0x0DC, 0xDC);
WWHD_OFFSET(HIO, field_0x0E0, 0xE0);
WWHD_OFFSET(HIO, field_0x0E4, 0xE4);
WWHD_OFFSET(HIO, field_0x0E8, 0xE8);
WWHD_OFFSET(HIO, field_0x0EC, 0xEC);
WWHD_OFFSET(HIO, field_0x0F0, 0xF0);
WWHD_OFFSET(HIO, field_0x0F4, 0xF4);
WWHD_OFFSET(HIO, field_0x0F8, 0xF8);
WWHD_OFFSET(HIO, field_0x0FC, 0xFC);
WWHD_OFFSET(HIO, field_0x100, 0x100);
WWHD_OFFSET(HIO, field_0x104, 0x104);
WWHD_OFFSET(HIO, field_0x110, 0x110);
WWHD_OFFSET(HIO, field_0x114, 0x114);
WWHD_OFFSET(HIO, field_0x118, 0x118);
WWHD_OFFSET(HIO, field_0x11C, 0x11C);
WWHD_OFFSET(HIO, field_0x11E, 0x11E);
WWHD_OFFSET(HIO, field_0x120, 0x120);
WWHD_OFFSET(HIO, field_0x124, 0x124);
WWHD_OFFSET(HIO, field_0x128, 0x128);
WWHD_OFFSET(HIO, field_0x12C, 0x12C);
WWHD_OFFSET(HIO, field_0x130, 0x130);
WWHD_SIZE(HIO, 0x15C);
inline HIO& hio() { return *gabi::at<HIO>(0x10463E0C); }
struct ModelMatrixBlock {
    u8 pad0[4];
    /* 0x004 */ be<u16> mFlags;
    u8 pad6[10];
    /* 0x010 */ gptr<Mtx34> mAnmMtx;};
WWHD_OFFSET(ModelMatrixBlock, mAnmMtx, 0x10);
struct JointNode {
    u8 pad0[8];
    be<u32> mCallBack;
    u8 padC[0x10];};
WWHD_SIZE(JointNode, 0x1C);
struct ModelData {
    u8 pad0[4];
    /* 0x004 */ be<u32> mJointCount;
    /* 0x008 */ gptr<JointNode> mJointNodes;};
struct Model {
    u8 pad0[0x2C];
    /* 0x02C */ gptr<ModelMatrixBlock> mMatrixBlock;
    u8 pad30[0x7C];
    /* 0x0AC */ gptr<J3DModelData> mModelData;
    u8 padB0[8];
    /* 0x0B8 */ gptr<daFm_c> mUserArea;
    /* 0x0BC */ cXyz mBaseScale;};
WWHD_OFFSET(Model, mModelData, 0xAC);
WWHD_OFFSET(Model, mUserArea, 0xB8);
WWHD_OFFSET(Model, mBaseScale, 0xBC);
struct Joint {
    u8 pad0[4];
    /* 0x004 */ be<u16> mJointNo;};
struct JointTree {
    u8 pad0[8];
    /* 0x008 */ be<u16> mJointCount;};
struct Player : fopAc_ac_c {
    u8 pad3AC[0x2C];
    /* 0x3D8 */ cXyz mHeadTopPos;};
WWHD_OFFSET(Player, mHeadTopPos, 0x3D8);
struct PlayerVtable {
    u8 pad0[0xBC];
    /* 0x0BC */ be<u32> getGrabActorID;};
struct FollowVtable {
    u8 pad0[0x44];
    /* 0x044 */ be<u32> remove;};
struct Follow {
    /* 0x000 */ gptr<FollowVtable> mVtable;};
struct Emitter {
    u8 pad0[0x220];
    /* 0x220 */ cXyz mGlobalScale;
    u8 pad22C[12];
    /* 0x238 */ cXyz mGlobalParticleScale;};
WWHD_OFFSET(Emitter, mGlobalParticleScale, 0x238);
struct QuaternionWords {
    /* 0x00C */ be<u32> x,y,z,w;};
struct LineCheck {
    u8 pad0[0x20];
    /* 0x020 */ be<u32> mVtable20;
    u8 pad24[0xC];
    /* 0x030 */ cXyz mCross;
    u8 pad3C[0x1C];
    /* 0x058 */ be<u32> mVtable58;
    u8 pad5C[8];
    /* 0x064 */ be<u32> mVtable64;};
WWHD_OFFSET(LineCheck, mCross, 0x30);
WWHD_OFFSET(LineCheck, mVtable58, 0x58);
WWHD_OFFSET(LineCheck, mVtable64, 0x64);
struct Acch {
    u8 pad0[0x14];
    /* 0x014 */ be<u32> mVtable14;
    u8 pad18[8];
    /* 0x020 */ be<u32> mVtable20;};
inline Model* model(J3DModel* m) { return reinterpret_cast<Model*>(m); }
inline J3DModel* currentModel() { return *gabi::at<gptr<J3DModel>>(0x104B462C); }
inline const QuaternionWords& zeroQuat() { return *gabi::at<const QuaternionWords>(0x101E9C38); }
inline Mtx34* stackMtx() { return gabi::at<Mtx34>(0x1048D0CC); }
inline Mtx34* currentMtx() { return gabi::at<Mtx34>(0x104B4868); }
inline Mtx34* anmMtx(J3DModel* m, u32 i) { auto* block=model(m)->mMatrixBlock.get(); Mtx34* matrices=block->mAnmMtx; block->mFlags |= 0x10; return matrices+i; }
inline JointNode* jointNode(J3DModelData* d,u32 i) { auto* data=reinterpret_cast<ModelData*>(d); u32 count=data->mJointCount; JointNode* nodes=data->mJointNodes; return i<count?nodes+i:nodes; }
inline u32 headerGuards() { return 0x10463DF0; }
inline u32 headerDestructors() { return 0x101B5228; }
constexpr u32 SafeStringVtable=0x1000F258, ActorVtable=0x1000F350, HIOVtable=0x1000F360, AabVtable=0x1000F270;
}
inline u32 daPy_getGrabActorID(fopAc_ac_c* p) { return gabi::call_ptr<u32>(gabi::at<fm_main::PlayerVtable>(p->__vtbl)->getGrabActorID,p); }
inline cXyz* daPy_getHeadTopPos(fopAc_ac_c* p) { return &reinterpret_cast<fm_main::Player*>(p)->mHeadTopPos; }
inline void fm_seStart(fopAc_ac_c* a,u32 id,u32 param) { s32 reverb=dComIfGp_getReverb(a->current.roomNo); mDoAud_seStart(id,&a->eyePos,param,reverb); }
void mDoMtx_stack_quatM(Quaternion_fm* q);
void PSMTXConcat_g(Mtx34* a, Mtx34* b, Mtx34* ab);
void cXyz_normZP(const cXyz* a, cXyz* res);
f32 PSVECSquareDistance(const cXyz* a, const cXyz* b);
bool daBomb_chk_state(fopAc_ac_c* b, s32 st);
u32 daObj_PrmAbstract(fopAc_ac_c* a, s32 w, s32 s);
fm_main::JointTree* fm_jointTree(J3DModelData* d);
void fm_jointHitHIO_ct(void* h);
void fm_invisible_ct(void* p);
void fm_sphere_ct(dCcD_Sph* p);
void fm_setAnmTables(const char* arc, mDoExt_McaMorf* morf, be<s8>* bck, be<s8>* prm, be<s8>* old, u32 bcks, u32 prms, bool force, s32 hd);
namespace fm_main {
struct AcchCir {
    u8 pad0[0x14];
    u8 mCircle[0xC];};
WWHD_SIZE(AcchCir, 0x20);
constexpr u32 AcchVtable20=0x1000F2B0, AcchVtable14=0x1000F2C0;
constexpr u32 LineVtable58=0x1000F300, LineVtable64=0x1000F290, LineVtable20=0x1000F280;
}
void fm_acch_dt(dBgS_ObjAcch* p, s32 flags);
void fm_circle_dt(void* p, s32 flags);
void fm_line_dt(void* p, s32 flags);
void fm_sphere_dt(dCcD_Sph* p, s32 flags);
void fm_invisible_dt(void* p, s32 flags);
void fm_actor_dt(fopAc_ac_c* p, s32 flags);
s32 fm_calcTimer(be<s32>* timer);
s32 fm_nameIndex(void* table, const char* name, s32 type);
inline s32 fm_charNameIndex(const char* name,s32 type) { return fm_nameIndex(gabi::at<void>(dComIfGp_ea() + PLAY_NAMETBL),name,type); }

bool invisibleModel_create(void* inv, J3DModel* model);
JntHit_c* JntHit_create(J3DModel* model, u32 data, s16 count);
namespace fm_main {
struct AnimationParameters {
    be<s8> mAnmIdx, mNextPrmIdx;
    be<s16> field_0x02;
    be<f32> mMorf,mPlaySpeed;
    be<s32> mLoopMode;};
WWHD_SIZE(AnimationParameters,0x10);
struct ModeRecord {
    be<s16> mInitDelta,mInitIndex;
    be<u32> mInitFunction;
    be<s16> mRunDelta,mRunIndex;
    be<u32> mRunFunction;
    gptr<const char> mName;};
WWHD_SIZE(ModeRecord,0x14);
using ModeFunction=void (daFm_c::*)();
inline const gabi::ptmf<ModeFunction>& initFunction(ModeRecord& record) { return *reinterpret_cast<gabi::ptmf<ModeFunction>*>(&record.mInitDelta); }
inline const gabi::ptmf<ModeFunction>& runFunction(ModeRecord& record) { return *reinterpret_cast<gabi::ptmf<ModeFunction>*>(&record.mRunDelta); }
struct JointHitRecord {
    be<s16> mShapeType,mJointIndex;
    be<f32> mRadius;
    gptr<cXyz> mpOffsets;};
WWHD_SIZE(JointHitRecord,0xC);
// dCcD source geometry is opaque in the shared header; these mirror its flat data records.
struct SphereSource {
    be<u32> mFlags,mAttackType;
    be<u8> mAttackPower;
    u8 padding[3];
    be<u32> mAttackParameters,mTargetType,mTargetParameters,mCollisionParameters;
    be<u8> mAttackSe,mAttackHitMark,mAttackSpecial,mAttackMaterial;
    be<u32> mAttackFlags;
    be<u8> mTargetSe,mTargetHitMark,mTargetSpecial,mTargetMaterial;
    be<u32> mTargetFlags,mCollisionFlags;
    be<f32> mCenterX,mCenterY,mCenterZ,mRadius;};
WWHD_SIZE(SphereSource,0x40);
struct CylinderSource {
    be<u32> mFlags,mAttackType;
    be<u8> mAttackPower;
    u8 padding[3];
    be<u32> mAttackParameters,mTargetType,mTargetParameters,mCollisionParameters;
    be<u8> mAttackSe,mAttackHitMark,mAttackSpecial,mAttackMaterial;
    be<u32> mAttackFlags;
    be<u8> mTargetSe,mTargetHitMark,mTargetSpecial,mTargetMaterial;
    be<u32> mTargetFlags,mCollisionFlags;
    be<f32> mCenterX,mCenterY,mCenterZ,mRadius,mHeight;};
WWHD_SIZE(CylinderSource,0x44);
}

namespace fm_main {
WWHD_SIZE(ModelMatrixBlock, 0x14);
WWHD_OFFSET(ModelMatrixBlock, mFlags, 0x4);
WWHD_SIZE(ModelData, 0xC);
WWHD_OFFSET(ModelData, mJointCount, 0x4);
WWHD_OFFSET(ModelData, mJointNodes, 0x8);
WWHD_SIZE(Model, 0xC8);
WWHD_OFFSET(Model, mMatrixBlock, 0x2C);
WWHD_SIZE(Joint, 0x6);
WWHD_OFFSET(Joint, mJointNo, 0x4);
WWHD_SIZE(JointTree, 0xA);
WWHD_OFFSET(JointTree, mJointCount, 0x8);
WWHD_SIZE(Player, 0x3E4);
WWHD_SIZE(PlayerVtable, 0xC0);
WWHD_OFFSET(PlayerVtable, getGrabActorID, 0xBC);
WWHD_SIZE(FollowVtable, 0x48);
WWHD_OFFSET(FollowVtable, remove, 0x44);
WWHD_SIZE(Follow, 0x4);
WWHD_OFFSET(Follow, mVtable, 0x0);
WWHD_SIZE(Emitter, 0x244);
WWHD_OFFSET(Emitter, mGlobalScale, 0x220);
WWHD_SIZE(QuaternionWords, 0x10);
WWHD_OFFSET(QuaternionWords, x, 0x0);
WWHD_OFFSET(QuaternionWords, y, 0x4);
WWHD_OFFSET(QuaternionWords, z, 0x8);
WWHD_OFFSET(QuaternionWords, w, 0xC);
WWHD_SIZE(LineCheck, 0x68);
WWHD_OFFSET(LineCheck, mVtable20, 0x20);
WWHD_SIZE(Acch, 0x24);
WWHD_OFFSET(Acch, mVtable14, 0x14);
WWHD_OFFSET(Acch, mVtable20, 0x20);
}
enum FmMode { MODE_SW_WAIT,MODE_HIDE,MODE_UNDERFOOT,MODE_PATH_MOVE,MODE_GOAL_KEEPER,MODE_APPEAR,MODE_DISAPPEAR,MODE_WAIT,MODE_ATTACK,MODE_THROW,MODE_DAMAGE,MODE_PREPARE_ITEM,MODE_DEATH,MODE_GRAB,MODE_GRAB_DEMO,MODE_GRAB_FOOT_DEMO,MODE_GRAB_NPC_DEMO,MODE_PLAYER_START_DEMO,MODE_DELETE,MODE_PARALYSIS,MODE_BIKUBIKU };
enum { FM_JNT_TE_e=5, FM_JNT_CYUBIA_e=6 };
enum { dRes_INDEX_FM_BCK_MODORU_e=0xC,dRes_INDEX_FM_BDL_FM_e=0x15,dRes_INDEX_FM_BDL_YPIT00_e=0x18,dRes_INDEX_FM_BTK_YPIT00_e=0x1B };
enum { fpcNm_FM_e=0x77,fpcNm_NPC_CB1_e=0x14E,fpcNm_NPC_MD_e=0x16F };
