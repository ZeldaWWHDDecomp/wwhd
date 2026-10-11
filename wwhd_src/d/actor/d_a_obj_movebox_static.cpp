/* Per-TU constructors and wrappers, WWHD movebox. */
#include "d/actor/d_a_obj_movebox.h"
void moveboxInitializeHeaderByte(be<u8>* value);
void moveboxInitializeHeaderFlag(be<u8>* value);
void moveboxConstructArray(void* array, s32 count, s32 stride, u32 constructor);
void moveboxDestroyArray(void* array, s32 count, s32 stride, u32 destructor, s32 first, s32 second);
void moveboxConstructWater(daObjMovebox::MoveboxWaterWorkHD* check);
void moveboxDestroyGroundBase(void* check, s32 flags);
void moveboxDestroyLineBase(void* check, s32 flags);
void moveboxConstructSmokeBase(daObjMovebox::EffSmokeCB* callback, s32 flags);
void moveboxDestroyActorBase(daObjMovebox::Act_c* actor, s32 flags);
void moveboxConstructActorBase(daObjMovebox::Act_c* actor);
u32 moveboxCheckAppearRegister(daObjMovebox::Act_c* actor);
namespace daObjMovebox {
/* 02376278 */
static Bgc_c* Bgc_ct(Bgc_c* p) {
    if (p == nullptr)
        p = static_cast<Bgc_c*>(operator_new(sizeof(Bgc_c)));
    if (p != nullptr) {
        for (s32 i = 0; i < 23; i++) {
            p->mGroundY[i] = 0.0f;
            p->mWallPos[i].copy(cXyz::Zero);
        }
        p->mWaterY = 0.0f;
        p->mMaxGroundIdx = -1;
        p->mWallIdx = -1;
        p->mNearestWallDist = 3.4028234663852886e38f;
        p->mStateFlags = 0;
    }
    return p;
}
/* 0237A670 */
static BOOL Mthd_Delete(Act_c* p) {
    BOOL result = p->MoveBGDelete();
    if (p->mbShouldAppear)
        dComIfG_resDelete(&p->mPhs, p->arc());
    return result;
}
/* 0237A6D4 */
static BOOL Mthd_Execute(Act_c* p) { return p->MoveBGExecute(); }
/* 0237A6D8 */
static BOOL Mthd_Draw(Act_c* p) { return p->Draw_v(); }
/* 0237A6E8 */
static BOOL Mthd_IsDelete(Act_c* p) { return p->IsDelete_v(); }
/* 0237A6F8 */
static void __sinit_d_a_obj_movebox_cpp() {
    for (s32 i = 0; i < 4; i++)
        l_statics.mHeaderWords[i] = 0;
    __register_global_object(gabi::ea(&l_registrations.mHeader));
    l_statics.mMinAngle = -3.1415927410125732f;
    l_statics.mMaxAngle = 3.1415927410125732f;
    moveboxInitializeHeaderByte(&l_statics.mHeaderByte);
    __register_global_object(gabi::ea(&l_registrations.mByte));
    moveboxInitializeHeaderFlag(&l_statics.mHeaderFlag);
    __register_global_object(gabi::ea(&l_registrations.mFlag));
    // The compiler initializes the nonzero direction components after static storage.
    Bgc_c::M_lin20[2].m08 = 1.0f;
    Bgc_c::M_lin20[2].m0C = 1.0f;
    Bgc_c::M_lin20[3].m0C = 1.0f;
    Bgc_c::M_lin20[5].m08 = 1.0f;
    Bgc_c::M_lin20[7].m08 = 1.0f;
    Bgc_c::M_lin20[7].m0C = 1.0f;
    Bgc_c::M_lin20[8].m0C = 1.0f;
    Bgc_c::M_lin20[10].m08 = 1.0f;
    Bgc_c::M_lin20[12].m08 = 1.0f;
    Bgc_c::M_lin20[12].m0C = 1.0f;
    Bgc_c::M_lin20[13].m0C = 1.0f;
    Bgc_c::M_lin20[15].m08 = 1.0f;
    Bgc_c::M_lin20[17].m08 = 1.0f;
    Bgc_c::M_lin20[17].m0C = 1.0f;
    Bgc_c::M_lin20[18].m0C = 1.0f;
    Bgc_c::M_lin20[20].m08 = 1.0f;
    Bgc_c::M_lin20[3].m08 = -1.0f;
    Bgc_c::M_lin20[4].m08 = -1.0f;
    Bgc_c::M_lin20[4].m0C = -1.0f;
    Bgc_c::M_lin20[5].m0C = -1.0f;
    Bgc_c::M_lin20[8].m08 = -1.0f;
    Bgc_c::M_lin20[9].m08 = -1.0f;
    Bgc_c::M_lin20[9].m0C = -1.0f;
    Bgc_c::M_lin20[10].m0C = -1.0f;
    Bgc_c::M_lin20[13].m08 = -1.0f;
    Bgc_c::M_lin20[14].m08 = -1.0f;
    Bgc_c::M_lin20[14].m0C = -1.0f;
    Bgc_c::M_lin20[15].m0C = -1.0f;
    Bgc_c::M_lin20[18].m08 = -1.0f;
    Bgc_c::M_lin20[19].m08 = -1.0f;
    Bgc_c::M_lin20[19].m0C = -1.0f;
    Bgc_c::M_lin20[20].m0C = -1.0f;
    Bgc_c::M_lin5[1].m08 = 1.0f;
    Bgc_c::M_lin5[1].m0C = 1.0f;
    Bgc_c::M_lin5[2].m0C = 1.0f;
    Bgc_c::M_lin5[4].m08 = 1.0f;
    Bgc_c::M_lin5[2].m08 = -1.0f;
    Bgc_c::M_lin5[3].m08 = -1.0f;
    Bgc_c::M_lin5[3].m0C = -1.0f;
    Bgc_c::M_lin5[4].m0C = -1.0f;
    moveboxConstructArray(&Bgc_c::M_gnd_work[0], 23, 0x54, 0x0237AABC);
    __register_global_object(gabi::ea(&l_registrations.mGround));
    moveboxConstructWater(&Bgc_c::M_wrt_work);
    __register_global_object(gabi::ea(&l_registrations.mWater));
    moveboxConstructArray(&Bgc_c::M_wall_work[0], 23, 0x6C, 0x0237AB6C);
    __register_global_object(gabi::ea(&l_registrations.mWall));
    for (s32 i = 0; i < 13; i++)
        Act_c::M_attr[i].m0C = 75.0f;
}
/* 0237A8F8 */
static void arraydtor_gnd() { moveboxDestroyArray(&Bgc_c::M_gnd_work[0], 23, 0x54, 0x0237A940, 0, 0); }
/* 0237A91C */
static void arraydtor_wall() { moveboxDestroyArray(&Bgc_c::M_wall_work[0], 23, 0x6C, 0x0237AA30, 0, 0); }
/* 0237A940 */
static void gnd_dt(void* p, s32 flags) {
    if (p != nullptr) {
        auto* check = static_cast<dBgS_GndChk*>(p);
        check->cBgS_PolyInfo::__vtbl = 0x1002D004;
        check->dBgS_PolyPassChk::__vtbl = 0x1002D024;
        check->dBgS_GrpPassChk::__vtbl = 0x1002CFE4;
        moveboxDestroyGroundBase(p, 0);
        if (flags & 1)
            operator_delete(p);
    }
}
/* 0237A9B8 */
static void water_dt(void* p, s32 flags) {
    if (p != nullptr) {
        auto* check = static_cast<MoveboxWaterDestructorHD*>(p);
        check->mPolyVtable = 0x1002D074;
        check->mPassVtable = 0x1002D094;
        check->mGroupVtable = 0x1002CFE4;
        moveboxDestroyLineBase(&check->mLinePrefix, 0);
        if (flags & 1)
            operator_delete(p);
    }
}
/* 0237AA30 */
static void wall_dt(void* p, s32 flags) {
    if (p != nullptr) {
        auto* check = static_cast<dBgS_LinChk*>(p);
        check->dBgS_PolyPassChk::__vtbl = 0x1002D104;
        check->dBgS_GrpPassChk::__vtbl = 0x1002CFE4;
        check->cBgS_PolyInfo::__vtbl = 0x1002CFD4;
        moveboxDestroyLineBase(p, 0);
        if (flags & 1)
            operator_delete(p);
    }
}
/* 0237AAA8 */
static void trivial_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
/* 0237AABC */
static dBgS_GndChk* gnd_ct(dBgS_GndChk* p) {
    if (p == nullptr)
        p = static_cast<dBgS_GndChk*>(operator_new(sizeof(dBgS_GndChk)));
    if (p != nullptr)
        dBgS_GndChk_ct(p, {0x1002D034, 0x1002D044, 0x1002D064, 0x1002D054}, true);
    return p;
}
/* 0237AB6C */
static dBgS_LinChk* wall_ct(dBgS_LinChk* p) {
    if (p == nullptr)
        p = static_cast<dBgS_LinChk*>(operator_new(sizeof(dBgS_LinChk)));
    if (p != nullptr)
        dBgS_LinChk_ct(p, {0x1002D114, 0x1002D124, 0x1002D144, 0x1002D134}, true);
    return p;
}
/* 0237AC1C */
static void empty_virtual(void* p) {}
/* 0237AC20 */
static void smoke_draw(void* p) {}
/* 0237AC24 */
static BOOL actor_IsDelete(Act_c* p) { return TRUE; }
/* 0237AC2C */
static void actor_empty(Act_c* p) {}
/* 0237AC30 */
static EffSmokeCB* smoke_ct(EffSmokeCB* p) {
    if (p == nullptr)
        p = static_cast<EffSmokeCB*>(operator_new(sizeof(EffSmokeCB)));
    if (p != nullptr) {
        moveboxConstructSmokeBase(p, 1);
        p->__vtbl = 0x1002D1D0;
    }
    return p;
}
/* 0237AC88 */
static void smoke_dt(EffSmokeCB* p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
/* 0237AC9C */
static void actor_dt(Act_c* p, s32 flags) {
    if (p != nullptr) {
        moveboxDestroyArray(&p->mSmokeCbs[0], 2, 0x34, 0x0237AC88, 0, 0);
        dCcD_Cyl_dt(&p->mCyl, 2);
        dCcD_Stts_dt(&p->mStts, 2);
        moveboxDestroyActorBase(p, 0);
        if (flags & 1)
            operator_delete(p);
    }
}
/* 0237A468 */
static s32 Mthd_Create(Act_c* p) {
    u32 condition = p->actor_condition;
    if (!(condition & 8)) {
        if (p != nullptr) {
            moveboxConstructActorBase(p);
            p->__vtbl = 0x1002D79C;
            dCcD_Stts_ct(&p->mStts);
            dCcD_Cyl_ct(&p->mCyl, 0x1002CFC4);
            Bgc_ct(&p->mBgc);
            moveboxConstructArray(&p->mSmokeCbs[0], 2, 0x34, 0x0237AC30);
            condition = p->actor_condition;
        }
        p->actor_condition = condition | 8;
    }
    p->mType = p->prm(4, 24);
    p->prmX_init();
    p->prmZ_init();
    s32 phase = 5;
    u32 appear = moveboxCheckAppearRegister(p);
    p->mbShouldAppear = appear;
    if (appear) {
        phase = dComIfG_resLoad(&p->mPhs, p->arc());
        if (phase == 4) {
            s32 heap = p->attr(0x1002D168, 0x1002D1AC)->mDZBHeapSize;
            p->path_init();
            const Attr_c* a = p->attr(0x1002D168, 0x1002D1AC);
            const char* arc = p->arc();
            s32 dzb = a->mDZBFileIndex;
            phase = p->MoveBGCreate(arc, dzb, MOVEBOX_CRR_TRANSLATE, heap);
            if (phase != 4 && phase != 5)
                JUT_ASSERT_fail(STR(0x1002D168), 0x7D4, STR(0x1002D218));
        }
    }
    return phase;
}

} // namespace daObjMovebox

using namespace daObjMovebox;
/* WWHD glue
ext 02376F9C u32 moveboxCheckAppearRegister(daObjMovebox::Act_c* actor)
ext 024F1D40 void moveboxConstructActorBase(daObjMovebox::Act_c* actor)
ext 025D50BC void moveboxDestroyActorBase(daObjMovebox::Act_c* actor,s32 flags)
ext 025A5B18 void moveboxConstructSmokeBase(daObjMovebox::EffSmokeCB* callback,s32 flags)
ext 02008B4C void moveboxDestroyLineBase(void* check,s32 flags)
ext 02008DAC void moveboxDestroyGroundBase(void* check,s32 flags)
ext 024F22DC void moveboxConstructWater(daObjMovebox::MoveboxWaterWorkHD* check)
ext 028F0164 void moveboxDestroyArray(void* array,s32 count,s32 stride,u32 destructor,s32 first,s32 second)
ext 028EFFD0 void moveboxConstructArray(void* array,s32 count,s32 stride,u32 constructor)
ext 028EAB2C void moveboxInitializeHeaderFlag(be<u8>* value)
ext 028ED6F8 void moveboxInitializeHeaderByte(be<u8>* value)
*/
