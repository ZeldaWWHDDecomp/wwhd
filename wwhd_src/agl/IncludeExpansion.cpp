#include "agl/CaptureTextTools.h"
#include "agl/UtilityText.h"
#include "sead/HeapRegistry.h"
void* aglIncludeAllocateObject(u32, sead::RegistryHeap*, s32);
void* aglIncludeAllocateArray(u32, sead::RegistryHeap*, s32);
void aglIncludeZeroBytes(void*, u32);
void aglIncludeReleaseScratch(void*);
void* aglIncludeMove(void*, const void*, u32, u32);
namespace agl { namespace capture_text {
static sead::SafeTextVtable sIncludeNameTable;
static s32 includeTextLength(const be<u8>* text) {
    s32 length = 0;
    if (text[0]) {
        do { if (++length > sead::kMaxTextLength) return 0; } while (text[length]);
    }
    return length;
}
static void destroyIncludeBuffer(sead::SafeTextBuffer* buffer) {
    u32 target = buffer->mVtable->mDestroy.mFunction;
    gabi::call_ptr<void>(target, buffer, 3u);
}
static sead::SafeTextBuffer* cloneIncludeText(sead::SafeText* source, sead::RegistryHeap* heap) {
    auto* object = static_cast<sead::SafeTextBuffer*>(aglIncludeAllocateObject(12, heap, 4));
    if (!object) return nullptr; // the caller retains the original unconditional subsequent access
    source->assureTermination();
    u32 bytes = static_cast<u32>(includeTextLength(source->mText)) + 1;
    auto* data = static_cast<be<u8>*>(aglIncludeAllocateArray(bytes, heap, 4));
    if (data) aglIncludeZeroBytes(data, bytes);
    source->assureTermination();
    s32 capacity = includeTextLength(source->mText) + 1;
    object->mText = data;
    object->mVtable = shadow_text::definitionBufferTable();
    object->mCapacity = capacity;
    /* A bounded length is0..0x40000, so the inline nonpositive-capacity branch is unreachable. */
    data[capacity - 1] = 0;
    object->mVtable = shadow_text::ownedIncludeBufferTable();
    be<u8>* destination = object->mText;
    source->assureTermination();
    s32 length = includeTextLength(source->mText);
    capacity = object->mCapacity;
    u32 copied = length < capacity ? static_cast<u32>(length) : static_cast<u32>(capacity) - 1;
    source->assureTermination();
    aglIncludeMove(destination, source->mText.get(), copied, 0);
    destination[copied] = 0;
    return object;
}
static sead::SafeTextBuffer* allocateIncludeWorkspace(u32 bytes, sead::RegistryHeap* heap) {
    auto* object = static_cast<sead::SafeTextBuffer*>(aglIncludeAllocateObject(12, heap, 4));
    if (!object) return nullptr; // no success is claimed for the caller's following null access
    auto* data = static_cast<be<u8>*>(aglIncludeAllocateArray(bytes, heap, 4));
    if (data) aglIncludeZeroBytes(data, bytes);
    object->mCapacity = static_cast<s32>(bytes);
    object->mText = data;
    object->mVtable = shadow_text::definitionBufferTable();
    if (static_cast<s32>(bytes) <= 0) {
        object->mCapacity = 0;
        object->mText = nullptr;
    } else {
        data[bytes - 1] = 0;
    }
    object->mVtable = shadow_text::ownedIncludeBufferTable();
    object->mText[0] = 0;
    return object;
}
static bool includeWhitespace(u8 value) {
    return value == ' ' || value == '\t' || value == '\r' || value == '\n';
}
/* 027C6060 */
sead::SafeTextBuffer* expandQuotedIncludes(sead::SafeText* source,
        const gptr<be<u8>>* names, const gptr<be<u8>>* contents, s32 count,
        be<u8>* used, sead::RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (used && count > 0)
        for (s32 index = 0; index < count; ++index) used[index] = 0;
    sead::SafeTextBuffer* result = cloneIncludeText(source, heap);
    result->assureTermination();
    u32 cachedLength = static_cast<u32>(includeTextLength(result->mText));
    result->assureTermination();
    be<u8>* scan = result->mText;
    while (*scan) {
        be<u8>* hash = scan;
        while (*hash && *hash != '#') ++hash;
        if (!*hash) return result;
        be<u8>* afterHash = hash + 1;
        be<u8>* keyword = afterHash;
        while (includeWhitespace(*keyword)) ++keyword;
        /* The original exhausted-directive path uses null, with no recovery guard. */
        if (!*keyword) keyword = nullptr;
        constexpr char includeKeyword[] = "include";
        bool include = true;
        for (u32 index = 0; index < 7; ++index)
            if (static_cast<u8>(keyword[index]) != static_cast<u8>(includeKeyword[index])) { include = false; break; }
        if (!include) { scan = keyword; continue; }
        be<u8>* openingQuote = keyword + 7;
        if (!*openingQuote) continue;
        while (*openingQuote && *openingQuote != '"') ++openingQuote;
        if (!*openingQuote) continue;
        be<u8>* nameStart = openingQuote + 1;
        if (!*nameStart) continue;
        be<u8>* closingQuote = nameStart;
        while (*closingQuote && *closingQuote != '"') ++closingQuote;
        if (!*closingQuote) continue;
        be<u8>* afterQuote = closingQuote + 1;
        gabi::FrameLocal<sead::FixedSafeText<1024>> includeName(0x24);
        includeName->mText = includeName->mStorage;
        includeName->mStorage[1023] = 0;
        includeName->mCapacity = 1024;
        includeName->mVtable = &sIncludeNameTable;
        gabi::FrameLocal<sead::SafeText> nameView(0x14);
        nameView->mVtable = shadow_text::definitionTextTable();
        nameView->mText = nameStart;
        includeName->mStorage[0] = 0;
        u32 copiedName = gabi::ea(closingQuote) - gabi::ea(nameStart);
        if (copiedName >= 1024) copiedName = 1023;
        nameView->assureTermination();
        aglIncludeMove(includeName->mStorage, nameView->mText.get(), copiedName, 0);
        includeName->mStorage[copiedName] = 0;
        if (count <= 0) return result;
        const be<u8>* replacement = nullptr;
        bool found = false;
        for (s32 index = 0; index < count; ++index) {
            gabi::FrameLocal<sead::SafeText> candidate(0xC);
            candidate->mVtable = shadow_text::definitionTextTable();
            candidate->mText = names[index];
            includeName->assureTermination();
            includeName->assureTermination();
            be<u8>* retained = includeName->mText;
            candidate->assureTermination();
            bool equal = retained == candidate->mText.get();
            if (!equal) {
                const be<u8>* left = includeName->mText;
                const be<u8>* right = candidate->mText;
                for (u32 byte = 0; byte <= sead::kMaxTextLength; ++byte) {
                    u8 a = left[byte], b = right[byte];
                    if (a != b) break;
                    if (!a) { equal = true; break; }
                }
            }
            if (!equal) continue;
            replacement = contents[index];
            if (used) used[index] = 1;
            found = true;
            break;
        }
        if (!found || !replacement) return result;
        gabi::FrameLocal<sead::SafeText> replacementView(0x1C);
        replacementView->mVtable = shadow_text::definitionTextTable();
        replacementView->mText = const_cast<be<u8>*>(replacement);
        shadow_text::assureShadowTermination(&*replacementView);
        u32 replacementLength = static_cast<u32>(includeTextLength(replacementView->mText));
        sead::SafeTextBuffer* workspace = allocateIncludeWorkspace(cachedLength + replacementLength + 1, heap);
        be<u8>* destination = workspace->mText;
        result->assureTermination();
        s32 length = includeTextLength(result->mText);
        s32 capacity = workspace->mCapacity;
        u32 copied = length < capacity ? static_cast<u32>(length) : static_cast<u32>(capacity) - 1;
        result->assureTermination();
        aglIncludeMove(destination, result->mText.get(), copied, 0);
        destination[copied] = 0;
        result->assureTermination();
        be<u8>* oldData = result->mText;
        auto* suffix = static_cast<be<u8>*>(aglIncludeAllocateArray(cachedLength + 1, heap, 4));
        workspace->assureTermination();
        replaceTextRange(workspace->mText, replacement,
                         gabi::ea(afterHash) - gabi::ea(oldData) - 1,
                         gabi::ea(afterQuote) - gabi::ea(oldData), suffix);
        aglIncludeReleaseScratch(suffix);
        destroyIncludeBuffer(result);
        result = cloneIncludeText(workspace, heap);
        destroyIncludeBuffer(workspace);
        result->assureTermination();
        scan = result->mText;
        result->assureTermination();
        cachedLength = static_cast<u32>(includeTextLength(result->mText));
    }
    return result;
}
} }
/* WWHD glue
data sIncludeNameTable 1016A4F0
ext 0273B050 void* aglIncludeAllocateObject(u32, sead::RegistryHeap*, s32)
ext 0273B0D4 void* aglIncludeAllocateArray(u32, sead::RegistryHeap*, s32)
ext 028F521C void aglIncludeZeroBytes(void*, u32)
ext 0273AF40 void aglIncludeReleaseScratch(void*)
ext C0009988 void* aglIncludeMove(void*, const void*, u32, u32)
*/
