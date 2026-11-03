/* Decimal floating fields, reconstructed from local instructions and image power records. */
#include "sead/RuntimeFormatSupport.h"
#include "sead/RuntimeIntegerFormat_2.h"
using namespace sead::format;
extern be<u32>* nextIntegerFormatArgument(FormatState*,u32);
extern void* clearFloatingDecimal(void*,s32,u32);
// A power of two as decimal digits: the decimal exponent of its first digit and 20 digits (0-9).
struct DecimalPower { be<s32> mExponent; be<u8> mDigits[20]; };
WWHD_SIZE(DecimalPower,24);
// Indexed by the binary exponent plus 52 (biased exponent + 51); a view of the game's table.
struct DecimalPowerTable { DecimalPower mPowers[2098]; };
static DecimalPowerTable sFloatingPowers;
struct FloatingStorage {
    be<u32> mDigits[21];
    u8 mPadding[4];
    be<u32> mHigh,mLow;
    be<u32> mExponentDigits[3];
};
WWHD_OFFSET(FloatingStorage,mHigh,0x58);
WWHD_OFFSET(FloatingStorage,mExponentDigits,0x60);
static bool emitFloatingCharacter(DigitBuffer* output,u32 character) {
    s32 count=output->mCount;
    if(count>=static_cast<s32>(output->mCapacity)) { output->mOverflow=1; return false; }
    output->mText[count]=character;
    output->mCount=static_cast<u32>(output->mCount)+1;
    return true;
}
static void emitFloatingRepeat(DigitBuffer* output,u32 count,u32 character) {
    for(u32 index=0;index<count;++index) if(!emitFloatingCharacter(output,character)) break;
}
/* 0275A86C */
void writeFloatingField(FormatState* state,DigitBuffer* output,u32 conversion,u32 uppercase) {
    gabi::AutoFrame frame{gabi::current_function()};
    be<u32>* argument=nextIntegerFormatArgument(state,3);
    gabi::FrameLocal<FloatingStorage> storage(8);
    storage->mHigh=argument[0];
    storage->mLow=argument[1];
    u32 high=storage->mHigh,low=storage->mLow;
    bool negative=(high>>31)!=0;
    u32 exponent=(high>>20)&0x7FF;
    u32 fraction=high&0xFFFFF;
    if(exponent==0x7FF) {
        state->mHasPrecision=0;
        u32 flags=state->mFlags;
        const char* special;
        if(fraction|low) special=negative?"-NaN":(flags&FormatState::ShowPlus)?"+NaN":(flags&FormatState::BlankSign)?" NaN":"NaN";
        else special=negative?"-Inf":(flags&FormatState::ShowPlus)?"+Inf":(flags&FormatState::BlankSign)?" Inf":"Inf";
        writeByteTextField(state,output,gabi::at<const be<u8>>(gabi::guest_str(special)));
        return;
    }
    u32 precision=state->mHasPrecision?static_cast<u32>(state->mPrecision):6;
    if(precision>512) precision=512;
    u32 mantissaHigh=(fraction<<11)|(low>>21);
    u32 mantissaLow=low<<11;
    u32 powerIndex;
    if(exponent) { mantissaHigh|=0x80000000; powerIndex=exponent+51; }
    else if(fraction|low) {
        powerIndex=52;
        do {
            mantissaHigh=(mantissaHigh<<1)|(mantissaLow>>31);
            mantissaLow<<=1;
            --powerIndex;
        } while(!(mantissaHigh&0x80000000));
    } else powerIndex=1074;
    // Finite subnormals normalize within this table; the lowest one reads record zero.
    const DecimalPower* power=&sFloatingPowers.mPowers[0]+static_cast<s32>(powerIndex);
    s32 point=power->mExponent;
    clearFloatingDecimal(storage->mDigits,0,84);
    while(mantissaHigh|mantissaLow) {
        if(mantissaHigh&0x80000000) {
            s32 offset=point-static_cast<s32>(power->mExponent)+1;
            if(offset<21) {
                for(s32 index=offset;index<21;++index) {
                    storage->mDigits[index]=static_cast<u32>(storage->mDigits[index])+static_cast<u32>(power->mDigits[index-offset]);
                }
            }
            if(offset>0 && offset>=21 && static_cast<u32>(power->mDigits[21-offset])>=5)
                storage->mDigits[20]=static_cast<u32>(storage->mDigits[20])+1;
        }
        mantissaHigh=(mantissaHigh<<1)|(mantissaLow>>31);
        mantissaLow<<=1;
        --power;
    }
    for(u32 index=20;index>0;--index) {
        u32 carry=static_cast<u32>(storage->mDigits[index])/10;
        storage->mDigits[index-1]=static_cast<u32>(storage->mDigits[index-1])+carry;
        storage->mDigits[index]=static_cast<u32>(storage->mDigits[index])-carry*10;
    }
    be<u32>* digits=storage->mDigits+1;
    if(static_cast<u32>(storage->mDigits[0])) { digits=storage->mDigits; ++point; }
    s32 significant=0;
    bool round=false;
    if(conversion=='e') { significant=precision+1; round=significant>=0; }
    else if(conversion=='f') { significant=precision+point; round=significant>=0; }
    else if(conversion=='g') { significant=precision?precision:1; round=true; }
    if(round) {
        if(significant>17) significant=17;
        if(static_cast<u32>(digits[significant])>=5) {
            be<u32>* cursor=digits+significant-1;
            u32 value=static_cast<u32>(*cursor)+1;
            while(value>=10 && gabi::ea(cursor)>=gabi::ea(storage->mDigits)) {
                *cursor=value-10;
                --cursor;
                value=static_cast<u32>(*cursor)+1;
            }
            *cursor=value;
            if(cursor==digits-1) { ++point; --digits; }
        }
    } else significant=0;
    u32 integerDigits=0,integerZeros=0,leadingFractionZeros=0,fractionDigits=0,trailingFractionZeros=0;
    bool decimal=false,scientific=false;
    bool alternate=(static_cast<u32>(state->mFlags)&FormatState::Alternate)!=0;
    if(conversion=='f') {
        if(point>=significant) {
            integerDigits=significant; integerZeros=point-significant;
            trailingFractionZeros=precision; decimal=precision!=0 || alternate;
        } else if(point>=0) {
            integerDigits=point; fractionDigits=significant-point;
            trailingFractionZeros=precision+point-significant; decimal=true;
        } else {
            integerZeros=1; leadingFractionZeros=0u-static_cast<u32>(point); decimal=true;
            if(precision<=leadingFractionZeros) leadingFractionZeros=precision;
            else if(precision<=leadingFractionZeros+significant) fractionDigits=precision+point;
            else { fractionDigits=significant; trailingFractionZeros=precision-significant+point; }
        }
    } else if(conversion=='e') {
        integerDigits=1; fractionDigits=significant-1;
        trailingFractionZeros=precision-fractionDigits;
        decimal=precision!=0 || alternate; scientific=true;
    } else if(conversion=='g') {
        if(!alternate) while(significant && static_cast<u32>(digits[significant-1])==0) --significant;
        if(point < -3 || point>static_cast<s32>(precision)) {
            scientific=true; integerDigits=1; fractionDigits=significant-1;
            decimal=precision>1 || alternate;
            if(alternate) trailingFractionZeros=precision-significant;
        } else if(point>=significant) {
            integerDigits=significant; integerZeros=point-significant;
            decimal=alternate;
            if(point<static_cast<s32>(precision) && alternate) trailingFractionZeros=precision-point;
        } else if(point>0) {
            integerDigits=point; fractionDigits=significant-point; decimal=true;
            if(alternate) trailingFractionZeros=precision-integerDigits-fractionDigits;
        } else {
            integerZeros=1; leadingFractionZeros=0u-static_cast<u32>(point);
            fractionDigits=significant; decimal=true;
            if(alternate) trailingFractionZeros=precision-significant;
        }
    }
    u32 extent=(negative || (static_cast<u32>(state->mFlags)&(FormatState::ShowPlus|FormatState::BlankSign)))?1:0;
    extent+=integerDigits+integerZeros+leadingFractionZeros+fractionDigits+trailingFractionZeros+decimal+(scientific?5:0);
    u32 before=0,zeros=0,after=0;
    u32 width=state->mWidth,flags=state->mFlags;
    if(extent<width) {
        if(flags&FormatState::LeftAligned) after=width-extent;
        else if(flags&FormatState::ZeroPadded) zeros=width-extent;
        else before=width-extent;
    }
    emitFloatingRepeat(output,before,' ');
    if(negative) emitFloatingCharacter(output,'-');
    else if(static_cast<u32>(state->mFlags)&FormatState::ShowPlus) emitFloatingCharacter(output,'+');
    else if(static_cast<u32>(state->mFlags)&FormatState::BlankSign) emitFloatingCharacter(output,' ');
    emitFloatingRepeat(output,zeros,'0');
    for(u32 index=0;index<integerDigits;++index) emitFloatingCharacter(output,static_cast<u32>(digits[index])+'0');
    emitFloatingRepeat(output,integerZeros,'0');
    if(decimal) emitFloatingCharacter(output,'.');
    emitFloatingRepeat(output,leadingFractionZeros,'0');
    for(u32 index=0;index<fractionDigits;++index) emitFloatingCharacter(output,static_cast<u32>(digits[integerDigits+index])+'0');
    emitFloatingRepeat(output,trailingFractionZeros,'0');
    if(scientific) {
        emitFloatingCharacter(output,uppercase?'E':'e');
        bool positive=point>0;
        u32 magnitude=positive?static_cast<u32>(point)-1:1u-static_cast<u32>(point);
        for(u32 index=0;index<3;++index) {
            storage->mExponentDigits[index]=magnitude%10;
            magnitude/=10;
        }
        emitFloatingCharacter(output,positive?'+':'-');
        for(u32 index=3;index>0;--index) emitFloatingCharacter(output,static_cast<u32>(storage->mExponentDigits[index-1])+'0');
    }
    emitFloatingRepeat(output,after,' ');
}
/* WWHD glue
 data sFloatingPowers 10151900
 ext C0009990 void* clearFloatingDecimal(void*,s32,u32)
 str 101518C4 "NaN"
 str 101518C8 "Inf"
 str 101518CC "-NaN"
 str 101518D4 "+NaN"
 str 101518DC " NaN"
 str 101518E4 "-Inf"
 str 101518EC "+Inf"
 str 101518F4 " Inf"
*/
