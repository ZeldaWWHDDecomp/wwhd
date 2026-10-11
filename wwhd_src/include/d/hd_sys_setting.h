/** HD console-setting singleton. No GameCube counterpart; private decomp source. */
#pragma once
#include "wwhd.h"
struct hdSysSetting_c {
    u8 _padding00[0xC];
    be<u32> mDisposerVtable;
    be<u32> mState;
    be<u32> mValue;
    enum State { UNAVAILABLE = 1, READ = 2 };
    enum Value { DEFAULT = 1, SECOND = 2, FIFTH = 5 };
};
WWHD_OFFSET(hdSysSetting_c, mDisposerVtable, 0xC);
WWHD_OFFSET(hdSysSetting_c, mState, 0x10);
WWHD_OFFSET(hdSysSetting_c, mValue, 0x14);
WWHD_SIZE(hdSysSetting_c, 0x18);
struct hdSettingOptions {
    u8 _padding00[4];
    be<u8> mLanguage;
};
WWHD_OFFSET(hdSettingOptions, mLanguage, 4);
struct hdSettingSaveInfo {
    u8 _padding00[0x12C0];
    u8 mOptions[1];
};
WWHD_OFFSET(hdSettingSaveInfo, mOptions, 0x12C0);
void* hdSettingAllocate(u32, void*, u32);
void hdSettingConstructDisposer(hdSysSetting_c*, void*, u32);
void hdSettingDestroyDisposer(hdSysSetting_c*, u32);
s32 hdSettingReadConsoleValue(be<u32>*);
hdSettingOptions* hdSettingGetOptions(void*);
