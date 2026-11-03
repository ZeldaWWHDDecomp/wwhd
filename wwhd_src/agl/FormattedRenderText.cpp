// Variadic ABI adapter: canonical GHS ReportSave layout, with conditional raw FPR saves as documented by GABI.
#include "agl/FormattedRenderText.h"
#include "sead/HeapRegistry.h"
#include "sead/Reports.h"
#include "sead/RuntimeFormatSupport.h"
#include <bit>
using namespace agl::render;
// The text's tables in construction order: SafeText base, 256-character fixed buffer, formatted text.
static sead::SafeTextVtable sSafeTextMethods;
static sead::SafeTextVtable sFixedTextMethods;
static sead::SafeTextVtable sFormattedTextMethods;
/* 027D778C */
sead::PathText* agl::render::createFormattedRenderText(sead::PathText* text,const char* format,u32 first,u32 second,u32 third,u32 fourth,u32 fifth,u32 sixth) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<ReportSave> saved(8);
    const u32 words[8] = {gabi::ea(text),gabi::ea(format),first,second,third,fourth,fifth,sixth};
    const u8 integerOrder[8] = {3,6,5,0,7,4,1,2};
    for (u32 index = 0; index < 8; ++index) {
        u32 word = integerOrder[index];
        saved->mIntegers[word] = words[word];
    }
    if (gabi::cpu->cr[6]) {
        const u8 floatOrder[8] = {3,7,6,2,4,8,5,1};
        for (u32 index = 0; index < 8; ++index) {
            u32 reg = floatOrder[index];
            u64 bits = std::bit_cast<u64>(gabi::cpu->f[reg].ps0);
            saved->mFloatWords[(reg - 1u) * 2u] = bits >> 32;
            saved->mFloatWords[(reg - 1u) * 2u + 1u] = (u32)bits;
        }
    }
    if (!text) text = static_cast<sead::PathText*>(allocateRegistryStorage(sizeof(sead::PathText)));
    if (!text) return NULL;
    text->mText = &text->mStorage[0];
    text->mVtable = &sSafeTextMethods;
    text->mCapacity = 256;
    text->mStorage[255] = 0;
    be<u8>* buffer = text->mText;
    text->mVtable = &sFixedTextMethods;
    *buffer = 0;
    text->mVtable = &sFormattedTextMethods;
    saved->mArguments.mIntegerIndex = 2;
    saved->mArguments.mOverflow = frame.ctx.entry + 8u;
    saved->mArguments.mSavedRegisters = gabi::ea(saved->mIntegers);
    saved->mArguments.mFloatIndex = 0;
    auto* output = reinterpret_cast<sead::format::ByteTextBuffer*>(text);
    output->formatArguments(format,reinterpret_cast<sead::format::VariadicState*>(&saved->mArguments));
    return text;
}
/* WWHD glue
data sSafeTextMethods 1016C198
data sFixedTextMethods 1016C1E0
data sFormattedTextMethods 1016C230
ext 02759C10 s32 sead::format::ByteTextBuffer::formatArguments(const char*,sead::format::VariadicState*)
*/
