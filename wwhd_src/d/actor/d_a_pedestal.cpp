/* Tower pedestal. */
#include "d/actor/d_a_pedestal.h"
using daPedestal::Action;
using daPedestal::daPds_c;
using daPedestal::daPds_infiniteEcallBack_c;
using daPedestal::PdsEmitter;
const gc_tu_data gc_tu = {.safeString = 0x10034318};
// HD: a private emitter view covers the fields used by this callback.
struct daPedestal::PdsEmitter {
    u8 _00[0x5C];
    be<u32> mMaxFrame;
    u8 _60[0x1E4 - 0x60];
    gptr<void> mpCallback;
    u8 _1E8[8];
    Mtx34 mGlobalRotation;
    u8 _220[12];
    cXyz mGlobalTranslation;
    u8 _238[0x254 - 0x238];
    be<u32> mStatus;
    u8 _258[10];
    be<u8> mDrawMode;
};
// HD: protect the guest caller linkage area for real nonleaf resource callees.
template <class T> struct ProtectedLocal {
    struct Frame {
        u8 linkage[16];
        T value;
    };
    gabi::Local<Frame> storage;
    T *get() const { return gabi::at<T>(storage.a + offsetof(Frame, value)); }
    T *operator->() const { return get(); }
};
struct ResourceName {
    be<char> bytes[6];
};
static ResourceName m_arcname;
struct AnmTableEntry {
    be<s32> loopMode;
    be<f32> speed;
    be<s32> unk_4;
};
struct AnmTable {
    AnmTableEntry entries[4];
};
static AnmTable brkAnmTbl;
struct Names3 {
    gptr<char> entries[3];
};
static Names3 l_os_name;
struct Names4 {
    gptr<char> entries[4];
};
static Names4 cut_name_tbl;
using EventInit = gabi::ptmf<void (daPds_c::*)(s32)>;
using EventAction = gabi::ptmf<BOOL (daPds_c::*)(s32)>;
struct EventInitializers {
    EventInit entries[4];
};
static EventInitializers event_init_tbl;
struct EventActions {
    EventAction entries[4];
};
static EventActions event_action_tbl;
static Action wait_action;
struct StaticWord {
    be<u32> value;
};
struct StaticFloat {
    be<f32> value;
};
struct StaticFlag {
    be<u8> value;
};
struct StaticRecord {
    be<u32> words[3];
};
static StaticWord staticZero0;
static StaticWord staticZero1;
static StaticWord staticZero2;
static StaticWord staticZero3;
static StaticFloat staticMinimum;
static StaticFloat staticMaximum;
static StaticFloat staticMinimumConstant;
static StaticFloat staticMaximumConstant;
static StaticFlag staticFlag0;
static StaticFlag staticFlag1;
static StaticRecord staticRecord0;
static StaticRecord staticRecord1;
static StaticRecord staticRecord2;
extern void *getResource(dRes_control_c *, SafeString *, s32);
extern BOOL initBrk(mDoExt_brkAnm *, J3DModelData *, J3DAnmTevRegKey *, u32, s32, f32, s32, s32, BOOL, s32);
extern s32 setBackground(dBgW *, cBgD_t *, u32, Mtx34 *);
extern void setEmitterRotation(s16, s16, s16, Mtx34 *);
extern void seStartLevel(u32, const cXyz *, u32, s32);
extern fopAc_ac_c *searchFromName(const char *, s32, s32);
extern f32 searchDistanceXZ(fopAc_ac_c *, fopAc_ac_c *);
extern s32 playBrk(mDoExt_brkAnm *);
extern void copyMatrix(const Mtx34 *, Mtx34 *);
extern void modelUpdateDL(J3DModel *, u32);
extern void cutEnd(void *, s32);
extern BOOL getIsAdvance(void *, s32);
extern void destroyActor(fopAc_ac_c *, s32);
extern void constructHeader0(void *);
extern void constructHeader1(void *);
extern void registerHeader(void *);
static BOOL heapCB(fopAc_ac_c *);
static void sound(daPds_c *a, u32 id) {
    s32 reverb = dComIfGp_getReverb(a->current.roomNo);
    seStartLevel(id, &a->current.pos, 0, reverb);
}
/* 023C84E4 */
void daPds_infiniteEcallBack_c::execute(PdsEmitter *e) {
    if (mpPos) {
        cXyz *p = mpPos;
        f32 x = p->x, z = p->z, y = p->y;
        // HD: the emitter draw mode reverses global Y for modes seven and above.
        if (e->mDrawMode >= 7)
            y = -y;
        e->mGlobalTranslation.y = y;
        e->mGlobalTranslation.x = x;
        e->mGlobalTranslation.z = z;
    }
    if (mpAngle) {
        csXyz *a = mpAngle;
        s16 x = a->x, y = a->y, z = a->z;
        setEmitterRotation(x, y, z, &e->mGlobalRotation);
    }
}
/* 023C853C */
void daPds_infiniteEcallBack_c::end() {
    if (mpEmitter) {
        PdsEmitter *e = mpEmitter;
        u32 flags = e->mStatus;
        e->mMaxFrame = 0xFFFFFFFF;
        e->mStatus = flags | 1;
        mpEmitter->mpCallback = nullptr;
        mpEmitter = nullptr;
        mpPos = nullptr;
        mpAngle = nullptr;
    }
}
/* 023C8578 */
void daPds_infiniteEcallBack_c::makeEmitter(u16 id, const cXyz *p, const csXyz *a, const cXyz *scale) {
    end();
    dComIfGp_particle_set(id, p, a, scale, 255, reinterpret_cast<dPa_levelEcallBack *>(this), -1, nullptr, nullptr,
                          nullptr);
    mpAngle = const_cast<csXyz *>(a);
    mpPos = const_cast<cXyz *>(p);
}
/* 023C85F8 */
BOOL daPds_c::initBrkAnm(u8 index, BOOL force) {
    J3DModelData *data = mpModel->getModelData();
    ProtectedLocal<SafeString> name;
    name->mStringTop = gabi::ea(&m_arcname);
    name->__vtbl = 0x10034318;
    auto *animation = static_cast<J3DAnmTevRegKey *>(getResource(dComIfG_resControl(), name.get(), 8));
    if (!animation)
        JUT_ASSERT_fail("d_a_pedestal.cpp", 0x28C, "a_brk != NULL");
    AnmTableEntry *entry = &brkAnmTbl.entries[index];
    s32 loop = entry->loopMode;
    f32 speed = entry->speed;
    BOOL ok = initBrk(&mBrk, data, animation, 1, loop, speed, 0, -1, force, 0);
    if (!ok)
        return FALSE;
    unk30E = index;
    if (entry->unk_4 < 0)
        mBrk.setFrame(mBrk.getEndFrame());
    unk308 = mBrk.getFrame();
    return TRUE;
}
/* 023C8718 */
BOOL daPds_c::CreateHeap() {
    ProtectedLocal<SafeString> name;
    name->mStringTop = gabi::ea(&m_arcname);
    name->__vtbl = 0x10034318;
    auto *data = static_cast<J3DModelData *>(getResource(dComIfG_resControl(), name.get(), 5));
    if (!data)
        JUT_ASSERT_fail("d_a_pedestal.cpp", 0xC1, "modelData != NULL");
    mpModel = mDoExt_J3DModel__create(data, 0, 0x11020203);
    if (!mpModel || !initBrkAnm(3, FALSE))
        return FALSE;
    mpBgW = new_dBgW();
    if (!mpBgW)
        return FALSE;
    ProtectedLocal<SafeString> collision;
    collision->mStringTop = gabi::ea(&m_arcname);
    collision->__vtbl = 0x10034318;
    auto *dzb = static_cast<cBgD_t *>(getResource(dComIfG_resControl(), collision.get(), 11));
    return setBackground(mpBgW, dzb, dBgW::MOVE_BG_e, &mMtx) == 0;
}
/* 023C8828 */
static BOOL heapCB(fopAc_ac_c *base) { return static_cast<daPds_c *>(base)->CreateHeap(); }
/* 023C882C */
BOOL daPds_c::wakeupCheck() {
    s8 n = argument;
    u16 bit;
    switch (n) {
    case 0:
        bit = 0x1780;
        break;
    case 1:
        bit = 0x1740;
        break;
    case 2:
        bit = 0x1720;
        break;
    default:
        return FALSE;
    }
    return dComIfGs_isEventBit(bit) != 0;
}
/* 023C88CC */
BOOL daPds_c::finishCheck() {
    s8 n = argument;
    u16 bit;
    switch (n) {
    case 0:
        bit = 0x1710;
        break;
    case 1:
        bit = 0x1704;
        break;
    case 2:
        bit = 0x1B01;
        break;
    default:
        return FALSE;
    }
    return dComIfGs_isEventBit(bit) != 0;
}
/* 023C896C */
void daPds_c::set_mtx() {
    mpModel->setBaseScale(scale);
    mDoMtx_stack_c::transS(current.pos);
    mDoMtx_stack_c::YrotM(current.angle.y);
    // HD: cache every element before the verified nonsequential model stores.
    f32 values[12];
    Mtx34 *matrix = mDoMtx_stack_c::get();
    for (u32 i = 0; i < 12; i++)
        values[i] = matrix->m[i / 4][i % 4];
    J3DModel *model = mpModel;
    const u8 order[12] = {4, 10, 7, 11, 5, 3, 6, 1, 9, 8, 2, 0};
    for (u32 i = 0; i < 12; i++)
        model->mBaseTransformMtx.m[order[i] / 4][order[i] % 4] = values[order[i]];
    copyMatrix(matrix, &mMtx);
}
/* 023C8A50 */
void daPds_c::CreateInit() {
    J3DModel *model = mpModel;
    mType = mParameters & 255;
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(model));
    fopAcM_setCullSizeBox(this, -150, -20, -150, 150, 250, 150);
    u8 type = mType;
    if (type == 0 && wakeupCheck()) {
        s8 n = argument;
        if (n <= 2) {
            const char *name = l_os_name.entries[(u32)(s32)n];
            fopAc_ac_c *npc = searchFromName(name, 0, 0);
            if (npc && searchDistanceXZ(this, npc) < 100)
                npc->current.pos.y = (f32)current.pos.y;
        }
        current.pos.y = (f32)current.pos.y - 240;
    } else if (type == 1 && finishCheck())
        current.pos.y = (f32)current.pos.y + 240;
    set_mtx();
    dComIfG_Bgsp()->Regist(mpBgW, this);
    mpBgW->Move();
}
/* 023C8BD8 */
s32 daPds_c::_create() {
    u32 flags = actor_condition;
    if (!(flags & fopAcCnd_INIT_e)) {
        if (this) {
            fopAc_ac_c_ct(this);
            __vtbl = 0x10034330;
            mDoExt_brkAnm::ct(&mBrk);
            flags = actor_condition;
            mOctagonGlowCb.__vtbl = 0x100343F8;
        }
        actor_condition = flags | fopAcCnd_INIT_e;
    }
    s32 phase = dComIfG_resLoad(&mPhase, "Hdai1");
    if (phase == cPhs_COMPLEATE_e) {
        if (!fopAcM_entrySolidHeap(this, heapCB, 0x2000)) {
            mpBgW = nullptr;
            return cPhs_ERROR_e;
        }
        CreateInit();
    }
    return phase;
}
/* 023C8CB8 */
BOOL daPds_c::_delete() {
    if (heap)
        dComIfG_Bgsp()->Release(mpBgW);
    mOctagonGlowCb.end();
    dComIfG_resDelete(&mPhase, "Hdai1");
    return TRUE;
}
/* 023C8D18 */
void daPds_c::playBrkAnm() {
    s32 result = playBrk(&mBrk);
    f32 frame = mBrk.getFrame();
    unk30F = result;
    unk308 = frame;
}
/* 023C8D54 */
s32 daPds_c::getMyStaffId() {
    const char *name;
    switch ((s8)argument) {
    case 0:
        name = "Hdai1";
        break;
    case 1:
        name = "Hdai2";
        break;
    case 2:
        name = "Hdai3";
        break;
    default:
        return -1;
    }
    return dComIfGp_evmng_getMyStaffId(name, 0, 0);
}
/* 023C8E14 */
BOOL daPds_c::eventProc() {
    s32 staff = getMyStaffId();
    if (!dComIfGp_event_runCheck())
        return FALSE;
    if (staff != -1) {
        s32 index = dComIfGp_evmng_getMyActIdx(staff, cut_name_tbl.entries, 4, 1, 0);
        void *manager = dComIfGp_getPEvtManager();
        if (index == -1)
            cutEnd(manager, staff);
        else {
            BOOL advance = getIsAdvance(manager, staff);
            if (advance)
                (this->*event_init_tbl.entries[(u32)index])(staff);
            if ((this->*event_action_tbl.entries[(u32)index])(staff))
                dComIfGp_evmng_cutEnd(staff);
        }
    }
    return TRUE;
}
/* 023C8FBC */
BOOL daPds_c::setAction(Action *next, void *argument) {
    if ((s16)mAction.i != 0) {
        mActionStatus = ACTION_ENDING;
        (this->*mAction)(argument);
    }
    // HD: preserve descriptor-copy aliasing and the original field-store order.
    s16 d = next->d, i = next->i;
    mAction.d = d;
    mAction.i = i;
    s16 slot = mAction.i;
    u32 f = next->f;
    unk310 = 0;
    mAction.f = f;
    unk316 = 0;
    unk314 = 0;
    mActionStatus = ACTION_STARTING;
    unk31C = 0;
    unk312 = 0;
    (void)slot;
    (this->*mAction)(argument);
    return TRUE;
}
/* 023C90DC */
void daPds_c::action(void *argument) {
    if ((s16)mAction.i == 0) {
        ProtectedLocal<Action> initial;
        initial->d = (s16)wait_action.d;
        initial->i = (s16)wait_action.i;
        initial->f = (u32)wait_action.f;
        setAction(initial.get(), nullptr);
    }
    (this->*mAction)(argument);
}
/* 023C9188 */
BOOL daPds_c::_execute() {
    playBrkAnm();
    if (!eventProc())
        action(nullptr);
    set_mtx();
    mpBgW->Move();
    if (mOctagonGlowCb.mpEmitter)
        sound(this, 0x61D7);
    return TRUE;
}
/* 023C9210 */
BOOL daPds_c::_draw() {
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), mpModel, &tevStr);
    dComIfGd_setListBG();
    J3DModelData *data = mpModel->getModelData();
    mBrk.entry(data, mBrk.getFrame());
    modelUpdateDL(mpModel, 0);
    mDoExt_brkAnm_remove(data);
    dComIfGd_setList();
    return TRUE;
}
/* 023C92D0 */
void daPds_c::initialDefault(s32 id) {}
/* 023C92D4 */
BOOL daPds_c::actionDefault(s32 id) { return true; }
/* 023C92DC */
void daPds_c::initialMoveEvent(s32 id) {
    auto *p = dComIfGp_evmng_getMyFloatP(id, "Speed");
    speed.y = p ? (f32)*p : 1.0f;
    auto *distance = dComIfGp_evmng_getMyFloatP(id, "Dist");
    f32 y = current.pos.y;
    mTargetYPos = distance ? y + (f32)*distance : y;
}
/* 023C93AC */
BOOL daPds_c::actionMoveEvent(s32 id) {

    f32 speed = (f32)this->speed.y, y = (f32)current.pos.y + speed, target = mTargetYPos;
    current.pos.y = y;
    if (speed < 0.0f) {
        if (y > target) {
            sound(this, 0x61D8);
            return false;
        }
    } else if (speed > 0.0f) {
        if (y < target) {
            sound(this, 0x61D8);
            return false;
        }
    } else {
        current.pos.y = target;
        sound(this, 0x69D9);
        return true;
    }
    current.pos.y = target;
    this->speed.y = 0;
    sound(this, 0x69D9);
    return true;
}
/* 023C94A4 */
void daPds_c::initialEffectSet(s32 id) {

    mOctagonGlowCb.makeEmitter(0x826D, &current.pos, &shape_angle, nullptr);
    initBrkAnm(0, true);
}
/* 023C94F8 */
void daPds_c::initialEffectEnd(s32 id) {

    mOctagonGlowCb.end();
    initBrkAnm(2, true);
}
/* 023C9530 */
BOOL daPds_c::waitAction(void *argument) {
    if (mActionStatus == ACTION_STARTING)
        mActionStatus = ACTION_ONGOING;
    return TRUE;
}
/* 023C954C */
static s32 create(daPds_c *a) { return a->_create(); }
/* 023C9550 */
static BOOL destroy(daPds_c *a) { return a->_delete(); }
/* 023C9554 */
static BOOL draw(daPds_c *a) { return a->_draw(); }
/* 023C9558 */
static BOOL execute(daPds_c *a) { return a->_execute(); }
/* 023C955C */
static void initialize() {
    staticZero2.value = 0;
    staticZero0.value = 0;
    staticZero3.value = 0;
    staticZero1.value = 0;
    registerHeader(&staticRecord0);
    f32 min = staticMinimumConstant.value, max = staticMaximumConstant.value;
    staticMinimum.value = min;
    staticMaximum.value = max;
    constructHeader0(&staticFlag0);
    registerHeader(&staticRecord1);
    constructHeader1(&staticFlag1);
    registerHeader(&staticRecord2);
}
/* 023C95F0 */
static void stringDestroy(u8 *p, u32 flags) {
    if (p && (flags & 1))
        operator_delete(p);
}
/* 023C9604 */
static void callbackHook0(daPds_infiniteEcallBack_c *p) {}
/* 023C9608 */
static void callbackHook1(daPds_infiniteEcallBack_c *p) {}
/* 023C960C */
static void callbackHook2(daPds_infiniteEcallBack_c *p) {}
/* 023C9610 */
static void stringTerminate(u8 *p) {}
/* 023C9614 */
void daPds_infiniteEcallBack_c::setup(PdsEmitter *p) { mpEmitter = p; }
/* 023C961C */
static void actorDestroy(daPds_c *p, u32 flags) {
    if (p) {
        destroyActor(p, 0);
        if (flags & 1)
            operator_delete(p);
    }
}
/* 023C9670 */
static BOOL isDelete(daPds_c *p) { return true; }
/* WWHD glue
data m_arcname 100343F0
data brkAnmTbl 101CE5DC
data l_os_name 101CE60C
data cut_name_tbl 101CE5AC
data event_init_tbl 101CE56C
data event_action_tbl 101CE58C
data wait_action 100342EC
data staticZero0 1046CC0C
data staticZero1 1046CC10
data staticZero2 1046CC14
data staticZero3 1046CC18
data staticMinimum 1046CC00
data staticMaximum 1046CC04
data staticMinimumConstant 100343E4
data staticMaximumConstant 100343E8
data staticFlag0 1046CC08
data staticFlag1 1046CC09
data staticRecord0 101CE618
data staticRecord1 101CE624
data staticRecord2 101CE630
str 100343F0 "Hdai1" in 023C8BD8
str 100343F0 "Hdai1" in 023C8CB8
str 100343B8 "Hdai1" in 023C8D54
str 100343C0 "Hdai2"
str 100343C8 "Hdai3"
str 100343D4 "Speed"
str 100343DC "Dist"
str 10034348 "d_a_pedestal.cpp" in 023C85F8
str 1003436C "d_a_pedestal.cpp" in 023C8718
str 1003435C "a_brk != NULL"
str 10034380 "modelData != NULL"
ext 026066C4 void* getResource(dRes_control_c*,SafeString*,s32)
ext 025E8154 BOOL initBrk(mDoExt_brkAnm*,J3DModelData*,J3DAnmTevRegKey*,u32,s32,f32,s32,s32,BOOL,s32)
ext 0200A030 s32 setBackground(dBgW*,cBgD_t*,u32,Mtx34*)
ext 028245AC void setEmitterRotation(s16,s16,s16,Mtx34*)
ext 025E1A40 void seStartLevel(u32,const cXyz*,u32,s32)
ext 025D9F38 fopAc_ac_c* searchFromName(const char*,s32,s32)
ext 025D6958 f32 searchDistanceXZ(fopAc_ac_c*,fopAc_ac_c*)
ext 025E742C s32 playBrk(mDoExt_brkAnm*)
ext 028E90D4 void copyMatrix(const Mtx34*,Mtx34*)
ext 025E2DE0 void modelUpdateDL(J3DModel*,u32)
ext 02543280 void cutEnd(void*,s32)
ext 025447C8 BOOL getIsAdvance(void*,s32)
ext 025D50BC void destroyActor(fopAc_ac_c*,s32)
ext 028ED6F8 void constructHeader0(void*)
ext 028EAB2C void constructHeader1(void*)
ext 028F026C void registerHeader(void*)
*/
