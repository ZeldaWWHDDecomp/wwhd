/* GameCube J2DPicture API, bound to HD. */
#pragma once
#include "gc_api/JSystem/J2DPane.h"

class J2DPicture : public J2DPane {
public:
    /* 0xCC */ u8 _CC[0x124 - 0xCC];
};
WWHD_SIZE(J2DPicture, 0x124);

// 027EF590 J2DPicture::J2DPicture(J2DPane*, JSURandomInputStream*) [v d_file_error]
inline void J2DPicture__ct(J2DPicture* picture, J2DPane* parent, JSURandomInputStream* stream) {
    gabi::call<void>(0x027EF590, picture, parent, stream);
}
// 027F021C J2DPicture::~J2DPicture() (GHS deleting flags) [v d_file_error]
inline void J2DPicture__dt(J2DPicture* picture, u32 flags) {
    gabi::call<void>(0x027F021C, picture, flags);
}
