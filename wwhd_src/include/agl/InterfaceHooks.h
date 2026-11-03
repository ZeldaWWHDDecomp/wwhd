// Small WWHD interface hooks reconstructed within AGL#12.
#pragma once
#include "sead/FileDevice.h"
namespace agl {
struct ResourceDescriptor {
    be<u32> mHeader;
    be<u32> mNameHash;
    be<u32> mTypeHash;
};
struct ResourceObject_l {
    u8 _00[0x50];
    be<u32> mTypeHash;
    be<u32> mNameHash;
};
struct EmptyInterface {
    be<u32> mVtable;
};
sead::SafeText* emptyText();
}
WWHD_OFFSET(agl::ResourceObject_l,mTypeHash,0x50);
WWHD_OFFSET(agl::ResourceObject_l,mNameHash,0x54);
WWHD_SIZE(agl::ResourceDescriptor,0xC);
WWHD_SIZE(agl::EmptyInterface,4);
