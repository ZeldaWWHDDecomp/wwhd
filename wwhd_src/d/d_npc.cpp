// NPC helpers.
// Free implementation names retain tww names with an _impl suffix to avoid shared inline call
// bindings.
#include "d/d_npc_readable.h"

static f32 npc_zero;
static u32 npc_headerD0;
static u32 npc_headerZ0;
static u32 npc_headerD1;
static u32 npc_headerZ1;
static f32 npc_piFirst;
static f32 npc_piSecond;
static f32 npc_piFirstSource;
static f32 npc_piSecondSource;
static u8 npc_headerByte;
static u8 npc_headerFlag;
static npc_readable::GlobalObjectRecord npc_headerRecord1;
static npc_readable::GlobalObjectRecord npc_headerRecord2;
static npc_readable::GlobalObjectRecord npc_headerRecord3;

static u32 npc_vtable;
static u32 npc_objAcchCheckVtable;
static u32 npc_polyPassVtable;
static u32 npc_grpPassVtable;
static u32 npc_sttsVtable;
static u32 npc_gsttsVtable;
static u32 npc_shapeAttrVtable;
static u32 npc_aabVtable;
static u32 npc_cylVtable;
static u32 npc_hitInfoVtable;
static u32 npc_cylShapeVtable;
static f32 npc_animEnd;
static f32 npc_maxAttnAngle;
static f32 npc_degreesToAngle;
static f32 npc_rodentMoveCorrection;
static u32 npc_safeStringVtable;
static f32 npc_turnTolerance;
static f32 npc_moveFactor;
static f32 npc_moveTolerance;
static f32 npc_defaultMoveSpeed;
static f64 npc_signedFloatBias;
static f32 npc_pathPlaneScale;
static f32 npc_infinity;
static u32 npc_hio_vtable;
namespace {
inline void* lookup(NpcResourceLookup getRes, const char* arc, s32 id) {
    gabi::Local<NpcString> key;
    key->data = gabi::ea(arc);
    key->vt = gabi::ea(&npc_safeStringVtable);
    return getRes(dComIfG_resControl(), key.get(), id);
}
inline void setMorph(mDoExt_McaMorf* self, void* anim, u32 loop, void* sound, f32 blend,
                     f32 speed) {
    npc_setMorph(self, anim, loop, sound, blend, speed, npc_zero, npc_animEnd);
}
} // namespace
/* 0259D24C */
static BOOL dNpc_setAnmIDRes_impl(mDoExt_McaMorf* self, u32 loop, f32 blend, f32 speed, s32 anim,
                                  s32 sound, const char* arc) {
    gabi::AutoFrame frame{0x0259D24C};
    if (!self)
        return FALSE;
    void* soundRes = nullptr;
    if (sound >= 0)
        soundRes = lookup(npc_getObjectIDRes, arc, sound);
    void* animRes = lookup(npc_getObjectIDRes, arc, anim);
    setMorph(self, animRes, loop, soundRes, blend, speed);
    return TRUE;
}
/* 0259D344 */
static BOOL dNpc_setAnmFNDirect_impl(mDoExt_McaMorf* self, u32 loop, f32 blend, f32 speed,
                                     const char* anim, const char* sound, const char* arc) {
    gabi::AutoFrame frame{0x0259D344};
    if (!self || !anim || !arc)
        return FALSE;
    gabi::Local<NpcString> a, b;
    a->data = gabi::ea(arc);
    a->vt = gabi::ea(&npc_safeStringVtable);
    b->data = gabi::ea(anim);
    b->vt = gabi::ea(&npc_safeStringVtable);
    void* animRes = npc_getObjectNamedRes(dComIfG_resControl(), a.get(), b.get());
    void* soundRes = nullptr;
    if (sound) {
        gabi::Local<NpcString> c, d;
        c->data = gabi::ea(arc);
        c->vt = gabi::ea(&npc_safeStringVtable);
        d->data = gabi::ea(sound);
        d->vt = gabi::ea(&npc_safeStringVtable);
        soundRes = npc_getObjectNamedRes(dComIfG_resControl(), c.get(), d.get());
    }
    setMorph(self, animRes, loop, soundRes, blend, speed);
    return TRUE;
}
/* 0259D454 */
static BOOL dNpc_setAnm_impl(mDoExt_McaMorf* self, u32 loop, f32 blend, f32 speed, s32 anim,
                             s32 sound, const char* arc) {
    gabi::AutoFrame frame{0x0259D454};
    if (!self)
        return FALSE;
    void* soundRes = nullptr;
    if (sound >= 0)
        soundRes = lookup(npc_getObjectRes, arc, sound);
    void* animRes = lookup(npc_getObjectRes, arc, anim);
    setMorph(self, animRes, loop, soundRes, blend, speed);
    return TRUE;
}
/* 0259D54C */
static u32 dNpc_playerEyePos_impl(cXyz* result, f32 offset) {
    gabi::AutoFrame frame{0x0259D54C};
    auto* player = static_cast<daPy_py_c*>(dComIfGp_getLinkPlayer());
    gabi::Local<cXyz> input, output;
    input->x = (f32)player->mHeadTopPos.x;
    input->y = (f32)player->mHeadTopPos.y;
    input->z = (f32)player->mHeadTopPos.z;
    MtxTrans(input->x, input->y, input->z, 0);
    input->x = npc_zero;
    input->y = offset;
    input->z = npc_zero;
    MtxPosition(input.get(), output.get());
    output->x = (f32)player->current.pos.x;
    output->z = (f32)player->current.pos.z;
    if (!result)
        result = npc_allocatePoint(12u);
    if (result) {
        result->x = (f32)output->x;
        result->y = (f32)output->y;
        result->z = (f32)output->z;
    }
    return gabi::ea(result);
}
/* 0259D624 */
static void dNpc_calc_DisXZ_AngY_impl(cXyz* a, cXyz* b, be<f32>* distance, be<s16>* angle) {
    gabi::AutoFrame frame{0x0259D624};
    f32 dx = gabi::fsubs_ppc(b->x, a->x);
    f32 dz = gabi::fsubs_ppc(b->z, a->z);
    if (distance)
        *distance = npc_sqrt(gabi::fmadds(dx, dx, gabi::fmuls_ppc(dz, dz)));
    if (angle)
        *angle = npc_atan2s(dx, dz);
}
/* 0259D6C8 */
static BOOL dNpc_chkArasoi_impl() {
    gabi::AutoFrame frame{0x0259D6C8};
    if (!dComIfGs_isEventBit(0x1220))
        return FALSE;
    return !dComIfGs_isEventBit(0x1808);
}
/* 0259D734 */
static BOOL dNpc_chkLetterPassed_impl() {
    gabi::AutoFrame frame{0x0259D734};
    if (!dComIfGs_isGetItemReserve(12))
        return FALSE;
    return !dComIfGs_checkReserveItem(dItemNo_FATHER_LETTER_e);
}
/* 0259D79C */
static BOOL dNpc_setAnm_2_impl(mDoExt_McaMorf* self, u32 loop, f32 blend, f32 speed, s32 anim,
                               s32 sound, const char* arc) {
    gabi::AutoFrame frame{0x0259D79C};
    // HD: this variant does not guard the morph pointer and looks up animation first.
    void* animRes;
    void* soundRes = nullptr;
    if (sound >= 0) {
        animRes = lookup(npc_getObjectRes, arc, anim);
        soundRes = lookup(npc_getObjectRes, arc, sound);
    } else
        animRes = lookup(npc_getObjectRes, arc, anim);
    setMorph(self, animRes, loop, soundRes, blend, speed);
    return TRUE;
}
/* 0259D8BC */
static BOOL dNpc_chkAttn_impl(fopAc_ac_c* actor, cXyz* target, u32 extended, f32 range,
                              f32 extension, f32 angle) {
    gabi::AutoFrame frame{0x0259D8BC};
    s16 facing = (s16)npc_targetAngleY(&actor->current.pos, target);
    gabi::Local<cXyz> delta, flat;
    npc_vectorSubtract(&actor->current.pos, delta.get(), target);
    flat->x = delta->x;
    flat->y = npc_zero;
    flat->z = delta->z;
    f32 length = npc_vectorSquareMag(flat.get());
    f32 dist = npc_sqrt(length);
    f32 limit = npc_maxAttnAngle;
    f32 diff = gabi::fsubs_ppc(angle, limit);
    angle = diff >= 0 ? limit : angle;
    f32 zero = npc_zero;
    angle = angle >= 0 ? angle : zero;
    s16 yaw = (s16)((u16)facing - (u16)(s16)actor->current.angle.y);
    if (extended)
        range = gabi::fadds_ppc(range, extension);
    if (dist > range)
        return FALSE;
    s16 threshold = (s16)gabi::ftoi(gabi::fmuls_ppc(angle, npc_degreesToAngle));
    s32 absolute = yaw < 0 ? -(s32)yaw : (s32)yaw;
    return absolute <= threshold;
}
/* 0259DA18 */
static u32 dNpc_HIO_c__ct(dNpc_HIO_c* self) {
    gabi::AutoFrame frame{0x0259DA18};
    if (!self)
        self = reinterpret_cast<dNpc_HIO_c*>(npc_allocatePoint(sizeof(dNpc_HIO_c)));
    if (!self)
        return 0;
    f32 zero = npc_zero;
    self->mMaxHeadY = 0;
    self->mAttnYOffset = zero;
    self->__vtbl = gabi::ea(&npc_hio_vtable);
    self->m04 = zero;
    self->mMinHeadY = 0;
    self->mMaxAttnAngleY = 0;
    self->mMaxBackboneY = 0;
    self->mMinHeadX = 0;
    self->mMaxTurnStep = 0;
    self->mMaxBackboneX = 0;
    self->mMaxHeadTurnVel = 0;
    self->mMinBackboneX = 0;
    self->mMinBackboneY = 0;
    self->mMaxHeadX = 0;
    self->m22 = 0;
    self->mMaxAttnDistXZ = zero;
    return gabi::ea(self);
}
/* 0259DAA0 */
static u32 dNpc_JntCtrl_c__ct(npc_readable::dNpc_JntCtrl_c* self) {
    gabi::AutoFrame frame{0x0259DAA0};
    if (!self)
        self = reinterpret_cast<npc_readable::dNpc_JntCtrl_c*>(
            npc_allocatePoint(sizeof(npc_readable::dNpc_JntCtrl_c)));
    if (!self)
        return 0;
    npc_clearMemory(self->mAngles, 8u);
    self->mbTrn = 0;
    self->mbBackBoneLock = 0;
    self->mBackboneJntNum = 0;
    self->mHeadJntNum = 0;
    self->mbHeadLock = 0;
    npc_clearMemory(self->mMinAngles, 8u);
    npc_clearMemory(self->mMaxAngles, 8u);
    npc_clearMemory(self->mMaxTurnStep, 8u);
    // HD: GHS clears the trailing four bytes of this padding as one word.
    *reinterpret_cast<be<u32>*>(&self->pad_0x26[2]) = 0;
    npc_clearMemory(&self->field_0x2C, 8u);
    self->mbHeadLock = 0;
    self->mbBackBoneLock = 0;
    return gabi::ea(self);
}
/* 0259DB48 */
void npc_readable::dNpc_JntCtrl_c::limitter(be<s16>* angle, s32 upper, s32 lower) {
    gabi::AutoFrame frame{0x0259DB48};
    // HD: compare against the full-width bounds before storing the halfword.
    s32 value = *angle;
    if (value > upper) {
        value = upper;
        *angle = value;
    }
    if (value < lower)
        *angle = lower;
}
/* 0259DB6C */
BOOL npc_readable::dNpc_JntCtrl_c::move(s32 requested, u32 axis) {
    gabi::AutoFrame frame{0x0259DB6C};
    gabi::Local<be<s16>[2]> targets;
    for (u32 i = 0; i < 2; i++) {
        bool locked = (i ? mbBackBoneLock : mbHeadLock) != 0;
        gabi::Local<be<s16>> target;
        *target = locked ? 0 : (s16)requested;
        limitter(target.get(), mMaxAngles[i][axis], mMinAngles[i][axis]);
        s16 selected = *target;
        requested = (s16)((u32)requested - (u32)(s32)selected);
        (*targets)[i] = selected;
        gabi::Local<be<s32>> moving;
        *moving = mAngles[i][axis];
        npc_addCalcAngleL(moving.get(), selected, 4, mMaxTurnStep[i][axis], 1);
        mAngles[i][axis] = (s32)*moving;
    }
    u32 reached = 0;
    for (u32 i = 0; i < 2; i++) {
        s32 difference = npc_angleDistance((s16)(*targets)[i], mAngles[i][axis]);
        if (difference <= (s32)mMaxTurnStep[i][axis] / 2)
            reached++;
    }
    return reached == 2;
}
/* 0259DCEC */
BOOL npc_readable::dNpc_JntCtrl_c::angCalcS(be<s16>* angle, s32 goal, s32 minimum, s32 maximum) {
    gabi::AutoFrame frame{0x0259DCEC};
    s16 oldDelta = (s16)((u32)(s32)*angle - (u32)goal);
    gabi::Local<be<s16>> delta;
    *delta = oldDelta;
    npc_addCalcAngle(delta.get(), 0, minimum, maximum, 0x60);
    s32 change = (s16)*delta;
    s32 absolute = change < 0 ? -change : change;
    if (absolute > minimum) {
        s16 value = (s16)((u32)(s32)*angle + (u32)(change - (s32)oldDelta));
        *angle = value;
        // HD: compare the stored halfword against the full-width goal.
        return (s32)value == goal;
    }
    *angle = goal;
    return TRUE;
}
/* 0259DDA0 */
BOOL npc_readable::dNpc_JntCtrl_c::follow(be<s16>* angle, s32 goal, s32 maxStep, u32 axis) {
    gabi::AutoFrame frame{0x0259DDA0};
    s16 before = *angle;
    angCalcS(angle, goal, 4, maxStep);
    s16 after = *angle;
    s16 change = (s16)((u32)(s32)after - (u32)(s32)before);
    s16 remaining = (s16)((u32)goal - (u32)(s32)after);
    s16 accumulated = 0, running = 0;
    for (u32 i = 0; i < 2; i++) {
        u32 joint = 1 - i;
        s16 current = mAngles[joint][axis];
        running = (s16)((u32)(s32)running + (u32)(s32)current);
        if (change < 0 ? running < remaining : (change > 0 && running > remaining)) {
            mAngles[joint][axis] = (u32)(s32)remaining - (u32)(s32)accumulated;
            limitter(&mAngles[joint][axis], mMaxAngles[joint][axis], mMinAngles[joint][axis]);
            current = mAngles[joint][axis];
        }
        accumulated = (s16)((u32)(s32)accumulated + (u32)(s32)current);
    }
    return goal != (s32)*angle;
}
/* 0259DED0 */
void npc_readable::dNpc_JntCtrl_c::lookAtTarget(be<s16>* angle, cXyz* target, cXyz* eye, s32 yaw,
                                                s32 velocity, u32 headOnly) {
    gabi::AutoFrame frame{0x0259DED0};
    s32 pitch = 0;
    if (target) {
        f32 dz = gabi::fsubs_ppc(target->z, eye->z);
        f32 dx = gabi::fsubs_ppc(target->x, eye->x);
        f32 dy = gabi::fsubs_ppc(target->y, eye->y);
        f32 range = npc_sqrt(gabi::fmadds(dx, dx, gabi::fmuls_ppc(dz, dz)));
        yaw = npc_atan2s(dx, dz);
        pitch = npc_atan2s(dy, range);
    }
    s16 relative = (s16)((u32)yaw - (u32)(s32)*angle);
    // HD: the inverted bound test and these component sums are intentional.
    s32 lower = (s32)mMaxAngles[1][1] + (s32)mMaxAngles[0][1];
    s32 upper = (s32)mMinAngles[1][1] + (s32)mMinAngles[0][1];
    u32 moved = move(relative, 1);
    bool shouldFollow = mbTrn != 0;
    if (!shouldFollow)
        shouldFollow = !(relative < lower && relative > upper) && moved != 0;
    if (shouldFollow && !headOnly)
        mbTrn = follow(angle, yaw, velocity, 1);
    move(pitch, 0);
}
/* 0259E08C */
void npc_readable::dNpc_JntCtrl_c::setParam(s32 a, s32 b, s32 c, s32 d, s32 e, s32 f, s32 g) {
    gabi::AutoFrame frame{0x0259E08C};
    // HD: the final two GHS arguments are halfwords in caller stack slots.
    s32 h = gabi::load<s16>(gabi::cpu->r[1] + 10);
    s32 i = gabi::load<s16>(gabi::cpu->r[1] + 14);
    mMinAngles[0][0] = g;
    mMaxTurnStep[1][1] = i;
    mMinAngles[1][1] = d;
    mMaxTurnStep[1][0] = i;
    mMaxAngles[0][0] = e;
    mMaxTurnStep[0][0] = i;
    mMaxAngles[1][1] = b;
    mMaxTurnStep[0][1] = i;
    mMinAngles[0][1] = h;
    mMaxAngles[1][0] = a;
    mMinAngles[1][0] = c;
    mMaxAngles[0][1] = f;
}
/* 0259E0C8 */
s16 npc_readable::dNpc_JntCtrl_c::chkLim(s32 value, u32 joint, u32 axis) {
    gabi::AutoFrame frame{0x0259E0C8};
    s32 upper = mMaxAngles[joint][axis];
    if (upper < value)
        value = upper;
    // HD: narrow between the upper and lower clamps, rather than at function entry.
    value = (s16)value;
    s32 lower = mMinAngles[joint][axis];
    if (lower > value)
        value = lower;
    return (s16)value;
}
/* 0259E104 */
void npc_readable::dNpc_JntCtrl_c::turn_fromBackbone2Head(s32 requested, be<s16>* head,
                                                          be<s16>* back, u32 direction) {
    gabi::AutoFrame frame{0x0259E104};
    *back = 0;
    if (!mbBackBoneLock) {
        // Keep the callee's full return register for the subsequent sign test.
        s32 value = npc_jointLimit(this, requested, 1, 1);
        *back = value;
        // HD: both previous-turn directions participate in the reset test.
        if (field_0x32 && ((direction && value < 0) || (!direction && value > 0)))
            *back = 0;
    }
    *head = 0;
    if (!mbHeadLock) {
        s16 remainder = (s16)((u32)requested - (u32)(s32)*back);
        *head = remainder;
        *head = npc_jointLimit(this, remainder, 0, 1);
    }
}
/* 0259E1DC */
void npc_readable::dNpc_JntCtrl_c::turn_fromHead2Backbone(s32 requested, be<s16>* head,
                                                          be<s16>* back) {
    gabi::AutoFrame frame{0x0259E1DC};
    *head = 0;
    if (!mbHeadLock) {
        s16 value = (s16)((u32)requested - (u32)(s32)field_0x32);
        *head = value;
        *head = npc_jointLimit(this, value, 0, 1);
    }
    *back = 0;
    if (!mbBackBoneLock) {
        s16 value = (s16)((u32)requested - (u32)(s32)*head);
        *back = value;
        *back = npc_jointLimit(this, value, 1, 1);
    }
}
/* 0259E28C */
s16 npc_readable::dNpc_JntCtrl_c::follow_current(be<s16>* angle, s32 amount) {
    gabi::AutoFrame frame{0x0259E28C};
    s16 value = *angle;
    s16 next = (s16)((u32)(s32)value - (u32)amount);
    if ((value > 0 && next < 0) || (value < 0 && next > 0)) {
        *angle = 0;
        return (s16) - (s32)next;
    }
    *angle = next;
    return 0;
}
/* 0259E6D0 */
BOOL npc_readable::dNpc_PathRun_c::setInf(s32 id, s32 room, u32 reverse) {
    gabi::AutoFrame frame{0x0259E6D0};
    // HD: direction is stored before clearing the path, and arguments remain full width.
    mbDir = reverse;
    mPath = 0;
    if (id == 255)
        return FALSE;
    mPath = gabi::at<dPath>(npc_pathGetRoom(id, room));
    mIdx = 0;
    return TRUE;
}

/* 0259E730 */
BOOL npc_readable::dNpc_PathRun_c::setInfDrct(u32 path) {
    gabi::AutoFrame frame{0x0259E730};
    mPath = gabi::at<dPath>(path);
    mIdx = 0;
    return TRUE;
}

/* 0259E744 */
u32 npc_readable::dNpc_PathRun_c::nextPath() {
    gabi::AutoFrame frame{0x0259E744};
    // HD: this entry point supplies only the path, without a room argument.
    u32 path = gabi::ea(mPath.get());
    return path ? npc_pathGetNext(path) : 0;
}

/* 0259E778 */
void npc_readable::dNpc_PathRun_c::getPoint(cXyz* output, u32 index) {
    gabi::AutoFrame frame{0x0259E778};
    f32 x = npc_zero, y = x, z = x;
    auto* path = reinterpret_cast<path_layout::dPath*>(mPath.get());
    if (path && index < (u16)path->m_num) {
        auto* point = &path->m_points[index];
        x = point->m_position.x;
        y = point->m_position.y;
        z = point->m_position.z;
    }
    // HD: a missing hidden result buffer is allocated; the scalar store order is y,x,z.
    if (!output)
        output = npc_allocatePoint(12u);
    if (output) {
        output->y = y;
        output->x = x;
        output->z = z;
    }
}

/* 0259EB1C */
BOOL npc_readable::dNpc_PathRun_c::incIdx() {
    gabi::AutoFrame frame{0x0259EB1C};
    if (!mPath.get())
        return TRUE;
    u32 index = ((u32)mIdx + 1u) & 0xFFu;
    mIdx = index;
    u32 count = reinterpret_cast<path_layout::dPath*>(mPath.get())->m_num;
    if (index < count)
        return TRUE;
    mIdx = count - 1;
    return FALSE;
}

/* 0259EB60 */
BOOL npc_readable::dNpc_PathRun_c::incIdxLoop() {
    gabi::AutoFrame frame{0x0259EB60};
    if (!mPath.get())
        return TRUE;
    u32 index = ((u32)mIdx + 1u) & 0xFFu;
    mIdx = index;
    u32 count = reinterpret_cast<path_layout::dPath*>(mPath.get())->m_num;
    if (index < count)
        return TRUE;
    mIdx = 0;
    return FALSE;
}

/* 0259EBA4 */
BOOL npc_readable::dNpc_PathRun_c::incIdxAuto() {
    gabi::AutoFrame frame{0x0259EBA4};
    auto* path = reinterpret_cast<path_layout::dPath*>(mPath.get());
    if (!path)
        return TRUE;
    u32 index = ((u32)mIdx + 1u) & 0xFFu;
    bool isLoop = ((u8)path->m_closed & 1) != 0;
    mIdx = index;
    u32 count = reinterpret_cast<path_layout::dPath*>(mPath.get())->m_num;
    if (index < count)
        return TRUE;
    mIdx = isLoop ? 0 : count - 1;
    return isLoop ? TRUE : FALSE;
}

/* 0259EC0C */
BOOL npc_readable::dNpc_PathRun_c::decIdx() {
    gabi::AutoFrame frame{0x0259EC0C};
    if (!mPath.get())
        return TRUE;
    u32 index = ((u32)mIdx - 1u) & 0xFFu;
    mIdx = index;
    u32 count = reinterpret_cast<path_layout::dPath*>(mPath.get())->m_num;
    if (index < count)
        return TRUE;
    mIdx = 0;
    return FALSE;
}

/* 0259EC50 */
BOOL npc_readable::dNpc_PathRun_c::decIdxLoop() {
    gabi::AutoFrame frame{0x0259EC50};
    if (!mPath.get())
        return TRUE;
    u32 index = ((u32)mIdx - 1u) & 0xFFu;
    mIdx = index;
    u32 count = reinterpret_cast<path_layout::dPath*>(mPath.get())->m_num;
    if (index < count)
        return TRUE;
    mIdx = count - 1;
    return FALSE;
}

/* 0259EC94 */
BOOL npc_readable::dNpc_PathRun_c::decIdxAuto() {
    gabi::AutoFrame frame{0x0259EC94};
    auto* path = reinterpret_cast<path_layout::dPath*>(mPath.get());
    if (!path)
        return TRUE;
    u32 index = ((u32)mIdx - 1u) & 0xFFu;
    bool isLoop = ((u8)path->m_closed & 1) != 0;
    mIdx = index;
    u32 count = reinterpret_cast<path_layout::dPath*>(mPath.get())->m_num;
    if (index < count)
        return TRUE;
    mIdx = isLoop ? count - 1 : 0;
    return isLoop ? TRUE : FALSE;
}

/* 0259ECFC */
BOOL npc_readable::dNpc_PathRun_c::nextIdx() {
    gabi::AutoFrame frame{0x0259ECFC};
    bool forward = mbDir != 0;
    u32 result = forward ? incIdx() : decIdx();
    if (!result) {
        if (forward)
            decIdx();
        else
            incIdx();
    }
    return result;
}

/* 0259ED58 */
BOOL npc_readable::dNpc_PathRun_c::nextIdxAuto() {
    gabi::AutoFrame frame{0x0259ED58};
    bool forward = mbDir != 0;
    u32 result = forward ? incIdxAuto() : decIdxAuto();
    if (!result) {
        if (forward)
            decIdx();
        else
            incIdx();
    }
    return result;
}

/* 0259EDB8 */
u32 npc_readable::dNpc_PathRun_c::maxPoint() {
    gabi::AutoFrame frame{0x0259EDB8};
    auto* path = reinterpret_cast<path_layout::dPath*>(mPath.get());
    // HD: the count is truncated to its low byte; a missing path yields 255.
    return path ? (u8)(u16)path->m_num : 255;
}
/* 0259EDD0 */
s32 npc_readable::dNpc_PathRun_c::absIdx(s32 first, s32 second) {
    gabi::AutoFrame frame{0x0259EDD0};
    if (!mPath.get())
        return 0;
    // HD: subtraction wraps at 32 bits before the signed comparison.
    s32 diff1 = (s32)((u32)first - (u32)second);
    s32 diff2 = (s32)((u32)second - (u32)first);
    if (diff1 < 0)
        diff1 = (s32)((u32)diff1 + maxPoint());
    if (diff2 < 0)
        diff2 = (s32)((u32)diff2 + maxPoint());
    return diff2 < diff1 ? diff2 : diff1;
}

/* 0259EE4C */
u32 npc_readable::dNpc_PathRun_c::pointArg(u32 index) {
    gabi::AutoFrame frame{0x0259EE4C};
    auto* path = reinterpret_cast<path_layout::dPath*>(mPath.get());
    // HD: compare the full-width index against the truncated path count.
    if (!path || index >= (u8)(u16)path->m_num)
        return 0;
    return path->m_points[index].mArg3;
}
static f32 signedFloat(s32 value) {
    u64 bits = (u64(0x43300000) << 32) | ((u32)value ^ 0x80000000u);
    f64 converted;
    __builtin_memcpy(&converted, &bits, 8);
    return (f32)(converted - npc_signedFloatBias);
}
/* 0259F36C */
void npc_readable::dNpc_HeadAnm_c::swing_vertical_init(s32 count, s32 step, s32 amplitude,
                                                       u32 resume) {
    gabi::AutoFrame frame{0x0259F36C};
    // The horizone initializer also tests the vertical procedure, as in tww.
    if (resume && mProcIndex == -1 && !mProcDelta && mProcFunc == SWING_VERTICAL)
        return;
    mProcFunc = SWING_VERTICAL;
    field_0x1C = 0;
    field_0x20 = count;
    field_0x1E = step;
    mProcDelta = 0;
    mProcIndex = -1;
    field_0x14 = signedFloat(amplitude);
}
/* 0259F448 */
void npc_readable::dNpc_HeadAnm_c::swing_vertical() {
    gabi::AutoFrame frame{0x0259F448};
    s16 before = field_0x1C;
    u16 phase = (u16)((s32)before + (s32)field_0x1E);
    f32 amplitude = field_0x14;
    field_0x1C = phase;
    f32 wave = cM_ssin(phase);
    s16 target = (s16)gabi::ftoi(gabi::fmuls_ppc(amplitude, wave));
    npc_addCalcAngle(&field_0x00.x, target, 4, 0x1000, 0x100);
    npc_addCalcAngle(&field_0x00.y, 0, 4, 0x1000, 0x100);
    if (before < 0 && field_0x1C >= 0) {
        s16 count = (s16)((s16)field_0x20 - 1);
        field_0x20 = count;
        if (count <= 0) {
            mProcFunc = 0;
            mProcDelta = 0;
            mProcIndex = 0;
        }
    }
}
/* 0259F514 */
void npc_readable::dNpc_HeadAnm_c::swing_horizone_init(s32 count, s32 step, s32 amplitude,
                                                       u32 resume) {
    gabi::AutoFrame frame{0x0259F514};
    // The horizone initializer also tests the vertical procedure, as in tww.
    if (resume && mProcIndex == -1 && !mProcDelta && mProcFunc == SWING_VERTICAL)
        return;
    field_0x1C = 0;
    field_0x20 = count;
    mProcFunc = SWING_HORIZONE;
    field_0x1E = step;
    mProcDelta = 0;
    mProcIndex = -1;
    field_0x18 = signedFloat(amplitude);
}
/* 0259F5A4 */
void npc_readable::dNpc_HeadAnm_c::swing_horizone() {
    gabi::AutoFrame frame{0x0259F5A4};
    s16 before = field_0x1C;
    u16 phase = (u16)((s32)before + (s32)field_0x1E);
    f32 amplitude = field_0x18;
    field_0x1C = phase;
    f32 wave = cM_ssin(phase);
    s16 target = (s16)gabi::ftoi(gabi::fmuls_ppc(amplitude, wave));
    npc_addCalcAngle(&field_0x00.x, 0, 4, 0x1000, 0x100);
    npc_addCalcAngle(&field_0x00.y, target, 4, 0x1000, 0x100);
    if (before < 0 && field_0x1C >= 0) {
        s16 count = (s16)((s16)field_0x20 - 1);
        field_0x20 = count;
        if (count <= 0) {
            mProcFunc = 0;
            mProcDelta = 0;
            mProcIndex = 0;
        }
    }
}
/* 0259F67C */
void npc_readable::dNpc_HeadAnm_c::move() {
    gabi::AutoFrame frame{0x0259F67C};
    s16 index = mProcIndex;
    if (index) {
        // HD: the eight-byte GHS descriptor includes direct and virtual dispatch.
        auto* proc =
            reinterpret_cast<gabi::ptmf<void (npc_readable::dNpc_HeadAnm_c::*)()>*>(&mProcDelta);
        (this->*(*proc))();
    } else {
        npc_addCalcAngle(&field_0x00.x, 0, 4, 0x1000, 0x100);
        npc_addCalcAngle(&field_0x00.y, 0, 4, 0x1000, 0x100);
    }
}
/* 0259F740 */
static u32 dNpc_EventCut_c__ct(npc_readable::dNpc_EventCut_c* self) {
    gabi::AutoFrame frame{0x0259F740};
    if (!self)
        self = reinterpret_cast<npc_readable::dNpc_EventCut_c*>(
            npc_allocatePoint(sizeof(npc_readable::dNpc_EventCut_c)));
    if (!self)
        return 0;
    f32 zero = npc_zero;
    self->mpEvtStaffName = 0;
    self->pDelDistance = zero;
    self->mTimer = 0;
    self->mSpeed = zero;
    self->mpActorName = 0;
    self->mpJntCtrl = nullptr;
    self->mpTalkActor = nullptr;
    self->field_0x14 = 0;
    self->mpTargetActor = nullptr;
    self->field_0x20 = 0;
    self->mEvtStaffId = 0;
    self->mpActor = nullptr;
    self->mTurnType = 0;
    self->mCurActIdx = 0;
    self->mSetId = 0;
    self->mbAttention = 0;
    self->field_0x52 = 0;
    self->mbNoTurn = 0;
    self->mTurnSpeed = 0;
    self->field_0x44 = 0;
    self->mAddAngle = 0;
    return gabi::ea(self);
}
/* 0259F7D4 */
void npc_readable::dNpc_EventCut_c::setActorInfo(const char* name, fopAc_ac_c* actor) {
    gabi::AutoFrame frame{0x0259F7D4};
    mpActor = actor;
    f32 zero = npc_zero;
    mpEvtStaffName = gabi::ea(name);
    // HD: preserve the original scalar store order for overlapping caller objects.
    mTargetActorPos.x = zero;
    mOffsetPos.y = zero;
    mPos.z = zero;
    mPos.y = zero;
    mTargetActorPos.y = zero;
    mOffsetPos.z = zero;
    mOffsetPos.x = zero;
    mCurActIdx = -1;
    mPos.x = zero;
    mTargetActorPos.z = zero;
}
/* 0259F814 */
void npc_readable::dNpc_EventCut_c::setActorInfo2(const char* name, ::fopNpc_npc_c* actor) {
    gabi::AutoFrame frame{0x0259F814};
    mpEvtStaffName = gabi::ea(name);
    f32 zero = npc_zero;
    mCurActIdx = -1;
    mOffsetPos.x = zero;
    mTargetActorPos.y = zero;
    mTargetActorPos.z = zero;
    mPos.z = zero;
    mTargetActorPos.x = zero;
    mPos.x = zero;
    mPos.y = zero;
    mOffsetPos.y = zero;
    mpTalkActor = actor;
    mpActor = actor;
    mOffsetPos.z = zero;
}
/* 0259F858 */
BOOL npc_readable::dNpc_EventCut_c::cutProc() {
    gabi::AutoFrame frame{0x0259F858};
    const char* name = gabi::at<const char>(mpEvtStaffName);
    if (!name || !mpActor.get())
        return FALSE;
    s32 staff = dComIfGp_evmng_getMyStaffId(name, nullptr, 0);
    mEvtStaffId = staff;
    if (staff == -1)
        return FALSE;
    static char* action_table[] = {
        "WAIT",        "TURN_TO_ACTOR", "MOVE_TO_ACTOR", "TURN_TO_POS",
        "MOVE_TO_POS", "TALK_MSG",      "CONTINUE_TALK",
    };
    static cut_entry_t cut_tbl[] = {
        {&npc_readable::dNpc_EventCut_c::cutWaitStart, &npc_readable::dNpc_EventCut_c::cutWaitProc},
        {&npc_readable::dNpc_EventCut_c::cutTurnToActorStart,
         &npc_readable::dNpc_EventCut_c::cutTurnToActorProc},
        {&npc_readable::dNpc_EventCut_c::cutMoveToActorStart,
         &npc_readable::dNpc_EventCut_c::cutMoveToActorProc},
        {&npc_readable::dNpc_EventCut_c::cutTurnToPosStart,
         &npc_readable::dNpc_EventCut_c::cutTurnToPosProc},
        {&npc_readable::dNpc_EventCut_c::cutMoveToPosStart,
         &npc_readable::dNpc_EventCut_c::cutMoveToPosProc},
        {&npc_readable::dNpc_EventCut_c::cutTalkMsgStart,
         &npc_readable::dNpc_EventCut_c::cutTalkMsgProc},
        {&npc_readable::dNpc_EventCut_c::cutContinueTalkStart,
         &npc_readable::dNpc_EventCut_c::cutTalkMsgProc},
    };
    s32 index = dComIfGp_evmng_getMyActIdx(staff, action_table, 7, TRUE, 0);
    mCurActIdx = index;
    if (index == -1)
        return FALSE;
    staff = mEvtStaffId;
    bool advancing = dComIfGp_evmng_getIsAddvance(staff) != 0;
    if (advancing) {
        mbAttention = 0;
        (this->*cut_tbl[mCurActIdx].start)();
    }
    (this->*cut_tbl[mCurActIdx].proc)();
    return TRUE;
}
/* 0259FA00 */
void npc_readable::dNpc_EventCut_c::cutWaitStart() {
    gabi::AutoFrame frame{0x0259FA00};
    s32 staff = mEvtStaffId;
    auto* timer = dComIfGp_evmng_getMyIntegerP(staff, "Timer");
    // HD: load the low signed halfword from the integer event substance.
    mTimer = timer ? eventLow(timer) : 0;
}
/* 0259FA64 */
void npc_readable::dNpc_EventCut_c::cutWaitProc() {
    gabi::AutoFrame frame{0x0259FA64};
    if (!npc_calcTimer(&mTimer)) {
        s32 staff = mEvtStaffId;
        dComIfGp_evmng_cutEnd(staff);
    }
}
/* 025A1090 */
void npc_readable::dNpc_EventCut_c::cutTalkMsgStart() {
    gabi::AutoFrame frame{0x025A1090};
    if (!mpTalkActor.get())
        return;
    mbAttention = dComIfGp_evmng_getMyIntegerP(mEvtStaffId, "Attention") != nullptr;
    auto* begin = dComIfGp_evmng_getMyIntegerP(mEvtStaffId, "msg_num");
    auto* end = dComIfGp_evmng_getMyIntegerP(mEvtStaffId, "end_msg");
    auto* actor = mpTalkActor.get();
    u32 first = begin ? (u32)(s32)*begin : 0;
    actor->mCurrMsgNo = first;
    actor = mpTalkActor.get();
    actor->mEndMsgNo = end ? (u32)(s32)*end : 0xFFFFFFFFu;
    field_0x52 = 1;
}
/* 025A1178 */
void npc_readable::dNpc_EventCut_c::cutContinueTalkStart() {
    gabi::AutoFrame frame{0x025A1178};
    if (!mpTalkActor.get())
        return;
    auto* end = dComIfGp_evmng_getMyIntegerP(mEvtStaffId, "end_num");
    mpTalkActor->mEndMsgNo = end ? (u32)(s32)*end : 0xFFFFFFFFu;
}
/* 025A1388 */
void npc_readable::dNpc_EventCut_c::cutTalkMsgProc() {
    gabi::AutoFrame frame{0x025A1388};
    auto* actor = mpTalkActor.get();
    bool end = false;
    if (!actor || !field_0x52)
        end = true;
    else {
        // HD: this caller tests the entire return register from the message routine.
        u32 state = npc_talk(actor, 0);
        if (state == 18) {
            field_0x52 = 0;
            end = true;
        } else if (state == 2 || state == 6) {
            actor = mpTalkActor.get();
            if (actor->mEndMsgNo == actor->mCurrMsgNo) {
                actor->mEndMsgNo = 0xFFFFFFFFu;
                end = true;
            }
        }
    }
    if (end) {
        s32 staff = mEvtStaffId;
        dComIfGp_evmng_cutEnd(staff);
    }
}
/* 025A1458 */
static u32 fopNpc_npc_c__ct(npc_readable::fopNpc_npc_c* self) {
    gabi::AutoFrame frame{0x025A1458};
    if (!self)
        self = reinterpret_cast<npc_readable::fopNpc_npc_c*>(
            npc_allocatePoint(sizeof(npc_readable::fopNpc_npc_c)));
    if (!self)
        return 0;
    npc_actorCtor(self);
    self->__vtbl = gabi::ea(&npc_vtable);
    npc_jointCtor(&self->m_jnt);
    npc_eventCutCtor(&self->mEventCut);
    self->mpMorf = nullptr;
    npc_objAcchCtor(&self->mObjAcch);
    self->mObjAcch.cBgS_Chk::__vtbl = gabi::ea(&npc_objAcchCheckVtable);
    self->mObjAcch.dBgS_PolyPassChk::__vtbl = gabi::ea(&npc_polyPassVtable);
    self->mObjAcch.mbObjThrough = 1;
    self->mObjAcch.dBgS_GrpPassChk::__vtbl = gabi::ea(&npc_grpPassVtable);
    npc_acchCirCtor(&self->mAcchCir);
    npc_ccSttsCtor(&self->mStts);
    npc_gSttsCtor(&self->mStts.__vtbl_gstts);
    self->mStts.__vtbl = gabi::ea(&npc_sttsVtable);
    self->mStts.__vtbl_gstts = gabi::ea(&npc_gsttsVtable);
    npc_gObjInfCtor(&self->mCyl);
    auto* shape = reinterpret_cast<npc_readable::ShapeAttr*>(self->mCyl.mShapeAttr);
    shape->shapeVtable = gabi::ea(&npc_shapeAttrVtable);
    shape->aabVtable = gabi::ea(&npc_aabVtable);
    npc_cylinderCtor(&self->mCyl.mCyl);
    // HD: keep GHS's interleaved member/vtable store order.
    self->mMsgStatus = 0;
    self->mCurrMsgNo = 0;
    self->mPartnerProcId = 0;
    self->mCyl.mCyl.__vtbl = gabi::ea(&npc_cylVtable);
    self->mEndMsgNo = 0;
    self->mTalkState = 0;
    self->mCyl.__vtbl_hitinf = gabi::ea(&npc_hitInfoVtable);
    shape->shapeVtable = gabi::ea(&npc_cylShapeVtable);
    self->field_0x7d6 = 0;
    self->mbHasMsg = 0;
    self->mCurrMsgBsPcId = 0xFFFFFFFFu;
    return gabi::ea(self);
}
/* 025A15A0 */
static u32 fopNpc_npc_c_next_msgStatus() {
    gabi::AutoFrame frame{0x025A15A0};
    return 16;
}
/* 025A15A8 */
static void fopNpc_npc_c_anmAtr() { gabi::AutoFrame frame{0x025A15A8}; }
/* 025A15AC */
void npc_readable::fopNpc_npc_c::setCollision(f32 radius, f32 height) {
    gabi::AutoFrame frame{0x025A15AC};
    mCyl.SetC(&current.pos);
    mCyl.SetR(radius);
    mCyl.SetH(height);
    cCcS_Set(dComIfG_Ccsp(), &mCyl);
}
/* 025A1648 */
static void __sinit_d_npc_cpp() {
    gabi::AutoFrame frame{0x025A1648};
    npc_headerD0 = 0;
    npc_headerZ0 = 0;
    npc_headerD1 = 0;
    npc_headerZ1 = 0;
    npc_registerGlobal(&npc_headerRecord1);
    f32 first = npc_piFirstSource, second = npc_piSecondSource;
    npc_piFirst = first;
    npc_piSecond = second;
    npc_initializeHeaderByte(&npc_headerByte);
    npc_registerGlobal(&npc_headerRecord2);
    npc_initializeHeaderFlag(&npc_headerFlag);
    npc_registerGlobal(&npc_headerRecord3);
}
/* 025A16DC */
static void sead_SafeString__dt(void* self, u32 flags) {
    gabi::AutoFrame frame{0x025A16DC};
    if (self && (flags & 1))
        npc_delete(self);
}
/* 025A16F0 */
static u32 fopNpc_npc_c_getMsg() {
    gabi::AutoFrame frame{0x025A16F0};
    return 0;
}
/* 025A16F8 */
static void fopNpc_npc_c__dt(npc_readable::fopNpc_npc_c* self, u32 flags) {
    gabi::AutoFrame frame{0x025A16F8};
    if (!self)
        return;
    npc_colliderDt(&self->mCyl, 2);
    npc_sttsDt(&self->mStts, 2);
    npc_circleDt(&self->mAcchCir.m_cir, 2);
    self->mObjAcch.dBgS_GrpPassChk::__vtbl = gabi::ea(&npc_grpPassVtable);
    self->mObjAcch.dBgS_PolyPassChk::__vtbl = gabi::ea(&npc_polyPassVtable);
    npc_objAcchDt(&self->mObjAcch, 0);
    npc_actorDt(self, 0);
    if (flags & 1)
        npc_delete(self);
}
/* 025A1794 */
static void sead_SafeString_assureTerminationImpl() { gabi::AutoFrame frame{0x025A1794}; }
/* 025A11EC */
u16 npc_readable::fopNpc_npc_c::talk(s32 mode) {
    gabi::AutoFrame frame{0x025A11EC};
    // HD: snapshot the global message manager once, including across virtual calls.
    msg_class* message = mpCurrMsg.get();
    u16 state = 255;
    if (mCurrMsgBsPcId == 0xFFFFFFFFu) {
        u32 number;
        if (mode == 1) {
            auto* vtable = gabi::at<NpcVtable>(__vtbl);
            number = gabi::call_ptr<u32>(vtable->getMsg.get(), this);
            mCurrMsgNo = number;
        } else
            number = mCurrMsgNo;
        u32 id = npc_messageSet(message, number, &eyePos);
        mCurrMsgBsPcId = id;
        if (id != 0xFFFFFFFFu)
            mbHasMsg = 0;
        return state;
    }
    if (!mbHasMsg) {
        mbHasMsg = 1;
        return state;
    }
    state = (u16)npc_messageStatus(message);
    if (state == fopMsgStts_MSG_DISPLAYED_e) {
        auto* vtable = gabi::at<NpcVtable>(__vtbl);
        u32 next = gabi::call_ptr<u32>(vtable->next_msgStatus.get(), this, &mCurrMsgNo);
        npc_messageSetStatus(message, next);
        // HD: this second status test uses the entire return register.
        if (npc_messageStatus(message) == fopMsgStts_MSG_CONTINUES_e)
            npc_messageSet(message, mCurrMsgNo, nullptr);
    } else if (state == fopMsgStts_BOX_CLOSED_e) {
        npc_messageSetStatus(message, fopMsgStts_MSG_DESTROYED_e);
        mCurrMsgBsPcId = 0xFFFFFFFFu;
    }
    auto* vtable = gabi::at<NpcVtable>(__vtbl);
    gabi::call_ptr(vtable->anmAtr.get(), this, (u32)state);
    return state;
}
/* 0259EE7C */
BOOL npc_readable::dNpc_PathRun_c::setNearPathIndx(cXyz* position, f32 weight) {
    gabi::AutoFrame frame{0x0259EE7C};
    if (!mPath.get())
        return FALSE;
    f32 best = npc_infinity, zero = npc_zero;
    u32 selected = 0;
    for (s32 index = 0; index < (s32)maxPoint(); index++) {
        gabi::Local<cXyz> point, copy, flat;
        getPoint(point.get(), (u32)index & 255);
        copy->copy(*point);
        npc_vectorSubtract(position, point.get(), copy.get());
        f32 dy = point->y;
        flat->x = point->x;
        flat->y = zero;
        flat->z = point->z;
        f32 horizontal = npc_vectorSquareMag(flat.get());
        f32 squared = gabi::fmadds(gabi::fmuls_ppc(dy, dy), weight, horizontal);
        f32 distance = npc_sqrt(squared);
        if (best > distance) {
            best = distance;
            selected = (u32)index & 255;
        }
    }
    mIdx = selected;
    return TRUE;
}
/* 0259EFE0 */
f32 npc_readable::dNpc_PathRun_c::setNearPathIndxMk(cXyz* position) {
    gabi::AutoFrame frame{0x0259EFE0};
    f32 best = npc_infinity;
    if (!mPath.get())
        return best;
    u32 selected = 0;
    for (s32 index = 0; index < (s32)maxPoint(); index++) {
        gabi::Local<cXyz> point, copy, difference;
        getPoint(point.get(), (u32)index & 255);
        copy->copy(*point);
        npc_vectorSubtract(position, point.get(), copy.get());
        difference->copy(*point);
        f32 squared = npc_vectorSquareMag(difference.get());
        f32 distance = npc_sqrt(squared);
        if (best > distance) {
            best = distance;
            selected = (u32)index & 255;
        }
    }
    mIdx = selected;
    return best;
}
/* 0259F0F8 */
BOOL npc_readable::dNpc_PathRun_c::setNearPathIndxMk2(cXyz* position, u32 current,
                                                      s32 maxDifference) {
    gabi::AutoFrame frame{0x0259F0F8};
    if (!mPath.get())
        return FALSE;
    f32 best = npc_infinity;
    u32 selected = current;
    BOOL found = FALSE;
    for (s32 index = 0; index < (s32)maxPoint(); index++) {
        gabi::Local<cXyz> point, copy, difference;
        getPoint(point.get(), (u32)index & 255);
        copy->copy(*point);
        npc_vectorSubtract(position, point.get(), copy.get());
        difference->copy(*point);
        f32 squared = npc_vectorSquareMag(difference.get());
        f32 distance = npc_sqrt(squared);
        s32 delta = absIdx(current, (u32)index & 255);
        // HD: the comparison uses the full-width current index and maximum difference.
        if (delta <= maxDifference && current != (u32)index && best > distance) {
            best = distance;
            found = TRUE;
            selected = (u32)index & 255;
        }
    }
    mIdx = selected;
    return found;
}
/* 0259F220 */
BOOL npc_readable::dNpc_PathRun_c::chkInside(cXyz* position) {
    gabi::AutoFrame frame{0x0259F220};
    setNearPathIndx(position, npc_zero);
    gabi::Local<cXyz> temporary, current, previous, next;
    getPoint(temporary.get(), mIdx);
    current->copy(*temporary);
    decIdxLoop();
    getPoint(temporary.get(), mIdx);
    previous->copy(*temporary);
    incIdxLoop();
    incIdxLoop();
    getPoint(temporary.get(), mIdx);
    next->copy(*temporary);
    s32 first = npc_targetAngleY(current.get(), previous.get());
    s32 direction = npc_targetAngleY(current.get(), position);
    s32 last = npc_targetAngleY(current.get(), next.get());
    s16 a = (s16)((u32)direction - (u32)last);
    s16 b = (s16)((u32)first - (u32)last);
    if (a > 0) {
        if (b < 0)
            return TRUE;
        return a < b;
    }
    if (b >= 0)
        return FALSE;
    return a < b;
}
/* 0259E838 */
BOOL npc_readable::dNpc_PathRun_c::chkPointPass(cXyz* position, u32 forward) {
    gabi::AutoFrame frame{0x0259E838};
    auto* path = reinterpret_cast<path_layout::dPath*>(mPath.get());
    if (!path)
        return FALSE;
    u32 index = mIdx;
    auto* points = path->m_points.get();
    auto* point = &points[index];
    f32 x = point->m_position.x, z = point->m_position.z;
    path_layout::dPnt *from, *to;
    if (!index) {
        from = points;
        to = points + 1;
    } else if (index == (u32)(u16)path->m_num - 1) {
        to = points + (u16)path->m_num - 1;
        from = to - 1;
    } else {
        from = point - 1;
        to = point + 1;
    }
    f32 dx = gabi::fsubs_ppc(to->m_position.x, from->m_position.x);
    f32 dz = gabi::fsubs_ppc(to->m_position.z, from->m_position.z);
    // HD: the tangent angle is computed separately for each table lookup.
    u16 first = (u16)npc_atan2s(dx, dz);
    f32 scale = npc_pathPlaneScale;
    f32 sin = gabi::fmuls_ppc(scale, cM_ssin(first));
    u16 second = (u16)npc_atan2s(dx, dz);
    f32 cos = gabi::fmuls_ppc(scale, cM_scos(second));
    f32 plane = gabi::fmadds(sin, x, gabi::fmuls_ppc(cos, z));
    f32 px = position->x, pz = position->z;
    f32 side = gabi::fadds_ppc(gabi::fmadds(sin, px, gabi::fmuls_ppc(cos, pz)), -plane);
    f32 zero = npc_zero;
    // HD: the backward case also accepts an unordered result.
    return forward ? side > zero : !(side > zero);
}
/* 0259FAB0 */
void npc_readable::dNpc_EventCut_c::cutTurnToActorStart() {
    gabi::AutoFrame frame{0x0259FAB0};
    mpActorName = gabi::ea(dComIfGp_evmng_getMyStringP(mEvtStaffId, "ActorName"));
    auto* set = dComIfGp_evmng_getMyIntegerP(mEvtStaffId, "SetId");
    mSetId = set ? (s32)*set : 0;
    auto* pos = dComIfGp_evmng_getMyXyzP(mEvtStaffId, "OffsetPos");
    f32 zero = npc_zero;
    if (pos)
        mOffsetPos.copy(*pos);
    else {
        mOffsetPos.y = zero;
        mOffsetPos.z = zero;
        mOffsetPos.x = zero;
    }
    auto* turnType = dComIfGp_evmng_getMyIntegerP(mEvtStaffId, "TurnType");
    mTurnType = turnType ? (u32)(s32)*turnType : 1;
    auto* timer = dComIfGp_evmng_getMyIntegerP(mEvtStaffId, "Timer");
    mTimer = timer ? eventLow(timer) : 1;
    auto* speed = dComIfGp_evmng_getMyIntegerP(mEvtStaffId, "TurnSpeed");
    mTurnSpeed = speed ? eventLow(speed) : 0;
    auto* angle = dComIfGp_evmng_getMyIntegerP(mEvtStaffId, "AddAngle");
    mAddAngle = angle ? eventLow(angle) : 0;
    auto* dist = dComIfGp_evmng_getMyFloatP(mEvtStaffId, "DelDistance");
    pDelDistance = dist ? (f32)*dist : zero;
    auto* noTurn = dComIfGp_evmng_getMyIntegerP(mEvtStaffId, "NoTurn");
    field_0x44 = 0;
    mbNoTurn = noTurn != nullptr;
}
/* 0259FD08 */
static u32 dNpc_EventCut_c_findActorCallBack(fopAc_ac_c* actor,
                                             npc_readable::dNpc_EventCut_c* cut) {
    gabi::AutoFrame frame{0x0259FD08};
    if (!cut)
        return 0;
    u32 id = cut->mSetId;
    if (id && actor->setID == id) {
        cut->mTargetActorPos.copy(actor->current.pos);
        cut->mpTargetActor = actor;
        return gabi::ea(actor);
    }
    const char* name = gabi::at<const char>(cut->mpActorName);
    if (!name)
        return 0;
    auto* record = npc_searchName(name);
    if (!record)
        return 0;
    s32 profile = actor ? (s16)actor->mProfName : 0x7FFF;
    if ((s32)record->procname != profile)
        return 0;
    if (actor->argument != record->argument)
        return 0;
    f32 squared = npc_vectorSquareMag(&cut->mTargetActorPos);
    f32 distance = npc_sqrt(squared);
    if (distance != npc_zero) {
        gabi::Local<cXyz> delta;
        npc_vectorSubtract(&cut->mTargetActorPos, delta.get(), &cut->mpActor->current.pos);
        squared = npc_vectorSquareMag(delta.get());
        f32 old = npc_sqrt(squared);
        npc_vectorSubtract(&actor->current.pos, delta.get(), &cut->mpActor->current.pos);
        squared = npc_vectorSquareMag(delta.get());
        distance = npc_sqrt(squared);
        if (!(distance < old))
            return 0;
    }
    cut->mTargetActorPos.copy(actor->current.pos);
    cut->mpTargetActor = actor;
    return 0;
}

/* 025A0984 */
void npc_readable::dNpc_EventCut_c::cutTurnToPosStart() {
    gabi::AutoFrame frame{0x025A0984};
    auto* pos = dComIfGp_evmng_getMyXyzP(mEvtStaffId, "Pos");
    f32 zero = npc_zero;
    if (pos)
        mPos.copy(*pos);
    else {
        mPos.y = zero;
        mPos.x = zero;
        mPos.z = zero;
    }
    auto* turnType = dComIfGp_evmng_getMyIntegerP(mEvtStaffId, "TurnType");
    mTurnType = turnType ? (u32)(s32)*turnType : 1;
    auto* timer = dComIfGp_evmng_getMyIntegerP(mEvtStaffId, "Timer");
    mTimer = timer ? eventLow(timer) : 1;
    auto* speed = dComIfGp_evmng_getMyIntegerP(mEvtStaffId, "TurnSpeed");
    mTurnSpeed = speed ? eventLow(speed) : 0;
    auto* noTurn = dComIfGp_evmng_getMyIntegerP(mEvtStaffId, "NoTurn");
    mbNoTurn = noTurn != nullptr;
    auto* angle = dComIfGp_evmng_getMyIntegerP(mEvtStaffId, "AddAngle");
    mAddAngle = angle ? eventLow(angle) : 0;
    field_0x44 = 0;
}
static void npc_endCut(dNpc_EventCut_c* self) {
    s32 staff = self->mEvtStaffId;
    dComIfGp_evmng_cutEnd(staff);
}
/* 025A0B3C */
void npc_readable::dNpc_EventCut_c::cutTurnToPosProc() {
    gabi::AutoFrame frame{0x025A0B3C};
    u32 yaw = npc_targetAngleY(&mpActor->current.pos, &mPos);
    auto* joint = mpJntCtrl.get();
    mbAttention = 1;
    if (joint) {
        u32 mode = mTurnType;
        if (mode == 2) {
            joint->mbTrn = 1;
            joint = mpJntCtrl.get();
        }
        joint->mbBackBoneLock = mode == 0;
    }
    // HD: the difference is a full-width signed integer before GHS float conversion.
    f32 absolute = std::fabs(signedFloat((s32)(yaw - (u32)(s32)mpActor->current.angle.y)));
    if (signedFloat(field_0x44) == absolute && absolute < npc_turnTolerance)
        npc_calcTimer(&mTimer);
    if (!mTimer) {
        mTurnSpeed = 0;
        npc_endCut(this);
    }
    absolute = std::fabs(signedFloat((s32)(yaw - (u32)(s32)mpActor->current.angle.y)));
    field_0x44 = gabi::ftoi(absolute);
}
/* 025A0D5C */
void npc_readable::dNpc_EventCut_c::cutMoveToPosStart() {
    gabi::AutoFrame frame{0x025A0D5C};
    auto* pos = dComIfGp_evmng_getMyXyzP(mEvtStaffId, "Pos");
    auto* speed = dComIfGp_evmng_getMyFloatP(mEvtStaffId, "Speed");
    auto* dist = dComIfGp_evmng_getMyFloatP(mEvtStaffId, "DelDistance");
    auto* attn = dComIfGp_evmng_getMyIntegerP(mEvtStaffId, "Attention");
    auto* noTurn = dComIfGp_evmng_getMyIntegerP(mEvtStaffId, "NoTurn");
    auto* angle = dComIfGp_evmng_getMyIntegerP(mEvtStaffId, "AddAngle");
    f32 zero = npc_zero;
    mbNoTurn = noTurn != nullptr;
    if (pos)
        mPos.copy(*pos);
    else {
        mPos.z = zero;
        mPos.x = zero;
        mPos.y = zero;
    }
    mbAttention = attn != nullptr;
    // HD: these two assignments copy words without quieting signalling NaNs.
    if (speed)
        mSpeed = *speed;
    else
        mSpeed = npc_defaultMoveSpeed;
    if (dist)
        pDelDistance = *dist;
    else
        pDelDistance = npc_zero;
    mAddAngle = angle ? eventLow(angle) : 0;
}

/* 025A0F00 */
void npc_readable::dNpc_EventCut_c::cutMoveToPosProc() {
    gabi::AutoFrame frame{0x025A0F00};
    u16 yaw = (u16)npc_targetAngleY(&mpActor->current.pos, &mPos);
    f32 speed = mSpeed, distance = pDelDistance, goalX = mPos.x;
    f32 sin = cM_ssin(yaw), zero = npc_zero;
    f32 x = gabi::fnmsubs(distance, sin, goalX);
    f32 cos = cM_scos(yaw), goalZ = mPos.z, y = mPos.y;
    f32 z = gabi::fnmsubs(distance, cos, goalZ);
    gabi::Local<cXyz> target, delta, flat;
    target->x = x;
    target->y = y;
    target->z = z;
    if (speed == zero) {
        auto* actor = mpActor.get();
        actor->current.pos.x = x;
        actor->current.pos.y = y;
        actor->current.pos.z = z;
        npc_endCut(this);
    }
    // HD: even a zero-speed teleport continues through the distance/movement calls.
    npc_vectorSubtract(target.get(), delta.get(), &mpActor->current.pos);
    flat->x = delta->x;
    flat->z = delta->z;
    flat->y = zero;
    f32 squared = npc_vectorSquareMag(flat.get());
    f32 remaining = npc_sqrt(squared), factor = npc_moveFactor;
    npc_addCalc2(&mpActor->current.pos.x, target->x, factor, mSpeed);
    npc_addCalc2(&mpActor->current.pos.z, target->z, factor, mSpeed);
    if (remaining < npc_moveTolerance) {
        mSpeed = zero;
        npc_endCut(this);
    }
}

/* 0259E2D4 */
void npc_readable::dNpc_JntCtrl_c::lookAtTarget_2(be<s16>* angle, cXyz* target, cXyz* origin,
                                                  s32 yaw, u32 velocity, u32 locked) {
    gabi::AutoFrame frame{0x0259E2D4};
    s32 pitch = 0;
    if (target) {
        yaw = npc_targetAngleY(origin, target);
        pitch = npc_targetAngleX(origin, target);
    }
    s16 relative = (s16)((u32)yaw - (u32)(s32)*angle);
    s32 previous = field_0x32;
    bool split = previous < 0 ? relative > previous : (relative < previous && previous != 0);
    gabi::Local<be<s16>> head, back;
    if (split)
        turn_fromBackbone2Head(relative, head.get(), back.get(), previous >= 0);
    else
        turn_fromHead2Backbone(relative, head.get(), back.get());
    s32 backTarget = (s16)*back;
    field_0x32 = backTarget;
    gabi::Local<be<s32>> headValue, backValue;
    *headValue = mAngles[0][1];
    *backValue = mAngles[1][1];
    s32 headTarget = (s16)*head;
    field_0x2E = headTarget;
    npc_addCalcAngleL(headValue.get(), headTarget, 4, mMaxTurnStep[0][0], 4);
    npc_addCalcAngleL(backValue.get(), backTarget, 4, mMaxTurnStep[0][0], 4);
    mAngles[0][1] = (s32)*headValue;
    bool following = mbTrn != 0;
    mAngles[1][1] = (s32)*backValue;
    if (following && !locked) {
        s16 old = *angle;
        npc_addCalcAngle(angle, yaw, 4, velocity, 0x100);
        s16 change = (s16)((u32)(s32)*angle - (u32)(s32)old);
        bool blocked = mbBackBoneLock != 0;
        mbTrn = change != 0;
        s32 amount = change;
        // HD: retain the full return register when passing the remainder to the head.
        if (!blocked)
            amount = npc_jointConsume(this, &mAngles[1][1], amount);
        if (!mbHeadLock)
            npc_jointConsume(this, &mAngles[0][1], amount);
    } else {
        s32 total = (s32)field_0x2E + (s32)field_0x32;
        mbTrn = relative < 0 ? relative < total : relative > total;
    }
    s32 top = npc_jointLimit(this, pitch, 0, 0);
    s32 bottom = npc_jointLimit(this, (s16)((u32)pitch - (u32)top), 1, 0);
    *headValue = mAngles[0][0];
    s32 rate = mMaxTurnStep[0][0];
    field_0x30 = bottom;
    *backValue = mAngles[1][0];
    field_0x2C = top;
    npc_addCalcAngleL(headValue.get(), (s16)top, 4, rate, 4);
    npc_addCalcAngleL(backValue.get(), bottom, 4, mMaxTurnStep[0][0], 4);
    mAngles[0][0] = (s32)*headValue;
    mAngles[1][0] = (s32)*backValue;
}

static bool npc_sameText(const char* a, const char* b) {
    // HD: event names may be arbitrary guest addresses; use the guest byte-read boundary.
    u32 first = gabi::guest_addr(a), second = gabi::guest_addr(b);
    for (;;) {
        u8 x = gabi::load<u8>(first++), y = gabi::load<u8>(second++);
        if (x != y)
            return false;
        if (!x)
            return true;
    }
}
static void npc_assure(NpcString* string) {
    auto* vtable = gabi::at<NpcSafeStringVtable>(string->vt);
    gabi::call_ptr(vtable->assure.get(), string);
}
static bool safeTextPair(gabi::Local<NpcString>& a, gabi::Local<NpcString>& b) {
    npc_assure(a.get());
    u32 saved = a->data;
    npc_assure(b.get());
    if (saved == (u32)b->data)
        return true;
    u32 first = a->data, second = b->data;
    // HD: SafeString comparison is bounded and reads through the guest boundary.
    for (u32 n = 0; n < 0x40001; n++) {
        u8 x = gabi::load<u8>(first++), y = gabi::load<u8>(second++);
        if (x != y)
            return false;
        if (!x)
            return true;
    }
    return false;
}
/* 025A02DC */
void npc_readable::dNpc_EventCut_c::cutMoveToActorStart() {
    gabi::AutoFrame frame{0x025A02DC};
    mpActorName = gabi::ea(dComIfGp_evmng_getMyStringP(mEvtStaffId, "ActorName"));
    auto* id = dComIfGp_evmng_getMyIntegerP(mEvtStaffId, "SetId");
    auto* speed = dComIfGp_evmng_getMyFloatP(mEvtStaffId, "Speed");
    auto* distance = dComIfGp_evmng_getMyFloatP(mEvtStaffId, "DelDistance");
    auto* position = dComIfGp_evmng_getMyXyzP(mEvtStaffId, "OffsetPos");
    auto* attention = dComIfGp_evmng_getMyIntegerP(mEvtStaffId, "Attention");
    auto* noTurn = dComIfGp_evmng_getMyIntegerP(mEvtStaffId, "NoTurn");
    auto* angle = dComIfGp_evmng_getMyIntegerP(mEvtStaffId, "ShapeAngle");
    mbNoTurn = noTurn != nullptr;
    f32 zero = npc_zero;
    mbAttention = attention != nullptr;
    if (position)
        mOffsetPos.copy(*position);
    else {
        mOffsetPos.x = zero;
        mOffsetPos.y = zero;
        mOffsetPos.z = zero;
    }
    mSetId = id ? (s32)*id : 0;
    if (speed)
        mSpeed = *speed;
    else
        mSpeed = npc_defaultMoveSpeed;
    if (distance)
        pDelDistance = *distance;
    else
        pDelDistance = npc_zero;
    // HD: preserve the stage/staff/actor-specific correction and all SafeString calls.
    if (distance) {
        gabi::Local<NpcString> key, actual;
        key->data = gabi::guest_addr("Pnezumi");
        key->vt = gabi::ea(&npc_safeStringVtable);
        const char* stage = dComIfGp_getStartStageName();
        actual->vt = gabi::ea(&npc_safeStringVtable);
        actual->data = gabi::ea(stage);
        npc_assure(key.get());
        bool matched = safeTextPair(key, actual);
        if (matched) {
            gabi::Local<NpcString> objectKey, object;
            objectKey->data = gabi::guest_addr("Tc");
            object->data = mpEvtStaffName;
            object->vt = gabi::ea(&npc_safeStringVtable);
            objectKey->vt = gabi::ea(&npc_safeStringVtable);
            npc_assureString(objectKey.get());
            matched = safeTextPair(objectKey, object);
            if (matched) {
                gabi::Local<NpcString> actorKey, actor;
                actorKey->data = gabi::guest_addr("KNOB00");
                actor->data = mpActorName;
                actor->vt = gabi::ea(&npc_safeStringVtable);
                actorKey->vt = gabi::ea(&npc_safeStringVtable);
                npc_assureString(actorKey.get());
                matched = safeTextPair(actorKey, actor);
            }
        }
        if (matched && gabi::ftoi(pDelDistance) == 100)
            pDelDistance = gabi::fadds_ppc(pDelDistance, npc_rodentMoveCorrection);
    }
    mAddAngle = angle ? eventLow(angle) : 0;
}

/* 0259FE88 */
void npc_readable::dNpc_EventCut_c::cutTurnToActorProc() {
    gabi::AutoFrame frame{0x0259FE88};
    f32 zero = npc_zero;
    if (!mSetId && (!mpActorName || npc_sameText(gabi::at<const char>(mpActorName), "Player"))) {
        mpTargetActor = dComIfGp_getPlayer(0);
    } else {
        mTargetActorPos.x = zero;
        mTargetActorPos.y = zero;
        mTargetActorPos.z = zero;
        npc_searchActors(dNpc_EventCut_c_findActorCallBack, this);
    }
    auto* joint = mpJntCtrl.get();
    if (joint) {
        u32 mode = mTurnType;
        if (mode == 2) {
            joint->mbTrn = 1;
            joint = mpJntCtrl.get();
        }
        joint->mbBackBoneLock = mode == 0;
    }
    // HD: a missing target ends the cut but does not return from this handler.
    if (!mpTargetActor.get())
        npc_endCut(this);
    const char* name = gabi::at<const char>(mpActorName);
    if (!name || npc_sameText(name, "Player")) {
        gabi::Local<cXyz> eye, goal;
        npc_playerEye(eye.get(), zero);
        npc_vectorAdd(eye.get(), goal.get(), &mOffsetPos);
        mPos.copy(*goal);
    } else if (mAddAngle && pDelDistance != zero) {
        auto* actor = mpTargetActor.get();
        if (actor) {
            mPos.x = actor->current.pos.x;
            u16 yaw = (u16)((u32)(s32)actor->shape_angle.y + (u32)(s32)mAddAngle);
            mPos.y = actor->current.pos.y;
            f32 x = mPos.x;
            mPos.z = actor->current.pos.z;
            f32 distance = pDelDistance, sin = cM_ssin(yaw);
            mPos.x = gabi::fnmsubs(distance, sin, x);
            mPos.z = gabi::fnmsubs(distance, cM_scos(yaw), mPos.z);
        }
    } else {
        gabi::Local<cXyz> goal;
        npc_vectorAdd(&mpTargetActor->current.pos, goal.get(), &mOffsetPos);
        mPos.copy(*goal);
    }
    mbAttention = 1;
    u32 yaw = npc_targetAngleY(&mpActor->current.pos, &mPos);
    f32 absolute = std::fabs(signedFloat((s32)(yaw - (u32)(s32)mpActor->current.angle.y)));
    if (signedFloat(field_0x44) == absolute && absolute < npc_turnTolerance)
        npc_calcTimer(&mTimer);
    if (!mTimer) {
        mTurnSpeed = 0;
        npc_endCut(this);
    }
    absolute = std::fabs(signedFloat((s32)(yaw - (u32)(s32)mpActor->current.angle.y)));
    field_0x44 = gabi::ftoi(absolute);
}
/* 025A0730 */
void npc_readable::dNpc_EventCut_c::cutMoveToActorProc() {
    gabi::AutoFrame frame{0x025A0730};
    f32 zero = npc_zero;
    if (!mSetId && (!mpActorName || npc_sameText(gabi::at<const char>(mpActorName), "Player"))) {
        mpTargetActor = dComIfGp_getPlayer(0);
    } else {
        mTargetActorPos.y = zero;
        mTargetActorPos.z = zero;
        mTargetActorPos.x = zero;
        npc_searchActors(dNpc_EventCut_c_findActorCallBack, this);
    }
    auto* actor = mpTargetActor.get();
    // HD: unlike the turn handler, movement returns when the target is missing.
    if (!actor) {
        npc_endCut(this);
        return;
    }
    gabi::Local<cXyz> target, delta, flat;
    npc_vectorAdd(&actor->current.pos, target.get(), &mOffsetPos);
    u32 yaw = npc_searchActorAngleY(mpActor.get(), mpTargetActor.get());
    s32 angle = mAddAngle;
    if (angle)
        yaw = (u32)(s32)(s16)((u32)(s32)mpTargetActor->shape_angle.y + (u32)angle);
    yaw &= 65535;
    f32 z = target->z, distance = pDelDistance, x = target->x;
    f32 sin = cM_ssin(yaw), cos = cM_scos(yaw);
    x = gabi::fnmsubs(distance, sin, x);
    f32 speed = mSpeed;
    z = gabi::fnmsubs(distance, cos, z);
    target->x = x;
    target->z = z;
    if (speed == zero) {
        actor = mpActor.get();
        actor->current.pos.x = x;
        actor->current.pos.z = z;
        actor->current.pos.y = target->y;
        npc_endCut(this);
    }
    npc_vectorSubtract(target.get(), delta.get(), &mpActor->current.pos);
    flat->x = delta->x;
    flat->z = delta->z;
    flat->y = zero;
    f32 squared = npc_vectorSquareMag(flat.get());
    f32 remaining = npc_sqrt(squared), factor = npc_moveFactor;
    npc_addCalc2(&mpActor->current.pos.x, target->x, factor, mSpeed);
    npc_addCalc2(&mpActor->current.pos.z, target->z, factor, mSpeed);
    if (remaining < npc_moveTolerance) {
        mSpeed = zero;
        npc_endCut(this);
    }
}

// clang-format off
/* WWHD glue
 data npc_vtable 10051858
 data npc_objAcchCheckVtable 1005157C
 data npc_polyPassVtable 1005159C
 data npc_grpPassVtable 1005158C
 data npc_sttsVtable 1004AE88
 data npc_gsttsVtable 1004AEC0
 data npc_shapeAttrVtable 100015A8
 data npc_aabVtable 1005156C
 data npc_cylVtable 1004B150
 data npc_hitInfoVtable 1004B108
 data npc_cylShapeVtable 1004B160
 data npc_headerD0 1047B0DC
 data npc_headerZ0 1047B0D4
 data npc_headerD1 1047B0E0
 data npc_headerZ1 1047B0D8
 data npc_piFirst 1047B0C8
 data npc_piSecond 1047B0CC
 data npc_piFirstSource 1005184C
 data npc_piSecondSource 10051850
 data npc_headerByte 1047B0D0
 data npc_headerFlag 1047B0D1
 data npc_headerRecord1 101EA16C
 data npc_headerRecord2 101EA178
 data npc_headerRecord3 101EA184
 ext 025D4ED0 void npc_actorCtor(fopAc_ac_c* actor)
 ext 0259DAA0 void npc_jointCtor(dNpc_JntCtrl_c* joint)
 ext 0259F740 void npc_eventCutCtor(dNpc_EventCut_c* cut)
 ext 024F0474 void npc_objAcchCtor(dBgS_ObjAcch* acch)
 ext 024EFE94 void npc_acchCirCtor(dBgS_AcchCir* circle)
 ext 0200BD2C void npc_ccSttsCtor(dCcD_Stts* status)
 ext 02515DA0 void npc_gSttsCtor(be<u32>* status)
 ext 02515FB8 void npc_gObjInfCtor(dCcD_GObjInf* collider)
 ext 02018590 void npc_cylinderCtor(cM3dGCyl* cylinder)
 ext 02515A70 void npc_colliderDt(dCcD_Cyl* collider, u32 flags)
 ext 02515860 void npc_sttsDt(dCcD_Stts* status, u32 flags)
 ext 02018034 void npc_circleDt(cM3dGCir_l* circle, u32 flags)
 ext 024EFD9C void npc_objAcchDt(dBgS_ObjAcch* acch, u32 flags)
 ext 025D50BC void npc_actorDt(fopAc_ac_c* actor, u32 flags)
 ext 0273AF40 void npc_delete(void* object)
 ext 028F026C void npc_registerGlobal(npc_readable::GlobalObjectRecord* record)
 ext 028ED6F8 void npc_initializeHeaderByte(be<u8>* object)
 ext 028EAB2C void npc_initializeHeaderFlag(be<u8>* object)
 ext 025F7DB0 u32 npc_messageSet(msg_class* message, u32 number, cXyz* eye)
 ext 025F795C u32 npc_messageStatus(msg_class* message)
 ext 025F74D0 void npc_messageSetStatus(msg_class* message, u32 state)
 data npc_animEnd 100515C0
 data npc_maxAttnAngle 100515D8
 data npc_degreesToAngle 100515DC
 ext 026067F4 void* npc_getObjectIDRes(dRes_control_c* manager, NpcString* arc, s32 id)
 ext 026066C4 void* npc_getObjectRes(dRes_control_c* manager, NpcString* arc, s32 id)
 ext 02606900 void* npc_getObjectNamedRes(dRes_control_c* manager, NpcString* arc, NpcString* name)
 ext 025E4A98 void npc_setMorph(mDoExt_McaMorf* morf, void* anim, u32 loop, void* sound, f32 blend, f32 speed, f32 start, f32 end)
 data action_table 101EA150
 data cut_tbl 100515F0
 data npc_safeStringVtable 10051554
 data npc_rodentMoveCorrection 10051734
 ext 025A1794 void npc_assureString(NpcString* string)
 str 10051770 "ActorName" in 025A02DC
 str 10051740 "SetId" in 025A02DC
 str 10051748 "Speed" in 025A02DC
 str 10051764 "DelDistance" in 025A02DC
 str 1005177C "OffsetPos" in 025A02DC
 str 10051788 "Attention" in 025A02DC
 str 10051750 "NoTurn" in 025A02DC
 str 10051794 "ShapeAngle" in 025A02DC
 str 10051738 "Pnezumi" in 025A02DC
 str 10051758 "Tc" in 025A02DC
 str 1005175C "KNOB00" in 025A02DC
 str 10051728 "Player" in 0259FE88
 str 100517A8 "Player" in 025A0730
 ext 025C109C npc_readable::dStage_objectNameInf* npc_searchName(const char* name)
 ext 025D5218 u32 npc_searchActors(NpcSearchCallback callback, npc_readable::dNpc_EventCut_c* cut)
 ext 0201AD78 void npc_vectorAdd(cXyz* a, cXyz* result, cXyz* b)
 ext 0259D54C u32 npc_playerEye(cXyz* result, f32 offset)
 ext 025D6894 u32 npc_searchActorAngleY(fopAc_ac_c* actor, fopAc_ac_c* target)
 data npc_turnTolerance 10051724
 data npc_moveFactor 100517A0
 data npc_moveTolerance 100517A4
 ext 0200ED84 void npc_addCalc2(be<f32>* value, f32 goal, f32 factor, f32 speed)
 data npc_defaultMoveSpeed 10051730
 str 100516E8 "ActorName" in 0259FAB0
 str 100516C4 "SetId" in 0259FAB0
 str 100516F4 "OffsetPos" in 0259FAB0
 str 1005170C "TurnType" in 0259FAB0
 str 100516CC "Timer" in 0259FAB0
 str 10051700 "TurnSpeed" in 0259FAB0
 str 10051718 "AddAngle" in 0259FAB0
 str 100516DC "DelDistance" in 0259FAB0
 str 100516D4 "NoTurn" in 0259FAB0
 str 100517B0 "Pos" in 025A0984
 str 100517D0 "TurnType" in 025A0984
 str 100517B4 "Timer" in 025A0984
 str 100517C4 "TurnSpeed" in 025A0984
 str 100517BC "NoTurn" in 025A0984
 str 100517DC "AddAngle" in 025A0984
 str 100517E8 "Pos" in 025A0D5C
 str 100517EC "Speed" in 025A0D5C
 str 100517FC "DelDistance" in 025A0D5C
 str 10051808 "Attention" in 025A0D5C
 str 100517F4 "NoTurn" in 025A0D5C
 str 10051814 "AddAngle" in 025A0D5C
 str 100516BC "Timer" in 0259FA00
 str 10051830 "Attention" in 025A1090
 str 10051820 "msg_num" in 025A1090
 str 10051828 "end_msg" in 025A1090
 str 1005183C "end_num" in 025A1178
 ext 0211D2F8 u32 npc_calcTimer(be<s32>* timer)
 ext 025A11EC u32 npc_talk(fopNpc_npc_c* actor, s32 mode)
 data npc_signedFloatBias 100515E8
 data npc_pathPlaneScale 100515C4
 data npc_infinity 100515E0
 ext 0201ADE0 void npc_vectorSubtract(cXyz* a, cXyz* result, cXyz* b)
 ext 028E8DD0 f32 npc_vectorSquareMag(cXyz* vector)
 data npc_hio_vtable 100515AC
 ext 028F521C void npc_clearMemory(void* memory, u32 size)
 ext 0200F93C s32 npc_targetAngleY(cXyz* origin, cXyz* target)
 ext 0200F974 s32 npc_targetAngleX(cXyz* origin, cXyz* target)
 ext 0259E28C s32 npc_jointConsume(npc_readable::dNpc_JntCtrl_c* self, be<s16>* angle, s32 amount)
 ext 0200F474 void npc_addCalcAngleL(be<s32>* angle, s32 goal, s32 scale, s32 maximum, s32 minimum)
 ext 0200FAAC s32 npc_angleDistance(s32 a, s32 b)
 ext 0259E0C8 s32 npc_jointLimit(npc_readable::dNpc_JntCtrl_c* self, s32 value, u32 joint, u32 axis)
 ext 028F4384 f32 npc_sqrt(f32 value)
 ext 020195B0 s32 npc_atan2s(f32 x, f32 z)
 data npc_zero 100515BC
 ext 0200F378 void npc_addCalcAngle(be<s16>* angle, s32 goal, s32 scale, s32 maximum, s32 minimum)
 ext 025AAF88 u32 npc_pathGetRoom(s32 id, s32 room)
 ext 025AB070 u32 npc_pathGetNext(u32 path)
 ext 0273AD10 cXyz* npc_allocatePoint(u32 size)
*/
// clang-format on
