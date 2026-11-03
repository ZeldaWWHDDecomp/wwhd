/* Layout renderer initialization and shared quad buffers from the WWHD binary. */
#include "nw/lyt/LayoutRenderStorage.h"
using namespace nw::lyt;
/* 0287EB18 */
LayoutRenderStorage* LayoutRenderStorage::create(LayoutRenderStorage* device) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!device) device=static_cast<LayoutRenderStorage*>(renderStorageAllocate(sizeof(LayoutRenderStorage)));
    if (device) {
        device->pixelShaderCount=0;
        device->vertexShaderCount=0;
        initializeLayoutShaderBundle(&device->bundle);
        device->programCache.config.storageSize=0;
        device->quadCoordinates=nullptr;
        device->quadIndices=nullptr;
        device->programCache.storage=nullptr;
        device->programCache.config.locationCount=1;
        device->programCache.config.displayListCapacity=512;
        device->programCache.config.attributeCount=1;
        device->programCache.config.programCount=1;
        for (u32 i=0;i<30;++i) device->programCache.config.enabled[i]=0;
        device->programCache.config.enabled[0]=1;
        for (u32 i=0;i<30;++i) device->programCache.programs[i]=nullptr;
        device->initialized=0;
    }
    return device;
}
/* 0287EBD4 */
void LayoutRenderStorage::createQuadBuffers() {
    gabi::AutoFrame frame{gabi::current_function()};
    quadCoordinates=static_cast<LayoutTextureCoordinates*>(allocateLayoutStorage(32,64));
    quadCoordinates->values[0][0]=0.0f;
    quadCoordinates->values[0][1]=0.0f;
    quadCoordinates->values[1][0]=1.0f;
    quadCoordinates->values[1][1]=0.0f;
    quadCoordinates->values[2][0]=0.0f;
    quadCoordinates->values[2][1]=1.0f;
    quadCoordinates->values[3][0]=1.0f;
    quadCoordinates->values[3][1]=1.0f;
    renderStorageInvalidate(0x40,quadCoordinates,32);
    quadIndices=static_cast<be<u16>*>(allocateLayoutStorage(8,32));
    quadIndices[0]=1;
    quadIndices[1]=0;
    quadIndices[2]=2;
    quadIndices[3]=3;
    renderStorageInvalidate(0x40,quadIndices,8);
}
/* 0287ECB4 */
void LayoutRenderStorage::initialize(void* archive,u32 mode) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 size=layoutShaderBundleSize(archive,mode);
    void* memory=allocateLayoutStorage(size,64);
    loadLayoutShaderBundle(&bundle,memory,archive,mode);
    createQuadBuffers();
    initialized=1;
}
/* 0287ED2C */
void LayoutRenderStorage::drawQuad() {
    gabi::AutoFrame frame{gabi::current_function()}; renderStorageDrawIndexed(0x13,4,4,quadIndices,0,1); }
/* 0287ED48 */
void LayoutRenderStorage::bindQuadBuffer() {
    gabi::AutoFrame frame{gabi::current_function()}; renderStorageAttributeBuffer(0,32,8,quadCoordinates); }
/* WWHD glue
ext 0273AD10 void* renderStorageAllocate(u32)
ext 028F6DF8 void initializeLayoutShaderBundle(nw::lyt::LayoutShaderBundle*)
ext 028F6CD8 u32 layoutShaderBundleSize(void*,u32)
ext 028F78F4 void loadLayoutShaderBundle(nw::lyt::LayoutShaderBundle*,void*,void*,u32)
ext C00064A8 void renderStorageInvalidate(u32,void*,u32)
ext C0006178 void renderStorageDrawIndexed(u32,u32,u32,const void*,u32,u32)
ext C0006798 void renderStorageAttributeBuffer(u32,u32,u32,const void*)
*/
