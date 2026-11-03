// WWHD reflected CRC-32 helper, reconstructed from the game binary.
#include "sead/Checksum.h"

static CrcWords sCrcTable;
static u8 sCrcReady = 0;

/* 0273B200 */
static void initializeCrcTable() {
    const u32 polynomial = 0xEDB88320;
    for (u32 index = 0; index < 256; ++index) {
        u32 value = index;
        for (u32 bit = 0; bit < 8; ++bit) {
            if (value & 1) value = (value >> 1) ^ polynomial;
            else value >>= 1;
        }
        sCrcTable.mValues[index] = value;
    }
    sCrcReady = 1;
}

/* 0273B264 */
static u32 calculateCrc(const be<u8>* bytes, u32 size) {
    if (sCrcReady == 0) initializeCrcTable();
    u32 value = ~0u;
    for (u32 remaining = size; remaining != 0; --remaining) {
        u32 next = *bytes++;
        value = (value >> 8) ^ (u32)sCrcTable.mValues[(value ^ next) & 255];
    }
    return ~value;
}

/* WWHD glue
 data sCrcTable 1049FBA0
 data sCrcReady 101F8AE5
*/
