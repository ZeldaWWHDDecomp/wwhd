/* Based on CC0 tww JASCalc.cpp memory helpers. HD omits debug assertions. */
#include "JSystem/JAudio/JASCalc.h"
static CalcHeaderStatics headerStatics;
static CalcDestructorRecord destructorRecord;
namespace JASystem { namespace Calc {
/* 02843CF4 */
void bcopyfast(const be<u32>* source, be<u32>* destination, u32 size) {
    for (u32 blocks = size / 16; blocks; --blocks) {
        u32 second = source[1];
        u32 third = source[2];
        u32 first = source[0];
        u32 fourth = source[3];
        destination[0] = first;
        destination[1] = second;
        destination[2] = third;
        destination[3] = fourth;
        source += 4;
        destination += 4;
    }
}
/* 02843D30 */
void bcopy(const void* source, void* destination, u32 size) {
    const be<u8>* bytes = (const be<u8>*)source;
    be<u8>* output = (be<u8>*)destination;
    u32 sourceAlignment = (uintptr_t)source & 3;
    u32 destinationAlignment = (uintptr_t)destination & 3;
    if (sourceAlignment == destinationAlignment && !(size & 15)) {
        bcopyfast((const be<u32>*)source, (be<u32>*)destination, size);
        return;
    }
    if (sourceAlignment == destinationAlignment && size >= 16) {
        if (sourceAlignment) {
            u32 prefix = 4 - sourceAlignment;
            size -= prefix;
            for (u32 i = 0; i < prefix; ++i) *output++ = (u8)*bytes++;
        }
        const be<u32>* words = (const be<u32>*)bytes;
        be<u32>* wordOutput = (be<u32>*)output;
        while (size >= 4) {
            u32 value = *words++;
            *wordOutput++ = value;
            size -= 4;
        }
        bytes = (const be<u8>*)words;
        output = (be<u8>*)wordOutput;
    }
    while (size--) *output++ = (u8)*bytes++;
}
/* 02843E08 */
void bzerofast(void* destination, u32 size) {
    be<u32>* output = (be<u32>*)destination;
    for (u32 blocks = size / 16; blocks; --blocks) {
        output[2] = 0;
        output[1] = 0;
        output[0] = 0;
        output[3] = 0;
        output += 4;
    }
}
/* 02843E34 */
void bzero(void* destination, u32 size) {
    if (!(size & 31) && !((uintptr_t)destination & 31)) {
        clearAudioCacheRange(destination, size);
        return;
    }
    u32 alignment = (uintptr_t)destination & 3;
    if (!(size & 15) && !alignment) {
        bzerofast(destination, size);
        return;
    }
    be<u8>* output = (be<u8>*)destination;
    if (size >= 16) {
        if (alignment) {
            u32 prefix = 4 - alignment;
            size -= prefix;
            for (u32 i = 0; i < prefix; ++i) *output++ = 0;
        }
        be<u32>* words = (be<u32>*)output;
        while (size >= 4) { *words++ = 0; size -= 4; }
        output = (be<u8>*)words;
    }
    while (size--) *output++ = 0;
}
}}
/* 02843F00 */
void __sinit_JASCalc_cpp() {
    headerStatics.mWords[2] = 0;
    headerStatics.mWords[0] = 0;
    headerStatics.mWords[3] = 0;
    headerStatics.mWords[1] = 0;
    registerCalcStatics(&destructorRecord);
}
/* WWHD glue
data headerStatics 104B65A4
data destructorRecord 101FC1FC
ext 028F026C void registerCalcStatics(CalcDestructorRecord*)
ext C00088C8 void clearAudioCacheRange(void*, u32)
*/
