// Statics of the heap-registry translation unit shared with the units that use the registry (SEAD#13, SEAD#14):
// the registry manager pointer and the registry-heap base runtime type (its guard and node, initialised inline by
// every heap kind's matchesType in each translation unit), declared in sead/HeapRegistry.h.
#include "sead/HeapRegistry.h"
static sead::RegistryManagerState* sRegistryManager;
static u32 sRegistryHeapTypeGuard;
static sead::RuntimeType sRegistryHeapType;
/* WWHD glue
data sRegistryManager 101F8B4C
data sRegistryHeapTypeGuard 101FD2C4
data sRegistryHeapType 101FDF9C
*/
