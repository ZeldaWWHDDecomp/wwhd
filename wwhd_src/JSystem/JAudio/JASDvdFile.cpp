/* Audio DVD-service ownership. HD file I/O stays behind the assigned adapter boundary. */
#include "JSystem/JAudio/JASDvdFile.h"
#include "JSystem/JAudio/JAIGlobalParameter.h"
using namespace JASystem::Dvd;
namespace JASystem { namespace Dvd {
static TDvdFileService* dvdFileService;
static DvdDisposer* dvdFileDisposer;
}}
static DvdFileVtable serviceVtable;
static DvdDisposerVtable disposerVtable;
static AudioHeaderStatics staticState;
static AudioDestructorRecord staticRecord;
/* 0280ED70 */
static TDvdFileService* constructDvdFileService(TDvdFileService* service) {
    if(!service) service=static_cast<TDvdFileService*>(dvdFileAllocate(sizeof(TDvdFileService)));
    if(service) { dvdFileBaseConstruct(service);service->mVtable=&serviceVtable; }
    return service;
}
namespace JASystem { namespace Dvd {
/* 0280EDC4 */
TDvdFileService* TDvdFileService::create(JKRHeap* heap) {
    if(!dvdFileService) {
        TDvdFileService* service=static_cast<TDvdFileService*>(dvdFileHeapAllocate(sizeof(TDvdFileService),heap,4));
        DvdDisposer* disposer=&service->mDisposer;
        if(gabi::ea(disposer)!=0) { dvdFileConstructDisposer(disposer,heap,3);disposer->mVtable=&disposerVtable; }
        dvdFileDisposer=disposer;
        dvdFileService=service?constructDvdFileService(service):nullptr;
    }
    return dvdFileService;
}
/* 0280EEB8 */
u32 TDvdFile::loadFile(const char* path,void* buffer) {
    if(!dvdFileOpen(this,path)) return 0;
    u32 size=dvdFileSize(this);
    if(!dvdFileRead(this,buffer,size)) return 0;
    dvdFileClose(this);
    return 1;
}
/* 0280EF40 */
u32 TDvdFile::getFileSize(const char* path) {
    if(!dvdFileOpen(this,path)) return 0;
    u32 size=dvdFileSize(this);
    dvdFileClose(this);return size;
}
}}
/* 0280EE64 */
static void destroyDvdFileService(TDvdFileService* service,u32 flags) {
    if(service) { dvdFileBaseDestroy(service,0);if(flags&1) dvdFileDelete(service); }
}
/* 0280EFB0 */
static void destroyDvdFileDisposer(DvdDisposer* disposer,u32 flags) {
    if(!disposer) return;
    disposer->mVtable=&disposerVtable;
    if(disposer==static_cast<DvdDisposer*>(dvdFileDisposer)) {
        TDvdFileService* service=dvdFileService;
        dvdFileDisposer=nullptr;
        DvdFileVtable* table=static_cast<DvdFileVtable*>(static_cast<void*>(service->mVtable));
        gabi::call_ptr<void>(table->mDestroy,service,2);
        dvdFileService=nullptr;
    }
    dvdFileDestroyDisposer(disposer,0);
    if(flags&1) dvdFileDelete(disposer);
}
/* 0280F058 */
static void __sinit_JASDvdFile_cpp() {
    staticState.mWords[2]=0;staticState.mWords[0]=0;staticState.mWords[3]=0;staticState.mWords[1]=0;
    dvdFileRegister(&staticRecord);
}
/* WWHD glue
data dvdFileService 101F9D58
data dvdFileDisposer 101F9D5C
data serviceVtable 1016FD7C
data disposerVtable 1016FD6C
data staticState 104B51D4
data staticRecord 101F9D4C
ext 0273AD10 void* dvdFileAllocate(u32)
ext 0273B0D4 void* dvdFileHeapAllocate(u32,JKRHeap*,s32)
ext 0280F4A0 void dvdFileBaseConstruct(JASystem::Dvd::TDvdFile*)
ext 0280F504 void dvdFileBaseDestroy(JASystem::Dvd::TDvdFile*,u32)
ext 02752B0C void dvdFileConstructDisposer(void*,JKRHeap*,u32)
ext 02752BEC void dvdFileDestroyDisposer(void*,u32)
ext 0273AF40 void dvdFileDelete(void*)
ext 0280F600 u32 dvdFileOpen(JASystem::Dvd::TDvdFile*,const char*)
ext 0280FB30 u32 dvdFileSize(JASystem::Dvd::TDvdFile*)
ext 0280FA68 u32 dvdFileRead(JASystem::Dvd::TDvdFile*,void*,u32)
ext 0280F9E8 void dvdFileClose(JASystem::Dvd::TDvdFile*)
ext 028F026C void dvdFileRegister(void*)
*/
