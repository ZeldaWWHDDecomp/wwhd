// Render view storage (RenderQueueManager::initialize 027C0CEC): the pending list, the job tables (two fixed jobs plus one
// per pending layer, and one secondary job per layer), the deferred/submitted command lists and their command table,
// then the view's display name, its logical surface size and the job kinds.
#include "agl/DisplayFrameState.h"
#include "agl/FormattedRenderText.h"
#include <bit>
using namespace agl::render;
static DisplayFloatConstant sStorageZero;
static DisplayDoubleConstant sStorageSignedBias;
// The display-name getter (027C2008) ignores r3; the caller passes the display-owner singleton word.
static DisplayObjectPointer sDisplayNameOwner;
void createDisplayName(void*,sead::SafeText*,u32);
// The command constructor (AGL-GX2#1 027DA694, file-local there).
agl::gx2util::TimedCounter* constructRecordedCommand(agl::gx2util::TimedCounter*);
struct IntegerConversion { be<u32> mHigh,mLow; };
struct StorageZeros { be<f32> mValues[4]; };
static f64 encodedInteger(const IntegerConversion* words) {
    return std::bit_cast<f64>(((u64)(u32)words->mHigh << 32) | (u32)words->mLow);
}
// Each table allocation loads the heap's table and its allocate slot into scratch registers before the indirect call
// (027D6130/6134: r7, r9 for the primary jobs; 027D61A4/61A8: r12, r0 for the secondary jobs; 027D6238/623C: r8, r10 for
// the command table). r6..r10 reach 027D778C as raw save state, so they are kept as the original
// leaves them when the allocator does not overwrite them.
struct TableRegisters { u32 mTable, mFunction; };
static constexpr TableRegisters kPrimaryJobRegisters{7,9};
static constexpr TableRegisters kSecondaryJobRegisters{12,0};
static constexpr TableRegisters kCommandRegisters{8,10};
static void* allocateTable(sead::RegistryHeap* requested,u32 bytes,TableRegisters registers) {
    sead::RegistryHeap* heap = requested;
    if (!heap) heap = registryCurrentHeap(sRegistryManager);
    sead::RegistryHeapDispatch* table = heap->dispatch();
    u32 allocate = table->mAllocate.mFunction;
    gabi::cpu->r[registers.mTable] = gabi::ea(table);
    gabi::cpu->r[registers.mFunction] = allocate;
    return gabi::call_ptr<void*>(allocate,heap,bytes,4);
}
static RenderJob* constructJobs(sead::RegistryHeap* heap,u32 count,TableRegisters registers) {
    RenderJob* jobs = static_cast<RenderJob*>(allocateTable(heap,count * sizeof(RenderJob),registers));
    for (u32 index = 0; index < count; ++index) {
        RenderJob* job = &jobs[index];
        if (gabi::ea(job)) createRenderJob(job);
    }
    return jobs;
}
/* 027D60D4 */
void agl::render::DisplayFrameState::allocateFrameStorage(u32 category,const DisplayIntegerExtent* logicalSize,const DisplayIntegerExtent* physicalSize,s32 pendingCapacity,sead::RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    mCategory = category;
    allocateInputArray(&mPending,pendingCapacity,heap,4);
    u32 primaryCount = (u32)pendingCapacity + 2u;
    if ((s32)primaryCount > 0) {
        RenderJob* jobs = constructJobs(heap,primaryCount,kPrimaryJobRegisters);
        if (jobs) {
            mPrimaryJobs.mCount = primaryCount;
            mPrimaryJobs.mJobs = jobs;
        }
    }
    if (pendingCapacity > 0) {
        RenderJob* jobs = constructJobs(heap,(u32)pendingCapacity,kSecondaryJobRegisters);
        if (jobs) {
            mSecondaryJobs.mCount = pendingCapacity;
            mSecondaryJobs.mJobs = jobs;
        }
    }
    mDeferred.allocateStorage((s32)((u32)pendingCapacity * 2u + 8u),heap,4);
    u32 commandCapacity = mDeferred.mCapacity;
    u32 submittedCapacity = commandCapacity;
    if ((s32)commandCapacity > 0) {
        auto* commands = static_cast<agl::gx2util::TimedCounter*>(allocateTable(heap,commandCapacity * sizeof(agl::gx2util::TimedCounter),kCommandRegisters));
        for (u32 index = 0; index < commandCapacity; ++index) {
            agl::gx2util::TimedCounter* command = &commands[index];
            if (gabi::ea(command)) constructRecordedCommand(command);
        }
        submittedCapacity = mDeferred.mCapacity;
        if (commands) {
            mSubmittedCommands.mCount = commandCapacity;
            mSubmittedCommands.mCommands = commands;
        }
    }
    mSubmitted.allocateStorage((s32)submittedCapacity,heap,4);
    gabi::FrameLocal<sead::SafeText> name(0x28);
    createDisplayName(sDisplayNameOwner,name.get(),mCategory);
    name->assureTermination();
    gabi::FrameLocal<sead::PathText> formatted(0x30);
    // 778C saves the physical EABI argument bank: r6..r10 are only what the calls above left in those registers
    // (exact save-state preservation, no semantic arguments).
    u32 residue6 = gabi::cpu->r[6],residue7 = gabi::cpu->r[7],residue8 = gabi::cpu->r[8];
    u32 residue9 = gabi::cpu->r[9],residue10 = gabi::cpu->r[10];
    gabi::cpu->cr[6] = 0;
    createFormattedRenderText(formatted.get(),"%s\x89\xe6\x96\xca",gabi::ea(name->mText),residue6,residue7,residue8,residue9,residue10);
    f32 zero = sStorageZero;
    f64 bias = sStorageSignedBias;
    mFrameBuffer.mArea.mMinY = zero;
    mFrameBuffer.mArea.mMaxY = zero;
    mFrameBuffer.mArea.mMinX = zero;
    mFrameBuffer.mArea.mMaxX = zero;
    u32 width = physicalSize->x,height = physicalSize->y;
    gabi::FrameLocal<IntegerConversion> conversion(8);
    conversion->mHigh = 0x43300000u;
    conversion->mLow = width ^ 0x80000000u;
    f64 encodedWidth = encodedInteger(conversion.get());
    conversion->mLow = height ^ 0x80000000u;
    f64 encodedHeight = encodedInteger(conversion.get());
    f64 wideWidth = gabi::fsub_ppc(encodedWidth,bias);
    gabi::FrameLocal<StorageZeros> zeros(0x18);
    zeros->mValues[0] = zero;
    f64 wideHeight = gabi::fsub_ppc(encodedHeight,bias);
    zeros->mValues[2] = zero;
    zeros->mValues[3] = zero;
    f32 physicalWidth = (f32)wideWidth;
    f32 physicalHeight = (f32)wideHeight;
    zeros->mValues[1] = zero;
    mFrameBuffer.mSize.mWidth = physicalWidth;
    mFrameBuffer.mSize.mHeight = physicalHeight;
    width = logicalSize->x;
    bias = sStorageSignedBias;
    height = logicalSize->y;
    conversion->mHigh = 0x43300000u;
    conversion->mLow = width ^ 0x80000000u;
    encodedWidth = encodedInteger(conversion.get());
    conversion->mLow = height ^ 0x80000000u;
    encodedHeight = encodedInteger(conversion.get());
    gabi::FrameLocal<sead::RenderVector2> logical(0x10);
    logical->x = (f32)gabi::fsub_ppc(encodedWidth,bias);
    logical->y = (f32)gabi::fsub_ppc(encodedHeight,bias);
    setSurfaceExtent(logical.get());
    GuestRange<RenderJob> primary = mPrimaryJobs.range();
    RenderJob* first = primary.first;
    RenderJob* end = primary.end;
    mEmbeddedBuffer.mColors[0] = &mSurfaceB;
    // The job index comes from the address the previous configureLayerCommand returned (it returns its job).
    for (RenderJob* job = first; job != end;) {
        u32 index = (gabi::ea(job) - gabi::ea(first)) / sizeof(RenderJob);
        u32 kind = index == 0 ? 1u : index == 1 ? 2u : 0u;
        job = job->configureLayerCommand(kind,this) + 1;
    }
    RenderJob* job;
    GuestRange<RenderJob> secondary = mSecondaryJobs.range();
    job = secondary.first;
    end = secondary.end;
    while (job != end) job = job->configureLayerCommand(3,this) + 1;
}
/* WWHD glue
data sDisplayNameOwner 101F95D0
data sStorageZero 1016C290
data sStorageSignedBias 1016C2E8
str 1016C2F0 "%s\x89\xe6\x96\xca"
ext 027C2008 void createDisplayName(void*,sead::SafeText*,u32)
ext 027DA694 agl::gx2util::TimedCounter* constructRecordedCommand(agl::gx2util::TimedCounter*)
*/
