/* GameCube J2DScreen API, bound to HD. */
#pragma once
#include "gc_api/JSystem/J2DPane.h"

class J2DScreen : public J2DPane {
public:
    // 027F0898 [v d_file_error]
    J2DPane* createPane(const J2DScrnBlockHeader* header, JSURandomInputStream* stream, J2DPane* parent) {
        return gabi::call<J2DPane*>(0x027F0898, this, header, stream, parent);
    }
};
