// HD Hookshot renderer.
#include "bindings.h"
namespace {
struct CallLinkage {u8 bytes[32];};
template<class R=void,class... A> R hookCall(u32 target,A... args) {
    gabi::Local<CallLinkage> linkage;
    return gabi::call<R>(target,args...);
}
u32 word(u32 p,u32 off=0) {return gabi::load<u32>(p+off);}
void put(u32 p,u32 off,u32 value) {gabi::store<u32>(p+off,value);}
void virtualCall(u32 receiver,u32 slot,u32 argument) {
    u32 target=word(word(receiver,0xC),slot);
    hookCall(target,receiver,argument);
}
void releaseAllocation(u32 p) {
    u32 allocation=word(p,4);
    u32 heap=hookCall<u32>(0x02755FEC,word(0x101F8B4C),allocation);
    virtualCall(heap,0x3C,word(p,4));
    put(p,0,0);put(p,4,0);
}
void clearMaterialBuffers(u32 buffer) {
    for(u32 offset: {8u,16u}) {
        u32 data=word(buffer,offset);
        if(data) {
            for(s32 i=0;i<(s32)word(buffer,offset-4);++i) {
                u32 element=data+u32(i)*0xF4;
                hookCall(word(word(element,0xF0),0xC),element,2);
                data=word(buffer,offset);
            }
            u32 heap=hookCall<u32>(0x02755FEC,word(0x101F8B4C),data);
            virtualCall(heap,0x3C,word(buffer,offset));
            put(buffer,offset-4,0);put(buffer,offset,0);
        }
    }
}
void resetShapeBuffers(u32 buffers) {
    hookCall(0x027BF7E8,buffers+0x158);
    u32 allocation=word(buffers,0x250);put(buffers,0,0);
    if(allocation)releaseAllocation(buffers+0x24C);
    hookCall(0x027BF7E8,buffers+0x3AC);
    allocation=word(buffers,0x4A4);put(buffers,0x254,0);
    if(allocation)releaseAllocation(buffers+0x4A0);
    put(buffers,0x4B8,0);
}
}

static void daHookshot_shape_dt(u32 shape,s32 flags) {
    WWHD_FUNC(0x02177FD4,void,shape,flags);
    if(!shape)return;
    put(shape,0xC,0x1001175C);
    u32 buffers=shape+0xA4;
    resetShapeBuffers(buffers);
    for(s32 i=0;i<(s32)word(shape,0x564);++i) {
        u32 material=word(shape,0x568);
        if(u32(i)<word(shape,0x564))material+=u32(i)*0x23C;
        for(u32 j=0;j<2;++j)hookCall(0x027BEBEC,material+0x10+j*0x1C);
    }
    for(u32 start: {0x580u,0x628u})
        for(u32 j=0;j<2;++j)hookCall(0x027BEBEC,shape+start+j*0x1C);
    for(u32 i=0;i<300;++i)
        for(u32 j=0;j<2;++j)hookCall(0x027BEBEC,shape+0x990+i*0xA8+j*0x1C);
    hookCall(0x027BE2B0,shape+0xCE78,2);
    hookCall(0x027B54A0,shape+0xCE60,2);
    hookCall(0x028F0164,shape+0x980,300,0xA8,0x02179F34,0,0);
    hookCall(0x027FB528,shape+0x618,0);
    hookCall(0x027FB528,shape+0x570,0);
    hookCall(0x027FD764,shape+0x564,2);
    if(buffers) {
        resetShapeBuffers(buffers);
        hookCall(0x028F0164,buffers,2,0x254,0x02179F88,0,0);
    }
    u32 materials=word(shape,0xA0);
    if(materials) {
        for(s32 i=0;i<(s32)word(shape,0x9C);++i) {
            u32 buffer=materials+u32(i)*0x14;
            if(buffer) {
                put(buffer,0,0);
                clearMaterialBuffers(buffer);
                materials=word(shape,0xA0);
            }
        }
        u32 heap=hookCall<u32>(0x02755FEC,word(0x101F8B4C),materials);
        virtualCall(heap,0x3C,word(shape,0xA0));
        put(shape,0x9C,0);put(shape,0xA0,0);
    }
    hookCall(0x027F13DC,shape,0);
    if(flags&1)hookCall(0x0273AF40,shape);
}
VERIFY(0x02177FD4,daHookshot_shape_dt);

namespace {
struct RenderState {u8 bytes[0x11C];};
void bindMaterialBuffer(u32 material,u32 source) {
    u32 buffer=source+0x10+word(source,0x4C)*0x1C;
    u32 parameters=word(material,0xC)?word(material,0x10):0;
    s16 vertex=gabi::load<s16>(parameters+0xC);
    u32 size=word(buffer,4),data=word(buffer,0xC);
    s16 pixel=gabi::load<s16>(parameters+0xE);
    s16 geometry=gabi::load<s16>(parameters+0x10);
    if(pixel!=-1)hookCall(0xC0006900,pixel,data,size);
    if(vertex!=-1)hookCall(0xC0006A38,vertex,data,size);
    if(geometry!=-1)hookCall(0xC00068A8,geometry,data,size);
}
void bindShapeTexture(u32 shape,u32 material) {
    u32 parameters=word(material,0x14)?word(material,0x18):0;
    hookCall(0x027BE53C,shape+0xCE78,parameters+4,-1,0);
}
}
static void daHookshot_shape_drawGX2(u32 shape,u32 context) {
    WWHD_FUNC(0x021783F4,void,shape,context);
    s32 links=(s32)word(word(shape,0x18),0xD504);
    if((s32)word(0x101B76F8)<links)put(0x101B76F8,0,links);
    if(links<=0)return;
    s32 index=(s32)word(context,0xC);
    u32 material=0;
    if(index<4) {
        u32 entry=word(shape,0xA0);
        if(u32(index)<word(shape,0x9C))entry+=u32(index)*0x14;
        material=word(entry);
    }
    u32 current=hookCall<u32>(0x027F29D4,0x104B45C0);
    u32 shader=word(material);
    if(shader!=word(current,4)) {
        u8 flags=gabi::load<u8>(shader);
        u32 previous=word(current);
        if(flags&2) {
            gabi::store<u8>(shader,flags&~2u);
            hookCall(0x027BB9E0,shader,0);
        }
        u32 program=word(word(shader,0x7C),0x28);
        if(previous!=program)hookCall(0x027B9F68,program);
        u32 displaySize=word(shader,0xC);
        if(displaySize)hookCall(0xC00060E0,word(shader,4),displaySize);
        else hookCall(0x027BB7CC,shader);
        put(current,0,program);put(current,4,shader);
    }
    u32 mode=word(context,0xC);
    if(mode==0) {
        u32 resource=word(context,0x14);
        if(resource)bindMaterialBuffer(material,word(resource,4));
    } else if(mode==1) {
        bindMaterialBuffer(material,word(shape,0x568));
        virtualCall(shape+0x618,0x2C,material);
        bindShapeTexture(shape,material);
    } else if(mode==2) {
        u32 source=word(context,4)==2?word(word(context,0x14),4):word(shape,0x568);
        bindMaterialBuffer(material,source);
        virtualCall(shape+0x618,0x2C,material);
        u32 custom=word(context,0x30);
        if(custom)virtualCall(custom,0x2C,material);
        bindShapeTexture(shape,material);
        hookCall(0x027FFE54,context,material);
    }
    gabi::Local<RenderState> state;
    hookCall(0x02750250,state.get());
    u32 stateEA=gabi::ea(state.get());
    u32 flags=word(stateEA,0xEC);
    gabi::store<u8>(stateEA+0xE0,1);
    put(stateEA,0xEC,(flags&0xFFFFFF00u)+0x17);
    u32 selected=word(context,0xC),pass=word(context,4);
    put(stateEA,0xC,pass==2?3:2);
    hookCall(0x0280037C,selected,state.get());
    hookCall(0x02750370,state.get());
    if(links<=0)links=1;
    u32 link=shape+0x980;
    for(s32 remaining=links;remaining;--remaining,link+=0xA8) {
        virtualCall(link,0x2C,material);
        u32 entry=word(shape,0xA0),selected=word(context,0xC);
        if(selected<word(shape,0x9C))entry+=selected*0x14;
        u32 alternate=word(shape,0x54C)==0?8:0;
        hookCall(0x027BFE5C,word(entry+alternate,8));
        u32 primitive=shape+0xCE60;
        u32 count=word(primitive,0xC);
        if(count)hookCall(0xC0006178,word(primitive,4),count,word(primitive),word(primitive,8),0,1);
    }
}
VERIFY(0x021783F4,daHookshot_shape_drawGX2);

namespace {
u32 allocateGpu(u32 bytes,u32 alignment) {
    u32 heap=hookCall<u32>(0x02756140,word(0x101F8B4C));
    return hookCall<u32>(word(word(heap,0xC),0x34),heap,bytes,alignment);
}
void constructMaterialBuffers(u32 entry,u32 shader,u32 buffers) {
    for(u32 offset: {8u,16u}) {
        u32 allocation=allocateGpu(0xF4,4);
        if(allocation)hookCall(0x027BF734,allocation);
        if(allocation){put(entry,offset,allocation);put(entry,offset-4,1);}
    }
    for(u32 i=0;i<2;++i) {
        u32 part=entry+4+i*8;
        u32 count=word(part),allocation=word(part,4);
        hookCall(0x027FF530,shader,allocation,buffers+4+i*0x254,buffers+0x4AC,0);
        (void)count; // The original tests this count but both tails perform the same call.
    }
}
f32 copiedRendererFloat(u32 bits) {f32 value;memcpy(&value,&bits,4);return value;}
void copyRendererFloats(u32 dst,u32 src,u32 count) {
    for(u32 i=0;i<count;++i)gabi::store<f32>(dst+4*i,copiedRendererFloat(word(src,4*i)));
}
}
static u32 daHookshot_shape_ct(u32 shape) {
    WWHD_FUNC(0x0217718C,u32,shape);
    if(!shape){shape=hookCall<u32>(0x0273AD10,0xD0A0);if(!shape)return 0;}
    hookCall(0x027F1278,shape);
    put(shape,0x98,0);put(shape,0xC,0x1001175C);
    u32 header=shape+0x9C;
    if(!header)header=hookCall<u32>(0x0273AD10,8);
    if(header){put(header,4,0);put(header,0,0);}
    u32 buffers=shape+0xA4,storage=buffers;
    if(!storage)storage=hookCall<u32>(0x0273AD10,0x4C0);
    if(storage) {
        hookCall(0x028EFFD0,storage,2,0x254,0x02179E3C);
        put(storage,0x4A8,0);put(storage,0x4AC,0);put(storage,0x4B8,0);
        put(storage,0x4B0,0x20);gabi::store<u8>(storage+0x4BC,0);
        put(storage,0,0);put(storage,0x254,0);
    }
    hookCall(0x027FD6F4,shape+0x564);
    hookCall(0x027FB40C,shape+0x570);put(shape,0x57C,0x1016EF84);
    hookCall(0x028F521C,shape+0x5E4,0x34);
    if(shape+0x5E4==0)hookCall<u32>(0x0273AD10,0x30);
    hookCall(0x027FB40C,shape+0x618);put(shape,0x624,0x1016EFB4);
    hookCall(0x028F521C,shape+0x68C,0x2F0);
    struct InitialFloat {u32 offset;f32 value;};
    static const InitialFloat initial[] = {
{0x68c,0.0f},{0x690,0.0f},{0x694,0.0f},{0x69c,0.0f},{0x6b8,1.0f},{0x6c4,0.0f},{0x6a8,1.0f},{0x6a4,0.0f},{0x6f4,0.0f},{0x714,0.0f},{0x6e4,0.0f},{0x708,1.0f},{0x6bc,0.0f},{0x6e0,0.0f},{0x710,0.0f},{0x700,0.0f},{0x6d0,0.0f},{0x6ac,0.0f},{0x6c0,0.0f},{0x6c8,1.0f},{0x6b0,0.0f},{0x6d4,0.0f},{0x6a0,0.0f},{0x6fc,0.0f},{0x704,0.0f},{0x718,1.0f},{0x6e8,1.0f},{0x6f8,1.0f},{0x6f0,0.0f},{0x6dc,0.0f},{0x70c,0.0f},{0x6b4,0.0f},{0x6ec,0.0f},{0x698,1.0f},{0x6cc,0.0f},{0x6d8,1.0f},{0x71c,0.0f},{0x720,0.0f},{0x724,0.0f},{0x728,1.0f},{0x72c,0.0f},{0x730,0.0f},{0x734,0.0f},{0x738,1.0f}
    };
    for(const auto& item: initial)gabi::store<f32>(shape+item.offset,item.value);
    for(u32 offset: {0x73Cu,0x75Cu,0x77Cu})hookCall(0x028EFFD0,shape+offset,2,0x10,0x02179E98);
    for(u32 offset=0x79C;offset<=0x8EC;offset+=0x30)
        if(shape+offset==0)hookCall<u32>(0x0273AD10,0x30);
    for(u32 offset=0x91C;offset<=0x96C;offset+=0x10)
        if(shape+offset==0)hookCall<u32>(0x0273AD10,0x10);
    gabi::store<u8>(shape+0x97C,0);
    hookCall(0x028EFFD0,shape+0x980,300,0xA8,0x02179EC4);
    u32 primitive=shape+0xCE60,texture=shape+0xCE78,resourceTexture=shape+0xD010;
    hookCall(0x027B5430,primitive);hookCall(0x027BDF7C,texture);hookCall(0x027BE6B8,resourceTexture);
    gabi::Local<SafeString> materialName;
    materialName->mStringTop=0x100116D8;materialName->__vtbl=0x10011560;
    u32 resources=hookCall<u32>(0x027FFCBC);
    s32 index=hookCall<s32>(0x027B90AC,word(resources,4),materialName.get());
    u32 descriptor=0;
    if(index>=0) {
        u32 count=word(resources,8),data=word(resources,0xC);
        u32 candidate=data+(u32(index)<count?u32(index)*0x24:0);
        if(!gabi::load<u8>(candidate+0x20)) {
            u32 package=word(resources,4);
            u32 shader=u32(index)<word(package,0x1C)?word(package,0x20)+u32(index)*0x84:0;
            hookCall(0x02800B0C,candidate,shader,0);
            count=word(resources,8);data=word(resources,0xC);
        }
        descriptor=data+(u32(index)<count?u32(index)*0x24:0);
    }
    hookCall(0x0280068C,shape+0x98,descriptor,0);
    put(buffers,0x4AC,0x13);put(buffers,0x4B4,0x10011750);
    for(u32 i=0;i<2;++i) {
        u32 buffer=buffers+i*0x254;
        u32 data=word(buffer);
        if(!data) {
            u32 allocation=allocateGpu(0x480,0x40);
            if(allocation){put(buffer,0x250,allocation);put(buffer,0x24C,0x24);}
            data=word(buffer,0x250);put(buffer,0,data);
        }
        hookCall(0x027FF478,buffer+4,data,0x24,buffers+0x4AC);
    }
    put(buffers,0x4B8,0);gabi::store<u8>(buffers+0x4BC,1);
    for(u32 i=0;i<word(shape,0x98);++i) {
        u32 entry=word(shape,0xA0);
        if(i<word(shape,0x9C))entry+=i*0x14;
        u32 shader=word(entry);put(entry,0,0);clearMaterialBuffers(entry);put(entry,0,shader);
        constructMaterialBuffers(entry,shader,buffers);
    }
    hookCall(0x027FE084,shape+0x564,1,0);
    for(u32 i=0;i<300;++i)hookCall(0x027FB5D4,shape+0x980+i*0xA8,0);
    hookCall(0x027B54E0,primitive,0x101B761C,4,word(0x101B7178));put(primitive,4,4);
    u32 active=word(buffers,0x4A8),buffer=buffers+active*0x254;
    u32 data=word(buffer),end=data+0x480;
    if(data<end) {
        for(u32 p=data;p<end;p+=0x20)
            for(u32 j=0;j<0x20;j+=4)put(p&~31u,j,0);
        active=word(buffers,0x4A8);buffer=buffers+active*0x254;
    }
    u32 vertices=word(0x101B7174),dst=word(buffer);
    for(u32 i=0;i<vertices;++i) {
        copyRendererFloats(dst+i*0x20,0x101B719C+i*12,3);
        copyRendererFloats(dst+i*0x20+12,0x101B734C+i*12,3);
        copyRendererFloats(dst+i*0x20+24,0x101B74FC+i*8,2);
    }
    if(vertices) {active=word(buffers,0x4A8);buffer=buffers+active*0x254;}
    u32 alternate=buffers+(active==0?0x254:0);
    for(u32 i=0;i<36;++i) {
        u32 src=word(buffer),target=word(alternate);
        copyRendererFloats(target+i*32,src+i*32,8);
    }
    active=word(buffers,0x4A8);buffer=buffers+active*0x254;
    hookCall(0x027B5E94,buffer+4,0,word(buffer,0x150));
    put(buffers,0x4A8,word(buffers,0x4A8)==0?1:0);
    hookCall(0x0274FBF8,word(0x101F8B18));
    gabi::Local<SafeString> archiveName,textureName;
    archiveName->mStringTop=0x100116E8;archiveName->__vtbl=0x10011560;
    textureName->mStringTop=0x100116F8;textureName->__vtbl=0x10011560;
    u32 textureData=hookCall<u32>(0x026124B0,word(0x101F4F7C),archiveName.get(),textureName.get(),0);
    hookCall(0x02773870,resourceTexture,textureData,0x100116CC);
    hookCall(0x0274FCCC,word(0x101F8B18));
    bool equal=true;
    for(u32 off: {4u,8u,12u,16u,20u,24u,56u,52u,28u})
        if(word(texture,off)!=word(resourceTexture,off)){equal=false;break;}
    if(!equal) {
        hookCall(0x027BDEB4,texture,resourceTexture);
        u8 flags=gabi::load<u8>(texture+0x190);
        put(texture,0x160,1);put(texture,0x15C,1);put(texture,0x164,1);
        gabi::store<u8>(texture+0x190,flags|2);
    } else {
        u8 flags=gabi::load<u8>(texture+0x190);
        u32 a=word(resourceTexture,0x28),b=word(resourceTexture,0x30);
        put(texture,0x28,a);gabi::store<u8>(texture+0x190,flags|2);
        put(texture,0xDC,b);put(texture,0x30,b);put(texture,0x164,1);
        put(texture,0xD4,a);put(texture,0x160,1);put(texture,0x15C,1);
    }
    return shape;
}
VERIFY(0x0217718C,daHookshot_shape_ct);
