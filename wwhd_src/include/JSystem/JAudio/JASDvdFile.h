/* Cafe DVD-file prefix and the audio singleton's heap-disposal wrapper. */
#pragma once
#include "JSystem/JAudio/JASWorkPool.h"
#include "JSystem/JAudio/JASDvdThread_extra.h"
namespace JASystem { namespace Dvd {
using DvdDisposer=Kernel::WorkPoolDisposer;
using DvdDisposerVtable=Kernel::WorkPoolDisposerVtable;
struct DvdFileVtable { be<u32> mPrefix[3];be<u32> mDestroy; };
// HD: path-based service adapters add no storage to the canonical native file facade.
class TDvdFile:public ::JASDvdFile {
public:
    u32 loadFile(const char*,void*);
    u32 getFileSize(const char*);
};
class TDvdFileService:public TDvdFile {
public:
    static TDvdFileService* create(JKRHeap*);
    DvdDisposer mDisposer;
};
extern gptr<TDvdFileService>& dvdFileService;
extern gptr<DvdDisposer>& dvdFileDisposer;
WWHD_SIZE(TDvdFile,0xB4);
WWHD_OFFSET(TDvdFileService,mDisposer,0xB4);
WWHD_SIZE(TDvdFileService,0xC4);
}}
void* dvdFileAllocate(u32);
void* dvdFileHeapAllocate(u32,JKRHeap*,s32);
void dvdFileBaseConstruct(JASystem::Dvd::TDvdFile*);
void dvdFileBaseDestroy(JASystem::Dvd::TDvdFile*,u32);
void dvdFileConstructDisposer(void*,JKRHeap*,u32);
void dvdFileDestroyDisposer(void*,u32);
void dvdFileDelete(void*);
u32 dvdFileOpen(JASystem::Dvd::TDvdFile*,const char*);
u32 dvdFileSize(JASystem::Dvd::TDvdFile*);
u32 dvdFileRead(JASystem::Dvd::TDvdFile*,void*,u32);
void dvdFileClose(JASystem::Dvd::TDvdFile*);
void dvdFileRegister(void*);
