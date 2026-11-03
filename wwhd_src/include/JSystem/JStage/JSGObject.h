// JStage default interface; Cafe uses explicit eight-byte vtable slots.
#pragma once
#include "wwhd.h"
namespace JStage {
struct TObject {
    gptr<void> __vtbl;
    void destroy(s32 flags);
    u32 JSGGetFlag() const;
    void JSGSetData(u32, const void*, u32);
    void JSGGetParent(gptr<TObject>*, be<u32>*) const;
    void JSGSetParent(TObject*, u32);
    void JSGSetRelation(u32, TObject*, u32);
    s32 JSGFindNodeID(const char*) const;
    bool JSGGetNodeTransformation(u32, Mtx34*) const;
};
WWHD_SIZE(TObject,4); WWHD_OFFSET(TObject,__vtbl,0);
}
