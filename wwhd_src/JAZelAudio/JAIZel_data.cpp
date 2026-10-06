/* JAIZel data translation units (WWHD): five sinit-only TUs of Zelda's sound control.
 * Verified against cking.rpx.
 *
 * Each of these TUs holds only tables (JAIZelCharVoiceTable, JAIZelParam, JAIZelScene and two
 * HD-only/unnamed tables); its only code is the 148-byte header __sinit that every JAIZel TU gets
 * (the JAudio header statics: zero a 16-byte bss block, register its global object, load the two
 * rodata floats). From the image: three sit between JAIZelBasic and JAIZelInst, two between
 * JAIZelInst and JAIZelSound. */
#include "bindings.h"

namespace JAIZel_data_cpp {
#include "jaizel_local.h"

/* 02029CA0 __sinit (data TU 1, after JAIZelBasic; probably JAIZelCharVoiceTable) */
void __sinit_JAIZel_data1() {
    WWHD_FUNC(0x02029CA0, void);
    header_sinit(0x101FFC7C, 0x1018D518, 0x10003A50);
}
VERIFY(0x02029CA0, __sinit_JAIZel_data1);

/* 02029D34 __sinit (data TU 2) */
void __sinit_JAIZel_data2() {
    WWHD_FUNC(0x02029D34, void);
    header_sinit(0x101FFC98, 0x1018DB2C, 0x10003A58);
}
VERIFY(0x02029D34, __sinit_JAIZel_data2);

/* 02029DC8 __sinit (data TU 3, before JAIZelInst) */
void __sinit_JAIZel_data3() {
    WWHD_FUNC(0x02029DC8, void);
    header_sinit(0x101FFCB4, 0x1018DB50, 0x10003A60);
}
VERIFY(0x02029DC8, __sinit_JAIZel_data3);

/* 0202ABA4 __sinit (data TU after JAIZelInst; JAIZelParam) */
void __sinit_JAIZelParam_cpp() {
    WWHD_FUNC(0x0202ABA4, void);
    header_sinit(0x101FFCEC, 0x1018DBF0, 0x10003B28);
}
VERIFY(0x0202ABA4, __sinit_JAIZelParam_cpp);

/* 0202AC38 __sinit (JAIZelScene) */
void __sinit_JAIZelScene_cpp() {
    WWHD_FUNC(0x0202AC38, void);
    header_sinit(0x101FFD08, 0x1018E2C8, 0x10003EE0);
}
VERIFY(0x0202AC38, __sinit_JAIZelScene_cpp);

} // namespace JAIZel_data_cpp
