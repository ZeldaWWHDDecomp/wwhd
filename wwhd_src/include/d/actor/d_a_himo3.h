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
    gptr<be<u8>> mTextPtr;
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
    /* 0x02 */ be<s16> mAlphaModelType;  /* was m06: read only by tww: daHimo3_Draw picks the glow's alpha model, 0 one sphere, 1 two spheres (both in the mAlphaColorR / G / B colour), 2 the spot model; WWHD writes it only in __sinit_d_a_himo3_cpp 02174F64 (0) and has no alpha model (2026-11-11 rule: tww's one role) */
    /* 0x04 */ be<f32> mEndScaleType1;  /* was m08: himo3_control (inlined in daHimo3_Execute 02172A00): the end model's scale for type 1; __sinit_d_a_himo3_cpp 02174F64 0.75 (tww the same) */
    /* 0x08 */ be<f32> mEndScale;  /* was m0C: himo3_control (inlined in daHimo3_Execute 02172A00): the end model's scale for the other types, which also turn it by -0x4000 about x; __sinit_d_a_himo3_cpp 02174F64 0.5 (tww the same) */
    /* 0x0C */ be<s16> mAlphaColorR;  /* was m10: read only by tww: daHimo3_Draw's alpha model colour r (dComIfGd_setAlphaModelColor); WWHD writes it only in __sinit_d_a_himo3_cpp 02174F64 (0xEB) (2026-11-11 rule: tww's one role) */
    /* 0x0E */ be<s16> mAlphaColorG;  /* was m12: read only by tww: daHimo3_Draw's alpha model colour g; WWHD writes it only in __sinit_d_a_himo3_cpp 02174F64 (0x7D) (2026-11-11 rule: tww's one role) */
    /* 0x10 */ be<s16> mAlphaColorB;  /* was m14: read only by tww: daHimo3_Draw's alpha model colour b; WWHD writes it only in __sinit_d_a_himo3_cpp 02174F64 (0) (2026-11-11 rule: tww's one role) */
    /* 0x12 */ u8 _12[2];
    /* 0x14 */ be<f32> mGlowMtxScale;  /* was m18: daHimo3_Execute 02172A00 copies it into the actor's mGlowScale every frame (the glow matrix scale); __sinit_d_a_himo3_cpp 02174F64 5.0 (tww m1870 = it) */
    /* 0x18 */ be<u32> __vtbl;
};
WWHD_SIZE(himo3HIO_c, 0x1C);

struct himo3_s {
    /* 0x00 */ cXyz mPos;  /* was m00: himo3_control (inlined in daHimo3_Execute 02172A00) moves each rope segment from the previous one by the gravity / pull offset and feeds the collision spheres and line from it; [0] = current.pos (tww the same; as himo2_s::mPos) */
    /* 0x0C */ cXyz mSpeed;  /* was m0C: himo3_control (in daHimo3_Execute 02172A00) adds it to the next step and stores (new - old) * a factor (0 in WWHD, a REG value in tww) (tww the same) */
};
WWHD_SIZE(himo3_s, 0x18);

struct h3_ga_s {
    /* 0x00 */ gptr<J3DModel> mpModel;
    /* 0x04 */ cXyz mPos;
    /* 0x10 */ cXyz mTargetPos;  /* was m10: ga_move (inlined in daHimo3_Execute 02172A00): a new random point within 150 / 100 / 150 of the rope end (mEndPos) whenever mRetargetTimer runs out; the moth turns toward it (tww the same) */
    /* 0x1C */ csXyz mAngle;  /* was m1C: ga_move: yaw / pitch eased toward mTargetPos by 0x1000 steps, the direction the moth flies; daHimo3_Draw 021727D4 (ga_draw inlined) turns its model by it (tww the same) */
    /* 0x22 */ u8 _22[2];
    /* 0x24 */ be<f32> mScale;  /* was m24: daHimo3_Create 02174730 (useHeapInit inlined): random 0.3..0.6; daHimo3_Draw (ga_draw) scales the moth model by it (tww the same) */
    /* 0x28 */ be<f32> mFlapScale;  /* was m28: ga_move: sin(mFlapPhase); daHimo3_Draw (ga_draw) multiplies the model's y scale by it, the wing flap (tww the same) */
    /* 0x2C */ be<s16> mFlapPhase;  /* was m2C: ga_move adds 0x3E00 per frame and takes mFlapScale = sin(it); daHimo3_Create (useHeapInit) random start (tww the same) */
    /* 0x2E */ be<u8> mActive;  /* was m2E: daHimo3_Create (useHeapInit) sets it for type 0; ga_move moves and daHimo3_Draw (ga_draw) draws the moth only while it is set (tww the same) */
    /* 0x2F */ be<u8> mRetargetTimer;  /* was m2F: ga_move counts it down and, at 0, sets a random 0..10 and picks a new mTargetPos (tww the same) */
};
WWHD_SIZE(h3_ga_s, 0x30);

/* GameCube +0x11C up to mSegmentNum; HD inserts a mDoExt_J3DModelPacketS (0xB0) after mSegmentNum and the
 * line material grew (0x3C -> 0x18C); m20F8 (shadow size) is gone. Size 0x2634. */
struct himo3_class : fopAc_ac_c {
    /* 0x03AC */ request_of_phase_process_class mPhase;
    /* 0x03B4 */ be<u8> mType;  /* was m0298: daHimo3_Create 02174730: param & 0xFF (0xFF -> 0); 0 = torch (fire, light, ga, model hook_bmd[0]), 1 = Link's hook model (hook_bmd[1] from Link), 2..4 Himo3 models, 0xF = the Bgn rope (no end model, Bgn texture, cylinder, 200 segments); daHimo3_Draw 021727D4 / daHimo3_Execute 02172A00 branch on it */
    /* 0x03B5 */ be<u8> mPathSpeed;  /* was m0299: daHimo3_Create 02174730: param >> 16; path_move (in daHimo3_Execute 02172A00) eases speedF to it when the path point's arg3 is 0 or 0xFF */
    /* 0x03B6 */ be<u8> mPathNo;  /* was m029A: daHimo3_Create 02174730: param >> 24 (0xFF for type 0xF); dPath_GetRoomPath(it, room) when not 0xFF */
    /* 0x03B7 */ be<s8> mPathOn;  /* was m029B: daHimo3_Create 02174730: mPathNo + 1 when a path was found; daHimo3_Execute 02172A00 runs path_move only while it is not 0 */
    /* 0x03B8 */ be<s8> mPathPntNo;  /* was m029C: path_move (in daHimo3_Execute 02172A00): the current path point, + mPathDir per point, wraps (closed path) or turns at the ends; mPathTargetPos and the point's arg3 speed come from it */
    /* 0x03B9 */ be<s8> mPathDir;  /* was m029D: path_move (in daHimo3_Execute 02172A00): step of mPathPntNo, 1 / -1 (turned at the ends of an open path); daHimo3_Create 02174730 sets 1 */
    /* 0x03BA */ u8 _3BA[2];
    /* 0x03BC */ gptr<himo3HD::Path> ppd;
    /* 0x03C0 */ be<s8> mPathMode;  /* was m02A4: path_move (in daHimo3_Execute 02172A00): switch: 0 takes the next point (sets mPathTargetPos, then 1), 1 moves toward it (back to 0 when nearer than 2 * speedF) */
    /* 0x03C1 */ u8 _3C1[3];
    /* 0x03C4 */ cXyz mPathTargetPos;  /* was m02A8: path_move (in daHimo3_Execute 02172A00): position of path point mPathPntNo; the actor moves toward it */
    /* 0x03D0 */ be<s16> mTimers[3];  /* was m02B4: daHimo3_Execute 02172A00 (type 0): 3 timers counted down every frame; [0] reloads 5..15 and picks a new mGlowSizeTarget, [1] reloads 3..9 */
    /* 0x03D6 */ be<s16> mFrameCounter;  /* was m02BA: daHimo3_Execute 02172A00: + 1 every frame; himo3_control (in daHimo3_Execute 02172A00) uses it as the phase of the wind sway (it * 500) and to space the 5 collision spheres along the rope */
    /* 0x03D8 */ be<s16> mSphOffTimer;  /* was m02BC: himo3_control (in daHimo3_Execute 02172A00): 0x1E on release (param 3), counted down by daHimo3_Execute 02172A00; the rope spheres get radius 30 only while nothing hangs and it is 0, else -200 */
    /* 0x03DA */ be<u8> mHangType;  /* was m02BE: himo3_control (in daHimo3_Execute 02172A00): 0 free, 1 the player hangs (param 1: hand position, shape_angle), 2 an actor hangs (param 2: mActorHangPos / mActorHangAngle); 0 on param 3; path_move stops while it is not 0 */
    /* 0x03DB */ u8 _3DB;
    /* 0x03DC */ himo3_s mSegments[200];  /* was m02C0: himo3_control (in daHimo3_Execute 02172A00): the 200 rope segments (himo3_s), [0] = current.pos, simulated up to mSegmentNum and copied into the line material; daHimo3_Create 02174730 clears them */
    /* 0x169C */ himo3HD::LineMaterial mLineMat; // HD: enlarged line material.
    /* 0x1828 */ be<s32> mSegmentNum;  /* was m15C0: daHimo3_Create 02174730: (param >> 8) & 0xFF, 200 for type 0xF (error above 200); himo3_control (in daHimo3_Execute 02172A00) loops over it; daHimo3_Draw 021727D4 updates the line with it as the point count */
    /* 0x182C */ u8 mPacket[0xB0]; /* HD: mDoExt_J3DModelPacketS */
    /* 0x18DC */ cXyz mOldHangPos;  /* was m15C4: himo3_control (in daHimo3_Execute 02172A00): the hang point of the previous frame (-23535 y while nothing hangs, then the first hang point); hang point - it drives mSwingX / mSwingZ */
    /* 0x18E8 */ be<f32> mSwingZ;  /* was m15D0: himo3_control (in daHimo3_Execute 02172A00): while hanging eased to -10 * the hang point's local z movement, else a damped spring with mSwingVelZ; local z of the rope's bend offset (spA0.z) */
    /* 0x18EC */ be<f32> mSwingVelZ;  /* was m15D4: himo3_control (in daHimo3_Execute 02172A00): while free, added to mSwingZ every frame and reduced by mSwingZ * 0.003 (spring) */
    /* 0x18F0 */ be<f32> mSwingX;  /* was m15D8: himo3_control (in daHimo3_Execute 02172A00): as mSwingZ for the local x movement; local x of the rope's bend offset (spA0.x) */
    /* 0x18F4 */ be<f32> mSwingVelX;  /* was m15DC: himo3_control (in daHimo3_Execute 02172A00): as mSwingVelZ for mSwingX */
    /* 0x18F8 */ be<f32> mLineFollowRate;  /* was m15E0: himo3_control (in daHimo3_Execute 02172A00): the line points follow the segments at 100 * it per frame (copied when >= 0.999), eased to 1; 0 on release, 1 when a hang starts; daHimo3_Create 02174730 sets 1 */
    /* 0x18FC */ be<f32> mSwingStepRate;  /* was m15E4: himo3_control (in daHimo3_Execute 02172A00): step of the mSwingX / mSwingZ ease (10 * it); -> 1 while the hang point moves more than 1, else -> 0.1 */
    /* 0x1900 */ be<f32> mWindSway;  /* was m15E8: himo3_control (in daHimo3_Execute 02172A00): eased to the wind power (half while hanging); amplitude of the sway along the wind direction (sin(mFrameCounter * 500) * 100 * it) */
    /* 0x1904 */ u8 _1904[4];
    /* 0x1908 */ be<f32> mGustSway;  /* was m15F0: himo3_control (in daHimo3_Execute 02172A00): eased to mGustSwayTarget; amplitude of the sway along mGustAngleY (sin(mGustPhase * 500) * 100 * it) */
    /* 0x190C */ be<f32> mGustSwayTarget;  /* was m15F4: daHimo3_Execute 02172A00: 1 when mSph is hit by an AT_TYPE_WIND attack; himo3_control (in daHimo3_Execute 02172A00) eases it back to 0 and mGustSway to it */
    /* 0x1910 */ be<s16> mGustAngleY;  /* was m15F8: daHimo3_Execute 02172A00: the player's shape_angle.y at the wind hit; himo3_control (in daHimo3_Execute 02172A00) sways the rope in that direction */
    /* 0x1912 */ be<s16> mGustPhase;  /* was m15FA: himo3_control (in daHimo3_Execute 02172A00): + 1 per frame while mGustSway > 0.01, else 0; phase of the gust sway (it * 500) */
    /* 0x1914 */ be<f32> mPlayerMoveLength;  /* was m15FC: tww accessor himo3_class::getPlayerMoveLength() returns it; daHimo3_Create 02174730: mSegmentNum * scale - 50 (- 50 more for type 0); Link's procRopeSwing_init 0241D040 / procRopeDown_init 02436780 limit the climb with it */
    /* 0x1918 */ LIGHT_INFLUENCE mFireLight;  /* was m1600: daHimo3_Execute 02172A00 (type 0): LIGHT_INFLUENCE at mEndPos, colour 600/400/120, power 150 * mFireLightRate; daHimo3_Create 02174730 dKy_plight_set, daHimo3_Delete 021743F0 dKy_plight_cut */
    /* 0x193C */ be<f32> mFireLightRate;  /* was m1620: daHimo3_Execute 02172A00: 1 when the torch fire emitter is created, eased to 1..1.2 (flicker) while it exists, 0 without it; mFireLight power = 150 * it */
    /* 0x1940 */ cXyz mEndPos;  /* was m1624: daHimo3_Execute 02172A00: MtxPosition of the end model's matrix (offset, scale); centre of mSph, the torch fire and heat haze, mFireLight; mAcch position (daHimo3_Create 02174730); ga_move (in daHimo3_Execute 02172A00) flies around it */
    /* 0x194C */ cXyz mOldEndPos;  /* was m1630: daHimo3_Execute 02172A00: mEndPos of the previous frame (copied at the end); the fire direction is (mEndPos - it) * -0.03; daHimo3_Create 02174730 mAcch old position */
    /* 0x1958 */ dBgS_AcchCir mAcchCir;
    /* 0x1998 */ dBgS_ObjAcch mAcch;
    /* 0x1B5C */ Mtx34 mGlowMtx;  /* was m1840: daHimo3_Execute 02172A00: end model matrix moved to the flame (10, -140, -15 for type 0) and scaled by mGlowScale; HD has no reader (the GameCube daHimo3_Draw passes it to dComIfGd_setAlphaModel / setSpotModel, tww d_a_himo3.cpp) */
    /* 0x1B8C */ be<f32> mGlowScale;  /* was m1870: daHimo3_Execute 02172A00: himo3HIO.mGlowMtxScale every frame; scales mGlowMtx */
    /* 0x1B90 */ u8 _1B90[4];
    /* 0x1B94 */ be<f32> mGlowSize;  /* was m1878: daHimo3_Execute 02172A00: eased to mGlowSizeTarget; HD has no reader (the GameCube daHimo3_Draw passes it to dComIfGd_setAlphaModel as the size, tww d_a_himo3.cpp) */
    /* 0x1B98 */ be<f32> mGlowSizeTarget;  /* was m187C: daHimo3_Execute 02172A00: random 4..20 each time mTimers[0] runs out; mGlowSize eases to it */
    /* 0x1B9C */ gptr<J3DModel> mpModel;
    /* 0x1BA0 */ dCcD_Stts mStts;
    /* 0x1BDC */ dCcD_Sph mSphs[5];
    /* 0x21B8 */ dCcD_Sph mSph;
    /* 0x22E4 */ dCcD_Cyl mCyl;
    /* 0x2414 */ dPa_followEcallBack mFireEcallBack;  /* was m20FC: daHimo3_Execute 02172A00: follow callback of the torch fire (dPa_name ID_AK_JN_TORCH at mEndPos); its emitter gets the direction and scale; himo3_class_ct 0217447C constructs it, daHimo3_Delete 021743F0 removes it */
    /* 0x2428 */ be<u8> mHioOwner;  /* was m2110: daHimo3_Create 02174730: written (0) when this actor created the l_HIO child; daHimo3_Delete 021743F0 deletes the HIO child only while it is not 0 */
    /* 0x2429 */ be<u8> m2111;
    /* 0x242A */ u8 _242A[2];
    /* 0x242C */ h3_ga_s mGa[1];  /* was m2114: daHimo3_Draw 021727D4 / ga_move (in daHimo3_Execute 02172A00): the h3_ga_s (model Himo3 H3_GA, type 0 only, created in useHeapInit (in daHimo3_Create 02174730)) that flies around mEndPos; named after its type h3_ga_s and tww ga_draw / ga_move */
    /* 0x245C */ dKy_tevstr_c m2144;
    /* 0x2624 */ cXyz mActorHangPos;  /* was m21F4: setActorHang 02174F44 (tww himo3_class::setActorHang(cXyz, s16)) stores the position; himo3_control (in daHimo3_Execute 02172A00) uses it as the hang point for mHangType 2 */
    /* 0x2630 */ be<s16> mActorHangAngle;  /* was m2200: setActorHang 02174F44 stores the angle; himo3_control (in daHimo3_Execute 02172A00) turns the rope (current.angle.y) to it for mHangType 2 */
    /* 0x2632 */ u8 _2632[2];
};
WWHD_OFFSET(himo3_class, mSegments, 0x3DC);
WWHD_OFFSET(himo3_class, mSegmentNum, 0x1828);
WWHD_OFFSET(himo3_class, mOldHangPos, 0x18DC);
WWHD_OFFSET(himo3_class, mFireLight, 0x1918);
WWHD_OFFSET(himo3_class, mAcch, 0x1998);
WWHD_OFFSET(himo3_class, mpModel, 0x1B9C);
WWHD_OFFSET(himo3_class, mCyl, 0x22E4);
WWHD_OFFSET(himo3_class, mGa, 0x242C);
WWHD_OFFSET(himo3_class, mActorHangPos, 0x2624);
WWHD_SIZE(himo3_class, 0x2634);

#endif
