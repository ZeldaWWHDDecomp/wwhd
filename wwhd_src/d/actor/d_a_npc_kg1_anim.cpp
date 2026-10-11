/* Salvatore (Squid-Hunt), reconstructed from WWHD. */
#include "d/actor/d_a_npc_kg1_anim.h"

const gc_tu_data gc_tu = {.safeString = 0x1001BC44};
static Kg1AnimationWords anm_prm;
static Kg1ResourceIndices bcks_tbl;
static Kg1TextureIndices btp_tbl;
static Kg1ResourceControl resources;

static J3DAnmTransform *objectResource(s32 index) {
    gabi::Local<Kg1ResourceName> storage;
    SafeString *name = &storage->name;
    name->mStringTop = gabi::guest_addr("Kg");
    name->__vtbl = gc_tu.safeString;
    return Kg1_getResource(resources.control, name, index);
}

/* 02265E24 */
void daNpc_Kg1_setAnm(daNpc_Kg1_c *actor) {
    Kg1Actor *a = reinterpret_cast<Kg1Actor *>(actor);
    // HD: retain the private table copy across arbitrary callee effects.
    gabi::Local<Kg1LocalAnimationWords> table;
    for (int i = 0; i < 48; ++i)
        table->words[i] = u32(anm_prm.words[i]);
    Kg1AnimationParameter *parameters = reinterpret_cast<Kg1AnimationParameter *>(table->words);
    s8 previous = a->mPreviousAnimation, requested = a->mRequestedAnimation;
    if (previous != requested)
        daNpc_Kg1_initTexPatternAnm(actor, btp_tbl.values[requested], 1);
    s8 animation = a->mAnimation;
    Kg1Morf *morf;
    if (animation == 2 || ((u32)(s32)animation >= 4 && (u32)(s32)animation <= 9)) {
        a->mSpecialAnimation = 1;
        if (a->mAnimation == 9) {
            morf = reinterpret_cast<Kg1Morf *>(a->mpMorf.get());
            if (f32(morf->mFrame) == 31.0f)
                a->mTalkAnimationWeight = 1.0f;
            morf = reinterpret_cast<Kg1Morf *>(a->mpMorf.get());
        } else
            morf = reinterpret_cast<Kg1Morf *>(a->mpMorf.get());
    } else {
        a->mSpecialAnimation = 0;
        morf = reinterpret_cast<Kg1Morf *>(a->mpMorf.get());
    }
    previous = a->mPreviousAnimation;
    requested = a->mRequestedAnimation;
    if (previous != requested) {
        s8 bck = parameters[requested].mAnmIdx;
        if (bck != -1) {
            a->mAnimation = bck;
            J3DAnmTransform *animationResource = objectResource(bcks_tbl.values[bck]);
            requested = a->mRequestedAnimation;
            Kg1_setMorfAnm(morf, animationResource, parameters[requested].mLoopMode, parameters[requested].mMorf,
                           parameters[requested].mPlaySpeed, 0.0f, -1.0f, 0);
            requested = a->mRequestedAnimation;
            f32 speed = parameters[requested].mPlaySpeed;
            if (speed < 0.0f) {
                morf->mFrame = f32(s16(morf->mEnd));
                requested = a->mRequestedAnimation;
            }
        }
    }
    a->mPreviousAnimation = requested;
    if ((u8(morf->mFlags) & 1) || f32(morf->mRate) == 0.0f) {
        requested = a->mRequestedAnimation;
        s8 next = parameters[requested].mNextPrmIdx;
        if (next != -1 && u32(s32(parameters[requested].mLoopMode)) == 0)
            a->mRequestedAnimation = next;
    }
}
/* WWHD glue
 data anm_prm 1001BDA0
 data bcks_tbl 1001BD60
 data btp_tbl 1001BD88
 data resources 101F4F28
 ext 0226586C BOOL daNpc_Kg1_initTexPatternAnm(daNpc_Kg1_c*, s32, s32)
 ext 026066C4 J3DAnmTransform* Kg1_getResource(void*, SafeString*, s32)
 ext 025E4A98 void Kg1_setMorfAnm(Kg1Morf*, J3DAnmTransform*, s32, f32, f32, f32, f32, u32)
 str 1001BF4C "Kg"
*/
