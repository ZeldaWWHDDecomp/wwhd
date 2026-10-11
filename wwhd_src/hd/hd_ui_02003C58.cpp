/* HD: NW4F layout accessor and pane builder, with Bounding wrapper. No GameCube counterpart. */
#include "hd/hd_ui_02003C58.h"
static const u32 layoutVtable = 0x10000810;
static const u32 paneVtable = 0x101800B8;
static const u32 stringVtable = 0x100007AC;
static const u32 bufferVtable = 0x100007DC;
static const u32 boundingVtable = 0x100008B8;
static u32 sLayoutTypeReady;
static u32 sAccessorTypeReady;
static u32 sRootTypeReady;
static u32 sOwnerTypeReady;
static u32 sOwnerRootTypeReady;
static hdLayout_typeInfo_c sLayoutType;
static hdLayout_typeInfo_c sAccessorType;
static hdLayout_typeInfo_c sRootType;
static hdLayout_typeInfo_c sOwnerType;
static hdLayout_typeInfo_c sOwnerRootType;

/* 02003C58 */
static hdLayout_typeInfo_c* hdLayout_getTypeInfo() {
    if (sLayoutTypeReady) return &sLayoutType;
    u32 parentReady = sAccessorTypeReady;
    sLayoutTypeReady = 1;
    if (parentReady) { sLayoutType.mpParent = &sAccessorType; return &sLayoutType; }
    u32 rootReady = sRootTypeReady;
    sAccessorTypeReady = 1;
    if (!rootReady) { sRootTypeReady = 1; sRootType.mpParent = nullptr; }
    sLayoutType.mpParent = &sAccessorType;
    sAccessorType.mpParent = &sRootType;
    return &sLayoutType;
}
static void hdLayout_constructChildren(hdLayout_c* layout) {
    // HD: preserve the embedded-array address wrapping to null at this == -0x3C.
    hdLayout_children_c* children = gabi::at<hdLayout_children_c>(gabi::ea(layout) + 0x3C);
    if (!children) {
        children = static_cast<hdLayout_children_c*>(operator_new(sizeof(hdLayout_children_c)));
        if (!children) return;
    }
    children->mpData = nullptr;
    children->mCount = 0;
    children->mCapacity = 0;
    hdLayout_setupChildren(children, 128, children->mStorage);
}
/* 02003CDC */
static hdLayout_c* hdLayout_construct(hdLayout_c* layout, hdLayout_c* owner, void* accessor) {
    if (!layout) {
        layout = static_cast<hdLayout_c*>(operator_new(sizeof(hdLayout_c)));
        if (!layout) return nullptr;
    }
    hdLayout_constructBase(layout);
    layout->mpResourceAccessor = accessor;
    layout->mpOwner = owner;
    layout->mVtable.v = layoutVtable;
    hdLayout_constructChildren(layout);
    return layout;
}
/* 02003D84 */
static hdLayout_c* hdLayout_constructDefault(hdLayout_c* layout) {
    if (!layout) {
        layout = static_cast<hdLayout_c*>(operator_new(sizeof(hdLayout_c)));
        if (!layout) return nullptr;
    }
    hdLayout_constructBase(layout);
    layout->mpResourceAccessor = layout;
    layout->mpOwner = nullptr;
    layout->mVtable.v = layoutVtable;
    hdLayout_constructChildren(layout);
    return layout;
}
/* 02003E1C */
void hdLayout_c::destroy(s32 flags) {
    if (!this) return;
    s32 count = mChildren.mCount;
    mVtable.v = layoutVtable;
    for (s32 i = 0; i < count; ++i) {
        if (u32(i) >= u32(count)) continue;
        hdLayout_c* child = *gabi::at<gptr<hdLayout_c>>(gabi::ea(mChildren.mpData) + u32(i) * 4);
        if (!child) continue;
        hdLayout_destroyChild(child, 3);
        count = mChildren.mCount;
    }
    mChildren.mCount = 0;
    hdLayout_destroyBase(this, 0);
    if (flags & 1) operator_delete(this);
}
static void* hdLayout_constructPane(u32 size, hdLayout_paneCtor_t ctor, void* resource, void* context, hdLayout_buildArgs_c* args) {
    gabi::Local<hdLayout_buildArgs_c> copy;
    for (u32 i = 0; i < 13; ++i) copy->mWords[i] = u32(args->mWords[i]);
    void* pane = hdLayout_allocate(size, 4);
    if (!pane) return nullptr;
    return ctor(pane, resource, context, copy);
}
enum hdLayout_signature_e { PANE_BOUNDING = 0x626E6431, PANE_PICTURE = 0x70696331, PANE_PARTS = 0x70727431, PANE_TEXT = 0x74787431, PANE_WINDOW = 0x776E6431 };
/* 02003EE8 */
void* hdLayout_c::buildPaneObj(u32 signature, void* resource, void* context, hdLayout_buildArgs_c* args) {
    switch (signature) {
    case PANE_BOUNDING: return hdLayout_constructPane(sizeof(hdLayout_bounding_c), hdLayout_constructBoundingPane, resource, context, args);
    case PANE_PICTURE: return hdLayout_constructPane(0xC0, hdLayout_constructPicture, resource, context, args);
    case PANE_PARTS: return hdLayout_constructPane(0xB0, hdLayout_constructPartsPane, resource, context, args);
    case PANE_TEXT: return hdLayout_constructPane(0x104, hdLayout_constructTextBox, resource, context, args);
    case PANE_WINDOW: return hdLayout_constructPane(0xD8, hdLayout_constructWindow, resource, context, args);
    default: return hdLayout_buildPaneBase(this, signature, resource, context, args);
    }
}
/* 020041EC */
hdLayout_c* hdLayout_c::buildParts(void* resource, void* name, void* options) {
    void* data = hdLayout_getPartsResource(this, resource, name, options);
    void* accessor = mpResourceAccessor;
    hdLayout_c* child = static_cast<hdLayout_c*>(hdLayout_allocate(sizeof(hdLayout_c), 4));
    if (child) child = hdLayout_constructChild(child, this, accessor);
    // HD: the original also performs this vtable read when allocation returns null.
    hdLayout_vtable_c* vtable = *gabi::at<gptr<hdLayout_vtable_c>>(gabi::ea(child) + 0x30);
    u32 build = vtable->mBuildParts;
    gabi::call_ptr<void>(build, child, data, (void*)mpBuildContext, options, name);
    return child;
}
/* 020042A4 */
void hdLayout_c::addChild(hdLayout_c* child) {
    s32 count = mChildren.mCount;
    if (count >= s32(mChildren.mCapacity)) return;
    // HD: a negative count retains the original guest-address wrap.
    *gabi::at<gptr<hdLayout_c>>(gabi::ea(mChildren.mpData) + u32(count) * 4) = child;
    mChildren.mCount = u32(mChildren.mCount) + 1;
}
/* 020042D0 */
hdLayout_pane_c* hdLayout_c::buildPane(hdLayout_string_c* name, hdLayout_string_c* group, u32 flags, u32 finishFlags) {
    name->assureTermination();
    void* resource = hdLayout_findResource(this, (be<u8>*)name->mpText);
    gabi::Local<hdLayout_resource_c> description;
    hdLayout_describeResource(description, resource);
    void* data = description->mpData;
    hdLayout_pane_c* pane = nullptr;
    if (data) {
        pane = static_cast<hdLayout_pane_c*>(hdLayout_allocate(sizeof(hdLayout_pane_c), 4));
        if (pane) {
            hdLayout_constructPaneBase(pane);
            pane->mFlags = 0;
            pane->mVtable.v = paneVtable;
            gabi::Local<be<u32>> owner;
            *owner = gabi::ea(this) + 4;
            hdLayout_linkPane(this, owner, pane);
            gabi::call_ptr<void>(pane->mVtable->mBuild, pane, data, (void*)mpBuildContext);
        }
    }
    group->assureTermination();
    hdLayout_group_c* found = hdLayout_findGroup(mpGroups, group->mpText);
    hdLayout_groupNode_c* head = gabi::at<hdLayout_groupNode_c>(gabi::ea(found) + 0xC);
    for (hdLayout_groupNode_c* node = found->mHead.mpNext; node != head; node = node->mpNext) {
        gabi::call_ptr<void>(mVtable->mBindGroupPane, this, pane, (void*)node->mpPane, flags);
    }
    hdLayout_paneVtable_c* vtable = *gabi::at<gptr<hdLayout_paneVtable_c>>(gabi::ea(pane) + 0x14);
    gabi::call_ptr<void>(vtable->mFinish, pane, finishFlags);
    return pane;
}
static bool hdLayout_namesEqual(hdLayout_string_c* a, hdLayout_string_c* b) {
    hdLayout_prepareString(a);
    a->assureTermination();
    u32 assure = b->mVtable->mAssureTermination;
    be<u8>* first = a->mpText;
    gabi::call_ptr<void>(assure, b);
    if (first == (be<u8>*)b->mpText) return true;
    u32 p = gabi::ea(a->mpText), q = gabi::ea(b->mpText) - 1;
    for (u32 k = 0; k < 0x40001; ++k) {
        u8 left = *gabi::at<be<u8>>(p);
        ++q;
        u8 right = *gabi::at<be<u8>>(q);
        if (left != right) return false;
        if (!left) return true;
        ++p;
    }
    return false;
}
/* 02004490 */
void hdLayout_c::bindParts(hdLayout_partsPane_c* pane, hdLayout_names_c* names, u32 index) {
    hdLayout_partsResource_c* resource = pane->mpResource;
    be<u32>* table = gabi::at<be<u32>>(gabi::ea(resource) + u32(resource->mTableOffset));
    gabi::Local<hdLayout_string_c> requested;
    gabi::Local<hdLayout_string_c> candidate;
    gabi::Local<hdLayout_fixedString_c> numbered;
    for (u32 i = 0; i < u16(resource->mCount); i = (i + 1) & 0xFFFF) {
        hdLayout_partEntry_c* entry = gabi::at<hdLayout_partEntry_c>(gabi::ea(resource) + u32(table[i]));
        candidate->mVtable.v = stringVtable;
        requested->mVtable.v = stringVtable;
        requested->mpText.v = gabi::ea(names) + 0x80;
        candidate->mpText = entry->mName;
        if (!hdLayout_namesEqual(requested, candidate)) continue;
        numbered->mpText = numbered->mBuffer;
        numbered->mCapacity = 0x1D;
        numbered->mBuffer[0] = 0;
        numbered->mBuffer[0x1C] = 0;
        numbered->mVtable.v = bufferVtable;
        hdLayout_format(numbered, "%s", entry, bufferVtable);
        numbered->assureTermination();
        u32 data = gabi::ea(numbered->mpText), length = 0, cursor = data;
        if (*gabi::at<be<u8>>(cursor)) {
            for (;;) {
                ++length;
                ++cursor;
                if (s32(length) > 0x40000) { length = 0xFFFFFFFF; break; }
                if (!*gabi::at<be<u8>>(cursor)) break;
            }
        }
        u32 cut = length == 0xFFFFFFFF ? 0 : length - (length < 2 ? length : 2);
        *gabi::at<be<u8>>(data + cut) = 0;
        numbered->assureTermination();
        hdLayout_format(numbered, "%s%02d", (be<u8>*)numbered->mpText, index);
        u32 material = entry->mMaterial;
        hdLayout_lookupVtable_c* lookup = mpPaneLookup->mVtable;
        numbered->assureTermination();
        u32 find = material ? u32(lookup->mFindMaterial) : u32(lookup->mFindPane);
        void* object = gabi::call_ptr<void*>(find, (hdLayout_lookup_c*)mpPaneLookup, (be<u8>*)numbered->mpText, 1);
        if (!object) continue;
        u32 bound = material ? hdLayout_bindMaterial(pane, object, entry) : hdLayout_bindPane(pane, object, entry);
        if (!bound) return;
    }
}
/* 02004794 */
static void hdLayout_initStatics() { sinit_header_statics_rodata(0x101FF258, 0x1018C350, 0x101FF264, 0x10000808); }
/* 02004828 */
static void hdLayout_destroyComponent(void* component, s32 flags) { if (component && (flags & 1)) operator_delete(component); }
/* 0200483C */
static void hdLayout_prepareStringImpl() {}
/* 02004840 */
static void hdLayout_assureTermination(hdLayout_bufferedString_c* string) {
    *gabi::at<be<u8>>(gabi::ea(string->mpText) + u32(string->mCapacity) - 1) = 0;
}
/* 02004858 */
static void hdLayout_destroyString(void* string, s32 flags) { if (string && (flags & 1)) operator_delete(string); }
/* 0200486C */
static void hdLayout_destroyBuffer(void* buffer, s32 flags) { if (buffer && (flags & 1)) operator_delete(buffer); }
/* 02004880 */
static hdLayout_typeInfo_c* hdLayout_getOwnerTypeInfo() {
    if (!sOwnerTypeReady) {
        u32 rootReady = sOwnerRootTypeReady;
        sOwnerTypeReady = 1;
        if (!rootReady) { sOwnerRootTypeReady = 1; sOwnerRootType.mpParent = nullptr; }
        sOwnerType.mpParent = &sOwnerRootType;
    }
    return &sOwnerType;
}
/* 020048D0 */
static hdLayout_bounding_c* hdLayout_constructBounding(hdLayout_bounding_c* pane, void* resource, void* context, hdLayout_buildArgs_c* args) {
    if (!pane) {
        pane = static_cast<hdLayout_bounding_c*>(operator_new(sizeof(hdLayout_bounding_c)));
        if (!pane) return nullptr;
    }
    hdLayout_constructBoundingBase(pane, resource, context, args);
    pane->mVtable = boundingVtable;
    return pane;
}
/* 02004954 */
static void hdLayout_destroyBounding(hdLayout_bounding_c* pane, s32 flags) {
    if (!pane) return;
    hdLayout_destroyBoundingBase(pane, 0);
    if (flags & 1) operator_delete(pane);
}
/* 020049A8 */
static void hdLayout_initBoundingStatics() { sinit_header_statics_rodata(0x101FF274, 0x1018C374, 0x101FF280, 0x100008B0); }

/* WWHD glue
data sLayoutTypeReady 101FD92C nocheck
data sAccessorTypeReady 101FD8C0 nocheck
data sRootTypeReady 101FD888 nocheck
data sOwnerTypeReady 101FD8C4 nocheck
data sOwnerRootTypeReady 101FD894 nocheck
data sLayoutType 101FD930 nocheck
data sAccessorType 101FD934 nocheck
data sRootType 101FD8FC nocheck
data sOwnerType 101FD900 nocheck
data sOwnerRootType 101FD904 nocheck
str 100007FC "%s" in 02004490
str 10000800 "%s%02d" in 02004490
ext 0273B560 void hdLayout_setupChildren(hdLayout_children_c* children, u32 capacity, gptr<hdLayout_c>* storage)
ext 0287F418 void hdLayout_constructBase(hdLayout_c* layout)
ext 0287F4E0 void hdLayout_destroyBase(hdLayout_c* layout, u32 flags)
ext 02005A7C void hdLayout_destroyChild(hdLayout_c* layout, u32 flags)
ext 0287F3DC void* hdLayout_allocate(u32 size, u32 alignment)
ext 020048D0 void* hdLayout_constructBoundingPane(void* pane, void* resource, void* context, hdLayout_buildArgs_c* args)
ext 02002698 void* hdLayout_constructPicture(void* pane, void* resource, void* context, hdLayout_buildArgs_c* args)
ext 020037D0 void* hdLayout_constructPartsPane(void* pane, void* resource, void* context, hdLayout_buildArgs_c* args)
ext 020021FC void* hdLayout_constructTextBox(void* pane, void* resource, void* context, hdLayout_buildArgs_c* args)
ext 02003AEC void* hdLayout_constructWindow(void* pane, void* resource, void* context, hdLayout_buildArgs_c* args)
ext 028807D0 void* hdLayout_buildPaneBase(hdLayout_c* layout, u32 signature, void* resource, void* context, hdLayout_buildArgs_c* args)
ext 02880AE0 void* hdLayout_getPartsResource(hdLayout_c* layout, void* resource, void* name, void* options)
ext 02003CDC hdLayout_c* hdLayout_constructChild(hdLayout_c* layout, hdLayout_c* owner, void* accessor)
ext 0287F6D0 void* hdLayout_findResource(hdLayout_c* layout, be<u8>* name)
ext 028727E8 void hdLayout_describeResource(hdLayout_resource_c* description, void* resource)
ext 02872998 void hdLayout_constructPaneBase(hdLayout_pane_c* pane)
ext 0286DA70 void hdLayout_linkPane(hdLayout_c* layout, be<u32>* owner, hdLayout_pane_c* pane)
ext 02874820 hdLayout_group_c* hdLayout_findGroup(void* groups, be<u8>* name)
ext 0200483C void hdLayout_prepareString(hdLayout_string_c* string)
ext 02759C28 void hdLayout_format(hdLayout_fixedString_c* string, const char* format, void* text, u32 value)
ext 02871710 u32 hdLayout_bindPane(hdLayout_partsPane_c* pane, void* object, hdLayout_partEntry_c* entry)
ext 0287174C u32 hdLayout_bindMaterial(hdLayout_partsPane_c* pane, void* object, hdLayout_partEntry_c* entry)
ext 028757F0 void hdLayout_constructBoundingBase(hdLayout_bounding_c* pane, void* resource, void* context, hdLayout_buildArgs_c* args)
ext 02875864 void hdLayout_destroyBoundingBase(hdLayout_bounding_c* pane, u32 flags)
*/
