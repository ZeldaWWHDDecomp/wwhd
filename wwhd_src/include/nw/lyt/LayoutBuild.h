/* Layout block-builder context, extending the merged pane build context without changing it. */
#pragma once
#include "nw/lyt/LayoutSession.h"
#include "nw/lyt/ResourceTables.h"
namespace nw { namespace lyt {
struct LayoutBuildListenerMethods { u8 prefix[0x14]; be<u32> container; };
struct LayoutBuildListener { gptr<LayoutBuildListenerMethods> methods; };
struct LayoutBuildPrefix {
    be<f32> scaleX,scaleY,offsetX,offsetY;
    gptr<LayoutBuildListener> listener;
};
struct LayoutBuildFlagPair { be<u16> primary,secondary; };
struct LayoutBlockBuildContext : PaneBuildContext {
    u8 secondaryFlagStorage[2];
    gptr<void> partFlags;
    gptr<LayoutPane> currentParent;
    gptr<FontBlock> userDataBlock;
    LayoutBuildPrefix* prefix() { return reinterpret_cast<LayoutBuildPrefix*>(header); }
    LayoutBuildFlagPair* flagPair() { return reinterpret_cast<LayoutBuildFlagPair*>(&flags); }
};
struct LayoutDescriptionBlock {
    be<u32> tag,size;
    be<u32> flags;
    be<f32> width,height,offsetX,offsetY;
    be<u8> name[1];
};
// The common head of every pane block (pan1, pic1, txt1, wnd1, bnd1, prt1); format facts: BFLYT pane blocks.
struct PaneResource {
    be<u32> tag,size;
    be<u8> flags,origin,alpha,scaleFlags;
    be<u8> name[24],userInfo[8];
    be<f32> translation[3],rotation[3],scale[2];
    PaneSize paneSize;
};
// A parts pane block: the pane head, the override table and, after its last entry, the nested layout's name.
struct LayoutPartsResource {
    PaneResource pane;
    be<u32> count;
    be<f32> width,height;
    LayoutPartResource entries[1];
    const be<u8>* layoutName() const { return entries[count].name; }
};
WWHD_SIZE(LayoutBuildPrefix,0x14); WWHD_SIZE(LayoutBuildFlagPair,4);
WWHD_SIZE(LayoutBlockBuildContext,0x34);
WWHD_OFFSET(LayoutBlockBuildContext,partFlags,0x28);
WWHD_OFFSET(LayoutBlockBuildContext,currentParent,0x2C);
WWHD_OFFSET(LayoutBlockBuildContext,userDataBlock,0x30);
WWHD_OFFSET(LayoutDescriptionBlock,name,0x1C);
WWHD_SIZE(PaneResource,0x54); WWHD_OFFSET(PaneResource,name,0xC); WWHD_OFFSET(PaneResource,paneSize,0x4C);
WWHD_OFFSET(LayoutPartsResource,count,0x54); WWHD_OFFSET(LayoutPartsResource,entries,0x60);
} }
void* sessionBuildAllocate(u32 size);
void sessionBuildInsert(nw::ut::HeapChildren*,gptr<nw::ut::HeapLink>*,void* node);
