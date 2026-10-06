/* Bindings: WWHD functions called by the decompiled source, by address.
 *
 * Each binding states the WWHD address and the argument/return types as the verified callers
 * use them. Names follow the GameCube decompilation where tools/decomp matched the function
 * (evidence in build/names.tsv); "HD:" marks a signature or behaviour that differs from
 * GameCube. Accessors to HD singletons replace GameCube globals. */
#pragma once
#include "f_op/f_op_actor.h"
#include "wwhd.h"
#include "d/d_com_inf_game.h"
#include "d/d_bg_s.h"
#include "d/d_cc_d.h"
#include "m_Do/m_Do_ext.h"
#include "d/d_npc.h"


/* ---- imports (Cafe OS) ---- */
inline void* memcpy_g(void* dst, const void* src, u32 n) { return gabi::call<void*>(0xC000A848, dst, src, n); }

/* ---- c_lib ---- */
inline BOOL cLib_chaseF(be<f32>* value, f32 target, f32 step) { return gabi::call<BOOL>(0x0200F5C8, value, target, step); }

/* ---- matrices ---- */
inline void PSMTXTrans(Mtx34* m, f32 x, f32 y, f32 z) { gabi::call(0x028E93CC, m, x, y, z); }
inline void mDoMtx_YrotM(Mtx34* m, s16 y) { gabi::call(0x025F1C28, m, y); }
struct mDoMtx_stack_c {
    static Mtx34* get() { return gabi::at<Mtx34>(0x1048D0CC); } /* mDoMtx_stack_c::now */
    static void transS(f32 x, f32 y, f32 z) { PSMTXTrans(get(), x, y, z); }
    static void YrotM(s16 y) { mDoMtx_YrotM(get(), y); }
    static void transM(f32 x, f32 y, f32 z) { gabi::call(0x025F24E0, x, y, z); }
    static void scaleM(f32 x, f32 y, f32 z) { gabi::call(0x025F2518, x, y, z); }
};

/* ---- J3D / m_Do_ext ---- */
/* matrix assignment: GHS loads all twelve values, then stores them */
inline void mtx_copy(Mtx34* dst, const Mtx34* src) {
    f32 t[3][4];
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 4; j++) t[i][j] = src->m[i][j];
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 4; j++) dst->m[i][j] = t[i][j];
}
inline void J3DModel_setBaseTRMtx(J3DModel* m, const Mtx34* src) { mtx_copy(gabi::at<Mtx34>(gabi::ea(m) + 0xC8), src); }
inline J3DModel* mDoExt_J3DModel__create(J3DModelData* data, u32 modelFlag, u32 differedDlistFlag) {
    return gabi::call<J3DModel*>(0x025E38E0, data, modelFlag, differedDlistFlag);
}
/* HD: second argument (0 from actors) */
inline void mDoExt_modelUpdateDL(J3DModel* m, u32 hd_arg = 0) { gabi::call(0x025E2DE0, m, hd_arg); }

/* ---- environment light (HD: function-local static, 0x10475A68) ---- */
inline dScnKy_env_light_c* dKy_getEnvlight() { return gabi::call<dScnKy_env_light_c*>(0x02555D0C); }
inline void settingTevStruct(dScnKy_env_light_c* l, s32 type, cXyz* pos, dKy_tevstr_c* tev) {
    gabi::call(0x025626A4, l, type, pos, tev);
}
inline void setLightTevColorType(dScnKy_env_light_c* l, J3DModel* m, dKy_tevstr_c* tev) { gabi::call(0x02562F5C, l, m, tev); }
enum { TEV_TYPE_ACTOR = 0, TEV_TYPE_BG0 = 1 };

/* ---- collision ---- */
/* HD: GHS constructors allocate when called with this == NULL: `new dBgW()` is dBgW::dBgW(NULL) */
inline dBgW* new_dBgW() { return gabi::call<dBgW*>(0x024F23F4, (u32)0); }
inline bool cBgW_Set(dBgW* w, cBgD_t* data, u32 flags, Mtx34* mtx) { return gabi::call<bool>(0x0200A030, w, data, flags, mtx); }
enum { cBgW_MOVE_BG_e = 1 };
inline void dBgW_Move(dBgW* w) { gabi::call(0x024F43DC, w); }
inline BOOL dBgS_Regist(dBgS* s, dBgW* w, fopAc_ac_c* actor) { return gabi::call<BOOL>(0x024EEA6C, s, w, actor); }
inline BOOL cBgS_Release(dBgS* s, dBgW* w) { return gabi::call<BOOL>(0x020087EC, s, w); }

/* ---- d_com_inf_game: see d/d_com_inf_game.h ---- */

/* ---- JUT ---- */
inline void JUT_ASSERT_fail(const char* file, s32 line, const char* msg) { gabi::call(0x0273AA24, file, line, msg); }

/* ---- audio ---- */
inline void mDoAud_seStart(u32 id, cXyz* pos, u32 param, s32 reverb) { gabi::call(0x025E1A40, id, pos, param, reverb); }

/* ---- f_op_actor_mng ---- */
inline u32 fopAcM_GetParam(fopAc_ac_c* a) { return a->mParameters; }
inline s8 fopAcM_GetRoomNo(fopAc_ac_c* a) { return a->current.roomNo; }
inline void fopAcM_SetSpeedF(fopAc_ac_c* a, f32 v) { a->speedF = v; }
inline bool fopAcM_CheckCondition(fopAc_ac_c* a, u32 c) { return (a->actor_condition & c) != 0; }
inline void fopAcM_OnCondition(fopAc_ac_c* a, u32 c) { a->actor_condition |= c; }
/* HD: the inline checks the actor and its eyePos for NULL */
inline void fopAcM_seStart(fopAc_ac_c* a, u32 id, u32 param) {
    if (a != nullptr && gabi::ea(&a->eyePos) != 0)
        mDoAud_seStart(id, &a->eyePos, param, dComIfGp_getReverb(fopAcM_GetRoomNo(a)));
}
typedef u32 heapCallbackFunc; /* guest address of a BOOL (*)(fopAc_ac_c*) */
inline BOOL fopAcM_entrySolidHeap(fopAc_ac_c* a, heapCallbackFunc cb, u32 size) { return gabi::call<BOOL>(0x025D63E8, a, cb, size); }

/* ---- more bindings (d_a_kamome) ---- */
WWHD_OPAQUE(mDoExt_McaMorf_c);

/* process framework */
inline BOOL fopAc_IsActor(void* p) { return gabi::call<BOOL>(0x025D4604, p); }
inline s16 fpcM_GetName(void* p) { return gabi::load<s16>(gabi::ea(p) + 0x8); } /* base_process_class::mProcName */
typedef u32 fpcM_SearchFunc; /* guest address of void* (*)(void*, void*) */
inline void* fpcM_Search(fpcM_SearchFunc fn, void* data) { return gabi::call<void*>(0x025DE508, fn, data); }
inline u32 fopAcM_create(s16 name, u32 param, cXyz* pos, s32 roomNo, csXyz* angle, cXyz* scale, s8 subtype, u32 createFunc) {
    return gabi::call<u32>(0x025D5834, name, param, pos, roomNo, angle, scale, subtype, createFunc);
}
inline void fopAcM_setStageLayer(void* a) { gabi::call(0x025D537C, a); }
inline dPath* dPath_GetRoomPath(s32 idx, s32 roomNo) { return gabi::call<dPath*>(0x025AAF88, idx, roomNo); }

/* math */
inline f32 std_sqrtf(f32 x) { return gabi::call<f32>(0x028F4384, x); }
inline s16 cM_atan2s(f32 y, f32 x) { return gabi::call<s16>(0x020195B0, y, x); }
inline f32 cM_rndF(f32 max) { return gabi::call<f32>(0x020198D8, max); }
inline f32 cM_rndFX(f32 max) { return gabi::call<f32>(0x02019918, max); }
inline void cLib_addCalcAngleS2(be<s16>* value, s16 target, s16 scale, s16 maxStep) {
    gabi::call(0x0200F428, value, target, scale, maxStep);
}
inline void cLib_addCalc2(be<f32>* value, f32 target, f32 scale, f32 maxStep) { gabi::call(0x0200ED84, value, target, scale, maxStep); }
inline void mDoMtx_YrotS(Mtx34* m, s16 y) { gabi::call(0x025F1884, m, y); }
inline void mDoMtx_XrotM(Mtx34* m, s16 x) { gabi::call(0x025F1BF4, m, x); }
inline void mDoMtx_ZrotM(Mtx34* m, s16 z) { gabi::call(0x025F1C5C, m, z); }
inline void PSMTXCopy(const Mtx34* src, Mtx34* dst) { gabi::call(0x028E90D4, src, dst); }
/* c_math: calc_mtx is a pointer (0x1018C7B0) to the scratch matrix */
inline Mtx34* calc_mtx() { return gabi::at<Mtx34>(gabi::load<u32>(0x1018C7B0)); }
inline void MtxTrans(f32 x, f32 y, f32 z, u8 concat) { gabi::call(0x0200FAD8, x, y, z, concat); }
inline void MtxPosition(cXyz* src, cXyz* dst) { gabi::call(0x0200FCD8, src, dst); }
inline void cMtx_YrotS(Mtx34* m, s16 y) { mDoMtx_YrotS(m, y); }
inline void cMtx_YrotM(Mtx34* m, s16 y) { mDoMtx_YrotM(m, y); }
inline void cMtx_XrotM(Mtx34* m, s16 x) { mDoMtx_XrotM(m, x); }
inline void cMtx_ZrotM(Mtx34* m, s16 z) { mDoMtx_ZrotM(m, z); }

/* debug registers g_regHIO (REGn_F(i), REGn_S(i)): children of 0x90 bytes, floats from 0x1047B610,
 * shorts from 0x1047B688 */
inline f32 REG_F(int child, int i) { return gabi::load<f32>(0x1047B610 + 0x90 * child + 4 * i); }
inline s16 REG_S(int child, int i) { return gabi::load<s16>(0x1047B688 + 0x90 * child + 2 * i); }
#define REG0_F(i) REG_F(0, i)
#define REG0_S(i) REG_S(0, i)

/* collision */
inline void cBgS_LinChk_ct(void* chk) { gabi::call(0x02008FEC, chk); }
inline void cBgS_LinChk_dt(void* chk, s32 flags) { gabi::call(0x02008B4C, chk, flags); }
inline void dBgS_LinChk_Set(void* chk, cXyz* start, cXyz* end, fopAc_ac_c* actor) { gabi::call(0x024F1AFC, chk, start, end, actor); }
inline BOOL cBgS_LineCross(dBgS* bgs, void* chk) { return gabi::call<BOOL>(0x02008860, bgs, chk); }
inline void dBgS_Acch_CrrPos(void* acch, dBgS* bgs) { gabi::call(0x024F08A8, acch, bgs); }

/* J3D (HD) */
inline J3DJoint* J3DNode_toJoint(J3DNode* n) { return gabi::call<J3DJoint*>(0x027F7878, n); }

/* mDoExt_McaMorf (HD layout: model at +0x90) */
inline J3DModel* McaMorf_getModel(mDoExt_McaMorf_c* m) { return gabi::at<J3DModel>(gabi::load<u32>(gabi::ea(m) + 0x90)); }
inline void McaMorf_setAnm(mDoExt_McaMorf_c* m, J3DAnmTransform* anm, s32 loopMode, f32 morf, f32 speed, f32 start, f32 end, void* sound) {
    gabi::call(0x025E4A98, m, anm, loopMode, morf, speed, start, end, sound);
}
inline void McaMorf_entryDL(mDoExt_McaMorf_c* m) { gabi::call(0x025E5590, m); }
inline void McaMorf_calc(mDoExt_McaMorf_c* m) { gabi::call(0x025E55A0, m); }

/* snap, HIO */
inline void dSnap_RegistFig(s32 type, fopAc_ac_c* a, f32 x, f32 y, f32 z) { gabi::call(0x025BED80, type, a, x, y, z); }
inline s8 mDoHIO_createChild(const char* name, void* hio) { return gabi::call<s8>(0x025F0A10, name, hio); }
inline void mDoHIO_deleteChild(s8 no) { gabi::call(0x025F0A18, no); }

/* =====================================================================================
 * Common actor bindings (added for the actor rollout). [v] = used by a verified function,
 * [g] = GameCube signature, not yet exercised (the harness checks it on first use).
 * Result types: GameCube types; s16/u8/bool where the WWHD code returns that width.
 * ===================================================================================== */
WWHD_OPAQUE(cBgS_GndChk);
WWHD_OPAQUE(cBgS_LinChk);
WWHD_OPAQUE(dSv_event_c);

/* ---- operator new/delete (HD: GHS runtime) ---- */
inline void* operator_new(u32 size) { return gabi::call<void*>(0x0273AD10, size); }   /* [v] constructors */
inline void operator_delete(void* p) { gabi::call(0x0273AF40, p); }                  /* 0273AF40 __dl [g] */

/* ---- f_op_actor_mng ---- */
inline BOOL fopAcM_delete(fopAc_ac_c* a) { return gabi::call<BOOL>(0x025D57E0, a); }                 /* [v kamome] */
inline void fopAcM_posMove(fopAc_ac_c* a, const cXyz* spd) { gabi::call(0x025D6800, a, spd); }       /* [g] */
inline void fopAcM_posMoveF(fopAc_ac_c* a, const cXyz* spd) { gabi::call(0x025D6870, a, spd); }      /* [g] */
inline s16 fopAcM_searchActorAngleY(fopAc_ac_c* a, fopAc_ac_c* b) { return gabi::call<s16>(0x025D6894, a, b); } /* [v kamome] */
inline f32 fopAcM_searchActorDistance(fopAc_ac_c* a, fopAc_ac_c* b) { return gabi::call<f32>(0x025D68EC, a, b); } /* [v kamome] */
inline f32 fopAcM_searchActorDistance2(fopAc_ac_c* a, fopAc_ac_c* b) { return gabi::call<f32>(0x025D6924, a, b); } /* [g] */
inline f32 fopAcM_searchActorDistanceXZ(fopAc_ac_c* a, fopAc_ac_c* b) { return gabi::call<f32>(0x025D6958, a, b); } /* [g] */
inline f32 fopAcM_searchActorDistanceXZ2(fopAc_ac_c* a, fopAc_ac_c* b) { return gabi::call<f32>(0x025D69AC, a, b); } /* [g] */
inline void fopAcM_setCullSizeBox(fopAc_ac_c* a, f32 x0, f32 y0, f32 z0, f32 x1, f32 y1, f32 z1) {
    gabi::call(0x025D674C, a, x0, y0, z0, x1, y1, z1);
} /* [g] */
inline fopAc_ac_c* fopAcIt_Judge(u32 judgeFn, void* data) { return gabi::call<fopAc_ac_c*>(0x025D5218, judgeFn, data); } /* [v kamome] */
inline void fopAcM_createDisappear(fopAc_ac_c* a, cXyz* pos, u8 p2, u8 p3, u8 p4) { gabi::call(0x025D99E8, a, pos, p2, p3, p4); } /* [g] */
inline BOOL fopAcM_orderOtherEventId(fopAc_ac_c* a, s16 ev, u8 mapToolId, u16 p3, u16 prio, u16 flag) {
    return gabi::call<BOOL>(0x025D7A58, a, ev, mapToolId, p3, prio, flag);
} /* [g] */
inline BOOL fopAcM_orderSpeakEvent(fopAc_ac_c* a) { return gabi::call<BOOL>(0x025D76A8, a); }        /* [g] */
inline s32 fopAcM_otoCheck(fopAc_ac_c* a, f32 r) { return gabi::call<s32>(0x025D9DA0, a, r); }       /* [v lamp] */
inline BOOL fopAcM_getGroundAngle(fopAc_ac_c* a, csXyz* out) { return gabi::call<BOOL>(0x025D9A70, a, out); } /* [g] */
inline void fopAcM_cancelCarryNow(fopAc_ac_c* a) { gabi::call(0x025D9D24, a); }                     /* [g] */
inline u32 fopAcM_GetID(void* p) { return p ? gabi::load<u32>(gabi::ea(p) + 4) : 0xFFFFFFFFu; }      /* [v kamome] base_process_class::mBsPcId */
/* fopAcM_SearchByID: HD inline; the id is passed by address to fpcSch_JudgeByID [v kamome] */
inline fopAc_ac_c* fopAcM_SearchByID(u32 id) {
    gabi::Local<be<u32>> key;
    *key = id;
    if (id == 0xFFFFFFFFu) return nullptr;
    return fopAcIt_Judge(0x025E1234 /* fpcSch_JudgeByID */, key.get());
}
inline fopAc_ac_c* dComIfGp_getPlayer0() { return dComIfGp_getPlayer(0); }
/* fopAcM_searchPlayer*: inline, the player pointer is re-read through dComIfGp_get() at each use */
inline f32 fopAcM_searchPlayerDistanceXZ2(fopAc_ac_c* a) { return fopAcM_searchActorDistanceXZ2(a, dComIfGp_getPlayer(0)); } /* [v swc00] */
inline f32 fopAcM_searchPlayerDistanceY(fopAc_ac_c* a) { return dComIfGp_getPlayer(0)->current.pos.y - a->current.pos.y; } /* [v swc00] */
inline s16 fopAcM_searchPlayerAngleY(fopAc_ac_c* a) { return fopAcM_searchActorAngleY(a, dComIfGp_getPlayer(0)); }
inline f32 fopAcM_searchPlayerDistance(fopAc_ac_c* a) { return fopAcM_searchActorDistance(a, dComIfGp_getPlayer(0)); }
inline f32 fopAcM_searchPlayerDistanceXZ(fopAc_ac_c* a) { return fopAcM_searchActorDistanceXZ(a, dComIfGp_getPlayer(0)); }

/* ---- c_lib / c_math ---- */
inline void cLib_addCalc0(be<f32>* v, f32 scale, f32 maxStep) { gabi::call(0x0200EDC8, v, scale, maxStep); } /* [g] */
inline s16 cLib_targetAngleY(const cXyz* a, const cXyz* b) { return gabi::call<s16>(0x0200F93C, a, b); }       /* [g] */
/* returns abs((s16)(a-b)) as an int, 0..0x8000 [v kb] */
inline s32 cLib_distanceAngleS(s16 a, s16 b) { return gabi::call<s32>(0x0200FAAC, a, b); }
inline BOOL cLib_chaseAngleS(be<s16>* v, s16 target, s16 step) { return gabi::call<BOOL>(0x0200F8D0, v, target, step); } /* [g] */
inline s16 cLib_addCalcAngleS(be<s16>* v, s16 target, s16 scale, s16 maxStep, s16 minStep) {
    return gabi::call<s16>(0x0200F378, v, target, scale, maxStep, minStep);
} /* [g] */
inline f32 cM_rnd() { return gabi::call<f32>(0x02019788); }  /* [g] */
/* cM_ssin / cM_scos: sin/cos table (8 bytes per entry: sin, cos) at 0x104A44F8 [v kamome sin] */
inline f32 cM_ssin(s32 a) { return gabi::load<f32>(0x104A44F8 + (((s32)(a & 0xFFFF) >> 3) << 3)); }
inline f32 cM_scos(s32 a) { return gabi::load<f32>(0x104A44F8 + 4 + (((s32)(a & 0xFFFF) >> 3) << 3)); }  /* [g] */
/* cXyz operators return through a hidden result pointer: (this, result, arg) [v kamome __mi] */
inline void cXyz_mi(const cXyz* a, cXyz* res, const cXyz* b) { gabi::call(0x0201ADE0, a, res, b); }
inline void cXyz_pl(const cXyz* a, cXyz* res, const cXyz* b) { gabi::call(0x0201AD78, a, res, b); }  /* [g] */
inline void cXyz_ml(const cXyz* a, cXyz* res, f32 s) { gabi::call(0x0201AE48, a, res, s); }        /* [g] */

/* ---- PSMTX / PSVEC (Cafe OS ports, no GameCube signature) ---- */
inline f32 PSVECSquareMag(const cXyz* v) { return gabi::call<f32>(0x028E8DD0, v); }                     /* [v kamome] */
inline void PSVECAdd(const cXyz* a, const cXyz* b, cXyz* out) { gabi::call(0x028E8D88, a, b, out); }   /* [v kamome] */
inline void PSVECScale(const cXyz* in, cXyz* out, f32 s) { gabi::call(0x028E8E64, in, out, s); }       /* [g] */
inline f32 PSVECDotProduct(const cXyz* a, const cXyz* b) { return gabi::call<f32>(0x028E8F44, a, b); } /* [g] */
inline void PSMTXMultVec(const Mtx34* m, const cXyz* in, cXyz* out) { gabi::call(0x028E8F64, m, in, out); }     /* [g] */
inline void PSMTXMultVecSR(const Mtx34* m, const cXyz* in, cXyz* out) { gabi::call(0x028E9044, m, in, out); }   /* [g] */
inline void mDoMtx_ZXYrotM(Mtx34* m, s16 x, s16 y, s16 z) { gabi::call(0x025F1B48, m, x, y, z); }      /* [g] */
/* mDoMtx_stack_c::transM / scaleM: static, on mDoMtx_stack_c::now (no `this`) */
inline void mDoMtx_stack_transM(f32 x, f32 y, f32 z) { gabi::call(0x025F24E0, x, y, z); } /* [g] */
inline void mDoMtx_stack_scaleM(f32 x, f32 y, f32 z) { gabi::call(0x025F2518, x, y, z); } /* [v spotbox] */
inline void MtxScale(f32 x, f32 y, f32 z, u8 concat) { gabi::call(0x0200FC74, x, y, z, concat); }      /* [g] */
inline void MtxPush() { gabi::call(0x0200FCF0); }                                                      /* [g] */
inline void MtxPull() { gabi::call(0x0200FD38); }                                                      /* [g] */

/* ---- save events (dSv_event_c in the save info) ---- */
/* HD: dComIfGs_isEventBit(flag) = dSv_event_c::isEventBit(&info.mSavedata.mEvent, flag); the
 * event block's address is still to be confirmed by a verified function: pass it explicitly */
inline BOOL dSv_event_isEventBit(dSv_event_c* ev, u16 flag) { return gabi::call<BOOL>(0x025B8B94, ev, flag); } /* [g] */
inline void dSv_event_onEventBit(dSv_event_c* ev, u16 flag) { gabi::call(0x025B8B68, ev, flag); }             /* [g] */
inline u8 dSv_event_getEventReg(dSv_event_c* ev, u16 reg) { return gabi::call<u8>(0x025B8BB0, ev, reg); }     /* [g] */
inline void dSv_event_setEventReg(dSv_event_c* ev, u16 reg, u8 v) { gabi::call(0x025B8AF4, ev, reg, v); }     /* [g] */

/* ---- environment ---- */
inline BOOL dKy_daynight_check() { return gabi::call<BOOL>(0x02556D14); }  /* [g] */
inline s32 dKy_rain_check() { return gabi::call<s32>(0x0256019C); }        /* [v kamome] */
inline f32 dComIfGs_getTime() { return gabi::load<f32>(gabi::load<u32>(0x101F84DC) + 0x44); } /* [v kamome] */
inline BOOL daSea_ChkArea(f32 x, f32 z) { return gabi::call<BOOL>(0x0246B6A4, x, z); }  /* [v kamome] */
inline f32 daSea_calcWave(f32 x, f32 z) { return gabi::call<f32>(0x0246BA0C, x, z); }   /* [v kamome] */
inline f32 dBgS_GetWaterHeight(cXyz* pos) { return gabi::call<f32>(0x024F17D4, pos); }  /* [v kamome] */
inline f32 cBgS_GroundCross(dBgS* bgs, void* chk) { return gabi::call<f32>(0x02008974, bgs, chk); } /* [v kamome] */
inline void fopKyM_createWpillar(const cXyz* pos, f32 sx, f32 sz, s32 p) { gabi::call(0x025DAE64, pos, sx, sz, p); } /* [g] */

/* ---- particles (dPa_control_c* at play+PLAY_PARTICLE) ---- */
/* 025A847C dPa_control_c::set(u8 grp, u16 id, pos, angle, scale, u8 alpha, cb, s8 setupInfo, prmColor,
 * envColor, scale2D) [g] [v lamp: this = dComIfGp_getParticle(), grp 0, setup -1 on the stack] */
inline JPABaseEmitter* dPa_control_set(dPa_control_c* pa, u8 grp, u16 id, const cXyz* pos, const csXyz* angle,
                                       const cXyz* scale, u8 alpha, dPa_levelEcallBack* cb, s8 setup,
                                       const GXColor* prm, const GXColor* env, const cXyz* scale2D) {
    return gabi::call<JPABaseEmitter*>(0x025A847C, pa, grp, id, pos, angle, scale, alpha, cb, setup, prm, env, scale2D);
}
WWHD_OPAQUE(dPa_smokeEcallBack);
WWHD_OPAQUE(dPa_rippleEcallBack);
/* dPa_followEcallBack (0x14, HD: vtable at +0; constructor 025A5894(this, u8, u8)) [v lamp] */
struct dPa_followEcallBack {
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ gptr<JPABaseEmitter> mpEmitter;
    /* 0x08 */ be<u32> m08;
    /* 0x0C */ be<u32> m0C;
    /* 0x10 */ be<u8> m10;
    /* 0x11 */ be<u8> m11;
    /* 0x12 */ be<u8> m12;
    /* 0x13 */ be<u8> m13;
    JPABaseEmitter* getEmitter() { return mpEmitter; }
    /* remove(): virtual (vtable +0x44) */
    void remove() { gabi::call_ptr(gabi::load<u32>(__vtbl + 0x44), this); }
};
WWHD_SIZE(dPa_followEcallBack, 0x14);
inline void dPa_followEcallBack_ct(dPa_followEcallBack* p, u8 a, u8 b) { gabi::call(0x025A5894, p, a, b); }
/* JPABaseEmitter::setDirection (inline): direction at +0x28 (HD) [v lamp] */
inline void JPABaseEmitter_setDirection(JPABaseEmitter* e, f32 x, f32 y, f32 z) {
    gabi::store<f32>(gabi::ea(e) + 0x28, x);
    gabi::store<f32>(gabi::ea(e) + 0x2C, y);
    gabi::store<f32>(gabi::ea(e) + 0x30, z);
}
/* dComIfGp_particle_set / setSimple: through the play object's dPa_control_c* (play+0x5AB0) [v lamp] */
inline JPABaseEmitter* dComIfGp_particle_set(u16 id, const cXyz* pos, const csXyz* angle = nullptr, const cXyz* scale = nullptr,
                                             u8 alpha = 0xFF, dPa_levelEcallBack* cb = nullptr, s8 setup = -1,
                                             const GXColor* prm = nullptr, const GXColor* env = nullptr, const cXyz* scale2D = nullptr) {
    dPa_control_c* pa = dComIfGp_getParticle();
    return dPa_control_set(pa, 0, id, pos, angle, scale, alpha, cb, setup, prm, env, scale2D);
}
/* HD: setSimple(id, pos, alpha, const GXColor& prm, const GXColor& env, int); the default colours are
 * g_whiteColor (0x101D5E98) [v lamp] */
inline BOOL dComIfGp_particle_setSimple(u16 id, const cXyz* pos, u8 alpha = 0xFF, const GXColor* prm = gabi::at<GXColor>(0x101D5E98),
                                        const GXColor* env = gabi::at<GXColor>(0x101D5E98), s32 p = 0) {
    dPa_control_c* pa = dComIfGp_getParticle();
    return gabi::call<BOOL>(0x025A8D40, pa, id, pos, alpha, prm, env, p);
}
inline void dPa_smokeEcallBack_end(dPa_smokeEcallBack* cb) { gabi::call(0x025A5F88, cb); }    /* [g] */
inline void dPa_followEcallBack_end(dPa_followEcallBack* cb) { gabi::call(0x025A5AC8, cb); }  /* [g] */
inline void dPa_rippleEcallBack_end(dPa_rippleEcallBack* cb) { gabi::call(0x025A9270, cb); }  /* [g] */

/* ---- point lights (d_kankyo) ---- */
/* LIGHT_INFLUENCE, HD 0x24 (GameCube 0x20): +0x20 an HD float, 1.0 after construction [v lamp] */
struct LIGHT_INFLUENCE {
    /* 0x00 */ cXyz mPos;
    /* 0x0C */ be<s16> mColorR, mColorG, mColorB, mColorA;
    /* 0x14 */ be<f32> mPower;
    /* 0x18 */ be<f32> mFluctuation;
    /* 0x1C */ be<s32> mIdx;
    /* 0x20 */ be<f32> mHD20;
};
WWHD_SIZE(LIGHT_INFLUENCE, 0x24);
inline void dKy_plight_set(LIGHT_INFLUENCE* l) { gabi::call(0x025564B4, l); }  /* [v lamp] */
inline void dKy_plight_cut(LIGHT_INFLUENCE* l) { gabi::call(0x0255A374, l); }  /* [v lamp] */

/* ---- c_lib matrix stack (calc_mtx) ---- */
inline void MtxRotX(f32 rad, u8 concat) { gabi::call(0x0200FB3C, rad, concat); }  /* [v lamp] */
inline void MtxRotZ(f32 rad, u8 concat) { gabi::call(0x0200FC0C, rad, concat); }  /* [v lamp] */

/* ---- audio / vibration ---- */
inline BOOL dComIfGp_getVibration_CheckQuake() { return gabi::call<BOOL>(0x025CB66C, dComIfGp_getVibration()); } /* [v lamp] */
inline void mDoAud_seDeleteObject(cXyz* pos) { gabi::call(0x025E1B34, pos); } /* 025E1B34 JAIZelBasic::seDeleteObject: HD static? [g] */

/* ---- per-TU static initialisation (HD) ----
 * Every actor's __sinit starts with the same header statics (one copy per translation unit):
 * a zeroed 16-byte object at P+0xC, a float pair {-pi, pi} at P, two objects at P+8 / P+9
 * initialised by 028ED6F8 / 028EAB2C, and three __register_global_object descriptors D, D+0xC,
 * D+0x18 (.data). [v kamome, swc00] */
inline void __register_global_object(u32 desc) { gabi::call(0x028F026C, desc); }
/* the zeroed 16-byte object is usually at P+0xC; some units place it elsewhere (Z) [v ba1/ls1/cc helpers] */
inline void sinit_header_statics_z(u32 P, u32 D, u32 Z) {
    for (int i = 0; i < 4; i++) gabi::store<u32>(Z + 4 * i, 0);
    __register_global_object(D);
    gabi::store<f32>(P, -3.1415927f);
    gabi::store<f32>(P + 4, 3.1415927f);
    gabi::call(0x028ED6F8, P + 8);
    __register_global_object(D + 0xC);
    gabi::call(0x028EAB2C, P + 9);
    __register_global_object(D + 0x18);
}
inline void sinit_header_statics(u32 P, u32 D) {
    for (int i = 0; i < 4; i++) gabi::store<u32>(P + 0xC + 4 * i, 0);
    __register_global_object(D);
    gabi::store<f32>(P, -3.1415927f);
    gabi::store<f32>(P + 4, 3.1415927f);
    gabi::call(0x028ED6F8, P + 8);
    __register_global_object(D + 0xC);
    gabi::call(0x028EAB2C, P + 9);
    __register_global_object(D + 0x18);
}

/* ---- items ---- */
/* 025D8120 fopAcM_createItemFromTable(pos, tbl, bitNo, roomNo, type, angle, action, scale) -> proc id [v switem] */
inline fpc_ProcID fopAcM_createItemFromTable(cXyz* pos, s32 tbl, s32 bitNo, s32 roomNo, s32 type, csXyz* angle, s32 action, cXyz* scale = nullptr) {
    return gabi::call<fpc_ProcID>(0x025D8120, pos, tbl, bitNo, roomNo, type, angle, action, scale);
}
inline BOOL dComIfGs_isItem(s32 bitNo, s32 roomNo) { return gabi::call<BOOL>(0x025BA494, dComIfGs_info(), bitNo, roomNo); } /* [v switem] */
inline BOOL fopAcM_isItem(fopAc_ac_c* a, s32 bitNo) { return dComIfGs_isItem(bitNo, a->home.roomNo); }      /* [v switem] home room */
inline BOOL isRupee(u8 itemNo) { return gabi::call<BOOL>(0x02551044, itemNo); }                                /* [v switem] */
/* dComIfGp_getItemTable(): play+0x5D24 [v switem] */
inline u32 dComIfGp_getItemTable() { return gabi::load<u32>(dComIfGp_ea() + 0x5D24); }
/* 0201A478 csXyz::csXyz(s16, s16, s16) (out of line) [v switem] */
inline csXyz* csXyz_ct(csXyz* p, s16 x, s16 y, s16 z) { return gabi::call<csXyz*>(0x0201A478, p, x, y, z); } /* returns this */
/* fopAcM_offDraw: fopDwTg_DrawQTo(&draw_tag) (draw_tag at +0xDC) [v switem] */
inline void fopAcM_offDraw(fopAc_ac_c* a) { gabi::call(0x025DA884, gabi::at<u8>(gabi::ea(a) + 0xDC)); }

/* ---- draw lists (HD): j3dSys opa/xlu draw buffers at 0x104B4634/0x104B4638, play lists at
 * play+0x5D70.. (BG opa/xlu 0x5D70/0x5D74, normal opa/xlu 0x5D78/0x5D7C) [v obj_table] ---- */
inline void dComIfGd_setListBG() {
    gabi::store<u32>(0x104B4634, gabi::load<u32>(dComIfGp_ea() + 0x5D70));
    gabi::store<u32>(0x104B4638, gabi::load<u32>(dComIfGp_ea() + 0x5D74));
}
inline void dComIfGd_setList() {
    gabi::store<u32>(0x104B4634, gabi::load<u32>(dComIfGp_ea() + 0x5D78));
    gabi::store<u32>(0x104B4638, gabi::load<u32>(dComIfGp_ea() + 0x5D7C));
}
/* J3DModel::setBaseScale: scale at +0xBC (HD) [v obj_table] */
inline void J3DModel_setBaseScale(J3DModel* m, const cXyz* s) {
    f32 x = s->x, y = s->y, z = s->z;
    gabi::store<f32>(gabi::ea(m) + 0xBC, x);
    gabi::store<f32>(gabi::ea(m) + 0xC0, y);
    gabi::store<f32>(gabi::ea(m) + 0xC4, z);
}

/* ---- pointers to member functions (GHS): {s16 this delta, s16 vtable index (<0: not virtual),
 * u32 function or vtable offset}, 8 bytes per entry [v ladder] ---- */
inline void ptmf_call(u32 entry, void* self) {
    s16 delta = gabi::load<s16>(entry);
    s16 idx = gabi::load<s16>(entry + 2);
    void* p = gabi::at<void>(gabi::ea(self) + delta);
    if (idx < 0) {
        gabi::call_ptr(gabi::load<u32>(entry + 4), p);
    } else {
        u32 vt = gabi::load<u32>(gabi::ea(p) + gabi::load<s16>(entry + 6));
        gabi::call_ptr(gabi::load<u32>(vt + idx * 8 + 4), p);
    }
}

/* ---- events / switches ---- */
inline BOOL fopAcM_isSwitch(fopAc_ac_c* a, s32 sw) { return dComIfGs_isSwitch(sw, a->home.roomNo); }   /* [v ladder] home room */
/* dComIfGp_evmng_existence(idx): getEventData(idx) != NULL [v ladder] */
inline bool dComIfGp_evmng_existence(s16 idx) { return gabi::call<u32>(0x02544044, dComIfGp_getPEvtManager(), idx) != 0; }
/* dComIfGp_event_reset(): the event control's flag word (play+0x52B8) |= 8 [v ladder] */
inline void dComIfGp_event_reset() {
    u32 a = dComIfGp_ea() + 0x52B8;
    gabi::store<u16>(a, (u16)(gabi::load<u16>(a) | 8));
}
/* dEvt_info_c eventInfo at actor+0xF8: mCommand (u16) +0xF8, mCondition (u16) +0xFA [v ladder] */
inline bool eventInfo_checkCommandDemoAccrpt(fopAc_ac_c* a) { return gabi::load<u16>(gabi::ea(a) + 0xF8) == 2; }
inline void eventInfo_onCondition(fopAc_ac_c* a, u16 c) { u32 p = gabi::ea(a) + 0xFA; gabi::store<u16>(p, (u16)(gabi::load<u16>(p) | c)); }
/* 025CB374 dVibration_c::StartShock(int, int, cXyz) (cXyz by value: pointer to a copy) [v ladder] */
inline BOOL dComIfGp_getVibration_StartShock(s32 strength, s32 flags, cXyz* pos) { return gabi::call<BOOL>(0x025CB374, dComIfGp_getVibration(), strength, flags, pos); }
inline void mDoMtx_stack_push() { gabi::call(0x025F23EC); }  /* [v ladder] */
inline void mDoMtx_stack_pop() { gabi::call(0x025F2468); }   /* [v ladder] */
/* cXyz::Zero (0x101FFBA8) [v ladder] */
#define cXyz_Zero gabi::at<cXyz>(0x101FFBA8)
/* 023123C0 daObj::posMoveF_stream(actor, const cXyz* ccMove, const cXyz* stream, f32 k1, f32 k2) [v ladder] */
inline void daObj_posMoveF_stream(fopAc_ac_c* a, const cXyz* cc, const cXyz* stream, f32 k1, f32 k2) { gabi::call(0x023123C0, a, cc, stream, k1, k2); }
