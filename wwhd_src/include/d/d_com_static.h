/** Shared actor statics from d_com_static; HD keeps full-register ABI adapters
 * for GameCube static methods until autoglue supports marked static members. */
#pragma once
#include "bindings.h"
#include "gc_api/f_op/f_op_actor.h"
namespace ComStatic {
struct SalvageControl { u8 opaque; };
struct SaveInfo {
    u8 prefix[0x20];
    u8 mSavedata[0x90];
    u8 mGetBagItem[0x644 - 0xB0];
    u8 mEvent[0x100];
};
struct HeaderStatics {
    be<f32> mNegativePi, mPositivePi;
    be<u8> mFirstObject, mSecondObject;
    u8 padding[2];
    be<u32> mZero[4];
};
struct SinCos { be<f32> sin, cos; };
struct SinCosTable { SinCos m[8192]; };
struct IballActors { be<u32> m_ib_actor[5]; };
}
void SalvageControl_init_room(ComStatic::SalvageControl*, u32);
void daIball_dead(fopAc_ac_c*);
s32 PlayerGetBagItem_isReserve(void*, u32);
s32 ComStatic_isEventBit(void*, u32);
void ComStaticSmoke_delete(void*, u32);
void ComStaticFirstHeaderObject_ct(be<u8>*);
void ComStaticSecondHeaderObject_ct(be<u8>*);

WWHD_SIZE(ComStatic::SinCos, 8);
WWHD_SIZE(ComStatic::IballActors, 20);
WWHD_SIZE(ComStatic::HeaderStatics, 0x1C);
WWHD_OFFSET(ComStatic::SaveInfo, mSavedata, 0x20);
WWHD_OFFSET(ComStatic::SaveInfo, mGetBagItem, 0xB0);
WWHD_OFFSET(ComStatic::SaveInfo, mEvent, 0x644);

// HD: GameCube tag-item tail follows the enlarged 0x3AC actor base.
class daTagKbItem_c : public fopAc_ac_c {
public:
    void dig_main();
    void kb_dig(fopAc_ac_c*);
    /* 0x3AC */ request_of_phase_process_class mPhase;
    /* 0x3B4 */ be<u8> field_0x298;
    /* 0x3B5 */ be<u8> field_0x299;
    u8 padding[2];
    /* 0x3B8 */ be<u32> mItemBitNo;
    /* 0x3BC */ be<u8> mItemNo, mEnemyKind;
    u8 padding_3BE[2];
    /* 0x3C0 */ be<u32> mSwBitNo;
    /* 0x3C4 */ gptr<fopAc_ac_c> mpActor;
};
namespace ComStatic {
struct StandItem : fopAc_ac_c {
    u8 prefix[0x734 - 0x3AC];
    be<u8> mItemNo;
    u8 padding[0x74C - 0x735];
    Mtx34 m630, m660;
    u8 padding_7AC[0x7D0 - 0x7AC];
    be<s16> m6B4;
};
struct Joint { u8 prefix[4]; be<u16> mJointNo; };
struct MtxBuffer {
    u8 prefix[4];
    be<u16> mFlags;
    u8 padding[0x10 - 6];
    gptr<Mtx34> mAnmMtx;
};
struct StandModel {
    u8 prefix[0x2C];
    gptr<MtxBuffer> mMtxBuffer;
    u8 padding[0xB8 - 0x30];
    gptr<StandItem> mUserArea;
};
}
s32 ComStatic_isSwitch(void*, u32, s32);
void ComStatic_onSwitch(void*, u32, s32);
s32 ComStatic_IsActor(fopAc_ac_c*);
f64 ComStatic_cM_rndF(f32);
f64 ComStatic_cM_rndFX(f32);
fopAc_ac_c* ComStatic_fastCreateItem(cXyz*, u32, s32, csXyz*, cXyz*, u32, u32, f64, f32, f32);
ComStatic::Joint* ComStatic_getJoint(J3DNode*);
WWHD_SIZE(daTagKbItem_c, 0x3C8);
WWHD_OFFSET(daTagKbItem_c, mItemNo, 0x3BC);
WWHD_OFFSET(daTagKbItem_c, mpActor, 0x3C4);
WWHD_OFFSET(ComStatic::StandItem, mItemNo, 0x734);
WWHD_OFFSET(ComStatic::StandItem, m630, 0x74C);
WWHD_OFFSET(ComStatic::StandItem, m660, 0x77C);
WWHD_OFFSET(ComStatic::StandItem, m6B4, 0x7D0);
WWHD_OFFSET(ComStatic::StandModel, mMtxBuffer, 0x2C);
WWHD_OFFSET(ComStatic::StandModel, mUserArea, 0xB8);
WWHD_SIZE(ComStatic::MtxBuffer, 0x14);

namespace ComStatic {
// HD: named view of the light prefix copied into three positions in tevstr.
// The eight parameter slots remain opaque; this TU only copies their values.
struct LightParameters {
    cXyz mPosition, mDirection;
    GXColor mColor;
    GXColorS10 mMaterialColor;
    be<f32> mParameters[8];
};
struct SmokeTevstr {
    LightParameters mLight;
    u8 padding_44[0xC0 - 0x44];
    LightParameters mHdCopy[1];
    u8 padding_104[0x144 - 0x104];
    LightParameters mHdCopy2;
    u8 padding_188[0x1C8 - 0x188];
};
struct SmokeCallback { u8 data[0x20]; };
struct PiPair { be<f32> negative, positive; };
}
void ComStaticSmoke_ct(ComStatic::SmokeCallback*, u32, u32, u32, u32);
WWHD_SIZE(ComStatic::LightParameters, 0x44);
WWHD_OFFSET(ComStatic::LightParameters, mParameters, 0x24);
WWHD_SIZE(ComStatic::SmokeTevstr, 0x1C8);
WWHD_OFFSET(ComStatic::SmokeTevstr, mHdCopy, 0xC0);
WWHD_OFFSET(ComStatic::SmokeTevstr, mHdCopy2, 0x144);
