#include "d/actor/d_a_am2.h"

static void* am2ObjectResource(s32 index,u32 archive=0x10007058) {
    // Reserve the linkage prefix for actual nonleaf callees before the SafeString.
    gabi::Local<SafeString> name;
    gabi::Local<u64> linkage;
    name->mStringTop = archive;
    name->__vtbl = 0x10006F74;
    u32 control = gabi::load<u32>(0x101F4F28);
    return gabi::call<void*>(0x026066C4, gabi::at<void>(control), name.get(), index);
}

static BOOL nodeCallBack(void* node, s32 timing) {
    WWHD_FUNC(0x0204C2C4, BOOL, node, timing);
    if (timing == 0) {
        void* joint = gabi::call<void*>(0x027F7878, node);
        u32 model = gabi::load<u32>(0x104B462C);
        u32 actor = gabi::load<u32>(model + 0xB8);
        u16 number = gabi::load<u16>(gabi::ea(joint) + 4);
        if (actor) {
            u32 matrixBlock = gabi::load<u32>(model + 0x2C);
            u32 matrices = gabi::load<u32>(matrixBlock + 0x10);
            gabi::store<u16>(matrixBlock+4, gabi::load<u16>(matrixBlock+4) | 0x10);
            u32 offset = number * 0x30;
            u32 current = gabi::load<u32>(0x1018C7B0);
            gabi::call(0x028E90D4, gabi::at<void>(matrices+offset), gabi::at<void>(current));
            if (number >= 1 && number <= 3) {
                gabi::Local<cXyz> local;
                gabi::Local<u64> linkage;
                local->x = number == 1 ? 0.0f : number == 2 ? 30.0f : -10.0f;
                local->y = number == 1 ? 50.0f : 0.0f;
                local->z = number == 1 ? -40.0f : 0.0f;
                u32 destination = actor + (number == 1 ? 0x414 : number == 2 ? 0x3FC : 0x408);
                gabi::call(0x0200FCD8, local.get(), gabi::at<cXyz>(destination));
            }
            matrixBlock = gabi::load<u32>(model+0x2C);
            current = gabi::load<u32>(0x1018C7B0);
            u16 flags = gabi::load<u16>(matrixBlock+4);
            matrices = gabi::load<u32>(matrixBlock+0x10);
            gabi::store<u16>(matrixBlock+4, flags | 0x10);
            mtx_copy(gabi::at<Mtx34>(matrices+offset), gabi::at<Mtx34>(current));
            current = gabi::load<u32>(0x1018C7B0);
            gabi::call(0x028E90D4, gabi::at<void>(current), gabi::at<void>(0x104B4868));
        }
    }
    return TRUE;
}
VERIFY(0x0204C2C4, nodeCallBack);

static void draw_SUB(am2_class* actor) {
    WWHD_FUNC(0x0204C4E4, void, actor);
    u32 model = gabi::load<u32>(actor->mpMorf + 0x90);
    f32 y = actor->scale.y, x = actor->scale.x, z = actor->scale.z;
    gabi::store<f32>(model+0xBC,x); gabi::store<f32>(model+0xC4,z); gabi::store<f32>(model+0xC0,y);
    auto matrix = gabi::at<Mtx34>(0x1048D0CC);
    gabi::call(0x028E93CC,matrix,(f32)actor->current.pos.x,(f32)actor->current.pos.y,(f32)actor->current.pos.z);
    gabi::call(0x025F1C28,matrix,(s16)actor->shape_angle.y);
    gabi::call(0x025F1BF4,matrix,(s16)actor->shape_angle.x);
    gabi::call(0x025F1C5C,matrix,(s16)actor->shape_angle.z);
    mtx_copy(gabi::at<Mtx34>(model+0xC8),matrix);
    gabi::call(0x025E55A0,gabi::at<void>(actor->mpMorf));
    void* env=gabi::call<void*>(0x02555D0C);
    gabi::call(0x025626A4,env,0,&actor->current.pos,&actor->tevStr);
}
VERIFY(0x0204C4E4, draw_SUB);

static BOOL daAM2_Draw(am2_class* actor) {
    WWHD_FUNC(0x0204C5F8, BOOL, actor);
    u32 model = gabi::load<u32>(actor->mpMorf+0x90);
    void* env=gabi::call<void*>(0x02555D0C);
    gabi::call(0x02562F5C,env,gabi::at<void>(model),&actor->tevStr);
    gabi::call(0x025BED80,0xB6,actor,1.0f,1.0f,1.0f);
    u32 animation=actor->mpBrkAnm, data=gabi::load<u32>(model+0xAC);
    f32 frame=gabi::load<f32>(animation+4);
    gabi::call(0x025E83FC,gabi::at<void>(animation),gabi::at<void>(data),frame);
    animation=actor->mpBtkAnm; data=gabi::load<u32>(model+0xAC); frame=gabi::load<f32>(animation+4);
    gabi::call(0x025E7FC4,gabi::at<void>(animation),gabi::at<void>(data),frame);
    gabi::call(0x025E5590,gabi::at<void>(actor->mpMorf));
    data=gabi::load<u32>(model+0xAC); gabi::store<u32>(data+0x48,0);
    data=gabi::load<u32>(model+0xAC); gabi::store<u32>(data+0x44,0);
    return TRUE;
}
VERIFY(0x0204C5F8, daAM2_Draw);

static void anm_init(am2_class* actor,s32 bck,f32 morph,u8 loop,f32 speed,s32 sound) {
    WWHD_FUNC(0x0204C69C,void,actor,bck,morph,loop,speed,sound);
    actor->mCurrBckIdx=bck;
    void* animation=am2ObjectResource(bck);
    void* audio=sound>=0 ? am2ObjectResource(sound) : nullptr;
    gabi::call(0x025E4A98,gabi::at<void>(actor->mpMorf),animation,loop,morph,speed,0.0f,-1.0f,audio);
}
VERIFY(0x0204C69C,anm_init);

static void BG_check(am2_class* actor) {
    WWHD_FUNC(0x0204CED4,void,actor);
    f32 height=gabi::load<f32>(0x1047BAC0)+40.0f;
    gabi::call(0x024EFF44,actor->mAcchCir,height,(f32)actor->mAcchRadius);
    f32 correction=actor->mCorrectionOffsetY;
    actor->current.pos.y=(f32)actor->current.pos.y-correction;
    actor->old.pos.y=(f32)actor->old.pos.y-correction;
    u32 play=gabi::call<u32>(0x025200D4);
    gabi::call(0x024F08A8,actor->mAcch,gabi::at<void>(play+0x12A0));
    f32 y=actor->current.pos.y; correction=actor->mCorrectionOffsetY; f32 oldY=actor->old.pos.y;
    actor->current.pos.y=y+correction; actor->old.pos.y=oldY+correction;
}
VERIFY(0x0204CED4,BG_check);

static BOOL daAM2_IsDelete(am2_class* actor) {
    WWHD_FUNC(0x0204F9C8,BOOL,actor);
    return TRUE;
}
VERIFY(0x0204F9C8,daAM2_IsDelete);

static BOOL daAM2_Delete(am2_class* actor) {
    WWHD_FUNC(0x0204F9D0,BOOL,actor);
    gabi::call(0x025204C8,&actor->mPhase,STR(0x100070E0));
    u32 callback=gabi::load<u32>(gabi::ea(actor->mSmokeCb));
    u32 end=gabi::load<u32>(callback+0x44);
    gabi::call_ptr(end,actor->mSmokeCb);
    gabi::call(0x025A9270,actor->mRippleCb);
    return TRUE;
}
VERIFY(0x0204F9D0,daAM2_Delete);

static void am2Sound(am2_class* actor,u32 id) {
    if (actor && gabi::ea(&actor->eyePos)) {
        s32 reverb=gabi::call<s32>(0x02520540,(s8)actor->current.roomNo);
        gabi::call(0x025E1A40,id,&actor->eyePos,0x42,reverb);
    }
}
static BOOL body_atari_check(am2_class* actor) {
    WWHD_FUNC(0x0204CCCC,BOOL,actor);
    u32 play=gabi::call<u32>(0x025200D4);
    u32 player=gabi::load<u32>(play+0x5B2C);
    gabi::call(0x02515E50,gabi::at<void>(gabi::ea(actor)+0x69C));
    actor->mHitDirection=0;
    if (!gabi::call<BOOL>(0x025162A4,actor->mBodyCyl)) return FALSE;
    u32 hit=gabi::call<u32>(0x02516300,actor->mBodyCyl);
    if (!hit) return FALSE;
    u32 type=gabi::load<u32>(hit+0x10);
    if (type==2) { am2Sound(actor,0x2803); return TRUE; }
    if (type==0x40 || type==0x80) { am2Sound(actor,0x2833); return TRUE; }
    if (type==0x10000) {
        am2Sound(actor,0x2855);
        if (actor->mAction!=3 && actor->mAction!=2) {
            actor->mAction=3; actor->mMode=30; actor->mHitDirection=7;
            if (gabi::load<u8>(player+0x3AC)==0x11) actor->mHitDirection=8;
        }
    } else am2Sound(actor,0x2834);
    return TRUE;
}
VERIFY(0x0204CCCC,body_atari_check);

static void* am2_ctor(void* storage) {
    WWHD_FUNC(0x0204FCDC,void*,storage);
    u32 actor=gabi::ea(storage);
    if (!actor) {
        actor=gabi::call<u32>(0x0273AD10,0xF60);
        if (!actor) return nullptr;
    }
    auto member=[&](u32 offset){return gabi::at<void>(actor+offset);};
    gabi::call(0x025D4ED0,member(0));
    gabi::store<u32>(actor+0xB4,0x1000702C);
    gabi::call(0x024EFE94,member(0x47C));
    gabi::call(0x024F0474,member(0x4BC));
    gabi::store<u32>(actor+0x4CC,0x10006FBC);
    gabi::store<u32>(actor+0x4DC,0x10006FCC);
    gabi::store<u32>(actor+0x4D0,0x10006FDC);
    gabi::store<u8>(actor+0x4D4,1);
    gabi::call(0x0200BD2C,member(0x680));
    gabi::call(0x02515DA0,member(0x69C));
    gabi::store<u32>(actor+0x698,0x1004AE88); gabi::store<u32>(actor+0x69C,0x1004AEC0);
    gabi::call(0x02515FB8,member(0x6BC));
    gabi::store<u32>(actor+0x7D0,0x100015A8); gabi::store<u32>(actor+0x7CC,0x10006F8C);
    gabi::call(0x02018590,member(0x7D4));
    gabi::store<u32>(actor+0x6F8,0x1004B108); gabi::store<u32>(actor+0x7E8,0x1004B150); gabi::store<u32>(actor+0x7D0,0x1004B160);
    gabi::call(0x02515FB8,member(0x7EC));
    gabi::store<u32>(actor+0x900,0x100015A8); gabi::store<u32>(actor+0x8FC,0x10006F8C);
    gabi::call(0x02018590,member(0x904));
    gabi::store<u32>(actor+0x828,0x1004B108); gabi::store<u32>(actor+0x918,0x1004B150); gabi::store<u32>(actor+0x900,0x1004B160);
    gabi::call(0x025166F0,member(0x91C)); gabi::call(0x025166F0,member(0xA48));
    gabi::call(0x025A5B18,member(0xB74),1); gabi::call(0x025A9084,member(0xB94));
    gabi::call(0x0200BD2C,member(0xBD8)); gabi::call(0x02515DA0,member(0xBF4));
    gabi::store<u32>(actor+0xBF0,0x1004AE88); gabi::store<u32>(actor+0xBF4,0x1004AEC0);
    gabi::call(0x02515FB8,member(0xC14));
    gabi::store<u32>(actor+0xD24,0x10006F8C); gabi::store<u32>(actor+0xD28,0x100015A8);
    gabi::call(0x02018590,member(0xD2C));
    gabi::store<u32>(actor+0xD28,0x1004B160); gabi::store<u32>(actor+0xD40,0x1004B150); gabi::store<u32>(actor+0xC50,0x1004B108);
    gabi::call(0x024EFE94,member(0xD5C)); gabi::call(0x024F0474,member(0xD9C));
    gabi::store<u32>(actor+0xDBC,0x10006FCC); gabi::store<u32>(actor+0xDAC,0x10006FBC); gabi::store<u32>(actor+0xDB0,0x10006FDC);
    gabi::store<u8>(actor+0xDB4,1);
    return gabi::at<void>(actor);
}
VERIFY(0x0204FCDC,am2_ctor);

static void am2_dtor(void* storage,s32 flags) {
    WWHD_FUNC(0x0205067C,void,storage,flags);
    u32 actor=gabi::ea(storage);
    if (!actor) return;
    auto member=[&](u32 offset){return gabi::at<void>(actor+offset);};
    gabi::store<u32>(actor+0xDBC,0x10006FCC); gabi::store<u32>(actor+0xDB0,0x10006FDC);
    gabi::call(0x024EFD9C,member(0xD9C),0);
    gabi::call(0x02018034,member(0xD70),2);
    gabi::call(0x02515A70,member(0xC14),2);
    gabi::call(0x02515860,member(0xBD8),2);
    gabi::call(0x02515AE8,member(0xA48),2); gabi::call(0x02515AE8,member(0x91C),2);
    gabi::call(0x02515A70,member(0x7EC),2); gabi::call(0x02515A70,member(0x6BC),2);
    gabi::call(0x02515860,member(0x680),2);
    gabi::store<u32>(actor+0x4DC,0x10006FCC); gabi::store<u32>(actor+0x4D0,0x10006FDC);
    gabi::call(0x024EFD9C,member(0x4BC),0); gabi::call(0x02018034,member(0x490),2);
    gabi::call(0x025D50BC,member(0),0);
    if (flags&1) gabi::call(0x0273AF40,storage);
}
VERIFY(0x0205067C,am2_dtor);

static void am2_empty_virtual(void* storage) {
    WWHD_FUNC(0x02050784,void,storage);
}
VERIFY(0x02050784,am2_empty_virtual);

static void am2_safe_string_dtor(void* storage,s32 flags) {
    WWHD_FUNC(0x0205034C,void,storage,flags);
    if (storage && (flags&1)) gabi::call(0x0273AF40,storage);
}
VERIFY(0x0205034C,am2_safe_string_dtor);

static void am2_static_init() {
    WWHD_FUNC(0x020502B8,void);
    gabi::store<u32>(0x104613F0,0); gabi::store<u32>(0x104613E8,0);
    gabi::store<u32>(0x104613F4,0); gabi::store<u32>(0x104613EC,0);
    gabi::call(0x028F026C,gabi::at<void>(0x1018FB98));
    gabi::store<f32>(0x104613DC,gabi::load<f32>(0x10007110));
    gabi::store<f32>(0x104613E0,gabi::load<f32>(0x10007114));
    gabi::call(0x028ED6F8,gabi::at<void>(0x104613E4));
    gabi::call(0x028F026C,gabi::at<void>(0x1018FBA4));
    gabi::call(0x028EAB2C,gabi::at<void>(0x104613E5));
    gabi::call(0x028F026C,gabi::at<void>(0x1018FBB0));
}
VERIFY(0x020502B8,am2_static_init);

static BOOL Line_check(am2_class* actor,const cXyz* destination) {
    WWHD_FUNC(0x0204CF64,BOOL,actor,destination);
    gabi::Local<dBgS_LinChk> check;
    gabi::Local<cXyz> center;
    gabi::Local<u64> linkage;
    dBgS_LinChk_ct(check.get(),{0x10006FEC,0x10006FFC,0x1000701C,0x1000700C},false);
    f32 x=actor->current.pos.x,y=actor->current.pos.y,z=actor->current.pos.z;
    center->x=x; center->y=y; center->z=z;
    y += gabi::load<f32>(0x1047BD1C)+100.0f;
    actor->mLinChkCenter.x=x; actor->mLinChkCenter.z=z; actor->mLinChkCenter.y=y;
    actor->mLinChkDest.copy(*destination);
    center->y=y;
    gabi::call(0x024F1AFC,check.get(),center.get(),destination,actor);
    u32 play=gabi::call<u32>(0x025200D4);
    BOOL crossed=gabi::call<BOOL>(0x02008860,gabi::at<void>(play+0x12A0),check.get());
    u32 p=gabi::ea(check.get());
    gabi::store<u32>(p+0x58,0x1000701C); gabi::store<u32>(p+0x64,0x10006FAC); gabi::store<u32>(p+0x20,0x10006F9C);
    gabi::call(0x02008B4C,check.get(),0);
    return crossed^1;
}
VERIFY(0x0204CF64,Line_check);

static BOOL naraku_check(am2_class* actor) {
    WWHD_FUNC(0x0204D0C4,BOOL,actor);
    auto acch=gabi::at<dBgS_Acch>(gabi::ea(actor->mAcch));
    bool fell=false;
    if (acch->m_ground_h!=-1000000000.0f) {
        u32 play=gabi::call<u32>(0x025200D4);
        if (gabi::call<BOOL>(0x02008254,gabi::at<void>(play+0x12A0),gabi::at<void>(gabi::ea(actor)+0x5A4))) {
            play=gabi::call<u32>(0x025200D4);
            if (gabi::call<s32>(0x024EF0BC,gabi::at<void>(play+0x12A0),gabi::at<void>(gabi::ea(actor)+0x5A4))==4) {
                actor->mInAbyssTimer=(u8)(actor->mInAbyssTimer+1);
                play=gabi::call<u32>(0x025200D4);
                u32 camera=gabi::load<u32>(play+0x5AF8);
                s32 id=actor ? gabi::load<s32>(gabi::ea(actor)+4) : -1;
                gabi::call(0x025052BC,gabi::at<void>(camera+0x248),id);
                fell=actor->current.pos.y<-500.0f || actor->mInAbyssTimer>50;
            }
        }
    }
    if (!fell) {
        if (acch->m_flags & 0x1000) {
            gabi::Local<cXyz> position;
            gabi::Local<cXyz> scale;
            gabi::Local<u64> linkage;
            if (!actor->mbMadeWaterSplash) {
                position->x=actor->current.pos.x; position->y=actor->current.pos.y; position->z=actor->current.pos.z;
                if (actor->mCountDownTimers[4]==0) {
                    f32 water=gabi::load<f32>(gabi::ea(actor)+0x678);
                    position->y=water;
                    f32 y=actor->current.pos.y;
                    actor->mCountDownTimers[4]=30;
                    f32 amplitude=gabi::load<f32>(0x1047BA94)+0.1f;
                    f32 height=(water-(y+80.0f))*amplitude;
                    if (height<0.0f) height=0.3f;
                    else if (height>1.0f) height=1.0f;
                    gabi::call(0x025DAE64,position.get(),1.0f,height,0);
                    if (gabi::ea(&actor->eyePos)) {
                        s32 reverb=gabi::call<s32>(0x02520540,(s8)actor->current.roomNo);
                        gabi::call(0x025E1A40,0x6919,&actor->eyePos,0,reverb);
                    }
                    scale->x=1.0f; scale->y=1.0f; scale->z=1.0f;
                    actor->mbMadeWaterSplash=1;
                    gabi::call(0x025A9270,actor->mRippleCb);
                    u32 play=gabi::call<u32>(0x025200D4);
                    u32 particles=gabi::load<u32>(play+0x5AB0);
                    gabi::call(0x025A847C,gabi::at<void>(particles),5,0x33,&actor->current.pos,(void*)nullptr,scale.get(),0xFF,actor->mRippleCb,-1,(void*)nullptr,(void*)nullptr,(void*)nullptr);
                    gabi::store<f32>(gabi::ea(actor)+0xBA4,0.0f);
                }
            }
            f32 threshold=gabi::load<f32>(gabi::ea(actor)+0x678)-(gabi::load<f32>(0x1047BCD0)+80.0f);
            fell=actor->current.pos.y<threshold;
        } else if (actor->mbMadeWaterSplash) {
            actor->mbMadeWaterSplash=0; gabi::call(0x025A9270,actor->mRippleCb);
        }
    }
    if (fell) {
        actor->gravity=0.0f; actor->speed.y=0.0f; actor->speedF=0.0f; actor->speed.x=0.0f; actor->speed.z=0.0f;
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x0204D0C4,naraku_check);

static void action_modoru_move(am2_class* actor) {
    WWHD_FUNC(0x02050360,void,actor);
    gabi::call(0x025200D4);
    u8 mode=actor->mMode;
    if (mode<40 || mode>42) return;
    if (mode==40) {
        s32 id=actor ? gabi::load<s32>(gabi::ea(actor)+4) : -1;
        void* camera=gabi::call<void*>(0x024F8044);
        gabi::call(0x025052BC,camera,id);
        actor->mInAbyssTimer=0; actor->mbMadeWaterSplash=0;
        gabi::call(0x025A9270,actor->mRippleCb);
        anm_init(actor,15,10.0f,0,1.0f,-1);
        if (actor && gabi::ea(&actor->eyePos)) {
            s32 reverb=gabi::call<s32>(0x02520540,(s8)actor->current.roomNo);
            gabi::call(0x025E1A40,0x5946,&actor->eyePos,0,reverb);
        }
        actor->mMode=(u8)(actor->mMode+1);
    }
    if (mode!=42) {
        gabi::call(0x0200EDC8,&actor->scale.x,1.0f,0.1f);
        f32 scale=actor->scale.x;
        actor->scale.y=scale; actor->scale.z=scale;
        if (scale<0.1f) {
            actor->speed.y=0.0f; actor->speed.x=0.0f; actor->speed.z=0.0f; actor->speedF=0.0f;
            actor->mbMadeWaterSplash=0; actor->gravity=-3.0f;
            gabi::call(0x025A9270,actor->mRippleCb);
            u32 y=gabi::load<u32>(gabi::ea(&actor->mSpawnPos.y));
            u32 x=gabi::load<u32>(gabi::ea(&actor->mSpawnPos.x));
            gabi::store<u32>(gabi::ea(&actor->current.pos.y),y); gabi::store<u32>(gabi::ea(&actor->current.pos.x),x);
            u8 mode=actor->mMode;
            u32 z=gabi::load<u32>(gabi::ea(&actor->mSpawnPos.z));
            s16 angle=actor->mSpawnRotY;
            gabi::store<u32>(gabi::ea(&actor->current.pos.z),z);
            actor->current.angle.y=angle; actor->mMode=(u8)(mode+1);
            actor->mTargetAngleY=angle; actor->shape_angle.y=angle;
            actor->scale.z=0.0f; actor->scale.x=0.0f; actor->scale.y=0.0f;
        }
        return;
    }
    u32 y=gabi::load<u32>(gabi::ea(&actor->mSpawnPos.y));
    u32 x=gabi::load<u32>(gabi::ea(&actor->mSpawnPos.x));
    gabi::store<u32>(gabi::ea(&actor->current.pos.y),y);
    s16 angle=actor->mSpawnRotY;
    gabi::store<u32>(gabi::ea(&actor->current.pos.x),x);
    actor->mTargetAngleY=angle;
    u32 z=gabi::load<u32>(gabi::ea(&actor->mSpawnPos.z));
    actor->current.angle.y=angle; actor->shape_angle.y=angle;
    gabi::store<u32>(gabi::ea(&actor->current.pos.z),z);
    gabi::call(0x0200ED84,&actor->scale.x,1.0f,1.0f,0.1f);
    f32 scale=actor->scale.x;
    actor->scale.z=scale; actor->scale.y=scale;
    if (scale>0.9f) {
        u32 weak=gabi::load<u32>(gabi::ea(actor)+0xA60)&~1u;
        u32 target=gabi::load<u32>(gabi::ea(actor)+0x804)&~1u;
        actor->scale.y=1.0f;
        u32 collision=gabi::load<u32>(gabi::ea(actor)+0x818)&~1u;
        gabi::store<u32>(gabi::ea(actor)+0xA60,weak); gabi::store<u32>(gabi::ea(actor)+0x804,target);
        u32 attack=gabi::load<u32>(gabi::ea(actor)+0x7EC)&~1u;
        actor->mCountUpTimers[1]=0; actor->scale.x=1.0f;
        gabi::store<u32>(gabi::ea(actor)+0x7EC,attack); actor->scale.z=1.0f;
        gabi::store<u32>(gabi::ea(actor)+0x818,collision);
        gabi::call(0x0251621C,actor->mWeakSph); gabi::call(0x0251621C,actor->mNeedleCyl);
        u8 inactive=actor->mStartsInactive;
        actor->mAction=0; actor->mMode=0;
        if (inactive==1 && actor->mSwitch!=0xFF) {
            u32 save=gabi::load<u32>(0x101F84DC);
            s8 room=gabi::load<s8>(0x1047E6C8);
            if (!gabi::call<BOOL>(0x025BA0C0,gabi::at<void>(save+0x20),(u8)actor->mSwitch,room)) {
                gabi::store<u32>(gabi::ea(actor)+0x39C,0);
                actor->mCountDownTimers[2]=600; actor->mAction=1; actor->mMode=12;
            }
        }
    }
}
VERIFY(0x02050360,action_modoru_move);

static BOOL useHeapInit(am2_class* actor) {
    WWHD_FUNC(0x0204FA28,BOOL,actor);
    void* modelData=am2ObjectResource(18,0x100070E4);
    void* animation=am2ObjectResource(15,0x100070E4);
    actor->mpMorf=gabi::call<u32>(0x025E4F64,(void*)nullptr,modelData,(void*)nullptr,(void*)nullptr,animation,2,1.0f,0,-1,1,(void*)nullptr,0,0x11020203);
    u32 morph=actor->mpMorf;
    if (!morph || !gabi::load<u32>(morph+0x90)) return FALSE;
    u32 model=gabi::load<u32>(morph+0x90);
    void* btk=gabi::call<void*>(0x0273AD10,0x74);
    if (btk) btk=gabi::call<void*>(0x025E7C6C,btk);
    actor->mpBtkAnm=gabi::ea(btk);
    if (!btk) return FALSE;
    animation=am2ObjectResource(24,0x100070E4);
    btk=gabi::at<void>(actor->mpBtkAnm);
    modelData=gabi::at<void>(gabi::load<u32>(model+0xAC));
    if (!gabi::call<BOOL>(0x025E7CE0,btk,modelData,animation,1,2,1.0f,0,-1,0,0)) return FALSE;
    if (!actor->mpBtkAnm) return FALSE;
    void* brk=gabi::call<void*>(0x0273AD10,0x78);
    if (brk) brk=gabi::call<void*>(0x025E80D0,brk);
    actor->mpBrkAnm=gabi::ea(brk);
    if (!brk) return FALSE;
    animation=am2ObjectResource(21,0x100070E4);
    brk=gabi::at<void>(actor->mpBrkAnm);
    modelData=gabi::at<void>(gabi::load<u32>(model+0xAC));
    if (!gabi::call<BOOL>(0x025E8154,brk,modelData,animation,1,2,1.0f,0,-1,0,0)) return FALSE;
    if (!actor->mpBrkAnm) return FALSE;
    morph=actor->mpMorf; model=gabi::load<u32>(morph+0x90);
    gabi::store<u32>(model+0xB8,gabi::ea(actor));
    morph=actor->mpMorf; model=gabi::load<u32>(morph+0x90);
    modelData=gabi::at<void>(gabi::load<u32>(model+0xAC));
    // This target is a relative-pointer table accessor, despite the stale __nw matcher name.
    u32 jointTable=gabi::call<u32>(0x027F3F94,modelData);
    u16 count=gabi::load<u16>(jointTable+8);
    morph=actor->mpMorf;
    for (u16 number=0;number<count;number=(u16)(number+1)) {
        model=gabi::load<u32>(morph+0x90);
        u32 data=gabi::load<u32>(model+0xAC);
        u32 limit=gabi::load<u32>(data+4);
        u32 table=gabi::load<u32>(data+8);
        u32 joint=number<limit ? table+number*0x1C : table;
        gabi::store<u32>(joint+8,0x0204C2C4);
        morph=actor->mpMorf; model=gabi::load<u32>(morph+0x90);
        jointTable=gabi::call<u32>(0x027F3F94,gabi::at<void>(gabi::load<u32>(model+0xAC)));
        count=gabi::load<u16>(jointTable+8);
        morph=actor->mpMorf;
    }
    u32 hit=gabi::call<u32>(0x02552B60,gabi::at<void>(gabi::load<u32>(morph+0x90)),gabi::at<void>(0x1018FA54),2);
    actor->mEyeJntHit=hit;
    if (!hit) return FALSE;
    actor->jntHit=hit;
    return TRUE;
}
VERIFY(0x0204FA28,useHeapInit);

static s32 daAM2_Create(am2_class* actor) {
    WWHD_FUNC(0x0204FE84,s32,actor);
    if (!(actor->actor_condition&8)) {
        if (actor) am2_ctor(actor);
        actor->actor_condition=actor->actor_condition|8;
    }
    s32 phase=gabi::call<s32>(0x02520460,&actor->mPhase,STR(0x10007104));
    if (phase!=4) return phase;
    if (!gabi::call<BOOL>(0x025D63E8,actor,0x0204FA28,0x1AA0)) return 5;
    u32 base=gabi::ea(actor);
    gabi::store<u8>(base+0xB85,0);
    u32 params=actor->mParameters;
    actor->mType=(u8)params;
    actor->mPrmAreaRadius=(u8)(actor->mParameters>>8);
    actor->mStartsInactive=(u8)(actor->mParameters>>16);
    actor->mSwitch=(u8)(actor->mParameters>>24);
    if ((u8)params==0xFF) actor->mType=0;
    if (actor->mStartsInactive==0xFF) actor->mStartsInactive=0;
    if (gabi::load<s16>(0x1047BB1A)!=0) actor->mType=1;
    u8 radius=actor->mPrmAreaRadius;
    actor->mAreaRadius=(radius==0xFF || radius==0) ? 600.0f : (f32)radius*100.0f;
    if (!actor->mStartsInactive && actor->mSwitch!=0xFF) {
        u32 save=gabi::load<u32>(0x101F84DC);
        s8 room=gabi::load<s8>(0x1047E6C8);
        if (gabi::call<BOOL>(0x025BA0C0,gabi::at<void>(save+0x20),(u8)actor->mSwitch,room)) return 5;
    }
    u32 play=gabi::call<u32>(0x025200D4);
    actor->itemTableIdx=gabi::call<s32>(0x0200E814,gabi::at<void>(play+0x50AC),STR(0x10007108),0);
    actor->max_health=2; actor->health=2; actor->stealItemLeft=3;
    actor->model=gabi::load<u32>(actor->mpMorf+0x90);
    gabi::call(0x025200D4);
    u32 model=gabi::load<u32>(actor->mpMorf+0x90);
    actor->cullMtx=model ? model+0xC8 : 0;
    gabi::call(0x025D674C,actor,-50.0f,0.0f,-20.0f,60.0f,180.0f,60.0f);
    gabi::store<u32>(base+0x39C,0);
    gabi::call(0x024F06B4,actor->mAcch,&actor->current.pos,&actor->old.pos,actor,1,actor->mAcchCir,&actor->speed,(void*)nullptr,(void*)nullptr);
    void* status=actor->mStts;
    gabi::call(0x02515F14,status,0xFE,1,actor);
    actor->gravity=-3.0f;
    gabi::store<f32>(base+0xD48,50.0f); gabi::store<f32>(base+0xD44,100.0f);
    gabi::store<u32>(base+0xBA8,base);
    gabi::store<u8>(base+0x38C,9);
    actor->actor_status=actor->actor_status|0x08000000;
    gabi::call(0x0251677C,actor->mEyeSph,gabi::at<void>(0x1018FA90));
    u32 flags=gabi::load<u32>(base+0x934);
    gabi::store<u32>(base+0x960,gabi::ea(status)); gabi::store<u32>(base+0x934,flags&~1u);
    gabi::call(0x0251677C,actor->mWeakSph,gabi::at<void>(0x1018FAD0));
    flags=gabi::load<u32>(base+0xA60);
    gabi::store<u32>(base+0xA8C,gabi::ea(status)); gabi::store<u32>(base+0xA60,flags&~1u);
    gabi::call(0x02516518,actor->mBodyCyl,gabi::at<void>(0x1018FB10));
    gabi::store<u32>(base+0x700,gabi::ea(status));
    gabi::call(0x02516518,actor->mNeedleCyl,gabi::at<void>(0x1018FB54));
    u32 attack=gabi::load<u32>(base+0x7EC),target=gabi::load<u32>(base+0x804),collision=gabi::load<u32>(base+0x818);
    gabi::store<u32>(base+0x7EC,attack&~1u); gabi::store<u32>(base+0x830,gabi::ea(status));
    gabi::store<u32>(base+0x818,collision&~1u); gabi::store<u32>(base+0x804,target&~1u);
    actor->m304.copy(actor->current.pos);
    actor->mTargetAngleY=actor->current.angle.y;
    actor->mSpawnPos.copy(actor->current.pos); actor->mSpawnRotY=actor->shape_angle.y;
    BG_check(actor); draw_SUB(actor);
    if (actor->mStartsInactive==1 && actor->mSwitch!=0xFF) {
        u32 save=gabi::load<u32>(0x101F84DC);
        s8 room=gabi::load<s8>(0x1047E6C8);
        if (!gabi::call<BOOL>(0x025BA0C0,gabi::at<void>(save+0x20),(u8)actor->mSwitch,room)) {
            gabi::store<u32>(base+0x39C,0); actor->mCountDownTimers[2]=600; actor->mAction=1; actor->mMode=12;
        }
    }
    actor->mAcchRadius=gabi::load<f32>(0x1047BAB8)+40.0f;
    return phase;
}
VERIFY(0x0204FE84,daAM2_Create);

struct Am2AttackInfo {
    be<u32> object;
    u8 _04[0x10];
    be<u32> particlePosition;
    u8 _18[4];
};
static void am2HitSound(am2_class* actor,u32 id) {
    if (gabi::ea(&actor->eyePos)) {
        s32 reverb=gabi::call<s32>(0x02520540,(s8)actor->current.roomNo);
        gabi::call(0x025E1A40,id,&actor->eyePos,0x35,reverb);
    }
}
static BOOL week_atari_check(am2_class* actor) {
    WWHD_FUNC(0x0204C7C8,BOOL,actor);
    u32 play=gabi::call<u32>(0x025200D4);
    u32 player=gabi::load<u32>(play+0x5B2C);
    play=gabi::call<u32>(0x025200D4);
    u32 target=gabi::load<u32>(play+0x5B2C);
    s16 angle=gabi::call<s16>(0x025D6894,actor,gabi::at<void>(target));
    s16 distance=gabi::call<s16>(0x0200FAAC,(s16)actor->shape_angle.y,angle);
    if (distance<0x4000) return FALSE;
    gabi::call(0x02515E50,gabi::at<void>(gabi::ea(actor)+0x69C));
    actor->mHitDirection=0;
    if (!gabi::call<BOOL>(0x025162A4,actor->mWeakSph)) { actor->mbIsWeakBeingHit=0; return FALSE; }
    if (actor->mbIsWeakBeingHit) return FALSE;
    u32 hit=gabi::call<u32>(0x02516300,actor->mWeakSph);
    actor->mbIsWeakBeingHit=1;
    if (!hit) return FALSE;
    u32 type=gabi::load<u32>(hit+0x10);
    if (type&0x100000) {
        gabi::store<u32>(gabi::ea(actor)+0x39C,0);
        gabi::store<f32>(gabi::ea(actor)+0xBB0,80.0f);
        gabi::store<f32>(gabi::ea(actor)+0xD54,1.0f);
        gabi::store<u8>(gabi::ea(actor)+0xBAE,1);
        return TRUE;
    }
    u8 hitType=0;
    switch(type) {
    case 2: case 0x400: case 0x800: case 0x4000000: case 0x10000000: {
        am2HitSound(actor,0x2803);
        u8 cut=gabi::load<u8>(player+0x3AC);
        if ((cut>=6&&cut<=10)||cut==12||(cut>=14&&cut<=16)||cut==21||(cut>=25&&cut<=27)||(cut>=30&&cut<=31)) {
            hitType=1; actor->mHitDirection=1;
        }
        break;
    }
    case 0x200000: actor->mHitDirection=3; return TRUE;
    case 0x40: actor->mHitDirection=4; return FALSE;
    case 0x80: case 0x2000: case 0x1000000: am2HitSound(actor,0x2833); break;
    case 0x10000:
        am2HitSound(actor,0x2855); actor->mHitDirection=7; hitType=1;
        if (gabi::load<u8>(player+0x3AC)==0x11) actor->mHitDirection=8;
        break;
    case 0x20: hitType=1; actor->mHitDirection=6; break;
    case 0x4000: case 0x40000: case 0x80000:
        hitType=1; actor->mHitDirection=5; am2HitSound(actor,0x2834); break;
    default: am2HitSound(actor,0x2834); break;
    }
    gabi::Local<cXyz> hitPosition;
    gabi::Local<cXyz> scale;
    gabi::Local<Am2AttackInfo> info;
    gabi::Local<u64> linkage;
    u32 base=gabi::ea(actor);
    hitPosition->x=gabi::load<f32>(base+0xB14); hitPosition->y=gabi::load<f32>(base+0xB18); hitPosition->z=gabi::load<f32>(base+0xB1C);
    hit=gabi::call<u32>(0x02516300,actor->mWeakSph);
    info->object=hit; info->particlePosition=0;
    gabi::call(0x025192A8,actor,info.get());
    play=gabi::call<u32>(0x025200D4);
    u32 particles=gabi::load<u32>(play+0x5AB0);
    auto angles=gabi::at<csXyz>(player+0x328);
    if (hitType==1) {
        gabi::call(0x025A847C,gabi::at<void>(particles),0,0x10,hitPosition.get(),(void*)nullptr,(void*)nullptr,0xFF,(void*)nullptr,-1,(void*)nullptr,(void*)nullptr,(void*)nullptr);
        scale->x=2.0f; scale->z=2.0f; scale->y=2.0f;
        play=gabi::call<u32>(0x025200D4); particles=gabi::load<u32>(play+0x5AB0);
        gabi::call(0x025A847C,gabi::at<void>(particles),0,0xF,hitPosition.get(),angles,scale.get(),0xFF,(void*)nullptr,-1,(void*)nullptr,(void*)nullptr,(void*)nullptr);
    } else {
        gabi::call(0x025A847C,gabi::at<void>(particles),0,0xD,hitPosition.get(),angles,(void*)nullptr,0xFF,(void*)nullptr,-1,(void*)nullptr,(void*)nullptr,(void*)nullptr);
    }
    u8 direction=actor->mHitDirection;
    actor->mAction=2; actor->mMode=20;
    if (direction==7||direction==8) actor->health=0;
    return TRUE;
}
VERIFY(0x0204C7C8,week_atari_check);

static void am2SoundAtEye(am2_class* actor,u32 id,u32 parameter=0) {
    if (actor && gabi::ea(&actor->eyePos)) {
        s32 room=(s8)actor->current.roomNo;
        s32 reverb=gabi::call<s32>(0x02520540,room);
        gabi::call(0x025E1A40,id,&actor->eyePos,parameter,reverb);
    }
}
static void am2Voice(am2_class* actor,u32 id) {
    if (gabi::ea(&actor->eyePos)) {
        s8 room=actor->current.roomNo;
        s32 process=actor ? gabi::load<s32>(gabi::ea(actor)+4) : -1;
        s32 reverb=gabi::call<s32>(0x02520540,room);
        gabi::call(0x025E1AA4,id,&actor->eyePos,process,0,reverb);
    }
}
static u32 am2Player() {
    u32 play=gabi::call<u32>(0x025200D4);
    return gabi::load<u32>(play+0x5B2C);
}
static f32 am2PlayerDistance(am2_class* actor) {
    u32 player=am2Player();
    return gabi::call<f32>(0x025D68EC,actor,gabi::at<void>(player));
}
static s16 am2PlayerAngle(am2_class* actor) {
    u32 player=am2Player();
    return gabi::call<s16>(0x025D6894,actor,gabi::at<void>(player));
}
static void am2Flag(am2_class* actor,u32 offset,bool enable) {
    u32 address=gabi::ea(actor)+offset;
    u32 flags=gabi::load<u32>(address);
    gabi::store<u32>(address,enable ? flags|1u : flags&~1u);
}
static bool am2Stopped(am2_class* actor) {
    u32 morph=actor->mpMorf;
    return (gabi::load<u8>(morph+0xA7)&1) || gabi::load<f32>(morph+0x98)==0.0f;
}
static void am2ResetUpTimers(am2_class* actor) {
    for (auto& timer:actor->mCountUpTimers) timer=0;
}
static void am2Fell(am2_class* actor) {
    if (actor->mbNotInHomeRoom) gabi::call(0x025D57E0,actor);
    else { actor->mMode=40; actor->mAction=4; }
}
static void am2Particle(am2_class* actor,s32 group,u16 id,const cXyz* position,const csXyz* angle=nullptr,const cXyz* scale=nullptr,u8 alpha=0xFF,void* callback=nullptr,s32 room=-1) {
    u32 play=gabi::call<u32>(0x025200D4);
    u32 particles=gabi::load<u32>(play+0x5AB0);
    gabi::call(0x025A847C,gabi::at<void>(particles),group,id,position,angle,scale,alpha,callback,room,(void*)nullptr,(void*)nullptr,(void*)nullptr);
}

// HD embeds the eye-hit state machine in Execute.
static bool am2EyeHit(am2_class* actor) {
    u32 player=am2Player();
    if (actor->mStartsInactive==1 && actor->mSwitch!=0xFF) {
        u32 save=gabi::load<u32>(0x101F84DC);
        s8 room=gabi::load<s8>(0x1047E6C8);
        if (!gabi::call<BOOL>(0x025BA0C0,gabi::at<void>(save+0x20),(u8)actor->mSwitch,room)) return false;
    }
    gabi::call(0x02515E50,gabi::at<void>(gabi::ea(actor)+0x69C));
    if (!gabi::call<BOOL>(0x025162A4,actor->mEyeSph)) return false;
    u32 hit=gabi::call<u32>(0x02516300,actor->mEyeSph);
    if (!hit) return false;
    gabi::Local<cXyz> position;
    gabi::Local<Am2AttackInfo> info;
    gabi::Local<u64> linkage;
    u32 base=gabi::ea(actor);
    position->x=gabi::load<f32>(base+0x9E8); position->y=gabi::load<f32>(base+0x9EC); position->z=gabi::load<f32>(base+0x9F0);
    u32 type=gabi::load<u32>(hit+0x10);
    if (type&0x08000000) {
        if (actor->mCurrBckIdx!=13) {
            if (actor->stealItemLeft>0) {
                u8 health=gabi::load<u8>(base+0x3A1); actor->health=10;
                hit=gabi::call<u32>(0x02516300,actor->mEyeSph);
                info->object=hit; info->particlePosition=0;
                gabi::call(0x025192A8,actor,info.get());
                gabi::store<u8>(base+0x3A1,health);
                am2Particle(actor,0,0x27B,gabi::at<cXyz>(base+0x390));
            } else am2Particle(actor,0,0xC,position.get());
            am2SoundAtEye(actor,0x2834,0x42);
        }
        return true;
    }
    if (type&0x100000) {
        gabi::store<f32>(base+0xD54,1.0f); gabi::store<u32>(base+0x39C,0);
        gabi::store<u8>(base+0xBAE,1); gabi::store<f32>(base+0xBB0,80.0f);
        return true;
    }
    if (!(type&0xC4000)) return false;
    if (actor->mCurrBckIdx==13) {
        anm_init(actor,15,1.0f,0,1.0f,-1);
        u32 status=actor->actor_status;
        actor->mAction=0;
        am2Flag(actor,0x804,true);
        am2Flag(actor,0x7EC,true);
        gabi::store<u32>(base+0x7F0,1);
        actor->actor_status=status|0x20;
        actor->mMode=2; gabi::store<u32>(base+0x39C,4);
    } else {
        am2Particle(actor,0,0x10,position.get(),gabi::at<csXyz>(player+0x328));
        am2SoundAtEye(actor,0x58B4); am2Voice(actor,0x48B5);
        actor->mAction=1; actor->mMode=10;
    }
    return true;
}

static void action_dousa(am2_class* actor) {
    u32 player=am2Player();
    gabi::Local<cXyz> offset,rotated,destination;
    gabi::Local<u64> linkage;
    u8 mode=actor->mMode;
    if (mode==4 || mode==5) {
        u32 matrix=gabi::load<u32>(0x1018C7B0);
        gabi::call(0x025F1884,gabi::at<void>(matrix),(s16)actor->current.angle.y);
        offset->x=0.0f; offset->y=0.0f; offset->z=200.0f;
        gabi::call(0x0200FCD8,offset.get(),rotated.get());
        gabi::call(0x028E8D88,rotated.get(),&actor->current.pos,rotated.get());
        f32 extra=gabi::load<f32>(0x1047BD1C)+100.0f;
        rotated->y=(f32)rotated->y+extra;
        mode=actor->mMode;
    }
    switch (mode) {
    case 0:
        am2ResetUpTimers(actor);
        actor->mAcchRadius=gabi::load<f32>(0x1047BABC)+80.0f;
        if (actor->mCurrBckIdx!=15) anm_init(actor,15,10.0f,0,1.0f,-1);
        actor->mMode=(u8)(actor->mMode+1);
        [[fallthrough]];
    case 1: {
        f32 distance=am2PlayerDistance(actor);
        if (distance<(f32)actor->mAreaRadius) {
            f32 extra=gabi::load<f32>(0x1047BD1C)+100.0f;
            f32 y=gabi::load<f32>(player+0x318);
            destination->x=gabi::load<f32>(player+0x314); destination->z=gabi::load<f32>(player+0x31C); destination->y=y+extra;
            if (Line_check(actor,destination.get())) {
                u32 status=actor->actor_status;
                gabi::store<u32>(gabi::ea(actor)+0x39C,4); actor->actor_status=status|0x20;
                anm_init(actor,14,1.0f,0,1.0f,-1); am2Voice(actor,0x48B3);
                u8 mode=actor->mMode;
                u32 eye=gabi::load<u32>(gabi::ea(actor)+0x934),weak=gabi::load<u32>(gabi::ea(actor)+0xA60);
                actor->mMode=(u8)(mode+1);
                gabi::store<u32>(gabi::ea(actor)+0xA60,weak|1); gabi::store<u32>(gabi::ea(actor)+0x934,eye|1);
            }
        }
        break;
    }
    case 2: {
        u32 morph=actor->mpMorf;
        if (gabi::call<BOOL>(0x027F2BF8,gabi::at<void>(morph+0x98),24.0f)) am2SoundAtEye(actor,0x58B9);
        if (!am2Stopped(actor)) break;
        actor->mMode=(u8)(actor->mMode+1);
        [[fallthrough]];
    }
    case 3: {
        am2ResetUpTimers(actor); actor->speedF=0.0f;
        f32 distance=am2PlayerDistance(actor);
        f32 radius=(f32)actor->mAreaRadius+200.0f;
        if (distance>radius) actor->mMode=6;
        else {
            u32 attack=gabi::load<u32>(gabi::ea(actor)+0x7EC),target=gabi::load<u32>(gabi::ea(actor)+0x804);
            gabi::store<u32>(gabi::ea(actor)+0x7F0,1); gabi::store<u32>(gabi::ea(actor)+0x7EC,attack|1); gabi::store<u32>(gabi::ea(actor)+0x804,target|1);
            s16 angle=am2PlayerAngle(actor); s32 animation=actor->mCurrBckIdx;
            actor->mTargetAngleY=angle; actor->gravity=-3.0f; actor->speed.y=12.0f;
            if (animation!=11) anm_init(actor,11,2.0f,2,1.0f,-1);
            actor->mMode=(u8)(actor->mMode+1);
        }
        break;
    }
    case 4:
        if (gabi::load<u32>(gabi::ea(actor)+0x4E4)&0x20) {
            actor->gravity=-3.0f; actor->speed.y=12.0f; am2SoundAtEye(actor,0x58B3);
            actor->mMode=3;
            s16 distance=gabi::call<s16>(0x0200FAAC,(s16)actor->shape_angle.y,(s16)actor->current.angle.y);
            if (distance<0x1000) {
                destination->z=rotated->z; destination->y=rotated->y; destination->x=rotated->x;
                if (Line_check(actor,destination.get()) || gabi::load<s16>(player+0x3B0)==0) actor->mMode=5;
            }
        }
        break;
    case 5: {
        f32 speed=actor->speedF;
        u32 ground=gabi::load<u32>(gabi::ea(actor)+0x4E4);
        if (speed>0.0f && (gabi::load<u32>(gabi::ea(actor)+0x840)&1)) { actor->speed.y=0.0f; actor->speedF=-9.0f; }
        if (ground&0x20) {
            am2SoundAtEye(actor,0x58B3); gabi::call(0x025A5F88,actor->mSmokeCb); am2SoundAtEye(actor,0x5882);
            s8 room=actor->current.roomNo;
            am2Particle(actor,2,0xA125,&actor->current.pos,&actor->shape_angle,nullptr,0xB9,actor->mSmokeCb,room);
            u32 emitter=gabi::load<u32>(gabi::ea(actor)+0xB78);
            if (emitter) {
                gabi::store<f32>(emitter+0x34,12.0f); emitter=gabi::load<u32>(gabi::ea(actor)+0xB78);
                for (u32 off:{0x224u,0x240u,0x238u,0x23Cu,0x220u,0x228u}) gabi::store<f32>(emitter+off,0.45f);
            }
            if (actor->mCountUpTimers[0]>8) actor->mMode=3;
            else {
                actor->speedF=9.0f; actor->speed.y=20.0f; actor->gravity=-8.0f;
                destination->z=rotated->z; destination->x=rotated->x; destination->y=rotated->y;
                actor->mCountUpTimers[1]=0;
                BOOL clear=Line_check(actor,destination.get()); s16 count=actor->mCountUpTimers[0];
                if (!clear || gabi::load<s16>(player+0x3B0)!=0) actor->speedF=0.0f;
                actor->mCountUpTimers[0]=(s16)(count+1);
            }
        }
        break;
    }
    case 6:
        anm_init(actor,13,1.0f,0,1.0f,-1); am2SoundAtEye(actor,0x58BA);
        am2Flag(actor,0xA60,false); gabi::call(0x0251621C,actor->mWeakSph);
        actor->mMode=(u8)(actor->mMode+1);
        [[fallthrough]];
    case 7:
        if (am2Stopped(actor)) {
            am2ResetUpTimers(actor);
            u32 attack=gabi::load<u32>(gabi::ea(actor)+0x7EC),target=gabi::load<u32>(gabi::ea(actor)+0x804);
            gabi::store<u32>(gabi::ea(actor)+0x7EC,attack&~1u); gabi::store<u32>(gabi::ea(actor)+0x804,target&~1u);
            gabi::call(0x0251621C,actor->mNeedleCyl);
            actor->mMode=1;
            u32 z=gabi::load<u32>(gabi::ea(&actor->current.pos.z)); gabi::store<u32>(gabi::ea(&actor->m304.z),z);
            u32 x=gabi::load<u32>(gabi::ea(&actor->current.pos.x)),y=gabi::load<u32>(gabi::ea(&actor->current.pos.y));
            gabi::store<u32>(gabi::ea(&actor->m304.x),x); gabi::store<u32>(gabi::ea(&actor->m304.y),y); actor->mAction=0;
        }
        break;
    }
    gabi::call(0x0200F428,&actor->current.angle.y,(s16)actor->mTargetAngleY,1,0x500);
    gabi::call(0x0200F428,&actor->shape_angle.y,(s16)actor->current.angle.y,1,0x500);
    if (naraku_check(actor)) am2Fell(actor);
    else if (!am2EyeHit(actor)) {
        if (actor->mMode<3 || !week_atari_check(actor)) body_atari_check(actor);
    }
}

static BOOL am2Switch(am2_class* actor) {
    u32 save=gabi::load<u32>(0x101F84DC); s8 room=gabi::load<s8>(0x1047E6C8);
    return gabi::call<BOOL>(0x025BA0C0,gabi::at<void>(save+0x20),(u8)actor->mSwitch,room);
}
static void am2Attention(am2_class* actor,u32 bits,bool enable) {
    u32 address=gabi::ea(actor)+0x39C;
    u32 flags=gabi::load<u32>(address); gabi::store<u32>(address,enable ? flags|bits : flags&~bits);
}
static void am2Smoke(am2_class* actor,f32 scale) {
    s8 room=actor->current.roomNo;
    am2Particle(actor,2,0xA125,&actor->current.pos,&actor->shape_angle,nullptr,0xB9,actor->mSmokeCb,room);
    u32 emitter=gabi::load<u32>(gabi::ea(actor)+0xB78);
    if (emitter) {
        gabi::store<f32>(emitter+0x34,12.0f); emitter=gabi::load<u32>(gabi::ea(actor)+0xB78);
        for (u32 offset:{0x224u,0x240u,0x238u,0x23Cu,0x220u,0x228u}) gabi::store<f32>(emitter+offset,scale);
    }
}
static void action_mahi(am2_class* actor) {
    u32 player=am2Player(); u32 base=gabi::ea(actor);
    switch ((u8)actor->mMode) {
    case 10: {
        am2ResetUpTimers(actor);
        u32 attack=gabi::load<u32>(base+0x7EC),target=gabi::load<u32>(base+0x804);
        gabi::store<u32>(base+0x7EC,attack&~1u); gabi::store<u32>(base+0x804,target&~1u);
        gabi::call(0x0251621C,actor->mNeedleCyl);
        actor->mTargetAngleY=(s16)(am2PlayerAngle(actor)+0x8000);
        actor->current.angle.y=(s16)(am2PlayerAngle(actor)+0x8000);
        actor->mCountDownTimers[2]=0; actor->mCountDownTimers[3]=0; actor->speedF=20.0f;
        am2SoundAtEye(actor,0x58BA); anm_init(actor,7,1.0f,0,1.0f,-1);
        actor->mMode=(u8)(actor->mMode+1); break;
    }
    case 11:
        gabi::call(0x0200EDC8,&actor->speedF,0.5f,1.0f);
        if (actor->speedF<0.2f && am2Stopped(actor)) {
            actor->speedF=0.0f; actor->mCountDownTimers[2]=600; anm_init(actor,12,1.0f,2,1.0f,-1);
            actor->mMode=(u8)(actor->mMode+1);
        }
        break;
    case 12:
        if (actor->mCountDownTimers[3]==0 || actor->mCountDownTimers[3]>3) am2Attention(actor,0x10,true);
        if (naraku_check(actor)) am2Fell(actor);
        else {
            if (actor->mCountUpTimers[1]!=0 && (gabi::load<u32>(base+0x4E4)&0x20)) {
                am2SoundAtEye(actor,0x58B2); actor->mCountUpTimers[1]=0;
            }
            if (actor->actor_status&0x2000) {
                actor->mbMadeWaterSplash=0; actor->mAcchRadius=gabi::load<f32>(0x1047BAB8)+40.0f;
                gabi::call(0x025A9270,actor->mRippleCb);
                u32 status=actor->actor_status; u8 mode=actor->mMode; u32 collision=gabi::load<u32>(base+0x6E8);
                f32 y=actor->current.pos.y;
                actor->actor_status=status&~0x20u; actor->mPickedUpYPos=y;
                s16 angle=gabi::load<s16>(player+0x32A);
                actor->speedF=0.0f; actor->mbNotInHomeRoom=0; actor->speed.y=0.0f; actor->mMode=(u8)(mode+1);
                actor->speed.x=0.0f; actor->mTargetAngleY=angle; actor->speed.z=0.0f; actor->current.angle.y=angle; actor->gravity=0.0f;
                gabi::store<u32>(base+0x6E8,collision&~1u);
            }
        }
        break;
    case 13: {
        s8 room=actor->current.roomNo,home=actor->home.roomNo;
        s16 angle=gabi::load<s16>(player+0x32A); actor->current.angle.y=angle;
        if (home!=room) actor->mbNotInHomeRoom=1;
        f32 picked=(f32)actor->mPickedUpYPos+10.0f;
        if (!(picked>(f32)actor->current.pos.y)) gabi::call(0x0200F428,&actor->shape_angle.y,(s16)actor->current.angle.y,1,0x1000);
        if (!(actor->actor_status&0x2000)) {
            u32 collision=gabi::load<u32>(base+0x6E8);
            f32 radius=gabi::load<f32>(0x1047BAB8)+40.0f; f32 speed=actor->speedF;
            gabi::store<u32>(base+0x6E8,collision|1); actor->mAcchRadius=radius;
            if (speed>0.0f) {
                u32 flags=gabi::load<u32>(base+0x4E4);
                actor->gravity=-5.0f; actor->mMode=14; gabi::store<u32>(base+0x4E4,flags|0x2000);
                actor->speedF=35.0f; actor->speed.y=25.0f;
            } else { actor->gravity=-3.0f; actor->mCountUpTimers[1]=1; actor->mMode=12; }
        }
        break;
    }
    case 14:
        if (gabi::load<u32>(base+0x4E4)&0x10) actor->speedF=0.0f;
        if (naraku_check(actor)) {
            if (actor->mbNotInHomeRoom) gabi::call(0x025D57E0,actor);
            else {
                u32 flags=gabi::load<u32>(base+0x4E4); actor->mAction=4;
                gabi::store<u32>(base+0x4E4,flags&~0x2000u); actor->mMode=40;
            }
        } else {
            if (gabi::load<u32>(base+0x4E4)&0x20) {
                gabi::Local<cXyz> direction; gabi::Local<u64> linkage;
                u32 play=gabi::call<u32>(0x025200D4);
                direction->x=0.0f; direction->y=1.0f; direction->z=0.0f;
                gabi::call(0x025CB374,gabi::at<void>(play+0x599C),1,-0x21,direction.get());
                play=gabi::call<u32>(0x025200D4);
                u32 camera=gabi::load<u32>(play+0x5AF8);
                s32 id=actor ? gabi::load<s32>(base+4) : -1;
                gabi::call(0x025052BC,gabi::at<void>(camera+0x248),id);
                u32 flags=gabi::load<u32>(base+0x4E4); actor->mbMadeWaterSplash=0;
                gabi::store<u32>(base+0x4E4,flags&~0x2000u); gabi::call(0x025A9270,actor->mRippleCb);
                if (!actor->mCountUpTimers[0]) {
                    gabi::call(0x025A5F88,actor->mSmokeCb); am2SoundAtEye(actor,0x58B3); am2Smoke(actor,0.8f);
                    s16 count=actor->mCountUpTimers[0]; actor->speed.y=25.0f; actor->mCountUpTimers[0]=(s16)(count+1);
                    actor->speedF=7.0f; actor->gravity=-14.0f;
                } else { actor->speedF=0.0f; actor->mCountUpTimers[0]=0; actor->mMode=12; }
            }
            gabi::call(0x0200EDC8,&actor->speedF,0.5f,1.0f);
        }
        break;
    case 15:
        if (naraku_check(actor)) {
            if (actor->mbNotInHomeRoom) gabi::call(0x025D57E0,actor);
            else { actor->mMode=40; actor->mAction=4; return; }
        }
        actor->shape_angle.y=(s16)(actor->shape_angle.y+0x1000);
        if (gabi::load<u32>(base+0x4E4)&0x20) {
            actor->gravity=-3.0f; actor->actor_status=actor->actor_status|0x20; actor->mMode=3;
            gabi::store<u32>(base+0x39C,4); actor->mAction=0;
        }
        break;
    }
    actor->mTargetAngleY=actor->current.angle.y;
    u8 mode=actor->mMode;
    if (mode>=12 && mode!=15) {
        if (actor->mStartsInactive==1 && actor->mSwitch!=0xFF) {
            if (!am2Switch(actor)) { actor->mCountDownTimers[2]=600; body_atari_check(actor); return; }
            u32 status=actor->actor_status;
            actor->mMode=0; actor->mSwitch=0xFF; actor->mAction=0;
            if (status&0x2000) {
                gabi::call(0x025D9D24,actor); am2Attention(actor,0x10,false); actor->speed.y=20.0f; actor->gravity=-4.0f;
            }
            return;
        }
        if (!actor->mCountDownTimers[2]) {
            if (actor->mCurrBckIdx!=6) {
                anm_init(actor,6,1.0f,2,1.0f,-1); actor->mCountDownTimers[3]=600;
                am2Attention(actor,0x10,false); am2SoundAtEye(actor,0x58B5); am2Voice(actor,0x48B3);
            }
            if (actor->mCountDownTimers[3]==1) {
                u32 status=actor->actor_status;
                if (status&0x2000) { gabi::call(0x025D9D24,actor); am2Attention(actor,0x10,false); status=actor->actor_status; }
                u32 collision=gabi::load<u32>(base+0x6E8);
                actor->speed.y=20.0f; actor->gravity=-4.0f; gabi::store<u32>(base+0x6E8,collision|1);
                actor->mMode=15; actor->actor_status=status|0x20;
            }
        }
    }
    if ((actor->actor_status&0x2000)||actor->mMode==15||!week_atari_check(actor)) body_atari_check(actor);
}

static void action_itai(am2_class* actor) {
    gabi::call(0x025200D4);
    u32 base=gabi::ea(actor);
    switch ((u8)actor->mMode) {
    case 20: {
        am2ResetUpTimers(actor);
        u32 attack=gabi::load<u32>(base+0x7EC),target=gabi::load<u32>(base+0x804);
        gabi::store<u32>(base+0x7EC,attack&~1u); gabi::store<u32>(base+0x804,target&~1u); gabi::call(0x0251621C,actor->mNeedleCyl);
        actor->mTargetAngleY=(s16)(am2PlayerAngle(actor)+0x8000); actor->current.angle.y=(s16)(am2PlayerAngle(actor)+0x8000);
        actor->speedF=20.0f; am2Voice(actor,0x48B6);
        am2Particle(actor,0,0x81AE,&actor->mWeakPos,&actor->shape_angle);
        if (actor->health>0) {
            anm_init(actor,7,1.0f,0,1.0f,-1);
            if (actor->mCountDownTimers[2]>5) actor->speedF=5.0f;
            actor->mMode=(u8)(actor->mMode+1);
        } else actor->mMode=22;
        break;
    }
    case 21:
        gabi::call(0x0200EDC8,&actor->speedF,0.5f,1.0f);
        if (actor->speedF<0.2f) {
            actor->speedF=0.0f; actor->gravity=-3.0f;
            if (actor->mCountDownTimers[2]<5) { actor->mAction=0; actor->mMode=3; }
            else { anm_init(actor,12,1.0f,2,1.0f,-1); actor->mAction=1; actor->mMode=12; }
        }
        break;
    case 22: {
        u32 eye=gabi::load<u32>(base+0x934),weak=gabi::load<u32>(base+0xA60);
        gabi::store<u32>(base+0x934,eye&~1u); gabi::store<u32>(base+0xA60,weak&~1u);
        gabi::call(0x0251621C,actor->mEyeSph); gabi::call(0x0251621C,actor->mWeakSph);
        anm_init(actor,8,1.0f,0,1.0f,-1); actor->mMode=(u8)(actor->mMode+1); break;
    }
    case 23:
        if (!am2Stopped(actor)) break;
        {
            u32 attack=gabi::load<u32>(base+0x7EC); gabi::store<u32>(base+0x7F0,1); gabi::store<u32>(base+0x7EC,attack|1);
            actor->gravity=-10.0f; actor->speed.y=25.0f; actor->speedF=10.0f; actor->mCountDownTimers[0]=100;
            actor->current.angle.y=am2PlayerAngle(actor);
            anm_init(actor,9,1.0f,0,1.0f,-1); actor->mMode=(u8)(actor->mMode+1);
        }
        [[fallthrough]];
    case 24:
        if (actor->speed.y>0.0f && !actor->mCountUpTimers[1]) { am2Voice(actor,0x48B4); actor->mCountUpTimers[1]=1; }
        actor->shape_angle.y=(s16)(actor->shape_angle.y+0x1000);
        if (gabi::load<u32>(base+0x4E4)&0x20) {
            gabi::call(0x025A5F88,actor->mSmokeCb); am2Smoke(actor,0.45f);
            am2SoundAtEye(actor,0x58B6); am2Voice(actor,0x4879);
            actor->speedF=10.0f; actor->speed.y=25.0f; actor->gravity=-10.0f;
        }
        if (!actor->mCountDownTimers[0]) {
            anm_init(actor,10,1.0f,0,1.0f,-1); am2SoundAtEye(actor,0x58B7);
            actor->speedF=0.0f; actor->mMode=(u8)(actor->mMode+1);
        }
        break;
    case 25:
        if (am2Stopped(actor)) {
            gabi::Local<cXyz> center; gabi::Local<u64> linkage;
            center->x=actor->current.pos.x; center->y=actor->current.pos.y; center->z=actor->current.pos.z;
            center->y=(f32)center->y+50.0f;
            am2Particle(actor,0,0x81AF,&actor->current.pos,&actor->shape_angle);
            am2Particle(actor,0,0x81B0,&actor->current.pos,&actor->shape_angle);
            am2SoundAtEye(actor,0x58B8);
            gabi::call(0x025D99E8,actor,center.get(),5,0,0xFF);
            s8 room=actor->home.roomNo; u32 save=gabi::load<u32>(0x101F84DC); u16 id=actor->setID;
            gabi::call(0x025BA5D4,gabi::at<void>(save+0x20),id,room); gabi::call(0x025D57E0,actor);
        }
        break;
    }
    if (naraku_check(actor)) {
        if (actor->mbNotInHomeRoom || actor->health<=0) {
            if (actor->mMode!=25) { anm_init(actor,10,1.0f,0,1.0f,-1); am2SoundAtEye(actor,0x58B7); actor->speedF=0.0f; actor->mMode=25; }
        } else { actor->mMode=40; actor->mAction=4; }
    }
}

static void action_handou_move(am2_class* actor) {
    u32 player=am2Player(); u32 base=gabi::ea(actor);
    switch ((u8)actor->mMode) {
    case 30: {
        actor->speedF=40.0f;
        s16 angle=am2PlayerAngle(actor);
        u8 direction=actor->mHitDirection;
        u32 attack=gabi::load<u32>(base+0x7EC),target=gabi::load<u32>(base+0x804);
        actor->current.angle.y=(s16)(angle+0x8000);
        if (direction==8) { actor->current.angle.y=(s16)(gabi::load<s16>(player+0x32A)-0x4000); actor->speedF=40.0f; }
        gabi::store<u32>(base+0x7EC,attack&~1u); gabi::store<u32>(base+0x804,target&~1u); gabi::call(0x0251621C,actor->mNeedleCyl);
        s16 current=actor->current.angle.y; u8 mode=actor->mMode;
        actor->mTargetAngleY=current; actor->mMode=(u8)(mode+1);
        [[fallthrough]];
    }
    case 31:
        am2SoundAtEye(actor,0x50BB); gabi::call(0x0200EDC8,&actor->speedF,0.8f,2.0f);
        if (actor->speedF<0.1f) {
            u8 inactive=actor->mStartsInactive; s16 angle=actor->shape_angle.y;
            actor->mAction=0; actor->current.angle.y=angle; actor->speedF=0.0f; actor->mMode=3;
            if (inactive==1 && actor->mSwitch!=0xFF && !am2Switch(actor)) {
                gabi::store<u32>(base+0x39C,0); actor->mCountDownTimers[2]=600; actor->mAction=1; actor->mMode=12;
            }
        }
        break;
    }
    if (naraku_check(actor)) am2Fell(actor);
}

static BOOL daAM2_Execute(am2_class* actor) {
    WWHD_FUNC(0x0204D3A4,BOOL,actor);
    for (auto& timer:actor->mCountDownTimers) {
        s16 value=timer;
        if (value) timer=(s16)(value-1);
    }
    gabi::call(0x025DA088,actor,0x27,0xB,0x29);
    if (gabi::call<BOOL>(0x020402C8,gabi::at<void>(gabi::ea(actor)+0xBA8))) {
        u32 morph=actor->mpMorf;
        auto matrix=gabi::at<Mtx34>(0x1048D0CC);
        u32 model=gabi::load<u32>(morph+0x90);
        mtx_copy(gabi::at<Mtx34>(model+0xC8),matrix);
        gabi::call(0x025E55A0,gabi::at<void>(actor->mpMorf));
        return TRUE;
    }
    actor->actor_status=actor->actor_status|0x400;
    switch ((u8)actor->mAction) {
    case 0: action_dousa(actor); break;
    case 1: action_mahi(actor); break;
    case 2: action_itai(actor); break;
    case 3: action_handou_move(actor); break;
    case 4: action_modoru_move(actor); break;
    }
    gabi::call(0x025E535C,gabi::at<void>(actor->mpMorf),(void*)nullptr,0,0);
    gabi::call(0x025E742C,gabi::at<void>(actor->mpBtkAnm));
    gabi::call(0x025E742C,gabi::at<void>(actor->mpBrkAnm));
    u32 matrix=gabi::load<u32>(0x1018C7B0);
    gabi::call(0x025F1884,gabi::at<void>(matrix),(s16)actor->current.angle.y);
    gabi::Local<cXyz> offset,rotated;
    gabi::Local<u64> linkage;
    offset->x=0.0f; offset->y=0.0f; offset->z=actor->speedF;
    gabi::call(0x0200FCD8,offset.get(),rotated.get());
    f32 y=actor->speed.y, x=rotated->x, gravity=actor->gravity;
    actor->speed.x=x;
    y+=gravity;
    actor->speed.z=rotated->z;
    if (y<-50.0f) y=-50.0f;
    f32 posX=actor->current.pos.x,posY=actor->current.pos.y;
    actor->speed.y=y;
    f32 posZ=actor->current.pos.z;
    gabi::store<f32>(gabi::ea(actor)+0x390,posX); gabi::store<f32>(gabi::ea(actor)+0x398,posZ); gabi::store<f32>(gabi::ea(actor)+0x394,posY+120.0f);
    actor->eyePos.x=actor->mEyeballPos.x;
    f32 eyeY=actor->mEyeballPos.y;
    actor->eyePos.y=eyeY; actor->eyePos.z=actor->mEyeballPos.z;
    actor->eyePos.y=eyeY-(gabi::load<f32>(0x1047BA98)+15.0f);
    u32 base=gabi::ea(actor);
    gabi::call(0x020182E0,gabi::at<void>(base+0x7D4),&actor->current.pos);
    gabi::call(0x02018428,gabi::at<void>(base+0x7D4),150.0f);
    gabi::call(0x020184DC,gabi::at<void>(base+0x7D4),35.0f);
    u32 play=gabi::call<u32>(0x025200D4);
    gabi::call(0x0200E240,gabi::at<void>(play+0x26A4),actor->mBodyCyl);
    gabi::call(0x020182E0,gabi::at<void>(base+0x904),&actor->mNeedlePos);
    gabi::call(0x02018428,gabi::at<void>(base+0x904),20.0f);
    gabi::call(0x020184DC,gabi::at<void>(base+0x904),gabi::load<f32>(0x1047BA9C)+55.0f);
    play=gabi::call<u32>(0x025200D4);
    gabi::call(0x0200E240,gabi::at<void>(play+0x26A4),actor->mNeedleCyl);
    gabi::call(0x02018D40,gabi::at<void>(base+0xA34),&actor->mEyeballPos);
    gabi::call(0x02018C8C,gabi::at<void>(base+0xA34),30.0f);
    play=gabi::call<u32>(0x025200D4);
    gabi::call(0x0200E240,gabi::at<void>(play+0x26A4),actor->mEyeSph);
    gabi::call(0x02018D40,gabi::at<void>(base+0xB60),&actor->mWeakPos);
    gabi::call(0x02018C8C,gabi::at<void>(base+0xB60),30.0f);
    play=gabi::call<u32>(0x025200D4);
    gabi::call(0x0200E240,gabi::at<void>(play+0x26A4),actor->mWeakSph);
    void* collision=(gabi::load<u32>(base+0x6E8)&1) ? actor->mStts : nullptr;
    gabi::call(0x025D6800,actor,collision);
    if (actor->mAction!=4) BG_check(actor);
    draw_SUB(actor);
    return TRUE;
}
VERIFY(0x0204D3A4,daAM2_Execute);
