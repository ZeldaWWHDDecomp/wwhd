#pragma once
#include "JSystem/J2DGraph/J2DPane.h"
#include "JSystem/JSupport/JSUInputStream.h"
#include "JSystem/JSupport/JSUMemoryStream.h"
#include "JSystem/J2DGraph/J2DOrthoGraph.h"
#include "JSystem/JKernel/JKRArchive.h"
struct J2DScreenHeader { be<u32> magic,type;u8 data[24]; };
struct J2DScreenInfo { be<u32> magic,size;be<u16> width,height;be<u32> color; };
struct J2DScreen : J2DPane {
    be<u8> mbClipToParent;be<u32> mColor;
    s32 makeHierarchyPanes(J2DPane*,JSURandomInputStream*);
    bool set(JSURandomInputStream*);
    bool set(const char*,JKRArchive*);
    void draw(f32,f32,const J2DGrafContext*);
    bool checkSignature(JSURandomInputStream*);
    bool getScreenInformation(JSURandomInputStream*);
    J2DPane* search(u32);
    void drawSelf(f32,f32,const Mtx34*);
    void calcMtx();
    J2DPane* createPane(const J2DScrnBlockHeader&,JSURandomInputStream*,J2DPane*);
};
WWHD_SIZE(J2DScreenHeader,32);WWHD_SIZE(J2DScreenInfo,16);
WWHD_OFFSET(J2DScreen,mColor,0xCD);WWHD_SIZE(J2DScreen,0xD1);
s32 screenRead(JSUInputStream*,void*,s32);
void screenPaneDestroy(J2DPane*,u32);
void screenDelete(void*);
J2DPane* screenPaneSearch(J2DPane*,u32);
void screenAssert(const char*,u32,const char*);
void screenRegisterStatic(void*);

s32 screenPeek(JSURandomInputStream*,void*,s32);
s32 screenSeek(JSURandomInputStream*,s32,u32);
void* screenCopyBytes(void*,const void*,u32);
void screenPaneDraw(J2DPane*,const J2DGrafContext*,u32,f32,f32);
void screenSetScissor(J2DGrafContext*);
J2DOrthoGraph* screenOrthoConstruct(J2DOrthoGraph*,f32,f32,f32,f32,f32,f32);
void screenOrthoSetPort(J2DOrthoGraph*);

const void* screenFindResource(const char*,JKRArchive*);
void screenMemorySet(JSUMemoryInputStream*,const void*,s32);
void screenMemoryDestroy(JSUMemoryInputStream*,u32);
