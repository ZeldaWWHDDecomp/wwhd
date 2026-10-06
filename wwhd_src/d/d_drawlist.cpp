#include "bindings.h"
#include "gabi.h"

namespace {
using namespace gabi;
template<class T> T read(u32 a,u32 o=0){return load<T>(a+o);}
template<class T> void write(u32 a,u32 o,T v){store<T>(a+o,v);}

void setViewPort(u32 window,f32 x,f32 y,f32 width,f32 height,f32 nearZ,f32 farZ){
 WWHD_FUNC(0x0252CC8C,void,window,x,y,width,height,nearZ,farZ);
 write(window,16,nearZ);write(window,0,x);write(window,12,height);
 write(window,20,farZ);write(window,4,y);write(window,8,width);
}
VERIFY(0x0252CC8C,setViewPort);
void setScissor(u32 window,f32 x,f32 y,f32 width,f32 height){
 WWHD_FUNC(0x0252CCA8,void,window,x,y,width,height);
 write(window,32,width);write(window,28,y);write(window,36,height);write(window,24,x);
}
VERIFY(0x0252CCA8,setScissor);
u32 constructDrawBase(u32 object){
 WWHD_FUNC(0x0252CCBC,u32,object);
 if(!object)object=call<u32>(0x0273AD10,4);
 if(object)write<u32>(object,0,0x1004CBA8);
 return object;
}
VERIFY(0x0252CCBC,constructDrawBase);
void destroyDrawBase(u32 object,u32 flags){
 WWHD_FUNC(0x0252CCFC,void,object,flags);
 if(object&&(flags&1))call(0x0273AF40,at<u8>(object),flags);
}
VERIFY(0x0252CCFC,destroyDrawBase);
void setThreeWords(u32 object,u32 a,u32 b,u32 c){
 WWHD_FUNC(0x0252CD10,void,object,a,b,c);
 write(object,4,b);write(object,8,c);write(object,0,a);
}
VERIFY(0x0252CD10,setThreeWords);
u32 appendPacket(u32 unused,u32 current,u32 end,u32 packet){
 WWHD_FUNC(0x0252CDC0,u32,unused,current,end,packet);
 u32 atSlot=read<u32>(current),limit=read<u32>(end);
 if(atSlot>=limit)return 0;
 write(atSlot,0,packet);atSlot=read<u32>(current);write(current,0,atSlot+4);return 1;
}
VERIFY(0x0252CDC0,appendPacket);
void drawPacketRange(u32 unused,u32 begin,u32 end){
 WWHD_FUNC(0x0252F480,void,unused,begin,end);
 for(u32 cursor=begin;cursor<end;cursor+=4){
  u32 packet=read<u32>(cursor),table=read<u32>(packet),draw=read<u32>(table,20);
  call_ptr(draw,at<u8>(packet));
 }
}
VERIFY(0x0252F480,drawPacketRange);
struct DrawVector {be<f32> x,y,z;};
void compareProjectedDepth(u32 pair){
 WWHD_FUNC(0x0252F4E0,void,pair);Local<DrawVector> first,second;
 call(0x028E8F64,at<u8>(0x104B45F8),at<u8>(pair),first.get());
 call(0x028E8F64,at<u8>(0x104B45F8),at<u8>(pair+12),second.get());
 f32 b=second->z,a=first->z;write<u8>(pair,24,b<a);
}
VERIFY(0x0252F4E0,compareProjectedDepth);
void setDepthColour(u32 colour,f32 depth){
 WWHD_FUNC(0x0252F54C,void,colour,depth);
 f32 positive=read<f32>(0x1004CA30);write<u8>(0x101D616C,0,1);
 f32 negative=read<f32>(0x1004CA34);write(0x101D6164,0,depth);
 u32 packed=read<u32>(colour);f32 direction=depth>=0?negative:positive;
 write(0x101D6168,0,packed);write(0x101D6160,0,direction);
}
VERIFY(0x0252F54C,setDepthColour);
void setNegativeDepth(f32 depth){
 WWHD_FUNC(0x0252F590,void,depth);call(0x0252F54C,at<u8>(0x101D5E94),-depth);
}
VERIFY(0x0252F590,setNegativeDepth);
void terminateText(u32 text){
 WWHD_FUNC(0x0252F6BC,void,text);u32 buffer=read<u32>(text),size=read<u32>(text,8);write<u8>(buffer+size-1,0,0);
}
VERIFY(0x0252F6BC,terminateText);
void destroyText(u32 object,u32 flags){
 WWHD_FUNC(0x0252F6D4,void,object,flags);if(object&&(flags&1))call(0x0273AF40,at<u8>(object),flags);
}
VERIFY(0x0252F6D4,destroyText);
void destroyPacket(u32 object,u32 flags){
 WWHD_FUNC(0x0252F6E8,void,object,flags);if(!object)return;
 call(0x027F13DC,at<u8>(object),0);if(flags&1)call(0x0273AF40,at<u8>(object));
}
VERIFY(0x0252F6E8,destroyPacket);
u32 constructTexturePacket(u32 object){
 WWHD_FUNC(0x0252F74C,u32,object);if(!object)object=call<u32>(0x0273AD10,0x254);
 if(object){call(0x027B5BD8,at<u8>(object+4));call(0x027BF734,at<u8>(object+0x158));write<u32>(object,0x250,0);write<u32>(object,0x24C,0);}return object;
}
VERIFY(0x0252F74C,constructTexturePacket);
u32 constructSmallPacket(u32 object){
 WWHD_FUNC(0x0252F7A8,u32,object);if(!object)object=call<u32>(0x0273AD10,16);return object;
}
VERIFY(0x0252F7A8,constructSmallPacket);
void destroyTexturePacket(u32 object,u32 flags){
 WWHD_FUNC(0x0252F7D8,void,object,flags);if(!object)return;
 call(0x027BF880,at<u8>(object+0x158),2);call(0x027B5CBC,at<u8>(object+4),2);
 if(flags&1)call(0x0273AF40,at<u8>(object));
}
VERIFY(0x0252F7D8,destroyTexturePacket);
void destroyPacketCopy(u32 object,u32 flags){
 WWHD_FUNC(0x0252F9D8,void,object,flags);if(!object)return;
 call(0x027F13DC,at<u8>(object),0);if(flags&1)call(0x0273AF40,at<u8>(object));
}
VERIFY(0x0252F9D8,destroyPacketCopy);
void emptyF73C(){WWHD_FUNC(0x0252F73C,void);}
VERIFY(0x0252F73C,emptyF73C);void emptyF740(){WWHD_FUNC(0x0252F740,void);}
VERIFY(0x0252F740,emptyF740);
void emptyF744(){WWHD_FUNC(0x0252F744,void);}
VERIFY(0x0252F744,emptyF744);void emptyF748(){WWHD_FUNC(0x0252F748,void);}
VERIFY(0x0252F748,emptyF748);
void emptyF7D4(){WWHD_FUNC(0x0252F7D4,void);}
VERIFY(0x0252F7D4,emptyF7D4);void emptyF9D4(){WWHD_FUNC(0x0252F9D4,void);}
VERIFY(0x0252F9D4,emptyF9D4);void emptyFB60(){WWHD_FUNC(0x0252FB60,void);}
VERIFY(0x0252FB60,emptyFB60);
void destroyFB4C(u32 object,u32 flags){WWHD_FUNC(0x0252FB4C,void,object,flags);if(object&&(flags&1))call(0x0273AF40,at<u8>(object),flags);}
VERIFY(0x0252FB4C,destroyFB4C);void destroyFB64(u32 object,u32 flags){WWHD_FUNC(0x0252FB64,void,object,flags);if(object&&(flags&1))call(0x0273AF40,at<u8>(object),flags);}
VERIFY(0x0252FB64,destroyFB64);
void destroyFB78(u32 object,u32 flags){WWHD_FUNC(0x0252FB78,void,object,flags);if(object&&(flags&1))call(0x0273AF40,at<u8>(object),flags);}
VERIFY(0x0252FB78,destroyFB78);void destroyFB8C(u32 object,u32 flags){WWHD_FUNC(0x0252FB8C,void,object,flags);if(object&&(flags&1))call(0x0273AF40,at<u8>(object),flags);}
VERIFY(0x0252FB8C,destroyFB8C);
void destroyFBA0(u32 object,u32 flags){WWHD_FUNC(0x0252FBA0,void,object,flags);if(object&&(flags&1))call(0x0273AF40,at<u8>(object),flags);}
VERIFY(0x0252FBA0,destroyFBA0);void destroyFBB4(u32 object,u32 flags){WWHD_FUNC(0x0252FBB4,void,object,flags);if(object&&(flags&1))call(0x0273AF40,at<u8>(object),flags);}
VERIFY(0x0252FBB4,destroyFBB4);
u32 constructRandomState(u32 object){
 WWHD_FUNC(0x0252FBC8,u32,object);if(!object)object=call<u32>(0x0273AD10,12);
 if(object){write<u32>(object,0,5);write<u32>(object,4,20);write<u32>(object,8,10);}return object;
}
VERIFY(0x0252FBC8,constructRandomState);
u32 trailingDrawFlag(){WWHD_FUNC(0x0252FCB4,u32);return 1;}
VERIFY(0x0252FCB4,trailingDrawFlag);
void updateDepthFade(){
 WWHD_FUNC(0x0252F5A0,void);if(!read<u8>(0x101D616C))return;
 f32 value=read<f32>(0x101D6160)+read<f32>(0x101D6164),minimum=read<f32>(0x1004CA34);
 if(value<minimum){write(0x101D6160,0,minimum);return;}
 f32 maximum=read<f32>(0x1004CA30);
 if(!(value>maximum)){write(0x101D6160,0,value);return;}
 write(0x101D6160,0,maximum);write<u8>(0x101D616C,0,0);
}
VERIFY(0x0252F5A0,updateDepthFade);
void initializeDrawGlobals(){
 WWHD_FUNC(0x0252F608,void);
 write<u32>(0x1047575C,8,0);write<u32>(0x1047575C,0,0);
 write<u32>(0x1047575C,12,0);write<u32>(0x1047575C,4,0);
 call(0x028F026C,at<u8>(0x101D613C));
 f32 x=read<f32>(0x1004CBA0),y=read<f32>(0x1004CBA4);
 write(0x10475720,0,x);write(0x10475724,0,y);call(0x028ED6F8,at<u8>(0x10475728));
 call(0x028F026C,at<u8>(0x101D6148));call(0x028EAB2C,at<u8>(0x10475729));
 call(0x028F026C,at<u8>(0x101D6154));call(0x0252CCBC,at<u8>(0x10475790));
 write<u32>(0x10475790,0,0x1004CBC0);
}
VERIFY(0x0252F608,initializeDrawGlobals);
void initializeRandomGlobals(){
 WWHD_FUNC(0x0252FC14,void);
 write<u32>(0x10475818,8,0);write<u32>(0x10475818,0,0);
 write<u32>(0x10475818,12,0);write<u32>(0x10475818,4,0);
 call(0x028F026C,at<u8>(0x101D6170));
 f32 x=read<f32>(0x1004CC68),y=read<f32>(0x1004CC6C);
 write(0x1047580C,0,x);write(0x10475810,0,y);call(0x028ED6F8,at<u8>(0x10475814));
 call(0x028F026C,at<u8>(0x101D617C));call(0x028EAB2C,at<u8>(0x10475815));
 call(0x028F026C,at<u8>(0x101D6188));call(0x0252FBC8,at<u8>(0x10475828));
}
VERIFY(0x0252FC14,initializeRandomGlobals);
u32 constructDrawTriangle(u32 object){
 WWHD_FUNC(0x0252CD20,u32,object);if(!object)object=call<u32>(0x0273AD10,0x38);
 if(!object)return 0;
 call(0x0252CCBC,at<u8>(object));write<u32>(object,0,0x1004CBF0);
 call(0x0252CD10,at<u8>(object+4),100,100,100);call(0x028F521C,at<u8>(object+0x1C),4);
 f32 zero=read<f32>(0x1004CA34);
 write<u16>(object,0x24,0);write(object,0x28,zero);write<u16>(object,0x22,0);write(object,0x2C,zero);
 write<u16>(object,0x20,0);write(object,0x30,zero);write<u16>(object,0x26,0);write(object,0x34,zero);return object;
}
VERIFY(0x0252CD20,constructDrawTriangle);
void enqueueDrawTriangle(u32 object,u32 vector,u32 colour,u32 a,u32 b,u32 c,u32 d,f32 x,f32 y,f32 z,f32 w){
 WWHD_FUNC(0x0252CDF0,void,object,vector,colour,a,b,c,d,x,y,z,w);
 write(object,16,read<u32>(vector));write(object,20,read<u32>(vector,4));write(object,24,read<u32>(vector,8));
 write(object,28,read<u32>(colour));write<u16>(object,32,a);write(object,48,z);write<u16>(object,36,c);
 write<u16>(object,38,d);write(object,52,w);write(object,44,y);write<u16>(object,34,b);write(object,40,x);
 u32 list=call<u32>(0x025200D4)+0x5D30;call(0x0252CDC0,at<u8>(list),at<u8>(list+0x1DC),at<u8>(list+0x1E0),at<u8>(object));
}
VERIFY(0x0252CDF0,enqueueDrawTriangle);
u32 enqueueDepthPeek(u32 queue,u32 x,u32 y,u32 output){
 WWHD_FUNC(0x0252E34C,u32,queue,x,y,output);u8 count=read<u8>(queue);
 if(count>=64)return 0;
 u32 entry=queue+(u32)count*8+4;write<u16>(entry,0,x);write<u16>(entry,2,y);write(entry,4,output);
 count=read<u8>(queue);write<u8>(queue,0,count+1);return 1;
}
VERIFY(0x0252E34C,enqueueDepthPeek);
void copyMirrorMatrix(u32 destination,u32 source){
 WWHD_FUNC(0x0252D9C4,void,destination,source);
 f32 matrix[12];
 // Capture the complete matrix before writing, including overlapping matrices.
 matrix[5]=read<f32>(source,20);matrix[4]=read<f32>(source,16);matrix[6]=read<f32>(source,24);
 matrix[7]=read<f32>(source,28);matrix[8]=read<f32>(source,32);matrix[9]=read<f32>(source,36);
 matrix[11]=read<f32>(source,44);matrix[0]=read<f32>(source);matrix[3]=read<f32>(source,12);
 matrix[1]=read<f32>(source,4);matrix[10]=read<f32>(source,40);matrix[2]=read<f32>(source,8);
 for(u32 i=0;i<12;i++)write(destination,i*4,matrix[i]);
}
VERIFY(0x0252D9C4,copyMirrorMatrix);
void resetDrawLists(u32 lists){
 WWHD_FUNC(0x0252F264,void,lists);
 const u32 offsets[]={0x1C,0x20,0x24,0x28,0x2C,0x34,0x38,0x3C,0x30,0x40,0x44,0x48,
  0x4C,0x50,0x54,0x58,0x5C,0x60,0x64,0x6C,0x68,0x70,0x74,0x78};
 for(u32 offset:offsets){u32 buffer=read<u32>(lists,offset);call(0x027F093C,at<u8>(buffer));}
 write<u32>(0x104B4634,0,read<u32>(lists,0x48));write<u32>(0x104B4638,0,read<u32>(lists,0x4C));
 write(lists,0x8C,lists+0x7C);write(lists,0xD4,lists+0x94);write(lists,0x1DC,lists+0xDC);write(lists,0x264,lists+0x1E4);
 call(0x025EDBC0,at<u8>(lists+0x284));call(0x025EDBC0,at<u8>(lists+0x320));
}
VERIFY(0x0252F264,resetDrawLists);
void entryZSort(u32 unused,u32 buffer,u32 packet,u32 position){
 WWHD_FUNC(0x0252F3B0,void,unused,buffer,packet,position);
 Local<DrawVector> snapshot;
 const u32 x=read<u32>(position),z=read<u32>(position,8),y=read<u32>(position,4);
 const u32 local=ea(snapshot.get());write(local,0,x);write(local,4,y);write(local,8,z);
 const double depth=-call<f64>(0x027F2B68,at<u8>(0x104B45F8),snapshot.get());
 u32 bucket=0;
 if(read<f32>(0x1004CB94)<depth){
  bucket=255;
  if(read<f32>(0x1004CB98)>depth){
   const f32 quotient=f32(depth/read<f32>(0x1004CB9C));
   bucket=u32(ftoi(quotient))&0xFFFF;
  }
 }
 call(0x027F0E04,at<u8>(buffer),at<u8>(packet),(255-bucket)&0xFFFF);
}
VERIFY(0x0252F3B0,entryZSort);
void destroyMirrorPacket(u32 object,u32 flags){
 WWHD_FUNC(0x0252F838,void,object,flags);if(!object)return;
 write(object,0x3414,u32(0x1004C9C8));write(object,0x3420,u32(0x1004C988));write(object,0x33DC,u32(0x1004C978));
 call(0x02008B4C,at<u8>(object+0x33BC),0);
 call(0x027FB528,at<u8>(object+0x3054),0);call(0x027FB528,at<u8>(object+0x2FAC),0);
 call(0x027FD764,at<u8>(object+0x2FA0),2);
 if(object+0x2AE0){
  call(0x027BF7E8,at<u8>(object+0x2C38));
  u32 allocation=read<u32>(object,0x2D30);write(object,0x2AE0,u32(0));
  if(allocation){
   const u32 heap=call<u32>(0x02755FEC,at<u8>(read<u32>(0x101F8B4C)),at<u8>(allocation));
   const u32 target=read<u32>(read<u32>(heap,12),0x3C);
   call_ptr(target,at<u8>(heap),at<u8>(read<u32>(object,0x2D30)));
   write(object,0x2D2C,u32(0));write(object,0x2D30,u32(0));
  }
  call(0x027BF7E8,at<u8>(object+0x2E8C));
  allocation=read<u32>(object,0x2F84);write(object,0x2D34,u32(0));
  if(allocation){
   const u32 heap=call<u32>(0x02755FEC,at<u8>(read<u32>(0x101F8B4C)),at<u8>(allocation));
   const u32 target=read<u32>(read<u32>(heap,12),0x3C);
   call_ptr(target,at<u8>(heap),at<u8>(read<u32>(object,0x2F84)));
   write(object,0x2F80,u32(0));write(object,0x2F84,u32(0));
  }
  write(object,0x2F98,u32(0));
  call(0x028F0164,at<u8>(object+0x2AE0),2,0x254,0x0252F7D8,0,0);
 }
 call(0x027BE2B0,at<u8>(object+0x2944),2);call(0x027BE2B0,at<u8>(object+0x27AC),2);
 call(0x027BE2B0,at<u8>(object+0x2614),2);call(0x027F13DC,at<u8>(object),0);
 if(flags&1)call(0x0273AF40,at<u8>(object));
}
VERIFY(0x0252F838,destroyMirrorPacket);
u32 constructFormattedText(u32 object,u32 format,u32 a5,u32 a6,u32 a7,u32 a8,u32 a9,u32 a10){
 WWHD_FUNC(0x0252FA2C,u32,object,format,a5,a6,a7,a8,a9,a10);
 // The formatter accepts the PPC va_list, including the caller's register-save area.
 const u32 entrySP=gabi::cpu->r[1];const bool floatingArguments=gabi::cpu->cr[6]!=0;
 Local<u8[0x80]> frame;const u32 base=ea(frame.get());
 const u32 words[]={object,format,a5,a6,a7,a8,a9,a10};
 for(u32 i=0;i<8;i++)write(base,0x18+i*4,words[i]);
 if(floatingArguments){
  for(u32 i=0;i<8;i++){u64 bits;std::memcpy(&bits,&gabi::cpu->f[i+1].ps0,8);
   write(base,0x38+i*8,u32(bits>>32));write(base,0x3C+i*8,u32(bits));}
 }
 if(!object)object=call<u32>(0x0273AD10,0x4C);
 if(object){
  write(object,0,object+12);write(object,4,u32(0x1004C8B8));write(object,8,u32(64));write<u8>(object,0x4B,0);
  const u32 buffer=read<u32>(object);write(object,4,u32(0x1004C8E0));write<u8>(buffer,0,0);write(object,4,u32(0x1004C930));
  write<u8>(base,8,2);write(base,12,entrySP+8);write(base,16,base+0x18);write<u8>(base,9,0);
  call(0x02759C10,at<u8>(object),at<u8>(format),at<u8>(base+8));
 }
 return object;
}
VERIFY(0x0252FA2C,constructFormattedText);
u32 constructDrawLists(u32 lists){
 WWHD_FUNC(0x0252E9CC,u32,lists);if(!lists)lists=call<u32>(0x0273AD10,0x5C0);if(!lists)return 0;
 const f32 zero=read<f32>(0x1004CA34);
 const u32 before[]={0x6C,0x28,0x78,0x54,0x2C,0x48,0x30,0x50,0x68,0x24,0x5C,0x4C,0x60,0x58,0x64,0x70,0x44,0x38,0x1C,0x74,0x3C,0x34,0x20,0x40};
 for(u32 offset:before)write(lists,offset,u32(0));
 for(u32 offset=0;offset<24;offset+=4)write(lists,offset,zero);write<u8>(lists,0x18,0);
 call(0x028F521C,at<u8>(lists+0x7C),16);write(lists,0x8C,u32(0));write(lists,0x90,u32(0));
 call(0x028F521C,at<u8>(lists+0x94),64);write(lists,0xD4,u32(0));write(lists,0xD8,u32(0));
 call(0x028F521C,at<u8>(lists+0xDC),256);write(lists,0x1DC,u32(0));write(lists,0x1E0,u32(0));
 call(0x028F521C,at<u8>(lists+0x1E4),128);
 const u32 tail[]={0x264,0x278,0x270,0x26C,0x268,0x280,0x274,0x27C};for(u32 offset:tail)write(lists,offset,u32(0));
 call(0x028EFFD0,at<u8>(lists+0x284),2,0x9C,0x025EDC2C);
 const u32 after[]={0x5C,0x3C,0x2C,0x78,0x24,0x38,0x64,0x44,0x54,0x74,0x4C,0x1C,0x58,0x6C,0x30,0x20,0x70,0x50,0x34,0x68,0x28,0x40,0x60,0x48};
 for(u32 offset:after)write(lists,offset,u32(0));
 write(lists,0x268,lists+0x264);write(lists,0x1E0,lists+0x1DC);write(lists,0x90,lists+0x8C);
 write<u8>(lists,0x3BC,0);write(lists,0xD8,lists+0xD4);return lists;
}
VERIFY(0x0252E9CC,constructDrawLists);
void destroyDrawLists(u32 lists,u32 flags){
 WWHD_FUNC(0x0252EBA0,void,lists,flags);if(!lists)return;
 const u32 offsets[]={0x1C,0x20,0x24,0x28,0x2C,0x34,0x38,0x3C,0x30,0x40,0x44,0x48,0x4C,0x50,0x54,0x58,0x5C,0x60,0x64,0x6C,0x68,0x70,0x74,0x78};
 for(u32 offset:offsets)call(0x027F0A0C,at<u8>(read<u32>(lists,offset)),3);
 call(0x028F0164,at<u8>(lists+0x284),2,0x9C,0x0252F6E8,0,0);
 if(flags&1)call(0x0273AF40,at<u8>(lists));
}
VERIFY(0x0252EBA0,destroyDrawLists);
u32 initializeDrawLists(u32 lists){
 WWHD_FUNC(0x0252ED28,u32,lists);
 struct Allocation {u32 field,capacity,name;};
 const Allocation allocations[]={{0x1C,32,0x1004CAFC},{0x20,32,0x1004CAFC},{0x24,1,0x1004CB58},{0x28,32,0x1004CB64},{0x2C,32,0x1004CB70},{0x34,1,0x1004CAD8},{0x38,32,0x1004CAE0},{0x3C,32,0x1004CAE8},{0x30,32,0x1004CB64},{0x40,128,0x1004CB04},{0x44,128,0x1004CB0C},{0x48,256,0x1004CB7C},{0x4C,256,0x1004CB88},{0x50,32,0x1004CB14},{0x54,32,0x1004CAF0},{0x58,256,0x1004CAF0},{0x5C,32,0x1004CB30},{0x60,32,0x1004CB30},{0x64,32,0x1004CB3C},{0x6C,1,0x1004CB1C},{0x68,1,0x1004CB24},{0x70,1,0x1004CAF8},{0x74,1,0x1004CB28},{0x78,32,0x1004CB48}};
 Local<u32[2]> name;
 u32 last=0;
 for(const Allocation& allocation:allocations){
  const u32 local=ea(name.get());write(local,4,u32(0x1004C8A0));write(local,0,allocation.name);
  last=call<u32>(0x0252C8B4,allocation.capacity,name.get());
  write(lists,allocation.field,last);
 }
 const u32 first=read<u32>(lists,0x1C);
 const u32 checks[]={0x20,0x24,0x28,0x2C,0x34,0x38,0x3C,0x30,0x40,0x44,0x48,0x4C,0x50,0x54,0x58,0x5C,0x60,0x64,0x6C,0x68,0x70,0x74};
 if(!first)return 0;for(u32 offset:checks)if(!read<u32>(lists,offset))return 0;if(!last)return 0;
 write(first,12,u32(5));
 const u32 five[]={0x20,0x24,0x28,0x2C,0x34,0x38,0x3C,0x30,0x44};
 for(u32 offset:five)write(read<u32>(lists,offset),12,u32(5));
 const u32 two[]={0x4C,0x50,0x58,0x60,0x64,0x6C};
 for(u32 offset:two)write(read<u32>(lists,offset),12,u32(2));
 const u32 finalFive[]={0x68,0x70,0x74,0x78};for(u32 offset:finalFive)write(read<u32>(lists,offset),12,u32(5));
 write<u32>(0x104B4634,0,read<u32>(lists,0x48));write<u32>(0x104B4638,0,read<u32>(lists,0x4C));
 write(lists,0x8C,lists+0x7C);write(lists,0x264,lists+0x1E4);write(lists,0x1DC,lists+0xDC);write(lists,0xD4,lists+0x94);return 1;
}
VERIFY(0x0252ED28,initializeDrawLists);
void initializeMirror(u32 mirror,u32 texture){
 WWHD_FUNC(0x0252D6E8,void,mirror,texture);
 Local<u32[2]> name;
 if(!texture){
  write(ea(name.get()),4,u32(0x1004C8A0));write(ea(name.get()),0,u32(0x1004CA4C));
  texture=call<u32>(0x026066C4,at<u8>(read<u32>(0x101F4F28)),name.get(),127);
 }
 call(0x02773798,at<u8>(mirror+0x2584),at<u8>(read<u32>(texture,32)));
 const u32 offsets[]={4,8,12,16,20,24,56,52,28};bool same=true;
 for(u32 offset:offsets){if(read<u32>(mirror,0x2614+offset)!=read<u32>(mirror,0x2584+offset)){same=false;break;}}
 if(!same)call(0x027BDEB4,at<u8>(mirror+0x2614),at<u8>(mirror+0x2584));
 else{const u32 a=read<u32>(mirror,0x25AC),b=read<u32>(mirror,0x25B4);write(mirror,0x26E8,a);write(mirror,0x26F0,b);write(mirror,0x263C,a);write(mirror,0x2644,b);}
 write(ea(name.get()),4,u32(0x1004C8A0));write(ea(name.get()),0,u32(0x1004CA54));
 const u32 model=call<u32>(0x027FFCBC);
 const u32 index=call<u32>(0x027B90AC,at<u8>(read<u32>(model,4)),name.get());
 u32 material=0;
 if(s32(index)>=0){
  u32 count=read<u32>(model,8),base=read<u32>(model,12);
  const u32 selected=index<count?base+index*36:base;
  if(!read<u8>(selected,32)){
   const u32 data=read<u32>(model,4),dataCount=read<u32>(data,28);
   const u32 initialization=index<dataCount?read<u32>(data,32)+index*132:0;
   call(0x02800B0C,at<u8>(selected),at<u8>(initialization),0);
   count=read<u32>(model,8);base=read<u32>(model,12);
  }
  material=index<count?base+index*36:base;
 }
 write(mirror,0x2ADC,material);
 const u32 packets=mirror+0x2AE0;
 for(u32 offset=0;offset<0x4A8;offset+=0x254){
  const u32 packet=packets+offset;u32 allocation=read<u32>(packet);
  if(!allocation){
   const u32 heap=call<u32>(0x02756140,at<u8>(read<u32>(0x101F8B4C)));
   const u32 target=read<u32>(read<u32>(heap,12),0x34);
   const u32 fresh=call_ptr<u32>(target,at<u8>(heap),0x260,64);
   if(fresh){write(packet,0x250,fresh);write(packet,0x24C,u32(4));}
   allocation=read<u32>(packet,0x250);write(packet,0,allocation);
  }
  call(0x027FF478,at<u8>(packet+4),at<u8>(allocation),4,at<u8>(packets+0x4AC));
  if(material&&material!=read<u32>(packets,0x4B8))call(0x027FF530,at<u8>(material),at<u8>(packet+0x158),at<u8>(packet+4),at<u8>(packets+0x4AC),0);
 }
 write(packets,0x4B8,material);write<u8>(packets,0x4BC,1);
 call(0x027FE084,at<u8>(mirror+0x2FA0),1,0);
}
VERIFY(0x0252D6E8,initializeMirror);
void setSpecularTextureMatrix(u32 position,u32 model,u32 data,f32 scale){
 WWHD_FUNC(0x0252CA4C,void,position,model,data,scale);
 const f32 one=read<f32>(0x1004CA30),reciprocal=f32(one/scale);
 Local<DrawVector> eye,light,difference;Local<f32[12]> lookAt;Local<u32> textureIndex;
 const u32 camera=call<u32>(0x024F8020);
 call(0x0201ADE0,at<u8>(position),eye.get(),at<u8>(camera+0xDC));
 call(0x02563F64,at<u8>(model+0x84),at<u8>(position),light.get());
 call(0x028E9E88,eye.get(),light.get(),difference.get());
 call(0x028E9684,lookAt.get(),at<u8>(0x101FFBA8),at<u8>(0x101FFBC0),difference.get());
 const u32 matrix=data+0xF8;
 call(0x028E945C,at<u8>(matrix),reciprocal,reciprocal,one);
 call(0x028E9108,at<u8>(matrix),at<u8>(0x101D610C),at<u8>(matrix));
 call(0x028E9108,at<u8>(matrix),lookAt.get(),at<u8>(matrix));
 const f32 zero=read<f32>(0x1004CA34);u32 remaining=read<u16>(data,0x2A);
 write(data,0x104,zero);write(data,0x124,zero);write(data,0x114,zero);
 for(u32 offset=0;remaining;offset+=60,--remaining){
  const u32 material=read<u32>(data,0x34)+offset;
  for(u32 slot=0;slot<8;slot++){
   write<u32>(ea(textureIndex.get()),0,0xFFFFFFFF);
   const u32 texture=call<u32>(0x027FA974,at<u8>(material),slot,textureIndex.get());
   if(!texture||read<u32>(texture)!=10)continue;
   u32 definition=read<u32>(material),relative=read<u32>(definition,0x34);
   const u32 table=relative?definition+0x34+relative:0;
   const u32 index=read<u32>(ea(textureIndex.get())),entry=table+index*20;
   if(s32(read<u32>(entry,4))>=0){
    const u32 flags=read<u16>(material,4),bits=read<u32>(material,12),word=(u32(s32(index)>>5))*4;
    write(material,4,u16(flags|4));write(bits,word,read<u32>(bits,word)|(u32(1)<<(index&31)));
    definition=read<u32>(material);
   }
   relative=read<u32>(definition,0x34);
   const u32 alternateTable=relative?definition+0x34+relative:0;
   const u32 alternate=read<u16>(entry,12);
   if(s32(read<u32>(alternateTable+alternate*20,4))>=0){
    const u32 flags=read<u16>(material,4),bits=read<u32>(material,12),word=(alternate>>5)*4;
    write(material,4,u16(flags|4));write(bits,word,read<u32>(bits,word)|(u32(1)<<(alternate&31)));
   }
   const u32 output=call<u32>(0x027FA678,at<u8>(material),slot);if(output)write(output,0,matrix);
  }
 }
}
VERIFY(0x0252CA4C,setSpecularTextureMatrix);
void bindMirrorTexture(u32 mirror,u32 texture,u32 textureObject,u32 unit){
 const u32 checks[]={4,8,12,16,20,24,56,52,28};bool same=true;
 for(u32 offset:checks)if(read<u32>(mirror,textureObject+offset)!=read<u32>(texture,offset)){same=false;break;}
 if(!same)call(0x027BDEB4,at<u8>(mirror+textureObject),at<u8>(texture));
 else{
  const u32 image=read<u32>(texture,48),buffer=read<u32>(texture,40);
  write(mirror,textureObject+48,image);write(mirror,textureObject+40,buffer);
  write(mirror,textureObject+0xDC,image);write(mirror,textureObject+0xD4,buffer);
 }
 const u32 material=read<u32>(mirror,0x2ADC);u32 record=0;
 if(read<u32>(material,20)>unit)record=read<u32>(material,24)+unit*20;
 call(0x027BE53C,at<u8>(mirror+textureObject),at<u8>(record+4),unit,0);
}
void drawMirrorMaterial(u32 mirror,u32 packet){
 WWHD_FUNC(0x0252D2A0,void,mirror,packet);if(read<u32>(packet,12)!=2)return;
 const u32 context=call<u32>(0x027F29D4,at<u8>(0x104B45C0));
 u32 material=read<u32>(mirror,0x2ADC);const u32 shape=read<u32>(material);
 if(shape!=read<u32>(context,4)){
  const u32 shapeFlags=read<u8>(shape),oldMaterial=read<u32>(context);
  if(shapeFlags&2){write(shape,0,u8(shapeFlags&~2));call(0x027BB9E0,at<u8>(shape),0);}
  const u32 currentMaterial=read<u32>(read<u32>(shape,0x7C),40);
  if(oldMaterial!=currentMaterial)call(0x027B9F68,at<u8>(currentMaterial));
  const u32 size=read<u32>(shape,12);
  if(size)call(0xC00060E0,at<u8>(read<u32>(shape,4)),size);
  else call(0x027BB7CC,at<u8>(shape));
  write(context,0,currentMaterial);write(context,4,shape);material=read<u32>(mirror,0x2ADC);
 }
 call(0x027FE118,at<u8>(mirror+0x2FA0),at<u8>(material),0);
 Local<u8[0x11C]> renderState;const u32 state=ea(renderState.get());
 call(0x02750250,renderState.get());
 const u32 config=read<u32>(state,0xEC);
 write(state,0xE0,u8(0));write(state,1,u8(0));write(state,0,u8(1));write(state,8,u32(0));
 write(state,0xEC,u32(((config&~15u)+7u&~240u)+16u));
 call(0x02750370,renderState.get());
 material=read<u32>(mirror,0x2ADC);
 const u32 record=read<u32>(material,20)?read<u32>(material,24):0;
 call(0x027BE53C,at<u8>(mirror+0x2614),at<u8>(record+4),0,0);
 u32 resource=read<u32>(read<u32>(packet),8);
 if(!read<u32>(0x101FD7BC)){write<u32>(0x101FD7BC,0,1);write<u32>(0x101FDCF8,0,0x1004C920);}
 if(resource){const u32 target=read<u32>(read<u32>(resource,24),12);if(!call_ptr<u32>(target,at<u8>(resource)))resource=0;}
 resource=read<u32>(resource,0x3C);
 if(resource){
  if(read<u32>(resource,0xA4)){
   if(read<u8>(resource,0x90)){call(0x027B6F90,at<u8>(resource));write(resource,0x90,u8(0));}
   call(0xC00061A0,at<u8>(resource+0xA8));call(0x0276B114,at<u8>(read<u32>(0x101F8BD8)));
  }
  call(0x027B6F18,at<u8>(resource));bindMirrorTexture(mirror,resource,0x27AC,1);
 }
 resource=call<u32>(0x027F81A4,at<u8>(read<u32>(0x101F9968)),3);
 if(resource)bindMirrorTexture(mirror,resource,0x2944,2);
 const u32 selected=read<u32>(mirror,0x2F88)?0:0x254;
 call(0x027BFE5C,at<u8>(mirror+0x2AE0+selected+0x158));
 const u32 count=read<u32>(0x104B4D08);
 if(count)call(0xC0006178,read<u32>(0x104B4D00),count,read<u32>(0x104B4CFC),read<u32>(0x104B4D04),0,1);
}
VERIFY(0x0252D2A0,drawMirrorMaterial);
u32 constructMirrorPacket(u32 mirror){
 WWHD_FUNC(0x0252CE74,u32,mirror);if(!mirror)mirror=call<u32>(0x0273AD10,0x3428);if(!mirror)return 0;
 call(0x027F1278,at<u8>(mirror));write(mirror,12,u32(0x1004CC08));
 call(0x028F521C,at<u8>(mirror+0x98),48);call(0x028F521C,at<u8>(mirror+0xC8),64);
 call(0x028F521C,at<u8>(mirror+0x108),48);call(0x028F521C,at<u8>(mirror+0x138),0x2408);
 write(mirror,0x138,u16(0));write(mirror,0x13C,u32(0x1004CA18));
 call(0x028F521C,at<u8>(mirror+0x2540),64);write(mirror,0x2580,u8(0));
 call(0x027BE6B8,at<u8>(mirror+0x2584));call(0x027BDF7C,at<u8>(mirror+0x2614));
 call(0x027BDF7C,at<u8>(mirror+0x27AC));call(0x027BDF7C,at<u8>(mirror+0x2944));
 u32 packets=mirror+0x2AE0;write(mirror,0x2ADC,u32(0));
 if(!packets)packets=call<u32>(0x0273AD10,0x4C0);
 if(packets){
  call(0x028EFFD0,at<u8>(packets),2,0x254,0x0252F74C);
  write(packets,0x4A8,u32(0));write(packets,0x4AC,u32(0));write(packets,0x4B8,u32(0));
  write(packets,0x4B0,u32(0x98));write(packets,0x4BC,u8(0));
  write(packets,0,u32(0));write(packets,0x254,u32(0));
 }
 call(0x027FD6F4,at<u8>(mirror+0x2FA0));call(0x027FB40C,at<u8>(mirror+0x2FAC));
 write(mirror,0x2FB8,u32(0x1016EF84));call(0x028F521C,at<u8>(mirror+0x3020),0x34);
 if(!(mirror+0x3020))call<u32>(0x0273AD10,48);
 call(0x027FB40C,at<u8>(mirror+0x3054));write(mirror,0x3060,u32(0x1016EFB4));
 call(0x028F521C,at<u8>(mirror+0x30C8),0x2F0);
 const f32 zero=read<f32>(0x10145180),one=read<f32>(0x1014517C);
 for(u32 i=0;i<44;i++)write(mirror,0x30C8+i*4,(i&3)==3?one:zero);
 call(0x028EFFD0,at<u8>(mirror+0x3178),2,16,0x0252F7A8);
 call(0x028EFFD0,at<u8>(mirror+0x3198),2,16,0x0252F7A8);
 call(0x028EFFD0,at<u8>(mirror+0x31B8),2,16,0x0252F7A8);
 for(u32 offset=0x11C;offset<=0x26C;offset+=0x30)if(!(mirror+0x30BC+offset))call<u32>(0x0273AD10,48);
 for(u32 offset=0x290;offset<=0x2E0;offset+=16)if(!(mirror+0x30C8+offset))call<u32>(0x0273AD10,16);
 write(mirror,0x33B8,u8(0));call(0x02008FEC,at<u8>(mirror+0x33BC));
 const u32 byteOffsets[]={0x3419,0x341D,0x341C,0x341E,0x3418,0x341A};
 for(u32 offset:byteOffsets)write(mirror,offset,u8(0));write(mirror,0x341B,u8(1));
 write(mirror,0x3414,u32(0x1004CA08));write(mirror,0x3424,u32(31));
 write(mirror,0x33BC,mirror+0x3414);write(mirror,0x33C0,mirror+0x3420);
 write(mirror,0x3420,u32(0x1004C9F8));write(mirror,0x33CC,u32(0x1004C9D8));write(mirror,0x33DC,u32(0x1004C9E8));return mirror;
}
VERIFY(0x0252CE74,constructMirrorPacket);

void updateMirrorPacket(u32 mirror,u32 matrix){
 WWHD_FUNC(0x0252DA64,void,mirror,matrix);
 Local<u8[0x2A0]> scratch; const u32 base=ea(scratch.get());
 auto local=[&](u32 offset){return base+offset-8;};
 const f32 a=read<f32>(0x1004CA68),b=read<f32>(0x1004CA6C),c=read<f32>(0x1004CA70),d=read<f32>(0x1004CA74);
 const f32 zero=read<f32>(0x1004CA34),extent=read<f32>(0x1004CA78);
 for(u32 i=0;i<8;i++){
  const u32 v=local(0x44+i*12);write(v,0,(i%4==0||i%4==3)?a:c);write(v,4,i%4<2?b:d);write(v,8,i<4?zero:extent);
 }
 for(u32 i=0;i<8;i++)call(0x028E8F64,at<u8>(matrix),at<u8>(local(0x44+i*12)),at<u8>(local(i<4?0x164+i*12:0x110+(i-4)*12)));
 f32 points[12];
 for(u32 i=0;i<4;i++){
  call(0x024F1AFC,at<u8>(mirror+0x33BC),at<u8>(local(0x164+i*12)),at<u8>(local(0x110+i*12)),0);
  const u32 play=call<u32>(0x025200D4);
  const bool hit=call<u32>(0x02008860,at<u8>(play+0x12A0),at<u8>(mirror+0x33BC))!=0;
  for(u32 j=0;j<3;j++){points[i*3+j]=read<f32>(hit?mirror+0x33EC:local(0x110+i*12),j*4);write(local(8+i*12),j*4,points[i*3+j]);}
 }
 u32 selector=read<u32>(mirror,0x2F88),packetBase=mirror+0x2AE0;
 const u32 allocation=read<u32>(packetBase+selector*0x254),end=allocation+0x260;
 if(allocation<end){
  for(u32 cursor=allocation;cursor<end;cursor+=32)for(u32 j=0;j<8;j++)write<u32>((cursor&~31u)+j*4,0,0);
  for(u32 i=0;i<12;i++)points[i]=read<f32>(local(8+i*4));selector=read<u32>(mirror,0x2F88);
 }
 const u32 output=read<u32>(packetBase+selector*0x254);
 const u32 offsets[]={0x1C8,0xA0,0x134,0,4,0x1CC,0x138,0x130,0x1D0,8,0x9C,0x98};
 const u32 indices[]={9,5,7,0,1,10,8,6,11,2,4,3};
 for(u32 i=0;i<12;i++)write(output,offsets[i],points[indices[i]]);
 const u32 selected=packetBase+read<u32>(packetBase,0x4A8)*0x254;
 call(0x027B5E94,at<u8>(selected+4),0,read<u32>(selected,0x150));
 write(packetBase,0x4A8,u32(read<u32>(packetBase,0x4A8)==0));
 const u32 renderer=0x104B45C0;
 for(u32 i=0;i<12;i++)write(local(0xB0),i*4,read<f32>(renderer,0x38+i*4));
 for(u32 i=0;i<16;i++)write(local(0x224),i*4,read<u32>(renderer,0x14C+i*4));
 call(0x027FDA54,at<u8>(mirror+0x2FA0),0,at<u8>(local(0xB0)),at<u8>(local(0x224)),at<u8>(read<u32>(renderer,0x148)+0x240));
 const f32 nearZ=read<f32>(read<u32>(call<u32>(0x025200D4),0x5FA4),0xCC);
 const f32 farZ=read<f32>(read<u32>(call<u32>(0x025200D4),0x5FA4),0xD0);
 const f32 one=read<f32>(0x1004CA30),half=read<f32>(0x1004CA44);
 write(mirror,0x3170,nearZ);write(mirror,0x316C,f32(one-f32(nearZ/farZ)));write(mirror,0x3174,farZ);write(mirror,0x3168,f32(nearZ/f32(farZ-nearZ)));
 const f32 fov=read<f32>(read<u32>(call<u32>(0x025200D4),0x5FA4),0xD4);
 const f64 tangent=call<f64>(0x028F423C,f32(f32(fov*half)*read<f32>(0x1004CA7C)));
 const f32 aspect=read<f32>(read<u32>(call<u32>(0x025200D4),0x5FA4),0xD8);
 write(mirror,0x3160,zero);write(mirror,0x315C,f32(tangent));write(mirror,0x3164,zero);write(mirror,0x3158,f32(tangent*f64(aspect)));
 if(!read<u32>(0x10475784)){write<u32>(0x10475784,0,1);write(0x1047576C,0,read<f32>(0x1004CA80));write(0x1047576C,8,zero);write(0x1047576C,4,read<f32>(0x1004CA84));}
 if(!read<u32>(0x10475788)){write<u32>(0x10475788,0,1);write(0x10475778,8,read<f32>(0x1004CA88));write(0x10475778,0,read<f32>(0x1004CA80));write(0x10475778,4,read<f32>(0x1004CA84));}
 if(!read<u32>(0x1047578C)){
  const f32 values[]={half,zero,half,zero,zero,read<f32>(0x1004CA8C),half,zero,zero,zero,one,zero};
  for(u32 i=0;i<12;i++)write(0x1047572C,i*4,values[i]);write<u32>(0x1047578C,0,1);
 }
 call(0x028E8F64,at<u8>(matrix),at<u8>(0x1047576C),at<u8>(local(0x14C)));
 call(0x028E8F64,at<u8>(matrix),at<u8>(0x10475778),at<u8>(local(0x158)));
 call(0x028E91EC,at<u8>(renderer+0x38),at<u8>(local(0x1F4)));
 call(0x025F1C90,at<u8>(local(0x1C4)),at<u8>(local(0x14C)),at<u8>(local(0x158)),0);
 call(0x028E9A70,at<u8>(local(0x264)),read<f32>(0x1004CA90),read<f32>(0x1004CA94),read<f32>(0x1004CA94),read<f32>(0x1004CA90),one,read<f32>(0x1004CA88));
 call(0x028E9108,at<u8>(local(0x1C4)),at<u8>(local(0x1F4)),at<u8>(mirror+0x108));
 call(0x025F1FC4,at<u8>(local(0x264)),at<u8>(mirror+0x108),at<u8>(mirror+0xC8));
 for(u32 i=0;i<4;i++)write(mirror,0xE8+i*4,read<f32>(mirror,0xF8+i*4));
 for(u32 i=0;i<12;i++)write(local(0xE0),i*4,read<f32>(renderer,0x14C+i*4));
 call(0x028E90D4,at<u8>(local(0xE0)),at<u8>(mirror+0x31D8));
 call(0x0252D9C4,at<u8>(local(0x194)),at<u8>(mirror+0xC8));
 call(0x028E9108,at<u8>(0x1047572C),at<u8>(local(0x194)),at<u8>(mirror+0x3208));
 call(0x028E8F64,at<u8>(renderer+0x38),at<u8>(local(0x14C)),at<u8>(local(0xA4)));
 call(0x028E8F64,at<u8>(renderer+0x38),at<u8>(local(0x158)),at<u8>(local(0x140)));
 for(u32 i=0;i<3;i++)write(mirror,0x3358+i*4,read<f32>(local(0xA4),i*4));write(mirror,0x3364,one);
 for(u32 i=0;i<3;i++)write(local(0x38),i*4,f32(read<f32>(local(0x140),i*4)-read<f32>(local(0xA4),i*4)));
 call(0x025155D8,at<u8>(local(0x38)));
 for(u32 i=0;i<3;i++)write(mirror,0x3368+i*4,read<f32>(local(0x38),i*4));write(mirror,0x3374,zero);
 call(0x027FE0DC,at<u8>(mirror+0x2FA0),0);
}
VERIFY(0x0252DA64,updateMirrorPacket);

void executeDepthPeek(u32 queue){
 WWHD_FUNC(0x0252E388,void,queue);
 Local<u8[0x78]> scratch;const u32 base=ea(scratch.get());auto local=[&](u32 o){return base+o-8;};
 const u32 manager=read<u32>(0x101F95D0),slot=read<u32>(manager,0x1024)+(read<u32>(manager,0x1020)>1?4:0);
 const u32 texture=read<u32>(slot)+0x6B3C;if(read<u32>(texture)!=58)return;
 const u32 width=s32(read<u32>(texture,8))>s32(read<u8>(texture,0x78))?read<u32>(texture,8):read<u8>(texture,0x78);
 const u32 height=s32(read<u32>(texture,12))>s32(read<u8>(texture,0x79))?read<u32>(texture,12):read<u8>(texture,0x79);
 const f32 xscale=f32(f32(width)/read<f32>(0x1004CAA0)),yscale=f32(f32(height)/read<f32>(0x1004CAA4));
 const u32 data=read<u32>(texture,0x28);
 if(!read<u8>(queue)){write(queue,0,u8(0));return;}
 const f32 one=read<f32>(0x1004CA30),zero=read<f32>(0x1004CA34),multiplier=read<f32>(0x1004CAB4);
 const f32 split=read<f32>(0x1004CAAC),xpacking=read<f32>(0x1004CAA8),ypacking=read<f32>(0x1004CAB0);
 for(u32 i=0;i<read<u8>(queue);i++){
  const u32 entry=queue+4+i*8;
  const u16 x=u16(ftoi(f32(f32(f32(read<s16>(entry))*xscale)*xpacking)));
  const u16 y=u16(ftoi(f32(f32(read<s16>(entry,2))*yscale)));
  const u32 bytes=call<u32>(0x027BFF40,58),row=u16(ftoi(f32(f32(y)*ypacking)));
  u32 pixel=data+(((width<<4)*row+(y&15)+x)*bytes),accumulated=0;
  for(u32 j=0;j<4;j++){
   if(call<s32>(0x027BFF54,58)<=s32(j)){write<u32>(local(0x10+j*4),0,0);continue;}
   const u32 channel=call<u32>(0x027BFF80,58,j),bits=call<u32>(0x027BFF68,58,channel);
   const u32 target=read<u32>(0x10147624),value=call_ptr<u32>(target,read<u32>(pixel));
   write(local(0x10+(channel<3?channel:3)*4),0,value);accumulated+=bits;
   if(accumulated==32){pixel+=4;accumulated=0;}
  }
  for(u32 j=0;j<4;j++){
   if(call<s32>(0x027BFF54,58)<=s32(j))write(local(0x24+j*4),0,call<s32>(0x027BFF9C,58,j)==5?one:zero);
   else if(call<u32>(0x027BFF68,58,j)==32){const u32 word=read<u32>(local(0x10+j*4));write(local(0x20),0,word);write(local(0x24+j*4),0,read<f32>(local(0x20)));}
  }
  const f32 value=f32(multiplier*read<f32>(local(0x24)));
  const u32 depth=value<split?u32(ftoi(value)):u32(ftoi(f32(value-split)))+0x80000000u;
  write(read<u32>(entry,4),0,depth);
  const s32 currentX=read<s16>(entry),currentY=read<s16>(entry,2);
  const u16 px=u16(ftoi(f32(f32(currentX)*xscale))),py=u16(ftoi(f32(f32(currentY)*yscale)));
  cpu->cr[6]=1;
  call(0x0252FA2C,at<u8>(local(0x34)),at<u8>(0x1004CAB8),i,read<u32>(read<u32>(entry,4)),u32(px),u32(py),0x43300000u,u32(currentX)^0x80000000u,
       read<f32>(local(0x24)),read<f32>(local(0x28)),read<f32>(local(0x2C)));
 }
 write(queue,0,u8(0));
}
VERIFY(0x0252E388,executeDepthPeek);
}
