#pragma once
#include "wwhd.h"
class JFWSystem { public: static void firstInit(); static void init(); };
struct JFWHeap_l { u8 mHeader[0x10]; gptr<const char> mName; };
struct JFWHeapManager_l { u8 mHeader[0x20]; be<u32> mSystemHeap; };
WWHD_OFFSET(JFWHeap_l,mName,0x10);
WWHD_SIZE(JFWHeap_l,0x14);
WWHD_OFFSET(JFWHeapManager_l,mSystemHeap,0x20);
WWHD_SIZE(JFWHeapManager_l,0x24);
void JFWSystemAssert(const char*,s32,const char*);
JFWHeap_l* JFWCreateSystemHeap(u32,u32,u32);
void* JFWBecomeSystemHeap(void*);
u32 JFWBecomeCurrentHeap(void*,u32);
void* JFWCreateMainThread(void*,u32);
