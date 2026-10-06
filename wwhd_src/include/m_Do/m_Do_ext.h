/* m_Do_ext: model/animation wrappers, WWHD layouts. 
 *
 * Status of each item: [v] used by a verified function, [g] GameCube layout/signature not yet
 * exercised by a verified function (the harness will tell). */
#pragma once
#include "SSystem/SComponent/c_xyz.h"
#include "wwhd.h"

/* J3DFrameCtrl, HD: no vtable, fields reordered [v: kamome rate/frame/state] */
struct J3DFrameCtrl {
    enum { EMode_NONE = 0, EMode_RESET = 1, EMode_LOOP = 2, EMode_REVERSE = 3, EMode_LOOP_REVERSE = 4 };
    enum { STATE_STOP_E = 1, STATE_LOOP_E = 2 };
    /* 0x0 */ be<f32> mRate;
    /* 0x4 */ be<f32> mFrame;
    /* 0x8 */ be<s16> mStart;
    /* 0xA */ be<s16> mEnd;
    /* 0xC */ be<s16> mLoop;
    /* 0xE */ be<u8> mAttribute;
    /* 0xF */ be<u8> mState;
    f32 getRate() { return mRate; }
    void setRate(f32 r) { mRate = r; }
    f32 getFrame() { return mFrame; }
    void setFrame(f32 f) { mFrame = f; }
    s16 getStart() { return mStart; }
    s16 getEnd() { return mEnd; }
    u8 getAttribute() { return mAttribute; }
    void setAttribute(u8 a) { mAttribute = a; }
    bool checkState(u8 s) { return (mState & s) != 0; }
    /* 027F2BF8 J3DFrameCtrl::checkPass [g] */
    BOOL checkPass(f32 frame) { return gabi::call<BOOL>(0x027F2BF8, this, frame); }
};
WWHD_SIZE(J3DFrameCtrl, 0x10);

/* mDoExt_McaMorf, HD size 0xC8 (constructor 025E4F64 allocates 0xC8); GameCube members +0x40 */
struct mDoExt_McaMorf {
    /* 0x00 */ u8 _00[0x90];             /* J3DMtxCalcMaya base (HD) */
    /* 0x90 */ gptr<J3DModel> mpModel;   /* [v] */
    /* 0x94 */ gptr<J3DAnmTransform> mpAnm;
    /* 0x98 */ J3DFrameCtrl mFrameCtrl;  /* [v] */
    /* 0xA8 */ u8 _A8[0xB4 - 0xA8];      /* mpTransformInfo, mpQuat, mCurMorf (GameCube 0x6C..0x74) */
    /* 0xB4 */ be<f32> mCurMorf;         /* [g] GameCube 0x74 + 0x40 */
    /* 0xB8 */ u8 _B8[0xC8 - 0xB8];

    J3DModel* getModel() { return mpModel; }
    J3DAnmTransform* getAnm() { return mpAnm; }
    f32 getFrame() { return mFrameCtrl.getFrame(); }
    void setFrame(f32 f) { mFrameCtrl.setFrame((f32)(s16)gabi::ftoi(f)); } /* GameCube: setFrame((s16)frame) */
    f32 getPlaySpeed() { return mFrameCtrl.getRate(); }
    void setPlaySpeed(f32 s) { mFrameCtrl.setRate(s); }
    int getPlayMode() { return mFrameCtrl.getAttribute(); }
    void setPlayMode(int m) { mFrameCtrl.setAttribute((u8)m); }
    f32 getEndFrame() { return (f32)mFrameCtrl.getEnd(); }
    BOOL isStop() { return mFrameCtrl.checkState(J3DFrameCtrl::STATE_STOP_E) || mFrameCtrl.getRate() == 0.0f; }
    bool isLoop() { return mFrameCtrl.checkState(J3DFrameCtrl::STATE_LOOP_E); }
    BOOL checkFrame(f32 f) { return mFrameCtrl.checkPass(f); }

    /* 025E4F64 constructor; HD: allocates when this == NULL, so `new mDoExt_McaMorf(...)` is
     * create(NULL, ...) [g] */
    static mDoExt_McaMorf* create(mDoExt_McaMorf* self, J3DModelData* data, mDoExt_McaMorfCallBack1_c* cb1,
                                  mDoExt_McaMorfCallBack2_c* cb2, J3DAnmTransform* anm, s32 loopMode, f32 speed,
                                  s32 startFrame, s32 endFrame, s32 param_8, void* basAnm, u32 modelFlag, u32 dlFlag) {
        return gabi::call<mDoExt_McaMorf*>(0x025E4F64, self, data, cb1, cb2, anm, loopMode, speed, startFrame,
                                           endFrame, param_8, basAnm, modelFlag, dlFlag);
    }
    /* 025E535C [g] */
    BOOL play(cXyz* pos, u32 se, s8 reverb) { return gabi::call<BOOL>(0x025E535C, this, pos, se, reverb); }
    /* 025E4A98 [v] */
    void setAnm(J3DAnmTransform* anm, s32 loopMode, f32 morf, f32 speed, f32 start, f32 end, void* bas) {
        gabi::call(0x025E4A98, this, anm, loopMode, morf, speed, start, end, bas);
    }
    void setMorf(f32 morf) { gabi::call(0x025E4A54, this, morf); }  /* 025E4A54 [g] */
    void calc() { gabi::call(0x025E55A0, this); }                   /* 025E55A0 [v] */
    void entryDL() { gabi::call(0x025E5590, this); }                /* 025E5590 [v] */
    void updateDL() { gabi::call(0x025E54D8, this); }               /* 025E54D8 [g] */
    void stopZelAnime() { gabi::call(0x025E563C, this); }           /* 025E563C [g] */
};
WWHD_OFFSET(mDoExt_McaMorf, mpModel, 0x90);
WWHD_OFFSET(mDoExt_McaMorf, mFrameCtrl, 0x98);
WWHD_SIZE(mDoExt_McaMorf, 0xC8);

/* ---- J3DModel (HD layout) ---- */
/* base transform at +0xC8 [v]. HD: taking its address is null-preserving */
inline Mtx34* J3DModel_getBaseTRMtx(J3DModel* m) { return m ? gabi::at<Mtx34>(gabi::ea(m) + 0xC8) : nullptr; }
/* J3DModel::getModelData: HD +0xAC [v mo2 021C6414] */
inline J3DModelData* J3DModel_getModelData(J3DModel* m) { return gabi::at<J3DModelData>(gabi::load<u32>(gabi::ea(m) + 0xAC)); }
/* 027F4D5C J3DModel::calc [g] */
inline void J3DModel_calc(J3DModel* m) { gabi::call(0x027F4D5C, m); }
/* 025E2E5C mDoExt_modelEntryDL [g] */
inline void mDoExt_modelEntryDL(J3DModel* m) { gabi::call(0x025E2E5C, m); }

/* ---- mDoExt_*Anm: sizes and layouts still to be measured (report them) ---- */
/* HD sizes measured in swhit0: mDoExt_bckAnm 0x8C, mDoExt_btkAnm 0x74 (constructor 025E7C6C, the
 * matcher calls it btkAnm::init). Both start with their J3DFrameCtrl (frame at +4). */
struct mDoExt_baseAnm {
    /* 0x0 */ J3DFrameCtrl mFrameCtrl;
    f32 getFrame() { return mFrameCtrl.getFrame(); }
    BOOL play() { return gabi::call<BOOL>(0x025E742C, this); } /* 025E742C [v swhit0] */
};
struct mDoExt_bckAnm : mDoExt_baseAnm {
    /* 0x10 */ u8 _10[0x8C - 0x10];
    /* 025E8508 init(data, anm, bool anmPlay, int mode, f32 speed, s16 start, s16 end, bool) [v swhit0] */
    BOOL init(J3DModelData* d, J3DAnmTransform* a, bool play, s32 mode, f32 speed, s16 start, s16 end, bool b) {
        return gabi::call<BOOL>(0x025E8508, this, d, a, play, mode, speed, start, end, b);
    }
    void entry(J3DModelData* d, f32 frame) { gabi::call(0x025E86B8, this, d, frame); } /* 025E86B8 [v swhit0] */
};
WWHD_SIZE(mDoExt_bckAnm, 0x8C);
struct mDoExt_btkAnm : mDoExt_baseAnm {
    /* 0x10 */ u8 _10[0x74 - 0x10];
    /* 025E7CE0 init(data, key, bool anmPlay, int mode, f32 speed, s16 start, s16 end, bool, int (stack)) [v swhit0] */
    BOOL init(J3DModelData* d, J3DAnmTextureSRTKey* k, bool play, s32 mode, f32 speed, s16 start, s16 end, bool b, s32 i) {
        return gabi::call<BOOL>(0x025E7CE0, this, d, k, play, mode, speed, start, end, b, i);
    }
    void entry(J3DModelData* d, f32 frame) { gabi::call(0x025E7FC4, this, d, frame); } /* 025E7FC4 [v swhit0] */
    static void ct(mDoExt_btkAnm* p) { gabi::call(0x025E7C6C, p); }                 /* 025E7C6C [v swhit0] */
};
WWHD_SIZE(mDoExt_btkAnm, 0x74);
WWHD_OPAQUE(mDoExt_brkAnm);
WWHD_OPAQUE(mDoExt_btpAnm);
/* 025E742C mDoExt_baseAnm::play [g] */
inline BOOL mDoExt_baseAnm_play(void* anm) { return gabi::call<BOOL>(0x025E742C, anm); }
/* 025E7B3C mDoExt_btpAnm::entry(J3DModelData*, s16 frame) [g] */
inline void mDoExt_btpAnm_entry(mDoExt_btpAnm* a, J3DModelData* d, s16 frame) { gabi::call(0x025E7B3C, a, d, frame); }
/* 025E7FC4 mDoExt_btkAnm::entry(J3DModelData*, f32) [g] */
inline void mDoExt_btkAnm_entry(mDoExt_btkAnm* a, J3DModelData* d, f32 frame) { gabi::call(0x025E7FC4, a, d, frame); }
/* 025E83FC mDoExt_brkAnm::entry(J3DModelData*, f32) [g] */
inline void mDoExt_brkAnm_entry(mDoExt_brkAnm* a, J3DModelData* d, f32 frame) { gabi::call(0x025E83FC, a, d, frame); }
