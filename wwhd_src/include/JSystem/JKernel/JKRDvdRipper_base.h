#pragma once
#include "wwhd.h"
struct JKRDvdRipper_baseStaticBlock { be<u32> mWords[4]; };
WWHD_SIZE(JKRDvdRipper_baseStaticBlock,16);
void JKRDvdRipper_baseConstructRegister(void*);

class JSUPtrList;
void JKRDvdRipper_baseInitiateList(JSUPtrList*);


#include "JSystem/JKernel/JKRDvdFile.h"
u32 JKRDvdFileGetSize(void*);
void JKRDvdRipperListDestroy(JSUPtrList*,u32);



JKRDvdFile* JKRDvdRipperFileConstruct(JKRDvdFile*);
void JKRDvdRipperFileDestroy(JKRDvdFile*,u32);
void* JKRDvdRipperLoadFile(JKRDvdFile*,void*,u32,u32,void*,u32,u32,void*,void*);
void JKRDvdRipperCopyName(void*,const void*,u32);
