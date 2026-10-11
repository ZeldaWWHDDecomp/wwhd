/**
 * d_a_vrbox2.cpp (WWHD)
 * Sky box: back clouds, horizon haze ("kasumi mae") and the false sea ("uso umi").
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_vrbox2.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 *
 * HD rewrote most of this actor: the wind/scroll computation of daVrbox2_color_set moved to
 * Execute (result kept in an HD member), the material updates (inlined into Draw) look up
 * materials and texture-SRT animations by name and write HD constant-colour registers, and Draw
 * renders through an HD layer with its own projection (sky near/far), with stage-specific
 * height offsets.
 */
#include "d/actor/d_a_vrbox2.h"
#include "gc_api/JSystem/J3DModelData.h"
#include "gc_api/JSystem/JUTAssert.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/d/d_kankyo_wether.h"
#include "gc_api/f_op/f_op_actor_mng.h"
#include "gc_api/tu.h"

using namespace gabi;
const gc_tu_data gc_tu = {
    .safeString = 0x10041D9C,
    .safeStringAssure = 0x024D3BD0,
    .headerStatics = {0x1046EABC, 0x101D2C00},
};
enum : u32 { vrbox2_class__vt = 0x10041DF4, projectionRTTI = 0x10041DE4, layerRTTI = 0x10041DD4 };
enum {
    SKY_COLOR_REGISTER = 7,
    SKY_CONSTANT_COLOR = 0x80,
    HAZE_COLOR_REGISTER = 4,
    HAZE_CONSTANT_COLOR = 0x10,
    LAYER_OVERRIDE_PROJECTION = 0x1000,
    STAGE_TYPE_MISC = 2
};
using Vrbox2RendererPointer = gptr<Vrbox2Renderer>;
using Vrbox2SavePointer = gptr<Vrbox2SaveWind>;
using Vrbox2IndexPointer = gptr<be<s16>>;
static Vrbox2RendererPointer sRenderer;
static Vrbox2SavePointer sSave;
static Vrbox2IndexPointer sLayerIndex;
static Vrbox2LayerRegistry sLayerRegistry;
static Vrbox2Projection sDefaultProjection;
static u32 sProjectionRTTIGuard;
static u32 sProjectionRTTI;
static u32 sLayerRTTIGuard;
static u32 sLayerRTTI;

extern s32 vrbox2NameIndex(JUTNameTab*, const char*);
extern void vrbox2VectorDirection(cXyz*, cXyz*, cXyz*);
extern f32 vrbox2VectorProduct2d(f32, f32, f32, f32, f32, f32);
extern Vrbox2TexSrt* vrbox2TexSrt(Vrbox2MaterialAnimation*, s32);
extern void vrbox2ScaleColor(Vrbox2Color4f*, Vrbox2Color4f*, f32);
extern Vrbox2Color4f* vrbox2ColorRegister(be<u32>*, s32);
extern void vrbox2ProjectionConstruct(Vrbox2Projection*);
extern void vrbox2ProjectionSet(Vrbox2Projection*, f32, f32, f32, f32);
extern void vrbox2ProjectionCopy(Vrbox2Projection*, be<u32>*);
extern void vrbox2ProjectionDestroy(Vrbox2Projection*, s32);
extern void vrbox2ColorFromGX(Vrbox2Color4f*, GXColor*);
extern void vrbox2ActorDestroy(vrbox2_class*, s32);
extern void vrbox2RestoreSeaLayer();

static Vrbox2Environment* vrbox2Env() {
    return (Vrbox2Environment*)dKy_getEnvlight();
}
// HD: retain the play accessor call at every view access.
static Vrbox2View* vrbox2View() {
    return ((Vrbox2GameView*)dComIfGp_get())->mpView;
}

// HD: sead::SafeString equality keeps the three virtual termination calls.
/* inlined into daVrbox2_Draw */
static bool startStageIs(const char* name) {
    gabi::Stack<SafeString> lhs;
    lhs->mStringTop = gabi::guest_addr(name);
    lhs->__vtbl = gc_tu.safeString;
    const char* stage = dComIfGp_getStartStageName();
    gabi::Stack<SafeString> rhs;
    rhs->mStringTop = ea(stage);
    rhs->__vtbl = gc_tu.safeString;
    gabi::call_ptr(at<Vrbox2StringVtable>(lhs->__vtbl)->assureTermination.get(),
                   lhs.get()); /* assureTerminationImpl_ */
    gabi::call_ptr(at<Vrbox2StringVtable>(lhs->__vtbl)->assureTermination.get(),
                   lhs.get()); /* cstr() */
    u32 a = lhs->mStringTop;
    gabi::call_ptr(at<Vrbox2StringVtable>(rhs->__vtbl)->assureTermination.get(),
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

/* 024D1C44 */
void texScrollCheck(be<f32>* v) {
    f32 f = *v;
    if (f < 0.0f) {
        do {
            f += 1.0f;
        } while (f < 0.0f);
        *v = f;
    }
    if (f > 1.0f) {
        do {
            f -= 1.0f;
        } while (f > 1.0f);
        *v = f;
    }
}

// HD: convert packed GX channels to floating-point renderer colors.
/* 024D1C90 */
static void vrbox2SetGXColor(Vrbox2Color4f* out, GXColor* c) {
    f32 r = (f32)(u8)c->r / 255.0f;
    f32 g = (f32)(u8)c->g / 255.0f;
    f32 b = (f32)(u8)c->b / 255.0f;
    f32 a = (f32)(u8)c->a / 255.0f;
    out->r = r;
    out->g = g;
    out->b = b;
    out->a = a;
}

/* ---- Draw helpers (daVrbox2_color_set, HD) ---- */
/* inlined into daVrbox2_Draw */
static bool envColorsZero() {
    Vrbox2Environment* e1 = vrbox2Env();
    Vrbox2Environment* e2 = vrbox2Env();
    s32 sum = (u8)e1->mVrKasumiMaeColor.r + (u8)e2->mVrKasumiMaeColor.g; /* mVrKasumiMaeColor */
    sum += (u8)vrbox2Env()->mVrKasumiMaeColor.b;
    sum += (u8)vrbox2Env()->mVrSkyColor.r; /* mVrSkyColor */
    sum += (u8)vrbox2Env()->mVrSkyColor.g;
    sum += (u8)vrbox2Env()->mVrSkyColor.b;
    sum += (u8)vrbox2Env()->mVrkumoColor.r; /* mVrkumoColor */
    sum += (u8)vrbox2Env()->mVrkumoColor.g;
    sum += (u8)vrbox2Env()->mVrkumoColor.b;
    return sum == 0;
}

// HD: named animation lookup uses the model's separate relative name table.
/* inlined into daVrbox2_Draw */
static Vrbox2MaterialAnimation* getMatAnmByName(J3DModel* model, const char* name) {
    Vrbox2ModelAnimations* animations = (Vrbox2ModelAnimations*)model;
    Vrbox2NameResource* resource = animations->mpNameResource;
    s32 off = resource->mNameTableOffset;
    JUTNameTab* tab = off != 0 ? at<JUTNameTab>(ea(&resource->mNameTableOffset) + off) : nullptr;
    s32 idx = vrbox2NameIndex(tab, name);
    if (idx < 0)
        return nullptr;
    Vrbox2MaterialAnimation* base = animations->mpMaterialAnimations;
    return base + idx;
}

/* inlined into daVrbox2_Draw */
static void scrollTexSrt(vrbox2_class* i_this, Vrbox2MaterialAnimation* anm, s32 i, s32 line) {
    Vrbox2TexSrt* tex_srt = vrbox2TexSrt(anm, i);
    if (tex_srt == 0) /* JUT_ASSERT(line, tex_srt != 0) */
        JUTAssertion::showAssert("d_a_vrbox2.cpp", line, "tex_srt != (0)");
    f32 v = (f32)tex_srt->mTranslationX + i_this->mScrollSpeed; /* mSRT.mTranslationX */
    tex_srt->mTranslationX = v;
    texScrollCheck(&tex_srt->mTranslationX);
}

/* mat->setTevKColor(0, k) (virtual on the material's TEV block) and the HD constant colour
 * register 7 = rgb(k) * scale, alpha k.a */
/* inlined into daVrbox2_Draw */
static void setKColor(Vrbox2Material* mat, GXColor* k, f32 scale) {
    Vrbox2TevBlock* tev = mat->mpTevBlock;
    call_ptr(tev->__vtbl->setTevKColor.get(), tev, 0, k);
    gabi::Stack<Vrbox2Color4f> c;
    vrbox2ColorFromGX(c.get(), k);
    gabi::Stack<Vrbox2Color4f> s;
    vrbox2ScaleColor(s.get(), c.get(), scale);
    mat->mConstantColorFlags = (u32)mat->mConstantColorFlags | SKY_CONSTANT_COLOR;
    Vrbox2Color4f* reg = vrbox2ColorRegister(&mat->mConstantColorFlags, SKY_COLOR_REGISTER);
    f32 a = (f32)(u8)k->a / 255.0f;
    // GHS copies the float bits, including SNaNs.
    reg->mChannelBits[0] = s->mChannelBits[0];
    reg->mChannelBits[1] = s->mChannelBits[1];
    reg->mChannelBits[2] = s->mChannelBits[2];
    reg->a = a;
}

/* inlined into daVrbox2_Draw */
static void cloudMaterial(vrbox2_class* i_this, J3DModelData* md, J3DModel* pBackCloud,
                          const char* matName, const char* anmName, s32 line0, s32 line1,
                          GXColor* k0) {
    Vrbox2Material* mat = (Vrbox2Material*)md->getMaterialNodePointer(matName);
    Vrbox2MaterialAnimation* anm = getMatAnmByName(pBackCloud, anmName);
    scrollTexSrt(i_this, anm, 0, line0);
    scrollTexSrt(i_this, anm, 1, line1);
    f32 scale = (f32)vrbox2Env()->mVrkumoColorScale;
    setKColor(mat, k0, scale);
}

/* daVrbox2_color_set (HD, inlined into Draw) */
/* inlined into daVrbox2_Draw */
static void daVrbox2_color_set(vrbox2_class* i_this, J3DModel* pBackCloud) {
    if (envColorsZero()) {
        return;
    }
    J3DModelData* modelData = i_this->mpBackCloud->getModelData();
    gabi::Stack<GXColor> k0;

    /* lambert121: material and its texture SRT animation */
    Vrbox2Material* mat = (Vrbox2Material*)modelData->getMaterialNodePointer("lambert121");
    Vrbox2MaterialAnimation* anm = getMatAnmByName(pBackCloud, "lambert121");
    scrollTexSrt(i_this, anm, 0, 0x1B1);
    scrollTexSrt(i_this, anm, 1, 0x1B9);
    k0->r = (u8)vrbox2Env()->mVrkumoColor.r;
    k0->g = (u8)vrbox2Env()->mVrkumoColor.g;
    Vrbox2Environment* e = vrbox2Env();
    k0->a = 0xFF;
    k0->b = (u8)e->mVrkumoColor.b;
    f32 scale = (f32)vrbox2Env()->mVrkumoColorScale;
    setKColor(mat, k0.get(), scale);

    /* lambert122 (HD: scrolled by the animation of lambert121, at the full speed) */
    cloudMaterial(i_this, modelData, pBackCloud, "lambert122", "lambert121", 0x1F5, 0x1FD,
                  k0.get());
    /* lambert121_2_ */
    cloudMaterial(i_this, modelData, pBackCloud, "lambert121_2_", "lambert121_2_", 0x233, 0x23B,
                  k0.get());

    J3DModel* kasumi = i_this->mpKasumiMae;
    if (kasumi != nullptr) {
        Vrbox2Material* kmat = (Vrbox2Material*)kasumi->getModelData()->getMaterialNodePointer(
            (u16)0); /* getMaterialNodePointer((u16)0) */
        gabi::Stack<Vrbox2ColorS10> c0;
        c0->r = (u8)vrbox2Env()->mVrKasumiMaeColor.r;
        c0->g = (u8)vrbox2Env()->mVrKasumiMaeColor.g;
        c0->b = (u8)vrbox2Env()->mVrKasumiMaeColor.b;
        Vrbox2Environment* e2 = vrbox2Env();
        k0->a = 0;
        k0->r = (u8)e2->mVrkumoColor.a; /* mVrkumoColor.a */
        k0->g = 0;
        k0->b = 0;
        f32 cscale = (f32)vrbox2Env()->mVrKasumiMaeColorScale;
        Vrbox2TevBlock* tev = kmat->mpTevBlock;
        call_ptr(tev->__vtbl->setTevColor.get(), tev, 0, c0.get()); /* setTevColor(0, c0) */
        gabi::Stack<Vrbox2Color4f> cf;
        {
            f32 r = (f32)(s16)c0->r / 255.0f;
            f32 g = (f32)(s16)c0->g / 255.0f;
            f32 b = (f32)(s16)c0->b / 255.0f;
            f32 a = (f32)(s16)c0->a / 255.0f;
            cf->r = r;
            cf->g = g;
            cf->b = b;
            cf->a = a;
        }
        gabi::Stack<Vrbox2Color4f> cs;
        vrbox2ScaleColor(cs.get(), cf.get(), cscale);
        kmat->mConstantColorFlags = (u32)kmat->mConstantColorFlags | HAZE_CONSTANT_COLOR;
        Vrbox2Color4f* reg = vrbox2ColorRegister(&kmat->mConstantColorFlags, HAZE_COLOR_REGISTER);
        f32 a = (f32)(s16)c0->a / 255.0f;
        // GHS copies the float bits, including SNaNs.
        reg->mChannelBits[0] = cs->mChannelBits[0];
        reg->mChannelBits[1] = cs->mChannelBits[1];
        reg->mChannelBits[2] = cs->mChannelBits[2];
        reg->a = a;
        setKColor(kmat, k0.get(), 1.0f);
    }

    J3DModel* uso = i_this->mpUsoUmi;
    if (uso != nullptr) {
        Vrbox2Material* umat = (Vrbox2Material*)uso->getModelData()->getMaterialNodePointer((u16)0);
        k0->r = (u8)vrbox2Env()->mVrUsoUmiColor.r; /* mVrUsoUmiColor */
        k0->g = (u8)vrbox2Env()->mVrUsoUmiColor.g;
        Vrbox2Environment* e3 = vrbox2Env();
        k0->a = 0xFF;
        k0->b = (u8)e3->mVrUsoUmiColor.b;
        f32 uscale = (f32)vrbox2Env()->mVrUsoUmiColorScale;
        setKColor(umat, k0.get(), uscale);
    }
    i_this->mScrollSpeed = 0.0f;
}

/* inlined into daVrbox2_Draw */
static inline void setModelTRMtx(J3DModel* m) {
    J3DModel_setBaseTRMtx(m, mDoMtx_stack_c::get());
    mDoExt_modelUpdateDL(m);
}

/* 024D1D44 */
static BOOL daVrbox2_Draw(vrbox2_class* i_this) {
    J3DModel* pUsoUmi = i_this->mpUsoUmi;
    J3DModel* pBackCloud = i_this->mpBackCloud;
    J3DModel* pKasumiMae = i_this->mpKasumiMae;
    f32 y_origin = 0.0f;

    daVrbox2_color_set(i_this, pBackCloud);

    if (envColorsZero()) {
        return TRUE;
    }

    s32 roomNo = dComIfGp_roomControl_getStayNo(); /* dComIfGp_roomControl_getStayNo() */
    if (roomNo >= 0) {
        Vrbox2Room* rc = (Vrbox2Room*)dComIfGp_roomControl_getStatusRoomDt(roomNo);
        if (rc != nullptr) {
            Vrbox2FileList* fili = call_ptr<Vrbox2FileList*>(rc->__vtbl->getFileListInfo.get(),
                                                             rc); /* getFileListInfo() */
            if (fili != nullptr) {
                y_origin = (f32)fili->mSeaLevel; /* mSeaLevel */
            }
        }
    }

    f32 y_offset;
    if (vrbox2View() != 0) { /* dComIfGd_getView() */
        Vrbox2View* view = vrbox2View();
        y_offset = ((f32)view->mInvViewMtx.m[1][3] - y_origin) * 0.09f;
    } else {
        y_offset = 0.0f;
    }

    /* HD: stage-specific offsets */
    if (startStageIs("A_mori")) {
        y_offset += 30000.0f;
    }
    if (startStageIs("sea_E")) {
        Vrbox2View* view = vrbox2View();
        f32 r = (f32)view->mFovy / 60.0f; /* fovy / 60 */
        if (r > 1.0f) {
            y_offset = gabi::fmadds(16600.0f, (r - 1.0f) / 0.854871988f, y_offset);
        }
    }
    bool special = true;
    if (!startStageIs("M_NewD2")) {
        if (!startStageIs("Siren")) {
            special = false;
        }
    }

    if (special) {
        /* HD: the clear colour of a sead layer (RTTI-checked) is the false-sea colour */
        Vrbox2LayerTable* table = sLayerRegistry.mpTable;
        s16 idx = *sLayerIndex;
        u32 count = table->mHandleCount;
        Vrbox2LayerHandle* handles = table->mpHandles;
        Vrbox2LayerHandle* handle = (u32)idx < count ? handles + (u32)idx : handles;
        Vrbox2ClearLayer* obj = nullptr;
        if ((u16)handle->mGeneration != 0) {
            u32 index = handle->mIndex;
            gptr<Vrbox2ClearLayer>* entry = nullptr;
            if (index < (u32)table->mLayerCount)
                entry = (gptr<Vrbox2ClearLayer>*)table->mpLayers + index;
            obj = *entry;
        }
        if (sLayerRTTIGuard == 0) {
            sLayerRTTIGuard = 1;
            sLayerRTTI = layerRTTI;
        }
        if (obj != nullptr) {
            if (call_ptr<u32>(obj->__vtbl->checkDerived.get(), obj, &sLayerRTTI) == 0)
                obj = nullptr;
        }
        gabi::Stack<GXColor> k;
        k->r = (u8)vrbox2Env()->mVrUsoUmiColor.r;
        k->g = (u8)vrbox2Env()->mVrUsoUmiColor.g;
        Vrbox2Environment* e = vrbox2Env();
        k->a = 0xFF;
        k->b = (u8)e->mVrUsoUmiColor.b;
        gabi::Stack<Vrbox2Color4f> c;
        vrbox2ColorFromGX(c.get(), k.get());
        obj->mClearColor.mChannelBits[0] = c->mChannelBits[0];
        obj->mClearColor.mChannelBits[1] = c->mChannelBits[1];
        obj->mClearColor.mChannelBits[2] = c->mChannelBits[2];
        obj->mClearColor.mChannelBits[3] = c->mChannelBits[3];
    }

    {
        /* mDoMtx_stack_c::transS(view->mInvViewMtx[0][3], [1][3] - y_offset, [2][3]) */
        f32 x = (f32)vrbox2View()->mInvViewMtx.m[0][3];
        f32 y = (f32)vrbox2View()->mInvViewMtx.m[1][3] - y_offset;
        f32 z = (f32)vrbox2View()->mInvViewMtx.m[2][3];
        mDoMtx_stack_c::transS(x, y, z);
    }

    /* HD: draw through the sky layer with its own projection */
    Vrbox2Renderer* mgr = sRenderer;
    u32 n = mgr->mLayerCount;
    gptr<Vrbox2Layer>* layers = mgr->mpLayers;
    if (n > 1)
        layers++;
    Vrbox2Layer* layer = *layers;
    if (layer != nullptr) {
        u32 flags = layer->mFlags;
        Vrbox2Projection* saved = layer->mpProjection;
        Vrbox2Projection* cam;
        if ((flags & LAYER_OVERRIDE_PROJECTION) == 0)
            cam = saved != nullptr ? saved : &sDefaultProjection;
        else
            cam = layer->mpOverrideProjection;
        gabi::Stack<Vrbox2Projection> proj;
        vrbox2ProjectionConstruct(proj.get());
        vrbox2ProjectionSet(proj.get(), cam->mNear, cam->mFar, cam->mFovy, cam->mAspect);
        proj->mOffsetBits[0] = cam->mOffsetBits[0];
        proj->mOffsetBits[1] = cam->mOffsetBits[1];
        proj->mDirty = 1;
        if (sProjectionRTTIGuard == 0) {
            sProjectionRTTIGuard = 1;
            sProjectionRTTI = projectionRTTI;
        }
        if (call_ptr<u32>(cam->__vtbl->checkDerived.get(), cam, &sProjectionRTTI) != 0 &&
            cam != nullptr) {
            vrbox2ProjectionCopy(proj.get(), cam->mFrustum);
        }
        proj->mNear = 0.0001f;
        proj->mFar = 16000.0f;
        proj->mDirty = 1;
        layer->mpProjection = proj.get();
        dComIfGd_setListSky();
        if (pUsoUmi != nullptr)
            setModelTRMtx(pUsoUmi);
        if (pKasumiMae != nullptr)
            setModelTRMtx(pKasumiMae);
        mDoMtx_stack_c::transM(0.0f, 100.0f, 0.0f);
        setModelTRMtx(pBackCloud);
        layer->mpProjection = saved;
        vrbox2ProjectionDestroy(proj.get(), 2);
    }

    dComIfGd_setList();

    bool special2 = true;
    if (!startStageIs("M_NewD2")) {
        if (!startStageIs("Siren")) {
            special2 = false;
        }
    }
    if (special2) {
        vrbox2RestoreSeaLayer(); /* HD */
    }
    return TRUE;
}

// HD: the wind part of daVrbox2_color_set runs during Execute.
/* 024D3248 */
static BOOL daVrbox2_Execute(vrbox2_class* i_this) {
    Vrbox2Camera* pCamera = (Vrbox2Camera*)dComIfGp_getCamera(0); /* dComIfGp_getCamera(0) */
    cXyz* windVec = dKyw_get_wind_vec();
    f32 windPow = dKyw_get_wind_pow();
    f32 windX_ = (f32)windVec->x;
    f32 windZ_ = (f32)windVec->z;

    if (!startStageIs("Name")) {
        Vrbox2StageInfo* stag = (Vrbox2StageInfo*)dComIfGp_getStageStagInfo();
        if ((((u32)stag->mType >> 16) & 7) == STAGE_TYPE_MISC) {
            s16 stageWindY = 0;
            if (startStageIs("LinkRM"))
                stageWindY = 0x4000;
            else if (startStageIs("Orichh"))
                stageWindY = -0x4000;
            else if (startStageIs("Ojhous2"))
                stageWindY = 0x7fff;
            else if (startStageIs("Omasao"))
                stageWindY = -0x4000;
            else if (startStageIs("Onobuta"))
                stageWindY = 0x4000;

            s32 windX;
            s16 windY;
            Vrbox2SaveWind* sv = sSave;
            if ((s16)sv->mWindX == -1 && (s16)sv->mWindY == -1) { /* dComIfGs_getWindX/Y */
                windX = 0;
                windY = 0;
            } else {
                windX = (u16)vrbox2Env()->mWind.mTactWindAngleX; /* mWind.mTactWindAngleX */
                windY = (s16)vrbox2Env()->mWind.mTactWindAngleY; /* mWind.mTactWindAngleY */
            }
            windY += stageWindY;
            windX_ = cM_scos(windX) * cM_scos(windY);
            windZ_ = cM_scos(windX) * cM_ssin(windY);
            windPow = 0.6f;
        }
    }

    gabi::Stack<cXyz> eyePosXZ;
    gabi::Stack<cXyz> centerPosXZ;
    gabi::Stack<cXyz> lookDirXZ;
    // GHS copies vector words before clearing the vertical components.
    *eyePosXZ = pCamera->mEye;
    *centerPosXZ = pCamera->mCenter;
    eyePosXZ->y = 0.0f;
    centerPosXZ->y = 0.0f;

    vrbox2VectorDirection(eyePosXZ.get(), centerPosXZ.get(), lookDirXZ.get());
    f32 windDirView =
        vrbox2VectorProduct2d(0.0f, 0.0f, -windX_, -windZ_, lookDirXZ->x, lookDirXZ->z) * 0.0005f;
    f32 scrollSpeed = windDirView * windPow;
    if (startStageIs("M_DragB"))
        scrollSpeed = -0.0004f;
    i_this->mScrollSpeed = scrollSpeed;
    return TRUE;
}

/* 024D3920 */
static BOOL daVrbox2_IsDelete(vrbox2_class*) {
    return TRUE;
}

/* 024D3928 */
static BOOL daVrbox2_Delete(vrbox2_class*) {
    return TRUE;
}

/* 024D3930 */
static BOOL daVrbox2_solidHeapCB(fopAc_ac_c* i_actor) {
    // HD: resources use .bmd names instead of .bdl.
    vrbox2_class* i_this = static_cast<vrbox2_class*>(i_actor);
    const char* arc = "Stage"; /* "Stage" */

    J3DModelData* modelData = (J3DModelData*)dComIfG_getStageRes(arc, "vr_back_cloud.bmd");
    if (modelData == 0) /* JUT_ASSERT(0x324, modelData != NULL) */
        JUTAssertion::showAssert("d_a_vrbox2.cpp", 0x324, "modelData != (0)");
    i_this->mpBackCloud = mDoExt_J3DModel__create(modelData, 0x80000, 0x11020202);

    modelData = (J3DModelData*)dComIfG_getStageRes(arc, "vr_kasumi_mae.bmd");
    if (modelData != 0)
        i_this->mpKasumiMae = mDoExt_J3DModel__create(modelData, 0x80000, 0x11020202);
    modelData = (J3DModelData*)dComIfG_getStageRes(arc, "vr_uso_umi.bmd");
    if (modelData != 0)
        i_this->mpUsoUmi = mDoExt_J3DModel__create(modelData, 0x80000, 0x11020202);

    i_this->mScrollSpeed = 0.0f; /* HD */
    BOOL success = FALSE;
    if (i_this->mpBackCloud != nullptr && i_this->mpKasumiMae != nullptr &&
        i_this->mpUsoUmi != nullptr) {
        success = TRUE;
    }
    return success;
}

/* 024D3A48 */
static cPhs_State daVrbox2_Create(fopAc_ac_c* i_actor) {
    /* fopAcM_ct(i_actor, vrbox2_class) */
    if (!fopAcM_CheckCondition(i_actor, fopAcCnd_INIT_e)) {
        if (i_actor != nullptr) {
            fopAc_ac_c_ct(i_actor);
            i_actor->__vtbl = vrbox2_class__vt;
        }
        fopAcM_OnCondition(i_actor, fopAcCnd_INIT_e);
    }
    cPhs_State phase_state = cPhs_COMPLEATE_e;
    if (!fopAcM_entrySolidHeap(i_actor, daVrbox2_solidHeapCB, 0x21a0))
        phase_state = cPhs_ERROR_e;
    return phase_state;
}

// HD: compiler-generated and template code of this translation unit.
/* 024D3AD4 */
static void __sinit_d_a_vrbox2_cpp() {
    sinit_header_statics(gc_tu.headerStatics[0], gc_tu.headerStatics[1]);
}

/* 024D3B68 */
static void SafeString_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1)) {
        operator_delete(p);
    }
}

/* 024D3B7C */
static void vrbox2_class_dt(vrbox2_class* i_this, s32 flags) {
    if (i_this != nullptr) {
        vrbox2ActorDestroy(i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 024D3BD0 */
static void SafeString_assureTerminationImpl(void*) {}

/* WWHD glue
str 10041E14
str 10041E40
str 10041E04
str 10041E90
str 10041E80
str 10041E78
str 10041E88
str 10041EBC
str 10041EC4
str 10041ECC
str 10041EA4
str 10041ED4
str 10041EAC
str 10041EB4
str 10041EDC
str 10041EE4
str 10041EF8
str 10041F30
str 10041E20 in 024D1D44
str 10041E30
str 10041F0C in 024D3930
str 10041F1C
data sRenderer 101F95D0
data sSave 101F84DC
data sLayerIndex 104A1460
data sLayerRegistry 104B45C0
data sDefaultProjection 104A20FC
data sProjectionRTTIGuard 101FD9F0
data sProjectionRTTI 101FDCD4
data sLayerRTTIGuard 101FD7DC
data sLayerRTTI 101FDCE0
ext 027DF9B0 s32 vrbox2NameIndex(JUTNameTab*, const char*)
ext 02563F64 void vrbox2VectorDirection(cXyz*, cXyz*, cXyz*)
ext 02010CFC f32 vrbox2VectorProduct2d(f32, f32, f32, f32, f32, f32)
ext 027FA7F8 Vrbox2TexSrt* vrbox2TexSrt(Vrbox2MaterialAnimation*, s32)
ext 0274D458 void vrbox2ScaleColor(Vrbox2Color4f*, Vrbox2Color4f*, f32)
ext 027F9F0C Vrbox2Color4f* vrbox2ColorRegister(be<u32>*, s32)
ext 027389F8 void vrbox2ProjectionConstruct(Vrbox2Projection*)
ext 0274E04C void vrbox2ProjectionSet(Vrbox2Projection*, f32, f32, f32, f32)
ext 02738AC0 void vrbox2ProjectionCopy(Vrbox2Projection*, be<u32>*)
ext 02738A6C void vrbox2ProjectionDestroy(Vrbox2Projection*, s32)
ext 024D1C90 void vrbox2ColorFromGX(Vrbox2Color4f*, GXColor*)
ext 025D50BC void vrbox2ActorDestroy(vrbox2_class*, s32)
ext 0255F84C void vrbox2RestoreSeaLayer()
*/
