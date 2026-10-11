/** Stage/room loading, scene changes, room control, and compiler initialization.
 * HD: room loading registers an exception-dump context, room memory blocks are absent,
 * and sky boxes are skipped in GTower and Hyrule. Lighting slots use the HD layout.
 */
#include "d/d_stage.h"

/* StageLifecycleAPI::dComIfGp_setNextStage(name, point, roomNo, layer, speed, mode, enable, wipe); the registers as the callers set them */

static const dStage_FuncTable l_stageInitFunctions;
static const dStage_layerFunctionTables l_layerFunctions;
static const dStage_stageFunctionTable l_stageFunctions;
static const dStage_roomFunctionTable l_roomFunctions;
static const dStage_roomReloadFunctionTable l_roomReloadFunctions;
static const dStage_SafeStringVtable l_roomLoaderVtable;
static const dStage_darkStatusTable l_darkStatus;
static dStage_registrationNodes l_registrationNodes;
static dStage_runtimeStatics l_runtimeStatics;
static const dStage_exitStageName l_asokoExitName;
static const f32 l_zero = 0.0f;
static const f32 l_restartMorning = 180.0f;
static const f32 l_darkRatioScale = 0.0078125f;
static const f32 l_one = 1.0f;
static const f32 l_initialMinAngle = -3.1415927410125732f;
static const f32 l_initialMaxAngle = 3.1415927410125732f;
static u8 l_timePass;
static const dStage_vtable l_roomVtable;
static const dStage_vtable l_stageVtable;
static const fopAc_lightBlockHD l_defaultLight;
static inline u32 dStage_getSaveBase() { return gabi::ea(dComIfGs_info()) - 0x20; }

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

// HD: FuncTable entries are 0xC bytes (tag, -, function +8); file nodes 0xC (tag, num, offset)
/* 025C2BC0 */
static void dStage_dt_c_decode(dStage_fileHeader* i_data, dStage_dt_c* i_stage, const dStage_FuncTable* i_funcTbl, s32 i_tblSize) {
    if (i_data == nullptr) return;
    u32 chunks = i_data->m_chunkCount;
    if (i_tblSize <= 0) return;
    for (u32 n = (u32)i_tblSize; n != 0; --n, ++i_funcTbl) {
        dStage_nodeHeader* node = i_data->m_nodes;
        if (chunks == 0) continue;
        u32 tag = i_funcTbl->m_tag;
        for (u32 j = chunks; j != 0; --j, ++node) {
            if (node->m_tag == tag) {
                u32 fn = i_funcTbl->m_function.get();
                if (fn != 0) gabi::call_ptr(fn, i_stage, node, (u32)node->m_entryNum, i_data);
                break;
            }
        }
    }
}

/* 025C2C70 */
static void dStage_dt_c_offsetToPtr(dStage_fileHeader* i_data) {
    u32 n = i_data->m_chunkCount;
    if (n == 0) return;
    dStage_nodeHeader* node = i_data->m_nodes;
    for (; n != 0; --n, ++node) {
        u32 off = node->m_offset;
        if (off != 0) node->m_offset = gabi::ea(i_data) + off;
    }
}

/* 025C2CA4 */
static void dStage_dt_c_stageInitLoader(dStage_fileHeader* i_data, dStage_dt_c* i_stage) {
    if (i_data == 0) StageLifecycleAPI::reportAssert("d_stage.cpp", 0x11CD, "i_data != (0)");
    if (i_stage == 0) StageLifecycleAPI::reportAssert("d_stage.cpp", 0x11CE, "i_stage != (0)");
    dStage_dt_c_offsetToPtr(i_data);
    gabi::call_ptr(i_stage->__vtbl->init.function.get(), i_stage); /* init */
    dStage_dt_c_decode(i_data, i_stage, &l_stageInitFunctions, 1); /* STAG */
}

/* 025C2D4C */
static void layerLoader(dStage_fileHeader* i_data, dStage_dt_c* i_stage, s32 i_roomNo) {
    s32 layer = StageLifecycleAPI::dComIfG_play_c_getLayerNo(i_roomNo);
    dStage_dt_c_decode(i_data, i_stage, l_layerFunctions.mEntries[layer].get(), 3); /* l_layerFuncTable_p */
}

/* 025C2DA8 */
static void dStage_dt_c_stageLoader(dStage_fileHeader* i_data, dStage_dt_c* i_stage) {
    dStage_dt_c_decode(i_data, i_stage, l_stageFunctions.mEntries, 34);
    layerLoader(i_data, i_stage, -1);
}

// HD: a crash-dump context {base 8 bytes, vtable +4, file +8, stage +0xC} is registered around the loading

/* 025C2DFC */
static void dStage_dt_c_roomLoader(dStage_fileHeader* i_data, dStage_dt_c* i_stage) {
    gabi::Local<dStage_roomLoaderContext> ctx;
    StageLifecycleAPI::dumpCtx_ct(gabi::ea(ctx.get()));
    ctx->mFile = gabi::ea(i_data);
    ctx->__vtbl = gabi::ea(&l_roomLoaderVtable);
    ctx->mStage = gabi::ea(i_stage);
    dStage_dt_c_offsetToPtr(i_data);
    gabi::call_ptr(i_stage->__vtbl->init.function.get(), i_stage); /* init */
    dStage_dt_c_decode(i_data, i_stage, l_roomFunctions.mEntries, 22);
    StageLifecycleAPI::dumpCtx_dt(gabi::ea(ctx.get()), 0);
}

/* 025C2E8C */
static void dStage_dt_c_roomReLoader(dStage_fileHeader* i_data, dStage_dt_c* i_stage, s32 param_2) {
    dStage_dt_c_decode(i_data, i_stage, l_roomReloadFunctions.mEntries, 7);
    layerLoader(i_data, i_stage, param_2);
}

/* 025C2EEC */
static void dStage_infoCreate() {
    u32 res = StageLifecycleAPI::dComIfG_getStageRes(gabi::guest_str("Stage"), gabi::guest_str("stage.dzs")); /* "Stage", "stage.dzs" */
    if (res == 0) StageLifecycleAPI::reportAssert("d_stage.cpp", 0x1307, "stageRsrc != (0)");
    u32 p = dComIfGp_ea();
    dStage_dt_c_stageInitLoader(gabi::at<dStage_fileHeader>(res), gabi::at<dStage_dt_c>(gabi::ea(gabi::at<dComIfG_inf_c>(p)->play.mStageData)));
}

/* 025C2F54 */
void dStage_roomDt_c::init() {
    mpPlayer = nullptr;
    mpLbnk = nullptr;
    mpSound = nullptr;
    mpDrTg = nullptr;
    mpLightVecInfo = nullptr;
    mpFileList = nullptr;
    mpMapInfoBase = nullptr;
    mpCamera = nullptr;
    mpPath2 = nullptr;
    mpMapInfo = nullptr;
    mpVrbox = nullptr;
    mLightVecInfoNum = 0;
    mpFloor = nullptr;
    mpArrow = nullptr;
    mpScls = nullptr;
    mpShip = nullptr;
    mpPoint2 = nullptr;
    mpTresure = nullptr;
}

// HD: static members, `this` unused; no memory blocks
/* 025C2FA4 */
static void dStage_roomControl_c_init(u32 self) {
    dStage_roomControlStatics::mOldStayNo = 0xFF;
    dStage_roomControlStatics::mStayNo = 0xFF;
    if ((s16)dComIfGp_getStartStagePoint() >= 0) StageLifecycleAPI::dSv_info_c_initZone(gabi::ea(dComIfGs_info()));
    dStage_roomStatus_c* status = dStage_roomControlStatics::mStatus;
    for (u32 i = 0x40; i != 0; i--, ++status) {
        status->mRoomDt.init();
        status->mFlags = 0;
        status->mDraw = 0;
        if ((s16)dComIfGp_getStartStagePoint() >= 0) {
            status->mpBgW = nullptr;
            status->mZoneNo = 0xFF;
            status->mMemBlockID = 0xFF;
        } else {
            s8 zone = (s8)status->mZoneNo;
            if (zone >= 0) StageLifecycleAPI::dSv_zoneBit_c_clearRoomSwitch(dStage_getSaveBase() + 0x7C8 + zone * 0x4C + 2);
            status->mMemBlockID = 0xFF;
            status->mpBgW = nullptr;
        }
    }
    dStage_roomControlStatics::mDarkRatio = 0xFF;
}

// HD: dKankyo_create and dStage_roomInit inlined. HD: the sky boxes are created when vr_sky.bmd exists, except in GTower and Hyrule; the HD room-change counter is reset
/* 025C3094 */
static void dStage_Create() {
    StageLifecycleAPI::fopKyM_Create(dStageProcess::fpcNm_KANKYO_e, 0, 0);
    StageLifecycleAPI::fopKyM_Create(dStageProcess::fpcNm_KYEFF_e, 0, 0);
    StageLifecycleAPI::fopKyM_Create(dStageProcess::fpcNm_KYEFF2_e, 0, 0);
    StageLifecycleAPI::fopKyM_Create(dStageProcess::fpcNm_ENVSE_e, 0, 0);
    u32 res = StageLifecycleAPI::dComIfG_getStageRes(gabi::guest_str("Stage"), gabi::guest_str("stage.dzs")); /* "Stage", "stage.dzs" */
    if (res == 0) StageLifecycleAPI::reportAssert("d_stage.cpp", 0x1323, "stageRsrc != (0)");
    dStage_roomControl_c_init(gabi::ea(&gabi::at<dComIfG_inf_c>(dComIfGp_ea())->play.mRoomCtrl));
    dStage_dt_c_stageLoader(gabi::at<dStage_fileHeader>(res), gabi::at<dStage_dt_c>(gabi::ea(gabi::at<dComIfG_inf_c>(dComIfGp_ea())->play.mStageData)));
    if ((s8)dComIfGp_getStartStageRoomNo() >= 0) {
        s32 roomNo = (s8)dComIfGp_getStartStageRoomNo();
        dComIfGp_ea();
        dStage_roomControlStatics::mStatus[roomNo].mFlags = 2;
        StageLifecycleAPI::createRoomScene(roomNo);
        StageLifecycleAPI::setStayNo(roomNo);
        dStage_roomControlStatics::mRoomChangeCount = 0;
    }
    StageLifecycleAPI::dMap_c_create();
    dStage_roomControlStatics::mDemoArcName[0] = 0;
    bool create = false;
    if (StageLifecycleAPI::dComIfG_getStageRes(gabi::guest_str("Stage"), gabi::guest_str("vr_sky.bmd")) != 0) { /* "vr_sky.bmd" */
        gabi::Local<SafeString> a;
        gabi::Local<SafeString> b;
        a->__vtbl = gabi::ea(&dStage_safeStringVtable);
        a->mStringTop = gabi::guest_str("GTower"); /* "GTower" */
        u32 p = dComIfGp_ea();
        b->__vtbl = gabi::ea(&dStage_safeStringVtable);
        b->mStringTop = gabi::ea(&gabi::at<dComIfG_inf_c>(p)->play.mCurStage);
        if (!dStage_isEqual(a, b)) {
            gabi::Local<SafeString> c;
            gabi::Local<SafeString> d;
            c->__vtbl = gabi::ea(&dStage_safeStringVtable);
            c->mStringTop = gabi::guest_str("Hyrule"); /* "Hyrule" */
            u32 p2 = dComIfGp_ea();
            d->__vtbl = gabi::ea(&dStage_safeStringVtable);
            d->mStringTop = gabi::ea(&gabi::at<dComIfG_inf_c>(p2)->play.mCurStage);
            if (!dStage_isEqual(c, d)) create = true;
        }
    }
    if (create) {
        StageLifecycleAPI::fpcSCtRq_Request(StageLifecycleAPI::fpcLy_CurrentLayer(), 0x1B5, 0, 0, 0); /* VRBOX */
        StageLifecycleAPI::fpcSCtRq_Request(StageLifecycleAPI::fpcLy_CurrentLayer(), 0x1B6, 0, 0, 0); /* VRBOX2 */
    }
    StageLifecycleAPI::dEvent_manager_c_create(gabi::ea(&gabi::at<dComIfG_inf_c>(dComIfGp_ea())->play.mEvtManager));
}

// HD: no memory blocks to destroy
/* 025C3370 */
static void dStage_Delete() {
    if (dStage_roomControlStatics::mDemoArcName[0] != 0) StageLifecycleAPI::dComIfG_deleteObjectRes(gabi::ea(dStage_roomControlStatics::mDemoArcName));
    u32 stage = gabi::ea(&gabi::at<dComIfG_inf_c>(dComIfGp_ea())->play.mStageData);
    u32 stag = gabi::call_ptr<u32>(gabi::at<dStage_dt_c>(stage)->__vtbl->getStagInfo.function.get(), stage);
    u32 stageNo = (gabi::at<stage_stag_info_class>(stag)->mProp >> 1) & 0x7F;
    StageLifecycleAPI::dSv_info_c_putSave(gabi::ea(dComIfGs_info()), stageNo);
    StageLifecycleAPI::dComIfG_deleteStageRes(gabi::guest_str("Stage")); /* "Stage" */
    StageLifecycleAPI::dMap_c_remove();
    StageLifecycleAPI::dEvent_manager_c_remove(gabi::ea(&gabi::at<dComIfG_inf_c>(dComIfGp_ea())->play.mEvtManager));
}

// HD: dComIfGs_removeZone inlined
/* 025C33E8 */
static void dStage_roomControl_c_zoneCountCheck(u32 self, s32 i_roomNo) {
    dStage_roomStatus_c* status = dStage_roomControlStatics::mStatus;
    for (u32 i = 0x40; i != 0; i--, ++status) {
        s32 zone = (s8)status->mZoneNo;
        if (zone < 0) continue;
        if ((s8)status->mZoneCount <= 0) continue;
        StageLifecycleAPI::dSv_zoneBit_c_clearRoomSwitch(dStage_getSaveBase() + 0x7C8 + zone * 0x4C + 2);
        if ((u32)i_roomNo == (u32)(s32)(s8)dStage_roomControlStatics::mOldStayNo) continue;
        u8 cnt = (u8)(status->mZoneCount - 1);
        status->mZoneCount = cnt;
        if ((s8)cnt != 0) continue;
        s32 z = (s8)status->mZoneNo;
        gabi::store<u8>(dStage_getSaveBase() + 0x7C8 + z * 0x4C, 0xFF);
        status->mZoneNo = 0xFF;
    }
    StageLifecycleAPI::setStayNo(i_roomNo);
}

// HD: stayRoomCheck inlined. HD-changed: every room of the list that is not loaded yet gets a room scene (the GameCube code returned after the first one)
/* 025C34AC */
static BOOL dStage_roomControl_c_loadRoom(u32 self, s32 roomCount, u32 rooms) {
    for (u32 i = 0; i < 64; ++i)
        if (dStage_roomControlStatics::mStatus[i].mFlags & 6) return FALSE;
    BOOL ok = TRUE;
    for (u32 roomNo = 0; roomNo < 64; ++roomNo) {
        u8 flag = dStage_roomControlStatics::mStatus[roomNo].mFlags;
        if (!(flag & 1)) continue;
        bool found = false;
        if (roomCount > 0) {
            for (u32 k = 0; k < (u32)roomCount; k++) {
                if (roomNo == (u32)(gabi::load<u8>(rooms + k) & 0x3F)) {
                    found = true;
                    break;
                }
            }
        }
        if (found) continue;
        dStage_roomControlStatics::mStatus[roomNo].mFlags = flag | 4;
        ok = FALSE;
    }
    if (!ok) return FALSE;
    if (roomCount <= 0) return TRUE;
    for (u32 k = 0; k < (u32)roomCount; k++) {
        u32 roomNo = gabi::load<u8>(rooms + k) & 0x3F;
        dStage_roomStatus_c* status = &dStage_roomControlStatics::mStatus[roomNo];
        u8 flag = status->mFlags;
        status->mZoneCount = 2;
        if (flag & 1) continue;
        if (StageLifecycleAPI::createRoomScene((s32)roomNo) == 0) continue;
        u8 b = gabi::load<u8>(rooms + k);
        u8 cur = status->mFlags;
        status->mFlags = cur | ((b & 0x80) ? 2 : 0xA);
    }
    return TRUE;
}

// HD: the time pass is the static 101EBFD8; a room change bumps the HD counter
/* 025C35E8 */
static s32 dStage_RoomCheck(u32 i_gndChk) {
    u32 p = dComIfGp_ea();
    s32 roomId = StageLifecycleAPI::dBgS_GetRoomId(gabi::ea(&gabi::at<dComIfG_inf_c>(p)->play.mBgS), i_gndChk ? i_gndChk + 0x14 : 0);
    if (roomId < 0) return 0;
    if ((u32)roomId != (u32)(s32)(s8)dStage_roomControlStatics::mStayNo) {
        dStage_roomControl_c_zoneCountCheck(gabi::ea(&gabi::at<dComIfG_inf_c>(dComIfGp_ea())->play.mRoomCtrl), roomId);
        dStage_roomControlStatics::mRoomChangeCount = dStage_roomControlStatics::mRoomChangeCount + 1;
    }
    p = dComIfGp_ea();
    u32 grp = StageLifecycleAPI::dBgS_GetGrpRoomInfId(gabi::ea(&gabi::at<dComIfG_inf_c>(p)->play.mBgS), i_gndChk ? i_gndChk + 0x14 : 0);
    if (grp == 0xFF) return 0;
    u32 stage = gabi::ea(&gabi::at<dComIfG_inf_c>(dComIfGp_ea())->play.mStageData);
    u32 room = gabi::call_ptr<u32>(gabi::at<dStage_dt_c>(stage)->__vtbl->getRoom.function.get(), stage);
    if (room == 0 || (s32)gabi::at<roomRead_class>(room)->num <= (s32)grp) return 1;
    u32 tp = gabi::at<roomRead_class>(room)->m_entries.get()[grp]->field_0x2 & 3; /* dStage_roomRead_dt_c_GetTimePass */
    dComIfGp_ea();
    l_timePass = (u8)tp;
    u32 e = gabi::ea(gabi::at<roomRead_class>(room)->m_entries.get()[grp].get());
    u32 list = gabi::ea(gabi::at<roomRead_data_class>(e)->m_rooms.get());
    u32 num = gabi::at<roomRead_data_class>(e)->num;
    return dStage_roomControl_c_loadRoom(gabi::ea(&gabi::at<dComIfG_inf_c>(dComIfGp_ea())->play.mRoomCtrl), (s32)num, list);
}

/* 025C3748 */
static s32 dStage_changeScene(s32 i_exitId, f32 speed, u32 mode, s32 room_no) {
    u32 scls;
    if (room_no == -1) {
        u32 stage = gabi::ea(&gabi::at<dComIfG_inf_c>(dComIfGp_ea())->play.mStageData);
        scls = gabi::call_ptr<u32>(gabi::at<dStage_dt_c>(stage)->__vtbl->getSclsInfo.function.get(), stage);
    } else {
        if ((u32)room_no >= 0x40) StageLifecycleAPI::reportAssert("d_stage.cpp", 0x1476, "0 <= room_no && room_no < 64");
        u32 rd = StageLifecycleAPI::getStatusRoomDt(gabi::ea(&gabi::at<dComIfG_inf_c>(dComIfGp_ea())->play.mRoomCtrl), room_no);
        scls = gabi::call_ptr<u32>(gabi::at<dStage_dt_c>(rd)->__vtbl->getSclsInfo.function.get(), rd);
    }
    if (scls == 0) return 0;
    if (i_exitId < 0 || i_exitId >= (s32)gabi::at<stage_scls_info_dummy_class>(scls)->num) StageLifecycleAPI::reportAssert("d_stage.cpp", 0x1480, "0 <= i_exitId && i_exitId < scls->num");
    u32 e = gabi::ea(gabi::at<stage_scls_info_dummy_class>(scls)->m_entries.get()) + i_exitId * 0xC;
    u32 wipe = gabi::at<stage_scls_info_class>(e)->mWipe & 0xF; /* dStage_sclsInfo_getWipe */
    if (wipe == 0xF) wipe = 0;
    u32 room = (u32)(s32)(s8)gabi::at<stage_scls_info_class>(e)->mRoom;
    u32 start = gabi::at<stage_scls_info_class>(e)->mStart;
    StageLifecycleAPI::dComIfGp_setNextStage(e, start, room, (u32)-1, speed, mode, 1, wipe);
    return 1;
}

// HD: IkadaGetRoomNoArg0/IkadaGetLinkIdArg1/IkadaGetIkadaIdArg2 and the stage name compare inlined
/* 025C38B8 */
static s32 dStage_changeSceneExitId(u32 i_poly, f32 i_speed, u32 i_mode, s32 i_roomNo) {
    u32 p = dComIfGp_ea();
    s32 exit_id = StageLifecycleAPI::dBgS_GetExitId(gabi::ea(&gabi::at<dComIfG_inf_c>(p)->play.mBgS), i_poly);
    if (exit_id == 0x3E || exit_id == 0x3B) {
        p = dComIfGp_ea();
        u32 actor = StageLifecycleAPI::cBgS_GetActorPointer(gabi::ea(&gabi::at<dComIfG_inf_c>(p)->play.mBgS), gabi::at<cBgS_PolyInfo>(i_poly)->mBgIndex);
        u32 prm = gabi::at<fopAc_ac_c>(actor)->mParameters;
        u32 point = (prm >> 10) & 0xFF;
        u32 roomNo = (u32)(s32)(s8)((prm >> 4) & 0x3F);
        if (exit_id == 0x3E) {
            StageLifecycleAPI::dComIfGp_setNextStage(gabi::guest_str("Obshop"), point, roomNo, (u32)-1, i_speed, i_mode, 1, 0); /* "Obshop" */
        } else if (exit_id == 0x3B) {
            StageLifecycleAPI::dComIfGp_setNextStage(gabi::guest_str("Abship"), point, roomNo, (u32)-1, i_speed, i_mode, 1, 0); /* "Abship" */
        }
        gabi::at<dComIfG_inf_c>(dComIfGp_ea())->play.mIkadaShipBeforeRoomId = (u8)i_roomNo; /* IkadaShipBeforeRoomId */
        u32 id = (gabi::at<fopAc_ac_c>(actor)->mParameters >> 18) & 0xFF;
        gabi::at<dComIfG_inf_c>(dComIfGp_ea())->play.mIkadaShipId = (u8)id; /* IkadaShipId */
        p = dComIfGp_ea();
        gabi::at<fopAc_vectorWordsHD>(gabi::ea(&gabi::at<dComIfG_inf_c>(p)->play.mIkadaLinkPos))->position[0] = gabi::at<fopAc_vectorWordsHD>(gabi::ea(&gabi::at<fopAc_ac_c>(actor)->current.pos))->position[0]; /* IkadaShipBeforePos = current.pos */
        gabi::at<fopAc_vectorWordsHD>(gabi::ea(&gabi::at<dComIfG_inf_c>(p)->play.mIkadaLinkPos))->position[1] = gabi::at<fopAc_vectorWordsHD>(gabi::ea(&gabi::at<fopAc_ac_c>(actor)->current.pos))->position[1];
        gabi::at<fopAc_vectorWordsHD>(gabi::ea(&gabi::at<dComIfG_inf_c>(p)->play.mIkadaLinkPos))->position[2] = gabi::at<fopAc_vectorWordsHD>(gabi::ea(&gabi::at<fopAc_ac_c>(actor)->current.pos))->position[2];
        return 1;
    }
    if (exit_id == 0x3D) {
        dComIfGp_ea();
        p = dComIfGp_ea();
        if (gabi::at<dComIfG_inf_c>(p)->play.mIkadaShipBeforeRoomId >= 0x40) StageLifecycleAPI::reportAssert("d_stage.cpp", 0x143B, "dComIfGp_getIkadaShipBeforeRoomId() >= 0 && dComIfGp_getIkadaShipBeforeRoomId() < 64");
        p = dComIfGp_ea();
        u32 room = (u32)(s32)(s8)gabi::at<dComIfG_inf_c>(p)->play.mIkadaShipBeforeRoomId;
        StageLifecycleAPI::dComIfGp_setNextStage(gabi::guest_str("sea"), (u32)-2, room, (u32)-1, i_speed, i_mode, 1, 0); /* "sea" */
        return 1;
    }
    if (exit_id == 0x3C) {
        gabi::Local<SafeString> a;
        gabi::Local<SafeString> b;
        a->mStringTop = gabi::guest_str("Asoko"); /* "Asoko" */
        a->__vtbl = gabi::ea(&dStage_safeStringVtable);
        p = dComIfGp_ea();
        b->__vtbl = gabi::ea(&dStage_safeStringVtable);
        b->mStringTop = gabi::ea(&gabi::at<dComIfG_inf_c>(p)->play.mCurStage);
        if (dStage_isEqual(a, b)) {
            if (!dComIfGs_isEventBit(dStageEvent::UNK_0808)) {
                StageLifecycleAPI::dComIfGp_setNextStage(gabi::guest_str("A_umikz"), 0, 0, (u32)-1, i_speed, i_mode, 1, 0); /* "A_umikz" */
            } else if (dComIfGs_isEventBit(dStageEvent::UNK_0520)) {
                StageLifecycleAPI::dComIfGp_setNextStage(gabi::guest_str("sea"), 5, 0xB, (u32)-1, i_speed, i_mode, 1, 0); /* "sea", Windfall */
            } else {
                StageLifecycleAPI::dComIfGp_setNextStage(gabi::guest_str("MajyuE"), 0x12, 0, (u32)-1, i_speed, i_mode, 1, 0); /* "MajyuE" */
            }
        } else {
            StageLifecycleAPI::dComIfGp_setNextStage(gabi::ea(l_asokoExitName.mText), 0, 0, (u32)-1, i_speed, i_mode, 1, 0); /* "Asoko" */
        }
        return 1;
    }
    return dStage_changeScene(exit_id, i_speed, i_mode, i_roomNo);
}

/* 025C3D68 */
static void dStage_restartRoom(u32 roomParam, u32 mode) {
    u32 p = dComIfGp_ea();
    u32 room = (u32)(s32)(s8)dComIfGs_info()->mRestart.mRestartRoom; /* dComIfGs_getRestartRoomNo */
    StageLifecycleAPI::dComIfGp_setNextStage(gabi::ea(&gabi::at<dComIfG_inf_c>(p)->play.mCurStage), (u32)-1, room, (u32)-1, l_zero, mode, 0, 0);
    dComIfGs_info()->mRestart.mRestartParam = roomParam; /* dComIfGs_setRestartRoomParam */
}

// HD: the day counts up for hours 6..23 (an unsigned range check)
/* 025C3DE4 */
static void dStage_turnRestart() {
    s32 layerNo = (s8)dComIfGp_getStartStageLayer();
    if (layerNo >= 0) layerNo = (s8)(layerNo ^ 1);
    u32 p = dComIfGp_ea();
    f32 zero = l_zero;
    u32 room = (u32)(s32)(s8)dComIfGs_info()->mTurnRestart.mRoomNo; /* dComIfGs_getTurnRestartRoomNo */
    StageLifecycleAPI::dComIfGp_setNextStage(gabi::ea(&gabi::at<dComIfG_inf_c>(p)->play.mCurStage), (u32)-3, room, (u32)layerNo, zero, 0, 0, 6);
    s32 hour = StageLifecycleAPI::dKy_getdaytime_hour();
    u32 h6 = (u32)(hour - 6);
    f32 nextTime = l_restartMorning; /* 180.0 */
    if (h6 < 12) nextTime = zero;
    if (h6 < 18) {
        u32 s = dStage_getSaveBase();
        gabi::at<dSv_info_c>(s + 0x20)->mSavedata.mPlayer.mPlayerStatusB.mDate = (u16)(gabi::at<dSv_info_c>(s + 0x20)->mSavedata.mPlayer.mPlayerStatusB.mDate + 1); /* dComIfGs_setDate(getDate() + 1) */
        StageLifecycleAPI::dKy_DayProc();
    }
    StageLifecycleAPI::dKy_set_nexttime(nextTime);
}

// HD: dComIfG_TimerDeleteRequest inlined
/* 025C3ED0 */
static void dStage_escapeRestart() {
    u32 player = gabi::ea(dComIfGp_getLinkPlayer());
    u32 angle = (u32)(s32)(s16)gabi::at<fopAc_ac_c>(player)->shape_angle.y;
    u32 pos = gabi::ea(&gabi::at<fopAc_ac_c>(player)->current.pos);
    u32 room = (u32)(s32)(s8)gabi::at<fopAc_ac_c>(player)->current.roomNo;
    u32 dn = StageLifecycleAPI::daPy_lk_c_getDayNightParamData(player);
    StageLifecycleAPI::dSv_turnRestart_c_set(gabi::ea(&dComIfGs_info()->mTurnRestart), pos, angle, room, dn, pos, angle, 0);
    if (gabi::at<dComIfG_inf_c>(dComIfGp_ea())->play.mTimerInfo.mTimerMode == 3) {
        u32 timer = gabi::at<dComIfG_inf_c>(dComIfGp_ea())->play.mTimerInfo.mTimerPtr;
        if (timer != 0) StageLifecycleAPI::dTimer_c_deleteRequest(timer);
    }
    u32 p = dComIfGp_ea();
    u32 r = (u32)(s32)(s8)dComIfGs_info()->mTurnRestart.mRoomNo;
    StageLifecycleAPI::dComIfGp_setNextStage(gabi::ea(&gabi::at<dComIfG_inf_c>(p)->play.mCurStage), (u32)-3, r, (u32)-1, l_zero, 0, 0, 9);
}

// HD: dStage_nextStage_c::dStage_nextStage_c() (0xE bytes)
/* 025C3FAC */
static u32 dStage_nextStage_c_ct(u32 self) {
    if (self == 0) {
        self = StageLifecycleAPI::operator_new(0xE);
        if (self == 0) return 0;
    }
    gabi::at<dStage_nextStage_c>(self)->wipe = 0; /* wipe */
    gabi::at<dStage_nextStage_c>(self)->enable = 0; /* enable */
    return self;
}

// HD: dStage_nextStage_c::set(name, roomNo, point, layer, wipe)
/* 025C3FEC */
static s32 dStage_nextStage_c_set(u32 self, u32 i_name, u32 i_roomNo, u32 i_point, u32 i_layer, u32 i_wipe) {
    if ((s8)gabi::at<dStage_nextStage_c>(self)->enable != 0) return 0;
    gabi::at<dStage_nextStage_c>(self)->wipe = (u8)i_wipe;
    gabi::at<dStage_nextStage_c>(self)->enable = 1;
    StageLifecycleAPI::dStage_startStage_c_set(self, i_name, i_roomNo, i_point, i_layer);
    return 1;
}

// HD: dStage_roomDt_c::dStage_roomDt_c() (HD 0x54 bytes, vtable 10055B90)
/* 025C4048 */
static u32 dStage_roomDt_c_ct(u32 self) {
    if (self == 0) {
        self = StageLifecycleAPI::operator_new(0x54);
        if (self == 0) return 0;
    }
    dStage_roomDt_c* room = gabi::at<dStage_roomDt_c>(self);
    room->__vtbl = gabi::ea(&l_roomVtable);
    room->mpLightVecInfo = nullptr;
    room->mLightVecInfoNum = 0;
    room->mpMapInfo = nullptr;
    room->mpMapInfoBase = nullptr;
    room->mpVrbox = nullptr;
    room->mpFileList = nullptr;
    room->mpShip = nullptr;
    room->mpPlayer = nullptr;
    room->mpPoint2 = nullptr;
    room->mpPath2 = nullptr;
    room->mpCamera = nullptr;
    room->mpArrow = nullptr;
    room->mpSound = nullptr;
    room->mpScls = nullptr;
    room->mpLbnk = nullptr;
    room->mpTresure = nullptr;
    room->mpDrTg = nullptr;
    room->mpFloor = nullptr;
    room->mVrboxNum = 0;
    room->mRoomNo = 0;
    room->mPlayerNum = 0;
    return self;
}

// HD: dStage_roomDt_c::getMapInfo2(int) (unnamed by the matcher; vtable slot +0x7C)
/* 025C40E0 */
u32 dStage_roomDt_c::getMapInfo2(u32 i_no) {
    const u32 self = gabi::ea(this);
    u32 base = gabi::call_ptr<u32>(gabi::at<dStage_dt_c>(self)->__vtbl->getMapInfoBase.function.get(), self);
    if (base == 0) return 0;
    u32 n = gabi::at<stage_map_info_dummy_class>(base)->num;
    if (n == 0) return 0;
    u32 e = gabi::ea(gabi::at<stage_map_info_dummy_class>(base)->m_entries.get());
    if (e == 0) return 0;
    if ((s32)n <= 0) return 0;
    for (; n != 0; n--, e += 0x38)
        if (i_no == gabi::at<stage_map_info_class>(e)->field_0x35) return e;
    return 0;
}

// HD: dStage_roomStatus_c::dStage_roomStatus_c() (0x22C bytes): the room data, then three copies of a 0x44-byte light template (1016E414, probably a light influence)
/* 025C415C */
static u32 dStage_roomStatus_c_ct(u32 self) {
    if (self == 0) {
        self = StageLifecycleAPI::operator_new(0x22C);
        if (self == 0) return 0;
    }
    dStage_roomDt_c_ct(self);
    dStage_roomStatus_c* status = gabi::at<dStage_roomStatus_c>(self);
    StageLifecycleAPI::memclr(gabi::ea(&status->mTevStr), sizeof(status->mTevStr));
    // HD: three enlarged light slots use the same scalar copy order as GHS.
    fopAc_tevInitHD* lighting = gabi::at<fopAc_tevInitHD>(gabi::ea(&status->mTevStr));
    for (u32 slot = 0; slot < 3; ++slot) {
        fopAc_lightBlockHD* light = slot == 0 ? &lighting->mLightObj.mDefault : &lighting->mHdLightParam[slot - 1].mDefault;
        for (u32 i = 0; i < 6; ++i) light->positionAndDirection[i] = l_defaultLight.positionAndDirection[i];
        for (u32 i = 0; i < 4; ++i) light->color[i] = l_defaultLight.color[i];
        for (u32 i = 0; i < 4; ++i) light->packedParameters[i] = l_defaultLight.packedParameters[i];
        for (u32 i = 0; i < 8; ++i) light->factors[i] = l_defaultLight.factors[i];
    }
    gabi::at<dStage_roomStatus_c>(self)->mFlags = 0;
    gabi::at<dStage_roomStatus_c>(self)->mDraw = 0;
    gabi::at<dStage_roomStatus_c>(self)->mZoneCount = 0;
    gabi::at<dStage_roomStatus_c>(self)->mZoneNo = 0;
    gabi::at<dStage_roomStatus_c>(self)->mMemBlockID = 0;
    gabi::at<dStage_roomStatus_c>(self)->mProcID = 0;
    gabi::at<dStage_roomStatus_c>(self)->mpBgW = nullptr;
    return self;
}

/* 025C4334 */
static BOOL dStage_roomControl_c_checkRoomDisp(u32 self, s32 i_roomNo) {
    u8 f = dStage_roomControlStatics::mStatus[i_roomNo].mFlags;
    if (f & 8) return FALSE;
    return (f & 0x10) ? TRUE : FALSE;
}

// HD: no spot models around the player in dark rooms (only the colour-set blend remains); the dark statuses are the static table 101EE5C8 (8 entries of 0x20)
/* 025C4364 */
static void dStage_roomControl_c_checkDrawArea(u32 self) {
    if ((s8)dStage_roomControlStatics::mStayNo < 0) return;
    if (gabi::ea(dComIfGp_getPlayer(0)) == 0) return; /* dComIfGp_getPlayer(0) */
    s32 stay = (s8)dStage_roomControlStatics::mStayNo;
    u32 plist = gabi::ea(dStage_roomControlStatics::mStatus[stay].mRoomDt.mpFileList.get()); /* mpFileList */
    u32 target = 0xFF;
    if (plist == 0) return;
    u32 w = gabi::at<dStage_FileList_dt_c>(plist)->mParam;
    u32 dark = (w >> 3) & 0xF; /* dStage_FileList_dt_DarkNo */
    if (dark >= 8) {
        StageLifecycleAPI::reportAssert("d_stage.h", 0x971, "i_no >= 0 && i_no < DARK_DATA_MAX");
        w = gabi::at<dStage_FileList_dt_c>(plist)->mParam;
    }
    u32 darkOn = w & 1;
    if (darkOn) target = l_darkStatus.mEntries[dark].mEnvAlpha;
    StageLifecycleAPI::cLib_chaseUC(gabi::ea(&dStage_roomControlStatics::mDarkRatio), target, 8);
    u32 r = dStage_roomControlStatics::mDarkRatio;
    if (r == 0xFF) return;
    f32 ratio = (f32)r * l_darkRatioScale; /* 1/128 */
    f32 one = l_one;
    if (!(ratio <= one)) ratio = one;
    if (darkOn) {
        if (r != target) return;
        StageLifecycleAPI::dKy_change_colset(0, 4, one - ratio);
    } else {
        StageLifecycleAPI::dKy_change_colset(4, 0, ratio);
    }
}

// HD: dStage_roomControl_c::getDarkStatus() (unnamed by the matcher)
/* 025C44AC */
static u32 dStage_roomControl_c_getDarkStatus() {
    s32 stay = (s8)dStage_roomControlStatics::mStayNo;
    u32 plist = gabi::ea(dStage_roomControlStatics::mStatus[stay].mRoomDt.mpFileList.get());
    if (plist == 0) return 0;
    u32 idx = (gabi::at<dStage_FileList_dt_c>(plist)->mParam >> 3) & 0xF;
    if (idx >= 8) StageLifecycleAPI::reportAssert("d_stage.h", 0x971, "i_no >= 0 && i_no < DARK_DATA_MAX");
    return gabi::ea(&l_darkStatus.mEntries[idx]);
}

// HD: dStage_stageDt_c::dStage_stageDt_c() (HD 0x7C bytes, vtable 10055DE0)
/* 025C4548 */
static u32 dStage_stageDt_c_ct(u32 self) {
    if (self == 0) {
        self = StageLifecycleAPI::operator_new(0x7C);
        if (self == 0) return 0;
    }
    dStage_stageDt_c* stage = gabi::at<dStage_stageDt_c>(self);
    stage->__vtbl = gabi::ea(&l_stageVtable);
    stage->mpCamera = nullptr;
    stage->mpArrow = nullptr;
    stage->mpPlayer = nullptr;
    stage->mpRoomReads = nullptr;
    stage->mpMapInfo = nullptr;
    stage->mpMapInfoBase = nullptr;
    stage->mpPaletInfo = nullptr;
    stage->mpPselectInfo = nullptr;
    stage->mpEnvrInfo = nullptr;
    stage->mpVrboxInfo = nullptr;
    stage->mpPlightInfo = nullptr;
    stage->mPaletInfoNum = 0;
    stage->mPselectInfoNum = 0;
    stage->mEnvrInfoNum = 0;
    stage->mVrboxInfoNum = 0;
    stage->mPlightInfoNum = 0;
    stage->mpStagInfo = nullptr;
    stage->mpScls = nullptr;
    stage->mpPnt = nullptr;
    stage->mpPath = nullptr;
    stage->mpPnt2 = nullptr;
    stage->mpPath2 = nullptr;
    stage->mpSoundInfo = nullptr;
    stage->mpEventInfo = nullptr;
    stage->mpFloorInfo = nullptr;
    stage->mpMulti = nullptr;
    stage->mpTresure = nullptr;
    stage->mpDMap = nullptr;
    stage->mpDrTg = nullptr;
    stage->mPlayerNum = 0;
    return self;
}

// HD: an empty deleting destructor emitted in this unit (called from 02524808 for the play object's member at +0x3EB0)
/* 025C4604 */
static void dStage_deleteEmptyMember(u32 self, u32 del) {
    if (self == 0) return;
    if (!(del & 1)) return;
    StageLifecycleAPI::operator_delete(self);
}

// HD: dStage_stageDt_c::init() (vtable slot +0xC)
/* 025C4618 */
void dStage_stageDt_c::init() {
    mpCamera = nullptr;
    mpArrow = nullptr;
    mpPlayer = nullptr;
    mpRoomReads = nullptr;
    mpMapInfo = nullptr;
    mpMapInfoBase = nullptr;
    mpPaletInfo = nullptr;
    mpPselectInfo = nullptr;
    mpEnvrInfo = nullptr;
    mpVrboxInfo = nullptr;
    mpPlightInfo = nullptr;
    mPlightInfoNum = 0;
    mpStagInfo = nullptr;
    mpScls = nullptr;
    mpPnt = nullptr;
    mpPath = nullptr;
    mpPnt2 = nullptr;
    mpPath2 = nullptr;
    mpSoundInfo = nullptr;
    mpEventInfo = nullptr;
    mpFloorInfo = nullptr;
    mpMulti = nullptr;
    mpTresure = nullptr;
    mpDMap = nullptr;
    mpDrTg = nullptr;
}

// HD: __sinit_d_stage_cpp
/* 025C4684 */
static void __sinit_d_stage_cpp() {
    l_runtimeStatics.mStorage[2] = 0;
    l_runtimeStatics.mStorage[0] = 0;
    l_runtimeStatics.mStorage[3] = 0;
    l_runtimeStatics.mStorage[1] = 0;
    StageLifecycleAPI::__register_global_object(gabi::ea(l_registrationNodes.mEntries[0]));
    l_runtimeStatics.mMinAngle = l_initialMinAngle; /* -pi */
    l_runtimeStatics.mMaxAngle = l_initialMaxAngle; /* pi */
    StageLifecycleAPI::constructByteObjectA(gabi::ea(&l_runtimeStatics.mByteObjectA));
    StageLifecycleAPI::__register_global_object(gabi::ea(l_registrationNodes.mEntries[1]));
    StageLifecycleAPI::constructByteObjectB(gabi::ea(&l_runtimeStatics.mByteObjectB));
    StageLifecycleAPI::__register_global_object(gabi::ea(l_registrationNodes.mEntries[2]));
    StageLifecycleAPI::__construct_array(gabi::ea(dStage_roomControlStatics::mStatus), 0x40, 0x22C, 0x025C415C); /* dStage_roomControlStatics::mStatus */
}

/* WWHD glue (tools/verify/autoglue.py): private ABI bindings
data l_stageInitFunctions 101EBAD4
data l_layerFunctions 101EBAE0
data l_stageFunctions 101EBCC0
data l_roomFunctions 101EBE58
data l_roomReloadFunctions 101EBF60
data l_roomLoaderVtable 100553AC
data l_darkStatus 101EE5C8
data l_registrationNodes 101EBFB4
data l_runtimeStatics 1047D994
data l_asokoExitName 10055A8C
data l_zero 1005592C
data l_restartMorning 10055B10
data l_darkRatioScale 10055B20
data l_one 10055B24
data l_initialMinAngle 10055B88
data l_initialMaxAngle 10055B8C
data l_timePass 101EBFD8
str 100559A0 in 025C2EEC
str 100559B4 in 025C2EEC
str 100559D4 in 025C3094
str 100559F8 in 025C3094
str 10055A18 in 025C3094
str 10055A24 in 025C3370
str 10055A80 in 025C38B8
str 10055A88 in 025C38B8
str 10055A94 in 025C38B8
str 10055A9C in 025C38B8
str 10055AA4 in 025C38B8

data l_roomVtable 10055B90
data l_stageVtable 10055DE0
data l_defaultLight 1016E414
str 10055974 in 025C2CA4
str 10055980 in 025C2CA4
str 10055990 in 025C2CA4
str 100559A8 in 025C2EEC
str 100559C0 in 025C2EEC
str 100559DC in 025C3094
str 100559E4 in 025C3094
str 100559EC in 025C3094
str 10055A04 in 025C3094
str 10055A2C in 025C3748
str 10055A38 in 025C3748
str 10055A60 in 025C3748
str 10055A8C in 025C38B8
str 10055AAC in 025C38B8
str 10055AB8 in 025C38B8
str 10055B28 in 025C4364
str 10055B34 in 025C4364
str 10055B58 in 025C44AC
str 10055B64 in 025C44AC
ext 0273AA24 void StageLifecycleAPI::reportAssert(const char* file, s32 line, const char* msg)
ext 025250C0 s32 StageLifecycleAPI::dComIfG_play_c_getLayerNo(s32 roomNo)
ext 0252447C u32 StageLifecycleAPI::dComIfG_getStageRes(u32 arc, u32 res)
ext 02524180 void StageLifecycleAPI::dComIfG_deleteStageRes(u32 arc)
ext 02520488 void StageLifecycleAPI::dComIfG_deleteObjectRes(u32 arc)
ext 02034434 void StageLifecycleAPI::dumpCtx_ct(u32 obj)
ext 02034538 void StageLifecycleAPI::dumpCtx_dt(u32 obj, s32 del)
ext 025DAD4C void StageLifecycleAPI::fopKyM_Create(s32 proc, u32 a1, u32 a2)
ext 025B99DC void StageLifecycleAPI::dSv_info_c_initZone(u32 info)
ext 025B93B0 void StageLifecycleAPI::dSv_zoneBit_c_clearRoomSwitch(u32 zone)
ext 025B9D24 void StageLifecycleAPI::dSv_info_c_putSave(u32 info, s32 stageNo)
ext 0258EFC8 void StageLifecycleAPI::dMap_c_create()
ext 02590FB0 void StageLifecycleAPI::dMap_c_remove()
ext 025DED64 u32 StageLifecycleAPI::fpcLy_CurrentLayer()
ext 025E14A8 u32 StageLifecycleAPI::fpcSCtRq_Request(u32 layer, s32 proc, u32 a2, u32 a3, u32 prm)
ext 02544488 void StageLifecycleAPI::dEvent_manager_c_create(u32 mng)
ext 025444F0 void StageLifecycleAPI::dEvent_manager_c_remove(u32 mng)
ext 025C1004 s32 StageLifecycleAPI::createRoomScene(s32 roomNo)
ext 025C1070 void StageLifecycleAPI::setStayNo(s32 roomNo)
ext 025C11DC u32 StageLifecycleAPI::getStatusRoomDt(u32 self, s32 no)
ext 025C1328 void StageLifecycleAPI::dStage_startStage_c_set(u32 self, u32 name, u32 roomNo, u32 point, u32 layer)
ext 024EF130 s32 StageLifecycleAPI::dBgS_GetRoomId(u32 bgs, u32 poly)
ext 024EECE8 u32 StageLifecycleAPI::dBgS_GetGrpRoomInfId(u32 bgs, u32 poly)
ext 024EECC8 s32 StageLifecycleAPI::dBgS_GetExitId(u32 bgs, u32 poly)
ext 02008438 u32 StageLifecycleAPI::cBgS_GetActorPointer(u32 bgs, u32 bgIndex)
ext 0252012C void StageLifecycleAPI::dComIfGp_setNextStage(u32 name, u32 point, u32 roomNo, u32 layer, f32 speed, u32 mode, u32 enable, u32 wipe)
ext 025B8B94 BOOL StageLifecycleAPI::dSv_event_c_isEventBit(u32 ev, u32 flag)
ext 02556C34 s32 StageLifecycleAPI::dKy_getdaytime_hour()
ext 02560768 void StageLifecycleAPI::dKy_DayProc()
ext 02560728 void StageLifecycleAPI::dKy_set_nexttime(f32 t)
ext 0243B188 u32 StageLifecycleAPI::daPy_lk_c_getDayNightParamData(u32 pl)
ext 025B998C void StageLifecycleAPI::dSv_turnRestart_c_set(u32 self, u32 pos, u32 angle, u32 roomNo, u32 param, u32 shipPos, u32 shipAngle, u32 a7)
ext 025C58D8 void StageLifecycleAPI::dTimer_c_deleteRequest(u32 timer)
ext 0200F4FC void StageLifecycleAPI::cLib_chaseUC(u32 p, u32 target, u32 step)
ext 0255FCEC void StageLifecycleAPI::dKy_change_colset(u32 a, u32 b, f32 ratio)
ext 0273AD10 u32 StageLifecycleAPI::operator_new(u32 size)
ext 0273AF40 void StageLifecycleAPI::operator_delete(u32 p)
ext 028F521C void StageLifecycleAPI::memclr(u32 p, u32 size)
ext 028F026C void StageLifecycleAPI::__register_global_object(u32 node)
ext 028ED6F8 void StageLifecycleAPI::constructByteObjectA(u32 obj)
ext 028EAB2C void StageLifecycleAPI::constructByteObjectB(u32 obj)
ext 028EFFD0 void StageLifecycleAPI::__construct_array(u32 p, u32 n, u32 size, u32 ctor)
*/
