/* WWHD merchant display pedestal. Derived game code: */
#include "bindings.h"
namespace Dai {
struct Act : fopNpc_npc_c {
    request_of_phase_process_class displayPhase,clothPhase;
    gptr<J3DModel> model;
    u8 status[0x3C],cylinder[0x130];
    be<s16> nearPlayer,itemEvent,talkEvent;
    be<u8> selectedItem,placedItem,talking,saveID;
    u8 padding[2]; be<u32> standItemID;
};
WWHD_OFFSET(Act,displayPhase,0x7DC);
WWHD_OFFSET(Act,model,0x7EC);
WWHD_OFFSET(Act,cylinder,0x82C);
WWHD_OFFSET(Act,nearPlayer,0x95C);
WWHD_SIZE(Act,0x96C);
static u32 play() { return gabi::call<u32>(0x025200D4); }
static u32 save() { return gabi::load<u32>(0x101F84DC); }
static u16 label(u8 id) { return gabi::load<u16>(0x1004BD4C+2u*id); }
static u32 saved(u8 id) {
    u16 event=label(id);
    return gabi::call<u32>(0x025B8BB0,gabi::at<u8>(save()+0x644),event);
}
static void* findActor(u32 id) {
    if(id==0xFFFFFFFFu) return nullptr;
    gabi::Local<be<u32>> argument; *argument=id;
    gabi::Local<u8[8]> callerLinkage;
    return gabi::call<void*>(0x025D5218,gabi::at<u8>(0x025E1234),argument.get());
}
static BOOL CreateHeap(Act* actor) {
    WWHD_FUNC(0x02115DDC,BOOL,actor);
    struct ResourceName { be<u32> text,table; };
    gabi::Local<ResourceName> name;
    gabi::Local<u8[8]> callerLinkage;
    name->text=0x1000CC68; name->table=0x1000CBB4;
    void* data=gabi::call<void*>(0x026066C4,gabi::at<u8>(gabi::load<u32>(0x101F4F28)),name.get(),17);
    if(!data) gabi::call<void>(0x0273AA24,gabi::at<u8>(0x1000CBFC),0x1A2,gabi::at<u8>(0x1000CC08));
    J3DModel* model=gabi::call<J3DModel*>(0x025E38E0,data,0x80000u,0x11000022u);
    actor->model=model; return model!=nullptr;
}
VERIFY(0x02115DDC,CreateHeap);
static BOOL CheckCreateHeap(Act* actor) {
    WWHD_FUNC(0x02115E78,BOOL,actor); return CreateHeap(actor);
}
VERIFY(0x02115E78,CheckCreateHeap);
static s16 XyCheckCB(Act* actor,s32 button) {
    WWHD_FUNC(0x02115E7C,s16,actor,button);
    u8 item=gabi::load<u8>(play()+0x5BBB+(u32)button);
    if(gabi::call<BOOL>(0x02550FAC,item) && saved(actor->saveID)==0) {
        actor->selectedItem=item; return 1;
    }
    return 0;
}
VERIFY(0x02115E7C,XyCheckCB);
static s16 XyCheckWrapper(Act* actor,s32 button) {
    WWHD_FUNC(0x02115F1C,s16,actor,button); return XyCheckCB(actor,button);
}
VERIFY(0x02115F1C,XyCheckWrapper);
static s16 XyEventCB(Act* actor,s32 button) {
    WWHD_FUNC(0x02115F20,s16,actor,button);
    u8 item=gabi::load<u8>(play()+0x5BBB+(u32)button);
    return gabi::call<BOOL>(0x02550FAC,item)?(s16)actor->itemEvent:(s16)actor->talkEvent;
}
VERIFY(0x02115F20,XyEventCB);
static s16 XyEventWrapper(Act* actor,s32 button) {
    WWHD_FUNC(0x02115F78,s16,actor,button); return XyEventCB(actor,button);
}
VERIFY(0x02115F78,XyEventWrapper);
static void set_mtx(Act* actor) {
    WWHD_FUNC(0x02115F7C,void,actor);
    u32 a=gabi::ea(actor);
    f32 sx=gabi::load<f32>(a+0x330),sy=gabi::load<f32>(a+0x334);
    u32 model=gabi::ea((J3DModel*)actor->model);
    f32 sz=gabi::load<f32>(a+0x338);
    gabi::store<f32>(model+0xBC,sx); gabi::store<f32>(model+0xC0,sy); gabi::store<f32>(model+0xC4,sz);
    f32 x=gabi::load<f32>(a+0x314),y=gabi::load<f32>(a+0x318),z=gabi::load<f32>(a+0x31C);
    Mtx34* matrix=gabi::at<Mtx34>(0x1048D0CC);
    gabi::call<void>(0x028E93CC,matrix,x,y,z);
    gabi::call<void>(0x025F1C28,matrix,gabi::load<s16>(a+0x322));
    f32 values[12];
    for(u32 i=0;i<12;++i) values[i]=gabi::load<f32>(0x1048D0CC+4*i);
    model=gabi::ea((J3DModel*)actor->model);
    for(u32 i=0;i<12;++i) gabi::store<f32>(model+0xC8+4*i,values[i]);
}
VERIFY(0x02115F7C,set_mtx);
static void CreateInit(Act* actor) {
    WWHD_FUNC(0x02116054,void,actor);
    u32 a=gabi::ea(actor),model=gabi::ea((J3DModel*)actor->model);
    gabi::store<u32>(a+0x348,model?model+0xC8:0);
    gabi::call<void>(0x025D674C,actor,-50.0f,0.0f,-50.0f,50.0f,200.0f,50.0f);
    gabi::call<void>(0x02515F14,gabi::at<u8>(a+0x7F0),255,255,actor);
    gabi::call<void>(0x02516518,gabi::at<u8>(a+0x82C),gabi::at<u8>(0x101B3DB0));
    gabi::store<u32>(a+0x870,a+0x7F0);
    f32 far=gabi::load<f32>(0x1048D04C);
    if(far>1.0f) gabi::store<f32>(a+0x364,5000.0f/far);
    set_mtx(actor);
    u16 y=gabi::load<u16>(a+0x322),z=gabi::load<u16>(a+0x324),x=gabi::load<u16>(a+0x320);
    gabi::store<u16>(a+0x328,x); gabi::store<u16>(a+0x32A,y); gabi::store<u16>(a+0x32C,z);
    gabi::store<u32>(a+0x104,0x02115F1C); gabi::store<u32>(a+0x100,0x02115F78);
    gabi::store<u32>(a+0x39C,gabi::load<u32>(a+0x39C)|0x20000008u);
    actor->itemEvent=gabi::call<s16>(0x02543F10,gabi::at<u8>(play()+0x52C4),gabi::at<u8>(0x1000CC34),255);
    actor->talkEvent=gabi::call<s16>(0x02543F10,gabi::at<u8>(play()+0x52C4),gabi::at<u8>(0x1000CC40),255);
    u32 parameters=gabi::load<u32>(a+0xB0);
    actor->standItemID=0xFFFFFFFFu; actor->saveID=(u8)parameters;
    if(saved((u8)parameters)!=0) {
        u32 item=saved(actor->saveID);
        void* created=gabi::call<void*>(0x025D5928,0x1CE,item,gabi::at<cXyz>(a+0x314),gabi::load<s8>(a+0x326),gabi::at<csXyz>(a+0x320),nullptr,-1,nullptr,0);
        actor->standItemID=created?gabi::load<u32>(gabi::ea(created)+4):0xFFFFFFFFu;
        gabi::store<u8>(0x10475656,(u8)(gabi::load<u8>(0x10475656)+1));
    }
    gabi::store<u8>(0x10475655,(u8)(gabi::load<u8>(0x10475655)+1));
}
VERIFY(0x02116054,CreateInit);
static s32 create(Act* actor) {
    WWHD_FUNC(0x02116258,s32,actor);
    u32 a=gabi::ea(actor),flags=gabi::load<u32>(a+0x2E4);
    if(!(flags&8)) {
        if(actor) {
            gabi::call<void>(0x025A1458,actor);
            gabi::store<u32>(a+0xB4,0x1000CC70);
            gabi::call<void>(0x0200BD2C,gabi::at<u8>(a+0x7F0));
            gabi::call<void>(0x02515DA0,gabi::at<u8>(a+0x80C));
            gabi::store<u32>(a+0x808,0x1004AE88); gabi::store<u32>(a+0x80C,0x1004AEC0);
            gabi::call<void>(0x02515FB8,gabi::at<u8>(a+0x82C));
            gabi::store<u32>(a+0x940,0x100015A8); gabi::store<u32>(a+0x93C,0x1000CBCC);
            gabi::call<void>(0x02018590,gabi::at<u8>(a+0x944));
            gabi::store<u32>(a+0x868,0x1004B108); gabi::store<u32>(a+0x940,0x1004B160); gabi::store<u32>(a+0x958,0x1004B150);
            flags=gabi::load<u32>(a+0x2E4);
        }
        gabi::store<u32>(a+0x2E4,flags|8);
    }
    if(!gabi::call<BOOL>(0x0254DA50,0x30,1)) return 5;
    s32 phase=gabi::call<s32>(0x02520460,&actor->displayPhase,gabi::at<u8>(0x1000CC68));
    if(phase!=4) return phase;
    s32 cloth=gabi::call<s32>(0x02520460,&actor->clothPhase,gabi::at<u8>(0x1000CC60));
    if(cloth!=4) return cloth;
    if(!gabi::call<BOOL>(0x025D63E8,actor,gabi::at<u8>(0x02115E78),0x4C0)) return 5;
    CreateInit(actor); return phase;
}
VERIFY(0x02116258,create);
static s32 CreateWrapper(Act* actor) { WWHD_FUNC(0x021163BC,s32,actor); return create(actor); }
VERIFY(0x021163BC,CreateWrapper);
static BOOL remove(Act* actor) {
    WWHD_FUNC(0x021163C0,BOOL,actor);
    gabi::call<void>(0x025204C8,&actor->displayPhase,gabi::at<u8>(0x1000CC68));
    gabi::call<void>(0x025204C8,&actor->clothPhase,gabi::at<u8>(0x1000CC60));
    return TRUE;
}
VERIFY(0x021163C0,remove);
static BOOL DeleteWrapper(Act* actor) {
    WWHD_FUNC(0x0211640C,BOOL,actor); return remove(actor);
}
VERIFY(0x0211640C,DeleteWrapper);
static BOOL draw(Act* actor) {
    WWHD_FUNC(0x02116410,BOOL,actor);
    void* light=gabi::call<void*>(0x02555D0C);
    u32 a=gabi::ea(actor);
    gabi::call<void>(0x025626A4,light,0,gabi::at<cXyz>(a+0x314),gabi::at<dKy_tevstr_c>(a+0x110));
    light=gabi::call<void*>(0x02555D0C);
    gabi::call<void>(0x02562F5C,light,(J3DModel*)actor->model,gabi::at<dKy_tevstr_c>(a+0x110));
    gabi::call<void>(0x025E2DE0,(J3DModel*)actor->model,0);
    return TRUE;
}
VERIFY(0x02116410,draw);
static BOOL DrawWrapper(Act* actor) {
    WWHD_FUNC(0x0211646C,BOOL,actor); return draw(actor);
}
VERIFY(0x0211646C,DrawWrapper);
static void checkOrder(Act* actor) {
    WWHD_FUNC(0x02116470,void,actor);
    u32 player=gabi::load<u32>(play()+0x5B2C),a=gabi::ea(actor);
    if(gabi::load<u16>(a+0xF8)!=1) return;
    u32 mode=gabi::load<u8>(play()+0x52B0);
    if(mode-1u>3u) { actor->talking=1; return; }
    if(!gabi::call<BOOL>(0x02550FAC,(u8)actor->selectedItem) || saved(actor->saveID)!=0) actor->talking=1;
    s16 event=actor->itemEvent;
    if(gabi::call<BOOL>(0x0254407C,gabi::at<u8>(play()+0x52C4),event)) {
        u32 table=gabi::load<u32>(player+0xB4);
        u32 id=gabi::call_ptr<u32>(gabi::load<u32>(table+0xBC),gabi::at<u8>(player));
        if(id!=0xFFFFFFFFu) {
            table=gabi::load<u32>(player+0xB4);
            actor->standItemID=gabi::call_ptr<u32>(gabi::load<u32>(table+0xBC),gabi::at<u8>(player));
            void* item=findActor(actor->standItemID);
            if(item) actor->placedItem=gabi::load<u8>(gabi::ea(item)+0x734);
        }
    }
    event=actor->itemEvent;
    if(gabi::call<BOOL>(0x025440C8,gabi::at<u8>(play()+0x52C4),event)) {
        void* item=findActor(actor->standItemID);
        if(item) {
            for(u32 i=0;i<3;++i) gabi::store<u32>(gabi::ea(item)+0x314+4*i,gabi::load<u32>(a+0x314+4*i));
        }
        u16 eventLabel=label(actor->saveID);
        u8 placed=actor->placedItem;
        gabi::call<void>(0x025B8AF4,gabi::at<u8>(save()+0x644),eventLabel,placed);
        gabi::call<void>(0x025B7270,gabi::at<u8>(save()+0x96));
        gabi::store<u8>(0x10475656,(u8)(gabi::load<u8>(0x10475656)+1));
        u32 p=play(); gabi::store<u16>(p+0x52B8,(u16)(gabi::load<u16>(p+0x52B8)|8));
    }
}
VERIFY(0x02116470,checkOrder);
static void proc(Act* actor) {
    WWHD_FUNC(0x02116648,void,actor);
    u32 player=gabi::load<u32>(play()+0x5B2C),a=gabi::ea(actor);
    gabi::Local<cXyz> difference,horizontal;
    gabi::Local<u8[8]> callerLinkage;
    gabi::call<void>(0x0201ADE0,gabi::at<cXyz>(player+0x314),difference.get(),gabi::at<cXyz>(a+0x314));
    horizontal->x=difference->x; horizontal->y=gabi::load<f32>(0x1000CC24); horizontal->z=difference->z;
    f32 square=gabi::call<f32>(0x028E8DD0,horizontal.get());
    f64 distance=gabi::call<f64>(0x028F4384,(f64)square);
    f32 dy=gabi::load<f32>(player+0x318)-gabi::load<f32>(a+0x318);
    actor->nearPlayer=0;
    if(distance<100.0 && __builtin_fabsf(dy)<10.0f) {
        s16 angle=gabi::call<s16>(0x0200F93C,gabi::at<cXyz>(player+0x314),gabi::at<cXyz>(a+0x314));
        if(gabi::call<s32>(0x0200FAAC,angle,gabi::load<s16>(player+0x32A))<12000) actor->nearPlayer=1;
    }
    if((u8)actor->talking!=0 && gabi::call<s32>(0x025A11EC,actor,1)==0x12) {
        u32 p=play(); gabi::store<u16>(p+0x52B8,(u16)(gabi::load<u16>(p+0x52B8)|8)); actor->talking=0;
    }
}
VERIFY(0x02116648,proc);
static void eventOrder(Act* actor) {
    WWHD_FUNC(0x02116750,void,actor);
    if((s16)actor->nearPlayer==1) {
        u32 flags=gabi::ea(actor)+0xFA;
        gabi::store<u16>(flags,(u16)(gabi::load<u16>(flags)|0x21));
    }
}
VERIFY(0x02116750,eventOrder);
static BOOL execute(Act* actor) {
    WWHD_FUNC(0x0211676C,BOOL,actor);
    checkOrder(actor); proc(actor); eventOrder(actor);
    u32 a=gabi::ea(actor);
    gabi::call<void>(0x020182E0,gabi::at<u8>(a+0x944),gabi::at<cXyz>(a+0x314));
    gabi::call<void>(0x0200E240,gabi::at<u8>(play()+0x26A4),gabi::at<u8>(a+0x82C));
    return TRUE;
}
VERIFY(0x0211676C,execute);
static BOOL ExecuteWrapper(Act* actor) { WWHD_FUNC(0x021167CC,BOOL,actor); return execute(actor); }
VERIFY(0x021167CC,ExecuteWrapper);
static u32 getMsg(Act* actor) {
    WWHD_FUNC(0x021167D0,u32,actor);
    u32 mode=gabi::load<u8>(play()+0x52B0);
    if(mode-1u<=3u) return 0xF13;
    return saved(actor->saveID)?0xF0D:0xF11;
}
VERIFY(0x021167D0,getMsg);
static u16 next_msgStatus(Act* actor,be<u32>* message) {
    WWHD_FUNC(0x02116848,u16,actor,message);
    u32 current=*message,manager=gabi::load<u32>(0x101F4B5C);
    if(current==0xF11) { *message=0xF12; return 0xF; }
    if(current!=0xF0D) return 0x10;
    if(gabi::load<u32>(manager+0x948)!=0) return 0x10;
    if(gabi::call<BOOL>(0x025B75A4,gabi::at<u8>(save()+0x96)) && gabi::call<s32>(0x02550F38)<3) {
        s32 reverb=gabi::call<s32>(0x02520540,gabi::load<s8>(gabi::ea(actor)+0x326));
        gabi::call<void>(0x025E1A40,0x2871,gabi::at<cXyz>(gabi::ea(actor)+0x37C),0,reverb);
        *message=0xF0F;
        void* item=findActor(actor->standItemID);
        if(item) gabi::call<void>(0x025D57E0,item);
        u8 oldItem=(u8)saved(actor->saveID);
        u16 event=label(actor->saveID);
        gabi::call<void>(0x025B8AF4,gabi::at<u8>(save()+0x644),event,0);
        gabi::call<void>(0x025B75AC,gabi::at<u8>(save()+0x96),oldItem);
        gabi::store<u8>(0x10475656,(u8)(gabi::load<u8>(0x10475656)-1));
        actor->selectedItem=0;
    } else *message=0xF0E;
    return 0xF;
}
VERIFY(0x02116848,next_msgStatus);
static void sinit() {
    WWHD_FUNC(0x021169A8,void);
    for(u32 i=0;i<4;++i) gabi::store<u32>(0x10462F9C+4*i,0);
    gabi::call<void>(0x028F026C,gabi::at<u8>(0x101B3DF4));
    f32 low=gabi::load<f32>(0x1000CC58),high=gabi::load<f32>(0x1000CC5C);
    gabi::store<f32>(0x10462F90,low); gabi::store<f32>(0x10462F94,high);
    gabi::call<void>(0x028ED6F8,gabi::at<u8>(0x10462F98));
    gabi::call<void>(0x028F026C,gabi::at<u8>(0x101B3E00));
    gabi::call<void>(0x028EAB2C,gabi::at<u8>(0x10462F99));
    gabi::call<void>(0x028F026C,gabi::at<u8>(0x101B3E0C));
}
VERIFY(0x021169A8,sinit);
static void trivialDestructor(void* object,s32 flag) {
    WWHD_FUNC(0x02116A3C,void,object,flag);
    if(object && ((u32)flag&1)) gabi::call<void>(0x0273AF40,object);
}
VERIFY(0x02116A3C,trivialDestructor);
static void destructor(Act* actor,s32 flag) {
    WWHD_FUNC(0x02116A58,void,actor,flag);
    if(!actor) return;
    u32 a=gabi::ea(actor);
    gabi::call<void>(0x02515A70,gabi::at<u8>(a+0x82C),2);
    gabi::call<void>(0x02515860,gabi::at<u8>(a+0x7F0),2);
    gabi::call<void>(0x02515A70,gabi::at<u8>(a+0x690),2);
    gabi::call<void>(0x02515860,gabi::at<u8>(a+0x654),2);
    gabi::call<void>(0x02018034,gabi::at<u8>(a+0x628),2);
    gabi::store<u32>(a+0x470,0x1000CBDC); gabi::store<u32>(a+0x464,0x1000CBEC);
    gabi::call<void>(0x024EFD9C,gabi::at<u8>(a+0x450),0);
    gabi::call<void>(0x025D50BC,actor,0);
    if((u32)flag&1) gabi::call<void>(0x0273AF40,actor);
}
VERIFY(0x02116A58,destructor);
static BOOL IsDelete(Act* actor) { WWHD_FUNC(0x02116A50,BOOL,actor); return TRUE; }
VERIFY(0x02116A50,IsDelete);
static void empty(void* object,s32 flag) { WWHD_FUNC(0x02116B0C,void,object,flag); }
VERIFY(0x02116B0C,empty);
}
