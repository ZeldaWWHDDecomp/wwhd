/* Program dirty propagation from the assigned WWHD companion-root traversal. */
#include "agl/ShaderProgram.h"
#include "agl/ShaderArchiveRelocations.h"
using namespace agl;
/* 027BBCA0 */
void ShaderProgram::markResourceDirty() {
    const auto* shared = static_cast<const ShaderProgramSharedData*>(binary.get());
    u32 address = gabi::ea(shared);
    if (address + 8) {
        ShaderProgram* root = shared->variation.root;
        root->flags = static_cast<u32>(root->flags) | 2;
        shared = static_cast<const ShaderProgramSharedData*>(binary.get());
        u32 count = shared->variation.programCount;
        u32 cursor = gabi::ea(shared->variation.programs.get()), end = cursor + count * 0x84;
        while (cursor != end) {
            ShaderProgram* program = gabi::at<ShaderProgram>(cursor);
            program->flags = static_cast<u32>(program->flags) | 2;
            cursor += 0x84;
        }
    } else
        flags = static_cast<u32>(flags) | 2;
}
/* WWHD glue */
