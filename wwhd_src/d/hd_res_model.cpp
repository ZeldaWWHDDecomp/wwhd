/** HD model resources: bfres model data and one sharcfb archive per model.
 * GameCube-style source verified as hd_res_model; no GameCube counterpart. */
#include "d/hd_res_model.h"

static const u32 safeStringVtable = 0x100E1858;
static const u32 disposerVtable = 0x100E1960;
static const u32 resourceVtable = 0x100E1970;
static hdResModel_globalPointer_c sResourceLoader = {};
static hdResModel_globalPointer_c sHeapManager = {};
static hdResModel_globalPointer_c sGX2Lock = {};

/* 02602AD4 */
static hdResModel_c* hdResModel_construct(hdResModel_c* i_this) {
    if (!i_this) {
        i_this = (hdResModel_c*)operator_new(sizeof(hdResModel_c));
        if (!i_this) return nullptr;
    }
    hdResModel_disposerConstruct(i_this);
    i_this->mDisposerVtable = disposerVtable;
    i_this->mpFile = nullptr;
    i_this->mVtable = resourceVtable;
    // HD: the embedded list address can wrap to null; GHS then allocates a list separately.
    hdResModel_entries_c* entries = gabi::at<hdResModel_entries_c>(
        gabi::ea(i_this) + (u32)offsetof(hdResModel_c, mEntries));
    if (!entries) entries = (hdResModel_entries_c*)operator_new(sizeof(hdResModel_entries_c));
    if (entries) {
        entries->mpEntries = nullptr;
        entries->mCount = 0;
    }
    i_this->mUnknown = 0;
    return i_this;
}

/* 02602B68 */
static void hdResModel_destroy(hdResModel_c* i_this, u32 flags) {
    if (!i_this) return;
    i_this->mVtable = resourceVtable;
    hdResModel_disposerDestroy(i_this, 0);
    if (flags & 1) operator_delete(i_this);
}

/* inlined into hdResModel_load and hdResModel_loadUnlocked */
static void hdResModel_loadFiles(hdResModel_c* i_this, void* archive,
                                hdResModel_safeString_c* name, void* bindArg,
                                hdResModel_heap_c* heap, const char* fileFormat,
                                const char* shaderFormat, bool lock, const char* materialName) {
    // HD: the two format buffers share this exact guest stack frame with shader data and names.
    gabi::Local<hdResModel_loadFrame_c> frame;
    gabi::call_ptr<void>(name->mVtable->mAssureTermination, name);
    hdResModel_path_c* path = hdResModel_formatPath(&frame->mFilePath, fileFormat, name->mStringTop);
    void* data = hdResModel_loadResource(sResourceLoader.mpObject, archive, path, 0);
    hdResModel_file_c* file = hdResModel_castFile(data);
    i_this->mpFile = file;
    hdResModel_relocateFile(file);
    if (bindArg) hdResModel_bindFile(i_this->mpFile, bindArg, 1);
    s32 count = i_this->mpFile->mModelCount;
    if (!count) return;
    hdResModel_heap_c* currentHeap = heap;
    if (!currentHeap) currentHeap = hdResModel_currentHeap(sHeapManager.mpObject);
    hdResModel_entry_c* entries = gabi::call_ptr<hdResModel_entry_c*>(
        currentHeap->mVtable->mAlloc, currentHeap, u32(count) << 4, 4u);
    if (entries) {
        i_this->mEntries.mpEntries = entries;
        i_this->mEntries.mCount = u32(count);
    }
    if (count <= 0) count = 1;
    for (u32 i = 0; count; ++i, --count) {
        // HD: shader allocations use the original heap argument, even when a current heap was found.
        hdResModel_shaderArchive_c* shader = hdResModel_allocShader(sizeof(hdResModel_shaderArchive_c), heap, 4);
        if (shader) shader = hdResModel_constructShader(shader);
        hdResModel_dictionary_c* models = i_this->mpFile->mModels.resolve<hdResModel_dictionary_c>();
        hdResModel_model_c* model = models->mNodes[i].mModel.resolve<hdResModel_model_c>();
        frame->mModelName.__vtbl = safeStringVtable;
        frame->mModelName.mStringTop = gabi::ea(model->mName.resolve<be<u8>>());
        hdResModel_assureTermination(&frame->mModelName);
        // HD: the shared SafeString stores a 32-bit address; re-read it after termination.
        be<u8>* modelName = gabi::at<be<u8>>(frame->mModelName.mStringTop);
        hdResModel_path_c* shaderPath = hdResModel_formatPath(&frame->mShaderPath, shaderFormat, modelName);
        void* shaderData = hdResModel_loadResource(sResourceLoader.mpObject, archive, shaderPath, 0);
        hdResModel_entry_c* entry = i_this->getEntry(i);
        if (shaderData) {
            entry->mpShader = shader;
            i_this->getEntry(i)->mpData = shaderData;
            i_this->getEntry(i)->mUnknown = 0;
            frame->mDataSource.mSize = 0;
            frame->mShaderData.mSize = 0;
            frame->mShaderData.mpData = shaderData;
            frame->mDataSource.mpData = shaderData;
            hdResModel_loadShader(shader, &frame->mShaderData.mpData, &frame->mShaderData.mSize, 0, heap);
            if (lock) hdResModel_lock(sGX2Lock.mpObject);
            hdResModel_setupShader(shader);
            if (lock) hdResModel_unlock(sGX2Lock.mpObject);
            i_this->getEntry(i)->mReady = 1;
            hdResModel_file_c* resFile = i_this->mpFile;
            frame->mMaterialName.__vtbl = safeStringVtable;
            frame->mMaterialName.mStringTop = gabi::guest_addr(materialName);
            hdResModel_bindShader(resFile, shader, &frame->mMaterialName);
        } else {
            entry->mpShader = nullptr;
            i_this->getEntry(i)->mpData = nullptr;
            i_this->getEntry(i)->mUnknown = 0;
        }
    }
}

/* 02602BC8 */
static void hdResModel_load(hdResModel_c* i_this, void* archive, hdResModel_safeString_c* name,
                            void* bindArg, hdResModel_heap_c* heap) {
    hdResModel_loadFiles(i_this, archive, name, bindArg, heap, "%s.bfres", "%s.sharcfb", true, "Mat");
}

/* 02602ECC */
static void hdResModel_loadNoBind(hdResModel_c* i_this, void* archive,
                                 hdResModel_safeString_c* name, hdResModel_heap_c* heap) {
    hdResModel_load(i_this, archive, name, nullptr, heap);
}

/* 02602ED8 */
static void hdResModel_loadUnlocked(hdResModel_c* i_this, void* archive, hdResModel_safeString_c* name,
                                    void* bindArg, hdResModel_heap_c* heap) {
    hdResModel_loadFiles(i_this, archive, name, bindArg, heap, "%s.bfres", "%s.sharcfb", false, "Mat");
}

/* 026031C4 */
static void hdResModel_loadUnlockedNoBind(hdResModel_c* i_this, void* archive,
                                         hdResModel_safeString_c* name, hdResModel_heap_c* heap) {
    hdResModel_loadUnlocked(i_this, archive, name, nullptr, heap);
}

/* 026031D0 */
static hdResModel_shaderArchive_c* hdResModel_getShader(hdResModel_c* i_this, u32 idx) {
    return i_this->getEntry(idx)->mpShader;
}

/* ---------------- HD: compiler-generated code ---------------- */

/* 026031F0 */
static void __sinit_hd_res_model_cpp() {
    sinit_header_statics_rodata(0x1048DCF8, 0x101F4EE0, 0x1048DD04, 0x100E1958);
}

/* WWHD glue
data sResourceLoader 101F4F7C nocheck
data sHeapManager 101F8B4C nocheck
data sGX2Lock 101F8B18 nocheck
str 100E1920 "Mat" in 02602BC8
str 100E1924 "%s.bfres" in 02602BC8
str 100E1930 "%s.sharcfb" in 02602BC8
str 100E193C "Mat" in 02602ED8
str 100E1940 "%s.bfres" in 02602ED8
str 100E194C "%s.sharcfb" in 02602ED8
ext 02752A84 void hdResModel_disposerConstruct(hdResModel_c* model)
ext 02752BEC void hdResModel_disposerDestroy(hdResModel_c* model, u32 flags)
ext 026032F0 hdResModel_path_c* hdResModel_formatPath(hdResModel_path_c* path, const char* format, be<u8>* name)
ext 026124F4 void* hdResModel_loadResource(void* loader, void* archive, hdResModel_path_c* path, u32 flags)
ext 027E2DC0 hdResModel_file_c* hdResModel_castFile(void* data)
ext 027E2DC4 void hdResModel_relocateFile(hdResModel_file_c* file)
ext 027E2EA4 void hdResModel_bindFile(hdResModel_file_c* file, void* bindArg, u32 flags)
ext 02756140 hdResModel_heap_c* hdResModel_currentHeap(void* manager)
ext 0273B050 hdResModel_shaderArchive_c* hdResModel_allocShader(u32 size, hdResModel_heap_c* heap, u32 alignment)
ext 027B744C hdResModel_shaderArchive_c* hdResModel_constructShader(hdResModel_shaderArchive_c* shader)
ext 02603298 void hdResModel_assureTermination(SafeString* name)
ext 027B8904 void hdResModel_loadShader(hdResModel_shaderArchive_c* shader, gptr<void>* data, be<u32>* size, u32 flags, hdResModel_heap_c* heap)
ext 0274FBF8 void hdResModel_lock(void* lock)
ext 0274FCCC void hdResModel_unlock(void* lock)
ext 027B9070 void hdResModel_setupShader(hdResModel_shaderArchive_c* shader)
ext 027735DC void hdResModel_bindShader(hdResModel_file_c* file, hdResModel_shaderArchive_c* shader, SafeString* materialName)
*/
