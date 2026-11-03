#include "agl/CaptureTextTools.h"
#include "agl/UtilityText.h"
s32 aglFormatDefinition(sead::format::ByteTextBuffer*, const char*, const be<u8>*, const be<u8>*);
void* aglDefinitionMove(void*, const void*, u32, u32);
namespace agl { namespace capture_text {
static bool definitionWhitespace(u8 value) {
    return value == ' ' || value == '\t' || value == '\r' || value == '\n';
}
static s32 definitionTextLength(const be<u8>* text) {
    s32 length = 0;
    if (text[0]) {
        do {
            ++length;
            if (length > sead::kMaxTextLength) return 0;
        } while (text[length]);
    }
    return length;
}
/* 027C5B20 */
void replaceDefineValues(sead::SafeTextBuffer* text, const gptr<be<u8>>* names,
        const gptr<be<u8>>* values, s32 count, be<u8>* scratch, u32 scratchSize) {
    gabi::AutoFrame frame{gabi::current_function()};
    struct HandledNames_l { be<u8> mFlags[1024]; };
    WWHD_SIZE(HandledNames_l, 0x400);
    gabi::FrameLocal<HandledNames_l> handled(0x38);
    for (s32 index = 0; index < count; ++index) handled->mFlags[index] = 0;
    text->assureTermination();
    be<u8>* source = text->mText;
    be<u8>* output = scratch;
    u32 scratchEnd = gabi::ea(scratch) + scratchSize;
    u32 matchedCount = 0;
    bool scanning = true;
    gabi::FrameLocal<be<u32>> endingWidth(0x20);
    while (scanning) {
        s32 ending = findLineEnding(source, endingWidth);
        if (ending == -1) break;
        bool replaced = false;
        if (source[0] == '#') {
            const be<u8>* directive = source + 1;
            while (definitionWhitespace(*directive)) ++directive;
            bool define = true;
            constexpr char keyword[] = "define";
            for (u32 letter = 0; letter < 6; ++letter) {
                if (static_cast<u8>(*directive++) != static_cast<u8>(keyword[letter])) { define = false; break; }
            }
            if (define) {
                u8 separator = *directive++;
                define = separator == ' ' || separator == '\t';
            }
            if (define) {
                while (definitionWhitespace(*directive)) ++directive;
                for (s32 index = 0; index < count; ++index) {
                    if (handled->mFlags[index]) continue;
                    const be<u8>* name = names[index];
                    u32 length = 0;
                    while (name[length] && directive[length] == name[length]) ++length;
                    if (name[length]) continue;
                    u8 separator = directive[length];
                    if (separator != ' ' && separator != '\t') continue;
                    s32 remaining = static_cast<s32>(scratchEnd - gabi::ea(output));
                    gabi::FrameLocal<sead::format::ByteTextBuffer> formatted(0x24);
                    formatted->mVtable = shadow_text::definitionBufferTable();
                    formatted->mText = remaining > 0 ? output : nullptr;
                    formatted->mCapacity = remaining > 0 ? remaining : 0;
                    if (remaining > 0) output[remaining - 1] = 0;
                    gabi::cpu->cr[6] = 0; // the two substitutions are GPR string arguments
                    s32 written = aglFormatDefinition(formatted, "#define %s %s", name, values[index]);
                    output += written;
                    for (u32 byteIndex = 0; byteIndex < static_cast<u32>(*endingWidth); ++byteIndex) {
                        remaining = static_cast<s32>(scratchEnd - gabi::ea(output));
                        gabi::FrameLocal<sead::SafeTextBuffer> characterBuffer(0x14);
                        characterBuffer->mVtable = shadow_text::definitionBufferTable();
                        characterBuffer->mText = remaining > 0 ? output : nullptr;
                        characterBuffer->mCapacity = remaining > 0 ? remaining : 0;
                        if (remaining > 0) output[remaining - 1] = 0;
                        u8 byte = source[static_cast<u32>(ending) + byteIndex];
                        characterBuffer->assureTermination();
                        be<u8>* destination = characterBuffer->mText;
                        s32 used = definitionTextLength(destination);
                        s32 capacity = characterBuffer->mCapacity;
                        if (used < capacity - 1) {
                            destination[used] = byte;
                            ++output;
                            destination[used + 1] = 0;
                        }
                    }
                    ++matchedCount;
                    handled->mFlags[index] = 1;
                    if (matchedCount == static_cast<u32>(count)) scanning = false;
                    replaced = true;
                    break;
                }
            }
        }
        u32 lineSize = static_cast<u32>(ending) + static_cast<u32>(*endingWidth);
        if (!replaced) {
            aglDefinitionMove(output, source, lineSize, 0);
            output += lineSize;
            *output = 0;
        }
        source += lineSize;
    }
    while (u8 byte = *source) { *output++ = byte; ++source; }
    gabi::FrameLocal<sead::SafeText> result(0x30);
    result->mVtable = shadow_text::definitionTextTable();
    *output = 0;
    be<u8>* destination = text->mText;
    result->mText = scratch;
    shadow_text::assureShadowTermination(&*result);
    s32 length = definitionTextLength(result->mText);
    s32 capacity = text->mCapacity;
    u32 copied = length < capacity ? static_cast<u32>(length) : static_cast<u32>(capacity) - 1;
    result->assureTermination();
    aglDefinitionMove(destination, result->mText.get(), copied, 0);
    destination[copied] = 0;
}
} }
/* WWHD glue
str 1016A508 "#define %s %s"
ext 02759C28 s32 aglFormatDefinition(sead::format::ByteTextBuffer*, const char*, const be<u8>*, const be<u8>*)
ext C0009988 void* aglDefinitionMove(void*, const void*, u32, u32)
*/
