// Formatted text construction and the observed GHS argument-save layout.
#include "agl/EnvironmentFormattedText.h"
#include "sead/Reports.h"
using namespace agl::env;
static sead::SafeTextVtable sShortFormattingBaseMethods;
static sead::SafeTextVtable sShortFormattingBufferMethods;
static sead::SafeTextVtable sShortFormattingFixedMethods;
static sead::SafeTextVtable sManagerFormattingBaseMethods;
static sead::SafeTextVtable sManagerFormattingBufferMethods;
static sead::SafeTextVtable sManagerFormattingFixedMethods;
static sead::SafeTextVtable sSetFormattingBaseMethods;
static sead::SafeTextVtable sSetFormattingBufferMethods;
static sead::SafeTextVtable sSetFormattingFixedMethods;
void* allocateEnvironmentFormattedText(u32);
void formatEnvironmentText(sead::SafeTextBuffer*,const char*,ReportArguments*);

/* inlined into the two formatted-text constructors */
static void saveEnvironmentFormattingArguments(ReportSave* save,const u32* registers,bool floats) {
    for (u32 index = 0; index < 8; ++index) save->mIntegers[index] = registers[index];
    if (floats) {
        for (u32 index = 0; index < 8; ++index) {
            u64 bits;
            memcpy(&bits,&gabi::cpu->f[index + 1].ps0,8);
            save->mFloatWords[index * 2] = u32(bits >> 32);
            save->mFloatWords[index * 2 + 1] = u32(bits);
        }
    }
}

/* 025F64C0 */
EnvironmentShortFormattedText* EnvironmentShortFormattedText::construct(EnvironmentShortFormattedText* object,const char* format,
    u32 arg5,u32 arg6,u32 arg7,u32 arg8,u32 arg9,u32 arg10) {
    u32 entry = gabi::cpu->r[1];
    bool floats = gabi::cpu->cr[6] != 0;
    gabi::NativeFrame<136> frame;
    ReportSave* save = &gabi::object_at<ReportSave>(frame + 8);
    const u32 registers[8] = {gabi::ea(object),gabi::ea(format),arg5,arg6,arg7,arg8,arg9,arg10};
    saveEnvironmentFormattingArguments(save,registers,floats);
    if (object == NULL) object = static_cast<EnvironmentShortFormattedText*>(allocateEnvironmentFormattedText(sizeof(EnvironmentShortFormattedText)));
    if (object == NULL) return NULL;
    object->mText = object->mStorage;
    object->mVtable = &sShortFormattingBaseMethods;
    object->mCapacity = 256;
    object->mStorage[255] = 0;
    be<u8>* first = object->mText;
    object->mVtable = &sShortFormattingBufferMethods;
    first[0] = 0;
    object->mVtable = &sShortFormattingFixedMethods;
    save->mArguments.mIntegerIndex = 2;
    save->mArguments.mFloatIndex = 0;
    save->mArguments.mOverflow = entry + 8;
    save->mArguments.mSavedRegisters = gabi::ea(save->mIntegers);
    formatEnvironmentText(object,format,&save->mArguments);
    return object;
}
/* 0276CC68 */
EnvironmentManagerFormattedText* EnvironmentManagerFormattedText::construct(EnvironmentManagerFormattedText* object,const char* format,
    u32 arg5,u32 arg6,u32 arg7,u32 arg8,u32 arg9,u32 arg10) {
    u32 entry = gabi::cpu->r[1];
    bool floats = gabi::cpu->cr[6] != 0;
    gabi::NativeFrame<136> frame;
    ReportSave* save = &gabi::object_at<ReportSave>(frame + 8);
    const u32 registers[8] = {gabi::ea(object),gabi::ea(format),arg5,arg6,arg7,arg8,arg9,arg10};
    saveEnvironmentFormattingArguments(save,registers,floats);
    if (object == NULL) object = static_cast<EnvironmentManagerFormattedText*>(allocateEnvironmentFormattedText(sizeof(EnvironmentManagerFormattedText)));
    if (object == NULL) return NULL;
    object->mText = object->mStorage;
    object->mVtable = &sManagerFormattingBaseMethods;
    object->mCapacity = 1024;
    object->mStorage[1023] = 0;
    be<u8>* first = object->mText;
    object->mVtable = &sManagerFormattingBufferMethods;
    first[0] = 0;
    object->mVtable = &sManagerFormattingFixedMethods;
    save->mArguments.mIntegerIndex = 2;
    save->mArguments.mFloatIndex = 0;
    save->mArguments.mOverflow = entry + 8;
    save->mArguments.mSavedRegisters = gabi::ea(save->mIntegers);
    formatEnvironmentText(object,format,&save->mArguments);
    return object;
}
/* 0276EC20 */
EnvironmentSetFormattedText* EnvironmentSetFormattedText::construct(EnvironmentSetFormattedText* object,const char* format,
    u32 arg5,u32 arg6,u32 arg7,u32 arg8,u32 arg9,u32 arg10) {
    u32 entry = gabi::cpu->r[1];
    bool floats = gabi::cpu->cr[6] != 0;
    gabi::NativeFrame<136> frame;
    ReportSave* save = &gabi::object_at<ReportSave>(frame + 8);
    const u32 registers[8] = {gabi::ea(object),gabi::ea(format),arg5,arg6,arg7,arg8,arg9,arg10};
    saveEnvironmentFormattingArguments(save,registers,floats);
    if (object == NULL) object = static_cast<EnvironmentSetFormattedText*>(allocateEnvironmentFormattedText(sizeof(EnvironmentSetFormattedText)));
    if (object == NULL) return NULL;
    object->mText = object->mStorage;
    object->mVtable = &sSetFormattingBaseMethods;
    object->mCapacity = 1024;
    object->mStorage[1023] = 0;
    be<u8>* first = object->mText;
    object->mVtable = &sSetFormattingBufferMethods;
    first[0] = 0;
    object->mVtable = &sSetFormattingFixedMethods;
    save->mArguments.mIntegerIndex = 2;
    save->mArguments.mFloatIndex = 0;
    save->mArguments.mOverflow = entry + 8;
    save->mArguments.mSavedRegisters = gabi::ea(save->mIntegers);
    formatEnvironmentText(object,format,&save->mArguments);
    return object;
}
/* WWHD glue
 data sShortFormattingBaseMethods 100E0818
 data sShortFormattingBufferMethods 100E0860
 data sShortFormattingFixedMethods 100E08D0
 ext 0273AD10 void* allocateEnvironmentFormattedText(u32)
 ext 02759C10 void formatEnvironmentText(sead::SafeTextBuffer*,const char*,ReportArguments*)
 data sManagerFormattingBaseMethods 1015FB60
 data sManagerFormattingBufferMethods 1015FBB8
 data sManagerFormattingFixedMethods 1015FBE8
 data sSetFormattingBaseMethods 10160134
 data sSetFormattingBufferMethods 10160234
 data sSetFormattingFixedMethods 10160264
*/
