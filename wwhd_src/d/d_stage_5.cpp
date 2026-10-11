/** Stage/room virtual accessors and compiler-emitted HD helpers. Private decomp source.
 * HD: MEMA/MECO handlers ignore their chunks; unsupported virtuals keep assertion/report
 * side effects. The room-loader exception callback dumps the first 0x800 file bytes.
 */
#include "d/d_stage.h"

// HD: dStage_GetKeepTresureInfo() (probably; returns the static TresureInfo)
/* 025C4734 */
static dStage_KeepTresureInfo* dStage_GetKeepTresureInfo() {
    return &TresureInfo;
}

// HD: dStage_GetKeepDoorInfo() (probably; returns the static DoorInfo)
/* 025C4740 */
static dStage_KeepDoorInfo* dStage_GetKeepDoorInfo() {
    return &DoorInfo;
}

// HD: dStage_memaInfoInit (HD: empty, the MEMA chunk is ignored)
/* 025C474C */
static s32 dStage_memaInfoInit(dStage_dt_c* i_stage, dStage_nodeHeader* i_data, s32 i_num, u32 i_file) {
    return 1;
}

// HD: dStage_mecoInfoInit (HD: empty, the MECO chunk is ignored)
/* 025C4754 */
static s32 dStage_mecoInfoInit(dStage_dt_c* i_stage, dStage_nodeHeader* i_data, s32 i_num, u32 i_file) {
    return 1;
}

// HD: sead::SafeString deleting destructor (this unit's copy, SafeString vtable +0xC)
/* 025C475C */
static void SafeString_dt(u32 self, u32 del) {
    if (self == 0) return;
    if (!(del & 1)) return;
    StageDebugAPI::operator_delete(self);
}

/* 025C4770 */
s32 dStage_roomDt_c::getRoomNo() {
    return (s8)mRoomNo;
}

/* 025C477C */
void dStage_roomDt_c::setCamera(stage_camera_class* v) {
    mpCamera = v;
}

/* 025C4784 */
stage_camera_class* dStage_roomDt_c::getCamera() {
    return mpCamera.get();
}

/* 025C478C */
void dStage_roomDt_c::setArrow(stage_arrow_class* v) {
    mpArrow = v;
}

/* 025C4794 */
stage_arrow_class* dStage_roomDt_c::getArrow() {
    return mpArrow.get();
}

/* 025C479C */
void dStage_roomDt_c::setPlayer(stage_actor_class* v) {
    mpPlayer = v;
}

/* 025C47A4 */
stage_actor_class* dStage_roomDt_c::getPlayer() {
    return mpPlayer.get();
}

/* 025C47AC */
void dStage_roomDt_c::setPlayerNum(u32 v) {
    mPlayerNum = (u16)v;
}

/* 025C47B4 */
u16 dStage_roomDt_c::getPlayerNum() {
    return mPlayerNum;
}

/* 025C47BC */
void dStage_roomDt_c::setRoom(u32 v) {
    StageDebugAPI::OSReport("Room non room data !!\n"); /* "Room non room data !!\n" */
    StageDebugAPI::reportAssert("d_stage.h", 0x6F0, "0");
}

/* 025C4800 */
u32 dStage_roomDt_c::getRoom() {
    StageDebugAPI::OSReport("Room non room data !!\n"); /* "Room non room data !!\n" */
    StageDebugAPI::reportAssert("d_stage.h", 0x6F5, "0");
    return 0;
}

/* 025C4848 */
void dStage_roomDt_c::setMapInfo(stage_map_info_class* v) {
    mpMapInfo = v;
}

/* 025C4850 */
stage_map_info_class* dStage_roomDt_c::getMapInfo() {
    return mpMapInfo.get();
}

/* 025C4858 */
void dStage_roomDt_c::setMapInfoBase(stage_map_info_dummy_class* v) {
    mpMapInfoBase = v;
}

/* 025C4860 */
stage_map_info_dummy_class* dStage_roomDt_c::getMapInfoBase() {
    return mpMapInfoBase.get();
}

/* 025C4868 */
void dStage_roomDt_c::setPaletInfo(u32 v) {
    StageDebugAPI::OSReport("Room non palet data !!\n"); /* "Room non palet data !!\n" */
    StageDebugAPI::reportAssert("d_stage.h", 0x717, "0");
}

/* 025C48AC */
u32 dStage_roomDt_c::getPaletInfo() {
    StageDebugAPI::OSReport("Room non palet data !!\n"); /* "Room non palet data !!\n" */
    StageDebugAPI::reportAssert("d_stage.h", 0x71B, "0");
    return 0;
}

/* 025C48F4 */
void dStage_roomDt_c::setPselectInfo(u32 v) {
    StageDebugAPI::OSReport("Room non pselect data !!\n"); /* "Room non pselect data !!\n" */
    StageDebugAPI::reportAssert("d_stage.h", 0x722, "0");
}

/* 025C4938 */
u32 dStage_roomDt_c::getPselectInfo() {
    StageDebugAPI::OSReport("Room non pselect data !!\n"); /* "Room non pselect data !!\n" */
    StageDebugAPI::reportAssert("d_stage.h", 0x726, "0");
    return 0;
}

/* 025C4980 */
void dStage_roomDt_c::setEnvrInfo(u32 v) {
    StageDebugAPI::OSReport("Room non envr data !!\n"); /* "Room non envr data !!\n" */
    StageDebugAPI::reportAssert("d_stage.h", 0x72D, "0");
}

/* 025C49C4 */
u32 dStage_roomDt_c::getEnvrInfo() {
    StageDebugAPI::OSReport("Room non envr data !!\n"); /* "Room non envr data !!\n" */
    StageDebugAPI::reportAssert("d_stage.h", 0x731, "0");
    return 0;
}

/* 025C4A0C */
void dStage_roomDt_c::setVrboxInfo(stage_vrbox_info_class* v) {
    mpVrbox = v;
}

/* 025C4A14 */
stage_vrbox_info_class* dStage_roomDt_c::getVrboxInfo() {
    return mpVrbox.get();
}

/* 025C4A1C */
void dStage_roomDt_c::setPlightInfo(u32 v) {
    StageDebugAPI::OSReport("Room non plight data !!\n"); /* "Room non plight data !!\n" */
    StageDebugAPI::reportAssert("d_stage.h", 0x745, "0");
}

/* 025C4A60 */
u32 dStage_roomDt_c::getPlightInfo() {
    StageDebugAPI::OSReport("Room non plight data !!\n"); /* "Room non plight data !!\n" */
    StageDebugAPI::reportAssert("d_stage.h", 0x749, "0");
    return 0;
}

/* 025C4AA8 */
void dStage_roomDt_c::setPlightNumInfo(u32 v) {
    StageDebugAPI::OSReport("Room non plight num data !!\n"); /* "Room non plight num data !!\n" */
    StageDebugAPI::reportAssert("d_stage.h", 0x776, "0");
}

/* 025C4AEC */
u32 dStage_roomDt_c::getPlightNumInfo() {
    StageDebugAPI::OSReport("Room non plight num data !!\n"); /* "Room non plight num data !!\n" */
    StageDebugAPI::reportAssert("d_stage.h", 0x77B, "0");
    return 0;
}

/* 025C4B34 */
void dStage_roomDt_c::setLightVecInfo(stage_lightvec_info_class* v) {
    mpLightVecInfo = v;
}

/* 025C4B3C */
void dStage_roomDt_c::setLightVecInfoNum(u32 v) {
    mLightVecInfoNum = v;
}

/* 025C4B44 */
void dStage_roomDt_c::setStagInfo(u32 v) {
    StageDebugAPI::OSReport("Room non stag data !!\n"); /* "Room non stag data !!\n" */
    StageDebugAPI::reportAssert("d_stage.h", 0x79C, "0");
}

/* 025C4B88 */
u32 dStage_roomDt_c::getStagInfo() {
    StageDebugAPI::OSReport("Room non stag data !!\n"); /* "Room non stag data !!\n" */
    StageDebugAPI::reportAssert("d_stage.h", 0x7A0, "0");
    return 0;
}

/* 025C4BD0 */
void dStage_roomDt_c::setSclsInfo(stage_scls_info_dummy_class* v) {
    mpScls = v;
}

/* 025C4BD8 */
stage_scls_info_dummy_class* dStage_roomDt_c::getSclsInfo() {
    return mpScls.get();
}

/* 025C4BE0 */
void dStage_roomDt_c::setPntInfo(u32 v) {
    StageDebugAPI::OSReport("Room non Pnt data !\n"); /* "Room non Pnt data !\n" */
    StageDebugAPI::reportAssert("d_stage.h", 0x7B9, "0");
}

/* 025C4C24 */
u32 dStage_roomDt_c::getPntInf() {
    StageDebugAPI::OSReport("Room non Pnts data !\n"); /* "Room non Pnts data !\n" */
    StageDebugAPI::reportAssert("d_stage.h", 0x7BD, "0");
    return 0;
}

/* 025C4C6C */
void dStage_roomDt_c::setPathInfo(u32 v) {
    StageDebugAPI::OSReport("Room non Path data !\n"); /* "Room non Path data !\n" */
    StageDebugAPI::reportAssert("d_stage.h", 0x7C4, "0");
}

/* 025C4CB0 */
u32 dStage_roomDt_c::getPathInf() {
    StageDebugAPI::OSReport("Room non Path data !\n"); /* "Room non Path data !\n" */
    StageDebugAPI::reportAssert("d_stage.h", 0x7C8, "0");
    return 0;
}

/* 025C4CF8 */
void dStage_roomDt_c::setPnt2Info(dStage_dPnt_c* v) {
    mpPoint2 = v;
}

/* 025C4D00 */
dStage_dPnt_c* dStage_roomDt_c::getPnt2Inf() {
    return mpPoint2.get();
}

/* 025C4D08 */
void dStage_roomDt_c::setPath2Info(dStage_dPath_c* v) {
    mpPath2 = v;
}

/* 025C4D10 */
dStage_dPath_c* dStage_roomDt_c::getPath2Inf() {
    return mpPath2.get();
}

/* 025C4D18 */
void dStage_roomDt_c::setSoundInfo(dStage_SoundInfo_c* v) {
    mpSound = v;
}

/* 025C4D20 */
dStage_SoundInfo_c* dStage_roomDt_c::getSoundInf() {
    return mpSound.get();
}

/* 025C4D28 */
void dStage_roomDt_c::setEventInfo(u32 v) {
    StageDebugAPI::OSReport("Room non event data!\n"); /* "Room non event data!\n" */
    StageDebugAPI::reportAssert("d_stage.h", 0x7FC, "0");
}

/* 025C4D6C */
u32 dStage_roomDt_c::getEventInfo() {
    StageDebugAPI::OSReport("Room non event data!\n"); /* "Room non event data!\n" */
    StageDebugAPI::reportAssert("d_stage.h", 0x800, "0");
    return 0;
}

/* 025C4DB4 */
void dStage_roomDt_c::setFileListInfo(dStage_FileList_dt_c* v) {
    mpFileList = v;
}

/* 025C4DBC */
dStage_FileList_dt_c* dStage_roomDt_c::getFileListInfo() {
    return mpFileList.get();
}

/* 025C4DC4 */
void dStage_roomDt_c::setFloorInfo(dStage_FloorInfo_c* v) {
    mpFloor = v;
}

/* 025C4DCC */
dStage_FloorInfo_c* dStage_roomDt_c::getFloorInfo() {
    return mpFloor.get();
}

/* 025C4DD4 */
void dStage_roomDt_c::setShip(dStage_Ship_c* v) {
    mpShip = v;
}

/* 025C4DDC */
dStage_Ship_c* dStage_roomDt_c::getShip() {
    return mpShip.get();
}

/* 025C4DE4 */
void dStage_roomDt_c::setMulti(u32 v) {
    StageDebugAPI::OSReport("Room non multi data!\n"); /* "Room non multi data!\n" */
    StageDebugAPI::reportAssert("d_stage.h", 0x878, "0");
}

/* 025C4E28 */
u32 dStage_roomDt_c::getMulti() {
    StageDebugAPI::OSReport("Room non multi data!\n"); /* "Room non multi data!\n" */
    StageDebugAPI::reportAssert("d_stage.h", 0x87D, "0");
    return 0;
}

/* 025C4E70 */
void dStage_roomDt_c::setLbnk(dStage_Lbnk_c* v) {
    mpLbnk = v;
}

/* 025C4E78 */
dStage_Lbnk_c* dStage_roomDt_c::getLbnk() {
    return mpLbnk.get();
}

/* 025C4E80 */
void dStage_roomDt_c::setTresure(stage_tresure_class* v) {
    mpTresure = v;
}

// HD: dStage_roomDt_c vtable +0x234 (GameCube slot setLbnk): HD: an assert only
/* 025C4E88 */
void dStage_roomDt_c::setDMap(u32 v) {
    StageDebugAPI::reportAssert("d_stage.h", 0x8A9, "0");
}

// HD: dStage_roomDt_c vtable +0x23C (GameCube slot getLbnk): HD: an assert, NULL
/* 025C4EA0 */
u32 dStage_roomDt_c::getDMap() {
    StageDebugAPI::reportAssert("d_stage.h", 0x8AE, "0");
    return 0;
}

/* 025C4ED8 */
void dStage_roomDt_c::setDrTg(stage_tgsc_class* v) {
    mpDrTg = v;
}

/* 025C4EE0 */
s32 dStage_stageDt_c::getRoomNo() {
    return -1;
}

/* 025C4EE8 */
void dStage_stageDt_c::setCamera(stage_camera_class* v) {
    mpCamera = v;
}

/* 025C4EF0 */
stage_camera_class* dStage_stageDt_c::getCamera() {
    return mpCamera.get();
}

/* 025C4EF8 */
void dStage_stageDt_c::setArrow(stage_arrow_class* v) {
    mpArrow = v;
}

/* 025C4F00 */
stage_arrow_class* dStage_stageDt_c::getArrow() {
    return mpArrow.get();
}

/* 025C4F08 */
void dStage_stageDt_c::setPlayer(stage_actor_class* v) {
    mpPlayer = v;
}

/* 025C4F10 */
stage_actor_class* dStage_stageDt_c::getPlayer() {
    return mpPlayer.get();
}

/* 025C4F18 */
void dStage_stageDt_c::setPlayerNum(u32 v) {
    mPlayerNum = (u16)v;
}

/* 025C4F20 */
u16 dStage_stageDt_c::getPlayerNum() {
    return mPlayerNum;
}

/* 025C4F28 */
void dStage_stageDt_c::setRoom(roomRead_class* v) {
    mpRoomReads = v;
}

/* 025C4F30 */
roomRead_class* dStage_stageDt_c::getRoom() {
    return mpRoomReads.get();
}

/* 025C4F38 */
void dStage_stageDt_c::setMapInfo(stage_map_info_class* v) {
    mpMapInfo = v;
}

/* 025C4F40 */
stage_map_info_class* dStage_stageDt_c::getMapInfo() {
    return mpMapInfo.get();
}

/* 025C4F48 */
void dStage_stageDt_c::setMapInfoBase(stage_map_info_dummy_class* v) {
    mpMapInfoBase = v;
}

/* 025C4F50 */
void dStage_stageDt_c::setPaletInfo(stage_palet_info_class* v) {
    mpPaletInfo = v;
}

/* 025C4F58 */
stage_palet_info_class* dStage_stageDt_c::getPaletInfo() {
    return mpPaletInfo.get();
}

/* 025C4F60 */
void dStage_stageDt_c::setPselectInfo(stage_pselect_info_class* v) {
    mpPselectInfo = v;
}

/* 025C4F68 */
stage_pselect_info_class* dStage_stageDt_c::getPselectInfo() {
    return mpPselectInfo.get();
}

/* 025C4F70 */
void dStage_stageDt_c::setEnvrInfo(stage_envr_info_class* v) {
    mpEnvrInfo = v;
}

/* 025C4F78 */
stage_envr_info_class* dStage_stageDt_c::getEnvrInfo() {
    return mpEnvrInfo.get();
}

/* 025C4F80 */
void dStage_stageDt_c::setVrboxInfo(stage_vrbox_info_class* v) {
    mpVrboxInfo = v;
}

/* 025C4F88 */
stage_vrbox_info_class* dStage_stageDt_c::getVrboxInfo() {
    return mpVrboxInfo.get();
}

/* 025C4F90 */
void dStage_stageDt_c::setPlightInfo(stage_plight_info_class* v) {
    mpPlightInfo = v;
}

/* 025C4F98 */
stage_plight_info_class* dStage_stageDt_c::getPlightInfo() {
    return mpPlightInfo.get();
}

/* 025C4FA0 */
void dStage_stageDt_c::setLightVecInfo(u32 v) {
    StageDebugAPI::OSReport("stage non LightVec data !!\n"); /* "stage non LightVec data !!\n" */
    StageDebugAPI::reportAssert("d_stage.h", 0xA46, "0");
}

/* 025C4FE4 */
void dStage_stageDt_c::setLightVecInfoNum(u32 v) {
    StageDebugAPI::OSReport("stage non LightVecNum data !!\n"); /* "stage non LightVecNum data !!\n" */
    StageDebugAPI::reportAssert("d_stage.h", 0xA51, "0");
}

/* 025C5028 */
void dStage_stageDt_c::setPlightNumInfo(u32 v) {
    mPlightInfoNum = v;
}

/* 025C5030 */
u32 dStage_stageDt_c::getPlightNumInfo() {
    return mPlightInfoNum;
}

/* 025C5038 */
void dStage_stageDt_c::setStagInfo(stage_stag_info_class* v) {
    mpStagInfo = v;
}

/* 025C5040 */
stage_stag_info_class* dStage_stageDt_c::getStagInfo() {
    return mpStagInfo.get();
}

/* 025C5048 */
void dStage_stageDt_c::setSclsInfo(stage_scls_info_dummy_class* v) {
    mpScls = v;
}

/* 025C5050 */
stage_scls_info_dummy_class* dStage_stageDt_c::getSclsInfo() {
    return mpScls.get();
}

/* 025C5058 */
void dStage_stageDt_c::setPntInfo(dStage_dPnt_c* v) {
    mpPnt = v;
}

/* 025C5060 */
dStage_dPnt_c* dStage_stageDt_c::getPntInf() {
    return mpPnt.get();
}

/* 025C5068 */
void dStage_stageDt_c::setPathInfo(dStage_dPath_c* v) {
    mpPath = v;
}

/* 025C5070 */
dStage_dPath_c* dStage_stageDt_c::getPathInf() {
    return mpPath.get();
}

/* 025C5078 */
void dStage_stageDt_c::setPnt2Info(dStage_dPnt_c* v) {
    mpPnt2 = v;
}

/* 025C5080 */
dStage_dPnt_c* dStage_stageDt_c::getPnt2Inf() {
    return mpPnt2.get();
}

/* 025C5088 */
void dStage_stageDt_c::setPath2Info(dStage_dPath_c* v) {
    mpPath2 = v;
}

/* 025C5090 */
dStage_dPath_c* dStage_stageDt_c::getPath2Inf() {
    return mpPath2.get();
}

/* 025C5098 */
void dStage_stageDt_c::setSoundInfo(dStage_SoundInfo_c* v) {
    mpSoundInfo = v;
}

/* 025C50A0 */
void dStage_stageDt_c::setEventInfo(dStage_EventInfo_c* v) {
    mpEventInfo = v;
}

/* 025C50A8 */
dStage_EventInfo_c* dStage_stageDt_c::getEventInfo() {
    return mpEventInfo.get();
}

/* 025C50B0 */
void dStage_stageDt_c::setFileListInfo(u32 v) {
    StageDebugAPI::OSReport("stage non filelist data!\n"); /* "stage non filelist data!\n" */
    StageDebugAPI::reportAssert("d_stage.h", 0xAD2, "0");
}

/* 025C50F4 */
void dStage_stageDt_c::setFloorInfo(dStage_FloorInfo_c* v) {
    mpFloorInfo = v;
}

/* 025C50FC */
dStage_FloorInfo_c* dStage_stageDt_c::getFloorInfo() {
    return mpFloorInfo.get();
}

/* 025C5104 */
void dStage_stageDt_c::setShip(u32 v) {
    StageDebugAPI::OSReport("stage non SHIP data!\n"); /* "stage non SHIP data!\n" */
    StageDebugAPI::reportAssert("d_stage.h", 0xB25, "0");
}

/* 025C5148 */
void dStage_stageDt_c::setMulti(dStage_Multi_c* v) {
    mpMulti = v;
}

/* 025C5150 */
dStage_Multi_c* dStage_stageDt_c::getMulti() {
    return mpMulti.get();
}

/* 025C5158 */
void dStage_stageDt_c::setLbnk(u32 v) {
    StageDebugAPI::OSReport("stage non Lbnk data!\n"); /* "stage non Lbnk data!\n" */
    StageDebugAPI::reportAssert("d_stage.h", 0xB46, "0");
}

/* 025C519C */
void dStage_stageDt_c::setTresure(stage_tresure_class* v) {
    mpTresure = v;
}

/* 025C51A4 */
void dStage_stageDt_c::setDMap(dStage_DMap_c* v) {
    mpDMap = v;
}

/* 025C51AC */
dStage_DMap_c* dStage_stageDt_c::getDMap() {
    return mpDMap.get();
}

/* 025C51B4 */
void dStage_stageDt_c::setDrTg(stage_tgsc_class* v) {
    mpDrTg = v;
}

// HD: sead::SafeString::assureTerminationImpl_ (this unit's copy, SafeString vtable +0x14: a literal needs nothing)
/* 025C51BC */
static void SafeString_assureTerminationImpl(u32 self) {
}

// HD: HD-only: RoomLoaderExceptionCallback (the crash-dump context of dStage_dt_c_roomLoader, vtable 100553AC +0x14): prints the room file and stage pointers and a hex dump of the first 0x800 bytes of the room file (128 rows of 16)
/* 025C51C0 */
static void RoomLoaderExceptionCallback(u32 self) {
    StageDebugAPI::excPrint("I am RoomLoaderExceptionCallback. \n"); /* "I am RoomLoaderExceptionCallback. \n" */
    StageDebugAPI::excPrint1("i_data=%08x\n", gabi::at<dStage_roomLoaderContext>(self)->mFile);
    StageDebugAPI::excPrint1("i_stage=%08x\n", gabi::at<dStage_roomLoaderContext>(self)->mStage);
    u32 p = gabi::at<dStage_roomLoaderContext>(self)->mFile;
    for (u32 row = 0x80; row != 0; row--) {
        StageDebugAPI::excPrint1(" %08x", p);
        for (u32 k = 0x10; k != 0; k--, p++) StageDebugAPI::excPrint1(" %02x", gabi::load<u8>(p));
        StageDebugAPI::excPrint("\n");
    }
}

/* WWHD glue (tools/verify/autoglue.py): private ABI bindings
str 10055378 in 025C51C0
str 10055380 in 025C51C0
str 10055388 in 025C51C0
str 10055428 in 025C51C0
str 10055718 in 025C51C0
str 10055870 in 025C51C0

str 10055330 in 025C47BC
str 10055332 in 025C4800
str 10055334 in 025C4868
str 10055336 in 025C48AC
str 10055338 in 025C48F4
str 1005533A in 025C4938
str 1005533C in 025C4980
str 1005533E in 025C49C4
str 10055340 in 025C4A1C
str 10055342 in 025C4A60
str 10055348 in 025C4AA8
str 1005534A in 025C4AEC
str 1005534C in 025C4B44
str 1005534E in 025C4B88
str 10055350 in 025C4BE0
str 10055352 in 025C4C24
str 10055354 in 025C4C6C
str 10055356 in 025C4CB0
str 10055358 in 025C4D28
str 1005535A in 025C4D6C
str 1005535C in 025C4DE4
str 1005535E in 025C4E28
str 10055360 in 025C4E88
str 10055362 in 025C4EA0
str 10055364 in 025C4FA0
str 10055368 in 025C4FE4
str 1005536C in 025C50B0
str 10055370 in 025C5104
str 10055374 in 025C5158
str 100553C4 in 025C4868
str 100553DC in 025C48AC
str 10055408 in 025C4FA0
str 10055460 in 025C47BC
str 1005546C in 025C4800
str 10055478 in 025C4868
str 10055484 in 025C48AC
str 10055490 in 025C48F4
str 100554AC in 025C48F4
str 100554B8 in 025C4938
str 100554D4 in 025C4938
str 100554E0 in 025C4980
str 100554EC in 025C49C4
str 100554F8 in 025C4A1C
str 10055504 in 025C4A60
str 10055528 in 025C4AA8
str 10055534 in 025C4AEC
str 10055540 in 025C4B44
str 1005554C in 025C4B88
str 10055558 in 025C4BE0
str 10055564 in 025C4C24
str 1005557C in 025C4C24
str 10055588 in 025C4C6C
str 100555A0 in 025C4C6C
str 100555AC in 025C4CB0
str 100555C4 in 025C4CB0
str 100555D0 in 025C4D28
str 100555E8 in 025C4D28
str 100555F4 in 025C4D6C
str 1005560C in 025C4D6C
str 10055618 in 025C4DE4
str 10055630 in 025C4DE4
str 1005563C in 025C4E28
str 10055654 in 025C4E28
str 10055660 in 025C4E88
str 1005566C in 025C4EA0
str 10055678 in 025C4FA0
str 10055688 in 025C4FE4
str 10055698 in 025C50B0
str 100556B4 in 025C50B0
str 100556C8 in 025C5104
str 100556E0 in 025C5104
str 100556F0 in 025C5158
str 10055708 in 025C5158
str 10055728 in 025C47BC
str 10055740 in 025C4800
str 10055758 in 025C4980
str 10055770 in 025C49C4
str 10055788 in 025C4A1C
str 100557A4 in 025C4A60
str 100557C8 in 025C4AA8
str 100557E8 in 025C4AEC
str 10055808 in 025C4B44
str 10055820 in 025C4B88
str 10055838 in 025C4BE0
str 10055850 in 025C4FE4
ext 0273AA24 void StageDebugAPI::reportAssert(const char* file, s32 line, const char* msg)
ext C0009EE8 void StageDebugAPI::OSReport(const char* fmt)
ext 02034598 void StageDebugAPI::excPrint(const char* fmt)
ext 02034598 void StageDebugAPI::excPrint1(const char* fmt, u32 a)
ext 0273AF40 void StageDebugAPI::operator_delete(u32 p)
*/
