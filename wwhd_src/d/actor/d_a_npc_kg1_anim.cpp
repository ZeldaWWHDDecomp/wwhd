/* Salvatore (Squid-Hunt), reconstructed from WWHD. */
#include "d/actor/d_a_npc_kg1.h"

template<class T> static T readAt(u32 p,u32 off) { return gabi::load<T>(p+off); }
template<class T> static void writeAt(u32 p,u32 off,T v) { gabi::store<T>(p+off,v); }
static u32 playObject() { return gabi::call<u32>(0x025200D4); }
static u32 modelOf(daNpc_Kg1_c* a) { return readAt<u32>(readAt<u32>(gabi::ea(a),0x44C),0x90); }
static u32 objectResource(s32 id) {
    struct ResourceName_l { u8 padding[8]; SafeString name; };
    gabi::Local<ResourceName_l> storage; SafeString* name=&storage->name;
    name->mStringTop=0x1001BF4C; name->__vtbl=0x1001BC44;
    u32 control=gabi::load<u32>(0x101F4F28);
    return gabi::call<u32>(0x026066C4,control,name,id);
}
void daNpc_Kg1_setAnm(daNpc_Kg1_c* a) {
    WWHD_FUNC(0x02265E24,void,a);
    // A private copy preserves the GHS animation table across arbitrary callee effects.
    struct AnimationTable_l { u8 callFrame[32]; be<u32> values[48]; };
    gabi::Local<AnimationTable_l> table;
    u32 tableAddress=gabi::ea(table->values);
    for(int i=0;i<48;++i) gabi::store<u32>(tableAddress+4*i,gabi::load<u32>(0x1001BDA0+4*i));
    auto byte=[&](s32 anim,int off)->s8 { return gabi::load<s8>(tableAddress+(u32)anim*16+off); };
    auto word=[&](s32 anim,int off)->u32 { return gabi::load<u32>(tableAddress+(u32)anim*16+off); };
    auto number=[&](s32 anim,int off)->f32 { return std::bit_cast<f32>(word(anim,off)); };
    s8 previous=a->mPreviousAnimation,requested=a->mRequestedAnimation;
    if(previous!=requested) gabi::call<BOOL>(0x0226586C,a,gabi::load<s16>(0x1001BD88+2*(u32)(s32)requested),1);
    s8 animation=a->mAnimation;
    u32 morf;
    if(animation==2 || ((u32)(s32)animation>=4 && (u32)(s32)animation<=9)) {
        a->mSpecialAnimation=1;
        if(a->mAnimation==9) {
            morf=readAt<u32>(gabi::ea(a),0x44C);
            if(readAt<f32>(morf,0x9C)==31.0f) writeAt<f32>(gabi::ea(a),0x7E4,1.0f);
            morf=readAt<u32>(gabi::ea(a),0x44C);
        } else morf=readAt<u32>(gabi::ea(a),0x44C);
    } else { a->mSpecialAnimation=0; morf=readAt<u32>(gabi::ea(a),0x44C); }
    previous=a->mPreviousAnimation; requested=a->mRequestedAnimation;
    if(previous!=requested) {
        s8 bck=byte(requested,0);
        if(bck!=-1) {
            a->mAnimation=bck;
            u32 animationResource=objectResource(gabi::load<s32>(0x1001BD60+4*(u32)(s32)bck));
            requested=a->mRequestedAnimation;
            gabi::call(0x025E4A98,morf,animationResource,(s32)word(requested,12),number(requested,4),number(requested,8),0.0f,-1.0f,0);
            requested=a->mRequestedAnimation;
            f32 speed=number(requested,8);
            if(speed<0.0f) {
                writeAt<f32>(morf,0x9C,(f32)readAt<s16>(morf,0xA2));
                requested=a->mRequestedAnimation;
            }
        }
    }
    a->mPreviousAnimation=requested;
    if((readAt<u8>(morf,0xA7)&1) || readAt<f32>(morf,0x98)==0.0f) {
        requested=a->mRequestedAnimation;
        s8 next=byte(requested,1);
        if(next!=-1 && word(requested,12)==0) a->mRequestedAnimation=next;
    }
}
VERIFY(0x02265E24,daNpc_Kg1_setAnm);
