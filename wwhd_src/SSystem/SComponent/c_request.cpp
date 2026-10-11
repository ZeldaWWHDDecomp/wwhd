/** Request commands and completion. WWHD, verified unit c_request. */
#include "SSystem/SComponent/c_request.h"
#include "bindings.h"

/* 0201A308 */
BOOL cReq_Is_Done(request_base_class* i_this) {
    if (i_this->mFlags & cReq_flag1) {
        i_this->mFlags = i_this->mFlags & ~cReq_flag1;
        return TRUE;
    }
    return FALSE;
}

/* 0201A330 */
void cReq_Done(request_base_class* i_this) {
    i_this->mFlags = cReq_flag1;
}

/* 0201A33C */
void cReq_Create(request_base_class* i_this, u32 param_1) {
    // HD: the public argument remains a full register; only flag2's six bits are stored.
    i_this->mFlags = (param_1 & cReq_flag2Mask) | cReq_flag0;
}

/* 0201A34C */
void cReq_Command(request_base_class* i_this, u32 param_1) {
    cReq_Create(i_this, param_1);
}

/* 0201A350 */
// HD: header statics and destructor registrations.
static void __sinit_c_request_cpp() {
    sinit_header_statics_rodata(0x101FFAC0, 0x1018D364, 0x101FFACC, 0x100036A0);
}

/* 0201A3E4 */
// HD: adjacent header-only translation unit, probably c_rnd by link order.
static void __sinit_adjacent_request_header_cpp() {
    sinit_header_statics_rodata(0x101FFADC, 0x1018D388, 0x101FFAE8, 0x100036A8);
}

/* WWHD glue (tools/verify/autoglue.py): no local data bindings
*/
