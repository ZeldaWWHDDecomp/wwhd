#ifndef D_A_HIMO3_H
#define D_A_HIMO3_H

#include "bindings.h"

namespace himo3HD {
struct SegmentCount {
    be<u16> high, low;
};
WWHD_SIZE(SegmentCount, 4);
WWHD_OFFSET(SegmentCount, low, 2);
struct StringVtable {
    u8 padding[0x14];
    gfn assureTermination;
};
struct String {
    gptr<be<u8>> mStringTop;
    gptr<StringVtable> __vtbl;
};
WWHD_SIZE(String, 8);
WWHD_OFFSET(String, __vtbl, 4);
struct Emitter {
    u8 padding[0x28];
    cXyz mDirection;
    u8 padding2[0x204];
    cXyz mGlobalParticleScale;
};
WWHD_OFFSET(Emitter, mDirection, 0x28);
WWHD_OFFSET(Emitter, mGlobalParticleScale, 0x238);
WWHD_SIZE(Emitter, 0x244);
struct LightBlock {
    be<f32> positionAndDirection[6];
    be<u8> color[4];
    be<s16> attenuation[4];
    be<f32> factors[8];
};
WWHD_SIZE(LightBlock, 0x44);
struct TevLightBlocks {
    LightBlock first;
    u8 padding[0x7C];
    LightBlock second;
    u8 padding2[0x40];
    LightBlock third;
};
WWHD_OFFSET(TevLightBlocks, second, 0xC0);
WWHD_OFFSET(TevLightBlocks, third, 0x144);
struct SaveBlock {
    u8 padding[0x644];
    dSv_event_c mEvent;
};
WWHD_OFFSET(SaveBlock, mEvent, 0x644);

struct PathPoint {
    be<u8> mArg0, mArg1, mArg2, mArg3;
    cXyz m_position;
};
struct Path {
    union {
        be<u16> m_num;
        struct {
            be<u8> high, low;
        } countBytes;
    };
    be<u16> m_nextID;
    be<u8> mArg0, m_closed;
    u8 padding[2];
    gptr<PathPoint> m_points;
};
WWHD_SIZE(PathPoint, 0x10);
WWHD_OFFSET(PathPoint, m_position, 4);
WWHD_SIZE(Path, 0xC);
WWHD_OFFSET(Path, m_nextID, 2);
WWHD_OFFSET(Path, m_closed, 5);
WWHD_OFFSET(Path, m_points, 8);
struct Player : fopAc_ac_c {
    u8 padding[0x14];
    be<u32> mStatus;
    u8 padding2[0x2C];
    cXyz mLeftHandPos;
    cXyz mRightHandPos;
};
WWHD_OFFSET(Player, mStatus, 0x3C0);
WWHD_OFFSET(Player, mLeftHandPos, 0x3F0);
WWHD_OFFSET(Player, mRightHandPos, 0x3FC);
WWHD_SIZE(Player, 0x408);
struct LineVtable {
    u8 padding[0x14];
    gfn getMaterialId;
};
WWHD_OFFSET(LineVtable, getMaterialId, 0x14);
struct LineMaterial {
    u8 padding[0x130];
    gptr<LineVtable> __vtbl;
    u8 padding2[0x50];
    gptr<gptr<cXyz>> mPositions;
    u8 padding3[4];
};
WWHD_OFFSET(LineMaterial, __vtbl, 0x130);
WWHD_OFFSET(LineMaterial, mPositions, 0x184);
WWHD_SIZE(LineMaterial, 0x18C);
} // namespace himo3HD

enum {
    dRes_INDEX_HIMO3_BMD_H3_GA_e = 3,
    dRes_INDEX_BGN_BTI_NOT_CUT1_e = 0x2E,
    dRes_INDEX_ALWAYS_BTI_ROPE_e = 0x7E,
};
enum { AT_TYPE_WIND = 0x00200000 };
enum { dPa_name_ID_AK_JN_TORCH = 0x1EA, dPa_name_ID_AK_JP_O_KAGEROU00 = 0x4004 };

// HD: actor base, line material and model packet enlarge the GameCube layout.
/* himo3HIO_c (HD: vtable after the members) */
struct himo3HIO_c {
    /* 0x00 */ be<s8> mNo;
    /* 0x01 */ u8 _01;
    /* 0x02 */ be<s16> m06;
    /* 0x04 */ be<f32> m08;
    /* 0x08 */ be<f32> m0C;
    /* 0x0C */ be<s16> m10;
    /* 0x0E */ be<s16> m12;
    /* 0x10 */ be<s16> m14;
    /* 0x12 */ u8 _12[2];
    /* 0x14 */ be<f32> m18;
    /* 0x18 */ be<u32> __vtbl;
};
WWHD_SIZE(himo3HIO_c, 0x1C);

struct himo3_s {
    /* 0x00 */ cXyz m00;
    /* 0x0C */ cXyz m0C;
};
WWHD_SIZE(himo3_s, 0x18);

struct h3_ga_s {
    /* 0x00 */ gptr<J3DModel> mpModel;
    /* 0x04 */ cXyz mPos;
    /* 0x10 */ cXyz m10;
    /* 0x1C */ csXyz m1C;
    /* 0x22 */ u8 _22[2];
    /* 0x24 */ be<f32> m24;
    /* 0x28 */ be<f32> m28;
    /* 0x2C */ be<s16> m2C;
    /* 0x2E */ be<u8> m2E;
    /* 0x2F */ be<u8> m2F;
};
WWHD_SIZE(h3_ga_s, 0x30);

/* GameCube +0x11C up to m15C0; HD inserts a mDoExt_J3DModelPacketS (0xB0) after m15C0 and the
 * line material grew (0x3C -> 0x18C); m20F8 (shadow size) is gone. Size 0x2634. */
struct himo3_class : fopAc_ac_c {
    /* 0x03AC */ request_of_phase_process_class mPhase;
    /* 0x03B4 */ be<u8> m0298;
    /* 0x03B5 */ be<u8> m0299;
    /* 0x03B6 */ be<u8> m029A;
    /* 0x03B7 */ be<s8> m029B;
    /* 0x03B8 */ be<s8> m029C;
    /* 0x03B9 */ be<s8> m029D;
    /* 0x03BA */ u8 _3BA[2];
    /* 0x03BC */ gptr<himo3HD::Path> ppd;
    /* 0x03C0 */ be<s8> m02A4;
    /* 0x03C1 */ u8 _3C1[3];
    /* 0x03C4 */ cXyz m02A8;
    /* 0x03D0 */ be<s16> m02B4[3];
    /* 0x03D6 */ be<s16> m02BA;
    /* 0x03D8 */ be<s16> m02BC;
    /* 0x03DA */ be<u8> m02BE;
    /* 0x03DB */ u8 _3DB;
    /* 0x03DC */ himo3_s m02C0[200];
    /* 0x169C */ himo3HD::LineMaterial mLineMat; // HD: enlarged line material.
    /* 0x1828 */ be<s32> m15C0;
    /* 0x182C */ u8 mPacket[0xB0]; /* HD: mDoExt_J3DModelPacketS */
    /* 0x18DC */ cXyz m15C4;
    /* 0x18E8 */ be<f32> m15D0;
    /* 0x18EC */ be<f32> m15D4;
    /* 0x18F0 */ be<f32> m15D8;
    /* 0x18F4 */ be<f32> m15DC;
    /* 0x18F8 */ be<f32> m15E0;
    /* 0x18FC */ be<f32> m15E4;
    /* 0x1900 */ be<f32> m15E8;
    /* 0x1904 */ u8 _1904[4];
    /* 0x1908 */ be<f32> m15F0;
    /* 0x190C */ be<f32> m15F4;
    /* 0x1910 */ be<s16> m15F8;
    /* 0x1912 */ be<s16> m15FA;
    /* 0x1914 */ be<f32> m15FC;
    /* 0x1918 */ LIGHT_INFLUENCE m1600;
    /* 0x193C */ be<f32> m1620;
    /* 0x1940 */ cXyz m1624;
    /* 0x194C */ cXyz m1630;
    /* 0x1958 */ dBgS_AcchCir mAcchCir;
    /* 0x1998 */ dBgS_ObjAcch mAcch;
    /* 0x1B5C */ Mtx34 m1840;
    /* 0x1B8C */ be<f32> m1870;
    /* 0x1B90 */ u8 _1B90[4];
    /* 0x1B94 */ be<f32> m1878;
    /* 0x1B98 */ be<f32> m187C;
    /* 0x1B9C */ gptr<J3DModel> mpModel;
    /* 0x1BA0 */ dCcD_Stts mStts;
    /* 0x1BDC */ dCcD_Sph mSphs[5];
    /* 0x21B8 */ dCcD_Sph mSph;
    /* 0x22E4 */ dCcD_Cyl mCyl;
    /* 0x2414 */ dPa_followEcallBack m20FC;
    /* 0x2428 */ be<u8> m2110;
    /* 0x2429 */ be<u8> m2111;
    /* 0x242A */ u8 _242A[2];
    /* 0x242C */ h3_ga_s m2114[1];
    /* 0x245C */ dKy_tevstr_c m2144;
    /* 0x2624 */ cXyz m21F4;
    /* 0x2630 */ be<s16> m2200;
    /* 0x2632 */ u8 _2632[2];
};
WWHD_OFFSET(himo3_class, m02C0, 0x3DC);
WWHD_OFFSET(himo3_class, m15C0, 0x1828);
WWHD_OFFSET(himo3_class, m15C4, 0x18DC);
WWHD_OFFSET(himo3_class, m1600, 0x1918);
WWHD_OFFSET(himo3_class, mAcch, 0x1998);
WWHD_OFFSET(himo3_class, mpModel, 0x1B9C);
WWHD_OFFSET(himo3_class, mCyl, 0x22E4);
WWHD_OFFSET(himo3_class, m2114, 0x242C);
WWHD_OFFSET(himo3_class, m21F4, 0x2624);
WWHD_SIZE(himo3_class, 0x2634);

#endif
