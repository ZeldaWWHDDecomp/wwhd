#include "gabi.h"
namespace {
using namespace gabi;
void cMl_Init(void* heap) {
    WWHD_FUNC(0x02019424,void,heap);
    store<u32>(0x1018CA3C,ea(heap));
}
VERIFY(0x02019424,cMl_Init);
void* cMl_MemalignB(s32 alignment,u32 size) {
    WWHD_FUNC(0x02019430,void*,alignment,size);
    if(size==0) return nullptr;
    u32 heap=load<u32>(0x1018CA3C);
    u32 table=load<u32>(heap+0xC);
    return call_ptr<void*>(load<u32>(table+0x34),at<void>(heap),size,alignment);
}
VERIFY(0x02019430,cMl_MemalignB);
void cMl_Free(void* memory) {
    WWHD_FUNC(0x0201945C,void,memory);
    if(!memory) return;
    u32 heap=load<u32>(0x1018CA3C);
    u32 table=load<u32>(heap+0xC);
    call_ptr<void>(load<u32>(table+0x3C),at<void>(heap),memory);
}
VERIFY(0x0201945C,cMl_Free);
void cMl_StaticInit() {
    WWHD_FUNC(0x0201947C,void);
    store<u32>(0x101FF9CC,0);store<u32>(0x101FF9C4,0);
    store<u32>(0x101FF9D0,0);store<u32>(0x101FF9C8,0);
    call<void>(0x028F026C,at<void>(0x1018CA18));
    f32 lower=load<f32>(0x100035B0),upper=load<f32>(0x100035B4);
    store<f32>(0x101FF9B8,lower);store<f32>(0x101FF9BC,upper);
    call<void>(0x028ED6F8,at<void>(0x101FF9C0));
    call<void>(0x028F026C,at<void>(0x1018CA24));
    call<void>(0x028EAB2C,at<void>(0x101FF9C1));
    call<void>(0x028F026C,at<void>(0x1018CA30));
}
VERIFY(0x0201947C,cMl_StaticInit);
}
