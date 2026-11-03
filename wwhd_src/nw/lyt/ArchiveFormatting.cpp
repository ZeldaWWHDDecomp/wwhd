/* Compiler-generated GHS variadic argument saves reconstructed from WWHD.
 * Raw CPU access is confined to ABI state: CR1's floating-argument marker and FPR bits. */
#include "nw/lyt/ArchiveFormatting.h"
#include "nw/lyt/ResourceHelpers.h"
struct FormatGeneralSaves { be<u32> values[8]; };
struct FormatFloatSaves { OSReportSavedFloat values[8]; };

/* 028737BC */
s32 layoutFormatBounded(be<u8>* destination, u32 capacity, u32 limit, const be<u8>* format, u32 a1, u32 a2, u32 a3, u32 a4) {
    u32 entry = gabi::cpu->r[1];
    bool hasFloats = gabi::cpu->cr[6] != 0;
    auto frame = gabi::AutoFrame(gabi::current_function());
    gabi::FrameLocal<FormatGeneralSaves> general(0x18);
    const u32 values[8] = {gabi::ea(destination), capacity, limit, gabi::ea(format), a1, a2, a3, a4};
    for (u32 index = 0; index < 8; ++index) general->values[index] = values[index];
    gabi::FrameLocal<FormatFloatSaves> floating(0x38);
    if (hasFloats) {
        for (u32 index = 0; index < 8; ++index) {
            u64 bits;
            memcpy(&bits, &gabi::cpu->f[index + 1].ps0, sizeof(bits));
            floating->values[index].high = u32(bits >> 32);
            floating->values[index].low = u32(bits);
        }
    }
    gabi::FrameLocal<OSReportVaList> arguments(8);
    arguments->overflow_arg_area = entry + 8;
    arguments->gpr = 4;
    u32 bound = limit + 1;
    if (capacity < bound) bound = capacity;
    arguments->reg_save_area = gabi::ea(general.get());
    arguments->fpr = 0;
    s32 result = archiveFormatV(destination, bound, format, arguments.get());
    *layoutAtOffset<be<u8>>(destination, capacity - 1) = 0;
    return result;
}

/* WWHD glue
ext 028F1748 s32 archiveFormatV(be<u8>*, u32, const be<u8>*, OSReportVaList*)
*/
