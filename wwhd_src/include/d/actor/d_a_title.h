/* Title ship and logo manager, with the GameCube member names.
 * HD: a 0x3C attribute object precedes the original proc members; an invisible
 * model and enlarged animation wrappers move its tail. The 2D logo belongs to
 * a separate HD layout singleton. */
#pragma once
#include "gc_api/gc_api.h"
#include <cstddef>

namespace title_actor {
struct EmitterHD;
struct HeapHD;
} // namespace title_actor
/* HD: L_attr became an object (vtable + the 14 floats, copied from the per-TU
 * attribute table) inside the proc */
struct daTitle_Attr_c {
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ union {
        struct {
            be<f32> field_0x00;
            /* 0x08 */ be<f32> field_0x04;
            /* 0x0C */ be<f32> field_0x08;
            /* 0x10 */ be<f32> field_0x0C;
            /* 0x14 */ be<f32> field_0x10;
            /* 0x18 */ be<f32> field_0x14;
            /* 0x1C */ be<f32> field_0x18;
            /* 0x20 */ be<f32> field_0x1C;
            /* 0x24 */ be<f32> field_0x20;
            /* 0x28 */ be<f32> field_0x24;
            /* 0x2C */ be<f32> field_0x28;
            /* 0x30 */ be<f32> field_0x2C;
            /* 0x34 */ be<f32> field_0x30;
            /* 0x38 */ be<f32> field_0x34;
        };
        be<u32> words[14];
    };
};
WWHD_SIZE(daTitle_Attr_c, 0x3C);

struct daTitle_proc_c {
    void proc_init3D();
    void setEnterMode();
    void set_mtx();
    void calc_2d_alpha();
    void proc_execute();
    void model_draw();
    s32 getEnterMode() { return mEnterMode; }

    /* 0x000 */ be<u32> __vtbl; /* dDlst_base_c */
    /* 0x004 */ daTitle_Attr_c mAttr;
    /* 0x040 */ gptr<title_actor::EmitterHD> mpEmitter;
    /* 0x044 */ gptr<title_actor::EmitterHD> mpEmitter2;
    /* 0x048 */ cXyz m00C;
    /* 0x054 */ be<s32> m018;
    /* 0x058 */ be<s32> m01C;
    /* 0x05C */ be<s32> m020;
    /* 0x060 */ be<s32> m024;
    /* 0x064 */ be<s32> m028;
    /* 0x068 */ be<s32> m02C;
    /* 0x06C */ be<s32> mEnterMode;
    /* 0x070 */ u8 m034[4];
    /* 0x074 */ gptr<J3DModel> mModel_ship;
    /* 0x078 */ gptr<J3DModel> mModel_subtitle;
    /* 0x07C */ gptr<J3DModel> mModel_kirari;
    /* 0x080 */ u8 mInvisible[8]; /* HD: mDoExt_invisibleModel */
    /* 0x088 */ mDoExt_bckAnm mBckShip;
    /* 0x114 */ mDoExt_bpkAnm mBpkShip; /* HD: anm class with ctor 025E7480 / init 025E74FC */
    /* 0x188 */ mDoExt_btkAnm mBtkSub;
    /* 0x1FC */ mDoExt_btkAnm mBtkKirari;
    /* 0x270 */ be<u8> m090;
    /* 0x271 */ u8 _271[3];
    /* 0x274 */ be<f32> m094;
    /* 0x278 */ be<s32> m098;
    /* 0x27C */ be<u32> m_Screen;
    /* 0x280 */ be<u32> m0A0[6];
    /* 0x298 */ u8 pane[6][0x38]; /* fopMsgM_pane_class */
    /* 0x3E8 */ be<u32> m_exp_heap;
    /* 0x3EC */ gptr<title_actor::HeapHD> m_solid_heap;

    J3DFrameCtrl& bpkFrameCtrl() { return mBpkShip.mFrameCtrl; }
};
WWHD_OFFSET(daTitle_proc_c, mEnterMode, 0x6C);
WWHD_OFFSET(daTitle_proc_c, mBckShip, 0x88);
WWHD_OFFSET(daTitle_proc_c, m094, 0x274);
WWHD_OFFSET(daTitle_proc_c, pane, 0x298);
WWHD_SIZE(daTitle_proc_c, 0x3F0);

struct daTitle_c : fopAc_ac_c {
    /* 0x3AC */ request_of_phase_process_class mPhs;
    /* 0x3B4 */ gptr<daTitle_proc_c> mpTitleProc;
    /* 0x3B8 */ be<u8> m29C;
    u8 paddingActor[3];
};
WWHD_OFFSET(daTitle_c, m29C, 0x3B8);

WWHD_OFFSET(daTitle_Attr_c, field_0x00, 4);
WWHD_OFFSET(daTitle_Attr_c, field_0x34, 0x38);
WWHD_OFFSET(daTitle_proc_c, mAttr, 4);
WWHD_OFFSET(daTitle_proc_c, mpEmitter, 0x40);
WWHD_OFFSET(daTitle_proc_c, mModel_ship, 0x74);
WWHD_OFFSET(daTitle_proc_c, mModel_subtitle, 0x78);
WWHD_OFFSET(daTitle_proc_c, mModel_kirari, 0x7C);
WWHD_OFFSET(daTitle_proc_c, mInvisible, 0x80);
WWHD_OFFSET(daTitle_proc_c, mBpkShip, 0x114);
WWHD_OFFSET(daTitle_proc_c, mBtkSub, 0x188);
WWHD_OFFSET(daTitle_proc_c, mBtkKirari, 0x1FC);
WWHD_OFFSET(daTitle_proc_c, m098, 0x278);
WWHD_OFFSET(daTitle_proc_c, m_solid_heap, 0x3EC);
WWHD_OFFSET(daTitle_c, mPhs, 0x3AC);
WWHD_OFFSET(daTitle_c, mpTitleProc, 0x3B4);
WWHD_SIZE(daTitle_c, 0x3BC);

enum dRes_INDEX_TLOGO {
    dRes_INDEX_TLOGO_BCK_TITLE_SHIP_e = 8,
    dRes_INDEX_TLOGO_BDL_SUBTITLE_KIRARI_e = 0xB,
    dRes_INDEX_TLOGO_BDL_SUBTITLE_START_ANIM_e = 0xC,
    dRes_INDEX_TLOGO_BDL_TITLE_SHIP_e = 0xD,
    dRes_INDEX_TLOGO_BPK_TITLE_SHIP_e = 0x10,
    dRes_INDEX_TLOGO_BTK_SUBTITLE_KIRARI_e = 0x13,
    dRes_INDEX_TLOGO_BTK_SUBTITLE_START_ANIM_e = 0x14,
};

namespace title_actor {
struct LayoutHD;
struct LayoutPositionHD {
    be<f32> x, y;
};
WWHD_SIZE(LayoutPositionHD, 8);
struct HeapHD {
    /* 0x00 */ u8 prefix[0x10];
    /* 0x10 */ gptr<const char> mName;
};
WWHD_OFFSET(HeapHD, mName, 0x10);
WWHD_SIZE(HeapHD, 0x14);
struct PadHD {
    /* 0x00 */ u8 prefix[0x18];
    /* 0x18 */ be<u32> mTrigger;
};
WWHD_OFFSET(PadHD, mTrigger, 0x18);
WWHD_SIZE(PadHD, 0x1C);
struct PadStatusHD {
    /* 0x00 */ u8 prefix[0x1C];
    /* 0x1C */ be<u8> mTriggerConsumed;
};
WWHD_OFFSET(PadStatusHD, mTriggerConsumed, 0x1C);
WWHD_SIZE(PadStatusHD, 0x1D);
struct ResetHD {
    be<u32> mReset;
};
WWHD_SIZE(ResetHD, 4);
struct AttrWordsHD {
    be<u32> words[14];
};
WWHD_SIZE(AttrWordsHD, 0x38);
using LayoutPointerHD = gptr<LayoutHD>;
using PadPointerHD = gptr<PadHD>;
using PadStatusPointerHD = gptr<PadStatusHD>;
using ResetPointerHD = gptr<ResetHD>;
using WordHD = be<u32>;
using ByteHD = be<u8>;
struct BckConstructorHD {
    /* 0x00 */ J3DFrameCtrl mFrameCtrl;
    /* 0x10 */ be<u32> mTransformVtable;
    /* 0x14 */ u8 mMtxCalc[0x34];
    /* 0x48 */ be<u32> mMtxCalcVtable;
    /* 0x4C */ u8 paddingFlags[0xC];
    /* 0x58 */ be<u32> mFlags;
    /* 0x5C */ u8 paddingTail[0x20];
    /* 0x7C */ be<u32> mTail[4];
};
WWHD_OFFSET(BckConstructorHD, mTransformVtable, 0x10);
WWHD_OFFSET(BckConstructorHD, mMtxCalc, 0x14);
WWHD_OFFSET(BckConstructorHD, mMtxCalcVtable, 0x48);
WWHD_OFFSET(BckConstructorHD, mFlags, 0x58);
WWHD_OFFSET(BckConstructorHD, mTail, 0x7C);
WWHD_SIZE(BckConstructorHD, 0x8C);
struct EmitterHD {
    /* 0x000 */ u8 prefix[0x5C];
    /* 0x05C */ be<s32> mMaxFrame;
    /* 0x060 */ u8 paddingScale[0x220 - 0x60];
    /* 0x220 */ cXyz mGlobalParticleScale;
    /* 0x22C */ cXyz mGlobalTranslation;
    /* 0x238 */ cXyz mGlobalScale;
    /* 0x244 */ u8 paddingFlags[0x10];
    /* 0x254 */ be<u32> mFlags;
    /* 0x258 */ u8 paddingType[0xA];
    /* 0x262 */ be<u8> mEmitterType;
    /* 0x263 */ u8 paddingEnd;
};
WWHD_OFFSET(EmitterHD, mMaxFrame, 0x5C);
WWHD_OFFSET(EmitterHD, mGlobalParticleScale, 0x220);
WWHD_OFFSET(EmitterHD, mGlobalTranslation, 0x22C);
WWHD_OFFSET(EmitterHD, mGlobalScale, 0x238);
WWHD_OFFSET(EmitterHD, mFlags, 0x254);
WWHD_OFFSET(EmitterHD, mEmitterType, 0x262);
WWHD_SIZE(EmitterHD, 0x264);
/* Only the native draw-target fields read or written by this unit are
 * described. */
struct RuntimeVtableHD {
    u8 prefix[0x44];
    be<u32> mCheckDerived;
};
WWHD_OFFSET(RuntimeVtableHD, mCheckDerived, 0x44);
WWHD_SIZE(RuntimeVtableHD, 0x48);
struct DrawTargetHD {
    u8 prefix[0x58];
    gptr<RuntimeVtableHD> mRuntimeVtable;
};
WWHD_OFFSET(DrawTargetHD, mRuntimeVtable, 0x58);
WWHD_SIZE(DrawTargetHD, 0x5C);
struct DrawParametersHD {
    /* target +0xF4 */ be<u32> mChannels[4];
    /* target +0x104 */ u8 paddingPlanes[0x28];
    /* target +0x12C */ be<f32> mFar;
    /* target +0x130 */ be<f32> mOffset;
    /* target +0x134 */ be<f32> mDistance;
};
WWHD_OFFSET(DrawParametersHD, mFar, 0x38);
WWHD_OFFSET(DrawParametersHD, mOffset, 0x3C);
WWHD_OFFSET(DrawParametersHD, mDistance, 0x40);
WWHD_SIZE(DrawParametersHD, 0x44);
struct ViewportEntryHD {
    be<u16> mIndex, mGeneration;
};
WWHD_SIZE(ViewportEntryHD, 4);
struct ViewportTableHD {
    /* 0x00 */ u8 prefix[8];
    /* 0x08 */ be<u32> mEntryCount;
    /* 0x0C */ gptr<ViewportEntryHD> mpEntries;
    /* 0x10 */ be<u32> mTargetCount;
    /* 0x14 */ gptr<gptr<DrawTargetHD>> mpTargets;
};
WWHD_OFFSET(ViewportTableHD, mpEntries, 0xC);
WWHD_OFFSET(ViewportTableHD, mpTargets, 0x14);
WWHD_SIZE(ViewportTableHD, 0x18);
struct J3DSysHD {
    /* 0x000 */ u8 prefix[0x74];
    /* 0x074 */ be<u32> mOpaDrawBuffer, mXluDrawBuffer;
    /* 0x07C */ u8 paddingViewports[0x148 - 0x7C];
    /* 0x148 */ gptr<ViewportTableHD> mpViewports;
};
WWHD_OFFSET(J3DSysHD, mOpaDrawBuffer, 0x74);
WWHD_OFFSET(J3DSysHD, mpViewports, 0x148);
WWHD_SIZE(J3DSysHD, 0x14C);
struct RendererHD {
    /* 0x0000 */ u8 prefix[0x1020];
    /* 0x1020 */ be<u32> mDrawBufferCount;
    /* 0x1024 */ gptr<WordHD> mpDrawBuffers;
};
WWHD_OFFSET(RendererHD, mDrawBufferCount, 0x1020);
WWHD_OFFSET(RendererHD, mpDrawBuffers, 0x1024);
WWHD_SIZE(RendererHD, 0x1028);
using RendererPointerHD = gptr<RendererHD>;
using ViewportPointerHD = gptr<be<s16>>;
struct RuntimeTypeHD {
    be<u32> mVtable;
};
WWHD_SIZE(RuntimeTypeHD, 4);
struct ColorChannelsHD {
    be<u32> words[4];
};
WWHD_SIZE(ColorChannelsHD, 16);
struct JointTreeHD {
    u8 prefix[0x14];
    be<u32> mpTransformAnimation;
};
WWHD_OFFSET(JointTreeHD, mpTransformAnimation, 0x14);
WWHD_SIZE(JointTreeHD, 0x18);
struct ModelDataHD {
    /* 0x00 */ u8 prefix[8];
    /* 0x08 */ gptr<JointTreeHD> mpJointTree;
    /* 0x0C */ u8 paddingColor[0x40 - 0xC];
    /* 0x40 */ be<u32> mpColorAnimation;
};
WWHD_OFFSET(ModelDataHD, mpJointTree, 8);
WWHD_OFFSET(ModelDataHD, mpColorAnimation, 0x40);
WWHD_SIZE(ModelDataHD, 0x44);
/* HD: explicitly address the target channel even when the RTTI cast returns
 * zero. */
inline DrawParametersHD* drawParameters(DrawTargetHD* target) {
    return gabi::at<DrawParametersHD>(gabi::ea(target) + 0xF4);
}
inline u32 readTargetSlot(gptr<DrawTargetHD>* slot) { return gabi::load<u32>(gabi::ea(slot)); }
struct ProcVtableHD {
    u8 prefix[0xC];
    be<u32> mDestructor;
};
WWHD_OFFSET(ProcVtableHD, mDestructor, 0xC);
WWHD_SIZE(ProcVtableHD, 0x10);
inline void deleteProc(daTitle_proc_c* proc) {
    ProcVtableHD* vt = gabi::at<ProcVtableHD>(proc->__vtbl);
    gabi::call_ptr(vt->mDestructor, proc, 3);
}
} // namespace title_actor

namespace title_actor {
struct GameInfoHD {
    /* 0x0000 */ u8 prefix[0x514C];
    /* 0x514C */ be<s8> mEnableNextStage;
    /* 0x514D */ u8 paddingLists[0x5D78 - 0x514D];
    /* 0x5D78 */ be<u32> mOpaDrawBuffer, mXluDrawBuffer;
    /* 0x5D80 */ u8 padding2D[0x18];
    /* 0x5D98 */ be<u32> m2DDrawBuffer;
};
WWHD_OFFSET(GameInfoHD, mEnableNextStage, 0x514C);
WWHD_OFFSET(GameInfoHD, mOpaDrawBuffer, 0x5D78);
WWHD_OFFSET(GameInfoHD, m2DDrawBuffer, 0x5D98);
WWHD_SIZE(GameInfoHD, 0x5D9C);
inline GameInfoHD* titleGameInfo() { return reinterpret_cast<GameInfoHD*>(dComIfGp_get()); }
inline WordHD* drawBufferSlot(WordHD* base, u32 offset) {
    return gabi::at<WordHD>(gabi::ea(base) + offset);
}
} // namespace title_actor
