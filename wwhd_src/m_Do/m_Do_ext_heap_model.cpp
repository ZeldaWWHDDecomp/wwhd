#include "gabi.h"
using namespace gabi;

u32 ext_isCurrentSolidHeap() {
 WWHD_FUNC(0x025E2AF8,u32);
 u32 heap=call<u32>(0x027EC230);
 u32 type=call<u32>(0x027EC23C,heap);
 if(type==0x534C4944) return 1;
 call(0x025F2710,at<void>(0x100586E4)); return 0;
}
VERIFY(0x025E2AF8,ext_isCurrentSolidHeap);
void ext_copyMatrix(void* dst,void* src) {
 WWHD_FUNC(0x025E2B54,void,dst,src);
 f32 v[12]; for(u32 i=0;i<12;i++) v[i]=load<f32>(ea(src)+i*4);
 for(u32 i=0;i<12;i++) store<f32>(ea(dst)+i*4,v[i]);
}
VERIFY(0x025E2B54,ext_copyMatrix);
void ext_2DA8(void* self) {
 WWHD_FUNC(0x025E2DA8,void,self);
 call(0x027F4FE4,self); call(0x025E2BF4,self,at<void>(0));
}
VERIFY(0x025E2DA8,ext_2DA8);
void ext_2DE0(void* self, void* data) {
 WWHD_FUNC(0x025E2DE0,void,self,data);
 call(0x027F4FE4,self); call(0x025E2BF4,self,data);
}
VERIFY(0x025E2DE0,ext_2DE0);
void ext_2E24(void* self) {
 WWHD_FUNC(0x025E2E24,void,self);
 call(0x027F4F1C,self); call(0x025E2BF4,self,at<void>(0));
}
VERIFY(0x025E2E24,ext_2E24);
void ext_2E5C(void* self) {
 WWHD_FUNC(0x025E2E5C,void,self);
 call(0x027F4F1C,self); call(0x025E2BF4,self,at<void>(0));
}
VERIFY(0x025E2E5C,ext_2E5C);
u32 ext_getGameHeap(s32 index) {
 WWHD_FUNC(0x025E2F64,u32,index);
 if(index>=2) call(0x0273AA24,at<void>(0x10058800),0xB0E,at<void>(0x10058810));
 return load<u32>(0x1048CF28+(u32(index)<<2));
}
VERIFY(0x025E2F64,ext_getGameHeap);
u32 ext_getGameHeap0() { WWHD_FUNC(0x025E2FBC,u32); return call<u32>(0x025E2F64,0); }
VERIFY(0x025E2FBC,ext_getGameHeap0);
void ext_setGameHeap(s32 ignored) {
 WWHD_FUNC(0x025E2FC4,void,ignored);
 u32 heap=call<u32>(0x025E2FBC); u32 prev=call<u32>(0x027EC1FC,heap); store<u32>(0x1048CF30,prev);
}
VERIFY(0x025E2FC4,ext_setGameHeap);
void ext_setGameHeap0() { WWHD_FUNC(0x025E2FF0,void); call(0x025E2FC4,0); }
VERIFY(0x025E2FF0,ext_setGameHeap0);
u32 ext_getPreviousHeap() { WWHD_FUNC(0x025E2FF8,u32); return load<u32>(0x1048CF30); }
VERIFY(0x025E2FF8,ext_getPreviousHeap);
void ext_drawModel(void* model,void* supplied) {
 WWHD_FUNC(0x025E2BF4,void,model,supplied);
 Local<u8[48]> savedView; Local<u8[64]> savedProj;
 call(0x025E2B54,savedView.get(),at<void>(0x104B45F8));
 for(u32 i=0;i<16;i++) store<u32>(ea(savedProj.get())+i*4,load<u32>(0x104B470C+i*4));
 u32 data=ea(supplied);
 if(!data) { u32 manager=load<u32>(0x101F95D0); u32 count=load<u32>(manager+0x1020); u32 list=load<u32>(manager+0x1024); if(count>1) list+=4; data=load<u32>(list); }
 u32 flags=load<u32>(data+0x50), view;
 if((flags&0x40)&&(flags&0x80)) view=data+0x84;
 else {view=load<u32>(data+0x48);if(!view)view=0x104A2098;}
 for(u32 i=0;i<12;i++) store<u32>(0x104B45F8+i*4,load<u32>(view+i*4));
 flags=load<u32>(data+0x50);u32 projection;
 if(flags&0x1000) projection=load<u32>(data+0x164);
 else {projection=load<u32>(data+0x4C);if(!projection)projection=0x104A20FC;}
 u32 transformed=call<u32>(0x0274D83C,at<void>(projection));
 call(0x028E8970,at<void>(transformed),at<void>(0x104B470C));
 call(0x027F55FC,model);
 for(u32 i=0;i<12;i++)store<u32>(0x104B45F8+i*4,load<u32>(ea(savedView.get())+i*4));
 call(0x028E8970,savedProj.get(),at<void>(0x104B470C));
}
VERIFY(0x025E2BF4,ext_drawModel);
u32 ext_createGameHeap(s32 index,u32 size,void* parent) {
 WWHD_FUNC(0x025E2E94,u32,index,size,parent);
 if(index>=2)call(0x0273AA24,at<void>(0x1005879C),0xAE2,at<void>(0x100587AC));
 u32 offset=u32(index)<<2,slot=0x1048CF28+offset;
 if(load<u32>(slot)&&size)call(0x0273AA24,at<void>(0x1005879C),0xAE4,at<void>(0x100587C4));
 u32 heap=call<u32>(0x027EBCB0,size,parent,1);store<u32>(slot,heap);
 if(heap){store<u32>(heap+0x94,0);u32 obj=load<u32>(slot),v=load<u32>(0x101F475C+offset);store<u32>(obj+0x10,v);obj=load<u32>(slot);v=load<u32>(obj+0x90);store<u32>(obj+0x90,v|1);heap=load<u32>(slot);}
 return heap;
}
VERIFY(0x025E2E94,ext_createGameHeap);
u32 ext_3004(u32 size,void* parent) {
 WWHD_FUNC(0x025E3004,u32,size,parent);
 if(load<u32>(0x1048CF34)&&size)call(0x0273AA24,at<void>(0x10058858),0xB2E,at<void>(0x10058828));
 u32 heap=call<u32>(0x027EBCB0,size,parent,1);store<u32>(0x1048CF34,heap);
 if(heap){store<u32>(heap+0x10,0x1005884C);heap=load<u32>(0x1048CF34);u32 flags=load<u32>(heap+0x90);store<u32>(heap+0x90,flags|1);heap=load<u32>(0x1048CF34);}
 return heap;
}
VERIFY(0x025E3004,ext_3004);
u32 ext_30DC(u32 size,void* parent) {
 WWHD_FUNC(0x025E30DC,u32,size,parent);
 if(load<u32>(0x1048CF3C)&&size)call(0x0273AA24,at<void>(0x10058898),0xB6F,at<void>(0x10058868));
 u32 heap=call<u32>(0x027EBCB0,size,parent,1);store<u32>(0x1048CF3C,heap);
 if(heap){store<u32>(heap+0x10,0x1005888C);heap=load<u32>(0x1048CF3C);u32 flags=load<u32>(heap+0x90);store<u32>(heap+0x90,flags|1);heap=load<u32>(0x1048CF3C);}
 return heap;
}
VERIFY(0x025E30DC,ext_30DC);
u32 ext_31B4(u32 size,void* parent) {
 WWHD_FUNC(0x025E31B4,u32,size,parent);
 if(load<u32>(0x1048CF44)&&size)call(0x0273AA24,at<void>(0x100588D8),0xBB3,at<void>(0x100588A8));
 u32 heap=call<u32>(0x027EBCB0,size,parent,1);store<u32>(0x1048CF44,heap);
 if(heap){store<u32>(heap+0x94,0);heap=load<u32>(0x1048CF44);store<u32>(heap+0x10,0x100588CC);heap=load<u32>(0x1048CF44);u32 flags=load<u32>(heap+0x90);store<u32>(heap+0x90,flags|1);heap=load<u32>(0x1048CF44);}
 return heap;
}
VERIFY(0x025E31B4,ext_31B4);
void ext_30AC() {WWHD_FUNC(0x025E30AC,void);u32 heap=load<u32>(0x1048CF34);u32 prev=call<u32>(0x027EC1FC,heap);store<u32>(0x1048CF38,prev);}
VERIFY(0x025E30AC,ext_30AC);
void ext_3184() {WWHD_FUNC(0x025E3184,void);u32 heap=load<u32>(0x1048CF3C);u32 prev=call<u32>(0x027EC1FC,heap);store<u32>(0x1048CF40,prev);}
VERIFY(0x025E3184,ext_3184);
u32 ext_getCommandHeap() { WWHD_FUNC(0x025E3268,u32);return load<u32>(0x1048CF44); }
VERIFY(0x025E3268,ext_getCommandHeap);
void ext_setCommandHeap() { WWHD_FUNC(0x025E3274,void);u32 heap=call<u32>(0x025E3268);u32 old=call<u32>(0x027EC1FC,heap);store<u32>(0x1048CF48,old); }
VERIFY(0x025E3274,ext_setCommandHeap);
u32 ext_solidFromGame(u32 size,u32 alignment) { WWHD_FUNC(0x025E3528,u32,size,alignment);u32 parent=call<u32>(0x025E2FBC);return call<u32>(0x025E33A8,size,parent,alignment); }
VERIFY(0x025E3528,ext_solidFromGame);
u32 ext_setCurrentHeap(void* heap) { WWHD_FUNC(0x025E3570,u32,heap);if(!ea(heap))call(0x0273AA24,at<void>(0x1005892C),0xDCC,at<void>(0x10058920));return call<u32>(0x027EC220,heap); }
VERIFY(0x025E3570,ext_setCurrentHeap);
u32 ext_solidToCurrent(u32 size,void* parent,u32 align) {
 WWHD_FUNC(0x025E35BC,u32,size,parent,align);
 u32 heap=call<u32>(0x025E33A8,size,parent,align);
 if(heap){if(load<u32>(0x1048CED8))call(0x0273AA24,at<void>(0x1005895C),0xD34,at<void>(0x1005893C));u32 prev=call<u32>(0x027EC230);store<u32>(0x1048CED8,prev);call(0x025E3570,at<void>(heap));}
 return heap;
}
VERIFY(0x025E35BC,ext_solidToCurrent);
u32 ext_3630(u32 size,u32 alignment) { WWHD_FUNC(0x025E3630,u32,size,alignment);u32 heap=call<u32>(0x025E2FBC);return call<u32>(0x025E35BC,size,heap,alignment); }
VERIFY(0x025E3630,ext_3630);
u32 ext_restoreCurrentHeap(){
 WWHD_FUNC(0x025E37D8,u32);u32 heap=load<u32>(0x1048CED8);
 if(!heap){call(0x0273AA24,at<void>(0x1005898C),0xE07,at<void>(0x1005896C));heap=load<u32>(0x1048CED8);}
 u32 result=call<u32>(0x027EC220,at<void>(heap));store<u32>(0x1048CED8,0);return result;
}
VERIFY(0x025E37D8,ext_restoreCurrentHeap);
u32 ext_3834(void* heap) { WWHD_FUNC(0x025E3834,u32,heap);u32 result=call<u32>(0x025E3678,heap);call(0x025E37D8);return result; }
VERIFY(0x025E3834,ext_3834);
void ext_3898() { WWHD_FUNC(0x025E3898,void);call(0x028F0164,at<void>(0x1048CEE8),2,8,at<void>(0x025EE034),0,0); }
VERIFY(0x025E3898,ext_3898);
void ext_38BC() { WWHD_FUNC(0x025E38BC,void);call(0x028F0164,at<void>(0x1048CEF8),2,8,at<void>(0x025EE034),0,0); }
VERIFY(0x025E38BC,ext_38BC);
static u32 ext_vtarget(u32 obj,u32 slot) {return load<u32>(load<u32>(obj+0xC)+slot);}
void ext_alignHeap(void* heap,u32 alignment) {
 WWHD_FUNC(0x025E32A0,void,heap,alignment);
 u32 obj=ea(heap);if(!obj||alignment<=4)return;
 Local<u8[8]> saved;
 u32 end=load<u32>(obj+0x98),begin=load<u32>(obj+0x94);
 store<u32>(ea(saved.get()),begin);store<u32>(ea(saved.get())+4,end);
 u32 allocated=call_ptr<u32>(ext_vtarget(obj,0x34),heap,1,4);
 call(0x02755478,heap,saved.get());if(!allocated)return;
 u32 start=call_ptr<u32>(ext_vtarget(obj,0x5C),heap),header=call<u32>(0x02754C90,0);
 if(allocated!=start+header)call(0x0273AA24,at<void>(0x10058910),0xC5D,at<void>(0x100588F0));
 u32 delta=((allocated+alignment-1)&~(alignment-1))-allocated;
 if(delta)call_ptr(ext_vtarget(obj,0x34),heap,delta,4);
}
VERIFY(0x025E32A0,ext_alignHeap);
u32 ext_createSolidHeap(u32 requested,void* parent,u32 alignment) {
 WWHD_FUNC(0x025E33A8,u32,requested,parent,alignment);
 if(!alignment)alignment=32;
 u32 pobj=ea(parent);if(!pobj)pobj=call<u32>(0x027EC230);
 bool unit=call<u32>(0x027EC23C,at<void>(pobj))==0x554E4954;
 u32 heap;
 if(!requested||requested==0xFFFFFFFF){heap=call<u32>(0x027EC464,0xFFFFFFFF,at<void>(pobj),0);}
 else {u32 size=(requested+3)&~3u;size+=call<u32>(0x02754C90,4);if(alignment>4&&!unit)size+=alignment-4;heap=call<u32>(0x027EC464,size,at<void>(pobj),0);}
 if(heap){call_ptr<u32>(ext_vtarget(heap,0x5C),at<void>(heap));call<u32>(0x02754C90,0);if(!unit)call(0x025E32A0,at<void>(heap),alignment);if(load<u8>(0x101F47B8))call_ptr(ext_vtarget(heap,0x6C),at<void>(heap));}
 store<u32>(0x101F4754,heap);store<u32>(0x101F4758,alignment);return heap;
}
VERIFY(0x025E33A8,ext_createSolidHeap);
u32 ext_adjustSolidHeap(void* heap) {
 WWHD_FUNC(0x025E3678,u32,heap);
 u32 obj=ea(heap);if(!obj)return 0;
 bool same=load<u32>(0x101F4754)==obj;u32 target=ext_vtarget(obj,0x9C),alignment=0;
 if(same)alignment=load<u32>(0x101F4758);
 u32 check=call_ptr<u32>(target,heap);
 if(check){u32 parent=load<u32>(obj+0x24);check=call_ptr<u32>(ext_vtarget(parent,0x94),at<void>(parent));}
 if(!check)return call_ptr<u32>(ext_vtarget(obj,0x6C),heap);
 call_ptr(ext_vtarget(obj,0x6C),heap);u32 reduced=call<u32>(0x027EC460,heap);
 call_ptr(ext_vtarget(obj,0x6C),heap);u32 start=call_ptr<u32>(ext_vtarget(obj,0x5C),heap);
 u32 header=call<u32>(0x02754C90,0),base=start+header;
 if(alignment)return reduced+base-((base+alignment-1)&~(alignment-1));return reduced;
}
VERIFY(0x025E3678,ext_adjustSolidHeap);
void ext_destroySolidHeap(void* heap) {
 WWHD_FUNC(0x025E3868,void,heap);
 if(load<u32>(0x101F4754)==ea(heap)){store<u32>(0x101F4754,0);store<u32>(0x101F4758,0);}
 call_ptr(ext_vtarget(ea(heap),0x24),heap);
}
VERIFY(0x025E3868,ext_destroySolidHeap);
