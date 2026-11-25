#pragma once
/* true60 (2026-11-24; d_a_player_main_04.cpp execute, the held branch).
 * Separate file: execute keeps one thin call.
 *
 * procShipSteer / procShipPaddle put Link on the ship every update: changeShipEndProc, then setShipRidePos(1) (current.pos =
 * the ship body's base matrix x l_ship_offset). On the frame the sail goes up (or down) the first step enters the other
 * procedure (procShipPaddle -> procShipSteer_init, its own setShipRidePos(1) at the first step's ship matrix) and the held-body
 * form (s_lkT60ProcHold) holds the new body over the frame's later steps: Link stayed where the first step put him while
 * the ship moved on (pair, N 2, frame 46: B y -60.796, A -62.617; the next frame lands on A again). A: the init's
 * setShipRidePos at the frame's ship matrix. Form: a held later step of SHIP_STEER (0x88) / SHIP_PADDLE (0x89) runs the
 * body's ride placement, setShipRidePos(1), as on every other frame's steps; no other part of the body. dt 1 (N = 1):
 * nothing is held, never called. Negative control WWHD_T60_LK_SHIPRIDE_HOLD_NEGCTRL=1 (game build): the held step keeps
 * the first step's place. */
#include <cstdlib>

/* the held branch (the body skipped on this later step): L = daPy_lk_c */
template <class L> static inline void lkT60ShipRide_held(L* lk) {
    static const bool negctrl = [] {
        const char* e = getenv("WWHD_T60_LK_SHIPRIDE_HOLD_NEGCTRL");
        return e && e[0] == '1';
    }();
    const int proc = (int)lk->mCurProc;
    /* (part 3, 2026-11-25; long3 seg 4 / 5, N 2: the drop onto the ship by
     * LINK_WARP, SHIP_JUMP_RIDE entered on the first step: its init's setShipRidePos(0) at the first step's ship matrix
     * held over the frame, y -188.572 for A's -187.035, the ship's frame-end place) SHIP_JUMP_RIDE (0x87) the same,
     * with its body's placement setShipRidePos(0). NC WWHD_T60_SHIPJUMPRIDE_HOLD_NEGCTRL=1. */
    static const bool jumpNc = [] {
        const char* e = getenv("WWHD_T60_SHIPJUMPRIDE_HOLD_NEGCTRL");
        return e && e[0] == '1';
    }();
    if (proc == 0x87 && !jumpNc) { /* daPyProc_SHIP_JUMP_RIDE_e */
        lk->setShipRidePos(0);
        return;
    }
    /* (2026-11-25; long3 seg 5, N 2, frame 4019: SHIP_CANNON entered on the
     * first step kept its init's setShipRidePos(1) at the first step's ship matrix over the frame, Link 49.7 behind the
     * sailing ship for that frame) SHIP_CANNON (0x8E) the same: its body's placement setShipRidePos(1) (procShipCannon,
     * when it does not end the ride). NC WWHD_T60_SHIPCANNON_HOLD_NEGCTRL=1. */
    static const bool cannonNc = [] {
        const char* e = getenv("WWHD_T60_SHIPCANNON_HOLD_NEGCTRL");
        return e && e[0] == '1';
    }();
    if (proc == 0x8E && !cannonNc) { /* daPyProc_SHIP_CANNON_e */
        lk->setShipRidePos(1);
        return;
    }
    if (negctrl || (proc != 0x88 && proc != 0x89)) /* daPyProc_SHIP_STEER_e, daPyProc_SHIP_PADDLE_e */
        return;
    lk->setShipRidePos(1);
}
