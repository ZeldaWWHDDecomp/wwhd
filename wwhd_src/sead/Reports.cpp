// WWHD assertion formatting and report forwarding, reconstructed from the binary.
#include "sead/Reports.h"

static u8 sWarningsEnabled;

static void saveReportRegisters(ReportSave* save, const u32* registers, bool floats) {
    for (u32 index = 0; index < 8; ++index) save->mIntegers[index] = registers[index];
    if (!floats) return;
    for (u32 index = 0; index < 8; ++index) {
        u64 bits;
        memcpy(&bits,&gabi::cpu->f[index + 1].ps0,8);
        save->mFloatWords[index * 2] = bits >> 32;
        save->mFloatWords[index * 2 + 1] = bits;
    }
}

static void initializeReportArguments(ReportSave* save,u32 entry,u8 first) {
    save->mArguments.mSavedRegisters = gabi::ea(save->mIntegers);
    save->mArguments.mOverflow = entry + 8;
    save->mArguments.mFloatIndex = 0;
    save->mArguments.mIntegerIndex = first;
}

/* 0273B158 */
void sendReport(const char* format, ReportArguments* arguments) {
    sendOsReport(format,arguments);
}

/* 0273B15C */
static void printReport(const char* format,u32 arg4,u32 arg5,u32 arg6,u32 arg7,u32 arg8,u32 arg9,u32 arg10) {
    u32 entry = gabi::cpu->r[1];
    bool floats = gabi::cpu->cr[6] != 0;
    gabi::NativeFrame<120> frame;
    auto* save = gabi::at<ReportSave>(frame + 8);
    const u32 registers[8] = {gabi::ea(format),arg4,arg5,arg6,arg7,arg8,arg9,arg10};
    saveReportRegisters(save,registers,floats);
    initializeReportArguments(save,entry,1);
    sendReport(format,&save->mArguments);
}

/* 0273AA38 */
static void printWarning(const char* file,u32 line,const char* format,u32 arg6,u32 arg7,u32 arg8,u32 arg9,u32 arg10) {
    u32 entry = gabi::cpu->r[1];
    bool floats = gabi::cpu->cr[6] != 0;
    gabi::NativeFrame<128> frame;
    auto* save = gabi::at<ReportSave>(frame + 8);
    const u32 registers[8] = {gabi::ea(file),line,gabi::ea(format),arg6,arg7,arg8,arg9,arg10};
    saveReportRegisters(save,registers,floats);
    if (sWarningsEnabled == 0) return;
    gabi::cpu->cr[6] = 0;
    reportWarningPrefix("%s(%d) Warning: ",file,line);
    initializeReportArguments(save,entry,3);
    sendReport(format,&save->mArguments);
}

/* 0273AAFC */
void stopAfterAssertion() {
    // the debugger break (twu r31,r31 at 0273AB18) when a debugger is attached: compared as a trap event
    if (debuggerIsPresent() != 0) { gabi::trap(0x0273AB18); return; }
    gabi::cpu->cr[6] = 0;
    assertionPanic("D:/home/Cafe/cking/DevEnv/sead/engine/library/modules/src/basis/cafe/seadAssertCafe.cpp",23,"");
}

/* 0273B1E4 */
void writeReportBytes(const char* message,s32 length) {
    if (length <= 0) return;
    gabi::cpu->cr[6] = 0;
    writeOsReport("%s",message);
}

/* 0273AB58 */
static void haltWithMessage(const char* file,u32 line,const char* format,u32 arg6,u32 arg7,u32 arg8,u32 arg9,u32 arg10) {
    u32 entry = gabi::cpu->r[1];
    bool floats = gabi::cpu->cr[6] != 0;
    gabi::NativeFrame<520> frame;
    auto* save = gabi::at<AssertionSave>(frame + 8);
    const u32 registers[8] = {gabi::ea(file),line,gabi::ea(format),arg6,arg7,arg8,arg9,arg10};
    saveReportRegisters(save,registers,floats);
    be<u8>* buffer = save->mMessage;
    gabi::cpu->cr[6] = 0;
    s32 length = formatAssertion(buffer,384,
        "\n//======================= PROGRAM HALT =======================//\nSource File: %s\nLine Number: %d\nDescription: ",
        file,line);
    if (length >= 0) {
        initializeReportArguments(save,entry,3);
        s32 result = formatAssertionArguments(buffer + length,384u - length,format,&save->mArguments);
        if (result >= 0) {
            length = (u32)length + result;
            gabi::cpu->cr[6] = 0;
            result = formatAssertionEnd(buffer + length,384u - length,
                "\n//============================================================//");
            if (result > 0) {
                length = (u32)length + result;
                if (length >= 382) length = 383;
                else {
                    buffer[length] = '\n';
                    buffer[(u32)length + 1] = 0;
                    length = (u32)length + 1;
                }
            }
        } else length = -1;
    }
    buffer[383] = 0;
    if (length < 0) {
        length = 0;
        while (buffer[length] != 0) ++length;
    }
    const char* message = reinterpret_cast<const char*>(buffer);
    writeReportBytes(message,length);
    notifyReportListeners(message);
    stopAfterAssertion();
}

/* 0273AA24 */
static void haltWithText(const char* file,u32 line,const char* message) {
    gabi::cpu->cr[6] = 0;
    reportAssertion(file,line,"%s",message);
}

/* WWHD glue
 data sWarningsEnabled 101F8AE4
 str 10142FA4 "%s" in 0273AA24
 str 10142FA8 "%s(%d) Warning: "
 str 10142FC0 "D:/home/Cafe/cking/DevEnv/sead/engine/library/modules/src/basis/cafe/seadAssertCafe.cpp"
 str 10142FBC ""
 str 10143018 "\n//======================= PROGRAM HALT =======================//\nSource File: %s\nLine Number: %d\nDescription: "
 str 10143088 "\n//============================================================//"
 str 101430CC "%s" in 0273B1E4
 ext C000A1D8 void sendOsReport(const char*, ReportArguments*)
 ext 0273B15C void reportWarningPrefix(const char*, const char*, u32)
 ext C0009DD0 u32 debuggerIsPresent()
 ext C0009EB0 void assertionPanic(const char*, u32, const char*)
 ext C0009EE8 void writeOsReport(const char*, const char*)
 ext 028F17A8 s32 formatAssertion(be<u8>*,u32,const char*,const char*,u32)
 ext 028F17A8 s32 formatAssertionEnd(be<u8>*,u32,const char*)
 ext 028F1748 s32 formatAssertionArguments(be<u8>*,u32,const char*,ReportArguments*)
 ext 0273EE18 void notifyReportListeners(const char*)
 ext 0273AB58 void reportAssertion(const char*,u32,const char*,const char*)
*/
