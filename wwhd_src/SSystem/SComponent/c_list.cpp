#include "gabi.h"
using gabi::load; using gabi::store;
void cLs_Init(void* list) {
 WWHD_FUNC(0x0200FDE0,void,list); u32 a=gabi::ea(list);
 store<u32>(a,0); store<u32>(a+4,0); store<u32>(a+8,0);
}
VERIFY(0x0200FDE0,cLs_Init);
s32 cLs_SingleCut(void* node) {
 WWHD_FUNC(0x0200FDF4,s32,node); u32 a=gabi::ea(node),list=load<u32>(a+4);
 u32 head=load<u32>(list),tail=load<u32>(list+4);
 if(a==head) store<u32>(list,load<u32>(a+8));
 if(a==tail) store<u32>(list+4,load<u32>(a));
 gabi::call<void>(0x02019B48,node); gabi::call<void>(0x02019C8C,node);
 u32 size=load<u32>(list+8)-1u; store<u32>(list+8,size);
 return s32(size)>0;
}
VERIFY(0x0200FDF4,cLs_SingleCut);
s32 cLs_Addition(void* list,void* node) {
 WWHD_FUNC(0x0200FE78,s32,list,node); u32 a=gabi::ea(list),tail=load<u32>(a+4);
 if(!tail) store<u32>(a,gabi::ea(node));
 else gabi::call<void>(0x02019B94,gabi::at<void>(tail),node);
 void* last=gabi::call<void*>(0x02019AD8,node); store<u32>(a+4,gabi::ea(last));
 gabi::call<void>(0x02019C6C,node,list);
 void* head=gabi::at<void>(load<u32>(a)); s32 size=gabi::call<s32>(0x02019A90,head);
 store<s32>(a+8,size); return size;
}
VERIFY(0x0200FE78,cLs_Addition);
s32 cLs_Insert(void* list,s32 index,void* node) {
 WWHD_FUNC(0x0200FF10,s32,list,index,node); u32 a=gabi::ea(list);
 void* head=gabi::at<void>(load<u32>(a)); void* existing=gabi::call<void*>(0x02019AFC,head,index);
 if(!gabi::ea(existing)) return cLs_Addition(list,node);
 gabi::call<void>(0x02019C6C,node,list); gabi::call<void>(0x02019C0C,existing,node);
 head=gabi::call<void*>(0x02019AB4,node); store<u32>(a,gabi::ea(head));
 s32 size=gabi::call<s32>(0x02019A90,head); store<s32>(a+8,size); return size;
}
VERIFY(0x0200FF10,cLs_Insert);
void* cLs_GetFirst(void* list) {
 WWHD_FUNC(0x0200FFB0,void*,list); u32 a=gabi::ea(list);
 if(!load<u32>(a+8)) return nullptr;
 void* head=gabi::at<void>(load<u32>(a)); cLs_SingleCut(head); return head;
}
VERIFY(0x0200FFB0,cLs_GetFirst);
void cLs_Create(void* list) { WWHD_FUNC(0x02010008,void,list); cLs_Init(list); }
VERIFY(0x02010008,cLs_Create);

// Per-translation-unit HD math/header globals and their registration records.
void cLs_StaticInit() {
 WWHD_FUNC(0x0201000C,void);
 store<u32>(0x101FF7E4,0);
 store<u32>(0x101FF7DC,0);
 store<u32>(0x101FF7E8,0);
 store<u32>(0x101FF7E0,0);
 gabi::call<void>(0x028F026C,gabi::at<void>(0x1018C7B4));
 f32 first=load<f32>(0x10001ED8);
 f32 second=load<f32>(0x10001EDC);
 store<f32>(0x101FF7D0,first);
 store<f32>(0x101FF7D4,second);
 gabi::call<void>(0x028ED6F8,gabi::at<void>(0x101FF7D8));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x1018C7C0));
 gabi::call<void>(0x028EAB2C,gabi::at<void>(0x101FF7D9));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x1018C7CC));
}
VERIFY(0x0201000C,cLs_StaticInit);
