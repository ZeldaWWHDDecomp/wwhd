// HD graphics singleton query adapter; reconstructed from local game instructions.
#include "bindings.h"
#include "JSystem/NativeGraphicsQuery.h"
#include "sead/GraphicsDevice.h"

/* 0284B990 */
static u32 queryNativeGraphicsDevice() {
    auto* device = reinterpret_cast<NativeGraphicsQueryDevice*>(sCurrentGraphicsDevice.get());
    return gabi::tail_ptr<u32>(device->mTable->mQuery,device);
}

/* WWHD glue
*/
