// Medli NPC actions. Private, non-owning views over the verified WWHD layouts.
#pragma once
#include "d/actor/d_a_npc_md.h"
#include "gc_api/f_op/f_op_actor.h"
namespace MdNpc {
enum Type { TYPE_ATORIZK = 0, TYPE_ADANMAE = 1, TYPE_M_DRA09 = 2, TYPE_SEA = 3, TYPE_EDAICHI = 4, TYPE_M_DAI = 5, TYPE_M_DAIB = 6, TYPE_SHIP_RIDE = 7 };
enum EventFlag {
    MEDLI_GAVE_FATHERS_LETTER = 0x0E02,
    UNK_1104 = 0x1104,
    UNK_1101 = 0x1101,
    UNK_1140 = 0x1140,
    UNK_1402 = 0x1402,
    UNK_1102 = 0x1102,
    UNK_1280 = 0x1280,
    UNK_4001 = 0x4001,
    UNK_4180 = 0x4180,
};
enum PlayerStatus { PLAYER_SHIP_RIDE = 0x10000, PLAYER_UNKNOWN_800000 = 0x800000 };
enum Sound {
    JA_SE_CV_MD_LIFT_UP = 0x48A5,
    JA_SE_CV_MD_LANDING = 0x48A7,
    JA_BGM_MEDORI_TAKT_8 = 0x80000060,
    JA_SE_CV_MD_CRASH = 0x48A9,
    JA_SE_CV_MD_THROW = 0x48A6,
    JA_SE_CV_MD_FLY_WITH_LINK = 0x491F,
    JA_SE_CV_MD_FLY_END = 0x4920,
};
constexpr u32 AT_TYPE_WIND = 0x200000;
struct Camera { u8 mPadding[0x248]; u8 mCamera; };
WWHD_OFFSET(Camera, mCamera, 0x248);
WWHD_SIZE(Camera, 0x249);
struct Play {
    u8 mPadding[0x5AF8]; gptr<Camera> mpCamera;
    u8 mPad5AFC[0x5B2C-0x5AFC]; gptr<fopAc_ac_c> mpPlayer;
    u8 mPad5B30[4]; gptr<daPy_py_l> mpLink;
    u8 mPad5B38[4]; gptr<fopAc_ac_c> mpShip;
    u8 mPad5B40[0x5CD8-0x5B40]; be<u32> mPlayerStatus0;
};
WWHD_OFFSET(Play, mpCamera, 0x5AF8);
WWHD_OFFSET(Play, mpPlayer, 0x5B2C);
WWHD_OFFSET(Play, mpLink, 0x5B34);
WWHD_OFFSET(Play, mpShip, 0x5B3C);
WWHD_OFFSET(Play, mPlayerStatus0, 0x5CD8);
WWHD_SIZE(Play, 0x5CDC);
inline Play* play() { return reinterpret_cast<Play*>(dComIfGp_get()); }
struct Morf2 { u8 mPadding[0xA4]; J3DFrameCtrl mFrameCtrl; be<f32> mCurMorf; };
WWHD_OFFSET(Morf2, mFrameCtrl, 0xA4);
WWHD_OFFSET(Morf2, mCurMorf, 0xB4);
WWHD_SIZE(Morf2, 0xB8);
inline Morf2* morf2(mDoExt_McaMorf2* m) { return reinterpret_cast<Morf2*>(m); }
struct PlayerVtable { u8 mPadding[0xA4]; gfn getBaseAnimeFrame; };
WWHD_OFFSET(PlayerVtable, getBaseAnimeFrame, 0xA4);
WWHD_SIZE(PlayerVtable, 0xA8);
struct Player : fopAc_ac_c {
    f32 getBaseAnimeFrame() { return gabi::call_ptr<f32>(gabi::at<PlayerVtable>(__vtbl)->getBaseAnimeFrame.get(), this); }
};
WWHD_SIZE(Player, 0x3AC);
struct CollisionObject { u8 mPadding[0x10]; be<u32> mAtType; };
WWHD_OFFSET(CollisionObject, mAtType, 0x10);
WWHD_SIZE(CollisionObject, 0x14);
struct CollisionVector { u8 mPadding[0xC0]; cXyz mTgRVec; };
WWHD_OFFSET(CollisionVector, mTgRVec, 0xC0);
WWHD_SIZE(CollisionVector, 0xCC);
struct SaveInfo { u8 mPadding[0x644]; u8 mEvent; };
WWHD_OFFSET(SaveInfo, mEvent, 0x644);
WWHD_SIZE(SaveInfo, 0x645);
struct SavePointer { gptr<SaveInfo> mpInfo; };
WWHD_SIZE(SavePointer, 4);
struct ActionWords { union { gabi::ptmf<BOOL (daNpc_Md_c::*)(void*)> fn; be<u32> words[2]; }; };
WWHD_OFFSET(ActionWords, fn, 0);
WWHD_OFFSET(ActionWords, words, 0);
WWHD_SIZE(ActionWords, 8);
}
void mdNpc_forceLockOff(void* camera, u32 actorId);
void mdNpc_bgmStart(u32 sound);
void mdNpc_monsSeStart(u32 sound, cXyz* pos, u32 actorId, s32 reverb);
u8 mdNpc_chkAttentionByte(daNpc_Md_c* actor, cXyz* pos, s16 angle, s32 mode);
u32 mdNpc_chkAttentionWord(daNpc_Md_c* actor, cXyz* pos, s16 angle, s32 mode);
void mdNpc_normalizeZP(cXyz* vector, cXyz* result);
void mdNpc_getAllWind(cXyz* pos, cXyz* vector, be<f32>* power);
BOOL mdNpc_chkTgHit(dCcD_GObjInf* object);
u32 mdNpc_getTgHitObj(dCcD_GObjInf* object);
s16 mdNpc_calcTimer(be<s16>* timer);
void mdNpc_lookAtTarget(dNpc_JntCtrl_c* control, be<s16>* outY, cXyz* target, cXyz* eye, s16 angle, s16 velocity, u8 headOnly);
s32 mdNpc_checkAutoJumpFlying(daPy_py_l* link);
void mdNpc_setRestart(fopAc_ac_c* actor, s8 option);
void mdNpc_setMorf(mDoExt_McaMorf2* morf, f32 value);
s32 mdNpc_windProcWord(daNpc_Md_c* actor);
