/* HD sound objects and path strings; no GameCube counterpart. */
#pragma once
#include "bindings.h"

struct hdSound_typeInfo_c { be<u32> mParent; };
WWHD_SIZE(hdSound_typeInfo_c, 4);
struct hdSound_objectVt_c { u8 _00[0xC]; be<u32> mIsDerived; };
WWHD_OFFSET(hdSound_objectVt_c, mIsDerived, 0xC);
WWHD_SIZE(hdSound_objectVt_c, 0x10);
struct hdSound_object_c {
    /* 0x000 */ gptr<hdSound_objectVt_c> mVtable;
    /* 0x004 */ u8 _004[0x144 - 4];
    /* 0x144 */ gptr<void> mpArchiveHandle;
};
WWHD_OFFSET(hdSound_object_c, mpArchiveHandle, 0x144);
WWHD_SIZE(hdSound_object_c, 0x148);
struct hdSound_player_c {
    /* 0x00 */ u8 _00[0x14];
    /* 0x14 */ gptr<hdSound_object_c> mpHeap;
    /* 0x18 */ u8 _18[4];
    /* 0x1C */ gptr<hdSound_object_c> mpArchive;
};
WWHD_OFFSET(hdSound_player_c, mpHeap, 0x14);
WWHD_OFFSET(hdSound_player_c, mpArchive, 0x1C);
WWHD_SIZE(hdSound_player_c, 0x20);
struct hdSound_playerGlobal_c { gptr<hdSound_player_c> mpPlayer; };
WWHD_SIZE(hdSound_playerGlobal_c, 4);
struct hdSound_stringVt_c { u8 _00[0x14]; be<u32> mAssureTermination; };
WWHD_OFFSET(hdSound_stringVt_c, mAssureTermination, 0x14);
WWHD_SIZE(hdSound_stringVt_c, 0x18);
struct hdSound_string_c {
    /* 0x0 */ gptr<be<u8>> mStringTop;
    /* 0x4 */ gptr<hdSound_stringVt_c> mVtable;
    void setVtable(u32 address) { mVtable.v = address; }
    void setText(const char* text) { mStringTop.v = gabi::guest_addr(text); }
    void assureTermination() {
        gabi::call_ptr<void>(mVtable->mAssureTermination, this);
    }
};
WWHD_OFFSET(hdSound_string_c, mStringTop, 0);
WWHD_OFFSET(hdSound_string_c, mVtable, 4);
WWHD_SIZE(hdSound_string_c, 8);
struct hdSound_bufferedString_c : hdSound_string_c {
    /* 0x8 */ be<s32> mCapacity;
};
WWHD_OFFSET(hdSound_bufferedString_c, mCapacity, 8);
WWHD_SIZE(hdSound_bufferedString_c, 0xC);
struct hdSound_fixedString128_c : hdSound_bufferedString_c { be<u8> mBuffer[128]; };
struct hdSound_fixedString256_c : hdSound_bufferedString_c { be<u8> mBuffer[256]; };
WWHD_OFFSET(hdSound_fixedString128_c, mBuffer, 0xC);
WWHD_OFFSET(hdSound_fixedString256_c, mBuffer, 0xC);
WWHD_SIZE(hdSound_fixedString128_c, 0x8C);
WWHD_SIZE(hdSound_fixedString256_c, 0x10C);

void* hdSound_archiveFromHandle(void* handle);
u32 hdSound_archiveIdByName(void* archive, void* name);
u32 hdSound_archiveItemById(void* archive, u32 id);
void OSBlockMove(be<u8>* dest, be<u8>* source, s32 size, u32 flags);
