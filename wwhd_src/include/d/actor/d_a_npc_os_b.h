/* Os part B private declarations.
 * The actor uses the shared GameCube field names at their measured HD offsets.
 * HD: the HIO child starts four bytes earlier and both HIO vtables trail their fields;
 * compact member-function descriptors remain eight bytes. No shared actor declarations change.
 */
#pragma once
#include "d/actor/d_a_npc_os_local.h"
namespace os_b {
// HD: HIO vtables trail their fields; the child starts at offset four.
struct daNpc_Os_HIO2_c {
    be<f32> field_0x04, field_0x08, field_0x0C, field_0x10, field_0x14, field_0x18, field_0x1C, field_0x20;
    be<s16> field_0x24, field_0x26, field_0x28;
    u8 _pad[2];
    be<u32> vtable;
};
WWHD_SIZE(daNpc_Os_HIO2_c, 0x2C);
struct daNpc_Os_HIO_c {
    /* 0x00 */ be<s8> mNo;
    u8 _pad[3];
    /* 0x04 */ daNpc_Os_HIO2_c mOs2;
    /* 0x30 */ dNpc_HIO_l mNpc;
    /* 0x58 */ gptr<daNpc_Os_c> field_0x5C;
    /* 0x5C */ be<f32> field_0x60;
    /* 0x60 */ be<f32> field_0x64;
    /* 0x64 */ be<f32> field_0x68;
    /* 0x68 */ be<f32> field_0x6C;
    /* 0x6C */ be<f32> field_0x70;
    /* 0x70 */ be<f32> field_0x74;
    /* 0x74 */ be<f32> field_0x78;
    /* 0x78 */ be<f32> field_0x7C;
    /* 0x7C */ be<f32> field_0x80;
    /* 0x80 */ be<f32> field_0x84;
    /* 0x84 */ be<f32> field_0x88;
    /* 0x88 */ be<f32> field_0x8C;
    /* 0x8C */ be<f32> field_0x90;
    /* 0x90 */ be<f32> field_0x94;
    /* 0x94 */ be<f32> field_0x98;
    /* 0x98 */ be<f32> field_0x9C;
    /* 0x9C */ be<f32> field_0xA0;
    /* 0xA0 */ be<f32> field_0xA4;
    /* 0xA4 */ be<f32> field_0xA8;
    /* 0xA8 */ be<f32> field_0xAC;
    /* 0xAC */ be<f32> field_0xB0;
    be<u32> vtable;
};
WWHD_SIZE(daNpc_Os_HIO_c, 0xB4);
WWHD_OFFSET(daNpc_Os_HIO_c, mNo, 0x00);
WWHD_OFFSET(daNpc_Os_HIO_c, mOs2, 0x04);
WWHD_OFFSET(daNpc_Os_HIO_c, mNpc, 0x30);
WWHD_OFFSET(daNpc_Os_HIO_c, field_0x5C, 0x58);
WWHD_OFFSET(daNpc_Os_HIO_c, field_0x8C, 0x88);
WWHD_OFFSET(daNpc_Os_HIO_c, field_0xAC, 0xA8);
WWHD_OFFSET(daNpc_Os_HIO_c, field_0xB0, 0xAC);
WWHD_OFFSET(daNpc_Os_HIO2_c, field_0x08, 0x04);
WWHD_OFFSET(daNpc_Os_HIO2_c, field_0x24, 0x20);
WWHD_OFFSET(daNpc_Os_HIO2_c, field_0x26, 0x22);
WWHD_OFFSET(daNpc_Os_HIO2_c, field_0x28, 0x24);
enum : u32 { STATUS_GRAVITY = 8, NPC_NOT_CHANGE = 0x40, JA_SE_OBJ_OSTATUE_PUT = 0x58FE };
enum MessageStatus : u16 {
    MSG_WAIT_e = 6,
    MSG_DISPLAYED_e = 0xE,
    MSG_CONTINUES_e = 0xF,
    MSG_ENDS_e = 0x10,
    MSG_BOX_CLOSED_e = 0x12,
    MSG_DESTROYED_e = 0x13,
    MSG_OTHER_e = 0x15
};
struct BrkTableEntry {
    be<u8> unk_1, unk_2;
};
WWHD_SIZE(BrkTableEntry, 2);
struct PlayerRooms {
    be<u8> rooms[256];
};
WWHD_SIZE(PlayerRooms, 256);
struct SinCosTable {
    struct Entry {
        be<f32> sin, cos;
    };
    Entry m[8192];
};
WWHD_SIZE(SinCosTable, 0x10000);
struct Smoke {
    be<u32> vtable;
};
WWHD_SIZE(Smoke, 4);
struct Attention {
    u8 _pad[0x20];
    be<u32> flags;
};
WWHD_SIZE(Attention, 0x24);
WWHD_OFFSET(Attention, flags, 0x20);
struct Save {
    u8 _pad0[0x20];
    u8 savedata[0x1AC];
    u8 priest[0x47C];
    u8 event[0xB00];
    u8 restart[1];
};
WWHD_SIZE(Save, 0x1149);
WWHD_OFFSET(Save, savedata, 0x20);
WWHD_OFFSET(Save, restart, 0x1148);
WWHD_OFFSET(Save, priest, 0x1CC);
// HD: byte table indices wrap in the 32-bit guest address space, including invalid signed inputs.
inline BrkTableEntry* brkEntry(BrkTableEntry* table, s32 index) {
    return gabi::at<BrkTableEntry>(gabi::ea(table) + u32(index) * 2);
}
inline void offPlayerRoom(PlayerRooms* rooms, s8 index) {
    gabi::store<u8>(gabi::ea(rooms) + u32(s32(index)), 0);
}
inline s16 lowHalf(void* data) {
    return gabi::load<s16>(gabi::ea(data) + 2);
}
inline Save* save() {
    return gabi::at<Save>(gabi::load<u32>(0x101F84DC));
}
inline fopAc_ac_c* partner() {
    return gabi::at<dComIfG_inf_c>(dComIfGp_ea())->play.mpPlayerPtr[1];
}
inline void* camera() {
    return gabi::at<void>(gabi::at<dComIfG_inf_c>(dComIfGp_ea())->play.mCameraInfo[0].mpCamera);
}
inline void onEventBit(u16 flag) {
    gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x644)->onEventBit(flag);
}
inline Attention* attention() {
    return gabi::at<Attention>(dComIfGp_ea() + PLAY_OFF(mAttention));
}
inline void removeSmoke(Smoke* smoke) {
    gabi::call_ptr(gabi::load<u32>(u32(smoke->vtable) + 0x44), gabi::ea(smoke));
}
} // namespace os_b
