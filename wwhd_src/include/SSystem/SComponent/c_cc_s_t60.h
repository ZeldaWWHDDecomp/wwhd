#pragma once
/* true60 (decomp build only, from a camera collision finding).
 * Separate file: c_cc_s.cpp keeps two thin calls in cCcSState::Move.
 *
 * cCcSState::Move runs on every logic step (d_s_play_8: the collider lists hold what the step's executes registered).
 * Its CalcArea sets the shared divide area (mDivideArea) from the listed colliders and every listed collider's divide
 * bits (mDivideInfo) against it. At N > 1 an earlier step lists only the converted colliders (Link), so its CalcArea
 * replaced the area with their small box, while the unconverted colliders kept the bits of the last tick30 Move (made
 * against the full area). A reader between Moves that compares divide bits against mDivideArea (dCcS::ChkCamera: the
 * talk camera and event cameras 4 / 6 / 8 / 11, CalcDivideInfoOverArea(&mDivideArea)) then compared bits from two
 * different areas and skipped colliders (ewh4: the moored ship 0xA5 missed, the bait-throw camera chose another eye).
 *
 * Form (step-gated): on a step that is not the tick30 step, Move saves mDivideArea before CalcArea, runs the step's
 * checks with the step's own area as before (the converted pairs' hits are unchanged), then puts the saved area back
 * and recomputes the step's listed colliders' bits against it (CalcArea's own call, the shapes' boxes of this step), so
 * the area and every collider's bits stay those of the last tick30 Move (unconverted) / against its area (converted).
 * N = 1 and the tick30 step: never consulted beyond the step-state read (dt-1 check: steps 1, tick30 1: unchanged).
 * Negative control: CCST60_AREA_NEGCTRL (the per-step area). */
#include "true60.h"

static inline bool ccsT60_keepArea() {
#ifdef CCST60_AREA_NEGCTRL
    return false;
#else
    return t60::steps() > 1 && !t60::tick30();
#endif
}

/* host state of the running Move (Move is not re-entered); host static, not a local: the verify build places a
 * function's locals in guest memory, which host code may only reach through be<> views */
struct CcsT60AreaKeep {
    bool kept;
    uint32_t area[sizeof(cCcD_DivideArea) / 4]; /* the area's words (guest big-endian values, copied as they are) */
};
static_assert(sizeof(cCcD_DivideArea) % 4 == 0, "divide area words");
static CcsT60AreaKeep s_ccsT60Area;

/* Move's head, before CalcArea */
static inline void ccsT60_saveArea(cCcD_DivideArea* a) {
    CcsT60AreaKeep& k = s_ccsT60Area;
    k.kept = ccsT60_keepArea();
    if (!k.kept)
        return;
    const uint32_t ea = gabi::ea(a);
    for (uint32_t i = 0; i < sizeof(cCcD_DivideArea) / 4; i++)
        k.area[i] = gabi::load<uint32_t>(ea + 4 * i);
}

/* Move's checks are done (before the lists are emptied): the saved area back, the listed colliders' bits against it.
 * S: cCcSState; shapeOf: c_cc_s.cpp's getShapeAttr. */
template <class S, class F> static inline void ccsT60_restoreArea(S* s, F shapeOf) {
    CcsT60AreaKeep& k = s_ccsT60Area;
    if (!k.kept)
        return;
    k.kept = false;
    const uint32_t ea = gabi::ea(&s->mDivideArea);
    for (uint32_t i = 0; i < sizeof(cCcD_DivideArea) / 4; i++)
        gabi::store<uint32_t>(ea + 4 * i, k.area[i]);
    auto* end = s->mpObj + (u32)s->mObjCount;
    for (auto* pObj = s->mpObj; pObj < end; ++pObj) {
        cCcD_Obj* obj = pObj->get();
        if (!obj)
            continue;
        cCcD_ShapeAttr* shape = shapeOf(obj);
        if (shape) {
            obj = pObj->get();
            u32 flag = (u32)obj->mFlags & cCcD_BsRevHit_e;
            cCcD_DivideArea_CalcDivideInfo(&s->mDivideArea, &obj->mDivideInfo, &shape->mAab, flag);
        }
        end = s->mpObj + (u32)s->mObjCount;
    }
}
