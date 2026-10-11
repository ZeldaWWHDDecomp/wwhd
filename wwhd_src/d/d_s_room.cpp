/** Room scenes and phased resource loading. */
#include "d/d_s_room.h"

static RoomStatusTable l_roomStatus;
static RoomDemoArcName l_demoArcName;
static RoomPhaseTable l_method;
static RoomSafeStringMethods l_safeStringMethods;
static RoomSafeStringMethods l_fixedStringMethods;
static RoomPlayerPtr l_player;
static RoomObjectPtr l_audio;
static RoomObjectPtr l_resourceManager;
static u8 l_seaTalk;
static RoomHeaderStatics l_headerStatics;
static const RoomConstants l_constants;

// HD: preserve SafeString's bounded comparison, including the post-assure pointer reloads.
static bool roomStringEqual(const char* first, const char* second) {
    if (first == second) {
        return true;
    }
    const be<u8>* firstBytes = reinterpret_cast<const be<u8>*>(first);
    const be<u8>* secondBytes = reinterpret_cast<const be<u8>*>(second);
    for (u32 n = 0; n < 0x40001; ++n) {
        u8 character = firstBytes[n];
        if (character != secondBytes[n]) {
            return false;
        }
        if (character == 0) {
            return true;
        }
    }
    return false;
}

static bool roomStageEqual(const char* name) {
    // HD: the two SafeString temporaries each occupy exactly eight guest bytes.
    gabi::Local<RoomSafeString> first, second;
    first->setString(name);
    first->mpMethods = &l_safeStringMethods;
    second->setString(roomPlay()->mStartStageName);
    second->mpMethods = &l_safeStringMethods;
    first->assureTermination();
    first->assureTermination();
    const char* captured = first->mpString;
    second->assureTermination();
    if (captured == second->mpString.get()) {
        return true;
    }
    return roomStringEqual(first->mpString, second->mpString);
}

/* 025B37EC */
void objectSetCheck(room_of_scene_class* i_this) {
    u32 roomNo = i_this->mParameters;
    dComIfGp_get();
    RoomStatus* status = &l_roomStatus.entries[roomNo];
    bool reloaded = i_this->mbReLoaded != 0;
    u32 hiddenFlag = status->mFlags & RoomFlag_HIDDEN;
    if (!reloaded) {
        if (!hiddenFlag) {
            Room_createBackground(Room_BG_PROCESS, roomNo, 0, -1, 0, 0, -1, 0);
            void* roomData = i_this->mpRoomData;
            RoomDt* roomDt = i_this->mpRoomDt;
            dStage_dt_c_roomReLoader(roomData, roomDt, roomNo);
            i_this->mbReLoaded = 1;
            i_this->mNotifyPlayer = 1;
        }
        return;
    }
    // HD: notify the player once after room objects have been reloaded.
    if (i_this->mNotifyPlayer) {
        if (roomNo != 0) {
            RoomAudio_sceneChange(l_audio);
            RoomPlayerState* player = l_player;
            player->mRoomReloadState = 0;
            player->mRoomTransitionState = 0;
        }
        if (roomStageEqual("sea")) {
            for (s32 n = 0; n < 2; ++n) {
                RoomPlayer_enterRoom(l_player, roomNo);
            }
            RoomPlayer_finishRoom(l_player, roomNo);
        }
        i_this->mNotifyPlayer = 0;
    }
    if (hiddenFlag) {
        // HD: the scene's node layer begins after the base's virtual destructor slot.
        layer_class* layer = &reinterpret_cast<process_node_class*>(i_this)->mLayer;
        Room_layerJudge(layer, Room_deleteJudge, 0);
        dComIfGp_get();
        s32 zoneNo = status->mZoneNo;
        dSv_zoneBit_clearRoomSwitch(&dComIfGs_info()->mZone[zoneNo].mZoneBit);
        i_this->mbReLoaded = 0;
    }
}

/* 025B39F8 */
BOOL dScnRoom_Execute(room_of_scene_class* i_this) {
    u32 roomNo = i_this->mParameters;
    dComIfGp_get();
    RoomStatus* status = &l_roomStatus.entries[roomNo];
    u8 flags = status->mFlags;
    if (flags & RoomFlag_DELETE) {
        fopScnM_DeleteReq(i_this);
        return TRUE;
    }
    dComIfGp_get();
    flags = status->mFlags;
    if (flags & RoomFlag_RELOAD) {
        dComIfGp_get();
        flags = status->mFlags;
        status->mFlags = static_cast<u8>(flags & ~RoomFlag_RELOAD);
        dComIfGp_get();
        flags = status->mFlags;
        status->mFlags = static_cast<u8>(flags | RoomFlag_ACTIVE);
    } else {
        Room_objectSetCheck(i_this);
    }
    return TRUE;
}

/* 025B3A9C */
BOOL dScnRoom_IsDelete(room_of_scene_class*) {
    return TRUE;
}

/* 025B3AA4 */
BOOL dScnRoom_Delete(room_of_scene_class* i_this) {
    u32 roomNo = i_this->mParameters;
    // HD: room-particle removal can defer scene deletion.
    if (i_this->mbHasRoomParticle) {
        RoomPlayView* play = roomPlay();
        if (!dPa_control_removeRoomScene(play->mpParticle, 0, roomNo)) {
            return FALSE;
        }
    }
    if (i_this->mbSetMap) {
        dComIfGp_map_deleteImage(i_this->mParameters);
    }
    dComIfG_deleteStageRes(i_this->mArcName);
    dComIfGp_get();
    l_roomStatus.entries[roomNo].mFlags = 0;
    RoomPlayView* play = roomPlay();
    RoomDt* roomDt = dStage_roomControl_getStatusRoomDt(&play->mRoomControl, roomNo);
    roomDt->init();
    play = roomPlay();
    if (play->mEnableNextStage == 0) {
        if (i_this->mPearlState == 1) {
            dComIfGs_onEventBit(RoomEvent_DIN_LEAVE);
        }
        if (roomStageEqual("sea")) {
            dSv_event_offEventBit(dComIfGs_getTmpEvent(), RoomEvent_SEA_TALK_TMP);
        }
    }
    if (i_this->mPearlState == 2) {
        dComIfGs_onEventBit(RoomEvent_FARORE_LEAVE);
    }
    if (roomStageEqual("sea")) {
        l_seaTalk = 0;
    }
    // HD: no GameCube salvage reset or room heap free follows these updates.
    return TRUE;
}

/* 025B3D18 */
cPhs_State phase_0(room_of_scene_class* i_this) {
    u32 roomNo = i_this->mParameters;
    u32 id = i_this ? static_cast<u32>(i_this->mBsPcId) : 0xFFFFFFFF;
    l_roomStatus.entries[roomNo].mProcID = id;
    // HD: cache an eight-byte archive name using unsigned room formatting.
    Room_formatArchiveName(i_this->mArcName, 8, "Room%u", roomNo);
    return cPhs_NEXT_e;
}

/* 025B3D70 */
cPhs_State phase_1(room_of_scene_class* i_this) {
    if (dComIfG_setStageRes(i_this->mArcName, 0) != 0) {
        return cPhs_NEXT_e;
    }
    dStage_escapeRestart();
    return cPhs_ERROR_e;
}

/* 025B3DBC */
cPhs_State phase_2(room_of_scene_class* i_this) {
    s32 result = dComIfG_syncStageRes(i_this->mArcName);
    if (result > 0) {
        return cPhs_INIT_e;
    }
    if (result < 0) {
        dStage_escapeRestart();
        return cPhs_ERROR_e;
    }
    u32 roomNo = i_this->mParameters;
    dComIfGp_get();
    RoomStatus* status = &l_roomStatus.entries[roomNo];
    if (status->mZoneNo < 0) {
        u32 zoneNo = dSv_info_createZone(dComIfGs_info(), roomNo);
        dComIfGp_get();
        status->mZoneNo = static_cast<u8>(zoneNo);
    }
    RoomPlayView* play = roomPlay();
    RoomDt* roomDt = dStage_roomControl_getStatusRoomDt(&play->mRoomControl, roomNo);
    i_this->mpRoomDt = roomDt;
    roomDt->mRoomNo = static_cast<u8>(roomNo);
    void* roomData = dComIfG_getStageRes(i_this->mArcName, "room.dzr");
    i_this->mpRoomData = roomData;
    if (roomData) {
        dStage_dt_c_roomLoader(roomData, i_this->mpRoomDt);
        if (l_demoArcName.text[0] == 0) {
            play = roomPlay();
            roomDt = dStage_roomControl_getStatusRoomDt(&play->mRoomControl, roomNo);
            RoomLbnk* lbnk = roomDt->getLbnk();
            if (lbnk) {
                be<u8>* banks = lbnk->mEntries;
                if (banks) {
                    u32 layerNo = dComIfG_play_getLayerNo(roomNo);
                    u32 bank = banks[layerNo];
                    if (bank != Room_NO_DEMO_BANK) {
                        if (bank >= 100) {
                            JUTAssertion::showAssert("d_s_room.cpp", 0x238, "0 <= bank && bank < 100");
                        }
                        char* demoArcName = reinterpret_cast<char*>(l_demoArcName.text);
                        Room_formatArchiveName(demoArcName, 8, "Demo%02d", bank);
                        if (!dComIfG_setObjectRes(demoArcName, 0, 0)) {
                            l_demoArcName.text[0] = 0;
                        }
                    }
                }
            }
        }
    }
    // HD: particle loading belongs to the following phase.
    return cPhs_NEXT_e;
}

/* 025B3F90 */
cPhs_State phase_3(room_of_scene_class* i_this) {
    if (l_demoArcName.text[0] != 0) {
        s32 result = dComIfG_syncObjectRes(reinterpret_cast<char*>(l_demoArcName.text));
        if (result > 0) {
            return cPhs_INIT_e;
        }
        if (result < 0) {
            dStage_escapeRestart();
            return cPhs_ERROR_e;
        }
    }
    u32 roomNo = i_this->mParameters;
    RoomPlayView* play = roomPlay();
    RoomDt* roomDt = dStage_roomControl_getStatusRoomDt(&play->mRoomControl, roomNo);
    RoomFileList* fileList = roomDt->getFileListInfo();
    if (!fileList) {
        JUTAssertion::showAssert("d_s_room.cpp", 0x27D, "fileList != (0)");
    }
    u32 particleNo = (fileList->mFlags >> 21) & 0xFF;
    play = roomPlay();
    u32 ready = dPa_control_readScene(play->mpParticle, particleNo, &i_this->sceneCommand);
    i_this->mbHasRoomParticle = static_cast<u8>(ready);
    // HD: load the room particle stream from the bfres archive using SafeString keys.
    gabi::Local<RoomSafeString> first, second;
    first->setString("Particle");
    first->mpMethods = &l_safeStringMethods;
    second->setString("Particle.bfres");
    second->mpMethods = &l_safeStringMethods;
    void* resource = RoomResourceManager_getResource(l_resourceManager, first.get(), second.get(), 0);
    if (resource) {
        RoomArchive* archive = RoomResource_getArchive(resource);
        gabi::Local<RoomFixedSafeString> buffer;
        buffer->mpString = reinterpret_cast<char*>(buffer->mBuffer);
        buffer->mpMethods = &l_fixedStringMethods;
        buffer->mSize = 32;
        buffer->mBuffer[0] = 0;
        buffer->mBuffer[31] = 0;
        RoomFixedSafeString_format(buffer.get(), "Pscene%03d.jpc", particleNo);
        buffer->assureTermination();
        void* root = archive->mRoot.get();
        RoomRelativePointer* entry = RoomArchive_findResource(root, buffer->mpString);
        if (entry) {
            void* contents = entry->get();
            play = roomPlay();
            dPa_control_createRoomScene(play->mpParticle, contents);
        }
    }
    Room_objectSetCheck(i_this);
    return cPhs_NEXT_e;
}

/* 025B419C */
cPhs_State phase_4(room_of_scene_class* i_this) {
    if (!roomPlay()->mpPlayer.get()) {
        return cPhs_INIT_e;
    }
    u32 roomNo = i_this->mParameters;
    if (dComIfGs_checkGetItem(Room_ITEM_PEARL_DIN)) {
        i_this->mPearlState = 1;
    }
    if (roomStageEqual("Omori") && dComIfGs_checkGetItem(Room_ITEM_PEARL_FARORE)) {
        i_this->mPearlState = 2;
    }
    if (roomNo == Room_DRAGON_ROOST) {
        dComIfGs_setEventReg(RoomEvent_DRI_REGISTER, 0);
        if (dComIfGs_isEventBit(RoomEvent_DRI_COMPLETE)) {
            dComIfGs_onEventBit(RoomEvent_DRI_ENABLE);
        }
    }
    return cPhs_COMPLEATE_e;
}

/* 025B435C */
cPhs_State dScnRoom_Create(room_of_scene_class* i_this) {
    return dComLbG_PhaseHandler(&i_this->mPhs, l_method.entries, i_this);
}

// HD: compiler-generated and template code of this translation unit.
/* 025B4370 */
static void __sinit_d_s_room_cpp() {
    l_headerStatics.zero[3] = 0;
    l_headerStatics.zero[2] = 0;
    l_headerStatics.zero[1] = 0;
    l_headerStatics.zero[0] = 0;
    __register_global_object(0x101EAD30);
    f32 low = l_constants.negativePi;
    f32 high = l_constants.positivePi;
    l_headerStatics.negativePi = low;
    l_headerStatics.positivePi = high;
    RoomFirstHeaderObject_ct(&l_headerStatics.firstObject);
    __register_global_object(0x101EAD3C);
    RoomSecondHeaderObject_ct(&l_headerStatics.secondObject);
    __register_global_object(0x101EAD48);
    high = l_constants.firstRange;
    low = l_constants.secondRange;
    l_headerStatics.firstRange[0] = high;
    l_headerStatics.secondRange[0] = low;
    l_headerStatics.firstRange[1] = high;
    l_headerStatics.secondRange[1] = low;
}

/* 025B4434 */
static void RoomSafeString_deletingDtor(RoomSafeString* text, u32 flags) {
    if (text && (flags & 1)) {
        operator_delete(text);
    }
}

/* 025B4448 */
static void RoomFixedSafeString_assureTerminationImpl(RoomFixedSafeString* text) {
    be<u8>* buffer = reinterpret_cast<be<u8>*>(text->mpString.get());
    u32 size = text->mSize;
    buffer[size - 1] = 0;
}

/* 025B4460 */
static void RoomFixedSafeString_deletingDtor_a(RoomFixedSafeString* text, u32 flags) {
    if (text && (flags & 1)) {
        operator_delete(text);
    }
}

/* 025B4474 */
static void RoomSafeString_assureTerminationImpl(RoomSafeString*) {}

/* 025B4478 */
static void RoomFixedSafeString_deletingDtor_b(RoomFixedSafeString* text, u32 flags) {
    if (text && (flags & 1)) {
        operator_delete(text);
    }
}

/* 025B448C */
static void RoomFixedSafeString_deletingDtor_c(RoomFixedSafeString* text, u32 flags) {
    if (text && (flags & 1)) {
        operator_delete(text);
    }
}

/* WWHD glue (tools/verify/autoglue.py): room status, HD resources and external calls
 data l_roomStatus 1047E6CC
 data l_demoArcName 1047E6B8
 data l_method 101EAD1C
 data l_safeStringMethods 100534B4
 data l_fixedStringMethods 100534FC
 data l_player 1018F450
 data l_audio 1018F4AC
 data l_resourceManager 101F4F7C
 data l_seaTalk 101D5F40
 data l_headerStatics 1047C8B8
 data l_constants 100535BC
 str 10053514 "sea" in 025B37EC
 str 10053518 "sea" in 025B3AA4
 str 1005351C "Room%u"
 str 10053524 "0 <= bank && bank < 100"
 str 1005353C "d_s_room.cpp" in 025B3DBC
 str 1005354C "room.dzr"
 str 10053558 "Demo%02d"
 str 10053564 "fileList != (0)"
 str 10053574 "d_s_room.cpp" in 025B3F90
 str 10053584 "Particle"
 str 10053590 "Particle.bfres"
 str 100535A0 "Pscene%03d.jpc"
 str 100535B0 "Omori"
 ext 025DC8D0 void fopScnM_DeleteReq(room_of_scene_class*)
 ext 028F17A8 void Room_formatArchiveName(char*, u32, const char*, u32)
 ext 02523A04 BOOL dComIfG_setStageRes(char*, u32)
 ext 02523D08 s32 dComIfG_syncStageRes(char*)
 ext 02524180 void dComIfG_deleteStageRes(char*)
 ext 0252447C void* dComIfG_getStageRes(char*, const char*)
 ext 025C3ED0 void dStage_escapeRestart()
 ext 02525FE4 cPhs_State dComLbG_PhaseHandler(request_of_phase_process_class*, cPhs__Handler*, void*)
 ext 025D5834 void Room_createBackground(u32, u32, u32, s32, u32, u32, s32, u32)
 ext 025C2E8C void dStage_dt_c_roomReLoader(void*, RoomDt*, u32)
 ext 025C2DFC void dStage_dt_c_roomLoader(void*, RoomDt*)
 ext 02039E4C void RoomAudio_sceneChange(void*)
 ext 02037D64 void RoomPlayer_enterRoom(RoomPlayerState*, u32)
 ext 02037230 void RoomPlayer_finishRoom(RoomPlayerState*, u32)
 ext 025DEE20 void* Room_layerJudge(void*, u32, u32)
 ext 025B93B0 void dSv_zoneBit_clearRoomSwitch(dSv_zoneBit_c*)
 ext 025A7EAC u32 dPa_control_removeRoomScene(dPa_control_c*, u32, u32)
 ext 0258F1D8 void dComIfGp_map_deleteImage(u32)
 ext 025C11DC RoomDt* dStage_roomControl_getStatusRoomDt(RoomControl*, u32)
 ext 025B9DD8 u32 dSv_info_createZone(dSv_info_c*, u32)
 ext 025250C0 u32 dComIfG_play_getLayerNo(u32)
 ext 02520354 u32 dComIfG_setObjectRes(char*, u32, u32)
 ext 025203D8 s32 dComIfG_syncObjectRes(char*)
 ext 025A7D8C u32 dPa_control_readScene(dPa_control_c*, u32, gptr<void>*)
 ext 026124B0 void* RoomResourceManager_getResource(void*, RoomSafeString*, RoomSafeString*, u32)
 ext 027E2DC0 RoomArchive* RoomResource_getArchive(void*)
 ext 02759C28 void RoomFixedSafeString_format(RoomFixedSafeString*, const char*, u32)
 ext 027DFA24 RoomRelativePointer* RoomArchive_findResource(void*, const char*)
 ext 025A7C30 void dPa_control_createRoomScene(dPa_control_c*, void*)
 ext 028ED6F8 void RoomFirstHeaderObject_ct(u8*)
 ext 028EAB2C void RoomSecondHeaderObject_ct(u8*)
 ext 025B37EC void Room_objectSetCheck(room_of_scene_class*)
 ext 025B8B7C void dSv_event_offEventBit(dSv_event_c*, u32)
*/
