/* JAIZel data translation units (WWHD): five sinit-only TUs of Zelda's sound control.
 * Verified against cking.rpx.
 *
 * Each of these TUs holds only tables (JAIZelCharVoiceTable, JAIZelParam, JAIZelScene and two
 * HD-only/unnamed tables); its only code is the 148-byte header __sinit that every JAIZel TU gets
 * (the JAudio header statics: zero a 16-byte bss block, register its global object, load the two
 * rodata floats). From the image: three sit between JAIZelBasic and JAIZelInst, two between
 * JAIZelInst and JAIZelSound. */
#include "bindings.h"


/* 02029CA0 */
// HD: first data translation unit after JAIZelBasic; original filename is unconfirmed.
static void __sinit_JAIZel_data1() {
    sinit_header_statics_rodata(0x101FFC7C, 0x1018D518, 0x101FFC88, 0x10003A50);
}

/* 02029D34 */
// HD: second data translation unit after JAIZelBasic; original filename is unknown.
static void __sinit_JAIZel_data2() {
    sinit_header_statics_rodata(0x101FFC98, 0x1018DB2C, 0x101FFCA4, 0x10003A58);
}

/* 02029DC8 */
// HD: data translation unit immediately before JAIZelInst; original filename is unknown.
static void __sinit_JAIZel_data3() {
    sinit_header_statics_rodata(0x101FFCB4, 0x1018DB50, 0x101FFCC0, 0x10003A60);
}

/* 0202ABA4 */
// HD: JAIZelParam header statics, copying this translation unit's rodata pi pair.
static void __sinit_JAIZelParam_cpp() {
    sinit_header_statics_rodata(0x101FFCEC, 0x1018DBF0, 0x101FFCF8, 0x10003B28);
}

/* 0202AC38 */
// HD: JAIZelScene header statics, copying this translation unit's rodata pi pair.
static void __sinit_JAIZelScene_cpp() {
    sinit_header_statics_rodata(0x101FFD08, 0x1018E2C8, 0x101FFD14, 0x10003EE0);
}


/* WWHD glue (tools/verify/autoglue.py): no translation unit data bindings.
*/
