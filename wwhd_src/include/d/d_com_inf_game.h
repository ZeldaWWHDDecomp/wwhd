/* d_com_inf_game: the global game info, WWHD. 
 *
 * HD: g_dComIfG_gameInfo.play is a function-local static (0x1046F0B0) behind the accessor
 * dComIfGp_get() (025200D4, called before every access); the save info is reached through the
 * pointer at 0x101F84DC (+0x20).
 *
 * dComIfG_play_c member offsets (GameCube -> WWHD). The shift is not constant: +0x12A0 up to
 * mStageData, +0x1298 from mRoomCtrl, +0x129C from mAttention, +0x1288/+0x128C around the
 * player pointers. Measured from calls whose `this` is play+OFF (tools/verify README:
 * "playoffs"), [v] = used by a verified function. */
#pragma once
#include "SSystem/SComponent/c_xyz.h"
#include "f_op/f_op_actor.h"
#include "wwhd.h"

inline u8* dComIfGp_get() { return gabi::call<u8*>(0x025200D4); }
inline u32 dComIfGp_ea() { return gabi::ea(dComIfGp_get()); }

enum : u32 {
    PLAY_BGS = 0x12A0,          /* dBgS mBgS [v] */
    PLAY_CCS = 0x26A4,          /* dCcS mCcS (cCcS::Set this) [v kamome] */
    PLAY_CCMASS = 0x4EF8,       /* dCcMassS_Mng (in dCcS) */
    PLAY_NAMETBL = 0x50AC,      /* cDT_NamePTbl (cDT::GetIndex this) */
    PLAY_STAGEDATA = 0x5150,    /* dStage_stageDt_c [v kamome] */
    PLAY_ROOMCTRL = 0x51CC,     /* dStage_roomControl_c */
    PLAY_EVTCTRL = 0x51D0,      /* dEvt_control_c (GameCube 0x3F38) */
    PLAY_EVTMANAGER = 0x52C4,   /* dEvent_manager_c (GameCube 0x402C) */
    PLAY_ATTENTION = 0x5804,    /* dAttention_c (GameCube 0x4568) */
    PLAY_VIBRATION = 0x599C,    /* dVibration_c (GameCube 0x4700) */
    PLAY_DETECT = 0x5A20,       /* dDetect_c (GameCube 0x4784) */
    PLAY_PARTICLE = 0x5AB0,     /* dPa_control_c* mParticle (GameCube 0x4824) -- inferred, confirm */
    PLAY_PLAYER = 0x5B2C,       /* fopAc_ac_c* mpPlayer[0] (GameCube 0x48A4) [v kamome] */
    PLAY_PLAYERPTR = 0x5B34,    /* fopAc_ac_c* mpPlayerPtr[3] (GameCube 0x48AC) -- inferred */
    PLAY_PLAYER_STATUS0 = 0x5CD8, /* daPy_py_c status word 0 [v kamome] */
    PLAY_DLST = 0x5D30,         /* dDlst_list_c */
};

inline dBgS* dComIfG_Bgsp() { return gabi::at<dBgS>(dComIfGp_ea() + PLAY_BGS); }
inline cCcS* dComIfG_Ccsp() { return gabi::at<cCcS>(dComIfGp_ea() + PLAY_CCS); }
inline dEvent_manager_c* dComIfGp_getPEvtManager() { return gabi::at<dEvent_manager_c>(dComIfGp_ea() + PLAY_EVTMANAGER); }
inline dEvt_control_c* dComIfGp_getEvent() { return gabi::at<dEvt_control_c>(dComIfGp_ea() + PLAY_EVTCTRL); }
inline dAttention_c* dComIfGp_getAttention() { return gabi::at<dAttention_c>(dComIfGp_ea() + PLAY_ATTENTION); }
inline dVibration_c* dComIfGp_getVibration() { return gabi::at<dVibration_c>(dComIfGp_ea() + PLAY_VIBRATION); }
inline dPa_control_c* dComIfGp_getParticle() { return gabi::at<dPa_control_c>(gabi::load<u32>(dComIfGp_ea() + PLAY_PARTICLE)); }
inline fopAc_ac_c* dComIfGp_getPlayer(int i) { return gabi::at<fopAc_ac_c>(gabi::load<u32>(dComIfGp_ea() + PLAY_PLAYER + 8 * i)); }
inline fopAc_ac_c* dComIfGp_getLinkPlayer() { return gabi::at<fopAc_ac_c>(gabi::load<u32>(dComIfGp_ea() + PLAY_PLAYERPTR)); }

/* 0200E240 cCcS::Set(cCcD_Obj*) [v kamome] */
inline void cCcS_Set(cCcS* ccs, void* obj) { gabi::call(0x0200E240, ccs, obj); }
inline void dComIfG_Ccsp_Set(void* obj) { cCcS_Set(dComIfG_Ccsp(), obj); }

/* ---- save info (HD: through the pointer at 0x101F84DC) ---- */
inline dSv_info_c* dComIfGs_info() { return gabi::at<dSv_info_c>(gabi::load<u32>(0x101F84DC) + 0x20); }
/* 025BA0C0 dSv_info_c::isSwitch [v mtoge] */
inline BOOL dComIfGs_isSwitch(s32 no, s32 roomNo) { return gabi::call<BOOL>(0x025BA0C0, dComIfGs_info(), no, roomNo); }
/* 025B9E38 dSv_info_c::onSwitch [v swc00] */
inline void dComIfGs_onSwitch(s32 no, s32 roomNo) { gabi::call(0x025B9E38, dComIfGs_info(), no, roomNo); }
/* 025B9F7C dSv_info_c::offSwitch [v swc00] */
inline void dComIfGs_offSwitch(s32 no, s32 roomNo) { gabi::call(0x025B9F7C, dComIfGs_info(), no, roomNo); }
/* 025BA5D4 dSv_info_c::onActor [g] */
inline void dComIfGs_onActor(s32 no, s32 roomNo) { gabi::call(0x025BA5D4, dComIfGs_info(), no, roomNo); }

/* ---- resources ---- */
inline cPhs_State dComIfG_resLoad(request_of_phase_process_class* p, const char* arc) { return gabi::call<cPhs_State>(0x02520460, p, arc); }
inline BOOL dComIfG_resDelete(request_of_phase_process_class* p, const char* arc) { return gabi::call<BOOL>(0x025204C8, p, arc); }
/* HD: resource names are passed as sead::SafeString temporaries {const char* mStringTop; vtable}
 * (GHS places the vtable pointer after the members). Each translation unit has its own copy of
 * the SafeString vtable, so the caller passes its TU's vtable address. */
struct SafeString {
    be<u32> mStringTop;
    be<u32> __vtbl;
};
inline dRes_control_c* dComIfG_resControl() { return gabi::at<dRes_control_c>(gabi::load<u32>(0x101F4F28)); }
/* 026066C4 HD: dRes_control_c::getRes(const sead::SafeString& arc, s32 index) [v] */
inline void* dComIfG_getObjectRes(const char* arc, s32 index, u32 safestring_vtbl) {
    gabi::Local<SafeString> key;
    key->mStringTop = gabi::ea(arc);
    key->__vtbl = safestring_vtbl;
    return gabi::call<void*>(0x026066C4, dComIfG_resControl(), key.get(), index);
}

/* ---- events (dEvent_manager_c at play+0x52C4) [g] ---- */
inline s32 dComIfGp_evmng_getMyStaffId(const char* name, fopAc_ac_c* actor, s32 tag) {
    return gabi::call<s32>(0x02542D88, dComIfGp_getPEvtManager(), name, actor, tag);
}
inline void dComIfGp_evmng_cutEnd(s32 staffId) { gabi::call(0x02543280, dComIfGp_getPEvtManager(), staffId); }
inline BOOL dComIfGp_evmng_endCheck(s16 eventIdx) { return gabi::call<BOOL>(0x025440C8, dComIfGp_getPEvtManager(), eventIdx); }
inline BOOL dComIfGp_evmng_startCheck(s16 eventIdx) { return gabi::call<BOOL>(0x0254407C, dComIfGp_getPEvtManager(), eventIdx); }
inline s16 dComIfGp_evmng_getEventIdx(const char* name, u8 evNo) {
    return gabi::call<s16>(0x02543F10, dComIfGp_getPEvtManager(), name, evNo);
}
inline BOOL dComIfGp_evmng_getIsAddvance(s32 staffId) { return gabi::call<BOOL>(0x025447C8, dComIfGp_getPEvtManager(), staffId); }
inline void* dComIfGp_evmng_getMySubstanceP(s32 staffId, const char* name, s32 type) {
    return gabi::call<void*>(0x0254487C, dComIfGp_getPEvtManager(), staffId, name, type);
}

/* 02520540 dComIfGp_getReverb(roomNo) [v] */
inline s32 dComIfGp_getReverb(s32 roomNo) { return gabi::call<s32>(0x02520540, roomNo); }
