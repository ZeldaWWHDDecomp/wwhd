/* sead string instance emitted in the hd_res_model translation unit (HDTU-LIB): the variadic
 * constructor of FormatFixedSafeString<260> (the model and texture path buffers), which formats
 * into its own storage.
 * Reconstruction from the game instructions; */
#include "bindings.h"
#include "sead/HdTuInstances.h"

static u8 sBufferedTextVtable;
static u8 sFixedTextVtable;
static u8 sFormatTextVtable;

/* 026032F0 */
sead::instances::res_model::PathText* sead::instances::res_model::PathText::construct(PathText* text, const char* format, u32 a, u32 b, u32 c, u32 d, u32 e, u32 f) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<VariadicSaveArea> save(8);
    const u32 registers[8] = {gabi::ea(text), gabi::ea(format), a, b, c, d, e, f};
    saveVariadicRegisters(save.get(), registers);
    if (text == NULL) {
        text = static_cast<PathText*>(operator_new(sizeof(PathText)));
        if (text == NULL) return NULL;
    }
    // BufferedSafeString(mStorage, 260), then the FixedSafeString and FormatFixedSafeString bodies
    text->mText = text->mStorage;
    text->mVtable = &sBufferedTextVtable;
    text->mCapacity = sizeof(text->mStorage);
    text->mStorage[sizeof(text->mStorage) - 1] = 0;
    be<u8>* buffer = text->mText;
    text->mVtable = &sFixedTextVtable;
    buffer[0] = 0;
    text->mVtable = &sFormatTextVtable;
    startVariadicArguments(save.get(), 0x88);
    text->formatArguments(format, &save->mArguments);
    return text;
}

/* WWHD glue
data sBufferedTextVtable 100E1870 nocheck
data sFixedTextVtable 100E18D8 nocheck
data sFormatTextVtable 100E1908 nocheck
ext 02759C10 s32 sead::format::ByteTextBuffer::formatArguments(const char*, sead::format::VariadicState*)
*/
