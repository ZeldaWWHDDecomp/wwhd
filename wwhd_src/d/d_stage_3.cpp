/** Stage-chunk handlers, room/path relocation, retained objects, and ship placement.
 * HD: stage-name comparisons retain SafeString termination calls; retained actor and
 * door records use the verified integer-word copy order.
 */
#include "d/d_stage.h"

static inline u32 dStage_getSaveBase() { return gabi::ea(dComIfGs_info()) - 0x20; }

struct dStage_shipPositionFrame { be<s16> shipProcessName, fallbackProcessName; SafeString expectedStage, currentStage; };

/* inline sead::SafeString::isEqual(a, b) with both strings' assureTermination calls */
static bool dStage_isEqual(SafeString* a, SafeString* b) {
    gabi::call_ptr(gabi::at<dStage_SafeStringVtable>(a->__vtbl)->assureTermination.function.get(), gabi::ea(a));
    gabi::call_ptr(gabi::at<dStage_SafeStringVtable>(a->__vtbl)->assureTermination.function.get(), gabi::ea(a));
    u32 fn = gabi::at<dStage_SafeStringVtable>(b->__vtbl)->assureTermination.function.get();
    u32 sa = a->mStringTop;
    gabi::call_ptr(fn, gabi::ea(b));
    u32 sb = b->mStringTop;
    if (sa == sb) return true;
    sb -= 1;
    for (u32 i = 0x40001; i != 0; i--) {
        u8 ca = gabi::load<u8>(sa);
        sb += 1;
        u8 cb = gabi::load<u8>(sb);
        if (ca != cb) return false;
        if (ca == 0) return true;
        sa++;
    }
    return false;
}

/* 025C1E44 */
static s32 dStage_stagInfoInit(dStage_dt_c* i_stage, dStage_nodeHeader* i_data, s32 i_num, u32 i_file) {
    u32 fn = i_stage->__vtbl->setStagInfo.function.get();
    gabi::call_ptr(fn, i_stage, (u32)i_data->m_offset);
    u32 stag = gabi::call_ptr<u32>(i_stage->__vtbl->getStagInfo.function.get(), i_stage);
    u32 stageNo = (gabi::at<stage_stag_info_class>(stag)->mProp >> 1) & 0x7F; /* dStage_stagInfo_GetSaveTbl */
    StageChunkAPI::dSv_info_c_getSave(gabi::ea(dComIfGs_info()), stageNo);
    StageChunkAPI::dSv_danBit_c_init(gabi::ea(&dComIfGs_info()->mDan), (s8)stageNo);
    return 1;
}

/* 025C1ECC */
static s32 dStage_sclsInfoInit(dStage_dt_c* i_stage, dStage_nodeHeader* i_data, s32 i_num, u32 i_file) {
    gabi::call_ptr(i_stage->__vtbl->setSclsInfo.function.get(), i_stage, gabi::ea(&i_data->m_entryNum));
    return 1;
}

/* 025C1F00 */
static s32 dStage_actorInit(dStage_dt_c* i_stage, dStage_nodeHeader* i_data, s32 i_num, u32 i_file) {
    s32 n = (s32)i_data->m_entryNum;
    u32 e = (u32)i_data->m_offset;
    if (n <= 0) return 1;
    for (u32 c = (u32)n; c != 0; c--, e += 0x20) {
        u32 fn = i_stage->__vtbl->getRoomNo.function.get();
        u32 setId = gabi::at<stage_actor_data_class>(e)->base.setID;
        u32 roomNo = gabi::call_ptr<u32>(fn, i_stage);
        if (StageChunkAPI::dSv_info_c_isActor(gabi::ea(dComIfGs_info()), setId, roomNo)) continue;
        u32 appen = StageChunkAPI::fopAcM_CreateAppend();
        if (appen == 0) continue;
        dStage_copyActorBase(appen, e + offsetof(stage_actor_data_class, base));
        u32 r = gabi::call_ptr<u32>(i_stage->__vtbl->getRoomNo.function.get(), i_stage);
        gabi::at<fopAcM_prm_class>(appen)->room_no = (s8)r;
        StageChunkAPI::dStage_actorCreate(e, appen);
    }
    return 1;
}

/* 025C1FE0 */
static s32 dStage_tgscInfoInit(dStage_dt_c* i_stage, dStage_nodeHeader* i_data, s32 i_num, u32 i_file) {
    s32 n = (s32)i_data->m_entryNum;
    u32 e = (u32)i_data->m_offset;
    if (n <= 0) return 1;
    for (u32 c = (u32)n; c != 0; c--, e += 0x24) {
        u32 appen = StageChunkAPI::fopAcM_CreateAppend();
        if (appen == 0) continue;
        dStage_copyActorBase(appen, e + offsetof(stage_actor_data_class, base));
        u32 r = gabi::call_ptr<u32>(i_stage->__vtbl->getRoomNo.function.get(), i_stage);
        gabi::at<fopAcM_prm_class>(appen)->room_no = (s8)r;
        u8 s0 = gabi::at<stage_tgsc_data_class>(e)->scale.x, s1 = gabi::at<stage_tgsc_data_class>(e)->scale.y, s2 = gabi::at<stage_tgsc_data_class>(e)->scale.z; /* scale u8[3] (lswi/stswi 3 bytes) */
        gabi::at<fopAcM_prm_class>(appen)->scale.x = s0;
        gabi::at<fopAcM_prm_class>(appen)->scale.y = s1;
        gabi::at<fopAcM_prm_class>(appen)->scale.z = s2;
        StageChunkAPI::dStage_actorCreate(e, appen);
    }
    return 1;
}

// HD: dStage_roomReadInit (unnamed by the matcher)
/* 025C20B0 */
static s32 dStage_roomReadInit(dStage_dt_c* i_stage, dStage_nodeHeader* i_data, s32 i_num, u32 i_file) {
    u32 rtbl = gabi::ea(&i_data->m_entryNum);
    u32 fn = i_stage->__vtbl->setRoom.function.get();
    u32 entries = (u32)i_data->m_offset;
    gabi::call_ptr(fn, i_stage, rtbl);
    s32 n = (s32)i_data->m_entryNum;
    if (0 >= n) return 1;
    u32 p = entries;
    for (s32 i = 0;;) {
        u32 r = gabi::at<gptr<roomRead_data_class>>(p)->v + i_file;
        gabi::at<gptr<roomRead_data_class>>(p)->v = r;
        gabi::at<roomRead_data_class>(r)->m_rooms.v = gabi::at<roomRead_data_class>(r)->m_rooms.v + i_file;
        p += 4;
        i++;
        if (!(i < (s32)gabi::at<roomRead_class>(rtbl)->num)) break;
    }
    return 1;
}

/* 025C2150 */
static u32 dStage_roomRead_dt_c_GetReverbStage(u32 room, s32 index) {
    u32 entries = gabi::ea(gabi::at<roomRead_class>(room)->m_entries.get());
    if (index < 0 || index >= (s32)gabi::at<roomRead_class>(room)->num) index = 0;
    return gabi::at<gptr<roomRead_data_class>>(entries)[index]->field_0x1 & 0x7F;
}

// HD: dStage_ppntInfoInit
/* 025C2184 */
static s32 dStage_ppntInfoInit(dStage_dt_c* i_stage, dStage_nodeHeader* i_data, s32 i_num, u32 i_file) {
    gabi::call_ptr(i_stage->__vtbl->setPntInfo.function.get(), i_stage, gabi::ea(&i_data->m_entryNum));
    return 1;
}

/* the path handlers: set the path chunk, then relocate each path's point pointer by the point
 * chunk's offset (getPntInf / getPnt2Inf) */
static s32 pathInit(dStage_dt_c* i_stage, dStage_nodeHeader* i_data, dStage_virtualEntry dStage_vtable::* setter, dStage_virtualEntry dStage_vtable::* getter) {
    u32 hdr = gabi::ea(&i_data->m_entryNum);
    u32 fn = (i_stage->__vtbl.get()->*setter).function.get();
    u32 path = (u32)i_data->m_offset;
    gabi::call_ptr(fn, i_stage, hdr);
    s32 n = (s32)i_data->m_entryNum;
    if (0 >= n) return 1;
    for (s32 i = 0;;) {
        u32 pnt = gabi::call_ptr<u32>((i_stage->__vtbl.get()->*getter).function.get(), i_stage);
        u32 off = gabi::at<dStage_dPnt_c>(pnt)->m_pnt_offset;
        gabi::at<path_layout::dPath>(path)->m_points.v = gabi::at<path_layout::dPath>(path)->m_points.v + off;
        i++;
        path += 0xC;
        if (!(i < (s32)gabi::at<dStage_dPath_c>(hdr)->num)) break;
    }
    return 1;
}

// HD: dStage_pathInfoInit
/* 025C21B8 */
static s32 dStage_pathInfoInit(dStage_dt_c* i_stage, dStage_nodeHeader* i_data, s32 i_num, u32 i_file) {
    return pathInit(i_stage, i_data, &dStage_vtable::setPathInfo, &dStage_vtable::getPntInf);
}

// HD: dStage_rppnInfoInit
/* 025C2264 */
static s32 dStage_rppnInfoInit(dStage_dt_c* i_stage, dStage_nodeHeader* i_data, s32 i_num, u32 i_file) {
    gabi::call_ptr(i_stage->__vtbl->setPnt2Info.function.get(), i_stage, gabi::ea(&i_data->m_entryNum));
    return 1;
}

// HD: dStage_rpatInfoInit
/* 025C2298 */
static s32 dStage_rpatInfoInit(dStage_dt_c* i_stage, dStage_nodeHeader* i_data, s32 i_num, u32 i_file) {
    return pathInit(i_stage, i_data, &dStage_vtable::setPath2Info, &dStage_vtable::getPnt2Inf);
}

// HD: dStage_soundInfoInit
/* 025C2344 */
static s32 dStage_soundInfoInit(dStage_dt_c* i_stage, dStage_nodeHeader* i_data, s32 i_num, u32 i_file) {
    gabi::call_ptr(i_stage->__vtbl->setSoundInfo.function.get(), i_stage, gabi::ea(&i_data->m_entryNum));
    return 1;
}

// HD: dStage_eventInfoInit
/* 025C2378 */
static s32 dStage_eventInfoInit(dStage_dt_c* i_stage, dStage_nodeHeader* i_data, s32 i_num, u32 i_file) {
    gabi::call_ptr(i_stage->__vtbl->setEventInfo.function.get(), i_stage, gabi::ea(&i_data->m_entryNum));
    return 1;
}

// HD: dStage_floorInfoInit
/* 025C23AC */
static s32 dStage_floorInfoInit(dStage_dt_c* i_stage, dStage_nodeHeader* i_data, s32 i_num, u32 i_file) {
    gabi::call_ptr(i_stage->__vtbl->setFloorInfo.function.get(), i_stage, gabi::ea(&i_data->m_entryNum));
    return 1;
}

// HD: fopAcM_SearchByName(SHIP) inlined (fopAcIt_Judge with fpcSch_JudgeForPName)
/* 025C23E0 */
static BOOL dStage_setShipPos(s32 param_0, s32 i_roomNo) {
    gabi::Local<dStage_shipPositionFrame> fr; /* the frame from SP+8: two process names, two SafeStrings */
    SafeString* s1 = &fr->expectedStage;
    SafeString* s2 = &fr->currentStage;
    s1->mStringTop = gabi::guest_str("GanonM"); /* "GanonM" */
    s1->__vtbl = gabi::ea(&dStage_safeStringVtable);
    bool clear = false;
    u32 p = dComIfGp_ea();
    s2->__vtbl = gabi::ea(&dStage_safeStringVtable);
    s2->mStringTop = gabi::ea(&gabi::at<dComIfG_inf_c>(p)->play.mCurStage); /* the start stage name */
    if (dStage_isEqual(s1, s2)) {
        if (!dComIfGs_isEventBit(dStageEvent::UNK_3D02)) clear = true;
    }
    if (clear) {
        param_0 = 0xFF;
        i_roomNo = 0xFF;
        gabi::at<dComIfG_inf_c>(dComIfGp_ea())->play.mShipId = 0xFF;
        gabi::at<dComIfG_inf_c>(dComIfGp_ea())->play.mShipRoomId = 0xFF;
    }
    if ((s16)dComIfGp_getStartStagePoint() == -3 && dComIfGs_info()->mTurnRestart.mHasShip != 0) {
        fr->shipProcessName = dStageProcess::fpcNm_SHIP_e;
        u32 ship = StageChunkAPI::fopAcIt_Judge(0x025E121C, gabi::ea(&fr->shipProcessName));
        if (ship != 0) {
            u32 s = dStage_getSaveBase();
            StageChunkAPI::daShip_c_initStartPos(ship, gabi::ea(&gabi::at<dSv_info_c>(s + 0x20)->mTurnRestart.mShipPos), (s16)gabi::at<dSv_info_c>(s + 0x20)->mTurnRestart.mShipAngleY);
        }
        dComIfGs_info()->mTurnRestart.mHasShip = 0;
    }
    s32 roomNo = i_roomNo;
    if (i_roomNo == 0xFF) {
        roomNo = (s8)dStage_roomControlStatics::mStayNo; /* mStayNo */
        if (param_0 == 0xFF) return FALSE;
    } else if (param_0 == 0xFF) {
        return FALSE;
    }
    u32 data = StageChunkAPI::dComIfGp_getShip(roomNo, param_0);
    if (data == 0) return FALSE;
    fr->fallbackProcessName = dStageProcess::fpcNm_SHIP_e;
    u32 ship = StageChunkAPI::fopAcIt_Judge(0x025E121C, gabi::ea(&fr->fallbackProcessName));
    if (ship == 0) return FALSE;
    StageChunkAPI::daShip_c_initStartPos(ship, data, (s16)gabi::load<u16>(data + 0xC));
    return TRUE;
}

// HD: dStage_chkTaura inlined
/* 025C2608 */
static s32 dStage_shipInfoInit(dStage_dt_c* i_stage, dStage_nodeHeader* i_data, s32 i_num, u32 i_file) {
    gabi::call_ptr(i_stage->__vtbl->setShip.function.get(), i_stage, gabi::ea(&i_data->m_entryNum));
    s32 shipId = gabi::at<dComIfG_inf_c>(dComIfGp_ea())->play.mShipId;
    s32 roomId = gabi::at<dComIfG_inf_c>(dComIfGp_ea())->play.mShipRoomId;
    gabi::Local<SafeString> s1;
    gabi::Local<SafeString> s2;
    s1->__vtbl = gabi::ea(&dStage_safeStringVtable);
    s1->mStringTop = gabi::guest_str("sea"); /* "sea" */
    bool taura = false;
    u32 p = dComIfGp_ea();
    s2->__vtbl = gabi::ea(&dStage_safeStringVtable);
    s2->mStringTop = gabi::ea(&gabi::at<dComIfG_inf_c>(p)->play.mCurStage);
    if (dStage_isEqual(s1.get(), s2.get())) {
        if (roomId == 0xB) taura = true;
    }
    if (taura && !dComIfGs_isEventBit(dStageEvent::RODE_KORL)) {
        if (dStage_setShipPos(0x80, roomId)) {
            shipId = 0xFF;
            roomId = 0xFF;
            gabi::at<dComIfG_inf_c>(dComIfGp_ea())->play.mShipId = 0xFF;
            gabi::at<dComIfG_inf_c>(dComIfGp_ea())->play.mShipRoomId = 0xFF;
        }
    }
    if (dStage_setShipPos(shipId, roomId)) {
        gabi::at<dComIfG_inf_c>(dComIfGp_ea())->play.mShipId = 0xFF;
        gabi::at<dComIfG_inf_c>(dComIfGp_ea())->play.mShipRoomId = 0xFF;
    }
    return 1;
}

// HD: dStage_multInfoInit
/* 025C27AC */
static s32 dStage_multInfoInit(dStage_dt_c* i_stage, dStage_nodeHeader* i_data, s32 i_num, u32 i_file) {
    gabi::call_ptr(i_stage->__vtbl->setMulti.function.get(), i_stage, gabi::ea(&i_data->m_entryNum));
    return 1;
}

// HD: dStage_lbnkInfoInit
/* 025C27E0 */
static s32 dStage_lbnkInfoInit(dStage_dt_c* i_stage, dStage_nodeHeader* i_data, s32 i_num, u32 i_file) {
    gabi::call_ptr(i_stage->__vtbl->setLbnk.function.get(), i_stage, gabi::ea(&i_data->m_entryNum));
    return 1;
}

/* the inlined dStage_KeepTresureInfoProc / dStage_KeepDoorInfoProc */
static void keepInfo(dStage_dt_c* i_stage, u32 src, u32 info, u32 max, u32 size) {
    u32 stag = gabi::call_ptr<u32>(i_stage->__vtbl->getStagInfo.function.get(), i_stage);
    if (stag == 0) return;
    u32 type = (gabi::at<stage_stag_info_class>(stag)->mStageTypeAndSchbit >> 16) & 7; /* dStage_stagInfo_GetSTType */
    if (type == 3 || type == 6) return;
    if (src == 0 || gabi::load<u32>(src) >= max) {
        gabi::store<u32>(info, 0);
        return;
    }
    u32 num = gabi::load<u32>(src);
    gabi::store<u32>(info, num);
    if (num == 0) return;
    u32 s = gabi::load<u32>(src + 4), d = info + 4;
    for (s32 i = 0; i < (s32)gabi::load<u32>(info); i++, s += size, d += size)
        for (u32 o = 0; o < size; o += 4) gabi::store<u32>(d + o, gabi::load<u32>(s + o));
}

// HD: dStage_stageTresureInit, dStage_KeepTresureInfoProc inlined (matcher: dStage_KeepTresureInfoProc)
/* 025C2814 */
static s32 dStage_stageTresureInit(dStage_dt_c* i_stage, dStage_nodeHeader* i_data, s32 i_num, u32 i_file) {
    u32 tres = gabi::ea(&i_data->m_entryNum);
    gabi::call_ptr(i_stage->__vtbl->setTresure.function.get(), i_stage, tres);
    dStage_actorInit(i_stage, i_data, i_num, i_file);
    keepInfo(i_stage, tres, gabi::ea(&TresureInfo), 0x20, 0x20);
    return 1;
}

/* 025C2950 */
static s32 dStage_roomTresureInit(dStage_dt_c* i_stage, dStage_nodeHeader* i_data, s32 i_num, u32 i_file) {
    gabi::call_ptr(i_stage->__vtbl->setTresure.function.get(), i_stage, gabi::ea(&i_data->m_entryNum));
    dStage_actorInit(i_stage, i_data, i_num, i_file);
    return 1;
}

/* 025C29C8 */
static s32 dStage_layerTresureInit(dStage_dt_c* i_stage, dStage_nodeHeader* i_data, s32 i_num, u32 i_file) {
    dStage_actorInit(i_stage, i_data, i_num, i_file);
    return 1;
}

// HD: dStage_dmapInfoInit
/* 025C29EC */
static s32 dStage_dmapInfoInit(dStage_dt_c* i_stage, dStage_nodeHeader* i_data, s32 i_num, u32 i_file) {
    gabi::call_ptr(i_stage->__vtbl->setDMap.function.get(), i_stage, gabi::ea(&i_data->m_entryNum));
    return 1;
}

// HD: dStage_stageDrtgInfoInit, dStage_KeepDoorInfoProc inlined (matcher: dStage_KeepDoorInfoProc)
/* 025C2A20 */
static s32 dStage_stageDrtgInfoInit(dStage_dt_c* i_stage, dStage_nodeHeader* i_data, s32 i_num, u32 i_file) {
    u32 drtg = gabi::ea(&i_data->m_entryNum);
    gabi::call_ptr(i_stage->__vtbl->setDrTg.function.get(), i_stage, drtg);
    dStage_tgscInfoInit(i_stage, i_data, i_num, i_file);
    keepInfo(i_stage, drtg, gabi::ea(&DoorInfo), 0x40, 0x24);
    return 1;
}

/* 025C2B48 */
static s32 dStage_roomDrtgInfoInit(dStage_dt_c* i_stage, dStage_nodeHeader* i_data, s32 i_num, u32 i_file) {
    gabi::call_ptr(i_stage->__vtbl->setDrTg.function.get(), i_stage, gabi::ea(&i_data->m_entryNum));
    dStage_tgscInfoInit(i_stage, i_data, i_num, i_file);
    return 1;
}

/* WWHD glue (tools/verify/autoglue.py): private ABI bindings
str 1005532C in 025C2608
str 1005596C in 025C23E0
ext 025D5600 u32 StageChunkAPI::fopAcM_CreateAppend()
ext 025B9C9C void StageChunkAPI::dSv_info_c_getSave(u32 info, s32 stageNo)
ext 025B9174 void StageChunkAPI::dSv_danBit_c_init(u32 dan, s32 stageNo)
ext 025BA6A4 BOOL StageChunkAPI::dSv_info_c_isActor(u32 info, u32 setId, u32 roomNo)
ext 025B8B94 BOOL StageChunkAPI::dSv_event_c_isEventBit(u32 ev, u32 flag)
ext 025D5218 u32 StageChunkAPI::fopAcIt_Judge(u32 judge, u32 data)
ext 024832E0 void StageChunkAPI::daShip_c_initStartPos(u32 ship, u32 pos, s32 angle)
ext 02520584 u32 StageChunkAPI::dComIfGp_getShip(s32 roomNo, s32 id)
ext 025C1154 void StageChunkAPI::dStage_actorCreate(u32 data, u32 prm)
*/
