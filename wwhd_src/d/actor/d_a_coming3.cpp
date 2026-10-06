/**
 * d_a_coming3.cpp (WWHD)
 * Coming3 - barrel challenge spawner (daComing3::Act_c)
 *
 * Written from the WWHD code with
 * the GameCube decompilation (zeldaret/tww src/d/actor/d_a_coming3.cpp) as reference, verified
 * against cking.rpx.
 */
#include "d/actor/d_a_coming3.h"
using daComing3::Act_c;
template<class T> static T* member(void* p,u32 off) {
    return gabi::at<T>(gabi::ea(p)+off);
}
static bool create_heap(Act_c* a) {
    WWHD_FUNC(0x02114A78,bool,a);
    void* data=dComIfG_getObjectRes(gabi::at<char>(0x1000CBA8),35,0x1000C9A0);
    if(!data) gabi::call(0x0273AA24,gabi::at<char>(0x1000CB0C),847,gabi::at<char>(0x1000CB1C));
    J3DModel* m=gabi::call<J3DModel*>(0x025E38E0,data,0,0x11020203);
    a->model=m;
    return data && m;
}
VERIFY(0x02114A78,create_heap);
static u8 solidHeapCB(Act_c* a) {
    WWHD_FUNC(0x02114B28,u8,a);
    return gabi::call<u8>(0x02114A78,a);
}
VERIFY(0x02114B28,solidHeapCB);
static void collision_init(Act_c* a) {
    WWHD_FUNC(0x02114B2C,void,a);
    gabi::call(0x02515F14,member<void>(a,0x3ac),255,255,a);
    gabi::call(0x02516518,member<void>(a,0x3e8),gabi::at<void>(0x1000CAC8));
    *member<be<u32>>(a,0x42c)=gabi::ea(a)+0x3ac;
    u32 zeroX=gabi::load<u32>(0x101FFBA8);
    u32 flags=*member<be<u32>>(a,0x47c);
    *member<be<u32>>(a,0x49c)=zeroX;
    *member<be<u32>>(a,0x4a0)=gabi::load<u32>(0x101FFBAC);
    u32 zeroZ=gabi::load<u32>(0x101FFBB0);
    *member<be<u32>>(a,0x47c)=flags|4;
    *member<be<u32>>(a,0x4a4)=zeroZ;
}
VERIFY(0x02114B2C,collision_init);
static u32 member_function_target(Act_c*& a,u32 table,u32 index) {
    u32 entry=table+index*8;
    s16 slot=gabi::load<s16>(entry+2), adjustment=gabi::load<s16>(entry);
    a=gabi::at<Act_c>(gabi::ea(a)+adjustment);
    if(slot<0) return gabi::load<u32>(entry+4);
    s16 vptrOffset=gabi::load<s16>(entry+6);
    u32 vptr=gabi::load<u32>(gabi::ea(a)+vptrOffset);
    return gabi::load<u32>(vptr+slot*8+4);
}
static bool coming_process_init(Act_c* a,s32 idx) {
    WWHD_FUNC(0x02114BA8,bool,a,idx);
    if((u32)(s32)idx>=3) return false;
    Act_c* receiver=a;
    u32 target=member_function_target(receiver,0x1000CB2C,idx);
    if(!gabi::call_ptr<s32>(target,receiver)) return false;
    a->process=idx;
    return true;
}
VERIFY(0x02114BA8,coming_process_init);
static void set_mtx(Act_c* a) {
    WWHD_FUNC(0x02114C60,void,a);
    J3DModel* m=a->model;
    *member<be<f32>>(m,0xc4)=2;
    *member<be<f32>>(m,0xc0)=2;
    *member<be<f32>>(m,0xbc)=2;
    f32 x=a->current.pos.x,y=a->current.pos.y,z=a->current.pos.z;
    PSMTXTrans(mDoMtx_stack_c::get(),x,y,z);
    s16 rz=a->shape_angle.z,rx=a->shape_angle.x,ry=a->shape_angle.y;
    gabi::call(0x025F1B48,mDoMtx_stack_c::get(),rx,ry,rz);
    mDoMtx_stack_c::transM(0,390,0);
    mDoMtx_stack_c::scaleM(2,2,2);
    J3DModel_setBaseTRMtx(a->model,mDoMtx_stack_c::get());
    gabi::call(0x028E90D4,mDoMtx_stack_c::get(),&a->barrelMatrix);
    gabi::call(0x027F4D5C,a->model.get());
}
VERIFY(0x02114C60,set_mtx);
static s32 create(Act_c* a) {
    WWHD_FUNC(0x02114D90,s32,a);
    u32 flags=*member<be<u32>>(a,0x2e4);
    if(!(flags&8)) {
        if(a) {
            gabi::call(0x025D4ED0,a);
            a->__vtbl=0x1000CAB8;
            gabi::call(0x0200BD2C,member<void>(a,0x3ac));
            gabi::call(0x02515DA0,member<void>(a,0x3c8));
            *member<be<u32>>(a,0x3c4)=0x1004AE88;
            *member<be<u32>>(a,0x3c8)=0x1004AEC0;
            gabi::call(0x02515FB8,member<void>(a,0x3e8));
            *member<be<u32>>(a,0x4fc)=0x100015A8;
            *member<be<u32>>(a,0x4f8)=0x1000C9B8;
            gabi::call(0x02018590,member<void>(a,0x500));
            *member<be<u32>>(a,0x424)=0x1004B108;
            *member<be<u32>>(a,0x4fc)=0x1004B160;
            flags=*member<be<u32>>(a,0x2e4);
            *member<be<u32>>(a,0x514)=0x1004B150;
        }
        *member<be<u32>>(a,0x2e4)=flags|8;
    }
    s32 result=gabi::call<s32>(0x02520460,&a->phase,gabi::at<char>(0x1000CBA8));
    if(result==4) {
        if(gabi::call<s32>(0x025D63E8,a,0x02114B28,0)) {
            a->challenge=(s16)(a->shape_angle.z+1);
            collision_init(a);
            coming_process_init(a,0);
            set_mtx(a);
        }
        else result=5;
    }
    return result;
}
VERIFY(0x02114D90,create);
static bool remove(Act_c* a) {
    WWHD_FUNC(0x02114ED4,bool,a);
    gabi::call(0x025204C8,&a->phase,gabi::at<char>(0x1000CBA8));
    return true;
}
VERIFY(0x02114ED4,remove);
static void coming_process_main(Act_c* a) {
    WWHD_FUNC(0x02114F04,void,a);
    s16 idx=a->process;
    if((u32)(s32)idx>=3)return;
    u32 target=member_function_target(a,0x1000CB50,idx);
    gabi::call_ptr(target,a);
}
VERIFY(0x02114F04,coming_process_main);
static bool execute(Act_c* a) {
    WWHD_FUNC(0x02114F5C,bool,a);
    coming_process_main(a);
    set_mtx(a);
    return true;
}
VERIFY(0x02114F5C,execute);
static bool draw(Act_c* a) {
    WWHD_FUNC(0x02114F94,bool,a);
    s32 state=a->gameState;
    if(state==1||state==2) {
        void* env=gabi::call<void*>(0x02555D0C);
        gabi::call(0x025626A4,env,0,&a->current.pos,&a->tevStr);
        env=gabi::call<void*>(0x02555D0C);
        gabi::call(0x02562F5C,env,a->model.get(),&a->tevStr);
        gabi::call(0x025E2DE0,a->model.get(),0);
    }
    return true;
}
VERIFY(0x02114F94,draw);
static BOOL coming_start_init(Act_c* a) {
    WWHD_FUNC(0x02115668,BOOL,a);
    a->gameState=3;
    a->barrelID=0xffffffff;
    a->distance=0;
    a->delay=30;
    return TRUE;
}
VERIFY(0x02115668,coming_start_init);
static s32 get_challenge_id(Act_c* a) {
    WWHD_FUNC(0x02115694,s32,a);
    return (a->challenge-1)&1;
}
VERIFY(0x02115694,get_challenge_id);
static f32 get_limit_dist(Act_c* a) {
    WWHD_FUNC(0x021156A4,f32,a);
    gabi::Local<be<u32>[2]> limits;
    (*limits)[0]=gabi::load<u32>(0x1000CB94);
    (*limits)[1]=gabi::load<u32>(0x1000CB98);
    s32 idx=get_challenge_id(a);
    return gabi::load<f32>(gabi::ea(limits.get())+idx*4);
}
VERIFY(0x021156A4,get_limit_dist);
static BOOL coming_game_init(Act_c* a) {
    WWHD_FUNC(0x02115958,BOOL,a);
    a->gameState=0;
    return a->barrelID!=0xffffffff;
}
VERIFY(0x02115958,coming_game_init);
static BOOL coming_wait_init(Act_c* a) {
    WWHD_FUNC(0x02115B40,BOOL,a);
    a->gameState=3;
    return TRUE;
}
VERIFY(0x02115B40,coming_wait_init);
static void coming_wait_main(Act_c* a) {
    WWHD_FUNC(0x02115B50,void,a);
    gabi::Local<gptr<Act_c>> barrel;
    gabi::call<s32>(0x025D54C4,(u32)a->barrelID,barrel.get());
    Act_c* b=*barrel;
    if(b) {
        gabi::Local<gptr<Act_c>> item;
        u32 id=*member<be<u32>>(b,0x574);
        if(!gabi::call<s32>(0x025D54C4,id,item.get())) {
            *member<be<u8>>(b,0x590)=1;
            gabi::call(0x025D57E0,a);
        }
    }
}
VERIFY(0x02115B50,coming_wait_main);
static s32 method_create(Act_c* a){
    WWHD_FUNC(0x02115BBC,s32,a);
    return create(a);
}
VERIFY(0x02115BBC,method_create);
static bool method_delete(Act_c* a){
    WWHD_FUNC(0x02115BC0,bool,a);
    return remove(a);
}
VERIFY(0x02115BC0,method_delete);
static bool method_execute(Act_c* a){
    WWHD_FUNC(0x02115BC4,bool,a);
    return execute(a);
}
VERIFY(0x02115BC4,method_execute);
static bool method_draw(Act_c* a){
    WWHD_FUNC(0x02115BC8,bool,a);
    return draw(a);
}
VERIFY(0x02115BC8,method_draw);
static void sinit(){
    WWHD_FUNC(0x02115BCC,void);
    sinit_header_statics_z(0x10462E70,0x101B3D1C,0x10462E7C);
}
VERIFY(0x02115BCC,sinit);
static void objground_destructor(void* p,s32 flags){
    WWHD_FUNC(0x02115C60,void,p,flags);
    if(p){
        *member<be<u32>>(p,0x20)=0x1000C9E8;
        *member<be<u32>>(p,0x40)=0x1000CA08;
        *member<be<u32>>(p,0x4c)=0x1000C9C8;
        gabi::call(0x02008DAC,p,0);
        if(flags&1)gabi::call(0x0273AF40,p);
    }
}
VERIFY(0x02115C60,objground_destructor);
static void water_destructor(void* p,s32 flags){
    WWHD_FUNC(0x02115CD8,void,p,flags);
    if(p){
        *member<be<u32>>(p,0x20)=0x1000CA58;
        *member<be<u32>>(p,0x24)=0x1000CA78;
        *member<be<u32>>(p,0x30)=0x1000C9C8;
        gabi::call(0x02008B4C,member<void>(p,0x10),0);
        if(flags&1)gabi::call(0x0273AF40,p);
    }
}
VERIFY(0x02115CD8,water_destructor);
static void safe_string_destructor(void* p,s32 flags){
    WWHD_FUNC(0x02115D50,void,p,flags);
    if(p&&(flags&1))gabi::call(0x0273AF40,p);
}
VERIFY(0x02115D50,safe_string_destructor);
static void actor_destructor(Act_c* a,s32 flags){
    WWHD_FUNC(0x02115D68,void,a,flags);
    if(a){
        gabi::call(0x02515A70,member<void>(a,0x3e8),2);
        gabi::call(0x02515860,member<void>(a,0x3ac),2);
        gabi::call(0x025D50BC,a,0);
        if(flags&1)gabi::call(0x0273AF40,a);
    }
}
VERIFY(0x02115D68,actor_destructor);
static BOOL is_delete(Act_c* a){
    WWHD_FUNC(0x02115DD4,BOOL,a);
    return TRUE;
}
VERIFY(0x02115DD4,is_delete);
static bool collision_main(Act_c* a) {
    WWHD_FUNC(0x02115004,bool,a);
    void* cyl=member<void>(a,0x3e8);
    if(gabi::call<s32>(0x025162A4,cyl)){
        gabi::call(0x0251621C,cyl);
        return true;
    }
    gabi::Local<cXyz> origin,center;
    origin->x=0;
    origin->y=0;
    origin->z=0;
    gabi::call(0x028E8F64,&a->barrelMatrix,origin.get(),center.get());
    gabi::call(0x020184DC,member<void>(a,0x500),130.f);
    gabi::call(0x02018428,member<void>(a,0x500),225.f);
    gabi::call(0x020182E0,member<void>(a,0x500),center.get());
    gabi::call(0x0200E240,dComIfG_Ccsp(),cyl);
    f32 y=center->y,x=center->x;
    a->eyePos.x=x;
    y+=50.f;
    a->eyePos.z=center->z;
    a->eyePos.y=y;
    return false;
}
VERIFY(0x02115004,collision_main);
struct SafetyCallback {
    gptr<cXyz> pos;
    be<s32> safe;
};
static void* position_is_safety_call_back(fopAc_ac_c* actor,SafetyCallback* cb) {
    WWHD_FUNC(0x021156E4,void*,actor,cb);
    if(gabi::call<s32>(0x025D4604,actor)&&actor->group==2) {
        cXyz* p=cb->pos;
        f32 z=actor->current.pos.z-p->z,x=actor->current.pos.x-p->x;
        f32 square=gabi::fmadds(x,x,z*z);
        if(square<200.f){
            cb->safe=0;
            return nullptr;
        }
    }
    return actor;
}
VERIFY(0x021156E4,position_is_safety_call_back);
static void ground_filter_init(u32 addr,u32 guard,u32 registration) {
    if(gabi::load<u32>(guard))return;
    gabi::store<u32>(guard,1);
    gabi::call(0x02008E0C,gabi::at<void>(addr));
    gabi::store<u32>(addr,addr+0x40);
    gabi::store<u32>(addr+4,addr+0x4c);
    gabi::store<u32>(addr+0x50,1);
    gabi::store<u32>(addr+0x10,0x1000CA18);
    gabi::store<u32>(addr+0x4c,0x1000CA38);
    gabi::store<u32>(addr+0x20,0x1000CA28);
    gabi::store<u8>(addr+0x44,1);
    for(u32 off=0x45;off<=0x4a;++off)gabi::store<u8>(addr+off,0);
    gabi::store<u32>(addr+0x40,0x1000CA48);
    gabi::call(0x028F026C,gabi::at<void>(registration));
}
static BOOL get_water_height(Act_c* a,be<f32>* height,be<s32>* room,const cXyz* pos) {
    WWHD_FUNC(0x021150F4,BOOL,a,height,room,pos);
    const u32 water=0x10462E8C;
    if(!gabi::load<u32>(0x10462F84)) {
        gabi::store<u32>(0x10462F84,1);
        gabi::call(0x024F22DC,gabi::at<void>(water));
        gabi::call(0x028F026C,gabi::at<void>(0x101B3CF0));
    }
    f32 waterHeight=-1000000000.f;
    s32 roomID=0;
    bool sea=gabi::call<s32>(0x0246B6A4,(f32)pos->x,(f32)pos->z)!=0;
    f32 x=pos->x,z=pos->z;
    if(sea)waterHeight=gabi::call<f32>(0x0246BA0C,x,z);
    f32 y=pos->y;
    if(sea){
        x=pos->x;
        z=pos->z;
    }
    gabi::store<f32>(water+0x38,x);
    gabi::store<f32>(water+0x3c,y-1000.f);
    gabi::store<f32>(water+0x44,y+1000.f);
    gabi::store<f32>(water+0x40,z);
    if(gabi::call<s32>(0x024EF7C0,dComIfG_Bgsp(),gabi::at<void>(water))) {
        if(!sea)waterHeight=gabi::load<f32>(water+0x48);
        roomID=gabi::call<s32>(0x024EECE8,dComIfG_Bgsp(),gabi::at<void>(water));
    }
    if(room)*room=roomID;
    if(height)*height=waterHeight;
    if(!(waterHeight>-1000000000.f))return FALSE;
    const u32 ground=0x10462EDC;
    ground_filter_init(ground,0x10462F88,0x101B3CFC);
    x=pos->x;
    y=pos->y;
    z=pos->z;
    gabi::store<f32>(ground+0x24,x);
    gabi::store<f32>(ground+0x2c,z);
    gabi::store<f32>(ground+0x28,y+20000.f);
    f32 groundHeight=gabi::call<f32>(0x02008974,dComIfG_Bgsp(),gabi::at<void>(ground));
    return !(groundHeight>waterHeight-100.f);
}
VERIFY(0x021150F4,get_water_height);
static void eff_break_tsubo(Act_c* a) {
    WWHD_FUNC(0x021153B4,void,a);
    const u32 ground=0x10462F30;
    ground_filter_init(ground,0x10462F8C,0x101B3D08);
    f32 x=a->current.pos.x,y=a->current.pos.y,z=a->current.pos.z;
    gabi::store<f32>(ground+0x24,x);
    gabi::store<f32>(ground+0x2c,z);
    gabi::store<f32>(ground+0x28,y+200.f);
    gabi::store<u32>(ground+8,*member<be<u32>>(a,4));
    gabi::call<f32>(0x02008974,dComIfG_Bgsp(),gabi::at<void>(ground));
    s32 material=gabi::call<s32>(0x024EECAC,dComIfG_Bgsp(),gabi::at<void>(ground+0x14));
    s32 reverb=gabi::call<s32>(0x02520540,(s32)(s8)a->current.roomNo);
    gabi::call(0x025E1A40,0x6806,&a->current.pos,material,reverb);
    gabi::Local<cXyz> center,scale,origin;
    center->copy(a->current.pos);
    scale->x=2;
    scale->y=2;
    scale->z=2;
    origin->x=0;
    origin->y=0;
    origin->z=0;
    gabi::call(0x028E8F64,&a->barrelMatrix,origin.get(),center.get());
    void* modelData=dComIfG_getObjectRes(gabi::at<char>(0x1000CB88),49,0x1000C9A0);
    void* pattern=dComIfG_getObjectRes(gabi::at<char>(0x1000CB88),103,0x1000C9A0);
    void* emitter=gabi::call<void*>(0x025A847C,dComIfGp_getParticle(),0,23,center.get(),nullptr,scale.get(),255,nullptr,-1,nullptr,nullptr,0);
    if(emitter) {
        void* modelEmitter=gabi::call<void*>(0x025A3BDC,nullptr,emitter,modelData,0,&a->tevStr,pattern,0,0);
        if(modelEmitter){
            u32 particle=gabi::ea(dComIfGp_getParticle());
            gabi::call(0x0200FE78,gabi::at<void>(gabi::load<u32>(particle+0x130)),modelEmitter);
        }
    }
    gabi::Local<be<u8>[4]> color;
    (*color)[1]=(u8)*member<be<s16>>(a,0x1a2);
    (*color)[2]=(u8)*member<be<s16>>(a,0x1a4);
    (*color)[3]=(u8)*member<be<s16>>(a,0x1a6);
    (*color)[0]=(u8)*member<be<s16>>(a,0x1a0);
    gabi::call(0x025A847C,dComIfGp_getParticle(),0,24,center.get(),nullptr,scale.get(),255,gabi::at<void>(0x1047B254),-1,color.get(),member<void>(a,0x1a8),0);
}
VERIFY(0x021153B4,eff_break_tsubo);
static void coming_start_main(Act_c* a) {
    WWHD_FUNC(0x02115788,void,a);
    fopAc_ac_c* player=dComIfGp_getPlayer(0);
    if(!player)return;
    gabi::Local<cXyz> playerPos,actorPos;
    playerPos->copy(player->current.pos);
    actorPos->x=a->current.pos.x;
    actorPos->y=0;
    actorPos->z=a->current.pos.z;
    playerPos->y=0;
    f32 squared=gabi::call<f32>(0x028E8DE8,playerPos.get(),actorPos.get());
    f32 distance=gabi::call<f32>(0x028F4384,squared);
    a->distance=distance;
    f32 limit=get_limit_dist(a);
    if(!(distance>limit)){
        a->delay=30;
        return;
    }
    s16 delay=a->delay;
    if(delay<0)return;
    if(delay>0){
        delay=(s16)(delay-1);
        a->delay=delay;
        if(delay)return;
    }
    gabi::Local<be<f32>> water;
    if(!get_water_height(a,water.get(),nullptr,&a->current.pos))return;
    gabi::Local<SafetyCallback> cb;
    cb->pos=&a->current.pos;
    cb->safe=1;
    gabi::call(0x025D5218,0x021156E4,cb.get());
    if(cb->safe!=1)return;
    gabi::Local<cXyz> position;
    position->x=a->current.pos.x;
    position->y=*water;
    position->z=a->current.pos.z;
    if(a->barrelID==0xffffffff) {
        s32 room=a->current.roomNo;
        s32 idx=get_challenge_id(a);
        u32 item=gabi::load<u32>(0x101B3D14+idx*4);
        gabi::Local<csXyz> angle;
        gabi::call(0x0201A478,angle.get(),0,(s32)(s16)a->shape_angle.y,0);
        u32 parameters=0x117F0100|(item&63);
        u32 id=gabi::call<u32>(0x025D5834,457,parameters,position.get(),room,angle.get(),0,-1,0);
        a->barrelID=id;
        if(id==0xffffffff)return;
    }
    coming_process_init(a,1);
}
VERIFY(0x02115788,coming_start_main);
static void coming_game_main(Act_c* a) {
    WWHD_FUNC(0x02115978,void,a);
    fopAc_ac_c* player=dComIfGp_getPlayer(0);
    gabi::Local<gptr<Act_c>> barrel;
    s32 found=gabi::call<s32>(0x025D54C4,(u32)a->barrelID,barrel.get());
    Act_c* b=*barrel;
    s32 state=a->gameState;
    if(b){
        a->current.pos.copy(b->current.pos);
        a->shape_angle.x=b->shape_angle.x;
        a->shape_angle.y=b->shape_angle.y;
        a->shape_angle.z=b->shape_angle.z;
    }
    switch(state) {
        case 0:
        if(player){
            f32 sq=gabi::call<f32>(0x028E8DE8,&player->current.pos,&a->current.pos);
            f32 dist=gabi::call<f32>(0x028F4384,sq);
            a->distance=dist;
            b=*barrel;
        }
        if(b)a->gameState=1;
        break;
        case 1:
        if(player){
            f32 sq=gabi::call<f32>(0x028E8DE8,&player->current.pos,&a->current.pos);
            a->distance=gabi::call<f32>(0x028F4384,sq);
        }
        if(collision_main(a)||!found){
            a->gameState=3;
            eff_break_tsubo(a);
        }
        else {
            f32 limit=get_limit_dist(a);
            if(!((f32)a->distance>limit)){
                b=*barrel;
                if(b){
                    *member<be<u8>>(b,0x590)=1;
                    a->gameState=2;
                }
            }
        }
        break;
        case 2:if(!found){
            a->barrelID=0xffffffff;
            coming_process_init(a,0);
        }
        break;
        default:coming_process_init(a,2);
        break;
    }
}
VERIFY(0x02115978,coming_game_main);

/* ---- leftover functions of the translation unit ---- */

/* 02115D64 sead::SafeStringBase<char>::assureTerminationImpl_ (empty); vtable slot 1000C9B4, after the destructor 02115D50 */
static void coming3_SafeString_assureTermination(void* p) {
    WWHD_FUNC(0x02115D64, void, p);
}
VERIFY(0x02115D64, coming3_SafeString_assureTermination);
