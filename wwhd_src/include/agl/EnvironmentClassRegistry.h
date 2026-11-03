// Registry record and ID views from the game's class registration routine.
#pragma once
#include "agl/EnvironmentObject.h"
#include "sead/ElementAt.h"   // sead::elementPtr / nextElement: 32-bit guest pointer walks
namespace agl { namespace env {
struct EnvironmentClassEntry {
    gptr<be<u8>> mName;
    gptr<be<u8>> mDescription;
    be<u32> mFactory;
    be<s32> mCategory;
    be<u32> mId;
};
struct EnvironmentClassRegistry {
    static EnvironmentClassDescriptor* registerClass(const sead::SafeText* name,const sead::SafeText* description,u32 factory,s32 category);
};
}}
WWHD_SIZE(agl::env::EnvironmentClassEntry,20);
WWHD_OFFSET(agl::env::EnvironmentClassEntry,mCategory,12);
WWHD_OFFSET(agl::env::EnvironmentClassEntry,mId,16);
