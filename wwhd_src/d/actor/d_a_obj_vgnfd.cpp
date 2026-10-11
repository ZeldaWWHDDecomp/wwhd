// Ganon's Tower trials door.
#include "d/actor/d_a_obj_vgnfd.h"
const gc_tu_data gc_tu = {.safeString = 0x10032BBC, .headerStatics = {0x1046C878, 0x101CD990, 0x1046C884}};
enum { dRes_INDEX_VGNFD_BTK_YGCBD00_e = 0x16, dRes_INDEX_VGNFD_DZB_VGNFD_e = 0x19 };
enum { STATE_WAIT, STATE_DEMO, STATE_DEMO_FIN };
enum { ACT_WAIT0, ACT_GANNON, ACT_VIB0, ACT_VIB1, ACT_BREAK, ACT_BURST };
enum {
    JA_SE_OBJ_B_BOSS_DR_LT_1 = 0x6A55,
    JA_SE_READ_RIDDLE_1 = 0x806,
    JA_SE_OBJ_B_BOSS_DR_BRK_1 = 0x6A1D,
    JA_SE_OBJ_B_BOSS_DR_BRK_2 = 0x6A1E,
    JA_SE_OBJ_B_BOSS_DR_BRK_3 = 0x6A1F
};
namespace dPa_name {
enum { ID_AK_ST_BOSSDOORSMOKE00 = 0xA344 };
}
// HD: private unit tables retain the tww names; autoglue does not support class statics yet.
struct VgnfdModelTable {
    be<s32> mIndex[5];
};
struct VgnfdDoorModelTable {
    be<s32> mIndex[2];
};
struct VgnfdEventTable {
    be<u16> mBit[4];
};
struct VgnfdDemoNames {
    gptr<char> mName[4];
};
struct VgnfdActNames {
    gptr<char> mName[2];
};
struct VgnfdBreakActNames {
    gptr<char> mName[6];
};
struct VgnfdParticleTable {
    be<u16> mEffect[6];
};
struct VgnfdVtable {
    be<u32> mEntry;
};
static VgnfdModelTable M_bdl_table;
static VgnfdModelTable M_brk_table;
static VgnfdDoorModelTable M_door_bdl_table;
static VgnfdEventTable M_door_ev_table;
static VgnfdEventTable M_boss_ev_table;
static VgnfdDemoNames l_daObjVgnfd_demo_name;
static VgnfdActNames l_daObjVgnfd_act_name;
static VgnfdBreakActNames l_daObjVgnfd_break_act_name;
static VgnfdParticleTable l_daObjVgnfd_pt_table;
static VgnfdVtable l_actorVtable;
void Vgnfd_brkCtor(mDoExt_brkAnm*);
using VgnfdArrayCtor = void (*)(mDoExt_brkAnm*);
void Vgnfd_constructArray(mDoExt_brkAnm*, u32, u32, VgnfdArrayCtor);
// HD: the raw call preserves a full-word event index before storing its signed halfword.
s32 Vgnfd_getEventIdx(dEvent_manager_c*, const char*, u32);
void Vgnfd_orderEvent(daObjVgnfd_c*, s32, u32, u32, u32, u32);
s32 Vgnfd_getStaffId(dEvent_manager_c*, const char*, fopAc_ac_c*, s32);
void Vgnfd_seStart(u32);
void Vgnfd_startQuake(dVibration_c*, s32, s32, cXyz*);
void Vgnfd_stopQuake(dVibration_c*, s32);
static gabi::HostFn l_brkCtorBinding(reinterpret_cast<const void*>(&Vgnfd_brkCtor), 0x025E80D0);
static void order(daObjVgnfd_c* actor, s32 event) {
    Vgnfd_orderEvent(actor, event, 0xFF, 0xFFFF, 0, 1);
}
static void condition(daObjVgnfd_c* actor) {
    actor->eventInfo.mCondition = u16(actor->eventInfo.mCondition) | dEvtCnd_UNK2_e;
}
static void audio(daObjVgnfd_c* actor, u32 id) {
    s32 reverb = dComIfGp_getReverb(actor->current.roomNo);
    mDoAud_seStart(id, &actor->current.pos, 0, reverb);
}
static void shock(s32 power, s32 duration) {
    auto* vibration = dComIfGp_getVibration();
    gabi::Local<cXyz> direction;
    direction->set(0.f, 1.f, 0.f);
    vibration->StartShock(power, duration, direction.get());
}
static void quake(s32 power, s32 duration) {
    auto* vibration = dComIfGp_getVibration();
    gabi::Local<cXyz> direction;
    direction->set(0.f, 1.f, 0.f);
    Vgnfd_startQuake(vibration, power, duration, direction.get());
}
/* 023B149C */
// HD: free binding for tww's static daObjVgnfd_c::solidHeapCB until autoglue supports static methods.
BOOL Vgnfd_solidHeapCB(fopAc_ac_c* actor) {
    return static_cast<daObjVgnfd_c*>(actor)->create_heap();
}
/* 023B11A0 */
BOOL daObjVgnfd_c::create_bdl_brk(s32 i) {
    auto* mdl_data = static_cast<J3DModelData*>(dComIfG_getObjectRes("VgnFD", M_bdl_table.mIndex[i]));
    if (!mdl_data) {
        JUT_ASSERT_fail("d_a_obj_vgnfd.cpp", 254, "mdl_data != 0");
        return false;
    }
    J3DModel* model = mDoExt_J3DModel__create(mdl_data, 0, 0x11020203);
    mModel[i] = model;
    if (!model)
        return false;
    s32 brk = M_brk_table.mIndex[i];
    if (brk == -1)
        return true;
    auto* brk_p = static_cast<J3DAnmTevRegKey*>(dComIfG_getObjectRes("VgnFD", brk));
    if (!brk_p) {
        JUT_ASSERT_fail("d_a_obj_vgnfd.cpp", 0x105, "brk_p != 0");
        return false;
    }
    return mBrkAnm[i].init(mdl_data, brk_p, true, J3DFrameCtrl::EMode_NONE, 1.f, 0, -1, false, 0) != 0;
}
/* 023B12D8 */
BOOL daObjVgnfd_c::create_heap() {
    for (s32 i = 0; i < 5; ++i)
        if (!create_bdl_brk(i))
            return false;
    for (u32 i = 0; i < 2; ++i) {
        auto* mdl_data = static_cast<J3DModelData*>(dComIfG_getObjectRes("VgnFD", M_door_bdl_table.mIndex[i]));
        if (!mdl_data) {
            JUT_ASSERT_fail("d_a_obj_vgnfd.cpp", 0x133, "mdl_data != 0");
            return false;
        }
        J3DModel* model = mDoExt_J3DModel__create(mdl_data, 0, 0x11020203);
        mModel2[i] = model;
        if (!model)
            return false;
    }
    auto* btk_data = static_cast<J3DAnmTextureSRTKey*>(dComIfG_getObjectRes("VgnFD", dRes_INDEX_VGNFD_BTK_YGCBD00_e));
    if (!btk_data) {
        JUT_ASSERT_fail("d_a_obj_vgnfd.cpp", 0x144, "btk_data != 0");
        return false;
    }
    if (!mBtkAnm.init(mModel2[1]->getModelData(), btk_data, true, J3DFrameCtrl::EMode_NONE, 1.f, 0, -1, false, 0))
        return false;
    auto* bg = static_cast<cBgD_t*>(dComIfG_getObjectRes("VgnFD", dRes_INDEX_VGNFD_DZB_VGNFD_e));
    J3DModel* model = mModel[0];
    Mtx34* matrix = model ? model->getBaseTRMtx() : nullptr;
    dBgW* world = dBgW_NewSet(bg, dBgW::MOVE_BG_e, matrix);
    M_bgw = world;
    if (!world) {
        JUT_ASSERT_fail("d_a_obj_vgnfd.cpp", 0x151, "M_bgw != (0)");
        return M_bgw.get() != nullptr;
    }
    return true;
}
/* 023B1694 */
cPhs_State daObjVgnfd_c::_create() {
    u32 flags = actor_condition;
    if (!(flags & fopAcCnd_INIT_e)) {
        if (this) {
            fopAc_ac_c_ct(this);
            __vtbl = gabi::ea(&l_actorVtable);
            mDoExt_btkAnm::ct(&mBtkAnm);
            Vgnfd_constructArray(mBrkAnm, 5, 0x78, Vgnfd_brkCtor);
            __ct(&mSmoke, 1);
            flags = actor_condition;
        }
        actor_condition = flags | fopAcCnd_INIT_e;
    }
    if (check_fin())
        return cPhs_ERROR_e;
    cPhs_State phase = dComIfG_resLoad(&mPhs, "VgnFD");
    if (phase != cPhs_COMPLEATE_e)
        return phase;
    if (!fopAcM_entrySolidHeap(this, Vgnfd_solidHeapCB, 0))
        return cPhs_ERROR_e;
    J3DModel* model = mModel[0];
    cullMtx = model ? gabi::ea(model->getBaseTRMtx()) : 0;
    init_mtx();
    mBrkAnm[0].setPlaySpeed(0.f);
    for (u32 i = 1; i < 5; ++i) {
        mBrkAnm[i].setPlaySpeed(0.f);
        if (dComIfGs_isEventBit(M_door_ev_table.mBit[i - 1]))
            mBrkAnm[i].setFrame(mBrkAnm[i].getEndFrame());
    }
    mBtkAnm.setPlaySpeed(0.f);
    s32 demo = get_start_demo_idx();
    M_demo_idx = demo;
    if (demo != -1) {
        s32 current = M_demo_idx;
        mState = STATE_DEMO;
        const char* name = l_daObjVgnfd_demo_name.mName[current];
        mEventIdx = Vgnfd_getEventIdx(dComIfGp_getPEvtManager(), name, 0xFF);
    }
    fopAcM_setCullSizeBox(this, -260.f, -10.f, -50.f, 260.f, 510.f, 100.f);
    mSmoke.mTevstr = gabi::ea(&tevStr);
    mSmoke.m11 = 0;
    mSmoke.mWindOff = 1;
    mSmoke.m12 = 1;
    auto* bg = dComIfG_Bgsp();
    dBgW* world = M_bgw;
    bg->Regist(world, this);
    world = M_bgw;
    world->Move();
    mInit = 1;
    return cPhs_COMPLEATE_e;
}
/* 023B18F8 */
bool daObjVgnfd_c::_delete() {
    if (heap) {
        dBgW* world = M_bgw;
        if (world && world->ChkUsed()) {
            auto* bg = dComIfG_Bgsp();
            world = M_bgw;
            bg->Release(world);
            M_bgw = nullptr;
        }
    }
    mSmoke.remove();
    dComIfG_resDelete(&mPhs, "VgnFD");
    return true;
}
/* 023B1604 */
s32 daObjVgnfd_c::get_start_demo_idx() {
    for (u32 i = 0; i < 4; ++i)
        if (!dComIfGs_isEventBit(M_door_ev_table.mBit[i]) && dComIfGs_isEventBit(M_boss_ev_table.mBit[i]))
            return i;
    return -1;
}
/* 023B1124 */
BOOL daObjVgnfd_c::check_fin() {
    for (u32 i = 0; i < 4; ++i)
        if (!dComIfGs_isEventBit(M_door_ev_table.mBit[i]))
            return false;
    return true;
}
/* 023B19EC */
void daObjVgnfd_c::on_fin() {
    dComIfGs_onEventBit(0x3204);
}
/* 023B14A0 */
void daObjVgnfd_c::init_mtx() {
    f32 x = scale.x, y = scale.y;
    J3DModel* model = mModel[0];
    f32 z = scale.z;
    model->mBaseScale.x = x;
    model->mBaseScale.y = y;
    model->mBaseScale.z = z;
    x = current.pos.x;
    y = current.pos.y;
    z = current.pos.z;
    mDoMtx_stack_c::transS(x, y, z);
    s16 ax = shape_angle.x, ay = shape_angle.y, az = shape_angle.z;
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), ax, ay, az);
    for (u32 i = 0; i < 5; ++i)
        mModel[i]->setBaseTRMtx(mDoMtx_stack_c::get());
    for (u32 i = 0; i < 2; ++i)
        mModel2[i]->setBaseTRMtx(mDoMtx_stack_c::get());
}
/* 023B1984 */
void daObjVgnfd_c::set_timer() {
    s32 staff = mStaffId;
    be<s32>* value = dComIfGp_evmng_getMyIntegerP(staff, "Timer");
    mTimer = 0;
    if (value)
        mTimer = s32(*value);
}
/* 023B1A00 */
bool daObjVgnfd_c::_execute() {
    bool done = false;
    u8 state = mState;
    if (state == STATE_DEMO || state == STATE_DEMO_FIN) {
        if (state == STATE_DEMO && s32(M_demo_idx) == -1)
            JUT_ASSERT_fail("d_a_obj_vgnfd.cpp", 0x253, "M_demo_idx != -1");
        if (!eventInfo.checkCommandDemoAccrpt()) {
            order(this, mEventIdx);
            condition(this);
        } else {
            s16 event = mEventIdx;
            BOOL end = dComIfGp_evmng_endCheck(event);
            // HD: the play object is fetched after endCheck even when the event continues.
            auto* play = &reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get())->play;
            if (end) {
                play->mEvtCtrl.mEventFlag = u16(play->mEvtCtrl.mEventFlag) | 8;
                if (state == STATE_DEMO) {
                    if (check_fin()) {
                        s32 next = Vgnfd_getEventIdx(dComIfGp_getPEvtManager(), "4_door_fin", 0xFF);
                        mEventIdx = next;
                        order(this, next);
                        u16 flags = eventInfo.mCondition;
                        mState = STATE_DEMO_FIN;
                        eventInfo.mCondition = flags | dEvtCnd_UNK2_e;
                    } else
                        mState = STATE_WAIT;
                } else {
                    on_fin();
                    done = true;
                    mState = STATE_WAIT;
                }
            } else {
                s32 staff = Vgnfd_getStaffId(&play->mEvtManager, "VgnFD", nullptr, 0);
                mStaffId = staff;
                if (staff != -1) {
                    s32 action = dComIfGp_evmng_getMyActIdx(
                        staff, state == STATE_DEMO ? l_daObjVgnfd_act_name.mName : l_daObjVgnfd_break_act_name.mName,
                        state == STATE_DEMO ? 2 : 6, false, 0);
                    staff = mStaffId;
                    BOOL advance = dComIfGp_evmng_getIsAddvance(staff);
                    if (state == STATE_DEMO) {
                        if (action == ACT_WAIT0) {
                            if (advance) {
                                s32 demo = M_demo_idx;
                                dComIfGs_onEventBit(M_door_ev_table.mBit[demo]);
                                set_timer();
                            }
                            s32 timer = static_cast<s32>(u32(mTimer) - 1);
                            mTimer = timer;
                            if (timer <= 0) {
                                dComIfGp_evmng_cutEnd(mStaffId);
                                s32 demo = M_demo_idx;
                                mBrkAnm[u32(demo) + 1].setPlaySpeed(1.f);
                                Vgnfd_seStart(JA_SE_OBJ_B_BOSS_DR_LT_1);
                                if (check_fin())
                                    Vgnfd_seStart(JA_SE_READ_RIDDLE_1);
                            }
                        }
                    } else {
                        if (advance) {
                            set_timer();
                            switch (action) {
                            case ACT_GANNON:
                                mBrkAnm[0].setPlaySpeed(1.f);
                                for (u32 i = 1; i < 5; ++i)
                                    mBrkAnm[i].setPlaySpeed(-1.f);
                                break;
                            case ACT_VIB0:
                                shock(8, 23);
                                audio(this, JA_SE_OBJ_B_BOSS_DR_BRK_1);
                                break;
                            case ACT_VIB1:
                                quake(2, 11);
                                break;
                            case ACT_BREAK:
                                mBtkAnm.setPlaySpeed(1.f);
                                mCurModel = 1;
                                // HD: these emitters retain their existing colors; the GC color override is absent.
                                for (u32 i = 0; i < 6; ++i) {
                                    u16 effect = l_daObjVgnfd_pt_table.mEffect[i];
                                    dPa_control_set(dComIfGp_getParticle(), 0, effect, &current.pos, &current.angle,
                                                    &scale, 255, nullptr, -1, nullptr, nullptr, nullptr);
                                }
                                dComIfGp_particle_setToon(dPa_name::ID_AK_ST_BOSSDOORSMOKE00, &current.pos,
                                                          &current.angle, &scale, 0xA0, &mSmoke, -1);
                                quake(4, 11);
                                audio(this, JA_SE_OBJ_B_BOSS_DR_BRK_2);
                                break;
                            case ACT_BURST:
                                Vgnfd_stopQuake(dComIfGp_getVibration(), -1);
                                shock(8, 27);
                                {
                                    s8 room = current.roomNo;
                                    mCurModel = 2;
                                    s32 reverb = dComIfGp_getReverb(room);
                                    mDoAud_seStart(JA_SE_OBJ_B_BOSS_DR_BRK_3, &current.pos, 0, reverb);
                                }
                                break;
                            }
                        }
                        s32 timer = mTimer;
                        if (timer > 0) {
                            timer = static_cast<s32>(u32(timer) - 1);
                            mTimer = timer;
                        }
                        if (u32(action) <= ACT_BREAK && timer <= 0)
                            dComIfGp_evmng_cutEnd(mStaffId);
                    }
                }
            }
        }
    }
    for (u32 i = 0; i < 5; ++i)
        mBrkAnm[i].play();
    mBtkAnm.play();
    if (done)
        fopAcM_delete(this);
    return true;
}
/* 023B203C */
bool daObjVgnfd_c::_draw() {
    auto* env = dKy_getEnvlight();
    settingTevStruct(env, 1, &current.pos, &tevStr);
    s32 current = mCurModel;
    if (current == 0) {
        env = dKy_getEnvlight();
        current = mCurModel;
        J3DModel* model = mModel2[current];
        setLightTevColorType(env, model, &tevStr);
        current = mCurModel;
        model = mModel2[current];
        mDoExt_modelUpdateDL(model);
        for (s32 i = 4; i >= 0; --i) {
            env = dKy_getEnvlight();
            model = mModel[i];
            setLightTevColorType(env, model, &tevStr);
            if (M_brk_table.mIndex[i] != -1) {
                model = mModel[i];
                f32 frame = mBrkAnm[i].getFrame();
                J3DModelData* data = model->getModelData();
                mBrkAnm[i].entry(data, frame);
            }
            model = mModel[i];
            mDoExt_modelUpdateDL(model);
        }
    } else if (current == 1) {
        env = dKy_getEnvlight();
        current = mCurModel;
        J3DModel* model = mModel2[current];
        setLightTevColorType(env, model, &tevStr);
        current = mCurModel;
        f32 frame = mBtkAnm.getFrame();
        model = mModel2[current];
        J3DModelData* data = model->getModelData();
        mBtkAnm.entry(data, frame);
        current = mCurModel;
        model = mModel2[current];
        mDoExt_modelUpdateDL(model);
    }
    return true;
}
/* 023B217C */
s32 Mthd_Create(daObjVgnfd_c* actor) {
    return actor->_create();
}
/* 023B2180 */
u8 Mthd_Delete(daObjVgnfd_c* actor) {
    return actor->_delete();
}
/* 023B2184 */
u8 Mthd_Execute(daObjVgnfd_c* actor) {
    return actor->_execute();
}
/* 023B2188 */
u8 Mthd_Draw(daObjVgnfd_c* actor) {
    return actor->_draw();
}
/* 023B228C */
BOOL Mthd_IsDelete(daObjVgnfd_c*) {
    return true;
}
/* 023B218C */
void sinit() {
    __sinit_header_statics();
}
/* 023B2220 */
void static_destructor(void* actor, u32 flags) {
    if (actor && (flags & 1))
        operator_delete(actor);
}
/* 023B2234 */
void destructor(daObjVgnfd_c* actor, u32 flags) {
    if (actor) {
        __dt(static_cast<fopAc_ac_c*>(actor));
        if (flags & 1)
            operator_delete(actor);
    }
}
/* 023B2288 */
void noop(void*) {}
/* WWHD glue
data M_bdl_table 10032D18
data M_brk_table 10032D2C
data M_door_bdl_table 10032CF8
data M_door_ev_table 10032D00
data M_boss_ev_table 10032D08
data l_daObjVgnfd_demo_name 101CD95C
data l_daObjVgnfd_act_name 101CD954
data l_daObjVgnfd_break_act_name 101CD96C
data l_daObjVgnfd_pt_table 101CD984
data l_actorVtable 10032BD4
str 10032D10
str 10032C18 in 023B11A0
str 10032C48 in 023B12D8
str 10032CBC in 023B1A00
str 10032C2C in 023B11A0
str 10032C3C
str 10032C5C in 023B12D8
str 10032C6C
str 10032C7C
str 10032CB0
str 10032CD0
str 10032CE4
ext 025E80D0 void Vgnfd_brkCtor(mDoExt_brkAnm*)
ext 028EFFD0 void Vgnfd_constructArray(mDoExt_brkAnm*,u32,u32,VgnfdArrayCtor)
ext 02543F10 s32 Vgnfd_getEventIdx(dEvent_manager_c*,const char*,u32)
ext 02542D88 s32 Vgnfd_getStaffId(dEvent_manager_c*,const char*,fopAc_ac_c*,s32)
ext 025D7A58 void Vgnfd_orderEvent(daObjVgnfd_c*,s32,u32,u32,u32,u32)
ext 025E1988 void Vgnfd_seStart(u32)
ext 025CB408 void Vgnfd_startQuake(dVibration_c*,s32,s32,cXyz*)
ext 025CB610 void Vgnfd_stopQuake(dVibration_c*,s32)
*/
