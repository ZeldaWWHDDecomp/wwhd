/** HD replacement material setup and draw wrappers.
 * tww d_material.cpp supplies the draw-list naming.
 */
#include "d/d_material.h"
#include "gc_api/JSystem/JUTAssert.h"
using namespace material_hd;
using HeapPtr = gptr<Heap>;
using ControlPtr = gptr<Control>;
using StatePtr = gptr<ExtendedState>;
using VoidPtr = gptr<void>;
static HeapPtr mHeap;
static ControlPtr mIce;
static StatePtr mOverride0;
static StatePtr mOverride1;
static VoidPtr resourceControl;
static u32 drawBufferOpa;
static u32 drawBufferXlu;

static void transfer(ExtendedState* dst, State* src) {
    State* state = &dst->base;
    bool same = state->field_0x4 == src->field_0x4 &&
                state->field_0x8 == src->field_0x8 &&
                state->field_0xc == src->field_0xc &&
                state->field_0x10 == src->field_0x10 &&
                state->field_0x14 == src->field_0x14 &&
                state->field_0x18 == src->field_0x18 &&
                state->field_0x38 == src->field_0x38 &&
                state->field_0x34 == src->field_0x34 &&
                state->field_0x1c == src->field_0x1c;
    if (!same) {
        copyState(dst, src);
    } else {
        u32 first = src->field_0x28;
        u32 second = src->field_0x30;
        state->field_0x28 = first;
        dst->field_0xdc = second;
        dst->field_0xd4 = first;
        state->field_0x30 = second;
    }
}

/* 02590FB8 */
static void dMat_setup(Control* self) {
    String first;
    String second;
    first.mStringTop = 0x10050fe4;
    first.__vtbl = 0x10050fcc;
    void* resources = resourceControl;
    Resource* a = getResource(resources, &first, 0x92);
    second.__vtbl = 0x10050fcc;
    resources = resourceControl;
    second.mStringTop = 0x10050fe4;
    Resource* b = getResource(resources, &second, 0x93);
    if (!a || !b)
        return;
    initTexture(&self->source[0], a->field_0x20);
    initTexture(&self->source[1], b->field_0x20);
    transfer(&self->target[0], &self->source[0]);
    transfer(&self->target[1], &self->source[1]);
    mOverride1 = &self->target[1];
    mOverride0 = &self->target[0];
}

/* 02591200 */
static void dMat_ice_updateDL(Model* model, s8 backupNo, void* invisible) {
    // HD: backupNo is unused; the model flag selects the replacement material.
    u32 oldOpa = drawBufferOpa;
    u32 oldXlu = drawBufferXlu;
    auto* play = reinterpret_cast<Play*>(dComIfGp_get());
    drawBufferOpa = play->mDrawBufferOpa;
    play = reinterpret_cast<Play*>(dComIfGp_get());
    drawBufferXlu = play->mDrawBufferXlu;
    Model* actual = model;
    actual->mMaterialOverride = 1;
    updateModel(model, 0);
    actual = model;
    actual->mMaterialOverride = 0;
    if (invisible)
        entryInvisible(invisible);
    drawBufferOpa = oldOpa;
    drawBufferXlu = oldXlu;
}

/* 02591284 */
static void dMat_ice_updateMorfDL(Morf* model, s8 backupNo, void* invisible) {
    // HD: backupNo is unused; the model flag selects the replacement material.
    u32 oldOpa = drawBufferOpa;
    u32 oldXlu = drawBufferXlu;
    auto* play = reinterpret_cast<Play*>(dComIfGp_get());
    drawBufferOpa = play->mDrawBufferOpa;
    play = reinterpret_cast<Play*>(dComIfGp_get());
    drawBufferXlu = play->mDrawBufferXlu;
    Model* actual = model->mpModel;
    actual->mMaterialOverride = 1;
    updateMorf(model);
    actual = model->mpModel;
    actual->mMaterialOverride = 0;
    if (invisible)
        entryInvisible(invisible);
    drawBufferOpa = oldOpa;
    drawBufferXlu = oldXlu;
}

/* 0259130C */
static void dMat_ice_entryDL(Model* model, s8 backupNo, void* invisible) {
    // HD: backupNo is unused; the model flag selects the replacement material.
    u32 oldOpa = drawBufferOpa;
    u32 oldXlu = drawBufferXlu;
    auto* play = reinterpret_cast<Play*>(dComIfGp_get());
    drawBufferOpa = play->mDrawBufferOpa;
    play = reinterpret_cast<Play*>(dComIfGp_get());
    drawBufferXlu = play->mDrawBufferXlu;
    Model* actual = model;
    actual->mMaterialOverride = 1;
    entryModel(model);
    actual = model;
    actual->mMaterialOverride = 0;
    if (invisible)
        entryInvisible(invisible);
    drawBufferOpa = oldOpa;
    drawBufferXlu = oldXlu;
}

/* 0259138C */
static void dMat_ice_entryMorfDL(Morf* model, s8 backupNo, void* invisible) {
    // HD: backupNo is unused; the model flag selects the replacement material.
    u32 oldOpa = drawBufferOpa;
    u32 oldXlu = drawBufferXlu;
    auto* play = reinterpret_cast<Play*>(dComIfGp_get());
    drawBufferOpa = play->mDrawBufferOpa;
    play = reinterpret_cast<Play*>(dComIfGp_get());
    drawBufferXlu = play->mDrawBufferXlu;
    Model* actual = model->mpModel;
    actual->mMaterialOverride = 1;
    entryMorf(model);
    actual = model->mpModel;
    actual->mMaterialOverride = 0;
    if (invisible)
        entryInvisible(invisible);
    drawBufferOpa = oldOpa;
    drawBufferXlu = oldXlu;
}

/* 02591414 */
static void dMat_ice_entryMorfDLAlternate(Morf* model, s8 backupNo, void* invisible) {
    // HD: backupNo is unused; the model flag selects the replacement material.
    u32 oldOpa = drawBufferOpa;
    u32 oldXlu = drawBufferXlu;
    auto* play = reinterpret_cast<Play*>(dComIfGp_get());
    drawBufferOpa = play->mDrawBufferOpa;
    play = reinterpret_cast<Play*>(dComIfGp_get());
    drawBufferXlu = play->mDrawBufferXlu;
    Model* actual = model->mpModel;
    actual->mMaterialOverride = 1;
    entryMorf(model);
    actual = model->mpModel;
    actual->mMaterialOverride = 0;
    if (invisible)
        entryInvisibleAlternate(invisible);
    drawBufferOpa = oldOpa;
    drawBufferXlu = oldXlu;
}

/* 0259149C */
static void dMat_control_create(Heap* parentHeap) {
    // HD: named heap and combined 0x450-byte render-state allocation replace GC backups.
    String name;
    name.__vtbl = 0x10050fcc;
    name.mStringTop = 0x10050fec;
    Heap* heap = createHeap(0x100000, &name, parentHeap, 1, 0);
    mHeap = heap;
    Control* control = allocateControl(sizeof(Control), heap, 4);
    if (control) {
        initState(&control->source[0]);
        initState(&control->source[1]);
        initExtendedState(&control->target[0]);
        initExtendedState(&control->target[1]);
    }
    mIce = control;
    if (!control) {
        JUTAssertion_showAssert("d_material.cpp", 0x140, "mIce != (0)");
        control = mIce;
    }
    dMat_setup(control);
    heap = mHeap;
    heap->adjustSize();
}

/* 02591588 */
static void dMat_control_remove() {
    mOverride0 = nullptr;
    Heap* heap = mHeap;
    mOverride1 = nullptr;
    mIce = nullptr;
    if (heap) {
        heap->destroy();
        mHeap = nullptr;
    }
}

/* 025915F0 */
static void __sinit_d_material_cpp() {
    sinit_header_statics(0x1047a970, 0x101ea000);
}

/* 02591684 */
static void SafeString_deletingDtor_d_material(String* self, u32 flags) {
    if (self && (flags & 1))
        operator_delete(self);
}

/* 02591698 */
static void SafeString_assureTerminationImpl_d_material(String*) {}

/* WWHD glue (tools/verify/autoglue.py)
data mHeap 101EA028
data mIce 101EA024
data mOverride0 104B45C0
data mOverride1 104B45C4
data resourceControl 101F4F28
data drawBufferOpa 104B4634
data drawBufferXlu 104B4638
str 10051008
str 10050FFC
ext 027BDEB4 void copyState(material_hd::ExtendedState* dst, material_hd::State* src)
ext 02773798 void initTexture(material_hd::State* state, void* texture)
ext 027BE6B8 void initState(material_hd::State* state)
ext 027BDF7C void initExtendedState(material_hd::ExtendedState* state)
ext 026066C4 material_hd::Resource* getResource(void* resources, material_hd::String* name, u32 index)
ext 02754B38 material_hd::Heap* createHeap(u32 size, material_hd::String* name, material_hd::Heap* parent, u32 direction, u32 flags)
ext 0273B050 material_hd::Control* allocateControl(u32 size, material_hd::Heap* heap, s32 alignment)
ext 025E2DE0 void updateModel(material_hd::Model* model, u32 flags)
ext 025E2E5C void entryModel(material_hd::Model* model)
ext 025E54D8 void updateMorf(material_hd::Morf* morf)
ext 025E5590 void entryMorf(material_hd::Morf* morf)
ext 025E8BC4 void entryInvisible(void* invisible)
ext 025E8EC0 void entryInvisibleAlternate(void* invisible)
*/
