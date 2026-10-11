#pragma once
#include "d/actor/d_a_npc_md.h"
#include "gc_api/f_op/f_op_actor.h"
#include "gc_api/d/d_com_inf_game.h"
#include "SSystem/SComponent/c_m3d_g_pla.h"
namespace MdMain {
using daNpc_Md_HIO_c = daNpc_Md_HIO_l;
using daNpc_Md_HIO2_c = daNpc_Md_HIO2_l;
using daNpc_Md_HIO3_c = daNpc_Md_HIO3_l;
using daNpc_Md_HIO4_c = daNpc_Md_HIO4_l;
using daNpc_Md_HIO5_c = daNpc_Md_HIO5_l;
using daNpc_Md_HIO6_c = daNpc_Md_HIO6_l;
struct Actor : daNpc_Md_c { BOOL returnToPlayerNpcAction(void*); };
WWHD_SIZE(Actor, 0x43DC);
struct ActionWords { union { gabi::ptmf<BOOL(daNpc_Md_c::*)(void*)> fn; be<u32> mWords[2]; }; };
WWHD_OFFSET(ActionWords, fn, 0);
WWHD_SIZE(ActionWords, 8);
struct Attention { u8 padding[0x20]; be<u32> mFlags; };
WWHD_OFFSET(Attention, mFlags, 0x20);
WWHD_SIZE(Attention, 0x24);
struct Morf2 { u8 padding[0xB4]; be<f32> mCurMorf; };
WWHD_OFFSET(Morf2, mCurMorf, 0xB4);
WWHD_SIZE(Morf2, 0xB8);
struct SaveInfo { u8 padding[0x644]; be<u8> mEvent; };
WWHD_OFFSET(SaveInfo, mEvent, 0x644);
WWHD_SIZE(SaveInfo, 0x645);
struct SavePointer { gptr<SaveInfo> mpInfo; };
WWHD_OFFSET(SavePointer, mpInfo, 0);
WWHD_SIZE(SavePointer, 4);
struct PlaneOwner { u8 padding[0x14]; u8 mPlane; };
WWHD_OFFSET(PlaneOwner, mPlane, 0x14);
WWHD_SIZE(PlaneOwner, 0x15);
struct ModelOwner { u8 padding[4]; u8 mModel[0x154]; u8 mModelData; };
WWHD_OFFSET(ModelOwner, mModel, 4);
WWHD_OFFSET(ModelOwner, mModelData, 0x158);
WWHD_SIZE(ModelOwner, 0x159);
inline void copyFloatWord(be<f32>* dst, const be<f32>* src) {
    *reinterpret_cast<be<u32>*>(dst) = *reinterpret_cast<const be<u32>*>(src);
}
enum Sound { JA_SE_CV_MD_LIFT_UP = 0x48A5, JA_SE_CM_MD_PIYO = 0x50BC,
    JA_SE_CV_MD_CRASH = 0x48A9, JA_SE_CV_MD_LANDING = 0x48A7,
    JA_SE_CV_MD_LEFT_ALONE = 0x491E, JA_SE_CV_MD_FLY_END = 0x4920 };
enum EventFlag { UNK_3320 = 0x3320, UNK_3310 = 0x3310 };
constexpr u32 TALK_ATTENTION = fopAc_Attn_LOCKON_TALK_e | fopAc_Attn_ACTION_SPEAK_e;
constexpr u32 GROUP_PASS_VTABLE = 0x1001D708;
constexpr u32 LINE_POLY_INFO_VTABLE = 0x1001D6F8;
constexpr u32 HIO6_VTABLE = 0x1001D7D8;
constexpr u32 HIO5_VTABLE = 0x1001D7F0;
constexpr u32 HIO4_VTABLE = 0x1001D808;
constexpr u32 HIO3_VTABLE = 0x1001D820;
constexpr u32 HIO2_VTABLE = 0x1001D838;
constexpr u32 HIO_VTABLE = 0x1001D850;
constexpr u32 HEADER_STATICS = 0x10467A38;
constexpr u32 HEADER_STATICS_INIT = 0x101C1EEC;
constexpr u32 GROUND_CHECK_VTABLE = 0x1001D718;
constexpr u32 GROUND_POLY_INFO_VTABLE = 0x1001D728;
constexpr u32 GROUND_POLY_PASS_VTABLE = 0x1001D748;
constexpr u32 GROUND_GROUP_PASS_VTABLE = 0x1001D738;
constexpr u32 LINE_CHECK_VTABLE = 0x1001D758;
constexpr u32 LINE_BASE_VTABLE = 0x1001D768;
constexpr u32 LINE_POLY_PASS_VTABLE = 0x1001D788;
constexpr u32 LINE_GROUP_PASS_VTABLE = 0x1001D778;
}
void mdMain_monsSeStart(u32 sound, cXyz* pos, u32 process, s32 reverb);
void mdMain_outprod(const cXyz* first, cXyz* result, const cXyz* second);
void mdMain_constructNpcHio(dNpc_HIO_l* hio);
void mdMain_destroyPlane(void* plane, s32 flags);
void mdMain_destroyModelData(void* data, s32 flags);
void mdMain_destroyModel(void* model, s32 flags);
s16 mdMain_calcTimer(be<s16>* timer);
u8 mdMain_checkAttention(daNpc_Md_c* actor, cXyz* pos, s16 angle, int mode);
void mdMain_lookAtTarget(dNpc_JntCtrl_c* control, be<s16>* outY, cXyz* target, cXyz* eye, s16 angle, s16 velocity, bool headOnly);
void mdMain_destroyGroundCheck(void* check, s32 flags);
void mdMain_destroyLineCheck(void* check, s32 flags);
void mdMain_offsetPos(cXyz* dst, cXyz* base, s16 angle, cXyz* offset);
f32 mdMain_stickValue(s32 port);
u8 mdMain_lockonTruth(MdMain::Attention* attention);
void mdMain_setRestart(daNpc_Md_c* actor, s8 option);
u8 mdMain_calcTimerU8(be<u8>* timer);
s32 mdMain_getRndValue(s32 base, s32 range);
s16 mdMain_getMaxFlyingTimer();
BOOL mdMain_trigA(s32 port);
BOOL mdMain_trigB(s32 port);
