/* Fixed-capacity variadic text adapters; the register save area is sead's VariadicSaveArea at SP+8 (0x88-byte frame). */
#include "agl/UtilityText.h"
namespace agl { namespace utility_text {
static sead::SafeTextVtable sUtilityBufferTable;
static sead::SafeTextVtable sUtilityFixedBufferTable;
static sead::SafeTextVtable sUtilityFormattedTable;
static sead::SafeTextVtable sCaptureBufferTable;
static sead::SafeTextVtable sCaptureFixedBufferTable;
static sead::SafeTextVtable sCaptureFormattedTable;
/* 027C4A38 */
FormattedText* formatUtilityText(FormattedText* object, const char* format,
                                  u32 a, u32 b, u32 c, u32 d, u32 e, u32 f) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<sead::format::VariadicSaveArea> save(8);
    const u32 registers[8] = {gabi::ea(object), gabi::guest_addr(format), a, b, c, d, e, f};
    sead::format::saveVariadicRegisters(save.get(), registers);
    if (!object) {
        object = static_cast<FormattedText*>(allocateFormattedText(sizeof(FormattedText)));
        if (!object) return nullptr;
    }
    object->mText = object->mStorage;
    object->mVtable = &sUtilityBufferTable;
    object->mCapacity = 1024;
    object->mStorage[1023] = 0;
    be<u8>* storage = object->mText;
    object->mVtable = &sUtilityFixedBufferTable;
    storage[0] = 0;
    object->mVtable = &sUtilityFormattedTable;
    sead::format::startVariadicArguments(save.get(), 0x88);
    object->formatArguments(format, &save->mArguments);
    return object;
}
/* 027C5978 */
FormattedText* formatCaptureText(FormattedText* object, const char* format,
                                  u32 a, u32 b, u32 c, u32 d, u32 e, u32 f) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<sead::format::VariadicSaveArea> save(8);
    const u32 registers[8] = {gabi::ea(object), gabi::guest_addr(format), a, b, c, d, e, f};
    sead::format::saveVariadicRegisters(save.get(), registers);
    if (!object) {
        object = static_cast<FormattedText*>(allocateFormattedText(sizeof(FormattedText)));
        if (!object) return nullptr;
    }
    object->mText = object->mStorage;
    object->mVtable = &sCaptureBufferTable;
    object->mCapacity = 1024;
    object->mStorage[1023] = 0;
    be<u8>* storage = object->mText;
    object->mVtable = &sCaptureFixedBufferTable;
    storage[0] = 0;
    object->mVtable = &sCaptureFormattedTable;
    sead::format::startVariadicArguments(save.get(), 0x88);
    object->formatArguments(format, &save->mArguments);
    return object;
}
} }
/* WWHD glue
data sUtilityBufferTable 10169F04
data sUtilityFixedBufferTable 10169F9C
data sUtilityFormattedTable 10169FCC
data sCaptureBufferTable 1016A380
data sCaptureFixedBufferTable 1016A3A8
data sCaptureFormattedTable 1016A3D8
ext 0273AD10 void* allocateFormattedText(u32)
ext 02759C10 s32 sead::format::ByteTextBuffer::formatArguments(const char*, sead::format::VariadicState*)
*/
