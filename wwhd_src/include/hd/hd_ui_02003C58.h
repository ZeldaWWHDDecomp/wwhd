/* HD: layout resource accessor, pane construction and parts binding; no GameCube counterpart. */
#pragma once
#include "bindings.h"
struct hdLayout_c;
struct hdLayout_stringVtable_c { u8 _00[0x14]; be<u32> mAssureTermination; };
struct hdLayout_string_c {
    gptr<be<u8>> mpText; gptr<hdLayout_stringVtable_c> mVtable;
    void assureTermination() { gabi::call_ptr<void>(mVtable->mAssureTermination, this); }
};
struct hdLayout_bufferedString_c : hdLayout_string_c { be<u32> mCapacity; };
struct hdLayout_fixedString_c : hdLayout_bufferedString_c { be<u8> mBuffer[0x1D]; };
WWHD_SIZE(hdLayout_string_c, 8);
WWHD_SIZE(hdLayout_fixedString_c, 0x29);
struct hdLayout_buildArgs_c { be<u32> mWords[13]; };
struct hdLayout_resource_c { u8 _00[4]; gptr<void> mpData; u8 _08[0xC]; };
WWHD_SIZE(hdLayout_buildArgs_c, 0x34);
WWHD_SIZE(hdLayout_resource_c, 0x14);
struct hdLayout_partEntry_c { be<u8> mName[0x1D]; be<u8> mMaterial; };
struct hdLayout_partsResource_c { u8 _00[0xE]; be<u16> mCount; be<u32> mTableOffset; };
struct hdLayout_partsPane_c { u8 _00[8]; gptr<hdLayout_partsResource_c> mpResource; };
struct hdLayout_names_c { u8 _00[0x80]; be<u8> mName[1]; };
struct hdLayout_lookupVtable_c { u8 _00[0x5C]; be<u32> mFindPane; u8 _60[4]; be<u32> mFindMaterial; };
struct hdLayout_lookup_c { u8 _00[8]; gptr<hdLayout_lookupVtable_c> mVtable; };
struct hdLayout_paneVtable_c { u8 _00[0x24]; be<u32> mFinish; u8 _28[0x44-0x28]; be<u32> mBuild; };
struct hdLayout_pane_c { u8 _00[0x14]; gptr<hdLayout_paneVtable_c> mVtable; u8 _18[0x30-0x18]; be<u32> mFlags; };
struct hdLayout_groupNode_c { gptr<hdLayout_groupNode_c> mpNext; u8 _04[4]; gptr<void> mpPane; };
struct hdLayout_group_c { u8 _00[0xC]; hdLayout_groupNode_c mHead; };
struct hdLayout_vtable_c { u8 _00[0x7C]; be<u32> mBuildParts; u8 _80[0x9C-0x80]; be<u32> mBindGroupPane; };
struct hdLayout_children_c { be<s32> mCount; be<s32> mCapacity; gptr<gptr<hdLayout_c>> mpData; gptr<hdLayout_c> mStorage[128]; };
WWHD_SIZE(hdLayout_children_c, 0x20C);
struct hdLayout_c {
    u8 _00[0xC]; gptr<hdLayout_lookup_c> mpPaneLookup; gptr<void> mpGroups;
    u8 _14[0x20-0x14]; gptr<void> mpBuildContext; u8 _24[0x30-0x24];
    gptr<hdLayout_vtable_c> mVtable; gptr<hdLayout_c> mpOwner; gptr<void> mpResourceAccessor;
    hdLayout_children_c mChildren;
    void destroy(s32 flags);
    hdLayout_c* buildParts(void* resource, void* name, void* options);
    void addChild(hdLayout_c* child);
    hdLayout_pane_c* buildPane(hdLayout_string_c* name, hdLayout_string_c* group, u32 flags, u32 finishFlags);
    void bindParts(hdLayout_partsPane_c* pane, hdLayout_names_c* names, u32 index);
    void* buildPaneObj(u32 signature, void* resource, void* context, hdLayout_buildArgs_c* args);
};
WWHD_OFFSET(hdLayout_c, mpPaneLookup, 0xC);
WWHD_OFFSET(hdLayout_c, mpGroups, 0x10);
WWHD_OFFSET(hdLayout_c, mpBuildContext, 0x20);
WWHD_OFFSET(hdLayout_c, mVtable, 0x30);
WWHD_OFFSET(hdLayout_c, mpOwner, 0x34);
WWHD_OFFSET(hdLayout_c, mpResourceAccessor, 0x38);
WWHD_OFFSET(hdLayout_c, mChildren, 0x3C);
WWHD_SIZE(hdLayout_c, 0x248);
struct hdLayout_bounding_c { u8 _00[8]; be<u32> mVtable; u8 _0C[0xA4-0xC]; };
WWHD_SIZE(hdLayout_bounding_c, 0xA4);
struct hdLayout_typeInfo_c { gptr<hdLayout_typeInfo_c> mpParent; };
typedef void* (*hdLayout_paneCtor_t)(void*, void*, void*, hdLayout_buildArgs_c*);

void hdLayout_setupChildren(hdLayout_children_c* children, u32 capacity, gptr<hdLayout_c>* storage);
void hdLayout_constructBase(hdLayout_c* layout);
void hdLayout_destroyBase(hdLayout_c* layout, u32 flags);
void hdLayout_destroyChild(hdLayout_c* layout, u32 flags);
void* hdLayout_allocate(u32 size, u32 alignment);
void* hdLayout_constructBoundingPane(void* pane, void* resource, void* context, hdLayout_buildArgs_c* args);
void* hdLayout_constructPicture(void* pane, void* resource, void* context, hdLayout_buildArgs_c* args);
void* hdLayout_constructPartsPane(void* pane, void* resource, void* context, hdLayout_buildArgs_c* args);
void* hdLayout_constructTextBox(void* pane, void* resource, void* context, hdLayout_buildArgs_c* args);
void* hdLayout_constructWindow(void* pane, void* resource, void* context, hdLayout_buildArgs_c* args);
void* hdLayout_buildPaneBase(hdLayout_c* layout, u32 signature, void* resource, void* context, hdLayout_buildArgs_c* args);
void* hdLayout_getPartsResource(hdLayout_c* layout, void* resource, void* name, void* options);
hdLayout_c* hdLayout_constructChild(hdLayout_c* layout, hdLayout_c* owner, void* accessor);
void* hdLayout_findResource(hdLayout_c* layout, be<u8>* name);
void hdLayout_describeResource(hdLayout_resource_c* description, void* resource);
void hdLayout_constructPaneBase(hdLayout_pane_c* pane);
void hdLayout_linkPane(hdLayout_c* layout, be<u32>* owner, hdLayout_pane_c* pane);
hdLayout_group_c* hdLayout_findGroup(void* groups, be<u8>* name);
void hdLayout_prepareString(hdLayout_string_c* string);
void hdLayout_format(hdLayout_fixedString_c* string, const char* format, void* text, u32 value);
u32 hdLayout_bindPane(hdLayout_partsPane_c* pane, void* object, hdLayout_partEntry_c* entry);
u32 hdLayout_bindMaterial(hdLayout_partsPane_c* pane, void* object, hdLayout_partEntry_c* entry);
void hdLayout_constructBoundingBase(hdLayout_bounding_c* pane, void* resource, void* context, hdLayout_buildArgs_c* args);
void hdLayout_destroyBoundingBase(hdLayout_bounding_c* pane, u32 flags);
