/* d_stage: stage/room data (dStage_*), part 5 of 5 (025C4734..025C527C), WWHD. See d_stage.cpp for the unit.
 *
 * The out-of-line copies of the dStage_roomDt_c (vtable 10055B90) and dStage_stageDt_c (vtable
 * 10055DE0) set/get virtuals, the "non room data" assert stubs, two static accessors, the
 * MEMA/MECO chunk stubs, this unit's sead::SafeString virtuals and the HD exception callback of
 * the room loader. HD vtables: 8-byte entries, the first virtual (init) at +0xC; GHS replaced
 * never-called virtuals by the deleted-virtual stub 028F036C. Names follow the GameCube
 * declaration order of the slots. */
#include "bindings.h"

namespace d_stage_5_cpp {

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline void JUT_ASSERT_l(u32 file, s32 line, u32 msg) { gabi::call(0x0273AA24, file, line, msg); }
static inline void OSReport_l(u32 fmt) { gabi::call(0xC0009EE8, fmt); } /* OSReport (import, through the thunk 028FE160), varargs */
static inline void excPrint_l(u32 fmt) { gabi::call(0x02034598, fmt); } /* exception console printf, varargs */
static inline void excPrint1_l(u32 fmt, u32 a) { gabi::call(0x02034598, fmt, a); }
static inline void operator_delete_l(u32 p) { gabi::call(0x0273AF40, p); }

static inline u32 ld(u32 a) { return gabi::load<u32>(a); }
static inline u16 ld16(u32 a) { return gabi::load<u16>(a); }
static inline u8 ld8(u32 a) { return gabi::load<u8>(a); }
static inline void st(u32 a, u32 v) { gabi::store<u32>(a, v); }
static inline void st16(u32 a, u16 v) { gabi::store<u16>(a, v); }

/* 025C4734: dStage_GetKeepTresureInfo() (probably; returns the static TresureInfo) */
static u32 dStage_GetKeepTresureInfo() {
    WWHD_FUNC(0x025C4734, u32);
    return 0x1047D9B0;
}
VERIFY(0x025C4734, dStage_GetKeepTresureInfo);

/* 025C4740: dStage_GetKeepDoorInfo() (probably; returns the static DoorInfo) */
static u32 dStage_GetKeepDoorInfo() {
    WWHD_FUNC(0x025C4740, u32);
    return 0x1047DDB4;
}
VERIFY(0x025C4740, dStage_GetKeepDoorInfo);

/* 025C474C: dStage_memaInfoInit (HD: empty, the MEMA chunk is ignored) */
static s32 dStage_memaInfoInit(u32 i_stage, u32 i_data, s32 i_num, u32 i_file) {
    WWHD_FUNC(0x025C474C, s32, i_stage, i_data, i_num, i_file);
    return 1;
}
VERIFY(0x025C474C, dStage_memaInfoInit);

/* 025C4754: dStage_mecoInfoInit (HD: empty, the MECO chunk is ignored) */
static s32 dStage_mecoInfoInit(u32 i_stage, u32 i_data, s32 i_num, u32 i_file) {
    WWHD_FUNC(0x025C4754, s32, i_stage, i_data, i_num, i_file);
    return 1;
}
VERIFY(0x025C4754, dStage_mecoInfoInit);

/* 025C475C: sead::SafeString deleting destructor (this unit's copy, SafeString vtable +0xC) */
static void SafeString_dt(u32 self, u32 del) {
    WWHD_FUNC(0x025C475C, void, self, del);
    if (self == 0) return;
    if (!(del & 1)) return;
    operator_delete_l(self);
}
VERIFY(0x025C475C, SafeString_dt);

/* 025C4770: dStage_roomDt_c::getRoomNo (vtable +014) */
static s32 dStage_roomDt_c_getRoomNo(u32 self) {
    WWHD_FUNC(0x025C4770, s32, self);
    return (s8)ld8(self + 0x4C);
}
VERIFY(0x025C4770, dStage_roomDt_c_getRoomNo);

/* 025C477C: dStage_roomDt_c::setCamera (vtable +01C) */
static void dStage_roomDt_c_setCamera(u32 self, u32 v) {
    WWHD_FUNC(0x025C477C, void, self, v);
    st(self + 0x2C, v);
}
VERIFY(0x025C477C, dStage_roomDt_c_setCamera);

/* 025C4784: dStage_roomDt_c::getCamera (vtable +024) */
static u32 dStage_roomDt_c_getCamera(u32 self) {
    WWHD_FUNC(0x025C4784, u32, self);
    return ld(self + 0x2C);
}
VERIFY(0x025C4784, dStage_roomDt_c_getCamera);

/* 025C478C: dStage_roomDt_c::setArrow (vtable +02C) */
static void dStage_roomDt_c_setArrow(u32 self, u32 v) {
    WWHD_FUNC(0x025C478C, void, self, v);
    st(self + 0x30, v);
}
VERIFY(0x025C478C, dStage_roomDt_c_setArrow);

/* 025C4794: dStage_roomDt_c::getArrow (vtable +034) */
static u32 dStage_roomDt_c_getArrow(u32 self) {
    WWHD_FUNC(0x025C4794, u32, self);
    return ld(self + 0x30);
}
VERIFY(0x025C4794, dStage_roomDt_c_getArrow);

/* 025C479C: dStage_roomDt_c::setPlayer (vtable +03C) */
static void dStage_roomDt_c_setPlayer(u32 self, u32 v) {
    WWHD_FUNC(0x025C479C, void, self, v);
    st(self + 0x20, v);
}
VERIFY(0x025C479C, dStage_roomDt_c_setPlayer);

/* 025C47A4: dStage_roomDt_c::getPlayer (vtable +044) */
static u32 dStage_roomDt_c_getPlayer(u32 self) {
    WWHD_FUNC(0x025C47A4, u32, self);
    return ld(self + 0x20);
}
VERIFY(0x025C47A4, dStage_roomDt_c_getPlayer);

/* 025C47AC: dStage_roomDt_c::setPlayerNum (vtable +04C) */
static void dStage_roomDt_c_setPlayerNum(u32 self, u32 v) {
    WWHD_FUNC(0x025C47AC, void, self, v);
    st16(self + 0x4E, (u16)v);
}
VERIFY(0x025C47AC, dStage_roomDt_c_setPlayerNum);

/* 025C47B4: dStage_roomDt_c::getPlayerNum (vtable +054) */
static u16 dStage_roomDt_c_getPlayerNum(u32 self) {
    WWHD_FUNC(0x025C47B4, u16, self);
    return ld16(self + 0x4E);
}
VERIFY(0x025C47B4, dStage_roomDt_c_getPlayerNum);

/* 025C47BC: dStage_roomDt_c::setRoom (vtable +05C) */
static void dStage_roomDt_c_setRoom(u32 self, u32 v) {
    WWHD_FUNC(0x025C47BC, void, self, v);
    OSReport_l(0x10055728); /* "Room non room data !!\n" */
    JUT_ASSERT_l(0x10055460, 0x6F0, 0x10055330);
}
VERIFY(0x025C47BC, dStage_roomDt_c_setRoom);

/* 025C4800: dStage_roomDt_c::getRoom (vtable +064) */
static u32 dStage_roomDt_c_getRoom(u32 self) {
    WWHD_FUNC(0x025C4800, u32, self);
    OSReport_l(0x10055740); /* "Room non room data !!\n" */
    JUT_ASSERT_l(0x1005546C, 0x6F5, 0x10055332);
    return 0;
}
VERIFY(0x025C4800, dStage_roomDt_c_getRoom);

/* 025C4848: dStage_roomDt_c::setMapInfo (vtable +06C) */
static void dStage_roomDt_c_setMapInfo(u32 self, u32 v) {
    WWHD_FUNC(0x025C4848, void, self, v);
    st(self + 0xC, v);
}
VERIFY(0x025C4848, dStage_roomDt_c_setMapInfo);

/* 025C4850: dStage_roomDt_c::getMapInfo (vtable +074) */
static u32 dStage_roomDt_c_getMapInfo(u32 self) {
    WWHD_FUNC(0x025C4850, u32, self);
    return ld(self + 0xC);
}
VERIFY(0x025C4850, dStage_roomDt_c_getMapInfo);

/* 025C4858: dStage_roomDt_c::setMapInfoBase (vtable +084) */
static void dStage_roomDt_c_setMapInfoBase(u32 self, u32 v) {
    WWHD_FUNC(0x025C4858, void, self, v);
    st(self + 0x10, v);
}
VERIFY(0x025C4858, dStage_roomDt_c_setMapInfoBase);

/* 025C4860: dStage_roomDt_c::getMapInfoBase (vtable +08C) */
static u32 dStage_roomDt_c_getMapInfoBase(u32 self) {
    WWHD_FUNC(0x025C4860, u32, self);
    return ld(self + 0x10);
}
VERIFY(0x025C4860, dStage_roomDt_c_getMapInfoBase);

/* 025C4868: dStage_roomDt_c::setPaletInfo (vtable +094) */
static void dStage_roomDt_c_setPaletInfo(u32 self, u32 v) {
    WWHD_FUNC(0x025C4868, void, self, v);
    OSReport_l(0x100553C4); /* "Room non palet data !!\n" */
    JUT_ASSERT_l(0x10055478, 0x717, 0x10055334);
}
VERIFY(0x025C4868, dStage_roomDt_c_setPaletInfo);

/* 025C48AC: dStage_roomDt_c::getPaletInfo (vtable +09C) */
static u32 dStage_roomDt_c_getPaletInfo(u32 self) {
    WWHD_FUNC(0x025C48AC, u32, self);
    OSReport_l(0x100553DC); /* "Room non palet data !!\n" */
    JUT_ASSERT_l(0x10055484, 0x71B, 0x10055336);
    return 0;
}
VERIFY(0x025C48AC, dStage_roomDt_c_getPaletInfo);

/* 025C48F4: dStage_roomDt_c::setPselectInfo (vtable +0A4) */
static void dStage_roomDt_c_setPselectInfo(u32 self, u32 v) {
    WWHD_FUNC(0x025C48F4, void, self, v);
    OSReport_l(0x10055490); /* "Room non pselect data !!\n" */
    JUT_ASSERT_l(0x100554AC, 0x722, 0x10055338);
}
VERIFY(0x025C48F4, dStage_roomDt_c_setPselectInfo);

/* 025C4938: dStage_roomDt_c::getPselectInfo (vtable +0AC) */
static u32 dStage_roomDt_c_getPselectInfo(u32 self) {
    WWHD_FUNC(0x025C4938, u32, self);
    OSReport_l(0x100554B8); /* "Room non pselect data !!\n" */
    JUT_ASSERT_l(0x100554D4, 0x726, 0x1005533A);
    return 0;
}
VERIFY(0x025C4938, dStage_roomDt_c_getPselectInfo);

/* 025C4980: dStage_roomDt_c::setEnvrInfo (vtable +0B4) */
static void dStage_roomDt_c_setEnvrInfo(u32 self, u32 v) {
    WWHD_FUNC(0x025C4980, void, self, v);
    OSReport_l(0x10055758); /* "Room non envr data !!\n" */
    JUT_ASSERT_l(0x100554E0, 0x72D, 0x1005533C);
}
VERIFY(0x025C4980, dStage_roomDt_c_setEnvrInfo);

/* 025C49C4: dStage_roomDt_c::getEnvrInfo (vtable +0BC) */
static u32 dStage_roomDt_c_getEnvrInfo(u32 self) {
    WWHD_FUNC(0x025C49C4, u32, self);
    OSReport_l(0x10055770); /* "Room non envr data !!\n" */
    JUT_ASSERT_l(0x100554EC, 0x731, 0x1005533E);
    return 0;
}
VERIFY(0x025C49C4, dStage_roomDt_c_getEnvrInfo);

/* 025C4A0C: dStage_roomDt_c::setVrboxInfo (vtable +0C4) */
static void dStage_roomDt_c_setVrboxInfo(u32 self, u32 v) {
    WWHD_FUNC(0x025C4A0C, void, self, v);
    st(self + 0x14, v);
}
VERIFY(0x025C4A0C, dStage_roomDt_c_setVrboxInfo);

/* 025C4A14: dStage_roomDt_c::getVrboxInfo (vtable +0CC) */
static u32 dStage_roomDt_c_getVrboxInfo(u32 self) {
    WWHD_FUNC(0x025C4A14, u32, self);
    return ld(self + 0x14);
}
VERIFY(0x025C4A14, dStage_roomDt_c_getVrboxInfo);

/* 025C4A1C: dStage_roomDt_c::setPlightInfo (vtable +0D4) */
static void dStage_roomDt_c_setPlightInfo(u32 self, u32 v) {
    WWHD_FUNC(0x025C4A1C, void, self, v);
    OSReport_l(0x10055788); /* "Room non plight data !!\n" */
    JUT_ASSERT_l(0x100554F8, 0x745, 0x10055340);
}
VERIFY(0x025C4A1C, dStage_roomDt_c_setPlightInfo);

/* 025C4A60: dStage_roomDt_c::getPlightInfo (vtable +0DC) */
static u32 dStage_roomDt_c_getPlightInfo(u32 self) {
    WWHD_FUNC(0x025C4A60, u32, self);
    OSReport_l(0x100557A4); /* "Room non plight data !!\n" */
    JUT_ASSERT_l(0x10055504, 0x749, 0x10055342);
    return 0;
}
VERIFY(0x025C4A60, dStage_roomDt_c_getPlightInfo);

/* 025C4AA8: dStage_roomDt_c::setPlightNumInfo (vtable +124) */
static void dStage_roomDt_c_setPlightNumInfo(u32 self, u32 v) {
    WWHD_FUNC(0x025C4AA8, void, self, v);
    OSReport_l(0x100557C8); /* "Room non plight num data !!\n" */
    JUT_ASSERT_l(0x10055528, 0x776, 0x10055348);
}
VERIFY(0x025C4AA8, dStage_roomDt_c_setPlightNumInfo);

/* 025C4AEC: dStage_roomDt_c::getPlightNumInfo (vtable +12C) */
static u32 dStage_roomDt_c_getPlightNumInfo(u32 self) {
    WWHD_FUNC(0x025C4AEC, u32, self);
    OSReport_l(0x100557E8); /* "Room non plight num data !!\n" */
    JUT_ASSERT_l(0x10055534, 0x77B, 0x1005534A);
    return 0;
}
VERIFY(0x025C4AEC, dStage_roomDt_c_getPlightNumInfo);

/* 025C4B34: dStage_roomDt_c::setLightVecInfo (vtable +134) */
static void dStage_roomDt_c_setLightVecInfo(u32 self, u32 v) {
    WWHD_FUNC(0x025C4B34, void, self, v);
    st(self + 0x4, v);
}
VERIFY(0x025C4B34, dStage_roomDt_c_setLightVecInfo);

/* 025C4B3C: dStage_roomDt_c::setLightVecInfoNum (vtable +144) */
static void dStage_roomDt_c_setLightVecInfoNum(u32 self, u32 v) {
    WWHD_FUNC(0x025C4B3C, void, self, v);
    st(self + 0x8, v);
}
VERIFY(0x025C4B3C, dStage_roomDt_c_setLightVecInfoNum);

/* 025C4B44: dStage_roomDt_c::setStagInfo (vtable +154) */
static void dStage_roomDt_c_setStagInfo(u32 self, u32 v) {
    WWHD_FUNC(0x025C4B44, void, self, v);
    OSReport_l(0x10055808); /* "Room non stag data !!\n" */
    JUT_ASSERT_l(0x10055540, 0x79C, 0x1005534C);
}
VERIFY(0x025C4B44, dStage_roomDt_c_setStagInfo);

/* 025C4B88: dStage_roomDt_c::getStagInfo (vtable +15C) */
static u32 dStage_roomDt_c_getStagInfo(u32 self) {
    WWHD_FUNC(0x025C4B88, u32, self);
    OSReport_l(0x10055820); /* "Room non stag data !!\n" */
    JUT_ASSERT_l(0x1005554C, 0x7A0, 0x1005534E);
    return 0;
}
VERIFY(0x025C4B88, dStage_roomDt_c_getStagInfo);

/* 025C4BD0: dStage_roomDt_c::setSclsInfo (vtable +164) */
static void dStage_roomDt_c_setSclsInfo(u32 self, u32 v) {
    WWHD_FUNC(0x025C4BD0, void, self, v);
    st(self + 0x38, v);
}
VERIFY(0x025C4BD0, dStage_roomDt_c_setSclsInfo);

/* 025C4BD8: dStage_roomDt_c::getSclsInfo (vtable +16C) */
static u32 dStage_roomDt_c_getSclsInfo(u32 self) {
    WWHD_FUNC(0x025C4BD8, u32, self);
    return ld(self + 0x38);
}
VERIFY(0x025C4BD8, dStage_roomDt_c_getSclsInfo);

/* 025C4BE0: dStage_roomDt_c::setPntInfo (vtable +174) */
static void dStage_roomDt_c_setPntInfo(u32 self, u32 v) {
    WWHD_FUNC(0x025C4BE0, void, self, v);
    OSReport_l(0x10055838); /* "Room non Pnt data !\n" */
    JUT_ASSERT_l(0x10055558, 0x7B9, 0x10055350);
}
VERIFY(0x025C4BE0, dStage_roomDt_c_setPntInfo);

/* 025C4C24: dStage_roomDt_c::getPntInf (vtable +17C) */
static u32 dStage_roomDt_c_getPntInf(u32 self) {
    WWHD_FUNC(0x025C4C24, u32, self);
    OSReport_l(0x10055564); /* "Room non Pnts data !\n" */
    JUT_ASSERT_l(0x1005557C, 0x7BD, 0x10055352);
    return 0;
}
VERIFY(0x025C4C24, dStage_roomDt_c_getPntInf);

/* 025C4C6C: dStage_roomDt_c::setPathInfo (vtable +184) */
static void dStage_roomDt_c_setPathInfo(u32 self, u32 v) {
    WWHD_FUNC(0x025C4C6C, void, self, v);
    OSReport_l(0x10055588); /* "Room non Path data !\n" */
    JUT_ASSERT_l(0x100555A0, 0x7C4, 0x10055354);
}
VERIFY(0x025C4C6C, dStage_roomDt_c_setPathInfo);

/* 025C4CB0: dStage_roomDt_c::getPathInf (vtable +18C) */
static u32 dStage_roomDt_c_getPathInf(u32 self) {
    WWHD_FUNC(0x025C4CB0, u32, self);
    OSReport_l(0x100555AC); /* "Room non Path data !\n" */
    JUT_ASSERT_l(0x100555C4, 0x7C8, 0x10055356);
    return 0;
}
VERIFY(0x025C4CB0, dStage_roomDt_c_getPathInf);

/* 025C4CF8: dStage_roomDt_c::setPnt2Info (vtable +194) */
static void dStage_roomDt_c_setPnt2Info(u32 self, u32 v) {
    WWHD_FUNC(0x025C4CF8, void, self, v);
    st(self + 0x24, v);
}
VERIFY(0x025C4CF8, dStage_roomDt_c_setPnt2Info);

/* 025C4D00: dStage_roomDt_c::getPnt2Inf (vtable +19C) */
static u32 dStage_roomDt_c_getPnt2Inf(u32 self) {
    WWHD_FUNC(0x025C4D00, u32, self);
    return ld(self + 0x24);
}
VERIFY(0x025C4D00, dStage_roomDt_c_getPnt2Inf);

/* 025C4D08: dStage_roomDt_c::setPath2Info (vtable +1A4) */
static void dStage_roomDt_c_setPath2Info(u32 self, u32 v) {
    WWHD_FUNC(0x025C4D08, void, self, v);
    st(self + 0x28, v);
}
VERIFY(0x025C4D08, dStage_roomDt_c_setPath2Info);

/* 025C4D10: dStage_roomDt_c::getPath2Inf (vtable +1AC) */
static u32 dStage_roomDt_c_getPath2Inf(u32 self) {
    WWHD_FUNC(0x025C4D10, u32, self);
    return ld(self + 0x28);
}
VERIFY(0x025C4D10, dStage_roomDt_c_getPath2Inf);

/* 025C4D18: dStage_roomDt_c::setSoundInfo (vtable +1B4) */
static void dStage_roomDt_c_setSoundInfo(u32 self, u32 v) {
    WWHD_FUNC(0x025C4D18, void, self, v);
    st(self + 0x34, v);
}
VERIFY(0x025C4D18, dStage_roomDt_c_setSoundInfo);

/* 025C4D20: dStage_roomDt_c::getSoundInf (vtable +1BC) */
static u32 dStage_roomDt_c_getSoundInf(u32 self) {
    WWHD_FUNC(0x025C4D20, u32, self);
    return ld(self + 0x34);
}
VERIFY(0x025C4D20, dStage_roomDt_c_getSoundInf);

/* 025C4D28: dStage_roomDt_c::setEventInfo (vtable +1C4) */
static void dStage_roomDt_c_setEventInfo(u32 self, u32 v) {
    WWHD_FUNC(0x025C4D28, void, self, v);
    OSReport_l(0x100555D0); /* "Room non event data!\n" */
    JUT_ASSERT_l(0x100555E8, 0x7FC, 0x10055358);
}
VERIFY(0x025C4D28, dStage_roomDt_c_setEventInfo);

/* 025C4D6C: dStage_roomDt_c::getEventInfo (vtable +1CC) */
static u32 dStage_roomDt_c_getEventInfo(u32 self) {
    WWHD_FUNC(0x025C4D6C, u32, self);
    OSReport_l(0x100555F4); /* "Room non event data!\n" */
    JUT_ASSERT_l(0x1005560C, 0x800, 0x1005535A);
    return 0;
}
VERIFY(0x025C4D6C, dStage_roomDt_c_getEventInfo);

/* 025C4DB4: dStage_roomDt_c::setFileListInfo (vtable +1D4) */
static void dStage_roomDt_c_setFileListInfo(u32 self, u32 v) {
    WWHD_FUNC(0x025C4DB4, void, self, v);
    st(self + 0x18, v);
}
VERIFY(0x025C4DB4, dStage_roomDt_c_setFileListInfo);

/* 025C4DBC: dStage_roomDt_c::getFileListInfo (vtable +1DC) */
static u32 dStage_roomDt_c_getFileListInfo(u32 self) {
    WWHD_FUNC(0x025C4DBC, u32, self);
    return ld(self + 0x18);
}
VERIFY(0x025C4DBC, dStage_roomDt_c_getFileListInfo);

/* 025C4DC4: dStage_roomDt_c::setFloorInfo (vtable +1E4) */
static void dStage_roomDt_c_setFloorInfo(u32 self, u32 v) {
    WWHD_FUNC(0x025C4DC4, void, self, v);
    st(self + 0x48, v);
}
VERIFY(0x025C4DC4, dStage_roomDt_c_setFloorInfo);

/* 025C4DCC: dStage_roomDt_c::getFloorInfo (vtable +1EC) */
static u32 dStage_roomDt_c_getFloorInfo(u32 self) {
    WWHD_FUNC(0x025C4DCC, u32, self);
    return ld(self + 0x48);
}
VERIFY(0x025C4DCC, dStage_roomDt_c_getFloorInfo);

/* 025C4DD4: dStage_roomDt_c::setMemoryConfig (vtable +1F4) */
static void dStage_roomDt_c_setMemoryConfig(u32 self, u32 v) {
    WWHD_FUNC(0x025C4DD4, void, self, v);
    st(self + 0x1C, v);
}
VERIFY(0x025C4DD4, dStage_roomDt_c_setMemoryConfig);

/* 025C4DDC: dStage_roomDt_c::getMemoryConfig (vtable +1FC) */
static u32 dStage_roomDt_c_getMemoryConfig(u32 self) {
    WWHD_FUNC(0x025C4DDC, u32, self);
    return ld(self + 0x1C);
}
VERIFY(0x025C4DDC, dStage_roomDt_c_getMemoryConfig);

/* 025C4DE4: dStage_roomDt_c::setMemoryMap (vtable +204) */
static void dStage_roomDt_c_setMemoryMap(u32 self, u32 v) {
    WWHD_FUNC(0x025C4DE4, void, self, v);
    OSReport_l(0x10055618); /* "Room non multi data!\n" */
    JUT_ASSERT_l(0x10055630, 0x878, 0x1005535C);
}
VERIFY(0x025C4DE4, dStage_roomDt_c_setMemoryMap);

/* 025C4E28: dStage_roomDt_c::getMemoryMap (vtable +20C) */
static u32 dStage_roomDt_c_getMemoryMap(u32 self) {
    WWHD_FUNC(0x025C4E28, u32, self);
    OSReport_l(0x1005563C); /* "Room non multi data!\n" */
    JUT_ASSERT_l(0x10055654, 0x87D, 0x1005535E);
    return 0;
}
VERIFY(0x025C4E28, dStage_roomDt_c_getMemoryMap);

/* 025C4E70: dStage_roomDt_c::setShip (vtable +214) */
static void dStage_roomDt_c_setShip(u32 self, u32 v) {
    WWHD_FUNC(0x025C4E70, void, self, v);
    st(self + 0x3C, v);
}
VERIFY(0x025C4E70, dStage_roomDt_c_setShip);

/* 025C4E78: dStage_roomDt_c::getShip (vtable +21C) */
static u32 dStage_roomDt_c_getShip(u32 self) {
    WWHD_FUNC(0x025C4E78, u32, self);
    return ld(self + 0x3C);
}
VERIFY(0x025C4E78, dStage_roomDt_c_getShip);

/* 025C4E80: dStage_roomDt_c::setMulti (vtable +224) */
static void dStage_roomDt_c_setMulti(u32 self, u32 v) {
    WWHD_FUNC(0x025C4E80, void, self, v);
    st(self + 0x40, v);
}
VERIFY(0x025C4E80, dStage_roomDt_c_setMulti);

/* 025C4E88: dStage_roomDt_c vtable +0x234 (GameCube slot setLbnk): HD: an assert only */
static void dStage_roomDt_c_setLbnk(u32 self, u32 v) {
    WWHD_FUNC(0x025C4E88, void, self, v);
    JUT_ASSERT_l(0x10055660, 0x8A9, 0x10055360);
}
VERIFY(0x025C4E88, dStage_roomDt_c_setLbnk);

/* 025C4EA0: dStage_roomDt_c vtable +0x23C (GameCube slot getLbnk): HD: an assert, NULL */
static u32 dStage_roomDt_c_getLbnk(u32 self) {
    WWHD_FUNC(0x025C4EA0, u32, self);
    JUT_ASSERT_l(0x1005566C, 0x8AE, 0x10055362);
    return 0;
}
VERIFY(0x025C4EA0, dStage_roomDt_c_getLbnk);

/* 025C4ED8: dStage_roomDt_c::setTresure (vtable +244) */
static void dStage_roomDt_c_setTresure(u32 self, u32 v) {
    WWHD_FUNC(0x025C4ED8, void, self, v);
    st(self + 0x44, v);
}
VERIFY(0x025C4ED8, dStage_roomDt_c_setTresure);

/* 025C4EE0: dStage_stageDt_c::getRoomNo (vtable +014) */
static s32 dStage_stageDt_c_getRoomNo(u32 self) {
    WWHD_FUNC(0x025C4EE0, s32, self);
    return -1;
}
VERIFY(0x025C4EE0, dStage_stageDt_c_getRoomNo);

/* 025C4EE8: dStage_stageDt_c::setCamera (vtable +01C) */
static void dStage_stageDt_c_setCamera(u32 self, u32 v) {
    WWHD_FUNC(0x025C4EE8, void, self, v);
    st(self + 0x4, v);
}
VERIFY(0x025C4EE8, dStage_stageDt_c_setCamera);

/* 025C4EF0: dStage_stageDt_c::getCamera (vtable +024) */
static u32 dStage_stageDt_c_getCamera(u32 self) {
    WWHD_FUNC(0x025C4EF0, u32, self);
    return ld(self + 0x4);
}
VERIFY(0x025C4EF0, dStage_stageDt_c_getCamera);

/* 025C4EF8: dStage_stageDt_c::setArrow (vtable +02C) */
static void dStage_stageDt_c_setArrow(u32 self, u32 v) {
    WWHD_FUNC(0x025C4EF8, void, self, v);
    st(self + 0x8, v);
}
VERIFY(0x025C4EF8, dStage_stageDt_c_setArrow);

/* 025C4F00: dStage_stageDt_c::getArrow (vtable +034) */
static u32 dStage_stageDt_c_getArrow(u32 self) {
    WWHD_FUNC(0x025C4F00, u32, self);
    return ld(self + 0x8);
}
VERIFY(0x025C4F00, dStage_stageDt_c_getArrow);

/* 025C4F08: dStage_stageDt_c::setPlayer (vtable +03C) */
static void dStage_stageDt_c_setPlayer(u32 self, u32 v) {
    WWHD_FUNC(0x025C4F08, void, self, v);
    st(self + 0xC, v);
}
VERIFY(0x025C4F08, dStage_stageDt_c_setPlayer);

/* 025C4F10: dStage_stageDt_c::getPlayer (vtable +044) */
static u32 dStage_stageDt_c_getPlayer(u32 self) {
    WWHD_FUNC(0x025C4F10, u32, self);
    return ld(self + 0xC);
}
VERIFY(0x025C4F10, dStage_stageDt_c_getPlayer);

/* 025C4F18: dStage_stageDt_c::setPlayerNum (vtable +04C) */
static void dStage_stageDt_c_setPlayerNum(u32 self, u32 v) {
    WWHD_FUNC(0x025C4F18, void, self, v);
    st16(self + 0x44, (u16)v);
}
VERIFY(0x025C4F18, dStage_stageDt_c_setPlayerNum);

/* 025C4F20: dStage_stageDt_c::getPlayerNum (vtable +054) */
static u16 dStage_stageDt_c_getPlayerNum(u32 self) {
    WWHD_FUNC(0x025C4F20, u16, self);
    return ld16(self + 0x44);
}
VERIFY(0x025C4F20, dStage_stageDt_c_getPlayerNum);

/* 025C4F28: dStage_stageDt_c::setRoom (vtable +05C) */
static void dStage_stageDt_c_setRoom(u32 self, u32 v) {
    WWHD_FUNC(0x025C4F28, void, self, v);
    st(self + 0x10, v);
}
VERIFY(0x025C4F28, dStage_stageDt_c_setRoom);

/* 025C4F30: dStage_stageDt_c::getRoom (vtable +064) */
static u32 dStage_stageDt_c_getRoom(u32 self) {
    WWHD_FUNC(0x025C4F30, u32, self);
    return ld(self + 0x10);
}
VERIFY(0x025C4F30, dStage_stageDt_c_getRoom);

/* 025C4F38: dStage_stageDt_c::setMapInfo (vtable +06C) */
static void dStage_stageDt_c_setMapInfo(u32 self, u32 v) {
    WWHD_FUNC(0x025C4F38, void, self, v);
    st(self + 0x14, v);
}
VERIFY(0x025C4F38, dStage_stageDt_c_setMapInfo);

/* 025C4F40: dStage_stageDt_c::getMapInfo (vtable +074) */
static u32 dStage_stageDt_c_getMapInfo(u32 self) {
    WWHD_FUNC(0x025C4F40, u32, self);
    return ld(self + 0x14);
}
VERIFY(0x025C4F40, dStage_stageDt_c_getMapInfo);

/* 025C4F48: dStage_stageDt_c::setMapInfoBase (vtable +084) */
static void dStage_stageDt_c_setMapInfoBase(u32 self, u32 v) {
    WWHD_FUNC(0x025C4F48, void, self, v);
    st(self + 0x18, v);
}
VERIFY(0x025C4F48, dStage_stageDt_c_setMapInfoBase);

/* 025C4F50: dStage_stageDt_c::setPaletInfo (vtable +094) */
static void dStage_stageDt_c_setPaletInfo(u32 self, u32 v) {
    WWHD_FUNC(0x025C4F50, void, self, v);
    st(self + 0x1C, v);
}
VERIFY(0x025C4F50, dStage_stageDt_c_setPaletInfo);

/* 025C4F58: dStage_stageDt_c::getPaletInfo (vtable +09C) */
static u32 dStage_stageDt_c_getPaletInfo(u32 self) {
    WWHD_FUNC(0x025C4F58, u32, self);
    return ld(self + 0x1C);
}
VERIFY(0x025C4F58, dStage_stageDt_c_getPaletInfo);

/* 025C4F60: dStage_stageDt_c::setPselectInfo (vtable +0A4) */
static void dStage_stageDt_c_setPselectInfo(u32 self, u32 v) {
    WWHD_FUNC(0x025C4F60, void, self, v);
    st(self + 0x20, v);
}
VERIFY(0x025C4F60, dStage_stageDt_c_setPselectInfo);

/* 025C4F68: dStage_stageDt_c::getPselectInfo (vtable +0AC) */
static u32 dStage_stageDt_c_getPselectInfo(u32 self) {
    WWHD_FUNC(0x025C4F68, u32, self);
    return ld(self + 0x20);
}
VERIFY(0x025C4F68, dStage_stageDt_c_getPselectInfo);

/* 025C4F70: dStage_stageDt_c::setEnvrInfo (vtable +0B4) */
static void dStage_stageDt_c_setEnvrInfo(u32 self, u32 v) {
    WWHD_FUNC(0x025C4F70, void, self, v);
    st(self + 0x24, v);
}
VERIFY(0x025C4F70, dStage_stageDt_c_setEnvrInfo);

/* 025C4F78: dStage_stageDt_c::getEnvrInfo (vtable +0BC) */
static u32 dStage_stageDt_c_getEnvrInfo(u32 self) {
    WWHD_FUNC(0x025C4F78, u32, self);
    return ld(self + 0x24);
}
VERIFY(0x025C4F78, dStage_stageDt_c_getEnvrInfo);

/* 025C4F80: dStage_stageDt_c::setVrboxInfo (vtable +0C4) */
static void dStage_stageDt_c_setVrboxInfo(u32 self, u32 v) {
    WWHD_FUNC(0x025C4F80, void, self, v);
    st(self + 0x28, v);
}
VERIFY(0x025C4F80, dStage_stageDt_c_setVrboxInfo);

/* 025C4F88: dStage_stageDt_c::getVrboxInfo (vtable +0CC) */
static u32 dStage_stageDt_c_getVrboxInfo(u32 self) {
    WWHD_FUNC(0x025C4F88, u32, self);
    return ld(self + 0x28);
}
VERIFY(0x025C4F88, dStage_stageDt_c_getVrboxInfo);

/* 025C4F90: dStage_stageDt_c::setPlightInfo (vtable +0D4) */
static void dStage_stageDt_c_setPlightInfo(u32 self, u32 v) {
    WWHD_FUNC(0x025C4F90, void, self, v);
    st(self + 0x2C, v);
}
VERIFY(0x025C4F90, dStage_stageDt_c_setPlightInfo);

/* 025C4F98: dStage_stageDt_c::getPlightInfo (vtable +0DC) */
static u32 dStage_stageDt_c_getPlightInfo(u32 self) {
    WWHD_FUNC(0x025C4F98, u32, self);
    return ld(self + 0x2C);
}
VERIFY(0x025C4F98, dStage_stageDt_c_getPlightInfo);

/* 025C4FA0: dStage_stageDt_c::setLightVecInfo (vtable +134) */
static void dStage_stageDt_c_setLightVecInfo(u32 self, u32 v) {
    WWHD_FUNC(0x025C4FA0, void, self, v);
    OSReport_l(0x10055408); /* "stage non LightVec data !!\n" */
    JUT_ASSERT_l(0x10055678, 0xA46, 0x10055364);
}
VERIFY(0x025C4FA0, dStage_stageDt_c_setLightVecInfo);

/* 025C4FE4: dStage_stageDt_c::setLightVecInfoNum (vtable +144) */
static void dStage_stageDt_c_setLightVecInfoNum(u32 self, u32 v) {
    WWHD_FUNC(0x025C4FE4, void, self, v);
    OSReport_l(0x10055850); /* "stage non LightVecNum data !!\n" */
    JUT_ASSERT_l(0x10055688, 0xA51, 0x10055368);
}
VERIFY(0x025C4FE4, dStage_stageDt_c_setLightVecInfoNum);

/* 025C5028: dStage_stageDt_c::setPlightNumInfo (vtable +124) */
static void dStage_stageDt_c_setPlightNumInfo(u32 self, u32 v) {
    WWHD_FUNC(0x025C5028, void, self, v);
    st(self + 0x40, v);
}
VERIFY(0x025C5028, dStage_stageDt_c_setPlightNumInfo);

/* 025C5030: dStage_stageDt_c::getPlightNumInfo (vtable +12C) */
static u32 dStage_stageDt_c_getPlightNumInfo(u32 self) {
    WWHD_FUNC(0x025C5030, u32, self);
    return ld(self + 0x40);
}
VERIFY(0x025C5030, dStage_stageDt_c_getPlightNumInfo);

/* 025C5038: dStage_stageDt_c::setStagInfo (vtable +154) */
static void dStage_stageDt_c_setStagInfo(u32 self, u32 v) {
    WWHD_FUNC(0x025C5038, void, self, v);
    st(self + 0x48, v);
}
VERIFY(0x025C5038, dStage_stageDt_c_setStagInfo);

/* 025C5040: dStage_stageDt_c::getStagInfo (vtable +15C) */
static u32 dStage_stageDt_c_getStagInfo(u32 self) {
    WWHD_FUNC(0x025C5040, u32, self);
    return ld(self + 0x48);
}
VERIFY(0x025C5040, dStage_stageDt_c_getStagInfo);

/* 025C5048: dStage_stageDt_c::setSclsInfo (vtable +164) */
static void dStage_stageDt_c_setSclsInfo(u32 self, u32 v) {
    WWHD_FUNC(0x025C5048, void, self, v);
    st(self + 0x4C, v);
}
VERIFY(0x025C5048, dStage_stageDt_c_setSclsInfo);

/* 025C5050: dStage_stageDt_c::getSclsInfo (vtable +16C) */
static u32 dStage_stageDt_c_getSclsInfo(u32 self) {
    WWHD_FUNC(0x025C5050, u32, self);
    return ld(self + 0x4C);
}
VERIFY(0x025C5050, dStage_stageDt_c_getSclsInfo);

/* 025C5058: dStage_stageDt_c::setPntInfo (vtable +174) */
static void dStage_stageDt_c_setPntInfo(u32 self, u32 v) {
    WWHD_FUNC(0x025C5058, void, self, v);
    st(self + 0x50, v);
}
VERIFY(0x025C5058, dStage_stageDt_c_setPntInfo);

/* 025C5060: dStage_stageDt_c::getPntInf (vtable +17C) */
static u32 dStage_stageDt_c_getPntInf(u32 self) {
    WWHD_FUNC(0x025C5060, u32, self);
    return ld(self + 0x50);
}
VERIFY(0x025C5060, dStage_stageDt_c_getPntInf);

/* 025C5068: dStage_stageDt_c::setPathInfo (vtable +184) */
static void dStage_stageDt_c_setPathInfo(u32 self, u32 v) {
    WWHD_FUNC(0x025C5068, void, self, v);
    st(self + 0x54, v);
}
VERIFY(0x025C5068, dStage_stageDt_c_setPathInfo);

/* 025C5070: dStage_stageDt_c::getPathInf (vtable +18C) */
static u32 dStage_stageDt_c_getPathInf(u32 self) {
    WWHD_FUNC(0x025C5070, u32, self);
    return ld(self + 0x54);
}
VERIFY(0x025C5070, dStage_stageDt_c_getPathInf);

/* 025C5078: dStage_stageDt_c::setPnt2Info (vtable +194) */
static void dStage_stageDt_c_setPnt2Info(u32 self, u32 v) {
    WWHD_FUNC(0x025C5078, void, self, v);
    st(self + 0x58, v);
}
VERIFY(0x025C5078, dStage_stageDt_c_setPnt2Info);

/* 025C5080: dStage_stageDt_c::getPnt2Inf (vtable +19C) */
static u32 dStage_stageDt_c_getPnt2Inf(u32 self) {
    WWHD_FUNC(0x025C5080, u32, self);
    return ld(self + 0x58);
}
VERIFY(0x025C5080, dStage_stageDt_c_getPnt2Inf);

/* 025C5088: dStage_stageDt_c::setPath2Info (vtable +1A4) */
static void dStage_stageDt_c_setPath2Info(u32 self, u32 v) {
    WWHD_FUNC(0x025C5088, void, self, v);
    st(self + 0x5C, v);
}
VERIFY(0x025C5088, dStage_stageDt_c_setPath2Info);

/* 025C5090: dStage_stageDt_c::getPath2Inf (vtable +1AC) */
static u32 dStage_stageDt_c_getPath2Inf(u32 self) {
    WWHD_FUNC(0x025C5090, u32, self);
    return ld(self + 0x5C);
}
VERIFY(0x025C5090, dStage_stageDt_c_getPath2Inf);

/* 025C5098: dStage_stageDt_c::setSoundInfo (vtable +1B4) */
static void dStage_stageDt_c_setSoundInfo(u32 self, u32 v) {
    WWHD_FUNC(0x025C5098, void, self, v);
    st(self + 0x60, v);
}
VERIFY(0x025C5098, dStage_stageDt_c_setSoundInfo);

/* 025C50A0: dStage_stageDt_c::setEventInfo (vtable +1C4) */
static void dStage_stageDt_c_setEventInfo(u32 self, u32 v) {
    WWHD_FUNC(0x025C50A0, void, self, v);
    st(self + 0x64, v);
}
VERIFY(0x025C50A0, dStage_stageDt_c_setEventInfo);

/* 025C50A8: dStage_stageDt_c::getEventInfo (vtable +1CC) */
static u32 dStage_stageDt_c_getEventInfo(u32 self) {
    WWHD_FUNC(0x025C50A8, u32, self);
    return ld(self + 0x64);
}
VERIFY(0x025C50A8, dStage_stageDt_c_getEventInfo);

/* 025C50B0: dStage_stageDt_c::setFileListInfo (vtable +1D4) */
static void dStage_stageDt_c_setFileListInfo(u32 self, u32 v) {
    WWHD_FUNC(0x025C50B0, void, self, v);
    OSReport_l(0x10055698); /* "stage non filelist data!\n" */
    JUT_ASSERT_l(0x100556B4, 0xAD2, 0x1005536C);
}
VERIFY(0x025C50B0, dStage_stageDt_c_setFileListInfo);

/* 025C50F4: dStage_stageDt_c::setFloorInfo (vtable +1E4) */
static void dStage_stageDt_c_setFloorInfo(u32 self, u32 v) {
    WWHD_FUNC(0x025C50F4, void, self, v);
    st(self + 0x68, v);
}
VERIFY(0x025C50F4, dStage_stageDt_c_setFloorInfo);

/* 025C50FC: dStage_stageDt_c::getFloorInfo (vtable +1EC) */
static u32 dStage_stageDt_c_getFloorInfo(u32 self) {
    WWHD_FUNC(0x025C50FC, u32, self);
    return ld(self + 0x68);
}
VERIFY(0x025C50FC, dStage_stageDt_c_getFloorInfo);

/* 025C5104: dStage_stageDt_c::setMemoryConfig (vtable +1F4) */
static void dStage_stageDt_c_setMemoryConfig(u32 self, u32 v) {
    WWHD_FUNC(0x025C5104, void, self, v);
    OSReport_l(0x100556C8); /* "stage non SHIP data!\n" */
    JUT_ASSERT_l(0x100556E0, 0xB25, 0x10055370);
}
VERIFY(0x025C5104, dStage_stageDt_c_setMemoryConfig);

/* 025C5148: dStage_stageDt_c::setMemoryMap (vtable +204) */
static void dStage_stageDt_c_setMemoryMap(u32 self, u32 v) {
    WWHD_FUNC(0x025C5148, void, self, v);
    st(self + 0x6C, v);
}
VERIFY(0x025C5148, dStage_stageDt_c_setMemoryMap);

/* 025C5150: dStage_stageDt_c::getMemoryMap (vtable +20C) */
static u32 dStage_stageDt_c_getMemoryMap(u32 self) {
    WWHD_FUNC(0x025C5150, u32, self);
    return ld(self + 0x6C);
}
VERIFY(0x025C5150, dStage_stageDt_c_getMemoryMap);

/* 025C5158: dStage_stageDt_c::setShip (vtable +214) */
static void dStage_stageDt_c_setShip(u32 self, u32 v) {
    WWHD_FUNC(0x025C5158, void, self, v);
    OSReport_l(0x100556F0); /* "stage non Lbnk data!\n" */
    JUT_ASSERT_l(0x10055708, 0xB46, 0x10055374);
}
VERIFY(0x025C5158, dStage_stageDt_c_setShip);

/* 025C519C: dStage_stageDt_c::setMulti (vtable +224) */
static void dStage_stageDt_c_setMulti(u32 self, u32 v) {
    WWHD_FUNC(0x025C519C, void, self, v);
    st(self + 0x70, v);
}
VERIFY(0x025C519C, dStage_stageDt_c_setMulti);

/* 025C51A4: dStage_stageDt_c::setLbnk (vtable +234) */
static void dStage_stageDt_c_setLbnk(u32 self, u32 v) {
    WWHD_FUNC(0x025C51A4, void, self, v);
    st(self + 0x74, v);
}
VERIFY(0x025C51A4, dStage_stageDt_c_setLbnk);

/* 025C51AC: dStage_stageDt_c::getLbnk (vtable +23C) */
static u32 dStage_stageDt_c_getLbnk(u32 self) {
    WWHD_FUNC(0x025C51AC, u32, self);
    return ld(self + 0x74);
}
VERIFY(0x025C51AC, dStage_stageDt_c_getLbnk);

/* 025C51B4: dStage_stageDt_c::setTresure (vtable +244) */
static void dStage_stageDt_c_setTresure(u32 self, u32 v) {
    WWHD_FUNC(0x025C51B4, void, self, v);
    st(self + 0x78, v);
}
VERIFY(0x025C51B4, dStage_stageDt_c_setTresure);

/* 025C51BC: sead::SafeString::assureTerminationImpl_ (this unit's copy, SafeString vtable
 * +0x14: a literal needs nothing) */
static void SafeString_assureTerminationImpl(u32 self) {
    WWHD_FUNC(0x025C51BC, void, self);
}
VERIFY(0x025C51BC, SafeString_assureTerminationImpl);

/* 025C51C0: HD-only: RoomLoaderExceptionCallback (the crash-dump context of
 * dStage_dt_c_roomLoader, vtable 100553AC +0x14): prints the room file and stage pointers and a
 * hex dump of the first 0x800 bytes of the room file (128 rows of 16) */
static void RoomLoaderExceptionCallback(u32 self) {
    WWHD_FUNC(0x025C51C0, void, self);
    excPrint_l(0x10055428); /* "I am RoomLoaderExceptionCallback. \n" */
    excPrint1_l(0x10055870, ld(self + 8));
    excPrint1_l(0x10055718, ld(self + 0xC));
    u32 p = ld(self + 8);
    for (u32 row = 0x80; row != 0; row--) {
        excPrint1_l(0x10055378, p);
        for (u32 k = 0x10; k != 0; k--, p++) excPrint1_l(0x10055380, ld8(p));
        excPrint_l(0x10055388);
    }
}
VERIFY(0x025C51C0, RoomLoaderExceptionCallback);

} // namespace d_stage_5_cpp
