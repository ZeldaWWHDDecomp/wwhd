// GPU resource and render-register adapters reconstructed from the WWHD binary.
#include "agl/gx2/RenderResources.h"
using namespace agl::gx2util;
// GHS resource dispatch descriptors contain a receiver adjustment and either a direct entry or a vtable slot.
template<class R,bool Tail = false,class... Args>
static inline R invokeResourceMethod(const RuntimeMethod& method,void* receiver,Args... args) {
    s32 slot = method.mVirtualSlot;
    s32 adjustment = method.mThisAdjustment;
    void* object = static_cast<u8*>(receiver) + adjustment;
    u32 target;
    if (slot < 0) {
        target = method.mEntry;
    } else {
        s32 vtableOffset = (s16)(u32)method.mEntry;
        const gptr<RuntimeVtableEntry>* binding = reinterpret_cast<const gptr<RuntimeVtableEntry>*>(static_cast<u8*>(object) + vtableOffset);
        const RuntimeVtableEntry* table = *binding;
        target = table[slot].mFunction;
    }
    if constexpr (Tail) return gabi::tail_ptr<R>(target, object, args...);
    else return gabi::call_ptr<R>(target, object, args...);
}
/* Read-only tables of this translation unit (views of their .data copies). */
struct EncodingTable31 { AttributeEncoding mEntries[31]; AttributeEncoding& operator[](u32 index) { return mEntries[index]; } };
struct MethodTable12 { RuntimeMethod mEntries[12]; RuntimeMethod& operator[](u32 index) { return mEntries[index]; } };
struct MethodTable3 { RuntimeMethod mEntries[3]; RuntimeMethod& operator[](u32 index) { return mEntries[index]; } };
struct MethodTable4 { RuntimeMethod mEntries[4]; RuntimeMethod& operator[](u32 index) { return mEntries[index]; } };
struct MethodTable9 { RuntimeMethod mEntries[9]; RuntimeMethod& operator[](u32 index) { return mEntries[index]; } };
struct WordTable3 { be<u32> mEntries[3]; be<u32>& operator[](u32 index) { return mEntries[index]; } };
struct WordTable32 { be<u32> mEntries[32]; be<u32>& operator[](u32 index) { return mEntries[index]; } };
struct WordTable4 { be<u32> mEntries[4]; be<u32>& operator[](u32 index) { return mEntries[index]; } };
/* Positive infinity: a cursor or frame stamp that never matches a frame. */
enum : u32 { kInvalidFrameBits = 0x7F800000u };
enum : u32 {
    PoseViewsUpdated = 0x10,      /* PaletteBuffer::mFlags: the views read this frame's world matrices */
    BillboardModeMask = 0x70000,  /* PoseBoneDescriptor::mFlags: billboard mode of the bone */
    NoBone = 0xFFFF,
};
/* The current frame of a frame controller as raw bits (stamps compare and copy bits). */
static inline u32 frameBits(const FrameController* frame) { return *reinterpret_cast<const be<u32>*>(&frame->mCurrent); }
/* Curve sampling methods, indexed by key kind and width. */
static MethodTable9 sFloatCurveMethods;
static MethodTable12 sIntegerCurveMethods;
static MethodTable3 sCursorMethods;
/* Texture-transform builders per mode (affine 2x3, matrix 3x4). */
static WordTable3 sAffineBuilders;
static WordTable3 sMatrixBuilders;
/* Pose blend accumulation (by rotation mode and normalisation) and finalisation methods. */
static MethodTable4 sBlendMethods;
static MethodTable4 sFinalizeMethods;
/* A self-relative link; the 32-bit text length precedes the linked text. */
const KeyBits* agl::gx2util::NameLink::key() const {
    s32 delta = mDelta;
    return delta ? reinterpret_cast<const KeyBits*>(reinterpret_cast<const u8*>(this) + delta - 4) : NULL;
}
/* The curve's key block plus a byte offset, formed like the original: a null key link still adds the offset. */
template <class T> static inline T* keyBlockAt(const be<u32>* keys, u32 offset) {
    return gabi::at<T>(gabi::ea(keys) + offset);
}
/* Element access of the owner's matrix rows (x, y, z, w are consecutive words). */
static inline MatrixScalar* rowCells(MatrixRow& row) { return &row.x; }
static inline const MatrixScalar* rowCells(const MatrixRow& row) { return &row.x; }
/* 027E18A8 */
void agl::gx2util::PolygonState::bind() {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    gx2SubmitPolygon(this);
}
/* 027E18AC */
u32 agl::gx2util::PolygonState::isFrontCulled() const {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    return mControl & 1;
}
/* 027E18B8 */
u32 agl::gx2util::PolygonState::isBackCulled() const {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    return (mControl >> 1) & 1;
}
/* 027E18C4 */
void agl::gx2util::PolygonState::setFrontOffset(u32 enabled) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    mControl = (mControl & ~0x800u) | ((enabled << 11) & 0x800u);
}
/* 027E18D4 */
void agl::gx2util::PolygonState::setBackOffset(u32 enabled) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    mControl = (mControl & ~0x1000u) | ((enabled << 12) & 0x1000u);
}
/* 027E18E4 */
void agl::gx2util::DepthState::bind() {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    gx2SubmitDepth(this);
}
/* 027E18E8 */
void agl::gx2util::DepthState::setDepthTest(u32 value) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    mControl = (mControl & ~0x2u) | ((value << 1) & 0x2u);
}
/* 027E18F8 */
void agl::gx2util::DepthState::setDepthWrite(u32 value) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    mControl = (mControl & ~0x4u) | ((value << 2) & 0x4u);
}
/* 027E1908 */
void agl::gx2util::DepthState::setDepthCompare(u32 value) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    mControl = (mControl & ~0x70u) | ((value << 4) & 0x70u);
}
/* 027E1918 */
u32 agl::gx2util::DepthState::isDepthTestEnabled() const {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    return (mControl >> 1) & 1;
}
/* 027E1924 */
void agl::gx2util::AlphaState::bind() {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    gx2SubmitAlpha(this);
}
/* 027E1928 */
void agl::gx2util::AlphaState::setEnabled(u32 enabled) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    mControl = (mControl & ~8u) | ((enabled << 3) & 8u);
}
/* 027E1938 */
void agl::gx2util::ColorState::bind() {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    gx2SubmitColor(this);
}
/* 027E193C */
void agl::gx2util::ColorState::setLogicOperation(u32 operation) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    mControl = (mControl & ~0xFF0000u) | ((operation << 16) & 0xFF0000u);
}
/* 027E194C */
void agl::gx2util::ColorState::setBlendMask(u32 mask) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    mControl = (mControl & ~0xFF00u) | ((mask << 8) & 0xFF00u);
}
/* 027E195C */
void agl::gx2util::ColorState::setColorWrite(u32 enabled) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    mControl = (mControl & ~0x70u) | ((16u - (enabled << 4)) & 0x70u);
}
/* 027E1974 */
u32 agl::gx2util::ColorState::getBlendMask() const {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    return (mControl >> 8) & 0xFF;
}
/* 027E1980 */
void agl::gx2util::BlendState::bind() {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    gx2SubmitBlend(this);
}
/* 027E1984 */
void agl::gx2util::BlendState::bindTarget(u32 target) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    gabi::FrameLocal<BlendState> state(8);
    state->mTarget = target;
    state->mControl = mControl;
    gx2SubmitBlend(state);
}
/* 027E19B4 */
void agl::gx2util::BlendState::setColorSource(u32 value) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    mControl = (mControl & ~0x1Fu) | ((value << 0) & 0x1Fu);
}
/* 027E19C4 */
void agl::gx2util::BlendState::setColorDestination(u32 value) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    mControl = (mControl & ~0x1F00u) | ((value << 8) & 0x1F00u);
}
/* 027E19D4 */
void agl::gx2util::BlendState::setSeparateAlpha(u32 value) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    mControl = (mControl & ~0x20000000u) | ((value << 29) & 0x20000000u);
}
/* 027E19E4 */
void agl::gx2util::BlendState::setAlphaSource(u32 value) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    mControl = (mControl & ~0x1F0000u) | ((value << 16) & 0x1F0000u);
}
/* 027E19F4 */
void agl::gx2util::BlendState::setAlphaDestination(u32 value) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    mControl = (mControl & ~0x1F000000u) | ((value << 24) & 0x1F000000u);
}
/* 027E1A04 */
void agl::gx2util::BlendColor::bind() {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    gx2SubmitBlendColor(this);
}
/* 027E1A08 */
void agl::gx2util::PolygonOffset::bind() {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    gx2SubmitOffset(this);
}
/* 027E1A0C */
void agl::gx2util::PolygonOffset::clear() {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    gx2ClearWords(this, 5);
}
/* 027E1A14 */
void agl::gx2util::PolygonOffset::setFrontScale(f32 scale) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    mFrontScale = gabi::fmuls_ppc(scale, 16.0f);
}
/* 027E1A28 */
void agl::gx2util::PolygonOffset::setFrontOffset(f32 offset) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    mFrontOffset = offset;
}
/* 027E1A30 */
void agl::gx2util::PolygonOffset::setBackScale(f32 scale) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    mBackScale = gabi::fmuls_ppc(scale, 16.0f);
}
/* 027E1A44 */
void agl::gx2util::PolygonOffset::setBackOffset(f32 offset) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    mBackOffset = offset;
}
/* 027E1468 */
void agl::gx2util::SamplerState::bindVertex(u32 slot) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    gx2SetVertexSampler(this, slot);
}
/* 027E146C */
void agl::gx2util::SamplerState::bindGeometry(u32 slot) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    gx2SetGeometrySampler(this, slot);
}
/* 027E1470 */
void agl::gx2util::SamplerState::bindPixel(u32 slot) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    gx2SetPixelSampler(this, slot);
}
/* 027E1474 */
f32 agl::gx2util::SamplerState::getLodBias() const {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    s16 bias = (s16)((s32)mLod >> 20);
    return gabi::fmuls_ppc((f32)bias, 1.0f / 64.0f);
}
/* 027E14A4 */
void agl::gx2util::TextureResource::initializeRegisters() {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    gx2InitializeTexture(this);
}
/* 027E14A8 */
void agl::gx2util::TextureResource::updateRegisters() {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    initializeRegisters();
}
/* 027E14AC */
void agl::gx2util::TextureResource::calculateLayout() {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    gx2CalculateSurface(this);
}
/* 027E14B0 */
void* agl::gx2util::TextureResource::getImage() const {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    return mImage;
}
/* 027E14B8 */
void* agl::gx2util::TextureResource::getMipImage() const {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    return mMipImage;
}
/* 027E14C0 */
void agl::gx2util::TextureResource::flush() {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    void* image = getImage();
    gx2FlushRange(image, mImageSize);
    void* mip = getMipImage();
    if (mip) gx2FlushRange(mip, mMipSize);
}
/* 027E150C */
void agl::gx2util::TextureResource::bindVertex(u32 slot) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    gx2SetVertexTexture(this, slot);
}
/* 027E1510 */
void agl::gx2util::TextureResource::bindGeometry(u32 slot) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    gx2SetGeometryTexture(this, slot);
}
/* 027E1514 */
void agl::gx2util::TextureResource::bindPixel(u32 slot) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    gx2SetPixelTexture(this, slot);
}
/* 027E1518 */
void agl::gx2util::TextureResource::setImages(void* image, void* mip) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    u32 levels = mMipCount;
    mImage = image;
    if (levels > 1 && !mip && image) {
        mip = static_cast<u8*>(image) + (u32)mMipOffset;
    }
    mMipImage = mip;
}
/* 027E159C */
void* agl::gx2util::FetchResource::getProgram() const {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    return mProgram;
}
/* 027E15A4 */
void agl::gx2util::FetchResource::flush() {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    void* program = getProgram();
    gx2FlushRange(program, mSize);
}
/* 027E15D0 */
void agl::gx2util::FetchResource::calculateSize() {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    u32 count = mStreamCount;
    u32 header = ((((count + 15u) >> 1) & ~7u) + 23u) & ~15u;
    mProgramHeaderSize = header;
    mSize = header + (count << 4);
}
/* 027E15F8 */
void agl::gx2util::FetchResource::bind() {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    gx2SetFetchShader(this);
}
/* 027E15FC */
void agl::gx2util::FetchResource::setProgram(void* program) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    mProgram = program;
}
/* 027E1604 */
FetchStream* agl::gx2util::FetchResource::getStream(void* program, u32 stream) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    u8* streams = static_cast<u8*>(program) + (u32)mProgramHeaderSize;
    return reinterpret_cast<FetchStream*>(streams + (stream << 4));
}
/* 027E1618 */
void agl::gx2util::FetchResource::clearStream(u32 stream) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    FetchStream* descriptor = getStream(mProgram, stream);
    descriptor->mReserved = 0;
}
/* 027E1374 */
void agl::gx2util::StridedBuffer::bindAttribute(u32 slot, u32 element) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    void* data = mutableSlice(element);
    u32 size = mSliceSize;
    u32 stride = mElementStride;
    gx2SetAttributeBuffer(slot, size, stride, data);
}
/* 027E13B4 */
void agl::gx2util::StridedBuffer::bindVertexUniform(u32 slot, u32 element) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    void* data = mutableSlice(element);
    u32 size = mSliceSize;
    gx2SetVertexUniform(slot, size, data);
}
/* 027E13F0 */
void agl::gx2util::StridedBuffer::bindGeometryUniform(u32 slot, u32 element) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    void* data = mutableSlice(element);
    u32 size = mSliceSize;
    gx2SetGeometryUniform(slot, size, data);
}
/* 027E142C */
void agl::gx2util::StridedBuffer::bindPixelUniform(u32 slot, u32 element) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    void* data = mutableSlice(element);
    u32 size = mSliceSize;
    gx2SetPixelUniform(slot, size, data);
}
/* Fetch encodings per attribute format (low six bits of the format). */
static EncodingTable31 sAttributeEncodings;
/* 027E1648 */
void agl::gx2util::FetchResource::initializeProgram(be<u32>* program) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    u32 streams = mStreamCount;
    u32 groups = (streams + 15u) >> 4;
    u32 cursor = 0;
    for (u32 group = 0; group < groups; ++group) {
        u32 address = (mProgramHeaderSize + (group << 8)) >> 3;
        program[cursor++] = __builtin_bswap32(address);
        u32 remaining = streams - 1u;
        u32 instruction;
        if (streams > (group + 1u) * 16u) {
            instruction = 0x01881C00;
        } else {
            instruction = 0x01800000 | ((remaining << 10) & 0x1C00u) | ((remaining << 16) & 0x80000u);
        }
        program[cursor++] = __builtin_bswap32(instruction);
    }
    program[cursor++] = 0;
    program[cursor++] = __builtin_bswap32(0x8A000000u);
    gx2ClearWords(&program[cursor], 2);
    for (u32 stream = 0; stream < streams; ++stream) {
        FetchStream* descriptor = getStream(program, stream);
        descriptor->mInstruction = __builtin_bswap32(0x3C00A001u);
        descriptor->mFormat = __builtin_bswap32(0x28D64800u);
        descriptor->mDivisor = __builtin_bswap32(0x000A0000u);
        descriptor->mReserved = 0;
    }
}
/* 027E1784 */
void agl::gx2util::FetchResource::setStreamBuffer(void* program,u32 stream,u32 buffer) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    FetchStream* descriptor = getStream(program, stream);
    u32 format = __builtin_bswap32((u32)descriptor->mFormat);
    descriptor->mFormat = __builtin_bswap32((format & ~0xFFu) | (buffer & 0xFFu));
}
/* 027E17B4 */
void agl::gx2util::FetchResource::setStreamLocation(void* program,u32 stream,u32 location) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    FetchStream* descriptor = getStream(program, stream);
    u32 instruction = __builtin_bswap32((u32)descriptor->mInstruction);
    descriptor->mInstruction = __builtin_bswap32((instruction & ~0xF00u) | ((location << 8) & 0xF00u));
}
/* 027E17E0 */
void agl::gx2util::FetchResource::setStreamFormat(void* program,u32 stream,u32 format) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    FetchStream* descriptor = getStream(program, stream);
    AttributeEncoding* encoding = &sAttributeEncodings[format & 63];
    u32 instruction = __builtin_bswap32((u32)descriptor->mInstruction);
    instruction = (instruction & ~0xFC000000u) | (((u32)encoding->mFetchKind << 26) & 0xFC000000u);
    descriptor->mInstruction = __builtin_bswap32(instruction);
    u32 control = __builtin_bswap32((u32)descriptor->mFormat);
    u32 numeric = (format & 0x800u) ? 2u : (format >> 8) & 1u;
    u32 selectors = encoding->mSelectors;
    control = (control & ~0x30000000u) | (numeric << 28);
    control = (control & ~0xE00u) | (((selectors << 17) | (selectors >> 15)) & 0xE00u);
    control = (control & ~0x7000u) | (((selectors << 28) | (selectors >> 4)) & 0x7000u);
    control = (control & ~0x38000u) | ((selectors << 7) & 0x38000u);
    control = (control & ~0x1C0000u) | (((selectors << 18) | (selectors >> 14)) & 0x1C0000u);
    control = (control & ~0x40000000u) | ((format << 21) & 0x40000000u);
    control = (control & ~0xFC00000u) | (((u32)encoding->mChannels << 22) & 0xFC00000u);
    descriptor->mFormat = __builtin_bswap32(control);
    u32 divisor = __builtin_bswap32((u32)descriptor->mDivisor);
    descriptor->mDivisor = __builtin_bswap32((divisor & ~0x30000u) | (((u32)encoding->mSignMode << 16) & 0x30000u));
}
/* 027E1878 */
void agl::gx2util::FetchResource::setStreamDivisor(void* program,u32 stream,u32 divisor) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    FetchStream* descriptor = getStream(program, stream);
    u32 control = __builtin_bswap32((u32)descriptor->mDivisor);
    descriptor->mDivisor = __builtin_bswap32((control & ~0xFFFFu) | (divisor & 0xFFFFu));
}
/* 027DEAD0 */
f32 agl::gx2util::AnimationCurve::wrapFrame(be<u32>* offset,f32 frame) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    u32 initialOffset = mOffsetBits;
    *offset = initialOffset;
    f32 first = mStartFrame;
    f32 last = mEndFrame;
    if (!(frame < first) && !(frame > last)) return frame;
    f32 distance = gabi::fsubs_ppc(first, frame);
    u32 mode;
    u32 after = 0;
    u32 flags = mFlags;
    if (distance > 0.0f) {
        mode = (flags >> 8) & 3;
        if (!mode) return first;
    } else {
        mode = (flags >> 12) & 3;
        if (!mode) return last;
        distance = gabi::fsubs_ppc(frame, last);
        after = 1;
    }
    f32 duration = gabi::fsubs_ppc(last, first);
    s32 cycles = gabi::ftoi(gabi::fdivs_ppc(distance, duration));
    f32 remainder = gabi::fnmsubs(duration, (f32)cycles, distance);
    u32 reverse = 1;
    if (mode == 3) {
        f32 count = (f32)(s32)((u32)cycles + 1u);
        f32 step = mRepeatOffset;
        f32 delta = gabi::fmuls_ppc(count, step);
        if (!after) delta = -delta;
        f32 base = __builtin_bit_cast(f32, initialOffset);
        *offset = __builtin_bit_cast(u32, gabi::fadds_ppc(base, delta));
    } else if (mode == 2 && !(cycles & 1)) {
        reverse = 0;
    }
    return reverse == after ? gabi::fadds_ppc(first, remainder) : gabi::fsubs_ppc(last, remainder);
}
/* 027DEFE4 */
u32 agl::gx2util::readKeyBit(const be<u8>* bytes,u32 byteCount,u32 bit) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    u32 byte = bit >> 3;
    return byte < byteCount ? ((u32)bytes[byte] >> (bit & 7)) & 1 : 0;
}
/* 027DF00C */
u32 agl::gx2util::KeyBits::test(u32 bit) const {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    return readKeyBit(mBytes, mByteCount, bit);
}
/* 027E1A8C */
u32 agl::gx2util::AnimationCurve::getFloatStorageSize() {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    f32 first = gx2FloorFrame(mStartFrame);
    f32 last = gx2FloorFrame(mEndFrame);
    s32 end = gabi::ftoi(gabi::fadds_ppc(last, 1.0f));
    s32 start = gabi::ftoi(first);
    u32 count = (u32)end - (u32)start + 1;
    return (s32)count > 2 ? (count << 2) + 16u : 0;
}
/* 027E1B24 */
u32 agl::gx2util::AnimationCurve::getIntegerStorageSize() {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    f32 first = gx2FloorFrame(mStartFrame);
    f32 last = gx2FloorFrame(mEndFrame);
    u32 count = (u32)gabi::ftoi(last) - (u32)gabi::ftoi(first) + 1u;
    if ((s32)count <= 2) return 0;
    u32 flags = mFlags;
    u32 kind = flags & 0x70;
    if (kind == 0x40 || kind == 0x50) {
        u32 width = flags & 0xC;
        if (width == 8) return count + 8u;
        if (width == 4) return (count << 1) + 8u;
        return (count << 2) + 8u;
    }
    u32 bytes = 0;
    if (kind == 0x60 || kind == 0x70) bytes = ((count + 31u) >> 3) & ~3u;
    return bytes + 8u;
}
/* 027E1E9C */
u32 agl::gx2util::AnimationCurve::sampleBoolean(f32 frame) const {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    s32 first = gabi::ftoi(mStartFrame);
    s32 sample = gabi::ftoi(frame);
    s32 index = (s32)((u32)sample - (u32)first);
    const be<u32>* bits = keyBlockAt<be<u32>>(mKeys.get(), (u32)(index >> 5) << 2);
    return (*bits >> ((u32)index & 31)) & 1;
}
/* Entry number of a node (entry 0 follows the root), from the 32-bit address difference. */
static inline s32 entryIndex(ResourceDictionary* dictionary,const DictionaryNode* node) {
    return (s32)(gabi::ea(node) - gabi::ea(&dictionary->mNodes[1])) >> 4;
}
/* 027DF01C */
DictionaryNode* agl::gx2util::ResourceDictionary::findKey(const KeyBits* key) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    DictionaryNode* base = mNodes;
    u32 previous = base->mBit;
    DictionaryNode* node = &base[(u16)base->mLeft];
    u32 bit = node->mBit;
    while (previous > bit) {
        u32 selected = key->test(bit);
        previous = node->mBit;
        u32 child = selected ? (u16)node->mRight : (u16)node->mLeft;
        node = &base[child];
        bit = node->mBit;
    }
    const KeyBits* name = node->mName.key();
    if (!name) return NULL;
    if (name == key) return node;
    if (name->mByteCount != key->mByteCount) return NULL;
    const be<u8>* left = name->mBytes;
    const be<u8>* right = key->mBytes;
    for (;;) {
        u32 a = *left++;
        u32 b = *right++;
        if (a != b) return NULL;
        if (!a) return node;
    }
}
/* 027DF108 */
s32 agl::gx2util::ResourceDictionary::findIndex(ResourceDictionary* dictionary,const KeyBits* key) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    if (!dictionary) return -1;
    DictionaryNode* node = dictionary->findKey(key);
    if (!node) return -1;
    return entryIndex(dictionary, node);
}
/* 027DF8B4 */
DictionaryNode* agl::gx2util::ResourceDictionary::findText(const be<u8>* text,u32 length) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    DictionaryNode* base = mNodes;
    u32 previous = base->mBit;
    DictionaryNode* node = &base[(u16)base->mLeft];
    u32 bit = node->mBit;
    while (previous > bit) {
        u32 selected = readKeyBit(text, length, bit);
        previous = node->mBit;
        u32 child = selected ? (u16)node->mRight : (u16)node->mLeft;
        node = &base[child];
        bit = node->mBit;
    }
    const KeyBits* name = node->mName.key();
    if (!name) return NULL;
    if (name->mByteCount != length) return NULL;
    const be<u8>* left = name->mBytes;
    const be<u8>* right = text;
    for (;;) {
        u32 a = *left++;
        u32 b = *right++;
        if (a != b) return NULL;
        if (!a) return node;
    }
}
/* 027DF9B0 */
s32 agl::gx2util::ResourceDictionary::findTextIndex(ResourceDictionary* dictionary,const be<u8>* text) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    if (!dictionary) return -1;
    const be<u8>* end = text;
    while (*end) ++end;
    DictionaryNode* node = dictionary->findText(text, (u32)(end - text));
    if (!node) return -1;
    return entryIndex(dictionary, node);
}
/* 027DFA24 */
void* agl::gx2util::ResourceDictionary::findTextValue(ResourceDictionary* dictionary,const be<u8>* text) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    if (!dictionary) return NULL;
    const be<u8>* end = text;
    while (*end) ++end;
    DictionaryNode* node = dictionary->findText(text, (u32)(end - text));
    if (!node) return NULL;
    return node->mValue.get();
}
/* 027E1EFC */
void* agl::gx2util::ResourceDictionary::findKeyValue(ResourceDictionary* dictionary,const KeyBits* key) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    if (!dictionary) return NULL;
    DictionaryNode* node = dictionary->findKey(key);
    if (!node) return NULL;
    return node->mValue.get();
}
/* 027DED78 */
void agl::gx2util::ChannelAllocation::calculate(const ChannelBuildCounts* counts) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    s32 objects = counts->mObjectCount;
    s32 channels = counts->mChannelCount;
    u32 curves = counts->mCurveCount;
    s32 capacity = objects < channels ? channels : objects;
    mStorage[0].mSize = curves << 2;
    mStorage[1].mSize = (u32)capacity << 2;
    u32 bindings = counts->mBindingCount;
    mStorage[2].mSize = ((bindings << 1) + 3u) & ~3u;
    u32 cacheSize = 0;
    if ((u8)counts->mUseCache && (u8)counts->mAllowCache) cacheSize = (curves * 12u + 3u) & ~3u;
    mStorage[0].mOffset = 0;
    mStorage[3].mSize = cacheSize;
    for (u32 index = 0; index < 3; ++index) {
        mStorage[index + 1].mOffset = mStorage[index].mOffset + mStorage[index].mSize;
    }
    u32 total = mStorage[3].mSize;
    u32 offset = mStorage[3].mOffset;
    mSegments = mStorage;
    mSize = offset + total;
}
/* 027DEFB8 */
u32 agl::gx2util::ChannelBuildPlan::allocationSize() {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    mAllocation.calculate(&mCounts);
    return mAllocation.mSize;
}
/* 027DFA98 */
void agl::gx2util::TransformAllocation::calculate(const TransformBuildCounts* counts) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    s32 objects = counts->mObjectCount;
    s32 bones = counts->mBoneCount;
    s32 capacity = objects < bones ? bones : objects;
    u32 curves = counts->mCurveCount;
    mStorage[1].mSize = (u32)capacity << 2;
    mStorage[0].mSize = ((u32)bones * 48u + 63u) & ~63u;
    u32 cacheSize = 0;
    if ((u8)counts->mUseCache && (u8)counts->mAllowCache) cacheSize = (curves * 12u + 3u) & ~3u;
    mStorage[0].mOffset = 0;
    mStorage[2].mSize = cacheSize;
    for (u32 index = 0; index < 2; ++index) {
        mStorage[index + 1].mOffset = mStorage[index].mOffset + mStorage[index].mSize;
    }
    u32 total = mStorage[2].mSize;
    u32 offset = mStorage[2].mOffset;
    mSegments = mStorage;
    mSize = offset + total;
}
/* 027DFCCC */
u32 agl::gx2util::TransformBuildPlan::allocationSize() {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    mAllocation.calculate(&mCounts);
    return mAllocation.mSize;
}
/* 027E0610 */
void agl::gx2util::ShapeAllocation::calculate(const ShapeBuildPlan* plan) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    u32 count = plan->mTargetCount;
    u32 size = (count * 56u + 63u) & ~63u;
    mStorage.mOffset = 0;
    mSize = size;
    mSegments = &mStorage;
    mStorage.mSize = size;
}
/* 027E06E0 */
u32 agl::gx2util::ShapeBuildPlan::allocationSize() {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    mAllocation.calculate(this);
    return mAllocation.mSize + 64u;
}
/* 027E1D5C */
void agl::gx2util::AnimationCurve::restoreFloatKeys() {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    if ((mFlags & 0x70u) != 0x20u) return;
    const be<u32>* block = mKeys.get();
    u32 size = getFloatStorageSize();
    FloatKeyTrailer* trailer = keyBlockAt<FloatKeyTrailer>(block, size - sizeof(FloatKeyTrailer));
    mFlags = trailer->mFlags;
    mKeyCount = trailer->mKeyCount;
    mScale.mBits = trailer->mScaleBits;
    mOffsetBits = trailer->mOffsetBits;
    u32 keys = trailer->mKeyAddress;
    mKeys.mDelta = keys ? keys - gabi::ea(&mKeys) : 0;
}
/* 027E1E00 */
void agl::gx2util::AnimationCurve::restoreIntegerKeys() {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    u32 kind = mFlags & 0x70u;
    if (kind != 0x50u && kind != 0x70u) return;
    const be<u32>* block = mKeys.get();
    u32 size = getIntegerStorageSize();
    IntegerKeyTrailer* trailer = keyBlockAt<IntegerKeyTrailer>(block, size - sizeof(IntegerKeyTrailer));
    mFlags = trailer->mFlags;
    mKeyCount = trailer->mKeyCount;
    u32 keys = trailer->mKeyAddress;
    mKeys.mDelta = keys ? keys - gabi::ea(&mKeys) : 0;
}
/* 027E08D0 */
void agl::gx2util::VisibilityAllocation::calculate(const ChannelBuildCounts* counts) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    s32 channels = counts->mChannelCount;
    s32 objects = counts->mObjectCount;
    u32 curves = counts->mCurveCount;
    u32 bindings = counts->mBindingCount;
    s32 capacity = objects < channels ? channels : objects;
    mStorage[2].mSize = (bindings + 3u) & ~3u;
    mStorage[0].mSize = ((bindings << 1) + 3u) & ~3u;
    mStorage[1].mSize = (u32)capacity << 2;
    u32 cacheSize = 0;
    if ((u8)counts->mUseCache && (u8)counts->mAllowCache) cacheSize = (curves * 12u + 3u) & ~3u;
    mStorage[0].mOffset = 0;
    mStorage[3].mSize = cacheSize;
    for (u32 index = 0; index < 3; ++index) {
        mStorage[index + 1].mOffset = mStorage[index].mOffset + mStorage[index].mSize;
    }
    u32 total = mStorage[3].mSize;
    u32 offset = mStorage[3].mOffset;
    mSegments = mStorage;
    mSize = offset + total;
}
/* 027E0AF0 */
u32 agl::gx2util::VisibilityBuildPlan::allocationSize() {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    mAllocation.calculate(&mCounts);
    return mAllocation.mSize;
}
/* 027E0BF0 */
void agl::gx2util::VisibilityTrack::copyDefaults(be<u16>* output) const {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    const be<u16>* indices = mIndices.get();
    u32 count = mCount;
    for (u32 index = 0; index < count; ++index) output[index] = indices[index];
}
/* 027E0C30 */
void agl::gx2util::AnimationRuntime::resetVisibilityResults() {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    u32 count = mBindings.mSourceCount;
    for (u32 index = 0; index < count; ++index) {
        VisibilityTrack* track = static_cast<VisibilityTrack*>(mTracks.get()) + index;
        u32 offset = track->mResultOffset;
        be<u16>* output = static_cast<be<u16>*>(mResults.get()) + offset;
        track->copyDefaults(output);
    }
}
/* 027E04C8 */
void agl::gx2util::FloatAnimationTrack::evaluateCached(u8* output,SampleCursor* cursors,f32 frame) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    u32 count = mCurveCount;
    for (u32 index = 0; index < count; ++index) {
        AnimationCurve* curve = mCurves.get() + index;
        u32 offset = curve->mTargetOffset;
        f32 value = curve->evaluateFloat(&cursors[index], frame);
        *reinterpret_cast<be<f32>*>(output + offset) = value;
    }
}
/* 027E0560 */
void agl::gx2util::FloatAnimationTrack::evaluateUncached(u8* output,f32 frame) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    u32 count = mCurveCount;
    gabi::FrameLocal<SampleCursor> cursor(8);
    for (u32 index = 0; index < count; ++index) {
        AnimationCurve* curve = mCurves.get() + index;
        u32 offset = curve->mTargetOffset;
        cursor->mStartBits = kInvalidFrameBits;
        f32 value = curve->evaluateFloat(cursor, frame);
        *reinterpret_cast<be<f32>*>(output + offset) = value;
    }
}
/* 027E0E5C */
void agl::gx2util::VisibilityTrack::evaluateCached(be<u16>* output,const be<s8>* bindings,SampleCursor* cursors,f32 frame) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    const VisibilityBinding* tracks = mBindings.get();
    u32 count = mCount;
    for (u32 index = 0; index < count; ++index) {
        if ((s8)bindings[index] == -1) continue;
        s32 curveIndex = tracks[index].mCurveIndex;
        if (curveIndex == -1) continue;
        AnimationCurve* curve = mCurves.get() + curveIndex;
        output[index] = curve->evaluateInteger(&cursors[curveIndex], frame);
    }
}
/* 027E0F40 */
void agl::gx2util::VisibilityTrack::evaluateUncached(be<u16>* output,const be<s8>* bindings,f32 frame) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    const VisibilityBinding* tracks = mBindings.get();
    u32 count = mCount;
    gabi::FrameLocal<SampleCursor> cursor(8);
    for (u32 index = 0; index < count; ++index) {
        if ((s8)bindings[index] == -1) continue;
        s32 curveIndex = tracks[index].mCurveIndex;
        if (curveIndex == -1) continue;
        AnimationCurve* curve = mCurves.get() + curveIndex;
        cursor->mStartBits = kInvalidFrameBits;
        output[index] = curve->evaluateInteger(cursor, frame);
    }
}
/* 027E1550 */
agl::gx2util::FetchResource* agl::gx2util::createFetchResource(FetchResource* resource) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    if (!resource) resource = static_cast<FetchResource*>(gx2Allocate(40));
    if (resource) gx2ClearWords(resource, 10);
    return resource;
}
/* 027E2084 */
void agl::gx2util::RenderPipelineState::setBlendMode(u32 mode) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    mFlags = (mFlags & ~0x30u) | (mode << 4);
    switch (mode) {
    case 0:
        mColor.setBlendMask(0);
        mColor.setLogicOperation(0xCC);
        break;
    case 1:
        mColor.setBlendMask(1);
        mColor.setLogicOperation(0xCC);
        break;
    case 2:
        mColor.setBlendMask(0);
        break;
    }
}
/* 027E212C */
void agl::gx2util::RenderPipelineState::setMaterialMode(u32 mode) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    mFlags = (mFlags & ~3u) | mode;
    if (!mode) return;
    mDepth.setDepthTest(1);
    mDepth.setDepthCompare(3);
    mColor.setLogicOperation(0xCC);
    switch (mode) {
    case 1:
        mDepth.setDepthWrite(1);
        mAlpha.setEnabled(0);
        setBlendMode(0);
        break;
    case 2:
        mDepth.setDepthWrite(1);
        mAlpha.setEnabled(1);
        setBlendMode(0);
        break;
    case 3:
        mDepth.setDepthWrite(0);
        setBlendMode(1);
        break;
    }
}
/* 027E220C */
void agl::gx2util::RenderPipelineState::bind() {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    mPolygon.bind();
    mDepth.bind();
    mAlpha.bind();
    mColor.bind();
    mBlend.bind();
    mBlendColor.bind();
}
/* 027E2DC0 */
// HD: a lone `blr`, shared (identical code folding) with the resource getters that return their argument: callers
// read r3 afterwards as the archive they passed in, so the definition returns its first argument unchanged.
u32 agl::gx2util::finishDrawCommands(u32 value) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    return value;
}
/* 027E39EC */
void agl::gx2util::onPaletteRebound() {
    gabi::AutoFrame nativeFrame{gabi::current_function()};}
/* 027E39F0 */
void agl::gx2util::onUniformRebound() {
    gabi::AutoFrame nativeFrame{gabi::current_function()};}
static agl::gx2util::TimerClock sPerformanceClock;
static agl::gx2util::TimerClock sAnimationClock;
static u32 sInvalidFrameBits = 0;
static agl::gx2util::TimerCounter sTimerCounter;
/* 027E1A4C */
void agl::gx2util::TimerClock::initialize() {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    SystemTimingInfo* system = gx2GetSystemTimingInfo();
    u32 clock = system->mBusClock;
    mTicks = 0;
    mFrequency = clock >> 2;
}
/* 027E3964 */
static void initializeResourceClocks() {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    sInvalidFrameBits = kInvalidFrameBits;
    agl::gx2util::SystemTimingInfo* system = gx2GetSystemTimingInfo();
    u32 clock = system->mBusClock;
    sPerformanceClock.mTicks = 0;
    sPerformanceClock.mFrequency = clock >> 2;
    sAnimationClock.initialize();
    sTimerCounter.mHigh = 0;
    sTimerCounter.mLow = 0;
}
/* 027E1F5C */
void agl::gx2util::TextureFileResource::initialize() {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    void* image = mImage.get();
    void* mip = mMip.get();
    mTexture.setImages(image, mip);
    mTexture.updateRegisters();
    mTexture.flush();
}
/* 027E1FC8 */
u32 agl::gx2util::UniformBinding::resolve(ResourceArchive* archive) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    if (mTexture.mDelta) return 0;
    const KeyBits* key = mName.key();
    ResourceDictionary* dictionary = archive->mTextures.get();
    void* texture = ResourceDictionary::findKeyValue(dictionary, key);
    if (!texture) return 1;
    mTexture.mDelta = gabi::ea(texture) - gabi::ea(&mTexture);
    return 0x10000;
}
/* 027DEC10 */
f32 agl::gx2util::AnimationCurve::evaluateFloat(SampleCursor* cursor,f32 frame) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    gabi::FrameLocal<be<u32>> offset(8);
    f32 wrapped = wrapFrame(offset, frame);
    u32 flags = mFlags;
    u32 method = ((flags >> 4) & 7u) * 3u + ((flags >> 2) & 3u);
    f32 value = invokeResourceMethod<f32>(sFloatCurveMethods[method], this, cursor, wrapped);
    f32 scale = mScale.mValue;
    f32 base = __builtin_bit_cast(f32, (u32)*offset);
    return gabi::fmadds(value, scale, base);
}
/* 027DECC4 */
u32 agl::gx2util::AnimationCurve::evaluateInteger(SampleCursor* cursor,f32 frame) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    gabi::FrameLocal<be<u32>> offset(8);
    f32 wrapped = wrapFrame(offset, frame);
    u32 flags = mFlags;
    u32 method = (((flags & 0x70u) - 0x40u) >> 4) * 3u + ((flags >> 2) & 3u);
    u32 value = invokeResourceMethod<u32>(sIntegerCurveMethods[method], this, cursor, wrapped);
    return value + mOffsetBits;
}
/* 027E1C34 */
void agl::gx2util::AnimationCurve::updateCursor(SampleCursor* cursor,f32 frame) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    f32 first = __builtin_bit_cast(f32, (u32)cursor->mStartBits);
    if (!(frame < first)) {
        f32 last = __builtin_bit_cast(f32, (u32)cursor->mEndBits);
        if (last > frame) return;
    }
    f32 start = mStartFrame;
    f32 duration = gabi::fsubs_ppc(mEndFrame, start);
    f32 reciprocal = (f32)gabi::fres(duration);
    f32 delta = gabi::fsubs_ppc(frame, start);
    f32 twice = gabi::fadds_ppc(reciprocal, reciprocal);
    f32 product = gabi::fmuls_ppc(duration, reciprocal);
    reciprocal = gabi::fnmsubs(product, reciprocal, twice);
    f32 normalized = gabi::fmuls_ppc(delta, reciprocal);
    cursor->mIndex = psq_quant(normalized, 5, 0);
    u32 method = mFlags & 3u;
    invokeResourceMethod<void>(sCursorMethods[method], this, cursor, frame);
}
/* 027E1CF0 */
u32 agl::gx2util::AnimationCurve::sampleBooleanCached(SampleCursor* cursor,f32 frame) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    updateCursor(cursor, frame);
    s32 index = cursor->mIndex;
    const be<u32>* bits = keyBlockAt<be<u32>>(mKeys.get(), (u32)(index >> 5) << 2);
    return (*bits >> ((u32)index & 31)) & 1;
}
/* 027DE778 */
void agl::gx2util::ModelRuntime::updatePalette(u32 view) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    mPose->updatePalette(view);
}
/* The first word of a matrix, as the view update takes it. */
static inline const be<u32>* matrixWords(const MatrixUtility* matrix) { return &matrix->mRows[0].x.mBits; }
static inline const MatrixUtility* worldMatrices(const PaletteBuffer* pose) { return static_cast<const MatrixUtility*>(pose->mWorld.get()); }
/* 027DE780 */
void agl::gx2util::ModelRuntime::updateViews(u32 view) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    PaletteBuffer* pose = mPose;
    pose->mFlags = pose->mFlags | PoseViewsUpdated;
    u32 count = mViewCount;
    const MatrixUtility* world = worldMatrices(pose);
    for (u32 index = 0; index < count; ++index) {
        ViewAllocation* allocation = &mViews[index];
        ViewDescriptor* resource = allocation->mResource;
        if ((u8)resource->mMode || (u8)allocation->mMultiBuffer) continue;
        u32 bone = resource->mBone;
        allocation->update(0, matrixWords(&world[bone]), view);
        count = mViewCount;
    }
}
/* 027DE824 */
void agl::gx2util::ModelRuntime::updateUniforms(u32 view) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    for (u32 index = 0; index < (u16)mUniformCount; ++index) {
        mUniforms[index].updateView(view);
    }
}
/* 027E39CC */
void agl::gx2util::fromTransformedPlanarColumn(MatrixUtility* output,const MatrixUtility* left,const MatrixUtility* right) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    output->fromTransformedPlanarColumn(left, right);
}
/* 027E39D0 */
void agl::gx2util::faceTransformedPlanarColumn(MatrixUtility* output,const MatrixUtility* left,const MatrixUtility* right) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    output->faceTranslationWithTransformedUp(left, right);
}
/* 027E39D4 */
void agl::gx2util::fromAlternatePlanarColumn(MatrixUtility* output,void*,void*,const MatrixUtility* input) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    output->fromPlanarColumn(input);
}
/* 027E39DC */
void agl::gx2util::faceAlternatePlanarColumn(MatrixUtility* output,void*,void*,const MatrixUtility* input) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    output->faceTranslationWithPlanarUp(input);
}
/* 027E39E4 */
void agl::gx2util::fromTransformedUp(MatrixUtility* output,const MatrixUtility* left,const MatrixUtility* right) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    output->fromTransformedUp(left, right);
}
/* 027E39E8 */
void agl::gx2util::faceTransformedUp(MatrixUtility* output,const MatrixUtility* left,const MatrixUtility* right) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    output->fromTransformedUpFacingTranslation(left, right);
}
/* output = left * right for 3x4 affine matrices; right is read whole first, so output may alias it. */
static inline void concatenateViewMatrix(MatrixUtility* output,const MatrixUtility* left,const MatrixUtility* right) {
    f32 basis[3][4];
    for (u32 row = 0; row < 3; ++row) {
        const MatrixScalar* cells = rowCells(right->mRows[row]);
        for (u32 column = 0; column < 4; ++column) basis[row][column] = cells[column].mValue;
    }
    for (u32 row = 0; row < 3; ++row) {
        const MatrixRow& source = left->mRows[row];
        f32 x = source.x.mValue;
        f32 y = source.y.mValue;
        f32 z = source.z.mValue;
        f32 translation = source.w.mValue;
        MatrixScalar* cells = rowCells(output->mRows[row]);
        for (u32 column = 0; column < 4; ++column) {
            f32 value = gabi::fmuls_ppc(basis[0][column], x);
            value = gabi::fmadds(basis[1][column], y, value);
            value = gabi::fmadds(basis[2][column], z, value);
            if (column >= 2) value = gabi::fmadds(column == 3 ? 1.0f : 0.0f, translation, value);
            cells[column].mValue = value;
        }
    }
}
/* 027DE8A0 */
void agl::gx2util::ModelRuntime::updateBillboardViews(u32 instance,const MatrixUtility* camera,u32 view) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    if (!(u8)mMultiBuffer) return;
    PaletteBuffer* pose = mPose;
    pose->mFlags = pose->mFlags | PoseViewsUpdated;
    u32 count = mViewCount;
    u32 cachedBone = ~0u;
    const MatrixUtility* world = worldMatrices(pose);
    gabi::FrameLocal<MatrixUtility> adjusted(8);
    gabi::FrameLocal<MatrixUtility> combined(0x38);
    for (u32 index = 0; index < count; ++index) {
        ViewAllocation* allocation = &mViews[index];
        ViewDescriptor* resource = allocation->mResource;
        if ((u8)resource->mMode || !(u8)allocation->mMultiBuffer) continue;
        u32 bone = resource->mBone;
        PaletteBuffer* currentPose = mPose;
        const PoseBoneDescriptor* descriptor = static_cast<const PoseBoneDescriptor*>(currentPose->mBones.get()) + bone;
        if (!(descriptor->mFlags & BillboardModeMask)) {
            allocation->update(instance, matrixWords(&world[bone]), view);
        } else {
            u32 parent = descriptor->mBillboardParent;
            if (parent == NoBone) {
                if (bone != cachedBone) {
                    currentPose->adjustBoneMatrix(adjusted, camera, bone, 1);
                    cachedBone = bone;
                }
                allocation->update(instance, matrixWords(adjusted), view);
            } else {
                if (parent != cachedBone) {
                    currentPose->adjustBoneMatrix(adjusted, camera, parent, 0);
                    cachedBone = parent;
                }
                concatenateViewMatrix(combined, adjusted, &world[bone]);
                allocation->update(instance, matrixWords(combined), view);
            }
        }
        count = mViewCount;
    }
}
/* Converter entry per uniform conversion kind (kinds below 28 keep their own). */
static WordTable32 sUniformConverters;
enum : u32 { FirstConvertedKind = 28 };
/* 027E25FC */
void agl::gx2util::UniformDescriptor::initializeConverters() {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    u32 count = mBitCount;
    for (u32 index = 0; index < count; ++index) {
        UniformFixup* conversion = mFixups.get() + index;
        u32 kind = conversion->mKind;
        if (kind >= FirstConvertedKind && !(u32)conversion->mConverter) conversion->mConverter = sUniformConverters[kind];
    }
}
/* 027E267C */
u32 agl::gx2util::UniformDescriptor::resolveTextures(ResourceArchive* archive) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    u32 count = mTextureCount;
    u32 result = 0;
    for (u32 index = 0; index < count; ++index) {
        UniformBinding* texture = mBindings.get() + index;
        if (!texture->mTexture.mDelta) result |= texture->resolve(archive);
    }
    return result;
}
/* 027E26FC */
void agl::gx2util::FileBufferResource::initialize() {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    void* data = mData.get();
    u32 size = mGpu.mSliceSize;
    mGpu.bind(data, size, 1);
    mGpu.flush(0);
}
/* 027E2748 */
void agl::gx2util::VertexResource::initializeBuffers() {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    u32 count = mBufferCount;
    for (u32 index = 0; index < count; ++index) {
        mBuffers.get()[index].initialize();
    }
}
/* 027E27B8 */
void agl::gx2util::IndexedMesh::draw(u32 first,u32 count,u32 instances) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    u32 format = mIndexFormat;
    IndexRange* ranges = mRanges.get();
    u32 startOffset = ranges[first].mByteOffset;
    u32 last = first + count - 1u;
    u32 finalOffset = ranges[last].mByteOffset;
    u32 finalCount = ranges[last].mCount;
    u32 indices = ((finalOffset - startOffset) >> ((format & 1u) + 1u)) + finalCount;
    StridedBuffer* buffer = mBuffer.get();
    u32 primitive = mPrimitive;
    u8* data = static_cast<u8*>(buffer->mutableSlice(0));
    void* start = data + startOffset;
    u32 baseVertex = mBaseVertex;
    gx2DrawIndices(primitive, indices, format, start, baseVertex, instances);
}
/* 027E288C */
void agl::gx2util::ParameterDescriptor::initializeBuffers() {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    u32 count = mMeshCount;
    for (u32 index = 0; index < count; ++index) {
        MeshBufferRecord* mesh = mMeshes.get() + index;
        FileBufferResource* buffer = mesh->mBuffer.get();
        void* data = buffer->mData.get();
        u32 size = buffer->mGpu.mSliceSize;
        buffer->mGpu.bind(data, size, 1);
        buffer->mGpu.flush(0);
    }
}
/* Unit scale (three 1.0 words) and identity rotation/zero translation (four words) defaults. */
static WordTable3 sDefaultScale;
static WordTable4 sDefaultRotation;
enum : u32 { TrackHasScale = 8, TrackHasRotation = 16, TrackHasTranslation = 32, TrackResultFlags = 0x0F800000 };
/* The rest pose's components as raw words (copied, never converted). */
static inline const be<u32>* restWords(const be<f32>* values) { return reinterpret_cast<const be<u32>*>(values); }
/* 027DFCF8 */
void agl::gx2util::TransformTrack::copyDefaults(TransformResult* output,const PoseBoneDescriptor* rest) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    be<u32>* defaults = mDefaults.get();
    u32 flags = mFlags;
    const be<u32>* scale;
    if (flags & TrackHasScale) {
        scale = defaults;
        defaults += 3;
    } else {
        scale = rest ? restWords(rest->mScale) : sDefaultScale.mEntries;
    }
    for (u32 index = 0; index < 3; ++index) output->mScale[index] = scale[index];
    const be<u32>* rotation;
    if (flags & TrackHasRotation) {
        rotation = defaults;
        defaults += 4;
    } else {
        rotation = rest ? restWords(rest->mRotation) : sDefaultRotation.mEntries;
    }
    for (u32 index = 0; index < 4; ++index) output->mRotation[index] = rotation[index];
    const be<u32>* translation = (flags & TrackHasTranslation) ? defaults : (rest ? restWords(rest->mTranslation) : sDefaultRotation.mEntries);
    for (u32 index = 0; index < 3; ++index) output->mTranslation[index] = translation[index];
    output->mFlags = flags & TrackResultFlags;
}
/* 027E0174 */
s32 agl::gx2util::PoseResource::findDescendantEnd(s32 bone) const {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    const PoseBoneDescriptor* bones = static_cast<const PoseBoneDescriptor*>(mBones.get());
    s32 count = mBoneCount;
    s32 parent = bones[bone].mParent;
    if (bone == 0) parent = -1;
    ++bone;
    while (bone < count) {
        s32 nextParent = bones[bone].mParent;
        if (!(nextParent > parent)) break;
        ++bone;
    }
    return bone;
}
enum : u32 { BindingTargetMask = 0x7FFF, BindingSourceMask = 0x3FFF8000, BindingModeMask = 0xC0000000 };
/* 027E01E0 */
void agl::gx2util::AnimationRuntime::setSubtreeFlags(const PoseResource* skeleton,s32 bone,u32 flags) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    s32 end = skeleton->findDescendantEnd(bone);
    u32 count = (s32)((u32)bone + 1u) < end ? (u32)end - (u32)bone : 1u;
    u32 mode = flags << 30;
    for (u32 index = 0; index < count; ++index) {
        u32 binding = mBindings.mSlots[(u32)bone + index];
        u32 target = (binding >> 15) & BindingTargetMask;
        if (target != BindingTargetMask) {
            mBindings.mSlots[target] = mBindings.mSlots[target] & ~BindingModeMask;
            mBindings.mSlots[target] = mBindings.mSlots[target] | mode;
        }
    }
}
/* 027E0288 */
void agl::gx2util::AnimationRuntime::evaluate() {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    FrameController* frame = mController;
    f32 previous = __builtin_bit_cast(f32, (u32)mLastFrameBits);
    f32 current = frame->mCurrent;
    if (previous == current) return;
    invokeResourceMethod<void>(mEvaluate, this);
    FrameController* updatedFrame = mController;
    mLastFrameBits = frameBits(updatedFrame);
}
/* 027E031C */
void agl::gx2util::AnimationRuntime::apply(void* target) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    invokeResourceMethod<void,true>(mApply, this, target);
}
/* 027E0360 */
void agl::gx2util::applyBoundAnimation(AnimationRuntime* animation,const ModelRuntime* owner) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    animation->apply(owner->mPose);
}
/* The value of a dictionary's entry (entry 0 is the tree root). */
static inline void* dictionaryValue(const ResourceLink<ResourceDictionary>& link,u32 index) {
    ResourceDictionary* dictionary = link.get();
    return dictionary->mNodes[index + 1u].mValue.get();
}
/* 027E2940 */
void agl::gx2util::ModelDescriptor::initialize() {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    u32 uniforms = mUniformCount;
    for (u32 index = 0; index < uniforms; ++index) {
        UniformDescriptor* descriptor = static_cast<UniformDescriptor*>(dictionaryValue(mUniforms, index));
        descriptor->initializeConverters();
    }
    u32 meshes = mViewCount;
    for (u32 index = 0; index < meshes; ++index) {
        ParameterDescriptor* mesh = static_cast<ParameterDescriptor*>(dictionaryValue(mViews, index));
        mesh->initializeBuffers();
    }
    u32 vertices = mVertexCount;
    for (u32 index = 0; index < vertices; ++index) {
        mVertices.get()[index].initializeBuffers();
    }
}
/* 027E2A68 */
u32 agl::gx2util::ModelDescriptor::resolveTextures(ResourceArchive* archive) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    u32 count = mUniformCount;
    u32 result = 0;
    for (u32 index = 0; index < count; ++index) {
        UniformDescriptor* descriptor = static_cast<UniformDescriptor*>(dictionaryValue(mUniforms, index));
        result |= descriptor->resolveTextures(archive);
    }
    return result;
}
enum : u32 { AllTargetsBound = 0xFFFF0000, SomeTargetMissing = 0xFFFF };
/* 027E2AF4 */
u32 agl::gx2util::TextureAnimationResource::bindModel(NameLookupResource* model) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    mBoundModelRelative = model ? gabi::ea(model) - gabi::ea(&mBoundModelRelative) : 0;
    be<u16>* bindings = mBindings.get();
    u32 count = mEntryCount;
    u32 result = 0;
    for (u32 index = 0; index < count; ++index) {
        TextureAnimationTrack* track = mTracks.get() + index;
        const KeyBits* key = track->mName.key();
        ResourceDictionary* dictionary = model->mNames.get();
        s32 target = ResourceDictionary::findIndex(dictionary, key);
        if (target >= 0) {
            result |= AllTargetsBound;
            bindings[index] = target;
        } else {
            result |= SomeTargetMissing;
            bindings[index] = NoBone;
        }
    }
    return result;
}
enum : u32 { CurvesCompressed = 1, CurveKindMask = 0x70, FirstIntegerKind = 0x40 };
/* 027E2BF4 */
void* agl::gx2util::TextureAnimationResource::restoreCurves() {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    if (!(mFlags & CurvesCompressed)) return NULL;
    u32 count = mEntryCount;
    bool found = false;
    void* firstKeys = NULL;
    for (u32 index = 0; index < count; ++index) {
        TextureAnimationTrack* track = mTracks.get() + index;
        u32 curves = track->mCurveCount;
        for (u32 curveIndex = 0; curveIndex < curves; ++curveIndex) {
            AnimationCurve* curve = track->mCurves.get() + curveIndex;
            if (!found) {
                firstKeys = curve->mKeys.get();
                found = true;
            }
            if ((curve->mFlags & CurveKindMask) < FirstIntegerKind) curve->restoreFloatKeys();
            else curve->restoreIntegerKeys();
        }
    }
    mFlags = mFlags ^ CurvesCompressed;
    return firstKeys;
}
/* 027E2CFC */
void* agl::gx2util::TextureAnimationResource::clearBindings() {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    be<u16>* bindings = mBindings.get();
    mBoundModelRelative = 0;
    u32 count = mEntryCount;
    for (u32 index = 0; index < count; ++index) bindings[index] = NoBone;
    return restoreCurves();
}
/* 027E2D40 */
u32 agl::gx2util::TextureAnimationResource::resolveTextures(ResourceArchive* archive) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    u32 count = mEntryCount;
    u32 result = 0;
    for (u32 index = 0; index < count; ++index) {
        UniformBinding* texture = mTextures.get() + index;
        if (!texture->mTexture.mDelta) result |= texture->resolve(archive);
    }
    return result;
}
struct RotationValues { f32 sine; f32 cosine; };
static inline RotationValues sampleRotation(f32 angle,AngleScratch* scratch) {
    Pair32 pair = gx2EncodeAngle(gabi::fmuls_ppc(angle, 683565248.0f));
    u32 encoded = pair.r4;
    scratch->mExponent = 0x43300000;
    scratch->mFraction = encoded & 0xFFFFFFu;
    f32 fraction = gabi::fmuls_ppc((f32)(encoded & 0xFFFFFFu), 5.960464477539063e-8f);
    const RotationSample* sample = &sPoseTrigTable.mEntries[encoded >> 24];
    f32 sine = gabi::fmadds(sample->mSinSlope, fraction, sample->mSin);
    f32 cosine = gabi::fmadds(sample->mCosSlope, fraction, sample->mCos);
    return {sine, cosine};
}
/* 027E2264 */
u32 agl::gx2util::AffineMatrix::fromPlanarTransform(const PlanarTransform* input) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    gabi::FrameLocal<AngleScratch> scratch(8);
    auto rotation = sampleRotation(input->mAngle, scratch);
    mValues[0].mValue = gabi::fmuls_ppc(input->mScaleX, rotation.cosine);
    mValues[1].mValue = gabi::fmuls_ppc(input->mScaleX, rotation.sine);
    mValues[2].mValue = -gabi::fmuls_ppc(input->mScaleY, rotation.sine);
    mValues[3].mValue = gabi::fmuls_ppc(input->mScaleY, rotation.cosine);
    mValues[4].mBits = input->mTranslationXBits;
    mValues[5].mBits = input->mTranslationYBits;
    return 24;
}
/* 027E234C */
u32 agl::gx2util::buildEulerMatrix(MatrixUtility* output,const EulerTransform* input) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    gabi::FrameLocal<AngleScratch> scratch(8);
    auto x = sampleRotation(input->mAngles[0], scratch);
    auto y = sampleRotation(input->mAngles[1], scratch);
    auto z = sampleRotation(input->mAngles[2], scratch);
    f32 sineProduct = gabi::fmuls_ppc(x.sine, y.sine);
    f32 scaledCosineY = gabi::fmuls_ppc(input->mScale[0], y.cosine);
    rowCells(output->mRows[0])[0].mValue = gabi::fmuls_ppc(scaledCosineY, z.cosine);
    f32 scaledSecondCosine = gabi::fmuls_ppc(input->mScale[1], y.cosine);
    rowCells(output->mRows[1])[0].mValue = gabi::fmuls_ppc(scaledSecondCosine, z.sine);
    rowCells(output->mRows[2])[0].mValue = -gabi::fmuls_ppc(input->mScale[2], y.sine);
    f32 cosineXSinZ = gabi::fmuls_ppc(x.cosine, z.sine);
    f32 firstCross = gabi::fmsubs(sineProduct, z.cosine, cosineXSinZ);
    f32 cosineXCosZ = gabi::fmuls_ppc(x.cosine, z.cosine);
    rowCells(output->mRows[0])[1].mValue = gabi::fmuls_ppc(input->mScale[0], firstCross);
    f32 secondCross = gabi::fmadds(sineProduct, z.sine, cosineXCosZ);
    rowCells(output->mRows[1])[1].mValue = gabi::fmuls_ppc(input->mScale[1], secondCross);
    f32 sineXCosZ = gabi::fmuls_ppc(x.sine, z.cosine);
    f32 scaledSineX = gabi::fmuls_ppc(input->mScale[2], x.sine);
    f32 sineXSinZ = gabi::fmuls_ppc(x.sine, z.sine);
    rowCells(output->mRows[2])[1].mValue = gabi::fmuls_ppc(scaledSineX, y.cosine);
    f32 thirdCross = gabi::fmadds(cosineXCosZ, y.sine, sineXSinZ);
    rowCells(output->mRows[0])[2].mValue = gabi::fmuls_ppc(input->mScale[0], thirdCross);
    f32 fourthCross = gabi::fmsubs(cosineXSinZ, y.sine, sineXCosZ);
    rowCells(output->mRows[1])[2].mValue = gabi::fmuls_ppc(input->mScale[1], fourthCross);
    f32 scaledCosineX = gabi::fmuls_ppc(input->mScale[2], x.cosine);
    rowCells(output->mRows[2])[2].mValue = gabi::fmuls_ppc(scaledCosineX, y.cosine);
    for (u32 row = 0; row < 3; ++row) output->mRows[row].w.mBits = input->mTranslationBits[row];
    return 48;
}
/* 027E2F98 */
u32 agl::gx2util::AffineMatrix::fromCenteredTexture(const TextureTransform* input) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    gabi::FrameLocal<AngleScratch> scratch(8);
    auto rotation = sampleRotation(input->mAngle, scratch);
    mValues[0].mValue = gabi::fmuls_ppc(input->mScaleX, rotation.cosine);
    f32 secondSine = gabi::fmuls_ppc(input->mScaleY, rotation.sine);
    f32 negativeHalfCosine = gabi::fmuls_ppc(-0.5f, rotation.cosine);
    mValues[1].mValue = -secondSine;
    mValues[2].mValue = gabi::fmuls_ppc(input->mScaleX, rotation.sine);
    f32 halfSineOffset = gabi::fmsubs(0.5f, rotation.sine, 0.5f);
    f32 secondCosine = gabi::fmuls_ppc(input->mScaleY, rotation.cosine);
    f32 offset = gabi::fsubs_ppc(negativeHalfCosine, halfSineOffset);
    mValues[3].mValue = secondCosine;
    offset = gabi::fsubs_ppc(offset, input->mTranslationX);
    mValues[4].mValue = gabi::fmuls_ppc(input->mScaleX, offset);
    offset = gabi::fadds_ppc(negativeHalfCosine, halfSineOffset);
    offset = gabi::fadds_ppc(offset, input->mTranslationY);
    mValues[5].mValue = gabi::fmadds(input->mScaleY, offset, 1.0f);
    return 24;
}
/* 027E30C0 */
u32 agl::gx2util::AffineMatrix::fromTranslatedTexture(const TextureTransform* input) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    gabi::FrameLocal<AngleScratch> scratch(8);
    auto rotation = sampleRotation(input->mAngle, scratch);
    f32 scaleY = input->mScaleY;
    f32 scaleX = input->mScaleX;
    f32 ySine = gabi::fmuls_ppc(scaleY, rotation.sine);
    f32 xCosine = gabi::fmuls_ppc(scaleX, rotation.cosine);
    mValues[0].mValue = xCosine;
    f32 xSine = gabi::fmuls_ppc(scaleX, rotation.sine);
    mValues[1].mValue = -ySine;
    f32 yCosine = gabi::fmuls_ppc(scaleY, rotation.cosine);
    mValues[2].mValue = xSine;
    mValues[3].mValue = yCosine;
    f32 shiftedX = gabi::fadds_ppc(input->mTranslationX, 0.5f);
    f32 shiftedY = gabi::fsubs_ppc(input->mTranslationY, 0.5f);
    f32 scaledX = gabi::fmuls_ppc(xCosine, shiftedX);
    f32 firstOffset = gabi::fmsubs(xSine, shiftedY, scaledX);
    mValues[4].mValue = gabi::fadds_ppc(firstOffset, 0.5f);
    shiftedY = gabi::fsubs_ppc(input->mTranslationY, 0.5f);
    shiftedX = gabi::fadds_ppc(input->mTranslationX, 0.5f);
    f32 scaledY = gabi::fmuls_ppc(yCosine, shiftedY);
    f32 secondOffset = gabi::fmadds(ySine, shiftedX, scaledY);
    mValues[5].mValue = gabi::fadds_ppc(secondOffset, 0.5f);
    return 24;
}
/* 027E31D8 */
u32 agl::gx2util::AffineMatrix::fromInverseTexture(const TextureTransform* input) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    gabi::FrameLocal<AngleScratch> scratch(8);
    auto rotation = sampleRotation(input->mAngle, scratch);
    f32 scaleX = input->mScaleX;
    f32 xCosine = gabi::fmuls_ppc(scaleX, rotation.cosine);
    f32 xSine = gabi::fmuls_ppc(scaleX, rotation.sine);
    f32 scaleY = input->mScaleY;
    f32 ySine = gabi::fmuls_ppc(scaleY, rotation.sine);
    mValues[0].mValue = xCosine;
    f32 yCosine = gabi::fmuls_ppc(scaleY, rotation.cosine);
    mValues[1].mValue = ySine;
    mValues[2].mValue = -xSine;
    mValues[3].mValue = yCosine;
    f32 firstOffset = gabi::fnmsubs(xCosine, input->mTranslationX, xSine);
    firstOffset = gabi::fnmsubs(xSine, input->mTranslationY, firstOffset);
    mValues[4].mValue = firstOffset;
    f32 secondOffset = gabi::fnmsubs(ySine, input->mTranslationX, -yCosine);
    secondOffset = gabi::fmadds(yCosine, input->mTranslationY, secondOffset);
    mValues[5].mValue = gabi::fadds_ppc(secondOffset, 1.0f);
    return 24;
}
/* 027E2DC4 */
void agl::gx2util::ResourceArchive::initialize() {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    u32 count = mModelCount;
    for (u32 index = 0; index < count; ++index) {
        ModelDescriptor* model = static_cast<ModelDescriptor*>(dictionaryValue(mModels, index));
        model->initialize();
    }
    count = mTextureCount;
    for (u32 index = 0; index < count; ++index) {
        TextureFileResource* texture = static_cast<TextureFileResource*>(dictionaryValue(mTextures, index));
        texture->initialize();
    }
}
/* 027E2EA4 */
u32 agl::gx2util::ResourceArchive::resolveTextures(ResourceArchive* textures,u32 flags) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    u32 result = 0;
    if (flags & 1u) {
        u32 count = mModelCount;
        for (u32 index = 0; index < count; ++index) {
            ModelDescriptor* model = static_cast<ModelDescriptor*>(dictionaryValue(mModels, index));
            result |= model->resolveTextures(textures);
        }
        count = mTextureAnimationCount;
        for (u32 index = 0; index < count; ++index) {
            TextureAnimationResource* animation = static_cast<TextureAnimationResource*>(dictionaryValue(mTextureAnimations, index));
            result |= animation->resolveTextures(textures);
        }
    }
    return result;
}
/* 027E25C4 */
u32 agl::gx2util::buildTextureAffine(AffineMatrix* output,const TextureTransform* input) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    return gabi::tail_ptr<u32>(sAffineBuilders[input->mMode], output, input);
}
/* 027E25E0 */
u32 agl::gx2util::buildTextureMatrix(MatrixUtility* output,const TextureTransform* input) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    return gabi::tail_ptr<u32>(sMatrixBuilders[input->mMode], output, input);
}
static inline u32 storeTextureMatrix(MatrixUtility* output,const MatrixUtility* extra,
                                     f32 a,f32 b,f32 c,f32 d,f32 x,f32 y,bool inverse) {
    if (extra) {
        for (u32 column = 0; column < 4; ++column) {
            f32 second = gabi::fmuls_ppc(b, rowCells(extra->mRows[1])[column].mValue);
            f32 first = inverse ? gabi::fmsubs(a, rowCells(extra->mRows[0])[column].mValue, second)
                                : gabi::fmadds(a, rowCells(extra->mRows[0])[column].mValue, second);
            rowCells(output->mRows[0])[column].mValue = gabi::fmadds(x, rowCells(extra->mRows[2])[column].mValue, first);
            f32 product = inverse ? gabi::fmuls_ppc(d, rowCells(extra->mRows[1])[column].mValue)
                                  : gabi::fmuls_ppc(c, rowCells(extra->mRows[0])[column].mValue);
            f32 lower = inverse ? gabi::fmadds(c, rowCells(extra->mRows[0])[column].mValue, product)
                                : gabi::fmsubs(d, rowCells(extra->mRows[1])[column].mValue, product);
            rowCells(output->mRows[1])[column].mValue = gabi::fmadds(y, rowCells(extra->mRows[2])[column].mValue, lower);
        }
        gx2CopyMemory(&output->mRows[2], &extra->mRows[2], sizeof(MatrixRow));
    } else {
        rowCells(output->mRows[0])[0].mValue = a;
        rowCells(output->mRows[1])[0].mValue = inverse ? c : -c;
        rowCells(output->mRows[0])[1].mValue = inverse ? -b : b;
        rowCells(output->mRows[1])[1].mValue = d;
        rowCells(output->mRows[0])[2].mValue = x;
        rowCells(output->mRows[1])[2].mValue = y;
        rowCells(output->mRows[2])[3].mValue = 0.0f;
        rowCells(output->mRows[2])[1].mValue = 0.0f;
        rowCells(output->mRows[2])[0].mValue = 0.0f;
        rowCells(output->mRows[1])[3].mValue = 0.0f;
        rowCells(output->mRows[0])[3].mValue = 0.0f;
        rowCells(output->mRows[2])[2].mValue = 1.0f;
    }
    return 48;
}
/* 027E32E0 */
u32 agl::gx2util::buildCenteredTextureMatrix(MatrixUtility* output,const TextureTransform* input) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    gabi::FrameLocal<AngleScratch> scratch(8);
    auto rotation = sampleRotation(input->mAngle, scratch);
    f32 scaleX = input->mScaleX;
    f32 xSine = gabi::fmuls_ppc(scaleX, rotation.sine);
    f32 xCosine = gabi::fmuls_ppc(scaleX, rotation.cosine);
    f32 scaleY = input->mScaleY;
    f32 yCosine = gabi::fmuls_ppc(scaleY, rotation.cosine);
    f32 ySine = gabi::fmuls_ppc(scaleY, rotation.sine);
    f32 horizontal = gabi::fadds_ppc(xCosine, xSine);
    f32 vertical = gabi::fsubs_ppc(yCosine, ySine);
    f32 translationX = gabi::fmuls_ppc(input->mTranslationX, scaleX);
    f32 translationY = gabi::fmuls_ppc(input->mTranslationY, scaleY);
    vertical = gabi::fadds_ppc(vertical, scaleY);
    horizontal = gabi::fsubs_ppc(horizontal, scaleX);
    f32 y = gabi::fmadds(-0.5f, vertical, translationY);
    f32 x = gabi::fmsubs(-0.5f, horizontal, translationX);
    const MatrixUtility* extra = input->mAdditionalMatrix;
    y = gabi::fadds_ppc(y, 1.0f);
    return storeTextureMatrix(output, extra, xCosine, xSine, ySine, yCosine, x, y, false);
}
/* 027E3514 */
u32 agl::gx2util::buildTranslatedTextureMatrix(MatrixUtility* output,const TextureTransform* input) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    gabi::FrameLocal<AngleScratch> scratch(8);
    auto rotation = sampleRotation(input->mAngle, scratch);
    f32 shiftedX = gabi::fadds_ppc(input->mTranslationX, 0.5f);
    f32 scaleX = input->mScaleX;
    f32 xCosine = gabi::fmuls_ppc(scaleX, rotation.cosine);
    f32 scaleY = input->mScaleY;
    f32 shiftedY = gabi::fsubs_ppc(input->mTranslationY, 0.5f);
    f32 yCosine = gabi::fmuls_ppc(scaleY, rotation.cosine);
    f32 xSine = gabi::fmuls_ppc(scaleX, rotation.sine);
    f32 lower = gabi::fmuls_ppc(yCosine, shiftedY);
    f32 upper = gabi::fmuls_ppc(xCosine, shiftedX);
    f32 ySine = gabi::fmuls_ppc(scaleY, rotation.sine);
    f32 x = gabi::fmsubs(xSine, shiftedY, upper);
    const MatrixUtility* extra = input->mAdditionalMatrix;
    f32 y = gabi::fmadds(ySine, shiftedX, lower);
    x = gabi::fadds_ppc(x, 0.5f);
    y = gabi::fadds_ppc(y, 0.5f);
    return storeTextureMatrix(output, extra, xCosine, xSine, ySine, yCosine, x, y, false);
}
/* 027E3744 */
u32 agl::gx2util::buildInverseTextureMatrix(MatrixUtility* output,const TextureTransform* input) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    gabi::FrameLocal<AngleScratch> scratch(8);
    auto rotation = sampleRotation(input->mAngle, scratch);
    f32 scaleX = input->mScaleX;
    f32 scaleY = input->mScaleY;
    f32 xCosine = gabi::fmuls_ppc(scaleX, rotation.cosine);
    f32 yCosine = gabi::fmuls_ppc(scaleY, rotation.cosine);
    f32 ySine = gabi::fmuls_ppc(scaleY, rotation.sine);
    f32 xSine = gabi::fmuls_ppc(scaleX, rotation.sine);
    f32 lower = gabi::fnmsubs(ySine, input->mTranslationX, -yCosine);
    f32 upper = gabi::fnmsubs(xCosine, input->mTranslationX, xSine);
    const MatrixUtility* extra = input->mAdditionalMatrix;
    f32 y = gabi::fmadds(yCosine, input->mTranslationY, lower);
    f32 x = gabi::fnmsubs(xSine, input->mTranslationY, upper);
    y = gabi::fadds_ppc(y, 1.0f);
    return storeTextureMatrix(output, extra, xCosine, xSine, ySine, yCosine, x, y, true);
}
/* Views of the clock's cursor cache: BufferView data = cursors, size = capacity, flags = live count. */
static inline s32 cursorCount(const AnimationRuntime* animation) { return (s32)(u32)animation->mCursors.mCount; }
static inline s32 cursorCapacity(const AnimationRuntime* animation) { return (s32)(u32)animation->mCursors.mSize; }
static inline SampleCursor* cursorAt(const AnimationRuntime* animation,u32 index) {
    return static_cast<SampleCursor*>(animation->mCursors.mData.get()) + index;
}
/* The clock word at +0x30 (AnimationClock::mFlags, cleared by the clock constructor) keeps the runtime's storage block. */
static inline void setStorage(AnimationRuntime* animation,void* storage) { animation->mStorage = gabi::ea(storage); }
enum : u32 {
    BindingsBound = 1,               /* BindingTable::mFlags */
    ResultsInvalid = 8,              /* AnimationRuntime::mState / BlendRuntime::mState */
    BindingSkipEvaluate = 0x40000000, BindingSkipApply = 0x80000000,
};
static inline void resetAnimationCache(AnimationRuntime* animation) {
    s32 count = cursorCount(animation);
    animation->mLastFrameBits = sInvalidFrameBits;
    if (count > 0 && count <= cursorCapacity(animation)) {
        s32 index = 0;
        do {
            cursorAt(animation, index)->mStartBits = kInvalidFrameBits;
            ++index;
        } while (index < cursorCount(animation));
    }
}
static inline void writeAnimationBinding(BindingTable* bindings,u32 source,u32 target) {
    bindings->mSlots[source] = bindings->mSlots[source] & BindingSourceMask;
    bindings->mSlots[source] = bindings->mSlots[source] | (target & BindingTargetMask);
    bindings->mSlots[target] = bindings->mSlots[target] & ~BindingSourceMask;
    bindings->mSlots[target] = bindings->mSlots[target] | ((source << 15) & BindingSourceMask);
}
/* 027DFE74 */
void agl::gx2util::AnimationRuntime::resetTransformResults(const PoseResource* skeleton) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    mState = mState | ResultsInvalid;
    u32 count = mBindings.mSourceCount;
    TransformResult* output = static_cast<TransformResult*>(mResults.get());
    for (u32 index = 0; index < count; ++index, ++output) {
        u32 target = mBindings.mSlots[index] & BindingTargetMask;
        if (target != BindingTargetMask) {
            TransformTrack* track = static_cast<TransformTrack*>(mTracks.get()) + index;
            const PoseBoneDescriptor* bones = static_cast<const PoseBoneDescriptor*>(skeleton->mBones.get());
            track->copyDefaults(output, &bones[target]);
        }
    }
}
/* 027DFF1C */
u32 agl::gx2util::AnimationRuntime::bindTransform(const PoseResource* skeleton) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    mBindings.reset(skeleton->mBoneCount);
    u32 count = mBindings.mSourceCount;
    u32 result = 0;
    for (u32 index = 0; index < count; ++index) {
        TransformTrack* track = static_cast<TransformTrack*>(mTracks.get()) + index;
        const be<u8>* name = track->mName.get();
        ResourceDictionary* names = skeleton->mNames.get();
        s32 target = ResourceDictionary::findTextIndex(names, name);
        if (target >= 0) {
            writeAnimationBinding(&mBindings, index, (u32)target);
            result |= AllTargetsBound;
        } else result |= SomeTargetMissing;
    }
    mBindings.mFlags = mBindings.mFlags | BindingsBound;
    resetAnimationCache(this);
    resetTransformResults(skeleton);
    return result;
}
/* 027E00AC */
void agl::gx2util::AnimationRuntime::bindIndexedTransform(const PoseResource* skeleton) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    mBindings.reset(skeleton->mBoneCount);
    AnimationResourceHeader* resource = mResource;
    const be<u16>* indices = resource->mBindings.get();
    mBindings.setTargets(indices);
    mBindings.mFlags = mBindings.mFlags | BindingsBound;
    resetAnimationCache(this);
    resetTransformResults(skeleton);
}
/* 027DF7F4 */
void agl::gx2util::AnimationRuntime::setChannelResource(AnimationResourceHeader* resource) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    mResource = resource;
    mTracks = resource->mTracks.get();
    mBindings.mFlags = mBindings.mFlags & ~BindingsBound;
    u32 repeat = ((u32)resource->mFlags >> 2) & 1u;
    u32 frames = resource->mFrameCount;
    setRange(frames, repeat);
    mBindings.mSourceCount = resource->mTrackCount;
    mCursors.mCount = resource->mCurveCount;
    resetAnimationCache(this);
}
/* Transform evaluate/apply entries per rotation encoding (resource flags bits 12-14). */
enum : u32 {
    QuaternionTransforms = 0x1000,
    EvaluateQuaternionTransforms = 0x027E6AA8, ApplyQuaternionTransforms = 0x027E6C04,
    EvaluateEulerTransforms = 0x027E6EA8, ApplyEulerTransforms = 0x027E7004,
};
/* 027E0368 */
void agl::gx2util::AnimationRuntime::setTransformResource(AnimationResourceHeader* resource) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    mResource = resource;
    mTracks = resource->mTransformTracks.get();
    mBindings.mFlags = mBindings.mFlags & ~BindingsBound;
    u32 mode = resource->mFlags & 0x7000u;
    mEvaluate.mThisAdjustment = 0;
    mEvaluate.mVirtualSlot = -1;
    mApply.mThisAdjustment = 0;
    mApply.mVirtualSlot = -1;
    if (mode == QuaternionTransforms) {
        mApply.mEntry = ApplyQuaternionTransforms;
        mEvaluate.mEntry = EvaluateQuaternionTransforms;
    } else {
        mApply.mEntry = ApplyEulerTransforms;
        mEvaluate.mEntry = EvaluateEulerTransforms;
    }
    u32 repeat = ((u32)resource->mFlags >> 2) & 1u;
    u32 frames = resource->mFrameCount;
    setRange(frames, repeat);
    mBindings.mSourceCount = resource->mTrackCount;
    mCursors.mCount = resource->mTransformCurveCount;
    resetAnimationCache(this);
}
/* storage + offset, formed like the original (a null storage still adds the offset). */
static inline void* storageAt(void* storage,u32 offset) { return gabi::at<void>(gabi::ea(storage) + offset); }
/* 027E063C */
u32 agl::gx2util::BlendRuntime::initialize(ShapeBuildPlan* plan,void* storage,u32 capacity) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    ShapeAllocation* allocation = &plan->mAllocation;
    if (!allocation->mSegments) allocation->calculate(plan);
    if ((u32)allocation->mSize > capacity) return 0;
    mStorage = storage;
    AllocationSegment* segment = allocation->mSegments;
    u32 bytes = segment->mSize;
    void* aligned = storageAt(NULL, (gabi::ea(storage) + 63u) & ~63u);
    void* data = bytes ? storageAt(aligned, segment->mOffset) : NULL;
    mData = data;
    mWork = data;
    mState = ResultsInvalid;
    u16 count = (u16)(u32)plan->mTargetCount;
    mCapacity = count;
    mTargetCount = count;
    return 1;
}
/* 027E0710 */
void agl::gx2util::BlendRuntime::invalidate() {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    u32 size = ((u32)mCapacity * 56u + 63u) & ~63u;
    mState = mState | 12u;
    clearGraphicsRange(mData, size);
}
/* 027E0738 */
void agl::gx2util::BlendRuntime::blend(AnimationRuntime* animation,f32 weight) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    if (__builtin_fabsf(weight) < 0.001f) return;
    mState = mState & ~4u;
    AnimationVtable* table = static_cast<AnimationVtable*>(animation->mVtable.get());
    // HD: the original leaves the animation in r4 as well (moved to r3 from it) for the virtual evaluate.
    gabi::call_ptr<void>(table->mEvaluate, animation, animation);
    AnimationResourceHeader* resource = animation->mResource;
    u32 state = mState;
    u32 mode = ((u32)resource->mFlags >> 12) & 7u;
    u32 index = (state & 2u) | mode;
    invokeResourceMethod<void>(sBlendMethods[index], this, animation, weight);
}
/* 027E0834 */
void agl::gx2util::BlendRuntime::finalize(void* target) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    u32 state = mState;
    u32 index = (state & 3u) | ((state >> 2) & 1u);
    // HD: the finalize methods (027E89FC ...) write the blended result into the caller's target (r4).
    invokeResourceMethod<void>(sFinalizeMethods[index], this, target);
    mState = mState | 4u;
}
static inline void* allocationPointer(AllocationSegment* segments,u32 index,void* storage) {
    u32 size = segments[index].mSize;
    return size ? storageAt(storage, segments[index].mOffset) : NULL;
}
template<class Allocation>
static inline void initializeAnimationBuffers(AnimationRuntime* animation,Allocation* allocation,void* storage,
                                              s32 objects,s32 curves,u32 cacheSegment) {
    void* bindings = allocationPointer(allocation->mSegments, 1, storage);
    animation->mBindings.configure(static_cast<be<u32>*>(bindings), objects);
    void* cursors = allocationPointer(allocation->mSegments, cacheSegment, storage);
    animation->mCursors.configure(cursors, curves);
}
/* 027DEE5C */
u32 agl::gx2util::AnimationRuntime::initializeChannels(ChannelBuildPlan* plan,void* storage,u32 capacity) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    ChannelAllocation* allocation = &plan->mAllocation;
    if (!allocation->mSegments) allocation->calculate(&plan->mCounts);
    if ((u32)allocation->mSize > capacity) return 0;
    gabi::FrameLocal<AllocationCounts> counts(8);
    counts->mObjects = plan->mCounts.mObjectCount;
    counts->mChannels = plan->mCounts.mChannelCount;
    s32 maximum = counts->mObjects < counts->mChannels ? (s32)counts->mChannels : (s32)counts->mObjects;
    s32 curves = plan->mCounts.mCurveCount;
    mResource = NULL;
    setStorage(this, storage);
    initializeAnimationBuffers(this, allocation, storage, maximum, curves, 3);
    mResults = allocationPointer(allocation->mSegments, 0, storage);
    mCapacity = plan->mCounts.mChannelCount;
    mState = plan->mCounts.mBindingCount;
    mTargets = static_cast<be<u16>*>(allocationPointer(allocation->mSegments, 2, storage));
    return 1;
}
/* 027DFB70 */
u32 agl::gx2util::AnimationRuntime::initializeTransforms(TransformBuildPlan* plan,void* storage,u32 capacity) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    TransformAllocation* allocation = &plan->mAllocation;
    if (!allocation->mSegments) allocation->calculate(&plan->mCounts);
    if ((u32)allocation->mSize > capacity) return 0;
    gabi::FrameLocal<AllocationCounts> counts(8);
    counts->mObjects = plan->mCounts.mObjectCount;
    counts->mChannels = plan->mCounts.mBoneCount;
    s32 maximum = counts->mObjects < counts->mChannels ? (s32)counts->mChannels : (s32)counts->mObjects;
    s32 curves = plan->mCounts.mCurveCount;
    setStorage(this, storage);
    mResource = NULL;
    initializeAnimationBuffers(this, allocation, storage, maximum, curves, 2);
    void* results = allocationPointer(allocation->mSegments, 0, storage);
    mTargets = static_cast<be<u16>*>(results);
    mResults = results;
    u32 bones = plan->mCounts.mBoneCount;
    mEvaluate.mEntry = 0;
    mCapacity = bones;
    mApply.mVirtualSlot = 0;
    mState = ResultsInvalid;
    mApply.mThisAdjustment = 0;
    mEvaluate.mVirtualSlot = 0;
    mApply.mEntry = 0;
    mEvaluate.mThisAdjustment = 0;
    return 1;
}
/* 027E09B8 */
u32 agl::gx2util::AnimationRuntime::initializeVisibility(VisibilityBuildPlan* plan,void* storage,u32 capacity) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    VisibilityAllocation* allocation = &plan->mAllocation;
    if (!allocation->mSegments) allocation->calculate(&plan->mCounts);
    if ((u32)allocation->mSize > capacity) return 0;
    gabi::FrameLocal<AllocationCounts> counts(8);
    counts->mObjects = plan->mCounts.mObjectCount;
    counts->mChannels = plan->mCounts.mChannelCount;
    s32 maximum = counts->mObjects < counts->mChannels ? (s32)counts->mChannels : (s32)counts->mObjects;
    s32 curves = plan->mCounts.mCurveCount;
    mResource = NULL;
    setStorage(this, storage);
    initializeAnimationBuffers(this, allocation, storage, maximum, curves, 3);
    mResults = allocationPointer(allocation->mSegments, 0, storage);
    mCapacity = plan->mCounts.mChannelCount;
    mState = plan->mCounts.mBindingCount;
    mTargets = static_cast<be<u16>*>(allocationPointer(allocation->mSegments, 2, storage));
    return 1;
}
/* 027E12B4 */
void agl::gx2util::AnimationRuntime::setVisibilityResource(AnimationResourceHeader* resource) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    mResource = resource;
    mTracks = resource->mTracks.get();
    mBindings.mFlags = mBindings.mFlags & ~BindingsBound;
    u32 repeat = ((u32)resource->mFlags >> 18) & 1u;
    u32 frames = resource->mFrameCount;
    setRange(frames, repeat);
    mBindings.mSourceCount = resource->mVisibilityTrackCount;
    mCursors.mCount = resource->mCurveCount;
    resetAnimationCache(this);
}
/* 027DF168 */
u32 agl::gx2util::AnimationRuntime::bindChannelTargets(const ChannelTrack* track,const UniformDescriptor* material) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    const ChannelBindingInfo* info = track->mBindings.get();
    u32 count = track->mTargetCount;
    u32 output = track->mFirstBinding;
    u32 result = 0;
    for (u32 index = 0; index < count; ++index) {
        const KeyBits* key = info[index].mName.key();
        ResourceDictionary* names = material->mChannelNames.get();
        s32 target = ResourceDictionary::findIndex(names, key);
        mTargets[output + index] = target;
        result |= target >= 0 ? AllTargetsBound : SomeTargetMissing;
    }
    return result;
}
/* The per-target result of a visibility binding is a signed byte. */
static inline be<s8>* visibilityTargets(const AnimationRuntime* animation) { return reinterpret_cast<be<s8>*>(animation->mTargets.get()); }
/* 027E0B1C */
u32 agl::gx2util::AnimationRuntime::bindVisibilityTargets(const VisibilityTrack* track,const UniformDescriptor* material) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    const VisibilityBinding* info = track->mBindings.get();
    u32 count = track->mCount;
    u32 output = track->mResultOffset;
    u32 result = 0;
    for (u32 index = 0; index < count; ++index) {
        const KeyBits* key = info[index].mName.key();
        ResourceDictionary* names = material->mVisibilityNames.get();
        s32 target = ResourceDictionary::findIndex(names, key);
        visibilityTargets(this)[output + index] = target;
        result |= target >= 0 ? AllTargetsBound : SomeTargetMissing;
    }
    return result;
}
/* 027DF240 */
u32 agl::gx2util::AnimationRuntime::bindChannels(const ModelDescriptor* model) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    mBindings.reset(model->mUniformCount);
    u32 count = mBindings.mSourceCount;
    u32 result = 0;
    for (u32 index = 0; index < count; ++index) {
        ChannelTrack* track = static_cast<ChannelTrack*>(mTracks.get()) + index;
        const KeyBits* key = track->mName.key();
        ResourceDictionary* names = model->mUniforms.get();
        s32 target = ResourceDictionary::findIndex(names, key);
        bool matched = false;
        if (target >= 0) {
            const UniformDescriptor* material = static_cast<const UniformDescriptor*>(names->mNodes[(u32)target + 1u].mValue.get());
            u32 status = bindChannelTargets(track, material);
            matched = (status >> 16) != 0;
        }
        if (matched) {
            writeAnimationBinding(&mBindings, index, (u32)target);
            result |= AllTargetsBound;
        } else result |= SomeTargetMissing;
    }
    mBindings.mFlags = mBindings.mFlags | BindingsBound;
    resetAnimationCache(this);
    return result;
}
/* 027E0C88 */
u32 agl::gx2util::AnimationRuntime::bindVisibility(const ModelDescriptor* model) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    mBindings.reset(model->mUniformCount);
    u32 count = mBindings.mSourceCount;
    u32 result = 0;
    for (u32 index = 0; index < count; ++index) {
        VisibilityTrack* track = static_cast<VisibilityTrack*>(mTracks.get()) + index;
        const KeyBits* key = track->mName.key();
        ResourceDictionary* names = model->mUniforms.get();
        s32 target = ResourceDictionary::findIndex(names, key);
        bool matched = false;
        if (target >= 0) {
            const UniformDescriptor* material = static_cast<const UniformDescriptor*>(names->mNodes[(u32)target + 1u].mValue.get());
            u32 status = bindVisibilityTargets(track, material);
            matched = (status >> 16) != 0;
        }
        if (matched) {
            writeAnimationBinding(&mBindings, index, (u32)target);
            result |= AllTargetsBound;
        } else result |= SomeTargetMissing;
    }
    mBindings.mFlags = mBindings.mFlags | BindingsBound;
    resetAnimationCache(this);
    resetVisibilityResults();
    return result;
}
/* 027DF40C */
void agl::gx2util::AnimationRuntime::evaluateChannels() {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    FrameController* frame = mController;
    if (__builtin_bit_cast(f32, (u32)mLastFrameBits) == frame->mCurrent) return;
    s32 cursors = cursorCount(this);
    f32 current = frame->mCurrent;
    u32 count = mBindings.mSourceCount;
    bool cached = cursors > 0 && cursors <= cursorCapacity(this);
    for (u32 index = 0; index < count; ++index) {
        u32 binding = mBindings.mSlots[index];
        ChannelTrack* track = static_cast<ChannelTrack*>(mTracks.get()) + index;
        u32 firstBinding = track->mFirstBinding;
        u32 output = track->mResultOffset;
        if (!(binding & BindingSkipEvaluate)) {
            be<u32>* results = static_cast<be<u32>*>(mResults.get()) + output;
            be<u16>* targets = &mTargets[firstBinding];
            if (cached) gx2EvaluateChannelCached(track, results, targets, cursorAt(this, output), current);
            else gx2EvaluateChannelUncached(track, results, targets, NULL, current);
        }
    }
    FrameController* updated = mController;
    mLastFrameBits = frameBits(updated);
}
/* 027E1038 */
void agl::gx2util::AnimationRuntime::evaluateVisibility() {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    FrameController* frame = mController;
    if (__builtin_bit_cast(f32, (u32)mLastFrameBits) == frame->mCurrent) return;
    s32 cursors = cursorCount(this);
    f32 current = frame->mCurrent;
    u32 count = mBindings.mSourceCount;
    bool cached = cursors > 0 && cursors <= cursorCapacity(this);
    for (u32 index = 0; index < count; ++index) {
        u32 binding = mBindings.mSlots[index];
        VisibilityTrack* track = static_cast<VisibilityTrack*>(mTracks.get()) + index;
        // the original loads the track's result offset (and, cached, its first curve) before the skip test
        // (027E10BC/027E10C4, 027E1138)
        u32 output = track->mResultOffset;
        u32 firstCurve = cached ? (u32)track->mFirstCurve : 0;
        if (!(binding & BindingSkipEvaluate)) {
            be<u16>* results = static_cast<be<u16>*>(mResults.get()) + output;
            be<s8>* targets = visibilityTargets(this) + output;
            if (cached) track->evaluateCached(results, targets, cursorAt(this, firstCurve), current);
            else track->evaluateUncached(results, targets, current);
        }
    }
    FrameController* updated = mController;
    mLastFrameBits = frameBits(updated);
}
/* 027E1198 */
void agl::gx2util::AnimationRuntime::applyVisibilityTrack(UniformBuffer* material,u32 index) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    VisibilityTrack* track = static_cast<VisibilityTrack*>(mTracks.get()) + index;
    u32 output = track->mResultOffset;
    be<u16>* values = static_cast<be<u16>*>(mResults.get()) + output;
    u32 count = track->mCount;
    for (u32 offset = 0; offset < count; ++offset) {
        s32 target = visibilityTargets(this)[output + offset];
        if (target != -1) {
            AnimationResourceHeader* resource = mResource;
            UniformBinding* texture = resource->mTextures.get() + (u32)values[offset];
            void* data = texture->mTexture.get();
            material->mBindings[(u32)target] = data;
        }
    }
}
/* 027E1234 */
void agl::gx2util::AnimationRuntime::applyVisibility(ModelRuntime* model) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    u32 count = mBindings.mSourceCount;
    for (u32 index = 0; index < count; ++index) {
        u32 binding = mBindings.mSlots[index];
        if (!(binding & BindingSkipApply)) {
            applyVisibilityTrack(&model->mUniforms[binding & BindingTargetMask], index);
        }
    }
}
enum : u32 { MaterialDirty = 4 };
static inline void dirtyMaterialChannel(UniformBuffer* material,u32 index) {
    material->mFlags = material->mFlags | MaterialDirty;
    u32 bit = 1u << (index & 31u);
    material->mDirty[index >> 5] = material->mDirty[index >> 5] | bit;
}
/* 027DF57C */
void agl::gx2util::AnimationRuntime::applyChannelTrack(UniformBuffer* material,u32 index) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    ChannelTrack* track = static_cast<ChannelTrack*>(mTracks.get()) + index;
    be<u32>* results = static_cast<be<u32>*>(mResults.get()) + (u32)track->mResultOffset;
    ChannelBindingInfo* info = track->mBindings.get();
    u32 count = track->mTargetCount;
    u32 first = track->mFirstBinding;
    u32 end = first + count;
    if (!((s32)first < (s32)end)) return;
    for (u32 entry = 0; entry < count; ++entry) {
        u32 target = mTargets[first + entry];
        if (target == NoBone) continue;
        UniformDescriptor* resource = material->mResource;
        UniformFixup* descriptor = resource->mFixups.get() + target;
        if ((s32)descriptor->mFlags >= 0) {
            dirtyMaterialChannel(material, target);
            resource = material->mResource;
        }
        UniformFixup* descriptors = resource->mFixups.get();
        u32 parent = descriptor->mParent;
        if ((s32)descriptors[parent].mFlags >= 0) dirtyMaterialChannel(material, parent);
        u32 begin = info[entry].mFirstCurve;
        u32 finish = begin + (u32)info[entry].mFloatCount + (u32)info[entry].mIntegerCount;
        u32 dataOffset = descriptor->mDataOffset;
        u8* data = material->mData + dataOffset;
        for (u32 curve = begin; curve < finish; ++curve) {
            AnimationCurve* curves = track->mCurves.get();
            u32 value = results[curve];
            u32 offset = curves[curve].mTargetOffset;
            *reinterpret_cast<be<u32>*>(data + offset) = value;
        }
        begin = info[entry].mFirstDefault;
        finish = begin + (u32)info[entry].mDefaultCount;
        for (u32 value = begin; value < finish; ++value) {
            ChannelDefault* defaults = track->mDefaults.get();
            u32 offset = defaults[value].mOffset;
            u32 bits = defaults[value].mValue;
            *reinterpret_cast<be<u32>*>(data + offset) = bits;
        }
    }
}
/* 027DF774 */
void agl::gx2util::AnimationRuntime::applyChannels(ModelRuntime* model) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    u32 count = mBindings.mSourceCount;
    for (u32 index = 0; index < count; ++index) {
        u32 binding = mBindings.mSlots[index];
        if (!(binding & BindingSkipApply)) {
            applyChannelTrack(&model->mUniforms[binding & BindingTargetMask], index);
        }
    }
}
/* ---- world matrices of a pose (PaletteBuffer: bones = traversal order, local = PoseLocal, world = MatrixUtility) ---- */
enum : u32 {
    LocalIdentityScale = 0x03000000, LocalIdentityRotation = 0x0C000000, LocalInheritedMask = 0xF0000000,
    LocalAccumulatedIdentity = 0x30000000, LocalCompensateScale = 0x00800000, PoseWorldUpdated = 4,
};
static inline const PoseBoneDescriptor* traversal(const PaletteBuffer* pose) { return static_cast<const PoseBoneDescriptor*>(pose->mBones.get()); }
static inline const MatrixScalar* localScale(const PoseLocal& local) { return reinterpret_cast<const MatrixScalar*>(local.mScale); }
static inline void copyPoseMatrix(MatrixUtility* output,const MatrixUtility* input) {
    for (u32 row = 0; row < 3; ++row) {
        MatrixScalar* destination = rowCells(output->mRows[row]);
        const MatrixScalar* source = rowCells(input->mRows[row]);
        for (u32 column = 0; column < 4; ++column) destination[column].mBits = source[column].mBits;
    }
}
static inline void scalePoseMatrix(MatrixUtility* output,const MatrixUtility* input,const MatrixScalar* scale) {
    f32 z = scale[2].mValue;
    for (u32 row = 0; row < 3; ++row) {
        const MatrixRow& source = input->mRows[row];
        MatrixRow& destination = output->mRows[row];
        f32 xValue = source.x.mValue;
        f32 yValue = source.y.mValue;
        f32 xScale = scale[0].mValue;
        f32 yScale = scale[1].mValue;
        destination.x.mValue = gabi::fmuls_ppc(xValue, xScale);
        destination.y.mValue = gabi::fmuls_ppc(yValue, yScale);
        f32 zValue = source.z.mValue;
        f32 wValue = source.w.mValue;
        destination.z.mValue = gabi::fmuls_ppc(zValue, z);
        destination.w.mValue = gabi::fmuls_ppc(wValue, 1.0f);
    }
}
static inline void initializePoseWorld(PoseLocal* local,MatrixUtility* world,const MatrixUtility* base) {
    if (((u32)local->mFlags & LocalIdentityRotation) == LocalIdentityRotation) copyPoseMatrix(world, base);
    else concatenateViewMatrix(world, base, &local->mMatrix);
    u32 flags = local->mFlags;
    local->mFlags = (flags & ~LocalInheritedMask) | ((flags << 4) & LocalAccumulatedIdentity);
}
static inline u32 propagatePoseFlags(PoseLocal* child,const PoseLocal* parent) {
    u32 flags = child->mFlags;
    u32 inherited = (u32)parent->mFlags & (flags << 4);
    flags = (flags & ~LocalInheritedMask) | (inherited & LocalInheritedMask);
    child->mFlags = flags;
    return flags;
}
/* Scales the translation column of a bone's world matrix by its parent's scale. */
static inline void scaleTranslation(MatrixUtility* world,const MatrixScalar* scale) {
    for (u32 row = 0; row < 3; ++row) {
        MatrixScalar& translation = rowCells(world->mRows[row])[3];
        translation.mValue = gabi::fmuls_ppc(translation.mValue, scale[row].mValue);
    }
}
/* 027E39F4 */
void agl::gx2util::PaletteBuffer::updateRigidWorld(const MatrixUtility* base) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    mFlags = mFlags | PoseWorldUpdated;
    PoseLocal* local = static_cast<PoseLocal*>(mLocal.get());
    MatrixUtility* world = static_cast<MatrixUtility*>(mWorld.get());
    u32 count = mBoneCount;
    initializePoseWorld(local, world, base);
    for (u32 index = 1; index < count; ++index) {
        u32 parent = traversal(this)[index].mParent;
        u32 bone = traversal(this)[index].mIndex;
        u32 flags = propagatePoseFlags(&local[bone], &local[parent]);
        if ((flags & LocalIdentityRotation) == LocalIdentityRotation) copyPoseMatrix(&world[bone], &world[parent]);
        else concatenateViewMatrix(&world[bone], &world[parent], &local[bone].mMatrix);
    }
}
/* 027E3CA8 */
void agl::gx2util::PaletteBuffer::updateScaledWorld(const MatrixUtility* base) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    mFlags = mFlags | PoseWorldUpdated;
    PoseLocal* local = static_cast<PoseLocal*>(mLocal.get());
    MatrixUtility* world = static_cast<MatrixUtility*>(mWorld.get());
    u32 count = mBoneCount;
    initializePoseWorld(local, world, base);
    for (u32 index = 1; index < count; ++index) {
        u32 parent = traversal(this)[index].mParent;
        u32 bone = traversal(this)[index].mIndex;
        u32 flags = propagatePoseFlags(&local[bone], &local[parent]);
        u32 parentFlags = local[parent].mFlags;
        if ((parentFlags & LocalIdentityScale) == LocalIdentityScale) {
            if ((flags & LocalIdentityRotation) == LocalIdentityRotation) copyPoseMatrix(&world[bone], &world[parent]);
            else concatenateViewMatrix(&world[bone], &world[parent], &local[bone].mMatrix);
        } else {
            scalePoseMatrix(&world[bone], &world[parent], localScale(local[parent]));
            flags = local[bone].mFlags;
            if ((flags & LocalIdentityRotation) != LocalIdentityRotation) concatenateViewMatrix(&world[bone], &world[bone], &local[bone].mMatrix);
        }
    }
    for (u32 bone = 0; bone < count; ++bone) {
        if (((u32)local[bone].mFlags & LocalIdentityScale) != LocalIdentityScale) scalePoseMatrix(&world[bone], &world[bone], localScale(local[bone]));
    }
}
/* 027E4134 */
void agl::gx2util::PaletteBuffer::updateCompensatedWorld(const MatrixUtility* base) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    mFlags = mFlags | PoseWorldUpdated;
    PoseLocal* local = static_cast<PoseLocal*>(mLocal.get());
    MatrixUtility* world = static_cast<MatrixUtility*>(mWorld.get());
    u32 count = mBoneCount;
    initializePoseWorld(local, world, base);
    for (u32 index = 1; index < count; ++index) {
        u32 bone = traversal(this)[index].mIndex;
        u32 parent = traversal(this)[index].mParent;
        u32 flags = propagatePoseFlags(&local[bone], &local[parent]);
        bool identityScale = ((u32)local[parent].mFlags & LocalIdentityScale) == LocalIdentityScale;
        if ((flags & LocalIdentityRotation) == LocalIdentityRotation) {
            if ((flags & LocalCompensateScale) || identityScale) copyPoseMatrix(&world[bone], &world[parent]);
            else scalePoseMatrix(&world[bone], &world[parent], localScale(local[parent]));
        } else if (identityScale) {
            concatenateViewMatrix(&world[bone], &world[parent], &local[bone].mMatrix);
        } else if (flags & LocalCompensateScale) {
            copyPoseMatrix(&world[bone], &local[bone].mMatrix);
            scaleTranslation(&world[bone], localScale(local[parent]));
            concatenateViewMatrix(&world[bone], &world[parent], &world[bone]);
        } else {
            scalePoseMatrix(&world[bone], &world[parent], localScale(local[parent]));
            concatenateViewMatrix(&world[bone], &world[bone], &local[bone].mMatrix);
        }
    }
    for (u32 bone = 0; bone < count; ++bone) {
        if (((u32)local[bone].mFlags & LocalIdentityScale) != LocalIdentityScale) scalePoseMatrix(&world[bone], &world[bone], localScale(local[bone]));
    }
}
/* 027E47BC */
void agl::gx2util::PaletteBuffer::updateAccumulatedWorld(const MatrixUtility* base) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    mFlags = mFlags | PoseWorldUpdated;
    MatrixUtility* world = static_cast<MatrixUtility*>(mWorld.get());
    PoseLocal* local = static_cast<PoseLocal*>(mLocal.get());
    u32 count = mBoneCount;
    PoseScale* scales = static_cast<PoseScale*>(mScale.get());
    initializePoseWorld(local, world, base);
    if (((u32)local->mFlags & LocalIdentityScale) == LocalIdentityScale) {
        scales[0].mValues[2].mValue = 1.0f;
        scales[0].mValues[1].mValue = 1.0f;
        scales[0].mValues[0].mValue = 1.0f;
    } else {
        for (u32 axis = 0; axis < 3; ++axis) scales[0].mValues[axis].mBits = local[0].mScale[axis];
    }
    for (u32 index = 1; index < count; ++index) {
        u32 bone = traversal(this)[index].mIndex;
        u32 parent = traversal(this)[index].mParent;
        u32 flags = propagatePoseFlags(&local[bone], &local[parent]);
        if ((flags & LocalIdentityRotation) == LocalIdentityRotation) copyPoseMatrix(&world[bone], &world[parent]);
        else if (((u32)local[parent].mFlags & LocalAccumulatedIdentity) == LocalAccumulatedIdentity) concatenateViewMatrix(&world[bone], &world[parent], &local[bone].mMatrix);
        else {
            copyPoseMatrix(&world[bone], &local[bone].mMatrix);
            scaleTranslation(&world[bone], scales[parent].mValues);
            concatenateViewMatrix(&world[bone], &world[parent], &world[bone]);
        }
        if (((u32)local[bone].mFlags & LocalIdentityScale) == LocalIdentityScale) {
            for (u32 axis = 0; axis < 3; ++axis) scales[bone].mValues[axis].mBits = scales[parent].mValues[axis].mBits;
        } else {
            for (u32 axis = 0; axis < 3; ++axis) scales[bone].mValues[axis].mValue = gabi::fmuls_ppc(scales[parent].mValues[axis].mValue, localScale(local[bone])[axis].mValue);
        }
    }
    for (u32 bone = 0; bone < count; ++bone) {
        if (((u32)local[bone].mFlags & LocalAccumulatedIdentity) != LocalAccumulatedIdentity) scalePoseMatrix(&world[bone], &world[bone], scales[bone].mValues);
    }
}
/* WWHD glue
data sPerformanceClock 104A2288
data sAnimationClock 104A2290
data sInvalidFrameBits 104A2298 nocheck
data sTimerCounter 104A229C
data sUniformConverters 1016C470 nocheck
data sAffineBuilders 1016C4F0 nocheck
data sMatrixBuilders 1016C4FC nocheck
data sAttributeEncodings 1016C508 nocheck
data sDefaultRotation 1016C604 nocheck
data sDefaultScale 1016C614 nocheck
data sBlendMethods 1016C6E0 nocheck
data sFinalizeMethods 1016C700 nocheck
data sCursorMethods 1016C720 nocheck
data sFloatCurveMethods 1016C738 nocheck
data sIntegerCurveMethods 1016C780 nocheck
ext C0009C80 agl::gx2util::SystemTimingInfo* gx2GetSystemTimingInfo()
ext C0006938 void gx2SubmitPolygon(const agl::gx2util::PolygonState*)
ext C0006850 void gx2SubmitDepth(const agl::gx2util::DepthState*)
ext C0006780 void gx2SubmitAlpha(const agl::gx2util::AlphaState*)
ext C00067D8 void gx2SubmitColor(const agl::gx2util::ColorState*)
ext C00067B8 void gx2SubmitBlend(const agl::gx2util::BlendState*)
ext C00067A8 void gx2SubmitBlendColor(const agl::gx2util::BlendColor*)
ext C0006948 void gx2SubmitOffset(const agl::gx2util::PolygonOffset*)
ext 028F5914 void gx2ClearWords(void*,u32)
ext C0006A18 void gx2SetVertexSampler(const agl::gx2util::SamplerState*,u32)
ext C0006878 void gx2SetGeometrySampler(const agl::gx2util::SamplerState*,u32)
ext C00068E0 void gx2SetPixelSampler(const agl::gx2util::SamplerState*,u32)
ext C0006498 void gx2InitializeTexture(agl::gx2util::TextureResource*)
ext C00060D0 void gx2CalculateSurface(agl::gx2util::TextureResource*)
ext C0006A30 void gx2SetVertexTexture(const agl::gx2util::TextureResource*,u32)
ext C00068A0 void gx2SetGeometryTexture(const agl::gx2util::TextureResource*,u32)
ext C00068F8 void gx2SetPixelTexture(const agl::gx2util::TextureResource*,u32)
ext C00088A0 void gx2FlushRange(void*,u32)
ext C0006860 void gx2SetFetchShader(const agl::gx2util::FetchResource*)
ext 027DBBEC void* agl::gx2util::StridedBuffer::mutableSlice(u32)
ext C0006798 void gx2SetAttributeBuffer(u32,u32,u32,void*)
ext C0006A38 void gx2SetVertexUniform(u32,u32,void*)
ext C00068A8 void gx2SetGeometryUniform(u32,u32,void*)
ext C0006900 void gx2SetPixelUniform(u32,u32,void*)
ext 028F4114 f32 gx2FloorFrame(f32)
ext 0273AD10 void* gx2Allocate(u32)
ext 027DBC2C void agl::gx2util::PaletteBuffer::updatePalette(u32)
ext 027DDBB0 void agl::gx2util::ViewAllocation::update(u32,const be<u32>*,u32)
ext 027DD468 void agl::gx2util::UniformBuffer::updateView(u32)
ext 027DC0C4 void agl::gx2util::MatrixUtility::fromTransformedPlanarColumn(const agl::gx2util::MatrixUtility*,const agl::gx2util::MatrixUtility*)
ext 027DC200 void agl::gx2util::MatrixUtility::faceTranslationWithTransformedUp(const agl::gx2util::MatrixUtility*,const agl::gx2util::MatrixUtility*)
ext 027DC598 void agl::gx2util::MatrixUtility::fromPlanarColumn(const agl::gx2util::MatrixUtility*)
ext 027DC650 void agl::gx2util::MatrixUtility::faceTranslationWithPlanarUp(const agl::gx2util::MatrixUtility*)
ext 027DC9A0 void agl::gx2util::MatrixUtility::fromTransformedUp(const agl::gx2util::MatrixUtility*,const agl::gx2util::MatrixUtility*)
ext 027DCBBC void agl::gx2util::MatrixUtility::fromTransformedUpFacingTranslation(const agl::gx2util::MatrixUtility*,const agl::gx2util::MatrixUtility*)
ext 027DB590 void agl::gx2util::PaletteBuffer::adjustBoneMatrix(agl::gx2util::MatrixUtility*,const agl::gx2util::MatrixUtility*,u32,u32)
ext 027DB134 void agl::gx2util::StridedBuffer::bind(void*,u32,u32)
ext 027DBC00 void agl::gx2util::StridedBuffer::flush(u32)
ext C0006178 void gx2DrawIndices(u32,u32,u32,void*,u32,u32)
ext 028F5680 Pair32 gx2EncodeAngle(f32)
ext C000A848 void* gx2CopyMemory(void*,const void*,u32)
ext 027DA898 void agl::gx2util::BindingTable::reset(u32)
ext 027DA8D8 void agl::gx2util::BindingTable::setTargets(const be<u16>*)
ext 027DAAB8 void agl::gx2util::AnimationClock::setRange(s32,u32)
ext 027DD9AC void clearGraphicsRange(void*,u32)
ext 027DA87C void agl::gx2util::BindingTable::configure(be<u32>*,u32)
ext 027DA960 void agl::gx2util::BufferView::configure(void*,u32)
ext 027E67EC void gx2EvaluateChannelCached(agl::gx2util::ChannelTrack*,be<u32>*,be<u16>*,agl::gx2util::SampleCursor*,f32)
ext 027E6930 void gx2EvaluateChannelUncached(agl::gx2util::ChannelTrack*,be<u32>*,be<u16>*,void*,f32)
*/
