/* Picto Box capture: tww dSnap object layout and HD rendering packet.
 * HD: packet objects begin at0xA0 (GC0x18); HD adds textures, camera and post-process state.
 * Exact verified register widths and guest layouts. */
#pragma once
#include "gc_api/gc_api.h"

enum {
    /* 0x00 */ DSNAP_TYPE_NONE,
    /* 0x01 */ DSNAP_TYPE_POST,        // Garrickson mailing his love letter
    /* 0x02 */ DSNAP_TYPE_BIKUTSUKI,   // Gossack scared
    /* 0x03 */ DSNAP_TYPE_COUPLE_LOOK, // Linda and Anton, the secret couple
    /* 0x04 */ DSNAP_TYPE_UNK04,
    /* 0x05 */ DSNAP_TYPE_UNK05,
    /* 0x06 */ DSNAP_TYPE_UNK06,
    /* 0x07 */ DSNAP_TYPE_UNK07,
    /* 0x08 */ DSNAP_TYPE_UNK08,
    /* 0x09 */ DSNAP_TYPE_UNK09,
    /* 0x0A */ DSNAP_TYPE_UNK0A,
    /* 0x0B */ DSNAP_TYPE_UNK0B,
    /* 0x0C */ DSNAP_TYPE_UNK0C,
    /* 0x0D */ DSNAP_TYPE_UNK0D,
    /* 0x0E */ DSNAP_TYPE_UNK0E,
    /* 0x0F */ DSNAP_TYPE_UNK0F,
    /* 0x10 */ DSNAP_TYPE_UNK10,
    /* 0x11 */ DSNAP_TYPE_UNK11,
    /* 0x12 */ DSNAP_TYPE_UNK12,
    /* 0x13 */ DSNAP_TYPE_UNK13,
    /* 0x14 */ DSNAP_TYPE_UNK14,
    /* 0x15 */ DSNAP_TYPE_UNK15,
    /* 0x16 */ DSNAP_TYPE_UNK16,
    /* 0x17 */ DSNAP_TYPE_UNK17,
    /* 0x18 */ DSNAP_TYPE_UNK18,
    /* 0x19 */ DSNAP_TYPE_UNK19,
    /* 0x1A */ DSNAP_TYPE_UNK1A,
    /* 0x1B */ DSNAP_TYPE_UNK1B,
    /* 0x1C */ DSNAP_TYPE_UNK1C,
    /* 0x1D */ DSNAP_TYPE_UNK1D,
    /* 0x1E */ DSNAP_TYPE_UNK1E,
    /* 0x1F */ DSNAP_TYPE_UNK1F,
    /* 0x20 */ DSNAP_TYPE_UNK20,
    /* 0x21 */ DSNAP_TYPE_UNK21,
    /* 0x22 */ DSNAP_TYPE_UNK22,
    /* 0x23 */ DSNAP_TYPE_UNK23,
    /* 0x24 */ DSNAP_TYPE_UNK24,
    /* 0x25 */ DSNAP_TYPE_UNK25,
    /* 0x26 */ DSNAP_TYPE_UNK26,
    /* 0x27 */ DSNAP_TYPE_UNK27,
    /* 0x28 */ DSNAP_TYPE_UNK28,
    /* 0x29 */ DSNAP_TYPE_UNK29,
    /* 0x2A */ DSNAP_TYPE_UNK2A,
    /* 0x2B */ DSNAP_TYPE_UNK2B,
    /* 0x2C */ DSNAP_TYPE_UNK2C,
    /* 0x2D */ DSNAP_TYPE_UNK2D,
    /* 0x2E */ DSNAP_TYPE_UNK2E,
    /* 0x2F */ DSNAP_TYPE_UNK2F,
    /* 0x30 */ DSNAP_TYPE_UNK30,
    /* 0x31 */ DSNAP_TYPE_UNK31,
    /* 0x32 */ DSNAP_TYPE_UNK32,
    /* 0x33 */ DSNAP_TYPE_UNK33,
    /* 0x34 */ DSNAP_TYPE_UNK34,
    /* 0x35 */ DSNAP_TYPE_UNK35,
    /* 0x36 */ DSNAP_TYPE_UNK36,
    /* 0x37 */ DSNAP_TYPE_UNK37,
    /* 0x38 */ DSNAP_TYPE_UNK38,
    /* 0x39 */ DSNAP_TYPE_UNK39,
    /* 0x3A */ DSNAP_TYPE_UNK3A,
    /* 0x3B */ DSNAP_TYPE_UNK3B,
    /* 0x3C */ DSNAP_TYPE_UNK3C,
    /* 0x3D */ DSNAP_TYPE_UNK3D,
    /* 0x3E */ DSNAP_TYPE_UNK3E,
    /* 0x3F */ DSNAP_TYPE_UNK3F,
    /* 0x40 */ DSNAP_TYPE_UNK40,
    /* 0x41 */ DSNAP_TYPE_UNK41,
    /* 0x42 */ DSNAP_TYPE_UNK42,
    /* 0x43 */ DSNAP_TYPE_UNK43,
    /* 0x44 */ DSNAP_TYPE_UNK44,
    /* 0x45 */ DSNAP_TYPE_UNK45,
    /* 0x46 */ DSNAP_TYPE_UNK46,
    /* 0x47 */ DSNAP_TYPE_UNK47,
    /* 0x48 */ DSNAP_TYPE_UNK48,
    /* 0x49 */ DSNAP_TYPE_NPC_LS1,
    /* 0x4A */ DSNAP_TYPE_NPC_BA1,
    /* 0x4B */ DSNAP_TYPE_UNK4B,
    /* 0x4C */ DSNAP_TYPE_UNK4C,
    /* 0x4D */ DSNAP_TYPE_NPC_JI1,
    /* 0x4E */ DSNAP_TYPE_UNK4E,
    /* 0x4F */ DSNAP_TYPE_UNK4F,
    /* 0x50 */ DSNAP_TYPE_UNK50,
    /* 0x51 */ DSNAP_TYPE_UNK51,
    /* 0x52 */ DSNAP_TYPE_UNK52,
    /* 0x53 */ DSNAP_TYPE_UNK53,
    /* 0x54 */ DSNAP_TYPE_KB,
    /* 0x55 */ DSNAP_TYPE_KAMOME,
    /* 0x56 */ DSNAP_TYPE_KN,
    /* 0x57 */ DSNAP_TYPE_NPC_TT,
    /* 0x58 */ DSNAP_TYPE_NPC_KM1_KK1,
    /* 0x59 */ DSNAP_TYPE_UNK59,
    /* 0x5A */ DSNAP_TYPE_UNK5A,
    /* 0x5B */ DSNAP_TYPE_UNK5B,
    /* 0x5C */ DSNAP_TYPE_UNK5C,
    /* 0x5D */ DSNAP_TYPE_UNK5D,
    /* 0x5E */ DSNAP_TYPE_NPC_PHOTO,
    /* 0x5F */ DSNAP_TYPE_NPC_RSH1,
    /* 0x60 */ DSNAP_TYPE_NPC_HO,
    /* 0x61 */ DSNAP_TYPE_NPC_MK,
    /* 0x62 */ DSNAP_TYPE_NPC_UG1_UG2,
    /* 0x63 */ DSNAP_TYPE_NPC_UM2,
    /* 0x64 */ DSNAP_TYPE_NPC_UM1,
    /* 0x65 */ DSNAP_TYPE_NPC_UM3,
    /* 0x66 */ DSNAP_TYPE_NPC_UW1,
    /* 0x67 */ DSNAP_TYPE_NPC_UW2,
    /* 0x68 */ DSNAP_TYPE_NPC_UO1,
    /* 0x69 */ DSNAP_TYPE_NPC_UO2,
    /* 0x6A */ DSNAP_TYPE_NPC_UO3,
    /* 0x6B */ DSNAP_TYPE_NPC_UB1_UB2,
    /* 0x6C */ DSNAP_TYPE_NPC_UB3,
    /* 0x6D */ DSNAP_TYPE_NPC_UB4,
    /* 0x6E */ DSNAP_TYPE_NPC_SA2,
    /* 0x6F */ DSNAP_TYPE_NPC_SA3,
    /* 0x70 */ DSNAP_TYPE_NPC_SA5,
    /* 0x71 */ DSNAP_TYPE_NPC_SA4,
    /* 0x72 */ DSNAP_TYPE_NPC_ZL1,
    /* 0x73 */ DSNAP_TYPE_NPC_P1A,
    /* 0x74 */ DSNAP_TYPE_NPC_P1B,
    /* 0x75 */ DSNAP_TYPE_NPC_P1C,
    /* 0x76 */ DSNAP_TYPE_UNK76,
    /* 0x77 */ DSNAP_TYPE_UNK77,
    /* 0x78 */ DSNAP_TYPE_UNK78,
    /* 0x79 */ DSNAP_TYPE_NPC_TC_NORMAL,
    /* 0x7A */ DSNAP_TYPE_NPC_TC_RED,
    /* 0x7B */ DSNAP_TYPE_NPC_TC_BLUE,
    /* 0x7C */ DSNAP_TYPE_NPC_TC_WHITE,
    /* 0x7D */ DSNAP_TYPE_UNK7D,
    /* 0x7E */ DSNAP_TYPE_NPC_ROTEN,
    /* 0x7F */ DSNAP_TYPE_NPC_AH,
    /* 0x80 */ DSNAP_TYPE_NPC_BS1,
    /* 0x81 */ DSNAP_TYPE_UNK81,
    /* 0x82 */ DSNAP_TYPE_NPC_SA1,
    /* 0x83 */ DSNAP_TYPE_SALVAGE_CORP,
    /* 0x84 */ DSNAP_TYPE_NPC_FA1,
    /* 0x85 */ DSNAP_TYPE_UNK85,
    /* 0x86 */ DSNAP_TYPE_UNK86,
    /* 0x87 */ DSNAP_TYPE_UNK87,
    /* 0x88 */ DSNAP_TYPE_UNK88,
    /* 0x89 */ DSNAP_TYPE_UNK89,
    /* 0x8A */ DSNAP_TYPE_NPC_MD,
    /* 0x8B */ DSNAP_TYPE_UNK8B,
    /* 0x8C */ DSNAP_TYPE_UNK8C,
    /* 0x8D */ DSNAP_TYPE_NPC_BM1_PST,
    /* 0x8E */ DSNAP_TYPE_NPC_BM1_SKT_KKT,
    /* 0x8F */ DSNAP_TYPE_NPC_BM1_BMD_1,
    /* 0x90 */ DSNAP_TYPE_NPC_BM1_BMC_0_1_2,
    /* 0x91 */ DSNAP_TYPE_NPC_BM1_BMB_2,
    /* 0x92 */ DSNAP_TYPE_NPC_BM1_BMD_0,
    /* 0x93 */ DSNAP_TYPE_NPC_BM1_BMB_0_1,
    /* 0x94 */ DSNAP_TYPE_UNK94,
    /* 0x95 */ DSNAP_TYPE_UNK95,
    /* 0x96 */ DSNAP_TYPE_NPC_BMSW,
    /* 0x97 */ DSNAP_TYPE_NPC_BM1_BMC_3,
    /* 0x98 */ DSNAP_TYPE_NPC_BTSW,
    /* 0x99 */ DSNAP_TYPE_DR,
    /* 0x9A */ DSNAP_TYPE_NPC_HR,
    /* 0x9B */ DSNAP_TYPE_UNK9B,
    /* 0x9C */ DSNAP_TYPE_NPC_CB1,
    /* 0x9D */ DSNAP_TYPE_UNK9D,
    /* 0x9E */ DSNAP_TYPE_UNK9E,
    /* 0x9F */ DSNAP_TYPE_UNK9F,
    /* 0xA0 */ DSNAP_TYPE_UNKA0,
    /* 0xA1 */ DSNAP_TYPE_UNKA1,
    /* 0xA2 */ DSNAP_TYPE_UNKA2,
    /* 0xA3 */ DSNAP_TYPE_UNKA3,
    /* 0xA4 */ DSNAP_TYPE_UNKA4,
    /* 0xA5 */ DSNAP_TYPE_UNKA5,
    /* 0xA6 */ DSNAP_TYPE_UNKA6,
    /* 0xA7 */ DSNAP_TYPE_NPC_MT,
    /* 0xA8 */ DSNAP_TYPE_NPC_MN,
    /* 0xA9 */ DSNAP_TYPE_UNKA9,
    /* 0xAA */ DSNAP_TYPE_BK,
    /* 0xAB */ DSNAP_TYPE_UNKAB,
    /* 0xAC */ DSNAP_TYPE_CC,
    /* 0xAD */ DSNAP_TYPE_UNKAD,
    /* 0xAE */ DSNAP_TYPE_KI,
    /* 0xAF */ DSNAP_TYPE_UNKAF,
    /* 0xB0 */ DSNAP_TYPE_BB,
    /* 0xB1 */ DSNAP_TYPE_UNKB1,
    /* 0xB2 */ DSNAP_TYPE_UNKB2,
    /* 0xB3 */ DSNAP_TYPE_KS,
    /* 0xB4 */ DSNAP_TYPE_UNKB4,
    /* 0xB5 */ DSNAP_TYPE_FM,
    /* 0xB6 */ DSNAP_TYPE_AM2,
    /* 0xB7 */ DSNAP_TYPE_AM,
    /* 0xB8 */ DSNAP_TYPE_UNKB8,
    /* 0xB9 */ DSNAP_TYPE_RD,
    /* 0xBA */ DSNAP_TYPE_UNKBA,
    /* 0xBB */ DSNAP_TYPE_UNKBB,
    /* 0xBC */ DSNAP_TYPE_UNKBC,
    /* 0xBD */ DSNAP_TYPE_MO2,
    /* 0xBE */ DSNAP_TYPE_UNKBE,
    /* 0xBF */ DSNAP_TYPE_TN_NO_SHIELD,
    /* 0xC0 */ DSNAP_TYPE_TN_SHIELD,
    /* 0xC1 */ DSNAP_TYPE_TN_MIGHTY,
    /* 0xC2 */ DSNAP_TYPE_FGANON,
    /* 0xC3 */ DSNAP_TYPE_ST,
    /* 0xC4 */ DSNAP_TYPE_UNKC4,
    /* 0xC5 */ DSNAP_TYPE_UNKC5,
    /* 0xC6 */ DSNAP_TYPE_DAIOCTA,
    /* 0xC7 */ DSNAP_TYPE_BTD,
    /* 0xC8 */ DSNAP_TYPE_BMD,
    /* 0xC9 */ DSNAP_TYPE_BST,
    /* 0xCA */ DSNAP_TYPE_BDK,
    /* 0xCB */ DSNAP_TYPE_BPW,
    /* 0xCC */ DSNAP_TYPE_BWD,
    /* 0xCD */ DSNAP_TYPE_BGN,
    /* 0xCE */ DSNAP_TYPE_GND,
    /* 0xCF */ DSNAP_TYPE_UNKCF,
    /* 0xD0 */ DSNAP_TYPE_LAST_INDEX,
};

static constexpr s32 CharaDataTblSize = 134;
static constexpr s32 SnapObjectCount = 63;
static constexpr u32 SnapJudgeCount = 11;
enum dSnapHD_TextureId { SnapColorTexture = 9, SnapDepthTexture = 10, SnapLargeTexture = 11 };
enum dSnapHD_CaptureFlag { SnapCapture = 1, SnapShutterReleased = 2 };

struct dSnap_Obj {
    /*0x00*/ cXyz mCenter;
    /*0x0C*/ be<f32> mRadius;
    /*0x10*/ be<f32> mHeight;
    /*0x14*/ be<u32> mActorPID;
    /*0x18*/ be<u8> mPhotoNo;
    /*0x19*/ be<u8> field_0x19;
    /*0x1A*/ be<u8> field_0x1a;
    /*0x1B*/ be<u8> field_0x1b;
    /*0x1C*/ be<s16> mCullAngle;
    /*0x1E*/ be<s16> mAngleY;
    /*0x20*/ be<s32> mCapturedPixels;
    /*0x24*/ be<f32> mCapturedRatio;
    /*0x28*/ be<s16> mMinX;
    /*0x2A*/ be<s16> mMinY;
    /*0x2C*/ be<s16> mMaxX;
    /*0x2E*/ be<s16> mMaxY;
    /*0x30*/ be<u32> __vtbl;
    void SetAreaClear();
    void SetArea(s16 x, s16 y);
    u32 ChkCamCull();
    void SetGeo(u32 v, f32 radius, f32 height, s16 angle);
    void SetInf(u32 type, u32 actor, u32 room, u32 points, s16 angle);
    void SetGeoSph(u32 v, f32 radius);
    u32 ChkPhoto(u32 type);
    u32 ChkSuccess(s32 area, f32 ratio);
    void draw(s32 alpha);
    f32 CalcArea2D();
};
WWHD_SIZE(dSnap_Obj, 0x34);
WWHD_OFFSET(dSnap_Obj, mCenter, 0);
struct dSnap_RegistObjElm : dSnap_Obj {
    /*0x34*/ be<f32> field_0x34;
    /*0x38*/ be<u32> __elmVtable;
    u32 Regist(u32 src);
};
WWHD_SIZE(dSnap_RegistObjElm, 0x3C);
WWHD_OFFSET(dSnap_RegistObjElm, field_0x34, 0x34);
WWHD_OFFSET(dSnap_RegistObjElm, __elmVtable, 0x38);
struct dSnap_packet {
    /*0x00*/ u8 _00[0xC];
    /*0x0C*/ be<u32> __vtbl;
    /*0x10*/ u8 _10[0x88];
    /*0x98*/ be<s32> mPhotoNo;
    /*0x9C*/ be<s32> field_0x14;
    /*0xA0*/ dSnap_RegistObjElm m_tbl[63];
    /*0xF64*/ be<u32> mFlag;
    /*0xF68*/ be<u32> mResult;
    /*0xF6C*/ be<u8> mResultDetail;
    /*0xF6D*/ u8 _f6d[3];
    /*0xF70*/ be<u32> mpColorTexture;
    /*0xF74*/ be<u32> mpDepthTexture;
    /*0xF78*/ be<u32> mpRenderBuffer;
    /*0xF7C*/ be<u32> mpHeap;
    /*0xF80*/ be<u32> mpCamera;
    /*0xF84*/ be<u32> mpProjection;
    /*0xF88*/ be<u32> mpViewport;
    /*0xF8C*/ be<u32> mpEffect;
    /*0xF90*/ be<u32> mpAlternateEffect;
    void ReleaseShutter();
    u32 ChkCapture();
    u32 ChkReleaseShutter();
    void Execute();
    s32 FindPhoto(s32 start, u32 type);
    u32 JudgePost();
    u32 JudgeBikutsuki();
    u32 JudgeGF();
    u32 JudgeCoupleLook();
    u32 JudgeGenzo();
    u32 JudgeObasan4();
    u32 JudgeGene();
    u32 JudgeTestM();
    u32 JudgeFigure(u32 index);
    void Remove();
    u32 Regist(u32 obj);
    void SetResult();
    void Judge();
    void Create(u32 heap);
    void draw(u32 context);
    void DebugDraw(u32 context);
};
WWHD_SIZE(dSnap_packet, 0xF94);
WWHD_OFFSET(dSnap_packet, m_tbl, 0xA0);
WWHD_OFFSET(dSnap_Obj, mRadius, 0xC);
WWHD_OFFSET(dSnap_Obj, mHeight, 0x10);
WWHD_OFFSET(dSnap_Obj, mActorPID, 0x14);
WWHD_OFFSET(dSnap_Obj, mPhotoNo, 0x18);
WWHD_OFFSET(dSnap_Obj, field_0x19, 0x19);
WWHD_OFFSET(dSnap_Obj, field_0x1a, 0x1A);
WWHD_OFFSET(dSnap_Obj, field_0x1b, 0x1B);
WWHD_OFFSET(dSnap_Obj, mCullAngle, 0x1C);
WWHD_OFFSET(dSnap_Obj, mAngleY, 0x1E);
WWHD_OFFSET(dSnap_Obj, mCapturedPixels, 0x20);
WWHD_OFFSET(dSnap_Obj, mCapturedRatio, 0x24);
WWHD_OFFSET(dSnap_Obj, mMinX, 0x28);
WWHD_OFFSET(dSnap_Obj, mMinY, 0x2A);
WWHD_OFFSET(dSnap_Obj, mMaxX, 0x2C);
WWHD_OFFSET(dSnap_Obj, mMaxY, 0x2E);
WWHD_OFFSET(dSnap_Obj, __vtbl, 0x30);
WWHD_OFFSET(dSnap_packet, mPhotoNo, 0x98);
WWHD_OFFSET(dSnap_packet, field_0x14, 0x9C);
WWHD_OFFSET(dSnap_packet, mFlag, 0xF64);
WWHD_OFFSET(dSnap_packet, mResult, 0xF68);
WWHD_OFFSET(dSnap_packet, mResultDetail, 0xF6C);
WWHD_OFFSET(dSnap_packet, mpColorTexture, 0xF70);
WWHD_OFFSET(dSnap_packet, mpDepthTexture, 0xF74);
WWHD_OFFSET(dSnap_packet, mpRenderBuffer, 0xF78);
WWHD_OFFSET(dSnap_packet, mpHeap, 0xF7C);
WWHD_OFFSET(dSnap_packet, mpCamera, 0xF80);
WWHD_OFFSET(dSnap_packet, mpProjection, 0xF84);
WWHD_OFFSET(dSnap_packet, mpViewport, 0xF88);
WWHD_OFFSET(dSnap_packet, mpEffect, 0xF8C);
WWHD_OFFSET(dSnap_packet, mpAlternateEffect, 0xF90);
WWHD_OFFSET(dSnap_packet, __vtbl, 0xC);

s32 dSnap_PhotoIndex2TableIndex(s32 index);
u32 dSnap_GetFigRoomId(s32 index);
void dSnap_project(u32 in, u32 dst);
u32 dSnap_Obj_ctor(u32 s);
u32 dSnap_RegistObjElm_ctor(u32 s);
void dSnap_Create(u32 heap);
void dSnap_ReleaseShutter();
u32 dSnap_ChkCapture();
void dSnap_Execute();
u32 dSnap_GetResult();
u32 dSnap_GetResultDetail();
u32 dSnap_RegistSnapObj(u32 obj);
void dSnap_RegistFigActor(u32 type, u32 actor, f32 x, f32 y, f32 z);
void dSnap_DebugDraw();
void dSnap_Delete();
void dSnap_MtxCopy(u32 dst, u32 src);
void dSnap_packet_dt(u32 s, u32 flags);
void dSnap_Obj_dt(u32 s, u32 flags);
void dSnapHD_createColorTexture(u32 s, u32 src, u32 mode, u32 alloc);
void dSnapHD_createDepthTexture(u32 s, u32 src, u32 mode, u32 alloc);
void dSnapHD_updateColorTexture(u32 s, u32 src, u32 mode, u32 alloc);
void dSnapHD_updateDepthTexture(u32 s, u32 src, u32 mode, u32 alloc);
void dSnapHD_assignColorTexture(u32 s, u32 src);
void dSnapHD_assignDepthTexture(u32 s, u32 src);
void dSnapHD_scaleTransMtx(u32 dst, u32 scale, u32 pos);
u32 dSnap_packet_ctor(u32 s);
void dSnap_RegistFigGuest(u32 type, u32 actor, u32 pos, s16 angle, f32 rScale, f32 hScale,
                          f32 pScale);
void __sinit_d_snap_cpp();

// HD: rendering bindings retain the verified guest ABI.
namespace snap_api {
inline s32 distanceAngleS(u32 a0, u32 a1) { return gabi::call<s32>(0x0200FAAC, a0, a1); }
inline f32 vectorProduct2d(f32 a0, f32 a1, f32 a2, f32 a3, f32 a4, f32 a5) {
    return gabi::call<f32>(0x02010CFC, a0, a1, a2, a3, a4, a5);
}
inline s32 atan2s(f32 a0, f32 a1) { return gabi::call<s32>(0x020195B0, a0, a1); }
inline void normalizeVec(u32 a0) { return gabi::call<void>(0x025155D8, a0); }
inline u32 getPlay() { return gabi::call<u32>(0x025200D4); }
inline void YrotS(u32 a0, u32 a1) { return gabi::call<void>(0x025F1884, a0, a1); }
inline void YrotM(u32 a0, u32 a1) { return gabi::call<void>(0x025F1C28, a0, a1); }
inline void scaleM(f32 a0, f32 a1, f32 a2) { return gabi::call<void>(0x025F2518, a0, a1, a2); }
inline u32 drawDepthTexture(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5) {
    return gabi::call<u32>(0x0272C6C0, a0, a1, a2, a3, a4, a5);
}
inline u32 drawColorTexture(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5) {
    return gabi::call<u32>(0x0272C898, a0, a1, a2, a3, a4, a5);
}
inline u32 getScreenSize() { return gabi::call<u32>(0x02739728); }
inline void showAssert(u32 a0, u32 a1, u32 a2) { return gabi::call<void>(0x0273AA24, a0, a1, a2); }
inline u32 alloc(u32 a0) { return gabi::call<u32>(0x0273AD10, a0); }
inline void free(u32 a0) { return gabi::call<void>(0x0273AF40, a0); }
inline u32 allocFromHeap(u32 a0, u32 a1, u32 a2) { return gabi::call<u32>(0x0273B050, a0, a1, a2); }
inline void project(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4) {
    return gabi::call<void>(0x0274CB50, a0, a1, a2, a3, a4);
}
inline void applyRenderBuffer(u32 a0) { return gabi::call<void>(0x0274D690, a0); }
inline void updateProjection(u32 a0) { return gabi::call<void>(0x0274D80C, a0); }
inline u32 getProjectionMtx(u32 a0) { return gabi::call<u32>(0x0274D83C, a0); }
inline void orthoCameraCtor(u32 a0) { return gabi::call<void>(0x0274E1C8, a0); }
inline void orthoCameraDtor(u32 a0, u32 a1) { return gabi::call<void>(0x0274E474, a0, a1); }
inline u32 projectionCtor(u32 a0) { return gabi::call<u32>(0x0274E87C, a0); }
inline void setProjectionMtx(u32 a0, u32 a1, u32 a2) {
    return gabi::call<void>(0x0274E910, a0, a1, a2);
}
inline void matrixProjectionCtor(u32 a0, u32 a1, u32 a2) {
    return gabi::call<void>(0x0274EAF0, a0, a1, a2);
}
inline void matrixProjectionDtor(u32 a0, u32 a1) { return gabi::call<void>(0x0274EB7C, a0, a1); }
inline void getDepthViewport(u32 a0, u32 a1) { return gabi::call<void>(0x0274F53C, a0, a1); }
inline void getColorViewport(u32 a0, u32 a1) { return gabi::call<void>(0x0274F5F0, a0, a1); }
inline void applyViewport(u32 a0, u32 a1) { return gabi::call<void>(0x0274F964, a0, a1); }
inline void renderStateCtor(u32 a0) { return gabi::call<void>(0x0274FDBC, a0); }
inline void applyRenderState(u32 a0) { return gabi::call<void>(0x0274FEB0, a0); }
inline void setDebugCamera(u32 a0, u32 a1) { return gabi::call<void>(0x027509A0, a0, a1); }
inline void setDebugProjection(u32 a0, u32 a1) { return gabi::call<void>(0x027509B4, a0, a1); }
inline void setDebugModelMtx(u32 a0, u32 a1) { return gabi::call<void>(0x027509C8, a0, a1); }
inline void beginDebugDraw(u32 a0) { return gabi::call<void>(0x027509D8, a0); }
inline void endDebugDraw(u32 a0) { return gabi::call<void>(0x027509EC, a0); }
inline void drawSphere(u32 a0, u32 a1, u32 a2) { return gabi::call<void>(0x02750A00, a0, a1, a2); }
inline void drawCylinder(u32 a0, u32 a1, u32 a2) {
    return gabi::call<void>(0x02750A24, a0, a1, a2);
}
inline void flushRenderContext(u32 a0) { return gabi::call<void>(0x0276B114, a0); }
inline void resetPostProcess(u32 a0) { return gabi::call<void>(0x0279D8E0, a0); }
inline u32 applyPostProcess(u32 a0, u32 a1, u32 a2) {
    return gabi::call<u32>(0x0279DB64, a0, a1, a2);
}
inline u32 getTextureResource(u32 a0, u32 a1) { return gabi::call<u32>(0x027A7558, a0, a1); }
inline u32 drawTextureQuad(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4) {
    return gabi::call<u32>(0x027AAD8C, a0, a1, a2, a3, a4);
}
inline void createColorTarget(u32 a0) { return gabi::call<void>(0x027B6C60, a0); }
inline u32 colorTargetCtor(u32 a0) { return gabi::call<u32>(0x027B6CB8, a0); }
inline void prepareColorTarget(u32 a0) { return gabi::call<void>(0x027B6D64, a0); }
inline void createDepthTarget(u32 a0) { return gabi::call<void>(0x027B6E2C, a0); }
inline u32 depthTargetCtor(u32 a0) { return gabi::call<u32>(0x027B6E64, a0); }
inline void prepareDepthTarget(u32 a0) { return gabi::call<void>(0x027B6F18, a0); }
inline void updateDepthTarget(u32 a0) { return gabi::call<void>(0x027B6F90, a0); }
inline void clearRenderTargets(u32 a0) { return gabi::call<void>(0x027B954C, a0); }
inline u32 renderBufferCtor(u32 a0) { return gabi::call<u32>(0x027B95B4, a0); }
inline void clearRenderBuffer(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, f32 a5) {
    return gabi::call<void>(0x027B997C, a0, a1, a2, a3, a4, a5);
}
inline void setRenderAlpha(u32 a0, u32 a1) { return gabi::call<void>(0x027B9D00, a0, a1); }
inline void setRenderMode(u32 a0) { return gabi::call<void>(0x027B9F68, a0); }
inline void assignTextureView(u32 a0, u32 a1) { return gabi::call<void>(0x027BDEB4, a0, a1); }
inline void textureViewCtor(u32 a0) { return gabi::call<void>(0x027BDF7C, a0); }
inline void createTextureView(u32 a0, u32 a1) { return gabi::call<void>(0x027BE114, a0, a1); }
inline void textureViewDtor(u32 a0, u32 a1) { return gabi::call<void>(0x027BE2B0, a0, a1); }
inline void invalidateTexture(u32 a0) { return gabi::call<void>(0x027BE794, a0); }
inline void textureReaderCtor(u32 a0) { return gabi::call<void>(0x027BF144, a0); }
inline void endTextureRead(u32 a0) { return gabi::call<void>(0x027BF1A4, a0); }
inline void textureReaderDtor(u32 a0, u32 a1) { return gabi::call<void>(0x027BF1E8, a0, a1); }
inline void readPixel(u32 a0, u32 a1, u32 a2, u32 a3) {
    return gabi::call<void>(0x027BF244, a0, a1, a2, a3);
}
inline void allocateTextureReader(u32 a0, u32 a1, u32 a2) {
    return gabi::call<void>(0x027BF3FC, a0, a1, a2);
}
inline void setReadTexture(u32 a0, u32 a1) { return gabi::call<void>(0x027BF598, a0, a1); }
inline void beginTextureRead(u32 a0) { return gabi::call<void>(0x027BF730, a0); }
inline void entryImm(u32 a0, u32 a1, u32 a2) { return gabi::call<void>(0x027F0E04, a0, a1, a2); }
inline void packetCtor(u32 a0) { return gabi::call<void>(0x027F1278, a0); }
inline void packetDtor(u32 a0, u32 a1) { return gabi::call<void>(0x027F13DC, a0, a1); }
inline u32 getDrawFlags(u32 a0) { return gabi::call<u32>(0x027F29D4, a0); }
inline u32 getTexture(u32 a0, u32 a1) { return gabi::call<u32>(0x027F81A4, a0, a1); }
inline u32 createTexture(u32 a0, u32 a1, u32 a2) { return gabi::call<u32>(0x027F81C0, a0, a1, a2); }
inline void removeTexture(u32 a0, u32 a1) { return gabi::call<void>(0x027F88C8, a0, a1); }
inline void copyProjectionMtx(u32 a0, u32 a1) { return gabi::call<void>(0x028E8970, a0, a1); }
inline void VECAdd(u32 a0, u32 a1, u32 a2) { return gabi::call<void>(0x028E8D88, a0, a1, a2); }
inline void VECSubtract(u32 a0, u32 a1, u32 a2) { return gabi::call<void>(0x028E8DAC, a0, a1, a2); }
inline f32 VECSquareDistance(u32 a0, u32 a1) { return gabi::call<f32>(0x028E8DE8, a0, a1); }
inline void MTXMultVec(u32 a0, u32 a1, u32 a2) { return gabi::call<void>(0x028E8F64, a0, a1, a2); }
inline void MTXTrans(u32 a0, f32 a1, f32 a2, f32 a3) {
    return gabi::call<void>(0x028E93CC, a0, a1, a2, a3);
}
inline void initializeByteState(u32 a0) { return gabi::call<void>(0x028EAB2C, a0); }
inline void initializeAngleState(u32 a0) { return gabi::call<void>(0x028ED6F8, a0); }
inline void constructArray(u32 a0, u32 a1, u32 a2, u32 a3) {
    return gabi::call<void>(0x028EFFD0, a0, a1, a2, a3);
}
inline void registerGlobalObject(u32 a0) { return gabi::call<void>(0x028F026C, a0); }
inline void invalidateDepthBuffer(u32 a0) { return gabi::call<void>(0xC00061A0, a0); }
} // namespace snap_api

// Integer transfers of float members are deliberate GHS word copies.
template <class T, class U> inline be<T> &snapFieldBits(be<U> &value) {
    static_assert(sizeof(T) == sizeof(U));
    return *gabi::at<be<T>>(gabi::ea(&value));
}

// HD: fields used by the verified Picto Box rendering paths.
struct dSnapHD_Texture {
    union {
        struct {
            /*0x0*/ be<u32> mVtable;
            /*0x4*/ be<u32> mDimension;
            /*0x8*/ be<s32> mWidth;
            /*0xC*/ be<s32> mHeight;
            /*0x10*/ be<u32> mDepth;
            /*0x14*/ be<u32> mMipCount;
            /*0x18*/ be<u32> mFormat;
            /*0x1C*/ be<u32> mSamples;
            u8 _pad20[8];
            /*0x28*/ be<u32> mMipData;
            u8 _pad2C[4];
            /*0x30*/ be<u32> mImageData;
            /*0x34*/ be<u32> mTileMode;
            /*0x38*/ be<u32> mAlignment;
            u8 _pad3C[60];
            /*0x78*/ be<u8> mMinWidth;
            /*0x79*/ be<u8> mMinHeight;
            u8 _pad7A[22];
            /*0x90*/ be<u8> mDirty;
            /*0x91*/ be<u8> mFlags;
            u8 _pad92[2];
            /*0x94*/ be<u32> mMode;
            /*0x98*/ be<u32> mAllocator;
            u8 _pad9C[8];
            /*0xA4*/ be<u32> mDepthBuffer;
            u8 _padA8[36];
            /*0xCC*/ be<u32> mDepthMipData;
            /*0xD0*/ be<u32> mColorMipData;
            /*0xD4*/ be<u32> mViewMipData;
            /*0xD8*/ be<u32> mColorImageData;
            /*0xDC*/ be<u32> mViewImageData;
            u8 _padE0[112];
            /*0x150*/ be<u32> mLevel;
            /*0x154*/ be<u32> mSlice;
            /*0x158*/ be<u32> mLayer;
            u8 _pad15C[52];
            /*0x190*/ be<u8> mViewFlags;
            u8 _pad191[7];
        };
        be<u32> mWords[0x198 / 4];
        be<u8> mBytes[0x198];
    };
    bool hasSameDescriptor(const dSnapHD_Texture &other) const {
        return mDimension == other.mDimension && mWidth == other.mWidth &&
               mHeight == other.mHeight && mDepth == other.mDepth && mMipCount == other.mMipCount &&
               mFormat == other.mFormat && mAlignment == other.mAlignment &&
               mTileMode == other.mTileMode && mSamples == other.mSamples;
    }
};
WWHD_SIZE(dSnapHD_Texture, 0x198);
WWHD_OFFSET(dSnapHD_Texture, mVtable, 0x0);
WWHD_OFFSET(dSnapHD_Texture, mDimension, 0x4);
WWHD_OFFSET(dSnapHD_Texture, mWidth, 0x8);
WWHD_OFFSET(dSnapHD_Texture, mHeight, 0xC);
WWHD_OFFSET(dSnapHD_Texture, mDepth, 0x10);
WWHD_OFFSET(dSnapHD_Texture, mMipCount, 0x14);
WWHD_OFFSET(dSnapHD_Texture, mFormat, 0x18);
WWHD_OFFSET(dSnapHD_Texture, mSamples, 0x1C);
WWHD_OFFSET(dSnapHD_Texture, mMipData, 0x28);
WWHD_OFFSET(dSnapHD_Texture, mImageData, 0x30);
WWHD_OFFSET(dSnapHD_Texture, mTileMode, 0x34);
WWHD_OFFSET(dSnapHD_Texture, mAlignment, 0x38);
WWHD_OFFSET(dSnapHD_Texture, mMinWidth, 0x78);
WWHD_OFFSET(dSnapHD_Texture, mMinHeight, 0x79);
WWHD_OFFSET(dSnapHD_Texture, mDirty, 0x90);
WWHD_OFFSET(dSnapHD_Texture, mFlags, 0x91);
WWHD_OFFSET(dSnapHD_Texture, mMode, 0x94);
WWHD_OFFSET(dSnapHD_Texture, mAllocator, 0x98);
WWHD_OFFSET(dSnapHD_Texture, mDepthBuffer, 0xA4);
WWHD_OFFSET(dSnapHD_Texture, mDepthMipData, 0xCC);
WWHD_OFFSET(dSnapHD_Texture, mColorMipData, 0xD0);
WWHD_OFFSET(dSnapHD_Texture, mViewMipData, 0xD4);
WWHD_OFFSET(dSnapHD_Texture, mColorImageData, 0xD8);
WWHD_OFFSET(dSnapHD_Texture, mViewImageData, 0xDC);
WWHD_OFFSET(dSnapHD_Texture, mLevel, 0x150);
WWHD_OFFSET(dSnapHD_Texture, mSlice, 0x154);
WWHD_OFFSET(dSnapHD_Texture, mLayer, 0x158);
WWHD_OFFSET(dSnapHD_Texture, mViewFlags, 0x190);

struct dSnap_CharaData {
    be<s16> offsetX, offsetY, offsetZ;
    be<s16> radius, height;
    be<u16> minRatio;
    be<s16> cullAngle;
    be<u16> minPixels;
    be<u8> figRoom, m11;
};
WWHD_SIZE(dSnap_CharaData, 0x12);
WWHD_OFFSET(dSnap_CharaData, m11, 0x11);
WWHD_OFFSET(dSnap_CharaData, figRoom, 0x10);
WWHD_OFFSET(dSnap_CharaData, minPixels, 0xe);
WWHD_OFFSET(dSnap_CharaData, cullAngle, 0xc);
WWHD_OFFSET(dSnap_CharaData, minRatio, 0xa);
WWHD_OFFSET(dSnap_CharaData, height, 0x8);
WWHD_OFFSET(dSnap_CharaData, radius, 0x6);
WWHD_OFFSET(dSnap_CharaData, offsetZ, 0x4);
WWHD_OFFSET(dSnap_CharaData, offsetY, 0x2);
WWHD_OFFSET(dSnap_CharaData, offsetX, 0x0);

// HD: fields used by the verified Picto Box rendering paths.
struct dSnapHD_Float2 {
    /*0x0*/ be<f32> x;
    /*0x4*/ be<f32> y;
};
WWHD_SIZE(dSnapHD_Float2, 0x8);
WWHD_OFFSET(dSnapHD_Float2, x, 0x0);
WWHD_OFFSET(dSnapHD_Float2, y, 0x4);

// HD: fields used by the verified Picto Box rendering paths.
struct dSnapHD_Color {
    /*0x0*/ be<f32> r;
    /*0x4*/ be<f32> g;
    /*0x8*/ be<f32> b;
    /*0xC*/ be<f32> a;
};
WWHD_SIZE(dSnapHD_Color, 0x10);
WWHD_OFFSET(dSnapHD_Color, r, 0x0);
WWHD_OFFSET(dSnapHD_Color, g, 0x4);
WWHD_OFFSET(dSnapHD_Color, b, 0x8);
WWHD_OFFSET(dSnapHD_Color, a, 0xC);

// HD: fields used by the verified Picto Box rendering paths.
struct dSnapHD_Buffer {
    /*0x0*/ be<f32> mWidth;
    /*0x4*/ be<f32> mHeight;
    /*0x8*/ be<f32> mOriginX;
    /*0xC*/ be<f32> mOriginY;
    /*0x10*/ be<f32> mViewportWidth;
    /*0x14*/ be<f32> mViewportHeight;
    /*0x18*/ be<u32> mVtable;
    /*0x1C*/ be<u32> mpColorTarget;
    u8 _pad20[28];
    /*0x3C*/ be<u32> mpDepthTarget;
};
WWHD_SIZE(dSnapHD_Buffer, 0x40);
WWHD_OFFSET(dSnapHD_Buffer, mWidth, 0x0);
WWHD_OFFSET(dSnapHD_Buffer, mHeight, 0x4);
WWHD_OFFSET(dSnapHD_Buffer, mOriginX, 0x8);
WWHD_OFFSET(dSnapHD_Buffer, mOriginY, 0xC);
WWHD_OFFSET(dSnapHD_Buffer, mViewportWidth, 0x10);
WWHD_OFFSET(dSnapHD_Buffer, mViewportHeight, 0x14);
WWHD_OFFSET(dSnapHD_Buffer, mVtable, 0x18);
WWHD_OFFSET(dSnapHD_Buffer, mpColorTarget, 0x1C);
WWHD_OFFSET(dSnapHD_Buffer, mpDepthTarget, 0x3C);

// HD: fields used by the verified Picto Box rendering paths.
struct dSnapHD_Frame {
    u8 _pad0[8];
    /*0x8*/ be<u32> mpBuffer;
    u8 _padC[8];
    /*0x14*/ be<u32> mpCamera;
    /*0x18*/ be<u32> mpProjection;
};
WWHD_SIZE(dSnapHD_Frame, 0x1C);
WWHD_OFFSET(dSnapHD_Frame, mpBuffer, 0x8);
WWHD_OFFSET(dSnapHD_Frame, mpCamera, 0x14);
WWHD_OFFSET(dSnapHD_Frame, mpProjection, 0x18);

// HD: fields used by the verified Picto Box rendering paths.
struct dSnapHD_Context {
    /*0x0*/ be<u32> mpFrame;
};
WWHD_SIZE(dSnapHD_Context, 0x4);
WWHD_OFFSET(dSnapHD_Context, mpFrame, 0x0);

// HD: fields used by the verified Picto Box rendering paths.
struct dSnapHD_RenderState {
    /*0x0*/ be<u8> mCullEnabled;
    /*0x1*/ be<u8> mBlendEnabled;
    u8 _pad2[2];
    /*0x4*/ be<u32> mDepthFunction;
    u8 _pad8[4];
    /*0xC*/ be<u8> mAlphaTestEnabled;
    u8 _padD[7];
    /*0x14*/ be<u32> mColorMask;
    u8 _pad18[4];
    /*0x1C*/ be<u32> mBlendMode;
    u8 _pad20[36];
    /*0x44*/ be<u8> mDepthTestEnabled;
    /*0x45*/ be<u8> mDepthWriteEnabled;
    /*0x46*/ be<u8> mColorWriteEnabled;
    /*0x47*/ be<u8> mAlphaWriteEnabled;
    u8 _pad48[44];
};
WWHD_SIZE(dSnapHD_RenderState, 0x74);
WWHD_OFFSET(dSnapHD_RenderState, mCullEnabled, 0x0);
WWHD_OFFSET(dSnapHD_RenderState, mBlendEnabled, 0x1);
WWHD_OFFSET(dSnapHD_RenderState, mDepthFunction, 0x4);
WWHD_OFFSET(dSnapHD_RenderState, mAlphaTestEnabled, 0xC);
WWHD_OFFSET(dSnapHD_RenderState, mColorMask, 0x14);
WWHD_OFFSET(dSnapHD_RenderState, mBlendMode, 0x1C);
WWHD_OFFSET(dSnapHD_RenderState, mDepthTestEnabled, 0x44);
WWHD_OFFSET(dSnapHD_RenderState, mDepthWriteEnabled, 0x45);
WWHD_OFFSET(dSnapHD_RenderState, mColorWriteEnabled, 0x46);
WWHD_OFFSET(dSnapHD_RenderState, mAlphaWriteEnabled, 0x47);

// HD: fields used by the verified Picto Box rendering paths.
struct dSnapHD_PostProcess {
    u8 _pad0[464];
    /*0x1D0*/ be<u32> mpEffect;
    /*0x1D4*/ be<s32> mEffectCount;
    u8 _pad1D8[900];
    /*0x55C*/ be<u32> mpTexture;
    u8 _pad560[860];
    /*0x8BC*/ be<u8> mEnabled;
    u8 _pad8BD[355];
    /*0xA20*/ be<u8> mFallbackEnabled;
    u8 _padA21[3];
};
WWHD_SIZE(dSnapHD_PostProcess, 0xA24);
WWHD_OFFSET(dSnapHD_PostProcess, mpEffect, 0x1D0);
WWHD_OFFSET(dSnapHD_PostProcess, mEffectCount, 0x1D4);
WWHD_OFFSET(dSnapHD_PostProcess, mpTexture, 0x55C);
WWHD_OFFSET(dSnapHD_PostProcess, mEnabled, 0x8BC);
WWHD_OFFSET(dSnapHD_PostProcess, mFallbackEnabled, 0xA20);

// HD: fields used by the verified Picto Box rendering paths.
struct dSnapHD_SaveView {
    u8 _pad0[100];
    /*0x64*/ be<u8> mStageType;
};
WWHD_SIZE(dSnapHD_SaveView, 0x65);
WWHD_OFFSET(dSnapHD_SaveView, mStageType, 0x64);

// HD: fields used by the verified Picto Box rendering paths.
struct dSnapHD_DrawFlags {
    /*0x0*/ be<u32> mFlags;
    /*0x4*/ be<u32> mSecondaryFlags;
};
WWHD_SIZE(dSnapHD_DrawFlags, 0x8);
WWHD_OFFSET(dSnapHD_DrawFlags, mFlags, 0x0);
WWHD_OFFSET(dSnapHD_DrawFlags, mSecondaryFlags, 0x4);

// HD: fields used by the verified Picto Box rendering paths.
struct dSnapHD_Graphics {
    u8 _pad0[36];
    /*0x24*/ be<u32> mpAlternateTexture;
    /*0x28*/ be<u32> mpTexture;
};
WWHD_SIZE(dSnapHD_Graphics, 0x2C);
WWHD_OFFSET(dSnapHD_Graphics, mpAlternateTexture, 0x24);
WWHD_OFFSET(dSnapHD_Graphics, mpTexture, 0x28);

// HD: fields used by the verified Picto Box rendering paths.
struct dSnapHD_Service {
    u8 _pad0[500];
    /*0x1F4*/ be<u32> mVtable;
    u8 _pad1F8[2668];
    /*0xC64*/ be<u32> mAlternateVtable;
};
WWHD_SIZE(dSnapHD_Service, 0xC68);
WWHD_OFFSET(dSnapHD_Service, mVtable, 0x1F4);
WWHD_OFFSET(dSnapHD_Service, mAlternateVtable, 0xC64);

// HD: fields used by the verified Picto Box rendering paths.
struct dSnapHD_Vtable {
    u8 _pad0[12];
    /*0xC*/ be<u32> mIsTexture;
    u8 _pad10[12];
    /*0x1C*/ be<u32> mDestructor;
    u8 _pad20[4];
    /*0x24*/ be<u32> mUpdateCamera;
    u8 _pad28[44];
    /*0x54*/ be<u32> mSetTexture;
};
WWHD_SIZE(dSnapHD_Vtable, 0x58);
WWHD_OFFSET(dSnapHD_Vtable, mIsTexture, 0xC);
WWHD_OFFSET(dSnapHD_Vtable, mDestructor, 0x1C);
WWHD_OFFSET(dSnapHD_Vtable, mUpdateCamera, 0x24);
WWHD_OFFSET(dSnapHD_Vtable, mSetTexture, 0x54);

// HD: fields used by the verified Picto Box rendering paths.
struct dSnapHD_Camera {
    be<u32> mMatrixWords[12];
    /*0x30*/ be<u32> mVtable;
    /*0x34*/ be<f32> mEyeX;
    /*0x38*/ be<f32> mEyeY;
    /*0x3C*/ be<f32> mEyeZ;
    /*0x40*/ be<f32> mAtX;
    /*0x44*/ be<f32> mAtY;
    /*0x48*/ be<f32> mAtZ;
    /*0x4C*/ be<f32> mUpX;
    /*0x50*/ be<f32> mUpY;
    /*0x54*/ be<f32> mUpZ;
};
WWHD_SIZE(dSnapHD_Camera, 0x58);
WWHD_OFFSET(dSnapHD_Camera, mVtable, 0x30);
WWHD_OFFSET(dSnapHD_Camera, mEyeX, 0x34);
WWHD_OFFSET(dSnapHD_Camera, mEyeY, 0x38);
WWHD_OFFSET(dSnapHD_Camera, mEyeZ, 0x3C);
WWHD_OFFSET(dSnapHD_Camera, mAtX, 0x40);
WWHD_OFFSET(dSnapHD_Camera, mAtY, 0x44);
WWHD_OFFSET(dSnapHD_Camera, mAtZ, 0x48);
WWHD_OFFSET(dSnapHD_Camera, mUpX, 0x4C);
WWHD_OFFSET(dSnapHD_Camera, mUpY, 0x50);
WWHD_OFFSET(dSnapHD_Camera, mUpZ, 0x54);

struct dSnapHD_Pixel {
    be<u32> r, g, b, a;
};
WWHD_SIZE(dSnapHD_Pixel, 16);

struct dSnapHD_Mtx44 {
    be<f32> m[4][4];
};
WWHD_SIZE(dSnapHD_Mtx44, 64);

using dSnapJudgeMethod = u32 (dSnap_packet::*)();
struct dSnap_JudgeEntry {
    gabi::ptmf<dSnapJudgeMethod> fn;
};
WWHD_SIZE(dSnap_JudgeEntry, 8);

template <class T, class U> inline const be<T> &snapFieldBits(const be<U> &value) {
    static_assert(sizeof(T) == sizeof(U));
    return *gabi::at<const be<T>>(gabi::ea(&value));
}
struct dSnapHD_ProjectionView {
    u8 _00[0x90];
    be<u32> mVtable;
};
WWHD_OFFSET(dSnapHD_ProjectionView, mVtable, 0x90);
WWHD_SIZE(dSnapHD_ProjectionView, 0x94);
template <class T> inline void dSnapHD_destroy(T *object) {
    const dSnapHD_Vtable *vtable = gabi::at<dSnapHD_Vtable>(object->mVtable);
    gabi::call_ptr<void>(vtable->mDestructor, object, 3);
}

struct dSnapHD_IdentityWords {
    be<u32> mWords[12];
};
WWHD_SIZE(dSnapHD_IdentityWords, 48);

struct dSnapHD_GameDrawLists {
    u8 _00[0x5D78];
    be<u32> mpOpaList, mpXluList;
    u8 _5D80[0x1C];
    be<u32> mpSnapList;
};
WWHD_OFFSET(dSnapHD_GameDrawLists, mpSnapList, 0x5D9C);
WWHD_SIZE(dSnapHD_GameDrawLists, 0x5DA0);
