/* Compiler-emitted support alongside the HD audio translation units. */
#pragma once
#include "bindings.h"
#include "JSystem/JAudio/JASCompanionStatics.h"
struct AudioProfileState { be<u32> mState; };
WWHD_SIZE(AudioProfileState,4);
// Read-only layout view for the shared buffered-string termination entry.
struct AudioTerminatedBuffer {
    gptr<be<u8>> mBuffer;
    be<u32> mVtable;
    be<u32> mCapacity;
};
WWHD_OFFSET(AudioTerminatedBuffer,mCapacity,8);
WWHD_SIZE(AudioTerminatedBuffer,12);
AudioProfileState* constructAudioProfileState(AudioProfileState* state);
void destroyAudioProfileState(void* object,u32 flags);
void destroyAudioStringBase(void* object,u32 flags);
void terminateAudioString(AudioTerminatedBuffer* string);
void destroyAudioStringBuffer(void* object,u32 flags);
void audioStringBaseNoOp();
void destroyAudioStringBufferCopy(void* object,u32 flags);
void destroyAudioStringBufferStorage(void* object,u32 flags);
void* allocateAudioSupport(u32 size);
void freeAudioSupport(void* object);
void registerAudioSupport(AudioCompanionRecord* record);
