#include "wwhd.h"
#include "gabi.h"

s32 cReq_Is_Done(void* request) {
    WWHD_FUNC(0x0201A308, s32, request);
    u32 a = gabi::ea(request);
    if (gabi::load<u8>(a) & 0x40) {
        gabi::store<u8>(a, gabi::load<u8>(a) & 0xBF);
        return 1;
    }
    return 0;
}
VERIFY(0x0201A308, cReq_Is_Done);
void cReq_Done(void* request) {
    WWHD_FUNC(0x0201A330, void, request);
    gabi::store<u8>(gabi::ea(request), 0x40);
}
VERIFY(0x0201A330, cReq_Done);
void cReq_Create(void* request, u32 command) {
    WWHD_FUNC(0x0201A33C, void, request, command);
    gabi::store<u8>(gabi::ea(request), (command & 0x3F) | 0x80);
}
VERIFY(0x0201A33C, cReq_Create);
void cReq_Command(void* request, u32 command) {
    WWHD_FUNC(0x0201A34C, void, request, command);
    cReq_Create(request, command);
}
VERIFY(0x0201A34C, cReq_Command);
void c_request_static_init() {
    WWHD_FUNC(0x0201A350, void);
    gabi::store<u32>(0x101FFAD4, 0);
    gabi::store<u32>(0x101FFACC, 0);
    gabi::store<u32>(0x101FFAD8, 0);
    gabi::store<u32>(0x101FFAD0, 0);
    gabi::call<void>(0x028F026C, gabi::at<void>(0x1018D364));
    f32 negativePi = gabi::load<f32>(0x100036A0);
    f32 positivePi = gabi::load<f32>(0x100036A4);
    gabi::store<f32>(0x101FFAC0, negativePi);
    gabi::store<f32>(0x101FFAC4, positivePi);
    gabi::call<void>(0x028ED6F8, gabi::at<void>(0x101FFAC8));
    gabi::call<void>(0x028F026C, gabi::at<void>(0x1018D370));
    gabi::call<void>(0x028EAB2C, gabi::at<void>(0x101FFAC9));
    gabi::call<void>(0x028F026C, gabi::at<void>(0x1018D37C));
}
VERIFY(0x0201A350, c_request_static_init);

/* ---- hosted here: the static initializer(s) of a separate header-static-only TU linked between c_request and c_sxyz (probably c_rnd by link order).
 * Each only initializes the shared header statics (zeroed 16-byte object, -pi/pi pair, two
 * registered global objects); no code of their own. ---- */
void hd_static_init_0201A3E4() {
 WWHD_FUNC(0x0201A3E4,void);
 gabi::store<u32>(0x101FFAF0,0);gabi::store<u32>(0x101FFAE8,0);gabi::store<u32>(0x101FFAF4,0);gabi::store<u32>(0x101FFAEC,0);
 gabi::call<void>(0x028F026C,gabi::at<void>(0x1018D388));
 f32 negativePi=gabi::load<f32>(0x100036A8),positivePi=gabi::load<f32>(0x100036AC);
 gabi::store<f32>(0x101FFADC,negativePi);gabi::store<f32>(0x101FFAE0,positivePi);
 gabi::call<void>(0x028ED6F8,gabi::at<void>(0x101FFAE4));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x1018D394));
 gabi::call<void>(0x028EAB2C,gabi::at<void>(0x101FFAE5));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x1018D3A0));
}
VERIFY(0x0201A3E4,hd_static_init_0201A3E4);
