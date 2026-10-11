/* J3DModelData::getMaterialNodePointer(name) (HD inline): the name goes in a sead::SafeString with
 * this TU's vtable, the TU's assureTerminationImpl_ is called, then the material name table
 * (resource header +0x18, self-relative) is searched. */
#pragma once
#include "bindings.h"
#include "gc_api/tu.h"

inline J3DMaterial* J3DModelData::getMaterialNodePointer(const char* name) {
    gabi::Stack<SafeString> key;
    key->mStringTop = gabi::guest_addr(name);
    key->__vtbl = gc_tu.safeString;
    u32 hdr = mpResHeader;
    gabi::call_ptr(gc_tu.safeStringAssure, key.get());
    u32 off = gabi::load<u32>(hdr + 0x18);
    JUTNameTab* tab = gabi::at<JUTNameTab>(off ? hdr + 0x18 + off : 0);
    s32 idx = gabi::call<s32>(0x027DF9B0, tab, (u32)key->mStringTop);
    if (idx < 0) return nullptr;
    u32 n = mMaterialNum;
    u32 p = mMaterials;
    return gabi::at<J3DMaterial>((u32)idx < n ? p + (u32)idx * 0x39C : p);
}
