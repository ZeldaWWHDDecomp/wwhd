#pragma once
#include "wwhd.h"
struct JUTResourceStaticBlock { be<u32> mWords[4]; };
WWHD_SIZE(JUTResourceStaticBlock,16);
void JUTResourceConstructRegister(void*);


#include "JSystem/JSupport/JSUInputStream.h"
struct JUTResReference {
    void* getResource(u32,void*);
    void* getResourceFromStream(JSUInputStream*,u32,void*);
    be<u8> mType;
    be<u8> mNameLength;
    be<u8> mName[256];
};
WWHD_OFFSET(JUTResReference,mType,0);
WWHD_OFFSET(JUTResReference,mNameLength,1);
WWHD_OFFSET(JUTResReference,mName,2);
WWHD_SIZE(JUTResReference,258);
void* JUTArchiveGetResource(u32,const void*,void*);
void* JUTFileGetResource(const void*,void*);
void* JUTFileGetGlobalResource(const void*);
