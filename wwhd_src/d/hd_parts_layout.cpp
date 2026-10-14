/* HD layout loading companion to the base layout screen. */
#include "d/hd_layout_mgr.h"
static u32 sLayoutHeaderStatics;
static u32 sLayoutHeaderDescriptors;
static u32 sLayoutHeaderZero;
static u32 sLayoutPiConstants;
static u32 sLayoutTypeGuard;
static u32 sLayoutParentGuard;
static u32 sLayoutRootGuard;
static hdScreenType sLayoutType;
static hdScreenType sLayoutParent;
static hdScreenType sLayoutRoot;
static hdScreenTable sPartsLayoutTable;
static hdLayoutUiSystem_c* sLayoutUi;
static hdScreenStringTable sLayoutFixedStringTable;
void layoutDestroyBase(void*, u32);
void layoutDestroyScreen(hdPartsLayout_c*, u32);
void layoutConstructBase(hdPartsLayout_c*);
void layoutConstructBinding(hdPartsResAccessor_c*);
void layoutDestroyBinding(hdPartsResAccessor_c*, u32);
u32 layoutBuildBase(hdPartsLayout_c*, u32, u32, void*, void*, u32, u32, void*);
hdPartsLyt_c* layoutConstructAccessor(hdPartsLyt_c*);
void layoutSetPartLayout(hdScreenAccessor*);
void* layoutAllocate(u32, hdLayoutHeap_c*, u32);
hdScreenHolder* layoutConstructHolder(hdScreenHolder*, void*, hdScreenAccessor*);
void layoutFormat(hdScreenFixedString*, const char*, const char*);
void layoutLoad(hdScreenAccessor*, const char*, u32, hdScreenHolder*, u32);
/* 026FACD8 */
static hdScreenType* getPartsLayoutType() {
    if (sLayoutTypeGuard)
        return &sLayoutType;
    u32 parent = sLayoutParentGuard;
    sLayoutTypeGuard = 1;
    if (parent) {
        sLayoutType.mParent = &sLayoutParent;
        return &sLayoutType;
    }
    u32 root = sLayoutRootGuard;
    sLayoutParentGuard = 1;
    if (!root) {
        sLayoutRootGuard = 1;
        sLayoutRoot.mParent = NULL;
    }
    sLayoutType.mParent = &sLayoutParent;
    sLayoutParent.mParent = &sLayoutRoot;
    return &sLayoutType;
}
/* 026FAD5C */
static void destroyLayoutAccessor(hdScreenAccessor* object, u32 flags) {
    if (!object)
        return;
    layoutDestroyBase(object, 0);
    if (flags & 1)
        operator_delete(object);
}
/* 026FADB0 */
static hdPartsLayout_c* constructPartsLayout(hdPartsLayout_c* object) {
    if (!object) {
        object = (hdPartsLayout_c*)operator_new(sizeof(hdPartsLayout_c));
        if (!object)
            return NULL;
    }
    layoutConstructBase(object);
    object->mVtable = &sPartsLayoutTable;
    layoutConstructBinding(&object->mResourceBinding);
    return object;
}
/* 026FAE0C */
static void destroyPartsLayout(hdPartsLayout_c* object, u32 flags) {
    if (!object)
        return;
    layoutDestroyBinding(&object->mResourceBinding, 0);
    layoutDestroyScreen(object, 0);
    if (flags & 1)
        operator_delete(object);
}
/* 026FAE6C */
u32 hdPartsLayout_c::prepare(u32 first, u32 second, void* resource, void* block, u32 option, u32 flags,
                             void* archive) {
    if (!layoutBuildBase(this, first, second, resource, block, option, flags, archive))
        return 0;
    hdPartsLyt_c* accessor = (hdPartsLyt_c*)mAccessor.get();
    hdLayoutResourceBindingTable_c* table = (hdLayoutResourceBindingTable_c*)accessor->mVtable.get();
    gabi::call_ptr(table->mAttach.get(), accessor, &mResourceBinding);
    accessor = (hdPartsLyt_c*)mAccessor.get();
    // The type guard is expanded here in GHS; the natural helper must remain inlined.
    hdScreenType* wanted;
    if (!sLayoutTypeGuard) {
        u32 parent = sLayoutParentGuard;
        sLayoutTypeGuard = 1;
        if (!parent) {
            u32 root = sLayoutRootGuard;
            sLayoutParentGuard = 1;
            if (!root) {
                sLayoutRootGuard = 1;
                sLayoutRoot.mParent = NULL;
            }
            sLayoutParent.mParent = &sLayoutRoot;
        }
        sLayoutType.mParent = &sLayoutParent;
    }
    wanted = &sLayoutType;
    hdScreenAccessor* matched = NULL;
    if (accessor) {
        hdScreenType* type = gabi::call_ptr<hdScreenType*>(accessor->mVtable->mGetRuntimeTypeInfo.get(), accessor);
        for (; type; type = type->mParent) {
            if (type == wanted) {
                matched = (hdScreenAccessor*)accessor;
                break;
            }
        }
    }
    layoutSetPartLayout(matched);
    return 1;
}
/* 026FAFC0 */
u32 hdPartsLayout_c::load(hdScreenString* name, u32 kind) {
    hdLayoutHeap_c* heap = sLayoutUi->mpHeap;
    heap->begin();
    heap->end();
    hdPartsLyt_c* accessor = (hdPartsLyt_c*)layoutAllocate(0x248, heap, 4);
    if (accessor)
        accessor = layoutConstructAccessor(accessor);
    mAccessor = (hdScreenAccessor*)accessor;
    if (!accessor)
        return 0;
    hdScreenFixedString path;
    path.mText = path.mBuffer;
    path.mVtable = &sLayoutFixedStringTable;
    path.mCapacity = 64;
    path.mBuffer[63] = 0;
    path.mBuffer[0] = 0;
    name->assureTermination();
    layoutFormat(&path, "%s.bflyt", name->mText);
    hdScreenHolder* holder = (hdScreenHolder*)layoutAllocate(12, heap, 4);
    if (holder)
        holder = layoutConstructHolder(holder, this, mAccessor);
    mHolder = holder;
    if (!holder)
        return 0;
    path.assureTermination();
    hdScreenHolder* bound = mHolder;
    const char* filename = (const char*)path.mText.get();
    layoutLoad(mAccessor, filename, kind, bound, 0);
    heap->begin();
    heap->end();
    return 1;
}
/* 026FB16C */
static void __sinit_hd_parts_layout_cpp() {
    sinit_header_statics_rodata(gabi::ea(&sLayoutHeaderStatics), gabi::ea(&sLayoutHeaderDescriptors), gabi::ea(&sLayoutHeaderZero),
                                gabi::ea(&sLayoutPiConstants));
}
/* WWHD glue
data sLayoutHeaderStatics 1049DDDC nocheck
data sLayoutHeaderDescriptors 101F722C nocheck
data sLayoutHeaderZero 1049DDE8 nocheck
data sLayoutPiConstants 1010A77C nocheck
data sLayoutTypeGuard 101FDD9C
data sLayoutParentGuard 101FD8C4
data sLayoutRootGuard 101FD894
data sLayoutType 101FDDA8
data sLayoutParent 101FD900
data sLayoutRoot 101FD904
data sPartsLayoutTable 1010A788
data sLayoutUi 1018C404
data sLayoutFixedStringTable 1010A758
str 1010A770 "%s.bflyt"
ext 02003E1C void layoutDestroyBase(void*,u32)
ext 02002918 void layoutDestroyScreen(hdPartsLayout_c*,u32)
ext 02002888 void layoutConstructBase(hdPartsLayout_c*)
ext 026025CC void layoutConstructBinding(hdPartsResAccessor_c*)
ext 0286FA30 void layoutDestroyBinding(hdPartsResAccessor_c*,u32)
ext 02002A40 u32 layoutBuildBase(hdPartsLayout_c*,u32,u32,void*,void*,u32,u32,void*)
ext 026FA398 hdPartsLyt_c* layoutConstructAccessor(hdPartsLyt_c*)
ext 026FA920 void layoutSetPartLayout(hdScreenAccessor*)
ext 0273B050 void* layoutAllocate(u32,hdLayoutHeap_c*,u32)
ext 02002450 hdScreenHolder* layoutConstructHolder(hdScreenHolder*,void*,hdScreenAccessor*)
ext 02759C28 void layoutFormat(hdScreenFixedString*,const char*,const char*)
ext 0287F640 void layoutLoad(hdScreenAccessor*,const char*,u32,hdScreenHolder*,u32)
*/
