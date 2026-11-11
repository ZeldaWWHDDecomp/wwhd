// Wind Temple biting blade trap.
#pragma once
#include "gc_api/gc_api.h"
namespace machine_layout {
struct dPnt {
    u8 mArgs[4];
    cXyz m_position;
};
struct dPath {
    be<u16> m_num, m_nextID;
    be<u8> mArg0, m_closed;
    u8 _06[2];
    gptr<dPnt> m_points;
};
struct WindMill : fopAc_ac_c {
    u8 _3AC[0x1534 - 0x3AC];
    be<u8> mType;
    u8 _1535;
    be<s16> mAngle[3];
};
} // namespace machine_layout
struct daMachine_c : fopAc_ac_c {
    BOOL CreateHeap();
    void CreateInit();
    void set_mtx();
    bool _delete();
    bool _draw();
    bool _execute();
    cPhs_State _create();
    machine_layout::WindMill* search_wind_mill();
    void set_speed();
    void set_next_pnt();
    void path_move();
    void set_cube();
    void attack();
    void set_body();
    void set_at();
    /* HD: grown actor and animation objects retain tww's member names/order. */
    /* 0x3AC */ request_of_phase_process_class mPhs;
    /* 0x3B4 */ gptr<J3DModel> mpModel;
    /* 0x3B8 */ dCcD_Stts mStts;
    /* 0x3F4 */ dCcD_Sph mSph;
    /* 0x520 */ dCcD_Sph mBodySph[3];  /* was field_0x404: CreateInit 021B7600 sets the three spheres up (l_sph_src_col); set_body 021B7F84 places them along the body (l_body0 / 1 / 2 in front of current.pos, turned by current.angle) and registers them; [0] also gets At and l_cullRadius (tww the same) */
    /* 0x8A4 */ dCcD_Sph field_0x788;
    /* 0x9D0 */ dBgS_ObjAcch mAcch;  /* was field_0x84b: CreateInit 021B7600 sets it up (Set with current.pos / old.pos / mAcchCir / speed); constructed in _create 021B7844, destroyed in the destructor 021B8268 (tww the same) */
    /* 0xB94 */ dBgS_AcchCir mAcchCir;  /* was field_0xa78: CreateInit 021B7600 sets its wall radius (l_wallRadius) and passes it to mAcch (tww the same) */
    /* 0xBD4 */ dCcD_Cps mSearchCps;  /* was field_0xab8: set_cube 021B7D8C sets the capsule from l_searchNear to l_cullFront in front of the machine (radius l_searchRadius); attack 021B7E48 starts the attack when mPlayerSph crosses it (tww the same) */
    /* 0xD0C */ cM3dGSph mPlayerSph;  /* was field_0xbf0: attack 021B7E48 sets it at the player (+ l_playerHeight, radius l_playerRadius) and tests it against mSearchCps (tww the same) */
    /* 0xD20 */ mDoExt_bckAnm mBck;  /* was field_0xc04: CreateHeap 021B7364 sets up the machine's bck; attack 021B7E48 restarts and plays it (state 1 / 2); set_at 021B8080 sets the attack sphere only between two of its frames; _draw 021B7A70 enters it (tww the same) */
    /* 0xDAC */ be<u8> mPathId;  /* was field_0xc14: CreateInit 021B7600 sets the low parameter byte (255 = no path, also when the room has no such path); path_move 021B7D14 and set_next_pnt 021B7C24 do nothing at 255 (tww the same) */
    /* 0xDAD */ be<s8> mPathPntNo;  /* was field_0xc15: set_next_pnt 021B7C24 steps it by mPathDir and turns at the path ends; the next target mNextPos is that point; CreateInit 021B7600 sets 1 (tww the same) */
    /* 0xDAE */ be<s8> mPathDir;  /* was field_0xc16: set_next_pnt 021B7C24: +1 / -1, reversed at the path ends; CreateInit 021B7600 sets 1 (tww the same) */
    /* 0xDAF */ be<s8> field_0xc17;
    /* 0xDB0 */ gptr<machine_layout::dPath> mpPath;  /* was field_0xc18: CreateInit 021B7600 stores the room path mPathId; set_next_pnt 021B7C24 reads the point count and points from it (tww the same) */
    /* 0xDB4 */ cXyz mNextPos;  /* was field_0xc1c: CreateInit 021B7600 / set_next_pnt 021B7C24 set it to path point mPathPntNo; path_move 021B7D14 moves current.pos toward it at speedF (tww the same) */
    /* 0xDC0 */ cXyz field_0xc28;
    /* 0xDCC */ u8 field_0xc34[8];
    /* 0xDD4 */ Mtx34 mJoint3Mtx;  /* was field_0xc3c: nodeCallBack 021B7494 copies the animated "joint3" matrix into it; set_at 021B8080 puts the attack sphere mSph at its translation (tww the same) */
    /* 0xE04 */ cXyz mAtPos;  /* was field_0xc6c: set_at 021B8080 copies the translation of mJoint3Mtx into it and centres the attack sphere mSph on it (tww the same) */
    /* 0xE10 */ be<u8> mAttackState;  /* was field_0xc78: attack 021B7E48: 0 waits for the player in mSearchCps, 1 starts the attack animation (JAMA_MECHA_OUT), 2 plays it and returns to 0 at its end (tww the same) */
    /* 0xE11 */ u8 field_0xC79[0x1B];
};
WWHD_OFFSET(daMachine_c, mBck, 0xD20);
WWHD_OFFSET(daMachine_c, mJoint3Mtx, 0xDD4);
WWHD_SIZE(daMachine_c, 0xE2C);
