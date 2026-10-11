/* HD line-material sorting, weighted vertex animation and generated helpers. Local source only. */
#include "m_Do/m_Do_ext_tail.h"

static HdTailHeapPointer l_opaqueDrawBuffer;
static HdTailHeapPointer l_heapManager;
static HdTailDrawState l_drawState;
static u32 l_sortVtable;
static u32 l_arrayVtable;
static u32 l_arrayMemberVtable;
static u32 l_shapeVtable;
static f32 l_zero;
static f32 l_lower;
static f32 l_upper;
static HdTailGlobals l_globals;
static HdTailRange l_range;
static u32 l_globalRegistration;
static u32 l_signedRegistration;
static u32 l_unsignedRegistration;
static u32 l_heapA;
static u32 l_heapB;
static u32 l_commandHeap;
static u32 l_heapC;
static u32 l_heapD;

enum HdTailFlags : u32 { HdTail_MatrixLocked = 2, HdTail_Delete = 1 };
enum HdLineMaterialSlot : u32 { HdLine_SetMaterial = 7, HdLine_Draw = 11 };
enum HdTailHeapSlot : u32 { HdTail_Free = 15, HdTail_PacketDestroy = 3 };

/* 025EDBC0 */
void HdLineSortPacket::reset() {
    HdLineMaterial* lineMat = mp3DlineMat;
    if (lineMat != NULL) {
        hdTailRemovePacket(l_opaqueDrawBuffer.mpHeap.get(), this, 0);
        mp3DlineMat = NULL;
        do {
            HdLineMaterial* next = lineMat->mpNextLineMat;
            lineMat->mpNextLineMat = NULL;
            lineMat = next;
        } while (lineMat != NULL);
    }
}

/* 025EDC2C */
// HD: nullable constructor destination and enlarged packet ABI.
static HdLineSortPacket* mDoExt_3DlineMatSortPacket__ct(HdLineSortPacket* self) {
    if (self == NULL) {
        self = static_cast<HdLineSortPacket*>(hdTailAllocate(sizeof(HdLineSortPacket)));
        if (self == NULL) return NULL;
    }
    hdTailPacketConstruct(self);
    self->mVtable = &l_sortVtable;
    self->reset();
    return self;
}

/* 025EDC88 */
void HdLineSortPacket::draw(void* arguments) {
    HdLineMaterial* lineMat = mp3DlineMat;
    gabi::call_ptr(lineMat->mVtable[HdLine_SetMaterial].get(), lineMat, arguments);
    lineMat = mp3DlineMat;
    do {
        gabi::call_ptr(lineMat->mVtable[HdLine_Draw].get(), lineMat, arguments);
        lineMat = lineMat->mpNextLineMat;
    } while (lineMat != NULL);
    // HD: restore the native draw state after submitting the sorted materials.
    hdTailDrawStateApply(&l_drawState);
}

/* 025EDD04 */
void HdLineSortPacket::setMat(HdLineMaterial* i_3DlineMat) {
    HdLineMaterial* lineMat = mp3DlineMat;
    if (lineMat == NULL) {
        hdTailEntryPacket(l_opaqueDrawBuffer.mpHeap.get(), this, 0);
        lineMat = mp3DlineMat;
    }
    i_3DlineMat->mpNextLineMat = lineMat;
    mp3DlineMat = i_3DlineMat;
}

/* 025EDD64 */
// HD: weighted vertex storage has no matching GameCube wrapper.
u32 HdVertexWeight::init(HdWeightInput* input) {
    u32 count = input->mCount;
    u32 rounded = (count * sizeof(cXyz) + 31) & ~31u;
    u32 vertices = (u32)(((u64)0xAAAAAAAB * rounded) >> 32) >> 3;
    cXyz* output = static_cast<cXyz*>(hdTailAllocateAligned(vertices * sizeof(cXyz), 0x20));
    mpOutput = output;
    if (output == NULL) return 0;
    mpInput = input;
    return 1;
}

/* 025EDE0C */
void HdVertexWeight::calc(HdWeightModel* model) {
    HdWeightResource* root = model->mResource;
    u16 flags = root->mFlags;
    bool wasLocked = (flags & HdTail_MatrixLocked) != 0;
    if (wasLocked) root->mFlags = flags & ~HdTail_MatrixLocked;
    s32 value = (s32)model->mBufferIndex + 1;
    s32 divisor = root->mMatrixCount;
    u32 index = divisor ? (u32)(value % divisor) : (u32)value;
    hdTailUpdateMatrix(&model->mMatrixTarget, index);
    if (wasLocked) root->mFlags = root->mFlags | HdTail_MatrixLocked;
    Mtx34* matrices = hdTailGetMatrices(&root->mMatrixResource, index);
    HdWeightInput* input = mpInput;
    u32 count = input->mCount;
    cXyz* output = mpOutput;
    // HD: retain cache-line clearing, including the original guest address wrap and alignment.
    u32 outputAddress = gabi::ea(output);
    u32 clearEnd = outputAddress + (count * sizeof(cXyz) & ~31u);
    if (outputAddress < clearEnd) {
        for (u32 address = outputAddress; address < clearEnd; address += 32) {
            u32 block = address & ~31u;
            for (u32 byte = 0; byte < 32; byte++) gabi::store<u8>(block + byte, 0);
        }
        input = mpInput;
        count = input->mCount;
    }
    u32 end = outputAddress + count * sizeof(cXyz);
    gabi::Local<cXyz> transformed;
    HdWeightVertex* vertex = input->mVertices;
    for (u32 address = outputAddress; address < end; address += sizeof(cXyz), vertex++) {
        cXyz* destination = gabi::at<cXyz>(address);
        f32 zero = l_zero;
        destination->x = zero;
        destination->z = zero;
        destination->y = zero;
        for (u32 weightIndex = 0; weightIndex < 8; weightIndex++) {
            s8 bone = vertex->mBones[weightIndex];
            if (bone < 0) continue;
            hdTailTransform(&matrices[bone], &vertex->mPosition, transformed.get());
            f32 z = transformed->z;
            f32 weight = vertex->mWeights[weightIndex];
            f32 x = transformed->x;
            f32 y = transformed->y;
            z = gabi::fmuls_ppc(z, weight);
            x = gabi::fmuls_ppc(x, weight);
            y = gabi::fmuls_ppc(y, weight);
            transformed->x = x;
            transformed->y = y;
            transformed->z = z;
            f32 oldX = destination->x;
            f32 oldY = destination->y;
            destination->x = gabi::fadds_ppc(oldX, x);
            destination->y = gabi::fadds_ppc(oldY, y);
            destination->z = gabi::fadds_ppc(destination->z, transformed->z);
        }
    }
}

/* 025EDFA0 */
static void __sinit_m_Do_ext_tail_cpp() {
    l_globals.mThird = 0;
    l_globals.mFirst = 0;
    l_globals.mFourth = 0;
    l_globals.mSecond = 0;
    hdTailRegister(&l_globalRegistration);
    f32 lower = l_lower;
    f32 upper = l_upper;
    l_range.mLower = lower;
    l_range.mUpper = upper;
    hdTailInitSigned(&l_range.mSignedInitialized);
    hdTailRegister(&l_signedRegistration);
    hdTailInitUnsigned(&l_range.mUnsignedInitialized);
    hdTailRegister(&l_unsignedRegistration);
}

/* 025EE034 */
static void mDoExt_inlineDelete(void* self, u32 flags) {
    if (self != NULL && (flags & HdTail_Delete)) hdTailDelete(self);
}

/* 025EE048 */
static u32 mDoExt_getAnimationHeapA() { return l_heapA; }
/* 025EE054 */
static u32 mDoExt_getAnimationHeapB() { return l_heapB; }
/* 025EE060 */
static u32 mDoExt_getCommandHeap() { return l_commandHeap; }
/* 025EE06C */
static u32 mDoExt_getAnimationHeapC() { return l_heapC; }
/* 025EE078 */
static u32 mDoExt_getAnimationHeapD() { return l_heapD; }

/* 025EE084 */
// HD: deleting destructor for the native invisible-joint packet array.
static void mDoExt_invJntPacketArray__dt(HdTailPacketArray* self, u32 flags) {
    if (self == NULL) return;
    HdTailPacket* packets = self->mpPackets;
    self->mVtable = &l_arrayVtable;
    self->mMemberVtable = &l_arrayMemberVtable;
    if (packets != NULL) {
        for (u32 i = 0; (s32)i < (s32)self->mCount; i++) {
            HdTailPacket* packet = &packets[i];
            gabi::call_ptr(packet->mVtable[HdTail_PacketDestroy].get(), packet, 2);
            packets = self->mpPackets;
        }
        HdTailHeap* heap = hdTailFindHeap(l_heapManager.mpHeap, packets);
        gabi::call_ptr(heap->mVtable[HdTail_Free].get(), heap, self->mpPackets.get());
        self->mCount = 0;
        self->mpPackets = NULL;
    }
    hdTailPacketArrayDestroy(self, 0);
    if (flags & HdTail_Delete) hdTailDelete(self);
}

/* 025EE17C */
static HdTailShape* mDoExt_inlineShape__ct(HdTailShape* self) {
    if (self == NULL) {
        self = static_cast<HdTailShape*>(hdTailAllocate(sizeof(HdTailShape)));
        if (self == NULL) return NULL;
    }
    hdTailShapeConstruct(self);
    self->mBuffer = 0;
    self->mVtable = &l_shapeVtable;
    return self;
}

/* WWHD glue (tools/verify/autoglue.py): HD render and animation backend operations.
data l_opaqueDrawBuffer 104B4634
data l_heapManager 101F8B4C
data l_drawState 104B474C
data l_sortVtable 10058E20
data l_arrayVtable 1005874C
data l_arrayMemberVtable 1005875C
data l_shapeVtable 1016D820
data l_zero 100586DC
data l_lower 10058C48
data l_upper 10058C4C
data l_globals 1048CF08
data l_range 1048CEDC
data l_globalRegistration 101F4794
data l_signedRegistration 101F47A0
data l_unsignedRegistration 101F47AC
data l_heapA 1048CF34
data l_heapB 1048CF38
data l_commandHeap 1048CF3C
data l_heapC 1048CF40
data l_heapD 1048CF48
ext 027F1278 void hdTailPacketConstruct(void*)
ext 0273AD10 void* hdTailAllocate(u32)
ext 0273AE48 void* hdTailAllocateAligned(u32, u32)
ext 0273AF40 void hdTailDelete(void*)
ext 027F0ECC void hdTailRemovePacket(void*, void*, u32)
ext 027F0E04 void hdTailEntryPacket(void*, void*, u32)
ext 02750370 void hdTailDrawStateApply(HdTailDrawState*)
ext 027DE778 void hdTailUpdateMatrix(void*, u32)
ext 027DBBB0 Mtx34* hdTailGetMatrices(void*, u32)
ext 028E8F64 void hdTailTransform(Mtx34*, cXyz*, cXyz*)
ext 028F026C void hdTailRegister(void*)
ext 028ED6F8 void hdTailInitSigned(be<u8>*)
ext 028EAB2C void hdTailInitUnsigned(be<u8>*)
ext 02755FEC HdTailHeap* hdTailFindHeap(HdTailHeap*, void*)
ext 025E39C8 void hdTailPacketArrayDestroy(HdTailPacketArray*, u32)
ext 027DA984 void hdTailShapeConstruct(HdTailShape*)
*/
