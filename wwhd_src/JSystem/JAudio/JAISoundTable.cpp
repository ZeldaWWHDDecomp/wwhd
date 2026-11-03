/* Sound table lookup, following CC0 tww and Cafe null-table handling. */
#include "JSystem/JAudio/JAIBasic.h"
#include "JSystem/JAudio/JAISoundTable.h"
using namespace JAInter::SoundTable;
static u8 mCategotyMax;
static u8 mVersion;
static u32 mDatasize;
struct SoundTableEntry { be<u16> count; be<u16> offset; };
struct SoundTableHeader { be<u8> formats[3]; be<u8> version; be<u16> reserved; SoundTableEntry entries[18]; };
WWHD_OFFSET(SoundTableHeader,entries,6);
WWHD_SIZE(SoundTableEntry,4);
using TableByte=be<u8>;
static TableByte* mAddress;
using SoundCount=be<u16>;
using SoundCategory=gptr<SoundInfo>;
static SoundCount* mSoundMax;
static SoundCategory* mPointerCategory;
static AudioHeaderStatics staticState;
static AudioDestructorRecord staticRecord;
namespace JAInter { namespace SoundTable {
/* 0280DCE0 */
void init(be<u8>* address,u32 size) {
    mDatasize=size;mAddress=address;
    SoundTableHeader* header=reinterpret_cast<SoundTableHeader*>(address);
    mVersion=header->version;
    JKRHeap* heap=audioCurrentHeap;
    if(!heap) { soundTableAssert("JAIBasic.h",169,"msCurrentHeap");heap=audioCurrentHeap; }
    mSoundMax=static_cast<SoundCount*>(soundTableAllocate(36,heap,4));
    heap=audioCurrentHeap;
    if(!heap) { soundTableAssert("JAIBasic.h",169,"msCurrentHeap");heap=audioCurrentHeap; }
    mPointerCategory=static_cast<SoundCategory*>(soundTableAllocate(72,heap,4));
    for(u32 i=0;i<18;++i) {
        header=reinterpret_cast<SoundTableHeader*>(static_cast<TableByte*>(mAddress));
        mSoundMax[i]=header->entries[i].count;
        mPointerCategory[i]=reinterpret_cast<SoundInfo*>(static_cast<TableByte*>(mAddress)+0x50)+u32(header->entries[i].offset);
        if(i<16 && mSoundMax[i]!=0) mCategotyMax=i+1;
    }
}
/* 0280E070 */
be<u8>* getAddress() { return mAddress; }
/* 0280DE34 */
SoundInfo* getInfoPointer(u32 soundID) {
    if(!mAddress) {
        soundTableAssert("JAISoundTable.cpp",56,"mAddress");
        if(!mAddress) return nullptr;
    }
    u32 category=0;
    switch(soundID&0xC0000000u) {
    case 0:
        category=(soundID>>12)&0xFF;
        if(category>=soundTableCategoryMax() && category>=soundTableCategoryMax())
            soundTableAssert("JAISoundTable.cpp",72,"(_category<JAIGlobalParameter::getParamSeCategoryMax())");
        break;
    case 0x80000000u:category=16;break;
    case 0xC0000000u:category=17;break;
    default:soundTableAssert("JAISoundTable.cpp",81,"(0)");break;
    }
    u32 index=soundID&0x3FF;
    if(mAddress && index<mSoundMax[category]) return mPointerCategory[category]+index;
    return nullptr;
}
/* 0280E03C */
u8 getCategotyMax() { return mCategotyMax; }
/* 0280DF98 */
u8 getInfoFormat(u32 soundID) {
    be<u8>* address=mAddress;
    if(!address) return 0;
    switch(soundID&0xC0000000u) {
    case 0:return address[0];
    case 0x80000000u:return address[1];
    case 0xC0000000u:return address[2];
    default:soundTableAssert("JAISoundTable.cpp",119,"(0)");return 0;
    }
}
}}
/* 0280E048 */
static void __sinit_JAISoundTable_cpp() {
    staticState.mWords[2]=0;staticState.mWords[0]=0;
    staticState.mWords[3]=0;staticState.mWords[1]=0;
    soundTableRegister(&staticRecord);
}
/* WWHD glue
data mCategotyMax 101F9CD4
data mVersion 104B50D0
data mDatasize 104B50C8
str 1016FCB4 "JAIBasic.h"
str 1016FCA4 "msCurrentHeap"
ext 0273B0D4 void* soundTableAllocate(u32,JKRHeap*,s32)
data mAddress 101F9CD0
data mSoundMax 104B50C4
data mPointerCategory 104B50CC
data staticState 104B50B4
data staticRecord 101F9CC4
str 1016FCFC in 0280DE34
str 1016FD10 in 0280DE34
str 1016FCC0 in 0280DE34
str 1016FCC4 in 0280DE34
str 1016FD20 in 0280DF98
str 1016FD1C in 0280DF98
ext 0280468C u32 soundTableCategoryMax()
ext 0273AA24 void soundTableAssert(const char*,u32,const char*)
ext 028F026C void soundTableRegister(void*)
*/
