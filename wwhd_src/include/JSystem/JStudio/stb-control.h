// STB ownership methods over the canonical 0x58 control prefix.
#pragma once
#include "JSystem/JStudio/stb-companion.h"
namespace JStudio::stb {
struct TObject_control : TObject {};
struct FactoryVTable { u8 prefix[0x14]; gfn create; u8 gap[4]; gfn destroy; };
struct TControl : jst_timeline::stb::TControlPrefix {
    void appendObject(TObject*);
    void removeObject(TObject*);
    void destroyObject(TObject*);
    void destroyObject_all();
    TObject* getObject(const void*, u32);
    void reset();
};
WWHD_SIZE(TObject_control, 0x34);
WWHD_SIZE(TControl, 0x58);
WWHD_OFFSET(TControl, pFactory, 8);
WWHD_OFFSET(TControl, mObjectContainer, 0x10);
WWHD_OFFSET(TControl, mStatus, 0x18);
WWHD_OFFSET(TControl, mObject_control, 0x1C);
WWHD_OFFSET(TControl, _54, 0x50);
WWHD_OFFSET(TControl, __vtbl, 0x54);
WWHD_OFFSET(FactoryVTable, destroy, 0x1C);
}
