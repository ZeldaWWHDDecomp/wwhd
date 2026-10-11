/** OS reporting: force-enable flags, error and warning wrappers.
 * Verified against WWHD (m_Do_printf). */
#include "m_Do/m_Do_printf.h"

static u8 __OSReport_enable;
static u8 __OSReport_Error_disable;
static u8 __OSReport_Warning_disable;
static u32 print_errors;
static u32 print_warings;
static OSReportHeaderStatics l_headerStatics;
static OSReportPiPair l_piPair;

/* 025F26F0 */
void OSReportForceEnableOn() {
    __OSReport_enable = true;
}

/* 025F2700 */
void OSReportForceEnableOff() {
    __OSReport_enable = false;
}

// HD: preserve the GHS variadic save area, including untouched padding and FPR bits.
static void saveReportArguments(OSReportFrame* frame, const u32* regs, bool floats) {
    for (u32 i = 0; i < 8; ++i) {
        frame->gpr[i] = regs[i];
    }
    if (floats) {
        for (u32 i = 0; i < 8; ++i) {
            u64 bits;
            memcpy(&bits, &gabi::cpu->f[i + 1].ps0, 8);
            frame->fpr[i].high = bits >> 32;
            frame->fpr[i].low = bits;
        }
    }
}

static void report(OSReportFrame* frame, u32 entry, u32 fmt, be<u32>* count, be<u8>* disabled, bool warning) {
    ++*count;
    if (*disabled) {
        return;
    }
    frame->args.overflow_arg_area = entry + 8;
    frame->args.reg_save_area = gabi::ea(frame->gpr);
    frame->args.gpr = 1;
    frame->args.fpr = 0;
    OSReportForceEnableOn();
    if (warning) {
        gabi::cpu->cr[6] = 0; // HD: variadic OSReport call has no floating arguments.
        OSReportColor("\x1B[43;30m");
    } else {
        printfColor("\x1B[41;37m");
    }
    OSVReport(fmt, &frame->args);
    if (warning) {
        gabi::cpu->cr[6] = 0;
        OSReportColor("\x1B[m");
    } else {
        printfColor("\x1B[m");
    }
    OSReportForceEnableOff();
}

/* 025F2710 */
// HD: explicit argument registers retain the verifier's GHS variadic signature.
static void OSReport_Error(u32 fmt, u32 a4, u32 a5, u32 a6, u32 a7, u32 a8, u32 a9, u32 a10) {
    u32 entry = gabi::cpu->r[1];
    bool floats = gabi::cpu->cr[6] != 0;
    gabi::Local<OSReportFrame> frame;
    const u32 regs[8] = {fmt, a4, a5, a6, a7, a8, a9, a10};
    saveReportArguments(frame.get(), regs, floats);
    report(frame.get(), entry, fmt, &print_errors, &__OSReport_Error_disable, false);
}

/* 025F27E8 */
static void OSReport_Warning(u32 fmt, u32 a4, u32 a5, u32 a6, u32 a7, u32 a8, u32 a9, u32 a10) {
    u32 entry = gabi::cpu->r[1];
    bool floats = gabi::cpu->cr[6] != 0;
    gabi::Local<OSReportFrame> frame;
    const u32 regs[8] = {fmt, a4, a5, a6, a7, a8, a9, a10};
    saveReportArguments(frame.get(), regs, floats);
    report(frame.get(), entry, fmt, &print_warings, &__OSReport_Warning_disable, true);
}

/* ---------------- HD: compiler-generated code of this translation unit ---------------- */

/* 025F28C8 */
static void __sinit_m_Do_printf_cpp() {
    l_headerStatics.zero[2] = 0;
    l_headerStatics.zero[0] = 0;
    l_headerStatics.zero[3] = 0;
    l_headerStatics.zero[1] = 0;
    __register_global_object(0x101F4924);
    l_headerStatics.negativePi = l_piPair.negativePi;
    l_headerStatics.positivePi = l_piPair.positivePi;
    OSReportFirstHeaderObject_ct(&l_headerStatics.firstObject);
    __register_global_object(0x101F4930);
    OSReportSecondHeaderObject_ct(&l_headerStatics.secondObject);
    __register_global_object(0x101F493C);
}

/* WWHD glue (tools/verify/autoglue.py): this translation unit's data
str 100590A0 "\x1B[41;37m"
str 1005909C "\x1B[m" in 025F2710
str 100590B0 "\x1B[43;30m"
str 100590AC "\x1B[m" in 025F27E8
data __OSReport_enable 1048D524
data __OSReport_Error_disable 1048D539
data __OSReport_Warning_disable 1048D53A
data print_errors 101F4948
data print_warings 101F494C
data l_headerStatics 1048D51C
data l_piPair 100590C4
ext 028ED6F8 void OSReportFirstHeaderObject_ct(u8*)
ext 028EAB2C void OSReportSecondHeaderObject_ct(u8*)
ext C000A1D8 void OSVReport(u32 fmt, OSReportVaList* args)
ext 028F24C4 void printfColor(const char* text)
ext C0009EE8 void OSReportColor(const char* text)
*/
