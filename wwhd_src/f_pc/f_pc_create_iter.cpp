#include "gabi.h"
using namespace gabi;
namespace f_pc_create_iter {
struct Filter { u32 callback; u32 user; };
struct LayerFilter { u32 layer; u32 callback; u32 user; };
static_assert(sizeof(Filter)==8 && sizeof(LayerFilter)==12);
s32 method(u32 callback,void *user) {
  WWHD_FUNC(0x025DD6D8,s32,callback,user);
  FrameLocal<Filter> filter(8); /* the original keeps it at sp+8 of its 0x18 frame and calls at that sp */
  store<u32>(ea(filter.get()),callback);
  store<u32>(ea(filter.get())+4,ea(user));
  return call<s32>(0x020100A0,at<void>(0x101F39A4),at<void>(0x0201A9F4),filter.get());
}
VERIFY(0x025DD6D8,method);
void *judge(u32 callback,void *user) {
  WWHD_FUNC(0x025DD714,void *,callback,user);
  FrameLocal<Filter> filter(8); /* the original keeps it at sp+8 of its 0x18 frame and calls at that sp */
  store<u32>(ea(filter.get()),callback);
  store<u32>(ea(filter.get())+4,ea(user));
  return call<void *>(0x020100BC,at<void>(0x101F39A4),at<void>(0x0201AA08),filter.get());
}
VERIFY(0x025DD714,judge);
void *filter_judge_layer(void *tag,void *filter) {
  WWHD_FUNC(0x025DD750,void *,tag,filter);
  u32 request=load<u32>(ea(tag)+12);
  u32 layer=load<u32>(request+0x44);
  u32 expected=load<u32>(ea(filter));
  if(load<u32>(layer+12)!=expected)return nullptr;
  u32 callback=load<u32>(ea(filter)+4);
  void *user=at<void>(load<u32>(ea(filter)+8));
  return call_ptr<void *>(callback,at<void>(load<u32>(request+0x40)),user);
}
VERIFY(0x025DD750,filter_judge_layer);
void *judge_layer(u32 layer,u32 callback,void *user) {
  WWHD_FUNC(0x025DD780,void *,layer,callback,user);
  Local<LayerFilter> filter;
  store<u32>(ea(filter.get()),layer);
  store<u32>(ea(filter.get())+4,callback);
  store<u32>(ea(filter.get())+8,ea(user));
  return call<void *>(0x025DD714,at<void>(0x025DD750),filter.get());
}
VERIFY(0x025DD780,judge_layer);
void static_init() {
  WWHD_FUNC(0x025DD7B8,void);
  store<u32>(0x1048A6D0,0);store<u32>(0x1048A6C8,0);store<u32>(0x1048A6D4,0);store<u32>(0x1048A6CC,0);
  call<void>(0x028F026C,at<void>(0x101F3938));
  f32 first=load<f32>(0x10057E74),second=load<f32>(0x10057E78);
  store<f32>(0x1048A6BC,first);store<f32>(0x1048A6C0,second);
  call<void>(0x028ED6F8,at<void>(0x1048A6C4));call<void>(0x028F026C,at<void>(0x101F3944));
  call<void>(0x028EAB2C,at<void>(0x1048A6C5));call<void>(0x028F026C,at<void>(0x101F3950));
}
VERIFY(0x025DD7B8,static_init);
}
