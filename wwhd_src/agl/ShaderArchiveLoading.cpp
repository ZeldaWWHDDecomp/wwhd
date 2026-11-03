/* Archive loading reconstructed from assigned WWHD operations; runtime validation pending. */
// References: WWHD's own instructions and strings only.
#include "agl/ShaderArchive.h"
#include "agl/GraphicsAccess.h"
#include "agl/BitmapTransfer.h"
#include "sead/Thread.h"
using namespace agl;
struct ArchiveLoadHandle_l {gptr<void> data;};
struct ArchiveLoadList_l {be<u32> bytes,count;};
struct ArchiveLoadRecord_l {be<u32> bytes,sectionOffset,stageIndices[3];};
struct ArchiveBinaryProgramRecord_l {be<u32> bytes,sectionOffset,flags,firstShader;};
struct ArchiveBinaryShaderRecord_l {be<u32> bytes,kind,packed,descriptorBytes;};
struct ArchiveLoadWalk_l {be<u32> index;gptr<void> record;};
struct ArchiveLoadStage_l {gptr<void> code,variations;be<u16> metrics[4];gptr<void> methods;};
struct SourceArchiveFrame_l {
 gptr<ArchiveLoadHandle_l> stackStage2;gptr<sead::RegistryHeap> stackHeap;
 ArchiveLoadHandle_l program;be<u32> selectorIndex;gptr<void> selectorRecord;sead::SafeText name;gptr<void> variationMirror;
 ArchiveLoadHandle_l variantList,firstResource,unusedResource,stage3,stage0,stage1,stage2;
 ArchiveLoadHandle_l variation,programSet,reusedProgramSet;
 ArchiveLoadHandle_l variantListMirror,stage3Mirror,stage0Mirror,stage1Mirror,stage2Mirror;
 be<u32> selectorCount;u8 pad68[4];ArchiveLoadWalk_l variations,programs;
 be<u32> flagWalkIndex;u8 pad80[4];be<u32> initialVariationCount;u8 pad88[4];
 be<u32> variationCount;u8 pad90[4];be<u32> programCount;u8 pad98[8];
 sead::TickTime tick;be<u8> included[0x400];
};
struct ArchiveLoadMetrics_l {be<u16> first,second;be<u8> sdkFirst,sdkSecond,third,fourth;};
struct BinaryArchiveFrame_l {
 gptr<ArchiveLoadHandle_l> stackStage2;gptr<sead::RegistryHeap> stackHeap;
 be<u32> programCount;gptr<sead::RegistryHeap> heap;gptr<sead::SafeTextVtable> nameMethods;ArchiveLoadHandle_l source;
 ArchiveLoadMetrics_l metrics;
 gptr<void> currentShader;be<u32> shaderWalkIndex;gptr<void> firstShader;
 ArchiveLoadHandle_l program;gptr<ArchiveLoadList_l> shaderList;be<u32> shaderIndex;u8 pad40[4];
 sead::SafeText name;ArchiveLoadHandle_l variantList,firstResource,unusedResource,stage3,stage0,stage1,stage2;
 ArchiveLoadHandle_l stage3Mirror,stage0Mirror,stage1Mirror,stage2Mirror,sourceArgument;
 ArchiveLoadWalk_l shaders,programs;be<u32> shaderCount;u8 pad90[4];be<u32> sourceProgramCount;
 u8 pad98[4];gptr<ShaderArchiveProgramView_l> programBegin;u8 padA0[4];gptr<ShaderArchiveProgramView_l> programEnd;u8 padA8[8];
};
WWHD_SIZE(ArchiveLoadHandle_l,4);
WWHD_SIZE(ArchiveLoadStage_l,0x14);
WWHD_OFFSET(ArchiveLoadStage_l,metrics,8);
WWHD_SIZE(SourceArchiveFrame_l,0x4A0);
WWHD_OFFSET(SourceArchiveFrame_l,name,0x14);
WWHD_OFFSET(SourceArchiveFrame_l,tick,0x98);
WWHD_OFFSET(SourceArchiveFrame_l,included,0xA0);
WWHD_SIZE(BinaryArchiveFrame_l,0xA8);
WWHD_OFFSET(BinaryArchiveFrame_l,metrics,0x18);
WWHD_OFFSET(BinaryArchiveFrame_l,program,0x2C);
WWHD_OFFSET(BinaryArchiveFrame_l,name,0x3C);
WWHD_OFFSET(BinaryArchiveFrame_l,sourceArgument,0x70);
static sead::SafeTextVtable sArchiveLoadNameMethods;
static bool prepareSourceArchive(ArchiveLoadHandle_l*);
static bool prepareBinaryArchive(ArchiveLoadHandle_l*,u32);
static void constructArchiveProgram(ShaderArchiveProgramView_l*);
static void* sourceAfterThree(ArchiveLoadHandle_l*);
static void* sourceStageBlock(ArchiveLoadHandle_l*,u32);
static void* binaryStageBlock(ArchiveLoadHandle_l*,u32);
static void configureLoadedProgram(ShaderArchiveProgramView_l*,sead::SafeText*,ArchiveLoadHandle_l*,ArchiveLoadHandle_l*,ArchiveLoadHandle_l*,ArchiveLoadHandle_l*,ArchiveLoadHandle_l*,ArchiveLoadHandle_l*,ArchiveLoadHandle_l*,sead::RegistryHeap*);
static u32 queryArchiveGpuAlignment();
static void* allocateArchiveGpuStorage(u32,sead::RegistryHeap*,u32);
static void flushArchiveGpuStorage(void*,u32);
static ArchiveLoadStage_l* archiveProgramStage(ShaderArchiveProgramView_l*,u32);
static void installArchiveShader(ArchiveLoadStage_l*,void*);
static u32 vertexArchiveMetric0(void*);static u32 vertexArchiveMetric1(void*);
static u32 pixelArchiveMetric0(void*);static u32 pixelArchiveMetric1(void*);
static u32 geometryArchiveMetric0(void*);static u32 geometryArchiveMetric1(void*);
static void assignArchiveProgramStorage(void*,void*,u32);
static void markArchiveProgramDirty(ShaderArchiveProgramView_l*);
static void loadArchiveSource(ShaderArchive_l*,const ArchiveLoadHandle_l*,sead::RegistryHeap*);
template<class T> static T* addArchiveLoad(void* p,u32 offset);
static ArchiveLoadList_l* nextArchiveLoadList(ArchiveLoadList_l* p);
static ArchiveLoadRecord_l* nextArchiveLoadRecord(ArchiveLoadRecord_l* p);
static void* allocateArchiveLoad(sead::RegistryHeap* requested,u32 bytes);
static ArchiveLoadList_l* archiveSourceFirst(ShaderArchive_l* archive);
static void* archiveBinaryImage(ShaderArchive_l* archive);
static ArchiveLoadList_l* archiveBinaryFirst(ShaderArchive_l* archive);
template<class T> static T* archiveLoadAt(const gptr<T>& array,u32 count,u32 index);
static void initializeArchiveLoadWalk(ArchiveLoadWalk_l& walk,void* record);
static void constructArchiveProgramArray(void* storage,u32 count);
template<class T> static void constructArchiveLoadArray(void* storage,u32 count);
struct ArchiveSharedFirstResource_l {u8 prefix[0x2C];gptr<void> firstResource;};
/* 027B82B8 */
static void loadArchiveSource(ShaderArchive_l* archive,const ArchiveLoadHandle_l* input,sead::RegistryHeap* heap) {
 gabi::AutoFrame activation{gabi::current_function()};
 gabi::FrameLocal<SourceArchiveFrame_l> f(8);
 void* image=input->data;archive->releaseTransient();
 if (!image) return;
 BitmapHeapSource_l* root=sBitmapHeapSource_l.get();
 if (!root->mCompileHeap.get()) return;
 archive->mResource=image;
 prepareSourceArchive(reinterpret_cast<ArchiveLoadHandle_l*>(&archive->mResource));
 ArchiveLoadList_l* first=archiveSourceFirst(archive);
 bool newPrograms=false;
 if (!static_cast<u32>(archive->mBinaryImageAddress)) {
  u32 count=first->count;
  if (static_cast<s32>(count)>0) {
   void* programs=allocateArchiveLoad(heap,count*0x84u);constructArchiveProgramArray(programs,count);
   if (programs) {archive->mPrograms=static_cast<ShaderArchiveProgramView_l*>(programs);archive->mProgramCount=count;}
   first=archiveSourceFirst(archive);
  }
  newPrograms=true;
 }
 u32 count=nextArchiveLoadList(first)->count;
 if (static_cast<s32>(count)>0) {
  void* variations=allocateArchiveLoad(heap,count*0x34u);constructArchiveLoadArray<ShaderVariation_l>(variations,count);
  if (variations) {archive->mVariations=static_cast<ShaderVariation_l*>(variations);archive->mVariationCount=count;}
 }
 count=archive->mVariationCount;
 if (static_cast<s32>(count)>0) {
  void* strings=allocateArchiveLoad(heap,count*4);
  if (strings) {archive->mStrings=static_cast<gptr<be<u8>>*>(strings);archive->mStringCount=count;}
 }
 count=archive->mVariationCount;
 if (static_cast<s32>(count)>0) {
  void* binaries=allocateArchiveLoad(heap,count*4);
  if (binaries) {archive->mBinaries=static_cast<gptr<be<u8>>*>(binaries);archive->mBinaryCount=count;}
 }
 count=archive->mProgramCount;
 if (static_cast<s32>(count)>0) {
  void* sets=allocateArchiveLoad(heap,count*0xF0u);constructArchiveLoadArray<ShaderArchiveProgramSet_l>(sets,count);
  if (sets) {archive->mProgramSets=static_cast<ShaderArchiveProgramSet_l*>(sets);archive->mProgramSetCount=count;}
 }
 first=archiveSourceFirst(archive);
 ArchiveLoadList_l* second=nextArchiveLoadList(first);
 auto* variation=addArchiveLoad<ArchiveLoadRecord_l>(second,8);
 auto* program=addArchiveLoad<ArchiveLoadRecord_l>(first,8);
 u32 variationCount=second->count;
 f->flagWalkIndex=0;f->initialVariationCount=variationCount;
 for (u32 v=0;v<variationCount;++v) {
  f->selectorIndex=0;f->selectorRecord=program;f->included[v]=0;
  u32 selectors=first->count;f->selectorCount=selectors;
  ArchiveLoadRecord_l* candidate=program;
  for (u32 i=0;i<selectors;++i) {
   bool found=false;
   for (u32 stage=0;stage<3;++stage) {
    s32 selected=static_cast<s16>(candidate->stageIndices[stage]);
    if (static_cast<u32>(selected)==v) {found=true;break;}
   }
   if (found) {f->included[v]=1;break;}
   if (static_cast<u8>(f->included[v])) break;
   candidate=nextArchiveLoadRecord(candidate);
  }
 }
 variationCount=second->count;
 initializeArchiveLoadWalk(f->variations,variation);f->variationCount=variationCount;
 for (u32 v=0;v<variationCount;++v) {
  ShaderVariation_l* target=archiveLoadAt(archive->mVariations,archive->mVariationCount,v);
  f->variationMirror=variation;f->variation.data=variation;
  target->initialize(archive,v,reinterpret_cast<const ShaderVariationSourceHandle_l*>(&f->variation),f->included[v],heap);
  variation=nextArchiveLoadRecord(variation);
 }
 sead::TickTime::construct(&f->tick);
 initializeArchiveLoadWalk(f->programs,program);
 u32 programs=first->count;f->programCount=programs;
 for (u32 p=0;p<programs;++p) {
  ShaderArchiveProgramView_l* target=archiveLoadAt(archive->mPrograms,archive->mProgramCount,p);
  f->program.data=program;
  if (newPrograms) {
   f->name.mVtable=&sArchiveLoadNameMethods;
   f->name.mText=addArchiveLoad<be<u8>>(program,0x14);
   f->variantList.data=sourceAfterThree(&f->program);f->variantListMirror=f->variantList;
   void* firstResourceBase=sourceAfterThree(&f->program);
   auto* firstResourceSpan=static_cast<ArchiveLoadList_l*>(sourceAfterThree(&f->program));
   f->firstResource.data=addArchiveLoad<void>(firstResourceBase,static_cast<u32>(firstResourceSpan->bytes));
   void* unusedBase=sourceAfterThree(&f->program);
   auto* unusedSpan=static_cast<ArchiveLoadList_l*>(sourceAfterThree(&f->program));
   void* unusedNext=addArchiveLoad<void>(unusedBase,static_cast<u32>(unusedSpan->bytes));
   void* secondBase=sourceAfterThree(&f->program);
   auto* secondSpan=static_cast<ArchiveLoadList_l*>(sourceAfterThree(&f->program));
   auto* secondList=addArchiveLoad<ArchiveLoadList_l>(secondBase,static_cast<u32>(secondSpan->bytes));
   f->unusedResource.data=addArchiveLoad<void>(unusedNext,static_cast<u32>(secondList->bytes));
   f->stage3.data=sourceStageBlock(&f->program,3);f->stage3Mirror=f->stage3;
   f->stage0.data=sourceStageBlock(&f->program,0);f->stage0Mirror=f->stage0;
   f->stage1.data=sourceStageBlock(&f->program,1);f->stage1Mirror=f->stage1;
   f->stage2.data=sourceStageBlock(&f->program,2);f->stage2Mirror=f->stage2;
   f->stackHeap=heap;f->stackStage2=&f->stage2;
   configureLoadedProgram(target,&f->name,&f->variantList,&f->firstResource,&f->unusedResource,&f->stage3,&f->stage0,&f->stage1,&f->stage2,heap);
   auto* set=archiveLoadAt(archive->mProgramSets,archive->mProgramSetCount,p);
   f->programSet.data=f->program.data;
   set->initialize(archive,p,reinterpret_cast<const ShaderProgramSourceHandle_l*>(&f->programSet),heap);
  } else {
   void* firstResourceBase=sourceAfterThree(&f->program);
   auto* firstResourceSpan=static_cast<ArchiveLoadList_l*>(sourceAfterThree(&f->program));
   auto* shared=reinterpret_cast<ArchiveSharedFirstResource_l*>(target->mBinary.get());
   shared->firstResource=addArchiveLoad<void>(firstResourceBase,static_cast<u32>(firstResourceSpan->bytes));
   auto* set=archiveLoadAt(archive->mProgramSets,archive->mProgramSetCount,p);
   f->reusedProgramSet.data=f->program.data;
   set->initialize(archive,p,reinterpret_cast<const ShaderProgramSourceHandle_l*>(&f->reusedProgramSet),heap);
  }
  program=nextArchiveLoadRecord(program);
 }
 archive->updateVariations();
}

// Unit-local helpers are included in the canonical mutation ranges.
template<class T> static T* addArchiveLoad(void* p,u32 offset) {return gabi::at<T>(gabi::ea(p)+offset);}

static ArchiveLoadList_l* nextArchiveLoadList(ArchiveLoadList_l* p) {return addArchiveLoad<ArchiveLoadList_l>(p,static_cast<u32>(p->bytes));}

static ArchiveLoadRecord_l* nextArchiveLoadRecord(ArchiveLoadRecord_l* p) {return addArchiveLoad<ArchiveLoadRecord_l>(p,static_cast<u32>(p->bytes));}

static void* allocateArchiveLoad(sead::RegistryHeap* requested,u32 bytes) {
 sead::RegistryHeap* heap=requested?requested:registryCurrentHeap(sRegistryManager.get());
 return gabi::call_ptr<void*>(heap->dispatch()->mAllocate.mFunction,heap,bytes,4u);
}

static ArchiveLoadList_l* archiveSourceFirst(ShaderArchive_l* archive) {
 void* image=archive->mResource;
 u32 offset=*addArchiveLoad<const be<u32>>(image,0x10);
 return addArchiveLoad<ArchiveLoadList_l>(image,offset+0x14);
}

static void* archiveBinaryImage(ShaderArchive_l* archive) {
 // Word0x14 is the binary image address, stored by the binary loader.
 return gabi::at<void>(static_cast<u32>(archive->mBinaryImageAddress));
}

static ArchiveLoadList_l* archiveBinaryFirst(ShaderArchive_l* archive) {
 void* image=archiveBinaryImage(archive);
 u32 offset=*addArchiveLoad<const be<u32>>(image,0x14);
 return addArchiveLoad<ArchiveLoadList_l>(image,offset+0x18);
}

template<class T> static T* archiveLoadAt(const gptr<T>& array,u32 count,u32 index) {
 u32 base=gabi::ea(array.get());return gabi::at<T>(base+(index<count?index*sizeof(T):0));
}

static void initializeArchiveLoadWalk(ArchiveLoadWalk_l& walk,void* record) {walk.index=0;walk.record=record;}

static void constructArchiveProgramArray(void* storage,u32 count) {
 u32 base=gabi::ea(storage),offset=0;
 for (u32 remaining=count;remaining;--remaining,offset+=0x84) if (base+offset) constructArchiveProgram(gabi::at<ShaderArchiveProgramView_l>(base+offset));
}

template<class T> static void constructArchiveLoadArray(void* storage,u32 count) {
 u32 base=gabi::ea(storage),offset=0;
 for (u32 remaining=count;remaining;--remaining,offset+=sizeof(T)) if (base+offset) gabi::at<T>(base+offset)->construct();
}

/* 027B8904 */
static void loadArchiveBinary(ShaderArchive_l* archive,const ArchiveLoadHandle_l* binary,const ArchiveLoadHandle_l* source,u32 flags,sead::RegistryHeap* heap) {
 gabi::AutoFrame activation{gabi::current_function()};
 gabi::FrameLocal<BinaryArchiveFrame_l> f(8);
 void* image=binary->data;
 f->heap=heap;f->source.data=source->data;
 archive->mBinaryImageAddress=gabi::ea(image);
 if (flags&2) archive->mFlags=static_cast<u16>(archive->mFlags)|1;
 u32 bufferAddress=0,usedBytes=0;
 bool copyPrograms=(flags&1)==0;
 if (image) {
  prepareBinaryArchive(reinterpret_cast<ArchiveLoadHandle_l*>(&archive->mBinaryImageAddress),1);
  ArchiveLoadList_l* descriptors=archiveBinaryFirst(archive);
  u32 count=nextArchiveLoadList(descriptors)->count;
  if (static_cast<s32>(count)>0) {
   void* programs=allocateArchiveLoad(heap,count*0x84u);
   constructArchiveProgramArray(programs,count);
   if (programs) {archive->mPrograms=static_cast<ShaderArchiveProgramView_l*>(programs);archive->mProgramCount=count;}
   descriptors=archiveBinaryFirst(archive);
  }
  if (copyPrograms) {
   u32 bytes=0,descriptorCount=descriptors->count;
   auto* descriptor=addArchiveLoad<ArchiveBinaryShaderRecord_l>(descriptors,8);
   initializeArchiveLoadWalk(f->shaders,descriptor);f->shaderCount=descriptorCount;
   for (u32 i=0;i<descriptorCount;++i) {
    bytes+=0x100;
    if (static_cast<u32>(descriptor->kind)==2) bytes+=0x80;
    descriptor=addArchiveLoad<ArchiveBinaryShaderRecord_l>(descriptor,static_cast<u32>(descriptor->bytes));
   }
   u32 alignment=queryArchiveGpuAlignment();
   void* storage=allocateArchiveGpuStorage(bytes,heap,alignment);
   archive->mFilename=storage;
   flushArchiveGpuStorage(storage,bytes);
   descriptors=archiveBinaryFirst(archive);
   bufferAddress=gabi::ea(archive->mFilename.get());
  }
  ArchiveLoadList_l* programList=nextArchiveLoadList(descriptors);
  auto* record=addArchiveLoad<ArchiveBinaryProgramRecord_l>(programList,8);
  u32 programCount=programList->count;
  initializeArchiveLoadWalk(f->programs,record);f->sourceProgramCount=programCount;
  if (programCount) f->nameMethods=&sArchiveLoadNameMethods;
  f->programCount=programCount;
  for (u32 p=0;p<programCount;++p) {
   f->program.data=record;
   auto* primary=archiveLoadAt(archive->mPrograms,archive->mProgramCount,p);
   f->name.mText=addArchiveLoad<be<u8>>(record,0x10);f->name.mVtable=f->nameMethods;
   f->unusedResource.data=nullptr;f->firstResource.data=nullptr;
   f->variantList.data=addArchiveLoad<void>(record,static_cast<u32>(record->sectionOffset)+0x10);
   f->stage3.data=binaryStageBlock(&f->program,3);f->stage3Mirror=f->stage3;
   f->stage0.data=binaryStageBlock(&f->program,0);f->stage0Mirror=f->stage0;
   f->stage1.data=binaryStageBlock(&f->program,1);f->stage1Mirror=f->stage1;
   f->stage2.data=binaryStageBlock(&f->program,2);f->stage2Mirror=f->stage2;
   f->stackHeap=f->heap;f->stackStage2=&f->stage2;
   configureLoadedProgram(primary,&f->name,&f->variantList,&f->firstResource,&f->unusedResource,&f->stage3,&f->stage0,&f->stage1,&f->stage2,f->heap);
   u32 ordinal=0,variantIndex=0xFFFFFFFFu,variantOffset=0u-0x84u;
   ShaderArchiveBinary_l* shared=primary->mBinary;
   while (static_cast<s32>(ordinal)<static_cast<s32>(static_cast<u32>(shared->mVariantCount)+1)) {
    ShaderArchiveProgramView_l* program;
    if (!ordinal) program=shared->mPrimary;
    else {
     u32 base=gabi::ea(shared->mVariants.get());
     program=gabi::at<ShaderArchiveProgramView_l>(base+(variantIndex<static_cast<u32>(shared->mVariantCount)?variantOffset:0));
    }
    u32 programBytes=0;
    for (u32 stage=0;stage<3;++stage) {
     ArchiveLoadStage_l* target=archiveProgramStage(program,stage);
     auto* current=static_cast<ArchiveBinaryProgramRecord_l*>(f->program.data.get());
     u32 stageFlags=current->flags;
     if (!(stageFlags&(1u<<stage))) continue;
     u32 stride=(stageFlags&4)?3:2;
     u32 shaderIndex=static_cast<u32>(current->firstShader)+stage+stride*ordinal;
     if (shaderIndex==0xFFFFFFFFu) continue;
     ArchiveLoadList_l* shaders=archiveBinaryFirst(archive);
     f->shaderWalkIndex=0;f->shaderList=shaders;f->shaderIndex=shaderIndex;
     auto* shader=addArchiveLoad<ArchiveBinaryShaderRecord_l>(shaders,8);
     f->firstShader=shader;
     for (u32 i=0;i!=shaderIndex;++i) {
      f->currentShader=shader;
      shader=addArchiveLoad<ArchiveBinaryShaderRecord_l>(shader,static_cast<u32>(shader->bytes));
     }
     void* nativeShader=addArchiveLoad<void>(shader,0x10);
     flushArchiveGpuStorage(nativeShader,shader->descriptorBytes);
     installArchiveShader(target,nativeShader);
     u32 packed=shader->packed;f->metrics.first=packed>>20;
     packed=shader->packed;f->metrics.second=(packed>>12)&255;
     packed=shader->packed;f->metrics.third=(packed>>6)&63;
     packed=shader->packed;f->metrics.fourth=packed&63;
     if (stage==0) {
      f->metrics.sdkFirst=vertexArchiveMetric0(target->code);
      f->metrics.sdkSecond=vertexArchiveMetric1(target->code);
     } else if (stage==1) {
      f->metrics.sdkFirst=pixelArchiveMetric0(target->code);
      f->metrics.sdkSecond=pixelArchiveMetric1(target->code);
     } else {
      f->metrics.sdkFirst=geometryArchiveMetric0(target->code);
      f->metrics.sdkSecond=geometryArchiveMetric1(target->code);
     }
     target->metrics[0]=static_cast<u16>(f->metrics.first);
     target->metrics[1]=static_cast<u16>(f->metrics.second);
     target->metrics[2]=(static_cast<u32>(f->metrics.sdkFirst)<<8)|static_cast<u32>(f->metrics.sdkSecond);
     target->metrics[3]=(static_cast<u32>(f->metrics.third)<<8)|static_cast<u32>(f->metrics.fourth);
     programBytes+=0x100;if (stage==2) programBytes+=0x80;
    }
    if (copyPrograms) {
     assignArchiveProgramStorage(addArchiveLoad<void>(program,4),gabi::at<void>(bufferAddress+usedBytes),programBytes);
     usedBytes+=programBytes;
    }
    shared=primary->mBinary;
    variantOffset+=0x84;++ordinal;++variantIndex;
   }
   record=addArchiveLoad<ArchiveBinaryProgramRecord_l>(record,static_cast<u32>(record->bytes));
  }
 }
 f->sourceArgument.data=f->source.data;
 loadArchiveSource(archive,&f->sourceArgument,f->heap);
 u32 cursor=gabi::ea(archive->mPrograms.get()),end=cursor+static_cast<u32>(archive->mProgramCount)*0x84u;
 f->programBegin=gabi::at<ShaderArchiveProgramView_l>(cursor);f->programEnd=gabi::at<ShaderArchiveProgramView_l>(end);
 while (cursor!=end) {markArchiveProgramDirty(gabi::at<ShaderArchiveProgramView_l>(cursor));cursor+=0x84;}
}
/* WWHD glue
ext 02756140 sead::RegistryHeap* registryCurrentHeap(void*)
data sArchiveLoadNameMethods 101699E0
ext 027BC854 bool prepareSourceArchive(ArchiveLoadHandle_l*)
ext 027BCDA0 bool prepareBinaryArchive(ArchiveLoadHandle_l*,u32)
ext 027B9F78 void constructArchiveProgram(ShaderArchiveProgramView_l*)
ext 027BC68C void* sourceAfterThree(ArchiveLoadHandle_l*)
ext 027BC6B8 void* sourceStageBlock(ArchiveLoadHandle_l*,u32)
ext 027BC73C void* binaryStageBlock(ArchiveLoadHandle_l*,u32)
ext 027B78D4 void configureLoadedProgram(ShaderArchiveProgramView_l*,sead::SafeText*,ArchiveLoadHandle_l*,ArchiveLoadHandle_l*,ArchiveLoadHandle_l*,ArchiveLoadHandle_l*,ArchiveLoadHandle_l*,ArchiveLoadHandle_l*,ArchiveLoadHandle_l*,sead::RegistryHeap*)
ext 027C07FC u32 queryArchiveGpuAlignment()
ext 0273B0D4 void* allocateArchiveGpuStorage(u32,sead::RegistryHeap*,u32)
ext C0008898 void flushArchiveGpuStorage(void*,u32)
ext 027BAC58 ArchiveLoadStage_l* archiveProgramStage(ShaderArchiveProgramView_l*,u32)
ext 027D4E84 void installArchiveShader(ArchiveLoadStage_l*,void*)
ext C0006390 u32 vertexArchiveMetric0(void*)
ext C0006398 u32 vertexArchiveMetric1(void*)
ext C00062D0 u32 pixelArchiveMetric0(void*)
ext C00062D8 u32 pixelArchiveMetric1(void*)
ext C0006268 u32 geometryArchiveMetric0(void*)
ext C0006270 u32 geometryArchiveMetric1(void*)
ext 027C05A0 void assignArchiveProgramStorage(void*,void*,u32)
ext 027BBCA0 void markArchiveProgramDirty(ShaderArchiveProgramView_l*)
*/
