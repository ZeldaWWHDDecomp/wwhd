// Common JStage adaptor data and enable forwarding.
#include "JSystem/JStudio/JStudio_JStage/object.h"
using namespace JStudio_JStage;
namespace {
struct StaticBlock { be<u32> words[4]; };
static StaticBlock sStaticBlock;
static u32 sStaticRegister;
}
void JStudioStageObject_register(void*);
/* 0284219C */
void TAdaptor_object_::adaptor_data_(jst_adaptors::StageObject* object,const void* id,u32 idSize,const void* data,u32 size) {
    auto* methods=reinterpret_cast<ObjectVtable*>(object->mVtable.get());
    u32 function=methods->setData.get();
    u32 index=0xFFFFFFFF;
    if (idSize) index=*static_cast<const be<u32>*>(id);
    gabi::tail_ptr<void>(function,object,index,data,size);
}
/* 028421C8 */
void TAdaptor_object_::adaptor_ENABLE_(jst_adaptors::StageObject* object,u32 operation,const be<u32>* data,u32) {
    if (operation!=2) return;
    u32 enable=*data;
    auto* methods=reinterpret_cast<ObjectVtable*>(object->mVtable.get());
    u32 get=methods->getFlags.get(),set=methods->setFlags.get();
    u32 flags=gabi::call_ptr<u32>(get,object);
    flags=enable?flags|2u:flags&~2u;
    gabi::call_ptr<void>(set,object,flags);
}
/* 02842260 */
static void __sinit_JStudioStageObject_cpp() {
    sStaticBlock.words[2]=0; sStaticBlock.words[0]=0; sStaticBlock.words[3]=0; sStaticBlock.words[1]=0;
    JStudioStageObject_register(&sStaticRegister);
}
/* WWHD glue:
data sStaticBlock 104B6424
data sStaticRegister 101FAED0
ext 028F026C void JStudioStageObject_register(void*)
*/
