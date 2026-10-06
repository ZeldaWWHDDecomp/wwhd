// Forsaken Fortress warp portal. HD-derived; GC bodies are placeholders.
// See wwhd_src/README.md.
#include "d/actor/d_a_warpmj.h"
using gabi::load; using gabi::store; using gabi::call;
static u32 play() { return call<u32>(0x025200D4); }
static u32 envlight() { return call<u32>(0x02555D0C); }
static u32 actorEA(daWarpmj_c* a) { return gabi::ea(a); }
static f32 constant(u32 a) { return load<f32>(a); }
static u32 resource(s32 index) {
    gabi::Local<SafeString> name;
    name->mStringTop = 0x10042A20; name->__vtbl = 0x100428F4;
    return call<u32>(0x026066C4, load<u32>(0x101F4F28), name.get(), index);
}
static void resourceAssert(s32 line, u32 message) {
    call(0x0273AA24, STR(0x10042998), line, STR(message));
}
static void copyModelMatrix(u32 model) {
    f32 values[12];
    for (unsigned i=0;i<12;++i) values[i]=load<f32>(0x1048D0CC+4*i);
    for (unsigned i=0;i<12;++i) store<f32>(model+0xC8+4*i,values[i]);
}
static BOOL CreateHeap(daWarpmj_c* a) {
    WWHD_FUNC(0x024DCE1C, BOOL, a);
    u32 modelData=resource(9);
    if(!modelData) resourceAssert(0xAC,0x100429A8);
    a->model=gabi::at<J3DModel>(call<u32>(0x025E38E0,modelData,0x80000,0x11000222));
    if(!a->model) return 0;
    u32 animation=resource(6);
    if(!animation) resourceAssert(0xBB,0x10042974);
    u32 bck=call<u32>(0x0273AD10,0x8C);
    if(bck) {
        call(0x027F2BC0,bck,0);
        store<u32>(bck+0x10,0x1016E54C);
        call(0x027DA984,bck+0x14);
        store<u32>(bck+0x48,0x1016D820);
        store<u32>(bck+0x10,0x1004290C);
        store<u32>(bck+0x58,0);
        store<u32>(bck+0x7C,0); store<u32>(bck+0x80,0);
        store<u32>(bck+0x84,0); store<u32>(bck+0x88,0);
    }
    a->bck=gabi::at<void>(bck);
    if(!bck) return 0;
    f32 rate=constant(0x10042960);
    if(!call<s32>(0x025E8508,bck,modelData,animation,1,0,rate,0,-1,0)) return 0;
    bck=gabi::ea(a->bck.get());
    store<f32>(bck,(f32)load<s16>(bck+0xA));
    animation=resource(0xF);
    if(!animation) resourceAssert(0xCE,0x10042980);
    u32 btk=call<u32>(0x025E7C6C,0);
    a->btk=gabi::at<void>(btk);
    if(!btk) return 0;
    if(!call<s32>(0x025E7CE0,btk,modelData,animation,1,2,rate,0,-1,0,0)) return 0;
    store<f32>(gabi::ea(a->btk.get()),constant(0x10042970));
    animation=resource(0xC);
    if(!animation) resourceAssert(0xDE,0x1004298C);
    u32 brk=call<u32>(0x025E80D0,0);
    a->brk=gabi::at<void>(brk);
    if(!brk) return 0;
    if(!call<s32>(0x025E8154,brk,modelData,animation,1,0,rate,0,-1,0,0)) return 0;
    brk=gabi::ea(a->brk.get());
    store<f32>(brk,(f32)load<s16>(brk+0xA));
    return 1;
}
VERIFY(0x024DCE1C, CreateHeap);
static BOOL CheckCreateHeap(daWarpmj_c* a) {
    WWHD_FUNC(0x024DD124, BOOL, a); return CreateHeap(a);
}
VERIFY(0x024DD124, CheckCreateHeap);
static daWarpmj_c* construct(daWarpmj_c* a) {
    WWHD_FUNC(0x024DD128, daWarpmj_c*, a);
    if(!a) { a=gabi::at<daWarpmj_c>(call<u32>(0x0273AD10,0x5AC)); if(!a) return a; }
    call(0x025D4ED0,a);
    u32 base=actorEA(a);
    store<u32>(base+0xB4,0x10042934);
    // Three tev structures use the same static HD defaults.
    for(u32 off: {0x3E4u,0x4A4u,0x528u}) {
        for(u32 i=0;i<0x18;i+=4) store<f32>(base+off+i,load<f32>(0x1016E414+i));
        for(u32 i=0x18;i<0x1C;++i) store<u8>(base+off+i,load<u8>(0x1016E414+i));
        for(u32 i=0x1C;i<0x24;i+=2) store<s16>(base+off+i,load<s16>(0x1016E414+i));
        for(u32 i=0x24;i<0x44;i+=4) store<f32>(base+off+i,load<f32>(0x1016E414+i));
    }
    return a;
}
VERIFY(0x024DD128, construct);
static f32 getSeaY(daWarpmj_c* a, cXyz* position) {
    WWHD_FUNC(0x024DD2E0, f32, a, position);
    f32 z=position->z, x=position->x;
    if(call<s32>(0x0246B6A4,x,z)) { x=position->x; z=position->z; return call<f32>(0x0246BA0C,x,z); }
    return call<f32>(0x024F1478,position);
}
VERIFY(0x024DD2E0, getSeaY);
static void setEndAnm(daWarpmj_c* a) {
    WWHD_FUNC(0x024DD334, void, a);
    u32 bck=gabi::ea(a->bck.get());
    if(bck) store<f32>(bck+4,(f32)load<s16>(bck+0xA));
    u32 brk=gabi::ea(a->brk.get());
    if(brk) store<f32>(brk+4,(f32)load<s16>(brk+0xA));
}
VERIFY(0x024DD334, setEndAnm);
static void CreateInit(daWarpmj_c* a) {
    WWHD_FUNC(0x024DD3B0, void, a);
    u32 base=actorEA(a), model=gabi::ea(a->model.get());
    store<u32>(base+0x348,model?model+0xC8:0);
    f32 zero=constant(0x10042970);
    f32 low=constant(0x100429BC), high=constant(0x100429C0), top=constant(0x100429C4);
    call(0x025D674C,a,low,zero,low,high,top,high);
    f32 x=load<f32>(base+0x314), rate=constant(0x10042960), y=load<f32>(base+0x318);
    store<f32>(base+0x364,rate);
    gabi::Local<cXyz> position;
    position->x=x; position->y=y+constant(0x100429C8); position->z=load<f32>(base+0x31C);
    f32 sea=getSeaY(a,position);
    model=gabi::ea(a->model.get());
    f32 sz=load<f32>(base+0x338), sy=load<f32>(base+0x334), sx=load<f32>(base+0x330);
    if(sea==constant(0x100429CC)) sea=zero;
    store<f32>(base+0x318,sea);
    store<f32>(model+0xC0,sy); store<f32>(model+0xBC,sx); store<f32>(model+0xC4,sz);
    y=load<f32>(base+0x318); x=load<f32>(base+0x314); f32 z=load<f32>(base+0x31C);
    call(0x028E93CC,0x1048D0CC,x,y+rate,z);
    copyModelMatrix(gabi::ea(a->model.get()));
    for(unsigned i=0;i<3;++i) {
        u32 particleSystem=load<u32>(play()+0x5AB0);
        u32 kind=i==2?4:0, id=i==2?0xC3FC:0x83FD+i;
        a->particles[i]=gabi::at<void>(call<u32>(0x025A847C,particleSystem,kind,id,base+0x314,0,0,0xFF,0,-1,0,0,0));
    }
    a->order=0; setEndAnm(a);
    u32 events=play()+0x52C4;
    s32 event=call<s32>(0x02543F10,events,STR(0x100429D0),0xFF);
    s32 room=load<s8>(base+0x326); u32 destination=load<u8>(base+0xB3);
    a->eventIndex=(s16)event; a->destination=destination;
    call(0x0255FFF4,base+0x3E4,room,0xFF);
}
VERIFY(0x024DD3B0, CreateInit);
static s32 create(daWarpmj_c* a) {
    WWHD_FUNC(0x024DD660, s32, a);
    u32 base=actorEA(a), flags=load<u32>(base+0x2E4);
    if(!(flags&8)) { if(a) { construct(a); flags=load<u32>(base+0x2E4); } store<u32>(base+0x2E4,flags|8); }
    u32 save=load<u32>(0x101F84DC);
    if(!call<s32>(0x025B8B94,save+0x644,0x3D02)) return 5;
    s32 phase=call<s32>(0x02520460,&a->phase,STR(0x10042A20));
    if(phase==4) {
        if(!call<s32>(0x025D63E8,a,0x024DD124,0x3000)) return 5;
        CreateInit(a);
    }
    return phase;
}
VERIFY(0x024DD660, create);
static s32 daWarpmj_Create(daWarpmj_c* a) { WWHD_FUNC(0x024DD734,s32,a); return create(a); }
VERIFY(0x024DD734,daWarpmj_Create);
static BOOL remove(daWarpmj_c* a) {
    WWHD_FUNC(0x024DD738,BOOL,a);
    call(0x025204C8,&a->phase,STR(0x10042A20)); return 1;
}
VERIFY(0x024DD738,remove);
static BOOL daWarpmj_Delete(daWarpmj_c* a) { WWHD_FUNC(0x024DD768,BOOL,a); return remove(a); }
VERIFY(0x024DD768,daWarpmj_Delete);
static void colorToFloat(be<f32>* output, be<u8>* color) {
    WWHD_FUNC(0x024DD76C,void,output,color);
    f32 rgba[4]; for(unsigned i=0;i<4;++i) rgba[i]=(f32)(u8)color[i]/255.0f;
    for(unsigned i=0;i<4;++i) output[i]=rgba[i];
}
VERIFY(0x024DD76C,colorToFloat);
static u32 virtualMaterial(u32 material,u32 slot,s32 index) {
    u32 object=load<u32>(material+0x18), vtable=load<u32>(object+4);
    return gabi::call_ptr<u32>(load<u32>(vtable+slot),object,index);
}
static BOOL draw(daWarpmj_c* a) {
    WWHD_FUNC(0x024DD820,BOOL,a);
    u32 base=actorEA(a), env=envlight();
    call(0x025626A4,env,0,base+0x314,base+0x110);
    env=envlight(); u32 model=gabi::ea(a->model.get());
    call(0x02562F5C,env,model,base+0x110);
    env=envlight(); call(0x025626A4,env,2,base+0x314,base+0x3E4);
    model=gabi::ea(a->model.get()); u32 data=load<u32>(model+0xAC);
    u32 materialBlock=call<u32>(0x027F3F8C,data);
    u16 index=0;
    if(index<load<u16>(materialBlock+0x24)) {
        f32 rate=constant(0x10042960);
        do {
            u32 count=load<u32>(data+0xC), material=load<u32>(data+0x10);
            if(index<count) material+=index*0x39C;
            for(unsigned channel=0;channel<3;++channel) {
                u32 color=virtualMaterial(material,0x4C,1);
                store<u8>(color+channel,load<u8>(base+0x47C+channel));
            }
            u32 color=virtualMaterial(material,0x4C,1);
            u32 object=load<u32>(material+0x18), vtable=load<u32>(object+4);
            gabi::call_ptr(load<u32>(vtable+0x3C),object,1,color);
            gabi::Local<be<f32>[4]> floats;
            colorToFloat(*floats,gabi::at<be<u8>>(color));
            gabi::Local<be<f32>[4]> corrected;
            call(0x0274D458,corrected.get(),floats.get(),rate);
            u32 flags=load<u32>(material+0xA0);
            store<u32>(material+0xA0,flags|0x100);
            u32 uniform=call<u32>(0x027F9F0C,material+0xA0,8);
            f32 alpha=(f32)load<u8>(color+3)/constant(0x100429E0);
            f32 red=(*corrected)[0], green=(*corrected)[1], blue=(*corrected)[2];
            store<f32>(uniform+4,green); store<f32>(uniform+8,blue);
            store<f32>(uniform,red); store<f32>(uniform+0xC,alpha);
            ++index;
            materialBlock=call<u32>(0x027F3F8C,data);
        } while(index<load<u16>(materialBlock+0x24));
    }
    u32 animation=gabi::ea(a->btk.get());
    if(animation) {
        model=gabi::ea(a->model.get()); f32 frame=load<f32>(animation+4);
        call(0x025E7FC4,animation,load<u32>(model+0xAC),frame);
    }
    animation=gabi::ea(a->brk.get());
    if(animation) {
        model=gabi::ea(a->model.get()); f32 frame=load<f32>(animation+4);
        call(0x025E83FC,animation,load<u32>(model+0xAC),frame);
    }
    animation=gabi::ea(a->bck.get());
    if(animation) {
        model=gabi::ea(a->model.get()); f32 frame=load<f32>(animation+4);
        call(0x025E86B8,animation,load<u32>(model+0xAC),frame);
    }
    call(0x025E2DE0,a->model.get(),0); return 1;
}
VERIFY(0x024DD820,draw);
static BOOL daWarpmj_Draw(daWarpmj_c* a) { WWHD_FUNC(0x024DDA74,BOOL,a); return draw(a); }
VERIFY(0x024DDA74,daWarpmj_Draw);
static u32 animPlay(daWarpmj_c* a) {
    WWHD_FUNC(0x024DDA78,u32,a);
    u32 animation=gabi::ea(a->btk.get());
    if(animation) { store<f32>(animation,constant(0x10042960)); return call<u32>(0x025E742C,a->btk.get()); }
    return (u32)gabi::ea(a); /* original: r3 still a */
}
VERIFY(0x024DDA78,animPlay);
static BOOL check_warp(daWarpmj_c* a) {
    WWHD_FUNC(0x024DDA98,BOOL,a);
    play(); u32 player=load<u32>(play()+0x5B3C);
    if(!(load<u32>(play()+0x5CD8)&0x10000)||!player) return 0;
    gabi::Local<cXyz> delta, horizontal;
    call(0x0201ADE0,player+0x314,delta.get(),actorEA(a)+0x314);
    horizontal->x=delta->x; horizontal->y=constant(0x10042970); horizontal->z=delta->z;
    f32 magnitude=call<f32>(0x028E8DD0,horizontal.get());
    f32 distance=call<f32>(0x028F4384,magnitude);
    return distance<constant(0x100429F0);
}
VERIFY(0x024DDA98,check_warp);
static void normal_execute(daWarpmj_c* a) {
    WWHD_FUNC(0x024DDB54,void,a);
    animPlay(a); if(check_warp(a)) a->order=1;
}
VERIFY(0x024DDB54,normal_execute);
static void checkOrder(daWarpmj_c* a) {
    WWHD_FUNC(0x024DDB98,void,a);
    u32 base=actorEA(a);
    if(load<u16>(base+0xF8)==2) {
        s32 event=a->eventIndex; u32 events=play()+0x52C4;
        s32 started=call<s32>(0x0254407C,events,event);
        event=a->eventIndex;
        if(started && a->order!=0) a->order=0;
        events=play()+0x52C4;
        if(call<s32>(0x025440C8,events,event)) {
            s32 room=load<s8>(base+0x326); u32 destination=load<u8>(base+0x3D3);
            call(0x02587EFC,destination,room);
        }
    } else if(a->order==0 && !load<u8>(play()+0x5292)) normal_execute(a);
}
VERIFY(0x024DDB98,checkOrder);
// GHS pointer-to-member descriptor: this delta, virtual slot (-1 for direct), target.
static s32 invokeAction(u32 descriptor,daWarpmj_c* a) {
    s16 slot=load<s16>(descriptor+2), delta=load<s16>(descriptor);
    s32 staff=a->staff; u32 object=actorEA(a)+(s32)delta, target;
    if(slot<0) target=load<u32>(descriptor+4);
    else {
        s16 vptrOffset=load<s16>(descriptor+6);
        u32 vtable=load<u32>(object+(s32)vptrOffset);
        target=load<u32>(vtable+(u32)(s32)slot*8+4);
    }
    return gabi::call_ptr<s32>(target,object,staff);
}
static void demo_proc(daWarpmj_c* a) {
    WWHD_FUNC(0x024DDC6C,void,a);
    u32 events=play()+0x52C4;
    a->staff=call<s32>(0x02542D88,events,STR(0x100429F4),0,0);
    if(!load<u8>(play()+0x5292) || load<u16>(actorEA(a)+0xF8)==1) return;
    s32 staff=a->staff;
    if(staff==-1) return;
    events=play()+0x52C4;
    s32 action=call<s32>(0x02542EDC,events,staff,0x101D32AC,3,0,0);
    staff=a->staff;
    if(action==-1) { events=play()+0x52C4; call(0x02543280,events,staff); return; }
    events=play()+0x52C4;
    if(call<s32>(0x025447C8,events,staff)) invokeAction(0x101D325C+(u32)action*8,a);
    if(invokeAction(0x101D3274+(u32)action*8,a)) {
        staff=a->staff; events=play()+0x52C4; call(0x02543280,events,staff);
    }
}
VERIFY(0x024DDC6C,demo_proc);
static void eventOrder(daWarpmj_c* a) {
    WWHD_FUNC(0x024DDE30,void,a);
    if(a->order==1) {
        call(0x025D7A58,a,(s32)a->eventIndex,0xFF,0xFFFF,0,1);
        u32 base=actorEA(a); store<u16>(base+0xFA,load<u16>(base+0xFA)|2);
    }
}
VERIFY(0x024DDE30,eventOrder);
static void demo_execute(daWarpmj_c* a) {
    WWHD_FUNC(0x024DDE90,void,a);
    u32 base=actorEA(a), id=load<u8>(base+0x2DC);
    if(id==0||id>32) return;
    u32 demo=load<u32>(0x101D5FFC);
    if(!demo) {
        call(0x0273AA24,STR(0x10042954),0x23A,STR(0x10042944));
        demo=load<u32>(0x101D5FFC);
    }
    u32 actor=call<u32>(0x02526E70,demo,id);
    if(!actor) return;
    s32 state=load<s32>(actor+0x28); a->demoState=state;
    if(state==0) call(0x025DA884,base+0xDC);
    else if(state==1) {
        u32 queue=call<u32>(0x025DF2B8,a);
        call(0x025DA874,base+0xDC,queue); animPlay(a);
    }
}
VERIFY(0x024DDE90,demo_execute);
static BOOL execute(daWarpmj_c* a) {
    WWHD_FUNC(0x024DDF70,BOOL,a);
    u32 base=actorEA(a);
    if(!load<u8>(base+0x2DC)) { checkOrder(a); demo_proc(a); eventOrder(a); }
    else demo_execute(a);
    s32 reverb=call<s32>(0x02520540,(s32)load<s8>(base+0x326));
    call(0x025E1A40,0x1089,base+0x37C,0,reverb);
    gabi::Local<cXyz> position;
    position->x=load<f32>(base+0x314);
    position->y=load<f32>(base+0x318)+constant(0x100429C8);
    position->z=load<f32>(base+0x31C);
    f32 sea=getSeaY(a,position);
    u32 model=gabi::ea(a->model.get());
    f32 sz=load<f32>(base+0x338), sx=load<f32>(base+0x330), sy=load<f32>(base+0x334);
    if(sea==constant(0x100429CC)) sea=constant(0x10042970);
    store<f32>(base+0x318,sea);
    store<f32>(model+0xBC,sx); store<f32>(model+0xC4,sz); store<f32>(model+0xC0,sy);
    f32 y=load<f32>(base+0x318), x=load<f32>(base+0x314), z=load<f32>(base+0x31C);
    call(0x028E93CC,0x1048D0CC,x,y+constant(0x10042960),z);
    copyModelMatrix(gabi::ea(a->model.get())); return 1;
}
VERIFY(0x024DDF70,execute);
static BOOL daWarpmj_Execute(daWarpmj_c* a) { WWHD_FUNC(0x024DE150,BOOL,a); return execute(a); }
VERIFY(0x024DE150,daWarpmj_Execute);
static BOOL actWait(daWarpmj_c* a,s32 staff) {
    WWHD_FUNC(0x024DE154,BOOL,a,staff); animPlay(a); return 1;
}
VERIFY(0x024DE154,actWait);
static void initWarp(daWarpmj_c* a,s32 staff) {
    WWHD_FUNC(0x024DE178,void,a,staff);
    u32 events=play()+0x52C4; call(0x02543714,events,actorEA(a)+0x314);
    call(0x025E1988,0x2891);
}
VERIFY(0x024DE178,initWarp);
static BOOL actWarp(daWarpmj_c* a,s32 staff) {
    WWHD_FUNC(0x024DE1B8,BOOL,a,staff); animPlay(a); return 1;
}
VERIFY(0x024DE1B8,actWarp);
static void initWarpArrive(daWarpmj_c* a,s32 staff) {
    WWHD_FUNC(0x024DE1DC,void,a,staff); setEndAnm(a); call(0x025E1988,0x2894);
}
VERIFY(0x024DE1DC,initWarpArrive);
static BOOL actWarpArrive(daWarpmj_c* a,s32 staff) {
    WWHD_FUNC(0x024DE204,BOOL,a,staff); animPlay(a); return 1;
}
VERIFY(0x024DE204,actWarpArrive);
static void staticInit() {
    WWHD_FUNC(0x024DE228,void);
    store<u32>(0x1046EBC0,0); store<u32>(0x1046EBB8,0);
    store<u32>(0x1046EBC4,0); store<u32>(0x1046EBBC,0);
    call(0x028F026C,0x101D32B8);
    f32 low=constant(0x10042A18), high=constant(0x10042A1C);
    store<f32>(0x1046EBAC,low); store<f32>(0x1046EBB0,high);
    call(0x028ED6F8,0x1046EBB4); call(0x028F026C,0x101D32C4);
    call(0x028EAB2C,0x1046EBB5); call(0x028F026C,0x101D32D0);
}
VERIFY(0x024DE228,staticInit);
static void staticDestructor(void* object,s32 flags) {
    WWHD_FUNC(0x024DE2BC,void,object,flags);
    if(object&&(flags&1)) call(0x0273AF40,object);
}
VERIFY(0x024DE2BC,staticDestructor);
static BOOL daWarpmj_IsDelete(daWarpmj_c* a) { WWHD_FUNC(0x024DE2D0,BOOL,a); return 1; }
VERIFY(0x024DE2D0,daWarpmj_IsDelete);
static void initWait(daWarpmj_c* a,s32 staff) { WWHD_FUNC(0x024DE2D8,void,a,staff); }
VERIFY(0x024DE2D8,initWait);
static void destructor(daWarpmj_c* a,s32 flags) {
    WWHD_FUNC(0x024DE2DC,void,a,flags);
    if(a) { call(0x025D50BC,a,0); if(flags&1) call(0x0273AF40,a); }
}
VERIFY(0x024DE2DC,destructor);
