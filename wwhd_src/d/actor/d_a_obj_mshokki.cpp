/* Forsaken Fortress dishes, derived from the WWHD disassembly. */
#include "d/actor/d_a_obj_mshokki.h"
static u8* ptr(u32 a) { return gabi::at<u8>(a); }
static u32 play() { return gabi::call<u32>(0x025200D4); }
static s32 wrappedAbs(s32 v) { return v < 0 ? (s32)(0u-(u32)v) : v; }
static f32 dishData(s32 type, u32 offset) { return gabi::load<f32>(0x1002DB44+(u32)type*16+offset); }
static u32 unsignedFloat(f32 v) {
    // bge after fcmpu tests !LT: an unordered value takes the high-half path.
    return v < 2147483648.0f ? (u32)gabi::ftoi(v) : (u32)gabi::ftoi(v-2147483648.0f)+0x80000000u;
}
bool daObjMshokki_c::create_heap() {
    WWHD_FUNC(0x0237CE88,bool,this);
    s32 index=gabi::load<s32>(0x101CBE00+(u32)(s32)mType*4);
    auto* data=(J3DModelData*)dComIfG_getObjectRes(STR(0x1002DAA8),index,0x1002DADC);
    if (!data) { JUT_ASSERT_fail(STR(0x1002DBBC),0x164,STR(0x1002DBB8)); return false; }
    mModel=mDoExt_J3DModel__create(data,0x80000,0x11000022);
    return mModel!=nullptr;
}
VERIFY(0x0237CE88,&daObjMshokki_c::create_heap);
static u8 solidHeapCB(daObjMshokki_c* self) {
    WWHD_FUNC(0x0237CF44,u8,self);
    return gabi::call<u8>(0x0237CE88,self);
}
VERIFY(0x0237CF44,solidHeapCB);
void daObjMshokki_c::co_hitCallback(fopAc_ac_c* actor,dCcD_GObjInf* own,fopAc_ac_c* other,dCcD_GObjInf* hit) {
    WWHD_FUNC(0x0237CF48,void,actor,own,other,hit);
    auto* self=(daObjMshokki_c*)actor;
    f32 dx=self->current.pos.x-other->current.pos.x;
    f32 dz=self->current.pos.z-other->current.pos.z;
    s16 angle=gabi::call<s16>(0x020195B0,dx,dz);
    u32 p=play();
    bool aligned=gabi::ea(other)==gabi::load<u32>(p+0x5B2C) && (f32)wrappedAbs((s32)(s16)other->current.angle.y-angle)<4096.0f;
    f32 speed=other->speedF;
    if (aligned && !(speed<17.0f) && wrappedAbs((s32)((u32)self->mLastLaunch-(u32)self->mFrame))>2) {
        f32 strength=speed-17.0f;
        // ble means !GT; NaN is retained, rather than clamped to one.
        if (strength>1.0f) strength=1.0f;
        s32 turn=gabi::ftoi(6144.0f*strength);
        f32 horizontal=speed*cM_scos(turn);
        f32 vertical=gabi::fmadds(0.0f,strength,24.0f);
        self->current.angle.y=angle;
        self->shape_angle.y=angle;
        u32 frame=self->mFrame;
        gabi::store<u32>(gabi::ea(self)+0x6DC,0);
        self->speedF=horizontal;
        self->speed.y=vertical;
        self->mLastLaunch=frame;
        return;
    }
    // The HD blt rejects only ordered values below six; unordered values continue.
    if (speed<6.0f) return;
    f32 strength=(speed-6.0f)/11.0f;
    s32 type=self->mType;
    if (strength>1.0f) strength=1.0f;
    s16 tilt=(s16)gabi::ftoi(strength*dishData(type,12));
    s16 facing=other->current.angle.y;
    self->mSwingReference=(s16)(facing+0x2000);
    self->shape_angle.x=tilt;
    self->shape_angle.y=facing;
}
VERIFY(0x0237CF48,daObjMshokki_c::co_hitCallback);
void daObjMshokki_c::set_mtx() {
    WWHD_FUNC(0x0237D17C,void,this);
    J3DModel_setBaseScale(mModel,&scale);
    mDoMtx_stack_c::transS(current.pos.x,current.pos.y,current.pos.z);
    gabi::call(0x025F1C28,mDoMtx_stack_c::get(),(s16)shape_angle.y);
    gabi::call(0x025F1BF4,mDoMtx_stack_c::get(),(s16)shape_angle.x);
    J3DModel_setBaseTRMtx(mModel,mDoMtx_stack_c::get());
}
VERIFY(0x0237D17C,&daObjMshokki_c::set_mtx);
static u32 paramAbstract(daObjMshokki_c* self,u32 width,u32 shift);
cPhs_State daObjMshokki_c::_create() {
    WWHD_FUNC(0x0237D260,cPhs_State,this);
    u32 a=gabi::ea(this),condition=actor_condition;
    if (!(condition&8)) {
        if (this) {
            fopAc_ac_c_ct(this); __vtbl=0x1002DB34;
            gabi::call(0x024F0474,&mAcch);
            gabi::store<u32>(a+0x3C8,0x1002DB04);
            gabi::store<u32>(a+0x3D8,0x1002DB14);
            gabi::store<u32>(a+0x3CC,0x1002DB24);
            gabi::store<u8>(a+0x3D0,1);
            gabi::call(0x024EFE94,&mAcchCir);
            gabi::call(0x0200BD2C,&mStts);
            gabi::call(0x02515DA0,ptr(a+0x5D8));
            gabi::store<u32>(a+0x5D4,0x1004AE88);
            gabi::store<u32>(a+0x5D8,0x1004AEC0);
            gabi::call(0x02515FB8,&mCyl);
            gabi::store<u32>(a+0x70C,0x100015A8);
            gabi::store<u32>(a+0x708,0x1002DAF4);
            gabi::call(0x02018590,ptr(a+0x710));
            gabi::store<u32>(a+0x634,0x1004B108);
            gabi::store<u32>(a+0x70C,0x1004B160);
            condition=actor_condition;
            gabi::store<u32>(a+0x724,0x1004B150);
        }
        actor_condition=condition|8;
    }
    if (gabi::load<u8>(a+0xC)==0) {
        u32 type=paramAbstract(this,2,0); mType=(s32)type;
        if (type>=3) { JUT_ASSERT_fail(STR(0x1002DC0C),0x288,STR(0x1002DC08)); mType=0; }
    }
    cPhs_State result=dComIfG_resLoad(&mPhase,STR(0x1002DAA8));
    if (result!=cPhs_COMPLEATE_e) return result;
    if (!gabi::call<BOOL>(0x025D63E8,this,0x0237CF44,0x4C0)) return cPhs_ERROR_e;
    mAcchCir.SetWall(50.0f,35.0f);
    mAcch.Set(&current.pos,&old.pos,this,1,&mAcchCir,&speed,&current.angle,&shape_angle);
    u32 flags=mAcch.m_flags;
    mAcch.m_roof_crr_height=50.0f; mAcch.m_flags=flags&~0x408u;
    u32 p=play(); mAcch.CrrPos((dBgS*)ptr(p+0x12A0));
    mAcch.m_flags=(u32)mAcch.m_flags&~0x80u;
    mStts.Init(100,255,this);
    gabi::call(0x02516518,&mCyl,ptr(0x1002DB74));
    gabi::store<u32>(a+0x63C,a+0x5BC);
    gabi::call(0x02516580,&mCyl,&current.pos);
    gabi::call(0x020184DC,ptr(a+0x710),dishData(mType,4));
    gabi::call(0x02018428,ptr(a+0x710),dishData(mType,8));
    gabi::store<u32>(a+0x6DC,0x0237CF48);
    set_mtx();
    cullMtx=gabi::ea(J3DModel_getBaseTRMtx(mModel));
    gravity=-6.0f;
    return result;
}
VERIFY(0x0237D260,&daObjMshokki_c::_create);
static cPhs_State actorCreate(daObjMshokki_c* self) { WWHD_FUNC(0x0237D53C,cPhs_State,self); return self->_create(); }
VERIFY(0x0237D53C,actorCreate);
bool daObjMshokki_c::_delete() { WWHD_FUNC(0x0237D540,bool,this); dComIfG_resDelete(&mPhase,STR(0x1002DAA8)); return true; }
VERIFY(0x0237D540,&daObjMshokki_c::_delete);
static bool actorDelete(daObjMshokki_c* self) { WWHD_FUNC(0x0237D570,bool,self); return self->_delete(); }
VERIFY(0x0237D570,actorDelete);
void daObjMshokki_c::set_se() {
    WWHD_FUNC(0x0237D574,void,this);
    s16 delta=(s16)((s16)shape_angle.y-(s16)mSwingReference);
    if (wrappedAbs(delta)>=4096) return;
    f32 tilt=(f32)(s16)shape_angle.x;
    s32 type=mType;
    s16 phase=(s16)gabi::ftoi((tilt/dishData(type,12))*16384.0f);
    if (phase>0x4000) phase=0x4000;
    f32 intensity=cM_ssin(phase)*100.0f;
    u32 sound=gabi::load<u32>(0x101CBE0C+(u32)type*4);
    u32 volume=unsignedFloat(intensity);
    s32 reverb=gabi::call<s32>(0x02520540,(s8)current.roomNo);
    gabi::call(0x025E1A40,sound,&current.pos,volume,reverb);
}
VERIFY(0x0237D574,&daObjMshokki_c::set_se);
void daObjMshokki_c::break_proc() {
    WWHD_FUNC(0x0237D6DC,void,this);
    u16 particle=gabi::load<u16>(0x101CBE18+(u32)(s32)mType*2);
    u32 control=gabi::load<u32>(play()+0x5AB0);
    gabi::call(0x025A847C,ptr(control),0,particle,&current.pos,&shape_angle,(u8*)nullptr,255,(u8*)nullptr,-1,ptr(gabi::ea(this)+0x1A8),0,0);
    u32 p=play();
    u32 material=gabi::call<u32>(0x024EECAC,ptr(p+0x12A0),ptr(gabi::ea(this)+0x4A0));
    s32 reverb=gabi::call<s32>(0x02520540,(s8)current.roomNo);
    gabi::call(0x025E1A40,0x6806,&current.pos,material,reverb);
    gabi::Local<cXyz> position; position->set(current.pos.x,current.pos.y,current.pos.z);
    u32 id=gabi::load<u32>(gabi::ea(this)+4);
    gabi::call(0x0255F458,position.get(),150,id,5);
    gabi::call(0x025D57E0,this);
}
VERIFY(0x0237D6DC,&daObjMshokki_c::break_proc);
bool daObjMshokki_c::checkCollision() {
    WWHD_FUNC(0x0237D7C8,bool,this);
    if (!gabi::call<BOOL>(0x025162A4,&mCyl)) return false;
    u32 hit=gabi::call<u32>(0x02516300,&mCyl);
    bool broken=false;
    if (hit) {
        if (gabi::load<u32>(hit+0x10)&0x200000u) {
            u32 a=gabi::ea(this);
            gabi::Local<cXyz> force; force->set(gabi::load<f32>(a+0x6B8),0.0f,gabi::load<f32>(a+0x6C0));
            f32 square=PSVECSquareMag(force.get());
            f32 magnitude=gabi::call<f32>(0x028F4384,square);
            f32 z=gabi::load<f32>(a+0x6C0),x=gabi::load<f32>(a+0x6B8);
            speedF=magnitude*0.5f;
            current.angle.y=gabi::call<s16>(0x020195B0,x,z);
        } else { break_proc(); broken=true; }
    }
    gabi::call(0x0251621C,&mCyl);
    return broken;
}
VERIFY(0x0237D7C8,&daObjMshokki_c::checkCollision);
bool daObjMshokki_c::_execute() {
    WWHD_FUNC(0x0237D89C,bool,this);
    gabi::call(0x025D6870,this,&mStts);
    u32 p=play(); mAcch.CrrPos((dBgS*)ptr(p+0x12A0));
    f32 damped=(f32)speedF*0.8f; speedF=damped;
    if (damped<0.1f) speedF=0.0f;
    if (mAcch.ChkGroundHit()) {
        s16 tilt=shape_angle.x;
        if (wrappedAbs(tilt)>128) { shape_angle.x=(s16)(tilt-128); shape_angle.y=(s16)((s16)shape_angle.y+4096); set_se(); }
        else shape_angle.x=0;
    } else shape_angle.x=(s16)((s16)shape_angle.x-3072);
    u32 flags=mAcch.m_flags;
    if (mHasRested==1 && (flags&0x80)) break_proc();
    else {
        if ((flags&0x20) && !(flags&0x80)) mHasRested=1;
        mStts.Move();
        if (!checkCollision()) {
            set_mtx(); gabi::call(0x02516680,&mCyl,&current.pos);
            mStts.mRoomId=(u8)(s8)current.roomNo;
            p=play(); gabi::call(0x0200E240,ptr(p+0x26A4),&mCyl);
            f32 x=current.pos.x,y=current.pos.y+dishData(mType,0),z=current.pos.z;
            u32 a=gabi::ea(this);
            gabi::store<f32>(a+0x390,x); gabi::store<f32>(a+0x394,y); gabi::store<f32>(a+0x398,z);
            eyePos.set(x,y,z);
        }
    }
    mFrame=(u32)mFrame+1;
    return true;
}
VERIFY(0x0237D89C,&daObjMshokki_c::_execute);
static bool actorExecute(daObjMshokki_c* self) { WWHD_FUNC(0x0237DA70,bool,self); return self->_execute(); }
VERIFY(0x0237DA70,actorExecute);
bool daObjMshokki_c::_draw() {
    WWHD_FUNC(0x0237DA74,bool,this);
    auto* env=dKy_getEnvlight(); settingTevStruct(env,0,&current.pos,&tevStr);
    env=dKy_getEnvlight(); J3DModel* model=mModel;
    setLightTevColorType(env,model,&tevStr); mDoExt_modelUpdateDL(mModel);
    return true;
}
VERIFY(0x0237DA74,&daObjMshokki_c::_draw);
static bool actorDraw(daObjMshokki_c* self) { WWHD_FUNC(0x0237DAD0,bool,self); return self->_draw(); }
VERIFY(0x0237DAD0,actorDraw);
static BOOL isDelete(daObjMshokki_c* self) { WWHD_FUNC(0x0237DAD4,BOOL,self); return 1; }
VERIFY(0x0237DAD4,isDelete);
static void initStatics() {
    WWHD_FUNC(0x0237DADC,void);
    for(u32 i=0;i<4;i++) gabi::store<u32>(0x1046B9E8+i*4,0);
    gabi::call(0x028F026C,ptr(0x101CBE20));
    gabi::store<f32>(0x1046B9DC,-3.1415927410125732f); gabi::store<f32>(0x1046B9E0,3.1415927410125732f);
    gabi::call(0x028ED6F8,ptr(0x1046B9E4)); gabi::call(0x028F026C,ptr(0x101CBE2C));
    gabi::call(0x028EAB2C,ptr(0x1046B9E5)); gabi::call(0x028F026C,ptr(0x101CBE38));
}
VERIFY(0x0237DADC,initStatics);
static void trivialDestructor(void* self,s32 flags) { WWHD_FUNC(0x0237DB70,void,self,flags); if(self && (flags&1)) gabi::call(0x0273AF40,self); }
VERIFY(0x0237DB70,trivialDestructor);
static void actorDestructor(daObjMshokki_c* self,s32 flags) {
    WWHD_FUNC(0x0237DB84,void,self,flags);
    if (!self) return;
    u32 a=gabi::ea(self);
    gabi::call(0x02515A70,&self->mCyl,2); gabi::call(0x02515860,&self->mStts,2); gabi::call(0x02018034,ptr(a+0x590),2);
    gabi::store<u32>(a+0x3D8,0x1002DB14); gabi::store<u32>(a+0x3CC,0x1002DB24);
    gabi::call(0x024EFD9C,&self->mAcch,0); gabi::call(0x025D50BC,self,0);
    if(flags&1) gabi::call(0x0273AF40,self);
}
VERIFY(0x0237DB84,actorDestructor);
static void emptyVirtual(void* self) { WWHD_FUNC(0x0237DC20,void,self); }
VERIFY(0x0237DC20,emptyVirtual);
static u32 paramAbstract(daObjMshokki_c* self,u32 width,u32 shift) {
    WWHD_FUNC(0x0237DC24,u32,self,width,shift);
    u32 mask=(width&0x20)?0:(1u<<(width&31));
    u32 value=(shift&0x20)?0:((u32)self->mParameters>>(shift&31));
    return value&(mask-1);
}
VERIFY(0x0237DC24,paramAbstract);
