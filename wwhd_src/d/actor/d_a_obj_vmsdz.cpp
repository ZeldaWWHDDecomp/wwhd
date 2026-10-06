/* WWHD port of zeldaret/tww d_a_obj_vmsdz.cpp.
 * Full HD TU: 023B346C through 023B37DC. Adjacent named units are
 * d_a_obj_vmc (ends 023B3468) and d_a_obj_vmsms (starts 023B37E0).
 */
#include "d/actor/d_a_obj_vmsdz.h"

BOOL daObjVmsdz_c::create_heap() {
    WWHD_FUNC(0x023B346C, BOOL, this);
    gabi::Local<SafeString> name;
    name->__vtbl = 0x10032E6C;
    name->mStringTop = 0x10032EC8;
    void* data = gabi::call<void*>(0x026066C4,
        gabi::at<u8>(gabi::load<u32>(0x101F4F28)), name.get(), 3);
    if (!data) {
        gabi::call(0x0273AA24, STR(0x10032E94), 0x59, STR(0x10032EA8));
        return FALSE;
    }
    mModel = gabi::call<J3DModel*>(0x025E38E0, data, 0, 0x11020203);
    return mModel != nullptr;
}
VERIFY(0x023B346C, &daObjVmsdz_c::create_heap);

static BOOL solidHeapCB(daObjVmsdz_c* actor) {
    WWHD_FUNC(0x023B3514, BOOL, actor);
    return actor->create_heap();
}
VERIFY(0x023B3514, solidHeapCB);

void daObjVmsdz_c::init_mtx() {
    WWHD_FUNC(0x023B3518, void, this);
    u32 model = gabi::ea((J3DModel*)mModel);
    f32 z = scale.z, x = scale.x, y = scale.y;
    gabi::store<f32>(model + 0xBC, x);
    gabi::store<f32>(model + 0xC4, z);
    gabi::store<f32>(model + 0xC0, y);
}
VERIFY(0x023B3518, &daObjVmsdz_c::init_mtx);

s32 daObjVmsdz_c::_create() {
    WWHD_FUNC(0x023B3538, s32, this);
    u32 base = gabi::ea(this), flags = gabi::load<u32>(base + 0x2E4);
    if (!(flags & 8)) {
        if (this) {
            gabi::call(0x025D4ED0, this);
            flags = gabi::load<u32>(base + 0x2E4);
            gabi::store<u32>(base + 0xB4, 0x10032E84);
        }
        gabi::store<u32>(base + 0x2E4, flags | 8);
    }
    s32 phase = gabi::call<s32>(0x02520460, &mPhs, STR(0x10032EC8));
    if (phase == 4) {
        if (!gabi::call<s32>(0x025D63E8, this, 0x023B3514, 0))
            return 5;
        u32 model = gabi::ea((J3DModel*)mModel);
        gabi::store<u32>(base + 0x348, model ? model + 0xC8 : 0);
        init_mtx();
        return 4;
    }
    return phase;
}
VERIFY(0x023B3538, &daObjVmsdz_c::_create);

BOOL daObjVmsdz_c::_delete() {
    WWHD_FUNC(0x023B35FC, BOOL, this);
    gabi::call(0x025204C8, &mPhs, STR(0x10032EC8));
    return TRUE;
}
VERIFY(0x023B35FC, &daObjVmsdz_c::_delete);

BOOL daObjVmsdz_c::_draw() {
    WWHD_FUNC(0x023B362C, BOOL, this);
    u32 light = gabi::call<u32>(0x02555D0C);
    gabi::call(0x025626A4, gabi::at<u8>(light), 1, &current.pos,
        gabi::at<u8>(gabi::ea(this) + 0x110));
    light = gabi::call<u32>(0x02555D0C);
    gabi::call(0x02562F5C, gabi::at<u8>(light), (J3DModel*)mModel,
        gabi::at<u8>(gabi::ea(this) + 0x110));
    u32 play = gabi::call<u32>(0x025200D4);
    gabi::store<u32>(0x104B4634, gabi::load<u32>(play + 0x5D70));
    play = gabi::call<u32>(0x025200D4);
    gabi::store<u32>(0x104B4638, gabi::load<u32>(play + 0x5D74));
    gabi::call(0x025E2DE0, (J3DModel*)mModel, 0);
    play = gabi::call<u32>(0x025200D4);
    gabi::store<u32>(0x104B4634, gabi::load<u32>(play + 0x5D78));
    play = gabi::call<u32>(0x025200D4);
    gabi::store<u32>(0x104B4638, gabi::load<u32>(play + 0x5D7C));
    return TRUE;
}
VERIFY(0x023B362C, &daObjVmsdz_c::_draw);

static s32 Mthd_Create(daObjVmsdz_c* actor) {
    WWHD_FUNC(0x023B36C4, s32, actor);
    return actor->_create();
}
VERIFY(0x023B36C4, Mthd_Create);
static BOOL Mthd_Delete(daObjVmsdz_c* actor) {
    WWHD_FUNC(0x023B36C8, BOOL, actor);
    return actor->_delete();
}
VERIFY(0x023B36C8, Mthd_Delete);
static BOOL Mthd_Execute(daObjVmsdz_c* actor) {
    WWHD_FUNC(0x023B36CC, BOOL, actor);
    return TRUE;
}
VERIFY(0x023B36CC, Mthd_Execute);
static BOOL Mthd_Draw(daObjVmsdz_c* actor) {
    WWHD_FUNC(0x023B36D4, BOOL, actor);
    return actor->_draw();
}
VERIFY(0x023B36D4, Mthd_Draw);

static void staticInitialize() {
    WWHD_FUNC(0x023B36D8, void);
    gabi::store<u32>(0x1046C8C4, 0);
    gabi::store<u32>(0x1046C8BC, 0);
    gabi::store<u32>(0x1046C8C8, 0);
    gabi::store<u32>(0x1046C8C0, 0);
    gabi::call(0x028F026C, gabi::at<u8>(0x101CDABC));
    gabi::store<f32>(0x1046C8B0, -3.1415927410125732f);
    gabi::store<f32>(0x1046C8B4, 3.1415927410125732f);
    gabi::call(0x028ED6F8, gabi::at<u8>(0x1046C8B8));
    gabi::call(0x028F026C, gabi::at<u8>(0x101CDAC8));
    gabi::call(0x028EAB2C, gabi::at<u8>(0x1046C8B9));
    gabi::call(0x028F026C, gabi::at<u8>(0x101CDAD4));
}
VERIFY(0x023B36D8, staticInitialize);
static void deleteStatic(void* p, s32 flags) {
    WWHD_FUNC(0x023B376C, void, p, flags);
    if (p && (flags & 1)) gabi::call(0x0273AF40, p);
}
VERIFY(0x023B376C, deleteStatic);
static void destruct(daObjVmsdz_c* actor, s32 flags) {
    WWHD_FUNC(0x023B3780, void, actor, flags);
    if (actor) {
        gabi::call(0x025D50BC, actor, 0);
        if (flags & 1) gabi::call(0x0273AF40, actor);
    }
}
VERIFY(0x023B3780, destruct);
static void emptyVirtual() { WWHD_FUNC(0x023B37D4, void); }
VERIFY(0x023B37D4, emptyVirtual);
static BOOL Mthd_IsDelete(daObjVmsdz_c* actor) {
    WWHD_FUNC(0x023B37D8, BOOL, actor);
    return TRUE;
}
VERIFY(0x023B37D8, Mthd_IsDelete);
