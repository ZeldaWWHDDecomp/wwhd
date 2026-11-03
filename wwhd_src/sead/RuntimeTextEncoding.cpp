/* Shift-JIS (byte text) <-> UTF-16 (wide text) conversion, recovered from local instructions and read-only tables. */
#include "sead/RuntimeFormatSupport.h"
// Views of the game's conversion tables (read from the image, never copied into the source).
// 50 rows of 188 trail columns: lead bytes 0x81-0x9F, 0xE0-0xEF and 0xFA-0xFC. The code picks the 0xFA
// rows' base for every lead byte above 0xE0, so 0xE1-0xEF read rows 22-36 (kept as the binary does).
struct DoubleByteDecodeTable { be<u16> mUnits[50 * 188]; };
// Records grouped by the UTF-16 unit's low byte and sorted by its high byte.
struct EncodeRecord { be<u8> mHighByte; be<u8> mLeadByte; be<u8> mTrailByte; };
struct EncodeRecordTable { EncodeRecord mRecords[7416]; };
// First record index per low byte; entry 256 ends the last group.
struct EncodeGroupTable { be<u16> mFirst[257]; };
WWHD_SIZE(EncodeRecord, 3);
static DoubleByteDecodeTable sDecodeMap;
static EncodeRecordTable sEncodeRecords;
static EncodeGroupTable sEncodeRanges;
/* 0275BAA0 */
static u32 decodeTextBytes(be<u16>* output,u32 capacity,const be<u8>* input,s32 length) {
    gabi::AutoFrame frame{gabi::current_function()};
    if(capacity==0 || length<-1) return 0;
    u32 count=0,index=0;
    while(length==-1 || static_cast<s32>(index)<length) {
        u32 character=input[index];
        if(character==0) break;
        ++index;
        if(count>=capacity-1) { count=capacity-1; break; }
        if(character>0x80) {
            if(character==0xA0) character=0xF8F0;
            else if(character-0xA1<0x3F) character+=0xFEC0;
            else if(character>=0xFD) character+=0xF7F4;
            else {
                if(length!=-1 && static_cast<s32>(index)>=length) break;
                u32 second=input[index++];
                if(second<0x40 || second==0x7F || second>0xFC) break;
                u32 column=second-(second>=0x80?0x41:0x40);
                if(character-0xF0<10) character=character*188+column+0x2FC0;
                else {
                    u32 first=character<=0x9F?0x81:(character<=0xE0?0xC1:0xCB);
                    character=sDecodeMap.mUnits[(character-first)*188+column];
                    if(character==0) break;
                }
            }
        }
        output[count++]=character;
    }
    output[count]=0;
    return count;
}
struct EncodedPair { be<u8> mBytes[2]; };
/* 0275BC8C */
static u32 encodeTextUnits(be<u8>* output,u32 capacity,const be<u16>* input,s32 length) {
    gabi::AutoFrame frame{gabi::current_function()};
    if(capacity==0 || length<-1) return 0;
    gabi::FrameLocal<EncodedPair> pair(8);
    u32 count=0,index=0;
    while(length==-1 || static_cast<s32>(index)<length) {
        u32 character=input[index++];
        if(character==0) break;
        if(count>=capacity-1) { count=capacity-1; break; }
        pair->mBytes[1]=0;
        pair->mBytes[0]=0;
        u32 first=0;
        if(character<0x80) first=character;
        else if(character-0xFF61<0x3F) first=(character-0xFEC0)&255;
        else if(character-0xD800>=0x2100) {
            s32 low=sEncodeRanges.mFirst[character&255];
            s32 high=static_cast<s32>(sEncodeRanges.mFirst[(character&255)+1])-1;
            while(low<=high) {
                s32 middle=(low+high)/2;
                const EncodeRecord& record=sEncodeRecords.mRecords[middle];
                u32 upper=record.mHighByte;
                if((character>>8)<upper) high=middle-1;
                else if((character>>8)>upper) low=middle+1;
                else {
                    pair->mBytes[1]=record.mTrailByte;
                    pair->mBytes[0]=record.mLeadByte;
                    first=pair->mBytes[0];
                    break;
                }
            }
        } else if(character-0xE000<0x758) {
            u32 offset=character-0xE000;
            u32 column=offset%188;
            first=(offset/188+0xF0)&255;
            pair->mBytes[1]=column+(column>=0x3F?0x41:0x40);
        } else if(character==0xF8F0) first=0xA0;
        else if(character-0xF8F1<3) first=(character-0xF7F4)&255;
        if(first==0) break;
        u32 second=pair->mBytes[1];
        if(second==0) output[count++]=first;
        else {
            if(count>=capacity-2) { count=capacity-2; break; }
            output[count++]=first;
            output[count++]=second;
        }
    }
    output[count]=0;
    return count;
}
/* WWHD glue
 data sDecodeMap 10147668
 data sEncodeRecords 1014BFD8
 data sEncodeRanges 101516C0
*/
