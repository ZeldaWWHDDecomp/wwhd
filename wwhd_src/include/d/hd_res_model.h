/* HD bfres models and per-model shader archives; no GameCube counterpart.
 * Relative pointers use unsigned 32-bit address addition, including wrap to zero. */
#pragma once
#include "bindings.h"

struct hdResModel_relativePtr_c {
    be<u32> mOffset;
    template<class T> T* resolve() {
        u32 offset = mOffset;
        return offset ? gabi::at<T>(gabi::ea(this) + offset) : nullptr;
    }
};
WWHD_SIZE(hdResModel_relativePtr_c, 4);
struct hdResModel_model_c { u8 _00[4]; hdResModel_relativePtr_c mName; };
WWHD_OFFSET(hdResModel_model_c, mName, 4);
WWHD_SIZE(hdResModel_model_c, 8);
struct hdResModel_dictionaryNode_c {
    hdResModel_relativePtr_c mModel;
    u8 _04[12];
};
WWHD_OFFSET(hdResModel_dictionaryNode_c, mModel, 0);
WWHD_SIZE(hdResModel_dictionaryNode_c, 16);
struct hdResModel_dictionary_c {
    u8 _00[0x24];
    hdResModel_dictionaryNode_c mNodes[1];
};
WWHD_OFFSET(hdResModel_dictionary_c, mNodes, 0x24);
WWHD_SIZE(hdResModel_dictionary_c, 0x34); // HD: prefix through the first variable-length node.
struct hdResModel_file_c {
    /* 0x00 */ u8 _00[0x20];
    /* 0x20 */ hdResModel_relativePtr_c mModels;
    /* 0x24 */ u8 _24[0x50 - 0x24];
    /* 0x50 */ be<u16> mModelCount;
};
WWHD_OFFSET(hdResModel_file_c, mModels, 0x20);
WWHD_OFFSET(hdResModel_file_c, mModelCount, 0x50);
WWHD_SIZE(hdResModel_file_c, 0x52);
struct hdResModel_shaderArchive_c { u8 _00[0x54]; };
WWHD_SIZE(hdResModel_shaderArchive_c, 0x54);
struct hdResModel_entry_c {
    /* 0x0 */ gptr<hdResModel_shaderArchive_c> mpShader;
    /* 0x4 */ gptr<void> mpData;
    /* 0x8 */ be<u32> mUnknown;
    /* 0xC */ be<u8> mReady;
    /* 0xD */ u8 _D[3];
};
WWHD_OFFSET(hdResModel_entry_c, mpShader, 0);
WWHD_OFFSET(hdResModel_entry_c, mpData, 4);
WWHD_OFFSET(hdResModel_entry_c, mUnknown, 8);
WWHD_OFFSET(hdResModel_entry_c, mReady, 0xC);
WWHD_SIZE(hdResModel_entry_c, 0x10);
struct hdResModel_entries_c {
    be<u32> mCount;
    gptr<hdResModel_entry_c> mpEntries;
};
WWHD_SIZE(hdResModel_entries_c, 8);
struct hdResModel_c {
    /* 0x00 */ u8 mDisposer[0xC];
    /* 0x0C */ be<u32> mDisposerVtable;
    /* 0x10 */ be<u32> mVtable;
    /* 0x14 */ gptr<hdResModel_file_c> mpFile;
    /* 0x18 */ hdResModel_entries_c mEntries;
    /* 0x20 */ be<u32> mUnknown;
    hdResModel_entry_c* getEntry(u32 idx) {
        u32 count = mEntries.mCount;
        hdResModel_entry_c* entries = mEntries.mpEntries;
        return idx < count ? entries + idx : entries;
    }
};
WWHD_OFFSET(hdResModel_c, mDisposerVtable, 0xC);
WWHD_OFFSET(hdResModel_c, mVtable, 0x10);
WWHD_OFFSET(hdResModel_c, mpFile, 0x14);
WWHD_OFFSET(hdResModel_c, mEntries, 0x18);
WWHD_OFFSET(hdResModel_c, mUnknown, 0x20);
WWHD_SIZE(hdResModel_c, 0x24);

struct hdResModel_safeStringVt_c { u8 _00[0x14]; be<u32> mAssureTermination; };
WWHD_OFFSET(hdResModel_safeStringVt_c, mAssureTermination, 0x14);
WWHD_SIZE(hdResModel_safeStringVt_c, 0x18);
struct hdResModel_safeString_c {
    gptr<be<u8>> mStringTop;
    gptr<hdResModel_safeStringVt_c> mVtable;
};
WWHD_OFFSET(hdResModel_safeString_c, mStringTop, 0);
WWHD_OFFSET(hdResModel_safeString_c, mVtable, 4);
WWHD_SIZE(hdResModel_safeString_c, 8);
struct hdResModel_path_c { u8 _000[0x110]; };
WWHD_SIZE(hdResModel_path_c, 0x110);
struct hdResModel_dataRange_c { gptr<void> mpData; be<u32> mSize; };
WWHD_SIZE(hdResModel_dataRange_c, 8);
struct hdResModel_loadFrame_c {
    /* 0x000 */ u8 _000[8];
    /* 0x008 */ SafeString mModelName;
    /* 0x010 */ SafeString mMaterialName;
    /* 0x018 */ hdResModel_dataRange_c mDataSource;
    /* 0x020 */ hdResModel_dataRange_c mShaderData;
    /* 0x028 */ hdResModel_path_c mShaderPath;
    /* 0x138 */ hdResModel_path_c mFilePath;
};
WWHD_OFFSET(hdResModel_loadFrame_c, mModelName, 8);
WWHD_OFFSET(hdResModel_loadFrame_c, mMaterialName, 0x10);
WWHD_OFFSET(hdResModel_loadFrame_c, mDataSource, 0x18);
WWHD_OFFSET(hdResModel_loadFrame_c, mShaderData, 0x20);
WWHD_OFFSET(hdResModel_loadFrame_c, mShaderPath, 0x28);
WWHD_OFFSET(hdResModel_loadFrame_c, mFilePath, 0x138);
WWHD_SIZE(hdResModel_loadFrame_c, 0x248);
struct hdResModel_globalPointer_c { gptr<void> mpObject; };
WWHD_SIZE(hdResModel_globalPointer_c, 4);
struct hdResModel_heapVt_c { u8 _00[0x34]; be<u32> mAlloc; };
WWHD_OFFSET(hdResModel_heapVt_c, mAlloc, 0x34);
WWHD_SIZE(hdResModel_heapVt_c, 0x38);
struct hdResModel_heap_c { u8 _00[0xC]; gptr<hdResModel_heapVt_c> mVtable; };
WWHD_OFFSET(hdResModel_heap_c, mVtable, 0xC);
WWHD_SIZE(hdResModel_heap_c, 0x10);

void hdResModel_disposerConstruct(hdResModel_c* model);
void hdResModel_disposerDestroy(hdResModel_c* model, u32 flags);
hdResModel_path_c* hdResModel_formatPath(hdResModel_path_c* path, const char* format, be<u8>* name);
void* hdResModel_loadResource(void* loader, void* archive, hdResModel_path_c* path, u32 flags);
hdResModel_file_c* hdResModel_castFile(void* data);
void hdResModel_relocateFile(hdResModel_file_c* file);
void hdResModel_bindFile(hdResModel_file_c* file, void* bindArg, u32 flags);
hdResModel_heap_c* hdResModel_currentHeap(void* manager);
hdResModel_shaderArchive_c* hdResModel_allocShader(u32 size, hdResModel_heap_c* heap, u32 alignment);
hdResModel_shaderArchive_c* hdResModel_constructShader(hdResModel_shaderArchive_c* shader);
void hdResModel_assureTermination(SafeString* name);
void hdResModel_loadShader(hdResModel_shaderArchive_c* shader, gptr<void>* data, be<u32>* size, u32 flags, hdResModel_heap_c* heap);
void hdResModel_lock(void* lock);
void hdResModel_unlock(void* lock);
void hdResModel_setupShader(hdResModel_shaderArchive_c* shader);
void hdResModel_bindShader(hdResModel_file_c* file, hdResModel_shaderArchive_c* shader, SafeString* materialName);
