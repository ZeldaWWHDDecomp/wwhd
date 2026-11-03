#pragma once
#include "JSystem/JStudio/fvb.h"
namespace JStudio { namespace fvb { namespace data {
struct TParse_TParagraph {
    gptr<const u8> raw;
    void getData(TParagraph*) const;
};
WWHD_SIZE(TParse_TParagraph,4); WWHD_OFFSET(TParse_TParagraph,raw,0);
}}}
