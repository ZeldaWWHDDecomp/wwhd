#pragma once
#include "gc_api/gc_api.h"
// HD: BRK animation work grows from 0x18 to 0x78.
struct YboilBrk : mDoExt_baseAnm {
 u8 padding[0x78-0x10];
 BOOL init(J3DModelData*,J3DAnmTevRegKey*,bool,s32,f32,s16,s16,bool,s32);
 void entry(J3DModelData*,f32);
};
class daObjYboil_c : public fopAc_ac_c {
public:
 enum Prm_e { PRM_SWSAVE_S=8, PRM_SWSAVE_W=8 };
 /* 0x03AC */ request_of_phase_process_class mPhs;
 /* 0x03B4 */ gptr<J3DModel> mModel[50];
 /* 0x047C */ mDoExt_bckAnm mBckAnm[50];
 /* 0x1FD4 */ mDoExt_btkAnm mBtkAnm[50];
 /* 0x367C */ YboilBrk mBrkAnm[50];
 // HD: decrement wraps in a full unsigned word.
 /* 0x4DEC */ be<u32> mMdlTimer[50];
 /* 0x4EB4 */ cXyz mMdlPos[50];
 /* 0x510C */ cXyz mMdlScale[50];
 /* 0x5364 */ be<f32> mScaleMax;
 /* 0x5368 */ be<f32> mScaleMin;
 s32 _create(); BOOL _delete(); BOOL _draw(); BOOL _execute();
 BOOL CreateHeap(); void pos_reset(s32); void set_mtx(); void CreateInit();
};
WWHD_OFFSET(daObjYboil_c,mModel,0x3B4);
WWHD_OFFSET(daObjYboil_c,mBckAnm,0x47C);
WWHD_OFFSET(daObjYboil_c,mBtkAnm,0x1FD4);
WWHD_OFFSET(daObjYboil_c,mBrkAnm,0x367C);
WWHD_OFFSET(daObjYboil_c,mMdlTimer,0x4DEC);
WWHD_OFFSET(daObjYboil_c,mMdlPos,0x4EB4);
WWHD_OFFSET(daObjYboil_c,mMdlScale,0x510C);
WWHD_OFFSET(daObjYboil_c,mScaleMax,0x5364);
WWHD_SIZE(daObjYboil_c,0x536C);

namespace yboil_layout {
struct FloatColor { be<f32> r,g,b,a; };
struct FloatColorBits { be<u32> r,g,b,a; };
struct TevVirtuals {
 u8 padSet[0x24]; gfn setTevColor;
 u8 padGet[0x34-0x28]; gfn getTevColor;
 u8 padSetK[0x3C-0x38]; gfn setTevKColor;
 u8 padGetK[0x4C-0x40]; gfn getTevKColor;
};
struct TevBlock {
 u8 padVtable[4]; gptr<TevVirtuals> vtable;
 GXColorS10* getTevColor() { return gabi::call_ptr<GXColorS10*>(vtable->getTevColor.get(),this,0); }
 void setTevColor(GXColorS10* color) { gabi::call_ptr<void>(vtable->setTevColor.get(),this,0,color); }
 GXColor* getTevKColor() { return gabi::call_ptr<GXColor*>(vtable->getTevKColor.get(),this,0); }
 void setTevKColor(GXColor* color) { gabi::call_ptr<void>(vtable->setTevKColor.get(),this,0,color); }
};
struct ShaderParams { be<u32> flags; };
struct Material {
 u8 padNext[4]; gptr<Material> mNext;
 u8 padTev[0x18-8]; gptr<TevBlock> mTevBlock;
 u8 padShader[0xA0-0x1C]; ShaderParams mShaderParams;
};
struct Joint { u8 padMesh[0x10]; gptr<Material> mMesh; };
struct ModelData { u8 padJoints[8]; gptr<Joint> mJointNodes; };
// HD: the inline BCK constructor owns a matrix calculator and its state slots.
struct BckConstructorView {
 J3DFrameCtrl mFrameCtrl;
 be<u32> __vtbl;
 u8 mCalc[0x34];
 be<u32> __vtblAnm;
 u8 padTransform[0xC]; be<u32> mTransform;
 u8 padState[0x20]; be<u32> mCalcState[3]; be<u32> mpAnm;
};
WWHD_OFFSET(BckConstructorView,__vtbl,0x10);
WWHD_OFFSET(BckConstructorView,mCalc,0x14);
WWHD_OFFSET(BckConstructorView,__vtblAnm,0x48);
WWHD_OFFSET(BckConstructorView,mTransform,0x58);
WWHD_OFFSET(BckConstructorView,mCalcState,0x7C);
WWHD_OFFSET(BckConstructorView,mpAnm,0x88);
WWHD_SIZE(BckConstructorView,0x8C);
}
