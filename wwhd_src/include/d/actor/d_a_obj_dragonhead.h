/* Dragonhead, WWHD layout. */
#pragma once
#include "gc_api/gc_api.h"
namespace daObjDragonhead {
struct Act_c : fopAc_ac_c {
  /* 0x3AC */ request_of_phase_process_class mPhs;
  /* 0x3B4 */ gptr<J3DModel> mpModel;
  /* 0x3B8 */ dCcD_Stts mStts;
  /* 0x3F4 */ dCcD_Sph mSph;
  /* 0x520 */ be<u8> mAlpha;
  /* 0x521 */ be<u8> mSwitchOn;
  /* 0x522 */ u8 padAlpha[2];
  /* 0x524 */ gptr<dBgW> mpBgW;
  /* 0x528 */ be<u8> field_0x40c; // tww: background registered flag
  /* 0x529 */ u8 padBgW[3];
  /* 0x52C */ cXyz mSphCenter;
  /* 0x538 */ Mtx34 mtx;
  BOOL CreateHeap();
  void set_mtx();
  void CreateInit();
};
WWHD_OFFSET(Act_c, mStts, 0x3B8);
WWHD_OFFSET(Act_c, mSph, 0x3F4);
WWHD_OFFSET(Act_c, mAlpha, 0x520);
WWHD_OFFSET(Act_c, mtx, 0x538);
WWHD_SIZE(Act_c, 0x568);
struct FloatColor { be<f32> r,g,b,a; };
// HD: these material operations are virtual, followed by a shader-parameter update.
struct DragonTevVirtuals {
  u8 padSet[0x3C]; gfn setTevKColor;
  u8 padGet[0x4C-0x40]; gfn getTevKColor;
};
struct DragonTevBlock {
  u8 padVtable[4]; gptr<DragonTevVirtuals> vtable;
  GXColor* getTevKColor(u32 index) {
    return gabi::call_ptr<GXColor*>(vtable->getTevKColor.get(), this, index);
  }
  void setTevKColor(u32 index, GXColor* color) {
    gabi::call_ptr<void>(vtable->setTevKColor.get(), this, index, color);
  }
};
struct DragonShaderParams { be<u32> flags; };
struct DragonMaterialView : J3DMaterial {
  u8 padTev[0x18-0xC]; gptr<DragonTevBlock> mTevBlock;
  u8 padParams[0xA0-0x1C]; DragonShaderParams mShaderParams;
};
WWHD_OFFSET(DragonMaterialView, mTevBlock, 0x18);
WWHD_OFFSET(DragonMaterialView, mShaderParams, 0xA0);
struct DragonModelResource { u8 padMaterialCount[0x24]; be<u16> mMaterialNum; };
struct DragonHeaderStatics {
  be<f32> piMin,piMax;
  u8 initializeA,initializeB;
  u8 padZeros[2]; be<u32> zero[4];
};
WWHD_OFFSET(DragonHeaderStatics, zero, 0xC);
}
