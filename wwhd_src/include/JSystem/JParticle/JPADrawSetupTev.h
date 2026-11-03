// Cafe initialization state for the JPA1 draw setup helper.
#pragma once
#include "wwhd.h"
namespace JPADrawExtension {
struct JPADrawSetupTev_l {
    be<u8> mPrmColorEnabled,mEnvColorEnabled,mTextureEnabled;
    void initialize();
};
WWHD_SIZE(JPADrawSetupTev_l,3);
}
