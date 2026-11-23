#pragma once
/* true60 test aid (the strict RNG-pair rule, port runtime/src/ WWHD_TEST_RNG_KEYED): the process whose execute,
 * create or draw method runs (its guest address: the process ids differ between the two runs of a pair, which create a
 * different number of processes while loading; the actors' heap addresses match, pair). Port-owned; set around
 * the method in the game build only (no guest state, nothing read by game code). Create and draw are keyed by their own
 * process too (q6bk): outside an execute every draw shared key 0, whose draw index then depended on how many draws the
 * draw phase made per 30 Hz frame (N steps at 60 Hz) and on every other process created before. */
#include "f_pc/f_pc_base.h"
#include "gabi_t60.h"
#if defined(GABI_GAME_BUILD) && GABI_T60
extern "C" __attribute__((visibility("default"))) uint32_t wwhd_t60_exec_proc;
/* 1 inside an execute (fpcEx_Execute) and what it calls (a create inside it too), 0 in
 * a draw method / a create outside an execute: the keyed test RNG counts a hold pass's draws on counters of their own
 * only outside an execute, since a converted process's execute on a step before the tick30 step (an
 * in-between pass that runs logic) is logic, not a draw-method replay (pair: the Yboil's pos_reset on its first
 * step drew from the hold counters, A from the logic ones: every reset bubble at another place) */
extern "C" __attribute__((visibility("default"))) uint32_t wwhd_t60_exec_logic;
struct T60ExecProc {
    uint32_t previous, previousLogic;
    explicit T60ExecProc(base_process_class* p, bool logic = false)
        : previous(wwhd_t60_exec_proc), previousLogic(wwhd_t60_exec_logic) {
        wwhd_t60_exec_proc = gabi::ea(p);
        if (logic) wwhd_t60_exec_logic = 1;
    }
    ~T60ExecProc() { wwhd_t60_exec_proc = previous; wwhd_t60_exec_logic = previousLogic; }
};
#define T60_EXEC_PROC(p) T60ExecProc t60ExecProc(p)
#define T60_EXEC_LOGIC(p) T60ExecProc t60ExecProc(p, true)
#else
#define T60_EXEC_PROC(p) ((void)0)
#define T60_EXEC_LOGIC(p) ((void)0)
#endif
