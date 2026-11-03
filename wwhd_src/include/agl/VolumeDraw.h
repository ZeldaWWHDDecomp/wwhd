// Caller views reconstructed from AGL#12's volume draw instructions.
#pragma once
#include "sead/RenderMath.h"
#include "sead/ViewTransform.h"
namespace agl {
struct VolumeQuadSettings_l {
    u8 _00[0x10C];
    be<u32> mFlags;
    u8 _110[8];
    sead::RenderVector2 mFirst;
    sead::RenderVector2 mSecond;
};
}
WWHD_OFFSET(agl::VolumeQuadSettings_l,mFirst,0x118);
WWHD_OFFSET(agl::VolumeQuadSettings_l,mSecond,0x120);
