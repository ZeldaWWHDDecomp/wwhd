// Qualified HD SaveMgr compiler/vtable helpers.
#include "d_menu_save_local.h"
void saveHelperDelete0(u32 p,u32 flags){
 WWHD_FUNC(0x027253A0,void,p,flags);if(p&&(flags&1))call<void>(0x0273AF40,p);
}
VERIFY(0x027253A0,saveHelperDelete0);
void saveHelperEmpty(){
 WWHD_FUNC(0x027253B4,void);
}
VERIFY(0x027253B4,saveHelperEmpty);
void saveHelperTerminate(u32 p){
 WWHD_FUNC(0x027253B8,void,p);store<u8>(U(p)+U(p,8)-1,0);
}
VERIFY(0x027253B8,saveHelperTerminate);
void saveHelperDelete1(u32 p,u32 flags){
 WWHD_FUNC(0x027253D0,void,p,flags);if(p&&(flags&1))call<void>(0x0273AF40,p);
}
VERIFY(0x027253D0,saveHelperDelete1);
void saveHelperDelete2(u32 p,u32 flags){
 WWHD_FUNC(0x027253E4,void,p,flags);if(p&&(flags&1))call<void>(0x0273AF40,p);
}
VERIFY(0x027253E4,saveHelperDelete2);
void saveHelperDelete3(u32 p,u32 flags){
 WWHD_FUNC(0x027253F8,void,p,flags);if(p&&(flags&1))call<void>(0x0273AF40,p);
}
VERIFY(0x027253F8,saveHelperDelete3);
void saveMgrThreadDtor(u32 p,u32 flags){
 WWHD_FUNC(0x0272540C,void,p,flags);if(!p)return;u32 heap=U(p,0x14);store<u32>(p+12,0x10140830u);if(heap)call<void>(0x02740BE0,heap,p);call<void>(0x02752BEC,p,0u);if(flags&1)call<void>(0x0273AF40,p);
}
VERIFY(0x0272540C,saveMgrThreadDtor);
u32 saveMemberDispatch(u32 p){
 WWHD_FUNC(0x02725480,u32,p);u32 obj=U(p,4);if(!obj)return p;s32 index=load<s16>(p+10);if(!index)return p;
 s32 adjust=load<s16>(p+8);u32 self=obj+(u32)adjust,target,offset=cpu->r[8];
 if(index<0)target=U(p,12);else{offset=(u32)(s32)load<s16>(p+14);u32 table=U(self+offset);target=U(table+((u32)index<<3)+4);}
 // The dispatcher establishes r6/r7/r9 and, on virtual dispatch, r8; forward all other live incoming arguments.
 return call_ptr<u32>(target,self,cpu->r[4],cpu->r[5],(u32)adjust,(u32)index,offset,obj,cpu->r[10],cpu->f[1].ps0,cpu->f[2].ps0,cpu->f[3].ps0,cpu->f[4].ps0,cpu->f[5].ps0,cpu->f[6].ps0,cpu->f[7].ps0,cpu->f[8].ps0);
}
VERIFY(0x02725480,saveMemberDispatch);
