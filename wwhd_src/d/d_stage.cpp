/** Stage and room metadata, object-name lookup, and scene creation.
 * HD: stage memory configuration/map slots are removed; the unit header records the
 * remaining virtual entries and the enlarged room lighting/status layout.
 */
#include "d/d_stage.h"

static const dStage_SafeStringVtable l_safeStringVtableData;
const dStage_SafeStringVtable& dStage_safeStringVtable = l_safeStringVtableData;
static dStage_roomControlData l_roomControlData;
be<char> (&dStage_roomControlStatics::mDemoArcName)[8] = l_roomControlData.mDemoArcName;
be<u32>& dStage_roomControlStatics::mRoomChangeCount = l_roomControlData.mRoomChangeCount;
be<u32>& dStage_roomControlStatics::mProcID = l_roomControlData.mProcID;
be<s8>& dStage_roomControlStatics::mStayNo = l_roomControlData.mStayNo;
be<s8>& dStage_roomControlStatics::mOldStayNo = l_roomControlData.mOldStayNo;
be<u8>& dStage_roomControlStatics::mDarkRatio = l_roomControlData.mDarkRatio;
dStage_roomStatus_c (&dStage_roomControlStatics::mStatus)[64] = l_roomControlData.mStatus;
static const dStage_objectNameTable l_objectNames;
static dStage_KeepTresureInfo l_treasureInfoData;
static dStage_KeepDoorInfo l_doorInfoData;
dStage_KeepTresureInfo& TresureInfo = l_treasureInfoData;
dStage_KeepDoorInfo& DoorInfo = l_doorInfoData;

/* 025C1004 */
static s32 createRoomScene(s32 param_0) {
    u32 ptr = StageSceneAPI::JKRAlloc(4, -4);
    if (ptr == 0) return 0;
    gabi::store<u32>(ptr, (u32)param_0);
    return (s32)StageSceneAPI::fopScnM_CreateReq(dStageProcess::fpcNm_ROOM_SCENE_e, 0x7FFF, 0, ptr);
}

// HD: dStage_roomControl_c::setStayNo(int), HD: static (no this); the 0xFF test of the GameCube source (always false) is gone
/* 025C1070 */
static void dStage_roomControl_c_setStayNo(s32 i_roomNo) {
    s8 no = (s8)i_roomNo;
    u8 old = dStage_roomControlStatics::mStayNo;
    dStage_roomControlStatics::mStayNo = no;
    dStage_roomControlStatics::mOldStayNo = (s8)old;
    dStage_roomControlStatics::mStatus[no].mDraw = 1; /* onStatusDraw */
}

/* 025C109C */
static const dStage_objectNameInf* dStage_searchName(const char* i_name) {
    const dStage_objectNameInf* obj = l_objectNames.mEntries;
    for (u32 i = sizeof(l_objectNames.mEntries) / sizeof(l_objectNames.mEntries[0]); i != 0; i--) {
        u32 a = gabi::ea(obj->name), b = gabi::ea(i_name);
        u8 ca, cb;
        for (;;) {
            ca = gabi::load<u8>(a);
            cb = gabi::load<u8>(b);
            if (ca != cb || ca == 0) break;
            a++;
            b++;
        }
        if (ca == cb) return obj;
        ++obj;
    }
    return 0;
}

// HD: an argument of -1 (in the table or the request) matches any, and when only the process name matches, the last such entry is returned instead of "?". The registers are compared whole (the callers pass them sign-extended).
/* 025C10E8 */
static const char* dStage_getName(u32 i_procName, u32 i_argument) {
    const dStage_objectNameInf* last = nullptr;
    const dStage_objectNameInf* obj = l_objectNames.mEntries;
    for (u32 i = sizeof(l_objectNames.mEntries) / sizeof(l_objectNames.mEntries[0]); i != 0; i--) {
        if ((u32)(s32)(s16)obj->procname == i_procName) {
            s32 arg = (s8)obj->argument;
            if ((u32)arg == i_argument) return obj->name;
            if (arg == -1) return obj->name;
            if ((s32)i_argument == -1) return obj->name;
            last = obj;
        }
        ++obj;
    }
    if (last == nullptr) return gabi::at<const char>(gabi::guest_str("誰？")); /* "誰？" */
    return last->name;
}

/* 025C1150 */
static const char* dStage_getName2(u32 i_procName, u32 i_argument) {
    return dStage_getName(i_procName, i_argument);
}

// HD: fopAcM_Create inlined (fpcSCtRq_Request in the current layer)
/* 025C1154 */
static void dStage_actorCreate(stage_actor_data_class* i_actorData, fopAcM_prm_class* i_actorPrm) {
    const dStage_objectNameInf* nameinf = dStage_searchName(i_actorData->name);
    if (nameinf == 0) {
        StageSceneAPI::JKRFree(gabi::ea(i_actorPrm));
        return;
    }
    i_actorPrm->argument = nameinf->argument;
    i_actorPrm->gbaName = nameinf->gbaName;
    s32 proc = (s16)nameinf->procname;
    u32 layer = StageSceneAPI::fpcLy_CurrentLayer();
    StageSceneAPI::fpcSCtRq_Request(layer, proc, 0, 0, gabi::ea(i_actorPrm));
}

// HD: an unsigned bound check with an assert
/* 025C11DC */
static dStage_roomDt_c* dStage_roomControl_c_getStatusRoomDt(u32 self, s32 i_statusIdx) {
    if ((u32)i_statusIdx >= 0x40) {
        StageSceneAPI::reportAssert("d_stage.cpp", 0x19E, "0");
        return 0;
    }
    return &dStage_roomControlStatics::mStatus[i_statusIdx].mRoomDt;
}

/* 025C123C */
static BOOL dStage_chkPlayerId(s32 playerId, s32 room_no) {
    stage_actor_class* player;
    if (room_no == -1) {
        u32 p = dComIfGp_ea();
        dStage_dt_c* stage = gabi::at<dStage_dt_c>(gabi::ea(gabi::at<dComIfG_inf_c>(p)->play.mStageData));
        player = gabi::call_ptr<stage_actor_class*>(stage->__vtbl->getPlayer.function.get(), stage);
    } else {
        if ((u32)room_no >= 64) StageSceneAPI::reportAssert("d_stage.cpp", 0x6F6, "0 <= room_no && room_no < 64");
        u32 p = dComIfGp_ea();
        dStage_roomDt_c* room = dStage_roomControl_c_getStatusRoomDt(gabi::ea(&gabi::at<dComIfG_inf_c>(p)->play.mRoomCtrl), room_no);
        dStage_dt_c* data = reinterpret_cast<dStage_dt_c*>(room);
        player = gabi::call_ptr<stage_actor_class*>(data->__vtbl->getPlayer.function.get(), data);
    }
    if (player == nullptr) return FALSE;
    s32 num = player->num;
    stage_actor_data_class* actor = player->m_entries.get();
    if (num <= 0) return FALSE;
    for (; num != 0; --num, ++actor) {
        // HD: the spawn ID is read as the low byte of the Z-angle word.
        if ((u32)gabi::load<u8>(gabi::ea(&actor->base.angle.z) + 1) == (u32)playerId) return TRUE;
    }
    return FALSE;
}

// HD: dStage_startStage_c::set(const char*, s8, s16, s8)
/* 025C1328 */
static void dStage_startStage_c_set(dStage_startStage_c* self, const char* i_name, s32 i_roomNo, s32 i_point, s32 i_layer) {
    StageSceneAPI::strcpy(gabi::ea(self), gabi::ea(i_name));
    self->mLayer = (s8)i_layer;
    self->mRoomNo = (s8)i_roomNo;
    self->mPoint = (s16)i_point;
}

/* WWHD glue (tools/verify/autoglue.py): private ABI bindings
str 100558E8 in 025C10E8
str 100558F0 in 025C11DC
str 100558F4 in 025C11DC
str 10055900 in 025C123C
str 1005590C in 025C123C
data l_roomControlData 1047E6B8
data l_safeStringVtableData 10055394
data l_objectNames 101EBFDC
data l_treasureInfoData 1047D9B0
data l_doorInfoData 1047DDB4
ext 0273AA24 void StageSceneAPI::reportAssert(const char* file, s32 line, const char* msg)
ext 027EC0AC u32 StageSceneAPI::JKRAlloc(u32 size, s32 align)
ext 027EC0B0 void StageSceneAPI::JKRFree(u32 p)
ext 025DC91C u32 StageSceneAPI::fopScnM_CreateReq(s32 proc, s32 prio, u32 a2, u32 data)
ext 025DED64 u32 StageSceneAPI::fpcLy_CurrentLayer()
ext 025E14A8 u32 StageSceneAPI::fpcSCtRq_Request(u32 layer, s32 proc, u32 a2, u32 a3, u32 prm)
ext 028F040C void StageSceneAPI::strcpy(u32 dst, u32 src)
*/
