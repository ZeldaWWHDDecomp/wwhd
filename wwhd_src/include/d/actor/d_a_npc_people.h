/* daNpcPeople_c (Windfall townspeople), WWHD layout, plus the translation-unit statics and
 * local bindings shared by the d_a_npc_people*.cpp parts.
 *
 * GameCube -> WWHD, measured from the constructor 022BE7CC (allocates 0x920) and the verified
 * functions: fopNpc_npc_c is 0x7DC (d/d_npc.h). Members +0x118 up to mBtpAnm (0x7FC);
 * mDoExt_btpAnm grew from 0x14 to 0x74 and mShadowId (GameCube 0x6F8) is gone (HD shadows), so
 * everything from mPathRun (GameCube 0x6FC) on is +0x174. Size 0x920 (GameCube 0x7A9). */
#pragma once
#include "bindings.h"

#define PEOPLE_SAFESTRING_VTBL 0x10020114 /* this TU's sead::SafeString vtable */
#define PEOPLE_VTBL 0x100209C4            /* daNpcPeople_c vtable (constructor) */

struct sUbMsgDat {
    /* 0x00 */ be<u32> field_0x00;
    /* 0x04 */ be<u8> field_0x04;
    /* 0x05 */ be<u8> field_0x05;
    /* 0x06 */ be<u8> field_0x06;
    /* 0x07 */ be<u8> field_0x07;
};
WWHD_SIZE(sUbMsgDat, 8);

struct sPeopleAnmDat {
    /* 0x00 */ be<u8> field_0x00;
    /* 0x01 */ be<u8> field_0x01;
    /* 0x02 */ be<u8> field_0x02;
};
WWHD_SIZE(sPeopleAnmDat, 3);

struct daNpcPeople_c__l_npc_dat {
    /* 0x00 */ be<s16> field_0x00;
    /* 0x02 */ be<s16> field_0x02;
    /* 0x04 */ be<s16> field_0x04;
    /* 0x06 */ be<s16> field_0x06;
    /* 0x08 */ be<s16> field_0x08;
    /* 0x0A */ be<s16> field_0x0A;
    /* 0x0C */ be<s16> field_0x0C;
    /* 0x0E */ be<s16> field_0x0E;
    /* 0x10 */ be<s16> field_0x10;
    /* 0x12 */ be<s16> field_0x12;
    /* 0x14 */ be<f32> field_0x14;
    /* 0x18 */ be<f32> field_0x18;
    /* 0x1C */ be<f32> field_0x1C;
    /* 0x20 */ be<f32> field_0x20;
    /* 0x24 */ be<f32> field_0x24;
    /* 0x28 */ be<f32> field_0x28;
    /* 0x2C */ be<f32> field_0x2C;
    /* 0x30 */ be<f32> field_0x30;
    /* 0x34 */ be<s16> field_0x34;
    /* 0x36 */ be<s16> field_0x36;
    /* 0x38 */ be<s16> field_0x38;
    /* 0x3A */ be<s16> field_0x3A;
    /* 0x3C */ be<f32> field_0x3C;
    /* 0x40 */ be<f32> field_0x40;
    /* 0x44 */ be<f32> field_0x44;
    /* 0x48 */ be<f32> field_0x48;
    /* 0x4C */ be<s16> field_0x4C;
    /* 0x4E */ be<s16> field_0x4E;
    /* 0x50 */ be<s16> field_0x50;
    /* 0x52 */ be<s16> field_0x52;
    /* 0x54 */ be<s16> field_0x54;
    /* 0x56 */ be<s16> field_0x56;
    /* 0x58 */ be<s16> field_0x58;
    /* 0x5A */ be<u8> field_0x5A;
    /* 0x5B */ be<u8> field_0x5B;
    /* 0x5C */ be<u8> field_0x5C;
    /* 0x5D */ be<u8> field_0x5D;
    /* 0x5E */ be<u8> field_0x5E;
    /* 0x5F */ be<u8> field_0x5F;
};
WWHD_SIZE(daNpcPeople_c__l_npc_dat, 0x60);

/* dNpc_PathRun_c (8 bytes, unchanged) */
struct dNpc_PathRun_l {
    /* 0x00 */ gptr<dPath> mPath;
    /* 0x04 */ be<u8> field_0x04;
    /* 0x05 */ be<u8> mIdx;
    /* 0x06 */ be<u8> mbDir;
    /* 0x07 */ be<u8> field_0x07;
};
WWHD_SIZE(dNpc_PathRun_l, 8);

/* GHS pointer to member function (8 bytes): this adjustment, virtual index (0: null, < 0: not
 * virtual), then the function address, or (virtual) the vtable pointer's offset at +6
 * (same as ProcFunc_l in d_a_npc_ba1.h) */
struct PeoplePmf_l {
    /* 0x0 */ be<s16> d;
    /* 0x2 */ be<s16> i;
    /* 0x4 */ be<u32> f;
};
WWHD_SIZE(PeoplePmf_l, 8);

struct daNpcPeople_c : fopNpc_npc_c {
    enum Prm_e {
        PRM_RAIL_ID_W = 0x8,
        PRM_RAIL_ID_S = 0x10,
        PRM_ARG0_W = 0x8,
        PRM_ARG0_S = 0x0,
    };

    u8 getNpcNo() { return mNpcNo; }
    request_of_phase_process_class* getPhaseP() { return &mPhase; }
    void setEtcFlag(u32 flags) { mEtcFlag = mEtcFlag | flags; }
    void setResFlag(u8 flag) { mResFlag = flag; }
    void setAnmFlag(u8 flags) { mAnmFlag = mAnmFlag | flags; }
    void setTalk(u8 value) { mTalk = value; }
    void setNoTalk(u8 value) { mNoTalk = value; }
    void setOrderEventNum(u8 value) { mOrderEventNum = value; }

    /* the declarations follow the GameCube header; a part may adjust the return type of its
     * own functions to what the WWHD code returns */
    cPhs_State _create();
    BOOL createHeap();
    cPhs_State createInit();
    bool _delete();
    bool _draw();
    bool _execute();
    bool executeCommon();
    void executeSetMode(u32); /* u8, not re-extended */
    s32 executeWaitInit();
    void executeWait();
    s32 executeTalkInit();
    void executeTalk();
    s32 executeWalkInit();
    void executeWalk();
    s32 executeTurnInit();
    void executeTurn();
    s32 executeBikkuriInit();
    void executeBikkuri();
    s32 executeFurueInit();
    void executeFurue();
    s32 executeKyoroInit();
    void executeKyoro();
    s32 executeLetterInit();
    void executeLetter();
    s32 executeLookInit();
    void executeLook();
    s32 executeLook2Init();
    void executeLook2();
    s32 executeUgWalkInit();
    void executeUgWalk();
    s32 executeUgTurnInit();
    void executeUgTurn();
    s32 executeUgLookInit();
    void executeUgLook();
    s32 executeUgLook2Init();
    void executeUgLook2();
    s32 executeUgSitInit();
    void executeUgSit();
    void checkOrder();
    void eventOrder();
    void eventMove();
    void privateCut();
    void eventMesSetTpInit(int);
    void eventMesSetInit(int);
    bool eventMesSet();
    bool eventMesSet2();
    void eventFlagSetInit(int);
    void eventGetItemInit(int);
    bool eventGetItem();
    void eventTurnToPlayerInit();
    bool eventTurnToPlayer();
    void eventUb1TalkInit(int);
    bool eventUb1Talk();
    void eventUb1TalkXyInit(int);
    bool eventUb1TalkXy();
    bool eventUb2Talk();
    bool eventUbSetAnm();
    void eventAreaMaxInit();
    void eventCameraStopInit();
    bool eventCameraStop();
    void eventCameraStartInit();
    void eventCoCylRInit(int);
    bool eventLookPo();
    void eventMesSetPoInit(int);
    bool eventMesSetPo();
    u16 talk2(int, fopAc_ac_c*);
    u16 talk3(int);
    u16 next_msgStatus(be<u32>*);
    u32 getMsg();
    u32 getMsg3();
    void chkMsg();
    void setMessage(u32);
    void setMessageUb(sUbMsgDat*);
    void setAnmFromMsgTag();
    u32 setAnmFromMsgTagUo(int);
    u32 setAnmFromMsgTagUb(int);
    u32 setAnmFromMsgTagUw(int);
    u32 setAnmFromMsgTagUm(int);
    u32 setAnmFromMsgTagSa(int);
    u32 setAnmFromMsgTagUg(int);
    u8 getPrmNpcNo();
    u8 getPrmRailID();
    u8 getPrmArg0();
    void setMtx();
    void chkAttention();
    void lookBack();
    BOOL initTexPatternAnm(u32); /* bool, passed on unnormalised */
    void playTexPatternAnm();
    void playAnm();
    int getBck(int); /* HD: the u8 index is not re-extended (full register) */
    int getHeadBck(int);
    void setAnm(u32, int, f32, f32); /* u8 index, full register */
    bool setAnmTbl(sPeopleAnmDat*, int);
    void setWaitAnm();
    s16 XyCheckCB(int);
    s16 XyEventCB(int);
    s16 photoCB(int);
    int getRand(int max);
    BOOL isPhoto(u8);
    BOOL isColor();
    void setCollision(dCcD_Cyl*, cXyz*, f32, f32); /* cXyz by value: pointer to a copy */
    BOOL chkSurprise();
    BOOL chkEndEvent();
    BOOL is1GetMap20();
    BOOL is1DayGetMap20();
    int getWindDir();
    BOOL isUo1FdaiAll();
    BOOL isUo1FdaiOne();
    s32 chkDaiza();
    BOOL checkPig();
    BOOL isPigOk();
    s16 getPigTimer();
    void resetPig();
    void initUgSearchArea();
    void getDirDistToPos(cXyz* result, s16, f32); /* cXyz returned through a hidden pointer */
    void warp();

    /* 0x7DC */ request_of_phase_process_class mPhase;
    /* 0x7E4 */ request_of_phase_process_class mPhase2;
    /* 0x7EC */ gptr<J3DModel> mpHeadModel;
    /* 0x7F0 */ gptr<J3DModel> mpEtcModel;
    /* 0x7F4 */ gptr<mDoExt_McaMorf> mpHeadMorf;
    /* 0x7F8 */ gptr<J3DAnmTexPattern> m_head_tex_pattern;
    /* 0x7FC */ u8 mBtpAnm[0x74];                  /* mDoExt_btpAnm (HD 0x74, constructor 025E7820) */
    /* 0x870 */ dNpc_PathRun_l mPathRun;           /* HD: no mShadowId before it */
    /* 0x878 */ u8 m704[0x884 - 0x878];
    /* 0x884 */ cXyz mLookAtPos;
    /* 0x890 */ cXyz m71C;
    /* 0x89C */ gptr<sPeopleAnmDat> m728;
    /* 0x8A0 */ gptr<daNpcPeople_c__l_npc_dat> mpNpcDat;
    /* 0x8A4 */ gptr<daNpcPeople_c> m730;
    /* 0x8A8 */ gptr<be<u32>> m734;
    /* 0x8AC */ gptr<sUbMsgDat> m738;
    /* 0x8B0 */ gptr<gptr<sUbMsgDat>> m73C;
    /* 0x8B4 */ be<f32> m740;
    /* 0x8B8 */ be<f32> m744;
    /* 0x8BC */ be<f32> m748;
    /* 0x8C0 */ be<f32> m74C;
    /* 0x8C4 */ be<f32> m750;
    /* 0x8C8 */ u8 m754[0x8CC - 0x8C8];
    /* 0x8CC */ be<u32> mEtcFlag;
    /* 0x8D0 */ be<s32> m75C;
    /* 0x8D4 */ be<s32> m760;
    /* 0x8D8 */ be<u8> m764;                       /* bool */
    /* 0x8D9 */ be<u8> m765;
    /* 0x8DA */ be<s16> m766[4];
    /* 0x8E2 */ be<s16> m76E;
    /* 0x8E4 */ be<s16> m770;
    /* 0x8E6 */ be<s16> m772;
    /* 0x8E8 */ be<s16> m774;
    /* 0x8EA */ be<s16> m776;
    /* 0x8EC */ be<s16> m778;
    /* 0x8EE */ be<s16> m77A;
    /* 0x8F0 */ be<u16> m77C;
    /* 0x8F2 */ be<u16> m77E;
    /* 0x8F4 */ be<s16> m780;
    /* 0x8F6 */ be<s16> m782;
    /* 0x8F8 */ be<s16> m784;
    /* 0x8FA */ be<s16> m786;
    /* 0x8FC */ be<u8> mTalk;
    /* 0x8FD */ be<u8> m789;
    /* 0x8FE */ be<u8> m78A;
    /* 0x8FF */ be<u8> m78B;
    /* 0x900 */ be<u8> mNoTalk;
    /* 0x901 */ be<u8> mOrderEventNum;
    /* 0x902 */ be<u8> m78E;
    /* 0x903 */ be<u8> m78F;
    /* 0x904 */ be<u8> mResFlag;
    /* 0x905 */ be<u8> mNpcNo;
    /* 0x906 */ be<u8> m792;
    /* 0x907 */ be<u8> m793;
    /* 0x908 */ be<u8> m794;
    /* 0x909 */ be<u8> mAnmFlag;
    /* 0x90A */ be<s8> m796;
    /* 0x90B */ be<s8> m797;
    /* 0x90C */ be<u8> m798;
    /* 0x90D */ be<s8> m799;
    /* 0x90E */ be<u8> m79A;
    /* 0x90F */ be<u8> mbIsNight;
    /* 0x910 */ be<u8> m79C;
    /* 0x911 */ be<u8> m79D;
    /* 0x912 */ be<u8> m79E;
    /* 0x913 */ be<u8> m79F;
    /* 0x914 */ be<u8> m7A0;
    /* 0x915 */ be<u8> m7A1;
    /* 0x916 */ be<u8> m7A2;
    /* 0x917 */ be<s8> m7A3;
    /* 0x918 */ be<u8> m7A4;
    /* 0x919 */ be<u8> m7A5;
    /* 0x91A */ be<u8> m7A6;
    /* 0x91B */ be<u8> m7A7;
    /* 0x91C */ be<u8> m7A8;
    /* 0x91D */ u8 _91D[3];
};
WWHD_OFFSET(daNpcPeople_c, mPhase, 0x7DC);
WWHD_OFFSET(daNpcPeople_c, mBtpAnm, 0x7FC);
WWHD_OFFSET(daNpcPeople_c, mPathRun, 0x870);
WWHD_OFFSET(daNpcPeople_c, mpNpcDat, 0x8A0);
WWHD_OFFSET(daNpcPeople_c, mEtcFlag, 0x8CC);
WWHD_OFFSET(daNpcPeople_c, m77A, 0x8EE);
WWHD_OFFSET(daNpcPeople_c, mNpcNo, 0x905);
WWHD_OFFSET(daNpcPeople_c, mbIsNight, 0x90F);
WWHD_OFFSET(daNpcPeople_c, m7A8, 0x91C);
WWHD_SIZE(daNpcPeople_c, 0x920);

enum NpcNo_e {
    NPC_UO1, NPC_UO2, NPC_UO3, NPC_UB1, NPC_UB2, NPC_UB3, NPC_UB4, NPC_UW1, NPC_UW2, NPC_UM1,
    NPC_UM2, NPC_UM3, NPC_SA1, NPC_SA2, NPC_SA3, NPC_SA4, NPC_SA5, NPC_UG1, NPC_UG2,
};

/* ---- translation-unit statics (.data / .rodata / .bss addresses) ---- */
enum : u32 {
    PEOPLE_l_arcname_tbl = 0x101C3EC8,     /* const char*[19] */
    PEOPLE_l_bck_ix_tbl = 0x101C3F14,      /* int*[19][2] */
    PEOPLE_l_head_bck_ix_tbl = 0x101C3FAC, /* int*[19][2] */
    PEOPLE_l_npc_dat = 0x101C4AD4,         /* l_npc_dat*[19][2] */
    PEOPLE_l_method = 0x101C4C5C,          /* _create's phase handler table */
};
static inline const char* people_arcname(u32 npcNo) { return gabi::at<const char>(gabi::load<u32>(PEOPLE_l_arcname_tbl + npcNo * 4)); }

/* ---- local bindings (SHARED-CANDIDATE; most are the same as in d_a_npc_ba1.cpp) ---- */
/* save info: event flags (dSv_event_c) at *(0x101F84DC) + 0x644, temporary flags at + 0x1178;
 * the pointer is re-read at every use */
static inline u32 dComIfGs_save() { return gabi::load<u32>(0x101F84DC); }
static inline dSv_event_c* dComIfGs_event() { return gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x644); }
static inline BOOL dComIfGs_isEventBit(u16 f) { return dSv_event_isEventBit(dComIfGs_event(), f); }
static inline void dComIfGs_onEventBit(u16 f) { dSv_event_onEventBit(dComIfGs_event(), f); }
static inline u8 dComIfGs_getEventReg(u16 reg) { return dSv_event_getEventReg(dComIfGs_event(), reg); }
static inline void dComIfGs_setEventReg(u16 reg, u8 v) { dSv_event_setEventReg(dComIfGs_event(), reg, v); }
static inline dSv_event_c* dComIfGs_tmpEvent() { return gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x1178); }
static inline BOOL dComIfGs_isTmpBit(u16 f) { return dSv_event_isEventBit(dComIfGs_tmpEvent(), f); }
static inline void dComIfGs_onTmpBit(u16 f) { dSv_event_onEventBit(dComIfGs_tmpEvent(), f); }
static inline u8 dComIfGs_getTmpReg(u16 reg) { return dSv_event_getEventReg(dComIfGs_tmpEvent(), reg); }
static inline void dComIfGs_setTmpReg(u16 reg, u8 v) { dSv_event_setEventReg(dComIfGs_tmpEvent(), reg, v); }
static inline u8 dComIfGs_checkCollect(int i) { return gabi::load<u8>(gabi::load<u32>(0x101F84DC) + 0xD4 + i); }
/* play object fields (dComIfGp_get() at each use) */
static inline u8 dComIfGp_getSelectItem(int btn) { return gabi::load<u8>(dComIfGp_ea() + btn + 0x5BBB); }
static inline u8 dComIfGp_event_runCheck() { return gabi::load<u8>(dComIfGp_ea() + 0x5292); }
static inline u8 dComIfGp_event_getTalkXYBtn() { return gabi::load<u8>(dComIfGp_ea() + 0x52B0); }
static inline BOOL dComIfGp_event_chkTalkXY() { return (u32)(dComIfGp_event_getTalkXYBtn() - 1) <= 3; }
static inline u8 dComIfGp_event_getPreItemNo() { return gabi::load<u8>(dComIfGp_ea() + 0x52B1); }
static inline u8 dComIfGp_getMesgAnimeAttrInfo() { return gabi::load<u8>(dComIfGp_ea() + PLAY_BGS + 0x4925); }
/* resources by id (HD: sead::SafeString key, per-TU vtable) */
static inline void* dComIfG_getObjectIDRes(const char* arc, s32 id) {
    gabi::Local<SafeString> key;
    key->mStringTop = gabi::ea(arc);
    key->__vtbl = PEOPLE_SAFESTRING_VTBL;
    return gabi::call<void*>(0x026067F4, dComIfG_resControl(), key.get(), id);
}
/* the same, with the values GHS leaves in r6/r7: the harness counts r3..r7 as read by getIDRes
 * (through its SafeString vtable calls), so a call site whose original sets them passes them */
static inline void* dComIfG_getObjectIDRes_r67(const char* arc, s32 id, u32 r6, u32 r7) {
    gabi::Local<SafeString> key;
    key->mStringTop = gabi::ea(arc);
    key->__vtbl = PEOPLE_SAFESTRING_VTBL;
    return gabi::call<void*>(0x026067F4, dComIfG_resControl(), key.get(), id, r6, r7);
}
/* set one argument register before a guest call that leaves it alone (for a register the harness
 * compares at a call, e.g. r6/r7 of getIDRes from its GameCube signature, whose value the
 * original happens to set) */
static inline void people_set_r(int n, u32 v) { gabi::cpu->r[n] = v; }
/* J3DAnmTexPattern::getFrameMax: virtual (vtable pointer at +4, slot +0x14) */
static inline s32 J3DAnm_getFrameMax(void* p) {
    u32 vt = gabi::load<u32>(gabi::ea(p) + 4);
    return gabi::call_ptr<s32>(gabi::load<u32>(vt + 0x14), p);
}
/* 02055B64 cLib_calcTimer<s16> (out of line) */
static inline s16 cLib_calcTimer(be<s16>* t) { return gabi::call<s16>(0x02055B64, t); }
/* 025E789C mDoExt_btpAnm::init(modelData, anm, anmPlay, attr, rate, start, end, modify, entry (stack)) */
static inline s32 mDoExt_btpAnm_init(void* anm, J3DModelData* d, void* p, s32 play, s32 attr, f32 rate, s32 start,
                                     s32 end, u32 modify, s32 entry) {
    return gabi::call<s32>(0x025E789C, anm, d, p, play, attr, rate, start, end, modify, entry);
}
static inline J3DModelData* J3DModel_getModelData_l(J3DModel* m) { return gabi::at<J3DModelData>(gabi::load<u32>(gabi::ea(m) + 0xAC)); }
/* dAttention_c (play + 0x5804): 024EC8D0 is LockonTarget, 024EE464 ActionTarget (matcher swapped) */
static inline bool dAttention_LockonTruth(dAttention_c* a) { return gabi::call<bool>(0x024EDFCC, a); }
static inline fopAc_ac_c* dAttention_LockonTarget(dAttention_c* a, s32 i) { return gabi::call<fopAc_ac_c*>(0x024EC8D0, a, i); }
static inline fopAc_ac_c* dAttention_ActionTarget(dAttention_c* a, s32 i) { return gabi::call<fopAc_ac_c*>(0x024EE464, a, i); }
/* event manager (play + 0x52C4) */
static inline void* dComIfGp_evmng_getMyIntegerP(s32 staffId, const char* name) { return dComIfGp_evmng_getMySubstanceP(staffId, name, 3); }
static inline void* dComIfGp_evmng_getMyFloatP(s32 staffId, const char* name) { return dComIfGp_evmng_getMySubstanceP(staffId, name, 0); }
static inline void* dComIfGp_evmng_getMyStringP(s32 staffId, const char* name) { return dComIfGp_evmng_getMySubstanceP(staffId, name, 4); }
static inline void dComIfGp_evmng_setGoal(cXyz* pos) { gabi::call(0x02543714, dComIfGp_getPEvtManager(), pos); }
static inline s8 dComIfGp_evmng_getMyActIdx(s32 staffId, u32 tbl, s32 n, BOOL force, s32 nameType) {
    return gabi::call<s8>(0x02542EDC, dComIfGp_getPEvtManager(), staffId, tbl, n, force, nameType);
}
/* 02544950 dEvent_manager_c::ChkPresentEnd */
static inline BOOL dComIfGp_evmng_ChkPresentEnd() { return gabi::call<BOOL>(0x02544950, dComIfGp_getPEvtManager()); }
/* 0259D54C dNpc_playerEyePos(f32): cXyz through a hidden result pointer (r3) */
static inline void dNpc_playerEyePos_l(cXyz* out, f32 offs) { gabi::call(0x0259D54C, out, offs); }
/* 0259DED0 dNpc_JntCtrl_c::lookAtTarget(s16* outY, cXyz* target, cXyz eye (pointer to a copy), s16 yrot, s16 vel, bool headOnly) */
static inline void dNpc_JntCtrl_lookAtTarget(dNpc_JntCtrl_c* j, be<s16>* outY, cXyz* target, cXyz* eye, s16 yrot, s16 vel, u8 headOnly) {
    gabi::call(0x0259DED0, j, outY, target, eye, yrot, vel, headOnly);
}
/* HD message manager (*(0x101F4B5C)): 025F795C returns the current message's status */
static inline u32 people_msgManager() { return gabi::load<u32>(0x101F4B5C); }
static inline u32 fopMsgM_getStatus() { return gabi::call<u32>(0x025F795C, gabi::load<u32>(0x101F4B5C)); }
/* J3DModelData (HD) joint names: 027F68FC returns the joint name table header */
static inline u32 J3DModelData_getJointName(J3DModelData* d) {
    u32 h = gabi::call<u32>(0x027F68FC, d);
    u32 off = gabi::load<u32>(h + 0x10);
    return off != 0 ? h + 0x10 + off : 0;
}
static inline s8 JUTNameTab_getIndex(u32 tab, const char* name) { return gabi::call<s8>(0x027DF9B0, tab, name); }
/* dBgS (play + 0x12A0) */
static inline s8 dBgS_GetRoomId(dBgS* bgs, void* poly) { return gabi::call<s8>(0x024EF130, bgs, poly); }
static inline u8 dBgS_GetPolyColor(dBgS* bgs, void* poly) { return gabi::call<u8>(0x024EEEB8, bgs, poly); }

/* HD J3D joint matrices: J3DModel +0x2C is the matrix block (flags u16 at +4, matrices at +0x10) */
struct J3DMtxBlock_l {
    /* 0x00 */ u8 _00[4];
    /* 0x04 */ be<u16> mFlags;
    /* 0x06 */ u8 _06[0x10 - 6];
    /* 0x10 */ gptr<Mtx34> mpMtx;
};
static inline Mtx34* j3dSys_mCurrentMtx() { return gabi::at<Mtx34>(0x104B4868); }

/* GHS pointer-to-member call: (self->*pmf)(arg) */
static inline void people_pmf_call(void* self, PeoplePmf_l* pmf, s32 arg) {
    s16 i = pmf->i;
    u32 thisp = gabi::ea(self) + (s32)(s16)pmf->d;
    if (i < 0) {
        gabi::call_ptr(pmf->f, thisp, arg);
    } else {
        s16 voff = gabi::load<s16>(gabi::ea(pmf) + 6);
        u32 vt = gabi::load<u32>(thisp + voff);
        gabi::call_ptr(gabi::load<u32>(vt + i * 8 + 4), thisp, arg);
    }
}
