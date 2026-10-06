/* WWHD Deku Leaf foliage pile. Derived game code: */
#include "bindings.h"
struct daObjLeaves_c : fopAc_ac_c {
    request_of_phase_process_class phase;
    gptr<J3DModel> model;
    gptr<dBgW> bg;
    dCcD_Stts status;
    dCcD_Sph sphere;
    be<f32> alpha, alphaStep;
    u8 smoke[0x20];
    be<s32> fadeDelay, retireDelay, effectKind;
    be<u8> appeared;
    u8 pad559[3];
    be<s16> actionDelta, actionIndex;
    be<u32> actionTarget;
    be<u8> hit;
    u8 pad565[3];
    void birthEffect(s32, cXyz*, csXyz*, GXColor*);
    u32 chk_appear(); bool create_heap(); void init_mtx();
    s32 create(); bool remove(); bool execute(); bool draw();
    void wait_proc(); void alpha_calc_start_wait_proc();
    void item_set_wait_proc(); u32 retire_wait_proc();
};
WWHD_OFFSET(daObjLeaves_c, sphere, 0x3F8);
WWHD_OFFSET(daObjLeaves_c, smoke, 0x52C);
WWHD_OFFSET(daObjLeaves_c, actionTarget, 0x560);
WWHD_SIZE(daObjLeaves_c, 0x568);
static const char* arc() { return STR(0x1002BF4C); }
static GXColor* tint(daObjLeaves_c* p) { return gabi::at<GXColor>(gabi::ea(p)+0x1A8); }
static void changeAction(daObjLeaves_c* p, u32 target) {
    p->actionIndex = -1; p->actionTarget = target; p->actionDelta = 0;
}
static void soundCurrent(daObjLeaves_c* p, cXyz* pos, u32 sound) {
    if (gabi::ea(p) != 0 && gabi::ea(pos) != 0) {
        s8 room = gabi::load<s8>(gabi::ea(pos)+0x12);
        s32 reverb = dComIfGp_getReverb(room);
        mDoAud_seStart(sound, pos, 0, reverb);
    }
}
void daObjLeaves_c::birthEffect(s32 index, cXyz* pos, csXyz* angle, GXColor* color) {
    WWHD_FUNC(0x02369D08, void, this, index, pos, angle, color);
    if ((u32)index >= 6) {
        JUT_ASSERT_fail(STR(0x1002BF0C), 0x1AE, STR(0x1002BF08));
        return;
    }
    u32 entry = 0x101CA874 + 0x14*index;
    effectKind = gabi::load<s32>(entry+4);
    s32 kind = gabi::load<s32>(entry+4);
    if (kind == 0) {
        u16 id = gabi::load<u16>(entry);
        dComIfGp_particle_set(id, pos, angle, nullptr, 255, nullptr, -1, color);
    } else if (kind == 1) {
        gabi::call(0x025A5F88, smoke);
        u16 id = gabi::load<u16>(entry);
        dPa_control_c* particles = dComIfGp_getParticle();
        dPa_control_set(particles, 2, id, pos, angle, nullptr, 160,
                       gabi::at<dPa_levelEcallBack>(gabi::ea(smoke)), -1, nullptr, nullptr, nullptr);
    } else {
        JUT_ASSERT_fail(STR(0x1002BF0C), 0x1D6, STR(0x1002BF08));
    }
    fadeDelay = gabi::load<s32>(entry+8);
    retireDelay = gabi::load<s32>(entry+12);
    alphaStep = gabi::load<f32>(entry+16);
}
VERIFY(0x02369D08, &daObjLeaves_c::birthEffect);
static void rideCallBack(dBgW* bg, daObjLeaves_c* p, fopAc_ac_c* rider) {
    WWHD_FUNC(0x02369E64, void, bg, p, rider);
    if (rider == nullptr || gabi::load<s16>(gabi::ea(rider)+8) != 0xA8) return;
    if (rider->speedF < 8.0f) return;
    if (!(gabi::load<u32>(gabi::ea(rider)+0x3C0)&0xC00)) return;
    p->birthEffect(2, &rider->current.pos, nullptr, tint(p));
}
VERIFY(0x02369E64, rideCallBack);
struct LinearColor_l { be<f32> channel[4]; };
static void colorToFloat(LinearColor_l* out, GXColor* color) {
    WWHD_FUNC(0x02369EB0, void, out, color);
    f32 values[4];
    for (u32 i=0; i<4; ++i) values[i] = (f32)gabi::load<u8>(gabi::ea(color)+i)/255.0f;
    for (u32 i=0; i<4; ++i) out->channel[i] = values[i];
}
VERIFY(0x02369EB0, colorToFloat);
static u32 PrmAbstract(daObjLeaves_c* p, u32 width, u32 shift) {
    WWHD_FUNC(0x0236AF4C, u32, p, width, shift);
    u32 parameters = p->mParameters;
    u32 mask = (width&32) ? 0 : (1U<<(width&31));
    u32 value = (shift&32) ? 0 : (parameters>>(shift&31));
    return value&(mask-1);
}
VERIFY(0x0236AF4C, PrmAbstract);
u32 daObjLeaves_c::chk_appear() {
    WWHD_FUNC(0x02369F64, u32, this);
    s32 sw = PrmAbstract(this, 8, 13);
    if (sw == 255) return true;
    return !dComIfGs_isSwitch(sw, home.roomNo);
}
VERIFY(0x02369F64, &daObjLeaves_c::chk_appear);
bool daObjLeaves_c::create_heap() {
    WWHD_FUNC(0x02369FC8, bool, this);
    auto data = static_cast<J3DModelData*>(dComIfG_getObjectRes(arc(), 4, 0x1002BF54));
    if (data == nullptr) {
        JUT_ASSERT_fail(STR(0x1002BFC0), 0x226, STR(0x1002BFBC));
        return false;
    }
    model = mDoExt_J3DModel__create(data, 0x80000, 0x31000202);
    auto collision = static_cast<cBgD_t*>(dComIfG_getObjectRes(arc(), 7, 0x1002BF54));
    Mtx34* matrix = J3DModel_getBaseTRMtx(model);
    dBgW* created = gabi::call<dBgW*>(0x024F2478, collision, 1, matrix);
    bool hasModel = model != nullptr;
    bg = created;
    if (!hasModel || created == nullptr) return false;
    gabi::store<u32>(gabi::ea(created)+0xB0, 0x02369E64);
    return true;
}
VERIFY(0x02369FC8, &daObjLeaves_c::create_heap);
static BOOL solidHeapCB(daObjLeaves_c* p) {
    WWHD_FUNC(0x0236A0BC, BOOL, p);
    return p->create_heap();
}
VERIFY(0x0236A0BC, solidHeapCB);
void daObjLeaves_c::init_mtx() {
    WWHD_FUNC(0x0236A0C0, void, this);
    f32 x=scale.x, y=scale.y, z=scale.z;
    J3DModel* m=model;
    gabi::store<f32>(gabi::ea(m)+0xBC,x);
    gabi::store<f32>(gabi::ea(m)+0xC0,y);
    gabi::store<f32>(gabi::ea(m)+0xC4,z);
    mDoMtx_stack_c::transS(current.pos.x,current.pos.y,current.pos.z);
    J3DModel_setBaseTRMtx(model,mDoMtx_stack_c::get());
}
VERIFY(0x0236A0C0, &daObjLeaves_c::init_mtx);
static void tg_hitCallback(daObjLeaves_c* p,dCcD_GObjInf* sphere,fopAc_ac_c* attacker,dCcD_GObjInf* other) {
    WWHD_FUNC(0x0236A18C, void, p,sphere,attacker,other);
    void* object=sphere->GetTgHitObj();
    if (object==nullptr) return;
    u32 type=gabi::load<u32>(gabi::ea(object)+0x10);
    cXyz* pos=&p->current.pos;
    if (type==0x200000) {
        fopAc_ac_c* player=gabi::at<fopAc_ac_c>(gabi::load<u32>(dComIfGp_ea()+0x5B2C));
        f32 x=p->current.pos.x-player->current.pos.x;
        f32 z=p->current.pos.z-player->current.pos.z;
        s16 yaw=gabi::call<s16>(0x020195B0,x,z);
        gabi::Local<csXyz> angle;
        angle->x=0; angle->y=yaw; angle->z=0;
        p->birthEffect(0,pos,angle,tint(p));
        p->birthEffect(1,pos,angle,nullptr);
        soundCurrent(p,pos,0x69EF);
        p->hit=1;
        return;
    }
    if (type==0x20) {
        p->birthEffect(5,pos,nullptr,tint(p));
        soundCurrent(p,pos,0x69EF);
        p->hit=1;
        return;
    }
    gabi::Local<cXyz> difference;
    gabi::call(0x0201ADE0,&p->sphere.mGObjTg.mHitPos,difference.get(),pos);
    gabi::Local<cXyz> horizontal;
    horizontal->x=difference->x; horizontal->y=0.0f; horizontal->z=difference->z;
    f32 squared=gabi::call<f32>(0x028E8DD0,horizontal.get());
    f32 distance=gabi::call<f32>(0x028F4384,squared);
    if (!(distance<110.0f)) return;
    switch (type) {
    case 2: case 8: case 0x40: case 0x80: case 0x400: case 0x800: case 0x1000:
    case 0x2000: case 0x4000: case 0x8000: case 0x80000: case 0x100000:
    case 0x1000000: case 0x2000000: case 0x4000000: case 0x10000000:
        p->birthEffect(2,&p->sphere.mGObjTg.mHitPos,nullptr,tint(p));
        break;
    case 0x10000:
        p->birthEffect(5,pos,nullptr,tint(p));
        soundCurrent(p,pos,0x69EF);
        break;
    case 0x200: case 0x20000: case 0x40000:
        p->birthEffect(3,pos,nullptr,nullptr);
        p->birthEffect(4,pos,nullptr,nullptr);
        soundCurrent(p,pos,0x69F0);
        p->hit=1;
        break;
    }
}
VERIFY(0x0236A18C,tg_hitCallback);
s32 daObjLeaves_c::create() {
    WWHD_FUNC(0x0236A554,s32,this);
    if (!(actor_condition&8)) {
        if (gabi::ea(this)!=0) {
            fopAc_ac_c_ct(this); __vtbl=0x1002BF6C;
            dCcD_Stts_ct(&status);
            gabi::call(0x025166F0,&sphere);
            gabi::call(0x025A5B18,smoke,1);
        }
        actor_condition=actor_condition|8;
    }
    s32 result=5;
    u32 isVisible;
    if (gabi::load<u8>(gabi::ea(this)+0xC)==0) {
        isVisible=chk_appear(); appeared=(u8)isVisible;
    } else isVisible=appeared;
    if (isVisible) result=dComIfG_resLoad(&phase,arc());
    if (result==4) {
        if (!fopAcM_entrySolidHeap(this,0x0236A0BC,0xC20)) return 5;
        dBgS* bgs=dComIfG_Bgsp();
        if (dBgS_Regist(bgs,bg,this)) return 5;
        cullMtx=gabi::ea(J3DModel_getBaseTRMtx(model));
        init_mtx(); status.Init(255,255,this);
        sphere.Set(gabi::at<dCcD_SrcSph>(0x1002BF7C));
        f32 x=current.pos.x,y=current.pos.y-115.0f,z=current.pos.z;
        sphere.SetStts(&status);
        gabi::call(0x02018F3C,&sphere.mSph,x,y,z);
        sphere.SetR(150.0f);
        gabi::store<u8>(gabi::ea(smoke)+0x11,0);
        alpha=255.0f;
        actionTarget=0x0236ABD0;
        gabi::store<u8>(gabi::ea(smoke)+0x12,1);
        actionIndex=-1;
        sphere.mGObjTg.mHitCallback=0x0236A18C;
        gabi::store<u32>(gabi::ea(smoke)+0x1C,gabi::ea(&tevStr));
        actionDelta=0;
    }
    return result;
}
VERIFY(0x0236A554,&daObjLeaves_c::create);
bool daObjLeaves_c::remove() {
    WWHD_FUNC(0x0236A74C,bool,this);
    if (appeared) {
        dComIfG_resDelete(&phase,arc());
        u32 vt=gabi::load<u32>(gabi::ea(smoke));
        gabi::call_ptr(gabi::load<u32>(vt+0x44),smoke);
        if (heap!=nullptr && bg!=nullptr) {
            dBgW* w=bg;
            if (dBgW_ChkUsed(w)) {
                dBgS* bgs=dComIfG_Bgsp();
                cBgS_Release(bgs,bg);
            }
            bg=nullptr;
        }
    }
    return true;
}
VERIFY(0x0236A74C,&daObjLeaves_c::remove);
bool daObjLeaves_c::execute() {
    WWHD_FUNC(0x0236A7E8,bool,this);
    s32 fade=(s32)((u32)fadeDelay-1); s32 retire=(s32)((u32)retireDelay-1);
    fadeDelay=fade>0 ? fade : 0;
    retireDelay=retire>0 ? retire : 0;
    ptmf_call(gabi::ea(&actionDelta),this);
    return true;
}
VERIFY(0x0236A7E8,&daObjLeaves_c::execute);
void daObjLeaves_c::wait_proc() {
    WWHD_FUNC(0x0236ABD0,void,this);
    dBgW* w=bg;
    if (w!=nullptr) dBgW_Move(w);
    status.Move(); sphere.ClrTgHit();
    if (!hit) {
        gabi::call(0x0200E240,dComIfG_Ccsp(),&sphere);
        return;
    }
    s32 sw=PrmAbstract(this,8,13);
    if (sw!=255) dComIfGs_onSwitch(sw,home.roomNo);
    actor_status=actor_status&~0x80U;
    w=bg;
    if (w!=nullptr && dBgW_ChkUsed(w)) {
        dBgS* bgs=dComIfG_Bgsp();
        cBgS_Release(bgs,bg);
    }
    changeAction(this,0x0236ACCC);
}
VERIFY(0x0236ABD0,&daObjLeaves_c::wait_proc);
void daObjLeaves_c::alpha_calc_start_wait_proc() {
    WWHD_FUNC(0x0236ACCC,void,this);
    if (fadeDelay<=0) changeAction(this,0x0236ACF8);
}
VERIFY(0x0236ACCC,&daObjLeaves_c::alpha_calc_start_wait_proc);
void daObjLeaves_c::item_set_wait_proc() {
    WWHD_FUNC(0x0236ACF8,void,this);
    f32 value=alpha+alphaStep; alpha=value;
    if (!(value>200.0f)) {
        u32 item=PrmAbstract(this,6,0);
        u32 saveBit=PrmAbstract(this,7,6);
        gabi::Local<cXyz> position;
        position->x=current.pos.x;position->y=current.pos.y;position->z=current.pos.z;
        gabi::Local<csXyz> angle;
        csXyz_ct(angle,0,0,0);
        position->y=position->y-30.0f;
        fopAcM_createItemFromTable(position,item,saveBit,home.roomNo,0,angle,1,nullptr);
        changeAction(this,0x0236ADFC);
    }
}
VERIFY(0x0236ACF8,&daObjLeaves_c::item_set_wait_proc);
u32 daObjLeaves_c::retire_wait_proc() {
    WWHD_FUNC(0x0236ADFC,u32,this);
    f32 value=alpha+alphaStep;
    if (!(value>0.0f)) {
        s32 delay=retireDelay;alpha=0.0f;
        if (delay<=0) return fopAcM_delete(this);
    } else alpha=value;
    return (u32)gabi::ea(this); /* original: r3 still this */
}
VERIFY(0x0236ADFC,&daObjLeaves_c::retire_wait_proc);
static void sinit() {
    WWHD_FUNC(0x0236AE34,void,(u32)0);
    sinit_header_statics(0x1046A470,0x101CA90C);
}
VERIFY(0x0236AE34,sinit);
static void trivial_dt(void* p,s32 flags) {
    WWHD_FUNC(0x0236AEC8,void,p,flags);
    if (p && (flags&1)) operator_delete(p);
}
VERIFY(0x0236AEC8,trivial_dt);
static void destroy(daObjLeaves_c* p,s32 flags) {
    WWHD_FUNC(0x0236AEDC,void,p,flags);
    if (p) {
        gabi::call(0x02515AE8,&p->sphere,2);
        dCcD_Stts_dt(&p->status,2);
        gabi::call(0x025D50BC,p,0);
        if (flags&1) operator_delete(p);
    }
}
VERIFY(0x0236AEDC,destroy);
static void emptyVirtual(void* p) {
    WWHD_FUNC(0x0236AF48,void,p);
}
VERIFY(0x0236AF48,emptyVirtual);
static s32 daObjLeaves_Create(daObjLeaves_c* p) {
    WWHD_FUNC(0x0236A748,s32,p); return p->create();
}
VERIFY(0x0236A748,daObjLeaves_Create);
static BOOL daObjLeaves_Delete(daObjLeaves_c* p) {
    WWHD_FUNC(0x0236A7E4,BOOL,p); return p->remove();
}
VERIFY(0x0236A7E4,daObjLeaves_Delete);
static BOOL daObjLeaves_Execute(daObjLeaves_c* p) {
    WWHD_FUNC(0x0236A880,BOOL,p); return p->execute();
}
VERIFY(0x0236A880,daObjLeaves_Execute);
static BOOL daObjLeaves_IsDelete(daObjLeaves_c* p) {
    WWHD_FUNC(0x0236ABC8,BOOL,p); return TRUE;
}
VERIFY(0x0236ABC8,daObjLeaves_IsDelete);
// The HD draw path also converts material colors into renderer float overrides.
static GXColor* materialKColor(u32 material) {
    u32 block=gabi::load<u32>(material+0x18);
    u32 vt=gabi::load<u32>(block+4);
    return gabi::call_ptr<GXColor*>(gabi::load<u32>(vt+0x4C),gabi::at<void>(block),3);
}
static void materialSetKColor(u32 material,GXColor* color) {
    u32 block=gabi::load<u32>(material+0x18);
    u32 vt=gabi::load<u32>(block+4);
    gabi::call_ptr(gabi::load<u32>(vt+0x3C),gabi::at<void>(block),3,color);
}
bool daObjLeaves_c::draw() {
    WWHD_FUNC(0x0236A884,bool,this);
    settingTevStruct(dKy_getEnvlight(),1,&current.pos,&tevStr);
    auto light=dKy_getEnvlight();
    setLightTevColorType(light,model,&tevStr);
    u8 opacity=(u8)gabi::ftoi(alpha);
    J3DModel* m=model;
    u32 data=gabi::load<u32>(gabi::ea(m)+0xAC);
    gabi::call(0x027F58E0,m,opacity!=255);
    u32 jointData=gabi::call<u32>(0x027F3F94,gabi::at<void>(data));
    for (u16 i=0; i<gabi::load<u16>(jointData+8); ++i) {
        u32 count=gabi::load<u32>(data+4);
        u32 joints=gabi::load<u32>(data+8);
        if (i<count) joints+=i*0x1C;
        u32 material=gabi::load<u32>(joints+0x10);
        while (material!=0) {
            u32 shape=gabi::load<u32>(material+8);
            if (opacity==0) {
                gabi::store<u8>(shape+4,0);
            } else {
                gabi::store<u8>(shape+4,1);
                if (opacity!=255) {
                    gabi::call(0x025F0AD0,gabi::at<void>(material),opacity,0);
                } else {
                    u32 entry=gabi::load<u32>(material);
                    u32 offset=gabi::load<u32>(entry+0x20);
                    void* blend=offset==0 ? nullptr : gabi::at<void>(entry+0x20+offset);
                    gabi::call(0x027E212C,blend,1);
                }
                GXColor* color=materialKColor(material);
                gabi::store<u8>(gabi::ea(color)+3,opacity);
                GXColor* copied=materialKColor(material);
                materialSetKColor(material,copied);
                gabi::Local<LinearColor_l> normalized;
                colorToFloat(normalized,copied);
                gabi::Local<cXyz> linear;
                gabi::call(0x0274D458,linear.get(),normalized.get(),1.0f);
                u32 dirty=gabi::load<u32>(material+0xA0);
                gabi::store<u32>(material+0xA0,dirty|0x400);
                void* overrideColor=gabi::call<void*>(0x027F9F0C,gabi::at<void>(material+0xA0),10);
                f32 a=(f32)gabi::load<u8>(gabi::ea(copied)+3)/255.0f;
                f32 x=linear->x,y=linear->y,z=linear->z;
                gabi::store<f32>(gabi::ea(overrideColor),x);
                gabi::store<f32>(gabi::ea(overrideColor)+4,y);
                gabi::store<f32>(gabi::ea(overrideColor)+8,z);
                gabi::store<f32>(gabi::ea(overrideColor)+12,a);
            }
            material=gabi::load<u32>(material+4);
        }
        jointData=gabi::call<u32>(0x027F3F94,gabi::at<void>(data));
    }
    mDoExt_modelUpdateDL(model);
    return true;
}
VERIFY(0x0236A884,&daObjLeaves_c::draw);
static BOOL daObjLeaves_Draw(daObjLeaves_c* p) {
    WWHD_FUNC(0x0236ABC4,BOOL,p); return p->draw();
}
VERIFY(0x0236ABC4,daObjLeaves_Draw);
