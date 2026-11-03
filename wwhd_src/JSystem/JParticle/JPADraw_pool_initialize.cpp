// HD: fixed GPU pool populations and shader binding initialization for JPA1.
#include "JSystem/JParticle/JPADraw_gpu_pools.h"
#include "JSystem/JParticle/JPADraw_initialize.h"

using namespace JPADrawGpu;
using namespace JPADrawExtension;
using JPADrawRootHeapPointer = gptr<heap_adapters::Heap>;
static JPADrawRootHeapPointer sPoolRootHeap;
static const u32 sPoolSafeStringVtable;
static const u32 sRegularVertexFormat;
static const u32 sParticleVertexFormat;
heap_adapters::Heap* JPADrawPoolCurrentHeap(heap_adapters::Heap*);
jpa_emitter_extension::JPAShaderMaterialSet* JPADrawPoolProgramArchive();
s32 JPADrawPoolFindProgram(jpa_emitter_extension::JPAShaderProgram*,const SafeString*);
void JPADrawPoolPrepareProgram(jpa_emitter_extension::JPAShaderMaterial*,jpa_emitter_extension::JPAShaderResource*,u32);
void JPADrawPoolPrepareVertices(JPAGpuVertexBuffer*,void*,u32,const be<u32>*);
void JPADrawPoolBindShape(void*,void*,JPAGpuVertexBuffer*,const be<u32>*,heap_adapters::Heap*);
/* 0282F050 */
void JPAManagerGpuPools_l::initializeBuffers(heap_adapters::Heap* heap) {
    auto allocateQueue = [&](jpa_emitter_extension::JPAGpuQueue& queue,u32 capacity) {
        auto currentHeap = JPADrawPoolCurrentHeap(sPoolRootHeap);
        u32 allocation = currentHeap->alloc(capacity * sizeof(u32),4);
        if (allocation) {
            queue.mIndices = gabi::at<be<u32>>(allocation);
            queue.mCapacity = capacity;
            queue.mCursor = 0;
            queue.mCount = 0;
        }
    };
    allocateQueue(mFieldQueue,100);
    allocateQueue(mParticleQueue,1500);
    allocateQueue(mMediumQueue,20);
    allocateQueue(mLargeQueue,20);
    auto resolveProgram = [&](const char* name) {
        SafeString key;
        key.__vtbl = gabi::ea(&sPoolSafeStringVtable);
        key.mTextPtr = gabi::guest_addr(name);
        auto archive = JPADrawPoolProgramArchive();
        s32 index = JPADrawPoolFindProgram(archive->mProgram.get(),&key);
        if (index < 0) return static_cast<jpa_emitter_extension::JPAShaderMaterial*>(nullptr);
        u32 count = archive->mCount;
        auto programs = archive->mMaterials.get();
        auto program = u32(index) < count ? &programs[index] : programs;
        if (!u8(program->mReady)) {
            auto resource = archive->mProgram.get();
            jpa_emitter_extension::JPAShaderResource* data = nullptr;
            if (u32(index) < u32(resource->mResourceCount)) data = &resource->mResources.get()[index];
            JPADrawPoolPrepareProgram(program,data,0);
            count = archive->mCount;
            programs = archive->mMaterials.get();
        }
        return u32(index) < count ? &programs[index] : programs;
    };
    mFieldMaterial = resolveProgram("jparticle_type_a");
    mParticleMaterial = resolveProgram("smoke_effect");
    mLargeMaterial = resolveProgram("track_effect");
    auto initializePool = [&](auto& pool,u32 buffersPerBank,u32 vertexCount,u32 allocationSize,u32 elementSize,const void* format,void* material) {
        pool.mState.mFormat = const_cast<void*>(format);
        pool.mState.mCursor = elementSize;
        for (u32 bank = 0; bank < 2; ++bank) {
            for (u32 j = 0; j < buffersPerBank; ++j) {
                auto& buffer = pool.mBuffers[bank + 2 * j];
                void* data = buffer.mBufferData.get();
                if (!data) {
                    auto allocationHeap = heap;
                    if (!allocationHeap) allocationHeap = JPADrawPoolCurrentHeap(sPoolRootHeap);
                    u32 allocation = allocationHeap->alloc(allocationSize,64);
                    if (allocation) {
                        buffer.mAllocation = gabi::at<void>(allocation);
                        buffer.mCapacity = vertexCount;
                    }
                    data = buffer.mAllocation.get();
                    buffer.mBufferData = data;
                }
                JPADrawPoolPrepareVertices(&buffer.mVertexBuffer,data,vertexCount,&pool.mState.mCursor);
                if (material && material != pool.mState.mMaterial.get())
                    JPADrawPoolBindShape(material,buffer.mShape,&buffer.mVertexBuffer,&pool.mState.mCursor,heap);
            }
        }
        pool.mState.mInitialized = 1;
        pool.mState.mMaterial = material;
    };
    auto enqueue = [&](jpa_emitter_extension::JPAGpuQueue& queue,u32 index) {
        s32 capacity = queue.mCapacity;
        s32 count = queue.mCount;
        if (count >= capacity) return;
        s32 cursor = queue.mCursor;
        count = queue.mCount;
        capacity = queue.mCapacity;
        s32 position = cursor + count;
        queue.mCount = count + 1;
        if (position >= capacity) position -= capacity;
        queue.mIndices.get()[position] = index;
    };
    for (u32 i = 0; i < 100; ++i) {
        initializePool(mFieldPools[i],2,750,15000,0x11,&sRegularVertexFormat,mFieldMaterial.get());
        enqueue(mFieldQueue,i);
    }
    for (u32 i = 0; i < 1500; ++i) {
        initializePool(mParticlePools[i],1,6,288,0x1013,&sParticleVertexFormat,mParticleMaterial.get());
        enqueue(mParticleQueue,i);
    }
    for (u32 i = 0; i < 20; ++i) {
        initializePool(mMediumPools[i],5,160,3200,0x11,&sRegularVertexFormat,mFieldMaterial.get());
        enqueue(mMediumQueue,i);
    }
    for (u32 i = 0; i < 20; ++i) {
        initializePool(mLargePools[i],150,6,120,0x11,&sRegularVertexFormat,mLargeMaterial.get());
        enqueue(mLargeQueue,i);
    }
}
/* WWHD glue
data sPoolRootHeap 101F8B4C
data sPoolSafeStringVtable 10171F28
data sRegularVertexFormat 10172214
data sParticleVertexFormat 1017221C
str 101721DC
str 101721F0
str 10172200
ext 02756140 heap_adapters::Heap* JPADrawPoolCurrentHeap(heap_adapters::Heap*)
ext 027FFCBC jpa_emitter_extension::JPAShaderMaterialSet* JPADrawPoolProgramArchive()
ext 027B90AC s32 JPADrawPoolFindProgram(jpa_emitter_extension::JPAShaderProgram*,const SafeString*)
ext 02800B0C void JPADrawPoolPrepareProgram(jpa_emitter_extension::JPAShaderMaterial*,jpa_emitter_extension::JPAShaderResource*,u32)
ext 027FF478 void JPADrawPoolPrepareVertices(JPAGpuVertexBuffer*,void*,u32,const be<u32>*)
ext 027FF530 void JPADrawPoolBindShape(void*,void*,JPAGpuVertexBuffer*,const be<u32>*,heap_adapters::Heap*)
*/
