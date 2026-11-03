/* Audio task-thread dispatch; HD task thread is168 bytes and carries a native thread pointer. */
#pragma once
#include "wwhd.h"
struct JASTaskThreadView { be<u32> reserved; gptr<void> nativeThread; };
struct DvdMessage { be<u32> value; gptr<be<u32>> result; be<u32> callback; };
WWHD_OFFSET(JASTaskThreadView,nativeThread,4);
WWHD_SIZE(JASTaskThreadView,8);
WWHD_SIZE(DvdMessage,12);
using DvdCallback=void(*)(DvdMessage*);
namespace JASystem { namespace Dvd {
JASTaskThreadView* getThread();
u32 createThread(s32 priority,s32 messages,u32 stackSize,u32 flags);
void dvdThreadCheckBack(DvdMessage*);
u32 checkPassDvdT(u32 value,be<u32>* result,u32 callback);
}}
void* dvdThreadAllocate(u32,void*,s32);
JASTaskThreadView* dvdThreadConstruct(void*,s32,s32,u32,u32);
void dvdThreadResume(void*);
u32 dvdThreadSend(JASTaskThreadView*,DvdCallback,DvdMessage*,u32);
void dvdThreadRegister(void*);
