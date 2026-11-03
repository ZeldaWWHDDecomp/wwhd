// Local text helpers and fixed parameter storage, reconstructed from WWHD.
#include "agl/TimingHistory_l.h"
#include "agl/ParameterTree.h"
using namespace agl_history_l;
void* allocateHistoryObject(u32);
void releaseHistoryObject(void*);
void formatHistoryText(void*,const char*,VarArgs*);
void initializeFixedParameter(void*,sead::SafeText*,sead::SafeText*,agl::parameter_tree::Value*);
void finishFixedParameter(void*);
void constructHistoryFields(void*,u32,u32,u32);
static sead::SafeTextVtable sLiteralTextTable;
static sead::SafeTextVtable sBufferedTextTable;
static sead::SafeTextVtable sClearedTextTable;
static sead::SafeTextVtable sFormattedTextTable;
static sead::SafeTextVtable sParameterTextTable;
static u8 sParameterFieldTable;
static ParameterDispatchPrefix sFixedParameterTable;
typedef be<u8> ParameterTextByte;
static ParameterTextByte* sDefaultParameterText;

/* 0278B9CC */
static sead::SafeText* createLiteralText(sead::SafeText* text, be<u8>* storage) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (text == NULL) text = (sead::SafeText*)allocateHistoryObject(sizeof(*text));
    if (text != NULL) {
        text->mText = storage;
        text->mVtable = &sLiteralTextTable;
    }
    return text;
}
/* 0278BA1C */
static void assureLiteralText(sead::SafeText*) {}
/* 0278BA20 */
static void terminateLiteralBuffer(sead::SafeTextBuffer* text) {
    text->mText[u32(text->mCapacity) - 1] = 0;
}
/* 0278BA74 */
static sead::FixedSafeText<1024>* createFormattedText(sead::FixedSafeText<1024>* text, const char* format) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    gabi::FrameLocal<VarArgSaveArea> saved(0x18);
    for (u32 i = 0; i < 8; ++i) saved->mGpr[i] = gabi::cpu->r[3 + i];
    if (gabi::cpu->cr[6]) {
        for (u32 i = 0; i < 8; ++i) saved->mFpr[i] = gabi::cpu->f[1 + i].ps0;
    }
    if (text == NULL) text = (sead::FixedSafeText<1024>*)allocateHistoryObject(sizeof(*text));
    if (text != NULL) {
        text->mText = memberAt<be<u8>>(text,12);
        text->mVtable = &sBufferedTextTable;
        text->mCapacity = 1024;
        text->mStorage[1023] = 0;
        text->mVtable = &sClearedTextTable;
        *text->mText = 0;
        text->mVtable = &sFormattedTextTable;
        gabi::FrameLocal<VarArgs> arguments(8);
        arguments->mGpr = 2;
        // The stack arguments start in the caller's parameter area, 8 bytes above the entry stack pointer.
        arguments->mOverflow = gabi::at<void>(gabi::native_sp() + 0x88 + 8);
        arguments->mRegisters = saved;
        arguments->mFpr = 0;
        formatHistoryText(text, format, arguments);
    }
    return text;
}
/* 0278C660 */
static void assureHistoryText(sead::SafeText*) {}
/* 0278C664 */
static void terminateHistoryBuffer(sead::SafeTextBuffer* text) {
    text->mText[u32(text->mCapacity) - 1] = 0;
}
/* 0278C7BC */
static void assureParameterText(sead::SafeText*) {}
/* 0278C7C0 */
static void terminateParameterBuffer(sead::SafeTextBuffer* text) {
    text->mText[u32(text->mCapacity) - 1] = 0;
}
/* 0278C814 */
static ParameterField* createParameterField(ParameterField* field) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (field == NULL) field = (ParameterField*)allocateHistoryObject(sizeof(*field));
    if (field != NULL) {
        field->mValue = 0;
        field->mVtable = &sParameterFieldTable;
        field->mFlag = 0;
        field->mQualifier = 4;
        field->mReserved = 0;
        field->mKind = 0;
    }
    return field;
}
/* 0278C6E0 */
static FixedParameter* createFixedParameter(FixedParameter* parameter, sead::SafeText* name, sead::SafeText* label,
    agl::parameter_tree::Value* owner) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (parameter == NULL) parameter = (FixedParameter*)allocateHistoryObject(sizeof(*parameter));
    if (parameter != NULL) {
        initializeFixedParameter(parameter,name,label,owner);
        parameter->mVtable = &sFixedParameterTable;
        constructHistoryFields(memberAt<ParameterField>(parameter,12),4,sizeof(ParameterField),gabi::guest_fn((const void*)createParameterField));
        finishFixedParameter(parameter);
        parameter->mFieldCount = 4;
        parameter->mEnabled = 0;
    }
    return parameter;
}
/* 0278C870 */
u32 agl_history_l::ParameterField::size() { return sizeof(ParameterField); }
/* 0278C878 */
be<u8>* agl_history_l::FixedParameter::storage() { return memberAt<be<u8>>(this,0x3C); }
/* 0278C880 */
const be<u8>* agl_history_l::FixedParameter::storage() const { return memberAt<be<u8>>(this,0x3C); }
/* 0278C888 */
u32 agl_history_l::FixedParameter::storageSize() { return 512; }
/* 0278C890 */
u32 agl_history_l::FixedParameter::assignCompatible(FixedParameter* source) {
    FixedParameter* destination = this;
    auto frame = gabi::AutoFrame(gabi::current_function());
    u32 destinationSize = destination->mVtable->mElementSize.mFunction;
    ParameterDispatchPrefix* sourceTable = source->mVtable;
    u32 size = gabi::call_ptr<u32>(destinationSize,destination);
    u32 sourceSize = gabi::call_ptr<u32>(sourceTable->mElementSize.mFunction,source);
    if (size != sourceSize) return 0;
    gabi::FrameLocal<sead::SafeText> destinationName(8);
    gabi::FrameLocal<sead::SafeText> sourceName(16);
    be<u8>* defaultText = sDefaultParameterText;
    sourceName->mVtable = &sParameterTextTable;
    destinationName->mText = defaultText;
    sourceName->mText = defaultText;
    destinationName->mVtable = &sParameterTextTable;
    assureParameterText(destinationName);
    destinationName->assureTermination();
    be<u8>* first = destinationName->mText;
    sourceName->assureTermination();
    be<u8>* second = sourceName->mText;
    bool matches = first == second;
    if (!matches) {
        first = destinationName->mText;
        for (u32 i = 0; i < 0x40001; ++i) {
            u8 a = *first++;
            u8 b = *second++;
            if (a != b) return 0;
            if (a == 0) { matches = true; break; }
        }
    }
    if (!matches) return 0;
    gabi::call_ptr<void>(destination->mVtable->mAssign.mFunction,destination,source);
    return 1;
}

/* 0278B9B8 */
static void releaseLiteralText(void* storage, u32 flags) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (storage != NULL && (flags & 1)) releaseHistoryObject(storage);
}

/* 0278BA38 */
static void releaseLiteralBuffer(void* storage, u32 flags) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (storage != NULL && (flags & 1)) releaseHistoryObject(storage);
}

/* 0278BA4C */
static void releaseClearedText(void* storage, u32 flags) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (storage != NULL && (flags & 1)) releaseHistoryObject(storage);
}

/* 0278BA60 */
static void releaseFixedTextVariant(void* storage, u32 flags) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (storage != NULL && (flags & 1)) releaseHistoryObject(storage);
}

/* 0278BB94 */
static void releaseFormattedText(void* storage, u32 flags) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (storage != NULL && (flags & 1)) releaseHistoryObject(storage);
}

/* 0278C64C */
static void releaseHistoryText(void* storage, u32 flags) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (storage != NULL && (flags & 1)) releaseHistoryObject(storage);
}

/* 0278C67C */
static void releaseHistoryBuffer(void* storage, u32 flags) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (storage != NULL && (flags & 1)) releaseHistoryObject(storage);
}

/* 0278C690 */
static void releaseHistoryLabel(void* storage, u32 flags) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (storage != NULL && (flags & 1)) releaseHistoryObject(storage);
}

/* 0278C6A4 */
static void releaseHistoryLabelBuffer(void* storage, u32 flags) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (storage != NULL && (flags & 1)) releaseHistoryObject(storage);
}

/* 0278C6B8 */
static void releaseHistoryTextB(void* storage, u32 flags) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (storage != NULL && (flags & 1)) releaseHistoryObject(storage);
}

/* 0278C6CC */
static void destroyTimingHistory(void* storage, u32 flags) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (storage != NULL && (flags & 1)) releaseHistoryObject(storage);
}

/* 0278C794 */
static void releaseFixedParameter(void* storage, u32 flags) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (storage != NULL && (flags & 1)) releaseHistoryObject(storage);
}

/* 0278C7A8 */
static void releaseParameterText(void* storage, u32 flags) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (storage != NULL && (flags & 1)) releaseHistoryObject(storage);
}

/* 0278C7D8 */
static void releaseParameterBuffer(void* storage, u32 flags) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (storage != NULL && (flags & 1)) releaseHistoryObject(storage);
}

/* 0278C7EC */
static void releaseParameterTextB(void* storage, u32 flags) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (storage != NULL && (flags & 1)) releaseHistoryObject(storage);
}

/* 0278C800 */
static void releaseParameterBufferB(void* storage, u32 flags) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (storage != NULL && (flags & 1)) releaseHistoryObject(storage);
}

/* WWHD glue
data sLiteralTextTable 10163784
data sBufferedTextTable 1016379C
data sClearedTextTable 101637D4
data sFormattedTextTable 10163804
data sParameterTextTable 1016576C
data sParameterFieldTable 10165754
data sFixedParameterTable 101657E4
data sDefaultParameterText 104A0CD8
ext 0273AD10 void* allocateHistoryObject(u32)
ext 0273AF40 void releaseHistoryObject(void*)
ext 02759C10 void formatHistoryText(void*,const char*,VarArgs*)
ext 0279E710 void initializeFixedParameter(void*,sead::SafeText*,sead::SafeText*,agl::parameter_tree::Value*)
ext 0278533C void finishFixedParameter(void*)
ext 028EFFD0 void constructHistoryFields(void*,u32,u32,u32)
*/
