/* daNpcPeople_c (Windfall townspeople), WWHD layout, plus the translation-unit statics and
 * local bindings shared by the d_a_npc_people*.cpp parts.
 *
 * GameCube -> WWHD, measured from the constructor 022BE7CC (allocates 0x920) and the verified
 * functions: fopNpc_npc_c is 0x7DC (d/d_npc.h). Members +0x118 up to mBtpAnm (0x7FC);
 * mDoExt_btpAnm grew from 0x14 to 0x74 and mShadowId (GameCube 0x6F8) is gone (HD shadows), so
 * everything from mPathRun (GameCube 0x6FC) on is +0x174. Size 0x920 (GameCube 0x7A9). */
#pragma once
#include "bindings.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/f_op/f_op_actor_mng.h"
#include "gc_api/f_op/f_op_actor.h"

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
    NPC_UM2, NPC_UM3, NPC_SA1, NPC_SA2, NPC_SA3, NPC_SA4, NPC_SA5, NPC_UG1, NPC_UG2, NPC_NO_MAX,
};

static inline void* dComIfG_getObjectIDRes_r67(const char* arc, s32 id, u32 r6, u32 r7) {
    gabi::Local<SafeString> key;
    key->mStringTop = gabi::ea(arc);
    key->__vtbl = PEOPLE_SAFESTRING_VTBL;
    return gabi::call<void*>(0x026067F4, dComIfG_resControl(), key.get(), id, r6, r7);
}

static inline void people_set_r(int n, u32 v) { gabi::cpu->r[n] = v; }
static inline s32 mDoExt_btpAnm_init(void* anm, J3DModelData* d, void* p, s32 play, s32 attr, f32 rate, s32 start,
                                     s32 end, u32 modify, s32 entry) {
    return gabi::call<s32>(0x025E789C, anm, d, p, play, attr, rate, start, end, modify, entry);
}

struct PsoData_l {
    be<f32> field_0x00, field_0x04, field_0x08, field_0x0C, field_0x10;
    be<s16> field_0x14;
    be<u8> field_0x16, field_0x17, photoNo;
    u8 _19[3];
};
struct dSnap_Obj_l { u8 _00[0x34]; };
struct PeopleMainPig : fopAc_ac_c {
    u8 _3AC[0x750-0x3AC]; be<u8> m408;
    u8 _751[0x798-0x751]; cXyz m450;
};
WWHD_OFFSET(PeopleMainPig, m408, 0x750);
WWHD_OFFSET(PeopleMainPig, m450, 0x798);
struct PeopleMainModelData {
    u8 _00[0x38]; be<u32> mTexNoAnimator;
};
struct PeopleMainPicture { u8 _00[0x3C030C]; be<u8> mPictureNum; };
struct PeopleMainGameView { u8 _00[0x12C0]; u8 mPictureSave; };
struct PeopleMainAcchView { u8 _00[0x14]; be<u32> mPolyPassVtable; u8 _18[8]; be<u32> mGrpPassVtable; };
struct PeopleMainSafeStringVtable { u8 _00[0x14]; gfn assureTermination; };
inline dComIfG_inf_c* peopleMainGame() { return gabi::at<dComIfG_inf_c>(dComIfGp_ea()); }
inline PeopleMainGameView* peopleMainSave() { return gabi::at<PeopleMainGameView>(dComIfGs_ea()); }
inline J3DModel* peopleCurrentModel() { return gabi::object_at<gptr<J3DModel>>(0x104B462C); }
inline Mtx34* peopleCurrentMtx() { return &gabi::object_at<Mtx34>(0x104B4868); }
inline f32 peopleClipperFar() { return gabi::object_at<be<f32>>(0x1048D04C); }
inline PeopleMainModelData* peopleModelDataTail(J3DModelData* p) { return reinterpret_cast<PeopleMainModelData*>(p); }
inline PeopleMainAcchView* peopleAcchVtables(dBgS_ObjAcch* p) { return reinterpret_cast<PeopleMainAcchView*>(p); }
// HD: SafeString termination is virtual; the left operand is terminated twice.
inline bool people_SafeString_eq(SafeString* a, SafeString* b) {
    gabi::call_ptr(gabi::at<PeopleMainSafeStringVtable>(a->__vtbl)->assureTermination.get(), a);
    gabi::call_ptr(gabi::at<PeopleMainSafeStringVtable>(a->__vtbl)->assureTermination.get(), a);
    gabi::call_ptr(gabi::at<PeopleMainSafeStringVtable>(b->__vtbl)->assureTermination.get(), b);
    u32 pa=a->mStringTop, pb=b->mStringTop;
    if (pa==pb) return true;
    const be<u8>* left=gabi::at<be<u8>>(pa);
    const be<u8>* right=gabi::at<be<u8>>(pb);
    for (u32 n=0; n<0x40001; ++n) {
        u8 c=left[n];
        if (c!=right[n]) return false;
        if (c==0) return true;
    }
    return false;
}
inline be<u32> (&l_pig_para())[3] { return gabi::object_at<be<u32>[3]>(0x1002014C); }
inline sPeopleAnmDat (&l_npc_anm_talkH2())[1] { return gabi::object_at<sPeopleAnmDat[1]>(0x101C318C); }
inline sPeopleAnmDat (&l_npc_anm_wait())[1] { return gabi::object_at<sPeopleAnmDat[1]>(0x101C31B4); }
inline sPeopleAnmDat (&l_npc_anm_um3_wait3())[1] { return gabi::object_at<sPeopleAnmDat[1]>(0x101C31D8); }
inline gptr<PsoData_l> (&l_pso_tbl())[6] { return gabi::object_at<gptr<PsoData_l>[6]>(0x101C39B0); }
inline be<u8> (&l_npc_dist_tbl())[19][2] { return gabi::object_at<be<u8>[19][2]>(0x101C3E28); }
inline gptr<char> (&l_npc_staff_id())[19] { return gabi::object_at<gptr<char>[19]>(0x101C3E7C); }
inline gptr<char> (&l_arcname_tbl())[19] { return gabi::object_at<gptr<char>[19]>(0x101C3EC8); }
inline gptr<be<s32>> (&l_bck_ix_tbl())[19][2] { return gabi::object_at<gptr<be<s32>>[19][2]>(0x101C3F14); }
inline gptr<be<s32>> (&l_head_bck_ix_tbl())[19][2] { return gabi::object_at<gptr<be<s32>>[19][2]>(0x101C3FAC); }
inline gptr<sPeopleAnmDat> (&l_npc_anm_wait_tbl())[19][2] { return gabi::object_at<gptr<sPeopleAnmDat>[19][2]>(0x101C4044); }
inline gptr<daNpcPeople_c__l_npc_dat> (&l_npc_dat())[19][2] { return gabi::object_at<gptr<daNpcPeople_c__l_npc_dat>[19][2]>(0x101C4AD4); }
inline const char* people_arcname(u32 n) { return l_arcname_tbl()[n]; }
cPhs_State dComLbG_PhaseHandler(request_of_phase_process_class* p, u32 tbl, void* self);
void fopNpc_npc_c_ct(void* p);
void mDoExt_btpAnm_ct(void* p);
void dNpc_PathRun_getPoint(dNpc_PathRun_l* r, cXyz* out, u8 idx);
BOOL dNpc_PathRun_incIdxLoop(dNpc_PathRun_l* r);
void dSnap_Obj_ct(dSnap_Obj_l* o);
void dSnap_Obj_SetInf(dSnap_Obj_l* o, u8 photoNo, fopAc_ac_c* a, u8 b, u8 c, s16 cull);
void dSnap_Obj_SetGeo(dSnap_Obj_l* o, cXyz* c, f32 r, f32 h, s16 ang);
void dSnap_RegistSnapObj(dSnap_Obj_l* o);
s32 dKy_get_dayofweek();
bool dNpc_PathRun_setInf(dNpc_PathRun_l* r, u8 idx, s8 room, u8 fwd);
PeopleMainPig* fopAcM_searchFromName_l(const char* name, u32 mask, u32 prm);
PeopleMainPicture* peoplePictureData(void* save);
s8 peopleJointNameIndex(JUTNameTab* tab, const char* name);
void peopleCylinderDestructor(dCcD_Cyl* p, s32 flags);
void peopleSttsDestructor(dCcD_Stts* p, s32 flags);
void peopleCircleDestructor(cM3dGCir_l* p, s32 flags);
void peopleAcchDestructor(dBgS_ObjAcch* p, s32 flags);
void peopleActorDestructor(fopAc_ac_c* p, s32 flags);
void peopleHeaderObjectOneCtor(u8* p);
void peopleHeaderObjectTwoCtor(u8* p);
enum : u32 { PEOPLE_GRP_PASS_VTBL=0x1002012C, PEOPLE_POLY_PASS_VTBL=0x1002013C };
struct PeopleMainHeaderStatics { be<f32> mMin, mMax; u8 mObjectOne, mObjectTwo; u8 _0A[2]; be<u32> mZeros[4]; };
struct PeopleMainRegistration { u8 _00[12]; };
struct PeopleMainPhase { gfn fn; };

inline dCcD_SrcCyl* peopleCylinderSource() { return &gabi::object_at<dCcD_SrcCyl>(0x101EA190); }
enum : u32 { PEOPLE_RESOURCE_HIGH_HALF=0x10020000 };
WWHD_OFFSET(PeopleMainModelData, mTexNoAnimator, 0x38);
WWHD_OFFSET(PeopleMainPicture, mPictureNum, 0x3C030C);
WWHD_OFFSET(PeopleMainGameView, mPictureSave, 0x12C0);
WWHD_OFFSET(PeopleMainAcchView, mPolyPassVtable, 0x14);
WWHD_OFFSET(PeopleMainAcchView, mGrpPassVtable, 0x20);
WWHD_OFFSET(PeopleMainSafeStringVtable, assureTermination, 0x14);
WWHD_OFFSET(PeopleMainHeaderStatics, mObjectOne, 0x08);
WWHD_OFFSET(PeopleMainHeaderStatics, mObjectTwo, 0x09);
WWHD_OFFSET(PeopleMainHeaderStatics, mZeros, 0x0C);
WWHD_SIZE(PeopleMainHeaderStatics, 0x1C);
WWHD_SIZE(PeopleMainRegistration, 12);
WWHD_SIZE(PeopleMainPhase, 4);
WWHD_SIZE(dSnap_Obj_l, 0x34);
WWHD_SIZE(PsoData_l, 0x1C);
enum PeopleItemNo_e {
    dItemNo_JOY_PENDANT_e=0x1F, dItemNo_PICTO_BOX_e=0x23,
    dItemNo_DELUXE_PICTO_BOX_e=0x26, dItemNo_SKULL_NECKLACE_e=0x45,
    dItemNo_COLLECT_MAP_20_e=0xEB, dItemNo_COLLECT_MAP_15_e=0xF0,
};
