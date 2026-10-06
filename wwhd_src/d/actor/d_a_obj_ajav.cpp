/* WWHD Jabun's cave barrier. Derived game code: */
#include "bindings.h"
namespace Ajav {
struct Act;
struct Method { be<s16> delta,index; be<u32> target; };
struct Part {
    cXyz center,centerToOrigin,displacement,rate,flaw;
    csXyz rotation,rotationRate;
    cXyz soundPosition;
    be<u16> trigger,timer; be<s16> angleLimit; be<u8> splashed;
    u8 padding[0x11];
    cXyz particlePosition;
    gptr<J3DModel> model; gptr<dKy_tevstr_c> lighting;
    Method execute,draw;
};
WWHD_OFFSET(Part,particlePosition,0x6C);
WWHD_OFFSET(Part,execute,0x80);
WWHD_SIZE(Part,0x90);
struct Act : fopAc_ac_c {
    be<u32> switchNo; request_of_phase_process_class phase;
    u8 collision[0x520-0x3B8]; cXyz sphereOffset;
    u8 collisionRest[0x970-0x52C]; Mtx34 matrix; u8 padding[12];
    Part parts[6]; be<s16> eventIndex; be<u8> status,loaded,action,padding2;
    be<u16> cutsceneDelay; be<u8> hintHits; u8 padding3[3]; gptr<dBgW> background;
};
WWHD_OFFSET(Act,parts,0x9AC);
WWHD_OFFSET(Act,background,0xD18);
WWHD_SIZE(Act,0xD1C);
static void setMethod(Method* method,u32 target) {
    method->delta=0; method->index=-1; method->target=target;
}
static void dispatch(Part* part,Method* method,Act* actor) {
    s16 index=method->index,delta=method->delta;
    u32 self=gabi::ea(part)+(s32)delta,target;
    if(index<0) target=method->target;
    else {
        s16 offset=gabi::load<s16>(gabi::ea(method)+6);
        u32 table=gabi::load<u32>(self+(s32)offset);
        target=gabi::load<u32>(table+(s32)index*8+4);
    }
    gabi::call_ptr(target,gabi::at<Part>(self),actor);
}
static void no_proc(Part* p,Act* actor) { WWHD_FUNC(0x02313AA8,void,p,actor); }
VERIFY(0x02313AA8,no_proc);
static BOOL check_angle(be<s16>* angle,s32 limit) {
    WWHD_FUNC(0x02313ED0,BOOL,angle,limit);
    s32 value=*angle,absolute=value<0?-value:value;
    if(absolute<limit) return FALSE;
    s16 excess=(s16)((u32)absolute-(u32)limit);
    u32 reflected=(u32)limit-(u32)(s32)excess;
    *angle=(s16)(value>0?reflected:0u-reflected);
    return TRUE;
}
VERIFY(0x02313ED0,check_angle);
static void limit_angle(be<s16>* angle,s32 limit) {
    WWHD_FUNC(0x02314D7C,void,angle,limit);
    s32 value=*angle,absolute=value<0?-value:value;
    if(absolute>limit) *angle=(s16)(value>0?(u32)limit:0u-(u32)limit);
}
VERIFY(0x02314D7C,limit_angle);
static BOOL check_all_wait(Act* actor) {
    WWHD_FUNC(0x0231513C,BOOL,actor);
    for(u32 i=0;i<6;++i) {
        Method* method=&actor->parts[i].execute;
        if((s16)method->index!=-1) return FALSE;
        if((s16)method->delta!=0 || (u32)method->target!=0x02313AA8) return FALSE;
    }
    return TRUE;
}
VERIFY(0x0231513C,check_all_wait);
static BOOL check_end(Act* actor) {
    WWHD_FUNC(0x02315194,BOOL,actor);
    if((u8)actor->status!=3) return FALSE;
    return check_all_wait(actor);
}
VERIFY(0x02315194,check_end);
static void empty(void* p) { WWHD_FUNC(0x02315E44,void,p); }
VERIFY(0x02315E44,empty);
static BOOL IsDelete(Act* actor) { WWHD_FUNC(0x02315F0C,BOOL,actor); return TRUE; }
VERIFY(0x02315F0C,IsDelete);
static void trivial_dt(void* p,s32 flags) {
    WWHD_FUNC(0x02315D3C,void,p,flags);
    if(p && (flags&1)) operator_delete(p);
}
VERIFY(0x02315D3C,trivial_dt);
static void copyWords(void* destination,const void* source,u32 count) {
    for(u32 i=0;i<count;++i) gabi::store<u32>(gabi::ea(destination)+4*i,gabi::load<u32>(gabi::ea(source)+4*i));
}
static void init_data(Part* p,const cXyz* actorPosition,const cXyz* offset,dKy_tevstr_c* lighting,const cXyz* particleOffset) {
    WWHD_FUNC(0x023135F0,void,p,actorPosition,offset,lighting,particleOffset);
    copyWords(&p->center,offset,3);
    gabi::Local<cXyz> temporary; gabi::Local<u8[16]> callerLinkage1;
    gabi::call(0x0201ADE0,gabi::at<cXyz>(0x101FFBA8),(cXyz*)temporary,&p->center);
    copyWords(&p->centerToOrigin,(cXyz*)temporary,3);
    for(u32 i=0;i<3;++i) gabi::store<u16>(gabi::ea(&p->rotation)+2*i,gabi::load<u16>(0x101FFB14+2*i));
    copyWords(&p->displacement,gabi::at<cXyz>(0x101FFBA8),3);
    u32 zeroX=gabi::load<u32>(0x101FFBA8); gabi::store<u32>(gabi::ea(&p->rate),zeroX);
    u32 zeroY=gabi::load<u32>(0x101FFBAC); gabi::store<u32>(gabi::ea(&p->rate)+4,zeroY);
    u32 zeroZ=gabi::load<u32>(0x101FFBB0); p->lighting=lighting;
    gabi::store<u32>(gabi::ea(&p->rate)+8,zeroZ);
    gabi::call(0x0201AD78,particleOffset,(cXyz*)temporary,actorPosition);
    copyWords(&p->particlePosition,(cXyz*)temporary,3);
    setMethod(&p->execute,0x02313AA8); setMethod(&p->draw,0x02313AAC);
}
VERIFY(0x023135F0,init_data);
// Keep the complete SafeString above the nonleaf callee linkage area.
static void* objectResource(const char* archive,s32 index) {
    gabi::Local<SafeString> name;
    gabi::Local<u8[16]> resourceLinkage;
    name->mStringTop=gabi::ea(archive);
    name->__vtbl=0x100247C0;
    return gabi::call<void*>(0x026066C4,dComIfG_resControl(),name.get(),index);
}
static BOOL set_mdl_area(Part* p,const char* archive,s32 index,u32 flags) {
    WWHD_FUNC(0x02313714,BOOL,p,archive,index,flags);
    auto data=gabi::at<J3DModelData>(gabi::ea(objectResource(archive,index)));
    if(!data) { JUT_ASSERT_fail(STR(0x10024858),0x25C,STR(0x10024848)); return FALSE; }
    p->model=mDoExt_J3DModel__create(data,0x80000,flags); return TRUE;
}
VERIFY(0x02313714,set_mdl_area);
static void init_mtx(Part* p,const cXyz* position,const csXyz* angle,const cXyz* scale) {
    WWHD_FUNC(0x023137B0,void,p,position,angle,scale);
    u32 model=gabi::ea(p->model); f32 sy=scale->y,sx=scale->x,sz=scale->z;
    gabi::store<f32>(model+0xBC,sx); gabi::store<f32>(model+0xC0,sy); gabi::store<f32>(model+0xC4,sz);
    f32 x=position->x,y=position->y,z=position->z;
    mDoMtx_stack_c::transS(x,y,z);
    s16 ax=angle->x,ay=angle->y,az=angle->z;
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(),ax,ay,az);
    J3DModel_setBaseTRMtx(p->model,mDoMtx_stack_c::get());
    gabi::call(0x027F4D5C,(J3DModel*)p->model);
}
VERIFY(0x023137B0,init_mtx);
static void draw_normal(Part* p,Act* actor) {
    WWHD_FUNC(0x02313AAC,void,p,actor);
    auto environment=dKy_getEnvlight();
    setLightTevColorType(environment,p->model,p->lighting);
    mDoExt_modelUpdateDL(p->model);
}
VERIFY(0x02313AAC,draw_normal);
static void fall_init(Part* p,const cXyz* rate,const csXyz* rotationRate,s16 limit,u16 trigger) {
    WWHD_FUNC(0x02313E70,void,p,rate,rotationRate,limit,trigger);
    copyWords(&p->rate,rate,3);
    for(u32 i=0;i<3;++i) gabi::store<u16>(gabi::ea(&p->rotationRate)+2*i,gabi::load<u16>(gabi::ea(rotationRate)+2*i));
    setMethod(&p->execute,0x02313F24); p->splashed=0; p->timer=0; p->angleLimit=limit; p->trigger=trigger;
}
VERIFY(0x02313E70,fall_init);
static void trans(const cXyz* vector) {
    f32 x=vector->x,y=vector->y,z=vector->z;
    mDoMtx_stack_c::transM(x,y,z);
}
static void rotate(const csXyz* angle) {
    s16 x=angle->x,y=angle->y,z=angle->z;
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(),x,y,z);
}
static void finishMatrix(Part* p) {
    J3DModel_setBaseTRMtx(p->model,mDoMtx_stack_c::get());
    gabi::call(0x027F4D5C,(J3DModel*)p->model);
}
static void set_fall_mtx(Part* p,const cXyz* position,const csXyz* angle) {
    WWHD_FUNC(0x023138A4,void,p,position,angle);
    f32 x=position->x,y=position->y,z=position->z;
    mDoMtx_stack_c::transS(x,y,z); rotate(angle);
    trans(&p->displacement); trans(&p->center); rotate(&p->rotation); trans(&p->centerToOrigin);
    finishMatrix(p);
}
VERIFY(0x023138A4,set_fall_mtx);
static void set_flaw_mtx(Part* p,const cXyz* position,const csXyz* angle) {
    WWHD_FUNC(0x023139C0,void,p,position,angle);
    f32 x=position->x,y=position->y,z=position->z;
    mDoMtx_stack_c::transS(x,y,z); rotate(angle); trans(&p->displacement); finishMatrix(p);
}
VERIFY(0x023139C0,set_flaw_mtx);
static void flaw(Part* p,Act* actor) {
    WWHD_FUNC(0x02314460,void,p,actor);
    gabi::call(0x028E8D88,&p->displacement,&p->flaw,&p->displacement);
    gabi::Local<cXyz> position; gabi::Local<csXyz> angle; gabi::Local<u8[16]> callerLinkage2;
    position->x=actor->current.pos.x; position->y=actor->current.pos.y; position->z=actor->current.pos.z;
    for(u32 i=0;i<3;++i) gabi::store<u16>(gabi::ea((csXyz*)angle)+i*2,gabi::load<u16>(gabi::ea(&actor->shape_angle)+i*2));
    set_flaw_mtx(p,position,angle); setMethod(&p->execute,0x02313AA8);
}
VERIFY(0x02314460,flaw);
static BOOL draw(Act* actor) {
    WWHD_FUNC(0x02315994,BOOL,actor);
    auto environment=dKy_getEnvlight();
    settingTevStruct(environment,1,&actor->current.pos,&actor->tevStr);
    for(u32 i=0;i<6;++i) dispatch(&actor->parts[i],&actor->parts[i].draw,actor);
    return TRUE;
}
VERIFY(0x02315994,draw);
static BOOL DrawWrapper(Act* actor) { WWHD_FUNC(0x02315A58,BOOL,actor); return draw(actor); }
VERIFY(0x02315A58,DrawWrapper);
static void* statusCtor(void* p) {
    WWHD_FUNC(0x02315D50,void*,p);
    if(!p) p=gabi::call<void*>(0x0273AD10,0x3C);
    if(p) {
        gabi::call(0x0200BD2C,p); gabi::call(0x02515DA0,gabi::at<void>(gabi::ea(p)+0x1C));
        gabi::store<u32>(gabi::ea(p)+0x18,0x1004AE88); gabi::store<u32>(gabi::ea(p)+0x1C,0x1004AEC0);
    }
    return p;
}
VERIFY(0x02315D50,statusCtor);
static void* cylinderCtor(void* p) {
    WWHD_FUNC(0x02315DB8,void*,p);
    if(!p) p=gabi::call<void*>(0x0273AD10,0x130);
    if(p) {
        u32 address=gabi::ea(p);
        gabi::call(0x02515FB8,p);
        gabi::store<u32>(address+0x114,0x100015A8); gabi::store<u32>(address+0x110,0x100247D8);
        gabi::call(0x02018590,gabi::at<void>(address+0x118));
        gabi::store<u32>(address+0x3C,0x1004B108); gabi::store<u32>(address+0x12C,0x1004B150); gabi::store<u32>(address+0x114,0x1004B160);
    }
    return p;
}
VERIFY(0x02315DB8,cylinderCtor);
static void destroy(Act* actor,s32 flags) {
    WWHD_FUNC(0x02315E48,void,actor,flags);
    if(!actor) return;
    u32 address=gabi::ea(actor);
    gabi::call(0x028F0164,gabi::at<void>(address+0x710),2,0x130,0x02515A70,0,0);
    gabi::call(0x028F0164,gabi::at<void>(address+0x698),2,0x3C,0x02515860,0,0);
    gabi::call(0x02515A70,gabi::at<void>(address+0x568),2);
    gabi::call(0x02515860,gabi::at<void>(address+0x52C),2);
    gabi::call(0x02515AE8,gabi::at<void>(address+0x3F4),2);
    gabi::call(0x02515860,gabi::at<void>(address+0x3B8),2);
    gabi::call(0x025D50BC,actor,0);
    if(flags&1) operator_delete(actor);
}
VERIFY(0x02315E48,destroy);
static BOOL create_heap(Act* actor) {
    WWHD_FUNC(0x023145D8,BOOL,actor);
    BOOL result=set_mdl_area(&actor->parts[0],STR(0x100249D8),4,0x11000002);
    if(!result) return FALSE;
    for(u32 i=1;i<6;++i) {
        s32 index=gabi::load<s32>(0x100247E8+i*4);
        result=set_mdl_area(&actor->parts[i],STR(0x100249D8),index,0x15021202);
        if(!result) return FALSE;
    }
    void* resource=objectResource(STR(0x100249D8),12);
    dBgW* background=gabi::call<dBgW*>(0x024F2478,resource,1,&actor->matrix);
    actor->background=background;
    return background?result:FALSE;
}
VERIFY(0x023145D8,create_heap);
static BOOL solidHeapCB(Act* actor) { WWHD_FUNC(0x023146B4,BOOL,actor); return create_heap(actor); }
VERIFY(0x023146B4,solidHeapCB);
static void actor_init_mtx(Act* actor) {
    WWHD_FUNC(0x023146B8,void,actor);
    for(u32 i=0;i<6;++i) {
        gabi::Local<cXyz> position,scale; gabi::Local<csXyz> angle; gabi::Local<u8[16]> callerLinkage3;
        position->x=actor->current.pos.x; position->y=actor->current.pos.y; position->z=actor->current.pos.z;
        scale->x=actor->scale.x; scale->y=actor->scale.y; scale->z=actor->scale.z;
        for(u32 j=0;j<3;++j) gabi::store<u16>(gabi::ea((csXyz*)angle)+j*2,gabi::load<u16>(gabi::ea(&actor->shape_angle)+j*2));
        init_mtx(&actor->parts[i],position,angle,scale);
    }
}
VERIFY(0x023146B8,actor_init_mtx);
static void set_tex(Act* actor) {
    WWHD_FUNC(0x02314760,void,actor);
    u32 data=gabi::load<u32>(gabi::ea(actor->parts[0].model)+0xAC);
    u32 texture=gabi::load<u32>(data+0x30),names=gabi::load<u32>(data+0x34);
    for(u32 i=1;i<6;++i) {
        data=gabi::load<u32>(gabi::ea(actor->parts[i].model)+0xAC);
        gabi::store<u32>(data+0x30,texture); gabi::store<u32>(data+0x34,names);
    }
}
VERIFY(0x02314760,set_tex);
static BOOL actor_delete(Act* actor) {
    WWHD_FUNC(0x02314BDC,BOOL,actor);
    if(gabi::load<u32>(gabi::ea(actor)+0xF4)!=0) {
        u32 background=gabi::ea(actor->background);
        if(background && gabi::load<u32>(background)<0x100) {
            auto space=dComIfG_Bgsp(); gabi::call(0x020087EC,space,(dBgW*)actor->background); actor->background=nullptr;
        }
    }
    dComIfG_resDelete(&actor->phase,STR(0x100249D8));
    for(u32 i=0;i<6;++i) gabi::call(0x025E1B34,&actor->parts[i].soundPosition);
    return TRUE;
}
VERIFY(0x02314BDC,actor_delete);
static BOOL DeleteWrapper(Act* actor) { WWHD_FUNC(0x02315A50,BOOL,actor); return actor_delete(actor); }
VERIFY(0x02315A50,DeleteWrapper);
static void to_broken(Act* actor) {
    WWHD_FUNC(0x02314C78,void,actor);
    const char* name=gabi::at<char>(gabi::load<u32>(0x10024810+(u8)actor->status*4));
    auto manager=dComIfGp_getPEvtManager(); s32 event=gabi::call<s32>(0x02543F10,manager,name,255);
    u16 command=gabi::load<u16>(gabi::ea(actor)+0xF8);
    actor->eventIndex=(s16)event; actor->action=1;
    if(command!=2) {
        gabi::call(0x025D7A58,actor,(s16)actor->eventIndex,255,0xFFFF,0,1);
        u32 address=gabi::ea(actor)+0xFA; gabi::store<u16>(address,gabi::load<u16>(address)|2);
    }
}
VERIFY(0x02314C78,to_broken);
static BOOL damage_part(Act* actor) {
    WWHD_FUNC(0x02314D14,BOOL,actor);
    void* sphere=gabi::at<void>(gabi::ea(actor)+0x3F4);
    if((u8)actor->status<3 && gabi::call<BOOL>(0x025162A4,sphere)) {
        to_broken(actor); gabi::call(0x0251621C,sphere); return TRUE;
    }
    return FALSE;
}
VERIFY(0x02314D14,damage_part);
static void set_co_offset(Act* actor) {
    WWHD_FUNC(0x02314794,void,actor);
    u32 status=actor->status;
    if(status>=3) { JUT_ASSERT_fail(STR(0x100248BC),0x4A0,STR(0x100248D0)); status=actor->status; }
    copyWords(&actor->sphereOffset,gabi::at<cXyz>(0x10468F3C+status*12),3);
    gabi::Local<cXyz> center; gabi::Local<u8[16]> callerLinkage4;
    gabi::call(0x0201AD78,&actor->current.pos,(cXyz*)center,&actor->sphereOffset);
    u32 address=gabi::ea(actor);
    gabi::call(0x02018D40,gabi::at<void>(address+0x50C),(cXyz*)center);
    status=actor->status;
    gabi::call(0x02018428,gabi::at<void>(address+0x680),gabi::load<f32>(0x101C79D0+status*4));
    status=actor->status;
    for(u32 i=0;i<2;++i) {
        u32 index=status*2+i;
        gabi::call(0x0201AD78,&actor->current.pos,(cXyz*)center,gabi::at<cXyz>(0x10468FA8+index*12));
        void* cylinder=gabi::at<void>(address+0x828+i*0x130);
        gabi::call(0x020182E0,cylinder,(cXyz*)center);
        gabi::call(0x020184DC,cylinder,gabi::load<f32>(0x101C79A0+index*4));
        gabi::call(0x02018428,cylinder,gabi::load<f32>(0x101C79B8+index*4));
    }
}
VERIFY(0x02314794,set_co_offset);
static void make_hamon(Part* p,cXyz* position,f32 size) {
    WWHD_FUNC(0x02314108,void,p,position,size);
    gabi::Local<cXyz> scale; gabi::Local<be<f32>> height; gabi::Local<u8[16]> callerLinkage5;
    f32 value=2.0f+size/2000.0f; scale->x=value; scale->y=value; scale->z=value;
    position->y=(f32)position->y+10.0f;
    if(gabi::call<s32>(0x025D9F70,position,(be<f32>*)height)==1) position->y=(f32)*height+0.1f;
    dComIfGp_particle_set(0x3F,position,nullptr,scale);
}
VERIFY(0x02314108,make_hamon);
static void make_hamon2(Act* actor,cXyz* position,f32 size) {
    WWHD_FUNC(0x02314E6C,void,actor,position,size);
    gabi::Local<cXyz> scale; gabi::Local<be<f32>> height; gabi::Local<u8[16]> callerLinkage6;
    f32 value=1.0f+size/3000.0f; scale->x=value; scale->y=value; scale->z=value;
    position->y=(f32)position->y+10.0f;
    if(gabi::call<s32>(0x025D9F70,position,(be<f32>*)height)==1) position->y=(f32)*height+0.1f;
    auto emitter=dComIfGp_particle_set(0x8454,position,nullptr,scale);
    if(emitter) {
        auto environment=dKy_getEnvlight(); settingTevStruct(environment,2,&actor->current.pos,&actor->tevStr);
        u32 address=gabi::ea(actor),output=gabi::ea(emitter);
        u8 red=gabi::load<u8>(address+0x1A1),blue=gabi::load<u8>(address+0x1A5);
        gabi::store<u8>(output+0x244,red);
        u8 green=gabi::load<u8>(address+0x1A3);
        gabi::store<u8>(output+0x246,blue); gabi::store<u8>(output+0x245,green);
    }
}
VERIFY(0x02314E6C,make_hamon2);
static void make_shot_rock(Act* actor) {
    WWHD_FUNC(0x02314DAC,void,actor);
    gabi::Local<cXyz> position; gabi::Local<u8[16]> callerLinkage7;
    u32 address=gabi::ea(actor);
    position->x=gabi::load<f32>(address+0x50C); position->y=gabi::load<f32>(address+0x510); position->z=gabi::load<f32>(address+0x514);
    auto emitter=dComIfGp_particle_set(0x8426,position);
    if(emitter) {
        auto environment=dKy_getEnvlight(); settingTevStruct(environment,1,position,&actor->tevStr);
        u8 red=gabi::load<u8>(address+0x1A8),green=gabi::load<u8>(address+0x1A9),blue=gabi::load<u8>(address+0x1AA);
        u32 output=gabi::ea(emitter); gabi::store<u8>(output+0x244,red); gabi::store<u8>(output+0x245,green); gabi::store<u8>(output+0x246,blue);
    }
}
VERIFY(0x02314DAC,make_shot_rock);
static void make_fall_rock(Part* p,BOOL increased) {
    WWHD_FUNC(0x02314500,void,p,increased);
    auto emitter=dComIfGp_particle_set(0x8427,&p->particlePosition);
    if(emitter && (dKy_tevstr_c*)p->lighting) {
        if(increased) gabi::store<f32>(gabi::ea(emitter)+0x34,20.0f);
        auto environment=dKy_getEnvlight(); settingTevStruct(environment,1,&p->particlePosition,p->lighting);
        u32 lighting=gabi::ea(p->lighting),output=gabi::ea(emitter);
        u8 red=gabi::load<u8>(lighting+0x98),green=gabi::load<u8>(lighting+0x99),blue=gabi::load<u8>(lighting+0x9A);
        gabi::store<u8>(output+0x244,red); gabi::store<u8>(output+0x245,green); gabi::store<u8>(output+0x246,blue);
    }
}
VERIFY(0x02314500,make_fall_rock);
static void set_hamon(Act* actor,f32 size) {
    WWHD_FUNC(0x02314F78,void,actor,size);
    gabi::Local<cXyz> forward,side,current,temporary,position; gabi::Local<u8[16]> callerLinkage8;
    gabi::call(0x025F1884,mDoMtx_stack_c::get(),(s16)actor->current.angle.y);
    gabi::call(0x028E8F64,mDoMtx_stack_c::get(),gabi::at<cXyz>(0x101FFBCC),(cXyz*)forward);
    gabi::call(0x028E8E64,(cXyz*)forward,(cXyz*)forward,1300.0f);
    gabi::call(0x025F1884,mDoMtx_stack_c::get(),(s16)actor->current.angle.y);
    gabi::call(0x028E8F64,mDoMtx_stack_c::get(),gabi::at<cXyz>(0x101FFBB4),(cXyz*)side);
    for(u32 i=0;i<2;++i) {
        copyWords((cXyz*)current,&actor->current.pos,3); copyWords((cXyz*)temporary,(cXyz*)side,3);
        f32 factor=gabi::fmadds(-2.0f,(f32)i,1.0f)*400.0f;
        gabi::call(0x028E8E64,(cXyz*)temporary,(cXyz*)temporary,factor);
        gabi::call(0x028E8D88,(cXyz*)current,(cXyz*)forward,(cXyz*)current);
        gabi::call(0x028E8D88,(cXyz*)current,(cXyz*)temporary,(cXyz*)current);
        position->x=current->x; position->y=current->y; position->z=current->z;
        make_hamon2(actor,position,size);
    }
}
VERIFY(0x02314F78,set_hamon);
static void fall_0(Part* p,Act* actor) {
    WWHD_FUNC(0x02313F24,void,p,actor);
    gabi::call(0x0201A554,&p->rotation,&p->rotationRate);
    BOOL reflected=check_angle(&p->rotation.x,p->angleLimit);
    s16 limit=p->angleLimit;
    if(reflected) p->rotationRate.x=(s16)-(s16)p->rotationRate.x;
    if(check_angle(&p->rotation.y,limit)) p->rotationRate.y=(s16)-(s16)p->rotationRate.y;
    gabi::call(0x028E8D88,&p->displacement,&p->rate,&p->displacement);
    f32 random=gabi::call<f32>(0x02019788);
    f32 factor=gabi::fmadds(0.2f,random,0.8f);
    u16 timer=(u16)((u16)p->timer+1),trigger=p->trigger;
    f32 speedZ=(f32)p->rate.z*factor;
    p->timer=timer; p->rate.z=speedZ;
    if(timer==trigger) {
        gabi::Local<cXyz> reduced; gabi::Local<u8[16]> callerLinkage9;
        gabi::call(0x0201AE48,&p->rate,(cXyz*)reduced,0.4f);
        f32 x=reduced->x,z=reduced->z;
        setMethod(&p->execute,0x023141CC); p->timer=0;
        p->rate.x=x; p->rate.y=5.0f; p->rate.z=z;
        u32 packed=gabi::call<u32>(0x0201A588,&p->rotationRate,0.3f);
        u32 last=gabi::cpu->r[4];
        p->rotationRate.x=(s16)(packed>>16); p->rotationRate.y=(s16)packed; p->rotationRate.z=(s16)(last>>16);
        s32 sign=gabi::ftoi(-2.0f*gabi::call<f32>(0x02019788));
        s32 magnitude=gabi::ftoi(511.0f*gabi::call<f32>(0x02019788));
        p->rotationRate.z=(s16)((u32)sign*(u32)magnitude);
    }
    gabi::Local<cXyz> position; gabi::Local<csXyz> angle; gabi::Local<u8[16]> callerLinkage10;
    position->x=actor->current.pos.x; position->y=actor->current.pos.y; position->z=actor->current.pos.z;
    for(u32 i=0;i<3;++i) gabi::store<u16>(gabi::ea((csXyz*)angle)+2*i,gabi::load<u16>(gabi::ea(&actor->shape_angle)+2*i));
    set_fall_mtx(p,position,angle);
}
VERIFY(0x02313F24,fall_0);
static void fall_1(Part* p,Act* actor) {
    WWHD_FUNC(0x023141CC,void,p,actor);
    gabi::call(0x028E8D88,&p->displacement,&p->rate,&p->displacement);
    f32 elapsed=(f32)(u16)p->timer;
    p->displacement.y=(f32)p->displacement.y-(elapsed+elapsed);
    gabi::call(0x0201A554,&p->rotation,&p->rotationRate);
    f32 y=p->displacement.y,originY=p->centerToOrigin.y;
    u16 timer=p->timer; u8 splashed=p->splashed; p->timer=(u16)(timer+1);
    if(!splashed && !(y>originY-100.0f)) {
        gabi::Local<cXyz> displaced,sum,position,scale; gabi::Local<u8[16]> callerLinkage11;
        gabi::call(0x0201AD78,&actor->current.pos,(cXyz*)displaced,&p->displacement);
        gabi::call(0x0201AD78,(cXyz*)displaced,(cXyz*)sum,&p->center);
        f32 size=4.0f+(f32)p->center.y/2000.0f,x=sum->x,z=sum->z;
        position->x=x; position->y=0.0f; position->z=z;
        scale->x=size; scale->y=4.0f; scale->z=size;
        dComIfGp_particle_set(0x3C,position,nullptr,scale);
        f32 centerY=p->center.y; p->splashed=1;
        position->x=x; position->y=0.0f; position->z=z;
        make_hamon(p,position,centerY);
        p->soundPosition.x=x; p->soundPosition.y=0.0f; p->soundPosition.z=z;
        s32 reverb=dComIfGp_getReverb(actor->current.roomNo);
        mDoAud_seStart(0x6A29,&p->soundPosition,0,reverb);
        originY=p->centerToOrigin.y; y=p->displacement.y;
    }
    if(!(y>originY-1000.0f)) { setMethod(&p->execute,0x02313AA8); setMethod(&p->draw,0x02313AA8); }
    gabi::Local<cXyz> position; gabi::Local<csXyz> angle; gabi::Local<u8[16]> callerLinkage12;
    position->x=actor->current.pos.x; position->y=actor->current.pos.y; position->z=actor->current.pos.z;
    for(u32 i=0;i<3;++i) gabi::store<u16>(gabi::ea((csXyz*)angle)+2*i,gabi::load<u16>(gabi::ea(&actor->shape_angle)+2*i));
    set_fall_mtx(p,position,angle);
}
VERIFY(0x023141CC,fall_1);
static u32 getColor(u32 material) {
    u32 block=gabi::load<u32>(material+0x18),table=gabi::load<u32>(block+4),target=gabi::load<u32>(table+0x4C);
    return gabi::call_ptr<u32>(target,gabi::at<void>(block),1);
}
struct ColorFloat { be<f32> value[4]; };
static void draw_flashing(Part* p,Act* actor) {
    WWHD_FUNC(0x02313AF0,void,p,actor);
    if(!(dKy_tevstr_c*)p->lighting) return;
    auto environment=dKy_getEnvlight(); setLightTevColorType(environment,p->model,p->lighting);
    u32 model=gabi::ea(p->model),data=gabi::load<u32>(model+0xAC),joint=gabi::load<u32>(data+8),material=gabi::load<u32>(joint+0x10);
    u8 red=0,green=0,blue=0;
    if(material) {
        f32 ratio=(f32)(u16)p->timer/(f32)(u16)p->trigger;
        u16 theta=(u16)((u16)gabi::ftoi(20479.0f*ratio)+0x3000);
        f32 strength=std::fabs(gabi::load<f32>(0x104A44F8+(theta>>3)*8));
        red=gabi::load<u8>(getColor(material)); green=gabi::load<u8>(getColor(material)+1); blue=gabi::load<u8>(getColor(material)+2);
        gabi::store<u8>(getColor(material),(u8)gabi::ftoi(85.0f*strength));
        gabi::store<u8>(getColor(material)+1,(u8)gabi::ftoi(11.0f*strength));
        gabi::store<u8>(getColor(material)+2,(u8)gabi::ftoi(9.0f*strength));
        u32 color=getColor(material),block=gabi::load<u32>(material+0x18),table=gabi::load<u32>(block+4);
        gabi::call_ptr(gabi::load<u32>(table+0x3C),gabi::at<void>(block),1,gabi::at<GXColor>(color));
        gabi::Local<ColorFloat> normalized,converted; gabi::Local<u8[16]> callerLinkage13;
        gabi::call(0x0231353C,(ColorFloat*)normalized,gabi::at<GXColor>(color));
        gabi::call(0x0274D458,(ColorFloat*)converted,(ColorFloat*)normalized,1.0f);
        gabi::store<u32>(material+0xA0,gabi::load<u32>(material+0xA0)|0x100);
        u32 output=gabi::call<u32>(0x027F9F0C,gabi::at<void>(material+0xA0),8);
        f32 alpha=(f32)gabi::load<u8>(color+3)/255.0f;
        f32 x=converted->value[0],y=converted->value[1],z=converted->value[2];
        gabi::store<f32>(output,x); gabi::store<f32>(output+4,y); gabi::store<f32>(output+8,z); gabi::store<f32>(output+12,alpha);
    }
    mDoExt_modelUpdateDL(p->model);
    if(material) {
        gabi::store<u8>(getColor(material),red); gabi::store<u8>(getColor(material)+1,green); gabi::store<u8>(getColor(material)+2,blue);
    }
}
VERIFY(0x02313AF0,draw_flashing);
static void draw_flashing_normal(Part* p,Act* actor) {
    WWHD_FUNC(0x02313E0C,void,p,actor);
    draw_flashing(p,actor);
    u16 timer=(u16)((u16)p->timer+1),trigger=p->trigger; p->timer=timer;
    if(timer==trigger) setMethod(&p->draw,0x02313AAC);
}
VERIFY(0x02313E0C,draw_flashing_normal);
static void constructStatus(u32 address) {
    gabi::call(0x0200BD2C,gabi::at<void>(address));
    gabi::call(0x02515DA0,gabi::at<void>(address+0x1C));
    gabi::store<u32>(address+0x18,0x1004AE88); gabi::store<u32>(address+0x1C,0x1004AEC0);
}
static void constructCylinder(u32 address) {
    gabi::call(0x02515FB8,gabi::at<void>(address));
    gabi::store<u32>(address+0x114,0x100015A8); gabi::store<u32>(address+0x110,0x100247D8);
    gabi::call(0x02018590,gabi::at<void>(address+0x118));
    gabi::store<u32>(address+0x3C,0x1004B108); gabi::store<u32>(address+0x12C,0x1004B150); gabi::store<u32>(address+0x114,0x1004B160);
}
static s32 create(Act* actor) {
    WWHD_FUNC(0x023148B4,s32,actor);
    u32 address=gabi::ea(actor),condition=actor->actor_condition;
    if(!(condition&8)) {
        if(address) {
            gabi::call(0x025D4ED0,actor); actor->__vtbl=0x10024800;
            constructStatus(address+0x3B8); gabi::call(0x025166F0,gabi::at<void>(address+0x3F4));
            constructStatus(address+0x52C); constructCylinder(address+0x568);
            gabi::call(0x028EFFD0,gabi::at<void>(address+0x698),2,0x3C,0x02315D50);
            gabi::call(0x028EFFD0,gabi::at<void>(address+0x710),2,0x130,0x02315DB8);
            condition=actor->actor_condition;
        }
        actor->actor_condition=condition|8;
    }
    u32 switchNo=gabi::load<u8>(address+0xB3); actor->loaded=0; actor->switchNo=switchNo;
    u32 save=gabi::load<u32>(0x101F84DC);
    if(!gabi::call<BOOL>(0x025B8B94,gabi::at<void>(save+0x644),0xA02)) return 5;
    s8 room=actor->home.roomNo; save=gabi::load<u32>(0x101F84DC);
    if(gabi::call<BOOL>(0x025BA0C0,gabi::at<void>(save+0x20),(u32)actor->switchNo,room)) return 5;
    s32 phase=dComIfG_resLoad(&actor->phase,STR(0x100249D8));
    if(phase!=4) return phase;
    if(!fopAcM_entrySolidHeap(actor,0x023146B4,0)) return 5;
    for(u32 i=0;i<6;++i) {
        gabi::Local<cXyz> position,offset; gabi::Local<u8[16]> callerLinkage14;
        position->x=actor->current.pos.x; position->y=actor->current.pos.y; position->z=actor->current.pos.z;
        offset->x=gabi::load<f32>(0x10468F60+i*12); offset->y=gabi::load<f32>(0x10468F64+i*12); offset->z=gabi::load<f32>(0x10468F68+i*12);
        init_data(&actor->parts[i],position,offset,&actor->tevStr,gabi::at<cXyz>(0x10468FF0+i*12));
    }
    actor_init_mtx(actor);
    gabi::call(0x028E90D4,J3DModel_getBaseTRMtx(actor->parts[5].model),&actor->matrix); set_tex(actor);
    gabi::call(0x02515F14,gabi::at<void>(address+0x3B8),0,255,actor);
    gabi::store<u32>(address+0x438,address+0x3B8);
    gabi::call(0x0251677C,gabi::at<void>(address+0x3F4),gabi::at<void>(0x101C7960));
    gabi::call(0x02515F14,gabi::at<void>(address+0x52C),0,255,actor);
    gabi::store<u32>(address+0x5AC,address+0x52C);
    gabi::call(0x02516518,gabi::at<void>(address+0x568),gabi::at<void>(0x101C79DC));
    gabi::call(0x020182E0,gabi::at<void>(address+0x680),&actor->current.pos);
    for(u32 i=0;i<2;++i) {
        u32 status=address+0x698+i*0x3C,cylinder=address+0x710+i*0x130;
        gabi::call(0x02515F14,gabi::at<void>(status),0,255,actor);
        gabi::store<u32>(cylinder+0x44,status);
        gabi::call(0x02516518,gabi::at<void>(cylinder),gabi::at<void>(0x101C7A20));
    }
    set_co_offset(actor);
    auto space=dComIfG_Bgsp(); gabi::call(0x024EEA6C,space,(dBgW*)actor->background,actor);
    actor->hintHits=0; actor->action=0; actor->loaded=1; return phase;
}
VERIFY(0x023148B4,create);
static s32 CreateWrapper(Act* actor) { WWHD_FUNC(0x02315A4C,s32,actor); return create(actor); }
VERIFY(0x02315A4C,CreateWrapper);
static void sinit() {
    WWHD_FUNC(0x02315A5C,void,(u32)0);
    sinit_header_statics_z(0x10468F20,0x101C7A64,0x10468F2C);
    const f32 sphere[9]={0,2550,200,250,1270,200,-290,740,200};
    const f32 parts[18]={0,3000,0,500,2400,0,-300,1900,0,500,1400,0,-550,650,0,250,400,0};
    const f32 cylinders[18]={33,2541,220,465,2083,220,-137.7f,1297,220,530,511,220,-124.5f,118,220,530,118,220};
    const f32 particles[18]={-170,3087,222,563,2384,222,-371,1880,222,615,1391,222,-494,748,222,249,535,222};
    for(u32 i=0;i<9;++i) gabi::store<f32>(0x10468F3C+i*4,sphere[i]);
    for(u32 i=0;i<18;++i) gabi::store<f32>(0x10468F60+i*4,parts[i]);
    for(u32 i=0;i<18;++i) gabi::store<f32>(0x10468FA8+i*4,cylinders[i]);
    for(u32 i=0;i<18;++i) gabi::store<f32>(0x10468FF0+i*4,particles[i]);
}
VERIFY(0x02315A5C,sinit);
static Part* partAt(Act* actor,u32 index) { return gabi::at<Part>(gabi::ea(actor)+0x9AC+index*0x90); }
static BOOL execute(Act* actor) {
    WWHD_FUNC(0x023151CC,BOOL,actor);
    u32 address=gabi::ea(actor);
    if(!gabi::load<u32>(0x10469080)) {
        gabi::store<u32>(0x10469080,1);
        const f32 offsets[18]={0,0,0,0,0,0,0,40,0,30,30,0,-20,0,0,20,0,0};
        for(u32 i=0;i<18;++i) gabi::store<f32>(0x10469038+i*4,offsets[i]);
    }
    for(u32 i=0;i<6;++i) {
        Part* part=partAt(actor,i); gabi::Local<cXyz> position,sum; gabi::Local<u8[16]> callerLinkage15;
        position->x=actor->current.pos.x; position->y=actor->current.pos.y; position->z=actor->current.pos.z;
        gabi::call(0x0201AD78,(cXyz*)position,(cXyz*)sum,&part->center);
        copyWords(&part->soundPosition,(cXyz*)sum,3);
        gabi::call(0x028E8D88,&part->soundPosition,&part->displacement,&part->soundPosition);
    }
    switch((u8)actor->action) {
    case 0:
        if(!damage_part(actor)) {
            bool counted=false,broken=false;
            for(u32 i=0;i<2;++i) {
                gabi::call(0x02515E50,gabi::at<void>(address+0x6B4+i*0x3C));
                void* cylinder=gabi::at<void>(address+0x710+i*0x130);
                if(gabi::call<BOOL>(0x025162A4,cylinder)) {
                    if(!counted) {
                        u8 count=(u8)((u8)actor->hintHits+1); actor->hintHits=count;
                        if(count>=3) { to_broken(actor); actor->hintHits=0; broken=true; }
                        counted=true;
                    }
                    gabi::call(0x0251621C,cylinder);
                }
            }
            if(!broken) {
                gabi::call(0x02515E50,gabi::at<void>(address+0x548));
                void* cylinder=gabi::at<void>(address+0x568);
                if(gabi::call<BOOL>(0x025162A4,cylinder)) {
                    u32 first=(u8)actor->status*2;
                    for(u32 i=0;i<2;++i) {
                        Part* part=partAt(actor,first+i); make_fall_rock(part,TRUE);
                        part->timer=0; part->trigger=30; setMethod(&part->draw,0x02313E0C);
                    }
                    gabi::call(0x0251621C,cylinder);
                }
            }
        }
        break;
    case 1:
        if(gabi::load<u16>(address+0xF8)==2) {
            u32 first=(u8)actor->status*2;
            for(u32 i=0;i<2;++i) {
                Part* part=partAt(actor,first+i);
                gabi::Local<cXyz> offset,center,difference,rate; gabi::Local<u8[16]> callerLinkage16;
                gabi::Local<csXyz> rotation,byValueRotation; gabi::Local<u8[16]> callerLinkage17;
                offset->x=actor->sphereOffset.x; offset->y=actor->sphereOffset.y; offset->z=actor->sphereOffset.z;
                center->x=part->center.x; center->y=part->center.y; center->z=part->center.z;
                gabi::call(0x0201ADE0,(cXyz*)offset,(cXyz*)difference,(cXyz*)center);
                rotation->x=(s16)gabi::ftoi(difference->y); rotation->y=(s16)gabi::ftoi(difference->x);
                gabi::store<u16>(gabi::ea((csXyz*)rotation)+4,gabi::load<u16>(0x101FFB18));
                limit_angle(&rotation->x,511); limit_angle(&rotation->y,511);
                for(u32 j=0;j<3;++j) gabi::store<u16>(gabi::ea((csXyz*)byValueRotation)+2*j,gabi::load<u16>(gabi::ea((csXyz*)rotation)+2*j));
                rate->x=((first+i)&1)?12.0f:-12.0f; rate->y=-10.0f; rate->z=55.0f;
                u16 trigger=(u16)((u16)gabi::ftoi(9.0f*gabi::call<f32>(0x02019788))+7);
                fall_init(part,rate,byValueRotation,511,trigger);
                s32 reverb=dComIfGp_getReverb(actor->current.roomNo);
                mDoAud_seStart(0x6A28,&part->soundPosition,0,reverb);
                setMethod(&part->draw,0x02313AAC); make_fall_rock(part,FALSE);
            }
            make_shot_rock(actor);
            u32 firstNow=(u8)actor->status*2;
            set_hamon(actor,partAt(actor,firstNow)->displacement.y);
            u8 status=(u8)((u8)actor->status+1); actor->status=status;
            if(status<3) {
                for(u32 i=0;i<2;++i) {
                    u32 index=status*2+i; Part* part=partAt(actor,index);
                    copyWords(&part->flaw,gabi::at<cXyz>(0x10469038+index*12),3);
                    setMethod(&part->execute,0x02314460);
                }
                set_co_offset(actor); actor->action=2;
            } else { actor->cutsceneDelay=60; actor->action=3; }
        } else {
            gabi::call(0x025D7A58,actor,(s16)actor->eventIndex,255,0xFFFF,0,1);
            gabi::store<u16>(address+0xFA,gabi::load<u16>(address+0xFA)|2);
        }
        break;
    case 2: {
        s16 event=actor->eventIndex; auto manager=dComIfGp_getPEvtManager();
        if(gabi::call<BOOL>(0x025440C8,manager,event)) {
            auto play=dComIfGp_get(); u32 flagAddress=gabi::ea(play)+0x52B8;
            gabi::store<u16>(flagAddress,gabi::load<u16>(flagAddress)|8);
            if((u8)actor->status<3) actor->action=0; else actor->action=4;
        }
        break;
    }
    case 3:
        if(check_end(actor)) {
            u16 delay=actor->cutsceneDelay;
            if(delay) actor->cutsceneDelay=(u16)(delay-1);
            else {
                auto manager=dComIfGp_getPEvtManager(); s32 staff=gabi::call<s32>(0x02542D88,manager,STR(0x10024934),0,0);
                manager=dComIfGp_getPEvtManager(); gabi::call(0x02543280,manager,staff);
                if(gabi::load<u32>(address+0xF4)) {
                    u32 background=gabi::ea(actor->background);
                    if(background && gabi::load<u32>(background)<0x100) {
                        auto space=dComIfG_Bgsp(); gabi::call(0x020087EC,space,(dBgW*)actor->background); actor->background=nullptr;
                    }
                }
                s8 room=actor->home.roomNo; u32 save=gabi::load<u32>(0x101F84DC);
                gabi::call(0x025B9E38,gabi::at<void>(save+0x20),(u32)actor->switchNo,room);
                gabi::call(0x025E1988,0x806); gabi::call(0x025E1928);
                u8 status=(u8)((u8)actor->status+1); actor->action=2; actor->status=status;
            }
        }
        break;
    case 4: break;
    default: actor->action=0; break;
    }
    if((u8)actor->status<3) {
        auto space=dComIfG_Ccsp(); gabi::call(0x0200E240,space,gabi::at<void>(address+0x3F4));
        space=dComIfG_Ccsp(); gabi::call(0x0200E240,space,gabi::at<void>(address+0x568));
        for(u32 i=0;i<2;++i) { space=dComIfG_Ccsp(); gabi::call(0x0200E240,space,gabi::at<void>(address+0x710+i*0x130)); }
    }
    for(u32 i=0;i<6;++i) { Part* part=partAt(actor,i); dispatch(part,&part->execute,actor); }
    u32 background=gabi::ea(actor->background);
    if(background && gabi::load<u32>(background)<0x100) gabi::call(0x024F43DC,gabi::at<dBgW>(background));
    return TRUE;
}
VERIFY(0x023151CC,execute);
static BOOL ExecuteWrapper(Act* actor) { WWHD_FUNC(0x02315A54,BOOL,actor); return execute(actor); }
VERIFY(0x02315A54,ExecuteWrapper);
}
