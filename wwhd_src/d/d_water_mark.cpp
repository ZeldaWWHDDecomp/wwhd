/** Water-mark effect. WWHD, verified unit d_water_mark. */
#include "d/d_water_mark.h"

static s16 m_circle_cnt;
static s16 m_player_foot_now_id;
static dBgS_ObjGndChk m_ground_check;
static WaterHeapPointer effectHeap;
static f32 clipScale;
static f32 groundOffset;
static f32 invalidGround;
static f32 zero;
static f32 surfaceOffset;
static f32 playRate;
static u32 waterVtable;
static u8 heapDebugName;
static u32 safeStringVtable;
static u8 clipper;
static Mtx34 viewMatrix;
static WaterSinCosTable sincosTable;
static u32 groundCtorChkVtable;
static u32 groundCtorPolyVtable;
static u32 groundCtorGroupVtable;
static u32 groundCtorPassVtable;
static u32 groundDtorPolyVtable;
static u32 groundDtorPassVtable;
static u32 groundDtorGroupVtable;

/* 025CC200 */
u32 dWaterMark_c::draw() {
    gabi::Local<cXyz> position;
    position->copy(mPos);
    u32 clipped = Water_clip(&clipper, &viewMatrix, position.get(),
                             gabi::fmuls_ppc(clipScale, mScale.x));
    if (clipped == 0) {
        Water_brkEntry(&mBrk, mpModel->getModelData(), mBrk.mFrameCtrl.mFrame);
        // HD: the texture frame comes from the lower half of mParam.
        Water_btpEntry(&mBtp, mpModel->getModelData(), s16(u32(mParam) & 0xFFFF));
        Water_modelUpdateDL(mpModel, 0);
    }
    return TRUE;
}

/* 025CC2B4 */
u32 dWaterMark_c::setMatrix() {
    f32 y = gabi::fadds_ppc(mPos.y, groundOffset);
    f32 x = mPos.x;
    f32 z = mPos.z;
    m_ground_check.m_pos.x = x;
    m_ground_check.m_pos.z = z;
    m_ground_check.m_pos.y = y;
    f32 ground = Water_GroundCross(dComIfG_Bgsp(), &m_ground_check);
    f32 bad = invalidGround;
    mPos.y = ground;
    if (ground == bad) {
        return FALSE;
    }
    dBgS* bgs = dComIfG_Bgsp();
    cM3dGPla* tri = Water_GetTriPla(bgs, m_ground_check.mBgIndex, m_ground_check.mPolyIndex);
    // HD: a missing triangle is a second failure path.
    if (tri == NULL) {
        return FALSE;
    }
    s16 direction = cM_atan2s(tri->mNormal.x, tri->mNormal.z);
    u32 relative = u16(s32(direction) - s32(field_0x12e));
    gabi::Local<cXyz> normalXZ;
    normalXZ->x = tri->mNormal.x;
    normalXZ->z = tri->mNormal.z;
    normalXZ->y = zero;
    f32 mag = PSVECSquareMag(normalXZ.get());
    f32 norm_xz_mag = Water_sqrt(mag);
    mDoMtx_stack_c::transS(mPos.x, gabi::fadds_ppc(mPos.y, surfaceOffset), mPos.z);
    WaterSinCosPair* angles = &sincosTable.entries[relative >> 3];
    s16 rotX = cM_atan2s(gabi::fmuls_ppc(norm_xz_mag, angles->cosine), tri->mNormal.y);
    s16 heading = field_0x12e;
    s16 rotZ = cM_atan2s(-gabi::fmuls_ppc(norm_xz_mag, angles->sine), tri->mNormal.y);
    Water_ZXYrotM(mDoMtx_stack_c::get(), rotX, heading, rotZ);
    // HD: load all twelve words before the original nonsequential matrix stores.
    f32 matrix[12];
    Mtx34* stack = mDoMtx_stack_c::get();
    for (u32 i = 0; i < 12; ++i) {
        matrix[i] = stack->m[i / 4][i % 4];
    }
    J3DModel* model = mpModel;
    Mtx34& dst = model->mBaseTransformMtx;
    dst.m[2][3] = matrix[11];
    dst.m[1][3] = matrix[7];
    dst.m[0][2] = matrix[2];
    dst.m[2][0] = matrix[8];
    dst.m[1][1] = matrix[5];
    dst.m[2][1] = matrix[9];
    dst.m[1][0] = matrix[4];
    dst.m[2][2] = matrix[10];
    dst.m[1][2] = matrix[6];
    dst.m[0][0] = matrix[0];
    dst.m[0][3] = matrix[3];
    dst.m[0][1] = matrix[1];
    field_0x12c = Water_ChkMoveBG(dComIfG_Bgsp(), static_cast<cBgS_PolyInfo*>(&m_ground_check));
    return TRUE;
}

/* 025CC4A4 */
u32 dWaterMark_c::execute() {
    bool play = field_0x134 == -1;
    if (!play) {
        s16 start = field_0x130;
        s16 end = field_0x132;
        s16 now = m_player_foot_now_id;
        if ((start < end && start <= now && end > now) ||
            (start >= end && (start <= now || end > now))) {
            field_0x134 = -1;
            play = true;
        }
    }
    if (play) {
        Water_brkPlay(&mBrk);
    }
    bool remove = (mBrk.mFrameCtrl.mState & J3DFrameCtrl::STATE_STOP_E) != 0;
    if (!remove) {
        if (mBrk.mFrameCtrl.mRate == zero) {
            remove = true;
        } else if (field_0x12c == 1) {
            remove = setMatrix() == FALSE;
        }
    }
    if (remove) {
        Water_kankyoDelete(this);
    }
    return TRUE;
}

/* 025CC564 */
static u32 dWaterMark_IsDelete(dWaterMark_c* i_this) {
    return TRUE;
}

/* 025CC56C */
u32 dWaterMark_c::wm_delete() {
    WaterSolidHeap* heap = mpHeap;
    if (heap != NULL) {
        Water_destroySolidHeap(heap);
    }
    if (mParam == 1) {
        m_circle_cnt = s16(u16(m_circle_cnt) - 1);
    }
    // HD: the environment base destructor is explicitly called.
    Water_baseDelete(this, 0);
    return TRUE;
}

/* 025CC5D0 */
u32 dWaterMark_c::create() {
    if (gabi::ea(this) != 0) {
        Water_baseConstruct(this);
        base.__vtbl = gabi::ea(&waterVtable);
        Water_brkConstruct(&mBrk);
        Water_btpConstruct(&mBtp);
    }
    u32 params = mParam;
    u32 kind = params & 0xFFFF;
    field_0x12e = u16(params >> 16);
    mParam = kind;
    if (kind > 2) {
        return cPhs_ERROR_e;
    }
    if (kind == 1) {
        s16 count = s16(u16(m_circle_cnt) + 1);
        m_circle_cnt = count;
        if (count > 10) {
            return cPhs_ERROR_e;
        }
    }
    JKRHeap* parent = effectHeap.mpHeap;
    WaterSolidHeap* heap;
    if (parent != NULL) {
        // HD: effect heap grows to 0x3534 with 0x100 alignment and explicit fallback heaps.
        heap = Water_createSolidHeap(0x3534, parent, 0x100);
        mpHeap = heap;
    } else {
        heap = mpHeap;
    }
    if (heap == NULL) {
        parent = Water_getGameHeap(0);
        heap = Water_createSolidHeap(0x3534, parent, 0x100);
        mpHeap = heap;
        if (heap == NULL) {
            parent = Water_getGameHeap(1);
            heap = Water_createSolidHeap(0x3534, parent, 0x100);
            mpHeap = heap;
            if (heap == NULL) {
                return cPhs_ERROR_e;
            }
        }
    }
    heap->mDebugName = gabi::ea(&heapDebugName);
    gabi::Local<WaterResourceName> modelName;
    modelName->__vtbl = gabi::ea(&safeStringVtable);
    modelName->mString = gabi::guest_addr("Always");
    J3DModelData* modelData = static_cast<J3DModelData*>(
        Water_getResource(dComIfG_resControl(), modelName.get(), dRes_INDEX_ALWAYS_BDL_MPA_SIMI_e));
    if (modelData == NULL) {
        Water_assert("d_water_mark.cpp", 0x14E, "modelData != NULL");
    }
    mpModel = mDoExt_J3DModel__create(modelData, 0x80000, 0x11020022);
    gabi::Local<WaterResourceName> brkName;
    brkName->mString = gabi::guest_addr("Always");
    brkName->__vtbl = gabi::ea(&safeStringVtable);
    void* brk = Water_getResource(dComIfG_resControl(), brkName.get(), dRes_INDEX_ALWAYS_BRK_MPA_SIMI_e);
    f32 rate = playRate;
    u32 brk_rt = Water_brkInit(&mBrk, modelData, brk, TRUE, J3DFrameCtrl::EMode_NONE,
                              0, 0xFFFFFFFF, 0, 0, rate);
    gabi::Local<WaterResourceName> btpName;
    btpName->mString = gabi::guest_addr("Always");
    btpName->__vtbl = gabi::ea(&safeStringVtable);
    void* btp = Water_getResource(dComIfG_resControl(), btpName.get(), dRes_INDEX_ALWAYS_BTP_MPA_SIMI_e);
    u32 is_anm_init = brk_rt & Water_btpInit(&mBtp, modelData, btp, FALSE,
                                           J3DFrameCtrl::EMode_NONE, 0, 0xFFFFFFFF, 0, 0, rate);
    Water_restoreCurrentHeap();
    Water_adjustSolidHeap(mpHeap);
    J3DModel* model = mpModel;
    if (model == NULL || is_anm_init == 0) {
        return cPhs_ERROR_e;
    }
    // HD: preserve the original z/x/y scale-store order.
    f32 sx = mScale.x;
    f32 sz = mScale.z;
    f32 sy = mScale.y;
    model->mBaseScale.z = sz;
    model->mBaseScale.x = sx;
    model->mBaseScale.y = sy;
    if (setMatrix() == FALSE) {
        return cPhs_ERROR_e;
    }
    if (mParam == 2) {
        s16 old = m_player_foot_now_id;
        field_0x134 = old;
        s16 next = s16(u16(m_player_foot_now_id) + 1);
        m_player_foot_now_id = next == 40 ? 0 : next;
        s16 start = s16(u16(field_0x134) + 20);
        if (start >= 40) {
            start = s16(u16(start) - 40);
        }
        s16 end = s16(u16(start) + 20);
        field_0x130 = start;
        if (end < 40) {
            field_0x132 = end;
            mParam = 0;
        } else {
            mParam = 0;
            field_0x132 = s16(u16(end) - 40);
        }
    } else {
        field_0x134 = -1;
    }
    return cPhs_COMPLEATE_e;
}

/* 025CC934 */
// HD: initialize per-TU header statics and the specialized object-ground check.
static void __sinit_d_water_mark_cpp() {
    sinit_header_statics_rodata(0x104872CC, 0x101F130C, 0x104872D8, 0x10056650);
    Water_groundConstruct(&m_ground_check);
    m_ground_check.mbCamThrough = 0;
    m_ground_check.mbLinkThrough = 0;
    m_ground_check.mPolyPassChk = gabi::ea(static_cast<dBgS_PolyPassChk*>(&m_ground_check));
    m_ground_check.mbArrowThrough = 0;
    m_ground_check.mbBombThrough = 0;
    m_ground_check.mGrpPassChk = gabi::ea(static_cast<dBgS_GrpPassChk*>(&m_ground_check));
    m_ground_check.mGrp = 1;
    m_ground_check.mbBoomerangThrough = 0;
    static_cast<cBgS_Chk*>(&m_ground_check)->__vtbl = gabi::ea(&groundCtorChkVtable);
    static_cast<cBgS_PolyInfo*>(&m_ground_check)->__vtbl = gabi::ea(&groundCtorPolyVtable);
    static_cast<dBgS_GrpPassChk*>(&m_ground_check)->__vtbl = gabi::ea(&groundCtorGroupVtable);
    m_ground_check.mbRopeThrough = 0;
    static_cast<dBgS_PolyPassChk*>(&m_ground_check)->__vtbl = gabi::ea(&groundCtorPassVtable);
    m_ground_check.mbObjThrough = 1;
    __register_global_object(0x101F1330);
}

/* 025CCA58 */
// HD: adjacent empty destructor with GHS deleting flags; original generating class unknown.
static void waterEmptyDeletingDtor(void* i_this, u32 flags) {
    if (i_this != NULL && (flags & 1)) {
        Water_delete(i_this);
    }
}

/* 025CCA6C */
// HD: per-TU ground-check destructor tables.
static void waterGroundDtor(dBgS_ObjGndChk* i_this, u32 flags) {
    if (i_this != NULL) {
        static_cast<cBgS_PolyInfo*>(i_this)->__vtbl = gabi::ea(&groundDtorPolyVtable);
        static_cast<dBgS_PolyPassChk*>(i_this)->__vtbl = gabi::ea(&groundDtorPassVtable);
        static_cast<dBgS_GrpPassChk*>(i_this)->__vtbl = gabi::ea(&groundDtorGroupVtable);
        Water_groundDestruct(i_this, 0);
        if (flags & 1) {
            Water_delete(i_this);
        }
    }
}

/* 025CCAE4 */
// HD: empty adjacent method; original generating class unknown.
static void waterEmpty(void* i_this) {}

/* WWHD glue (tools/verify/autoglue.py): per-TU state, constants and external operations
data m_circle_cnt 101F133C
data m_player_foot_now_id 101F133E
data m_ground_check 104872E8
data effectHeap 1047C8B4
data clipScale 10056540
data groundOffset 10056638
data invalidGround 1005663C
data zero 10056640
data surfaceOffset 10056644
data playRate 10056648
data waterVtable 10056570
data heapDebugName 10056548
data safeStringVtable 10056550
data clipper 1048CFF0
data viewMatrix 104B45F8
data sincosTable 104A44F8
data groundCtorChkVtable 100565D0
data groundCtorPolyVtable 100565E0
data groundCtorGroupVtable 100565F0
data groundCtorPassVtable 10056600
data groundDtorPolyVtable 100565A0
data groundDtorPassVtable 100565C0
data groundDtorGroupVtable 10056580
str 10056558 "Always" renamed
str 10056610 "d_water_mark.cpp" renamed
str 10056624 "modelData != NULL" renamed
ext 02838148 u32 Water_clip(void*, Mtx34*, const cXyz*, f32)
ext 025E83FC void Water_brkEntry(WaterBrkAnm*, J3DModelData*, f32)
ext 025E7B3C void Water_btpEntry(WaterBtpAnm*, J3DModelData*, s16)
ext 025E2DE0 void Water_modelUpdateDL(J3DModel*, u32)
ext 02008974 f32 Water_GroundCross(dBgS*, dBgS_ObjGndChk*)
ext 020084C8 cM3dGPla* Water_GetTriPla(dBgS*, u16, u16)
ext 024EEABC s16 Water_ChkMoveBG(dBgS*, cBgS_PolyInfo*)
ext 028F4384 f32 Water_sqrt(f32)
ext 025F1B48 void Water_ZXYrotM(Mtx34*, s16, s16, s16)
ext 025E742C void Water_brkPlay(WaterBrkAnm*)
ext 025DAD48 void Water_kankyoDelete(dWaterMark_c*)
ext 025E3868 void Water_destroySolidHeap(WaterSolidHeap*)
ext 025DD630 void Water_baseDelete(dWaterMark_c*, u32)
ext 025DD5F0 void Water_baseConstruct(dWaterMark_c*)
ext 025E80D0 void Water_brkConstruct(WaterBrkAnm*)
ext 025E7820 void Water_btpConstruct(WaterBtpAnm*)
ext 025E35BC WaterSolidHeap* Water_createSolidHeap(u32, JKRHeap*, u32)
ext 025E2F64 JKRHeap* Water_getGameHeap(u32)
ext 026066C4 void* Water_getResource(dRes_control_c*, WaterResourceName*, u32)
ext 0273AA24 void Water_assert(const char*, u32, const char*)
ext 025E8154 u32 Water_brkInit(WaterBrkAnm*, J3DModelData*, void*, u32, u32, u32, u32, u32, u32, f32)
ext 025E789C u32 Water_btpInit(WaterBtpAnm*, J3DModelData*, void*, u32, u32, u32, u32, u32, u32, f32)
ext 025E37D8 void Water_restoreCurrentHeap()
ext 025E3678 void Water_adjustSolidHeap(WaterSolidHeap*)
ext 02008E0C void Water_groundConstruct(dBgS_ObjGndChk*)
ext 02008DAC void Water_groundDestruct(dBgS_ObjGndChk*, u32)
ext 0273AF40 void Water_delete(void*)
*/
