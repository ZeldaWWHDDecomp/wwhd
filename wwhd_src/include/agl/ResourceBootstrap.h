/* AGL#14 bootstrap settings, sized by its own constructor. */
#pragma once
#include "sead/FileDevice.h"
namespace agl {
struct BootstrapHeap_l;
struct BootstrapSettings_l {gptr<BootstrapHeap_l> heap;be<u32> options[11];sead::FixedSafeText<256> name;be<u8> enabled,secondary;u8 padding[2];};
BootstrapSettings_l* createBootstrapSettings(BootstrapSettings_l* settings);
void initializeBootstrapResources(BootstrapSettings_l* settings);
}
WWHD_SIZE(agl::BootstrapSettings_l,0x140);
WWHD_OFFSET(agl::BootstrapSettings_l,name,0x30);
WWHD_OFFSET(agl::BootstrapSettings_l,enabled,0x13C);
WWHD_OFFSET(agl::BootstrapSettings_l,heap,0);
WWHD_OFFSET(agl::BootstrapSettings_l,options,4);
