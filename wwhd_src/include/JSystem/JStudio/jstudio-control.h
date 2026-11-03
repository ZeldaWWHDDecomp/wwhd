// Full JStudio control extensions over the already-verified timeline prefix.
#pragma once
#include "JSystem/JStudio/JST_timeline.h"
#include "JSystem/JStudio/fvb.h"
#include "JSystem/JStudio/jstudio-math.h"
namespace JStudio {
namespace stb { namespace data {
struct TBlock { be<u32> size, type; };
struct TParse_TBlock { gptr<const TBlock> raw; };
struct THeader { u8 prefix[0x10]; be<u8> target[8]; u8 reserved[6]; be<u16> target_version; };
struct TParse_THeader { gptr<const THeader> raw; };
}}
struct Origin { union { Vec value; be<u32> words[3]; }; };
struct TControl : jst_timeline::TControl {
    fvb::TControl fvb_Control;
    be<u8> mTransformOnSet, mTransformOnGet;
    u8 padding[2];
    Origin mTransformOnSet_Origin, mTransformOnGet_Origin;
    be<f32> mTransformOnSet_RotationY, mTransformOnGet_RotationY;
    Mtx34 mTransformOnSet_Matrix, mTransformOnGet_Matrix;
    void transformOnSet_setOrigin(const Vec&, f32);
    void transformOnGet_setOrigin(const Vec&, f32);
};
struct CreateVTable { u8 prefix[0x14]; gfn create; };
struct TCreateObject { JGadget::TLinkListNode mNode; gptr<CreateVTable> __vtbl; };
struct TFactory {
    gptr<void> __vtbl;
    JGadget::TNodeLinkList mList;
    fvb::TFactory fvb_Factory;
    void appendCreateObject(TCreateObject*);
    jst_timeline::TObject* create(const stb::data::TParse_TBlock&);
};
struct TParse {
    gptr<void> __vtbl;
    gptr<TControl> pControl;
    fvb::TParse fvb_Parse;
    bool parseHeader(const stb::data::TParse_THeader&, u32);
    u32 parseBlock_block_fvb_(const stb::data::TParse_TBlock&, u32);
    u32 parseBlock_block(const stb::data::TParse_TBlock&, u32);
};
WWHD_SIZE(TControl,0xF8); WWHD_OFFSET(TControl,fvb_Control,0x60); WWHD_OFFSET(TControl,mTransformOnSet,0x74); WWHD_OFFSET(TControl,mTransformOnGet,0x75);
WWHD_OFFSET(TControl,mTransformOnSet_Origin,0x78); WWHD_OFFSET(TControl,mTransformOnGet_Origin,0x84); WWHD_OFFSET(TControl,mTransformOnSet_RotationY,0x90); WWHD_OFFSET(TControl,mTransformOnGet_RotationY,0x94); WWHD_OFFSET(TControl,mTransformOnSet_Matrix,0x98); WWHD_OFFSET(TControl,mTransformOnGet_Matrix,0xC8);
WWHD_SIZE(TFactory,0x14); WWHD_OFFSET(TFactory,mList,4); WWHD_OFFSET(TFactory,fvb_Factory,0x10); WWHD_SIZE(TCreateObject,0xC); WWHD_OFFSET(TCreateObject,__vtbl,8);
WWHD_SIZE(TParse,0x10); WWHD_OFFSET(TParse,fvb_Parse,8);
}
