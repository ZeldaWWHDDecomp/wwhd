/* WWHD pushable box. Ported from zeldaret/tww with HD changes verified against cking.rpx.
 */
#include "d/actor/d_a_obj_movebox.h"
namespace daObjMovebox {
void Act_c::prmX_init() {
    WWHD_FUNC(0x02376EE8,void,this);
    if(!mbPrmXInitialized) { s16 x=home.angle.x; mbPrmXInitialized=1; mPrmX=x; }
    home.angle.x=0; current.angle.x=0; shape_angle.x=0;
}
VERIFY(0x02376EE8,&Act_c::prmX_init);
void Act_c::prmZ_init() {
    WWHD_FUNC(0x02376F18,void,this);
    if(mbPrmZInitialized) return;
    mbPrmZInitialized=1; u16 z=0xFFFF; if(!prm(1,30)) z=home.angle.z;
    mPrmZ=z; home.angle.z=0; shape_angle.z=0; current.angle.z=0;
}
VERIFY(0x02376F18,&Act_c::prmZ_init);
s32 Act_c::prm_get_swSave1() {
    WWHD_FUNC(0x02376F90,s32,this); return prm(8,8);
}
VERIFY(0x02376F90,&Act_c::prm_get_swSave1);
bool Act_c::chk_appear() {
    WWHD_FUNC(0x02376F9C,bool,this);
    u8 path=pathId(); s32 sw=prm_get_swSave1(); if(path!=255 || sw==255) return true;
    bool set=dComIfGs_isSwitch(sw,home.roomNo)!=0; bool enabled=prm(1,30)!=0; return set==enabled;
}
VERIFY(0x02376F9C,&Act_c::chk_appear);
void Act_c::path_init() {
    WWHD_FUNC(0x02377078,void,this);
    s32 type=mType; u8 path=type==11 ? 255 : (mPrmZ&255); s32 sw=prm_get_swSave1();
    if(path==255 || sw==255) return;
    u8 sw2=type==11 ? 255 : (mPrmZ>>8);
    bool moved1=dComIfGs_isSwitch(sw,home.roomNo)!=0; bool moved2=false;
    if(sw2!=255) moved2=dComIfGs_isSwitch(swSave2(),home.roomNo)!=0;
    s32 idx=moved1 ? 1 : 0; if(moved2) idx+=2;
    u32 p=gabi::call<u32>(0x025AAF88,path,(s8)home.roomNo); mpPath=p;
    u32 point=gabi::call<u32>(0x025AAEB8,gabi::at<u8>(p),idx);
    home.pos.x=gabi::load<f32>(point+4); home.pos.y=gabi::load<f32>(point+8); home.pos.z=gabi::load<f32>(point+12);
    current.pos.x=gabi::load<f32>(point+4); current.pos.y=gabi::load<f32>(point+8); current.pos.z=gabi::load<f32>(point+12);
}
VERIFY(0x02377078,&Act_c::path_init);
BOOL Act_c::CreateHeap() {
    WWHD_FUNC(0x023771D8,BOOL,this);
    const Attr_c* a=attr(0x1002D278); if(a->mModelFileIndex<0) { mpModel=nullptr; return TRUE; }
    const char* name=arc(); s32 idx=attr(0x1002D278)->mModelFileIndex;
    auto* data=(J3DModelData*)dComIfG_getObjectRes(name,idx,0x1002CFAC);
    if(!data) JUT_ASSERT_fail(STR(0x1002D278),0x6C1,STR(0x1002D2B0));
    J3DModel* model=mDoExt_J3DModel__create(data,0x80000,0x11000022);
    BOOL success=model!=nullptr; mpModel=model; return success;
}
VERIFY(0x023771D8,&Act_c::CreateHeap);
void Act_c::clr_moment_cnt() {
    WWHD_FUNC(0x023772EC,void,this); for(s32 i=0;i<4;i++) mMomentCnt[i]=0;
}
VERIFY(0x023772EC,&Act_c::clr_moment_cnt);
void Act_c::mode_wait_init() {
    WWHD_FUNC(0x02377B84,void,this);
    speedF=0.0f; const Attr_c* a=attr(0x1002D394); dBgW* w=mpBgW;
    gravity=a->m14; gabi::store<u32>(gabi::ea(w)+0xA8,0x024EE76C);
    clr_moment_cnt(); m634=-1; mMode=0;
}
VERIFY(0x02377B84,&Act_c::mode_wait_init);
void Act_c::mode_afl_init() {
    WWHD_FUNC(0x0237845C,void,this); dBgW* w=mpBgW;
    speedF=0.0f; gabi::store<u32>(gabi::ea(w)+0xA8,0x024EE658); mMode=2;
}
VERIFY(0x0237845C,&Act_c::mode_afl_init);
void Act_c::sound_land() {
    WWHD_FUNC(0x02378484,void,this);
    u32 material=0; s32 idx=mBgc.mMaxGroundIdx;
    if(idx>=0) { u32 check=0x1046A7CC+idx*0x54;
        if(gabi::load<u16>(check+0x16)<0x100) {
            dBgS* bgs=dComIfG_Bgsp(); material=dBgS_GetMtrlSndId(bgs,gabi::at<cBgS_PolyInfo>(check+0x14));
        }
    }
    const Attr_c* a=attr(0x1002D41C); s8 reverb=mReverb; u32 sound=a->mNormalFallSE;
    mDoAud_seStart(sound,&eyePos,material,reverb);
}
VERIFY(0x02378484,&Act_c::sound_land);
void Act_c::vib_land() {
    WWHD_FUNC(0x02378544,void,this); u32 play=dComIfGp_ea();
    gabi::Local<cXyz> v; v->x=0.0f; v->y=1.0f; v->z=0.0f;
    gabi::call(0x025CB374,gabi::at<u8>(play+0x599C),4,-33,v.get());
}
VERIFY(0x02378544,&Act_c::vib_land);
void Act_c::eff_land_smoke() {
    WWHD_FUNC(0x02378594,void,this);
    f32 size=attr(0x1002D458)->mLandSmokeScale; gabi::Local<cXyz> v; v->set(size,size,size);
    gabi::call(0x028E8E64,v.get(),v.get(),1.6666666269302368f);
    gabi::call(0x025D5834,0x1D3,3,&current.pos,-1,(csXyz*)nullptr,v.get(),-1,0);
}
VERIFY(0x02378594,&Act_c::eff_land_smoke);
void Act_c::eff_smoke_slip_remove() {
    WWHD_FUNC(0x02378A4C,void,this);
    for(s32 i=0;i<2;i++) { EffSmokeCB* cb=&mSmokeCbs[i]; u32 vt=gabi::load<u32>(gabi::ea(cb)); gabi::call(gabi::load<u32>(vt+0x44),cb); }
}
VERIFY(0x02378A4C,&Act_c::eff_smoke_slip_remove);
BOOL Act_c::Delete() {
    WWHD_FUNC(0x02378A9C,BOOL,this); eff_smoke_slip_remove();
    if(mpBgW!=nullptr) { gabi::store<u32>(gabi::ea(mpBgW)+0xB0,0); gabi::store<u32>(gabi::ea(mpBgW)+0xB4,0); }
    return TRUE;
}
VERIFY(0x02378A9C,&Act_c::Delete);
void Act_c::mode_walk_init() {
    WWHD_FUNC(0x023792E8,void,this); speedF=0.0f; m64F=0; mMode=1;
}
VERIFY(0x023792E8,&Act_c::mode_walk_init);
void Act_c::sound_slip() {
    WWHD_FUNC(0x0237966C,void,this);
    u32 material=0; s32 idx=mBgc.mMaxGroundIdx;
    if(idx>=0) { u32 check=0x1046A7CC+idx*0x54; if(gabi::load<u16>(check+0x16)<0x100) {
        dBgS* bgs=dComIfG_Bgsp(); material=dBgS_GetMtrlSndId(bgs,gabi::at<cBgS_PolyInfo>(check+0x14));
    } }
    const Attr_c* a=attr(0x1002D5F8); s8 reverb=mReverb; u32 sound=a->mMoveSE;
    mDoAud_seStart(sound,&eyePos,material,reverb);
}
VERIFY(0x0237966C,&Act_c::sound_slip);
void Act_c::sound_limit() {
    WWHD_FUNC(0x0237972C,void,this);
    u32 material=0; s32 idx=mBgc.mWallIdx;
    if(idx>=0) { u32 check=0x1046AF58+idx*0x6C; if(gabi::load<u16>(check+0x16)<0x100) {
        dBgS* bgs=dComIfG_Bgsp(); material=dBgS_GetMtrlSndId(bgs,gabi::at<cBgS_PolyInfo>(check+0x14));
    } }
    const Attr_c* a=attr(0x1002D630); s8 reverb=mReverb; u32 sound=a->mCantMoveSE;
    mDoAud_seStart(sound,&eyePos,material,reverb);
}
VERIFY(0x0237972C,&Act_c::sound_limit);
void Act_c::eff_smoke_slip_end() {
    WWHD_FUNC(0x023797EC,void,this);
    for(s32 i=0;i<2;i++) gabi::call(0x025A5F88,&mSmokeCbs[i]);
}
VERIFY(0x023797EC,&Act_c::eff_smoke_slip_end);
void Act_c::make_item() {
    WWHD_FUNC(0x02378150,void,this);
    s32 item=prm(6,0); s32 bit=prm(7,16);
    gabi::call(0x025D8120,&current.pos,item,bit,(s8)home.roomNo,0,&current.angle,7,0);
}
VERIFY(0x02378150,&Act_c::make_item);
void Act_c::eff_break() {
    WWHD_FUNC(0x023781C8,void,this);
    if(!gabi::load<u32>(0x1046A76C)) {
        gabi::at<cXyz>(0x1046A6F0)->set(2.0f,2.0f,1.0f); gabi::store<u32>(0x1046A76C,1);
    }
    gabi::Local<cXyz> pos; pos->set(current.pos.x,current.pos.y+75.0f,current.pos.z);
    u32 play=dComIfGp_ea();
    u32 emitter=gabi::call<u32>(0x025A847C,gabi::at<u8>(gabi::load<u32>(play+0x5AB0)),0,0x3E6,pos.get(),0,0,255,0,-1,gabi::at<u8>(gabi::ea(this)+0x1A8),gabi::at<u8>(gabi::ea(this)+0x1A8),gabi::at<cXyz>(0x1046A6F0));
    if(emitter) { gabi::store<u16>(emitter+0x60,30); gabi::store<f32>(emitter+0x6C,30.0f); }
    gabi::call(0x025D5834,0x1D3,5,pos.get(),-1,0,0,-1,0);
}
VERIFY(0x023781C8,&Act_c::eff_break);
void Act_c::sound_break() {
    WWHD_FUNC(0x023782D8,void,this);
    f32 x=current.pos.x,y=current.pos.y+100.0f,z=current.pos.z;
    gabi::Local<u8[0x54]> check; u32 c=gabi::ea(check.get());
    dBgS_GndChk_ct(check.get(),{0x1002D034,0x1002D044,0x1002D064,0x1002D054},true);
    gabi::at<cXyz>(c+0x24)->set(x,y,z); gabi::store<u32>(c+8,gabi::load<u32>(gabi::ea(this)+4));
    gabi::call<f32>(0x02008974,dComIfG_Bgsp(),check.get());
    u32 material=0; if(gabi::load<u16>(c+0x16)<0x100) material=dBgS_GetMtrlSndId(dComIfG_Bgsp(),gabi::at<cBgS_PolyInfo>(c+0x14));
    s8 reverb=dComIfGp_getReverb(current.roomNo); mDoAud_seStart(0x6805,&eyePos,material,reverb);
    gabi::store<u32>(c+0x20,0x1002D004); gabi::store<u32>(c+0x40,0x1002D024); gabi::store<u32>(c+0x4C,0x1002CFE4);
    gabi::call(0x02008DAC,check.get(),0);
}
VERIFY(0x023782D8,&Act_c::sound_break);
BOOL Act_c::Draw() {
    WWHD_FUNC(0x02378970,BOOL,this);
    if(mpModel!=nullptr) {
        u8 tevType=attr(0x1002D4C8)->mbUseBGTevType;
        u32 light=gabi::call<u32>(0x02555D0C);
        gabi::call(0x025626A4,gabi::at<u8>(light),tevType,&current.pos,gabi::at<u8>(gabi::ea(this)+0x110));
        light=gabi::call<u32>(0x02555D0C);
        gabi::call(0x02562F5C,gabi::at<u8>(light),mpModel.get(),gabi::at<u8>(gabi::ea(this)+0x110));
        u32 play=dComIfGp_ea(); gabi::store<u32>(0x104B4634,gabi::load<u32>(play+0x5D70));
        play=dComIfGp_ea(); gabi::store<u32>(0x104B4638,gabi::load<u32>(play+0x5D74));
        gabi::call(0x025E2DE0,mpModel.get(),0);
        play=dComIfGp_ea(); gabi::store<u32>(0x104B4634,gabi::load<u32>(play+0x5D78));
        play=dComIfGp_ea(); gabi::store<u32>(0x104B4638,gabi::load<u32>(play+0x5D7C));
    }
    return TRUE;
}
VERIFY(0x02378970,&Act_c::Draw);

void Bgc_c::wrt_pos(const cXyz* pos) {
    WWHD_FUNC(0x02376518,void,this,pos);
    u32 w=0x1046A77C; f32 y=pos->y,z=pos->z,x=pos->x;
    gabi::at<cXyz>(w+0x38)->set(x,y-100.0f,z); gabi::store<f32>(w+0x44,y+400.0f);
    if(gabi::call<BOOL>(0x024EF7C0,dComIfG_Bgsp(),gabi::at<u8>(w))) {
        u32 flags=mStateFlags; f32 height=gabi::load<f32>(w+0x48); flags|=8;
        mWaterY=height; mStateFlags=flags;
        if(height>pos->y) mStateFlags=flags|0x10;
    }
}
VERIFY(0x02376518,&Bgc_c::wrt_pos);
void Act_c::init_mtx() {
    WWHD_FUNC(0x02377B5C,void,this);
    if(mpModel!=nullptr) {
        f32 y=scale.y,z=scale.z,x=scale.x;
        gabi::store<f32>(gabi::ea(mpModel.get())+0xC4,z);
        gabi::store<f32>(gabi::ea(mpModel.get())+0xC0,y);
        gabi::store<f32>(gabi::ea(mpModel.get())+0xBC,x);
    }
    set_mtx();
}
VERIFY(0x02377B5C,&Act_c::init_mtx);
void Act_c::path_save() {
    WWHD_FUNC(0x02378AE8,void,this);
    s32 type=mType; u8 path=type==11 ? 255 : (mPrmZ&255); s32 sw=prm_get_swSave1();
    if(path==255 || sw==255) return;
    u8 second=type==11 ? 255 : (mPrmZ>>8); u32 p=mpPath;
    s32 count=second==255 ? 2 : 4; s32 num=gabi::load<u16>(p); if(count>num) count=num;
    for(s32 index=0;index<count;index++) {
        u32 point=gabi::call<u32>(0x025AAEB8,gabi::at<u8>(p),index);
        gabi::Local<cXyz> pos; pos->set(gabi::load<f32>(point+4),gabi::load<f32>(point+8),gabi::load<f32>(point+12));
        f32 dist=gabi::call<f32>(0x028E8DE8,&current.pos,pos.get());
        if(dist<9.0f) {
            s32 first=prm_get_swSave1();
            if(index&1) dComIfGs_onSwitch(first,home.roomNo); else dComIfGs_offSwitch(first,home.roomNo);
            if(second!=255) {
                if(index&2) dComIfGs_onSwitch(swSave2(),home.roomNo); else dComIfGs_offSwitch(swSave2(),home.roomNo);
            }
            return;
        }
        p=mpPath;
    }
}
VERIFY(0x02378AE8,&Act_c::path_save);

bool Bgc_c::chk_wall_pre(const Act_c* actor,const BgcSrc_c* src,s32 count,s16 angle) {
    WWHD_FUNC(0x02376B14,bool,this,actor,src,count,angle);
    gabi::call(0x023765D0,this,actor,src,count,angle,74.0f);
    return mWallIdx>=0;
}
VERIFY(0x02376B14,&Bgc_c::chk_wall_pre);
bool Bgc_c::chk_wall_touch2(const Act_c* actor,const BgcSrc_c* src,s32 count,s16 angle) {
    WWHD_FUNC(0x02376E74,bool,this,actor,src,count,angle);
    for(s32 i=0;i<count;i++) if(gabi::call<bool>(0x02376B54,this,actor,src+i,angle)) return true;
    return false;
}
VERIFY(0x02376E74,&Bgc_c::chk_wall_touch2);

void Bgc_c::proc_vertical(Act_c* actor) {
    WWHD_FUNC(0x02376914,void,this,actor);
    u32 previous=mStateFlags; mStateFlags=0;
    u32 src=actor->attr(0x1002D154,0x1002D188)->m9A ? 0x101CB2B4 : 0x101CB264;
    s32 count=actor->attr(0x1002D154,0x1002D188)->m9A ? 21 : 5;
    gabi::call(0x02376314,this,actor,gabi::at<BgcSrc_c>(src),count,100.0f);
    s32 idx=mMaxGroundIdx;
    if(idx>=0 && mGroundY[idx]>actor->current.pos.y) {
        actor->current.pos.y=mGroundY[idx]; mStateFlags=mStateFlags|1; actor->speed.y=0.0f;
    }
    gabi::call(0x02376518,this,&actor->current.pos);
    u32 flags=mStateFlags;
    if((flags&8) && mWaterY>actor->current.pos.y) { flags|=16; mStateFlags=flags; }
    if(!(flags&1) && (previous&1)) {
        mStateFlags=flags|2; actor->speed.y=actor->attr(0x1002D154,0x1002D188)->m20;
        flags=mStateFlags;
    }
    if((flags&1) && !(previous&1)) { flags|=4; mStateFlags=flags; }
    if((flags&16) && !(previous&16)) { flags|=32; mStateFlags=flags; }
    if((flags&1) && mMaxGroundIdx>=0) {
        dBgS* bg=dComIfG_Bgsp(); u32 check=0x1046A7CC+(s32)mMaxGroundIdx*0x54;
        gabi::call(0x024EFBC0,bg,gabi::at<u8>(check ? check+0x14 : 0),actor);
    }
}
VERIFY(0x02376914,&Bgc_c::proc_vertical);

fopAc_ac_c* Act_c::PPCallBack(fopAc_ac_c* target,fopAc_ac_c* unused,s16 angle,u32 label) {
    WWHD_FUNC(0x023775DC,fopAc_ac_c*,target,unused,angle,label);
    Act_c* box=(Act_c*)target; u32 push=label&3;
    bool eligible=box->attr(0x1002D300)->m9A ? ((label>>3)&1)!=0 : true;
    if(push && eligible) {
        s32 facing=angle; if(push&2) facing-=0x8000;
        facing=(s16)(facing-(s16)box->home.angle.y);
        if(push==3) JUT_ASSERT_fail(STR(0x1002D300),0x71C,STR(0x1002D338));
        box->mPPLabel=label; u32 direction;
        if((u32)(facing+0x2000)<0x4000) direction=0;
        else if((u32)(facing-0x2000)<0x4000) direction=1;
        else direction=(u32)(facing+0x6000)<0xC000 ? 3 : 2;
        for(u32 j=0;j<4;j++) box->mMomentCnt[j]=j==direction ? (s16)(box->mMomentCnt[j]+1) : 0;
        box->m64A=1;
    }
    return target;
}
VERIFY(0x023775DC,Act_c::PPCallBack);

void Bgc_c::gnd_pos(const Act_c* actor,const BgcSrc_c* src,s32 count,f32 height) {
    WWHD_FUNC(0x02376314,void,this,actor,src,count,height);
    f32 max=-1000000000.0f;
    gabi::call(0x028E93CC,gabi::at<Mtx34>(0x1048D0CC),(f32)actor->current.pos.x,(f32)actor->current.pos.y,(f32)actor->current.pos.z);
    gabi::call(0x025F1C28,gabi::at<Mtx34>(0x1048D0CC),(s16)actor->home.angle.y);
    mMaxGroundIdx=-1;
    for(s32 i=0;i<count;i++,src++) {
        f32 width=actor->attr(0x1002D154,0x1002D188)->mScaleXZ;
        f32 x=gabi::fmadds(src->m04,width,src->m0C);
        width=actor->attr(0x1002D154,0x1002D188)->mScaleXZ;
        f32 z=gabi::fmadds(src->m00,width,src->m08);
        gabi::Local<cXyz> offset,pos; offset->set(x,height,z);
        gabi::call(0x028E8F64,gabi::at<Mtx34>(0x1048D0CC),offset.get(),pos.get());
        u32 check=0x1046A7CC+i*0x54;
        gabi::at<cXyz>(check+0x24)->copy(*pos.get());
        gabi::store<u32>(check+8,gabi::load<u32>(gabi::ea(actor)+4));
        f32 ground=gabi::call<f32>(0x02008974,dComIfG_Bgsp(),gabi::at<u8>(check)); mGroundY[i]=ground;
        if(ground>max) {
            dBgS* bg=dComIfG_Bgsp(); u32 other=gabi::call<u32>(0x02008438,bg,gabi::load<u16>(check+0x16));
            if(!other || gabi::load<s16>(other+8)!=0x2B || gabi::load<s32>(other+0x41C)!=2) {
                max=mGroundY[i]; mMaxGroundIdx=i;
            }
        }
    }
}
VERIFY(0x02376314,&Bgc_c::gnd_pos);

void Act_c::eff_set_slip_smoke_pos() {
    WWHD_FUNC(0x02379010,void,this);
    if(!gabi::load<u32>(0x1046A770)) {
        gabi::store<u32>(0x1046A770,1);
        gabi::at<cXyz>(0x1046A68C)->set(-0.5f,0.0f,-0.5f);
        gabi::at<cXyz>(0x1046A698)->set(0.5f,0.0f,-0.5f);
    }
    s16 angle=(s16)((s16)home.angle.y+gabi::load<s16>(0x1004BCE8+2*(s32)m634));
    f32 scale=attr(0x1002D540)->mScaleXZ;
    gabi::call(0x028E93CC,gabi::at<Mtx34>(0x1048D0CC),(f32)current.pos.x,(f32)current.pos.y,(f32)current.pos.z);
    gabi::call(0x025F1C28,gabi::at<Mtx34>(0x1048D0CC),angle);
    gabi::call(0x025F24E0,0.0f,0.0f,10.0f);
    gabi::call(0x025F2518,scale,scale,scale);
    for(s32 i=0;i<2;i++) {
        gabi::call(0x028E8F64,gabi::at<Mtx34>(0x1048D0CC),gabi::at<cXyz>(0x1046A68C+i*12),&mSmokeCbs[i].field_0x20);
        mSmokeCbs[i].field_0x2C.x=0; mSmokeCbs[i].field_0x2C.y=angle; mSmokeCbs[i].field_0x2C.z=0;
    }
}
VERIFY(0x02379010,&Act_c::eff_set_slip_smoke_pos);
void Act_c::eff_smoke_slip_start() {
    WWHD_FUNC(0x0237918C,void,this);
    if(!gabi::load<u32>(0x1046A774)) {
        gabi::store<u32>(0x1046A774,1); gabi::at<cXyz>(0x1046A6E4)->set(0.6000000238418579f,0.6000000238418579f,0.6000000238418579f);
    }
    attr(0x1002D584); eff_set_slip_smoke_pos();
    for(s32 i=0;i<2;i++) {
        EffSmokeCB* cb=&mSmokeCbs[i]; s8 room=current.roomNo;
        u32 play=dComIfGp_ea();
        u32 emitter=gabi::call<u32>(0x025A847C,gabi::at<u8>(gabi::load<u32>(play+0x5AB0)),2,0x2022,&cb->field_0x20,&cb->field_0x2C,gabi::at<cXyz>(0x1046A6E4),0xB9,cb,room,0,0,0);
        if(emitter) {
            gabi::store<f32>(emitter+0x70,15.0f); gabi::store<f32>(emitter+0x58,0.15000000596046448f);
            gabi::store<f32>(emitter+0x34,2.0f); gabi::store<u16>(emitter+0x60,30);
        }
    }
}
VERIFY(0x0237918C,&Act_c::eff_smoke_slip_start);

void Act_c::RideCallBack(dBgW* bg,fopAc_ac_c* target,fopAc_ac_c* rider) {
    WWHD_FUNC(0x02377308,void,bg,target,rider);
    Act_c* box=(Act_c*)target; if(box->mMode!=2) return;
    const Attr_c* a=box->attr(0x1002D2C8); const Attr_c* b=box->attr(0x1002D2C8);
    f32 limit=(f32)a->m2C+(f32)b->m30;
    f32 dx=(f32)rider->current.pos.x-(f32)box->current.pos.x;
    f32 dz=(f32)rider->current.pos.z-(f32)box->current.pos.z;
    f32 weight,moment; const Attr_c* reciprocal;
    if(rider!=nullptr && gabi::load<s16>(gabi::ea(rider)+0xE)==0xA8) {
        weight=box->attr(0x1002D2C8)->m2C;
        const Attr_c* size=box->attr(0x1002D2C8); const Attr_c* force=box->attr(0x1002D2C8);
        moment=(f32)size->m74*(f32)force->m44; reciprocal=box->attr(0x1002D2C8);
    } else {
        weight=box->attr(0x1002D2C8)->m30;
        const Attr_c* size=box->attr(0x1002D2C8); const Attr_c* force=box->attr(0x1002D2C8);
        moment=(f32)size->m74*(f32)force->m48; reciprocal=box->attr(0x1002D2C8);
    }
    f32 distance=gabi::call<f32>(0x028F4384,gabi::fmadds(dx,dx,dz*dz));
    f32 ratio=gabi::fnmsubs(distance,reciprocal->m74,1.0f);
    ratio=gabi::fmadds(ratio,0.8999999761581421f,0.10000000149011612f);
    ratio=ratio>=0.0f ? ratio : 0.0f;
    f32 down=gabi::fmadds(weight,ratio,box->m608);
    f32 x=box->m60C,z=box->m610; if(down<limit) down=limit;
    box->m608=down; box->m60C=gabi::fmadds(moment,dx,x); box->m610=gabi::fmadds(moment,dz,z);
}
VERIFY(0x02377308,Act_c::RideCallBack);

void Bgc_c::wall_pos(const Act_c* actor,const BgcSrc_c* src,s32 count,s16 angle,f32 distance) {
    WWHD_FUNC(0x023765D0,void,this,actor,src,count,angle,distance);
    angle=(s16)(angle+(s16)actor->home.angle.y); mWallIdx=-1; mNearestWallDist=3.4028234663852886e38f;
    auto* matrix=gabi::at<Mtx34>(0x1048D0CC);
    gabi::Local<cXyz> dir,offset,rotated,start,finish,tmp;
    gabi::call(0x025F1884,matrix,angle); gabi::call(0x025F1BF4,matrix,0x4000);
    gabi::call(0x028E8F64,matrix,gabi::at<cXyz>(0x101FFBC0),dir.get());
    f32 size=actor->attr(0x1002D154,0x1002D188)->mScaleXZ;
    gabi::call(0x028E8E64,dir.get(),dir.get(),gabi::fmadds(size,0.5f,distance));
    for(s32 i=0;i<count;i++,src++) {
        gabi::call(0x025F18EC,matrix,0x4000);
        offset->set(src->m0C,0.0f,src->m08); gabi::call(0x028E8F64,matrix,offset.get(),rotated.get());
        gabi::call(0x025F1884,matrix,angle);
        gabi::call(0x025F24E0,(f32)rotated->x,(f32)rotated->y,(f32)rotated->z);
        const Attr_c* a=actor->attr(0x1002D154,0x1002D188);
        const Attr_c* b=actor->attr(0x1002D154,0x1002D188);
        const Attr_c* c=actor->attr(0x1002D154,0x1002D188);
        gabi::call(0x025F2518,(f32)a->mScaleXZ,(f32)b->mScaleY,(f32)c->mScaleXZ);
        gabi::call(0x025F24E0,0.0f,0.5f,0.0f); gabi::call(0x025F1BF4,matrix,0x4000);
        offset->set(src->m04,0.0f,src->m00); gabi::call(0x028E8F64,matrix,offset.get(),start.get());
        gabi::call(0x028E8D88,start.get(),&actor->current.pos,start.get());
        gabi::call(0x0201AD78,start.get(),tmp.get(),dir.get()); finish->copy(*tmp.get());
        u32 check=0x1046AF58+i*0x6C;
        gabi::call(0x024F1AFC,gabi::at<u8>(check),start.get(),finish.get(),actor);
        gabi::store<u32>(check+8,gabi::load<u32>(gabi::ea(actor)+4));
        if(gabi::call<BOOL>(0x02008860,dComIfG_Bgsp(),gabi::at<u8>(check))) {
            mWallPos[i].copy(*gabi::at<cXyz>(check+0x30));
            f32 dist=gabi::call<f32>(0x028E8DE8,start.get(),&mWallPos[i]);
            if(dist<mNearestWallDist) { mNearestWallDist=dist; mWallIdx=i; }
        } else mWallPos[i].copy(*gabi::at<cXyz>(0x101FFBA8));
    }
}
VERIFY(0x023765D0,&Bgc_c::wall_pos);

bool Bgc_c::chk_wall_touch(const Act_c* actor,const BgcSrc_c* src,s16 angle) {
    WWHD_FUNC(0x02376B54,bool,this,actor,src,angle);
    u32 check=0x1046A6FC;
    if(!gabi::load<u32>(0x1046A768)) {
        gabi::store<u32>(0x1046A768,1);
        dBgS_LinChk_ct(gabi::at<u8>(check),{0x1002D114,0x1002D124,0x1002D144,0x1002D134},true);
        gabi::call(0x028F026C,gabi::at<u8>(0x101CB1C0));
    }
    angle=(s16)(angle+(s16)actor->home.angle.y); auto* matrix=gabi::at<Mtx34>(0x1048D0CC);
    gabi::Local<cXyz> dir,offset,rotated,start,finish,tmp;
    gabi::call(0x025F1884,matrix,angle); gabi::call(0x025F1BF4,matrix,0x4000);
    offset->set(src->m0C,0.0f,src->m08);
    gabi::call(0x028E8F64,matrix,gabi::at<cXyz>(0x101FFBC0),dir.get());
    f32 size=actor->attr(0x1002D154,0x1002D188)->mScaleXZ;
    gabi::call(0x028E8E64,dir.get(),dir.get(),gabi::fmadds(size,0.5f,10.0f));
    gabi::call(0x025F1BF4,matrix,0x4000);
    offset->set(src->m0C,0.0f,src->m08); gabi::call(0x028E8F64,matrix,offset.get(),rotated.get());
    gabi::call(0x025F1884,matrix,angle);
    gabi::call(0x025F24E0,(f32)rotated->x,(f32)rotated->y,(f32)rotated->z);
    const Attr_c* a=actor->attr(0x1002D154,0x1002D188);
    const Attr_c* b=actor->attr(0x1002D154,0x1002D188);
    const Attr_c* c=actor->attr(0x1002D154,0x1002D188);
    gabi::call(0x025F2518,(f32)a->mScaleXZ,(f32)b->mScaleY,(f32)c->mScaleXZ);
    gabi::call(0x025F24E0,0.0f,0.5f,0.0f); gabi::call(0x025F1BF4,matrix,0x4000);
    offset->set(src->m04,0.0f,src->m00); gabi::call(0x028E8F64,matrix,offset.get(),start.get());
    gabi::call(0x028E8D88,start.get(),&actor->current.pos,start.get());
    gabi::call(0x0201AD78,start.get(),tmp.get(),dir.get());
    gabi::store<u32>(check+8,gabi::load<u32>(gabi::ea(actor)+4)); finish->copy(*tmp.get());
    gabi::call(0x024F1AFC,gabi::at<u8>(check),start.get(),finish.get(),actor);
    return gabi::call<bool>(0x02008860,dComIfG_Bgsp(),gabi::at<u8>(check));
}
VERIFY(0x02376B54,&Bgc_c::chk_wall_touch);

s32 Act_c::check_to_walk() {
    WWHD_FUNC(0x02378D18,s32,this);
    s32 result=-1; bool clear=true;
    if(m64A && (mBgc.mStateFlags&1) && !(mType==11 && mChildPID!=0xFFFFFFFF)) {
        u32 label=mPPLabel; const Attr_c* a=attr(0x1002D504);
        s16 threshold=(label&2) ? ((label&4) ? (s16)a->m06 : (s16)a->m08) : ((label&4) ? (s16)a->m00 : (s16)a->m02);
        for(s32 i=0;i<4;i++) {
            if(mMomentCnt[i]<threshold) clear=false;
            else {
                u32 src=attr(0x1002D504)->m9A ? 0x101CB2B4 : 0x101CB264;
                bool extended=attr(0x1002D504)->m9A!=0;
                s32 count=extended ? ((s32)mType==10 ? 23 : 21) : 5;
                s16 angle=gabi::load<s16>(0x1004BCE8+i*2);
                if(!gabi::call<bool>(0x02376B14,&mBgc,this,gabi::at<BgcSrc_c>(src),count,angle)) result=i;
            }
        }
    }
    if(clear) clr_moment_cnt(); m64A=0; return result;
}
VERIFY(0x02378D18,&Act_c::check_to_walk);

void Act_c::mode_wait() {
    WWHD_FUNC(0x02379308,void,this);
    if(m646>0) m646=m646-1;
    s32 direction=check_to_walk();
    if(!m64F && (mBgc.mStateFlags&1)) { m64F=1; path_save(); }
    const Attr_c* a=attr(0x1002D5C0); const Attr_c* b=attr(0x1002D5C0);
    gabi::call(0x023123C0,this,0,gabi::at<cXyz>(0x101FFBA8),(f32)a->m18,(f32)b->m1C);
    auto* matrix=gabi::at<Mtx34>(0x1048D0CC);
    gabi::call(0x028E93CC,matrix,(f32)home.pos.x,(f32)home.pos.y,(f32)home.pos.z);
    gabi::call(0x025F1C28,matrix,(s16)home.angle.y);
    a=attr(0x1002D5C0); b=attr(0x1002D5C0);
    f32 x=(f32)(s32)m628*(f32)a->m0C,z=(f32)(s32)m62C*(f32)b->m0C;
    gabi::call(0x025F24E0,x,0.0f,z);
    gabi::Local<cXyz> pos; gabi::call(0x028E8F64,matrix,gabi::at<cXyz>(0x101FFBA8),pos.get());
    current.pos.x=pos->x; current.pos.z=pos->z;
    if(direction!=-1) {
        m634=direction; eff_smoke_slip_start();
        u32 player=gabi::load<u32>(dComIfGp_ea()+0x5B2C);
        gabi::store<u32>(player+0x3B8,gabi::load<u32>(player+0x3B8)|0x800);
        mode_walk_init();
        if(mPPLabel&2) {
            m644=attr(0x1002D5C0)->m0A;
            m630=32768.0f/(f32)(s16)attr(0x1002D5C0)->m0A;
        } else {
            m644=attr(0x1002D5C0)->m04;
            m630=32768.0f/(f32)(s16)attr(0x1002D5C0)->m04;
        }
    }
}
VERIFY(0x02379308,&Act_c::mode_wait);

void Act_c::afl_sway() {
    WWHD_FUNC(0x02379D00,void,this);
    f32 z=m610,x=m60C,mag=gabi::fmadds(x,x,z*z);
    const Attr_c* a=attr(0x1002D6A4); const Attr_c* b=attr(0x1002D6A4);
    f32 max=(f32)a->m4C*(f32)b->m4C;
    u32 src=attr(0x1002D6A4)->m9A ? 0x101CB2B4 : 0x101CB264;
    s32 count=attr(0x1002D6A4)->m9A ? 21 : 5;
    bool wallZ=gabi::call<bool>(0x02376E74,&mBgc,this,gabi::at<BgcSrc_c>(src),count,gabi::load<s16>(0x1004BCE8));
    if(!wallZ) wallZ=gabi::call<bool>(0x02376E74,&mBgc,this,gabi::at<BgcSrc_c>(src),count,gabi::load<s16>(0x1004BCEC));
    bool wallX=gabi::call<bool>(0x02376E74,&mBgc,this,gabi::at<BgcSrc_c>(src),count,gabi::load<s16>(0x1004BCEA));
    if(!wallX) wallX=gabi::call<bool>(0x02376E74,&mBgc,this,gabi::at<BgcSrc_c>(src),count,gabi::load<s16>(0x1004BCEE));
    f32 dx,dz;
    if(mag>max) {
        a=attr(0x1002D6A4); f32 distance=gabi::call<f32>(0x028F4384,mag);
        f32 ratio=(f32)a->m4C/distance;
        x=(f32)m60C*ratio; z=(f32)m610*ratio;
        f32 oldX=m614; m60C=x; f32 oldZ=m618; m610=z;
        dx=oldX-x; dz=oldZ-z;
        a=attr(0x1002D6A4);
    } else { dx=(f32)m614-(f32)m60C; dz=(f32)m618-(f32)m610; a=attr(0x1002D6A4); }
    f32 forceX=-(dx*(f32)a->m50);
    f32 forceZ=-(dz*(f32)attr(0x1002D6A4)->m50);
    a=attr(0x1002D6A4); f32 velocityX=m61C;
    f32 dampX=-(velocityX*(f32)a->m54);
    bool reload=(u32)mType>=13; a=attr(0x1002D6A4);
    if(reload) velocityX=m61C; f32 velocityZ=m620;
    f32 sumX=forceX+dampX;
    f32 sumZ=gabi::fnmsubs(velocityZ,a->m54,forceZ);
    velocityX=velocityX+sumX; velocityZ=velocityZ+sumZ;
    f32 oldX=m614,oldZ=m618;
    m61C=velocityX; m620=velocityZ; m614=oldX+velocityX; m618=oldZ+velocityZ;
    if(wallX) m614=0.0f; if(wallZ) m618=0.0f; m60C=0.0f; m610=0.0f;
}
VERIFY(0x02379D00,&Act_c::afl_sway);

void Act_c::mode_afl() {
    WWHD_FUNC(0x0237A0A8,void,this);
    f32 immersion=(f32)current.pos.y-(f32)mBgc.mWaterY;
    const Attr_c* a;
    if(immersion<0.0f) {
        a=attr(0x1002D6DC);
        if(!(immersion>-(f32)a->m68)) immersion=1.0f;
        else immersion=-(immersion*(f32)attr(0x1002D6DC)->m6C);
    } else immersion=0.0f;
    a=attr(0x1002D6DC); f32 period=(f32)(s16)a->m38;
    f32 random=gabi::call<f32>(0x02019788);
    m604=(s16)(m604+(s16)gabi::ftoi(period*(random+1.0f)));
    const Attr_c* buoy=attr(0x1002D6DC); const Attr_c* grav=attr(0x1002D6DC); const Attr_c* wave=attr(0x1002D6DC);
    u16 phase=m604; f32 g=grav->m14,b=buoy->m28,w=wave->m34;
    f32 sin=gabi::load<f32>(0x104A44F8+8*(phase>>3));
    f32 base=gabi::fmadds(immersion,b,g); f32 force=gabi::fmadds(w,sin,base);
    f32 down=m608; m608=0.0f; gravity=force+down;
    afl_sway(); f32 air=1.0f-immersion;
    a=attr(0x1002D6DC); const Attr_c* friction=attr(0x1002D6DC);
    f32 drag=gabi::fmadds(a->m3C,immersion,(f32)friction->m18*air);
    a=attr(0x1002D6DC); friction=attr(0x1002D6DC);
    f32 resistance=gabi::fmadds(a->m40,immersion,(f32)friction->m1C*air);
    f32 depth=(f32)mBgc.mWaterY-(f32)current.pos.y;
    if(depth<0.0f) m624=0.0f;
    else {
        m624=depth; bool reload=(u32)mType>=13; a=attr(0x1002D6DC); if(reload) depth=m624;
        if(depth>a->m68) m624=attr(0x1002D6DC)->m68;
    }
    gabi::call(0x023123C0,this,0,gabi::at<cXyz>(0x101FFBA8),drag,resistance);
}
VERIFY(0x0237A0A8,&Act_c::mode_afl);

void Act_c::mode_walk() {
    WWHD_FUNC(0x02379834,void,this);
    if(!gabi::load<u32>(0x1046A778)) {
        const f32 values[12]={0,0,1,1,0,0,0,0,-1,-1,0,0};
        for(s32 i=0;i<12;i++) gabi::store<f32>(0x1046A6A4+i*4,values[i]);
        gabi::store<u32>(0x1046A778,1);
    }
    s16 counter=(s16)(m644-1); u16 phase=(u16)gabi::ftoi((f32)counter*(f32)m630);
    m644=counter; f32 blend=(gabi::load<f32>(0x104A44FC+8*(phase>>3))+1.0f)*0.5f;
    bool finished=counter<=0; auto* matrix=gabi::at<Mtx34>(0x1048D0CC);
    gabi::call(0x028E93CC,matrix,(f32)home.pos.x,(f32)home.pos.y,(f32)home.pos.z);
    gabi::call(0x025F1C28,matrix,(s16)home.angle.y);
    const Attr_c* a=attr(0x1002D66C); const Attr_c* b=attr(0x1002D66C);
    u32 vec=0x1046A6A4+12*(s32)m634;
    f32 x=gabi::fmadds(gabi::load<f32>(vec),blend,(f32)(s32)m628);
    f32 z=gabi::fmadds(gabi::load<f32>(vec+8),blend,(f32)(s32)m62C);
    gabi::call(0x025F24E0,x*(f32)a->m0C,0.0f,z*(f32)b->m0C);
    gabi::Local<cXyz> pos; gabi::call(0x028E8F64,matrix,gabi::at<cXyz>(0x101FFBA8),pos.get());
    x=pos->x; z=pos->z; f32 y=current.pos.y;
    current.pos.x=x; eyePos.y=y; eyePos.z=z; eyePos.x=x; current.pos.z=z;
    sound_slip();
    if(finished) {
        u32 src=attr(0x1002D66C)->m9A ? 0x101CB2B4 : 0x101CB264;
        bool extended=attr(0x1002D66C)->m9A!=0;
        s32 count=extended ? ((s32)mType==10 ? 23 : 21) : 5;
        s16 angle=gabi::load<s16>(0x1004BCE8+2*(s32)m634);
        if(gabi::call<bool>(0x02376B14,&mBgc,this,gabi::at<BgcSrc_c>(src),count,angle)) sound_limit();
        eff_smoke_slip_end();
    } else eff_set_slip_smoke_pos();
    a=attr(0x1002D66C); b=attr(0x1002D66C);
    gabi::call(0x023123C0,this,0,gabi::at<cXyz>(0x101FFBA8),(f32)a->m18,(f32)b->m1C);
    current.pos.x=pos->x; current.pos.z=pos->z;
    if(finished) {
        s32 direction=m634;
        if(direction==0) m62C=m62C+1; else if(direction==1) m628=m628+1;
        else if(direction==2) m62C=m62C-1; else if(direction==3) m628=m628-1;
        u32 player=gabi::load<u32>(dComIfGp_ea()+0x5B2C);
        gabi::store<u32>(player+0x3B8,gabi::load<u32>(player+0x3B8)&~0x800u);
        mode_wait_init();
    }
}
VERIFY(0x02379834,&Act_c::mode_walk);

BOOL Act_c::Execute(gptr<Mtx34>* output) {
    WWHD_FUNC(0x02378638,BOOL,this,output);
    if(gabi::call<BOOL>(0x025162A4,&mCyl) || mbRollCrash) {
        make_item(); eff_break(); sound_break(); gabi::call(0x025D57E0,this);
        *output=&mMtx; return TRUE;
    }
    if(mBgc.mStateFlags&32) {
        const Attr_c* a=attr(0x1002D490); s8 reverb=mReverb;
        mDoAud_seStart(a->mWaterFallSE,&eyePos,0,reverb);
    }
    s32 mode=mMode;
    if(mode==0) {
        u32 flags=mBgc.mStateFlags;
        if((flags&16) && m646==0) {
            bool floatUp=!(flags&1);
            if(!floatUp) {
                const Attr_c* a=attr(0x1002D490); const Attr_c* b=attr(0x1002D490);
                floatUp=((f32)a->m28+(f32)b->m14)>0.0f;
            }
            if(floatUp) mode_afl_init();
        }
    } else if(mode==2 && (mBgc.mStateFlags&1)) { mode_wait_init(); m646=20; }
    if(!gabi::load<u32>(0x101FDC24)) {
        gabi::store<u32>(0x101FDC24,1); memcpy_g(gabi::at<u8>(0x101FDC28),gabi::at<u8>(0x1002CF94),24);
    }
    ptmf_call(0x101FDC28+8*(s32)mMode,this);
    gabi::call(0x02376914,&mBgc,this);
    if(mBgc.mMaxGroundIdx>=0) {
        gabi::store<u8>(gabi::ea(this)+0x1C9,current.roomNo);
        dBgS* bg=dComIfG_Bgsp(); u32 check=0x1046A7CC+0x54*(s32)mBgc.mMaxGroundIdx;
        u32 color=gabi::call<u32>(0x024EEEB8,bg,gabi::at<u8>(check ? check+0x14 : 0));
        gabi::store<u8>(gabi::ea(this)+0x1CA,color);
    }
    if(m648>0) { m648=m648-1; set_mtx(); }
    else {
        if(mBgc.mStateFlags&4) {
            sound_land(); vib_land(); if(!(mBgc.mStateFlags&16)) eff_land_smoke();
        }
        set_mtx();
    }
    if(mType==0 || mType==5) {
        gabi::call(0x02516680,&mCyl,&current.pos);
        u32 play=dComIfGp_ea(); gabi::call(0x0200E240,gabi::at<u8>(play+0x26A4),&mCyl);
    }
    *output=&mMtx; return TRUE;
}
VERIFY(0x02378638,&Act_c::Execute);

void Act_c::set_mtx() {
    WWHD_FUNC(0x02377764,void,this);
    bool floating=mMode==2; auto* matrix=gabi::at<Mtx34>(0x1048D0CC);
    gabi::call(0x028E93CC,matrix,(f32)current.pos.x,(f32)current.pos.y,(f32)current.pos.z);
    if(floating) {
        gabi::Local<cXyz> vec; vec->set(m614,1.0f,m618);
        gabi::call(0x025F24E0,0.0f,(f32)m624,0.0f);
        gabi::Local<be<f32>[4]> quat; gabi::call(0x023123D8,quat.get(),vec.get());
        gabi::call(0x025F25CC,quat.get());
    }
    gabi::call(0x025F1B48,matrix,(s16)shape_angle.x,(s16)shape_angle.y,(s16)shape_angle.z);
    if(floating) gabi::call(0x025F24E0,0.0f,-(f32)m624,0.0f);
    if(mpModel!=nullptr) {
        f32 values[12]; for(s32 i=0;i<12;i++) values[i]=gabi::load<f32>(0x1048D0CC+4*i);
        for(s32 i=0;i<12;i++) gabi::store<f32>(gabi::ea(mpModel.get())+0xC8+4*i,values[i]);
    }
    gabi::call(0x028E90D4,matrix,&mMtx);
    if(mChildPID==0xFFFFFFFF) return;
    gabi::Local<be<u32>> found;
    if(!gabi::call<BOOL>(0x025D54C4,(u32)mChildPID,found.get())) { mChildPID=0xFFFFFFFF; return; }
    u32 child=*found.get(); if(!child) return;
    s32 type=mType;
    if(type==7 || type==10) {
        f32 distance=gabi::call<f32>(0x028E8DE8,gabi::at<cXyz>(child+0x314),&current.pos);
        if(distance>0.00009999999747378752f) {
            gabi::store<f32>(child+0x314,current.pos.x); gabi::store<f32>(child+0x318,current.pos.y);
            f32 z=current.pos.z; gabi::store<u8>(child+(type==7 ? 0x3FA : 0xDD4),1); gabi::store<f32>(child+0x31C,z);
        }
    } else if(type==11) {
        gabi::Local<cXyz> pos; pos->set(current.pos.x,current.pos.y+150.0f,current.pos.z);
        f32 distance=gabi::call<f32>(0x028E8DE8,gabi::at<cXyz>(child+0x314),pos.get());
        if(distance>0.00009999999747378752f) {
            gabi::store<f32>(child+0x314,pos->x); gabi::store<f32>(child+0x318,pos->y);
            f32 z=pos->z; gabi::store<u8>(child+0x10D0,1); gabi::store<f32>(child+0x31C,z);
        }
    } else {
        f32 height=attr(0x1002D35C)->m68-5.0f;
        gabi::Local<cXyz> offset,pos; offset->set(0.0f,height,0.0f);
        gabi::call(0x028E9044,matrix,offset.get(),pos.get());
        f32 x=pos->x,y=pos->y,z=pos->z;
        f32 mx=gabi::load<f32>(0x1048D0CC+12),my=gabi::load<f32>(0x1048D0CC+28),mz=gabi::load<f32>(0x1048D0CC+44);
        gabi::store<f32>(0x1048D0CC+12,mx+x); gabi::store<f32>(0x1048D0CC+28,my+y); gabi::store<f32>(0x1048D0CC+44,mz+z);
        gabi::call(0x028E90D4,matrix,gabi::at<Mtx34>((u32)*found.get()+0x3184));
    }
}
VERIFY(0x02377764,&Act_c::set_mtx);

BOOL Act_c::Create() {
    WWHD_FUNC(0x02377C1C,BOOL,this);
    m608=0.0f;m60C=0.0f;m610=0.0f;m614=0.0f;m618=0.0f;m61C=0.0f;m604=0;m620=0.0f;
    f32 height=attr(0x1002D3D8)->m68;
    m628=0;m62C=0;m634=-1;mPPLabel=0;m624=height*0.5f;m630=0.0f;
    clr_moment_cnt(); m64A=0;m644=0;m646=0;m648=20;
    gabi::store<u32>(gabi::ea(mpBgW)+0xB0,0x02377308);
    gabi::store<u32>(gabi::ea(mpBgW)+0xB4,0x023775DC);
    gabi::call(0x02515F14,&mStts,255,255,this);
    gabi::call(0x02516518,&mCyl,gabi::at<u8>(0x1002D758));
    u32 self=gabi::ea(this),co=gabi::load<u32>(self+0x488);
    gabi::store<u32>(self+0x4A0,self+0x420);
    gabi::at<cXyz>(self+0x510)->copy(*gabi::at<cXyz>(0x101FFBA8));
    u32 tg=gabi::load<u32>(self+0x4F0);
    gabi::store<u32>(self+0x348,self+0x3E8);
    gabi::store<u32>(self+0x488,co&~1u);gabi::store<u32>(self+0x4F0,tg|4);
    const Attr_c* a[6]; for(s32 i=0;i<6;i++) a[i]=attr(0x1002D3D8);
    gabi::call(0x025D674C,this,(f32)(s16)a[0]->mCullMinX,(f32)(s16)a[1]->mCullMinY,(f32)(s16)a[2]->mCullMinZ,(f32)(s16)a[3]->mCullMaxX,(f32)(s16)a[4]->mCullMaxY,(f32)(s16)a[5]->mCullMaxZ);
    speedF=0.0f; gravity=attr(0x1002D3D8)->m14;
    gabi::call(0x025D6870,this,0);gabi::call(0x02376914,&mBgc,this);
    s8 room=home.roomNo;u32 flags=mBgc.mStateFlags;m64F=1;mBgc.mStateFlags=flags&~0x26u;
    mReverb=dComIfGp_getReverb(room);mChildPID=0xFFFFFFFF;
    s32 noBuoy=prm(1,31);s32 type=mType;
    if(!noBuoy) {
        f32 x=current.pos.x;f32 h=attr(0x1002D3D8)->m68;
        f32 y=current.pos.y; s8 childRoom=current.roomNo;
        gabi::Local<cXyz> pos;pos->set(x,(y+h)-5.0f,current.pos.z);
        mChildPID=gabi::call<u32>(0x025D5A20,0x1D1,gabi::load<u32>(self+4),0,pos.get(),childRoom,&shape_angle,0,-1,0);
    } else if(type==7 || type==10) {
        s8 childRoom=current.roomNo;u32 pid=gabi::load<u32>(self+4);
        mChildPID=gabi::call<u32>(0x025D5A20,type==7 ? 0xA6 : 0x110,pid,type==7 ? 1 : 0,&current.pos,childRoom,&shape_angle,0,-1,0);
    } else if(type==11) {
        u32 low=mPrmX&255;gabi::Local<cXyz> pos;pos->set(current.pos.x,current.pos.y+150.0f,current.pos.z);
        u32 second=mPrmZ>>8;u32 params=low|0x3000|(second<<16);
        s8 childRoom=current.roomNo;u32 pid=gabi::load<u32>(self+4);
        mChildPID=gabi::call<u32>(0x025D5A20,0x4B,pid,params,pos.get(),childRoom,&shape_angle,0,-1,0);
    }
    init_mtx();mbRollCrash=0;mode_wait_init();return TRUE;
}
VERIFY(0x02377C1C,&Act_c::Create);

}
