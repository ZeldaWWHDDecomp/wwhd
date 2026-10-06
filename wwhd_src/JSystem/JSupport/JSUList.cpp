// Reconstruction of HD JSU pointer lists;
#include "gabi.h"
using namespace gabi;
namespace jsu_list {
static u32 get(u32 p,u32 off=0){return load<u32>(p+off);}
static void put(u32 p,u32 off,u32 v){store<u32>(p+off,v);}
u32 link_ctor(u32 self,u32 object){
 WWHD_FUNC(0x027EC50C,u32,self,object);
 if(!self)self=call<u32>(0x0273AD10,16u);
 if(self){put(self,0,object);put(self,4,0);put(self,8,0);put(self,12,0);}return self;
}
VERIFY(0x027EC50C,link_ctor);
u32 remove(u32 self,u32 link){
 WWHD_FUNC(0x027EC560,u32,self,link);
 if(get(link,4)!=self)return 0;
 if(get(self,8)==1){put(self,4,0);put(self,0,0);}
 else if(link==get(self)){put(get(link,12),8,0);put(self,0,get(link,12));}
 else if(link==get(self,4)){put(get(link,8),12,0);put(self,4,get(link,8));}
 else{u32 previous=get(link,8);put(previous,12,get(link,12));u32 next=get(link,12);put(next,8,get(link,8));}
 put(link,4,0);put(self,8,get(self,8)-1);return 1;
}
VERIFY(0x027EC560,remove);
void link_dtor(u32 self,u32 flags){
 WWHD_FUNC(0x027EC62C,void,self,flags);
 if(self){u32 list=get(self,4);if(list)call<u32>(0x027EC560,list,self);if(flags&1)call<void>(0x0273AF40,self);}
}
VERIFY(0x027EC62C,link_dtor);
void initiate(u32 self){
 WWHD_FUNC(0x027EC678,void,self);put(self,0,0);put(self,4,0);put(self,8,0);
}
VERIFY(0x027EC678,initiate);
void list_dtor(u32 self,u32 flags){
 WWHD_FUNC(0x027EC68C,void,self,flags);if(!self)return;
 u32 count=get(self,8),removed=0,node=get(self);
 while(removed<count){put(node,4,0);count=get(self,8);++removed;node=get(node,12);}
 if(flags&1)call<void>(0x0273AF40,self);
}
VERIFY(0x027EC68C,list_dtor);
void set_first(u32 self,u32 first){
 WWHD_FUNC(0x027EC6D0,void,self,first);put(first,4,self);put(first,8,0);put(first,12,0);put(self,4,first);put(self,0,first);put(self,8,1);
}
VERIFY(0x027EC6D0,set_first);
u32 append(u32 self,u32 link){
 WWHD_FUNC(0x027EC6F4,u32,self,link);u32 old=get(link,4),result=old?call<u32>(0x027EC560,old,link):1;
 if(result){if(!get(self,8))call<void>(0x027EC6D0,self,link);
  else{put(link,4,self);u32 tail=get(self,4);put(link,12,0);put(link,8,tail);put(get(self,4),12,link);u32 count=get(self,8);put(self,4,link);put(self,8,count+1);}}
 return result;
}
VERIFY(0x027EC6F4,append);
u32 prepend(u32 self,u32 link){
 WWHD_FUNC(0x027EC778,u32,self,link);u32 old=get(link,4),result=old?call<u32>(0x027EC560,old,link):1;
 if(result){if(!get(self,8))call<void>(0x027EC6D0,self,link);
  else{put(link,4,self);put(link,8,0);put(link,12,get(self));put(get(self),8,link);u32 count=get(self,8);put(self,0,link);put(self,8,count+1);}}
 return result;
}
VERIFY(0x027EC778,prepend);
u32 insert(u32 self,u32 before,u32 link){
 WWHD_FUNC(0x027EC7FC,u32,self,before,link);
 if(before==get(self))return call<u32>(0x027EC778,self,link);
 if(!before)return call<u32>(0x027EC6F4,self,link);
 if(get(before,4)!=self)return 0;
 u32 old=get(link,4),result=old?call<u32>(0x027EC560,old,link):1;
 if(result){u32 previous=get(before,8);put(link,4,self);put(link,8,previous);put(link,12,before);put(previous,12,link);put(before,8,link);put(self,8,get(self,8)+1);}
 return result;
}
VERIFY(0x027EC7FC,insert);
void static_init(){
 WWHD_FUNC(0x027EC8E0,void);put(0x104A2470,8,0);put(0x104A2470,0,0);put(0x104A2470,12,0);put(0x104A2470,4,0);call<void>(0x028F026C,0x101F97B0u);
}
VERIFY(0x027EC8E0,static_init);
}
