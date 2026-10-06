#include "bindings.h"
// Qualified two-entry SDK subset, not the complete JKRDvdRipper translation unit.
namespace dvd_ripper_adapters {
using namespace gabi;
static u32 word(u32 a){return load<u32>(a);}
static void put(u32 a,u32 v){store<u32>(a,v);}
static s32 readDVD(u32 file,u32 dst,u32 size,u32 offset){return call<s32>(0x0284B6EC,file+0x7C,dst,size,offset,2u);}
static bool readRetry(u32 file,u32 dst,u32 size,u32 offset){
 s32 result=readDVD(file,dst,size,offset);
 while(result<0){if(result==-3||!load<u8>(0x101F9750))return false;call(0x0284B990);result=readDVD(file,dst,size,offset);}return true;
}
static bool streamRead(u32 dst,u32 size){
 s32 result=readDVD(word(0x104A23A0),dst,size,word(0x104A2394));
 while(result<0){if(result==-3||!load<u8>(0x101F9750))return false;call(0x0284B990);result=readDVD(word(0x104A23A0),dst,size,word(0x104A2394));}return true;
}
static u32 detect(u32 p){u32 kind=call<u32>(0x028428FC,p);return kind==3?0:kind;}
static u32 alloc(u32 heap,u32 size,s32 direction){return call<u32>(0x027EC0D8,heap,size,direction);}
static void freeMem(u32 p){call(0x027EC0B0,p);}
static void invalidate(u32 p,u32 size){call(0xC00088A8,p,size);}
u32 loadToMainRAM(u32 file,u32 dst,u32 expand,u32 length,u32 heap,u32 direction,u32 offset,u32 compressionOut){
 WWHD_FUNC(0x027EB5F4,u32,file,dst,expand,length,heap,direction,offset,compressionOut);
 // Ninth incoming integer argument is the decompressed-size output.
 u32 sizeOut=word(cpu->r[1]+8);
 u32 vtable=word(file+0xC);
 u32 aligned=(call_ptr<u32>(word(vtable+0x34),file)+31)&~31u;
 u32 compression=0,expandedSize=0,temporary=0;
 bool allocated=false;
 // The native 32-byte read target is 64-byte aligned within a local 64-byte buffer.
 Local<u8[128]> header;u32 headerAddress=(ea(header.get())+63)&~63u;
 if(expand==1){
  if(!readRetry(file,headerAddress,32,0))return 0;
  invalidate(headerAddress,32);compression=detect(headerAddress);
  expandedSize=(u32(load<u8>(headerAddress+4))<<24)|(u32(load<u8>(headerAddress+5))<<16)|(u32(load<u8>(headerAddress+6))<<8)|load<u8>(headerAddress+7);
 }
 if(compressionOut)put(compressionOut,compression);
 if(expand==1&&compression){
  if(length&&expandedSize>length)expandedSize=length;
  if(!dst){dst=alloc(heap,expandedSize,direction==1?64:-64);allocated=true;}
  if(!dst)return 0;
  if(compression==1){temporary=alloc(heap,aligned,64);if(!temporary&&allocated){freeMem(dst);return 0;}}
 }else{
  if(!dst){u32 size=aligned-offset;if(length&&size>length)size=length;dst=alloc(heap,size,direction==1?64:-64);allocated=true;}
  if(!dst)return 0;
 }
 if(!compression){
  u32 subCompression=0;
  if(offset){
   if(!readRetry(file,headerAddress,32,offset)){if(allocated)freeMem(dst);return 0;}
   invalidate(headerAddress,32);subCompression=detect(headerAddress);
  }
  if(!subCompression||expand==2||!expand){
   u32 size=aligned-offset;if(length&&length<size)size=length;
   if(!readRetry(file,dst,size,offset)){if(allocated)freeMem(dst);return 0;}
   if(sizeOut)put(sizeOut,size);return dst;
  }
  if(subCompression==2){call(0x027EAE70,file,dst,aligned,length,0u,offset,sizeOut);return dst;}
  call(0x0273AA24,0x1016DBDCu,0x15Du,0x1016DBB8u);return dst;
 }
 if(compression==1){
  if(offset)call(0x0273AA24,0x1016DBDCu,0x167u,0x1016DBF0u);
  if(!readRetry(file,temporary,aligned,0)){if(allocated)freeMem(dst);freeMem(temporary);return 0;}
  invalidate(temporary,aligned);call(0x02842B38,temporary,dst,expandedSize,offset);freeMem(temporary);
  if(sizeOut)put(sizeOut,expandedSize);return dst;
 }
 if(compression==2){if(call<u32>(0x027EAE70,file,dst,aligned,expandedSize,offset,0u,sizeOut)){if(allocated)freeMem(dst);dst=0;}return dst;}
 if(allocated)freeMem(dst);return 0;
}
VERIFY(0x027EB5F4,loadToMainRAM);
static s32 decompress(u32 file,u32 dst,u32 fileSize,u32 maxDestination,u32 fileOffset,u32 sourceOffset,u32 sizeOut){
 WWHD_FUNC(0x027EAE70,s32,file,dst,fileSize,maxDestination,fileOffset,sourceOffset,sizeOut);
 u32 interruptState=call<u32>(0xC0009A60);
 if(!load<u8>(0x101F9730)){call(0xC0009D68,0x104A23C8u);store<u8>(0x101F9730,1);}
 call(0xC0009F30,interruptState);call(0xC0009E40,0x104A23C8u);
 u32 bufferSize=word(0x101F974C);
 u32 buffer=call<u32>(0x027EC0B4,bufferSize,-32);put(0x104A2380,buffer);
 if(!buffer){call(0x0273AA24,0x1016DBA4u,0x3ABu,0x1016DB8Cu);buffer=word(0x104A2380);}
 put(0x104A2384,buffer+bufferSize);
 if(fileOffset){
  u32 ref=call<u32>(0x027EC0B4,0x1120u,-4);put(0x104A2388,ref);
  if(!ref){call(0x0273AA24,0x1016DBA4u,0x3B4u,0x1016DB98u);ref=word(0x104A2388);}
  put(0x104A23A0,file);put(0x104A2390,ref);put(0x104A23AC,maxDestination);put(0x104A238C,ref+0x1120);
  put(0x104A23A8,0);put(0x104A23A4,fileOffset);put(0x104A2398,fileSize-sourceOffset);put(0x104A2394,sourceOffset);
 }else{
  put(0x104A2388,0);put(0x104A23A0,file);put(0x104A23AC,maxDestination);put(0x104A23A8,0);put(0x104A23A4,fileOffset);
  put(0x104A2398,fileSize-sourceOffset);put(0x104A2394,sourceOffset);
 }
 if(!sizeOut)sizeOut=0x104A23B4;put(0x104A23B0,sizeOut);put(sizeOut,0);
 s32 status=-1;
 u32 input=word(0x104A2380),endBuffer=word(0x104A2384);
 put(0x104A239C,endBuffer-25);
 u32 initialSize=endBuffer-input;if(initialSize>word(0x104A2398))initialSize=word(0x104A2398);
 if(!streamRead(input,initialSize))goto cleanup;
 invalidate(input,initialSize);
 {u32 next=word(0x104A2394)+initialSize;u32 left=word(0x104A2398)-initialSize;put(0x104A2394,next);put(0x104A2398,left);}
 if(!input)goto cleanup;
 if(load<u8>(input)!='Y'||load<u8>(input+1)!='a'||load<u8>(input+2)!='z'||load<u8>(input+3)!='0')goto cleanup;
 {
 u32 out=dst,end=dst+(word(input+4)-word(0x104A23A4));u32 limit=dst+word(0x104A23AC);if(end>limit)end=limit;
 u32 written=0,code=0,bits=0;input+=16;
 // Native do-loop consumes at least one symbol, even when the requested output limit is zero.
 for(;;){
  if(!bits){
   if(input>word(0x104A239C)&&word(0x104A2398)){
    u32 remainder=word(0x104A2384)-input;
    u32 relocated=word(0x104A2380);if(remainder&31)relocated+=32-(remainder&31);
    call(0xC000A848,relocated,input,remainder);
    u32 nextRead=relocated+remainder;u32 count=word(0x104A2384)-nextRead;if(count>word(0x104A2398))count=word(0x104A2398);
    if(!count)call(0x0273AA24,0x1016DB78u,0x4D6u,0x1016DB68u);
    if(!streamRead(nextRead,count)){
     // Native refill-failure cleanup explicitly supplies r4=0 to the first free call.
     call(0x027EC0B0,word(0x104A2380),0u);u32 ref=word(0x104A2388);if(ref)freeMem(ref);
     call(0xC00088B8,dst,word(word(0x104A23B0)));call(0xC000A1C8,0x104A23C8u);return -1;
    }
    invalidate(nextRead,count);u32 next=word(0x104A2394)+count;u32 left=word(0x104A2398)-count;
    put(0x104A2394,next);put(0x104A2398,left);if(!left)put(0x104A239C,nextRead+count);
    input=relocated;if(!input)goto cleanup;
   }
   code=load<u8>(input++);bits=8;
  }
  u32 skip=word(0x104A23A4);
  if(code&0x80){
   if(skip){
    u32 consumed=word(0x104A23A8);
    if(consumed>=skip){store<u8>(out++,load<u8>(input));++written;if(out==end)break;}
    u32 cursor=word(0x104A2390);store<u8>(cursor,load<u8>(input));cursor=word(0x104A2390)+1;
    if(cursor==word(0x104A238C))cursor=word(0x104A2388);
    u32 consumedNext=word(0x104A23A8)+1;++input;put(0x104A2390,cursor);put(0x104A23A8,consumedNext);
   }else{
    store<u8>(out++,load<u8>(input++));++written;if(out==end)break;
    put(0x104A23A8,word(0x104A23A8)+1);
   }
  }else{
   u32 first=load<u8>(input),second=load<u8>(input+1);input+=2;
   u32 distance=((first&15)<<8)|second;u32 count=first>>4;u32 copy;
   if(skip){copy=word(0x104A2390)-distance-1;u32 start=word(0x104A2388);if(copy<start)copy+=word(0x104A238C)-start;}
   else copy=out-distance-1;
   if(!count)count=load<u8>(input++)+18;else count+=2;
   for(u32 i=0;i<count;++i){
    if(skip){
     if(word(0x104A23A8)>=word(0x104A23A4)){store<u8>(out++,load<u8>(copy));++written;if(out==end)break;}
     u32 cursor=word(0x104A2390);store<u8>(cursor,load<u8>(copy));cursor=word(0x104A2390)+1;
     u32 refEnd=word(0x104A238C);if(cursor==refEnd)cursor=word(0x104A2388);
     ++copy;put(0x104A2390,cursor);if(copy==refEnd)copy=word(0x104A2388);
     put(0x104A23A8,word(0x104A23A8)+1);
    }else{
     store<u8>(out++,load<u8>(copy));++written;if(out==end)break;
     put(0x104A23A8,word(0x104A23A8)+1);++copy;
    }
   }
  }
  --bits;code<<=1;if(out>=end)break;
 }
 put(word(0x104A23B0),written);status=0;
 }
cleanup:
 freeMem(word(0x104A2380));{u32 ref=word(0x104A2388);if(ref)freeMem(ref);}
 call(0xC00088B8,dst,word(word(0x104A23B0)));call(0xC000A1C8,0x104A23C8u);return status;
}
VERIFY(0x027EAE70,decompress);
}
