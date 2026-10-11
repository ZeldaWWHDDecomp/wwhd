// Tetra's tree branch. Private WWHD layout.
#pragma once
#include "gc_api/gc_api.h"

struct BranchDemoActor {
    enum { ENABLE_ANM_e = 0x20 };
    u8 mPadding[0x28];
    be<u32> mShapeId;
    u32 getShapeId() { return mShapeId; }
};
WWHD_OFFSET(BranchDemoActor, mShapeId, 0x28);
struct BranchDemoObject { BranchDemoActor* getActor(u8 id); };
void mDoMtx_XYZrotM(Mtx34*, s16, s16, s16);
BOOL dDemo_setDemoData(fopAc_ac_c*, u8, mDoExt_McaMorf*, const char*, s32, be<u16>*, u32, s8);

class daBranch_c : public fopAc_ac_c {
public:
    daBranch_c();
    cPhs_State create();
    BOOL draw();
    BOOL execute();
    void set_mtx();
    void set_anim(int, int, int);
    // HD: the generated wrapper exposes the callee's returned word.
    u32 demoPlay(mDoExt_McaMorf*);
    BOOL CreateHeap();
    Mtx34* getJointMtx(const char*);
    static const char m_arcname[];
    /* 0x3AC */ request_of_phase_process_class mPhase;
    /* 0x3B4 */ gptr<J3DModel> mModel[2];
    /* 0x3BC */ u8 dummy2[0x08];
    /* 0x3C4 */ gptr<mDoExt_McaMorf> mAnims[2];
    /* 0x3CC */ u8 dummy[0x08];
    /* 0x3D4 */ be<u32> m02B8;
    /* 0x3D8 */ be<u8> m02BC;
    /* 0x3D9 */ be<u8> m02BD;
    u8 mPadding3DA[2];
};
WWHD_OFFSET(daBranch_c, mModel, 0x3B4);
WWHD_OFFSET(daBranch_c, mAnims, 0x3C4);
WWHD_OFFSET(daBranch_c, m02B8, 0x3D4);
WWHD_SIZE(daBranch_c, 0x3DC);
