#include "gabi.h"
namespace {
using namespace gabi;
int cTrIt_Method(void* tree,void* callback,void* data) {
    WWHD_FUNC(0x0201ABF0,int,tree,callback,data);
    s32 count=load<s32>(ea(tree)+4);
    u32 list=load<u32>(ea(tree)); int result=1;
    for(s32 remaining=count;remaining>0;--remaining) {
        int sub=call<int>(0x020100A0,at<void>(list),callback,data);
        list+=12u;
        if(sub==0) result=0;
    }
    return result;
}
VERIFY(0x0201ABF0,cTrIt_Method);
void* cTrIt_Judge(void* tree,void* callback,void* data) {
    WWHD_FUNC(0x0201AC60,void*,tree,callback,data);
    s32 count=load<s32>(ea(tree)+4);
    u32 list=load<u32>(ea(tree));
    for(s32 remaining=count;remaining>0;--remaining) {
        void* result=call<void*>(0x020100BC,at<void>(list),callback,data);
        list+=12u;
        if(result) return result;
    }
    return nullptr;
}
VERIFY(0x0201AC60,cTrIt_Judge);
void cTrIt_StaticInit() {
    WWHD_FUNC(0x0201ACE4,void);
    store<u32>(0x101FFB84,0);store<u32>(0x101FFB7C,0);
    store<u32>(0x101FFB88,0);store<u32>(0x101FFB80,0);
    call<void>(0x028F026C,at<void>(0x1018D43C));
    f32 lower=load<f32>(0x100036E0),upper=load<f32>(0x100036E4);
    store<f32>(0x101FFB70,lower);store<f32>(0x101FFB74,upper);
    call<void>(0x028ED6F8,at<void>(0x101FFB78));
    call<void>(0x028F026C,at<void>(0x1018D448));
    call<void>(0x028EAB2C,at<void>(0x101FFB79));
    call<void>(0x028F026C,at<void>(0x1018D454));
}
VERIFY(0x0201ACE4,cTrIt_StaticInit);
}
