/* HD: string-table storage and compiler-emitted helpers, reconstructed from the Cafe binary. */
#include "JSystem/JUtility/JUTStringTable.h"
using namespace jut_string_table;
static SystemSlot sHeapSystem;
static StaticState sTableStatic;
static u8 sTableRecord;
static StaticState sStringStatic;
static u8 sStringRecord;
static StringVtable sStringVtable;
static u8 sEmptyString;
/* 027ED08C */
static void __sinit_JUTStringTable_cpp() {
    sTableStatic.words[2]=0;sTableStatic.words[0]=0;sTableStatic.words[3]=0;sTableStatic.words[1]=0;
    stringTableRegisterStatic(&sTableRecord);
}
/* 027ED0B4 */
void JUTStringTable::allocate(s32 count,Heap* heap) {
    if(count!=0) {
        String* strings=mStrings;
        if(strings) {
            for(s32 i=0;i<mCount;++i) {
                String* string=&strings[i];
                gabi::call_ptr<void>(string->mVtable->destroy,string,2);
                strings=mStrings;
            }
            Heap* freeHeap=stringTableFreeHeap(sHeapSystem.system,strings);
            u32 freeMethod=freeHeap->mVtable->mFree;
            gabi::call_ptr<void>(freeMethod,freeHeap,mStrings.get());
            mCount=0;mStrings=nullptr;
        }
        if(count>0) {
            if(!heap) heap=stringTableCurrentHeap(sHeapSystem.system);
            u32 allocateMethod=heap->mVtable->mAlloc;
            String* array=gabi::call_ptr<String*>(allocateMethod,heap,static_cast<u32>(count)*8,4);
            for(s32 i=0;i<count;++i) {
                String* string=&array[i];
                if(gabi::ea(string)!=0) { string->mText=&sEmptyString; string->mVtable=&sStringVtable; }
            }
            if(array) { mStrings=array;mCount=count; }
        }
    }
    mCapacity=count;
}
/* 027ED1F0 */
const be<u8>* JUTStringTable::get(u32 index) {
    if(index>=mCapacity) return nullptr;
    u32 count=mCount;
    String* string=mStrings;
    if(index<count) string+=index;
    gabi::call_ptr<void>(string->mVtable->assureTermination,string);
    return string->mText;
}
/* 027ED268 */
void JUTStringTable::set(u32 index,const gptr<const be<u8>>* text) {
    u32 count=mCount;
    String* string=mStrings;
    const be<u8>* value=*text;
    if(index<count) string+=index;
    string->mText=value;
}
/* 027ED290 */
static void __sinit_JUTStringTable_strings_cpp() {
    sStringStatic.words[2]=0;sStringStatic.words[0]=0;sStringStatic.words[3]=0;sStringStatic.words[1]=0;
    stringTableRegisterStatic(&sStringRecord);
}
/* 027ED2B8 */
static void JUTStringTableString_destroy(String* string,u32 flags) {
    if(string && (flags&1)) stringTableDelete(string);
}
/* 027ED2CC */
static void JUTStringTableString_assureTermination(String*) {}
/* WWHD glue
data sHeapSystem 101F8B4C
data sTableStatic 104A24B0
data sTableRecord 101F97E4
data sStringStatic 104A24C0
data sStringRecord 101F97F0
data sStringVtable 1016DDA0
data sEmptyString 10000160
ext 02755FEC jut_string_table::Heap* stringTableFreeHeap(jut_string_table::HeapSystem*,jut_string_table::String*)
ext 02756140 jut_string_table::Heap* stringTableCurrentHeap(jut_string_table::HeapSystem*)
ext 028F026C void stringTableRegisterStatic(void*)
ext 0273AF40 void stringTableDelete(void*)
*/
