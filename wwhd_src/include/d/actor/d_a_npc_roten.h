/* daNpcRoten_c (Traveling Merchants), WWHD layout. 
 *
 * GameCube -> WWHD, measured from the constructor (022D30F4, allocates 0xB40; the matcher calls
 * it daNpcRoten_c::setAnm) and the functions of d_a_npc_roten:
 * - fopNpc_npc_c is 0x7DC (d/d_npc.h), so members are +0x118 up to mBtpAnm;
 * - mDoExt_btpAnm grew from 0x14 to 0x74 (+0x60) and mShadowId (GameCube 0x6F4) is gone (HD
 *   shadows, -4): everything from field_0x6F8 on is +0x174.
 * Size 0xB40 (GameCube 0x9CC). */
#pragma once
#include "bindings.h"

struct sRotenAnmDat {
    /* 0x00 */ be<u8> field_0x00;
    /* 0x01 */ be<u8> field_0x01;
    /* 0x02 */ be<u8> field_0x02;
};
WWHD_SIZE(sRotenAnmDat, 3);

/* dNpc_PathRun_c (8 bytes) */
struct dNpc_PathRun_r {
    /* 0x00 */ gptr<dPath> mPath;
    /* 0x04 */ be<u8> mCurrPointIndex;
    /* 0x05 */ be<u8> mIdx;
    /* 0x06 */ be<u8> mbDir;
    /* 0x07 */ be<u8> field_0x07;
    bool isPath() { return mPath.get() != nullptr; }
    u32 getDir() { return mbDir != 0; }
    void turnDir() { mbDir = mbDir ^ 1; }
};
WWHD_SIZE(dNpc_PathRun_r, 8);

/* l_npc_dat entry (.data 0x101C5D54, 3 entries of 0x54) */
struct RotenNpcDat {
    /* 0x00 */ be<f32> field_0x00;
    /* 0x04 */ be<s16> field_0x04;
    /* 0x06 */ be<s16> field_0x06;
    /* 0x08 */ be<s16> field_0x08;
    /* 0x0A */ be<s16> field_0x0A;
    /* 0x0C */ be<s16> field_0x0C;
    /* 0x0E */ be<s16> field_0x0E;
    /* 0x10 */ be<s16> field_0x10;
    /* 0x12 */ be<s16> field_0x12;
    /* 0x14 */ be<s16> field_0x14;
    /* 0x16 */ be<s16> field_0x16;
    /* 0x18 */ be<s16> field_0x18;
    /* 0x1A */ be<s16> field_0x1A;
    /* 0x1C */ be<f32> field_0x1C;
    /* 0x20 */ be<s16> field_0x20;
    /* 0x22 */ u8 _22[2];
    /* 0x24 */ be<f32> field_0x24;
    /* 0x28 */ be<f32> field_0x28;
    /* 0x2C */ be<f32> field_0x2C;
    /* 0x30 */ be<f32> field_0x30;
    /* 0x34 */ be<f32> field_0x34;
    /* 0x38 */ be<f32> field_0x38;
    /* 0x3C */ be<f32> field_0x3C;
    /* 0x40 */ be<f32> field_0x40;
    /* 0x44 */ be<f32> field_0x44;
    /* 0x48 */ be<s16> field_0x48;
    /* 0x4A */ be<s16> field_0x4A;
    /* 0x4C */ be<s16> field_0x4C;
    /* 0x4E */ be<s16> field_0x4E;
    /* 0x50 */ be<s16> field_0x50;
    /* 0x52 */ be<s16> field_0x52;
};
WWHD_SIZE(RotenNpcDat, 0x54);

struct daNpcRoten_c : fopNpc_npc_c {
    enum Prm_e {
        PRM_RAIL_ID_W = 0x08,
        PRM_RAIL_ID_S = 0x10,
        PRM_NPC_NO_W = 0x08,
        PRM_NPC_NO_S = 0x18,
    };

    cPhs_State _create();
    BOOL createHeap();
    cPhs_State createInit();
    bool _delete();
    bool _draw();
    bool _execute();
    u8 executeCommon(); /* returns field_0x9B4 as stored */
    void executeSetMode(u8);
    s32 executeWaitInit();
    void executeWait();
    s32 executeTalkInit();
    void executeTalk();
    s32 executeWalkInit();
    void executeWalk();
    s32 executeTurnInit();
    void executeTurn();
    s32 executeWindInit();
    void executeWind();
    void checkOrder();
    void eventOrder();
    void eventMove();
    void privateCut();
    void eventMesSetInit(int);
    bool eventMesSet();
    void eventSetItemInit();
    bool eventSetItem();
    void eventClrItemInit();
    void eventGetItemInit(int);
    void eventSetAngleInit();
    void eventOnPlrInit();
    void eventOffPlrInit();
    u16 next_msgStatus(be<u32>*);
    u32 getMsg();
    void setMessage(u32);
    void setAnmFromMsgTag();
    u8 getPrmNpcNo();
    u8 getPrmRailID();
    void setMtx();
    void chkAttention();
    void lookBack();
    BOOL initTexPatternAnm(u8); /* bool, passed on unnormalised */
    void playTexPatternAnm();
    void playAnm();
    void setAnm(u8, int, f32);
    bool setAnmTbl(sRotenAnmDat*);
    BOOL isHaitatuItem(u8);
    BOOL isKoukanItem(u8);
    BOOL isGetMap(u8);
    s16 XyEventCB(int);
    void setCollisionB();
    void setCollisionH();

    /* 0x7DC */ request_of_phase_process_class mPhs;
    /* 0x7E4 */ request_of_phase_process_class mPhs2;
    /* 0x7EC */ gptr<J3DModel> field_0x6D4;
    /* 0x7F0 */ gptr<mDoExt_McaMorf> field_0x6D8;
    /* 0x7F4 */ gptr<J3DAnmTexPattern> m_head_tex_pattern;
    /* 0x7F8 */ u8 mBtpAnm[0x74];                 /* mDoExt_btpAnm (HD 0x74); HD: no mShadowId after it */
    /* 0x86C */ be<u32> field_0x6F8;
    /* 0x870 */ be<u32> field_0x6FC;
    /* 0x874 */ dNpc_PathRun_r mPathRun;
    /* 0x87C */ cXyz field_0x708;
    /* 0x888 */ cXyz field_0x714;
    /* 0x894 */ cXyz field_0x720;
    /* 0x8A0 */ dCcD_Cyl mCyl2;
    /* 0x9D0 */ dCcD_Sph mSph;
    /* 0xAFC */ gptr<sRotenAnmDat> field_0x988;
    /* 0xB00 */ gptr<be<u32>> field_0x98C;
    /* 0xB04 */ be<f32> field_0x990;
    /* 0xB08 */ be<f32> field_0x994;
    /* 0xB0C */ be<s32> field_0x998;
    /* 0xB10 */ be<u8> field_0x99C;
    /* 0xB11 */ u8 _B11;
    /* 0xB12 */ be<s16> field_0x99E;
    /* 0xB14 */ be<s16> field_0x9A0;
    /* 0xB16 */ be<s16> field_0x9A2;
    /* 0xB18 */ be<s16> field_0x9A4;
    /* 0xB1A */ be<s16> field_0x9A6;
    /* 0xB1C */ be<s16> field_0x9A8;
    /* 0xB1E */ be<s16> field_0x9AA;
    /* 0xB20 */ be<s16> field_0x9AC;
    /* 0xB22 */ be<s16> field_0x9AE;
    /* 0xB24 */ be<s16> field_0x9B0;
    /* 0xB26 */ be<u16> field_0x9B2;
    /* 0xB28 */ be<u8> field_0x9B4;
    /* 0xB29 */ be<u8> field_0x9B5;
    /* 0xB2A */ be<u8> field_0x9B6;
    /* 0xB2B */ be<u8> field_0x9B7;
    /* 0xB2C */ be<u8> field_0x9B8;
    /* 0xB2D */ be<s8> m_hand_L_jnt_num;
    /* 0xB2E */ be<s8> m_bag_jnt_num;
    /* 0xB2F */ be<u8> field_0x9BB;
    /* 0xB30 */ be<u8> field_0x9BC;
    /* 0xB31 */ be<u8> mNpcNo;
    /* 0xB32 */ be<u8> field_0x9BE;
    /* 0xB33 */ be<u8> field_0x9BF;
    /* 0xB34 */ be<u8> field_0x9C0;
    /* 0xB35 */ be<u8> field_0x9C1;
    /* 0xB36 */ be<s8> field_0x9C2;
    /* 0xB37 */ be<s8> field_0x9C3;
    /* 0xB38 */ be<s8> field_0x9C4;
    /* 0xB39 */ be<u8> mShownItemBtn;
    /* 0xB3A */ be<u8> field_0x9C6;
    /* 0xB3B */ be<u8> field_0x9C7;
    /* 0xB3C */ be<u8> field_0x9C8;
    /* 0xB3D */ be<u8> field_0x9C9;
    /* 0xB3E */ be<u8> field_0x9CA;
    /* 0xB3F */ u8 _B3F;
};
WWHD_OFFSET(daNpcRoten_c, mBtpAnm, 0x7F8);
WWHD_OFFSET(daNpcRoten_c, field_0x6F8, 0x86C);
WWHD_OFFSET(daNpcRoten_c, mCyl2, 0x8A0);
WWHD_OFFSET(daNpcRoten_c, mSph, 0x9D0);
WWHD_OFFSET(daNpcRoten_c, field_0x988, 0xAFC);
WWHD_OFFSET(daNpcRoten_c, field_0x9B2, 0xB26);
WWHD_OFFSET(daNpcRoten_c, mNpcNo, 0xB31);
WWHD_SIZE(daNpcRoten_c, 0xB40);

/* ---- local bindings (SHARED-CANDIDATE) ---- */
#define ROTEN_SAFESTRING_VTBL 0x10021278 /* this TU's sead::SafeString vtable */
#define ROTEN_VTBL 0x10021614            /* daNpcRoten_c vtable */

/* .data / .rodata tables */
enum : u32 {
    l_arcname_tbl = 0x101C5A38,      /* const char*[3] ("Ro") */
    l_npc_staff_id = 0x101C5D3C,     /* const char*[3] ("Roten") */
    l_npc_dat = 0x101C5D54,          /* RotenNpcDat[3] */
    l_bmd_ix_tbl = 0x100212D8,
    l_head_bmd_ix_tbl = 0x100212E4,
    l_head_bck_ix_tbl = 0x100212F0,
    l_btp_ix_tbl = 0x100212FC,
    l_bck_ix_tbl = 0x1002132C,       /* s32[3][10] */
    l_save_dat = 0x100212C0,         /* SaveDatStruct[3] (u16 x4) */
    l_item_dat = 0x10021308,         /* u8[3][12] */
    l_npc_anm_wait = 0x101C5478,
    l_npc_anm_walk = 0x101C547E,
    l_npc_anm_wind = 0x101C5D48,
    l_sph_src = 0x101C5988,
    dNpc_cyl_src = 0x101EA190,
};
static inline RotenNpcDat& npc_dat(u32 no) { return *gabi::at<RotenNpcDat>(l_npc_dat + no * 0x54); }
static inline const char* arcname(u32 no) { return gabi::at<const char>(gabi::load<u32>(l_arcname_tbl + no * 4)); }
static inline u16 save_dat(u32 no, u32 field) { return gabi::load<u16>(l_save_dat + no * 8 + field); }
static inline u8 item_dat(u32 no, u32 idx) { return gabi::load<u8>(l_item_dat + no * 12 + idx); }

/* save events (dSv_event_c at save + 0x644) */
static inline dSv_event_c* roten_event() { return gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x644); }
static inline BOOL dComIfGs_isEventBit(u16 f) { return dSv_event_isEventBit(roten_event(), f); }
static inline void dComIfGs_onEventBit(u16 f) { dSv_event_onEventBit(roten_event(), f); }
static inline u8 dComIfGs_getEventReg(u16 r) { return dSv_event_getEventReg(roten_event(), r); }
static inline void dComIfGs_setEventReg(u16 r, u8 v) { dSv_event_setEventReg(roten_event(), r, v); }
/* dSv_player_get_bag_item_c at save + 0xB0, dSv_player_bag_item_c at save + 0x96 */
static inline void dComIfGs_onGetItemReserve(u8 i) { gabi::call(0x025B77D8, gabi::load<u32>(0x101F84DC) + 0xB0, i); }
static inline BOOL dComIfGs_isGetItemReserve(u8 i) { return gabi::call<BOOL>(0x025B7840, gabi::load<u32>(0x101F84DC) + 0xB0, i); }
static inline void dComIfGs_setReserveItemEmpty() { gabi::call(0x025B7270, gabi::load<u32>(0x101F84DC) + 0x96); }
static inline void dComIfGs_setReserveItemChange(u8 btn, u8 item) { gabi::call(0x025B7278, gabi::load<u32>(0x101F84DC) + 0x96, btn, item); }
static inline u16 dComIfGs_getRupee() { return gabi::load<u16>(gabi::load<u32>(0x101F84DC) + 0x24); }

/* play object fields */
static inline u8 dComIfGp_event_runCheck() { return gabi::load<u8>(dComIfGp_ea() + 0x5292); }
static inline u8 dComIfGp_event_getTalkXYBtn() { return gabi::load<u8>(dComIfGp_ea() + 0x52B0); }
static inline BOOL dComIfGp_event_chkTalkXY() { return (u32)(gabi::load<u8>(dComIfGp_ea() + 0x52B0) - 1) <= 3; }
static inline u8 dComIfGp_event_getPreItemNo() { return gabi::load<u8>(dComIfGp_ea() + 0x52B1); }
static inline void dComIfGp_event_setItemPartnerId(u32 id) { gabi::store<u32>(dComIfGp_ea() + 0x52A0, id); }
static inline BOOL dComIfGp_evmng_ChkPresentEnd() { return gabi::call<BOOL>(0x02544950, dComIfGp_getPEvtManager()); }
static inline u8 dComIfGp_getSelectItem(int btn) { return gabi::load<u8>(dComIfGp_ea() + btn + 0x5BBB); }
static inline u8 dComIfGp_getMesgAnimeAttrInfo() { return gabi::load<u8>(dComIfGp_ea() + 0x5BC5); }
static inline void dComIfGp_clearMesgAnimeAttrInfo() { gabi::store<u8>(dComIfGp_ea() + 0x5BC5, 0xFF); }
static inline s16 dComIfGp_getMessageRupee() { return gabi::load<s16>(dComIfGp_ea() + 0x5BA4); }
static inline s16 dComIfGp_evmng_getEventIdx_r(u32 name) { return dComIfGp_evmng_getEventIdx(gabi::at<const char>(name), 0xFF); }
static inline s8 dComIfGp_evmng_getMyActIdx(s32 staffId, u32 tbl, s32 n, BOOL force, s32 nameType) {
    return gabi::call<s8>(0x02542EDC, dComIfGp_getPEvtManager(), staffId, tbl, n, force, nameType);
}
static inline void* dComIfGp_evmng_getMyIntegerP(s32 staffId, const char* name) { return dComIfGp_evmng_getMySubstanceP(staffId, name, 3); }
/* HD message manager (*(0x101F4B5C)): the selected answer at +0x948 (GameCube mpCurrMsg->mSelectNum) */
static inline u32 msgMgr() { return gabi::load<u32>(0x101F4B5C); }

/* resources (SafeString keys with this TU's vtable) */
static inline void* dComIfG_getObjectIDRes(const char* arc, s32 id) {
    gabi::Local<SafeString> key;
    key->mStringTop = gabi::ea(arc);
    key->__vtbl = ROTEN_SAFESTRING_VTBL;
    return gabi::call<void*>(0x026067F4, dComIfG_resControl(), key.get(), id);
}

/* actors */
/* 025D54C4 fopAcM_SearchByID(id, fopAc_ac_c** out) (out of line) */
static inline BOOL fopAcM_SearchByID_o(u32 id, gptr<fopAc_ac_c>* out) { return gabi::call<BOOL>(0x025D54C4, id, out); }
/* 025D7DEC fopAcM_createItemForPresentDemo(pos, itemNo, argFlag, itemBitNo, roomNo, angle, scale) */
static inline u32 fopAcM_createItemForPresentDemo(cXyz* pos, u8 itemNo, u8 flag, s32 bitNo, s32 roomNo) {
    return gabi::call<u32>(0x025D7DEC, pos, itemNo, flag, bitNo, roomNo, (u32)0, (u32)0);
}
/* 025D7970 fopAcM_orderChangeEventId(actor, partner, s16 eventIdx, u16 flag, u16 hind) */
static inline BOOL fopAcM_orderChangeEventId(fopAc_ac_c* a, fopAc_ac_c* b, s16 ev, u16 flag, u16 hind) {
    return gabi::call<BOOL>(0x025D7970, a, b, ev, flag, hind);
}
/* daItemBase_c::dead (0218432C), ::show (021842C8) */
static inline void daItemBase_dead(fopAc_ac_c* a) { gabi::call(0x0218432C, a); }
static inline void daItemBase_show(fopAc_ac_c* a) { gabi::call(0x021842C8, a); }

/* d_npc */
/* 0259D624 dNpc_calc_DisXZ_AngY(cXyz, cXyz (pointers to copies), f32* dist, s16* angle) */
static inline void dNpc_calc_DisXZ_AngY(cXyz* a, cXyz* b, be<f32>* dist, be<s16>* ang) { gabi::call(0x0259D624, a, b, dist, ang); }
static inline void dNpc_playerEyePos_r(cXyz* out, f32 offs) { gabi::call(0x0259D54C, out, offs); }
static inline void dNpc_JntCtrl_lookAtTarget(dNpc_JntCtrl_c* j, be<s16>* outY, cXyz* target, cXyz* eye, s16 yrot, s16 vel, u8 headOnly) {
    gabi::call(0x0259DED0, j, outY, target, eye, yrot, vel, headOnly);
}
static inline void dNpc_PathRun_setInf(dNpc_PathRun_r* r, u8 id, s8 room, u8 b) { gabi::call(0x0259E6D0, r, id, room, b); }
static inline void dNpc_PathRun_getPoint(dNpc_PathRun_r* r, cXyz* out, u8 idx) { gabi::call(0x0259E778, r, out, idx); }
static inline void dNpc_PathRun_incIdxLoop(dNpc_PathRun_r* r) { gabi::call(0x0259EB60, r); }
static inline BOOL dNpc_PathRun_chkPointPass(dNpc_PathRun_r* r, cXyz* pos, u32 dir) { return gabi::call<BOOL>(0x0259E838, r, pos, dir); }
static inline BOOL dNpc_PathRun_nextIdxAuto(dNpc_PathRun_r* r) { return gabi::call<BOOL>(0x0259ED58, r); }

/* J3D (HD) */
static inline J3DModelData* J3DModel_getModelData_r(J3DModel* m) { return gabi::at<J3DModelData>(gabi::load<u32>(gabi::ea(m) + 0xAC)); }
/* J3DModel::getAnmMtx (HD: the matrix block's dirty flag is set on access) */
static inline Mtx34* getAnmMtx(J3DModel* model, s32 jntNo) {
    u32 blk = gabi::load<u32>(gabi::ea(model) + 0x2C);
    gabi::store<u16>(blk + 4, (u16)(gabi::load<u16>(blk + 4) | 0x10));
    return gabi::at<Mtx34>(gabi::load<u32>(blk + 0x10) + jntNo * 0x30);
}
static inline void PSMTXConcat(Mtx34* a, Mtx34* b, Mtx34* ab) { gabi::call(0x028E9108, a, b, ab); }
static inline s32 J3DAnm_getFrameMax(J3DAnmTexPattern* p) {
    u32 vt = gabi::load<u32>(gabi::ea(p) + 4);
    return gabi::call_ptr<s32>(gabi::load<u32>(vt + 0x14), p);
}
static inline s16 cLib_calcTimer(be<s16>* t) { return gabi::call<s16>(0x02055B64, t); }

/* GHS pointer to member function call through a table entry, returning r3 */
static inline s32 roten_pmf_call(void* self, u32 entry) {
    s16 i = gabi::load<s16>(entry + 2);
    u32 thisp = gabi::ea(self) + gabi::load<s16>(entry);
    if (i < 0) {
        return gabi::call_ptr<s32>(gabi::load<u32>(entry + 4), thisp);
    } else {
        u32 vt = gabi::load<u32>(thisp + gabi::load<s16>(entry + 6));
        return gabi::call_ptr<s32>(gabi::load<u32>(vt + i * 8 + 4), thisp);
    }
}
