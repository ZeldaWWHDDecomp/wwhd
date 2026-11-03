#pragma once
#include "wwhd.h"
#include "JSystem/JKernel/JKRHeap_base.h"
namespace jut_string_table {
struct StringVtable { u8 prefix[0xC]; be<u32> destroy; u8 pad10[4]; be<u32> assureTermination; };
struct String { gptr<const be<u8>> mText; gptr<StringVtable> mVtable; };
using Heap=JKRHeap_l;
struct StaticState { be<u32> words[4]; };
struct HeapSystem { be<u32> opaque; };
struct SystemSlot { gptr<HeapSystem> system; };
WWHD_SIZE(String,8); WWHD_OFFSET(String,mVtable,4); WWHD_SIZE(StringVtable,0x18);

WWHD_SIZE(StaticState,0x10); WWHD_SIZE(SystemSlot,4);
}
struct JUTStringTable {
    be<s32> mCount;
    gptr<jut_string_table::String> mStrings;
    be<u16> mCapacity;
    u8 padA[2];
    void allocate(s32,jut_string_table::Heap*);
    const be<u8>* get(u32);
    void set(u32,const gptr<const be<u8>>*);
};
WWHD_OFFSET(JUTStringTable,mStrings,4); WWHD_OFFSET(JUTStringTable,mCapacity,8); WWHD_SIZE(JUTStringTable,0xC);
jut_string_table::Heap* stringTableFreeHeap(jut_string_table::HeapSystem*,jut_string_table::String*);
jut_string_table::Heap* stringTableCurrentHeap(jut_string_table::HeapSystem*);
void stringTableRegisterStatic(void*);
void stringTableDelete(void*);
