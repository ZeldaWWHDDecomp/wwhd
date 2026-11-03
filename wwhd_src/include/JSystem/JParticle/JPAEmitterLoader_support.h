/** Per-TU Cafe string support for the JPA archive loader. */
#pragma once
#include "wwhd.h"
#include "JSystem/JParticle/JPA_emitter_calc.h"
#include "JSystem/JParticle/JPAResourceManager.h"
namespace jpa_loader {
// HD: this TU emits the buffered SafeString tail in addition to the canonical base.
struct LoaderBufferedString : SafeString {be<u32> mLength;};

struct ArchiveHeader {be<u32> magic,version;be<u16> emitterCount,textureCount;};
struct ArchiveLoader {gptr<void> pHeap;gptr<const ArchiveHeader> pData;gptr<void> mpEmtrRes,mpTexRes,mTextureContext;void load();};
struct LoaderStaticBlock {be<u32> words[4];};
WWHD_OFFSET(LoaderBufferedString,mLength,8);
WWHD_SIZE(LoaderBufferedString,12);
WWHD_SIZE(ArchiveLoader,0x14);
void registerStatic(void*);
}

void JPALoaderDelete(void*);
struct JPAEmitterArchiveLoaderDataBase {
 static void load(const jpa_loader::ArchiveHeader*,void*,gptr<void>*,gptr<void>*,void*);
};
void JPALoaderRegister(void*);
namespace jpa_loader {
struct ParticleHeader {be<u32> magic,type;u8 reserved[4];be<u32> blockNum;u8 flags[4];be<u8> keyNum,fldNum,textureNum,padding;be<u16> resID;};
struct BlockHeader {be<u32> magic,size;u8 reserved[4];};
struct LoaderTextureVtable {u8 prefix[0x14];be<u32> getName;};
WWHD_OFFSET(ParticleHeader,resID,0x18);WWHD_SIZE(BlockHeader,12);
}
void* JPAHeapNew(u32,void*,s32);
void* JPAHeapNewArray(u32,void*,s32);
void* JPALoaderCreateEmitterResource(void*,u32,void*);
void* JPALoaderCreateTextureResource(void*,u32,void*,void*);
void JPALoaderAssert(const char*,u32,const char*);
void* JPALoaderFieldConstruct(void*,const void*);
void* JPALoaderKeyConstruct(void*,const void*);
void* JPALoaderDynamicsConstruct(void*,const void*);
void* JPALoaderBaseShapeConstruct(void*,const void*,void*);
void* JPALoaderExtraShapeConstruct(void*,const void*);
void* JPALoaderSweepShapeConstruct(void*,const void*);
void* JPALoaderExTexShapeConstruct(void*,const void*);
void* JPALoaderTextureConstruct(void*,const void*);
void* JPALoaderDescriptorConstruct(void*);
void JPALoaderRegisterEmitter(void*,JPAEmitterData*,u32);
void JPALoaderRegisterTexture(void*,void*,SafeString*);
