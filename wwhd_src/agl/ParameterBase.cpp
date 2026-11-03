// agl parameter base: name hashing, construction, typed copies and the application of parameter
// resources (whole, or added with a weight for blending), plus the text copies of this translation unit.
// Written from WWHD's instructions; type ids: AAMP format facts (public format documentation).
#include "agl/ParameterBase.h"
using namespace agl;

static void* allocateObject(u32 size);
static void releaseObject(void* object);
static u32 hashBytes(const be<u8>* bytes, u32 length);
static void appendToOwner(parameter_tree::Value* owner, ParameterBase* parameter);
static void copyBytes(void* destination, const void* source, u32 size, u32 flags);
static void multiplyColor(ParameterFloats<4>* result, const ParameterFloats<4>* color, const ParameterFloats<4>* factors);
static void addColor(ParameterFloats<4>* color, const ParameterFloats<4>* addend);
static ParameterMethods sParameterBaseMethods;
static sead::SafeTextVtable sTextMethods;

/* inlined into the name hash: SafeString length scan, 0 for an unterminated text */
static s32 textLength(const be<u8>* text) {
    s32 length = 0;
    while (text[length] != 0) {
        ++length;
        if (length > sead::kMaxTextLength) return 0;
    }
    return length;
}

/* 0279E57C */
u32 ParameterBase::hashName(sead::SafeText* name) {
    gabi::AutoFrame frame{gabi::current_function()};
    // Two inlined cstr() calls: the first gives the hashed bytes, the second the scanned length.
    name->assureTermination();
    u32 terminate = name->mVtable->mAssureTermination.mFunction;
    const be<u8>* text = name->mText;
    gabi::call_ptr<void>(terminate, name);
    s32 length = textLength(name->mText);
    return hashBytes(text, length);
}

/* 0279E618 */
void ParameterBase::initialize(sead::SafeText* name, sead::SafeText* label, sead::SafeText* meta, parameter_tree::Value* owner) {
    gabi::AutoFrame frame{gabi::current_function()};
    mNext = nullptr;
    mNameHash = hashName(name);
    if (owner != nullptr) appendToOwner(owner, this);
}

/* 0279E674 */
ParameterBase* ParameterBase::create(ParameterBase* storage) {
    gabi::AutoFrame frame{gabi::current_function()};
    ParameterBase* parameter = storage;
    if (parameter == nullptr) parameter = static_cast<ParameterBase*>(allocateObject(sizeof(ParameterBase)));
    if (parameter == nullptr) return parameter;
    parameter->mMethods = reinterpret_cast<sead::VtableSlot*>(&sParameterBaseMethods);
    gabi::FrameLocal<sead::SafeText> name(8), label(0x10), meta(0x18);
    name->mText.v = gabi::guest_str("default");
    name->mVtable = &sTextMethods;
    label->mText.v = gabi::guest_str("parameter");
    label->mVtable = &sTextMethods;
    meta->mText.v = gabi::guest_str("");
    meta->mVtable = &sTextMethods;
    parameter->initialize(name, label, meta, nullptr);
    return parameter;
}

/* 0279E710 */
ParameterBase* ParameterBase::create(ParameterBase* storage, sead::SafeText* name, sead::SafeText* label, parameter_tree::Value* owner) {
    gabi::AutoFrame frame{gabi::current_function()};
    ParameterBase* parameter = storage;
    if (parameter == nullptr) parameter = static_cast<ParameterBase*>(allocateObject(sizeof(ParameterBase)));
    if (parameter == nullptr) return parameter;
    parameter->mMethods = reinterpret_cast<sead::VtableSlot*>(&sParameterBaseMethods);
    gabi::FrameLocal<sead::SafeText> meta(8);
    meta->mText.v = gabi::guest_str("");
    meta->mVtable = &sTextMethods;
    parameter->initialize(name, label, meta, owner);
    return parameter;
}

/* 0279E7B0 */
u32 ParameterBase::copy(ParameterBase* source) {
    gabi::AutoFrame frame{gabi::current_function()};
    ParameterMethods* sourceMethods = source->methods();
    u32 type = valueKind();
    u32 sourceType = gabi::call_ptr<u32>(sourceMethods->mValueKind.mFunction, source);
    if (type != sourceType || static_cast<u32>(mNameHash) != static_cast<u32>(source->mNameHash)) return 0;
    gabi::call_ptr<void>(methods()->mCopyUnchecked.mFunction, this, source);
    return 1;
}

/* 0279E868 */
void ParameterBase::copyUnchecked(ParameterBase* source) {
    gabi::AutoFrame frame{gabi::current_function()};
    be<u8>* destination = static_cast<be<u8>*>(value());
    const be<u8>* bytes = static_cast<const be<u8>*>(source->constValue());
    s32 size = valueBytes();
    for (s32 index = 0; index < size; ++index) destination[index] = static_cast<u8>(bytes[index]);
}

/* 0279E904 */
u32 ParameterBase::makeZero() {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 type = valueKind();
    if (type != ParameterType_F32 && (type < ParameterType_Vec2 || type > ParameterType_Color)) return 0;
    be<u32>* words = static_cast<be<u32>*>(value());
    // Stores of the float constant 0.0: the words become +0.0.
    switch (type) {
    case ParameterType_F32: words[0] = 0; break;
    case ParameterType_Vec2: words[1] = 0; words[0] = 0; break;
    case ParameterType_Vec3: words[0] = 0; words[1] = 0; words[2] = 0; break;
    case ParameterType_Vec4: words[2] = 0; words[0] = 0; words[3] = 0; words[1] = 0; break;
    case ParameterType_Color: words[2] = 0; words[3] = 0; words[1] = 0; words[0] = 0; break;
    }
    return 1;
}

/* 0279EACC */
u32 ParameterBase::isFloatType() {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 type = valueKind();
    return type == ParameterType_F32 || (type >= ParameterType_Vec2 && type <= ParameterType_Color);
}

/* 0279EB28 */
void ParameterBase::applyResource(gptr<ParameterRecord>* record) {
    gabi::AutoFrame frame{gabi::current_function()};
    ParameterRecord* input = *record;
    u32 type = valueKind();
    be<u8>* destination = static_cast<be<u8>*>(value());
    if (type != ParameterType_Bool) {
        be<u8>* data = input->mData;
        u32 available = input->payloadSize();
        u32 capacity = valueBytes();
        copyBytes(destination, data, available < capacity ? available : capacity, 0);
        notifyResourceApplied(data, input->payloadSize());
    } else {
        destination[0] = input->mData[0] != 0;
        notifyResourceApplied(input->mData, input->payloadSize());
    }
}

/* inlined into the weighted resource application: fmadds with the unrounded weight */
static f32 addWeighted(f64 value, f64 weight, f64 previous) {
    return static_cast<f32>(gabi::ppc_nan3(value, previous, weight,
        value * round25(weight) + previous, gabi::ppc_dnan64()));
}

/* 0279EC1C */
void ParameterBase::addWeightedResource(gptr<ParameterRecord>* record, f64 weight) {
    gabi::AutoFrame frame{gabi::current_function()};
    ParameterRecord* input = *record;
    switch (valueKind()) {
    case ParameterType_Bool: {
        be<u8>* destination = static_cast<be<u8>*>(value());
        destination[0] = input->mData[0] != 0;
        break;
    }
    case ParameterType_F32: {
        u32 size = valueBytes();
        gabi::FrameLocal<be<f32>> payload(0x40);
        copyBytes(payload, input->mData, size, 0);
        f32 addend = *payload;
        be<f32>* destination = static_cast<be<f32>*>(value());
        f32 previous = destination[0];
        destination[0] = addWeighted(addend, weight, previous);
        break;
    }
    case ParameterType_Vec2: {
        u32 size = valueBytes();
        gabi::FrameLocal<ParameterFloats<2>> payload(0x58), product(8), scaled(0x50);
        copyBytes(payload, input->mData, size, 0);
        f32 x = payload->mValues[0];
        f32 y = payload->mValues[1];
        product->mValues[0] = gabi::fmuls_fpr(x, weight);
        product->mValues[1] = gabi::fmuls_fpr(y, weight);
        scaled->mWords[0] = static_cast<u32>(product->mWords[0]);
        scaled->mWords[1] = static_cast<u32>(product->mWords[1]);
        be<f32>* destination = static_cast<be<f32>*>(value());
        f32 oldX = destination[0];
        f32 addX = scaled->mValues[0];
        f32 oldY = destination[1];
        destination[0] = gabi::fadds_ppc(oldX, addX);
        f32 addY = scaled->mValues[1];
        destination[1] = gabi::fadds_ppc(oldY, addY);
        break;
    }
    case ParameterType_Vec3: {
        u32 size = valueBytes();
        gabi::FrameLocal<ParameterFloats<3>> payload(0x8C), product(0x44), scaled(0x80);
        copyBytes(payload, input->mData, size, 0);
        f32 x = payload->mValues[0], y = payload->mValues[1], z = payload->mValues[2];
        product->mValues[0] = gabi::fmuls_fpr(x, weight);
        product->mValues[1] = gabi::fmuls_fpr(y, weight);
        product->mValues[2] = gabi::fmuls_fpr(z, weight);
        scaled->mWords[1] = static_cast<u32>(product->mWords[1]);
        scaled->mWords[2] = static_cast<u32>(product->mWords[2]);
        scaled->mWords[0] = static_cast<u32>(product->mWords[0]);
        be<f32>* destination = static_cast<be<f32>*>(value());
        f32 addX = scaled->mValues[0], addY = scaled->mValues[1];
        f32 oldX = destination[0], oldY = destination[1], oldZ = destination[2];
        destination[0] = gabi::fadds_ppc(oldX, addX);
        destination[1] = gabi::fadds_ppc(oldY, addY);
        f32 addZ = scaled->mValues[2];
        destination[2] = gabi::fadds_ppc(oldZ, addZ);
        break;
    }
    case ParameterType_Vec4: {
        u32 size = valueBytes();
        gabi::FrameLocal<ParameterFloats<4>> payload(0x70), product(0x20), scaled(0x60);
        copyBytes(payload, input->mData, size, 0);
        f32 y = payload->mValues[1], x = payload->mValues[0];
        f32 productY = gabi::fmuls_fpr(y, weight);
        f32 productX = gabi::fmuls_fpr(x, weight);
        f32 z = payload->mValues[2], w = payload->mValues[3];
        product->mValues[0] = productX;
        f32 productZ = gabi::fmuls_fpr(z, weight);
        product->mValues[1] = productY;
        f32 productW = gabi::fmuls_fpr(w, weight);
        product->mValues[2] = productZ;
        scaled->mWords[0] = static_cast<u32>(product->mWords[0]);
        product->mValues[3] = productW;
        scaled->mWords[1] = static_cast<u32>(product->mWords[1]);
        scaled->mWords[3] = static_cast<u32>(product->mWords[3]);
        scaled->mWords[2] = static_cast<u32>(product->mWords[2]);
        be<f32>* destination = static_cast<be<f32>*>(value());
        f32 addX = scaled->mValues[0], oldX = destination[0];
        f32 sumX = gabi::fadds_ppc(oldX, addX);
        f32 oldY = destination[1];
        destination[0] = sumX;
        f32 addY = scaled->mValues[1], sumY = gabi::fadds_ppc(oldY, addY);
        f32 oldZ = destination[2];
        destination[1] = sumY;
        f32 addZ = scaled->mValues[2], sumZ = gabi::fadds_ppc(oldZ, addZ);
        f32 oldW = destination[3];
        destination[2] = sumZ;
        f32 addW = scaled->mValues[3];
        destination[3] = gabi::fadds_ppc(oldW, addW);
        break;
    }
    case ParameterType_Color: {
        gabi::FrameLocal<ParameterFloats<4>> color(0x10), factors(0x30), scaled(0x98);
        color->mValues[1] = 0;
        color->mValues[3] = 1;
        color->mValues[0] = 0;
        color->mValues[2] = 0;
        u32 size = valueBytes();
        copyBytes(color, input->mData, size, 0);
        ParameterMethods* table = methods();
        // Plain stfs stores of the incoming weight register: no arithmetic rounding step.
        u32 weightBits = ppc_stfs_bits(weight);
        factors->mWords[0] = weightBits;
        factors->mWords[3] = weightBits;
        factors->mWords[1] = weightBits;
        factors->mWords[2] = weightBits;
        multiplyColor(scaled, color, factors);
        ParameterFloats<4>* destination = gabi::call_ptr<ParameterFloats<4>*>(table->mValue.mFunction, this);
        addColor(destination, scaled);
        break;
    }
    case ParameterType_Int: case ParameterType_String32: case ParameterType_String64: case ParameterType_String256: {
        void* destination = value();
        u32 size = valueBytes();
        copyBytes(destination, input->mData, size, 0);
        break;
    }
    case ParameterType_BufferInt: case ParameterType_BufferF32: {
        void* destination = value();
        u32 available = input->payloadSize();
        u32 capacity = valueBytes();
        copyBytes(destination, input->mData, available < capacity ? available : capacity, 0);
        break;
    }
    }
    notifyResourceApplied(input->mData, input->payloadSize());
}

/* HD: compiler-generated and template code of this translation unit */

/* 0279F1A8 */
void parameter_text::Text::destroy(u32 flags) {
    if (gabi::ea(this) != 0 && (flags & 1)) gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseObject), this);
}

/* 0279F1BC */
void parameter_text::Text::assureTerminationImpl() {}

/* 0279F1C0 */
void parameter_text::TextBuffer::assureTerminationImpl() {
    mText[static_cast<s32>(mCapacity) - 1] = 0;
}

/* 0279F1D8 */
void parameter_text::TextBuffer::destroy(u32 flags) {
    if (gabi::ea(this) != 0 && (flags & 1)) gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseObject), this);
}

/* 0279F1EC */
void parameter_text::FixedTextA::destroy(u32 flags) {
    if (gabi::ea(this) != 0 && (flags & 1)) gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseObject), this);
}

/* 0279F200 */
void parameter_text::FixedTextB::destroy(u32 flags) {
    if (gabi::ea(this) != 0 && (flags & 1)) gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseObject), this);
}

/* 0279F214 */
void ParameterBase::onResourceApplied(const be<u8>* data, u32 size) {}

/* WWHD glue
ext 0273AD10 void* allocateObject(u32 size)
ext 0273AF40 void releaseObject(void* object) callback
ext 0273B264 u32 hashBytes(const be<u8>* bytes, u32 length)
ext 027A861C void appendToOwner(parameter_tree::Value* owner, ParameterBase* parameter)
ext C0009988 void copyBytes(void* destination, const void* source, u32 size, u32 flags)
ext 0274D3EC void multiplyColor(ParameterFloats<4>* result, const ParameterFloats<4>* color, const ParameterFloats<4>* factors)
ext 0274D1E0 void addColor(ParameterFloats<4>* color, const ParameterFloats<4>* addend)
data sParameterBaseMethods 10167D60
data sTextMethods 10167CCC
str 10167D3C "default" in 0279E674
str 10167D48 "parameter" in 0279E674
str 10167D44 "" in 0279E674
str 10167D54 "" in 0279E710
*/
