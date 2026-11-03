/* Byte-text helpers reconstructed from AGL#17's capture preprocessing callers. */
#pragma once
#include "sead/Foundation.h"
#include "sead/FileDevice.h"
namespace sead { struct RegistryHeap; }
namespace agl { namespace capture_text {
u32 describeFileDialogRequest(void* unusedFirst, void* unusedSecond, sead::SafeText* mode,
                              sead::SafeText* optionalId, sead::SafeText* extension,
                              sead::SafeText* fileName);
/* count uses at most the original 1024-byte handled-name workspace. */
void replaceDefineValues(sead::SafeTextBuffer*, const gptr<be<u8>>* names,
                         const gptr<be<u8>>* values, s32 count, be<u8>* scratch, u32 scratchSize);
sead::SafeTextBuffer* expandQuotedIncludes(sead::SafeText*,
        const gptr<be<u8>>* names, const gptr<be<u8>>* contents, s32 count,
        be<u8>* used, sead::RegistryHeap* heap);
s32 findLineEnding(const be<u8>* text, be<u32>* endingWidth);
be<u8>* replaceTextRange(be<u8>* text, const be<u8>* replacement,
                      u32 begin, u32 end, be<u8>* suffixStorage);
} }
