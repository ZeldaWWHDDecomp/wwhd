// Barrel and sea mine actor, ported from GameCube and WWHD disassembly.
// Binary-derived;
#include "d/actor/d_a_obj_barrel2.h"
#include "gc_api/gc_api.h"
using daObjBarrel2::Act_c;
using daObjBarrel2::Attr_c;
// HD: mapped names are aliases of the original guest storage.
static barrel2_hd::Attributes barrelAttributes;
static barrel2_hd::ItemPointer barrelTmpItem;
static barrel2_hd::ModeTable barrelModes;
static barrel2_hd::SinTable barrelSinCos;
static barrel2_hd::Angles barrelZeroAngles;
static barrel2_hd::WaterCheck barrelWater;
static u32 barrelWaterGuard;
static Mtx34 barrelStackMatrix;
static barrel2_hd::InitializerData barrelInitialization;
static barrel2_hd::DestructorChains barrelChains;
static barrel2_hd::CylinderSource barrelCylinderSource;
static f32 barrelExitSpeed;
static f32 barrelMineExitSpeed;
static f32 barrelItemDistanceMax;

static gptr<fopAc_ac_c> &tmp_item() { return barrelTmpItem.mpActor; }
static Attr_c *attr(Act_c *a, const char *file, const char *expr) {
    if ((u32)a->m410 >= 4) {
        Barrel_assertion(file, 0x1EA, expr);
    }
    return gabi::at<Attr_c>(gabi::ea(&barrelAttributes) + (u32)a->m410 * sizeof(Attr_c));
}

// HD: item deletion retains the complete tail-call result register.

static bool end_demo() {
    u32 play = dComIfGp_ea();
    if (Barrel_eventEndCheck(&gabi::at<dComIfG_inf_c>(play)->play.mEvtManager, "barrel2_brk")) {
        dComIfGp_event_reset();
        return false;
    }
    return true;
}
static bool exists_demo() {
    u32 play = dComIfGp_ea();
    void *mgr = &gabi::at<dComIfG_inf_c>(play)->play.mEvtManager;
    u32 second = dComIfGp_ea();
    s32 idx = Barrel_eventIndex(&gabi::at<dComIfG_inf_c>(second)->play.mEvtManager, "barrel2_brk",
                                (u8)0xFF);
    return Barrel_eventData(mgr, idx) != nullptr;
}

static void barrel_sound(Act_c *a, u32 id) {
    if (a && gabi::ea(&a->eyePos)) {
        s32 reverb = Barrel_reverb((s8)a->current.roomNo);
        Barrel_sound(id, &a->eyePos, 0, reverb);
    }
}
static void *barrel_particle(u32 id, cXyz *pos, csXyz *angle, cXyz *scale, s32 toon = 0,
                             void *color = nullptr) {
    u32 play = dComIfGp_ea();
    void *ctrl = gabi::at<dComIfG_inf_c>(play)->play.mParticle.get();
    return Barrel_particleSet(ctrl, toon, id, pos, angle, scale, (u8)0xFF, nullptr, -1, color,
                              color, nullptr);
}
static void effect_position(Act_c *a, const char *file, const char *expr, cXyz *pos, cXyz *scale) {
    Attr_c *t0 = attr(a, file, expr);
    Attr_c *t1 = attr(a, file, expr);
    f32 mag = (f32)t0->m24 * (f32)t1->m20;
    f32 x = a->current.pos.x;
    t0 = attr(a, file, expr);
    t1 = attr(a, file, expr);
    f32 y = gabi::fmadds((f32)t0->m14 * (f32)t1->m50, mag, a->current.pos.y);
    f32 z = a->current.pos.z;
    pos->set(x, y, z);
    scale->set(mag, mag, mag);
}

// HD: static callbacks use free adapters until autoglue supports their class declarations.
static bool solidHeapCB(Act_c *);
static void tg_hitCB(Act_c *, dCcD_GObjInf *, fopAc_ac_c *, dCcD_GObjInf *);
static void co_hitCB(Act_c *, dCcD_GObjInf *, fopAc_ac_c *, dCcD_GObjInf *);

/* 0231E7FC */
static bool solidHeapCB(Act_c *a) { return a->create_heap(); }

/* 0231E670 */
bool daObjBarrel2::Act_c::create_heap() {
    Attr_c *t = attr(this, "d_a_obj_barrel2.cpp", "(M_type >= 0) && (M_type < Type_Max)");
    u16 index = t->m00;
    J3DModelData *mdl =
        (J3DModelData *)dComIfG_getObjectRes("Ktaru_00", index, barrel2_hd::ResourceStringVtable);
    if (!mdl) {
        Barrel_assertion("d_a_obj_barrel2.cpp", 0x217, "mdl_data != 0");
    }
    m298 = mDoExt_J3DModel__create(mdl, 0x80000, 0x11000022);
    t = attr(this, "d_a_obj_barrel2.cpp", "(M_type >= 0) && (M_type < Type_Max)");
    index = t->m02;
    void *brk = dComIfG_getObjectRes("Ktaru_00", index, barrel2_hd::ResourceStringVtable);
    if (!brk) {
        Barrel_assertion("d_a_obj_barrel2.cpp", 0x221, "brk_data != 0");
    }
    void *anm = Barrel_allocate(0x78);
    if (anm) {
        anm = Barrel_brkConstruct(anm);
    }
    m29C = (mDoExt_brkAnm *)anm;
    s32 result = 0;
    if (anm) {
        result = Barrel_brkInit(anm, mdl, brk, 1, 0, 1.f, 0, -1, 0, 0);
    }
    return m298 != nullptr && result != 0;
}

/* 0231F350 */
s32 daObjBarrel2::Act_c::_create() {
    u32 condition = actor_condition;
    if (!(condition & 8)) {
        if (gabi::ea(this) != 0) {
            fopAc_ac_c_ct(this);
            __vtbl = barrel2_hd::ActorVtable;
            Barrel_statusConstruct(&mStts);
            Barrel_gameStatusConstruct(&mStts.__vtbl_gstts);
            mStts.__vtbl = barrel2_hd::StatusVtable;
            mStts.__vtbl_gstts = barrel2_hd::GameStatusVtable;
            Barrel_cylinderConstruct(&mCyl);
            gabi::at<barrel2_hd::ShapeVtables>(gabi::ea(&mCyl))->mAabVtable =
                barrel2_hd::AabBaseVtable;
            gabi::at<barrel2_hd::ShapeVtables>(gabi::ea(&mCyl))->mShapeVtable =
                barrel2_hd::ShapeVtable;
            Barrel_cylinderShapeConstruct(&mCyl.mCyl);
            condition = actor_condition;
            gabi::at<barrel2_hd::ShapeVtables>(gabi::ea(&mCyl))->mAabVtable = barrel2_hd::AabVtable;
            mCyl.__vtbl_hitinf = barrel2_hd::HitInfoVtable;
            mCyl.mCyl.__vtbl = barrel2_hd::CylinderVtable;
        }
        actor_condition = condition | 8;
    }
    m410 = PrmAbstract(this, PRM_TYPE_W, PRM_TYPE_S);
    s32 phase = dComIfG_resLoad(&mPhase, "Ktaru_00");
    if (phase != 4) {
        return phase;
    }
    auto attribute = [&]() {
        return attr(this, "d_a_obj_barrel2.cpp", "(M_type >= 0) && (M_type < Type_Max)");
    };
    Attr_c *t = attribute();
    if (!Barrel_entrySolidHeap(this, gabi::at<void>(gabi::guest_fn((const void *)solidHeapCB)),
                               (u32)t->m04)) {
        return 5;
    }
    J3DModel *mdl = m298;
    cullMtx = mdl ? gabi::ea(mdl) + 0xC8 : 0;
    mStts.Init(200, 255, this);
    Barrel_setCylinder(&mCyl, &barrelCylinderSource.mData);
    mCyl.mStts = &mStts;
    t = attribute();
    Barrel_setCylinderRadius(&mCyl.mCyl, (f32)t->m08);
    t = attribute();
    Barrel_setCylinderHeight(&mCyl.mCyl, (f32)t->m0C);
    gbaName = 0x3D;
    mCyl.mGObjTg.mHitCallback = gabi::guest_fn((const void *)tg_hitCB);
    mCyl.mGObjCo.mHitCallback = gabi::guest_fn((const void *)co_hitCB);
    if (m410 == 3) {
        actor_status = actor_status & ~0x180u;
    }
    BOOL coming = PrmAbstract(this, PRM_COMING_W, PRM_COMING_S);
    if (coming) {
        Attr_c *t0 = attribute();
        Attr_c *t1 = attribute();
        Attr_c *t2 = attribute();
        m450 = -(((f32)t0->m18 * (f32)t1->m20) * (f32)t2->m24) - 50.f;
        if (m410 == 2) {
            m440 = cM_rndFX(1.f);
            m444 = cM_rndFX(1.f);
        } else {
            m440 = 0.f;
            m444 = 0.f;
        }
    } else {
        m450 = 0.f;
        m440 = 0.f;
        m444 = 0.f;
    }
    m448 = 0.f;
    m44C = 0.f;
    m42C = 0.f;
    m430 = 0.f;
    m434 = 0.f;
    m438 = 0.f;
    set_water_pos();
    set_pos_y();
    attention_info.position.x = current.pos.x;
    Attr_c *t0 = attribute();
    Attr_c *t1 = attribute();
    Attr_c *t2 = attribute();
    f32 z = current.pos.z;
    f32 y = gabi::fmadds((f32)t0->m18 * (f32)t1->m20, t2->m24, current.pos.y);
    f32 x = attention_info.position.x;
    eyePos.z = z;
    eyePos.x = x;
    attention_info.position.z = z;
    eyePos.y = y;
    attention_info.position.y = y;
    t0 = attribute();
    t1 = attribute();
    t2 = attribute();
    f32 cy = ((f32)t0->m18 * (f32)t1->m24) * (f32)t2->m20;
    Barrel_setCullSphere(this, 0.f, cy, 0.f, 300.f);
    gabi::Local<cXyz> pos, scale;
    pos->set(current.pos.x, current.pos.y, current.pos.z);
    gabi::Local<csXyz> angle;
    Barrel_angleConstruct(angle.get(), (s16)0, (s16)home.angle.y, (s16)0);
    t0 = attribute();
    t1 = attribute();
    f32 mag = (f32)t0->m28 * (f32)t1->m2C;
    scale->set(mag, mag, mag);
    s32 item = PrmAbstract(this, PRM_ITEM_NO_W, PRM_ITEM_NO_S);
    s32 save = PrmAbstract(this, PRM_ITEM_SAVE_W, PRM_ITEM_SAVE_S);
    BOOL spawn = PrmAbstract(this, PRM_COMING_W, PRM_COMING_S);
    s8 room = home.roomNo;
    mItemId =
        Barrel_createRaceItem(pos.get(), item, save, room, angle.get(), scale.get(), spawn != 0);
    m468 = 0;
    m460 = fpcM_ERROR_PROCESS_ID_e;
    m470 = 0;
    m45C = barrelItemDistanceMax;
    if (!PrmAbstract(this, PRM_BUOY_W, PRM_BUOY_S)) {
        f32 x = current.pos.x;
        t0 = attribute();
        t1 = attribute();
        t2 = attribute();
        f32 mag = (f32)t1->m24 * (f32)t2->m20;
        gabi::Local<cXyz> flagpos;
        flagpos->set(x, gabi::fmadds((f32)t0->m14 - 5.f, mag, current.pos.y), current.pos.z);
        s32 type = m410;
        u32 param = (type == 2 || type == 3) ? 2 : 1;
        if (type == 3) {
            param |= 0x80000000;
        }
        u32 parent = (u32)mBsPcId;
        s8 room = current.roomNo;
        s32 texture = PrmAbstract(this, PRM_TEXTURE_W, PRM_TEXTURE_S);
        param |= (u32)texture << 8;
        m460 = Barrel_createChild(0x1D1, parent, param, flagpos.get(), room, &shape_angle, nullptr,
                                  -1, nullptr);
    }
    init_mtx();
    u32 play = dComIfGp_ea();
    m46A = (s16)gabi::at<dComIfG_inf_c>(play)->play.mCurStage.mPoint;
    mode_afl_init();
    return phase;
}

/* 0231FB78 */
bool daObjBarrel2::Act_c::_delete() {
    dComIfG_resDelete(&mPhase, "Ktaru_00");
    return true;
}

/* 0231E800 */
static void tg_hitCB(Act_c *a, dCcD_GObjInf *obj, fopAc_ac_c *other, dCcD_GObjInf *otherobj) {
    void *hit = obj->GetTgHitObj();
    if (hit && ((u32) static_cast<dCcD_GObjInf *>(hit)->mAtType & 0x20)) {
        a->m470 = 3;
    }
}

/* 0231E84C */
static void co_hitCB(Act_c *a, dCcD_GObjInf *obj, fopAc_ac_c *other, dCcD_GObjInf *otherobj) {
    if (!other || (s16)other->mProfName != 0xA5) {
        return;
    }
    if (a->m410 != 0) {
        a->m470 = 2;
        return;
    }
    Attr_c *t = attr(a, "d_a_obj_barrel2.cpp", "(M_type >= 0) && (M_type < Type_Max)");
    if (!((f32)other->speedF > (f32)t->m38)) {
        return;
    }
    gabi::Local<cXyz> delta, velocity;
    delta->x = (f32)other->current.pos.x - (f32)a->current.pos.x;
    delta->y = 0;
    delta->z = (f32)other->current.pos.z - (f32)a->current.pos.z;
    velocity->set(other->speed.x, 0, other->speed.z);
    Attr_c *t0 = attr(a, "d_a_obj_barrel2.cpp", "(M_type >= 0) && (M_type < Type_Max)");
    Attr_c *t1 = attr(a, "d_a_obj_barrel2.cpp", "(M_type >= 0) && (M_type < Type_Max)");
    f32 sq = (f32)t0->m3C * (f32)t1->m3C;
    if (Barrel_innerProduct(delta.get(), velocity.get()) < sq) {
        a->m470 = 1;
        auto *shipFlags = gabi::at<barrel2_hd::Ship>(gabi::ea(other));
        shipFlags->mFlags = shipFlags->mFlags | 4;
        return;
    }
    if (a->m46D != 0) {
        return;
    }
    auto *shipFlags = gabi::at<barrel2_hd::Ship>(gabi::ea(other));
    shipFlags->mFlags = shipFlags->mFlags | 0x20;
    a->m454 = -60.f;
    a->m46D = 20;
    if (a && gabi::ea(&a->eyePos)) {
        s32 reverb = Barrel_reverb((s8)a->current.roomNo);
        Barrel_sound(0x6944, &a->eyePos, 0, reverb);
    }
}

/* 02320EDC */
void daObjBarrel2::Act_c::cull_set_draw() {
    Attr_c *t[5];
    for (int i = 0; i < 5; i++) {
        t[i] = attr(this, "d_a_obj_barrel2.cpp", "(M_type >= 0) && (M_type < Type_Max)");
    }
    f32 y = ((f32)t[0]->m18 * (f32)t[1]->m24) * (f32)t[2]->m20;
    Barrel_setCullSphere(this, 0.f, y, 0.f, (75.f * (f32)t[3]->m24) * (f32)t[4]->m20);
}

/* 0231FBA8 */
void daObjBarrel2::Act_c::cull_set_move() {
    Attr_c *t[4];
    for (int i = 0; i < 4; i++) {
        t[i] = attr(this, "d_a_obj_barrel2.cpp", "(M_type >= 0) && (M_type < Type_Max)");
    }
    f32 y = ((f32)t[0]->m18 * (f32)t[1]->m24) * (f32)t[2]->m20;
    Barrel_setCullSphere(this, 0.f, y, 0.f, (f32)t[3]->m10);
}

/* 0231FD50 */
bool daObjBarrel2::Act_c::demo_mode_chk() {
    u32 mode = m40C;
    return !(mode < 1 || (mode > 3 && (mode < 5 || mode > 8)));
}

/* 0231F344 */
void daObjBarrel2::Act_c::mode_afl_init() { m40C = MODE_AFL; }

/* 02321564 */
bool daObjBarrel2::Act_c::mode_afl() {
    BOOL coming = PrmAbstract(this, PRM_COMING_W, PRM_COMING_S);
    f32 pos = m450, vel = m454;
    f32 limit;
    if (coming) {
        f32 force = (-pos >= 0.f) ? -0.01f : -0.02f;
        vel = gabi::fmadds(pos, force, vel) * 0.92f;
        limit = 0.6f;
    } else {
        vel = gabi::fmadds(pos, -0.01f, vel) * 0.94f;
        limit = 0.2f;
    }
    pos += vel;
    m454 = vel;
    m450 = pos;
    Attr_c *t = attr(this, "d_a_obj_barrel2.cpp", "(M_type >= 0) && (M_type < Type_Max)");
    if (m450 > (f32)t->m14 * limit) {
        t = attr(this, "d_a_obj_barrel2.cpp", "(M_type >= 0) && (M_type < Type_Max)");
        m450 = (f32)t->m14 * limit;
    }
    t = attr(this, "d_a_obj_barrel2.cpp", "(M_type >= 0) && (M_type < Type_Max)");
    f32 step = (f32)(s16)t->m58;
    f32 rnd = cM_rnd();
    m43C = (s16)((s16)m43C + (s16)gabi::ftoi(step * (rnd + 1.f)));
    set_pos_y();
    f32 x = current.pos.x, z = current.pos.z;
    f32 vx = gabi::fmadds(x - (f32)home.pos.x, -0.002f, m414) * 0.9f;
    f32 vz = gabi::fmadds(z - (f32)home.pos.z, -0.002f, m418) * 0.9f;
    f32 nx = x + ((f32)mStts.m_cc_move.x + vx);
    f32 nz = z + ((f32)mStts.m_cc_move.z + vz);
    m414 = vx;
    m418 = vz;
    current.pos.x = nx;
    current.pos.z = nz;
    afl_sway();
    set_item_position();
    return true;
}

/* 02320918 */
void daObjBarrel2::Act_c::mode_exit_v_init() {
    m40C = MODE_EXIT_V;
    m464 = 4;
    m454 = barrelExitSpeed;
}

/* 0232183C */
bool daObjBarrel2::Act_c::mode_exit_v() {
    f32 velocity = ((f32)m454 - 5.f) * 0.96f;
    f32 pos = (f32)m450 + velocity;
    m454 = velocity;
    m450 = pos;
    set_pos_y();
    set_item_position();
    Attr_c *t[4];
    for (int i = 0; i < 4; i++) {
        t[i] = attr(this, "d_a_obj_barrel2.cpp", "(M_type >= 0) && (M_type < Type_Max)");
    }
    f32 lower = -(((f32)t[0]->m18 * (f32)t[1]->m20) * (f32)t[2]->m24) - (f32)t[3]->m30;
    if ((f32)m450 > lower) {
        return true;
    }
    item_drop_init(0.f);
    buoy_jump(0.f);
    return false;
}

/* 02320938 */
void daObjBarrel2::Act_c::mode_exit_h_init() {
    m40C = MODE_EXIT_H;
    m454 = barrelExitSpeed;
}

/* 023219D8 */
bool daObjBarrel2::Act_c::mode_exit_h() {
    f32 velocity = ((f32)m454 - 5.f) * 0.94f;
    f32 pos = (f32)m450 + velocity;
    m454 = velocity;
    m450 = pos;
    set_pos_y();
    set_item_position();
    Attr_c *t[4];
    for (int i = 0; i < 4; i++) {
        t[i] = attr(this, "d_a_obj_barrel2.cpp", "(M_type >= 0) && (M_type < Type_Max)");
    }
    f32 lower = -(((f32)t[0]->m18 * (f32)t[1]->m20) * (f32)t[2]->m24) - (f32)t[3]->m30;
    if ((f32)m450 > lower) {
        return true;
    }
    item_drop_init(0.f);
    buoy_jump(0.f);
    return false;
}

/* 02320950 */
void daObjBarrel2::Act_c::mode_exit_mine_init() {
    m40C = MODE_EXIT_MINE;
    m454 = barrelMineExitSpeed;
}

/* 02321B74 */
bool daObjBarrel2::Act_c::mode_exit_mine() {
    f32 velocity = ((f32)m454 - 5.f) * 0.94f;
    f32 pos = (f32)m450 + velocity;
    m454 = velocity;
    m450 = pos;
    set_pos_y();
    set_item_position();
    Attr_c *t[3];
    for (int i = 0; i < 3; i++) {
        t[i] = attr(this, "d_a_obj_barrel2.cpp", "(M_type >= 0) && (M_type < Type_Max)");
    }
    f32 lower = -(((f32)t[0]->m18 * (f32)t[1]->m20) * (f32)t[2]->m24) - 50.f;
    if ((f32)m450 > lower) {
        return true;
    }
    item_drop_init(0.f);
    buoy_jump(0.f);
    return false;
}

/* 023203B0 */
void daObjBarrel2::Act_c::mode_explode_init() {
    m464 = 30;
    eff_explode();
    buoy_jump(300.f);
    item_drop_init(130.f);
    u32 co = mCyl.mObjCo.mSPrm;
    m40C = MODE_EXPLODE;
    mCyl.mObjCo.mSPrm = co & ~1u;
}

/* 02321D54 */
bool daObjBarrel2::Act_c::mode_explode() {
    m464 = (u32)m464 - 1;
    if (m464 < 0) {
        item_drop();
        return false;
    }
    f32 pos = m450;
    f32 velocity = gabi::fmadds(pos, -0.01f, m454) * 0.94f;
    pos += velocity;
    m454 = velocity;
    m450 = pos;
    Attr_c *t = attr(this, "d_a_obj_barrel2.cpp", "(M_type >= 0) && (M_type < Type_Max)");
    if (m450 > (f32)t->m14 * 0.2f) {
        t = attr(this, "d_a_obj_barrel2.cpp", "(M_type >= 0) && (M_type < Type_Max)");
        m450 = (f32)t->m14 * 0.2f;
    }
    t = attr(this, "d_a_obj_barrel2.cpp", "(M_type >= 0) && (M_type < Type_Max)");
    f32 step = (f32)(s16)t->m58;
    f32 rnd = cM_rnd();
    m43C = (s16)((s16)m43C + (s16)gabi::ftoi(step * (rnd + 1.f)));
    set_pos_y();
    item_drop();
    return true;
}

/* 02320890 */
void daObjBarrel2::Act_c::mode_demo_break0_init() {
    u32 tg = mCyl.mObjTg.mSPrm;
    u32 co = mCyl.mObjCo.mSPrm;
    m40C = MODE_DEMO_BREAK0;
    mCyl.mObjTg.mSPrm = tg & ~1u;
    m464 = 1;
    mCyl.mObjCo.mSPrm = co & ~1u;
    eff_break();
}

/* 02321F44 */
bool daObjBarrel2::Act_c::mode_demo_break0() {
    if (exists_demo()) {
        if (eventInfo_checkCommandDemoAccrpt(this)) {
            item_give();
            mode_demo_break1_init();
            return true;
        }
        m464 = (u32)m464 - 1;
        if (m464 >= 0) {
            Barrel_orderOtherEvent(this, "barrel2_brk", (u16)1, (u16)0xFFFF);
            eventInfo_onCondition(this, 2);
            return true;
        }
    }
    item_give();
    return false;
}

/* 02321F38 */
void daObjBarrel2::Act_c::mode_demo_break1_init() { m40C = MODE_DEMO_BREAK1; }

/* 02322024 */
bool daObjBarrel2::Act_c::mode_demo_break1() { return end_demo(); }

/* 0232061C */
void daObjBarrel2::Act_c::mode_demo_explode0_init() {
    u32 co = mCyl.mObjCo.mSPrm;
    u32 tg = mCyl.mObjTg.mSPrm;
    m40C = MODE_DEMO_EXPLODE0;
    mCyl.mObjCo.mSPrm = co & ~1u;
    m464 = 2;
    mCyl.mObjTg.mSPrm = tg & ~1u;
}

/* 02322090 */
bool daObjBarrel2::Act_c::mode_demo_explode0() {
    item_drop();
    if (!exists_demo()) {
        return false;
    }
    if (eventInfo_checkCommandDemoAccrpt(this)) {
        mode_demo_explode1_init();
        return true;
    }
    m464 = (u32)m464 - 1;
    if (m464 < 0) {
        return false;
    }
    if (m464 == 0) {
        Barrel_orderOtherEvent(this, "barrel2_brk", (u16)1, (u16)0xFFFF);
        eventInfo_onCondition(this, 2);
    }
    return true;
}

/* 02322084 */
void daObjBarrel2::Act_c::mode_demo_explode1_init() { m40C = MODE_DEMO_EXPLODE1; }

/* 02322168 */
bool daObjBarrel2::Act_c::mode_demo_explode1() {
    item_drop();
    return end_demo();
}

/* 02320968 */
BOOL daObjBarrel2::Act_c::mode_proc_call() {
    set_water_pos();
    // HD: the descriptor dispatch retains a full BOOL result register.
    auto *mode = gabi::at<barrel2_hd::ModePtmf>(gabi::ea(&barrelModes) +
                                                (u32)m40C * sizeof(barrel2_hd::ModePtmf));
    return (this->*(*mode))();
}

/* 0231ED94 */
void daObjBarrel2::Act_c::set_pos_y() {
    Attr_c *t0 = attr(this, "d_a_obj_barrel2.cpp", "(M_type >= 0) && (M_type < Type_Max)");
    Attr_c *t1 = attr(this, "d_a_obj_barrel2.cpp", "(M_type >= 0) && (M_type < Type_Max)");
    Attr_c *t2 = attr(this, "d_a_obj_barrel2.cpp", "(M_type >= 0) && (M_type < Type_Max)");
    f32 sin = (f32)barrelSinCos.mData[(u16)m43C >> 3].mSin;
    Attr_c *t3 = attr(this, "d_a_obj_barrel2.cpp", "(M_type >= 0) && (M_type < Type_Max)");
    Attr_c *t4 = attr(this, "d_a_obj_barrel2.cpp", "(M_type >= 0) && (M_type < Type_Max)");
    f32 x = (f32)t0->m50 * (f32)t1->m14;
    f32 sway = gabi::fmsubs(t2->m54, sin, x);
    f32 y = (f32)m41C + (f32)m450;
    current.pos.y = gabi::fmadds(sway * (f32)t3->m20, t4->m24, y);
}

/* 0231EEF4 */
void daObjBarrel2::Act_c::set_mtx() {
    s32 type = m410;
    bool upright = type == 0 || type == 2 || type == 3;
    Attr_c *t0 = attr(this, "d_a_obj_barrel2.cpp", "(M_type >= 0) && (M_type < Type_Max)");
    Attr_c *t1 = attr(this, "d_a_obj_barrel2.cpp", "(M_type >= 0) && (M_type < Type_Max)");
    f32 mag = (f32)t0->m24 * (f32)t1->m20;
    gabi::Local<cXyz> scaled;
    Barrel_scaleVector(&scale, scaled.get(), mag);
    void *buoy = nullptr;
    if (m460 != fpcM_ERROR_PROCESS_ID_e) {
        gabi::Local<be<u32>> id;
        *id = (u32)m460;
        buoy = Barrel_searchActor(gabi::at<void>(barrel2_hd::SearchByIdJudge), id.get());
    }
    J3DModel_setBaseScale(m298, scaled.get());
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    gabi::Local<cXyz> axis;
    axis->set(m440, 1.f, m444);
    Attr_c *t = attr(this, "d_a_obj_barrel2.cpp", "(M_type >= 0) && (M_type < Type_Max)");
    mDoMtx_stack_c::transM(0.f, (f32)t->m18 * mag, 0.f);
    gabi::Local<u8[16]> quat;
    Barrel_quaternionRotBaseY(quat.get(), axis.get());
    Barrel_quaternionMatrix(quat.get());
    s16 z = shape_angle.z;
    if (!upright) {
        z = (s16)(z + 0x4000);
    }
    Barrel_zxyRotation(mDoMtx_stack_c::get(), (s16)shape_angle.x, (s16)shape_angle.y, z);
    t = attr(this, "d_a_obj_barrel2.cpp", "(M_type >= 0) && (M_type < Type_Max)");
    mDoMtx_stack_c::transM(0.f, -((f32)t->m18 * mag), 0.f);
    J3DModel_setBaseTRMtx(m298, mDoMtx_stack_c::get());
    Barrel_copyMatrix(mDoMtx_stack_c::get(), &m478);
    if (buoy) {
        gabi::Local<cXyz> offset, out;
        if (upright) {
            t = attr(this, "d_a_obj_barrel2.cpp", "(M_type >= 0) && (M_type < Type_Max)");
            offset->set(0.f, ((f32)t->m14 - 5.f) * mag, 0.f);
        } else {
            Barrel_zRotation(mDoMtx_stack_c::get(), (s16)-0x4000);
            t = attr(this, "d_a_obj_barrel2.cpp", "(M_type >= 0) && (M_type < Type_Max)");
            offset->set(-((f32)t->m18 * mag), 45.f * mag, 0.f);
        }
        Barrel_matrixVectorSR(mDoMtx_stack_c::get(), offset.get(), out.get());
        Mtx34 *m = &barrelStackMatrix;
        f32 y = (f32)m->m[1][3] + (f32)out->y;
        f32 x = (f32)m->m[0][3] + (f32)out->x;
        f32 z = (f32)m->m[2][3] + (f32)out->z;
        m->m[1][3] = y;
        m->m[0][3] = x;
        m->m[2][3] = z;
        Barrel_copyMatrix(mDoMtx_stack_c::get(), &static_cast<daObjBuoyflag::Act_c *>(buoy)->mMtx);
    }
}

/* 0231F340 */
void daObjBarrel2::Act_c::init_mtx() { set_mtx(); }

/* 02320648 */
void daObjBarrel2::Act_c::eff_break() {
    barrel_sound(this, 0x691E);
    gabi::Local<cXyz> pos, scale;
    effect_position(this, "d_a_obj_barrel2.cpp", "(M_type >= 0) && (M_type < Type_Max)", pos.get(),
                    scale.get());
    barrel_particle(0x460, pos.get(), nullptr, scale.get());
    barrel_particle(0x45F, pos.get(), nullptr, scale.get());
    void *emitter = barrel_particle(0x3E6, pos.get(), nullptr, scale.get(), 0, &tevStr.mColorK0);
    if (emitter) {
        auto *effect = static_cast<barrel2_hd::Emitter *>(emitter);
        effect->mAwayFromAxisSpeed = 5.f;
        effect->mDirectionalSpeed = 25.f;
        effect->mLifeTime = 30;
    }
}

/* 0231FE60 */
void daObjBarrel2::Act_c::eff_explode() {
    gabi::Local<cXyz> pos, scale;
    effect_position(this, "d_a_obj_barrel2.cpp", "(M_type >= 0) && (M_type < Type_Max)", pos.get(),
                    scale.get());
    barrel_sound(this, 0x6938);
    u32 play = dComIfGp_ea();
    s8 id = (s8)gabi::at<dComIfG_inf_c>(play)->play.mPlayerInfo[0].mCameraID;
    play = dComIfGp_ea();
    auto *cameraInfo = gabi::at<dComIfG_camera_info_class>(
        gabi::ea(&gabi::at<dComIfG_inf_c>(play)->play.mCameraInfo) +
        (u32)(s32)id * sizeof(dComIfG_camera_info_class));
    auto *cam = gabi::at<camera_class>(cameraInfo->mpCamera);
    gabi::Local<csXyz> angle;
    angle->x = (s16) - (s16)cam->mAngle.x;
    angle->y = (s16)((s16)cam->mAngle.y + 0x8000);
    angle->z = 0;
    barrel_particle(0xB, pos.get(), angle.get(), scale.get());
    play = dComIfGp_ea();
    void *ctrl = gabi::at<dComIfG_inf_c>(play)->play.mParticle.get();
    Barrel_particleBombSmoke(ctrl, 0x200A, pos.get(), nullptr, scale.get(), (u8)0xFF);
    Attr_c *t0 = attr(this, "d_a_obj_barrel2.cpp", "(M_type >= 0) && (M_type < Type_Max)");
    Attr_c *t1 = attr(this, "d_a_obj_barrel2.cpp", "(M_type >= 0) && (M_type < Type_Max)");
    Barrel_createWaterPillar(&current.pos, (f32)t0->m6C, (f32)t1->m70, 1);
    barrel_particle(0x2041, pos.get(), nullptr, scale.get(), 2);
    barrel_particle(0x3C, pos.get(), nullptr, scale.get());
    void *emitter = barrel_particle(0x3E6, pos.get(), nullptr, scale.get());
    if (emitter) {
        auto *effect = static_cast<barrel2_hd::Emitter *>(emitter);
        effect->mSpread = 0.4f;
        effect->mDirectionalSpeed = 25.f;
        effect->mAwayFromAxisSpeed = 10.f;
        effect->mLifeTime = 40;
    }
}

/* 023213F8 */
void daObjBarrel2::Act_c::set_item_position() {
    fopAc_ac_c *item = tmp_item();
    if (!item) {
        m45C = barrelItemDistanceMax;
        return;
    }
    u8 event = m46C;
    f32 x = current.pos.x;
    if (event) {
        Attr_c *t = attr(this, "d_a_obj_barrel2.cpp", "(M_type >= 0) && (M_type < Type_Max)");
        gabi::Local<cXyz> pos;
        pos->x = x;
        pos->y = (f32)current.pos.y + (f32)t->m30;
        pos->z = current.pos.z;
        Barrel_setItemMatrix(item, pos.get());
    } else {
        item->current.pos.x = x;
        Attr_c *t = attr(this, "d_a_obj_barrel2.cpp", "(M_type >= 0) && (M_type < Type_Max)");
        item->current.pos.y = (f32)current.pos.y + (f32)t->m30;
        f32 z = current.pos.z;
        item->speed.y = 0.f;
        item->current.pos.z = z;
    }
    Barrel_startOffsetPos(item);
    u32 play = dComIfGp_ea();
    void *player =
        reinterpret_cast<dComIfG_inf_c *>(gabi::at<void>(play))->play.mPlayerInfo[0].mpPlayer.get();
    m45C = Barrel_playerDistance2(item, player);
}

/* 02320294 */
void daObjBarrel2::Act_c::item_drop_init(f32 speed) {
    fopAc_ac_c *item = tmp_item();
    if (!item) {
        m45C = barrelItemDistanceMax;
        return;
    }
    Attr_c *t0 = attr(this, "d_a_obj_barrel2.cpp", "(M_type >= 0) && (M_type < Type_Max)");
    Attr_c *t1 = attr(this, "d_a_obj_barrel2.cpp", "(M_type >= 0) && (M_type < Type_Max)");
    gabi::Local<cXyz> scale;
    f32 v = (f32)t0->m28 * (f32)t1->m2C;
    scale->set(v, v, v);
    Barrel_endOffsetPos(item, -7.f, scale.get(), speed, 0.f, &barrelZeroAngles.mZero);
    u32 play = dComIfGp_ea();
    void *player =
        reinterpret_cast<dComIfG_inf_c *>(gabi::at<void>(play))->play.mPlayerInfo[0].mpPlayer.get();
    m45C = Barrel_playerDistance2(item, player);
}

/* 02321CEC */
void daObjBarrel2::Act_c::item_drop() {
    fopAc_ac_c *item = tmp_item();
    if (item) {
        u32 play = dComIfGp_ea();
        fopAc_ac_c *player = reinterpret_cast<dComIfG_inf_c *>(gabi::at<void>(play))
                                 ->play.mPlayerInfo[0]
                                 .mpPlayer.get();
        m45C = Barrel_playerDistance2(item, player);
    } else {
        m45C = barrelItemDistanceMax;
    }
}

/* 02320AC0 */
u32 daObjBarrel2::Act_c::item_delete() {
    fopAc_ac_c *item = tmp_item();
    if (!item) {
        return 0;
    }
    return Barrel_deleteActor(item);
}

/* 023208BC */
void daObjBarrel2::Act_c::item_give() {
    fopAc_ac_c *item = tmp_item();
    if (item) {
        Barrel_raceItemForceGet(item);
        mItemId = fpcM_ERROR_PROCESS_ID_e;
        m476 = 1;
        tmp_item() = nullptr;
    }
}

/* 0231FCAC */
void daObjBarrel2::Act_c::item_connect_check() {
    tmp_item() = nullptr;
    if (mItemId != fpcM_ERROR_PROCESS_ID_e) {
        gabi::Local<gptr<fopAc_ac_c>> item;
        if (Barrel_searchById((u32)mItemId, item.get())) {
            fopAc_ac_c *found = *item;
            if (found) {
                if (Barrel_checkOffsetPos(found)) {
                    tmp_item() = found;
                } else {
                    mItemId = fpcM_ERROR_PROCESS_ID_e;
                }
            }
        } else {
            mItemId = fpcM_ERROR_PROCESS_ID_e;
        }
    }
}

/* 023201B0 */
void daObjBarrel2::Act_c::buoy_jump(f32 speed) {
    if (m460 == fpcM_ERROR_PROCESS_ID_e) {
        return;
    }
    gabi::Local<be<u32>> id;
    *id = (u32)m460;
    void *buoy = Barrel_searchActor(gabi::at<void>(barrel2_hd::SearchByIdJudge), id.get());
    if (buoy) {
        s16 angle = (s16)gabi::ftoi(cM_rndFX(32768.f));
        auto *flag = static_cast<daObjBuoyflag::Act_c *>(buoy);
        flag->speed.y = speed;
        flag->speedF = 50.f;
        flag->current.angle.y = angle;
        Barrel_copyMatrix(&flag->mMtx, &flag->mJumpMtx);
        u32 status = (u32)flag->actor_status;
        flag->mbInit = 1;
        flag->mJumpMtx.m[0][3] = 0.f;
        flag->mMode = 1;
        flag->mJumpMtx.m[1][3] = 0.f;
        flag->mJumpMtx.m[2][3] = 0.f;
        flag->actor_status = status & ~0x80u;
    }
    m460 = fpcM_ERROR_PROCESS_ID_e;
}

/* 0231EA58 */
void daObjBarrel2::Act_c::set_water_pos() {
    f32 x = current.pos.x, z = current.pos.z;
    if (Barrel_seaCheckArea(x, z)) {
        x = current.pos.x;
        z = current.pos.z;
        f32 w0 = Barrel_seaCalcWave(x - 32.f, z - 32.f);
        x = current.pos.x;
        z = current.pos.z;
        f32 w1 = Barrel_seaCalcWave(x - 32.f, z + 32.f);
        x = current.pos.x;
        z = current.pos.z;
        f32 w2 = Barrel_seaCalcWave(x + 32.f, z - 32.f);
        gabi::Local<cXyz> u, v, cross;
        u->set(0.f, w1 - w0, 32.f);
        v->set(32.f, w2 - w0, 0.f);
        m41C = ((w0 + w1) + w2) * 0.3333333432674408f;
        Barrel_outProduct(u.get(), cross.get(), v.get());
        m420.copy(*cross);
        Barrel_normalizeZP(&m420, cross.get());
        return;
    }
    auto *check = &barrelWater;
    if ((u32)barrelWaterGuard == 0) {
        barrelWaterGuard = 1;
        Barrel_waterCheckConstruct(check);
        Barrel_destructorChain(&barrelChains.mData[0]);
    }
    x = current.pos.x;
    f32 y = current.pos.y;
    z = current.pos.z;
    check->mPos.x = x;
    check->mPos.z = z;
    check->mPos.y = y - 1000.f;
    check->mUpperY = y + 1000.f;
    u32 play = dComIfGp_ea();
    BOOL found = Barrel_specialGroupCheck(&gabi::at<dComIfG_inf_c>(play)->play.mBgS, check);
    m41C = found ? (f32)check->mHeight : 0.f;
    f32 oldx = m42C;
    f32 random = cM_rndFX(0.001f);
    f32 ax = gabi::fmadds(oldx, -0.2f, random);
    f32 oldz = m430;
    random = cM_rndFX(0.001f);
    f32 az = gabi::fmadds(oldz, -0.2f, random);
    f32 vx = ((f32)m434 + ax) * 0.99f;
    f32 px = (f32)m42C + vx;
    f32 vz = ((f32)m438 + az) * 0.99f;
    f32 pz = (f32)m430 + vz;
    m434 = vx;
    m42C = px;
    m420.y = 1.f;
    m420.x = px;
    m438 = vz;
    m430 = pz;
    m420.z = pz;
    gabi::Local<cXyz> out;
    Barrel_normalize(&m420, out.get());
}

/* 0232115C */
void daObjBarrel2::Act_c::afl_sway() {
    Attr_c *t0 = attr(this, "d_a_obj_barrel2.cpp", "(M_type >= 0) && (M_type < Type_Max)");
    Attr_c *t1 = attr(this, "d_a_obj_barrel2.cpp", "(M_type >= 0) && (M_type < Type_Max)");
    f32 maxsq = (f32)t0->m5C * (f32)t1->m5C;
    Attr_c *t = attr(this, "d_a_obj_barrel2.cpp", "(M_type >= 0) && (M_type < Type_Max)");
    f32 x = (f32)m420.x * (f32)t->m68;
    t = attr(this, "d_a_obj_barrel2.cpp", "(M_type >= 0) && (M_type < Type_Max)");
    f32 z = (f32)m420.z * (f32)t->m68;
    f32 sq = gabi::fmadds(x, x, z * z);
    if (sq > maxsq) {
        t = attr(this, "d_a_obj_barrel2.cpp", "(M_type >= 0) && (M_type < Type_Max)");
        f32 root = Barrel_sqrt(sq);
        f32 ratio = (f32)t->m5C / root;
        x *= ratio;
        z *= ratio;
    }
    x = (f32)m440 - x;
    z = (f32)m444 - z;
    t = attr(this, "d_a_obj_barrel2.cpp", "(M_type >= 0) && (M_type < Type_Max)");
    f32 ax = -(x * (f32)t->m60);
    t = attr(this, "d_a_obj_barrel2.cpp", "(M_type >= 0) && (M_type < Type_Max)");
    f32 az = -(z * (f32)t->m60);
    t = attr(this, "d_a_obj_barrel2.cpp", "(M_type >= 0) && (M_type < Type_Max)");
    f32 dx = -((f32)m448 * (f32)t->m64);
    t = attr(this, "d_a_obj_barrel2.cpp", "(M_type >= 0) && (M_type < Type_Max)");
    f32 vx = (f32)m448 + (ax + dx);
    f32 vz = (f32)m44C + gabi::fnmsubs(m44C, t->m64, az);
    f32 px = (f32)m440 + vx;
    m448 = vx;
    m440 = px;
    f32 pz = (f32)m444 + vz;
    m44C = vz;
    m444 = pz;
}

/* 023209E4 */
bool daObjBarrel2::Act_c::mine_chk_range_flash() {
    u32 play = dComIfGp_ea();
    void *ship = reinterpret_cast<dComIfG_inf_c *>(gabi::at<void>(play))->play.mpPlayerPtr[2].get();
    Attr_c *t0 = attr(this, "d_a_obj_barrel2.cpp", "(M_type >= 0) && (M_type < Type_Max)");
    Attr_c *t1 = attr(this, "d_a_obj_barrel2.cpp", "(M_type >= 0) && (M_type < Type_Max)");
    f32 sq = (f32)t0->m40 * (f32)t1->m40;
    return ship && (Barrel_actorDistanceXZ2(this, ship) < sq);
}

/* 0231FD84 */
bool daObjBarrel2::Act_c::mine_chk_range_explode() {
    u32 play = dComIfGp_ea();
    void *ship = reinterpret_cast<dComIfG_inf_c *>(gabi::at<void>(play))->play.mpPlayerPtr[2].get();
    Attr_c *t0 = attr(this, "d_a_obj_barrel2.cpp", "(M_type >= 0) && (M_type < Type_Max)");
    Attr_c *t1 = attr(this, "d_a_obj_barrel2.cpp", "(M_type >= 0) && (M_type < Type_Max)");
    f32 sq = (f32)t0->m44 * (f32)t1->m44;
    return ship && (Barrel_actorDistanceXZ2(this, ship) < sq);
}

/* 02320418 */
bool daObjBarrel2::Act_c::mine_chk_range_damage() {
    u32 play = dComIfGp_ea();
    fopAc_ac_c *ship =
        reinterpret_cast<dComIfG_inf_c *>(gabi::at<void>(play))->play.mpPlayerPtr[2].get();
    Attr_c *t0 = attr(this, "d_a_obj_barrel2.cpp", "(M_type >= 0) && (M_type < Type_Max)");
    Attr_c *t1 = attr(this, "d_a_obj_barrel2.cpp", "(M_type >= 0) && (M_type < Type_Max)");
    f32 sq = (f32)t0->m48 * (f32)t1->m48;
    if (!ship) {
        return false;
    }
    if (!(Barrel_actorDistanceXZ2(this, ship) < sq)) {
        return false;
    }
    Attr_c *t = attr(this, "d_a_obj_barrel2.cpp", "(M_type >= 0) && (M_type < Type_Max)");
    if (!((f32)ship->current.pos.y < (f32)current.pos.y + (f32)t->m4C)) {
        return false;
    }
    play = dComIfGp_ea();
    if (!((u32)gabi::at<dComIfG_inf_c>(play)->play.mPlayerStatus[0][0] & 0x10000)) {
        return false;
    }
    gabi::Local<cXyz> delta, velocity;
    delta->set((f32)ship->current.pos.x - (f32)current.pos.x, 0,
               (f32)ship->current.pos.z - (f32)current.pos.z);
    velocity->set(ship->speed.x, 0, ship->speed.z);
    t0 = attr(this, "d_a_obj_barrel2.cpp", "(M_type >= 0) && (M_type < Type_Max)");
    t1 = attr(this, "d_a_obj_barrel2.cpp", "(M_type >= 0) && (M_type < Type_Max)");
    sq = (f32)t0->m3C * (f32)t1->m3C;
    if (Barrel_innerProduct(delta.get(), velocity.get()) < sq) {
        auto *shipFlags = gabi::at<barrel2_hd::Ship>(gabi::ea(ship));
        shipFlags->mFlags = shipFlags->mFlags | 4;
        return true;
    }
    return false;
}

/* 02320AD4 */
void daObjBarrel2::Act_c::execute_sub() {
    s32 hit = m470;
    m470 = 0;
    if (m410 == 2) {
        s32 mode = m40C;
        if (mode != 4) {
            BOOL demo = demo_mode_chk();
            if (!demo) {
                if (mine_chk_range_explode()) {
                    mode_explode_init();
                } else if (m40C == MODE_EXPLODE && mine_chk_range_damage()) {
                    mode_demo_explode0_init();
                }
            } else if (mode == 4 && mine_chk_range_damage()) {
                mode_demo_explode0_init();
            }
        } else if (mine_chk_range_damage()) {
            mode_demo_explode0_init();
        }
    }
    if (hit == 1 && !demo_mode_chk()) {
        mode_demo_break0_init();
    } else if (hit == 2) {
        f32 v = m454;
        if (v > 0.f) {
            v = 0.f;
        }
        v -= 8.f;
        if (v < -30.f) {
            v = -30.f;
        }
        m454 = v;
    }
    if (m46D) {
        m46D = (u8)m46D - 1;
    }
    Attr_c *t0 = attr(this, "d_a_obj_barrel2.cpp", "(M_type >= 0) && (M_type < Type_Max)");
    Attr_c *t1 = attr(this, "d_a_obj_barrel2.cpp", "(M_type >= 0) && (M_type < Type_Max)");
    f32 sq = (f32)t0->m34 * (f32)t1->m34;
    if (m45C < sq) {
        item_give();
    }
    if (m474 && m40C == MODE_AFL) {
        if (m410 == 0) {
            mode_exit_v_init();
        } else if (m410 == 1) {
            mode_exit_h_init();
        } else if (m410 == 2 && m468 == 0) {
            mode_exit_mine_init();
        }
    }
    bool remove = false;
    if (hit == 3) {
        eff_break();
        buoy_jump(300.f);
        item_drop_init(130.f);
        remove = true;
    } else if (mode_proc_call()) {
        set_mtx();
        Barrel_setCylinderCenter(&mCyl.mCyl, &current.pos);
        u32 play = dComIfGp_ea();
        Barrel_registerCollision(&gabi::at<dComIfG_inf_c>(play)->play.mCcS, &mCyl);
        attention_info.position.x = current.pos.x;
        Attr_c *t[3];
        for (int i = 0; i < 3; i++) {
            t[i] = attr(this, "d_a_obj_barrel2.cpp", "(M_type >= 0) && (M_type < Type_Max)");
        }
        f32 z = current.pos.z;
        f32 y = gabi::fmadds((f32)t[0]->m18 * (f32)t[1]->m20, t[2]->m24, current.pos.y);
        f32 x = attention_info.position.x;
        attention_info.position.z = z;
        eyePos.z = z;
        eyePos.x = x;
        attention_info.position.y = y;
        eyePos.y = y;
        if (m410 == 2 && mine_chk_range_flash()) {
            m468 = 1;
        }
        if (m40C == MODE_AFL && m468 && Barrel_brkPlay((mDoExt_brkAnm *)m29C)) {
            mode_explode_init();
        }
    } else {
        remove = true;
    }
    if (m475) {
        item_delete();
        remove = true;
    }
    if (remove) {
        fopAcM_delete(this);
    }
}

/* 0232101C */
bool daObjBarrel2::Act_c::_execute() {
    cull_set_move();
    item_connect_check();
    u8 event = 0;
    if (m46A != 1) {
        u32 play = dComIfGp_ea();
        if ((u8)gabi::at<dComIfG_inf_c>(play)->play.mEvtCtrl.mMode) {
            event = 1;
        }
    }
    m46C = event;
    if (m40C != MODE_AFL || !(actor_condition & 4) || !Barrel_cullingCheck(this) || m46C != 0 ||
        m474 != 0 || m475 != 0) {
        execute_sub();
    }
    cull_set_draw();
    return true;
}

/* 023210E0 */
bool daObjBarrel2::Act_c::_draw() {
    if ((u32)m40C <= MODE_EXIT_MINE) {
        settingTevStruct(dKy_getEnvlight(), 0, &current.pos, &tevStr);
        setLightTevColorType(dKy_getEnvlight(), m298, &tevStr);
        J3DModel *mdl = m298;
        mDoExt_brkAnm *anm = m29C;
        J3DModelData *data = mdl->mModelData.get();
        f32 frame = (f32)anm->mFrameCtrl.mFrame;
        mDoExt_brkAnm_entry(anm, data, frame);
        mDoExt_modelUpdateDL(m298);
    }
    return true;
}

/* 023221CC */
static s32 MethodCreate(Act_c *a) { return a->_create(); }

/* 023221D0 */
static bool MethodDelete(Act_c *a) { return a->_delete(); }

/* 023221D4 */
static bool MethodExecute(Act_c *a) { return a->_execute(); }

/* 023221D8 */
static bool MethodDraw(Act_c *a) { return a->_draw(); }

/* 0232236C */
static BOOL MethodIsDelete(void *a) { return TRUE; }

// HD: compiler-generated and template code of this translation unit.

/* 023222FC */
static void empty_virtual(void *a) {}

/* 02322374 */
u32 PrmAbstract(fopAc_ac_c *a, u32 width, u32 shift) {
    u32 p = fopAcM_GetParam(a);
    u32 mask = (width & 32) ? 0 : (1u << (width & 31));
    u32 val = (shift & 32) ? 0 : (p >> (shift & 31));
    return val & (mask - 1);
}

/* 02322270 */
static void watercheck_dt(void *obj, s32 flags) {
    if (obj) {
        auto *water = static_cast<barrel2_hd::WaterCheck *>(obj);
        water->mVtable = barrel2_hd::WaterVtable;
        water->mPolyVtable = barrel2_hd::WaterPolyVtable;
        water->mGroupVtable = barrel2_hd::WaterGroupVtable;
        Barrel_polyInfoDestroy(&water->mPolyInfo, 0);
        if (flags & 1) {
            operator_delete(obj);
        }
    }
}

/* 02322300 */
static void actor_dt(Act_c *a, s32 flags) {
    if (a) {
        Barrel_cylinderDestroy(&a->mCyl, 2);
        Barrel_statusDestroy(&a->mStts, 2);
        Barrel_actorDestroy(a, 0);
        if (flags & 1) {
            operator_delete(a);
        }
    }
}

/* 023221DC */
static void __sinit_d_a_obj_barrel2_cpp() {
    barrelInitialization.mWords[2] = 0;
    barrelInitialization.mWords[0] = 0;
    barrelInitialization.mWords[3] = 0;
    barrelInitialization.mWords[1] = 0;
    Barrel_destructorChain(&barrelChains.mData[1]);
    barrelInitialization.mMinAngle = -3.1415927410125732f;
    barrelInitialization.mMaxAngle = 3.1415927410125732f;
    Barrel_byteConstructor(&barrelInitialization.mByte);
    Barrel_destructorChain(&barrelChains.mData[2]);
    Barrel_otherByteConstructor(&barrelInitialization.mOtherByte);
    Barrel_destructorChain(&barrelChains.mData[3]);
}

/* 023222E8 */
static void trivial_dt(void *a, s32 flags) {
    if (a && (flags & 1)) {
        operator_delete(a);
    }
}

// clang-format off
/* WWHD glue (tools/verify/autoglue.py)
ext 0273AA24 void Barrel_assertion(const char* arg0, s32 arg1, const char* arg2)
ext 02459F6C void Barrel_raceItemForceGet(fopAc_ac_c* arg0)
ext 025D57E0 u32 Barrel_deleteActor(fopAc_ac_c* arg0)
ext 025D6924 f32 Barrel_playerDistance2(fopAc_ac_c* arg0, void* arg1)
ext 025D54C4 BOOL Barrel_searchById(u32 arg0, gptr<fopAc_ac_c>* arg1)
ext 0245A000 BOOL Barrel_checkOffsetPos(fopAc_ac_c* arg0)
ext 02008B4C void Barrel_polyInfoDestroy(void* arg0, s32 arg1)
ext 02515A70 void Barrel_cylinderDestroy(dCcD_Cyl* arg0, s32 arg1)
ext 02515860 void Barrel_statusDestroy(dCcD_Stts* arg0, s32 arg1)
ext 025D50BC void Barrel_actorDestroy(fopAc_ac_c* arg0, s32 arg1)
ext 028F026C void Barrel_destructorChain(void* arg0)
ext 028ED6F8 void Barrel_byteConstructor(void* arg0)
ext 028EAB2C void Barrel_otherByteConstructor(void* arg0)
ext 025D6768 void Barrel_setCullSphere(fopAc_ac_c* arg0, f32 arg1, f32 arg2, f32 arg3, f32 arg4)
ext 025D69AC f32 Barrel_actorDistanceXZ2(fopAc_ac_c* arg0, void* arg1)
ext 025D6CE8 BOOL Barrel_cullingCheck(fopAc_ac_c* arg0)
ext 0254457C BOOL Barrel_eventEndCheck(void* arg0, const char* arg1)
ext 02543F10 s32 Barrel_eventIndex(void* arg0, const char* arg1, u8 arg2)
ext 02544044 void* Barrel_eventData(void* arg0, s32 arg1)
ext 025D77DC void Barrel_orderOtherEvent(fopAc_ac_c* arg0, const char* arg1, u16 arg2, u16 arg3)
ext 02459FA0 void Barrel_endOffsetPos(fopAc_ac_c* arg0, f32 arg1, cXyz* arg2, f32 arg3, f32 arg4, csXyz* arg5)
ext 0245A028 void Barrel_setItemMatrix(fopAc_ac_c* arg0, cXyz* arg1)
ext 02459F90 void Barrel_startOffsetPos(fopAc_ac_c* arg0)
ext 025D5218 void* Barrel_searchActor(void* arg0, be<u32>* arg1)
ext 028E90D4 void Barrel_copyMatrix(void* arg0, void* arg1)
ext 028E8F44 f32 Barrel_innerProduct(cXyz* arg0, cXyz* arg1)
ext 02520540 s32 Barrel_reverb(s8 arg0)
ext 025E1A40 void Barrel_sound(u32 arg0, void* arg1, s32 arg2, s32 arg3)
ext 028F4384 f32 Barrel_sqrt(f32 arg0)
ext 0273AD10 void* Barrel_allocate(u32 arg0)
ext 025E80D0 void* Barrel_brkConstruct(void* arg0)
ext 025E8154 s32 Barrel_brkInit(void* arg0, J3DModelData* arg1, void* arg2, s32 arg3, s32 arg4, f32 arg5, s32 arg6, s32 arg7, s32 arg8, s32 arg9)
ext 0246B6A4 BOOL Barrel_seaCheckArea(f32 arg0, f32 arg1)
ext 0246BA0C f32 Barrel_seaCalcWave(f32 arg0, f32 arg1)
ext 0201B080 void Barrel_outProduct(cXyz* arg0, cXyz* arg1, cXyz* arg2)
ext 0201B3C0 void Barrel_normalizeZP(cXyz* arg0, cXyz* arg1)
ext 024F22DC void Barrel_waterCheckConstruct(void* arg0)
ext 024EF7C0 BOOL Barrel_specialGroupCheck(void* arg0, void* arg1)
ext 0201B31C void Barrel_normalize(cXyz* arg0, cXyz* arg1)
ext 025A847C void* Barrel_particleSet(void* arg0, s32 arg1, u32 arg2, cXyz* arg3, csXyz* arg4, cXyz* arg5, u8 arg6, void* arg7, s32 arg8, void* arg9, void* arg10, void* arg11)
ext 025A866C void Barrel_particleBombSmoke(void* arg0, u32 arg1, cXyz* arg2, csXyz* arg3, cXyz* arg4, u8 arg5)
ext 025DAE64 void Barrel_createWaterPillar(cXyz* arg0, f32 arg1, f32 arg2, s32 arg3)
ext 0201AE48 void Barrel_scaleVector(cXyz* arg0, cXyz* arg1, f32 arg2)
ext 023123D8 void Barrel_quaternionRotBaseY(void* arg0, cXyz* arg1)
ext 025F25CC void Barrel_quaternionMatrix(void* arg0)
ext 025F1B48 void Barrel_zxyRotation(Mtx34* arg0, s16 arg1, s16 arg2, s16 arg3)
ext 025F1C5C void Barrel_zRotation(Mtx34* arg0, s16 arg1)
ext 028E9044 void Barrel_matrixVectorSR(Mtx34* arg0, cXyz* arg1, cXyz* arg2)
ext 020182E0 void Barrel_setCylinderCenter(void* arg0, cXyz* arg1)
ext 0200E240 void Barrel_registerCollision(void* arg0, dCcD_Cyl* arg1)
ext 025E742C BOOL Barrel_brkPlay(mDoExt_brkAnm* arg0)
ext 0200BD2C void Barrel_statusConstruct(void* arg0)
ext 02515DA0 void Barrel_gameStatusConstruct(void* arg0)
ext 02515FB8 void Barrel_cylinderConstruct(dCcD_Cyl* arg0)
ext 02018590 void Barrel_cylinderShapeConstruct(void* arg0)
ext 025D63E8 BOOL Barrel_entrySolidHeap(fopAc_ac_c* arg0, void* arg1, u32 arg2)
ext 02516518 void Barrel_setCylinder(dCcD_Cyl* arg0, void* arg1)
ext 020184DC void Barrel_setCylinderRadius(void* arg0, f32 arg1)
ext 02018428 void Barrel_setCylinderHeight(void* arg0, f32 arg1)
ext 0201A478 void Barrel_angleConstruct(csXyz* arg0, s16 arg1, s16 arg2, s16 arg3)
ext 025D86E0 u32 Barrel_createRaceItem(cXyz* arg0, s32 arg1, s32 arg2, s8 arg3, csXyz* arg4, cXyz* arg5, BOOL arg6)
ext 025D5A20 u32 Barrel_createChild(s32 arg0, u32 arg1, u32 arg2, cXyz* arg3, s8 arg4, csXyz* arg5, cXyz* arg6, s32 arg7, void* arg8)
data barrelAttributes 10025E30
data barrelTmpItem 101C7F30
data barrelModes 10025AF0
data barrelSinCos 104A44F8
data barrelZeroAngles 101FFB14
data barrelWater 10469228
data barrelWaterGuard 10469278
data barrelStackMatrix 1048D0CC
data barrelExitSpeed 10025AE8
data barrelMineExitSpeed 10025AEC
data barrelItemDistanceMax 10025928
str 10025DE0 "Ktaru_00" in 0231F350
str 1002592C "d_a_obj_barrel2.cpp" in 0231F350
str 10025940 "(M_type >= 0) && (M_type < Type_Max)" in 0231F350
str 10025B7C "d_a_obj_barrel2.cpp" in 02320AD4
str 10025B90 "(M_type >= 0) && (M_type < Type_Max)" in 02320AD4
str 10025C98 "d_a_obj_barrel2.cpp" in 02321564
str 10025CAC "(M_type >= 0) && (M_type < Type_Max)" in 02321564
str 100258E4 "d_a_obj_barrel2.cpp" in 0231EEF4
str 100258F8 "(M_type >= 0) && (M_type < Type_Max)" in 0231EEF4
str 100259EC "d_a_obj_barrel2.cpp" in 0231FE60
str 10025A00 "(M_type >= 0) && (M_type < Type_Max)" in 0231FE60
str 10025AAC "d_a_obj_barrel2.cpp" in 02320648
str 10025AC0 "(M_type >= 0) && (M_type < Type_Max)" in 02320648
str 10025DE0 "Ktaru_00" in 0231E670
str 100257E8 "d_a_obj_barrel2.cpp" in 0231E670
str 100257FC "mdl_data != 0" in 0231E670
str 1002580C "brk_data != 0" in 0231E670
str 1002581C "(M_type >= 0) && (M_type < Type_Max)" in 0231E670
str 10025BF8 "d_a_obj_barrel2.cpp" in 0232115C
str 10025C0C "(M_type >= 0) && (M_type < Type_Max)" in 0232115C
str 10025D8C "d_a_obj_barrel2.cpp" in 02321D54
str 10025DA0 "(M_type >= 0) && (M_type < Type_Max)" in 02321D54
str 10025A70 "d_a_obj_barrel2.cpp" in 02320418
str 10025A84 "(M_type >= 0) && (M_type < Type_Max)" in 02320418
str 1002584C "d_a_obj_barrel2.cpp" in 0231E84C
str 10025860 "(M_type >= 0) && (M_type < Type_Max)" in 0231E84C
str 10025C34 "d_a_obj_barrel2.cpp" in 023213F8
str 10025C48 "(M_type >= 0) && (M_type < Type_Max)" in 023213F8
str 10025A30 "d_a_obj_barrel2.cpp" in 02320294
str 10025A44 "(M_type >= 0) && (M_type < Type_Max)" in 02320294
str 10025D50 "d_a_obj_barrel2.cpp" in 02321B74
str 10025D64 "(M_type >= 0) && (M_type < Type_Max)" in 02321B74
str 10025D14 "d_a_obj_barrel2.cpp" in 023219D8
str 10025D28 "(M_type >= 0) && (M_type < Type_Max)" in 023219D8
str 10025CD8 "d_a_obj_barrel2.cpp" in 0232183C
str 10025CEC "(M_type >= 0) && (M_type < Type_Max)" in 0232183C
str 10025B38 "d_a_obj_barrel2.cpp" in 023209E4
str 10025B4C "(M_type >= 0) && (M_type < Type_Max)" in 023209E4
str 100259A4 "d_a_obj_barrel2.cpp" in 0231FD84
str 100259B8 "(M_type >= 0) && (M_type < Type_Max)" in 0231FD84
str 100258A0 "d_a_obj_barrel2.cpp" in 0231ED94
str 100258B4 "(M_type >= 0) && (M_type < Type_Max)" in 0231ED94
str 10025BBC "d_a_obj_barrel2.cpp" in 02320EDC
str 10025BD0 "(M_type >= 0) && (M_type < Type_Max)" in 02320EDC
str 10025968 "d_a_obj_barrel2.cpp" in 0231FBA8
str 1002597C "(M_type >= 0) && (M_type < Type_Max)" in 0231FBA8
str 10025DE0 "Ktaru_00" in 0231FB78
data barrelInitialization 1046920C
data barrelChains 101C7F00
str 10025DD4 "barrel2_brk"
data barrelCylinderSource 10025DEC
*/
// clang-format on
