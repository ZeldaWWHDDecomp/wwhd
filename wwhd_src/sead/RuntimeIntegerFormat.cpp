/* Bounded formatting and reverse integer digits, from local game instructions. */
#include "sead/RuntimeFormatSupport.h"
#include "sead/RuntimeIntegerFormat_2.h"
#include "sead/ElementAt.h"
using namespace sead::format;
extern be<u32>* nextIntegerFormatArgument(FormatState*,u32);
extern Pair32 divideFormatDoubleWord(u32,u32,u32,u32);
extern s32 formatPlatformBytes(be<u8>*,s32,const char*,VariadicState*);
extern s32 formatWideUnits(be<u16>*,s32,const be<u16>*,VariadicState*);
static void appendFormatDigit(DigitBuffer* output,u32 character) {
    s32 count=output->mCount;
    if(count>=static_cast<s32>(output->mCapacity)) output->mOverflow=1;
    else {
        be<u16>* text=output->mText;
        text[count]=character;
        output->mCount=static_cast<u32>(output->mCount)+1;
    }
}
/* 02759BA0 */
static s32 formatBoundedBytes(be<u8>* output,s32 capacity,const char* format,VariadicState* arguments) {
    gabi::AutoFrame frame{gabi::current_function()};
    s32 result=formatPlatformBytes(output,capacity,format,arguments);
    if(result<0 || result>=capacity) {
        output[static_cast<u32>(capacity)-1]=0;
        return static_cast<u32>(capacity)-1;
    }
    return result;
}
/* 02759C10 */
s32 sead::format::ByteTextBuffer::formatArguments(const char* format,VariadicState* arguments) {
    gabi::AutoFrame frame{gabi::current_function()};
    return formatBoundedBytes(mText,mCapacity,format,arguments);
}
/* 02759CB0 */
static s32 formatBoundedWideUnits(be<u16>* output,s32 capacity,const be<u16>* format,VariadicState* arguments) {
    gabi::AutoFrame frame{gabi::current_function()};
    s32 result=formatWideUnits(output,capacity,format,arguments);
    if(result<0 || result>=capacity) {
        sead::elementAt(output,static_cast<u32>(capacity)-1)=0;
        return static_cast<u32>(capacity)-1;
    }
    return result;
}
/* 02759D24 */
s32 sead::format::WideTextBuffer::formatArguments(const be<u16>* format,VariadicState* arguments) {
    gabi::AutoFrame frame{gabi::current_function()};
    return formatBoundedWideUnits(mText,mCapacity,format,arguments);
}
/* 0275BF18 */
void sead::format::DigitBuffer::writeWord(u32 radix,u32 uppercase,u32 value) {
    gabi::AutoFrame frame{gabi::current_function()};
    if(value==0) { appendFormatDigit(this,'0'); return; }
    if(radix!=8 && radix!=10 && radix!=16) return;
    u32 alphabet=uppercase!=0?'A':'a';
    while(value!=0) {
        u32 quotient;
        u32 digit;
        if(radix==8) { digit=value&7; quotient=value>>3; }
        else if(radix==16) { digit=value&15; quotient=value>>4; }
        else { quotient=value/10; digit=value-quotient*10; }
        appendFormatDigit(this,digit<10?'0'+digit:alphabet+digit-10);
        value=quotient;
    }
}
/* 0275C0D0 */
void sead::format::FormatState::writeByteDigits(DigitBuffer* digits,be<u8>* negative,u32 isSigned,u32 radix,u32 uppercase) {
    gabi::AutoFrame frame{gabi::current_function()};
    be<u32>* argument=nextIntegerFormatArgument(this,1);
    auto bytes=reinterpret_cast<be<u8>*>(argument);
    u32 value=bytes[3];
    bool minus=isSigned!=0 && static_cast<s8>(value)<0;
    if(minus) value=0u-static_cast<u32>(static_cast<s32>(static_cast<s8>(value)));
    digits->writeWord(radix,uppercase,value);
    if(minus) negative[0]=1;
}
/* 0275C1B0 */
void sead::format::FormatState::writeHalfwordDigits(DigitBuffer* digits,be<u8>* negative,u32 isSigned,u32 radix,u32 uppercase) {
    gabi::AutoFrame frame{gabi::current_function()};
    be<u32>* argument=nextIntegerFormatArgument(this,1);
    auto halves=reinterpret_cast<be<u16>*>(argument);
    u32 value=halves[1];
    bool minus=isSigned!=0 && static_cast<s16>(value)<0;
    if(minus) value=0u-static_cast<u32>(static_cast<s32>(static_cast<s16>(value)));
    digits->writeWord(radix,uppercase,value);
    if(minus) negative[0]=1;
}
/* 0275C290 */
void sead::format::FormatState::writeWordDigits(DigitBuffer* digits,be<u8>* negative,u32 isSigned,u32 radix,u32 uppercase) {
    gabi::AutoFrame frame{gabi::current_function()};
    be<u32>* argument=nextIntegerFormatArgument(this,1);
    u32 value=argument[0];
    bool minus=isSigned!=0 && static_cast<s32>(value)<0;
    if(minus) value=0u-value;
    digits->writeWord(radix,uppercase,value);
    if(minus) negative[0]=1;
}
/* 0275C370 */
void sead::format::DigitBuffer::writeDoubleWord(u32 radix,u32 uppercase,u32 alignmentWord,u32 high,u32 low) {
    gabi::AutoFrame frame{gabi::current_function()};
    if((high|low)==0) { appendFormatDigit(this,'0'); return; }
    if(radix!=8 && radix!=10 && radix!=16) return;
    u32 alphabet=uppercase!=0?'A':'a';
    while((high|low)!=0) {
        u32 nextHigh;
        u32 nextLow;
        u32 digit;
        if(radix==10) {
            auto quotient=divideFormatDoubleWord(high,low,0,10);
            nextHigh=quotient.r3;
            nextLow=quotient.r4;
            digit=low-nextLow*10;
        } else {
            u32 shift=radix==8?3:4;
            digit=low&((1u<<shift)-1);
            nextLow=(low>>shift)|(high<<(32-shift));
            nextHigh=high>>shift;
        }
        appendFormatDigit(this,digit<10?'0'+digit:alphabet+digit-10);
        high=nextHigh;
        low=nextLow;
    }
}
/* 0275C62C */
void sead::format::FormatState::writeDoubleWordDigits(DigitBuffer* digits,be<u8>* negative,u32 isSigned,u32 radix,u32 uppercase) {
    gabi::AutoFrame frame{gabi::current_function()};
    be<u32>* argument=nextIntegerFormatArgument(this,2);
    u32 high=argument[0];
    u32 low=argument[1];
    bool minus=isSigned!=0 && static_cast<s32>(high)<0;
    if(minus) { high=0u-high-(low!=0); low=0u-low; }
    // The pair is aligned at r7:r8; r6 keeps the variadic helper's live value.
    u32 alignmentWord=gabi::cpu->r[6];
    digits->writeDoubleWord(radix,uppercase,alignmentWord,high,low);
    if(minus) negative[0]=1;
}

/* 02759C28 */
s32 sead::format::ByteTextBuffer::format(const char* text,u32 a,u32 b,u32 c,u32 d,u32 e,u32 f) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<VariadicSaveArea> save(8);
    const u32 registers[8]={gabi::ea(this),gabi::ea(text),a,b,c,d,e,f};
    saveVariadicRegisters(save.get(),registers);
    startVariadicArguments(save.get(),0x78);
    return formatArguments(text,&save->mArguments);
}
/* 02759D3C */
s32 sead::format::WideTextBuffer::format(const be<u16>* text,u32 a,u32 b,u32 c,u32 d,u32 e,u32 f) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<VariadicSaveArea> save(8);
    const u32 registers[8]={gabi::ea(this),gabi::ea(text),a,b,c,d,e,f};
    saveVariadicRegisters(save.get(),registers);
    startVariadicArguments(save.get(),0x78);
    return formatArguments(text,&save->mArguments);
}

static bool appendFieldCharacter(DigitBuffer* output,u32 character) {
    s32 count=output->mCount;
    if(count>=static_cast<s32>(output->mCapacity)) {
        output->mOverflow=1;
        return false;
    }
    output->mText[count]=character;
    output->mCount=static_cast<u32>(output->mCount)+1;
    return true;
}
/* 0275A5AC */
void sead::format::FormatState::writeCharacter(DigitBuffer* output) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 character=mLength;
    if(character<=4) {
        be<u32>* argument=nextIntegerFormatArgument(this,1);
        if(character<=1) character=reinterpret_cast<be<u8>*>(argument)[3];
        else character=reinterpret_cast<be<u16>*>(argument)[1];
    }
    u32 present=character!=0;
    u32 width=mWidth;
    if(present>=width) {
        if(character) appendFieldCharacter(output,character);
        return;
    }
    u32 padding=width-present;
    if(!(static_cast<u32>(mFlags)&LeftAligned)) {
        u32 fill=(static_cast<u32>(mFlags)&ZeroPadded)?'0':' ';
        for(u32 index=0;index<padding;++index) {
            if(!appendFieldCharacter(output,fill)) break;
        }
    }
    if(character) appendFieldCharacter(output,character);
    if(static_cast<u32>(mFlags)&LeftAligned) {
        for(u32 index=0;index<padding;++index) {
            if(!appendFieldCharacter(output,' ')) break;
        }
    }
}
/* 0275A7BC */
void sead::format::FormatState::writeString(DigitBuffer* output) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 length=mLength;
    if(length<=1) {
        gptr<const be<u8>>* text=reinterpret_cast<gptr<const be<u8>>*>(nextIntegerFormatArgument(this,1));
        writeByteTextField(this,output,*text);
    } else if(length<=4) {
        gptr<const be<u16>>* text=reinterpret_cast<gptr<const be<u16>>*>(nextIntegerFormatArgument(this,1));
        writeWideTextField(this,output,*text);
    }
}

extern FormatState* constructFormatState(FormatState*,const VariadicState*);
extern u32 parseFormatConversion(FormatState*,DigitBuffer*,const be<u16>*);
/* 0275B960 */
s32 formatWideUnits(be<u16>* output,s32 capacity,const be<u16>* text,VariadicState* arguments) {
    gabi::AutoFrame frame{gabi::current_function()};
    if(capacity==0) return -1;
    gabi::FrameLocal<DigitBuffer> buffer(8);
    buffer->mText=output;
    buffer->mCapacity=static_cast<u32>(capacity)-1;
    buffer->mCount=0;
    buffer->mOverflow=0;
    gabi::FrameLocal<FormatState> state(0x18);
    constructFormatState(state.get(),arguments);
    u32 index=0;
    for(;;) {
        u32 character=text[index++];
        if(character==0) {
            s32 count=buffer->mCount;
            buffer->mText[count]=0;
            return count;
        }
        if(character=='%') {
            index+=parseFormatConversion(state.get(),buffer.get(),text+index);
        } else {
            s32 count=buffer->mCount;
            if(count>=static_cast<s32>(buffer->mCapacity)) {
                buffer->mText[count]=0;
                return -1;
            }
            buffer->mText[count]=character;
            buffer->mCount=static_cast<u32>(buffer->mCount)+1;
        }
        if(static_cast<u32>(buffer->mOverflow)) {
            buffer->mText[static_cast<s32>(buffer->mCount)]=0;
            return -1;
        }
    }
}

extern void writeIntegerField(FormatState*,DigitBuffer*,u32,u32,u32);
extern void writeFloatingField(FormatState*,DigitBuffer*,u32,u32);
/* 0275B5CC */
u32 parseFormatConversion(FormatState* state,DigitBuffer* output,const be<u16>* text) {
    gabi::AutoFrame frame{gabi::current_function()};
    state->reset();
    be<u16>* cursor=state->parseFlags(const_cast<be<u16>*>(text));
    cursor=state->parseWidth(cursor);
    cursor=state->parsePrecision(cursor);
    cursor=state->parseLength(cursor);
    u32 conversion=cursor[0];
    switch(conversion) {
    case '%': appendFieldCharacter(output,'%'); break;
    case 'd': case 'i': writeIntegerField(state,output,10,1,0); break;
    case 'u': writeIntegerField(state,output,10,0,0); break;
    case 'o': writeIntegerField(state,output,8,0,0); break;
    case 'x': writeIntegerField(state,output,16,0,0); break;
    case 'X': writeIntegerField(state,output,16,0,1); break;
    case 'p':
        state->mPrecision=8;
        state->mLength=FormatState::Word;
        state->mFlags=static_cast<u32>(state->mFlags)&~static_cast<u32>(FormatState::ZeroPadded);
        writeIntegerField(state,output,16,0,0);
        break;
    case 'c': state->writeCharacter(output); break;
    case 's': state->writeString(output); break;
    case 'f': writeFloatingField(state,output,'f',0); break;
    case 'e': writeFloatingField(state,output,'e',0); break;
    case 'E': writeFloatingField(state,output,'e',1); break;
    case 'g': writeFloatingField(state,output,'g',0); break;
    case 'G': writeFloatingField(state,output,'g',1); break;
    default: return 0;
    }
    u32 difference=gabi::ea(cursor)+2-gabi::ea(text);
    return static_cast<s32>(difference)>>1;
}

struct IntegerFieldStorage {
    be<u8> mNegative;
    u8 mPadding[3];
    DigitBuffer mDigits;
    be<u16> mText[32];
};
/* 0275A094 */
void writeIntegerField(FormatState* state,DigitBuffer* output,u32 radix,u32 isSigned,u32 uppercase) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<IntegerFieldStorage> storage(8);
    storage->mDigits.mCapacity=31;
    storage->mDigits.mText=storage->mText;
    storage->mNegative=0;
    storage->mDigits.mOverflow=0;
    storage->mDigits.mCount=0;
    u32 length=state->mLength;
    if(length==FormatState::Byte) state->writeByteDigits(&storage->mDigits,&storage->mNegative,isSigned,radix,uppercase);
    else if(length==FormatState::Halfword) state->writeHalfwordDigits(&storage->mDigits,&storage->mNegative,isSigned,radix,uppercase);
    else if(length<FormatState::DoubleWord) state->writeWordDigits(&storage->mDigits,&storage->mNegative,isSigned,radix,uppercase);
    else if(length==FormatState::DoubleWord) state->writeDoubleWordDigits(&storage->mDigits,&storage->mNegative,isSigned,radix,uppercase);
    u32 extent=storage->mDigits.mCount;
    u32 zeros=0,before=0,after=0;
    u32 hasPrecision=state->mHasPrecision;
    if(hasPrecision && extent<static_cast<u32>(state->mPrecision)) {
        zeros=static_cast<u32>(state->mPrecision)-extent;
        extent=state->mPrecision;
    }
    u32 flags=state->mFlags;
    if(isSigned) {
        if(static_cast<u32>(storage->mNegative) || (flags&(FormatState::ShowPlus|FormatState::BlankSign))) ++extent;
    } else if(flags&FormatState::Alternate) {
        if(radix==8) ++extent;
        else if(radix==16) extent+=2;
    }
    u32 width=state->mWidth;
    if(extent<width) {
        if(flags&FormatState::LeftAligned) after=width-extent;
        else if((flags&FormatState::ZeroPadded) && !hasPrecision) zeros+=width-extent;
        else before=width-extent;
    }
    for(u32 index=0;index<before;++index) if(!appendFieldCharacter(output,' ')) break;
    if(isSigned) {
        if(static_cast<u32>(storage->mNegative)) appendFieldCharacter(output,'-');
        else if(static_cast<u32>(state->mFlags)&FormatState::ShowPlus) appendFieldCharacter(output,'+');
        else if(static_cast<u32>(state->mFlags)&FormatState::BlankSign) appendFieldCharacter(output,' ');
    } else if(static_cast<u32>(state->mFlags)&FormatState::Alternate) {
        if(radix==8) appendFieldCharacter(output,'0');
        else if(radix==16) {
            appendFieldCharacter(output,'0');
            appendFieldCharacter(output,uppercase?'X':'x');
        }
    }
    for(u32 index=0;index<zeros;++index) if(!appendFieldCharacter(output,'0')) break;
    s32 count=storage->mDigits.mCount;
    for(s32 index=count-1;index>=0;--index) appendFieldCharacter(output,storage->mDigits.mText[index]);
    for(u32 index=0;index<after;++index) if(!appendFieldCharacter(output,' ')) break;
    storage->mDigits.mText[static_cast<s32>(storage->mDigits.mCount)]=0;
}
/* WWHD glue
 ext 0275C724 void sead::format::writeByteTextField(const sead::format::FormatState*,sead::format::DigitBuffer*,const be<u8>*)
 ext 0275C998 void sead::format::writeWideTextField(const sead::format::FormatState*,sead::format::DigitBuffer*,const be<u16>*)
 ext 028F5C88 be<u32>* nextIntegerFormatArgument(sead::format::FormatState*,u32)
 ext 028F1748 s32 formatPlatformBytes(be<u8>*,s32,const char*,sead::format::VariadicState*)
 ext 028F5A1C Pair32 divideFormatDoubleWord(u32,u32,u32,u32)
*/
