// Shared sead statics that several translation units read and write inline: the current graphics device
// (101F8B18) and the lazily initialised runtime-type guards/nodes of the method-tree manager and the stream
// classes, the agl curve/common base types shared by AGL#13/#14/#18/#19 (2026-10-25), and the two read-only type tables those stream types point to (101451E0 stream base, 101451F0 stream).
// Declared in sead/GraphicsDevice.h and sead/RuntimeType.h.
#include "sead/GraphicsDevice.h"
#include "sead/RuntimeType.h"
static sead::GraphicsDevice* sCurrentGraphicsDevice;
static u32 sMethodTreeMgrTypeGuard;
static sead::RuntimeType sMethodTreeMgrType;
static u32 sStreamBaseGuard;
static sead::RuntimeType sStreamBaseType;
static u32 sStreamGuard;
static sead::RuntimeType sStreamType;
static u32 sStreamBaseTable;
static u32 sStreamTable;
static u32 sCurveBaseGuard;
static sead::RuntimeType sCurveBaseType;
static u32 sAglCommonBaseGuard;
static sead::RuntimeType sAglCommonBaseType;
/* WWHD glue
data sCurrentGraphicsDevice 101F8B18
data sMethodTreeMgrTypeGuard 101FD718
data sMethodTreeMgrType 101FD990
data sStreamBaseGuard 101FD69C
data sStreamBaseType 101FDCD8
data sStreamGuard 101FD6A0
data sStreamType 101FDCDC
data sStreamBaseTable 101451E0
data sStreamTable 101451F0
data sCurveBaseGuard 101FD8E0
data sCurveBaseType 101FDFFC
data sAglCommonBaseGuard 101FD8D0
data sAglCommonBaseType 101FE000
*/
