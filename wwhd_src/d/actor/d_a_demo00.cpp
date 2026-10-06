#include "d/actor/d_a_demo00.h"
#include "gabi.h"

namespace {
void* ptr(u32 a) { return gabi::at<void>(a); }
void* offset(void* p, u32 n) { return ptr(gabi::ea(p) + n); }
be<u32>& word(void* p, u32 n) { return *gabi::at<be<u32>>(gabi::ea(p) + n); }
be<f32>& scalar(void* p, u32 n) { return *gabi::at<be<f32>>(gabi::ea(p) + n); }
void setAction(daDemo00_c* actor, u32 target) {
    actor->actionAdjustment = 0;
    actor->actionIndex = -1;
    actor->actionTarget = target;
}
}

void* demo00_lightConstructor(void* light) {
    WWHD_FUNC(0x0211F31C, void*, light);
    if (!light) light = gabi::call<void*>(0x0273AD10, 0x34u);
    if (light) scalar(light, 0x28) = *gabi::at<be<f32>>(0x1000D6A8);
    return light;
}
VERIFY(0x0211F31C, demo00_lightConstructor);

void demo00_resetIDs(Demo00ResourceIDs* ids) {
    WWHD_FUNC(0x0212139C, void, ids);
    ids->brk = -1u;
    ids->plight = -1u;
    ids->auxiliary = -1u;
    ids->bck = -1u;
    ids->btk = -1u;
    ids->btp = -1u;
    ids->shape = -1u;
}
VERIFY(0x0212139C, demo00_resetIDs);

s32 demo00_create(daDemo00_c* actor) {
    WWHD_FUNC(0x021213C0, s32, actor);
    u32 condition = actor->actor_condition;
    if (!(condition & 8)) {
        if (actor) {
            gabi::call(0x025D4ED0, actor);
            condition = actor->actor_condition;
            actor->__vtbl = 0x1000D9E4;
        }
        actor->actor_condition = condition | 8;
    }
    s8 room = *gabi::at<be<s8>>(0x1047E6C8);
    gabi::call(0x0255FFF4, &actor->tevStr, room, 0xFFu);
    setAction(actor, 0x02121624);
    demo00_resetIDs(&actor->nextIDs);
    actor->previousDrawMode = -1;
    return 4;
}
VERIFY(0x021213C0, demo00_create);

void demo00_resetModel(Demo00Model* model) {
    WWHD_FUNC(0x02121460, void, model);
    demo00_resetIDs(&model->ids);
    void* light = model->plight;
    model->model = nullptr;
    model->auxiliary = nullptr;
    model->btp = nullptr;
    model->btk = nullptr;
    model->brk = nullptr;
    if (light) {
        gabi::call(0x02563BF0, light);
        model->plight = nullptr;
    }
}
VERIFY(0x02121460, demo00_resetModel);

void demo00_destructor(daDemo00_c* actor, u32 flags) {
    WWHD_FUNC(0x021214C0, void, actor, flags);
    if (actor) {
        void* heap = actor->heap;
        actor->__vtbl = 0x1000D9E4;
        if (heap) {
            void* morf = actor->demoModel.morf;
            if (morf) gabi::call(0x025E563C, morf);
        }
        gabi::call(0x025D50BC, actor, 0u);
        if (flags & 1) gabi::call(0x0273AF40, actor);
    }
}
VERIFY(0x021214C0, demo00_destructor);

s32 demo00_leaving(daDemo00_c* actor, void* demoActor) {
    WWHD_FUNC(0x02122804, s32, actor, demoActor);
    void* morf = actor->demoModel.morf;
    if (morf) gabi::call(0x025E563C, morf);
    gabi::call(0x025D6134, actor);
    setAction(actor, 0x02121624);
    return 1;
}
VERIFY(0x02122804, demo00_leaving);

void demo00_staticInit() {
    WWHD_FUNC(0x02122864, void);
    word(ptr(0x10463AEC), 8) = 0;
    word(ptr(0x10463AEC), 0) = 0;
    word(ptr(0x10463AEC), 12) = 0;
    word(ptr(0x10463AEC), 4) = 0;
    gabi::call(0x028F026C, ptr(0x101B42FC));
    scalar(ptr(0x10463AE0), 0) = *gabi::at<be<f32>>(0x1000D9D8);
    scalar(ptr(0x10463AE4), 0) = *gabi::at<be<f32>>(0x1000D9DC);
    gabi::call(0x028ED6F8, ptr(0x10463AE8));
    gabi::call(0x028F026C, ptr(0x101B4308));
    gabi::call(0x028EAB2C, ptr(0x10463AE9));
    gabi::call(0x028F026C, ptr(0x101B4314));
}
VERIFY(0x02122864, demo00_staticInit);

void demo00_cleanupDestructor(void* object, u32 flags) {
    WWHD_FUNC(0x021228F8, void, object, flags);
    if (object && (flags & 1)) gabi::call(0x0273AF40, object);
}
VERIFY(0x021228F8, demo00_cleanupDestructor);

s32 demo00_delete(daDemo00_c* actor) {
    WWHD_FUNC(0x02121394, s32, actor);
    return 1;
}
VERIFY(0x02121394, demo00_delete);

s32 demo00_isDelete(daDemo00_c* actor) {
    WWHD_FUNC(0x0212290C, s32, actor);
    return 1;
}
VERIFY(0x0212290C, demo00_isDelete);

void demo00_stringCallback(void* string) {
    WWHD_FUNC(0x02122914, void, string);
}
VERIFY(0x02122914, demo00_stringCallback);

void demo00_setBaseMtx(daDemo00_c* actor) {
    WWHD_FUNC(0x0212153C, void, actor);
    void* matrix = ptr(0x1048D0CC);
    f32 x = actor->current.pos.x, z = actor->current.pos.z, y = actor->current.pos.y;
    gabi::call(0x028E93CC, matrix, x, y, z);
    s16 ax = actor->shape_angle.x, az = actor->shape_angle.z, ay = actor->shape_angle.y;
    gabi::call(0x025F19F8, matrix, ax, ay, az);
    f32 values[12];
    for (u32 i = 0; i < 12; ++i) values[i] = scalar(matrix, 4 * i);
    void* model = actor->demoModel.model;
    for (u32 i = 0; i < 12; ++i) scalar(model, 0xC8 + 4 * i) = values[i];
    model = actor->demoModel.model;
    y = actor->scale.y; x = actor->scale.x; z = actor->scale.z;
    scalar(model, 0xBC) = x;
    scalar(model, 0xC0) = y;
    scalar(model, 0xC4) = z;
    model = actor->demoModel.model;
    gabi::call(0x027F4D5C, model);
}
VERIFY(0x0212153C, demo00_setBaseMtx);

namespace {
struct DemoString { be<u32> string, vtable; };
bool demoArchiveEquals(u32 literal) {
    gabi::Local<DemoString> left, right;
    right->vtable = 0x1000D6D0;
    right->string = 0x1047E6B8;
    left->vtable = 0x1000D6D0;
    left->string = literal;
    demo00_stringCallback(left.get());
    u32 vtable = left->vtable;
    gabi::call((u32)word(ptr(vtable), 0x14), left.get());
    vtable = right->vtable;
    u32 target = word(ptr(vtable), 0x14);
    u32 a = left->string;
    gabi::call(target, right.get());
    u32 b = right->string;
    if (a == b) return true;
    for (u32 i = 0; i < 0x40001; ++i) {
        u8 ca = *gabi::at<be<u8>>(a + i);
        u8 cb = *gabi::at<be<u8>>(b + i);
        if (ca != cb) return false;
        if (!ca) return true;
    }
    return false;
}
void invokeAction(daDemo00_c* actor, void* demoActor) {
    s16 index = actor->actionIndex;
    s16 adjustment = actor->actionAdjustment;
    void* self = ptr(gabi::ea(actor) + adjustment);
    u32 target;
    if (index < 0) target = actor->actionTarget;
    else {
        s16 tableOffset = *gabi::at<be<s16>>(gabi::ea(actor) + 0x3B2);
        u32 table = word(self, tableOffset);
        target = word(ptr(table), (u32)(s32)index * 8 + 4);
    }
    gabi::call(target, self, demoActor);
}
}

s32 demo00_standby(daDemo00_c* actor, void* demoActor) {
    WWHD_FUNC(0x02121624, s32, actor, demoActor);
    if ((u32)actor->nextIDs.shape == -1u && (u32)actor->nextIDs.plight == -1u) return 1;
    u32 auxiliary = actor->nextIDs.auxiliary, shape = actor->nextIDs.shape, bck = actor->nextIDs.bck;
    actor->demoModel.ids.shape = shape;
    actor->demoModel.ids.bck = bck;
    u32 btp = actor->nextIDs.btp;
    actor->demoModel.ids.auxiliary = auxiliary;
    actor->demoModel.ids.btp = btp;
    u32 brk = actor->nextIDs.brk, btk = actor->nextIDs.btk, plight = actor->nextIDs.plight;
    actor->demoModel.ids.btk = btk;
    actor->demoModel.ids.brk = brk;
    actor->demoModel.ids.plight = plight;
    if (!gabi::call<s32>(0x025D63E8, actor, ptr(0x02120244), 0x4000u)) return 1;
    if ((void*)actor->demoModel.model) {
        demo00_setBaseMtx(actor);
        void* model = actor->demoModel.model;
        actor->cullMtx = model ? gabi::ea(model) + 0xC8 : 0;
        word(demoActor, 0x48) = gabi::ea(model);
        void* morf = actor->demoModel.morf;
        if (morf) scalar(demoActor, 0x38) = (f32)(s16)*gabi::at<be<s16>>(gabi::ea(morf) + 0xA2);
    }
    s8 argument = actor->argument;
    setAction(actor, 0x021219B8);
    bool enter = argument == 1 && demoArchiveEquals(0x1000D94C);
    if (!enter) {
        if (argument == 1) argument = actor->argument;
        enter = argument == 2 && demoArchiveEquals(0x1000D954);
    }
    if (enter) invokeAction(actor, demoActor);
    return 1;
}
VERIFY(0x02121624, demo00_standby);

void demo00_adjustPatternFrame(daDemo00_c* actor) {
    WWHD_FUNC(0x021218E4, void, actor);
    gabi::Local<cXyz> motion;
    gabi::call(0x0201ADE0, &actor->current.pos, motion.get(), &actor->old.pos);
    void* morf = actor->demoModel.morf;
    u32 state = word(morf, 0x54);
    void* direction = ptr((u32)word(morf, 0x30) + 0x10);
    word(morf, 0x54) = state | 8;
    gabi::call(0x028E8D88, motion.get(), direction, motion.get());
    f32 squared = gabi::call<f32>(0x028E8DD0, motion.get());
    f32 length = gabi::call<f32>(0x028F4384, squared);
    f32 bias = *gabi::at<be<f32>>(0x1000D95C);
    f32 target = gabi::fmadds(length, bias, bias);
    f32 minimum = *gabi::at<be<f32>>(0x1000D6A8);
    f32 frameTarget = minimum;
    if (!(target < minimum)) {
        f32 maximum = *gabi::at<be<f32>>(0x1000D848);
        frameTarget = target - maximum >= 0.0f ? maximum : target;
    }
    void* animation = actor->demoModel.auxiliary;
    f32 rate = *gabi::at<be<f32>>(0x1000D960);
    f32 limit = *gabi::at<be<f32>>(0x1000D964);
    gabi::Local<be<f32>> frame;
    *frame = scalar(animation, 4);
    gabi::call(0x0200ECD4, frame.get(), frameTarget, rate, limit, rate);
    f32 result = *frame;
    animation = actor->demoModel.auxiliary;
    scalar(animation, 4) = result;
    animation = actor->demoModel.auxiliary;
    gabi::call(0x025E742C, offset(animation, 4));
}
VERIFY(0x021218E4, demo00_adjustPatternFrame);

namespace {
u8 byte(u32 address) { return *gabi::at<be<u8>>(address); }
void storeByte(void* p, u32 n, u8 value) { *gabi::at<be<u8>>(gabi::ea(p) + n) = value; }
void storeHalf(void* p, u32 n, u16 value) { *gabi::at<be<u16>>(gabi::ea(p) + n) = value; }
bool equalChars(u32 a, u32 b, u32 maximum = 0x40001) {
    if (a == b) return true;
    for (u32 i = 0; i < maximum; ++i) {
        u8 ca = byte(a + i), cb = byte(b + i);
        if (ca != cb) return false;
        if (!ca) return true;
    }
    return false;
}
void callStringCallback(DemoString* string) {
    u32 table = string->vtable;
    gabi::call((u32)word(ptr(table), 0x14), string);
}
bool equalStrings(DemoString* left, DemoString* right, unsigned leftCalls) {
    for (unsigned i = 0; i < leftCalls; ++i) callStringCallback(left);
    u32 table = right->vtable;
    u32 target = word(ptr(table), 0x14);
    u32 a = left->string;
    gabi::call(target, right);
    return equalChars(a, right->string);
}
void initString(DemoString* string, u32 text) {
    string->vtable = 0x1000D6D0;
    string->string = text;
}
void* demoResource(u16 id) {
    gabi::Local<DemoString> archive;
    archive->vtable = 0x1000D6D0;
    void* manager = ptr(*gabi::at<be<u32>>(0x101F4F28));
    archive->string = 0x1047E6B8;
    return gabi::call<void*>(0x026067F4, manager, archive.get(), id);
}
void* namedDemoResource(u32 archiveName, u32 resourceName) {
    gabi::Local<DemoString> archive, resource;
    initString(archive.get(), archiveName);
    initString(resource.get(), resourceName);
    void* manager = ptr(*gabi::at<be<u32>>(0x101F4F28));
    return gabi::call<void*>(0x02606900, manager, archive.get(), resource.get());
}
u32 animationMode(void* resource) {
    u32 table = word(resource, 4);
    return gabi::call<u32>((u32)word(ptr(table), 0xC), resource);
}
u32 shiftLeft(u32 word, u32 amount) { return amount & 32 ? 0 : word << (amount & 31); }
u32 shiftRight(u32 word, u32 amount) { return amount & 32 ? 0 : word >> (amount & 31); }
struct DemoTextureDescriptor { be<u32> words[9]; };
struct DemoHeapTextureScratch { DemoTextureDescriptor descriptor; u8 reserved[0x160 - sizeof(DemoTextureDescriptor)]; };
}

s32 demo00_createHeap(daDemo00_c* actor) {
    WWHD_FUNC(0x0211F35C, s32, actor);
    // The texture descriptor encodes its guest address into resource-relative offsets.
    // HD places it at entry SP - 0x160 (frame 0x178, descriptor at +0x18).
    gabi::Local<DemoHeapTextureScratch> textureScratch;
    if ((u32)actor->demoModel.ids.shape != -1u) {
        void* data = demoResource((u16)(u32)actor->demoModel.ids.shape);
        if (!data) gabi::call(0x0273AA24, ptr(0x1000D7FC), 0x115u, ptr(0x1000D80C));
        u32 modelFlags = 0x11000002;
        f32 speed = *gabi::at<be<f32>>(0x1000D6A8);
        if ((u32)actor->demoModel.ids.btp != -1u) {
            void* controller = gabi::call<void*>(0x025E7820, nullptr);
            actor->demoModel.btp = controller;
            if (!controller) return 0;
            void* resource = demoResource((u16)(u32)actor->demoModel.ids.btp);
            if (!resource) return 1;
            u32 mode = animationMode(resource);
            controller = actor->demoModel.btp;
            if (!gabi::call<s32>(0x025E789C, controller, data, resource, 1u, mode, 0u, -1, 0u, 0u, speed)) return 0;
            modelFlags = 0x15020002;
        }
        if ((u32)actor->demoModel.ids.btk != -1u) {
            void* controller = gabi::call<void*>(0x025E7C6C, nullptr);
            actor->demoModel.btk = controller;
            if (!controller) return 0;
            void* resource = demoResource((u16)(u32)actor->demoModel.ids.btk);
            if (!resource) return 1;
            u32 mode = animationMode(resource);
            controller = actor->demoModel.btk;
            if (!gabi::call<s32>(0x025E7CE0, controller, data, resource, 1u, mode, 0u, -1, 0u, 0u, speed)) return 0;
            modelFlags |= ((u32)actor->demoModel.ids.btk & 0x10000000) ? 0x1200 : 0x200;
        }
        if ((u32)actor->demoModel.ids.brk != -1u) {
            void* controller = gabi::call<void*>(0x025E80D0, nullptr);
            actor->demoModel.brk = controller;
            if (!controller) return 0;
            void* resource = demoResource((u16)(u32)actor->demoModel.ids.brk);
            if (!resource) return 1;
            u32 mode = animationMode(resource);
            controller = actor->demoModel.brk;
            if (!gabi::call<s32>(0x025E8154, controller, data, resource, 1u, mode, 0u, -1, 0u, 0u, speed)) return 0;
        }
        if ((u32)actor->demoModel.ids.bck == -1u) {
            actor->demoModel.morf = nullptr;
            void* model = gabi::call<void*>(0x025E38E0, data, 0x80000u, modelFlags);
            actor->demoModel.model = model;
            if (!model) return 0;
            gabi::Local<DemoString> name, first, second, third;
            initString(first.get(), 0x1000D820);
            initString(second.get(), 0x1000D7B8);
            initString(third.get(), 0x1000D7C4);
            void* metadata = gabi::call<void*>(0x027F3F8C, data);
            u32 relative = word(metadata, 4);
            initString(name.get(), relative ? gabi::ea(metadata) + 4 + relative : 0);
            bool special = equalStrings(first.get(), name.get(), 2);
            if (!special) special = equalStrings(second.get(), name.get(), 2);
            if (!special) special = equalStrings(third.get(), name.get(), 2);
            if (special) {
                model = actor->demoModel.model;
                u32 flags = (u32)word(model, 0x74) & ~1u;
                word(model, 0x74) = flags;
                gabi::call(0x027F596C, model, flags);
            }
        } else {
            void* resource = demoResource((u16)(u32)actor->demoModel.ids.bck);
            if (!resource) gabi::call(0x0273AA24, ptr(0x1000D7FC), 0x1A9u, ptr(0x1000D830));
            void* morf = gabi::call<void*>(0x025E4F64, nullptr, data, nullptr, nullptr, resource,
                                         -1, nullptr, -1, 1u, nullptr, 0x80000u, modelFlags, speed);
            actor->demoModel.morf = morf;
            if (!morf || !(u32)word(morf, 0x90)) return 0;
            actor->demoModel.model = ptr(word(morf, 0x90));
            gabi::Local<DemoString> modelName, expected;
            initString(expected.get(), 0x1000D770);
            void* metadata = gabi::call<void*>(0x027F3F8C, data);
            u32 relative = word(metadata, 4);
            initString(modelName.get(), relative ? gabi::ea(metadata) + 4 + relative : 0);
            bool auxiliary = equalStrings(expected.get(), modelName.get(), 2);
            if (auxiliary) {
                auxiliary = false;
                for (u32 text = 0x1000D778; text <= 0x1000D7A8; text += 8) {
                    if (demoArchiveEquals(text)) { auxiliary = true; break; }
                }
            }
            if (auxiliary) {
                void* object = gabi::call<void*>(0x0273AD10, 0x78u);
                if (object) gabi::call(0x025E7C6C, offset(object, 4));
                actor->demoModel.auxiliary = object;
                if (!object) return 0;
                void* auxiliaryData = namedDemoResource(0x1000D7B0, 0x1000D7D4);
                object = actor->demoModel.auxiliary;
                void* auxiliaryModel = gabi::call<void*>(0x025E38E0, auxiliaryData, 0u, 0x11020203u);
                word(object, 0) = gabi::ea(auxiliaryModel);
                object = actor->demoModel.auxiliary;
                if (!(u32)word(object, 0)) return 0;
                void* auxiliaryAnimation = namedDemoResource(0x1000D7B0, 0x1000D7E8);
                object = actor->demoModel.auxiliary;
                if (!gabi::call<s32>(0x025E7CE0, offset(object, 4), auxiliaryData, auxiliaryAnimation,
                                     1u, 2u, 0u, -1, 0u, 0u, speed)) return 0;
            }
            void* model = actor->demoModel.model;
            void* modelData = ptr(word(model, 0xAC));
            void* textures = ptr(word(modelData, 0x30));
            if (textures) {
                void* names = ptr(word(modelData, 0x34));
                if (names) {
                    for (u16 index = 0; index < (u16)*gabi::at<be<u16>>(gabi::ea(textures)); ++index) {
                        void* text = gabi::call<void*>(0x027ED1F0, names, index);
                        if (!text) continue;
                        bool framebuffer = equalChars(gabi::ea(text), 0x1000D6AC, -1u);
                        if (!framebuffer && !equalChars(gabi::ea(text), 0x1000D748, -1u)) continue;
                        storeByte(model, 0x10, 1);
                        if (!equalChars(gabi::ea(text), 0x1000D6AC, -1u)) continue;
                        void* system = ptr(*gabi::at<be<u32>>(0x101F9968));
                        void* framebufferData = gabi::call<void*>(0x027F81A4, system, 6u);
                        void* image = gabi::call<void*>(0x0273AD10, 0xC0u);
                        image = gabi::call<void*>(0x02773680, image, framebufferData);
                        DemoTextureDescriptor* descriptor = &textureScratch->descriptor;
                        // The image pointer is kept in descriptor word 8 (frame +0x38) and is copied with
                        // the descriptor; the final destination+0x20 store reloads it from there.
                        descriptor->words[8] = gabi::ea(image);
                        storeHalf(descriptor, 2, (u16)(u32)word(image, 8));
                        u32 height = word(image, 0xC);
                        storeByte(descriptor, 8, 0);
                        storeHalf(descriptor, 4, (u16)height);
                        void* destination = ptr((u32)word(textures, 4) + (u32)index * 36);
                        for (u32 i = 0; i < 9; ++i) word(destination, i * 4) = (u32)descriptor->words[i];
                        destination = ptr((u32)word(textures, 4) + (u32)index * 36);
                        word(destination, 0x1C) = (u32)word(destination, 0x1C) + gabi::ea(descriptor) - gabi::ea(destination);
                        destination = ptr((u32)word(textures, 4) + (u32)index * 36);
                        word(destination, 0xC) = (u32)word(destination, 0xC) + gabi::ea(descriptor) - gabi::ea(destination);
                        destination = ptr((u32)word(textures, 4) + (u32)index * 36);
                        word(destination, 0x20) = (u32)descriptor->words[8];
                        u32 a = word(textures, 0x18), b = word(textures, 0x1C);
                        if (index < 64) {
                            u32 mask = shiftLeft(a, index) | shiftRight(b, 32 - index) | shiftLeft(b, index + 32);
                            u32 low = word(textures, 0xC), high = word(textures, 8);
                            word(textures, 8) = high | mask;
                            word(textures, 0xC) = low | shiftLeft(b, index);
                        } else {
                            u32 n = index - 64;
                            u32 low = word(textures, 0x14), high = word(textures, 0x10);
                            u32 mask = shiftLeft(a, n) | shiftRight(b, 32 - n) | shiftLeft(b, n + 32);
                            word(textures, 0x14) = low | shiftLeft(b, n);
                            word(textures, 0x10) = high | mask;
                        }
                    }
                }
            }
        }
        if ((u8)actor->drawMode == 3) {
            void* invisible = gabi::call<void*>(0x025E895C, nullptr);
            actor->demoModel.invisibleModel = invisible;
            if (!invisible) return 0;
            void* model = actor->demoModel.model;
            if (!gabi::call<s32>(0x025E8A48, invisible, model)) return 0;
        } else actor->demoModel.invisibleModel = nullptr;
        void* background = gabi::call<void*>(0x0273AD10, 0x58u);
        if (background) {
            gabi::call(0x02008E0C, background);
            storeByte(background, 0x44, 0); storeByte(background, 0x46, 0);
            storeByte(background, 0x47, 0); storeByte(background, 0x48, 0);
            word(background, 4) = gabi::ea(background) + 0x4C;
            word(background, 0x40) = 0x1000D728;
            storeByte(background, 0x49, 0); storeByte(background, 0x45, 0);
            word(background, 0x50) = 1;
            word(background, 0x20) = 0x1000D708;
            storeByte(background, 0x4A, 0);
            word(background, 0) = gabi::ea(background) + 0x40;
            word(background, 0x10) = 0x1000D6F8;
            word(background, 0x4C) = 0x1000D718;
        }
        actor->demoModel.background = background;
        if (!background) return 0;
        word(background, 0x30) = (u32)word(background, 0x30) & ~2u;
    }
    if ((u32)actor->demoModel.ids.plight != -1u) {
        void* light = demo00_lightConstructor(nullptr);
        actor->demoModel.plight = light;
        if (!light) return 0;
        u32 delay = demoArchiveEquals(0x1000D798) ? 60 : 0;
        u32 id = actor->demoModel.ids.plight;
        light = actor->demoModel.plight;
        u8 type = byte(0x101B42B9 + id * 8);
        gabi::call(0x02563ABC, light, &actor->current.pos, id, type, delay);
    }
    return 1;
}
VERIFY(0x0211F35C, demo00_createHeap);

s32 demo00_createHeapCallback(daDemo00_c* actor) {
    WWHD_FUNC(0x02120244, s32, actor);
    return demo00_createHeap(actor);
}
VERIFY(0x02120244, demo00_createHeapCallback);

s32 demo00_draw(daDemo00_c* actor) {
    WWHD_FUNC(0x0212070C, s32, actor);
    void* model = actor->demoModel.model;
    if (!model) return 1;
    void* environment = gabi::call<void*>(0x02555D0C);
    gabi::call(0x025626A4, environment, 0, &actor->current.pos, offset(actor, 0x110));
    environment = gabi::call<void*>(0x02555D0C);
    model = actor->demoModel.model;
    gabi::call(0x02562F5C, environment, model, offset(actor, 0x110));

    if (actor->demoModel.btp) {
        bool skipPattern = false;
        if (demoArchiveEquals(0x1000D6B8)) {
            for (u32 literal : {0x1000D6C0u, 0x1000D6C3u}) {
                model = actor->demoModel.model;
                void* metadata = ptr(word(model, 0x14));
                u32 relative = word(metadata, 4);
                gabi::Local<DemoString> name, expected;
                initString(name, relative ? gabi::ea(metadata) + 4 + relative : 0);
                initString(expected, literal);
                demo00_stringCallback(expected);
                if (equalStrings(expected, name, 1)) { skipPattern = true; break; }
            }
        }
        if (!skipPattern) {
            void* controller = actor->demoModel.btp;
            s16 frame = (s16)gabi::ftoi((f32)scalar(controller, 4));
            model = actor->demoModel.model;
            gabi::call(0x025E7B3C, controller, ptr(word(model, 0xAC)), frame);
        }
    }
    if (actor->demoModel.btk) {
        void* controller = actor->demoModel.btk;
        model = actor->demoModel.model;
        f32 frame = scalar(controller, 4);
        gabi::call(0x025E7FC4, controller, ptr(word(model, 0xAC)), frame);
    }
    if (actor->demoModel.brk) {
        void* controller = actor->demoModel.brk;
        model = actor->demoModel.model;
        f32 frame = scalar(controller, 4);
        gabi::call(0x025E83FC, controller, ptr(word(model, 0xAC)), frame);
    }
    model = actor->demoModel.model;
    if (byte(gabi::ea(model) + 0x10)) {
        for (u32 i = 0; i < (u16)*reinterpret_cast<be<u16>*>(offset(model, 0x2A)); ++i) {
            void* packet = ptr((u32)word(model, 0x34) + i * 0x3C);
            for (u32 slot = 0; slot != 8; ++slot) gabi::call(0x027FA7F8, packet, slot);
            model = actor->demoModel.model;
        }
    }
    void* invisible = actor->demoModel.invisibleModel;
    if (invisible) {
        void* morf = actor->demoModel.morf;
        if (morf) gabi::call(0x025E8E58, invisible, morf);
        else gabi::call(0x025E8DEC, invisible, model);
    } else {
        bool specialList = false;
        if (byte(gabi::ea(model) + 0x10)) {
            gabi::Local<DemoString> expected, stage;
            initString(expected, 0x1000D6C8);
            void* game = gabi::call<void*>(0x025200D4);
            initString(stage, gabi::ea(game) + 0x5134);
            specialList = equalStrings(expected, stage, 2);
        }
        auto selectLists = [](u32 first, u32 second) {
            void* game = gabi::call<void*>(0x025200D4);
            word(ptr(0x104B45C0), 0x74) = word(game, first);
            game = gabi::call<void*>(0x025200D4);
            word(ptr(0x104B45C0), 0x78) = word(game, second);
        };
        if (specialList) selectLists(0x5D8C, 0x5D90);
        u8 mode = actor->drawMode;
        if (mode == 2) selectLists(0x5D84, 0x5D88);
        else if (mode == 8) selectLists(0x5D58, 0x5D60);
        void* morf = actor->demoModel.morf;
        if (morf) {
            gabi::call(0x025E54D8, morf);
            if (actor->demoModel.auxiliary) gabi::call(0x02120248, actor);
        } else {
            model = actor->demoModel.model;
            gabi::call(0x025E2DE0, model, 0);
        }
        model = actor->demoModel.model;
        if (byte(gabi::ea(model) + 0x10) || (u8)actor->drawMode == 2 || (u8)actor->drawMode == 8)
            selectLists(0x5D78, 0x5D7C);
    }
    if (actor->demoModel.btp) {
        model = actor->demoModel.model;
        word(ptr(word(model, 0xAC)), 0x38) = 0;
    }
    if (actor->demoModel.btk) {
        model = actor->demoModel.model;
        word(ptr(word(model, 0xAC)), 0x44) = 0;
    }
    if (actor->demoModel.brk) {
        model = actor->demoModel.model;
        word(ptr(word(model, 0xAC)), 0x48) = 0;
    }
    return 1;
}
VERIFY(0x0212070C, demo00_draw);

void demo00_drawAuxiliary(daDemo00_c* actor) {
    WWHD_FUNC(0x02120248, void, actor);
    void* morf = actor->demoModel.morf;
    word(morf, 0x54) = (u32)word(morf, 0x54) | 8;
    void* joint = ptr(word(morf, 0x30));
    f32 height = scalar(ptr(0x1000D83C), 0);
    if (gabi::call<s32>(0x0246B6A4, (f32)actor->current.pos.x, (f32)actor->current.pos.z))
        height = gabi::call<f32>(0x0246BA0C, (f32)actor->current.pos.x, (f32)actor->current.pos.z);
    if (demoArchiveEquals(0x1000D860)) height = scalar(ptr(0x1000D840), 0);
    else if (demoArchiveEquals(0x1000D868)) height = scalar(ptr(0x1000D844), 0);
    void* matrix = ptr(0x1048D0CC);
    gabi::call(0x028E93CC, matrix,
        (f32)((f32)actor->current.pos.x + (f32)scalar(joint, 0x10)),
        (f32)(height + (f32)scalar(ptr(0x1000D848), 0)),
        (f32)((f32)actor->current.pos.z + (f32)scalar(joint, 0x18)));
    f32 angle = (f32)((f32)scalar(joint, 0x24) + (f32)scalar(ptr(0x1000D84C), 0));
    s16 turn = (s16)gabi::ftoi((f32)(angle * (f32)scalar(ptr(0x1000D850), 0)));
    s16 yaw = *reinterpret_cast<be<s16>*>(offset(actor, 0x32A));
    gabi::call(0x025F1C28, matrix, (s16)(yaw + turn));
    void* auxiliary = actor->demoModel.auxiliary;
    void* model = ptr(word(auxiliary, 0));
    f32 transform[12];
    for (u32 i = 0; i != 12; ++i) transform[i] = scalar(matrix, i * 4);
    for (u32 i = 0; i != 12; ++i) scalar(model, 0xC8 + i * 4) = transform[i];
    auxiliary = actor->demoModel.auxiliary;
    model = ptr(word(auxiliary, 0));
    void* data = ptr(word(model, 0xAC));
    void* material = ptr(word(data, 0x10));
    gabi::Local<be<u32>> color;
    gabi::Local<be<f32>> ignored;
    gabi::call(0x025602F0, &*color, &*ignored);
    f32 delta = (f32)(height - (f32)actor->current.pos.y);
    s32 alpha = (s32)(255u - (u32)gabi::ftoi((f32)(delta + delta)));
    if (alpha < 0) alpha = 0;
    else if (alpha > 255) alpha = 255;
    struct SignedColor { be<s16> r, g, b, a; };
    gabi::Local<SignedColor> channels;
    channels->r = byte(gabi::ea(color));
    channels->g = byte(gabi::ea(color) + 1);
    channels->b = byte(gabi::ea(color) + 2);
    channels->a = alpha;
    void* environment = gabi::call<void*>(0x02555D0C);
    f32 exposure = scalar(environment, 0x10B8);
    void* colorBlock = ptr(word(material, 0x18));
    void* table = ptr(word(colorBlock, 4));
    gabi::call(word(table, 0x24), colorBlock, 1, &*channels);
    struct FloatColor { be<f32> r, g, b, a; };
    gabi::Local<FloatColor> normalized, adjusted;
    f32 maximum = scalar(ptr(0x1000D854), 0);
    normalized->r = (f32)((s16)channels->r / maximum);
    normalized->g = (f32)((s16)channels->g / maximum);
    normalized->b = (f32)((s16)channels->b / maximum);
    normalized->a = (f32)((s16)channels->a / maximum);
    gabi::call(0x0274D458, &*adjusted, &*normalized, exposure);
    word(material, 0xA0) = (u32)word(material, 0xA0) | 0x20;
    void* destination = gabi::call<void*>(0x027F9F0C, offset(material, 0xA0), 5);
    f32 red = adjusted->r, green = adjusted->g, blue = adjusted->b;
    scalar(destination, 4) = green;
    scalar(destination, 8) = blue;
    scalar(destination, 0) = red;
    scalar(destination, 0xC) = (f32)((s16)channels->a / maximum);
    auxiliary = actor->demoModel.auxiliary;
    f32 frame = scalar(auxiliary, 8);
    gabi::call(0x025E7FC4, offset(auxiliary, 4), data, frame);
    auxiliary = actor->demoModel.auxiliary;
    gabi::call(0x025E2DE0, ptr(word(auxiliary, 0)), 0);
}
VERIFY(0x02120248, demo00_drawAuxiliary);

s32 demo00_execute(daDemo00_c* actor) {
    WWHD_FUNC(0x02120CBC, s32, actor);
    u8 actorID = byte(gabi::ea(actor) + 0x2DC);
    actor->groundValid = 0;
    void* demoActor = nullptr;
    if (actorID && actorID <= 32) {
        void* manager = ptr(word(ptr(0x101D5FFC), 0));
        if (!manager) {
            gabi::call(0x0273AA24, ptr(0x1000D764), 0x23A, ptr(0x1000D738));
            manager = ptr(word(ptr(0x101D5FFC), 0));
        }
        demoActor = gabi::call<void*>(0x02526E70, manager, actorID);
    }
    if (!demoActor) { gabi::call(0x025D57E0, actor); return 1; }
    auto enabled = [demoActor](u16 bit) {
        return ((u16)*reinterpret_cast<be<u16>*>(offset(demoActor, 4)) & bit) != 0;
    };
    if (enabled(0x10)) actor->nextIDs.shape = word(demoActor, 0x28);
    if (enabled(0x20)) {
        u32 animation = word(demoActor, 0x2C);
        if ((s16)animation != 26 || !demoArchiveEquals(0x1000D870))
            actor->nextIDs.bck = word(demoActor, 0x2C);
    }
    if (enabled(1)) {
        u8 oldMode = actor->drawMode;
        u8 mode = byte(gabi::ea(demoActor) + 0x4F);
        actor->drawMode = mode;
        struct ParsedData { be<u32> words[5]; };
        gabi::Local<be<u32>> parameter;
        gabi::Local<ParsedData> parsed;
        *parameter = word(demoActor, 0x50);
        gabi::call(0x0283CA7C, &*parameter, &*parsed);
        u8 type = byte(gabi::ea(parsed));
        u32 begin = parsed->words[3], valid = parsed->words[4];
        u8 expectedType = (mode == 6 || mode == 9 || mode == 10) ? 33 :
                          (mode == 4 || mode == 5 || mode == 7) ? 49 : 51;
        if (type && begin && type == expectedType && valid) {
            if (mode == 4) {
                u8 index = byte(begin);
                if (index >= 50) gabi::call(0x0273AA24, ptr(0x1000D8AC), 0x4AA, ptr(0x1000D8BC));
                u16 event = *reinterpret_cast<be<u16>*>(ptr(0x1000D8E8 + index * 2));
                if (event != 0xFFFF) {
                    void* save = ptr(word(ptr(0x101F84DC), 0));
                    gabi::call(0x025B8B68, offset(save, 0x644), event);
                }
            } else if (mode == 5) {
                u8 index = byte(begin);
                if (index >= 10) gabi::call(0x0273AA24, ptr(0x1000D8AC), 0x4C4, ptr(0x1000D884));
                u8 item = byte(0x1000D878 + index);
                if (item != 255) gabi::call(0x0254DA38, item);
            } else if (mode == 6) {
                storeHalf(ptr(0x101F4822), 0, (s8)byte(begin));
            } else if (mode == 7) {
                u8 strength = byte(begin);
                f32 zero = scalar(ptr(0x1000D83C), 0), one = scalar(ptr(0x1000D6A8), 0);
                void* game = gabi::call<void*>(0x025200D4);
                if (strength == 255) gabi::call(0x025CB610, offset(game, 0x599C), 1);
                else {
                    gabi::Local<cXyz> direction;
                    direction->x = zero; direction->y = one; direction->z = zero;
                    if (strength < 100) gabi::call(0x025CB374, offset(game, 0x599C), strength, 1, &*direction);
                    else gabi::call(0x025CB408, offset(game, 0x599C), strength - 100, 1, &*direction);
                }
            } else if (mode == 9 || mode == 10) {
                u32 end = begin + (u32)parsed->words[2];
                if (begin != end) {
                    s8 fade = byte(begin);
                    if ((u8)actor->drawMode != oldMode || fade != (s8)actor->previousDrawMode) {
                        actor->previousDrawMode = fade;
                        s8 speed = begin + 1 == end ? 0 : (s8)byte(begin + 1);
                        if (!fade) gabi::call(0x025F05E8, speed);
                        else gabi::call(0x025F05D0, speed, 0);
                        u32 color = (u8)actor->drawMode == 9 ? 0x101D5E94 : 0x101D5E9C;
                        gabi::call(0x025F05F8, ptr(color));
                    }
                }
            } else {
                u32 end = begin + (u32)parsed->words[2] * 4;
                s32 selector = -1;
                for (u32 entry = begin; entry != end; entry += 4) {
                    u32 value = word(ptr(entry), 0);
                    if (selector < 0) { selector = value; continue; }
                    switch (selector) {
                    case 0: actor->nextIDs.btp = value; break;
                    case 1: actor->nextIDs.btk = value; break;
                    case 2: actor->nextIDs.plight = value; break;
                    case 4: actor->nextIDs.brk = value; break;
                    case 6: actor->nextIDs.btk = value | 0x10000000; break;
                    case 7: actor->nextIDs.brk = value | 0x10000000; break;
                    }
                    selector = -1;
                }
            }
        }
    }
    invokeAction(actor, demoActor);
    return 1;
}
VERIFY(0x02120CBC, demo00_execute);

s32 demo00_executeCallback(daDemo00_c* actor) {
    WWHD_FUNC(0x02121390, s32, actor);
    return demo00_execute(actor);
}
VERIFY(0x02121390, demo00_executeCallback);

namespace {
f32 demoConstant(u32 address) { return scalar(ptr(address), 0); }
u16 demoEnableBits(void* demoActor) {
    return *reinterpret_cast<be<u16>*>(offset(demoActor, 4));
}
void clearDemoFrameEnable(void* demoActor) {
    storeHalf(demoActor, 4, demoEnableBits(demoActor) & ~0x40);
}

// These adjustments are present in the HD scene scripts' native actor path.
void adjustDemoPosition(daDemo00_c* actor, void* demoActor, f32 threshold, f32 amplitude) {
    s8 index = byte(gabi::ea(actor) + 0x2DD);
    if (index == 10 && demoArchiveEquals(0x1000D9A8)) {
        actor->current.pos.x = (f32)((f32)actor->current.pos.x + demoConstant(0x1000D978));
        actor->current.pos.y = (f32)((f32)actor->current.pos.y + amplitude);
        actor->current.pos.z = (f32)((f32)actor->current.pos.z + demoConstant(0x1000D97C));
        demo00_setBaseMtx(actor);
        return;
    }
    index = byte(gabi::ea(actor) + 0x2DD);
    if (index == 0 && (u16)(u32)actor->demoModel.ids.bck == 3 && demoArchiveEquals(0x1000D9B0)) {
        s16 phase = *reinterpret_cast<be<s16>*>(ptr(0x101B42F8));
        phase = (s16)(phase + 0x320);
        storeHalf(ptr(0x101B42F8), 0, phase);
        u32 tableOffset = ((u16)phase >> 3) * 8;
        f32 cosine = scalar(ptr(0x104A44F8 + tableOffset), 4);
        actor->current.pos.y = gabi::fmadds(amplitude, cosine, demoConstant(0x1000D980));
        f32 sine = scalar(ptr(0x104A44F8 + tableOffset), 0);
        s16 roll = (s16)gabi::ftoi((f32)(demoConstant(0x1000D984) * sine));
        storeHalf(actor, 0x32C, roll - 0x16A);
        clearDemoFrameEnable(demoActor);
        demo00_setBaseMtx(actor);
        return;
    }
    index = byte(gabi::ea(actor) + 0x2DD);
    if ((u32)((s32)index - 1) < 4 && demoArchiveEquals(0x1000D9B8)) {
        index = byte(gabi::ea(actor) + 0x2DD);
        u32 frame = word(ptr(0x101D600C), 0);
        if (index == 3 || index == 4) {
            if (frame - 120 < 0x38F) {
                actor->current.pos.y = (f32)((f32)actor->current.pos.y - demoConstant(0x1000D988));
                actor->current.pos.z = (f32)((f32)actor->current.pos.z + demoConstant(0x1000D98C));
                if ((s8)byte(gabi::ea(actor) + 0x2DD) == 3 && frame - 908 < 0x7B)
                    actor->current.pos.x = (f32)((f32)actor->current.pos.x - threshold);
            } else if (frame - 1620 < 0x5D) {
                u32 actorName = index == 3 ? 0x1000D998 : 0x1000D99C;
                void* other = gabi::call<void*>(0x025D9F38, ptr(actorName), 0, 0);
                if (other) {
                    void* morf = ptr(word(other, 0x3C4));
                    void* model = ptr(word(morf, 0x90));
                    void* matrices = ptr(word(model, 0x2C));
                    storeHalf(matrices, 4, (u16)*reinterpret_cast<be<u16>*>(offset(matrices, 4)) | 0x10);
                    void* transforms = ptr(word(matrices, 0x10));
                    gabi::Local<cXyz> jointPosition, translated;
                    jointPosition->x = scalar(transforms, 0x21C);
                    jointPosition->y = scalar(transforms, 0x22C);
                    jointPosition->z = scalar(transforms, 0x23C);
                    gabi::call(0x0201AD78, &*jointPosition, &*translated, ptr(0x1000D9C8));
                    actor->current.pos = *translated;
                }
            }
        } else if (index == 2 && frame - 1044 < 0xA7) {
            actor->current.pos.x = (f32)((f32)actor->current.pos.x - demoConstant(0x1000D990));
        } else if (index == 1 && frame - 1620 < 0x5D) {
            clearDemoFrameEnable(demoActor);
            void* morf = actor->demoModel.morf;
            scalar(morf, 0x98) = demoConstant(0x1000D994);
        }
    }
    demo00_setBaseMtx(actor);
}
}

s32 demo00_performance(daDemo00_c* actor, void* demoActor) {
    WWHD_FUNC(0x021219B8, s32, actor, demoActor);
    if ((u32)actor->demoModel.ids.shape != (u32)actor->nextIDs.shape ||
        (u32)actor->demoModel.ids.plight != (u32)actor->nextIDs.plight) {
        demo00_resetModel(&actor->demoModel);
        setAction(actor, 0x02122804);
        return 1;
    }
    if (!actor->demoModel.model) {
        if (actor->demoModel.plight) {
            gabi::call(0x02527028, actor, 2, 0, 0, 0, 0, 0, 0);
            gabi::call(0x02563C50, actor->demoModel.plight.get(), &actor->current.pos);
        }
        return 1;
    }
    f32 one = demoConstant(0x1000D6A8);
    if (actor->demoModel.morf && (u32)actor->demoModel.ids.bck != (u32)actor->nextIDs.bck) {
        void* animation = demoResource((u16)(u32)actor->nextIDs.bck);
        if (!animation) return 1;
        f32 zero = demoConstant(0x1000D83C);
        f32 transition = demoEnableBits(demoActor) & 0x80 ? (f32)scalar(demoActor, 0x34) : zero;
        gabi::call(0x025E4A98, actor->demoModel.morf.get(), animation, -1, 0,
                   transition, one, zero, demoConstant(0x1000D968));
        actor->demoModel.ids.bck = actor->nextIDs.bck;
        storeHalf(ptr(0x101B42F8), 0, 0);
    }
    if ((u32)actor->demoModel.ids.btp != (u32)actor->nextIDs.btp) {
        void* animation = demoResource((u16)(u32)actor->nextIDs.btp);
        if (!animation) return 1;
        void* model = actor->demoModel.model;
        u32 mode = animationMode(animation);
        gabi::call(0x025E789C, actor->demoModel.btp.get(), ptr(word(model, 0xAC)), animation,
                   1, mode, 0, -1, 1, 0, one);
        actor->demoModel.ids.btp = actor->nextIDs.btp;
    }
    if ((u32)actor->demoModel.ids.btk != (u32)actor->nextIDs.btk) {
        if (demoArchiveEquals(0x1000D9A0) && (u32)actor->nextIDs.btk == 0x1003C)
            actor->nextIDs.btk = 0x10021;
        void* animation = demoResource((u16)(u32)actor->nextIDs.btk);
        if (!animation) return 1;
        bool loop = ((u32)actor->demoModel.ids.btk & 0x10000000) != 0;
        void* model = actor->demoModel.model;
        u32 mode = loop ? 2 : animationMode(animation);
        void* controller = actor->demoModel.btk;
        loop = ((u32)actor->demoModel.ids.btk & 0x10000000) != 0;
        s16 frame = loop ? (s16)gabi::ftoi((f32)scalar(controller, 4)) : 0;
        gabi::call(0x025E7CE0, controller, ptr(word(model, 0xAC)), animation,
                   1, mode, frame, -1, 1, 0, one);
        actor->demoModel.ids.btk = actor->nextIDs.btk;
    }
    if ((u32)actor->demoModel.ids.brk != (u32)actor->nextIDs.brk) {
        void* animation = demoResource((u16)(u32)actor->nextIDs.brk);
        if (!animation) return 1;
        bool loop = ((u32)actor->demoModel.ids.brk & 0x10000000) != 0;
        void* model = actor->demoModel.model;
        u32 mode = loop ? 2 : animationMode(animation);
        void* controller = actor->demoModel.brk;
        loop = ((u32)actor->demoModel.ids.brk & 0x10000000) != 0;
        s16 frame = loop ? (s16)gabi::ftoi((f32)scalar(controller, 4)) : 0;
        gabi::call(0x025E8154, controller, ptr(word(model, 0xAC)), animation,
                   1, mode, frame, -1, 1, 0, one);
        actor->demoModel.ids.brk = actor->nextIDs.brk;
    }
    f32 preservedX = scalar(actor, 0x300), preservedY = scalar(actor, 0x304), preservedZ = scalar(actor, 0x308);
    gabi::call(0x02527028, actor, 0x2A, 0, 0, 0, 0, 0, 0);
    void* background = actor->demoModel.background;
    scalar(actor, 0x304) = preservedY; scalar(actor, 0x300) = preservedX; scalar(actor, 0x308) = preservedZ;
    if (background) {
        f32 x = actor->current.pos.x;
        f32 y = (f32)((f32)actor->current.pos.y + demoConstant(0x1000D96C));
        f32 z = actor->current.pos.z;
        scalar(background, 0x24) = x; scalar(background, 0x28) = y; scalar(background, 0x2C) = z;
        void* game = gabi::call<void*>(0x025200D4);
        background = actor->demoModel.background;
        f32 ground = gabi::call<f32>(0x02008974, offset(game, 0x12A0), background);
        scalar(background, 0x54) = ground;
        actor->groundValid = 1;
    }
    f32 threshold = demoConstant(0x1000D970), amplitude = demoConstant(0x1000D974);
    adjustDemoPosition(actor, demoActor, threshold, amplitude);
    if (demoEnableBits(demoActor) & 0x40) {
        bool automatic = false;
        f32 frame = scalar(demoActor, 0x30);
        if ((s8)byte(gabi::ea(actor) + 0x2DD) == 1 && (u16)(u32)actor->demoModel.ids.bck == 31 &&
            (u32)word(ptr(0x101D600C), 0) < 0x398)
            automatic = demoArchiveEquals(0x1000D9C0);
        if (frame > one || automatic) {
            frame = (f32)(frame - one);
            void* morf = actor->demoModel.morf;
            if (morf) {
                if (!automatic) scalar(morf, 0x9C) = (f32)(s16)gabi::ftoi(frame);
                u32 sound = 0;
                background = actor->demoModel.background;
                if (background && (u8)actor->groundValid &&
                    std::fabs((f32)((f32)scalar(background, 0x54) - (f32)actor->current.pos.y)) < threshold) {
                    void* game = gabi::call<void*>(0x025200D4);
                    background = actor->demoModel.background;
                    sound = gabi::call<u32>(0x024EECAC, offset(game, 0x12A0), background ? offset(background, 0x14) : nullptr);
                }
                s8 room = byte(0x1047E6C8);
                s32 reverb = gabi::call<s32>(0x02520540, room);
                gabi::call(0x025E535C, actor->demoModel.morf.get(), &actor->current.pos, sound, reverb);
                if (automatic) frame = scalar(actor->demoModel.morf, 0x9C);
            }
            void* controller = actor->demoModel.btp;
            if (controller) { scalar(controller, 4) = frame; controller = actor->demoModel.btp; gabi::call(0x025E742C, controller); }
            controller = actor->demoModel.btk;
            if (controller) {
                if (!((u32)actor->demoModel.ids.btk & 0x10000000)) { scalar(controller, 4) = frame; controller = actor->demoModel.btk; }
                gabi::call(0x025E742C, controller);
            }
            controller = actor->demoModel.brk;
            if (controller) {
                if (!((u32)actor->demoModel.ids.brk & 0x10000000)) { scalar(controller, 4) = frame; controller = actor->demoModel.brk; }
                gabi::call(0x025E742C, controller);
            }
        } else {
            void* morf = actor->demoModel.morf;
            if (morf) scalar(morf, 0x9C) = (f32)(s16)gabi::ftoi(frame);
            void* controller = actor->demoModel.btp;
            if (controller) scalar(controller, 4) = frame;
            controller = actor->demoModel.btk;
            if (controller) {
                if ((u32)actor->demoModel.ids.btk & 0x10000000) gabi::call(0x025E742C, controller);
                else scalar(controller, 4) = frame;
            }
            controller = actor->demoModel.brk;
            if (controller) {
                if ((u32)actor->demoModel.ids.brk & 0x10000000) gabi::call(0x025E742C, controller);
                else scalar(controller, 4) = frame;
            }
        }
    } else {
        void* controller = actor->demoModel.morf;
        if (controller) gabi::call(0x025E535C, controller, &actor->current.pos, 0, 0);
        else if ((controller = actor->demoModel.btp)) gabi::call(0x025E742C, controller);
        else if ((controller = actor->demoModel.btk)) gabi::call(0x025E742C, controller);
        else if ((controller = actor->demoModel.brk)) gabi::call(0x025E742C, controller);
    }
    void* light = actor->demoModel.plight;
    if (demoEnableBits(demoActor) & 4) {
        word(actor, 0x330) = word(demoActor, 0x14);
        word(actor, 0x334) = word(demoActor, 0x18);
        word(actor, 0x338) = word(demoActor, 0x1C);
    }
    if (light) {
        gabi::Local<cXyz> position;
        position->x = actor->current.pos.x; position->y = actor->current.pos.y; position->z = actor->current.pos.z;
        void* definition = ptr(0x101B42B8 + (u32)actor->demoModel.ids.plight * 8);
        if (byte(gabi::ea(definition)) == 1) {
            void* model = actor->demoModel.model;
            u32 name = word(definition, 4);
            void* metadata = gabi::call<void*>(0x027F68FC, ptr(word(model, 0xAC)));
            u32 relative = word(metadata, 0x10);
            void* names = relative ? offset(metadata, 0x10 + relative) : nullptr;
            s32 joint = gabi::call<s32>(0x027DF9B0, names, ptr(name));
            model = actor->demoModel.model;
            void* matrices = ptr(word(model, 0x2C));
            u16 flags = *reinterpret_cast<be<u16>*>(offset(matrices, 4));
            void* transform = ptr((u32)word(matrices, 0x10) + (u32)joint * 48);
            storeHalf(matrices, 4, flags | 0x10);
            position->x = scalar(transform, 0xC); position->y = scalar(transform, 0x1C); position->z = scalar(transform, 0x2C);
            light = actor->demoModel.plight;
        }
        gabi::call(0x02563C50, light, &*position);
    }
    if (actor->demoModel.auxiliary) demo00_adjustPatternFrame(actor);
    return 1;
}
VERIFY(0x021219B8, demo00_performance);
