/**
 * d_a_pirate_flag.cpp (WWHD)
 * Pirate ship flag: HD cloth simulation and GX2 packet support.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_pirate_flag.cpp) to the WWHD layout, with the HD-only GX2
 * packet/material code written from the WWHD code, and verified against cking.rpx.
 */
#include "d/actor/d_a_pirate_flag.h"
#include <cmath>
namespace {
template<class T> T load(u32 a) {return *gabi::at<be<T>>(a);}
template<class T> void store(u32 a,T v) {*gabi::at<be<T>>(a)=v;}
void* ptr(u32 a) {return gabi::at<void>(a);}
}
void pirate_copy_matrix(void* dest,void* source) {
    WWHD_FUNC(0x023D0C50,void,dest,source);
    f32 matrix[12];
    for (u32 i=0;i<12;++i) matrix[i]=load<f32>(gabi::ea(source)+i*4);
    for (u32 i=0;i<12;++i) store<f32>(gabi::ea(dest)+i*4,matrix[i]);
}
VERIFY(0x023D0C50,pirate_copy_matrix);
void pirate_color_s16(void* dest,void* source) {
    WWHD_FUNC(0x023D0CF0,void,dest,source);
    f32 color[4];
    for(u32 i=0;i<4;++i) color[i]=f32(load<s16>(gabi::ea(source)+i*2))/255.0f;
    for(u32 i=0;i<4;++i) store<f32>(gabi::ea(dest)+i*4,color[i]);
}
VERIFY(0x023D0CF0,pirate_color_s16);
void pirate_color_u8(void* dest,void* source) {
    WWHD_FUNC(0x023D0DB4,void,dest,source);
    f32 color[4];
    for(u32 i=0;i<4;++i) color[i]=f32(load<u8>(gabi::ea(source)+i))/255.0f;
    for(u32 i=0;i<4;++i) store<f32>(gabi::ea(dest)+i*4,color[i]);
}
VERIFY(0x023D0DB4,pirate_color_u8);
void pirate_set_correct_normal(daPirate_Flag_packet_c* packet,s16 windAngle,f32 windStrength) {
    WWHD_FUNC(0x023D1804,void,packet,windAngle,windStrength);
    f32 random=gabi::call<f32>(0x020198D8,200.0f);
    s16 wave=s16(u32(s32(packet->mWaveAngle))+u32(gabi::ftoi(random))+900u);
    packet->mWaveAngle=wave;
    f32 sine=load<f32>(0x104A44F8+u32(u16(wave)>>3)*8);
    packet->mNormalAngle=s16(gabi::ftoi(300.0f*sine));
    f32 attenuation=gabi::fnmsubs(0.5f,windStrength,1.0f);
    s16 escape=s16(gabi::ftoi(load<f32>(0x1046CC60)*attenuation));
    s16 opposite=s16(u16(windAngle)+0x8000u);
    s16 threshold=s16(gabi::ftoi((f32(escape)*1.25f)*182.04444885253906f));
    s16 target=0;
    if(std::abs(s32(opposite))<threshold) {
        s32 degree=opposite>0?-s32(escape):s32(escape);
        target=s16(gabi::ftoi(f32(degree)*182.04444885253906f));
    } else if(std::abs(s32(windAngle))<threshold) {
        s32 degree=windAngle>0?-s32(escape):s32(escape);
        target=s16(gabi::ftoi(f32(degree)*182.04444885253906f));
    }
    gabi::call(0x0200F428,&packet->mEscapeAngle,target,5,0xC0);
    s16 normal=packet->mNormalAngle;
    packet->mNormalAngle=s16(u16(normal)+u16(s16(packet->mEscapeAngle)));
}
VERIFY(0x023D1804,pirate_set_correct_normal);
void pirate_set_back_normals(daPirate_Flag_packet_c* packet) {
    WWHD_FUNC(0x023D1E08,void,packet);
    u32 buffer=packet->mBuffer;
    u32 back=gabi::ea(packet)+0x57C+buffer*0x12C;
    u32 normal=gabi::ea(packet)+0x324+buffer*0x12C;
    for(u32 i=0;i<25;++i) {
        store<f32>(back,0.0f);store<f32>(back+4,0.0f);store<f32>(back+8,0.0f);
        gabi::call(0x028E8DAC,ptr(back),ptr(normal),ptr(back));
        back+=12;normal+=12;
    }
}
VERIFY(0x023D1E08,pirate_set_back_normals);
BOOL pirate_is_delete(void* actor) {WWHD_FUNC(0x023D2718,BOOL,actor);return 1;}
VERIFY(0x023D2718,pirate_is_delete);
BOOL pirate_delete(void* actor) {
    WWHD_FUNC(0x023D2720,BOOL,actor);
    u32 a=gabi::ea(actor);
    gabi::call(0x025204C8,ptr(a+0x3AC),ptr(0x10034700));
    gabi::call(0x025204C8,ptr(a+0x3B4),ptr(0x10034708));return 1;
}
VERIFY(0x023D2720,pirate_delete);
void pirate_empty_destructor(void* object,u32 flags) {
    WWHD_FUNC(0x023D3BA4,void,object,flags);
    if(object&&(flags&1)) gabi::call(0x0273AF40,object);
}
VERIFY(0x023D3BA4,pirate_empty_destructor);
void* pirate_vertex_buffer_construct(void* object) {
    WWHD_FUNC(0x023D3BB8,void*,object);
    if(!object)object=gabi::call<void*>(0x0273AD10,0x254);
    if(object) {
        u32 a=gabi::ea(object);
        gabi::call(0x027B5BD8,ptr(a+4));gabi::call(0x027BF734,ptr(a+0x158));
        store<u32>(a+0x250,0);store<u32>(a+0x24C,0);
    }
    return object;
}
VERIFY(0x023D3BB8,pirate_vertex_buffer_construct);
void* pirate_material_block_allocate(void* object) {
    WWHD_FUNC(0x023D3C14,void*,object);
    return object?object:gabi::call<void*>(0x0273AD10,0x10);
}
VERIFY(0x023D3C14,pirate_material_block_allocate);
void pirate_vertex_buffer_destruct(void* object,u32 flags) {
    WWHD_FUNC(0x023D3C40,void,object,flags);
    if(object) {
        u32 a=gabi::ea(object);
        gabi::call(0x027BF880,ptr(a+0x158),2);gabi::call(0x027B5CBC,ptr(a+4),2);
        if(flags&1)gabi::call(0x0273AF40,object);
    }
}
VERIFY(0x023D3C40,pirate_vertex_buffer_destruct);
void pirate_empty_material_destructor(void* object) {WWHD_FUNC(0x023D40C8,void,object);}
VERIFY(0x023D40C8,pirate_empty_material_destructor);
void pirate_hio_destruct(void* object,u32 flags) {
    WWHD_FUNC(0x023D44FC,void,object,flags);
    if(object) {
        store<s8>(gabi::ea(object),-1);store<u32>(gabi::ea(object)+0x1C,0x10034648);
        if(flags&1)gabi::call(0x0273AF40,object);
    }
}
VERIFY(0x023D44FC,pirate_hio_destruct);
void pirate_hio_empty(void* object) {WWHD_FUNC(0x023D4524,void,object);}
VERIFY(0x023D4524,pirate_hio_empty);
BOOL pirate_execute(void* actor) {
    WWHD_FUNC(0x023D2610,BOOL,actor);
    u32 a=gabi::ea(actor);
    if(load<u8>(0x1046CC4B))return 1;
    u32 ship=load<u32>(0x1046CC38);
    if(load<u32>(ship+0x2E4)&4)return 1;
    if(!load<u32>(0x1046CC84)) {
        store<u32>(0x1046CC84,1);
        store<f32>(0x1046CC80,100.0f);store<f32>(0x1046CC78,0.0f);store<f32>(0x1046CC7C,1000.0f);
    }
    u32 model=load<u32>(ship+0x3E8);
    gabi::call(0x028E8F64,ptr(model?model+0xC8:0),ptr(0x1046CC78),ptr(a+0x314));
    ship=load<u32>(0x1046CC38);
    store<u16>(a+0x320,load<u16>(ship+0x328));
    store<u16>(a+0x322,load<u16>(ship+0x32A));
    store<u16>(a+0x324,load<u16>(ship+0x32C));
    gabi::call(0x025200D4);
    u32 wind=gabi::call<u32>(0x0257DAA8);
    f32 z=load<f32>(wind+8),x=load<f32>(wind);
    s16 angle=gabi::call<s16>(0x020195B0,x,z);
    s16 target=s16(u16(angle)-u16(load<s16>(a+0x322)));
    gabi::call(0x0200F428,ptr(a+0x32A),target,8,0x400);
    gabi::call(0x023D1EA4,actor);
    return 1;
}
VERIFY(0x023D2610,pirate_execute);
// Linkage area is reserved below temporaries for real guest callees' LR save.
namespace {
struct ClothTemporary {u8 linkage[16];cXyz displacement,unit;};
struct ParentIDTemporary {u8 linkage[16];be<u32> id;};
}
void pirate_cloth_spring(cXyz* origin,cXyz* neighbor,cXyz* accumulated,f32 restLength) {
    WWHD_FUNC(0x023D176C,void,origin,neighbor,accumulated,restLength);
    gabi::Local<ClothTemporary> local;
    auto* displacement=&local->displacement;
    auto* unit=&local->unit;
    gabi::call(0x0201ADE0,neighbor,displacement,origin);
    gabi::call(0x0201B12C,displacement,unit);
    f32 lengthSquared=gabi::call<f32>(0x028E8DD0,displacement);
    f32 length=gabi::call<f32>(0x028F4384,lengthSquared);
    f32 spring=(length-restLength)*load<f32>(0x1046CC5C);
    gabi::call(0x028E8E64,unit,unit,spring);
    gabi::call(0x028E8D88,accumulated,unit,accumulated);
}
VERIFY(0x023D176C,pirate_cloth_spring);
s32 pirate_create(void* actor) {
    WWHD_FUNC(0x023D33D4,s32,actor);
    u32 a=gabi::ea(actor);
    gabi::call(0x025200D4);
    u32 flags=load<u32>(a+0x2E4);
    if(!(flags&8)) {
        if(actor) {
            gabi::call(0x025D4ED0,actor);
            store<u32>(a+0xB4,0x10034658);
            gabi::call(0x023D3008,ptr(a+0x3D4));
            flags=load<u32>(a+0x2E4);
        }
        store<u32>(a+0x2E4,flags|8);
    }
    s32 phase=gabi::call<s32>(0x02520460,ptr(a+0x3AC),ptr(0x1003475C));
    if(phase!=4)return phase;
    phase=gabi::call<s32>(0x02520460,ptr(a+0x3B4),ptr(0x10034764));
    if(phase!=4)return phase;
    u32 dest=a+0x4A0+u32(load<u8>(a+0xCDA))*0x12C;
    u32 source=0x101CE884;
    for(u32 row=0;row<5;++row) {
        for(u32 col=0;col<5;++col) {
            f32 x=load<f32>(source+col*12),y=load<f32>(source+col*12+4),z=load<f32>(source+col*12+8);
            store<f32>(dest,x);store<f32>(dest+4,y);store<f32>(dest+8,z);dest+=12;
        }
        source+=60;
    }
    u32 parent=load<u32>(a+0x2E8),ship=0;
    gabi::Local<ParentIDTemporary> key;key->id=parent;
    if(parent!=0xFFFFFFFFu)ship=gabi::call<u32>(0x025D5218,ptr(0x025E1234),&key->id);
    store<u32>(0x1046CC38,ship);
    gabi::call(0x023D1EA4,actor);
    return 4;
}
VERIFY(0x023D33D4,pirate_create);
void pirate_static_init() {
    WWHD_FUNC(0x023D3A90,void);
    store<u32>(0x1046CC6C,0);store<u32>(0x1046CC74,0);store<u32>(0x1046CC68,0);store<u32>(0x1046CC70,0);
    gabi::call(0x028F026C,ptr(0x101CEA78));
    store<f32>(0x1046CC3C,-3.1415927410125732f);store<f32>(0x1046CC40,3.1415927410125732f);
    gabi::call(0x028ED6F8,ptr(0x1046CC44));gabi::call(0x028F026C,ptr(0x101CEA84));
    gabi::call(0x028EAB2C,ptr(0x1046CC45));gabi::call(0x028F026C,ptr(0x101CEA90));
    store<f32>(0x1046CC60,0.0f);store<f32>(0x1046CC50,13.0f);store<f32>(0x1046CC54,7.0f);store<f32>(0x1046CC58,-3.5f);
    store<s8>(0x1046CC48,-1);store<f32>(0x1046CC5C,0.45f);store<u32>(0x1046CC64,0x10034648);
    store<u8>(0x1046CC4A,0);store<u8>(0x1046CC49,0);store<u32>(0x1046CC4C,0x40);store<u8>(0x1046CC4B,0);
    gabi::call(0x028F026C,ptr(0x101CEA9C));
}
VERIFY(0x023D3A90,pirate_static_init);
void* pirate_packet_construct(void* object) {
    WWHD_FUNC(0x023D3008,void*,object);
    if(!object)object=gabi::call<void*>(0x0273AD10,0x1B00);
    if(!object)return nullptr;
    u32 a=gabi::ea(object);
    gabi::call(0x027F1278,object);
    store<s16>(a+0x904,0);store<u32>(a+0x908,0);store<u32>(a+0xC,0x10034794);
    store<s16>(a+0x902,0);store<s16>(a+0x900,0);store<u8>(a+0x907,1);store<u8>(a+0x906,0);
    u32 link=a+0x90C;
    if(!link)link=gabi::call<u32>(0x0273AD10,8);
    if(link){store<u32>(link+4,0);store<u32>(link,0);}
    u32 buffers=a+0x914;
    if(!buffers)buffers=gabi::call<u32>(0x0273AD10,0x968);
    if(buffers) {
        gabi::call(0x028EFFD0,ptr(buffers),4,0x254,ptr(0x023D3BB8));
        store<u32>(buffers+0x950,0);store<u32>(buffers+0x960,0);store<u32>(buffers+0x958,0x20);
        store<u8>(buffers+0x964,0);store<u32>(buffers+0x954,0);
        for(u32 channel=0;channel<2;++channel)
            for(u32 frame=0;frame<2;++frame)store<u32>(buffers+channel*0x254+frame*0x4A8,0);
    }
    gabi::call(0x027FD6F4,ptr(a+0x127C));gabi::call(0x027FB40C,ptr(a+0x1288));
    store<u32>(a+0x1294,0x1016EF84);gabi::call(0x028F521C,ptr(a+0x12FC),0x34);
    if(!(a+0x12FC))gabi::call(0x0273AD10,0x30);
    gabi::call(0x027FB40C,ptr(a+0x1330));store<u32>(a+0x133C,0x1016EFB4);
    gabi::call(0x028F521C,ptr(a+0x13A4),0x2F0);
    // GX2 material identity matrices and initial RGBA blocks.
    for(u32 offset=0x13A4;offset<=0x1450;offset+=4)store<f32>(a+offset,0.0f);
    for(u32 offset: {0x13B0u,0x13C0u,0x13D0u,0x13E0u,0x13F0u,0x1400u,0x1410u,0x1420u,0x1430u,0x1440u,0x1450u})store<f32>(a+offset,1.0f);
    for(u32 offset: {0x1454u,0x1474u,0x1494u})gabi::call(0x028EFFD0,ptr(a+offset),2,0x10,ptr(0x023D3C14));
    for(u32 i=0;i<8;++i)if(!(a+0x14B4+i*0x30))gabi::call(0x0273AD10,0x30);
    for(u32 i=0;i<6;++i)if(!(a+0x1634+i*0x10))gabi::call(0x0273AD10,0x10);
    store<u8>(a+0x1694,0);
    gabi::call(0x027B5430,ptr(a+0x1698));gabi::call(0x027BE6B8,ptr(a+0x16B0));gabi::call(0x027BE6B8,ptr(a+0x1740));
    gabi::call(0x027BDF7C,ptr(a+0x17D0));gabi::call(0x027BDF7C,ptr(a+0x1968));
    gabi::call(0x023D276C,object);
    return object;
}
VERIFY(0x023D3008,pirate_packet_construct);
namespace {
void free_owned_pointer(u32 slot) {
    u32 storage=load<u32>(slot);
    u32 manager=load<u32>(0x101F8B4C);
    u32 heap=gabi::call<u32>(0x02755FEC,ptr(manager),ptr(storage));
    u32 vtable=load<u32>(heap+0xC),target=load<u32>(vtable+0x3C);
    gabi::call(target,ptr(heap),ptr(load<u32>(slot)));
}
void release_cloth_vertex_buffers(u32 buffers) {
    for(u32 frame=0;frame<2;++frame) {
        u32 first=buffers+frame*0x4A8;
        for(u32 channel=0;channel<2;++channel) {
            u32 buffer=first+channel*0x254;
            gabi::call(0x027BF7E8,ptr(buffer+0x158));
            u32 storage=load<u32>(buffer+0x250);
            store<u32>(buffer,0);
            if(storage) {
                (void)load<u32>(buffer+0x24C);
                free_owned_pointer(buffer+0x250);
                store<u32>(buffer+0x24C,0);store<u32>(buffer+0x250,0);
            }
        }
    }
}
void release_material_array(u32 material,u32 countOffset,u32 pointerOffset) {
    u32 storage=load<u32>(material+pointerOffset);
    if(!storage)return;
    s32 count=load<s32>(material+countOffset);
    for(s32 index=0;index<count;++index) {
        u32 element=storage+u32(index)*0xF4;
        u32 vtable=load<u32>(element+0xF0),target=load<u32>(vtable+0xC);
        gabi::call(target,ptr(element),2);
        count=load<s32>(material+countOffset);
        storage=load<u32>(material+pointerOffset);
    }
    free_owned_pointer(material+pointerOffset);
    store<u32>(material+countOffset,0);store<u32>(material+pointerOffset,0);
}
void release_cloth_packet(u32 packet) {
    store<u32>(packet+0xC,0x10034794);
    u32 buffers=packet+0x914;
    release_cloth_vertex_buffers(buffers);
    store<u32>(buffers+0x960,0);
    s32 materials=load<s32>(packet+0x127C);
    for(s32 index=0;index<materials;++index) {
        u32 storage=load<u32>(packet+0x1280);
        if(u32(index)<u32(materials))storage+=u32(index)*0x23C;
        for(u32 channel=0;channel<2;++channel)gabi::call(0x027BEBEC,ptr(storage+0x10+channel*0x1C));
        materials=load<s32>(packet+0x127C);
    }
    for(u32 offset:{0x1298u,0x1340u})
        for(u32 channel=0;channel<2;++channel)gabi::call(0x027BEBEC,ptr(packet+offset+channel*0x1C));
    gabi::call(0x027BE2B0,ptr(packet+0x1968),2);gabi::call(0x027BE2B0,ptr(packet+0x17D0),2);
    gabi::call(0x027B54A0,ptr(packet+0x1698),2);
    gabi::call(0x027FB528,ptr(packet+0x1330),0);gabi::call(0x027FB528,ptr(packet+0x1288),0);
    gabi::call(0x027FD764,ptr(packet+0x127C),2);
    if(buffers) {
        release_cloth_vertex_buffers(buffers);store<u32>(buffers+0x960,0);
        gabi::call(0x028F0164,ptr(buffers),4,0x254,ptr(0x023D3C40),0,0);
    }
    u32 list=load<u32>(packet+0x910);
    if(list) {
        s32 count=load<s32>(packet+0x90C);
        for(s32 index=0;index<count;++index) {
            u32 material=list+u32(index)*0x14;
            if(material) {
                store<u32>(material,0);
                release_material_array(material,4,8);
                release_material_array(material,0xC,0x10);
                count=load<s32>(packet+0x90C);
                list=load<u32>(packet+0x910);
            }
        }
        free_owned_pointer(packet+0x910);
        store<u32>(packet+0x90C,0);store<u32>(packet+0x910,0);
    }
    gabi::call(0x027F13DC,ptr(packet),0);
}
}
void pirate_packet_destruct(void* object,u32 flags) {
    WWHD_FUNC(0x023D3CA0,void,object,flags);
    if(object){release_cloth_packet(gabi::ea(object));if(flags&1)gabi::call(0x0273AF40,object);}
}
VERIFY(0x023D3CA0,pirate_packet_destruct);
void pirate_actor_destruct(void* object,u32 flags) {
    WWHD_FUNC(0x023D40CC,void,object,flags);
    if(object) {
        release_cloth_packet(gabi::ea(object)+0x3D4);
        gabi::call(0x025D50BC,object,0);
        if(flags&1)gabi::call(0x0273AF40,object);
    }
}
VERIFY(0x023D40CC,pirate_actor_destruct);
namespace {
struct NormalTemporary {u8 linkage[16];cXyz sum,center,side,other,cross,unit,normalized,scratch;};
void copy_vector(cXyz* destination,cXyz* source) {
    u32 a=gabi::ea(source),b=gabi::ea(destination);
    u32 x=load<u32>(a),y=load<u32>(a+4),z=load<u32>(a+8);
    store<u32>(b,x);store<u32>(b+4,y);store<u32>(b+8,z);
}
}
void pirate_set_vertex_normal(daPirate_Flag_packet_c* packet,cXyz* normal,s32 column,s32 row) {
    WWHD_FUNC(0x023D1A2C,void,packet,normal,column,row);
    u32 base=gabi::ea(packet)+0xCC+u32(packet->mBuffer)*0x12C;
    s32 index=column+row*5;
    gabi::Local<NormalTemporary> local;
    local->center.x=load<f32>(base+u32(index)*12);
    local->center.y=load<f32>(base+u32(index)*12+4);
    local->center.z=load<f32>(base+u32(index)*12+8);
    local->sum.x=0.0f;local->sum.y=0.0f;local->sum.z=0.0f;
    auto subtract=[&](s32 neighbor,cXyz* destination) {
        gabi::call(0x0201ADE0,ptr(base+u32(neighbor)*12),&local->scratch,&local->center);
        copy_vector(destination,&local->scratch);
    };
    auto accumulate=[&](bool otherFirst) {
        gabi::call(0x0201B080,otherFirst?&local->other:&local->side,&local->scratch,otherFirst?&local->side:&local->other);
        copy_vector(&local->cross,&local->scratch);
        gabi::call(0x0201B12C,&local->cross,&local->scratch);
        copy_vector(&local->cross,&local->scratch);
        gabi::call(0x028E8D88,&local->sum,&local->cross,&local->sum);
    };
    if(column!=0) {
        subtract(index-1,&local->side);
        if(row!=0){subtract(index-5,&local->other);accumulate(false);}
        if(row!=4){subtract(index+5,&local->other);accumulate(true);}
    }
    if(column!=4) {
        subtract(index+1,&local->side);
        if(row!=0){subtract(index-5,&local->other);accumulate(true);}
        if(row!=4){subtract(index+5,&local->other);accumulate(false);}
    }
    gabi::call(0x0201B12C,&local->sum,&local->normalized);
    copy_vector(&local->sum,&local->normalized);
    gabi::call(0x0200FCF0);
    u16 phase=u16(u32(column+row)*u32(-800));
    f32 sine=load<f32>(0x104A44F8+u32(phase>>3)*8);
    s16 angle=s16(gabi::ftoi(900.0f*sine));
    u32 matrix=load<u32>(0x1018C7B0);
    gabi::call(0x025F1C28,ptr(matrix),angle);
    gabi::call(0x0200FCD8,&local->sum,&local->cross);
    gabi::call(0x0201B12C,&local->cross,&local->normalized);
    copy_vector(normal,&local->normalized);
    gabi::call(0x0200FD38);
}
VERIFY(0x023D1A2C,pirate_set_vertex_normal);
namespace {struct PacketTemporary {u8 linkage[16];be<f32> matrix[12],position[12],color[4];};}
void pirate_packet_draw(void* object) {
    WWHD_FUNC(0x023D0E68,void,object);
    u32 a=gabi::ea(object),buffers=a+0x914;
    for(u32 channel=0;channel<2;++channel) {
        u32 frame=load<u32>(buffers+0x950);
        u32 vertex=load<u32>(buffers+(channel*2+frame)*0x254);
        for(u32 i=0;i<25;++i) {
            u32 index=load<u8>(a+0x906),source=a+0xCC+(index*25+i)*12;
            f32 z=load<f32>(source+8),x=load<f32>(source),y=load<f32>(source+4);
            u32 dest=vertex+i*0x20;
            store<f32>(dest,x);store<f32>(dest+4,y);store<f32>(dest+8,z);
            index=load<u8>(a+0x906);
            source=a+(channel?0x57C:0x324)+(index*25+i)*12;
            x=load<f32>(source);y=load<f32>(source+4);z=load<f32>(source+8);
            store<f32>(dest+0xC,x);store<f32>(dest+0x10,y);store<f32>(dest+0x14,z);
        }
    }
    u32 frame=load<u32>(buffers+0x950),buffer=buffers+frame*0x254+4;
    for(u32 channel=0;channel<2;++channel) {
        u32 bytes=load<u32>(buffer+0x14C);
        gabi::call(0x027B5E94,ptr(buffer),0,bytes);buffer+=0x4A8;
    }
    frame=load<u32>(buffers+0x950);store<u32>(buffers+0x950,frame==0?1:0);
    gabi::call(0x0255F8F4,ptr(load<u32>(a+0xC8)));
    gabi::call(0x0255FE90,ptr(load<u32>(a+0xC8)));
    gabi::Local<PacketTemporary> local;
    pirate_copy_matrix(local->matrix,ptr(0x104B45F8));
    u32 view=load<u32>(0x104B4708);
    gabi::call(0x027FDA54,ptr(a+0x127C),0,local->matrix,ptr(0x104B470C),ptr(view+0x240));
    u32 tev=load<u32>(a+0xC8),material=load<u32>(a+0x1280);
    pirate_color_s16(local->color,ptr(tev+0x90));
    f32 scale=load<f32>(load<u32>(a+0xC8)+0x28);
    gabi::call(0x0274D458,ptr(material+0x1C4),local->color,scale);
    tev=load<u32>(a+0xC8);pirate_color_s16(local->color,ptr(tev+0x160));
    scale=load<f32>(load<u32>(a+0xC8)+0x16C);
    gabi::call(0x0274D458,ptr(material+0x1D4),local->color,scale);
    gabi::call(0x027FDFF4,ptr(a+0x127C),0);
    tev=load<u32>(a+0xC8);
    u32 primary=tev+0x90,secondary=tev+0x98,extra=tev+0x9C;
    pirate_color_s16(ptr(a+0x13E4),ptr(primary));
    pirate_color_u8(ptr(a+0x13F4),ptr(secondary));
    scale=load<f32>(load<u32>(a+0xC8)+0x24);
    gabi::call(0x0274D2AC,ptr(a+0x13F4),scale);
    if(load<u8>(tev+0x9F))pirate_color_u8(ptr(a+0x1404),ptr(extra));
    else for(u32 i=0;i<4;++i)store<f32>(a+0x1404+i*4,0.0f);
    gabi::call(0x027FB678,ptr(a+0x1330));
    pirate_copy_matrix(local->position,ptr(a+0x98));
    gabi::call(0x028E90D4,local->position,ptr(a+0x12FC));
    gabi::call(0x027FB678,ptr(a+0x1288));
}
VERIFY(0x023D0E68,pirate_packet_draw);
namespace {struct DrawTemporary {u8 linkage[16];be<f32> identity[12];cXyz light,result,arrows[4];};}
BOOL pirate_draw(void* actor) {
    WWHD_FUNC(0x023D116C,BOOL,actor);
    u32 a=gabi::ea(actor),ship=load<u32>(0x1046CC38);
    if(!load<u8>(ship+0x3E4)||(load<u32>(ship+0x2E4)&4))return 0;
    // The environmental lighting record has float and integer subrecords; keep
    // lfs/stfs fields separate so signaling NaNs match the original copies.
    auto floats=[&](u32 first,u32 last){for(u32 o=first;o<=last;o+=4)store<f32>(a+o,load<f32>(ship+o));};
    auto bytes=[&](u32 first,u32 last){for(u32 o=first;o<=last;++o)store<u8>(a+o,load<u8>(ship+o));};
    auto halves=[&](u32 first,u32 last){for(u32 o=first;o<=last;o+=2)store<u16>(a+o,load<u16>(ship+o));};
    floats(0x110,0x124);bytes(0x128,0x12B);halves(0x12C,0x132);floats(0x134,0x150);
    for(u32 o=0x194;o<=0x19C;o+=4)store<u32>(a+o,load<u32>(ship+o));
    halves(0x1A0,0x1A6);bytes(0x1A8,0x1AF);halves(0x1B0,0x1B6);floats(0x1B8,0x1C0);bytes(0x1C4,0x1CC);
    floats(0x1D0,0x210);floats(0x254,0x268);bytes(0x26C,0x26F);halves(0x270,0x276);floats(0x278,0x294);
    f32 y=load<f32>(a+0x318),x=load<f32>(a+0x314),z=load<f32>(a+0x31C);
    gabi::call(0x0200FAD8,0,x,y,z);
    s16 angle=s16(u16(load<s16>(a+0x322))+u16(load<s16>(a+0x32A)));
    gabi::call(0x025F1C28,ptr(load<u32>(0x1018C7B0)),angle);
    gabi::call(0x025F1BF4,ptr(load<u32>(0x1018C7B0)),load<s16>(a+0x320));
    gabi::call(0x025F1C5C,ptr(load<u32>(0x1018C7B0)),load<s16>(a+0x324));
    gabi::call(0x0200FAD8,1,0.0f,0.0f,30.0f);
    gabi::Local<DrawTemporary> local;
    gabi::call(0x028E9098,local->identity);
    gabi::call(0x028E9108,local->identity,ptr(load<u32>(0x1018C7B0)),ptr(a+0x46C));
    store<u32>(a+0x49C,a+0x110);
    u32 play=gabi::call<u32>(0x025200D4);
    store<u32>(0x104B4634,load<u32>(play+0x5D70));
    play=gabi::call<u32>(0x025200D4);
    u32 translucent=load<u32>(play+0x5D74),opaque=load<u32>(0x104B4634);
    store<u32>(0x104B4638,translucent);
    gabi::call(0x027F0E04,ptr(opaque),ptr(a+0x3D4),0);
    play=gabi::call<u32>(0x025200D4);store<u32>(0x104B4634,load<u32>(play+0x5D78));
    play=gabi::call<u32>(0x025200D4);store<u32>(0x104B4638,load<u32>(play+0x5D7C));
    pirate_packet_draw(ptr(a+0x3D4));
    if(load<u8>(0x1046CC49)) {
        u16 facing=load<u16>(a+0x322);
        gabi::call(0x0255F5FC,&local->result,&local->light);
        x=local->light.x;z=local->light.z;
        u16 light=u16(gabi::call<s16>(0x020195B0,x,z));
        u16 corrected=u16(facing+u16(load<s16>(a+0xCD6)));
        u32 angles[4]={light,light,facing,corrected};
        for(u32 i=0;i<4;++i) {
            f32 length=i==1?-400.0f:400.0f;
            u32 table=0x104A44F8+(angles[i]>>3)*8;
            local->arrows[i].x=length*load<f32>(table);
            local->arrows[i].y=0.0f;
            local->arrows[i].z=length*load<f32>(table+4);
        }
        for(u32 i=0;i<4;++i)gabi::call(0x028E8D88,&local->arrows[i],ptr(a+0x314),&local->arrows[i]);
        constexpr u32 guards[3]={0x101FDA48,0x101FDAC0,0x101FDA50};
        constexpr u32 objects[3]={0x101FEBEC,0x101FEBF8,0x101FEBF4};
        constexpr u32 names[3]={0x10034620,0x10034624,0x10034628};
        for(u32 i=0;i<3;++i)if(!load<u32>(guards[i])) {
            store<u32>(guards[i],1);gabi::call(0xC000A848,ptr(objects[i]),ptr(names[i]),4);
        }
    }
    return 1;
}
VERIFY(0x023D116C,pirate_draw);
namespace {
struct ClothMoveTemporary {
    u8 linkage[16];cXyz spring,center,delta,wind,light,probe,result,scaled;
    be<u32> stageName,stageVtable,expectedName,expectedVtable;
};
}
void pirate_move_cloth(void* actor) {
    WWHD_FUNC(0x023D1EA4,void,actor);
    u32 a=gabi::ea(actor);
    gabi::call(0x025200D4);
    u32 wind=gabi::call<u32>(0x0257DAA8);
    f32 x=load<f32>(wind),z=load<f32>(wind+8);
    s16 windAngle=gabi::call<s16>(0x020195B0,x,z);
    s16 rotation=s16(u16(windAngle)-u16(load<s16>(a+0x322))-u16(load<s16>(a+0x32A)));
    gabi::call(0x025F1884,ptr(load<u32>(0x1018C7B0)),rotation);
    gabi::Local<ClothMoveTemporary> local;
    local->probe.x=0.0f;local->probe.y=0.0f;local->probe.z=0.06400000303983688f;
    gabi::call(0x0200FCD8,&local->probe,&local->result);
    local->probe.z=0.0f;local->probe.x=1.0f;
    gabi::call(0x0200FCD8,&local->probe,&local->result);
    s16 phase=s16(u32(load<s16>(a+0x3C4))+load<u32>(0x1046CC4C));
    store<s16>(a+0x3C4,phase);
    f32 wave=load<f32>(0x104A44F8+u32(u16(phase)>>3)*8);
    f32 factor=gabi::fmadds(0.5f,wave,0.5f);
    f32 strength=gabi::fmadds(load<f32>(0x1046CC50),factor,load<f32>(0x1046CC54)*(1.0f-factor));
    f32 normalStrength=std::fabs(f32(local->result.z));
    local->wind.x=0.0f;local->wind.y=0.0f;local->wind.z=strength;
    s32 iterations=1;
    u32 play=gabi::call<u32>(0x025200D4);
    if(load<u8>(play+0x5292)) {
        local->stageName=0x100346F4;local->stageVtable=0x10034630;
        local->expectedName=0x1047E6B8;local->expectedVtable=0x10034630;
        pirate_hio_empty(&local->stageName);
        u32 target=load<u32>(u32(local->stageVtable)+0x14);
        gabi::call(target,&local->stageName);
        u32 first=local->stageName;
        target=load<u32>(u32(local->expectedVtable)+0x14);
        gabi::call(target,&local->expectedName);
        u32 second=local->expectedName;
        bool equal=first==second;
        if(!equal) {
            equal=true;
            for(u32 i=0;i<0x40001;++i) {
                u8 left=load<u8>(first+i),right=load<u8>(second+i);
                if(left!=right){equal=false;break;}
                if(!left)break;
            }
        }
        if(equal) {
            f32 power=gabi::call<f32>(0x02578348);
            gabi::call(0x028E8E64,&local->wind,&local->wind,power);
            if(!load<u32>(0x101D6008))iterations=60;
        }
    }
    for(s32 step=0;step<iterations;++step) {
        u32 previous=load<u8>(a+0xCDA),current=previous^1;
        u32 normals=a+0x6F8+previous*0x12C,positions=a+0x4A0+previous*0x12C;
        u32 next=a+0x4A0+current*0x12C;
        store<u8>(a+0xCDA,u8(current));
        for(s32 column=0;column<5;++column)for(s32 row=0;row<5;++row) {
            u32 index=u32(column+row*5),source=positions+index*12,dest=next+index*12,velocity=a+0xBA8+index*12;
            store<u32>(dest,load<u32>(source));store<u32>(dest+4,load<u32>(source+4));store<u32>(dest+8,load<u32>(source+8));
            local->center.x=load<f32>(source);local->center.y=load<f32>(source+4);local->center.z=load<f32>(source+8);
            f32 dot=gabi::call<f32>(0x028E8F44,&local->wind,ptr(normals+index*12));
            gabi::call(0x0201AE48,ptr(normals+index*12),&local->scaled,dot);
            f32 gravity=load<f32>(0x1046CC58);
            local->spring.x=local->scaled.x;local->spring.y=f32(local->scaled.y)+gravity;local->spring.z=local->scaled.z;
            auto spring=[&](s32 neighbor,f32 length) {
                gabi::call(0x023D176C,&local->center,ptr(positions+u32(neighbor)*12),&local->spring,ptr(velocity),length);
            };
            bool anchored=column==0&&(row==0||row==4);
            if(column!=0) {
                spring(s32(index)-1,200.0f);
                if(row!=0){spring(s32(index)-5,100.0f);spring(s32(index)-6,223.60679626464844f);}
                if(row!=4){spring(s32(index)+5,100.0f);spring(s32(index)+4,223.60679626464844f);}
                if(column!=4) {
                    spring(s32(index)+1,200.0f);
                    if(row!=0)spring(s32(index)-4,223.60679626464844f);
                    if(row!=4)spring(s32(index)+6,223.60679626464844f);
                }
            } else if(!anchored) {
                spring(s32(index)+1,200.0f);
                if(row!=0){spring(s32(index)-5,100.0f);spring(s32(index)-4,223.60679626464844f);}
                if(row!=4){spring(s32(index)+5,100.0f);spring(s32(index)+6,223.60679626464844f);}
            }
            if(anchored) {
                local->delta.x=load<f32>(0x101FFBA8);local->delta.y=load<f32>(0x101FFBAC);local->delta.z=load<f32>(0x101FFBB0);
            } else copy_vector(&local->delta,&local->spring);
            gabi::call(0x028E8D88,ptr(velocity),&local->delta,ptr(velocity));
            gabi::call(0x028E8E64,ptr(velocity),ptr(velocity),0.875f);
            gabi::call(0x028E8D88,ptr(dest),ptr(velocity),ptr(dest));
        }
        u32 buffer=load<u8>(a+0xCDA),normal=a+0x6F8+buffer*0x12C;
        s16 facing=load<s16>(a+0x322);
        gabi::call(0x0255F5FC,&local->result,&local->light);
        x=local->light.x;z=local->light.z;
        s16 light=gabi::call<s16>(0x020195B0,x,z);
        pirate_set_correct_normal(gabi::at<daPirate_Flag_packet_c>(a+0x3D4),s16(u16(light)-u16(facing)),normalStrength);
        gabi::call(0x025F1884,ptr(load<u32>(0x1018C7B0)),load<s16>(a+0xCD4));
        for(s32 row=0;row<5;++row)for(s32 column=0;column<5;++column) {
            pirate_set_vertex_normal(gabi::at<daPirate_Flag_packet_c>(a+0x3D4),gabi::at<cXyz>(normal),column,row);normal+=12;
        }
    }
    pirate_set_back_normals(gabi::at<daPirate_Flag_packet_c>(a+0x3D4));
    u32 buffer=load<u8>(a+0xCDA);
    gabi::call(0xC00088B8,ptr(a+0x4A0+buffer*0x12C),0x12C);
    buffer=load<u8>(a+0xCDA);gabi::call(0xC00088B8,ptr(a+0x6F8+buffer*0x12C),0x12C);
    buffer=load<u8>(a+0xCDA);gabi::call(0xC00088B8,ptr(a+0x950+buffer*0x12C),0x12C);
}
VERIFY(0x023D1EA4,pirate_move_cloth);
namespace {
struct ArchiveTemporary {u8 linkage[16];be<u32> shaderName,shaderVtable,clothName,clothVtable,shipName,shipVtable;};
u32 allocate_cloth_storage(u32 bytes,u32 alignment) {
    u32 heap=gabi::call<u32>(0x02756140,ptr(load<u32>(0x101F8B4C)));
    u32 target=load<u32>(load<u32>(heap+0xC)+0x34);
    return gabi::call<u32>(target,ptr(heap),bytes,alignment);
}
void initialize_cloth_texture(u32 destination,u32 source) {
    bool same=true;
    for(u32 offset:{4u,8u,0xCu,0x10u,0x14u,0x18u,0x38u,0x34u,0x1Cu})
        if(load<u32>(destination+offset)!=load<u32>(source+offset)){same=false;break;}
    if(!same)gabi::call(0x027BDEB4,ptr(destination),ptr(source));
    else {
        u32 image=load<u32>(source+0x28),mipmap=load<u32>(source+0x30);
        store<u32>(destination+0x28,image);store<u32>(destination+0xD4,image);
        store<u32>(destination+0x30,mipmap);store<u32>(destination+0xDC,mipmap);
    }
    u8 dirty=load<u8>(destination+0x190);
    store<u32>(destination+0x160,2);store<u32>(destination+0x15C,2);store<u32>(destination+0x164,2);
    store<u8>(destination+0x190,u8(dirty|2));
}
}
void pirate_initialize_graphics(void* object) {
    WWHD_FUNC(0x023D276C,void,object);
    u32 a=gabi::ea(object),buffers=a+0x914;
    gabi::Local<ArchiveTemporary> local;
    local->shaderName=0x10034730;local->shaderVtable=0x10034630;
    u32 manager=gabi::call<u32>(0x027FFCBC,object);
    s32 shader=gabi::call<s32>(0x027B90AC,ptr(load<u32>(manager+4)),&local->shaderName);
    u32 options=0;
    if(shader>=0) {
        u32 count=load<u32>(manager+8),records=load<u32>(manager+0xC);
        u32 record=records+(u32(shader)<count?u32(shader)*0x24:0);
        if(!load<u8>(record+0x20)) {
            u32 archive=load<u32>(manager+4),archiveCount=load<u32>(archive+0x1C),program=0;
            if(u32(shader)<archiveCount)program=load<u32>(archive+0x20)+u32(shader)*0x84;
            gabi::call(0x02800B0C,ptr(record),ptr(program),0);
            count=load<u32>(manager+8);records=load<u32>(manager+0xC);
        }
        options=records+(u32(shader)<count?u32(shader)*0x24:0);
    }
    gabi::call(0x0280068C,ptr(a+0x908),ptr(options),0);
    store<u32>(a+0x1268,0x13);store<u32>(a+0x1270,0x10034788);
    for(u32 channel=0;channel<2;++channel)for(u32 frame=0;frame<2;++frame) {
        u32 buffer=buffers+channel*0x254+frame*0x4A8;
        u32 vertices=load<u32>(buffer);
        if(!vertices) {
            u32 storage=allocate_cloth_storage(0x320,0x40);
            if(storage){store<u32>(buffer+0x250,storage);store<u32>(buffer+0x24C,25);}
            vertices=load<u32>(buffer+0x250);store<u32>(buffer,vertices);
        }
        gabi::call(0x027FF478,ptr(buffer+4),ptr(vertices),25,ptr(buffers+0x954));
    }
    store<u32>(buffers+0x960,0);store<u8>(buffers+0x964,1);
    for(u32 index=0;index<load<u32>(a+0x908);++index) {
        u32 count=load<u32>(a+0x90C),material=load<u32>(a+0x910);
        if(index<count)material+=index*0x14;
        u32 program=load<u32>(material);
        store<u32>(material,0);
        release_material_array(material,4,8);release_material_array(material,0xC,0x10);
        store<u32>(material,program);
        for(u32 channel=0;channel<2;++channel) {
            u32 storage=allocate_cloth_storage(0x1E8,4);
            for(u32 frame=0;frame<2;++frame) {
                u32 texture=storage+frame*0xF4;
                if(texture)gabi::call(0x027BF734,ptr(texture));
            }
            if(storage){store<u32>(material+8+channel*8,storage);store<u32>(material+4+channel*8,2);}
        }
        for(u32 channel=0;channel<2;++channel)for(u32 frame=0;frame<2;++frame) {
            u32 textureCount=load<u32>(material+4+channel*8),texture=load<u32>(material+8+channel*8);
            if(frame<textureCount)texture+=frame*0xF4;
            gabi::call(0x027FF530,ptr(program),ptr(texture),ptr(buffers+4+channel*0x254+frame*0x4A8),ptr(buffers+0x954),0);
        }
    }
    gabi::call(0x027FE084,ptr(a+0x127C),1,0);
    gabi::call(0x027B54E0,ptr(a+0x1698),ptr(0x10034668),4,0x28);
    store<u32>(a+0x169C,6);
    for(u32 channel=0;channel<2;++channel) {
        u32 frame=load<u32>(buffers+0x950),vertex=load<u32>(buffers+(channel*2+frame)*0x254),end=vertex+0x320;
        for(u32 block=vertex;block<end;block+=0x20)
            for(u32 byte=0;byte<0x20;++byte)store<u8>((block&~31u)+byte,0);
        frame=load<u32>(buffers+0x950);vertex=load<u32>(buffers+(channel*2+frame)*0x254);
        for(u32 i=0;i<25;++i) {
            u32 position=0x101CE884+i*12,uv=0x101CE9B0+i*8,dest=vertex+i*0x20;
            f32 x=load<f32>(position),y=load<f32>(position+4),z=load<f32>(position+8);
            store<f32>(dest,x);store<f32>(dest+0xC,0.0f);store<f32>(dest+8,z);store<f32>(dest+0x10,0.0f);
            store<f32>(dest+4,y);store<f32>(dest+0x14,0.0f);
            store<f32>(dest+0x18,load<f32>(uv));store<f32>(dest+0x1C,load<f32>(uv+4));
        }
    }
    u32 frame=load<u32>(buffers+0x950),other=frame==0?1:0;
    for(u32 channel=0;channel<2;++channel)for(u32 vertex=0;vertex<25;++vertex) {
        u32 source=load<u32>(buffers+(channel*2+frame)*0x254),dest=load<u32>(buffers+(channel*2+other)*0x254);
        for(u32 component=0;component<8;++component)
            store<f32>(dest+vertex*0x20+component*4,load<f32>(source+vertex*0x20+component*4));
    }
    frame=load<u32>(buffers+0x950);u32 buffer=buffers+frame*0x254+4;
    for(u32 channel=0;channel<2;++channel){gabi::call(0x027B5E94,ptr(buffer),0,load<u32>(buffer+0x14C));buffer+=0x4A8;}
    frame=load<u32>(buffers+0x950);store<u32>(buffers+0x950,frame==0?1:0);
    gabi::call(0x0274FBF8,ptr(load<u32>(0x101F8B18)));
    local->clothVtable=0x10034630;local->clothName=0x10034740;
    u32 resource=gabi::call<u32>(0x026066C4,ptr(load<u32>(0x101F4F28)),&local->clothName,9);
    if(!resource)gabi::call(0x0273AA24,ptr(0x1003471C),0x2A7,ptr(0x1003474C));
    gabi::call(0x02773798,ptr(a+0x16B0),ptr(load<u32>(resource+0x20)));
    local->shipVtable=0x10034630;local->shipName=0x10034714;
    resource=gabi::call<u32>(0x026066C4,ptr(load<u32>(0x101F4F28)),&local->shipName,3);
    if(!resource)gabi::call(0x0273AA24,ptr(0x1003471C),0x2AE,ptr(0x1003474C));
    gabi::call(0x02773798,ptr(a+0x1740),ptr(load<u32>(resource+0x20)));
    gabi::call(0x0274FCCC,ptr(load<u32>(0x101F8B18)));
    initialize_cloth_texture(a+0x17D0,a+0x16B0);initialize_cloth_texture(a+0x1968,a+0x1740);
}
VERIFY(0x023D276C,pirate_initialize_graphics);
namespace {
struct RasterTemporary {u8 linkage[16];u8 descriptor[0x120];};
void bind_material_uniforms(u32 program,u32 material) {
    u32 table=load<u32>(material+0x4C)*0x1C+material+0x10;
    u32 record=load<u32>(program+0xC)?load<u32>(program+0x10):0;
    s16 vertex=load<s16>(record+0xC),pixel=load<s16>(record+0xE),geometry=load<s16>(record+0x10);
    u32 size=load<u32>(table+4),data=load<u32>(table+0xC);
    if(pixel!=-1)gabi::call(0xC0006900,pixel,ptr(data),size);
    if(vertex!=-1)gabi::call(0xC0006A38,vertex,ptr(data),size);
    if(geometry!=-1)gabi::call(0xC00068A8,geometry,ptr(data),size);
}
void call_material_apply(u32 material,u32 program) {
    u32 target=load<u32>(load<u32>(material+0xC)+0x2C);
    gabi::call(target,ptr(material),ptr(program));
}
}
void pirate_render_material(void* object,void* context) {
    WWHD_FUNC(0x023D350C,void,object,context);
    u32 a=gabi::ea(object),c=gabi::ea(context),index=load<u32>(c+0xC),program=0;
    if(s32(index)<4) {
        u32 count=load<u32>(a+0x90C),record=load<u32>(a+0x910);
        if(index<count)record+=index*0x14;
        program=load<u32>(record);
    }
    u32 cache=gabi::call<u32>(0x027F29D4,ptr(0x104B45C0));
    u32 shader=load<u32>(program),cachedShader=load<u32>(cache+4);
    if(shader!=cachedShader) {
        u8 flags=load<u8>(shader);u32 cachedArchive=load<u32>(cache);
        if(flags&2){store<u8>(shader,u8(flags&~2u));gabi::call(0x027BB9E0,ptr(shader),0);}
        u32 archive=load<u32>(load<u32>(shader+0x7C)+0x28);
        if(cachedArchive!=archive)gabi::call(0x027B9F68,ptr(archive));
        if(load<u32>(shader+0xC))gabi::call(0xC00060E0,ptr(load<u32>(shader+4)),load<u32>(shader+0xC)); /* GX2CallDisplayList(list, size) (game test 2026-10-05) */
        else gabi::call(0x027BB7CC,ptr(shader));
        store<u32>(cache,archive);store<u32>(cache+4,shader);
    }
    index=load<u32>(c+0xC);
    if(index==0) {
        call_material_apply(a+0x1288,program);
        u32 material=load<u32>(c+0x14);
        if(material)bind_material_uniforms(program,load<u32>(material+4));
    } else if(index==1) {
        bind_material_uniforms(program,load<u32>(a+0x1280));
        call_material_apply(a+0x1330,program);call_material_apply(a+0x1288,program);
    } else if(index==2) {
        bind_material_uniforms(program,load<u32>(a+0x1280));
        call_material_apply(a+0x1330,program);call_material_apply(a+0x1288,program);
        u32 material=load<u32>(c+0x30);if(material)call_material_apply(material,program);
        gabi::call(0x027FFE54,context,ptr(program));
    }
    u32 textures=load<u32>(program+0x14)?load<u32>(program+0x18):0;
    gabi::call(0x027BE53C,ptr(a+0x17D0),ptr(textures+4),-1,0);
    u32 textureCount=load<u32>(program+0x14);
    textures=textureCount>1?load<u32>(program+0x18)+0x14:0;
    gabi::call(0x027BE53C,ptr(a+0x1968),ptr(textures+4),-1,0);
    gabi::Local<RasterTemporary> local;
    u32 descriptor=gabi::ea(local.get())+16;
    gabi::call(0x02750250,ptr(descriptor));
    store<u32>(descriptor+0xC,2);store<u32>(descriptor+8,0);
    u32 bits=load<u32>(descriptor+0xEC)&~15u;
    store<u32>(descriptor+0xE4,4);store<f32>(descriptor+0xE8,0.5f);
    store<u8>(descriptor+0xE0,1);store<u32>(descriptor+0xEC,(((bits+7)&0xFFFFFF0Fu)+0x10));
    gabi::call(0x0280037C,load<u32>(c+0xC),ptr(descriptor));
    gabi::call(0x02750370,ptr(descriptor));
    u32 count=load<u32>(a+0x90C),record=load<u32>(a+0x910);
    index=load<u32>(c+0xC);if(index<count)record+=index*0x14;
    u32 frame=load<u32>(a+0x1264)==0?2:0;
    gabi::call(0x027BFE5C,ptr(load<u32>(record+frame*4+8)));
    for(u32 face=0;face<4;++face) {
        u32 topology=load<u32>(a+0x1698),stride=load<u32>(a+0x16A8),indices=load<u32>(a+0x16A0),type=load<u32>(a+0x169C);
        gabi::call(0xC0006178,type,0xA,topology,ptr(indices+stride*(face*10)),0,1);
    }
    store<u32>(descriptor+8,1);gabi::call(0x02750684,ptr(descriptor));
    count=load<u32>(a+0x90C);index=load<u32>(c+0xC);record=load<u32>(a+0x910);
    if(index<count)record+=index*0x14;
    frame=load<u32>(a+0x1264)==0?2:0;
    u32 textureCount2=load<u32>(record+frame*4+4),vertices=load<u32>(record+frame*4+8);
    if(textureCount2>1)vertices+=0xF4;
    gabi::call(0x027BFE5C,ptr(vertices));
    for(u32 face=0;face<4;++face) {
        u32 topology=load<u32>(a+0x1698),stride=load<u32>(a+0x16A8),indices=load<u32>(a+0x16A0),type=load<u32>(a+0x169C);
        gabi::call(0xC0006178,type,0xA,topology,ptr(indices+stride*(face*10)),0,1);
    }
    gabi::call(0x02750370,ptr(0x104B474C));
}
VERIFY(0x023D350C,pirate_render_material);
