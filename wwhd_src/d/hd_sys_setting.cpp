/** HD system setting and its save-option mirror; private decomp source.
 * No GameCube counterpart. Numeric console values are retained; their names are unconfirmed. */
#include "bindings.h"
#include "d/hd_sys_setting.h"
static hdSysSetting_c* sSetting;
static hdSysSetting_c* sSettingDisposer;
static hdSettingSaveInfo* sSaveInfo;

/* 025F9370 */
static hdSysSetting_c* constructSystemSetting(hdSysSetting_c* i_this) {
    if (i_this == NULL) {
        i_this = (hdSysSetting_c*)operator_new(sizeof(hdSysSetting_c));
        if (i_this == NULL) {
            return NULL;
        }
    }
    i_this->mState = hdSysSetting_c::UNAVAILABLE;
    i_this->mValue = 0;
    return i_this;
}

/* 025F93B4 */
static void createSystemSetting(void* heap) {
    if (sSetting != NULL) {
        return;
    }
    hdSysSetting_c* setting = (hdSysSetting_c*)hdSettingAllocate(sizeof(hdSysSetting_c), heap, 4);
    if (setting != NULL) {
        hdSettingConstructDisposer(setting, heap, 3);
        setting->mDisposerVtable = 0x100E0D88;
    }
    sSettingDisposer = setting;
    sSetting = setting != NULL ? constructSystemSetting(setting) : NULL;
}

/* 025F9448 */
static void updateSystemSetting(hdSysSetting_c* i_this) {
    if (hdSettingReadConsoleValue(&i_this->mValue) == -3) {
        i_this->mState = hdSysSetting_c::UNAVAILABLE;
        i_this->mValue = 0;
        return;
    }
    u32 value = i_this->mValue;
    i_this->mState = hdSysSetting_c::READ;
    if (value < hdSysSetting_c::DEFAULT ||
        (value > hdSysSetting_c::SECOND && value != hdSysSetting_c::FIFTH)) {
        value = hdSysSetting_c::DEFAULT;
        i_this->mValue = hdSysSetting_c::DEFAULT;
    }
    u8 language = 0;
    if (value == hdSysSetting_c::SECOND) {
        language = 2;
    } else if (value == hdSysSetting_c::FIFTH) {
        language = 3;
    }
    hdSettingOptions* options = hdSettingGetOptions(sSaveInfo->mOptions);
    options->mLanguage = language;
}

/* 025F9530 */
static void destroySystemSetting(hdSysSetting_c* i_this, u32 flags) {
    if (i_this == NULL) {
        return;
    }
    i_this->mDisposerVtable = 0x100E0D88;
    if (i_this == sSettingDisposer) {
        sSettingDisposer = NULL;
        sSetting = NULL;
    }
    hdSettingDestroyDisposer(i_this, 0);
    if (flags & 1) {
        operator_delete(i_this);
    }
}

/* 025F95B0 */
static void __sinit_hd_sys_setting_cpp() {
    sinit_header_statics_rodata(0x1048D79C, 0x101F4B88, 0x1048D7A8, 0x100E0D80);
}

/* WWHD glue (tools/verify/autoglue.py): HD setting globals and console APIs
 data sSetting 101F4BAC
 data sSettingDisposer 101F4BB0
 data sSaveInfo 101F84DC
 ext 0273B0D4 void* hdSettingAllocate(u32, void*, u32)
 ext 02752B0C void hdSettingConstructDisposer(hdSysSetting_c*, void*, u32)
 ext 02752BEC void hdSettingDestroyDisposer(hdSysSetting_c*, u32)
 ext 0286D620 s32 hdSettingReadConsoleValue(be<u32>*)
 ext 027200D0 hdSettingOptions* hdSettingGetOptions(void*)
*/
