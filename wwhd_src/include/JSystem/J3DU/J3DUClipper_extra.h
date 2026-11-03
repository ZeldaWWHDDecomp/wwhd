#pragma once
#include "JSystem/J3DUClipper.h"
struct J3DUVec2 { be<f32> x,y; };
WWHD_SIZE(J3DUVec2,8);
void* clipperExtraAllocate(u32);
struct J3DUCallbackVtable { u8 prefix[0x34];be<u32> invoke; };
struct J3DUCallback { gptr<J3DUCallbackVtable> mVtable; };
struct J3DUCallbackOwner { u8 prefix[0x1E4];gptr<J3DUCallback> mCallback; };
struct J3DUCallbackArgument { u8 prefix[0xC];gptr<J3DUCallbackOwner> mOwner; };
WWHD_SIZE(J3DUCallbackVtable,0x38);WWHD_SIZE(J3DUCallback,4);
WWHD_OFFSET(J3DUCallbackOwner,mCallback,0x1E4);WWHD_SIZE(J3DUCallbackOwner,0x1E8);
WWHD_OFFSET(J3DUCallbackArgument,mOwner,0xC);WWHD_SIZE(J3DUCallbackArgument,0x10);
void clipperExtraRegisterStatic(void*);
