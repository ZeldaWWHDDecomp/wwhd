#pragma once
#include "JSystem/JStudio/JST_adaptors.h"
namespace JStudio_JStage {
struct TAdaptor_object_ {
    static void adaptor_data_(jst_adaptors::StageObject*,const void*,u32,const void*,u32);
    static void adaptor_ENABLE_(jst_adaptors::StageObject*,u32,const be<u32>*,u32);
};
struct ObjectVtable { u8 prefix[0x24]; gfn getFlags; u8 gap[4]; gfn setFlags; u8 gapData[0xC]; gfn setData; };
WWHD_OFFSET(ObjectVtable,setData,0x3C);
}
