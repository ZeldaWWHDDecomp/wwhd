/* HD animation accumulation and solid-heap scopes. */
#include "m_Do/m_Do_ext_completeness.h"
static f32 blendOne;

/* 025EE884 */
// HD: matrix/translation accumulation over mapped animation records; no GC calc signature claim.
void extBlendAccumulate(ExtBlendDestination* dst, ExtBlendAnimation* anim, be<f32>* weights) {
    anim->mFlags = u32(anim->mFlags) | ExtBlend_Accumulated;
    ExtBlendRecord* record = anim->mRecords;
    dst->mFlags = u32(dst->mFlags) | ExtBlend_Accumulated;
    u16 count = anim->mCount;
    ExtBlendResult* out = dst->mResults;
    if (count == 0) return;
    f32 one = blendOne;
    for (u32 i  =  0; i < count; ++i, ++record) {
        u32 flag = anim->mBindings.get()[i];
        if (flag & ExtBlend_BindingDisabled) continue;
        f32 z = record->mRotation.z;
        f32 y = record->mRotation.y;
        f32 zz = z + z;
        f32 yy = y + y;
        f32 z2 = gabi::fmuls_ppc(zz, z);
        f32 q = record->mRotation.w;
        f32 c = gabi::fnmsubs(yy, y, one);
        f32 x = record->mRotation.x;
        f32 qq = q + q;
        f32 xx = x + x;
        f32 m00 = c - z2;
        f32 yz = gabi::fmuls_ppc(qq, y);
        f32 c2 = gabi::fnmsubs(xx, x, one);
        f32 m02 = gabi::fmadds(xx, z, yz);
        f32 xz = gabi::fmuls_ppc(xx, y);
        f32 weight = weights[i];
        f32 qz = gabi::fmuls_ppc(qq, z);
        f32 m11 = c2 - z2;
        f32 m01 = xz - qz;
        f32 xy = gabi::fmuls_ppc(yy, z);
        f32 m10 = xz + qz;
        f32 m12 = gabi::fnmsubs(qq, x, xy);
        ExtBlendResult* p  =  &out[flag & ExtBlend_BindingIndex];
        f32 oldRotation00 = f32(p->mRotation[0][0]);
        f32 oldRotation01 = f32(p->mRotation[0][1]);
        f32 oldRotation02 = f32(p->mRotation[0][2]);
        f32 oldRotation10 = f32(p->mRotation[1][0]);
        p->mRotation[0][0]  =  gabi::fmadds(m00, weight, oldRotation00);
        p->mRotation[0][1]  =  gabi::fmadds(m01, weight, oldRotation01);
        f32 oldRotation11 = f32(p->mRotation[1][1]);
        f32 oldRotation12 = f32(p->mRotation[1][2]);
        p->mRotation[0][2]  =  gabi::fmadds(m02, weight, oldRotation02);
        p->mRotation[1][0]  =  gabi::fmadds(m10, weight, oldRotation10);
        f32 oldScaleX = f32(p->mScale[0]);
        f32 oldScaleY = f32(p->mScale[1]);
        p->mRotation[1][1]  =  gabi::fmadds(m11, weight, oldRotation11);
        p->mRotation[1][2]  =  gabi::fmadds(m12, weight, oldRotation12);
        f32 scaleX = record->mScale[0];
        f32 scaleY = record->mScale[1];
        weight = weights[i];
        f32 oldWeight = f32(p->mWeight);
        f32 scaleZ = record->mScale[2];
        f32 translateX = record->mTranslation[0];
        f32 oldScaleZ = f32(p->mScale[2]);
        f32 oldTranslateX = f32(p->mTranslation[0]);
        p->mScale[0]  =  gabi::fmadds(scaleX, weight, oldScaleX);
        p->mScale[1]  =  gabi::fmadds(scaleY, weight, oldScaleY);
        f32 translateY = record->mTranslation[1];
        f32 translateZ = record->mTranslation[2];
        f32 oldTranslateY = f32(p->mTranslation[1]);
        f32 oldTranslateZ = f32(p->mTranslation[2]);
        p->mScale[2]  =  gabi::fmadds(scaleZ, weight, oldScaleZ);
        p->mTranslation[0]  =  gabi::fmadds(translateX, weight, oldTranslateX);
        u32 oldflags = u32(p->mFlags), newflags = u32(record->mFlags);
        p->mTranslation[1]  =  gabi::fmadds(translateY, weight, oldTranslateY);
        p->mTranslation[2]  =  gabi::fmadds(translateZ, weight, oldTranslateZ);
        p->mWeight  =  oldWeight + weight;
        p->mFlags  =  oldflags | ~newflags;
    }
}

/* 025F0148 */
// HD: free through the parent heap, destroying it when it is a solid heap.
void extDestroyParentSolidHeap(ExtHeap* heap) {
    if (heap == NULL) return;
    ExtHeap* parent = extGetParentHeap(heap);
    u32 kind = extGetHeapKind(parent);
    if (kind == ExtHeap_SolidKind) extDestroySolidHeap(parent);
    else {
        gfn* vtable = parent->mVtable;
        gabi::call_ptr<void>(vtable[ExtHeap_FreeSlot].get(), parent, heap);
    }
}

/* 025F01D8 */
// HD: explicit GHS constructor entry may allocate its four-byte scope object.
ExtSolidHeapScope* extSolidHeapScope__ct(ExtSolidHeapScope* self, u32 tag, u32 size, u32 align) {
    if (self == NULL) {
        self = extAllocateScope(sizeof(ExtSolidHeapScope));
        if (self == NULL) return NULL;
    }
    if (align == 0) align = ExtHeap_DefaultAlignment;
    ExtHeap* heap = extCreateSolidHeap(size, NULL, align);
    self->mpHeap = heap;
    if (heap != NULL && tag != 0) heap->mDebugName = tag;
    return self;
}

/* 025F0270 */
// HD: scope destructor adjusts its heap; explicit destroy is separate.
void extSolidHeapScope__dt(ExtSolidHeapScope* self, u32 flags) {
    if (self == NULL) return;
    if (self->mpHeap.get() != NULL) {
        extRestoreCurrentHeap(self);
        extAdjustSolidHeap(self->mpHeap);
    }
    if (flags & ExtHeap_DeleteStorage) extDeleteScope(self);
}

/* 025F02D0 */
void ExtSolidHeapScope::destroy() {
    if (mpHeap.get() != NULL) {
        extRestoreCurrentHeap(this);
        extDestroySolidHeap(mpHeap);
        mpHeap = NULL;
    }
}

/* 025F0318 */
static void __sinit_m_Do_ext_completeness_cpp() {
    // HD: common-header statics emitted at the end of this unit.
    sinit_header_statics_rodata(0x1048CF4C, 0x101F47BC, 0x1048CF58, 0x10058E84);
}

/* WWHD glue
data blendOne 100586D4
ext 027EC124 ExtHeap* extGetParentHeap(ExtHeap*)
ext 027EC23C u32 extGetHeapKind(ExtHeap*)
ext 025E3868 void extDestroySolidHeap(ExtHeap*)
ext 0273AD10 ExtSolidHeapScope* extAllocateScope(u32)
ext 025E35BC ExtHeap* extCreateSolidHeap(u32, ExtHeap*, u32)
ext 025E37D8 void extRestoreCurrentHeap(ExtSolidHeapScope*)
ext 025E3678 void extAdjustSolidHeap(ExtHeap*)
ext 0273AF40 void extDeleteScope(ExtSolidHeapScope*)
*/
