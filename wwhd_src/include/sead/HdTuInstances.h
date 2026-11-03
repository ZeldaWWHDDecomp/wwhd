/* sead template and inline instances that GHS emitted inside HD game translation units: the
 * string-keyed red-black tree map of the resource managers, the pooled tree-map nodes, the tree
 * walk with a member-function delegate, the formatted fixed strings, the resource factories of the
 * resource manager and the per-layer render-step name tables. Each instance keeps its own entry
 * point; its companion source sits next to the HD game TU that emitted it (`<tu>_lib.cpp`).
 * Layouts come from this game's constructors and instructions. */
#pragma once
#include "sead/InlineInstances.h"
#include "sead/Resource.h"
#include "sead/ArchiveRes.h"
#include "sead/RuntimeFormatSupport.h"
#include "sead/TreeMapPool.h"

namespace sead { namespace instances {

/* ---- red-black tree map nodes (left-leaning; colour byte 1 = red) ---- */
struct TreeNodeVtable {
    VtableSlot mHeader;
    VtableSlot mDestroy;
    VtableSlot mErase;  // returns the node to its map's free list
};

/* the part every map node shares: links, colour, then key/vtable/value, then the owning map's node
 * pool (sead/TreeMapPool.h). Nodes with a larger key or value continue after +0x20 (the archive
 * map's are 0x54, ResourceManagerHD::ArchiveNode; the window config map's 0x64). */
struct PooledTreeNode {
    gptr<PooledTreeNode> mLeft;  // also the free-list link of a released node
    gptr<PooledTreeNode> mRight;
    be<u8> mColor;
    u8 mPadding[3];
    u8 mKeyAndValue[0x10];
    gptr<TreeMapPool<PooledTreeNode>> mPool;
};

/* the common part of a node of a map keyed by a SafeString (larger nodes continue after +0x20) */
struct StrTreeNode {
    gptr<StrTreeNode> mLower;   // smaller keys
    gptr<StrTreeNode> mUpper;   // larger keys
    be<u8> mRedLink;  // the link from the parent is red
    u8 mPadding[3];
    SafeText mKey;
    gptr<TreeNodeVtable> mVtable;
    be<u32> mValue;
    gptr<TreeMapPool<PooledTreeNode>> mPool;

    enum { cRedLink = 1 };
    void erase() { gabi::call_ptr<void>(mVtable->mErase.mFunction, this); }
};

/* the shared tree algorithms of the SafeString-keyed maps (one instance for every such map) */
struct StrTreeMap {
    gptr<StrTreeNode> mRoot;

    StrTreeNode* find(StrTreeNode* node, SafeText* key);
    StrTreeNode* insert(StrTreeNode* root, StrTreeNode* node);
    StrTreeNode* erase(StrTreeNode* root, SafeText* key);
    static StrTreeNode* eraseMin(StrTreeNode* root);
};

/* ---- formatted fixed strings (the variadic register save area: sead/RuntimeFormatSupport.h) ---- */
using format::VariadicSaveArea;
using format::saveVariadicRegisters;
using format::startVariadicArguments;

/* FormatFixedSafeString<N> / WFormatFixedSafeString<N>: a buffered string over its own storage */
template <u32 N> struct FormatFixedText : format::ByteTextBuffer {
    be<u8> mStorage[N];
};
template <u32 N> struct FormatFixedWideText : format::WideTextBuffer {
    be<u16> mStorage[N];
};

/* ---- per-TU instances ---- */
namespace res_model {
/* FormatFixedSafeString<260>: model and texture paths */
struct PathText : FormatFixedText<0x104> {
    static PathText* construct(PathText* text, const char* format, u32 a, u32 b, u32 c, u32 d, u32 e, u32 f);
};
}

namespace res_load_task {
struct ReferenceNode : PooledTreeNode { void erase(); };
}

namespace res_mgr {
/* the archive and pack map nodes: ResourceManagerHD::ArchiveNode / PackNode (d/hd_res_mgr.h) */

/* a resource made from a pack file: a DirectResource without own data */
struct PackResource : DirectResource {
    void destroy(u32 flags);
    u32 getLoadDataAlignment();
};

/* DirectResourceFactory<T>::newResource_ for the SARC archive and pack resources */
struct ArchiveFactory : DirectResourceFactoryBase {
    DirectResource* newResource(RegistryHeap* heap, s32 alignment);
};
struct PackFactory : DirectResourceFactoryBase {
    DirectResource* newResource(RegistryHeap* heap, s32 alignment);
};

struct DirectoryHandle : sead::DirectoryHandle {
    void destroy(u32 flags);
};
}

namespace btn_map_icon_number {
/* WFormatFixedSafeString<8>: the map icon number */
struct CountText : FormatFixedWideText<8> {
    static CountText* construct(CountText* text, const be<u16>* format, u32 a, u32 b, u32 c, u32 d, u32 e, u32 f);
};
}

namespace screen_menu_map_states {
/* WFormatFixedSafeString<8>: the floor and chart numbers of the map menu */
struct NumberText : FormatFixedWideText<8> {
    static NumberText* construct(NumberText* text, const be<u16>* format, u32 a, u32 b, u32 c, u32 d, u32 e, u32 f);
};
}

namespace parts_window {
struct WindowNode : PooledTreeNode { void erase(); };

/* the two words of a GHS member-function pointer, as a delegate constructor copies them */
struct MemberFunctionWords { be<u32> mWords[2]; };

/* Delegate1<R, StrTreeNode*> over a member function of the map's for-each context */
struct NodeDelegate : DelegateLayout {
    static NodeDelegate* construct(NodeDelegate* delegate, void* object, const MemberFunction* method);
    void invoke(StrTreeNode* node);
};
/* the user's per-node delete callback (through the map's for-each context) */
struct DeleteDelegate : NodeDelegate {
    static DeleteDelegate* construct(DeleteDelegate* delegate, void* object, const MemberFunction* method);
};
/* the map's own per-node clear callback */
struct ClearDelegate : NodeDelegate {
    static ClearDelegate* construct(ClearDelegate* delegate, void* object, const MemberFunction* method);
};
/* TreeMapImpl::forEach(node, delegate): in order, left subtree, node, right subtree */
struct NodeWalk {
    static void forEachDelete(StrTreeNode* node, DeleteDelegate* delegate);
    static void forEachClear(StrTreeNode* node, ClearDelegate* delegate);
};
}

namespace layout_res_mgr {
struct CacheNode : PooledTreeNode { void erase(); };
}

/* agl render layers: the name of render step `step` of the layer (a SafeString returned by value) */
struct RenderStepNames {};
namespace layer_2d_drc { struct Layer : RenderStepNames { SafeText* getRenderStepName(SafeText* name, s32 step) const; }; }
namespace layer_2d_drc_fore { struct Layer : RenderStepNames { SafeText* getRenderStepName(SafeText* name, s32 step) const; }; }
namespace layer_2d_fore { struct Layer : RenderStepNames { SafeText* getRenderStepName(SafeText* name, s32 step) const; }; }
namespace layer_3d { struct Layer : RenderStepNames { SafeText* getRenderStepName(SafeText* name, s32 step) const; }; }
namespace layer_mask { struct Layer : RenderStepNames { SafeText* getRenderStepName(SafeText* name, s32 step) const; }; }
namespace layer_base { struct Layer : RenderStepNames { SafeText* getRenderStepName(SafeText* name, s32 step) const; }; }
namespace error_viewer { struct Layer : RenderStepNames { SafeText* getRenderStepName(SafeText* name, s32 step) const; }; }
namespace fader { struct Layer : RenderStepNames { SafeText* getRenderStepName(SafeText* name, s32 step) const; }; }
namespace multi_filter { struct Layer : RenderStepNames { SafeText* getRenderStepName(SafeText* name, s32 step) const; }; }
namespace layer_shadow { struct Layer : RenderStepNames { SafeText* getRenderStepName(SafeText* name, s32 step) const; }; }
namespace render_commands { struct Layer : RenderStepNames { SafeText* getRenderStepName(SafeText* name, s32 step) const; }; }

} }

WWHD_OFFSET(sead::instances::TreeNodeVtable, mErase, 0x10);
WWHD_OFFSET(sead::instances::PooledTreeNode, mColor, 8);
WWHD_OFFSET(sead::instances::PooledTreeNode, mPool, 0x1C);
WWHD_SIZE(sead::instances::PooledTreeNode, 0x20);
WWHD_OFFSET(sead::instances::StrTreeNode, mRedLink, 8);
WWHD_OFFSET(sead::instances::StrTreeNode, mKey, 0xC);
WWHD_OFFSET(sead::instances::StrTreeNode, mVtable, 0x14);
WWHD_OFFSET(sead::instances::StrTreeNode, mValue, 0x18);
WWHD_OFFSET(sead::instances::StrTreeNode, mPool, 0x1C);
WWHD_SIZE(sead::instances::res_model::PathText, 0x110);
WWHD_SIZE(sead::instances::btn_map_icon_number::CountText, 0x1C);
WWHD_SIZE(sead::instances::screen_menu_map_states::NumberText, 0x1C);
WWHD_SIZE(sead::instances::res_mgr::PackResource, 0x24);
