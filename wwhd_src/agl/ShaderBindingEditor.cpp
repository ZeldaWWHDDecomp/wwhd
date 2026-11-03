// Shader binding snapshots, restored in the binary's individual load/store order.
#include "agl/ShaderBindingEditor.h"
using namespace agl::environment;
void* allocateBindingEditor(u32 size);
void releaseBindingEditor(void* editor);
f32 shaderVectorMagnitude(const ShaderVector* input);
void bindSelectedProgram(ProgramInstance* instance,void* name,ProgramDescriptor* descriptor,void* selection,const be<u8>* tag);
template <class T> static T* bindingEditorPointer(u32 address) {
    return gabi::at<T>(address);
}
template <class T> static T* bindingEditorElement(T* base,u32 index,u32 stride) {
    return bindingEditorPointer<T>(gabi::ea(base)+index*stride);
}
static u8 sSelectionTag;
/* 02772C74 */
static f32 normalizeShaderVector(ShaderVector* output,const ShaderVector* input) {
    auto frame=gabi::AutoFrame(gabi::current_function());
    f32 magnitude=shaderVectorMagnitude(input);
    if(magnitude>0.0f) {
        f32 inverse=gabi::fdivs_ppc(1.0f,magnitude);
        output->x=gabi::fmuls_ppc(input->x,inverse);
        output->y=gabi::fmuls_ppc(input->y,inverse);
        output->z=gabi::fmuls_ppc(input->z,inverse);
    }
    return magnitude;
}
/* 02772CF0 */
BindingStateEditor* BindingStateEditor::allocate(BindingStateEditor* editor) {
    auto frame=gabi::AutoFrame(gabi::current_function());
    if(editor==nullptr) editor=static_cast<BindingStateEditor*>(allocateBindingEditor(sizeof(BindingStateEditor)));
    return editor;
}
/* 02772D1C */
void BindingStateEditor::restore(const SavedBindingWords* words) {
    ShaderRecord* record=mRecord;
    record->mBinding.mWords[0]=words->mWords[0];
    record->mBinding.mWords[1]=words->mWords[1];
    record->mBinding.mWords[2]=words->mWords[2];
    record->mBinding.mWords[3]=words->mWords[3];
    record->mBinding.mWords[4]=words->mWords[4];
}
/* 02772D4C */
void BindingStateEditor::restoreSaved() {
    auto frame=gabi::AutoFrame(gabi::current_function());
    restore(&mSaved);
}
/* 02772D54 */
static void destroyBindingEditor(BindingStateEditor* editor,u32 flags) {
    auto frame=gabi::AutoFrame(gabi::current_function());
    if(editor!=nullptr) {
        editor->restoreSaved();
        if(flags&1) releaseBindingEditor(editor);
    }
}
/* 02772D8C */
void BindingStateEditor::initialize(ShaderArchiveRuntime* owner,u32 index,void*) {
    mOwner=owner;
    ShaderRecord* record=bindingEditorElement(owner->mRecords.get(),index,0x3C);
    mRecord=record;
    mSaved.mWords[0]=record->mBinding.mWords[0];
    mSaved.mWords[1]=record->mBinding.mWords[1];
    mSaved.mWords[2]=record->mBinding.mWords[2];
    mSaved.mWords[3]=record->mBinding.mWords[3];
    u32 last=record->mBinding.mWords[4];
    mSelection=nullptr;
    mSaved.mWords[4]=last;
    mActive=1;
}
/* 02772DDC */
void BindingStateEditor::select(void* selection) {
    auto frame=gabi::AutoFrame(gabi::current_function());
    ShaderArchiveRuntime* owner=mOwner;
    mSelection=selection;
    for(s32 index=0;index<(u16)owner->mProgramCount;++index) {
        ProgramDescriptor* descriptor=owner->mDescriptors[index].mDescriptor;
        u32 editorAddress=gabi::ea(owner->mEditors.get())+(u16)descriptor->mEditorIndex*0x24;
        if(editorAddress==gabi::ea(this)) {
            auto* slot=bindingEditorElement(owner->mPrograms.get(),u32(index),0x1D8);
            descriptor=owner->mDescriptors[index].mDescriptor;
            void* name=mRecord->mName;
            bindSelectedProgram(slot,name,descriptor,selection,&sSelectionTag);
            owner=mOwner;
        }
    }
}

using ArrayConstructor=void* (*)(void*);
using ArrayDestructor=void (*)(void*,u32);
void destroyShaderArray(void* array,s32 count,u32 stride,ArrayDestructor destructor,u32 destroyStorage,u32 flags);
void* constructShaderArray(void* storage,u32 count,u32 stride,ArrayConstructor constructor,u32 flags);
void* allocateShaderArrays(u32 size,void* heap,s32 alignment);
void* constructProgramArrayElement(void* storage);
void* constructEditorArrayElement(void* storage);
void destroyProgramArrayElement(void* storage,u32 flags);
void destroyEditorArrayElement(void* storage,u32 flags);
void initializeProgramArrayElement(ProgramInstance* instance,void* heap);
static u32 sShaderArchiveTable;
static u32 sArrayHeaderSize;
/* 02772E8C */
ShaderArchiveRuntime* ShaderArchiveRuntime::construct(ShaderArchiveRuntime* archive) {
    auto frame=gabi::AutoFrame(gabi::current_function());
    if(archive==nullptr) archive=static_cast<ShaderArchiveRuntime*>(allocateBindingEditor(sizeof(ShaderArchiveRuntime)));
    if(archive!=nullptr) {
        archive->mEditors=nullptr;
        archive->mTable=&sShaderArchiveTable;
        archive->mWorkCapacity=0;
        archive->mPrograms=nullptr;
        archive->mWorkSize=0;
        archive->mWork=nullptr;
        archive->mBinary=nullptr;
    }
    return archive;
}
/* 02772EE8 */
void ShaderArchiveRuntime::releaseArrays() {
    auto frame=gabi::AutoFrame(gabi::current_function());
    ProgramInstance* programs=mPrograms;
    if(programs!=nullptr) {
        destroyShaderArray(programs,-1,0x1D8,destroyProgramArrayElement,1,0);
        mPrograms=nullptr;
    }
    BindingStateEditor* editors=mEditors;
    if(editors!=nullptr) {
        destroyShaderArray(editors,-1,0x24,destroyEditorArrayElement,1,0);
        mEditors=nullptr;
    }
}
/* 02772F74 */
static void destroyShaderArchive(ShaderArchiveRuntime* archive,u32 flags) {
    auto frame=gabi::AutoFrame(gabi::current_function());
    if(archive!=nullptr) {
        archive->mTable=&sShaderArchiveTable;
        archive->releaseArrays();
        if(flags&1) releaseBindingEditor(archive);
    }
}
/* 02772FD0 */
void ShaderArchiveRuntime::allocateArrays(void* heap) {
    auto frame=gabi::AutoFrame(gabi::current_function());
    BinaryShaderHeader* binary=mBinary;
    u32 programCount=binary->mProgramCount;
    u32 request=programCount*0x1D8+sArrayHeaderSize;
    u32 editorCount=binary->mEditorCount;
    void* allocation=allocateShaderArrays(request,heap,4);
    if(allocation!=nullptr) {
        void* storage=bindingEditorPointer<void>(gabi::ea(allocation)+sArrayHeaderSize);
        mPrograms=static_cast<ProgramInstance*>(constructShaderArray(storage,programCount,0x1D8,constructProgramArrayElement,0));
    } else mPrograms=nullptr;
    request=editorCount*0x24+sArrayHeaderSize;
    allocation=allocateShaderArrays(request,heap,4);
    BindingStateEditor* editors=nullptr;
    if(allocation!=nullptr) {
        void* storage=bindingEditorPointer<void>(gabi::ea(allocation)+sArrayHeaderSize);
        editors=static_cast<BindingStateEditor*>(constructShaderArray(storage,editorCount,0x24,constructEditorArrayElement,0));
    }
    mEditors=editors;
    for(u32 index=0;index<editorCount;++index) {
        BindingStateEditor* element=bindingEditorElement(editors,index,0x24);
        element->initialize(this,index,heap);
        if(index+1<editorCount) editors=mEditors;
    }
    for(u32 index=0;index<programCount;++index) {
        ProgramInstance* program=bindingEditorElement(mPrograms.get(),index,0x1D8);
        initializeProgramArrayElement(program,heap);
    }
}
/* 02773134 */
void ShaderArchiveRuntime::select(void* selection) {
    auto frame=gabi::AutoFrame(gabi::current_function());
    for(u32 index=0;index<(u16)mEditorCount;++index) {
        BindingStateEditor* editor=bindingEditorElement(mEditors.get(),index,0x24);
        editor->select(selection);
    }
}
/* 027731B0 */
static void destroyArchiveCallbackStorage(void* storage,u32 flags) {
    auto frame=gabi::AutoFrame(gabi::current_function());
    if(storage!=nullptr && (flags&1)) releaseBindingEditor(storage);
}
/* 027731C4 */
static void emptyArchiveCallback() {}
/* WWHD glue
ext 0273AD10 void* allocateBindingEditor(u32)
ext 0273AF40 void releaseBindingEditor(void*)
ext 028E8E10 f32 shaderVectorMagnitude(const agl::environment::ShaderVector*)
ext 027746E4 void bindSelectedProgram(agl::environment::ProgramInstance*,void*,agl::environment::ProgramDescriptor*,void*,const be<u8>*)
data sSelectionTag 101616B8

ext 028F0164 void destroyShaderArray(void*,s32,u32,ArrayDestructor,u32,u32)
ext 028EFF8C void* constructShaderArray(void*,u32,u32,ArrayConstructor,u32)
ext 0273B0D4 void* allocateShaderArrays(u32,void*,s32)
ext 02774320 void* constructProgramArrayElement(void*) callback
ext 02772CF0 void* constructEditorArrayElement(void*) callback
ext 027743DC void destroyProgramArrayElement(void*,u32) callback
ext 02772D54 void destroyEditorArrayElement(void*,u32) callback
ext 0277445C void initializeProgramArrayElement(agl::environment::ProgramInstance*,void*)
data sShaderArchiveTable 101616BC
data sArrayHeaderSize 101FCD08
*/
