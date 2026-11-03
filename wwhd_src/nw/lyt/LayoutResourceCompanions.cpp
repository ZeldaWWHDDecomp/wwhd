/* Layout coordinate storage and initialization helpers, reconstructed from the WWHD binary. */
#include "nw/lyt/LayoutResourceCompanions.h"
using namespace nw::lyt;
void* resourceHelperAllocate(u32 size);
// Runtime-type nodes of the window pane and of the material, each behind its own guard; the window's parent is the
// shared pane base node (sPaneType, nw/lyt/ResourceProvider.h), initialised inline as every pane kind does.
static RuntimeTypeGuard sWindowTypeGuard;
static RuntimeTypeGuard sMaterialTypeGuard;
static RuntimeTypeNode sWindowType;
static RuntimeTypeNode sMaterialType;
/* 0287D414 */
RuntimeTypeNode* nw::lyt::windowRuntimeType() {
    if (!static_cast<u32>(sWindowTypeGuard.ready)) {
        u32 baseReady=sPaneTypeGuard.ready;
        sWindowTypeGuard.ready=1;
        if (!baseReady) {
            sPaneTypeGuard.ready=1;
            sPaneType.parent=nullptr;
        }
        sWindowType.parent=&sPaneType;
    }
    return &sWindowType;
}
/* 02880BF8 */
RuntimeTypeNode* nw::lyt::materialRuntimeType() {
    if (!static_cast<u32>(sMaterialTypeGuard.ready)) {
        sMaterialTypeGuard.ready=1;
        sMaterialType.parent=nullptr;
    }
    return &sMaterialType;
}
/* 02880CE8 */
TextureCoordinateArray* TextureCoordinateArray::create(TextureCoordinateArray* array) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!array) array=static_cast<TextureCoordinateArray*>(resourceHelperAllocate(sizeof(TextureCoordinateArray)));
    if (array) {
        array->capacity=0;
        array->coordinates=nullptr;
        array->count=0;
    }
    return array;
}
/* 02880D2C */
void TextureCoordinateArray::clear() {
    gabi::AutoFrame frame{gabi::current_function()};
    LayoutTextureCoordinates* data=coordinates;
    if (data) {
        releaseLayoutStorage(data);
        coordinates=nullptr;
        count=0;
        capacity=0;
    }
}
/* 02880D84 */
void TextureCoordinateArray::reserve(u32 requested) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (static_cast<u32>(capacity)<requested) {
        clear();
        void* memory=allocateLayoutStorage(requested*32,4);
        coordinates=static_cast<LayoutTextureCoordinates*>(memory);
        if (memory) capacity=requested;
    }
}
/* 02880E24 */
void TextureCoordinateArray::assign(const LayoutTextureCoordinates* source,u32 requested) {
    u32 used=count;
    if (requested>used) used=requested;
    count=used;
    if (static_cast<s32>(requested)<=0) return;
    for (u32 set=0;set<requested;++set) {
        for (u32 vertex=0;vertex<4;++vertex) {
            LayoutTextureCoordinates* destination=coordinates.get()+set;
            const be<u32>* input=reinterpret_cast<const be<u32>*>(source->values[vertex]);
            u32 horizontal=input[0];
            u32 vertical=input[1];
            be<u32>* output=reinterpret_cast<be<u32>*>(destination->values[vertex]);
            output[0]=horizontal;
            output[1]=vertical;
        }
        ++source;
    }
}
/* 0288112C */
void MaterialGpu::clearCounts(be<u32>* flags) { *flags=static_cast<u32>(*flags)&0x7FFF; }
/* 0288113C */
MaterialGpu* MaterialGpu::initialize() {
    gabi::AutoFrame frame{gabi::current_function()};
    colors[0].value=0;
    colors[1].value=0xFFFFFFFF;
    clearCounts(&layoutFlags);
    clearCounts(&featureFlags);
    ownershipFlags=0;
    storage=nullptr;
    setProgramIndex(0);
    return this;
}
/* 02883C28 */
ShaderLocation* nw::lyt::createPixelUniformLocation(ShaderLocation* location) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!location) location=static_cast<ShaderLocation*>(resourceHelperAllocate(sizeof(ShaderLocation)));
    if (location) location->location=-1;
    return location;
}
/* WWHD glue
data sWindowTypeGuard 101FD8C0
data sWindowType 101FD934
data sMaterialTypeGuard 101FD894
data sMaterialType 101FD904
ext 0273AD10 void* resourceHelperAllocate(u32)
*/
