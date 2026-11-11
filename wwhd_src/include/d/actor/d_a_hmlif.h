/* Tower of the Gods glowing platform. Private decomp-derived code.
 * HD: actor/moving-background prefix grows to 0x3E0; collider sizes retain GC layout.
 * Tail names are the corresponding zeldaret/tww members. */
#pragma once
#include "d/d_cc_d.h"
#include "m_Do/m_Do_ext.h"
namespace hmlifHD {
struct SaveBlock {
    u8 header[0x20];
    u8 save;
};
struct HeaderZeros {
    be<u32> first, second, third, fourth;
};
struct DestructorRecord {
    be<u32> next, destructor, object;
};
struct DrawLists {
    u8 prefix[0x5D70];
    be<u32> bgOpa, bgXlu, opa, xlu;
};
WWHD_OFFSET(SaveBlock, save, 0x20);
WWHD_SIZE(HeaderZeros, 0x10);
WWHD_SIZE(DestructorRecord, 0xC);
WWHD_OFFSET(DrawLists, bgOpa, 0x5D70);
WWHD_SIZE(DrawLists, 0x5D80);
struct SphereSource {
    u8 data[0x40];
};
WWHD_SIZE(SphereSource, 0x40);
struct CullBox {
    be<f32> minX, minY, minZ, maxX, maxY, maxZ;
};
WWHD_SIZE(CullBox, 0x18);
struct Point {
    be<u8> mArg0, mArg1, mArg2, mArg3;
    cXyz m_position;
};
struct Path {
    be<u16> m_num, m_nextID;
    be<u8> mArg0, m_closed, mArg2, mArg3;
    gptr<Point> m_points;
};
WWHD_OFFSET(Point, m_position, 4);
WWHD_SIZE(Point, 0x10);
WWHD_OFFSET(Path, m_points, 8);
WWHD_SIZE(Path, 0xC);
} // namespace hmlifHD
class daHmlif_c : public dBgS_MoveBgActor {
  public:
    enum MoveState { MOVE_INIT, MOVE_ACCELERATE, MOVE_CRUISE, MOVE_DECELERATE };
    s32 Delete();
    s32 daHmlifDelete();
    s32 CreateHeap();
    s32 Create();
    s32 daHmlifCreate();
    void set_mtx();
    s32 Execute(be<u32>* o_mtx);
    void move();
    void set_se();
    void anim_play();
    void check_col();
    void lift_smooth_move();
    void lift_normal_move();
    void set_next_pnt();
    s32 setNextPath();
    u32 chkSwitchPathType();
    s32 Draw();
    /* 0x3E0 */ request_of_phase_process_class mPhase;
    /* 0x3E8 */ gptr<J3DModel> mpModel;
    /* 0x3EC */ gptr<mDoExt_btpAnm> mpBtp;
    /* 0x3F0 */ gptr<mDoExt_brkAnm> mpBrk;
    /* 0x3F4 */ dCcD_Sph mSph;
    /* 0x520 */ dCcD_Stts mStts;
    /* 0x55C */ be<u8> mPathId, mCurPathId;  /* was m444: Create 021762C8: params >> 8 (tww daHmlif_prm::getPathId()); set_next_pnt 02175908 / setNextPath 021757CC fetch the home path with it; was m445: Create 021762C8: mPathId, 255 when the room has no such path; move 02175DE4 / set_next_pnt 02175908 stop while 255; setNextPath 021757CC sets mPathId again on return (tww the same) */
    /* 0x55E */ be<s8> mPathDir, mSavedPathDir, mPointNo;  /* was m446: Create 021762C8: +1 / -1 from the direction parameter (random for 2); set_next_pnt 02175908 adds it to mPointNo and reverses it at the ends of an open path; setNextPath 021757CC sets it (tww the same); was m447: Create 021762C8 copies mPathDir; setNextPath 021757CC keeps mPathDir in it when switching to the next path, sets -1 / 1 at the path ends and restores mPathDir from it on return (tww the same); was m448: Create 021762C8: the start point (params >> 22 & 31); set_next_pnt 02175908 steps it by mPathDir (wraps / bounces) and loads mTargetPos from that point; setNextPath 021757CC sets 1 / mArg0 + dir (tww the same) */
    /* 0x561 */ u8 m449[3];
    /* 0x564 */ gptr<hmlifHD::Path> ppd;
    /* 0x568 */ cXyz mTargetPos;  /* was m450: Create 021762C8: the start point; set_next_pnt 02175908: the position of point mPointNo; lift_normal_move 02175C34 / lift_smooth_move 02175AD0 move the lift toward it (tww the same) */
    /* 0x574 */ cXyz mPrevTargetPos;  /* was m45C: Create 021762C8: the start position; set_next_pnt 02175908 keeps the old mTargetPos in it; lift_smooth_move 02175AD0 turns toward the angle from it to mTargetPos (tww the same) */
    /* 0x580 */ cXyz mJunctionPos;  /* was m468: Create 021762C8: while mSwitchNo != 255, the position of path point mArg0 (the point where setNextPath 021757CC switches paths) (tww the same); never read in WWHD or tww */
    /* 0x58C */ be<s32> mSwitchNo;  /* was m474: Create 021762C8: params & 0xFF (tww daHmlif_prm::getSwitchNo()); chkSwitchPathType 021757B8: != 255; setNextPath 021757CC takes the next room path while the switch is on */
    /* 0x590 */ be<u32> mSwitchNo2;  /* was m478: CreateHeap 02175520: home.angle.z & 0xFF (tww daHmlif_prm::getSwitchNo2()); check_col 02175F48 turns this switch on when hit; anim_play 02175E60 / CreateHeap set mBtpFrame by it */
    /* 0x594 */ u8 m47C[2];
    /* 0x596 */ be<u8> mSmoothMove;  /* was m47E: Create 021762C8: params >> 31 (tww daHmlif_prm::getSmooth()); move 02175DE4 runs lift_smooth_move while set, else lift_normal_move */
    /* 0x597 */ u8 m47F;
    /* 0x598 */ be<f32> mMaxSpeed;  /* was m480: Create 021762C8: the speed table entry of params >> 16 & 15 (capped at 8), also speedF; lift_smooth_move 02175AD0 / lift_normal_move 02175C34 ease speedF to it (tww the same) */
    /* 0x59C */ be<u32> mMoveCount;  /* was m484: lift_normal_move 02175C34: 0 in MOVE_INIT / MOVE_CRUISE, +1 per frame while accelerating or decelerating (tww the same); never otherwise read in WWHD or tww */
    /* 0x5A0 */ be<u8> mMoveState, mType;  /* was m488: lift_normal_move 02175C34: the MoveState (MOVE_INIT, MOVE_ACCELERATE, MOVE_CRUISE, MOVE_DECELERATE) it switches on and advances (tww the same); was m489: daHmlifCreate 021753A8: params >> 27 & 15, 0 when >= 3 (tww daHmlif_prm::getType()); CreateHeap 02175520 / Create 021762C8 / check_col 02175F48 pick the arc, model, cull box and sphere by it */
    /* 0x5A2 */ be<s16> mBtpFrame;  /* was m48A: CreateHeap 02175520: 0, or 3 when mSwitchNo2 is on; anim_play 02175E60 steps it toward 3 (switch on) or 0 (off); Draw 021761F8 enters the btp at it (tww the same) */
};
WWHD_OFFSET(daHmlif_c, mPhase, 0x3E0);
WWHD_OFFSET(daHmlif_c, mpModel, 0x3E8);
WWHD_OFFSET(daHmlif_c, mpBtp, 0x3EC);
WWHD_OFFSET(daHmlif_c, mpBrk, 0x3F0);
WWHD_OFFSET(daHmlif_c, mSph, 0x3F4);
WWHD_OFFSET(daHmlif_c, mStts, 0x520);
WWHD_OFFSET(daHmlif_c, mPathId, 0x55C);
WWHD_OFFSET(daHmlif_c, mCurPathId, 0x55D);
WWHD_OFFSET(daHmlif_c, mPathDir, 0x55E);
WWHD_OFFSET(daHmlif_c, mSavedPathDir, 0x55F);
WWHD_OFFSET(daHmlif_c, mPointNo, 0x560);
WWHD_OFFSET(daHmlif_c, ppd, 0x564);
WWHD_OFFSET(daHmlif_c, mTargetPos, 0x568);
WWHD_OFFSET(daHmlif_c, mPrevTargetPos, 0x574);
WWHD_OFFSET(daHmlif_c, mJunctionPos, 0x580);
WWHD_OFFSET(daHmlif_c, mSwitchNo, 0x58C);
WWHD_OFFSET(daHmlif_c, mSwitchNo2, 0x590);
WWHD_OFFSET(daHmlif_c, mSmoothMove, 0x596);
WWHD_OFFSET(daHmlif_c, mMaxSpeed, 0x598);
WWHD_OFFSET(daHmlif_c, mMoveCount, 0x59C);
WWHD_OFFSET(daHmlif_c, mMoveState, 0x5A0);
WWHD_OFFSET(daHmlif_c, mType, 0x5A1);
WWHD_OFFSET(daHmlif_c, mBtpFrame, 0x5A2);
WWHD_SIZE(daHmlif_c, 0x5A4);

// HD: the resource manager accepts this TU's eight-byte SafeString object.
struct HmlifName {
    be<u32> text, vt;
};
WWHD_OFFSET(HmlifName, vt, 4);
WWHD_SIZE(HmlifName, 8);
