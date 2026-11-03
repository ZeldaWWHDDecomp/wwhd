#include "agl/CaptureTextTools.h"
#include "agl/UtilityText.h"
namespace agl { namespace capture_text {
static sead::SafeTextVtable sDialogTextTable;
/* 027C52D4 */
u32 describeFileDialogRequest(void* unusedFirst, void* unusedSecond, sead::SafeText* mode,
                              sead::SafeText* optionalId, sead::SafeText* extension,
                              sead::SafeText* fileName) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<sead::SafeText> empty(8);
    empty->mText.v = gabi::guest_str("");
    empty->mVtable = &sDialogTextTable;
    optionalId->assureTermination();
    optionalId->assureTermination();
    u32 target = empty->mVtable->mAssureTermination.mFunction;
    be<u8>* firstIdText = optionalId->mText;
    gabi::call_ptr<void>(target, &*empty);
    bool useExtension = firstIdText == empty->mText.get();
    if (!useExtension) {
        const be<u8>* left = optionalId->mText;
        const be<u8>* right = empty->mText;
        for (u32 index = 0; index < sead::kMaxTextLength + 1u; ++index) {
            u8 a = left[index];
            u8 b = right[index];
            if (a != b) break;
            if (a == 0) { useExtension = true; break; }
        }
    }
    if (useExtension) optionalId = extension;
    gabi::FrameLocal<sead::SafeText> selectedId(0x10);
    selectedId->mVtable = &sDialogTextTable;
    be<u8>* idText = optionalId->mText;
    sead::SafeTextVtable* fileNameTable = fileName->mVtable;
    selectedId->mText = idText;
    gabi::call_ptr<void>(fileNameTable->mAssureTermination.mFunction, fileName);
    target = mode->mVtable->mAssureTermination.mFunction;
    be<u8>* fileNameText = fileName->mText;
    gabi::call_ptr<void>(target, mode);
    target = selectedId->mVtable->mAssureTermination.mFunction;
    be<u8>* modeText = mode->mText;
    gabi::call_ptr<void>(target, &*selectedId);
    target = extension->mVtable->mAssureTermination.mFunction;
    idText = selectedId->mText;
    gabi::call_ptr<void>(target, extension);
    target = extension->mVtable->mAssureTermination.mFunction;
    be<u8>* filterText = extension->mText;
    gabi::call_ptr<void>(target, extension);
    target = extension->mVtable->mAssureTermination.mFunction;
    be<u8>* extensionText = extension->mText;
    gabi::call_ptr<void>(target, extension);
    be<u8>* patternText = extension->mText;
    gabi::FrameLocal<utility_text::FormattedText> description(0x18);
    gabi::cpu->cr[6] = 0; // variadic call passes strings in GPRs only
    utility_text::formatCaptureText(&*description, "FileName = %s, Mode = %s, Id = %s, Filter = %s\203t\203@\203C\203\213(*.%s)|*.%s|\202\267\202\327\202\304\202\314\203t\203@\203C\203\213(*.*)|*.*",
        gabi::ea(fileNameText), gabi::ea(modeText), gabi::ea(idText),
        gabi::ea(filterText), gabi::ea(extensionText), gabi::ea(patternText));
    return 0;
}

/* 027C5AAC */
s32 findLineEnding(const be<u8>* text, be<u32>* endingWidth) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 index = 0;
    u8 character = text[0];
    while (character) {
        if (character == '\r') {
            u8 following = text[index + 1];
            u32 width = following == '\n' ? 2 : 1;
            if (endingWidth) *endingWidth = width;
            return static_cast<s32>(index);
        }
        if (character == '\n') {
            if (endingWidth) *endingWidth = 1;
            return static_cast<s32>(index);
        }
        ++index;
        character = text[index];
    }
    return -1;
}
/* 027C5FD8 */
be<u8>* replaceTextRange(be<u8>* text, const be<u8>* replacement,
                      u32 begin, u32 end, be<u8>* suffixStorage) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 suffixLength = 0;
    const be<u8>* suffix = text + end;
    u8 character = suffix[0];
    if (character) {
        character = suffix[0];
        do {
            suffixStorage[suffixLength] = character;
            ++suffix;
            character = suffix[0];
            ++suffixLength;
        } while (character);
    }
    suffixStorage[suffixLength] = 0;
    u32 length = 0;
    character = replacement[0];
    be<u8>* insertion = text + begin;
    while (character) {
        insertion[length] = character;
        ++length;
        character = replacement[length];
    }
    insertion += length;
    length = 0;
    character = suffixStorage[0];
    while (character) {
        insertion[length] = character;
        ++length;
        character = suffixStorage[length];
    }
    insertion[length] = 0;
    return insertion;
}
} }
/* WWHD glue
data sDialogTextTable 1016A368
str 1016A3FC ""
str 1016A400 "FileName = %s, Mode = %s, Id = %s, Filter = %s\203t\203@\203C\203\213(*.%s)|*.%s|\202\267\202\327\202\304\202\314\203t\203@\203C\203\213(*.*)|*.*"
*/
