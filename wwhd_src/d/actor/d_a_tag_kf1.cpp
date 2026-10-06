/** Kf1 event tag, WWHD reconstruction (see wwhd_src/README.md).
 * GC supplies method names but placeholder bodies; HD instructions are authoritative.
 * Actual TU: 024ABBF4..024ACAC0, 33 functions including static init and destructor.
 * Leading KbItem destructor and following Kk1 create are excluded by vtable/profile evidence.
 */
#include "d/actor/d_a_tag_kf1.h"
using gabi::load; using gabi::store; using gabi::call;
static u32 play() { return call<u32>(0x025200D4); }
// Resolve the GHS descriptor at the point of invocation, allowing callees to change fields.
static void invokeAction(u32 actor, u32 context) {
    s32 index=load<s16>(actor+0x7DE), adjust=load<s16>(actor+0x7DC);
    u32 object=actor+adjust, target;
    if(index<0) target=load<u32>(actor+0x7E0);
    else { s32 offset=load<s16>(actor+0x7E2); u32 table=load<u32>(object+offset); target=load<u32>(table+8*index+4); }
    gabi::call_ptr(target,object,context);
}
static u32 searchActor_Kutani(u32 actor,u32 context) {
    WWHD_FUNC(0x024ABBF4,u32,actor,context);
    if(load<s32>(0x1046E388)<100 && call<u32>(0x025D4604,actor) && actor && load<s16>(actor+8)==0x1C5 && call<u32>(0x02048038,actor,4,24)==14) {
        u32 count=load<u32>(0x1046E388); store<u32>(0x1046E388,count+1); store<u32>(0x1046E3BC+4*count,actor);
    }
    return 0;
}
VERIFY(0x024ABBF4,searchActor_Kutani);
static u32 set_action(u32 actor,u32 descriptor,u32 context) {
    WWHD_FUNC(0x024ABC8C,u32,actor,descriptor,context);
    s32 index=load<s16>(descriptor+2), oldIndex=load<s16>(actor+0x7DE), adjust; u32 target;
    if(oldIndex==index) {
        if(!oldIndex) return 1;
        adjust=load<s16>(descriptor); s32 oldAdjust=load<s16>(actor+0x7DC); target=load<u32>(descriptor+4);
        if(oldAdjust==adjust && load<u32>(actor+0x7E0)==target) return 1;
    } else { target=load<u32>(descriptor+4); adjust=load<s16>(descriptor); }
    if(oldIndex) { store<u8>(actor+0x87E,0xFF); invokeAction(actor,context); }
    store<u32>(actor+0x7E0,target); store<s16>(actor+0x7DC,adjust); store<s16>(actor+0x7DE,index); store<u8>(actor+0x87E,0);
    invokeAction(actor,context); return 1;
}
VERIFY(0x024ABC8C,set_action);
static u32 createInit(u32 actor) {
    WWHD_FUNC(0x024ABDB0,u32,actor);
    call(0x0259F7D4,actor+0x7E4,0x1003F5B8,actor);
    gabi::Local<be<u32>[2]> descriptor;
    (*descriptor)[0]=load<u32>(0x1003F578); (*descriptor)[1]=load<u32>(0x1003F57C);
    set_action(actor,descriptor.a,0); return 1;
}
VERIFY(0x024ABDB0,createInit);
static u32 create(u32 actor) {
    WWHD_FUNC(0x024ABE14,u32,actor);
    u32 flags=load<u32>(actor+0x2E4);
    if(!(flags&8)) { if(actor) { call(0x025A1458,actor); store<u32>(actor+0xB4,0x1003F648); call(0x0259F740,actor+0x7E4); flags=load<u32>(actor+0x2E4); } store<u32>(actor+0x2E4,flags|8); }
    if(load<s16>(actor+8)!=0x19E) return 5;
    store<u8>(actor+0x87D,0);
    if(load<s8>(0x1046E3A8)<0) { u32 id=call<u32>(0x025F0A10,0x1003F5C0,0x1046E3A8); store<u8>(0x1046E3A8,id); }
    return createInit(actor)?4:5;
}
VERIFY(0x024ABE14,create);
static u32 Create(u32 actor) { WWHD_FUNC(0x024ABF00,u32,actor); return create(actor); }
VERIFY(0x024ABF00,Create);
static u32 remove(u32 actor) {
    WWHD_FUNC(0x024ABF04,u32,actor);
    s32 id=load<s8>(0x1046E3A8); if(id>=0) { call(0x025F0A18,id); store<u8>(0x1046E3A8,0xFF); } return 1;
}
VERIFY(0x024ABF04,remove);
static u32 Delete(u32 actor) { WWHD_FUNC(0x024ABF48,u32,actor); return remove(actor); }
VERIFY(0x024ABF48,Delete);
static void checkOrder(u32 actor) {
    WWHD_FUNC(0x024ABF4C,void,actor);
    if(load<u16>(actor+0xF8)==2) { u32 events=play()+0x52C4; if(call<u32>(0x025445B8,events,0x1003F5D4) && load<s8>(actor+0x87B)==3) store<u8>(actor+0x87B,0); }
}
VERIFY(0x024ABF4C,checkOrder);
static void setStt(u32 actor,u32 state) { WWHD_FUNC(0x024ABFB4,void,actor,state); store<u8>(actor+0x87C,state); }
VERIFY(0x024ABFB4,setStt);
static void event_talkInit(u32 actor,s32 staff) {
    WWHD_FUNC(0x024ABFBC,void,actor,staff);
    u32 events=play()+0x52C4; u32 substance=call<u32>(0x0254487C,events,staff,0x1003F5DC,3);
    store<u32>(actor+0x7C8,0xFFFFFFFF);
    if(!substance) store<u32>(actor+0x7C0,0);
    else { u32 msg=load<u32>(substance); store<u32>(actor+0x7C0,msg); if(msg==0x1C2D) { s32 debt=load<s16>(actor+0x852)*10; u32 state=play(); store<u16>(state+0x5BA0,debt); } }
}
VERIFY(0x024ABFBC,event_talkInit);
static void bensyoInit(u32 actor) {
    WWHD_FUNC(0x024AC05C,void,actor);
    u32 debt=(u32)(load<s16>(actor+0x852)*10), state=play(); u32 balance=load<u32>(state+0x5B48); store<u32>(state+0x5B48,balance-debt);
    s32 cost=load<s16>(actor+0x852)*10; u32 rupees=load<u16>(actor+0x856);
    store<u32>(actor+0x7C8,0xFFFFFFFF); store<u32>(actor+0x7C0,(s32)rupees>cost?0x1C2F:0x1C30);
}
VERIFY(0x024AC05C,bensyoInit);
static void goto_nextStage(u32 actor) {
    WWHD_FUNC(0x024AC0CC,void,actor);
    u32 stage=play()+0x5134; f32 speed=load<f32>(0x1003F5E4); call(0x0252012C,stage,0,-1,-1,0,1,0,speed);
}
VERIFY(0x024AC0CC,goto_nextStage);
static s32 checkPartner(u32 actor) {
    WWHD_FUNC(0x024AC114,s32,actor);
    s32 found=0, count=load<s16>(actor+0x878);
    gabi::Local<be<u32>> id;
    for(s32 i=0;i<count;++i) { u32 value=load<u32>(actor+0x858+4*i); *id=value;
        if(value!=0xFFFFFFFF) { u32 result=call<u32>(0x025D5218,0x025E1234,id.a); count=load<s16>(actor+0x878); if(result) found=(s16)(found+1); }
    }
    return found;
}
VERIFY(0x024AC114,checkPartner);
static void event_cntTsubo(u32 actor) { WWHD_FUNC(0x024AC19C,void,actor); s32 found=checkPartner(actor); s32 count=load<s16>(actor+0x878); store<s16>(actor+0x852,count-found); }
VERIFY(0x024AC19C,event_cntTsubo);
static u32 event_mesSet(u32 actor) { WWHD_FUNC(0x024AC1D4,u32,actor); call(0x025A11EC,actor,0); return load<u32>(actor+0x7C8)!=0xFFFFFFFF; }
VERIFY(0x024AC1D4,event_mesSet);
static u32 event_mesEnd(u32 actor) { WWHD_FUNC(0x024AC218,u32,actor); return call<u32>(0x025A11EC,actor,0)==0x12; }
VERIFY(0x024AC218,event_mesEnd);
static u32 event_bensyo(u32 actor) { WWHD_FUNC(0x024AC248,u32,actor); return event_mesSet(actor); }
VERIFY(0x024AC248,event_bensyo);
static void privateCut(u32 actor) {
    WWHD_FUNC(0x024AC24C,void,actor);
    u32 events=play()+0x52C4; s32 staff=call<s32>(0x02542D88,events,0x1003F5E8,0,0); if(staff==-1) return;
    events=play()+0x52C4; s32 cut=(s8)call<u32>(0x02542EDC,events,staff,0x101D1BF8,5,1,0); store<s8>(actor+0x87A,cut);
    events=play()+0x52C4; if(cut==-1) { call(0x02543280,events,staff); return; }
    if(call<u32>(0x025447C8,events,staff)) {
        cut=load<s8>(actor+0x87A);
        if(cut==0) event_talkInit(actor,staff);
        else if(cut==2) bensyoInit(actor);
        else if(cut==3) goto_nextStage(actor);
        else if(cut==4) event_cntTsubo(actor);
        // Original reloads only after one of the four initialization callbacks.
        if(cut==0 || cut==2 || cut==3 || cut==4) cut=load<s8>(actor+0x87A);
    } else cut=load<s8>(actor+0x87A);
    bool finish;
    if((u32)cut<1) finish=event_mesSet(actor)!=0;
    else if(cut==1) finish=event_mesEnd(actor)!=0;
    else if(cut==2) finish=event_bensyo(actor)!=0;
    else finish=true;
    if(finish) { events=play()+0x52C4; call(0x02543280,events,staff); }
}
VERIFY(0x024AC24C,privateCut);
static void event_proc(u32 actor) {
    WWHD_FUNC(0x024AC3E4,void,actor);
    u32 events=play()+0x52C4;
    if(call<u32>(0x0254457C,events,0x1003F624)) { setStt(actor,2); return; }
    u32 saved=load<u8>(actor+0x844); u32 result=call<u32>(0x0259F858,actor+0x7E4);
    if(result) { if(!load<u8>(actor+0x844)) store<u8>(actor+0x844,saved); }
    else privateCut(actor);
}
VERIFY(0x024AC3E4,event_proc);
static void eventOrder(u32 actor) {
    WWHD_FUNC(0x024AC498,void,actor);
    s32 order=load<s8>(actor+0x87B);
    if(order==1 || order==2) { s32 current=load<s8>(actor+0x87B); u32 flags=load<u16>(actor+0xFA); store<u16>(actor+0xFA,flags|1); if(current==1) call(0x025D76A8,actor); }
    else if(order>=3) { u32 name=load<u32>(0x101D1C00+4*order); call(0x025D77DC,actor,name,1,0xFFFF); }
}
VERIFY(0x024AC498,eventOrder);
static u32 execute(u32 actor) {
    WWHD_FUNC(0x024AC4F8,u32,actor); checkOrder(actor); u32 state=play();
    if(load<u8>(state+0x5292) && load<u16>(actor+0xF8)!=1) event_proc(actor); else invokeAction(actor,0);
    eventOrder(actor); return 1;
}
VERIFY(0x024AC4F8,execute);
static u32 Execute(u32 actor) { WWHD_FUNC(0x024AC5AC,u32,actor); return execute(actor); }
VERIFY(0x024AC5AC,Execute);
static u32 draw(u32 actor) {
    WWHD_FUNC(0x024AC5B0,u32,actor);
    if(load<u8>(0x1046E3B4) && !load<u32>(0x101FDA50)) { store<u32>(0x101FDA50,1); call(0xC000A848,0x101FEBF4,0x1003F580,4); } return 1;
}
VERIFY(0x024AC5B0,draw);
static u32 Draw(u32 actor) { WWHD_FUNC(0x024AC610,u32,actor); return draw(actor); }
VERIFY(0x024AC610,Draw);
static u32 IsDelete(u32 actor) { WWHD_FUNC(0x024AC614,u32,actor); return 1; }
VERIFY(0x024AC614,IsDelete);
static u32 next_msgStatus(u32 actor,u32 message) {
    WWHD_FUNC(0x024AC61C,u32,actor,message); u32 number=load<u32>(message);
    if(number==0x1C30) { store<u32>(message,0x1C35); return 15; }
    if(number==0x1C35) { store<u32>(message,0x1C31); return 15; }
    if(number==0x1C2E) { u32 save=load<u32>(0x101F84DC); u32 rupees=load<u16>(save+0x24); store<u16>(actor+0x856,rupees); } return 16;
}
VERIFY(0x024AC61C,next_msgStatus);
static u32 chkAttention(u32 actor,u32 position) {
    WWHD_FUNC(0x024AC678,u32,actor,position);
    u32 state=play(), player=load<u32>(state+0x5B2C); f32 distance2=call<f32>(0x028E8DE8,position,player+0x314); f32 distance=call<f32>(0x028F4384,distance2);
    state=play(); player=load<u32>(state+0x5B2C); f32 horizontal=load<f32>(0x1046E3AC), playerY=load<f32>(player+0x318), y=load<f32>(position+4); f32 dy=(f32)((f64)playerY-(f64)y);
    // PPC bge tests !LT, so unordered NaNs fail either attention comparison.
    return distance<horizontal && dy<load<f32>(0x1046E3B0);
}
VERIFY(0x024AC678,chkAttention);
static u32 partner_srch(u32 actor) {
    WWHD_FUNC(0x024AC71C,u32,actor);
    for(u32 i=0;i<8;++i) store<u32>(actor+0x858+4*i,0xFFFFFFFF);
    store<u32>(0x1046E388,0); for(u32 i=0;i<100;++i) store<u32>(0x1046E3BC+4*i,0);
    call(0x025DE508,0x024ABBF4,actor); s32 count=load<s32>(0x1046E388); if(count>8 || count==0) return 0;
    store<s16>(actor+0x878,0); count=load<s32>(0x1046E388);
    for(s32 i=0;i<count;++i) { u32 partner=load<u32>(0x1046E3BC+4*i); u32 id=partner?load<u32>(partner+4):0xFFFFFFFF; store<u32>(actor+0x858+4*i,id); s32 saved=load<s16>(actor+0x878); store<s16>(actor+0x878,saved+1); count=load<s32>(0x1046E388); }
    return 1;
}
VERIFY(0x024AC71C,partner_srch);
static u32 wait01(u32 actor) {
    WWHD_FUNC(0x024AC80C,u32,actor); u32 attention=load<u8>(actor+0x850); store<u8>(actor+0x87B,0);
    if(attention) { u32 found=checkPartner(actor); u32 count=(u32)load<s16>(actor+0x878); if(count!=found) store<u8>(actor+0x87B,3); } return 1;
}
VERIFY(0x024AC80C,wait01);
static u32 wait_action1(u32 actor,u32 context) {
    WWHD_FUNC(0x024AC868,u32,actor,context); s32 phase=load<s8>(actor+0x87E);
    if(!phase) { setStt(actor,1); u32 value=load<u8>(actor+0x87E); store<u8>(actor+0x87E,value+1); return 1; }
    if(phase!=-1) {
        if(phase==1) { partner_srch(actor); store<u8>(actor+0x87E,2); }
        gabi::Local<cXyz> position; position->x=load<f32>(actor+0x314); position->y=load<f32>(actor+0x318); position->z=load<f32>(actor+0x31C);
        u32 attention=chkAttention(actor,position.a); s32 state=load<s8>(actor+0x87C); store<u8>(actor+0x850,attention); if(state==1) wait01(actor);
    }
    return 1;
}
VERIFY(0x024AC868,wait_action1);
static u32 HIO_ctor(u32 object) {
    WWHD_FUNC(0x024AC920,u32,object); if(!object) { object=call<u32>(0x0273AD10,0x14); if(!object) return 0; }
    store<u32>(object+0x10,0x1003F5A8); u32 word=load<u32>(0x101D1C10); store<u32>(object+4,word); word=load<u32>(0x101D1C14); store<u32>(object+8,word); word=load<u32>(0x101D1C18); store<u8>(object,0xFF); store<u32>(object+12,word); return object;
}
VERIFY(0x024AC920,HIO_ctor);
static void static_init() {
    WWHD_FUNC(0x024AC984,void); store<u32>(0x1046E3A0,0); store<u32>(0x1046E398,0); store<u32>(0x1046E3A4,0); store<u32>(0x1046E39C,0);
    call(0x028F026C,0x101D1C1C); store<f32>(0x1046E38C,load<f32>(0x1003F63C)); store<f32>(0x1046E390,load<f32>(0x1003F640));
    call(0x028ED6F8,0x1046E394); call(0x028F026C,0x101D1C28); call(0x028EAB2C,0x1046E395); call(0x028F026C,0x101D1C34); HIO_ctor(0x1046E3A8);
}
VERIFY(0x024AC984,static_init);
static void destructor(u32 actor,u32 flags) {
    WWHD_FUNC(0x024ACA24,void,actor,flags);
    if(actor) { call(0x02515A70,actor+0x690,2); call(0x02515860,actor+0x654,2); call(0x02018034,actor+0x628,2); store<u32>(actor+0x470,0x1003F588); store<u32>(actor+0x464,0x1003F598); call(0x024EFD9C,actor+0x450,0); call(0x025D50BC,actor,0); if(flags&1) call(0x0273AF40,actor); }
}
VERIFY(0x024ACA24,destructor);
