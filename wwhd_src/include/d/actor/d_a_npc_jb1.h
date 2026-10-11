// Jabun NPC: GameCube field names on the HD NPC base and enlarged animation/lighting layout.
#pragma once
#include "bindings.h"
#include "gc_api/tu.h"
#include "gc_api/f_op/f_op_actor.h"
#include "gc_api/d/d_com_inf_game.h"
#include "d/d_npc.h"
#include "m_Do/m_Do_ext.h"

struct JbAnimationParameters {
    be<s8> field_0x0;
    be<s8> field_0x1;
    u8 _pad[2];
    be<f32> mMorf, mPlaySpeed;
    be<s32> mLoopMode;
};
WWHD_SIZE(JbAnimationParameters, 16);
struct JbActionDescriptor {
    be<s16> adjustment, selector;
    be<u32> target;
};
WWHD_SIZE(JbActionDescriptor, 8);

// HD: embedded animation and lighting objects enlarge the original NPC layout.
struct daNpc_Jb1_c : fopNpc_npc_c {
    request_of_phase_process_class mPhs;
    be<s8> m_cse_jnt_num;
    u8 pad7E5[3];
    cXyz field_0x6D0;
    dKy_tevstr_c field_0x6DC;
    u8 pad9BC[0x1C];
    mDoExt_bckAnm mBckAnm;
    u8 padA64[8];
    mDoExt_btkAnm mBtkAnm;
    u8 padAE0[8];
    mDoExt_brkAnm mBrkAnm3;
    u8 padB60[4];
    LIGHT_INFLUENCE field_0x7F8;
    gptr<J3DAnmTevRegKey> m_lmp_brk;
    mDoExt_brkAnm mBrkAnm;
    be<s16> field_0x834;
    u8 padC06[2];
    gptr<J3DModel> m_lgt_mdl;
    gptr<J3DAnmTevRegKey> m_lgt_brk;
    mDoExt_brkAnm mBrkAnm2;
    be<s16> field_0x858;
    u8 padC8A[2];
    gabi::ptmf<BOOL (daNpc_Jb1_c::*)(void*)> mCurrActionFunc;
    u8 padC94[8];
    dNpc_EventCut_c mEventCut;
    u8 padD08[4];
    cXyz mCurrentPos;
    csXyz mCurrentRot;
    u8 padD1E[2];
    cXyz field_0x8F4;
    be<f32> mFrame;
    u8 padD30[4];
    be<u8> field_0x908, field_0x909;
    u8 padD36[2];
    be<u8> field_0x90C, field_0x90D, field_0x90E, field_0x90F;
    be<s32> field_0x910;
    be<u8> mHasAttention, field_0x915, field_0x916, field_0x917;
    be<u8> mActIdx, mActNo, field_0x91A, field_0x91B;
    be<s8> field_0x91C, field_0x91D, field_0x91E, field_0x91F;
    be<u8> field_0x920, field_0x921;
    be<s8> field_0x922, field_0x923;

    void checkOrder();
    void eventOrder();
    s32 isEventEntry();
    void event_actionInit(s32 staff);
    BOOL event_action();
    BOOL charDecide(s32 character);
    s32 anmNum_toResID(s32 field_0x0);
    BOOL setAnm_anm(JbAnimationParameters* parameters);
    BOOL setAnm();
    BOOL setAnm_ATR();
    void chg_anmAtr(u8 attribute);
    void anmAtr(u16 status);
    u16 next_msgStatus(u32* message);
    u32 getMsg();
    BOOL chkAttention();
    BOOL setStt(s8 state);
    BOOL wait_action1(void* argument);
    void emptyVirtual();
    void privateCut(s32 staff);
    void event_proc(s32 staff);
    u8 demo();
    BOOL _delete();
    void playBrkAnm(void* field_0x0, be<s16>* frame);
    BOOL set_action(JbActionDescriptor* action, void* argument);
    BOOL init_JB1_0();
    BOOL createInit();
    BOOL _execute();
    BOOL _draw();
    void setAttention();
    BOOL CreateHeap();
    s32 _create();
    void* create_Anm();
    BOOL create_lgt();
    void setMtx();
};
WWHD_SIZE(daNpc_Jb1_c, 0xD50);
WWHD_OFFSET(daNpc_Jb1_c, mPhs, 0x7DC);
WWHD_OFFSET(daNpc_Jb1_c, mBckAnm, 0x9D8);
WWHD_OFFSET(daNpc_Jb1_c, mCurrActionFunc, 0xC8C);
WWHD_OFFSET(daNpc_Jb1_c, mFrame, 0xD2C);
WWHD_OFFSET(daNpc_Jb1_c, field_0x922, 0xD4E);

// HD: private views preserve nullable guest accesses, word copies and wrapped signed indices.
namespace jb1 {
struct WordVector {
    be<u32> x, y, z;
};
inline WordVector* words(cXyz* p) {
    return reinterpret_cast<WordVector*>(p);
}
struct MtxCalc {
    u8 data[0x34];
};
struct BckConstructView : mDoExt_baseAnm {
    be<u32> mAnmVtable;
    MtxCalc mMtxCalc;
    be<u32> mMtxCalcVtable;
    u8 pad4C[12];
    be<u32> m58;
    u8 pad5C[0x20];
    be<u32> m7C, m80, m84, m88;
};
WWHD_SIZE(BckConstructView, 0x8C);
inline BckConstructView* bck(mDoExt_bckAnm* p) {
    return reinterpret_cast<BckConstructView*>(p);
}
struct LightParameters {
    cXyz position, direction;
    GXColor color;
    GXColorS10 materialColor;
    be<f32> parameters[8];
};
WWHD_SIZE(LightParameters, 0x44);
inline LightParameters* lightBlock(dKy_tevstr_c* p, u32 offset) {
    return gabi::at<LightParameters>(gabi::ea(p) + offset);
}
inline s16 virtualOffset(const void* p) {
    return gabi::load<s16>(gabi::ea(p) + 6);
}
inline s32 animationMaximum(void* animation) {
    u32 table = gabi::load<u32>(gabi::ea(animation) + 4);
    return gabi::call_ptr<s32>(gabi::load<u32>(table + 0x14), animation);
}
template <class T> inline T* atIndex(T* p, u32 index) {
    return gabi::at<T>(gabi::ea(p) + index * sizeof(T));
}
struct ModelDataView {
    u8 pad00[0x48];
    be<u32> animation;
};
inline ModelDataView* modelDataView(void* p) {
    return static_cast<ModelDataView*>(p);
}
inline J3DModel* morphModel(void* p) {
    return gabi::at<J3DModel>(gabi::load<u32>(gabi::ea(p) + offsetof(mDoExt_McaMorf, mpModel)));
}
inline J3DModelData* modelData(void* p) {
    return gabi::at<J3DModelData>(gabi::load<u32>(gabi::ea(p) + offsetof(J3DModel, mModelData)));
}
inline f32 morphFrame(void* p) {
    return gabi::load<f32>(gabi::ea(p) + offsetof(mDoExt_McaMorf, mFrameCtrl) +
                           offsetof(J3DFrameCtrl, mFrame));
}
inline be<f32>* matrixFloats(void* p) {
    return static_cast<be<f32>*>(p);
}
inline be<f32>* modelMatrix(void* p) {
    return gabi::at<be<f32>>(gabi::ea(p) + offsetof(J3DModel, mBaseTransformMtx));
}
inline u32 objectVtable(void* p) {
    return *static_cast<be<u32>*>(p);
}
inline void destroyMorf(void* p, u32 flags) {
    gabi::call_ptr<void>(gabi::load<u32>(objectVtable(p) + 12), p, flags);
}
} // namespace jb1

namespace jb1 {
inline void dispatchAction(daNpc_Jb1_c* actor, void* argument) {
    s16 selector = actor->mCurrActionFunc.i;
    s16 adjustment = actor->mCurrActionFunc.d;
    void* self = gabi::at<void>(gabi::ea(actor) + u32(s32(adjustment)));
    u32 target;
    if (selector < 0)
        target = actor->mCurrActionFunc.f;
    else {
        s16 vtableOffset = jb1::virtualOffset(&actor->mCurrActionFunc);
        u32 table = gabi::load<u32>(gabi::ea(self) + u32(s32(vtableOffset)));
        target = gabi::load<u32>(table + u32(s32(selector)) * 8 + 4);
    }
    gabi::call_ptr<void>(target, self, argument);
}
} // namespace jb1

namespace jb1 {
struct HioParameters {
    be<f32> field_0x00;
    be<s16> field_0x04, field_0x06, field_0x08, field_0x0A;
    be<f32> field_0x0C, field_0x10;
};
struct Hio {
    be<s8> mNo;
    u8 pad01[3];
    be<s32> field_0x8;
    HioParameters mPrmTbl;
    be<u32> __vtbl;
};
WWHD_SIZE(Hio, 0x20);
struct PiPair {
    be<f32> negative, positive;
};
inline u8 debugEnabled(const Hio* p) {
    return *gabi::at<be<u8>>(gabi::ea(&p->mPrmTbl.field_0x04));
}
inline dComIfG_play_c* gamePlay(void* p) {
    return &static_cast<dComIfG_inf_c*>(p)->play;
}
inline void* jointMatrix(daNpc_Jb1_c* actor) {
    void* morph = actor->mpMorf.get();
    s8 index = actor->m_cse_jnt_num;
    auto model = morphModel(morph);
    auto buffer =
        gabi::at<J3DMtxBuffer_hd>(gabi::load<u32>(gabi::ea(model) + offsetof(J3DModel, mpMtxBuffer)));
    u32 matrices = buffer->mpAnmMtx;
    u16 flags = buffer->mFlags;
    buffer->mFlags = flags | 0x10;
    return gabi::at<void>(matrices + u32(s32(index)) * 48);
}
inline void* nameTable(void* names) {
    u32 offset = gabi::load<u32>(gabi::ea(names) + 16);
    return gabi::at<void>(offset ? gabi::ea(names) + 16 + offset : 0);
}
} // namespace jb1

enum JbResource : s32 {
    dRes_ID_JB_BCK_JB_WAIT01_e = 1,
    dRes_ID_JB_BDL_JB_e = 2,
    dRes_ID_JB_BRK_JB_LAMP_e = 3,
    dRes_ID_JB_BRK_LF_e = 8,
    dRes_ID_JB_BMD_LF_e = 9
};

namespace jb1 {
inline void* demoObjects(u32 address) {
    return gabi::at<void>(address);
}
} // namespace jb1
WWHD_OFFSET(daNpc_Jb1_c, field_0x6DC, 0x7F4);
WWHD_OFFSET(daNpc_Jb1_c, mBtkAnm, 0xA6C);
WWHD_OFFSET(daNpc_Jb1_c, mBrkAnm3, 0xAE8);
WWHD_OFFSET(daNpc_Jb1_c, field_0x7F8, 0xB64);
WWHD_OFFSET(daNpc_Jb1_c, mBrkAnm, 0xB8C);
WWHD_OFFSET(daNpc_Jb1_c, m_lgt_mdl, 0xC08);
WWHD_OFFSET(daNpc_Jb1_c, mBrkAnm2, 0xC10);
WWHD_OFFSET(daNpc_Jb1_c, mEventCut, 0xC9C);
WWHD_OFFSET(daNpc_Jb1_c, mCurrentPos, 0xD0C);
