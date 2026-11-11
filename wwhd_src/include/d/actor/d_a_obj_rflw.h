/* Rito flower, WWHD layout. */
#pragma once
#include "gc_api/gc_api.h"
enum { dRes_INDEX_RFLW_BDL_PHANA_e = 3, JA_SE_OBJ_TREE_SWING_S = 0x69E6 };
/* HD: this unit captures the matrix pointer before setting the dirty flag. */
struct J3DModel_rflw_view : J3DModel {
    Mtx34* getAnmMtx(int joint) {
        J3DMtxBuffer_hd* buffer = mpMtxBuffer;
        u32 matrices = buffer->mpAnmMtx;
        buffer->mFlags |= 0x10;
        return gabi::at<Mtx34>(matrices + (u32)joint * 0x30);
    }
};
struct daObjRflw_c : fopAc_ac_c {
    daObjRflw_c() {
        fopAc_ac_c_ct(this);
        __vtbl = 0x1002ECF4; // HD: actor vtable.
        dCcD_Stts_ct(&mStts);
        dCcD_Cyl_ct(&mCyl, 0x1002ECE4);
    }
    cPhs_State _create();
    BOOL _delete();
    BOOL _draw();
    BOOL _execute();

    BOOL CreateHeap();
    void CreateInit();
    void set_mtx();

    /* 0x3AC */ request_of_phase_process_class mPhs;
    /* 0x3B4 */ gptr<J3DModel> mpModel;
    /* 0x3B8 */ dCcD_Stts mStts;
    /* 0x3F4 */ dCcD_Cyl mCyl;
    /* 0x524 */ be<s16> mSwingAngle;  /* was m408: _execute (in daObjRflw_Execute 02388064): sin(mSwingPhase) * (0x100 - mHitTimer) * 4 (first 8 frames) or * 2, 0 at the end; nodeCallBack 02387A74 rotates each joint by it about Y (tww the same); CreateInit 02387DA8 clears it */
    /* 0x526 */ be<s16> mSwingPhase;  /* was m40A: _execute (in daObjRflw_Execute 02388064): 0 on a hit, + 0x1000 per frame for the first 8 frames then + 0x800; the sine argument of mSwingAngle (tww the same) */
    /* 0x528 */ be<s16> mHitTimer;  /* was m40C: _execute (in daObjRflw_Execute 02388064): 0 on a hit, + 1 per frame while mIsSwinging, ends the swing above 0x100; a new hit counts only above 0x38; damps mSwingAngle by (0x100 - it) (tww the same); the unit spec d_a_obj_rflw.txt already calls 0x528 mHitTimer */
    /* 0x52A */ be<s16> mSwingDirAngle;  /* was m40E: _execute (in daObjRflw_Execute 02388064): the angle from the colliding actor to the flower (cM_atan2s) - 0x4000 on a hit; nodeCallBack 02387A74 rotates X by it, Y by mSwingAngle, X back by -it, so the flower swings away from the hitter (tww the same) */
    /* 0x52C */ be<u8> mIsSwinging;  /* was m410: _execute (in daObjRflw_Execute 02388064): 1 on a cylinder co-hit (with the JA_SE_OBJ_TREE_SWING_S sound), 0 when mHitTimer passes 0x100; the swing runs while set; _create (in daObjRflw_Create 02387E5C) clears it (tww the same); the unit spec calls 0x52C "swinging" */
    /* 0x52D */ u8 m411[3];
};
WWHD_OFFSET(daObjRflw_c, mCyl, 0x3F4);
WWHD_OFFSET(daObjRflw_c, mIsSwinging, 0x52C);
WWHD_SIZE(daObjRflw_c, 0x530);

