// Jabun NPC: GameCube field names on the HD NPC base and enlarged animation/lighting layout.
#pragma once
#include "bindings.h"
#include "gc_api/tu.h"
#include "gc_api/f_op/f_op_actor.h"
#include "gc_api/d/d_com_inf_game.h"
#include "d/d_npc.h"
#include "m_Do/m_Do_ext.h"

struct JbAnimationParameters {
    be<s8> anmNum;  /* was field_0x0: setAnm_anm 02248780 compares it with mAnmNum and loads it; setAnm 02248814 plays only entries with it >= 0 (tww the same); after tww daNpc_Bm1_c::anm_prm_c anmNum (same setAnm_anm code) */
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
    cXyz mLightPos;  /* was field_0x6D0: setMtx 02247B7C: an offset through the m_cse_jnt_num joint matrix, copied to mPointLight.mPos (tww mDoMtx_stack_c::multVec(&temp, &it); field_0x7F8.mPos.set(it)) */
    dKy_tevstr_c field_0x6DC;
    u8 pad9BC[0x1C];
    mDoExt_bckAnm mBckAnm;
    u8 padA64[8];
    mDoExt_btkAnm mBtkAnm;
    u8 padAE0[8];
    mDoExt_brkAnm mBrkAnm3;
    u8 padB60[4];
    LIGHT_INFLUENCE mPointLight;  /* was field_0x7F8: createInit 02247E84 registers it (tww dKy_plight_set(&it)); setMtx 02247B7C sets its position from mLightPos and its colour, power and fluctuation from the HIO (tww); _delete removes it (tww dKy_plight_cut) */
    gptr<J3DAnmTevRegKey> m_lmp_brk;
    mDoExt_brkAnm mBrkAnm;
    be<s16> mLmpBrkFrame;  /* was field_0x834: setMtx 02247B7C plays m_lmp_brk with it (tww playBrkAnm(m_lmp_brk, &it)); _draw 022485C4 enters mBrkAnm at it (tww mBrkAnm.entry(modelData, it)); create_Anm 02247260 0 */
    u8 padC06[2];
    gptr<J3DModel> m_lgt_mdl;
    gptr<J3DAnmTevRegKey> m_lgt_brk;
    mDoExt_brkAnm mBrkAnm2;
    be<s16> mLgtBrkFrame;  /* was field_0x858: setMtx 02247B7C plays m_lgt_brk with it (tww playBrkAnm(m_lgt_brk, &it)); _draw 022485C4 enters mBrkAnm2 at it on m_lgt_mdl (tww); create_lgt 022474D8 0 */
    u8 padC8A[2];
    gabi::ptmf<BOOL (daNpc_Jb1_c::*)(void*)> mCurrActionFunc;
    u8 padC94[8];
    dNpc_EventCut_c mEventCut;
    u8 padD08[4];
    cXyz mCurrentPos;
    csXyz mCurrentRot;
    u8 padD1E[2];
    cXyz mAttPos;  /* was field_0x8F4: setAttention 02247AC4: current.pos plus a fixed offset turned by current.angle.y; attention_info.position is it plus HIO mAttPosOffsetY and eyePos is it (tww the same) */
    be<f32> mFrame;
    u8 padD30[4];
    /* mbMorfAnimStopped (0xD34) was field_0x908: setMtx 02247B7C: the morf's play() result, or 1; setAnm_anm 02248780 0 (tww the same); after tww daNpc_Ls1_c mbMorfAnimStopped (same code) */
    be<u8> mbMorfAnimStopped, field_0x909;
    u8 padD36[2];
    /* mbNoShapeAngle (0xD39) was field_0x90D: _execute 02248444: shape_angle follows current.angle only while it is 0 (tww if (!it) shape_angle = current.angle); read only, never written in WWHD or tww; after our decomp's daNpc_Pf1_c mbNoShapeAngle (same code) */
    /* mbRanExecute (0xD3B) was field_0x90F: _execute 02248444: 0 on the first run, when mCurrentPos and mCurrentRot are taken, then 1 (tww the same); after tww daNpc_Bm1_c mbRanExecute (same code) */
    be<u8> field_0x90C, mbNoShapeAngle, field_0x90E, mbRanExecute;
    be<s32> mbSetEyePos;  /* was field_0x910: wait_action1 02248A0C stores 1, the inlined wait_1() result (tww it = wait_1()); written only in WWHD and tww; after tww daNpc_Ls1_c mbSetEyePos (same wait_action1 code) */
    /* mTalking (0xD41) was field_0x915: checkOrder 02248128 sets 1 on the talk command (tww the same); written only in WWHD and tww; same checkOrder code as daNpc_Km1_c / daNpc_Hi1_c::mTalking */
    /* mInDemo (0xD43) was field_0x917: demo 02248168: 1 while demoActorID is set, 0 otherwise, and returns it; setMtx 02247B7C plays the animation only while it is 0 (tww the same); same code as daNpc_Km1_c / daNpc_Hi1_c::mInDemo */
    be<u8> mHasAttention, mTalking, field_0x916, mInDemo;
    /* mAnmAtrNo (0xD46) was field_0x91A: chg_anmAtr 02248874 stores the attribute; setAnm_ATR 0224885C plays l_atrPrm[it]; anmAtr 02248894 0xFF (tww the same, control_anmAtr switches on it); same code as daNpc_Km1_c / daNpc_Hi1_c::mAnmAtrNo */
    /* mMesgAnimeTag (0xD47) was field_0x91B: anmAtr 02248894: the message's animation tag when it changes (if (tag != 0xFF && it != tag)) (tww the same); after tww daNpc_Ls1_c mMesgAnimeTag (same code) */
    be<u8> mActIdx, mActNo, mAnmAtrNo, mMesgAnimeTag;
    /* mAnmNum (0xD48) was field_0x91C: setAnm_anm 02248780 skips an entry whose anmNum equals it, else stores it and loads anmNum_toResID(it) (tww the same); createInit 02247E84 2; after tww daNpc_Bm1_c mAnmNum (same setAnm_anm code) */
    /* mOrderEvent (0xD49) was field_0x91D: eventOrder 0224840C orders the talk at 1 / 2; checkOrder 02248128 starts it and clears it (tww the same); same code as daNpc_Km1_c / daNpc_Hi1_c::mOrderEvent */
    /* mStt (0xD4A) was field_0x91E: setStt 02248A04 stores the state; setAnm 02248814 plays l_anmPrm[it]; wait_action1 02248A0C switches on it (tww the same); same code as daNpc_Km1_c / daNpc_Hi1_c::mStt */
    be<s8> mAnmNum, mOrderEvent, mStt, field_0x91F;
    /* mHeapSizeIdx (0xD4C) was field_0x920: charDecide 022478D8 0 (tww the same); tww _create creates the solid heap with a_size_tbl[it]; same code as daNpc_Ls1_c / daNpc_Hi1_c::mHeapSizeIdx */
    /* mType (0xD4D) was field_0x921: charDecide 022478D8 0 (tww -1 or 0); createInit 02247E84 switches on it (tww createInit / getMsg switch (it)); after tww daNpc_Ls1_c mType (same charDecide code) */
    be<u8> mHeapSizeIdx, mType;
    /* mActionStatus (0xD4E) was field_0x922: set_action 022478EC sets 9 for the leaving action and 0 for the new one; wait_action1 02248A0C switches on it and steps it (tww the same); after tww daNpc_Bs1_c mActionStatus */
    /* mAnmAtrSetCnt (0xD4F) was field_0x923: anmAtr 02248894: applies the message's animation attribute only when it is 0, then adds 1; resets it (tww the same); same code as daNpc_Km1_c / daNpc_Hi1_c::mAnmAtrSetCnt */
    be<s8> mActionStatus, mAnmAtrSetCnt;

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
WWHD_OFFSET(daNpc_Jb1_c, mActionStatus, 0xD4E);

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
    be<f32> mAttPosOffsetY;  /* was field_0x00: setAttention 02247AC4 puts attention_info.position this far above mAttPos (tww the same); same role as tww daNpc_Ls1_c::hio_prm_c mAttPosOffsetY */
    /* mLightColorR (+0x06) was field_0x06: setMtx 02247B7C: mPointLight.mColorR (tww field_0x7F8.mColor.r = it) */
    /* mLightColorG (+0x08) was field_0x08: setMtx 02247B7C: mPointLight.mColorG (tww field_0x7F8.mColor.g = it) */
    /* mLightColorB (+0x0A) was field_0x0A: setMtx 02247B7C: mPointLight.mColorB (tww field_0x7F8.mColor.b = it) */
    be<s16> field_0x04, mLightColorR, mLightColorG, mLightColorB;
    /* mLightPower (+0x0C) was field_0x0C: setMtx 02247B7C: mPointLight.mPower (tww field_0x7F8.mPower = it) */
    /* mLightFluctuation (+0x10) was field_0x10: setMtx 02247B7C: mPointLight.mFluctuation (tww field_0x7F8.mFluctuation = it) */
    be<f32> mLightPower, mLightFluctuation;
};
struct Hio {
    be<s8> mNo;
    u8 pad01[3];
    be<s32> mRefCount;  /* was field_0x8: the HIO constructor 02248A98 -1; _create 02247F64 adds 1 (creating the HIO child at < 0); _delete 02248098 subtracts 1 (tww the same); after our decomp's daTag_Ba1_HIO_c / daNpc_Pm1_HIO_c mRefCount (same code) */
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
WWHD_OFFSET(daNpc_Jb1_c, mPointLight, 0xB64);
WWHD_OFFSET(daNpc_Jb1_c, mBrkAnm, 0xB8C);
WWHD_OFFSET(daNpc_Jb1_c, m_lgt_mdl, 0xC08);
WWHD_OFFSET(daNpc_Jb1_c, mBrkAnm2, 0xC10);
WWHD_OFFSET(daNpc_Jb1_c, mEventCut, 0xC9C);
WWHD_OFFSET(daNpc_Jb1_c, mCurrentPos, 0xD0C);
