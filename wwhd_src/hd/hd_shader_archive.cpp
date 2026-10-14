/* HD shader resources for primitive drawing, particles and render buffers. Private decomp source. */
#include "hd/hd_shader_archive.h"
static hdShaderArchiveHeaderZero_c sShaderArchiveHeaderZero;
static hdShaderArchiveHeaderDescriptor_c sShaderArchiveHeaderDescriptor;
static hdShaderArchiveHeaderDescriptor_c sShaderArchiveRotationDescriptor;
static hdShaderArchiveHeaderDescriptor_c sShaderArchivePositiveRotationDescriptor;
static f32 sShaderArchiveMinRotation;
static f32 sShaderArchiveMaxRotation;
static u8 sShaderArchiveRotationGuard;
static u8 sShaderArchivePositiveRotationGuard;
static void initializeRotationGuard(void* guard);
static void initializePositiveRotationGuard(void* guard);
static hdShaderArchiveMgr_c* sShaderArchiveShaderManager;
static hdShaderArchiveMgr_c* sShaderArchiveShaderManagerDisposer;
static void* sShaderArchiveResourceManager;
static void* sShaderArchiveHeapManager;
static u32 sShaderArchiveTypeGuard;
static u32 sShaderArchiveTypeDescriptor;
static u32 sShaderArchiveFileType;
static u32 sShaderArchiveSafeStringVtable;
static u32 sShaderArchiveManagerVtable;
static void* allocateShaderManager(u32 size, void* heap, u32 alignment);
static void constructShaderDisposer(hdShaderArchiveMgr_c* manager, void* heap, u32 mode);
static void* allocateShaderAuxiliary(u32 size);
static hdShaderFileResource_l* loadShaderFile(void* manager, hdShaderLoadArg_c* argument);
static void constructShaderArchive(hdShaderArchive_c* archive);
static void setupShaderArchive(hdShaderArchive_c* archive, hdShaderBinaryArg_c* binary, hdShaderWorkArg_c* work, u32 flags, u32 mode);
static BOOL compileShaderArchive(hdShaderArchive_c* archive);
static hdShaderHeap_l* getShaderHeap();

/* 02738C0C */
static s32 shaderManagerNoAction() { return 0; }
/* 02738CD0 */
static hdShaderArchiveMgr_c* createShaderManager(void* heap) {
    hdShaderArchiveMgr_c* manager = sShaderArchiveShaderManager;
    if (manager == NULL) {
        manager = (hdShaderArchiveMgr_c*)allocateShaderManager(sizeof(hdShaderArchiveMgr_c), heap, 4);
        hdShaderArchiveMgr_c* disposer = manager;
        if (disposer != NULL) {
            constructShaderDisposer(disposer, heap, 3);
            disposer->mVtable = gabi::ea(&sShaderArchiveManagerVtable);
        }
        sShaderArchiveShaderManagerDisposer = disposer;
        if (manager != NULL) {
            be<u32>* array = gabi::at<be<u32>>(gabi::ea(manager)+offsetof(hdShaderArchiveMgr_c, mCapacity));
            if (array == NULL) array = (be<u32>*)allocateShaderAuxiliary(8);
            if (array != NULL) {
                array[1] = 0;
                array[0] = 0;
            }
            manager->mLoaded = 0;
            manager->mCount = 0;
        }
        sShaderArchiveShaderManager = manager;
    }
    return manager;
}
/* inlined into shader archive loaders */
static void loadArchive(hdShaderArchiveMgr_c* manager, const char* path) {
    void* resourceManager = sShaderArchiveResourceManager;
    hdShaderLoadArg_c argument;
    argument.mFlags = 0;
    argument.mReadFlags = 0;
    argument.mAlignment = 32;
    argument.mReadSize = 0;
    argument.mFileDevice = 0;
    argument.mPath.__vtbl = gabi::ea(&sShaderArchiveSafeStringVtable);
    argument.mPath.mStringTop = gabi::guest_addr(path);
    argument.mBuffer = 0;
    argument.mBufferSize = 256;
    argument.mOffset = 0;
    argument.mHeap = 0;
    hdShaderFileResource_l* resource = loadShaderFile(resourceManager, &argument);
    if (sShaderArchiveTypeGuard == 0) {
        sShaderArchiveTypeGuard = 1;
        sShaderArchiveTypeDescriptor = gabi::ea(&sShaderArchiveFileType);
    }
    if (resource != NULL) {
        BOOL matches = gabi::call_ptr<BOOL>(resource->mpVtable->mIsKindOf, resource, &sShaderArchiveTypeDescriptor);
        if (!matches) resource = NULL;
    }
    hdShaderArchive_c* archive = manager->currentArchive();
    hdShaderBinaryArg_c binary;
    hdShaderWorkArg_c work;
    binary.mpBinary = resource->mpBinary;
    work.mWork = 0;
    setupShaderArchive(archive, &binary, &work, 0, 0);
    archive = manager->currentArchive();
    compileShaderArchive(archive);
}
/* 02738D9C */
void hdShaderArchiveMgr_c::loadPrimitive() { loadArchive(this, "shaders/Prim/wii_pipeline.sharcfb"); }
/* 02738EEC */
void hdShaderArchiveMgr_c::loadParticle() { loadArchive(this, "shaders/particle/wii_pipeline.sharcfb"); }
/* 0273903C */
void hdShaderArchiveMgr_c::loadRenderBuffer() { loadArchive(this, "shaders/render_buffer/render_buffer.sharcfb"); }
/* 0273918C */
void hdShaderArchiveMgr_c::initialize() {
    // GHS leaves the singleton value in r3, although the heap accessor does not read it.
    hdShaderHeap_l* heap = getShaderHeap();
    u32 allocate = heap->mpVtable->mAllocate;
    hdShaderArchive_c* archives = gabi::call_ptr<hdShaderArchive_c*>(allocate, heap, 3 * sizeof(hdShaderArchive_c), 4);
    for (u32 index = 0; index < 3; ++index) {
        hdShaderArchive_c* archive = gabi::at<hdShaderArchive_c>(gabi::ea(archives) + index * sizeof(hdShaderArchive_c));
        if (archive != NULL) {
            constructShaderArchive(archive);
            archive->mpName.v = gabi::guest_addr("");
            archive->mNameVtable = gabi::ea(&sShaderArchiveSafeStringVtable);
        }
    }
    if (archives != NULL) {
        mpArchives = archives;
        mCapacity = 3;
    }
    loadPrimitive();
    mCount = mCount + 1;
    loadParticle();
    mCount = mCount + 1;
    loadRenderBuffer();
    mCount = 3;
    mLoaded = 1;
}
/* 027392EC */
static void __sinit_hd_shader_archive_cpp() {
    for (u32 i = 0; i < 4; ++i) sShaderArchiveHeaderZero.mWords[i] = 0;
    __register_global_object(gabi::ea(&sShaderArchiveHeaderDescriptor));
    sShaderArchiveMinRotation = -3.1415927410125732f;
    sShaderArchiveMaxRotation = 3.1415927410125732f;
    initializeRotationGuard(&sShaderArchiveRotationGuard);
    __register_global_object(gabi::ea(&sShaderArchiveRotationDescriptor));
    initializePositiveRotationGuard(&sShaderArchivePositiveRotationGuard);
    __register_global_object(gabi::ea(&sShaderArchivePositiveRotationDescriptor));
}
/* WWHD glue
 data sShaderArchiveSafeStringVtable 10142D70 nocheck
 data sShaderArchiveManagerVtable 10142E28 nocheck
 data sShaderArchiveHeaderZero 1049FB40
 data sShaderArchiveHeaderDescriptor 101F8A70
 data sShaderArchiveRotationDescriptor 101F8A7C
 data sShaderArchivePositiveRotationDescriptor 101F8A88
 data sShaderArchiveMinRotation 1049FB34
 data sShaderArchiveMaxRotation 1049FB38
 data sShaderArchiveRotationGuard 1049FB3C
 data sShaderArchivePositiveRotationGuard 1049FB3D
 ext 028ED6F8 void initializeRotationGuard(void* guard)
 ext 028EAB2C void initializePositiveRotationGuard(void* guard)
 data sShaderArchiveShaderManager 101F8A94
 data sShaderArchiveShaderManagerDisposer 101F8A98
 data sShaderArchiveResourceManager 101F8B68
 data sShaderArchiveHeapManager 101F8B4C
 data sShaderArchiveTypeGuard 101FD6E0
 data sShaderArchiveTypeDescriptor 101FD944
 data sShaderArchiveFileType 10142D98
 str 10142DA8
 str 10142DCC
 str 10142DF4
 str 10000160 "" in 0273918C
 ext 0273B0D4 void* allocateShaderManager(u32 size, void* heap, u32 alignment)
 ext 02752B0C void constructShaderDisposer(hdShaderArchiveMgr_c* manager, void* heap, u32 mode)
 ext 0273AD10 void* allocateShaderAuxiliary(u32 size)
 ext 0275DB94 hdShaderFileResource_l* loadShaderFile(void* manager, hdShaderLoadArg_c* argument)
 ext 027B744C void constructShaderArchive(hdShaderArchive_c* archive)
 ext 027B8904 void setupShaderArchive(hdShaderArchive_c* archive, hdShaderBinaryArg_c* binary, hdShaderWorkArg_c* work, u32 flags, u32 mode)
 ext 027B9070 BOOL compileShaderArchive(hdShaderArchive_c* archive)
 ext 02756140 hdShaderHeap_l* getShaderHeap()
*/
