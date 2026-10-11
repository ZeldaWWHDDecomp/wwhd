/** JAIZelSound: WWHD sound instance.
 * HD: the base loses the leading GameCube vtable; its vtable is at the end.
 * GameCube member names are retained at their WWHD offsets. */
#pragma once
#include "bindings.h"

struct JAISound {
    struct PositionInfo_t {
        be<f32> field_0x0;
        u8 padding_04[4];
        be<f32> field_0x8;
        u8 padding_0c[12];
        be<f32> field_0x18;
    };
    u8 padding_00[4];
    be<u8> field_0x8;
    u8 padding_05[15];
    be<u32> field_0x18;
    u8 padding_18[4];
    gptr<PositionInfo_t> mPositionInfo;
    u8 padding_20[4];
    gptr<cXyz> field_0x28;
    u8 padding_28[28];
    struct Vtable {
        u8 padding_00[0x24];
        be<u32> setDistanceVolumeCommon;
        u8 padding_28[4];
        be<u32> setDistancePanCommon;
    };
    gptr<Vtable> __vtbl;
    u32 getSwBit();
    u32 getID();
    // HD: full register arguments preserve the unextended u8 values at call sites.
    void setSeInterVolume(u32, f32, u32, u32);
    void setSeInterPan(u32, f32, u32, u32);
    void setSeInterDolby(u32, f32, u32, u32);
};
WWHD_SIZE(JAISound::PositionInfo_t, 0x1c);
WWHD_OFFSET(JAISound, field_0x8, 4);
WWHD_OFFSET(JAISound, field_0x18, 0x14);
WWHD_OFFSET(JAISound, mPositionInfo, 0x1c);
WWHD_OFFSET(JAISound, field_0x28, 0x24);
WWHD_OFFSET(JAISound, __vtbl, 0x44);
WWHD_SIZE(JAISound, 0x48);

struct JAIZelSound : JAISound {
    // HD: keep full incoming registers rather than normalizing the GameCube u8 parameters.
    f32 setDistanceVolumeCommon(f32, u32);
    f32 distanceVolumeHD(f32, f32, f32);
    void setSeDistanceVolume(u32);
    void setSeDistancePan(u32);
    void setSeDistanceDolby(u32);
};
enum JAISoundType { SOUNDPARAM_Distance = 4 };
// HD: narrow view of the scene discriminator used only by the sea-volume curve.
// The semantic name describes this use; no full JAIZelBasic layout is inferred.
struct JAIZelBasicSoundView {
    u8 padding[0x294];
    be<s32> mScene;
};
