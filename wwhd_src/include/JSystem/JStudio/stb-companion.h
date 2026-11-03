// STB object lifecycle and factory extensions over the canonical timeline prefix.
#pragma once
#include "JSystem/JStudio/jstudio-control.h"
namespace JStudio::stb {
namespace data {
struct ObjectBlock { be<u32> size, type; be<u16> flags, idSize; be<u8> content[1]; };
struct TParse_TBlock_object { gptr<const ObjectBlock> raw; };
WWHD_OFFSET(ObjectBlock, content, 0xC);
WWHD_SIZE(TParse_TBlock_object, 4);
}
struct TObject : jst_timeline::stb::TObject {
    void reset(const void*);
    void do_begin();
    void do_end();
    void do_paragraph(u32, const void*, u32);
    void do_wait(u32);
    void do_data(const void*, u32, const void*, u32);
};
struct TFactory {
    gptr<void> __vtbl;
    TObject* create(const data::TParse_TBlock_object&);
    void destroy(TObject*);
};
struct TParse {
    gptr<void> __vtbl;
    gptr<void> pControl;
    bool parseHeader(const data::TParse_THeader&, u32);
    u32 parseHeader_next(gptr<const u8>*, be<u32>*, u32);
    u32 parseBlock_next(gptr<const u8>*, be<u32>*, u32);
    u32 parseBlock_block(const data::TParse_TBlock&, u32);
    u32 parseBlock_object(const data::TParse_TBlock_object&, u32);
};
WWHD_SIZE(TObject, 0x34);
WWHD_SIZE(TFactory, 4);
WWHD_SIZE(TParse, 8);
WWHD_OFFSET(TParse, pControl, 4);
}
