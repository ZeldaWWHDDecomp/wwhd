// The environment object's formatted icon-label text (sead::FixedSafeText<1024>) and its variadic save area.
#include "agl/EnvironmentFormattedText.h"
#include "sead/Reports.h"
using namespace agl::env;
void* allocateFormattedText(u32 size);
void formatBufferedArguments(sead::SafeTextBuffer* text,const char* format,ReportArguments* arguments);
static sead::SafeTextVtable sBufferTextMethods;
static sead::SafeTextVtable sIntermediateTextMethods;
static sead::SafeTextVtable sFormattedTextMethods;
// A floating-point argument register in the save area: the two words of its double value.
static void saveFloatArgument(ReportSave* save,u32 index,f64 value) {
    u64 bits;
    memcpy(&bits,&value,8);
    save->mFloatWords[index*2]=u32(bits>>32);
    save->mFloatWords[index*2+1]=u32(bits);
}
/* 0277077C */
EnvironmentObjectFormattedText* EnvironmentObjectFormattedText::construct(EnvironmentObjectFormattedText* destination,const char* format,
    u32 firstArgument,u32 secondArgument,u32 thirdArgument,u32 fourthArgument,u32 fifthArgument,u32 sixthArgument) {
    const u32 entry=gabi::cpu->r[1];
    auto frame=gabi::AutoFrame(gabi::current_function());
    // The GHS variadic save area: ReportSave at sp+8 (arguments, then r3-r10 at sp+0x18 and f1-f8 at sp+0x38).
    gabi::FrameLocal<ReportSave> save(8);
    save->mIntegers[3]=secondArgument;
    save->mIntegers[6]=fifthArgument;
    save->mIntegers[5]=fourthArgument;
    save->mIntegers[0]=gabi::ea(destination);
    save->mIntegers[7]=sixthArgument;
    save->mIntegers[4]=thirdArgument;
    save->mIntegers[1]=gabi::ea(format);
    save->mIntegers[2]=firstArgument;
    if(gabi::cpu->cr[6]) {
        saveFloatArgument(save.get(),2,gabi::cpu->f[3].ps0);
        saveFloatArgument(save.get(),6,gabi::cpu->f[7].ps0);
        saveFloatArgument(save.get(),5,gabi::cpu->f[6].ps0);
        saveFloatArgument(save.get(),1,gabi::cpu->f[2].ps0);
        saveFloatArgument(save.get(),3,gabi::cpu->f[4].ps0);
        saveFloatArgument(save.get(),7,gabi::cpu->f[8].ps0);
        saveFloatArgument(save.get(),4,gabi::cpu->f[5].ps0);
        saveFloatArgument(save.get(),0,gabi::cpu->f[1].ps0);
    }
    if(destination==nullptr) destination=static_cast<EnvironmentObjectFormattedText*>(allocateFormattedText(sizeof(EnvironmentObjectFormattedText)));
    if(destination!=nullptr) {
        destination->mText=destination->mStorage;
        destination->mVtable=&sBufferTextMethods;
        destination->mCapacity=1024;
        destination->mStorage[1023]=0;
        be<u8>* first=destination->mText;
        destination->mVtable=&sIntermediateTextMethods;
        *first=0;
        destination->mVtable=&sFormattedTextMethods;
        save->mArguments.mIntegerIndex=2;
        // The overflow arguments are the caller's stack arguments at entry sp + 8.
        save->mArguments.mOverflow=entry+8;
        save->mArguments.mSavedRegisters=gabi::ea(save->mIntegers);
        save->mArguments.mFloatIndex=0;
        formatBufferedArguments(destination,format,&save->mArguments);
    }
    return destination;
}
/* WWHD glue
ext 0273AD10 void* allocateFormattedText(u32)
ext 02759C10 void formatBufferedArguments(sead::SafeTextBuffer*,const char*,ReportArguments*)
data sBufferTextMethods 101607F4
data sIntermediateTextMethods 1016084C
data sFormattedTextMethods 1016087C
*/
