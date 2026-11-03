// Statics of the file-device translation unit shared by its companion sources (sead/FileDevice.cpp,
// sead/CafeFileDevices.cpp): the device manager singleton and the empty text, declared in sead/FileDevice.h.
// Also the file-device base vtables, which the base constructor (inlined into the file-device and
// file-route translation units) and the base destructor store.
#include "sead/FileDevice.h"
using namespace sead;
static FileDeviceManager* sFileDeviceManager;
static SafeText sFileDeviceEmptyText;
static FileDeviceVtable sFileDeviceVtable;
static DisposerVtable sFileDeviceDisposerVtable;
/* WWHD glue
data sFileDeviceManager 101F8B08
data sFileDeviceEmptyText 104A0CD8 nocheck
data sFileDeviceVtable 10143B78
data sFileDeviceDisposerVtable 10143C58
*/
