/* Window dispatch and GPU program accessors, reconstructed from the WWHD binary. */
#include "nw/lyt/LayoutGpuResources.h"
using namespace nw::lyt;
static LayoutAllocatorSlot sLayoutAllocator;
/* 0287D2CC */
void WindowDrawDispatch::draw(LayoutDrawContext* context) {
    gabi::AutoFrame frame{gabi::current_function()};
    auto* methods=paneMethods<WindowDrawMethods>(this);
    gabi::call_ptr<void>(methods->prepare,this,context);
    switch (kind) {
    case Window_Around: drawAround(context); break;
    case Window_Horizontal: drawHorizontal(context); break;
    case Window_HorizontalNoContent: drawHorizontalNoContent(context); break;
    }
}
/* 0287D394 */
ShaderLocation* ShaderLocation::create(ShaderLocation* value) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!value) value=static_cast<ShaderLocation*>(gpuResourceAllocate(sizeof(ShaderLocation)));
    if (value) value->location=-1;
    return value;
}
/* 0287D3D0 */
ShaderPoint* ShaderPoint::create(ShaderPoint* point) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!point) point=static_cast<ShaderPoint*>(gpuResourceAllocate(sizeof(ShaderPoint)));
    if (point) { point->y=0.0f; point->x=0.0f; }
    return point;
}
/* 0287D464 */
u32 ShaderProgramConfig::calculateStorageSize() {
    storageSize=0xCC;
    u32 count=0;
    for (u32 i=0;i<30;++i) if (enabled[i]==1) ++count;
    programCount=count;
    u32 size=storageSize;
    if (count) {
        u32 attributeBytes=static_cast<u32>(attributeCount)*0x20;
        u32 locationBytes=static_cast<u32>(locationCount)*4;
        u32 displayBytes=displayListCapacity;
        for (u32 i=0;i<count;++i) {
            size=(size+3)&~3u;
            size=(size+0x47)&~3u;
            size=(size+attributeBytes+3)&~3u;
            size=(size+locationBytes+31)&~31u;
            size+=displayBytes;
        }
        storageSize=size;
    }
    return size;
}
/* 0287D514 */
void ShaderProgramCache::initialize(ShaderProgramConfig* input,void* memory) {
    gabi::AutoFrame frame{gabi::current_function()};
    auto* source=reinterpret_cast<const be<u32>*>(input);
    auto* target=reinterpret_cast<be<u32>*>(&config);
    for (u32 i=0;i<13;++i) target[i]=static_cast<u32>(source[i]);
    storage=memory;
    void* heap=gpuCreateFrameHeap(memory,input->storageSize,0);
    gabi::FrameLocal<ShaderAttribute> empty(8);
    for (u32 i=0;i<30;++i) {
        if (input->enabled[i]) {
            ShaderProgram* item=static_cast<ShaderProgram*>(gpuAllocateFrameHeap(heap,sizeof(ShaderProgram),4));
            if (item) {
                item->displayListCapacity=0;
                item->displayList=nullptr;
                item->displayListSize=0;
            }
            programs[i]=item;
            void* attributes=gpuAllocateFrameHeap(heap,static_cast<u32>(input->attributeCount)*32,4);
            programs[i]->attributes=static_cast<ShaderAttribute*>(attributes);
            gpuZeroMemory(empty.get(),32);
            u32 count=input->attributeCount;
            ShaderAttribute* attribute=programs[i]->attributes;
            for (u32 n=0;n<count;++n,attribute=layoutAtOffset<ShaderAttribute>(attribute,sizeof(ShaderAttribute))) {
                if (attribute) {
                    for (u32 word=0;word<8;++word) attribute->words[word]=static_cast<u32>(empty->words[word]);
                }
            }
            void* locations=gpuAllocateFrameHeap(heap,static_cast<u32>(input->locationCount)*4,4);
            programs[i]->locations=static_cast<be<s32>*>(locations);
            count=input->locationCount;
            be<s32>* location=programs[i]->locations;
            for (u32 n=0;n<count;++n) location[n]=-1;
            void* list=gpuAllocateFrameHeap(heap,input->displayListCapacity,32);
            ShaderProgram* program=programs[i];
            u32 capacity=input->displayListCapacity;
            program->displayList=list;
            program->displayListCapacity=capacity;
            gpuInvalidate(0x40,list,capacity);
        } else if (i>=15 && input->enabled[i-15]) {
            programs[i]=programs[i-15].get();
        } else {
            programs[i]=programs[0].get();
        }
    }
    if (heap) gpuDestroyFrameHeap(heap);
}
/* 0287D73C */
ShaderProgram* ShaderProgramCache::configureProgram(u32 index,void* vertex,void* pixel) {
    ShaderProgram* item=programs[index];
    item->pixelShader=pixel;
    item->vertexShader=vertex;
    item->usesVertexColor=index!=1 && index!=2;
    return item;
}
/* 0287D778 */
void ShaderProgramCache::recordProgram(ShaderProgram* item) {
    gabi::AutoFrame frame{gabi::current_function()};
    void* list=item->displayList;
    item->displayListSize=0;
    if (list) gpuBeginDisplayList(list,item->displayListCapacity,1);
    void* vertex=item->vertexShader;
    void* pixel=item->pixelShader;
    gpuSetFetchShader(&item->fetch);
    gpuSetVertexShader(vertex);
    gpuSetPixelShader(pixel);
    list=item->displayList;
    if (list) item->displayListSize=gpuEndDisplayList(list);
}
/* 0287D808 */
ShaderProgram* ShaderProgramCache::program(u32 index) { return programs[index]; }
/* 0287D818 */
void* ShaderProgramCache::vertexShader(ShaderProgram* item) { return item->vertexShader; }
/* 0287D820 */
void* ShaderProgramCache::pixelShader(ShaderProgram* item) { return item->pixelShader; }
/* 0287D828 */
ShaderFetch* ShaderProgramCache::fetchShader(ShaderProgram* item) { return &item->fetch; }
/* 0287D830 */
ShaderAttribute* ShaderProgramCache::attributes(ShaderProgram* item) { return item->attributes; }
/* 0287D838 */
ShaderAttribute* ShaderProgramCache::attribute(ShaderProgram* item,u32 index) { return item->attributes.get()+index; }
/* 0287D848 */
be<s32>* ShaderProgramCache::locations(ShaderProgram* item) { return item->locations; }
/* 0287D850 */
s32 ShaderProgramCache::location(ShaderProgram* item,u32 index) { return item->locations[index]; }
/* 0287D860 */
void ShaderProgramCache::bindProgram(ShaderProgram* item) {
    gabi::AutoFrame frame{gabi::current_function()};
    void* list=item->displayList;
    u32 size=0;
    if (list) size=item->displayListSize;
    if (!list || !size) {
        list=item->displayList;
        if (list) gpuBeginDisplayList(list,item->displayListCapacity,1);
        void* vertex=item->vertexShader;
        void* pixel=item->pixelShader;
        gpuSetFetchShader(&item->fetch);
        gpuSetVertexShader(vertex);
        gpuSetPixelShader(pixel);
        list=item->displayList;
        if (list) item->displayListSize=gpuEndDisplayList(list);
        size=item->displayListSize;
        if (!size) return;
        list=item->displayList;
    }
    gpuCallDisplayList(list,size);
}
/* 0287D91C */
u8 ShaderProgramCache::vertexColorEnabled(ShaderProgram* item) { return item->usesVertexColor; }
/* 0287D924 */
LayoutShaderSet* LayoutShaderSet::create(LayoutShaderSet* array) {
    if (!array) array=static_cast<LayoutShaderSet*>(gpuResourceAllocate(sizeof(LayoutShaderSet)));
    if (array) gpuResourceConstructArray(array,30,sizeof(ShaderProgramSlot),ShaderProgramSlot::create);
    return array;
}
/* 0287ED5C */
ShaderProgramSlot* ShaderProgramSlot::create(ShaderProgramSlot* state) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!state) state=static_cast<ShaderProgramSlot*>(gpuResourceAllocate(sizeof(ShaderProgramSlot)));
    if (state) { state->vertexShader=nullptr; state->fetchProgram=nullptr; state->pixelShader=nullptr; }
    return state;
}
/* 0287F014 */
bool nw::lyt::containsLayoutPoint(const TextBounds* bounds,const PaneSize* point) {
    f32 x=point->width;
    if (static_cast<f32>(bounds->left)>x) return false;
    if (x>static_cast<f32>(bounds->right)) return false;
    f32 y=point->height;
    if (static_cast<f32>(bounds->bottom)>y) return false;
    if (y>static_cast<f32>(bounds->top)) return false;
    return true;
}
/* 0287F1EC */
u32 nw::lyt::decodeTextureFormat(be<u32>* output,u32 format,u32 alpha) {
    u32 value;
    switch (format) {
    case 1: value=alpha; break;
    case 2: value=2; break;
    case 7: value=3; break;
    case 8: value=5; break;
    case 10: value=7; break;
    case 11: value=8; break;
    case 25: value=24; break;
    case 26: value=9; break;
    case 0x41A: value=20; break;
    case 0x31: value=12; break;
    case 0x431: value=21; break;
    case 0x32: value=13; break;
    case 0x432: value=22; break;
    case 0x33: value=14; break;
    case 0x433: value=23; break;
    case 0x34: value=alpha?16:15; break;
    case 0x35: value=17; break;
    default: return 0;
    }
    *output=value;
    return 1;
}
/* 0287F3D0 */
void nw::lyt::setLayoutAllocator(LayoutAllocator* allocator) { sLayoutAllocator.value=allocator; }
/* 0287F3DC */
void* nw::lyt::allocateLayoutStorage(u32 size,s32 alignment) {
    LayoutAllocator* allocator=sLayoutAllocator.value;
    return gabi::tail_ptr<void*>(allocator->methods->allocate,allocator,size,alignment);
}
/* 0287F3FC */
void nw::lyt::releaseLayoutStorage(void* storage) {
    LayoutAllocator* allocator=sLayoutAllocator.value;
    gabi::tail_ptr<void>(allocator->methods->release,allocator,storage);
}
/* WWHD glue
data sLayoutAllocator 101FC8B4
ext C00096E0 void* gpuCreateFrameHeap(void*,u32,u32)
ext C00096A0 void* gpuAllocateFrameHeap(void*,u32,s32)
ext C0009708 void gpuDestroyFrameHeap(void*)
ext 028F521C void gpuZeroMemory(void*,u32)
ext C00064A8 void gpuInvalidate(u32,void*,u32)
ext 0273AD10 void* gpuResourceAllocate(u32)
ext 028EFFD0 void gpuResourceConstructArray(void*,u32,u32,nw::lyt::ShaderSlotConstructor)
ext C0006090 void gpuBeginDisplayList(void*,u32,u32)
ext C0006190 u32 gpuEndDisplayList(void*)
ext C00060E0 void gpuCallDisplayList(void*,u32)
ext C0006860 void gpuSetFetchShader(nw::lyt::ShaderFetch*)
ext C0006A28 void gpuSetVertexShader(void*)
ext C00068F0 void gpuSetPixelShader(void*)
ext 0287B180 void nw::lyt::WindowPane::drawAround(nw::lyt::LayoutDrawContext*)
ext 0287C358 void nw::lyt::WindowPane::drawHorizontal(nw::lyt::LayoutDrawContext*)
ext 0287D1C4 void nw::lyt::WindowPane::drawHorizontalNoContent(nw::lyt::LayoutDrawContext*)
*/
