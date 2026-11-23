/** Process execution queues. */
#include "f_pc/f_pc_executor.h"
#include "true60.h"
#include "f_pc/f_pc_t60_exec_proc.h"
#include "d/d_com_inf_game.h"
#include "f_pc/f_pc_t60.h"
#define WWHD_T60_FRAME_START_IMPL /* the process-order read rule: the one definition of the frame-start table */
#include "f_pc/f_pc_t60_frame_start.h"
#define WWHD_T60_PICTURE_IMPL /* the one definition of the picture table (WWHD_T60_PIC_RESET) */
#include "f_pc/f_pc_t60_picture.h"
#define WWHD_T60_CULL_IMPL /* the first-step AI-timer rule extended: the one definition of the frame-end view copy */
#include "f_op/f_op_actor_mng_t60_cull.h"
#define WWHD_T60_FRAME_LATCH_IMPL /* the process-order read rule addendum: the one definition of the frame-start latch */
#include "f_pc/f_pc_t60_frame_latch.h"
#include "d/d_camera_follow_whole_t60.h" /* the follow camera's tick30 frame-start restore (declaration only here) */
#define WWHD_T60_PY_IMPL /* the process-order read rule addendum 2: the one definition of the Link reset-flag frame-OR */
#include "d/t60_py.h" /* true60 (the process-order read rule addendum): Link status frame latch */
#include "f_pc/f_pc_t60_pystat.h" /* true60 (the process-order read rule): the player status words' frame start for readers before Link */
static s32 g_fpcNd_type;
static ExecutorHeaderStatics l_headerStatics;
static const ExecutorPiPair l_piPair;

/* 025DE508 */
base_process_class* fpcEx_Search(u32 i_judgeFunc, void* i_data) {
    return (base_process_class*)fpcLyIt_AllJudge(i_judgeFunc, i_data);
}

/* 025DE50C */
base_process_class* fpcEx_SearchByID(u32 i_id) {
    // HD: retain the guest key write even for the two rejected process IDs.
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::Local<be<u32>> key;
    *key = i_id;
    if (i_id == 0xFFFFFFFE || i_id == fpcM_ERROR_PROCESS_ID_e) {
        return NULL;
    }
    return fpcEx_Search(fpcSch_JudgeByID, key.get());
}

/* 025DE564 */
BOOL fpcEx_IsExist(u32 i_id) {
    return fpcEx_SearchByID(i_id) != NULL;
}

/* T60_EXEC_LOGIC: f_pc/f_pc_t60_exec_proc.h (true60 test aid, the strict RNG-pair rule) */

/* 025DE58C */
BOOL fpcEx_Execute(base_process_class* i_proc) {
    if (i_proc->mInitState != 2 || fpcPause_IsEnable(i_proc, 1) == 1) {
        return FALSE;
    }
    T60_EXEC_LOGIC(i_proc);
#if defined(GABI_GAME_BUILD) && GABI_T60
    if (wwhd_t60_pic_window()) /* debug check (WWHD_T60_PIC_RESET): no execute inside the draw window */
        wwhd_t60_pic_warn((int)i_proc->mProfName);
#endif
    if (t60::steps() == 1) {
        return fpcBs_Execute(i_proc);
    }
    t60_pystat_line(t60::steps()); /* true60 (f_pc_t60_pystat.h): the step's line starts with the frame-start status words */
    t60_cam_tick30_start(); /* true60 (d_camera_follow_whole_t60.h): the tick30 step's first execute puts the follow camera's frame start back */
    t60_flatch_exec(i_proc->mProfName); /* true60 (f_pc_t60_frame_latch.h): the reader's first-step turn copies the words */
    // true60: the per-process rate gate. A converted process (true60.h kT60Converted) executes on every step with
    // g_dt = 1/N; any other process only on the tick30 step, with g_dt = 1 (drawn interpolated in between).
    // (the first-step frame-level rule, f_pc_t60.h: with WWHD_T60_SCENE_FIRST the play scene runs on the frame's first step, dt 1)
    if (t60_frame_work_proc(i_proc->mProfName)) {
#if defined(GABI_GAME_BUILD) && GABI_T60
        wwhd_t60_ran = t60::first_step() ? 1.0f : 0.0f;
#endif
        if (!t60::first_step())
            return FALSE;
        u32 t60Tick = t60_frame_work_begin();
        f32 previousDt = t60_set_dt(1.0f);
        t60_cull_exec(true); /* true60 (f_op_actor_mng_t60_cull.h): an execute; the frame's first copies the view */
        BOOL ret = fpcBs_Execute(i_proc);
        t60_cull_exec(false);
        t60_set_dt(previousDt);
        t60_frame_work_end(t60Tick);
        return ret;
    }
    // (a converted process coupled to an unconverted carrier this frame runs as an unconverted one: t60_held)
    bool converted = t60_converted(i_proc->mProfName) && !t60_held(i_proc->mBsPcId);
    // (rule 14, carrier-bg pair: the camera follows Link; while Link is held on an unconverted carrier the
    // converted camera chased a target that moves on the tick30 step only and lagged it (eye 2-10 units). The camera
    // (0x1DC) is held with Link: both run on the tick30 step at dt 1 then, as at 30 Hz.)
    if (converted && (s16)i_proc->mProfName == 0x1DC) {
        const u32 player = gabi::load<u32>(dComIfGp_ea() + PLAY_OFF(mpPlayerPtr[0]));
        if (player != 0 && t60_held(gabi::load<u32>(player + 4) /* base.mBsPcId */)) {
            converted = false;
        }
    }
    // (rule 14, 2026-11-17: the hookshot (0xA9) is coupled to Link: his procHookshotFly adds its pull
    // vector on each of his steps and he sets its mode; it runs on every step only while Link does, else as at 30 Hz)
    if (converted && (s16)i_proc->mProfName == 0xA9) {
        const u32 player = gabi::load<u32>(dComIfGp_ea() + PLAY_OFF(mpPlayerPtr[0]));
        if (player == 0 || !t60_converted(gabi::load<s16>(player + 0x0E) /* base.mProfName */) ||
            t60_held(gabi::load<u32>(player + 4) /* base.mBsPcId */)) {
            converted = false;
        }
    }
    f32 dt = converted ? t60_step_dt() : 1.0f;
#if defined(GABI_GAME_BUILD) && GABI_T60
    // (the process-order read rule, f_pc_t60_frame_start.h: a converted actor's frame-start eye / position for readers before it)
    wwhd_t60_fs_exec(gabi::ea(i_proc), converted,
                     converted && t60::first_step() && gabi::call<BOOL>(0x025D4604 /* fopAc_IsActor */, i_proc) != 0);
    wwhd_t60_ran = !converted && !t60::tick30() ? 0.0f : dt;
#endif
    if (!converted && !t60::tick30()) {
        return FALSE;
    }
    f32 previousDt = t60_set_dt(dt);
    t60_cull_exec(true); /* true60 (f_op_actor_mng_t60_cull.h): an execute; the frame's first copies the view */
    /* true60 (the process-order read rule addendum 2, d/t60_py.h): a tick30-only actor reads Link's reset flags as his whole frame */
    const bool t60PySwap = !converted && (s16)i_proc->mProfName != 0xA8;
    if (t60PySwap)
        t60py_swap_begin();
    if ((s16)i_proc->mProfName == 0xA8)
        t60_pystat_link(); /* true60 (f_pc_t60_pystat.h): Link's execute sees his live status words */
    BOOL ret = fpcBs_Execute(i_proc);
    if (t60PySwap)
        t60py_swap_end();
    t60_cull_exec(false);
    if ((s16)i_proc->mProfName == 0xA8) /* true60 (the process-order read rule addendum, d/t60_py.h): Link's one-step status bits */
        t60py::note(gabi::ea(i_proc));
    t60_set_dt(previousDt);
    return ret;
}

#if defined(GABI_GAME_BUILD) && GABI_T60
/* true60, game build only (no guest function): whether a process with this profile name is converted (the rate gate's
 * table, true60.h kT60Converted, plus the test override). The port's draw alignment asks it for the process whose draw
 * runs (runtime/src/: converted processes are drawn at the interpolated world's phase). */
extern "C" __attribute__((visibility("default"))) int wwhd_t60_proc_converted(int profName) {
    return t60_converted(profName) ? 1 : 0;
}
#endif

/* 025DE5F4 */
s32 fpcEx_ToLineQ(base_process_class* i_proc) {
    layer_class* pLayer = i_proc->mLyTg.mpLayer;
    process_node_class* pLayerPcNode = pLayer->mpPcNode;
    if (pLayer->mLayerID != 0 && cTg_IsUse(fpcEx_getLineTag(&pLayerPcNode->base)) != TRUE) {
        return 0;
    }
    u16 listID = fpcEx_getPriority(i_proc)->mInfoCurr.mListID;
    if (fpcLnTg_ToQueue(fpcEx_getLineTag(i_proc), listID) == 0) {
        fpcLyTg_QueueTo(&i_proc->mLyTg);
        return 0;
    }
    // HD: read subtype before changing initialization state.
    u32 subtype = ((process_node_class*)i_proc)->mSubType;
    i_proc->mInitState = 2;
    if (fpcBs_Is_JustOfType(g_fpcNd_type, subtype) != 0) {
        process_node_class* pNode = (process_node_class*)i_proc;
        fpcLyIt_OnlyHere(&pNode->mLayer, gabi::guest_fn((const void*)fpcEx_ToLineQ), i_proc);
    }
    return 1;
}

/* 025DE6C4 */
s32 fpcEx_ExecuteQTo(base_process_class* i_proc) {
    if (fpcLyTg_QueueTo(&i_proc->mLyTg) != TRUE) {
        return 0;
    }
    i_proc->mInitState = 3;
    return 1;
}

/* 025DE720 */
s32 fpcEx_ToExecuteQ(base_process_class* i_proc) {
    process_priority_queue_info* info = &fpcEx_getPriority(i_proc)->mInfoCurr;
    // HD: keep the verified list, priority, layer read order.
    u16 listID = info->mListID;
    u16 priority = info->mListPrio;
    u32 layer = info->mLayer;
    if (fpcLyTg_ToQueue(&i_proc->mLyTg, layer, listID, priority) != TRUE) {
        return 0;
    }
    fpcEx_ToLineQ(i_proc);
    return 1;
}

/* 025DE788 */
void fpcEx_Handler(u32 i_queueFunc) {
    fpcLnIt_Queue(i_queueFunc);
}

/* 025DE78C */
static void __sinit_f_pc_executor_cpp() {
    // HD: preserve header objects, constant copies and registration order.
    l_headerStatics.zero[2] = 0;
    l_headerStatics.zero[0] = 0;
    l_headerStatics.zero[3] = 0;
    l_headerStatics.zero[1] = 0;
    __register_global_object(0x101F3A94);
    l_headerStatics.negativePi = l_piPair.negativePi;
    l_headerStatics.positivePi = l_piPair.positivePi;
    ExecutorFirstHeaderObject_ct(&l_headerStatics.firstObject);
    __register_global_object(0x101F3AA0);
    ExecutorSecondHeaderObject_ct(&l_headerStatics.secondObject);
    __register_global_object(0x101F3AAC);
}

/* WWHD glue (tools/verify/autoglue.py): this translation unit's data
data g_fpcNd_type 101F3D60
data l_headerStatics 1048A7B8
data l_piPair 100582DC
ext 025DEE58 void* fpcLyIt_AllJudge(u32, void*)
ext 025E0B20 s32 fpcPause_IsEnable(base_process_class*, u32)
ext 025DD2B8 s32 fpcBs_Execute(base_process_class*)
ext 0201A71C s32 cTg_IsUse(create_tag_class*)
ext 025DF6C8 s32 fpcLnTg_ToQueue(create_tag_class*, u16)
ext 025DF084 s32 fpcLyTg_QueueTo(layer_management_tag_class*)
ext 025DEF68 s32 fpcLyTg_ToQueue(layer_management_tag_class*, u32, u16, u16)
ext 025DD258 BOOL fpcBs_Is_JustOfType(int, int)
ext 025DED70 s32 fpcLyIt_OnlyHere(layer_class*, u32, void*)
ext 025DF5C0 void fpcLnIt_Queue(u32)
ext 028ED6F8 void ExecutorFirstHeaderObject_ct(u8*)
ext 028EAB2C void ExecutorSecondHeaderObject_ct(u8*)
*/
