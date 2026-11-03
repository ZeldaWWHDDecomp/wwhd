#pragma once
#include "sead/RuntimeType.h"
namespace sead {
bool isViewType(const void*,const RuntimeType*);
bool isLookAtViewType(const void*,const RuntimeType*);
bool isDerivedLookAtViewType(const void*,const RuntimeType*);
bool isOrthographicViewType(const void*,const RuntimeType*);
bool isProjectionType(const void*,const RuntimeType*);
bool isFrustumProjectionType(const void*,const RuntimeType*);
bool isStoredProjectionType(const void*,const RuntimeType*);
void destroyViewResource(void*,u32);
}
// Look-at view and stored-projection runtime types, owned by sead/ViewTypeIdentity.cpp (guards 101FD6B4/101FD6A4,
// nodes 101FDF70/101FDF80); shared with units that initialise them inline (exported 2026-10-24).
extern be<u32>& sLookAtGuard;
extern sead::RuntimeType& sLookAtTag;
extern be<u32>& sStoredProjectionGuard;
extern sead::RuntimeType& sStoredProjectionTag;
