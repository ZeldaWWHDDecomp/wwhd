// Ganon's Castle warp portal. See wwhd_src/README.md.
#include "d/actor/d_a_ygcwp.h"
using gabi::load; using gabi::store; using gabi::call;
static u32 play() { return call<u32>(0x025200D4); }
static u32 environment() { return call<u32>(0x02555D0C); }
static f32 constant(u32 address) { return load<f32>(address); }
static u32 resource(u32 index) {
    gabi::Local<SafeString> name;
    name->mStringTop=0x10043010; name->__vtbl=0x10042F54;
    return call<u32>(0x026066C4,load<u32>(0x101F4F28),name.get(),index);
}
static void resourceAssert(s32 line,u32 message) {
    call(0x0273AA24,STR(0x10042FB0),line,STR(message));
}
static BOOL create_heap(daYgcwp_c* actor) {
    WWHD_FUNC(0x024E9D04,BOOL,actor);
    u32 data=resource(4);
    if(!data) { resourceAssert(0xBE,0x10042FC0); return 0; }
    actor->model=gabi::at<J3DModel>(call<u32>(0x025E38E0,data,0x80000,0x11000222));
    if(!actor->model) return 0;
    f32 speed=constant(0x10042FAC);
    for(unsigned i=0;i<2;++i) {
        u32 animation=resource(load<u32>(0x10043000+4*i));
        if(!animation) { resourceAssert(0xC9,0x10042FD0); return 0; }
        u32 mode=load<u32>(0x10043008+4*i);
        if(!call<s32>(0x025E8154,&actor->brk[i],data,animation,1,mode,speed,0,-1,0,0)) return 0;
    }
    return 1;
}
VERIFY(0x024E9D04,create_heap);
static BOOL solidHeapCB(daYgcwp_c* actor) {
    WWHD_FUNC(0x024E9E68,BOOL,actor); return create_heap(actor);
}
VERIFY(0x024E9E68,solidHeapCB);
static void copyModelMatrix(u32 model) {
    f32 matrix[12];
    for(unsigned i=0;i<12;++i) matrix[i]=load<f32>(0x1048D0CC+4*i);
    for(unsigned i=0;i<12;++i) store<f32>(model+0xC8+4*i,matrix[i]);
}
static void init_mtx(daYgcwp_c* actor) {
    WWHD_FUNC(0x024E9E6C,void,actor);
    u32 base=gabi::ea(actor),model=gabi::ea(actor->model.get());
    f32 x=load<f32>(base+0x330),y=load<f32>(base+0x334),z=load<f32>(base+0x338);
    store<f32>(model+0xBC,x); store<f32>(model+0xC0,y); store<f32>(model+0xC4,z);
    x=load<f32>(base+0x314); y=load<f32>(base+0x318); z=load<f32>(base+0x31C);
    call(0x028E93CC,0x1048D0CC,x,y,z);
    s32 rx=load<s16>(base+0x328),ry=load<s16>(base+0x32A),rz=load<s16>(base+0x32C);
    call(0x025F1B48,0x1048D0CC,rx,ry,rz);
    copyModelMatrix(gabi::ea(actor->model.get()));
}
VERIFY(0x024E9E6C,init_mtx);
static s32 create(daYgcwp_c* actor) {
    WWHD_FUNC(0x024E9F4C,s32,actor);
    u32 base=gabi::ea(actor),flags=load<u32>(base+0x2E4);
    if(!(flags&8)) {
        if(actor) {
            call(0x025D4ED0,actor);
            store<u32>(base+0xB4,0x10042F6C);
            call(0x028EFFD0,&actor->brk[0],2,0x78,0x025E80D0);
            flags=load<u32>(base+0x2E4);
        }
        store<u32>(base+0x2E4,flags|8);
    }
    s32 phase=call<s32>(0x02520460,&actor->phase,STR(0x10043010));
    if(phase!=4) return phase;
    if(!call<s32>(0x025D63E8,actor,0x024E9E68,0)) return 5;
    u32 model=gabi::ea(actor->model.get());
    store<u32>(base+0x348,model?model+0xC8:0);
    init_mtx(actor);
    u32 parameter=load<u32>(base+0xB0)&15;
    actor->parameter=parameter<3?(s32)parameter:0;
    f32 speed=constant(0x10042FDC);
    actor->brk[1].speed=speed;
    u32 save=load<u32>(0x101F84DC);
    if(!call<s32>(0x025B8B94,save+0x1178,0x480)) {
        speed=constant(0x10042FAC); actor->currentBrk=0;
    }
    actor->brk[0].speed=speed;
    save=load<u32>(0x101F84DC);
    call(0x025B8B7C,save+0x1178,0x480);
    f32 low=constant(0x10042FE0),high=constant(0x10042FE4);
    f32 bottom=constant(0x10042FE8),top=constant(0x10042FEC);
    call(0x025D674C,actor,low,bottom,low,high,top,high);
    return 4;
}
VERIFY(0x024E9F4C,create);
static BOOL remove(daYgcwp_c* actor) {
    WWHD_FUNC(0x024EA0DC,BOOL,actor);
    call(0x025204C8,&actor->phase,STR(0x10043010)); return 1;
}
VERIFY(0x024EA0DC,remove);
static void make_shine(daYgcwp_c* actor) {
    WWHD_FUNC(0x024EA10C,void,actor);
    u32 base=gabi::ea(actor),particles=load<u32>(play()+0x5AB0);
    call<u32>(0x025A847C,particles,0,0x8316,base+0x314,0,base+0x330,0xFF,0,-1,0,0,0);
}
VERIFY(0x024EA10C,make_shine);
static void set_timer(daYgcwp_c* actor) {
    WWHD_FUNC(0x024EA174,void,actor);
    s32 staff=actor->staff;
    u32 events=play()+0x52C4;
    u32 value=call<u32>(0x0254487C,events,staff,STR(0x10042FF0),3);
    actor->timer=0;
    if(value) actor->timer=load<s32>(value);
}
VERIFY(0x024EA174,set_timer);
static BOOL execute(daYgcwp_c* actor) {
    WWHD_FUNC(0x024EA1DC,BOOL,actor);
    u32 base=gabi::ea(actor),player=load<u32>(play()+0x5B2C);
    u32 events=play()+0x52C4;
    s32 staff=call<s32>(0x02542D88,events,STR(0x10043010),0,0);
    actor->staff=staff;
    if(staff!=-1) {
        events=play()+0x52C4;
        s32 action=call<s32>(0x02542EDC,events,staff,0x101D3B9C,3,0,0);
        staff=actor->staff; events=play()+0x52C4;
        if(call<s32>(0x025447C8,events,staff)) {
            f32 zero=constant(0x10042FDC),one=constant(0x10042FAC);
            if(action==0) {
                make_shine(actor);
                actor->brk[1].frame=zero; actor->brk[1].speed=one; actor->currentBrk=1;
                store<u32>(player+0x3B8,load<u32>(player+0x3B8)|0x08000000);
                set_timer(actor);
                s32 room=load<s8>(base+0x326);
                s32 reverb=call<s32>(0x02520540,room);
                call(0x025E1A40,0x288A,base+0x314,0,reverb);
            } else if(action==1) {
                set_timer(actor);
                store<u32>(player+0x3B8,load<u32>(player+0x3B8)&~0x08000000u);
            } else if(action==2) {
                actor->brk[0].frame=zero; actor->brk[0].speed=one; actor->currentBrk=0;
            }
        }
        if((u32)action<=1) {
            s32 timer=(s32)((u32)(s32)actor->timer-1u);
            actor->timer=timer;
            if(timer<=0) { staff=actor->staff; events=play()+0x52C4; call(0x02543280,events,staff); }
        }
    }
    for(unsigned i=0;i<2;++i) call<s32>(0x025E742C,&actor->brk[i]);
    return 1;
}
VERIFY(0x024EA1DC,execute);
static BOOL draw(daYgcwp_c* actor) {
    WWHD_FUNC(0x024EA3C4,BOOL,actor);
    u32 base=gabi::ea(actor),env=environment();
    call(0x025626A4,env,1,base+0x314,base+0x110);
    env=environment(); u32 model=gabi::ea(actor->model.get());
    call(0x02562F5C,env,model,base+0x110);
    for(unsigned i=0;i<2;++i) {
        if((u32)(s32)actor->currentBrk==i) {
            model=gabi::ea(actor->model.get());
            f32 frame=actor->brk[i].frame;
            u32 data=load<u32>(model+0xAC);
            call(0x025E83FC,&actor->brk[i],data,frame); break;
        }
    }
    call(0x025E2DE0,actor->model.get(),0); return 1;
}
VERIFY(0x024EA3C4,draw);
static s32 Mthd_Create(daYgcwp_c* actor) { WWHD_FUNC(0x024EA474,s32,actor); return create(actor); }
VERIFY(0x024EA474,Mthd_Create);
static BOOL Mthd_Delete(daYgcwp_c* actor) { WWHD_FUNC(0x024EA478,BOOL,actor); return remove(actor); }
VERIFY(0x024EA478,Mthd_Delete);
static BOOL Mthd_Execute(daYgcwp_c* actor) { WWHD_FUNC(0x024EA47C,BOOL,actor); return execute(actor); }
VERIFY(0x024EA47C,Mthd_Execute);
static BOOL Mthd_Draw(daYgcwp_c* actor) { WWHD_FUNC(0x024EA480,BOOL,actor); return draw(actor); }
VERIFY(0x024EA480,Mthd_Draw);
static void staticInit() {
    WWHD_FUNC(0x024EA484,void);
    store<u32>(0x1046EC4C,0); store<u32>(0x1046EC44,0);
    store<u32>(0x1046EC50,0); store<u32>(0x1046EC48,0);
    call(0x028F026C,0x101D3BA8);
    store<f32>(0x1046EC38,constant(0x10042FF8));
    store<f32>(0x1046EC3C,constant(0x10042FFC));
    call(0x028ED6F8,0x1046EC40); call(0x028F026C,0x101D3BB4);
    call(0x028EAB2C,0x1046EC41); call(0x028F026C,0x101D3BC0);
}
VERIFY(0x024EA484,staticInit);
static void SafeStringDestructor(SafeString* object,s32 flags) {
    WWHD_FUNC(0x024EA518,void,object,flags);
    if(object&&(flags&1)) call(0x0273AF40,object);
}
VERIFY(0x024EA518,SafeStringDestructor);
static void destructor(daYgcwp_c* actor,s32 flags) {
    WWHD_FUNC(0x024EA52C,void,actor,flags);
    if(actor) { call(0x025D50BC,actor,0); if(flags&1) call(0x0273AF40,actor); }
}
VERIFY(0x024EA52C,destructor);
static void SafeStringVirtual(SafeString* object) { WWHD_FUNC(0x024EA580,void,object); }
VERIFY(0x024EA580,SafeStringVirtual);
static BOOL Mthd_IsDelete(daYgcwp_c* actor) { WWHD_FUNC(0x024EA584,BOOL,actor); return 1; }
VERIFY(0x024EA584,Mthd_IsDelete);
