/* Bubble. Direct WWHD reconstruction: GC implementation is unavailable.
 * Actor allocation 0xE18; HD TU 020A8A9C–020AE137. */
#include "d/actor/d_a_bl.h"
namespace {
template<class T> T* part(void* actor,u32 offset) { return gabi::at<T>(gabi::ea(actor)+offset); }
template<class T> be<T>& field(void* actor,u32 offset) { return *part<be<T>>(actor,offset); }
u32 model(bl_class* actor) { return gabi::load<u32>(gabi::ea((u8*)actor->morph)+0x90); }
u32 matrixStack() { return gabi::load<u32>(0x1018C7B0); }

void draw_SUB(bl_class* actor) {
    WWHD_FUNC(0x020A8A9C,void,actor);
    u32 m=model(actor);
    f32 x=actor->scale.x,z=actor->scale.z,y=actor->scale.y;
    gabi::store<f32>(m+0xBC,x); gabi::store<f32>(m+0xC4,z); gabi::store<f32>(m+0xC0,y);
    y=(f32)actor->current.pos.y+(f32)actor->modelYOffset;
    x=actor->current.pos.x; z=actor->current.pos.z;
    gabi::call(0x0200FAD8,0,x,y,z);
    gabi::call(0x025F1C28,gabi::at<u8>(matrixStack()),(s16)actor->shape_angle.y);
    gabi::call(0x025F1BF4,gabi::at<u8>(matrixStack()),(s16)actor->shape_angle.x);
    gabi::call(0x025F1C5C,gabi::at<u8>(matrixStack()),(s16)actor->shape_angle.z);
    y=-(f32)actor->modelYOffset;
    gabi::call(0x0200FAD8,1,0.0f,y,0.0f);
    mtx_copy(gabi::at<Mtx34>(m+0xC8),gabi::at<Mtx34>(matrixStack()));
    gabi::call(0x025E55A0,(u8*)actor->morph);
    gabi::call(0x02041570,reinterpret_cast<u8*>(&actor->fireHelper));
    u32 environment=gabi::call<u32>(0x02555D0C);
    gabi::call(0x025626A4,gabi::at<u8>(environment),0,&actor->current.pos,part<u8>(actor,0x110));
}
VERIFY(0x020A8A9C,draw_SUB);

enum class NameMatch { Different,Equal,Limit };
NameMatch stageMatch(SafeString* name,SafeString* stage) {
    u32 table=name->__vtbl;
    gabi::call(gabi::load<u32>(table+0x14),name);
    table=name->__vtbl; gabi::call(gabi::load<u32>(table+0x14),name);
    table=stage->__vtbl; u32 left=name->mStringTop;
    gabi::call(gabi::load<u32>(table+0x14),stage);
    u32 right=stage->mStringTop;
    if(left==right) return NameMatch::Equal;
    for(u32 i=0;i<0x40001;++i) {
        u8 a=gabi::load<u8>(left+i),b=gabi::load<u8>(right+i);
        if(a!=b) return NameMatch::Different;
        if(a==0) return NameMatch::Equal;
    }
    return NameMatch::Limit;
}
BOOL daBL_Draw(bl_class* actor) {
    WWHD_FUNC(0x020A8BDC,BOOL,actor);
    u32 m=model(actor);
    u32 environment=gabi::call<u32>(0x02555D0C);
    gabi::call(0x02562F5C,gabi::at<u8>(environment),gabi::at<u8>(m),part<u8>(actor,0x110));
    u8 switchNo=actor->switchNo;
    if(switchNo!=0xFF) {
        u32 save=gabi::load<u32>(0x101F84DC)+0x20;
        s8 room=gabi::load<s8>(0x1047E6C8);
        if(!gabi::call<s32>(0x025BA0C0,gabi::at<u8>(save),switchNo,room)) return TRUE;
    }
    gabi::Local<SafeString> first,stage;
    first->mStringTop=0x1000997C; first->__vtbl=0x100098AC;
    u32 play=gabi::call<u32>(0x025200D4);
    stage->__vtbl=0x100098AC; stage->mStringTop=play+0x5134;
    NameMatch match=stageMatch(first.get(),stage.get());
    bool skipFigure=match==NameMatch::Equal && field<s8>(actor,0x2FE)==0;
    if(!skipFigure) {
        gabi::Local<SafeString> second,otherStage;
        second->__vtbl=0x100098AC; second->mStringTop=0x10009984;
        play=gabi::call<u32>(0x025200D4);
        otherStage->mStringTop=play+0x5134; otherStage->__vtbl=0x100098AC;
        NameMatch secondMatch=stageMatch(second.get(),otherStage.get());
        skipFigure=secondMatch==NameMatch::Equal && field<s8>(actor,0x2FE)==0;
    }
    if(!skipFigure) gabi::call(0x025BED80,0xB4,actor,1.0f,1.0f,1.0f);
    if(field<s16>(actor,0x83A)>20) {
        gabi::call(0x0259138C,(u8*)actor->morph,-1,reinterpret_cast<u8*>(&actor->lightInfo));
        return TRUE;
    }
    u8 state=actor->burning; u32 data=gabi::load<u32>(m+0xAC);
    u32 animation=state?gabi::ea((u8*)actor->textureAnimation):gabi::ea((u8*)actor->colorAnimation);
    f32 frame=gabi::load<f32>(animation+4);
    gabi::call(0x025E7FC4,gabi::at<u8>(animation),gabi::at<u8>(data),frame);
    gabi::call(0x025200D4);
    gabi::call(0x025E5590,(u8*)actor->morph);
    data=gabi::load<u32>(m+0xAC); gabi::store<u32>(data+0x44,0);
    return TRUE;
}
VERIFY(0x020A8BDC,daBL_Draw);

void smoke_set(bl_class* actor) {
    WWHD_FUNC(0x020A8EE0,void,actor);
    u32 emitter=field<u32>(actor,0x7E8);
    if(!emitter) {
        s8 room=actor->current.roomNo;
        u32 play=gabi::call<u32>(0x025200D4);
        u32 particles=gabi::load<u32>(play+0x5AB0);
        gabi::call(0x025A847C,gabi::at<u8>(particles),2,0x2027,part<cXyz>(actor,0x7D8),&actor->shape_angle,0,0xB9,reinterpret_cast<u8*>(&actor->smokeCallback),room,0,0,0);
        emitter=field<u32>(actor,0x7E8);
        if(!emitter) return;
    }
    gabi::store<f32>(emitter+0x240,1.6f);
    gabi::store<f32>(emitter+0x23C,1.6f);
    gabi::store<f32>(emitter+0x238,1.6f);
    emitter=field<u32>(actor,0x7E8); gabi::store<f32>(emitter+0x34,10.0f);
    emitter=field<u32>(actor,0x7E8); gabi::store<u32>(emitter+0x5C,1);
    emitter=field<u32>(actor,0x7E8); gabi::store<f32>(emitter+0x68,8.0f);
    emitter=field<u32>(actor,0x7E8);
    gabi::store<u32>(emitter+0x254,gabi::load<u32>(emitter+0x254)|0x40);
    gabi::store<u32>(gabi::ea(actor)+0x7FA,0xA0A080B4);
}
VERIFY(0x020A8EE0,smoke_set);

void fire_move_set(bl_class* actor) {
    WWHD_FUNC(0x020A8FE8,void,actor);
    u32 effect=actor->variant?0x8123:0x8124;
    if(field<u32>(actor,0x81C)) return;
    s8 room=actor->current.roomNo;
    u32 play=gabi::call<u32>(0x025200D4);
    u32 particles=gabi::load<u32>(play+0x5AB0);
    gabi::call(0x025A847C,gabi::at<u8>(particles),0,effect,&actor->current.pos,0,0,0xFF,reinterpret_cast<u8*>(&actor->fireCallback),room,0,0,0);
    if(actor->variant==1) {
        u32 flags=field<u32>(actor,0x71C);
        actor->bodyCollider.targetState=5; actor->bodyCollider.targetMaterial=12;
        field<u32>(actor,0x71C)=flags|1;
    }
}
VERIFY(0x020A8FE8,fire_move_set);

void fire_emitter_clr(bl_class* actor) {
    WWHD_FUNC(0x020A90AC,void,actor);
    if(field<u32>(actor,0x81C) && actor->timers[2]==0) {
        if(actor->variant==0) {
            u32 play=gabi::call<u32>(0x025200D4);
            u32 player=gabi::load<u32>(play+0x5B2C);
            s16 angle=(s16)(gabi::load<s16>(player+0x32A)+0x8000);
            u32 emitter=field<u32>(actor,0x81C);
            gabi::call(0x028245AC,0,angle,0,gabi::at<u8>(emitter+0x1F0));
            emitter=field<u32>(actor,0x81C); gabi::store<f32>(emitter+0x34,3.0f);
            emitter=field<u32>(actor,0x81C); gabi::store<u16>(emitter+0x60,10);
            emitter=field<u32>(actor,0x81C); gabi::store<f32>(emitter+0x70,50.0f);
            actor->timers[2]=21;
        } else actor->timers[2]=1;
    }
    gabi::call(0x025A5AC8,reinterpret_cast<u8*>(&actor->appearanceCallback));
}
VERIFY(0x020A90AC,fire_emitter_clr);

void fire_kaiten_keisan(bl_class* actor) {
    WWHD_FUNC(0x020A916C,void,actor);
    u32 emitter=field<u32>(actor,0x81C);
    if(!emitter) return;
    if(actor->variant==0) {
        u32 m=model(actor),joint=gabi::load<u32>(m+0x2C);
        u16 flags=gabi::load<u16>(joint+4); u32 matrices=gabi::load<u32>(joint+0x10);
        gabi::store<u16>(joint+4,flags|0x10);
        gabi::call(0x028249B0,gabi::at<u8>(matrices),gabi::at<u8>(emitter+0x1F0),gabi::at<u8>(emitter+0x22C));
        if(actor && gabi::ea(actor)+0x37C) {
            s32 reverb=gabi::call<s32>(0x02520540,(s8)actor->current.roomNo);
            gabi::call(0x025E1A40,0x7023,part<cXyz>(actor,0x37C),0,reverb);
        }
        return;
    }
    if(actor && gabi::ea(actor)+0x37C) {
        s32 reverb=gabi::call<s32>(0x02520540,(s8)actor->current.roomNo);
        gabi::call(0x025E1A40,0x7024,part<cXyz>(actor,0x37C),0,reverb);
    }
    u32 play=gabi::call<u32>(0x025200D4);
    s8 camera=gabi::load<s8>(play+0x5B30);
    play=gabi::call<u32>(0x025200D4);
    u32 m=model(actor),joint=gabi::load<u32>(m+0x2C);
    u16 flags=gabi::load<u16>(joint+4);
    u32 cameraObject=gabi::load<u32>(play+(u32)((s32)camera*0x34)+0x5AF8);
    u32 matrices=gabi::load<u32>(joint+0x10); gabi::store<u16>(joint+4,flags|0x10);
    gabi::call(0x028E90D4,gabi::at<u8>(matrices),gabi::at<u8>(matrixStack()));
    s16 yaw=actor->shape_angle.y;
    s16 relative=(s16)(gabi::load<s16>(cameraObject+0x236)-yaw);
    gabi::call(0x025F1C28,gabi::at<u8>(matrixStack()),relative);
    emitter=field<u32>(actor,0x81C);
    gabi::call(0x028249B0,gabi::at<u8>(matrixStack()),gabi::at<u8>(emitter+0x1F0),gabi::at<u8>(emitter+0x22C));
}
VERIFY(0x020A916C,fire_kaiten_keisan);

BOOL shock_damage_check(bl_class* actor) {
    WWHD_FUNC(0x020A92D0,BOOL,actor);
    u32 play=gabi::call<u32>(0x025200D4);
    u8 type=actor->variant;
    u32 player=gabi::load<u32>(play+0x5B2C);
    if(!(type&0x80) && actor->burning) return FALSE;
    if(!(gabi::load<u32>(player+0x3C0)&0x20000)) return FALSE;
    f32 dz=gabi::load<f32>(player+0x3EC)-(f32)actor->current.pos.z;
    f32 dx=gabi::load<f32>(player+0x3E4)-(f32)actor->current.pos.x;
    f32 distance=gabi::call<f32>(0x028F4384,gabi::fmadds(dx,dx,dz*dz));
    if(!(distance<1000.0f)) return FALSE;
    actor->timers[2]=0; fire_emitter_clr(actor);
    return TRUE;
}
VERIFY(0x020A92D0,shock_damage_check);

void anm_init(bl_class* actor,s32 animation,f32 morph,u8 mode,f32 rate,s32 sound) {
    WWHD_FUNC(0x020A9384,void,actor,animation,morph,mode,rate,sound);
    actor->animationId=animation;
    gabi::Local<SafeString> name; name->mStringTop=0x100099A8; name->__vtbl=0x100098AC;
    u32 resources=gabi::load<u32>(0x101F4F28);
    auto* resource=gabi::call<u8*>(0x026066C4,gabi::at<u8>(resources),name.get(),animation);
    u8* soundResource=nullptr;
    if(sound>=0) {
        gabi::Local<SafeString> soundName; soundName->mStringTop=0x100099A8; soundName->__vtbl=0x100098AC;
        resources=gabi::load<u32>(0x101F4F28);
        soundResource=gabi::call<u8*>(0x026066C4,gabi::at<u8>(resources),soundName.get(),sound);
    }
    gabi::call(0x025E4A98,(u8*)actor->morph,resource,mode,soundResource,morph,rate,0.0f,-1.0f);
}
VERIFY(0x020A9384,anm_init);

BOOL skull_atari_check(bl_class* actor) {
    WWHD_FUNC(0x020A94B0,BOOL,actor);
    u32 play=gabi::call<u32>(0x025200D4);
    u32 player=gabi::load<u32>(play+0x5B2C);
    gabi::call(0x02515E50,part<u8>(actor,0x668));
    if((s8)actor->health==0) return TRUE;
    if(shock_damage_check(actor)) {
        actor->action=3; actor->actionStep=34;
        return TRUE;
    }
    if(!gabi::call<s32>(0x025162A4,reinterpret_cast<u8*>(&actor->bodyCollider))) return FALSE;
    u32 hit=gabi::call<u32>(0x02516300,reinterpret_cast<u8*>(&actor->bodyCollider));
    if(!hit || actor->timers[4]) return FALSE;
    play=gabi::call<u32>(0x025200D4);
    u32 opponent=gabi::load<u32>(play+0x12A0+0x488C);
    s32 angle=gabi::call<s32>(0x025D6894,actor,gabi::at<u8>(opponent));
    actor->damageKind=0; actor->timers[4]=8;
    actor->current.angle.y=(s16)((u32)angle+0x8000u);
    u32 attack=gabi::load<u32>(hit+0x10);
    switch(attack) {
    case 2: actor->damageKind=6; break;
    case 0x200000:
        actor->damageKind=1; actor->action=3; actor->actionStep=36;
        return TRUE;
    case 0x10000:
        actor->damageKind=4;
        if(gabi::load<u8>(player+0x3AC)==17) {
            s16 yaw=gabi::load<s16>(player+0x32A); field<f32>(actor,0x340)=30.0f;
            actor->current.angle.y=(s16)(yaw-0x4000); actor->damageKind=5; actor->speedF=20.0f;
        }
        break;
    case 0x40: case 0x80: actor->damageKind=3; break;
    case 0x20: field<f32>(actor,0x340)=30.0f; [[fallthrough]];
    case 0x200: field<s16>(actor,0xBE8)=1000; break;
    case 0x8000:
        actor->damageKind=7; actor->action=5; actor->actionStep=60;
        return TRUE;
    default: break;
    }
    if(gabi::ea(actor)+0x37C) {
        s32 reverb=gabi::call<s32>(0x02520540,(s8)actor->current.roomNo);
        gabi::call(0x025E1A40,0x692B,part<cXyz>(actor,0x37C),0,reverb);
    }
    actor->health=0; field<u32>(actor,0x39C)=0;
    actor->action=10; actor->actionStep=103;
    return TRUE;
}
VERIFY(0x020A94B0,skull_atari_check);

void hitSound(bl_class* actor,u32 sound,u32 volume) {
    if(gabi::ea(actor)+0x37C) {
        s32 reverb=gabi::call<s32>(0x02520540,(s8)actor->current.roomNo);
        gabi::call(0x025E1A40,sound,part<cXyz>(actor,0x37C),volume,reverb);
    }
}
void stopFireSound(bl_class* actor) {
    if(gabi::ea(actor)+0x37C) {
        hitSound(actor,0x588B,0);
        s8 room=actor->current.roomNo;
        u32 id=actor?field<u32>(actor,4):0xFFFFFFFF;
        s32 reverb=gabi::call<s32>(0x02520540,room);
        gabi::call(0x025E1AA4,0x4873,part<cXyz>(actor,0x37C),id,0,reverb);
    }
}
void collisionParticle(bl_class* actor,u32 effect,cXyz* position,void* rotation=nullptr,cXyz* scale=nullptr) {
    u32 play=gabi::call<u32>(0x025200D4);
    u32 particles=gabi::load<u32>(play+0x5AB0);
    gabi::call(0x025A847C,gabi::at<u8>(particles),0,effect,position,rotation,scale,0xFF,0,-1,0,0,0);
}
struct AttackInfo { u8 bytes[0x20]; };
void checkAttack(bl_class* actor,AttackInfo* info) {
    u32 hit=gabi::call<u32>(0x02516300,reinterpret_cast<u8*>(&actor->bodyCollider));
    field<u32>(info,0)=hit; field<u32>(info,0x14)=0;
    gabi::call(0x025192A8,actor,info);
}
BOOL blue_body_atari_check(bl_class* actor) {
    WWHD_FUNC(0x020A9744,BOOL,actor);
    u32 play=gabi::call<u32>(0x025200D4);
    u32 player=gabi::load<u32>(play+0x5B2C);
    gabi::call(0x02515E50,part<u8>(actor,0x668));
    if(shock_damage_check(actor)) {
        actor->action=3; actor->actionStep=34;
        return TRUE;
    }
    if(!gabi::call<s32>(0x025162A4,reinterpret_cast<u8*>(&actor->bodyCollider))) return FALSE;
    u32 hit=gabi::call<u32>(0x02516300,reinterpret_cast<u8*>(&actor->bodyCollider));
    if(!hit || actor->timers[4]) return FALSE;
    play=gabi::call<u32>(0x025200D4);
    u32 opponent=gabi::load<u32>(play+0x12A0+0x488C);
    s32 angle=gabi::call<s32>(0x025D6894,actor,gabi::at<u8>(opponent));
    actor->damageKind=0;
    actor->current.angle.y=(s16)((u32)angle+0x8000u); actor->timers[4]=8;
    u32 attack=gabi::load<u32>(hit+0x10);
    bool suppressDamage=false;
    gabi::Local<AttackInfo> info;
    switch(attack) {
    case 0x08000000:
        if(actor->burning) {
            s8 count=actor->stolenItemCount; actor->stealItemLeft=count;
            if(count>0) {
                s8 health=actor->health; actor->health=10;
                checkAttack(actor,info.get()); actor->health=health;
            }
            hitSound(actor,0x2834,0x42);
            count=actor->stolenItemCount; if(count>0) actor->stolenItemCount=count-1;
            collisionParticle(actor,0x27B,part<cXyz>(actor,0x390));
            suppressDamage=true;
        } else {
            actor->stealItemLeft=0; hitSound(actor,0x2834,0x33);
        }
        break;
    case 2: {
        u32 volume=actor->burning?0x42:0x33;
        hitSound(actor,0x2803,volume);
        u8 action=gabi::load<u8>(player+0x3AC);
        if((action>=5 && action<=10) || action==12 || (action>=14 && action<=16) || action==21 || action==23 || (action>=25 && action<=27) || (action>=30 && action<=31)) actor->damageKind=6;
        break;
    }
    case 0x20:
        if(!actor->burning) { actor->damageKind=2; break; }
        [[fallthrough]];
    case 0x200000: {
        u32 ground=field<u32>(actor,0x4B0);
        actor->action=3; actor->actionStep=36;
        suppressDamage=true; actor->damageKind=1;
        if(!(ground&0x20) && (f32)field<f32>(actor,0x374)>-2.0f) actor->actionStep=30;
        break;
    }
    case 0x10000: {
        u32 volume=actor->burning?0x42:0x33; hitSound(actor,0x2855,volume);
        actor->damageKind=4;
        u8 action=gabi::load<u8>(player+0x3AC);
        u8 burning=actor->burning;
        if(action==17) { s16 yaw=gabi::load<s16>(player+0x32A); actor->damageKind=5; actor->current.angle.y=(s16)(yaw-0x4000); }
        if(!burning) actor->health=0;
        break;
    }
    case 0x40: case 0x80: {
        suppressDamage=true;
        u32 volume=actor->burning?0x42:0x33; hitSound(actor,0x2833,volume);
        actor->damageKind=3;
        break;
    }
    case 0x200: case 0x40000:
        if(!actor->burning) {
            field<s16>(actor,0xBE8)=100; stopFireSound(actor);
            u32 flags=actor->bodyCollider.attackFlags; actor->timers[2]=0;
            field<u32>(actor,0x39C)=0; actor->bodyCollider.attackFlags=flags&~1u;
            fire_emitter_clr(actor);
        }
        break;
    case 0x100000: {
        field<f32>(actor,0x9D8)=1.0f; field<f32>(actor,0x834)=40.0f;
        field<u8>(actor,0x832)=1; stopFireSound(actor);
        u32 flags=actor->bodyCollider.attackFlags;
        field<f32>(actor,0x374)=0.0f; actor->burning=0;
        actor->shape_angle.z=0; field<f32>(actor,0x340)=0.0f;
        actor->shape_angle.x=0; actor->current.angle.x=0;
        actor->timers[0]=0; field<u32>(actor,0x39C)=0;
        actor->speedF=0.0f; actor->timers[2]=0;
        field<f32>(actor,0x344)=0.0f; field<f32>(actor,0x33C)=0.0f;
        actor->current.angle.z=0; actor->bodyCollider.attackFlags=flags&~1u;
        fire_emitter_clr(actor);
        break;
    }
    case 0x80000:
        suppressDamage=true;
        if(!actor->burning) {
            u32 flags=actor->bodyCollider.attackFlags;
            actor->current.angle.z=0; field<s16>(actor,0x830)=200;
            actor->bodyCollider.attackFlags=flags&~1u; actor->burning=0;
            actor->timers[0]=0; field<f32>(actor,0x374)=0.0f;
            actor->shape_angle.z=0; actor->speedF=0.0f;
            field<f32>(actor,0x33C)=0.0f; field<f32>(actor,0x344)=0.0f;
            actor->current.angle.x=0; actor->shape_angle.x=0;
            field<f32>(actor,0x340)=0.0f;
            gabi::call(0x02041C30,reinterpret_cast<u8*>(&actor->fireHelper)); field<u32>(actor,0x39C)=0;
        } else {
            actor->action=3; actor->damageKind=1; actor->actionStep=30;
        }
        break;
    case 0x8000: {
        suppressDamage=true;
        u32 volume=actor->burning?0x42:0x33; hitSound(actor,0x2834,volume);
        actor->damageKind=7; actor->actionStep=60; actor->action=5;
        break;
    }
    default: {
        u32 volume=actor->burning?0x42:0x33; hitSound(actor,0x2834,volume);
        break;
    }
    }
    u8 burning=actor->burning;
    gabi::Local<cXyz> impact,scale;
    impact->x=field<f32>(actor,0x754); impact->y=field<f32>(actor,0x758); impact->z=field<f32>(actor,0x75C);
    if(burning) {
        u8 kind=actor->damageKind;
        if(kind!=7 && kind!=1) {
            collisionParticle(actor,12,impact.get());
            actor->action=2; actor->actionStep=20;
            return TRUE;
        }
    }
    if(!suppressDamage) {
        checkAttack(actor,info.get());
        u8 kind=actor->damageKind;
        if((u32)(kind-4)<=2 || (s8)actor->health<=0) {
            collisionParticle(actor,16,impact.get());
            scale->x=2.0f; scale->z=2.0f; scale->y=2.0f;
            collisionParticle(actor,15,impact.get(),gabi::at<u8>(player+0x328),scale.get());
        } else collisionParticle(actor,13,impact.get(),gabi::at<u8>(player+0x328));
        actor->action=4; actor->actionStep=40;
    }
    return TRUE;
}
VERIFY(0x020A9744,blue_body_atari_check);

BOOL red_body_atari_check(bl_class* actor) {
    WWHD_FUNC(0x020AA068,BOOL,actor);
    u32 play=gabi::call<u32>(0x025200D4);
    u32 player=gabi::load<u32>(play+0x5B2C);
    gabi::call(0x02515E50,part<u8>(actor,0x668));
    if(shock_damage_check(actor)) {
        actor->action=3; actor->actionStep=34;
        return TRUE;
    }
    if(!gabi::call<s32>(0x025162A4,reinterpret_cast<u8*>(&actor->bodyCollider))) return FALSE;
    u32 hit=gabi::call<u32>(0x02516300,reinterpret_cast<u8*>(&actor->bodyCollider));
    if(!hit || actor->timers[4]) return FALSE;
    play=gabi::call<u32>(0x025200D4);
    u32 opponent=gabi::load<u32>(play+0x12A0+0x488C);
    s32 angle=gabi::call<s32>(0x025D6894,actor,gabi::at<u8>(opponent));
    actor->current.angle.y=(s16)((u32)angle+0x8000u);
    gabi::Local<cXyz> impact,scale;
    impact->x=field<f32>(actor,0x754); impact->z=field<f32>(actor,0x75C); impact->y=field<f32>(actor,0x758);
    actor->timers[4]=8; actor->damageKind=0;
    u32 attack=gabi::load<u32>(hit+0x10);
    gabi::Local<AttackInfo> info;
    switch(attack) {
    case 0x08000000:
        if(actor->burning) {
            s8 count=actor->stolenItemCount; actor->stealItemLeft=count;
            if(count>0) {
                s8 health=actor->health; actor->health=10;
                checkAttack(actor,info.get()); actor->health=health;
            }
            hitSound(actor,0x2834,0x42);
            count=actor->stolenItemCount; if(count>0) actor->stolenItemCount=count-1;
            collisionParticle(actor,0x27B,part<cXyz>(actor,0x390));
            return TRUE;
        }
        actor->stealItemLeft=0; hitSound(actor,0x2834,0x33);
        break;
    case 2: {
        actor->damageKind=8; hitSound(actor,0x2803,0x33);
        u8 action=gabi::load<u8>(player+0x3AC);
        if((action>=5 && action<=10) || action==12 || (action>=14 && action<=16) || action==21 || action==23 || (action>=25 && action<=27) || (action>=30 && action<=31)) actor->damageKind=6;
        break;
    }
    case 0x20:
        if(!actor->burning) { actor->damageKind=2; break; }
        [[fallthrough]];
    case 0x200000: {
        u32 ground=field<u32>(actor,0x4B0); actor->damageKind=1;
        actor->action=3; actor->actionStep=36;
        if(!(ground&0x20) && (f32)field<f32>(actor,0x374)>-2.0f) actor->actionStep=30;
        return TRUE;
    }
    case 0x10000:
        hitSound(actor,0x2855,0x33); actor->damageKind=4;
        if(gabi::load<u8>(player+0x3AC)==17) {
            s16 yaw=gabi::load<s16>(player+0x32A); actor->damageKind=5;
            actor->current.angle.y=(s16)(yaw-0x4000);
        }
        actor->health=0;
        break;
    case 0x40: case 0x80: {
        u32 volume=actor->burning?0x42:0x33; hitSound(actor,0x2833,volume);
        actor->damageKind=3; collisionParticle(actor,12,impact.get());
        return TRUE;
    }
    case 0x200: case 0x4000: case 0x40000: {
        u8 burning=actor->burning;
        if(burning && (attack&0x200)) break;
        if(!burning && !(attack&0x4000)) field<s16>(actor,0xBE8)=100;
        stopFireSound(actor);
        u32 flags=actor->bodyCollider.attackFlags; actor->timers[2]=0;
        field<u32>(actor,0x39C)=0; actor->bodyCollider.attackFlags=flags&~1u;
        fire_emitter_clr(actor);
        break;
    }
    case 0x100000: {
        field<f32>(actor,0x9D8)=1.0f; field<u8>(actor,0x832)=1; field<f32>(actor,0x834)=40.0f;
        stopFireSound(actor); u32 flags=actor->bodyCollider.attackFlags;
        field<f32>(actor,0x374)=0.0f; actor->burning=0;
        actor->shape_angle.z=0; field<f32>(actor,0x340)=0.0f;
        actor->shape_angle.x=0; actor->current.angle.x=0;
        actor->timers[0]=0; field<u32>(actor,0x39C)=0;
        actor->speedF=0.0f; actor->timers[2]=0;
        field<f32>(actor,0x344)=0.0f; field<f32>(actor,0x33C)=0.0f;
        actor->current.angle.z=0; actor->bodyCollider.attackFlags=flags&~1u;
        fire_emitter_clr(actor);
        break;
    }
    case 0x80000:
        actor->timers[2]=0; actor->speedF=0.0f;
        field<f32>(actor,0x340)=0.0f; actor->burning=0;
        actor->shape_angle.z=0; actor->current.angle.x=0;
        field<f32>(actor,0x33C)=0.0f; field<f32>(actor,0x374)=0.0f;
        actor->current.angle.z=0; actor->shape_angle.x=0;
        field<f32>(actor,0x344)=0.0f; actor->timers[0]=0;
        fire_emitter_clr(actor);
        field<s16>(actor,0x830)=200;
        actor->bodyCollider.attackFlags=(u32)actor->bodyCollider.attackFlags&~1u;
        gabi::call(0x02041C30,reinterpret_cast<u8*>(&actor->fireHelper)); field<u32>(actor,0x39C)=0;
        return TRUE;
    case 0x8000: {
        u32 volume=actor->burning?0x42:0x33; hitSound(actor,0x2834,volume);
        actor->damageKind=7; actor->action=5; actor->actionStep=60;
        return TRUE;
    }
    default: {
        u32 volume=actor->burning?0x42:0x33; hitSound(actor,0x2834,volume);
        collisionParticle(actor,12,impact.get());
        return TRUE;
    }
    }
    checkAttack(actor,info.get());
    u8 kind=actor->damageKind;
    if((u32)(kind-4)<=2 || (s8)actor->health<=0) {
        collisionParticle(actor,16,impact.get());
        scale->x=2.0f; scale->z=2.0f; scale->y=2.0f;
        collisionParticle(actor,15,impact.get(),gabi::at<u8>(player+0x328),scale.get());
    } else collisionParticle(actor,13,impact.get(),gabi::at<u8>(player+0x328));
    if(actor->burning) {
        anm_init(actor,23,1.0f,0,1.0f,-1);
        actor->speedF=gabi::load<f32>(0x1047BAB8)+60.0f;
        if((s8)actor->health<=0) {
            u8 kind=actor->damageKind;
            actor->speedF=gabi::load<f32>(0x1047BABC)+60.0f;
            if(kind==5) actor->current.angle.y=(s16)(gabi::load<s16>(player+0x32A)-0x4000);
        }
        if(gabi::ea(actor)+0x37C) {
            u32 id=actor?field<u32>(actor,4):0xFFFFFFFF;
            s32 reverb=gabi::call<s32>(0x02520540,(s8)actor->current.roomNo);
            gabi::call(0x025E1AA4,0x4873,part<cXyz>(actor,0x37C),id,0,reverb);
        }
        actor->action=2; actor->actionStep=21;
    } else { actor->action=4; actor->actionStep=40; }
    return TRUE;
}
VERIFY(0x020AA068,red_body_atari_check);

void bound_sound_set(bl_class* actor) {
    WWHD_FUNC(0x020AAB58,void,actor);
    f32 strength=(f32)field<f32>(actor,0x340)*3.3f;
    u32 volume=strength<2147483648.0f ? (u32)gabi::ftoi(strength) : (u32)gabi::ftoi(strength-2147483648.0f)+0x80000000u;
    if(volume>100) volume=100;
    if(gabi::ea(actor)+0x37C) {
        s32 reverb=gabi::call<s32>(0x02520540,(s8)actor->current.roomNo);
        gabi::call(0x025E1A40,0x588C,part<cXyz>(actor,0x37C),volume,reverb);
    }
}
VERIFY(0x020AAB58,bound_sound_set);

void fuwafuwa_keisan(bl_class* actor) {
    WWHD_FUNC(0x020AAC08,void,actor);
    f32 target;
    if(actor->action==1) {
        u32 play=gabi::call<u32>(0x025200D4);
        s16 phase=(s16)(actor->flightPhase+1000); actor->flightPhase=phase;
        u32 player=gabi::load<u32>(play+0x5B2C);
        target=gabi::load<f32>(player+0x318)+80.0f; actor->targetHeight=target;
        f32 sine=gabi::load<f32>(0x104A44F8+((u16)phase>>3)*8);
        target=gabi::fmadds(sine,10.0f,target); actor->targetHeight=target;
        gabi::call(0x0200ED84,&actor->current.pos.y,target,1.0f,6.0f);
    } else {
        s16 phase=(s16)(actor->flightPhase+500);
        target=(f32)actor->homeGroundHeight+100.0f; actor->flightPhase=phase;
        actor->targetHeight=target;
        f32 sine=gabi::load<f32>(0x104A44F8+((u16)phase>>3)*8);
        target=gabi::fmadds(sine,40.0f,target); actor->targetHeight=target;
        gabi::call(0x0200ED84,&actor->current.pos.y,target,1.0f,3.0f);
    }
}
VERIFY(0x020AAC08,fuwafuwa_keisan);

void BG_check(bl_class* actor) {
    WWHD_FUNC(0x020AAD08,void,actor);
    if(actor->actionStep==2) return;
    gabi::call(0x024EFF44,reinterpret_cast<u8*>(&actor->groundCircle),40.0f,40.0f);
    u32 play=gabi::call<u32>(0x025200D4);
    gabi::call(0x024F08A8,reinterpret_cast<u8*>(&actor->groundCollision),gabi::at<u8>(play+0x12A0));
    field<u32>(actor,0x4B0)=(u32)field<u32>(actor,0x4B0)|0x2000;
    if(actor->flying==0) {
        f32 ground=field<f32>(actor,0x51C);
        if(ground==-1000000000.0f) actor->homeGroundHeight=ground;
    }
}
VERIFY(0x020AAD08,BG_check);

BOOL roll_check(bl_class* actor) {
    WWHD_FUNC(0x020AAD90,BOOL,actor);
    gabi::call(0x0200F428,&actor->shape_angle.x,0,1,0x1000);
    gabi::call(0x0200F428,&actor->shape_angle.z,0,1,0x1000);
    s32 x=actor->shape_angle.x; actor->spinX=0; actor->spinZ=0;
    if(x<0) x=-x;
    if(x>=0x200) return FALSE;
    s32 z=actor->shape_angle.z; if(z<0) z=-z;
    return z<0x200;
}
VERIFY(0x020AAD90,roll_check);

struct LineCheck { u8 bytes[0x6C]; };
s16 way_check(bl_class* actor,s16 angle) {
    WWHD_FUNC(0x020AAE38,s16,actor,angle);
    gabi::Local<LineCheck> line;
    gabi::call(0x02008FEC,line.get());
    field<u32>(line.get(),0)=gabi::ea(line.get())+0x58;
    field<u32>(line.get(),0x10)=0x10009924;
    field<u8>(line.get(),0x62)=0; field<u8>(line.get(),0x5D)=0; field<u8>(line.get(),0x61)=0;
    field<u32>(line.get(),0x20)=0x10009934;
    field<u8>(line.get(),0x60)=0;
    field<u32>(line.get(),0x68)=1;
    field<u32>(line.get(),4)=gabi::ea(line.get())+0x64;
    field<u8>(line.get(),0x5E)=0;
    field<u32>(line.get(),0x58)=0x10009954;
    field<u32>(line.get(),0x64)=0x10009944;
    field<u8>(line.get(),0x5C)=0; field<u8>(line.get(),0x5F)=0;
    s32 step=gabi::call<f32>(0x02019918,1.0f)<0.5f?-0x2000:0x2000;
    s16 direction=angle;
    gabi::Local<cXyz> forward,destination;
    for(u32 attempts=0;attempts<8;++attempts) {
        gabi::call(0x025F1884,gabi::at<u8>(matrixStack()),direction);
        forward->x=0.0f; forward->y=0.0f; forward->z=300.0f;
        gabi::call(0x0200FCD8,forward.get(),destination.get());
        gabi::call(0x028E8D88,destination.get(),&actor->current.pos,destination.get());
        gabi::call(0x024F1AFC,line.get(),&actor->current.pos,destination.get(),actor);
        u32 play=gabi::call<u32>(0x025200D4);
        BOOL blocked=gabi::call<BOOL>(0x02008860,gabi::at<u8>(play+0x12A0),line.get());
        if(!blocked) {
            field<u32>(line.get(),0x58)=0x10009954;
            field<u32>(line.get(),0x64)=0x100098E4;
            field<u32>(line.get(),0x20)=0x100098D4;
            gabi::call(0x02008B4C,line.get(),0);
            return direction;
        }
        direction=(s16)(direction+step);
    }
    field<u32>(line.get(),0x58)=0x10009954;
    field<u32>(line.get(),0x64)=0x100098E4;
    field<u32>(line.get(),0x20)=0x100098D4;
    gabi::call(0x02008B4C,line.get(),0);
    return angle;
}
VERIFY(0x020AAE38,way_check);

// The HD compiler inlines these action states into daBL_Execute.
bool hasPath(bl_class* a) { return a->pathNo!=0xFF && a->path!=0; }
u32 opponent() { u32 play=gabi::call<u32>(0x025200D4); return gabi::load<u32>(play+0x12A0+0x488C); }
bool animationStopped(bl_class* a) { u32 m=gabi::ea((u8*)a->morph); return (gabi::load<u8>(m+0xA7)&1) || gabi::load<f32>(m+0x98)==0.0f; }
void updateAppearanceEmitter(bl_class* a,u32 emitter) {
    u32 joint=gabi::load<u32>(model(a)+0x2C);
    u16 flags=gabi::load<u16>(joint+4);
    u32 matrices=gabi::load<u32>(joint+0x10);
    gabi::store<u16>(joint+4,flags|0x10);
    gabi::call(0x028249B0,gabi::at<u8>(matrices),gabi::at<u8>(emitter+0x1F0),gabi::at<u8>(emitter+0x22C));
}
void actorSound(bl_class* a,u32 sound) {
    if(gabi::ea(a)+0x37C==0) return;
    s8 room=a->current.roomNo; u32 id=a?gabi::load<u32>(gabi::ea(a)+4):0xFFFFFFFF;
    s32 reverb=gabi::call<s32>(0x02520540,room);
    gabi::call(0x025E1AA4,sound,part<u8>(a,0x37C),id,0,reverb);
}
bool collisionBody(bl_class* a) { return a->variant==1?blue_body_atari_check(a):red_body_atari_check(a); }
void pathDelta(bl_class* a,f32& x,f32& z) {
    u32 path=a->path,points=gabi::load<u32>(path+8);
    s8 index=a->pathPoint; u32 point=points+(s32)index*16;
    x=gabi::load<f32>(point+4)-(f32)a->current.pos.x;
    z=gabi::load<f32>(point+12)-(f32)a->current.pos.z;
}
void actionRoam(bl_class* a,bool& forcePlay) {
    s16 state=a->actionStep;
    switch(state) {
    case 0: {
        a->flightPhase=0;
        if(gabi::call<f32>(0x025D68EC,a,gabi::at<u8>(opponent()))>1000.0f) break;
        field<u32>(a,0x39C)=4; field<u32>(a,0x2E0)=(u32)field<u32>(a,0x2E0)|0x20;
        anm_init(a,21,1.0f,0,1.0f,-1); a->actionStep=(s16)((s16)a->actionStep+1); break;
    }
    case 1: {
        a->targetScale=1.5f; u32 emitter=field<u32>(a,0x808);
        if(!emitter) {
            u32 play=gabi::call<u32>(0x025200D4),particles=gabi::load<u32>(play+0x5AB0);
            gabi::call(0x025A847C,gabi::at<u8>(particles),0,0x8122,&a->current.pos,0,0,0xFF,reinterpret_cast<u8*>(&a->appearanceCallback),-1,0,0,0);
            if(a && gabi::ea(a)+0x37C) hitSound(a,0x5888,0);
            emitter=field<u32>(a,0x808);
        }
        if(emitter) {
            bool red=a->variant==0;
            gabi::store<u8>(emitter+0x248,red?0x55:0xD);
            gabi::store<u8>(emitter+0x249,red?0x1A:0x20);
            gabi::store<u8>(emitter+0x24A,red?0xD:0x41);
        }
        if(!animationStopped(a)) break;
        a->actionStep=(s16)((s16)a->actionStep+1);
        field<f32>(a,0x374)=0; field<f32>(a,0x340)=0;
        [[fallthrough]];
    }
    case 2: {
        u32 emitter=field<u32>(a,0x808); if(emitter) updateAppearanceEmitter(a,emitter);
        gabi::call(0x0200ED84,part<u8>(a,0x340),10.0f,0.8f,1.0f);
        if((f32)a->current.pos.y<(f32)a->homeGroundHeight+100.0f) break;
        fire_move_set(a); field<f32>(a,0x340)=0;
        hitSound(a,0x5889,0); actorSound(a,0x4871);
        anm_init(a,22,1.0f,0,1.0f,-1);
        a->actionStep=(s16)((s16)a->actionStep+1); break;
    }
    case 3:
        if(gabi::call<BOOL>(0x027F2BF8,gabi::at<u8>(gabi::ea((u8*)a->morph)+0x98),10.0f)) gabi::call(0x025A5AC8,reinterpret_cast<u8*>(&a->appearanceCallback));
        if(animationStopped(a)) a->actionStep=5;
        break;
    case 4:
        fire_move_set(a); field<u32>(a,0x2E0)=(u32)field<u32>(a,0x2E0)|0x20;
        a->timers[3]=5; a->actionStep=5;
        [[fallthrough]];
    case 5: {
        field<u32>(a,0x2E0)=(u32)field<u32>(a,0x2E0)|0x20;
        if(a->animationId!=17) {
            anm_init(a,17,1.0f,2,1.0f,-1);
            u32 flags=a->bodyCollider.attackFlags; a->burning=1; field<u8>(a,0x660)=2;
            a->bodyCollider.attackFlags=flags|1; a->bodyCollider.attackType=1;
        }
        field<u32>(a,0x39C)=4; s16 heading;
        if(hasPath(a)) {
            f32 x,z; pathDelta(a,x,z); heading=gabi::call<s16>(0x020195B0,x,z);
            a->speedF=a->pathSpeed;
        } else {
            a->timers[0]=(s16)gabi::ftoi(gabi::call<f32>(0x02019918,50.0f)+100.0f);
            heading=(s16)gabi::ftoi(gabi::call<f32>(0x02019918,32767.0f));
            if((f32)a->speedF==0.0f) a->speedF=gabi::call<f32>(0x020198D8,2.0f)+4.0f;
        }
        a->targetYaw=way_check(a,heading); a->actionStep=(s16)((s16)a->actionStep+1);
        [[fallthrough]];
    }
    case 6: {
        if(hasPath(a)) {
            f32 x,z; pathDelta(a,x,z); s32 heading=gabi::call<s32>(0x020195B0,x,z);
            f32 distance=gabi::call<f32>(0x028F4384,gabi::fmadds(x,x,z*z));
            f32 threshold=gabi::load<f32>(0x1047BA9C)+80.0f;
            u32 savedY=field<u32>(a,0x318),savedX=field<u32>(a,0x314);
            if(distance<threshold) {
                s8 index=(s8)((s8)a->pathPoint+1); a->pathPoint=index;
                if(index>=gabi::load<u16>((u32)a->path)) a->pathPoint=0;
            }
            u32 savedZ=field<u32>(a,0x31C);
            field<u32>(a,0x3E0)=savedX; field<u32>(a,0x3E4)=savedY; field<u32>(a,0x3E8)=savedZ;
            if(a->timers[0]==0) {
                s16 next=way_check(a,(s16)heading); a->targetYaw=next;
                if(next!=heading) a->timers[0]=(s16)gabi::ftoi(gabi::call<f32>(0x020198D8,10.0f)+10.0f);
            }
            break;
        }
        f32 x=(f32)a->current.pos.x-(f32)a->roamingOrigin.x,z=(f32)a->current.pos.z-(f32)a->roamingOrigin.z;
        if(gabi::call<f32>(0x028F4384,gabi::fmadds(x,x,z*z))<250.0f) {
            if(a->timers[0]==0) a->actionStep=5;
            break;
        }
        a->actionStep=7;
        [[fallthrough]];
    }
    case 7: {
        f32 x=(f32)a->roamingOrigin.x-(f32)a->current.pos.x,z=(f32)a->roamingOrigin.z-(f32)a->current.pos.z;
        a->targetYaw=gabi::call<s16>(0x020195B0,x,z);
        f32 radius=hasPath(a)?80.0f:10.0f;
        if(gabi::call<f32>(0x028F4384,gabi::fmadds(x,x,z*z))<radius) a->actionStep=5;
        break;
    }
    }
    s16 step=a->actionStep;
    if((u32)(step-1)<3) { u32 emitter=field<u32>(a,0x808); if(emitter) updateAppearanceEmitter(a,emitter); }
    if(a->timers[3]==0) fire_kaiten_keisan(a);
    bool path=hasPath(a);
    gabi::call(0x0200F428,&a->current.angle.y,(s16)a->targetYaw,1,path?0x1000:0x120);
    gabi::call(0x0200F428,&a->shape_angle.y,(s16)a->current.angle.y,1,path?0x1000:0x400);
    if((s16)a->actionStep>=3) { a->targetScale=1.5f; fuwafuwa_keisan(a); }
    if(collisionBody(a)) return;
    if((s16)a->actionStep<5) return;
    if((u32)a->bodyCollider.contactFlags&1) {
        if(hasPath(a) && a->actionStep==7) { forcePlay=true; return; }
        s32 angle=gabi::call<s32>(0x025D6894,a,gabi::at<u8>(opponent()));
        a->action=1; a->current.angle.y=(s16)(angle+0x8000); a->speedF=50; a->actionStep=10;
        forcePlay=true; return;
    }
    if(!a->burning) { forcePlay=true; return; }
    if(!(gabi::call<f32>(0x025D68EC,a,gabi::at<u8>(opponent()))<600.0f)) return;
    u32 play=gabi::call<u32>(0x025200D4),player=gabi::load<u32>(play+0x5B2C);
    gabi::Local<cXyz> target,origin; target->x=gabi::load<f32>(player+0x314); target->y=gabi::load<f32>(player+0x318); target->z=gabi::load<f32>(player+0x31C);
    gabi::Local<LineCheck> line; gabi::call(0x02008FEC,line.get());
    field<u32>(line.get(),0x68)=1; field<u8>(line.get(),0x5E)=0; field<u32>(line.get(),0x10)=0x10009924;
    field<u32>(line.get(),0x20)=0x10009934; field<u8>(line.get(),0x60)=0; field<u8>(line.get(),0x5D)=0; field<u8>(line.get(),0x5C)=0; field<u8>(line.get(),0x5F)=0;
    field<u32>(line.get(),0)=gabi::ea(line.get())+0x58; field<u32>(line.get(),4)=gabi::ea(line.get())+0x64;
    field<u32>(line.get(),0x58)=0x10009954; field<u32>(line.get(),0x64)=0x10009944; field<u8>(line.get(),0x62)=0; field<u8>(line.get(),0x61)=0;
    origin->x=a->current.pos.x; origin->y=a->current.pos.y; origin->z=a->current.pos.z;
    target->y=(f32)target->y+100.0f; origin->y=(f32)origin->y+100.0f;
    gabi::call(0x024F1AFC,line.get(),origin.get(),target.get(),a);
    play=gabi::call<u32>(0x025200D4); BOOL blocked=gabi::call<BOOL>(0x02008860,gabi::at<u8>(play+0x12A0),line.get());
    field<u32>(line.get(),0x58)=0x10009954; field<u32>(line.get(),0x64)=0x100098E4; field<u32>(line.get(),0x20)=0x100098D4;
    gabi::call(0x02008B4C,line.get(),0); if(blocked) return;
    if(!(hasPath(a) && a->actionStep==7)) { a->action=1; a->actionStep=10; }
    forcePlay=true;
}
void actionAttack(bl_class* a,bool& forcePlay) {
    u32 play=gabi::call<u32>(0x025200D4),player=gabi::load<u32>(play+0x5B2C);
    u32 morph=0; bool haveMorph=false;
    switch((s16)a->actionStep) {
    case 10:
        for(u32 i=0;i<4;++i) field<s16>(a,0x414+i*2)=0;
        a->bodyCollider.attackFlags=(u32)a->bodyCollider.attackFlags|1; a->bodyCollider.attackType=1; a->flightPhase=0;
        anm_init(a,18,1.0f,0,1.0f,-1); if(a) actorSound(a,0x4870);
        a->actionStep=(s16)((s16)a->actionStep+1); a->speedF=0; break;
    case 11:
        morph=gabi::ea((u8*)a->morph); haveMorph=true;
        if(!(gabi::load<u8>(morph+0xA7)&1) && gabi::load<f32>(morph+0x98)!=0.0f) break;
        a->actionStep=(s16)((s16)a->actionStep+1);
        [[fallthrough]];
    case 12: {
        anm_init(a,16,1.0f,2,1.0f,6); a->speedF=10;
        u32 id=a?gabi::load<u32>(gabi::ea(a)+4):0xFFFFFFFF;
        f32 random=gabi::call<f32>(0x02019918,768.0f);
        a->attackYawOffset=(s16)gabi::ftoi(random*(f32)(id&3));
        a->actionStep=(s16)((s16)a->actionStep+1); haveMorph=false;
        [[fallthrough]];
    }
    case 13:
        if(hasPath(a)) {
            f32 z=(f32)a->current.pos.z-(f32)a->roamingOrigin.z,x=(f32)a->current.pos.x-(f32)a->roamingOrigin.x;
            if(!(gabi::call<f32>(0x028F4384,gabi::fmadds(x,x,z*z))>700.0f)) break;
            anm_init(a,17,1.0f,2,1.0f,-1); a->speedF=gabi::call<f32>(0x020198D8,2.0f)+4.0f;
            a->actionStep=7;
        } else {
            if(!(gabi::call<f32>(0x025D68EC,a,gabi::at<u8>(opponent()))>700.0f)) break;
            a->speedF=gabi::call<f32>(0x020198D8,2.0f)+4.0f; a->actionStep=5;
        }
        a->action=0; forcePlay=true; return;
    case 14:
        gabi::call(0x0200EDC8,&a->speedF,0.5f,1.0f);
        if(!(std::fabs((f32)a->speedF)<0.2f)) break;
        a->speedF=0; anm_init(a,24,1.0f,0,1.0f,-1); actorSound(a,0x4870); a->actionStep=15;
        break;
    case 15:
        morph=gabi::ea((u8*)a->morph); haveMorph=true;
        if((gabi::load<u8>(morph+0xA7)&1) || gabi::load<f32>(morph+0x98)==0.0f) a->actionStep=12;
        break;
    }
    if(!haveMorph) morph=gabi::ea((u8*)a->morph);
    gabi::store<f32>(morph+0x98,1.0f);
    if(a->actionStep==13 && gabi::call<f32>(0x025D68EC,a,gabi::at<u8>(opponent()))<230.0f) gabi::store<f32>(gabi::ea((u8*)a->morph)+0x98,2.0f);
    if((u32)a->bodyCollider.contactFlags&1) {
        if(a->actionStep!=14) { a->action=1; a->speedF=-20; a->actionStep=14; }
    } else if(gabi::call<BOOL>(0x025160DC,reinterpret_cast<u8*>(&a->bodyCollider))) {
        u32 hit=gabi::call<u32>(0x02515BBC,part<u8>(a,0x6D8));
        if(hit && hit==player && a->actionStep!=15) {
            anm_init(a,20,1.0f,0,1.0f,-1); if(a) actorSound(a,0x4872);
            a->speedF=0; a->spinX=0; a->actionStep=15;
        }
    }
    fire_kaiten_keisan(a);
    s32 heading=gabi::call<s32>(0x025D6894,a,gabi::at<u8>(opponent()));
    s16 target=(s16)(heading+(s16)a->attackYawOffset); a->targetYaw=target;
    gabi::call(0x0200F428,&a->current.angle.y,target,1,0x1000);
    gabi::call(0x0200F428,&a->shape_angle.y,(s16)a->current.angle.y,1,0x400);
    if((s16)a->actionStep>=12) fuwafuwa_keisan(a);
    collisionBody(a);
}
void actionRetreat(bl_class* a) {
    u32 play=gabi::call<u32>(0x025200D4),player=gabi::load<u32>(play+0x5B2C);
    s16 state=a->actionStep;
    if(state==20) {
        anm_init(a,23,1.0f,0,1.0f,-1); a->speedF=gabi::load<f32>(0x1047BAB8)+20.0f;
        u8 damage=a->damageKind;
        if(damage==4 || damage==5) {
            a->speedF=gabi::load<f32>(0x1047BABC)+60.0f;
            if(a->damageKind==5) a->current.angle.y=(s16)(gabi::load<s16>(player+0x32A)-0x4000);
        }
        a->actionStep=(s16)((s16)a->actionStep+1); state=21;
    }
    if(state==21) {
        gabi::call(0x0200EDC8,&a->speedF,0.8f,5.0f);
        if((f32)a->speedF<0.1f) {
            a->action=1; a->actionStep=10;
            if(a->burning && a->variant==0 && field<s8>(a,0x3A1)<=0) {
                gabi::Local<cXyz> position; position->x=a->current.pos.x; position->y=a->current.pos.y; position->z=a->current.pos.z;
                position->y=(f32)position->y+30.0f;
                gabi::call(0x025D99E8,a,position.get(),5,0,0xFF);
                s8 room=field<s8>(a,0x2FE); field<u32>(a,0x39C)=0;
                u32 save=gabi::load<u32>(0x101F84DC)+0x20; u16 id=field<u16>(a,0x2D8);
                gabi::call(0x025BA5D4,gabi::at<u8>(save),id,room); gabi::call(0x025D57E0,a);
            }
        }
    }
    fire_kaiten_keisan(a); collisionBody(a);
}
void actionWind(bl_class* a) {
    bool followed=false;
    switch((s16)a->actionStep) {
    case 30: {
        for(u32 i=0;i<4;++i) field<s16>(a,0x414+i*2)=0;
        a->bodyCollider.targetState=0; a->bodyCollider.targetMaterial=0;
        u32 flags=a->bodyCollider.attackFlags; field<u32>(a,0x39C)=4;
        u32 hit=field<u32>(a,0x71C); a->bounceSpeed=7; a->speedF=15;
        field<u32>(a,0x71C)=hit&~1u; a->bodyCollider.attackFlags=flags&~1u;
        a->spinX=(s16)gabi::ftoi(gabi::call<f32>(0x020198D8,500.0f)+512.0f);
        if(gabi::call<f32>(0x02019788)<0.5f) a->spinX=(s16)(-(s16)a->spinX);
        if(a->animationId!=23) anm_init(a,23,1.0f,0,1.0f,-1);
        a->timers[2]=0; fire_emitter_clr(a); stopFireSound(a);
        a->actionStep=(s16)((s16)a->actionStep+1);
        [[fallthrough]];
    }
    case 31:
        a->shape_angle.z=(s16)((s16)a->shape_angle.z+(s16)a->spinX);
        gabi::call(0x0200ED84,part<u8>(a,0x340),(f32)a->bounceSpeed,0.8f,2.0f);
        gabi::call(0x0200EDC8,&a->speedF,0.8f,3.0f);
        if((f32)a->speedF>0.1f) break;
        a->actionStep=(s16)((s16)a->actionStep+1);
        [[fallthrough]];
    case 32:
        a->burning=0; a->actionStep=(s16)((s16)a->actionStep+1); a->speedF=0;
        [[fallthrough]];
    case 33:
        gabi::call(0x0200ED84,&a->gravity,-3.0f,0.8f,0.1f);
        gabi::call(0x0200F428,&a->shape_angle.x,-0x7FFF,1,0x120);
        if(!((u32)field<u32>(a,0x4B0)&0x20)) break;
        a->actionStep=(s16)((s16)a->actionStep+1);
        [[fallthrough]];
    case 34:
        field<f32>(a,0x340)=15; a->gravity=-3; bound_sound_set(a);
        a->actionStep=(s16)((s16)a->actionStep+1);
        [[fallthrough]];
    case 35:
        gabi::call(0x0200F428,&a->shape_angle.x,0,1,0x400); gabi::call(0x0200F428,&a->shape_angle.z,0,1,0x400);
        if(std::abs((s16)a->shape_angle.x)>=0x100 || std::abs((s16)a->shape_angle.z)>=0x100) break;
        a->shape_angle.x=0; a->shape_angle.z=0;
        if((u8)a->variant&0x80) { a->action=10; a->actionStep=100; }
        else a->actionStep=38;
        gabi::call(0x025A5AC8,reinterpret_cast<u8*>(&a->appearanceCallback)); followed=true; a->targetScale=1;
        break;
    case 36: {
        for(u32 i=0;i<4;++i) field<s16>(a,0x414+i*2)=0;
        field<f32>(a,0x340)=15; a->gravity=-3; bound_sound_set(a);
        if(!((u8)a->variant&0x80)) field<u32>(a,0x39C)=4;
        u32 flags=a->bodyCollider.attackFlags; a->speedF=30;
        gabi::Local<cXyz> forward,direction; forward->x=0; forward->z=5000; forward->y=0;
        a->bodyCollider.attackFlags=flags&~1u; u32 stack=matrixStack();
        s32 heading=gabi::call<s32>(0x025D6894,a,gabi::at<u8>(opponent()));
        gabi::call(0x025F1884,gabi::at<u8>(stack),(s16)(heading+0x8000)); gabi::call(0x0200FCD8,forward.get(),direction.get());
        f32 x=-(f32)direction->x; field<u8>(a,0x660)=0x50; f32 z=direction->z;
        a->spinX=(s16)gabi::ftoi(x); a->spinZ=(s16)gabi::ftoi(z);
        a->actionStep=(s16)((s16)a->actionStep+1);
        [[fallthrough]];
    }
    case 37:
        a->shape_angle.x=(s16)((s16)a->shape_angle.x+(s16)a->spinX);
        a->shape_angle.z=(s16)((s16)a->shape_angle.z+(s16)a->spinZ);
        if((f32)a->speedF<5.0f) {
            if(roll_check(a)) {
                if((u8)a->variant&0x80) { a->actionStep=100; a->action=10; }
                else a->actionStep=38;
            } else if(a->bounceOrSmokeFade==0) break;
        } else if((u32)field<u32>(a,0x4B0)&0x20) { field<f32>(a,0x340)=10; a->bounceOrSmokeFade=1; bound_sound_set(a); }
        if(a->bounceOrSmokeFade==0) break;
        gabi::call(0x0200EDC8,&a->speedF,0.8f,1.0f); gabi::call(0x025A5AC8,reinterpret_cast<u8*>(&a->appearanceCallback));
        followed=true; a->targetScale=1; break;
    case 38:
        a->timers[1]=(s16)gabi::ftoi(gabi::call<f32>(0x020198D8,35.0f)+70.0f); a->speedF=0;
        a->actionStep=(s16)((s16)a->actionStep+1);
        [[fallthrough]];
    case 39:
        if(a->timers[1]!=0) break;
        anm_init(a,21,1.0f,0,1.0f,-1); a->speedF=0; a->current.angle.y=a->shape_angle.y;
        a->action=0; a->actionStep=1; break;
    }
    if(!followed) { gabi::call(0x025A5AC8,reinterpret_cast<u8*>(&a->appearanceCallback)); a->targetScale=1; }
    if((u8)a->variant&0x80) skull_atari_check(a); else collisionBody(a);
}
void recordActorDeath(bl_class* a) {
    s8 room=field<s8>(a,0x2FE); u32 save=gabi::load<u32>(0x101F84DC)+0x20; u16 id=field<u16>(a,0x2D8);
    gabi::call(0x025BA5D4,gabi::at<u8>(save),id,room); gabi::call(0x025D57E0,a);
}
void actionHurt(bl_class* a) {
    u32 play=gabi::call<u32>(0x025200D4),player=gabi::load<u32>(play+0x5B2C);
    switch((s16)a->actionStep) {
    case 40: {
        a->targetScale=1; for(u32 i=0;i<4;++i) field<s16>(a,0x414+i*2)=0;
        s32 animation=a->animationId; a->gravity=-3;
        field<u32>(a,0x39C)=4; a->bodyCollider.attackFlags=(u32)a->bodyCollider.attackFlags&~1u;
        if(animation!=23) anm_init(a,23,1.0f,0,1.0f,-1);
        a->timers[2]=0; fire_emitter_clr(a);
        gabi::Local<cXyz> forward,direction; forward->x=0; forward->y=0;
        if(field<s8>(a,0x3A1)<=0) {
            if(a->damageKind==4) {
                a->timers[0]=0; field<u32>(a,0x39C)=0; a->actionStep=42;
                // HD deliberately leaves forward.z as the original stack contents.
            } else {
                a->timers[0]=50; u8 damage=a->damageKind;
                field<f32>(a,0x340)=30; a->speedF=20; a->bounceSpeed=30; forward->z=5000;
                if(damage==5) a->current.angle.y=(s16)(gabi::load<s16>(player+0x32A)-0x4000);
                bound_sound_set(a); field<u32>(a,0x39C)=0; a->actionStep=42;
            }
        } else {
            a->actionStep=41; field<f32>(a,0x340)=15; a->speedF=10; forward->z=5000; bound_sound_set(a);
        }
        u32 stack=matrixStack(); s32 angle=gabi::call<s32>(0x025D6894,a,gabi::at<u8>(opponent()));
        gabi::call(0x025F1884,gabi::at<u8>(stack),(s16)(angle+0x8000)); gabi::call(0x0200FCD8,forward.get(),direction.get());
        a->spinX=(s16)gabi::ftoi(-(f32)direction->x); a->spinZ=(s16)gabi::ftoi((f32)direction->z);
        break;
    }
    case 41:
        if((f32)a->speedF<0.1f && roll_check(a)) { a->action=3; a->actionStep=38; }
        gabi::call(0x0200EDC8,&a->speedF,0.8f,1.0f); collisionBody(a); break;
    case 42:
        if(!((u32)field<u32>(a,0x4B0)&0x20)) break;
        {
            f32 bounce=(f32)a->bounceSpeed*0.5f; a->bounceSpeed=bounce;
            if(bounce<10.0f) { bounce=10; a->bounceSpeed=10; }
            field<f32>(a,0x340)=bounce; bound_sound_set(a);
        }
        if(a->timers[0]!=0) break;
        {
            gabi::Local<cXyz> vanish; vanish->x=a->current.pos.x; vanish->y=a->current.pos.y; vanish->z=a->current.pos.z;
            vanish->y=(f32)vanish->y+30; gabi::call(0x025D99E8,a,vanish.get(),5,0,0xFF);
        }
        if(a->damageKind!=4) { recordActorDeath(a); break; }
        {
            f32 height=gabi::load<f32>(0x1047BA98)+25;
            gabi::Local<cXyz> smoke; smoke->x=a->current.pos.x; smoke->y=(f32)a->current.pos.y+height; smoke->z=a->current.pos.z;
            s8 room=a->current.roomNo; play=gabi::call<u32>(0x025200D4); u32 particles=gabi::load<u32>(play+0x5AB0);
            u32 emitter=gabi::call<u32>(0x025A847C,gabi::at<u8>(particles),0,0x3E8,smoke.get(),&a->current.angle,&a->scale,0xFF,0,room,part<u8>(a,0x1A8),part<u8>(a,0x1A8),0);
            if(emitter) { gabi::store<f32>(emitter+0x6C,30); gabi::store<f32>(emitter+0x70,20); }
            field<u32>(a,0x7D8)=field<u32>(smoke.get(),0); field<u32>(a,0x7DC)=field<u32>(smoke.get(),4); field<u32>(a,0x7E0)=field<u32>(smoke.get(),8);
            smoke_set(a); a->bodyCollider.targetFlags=(u32)a->bodyCollider.targetFlags&~1u; a->bodyCollider.correctionFlags=(u32)a->bodyCollider.correctionFlags&~1u;
            gabi::call(0x0251621C,reinterpret_cast<u8*>(&a->bodyCollider));
            a->scale.x=0; a->scale.y=0; a->scale.z=0; a->bounceOrSmokeFade=180; a->targetScale=0; a->timers[1]=10;
            a->actionStep=(s16)((s16)a->actionStep+1);
        }
        break;
    case 43: {
        u32 emitter=field<u32>(a,0x7E8); if(!emitter) { recordActorDeath(a); break; }
        if(a->timers[1]!=0) break;
        gabi::store<u8>(emitter+0x247,(u8)field<u8>(a,0x419)); s16 fade=(s16)((s16)a->bounceOrSmokeFade-4); a->bounceOrSmokeFade=fade;
        if(fade<0) gabi::call(0x025A5F88,reinterpret_cast<u8*>(&a->smokeCallback));
        break;
    }
    }
    a->shape_angle.x=(s16)((s16)a->shape_angle.x+(s16)a->spinX);
    a->shape_angle.z=(s16)((s16)a->shape_angle.z+(s16)a->spinZ);
}
void actionHook(bl_class* a) {
    gabi::call(0x025200D4);
    if(a->actionStep==60) {
        for(u32 i=0;i<4;++i) field<s16>(a,0x414+i*2)=0;
        if(field<u32>(a,0x81C)) {
            a->timers[2]=0; fire_emitter_clr(a); anm_init(a,23,1.0f,0,1.0f,-1);
            a->speedF=0; a->bodyCollider.attackFlags=(u32)a->bodyCollider.attackFlags&~1u; stopFireSound(a);
        }
        u32 flags=field<u32>(a,0x71C),status=field<u32>(a,0x2E0);
        a->bodyCollider.targetState=0; a->bodyCollider.targetMaterial=0; field<u32>(a,0x71C)=flags&~1u;
        field<u32>(a,0x2E0)=status|0x20; field<u32>(a,0x39C)=4;
        a->actionStep=(s16)((s16)a->actionStep+1);
    } else if(a->actionStep==61 && !((u32)field<u32>(a,0x2E0)&0x100000)) {
        a->action=3; a->actionStep=32;
    }
    gabi::call(0x025A5AC8,reinterpret_cast<u8*>(&a->appearanceCallback));
}
void actionWait(bl_class* a,bool& forcePlay) {
    gabi::call(0x025200D4); if(a->actionStep!=70) return;
    u8 switchNo=a->switchNo; if(switchNo==0xFF) { forcePlay=true; return; }
    u32 save=gabi::load<u32>(0x101F84DC)+0x20; s8 room=gabi::load<s8>(0x1047E6C8);
    if(!gabi::call<BOOL>(0x025BA0C0,gabi::at<u8>(save),switchNo,room)) return;
    a->action=3; field<u32>(a,0x2E0)=(u32)field<u32>(a,0x2E0)|0x20; a->actionStep=38; forcePlay=true;
}
void actionSkull(bl_class* a) {
    gabi::call(0x025200D4); gabi::Local<cXyz> position;
    position->x=a->current.pos.x; position->y=a->current.pos.y; position->z=a->current.pos.z;
    switch((s16)a->actionStep) {
    case 100:
        {
            f32 radius=gabi::load<f32>(0x1047BAAC)+30;
            u32 status=field<u32>(a,0x2E0); a->gravity=-3; a->collisionRadius=radius;
            if(!(status&0x2000)) break;
            a->bodyCollider.correctionFlags=(u32)a->bodyCollider.correctionFlags&~1u; a->gravity=0;
            a->actionStep=(s16)((s16)a->actionStep+1);
        }
        [[fallthrough]];
    case 101:
        if((u32)field<u32>(a,0x2E0)&0x2000) break;
        a->bodyCollider.correctionFlags=(u32)a->bodyCollider.correctionFlags|1; a->gravity=-3;
        if((f32)a->speedF>0.0f) { a->actionStep=102; field<f32>(a,0x340)=25; a->speedF=35; }
        else { a->speedF=0; field<f32>(a,0x33C)=0; field<f32>(a,0x340)=0; field<f32>(a,0x344)=0; a->gravity=-3; a->actionStep=100; }
        break;
    case 102:
        if(!((u32)field<u32>(a,0x4B0)&0x30)) break;
        {
            s16 bounce=field<s16>(a,0xBE8); field<f32>(a,0x33C)=0; field<f32>(a,0x340)=0; field<f32>(a,0x344)=0;
            if(bounce!=0) break;
            a->speedF=0; a->actionStep=104; field<u32>(a,0x39C)=0; field<s8>(a,0x3A1)=0;
        }
        break;
    case 103:
        bound_sound_set(a); a->actionStep=102; break;
    case 104: {
        f32 height=gabi::load<f32>(0x1047BA98)+25; s8 room=a->current.roomNo;
        position->y=(f32)position->y+height;
        u32 play=gabi::call<u32>(0x025200D4),particles=gabi::load<u32>(play+0x5AB0);
        u32 emitter=gabi::call<u32>(0x025A847C,gabi::at<u8>(particles),0,0x3E8,position.get(),&a->current.angle,&a->scale,0xFF,0,room,part<u8>(a,0x1A8),part<u8>(a,0x1A8),0);
        if(emitter) { gabi::store<f32>(emitter+0x6C,30); gabi::store<f32>(emitter+0x70,20); }
        field<u32>(a,0x7D8)=field<u32>(position.get(),0); field<u32>(a,0x7DC)=field<u32>(position.get(),4); field<u32>(a,0x7E0)=field<u32>(position.get(),8); smoke_set(a);
        s32 items=0;
        while(items<gabi::ftoi(gabi::call<f32>(0x020198D8,1.99f))) {
            gabi::call(0x025D8870,&a->current.pos,0,-1,-1,0,0,4,0); ++items;
        }
        a->bodyCollider.targetFlags=(u32)a->bodyCollider.targetFlags&~1u; a->bodyCollider.correctionFlags=(u32)a->bodyCollider.correctionFlags&~1u;
        gabi::call(0x0251621C,reinterpret_cast<u8*>(&a->bodyCollider));
        a->scale.x=0; a->scale.y=0; a->scale.z=0; a->targetScale=0; a->skullSmokeFade=180; a->timers[1]=10;
        a->actionStep=(s16)((s16)a->actionStep+1); break;
    }
    case 105: {
        u32 emitter=field<u32>(a,0x7E8); if(!emitter) { gabi::call(0x025D57E0,a); break; }
        if(a->timers[1]!=0) break;
        gabi::store<u8>(emitter+0x247,(u8)field<u8>(a,0x41B)); s16 fade=(s16)((s16)a->skullSmokeFade-4); a->skullSmokeFade=fade;
        if(fade<0) gabi::call(0x025A5F88,reinterpret_cast<u8*>(&a->smokeCallback)); break;
    }
    }
    a->modelYOffset=24; skull_atari_check(a);
}
BOOL daBL_Execute(bl_class* a) {
    WWHD_FUNC(0x020AB020,BOOL,a);
    if(a->timers[2]==1) gabi::call(0x025A5AC8,reinterpret_cast<u8*>(&a->fireCallback));
    s16 immunity=a->timers[5]; u32 x=field<u32>(a,0x314);
    if(immunity==1) field<u32>(a,0x2E0)=(u32)field<u32>(a,0x2E0)&~0x4000u;
    field<u32>(a,0x37C)=x; u32 y=field<u32>(a,0x318),z=field<u32>(a,0x31C);
    field<u32>(a,0x380)=y; f32 eyeY=field<f32>(a,0x380); field<u32>(a,0x384)=z;
    field<u32>(a,0x390)=x; field<u32>(a,0x394)=y; f32 attentionY=field<f32>(a,0x394); field<u32>(a,0x398)=z;
    field<f32>(a,0x380)=eyeY+40; field<f32>(a,0x394)=attentionY+80;
    if(!((u8)a->variant&0x80)) gabi::call(0x025E742C,a->burning?(u8*)a->textureAnimation:(u8*)a->colorAnimation);
    if(gabi::call<BOOL>(0x020402C8,reinterpret_cast<u8*>(&a->iceState))) {
        mtx_copy(gabi::at<Mtx34>(model(a)+0xC8),gabi::at<Mtx34>(0x1048D0CC)); gabi::call(0x025E55A0,(u8*)a->morph);
        s16 timer=a->timers[2]; if(timer) a->timers[2]=(s16)(timer-1); return TRUE;
    }
    for(u32 i=0;i<6;++i) { s16 timer=field<s16>(a,0x408+i*2); if(timer) field<s16>(a,0x408+i*2)=(s16)(timer-1); }
    bool forcePlay=false;
    switch((u8)a->action) {
    case 0: actionRoam(a,forcePlay); break;
    case 1: actionAttack(a,forcePlay); break;
    case 2: actionRetreat(a); break;
    case 3: actionWind(a); break;
    case 4: actionHurt(a); break;
    case 5: actionHook(a); break;
    case 6: actionWait(a,forcePlay); break;
    case 10: actionSkull(a); break;
    }
    if(forcePlay || a->actionStep!=0) {
        u32 material=0;
        if((u32)field<u32>(a,0x4B0)&0x20) {
            u32 play=gabi::call<u32>(0x025200D4); material=gabi::call<u32>(0x024EECAC,gabi::at<u8>(play+0x12A0),part<u8>(a,0x570));
        }
        s32 reverb=gabi::call<s32>(0x02520540,(s8)a->current.roomNo);
        gabi::call(0x025E535C,(u8*)a->morph,part<u8>(a,0x37C),material,reverb);
    }
    gabi::call(0x025F1884,gabi::at<u8>(matrixStack()),(s16)a->current.angle.y);
    gabi::Local<cXyz> forward,direction; forward->x=0; forward->y=0; forward->z=a->speedF;
    gabi::call(0x0200FCD8,forward.get(),direction.get());
    f32 velocity=field<f32>(a,0x340),dx=direction->x,gravity=a->gravity;
    field<f32>(a,0x33C)=dx; velocity=velocity+gravity; field<f32>(a,0x344)=direction->z;
    if(velocity<-55.0f) velocity=-55.0f;
    field<f32>(a,0x340)=velocity;
    gabi::call(0x0200ED84,&a->scale.x,(f32)a->targetScale,0.3f,0.3f);
    f32 scale=a->scale.x; a->scale.z=scale; a->scale.y=scale;
    gabi::Local<cXyz> center; center->x=a->current.pos.x; center->y=a->current.pos.y; center->z=a->current.pos.z; center->y=(f32)center->y+20;
    gabi::call(0x02018D40,part<u8>(a,0x7A0),center.get()); gabi::call(0x02018C8C,part<u8>(a,0x7A0),(f32)a->collisionRadius);
    u32 play=gabi::call<u32>(0x025200D4); gabi::call(0x0200E240,gabi::at<u8>(play+0x26A4),reinterpret_cast<u8*>(&a->bodyCollider));
    gabi::call(0x025D6800,a,((u32)a->bodyCollider.correctionFlags&1)?reinterpret_cast<u8*>(&a->bodyCollisionStatus):nullptr); BG_check(a);
    if(!(a->flying && (f32)a->gravity==0.0f) && (f32)field<f32>(a,0x51C)!=-1000000000.0f) {
        play=gabi::call<u32>(0x025200D4);
        if(gabi::call<BOOL>(0x02008254,gabi::at<u8>(play+0x12A0),part<u8>(a,0x570))) {
            play=gabi::call<u32>(0x025200D4);
            if(gabi::call<s32>(0x024EF0BC,gabi::at<u8>(play+0x12A0),part<u8>(a,0x570))==4 && (f32)a->current.pos.y<(f32)a->homeGroundHeight-500.0f) {
                a->speedF=0; field<f32>(a,0x33C)=0; field<f32>(a,0x340)=0; field<f32>(a,0x344)=0; a->gravity=0; recordActorDeath(a);
            }
        }
    }
    draw_SUB(a); return TRUE;
}
VERIFY(0x020AB020,daBL_Execute);

BOOL daBL_IsDelete(bl_class* actor) {
    WWHD_FUNC(0x020AD594,BOOL,actor); return TRUE;
}
VERIFY(0x020AD594,daBL_IsDelete);
BOOL useHeapInit(bl_class* actor) {
    WWHD_FUNC(0x020AD61C,BOOL,actor);
    gabi::Local<SafeString> modelName,animationName;
    modelName->__vtbl=0x100098AC; modelName->mStringTop=0x10009A57;
    u32 resources=gabi::load<u32>(0x101F4F28);
    u8* modelResource=gabi::call<u8*>(0x026066C4,gabi::at<u8>(resources),modelName.get(),0x1B);
    animationName->mStringTop=0x10009A57; animationName->__vtbl=0x100098AC;
    resources=gabi::load<u32>(0x101F4F28);
    u8* animationResource=gabi::call<u8*>(0x026066C4,gabi::at<u8>(resources),animationName.get(),0x15);
    u8* morph=gabi::call<u8*>(0x025E4F64,0,modelResource,0,0,animationResource,1,0,-1,1,0,0x80000,0x37441422,0.0f);
    actor->morph=morph;
    if(!morph || !gabi::load<u32>(gabi::ea(morph)+0x90)) return FALSE;
    u32 m=gabi::load<u32>(gabi::ea(morph)+0x90);
    for(u32 offset : {0x3DCu,0x3D8u}) {
        u8* animation=gabi::call<u8*>(0x0273AD10,0x74);
        if(animation) animation=gabi::call<u8*>(0x025E7C6C,animation);
        field<u32>(actor,offset)=gabi::ea(animation);
        if(!animation) return FALSE;
        gabi::Local<SafeString> name;
        name->__vtbl=0x100098AC; name->mStringTop=0x10009A57;
        resources=gabi::load<u32>(0x101F4F28);
        u8* resource=gabi::call<u8*>(0x026066C4,gabi::at<u8>(resources),name.get(),offset==0x3DC?0x1F:0x1E);
        animation=gabi::at<u8>(field<u32>(actor,offset));
        u32 data=gabi::load<u32>(m+0xAC);
        if(!gabi::call<s32>(0x025E7CE0,animation,gabi::at<u8>(data),resource,1,0,0,-1,0,0,1.0f)) return FALSE;
        if(!field<u32>(actor,offset)) return FALSE;
    }
    m=model(actor); gabi::store<u32>(m+0xB8,gabi::ea(actor));
    m=model(actor);
    return gabi::call<s32>(0x025E8A48,reinterpret_cast<u8*>(&actor->lightInfo),gabi::at<u8>(m))!=0;
}
VERIFY(0x020AD61C,useHeapInit);
BOOL daBL_Delete(bl_class* actor) {
    WWHD_FUNC(0x020AD59C,BOOL,actor);
    for(u32 offset : {0x804u,0x818u,0x7E4u}) {
        u32 table=field<u32>(actor,offset); u32 target=gabi::load<u32>(table+0x44);
        gabi::call(target,part<u8>(actor,offset));
    }
    gabi::call(0x02041C30,reinterpret_cast<u8*>(&actor->fireHelper));
    gabi::call(0x025204C8,&actor->phase,STR(0x10009A54));
    return TRUE;
}
VERIFY(0x020AD59C,daBL_Delete);

u8* constructFire(u8* fire) {
    WWHD_FUNC(0x020AD850,u8*,fire);
    if(!fire) {
        fire=gabi::call<u8*>(0x0273AD10,0x22C);
        if(!fire) return nullptr;
    }
    if(gabi::ea(fire)+0x8C==0) gabi::call(0x0273AD10,12);
    gabi::call(0x0200BD2C,part<u8>(fire,0xA0));
    gabi::call(0x02515DA0,part<u8>(fire,0xBC));
    field<u32>(fire,0xB8)=0x1004AE88;
    field<u32>(fire,0xBC)=0x1004AEC0;
    gabi::call(0x025166F0,part<u8>(fire,0xDC));
    field<f32>(fire,0x228)=1.0f;
    return fire;
}
VERIFY(0x020AD850,constructFire);

bl_class* constructBubble(bl_class* actor) {
    WWHD_FUNC(0x020AD8DC,bl_class*,actor);
    if(!actor) {
        actor=gabi::call<bl_class*>(0x0273AD10,0xE18);
        if(!actor) return nullptr;
    }
    gabi::call(0x025D4ED0,actor);
    field<u32>(actor,0xB4)=0x10009964;
    gabi::call(0x024EFE94,reinterpret_cast<u8*>(&actor->groundCircle));
    gabi::call(0x024F0474,reinterpret_cast<u8*>(&actor->groundCollision));
    field<u32>(actor,0x498)=0x100098F4;
    field<u8>(actor,0x4A0)=1;
    field<u32>(actor,0x4A8)=0x10009904;
    field<u32>(actor,0x49C)=0x10009914;
    gabi::call(0x0200BD2C,reinterpret_cast<u8*>(&actor->bodyCollisionStatus));
    gabi::call(0x02515DA0,part<u8>(actor,0x668));
    field<u32>(actor,0x664)=0x1004AE88;
    field<u32>(actor,0x668)=0x1004AEC0;
    gabi::call(0x025166F0,reinterpret_cast<u8*>(&actor->bodyCollider));
    gabi::call(0x025A5B18,reinterpret_cast<u8*>(&actor->smokeCallback),1);
    gabi::call(0x025A5894,reinterpret_cast<u8*>(&actor->appearanceCallback),0,0);
    gabi::call(0x025A5894,reinterpret_cast<u8*>(&actor->fireCallback),0,0);
    gabi::call(0x0200BD2C,reinterpret_cast<u8*>(&actor->fireCollisionStatus));
    gabi::call(0x02515DA0,part<u8>(actor,0x878));
    field<u32>(actor,0x874)=0x1004AE88;
    field<u32>(actor,0x878)=0x1004AEC0;
    gabi::call(0x02515FB8,reinterpret_cast<u8*>(&actor->fireCylinder));
    field<u32>(actor,0x9AC)=0x100015A8;
    field<u32>(actor,0x9A8)=0x100098C4;
    gabi::call(0x02018590,part<u8>(actor,0x9B0));
    field<u32>(actor,0x8D4)=0x1004B108;
    field<u32>(actor,0x9C4)=0x1004B150;
    field<u32>(actor,0x9AC)=0x1004B160;
    gabi::call(0x024EFE94,reinterpret_cast<u8*>(&actor->fireGroundCircle));
    gabi::call(0x024F0474,reinterpret_cast<u8*>(&actor->fireGroundCollision));
    field<u32>(actor,0xA30)=0x100098F4;
    field<u32>(actor,0xA34)=0x10009914;
    field<u32>(actor,0xA40)=0x10009904;
    field<u8>(actor,0xA38)=1;
    constructFire(reinterpret_cast<u8*>(&actor->fireHelper));
    gabi::call(0x025E895C,reinterpret_cast<u8*>(&actor->lightInfo));
    return actor;
}
VERIFY(0x020AD8DC,constructBubble);

s32 daBL_Create(bl_class* actor) {
    WWHD_FUNC(0x020ADA5C,s32,actor);
    u32 flags=field<u32>(actor,0x2E4);
    if(!(flags&8)) {
        if(actor) { constructBubble(actor); flags=field<u32>(actor,0x2E4); }
        field<u32>(actor,0x2E4)=flags|8;
    }
    s32 phase=gabi::call<s32>(0x02520460,&actor->phase,STR(0x10009A78));
    if(phase!=4) return phase;
    u32 params=field<u32>(actor,0xB0);
    s16 pitch=actor->current.angle.z;
    actor->variant=params; actor->flying=params>>24;
    actor->pathNo=params>>16; actor->switchNo=params>>8;
    u8 type=actor->variant;
    actor->current.angle.z=0; actor->shape_angle.z=0;
    actor->pathSpeed=(f32)pitch;
    if(type==0xFF) actor->variant=0;
    if(params>>24==0xFF) actor->flying=0;
    if(gabi::load<s16>(0x1047BB18)) actor->flying=1;
    s16 debugType=gabi::load<s16>(0x1047BB1A);
    if(debugType) actor->variant=debugType-1;
    if(!gabi::call<s32>(0x025D63E8,actor,0x020AD61C,0x14E0)) return 5;
    type=actor->variant;
    if(type&2) {
        u8 pathNo=actor->pathNo; actor->variant=type^2;
        if(pathNo!=0xFF) {
            u8* path=gabi::call<u8*>(0x025AAF88,pathNo,(s8)actor->current.roomNo);
            f32 length=actor->pathSpeed; actor->path=gabi::ea(path);
            if(length<4.0f) actor->pathSpeed=gabi::call<f32>(0x020198D8,2.0f)+4.0f;
        }
        if(gabi::load<s16>(0x1047BB1A)) {
            s8 room=actor->current.roomNo; actor->pathNo=0;
            actor->path=gabi::ea(gabi::call<u8*>(0x025AAF88,0,room));
        }
    }
    u32 m=model(actor); field<u32>(actor,0x348)=m?m+0xC8:0;
    field<u32>(actor,0x39C)=0;
    gabi::call(0x024F06B4,reinterpret_cast<u8*>(&actor->groundCollision),&actor->current.pos,part<cXyz>(actor,0x300),actor,1,reinterpret_cast<u8*>(&actor->groundCircle),part<cXyz>(actor,0x33C),0,0);
    gabi::call(0x02515F14,reinterpret_cast<u8*>(&actor->bodyCollisionStatus),0x50,1,actor);
    field<u32>(actor,0x82C)=gabi::ea(actor);
    actor->max_health=2;
    u32 morph=gabi::ea((u8*)actor->morph); actor->health=2; field<u32>(actor,0xBF0)=morph;
    field<f32>(actor,0x9CC)=50.0f; field<u32>(actor,0xBE4)=gabi::ea(actor); field<f32>(actor,0x9C8)=50.0f;
    for(u32 i=0;i<10;++i) {
        field<u8>(actor,0xBF4+i)=gabi::load<u8>(0x1019194C+i);
        field<f32>(actor,0xC00+4*i)=gabi::load<f32>(0x10191924+4*i);
    }
    s16 yaw=actor->shape_angle.y; actor->collisionRadius=50.0f;
    actor->targetYaw=yaw;
    gabi::call(0x0251677C,reinterpret_cast<u8*>(&actor->bodyCollider),gabi::at<u8>(0x101918E4));
    u32 sphereFlags=actor->bodyCollider.attackFlags; field<u8>(actor,0x38A)=0x29;
    field<u32>(actor,0x6CC)=gabi::ea(actor)+0x64C; actor->bodyCollider.attackFlags=sphereFlags&~1u;
    u32 play=gabi::call<u32>(0x025200D4);
    s32 item=gabi::call<s32>(0x0200E814,gabi::at<u8>(play+0x50AC),STR(0x10009A68),0);
    type=actor->variant; actor->itemTableIdx=item; field<u8>(actor,0x2DE)=14;
    if(type==1) {
        field<u8>(actor,0x6F7)=3; field<u32>(actor,0x698)=0x800;
        play=gabi::call<u32>(0x025200D4);
        item=gabi::call<s32>(0x0200E814,gabi::at<u8>(play+0x50AC),STR(0x10009A70),0);
        actor->itemTableIdx=item; field<u8>(actor,0x2DE)=15;
    }
    f32 velocity=(f32)field<f32>(actor,0x340)-3.0f;
    field<f32>(actor,0x374)=-3.0f; if(velocity<-55.0f) velocity=-55.0f;
    u32 z=field<u32>(actor,0x31C),x=field<u32>(actor,0x314);
    field<f32>(actor,0x340)=velocity; field<u32>(actor,0x3E0)=x;
    u32 y=field<u32>(actor,0x318); actor->targetScale=1.0f;
    field<u32>(actor,0x3E4)=y; field<u32>(actor,0x3E8)=z; actor->modelYOffset=24.0f;
    gabi::call(0x025D6800,actor,reinterpret_cast<u8*>(&actor->bodyCollisionStatus)); BG_check(actor);
    u8 flying=actor->flying; f32 height=actor->current.pos.y;
    u32 collisionFlags=field<u32>(actor,0x4B0); actor->homeGroundHeight=height;
    if(!flying) actor->homeGroundHeight=(f32)field<f32>(actor,0x51C);
    actor->action=0;
    u8 switchNo=actor->switchNo;
    if(!(collisionFlags&0x20)) {
        height=actor->homeGroundHeight;
        actor->scale.x=0.0f; actor->scale.y=0.0f; actor->scale.z=0.0f;
        field<f32>(actor,0x374)=0.0f; field<f32>(actor,0x340)=0.0f;
        actor->actionStep=4; actor->current.pos.y=height+100.0f;
    } else actor->actionStep=0;
    if(switchNo!=0xFF) {
        u32 save=gabi::load<u32>(0x101F84DC)+0x20;
        s8 room=gabi::load<s8>(0x1047E6C8);
        if(!gabi::call<s32>(0x025BA0C0,gabi::at<u8>(save),switchNo,room)) {
            u32 status=field<u32>(actor,0x2E0); actor->action=6;
            actor->actionStep=70; field<u32>(actor,0x2E0)=status&~0x20u;
        }
    }
    if(actor->variant&0x80) {
        u32 sphere=actor->bodyCollider.correctionFlags;
        actor->max_health=1; actor->burning=1;
        actor->bodyCollider.correctionFlags=sphere|1; actor->health=1;
        s16 heading=(s16)gabi::ftoi(gabi::call<f32>(0x02019918,32767.0f));
        u32 status=field<u32>(actor,0x2E0);
        actor->action=10; actor->current.angle.y=heading; actor->shape_angle.y=heading;
        u32 attention=field<u32>(actor,0x39C); actor->timers[5]=4;
        actor->actionStep=100; field<u32>(actor,0x2E0)=(status|0x4000)&~0x20u;
        field<u32>(actor,0x39C)=attention|0x10;
    } else { field<u8>(actor,0x406)=1; actor->stealItemLeft=1; }
    draw_SUB(actor);
    return phase;
}
VERIFY(0x020ADA5C,daBL_Create);

void initializeStatics() {
    WWHD_FUNC(0x020ADF84,void);
    gabi::store<u32>(0x1046248C,0); gabi::store<u32>(0x10462484,0);
    gabi::store<u32>(0x10462490,0); gabi::store<u32>(0x10462488,0);
    gabi::call(0x028F026C,gabi::at<u8>(0x10191958));
    gabi::store<f32>(0x10462478,-3.1415927410125732f);
    gabi::store<f32>(0x1046247C,3.1415927410125732f);
    gabi::call(0x028ED6F8,gabi::at<u8>(0x10462480));
    gabi::call(0x028F026C,gabi::at<u8>(0x10191964));
    gabi::call(0x028EAB2C,gabi::at<u8>(0x10462481));
    gabi::call(0x028F026C,gabi::at<u8>(0x10191970));
}
VERIFY(0x020ADF84,initializeStatics);
void deleteStatic(u8* object,u32 flags) {
    WWHD_FUNC(0x020AE018,void,object,flags);
    if(object && (flags&1)) gabi::call(0x0273AF40,object);
}
VERIFY(0x020AE018,deleteStatic);
void destructBubble(bl_class* actor,u32 flags) {
    WWHD_FUNC(0x020AE02C,void,actor,flags);
    if(!actor) return;
    gabi::call(0x025E89F8,reinterpret_cast<u8*>(&actor->lightInfo),2);
    gabi::call(0x02515AE8,part<u8>(actor,0xCC0),2);
    gabi::call(0x02515860,part<u8>(actor,0xC84),2);
    field<u32>(actor,0xA40)=0x10009904; field<u32>(actor,0xA34)=0x10009914;
    gabi::call(0x024EFD9C,reinterpret_cast<u8*>(&actor->fireGroundCollision),0);
    gabi::call(0x02018034,part<u8>(actor,0x9F4),2);
    gabi::call(0x02515A70,reinterpret_cast<u8*>(&actor->fireCylinder),2);
    gabi::call(0x02515860,reinterpret_cast<u8*>(&actor->fireCollisionStatus),2);
    gabi::call(0x02515AE8,reinterpret_cast<u8*>(&actor->bodyCollider),2);
    gabi::call(0x02515860,reinterpret_cast<u8*>(&actor->bodyCollisionStatus),2);
    field<u32>(actor,0x4A8)=0x10009904; field<u32>(actor,0x49C)=0x10009914;
    gabi::call(0x024EFD9C,reinterpret_cast<u8*>(&actor->groundCollision),0);
    gabi::call(0x02018034,part<u8>(actor,0x45C),2);
    gabi::call(0x025D50BC,actor,0);
    if(flags&1) gabi::call(0x0273AF40,actor);
}
VERIFY(0x020AE02C,destructBubble);
void emptyVirtual(bl_class* actor) {
    WWHD_FUNC(0x020AE134,void,actor);
}
VERIFY(0x020AE134,emptyVirtual);
}
