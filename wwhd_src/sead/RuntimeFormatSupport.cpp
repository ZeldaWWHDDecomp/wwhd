/* Byte ordering and the bounded UTF-16 formatter's parsing state.
 * Reconstructed from local game instructions; no external reference used. */
#include "sead/RuntimeFormatSupport.h"
using sead::format::FormatState;
using sead::format::VariadicState;
extern void* allocateRuntimeObject(u32 size);
extern be<u32>* nextFormatArgument(FormatState* state,u32 kind);
static u32 sNativeByteOrder;
struct ByteOrderProbe { be<u16> mMarker; };
static u32 reverseWordBits(u32 value) {
    return (value << 24) | ((value << 8) & 0xFF0000) | ((value >> 8) & 0xFF00) | (value >> 24);
}
/* 02759A34 */
static u32 reverseHalfword(u32 value) {
    gabi::AutoFrame frame{gabi::current_function()};
    return ((value << 8) | static_cast<u32>(static_cast<s32>(value) >> 8)) & 0xFFFF;
}
/* 02759A48 */
static u32 reverseWord(u32 value) {
    gabi::AutoFrame frame{gabi::current_function()};
    return reverseWordBits(value);
}
/* 02759A60 */
static Pair32 reverseDoubleWord(u32 high,u32 low) {
    gabi::AutoFrame frame{gabi::current_function()};
    return {reverseWordBits(low),reverseWordBits(high)};
}
/* 02759AD8 */
static void initializeByteOrder() {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<ByteOrderProbe> probe(8);
    probe->mMarker=0xFEFF;
    auto bytes=reinterpret_cast<be<u8>*>(probe.get());
    u32 first=bytes[0];
    if(first!=0xFF && first==0xFE && static_cast<u32>(bytes[1])==0xFF) sNativeByteOrder=0;
    else sNativeByteOrder=1;
}
/* Identity converters: the {identity, reverse} pairs of the byte-order table 10147610 (8, 16, 32 and 64 bit)
 * select these when the data already has native order. Their bodies are empty in the binary, so the value in
 * r3 (r3/r4 for 64 bit) comes back unchanged. A byte has no order, so both 8-bit entries are identities. */
/* 02759B2C */
static u32 keepByteReversed(u32 value) { gabi::AutoFrame frame{gabi::current_function()}; return value; }
/* 02759B30 */
static u32 keepByte(u32 value) { gabi::AutoFrame frame{gabi::current_function()}; return value; }
/* 02759B34 */
static u32 keepHalfword(u32 value) { gabi::AutoFrame frame{gabi::current_function()}; return value; }
/* 02759B38 */
static u32 keepWord(u32 value) { gabi::AutoFrame frame{gabi::current_function()}; return value; }
/* 02759B3C */
static Pair32 keepDoubleWord(u32 high,u32 low) { gabi::AutoFrame frame{gabi::current_function()}; return {high,low}; }
/* 02759B98 */
static void emptyFormatHookA() { gabi::AutoFrame frame{gabi::current_function()}; }
/* 02759B9C */
static void emptyFormatHookB() { gabi::AutoFrame frame{gabi::current_function()}; }
/* 02759DC4 */
void sead::format::FormatState::reset() {
    gabi::AutoFrame frame{gabi::current_function()};
    mPrecision=0;
    mLength=Word;
    mHasPrecision=0;
    mWidth=0;
    mFlags=0;
}
/* 02759DE4 */
FormatState* constructFormatState(FormatState* object,const VariadicState* arguments) {
    gabi::AutoFrame frame{gabi::current_function()};
    if(object==NULL) object=static_cast<FormatState*>(allocateRuntimeObject(sizeof(FormatState)));
    if(object!=NULL) {
        object->mArguments.mRegisterState=arguments->mRegisterState;
        object->mArguments.mOverflowArguments=arguments->mOverflowArguments;
        object->mArguments.mRegisterArguments=arguments->mRegisterArguments;
        object->reset();
    }
    return object;
}
/* 02759E40 */
be<u16>* sead::format::FormatState::parseFlags(be<u16>* text) {
    gabi::AutoFrame frame{gabi::current_function()};
    for(;;) {
        u32 character=text[0];
        u32 flag;
        switch(character) {
        case '-': flag=LeftAligned; break;
        case '+': flag=ShowPlus; break;
        case '0': flag=ZeroPadded; break;
        case ' ': flag=BlankSign; break;
        case '#': flag=Alternate; break;
        default: return text;
        }
        mFlags=static_cast<u32>(mFlags)|flag;
        ++text;
    }
}
/* 02759EE8 */
be<u16>* sead::format::FormatState::parseWidth(be<u16>* text) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 character=text[0];
    if(character=='*') {
        be<u32>* value=nextFormatArgument(this,1);
        mWidth=value[0];
        return text+1;
    }
    if(character-'1'<9) {
        u32 width=character-'0';
        for(;;) {
            mWidth=width;
            character=(++text)[0];
            if(character-'0'>=10) break;
            width=width*10+(character-'0');
        }
    }
    return text;
}
/* 02759F7C */
be<u16>* sead::format::FormatState::parsePrecision(be<u16>* text) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 character=text[0];
    if(character=='.') {
        mHasPrecision=1;
        character=(++text)[0];
        if(character=='*') {
            be<u32>* value=nextFormatArgument(this,1);
            mPrecision=value[0];
            return text+1;
        }
        if(character-'0'<10) {
            u32 precision=character-'0';
            for(;;) {
                mPrecision=precision;
                character=(++text)[0];
                if(character-'0'>=10) break;
                precision=precision*10+(character-'0');
            }
        }
    }
    return text;
}
/* 0275A024 */
be<u16>* sead::format::FormatState::parseLength(be<u16>* text) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 character=text[0];
    if(character=='h') {
        character=(++text)[0];
        if(character=='h') { mLength=Byte; ++text; }
        else mLength=Halfword;
    } else if(character=='l') {
        character=(++text)[0];
        if(character=='l') { mLength=DoubleWord; ++text; }
        else mLength=LongWord;
    }
    return text;
}
/* WWHD glue
 data sNativeByteOrder 104A0CD4
 ext 0273AD10 void* allocateRuntimeObject(u32)
 ext 028F5C88 be<u32>* nextFormatArgument(sead::format::FormatState*,u32)
*/
