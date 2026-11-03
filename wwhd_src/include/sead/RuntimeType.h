// The game's runtime identity is a four-byte object containing its own table pointer.
#pragma once
#include "wwhd.h"
namespace sead {
struct RuntimeType { gptr<void> mTable; };
}
WWHD_OFFSET(sead::RuntimeType,mTable,0);
WWHD_SIZE(sead::RuntimeType,4);

// Lazily initialised runtime-type guards and nodes shared by several translation units (each initialiser stores
// its own type table), owned by sead/RuntimeStatics.cpp.
extern be<u32>& sMethodTreeMgrTypeGuard;
extern sead::RuntimeType& sMethodTreeMgrType;
extern be<u32>& sStreamBaseGuard;
extern sead::RuntimeType& sStreamBaseType;
extern be<u32>& sStreamGuard;
extern sead::RuntimeType& sStreamType;
// agl curve-collection and common base runtime types (guards 101FD8E0/101FD8D0, nodes 101FDFFC/101FE000), owned by
// sead/RuntimeStatics.cpp; initialised inline by AGL#13/#14/#18/#19 (each stores its own type table).
extern be<u32>& sCurveBaseGuard;
extern sead::RuntimeType& sCurveBaseType;
extern be<u32>& sAglCommonBaseGuard;
extern sead::RuntimeType& sAglCommonBaseType;
// Read-only type tables stored into sStreamBaseType / sStreamType (and the input, output and stored-projection
// tags) by every first-use initialiser, owned by sead/RuntimeStatics.cpp.
extern be<u32>& sStreamBaseTable;
extern be<u32>& sStreamTable;
// The object-base runtime type (guard 101FD7C0, node 101FDCE8), owned by sead/RuntimeType.cpp; AGL units that
// initialise it inline with their own table share it (exported 2026-10-24).
extern be<u32>& sObjectBaseGuard;
extern sead::RuntimeType& sObjectBaseType;
// The audio system's runtime type (guard 101FD648, node 101FD958) is sAudioSystemTypeGuard / sAudioSystemType,
// owned by sead/AudioStatics.cpp and declared in sead/AudioSystem.h.
