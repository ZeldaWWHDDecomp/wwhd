/* daObjLpalm_c (palm tree), WWHD layout.
 *
 * GameCube -> WWHD: members +0x11C (fopAc_ac_c is 0x3AC). HD adds a separate collision matrix
 * (mBgMtx, 0x408): the collision model follows a copy of the model matrix scaled to 70% in x/z.
 * Size 0x438. */
#pragma once
#include "gc_api/gc_api.h"

/* Oyashi archive: resource indices and joints (HD values) */
enum {
    dRes_INDEX_OYASHI_BDL_OYASHI_e = 4,
    dRes_INDEX_OYASHI_DZB_OYASHI_e = 7,
};
enum {
    OYASHI_JNT_J_HAPPA1_e = 2,
    OYASHI_JNT_J_HAPPA2_e = 3,
};

class daObjLpalm_c : public fopAc_ac_c {
public:
    enum : u32 { __vt = 0x1002C144 }; /* HD: daObjLpalm_c's vtable */

    daObjLpalm_c();
    inline cPhs_State _create();
    inline bool _delete();
    inline bool _draw();
    inline bool _execute();

    BOOL CreateHeap();
    void CreateInit();

    static const char M_arcname[];

public:
    /* 0x3AC */ u8 m290[0x3B0 - 0x3AC];
    /* 0x3B0 */ Quaternion mBaseQuat;
    /* 0x3C0 */ Quaternion mBaseQuatTarget;
    /* 0x3D0 */ Quaternion mAnmMtxQuat[2];
    /* 0x3F0 */ be<s16> mAnimDir[2];
    /* 0x3F4 */ be<s16> mAnimWave[2];
    /* 0x3F8 */ request_of_phase_process_class mPhs;
    /* 0x400 */ gptr<J3DModel> mModel;
    /* 0x404 */ gptr<dBgW> mpBgW;
    /* 0x408 */ Mtx34 mBgMtx; /* HD: collision matrix */
};
WWHD_OFFSET(daObjLpalm_c, mBaseQuat, 0x3B0);
WWHD_OFFSET(daObjLpalm_c, mPhs, 0x3F8);
WWHD_OFFSET(daObjLpalm_c, mBgMtx, 0x408);
WWHD_SIZE(daObjLpalm_c, 0x438);
