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
    /* 0x524 */ be<s16> m408;
    /* 0x526 */ be<s16> m40A;
    /* 0x528 */ be<s16> m40C;
    /* 0x52A */ be<s16> m40E;
    /* 0x52C */ be<u8> m410;
    /* 0x52D */ u8 m411[3];
};
WWHD_OFFSET(daObjRflw_c, mCyl, 0x3F4);
WWHD_OFFSET(daObjRflw_c, m410, 0x52C);
WWHD_SIZE(daObjRflw_c, 0x530);

