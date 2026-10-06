/* WWHD reconstruction. GC functions are predominantly stubs.
 * Actual TU:0231AE94–0231C2AB; adjacent Aygr/Barrel boundaries audited.
 */
#include "d/actor/d_a_obj_balancelift.h"
namespace {
constexpr u32 HIO=0x10469198;
template<class T> T* part(void* actor,u32 offset) { return gabi::at<T>(gabi::ea(actor)+offset); }
void copyWords(u32 dst,u32 src,u32 count) {
    for(u32 i=0;i<count;++i) gabi::store<u32>(dst+4*i,gabi::load<u32>(src+4*i));
}
s32 ride_actor_check(fopAc_ac_c* actor) {
    WWHD_FUNC(0x0231AE94,s32,actor);
    if(!gabi::call<s32>(0x025D4604,actor) || !actor) return 0;
    s16 type=gabi::load<s16>(gabi::ea(actor)+8);
    if(type==0x1CA) return 1;
    if(type==0xA8) {
        u32 vtable=gabi::load<u32>(gabi::ea(actor)+0xB4);
        u32 target=gabi::load<u32>(vtable+0xBC);
        s32 carried=gabi::call<s32>(target,actor);
        if(carried==-1) return 1;
        gabi::Local<be<s32>> id; *id=carried;
        auto* child=gabi::call<fopAc_ac_c*>(0x025D5218,0x025E1234,id.get());
        return child ? ride_actor_check(child)+1 : 1;
    }
    return type==0xCB || type==0x13A || type==0x14E;
}
VERIFY(0x0231AE94,ride_actor_check);
void ride_call_back(void* bg,daBalancelift_c* lift,fopAc_ac_c* rider) {
    WWHD_FUNC(0x0231AF88,void,bg,lift,rider);
    s32 added=ride_actor_check(rider);
    gabi::Local<cXyz> riderPos,delta;
    riderPos->z=rider->current.pos.z;
    riderPos->y=rider->current.pos.y;
    riderPos->x=rider->current.pos.x;
    gabi::call(0x0201ADE0,riderPos.get(),delta.get(),&lift->pivot);
    f32 factor=gabi::load<f32>(HIO+0xC);
    gabi::call(0x028E8E64,delta.get(),delta.get(),factor);
    gabi::call(0x028E8D88,&lift->pivotVelocity,delta.get(),&lift->pivotVelocity);
    s16 second=lift->isSecond;
    auto* weight=(be<s16>*)lift->sharedWeight;
    s16 value=*weight;
    *weight=(s16)(second ? value-added : value+added);
}
VERIFY(0x0231AF88,ride_call_back);
}

BOOL daBalancelift_c::CreateHeap() {
    WWHD_FUNC(0x0231B05C,BOOL,this);
    gabi::Local<SafeString> modelName,bgName;
    modelName->__vtbl=0x1002542C; modelName->mStringTop=0x10025548;
    void* data=gabi::call<void*>(0x026066C4,gabi::at<u8>(gabi::load<u32>(0x101F4F28)),modelName.get(),4);
    if(!data) gabi::call(0x0273AA24,STR(0x10025478),0x1AA,STR(0x100254A0));
    model=gabi::call<J3DModel*>(0x025E38E0,data,0,0x11020203);
    if(!model) gabi::call(0x0273AA24,STR(0x10025478),0x1AC,STR(0x100254B4));
    chainPacket=gabi::call<u8*>(0x0251A2CC,3,part<u8>(this,0x110),2.0f);
    if(!chainPacket) gabi::call(0x0273AA24,STR(0x10025478),0x1AE,STR(0x100254C4));
    background=gabi::call<u8*>(0x024F23F4,0);
    if(!background) gabi::call(0x0273AA24,STR(0x10025478),0x1B1,STR(0x10025490));
    bgName->__vtbl=0x1002542C; bgName->mStringTop=0x10025548;
    data=gabi::call<void*>(0x026066C4,gabi::at<u8>(gabi::load<u32>(0x101F4F28)),bgName.get(),7);
    auto* bg=(u8*)background;
    gabi::call(0x0200A030,bg,data,1,&backgroundMatrix);
    bg=(u8*)background;
    gabi::store<u32>(gabi::ea(bg)+0xA8,0x024EE658);
    if(!model) return FALSE;
    if(!chainPacket) return FALSE;
    return background!=nullptr;
}
VERIFY(0x0231B05C,&daBalancelift_c::CreateHeap);
namespace {
BOOL CheckCreateHeap(daBalancelift_c* lift) {
    WWHD_FUNC(0x0231B1C8,BOOL,lift);
    return lift->CreateHeap();
}
VERIFY(0x0231B1C8,CheckCreateHeap);
}

void daBalancelift_c::set_mtx() {
    WWHD_FUNC(0x0231B1CC,void,this);
    auto* matrix=gabi::at<Mtx34>(0x1048D0CC);
    f32 x=current.pos.x,z=current.pos.z,y=current.pos.y;
    gabi::call(0x028E93CC,matrix,x,y,z);
    gabi::call(0x025F25CC,&supportRotation);
    y=chainLength;
    gabi::call(0x025F24E0,0.0f,-y,0.0f);
    gabi::Local<cXyz> upper,origin;
    upper->z=0.0f; upper->x=0.0f; upper->y=300.0f;
    gabi::call(0x028E8F64,matrix,upper.get(),&anchorTop);
    gabi::call(0x028E8F64,matrix,&chainPosition,&chainTop);
    origin->x=0.0f; origin->y=0.0f; origin->z=0.0f;
    gabi::call(0x028E8F64,matrix,origin.get(),&platformCenter);
    gabi::call(0x025F25CC,&platformRotation);
    x=gabi::load<f32>(HIO+0x28); y=gabi::load<f32>(HIO+0x2C); z=gabi::load<f32>(HIO+0x30);
    gabi::call(0x025F2518,x,y,z);
    J3DModel_setBaseTRMtx((J3DModel*)model,matrix);
    y=gabi::load<f32>(0x1047BBB0)+0.6f;
    gabi::call(0x025F2518,1.0f,y,1.0f);
    y=gabi::load<f32>(0x1047BBB4)-17.0f;
    gabi::call(0x025F24E0,0.0f,y,0.0f);
    gabi::call(0x028E90D4,matrix,&backgroundMatrix);
    u32 packet=gabi::ea((u8*)chainPacket);
    u32 vertices=gabi::load<u32>(packet+0xAC);
    // The integer copies preserve signalling-NaN payloads and alias ordering.
    copyWords(vertices,gabi::ea(&platformCenter),3);
    copyWords(vertices+12,gabi::ea(&chainTop),3);
    copyWords(vertices+24,gabi::ea(&current.pos),3);
}
VERIFY(0x0231B1CC,&daBalancelift_c::set_mtx);

s32 daBalancelift_c::CreateInit() {
    WWHD_FUNC(0x0231B3D0,s32,this);
    u32 base=gabi::ea(this);
    u32 pathNo=(gabi::load<u32>(base+0xB0)>>16)&0xFF;
    be<s16>* weightSlot=&weight;
    be<s16>* flagsSlot=&weightFlags;
    if(pathNo!=0xFF) {
        s8 room=gabi::load<s8>(base+0x326);
        path=gabi::call<u8*>(0x025AAF88,pathNo,room);
        u32 p=gabi::ea((u8*)path);
        if(p && gabi::load<u16>(p)) {
            u32 points=gabi::load<u32>(p+8);
            f32 x=gabi::load<f32>(points+4);
            current.pos.x=x;
            f32 y=gabi::load<f32>(points+8);
            p=gabi::ea((u8*)path);
            current.pos.y=y;
            f32 z=gabi::load<f32>(points+12);
            gabi::store<f32>(base+0x2EC,x); current.pos.z=z;
            gabi::store<f32>(base+0x2F0,y); gabi::store<f32>(base+0x2F4,z);
            if(gabi::load<u16>(p)>1) {
                points=gabi::load<u32>(p+8);
                gabi::Local<cXyz> childPos;
                childPos->x=gabi::load<f32>(points+0x14);
                childPos->y=gabi::load<f32>(points+0x18);
                s8 layer=gabi::load<s8>(base+0x1C9);
                u32 parent=gabi::load<u32>(base+4);
                childPos->z=gabi::load<f32>(points+0x1C);
                u32 id=gabi::call<u32>(0x025D5A20,0x6F,parent,-1,childPos.get(),layer,0,0,-1,0);
                gabi::store<u32>(base+0x2E8,id);
            }
        }
        weight=0; weightFlags=0; isSecond=0;
    } else {
        u32 parent=gabi::load<u32>(base+0x2E8);
        weight=0; weightFlags=0;
        if(parent!=0xFFFFFFFF) {
            gabi::Local<be<u32>> id; *id=parent;
            auto* other=gabi::call<u8*>(0x025D5218,0x025E1234,id.get());
            if(other) {
                flagsSlot=part<be<s16>>(other,0x43A);
                weightSlot=part<be<s16>>(other,0x430);
                isSecond=1;
            } else isSecond=0;
        } else isSecond=0;
    }
    sharedWeight=weightSlot; sharedFlags=flagsSlot;
    f32 length=gabi::load<f32>(HIO+0x24);
    f32 range=gabi::load<f32>(HIO+0x20);
    verticalSpeed=0.0f; chainLength=gabi::fnmsubs(range,0.5f,length);
    u32 play=gabi::call<u32>(0x025200D4);
    auto* bg=(u8*)background;
    gabi::call(0x024EEA6C,gabi::at<u8>(play+0x12A0),bg,this);
    f32 y=current.pos.y,offset=chainLength,x=current.pos.x,z=current.pos.z;
    pivot.x=x; pivot.z=z; pivot.y=y-offset;
    copyWords(gabi::ea(&pivotVelocity),0x101FFBA8,3);
    copyWords(gabi::ea(&chainVelocity),0x101FFBA8,3);
    copyWords(gabi::ea(&chainPosition),0x101FFBA8,3);
    copyWords(gabi::ea(&targetChainPosition),0x101FFBA8,3);
    copyWords(gabi::ea(&supportRotation),0x101E9C38,4);
    copyWords(gabi::ea(&platformRotation),0x101E9C38,4);
    scale.z=2.0f; scale.x=2.0f; scale.y=2.0f;
    set_mtx();
    bg=(u8*)background;
    gabi::store<u32>(gabi::ea(bg)+0xB0,0x0231AF88);
    gabi::call(0x02515F14,part<u8>(this,0x604),0xC0,0xFF,this);
    gabi::call(0x02516518,part<u8>(this,0x640),gabi::at<u8>(0x101C7DF4));
    u32 flags=gabi::load<u32>(base+0x6D4);
    gabi::store<u32>(base+0x684,base+0x604);
    gabi::store<u32>(base+0x6D4,flags|3);
    if(gabi::load<s8>(HIO)<0) {
        s32 child=gabi::call<s32>(0x025F0A10,STR(0x100254EC),gabi::at<u8>(HIO));
        gabi::store<u8>(HIO,(u8)child);
    }
    return 4;
}
VERIFY(0x0231B3D0,&daBalancelift_c::CreateInit);
namespace {
s32 daBalanceliftCreate(daBalancelift_c* lift) {
    WWHD_FUNC(0x0231B6CC,s32,lift);
    u32 base=gabi::ea(lift),flags=gabi::load<u32>(base+0x2E4);
    if(!(flags&8)) {
        if(lift) {
            gabi::call(0x025D4ED0,lift);
            gabi::store<u32>(base+0xB4,0x10025464);
            gabi::call(0x025E9960,part<u8>(lift,0x4B8));
            gabi::call(0x0200BD2C,part<u8>(lift,0x604));
            gabi::call(0x02515DA0,part<u8>(lift,0x620));
            gabi::store<u32>(base+0x61C,0x1004AE88);
            gabi::store<u32>(base+0x620,0x1004AEC0);
            gabi::call(0x02515FB8,part<u8>(lift,0x640));
            gabi::store<u32>(base+0x754,0x100015A8);
            gabi::store<u32>(base+0x750,0x10025444);
            gabi::call(0x02018590,part<u8>(lift,0x758));
            gabi::store<u32>(base+0x67C,0x1004B108);
            gabi::store<u32>(base+0x754,0x1004B160);
            flags=gabi::load<u32>(base+0x2E4);
            gabi::store<u32>(base+0x76C,0x1004B150);
        }
        gabi::store<u32>(base+0x2E4,flags|8);
    }
    s32 phase=gabi::call<s32>(0x02520460,&lift->phase,STR(0x10025548));
    if(phase==4) {
        if(!gabi::call<s32>(0x025D63E8,lift,0x0231B1C8,0xE40)) return 5;
        phase=lift->CreateInit();
        u32 model=gabi::ea((J3DModel*)lift->model);
        gabi::store<u32>(base+0x348,model?model+0xC8:0);
        gabi::call(0x025D674C,lift,-150.0f,-150.0f,-150.0f,150.0f,1000.0f,150.0f);
        model=gabi::ea((J3DModel*)lift->model);
        gabi::call(0x028E90D4,gabi::at<u8>(model?model+0xC8:0),&lift->backgroundMatrix);
    }
    return phase;
}
VERIFY(0x0231B6CC,daBalanceliftCreate);
BOOL daBalanceliftDelete(daBalancelift_c* lift) {
    WWHD_FUNC(0x0231B86C,BOOL,lift);
    u32 play=gabi::call<u32>(0x025200D4);
    gabi::call(0x020087EC,gabi::at<u8>(play+0x12A0),(u8*)lift->background);
    gabi::call(0x025204C8,&lift->phase,STR(0x10025548));
    u32 packet=gabi::ea((u8*)lift->chainPacket);
    if(packet) {
        u32 vtable=gabi::load<u32>(packet+0xC);
        u32 target=gabi::load<u32>(vtable+0xC);
        gabi::call(target,gabi::at<u8>(packet),3);
    }
    lift->chainPacket=nullptr;
    s8 child=gabi::load<s8>(HIO);
    if(child>=0) {
        gabi::call(0x025F0A18,child);
        gabi::store<u8>(HIO,0xFF);
    }
    return TRUE;
}
VERIFY(0x0231B86C,daBalanceliftDelete);
}

void daBalancelift_c::calc_weight() {
    WWHD_FUNC(0x0231B904,void,this);
    auto* weight=(be<s16>*)sharedWeight;
    s16 second=isSecond,value=*weight;
    f32 length=gabi::load<f32>(HIO+0x24);
    if(!second) {
        if(value==0) {
            auto* flags=(be<s16>*)sharedFlags; *flags=(s16)((s16)*flags|1);
            flags=(be<s16>*)sharedFlags;
            s16 bits=*flags;
            length=gabi::fnmsubs(gabi::load<f32>(HIO+0x20),0.5f,length);
            if(bits==3) {
                weight=(be<s16>*)sharedWeight; *weight=0;
                flags=(be<s16>*)sharedFlags; *flags=0;
            }
        } else {
            if(value<=0) length-=gabi::load<f32>(HIO+0x20);
            auto* flags=(be<s16>*)sharedFlags; *flags=(s16)((s16)*flags|1);
            flags=(be<s16>*)sharedFlags;
            if((s16)*flags==3) {
                weight=(be<s16>*)sharedWeight; *weight=0;
                flags=(be<s16>*)sharedFlags; *flags=0;
            }
        }
    } else {
        if(value==0) length=gabi::fnmsubs(gabi::load<f32>(HIO+0x20),0.5f,length);
        else if(value>0) length-=gabi::load<f32>(HIO+0x20);
        auto* flags=(be<s16>*)sharedFlags; *flags=(s16)((s16)*flags|2);
        flags=(be<s16>*)sharedFlags;
        if((s16)*flags==3) {
            weight=(be<s16>*)sharedWeight; *weight=0;
            flags=(be<s16>*)sharedFlags; *flags=0;
        }
    }
    gabi::Local<cXyz> horizontal,delta;
    horizontal->y=0.0f; horizontal->x=chainPosition.x; horizontal->z=chainPosition.z;
    f32 distance=gabi::call<f32>(0x028E8DD0,horizontal.get());
    f32 squared=gabi::fmsubs(length,length,distance);
    f32 target=gabi::call<f32>(0x028F4384,squared);
    f32 old=chainLength, velocity=verticalSpeed;
    velocity=gabi::fmadds(0.005f,target-old,velocity)*0.92f;
    verticalSpeed=velocity; chainLength=old+velocity;
    gabi::call(0x0201ADE0,&targetChainPosition,delta.get(),&chainPosition);
    f32 force=gabi::load<f32>(HIO+0x1C);
    gabi::call(0x028E8E64,delta.get(),delta.get(),force);
    gabi::call(0x028E8D88,&chainVelocity,delta.get(),&chainVelocity);
    f32 damping=gabi::load<f32>(HIO+0x18);
    gabi::call(0x028E8E64,&chainVelocity,&chainVelocity,damping);
    gabi::call(0x028E8D88,&chainPosition,&chainVelocity,&chainPosition);
    if((f32)verticalSpeed>1.0f) {
        s8 room=gabi::load<s8>(gabi::ea(this)+0x326);
        s32 reverb=gabi::call<s32>(0x02520540,room);
        gabi::call(0x025E1A40,0x3024,&current.pos,0,reverb);
    }
}
VERIFY(0x0231B904,&daBalancelift_c::calc_weight);

void daBalancelift_c::calc_quat() {
    WWHD_FUNC(0x0231BB1C,void,this);
    gabi::Local<cXyz> desired,delta,force,topDirection,baseDirection;
    gabi::Local<BalanceQuat> rotation,result;
    f32 x=current.pos.x,length=chainLength,y=current.pos.y,z=current.pos.z;
    desired->x=x; desired->z=z; desired->y=y-length;
    gabi::call(0x0201ADE0,desired.get(),delta.get(),&pivot);
    // Original reads integer words from the callee-produced temporary.
    copyWords(gabi::ea(force.get()),gabi::ea(delta.get()),3);
    f32 stiffness=gabi::load<f32>(HIO+8);
    gabi::call(0x028E8E64,force.get(),force.get(),stiffness);
    gabi::call(0x028E8D88,&pivotVelocity,force.get(),&pivotVelocity);
    f32 damping=gabi::load<f32>(HIO+0x10);
    gabi::call(0x028E8E64,&pivotVelocity,&pivotVelocity,damping);
    gabi::call(0x028E8D88,&pivot,&pivotVelocity,&pivot);
    topDirection->z=anchorTop.z; topDirection->x=anchorTop.x; topDirection->y=anchorTop.y;
    baseDirection->z=current.pos.z; baseDirection->y=current.pos.y; baseDirection->x=current.pos.x;
    gabi::call(0x028E8DAC,topDirection.get(),&pivot,topDirection.get());
    gabi::call(0x028E8DAC,baseDirection.get(),&pivot,baseDirection.get());
    gabi::call(0x02312548,rotation.get(),baseDirection.get());
    gabi::call(0x028E8BA4,rotation.get(),rotation.get());
    f32 blend=gabi::load<f32>(0x1047BC0C)+0.1f;
    gabi::call(0x028E9BC0,&supportRotation,rotation.get(),result.get(),blend);
    copyWords(gabi::ea(&supportRotation),gabi::ea(result.get()),4);
    gabi::call(0x02312548,rotation.get(),topDirection.get());
    gabi::call(0x028E8BA4,rotation.get(),rotation.get());
    blend=gabi::load<f32>(0x1047BC10)+0.15f;
    gabi::call(0x028E9BC0,&platformRotation,rotation.get(),result.get(),blend);
    copyWords(gabi::ea(&platformRotation),gabi::ea(result.get()),4);
}
VERIFY(0x0231BB1C,&daBalancelift_c::calc_quat);
namespace {
BOOL daBalanceliftExecute(daBalancelift_c* lift) {
    WWHD_FUNC(0x0231BCE4,BOOL,lift);
    u32 base=gabi::ea(lift);
    lift->calc_weight(); lift->calc_quat(); lift->set_mtx();
    gabi::call(0x024F43DC,(u8*)lift->background);
    gabi::call(0x02515E50,part<u8>(lift,0x620));
    if(gabi::call<s32>(0x025162A4,part<u8>(lift,0x640))) {
        auto* attacker=gabi::call<u8*>(0x02515BBC,part<u8>(lift,0x6D4));
        auto* hit=gabi::call<u8*>(0x02516300,part<u8>(lift,0x640));
        s32 actor=gabi::call<s32>(0x025D4604,attacker);
        gabi::Local<cXyz> direction,offset,impulse,distanceVector;
        if(actor && attacker && gabi::load<s16>(gabi::ea(attacker)+0xE)==0x126) {
            offset->x=lift->platformCenter.x;
            offset->z=lift->platformCenter.z;
            offset->y=(f32)lift->platformCenter.y-50.0f;
            gabi::call(0x0201ADE0,offset.get(),direction.get(),part<cXyz>(attacker,0x314));
            if(!gabi::call<s32>(0x0201B47C,direction.get())) {
                direction->z=1.0f; direction->y=0.0f; direction->x=0.0f;
            }
            gabi::call(0x0201AE48,direction.get(),impulse.get(),-20.0f);
            gabi::call(0x028E8D88,&lift->pivotVelocity,impulse.get(),&lift->pivotVelocity);
            f32 dy=direction->y;
            lift->verticalSpeed=(f32)lift->verticalSpeed-(dy+dy);
            gabi::call(0x020182E0,part<u8>(lift,0x758),&lift->platformCenter);
            u32 play=gabi::call<u32>(0x025200D4);
            gabi::call(0x0200E240,gabi::at<u8>(play+0x26A4),part<u8>(lift,0x640));
            return 0;
        }
        if(hit) {
            direction->y=gabi::load<f32>(base+0x704);
            direction->x=gabi::load<f32>(base+0x700);
            direction->z=gabi::load<f32>(base+0x708);
            if(!gabi::call<s32>(0x0201B47C,direction.get())) {
                direction->z=1.0f; direction->y=0.0f; direction->x=0.0f;
            }
            u32 flags=gabi::load<u32>(gabi::ea(hit)+0x10);
            if(flags&0x200000) {
                gabi::call(0x0201AE48,direction.get(),impulse.get(),-12.0f);
                gabi::call(0x028E8D88,&lift->pivotVelocity,impulse.get(),&lift->pivotVelocity);
                f32 dy=direction->y;
                lift->verticalSpeed=(f32)lift->verticalSpeed-(dy+dy);
                gabi::call(0x020182E0,part<u8>(lift,0x758),&lift->platformCenter);
                u32 play=gabi::call<u32>(0x025200D4);
                gabi::call(0x0200E240,gabi::at<u8>(play+0x26A4),part<u8>(lift,0x640));
                return 0;
            }
            if(flags&2) {
                gabi::call(0x0201AE48,direction.get(),impulse.get(),-8.0f);
                gabi::call(0x028E8D88,&lift->pivotVelocity,impulse.get(),&lift->pivotVelocity);
                f32 dy=direction->y;
                lift->verticalSpeed=(f32)lift->verticalSpeed-(dy+dy);
                auto* impact=part<cXyz>(lift,0x70C);
                if(impact) {
                    gabi::call(0x0201ADE0,impact,distanceVector.get(),&lift->platformCenter);
                    f32 square=gabi::call<f32>(0x028E8DD0,distanceVector.get());
                    f32 distance=gabi::call<f32>(0x028F4384,square);
                    f32 force=gabi::load<f32>(HIO+0x14);
                    gabi::call(0x0201AE48,direction.get(),impulse.get(),force);
                    gabi::call(0x028E8D88,&lift->chainVelocity,impulse.get(),&lift->chainVelocity);
                    lift->targetChainPosition.y=distance; lift->chainPosition.y=distance;
                    u32 play=gabi::call<u32>(0x025200D4);
                    auto* particles=gabi::at<u8>(gabi::load<u32>(play+0x5AB0));
                    gabi::call(0x025A847C,particles,0,0xC,impact,0,0,0xFF,0,-1,0,0,0);
                }
                s8 room=gabi::load<s8>(base+0x326);
                s32 reverb=gabi::call<s32>(0x02520540,room);
                gabi::call(0x025E1A40,0x6817,part<cXyz>(lift,0x37C),0,reverb);
            }
        }
    }
    gabi::call(0x020182E0,part<u8>(lift,0x758),&lift->platformCenter);
    u32 play=gabi::call<u32>(0x025200D4);
    gabi::call(0x0200E240,gabi::at<u8>(play+0x26A4),part<u8>(lift,0x640));
    return 0;
}
VERIFY(0x0231BCE4,daBalanceliftExecute);
BOOL daBalanceliftDraw(daBalancelift_c* lift) {
    WWHD_FUNC(0x0231C03C,BOOL,lift);
    u32 light=gabi::call<u32>(0x02555D0C);
    gabi::call(0x025626A4,gabi::at<u8>(light),0,&lift->current.pos,part<u8>(lift,0x110));
    light=gabi::call<u32>(0x02555D0C);
    gabi::call(0x02562F5C,gabi::at<u8>(light),(J3DModel*)lift->model,part<u8>(lift,0x110));
    gabi::call(0x025E2DE0,(J3DModel*)lift->model,0);
    u32 buffer=gabi::load<u32>(0x104B4634);
    auto* packet=(u8*)lift->chainPacket;
    gabi::call(0x027F0E04,gabi::at<u8>(buffer),packet,0);
    gabi::call(0x0251AE90,(u8*)lift->chainPacket);
    return TRUE;
}
VERIFY(0x0231C03C,daBalanceliftDraw);
u8* hioConstructor(u8* p) {
    WWHD_FUNC(0x0231C0B4,u8*,p);
    if(!p) p=gabi::call<u8*>(0x0273AD10,0x38);
    if(p) {
        u32 base=gabi::ea(p);
        gabi::store<u8>(base,0xFF);
        gabi::store<f32>(base+4,45.0f);
        gabi::store<f32>(base+0xC,0.005f);
        gabi::store<f32>(base+0x24,1800.0f);
        gabi::store<f32>(base+0x10,0.9f);
        gabi::store<f32>(base+0x1C,0.9f);
        gabi::store<f32>(base+0x30,2.0f);
        gabi::store<f32>(base+0x14,25.0f);
        gabi::store<u32>(base+0x34,0x10025454);
        gabi::store<f32>(base+8,0.05f);
        gabi::store<f32>(base+0x20,440.0f);
        gabi::store<f32>(base+0x18,0.65f);
        gabi::store<f32>(base+0x28,2.0f);
        gabi::store<f32>(base+0x2C,2.0f);
    }
    return p;
}
VERIFY(0x0231C0B4,hioConstructor);
void staticInitialize() {
    WWHD_FUNC(0x0231C174,void);
    gabi::store<u32>(0x104691D8,0);
    gabi::store<u32>(0x104691D0,0);
    gabi::store<u32>(0x104691DC,0);
    gabi::store<u32>(0x104691D4,0);
    gabi::call(0x028F026C,gabi::at<u8>(0x101C7E38));
    gabi::store<f32>(0x1046918C,-3.1415927410125732f);
    gabi::store<f32>(0x10469190,3.1415927410125732f);
    gabi::call(0x028ED6F8,gabi::at<u8>(0x10469194));
    gabi::call(0x028F026C,gabi::at<u8>(0x101C7E44));
    gabi::call(0x028EAB2C,gabi::at<u8>(0x10469195));
    gabi::call(0x028F026C,gabi::at<u8>(0x101C7E50));
    hioConstructor(gabi::at<u8>(HIO));
}
VERIFY(0x0231C174,staticInitialize);
void hioDestructor(void* p,s32 flags) {
    WWHD_FUNC(0x0231C214,void,p,flags);
    if(p && (flags&1)) gabi::call(0x0273AF40,p);
}
VERIFY(0x0231C214,hioDestructor);
BOOL daBalanceliftIsDelete(daBalancelift_c* lift) {
    WWHD_FUNC(0x0231C228,BOOL,lift);
    return TRUE;
}
VERIFY(0x0231C228,daBalanceliftIsDelete);
void actorDestructor(daBalancelift_c* lift,s32 flags) {
    WWHD_FUNC(0x0231C230,void,lift,flags);
    if(lift) {
        gabi::call(0x02515A70,part<u8>(lift,0x640),2);
        gabi::call(0x02515860,part<u8>(lift,0x604),2);
        gabi::call(0x025E99E0,part<u8>(lift,0x4B8),2);
        gabi::call(0x025D50BC,lift,0);
        if(flags&1) gabi::call(0x0273AF40,lift);
    }
}
VERIFY(0x0231C230,actorDestructor);
void emptyVirtual() { WWHD_FUNC(0x0231C2A8,void); }
VERIFY(0x0231C2A8,emptyVirtual);
}
