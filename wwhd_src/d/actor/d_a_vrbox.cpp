/**
 * d_a_vrbox.cpp (WWHD)
 * Sky box (vr_sky): sky and horizon-haze colours, dungeon rain/thunder.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_vrbox.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 *
 * HD rewrote Draw like d_a_vrbox2's: daVrbox_color_set (inlined) looks materials up by name and
 * writes HD constant-colour registers, the "A_mori" stage raises the sky, and the model is drawn
 * through an HD layer with its own projection (near 0.0001, far 16000). Create first waits for
 * the "Stage" archive.
 */
#include "d/actor/d_a_vrbox.h"
#include "gc_api/JSystem/J3DModelData.h"
#include "gc_api/JSystem/JUTAssert.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/f_op/f_op_actor_mng.h"
#include "gc_api/tu.h"

using namespace gabi;
const gc_tu_data gc_tu = {
    .safeString = 0x10041CB0,
    .safeStringAssure = 0x024D1C40,
    .headerStatics = {0x1046EAA0, 0x101D2B8C},
};
enum : u32 { vrbox_class__vt = 0x10041CF8, projectionRTTI = 0x10041CE8 };
enum { DUNGEON_CLEAR, DUNGEON_RAIN_THUNDER, DUNGEON_THUNDER_ONLY };
enum { SKY_COLOR_REGISTER = 7, SKY_CONSTANT_COLOR = 0x80, LAYER_OVERRIDE_PROJECTION = 0x1000 };
using RendererPointer = gptr<VrboxRenderer>;
static RendererPointer sRenderer;
static VrboxProjection sDefaultProjection;
static u32 sProjectionRTTIGuard;
static u32 sProjectionRTTI;

extern void Color4f_scaleRGB(VrboxColor4f*, VrboxColor4f*, f32);
extern VrboxColor4f* matColorReg(be<u32>*, s32);
extern void SkyProjection_ct(VrboxProjection*);
extern void SkyProjection_set(VrboxProjection*, f32, f32, f32, f32);
extern void SkyProjection_copy(VrboxProjection*, be<u32>*);
extern void SkyProjection_dt(VrboxProjection*, s32);
extern BOOL dKy_checkEventNightStop();
extern void dKy_change_colpat(u8);
extern void dKyw_rain_set(s32);
extern void Color4f_setGXColor(VrboxColor4f*, GXColor*);
extern void SafeString_assureNative(SafeString*);
extern void vrboxActorDestroyNative(vrbox_class*, s32);

static VrboxEnvironment* vrboxEnv() {
    return (VrboxEnvironment*)dKy_getEnvlight();
}
// HD: the view pointer is beyond the shared play layout; retain the call on each access.
static VrboxView* vrboxView() {
    return ((VrboxGameView*)dComIfGp_get())->mpView;
}

/* sead::SafeStringBase<char>::isEqual (HD inline): a literal vs the start stage name (play+0x5134)
 */
/* inlined into daVrbox_Draw */
static bool startStageIs(const char* name) {
    gabi::Stack<SafeString> lhs;
    lhs->mStringTop = gabi::guest_addr(name);
    lhs->__vtbl = gc_tu.safeString;
    const char* stage = dComIfGp_getStartStageName();
    gabi::Stack<SafeString> rhs;
    rhs->mStringTop = ea(stage);
    rhs->__vtbl = gc_tu.safeString;
    gabi::call_ptr(at<VrboxStringVtable>(lhs->__vtbl)->assureTermination.get(),
                   lhs.get()); /* assureTerminationImpl_ */
    gabi::call_ptr(at<VrboxStringVtable>(lhs->__vtbl)->assureTermination.get(),
                   lhs.get()); /* cstr() */
    u32 a = lhs->mStringTop;
    gabi::call_ptr(at<VrboxStringVtable>(rhs->__vtbl)->assureTermination.get(),
                   rhs.get()); /* str.cstr() */
    if (a == rhs->mStringTop) {
        return true;
    }
    u32 b = rhs->mStringTop;
    a = lhs->mStringTop;
    for (u32 i = 0; i < 0x40001; i++) {
        u8 c = (u8)*at<be<u8>>(a + i);
        if (c != (u8)*at<be<u8>>(b + i)) {
            return false;
        }
        if (c == 0) {
            return true;
        }
    }
    return false;
}

/* mat->setTevKColor(0, k) (virtual on the material's TEV block) and the HD constant colour
 * register 7 = rgb(k) * scale, alpha k.a */
/* inlined into daVrbox_Draw */
static void setKColor(VrboxMaterial* mat, GXColor* k, f32 scale) {
    VrboxTevBlock* tev = mat->mpTevBlock;
    call_ptr(tev->__vtbl->setTevKColor.get(), tev, 0, k);
    gabi::Stack<VrboxColor4f> c;
    Color4f_setGXColor(c.get(), k);
    gabi::Stack<VrboxColor4f> scaled;
    Color4f_scaleRGB(scaled.get(), c.get(), scale);
    mat->mConstantColorFlags = (u32)mat->mConstantColorFlags | SKY_CONSTANT_COLOR;
    VrboxColor4f* reg = matColorReg(&mat->mConstantColorFlags, SKY_COLOR_REGISTER);
    f32 alpha = (f32)(u8)k->a / 255.0f;
    // GHS copies these float words without quieting SNaNs.
    reg->mChannelBits[0] = scaled->mChannelBits[0];
    reg->mChannelBits[1] = scaled->mChannelBits[1];
    reg->mChannelBits[2] = scaled->mChannelBits[2];
    reg->a = alpha;
}

/* daVrbox_color_set (HD, inlined into Draw) */
/* inlined into daVrbox_Draw */
static void daVrbox_color_set(vrbox_class* i_this) {
    VrboxEnvironment* e1 = vrboxEnv();
    VrboxEnvironment* e2 = vrboxEnv();
    s32 sum = e1->mVrKasumiMaeColor.r + e2->mVrKasumiMaeColor.g; /* mVrKasumiMaeColor */
    sum += vrboxEnv()->mVrKasumiMaeColor.b;
    sum += vrboxEnv()->mVrSkyColor.r; /* mVrSkyColor */
    sum += vrboxEnv()->mVrSkyColor.g;
    sum += vrboxEnv()->mVrSkyColor.b;
    sum += vrboxEnv()->mVrkumoColor.r; /* mVrkumoColor */
    sum += vrboxEnv()->mVrkumoColor.g;
    sum += vrboxEnv()->mVrkumoColor.b;
    if (sum == 0) {
        vrboxEnv()->mbVrboxInvisible = 1; /* mbVrboxInvisible */
        return;
    }
    vrboxEnv()->mbVrboxInvisible = 0;

    J3DModelData* modelData = i_this->mpModel->getModelData();
    gabi::Stack<GXColor> k;

    VrboxMaterial* mat = (VrboxMaterial*)modelData->getMaterialNodePointer("lambert1_v_x");
    k->r = vrboxEnv()->mVrKasumiMaeColor.r;
    k->g = vrboxEnv()->mVrKasumiMaeColor.g;
    VrboxEnvironment* e = vrboxEnv();
    k->a = 0xFF;
    k->b = e->mVrKasumiMaeColor.b;
    f32 scale = (f32)vrboxEnv()->mVrKasumiMaeColorScale;
    setKColor(mat, k.get(), scale);

    mat = (VrboxMaterial*)modelData->getMaterialNodePointer("sora_v");
    k->r = vrboxEnv()->mVrSkyColor.r;
    k->g = vrboxEnv()->mVrSkyColor.g;
    e = vrboxEnv();
    k->a = 0xFF;
    k->b = e->mVrSkyColor.b;
    scale = (f32)vrboxEnv()->mVrSkyColorScale;
    setKColor(mat, k.get(), scale);
}

/* 024D0C50 */
static BOOL daVrbox_Draw(vrbox_class* i_this) {
    J3DModel* model = i_this->mpModel;
    f32 y_origin = 0.0f;

    daVrbox_color_set(i_this);

    if ((u8)vrboxEnv()->mbVrboxInvisible != 0) { /* g_env_light.mbVrboxInvisible */
        return TRUE;
    }

    s32 roomNo = dComIfGp_roomControl_getStayNo(); /* dComIfGp_roomControl_getStayNo() */
    if (roomNo >= 0) {
        VrboxRoom* rc = (VrboxRoom*)dComIfGp_roomControl_getStatusRoomDt(roomNo);
        if (rc != nullptr) {
            VrboxFileList* fili = call_ptr<VrboxFileList*>(rc->__vtbl->getFileListInfo.get(),
                                                           rc); /* getFileListInfo() */
            if (fili != nullptr) {
                y_origin = (f32)fili->mSeaLevel; /* mSeaLevel */
            }
        }
    }

    f32 y_offset;
    if (vrboxView() != 0) { /* dComIfGd_getView() */
        VrboxView* view = vrboxView();
        y_offset = ((f32)view->mInvViewMtx.m[1][3] - y_origin) * 0.09f;
    } else {
        y_offset = 0.0f;
    }

    /* HD: the sky of Forest Haven is raised */
    if (startStageIs("A_mori")) {
        y_offset += 30000.0f;
    }

    {
        /* mDoMtx_stack_c::transS(view->mInvViewMtx[0][3], [1][3] - y_offset, [2][3]) */
        f32 x = (f32)vrboxView()->mInvViewMtx.m[0][3];
        f32 y = (f32)vrboxView()->mInvViewMtx.m[1][3] - y_offset;
        f32 z = (f32)vrboxView()->mInvViewMtx.m[2][3];
        mDoMtx_stack_c::transS(x, y, z);
    }
    J3DModel_setBaseTRMtx(model, mDoMtx_stack_c::get());

    /* HD: draw through the sky layer with its own projection */
    VrboxRenderer* mgr = sRenderer;
    u32 n = mgr->mLayerCount;
    gptr<VrboxLayer>* layers = mgr->mpLayers;
    if (n > 1)
        layers++;
    VrboxLayer* layer = *layers;
    if (layer != nullptr) {
        u32 flags = layer->mFlags;
        VrboxProjection* saved = layer->mpProjection;
        VrboxProjection* cam;
        if ((flags & LAYER_OVERRIDE_PROJECTION) == 0)
            cam = saved != nullptr ? saved : &sDefaultProjection;
        else
            cam = layer->mpOverrideProjection;
        gabi::Stack<VrboxProjection> proj;
        SkyProjection_ct(proj.get());
        SkyProjection_set(proj.get(), cam->mNear, cam->mFar, cam->mFovy, cam->mAspect);
        proj->mOffsetBits[0] = cam->mOffsetBits[0];
        proj->mOffsetBits[1] = cam->mOffsetBits[1];
        proj->mDirty = 1;
        if (sProjectionRTTIGuard == 0) {
            sProjectionRTTIGuard = 1;
            sProjectionRTTI = projectionRTTI;
        }
        if (call_ptr<u32>(cam->__vtbl->checkDerived.get(), cam, &sProjectionRTTI) != 0 &&
            cam != nullptr) {
            SkyProjection_copy(proj.get(), cam->mFrustum);
        }
        proj->mNear = 0.0001f;
        proj->mFar = 16000.0f;
        proj->mDirty = 1;
        layer->mpProjection = proj.get();
        dComIfGd_setListSky();
        mDoExt_modelUpdateDL(model);
        layer->mpProjection = saved;
        SkyProjection_dt(proj.get(), 2);
    }

    dComIfGd_setList();
    return TRUE;
}

/* dungeon_rain_proc (inlined in Execute) */
static inline void dungeon_rain_proc() {
    VrboxEnvironment* env_light = vrboxEnv();
    u8 mode = DUNGEON_CLEAR;
    s32 roomNo = dComIfGp_roomControl_getStayNo(); /* dComIfGp_roomControl_getStayNo() */

    if (!dKy_checkEventNightStop()) {
        return;
    }

    if (startStageIs("M_NewD2")) {
        if (roomNo == 3) {
            mode = DUNGEON_RAIN_THUNDER;
        }
    } else if (startStageIs("M_Dra09")) {
        mode = DUNGEON_RAIN_THUNDER;
    } else if (startStageIs("kinMB")) {
        mode = DUNGEON_RAIN_THUNDER;
    } else if (startStageIs("kindan")) {
        if (roomNo == 2 || roomNo == 13) {
            mode = DUNGEON_RAIN_THUNDER;
        } else if (roomNo == 4) {
            mode = DUNGEON_THUNDER_ONLY;
        }
    } else {
        return;
    }

    if (mode == DUNGEON_RAIN_THUNDER) {              /* rain and thunder */
        if ((s32)env_light->mRainCountOrig != 250) { /* mRainCountOrig */
            dKy_change_colpat(1);
            dKyw_rain_set(250);
            vrboxEnv()->mThunderEff.mMode = 1; /* mThunderEff.mMode */
        }
    } else if (mode == DUNGEON_THUNDER_ONLY) { /* thunder, no rain */
        if ((s32)vrboxEnv()->mThunderEff.mMode == 0) {
            dKy_change_colpat(1);
            vrboxEnv()->mThunderEff.mMode = 0xA;
        }
    } else { /* no rain or thunder */
        if ((s32)vrboxEnv()->mThunderEff.mMode != 0) {
            dKyw_rain_set(0);
            vrboxEnv()->mThunderEff.mMode = 0;
        }
    }
}

/* 024D15BC */
static BOOL daVrbox_Execute(vrbox_class* i_this) {
    dungeon_rain_proc();
    return TRUE;
}

/* 024D1988 */
static BOOL daVrbox_IsDelete(vrbox_class* i_this) {
    return TRUE;
}

/* 024D1990 */
static BOOL daVrbox_Delete(vrbox_class* i_this) {
    return TRUE;
}

/* 024D1998 */
static BOOL daVrbox_solidHeapCB(fopAc_ac_c* i_actor) {
    vrbox_class* i_this = static_cast<vrbox_class*>(i_actor);

    J3DModelData* modelData = (J3DModelData*)dComIfG_getStageRes("Stage", "vr_sky.bmd");
    if (modelData == 0) /* JUT_ASSERT(0x219, modelData != NULL) */
        JUTAssertion::showAssert("d_a_vrbox.cpp", 0x219, "modelData != (0)");

    i_this->mpModel = mDoExt_J3DModel__create(modelData, 0x80000, 0x11020202);

    bool success = FALSE;
    if (modelData != 0 && i_this->mpModel != nullptr) {
        success = TRUE;
    }
    return success;
}

/* 024D1A30 */
static cPhs_State daVrbox_Create(fopAc_ac_c* i_actor) {
    /* HD: wait for the stage archive first */
    s32 rt = dComIfG_syncStageRes("Stage");
    if (rt < 0) {
        return cPhs_ERROR_e;
    }
    if (rt > 0) {
        return cPhs_INIT_e;
    }

    /* fopAcM_ct(i_actor, vrbox_class) */
    if (!fopAcM_CheckCondition(i_actor, fopAcCnd_INIT_e)) {
        if (i_actor != nullptr) {
            fopAc_ac_c_ct(i_actor);
            i_actor->__vtbl = vrbox_class__vt;
        }
        fopAcM_OnCondition(i_actor, fopAcCnd_INIT_e);
    }
    vrbox_class* i_this = static_cast<vrbox_class*>(i_actor);

    cPhs_State phase_state = cPhs_COMPLEATE_e;
    i_this->m29C = 0;
    if (fopAcM_entrySolidHeap(i_this, daVrbox_solidHeapCB, 0xC60)) {
        dComIfG_inf_c* info = (dComIfG_inf_c*)dComIfGp_get();
        info->play.mStatus = (u16)info->play.mStatus | 1;
        vrboxEnv()->mbVrboxInvisible = 0; /* g_env_light.mbVrboxInvisible */
    } else {
        phase_state = cPhs_ERROR_e;
    }
    return phase_state;
}

/* 024D1B44 */
static void __sinit_d_a_vrbox_cpp() {
    sinit_header_statics(gc_tu.headerStatics[0], gc_tu.headerStatics[1]);
}

/* 024D1BD8 */
static void SafeString_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1)) {
        operator_delete(p);
    }
}

/* 024D1BEC */
static void vrbox_class_dt(vrbox_class* i_this, s32 flags) {
    if (i_this != nullptr) {
        vrboxActorDestroyNative(i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 024D1C40 */
static void SafeString_assureTerminationImpl(void*) {}

/* WWHD glue
str 10041D08
str 10041CA0
str 10041D28
str 10041C84
str 10041C8C
str 10041C98
str 10041CA8
str 10041D30 in 024D1998
str 10041D68 in 024D1A30
str 10041D48
str 10041D38
str 10041D54
data sRenderer 101F95D0
data sDefaultProjection 104A20FC
data sProjectionRTTIGuard 101FD9F0
data sProjectionRTTI 101FDCD4
ext 0274D458 void Color4f_scaleRGB(VrboxColor4f*, VrboxColor4f*, f32)
ext 027F9F0C VrboxColor4f* matColorReg(be<u32>*, s32)
ext 027389F8 void SkyProjection_ct(VrboxProjection*)
ext 0274E04C void SkyProjection_set(VrboxProjection*, f32, f32, f32, f32)
ext 02738AC0 void SkyProjection_copy(VrboxProjection*, be<u32>*)
ext 02738A6C void SkyProjection_dt(VrboxProjection*, s32)
ext 02556BC0 BOOL dKy_checkEventNightStop()
ext 0255FD48 void dKy_change_colpat(u8)
ext 0257E7C0 void dKyw_rain_set(s32)
ext 024D0B9C void Color4f_setGXColor(VrboxColor4f*, GXColor*)
ext 024D1C40 void SafeString_assureNative(SafeString*)
ext 025D50BC void vrboxActorDestroyNative(vrbox_class*, s32)
*/
