/** Warp Demo 20: HD actor behavior. */
#include "bindings.h"
#include "d/actor/d_a_warpdm20.h"

struct Warpdm20ResourceKey {
    gptr<const char> text;
    be<u32> vtable;
};
WWHD_SIZE(Warpdm20ResourceKey, 8);

void daWarpdm20_c::animPlay() {
    WWHD_FUNC(0x024D5208, void, this);
    if (mpBtkAnm) {
        *gabi::at<be<f32>>(gabi::ea(mpBtkAnm.get())) = 1.0f;
        gabi::call(0x025E742C, mpBtkAnm.get());
    }
    if (mpBrkAnm) {
        *gabi::at<be<f32>>(gabi::ea(mpBrkAnm.get())) = 1.0f;
        gabi::call(0x025E742C, mpBrkAnm.get());
    }
    if (mpBckAnm) {
        *gabi::at<be<f32>>(gabi::ea(mpBckAnm.get())) = 1.0f;
        gabi::call(0x025E742C, mpBckAnm.get());
    }
}
VERIFY(0x024D5208, &daWarpdm20_c::animPlay);

static void Warpdm20SetEndAnm(daWarpdm20_c* actor) {
    WWHD_FUNC(0x024D4C94, void, actor);
    if (actor->mpBckAnm) {
        auto* animation = actor->mpBckAnm.get();
        *gabi::at<be<f32>>(gabi::ea(animation) + 4) = f32(*gabi::at<be<s16>>(gabi::ea(animation) + 0xA));
    }
    if (actor->mpBrkAnm) {
        auto* animation = actor->mpBrkAnm.get();
        *gabi::at<be<f32>>(gabi::ea(animation) + 4) = f32(*gabi::at<be<s16>>(gabi::ea(animation) + 0xA));
    }
}
VERIFY(0x024D4C94, Warpdm20SetEndAnm);

static f32 Warpdm20GetSeaY(daWarpdm20_c* actor, cXyz* position) {
    WWHD_FUNC(0x024D4C40, f32, actor, position);
    if (gabi::call<s32>(0x0246B6A4, f32(position->x), f32(position->z)))
        return gabi::call<f32>(0x0246BA0C, f32(position->x), f32(position->z));
    return gabi::call<f32>(0x024F1478, position);
}
VERIFY(0x024D4C40, Warpdm20GetSeaY);

static BOOL Warpdm20DeleteImpl(daWarpdm20_c* actor) {
    WWHD_FUNC(0x024D5034, BOOL, actor);
    gabi::call(0x025A9270, &actor->mRippleVtable);
    gabi::call(0x025204C8, &actor->mPhase, gabi::at<const char>(0x10042220));
    return TRUE;
}
VERIFY(0x024D5034, Warpdm20DeleteImpl);

static BOOL Warpdm20HeapCallback(daWarpdm20_c* actor) {
    WWHD_FUNC(0x024D4C3C, BOOL, actor);
    return gabi::call<BOOL>(0x024D4958, actor);
}
VERIFY(0x024D4C3C, Warpdm20HeapCallback);

static s32 Warpdm20Create(daWarpdm20_c* actor) {
    WWHD_FUNC(0x024D5030, s32, actor);
    return gabi::call<s32>(0x024D4F58, actor);
}
VERIFY(0x024D5030, Warpdm20Create);

static BOOL Warpdm20Delete(daWarpdm20_c* actor) {
    WWHD_FUNC(0x024D5078, BOOL, actor);
    return gabi::call<BOOL>(0x024D5034, actor);
}
VERIFY(0x024D5078, Warpdm20Delete);

static BOOL Warpdm20Draw(daWarpdm20_c* actor) {
    WWHD_FUNC(0x024D5138, BOOL, actor);
    return gabi::call<BOOL>(0x024D507C, actor);
}
VERIFY(0x024D5138, Warpdm20Draw);

static BOOL Warpdm20Execute(daWarpdm20_c* actor) {
    WWHD_FUNC(0x024D5A94, BOOL, actor);
    return gabi::call<BOOL>(0x024D58FC, actor);
}
VERIFY(0x024D5A94, Warpdm20Execute);

static s32 Warpdm20CreateImpl(daWarpdm20_c* actor) {
    WWHD_FUNC(0x024D4F58, s32, actor);
    auto* condition = gabi::at<be<u32>>(gabi::ea(actor) + 0x2E4);
    if (!(u32(*condition) & 8)) {
        if (actor) {
            gabi::call(0x025D4ED0, actor);
            actor->__vtbl = 0x1004211C;
            gabi::call(0x025A9084, &actor->mRippleVtable);
        }
        *condition = u32(*condition) | 8;
    }
    actor->mType = u32(actor->mParameters) & 15;
    const s32 phase = gabi::call<s32>(0x02520460, &actor->mPhase, gabi::at<const char>(0x10042220));
    if (phase == 4) {
        if (!gabi::call<s32>(0x025D63E8, actor, 0x024D4C3C, 0x3000)) return 5;
        gabi::call(0x024D4D10, actor);
    }
    return phase;
}
VERIFY(0x024D4F58, Warpdm20CreateImpl);

static void Warpdm20EventOrder(daWarpdm20_c* actor) {
    WWHD_FUNC(0x024D5708, void, actor);
    if (s32(actor->mEventOrderState) != 1) return;
    gabi::call(0x025D7A58, actor, s32(actor->mEventIdx), 255, 65535, 0, 1);
    auto* condition = gabi::at<be<u16>>(gabi::ea(actor) + 0xFA);
    *condition = u16(*condition) | 2;
}
VERIFY(0x024D5708, Warpdm20EventOrder);

static BOOL Warpdm20CheckWarp(daWarpdm20_c* actor) {
    WWHD_FUNC(0x024D5298, BOOL, actor);
    gabi::call<u32>(0x025200D4);
    u32 game = gabi::call<u32>(0x025200D4);
    const u32 ship = *gabi::at<be<u32>>(game + 0x5B3C);
    game = gabi::call<u32>(0x025200D4);
    if (!(u32(*gabi::at<be<u32>>(game + 0x5CD8)) & 0x10000) || !ship) return FALSE;
    gabi::Local<cXyz> delta, horizontal;
    gabi::call(0x0201ADE0, gabi::at<cXyz>(ship + 0x314), delta.get(), &actor->current.pos);
    horizontal->x = delta->x;
    horizontal->y = 0.0f;
    horizontal->z = delta->z;
    gabi::call<f32>(0x028E8DD0, horizontal.get());
    const f32 distance = gabi::call<f32>(0x028F4384);
    return distance < 500.0f;
}
VERIFY(0x024D5298, Warpdm20CheckWarp);

static BOOL Warpdm20ActDead(daWarpdm20_c* actor, s32 action) {
    WWHD_FUNC(0x024D5B84, BOOL, actor, action);
    BOOL done = FALSE;
    if (actor->mpBtkAnm) {
        actor->mpBtkAnm->mFrameCtrl.mRate = 1.0f;
        gabi::call(0x025E742C, actor->mpBtkAnm.get());
    }
    if (actor->mpBrkAnm) {
        actor->mpBrkAnm->mFrameCtrl.mRate = 1.0f;
        gabi::call(0x025E742C, actor->mpBrkAnm.get());
    }
    if (actor->mpBckAnm) {
        actor->mpBckAnm->mFrameCtrl.mRate = 1.0f;
        if (gabi::call<s32>(0x025E742C, actor->mpBckAnm.get())) {
            actor->mVisible = 0;
            done = TRUE;
        }
    }
    const u32 emitter = gabi::ea(actor->mpRippleEmitter.get());
    if (emitter) {
        gabi::call(0x0200F62C, &actor->mRippleScale, gabi::at<cXyz>(0x101FFBA8), 0.2f);
        const f32 x = actor->mRippleScale.x, y = actor->mRippleScale.y, z = actor->mRippleScale.z;
        *gabi::at<be<f32>>(emitter + 0x240) = z;
        *gabi::at<be<f32>>(emitter + 0x238) = x;
        *gabi::at<be<f32>>(emitter + 0x224) = y;
        *gabi::at<be<f32>>(emitter + 0x220) = x;
        *gabi::at<be<f32>>(emitter + 0x228) = z;
        *gabi::at<be<f32>>(emitter + 0x23C) = y;
    }
    return done;
}
VERIFY(0x024D5B84, Warpdm20ActDead);

static BOOL Warpdm20ActWaitDead(daWarpdm20_c* actor, s32 action) {
    WWHD_FUNC(0x024D5E08, BOOL, actor, action);
    if (actor->mpBtkAnm) {
        actor->mpBtkAnm->mFrameCtrl.mRate = 1.0f;
        gabi::call(0x025E742C, actor->mpBtkAnm.get());
    }
    if (actor->mpBrkAnm) {
        actor->mpBrkAnm->mFrameCtrl.mRate = 1.0f;
        gabi::call(0x025E742C, actor->mpBrkAnm.get());
    }
    return TRUE;
}
VERIFY(0x024D5E08, Warpdm20ActWaitDead);

static void Warpdm20CheckOrder(daWarpdm20_c* actor) {
    WWHD_FUNC(0x024D5468, void, actor);
    if (u16(*gabi::at<be<u16>>(gabi::ea(actor) + 0xF8)) == 2) {
        s32 event = actor->mEventIdx;
        u32 game = gabi::call<u32>(0x025200D4);
        const s32 started = gabi::call<s32>(0x0254407C, gabi::at<void>(game + 0x52C4), event);
        event = actor->mEventIdx;
        if (started && s32(actor->mEventOrderState)) actor->mEventOrderState = 0;
        game = gabi::call<u32>(0x025200D4);
        if (gabi::call<s32>(0x025440C8, gabi::at<void>(game + 0x52C4), event)) {
            game = gabi::call<u32>(0x025200D4);
            auto* flags = gabi::at<be<u16>>(game + 0x52B8);
            *flags = u16(*flags) | 8;
            gabi::call(0x025D57E0, actor);
        }
    } else if (s32(actor->mEventOrderState) == 0) {
        const u32 game = gabi::call<u32>(0x025200D4);
        if (u8(*gabi::at<be<u8>>(game + 0x5292)) == 0)
            gabi::call(0x024D5354, actor);
    }
}
VERIFY(0x024D5468, Warpdm20CheckOrder);

static BOOL Warpdm20DrawImpl(daWarpdm20_c* actor) {
    WWHD_FUNC(0x024D507C, BOOL, actor);
    if (!u8(actor->mVisible)) return TRUE;
    u32 environment = gabi::call<u32>(0x02555D0C);
    gabi::call(0x025626A4, gabi::at<void>(environment), 0, &actor->current.pos, &actor->tevStr);
    environment = gabi::call<u32>(0x02555D0C);
    gabi::call(0x02562F5C, gabi::at<void>(environment), actor->mpModel.get(), &actor->tevStr);
    if (actor->mpBtkAnm) {
        auto* animation = actor->mpBtkAnm.get();
        const u32 data = *gabi::at<be<u32>>(gabi::ea(actor->mpModel.get()) + 0xAC);
        gabi::call(0x025E7FC4, animation, gabi::at<void>(data), f32(animation->mFrameCtrl.mFrame));
    }
    if (actor->mpBrkAnm) {
        auto* animation = actor->mpBrkAnm.get();
        const u32 data = *gabi::at<be<u32>>(gabi::ea(actor->mpModel.get()) + 0xAC);
        gabi::call(0x025E83FC, animation, gabi::at<void>(data), f32(animation->mFrameCtrl.mFrame));
    }
    if (actor->mpBckAnm) {
        auto* animation = actor->mpBckAnm.get();
        const u32 data = *gabi::at<be<u32>>(gabi::ea(actor->mpModel.get()) + 0xAC);
        gabi::call(0x025E86B8, animation, gabi::at<void>(data), f32(animation->mFrameCtrl.mFrame));
    }
    gabi::call(0x025E2DE0, actor->mpModel.get(), 0);
    return TRUE;
}
VERIFY(0x024D507C, Warpdm20DrawImpl);

static BOOL Warpdm20SetEffect(daWarpdm20_c* actor, u32 effect) {
    WWHD_FUNC(0x024D513C, BOOL, actor, effect);
    if (actor->mpRippleEmitter) return FALSE;
    const u32 game = gabi::call<u32>(0x025200D4);
    const u32 particles = *gabi::at<be<u32>>(game + 0x5AB0);
    gabi::call(0x025A847C, gabi::at<void>(particles), 5, effect, &actor->mEffectPosition,
        0, &actor->mRippleScale, 255, &actor->mRippleVtable, -1, 0, 0, 0);
    const u32 emitter = gabi::ea(actor->mpRippleEmitter.get());
    auto* flags = gabi::at<be<u32>>(gabi::ea(actor) + 0x3D4);
    *gabi::at<be<f32>>(gabi::ea(actor) + 0x3D8) = 7.0f;
    *flags = u32(*flags) & ~1u;
    if (emitter) {
        auto* emitterFlags = gabi::at<be<u32>>(emitter + 0x254);
        *emitterFlags = u32(*emitterFlags) | 1;
    }
    return TRUE;
}
VERIFY(0x024D513C, Warpdm20SetEffect);

static void Warpdm20SetWarpFlash(daWarpdm20_c* actor) {
    WWHD_FUNC(0x024D5768, void, actor);
    if (u8(actor->mWarpFlashSet)) return;
    const u32 game = gabi::call<u32>(0x025200D4);
    const u32 particles = *gabi::at<be<u32>>(game + 0x5AB0);
    gabi::call(0x025A847C, gabi::at<void>(particles), 0, 0x82BC, &actor->current.pos,
        0, 0, 255, 0, -1, 0, 0, 0);
    actor->mWarpFlashSet = 1;
}
VERIFY(0x024D5768, Warpdm20SetWarpFlash);

static void Warpdm20InitBckAnm(daWarpdm20_c* actor, s32 index) {
    WWHD_FUNC(0x024D5C88, void, actor, index);
    gabi::Local<Warpdm20ResourceKey> modelKey, animationKey;
    modelKey->text = gabi::at<const char>(0x10042220);
    modelKey->vtable = 0x100420DC;
    const u32 control = *gabi::at<be<u32>>(0x101F4F28);
    const u32 model = gabi::call<u32>(0x026066C4, gabi::at<void>(control), modelKey.get(), 10);
    animationKey->text = gabi::at<const char>(0x10042220);
    animationKey->vtable = 0x100420DC;
    const u32 currentControl = *gabi::at<be<u32>>(0x101F4F28);
    const u32 animation = gabi::call<u32>(0x026066C4, gabi::at<void>(currentControl), animationKey.get(), index);
    gabi::call(0x025E8508, actor->mpBckAnm.get(), gabi::at<void>(model), gabi::at<void>(animation),
        1, 0, 1.0f, 0, -1, 1);
}
VERIFY(0x024D5C88, Warpdm20InitBckAnm);

static void Warpdm20StaticInit() {
    WWHD_FUNC(0x024D5F24, void);
    *gabi::at<be<u32>>(0x1046EB08) = 0;
    *gabi::at<be<u32>>(0x1046EB00) = 0;
    *gabi::at<be<u32>>(0x1046EB0C) = 0;
    *gabi::at<be<u32>>(0x1046EB04) = 0;
    gabi::call(0x028F026C, gabi::at<void>(0x101D2DC4));
    *gabi::at<be<f32>>(0x1046EAF4) = *gabi::at<be<f32>>(0x10042218);
    *gabi::at<be<f32>>(0x1046EAF8) = *gabi::at<be<f32>>(0x1004221C);
    gabi::call(0x028ED6F8, gabi::at<void>(0x1046EAFC));
    gabi::call(0x028F026C, gabi::at<void>(0x101D2DD0));
    gabi::call(0x028EAB2C, gabi::at<void>(0x1046EAFD));
    gabi::call(0x028F026C, gabi::at<void>(0x101D2DDC));
}
VERIFY(0x024D5F24, Warpdm20StaticInit);

static void Warpdm20InitWaitDead(daWarpdm20_c* actor, s32 action) {
    WWHD_FUNC(0x024D5D28, void, actor, action);
    const u32 emitter = gabi::ea(actor->mpRippleEmitter.get());
    if (emitter) {
        auto* flags = gabi::at<be<u32>>(emitter + 0x254);
        *flags = u32(*flags) & ~1u;
    }
    gabi::Local<cXyz> goal;
    goal->x = actor->current.pos.x;
    goal->y = actor->current.pos.y;
    goal->z = f32(actor->current.pos.z) + 1000.0f;
    const u32 game = gabi::call<u32>(0x025200D4);
    gabi::call(0x02543714, gabi::at<void>(game + 0x52C4), goal.get());
    actor->mVisible = 1;
    gabi::call(0x024D5C88, actor, 7);
    if (actor->mpBrkAnm) {
        auto* animation = actor->mpBrkAnm.get();
        animation->mFrameCtrl.mFrame = f32(animation->mFrameCtrl.mEnd);
    }
    if (actor->mpBckAnm) actor->mpBckAnm->mFrameCtrl.mRate = 0.0f;
    gabi::call(0x025E1988, 0x2890);
}
VERIFY(0x024D5D28, Warpdm20InitWaitDead);

static void Warpdm20NormalExecute(daWarpdm20_c* actor) {
    WWHD_FUNC(0x024D5354, void, actor);
    const u32 emitter = gabi::ea(actor->mpRippleEmitter.get());
    const u32 game = gabi::call<u32>(0x025200D4);
    const u32 player = *gabi::at<be<u32>>(game + 0x5B2C);
    gabi::Local<cXyz> delta, horizontal;
    gabi::call(0x0201ADE0, gabi::at<cXyz>(player + 0x314), delta.get(), &actor->current.pos);
    horizontal->x = delta->x;
    horizontal->y = 0.0f;
    horizontal->z = delta->z;
    gabi::call<f32>(0x028E8DD0, horizontal.get());
    const f32 distance = gabi::call<f32>(0x028F4384);
    const u32 save = *gabi::at<be<u32>>(0x101F84DC);
    if (!gabi::call<s32>(0x025B8B94, gabi::at<void>(save + 0x644), 0x2D08)) return;
    actor->mVisible = 1;
    gabi::call(0x024D5208, actor);
    if (emitter) {
        auto* flags = gabi::at<be<u32>>(emitter + 0x254);
        if (distance < 2000.0f) *flags = u32(*flags) & ~1u;
        else *flags = u32(*flags) | 1;
    }
    if (gabi::call<s32>(0x024D5298, actor)) actor->mEventOrderState = 1;
}
VERIFY(0x024D5354, Warpdm20NormalExecute);

static void Warpdm20DemoExecute(daWarpdm20_c* actor) {
    WWHD_FUNC(0x024D57E4, void, actor);
    const u32 demoId = actor->demoActorID;
    actor->mVisible = 1;
    if (!demoId || demoId > 32) return;
    u32 objects = *gabi::at<be<u32>>(0x101D5FFC);
    if (!objects) {
        gabi::call(0x0273AA24, gabi::at<const char>(0x1004213C), 0x23A, gabi::at<const char>(0x1004212C));
        objects = *gabi::at<be<u32>>(0x101D5FFC);
    }
    const u32 demo = gabi::call<u32>(0x02526E70, gabi::at<void>(objects), demoId);
    if (!demo) return;
    const u32 action = *gabi::at<be<u32>>(demo + 0x28);
    actor->mAction = action;
    if (action == 0) {
        gabi::call(0x025DA884, gabi::at<void>(gabi::ea(actor) + 0xDC));
    } else if (action == 1) {
        gabi::call(0x024D513C, actor, 0x82BD);
        const u32 emitter = gabi::ea(actor->mpRippleEmitter.get());
        if (emitter) {
            auto* flags = gabi::at<be<u32>>(emitter + 0x254);
            *flags = u32(*flags) & ~1u;
        }
        gabi::call(0x024D5768, actor);
        const u32 priority = gabi::call<u32>(0x025DF2B8, actor);
        gabi::call(0x025DA874, gabi::at<void>(gabi::ea(actor) + 0xDC), priority);
        gabi::call(0x024D5208, actor);
    }
}
VERIFY(0x024D57E4, Warpdm20DemoExecute);

// HD uses a fixed translation matrix and copies its twelve single-precision entries.
static void Warpdm20CopyTranslation(daWarpdm20_c* actor) {
    gabi::call(0x028E93CC, gabi::at<void>(0x1048D0CC),
        f32(actor->current.pos.x), f32(actor->current.pos.y), f32(actor->current.pos.z));
    u32 values[12];
    for (u32 i = 0; i < 12; ++i) values[i] = *gabi::at<be<u32>>(0x1048D0CC + i * 4);
    const u32 model = gabi::ea(actor->mpModel.get());
    for (u32 i = 0; i < 12; ++i) *gabi::at<be<u32>>(model + 0xC8 + i * 4) = values[i];
}

static BOOL Warpdm20ExecuteImpl(daWarpdm20_c* actor) {
    WWHD_FUNC(0x024D58FC, BOOL, actor);
    if (!u8(actor->demoActorID)) {
        gabi::call(0x024D513C, actor, 0x83D8);
        gabi::call(0x024D5468, actor);
        gabi::call(0x024D5544, actor);
        gabi::call(0x024D5708, actor);
    } else gabi::call(0x024D57E4, actor);
    if (u8(actor->mVisible)) {
        const s32 reverb = gabi::call<s32>(0x02520540, s32(actor->current.roomNo));
        gabi::call(0x025E1A40, 0x1088, gabi::at<void>(gabi::ea(actor) + 0x37C), 0, reverb);
    }
    gabi::Local<cXyz> probe;
    probe->x = actor->current.pos.x;
    probe->y = gabi::fadds_ppc(f32(actor->current.pos.y), 2000.0f);
    probe->z = actor->current.pos.z;
    f32 water = gabi::call<f32>(0x024D4C40, actor, probe.get());
    const f32 scaleY = actor->scale.y;
    if (water == -1000000000.0f) water = 0.0f;
    actor->mEffectPosition.y = water;
    const f32 scaleX = actor->scale.x, scaleZ = actor->scale.z;
    const u32 model = gabi::ea(actor->mpModel.get());
    actor->current.pos.y = water;
    *gabi::at<be<f32>>(model + 0xBC) = scaleX;
    *gabi::at<be<f32>>(model + 0xC0) = scaleY;
    *gabi::at<be<f32>>(model + 0xC4) = scaleZ;
    Warpdm20CopyTranslation(actor);
    return TRUE;
}
VERIFY(0x024D58FC, Warpdm20ExecuteImpl);

static void Warpdm20CreateInit(daWarpdm20_c* actor) {
    WWHD_FUNC(0x024D4D10, void, actor);
    const u32 model = gabi::ea(actor->mpModel.get());
    actor->cullMtx = model ? model + 0xC8 : 0;
    gabi::call(0x025D674C, actor, -300.0f, 0.0f, -300.0f, 300.0f, 500.0f, 300.0f);
    gabi::Local<cXyz> probe;
    probe->x = actor->current.pos.x;
    probe->y = gabi::fadds_ppc(f32(actor->current.pos.y), 2000.0f);
    probe->z = actor->current.pos.z;
    *gabi::at<be<f32>>(gabi::ea(actor) + 0x364) = 1.0f;
    f32 water = gabi::call<f32>(0x024D4C40, actor, probe.get());
    const f32 z = actor->scale.z, y = actor->scale.y, x = actor->scale.x;
    const u32 currentModel = gabi::ea(actor->mpModel.get());
    if (water == -1000000000.0f) water = 0.0f;
    actor->current.pos.y = water;
    actor->mEffectPosition.y = water;
    *gabi::at<be<f32>>(currentModel + 0xC4) = z;
    *gabi::at<be<f32>>(currentModel + 0xBC) = x;
    *gabi::at<be<f32>>(currentModel + 0xC0) = y;
    Warpdm20CopyTranslation(actor);
    actor->mRippleScale.z = 1.0f;
    *gabi::at<be<u32>>(gabi::ea(actor) + 0x404) = *gabi::at<be<u32>>(gabi::ea(actor) + 0x31C);
    *gabi::at<be<u32>>(gabi::ea(actor) + 0x400) = *gabi::at<be<u32>>(gabi::ea(actor) + 0x318);
    actor->mRippleScale.x = 1.0f;
    actor->mEventOrderState = 0;
    *gabi::at<be<u32>>(gabi::ea(actor) + 0x3FC) = *gabi::at<be<u32>>(gabi::ea(actor) + 0x314);
    actor->mRippleScale.y = 1.0f;
    u32 save = *gabi::at<be<u32>>(0x101F84DC);
    if (gabi::call<s32>(0x025B8B94, gabi::at<void>(save + 0x644), 0x2D08)) {
        actor->mVisible = 1;
        gabi::call(0x024D4C94, actor);
    }
    const u32 game = gabi::call<u32>(0x025200D4);
    actor->mEventIdx = gabi::call<s32>(0x02543F10, gabi::at<void>(game + 0x52C4), gabi::at<const char>(0x100421BC), 255);
    save = *gabi::at<be<u32>>(0x101F84DC);
    if (gabi::call<s32>(0x025B8B94, gabi::at<void>(save + 0x644), 0x2D02)) {
        save = *gabi::at<be<u32>>(0x101F84DC);
        gabi::call(0x025B8B68, gabi::at<void>(save + 0x644), 0x1820);
        gabi::call(0x02586E94, 0xB203);
    }
}
VERIFY(0x024D4D10, Warpdm20CreateInit);

struct Warpdm20EventMethod {
    be<s16> adjustment;
    be<s16> virtualIndex;
    be<u32> target;
};
WWHD_SIZE(Warpdm20EventMethod, 8);

static s32 Warpdm20DispatchEventMethod(u32 table, u32 index, daWarpdm20_c* actor, s32 staff) {
    const u32 descriptor = table + index * 8;
    const s32 adjustment = *gabi::at<be<s16>>(descriptor);
    const s32 virtualIndex = *gabi::at<be<s16>>(descriptor + 2);
    const u32 object = gabi::ea(actor) + u32(adjustment);
    u32 target;
    if (virtualIndex < 0) target = *gabi::at<be<u32>>(descriptor + 4);
    else {
        const s32 vtableOffset = *gabi::at<be<s16>>(descriptor + 6);
        const u32 vtable = *gabi::at<be<u32>>(object + u32(vtableOffset));
        target = *gabi::at<be<u32>>(vtable + u32(virtualIndex) * 8 + 4);
    }
    return gabi::call_ptr<s32>(target, gabi::at<void>(object), staff);
}

static void Warpdm20DemoProc(daWarpdm20_c* actor) {
    WWHD_FUNC(0x024D5544, void, actor);
    u32 game = gabi::call<u32>(0x025200D4);
    actor->mStaffIdx = gabi::call<s32>(0x02542D88, gabi::at<void>(game + 0x52C4), gabi::at<const char>(0x100421D0), 0, 0);
    game = gabi::call<u32>(0x025200D4);
    if (u8(*gabi::at<be<u8>>(game + 0x5292)) == 0) return;
    if (u16(*gabi::at<be<u16>>(gabi::ea(actor) + 0xF8)) == 1) return;
    s32 staff = actor->mStaffIdx;
    if (staff == -1) return;
    game = gabi::call<u32>(0x025200D4);
    const s32 action = gabi::call<s32>(0x02542EDC, gabi::at<void>(game + 0x52C4), staff,
        gabi::at<void>(0x101D2DAC), 6, 0, 0);
    staff = actor->mStaffIdx;
    if (action == -1) {
        game = gabi::call<u32>(0x025200D4);
        gabi::call(0x02543280, gabi::at<void>(game + 0x52C4), staff);
        return;
    }
    game = gabi::call<u32>(0x025200D4);
    if (gabi::call<s32>(0x025447C8, gabi::at<void>(game + 0x52C4), staff))
        Warpdm20DispatchEventMethod(0x101D2D2C, u32(action), actor, s32(actor->mStaffIdx));
    if (Warpdm20DispatchEventMethod(0x101D2D5C, u32(action), actor, s32(actor->mStaffIdx))) {
        staff = actor->mStaffIdx;
        game = gabi::call<u32>(0x025200D4);
        gabi::call(0x02543280, gabi::at<void>(game + 0x52C4), staff);
    }
}
VERIFY(0x024D5544, Warpdm20DemoProc);

static BOOL Warpdm20CreateHeap(daWarpdm20_c* actor) {
    WWHD_FUNC(0x024D4958, BOOL, actor);
    gabi::Local<Warpdm20ResourceKey> modelKey, bckKey, btkKey, brkKey;
    modelKey->text = gabi::at<const char>(0x10042220);
    modelKey->vtable = 0x100420DC;
    const u32 modelData = gabi::call<u32>(0x026066C4,
        gabi::at<void>(u32(*gabi::at<be<u32>>(0x101F4F28))), modelKey.get(), 10);
    if (!modelData) gabi::call(0x0273AA24, gabi::at<const char>(0x10042174), 0xDB, gabi::at<const char>(0x10042188));
    actor->mpModel = gabi::call<J3DModel*>(0x025E38E0, gabi::at<void>(modelData), 0x80000, 0x11000222);
    if (!actor->mpModel) return FALSE;

    bckKey->text = gabi::at<const char>(0x10042220);
    bckKey->vtable = 0x100420DC;
    actor->mpBckAnm = nullptr;
    const u32 bckResource = gabi::call<u32>(0x026066C4,
        gabi::at<void>(u32(*gabi::at<be<u32>>(0x101F4F28))), bckKey.get(), 6);
    if (!bckResource) gabi::call(0x0273AA24, gabi::at<const char>(0x10042174), 0xED, gabi::at<const char>(0x10042150));
    const u32 bck = gabi::call<u32>(0x0273AD10, 0x8C);
    if (bck) {
        gabi::call(0x027F2BC0, gabi::at<void>(bck), 0);
        *gabi::at<be<u32>>(bck + 0x10) = 0x1016E54C;
        gabi::call(0x027DA984, gabi::at<void>(bck + 0x14));
        *gabi::at<be<u32>>(bck + 0x80) = 0;
        *gabi::at<be<u32>>(bck + 0x58) = 0;
        *gabi::at<be<u32>>(bck + 0x48) = 0x1016D820;
        *gabi::at<be<u32>>(bck + 0x84) = 0;
        *gabi::at<be<u32>>(bck + 0x10) = 0x100420F4;
        *gabi::at<be<u32>>(bck + 0x7C) = 0;
        *gabi::at<be<u32>>(bck + 0x88) = 0;
    }
    actor->mpBckAnm = gabi::at<mDoExt_baseAnm>(bck);
    if (!bck) return FALSE;
    if (!gabi::call<s32>(0x025E8508, gabi::at<void>(bck), gabi::at<void>(modelData), gabi::at<void>(bckResource),
        1, 0, 1.0f, 0, -1, 0)) return FALSE;
    actor->mpBckAnm->mFrameCtrl.mRate = 0.0f;

    actor->mpBtkAnm = nullptr;
    btkKey->text = gabi::at<const char>(0x10042220);
    btkKey->vtable = 0x100420DC;
    const u32 btkResource = gabi::call<u32>(0x026066C4,
        gabi::at<void>(u32(*gabi::at<be<u32>>(0x101F4F28))), btkKey.get(), 18);
    if (!btkResource) gabi::call(0x0273AA24, gabi::at<const char>(0x10042174), 0x117, gabi::at<const char>(0x1004215C));
    actor->mpBtkAnm = gabi::call<mDoExt_baseAnm*>(0x025E7C6C, 0);
    if (!actor->mpBtkAnm) return FALSE;
    if (!gabi::call<s32>(0x025E7CE0, actor->mpBtkAnm.get(), gabi::at<void>(modelData), gabi::at<void>(btkResource),
        1, 2, 1.0f, 0, -1, 0, 0)) return FALSE;
    actor->mpBtkAnm->mFrameCtrl.mRate = 0.0f;

    actor->mpBrkAnm = nullptr;
    brkKey->text = gabi::at<const char>(0x10042220);
    brkKey->vtable = 0x100420DC;
    const u32 brkResource = gabi::call<u32>(0x026066C4,
        gabi::at<void>(u32(*gabi::at<be<u32>>(0x101F4F28))), brkKey.get(), 14);
    if (!brkResource) gabi::call(0x0273AA24, gabi::at<const char>(0x10042174), 0x12B, gabi::at<const char>(0x10042168));
    actor->mpBrkAnm = gabi::call<mDoExt_baseAnm*>(0x025E80D0, 0);
    if (!actor->mpBrkAnm) return FALSE;
    if (!gabi::call<s32>(0x025E8154, actor->mpBrkAnm.get(), gabi::at<void>(modelData), gabi::at<void>(brkResource),
        1, 0, 1.0f, 0, -1, 0, 0)) return FALSE;
    actor->mpBrkAnm->mFrameCtrl.mRate = 0.0f;
    return TRUE;
}
VERIFY(0x024D4958, Warpdm20CreateHeap);

void daWarpdm20_c::initWait(s32 action) {
    WWHD_FUNC(0x024D5A98, void, this, action);
    if (mpRippleEmitter) {
        auto* flags = gabi::at<be<u32>>(gabi::ea(mpRippleEmitter.get()) + 0x254);
        *flags = u32(*flags) & ~1u;
    }
}
VERIFY(0x024D5A98, &daWarpdm20_c::initWait);

BOOL daWarpdm20_c::actWait(s32 action) {
    WWHD_FUNC(0x024D5AB4, BOOL, this, action);
    gabi::call(0x024D5208, this);
    return TRUE;
}
VERIFY(0x024D5AB4, &daWarpdm20_c::actWait);

void daWarpdm20_c::initWarp(s32 action) {
    WWHD_FUNC(0x024D5AD8, void, this, action);
    gabi::call(0x025E1988, 0x288D);
    const u32 game = gabi::call<u32>(0x025200D4);
    gabi::call(0x02543714, gabi::at<void>(game + 0x52C4), &current.pos);
}
VERIFY(0x024D5AD8, &daWarpdm20_c::initWarp);

BOOL daWarpdm20_c::actWarp(s32 action) {
    WWHD_FUNC(0x024D5B18, BOOL, this, action);
    gabi::call(0x024D5208, this);
    return TRUE;
}
VERIFY(0x024D5B18, &daWarpdm20_c::actWarp);

void daWarpdm20_c::initDead(s32 action) {
    WWHD_FUNC(0x024D5B3C, void, this, action);
    const s32 reverb = gabi::call<s32>(0x02520540, s32(current.roomNo));
    gabi::call(0x025E1A40, 0x6A56, gabi::at<void>(gabi::ea(this) + 0x37C), 0, reverb);
}
VERIFY(0x024D5B3C, &daWarpdm20_c::initDead);

void daWarpdm20_c::initWait2(s32 action) {
    WWHD_FUNC(0x024D5E84, void, this, action);
    const u32 game = gabi::call<u32>(0x025200D4);
    gabi::call(0x02543714, gabi::at<void>(game + 0x52C4), &current.pos);
}
VERIFY(0x024D5E84, &daWarpdm20_c::initWait2);

BOOL daWarpdm20_c::actWait2(s32 action) {
    WWHD_FUNC(0x024D5EBC, BOOL, this, action);
    gabi::call(0x024D5208, this);
    return TRUE;
}
VERIFY(0x024D5EBC, &daWarpdm20_c::actWait2);

u32 daWarpdm20_c::initReturnWait(s32 action) {
    WWHD_FUNC(0x024D5EE0, u32, this, action);
    if (mpRippleEmitter) {
        auto* flags = gabi::at<be<u32>>(gabi::ea(mpRippleEmitter.get()) + 0x254);
        *flags = u32(*flags) & ~1u;
    }
    return gabi::call<u32>(0x025E1988, 0x2890);
}
VERIFY(0x024D5EE0, &daWarpdm20_c::initReturnWait);

BOOL daWarpdm20_c::actReturnWait(s32 action) {
    WWHD_FUNC(0x024D5F00, BOOL, this, action);
    gabi::call(0x024D5208, this);
    return TRUE;
}
VERIFY(0x024D5F00, &daWarpdm20_c::actReturnWait);

static void Warpdm20SafeStringDtor(void* key, u32 flags) {
    WWHD_FUNC(0x024D5FB8, void, key, flags);
    if (key && (flags & 1)) gabi::call(0x0273AF40, key);
}
VERIFY(0x024D5FB8, Warpdm20SafeStringDtor);

static BOOL Warpdm20IsDelete(daWarpdm20_c* actor) {
    WWHD_FUNC(0x024D5FCC, BOOL, actor);
    return TRUE;
}
VERIFY(0x024D5FCC, Warpdm20IsDelete);

static void Warpdm20Dtor(daWarpdm20_c* actor, u32 flags) {
    WWHD_FUNC(0x024D5FD4, void, actor, flags);
    if (actor) {
        gabi::call(0x025D50BC, actor, 0);
        if (flags & 1) gabi::call(0x0273AF40, actor);
    }
}
VERIFY(0x024D5FD4, Warpdm20Dtor);

static void Warpdm20SafeStringEmpty(void* key, void* context) {
    WWHD_FUNC(0x024D6028, void, key, context);
}
VERIFY(0x024D6028, Warpdm20SafeStringEmpty);
