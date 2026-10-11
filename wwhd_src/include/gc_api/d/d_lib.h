/* d_lib (GameCube names). */
#pragma once
#include "bindings.h"

struct dLib_anm_idx_c {
    /* 0x00 */ be<s32> mBckIdx;
    /* 0x04 */ be<s32> mSndIdx;
};
struct dLib_anm_prm_c {
    /* 0x00 */ be<s8> mAnmIdx;
    /* 0x01 */ be<s8> mNextPrmIdx;
    /* 0x02 */ be<s16> field_0x02;
    /* 0x04 */ be<f32> mMorf;
    /* 0x08 */ be<f32> mPlaySpeed;
    /* 0x0C */ be<s32> mLoopMode;
};
/* 025877F8 dLib_setAnm (HD: one more argument, 0) [v nz] */
inline void dLib_setAnm(const char* arc, mDoExt_McaMorf* morf, be<s8>* bckIdx, be<s8>* prmIdx, be<s8>* prevPrmIdx,
                        const dLib_anm_idx_c* idxTbl, const dLib_anm_prm_c* prmTbl, u8 modify /* bool: the byte is passed on */) {
    gabi::call(0x025877F8, arc, morf, bckIdx, prmIdx, prevPrmIdx, idxTbl, prmTbl, modify, 0);
}

/* 02587EFC [v d_a_bmd]: HD retains the full room argument register. */
inline void dLib_setNextStageBySclsNum(s32 exit, s32 room) { gabi::call(0x02587EFC, exit, room); }
