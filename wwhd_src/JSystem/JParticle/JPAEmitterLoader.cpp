/** GameCube JPA1 archive entry with Cafe texture-context forwarding. */
#include "JSystem/JParticle/JPAEmitterLoader_support.h"
using namespace jpa_loader;
static LoaderStaticBlock sStaticBlock;
static u32 sStaticRegister;
static u32 sNameVtable;
/* 028475D4 */
static u32 getArchiveDefaultValue(){return 0;}
/* 028475DC */
static u32 getArchiveVersionKind(){return 3;}
/* 02847D14 */
void JPAEmitterArchiveLoaderDataBase::load(const ArchiveHeader* data,void* heap,gptr<void>* emitterResource,gptr<void>* textureResource,void* textureContext){
 if(data && (u32)data->magic==0x4A504143 && (u32)data->version==0x312D3030){
  ArchiveLoader loader;
  loader.pData=data;
  loader.mTextureContext=textureContext;
  loader.mpTexRes=nullptr;
  loader.pHeap=heap;
  loader.mpEmtrRes=nullptr;
  loader.load();
  *emitterResource=loader.mpEmtrRes;
  *textureResource=loader.mpTexRes;
 }
}
/* 02847DA8 */
static void __sinit_JPAEmitterLoader_cpp(){
 sStaticBlock.words[2]=0;sStaticBlock.words[0]=0;sStaticBlock.words[3]=0;sStaticBlock.words[1]=0;
 JPALoaderRegister(&sStaticRegister);
}
/* 028475E4 */
void jpa_loader::ArchiveLoader::load(){
 using LinkInfo=jpa_emitter_calc::JPADataBlockLinkInfo;
 const ArchiveHeader* header=pData;
 void* resource=JPAHeapNew(0xC,pHeap.get(),4);
 if(resource) resource=JPALoaderCreateEmitterResource(resource,header->emitterCount,pHeap.get());
 mpEmtrRes=resource;
 resource=JPAHeapNew(0x60,pHeap.get(),4);
 if(resource)resource=JPALoaderCreateTextureResource(resource,header->textureCount,pHeap.get(),mTextureContext.get());
 mpTexRes=resource;
 u32 emitterCount=header->emitterCount,offset=0x20;
 while(emitterCount--){
  const auto* particle=reinterpret_cast<const ParticleHeader*>(reinterpret_cast<const u8*>(pData.get())+offset);
  auto* emitter=reinterpret_cast<JPAEmitterData*>(JPAHeapNew(8,pHeap.get(),4));
  if(emitter)emitter->pLinkInfoArray=nullptr;
  else JPALoaderAssert("JPAEmitterLoader.cpp",0xF5,"pEmtrRes");
  auto* link=reinterpret_cast<LinkInfo*>(JPAHeapNew(0x24,pHeap.get(),4));
  if(link){
   link->fieldNum=0;link->fieldBlocks=nullptr;link->mpSweepShape=nullptr;link->mpDynamics=nullptr;
   link->textureNum=0;link->textureMap=nullptr;link->keyBlocks=nullptr;link->mpExTexShape=nullptr;link->mpBaseShape=nullptr;
   link->keyNum=0;link->mpExtraShape=nullptr;
  }else JPALoaderAssert("JPAEmitterLoader.cpp",0xF9,"pLinkInfo");
  emitter->infoNum=1;
  auto* links=reinterpret_cast<gptr<LinkInfo>*>(JPAHeapNewArray(4,pHeap.get(),4));emitter->pLinkInfoArray=links;
  if(!links){JPALoaderAssert("JPAEmitterLoader.cpp",0xFE,"pEmtrRes->pLinkInfoArray");links=emitter->pLinkInfoArray.get();}
  links[0]=link;
  u32 keyNum=particle->keyNum;link->keyNum=keyNum;
  void* keys=nullptr;if(keyNum)keys=JPAHeapNewArray(keyNum*4,pHeap.get(),4);
  link->keyBlocks=reinterpret_cast<gptr<jpa_emitter_calc::JPAKeyBlock>*>(keys);
  if(!keys && link->keyNum)JPALoaderAssert("JPAEmitterLoader.cpp",0x105,"pLinkInfo->keyBlocks || pLinkInfo->keyNum == 0");
  u32 fieldNum=particle->fldNum;link->fieldNum=fieldNum;
  void* fields=nullptr;if(fieldNum)fields=JPAHeapNewArray(fieldNum*4,pHeap.get(),4);
  link->fieldBlocks=fields;
  if(!fields && link->fieldNum)JPALoaderAssert("JPAEmitterLoader.cpp",0x10B,"pLinkInfo->fldBlocks || pLinkInfo->fldNum == 0");
  u32 textureNum=particle->textureNum;link->textureMap=nullptr;link->textureNum=textureNum;
  u32 blockCount=particle->blockNum;offset+=0x20;u32 fieldIndex=0,keyIndex=0;
  while(blockCount--){
   const auto* block=reinterpret_cast<const BlockHeader*>(reinterpret_cast<const u8*>(pData.get())+offset);
   u32 size=block->size;
   switch((u32)block->magic){
   case 0x464C4431:{
    if(!link->fieldBlocks)JPALoaderAssert("JPAEmitterLoader.cpp",0x11E,"pLinkInfo->fldBlocks != (0)");
    void* object=JPAHeapNew(8,pHeap.get(),4);if(object)object=JPALoaderFieldConstruct(object,reinterpret_cast<const u8*>(pData.get())+offset);
    reinterpret_cast<gptr<void>*>(link->fieldBlocks.get())[fieldIndex]=object;
    if(!reinterpret_cast<gptr<void>*>(link->fieldBlocks.get())[fieldIndex])JPALoaderAssert("JPAEmitterLoader.cpp",0x120,"pLinkInfo->fldBlocks[fld_cntr]");
    ++fieldIndex;break;}
   case 0x4B464131:{
    if(!link->keyBlocks)JPALoaderAssert("JPAEmitterLoader.cpp",0x125,"pLinkInfo->keyBlocks != (0)");
    void* object=JPAHeapNew(0xC,pHeap.get(),4);if(object)object=JPALoaderKeyConstruct(object,reinterpret_cast<const u8*>(pData.get())+offset);
    link->keyBlocks[keyIndex]=reinterpret_cast<jpa_emitter_calc::JPAKeyBlock*>(object);
    if(!link->keyBlocks[keyIndex])JPALoaderAssert("JPAEmitterLoader.cpp",0x127,"pLinkInfo->keyBlocks[key_cntr]");
    ++keyIndex;break;}
   case 0x42454D31:{
    void* object=JPAHeapNew(0x8,pHeap.get(),4);
    if(object)object=JPALoaderDynamicsConstruct(object,reinterpret_cast<const u8*>(pData.get())+offset);
    link->mpDynamics=object;
    if(!object)JPALoaderAssert("JPAEmitterLoader.cpp",0x12C,"pLinkInfo->dynBlock");
    break;}
   case 0x42535031:{
    void* object=JPAHeapNew(0x24,pHeap.get(),4);
    if(object)object=JPALoaderBaseShapeConstruct(object,reinterpret_cast<const u8*>(pData.get())+offset,pHeap.get());
    link->mpBaseShape=object;
    if(!object)JPALoaderAssert("JPAEmitterLoader.cpp",0x130,"pLinkInfo->bspBlock");
    break;}
   case 0x45535031:{
    void* object=JPAHeapNew(0x20,pHeap.get(),4);
    if(object)object=JPALoaderExtraShapeConstruct(object,reinterpret_cast<const u8*>(pData.get())+offset);
    link->mpExtraShape=object;
    if(!object)JPALoaderAssert("JPAEmitterLoader.cpp",0x134,"pLinkInfo->espBlock");
    break;}
   case 0x53535031:{
    void* object=JPAHeapNew(0x10,pHeap.get(),4);
    if(object)object=JPALoaderSweepShapeConstruct(object,reinterpret_cast<const u8*>(pData.get())+offset);
    link->mpSweepShape=object;
    if(!object)JPALoaderAssert("JPAEmitterLoader.cpp",0x138,"pLinkInfo->sspBlock");
    break;}
   case 0x45545831:{
    void* object=JPAHeapNew(0x8,pHeap.get(),4);
    if(object)object=JPALoaderExTexShapeConstruct(object,reinterpret_cast<const u8*>(pData.get())+offset);
    link->mpExTexShape=object;
    if(!object)JPALoaderAssert("JPAEmitterLoader.cpp",0x13C,"pLinkInfo->etxBlock");
    break;}
   case 0x54444231:link->textureMap=reinterpret_cast<be<u16>*>(const_cast<BlockHeader*>(block)+1);break;
   }
   offset+=size;
  }
  JPALoaderRegisterEmitter(mpEmtrRes.get(),emitter,particle->resID);
 }
 u32 textures=header->textureCount;
 SafeString name;
 while(textures--){
  auto* texture=reinterpret_cast<JPATextureArc*>(JPAHeapNew(0x68,pHeap.get(),4));
  if(texture)texture=reinterpret_cast<JPATextureArc*>(JPALoaderTextureConstruct(texture,reinterpret_cast<const u8*>(pData.get())+offset));
  if(!texture)JPALoaderAssert("JPAEmitterLoader.cpp",0x154,"pTex");
  void* descriptor=JPAHeapNew(0x90,pHeap.get(),4);if(descriptor)descriptor=JPALoaderDescriptorConstruct(descriptor);
  name.mTextPtr=gabi::ea(gabi::call_ptr<const char*>(static_cast<const LoaderTextureVtable*>(texture->__vtbl.get())->getName,texture));
  name.__vtbl=gabi::ea(&sNameVtable);
  JPALoaderRegisterTexture(mpTexRes.get(),descriptor,&name);
  offset+=0x20;
 }
}
/* WWHD glue
 ext 0273B050 void* JPAHeapNew(u32,void*,s32)
 ext 0273B0D4 void* JPAHeapNewArray(u32,void*,s32)
 ext 02826E60 void* JPALoaderCreateEmitterResource(void*,u32,void*)
 ext 02826A74 void* JPALoaderCreateTextureResource(void*,u32,void*,void*)
 ext 0273AA24 void JPALoaderAssert(const char*,u32,const char*)
 ext 028482B8 void* JPALoaderFieldConstruct(void*,const void*)
 ext 02848430 void* JPALoaderKeyConstruct(void*,const void*)
 ext 0284B45C void* JPALoaderDynamicsConstruct(void*,const void*)
 ext 0284AE48 void* JPALoaderBaseShapeConstruct(void*,const void*,void*)
 ext 02847F18 void* JPALoaderExtraShapeConstruct(void*,const void*)
 ext 02848610 void* JPALoaderSweepShapeConstruct(void*,const void*)
 ext 02847E3C void* JPALoaderExTexShapeConstruct(void*,const void*)
 ext 02827DF4 void* JPALoaderTextureConstruct(void*,const void*)
 ext 027BE6B8 void* JPALoaderDescriptorConstruct(void*)
 ext 02826F2C void JPALoaderRegisterEmitter(void*,JPAEmitterData*,u32)
 ext 02826B98 void JPALoaderRegisterTexture(void*,void*,SafeString*)
 str 1017AC48
 str 1017AC60
 str 1017AC3C
 str 1017AC6C
 str 1017AC88
 str 1017ACB8
 str 1017ABA0
 str 1017ACE8
 str 1017ABBC
 str 1017AD08
 str 1017ABD8
 str 1017ABEC
 str 1017AC00
 str 1017AC14
 str 1017AC28
 str 1017AB98
 data sNameVtable 1017AB10
 data sStaticBlock 104B6684
 data sStaticRegister 101FC720
 ext 028F026C void JPALoaderRegister(void*)
*/
