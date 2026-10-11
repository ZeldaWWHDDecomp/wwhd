/* d_com_inf_game (GameCube names) -> WWHD: the play object behind dComIfGp_get() (025200D4, called
 * at every access; arguments are evaluated before it, as in the HD code), the save data behind
 * the pointer at 0x101F84DC. Already in d/d_com_inf_game.h / bindings.h: dComIfG_resLoad,
 * dComIfG_resDelete, dComIfG_Bgsp, dComIfGp_getPlayer, dComIfGp_evmng_cutEnd / endCheck /
 * getIsAddvance, dComIfGp_event_reset, dComIfGp_getReverb, dComIfGd_setList. */
#pragma once
#include "bindings.h"
#include "gc_api/tu.h"
#include "gc_api/m_Do/m_Do_ext.h"

/* ---- resources ---- */
/* HD: the archive name goes in a sead::SafeString temporary with this TU's vtable [v nz] */
inline void* dComIfG_getObjectRes(const char* arc, s32 index) { return dComIfG_getObjectRes(arc, index, gc_tu.safeString); }
#define dComIfG_resDeleteDemo dComIfG_resDelete

/* ---- events (dComIfG_play_c::mEvtManager) ---- */
inline s16 dComIfGp_evmng_getEventIdx(const char* name) { return dComIfGp_evmng_getEventIdx(name, 0xFF); }
inline s32 dComIfGp_evmng_getMyStaffId(const char* name) { return dComIfGp_evmng_getMyStaffId(name, nullptr, 0); }
/* 02542EDC getMyActIdx(staffIdx, action table, count, force, ...) [v nz] */
inline s32 dComIfGp_evmng_getMyActIdx(s32 staffIdx, gptr<char>* actions, int num, BOOL force, int p5) {
    return gabi::call<s32>(0x02542EDC, dComIfGp_getPEvtManager(), staffIdx, actions, num, force, p5);
}
/* getMySubstanceP(staffIdx, name, 3 = integer) [v nz] */
inline be<s32>* dComIfGp_evmng_getMyIntegerP(s32 staffIdx, const char* name) {
    return (be<s32>*)dComIfGp_evmng_getMySubstanceP(staffIdx, name, 3);
}
/* dComIfGp_event_setGtItm: dEvt_control_c::mGetItemNo [v nz] */
inline void dComIfGp_event_setGtItm(u8 itemNo) { gabi::store<u8>(dComIfGp_ea() + PLAY_OFF(mEvtCtrl.mGetItemNo), itemNo); }

/* ---- play object fields (HD offsets) [v nz] ---- */
inline u8 dComIfGp_getSelectItem(int btn) { return gabi::load<u8>(dComIfGp_ea() + PLAY_OFF(mSelectItem) + btn); }
inline BOOL dComIfGp_checkMesgSendButton() { return gabi::load<u8>(dComIfGp_ea() + PLAY_OFF(mMesgSendButton)); }
inline s16 dComIfGp_getMessageRupee() { return gabi::load<s16>(dComIfGp_ea() + PLAY_OFF(mMessageRupee)); }
inline void dComIfGp_setItemRupeeCount(s32 count) {
    u32 a = dComIfGp_ea() + PLAY_OFF(mItemRupeeCount);
    gabi::store<s32>(a, gabi::load<s32>(a) + count);
}

/* ---- save data (HD: *(0x101F84DC); dSv_info_c at +0x20, d/d_save.h) [v nz] ---- */
inline u32 dComIfGs_ea() { return gabi::load<u32>(0x101F84DC); }
inline u16 dComIfGs_getRupee() { return gabi::load<u16>(dComIfGs_ea() + SAVE_OFF(mSavedata.mPlayer.mPlayerStatusA.mRupee)); }
inline u8 dComIfGs_getItem(int slot) { return gabi::load<u8>(dComIfGs_ea() + SAVE_OFF(mSavedata.mPlayer.mPlayerItem) + slot); }
/* HD inline: any of the four bottle slots (0xE..0x11) not empty */
inline BOOL dComIfGs_checkGetBottle() {
    u8 n = 0;
    for (int i = 0; i < 4; i++)
        if (dComIfGs_getItem(0xE + i) != 0xFF) n++;
    return n != 0;
}
inline BOOL dComIfGs_checkGetItem(u8 itemNo) { return gabi::call<BOOL>(0x02520C0C, itemNo); }
inline BOOL dComIfGs_checkEmptyBottle() { return gabi::call<BOOL>(0x025B5C54, dComIfGs_ea() + SAVE_OFF(mSavedata.mPlayer.mPlayerItem)); }
inline BOOL dComIfGs_checkBaitItemEmpty() { return gabi::call<BOOL>(0x025B6E90, dComIfGs_ea() + SAVE_OFF(mSavedata.mPlayer.mBagItem)); }
inline u8 dComIfGs_getArrowNum() { return gabi::load<u8>(dComIfGs_ea() + SAVE_OFF(mSavedata.mPlayer.mItemRecord.mItemRecord2.mArrowNum)); }
inline u8 dComIfGs_getBombNum() { return gabi::load<u8>(dComIfGs_ea() + SAVE_OFF(mSavedata.mPlayer.mItemRecord.mItemRecord2.mBombNum)); }
inline u8 dComIfGs_getArrowMax() { return gabi::load<u8>(dComIfGs_ea() + SAVE_OFF(mSavedata.mPlayer.mItemMax.mItemMax2.mArrowNum)); }
inline u8 dComIfGs_getBombMax() { return gabi::load<u8>(dComIfGs_ea() + SAVE_OFF(mSavedata.mPlayer.mItemMax.mItemMax2.mBombNum)); }

/* ---- draw lists ---- */
/* HD: j3dSys opa/xlu buffers <- play+0x5D84/0x5D88 [v nz] */
inline void dComIfGd_setListMaskOff() {
    gabi::store<u32>(0x104B4634, gabi::load<u32>(dComIfGp_ea() + 0x5D84));
    gabi::store<u32>(0x104B4638, gabi::load<u32>(dComIfGp_ea() + 0x5D88));
}
/* HD: the line material's sort packet (play+0x5FB4, 0x9C each by material id) [v nz] */
inline void dComIfGd_set3DlineMat(mDoExt_3DlineMat1_c* mat) {
    u32 packets = dComIfGp_ea() + 0x5FB4;
    int id = mat->getMaterialID();
    gabi::call(0x025EDD04, packets + id * 0x9C, mat);
}

/* dEvt_control_c::mMode, nonzero while an event runs [v d_a_agbsw0] */
inline BOOL dComIfGp_event_runCheck() { return gabi::load<u8>(dComIfGp_ea() + PLAY_OFF(mEvtCtrl.mMode)) != 0; }

/* ---- save events (dSv_info_c::mSavedata.mEvent / mTmp; HD re-reads *(0x101F84DC) at each use, as
 * the raw units' local copies do) [g; the local copies are verified in npc_photo, npc_people, ship] ---- */
inline dSv_event_c* dComIfGs_getEvent() { return gabi::at<dSv_event_c>(dComIfGs_ea() + SAVE_OFF(mSavedata.mEvent)); }
inline dSv_event_c* dComIfGs_getTmpEvent() { return gabi::at<dSv_event_c>(dComIfGs_ea() + SAVE_OFF(mTmp)); }
inline BOOL dComIfGs_isEventBit(u16 id) { return dComIfGs_getEvent()->isEventBit(id); }
inline void dComIfGs_onEventBit(u16 id) { dComIfGs_getEvent()->onEventBit(id); }
inline u8 dComIfGs_getEventReg(u16 reg) { return dComIfGs_getEvent()->getEventReg(reg); }
inline void dComIfGs_setEventReg(u16 reg, u8 v) { dComIfGs_getEvent()->setEventReg(reg, v); }
inline BOOL dComIfGs_isTmpBit(u16 id) { return dComIfGs_getTmpEvent()->isEventBit(id); }
inline void dComIfGs_onTmpBit(u16 id) { dComIfGs_getTmpEvent()->onEventBit(id); }

/* ---- magma floor [v d_a_magma] ---- */
#include "gc_api/d/d_magma.h"
/* 02524C10: HD play object's createMagma subobject at +0x12A0 [v d_a_magma] */
inline BOOL dComIfGp_createMagma() { return gabi::call<BOOL>(0x02524C10, dComIfGp_ea() + (u32)offsetof(dComIfG_inf_c, play)); }
/* HD: magma packet pointer at play+0x5AB4; retain one play accessor per use [v d_a_magma] */
inline dMagma_packet_c* dComIfGp_getMagma() { return gabi::at<dMagma_packet_c>(gabi::load<u32>(dComIfGp_ea() + 0x5AB4)); }

/* =====================================================================================================
 * The tww d_com_inf_game API. Every accessor below is the HD form that verified units use
 * (their local copies, gathered from the unit headers: the address/offset is checked by those units),
 * written on the dComIfG_play_c / dSv_info_c members (d/d_com_inf_game.h, d/d_save.h). The accessor
 * re-reads dComIfGp_get() / *(0x101F84DC) at every use, as HD does. Accessors on a member marked [i]
 * (offset inferred from the GameCube order) carry [i]: the first unit using one verifies it.
 * Accessors are added here only once their HD form is verified.
 * ===================================================================================================== */

/* ---- resources ---- */
/* 026067F4 dRes_control_c::getIDRes(SafeString arc, s32 id) [v npc_kf1, ko1, gp1, ...] */
inline void* dComIfG_getObjectIDRes(const char* arc, s32 id) {
    gabi::Local<SafeString> key;
    key->mStringTop = gabi::guest_addr(arc);
    key->__vtbl = gc_tu.safeString;
    return gabi::call<void*>(0x026067F4, dComIfG_resControl(), key.get(), id);
}
inline void* dComIfG_getStageRes(const char* arc, const char* name) { return gabi::call<void*>(0x0252447C, arc, name); } /* [v vrbox, bg_heap] */
inline s32 dComIfG_syncStageRes(const char* arc) { return gabi::call<s32>(0x02523D08, arc); }                         /* [v vrbox] */

/* ---- stage ---- */
inline const char* dComIfGp_getStartStageName() { return gabi::at<const char>(dComIfGp_ea() + PLAY_OFF(mCurStage.mName)); } /* [v mo2, hole] */
inline s16 dComIfGp_getStartStagePoint() { return gabi::load<s16>(dComIfGp_ea() + PLAY_OFF(mCurStage.mPoint)); }            /* [v npc_photo] */
inline s8 dComIfGp_getStartStageRoomNo() { return gabi::load<s8>(dComIfGp_ea() + PLAY_OFF(mCurStage.mRoomNo)); }            /* [i] */
inline s8 dComIfGp_getStartStageLayer() { return gabi::load<s8>(dComIfGp_ea() + PLAY_OFF(mCurStage.mLayer)); }              /* [v bmcon1] */
/* dStage_stageDt_c::getStagInfo: virtual, vtable +0x15C [v fallrock_tag, shutter, tsubo, warpf, ep] */
inline void* dComIfGp_getStageStagInfo() {
    u32 dt = dComIfGp_ea() + PLAY_STAGEDATA;
    return gabi::call_ptr<void*>(gabi::load<u32>(gabi::load<u32>(dt) + 0x15C), dt);
}
/* the room the player is in: HD a global (dStage_roomControl_c::mStayNo, 0x1047E6C8) [v am, pt, bk, cc, ...] */
inline s32 dComIfGp_roomControl_getStayNo() { return gabi::load<s8>(0x1047E6C8); }
/* 025C11DC dStage_roomControl_c::getStatusRoomDt(int) [v sea] */
inline void* dComIfGp_roomControl_getStatusRoomDt(int roomNo) { return gabi::call<void*>(0x025C11DC, dComIfGp_ea() + PLAY_ROOMCTRL, roomNo); }

/* ---- players, camera ---- */
struct camera_class;
struct J3DDrawBuffer;
inline fopAc_ac_c* dComIfGp_getCb1Player() { return gabi::at<fopAc_ac_c>(gabi::load<u32>(dComIfGp_ea() + PLAY_OFF(mpPlayerPtr[1]))); } /* [v mknjd] */
inline fopAc_ac_c* dComIfGp_getShipActor() { return gabi::at<fopAc_ac_c>(gabi::load<u32>(dComIfGp_ea() + PLAY_OFF(mpPlayerPtr[2]))); } /* [v hr, npc_people] */
inline s8 dComIfGp_getPlayerCameraID(int i) { return gabi::load<s8>(dComIfGp_ea() + PLAY_PLAYER + 4 + 8 * i); }                      /* [v bk_exec] */
inline camera_class* dComIfGp_getCamera(int i) { return gabi::at<camera_class>(gabi::load<u32>(dComIfGp_ea() + PLAY_OFF(mCameraInfo) + 0x34 * i)); } /* [v ship, bk_exec] */
inline BOOL dComIfGp_checkCameraAttentionStatus(int i, u32 flag) {
    return (gabi::load<u32>(dComIfGp_ea() + PLAY_OFF(mCameraInfo[0].mCameraAttentionStatus) + i * 0x34) & flag) != 0; /* [v ship, arrow_lighteff] */
}
inline u32 dComIfGp_getCameraAttentionStatus(int i) { return gabi::load<u32>(dComIfGp_ea() + PLAY_OFF(mCameraInfo[0].mCameraAttentionStatus) + i * 0x34); }
/* player status words (daPy_py_c mPlayerStatus[1][4]: status0 [0], status1 [1]) [v kamome, swtact, kanban, jbo, fm] */
inline u32 dComIfGp_getPlayerStatus0(int i = 0) { return gabi::load<u32>(dComIfGp_ea() + PLAY_OFF(mPlayerStatus) + 0x10 * i); }
inline u32 dComIfGp_getPlayerStatus1(int i = 0) { return gabi::load<u32>(dComIfGp_ea() + PLAY_OFF(mPlayerStatus) + 0x10 * i + 4); }
inline u32 dComIfGp_checkPlayerStatus0(int i, u32 flag) { return dComIfGp_getPlayerStatus0(i) & flag; } /* GameCube: the masked word */
inline u32 dComIfGp_checkPlayerStatus1(int i, u32 flag) { return dComIfGp_getPlayerStatus1(i) & flag; }
inline void dComIfGp_setPlayerStatus0(int i, u32 flag) { u32 a = dComIfGp_ea() + PLAY_OFF(mPlayerStatus) + 0x10 * i; gabi::store<u32>(a, gabi::load<u32>(a) | flag); }
inline void dComIfGp_setPlayerStatus1(int i, u32 flag) { u32 a = dComIfGp_ea() + PLAY_OFF(mPlayerStatus) + 0x10 * i + 4; gabi::store<u32>(a, gabi::load<u32>(a) | flag); }
inline void dComIfGp_clearPlayerStatus0(int i, u32 flag) { u32 a = dComIfGp_ea() + PLAY_OFF(mPlayerStatus) + 0x10 * i; gabi::store<u32>(a, gabi::load<u32>(a) & ~flag); }
inline void dComIfGp_clearPlayerStatus1(int i, u32 flag) { u32 a = dComIfGp_ea() + PLAY_OFF(mPlayerStatus) + 0x10 * i + 4; gabi::store<u32>(a, gabi::load<u32>(a) & ~flag); }

/* ---- packets (grass, tree, wood, flower, magma) ---- */
struct dGrass_packet_c;
struct dTree_packet_c;
struct dFlower_packet_c;
namespace dWood { struct Packet_c; }
/* 02524CC0 / 02524DC0 / 02524EC0 / 02524FC0 dComIfG_play_c::createGrass/Tree/Wood/Flower [v grass, obj_wood] */
inline void* dComIfGp_createGrass() { return gabi::call<void*>(0x02524CC0, dComIfGp_ea() + (u32)offsetof(dComIfG_inf_c, play)); }
inline void* dComIfGp_createTree() { return gabi::call<void*>(0x02524DC0, dComIfGp_ea() + (u32)offsetof(dComIfG_inf_c, play)); }
inline void* dComIfGp_createWood() { return gabi::call<void*>(0x02524EC0, dComIfGp_ea() + (u32)offsetof(dComIfG_inf_c, play)); }
inline void* dComIfGp_createFlower() { return gabi::call<void*>(0x02524FC0, dComIfGp_ea() + (u32)offsetof(dComIfG_inf_c, play)); }
inline dGrass_packet_c* dComIfGp_getGrass() { return gabi::at<dGrass_packet_c>(gabi::load<u32>(dComIfGp_ea() + PLAY_OFF(mpGrassPacket))); }
inline dTree_packet_c* dComIfGp_getTree() { return gabi::at<dTree_packet_c>(gabi::load<u32>(dComIfGp_ea() + PLAY_OFF(mpTreePacket))); }
inline dWood::Packet_c* dComIfGp_getWood() { return gabi::at<dWood::Packet_c>(gabi::load<u32>(dComIfGp_ea() + PLAY_OFF(mpWoodPacket))); }
inline dFlower_packet_c* dComIfGp_getFlower() { return gabi::at<dFlower_packet_c>(gabi::load<u32>(dComIfGp_ea() + PLAY_OFF(mpFlowerPacket))); }

/* ---- event control (dEvt_control_c) ---- */
inline u8 dComIfGp_event_getTalkXYBtn() { return gabi::load<u8>(dComIfGp_ea() + PLAY_OFF(mEvtCtrl.mTalkButton)); }        /* [v npc_*] */
inline BOOL dComIfGp_event_chkTalkXY() { return (u32)(dComIfGp_event_getTalkXYBtn() - 1) <= 3; }                          /* [v npc_*] */
inline u8 dComIfGp_event_getPreItemNo() { return gabi::load<u8>(dComIfGp_ea() + PLAY_OFF(mEvtCtrl.mItemNo)); }            /* [v npc_*] */
inline void dComIfGp_event_setItemPartnerId(fpc_ProcID id) { gabi::store<u32>(dComIfGp_ea() + PLAY_OFF(mEvtCtrl.mPtItem), id); } /* [v npc_*] */
/* GameCube: the masked flag word [v waterlevel] */
inline u16 dComIfGp_event_chkEventFlag(u16 flag) { return ((dComIfG_inf_c*)dComIfGp_get())->play.mEvtCtrl.mEventFlag & flag; }
inline void dComIfGp_event_onEventFlag(u16 flag) { u32 a = dComIfGp_ea() + PLAY_OFF(mEvtCtrl.mEventFlag); gabi::store<u16>(a, (u16)(gabi::load<u16>(a) | flag)); } /* [v mn] */
/* 0253EE04 dEvt_control_c::convPId, 0253F124 dEvt_control_c::getPId [v ship, npc_people, mknjd, uk, kmon] */
inline fopAc_ac_c* dComIfGp_event_getPt1() { u32 p = dComIfGp_ea(); return gabi::call<fopAc_ac_c*>(0x0253EE04, p + PLAY_EVTCTRL, gabi::load<u32>(p + PLAY_OFF(mEvtCtrl.mPt1))); }
inline fopAc_ac_c* dComIfGp_event_getPt2() { u32 p = dComIfGp_ea(); return gabi::call<fopAc_ac_c*>(0x0253EE04, p + PLAY_EVTCTRL, gabi::load<u32>(p + PLAY_OFF(mEvtCtrl.mPt2))); }
inline fopAc_ac_c* dComIfGp_event_getTalkPartner() { u32 p = dComIfGp_ea(); return gabi::call<fopAc_ac_c*>(0x0253EE04, p + PLAY_EVTCTRL, gabi::load<u32>(p + PLAY_OFF(mEvtCtrl.mPtTalk))); }
inline void dComIfGp_event_setTalkPartner(fopAc_ac_c* a) { u32 evt = dComIfGp_ea() + PLAY_EVTCTRL; gabi::store<u32>(evt + 0xCC, gabi::call<u32>(0x0253F124, evt, a)); }
inline void dComIfGp_event_setItemPartner(fopAc_ac_c* a) { u32 evt = dComIfGp_ea() + PLAY_EVTCTRL; gabi::store<u32>(evt + 0xD0, gabi::call<u32>(0x0253F124, evt, a)); }
/* 0253EB70 dEvt_control_c::giveItemCut [v ho] */
inline BOOL dComIfGp_event_giveItemCut(u8 item) { return gabi::call<BOOL>(0x0253EB70, dComIfGp_getEvent(), item); }

/* ---- event manager (dEvent_manager_c) ---- */
inline BOOL dComIfGp_evmng_ChkPresentEnd() { return gabi::call<BOOL>(0x02544950, dComIfGp_getPEvtManager()); }           /* [v npc_*] */
inline void dComIfGp_evmng_CancelPresent() { gabi::call(0x02544980, dComIfGp_getPEvtManager()); }                         /* [v gp1, kp1, ho] */
inline void dComIfGp_evmng_setGoal(cXyz* pos) { gabi::call(0x02543714, dComIfGp_getPEvtManager(), pos); }                /* [v npc_*] */
inline cXyz* dComIfGp_evmng_getGoal() { return gabi::call<cXyz*>(0x02544900, dComIfGp_getPEvtManager()); }                /* [v ship] */
inline const char* dComIfGp_evmng_getMyActName(s32 staffId) { return gabi::call<const char*>(0x025447EC, dComIfGp_getPEvtManager(), staffId); } /* [v md_event] */
inline s32 dComIfGp_evmng_getMySubstanceNum(s32 staffId, const char* name) { return gabi::call<s32>(0x025448CC, dComIfGp_getPEvtManager(), staffId, name); } /* [v hr] */
/* getMySubstanceP(staffIdx, name, type): 0 float, 1 vec3d/xyz, 3 integer, 4 string [v npc_*] */
inline be<f32>* dComIfGp_evmng_getMyFloatP(s32 staffId, const char* name) { return (be<f32>*)dComIfGp_evmng_getMySubstanceP(staffId, name, 0); }
inline cXyz* dComIfGp_evmng_getMyVec3dP(s32 staffId, const char* name) { return (cXyz*)dComIfGp_evmng_getMySubstanceP(staffId, name, 1); }
inline cXyz* dComIfGp_evmng_getMyXyzP(s32 staffId, const char* name) { return (cXyz*)dComIfGp_evmng_getMySubstanceP(staffId, name, 1); }
inline char* dComIfGp_evmng_getMyStringP(s32 staffId, const char* name) { return (char*)dComIfGp_evmng_getMySubstanceP(staffId, name, 4); }

/* ---- attention ---- */
/* dAttention_c flags word (+0x20) [v kamome_exec, mknjd, npc_ji1_exec] */
inline BOOL dComIfGp_att_chkEnemySound() { return (gabi::load<u32>(dComIfGp_ea() + PLAY_ATTENTION + 0x20) >> 8) & 1; }
inline void dComIfGp_att_offAleart() { u32 a = dComIfGp_ea() + PLAY_ATTENTION + 0x20; gabi::store<u32>(a, gabi::load<u32>(a) | 0x80000000u); }
inline void dComIfGp_att_revivalAleart() { u32 a = dComIfGp_ea() + PLAY_ATTENTION + 0x20; gabi::store<u32>(a, gabi::load<u32>(a) & 0x7FFFFFFFu); }

/* ---- particles ---- */
inline JPABaseEmitter* dComIfGp_particle_setP1(u16 id, const cXyz* pos, const csXyz* angle = nullptr, const cXyz* scale = nullptr,
                                               u8 alpha = 0xFF, dPa_levelEcallBack* cb = nullptr, s8 setup = -1,
                                               const GXColor* prm = nullptr, const GXColor* env = nullptr, const cXyz* scale2D = nullptr) {
    dPa_control_c* pa = dComIfGp_getParticle();
    return pa->set(1, id, pos, angle, scale, alpha, cb, setup, prm, env, scale2D); /* [v arrow_lighteff] */
}
inline JPABaseEmitter* dComIfGp_particle_setShipTail(u16 id, const cXyz* pos, const csXyz* angle = nullptr, const cXyz* scale = nullptr,
                                                     u8 alpha = 0xFF, dPa_levelEcallBack* cb = nullptr) {
    dPa_control_c* pa = dComIfGp_getParticle();
    return pa->set(5, id, pos, angle, scale, alpha, cb, -1, nullptr, nullptr, nullptr); /* [v kanban] */
}
/* 025A8BFC dPa_control_c::setSimpleLand(cBgS_PolyInfo&, pos, angle, size, scale, rate, tevstr, int* attr, int type) [v npc_zl1_draw] */
inline JPABaseEmitter* dComIfGp_particle_setSimpleLand(cBgS_PolyInfo* poly, const cXyz* pos, const csXyz* angle, f32 size, f32 scale, f32 rate,
                                                       dKy_tevstr_c* tev, be<s32>* attr, int type) {
    return gabi::call<JPABaseEmitter*>(0x025A8BFC, dComIfGp_getParticle(), poly, pos, angle, size, scale, rate, tev, attr, type);
}
/* dPa_control_c's model-emitter list (+0x130) [v fallrock, tsubo_mode, eskban] */
inline void dComIfGp_particle_addModelEmitter(void* e) { u32 pa = gabi::ea(dComIfGp_getParticle()); gabi::call(0x0200FE78, gabi::load<u32>(pa + 0x130), e); }

/* ---- items and messages (play) ---- */
inline void dComIfGp_setItemLifeCount(f32 n) { u32 a = dComIfGp_ea() + PLAY_OFF(mItemLifeCount); gabi::store<f32>(a, gabi::fadds_ppc(gabi::load<f32>(a), n)); } /* [v d_item] */
inline void dComIfGp_setItemKeyNumCount(s16 n) { u32 a = dComIfGp_ea() + PLAY_OFF(mItemKeyNumCount); gabi::store<s16>(a, (s16)(gabi::load<s16>(a) + n)); }
inline void dComIfGp_setItemMaxLifeCount(s16 n) { u32 a = dComIfGp_ea() + PLAY_OFF(mItemMaxLifeCount); gabi::store<s16>(a, (s16)(gabi::load<s16>(a) + n)); }
inline void dComIfGp_setItemMagicCount(s16 n) { u32 a = dComIfGp_ea() + PLAY_OFF(mItemMagicCount); gabi::store<s16>(a, (s16)(gabi::load<s16>(a) + n)); } /* [v jbo, ji1] */
inline void dComIfGp_setItemMaxMagicCount(s16 n) { u32 a = dComIfGp_ea() + PLAY_OFF(mItemMaxMagicCount); gabi::store<s16>(a, (s16)(gabi::load<s16>(a) + n)); }
inline void dComIfGp_setItemArrowNumCount(s16 n) { u32 a = dComIfGp_ea() + PLAY_OFF(mItemArrowNumCount); gabi::store<s16>(a, (s16)(gabi::load<s16>(a) + n)); }
inline void dComIfGp_setItemBombNumCount(s16 n) { u32 a = dComIfGp_ea() + PLAY_OFF(mItemBombNumCount); gabi::store<s16>(a, (s16)(gabi::load<s16>(a) + n)); }
inline void dComIfGp_setItemBeastNumCount(int i, s16 n) { u32 a = dComIfGp_ea() + PLAY_OFF(mItemBeastNumCounts) + 2 * i; gabi::store<s16>(a, (s16)(gabi::load<s16>(a) + n)); }
inline void dComIfGp_setMessageCountNumber(s16 n) { gabi::store<s16>(dComIfGp_ea() + PLAY_OFF(mMsgCountNumber), n); }  /* [v ji1, bmcon1] */
inline void dComIfGp_resetItemTimer(s16 t) { u32 p = dComIfGp_ea(); gabi::store<s16>(p + PLAY_OFF(mItemTimer), t); gabi::store<u8>(p + PLAY_OFF(mStartItemTimer), 0); } /* [v npc_people] */
inline u8 dComIfGp_getScopeMesgStatus() { return gabi::load<u8>(dComIfGp_ea() + PLAY_OFF(mScopeMesgStatus)); }       /* [v mn, npc_people] */
inline void dComIfGp_setScopeMesgStatus(u8 v) { gabi::store<u8>(dComIfGp_ea() + PLAY_OFF(mScopeMesgStatus), v); }
inline u8 dComIfGp_getDoStatus() { return gabi::load<u8>(dComIfGp_ea() + PLAY_OFF(mDoStatus)); }                      /* [v ji1] */
inline void dComIfGp_setDoStatus(u8 v) { gabi::store<u8>(dComIfGp_ea() + PLAY_OFF(mDoStatus), v); }                   /* [i] */
inline u8 dComIfGp_getRStatus() { return gabi::load<u8>(dComIfGp_ea() + PLAY_OFF(mRStatus)); }                        /* [i] */
inline void dComIfGp_setRStatus(u8 v) { gabi::store<u8>(dComIfGp_ea() + PLAY_OFF(mRStatus), v); }                     /* [i] */
inline u8 dComIfGp_getAStatus() { return gabi::load<u8>(dComIfGp_ea() + PLAY_OFF(mAStatus)); }                        /* [i] */
inline void dComIfGp_setAStatus(u8 v) { gabi::store<u8>(dComIfGp_ea() + PLAY_OFF(mAStatus), v); }                     /* [i] */
inline void dComIfGp_setDoStatusForce(u8 v) { gabi::store<u8>(dComIfGp_ea() + PLAY_OFF(mDoStatusForce), v); }         /* [i] */
inline void dComIfGp_setAStatusForce(u8 v) { gabi::store<u8>(dComIfGp_ea() + PLAY_OFF(mAStatusForce), v); }           /* [i] */
inline void dComIfGp_setRStatusForce(u8 v) { gabi::store<u8>(dComIfGp_ea() + PLAY_OFF(mRStatusForce), v); }           /* [i] */
inline u8 dComIfGp_getMesgAnimeAttrInfo() { return gabi::load<u8>(dComIfGp_ea() + PLAY_OFF(mMesgAnime)); }            /* [v npc_*] */
inline void dComIfGp_setMesgAnimeAttrInfo(u8 v) { gabi::store<u8>(dComIfGp_ea() + PLAY_OFF(mMesgAnime), v); }        /* [v mn] */
inline void dComIfGp_clearMesgAnimeAttrInfo() { gabi::store<u8>(dComIfGp_ea() + PLAY_OFF(mMesgAnime), 0xFF); }        /* [v roten, ah, sv] */
inline u8 dComIfGp_getMesgAnimeTagInfo() { return gabi::load<u8>(dComIfGp_ea() + PLAY_OFF(mMesgAnimeTagInfo)); }      /* [v ko1] */
inline void dComIfGp_clearMesgAnimeTagInfo() { gabi::store<u8>(dComIfGp_ea() + PLAY_OFF(mMesgAnimeTagInfo), 0xFF); }  /* [v ko1] */
inline BOOL dComIfGp_checkMesgCancelButton() { return gabi::load<u8>(dComIfGp_ea() + PLAY_OFF(mMesgCancelButton)); }  /* [v hr] */
inline void dComIfGp_setMelodyNum(u8 n) { gabi::store<u8>(dComIfGp_ea() + PLAY_OFF(mMelodyNum), n); }                 /* [v hr, tt] */
inline u8 dComIfGp_getPictureResult() { return gabi::load<u8>(dComIfGp_ea() + PLAY_OFF(mPictureResult)); }            /* [v npc_photo, npc_people] */
inline u8 dComIfGp_getPictureResultDetail() { return gabi::load<u8>(dComIfGp_ea() + PLAY_OFF(mPictureResultDetail)); } /* [v npc_people] */

/* ---- mini games, timer ---- */
inline u8 dComIfGp_getMiniGameType() { return gabi::load<u8>(dComIfGp_ea() + PLAY_OFF(mMiniGameType)); }              /* [v ji1, kmon] */
inline void dComIfGp_startMiniGame(u8 type, u16 bit) {
    u32 p = dComIfGp_ea();
    gabi::store<u8>(p + PLAY_OFF(mMiniGameType), type);
    gabi::store<u16>(p + PLAY_OFF(mMinigameFlags), (u16)(gabi::load<u16>(p + PLAY_OFF(mMinigameFlags)) | bit)); /* [v ji1_event] */
}
inline void dComIfGp_endMiniGame(u16 bit) {
    u32 p = dComIfGp_ea();
    u16 f = gabi::load<u16>(p + PLAY_OFF(mMinigameFlags));
    gabi::store<u8>(p + PLAY_OFF(mMinigameResult), 0);
    gabi::store<u8>(p + PLAY_OFF(mMiniGameType), 0);
    gabi::store<u16>(p + PLAY_OFF(mMinigameFlags), (u16)(f ^ bit)); /* [v ji1_event, ji1_exec] */
}
inline void dComIfGp_setMiniGameRupee(s16 n) { gabi::store<s16>(dComIfGp_ea() + PLAY_OFF(mMiniGameRupee), n); }       /* [v ji1_battle] */
inline void dComIfGp_plusMiniGameRupee(s16 n) {
    u32 a = dComIfGp_ea() + PLAY_OFF(mMiniGameRupee);
    s16 v = gabi::load<s16>(a);
    gabi::store<s16>(a, v + n > 0 ? (s16)(v + n) : (s16)0); /* [v ji1_teachsp] */
}
inline void* dComIfG_getTimerPtr() { return gabi::at<void>(gabi::load<u32>(dComIfGp_ea() + PLAY_OFF(mTimerInfo.mTimerPtr))); } /* [v bmsw, ji1_teachsp] */
inline s32 dComIfG_getTimerRestTimeMs() {
    s32 limit = gabi::load<s32>(dComIfGp_ea() + PLAY_OFF(mTimerInfo.mTimerLimitTimeMs));
    s32 now = gabi::load<s32>(dComIfGp_ea() + PLAY_OFF(mTimerInfo.mTimerNowTimeMs));
    return (s32)((u32)(limit - now) * 1000u) / 30; /* wrapping product, as mulli [v bmsw] */
}

/* ---- draw lists (HD: j3dSys opa/xlu buffers <- the draw list's buffers) ---- */
inline void dComIfGd_setListP1() {
    gabi::store<u32>(0x104B4634, gabi::load<u32>(dComIfGp_ea() + 0x5D58));
    gabi::store<u32>(0x104B4638, gabi::load<u32>(dComIfGp_ea() + 0x5D60)); /* [v ship] */
}
/* dComIfGd_setListSky / dComIfGd_getOpaListSky: bindings.h (raw d_a_bgn units use them too) */
inline Mtx34* dComIfGd_getInvViewMtx() { return gabi::at<Mtx34>(gabi::load<u32>(dComIfGp_ea() + 0x5FA4) + 0x174); }        /* [v demo_item] */

/* ---- save info (dSv_info_c) ---- */
inline u16 dComIfGs_getMaxLife() { return gabi::load<u16>(dComIfGs_ea() + SAVE_OFF(mSavedata.mPlayer.mPlayerStatusA.mMaxLife)); }
inline u16 dComIfGs_getLife() { return gabi::load<u16>(dComIfGs_ea() + SAVE_OFF(mSavedata.mPlayer.mPlayerStatusA.mLife)); }    /* [v fm, bita, ki] */
inline u8 dComIfGs_getMaxMagic() { return gabi::load<u8>(dComIfGs_ea() + SAVE_OFF(mSavedata.mPlayer.mPlayerStatusA.mMaxMagic)); } /* [v ki, ji1] */
inline u8 dComIfGs_getMagic() { return gabi::load<u8>(dComIfGs_ea() + SAVE_OFF(mSavedata.mPlayer.mPlayerStatusA.mMagic)); }  /* [v ki, cc, arrow] */
inline u8 dComIfGs_getSelectEquip(int i) { return gabi::load<u8>(dComIfGs_ea() + SAVE_OFF(mSavedata.mPlayer.mPlayerStatusA.mSelectEquip) + i); }
inline u8 dComIfGs_getWalletSize() { return gabi::load<u8>(dComIfGs_ea() + SAVE_OFF(mSavedata.mPlayer.mPlayerStatusA.mWalletSize)); }
inline void dComIfGs_setItem(int slot, u8 item) { gabi::store<u8>(dComIfGs_ea() + SAVE_OFF(mSavedata.mPlayer.mPlayerItem) + slot, item); } /* [v d_item] */
inline u8 dComIfGs_getBeastNum(int i) { return gabi::load<u8>(dComIfGs_ea() + SAVE_OFF(mSavedata.mPlayer.mBagItemRecord.mBeastNum) + i); } /* [v ds1] */
inline u8 dComIfGs_getClearCount() { return gabi::load<u8>(dComIfGs_ea() + SAVE_OFF(mSavedata.mPlayer.mInfo.mClearCount)); } /* [v tc_msg] */
inline u8 dComIfGs_checkCollect(int i) { return gabi::load<u8>(dComIfGs_ea() + SAVE_OFF(mSavedata.mPlayer.mCollect) + i); } /* [v npc_*] */
/* dSv_player_item_c (mPlayerItem) methods [v ba1, ds1_talk, photo_msg] */
inline BOOL dComIfGs_checkBottle(u8 item) { return gabi::call<BOOL>(0x025B5C80, dComIfGs_ea() + SAVE_OFF(mSavedata.mPlayer.mPlayerItem), item); }
inline void dComIfGs_setBottleItemIn(u8 a, u8 b) { gabi::call(0x025B51DC, dComIfGs_ea() + SAVE_OFF(mSavedata.mPlayer.mPlayerItem), a, b); }
inline void dComIfGs_setEmptyBottleItemIn(u8 a) { gabi::call(0x025B5448, dComIfGs_ea() + SAVE_OFF(mSavedata.mPlayer.mPlayerItem), a); }
inline void dComIfGs_setEquipBottleItemEmpty() { gabi::call(0x025B5C4C, dComIfGs_ea() + SAVE_OFF(mSavedata.mPlayer.mPlayerItem)); }
/* dSv_player_get_item_c (mGetItem) [v ba1, ship] */
inline BOOL dComIfGs_isGetBottleItem(u8 item) { return gabi::call<BOOL>(0x025B5F44, dComIfGs_ea() + SAVE_OFF(mSavedata.mPlayer.mGetItem), item); }
inline BOOL dComIfGs_isGetItem(int i, int j) { return gabi::call<BOOL>(0x025B5D40, dComIfGs_ea() + SAVE_OFF(mSavedata.mPlayer.mGetItem), i, j); }
/* dSv_player_bag_item_c (mBagItem) [v gp1, kp1, roten, co1_b] */
inline BOOL dComIfGs_checkReserveItem(u8 item) { return gabi::call<BOOL>(0x025B7570, dComIfGs_ea() + SAVE_OFF(mSavedata.mPlayer.mBagItem), item); }
inline void dComIfGs_setReserveItemEmpty() { gabi::call(0x025B7270, dComIfGs_ea() + SAVE_OFF(mSavedata.mPlayer.mBagItem)); }
inline void dComIfGs_setReserveItemChange(u8 btn, u8 item) { gabi::call(0x025B7278, dComIfGs_ea() + SAVE_OFF(mSavedata.mPlayer.mBagItem), btn, item); }
/* dSv_player_get_bag_item_c (mGetBagItem) [v item, bm1, roten] */
inline BOOL dComIfGs_isGetItemBeast(u8 i) { return gabi::call<BOOL>(0x025B7690, dComIfGs_ea() + SAVE_OFF(mSavedata.mPlayer.mGetBagItem), i); }
inline void dComIfGs_onGetItemBeast(u8 i) { gabi::call(0x025B7628, dComIfGs_ea() + SAVE_OFF(mSavedata.mPlayer.mGetBagItem), i); }
inline BOOL dComIfGs_isGetItemBait(u8 i) { return gabi::call<BOOL>(0x025B7768, dComIfGs_ea() + SAVE_OFF(mSavedata.mPlayer.mGetBagItem), i); }
inline void dComIfGs_onGetItemBait(u8 i) { gabi::call(0x025B7700, dComIfGs_ea() + SAVE_OFF(mSavedata.mPlayer.mGetBagItem), i); }
inline BOOL dComIfGs_isGetItemReserve(u8 i) { return gabi::call<BOOL>(0x025B7840, dComIfGs_ea() + SAVE_OFF(mSavedata.mPlayer.mGetBagItem), i); }
inline void dComIfGs_onGetItemReserve(u8 i) { gabi::call(0x025B77D8, dComIfGs_ea() + SAVE_OFF(mSavedata.mPlayer.mGetBagItem), i); }
/* dSv_player_collect_c (mCollect) [v mo2, zouK, kf1, ship, npc_*] */
inline BOOL dComIfGs_isCollect(int i, u8 item) { return gabi::call<BOOL>(0x025B7A2C, dComIfGs_ea() + SAVE_OFF(mSavedata.mPlayer.mCollect), i, item); }
inline void dComIfGs_onCollect(int i, u8 item) { gabi::call(0x025B7944, dComIfGs_ea() + SAVE_OFF(mSavedata.mPlayer.mCollect), i, item); }
inline void dComIfGs_offCollect(int i, u8 item) { gabi::call(0x025B79B8, dComIfGs_ea() + SAVE_OFF(mSavedata.mPlayer.mCollect), i, item); }
inline BOOL dComIfGs_isTact(u8 i) { return gabi::call<BOOL>(0x025B7B10, dComIfGs_ea() + SAVE_OFF(mSavedata.mPlayer.mCollect), i); }
inline BOOL dComIfGs_isSymbol(u8 i) { return gabi::call<BOOL>(0x025B7D90, dComIfGs_ea() + SAVE_OFF(mSavedata.mPlayer.mCollect), i); }
inline s32 dComIfGs_getTriforceNum() { return gabi::call<s32>(0x025B7E00, dComIfGs_ea() + SAVE_OFF(mSavedata.mPlayer.mCollect)); }
/* dSv_player_map_c (mMap); HD passes the index - 1 [v tc_cut, tc_msg] */
inline BOOL dComIfGs_isGetCollectMap(int i) { return gabi::call<BOOL>(0x025B7F68, dComIfGs_ea() + SAVE_OFF(mSavedata.mPlayer.mMap), i - 1); }
inline BOOL dComIfGs_isCollectMapTriforce(int i) { return gabi::call<BOOL>(0x025B8308, dComIfGs_ea() + SAVE_OFF(mSavedata.mPlayer.mMap), i - 1); }
inline void dComIfGs_onCollectMapTriforce(int i) { gabi::call(0x025B82A0, dComIfGs_ea() + SAVE_OFF(mSavedata.mPlayer.mMap), i - 1); }
/* events: off (025B8B7C dSv_event_c::offEventBit), tmp registers [v people_msg, bmcon1, ko1, mk] */
inline void dComIfGs_offEventBit(u16 id) { gabi::call(0x025B8B7C, dComIfGs_getEvent(), id); }
inline void dComIfGs_offTmpBit(u16 id) { gabi::call(0x025B8B7C, dComIfGs_getTmpEvent(), id); }
inline u8 dComIfGs_getTmpReg(u16 reg) { return dComIfGs_getTmpEvent()->getEventReg(reg); }
inline void dComIfGs_setTmpReg(u16 reg, u8 v) { dComIfGs_getTmpEvent()->setEventReg(reg, v); }
/* switches: 025BA20C dSv_info_c::revSwitch [v swpropeller] */
inline void dComIfGs_revSwitch(int no, int roomNo) { gabi::call(0x025BA20C, dComIfGs_info(), no, roomNo); }
/* stage memory (dSv_info_c::mMemory.mMembit, 025B9100 isDungeonItem) [v bmdfoot, bmdhand, warpf] */
inline BOOL dComIfGs_isDungeonItem(int i) { return gabi::call<BOOL>(0x025B9100, dComIfGs_ea() + SAVE_OFF(mMemory), i); }
inline BOOL dComIfGs_isStageBossDemo() { return dComIfGs_isDungeonItem(dSv_memBit_c::STAGE_BOSS_DEMO); }
/* HD out of line: 02520A84 dComIfGs_isStageBossEnemy(int stageNo), 02520B88 dComIfGs_isStageLife(int stageNo) [v oq, so, boss_item] */
inline BOOL dComIfGs_isStageBossEnemy(int stageNo) { return gabi::call<BOOL>(0x02520A84, stageNo); }
inline BOOL dComIfGs_isStageBossEnemy() { return dComIfGs_isDungeonItem(dSv_memBit_c::STAGE_BOSS_ENEMY); } /* [v warpf] */
inline BOOL dComIfGs_isStageLife(int stageNo) { return gabi::call<BOOL>(0x02520B88, stageNo); }

/* Not mapped yet (no verified HD form): the remaining tww accessors, e.g.
 * dComIfGp_checkStageName (SafeString compare), dComIfGp_demo_* (dDemo_manager_c: HD has no
 * play pointer), dComIfGp_map_*, dComIfGp_getMesgStatus/setMesgStatus (message manager), most
 * dComIfGd_* (shadows, alpha models: the draw list layout is not measured), dComIfGs_*Name/Date/
 * Config/Restart/TurnRestart/Zone accessors (layouts in d/d_save.h; the methods' HD addresses are
 * matched in build/names.tsv but not verified). */
/* HD letter callers pass the full index register and compare the full return register.
 * Keep these separate from the GameCube u16/u8 API, which deliberately narrows them.
 * 025B8BB0 dSv_event_c::getEventReg; 025B8AF4 dSv_event_c::setEventReg [v d_letter] */
inline u32 dComIfGs_getEventRegHD(u32 reg) {
    return gabi::call<u32>(0x025B8BB0, dComIfGs_getEvent(), reg);
}
inline void dComIfGs_setEventRegHD(u32 reg, u32 value) {
    gabi::call(0x025B8AF4, dComIfGs_getEvent(), reg, value);
}
