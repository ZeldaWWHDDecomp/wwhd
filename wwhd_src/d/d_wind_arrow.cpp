/** Wind direction arrow. */
#include "d/d_wind_arrow.h"
static const f32 l_zero;
static const f32 l_offsetY;
static const f32 l_offsetZ;
static const f32 l_scaleValue;
static const f32 l_animationSpeed;
static u32 l_offsetPosInitialized;
static cXyz l_offsetPos;
static u32 l_scaleInitialized;
static cXyz l_scale;
static u8 l_monotone;
static WindArrowDrawBuffers l_drawBuffers;
static WindArrowHeaderStatics l_headerStatics;
static const WindArrowPiConstants l_pi;
enum { daPyStts0_SHIP_RIDE_e = 0x10000, daPyStts1_WIND_WAKER_CONDUCT_e = 1 };
enum { dRes_INDEX_ALWAYS_BDL_YA_e = 0x3C, dRes_INDEX_ALWAYS_BTK_YA_e = 0x5F };

/* 025CCAE8 */
u32 dWindArrow_c::draw() {
    f32 zero = l_zero;
    if (mModelInfo.mBtkAnm.mFrameCtrl.mRate == zero) return TRUE;
    if (!(reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get())->play.mPlayerStatus[0][0] & daPyStts0_SHIP_RIDE_e)) return TRUE;
    if (dComIfGp_event_runCheck() &&
        !(reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get())->play.mPlayerStatus[0][1] & daPyStts1_WIND_WAKER_CONDUCT_e)) return TRUE;
    // HD: initialize guarded statics in the original store order.
    if (!l_offsetPosInitialized) {
        l_offsetPosInitialized = 1;
        f32 z = l_offsetZ, y = l_offsetY;
        l_offsetPos.z = z; l_offsetPos.x = zero; l_offsetPos.y = y;
    }
    if (!l_scaleInitialized) {
        f32 scale = l_scaleValue;
        l_scaleInitialized = 1;
        l_scale.x = scale; l_scale.z = scale; l_scale.y = scale;
    }
    fopAc_ac_c* ac = gabi::at<fopAc_ac_c>(mParam);
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::Local<Mtx34> mtx, rotMtx;
    gabi::Local<cXyz> offs;
    mDoMtx_stack_c::transS(ac->current.pos.x, ac->current.pos.y, ac->current.pos.z);
    mDoMtx_stack_c::YrotM(ac->shape_angle.y);
    PSMTXCopy(mDoMtx_stack_c::get(), mtx.get());
    cXyz* windVec = dKyw_get_wind_vec();
    s16 windAngle = cM_atan2s(windVec->x, windVec->z);
    mDoMtx_stack_c::YrotS(windAngle);
    PSMTXCopy(mDoMtx_stack_c::get(), rotMtx.get());
    J3DModel* model = mModelInfo.mpModel;
    f32 sx = l_scale.x, sz = l_scale.z, sy = l_scale.y;
    model->mBaseScale.z = sz; model->mBaseScale.x = sx; model->mBaseScale.y = sy;
    PSMTXMultVec(mtx.get(), &l_offsetPos, offs.get());
    f32 x = offs->x, y = offs->y, z = offs->z;
    rotMtx->m[0][3] = x; rotMtx->m[1][3] = y; rotMtx->m[2][3] = z;
    f32 a = rotMtx->m[0][0], b = rotMtx->m[0][1], c = rotMtx->m[0][2];
    f32 d = rotMtx->m[1][0], e = rotMtx->m[1][1], f = rotMtx->m[1][2];
    f32 h = rotMtx->m[2][0], i = rotMtx->m[2][1], j = rotMtx->m[2][2];
    // HD: matrix assignment captures every component before the interleaved stores.
    model = mModelInfo.mpModel;
    model->mBaseTransformMtx.m[2][1] = i; model->mBaseTransformMtx.m[2][2] = j;
    model->mBaseTransformMtx.m[0][2] = c; model->mBaseTransformMtx.m[0][1] = b;
    model->mBaseTransformMtx.m[2][0] = h; model->mBaseTransformMtx.m[1][1] = e;
    model->mBaseTransformMtx.m[1][0] = d; model->mBaseTransformMtx.m[0][3] = x;
    model->mBaseTransformMtx.m[1][2] = f; model->mBaseTransformMtx.m[0][0] = a;
    model->mBaseTransformMtx.m[1][3] = y; model->mBaseTransformMtx.m[2][3] = z;
    model = mModelInfo.mpModel;
    mModelInfo.mBtkAnm.entry(model->getModelData(), mModelInfo.mBtkAnm.getFrame());
    u8 monotone = l_monotone;
    WindArrowPlayLists* lists = dWindArrow_getLists();
    l_drawBuffers.mpOpa = monotone ? lists->mpP1Opa : lists->mpMaskOffOpa;
    lists = dWindArrow_getLists();
    l_drawBuffers.mpXlu = monotone ? lists->mpP1Xlu : lists->mpMaskOffXlu;
    mDoExt_modelUpdateDL(mModelInfo.mpModel, 0);
    l_drawBuffers.mpOpa = dWindArrow_getLists()->mpOpa;
    l_drawBuffers.mpXlu = dWindArrow_getLists()->mpXlu;
    mDoExt_invisibleModel_entryMaskOff(&mModelInfo.mInvisibleModel);
    // HD: remove the texture animator directly, after the invisible-model call.
    reinterpret_cast<WindArrowModelData*>(mModelInfo.mpModel->getModelData())->mpTextureSRTAnimator = 0;
    return TRUE;
}
/* 025CCDA0 */
u32 dWindArrow_c::execute() {
    mModelInfo.mBtkAnm.mFrameCtrl.mRate = dKyw_get_wind_power()->mPower;
    mModelInfo.mBtkAnm.play();
    return TRUE;
}
/* 025CCDE0 */
void dWindArrow_modelInfo_dt(dWindArrow_modelInfo_c* info, u32 flags) {
    if (info) {
        mDoExt_invisibleModel_dt(&info->mInvisibleModel, 2);
        if (flags & 1) dWindArrow_free(info);
    }
}
/* 025CCE34 */
void dWindArrow_dt(dWindArrow_c* arrow, u32 flags) {
    if (arrow) {
        mDoExt_invisibleModel_remove(&arrow->mModelInfo.mInvisibleModel);
        mDoExt_destroySolidHeap(arrow->heap);
        dWindArrow_modelInfo_dt(&arrow->mModelInfo, 2);
        dWindArrow_base_dt(arrow, 0);
        if (flags & 1) dWindArrow_free(arrow);
    }
}
/* 025CCEB0 */
static u32 dWindArrow_Delete(dWindArrow_c* arrow) {
    dWindArrow_dt(arrow, 2);
    return TRUE;
}
/* 025CCED8 */
dWindArrow_modelInfo_c* dWindArrow_modelInfo_ct(dWindArrow_modelInfo_c* info) {
    // HD: this constructor entry can allocate and returns the object address.
    if (!info) {
        info = static_cast<dWindArrow_modelInfo_c*>(dWindArrow_alloc(0x80));
        if (!info) return nullptr;
    }
    info->mpModel = nullptr;
    mDoExt_invisibleModel_ct(&info->mInvisibleModel);
    mDoExt_btkAnm::ct(&info->mBtkAnm);
    return info;
}
/* 025CCF30 */
dWindArrow_c* dWindArrow_ct(dWindArrow_c* arrow) {
    if (!arrow) {
        // HD: allocation includes trailing storage beyond the fields used here.
        arrow = static_cast<dWindArrow_c*>(dWindArrow_alloc(0x180));
        if (!arrow) return nullptr;
    }
    dWindArrow_base_ct(arrow);
    arrow->base.__vtbl = 0x10056680;
    dWindArrow_modelInfo_ct(&arrow->mModelInfo);
    return arrow;
}
/* 025CCF8C */
u32 dWindArrow_c::createHeap() {
    if (!heap) {
        WindArrowSolidHeap* newHeap = mDoExt_createSolidHeapFromGameToCurrent(0, 0);
        heap = newHeap;
        if (!newHeap) return FALSE;
        // HD: debug heap name is stored by this entry.
        newHeap->mName = STR(0x100566C4);
    }
    return TRUE;
}
/* 025CD000 */
void dWindArrow_c::adjustHeap() {
    mDoExt_restoreCurrentHeap();
    if (mDoExt_adjustSolidHeap(heap) >= 0) {
        WindArrowSolidHeap* solidHeap = heap;
        WindArrowHeapVtable* vtable = solidHeap->__vtbl;
        // HD: capture the vtable once, and query size before start address.
        u32 size = gabi::call_ptr<u32>(vtable->getHeapSize.get(), solidHeap);
        u32 start = gabi::call_ptr<u32>(vtable->getStartAddr.get(), solidHeap);
        DCStoreRangeNoSync(start, size);
    }
}
/* 025CD078 */
cPhs_State dWindArrow_c::create() {
    // HD: the create entry includes heap setup and adjustment on every completed path.
    if (!createHeap()) return cPhs_ERROR_e;
    if (this) dWindArrow_ct(this);
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::Local<SafeString> name;
    name->mStringTop = 0x10056660; name->__vtbl = 0x10056668;
    J3DModelData* modelData = static_cast<J3DModelData*>(dWindArrow_getRes(dComIfG_resControl(), name.get(), dRes_INDEX_ALWAYS_BDL_YA_e));
    if (!modelData) dWindArrow_assert(STR(0x10056690), 0x73, STR(0x100566A4));
    J3DModel* model = mDoExt_J3DModel__create(modelData, 0x80000, 0x200);
    mModelInfo.mpModel = model;
    if (!model || !mDoExt_invisibleModel_create(&mModelInfo.mInvisibleModel, model)) {
        adjustHeap();
        return cPhs_ERROR_e;
    }
    gabi::Local<SafeString> animName;
    animName->__vtbl = 0x10056668; animName->mStringTop = 0x10056660;
    J3DAnmTextureSRTKey* anm = static_cast<J3DAnmTextureSRTKey*>(dWindArrow_getRes(dComIfG_resControl(), animName.get(), dRes_INDEX_ALWAYS_BTK_YA_e));
    u32 ok = mModelInfo.mBtkAnm.init(modelData, anm, true, J3DFrameCtrl::EMode_LOOP, l_animationSpeed, 0, -1, false, 0);
    adjustHeap();
    return ok ? cPhs_COMPLEATE_e : cPhs_ERROR_e;
}
/* 025CD1B8 */
static void __sinit_d_wind_arrow_cpp() {
    l_headerStatics.zero[2] = 0; l_headerStatics.zero[0] = 0;
    l_headerStatics.zero[3] = 0; l_headerStatics.zero[1] = 0;
    __register_global_object(0x101F137C);
    f32 a = l_pi.negativePi, b = l_pi.positivePi;
    l_headerStatics.negativePi = a; l_headerStatics.positivePi = b;
    dWindArrow_firstHeader_ct(&l_headerStatics.firstObject);
    __register_global_object(0x101F1388);
    dWindArrow_secondHeader_ct(&l_headerStatics.secondObject);
    __register_global_object(0x101F1394);
}
/* 025CD24C */
static u32 dWindArrow_IsDelete(dWindArrow_c* arrow) { return TRUE; }
/* 025CD254 */
static void dWindArrow_empty_dt(void* object, u32 flags) {
    // HD: compiler-generated empty base destructor still honors its delete flag.
    if (object && (flags & 1)) dWindArrow_free(object);
}
/* 025CD268 */
static void dWindArrow_empty_ct(void* object) {}

/* WWHD glue (tools/verify/autoglue.py): this translation unit's data
data l_zero 10056658
data l_offsetY 100566B8
data l_offsetZ 100566BC
data l_scaleValue 100566C0
data l_animationSpeed 100566D0
data l_offsetPosInitialized 101FDD18
data l_offsetPos 101FDD1C
data l_scaleInitialized 101FDD28
data l_scale 101FDD2C
data l_monotone 101F4829
data l_drawBuffers 104B4634
data l_headerStatics 1048733C
data l_pi 100566D8
ext 0273AD10 void* dWindArrow_alloc(u32)
ext 0273AF40 void dWindArrow_free(void*)
ext 025E895C void mDoExt_invisibleModel_ct(mDoExt_invisibleModel*)
ext 025E89F8 void mDoExt_invisibleModel_dt(mDoExt_invisibleModel*, u32)
ext 025E8A48 u32 mDoExt_invisibleModel_create(mDoExt_invisibleModel*, J3DModel*)
ext 025E899C void mDoExt_invisibleModel_remove(mDoExt_invisibleModel*)
ext 025E8CD8 void mDoExt_invisibleModel_entryMaskOff(mDoExt_invisibleModel*)
ext 025DD5F0 void dWindArrow_base_ct(dWindArrow_c*)
ext 025DD630 void dWindArrow_base_dt(dWindArrow_c*, u32)
ext 025E3630 WindArrowSolidHeap* mDoExt_createSolidHeapFromGameToCurrent(u32, u32)
ext 025E3868 void mDoExt_destroySolidHeap(WindArrowSolidHeap*)
ext 025E37D8 void mDoExt_restoreCurrentHeap()
ext 025E3678 s32 mDoExt_adjustSolidHeap(WindArrowSolidHeap*)
ext C00088B8 void DCStoreRangeNoSync(u32, u32)
ext 026066C4 void* dWindArrow_getRes(dRes_control_c*, SafeString*, u32)
ext 0273AA24 void dWindArrow_assert(const char*, u32, const char*)
ext 0257DB04 WindArrowWindPower* dKyw_get_wind_power()
ext 028ED6F8 void dWindArrow_firstHeader_ct(u8*)
ext 028EAB2C void dWindArrow_secondHeader_ct(u8*)
*/
