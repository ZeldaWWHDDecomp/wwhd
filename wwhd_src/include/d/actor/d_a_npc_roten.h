/* daNpcRoten_c (Traveling Merchants), WWHD layout. 
 *
 * GameCube -> WWHD, measured from the allocating constructor and the functions of d_a_npc_roten:
 * - fopNpc_npc_c is 0x7DC (d/d_npc.h), so members are +0x118 up to mBtpAnm;
 * - mDoExt_btpAnm grew from 0x14 to 0x74 (+0x60) and mShadowId (GameCube 0x6F4) is gone (HD
 *   shadows, -4): everything from field_0x6F8 on is +0x174.
 * Size 0xB40 (GameCube 0x9CC). */
#pragma once
#include "gc_api/gc_api.h"

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

// Merchant tuning data, three entries.
struct NpcDatStruct {
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
WWHD_SIZE(NpcDatStruct, 0x54);

struct daNpcRoten_c : fopNpc_npc_c {
    enum TradeItem {
        dItemNo_TOWN_FLOWER_e = 0x8C,
        dItemNo_SHOP_GURU_STATUE_e = 0x97,
        dItemNo_XXX_039_e = 0xA2
    };
    enum ItemButton { dItemBtn_X_e, dItemBtn_Y_e, dItemBtn_Z_e, dItemBtn_NONE_e };
    enum TalkButton { dTalkBtn_X_e = 1, dTalkBtn_Y_e, dTalkBtn_Z_e };
    enum Mode { MODE_WAIT, MODE_TALK, MODE_WALK, MODE_TURN, MODE_WIND };
    enum Animation {
        ANM_WAIT,
        ANM_TALK,
        ANM_WALK,
        ANM_TALK2,
        ANM_TALK3,
        ANM_TALK4,
        ANM_TALK5,
        ANM_WATASU,
        ANM_ITEMWAIT,
        ANM_WIND
    };
    enum Cut {
        CUT_INIT,
        CUT_MES_SET,
        CUT_SET_ITEM,
        CUT_CLR_ITEM,
        CUT_GET_ITEM,
        CUT_SET_ANGLE,
        CUT_ON_PLR,
        CUT_OFF_PLR
    };
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
    /* 0x7F8 */ mDoExt_btpAnm mBtpAnm; // HD: the texture animator grows; mShadowId is absent.
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

void roten_constructNpc(daNpcRoten_c* actor);
void roten_constructSphere(dCcD_Sph* sphere);
s32 roten_initBtp(mDoExt_btpAnm* animator, J3DModelData* model, J3DAnmTexPattern* animation,
                  s32 play, s32 mode, s32 start, s32 end, u32 modify, s32 entry, f32 speed);
s8 roten_getJointIndex(u32 table, const char* name);
cPhs_State roten_phaseHandler(request_of_phase_process_class* phase, void* handlers,
                              daNpcRoten_c* actor);
void roten_destroySphere(dCcD_Sph* sphere, s32 flags);
void roten_destroyCircle(void* circle, s32 flags);
void roten_destroyAcch(dBgS_ObjAcch* acch, s32 flags);
void roten_destroyActor(fopAc_ac_c* actor, s32 flags);
u32 roten_createPresentItem(cXyz* position, u32 itemNo, u32 flags, s32 bitNo, s32 room, u32 angle,
                            u32 scale);
s8 roten_getActionIndex(void* manager, s32 staff, u32 names, s32 count, BOOL force, s32 nameType);

struct RotenInitTable {
    gabi::ptmf<s32 (daNpcRoten_c::*)()> entries[5];
};
struct RotenMoveTable {
    gabi::ptmf<void (daNpcRoten_c::*)()> entries[5];
};
WWHD_SIZE(RotenInitTable, 40);
WWHD_SIZE(RotenMoveTable, 40);
extern RotenInitTable& l_execute_init;
extern RotenMoveTable& moveProc;
struct RotenCutNames {
    gptr<char> entries[8];
};
WWHD_SIZE(RotenCutNames, 32);
extern RotenCutNames& cut_name_tbl;
struct RotenPhaseHandlers {
    be<u32> phase1, phase2, end;
};
WWHD_SIZE(RotenPhaseHandlers, 12);
struct RotenAcchVtables {
    u8 _00[0x14];
    be<u32> dBgS_Chk_vtable;
    u8 _18[8];
    be<u32> cBgS_Chk_vtable;
};
WWHD_OFFSET(RotenAcchVtables, dBgS_Chk_vtable, 0x14);
WWHD_OFFSET(RotenAcchVtables, cBgS_Chk_vtable, 0x20);
WWHD_SIZE(RotenAcchVtables, 0x24);
struct RotenActorVtable {
    u8 _00[0x1C];
    be<u32> getMsg;
};
WWHD_OFFSET(RotenActorVtable, getMsg, 0x1C);
WWHD_SIZE(RotenActorVtable, 0x20);
struct RotenAnimationVtable {
    u8 _00[0x14];
    be<u32> getFrameMax;
};
WWHD_OFFSET(RotenAnimationVtable, getFrameMax, 0x14);
WWHD_SIZE(RotenAnimationVtable, 0x18);
struct RotenAnimationView {
    u8 _00[4];
    gptr<RotenAnimationVtable> vtable;
};
WWHD_OFFSET(RotenAnimationView, vtable, 4);
WWHD_SIZE(RotenAnimationView, 8);

// Private exact-width HD bindings.

static constexpr u32 l_actorVtable = 0x10021614;

// HD: both source parts refer to these single mapped table definitions in the main source.
struct SaveDatStruct {
    be<u16> field_0x00, field_0x02, field_0x04, field_0x06;
};
WWHD_SIZE(SaveDatStruct, 8);
struct RotenStringTable {
    gptr<const char> entries[3];
};
struct RotenNpcTable {
    NpcDatStruct entries[3];
};
struct RotenIndexTable {
    be<s32> entries[3];
};
struct RotenAnimationTable {
    be<s32> entries[3][10];
};
struct RotenSaveTable {
    SaveDatStruct entries[3];
};
struct RotenItemTable {
    be<u8> entries[3][12];
};
struct RotenWindAnmTable {
    sRotenAnmDat entries[3];
};
WWHD_SIZE(RotenStringTable, 12);
WWHD_SIZE(RotenNpcTable, 0xFC);
WWHD_SIZE(RotenIndexTable, 12);
WWHD_SIZE(RotenAnimationTable, 120);
WWHD_SIZE(RotenSaveTable, 24);
WWHD_SIZE(RotenItemTable, 36);
WWHD_SIZE(RotenWindAnmTable, 9);
extern RotenStringTable& l_arcname_tbl;
extern RotenStringTable& l_npc_staff_id;
extern RotenNpcTable& l_npc_dat;
extern RotenIndexTable& l_bmd_ix_tbl;
extern RotenIndexTable& l_head_bmd_ix_tbl;
extern RotenIndexTable& l_head_bck_ix_tbl;
extern RotenIndexTable& l_btp_ix_tbl;
extern RotenAnimationTable& l_bck_ix_tbl;
extern RotenSaveTable& l_save_dat;
extern RotenItemTable& l_item_dat;
extern sRotenAnmDat& l_npc_anm_wait;
extern sRotenAnmDat& l_npc_anm_walk;
extern RotenWindAnmTable& l_npc_anm_wind;
extern dCcD_SrcSph& l_sph_src;
extern dCcD_SrcCyl& dNpc_cyl_src;
static inline NpcDatStruct& npc_dat(u32 no) { return l_npc_dat.entries[no]; }
static inline const char* arcname(u32 no) { return l_arcname_tbl.entries[no].get(); }
static inline u16 save_dat(u32 no, u32 field) {
    const auto& dat = l_save_dat.entries[no];
    switch (field) {
    case 0:
        return dat.field_0x00;
    case 2:
        return dat.field_0x02;
    case 4:
        return dat.field_0x04;
    default:
        return dat.field_0x06;
    }
}
static inline u8 item_dat(u32 no, u32 idx) { return l_item_dat.entries[no][idx]; }

/* play object fields */
// HD: keep the event mode byte unnormalized.
static inline u8 roten_dComIfGp_event_runCheck() {
    return gabi::at<dComIfG_inf_c>(dComIfGp_ea())->play.mEvtCtrl.mMode;
}
static inline s8 roten_dComIfGp_evmng_getMyActIdx(s32 staffId, u32 tbl, s32 n, BOOL force,
                                                  s32 nameType) {
    return roten_getActionIndex(dComIfGp_getPEvtManager(), staffId, tbl, n, force, nameType);
}

/* actors */

BOOL roten_fopAcM_SearchByID_o(u32 id, gptr<fopAc_ac_c>* out);

static inline u32 roten_fopAcM_createItemForPresentDemo(cXyz* pos, u8 itemNo, u8 flag, s32 bitNo,
                                                        s32 roomNo) {
    return roten_createPresentItem(pos, itemNo, flag, bitNo, roomNo, 0, 0);
}

BOOL roten_fopAcM_orderChangeEventId(fopAc_ac_c* a, fopAc_ac_c* b, s16 ev, u16 flag, u16 hind);

void daItemBase_dead(fopAc_ac_c* a);
void daItemBase_show(fopAc_ac_c* a);

/* d_npc */

void roten_dNpc_JntCtrl_lookAtTarget(dNpc_JntCtrl_c* j, be<s16>* outY, cXyz* target, cXyz* eye,
                                     s16 yrot, s16 vel, u8 headOnly);
void roten_dNpc_PathRun_setInf(dNpc_PathRun_r* r, u8 id, s8 room, u8 b);
void roten_dNpc_PathRun_getPoint(dNpc_PathRun_r* r, cXyz* out, u8 idx);
void roten_dNpc_PathRun_incIdxLoop(dNpc_PathRun_r* r);
BOOL roten_dNpc_PathRun_chkPointPass(dNpc_PathRun_r* r, cXyz* pos, u32 dir);
BOOL roten_dNpc_PathRun_nextIdxAuto(dNpc_PathRun_r* r);

/* J3D (HD) */
/* J3DModel::getAnmMtx (HD: the matrix block's dirty flag is set on access) */
static inline Mtx34* getAnmMtx(J3DModel* model, s32 jntNo) { return model->getAnmMtx(jntNo); }
void roten_PSMTXConcat(Mtx34* a, Mtx34* b, Mtx34* ab);
static inline s32 J3DAnm_getFrameMax(J3DAnmTexPattern* animation) {
    auto* view = reinterpret_cast<RotenAnimationView*>(animation);
    return gabi::call_ptr<s32>(view->vtable->getFrameMax, animation);
}

struct RotenMessageNumbers {
    be<u32> entries[3];
};
struct RotenForceMessages {
    be<u32> entries[7];
};
struct RotenMapMessages {
    be<u32> entries[2];
};
struct RotenMessageTable {
    gptr<be<u32>> entries[3];
};
struct RotenExchangeMessages {
    gptr<be<u32>> entries[3][12];
};
struct RotenMessageAnimations {
    gptr<sRotenAnmDat> entries[9];
};
struct RotenItemNumber {
    be<u32> itemNo;
};
WWHD_SIZE(RotenMessageNumbers, 12);
WWHD_SIZE(RotenForceMessages, 28);
WWHD_SIZE(RotenMapMessages, 8);
WWHD_SIZE(RotenMessageTable, 12);
WWHD_SIZE(RotenExchangeMessages, 144);
WWHD_SIZE(RotenMessageAnimations, 36);
WWHD_SIZE(RotenItemNumber, 4);
// HD: model and message fields used only by this merchant translation unit.
struct RotenModelData : J3DModelData {
    u8 _14[0x38 - 0x14];
    be<u32> mpTexNoAnm;
};
WWHD_OFFSET(RotenModelData, mpTexNoAnm, 0x38);
WWHD_SIZE(RotenModelData, 0x3C);
struct RotenMessageManager {
    u8 _00[0x948];
    be<u32> mSelectNum;
};
WWHD_OFFSET(RotenMessageManager, mSelectNum, 0x948);
WWHD_SIZE(RotenMessageManager, 0x94C);
struct RotenCurrentModel {
    gptr<J3DModel> model;
};
struct RotenMessageManagerPtr {
    gptr<RotenMessageManager> manager;
};
struct RotenPresentItem : fopAc_ac_c {
    u8 _3AC[0x760 - 0x3AC];
    cXyz mOffsetPos;
};
WWHD_OFFSET(RotenPresentItem, mOffsetPos, 0x760);
WWHD_SIZE(RotenPresentItem, 0x76C);

extern RotenMessageNumbers& l_msg_xy_koukan_end;
extern RotenMessageNumbers& l_msg_xy_koukan_first;
extern RotenMessageNumbers& l_msg_xy_koukan_no;
extern RotenMessageNumbers& l_msg_xy_koukan_rupee;
extern RotenForceMessages& l_msg_try_force;
extern RotenMapMessages& l_msg_collect_map;
extern RotenMessageTable& l_msg_xy_exchange3;
extern RotenMessageTable& l_msg_xy_no_roten_item;
extern RotenMessageTable& l_msg_xy_invalid_item;
extern RotenMessageTable& l_msg_1st_talk;
extern RotenMessageTable& l_msg_exchange3;
extern RotenMessageTable& l_msg_exchange;
extern RotenMessageTable& l_msg_etc;
extern RotenExchangeMessages& l_msg_xy_koukan_item;
extern RotenExchangeMessages& l_msg_xy_koukan_item2;
extern RotenExchangeMessages& l_msg_xy_koukan_item3;
extern RotenMessageTable& l_msg_xy_after_get_demo;
extern RotenMessageTable& l_msg_xy_after_get_demo2;
extern RotenMessageAnimations& l_msg_anm;
extern RotenItemNumber& l_get_item_no;
extern RotenCurrentModel& l_currentModel;
extern Mtx34& l_currentMtx;
extern RotenMessageManagerPtr& l_messageManager;

static inline J3DModelData* roten_modelData(J3DModel* model) { return model->getModelData(); }
