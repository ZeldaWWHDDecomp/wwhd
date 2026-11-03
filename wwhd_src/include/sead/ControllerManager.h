// WWHD manager initialization data, observed in this build only.
#pragma once
#include "sead/DeviceBuffers.h"
#include "sead/RuntimeType.h"
struct ControllerInitCallback { gptr<be<u32>> mTable; };
struct ControllerInit {
    gptr<be<u32>> mTable;
    be<s32> mCapacity;
    gptr<ControllerInitCallback> mCallback;
};
WWHD_OFFSET(ControllerInit,mTable,0);
WWHD_OFFSET(ControllerInit,mCapacity,4);
WWHD_OFFSET(ControllerInit,mCallback,8);
WWHD_SIZE(ControllerInit,12);
