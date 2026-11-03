/* Layout shader resource loading reconstructed from the WWHD binary. */
#include "nw/lyt/LayoutShaderLoader.h"
using namespace nw::lyt;
/* 0287D97C */
u32 LayoutShaderLoader::loadVertex(void* cache,gptr<nw::ut::VertexShader>* output,u32 index,const void* archive) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!archive || !output) return 0;
    if (index>=quadVertexCount(archive)) return 0;
    u32 headerSize=quadVertexHeaderSize(index,archive);
    u32 programSize=quadVertexProgramSize(index,archive);
    if (!headerSize || !programSize) return 0;
    auto* header=static_cast<nw::ut::VertexShader*>(allocateLayoutStorage(headerSize,64));
    void* program=allocateLayoutStorage(programSize,256);
    u32 result=quadLoadVertex(header,gabi::ea(program),index,archive);
    if (result) {
        u32 size=header->programSize;
        u32 data=header->program;
        quadInvalidate(0x40,data,size);
        *output=header;
    } else {
        if (header) releaseLayoutStorage(header);
        if (program) releaseLayoutStorage(program);
    }
    return result!=0;
}
/* 0287DA94 */
u32 LayoutShaderLoader::loadPixel(void* cache,gptr<nw::ut::PixelShader>* output,u32 index,const void* archive) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!archive || !output) return 0;
    if (index>=quadPixelCount(archive)) return 0;
    u32 headerSize=quadPixelHeaderSize(index,archive);
    u32 programSize=quadPixelProgramSize(index,archive);
    if (!headerSize || !programSize) return 0;
    auto* header=static_cast<nw::ut::PixelShader*>(allocateLayoutStorage(headerSize,64));
    void* program=allocateLayoutStorage(programSize,256);
    u32 result=quadLoadPixel(header,gabi::ea(program),index,archive);
    if (result) {
        u32 size=header->programSize;
        u32 data=header->program;
        quadInvalidate(0x40,data,size);
        *output=header;
    } else {
        if (header) releaseLayoutStorage(header);
        if (program) releaseLayoutStorage(program);
    }
    return result!=0;
}
/* WWHD glue
ext 028E8428 u32 quadVertexCount(const void*)
ext 028E8434 u32 quadPixelCount(const void*)
ext 028E8518 u32 quadVertexHeaderSize(u32,const void*)
ext 028E8538 u32 quadVertexProgramSize(u32,const void*)
ext 028E8528 u32 quadPixelHeaderSize(u32,const void*)
ext 028E8548 u32 quadPixelProgramSize(u32,const void*)
ext 028E8624 u32 quadLoadVertex(nw::ut::VertexShader*,u32,u32,const void*)
ext 028E87A8 u32 quadLoadPixel(nw::ut::PixelShader*,u32,u32,const void*)
ext C00064A8 void quadInvalidate(u32,u32,u32)
*/
