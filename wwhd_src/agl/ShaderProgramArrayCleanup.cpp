/* Program resource-array release and fixed-end variant traversal from assigned WWHD code. */
#include "agl/ShaderProgram.h"
#include "sead/HeapRegistry.h"
using namespace agl;
struct ProgramCleanupList_l {
    u8 prefix[0xC];
    be<u32> count;
    gptr<ShaderProgram> programs;
};
struct ProgramResourceArray_l {
    be<u32> count;
    gptr<void> records;
};
static void releaseProgramArray(ShaderProgram*, be<u32> ShaderProgram::*);
/* 027BA0D0 */
void ShaderProgram::releaseAttributes() {
    gabi::AutoFrame frame{gabi::current_function()};
    releaseProgramArray(this, &ShaderProgram::attributeCount);
}
static void releaseOneProgramArray(ProgramResourceArray_l* array) {
    void* records = array->records;
    if (records) {
        sead::RegistryHeap* heap = findRegistryHeap(sRegistryManager.get(), records);
        const sead::RegistryHeapDispatch* methods = heap->dispatch();
        u32 release = methods->mRelease.mFunction.get();
        records = array->records;
        gabi::call_ptr<void>(release, heap, records);
        array->count = 0;
        array->records = nullptr;
    }
}
static void releaseProgramArray(ShaderProgram* program, be<u32> ShaderProgram::* countMember) {
    releaseOneProgramArray(gabi::at<ProgramResourceArray_l>(gabi::ea(&(program->*countMember))));
    const ProgramCleanupList_l* list = gabi::at<const ProgramCleanupList_l>(gabi::ea(program->binary.get()));
    u32 count = list->count;
    u32 cursor = gabi::ea(list->programs.get()), end = cursor + count * 0x84;
    while (cursor != end) {
        ShaderProgram* variant = gabi::at<ShaderProgram>(cursor);
        releaseOneProgramArray(gabi::at<ProgramResourceArray_l>(gabi::ea(&(variant->*countMember))));
        cursor += 0x84;
    }
}
/* 027BA1A0 */
void ShaderProgram::releaseUniforms() {
    gabi::AutoFrame frame{gabi::current_function()};
    releaseProgramArray(this, &ShaderProgram::uniformCount);
}
/* 027BA270 */
void ShaderProgram::releaseSamplers() {
    gabi::AutoFrame frame{gabi::current_function()};
    releaseProgramArray(this, &ShaderProgram::samplerCount);
}
/* 027BA340 */
void ShaderProgram::releaseUniformBlocks() {
    gabi::AutoFrame frame{gabi::current_function()};
    releaseProgramArray(this, &ShaderProgram::uniformBlockCount);
}
/* WWHD glue
*/
