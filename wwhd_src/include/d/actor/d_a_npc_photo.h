/* daNpcPhoto_c (Lenzo, Windfall photographer), WWHD layout.
 *
 * GameCube -> WWHD, measured from the constructor 022CC418 (allocates 0xB50) and the verified
 * functions: fopNpc_npc_c is 0x7DC (d/d_npc.h), so members are +0x118 up to mBtpAnm (0x7F0);
 * mDoExt_btpAnm grew from 0x14 to 0x74 and mShadowId (GameCube 0x6EC) is gone (HD shadows), so
 * everything from mPathRun (GameCube 0x6F0) on is +0x174. 0xB4E is an HD-only byte (zeroed by
 * the constructor). Size 0xB50 (GameCube 0x9DC). */
#pragma once
#include "bindings.h"

#define PHOTO_SAFESTRING_VTBL 0x10020C40 /* this TU's sead::SafeString vtable */
#define PHOTO_VTBL 0x10020FFC            /* daNpcPhoto_c vtable (constructor) */
#define PHOTO_AAB_VTBL 0x10020C58        /* this TU's cM3dGAab vtable */

struct sPhotoAnmDat {
    /* 0x00 */ be<u8> field_0x00;
    /* 0x01 */ be<u8> field_0x01;
    /* 0x02 */ be<u8> field_0x02;
};
WWHD_SIZE(sPhotoAnmDat, 3);

/* l_npc_dat (.data 0x101C521C) */
struct PhotoNpcDat_l {
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
    /* 0x1A */ u8 _1A[2];
    /* 0x1C */ be<f32> field_0x1C;
    /* 0x20 */ be<f32> field_0x20;
    /* 0x24 */ be<f32> field_0x24;
    /* 0x28 */ be<s16> field_0x28;
    /* 0x2A */ u8 _2A[2];
    /* 0x2C */ be<f32> field_0x2C;
    /* 0x30 */ be<f32> field_0x30;
    /* 0x34 */ be<f32> field_0x34;
    /* 0x38 */ be<f32> field_0x38;
    /* 0x3C */ be<f32> field_0x3C;
    /* 0x40 */ be<f32> field_0x40;
    /* 0x44 */ be<s16> field_0x44;
    /* 0x46 */ be<s16> field_0x46;
    /* 0x48 */ be<s16> field_0x48;
    /* 0x4A */ be<s16> field_0x4A;
    /* 0x4C */ be<s16> field_0x4C;
    /* 0x4E */ be<s16> field_0x4E;
    /* 0x50 */ be<s16> field_0x50;
    /* 0x52 */ be<u8> field_0x52;
    /* 0x53 */ be<u8> field_0x53;
};
WWHD_SIZE(PhotoNpcDat_l, 0x54);

/* dNpc_PathRun_c (8 bytes, unchanged) */
struct PhotoPathRun_l {
    /* 0x00 */ gptr<dPath> mPath;
    /* 0x04 */ be<u8> field_0x04;
    /* 0x05 */ be<u8> mIdx;
    /* 0x06 */ be<u8> mbDir;
    /* 0x07 */ be<u8> field_0x07;
};
WWHD_SIZE(PhotoPathRun_l, 8);

struct daNpcPhoto_c : fopNpc_npc_c {
    enum Prm_e {
        PRM_RAIL_ID_W = 0x8,
        PRM_RAIL_ID_S = 0x10,
        PRM_ARG0_W = 0x8,
        PRM_ARG0_S = 0x0,
    };

    request_of_phase_process_class* getPhaseP() { return &mPhs1; }

    cPhs_State _create();
    BOOL createHeap();
    cPhs_State createInit();
    bool _delete();
    bool _draw();
    bool _execute();
    u8 executeCommon(); /* bool, returned as loaded */
    void executeSetMode(u32); /* u8 */
    void executeWait();
    void executeTalk();
    void executeWalk();
    void executeTurn();
    void checkOrder();
    void eventOrder();
    void eventMove();
    void privateCut();
    void eventMesSetInit(int);
    bool eventMesSet();
    void eventSeSetInit(int);
    void eventPosSetInit();
    void eventGetItemInit();
    bool eventGetItem();
    void eventSetAngleInit();
    void eventSetEyeInit();
    bool eventSetEye();
    void eventTurnToPlayerInit();
    bool eventTurnToPlayer();
    void eventClrHanmeInit();
    void eventGetPhotoInit();
    bool eventGetPhoto();
    void eventMesSetUbInit(int);
    u32 eventMesSetUb(); /* bool; returns eventMesSet's register as is */
    bool eventLookUb();
    u16 talk2(int);
    void setMsgCamera();
    u16 next_msgStatus(be<u32>*);
    u32 getMsg();
    void setMessage(u32);
    void setAnmFromMsgTag();
    u8 getPrmRailID();
    u8 getPrmArg0();
    void setMtx();
    void chkAttention();
    void lookBack();
    BOOL initTexPatternAnm(u32, int); /* bool */
    void playTexPatternAnm();
    void playAnm();
    void setAnm(u32, int, f32); /* u8 */
    bool setAnmTbl(sPhotoAnmDat*);
    s16 XyCheckCB(int);
    s16 XyEventCB(int);
    BOOL isPhotoOk();
    BOOL isPhotoDxOk();
    void setCollision(dCcD_Cyl*, cXyz*, f32, f32); /* cXyz by value: pointer to a copy */

    /* 0x7DC */ request_of_phase_process_class mPhs1;
    /* 0x7E4 */ request_of_phase_process_class mPhs2;
    /* 0x7EC */ gptr<J3DAnmTexPattern> m_head_tex_pattern;
    /* 0x7F0 */ u8 mBtpAnm[0x74];          /* mDoExt_btpAnm (HD 0x74) */
    /* 0x864 */ PhotoPathRun_l mPathRun;   /* HD: no mShadowId before it */
    /* 0x86C */ dCcD_Cyl field_0x6F8[2];
    /* 0xACC */ cXyz field_0x958;
    /* 0xAD8 */ cXyz mLookAtPos;
    /* 0xAE4 */ cXyz mEyePos;
    /* 0xAF0 */ gptr<sPhotoAnmDat> mpAnmDat;
    /* 0xAF4 */ gptr<be<u32>> field_0x980;
    /* 0xAF8 */ be<f32> field_0x984;
    /* 0xAFC */ be<f32> field_0x988;
    /* 0xB00 */ be<f32> field_0x98C;
    /* 0xB04 */ be<s32> mMsgNno;
    /* 0xB08 */ be<u8> field_0x994;
    /* 0xB09 */ u8 _B09;
    /* 0xB0A */ be<s16> mPhotoLinkBackEventIdx;
    /* 0xB0C */ be<s16> mPhotoGetItemEventIdx;
    /* 0xB0E */ be<s16> mPhotoGetItem2EventIdx;
    /* 0xB10 */ be<s16> mPhotoGetPhotoEventIdx;
    /* 0xB12 */ be<s16> mPhotoGalleryEventIdx;
    /* 0xB14 */ be<s16> mPhotoCounterTalk0EventIdx;
    /* 0xB16 */ be<s16> mPhotoCounterTalk1EventIdx;
    /* 0xB18 */ be<s16> mPhotoDateUB4EventIdx;
    /* 0xB1A */ be<s16> field_0x9A6;
    /* 0xB1C */ be<s16> field_0x9A8;
    /* 0xB1E */ u8 _B1E[2];
    /* 0xB20 */ be<s16> field_0x9AC;
    /* 0xB22 */ be<s16> field_0x9AE;
    /* 0xB24 */ be<s16> field_0x9B0;
    /* 0xB26 */ be<s16> field_0x9B2;
    /* 0xB28 */ be<s16> mTimer;
    /* 0xB2A */ be<s16> field_0x9B6;
    /* 0xB2C */ be<s16> field_0x9B8;
    /* 0xB2E */ be<s16> field_0x9BA;
    /* 0xB30 */ be<u8> field_0x9BC;
    /* 0xB31 */ be<u8> field_0x9BD;
    /* 0xB32 */ be<u8> field_0x9BE;
    /* 0xB33 */ be<u8> mFrame;
    /* 0xB34 */ be<u8> field_0x9C0;
    /* 0xB35 */ be<u8> field_0x9C1;
    /* 0xB36 */ be<u8> field_0x9C2;
    /* 0xB37 */ be<u8> field_0x9C3;
    /* 0xB38 */ be<u8> field_0x9C4;
    /* 0xB39 */ be<u8> mItemNo;
    /* 0xB3A */ be<u8> field_0x9C6;
    /* 0xB3B */ be<u8> field_0x9C7;
    /* 0xB3C */ be<u8> field_0x9C8;
    /* 0xB3D */ be<u8> field_0x9C9;
    /* 0xB3E */ be<s8> field_0x9CA;
    /* 0xB3F */ be<s8> mActIdx;
    /* 0xB40 */ be<u8> field_0x9CC;
    /* 0xB41 */ be<u8> field_0x9CD;
    /* 0xB42 */ u8 _B42[2];
    /* 0xB44 */ be<u32> field_0x9D0;       /* u8* (camera index list) */
    /* 0xB48 */ be<u8> field_0x9D4;
    /* 0xB49 */ be<u8> field_0x9D5;
    /* 0xB4A */ be<s8> field_0x9D6;
    /* 0xB4B */ be<u8> field_0x9D7;
    /* 0xB4C */ be<u8> field_0x9D8;
    /* 0xB4D */ u8 _B4D;
    /* 0xB4E */ be<u8> mHD_B4E;            /* HD only: zeroed by the constructor */
    /* 0xB4F */ u8 _B4F;
};
WWHD_OFFSET(daNpcPhoto_c, mPhs1, 0x7DC);
WWHD_OFFSET(daNpcPhoto_c, mBtpAnm, 0x7F0);
WWHD_OFFSET(daNpcPhoto_c, mPathRun, 0x864);
WWHD_OFFSET(daNpcPhoto_c, field_0x6F8, 0x86C);
WWHD_OFFSET(daNpcPhoto_c, field_0x958, 0xACC);
WWHD_OFFSET(daNpcPhoto_c, mMsgNno, 0xB04);
WWHD_OFFSET(daNpcPhoto_c, field_0x9AE, 0xB22);
WWHD_OFFSET(daNpcPhoto_c, mFrame, 0xB33);
WWHD_OFFSET(daNpcPhoto_c, field_0x9D0, 0xB44);
WWHD_OFFSET(daNpcPhoto_c, mHD_B4E, 0xB4E);
WWHD_SIZE(daNpcPhoto_c, 0xB50);

/* ---- translation-unit statics (.data / .rodata addresses) ---- */
enum : u32 {
    PHOTO_l_arcname_tbl = 0x101C4F4C, /* const char*[1]: "Po" */
    PHOTO_l_npc_staff_id = 0x101C4F54,
    PHOTO_l_npc_dat = 0x101C521C,
    PHOTO_l_cyl_src2 = 0x101C5270,
    PHOTO_l_method = 0x101C52B4,
    PHOTO_l_bck_ix_tbl = 0x10020CD8,
    PHOTO_l_btp_ix_tbl = 0x10020C30,
};
static inline const char* photo_arcname() { return gabi::at<const char>(gabi::load<u32>(PHOTO_l_arcname_tbl)); }
static inline PhotoNpcDat_l& l_npc_dat() { return *gabi::at<PhotoNpcDat_l>(PHOTO_l_npc_dat); }

/* ---- local bindings (SHARED-CANDIDATE; the same as in d_a_npc_people.h) ---- */
/* save info: event flags at *(0x101F84DC) + 0x644, temporary flags at + 0x1178 */
static inline u32 dComIfGs_save() { return gabi::load<u32>(0x101F84DC); }
static inline dSv_event_c* dComIfGs_event() { return gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x644); }
static inline BOOL dComIfGs_isEventBit(u16 f) { return dSv_event_isEventBit(dComIfGs_event(), f); }
static inline void dComIfGs_onEventBit(u16 f) { dSv_event_onEventBit(dComIfGs_event(), f); }
static inline u8 dComIfGs_getEventReg(u16 reg) { return dSv_event_getEventReg(dComIfGs_event(), reg); }
static inline void dComIfGs_setEventReg(u16 reg, u8 v) { dSv_event_setEventReg(dComIfGs_event(), reg, v); }
static inline dSv_event_c* dComIfGs_tmpEvent() { return gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x1178); }
static inline BOOL dComIfGs_isTmpBit(u16 f) { return dSv_event_isEventBit(dComIfGs_tmpEvent(), f); }
static inline void dComIfGs_onTmpBit(u16 f) { dSv_event_onEventBit(dComIfGs_tmpEvent(), f); }
/* 02520C0C dComIfGs_checkGetItem(u8) */
static inline BOOL dComIfGs_checkGetItem(u8 item) { return gabi::call<BOOL>(0x02520C0C, item); }
/* HD: dComIfGs_getPictureNum() is a byte of the picture save data (02720144 returns it) */
static inline u8 dComIfGs_getPictureNum() { return gabi::load<u8>(gabi::call<u32>(0x02720144, dComIfGs_save() + 0x12C0) + 0x3C030C); }
static inline s16 dComIfGp_getStartStagePoint() { return gabi::load<s16>(dComIfGp_ea() + 0x513C); }
static inline u8 dComIfGp_getSelectItem(int btn) { return gabi::load<u8>(dComIfGp_ea() + btn + 0x5BBB); }
static inline u8 dComIfGp_event_runCheck() { return gabi::load<u8>(dComIfGp_ea() + 0x5292); }
static inline BOOL dComIfGp_event_chkTalkXY() { return (u32)(gabi::load<u8>(dComIfGp_ea() + 0x52B0) - 1) <= 3; }
static inline u8 dComIfGp_event_getPreItemNo() { return gabi::load<u8>(dComIfGp_ea() + 0x52B1); }
static inline u8 dComIfGp_getMesgAnimeAttrInfo() { return gabi::load<u8>(dComIfGp_ea() + PLAY_BGS + 0x4925); }
static inline u8 dComIfGp_getPictureResult() { return gabi::load<u8>(dComIfGp_ea() + 0x5BE6); }
static inline BOOL dComIfGp_evmng_ChkPresentEnd() { return gabi::call<BOOL>(0x02544950, dComIfGp_getPEvtManager()); }
static inline void* dComIfGp_evmng_getMyIntegerP(s32 staffId, const char* name) { return dComIfGp_evmng_getMySubstanceP(staffId, name, 3); }
static inline s8 dComIfGp_evmng_getMyActIdx(s32 staffId, u32 tbl, s32 n, BOOL force, s32 nameType) {
    return gabi::call<s8>(0x02542EDC, dComIfGp_getPEvtManager(), staffId, tbl, n, force, nameType);
}
/* resources by id (HD: sead::SafeString key, per-TU vtable) */
static inline void* dComIfG_getObjectIDRes(const char* arc, s32 id) {
    gabi::Local<SafeString> key;
    key->mStringTop = gabi::ea(arc);
    key->__vtbl = PHOTO_SAFESTRING_VTBL;
    return gabi::call<void*>(0x026067F4, dComIfG_resControl(), key.get(), id);
}
static inline s32 J3DAnm_getFrameMax(void* p) {
    u32 vt = gabi::load<u32>(gabi::ea(p) + 4);
    return gabi::call_ptr<s32>(gabi::load<u32>(vt + 0x14), p);
}
static inline s16 cLib_calcTimer(be<s16>* t) { return gabi::call<s16>(0x02055B64, t); }
static inline s32 mDoExt_btpAnm_init(void* anm, J3DModelData* d, void* p, s32 play, s32 attr, f32 rate, s32 start,
                                     s32 end, u32 modify, s32 entry) {
    return gabi::call<s32>(0x025E789C, anm, d, p, play, attr, rate, start, end, modify, entry);
}
static inline J3DModelData* J3DModel_getModelData_l(J3DModel* m) { return gabi::at<J3DModelData>(gabi::load<u32>(gabi::ea(m) + 0xAC)); }
static inline void dNpc_playerEyePos_l(cXyz* out, f32 offs) { gabi::call(0x0259D54C, out, offs); }
static inline void dNpc_JntCtrl_lookAtTarget(dNpc_JntCtrl_c* j, be<s16>* outY, cXyz* target, cXyz* eye, s16 yrot, s16 vel, u8 headOnly) {
    gabi::call(0x0259DED0, j, outY, target, eye, yrot, vel, headOnly);
}
/* 0259D624 dNpc_calc_DisXZ_AngY(cXyz, cXyz (pointers to copies), f32* dist, s16* angle) */
static inline void dNpc_calc_DisXZ_AngY(cXyz* a, cXyz* b, void* dist, void* ang) { gabi::call(0x0259D624, a, b, dist, ang); }
/* dNpc_PathRun_c */
static inline void dNpc_PathRun_getPoint(PhotoPathRun_l* r, cXyz* out, u8 idx) { gabi::call(0x0259E778, r, out, idx); }
static inline BOOL dNpc_PathRun_incIdxLoop(PhotoPathRun_l* r) { return gabi::call<BOOL>(0x0259EB60, r); }
static inline bool dNpc_PathRun_setInf(PhotoPathRun_l* r, u8 idx, s8 room, u8 fwd) { return gabi::call<bool>(0x0259E6D0, r, idx, room, fwd); }
static inline BOOL dNpc_PathRun_chkPointPass(PhotoPathRun_l* r, cXyz* pos, u32 dir) { return gabi::call<BOOL>(0x0259E838, r, pos, dir); }
static inline BOOL dNpc_PathRun_nextIdxAuto(PhotoPathRun_l* r) { return gabi::call<BOOL>(0x0259ED58, r); }
/* HD message manager (*(0x101F4B5C)) */
static inline u32 photo_msgManager() { return gabi::load<u32>(0x101F4B5C); }
static inline u32 msgMng_getStatus(u32 mgr) { return gabi::call<u32>(0x025F795C, mgr); }
static inline void msgMng_setStatus(u32 mgr, u32 st) { gabi::call(0x025F74D0, mgr, st); }
static inline u32 msgMng_messageSet(u32 mgr, u32 msgNo, cXyz* pos) { return gabi::call<u32>(0x025F7DB0, mgr, msgNo, pos); }
/* J3DModelData (HD) */
static inline u32 J3DModelData_getJointName(J3DModelData* d) {
    u32 h = gabi::call<u32>(0x027F68FC, d);
    u32 off = gabi::load<u32>(h + 0x10);
    return off != 0 ? h + 0x10 + off : 0;
}
static inline s8 JUTNameTab_getIndex(u32 tab, const char* name) { return gabi::call<s8>(0x027DF9B0, tab, name); }
static inline u16 J3DModelData_getJointNum_l(J3DModelData* d) { return gabi::load<u16>(gabi::call<u32>(0x027F3F94, d) + 8); }
static inline void J3DModelData_setJointCallBack_l(J3DModelData* d, u32 i, u32 cb) {
    u32 n = gabi::load<u32>(gabi::ea(d) + 4);
    u32 p = gabi::load<u32>(gabi::ea(d) + 8);
    if (i < n) {
        p += i * 0x1C;
    }
    gabi::store<u32>(p + 8, cb);
}
struct J3DMtxBlock_l {
    /* 0x00 */ u8 _00[4];
    /* 0x04 */ be<u16> mFlags;
    /* 0x06 */ u8 _06[0x10 - 6];
    /* 0x10 */ gptr<Mtx34> mpMtx;
};
static inline Mtx34* j3dSys_mCurrentMtx() { return gabi::at<Mtx34>(0x104B4868); }
/* eventInfo (fopAc_ac_c + 0xF8): mCommand u16 at 0xF8, mEventId s16 at 0xFC */
static inline u16 eventInfo_getCommand(fopAc_ac_c* a) { return gabi::load<u16>(gabi::ea(a) + 0xF8); }
static inline void eventInfo_setEventId(fopAc_ac_c* a, s16 id) { gabi::store<s16>(gabi::ea(a) + 0xFC, id); }
/* 025D9F38 fopAcM_searchFromName(name, param mask, param) */
static inline fopAc_ac_c* fopAcM_searchFromName(const char* name, u32 mask, u32 prm) { return gabi::call<fopAc_ac_c*>(0x025D9F38, name, mask, prm); }
/* 02554288 dKy_get_dayofweek */
static inline s32 dKy_get_dayofweek() { return gabi::call<s32>(0x02554288); }
/* dEvt_control_c (play + 0x51D0): mPtTalk at +0xCC; 0253F124 getPId */
static inline void dComIfGp_event_setTalkPartner(fopAc_ac_c* a) {
    u32 evt = dComIfGp_ea() + PLAY_EVTCTRL;
    gabi::store<u32>(evt + 0xCC, gabi::call<u32>(0x0253F124, evt, a));
}
static inline void dComIfGp_event_setItemPartnerId(u32 id) { gabi::store<u32>(dComIfGp_ea() + 0x52A0, id); }
