// Views of the five saved shader words and the archive's editing records.
#pragma once
#include "wwhd.h"
#include "agl/ShaderCommandState.h"
namespace agl::environment {
struct ShaderVector { be<f32> x,y,z; };
struct SavedBindingWords { be<u32> mWords[5]; };
struct ShaderRecord {
    gptr<void> mName; u8 mBeforeBinding[0x10]; SavedBindingWords mBinding;
    u8 mTail[0x14];
};
struct ProgramDescriptor { u8 mBeforeEditorIndex[0xE]; be<u16> mEditorIndex; };
struct ProgramDescriptorSlot { gptr<ProgramDescriptor> mDescriptor; u8 mTail[0x2C]; };
struct BindingStateEditor;
struct BinaryShaderHeader { u8 mBeforeCounts[0x22]; be<u16> mProgramCount,mEditorCount; };
struct ShaderArchiveRuntime {
    gptr<BinaryShaderHeader> mBinary; u8 mBeforeProgramCount[0x10];
    be<u16> mProgramCount,mEditorCount; be<u32> mReserved;
    gptr<ProgramDescriptorSlot> mDescriptors; gptr<ShaderRecord> mRecords;
    u8 mBeforeWork[4]; gptr<void> mWork; be<u32> mWorkSize;
    be<u32> mWorkCapacity; gptr<ProgramInstance> mPrograms;
    gptr<BindingStateEditor> mEditors; gptr<void> mTable;
    static ShaderArchiveRuntime* construct(ShaderArchiveRuntime* archive);
    void releaseArrays(); void allocateArrays(void* heap); void select(void* selection);
};
struct BindingStateEditor {
    gptr<ShaderArchiveRuntime> mOwner; gptr<ShaderRecord> mRecord;
    gptr<void> mSelection; SavedBindingWords mSaved; be<u8> mActive; u8 mTail[3];
    static BindingStateEditor* allocate(BindingStateEditor* editor);
    void restore(const SavedBindingWords* words); void restoreSaved();
    void initialize(ShaderArchiveRuntime* owner,u32 index,void* heap);
    void select(void* selection);
};
}
WWHD_SIZE(agl::environment::ShaderVector,12);
WWHD_SIZE(agl::environment::SavedBindingWords,20);
WWHD_OFFSET(agl::environment::ShaderRecord,mBinding,0x14);
WWHD_SIZE(agl::environment::ShaderRecord,0x3C);
WWHD_SIZE(agl::environment::ProgramDescriptorSlot,0x30);
WWHD_OFFSET(agl::environment::ShaderArchiveRuntime,mProgramCount,0x14);
WWHD_OFFSET(agl::environment::ShaderArchiveRuntime,mDescriptors,0x1C);
WWHD_OFFSET(agl::environment::ShaderArchiveRuntime,mRecords,0x20);
WWHD_OFFSET(agl::environment::ShaderArchiveRuntime,mPrograms,0x34);
WWHD_OFFSET(agl::environment::ShaderArchiveRuntime,mEditors,0x38);
WWHD_SIZE(agl::environment::ShaderArchiveRuntime,0x40);
WWHD_OFFSET(agl::environment::BindingStateEditor,mSaved,0xC);
WWHD_OFFSET(agl::environment::BindingStateEditor,mActive,0x20);
WWHD_SIZE(agl::environment::BindingStateEditor,0x24);
