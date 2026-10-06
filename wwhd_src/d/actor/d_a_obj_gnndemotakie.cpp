// Ganondorf central waterfall after fight, HD TU0234B4C0..0234B8DF.
#include "d/actor/d_a_obj_gnndemotakie.h"
#include "bindings.h"
namespace {
template<class T> T read(u32 address) { return *gabi::at<be<T>>(address); }
template<class T> void write(u32 address, T value) { *gabi::at<be<T>>(address) = value; }
void* ptr(u32 address) { return gabi::at<void>(address); }
struct ArchiveName { be<u32> name, vtable; };
}
BOOL gnndemotakie_create_heap(daObjGnntakie_c* waterfall) {
    WWHD_FUNC(0x0234B4C0, BOOL, waterfall);
    gabi::Local<ArchiveName> archive;
    archive->vtable = 0x10029744;
    archive->name = 0x100297C0;
    u32 data = gabi::call<u32>(0x026066C4, ptr(read<u32>(0x101F4F28)), archive.get(), 4);
    if (!data) {
        gabi::call(0x0273AA24, ptr(0x10029790), 0x5A, ptr(0x10029770));
        return 0;
    }
    waterfall->mModel = gabi::call<J3DModel*>(0x025E38E0, ptr(data), 0, 0x11020203);
    if (!waterfall->mModel.get()) return 0;
    gabi::Local<ArchiveName> animationArchive;
    animationArchive->vtable = 0x10029744;
    animationArchive->name = 0x100297C0;
    u32 animation = gabi::call<u32>(0x026066C4, ptr(read<u32>(0x101F4F28)), animationArchive.get(), 7);
    if (!animation) {
        gabi::call(0x0273AA24, ptr(0x10029790), 0x61, ptr(0x10029780));
        return 0;
    }
    return gabi::call<s32>(0x025E7CE0, &waterfall->mpBtkAnm, ptr(data), ptr(animation), 1, 2, 1.0f, 0, -1, 0, 0) != 0;
}
VERIFY(0x0234B4C0, gnndemotakie_create_heap);
BOOL gnndemotakie_solidHeapCB(daObjGnntakie_c* waterfall) {
    WWHD_FUNC(0x0234B5D8, BOOL, waterfall);
    return gnndemotakie_create_heap(waterfall);
}
VERIFY(0x0234B5D8, gnndemotakie_solidHeapCB);
void gnndemotakie_init_mtx(daObjGnntakie_c* waterfall) {
    WWHD_FUNC(0x0234B5DC, void, waterfall);
    u32 model = gabi::ea(waterfall->mModel.get());
    f32 z = waterfall->scale.z;
    f32 x = waterfall->scale.x;
    f32 y = waterfall->scale.y;
    write<f32>(model + 0xBC, x);
    write<f32>(model + 0xC4, z);
    write<f32>(model + 0xC0, y);
}
VERIFY(0x0234B5DC, gnndemotakie_init_mtx);
s32 gnndemotakie_create(daObjGnntakie_c* waterfall) {
    WWHD_FUNC(0x0234B5FC, s32, waterfall);
    u32 condition = waterfall->actor_condition;
    if (!(condition & 8)) {
        if (waterfall) {
            fopAc_ac_c_ct(waterfall);
            waterfall->__vtbl = 0x1002975C;
            gabi::call(0x025E7C6C, &waterfall->mpBtkAnm);
            condition = waterfall->actor_condition;
        }
        waterfall->actor_condition = condition | 8;
    }
    s32 phase = gabi::call<s32>(0x02520460, &waterfall->mPhs, ptr(0x100297C0));
    if (phase == 4) {
        if (!gabi::call<s32>(0x025D63E8, waterfall, ptr(0x0234B5D8), 0)) return 5;
        u32 model = gabi::ea(waterfall->mModel.get());
        waterfall->cullMtx = model ? model + 0xC8 : 0;
        gnndemotakie_init_mtx(waterfall);
        waterfall->mpBtkAnm.mFrameCtrl.mFrame = (f32)(s16)waterfall->mpBtkAnm.mFrameCtrl.mStart;
        waterfall->mpBtkAnm.mFrameCtrl.mRate = 1.0f;
        return 4;
    }
    return phase;
}
VERIFY(0x0234B5FC, gnndemotakie_create);
bool gnndemotakie_delete(daObjGnntakie_c* waterfall) {
    WWHD_FUNC(0x0234B700, bool, waterfall);
    gabi::call(0x025204C8, &waterfall->mPhs, ptr(0x100297C0));
    return true;
}
VERIFY(0x0234B700, gnndemotakie_delete);
bool gnndemotakie_execute(daObjGnntakie_c* waterfall) {
    WWHD_FUNC(0x0234B730, bool, waterfall);
    gabi::call(0x025E742C, &waterfall->mpBtkAnm);
    return true;
}
VERIFY(0x0234B730, gnndemotakie_execute);
bool gnndemotakie_draw(daObjGnntakie_c* waterfall) {
    WWHD_FUNC(0x0234B758, bool, waterfall);
    auto light = dKy_getEnvlight();
    settingTevStruct(light, 4, &waterfall->current.pos, &waterfall->tevStr);
    light = dKy_getEnvlight();
    setLightTevColorType(light, waterfall->mModel.get(), &waterfall->tevStr);
    u32 model = gabi::ea(waterfall->mModel.get());
    f32 frame = waterfall->mpBtkAnm.mFrameCtrl.mFrame;
    u32 modelData = read<u32>(model + 0xAC);
    gabi::call(0x025E7FC4, &waterfall->mpBtkAnm, ptr(modelData), frame);
    mDoExt_modelUpdateDL(waterfall->mModel.get(), 0);
    return true;
}
VERIFY(0x0234B758, gnndemotakie_draw);
s32 gnndemotakie_Mthd_Create(daObjGnntakie_c* waterfall) {
    WWHD_FUNC(0x0234B7C8, s32, waterfall);
    return gnndemotakie_create(waterfall);
}
VERIFY(0x0234B7C8, gnndemotakie_Mthd_Create);
bool gnndemotakie_Mthd_Delete(daObjGnntakie_c* waterfall) {
    WWHD_FUNC(0x0234B7CC, bool, waterfall);
    return gnndemotakie_delete(waterfall);
}
VERIFY(0x0234B7CC, gnndemotakie_Mthd_Delete);
bool gnndemotakie_Mthd_Execute(daObjGnntakie_c* waterfall) {
    WWHD_FUNC(0x0234B7D0, bool, waterfall);
    return gnndemotakie_execute(waterfall);
}
VERIFY(0x0234B7D0, gnndemotakie_Mthd_Execute);
bool gnndemotakie_Mthd_Draw(daObjGnntakie_c* waterfall) {
    WWHD_FUNC(0x0234B7D4, bool, waterfall);
    return gnndemotakie_draw(waterfall);
}
VERIFY(0x0234B7D4, gnndemotakie_Mthd_Draw);
void gnndemotakie_staticInit() {
    WWHD_FUNC(0x0234B7D8, void);
    write<u32>(0x10469C88, 0);
    write<u32>(0x10469C80, 0);
    write<u32>(0x10469C8C, 0);
    write<u32>(0x10469C84, 0);
    gabi::call(0x028F026C, ptr(0x101C98CC));
    write<f32>(0x10469C74, -3.1415927410125732f);
    write<f32>(0x10469C78, 3.1415927410125732f);
    gabi::call(0x028ED6F8, ptr(0x10469C7C));
    gabi::call(0x028F026C, ptr(0x101C98D8));
    gabi::call(0x028EAB2C, ptr(0x10469C7D));
    gabi::call(0x028F026C, ptr(0x101C98E4));
}
VERIFY(0x0234B7D8, gnndemotakie_staticInit);
void gnndemotakie_staticDtor(void* object, u32 flags) {
    WWHD_FUNC(0x0234B86C, void, object, flags);
    if (object && (flags & 1)) gabi::call(0x0273AF40, object);
}
VERIFY(0x0234B86C, gnndemotakie_staticDtor);
void gnndemotakie_destructor(daObjGnntakie_c* waterfall, u32 flags) {
    WWHD_FUNC(0x0234B880, void, waterfall, flags);
    if (waterfall) {
        gabi::call(0x025D50BC, waterfall, 0);
        if (flags & 1) gabi::call(0x0273AF40, waterfall);
    }
}
VERIFY(0x0234B880, gnndemotakie_destructor);
void gnndemotakie_staticNoop(void* object) { WWHD_FUNC(0x0234B8D4, void, object); }
VERIFY(0x0234B8D4, gnndemotakie_staticNoop);
BOOL gnndemotakie_IsDelete(daObjGnntakie_c* waterfall) {
    WWHD_FUNC(0x0234B8D8, BOOL, waterfall);
    return 1;
}
VERIFY(0x0234B8D8, gnndemotakie_IsDelete);
