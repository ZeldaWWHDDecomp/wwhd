/** Player spawning and the first stage-chunk handlers. Private decomp source.
 * HD: raft/restart modes are inlined, a missing spawn point selects the first player,
 * and the meter is created without an AGB actor. Integer copies preserve float bits.
 */
#include "d/d_stage.h"

// HD: the inlined raft assertion uses its own "d_stage.cpp" string copy.
static const dStage_sourceFileName l_ikadaSourceFile;
static const dStage_actorTagTable l_actorTags;
static inline u32 dStage_getSaveBase() { return gabi::ea(dComIfGs_info()) - 0x20; }

/* inline strcmp(a, b) == 0 */
static bool dStage_nameMatches(u32 a, const char* name) {
    u32 b = gabi::guest_str(name);
    for (;;) {
        u8 ca = gabi::load<u8>(a), cb = gabi::load<u8>(b);
        if (ca != cb) return false;
        if (ca == 0) return true;
        a++;
        b++;
    }
}

/* the common end of dStage_playerInit's four start modes */
static s32 playerInit_tail(u32 appen, u32 player_data) {
    dComIfGs_info()->mRestart.mRestartParam = 0; /* dComIfGs_setRestartRoomParam(0) */
    gabi::at<fopAcM_prm_class>(appen)->room_no = 0xFF;
    gabi::at<fopAcM_prm_class>(appen)->base.setID = 0xFFFF;
    u32 stage = gabi::ea(&gabi::at<dComIfG_inf_c>(dComIfGp_ea())->play.mCurStage);
    u32 name = gabi::ea(&gabi::at<dComIfG_inf_c>(dComIfGp_ea())->play.mCurStage);
    s32 point = (s16)dComIfGp_getStartStagePoint();
    u32 p = dComIfGp_ea();
    s32 layer = (s8)gabi::at<dComIfG_inf_c>(p)->play.mCurStage.mLayer;
    StagePlayerAPI::dStage_startStage_c_set(stage, name, gabi::at<fopAcM_prm_class>(appen)->base.parameters & 0x3F, point, layer);
    u32 shipId = (gabi::at<fopAcM_prm_class>(appen)->base.angle.z >> 8) & 0xFF;
    gabi::at<dComIfG_inf_c>(dComIfGp_ea())->play.mShipId = (u8)shipId;
    u32 shipRoom = gabi::at<fopAcM_prm_class>(appen)->base.parameters & 0x3F;
    gabi::at<dComIfG_inf_c>(dComIfGp_ea())->play.mShipRoomId = (u8)shipRoom;
    StagePlayerAPI::dStage_actorCreate(player_data, appen);
    u32 proc = StagePlayerAPI::fopScnM_SearchByID(dStage_roomControlStatics::mProcID); /* dStage_roomControlStatics::mProcID */
    if (proc == 0) {
        StagePlayerAPI::reportAssert("d_stage.cpp", 0x7BA, "stageProc != (0)");
        StagePlayerAPI::fopAcM_create(dStageProcess::fpcNm_TITLE_e, 0, 0, -1, 0, 0, -1, 0); /* TITLE */
    } else if ((s16)gabi::at<base_process_class>(proc)->mProcName != dStageProcess::fpcNm_PLAY_SCENE_e) {
        StagePlayerAPI::fopAcM_create(dStageProcess::fpcNm_TITLE_e, 0, 0, -1, 0, 0, -1, 0);
    }
    /* fopMsgM_Create(METER): HD: always, and no AGB */
    u32 layer2 = StagePlayerAPI::fpcLy_CurrentLayer();
    StagePlayerAPI::fpcSCtRq_Request(layer2, dStageProcess::fpcNm_METER_e, 0, 0, 0);
    return 1;
}

// HD: the player list count comes from the chunk (not the argument); an unknown start point falls back to the first entry; the meter is always created and the GBA (AGB) actor is gone
/* 025C1384 */
static s32 dStage_playerInit(dStage_dt_c* i_stage, dStage_nodeHeader* i_data, s32 num, u32 i_file) {
    (void)num;
    if (gabi::ea(dComIfGp_getPlayer(0)) != 0) return 1;
    u32 player = gabi::ea(&i_data->m_entryNum);
    gabi::call_ptr(i_stage->__vtbl->setPlayer.function.get(), i_stage, player);
    u32 fn = i_stage->__vtbl->setPlayerNum.function.get();
    u32 n = (u32)i_data->m_entryNum;
    gabi::call_ptr(fn, i_stage, n & 0xFFFF);
    u32 appen = StagePlayerAPI::fopAcM_CreateAppend();
    if (appen == 0) StagePlayerAPI::reportAssert("d_stage.cpp", 0x746, "appen != (0)");
    u32 p = dComIfGp_ea();
    s32 point = (s16)gabi::at<dComIfG_inf_c>(p)->play.mCurStage.mPoint;
    u32 player_data = (u32)i_data->m_offset;
    u32 save = dStage_getSaveBase();
    u32 roomParam = gabi::at<dSv_info_c>(save + 0x20)->mRestart.mRestartParam;
    if (point == -2) {
        /* dStage_playerInitIkada / dStage_decodeSearchIkada */
        u32 shipId = gabi::at<dComIfG_inf_c>(dComIfGp_ea())->play.mIkadaShipId;
        u32 ikada = 0;
        bool found = false;
        if (i_file != 0) {
            s32 cnt = (s32)gabi::at<dStage_fileHeader>(i_file)->m_chunkCount;
            for (u32 i = 0; i < 13 && !found; i++) {
                u32 tag = l_actorTags.mTags[i].mTag; /* "ACTR", "ACT0".."ACTb" */
                u32 node = i_file + 4;
                for (s32 j = 0; j < cnt; j++, node += 0xC) {
                    if (gabi::at<dStage_nodeHeader>(node)->m_tag != tag) continue;
                    s32 en = (s32)(u32)gabi::at<dStage_nodeHeader>(node)->m_entryNum;
                    u32 a = (u32)gabi::at<dStage_nodeHeader>(node)->m_offset;
                    for (s32 k = 0; k < en; k++, a += 0x20) {
                        if (dStage_nameMatches(a, "ikada_h") || dStage_nameMatches(a, "ikada_u") || dStage_nameMatches(a, "ikadaS")) {
                            if (shipId == ((gabi::at<stage_actor_data_class>(a)->base.parameters >> 18) & 0xFF)) {
                                ikada = a;
                                found = true;
                                break;
                            }
                        }
                    }
                    break;
                }
            }
        }
        // HD: this inlined raft check uses a separate copy of the source filename.
        if (!found) StagePlayerAPI::reportAssert(l_ikadaSourceFile.mText, 0x6A3, "ikada_data != (0)");
        u32 roomNo = gabi::at<dComIfG_inf_c>(dComIfGp_ea())->play.mIkadaShipBeforeRoomId;
        gabi::at<fopAcM_prm_class>(appen)->base.parameters = (roomNo | 0xFF000000) | 0x80;
        StagePlayerAPI::PSMTXTrans(gabi::ea(mDoMtx_stack_c::get()), gabi::at<stage_actor_data_class>(ikada)->base.position.x, gabi::at<stage_actor_data_class>(ikada)->base.position.y, gabi::at<stage_actor_data_class>(ikada)->base.position.z);
        StagePlayerAPI::mDoMtx_YrotM(gabi::ea(mDoMtx_stack_c::get()), (s16)gabi::at<stage_actor_data_class>(ikada)->base.angle.y);
        gabi::Local<cXyz> offset;
        gabi::Local<cXyz> pos;
        offset->x = 0.0f;
        offset->y = 87.0f;
        offset->z = 550.0f;
        StagePlayerAPI::PSMTXMultVec(gabi::ea(mDoMtx_stack_c::get()), gabi::ea(offset.get()), gabi::ea(pos.get()));
        StagePlayerAPI::daSea_execute(gabi::ea(pos.get()));
        f32 h = 0.0f;
        if (StagePlayerAPI::daSea_ChkArea(gabi::at<stage_actor_data_class>(ikada)->base.position.x, gabi::at<stage_actor_data_class>(ikada)->base.position.z)) h = StagePlayerAPI::daSea_calcWave(gabi::at<stage_actor_data_class>(ikada)->base.position.x, gabi::at<stage_actor_data_class>(ikada)->base.position.z);
        f32 y = pos->y;
        y = y + h;
        y = y + 500.0f;
        f32 x = pos->x;
        pos->y = y;
        gabi::at<fopAcM_prm_class>(appen)->base.position.x = x;
        gabi::at<fopAcM_prm_class>(appen)->base.position.y = y;
        gabi::at<fopAcM_prm_class>(appen)->base.position.z = (f32)pos->z;
        u16 az = (u16)((u8)gabi::at<stage_actor_data_class>(ikada)->base.angle.x << 8);
        u16 ay = gabi::at<stage_actor_data_class>(ikada)->base.angle.y;
        gabi::at<fopAcM_prm_class>(appen)->base.angle.x = 0;
        gabi::at<fopAcM_prm_class>(appen)->base.angle.z = az;
        gabi::at<fopAcM_prm_class>(appen)->base.angle.y = ay;
        return playerInit_tail(appen, player_data);
    }
    if (point == -3) {
        gabi::at<fopAcM_prm_class>(appen)->base.parameters = gabi::at<dSv_info_c>(save + 0x20)->mTurnRestart.mParam; /* turn restart param */
        u32 s = dStage_getSaveBase();
        gabi::at<dStage_actorBaseWords>(appen)->position[0] = gabi::load<u32>(gabi::ea(&gabi::at<dSv_info_c>(s + 0x20)->mTurnRestart));
        gabi::at<dStage_actorBaseWords>(appen)->position[1] = gabi::load<u32>(gabi::ea(&gabi::at<dSv_info_c>(s + 0x20)->mTurnRestart.mPosition) + 4);
        gabi::at<dStage_actorBaseWords>(appen)->position[2] = gabi::load<u32>(gabi::ea(&gabi::at<dSv_info_c>(s + 0x20)->mTurnRestart.mPosition) + 8);
        s = dStage_getSaveBase();
        u16 ay = gabi::at<dSv_info_c>(s + 0x20)->mTurnRestart.mAngleY;
        gabi::at<fopAcM_prm_class>(appen)->base.angle.x = 0;
        gabi::at<fopAcM_prm_class>(appen)->base.angle.y = ay;
        gabi::at<fopAcM_prm_class>(appen)->base.angle.z = 0xFF00;
        return playerInit_tail(appen, player_data);
    }
    if (point == -1) {
        gabi::at<fopAcM_prm_class>(appen)->base.parameters = roomParam;
        u32 s = dStage_getSaveBase();
        gabi::at<dStage_actorBaseWords>(appen)->position[0] = gabi::load<u32>(gabi::ea(&gabi::at<dSv_info_c>(s + 0x20)->mRestart.mRestartPos));
        gabi::at<dStage_actorBaseWords>(appen)->position[1] = gabi::load<u32>(gabi::ea(&gabi::at<dSv_info_c>(s + 0x20)->mRestart.mRestartPos) + 4);
        gabi::at<dStage_actorBaseWords>(appen)->position[2] = gabi::load<u32>(gabi::ea(&gabi::at<dSv_info_c>(s + 0x20)->mRestart.mRestartPos) + 8);
        s = dStage_getSaveBase();
        u16 ay = gabi::at<dSv_info_c>(s + 0x20)->mRestart.mRestartAngle;
        gabi::at<fopAcM_prm_class>(appen)->base.angle.z = 0xFF00;
        gabi::at<fopAcM_prm_class>(appen)->base.angle.y = ay;
        gabi::at<fopAcM_prm_class>(appen)->base.angle.x = 0;
        return playerInit_tail(appen, player_data);
    }
    u32 e = player_data;
    u32 i = 0;
    if ((s32)n > 0) {
        for (u32 c = n; c != 0; c--) {
            if ((u32)gabi::load<u8>(e + 0x1D) == (u32)point) break;
            e += 0x20;
            i++;
        }
    }
    if (i == n) e = player_data; /* HD: not found -> the first entry (GameCube: assert) */
    dStage_copyActorBase(appen, e + offsetof(stage_actor_data_class, base));
    if (roomParam != 0) {
        u32 prm = gabi::at<fopAcM_prm_class>(appen)->base.parameters;
        if (((prm >> 12) & 0xF) != 2) gabi::at<fopAcM_prm_class>(appen)->base.parameters = (roomParam & ~0x3Fu) | (prm & 0x3F);
    }
    return playerInit_tail(appen, e);
}

// HD: dStage_cameraInit, dStage_cameraCreate inlined (matcher: dStage_cameraCreate)
/* 025C1AD0 */
static s32 dStage_cameraInit(dStage_dt_c* i_stage, dStage_nodeHeader* i_data, s32 i_num, u32 i_file) {
    gabi::call_ptr(i_stage->__vtbl->setCamera.function.get(), i_stage, gabi::ea(&i_data->m_entryNum));
    u32 prm = StagePlayerAPI::cMl_memalignB(-4, 0x18);
    if (prm != 0) {
        gabi::at<fopAcM_prmBase_class>(prm)->parameters = 0;
        gabi::at<fopAcM_prmBase_class>(prm)->position.x = 0.0f;
        gabi::at<fopAcM_prmBase_class>(prm)->position.y = 0.0f;
        StagePlayerAPI::fopCamM_Create(0, dStageProcess::fpcNm_CAMERA_e, prm);
    }
    return 1;
}

/* 025C1B38 */
static s32 dStage_RoomCameraInit(dStage_dt_c* i_stage, dStage_nodeHeader* i_data, s32 i_num, u32 i_file) {
    gabi::call_ptr(i_stage->__vtbl->setCamera.function.get(), i_stage, gabi::ea(&i_data->m_entryNum));
    return 1;
}

/* 025C1B6C */
static s32 dStage_arrowInit(dStage_dt_c* i_stage, dStage_nodeHeader* i_data, s32 i_num, u32 i_file) {
    gabi::call_ptr(i_stage->__vtbl->setArrow.function.get(), i_stage, gabi::ea(&i_data->m_entryNum));
    return 1;
}

/* 025C1BA0 */
static s32 dStage_mapInfo_GetOceanX(u32 i_mapInfo) {
    s32 rt = gabi::at<stage_map_info_class>(i_mapInfo)->mOceanXZ & 0xF;
    if (rt & 8) return rt - 0x10;
    return rt;
}

/* 025C1BB8 */
static s32 dStage_mapInfo_GetOceanZ(u32 i_mapInfo) {
    s32 rt = (gabi::at<stage_map_info_class>(i_mapInfo)->mOceanXZ >> 4) & 0xF;
    if (rt & 8) return rt - 0x10;
    return rt;
}

/* 025C1BD4 */
static s32 dStage_mapInfoInit(dStage_dt_c* i_stage, dStage_nodeHeader* i_data, s32 i_num, u32 i_file) {
    u32 fn = i_stage->__vtbl->setMapInfo.function.get();
    gabi::call_ptr(fn, i_stage, (u32)i_data->m_offset);
    gabi::call_ptr(i_stage->__vtbl->setMapInfoBase.function.get(), i_stage, gabi::ea(&i_data->m_entryNum));
    return 1;
}

/* 025C1C38 */
static s32 dStage_paletInfoInit(dStage_dt_c* i_stage, dStage_nodeHeader* i_data, s32 i_num, u32 i_file) {
    u32 fn = i_stage->__vtbl->setPaletInfo.function.get();
    gabi::call_ptr(fn, i_stage, (u32)i_data->m_offset);
    return 1;
}

/* 025C1C6C */
static s32 dStage_pselectInfoInit(dStage_dt_c* i_stage, dStage_nodeHeader* i_data, s32 i_num, u32 i_file) {
    u32 fn = i_stage->__vtbl->setPselectInfo.function.get();
    gabi::call_ptr(fn, i_stage, (u32)i_data->m_offset);
    return 1;
}

/* 025C1CA0 */
static s32 dStage_envrInfoInit(dStage_dt_c* i_stage, dStage_nodeHeader* i_data, s32 i_num, u32 i_file) {
    u32 fn = i_stage->__vtbl->setEnvrInfo.function.get();
    gabi::call_ptr(fn, i_stage, (u32)i_data->m_offset);
    return 1;
}

/* 025C1CD4 */
static s32 dStage_filiInfoInit(dStage_dt_c* i_stage, dStage_nodeHeader* i_data, s32 i_num, u32 i_file) {
    u32 fn = i_stage->__vtbl->setFileListInfo.function.get();
    if (i_num == 0) gabi::call_ptr(fn, i_stage, 0u);
    else gabi::call_ptr(fn, i_stage, (u32)i_data->m_offset);
    return 1;
}

/* 025C1D24 */
static s32 dStage_vrboxInfoInit(dStage_dt_c* i_stage, dStage_nodeHeader* i_data, s32 i_num, u32 i_file) {
    u32 fn = i_stage->__vtbl->setVrboxInfo.function.get();
    gabi::call_ptr(fn, i_stage, (u32)i_data->m_offset);
    return 1;
}

/* 025C1D58 */
static s32 dStage_plightInfoInit(dStage_dt_c* i_stage, dStage_nodeHeader* i_data, s32 i_num, u32 i_file) {
    u32 fn = i_stage->__vtbl->setPlightInfo.function.get();
    gabi::call_ptr(fn, i_stage, (u32)i_data->m_offset);
    gabi::call_ptr(i_stage->__vtbl->setPlightNumInfo.function.get(), i_stage, (u32)i_num);
    return 1;
}

/* 025C1DBC */
static s32 dStage_lgtvInfoInit(dStage_dt_c* i_stage, dStage_nodeHeader* i_data, s32 i_num, u32 i_file) {
    gabi::call_ptr(i_stage->__vtbl->setLightVecInfoNum.function.get(), i_stage, (u32)i_num);
    u32 fn = i_stage->__vtbl->setLightVecInfo.function.get();
    if (i_num == 0) gabi::call_ptr(fn, i_stage, 0u);
    else gabi::call_ptr(fn, i_stage, (u32)i_data->m_offset);
    return 1;
}

/* WWHD glue (tools/verify/autoglue.py): private ABI bindings
data l_actorTags 10055880
data l_ikadaSourceFile 100553F4
str 10055318 in 025C1384
str 10055320 in 025C1384
str 1005538C in 025C1384

str 1005544C in 025C1384
str 1005593C in 025C1384
str 10055948 in 025C1384
str 10055958 in 025C1384
ext 0273AA24 void StagePlayerAPI::reportAssert(const char* file, s32 line, const char* msg)
ext 025D5600 u32 StagePlayerAPI::fopAcM_CreateAppend()
ext 025DC80C u32 StagePlayerAPI::fopScnM_SearchByID(u32 id)
ext 025D5834 u32 StagePlayerAPI::fopAcM_create(s32 proc, u32 prm, u32 pos, s32 room, u32 angle, u32 scale, s32 subtype, u32 cb)
ext 025DED64 u32 StagePlayerAPI::fpcLy_CurrentLayer()
ext 025E14A8 u32 StagePlayerAPI::fpcSCtRq_Request(u32 layer, s32 proc, u32 a2, u32 a3, u32 prm)
ext 028E93CC void StagePlayerAPI::PSMTXTrans(u32 m, f32 x, f32 y, f32 z)
ext 025F1C28 void StagePlayerAPI::mDoMtx_YrotM(u32 m, s16 y)
ext 028E8F64 void StagePlayerAPI::PSMTXMultVec(u32 m, u32 in, u32 out)
ext 0246C5C8 void StagePlayerAPI::daSea_execute(u32 pos)
ext 0246B6A4 BOOL StagePlayerAPI::daSea_ChkArea(f32 x, f32 z)
ext 0246BA0C f32 StagePlayerAPI::daSea_calcWave(f32 x, f32 z)
ext 02019430 u32 StagePlayerAPI::cMl_memalignB(s32 align, u32 size)
ext 025DA654 u32 StagePlayerAPI::fopCamM_Create(s32 idx, s32 proc, u32 prm)
ext 025C1328 void StagePlayerAPI::dStage_startStage_c_set(u32 self, u32 name, u32 roomNo, s32 point, s32 layer)
ext 025C1154 void StagePlayerAPI::dStage_actorCreate(u32 data, u32 prm)
*/
