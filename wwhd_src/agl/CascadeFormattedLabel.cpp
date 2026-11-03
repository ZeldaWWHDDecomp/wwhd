// The formatting constructor saves the GHS integer/FPR arguments before any allocation.
#include "bindings.h"
#include "agl/CascadeFormattedLabel.h"
void* allocateCascadeFormattedLabel(u32);
static u32 sCascadeFormattedBufferBaseTable;
static u32 sCascadeFormattedBaseTable;
static u32 sCascadeFormattedTable;
/* 02798F8C */
agl::CascadeFormattedLabel* agl::CascadeFormattedLabel::createFormatted(CascadeFormattedLabel* label,
        const char* format, u32 firstArgument, u32 secondArgument, u32 thirdArgument,
        u32 fourthArgument, u32 fifthArgument, u32 sixthArgument) {
    u32 entryStack = gabi::cpu->r[1];
    bool floatingArguments = gabi::cpu->cr[6] != 0;
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<CascadeFormatSave> save(8);
    save->mIntegerArguments[0] = gabi::ea(label);
    save->mIntegerArguments[1] = gabi::ea(format);
    save->mIntegerArguments[2] = firstArgument;
    save->mIntegerArguments[3] = secondArgument;
    save->mIntegerArguments[4] = thirdArgument;
    save->mIntegerArguments[5] = fourthArgument;
    save->mIntegerArguments[6] = fifthArgument;
    save->mIntegerArguments[7] = sixthArgument;
    if (floatingArguments) {
        for (u32 index = 0; index < 8; ++index) {
            u64 bits;
            memcpy(&bits, &gabi::cpu->f[index + 1].ps0, sizeof(bits));
            save->mFloatingArgumentWords[index * 2] = bits >> 32;
            save->mFloatingArgumentWords[index * 2 + 1] = (u32)bits;
        }
    }
    if (label == NULL) label = (CascadeFormattedLabel*)allocateCascadeFormattedLabel(sizeof(CascadeFormattedLabel));
    if (label == NULL) return label;
    sead::format::ByteTextBuffer* base = label;
    if (base == NULL) base = (sead::format::ByteTextBuffer*)allocateCascadeFormattedLabel(sizeof(sead::format::ByteTextBuffer));
    if (base != NULL) {
        base->mText = gabi::at<be<u8>>(gabi::ea(label) + offsetof(CascadeFormattedLabel, mStorage));
        base->mVtable = gabi::at<void>(gabi::ea(&sCascadeFormattedBufferBaseTable));
        base->mCapacity = 32;
        label->mStorage[31] = 0;
    }
    be<u8>* text = label->mText;
    label->mVtable = gabi::at<void>(gabi::ea(&sCascadeFormattedBaseTable));
    *text = 0;
    label->mVtable = gabi::at<void>(gabi::ea(&sCascadeFormattedTable));
    // The other two bytes of the packed register-state word are untouched GHS padding.
    auto* indexes = gabi::at<be<u8>>(gabi::ea(&save->mArguments.mRegisterState));
    indexes[0] = 2;
    save->mArguments.mOverflowArguments = gabi::at<void>(entryStack + 8);
    save->mArguments.mRegisterArguments = &save->mIntegerArguments[0];
    indexes[1] = 0;
    label->formatArguments(format, &save->mArguments);
    return label;
}
/* WWHD glue
 data sCascadeFormattedBufferBaseTable 10166DB0
 data sCascadeFormattedBaseTable 10166DE8
 data sCascadeFormattedTable 10166E58
 ext 0273AD10 void* allocateCascadeFormattedLabel(u32)
 ext 02759C10 s32 sead::format::ByteTextBuffer::formatArguments(const char*, sead::format::VariadicState*)
*/
