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
    /* 0x55C */ be<u8> m444, m445;
    /* 0x55E */ be<s8> m446, m447, m448;
    /* 0x561 */ u8 m449[3];
    /* 0x564 */ gptr<hmlifHD::Path> ppd;
    /* 0x568 */ cXyz m450;
    /* 0x574 */ cXyz m45C;
    /* 0x580 */ cXyz m468;
    /* 0x58C */ be<s32> m474;
    /* 0x590 */ be<u32> m478;
    /* 0x594 */ u8 m47C[2];
    /* 0x596 */ be<u8> m47E;
    /* 0x597 */ u8 m47F;
    /* 0x598 */ be<f32> m480;
    /* 0x59C */ be<u32> m484;
    /* 0x5A0 */ be<u8> m488, m489;
    /* 0x5A2 */ be<s16> m48A;
};
WWHD_OFFSET(daHmlif_c, mPhase, 0x3E0);
WWHD_OFFSET(daHmlif_c, mpModel, 0x3E8);
WWHD_OFFSET(daHmlif_c, mpBtp, 0x3EC);
WWHD_OFFSET(daHmlif_c, mpBrk, 0x3F0);
WWHD_OFFSET(daHmlif_c, mSph, 0x3F4);
WWHD_OFFSET(daHmlif_c, mStts, 0x520);
WWHD_OFFSET(daHmlif_c, m444, 0x55C);
WWHD_OFFSET(daHmlif_c, m445, 0x55D);
WWHD_OFFSET(daHmlif_c, m446, 0x55E);
WWHD_OFFSET(daHmlif_c, m447, 0x55F);
WWHD_OFFSET(daHmlif_c, m448, 0x560);
WWHD_OFFSET(daHmlif_c, ppd, 0x564);
WWHD_OFFSET(daHmlif_c, m450, 0x568);
WWHD_OFFSET(daHmlif_c, m45C, 0x574);
WWHD_OFFSET(daHmlif_c, m468, 0x580);
WWHD_OFFSET(daHmlif_c, m474, 0x58C);
WWHD_OFFSET(daHmlif_c, m478, 0x590);
WWHD_OFFSET(daHmlif_c, m47E, 0x596);
WWHD_OFFSET(daHmlif_c, m480, 0x598);
WWHD_OFFSET(daHmlif_c, m484, 0x59C);
WWHD_OFFSET(daHmlif_c, m488, 0x5A0);
WWHD_OFFSET(daHmlif_c, m489, 0x5A1);
WWHD_OFFSET(daHmlif_c, m48A, 0x5A2);
WWHD_SIZE(daHmlif_c, 0x5A4);

// HD: the resource manager accepts this TU's eight-byte SafeString object.
struct HmlifName {
    be<u32> text, vt;
};
WWHD_OFFSET(HmlifName, vt, 4);
WWHD_SIZE(HmlifName, 8);
