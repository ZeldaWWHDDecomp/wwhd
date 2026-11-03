// agl depth of field: the translation unit's template and inline instances (sead::SafeString family,
// agl::utl::Parameter<T> accessors, IParameterObj hooks) and the vignetting-shape mesh's construction.
#include "bindings.h"
#include "agl/DepthOfFieldDraw.h"

using namespace agl::dof;

static u32 sTextBufferMethods;
static u32 sFixedTextMethods;
static u32 sFormatTextMethods;
static sead::DisposerVtable sOwnerMethods;
static u32 sOwnerSecondMethods;
static void* allocateObject(u32 size);
static void constructVertexBuffer(agl::VertexBufferLayout* buffer);
static void constructVertexAttribute(agl::VertexInputSet* attribute);
static void constructIndexStream(agl::IndexBufferLayout* stream);
static void destroyVertexBuffer(agl::VertexBufferLayout* buffer, u32 flags);
static void destroyVertexAttribute(agl::VertexInputSet* attribute, u32 flags);
static void destroyIndexStream(agl::IndexBufferLayout* stream, u32 flags);
static void finalizeParameterOwner(agl::ResourceRecord* owner);
static void destroyDisposer(agl::ResourceRecord* owner, u32 flags);

/* 02782804 */
void Text::destroy(Text* text, u32 flags) {
    if (text != NULL && (flags & 1)) operator_delete(text);
}
/* 02782818 */
void FixedText32::destroy(FixedText32* text, u32 flags) {
    if (text != NULL && (flags & 1)) operator_delete(text);
}
/* 02782898 */
void Text::assureTerminationImpl() {}
/* 0278289C */
void TextBuffer::assureTerminationImpl() {
    be<u8>* text = mText;
    s32 capacity = mCapacity;
    gabi::at<be<u8>>(gabi::ea(text) + capacity - 1)[0] = 0;
}
/* 027828B4 */
void TextBuffer::destroy(TextBuffer* text, u32 flags) {
    if (text != NULL && (flags & 1)) operator_delete(text);
}
/* 027828C8 */
void FixedTextB::destroy(FixedTextB* text, u32 flags) {
    if (text != NULL && (flags & 1)) operator_delete(text);
}

/* 027828DC */
// sead::FormatFixedSafeString<32>: GHS saves the integer (and, when cr1.eq says so, the float) argument
// registers before the allocation, then formats from the variadic state.
FormatText32* FormatText32::create(FormatText32* text, const char* format, u32 first, u32 second, u32 third,
                                   u32 fourth, u32 fifth, u32 sixth) {
    u32 entryStack = gabi::cpu->r[1];
    bool floatingArguments = gabi::cpu->cr[6] != 0;
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<agl::CascadeFormatSave> save(8);
    save->mIntegerArguments[0] = gabi::ea(text);
    save->mIntegerArguments[1] = gabi::ea(format);
    save->mIntegerArguments[2] = first;
    save->mIntegerArguments[3] = second;
    save->mIntegerArguments[4] = third;
    save->mIntegerArguments[5] = fourth;
    save->mIntegerArguments[6] = fifth;
    save->mIntegerArguments[7] = sixth;
    if (floatingArguments) {
        for (u32 index = 0; index < 8; ++index) {
            u64 bits;
            memcpy(&bits, &gabi::cpu->f[index + 1].ps0, sizeof(bits));
            save->mFloatingArgumentWords[index * 2] = bits >> 32;
            save->mFloatingArgumentWords[index * 2 + 1] = (u32)bits;
        }
    }
    if (text == NULL) text = (FormatText32*)allocateObject(sizeof(FormatText32));
    if (text == NULL) return text;
    TextBuffer* base = text;
    if (base == NULL) base = (TextBuffer*)allocateObject(sizeof(sead::SafeTextBuffer));
    if (base != NULL) {
        base->mText = text->mStorage;
        base->mVtable = gabi::at<sead::SafeTextVtable>(gabi::ea(&sTextBufferMethods));
        base->mCapacity = 32;
        text->mStorage[31] = 0;
    }
    be<u8>* storage = text->mText;
    text->mVtable = gabi::at<sead::SafeTextVtable>(gabi::ea(&sFixedTextMethods));
    *storage = 0;
    text->mVtable = gabi::at<sead::SafeTextVtable>(gabi::ea(&sFormatTextMethods));
    // The other two bytes of the packed register-state word are untouched GHS padding.
    auto* indexes = gabi::at<be<u8>>(gabi::ea(&save->mArguments.mRegisterState));
    indexes[0] = 2;
    save->mArguments.mOverflowArguments = gabi::at<void>(entryStack + 8);
    save->mArguments.mRegisterArguments = &save->mIntegerArguments[0];
    indexes[1] = 0;
    reinterpret_cast<sead::format::ByteTextBuffer*>(text)->formatArguments(format, &save->mArguments);
    return text;
}

/* 027829FC */
void FormatText32::destroy(FormatText32* text, u32 flags) {
    if (text != NULL && (flags & 1)) operator_delete(text);
}

// ---- agl::utl::Parameter<T> (bool, s32, f32, Vector2f, Vector4f, Color4f) ----
/* 02782A10 */
void BoolParameter::postApply() {}
/* 02782A14 */
u32 BoolParameter::type() const { return agl::ParameterType_Bool; }
/* 02782A1C */
be<u8>* BoolParameter::ptr() { return &mValue; }
/* 02782A24 */
const be<u8>* BoolParameter::constPtr() const { return &mValue; }
/* 02782A2C */
u32 BoolParameter::size() const { return sizeof(u8); }
/* 02782A34 */
u32 IntParameter::type() const { return agl::ParameterType_Int; }
/* 02782A3C */
be<s32>* IntParameter::ptr() { return &mValue; }
/* 02782A44 */
const be<s32>* IntParameter::constPtr() const { return &mValue; }
/* 02782A4C */
u32 IntParameter::size() const { return sizeof(s32); }
/* 02782A54 */
u32 F32Parameter::type() const { return agl::ParameterType_F32; }
/* 02782A5C */
be<f32>* F32Parameter::ptr() { return &mValue; }
/* 02782A64 */
const be<f32>* F32Parameter::constPtr() const { return &mValue; }
/* 02782A6C */
u32 F32Parameter::size() const { return sizeof(f32); }
/* 02782A74 */
u32 Vec2Parameter::type() const { return agl::ParameterType_Vec2; }
/* 02782A7C */
be<f32>* Vec2Parameter::ptr() { return &x; }
/* 02782A84 */
const be<f32>* Vec2Parameter::constPtr() const { return &x; }
/* 02782A8C */
u32 Vec2Parameter::size() const { return 2 * sizeof(f32); }
/* 02782A94 */
u32 Vec4Parameter::type() const { return agl::ParameterType_Vec4; }
/* 02782A9C */
be<f32>* Vec4Parameter::ptr() { return &mValue.r; }
/* 02782AA4 */
const be<f32>* Vec4Parameter::constPtr() const { return &mValue.r; }
/* 02782AAC */
u32 Vec4Parameter::size() const { return 4 * sizeof(f32); }
/* 02782AB4 */
u32 ColorParameter::type() const { return agl::ParameterType_Color; }
/* 02782ABC */
be<f32>* ColorParameter::ptr() { return &mValue.r; }
/* 02782AC4 */
const be<f32>* ColorParameter::constPtr() const { return &mValue.r; }
/* 02782ACC */
u32 ColorParameter::size() const { return 4 * sizeof(f32); }

// ---- agl::utl::IParameterObj hooks (default answers and the name-hash checks) ----
/* 02782AD4 */
u32 ParameterObjectHooks::isApplicable() const { return 1; }
/* 02782ADC */
u32 ParameterObjectHooks::isApplicableSecond() const { return 1; }
/* 02782AE4 */
void ParameterObjectHooks::preWrite() {}
/* 02782AE8 */
u32 ParameterObjectHooks::matchesName(const gptr<agl::parameter_tree::MatchRecord>* record) const {
    return (u32)(*record)->mComparisonValue == (u32)mNameHash;
}
/* 02782B04 */
u32 ParameterListHooks::isApplicableThird() const { return 1; }
/* 02782B0C */
u32 ParameterListHooks::matchesSecondName(const gptr<agl::parameter_tree::MatchRecord>* record) const {
    return (u32)(*record)->mSecondaryValue == (u32)mNameHash;
}
/* 02782B28 */
void ParameterListHooks::postWrite() {}
/* 02782B2C */
void ParameterListHooks::preRead() {}
/* 02782B30 */
void ParameterListHooks::postReadSecond() {}

/* 02782B34 */
void ParameterOwner::destroy(ParameterOwner* owner, u32 flags) {
    if (owner == NULL) return;
    owner->mVtable = &sOwnerMethods;
    owner->mInterfaceMethods = &sOwnerSecondMethods;
    finalizeParameterOwner(owner);
    destroyDisposer(owner, 0);
    if (flags & 1) operator_delete(owner);
}

/* 02782BA8 */
u32 ContextHost::isApplicable() const { return 1; }
/* 02782BB0 */
void ContextHost::postApply() {}

/* 02782BB4 */
VignettingMesh* VignettingMesh::construct(VignettingMesh* mesh) {
    if (mesh == NULL) mesh = (VignettingMesh*)allocateObject(sizeof(VignettingMesh));
    if (mesh == NULL) return mesh;
    mesh->mIndices = NULL;
    mesh->mIndexCapacity = 0;
    mesh->mVertices = NULL;
    mesh->mVertexCapacity = 0;
    constructVertexBuffer(&mesh->mVertexBuffer);
    constructVertexAttribute(&mesh->mVertexBindings);
    constructIndexStream(&mesh->mIndexBuffer);
    return mesh;
}

/* 0278282C */
void VignettingMesh::destroy(VignettingMesh* mesh, u32 flags) {
    if (mesh == NULL) return;
    destroyIndexStream(&mesh->mIndexBuffer, 2);
    destroyVertexAttribute(&mesh->mVertexBindings, 2);
    destroyVertexBuffer(&mesh->mVertexBuffer, 2);
    if (flags & 1) operator_delete(mesh);
}

/* WWHD glue
 data sTextBufferMethods 10162630
 data sFixedTextMethods 10162668
 data sFormatTextMethods 101626E0
 data sOwnerMethods 101626A8
 data sOwnerSecondMethods 101626B8
 ext 0273AD10 void* allocateObject(u32)
 ext 02759C10 s32 sead::format::ByteTextBuffer::formatArguments(const char*, sead::format::VariadicState*)
 ext 027B5BD8 void constructVertexBuffer(agl::VertexBufferLayout*)
 ext 027BF734 void constructVertexAttribute(agl::VertexInputSet*)
 ext 027B5430 void constructIndexStream(agl::IndexBufferLayout*)
 ext 027B5CBC void destroyVertexBuffer(agl::VertexBufferLayout*, u32)
 ext 027BF880 void destroyVertexAttribute(agl::VertexInputSet*, u32)
 ext 027B54A0 void destroyIndexStream(agl::IndexBufferLayout*, u32)
 ext 027A0998 void finalizeParameterOwner(agl::ResourceRecord*)
 ext 02752BEC void destroyDisposer(agl::ResourceRecord*, u32)
*/
