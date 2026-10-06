/* Retained HD 28-byte DSPChannel cluster028157D8..02815DA4.
 * GC-only channel APIs and neighboring DSPInterface excluded. */
#include "gabi.h"
using namespace gabi;
namespace dsp_channel_cpp {
void* construct(void* self) {
  WWHD_FUNC(0x028157D8, void*, self);
u32 p=ea(self);if(!p) p=call<u32>(0x0273AD10,28u);
if(p) {store<u32>(p,1);store<u16>(p+4,65535);store<u32>(p+0x18,0);store<u32>(p+8,0);store<u32>(p+0x14,0);store<u32>(p+0xC,0);store<u32>(p+0x10,0);}return at<void>(p);
}
VERIFY(0x028157D8, construct);
void* getLower(s32 maximum) {
  WWHD_FUNC(0x02815834, void*, maximum);
u32 base=load<u32>(0x101F9F64),time=0; s32 chosen=-1,priority=255;
for(u32 i=0;i<64;++i) {u32 p=base+i*28;s32 pr=load<s16>(p+4);
 if(pr<0) return at<void>(p);
 if(pr<=maximum && pr<=priority) {u32 age=load<u32>(p+0xC);if(pr<priority || age>time) {time=age;chosen=i;priority=pr;}}
}return chosen<0?nullptr:at<void>(base+u32(chosen)*28);
}
VERIFY(0x02815834, getLower);
void forceStop(void* self) {
  WWHD_FUNC(0x028158C8, void, self);
u32 p=ea(self),cb=load<u32>(p+0x10);
if(cb) call_ptr<s32>(cb,3u,0u,load<u32>(p+0x14));
u32 flags=load<u32>(p+8)&~1u,status=load<u32>(p);
store<u16>(p+4,65535);store<u32>(p+0x10,0);store<u32>(p+0x14,0);store<u32>(p+8,flags);
if(status==0) {store<u32>(p+8,flags|2);store<u32>(p,2);}
}
VERIFY(0x028158C8, forceStop);
void* allocate(s32 priority, u32 callback, u32 data) {
  WWHD_FUNC(0x0281594C, void*, priority, callback, data);
void *out=call<void*>(0x02815834,priority);u32 p=ea(out);
if(p) {call<void>(0x028158C8,out);store<u32>(p+0x10,callback);store<u32>(p+0xC,0);store<u32>(p+0x14,data);store<u16>(p+4,priority);}return out;
}
VERIFY(0x0281594C, allocate);
void detachCallback(void* self) {
  WWHD_FUNC(0x028159C0, void, self);
store<u32>(ea(self)+0x10,0);store<u32>(ea(self)+0x14,0);
}
VERIFY(0x028159C0, detachCallback);
void setPriority(void* self, u32 value) {
  WWHD_FUNC(0x028159D0, void, self, value);
store<u16>(ea(self)+4,value);
}
VERIFY(0x028159D0, setPriority);
void play(void* self) {
  WWHD_FUNC(0x028159D8, void, self);
u32 p=ea(self);store<u32>(p+8,load<u32>(p+8)|1);
}
VERIFY(0x028159D8, play);
void markWaveReady(void* self) {
  WWHD_FUNC(0x028159E8, void, self);
u32 buffer=load<u32>(ea(self)+0x18);if(buffer)call<void>(0x028161B8,at<void>(buffer));
}
VERIFY(0x028159E8, markWaveReady);
void initAll() {
  WWHD_FUNC(0x028159F8, void);
u32 padding=load<u32>(0x101FCD08);
u32 allocation=call<u32>(0x0273B0D4,padding+1792,at<void>(load<u32>(0x101F9DA8)),32u);
u32 base=allocation?call<u32>(0x028EFF8C,at<void>(allocation+load<u32>(0x101FCD08)),64u,28u,0x028157D8u,0u):0;
store<u32>(0x101F9F64,base);
for(u32 i=0;i<64;++i) {u32 p=load<u32>(0x101F9F64)+i*28;u32 buffer=call<u32>(0x02816240,i);store<u32>(p+0x18,buffer);}
}
VERIFY(0x028159F8, initAll);
void* getLowerActive() {
  WWHD_FUNC(0x02815AB0, void*);
u32 base=load<u32>(0x101F9F64),time=0;s32 chosen=-1,priority=255;
for(u32 i=0;i<64;++i){u32 p=base+i*28;if(load<u32>(p)!=0)continue;s32 pr=load<s16>(p+4);
 if(pr<127 && pr<=priority){u32 age=load<u32>(p+0xC);if(pr<priority || age>time){time=age;chosen=i;priority=pr;}}
}return chosen<0?nullptr:at<void>(base+u32(chosen)*28);
}
VERIFY(0x02815AB0, getLowerActive);
u32 breakLowerActive() {
  WWHD_FUNC(0x02815B40, u32);
void *p=call<void*>(0x02815AB0);if(!p)return 0;call<void>(0x028158C8,p);return 1;
}
VERIFY(0x02815B40, breakLowerActive);
void update(void* self) {
  WWHD_FUNC(0x02815B84, void, self);
u32 p=ea(self);u32 finished=call<u32>(0x02815EDC,at<void>(load<u32>(p+0x18)));
u32 flags=load<u32>(p+8);
if(finished){u32 status=load<u32>(p);store<u32>(p+8,flags&~2u);
 if(status==0){u32 cb=load<u32>(p+0x10);if(!cb || call_ptr<s32>(cb,2u,0u,load<u32>(p+0x14))<0)store<u16>(p+4,65535);}
 u32 buffer=load<u32>(p+0x18);store<u32>(p,1);call<void>(0x02815ECC,at<void>(buffer));call<void>(0x02816444,at<void>(load<u32>(p+0x18)));return;
}
if(flags&2){u32 buffer=load<u32>(p+0x18);store<u32>(p+8,flags&~2u);call<void>(0x02815EC0,at<void>(buffer));call<void>(0x02816444,at<void>(load<u32>(p+0x18)));return;}
u32 status=load<u32>(p);if(status==2)return;
if(status==1){if(!(flags&1))return;u32 buffer=load<u32>(p+0x18);store<u32>(p,0);store<u32>(p+8,flags&~1u);call<void>(0x02815E18,at<void>(buffer));
 u32 cb=load<u32>(p+0x10);if(cb)call_ptr<s32>(cb,1u,load<u32>(p+0x18),load<u32>(p+0x14));
 call<void>(0x02815E3C,at<void>(load<u32>(p+0x18)));call<void>(0x02816444,at<void>(load<u32>(p+0x18)));return;}
u32 cb=load<u32>(p+0x10);bool have=cb!=0;
if(cb && call_ptr<s32>(cb,0u,load<u32>(p+0x18),load<u32>(p+0x14))<0){cb=load<u32>(p+0x10);store<u32>(p,1);
 if(!cb || call_ptr<s32>(cb,2u,0u,load<u32>(p+0x14))<0)store<u16>(p+4,65535);
 call<void>(0x02815EB4,at<void>(load<u32>(p+0x18)));call<void>(0x02816444,at<void>(load<u32>(p+0x18)));return;}
store<u32>(p+0xC,load<u32>(p+0xC)+1);if(have)call<void>(0x02816444,at<void>(load<u32>(p+0x18)));
}
VERIFY(0x02815B84, update);
void updateAll() {
  WWHD_FUNC(0x02815D44, void);
for(u32 i=0;i<64;++i)call<void>(0x02815B84,at<void>(load<u32>(0x101F9F64)+i*28));call<void>(0x02816210,3u);
}
VERIFY(0x02815D44, updateAll);
void staticInit() {
  WWHD_FUNC(0x02815DA4, void);
store<u32>(0x104B5644,0);store<u32>(0x104B563C,0);store<u32>(0x104B5648,0);store<u32>(0x104B5640,0);call<void>(0x028F026C,at<void>(0x101F9F58));
}
VERIFY(0x02815DA4, staticInit);
}
