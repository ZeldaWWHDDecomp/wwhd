/* Traveling merchant Btsw2.
 * HD: NPC base is 0x7DC, btp animation is0x74, shadow id is absent,
 * and the GHS action member pointer occupies eight bytes. */
#pragma once
#include "gc_api/gc_api.h"
struct daNpc_Btsw2_c : fopNpc_npc_c {
    enum ActionStatus { ACTION_STARTING = 0, ACTION_ONGOING = 1, ACTION_ENDING = -1 };
    static const char m_arc_name[];
    void checkOrder();
    void eventOrder();
    u32 getMsg();
    u16 next_msgStatus(u32* message);
    void setAttention();
    void wait01();
    BOOL _delete();
    void setAnm(s8 animation);
    void talk01();
    void anmAtr(u16 messageStatus);
    bool chkAttention(cXyz* position, s16 facing);
    BOOL _create();
    BOOL _draw();
    BOOL _execute();
    void playTexPatternAnm();
    void lookBack();
    BOOL wait_action(void* argument);
    BOOL CreateInit();
    BOOL initTexPatternAnm(u8 modify);
    BOOL CreateHeap();
    void pathMove();
    /* 0x7DC */ request_of_phase_process_class mPhs;
    /* 0x7E4 */ gptr<J3DModel> mpKabanModel;
    /* 0x7E8 */ gptr<J3DModel> mpTirasiModel;
    /* 0x7EC */ gptr<void> m_btp;
    /* 0x7F0 */ mDoExt_btpAnm mBtpAnm;
    /* 0x864 */ be<u8> mBtpFrame;
    /* 0x865 */ be<s8> m_handL_jnt_num;
    /* 0x866 */ be<s8> m_handR_jnt_num;
    u8 _867;
    /* 0x868 */ be<s16> mBlinkTimer;
    u8 _86A[0x878 - 0x86A];
    /* 0x878 */ cXyz mAttPos;
    u8 _884[0x888 - 0x884];
    /* 0x888 */ csXyz m714;
    u8 _88E[3];
    /* 0x891 */ be<u8> mHasAttention;
    /* 0x892 */ be<u8> m71E;
    u8 _893[5];
    /* 0x898 */ be<u8> m724;
    u8 _899[3];
    /* 0x89C */ gabi::ptmf<BOOL (daNpc_Btsw2_c::*)(void*)> mCurrActionFunc;
    /* 0x8A4 */ be<s8> mPathNo;
    /* 0x8A5 */ be<u8> m735;
    /* 0x8A6 */ be<u8> m736;
    u8 _8A7;
    /* 0x8A8 */ gptr<void> mpPath;
    /* 0x8AC */ be<s16> mFinalPathPntIdx;
    /* 0x8AE */ be<s16> m73E;
    /* 0x8B0 */ be<s16> mPathPntIdx;
    /* 0x8B2 */ be<s16> m742;
    /* 0x8B4 */ be<s8> m744;
    /* 0x8B5 */ be<s8> m745;
    /* 0x8B6 */ be<s8> m746;
    /* 0x8B7 */ be<s8> m747;
    u8 _8B8[2];
    /* 0x8BA */ be<s8> mActionStatus;
    u8 _8BB[1];
};
WWHD_OFFSET(daNpc_Btsw2_c, mBtpFrame, 0x864);
WWHD_OFFSET(daNpc_Btsw2_c, mAttPos, 0x878);
WWHD_OFFSET(daNpc_Btsw2_c, mCurrActionFunc, 0x89C);
WWHD_OFFSET(daNpc_Btsw2_c, mActionStatus, 0x8BA);
WWHD_SIZE(daNpc_Btsw2_c, 0x8BC);

// HD: views preserve integer copies and the GHS virtual member-pointer encoding.
struct Btsw2Words {
    be<u32> x, y, z;
};
struct Btsw2PtmfView {
    be<s16> mDelta, mIndex;
    be<u16> mHigh;
    be<s16> mVtableOffset;
};
WWHD_SIZE(Btsw2Words, 12);
WWHD_SIZE(Btsw2PtmfView, 8);
WWHD_OFFSET(Btsw2PtmfView, mVtableOffset, 6);

// HD: unit-private call boundaries retain the verified raw ABI.
BOOL dKy_daynight_check();
void Btsw2CylinderDestroy(void*, s32);
void Btsw2StatusDestroy(void*, s32);
void Btsw2CircleDestroy(void*, s32);
void Btsw2AcchDestroy(void*, s32);
void Btsw2ActorDestroy(void*, s32);
void Btsw2NpcConstruct(void*);
BOOL Btsw2CallCreateInit(void*);
BOOL Btsw2CallCreateHeap(void*);
void Btsw2NpcHioConstruct(void*);
void Btsw2DestructorChain(void*);
void Btsw2HeaderByteConstruct(void*);
void Btsw2HeaderOtherByteConstruct(void*);
void Btsw2BtpEntry(void*, void*, u8);
void Btsw2ModelUpdateDL(void*, s32);
void dSnap_RegistFig(s32, void*, void*, s16, f32, f32, f32);
void Btsw2CallPlayTexPattern(void*);
void Btsw2CorrectPosition(void*, void*);
s8 Btsw2GroundRoom(void*, void*);
u8 Btsw2GroundColor(void*, void*);
void PSMTXTrans(Mtx34*, f32, f32, f32);
void Btsw2PlayerEye(cXyz*, f32);
void Btsw2LookAt(void*, void*, cXyz*, cXyz*, s16, s16, s32);
u8 Btsw2CallChkAttention(void*, cXyz*, s16);
void Btsw2CallPathMove(void*);
void Btsw2CylinderSet(void*, void*);
void* Btsw2RoomPath(s32, s32);
void* Btsw2Resource(void*, SafeString*, u32);
BOOL Btsw2BtpInit(void*, void*, void*, s32, s32, s32, s32, u8, f32);
void* Btsw2JointNames(void*);
s32 Btsw2NameIndex(void*, const char*);
void* Btsw2MorfCreate(void*, void*, void*, void*, void*, s32, s32, s32, s32, s32, u32, u32, f32);
void* Btsw2ModelCreate(void*, s32, u32);
void* Btsw2JointObject();
void PSMTXMultVec(const Mtx34*, const cXyz*, cXyz*);
void PSMTXConcat(const Mtx34*, const Mtx34*, Mtx34*);
f32 PSVECDotProduct(const cXyz*, const cXyz*);
s16 Btsw2DistanceAngle(s16, s16);
void Btsw2PosMove(void*, void*);

BOOL Btsw2NormalizeRS(cXyz*);

struct daNpc_Btsw2_HIO_c {
    /* 0x00 */ be<s8> mNo;
    u8 pad1[3];
    /* 0x04 */ dNpc_HIO_c mNpc;
    /* 0x2C */ be<s16> m30, m32;
    /* 0x30 */ be<f32> m34, m38;
    /* 0x38 */ be<s16> m3C, m3E;
    /* 0x3C */ be<u32> __vtbl;
};
WWHD_OFFSET(daNpc_Btsw2_HIO_c, mNpc, 4);
WWHD_OFFSET(daNpc_Btsw2_HIO_c, m30, 0x2C);
WWHD_OFFSET(daNpc_Btsw2_HIO_c, m34, 0x30);
WWHD_OFFSET(daNpc_Btsw2_HIO_c, __vtbl, 0x3C);
WWHD_SIZE(daNpc_Btsw2_HIO_c, 0x40);
struct Btsw2HeaderZero {
    be<u32> mWords[4];
};
struct Btsw2DestructorRecord {
    be<u32> mWords[3];
};
struct Btsw2DestructorRecords {
    Btsw2DestructorRecord mRecords[3];
};
struct Btsw2SinCos {
    be<f32> mSin, mCos;
};
struct Btsw2SinCosTable {
    Btsw2SinCos mEntries[8192];
};
WWHD_SIZE(Btsw2SinCos, 8);

// HD: partial external objects, represented only by fields used by this actor.
struct Btsw2TexturePattern {
    u8 pad0[4];
    be<u32> __vtbl;
};
struct Btsw2TextureVtable {
    u8 pad0[0x14];
    be<u32> mpGetFrameMax;
};
struct Btsw2DispatchEntry {
    be<u32> mDelta, mpFunction;
};
struct Btsw2DispatchVtable {
    Btsw2DispatchEntry mEntries[0x8000];
};
struct Btsw2PathPoint {
    u8 mArgs[4];
    cXyz m_position;
};
struct Btsw2Path {
    be<u16> m_num;
    u8 pad2[6];
    be<u32> m_points;
};
struct Btsw2NameBlock {
    u8 pad0[0x10];
    be<u32> mTableOffset;
};
struct Btsw2JointData {
    u8 pad0[4];
    be<u16> mJointNo;
};
struct Btsw2PatternBinding {
    u8 pad0[0x38];
    be<u32> mpPattern;
};
struct Btsw2CylinderSource {
    u8 mData[0x44];
};
WWHD_OFFSET(Btsw2TexturePattern, __vtbl, 4);
WWHD_OFFSET(Btsw2TextureVtable, mpGetFrameMax, 0x14);
WWHD_SIZE(Btsw2DispatchEntry, 8);
WWHD_OFFSET(Btsw2DispatchEntry, mpFunction, 4);
WWHD_SIZE(Btsw2PathPoint, 16);
WWHD_OFFSET(Btsw2PathPoint, m_position, 4);
WWHD_OFFSET(Btsw2Path, m_points, 8);
WWHD_OFFSET(Btsw2NameBlock, mTableOffset, 0x10);
WWHD_OFFSET(Btsw2JointData, mJointNo, 4);
WWHD_OFFSET(Btsw2PatternBinding, mpPattern, 0x38);

// HD: translation-unit vtables and the immediate GHS action target.
namespace btsw2_tu {
constexpr u32 acchGroupVtable = 0x10018A18;
constexpr u32 acchPolyVtable = 0x10018A28;
constexpr u32 actorVtable = 0x10018BB8;
constexpr u32 hioVtable = 0x10018A38;
constexpr u32 waitAction = 0x0221DD2C;
} // namespace btsw2_tu

struct Btsw2ArchiveName {
    char mName[5];
};
WWHD_SIZE(Btsw2ArchiveName, 5);

WWHD_OFFSET(daNpc_Btsw2_c, mPhs, 0x7DC);
WWHD_OFFSET(daNpc_Btsw2_c, mpKabanModel, 0x7E4);
WWHD_OFFSET(daNpc_Btsw2_c, m_btp, 0x7EC);
WWHD_OFFSET(daNpc_Btsw2_c, mBtpAnm, 0x7F0);
WWHD_OFFSET(daNpc_Btsw2_c, m714, 0x888);
WWHD_OFFSET(daNpc_Btsw2_c, m724, 0x898);
WWHD_OFFSET(daNpc_Btsw2_c, mpPath, 0x8A8);
WWHD_OFFSET(daNpc_Btsw2_c, m745, 0x8B5);
WWHD_OFFSET(daNpc_Btsw2_HIO_c, m38, 0x34);
WWHD_OFFSET(daNpc_Btsw2_HIO_c, m3C, 0x38);
WWHD_OFFSET(daNpc_Btsw2_HIO_c, m3E, 0x3A);
