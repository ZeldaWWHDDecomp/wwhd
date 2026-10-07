#include "wwhd.h"
using namespace gabi;
// Reconstructed solely from the WWHD instructions. No SDK implementation template.
namespace {
struct Animator_l {
    u8 links[8];
    gptr<void> resource;
    be<f32> frame;
    u8 pad10[0x14];
    be<f32> speed;
    be<u32> playType;
    be<u32> flags;
};
WWHD_OFFSET(Animator_l,resource,0x8);
WWHD_OFFSET(Animator_l,frame,0xC);
WWHD_OFFSET(Animator_l,speed,0x24);
WWHD_OFFSET(Animator_l,playType,0x28);
WWHD_OFFSET(Animator_l,flags,0x2C);
static f32 frameCount(u32 animator) { return (f32)call<u32>(0x028713B8,animator); }
}
void Animator_UpdateFrame(u32 self) {
    WWHD_FUNC(0x02872DFC,void,self);
    f32 zero=load<f32>(0x1017FFC8);
    u32 flags=load<u32>(self+0x2C);
    f32 speed=load<f32>(self+0x24);
    store<u32>(self+0x2C,flags&~7u);
    if(speed==zero) return;
    f32 frame=load<f32>(self+0xC)+speed;
    if(speed>zero) {
        f32 end=frameCount(self);
        if(!(frame<end)) {
            u32 type=load<u32>(self+0x28);
            if(type==0) {
                frame=frameCount(self);
                flags=load<u32>(self+0x2C);
                store<f32>(self+0x24,zero);
                store<u32>(self+0x2C,flags|1u);
            } else if(type==1) {
                f32 length=frameCount(self);
                flags=load<u32>(self+0x2C);
                store<u32>(self+0x2C,flags|2u);
                frame=frame-length;
            } else if(type==2) {
                f32 end1=frameCount(self),end2=frameCount(self);
                f32 currentSpeed=load<f32>(self+0x24);
                flags=load<u32>(self+0x2C);
                store<f32>(self+0x24,-currentSpeed);
                store<u32>(self+0x2C,flags|2u);
                frame=end1-(frame-end2);
            }
        }
    } else if(!(frame>zero)) {
        u32 type=load<u32>(self+0x28);
        if(type==0) {
            flags=load<u32>(self+0x2C);
            store<f32>(self+0x24,zero);
            frame=zero;
            store<u32>(self+0x2C,flags|1u);
        } else if(type==1) {
            f32 length=frameCount(self);
            flags=load<u32>(self+0x2C);
            store<u32>(self+0x2C,flags|4u);
            frame=frame+length;
        } else if(type==2) {
            flags=load<u32>(self+0x2C);
            store<f32>(self+0x24,-speed);
            frame=-frame;
            store<u32>(self+0x2C,flags|4u);
        }
    }
    store<f32>(self+0xC,frame);
}
VERIFY(0x02872DFC,Animator_UpdateFrame);
void Layout_UpdateAnimFrame(u32 self) {
    WWHD_FUNC(0x0287FB68,void,self);
    u32 sentinel=self+4,node=load<u32>(sentinel);
    while(node!=sentinel) {
        u32 vt=load<u32>(node+0x14),target=load<u32>(vt+0x1C);
        call_ptr<void>(target,node);
        node=load<u32>(node);
    }
    sentinel=self+0x28;node=load<u32>(sentinel);
    while(node!=sentinel) {
        u32 part=load<u32>(node+8),vt=load<u32>(part+0x30),target=load<u32>(vt+0x54);
        call_ptr<void>(target,part);
        node=load<u32>(node);
    }
}
VERIFY(0x0287FB68,Layout_UpdateAnimFrame);
