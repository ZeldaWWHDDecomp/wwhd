#ifndef WWHD_JASINSTRAND_H
#define WWHD_JASINSTRAND_H
#include "JSystem/JAudio/JASInstEffect.h"
namespace JASystem {
class TInstRand : public TInstEffect {
public:
    f32 getY(s32 key, s32 velocity) const;
    be<f32> mBase;
    be<f32> mWidth;
};
}
WWHD_OFFSET(JASystem::TInstRand, mBase, 8);
WWHD_OFFSET(JASystem::TInstRand, mWidth, 0xC);
WWHD_SIZE(JASystem::TInstRand, 0x10);
#endif
