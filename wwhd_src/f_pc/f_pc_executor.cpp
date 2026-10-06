#include "bindings.h"

// Opaque process/node storage: offsets below are the HD process ABI.
void* fpcEx_Search(u32 judge,void* data) {
 WWHD_FUNC(0x025DE508,void*,judge,data);
 return gabi::call<void*>(0x025DEE58,judge,data);
}
VERIFY(0x025DE508,fpcEx_Search);

void* fpcEx_SearchByID(u32 id) {
 WWHD_FUNC(0x025DE50C,void*,id);
 gabi::Local<be<u32>> key;*key=id;
 if(id==0xFFFFFFFE || id==0xFFFFFFFF) return nullptr;
 return fpcEx_Search(0x025E1234,key.get());
}
VERIFY(0x025DE50C,fpcEx_SearchByID);

s32 fpcEx_IsExist(u32 id) {
 WWHD_FUNC(0x025DE564,s32,id);
 return fpcEx_SearchByID(id)!=nullptr;
}
VERIFY(0x025DE564,fpcEx_IsExist);

s32 fpcEx_Execute(u8* proc) {
 WWHD_FUNC(0x025DE58C,s32,proc);
 u32 ea=gabi::ea(proc);
 if(gabi::load<u8>(ea+0xC)!=2) return 0;
 if(gabi::call<s32>(0x025E0B20,proc,1)==1) return 0;
 return gabi::call<s32>(0x025DD2B8,proc);
}
VERIFY(0x025DE58C,fpcEx_Execute);

s32 fpcEx_ToLineQ(u8* proc) {
 WWHD_FUNC(0x025DE5F4,s32,proc);
 u32 ea=gabi::ea(proc),layer=gabi::load<u32>(ea+0x2C);
 u32 layerID=gabi::load<u32>(layer+0xC);
 u32 parent=gabi::load<u32>(layer+0x18);
 if(layerID!=0 && gabi::call<s32>(0x0201A71C,gabi::at<u8>(parent+0x34))!=1) return 0;
 u16 list=gabi::load<u16>(ea+0xA4);
 if(gabi::call<s32>(0x025DF6C8,gabi::at<u8>(ea+0x34),list)==0) {
  gabi::call<s32>(0x025DF084,gabi::at<u8>(ea+0x18));return 0;
 }
 u32 subtype=gabi::load<u32>(ea+0xB8);
 gabi::store<u8>(ea+0xC,2);
 u32 nodeType=gabi::load<u32>(0x101F3D60);
 if(gabi::call<s32>(0x025DD258,nodeType,subtype)!=0)
  gabi::call<s32>(0x025DED70,gabi::at<u8>(ea+0xC0),0x025DE5F4,proc);
 return 1;
}
VERIFY(0x025DE5F4,fpcEx_ToLineQ);

s32 fpcEx_ExecuteQTo(u8* proc) {
 WWHD_FUNC(0x025DE6C4,s32,proc);
 u32 ea=gabi::ea(proc);
 if(gabi::call<s32>(0x025DF084,gabi::at<u8>(ea+0x18))!=1) return 0;
 gabi::store<u8>(ea+0xC,3);return 1;
}
VERIFY(0x025DE6C4,fpcEx_ExecuteQTo);

s32 fpcEx_ToExecuteQ(u8* proc) {
 WWHD_FUNC(0x025DE720,s32,proc);
 u32 ea=gabi::ea(proc);
 u16 list=gabi::load<u16>(ea+0xA4);
 u16 priority=gabi::load<u16>(ea+0xA6);
 u32 layer=gabi::load<u32>(ea+0xA0);
 if(gabi::call<s32>(0x025DEF68,gabi::at<u8>(ea+0x18),layer,list,priority)!=1) return 0;
 fpcEx_ToLineQ(proc);return 1;
}
VERIFY(0x025DE720,fpcEx_ToExecuteQ);

void fpcEx_Handler(u32 queue) {
 WWHD_FUNC(0x025DE788,void,queue);
 gabi::call(0x025DF5C0,queue);
}
VERIFY(0x025DE788,fpcEx_Handler);

/* 025DE78C __sinit: the TU's header-statics initializer. The header statics block at
 * 1048A7B8 (zeroed 16-byte object at +0xC, {-pi, pi} from 100582DC, two one-byte objects at +8/+9),
 * each registered for destruction (records 101F3A94, +0xC, +0x18). */
static void __sinit_f_pc_executor_cpp() {
    WWHD_FUNC(0x025DE78C, void);
    const u32 bss = 0x1048A7B8, rec = 0x101F3A94, ro = 0x100582DC;
    gabi::store<u32>(bss + 0x14, 0); gabi::store<u32>(bss + 0xC, 0); gabi::store<u32>(bss + 0x18, 0); gabi::store<u32>(bss + 0x10, 0);
    gabi::call(0x028F026C, rec);
    f32 lo = gabi::load<f32>(ro), hi = gabi::load<f32>(ro + 4);
    gabi::store<f32>(bss, lo);
    gabi::store<f32>(bss + 4, hi);
    gabi::call(0x028ED6F8, bss + 8);
    gabi::call(0x028F026C, rec + 0xC);
    gabi::call(0x028EAB2C, bss + 9);
    gabi::call(0x028F026C, rec + 0x18);
}
VERIFY(0x025DE78C, __sinit_f_pc_executor_cpp);
