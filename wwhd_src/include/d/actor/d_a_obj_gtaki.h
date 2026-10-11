/* Ganon waterfall, WWHD layout. */
#pragma once
#include "gc_api/gc_api.h"
class daObjGtaki_c : public fopAc_ac_c {
public:
  /* 0x3AC */ request_of_phase_process_class mPhase;
  /* 0x3B4 */ gptr<J3DModel> mpModel;
  /* 0x3B8 */ mDoExt_btkAnm mBtkAnm;
  /* 0x42C */ dCcD_Stts mStts;
  /* 0x468 */ dCcD_Cyl mCyl;
  /* 0x598 */ gptr<dBgW> mpBgW;
  /* 0x59C */ Mtx34 mMtx;
  /* 0x5CC */ dKy_tevstr_c mTevStr;
  void set_mtx();
  s32 _create();
  s32 _delete();
  s32 _draw();
  s32 _execute();
  s32 CreateInit();
  s32 CreateHeap();
  void setDummyTexture();
};
WWHD_OFFSET(daObjGtaki_c,mTevStr,0x5CC);
WWHD_SIZE(daObjGtaki_c,0x794);
namespace gtaki_layout {
// HD: ResTIMG carries a direct buffer pointer in addition to the GC header.
struct TextureImage {
  be<u8> format,alphaEnabled;
  be<u16> width,height;
  be<u8> wrapS,wrapT,indexTexture,colorFormat;
  be<u16> numColors;
  be<u32> paletteOffset;
  be<u8> mipmapEnabled,doEdgeLOD,biasClamp,maxAnisotropy,minFilter,magFilter;
  be<s8> minLOD,maxLOD;
  be<u8> mipmapCount,unknown;
  be<s16> LODBias;
  be<u32> imageOffset;
  be<u32> buffer;
};
WWHD_SIZE(TextureImage,36);
struct TextureWords { be<u32> word[9]; };
struct TextureTable {
  be<u16> mNum; u8 padImages[2]; gptr<TextureImage> mpRes;
  be<u32> modifiedHigh,modifiedLow,modifiedSecondHigh,modifiedSecondLow;
  be<u32> maskHigh,maskLow;
  // HD: the assert may return; GHS still reads the count at guest address zero.
  u16 getNum() { return mNum; }
};
struct ModelDataView : J3DModelData {
  u8 padTextures[0x30-0x14];
  gptr<TextureTable> mTexture;
  gptr<JUTNameTab> mTextureName;
  u8 padAnm[0x44-0x38];
  be<u32> mTextureSRTAnm;
};
struct CaptureBuffer { u8 padSize[8]; be<u32> width,height; };
struct LightPrefix { be<f32> vectors[6]; be<u8> color[4]; be<s16> color16[4]; be<f32> factors[8]; };
WWHD_SIZE(LightPrefix,0x44);
struct TevConstructorView {
  LightPrefix light;
  u8 padLightA[0xC0-0x44]; LightPrefix hdLightA;
  u8 padLightB[0x144-0x104]; LightPrefix hdLightB;
};
struct EmitterView : JPABaseEmitter {
  u8 padDynamics[0x220-0x70]; cXyz mGlobalDynamicsScale;
  u8 padParticle[0x238-0x22C]; cXyz mGlobalParticleScale;
  GXColor mGlobalPrmColor;
};
struct HeaderStatics {
  be<f32> piMin,piMax;
  u8 initializeA,initializeB,padZeros[2]; be<u32> zero[4];
};
struct DrawListView { u8 padInvisible[0x5C]; be<u32> invisibleOpa,invisibleXlu; };
}
