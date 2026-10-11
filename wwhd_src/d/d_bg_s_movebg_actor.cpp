/** Moving-background actor lifecycle and collision matrix. */
#include "d/d_bg_s_movebg_actor.h"

static const f32 headerPiMin = -3.1415927;
static const f32 headerPiMax = 3.1415927;
// HD: per-unit vtable and resource-name type descriptor.
enum : u32 { actorVtable = 0x10043BC4, resourceNameType = 0x10043BAC };
#define moveBgSetup (*gabi::at<MoveBgSetup>(0x101D5300))
#define resourceController (*gabi::at<gptr<void>>(0x101F4F28))
#define matrixStack (*gabi::at<Mtx34>(0x1048D0CC))

/* inlined into MoveBGCreate and MoveBGExecute */
static void setBgMtx(dBgS_MoveBgActor *actor) {
    f32 x = actor->current.pos.x, z = actor->current.pos.z, y = actor->current.pos.y;
    PSMTXTrans(&matrixStack, x, y, z);
    s32 angle = actor->shape_angle.y;
    mDoMtx_YrotM(&matrixStack, angle);
    f32 sy = actor->scale.y, sx = actor->scale.x, sz = actor->scale.z;
    mDoMtx_stack_scaleM(sx, sy, sz);
    PSMTXCopy(&matrixStack, &actor->mBgMtx);
}

/* 024F1D40 */
// HD: GHS allocating constructor and explicit inherited vtable.
static dBgS_MoveBgActor *dBgS_MoveBgActor__ct(dBgS_MoveBgActor *i_this) {
    if (i_this == NULL)
        i_this = (dBgS_MoveBgActor *)operator_new(sizeof(dBgS_MoveBgActor));
    if (i_this != NULL) {
        constructActor(i_this);
        i_this->mpBgW = NULL;
        i_this->__vtbl = actorVtable;
    }
    return i_this;
}

/* 024F1D3C */
static s32 CheckCreateHeap(dBgS_MoveBgActor *i_actor) {
    return i_actor->MoveBGCreateHeap();
}

/* 024F1C44 */
s32 dBgS_MoveBgActor::MoveBGCreateHeap() {
    if (DispatchCreateHeap() == 0)
        return 0;
    dBgW *bg = constructBackground(NULL);
    if (bg != NULL) {
        mpBgW = bg;
        u32 resourceIndex = moveBgSetup.m_dzb_id;
        void *controller = resourceController.get();
        const char *name = moveBgSetup.m_name.get();
        gabi::Stack<MoveBgResourceKey> key;
        key->name = name;
        key->table = resourceNameType;
        void *resource = getObjectResource(controller, key.get(), resourceIndex);
        bg = mpBgW.get();
        if (SetBackground(bg, resource, dBgW::MOVE_BG_e, &mBgMtx) == 0) {
            u32 callback = moveBgSetup.m_set_func;
            if (callback != 0) {
                bg = mpBgW.get();
                bg->m_crr_func = callback;
            }
            return 1;
        }
    }
    mpBgW = NULL;
    return 0;
}

/* 024F1D9C */
s32 dBgS_MoveBgActor::MoveBGCreate(const char *i_arcName, u32 i_dzb_id, u32 i_setFunc,
                                   u32 i_heapSize) {
    setBgMtx(this);
    moveBgSetup.m_name = i_arcName;
    moveBgSetup.m_dzb_id = i_dzb_id;
    moveBgSetup.m_set_func = i_setFunc;
    if (fopAcM_entrySolidHeap(this, CheckCreateHeap, i_heapSize) != 0) {
        MoveBgGameInfo *game = dComIfGp_getGameInfo();
        dBgW *bg = mpBgW.get();
        if (RegisterBackground(&game->mBgS, bg, this) == 0 && DispatchCreate() != 0)
            return cPhs_COMPLEATE_e;
    }
    return cPhs_ERROR_e;
}

/* 024F1F64 */
s32 dBgS_MoveBgActor::MoveBGDelete() {
    s32 result = DispatchDelete();
    dBgW *bg = mpBgW.get();
    if (bg != NULL && (u32)bg->m_id < 0x100) {
        MoveBgGameInfo *game = dComIfGp_getGameInfo();
        bg = mpBgW.get();
        ReleaseBackground(&game->mBgS, bg);
    }
    return result;
}

/* 024F1E9C */
s32 dBgS_MoveBgActor::MoveBGExecute() {
    gabi::Stack<be<u32>> output;
    *output = 0;
    s32 result = DispatchExecute(output.get());
    u32 matrix = *output;
    if (matrix == 0)
        setBgMtx(this);
    else
        PSMTXCopy(gabi::at<Mtx34>(matrix), &mBgMtx);
    MoveBackground(mpBgW.get());
    return result;
}

// HD: compiler-generated base entries have no live arguments.
/* 024F207C */
static s32 dBgS_MoveBgActor_CreateHeap() {
    return 1;
}
/* 024F2084 */
static s32 dBgS_MoveBgActor_Create() {
    return 1;
}
/* 024F208C */
static s32 dBgS_MoveBgActor_Draw() {
    return 1;
}
/* 024F2094 */
static s32 dBgS_MoveBgActor_Execute() {
    return 1;
}
/* 024F209C */
static s32 dBgS_MoveBgActor_IsDelete() {
    return 1;
}
/* 024F20A4 */
static s32 dBgS_MoveBgActor_Delete() {
    return 1;
}

/* 024F2068 */
// HD: deleting destructor for the empty base tests the low GHS flag bit.
static void dBgS_MoveBgActor_empty__dt(dBgS_MoveBgActor *i_this, u32 flags) {
    if (i_this != NULL && (flags & 1))
        operator_delete(i_this);
}

/* 024F20AC */
static void dBgS_MoveBgActor__dt(dBgS_MoveBgActor *i_this, u32 flags) {
    if (i_this != NULL) {
        destroyActor(i_this, 0);
        if (flags & 1)
            operator_delete(i_this);
    }
}

// HD: compiler-generated initialization of this translation unit's header statics.
struct HeaderStatics {
    be<f32> piMin, piMax;
    u8 byteObject, flagObject;
    u8 padding[2];
};
struct HeaderZeroObject {
    be<u32> words[4];
};
#define headerStatics (*gabi::at<HeaderStatics>(0x1046ED7C))
#define headerZeroObject (*gabi::at<HeaderZeroObject>(0x1046ED88))
/* 024F1FD4 */
static void __sinit_d_bg_s_movebg_actor_cpp() {
    headerZeroObject.words[2] = 0;
    headerZeroObject.words[0] = 0;
    headerZeroObject.words[3] = 0;
    headerZeroObject.words[1] = 0;
    registerGlobalObject(gabi::at<void>(0x101D52DC));
    f32 lo = headerPiMin, hi = headerPiMax;
    headerStatics.piMin = lo;
    headerStatics.piMax = hi;
    initializeHeaderByte(&headerStatics.byteObject);
    registerGlobalObject(gabi::at<void>(0x101D52E8));
    initializeHeaderFlag(&headerStatics.flagObject);
    registerGlobalObject(gabi::at<void>(0x101D52F4));
}

/* WWHD glue (tools/verify/autoglue.py)
data headerPiMin 10043C1C
data headerPiMax 10043C20
ext 0273AD10 void* operator_new(u32)
ext 0273AF40 void operator_delete(void*)
ext 025D4ED0 void constructActor(dBgS_MoveBgActor*)
ext 025D50BC void destroyActor(dBgS_MoveBgActor*,u32)
ext 024F23F4 dBgW* constructBackground(dBgW*)
ext 026066C4 void* getObjectResource(void*,MoveBgResourceKey*,u32)
ext 0200A030 s32 SetBackground(dBgW*,void*,u32,Mtx34*)
ext 025D63E8 s32 fopAcM_entrySolidHeap(dBgS_MoveBgActor*,MoveBgHeapCallback,u32)
ext 025200D4 MoveBgGameInfo* dComIfGp_getGameInfo()
ext 024EEA6C s32 RegisterBackground(void*,dBgW*,dBgS_MoveBgActor*)
ext 020087EC void ReleaseBackground(void*,dBgW*)
ext 024F43DC void MoveBackground(dBgW*)
ext 028E93CC void PSMTXTrans(Mtx34*,f32,f32,f32)
ext 025F1C28 void mDoMtx_YrotM(Mtx34*,s32)
ext 025F2518 void mDoMtx_stack_scaleM(f32,f32,f32)
ext 028E90D4 void PSMTXCopy(const Mtx34*,Mtx34*)
ext 028F026C void registerGlobalObject(void*)
ext 028ED6F8 void initializeHeaderByte(void*)
ext 028EAB2C void initializeHeaderFlag(void*)
*/
