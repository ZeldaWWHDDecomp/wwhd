#include "gabi.h"
using gabi::load; using gabi::store;
bool mDoMch_Create() {
 WWHD_FUNC(0x025F12F8,bool);
 auto settingAssert=[](u32 line) { gabi::call<void>(0x0273AA24,gabi::at<void>(0x10058FC4),line,gabi::at<void>(0x10058FDC)); };
 u8 initialized=load<u8>(0x101F9B18);
 if(initialized) {settingAssert(0x33);initialized=load<u8>(0x101F9B18);}
 store<u32>(0x101F9B04,1);
 if(initialized) {settingAssert(0x36);initialized=load<u8>(0x101F9B18);}
 store<u32>(0x101F9B08,0x02A22000);
 if(initialized) {settingAssert(0x39);initialized=load<u8>(0x101F9B18);}
 store<u32>(0x101F9B0C,0xA0000);
 if(initialized) {settingAssert(0x3E);initialized=load<u8>(0x101F9B18);}
 store<u32>(0x101F9B10,0xA00000);
 if(initialized) settingAssert(0x41);
 store<u32>(0x101F9B14,0x5CE000);
 gabi::call<void>(0x0280136C);
 u32 saved=1, requested=load<u32>(0x101F9AF8);
 if(requested) {void* heap=gabi::at<void>(load<u32>(0x101F8B4C));saved=gabi::call<u32>(0x02756170,heap,requested);}
 if(!load<s8>(0x101F48C0)) {u32 db=load<u32>(0x101F97D4);store<u8>(db+0xC,0);}
 void* framework=gabi::call<void*>(0x027EC230);u32 root=load<u32>(gabi::ea(framework)+0x24);
 framework=gabi::call<void*>(0x027EC230);u32 root2=load<u32>(gabi::ea(framework)+0x24);
 auto failAssert=[](u32 line,u32 message) {gabi::call<void>(0x0273AA24,gabi::at<void>(0x10058FF0),line,gabi::at<void>(message));};
 if(!gabi::ea(gabi::call<void*>(0x025E30DC,u32(0x10000),gabi::at<void>(root))))failAssert(0xB2,0x10058FD0);
 if(!gabi::ea(gabi::call<void*>(0x025E31B4,u32(0x90000),gabi::at<void>(root2))))failAssert(0xB7,0x10058FD0);
 if(!gabi::ea(gabi::call<void*>(0x025E2E94,u32(0),u32(0xA00000),gabi::at<void>(root))))failAssert(0xC4,0x10058FD0);
 if(!gabi::ea(gabi::call<void*>(0x025E2E94,u32(1),u32(0xA00000),gabi::at<void>(root))))failAssert(0xC8,0x10058FD0);
 void* system=gabi::call<void*>(0x027EC42C);
 u32 table=load<u32>(gabi::ea(system)+0xC),target=load<u32>(table+0x74);
 u32 size=gabi::call_ptr<u32>(target,system)-0x10000;
 if(s32(size)<=0)failAssert(0xD4,0x10059004);
 void* zelda=gabi::call<void*>(0x025E3004,size,system);
 if(!gabi::ea(zelda))failAssert(0xD6,0x10059010);
 gabi::call<void>(0x027EC220,zelda);
 gabi::call<void>(0x025EE048);
 gabi::call<void>(0x02019424);
 gabi::call<void>(0x0201976C,u32(100),u32(100),u32(100));
 u32 priority=load<u32>(0x101F8B9C);gabi::call<void>(0x025E28B0,priority-2);
 if(saved!=1){void* heap=gabi::at<void>(load<u32>(0x101F8B4C));gabi::call<void>(0x02756170,heap,saved);}
 return true;
}
VERIFY(0x025F12F8,mDoMch_Create);
void mDoMch_StaticInit() {
 WWHD_FUNC(0x025F15C0,void);
 store<u32>(0x1048D068,0);store<u32>(0x1048D060,0);
 store<u32>(0x1048D06C,0);store<u32>(0x1048D064,0);
 gabi::call<void>(0x028F026C,gabi::at<void>(0x101F4874));
 f32 first=load<f32>(0x1005902C),second=load<f32>(0x10059030);
 store<f32>(0x1048D054,first);store<f32>(0x1048D058,second);
 gabi::call<void>(0x028ED6F8,gabi::at<void>(0x1048D05C));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x101F4880));
 gabi::call<void>(0x028EAB2C,gabi::at<void>(0x1048D05D));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x101F488C));
}
VERIFY(0x025F15C0,mDoMch_StaticInit);

/* 025F1654 periodic heap check called by the m_Do_main frame loop every N frames (probably mDoMch_HeapCheckAll; empty in the retail HD build): empty function */
static void mDoMch_HeapCheckAll_hd() {
    WWHD_FUNC(0x025F1654, void);
}
VERIFY(0x025F1654, mDoMch_HeapCheckAll_hd);
